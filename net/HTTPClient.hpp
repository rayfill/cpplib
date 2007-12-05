#include <Socket/ClientSocket.hpp>
#include <net/HTTPProperty.hpp>
#include <net/URL.hpp>
#include <net/HTTPResult.hpp>
#include <net/HTTPException.hpp>
#include <text/LexicalCast.hpp>
#include <util/format/Base64.hpp>
#include <algorithm>

template <typename HttpResultType = HTTPResult<> >
class HTTPClient
{
	friend class HTTPClientTest;

public:
	typedef HttpResultType http_result_t;

private:
	ClientSocket socket;
	HTTPProperty property;

	/**
	 * 現在接続しているサーバの情報。ダイレクト接続時にはそのサーバ、
	 * proxy接続時にはproxyの情報が入る。
	 */
	std::string currentConnectionServer;
	unsigned short currentConnectionPort;

	/**
	 * 使用するproxyサーバ情報
	 */
	std::string proxyServerName;
	unsigned short proxyServerPort;

	/**
	 * keepaliveを使うかのフラグ
	 */
	bool isKeepAlive;

	void connect(const char* serverName, unsigned short portNumber)
	{
		// set option.
		setHostName(serverName);
		currentConnectionPort = portNumber;

		// open TCP connection.
		socket.connect(IP(serverName, portNumber));
	}

	void connectWithProxy(const char* serverName,
						  unsigned short portNumber,
						  const char* proxyName,
						  unsigned short proxyPort)
	{
		setHostName(serverName, portNumber);
		currentConnectionServer = proxyName; currentConnectionPort = proxyPort;

		socket.connect(IP(proxyName, proxyPort));
	}

	bool isUseProxy() const
	{
		return proxyServerName != "";
	}

	bool isContinuedRequest(const URL& url)
	{
		const std::string connectServerName =
			isUseProxy() ? proxyServerName : url.getServerName();
		const unsigned short connectPort = 
			isUseProxy() ? proxyServerPort : 
			(url.getPortNumber() == 0 ? 80 : url.getPortNumber());

		if (currentConnectionServer != connectServerName ||
			currentConnectionPort != connectPort)
			return false;

		return true;
	}

	bool isDisconnect(const http_result_t& result) const
	{
		if (isKeepAlive == false ||
			result.getResponseHeaders().get("Connection") == "close" ||
			result.getResponseHeaders().get("Proxy-Connection") == "close")
			return true;

		return false;
	}

	http_result_t processHEADResponse()
	{
		http_result_t result;
		result.readHeadResponse(socket);
		if (isDisconnect(result))
			close();

		return result;
	}

	http_result_t processGETResponse()
	{
		http_result_t result;
		result.readGetResponse(socket);
		if (isDisconnect(result))
			close();

		return result;
	}

	void sendCommand(const std::string& command)
	{
		socket.write(command.c_str(), command.length());
	}

	std::string getHEADCommand(const URL& url) const
	{
		return std::string(std::string("HEAD ") +
						   url.getResourcePath() +
						   " HTTP/1.1\r\n");
	}

	std::string getGETCommand(const URL& url) const
	{
		const std::string resourcePath = isUseProxy() ?
			url.toString() : url.getResourcePath();
		
		return std::string(std::string("GET ") +
						   resourcePath +
						   " HTTP/1.1\r\n");
	}

	void setHostName(const char* hostName)
	{
		property.setHostName(hostName);
		currentConnectionServer = hostName;
	}

	void setHostName(const char* hostName, unsigned short portNumber)
	{
		property.setHostName((std::string(hostName) + ":" +
							  stringCast<unsigned short>(portNumber)).c_str());
		currentConnectionServer = hostName;
	}

	/**
	 * @todo Socket基底のread/writeから処理バイト数をとり、切断例外を
	 * 処理、および状態として保持して本関数内で再接続の試行が必要
	 * proxyも要考慮
	 */
	void connectTarget(const URL& url)
	{
		if (isKeepAlive == false ||
			(!socket.isConnected()))
		{
			if (currentConnectionServer != "" &&
				!isContinuedRequest(url))
				throw InvalidConnectRequestException(url.getServerName(),
													 url.getPortNumber());
				
			// open connection
			if (proxyServerName == "")
				connect(url.getServerName().c_str(),
						url.getPortNumber() == 0 ? 80 : url.getPortNumber());
			else
				connectWithProxy(
					url.getServerName().c_str(),
					url.getPortNumber() == 0 ? 80 : url.getPortNumber(),
					proxyServerName.c_str(),
					proxyServerPort == 0 ? 80 : proxyServerPort);
		}
	}

public:
	HTTPClient():
		socket(),
		property(),
		currentConnectionServer(""),
		currentConnectionPort(),
		proxyServerName(),
		proxyServerPort(),
		isKeepAlive(true)
	{
		property.isConnectionKeeping(isKeepAlive);
		property.setUserAgent("C++ library Web auto crawler/1.0");
	}

	virtual ~HTTPClient()
	{}

	void setProxy(const std::string& proxyName, unsigned short proxyPort)
	{
		property.isProxyConnectionKeeping(isKeepAlive);
		proxyServerName = proxyName;
		proxyServerPort = proxyPort == 0 ? 80 : proxyPort;
	}

	void clearProxy()
	{
		property.isConnectionKeeping(isKeepAlive);
		proxyServerName = "";
		proxyServerPort = 0;
	}
	
	void setProxyAuthorization(const char* username,
							   const char* password)
	{
		std::string user_pass = std::string(username) + ":" + password;
		std::vector<char> vec(user_pass.begin(), user_pass.end());
		user_pass = Base64::encode(vec);

		property.setProxyAuthorization(user_pass);
	}

	void clearProxyAuthorization()
	{
		property.clearProxyAuthorization();
	}

	std::string getProxyServerName() const
	{
		return proxyServerName;
	}

	unsigned short getProxyPort() const
	{
		return proxyServerPort;
	}

	void setKeepAlive(bool keepAlive)
	{
		isKeepAlive = keepAlive;
		property.isConnectionKeeping(isKeepAlive);
	}

	void addCookie(const char* cookie)
	{
		assert(cookie != NULL);

		property.addCookie(cookie);
	}

	void setAcceptEncoding(const char* encoding)
	{
		assert(encoding != NULL);

		property.setAcceptEncoding(encoding);
	}

	void setKeepAliveTime(const int time)
	{
		property.setKeepAliveTime(stringCast<int>(time).c_str());
	}

	void setUserAgent(const char* newAgent)
	{
		assert(newAgent != NULL);

		property.setUserAgent(newAgent);
	}

	void addAcceptLanguage(const char* lang)
	{
		property.addAcceptLanguage(lang);
	}

	void setRange(const unsigned int first)
	{
		property.setRange(first);
	}

	void setRange(const unsigned int first, const unsigned int last)
	{
		property.setRange(first, last);
	}

	/**
	 * @todo ソケットクローズのスマート化。HTTPResultの方に移して
	 * HTTP headerのConection: closeを見るようにする。
	 */
	http_result_t getHeaderResponse(const URL& url)
	{
		connectTarget(url);

		// send GET command.
		sendCommand(getHEADCommand(url));

		// send HTTP option parameter.
		sendCommand(property.toString());

		// send HTTP request terminater.
		sendCommand(std::string("\r\n"));

		return processHEADResponse();
	}

	/**
	 * @todo ソケットクローズのスマート化。HTTPResultの方に移して
	 * HTTP headerのConection: closeを見るようにする。
	 */
	http_result_t getResource(const URL& url)
	{
		connectTarget(url);

		// send GET command.
		sendCommand(getGETCommand(url));

		// send HTTP option parameter.
		sendCommand(property.toString());

		// send HTTP request terminater.
		sendCommand(std::string("\r\n"));

		return processGETResponse();
	}

	http_result_t getHeaderResponse(const char* url)
	{
		return getHeaderResponse(URL(url));
	}

	http_result_t getResource(const char* url)
	{
		return getResource(URL(url));
	}

	void setTimeout(const long sec, const long usec = 0)
	{
		socket.setTimeout(sec, usec);
	}

	void close()
	{
		currentConnectionServer = "";
		currentConnectionPort = 0;
		proxyServerName = "";
		proxyServerPort = 0;
		socket.close();
		property = HTTPProperty();
	}
};

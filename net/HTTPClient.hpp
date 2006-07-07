#include <Socket/ClientSocket.hpp>
#include <net/HTTPProperty.hpp>
#include <net/URL.hpp>
#include <net/HTTPResult.hpp>
#include <text/LexicalCast.hpp>

class HTTPClient
{
	friend class HTTPClientTest;

private:
	ClientSocket socket;
	HTTPProperty property;
	std::string currentConnectionServer;
	unsigned short currentConnectionPort;
	bool isKeepAlive;

	void connect(const char* serverName, unsigned short portNumber)
		throw (SocketException, NotAddressResolvException)
	{
		// set option.
		setHostName(serverName);
		currentConnectionPort = portNumber;

		// open TCP connection.
		socket.connect(IP(serverName, portNumber));
	}

	HTTPResult<> processHEADResponse()
	{
		HTTPResult<> result;
		result.readHeadResponse(socket);

		return result;
	}

	HTTPResult<> processGETResponse()
	{
		HTTPResult<> result;
		result.readGetResponse(socket);

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
		return std::string(std::string("GET ") +
						   url.getResourcePath() +
						   " HTTP/1.1\r\n");
	}

	void setHostName(const char* hostName)
	{
		property.setHostName(hostName);
		currentConnectionServer = hostName;
	}

	/**
	 * @todo Socket基底のread/writeから処理バイト数をとり、切断例外を
	 * 処理、および状態として保持して本関数内で再接続の試行が必要
	 */
	void connectTarget(const URL& url)
	{
		if (isKeepAlive == false ||
			(!socket.isConnected() ||
			 currentConnectionServer != url.getServerName() ||
			 currentConnectionPort != 
			 (url.getPortNumber() == 0 ? 80 : url.getPortNumber())))
		{
			// open connection
			connect(url.getServerName().c_str(),
					url.getPortNumber() == 0 ? 80 : url.getPortNumber());
		}
	}

public:
	HTTPClient():
		socket(),
		property(),
		currentConnectionServer(""),
		currentConnectionPort(),
		isKeepAlive(true)
	{
		property.isConnectionKeeping(isKeepAlive);
		property.setUserAgent("C++ library Web auto crawler/1.0");
	}

	virtual ~HTTPClient()
	{
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
	HTTPResult<> getHeaderResponse(const URL& url)
	{
		connectTarget(url);

		// send GET command.
		sendCommand(getHEADCommand(url));

		// send HTTP option parameter.
		sendCommand(property.toString());

		// send HTTP request terminater.
		sendCommand(std::string("\r\n"));

		HTTPResult<> result = processHEADResponse();

		if (isKeepAlive == false && socket.isConnected() == false)
			socket.close();

		return result;
	}

	/**
	 * @todo ソケットクローズのスマート化。HTTPResultの方に移して
	 * HTTP headerのConection: closeを見るようにする。
	 */
	HTTPResult<> getResource(const URL& url)
	{
		connectTarget(url);

		// send GET command.
		sendCommand(getGETCommand(url));

		// send HTTP option parameter.
		sendCommand(property.toString());

		// send HTTP request terminater.
		sendCommand(std::string("\r\n"));

		HTTPResult<> result = processGETResponse();
		if (isKeepAlive == false && socket.isConnected() == false)
			socket.close();

		return result;
	}

	HTTPResult<> getHeaderResponse(const char* url)
	{
		return getHeaderResponse(URL(url));
	}

	HTTPResult<> getResource(const char* url)
	{
		return getResource(URL(url));
	}

	void setTimeout(const long sec, const long usec = 0)
	{
		socket.setTimeout(sec, usec);
	}

};

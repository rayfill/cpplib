#include <string>
#include <stdexcept>
#include <text/LexicalCast.hpp>

class URLParseError : public std::runtime_error
{
public:
	URLParseError(const std::string& reason = "URL parsing error."):
		std::runtime_error(reason)
	{}

	virtual ~URLParseError() throw() {}

	virtual const char* what() const throw()
	{
		return std::runtime_error::what();
	}
};

class URL
{
	friend class URLTest;

private:
	std::string scheme;
	std::string urlString;
	std::string server;
	std::string resource;
	unsigned short port;

	bool parseURL(const std::string& url)
	{
		// scheme
		std::string::size_type schemeTermPos = url.find("://");
		if (schemeTermPos == std::string::npos)
			return false;
		schemeTermPos += 2; // offset with "//" mark.

		// server with port
		std::string::size_type serverWithPortTermPos = 
			url.find("/", schemeTermPos + 1);
		if (serverWithPortTermPos == std::string::npos)
			return false;

		std::string::size_type portSeparatorPos = 
			url.find(":", schemeTermPos + 1);

		const bool isWritePortNumber = 
			(portSeparatorPos != std::string::npos);

		scheme = url.substr(0, schemeTermPos + 1);
		server = isWritePortNumber ?
			url.substr(schemeTermPos + 1, 
					   portSeparatorPos - schemeTermPos - 1) :
			url.substr(schemeTermPos + 1, 
					   serverWithPortTermPos - schemeTermPos - 1);

		port = isWritePortNumber ?
			lexicalCast<unsigned short>(
					url.substr(portSeparatorPos + 1,
							   serverWithPortTermPos - portSeparatorPos - 1)) :
			0;

		resource = url.substr(serverWithPortTermPos);

		urlString = url;

		return true;
	}

public:
	URL(const std::string& url):
		scheme(),
		urlString(),
		server(),
		resource(),
		port()
	{
		if (!parseURL(url))
			throw URLParseError(url + " is not valid url.");
	}

	URL(const URL& rhs):
		scheme(rhs.getScheme()),
		urlString(rhs.toString()),
		server(rhs.getServerName()),
		resource(rhs.getResourcePath()),
		port(rhs.getPortNumber())
	{}

	URL& operator=(const URL& rhs)
	{
		scheme = rhs.getScheme();
		server = rhs.getServerName();
		port = rhs.getPortNumber();
		resource = rhs.getResourcePath();
		urlString = rhs.toString();

		return *this;
	}

	const std::string& getScheme() const
	{
		return scheme;
	}

	const std::string& getServerName() const
	{
		return server;
	}
	
	const std::string& getResourcePath() const
	{
		return resource;
	}

	const unsigned short getPortNumber() const
	{
		return port;
	}

	const std::string& toString() const
	{
		return urlString;
	}
};

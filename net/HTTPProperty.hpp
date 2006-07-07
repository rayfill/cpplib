#include <util/Property.hpp>
#include <text/LexicalCast.hpp>

class HTTPProperty
{
private:
	Property headerProperties;
	Property properties;

public:
	HTTPProperty():
		headerProperties(),
		properties()
	{
	}

	virtual ~HTTPProperty()
	{}

	void setHostName(const char* hostName)
	{
		headerProperties.set("Host", hostName);
	}

	void addCookie(const char* cookie)
	{
		std::string current = properties.get("Cookie");
		if (current.length() != 0)
			current += "; ";

		current += cookie;

		properties.set("Cookie", current);
	}

	void isConnectionKeeping(bool isKeepAlive)
	{
		if (isKeepAlive)
			headerProperties.set("Connection", "Keep-Alive");
		else
			headerProperties.set("connection", "close");
	}

	void addAcceptLanguage(const char* lang)
	{
		std::string current = properties.get("Accept-Language");

		if (current.length() != 0)
			current += ", ";

		current += lang;

		properties.set("Accept-Language", current);
	}

	void setAcceptEncoding(const char* encoding)
	{
		properties.set("Accept-Encoding", encoding);
	}

	void setKeepAliveTime(const char* time)
	{
		properties.set("Keep-Alive", time);
	}

	void setUserAgent(const char* userAgent)
	{
		properties.set("User-Agent", userAgent);
	}

	void setReferer(const char* referer)
	{
		properties.set("Referer", referer);
	}

	void setRange(const unsigned int first)
	{
		properties.set("Range", "bytes=" +
					   stringCast<const unsigned int>(first) + "-");
	}

	void setRange(const unsigned int first, const unsigned int last)
	{
		properties.set("range", "bytes=" +
					   stringCast<const unsigned int>(first) +
					   "-" + stringCast<const unsigned int>(last));
	}

	std::string toString()
	{
		return headerProperties.toString() + properties.toString();
	}

	HTTPProperty& operator=(const HTTPProperty& rhs)
	{
		this->headerProperties = rhs.headerProperties;
		this->properties = rhs.properties;

		return *this;
	}
};

#ifndef HTTPPROPERTY_HPP_
#define HTTPPROPERTY_HPP_

#include <util/Property.hpp>
#include <text/LexicalCast.hpp>
#include <util/Predicate.hpp>
#include <util/format/Base64.hpp>
#include <cstring>

class HTTPProperty
{
	Property<Predicate::IgnoreCaseComparator> headerProperties;
	Property<Predicate::IgnoreCaseComparator> properties;

public:
	HTTPProperty():
		headerProperties(),
		properties()
	{}

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

	void isProxyConnectionKeeping(bool isKeepAlive)
	{
		headerProperties.erase("Connection");
		
		if (isKeepAlive)
			headerProperties.set("Proxy-Connection", "Keep-Alive");
		else
			headerProperties.set("Proxy-Connection", "close");
		
	}

	void isConnectionKeeping(bool isKeepAlive)
	{
		headerProperties.erase("Proxy-Connection");

		if (isKeepAlive)
			headerProperties.set("Connection", "Keep-Alive");
		else
			headerProperties.set("Connection", "close");
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

	void clearUserAgent()
	{
		properties.erase("User-Agent");
	}

	void setReferer(const char* referer)
	{
		properties.set("Referer", referer);
	}

	void clearReferer()
	{
		properties.erase("Referer");
	}

	void setRange(const unsigned int first)
	{
		properties.set("Range", "bytes=" +
					   stringCast<unsigned int>(first) + "-");
	}

	void setRange(const unsigned int first, const unsigned int last)
	{
		properties.set("Range", "bytes=" +
					   stringCast<unsigned int>(first) +
					   "-" + stringCast<unsigned int>(last));
	}

	void clearRange()
	{
		properties.erase("Range");
	}

	void clearProxyAuthorization()
	{
		properties.erase("Proxy-Authorization");
	}

	void setProxyAuthorization(const std::string& base64ed)
	{
		std::string authenticate = "Basic ";
		authenticate += base64ed;
		properties.set("Proxy-Authorization", authenticate);
	}

	void setProxyAuthorization(const std::string& user,
							   const std::string& pass)
	{
		std::string user_pass = std::string(user) + ":" + pass;
		std::vector<char> vec(user_pass.begin(), user_pass.end());
		user_pass = Base64::encode(vec);
		setProxyAuthorization(user_pass);
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

#endif /* HTTPPROPERTY_HPP_ */


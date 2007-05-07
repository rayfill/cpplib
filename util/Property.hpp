#ifndef PROPERTY_HPP_
#define PROPERTY_HPP_
#include <string>
#include <map>
#include <cstring>

template <typename Comparator = std::less<std::string> >
class Property
{
	typedef std::map<std::string, std::string, Comparator> Properties;

private:
	Properties properties;

public:
	Property():
		properties()
	{}

	virtual ~Property()
	{}

	void set(const char* key, const std::string& value)
	{
		properties[key] = value;
	}

	std::string get(const char* key)
	{
		return properties[key];
	}

	void clear()
	{
		properties.clear();
	}

	void erase(const char* key)
	{
		properties.erase(key);
	}

	std::string toString()
	{
		std::string result;

		for (typename Properties::const_iterator itor = properties.begin();
			 itor != properties.end(); ++itor)
			result += itor->first + ": " + itor->second + "\r\n";

		return result;
	}

	Property& operator=(const Property& rhs)
	{
		this->properties = rhs.properties;

		return *this;
	}

	Property(const Property& rhs):
		properties(rhs.properties)
	{}
};

#endif /* PROPERTY_HPP_ */


#ifndef XMLEXCEPTION_HPP_
#define XMLEXCEPTION_HPP_

#include <exception>
#include <stdexcept>

class InvalidXMLPathException : public std::runtime_error
{
public:
	InvalidXMLPathException(const char* reason =
						"invalid argument for XML path."):
		std::runtime_error(reason)
	{}

	virtual ~InvalidXMLPathException() throw()
	{}
};

class WellformedException : public std::runtime_error
{
public:
	WellformedException(const char* reason =
						"bad element pair parsing document."):
		std::runtime_error(reason)
	{}

	virtual ~WellformedException() throw()
	{}
};

class InvalidTagException : public std::runtime_error
{
public:
	InvalidTagException(const char* reason =
						"bad tag name in document parsing."):
		std::runtime_error(reason)
	{}

	virtual ~InvalidTagException() throw()
	{}
};

#endif /* XMLEXCEPTION_HPP_ */

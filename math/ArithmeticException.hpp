#ifndef ARITHMETICEXCEPTION_HPP_
#define ARITHMETICEXCEPTION_HPP_
#include <stdexcept>
#include <string>

class ArithmeticException : public std::runtime_error
{
public:
	ArithmeticException(const std::string& message = "arithmetic exception")
		: std::runtime_error(message.c_str())
	{
	}

	virtual ~ArithmeticException() throw()
	{}
};

class OverflowException : public ArithmeticException
{
public:
	OverflowException()
		: ArithmeticException("overflow in calculate operation.")
	{}
};

class ZeroDivideException : public ArithmeticException
{
public:
	ZeroDivideException()
		: ArithmeticException("divide of zero exception.")
	{}
};

class TokenParseException : public std::runtime_error
{
public:
	TokenParseException(const std::string& message = "Token parsing process in invalid token found.")
		: std::runtime_error(message)
	{}
};

class OperationSourceException : public std::runtime_error
{
public:
	OperationSourceException()
		: std::runtime_error("invalid distination value of operation.")
	{}
};


#endif /* ARITHMETICEXCEPTION_HPP_ */

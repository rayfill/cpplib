#ifndef HTTPEXCEPTION_HPP_
#define HTTPEXCEPTION_HPP_

#include <stdexcept>
#include <string>
#include <text/LexicalCast.hpp>

class InvalidConnectRequestException : public std::runtime_error
{
private:
	const std::string& currentConnectionServer;
	const unsigned short currentConnectionPort;

public:
	InvalidConnectRequestException(const std::string& server,
								   unsigned short port):
		std::runtime_error("InvalidConnectRequestException."),
		currentConnectionServer(server),
		currentConnectionPort(port)
	{}

	~InvalidConnectRequestException() throw()
	{}

	virtual const char* what() const throw()
	{
		return
			(std::string("Invalid connect server, current connection for ") +
			 currentConnectionServer + ":" +
			 stringCast<unsigned short>(currentConnectionPort)).c_str();
	}
};



#endif /* HTTPEXCEPTION_HPP_ */

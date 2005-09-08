#ifndef SOCKETEXCEPTION_HPP_
#define SOCKETEXCEPTION_HPP_

#include <stdexcept>

/**
 * Socket‚ª”­¶‚³‚¹‚é—áŠOƒNƒ‰ƒX
 */
class SocketException : public std::runtime_error
{
public:
	SocketException():
		std::runtime_error("raise SocketException.")
	{}
};



#endif /* SOCKETEXCEPTION_HPP_ */

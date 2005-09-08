#ifndef SOCKETEXCEPTION_HPP_
#define SOCKETEXCEPTION_HPP_

#include <stdexcept>

/**
 * Socket�������������O�N���X
 */
class SocketException : public std::runtime_error
{
public:
	SocketException():
		std::runtime_error("raise SocketException.")
	{}
};



#endif /* SOCKETEXCEPTION_HPP_ */

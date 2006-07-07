#ifndef SOCKETEXCEPTION_HPP_
#define SOCKETEXCEPTION_HPP_

#include <stdexcept>

/**
 * Socket�������������O�N���X
 */
class SocketException : public std::runtime_error
{
public:
	explicit SocketException(const char* reason = "raise SocketException."):
		std::runtime_error(reason)
	{}
};

/**
 * �ڑ��������Ă���ꍇ�ɔ��������O
 */
class ConnectionClosedException : public SocketException
{
public:
	explicit ConnectionClosedException(const char* reason = "socket closed."):
		SocketException(reason)
	{}
};



#endif /* SOCKETEXCEPTION_HPP_ */

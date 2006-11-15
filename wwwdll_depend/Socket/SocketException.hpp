#ifndef SOCKETEXCEPTION_HPP_
#define SOCKETEXCEPTION_HPP_

#include <stdexcept>

/**
 * Socketが発生させる例外クラス
 */
class SocketException : public std::runtime_error
{
public:
	explicit SocketException(const char* reason = "raise SocketException."):
		std::runtime_error(reason)
	{}
};

/**
 * 接続が閉じられている場合に発生する例外
 */
class ConnectionClosedException : public SocketException
{
public:
	explicit ConnectionClosedException(const char* reason = "socket closed."):
		SocketException(reason)
	{}
};



#endif /* SOCKETEXCEPTION_HPP_ */

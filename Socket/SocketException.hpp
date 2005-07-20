#ifndef SOCKETEXCEPTION_HPP_
#define SOCKETEXCEPTION_HPP_

#include <stdexcept>
/**
 * Socketが発生させる例外クラス
 */
class SocketException : public std::runtime_error
{
};

#endif /* SOCKETEXCEPTION_HPP_ */

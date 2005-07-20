#ifndef THREADEXCEPTION_HPP_
#define THREADEXCEPTION_HPP_

#include <exception>
#include <stdexcept>
#include <string>

/**
 * Thread クラスインスタンス実行中に発生する例外
 */
class ThreadException : public std::runtime_error
{
public:
	/**
	 * デフォルトコンストラクタ
	 */ 
	ThreadException() throw()
		: std::runtime_error("thread exception.")
	{}
	
	/**
	 * デストラクタ
	 */
	virtual ~ThreadException() throw() {}
	
	/**
	 * コピーコンストラクタ
	 * @arg te コピー元オブジェクト
	 */
	ThreadException(const ThreadException& te) throw()
		: std::runtime_error(te.what())
	{}
	
	/**
	 * コンストラクタ
	 * @arg reason 例外発生理由を示す文字列
	 */
	ThreadException(const char* reason) throw()
		: std::runtime_error(reason)
	{}

	virtual ThreadException* clone() const throw()
	{
		return new ThreadException(*this);
	}
};

/**
 * 操作がタイムアウトしたことを意味する例外
 */
class TimeoutException : public std::runtime_error
{
public:
	/**
	 * デフォルトコンストラクタ
	 */
	TimeoutException() throw()
		: std::runtime_error("timeout exception.")
	{}

	/**
	 * デストラクタ
	 */
	virtual ~TimeoutException() throw() {}

	/**
	 * コンストラクタ
	 * @arg reason 例外発生理由を示す文字列
	 */
	TimeoutException(const char* reason) throw()
		: std::runtime_error(reason)
	{}
};

#endif /* THREADEXCEPTION_HPP_ */

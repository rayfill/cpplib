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
	 * @param te コピー元オブジェクト
	 */
	ThreadException(const ThreadException& te) throw()
		: std::runtime_error(te.what())
	{}
	
	/**
	 * コンストラクタ
	 * @param reason 例外発生理由を示す文字列
	 */
	ThreadException(const char* reason) throw()
		: std::runtime_error(reason)
	{}

	virtual ThreadException* clone() const throw(std::bad_alloc)
	{
		return new ThreadException(*this);
	}
};

/**
 * Threadの操作に割り込んで起こる例外
 */
class InteruptedException : public ThreadException
{
public:
	InteruptedException(const char* reason = "InteruptedException raised.")
		: ThreadException(reason)
	{}

	explicit InteruptedException(const InteruptedException& ie) throw()
			: ThreadException(ie.what())
	{}

	virtual ~InteruptedException() throw()
	{}

	virtual InteruptedException* clone() const throw(std::bad_alloc)
	{
		return new InteruptedException(*this);
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
	 * @param reason 例外発生理由を示す文字列
	 */
	TimeoutException(const char* reason) throw()
		: std::runtime_error(reason)
	{}
};

#endif /* THREADEXCEPTION_HPP_ */

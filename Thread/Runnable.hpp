#ifndef RUNNABLE_HPP_
#define RUNNABLE_HPP_

#include <Thread/ThreadException.hpp>
#include <cassert>
/**
 * 実行エントリポイントインタフェース
 */
class Runnable
{
public:
	/**
	 * 実行前の前処理
	 */
	virtual void prepare() throw()
	{}
		
	/**
	 * 実行後の後処理
	 */
	virtual void dispose() throw()
	{}

	/// ワーカー用エントリポイント。オーバーライドして使用する。
	virtual unsigned run() throw(ThreadException)
	{
		return 0;
	}

	virtual ~Runnable()
	{}
};

#endif /* RUNNABLE_HPP_ */

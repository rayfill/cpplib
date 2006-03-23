#ifndef RUNNABLE_HPP_
#define RUNNABLE_HPP_

#include <Thread/ThreadException.hpp>

/**
 * 実行エントリポイントインタフェース
 */
class Runnable
{
public:
	/**
	 * 実行前の前処理
	 */
	virtual void prepare() throw() = 0;
		

	/**
	 * 実行後の後処理
	 */
	virtual void dispose() throw() = 0;

	/// ワーカー用エントリポイント。オーバーライドして使用する。
	virtual unsigned run() throw(ThreadException) = 0;

};

#endif /* RUNNABLE_HPP_ */

#ifndef RERUNNABLETHREAD_HPP_
#define RERUNNABLETHREAD_HPP_

#include <Thread/Thread.hpp>
#include <Thread/TwinLock.hpp>
#include <Thread/Mutex.hpp>

/**
 * 再実行可能なThread
 * @todo quitable とかを変数にしてロック併用でやんなきゃダメか・・・
 */
class RerunnableThread : public Thread
{
	friend class ThreadPoolTest;

public:
	enum State {
		none,
		started,
		ended,
		quitable
	};
private:
	/**
	 * このクラス専用の実行エントリポイント
	 */
	Runnable* runnablePoint;

	/**
	 * 実行状態制御用
	 */
	TwinLock<Mutex> blocker;

	/**
	 * 実行状態変更の排他制御用
	 */
	Mutex stateLock;

	/**
	 * スレッドの実行状態
	 * @note 書き換えは全て子スレッド側で行う
	 */
	State state;

	/**
	 * 実行エントリの取得
	 * @return 現在設定されている実行エントリ
	 */
	Runnable* getRunnable() const
	{
		return runnablePoint;
	}

	/**
	 * 実行エントリの取得
	 * @param newPoint 新しい実行エントリポイント
	 */
	void setRunnable(Runnable* newPoint)
	{
		runnablePoint = newPoint;
	}

	/**
	 * 開始および完全終了用ブロックメソッド
	 * @return run()ループを脱出する場合true
	 */
	bool isQuitAndBlock()
	{
		{
			/**
			 * @todo reader writer lock もいるな・・・
			 */
			ScopedLock<Mutex> lock(stateLock);
			if (state == quitable)
				return true;
			state = ended;
		}

		// wait for parent's start signal.
		blocker.waitFromChild();

		{
			ScopedLock<Mutex> lock(stateLock);
			if (state == quitable)
				return true;
			state = started;
		}

		return false;
	}

	/**
	 * Runnableインタフェースの置換
	 */
	void replace(Runnable* entryPoint) throw()
	{
		assert(isReplacable());

		setRunnable(entryPoint);
	}

	RerunnableThread(const RerunnableThread&);
	RerunnableThread& operator=(const RerunnableThread&);
public:
	/**
	 * コンストラクタ
	 */
	RerunnableThread()
		: Thread(false),
		  runnablePoint(),
		  blocker(),
		  stateLock(),
		  state(none)
	{
		Thread::start(dynamic_cast<Runnable*>(this));
		Thread::yield();
	}

	~RerunnableThread() throw()
	{}

	/**
	 * Runnableインタフェースの置換判定
	 * @return 置換可能ならtrue
	 * @note PosixMutexが再入ロックサポートしてるか不明なので
	 * todoの実現はちょっと無理そう。再入できるんなら呼出元でロックして
	 * ここの中でもロックを行うことで実現可能なんだけど・・・
	 * PosixMutexをPosixCriticalSectionにしてMutexはそれを包括する形で
	 * 実装した方がいいのかねぇ?ってWinCriticalSection再入可能ジャン!!
	 * @todo ロック処理を前提とした安全なエントリポイント再配置処理の実装
	 */
	bool isReplacable()
	{
		return (state == none || state == ended);
	}

	virtual void start(Runnable* entryPoint) throw()
	{
		{
			ScopedLock<Mutex> lock(stateLock);
			if (entryPoint != NULL)
				replace(entryPoint);
		}

		blocker.waitFromParent();
	}

	virtual void start() throw()
	{
		start(NULL);
	}

	virtual unsigned int join()
		throw(ThreadException, TimeoutException)
	{
		assert(state != quitable);

		blocker.waitFromParent();

		return 0;
	}

	/**
	 * 脱出処理
	 * run()ループ脱出指示
	 */
	void quit() throw()
	{
		{
			ScopedLock<Mutex> lock(stateLock);
			state = quitable;
		}
		start(NULL);
		Thread::join();
	}

protected:
	virtual void prepare() throw()
	{
		blocker.childPrepare();
	}

	virtual void dispose() throw()
	{
		blocker.childDispose();
	}

	virtual unsigned int run() throw(ThreadException)
	{
		assert(dynamic_cast<RerunnableThread*>(this) == this);

		for (;;)
			{
				try
					{
						if(isQuitAndBlock())
							break;
					
						Runnable* target = NULL;
						{
							ScopedLock<Mutex> lock(stateLock);
							target = this->getRunnable();
						}
						assert(target != NULL);

						target->prepare();
						target->run();
						target->dispose();
					}
				catch (ThreadException& /*e*/)
					{
						/// @todo ログクラスとか作ったらそのエントリを置くか・・・
						;
					}

				ScopedLock<Mutex> lock(stateLock);
				state = ended;
				blocker.waitFromChild();
			}

		return 0;
	}
};

#endif /* RERUNNABLETHREAD_HPP_ */


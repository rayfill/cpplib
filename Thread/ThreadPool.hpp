#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <Thread/Thread.hpp>
#include <Thread/Event.hpp>
#include <Thread/ThreadException.hpp>

/**
 * スレッドプール
 * @param @managementThreads 管理対象のスレッド数
 * @param @isPrecreted 管理対象スレッドを最初から生成しておくかのフラグ
 * trueなら生成しておく。
 * @param ThreadType 管理対象のスレッドクラス
 * 
 */
template <typename ThreadType = Thread,
		  int managementThreads = 10,
		  bool isPrecreated = false>
class ThreadPool
{
	friend class ThreadPoolTest;

public:
	typedef ThreadType thread_t;
	
private:
	/**
	 * 保持するスレッド
	 */
	thread_t* threads[managementThreads];

	/**
	 * スレッドの回収をおこなう。
	 * 全ての保持スレッドの回収を行う。
	 */
	void collect() throw()
	{
		for (int index = 0; index < managementThreads; ++index)
		{
			if (threads[index])
			{
				if (threads[index]->isRunning())
				{
					threads[index]->quit();
					threads[index]->join();
				}

				delete threads[index];
				threads = NULL;
			}
		}
	}

	/**
	 * 
	 */
	thread_t* poolCreate()
	{
		return new RerunnableThread();
	}

	
public:
	/**
	 * 再実行可能なThread
	 * @todo Runnableホルダーを基底のRunnableスロット以外に設ける
	 */
	class RerunnableThread : public thread_t
	{
		friend class ThreadPoolTest;

	private:
		/**
		 * 開始イベント
		 */
		Event started;

	    /**
	     * ジョブ終了イベント
	     */
		Event ended;

		/**
		 * スレッド終了イベント
		 */
		Event quitable;

		/**
		 * このクラス専用の実行エントリポイント
		 */
		Runnable* runnablePoint;

		Runnable* getRunnable() const
		{
			return runnablePoint;
		}

		void setRunnable(Runnable* newPoint)
		{
			runnablePoint = newPoint;
		}

		/**
		 * スレッドを停止しなければならない状態かの判定
		 * @return 停止しなければならないときにtrue
		 */
		bool isQuit() throw()
		{
			return quitable.isEventArrived();
		}

		/**
		 * 開始および完全終了用ブロックメソッド
		 * @return run()ループを脱出する場合true
		 */
		bool isQuitAndBlock() throw()
		{
			HANDLE waits[2];
			waits[0] = started.getHandle();
			waits[1] = quitable.getHandle();

			DWORD result =
				::WaitForMultipleObjects(2,
										 waits,
										 FALSE,
										 INFINITE);
			if (started.isEventArrived())
				started.resetEvent();


			if (result == WAIT_OBJECT_0)
				return false;
			else if (result == (WAIT_OBJECT_0 + 1))
				return true;
				
			assert(false); // not arraived.
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

	public:
		/**
		 * コンストラクタ
		 */
		RerunnableThread()
				: thread_t(this, false), 
				  started(false),
				  ended(true),
				  quitable(true),
				  runnablePoint()
		{
			assert(started.getHandle() != NULL);
			thread_t::start();
		}

		~RerunnableThread() throw()
		{}

		/**
		 * Runnableインタフェースの置換判定
		 * @return 置換可能ならtrue
		 * @todo ロック処理を前提とした安全なエントリポイント再配置処理の実装
		 */
		bool isReplacable()
		{
			return !started.isEventArrived();
		}

		virtual unsigned start(Runnable* entryPoint) throw()
		{
			replace(entryPoint);

			started.setEvent();

			return 0;
		}

		virtual unsigned start() throw()
		{
			return start(this);
		}

		virtual unsigned join(DWORD waitTime = INFINITE)
		throw(ThreadException, TimeoutException)
		{
			ended.waitEventArrive(waitTime);

			return 0;
		}

		/**
		 * 脱出処理
		 * run()ループ脱出指示
		 */
		void quit() throw()
		{
			quitable.setEvent();
			Thread::join();
		}

	protected:
		virtual unsigned int run() throw(ThreadException)
		{
			assert(dynamic_cast<RerunnableThread*>(this) == this);

			for (;;)
			{
				if(isQuitAndBlock())
				{
					ended.setEvent();
					break;
				}

				try
				{
					if (this->getRunnable() == this)
						continue;

//					assert(this->getRunnable() != this);

					this->getRunnable()->prepare();
					this->getRunnable()->run();
					this->getRunnable()->dispose();
				}
				catch (InterruptedException& e)
				{
					;
				}

				ended.setEvent();
			}
			return 0;
		}
	};

	/**
	 * スレッドプールの作成
	 */
	ThreadPool():
		threads()
	{
		
	}

	virtual ~ThreadPool() throw()
	{
		collect();
	}

	thread_t* getThread() throw()
	{
		assert(false && !"not impremented yet.");

		for (int index = 0; index < managementThreads; ++ index)
		{
//			if (threads[index])
				
		}

		return NULL;
	}
};


#endif /* THREADPOOL_HPP_ */


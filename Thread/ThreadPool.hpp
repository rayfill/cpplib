#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <Thread/Thread.hpp>
#include <Thread/Event.hpp>
#include <Thread/ThreadException.hpp>
#include <Thread/SyncOperator.hpp>

/**
 * スレッドプール
 * @param @managementThreads 管理対象のスレッド数
 * @param @isPrecreted 管理対象スレッドを最初から生成しておくかのフラグ
 * trueなら生成しておく。
 * @param ThreadType 管理対象のスレッドクラス
 * @todo さっさと完成させちゃいましょう・・・
 */
template <int managementThreads = 10,
		  bool isPreCreated = false>
class ThreadPool
{
friend class ThreadPoolTest;

public:
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
		Event starter;

		/**
		 * 実行状態変更の排他制御用
		 */
		Mutex stateLock;

		/**
		 * スレッドの実行状態
		 */
		State state;


		Runnable* getRunnable() const
		{
			return runnablePoint;
		}

		void setRunnable(Runnable* newPoint)
		{
			ScopedLock<Mutex> lock(stateLock);
			runnablePoint = newPoint;
		}

		/**
		 * スレッドを停止しなければならない状態かの判定
		 * @return 停止しなければならないときにtrue
		 */
		bool isQuit() throw()
		{
			ScopedLock<Mutex> lock(stateLock);
			return state == quitable;
		}

		/**
		 * 開始および完全終了用ブロックメソッド
		 * @return run()ループを脱出する場合true
		 */
		bool isQuitAndBlock()
		{
			{
				starter.lock(); // parent thread ownership release wait.
				starter.unlock(); // release ownership.
			}
			ScopedLock<Mutex> lock(stateLock);
			state = started;
		}

		virtual unsigned start(Runnable* entryPoint) throw()
		{
			while (state == none)
				Thread::yield();

			{
				replace(entryPoint);
			}
			
			starter.unlock();
			ScopedLock<Mutex> lock(stateLock);
			while (!starter.tryLock());


			return 0;
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
				: Thread(this, false),
				  runnablePoint(),
				  starter(),
				  stateLock(),
				  state(none)
		{
			Thread::start();
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
			return (state == non || state == ended);
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
					break;
					
				try
				{
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
				catch (InterruptedException& /*e*/)
				{
					/// @todo ログクラスとか作ったらそのエントリを置くか・・・
					;
				}

				ScopedLock<Mutex> lock(stateLock);
				states = ended;
			}

			return 0;
		}
	};


	typedef RerunnableThread thread_t;
	
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
				threads[index] = NULL;
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


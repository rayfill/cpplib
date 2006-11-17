#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <Thread/Thread.hpp>
#include <Thread/Event.hpp>
#include <Thread/ThreadException.hpp>
#include <Thread/TwinLock.hpp>

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
		TwinLock blocker;

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
			ScopedLock<Mutex> lock(stateLock);
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
				  parent(),
				  child(),
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

		virtual void start(Runnable* entryPoint) throw()
		{
			{
				ScopedLock<Mutex> lock(stateLock);
				if (entryPoint != NULL)
					replace(entryPoint);
			}

			blocker.waitFromParent();

			return 0;
		}

		virtual void start() throw()
		{
			start(NULL);
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
			{
				ScopedLock<Mutex> lock(statusLock);
				state = qutable;
			}
			start(NULL);
			Thread::join();
		}

	protected:
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

<		for (int index = 0; index < managementThreads; ++ index)
		{
//			if (threads[index])
				
		}

		return NULL;
	}
};


#endif /* THREADPOOL_HPP_ */


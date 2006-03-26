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

	/**
	 * 再実行可能なThread
	 */
	class RerunnableThread : public thread_t
	{
		friend class ThreadPoolTest;

	private:
		/**
		 * 開始イベント
		 */
		Event startable;

		/**
		 * 終了イベント
		 */
		Event quitable;

		/**
		 * 実行エントリポイントの置換可能判定フラグ
		 * @todo イベントオブジェクトにすればロック機構なしでいけるかも
		 */
		volatile bool replacable;

		/**
		 * 脱出処理
		 * @throw InterruptedException 脱出シグナル例外
		 * @note 脱出処理に例外使うのは気が進まんが・・・いい方法が思いつかない
		 * @todo 処理内部からInterruptedException投げられたら判断つかんので
		 * QuitableExceptionでも作るか、もしくはblock()内からしかquitできない
		 * (run()にいてquitする場合はThreadに明示的にabort()
		 * してもらってInterruptExceptionハンドラ->block()でquit)
		 * ようにするか・・・
		 */
		void quit() throw(InterruptedException)
		{
			throw InterruptedException();
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
		 * @exception InterruptedException 終了処理例外
		 * @todo quit() で書いてる通り方式かえると思うので例外指定とか
		 * なくなる可能性高し
		 */
		bool isQuitAndBlock() throw()
		{
			if (startable.isEventArrived())
				startable.resetEvent();

			HANDLE waits[2];
			DWORD result =
				::WaitForMultipleObjects(2,
										 waits,
										 FALSE,
										 INFINITE);

			if (result == WAIT_OBJECT_0)
				return false;
			else if (result == (WAIT_OBJECT_0 + 1))
				return true;
				
			assert(false); // not arraived.
		}

		/**
		 * Runnableインタフェースの置換
		 */
		void replace(Runnable* entryPoint) throw()
		{
			assert(isReplacable());

			thread_t::setRunningTarget(entryPoint);
		}

	public:
		/**
		 * コンストラクタ
		 */
		RerunnableThread()
				: thread_t(false), 
				  startable("startable", false),
				  quitable("quitable", false)
		{
			assert(startable.getHandle() != NULL);
			thread_t::start();
		}

		~RerunnableThread() throw()
		{}

		/**
		 * Runnableインタフェースの置換判定
		 * @return 置換可能ならtrue
		 * @todo ロック処理を前提とした安全なエントリポイント再配置処理の実装
		 */
		bool isReplacable() const
		{
			return !startable.isEventArrived();
		}

		/**
		 * 保持しているRunnableインタフェースの実行
		 * @todo startableイベント見てまだ実行中ならブロックして待機
		 */
		void starting(Runnable* entryPoint) 
		{
			replace(entryPoint);

			startable.setEvent();
		}

	protected:
		virtual unsigned int callback() throw()
		{
			assert(dynamic_cast<RerunnableThread*>(this) == this);

			try
			{
				for (;;)
				{
					if(isQuitAndBlock())
						break;

					try
					{
						this->getRunningTarget()->prepare();
						this->getRunningTarget()->run();
						this->getRunningTarget()->dispose();
					}
					catch (InterruptedException& e)
					{
						;
					}
				}
			} 
			catch (...)
			{
				assert(false);
			}
			return 0;
		}
	};
	
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


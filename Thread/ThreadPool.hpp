#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <Thread/Thread.hpp>

/**
 * スレッドプール
 * @param @managementThreads 管理対象のスレッド数
 * @param @isPrecreted 管理対象スレッドを最初から生成しておくかのフラグ
 * trueなら生成しておく。
 * @param ThreadType 管理対象のスレッドクラス
 */
template <typename int managementThreads = 10,
		  typename bool isPrecreated = false,
		  typename ThreadType = Thread>
class ThreadPool
{
	friend class ThreadPoolTest;

public:
	typedef ThreadType thread_t;
	
private:

	thread_t* threads[managementThreads];

	/**
	 * スレッドの回収をおこなう。
	 * 全てのスレッドの回収を行う。
	 */
	void collect() throw()
	{
		for (int index = 0; index < managementThreads; ++index)
		{
			if (threads[index])
			{
				threads[index]->join();
				delete threads[index];
				threads = NULL;
			}
		}
	}

	/**
	 * 再実行可能なThread
	 */
	class RerunnableThread : public thread_t
	{
	private:
		void block() throw()
		{
			
		}

	public:
		RerunnableThread():
			thread_t()
		{}

		~RerunnableThread() throw()
		{}

	protected:
		virtual unsigned int callback() throw()
		{
			for (;;)
			{
				block();

				
			}

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
		for (int index = 0; index < managementThreads; ++ index)
		{
			if (threads[index])
		}
	}
};


#endif /* THREADPOOL_HPP_ */


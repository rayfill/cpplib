#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <Thread/Thread.hpp>

/**
 * �X���b�h�v�[��
 * @param @managementThreads �Ǘ��Ώۂ̃X���b�h��
 * @param @isPrecreted �Ǘ��ΏۃX���b�h���ŏ����琶�����Ă������̃t���O
 * true�Ȃ琶�����Ă����B
 * @param ThreadType �Ǘ��Ώۂ̃X���b�h�N���X
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
	 * �X���b�h�̉���������Ȃ��B
	 * �S�ẴX���b�h�̉�����s���B
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
	 * �Ď��s�\��Thread
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
	 * �X���b�h�v�[���̍쐬
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


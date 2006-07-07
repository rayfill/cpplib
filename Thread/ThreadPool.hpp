#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <Thread/Thread.hpp>
#include <Thread/Event.hpp>
#include <Thread/ThreadException.hpp>

/**
 * �X���b�h�v�[��
 * @param @managementThreads �Ǘ��Ώۂ̃X���b�h��
 * @param @isPrecreted �Ǘ��ΏۃX���b�h���ŏ����琶�����Ă������̃t���O
 * true�Ȃ琶�����Ă����B
 * @param ThreadType �Ǘ��Ώۂ̃X���b�h�N���X
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
	 * �ێ�����X���b�h
	 */
	thread_t* threads[managementThreads];

	/**
	 * �X���b�h�̉���������Ȃ��B
	 * �S�Ă̕ێ��X���b�h�̉�����s���B
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
	 * �Ď��s�\��Thread
	 * @todo Runnable�z���_�[������Runnable�X���b�g�ȊO�ɐ݂���
	 */
	class RerunnableThread : public thread_t
	{
		friend class ThreadPoolTest;

	private:
		/**
		 * �J�n�C�x���g
		 */
		Event started;

	    /**
	     * �W���u�I���C�x���g
	     */
		Event ended;

		/**
		 * �X���b�h�I���C�x���g
		 */
		Event quitable;

		/**
		 * ���̃N���X��p�̎��s�G���g���|�C���g
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
		 * �X���b�h���~���Ȃ���΂Ȃ�Ȃ���Ԃ��̔���
		 * @return ��~���Ȃ���΂Ȃ�Ȃ��Ƃ���true
		 */
		bool isQuit() throw()
		{
			return quitable.isEventArrived();
		}

		/**
		 * �J�n����ъ��S�I���p�u���b�N���\�b�h
		 * @return run()���[�v��E�o����ꍇtrue
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
		 * Runnable�C���^�t�F�[�X�̒u��
		 */
		void replace(Runnable* entryPoint) throw()
		{
			assert(isReplacable());

			setRunnable(entryPoint);
		}

	public:
		/**
		 * �R���X�g���N�^
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
		 * Runnable�C���^�t�F�[�X�̒u������
		 * @return �u���\�Ȃ�true
		 * @todo ���b�N������O��Ƃ������S�ȃG���g���|�C���g�Ĕz�u�����̎���
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
		 * �E�o����
		 * run()���[�v�E�o�w��
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
		assert(false && !"not impremented yet.");

		for (int index = 0; index < managementThreads; ++ index)
		{
//			if (threads[index])
				
		}

		return NULL;
	}
};


#endif /* THREADPOOL_HPP_ */


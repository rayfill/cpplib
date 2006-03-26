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

	/**
	 * �Ď��s�\��Thread
	 */
	class RerunnableThread : public thread_t
	{
		friend class ThreadPoolTest;

	private:
		/**
		 * �J�n�C�x���g
		 */
		Event startable;

		/**
		 * �I���C�x���g
		 */
		Event quitable;

		/**
		 * ���s�G���g���|�C���g�̒u���\����t���O
		 * @todo �C�x���g�I�u�W�F�N�g�ɂ���΃��b�N�@�\�Ȃ��ł����邩��
		 */
		volatile bool replacable;

		/**
		 * �E�o����
		 * @throw InterruptedException �E�o�V�O�i����O
		 * @note �E�o�����ɗ�O�g���̂͋C���i�܂񂪁E�E�E�������@���v�����Ȃ�
		 * @todo ������������InterruptedException������ꂽ�画�f����̂�
		 * QuitableException�ł���邩�A��������block()�����炵��quit�ł��Ȃ�
		 * (run()�ɂ���quit����ꍇ��Thread�ɖ����I��abort()
		 * ���Ă������InterruptException�n���h��->block()��quit)
		 * �悤�ɂ��邩�E�E�E
		 */
		void quit() throw(InterruptedException)
		{
			throw InterruptedException();
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
		 * @exception InterruptedException �I��������O
		 * @todo quit() �ŏ����Ă�ʂ����������Ǝv���̂ŗ�O�w��Ƃ�
		 * �Ȃ��Ȃ�\������
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
		 * Runnable�C���^�t�F�[�X�̒u��
		 */
		void replace(Runnable* entryPoint) throw()
		{
			assert(isReplacable());

			thread_t::setRunningTarget(entryPoint);
		}

	public:
		/**
		 * �R���X�g���N�^
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
		 * Runnable�C���^�t�F�[�X�̒u������
		 * @return �u���\�Ȃ�true
		 * @todo ���b�N������O��Ƃ������S�ȃG���g���|�C���g�Ĕz�u�����̎���
		 */
		bool isReplacable() const
		{
			return !startable.isEventArrived();
		}

		/**
		 * �ێ����Ă���Runnable�C���^�t�F�[�X�̎��s
		 * @todo startable�C�x���g���Ă܂����s���Ȃ�u���b�N���đҋ@
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


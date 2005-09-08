#ifndef RECOLLECTABLETHREADGROUP_HPP_
#define RECOLLECTABLETHREADGROUP_HPP_

#include <set>
#include <cassert>
#include <Thread/ThreadGroup.hpp>
#include <util/Notification.hpp>

/**
 * �I��Thread�̉���\��ThreadGroup�N���X
 */
class CollectableThreadGroup :
	public ThreadGroup, public Observer
{
private:
	typedef std::set<Thread::ThreadId_t> CollectableThreadIds;

	/**
	 * ����\�ƂȂ���Thread ID ��ێ�����R���e�i
	 */
	CollectableThreadIds collectable;

	/**
	 * �ʒm�n���h��
	 * @see Observable
	 */
	virtual void notify(Observable* notifier)
	{
		Thread* thread = dynamic_cast<Thread*>(notifier);
		assert(thread != NULL);

		collectable.insert(thread->getThreadId());
	}
	
public:
	/**
	 * �f�t�H���g�R���X�g���N�^
	 */
	CollectableThreadGroup() throw()
		: ThreadGroup(), Observer(), collectable()
	{}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~CollectableThreadGroup() throw()
	{}

	/**
	 * ����\��Thread�݂̂����
	 * @exception ThreadException join()����Thread����O�I�������ꍇ
	 */
	void join_recollectable() throw(ThreadException)
	{
		CollectableThreadIds::iterator itor;
		itor = collectable.begin();
		while(itor != collectable.end())
		{
			Thread* thread = this->detach(*itor);
			collectable.erase(itor++);
			try	{
				thread->join();
				delete thread;
			} catch(TimeoutException& /*e*/) {
				assert(false); // TimeoutException ��O�͔������Ȃ�
			}
		}
	}

	/**
	 * �V�����X���b�h�̓o�^
	 * @arg thread �o�^���� Thread �N���X�̃|�C���^
	 * @exception ThreadException ������thread���s���S�ȃX���b�h�̏ꍇ
	 * ���s�����
	 */
	virtual void attach(Thread* thread) throw(ThreadException)
	{
		Observable* observable = dynamic_cast<Observable*>(thread);
		assert(observable != NULL);

		CriticalSection lock;
		observable->attachObserver(static_cast<Observer*>(this));

		ThreadGroup::attach(thread);
	}

	/**
	 * �X���b�h�̓o�^����
	 * @arg id �o�^��������X���b�h��ThreadID
	 * @return �o�^�������ꂽ Thread �I�u�W�F�N�g�̃|�C���^
	 */
	virtual Thread* detach(const Thread::ThreadId_t id) throw()
	{
		Thread* thread = ThreadGroup::detach(id);
		assert(thread != NULL);

		CriticalSection lock;
		Observable* observable = dynamic_cast<Observable*>(thread);
		assert(observable != NULL);

		observable->detachObserver(static_cast<Observer*>(this));

		return thread;
	}
};

/**
 * ����\�ȃX���b�h�̃e���v���[�g�B���[�J�[���\�b�h(run())���I�[�o�[
 * ���C�h�����N���X�������ɂ��Ď��̉����A���p����B
 */
template <typename BaseThreadClass>
class CollectableThread
	: public BaseThreadClass
{
protected:
	/**
	 * ��n��(final�C���Ȃ̂Ōp�����Ȃ��ł�)
	 */ 
	virtual void dispose() throw()
	{
		assert(dynamic_cast<Observable*>(this) != 0);
		dynamic_cast<Observable*>(this)->update();
	}
public:

	/**
	 * �R���X�g���N�^
	 * �R���N�^(CollectableThreadGroup)�ɓo�^���Ȃ��܂܏I�������
	 * �\�������邽�߁A�쐬�Ɠ����Ɏ��s�͂ł��Ȃ�
	 */
	CollectableThread() throw():
		BaseThreadClass(false)
	{
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~CollectableThread() throw()
	{
	}
};
	
#endif /* RECOLLECTABLETHREADGROUP_HPP_ */

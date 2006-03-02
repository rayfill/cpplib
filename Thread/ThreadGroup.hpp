#ifndef THREADGROUP_HPP_
#define THREADGROUP_HPP_

#include <Thread/Thread.hpp>
#include <map>

/**
 * �X���b�h�}�l�[�W��
 */
class ThreadGroup
{
private:
	/**
	 * �X���b�h�}�b�p�[�^
	 */
	typedef std::map<Thread::thread_id_t, Thread*> ThreadMap;

	/**
	 * �X���b�h�}�b�s���O�p�}�b�v�ϐ�
	 */ 
	ThreadMap threadMapper;

	/**
	 * ���̃I�u�W�F�N�g�̃I�[�i�[��Thread ID
	 */
	const Thread::thread_id_t ownerId;

	/**
	 * �R�s�[�s�\�̂��߂̃v���C�x�[�g�R�s�[�R���X�g���N�^
	 */
	ThreadGroup(const ThreadGroup&) throw():
		threadMapper(), ownerId(0) {}

public:
	/**
	 * �f�t�H���g�R���X�g���N�^
	 */
	ThreadGroup() throw():
		threadMapper(), ownerId(::GetCurrentThreadId()) {}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~ThreadGroup() throw()
	{
		join_all();

		ThreadMap::iterator itor = threadMapper.begin();
		while(itor != threadMapper.end())
			threadMapper.erase(itor++);
	}

	/**
	 * �V�����X���b�h�̓o�^
	 * @param thread �o�^���� Thread �N���X�̃|�C���^
	 * @exception ThreadException ������thread���s���S�ȃX���b�h�̏ꍇ
	 * ���s�����
	 */
	virtual void attach(Thread* thread) throw(ThreadException)
	{
		CriticalSection lock;
		if (thread->getThreadId() == 0)
			throw ThreadException("Invalid argument, "
								  "incomplete thread object.");

		threadMapper[thread->getThreadId()] = thread;
	}

	/**
	 * �X���b�h�̓o�^����
	 * @param id �o�^��������X���b�h��ThreadID
	 * @return �o�^�������ꂽ Thread �I�u�W�F�N�g�̃|�C���^
	 */
	virtual Thread* detach(const Thread::thread_id_t id) throw()
	{
		assert(threadMapper[id] != NULL);

		Thread* thread = threadMapper[id];
		threadMapper.erase(id);

		assert(thread != NULL);
		return thread;
	}

	/**
	 * �X���b�h�̎��s�I���ҋ@
	 * @param id �ҋ@����X���b�h���ʎq
	 * @exception ThreadException �ҋ@����X���b�h����O�𓊂��ďI����
	 * ���ꍇ
	 */
	void join(const Thread::thread_id_t id) throw(ThreadException)
	{
		assert(this->ownerId == GetCurrentThreadId());

		CriticalSection lock;
		Thread* thread = this->detach(id);

		if (threadMapper[id]->join() ==
			Thread::abort_by_exception)
		try
		{
			if (threadMapper[id]->isAbnormalEnd())
				throw ThreadException(threadMapper[id]->reason().c_str());
		}
		catch (...)
		{
			delete thread;
			throw;
		}

		delete thread;
	}

	/**
	 * �Ǘ����Ă���X���b�h�̌��̎擾
	 * @return �Ǘ����Ă���X���b�h�̌�
	 */
	const size_t count() const throw()
	{
		return threadMapper.size();
	}

	/**
	 * �Ǘ����Ă���X���b�h�̎擾
	 * @param index �X���b�h�O���[�v�̃C���f�b�N�X�ԍ�
	 * @exception std::range_error �������͈͊O�������ꍇ
	 */
	Thread* operator[](size_t index) throw(std::range_error)
	{
		if (index >= this->count())
			throw std::range_error("index is out of bounds.");

		ThreadMap::iterator itor = threadMapper.begin();
		for(size_t offset = 0; offset < index; ++offset)
		{
			++itor;
		}

		return itor->second;
	}

	/**
	 * �Ǘ����Ă��邷�ׂẴX���b�h�̎��s�J�n
	 */
	void start_all() throw()
	{
		assert(this->ownerId == GetCurrentThreadId());

		for(ThreadMap::iterator itor = threadMapper.begin();
			itor != threadMapper.end();
			++itor)
			if (!itor->second->isRunning())
				itor->second->start();
	}

	/**
	 * ���ׂẴX���b�h�̏I���ҋ@
	 * @return ���ׂẴX���b�h�����s�I�������ꍇ: true, ����ȊO: false. 
	 * @see WinThread::join()
	 */
	bool join_all(DWORD waitTime = INFINITE) throw()
	{
		assert(this->ownerId == GetCurrentThreadId());

		bool isAllJoined = true;
			
		ThreadMap::iterator itor = threadMapper.begin();
		while(itor != threadMapper.end())
		{
			try
			{
				itor->second->join(waitTime);
			}
			catch (TimeoutException& /* e */)
			{
				isAllJoined = false;
			}
			++itor;
		}

		return isAllJoined;
	}
	
	/**
	 * �����I�ȃX���b�h�̎��s�I���ҋ@. �I�����Ă��Ȃ��ꍇ�����I�ɏI��
	 * ������. ���̏ꍇ�A�ȍ~�̎��s��Ԃ͕ۏ؂���Ȃ�.
	 * @see MSDN::Win32::TerminateThread()
	 */
	void force_join_all() throw()
	{
		assert(this->ownerId == GetCurrentThreadId());

		ThreadMap::iterator itor = threadMapper.begin();
		while(itor != threadMapper.end())
		{
			if (itor->second->isRunning())
				itor->second->abort();
			itor->second->join();
			++itor;
		}
	}
};

#endif /* THREADGROUP_HPP_ */

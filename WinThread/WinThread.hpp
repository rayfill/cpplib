#ifndef WINTHREAD_HPP_
#define WINTHREAD_HPP_

#ifdef _MSC_VER
#	pragma warning( disable : 4290 )
#endif /* _MSC_VER */

#include <windows.h>
#include <process.h>
#include <cassert>
#include <string>
#include <Thread/ThreadException.hpp>
#include <Thread/CriticalSection.hpp>

/**
 * Win32�x�[�X�X���b�h�N���X
 * �g�p�͌p�����A���z�֐� run()���I�[�o�[���C�h���ĉ�����
 * @Todo ��Ԍ����ƃV�O�i���x�[�X�̒��f�����ւ̕ύX
 */
class WinThread
{
public:
	/**
	 * ��Ԓ萔
	 */
	enum ThreadStatus
	{
		created,
		running,
		suspend,
		stop
	};

	static const unsigned abort_by_exception = 0xffffffff;
	static const unsigned abort_by_parent = 0xfffffffe;

	/**
	 * �X���b�h���ʎq�^
	 */
	typedef unsigned ThreadId_t;

private:
	friend class ThreadGroup;

	/**
	 * �X���b�h���
	 */
	ThreadStatus threadStatus;

	/**
	 * �X���b�h�n���h��
	 */
	HANDLE ThreadHandle;

	/**
	 * �X���b�h���ʎq
	 */
	ThreadId_t ThreadId;

	/**
	 * ������O�`�B�p�|�C���^
	 */
	ThreadException* transporter;

	/**
	 * �V�X�e���R�[���o�b�N�p�G���g���|�C���g
	 */
	static unsigned __stdcall CallbackDispatcher(void* DispatchKey) throw()
	{
		unsigned retValue = 0;
		WinThread* This = reinterpret_cast<WinThread*>(DispatchKey);

		{
			CriticalSection atomicOp;
			This->prepare();
			This->threadStatus = running;
		}

		try
		{
			retValue = This->run();
		}
		catch (ThreadException& e)
		{
			This->transporter =	e.clone();
			retValue = abort_by_exception;
		}
		catch (...)
		{
			This->transporter =	new ThreadException("unknown exception.");
			retValue = abort_by_exception;
		}

		{
			CriticalSection atomicOp;
			This->dispose();
			This->threadStatus = stop;
		}

		return retValue;
	}

	/// �R�s�[�h�~�p
	WinThread& operator=(WinThread&);

	/// �R�s�[�h�~�p
	WinThread(WinThread&);

protected:
	/**
	 * ���s�O�̑O����
	 */
	virtual void prepare() throw()
	{}

	/**
	 * ���s��̌㏈��
	 */
	virtual void dispose() throw()
	{}

	/**
	 * �N���X�\�z�w���p
	 */
	void create(bool createOnRun) throw(ThreadException)
	{
		CriticalSection atomicOp;

		this->ThreadHandle = (HANDLE)_beginthreadex(
			NULL,
			0,
			(unsigned (__stdcall*)(void*))WinThread::CallbackDispatcher,
			this,
			createOnRun ? 0 : CREATE_SUSPENDED,
			&this->ThreadId);

		if (this->ThreadHandle == NULL ||
			this->ThreadHandle == INVALID_HANDLE_VALUE)
			throw ThreadException("Can not create thread.");

		threadStatus = created;
	}

	/// ���[�J�[�p�G���g���|�C���g�B�I�[�o�[���C�h���Ďg�p����B
	virtual unsigned run() throw(ThreadException) = 0;

public:
	/**
	 * �f�t�H���g�R���X�g���N�^
	 * @arg createOnRun �쐬�Ɠ����Ɏ��s�J�n���邩�̃t���O
	 */
	WinThread(bool createOnRun = false) throw (ThreadException)
		: threadStatus(), ThreadHandle(),
		  ThreadId(), transporter(NULL)
	{
		create(createOnRun);
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~WinThread() throw()
	{
		assert(threadStatus == stop);

		if ((HANDLE)(this->ThreadHandle) != INVALID_HANDLE_VALUE ||
			this->ThreadHandle != 0)
			CloseHandle((HANDLE)this->ThreadHandle);

		if (transporter != NULL)
			delete transporter;
	}

	/**
	 * �X���b�h�̎��s
	 */
	unsigned start() throw()
	{
		assert(this->ThreadHandle);

		CriticalSection atomicOp;
		threadStatus = running;

		DWORD resumeCount = ResumeThread((HANDLE)this->ThreadHandle);
		assert(resumeCount == 1 || resumeCount == 0);
		return resumeCount;
	}

	/**
	 * �X���b�h�̎��s��Ԃ̎擾
	 * @return true: ���s���̓T�X�y���h��, false: ��~��
	 */
	bool isRunning() throw()
	{
		CriticalSection atomicOp;
		if (threadStatus == running || 
			threadStatus == suspend ||
			threadStatus == created)
			return true;
		else
			return false;
	}

	/**
	 * �X���b�h�̎��s���ꎞ�x�~
	 * \arg waitTimeForMilliSeconds �x�~���鎞�Ԃ��~���b�Ŏw�肷��
	 * 0��n���Ǝ��s�����ق��̃X���b�h�ɓn���B
	 */
	static void sleep(unsigned int waitTimeForMilliSeconds) throw()
	{
		::Sleep(waitTimeForMilliSeconds);
	}

	/**
	 * �X���b�h�̎��s�𒆎~����B�Ȍ�̏�Ԃ͕ۏ؂���Ȃ��B
	 * @exception ThreadException �X���b�h�̒��~�����s�����ꍇ
	 */
	void abort() throw(ThreadException)
	{
		assert(this->ThreadHandle != NULL);

		CriticalSection atmicOp;
		if (!::TerminateThread(this->ThreadHandle,
							  static_cast<DWORD>(abort_by_parent)))
			throw ThreadException("Thread termination fail.");

		threadStatus = stop;
	}

	/**
	 * �X���b�h�̎��s�̒��f�B�ĊJ����ꍇ��start()���\�b�h���g��
	 */
	void cancel() throw()
	{
		assert(this->ThreadHandle != NULL);

		CriticalSection atomicOp;
		DWORD suspendCount = SuspendThread((HANDLE)this->ThreadHandle);
		threadStatus = suspend;

		assert(suspendCount == 0);
	}

	/**
	 * ���݂̃X���b�h�̃X���b�h���ʎq��Ԃ�
	 */
	static const WinThread::ThreadId_t self() throw()
	{
		return GetCurrentThreadId();
	}

	/**
	 * �X���b�h�I�u�W�F�N�g�̎��ʎq��Ԃ�
	 */
	const WinThread::ThreadId_t getThreadId() throw()
	{
		return this->ThreadId;
	}

	/**
	 * �X���b�h�̏I���ҋ@
	 * \arg waitTime �X���b�h�̏I���ҋ@����(�~���b)�B�f�t�H���g�͖���
	 * @exception TimeoutException �ҋ@���Ԃ��߂��Ă��X���b�h���I����
	 * �Ȃ������ꍇ
	 * @exception ThreadExcpetion ���炩�ُ̈킪���������ꍇ
	 */
	unsigned join(DWORD waitTime = INFINITE)
		throw(ThreadException, TimeoutException)
	{
		assert(this->ThreadHandle != NULL);

		DWORD result = WaitForSingleObject(this->ThreadHandle, waitTime);
		switch(result)
		{
		case WAIT_OBJECT_0:
			unsigned retValue;
			GetExitCodeThread(this->ThreadHandle, (DWORD*)&retValue);
			return retValue;

		case WAIT_TIMEOUT:
			throw TimeoutException();

		case WAIT_ABANDONED:
			throw ThreadException("Invalid thread wait.");

		default:
			throw ThreadException();
		}
	}

	/**
	 * �X���b�h����O�𔭍s�����ꍇ�̗�O�������Ԃ�
	 */
	std::string reason() const throw()
	{
		assert(threadStatus == stop);

		if (transporter)
			return transporter->what();
		else
			return "";
	}

	/**
	 * �X���b�h�̏I����Ԃ̎擾
	 * @return true: �ُ�I��, false: ����I��
	 */
	bool isAbnormalEnd() const throw()
	{
		assert(threadStatus == stop);

		if (transporter)
			return true;
		return false;
	}

};

typedef WinThread Thread;

#endif /* WINTHREAD_HPP_ */

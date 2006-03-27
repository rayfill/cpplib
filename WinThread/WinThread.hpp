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
#include <Thread/Runnable.hpp>

/**
 * Win32�x�[�X�X���b�h�N���X
 *
 * �p�����A���z�֐� run()���I�[�o�[���C�h���邩�A
 * Runnable�C���^�t�F�[�X�����������N���X��p�ӂ���
 * �R���X�g���N�^�ɐH�킹�Ă��������B
 * @todo �e���b�N�����̃|���V�[���Bthis�|�C���^���b�N�ɂ����ق��������悳��
 * @todo ��Ԍ����ƃV�O�i���`�b�N�Ȓ��f�����ւ̕ύX
 * @todo Runnable�z���_�[���e���v���[�g�ɂ��Ƃ���SmartPointer�Ƃ�
 * AutoPtr�Ƃ��g����Ȃ��E�E�E
 */
class WinThread : public Runnable
{
	friend class ThreadGroup;

public:
	enum {
		/// ��O�ɂ���~������킷�萔
		abort_by_exception = 0xffffffff,
		/// �e����̃��N�G�X�g�ɂ�蒆�f��\���萔
		abort_by_parent = 0xfffffffe
	};

	/**
	 * �X���b�h���ʎq�^
	 */
	typedef unsigned thread_id_t;

private:
	/**
	 * ��Ԓ萔
	 */
	enum RunningStatus
	{
		created,
		running,
		suspend,
		stop
	};

	/**
	 * ���s�Ώ�
	 */
	Runnable* runningTarget;

	/**
	 * �X���b�h���
	 */
	volatile RunningStatus status;

	/**
	 * �X���b�h�n���h��
	 */
	HANDLE ThreadHandle;

	/**
	 * �X���b�h���ʎq
	 */
	thread_id_t ThreadId;

	/**
	 * ������O�`�B�p�|�C���^
	 */
	ThreadException* transporter;

	/**
	 * ��~�p�t���O
	 */
	volatile bool isAborting;

	/**
	 * �V�X�e���R�[���o�b�N�p�G���g���|�C���g
	 * @see Win32 CreateThread() API
	 */
	static unsigned __stdcall CallbackDispatcher(void* DispatchKey) throw()
	{
		WinThread* This = static_cast<WinThread*>(DispatchKey);

		return This->callback();
	}

	/// �R�s�[�h�~�p
	WinThread& operator=(WinThread&);

	/// �R�s�[�h�~�p
	WinThread(WinThread&);

protected:
	/**
	 * ���݂̃G���g���|�C���g�̎擾
	 * @return ���݂̃G���g���|�C���g
	 */
	Runnable* getRunningTarget() const throw()
	{
		return runningTarget;
	}

	/**
	 * �V�����G���g���|�C���g�̐ݒ�
	 * @param runnable �V�����G���g���|�C���g
	 * @note ���b�N�@�\�����ĂȂ��̂Ŏ��s���ɏ��������Ȃ��悤���ӁB
	 * @todo �K�v�Ȃ烍�b�N�@�\�������ق�����������
	 */
	void setRunningTarget(Runnable* runnable) throw()
	{
		runningTarget = runnable;
	}

	/**
	 * �V�X�e���R�[���o�b�N�p�G���g���|�C���g
	 * ������J�X�^�}�C�Y�ł���悤protected�X�R�[�v�ɔz�u
	 */
	virtual unsigned int callback() throw()
	{
		unsigned retValue = 0;

		if (this->runningTarget == NULL)
			this->runningTarget = this;

		{
			CriticalSection atomicOp;
			this->runningTarget->prepare();
			this->status = running;
		}

		try
		{
			retValue = this->runningTarget->run();
		}
		catch (InterruptedException& e)
		{
			this->transporter = e.clone();
			retValue = abort_by_parent;
		}
		catch (ThreadException& e)
		{
			this->transporter =	e.clone();
			retValue = abort_by_exception;
		}
		catch (...)
		{
			this->transporter =	new ThreadException("unknown exception.");
			retValue = abort_by_exception;
		}

		{
			CriticalSection atomicOp;
			this->runningTarget->dispose();
			this->status = stop;
		}

		return retValue;
	}

	/**
	 * �N���X�\�z�w���p
	 * @param createOnRun �쐬�Ɠ����ɃX���b�h�����s���邩�̃t���O
	 * true�ō쐬���Ɏ��s�J�n�Afalse���ƍ쐬���suspend���Ă���B
	 * start() ���\�b�h�Ŏ��s�J�n����B
	 * @see start()
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

		status = created;
	}

	/**
	 * ��~����
	 * @return ��~��ԂȂ�true
	 */
	bool isAbort() const throw()
	{
		return isAborting;
	}

public:
	/**
	 * �f�t�H���g�R���X�g���N�^
	 * @param createOnRun �쐬�Ɠ����Ɏ��s�J�n���邩�����ʂ���t���O
	 */
	WinThread(bool createOnRun = false) throw (ThreadException)
		: runningTarget(), status(), ThreadHandle(),
		  ThreadId(), transporter(NULL), isAborting(false)
	{
		create(createOnRun);
	}

	/**
	 * �R���X�g���N�^
	 * @param runnableObject ���s�G���g���|�C���g�I�u�W�F�N�g
	 * @param createOnRun �쐬�Ɠ����Ɏ��s�J�n���邩�����ʂ���t���O
	 */
	WinThread(Runnable* runnable_,
			  bool createOnRun = false) throw (ThreadException)
		: runningTarget(runnable_), status(), ThreadHandle(),
		  ThreadId(), transporter(NULL), isAborting(false)
	{
		assert(runnable_ != NULL);
		create(createOnRun);
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~WinThread() throw()
	{
		assert(status == stop ||
			   status == created);

		if ((HANDLE)(this->ThreadHandle) != INVALID_HANDLE_VALUE ||
			this->ThreadHandle != 0)
			CloseHandle((HANDLE)this->ThreadHandle);

		if (transporter != NULL)
			delete transporter;
	}

	/**
	 * �X���b�h�̎��s
	 * @return ���W���[�����x���B0�Ŏ��s�J�n�A>0�ŃT�X�y���h��
	 */
	virtual unsigned start() throw()
	{
		assert(this->ThreadHandle);

		CriticalSection atomicOp;
		assert(status == created);

		status = running;

		DWORD resumeCount = ResumeThread((HANDLE)this->ThreadHandle);
		assert(resumeCount == 1 || resumeCount == 0);
		return resumeCount;
	}

	/**
	 * �X���b�h�̎��s
	 * @param entryPoint ���s�ꏊ�����I�u�W�F�N�g�̃|�C���^
	 * @return ���W���[�����x���B0�Ŏ��s�J�n�A>0�ŃT�X�y���h��
	 */
	virtual unsigned start(Runnable* entryPoint) throw()
	{
		assert(this->ThreadHandle);

		CriticalSection atomicOp;
		this->setRunningTarget(entryPoint);

		return start();
	}

	/**
	 * �X���b�h�̎��s��Ԃ̎擾
	 * @return true: ���s���̓T�X�y���h��, false: ��~��
	 */
	bool isRunning() throw()
	{
		CriticalSection atomicOp;
		if (status == running || 
			status == suspend)
			return true;
		else
			return false;
	}

	/**
	 * �X���b�h�̎��s���ꎞ�x�~
	 * @param waitTimeForMilliSeconds �x�~���鎞�Ԃ��~���b�Ŏw�肷��
	 * 0��n���Ǝ��s�����ق��̃X���b�h�ɓn���B
	 */
	static void sleep(unsigned int waitTimeForMilliSeconds) throw()
	{
		::Sleep(waitTimeForMilliSeconds);
	}

	/**
	 * �X���b�h�̎��s���𑼂֓n��
	 * @exception InterruptedException �X���b�h�C���X�^���X����
	 * abort()���Ă΂�Ă����ꍇ
	 */
	void yield() throw(InterruptedException)
	{
		if (isAbort())
			throw InterruptedException();

		WinThread::sleep(0);
	}

	/**
	 * �X���b�h�̎��s�𒆎~����B
	 * ��~�����X���b�h�͓���������yield()���Ăяo�����Ƃ���
	 * InterruptedException�����s�����B
	 * @todo ��~����Method�̑���
	 */
	void abort() throw()
	{
		assert(this->ThreadHandle != NULL);

		CriticalSection atmicOp;
		isAborting = true;
	}

	/**
	 * �X���b�h�̎��s�̒��f�B�ĊJ����ꍇ��start()���\�b�h���g��
	 */
	void cancel() throw()
	{
		assert(this->ThreadHandle != NULL);

		CriticalSection atomicOp;
		DWORD suspendCount = SuspendThread((HANDLE)this->ThreadHandle);
		status = suspend;

		assert(suspendCount == 0);
	}

	/**
	 * ���݂̃X���b�h�̃X���b�h���ʎq��Ԃ�
	 * @return �X���b�h���ʎq
	 */
	static const WinThread::thread_id_t self() throw()
	{
		return GetCurrentThreadId();
	}

	/**
	 * �X���b�h�I�u�W�F�N�g�̎��ʎq��Ԃ�
	 * @return �X���b�h���ʎq
	 */
	const WinThread::thread_id_t getThreadId() throw()
	{
		return this->ThreadId;
	}

	/**
	 * �X���b�h�̏I���ҋ@
	 * @param waitTime �X���b�h�̏I���ҋ@����(�~���b)�B�f�t�H���g�͖���
	 * @exception TimeoutException �ҋ@���Ԃ��߂��Ă��X���b�h���I����
	 * �Ȃ������ꍇ
	 * @exception ThreadExcpetion ���炩�ُ̈킪���������ꍇ
	 */
	virtual unsigned join(DWORD waitTime = INFINITE)
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
	 * �X���b�h����O�𔭍s�����ꍇ�̗�O���R�������Ԃ�
	 * @return ��O���R
	 */
	std::string reason() const throw()
	{
		assert(status == stop);

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
		assert(status == stop);

		if (transporter)
			return true;
		return false;
	}

	virtual void prepare() throw()
	{}

	virtual void dispose() throw()
	{}

	virtual unsigned run() throw(ThreadException) 
	{
		return 0;
	}
	
};

typedef WinThread Thread;

#endif /* WINTHREAD_HPP_ */

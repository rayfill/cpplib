#include <pthread.h>
#include <Thread/ThreadException.hpp>

class PosixThread
{
public:
	/**
	 * �I����Ԃ������萔
	 */
	static const unsigned abort_by_exception = 0xffffffff;

	/**
	 * �X���b�h���ʎq�^
	 */
	typedef pthread_t thread_id_t;

private:
	/**
	 * �X���b�h�n���h��
	 * �X���b�h���ʎq
	 */
	thread_id_t ThreadId;

	/**
	 * ������O�`�B�p�|�C���^
	 */
	ThreadException* transporter;

	/**
	 * ���s��ԃt���O
	 */
	bool isRun;
	ThreadException

	/**
	 * �V�X�e���R�[���o�b�N�p�G���g���|�C���g
	 */
	static void CallbackDispatcher(void* DispatchKey)
	{
		int retValue = 0;
		do
		{
			// CriticalSection atomicOp;
			reinterpret_cast<PosixThread*>(DispatchKey)->isRun = true;
		} while (false);

		try 
		{
			retValue = (reinterpret_cast<PosixThread*>(DispatchKey)->run());
		}
		catch (ThreadException& e)
		{
			This->transporter = new ThreadException(e);
			retValue = abort_by_exception;
		}
		catch (...)
		{
			This->transporter =	new ThreadException("unknown exception.");
			retValue = abort_by_exception;
		}

		do
		{
			// CriticalSection atomicOp;
			reinterpret_cast<PosixThread*>(DispatchKey)->isRun = false;
		} while (false);

		pthread_exit(reinterpret_cast<void*>(retValue));
	}

	/// �R�s�[�h�~�p
	PosixThread& operator=(PosixThread&);
	/// �R�s�[�h�~�p
	PosixThread(PosixThread&);

protected:
	void create(bool createOnRun) throw(ThreadException)
	{
		/// @Todo ��O�����@�\�̒ǉ�
		pthread_create(&this->ThreadId, NULL,
					   static_cast<void*(*)(void*)>
					   (PosixThread::CallbackDispatcher),
					   this);
	}

	/// ���[�J�[�p�G���g���|�C���g�B�I�[�o�[���C�h���Ďg�p����B
	virtual unsigned int run() throw(ThreadException)
	{
	}

public:
	/**
	 * �f�t�H���g�R���X�g���N�^
	 * @param createOnRun �쐬�Ɠ����Ɏ��s�J�n���邩�̃t���O
	 */
	PosixThread(bool createOnRun = false) throw (ThreadException)
		: ThreadId(), transporter(NULL), isRun(false)
	{
		create(createOnRun);
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~PosixThread()
	{
		if (this->ThreadId != NULL)
	}

	void sleep(unsigned int WaitTimeForMilliSeconds) {
		//#error not implemented.
	}

	void cancel() {
		pthread_cancel(this->ThreadId);
	}
	bool isRunning() {
		return this->isRun;
	}

	static pthread_t self() {
		return pthread_self();
	}

	int join() {
		int retValue;
		pthread_join(this->ThreadId, reinterpret_cast<void**>(&retValue));
		return retValue;
	}
};



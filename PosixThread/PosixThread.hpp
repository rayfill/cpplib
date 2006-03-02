#include <pthread.h>
#include <Thread/ThreadException.hpp>

class PosixThread
{
public:
	/**
	 * 終了状態を示す定数
	 */
	static const unsigned abort_by_exception = 0xffffffff;

	/**
	 * スレッド識別子型
	 */
	typedef pthread_t thread_id_t;

private:
	/**
	 * スレッドハンドル
	 * スレッド識別子
	 */
	thread_id_t ThreadId;

	/**
	 * 処理例外伝達用ポインタ
	 */
	ThreadException* transporter;

	/**
	 * 実行状態フラグ
	 */
	bool isRun;
	ThreadException

	/**
	 * システムコールバック用エントリポイント
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

	/// コピー防止用
	PosixThread& operator=(PosixThread&);
	/// コピー防止用
	PosixThread(PosixThread&);

protected:
	void create(bool createOnRun) throw(ThreadException)
	{
		/// @Todo 例外処理機構の追加
		pthread_create(&this->ThreadId, NULL,
					   static_cast<void*(*)(void*)>
					   (PosixThread::CallbackDispatcher),
					   this);
	}

	/// ワーカー用エントリポイント。オーバーライドして使用する。
	virtual unsigned int run() throw(ThreadException)
	{
	}

public:
	/**
	 * デフォルトコンストラクタ
	 * @param createOnRun 作成と同時に実行開始するかのフラグ
	 */
	PosixThread(bool createOnRun = false) throw (ThreadException)
		: ThreadId(), transporter(NULL), isRun(false)
	{
		create(createOnRun);
	}

	/**
	 * デストラクタ
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



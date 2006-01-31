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
 * Win32ベーススレッドクラス
 * 使用は継承し、仮想関数 run()をオーバーライドして下さい
 * @Todo 状態検査とシグナルベースの中断処理への変更
 */
class WinThread
{
public:
	/**
	 * 状態定数
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
	 * スレッド識別子型
	 */
	typedef unsigned ThreadId_t;

private:
	friend class ThreadGroup;

	/**
	 * スレッド状態
	 */
	ThreadStatus threadStatus;

	/**
	 * スレッドハンドル
	 */
	HANDLE ThreadHandle;

	/**
	 * スレッド識別子
	 */
	ThreadId_t ThreadId;

	/**
	 * 処理例外伝達用ポインタ
	 */
	ThreadException* transporter;

	/**
	 * システムコールバック用エントリポイント
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

	/// コピー防止用
	WinThread& operator=(WinThread&);

	/// コピー防止用
	WinThread(WinThread&);

protected:
	/**
	 * 実行前の前処理
	 */
	virtual void prepare() throw()
	{}

	/**
	 * 実行後の後処理
	 */
	virtual void dispose() throw()
	{}

	/**
	 * クラス構築ヘルパ
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

	/// ワーカー用エントリポイント。オーバーライドして使用する。
	virtual unsigned run() throw(ThreadException) = 0;

public:
	/**
	 * デフォルトコンストラクタ
	 * @arg createOnRun 作成と同時に実行開始するかのフラグ
	 */
	WinThread(bool createOnRun = false) throw (ThreadException)
		: threadStatus(), ThreadHandle(),
		  ThreadId(), transporter(NULL)
	{
		create(createOnRun);
	}

	/**
	 * デストラクタ
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
	 * スレッドの実行
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
	 * スレッドの実行状態の取得
	 * @return true: 実行又はサスペンド中, false: 停止中
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
	 * スレッドの実行を一時休止
	 * \arg waitTimeForMilliSeconds 休止する時間をミリ秒で指定する
	 * 0を渡すと実行権をほかのスレッドに渡す。
	 */
	static void sleep(unsigned int waitTimeForMilliSeconds) throw()
	{
		::Sleep(waitTimeForMilliSeconds);
	}

	/**
	 * スレッドの実行を中止する。以後の状態は保証されない。
	 * @exception ThreadException スレッドの中止が失敗した場合
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
	 * スレッドの実行の中断。再開する場合はstart()メソッドを使う
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
	 * 現在のスレッドのスレッド識別子を返す
	 */
	static const WinThread::ThreadId_t self() throw()
	{
		return GetCurrentThreadId();
	}

	/**
	 * スレッドオブジェクトの識別子を返す
	 */
	const WinThread::ThreadId_t getThreadId() throw()
	{
		return this->ThreadId;
	}

	/**
	 * スレッドの終了待機
	 * \arg waitTime スレッドの終了待機時間(ミリ秒)。デフォルトは無限
	 * @exception TimeoutException 待機時間を過ぎてもスレッドが終了し
	 * なかった場合
	 * @exception ThreadExcpetion 何らかの異常が発生した場合
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
	 * スレッドが例外を発行した場合の例外文字列を返す
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
	 * スレッドの終了状態の取得
	 * @return true: 異常終了, false: 正常終了
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

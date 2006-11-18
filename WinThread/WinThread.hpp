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
#include <Thread/ScopedLock.hpp>

/**
 * Win32ベーススレッドクラス
 *
 * 継承し、仮想関数 run()をオーバーライドするか、
 * Runnableインタフェースを実装したクラスを用意して
 * コンストラクタに食わせてください。
 */
class WinThread : public Runnable
{
	friend class ThreadGroup;

public:
	enum {
		/// 例外により停止
		abort_by_exception = 0xffffffff,

		/// 親からのリクエストにより中断
		abort_by_parent = 0xfffffffe
	};

	/**
	 * スレッド識別子型
	 */
	typedef unsigned thread_id_t;

private:
	/**
	 * 実行対象
	 */
	Runnable* runningTarget;

	/**
	 * スレッドハンドル
	 */
	HANDLE threadHandle;

	/**
	 * スレッド識別子
	 */
	thread_id_t threadId;

	/**
	 * 処理例外伝達用ポインタ
	 */
	ThreadException* transporter;

	/**
	 * 状態変更ロック
	 */
	CriticalSection section;

	/**
	 * 実行状態
	 */
	bool isRun;

	/**
	 * システムコールバック用エントリポイント
	 * @see Win32 CreateThread() API
	 */
	static unsigned __stdcall CallbackDispatcher(void* DispatchKey) throw()
	{
		WinThread* This = static_cast<WinThread*>(DispatchKey);

		return This->callback();
	}

	/**
	 * 割り込み処理。
	 * abort()された場合にスレッド内部でabort処理をするために
	 * InterruptedExceptionを呼び出すヘルパ
	 */
	void processInterruption() throw (InterruptedException)
	{
		if (this->getThreadId() != self())
			return;

	}

	/// コピー防止用
	WinThread& operator=(WinThread&);

	/// コピー防止用
	WinThread(WinThread&);

protected:
	/**
	 * 現在のエントリポイントの取得
	 * @return 現在のエントリポイント
	 */
	Runnable* getRunningTarget()
	{
		return (runningTarget == NULL) ? this : runningTarget;
	}

	/**
	 * 新しいエントリポイントの設定
	 * @param runnable 新しいエントリポイント
	 */
	void setRunningTarget(Runnable* runnable) throw()
	{
		runningTarget = runnable;
	}

	/**
	 * システムコールバック用エントリポイント
	 */
	virtual unsigned int callback() throw()
	{
		unsigned retValue = 0;

		Runnable* target = NULL;
		{
			ScopedLock<CriticalSection> lock(section);
			target = getRunningTarget();
			isRun = true;
		}

		try
		{
			target->prepare();
			retValue = target->run();
			target->dispose();
		}
		catch (ThreadException& e)
		{
			this->transporter =	e.clone();
			retValue = static_cast<unsigned>(abort_by_exception);
		}
		catch (...)
		{
			this->transporter =	new ThreadException("unknown exception.");
			retValue = static_cast<unsigned>(abort_by_exception);
		}

		{
			ScopedLock<CriticalSection> lock(section);
			isRun = false;
		}

		return retValue;
	}

	/**
	 * クラス構築ヘルパ
	 * @param createOnRun 作成と同時にスレッドを実行するかのフラグ
	 * trueで作成時に実行開始、falseだと作成後はsuspendしている。
	 * start() メソッドで実行開始する。
	 * @see start()
	 */
	void create(bool createOnRun) throw(ThreadException)
	{
		ScopedLock<CriticalSection> lock(section);

		this->threadHandle = (HANDLE)_beginthreadex(
			NULL,
			0,
			(unsigned (__stdcall*)(void*))WinThread::CallbackDispatcher,
			this,
			createOnRun ? 0 : CREATE_SUSPENDED,
			&this->threadId);

		if (this->threadHandle == NULL ||
			this->threadHandle == INVALID_HANDLE_VALUE)
			throw ThreadException("Can not create thread.");

	}

public:
	/**
	 * デフォルトコンストラクタ
	 * @param createOnRun 作成と同時に実行開始するかを識別するフラグ
	 */
	explicit WinThread(bool createOnRun = false) throw (ThreadException)
		: runningTarget(), threadHandle(),
		  threadId(), transporter(NULL), section(), isRun(false)
	{
		runningTarget = this;
		create(createOnRun);
	}

	/**
	 * コンストラクタ
	 * @param runnableObject 実行エントリポイントオブジェクト
	 * @param createOnRun 作成と同時に実行開始するかを識別するフラグ
	 */
	explicit WinThread(Runnable* runnable_,
			  bool createOnRun = false) throw (ThreadException)
		: runningTarget(runnable_), threadHandle(),
		  threadId(), transporter(NULL), section(), isRun(false)
	{
		assert(runnable_ != NULL);
		create(createOnRun);
	}

	/**
	 * デストラクタ
	 */
	virtual ~WinThread() throw(ThreadException)
	{
		if ((HANDLE)(this->threadHandle) != INVALID_HANDLE_VALUE ||
			this->threadHandle != 0)
			CloseHandle((HANDLE)this->threadHandle);

		if (transporter != NULL)
			delete transporter;
	}

	/**
	 * スレッドの実行
	 */
	virtual void start() throw()
	{
		assert(this->threadHandle);

		ScopedLock<CriticalSection> lock(section);
		DWORD resumeCount =
			ResumeThread(this->threadHandle);
		assert(resumeCount == 1 || resumeCount == 0);
	}

	/**
	 * スレッドの実行
	 * @param entryPoint 実行場所を持つオブジェクトのポインタ
	 */
	virtual void start(Runnable* entryPoint) throw()
	{
		assert(this->threadHandle);

		ScopedLock<CriticalSection> lock(section);
		this->setRunningTarget(entryPoint);
		DWORD resumeCount =
			ResumeThread(this->threadHandle);
		assert(resumeCount == 1 || resumeCount == 0);
	}

	/**
	 * 現在の実行状態を返す
	 * @return 実行中であればtrue
	 */
	bool isRunning()
	{
		ScopedLock<CriticalSection> lock(section);
		return isRun;
	}
	
	/**
	 * スレッドの実行を一時休止
	 * @param waitTimeForMilliSeconds 休止する時間をミリ秒で指定する
	 * 0を渡すと実行権をほかのスレッドに渡す。
	 */
	static void sleep(unsigned int waitTimeForMilliSeconds) throw()
	{
		::Sleep(waitTimeForMilliSeconds);
	}

	/**
	 * スレッドの実行権を他へ渡す
	 * @exception InterruptedException スレッドインスタンスから
	 * abort()が呼ばれていた場合
	 */
	void static yield() throw()
	{
		WinThread::sleep(0);
	}

	/**
	 * 現在のスレッドのスレッド識別子を返す
	 * @return スレッド識別子
	 */
	static const WinThread::thread_id_t self() throw()
	{
		return GetCurrentThreadId();
	}

	/**
	 * スレッドオブジェクトの識別子を返す
	 * @return スレッド識別子
	 */
	const WinThread::thread_id_t getThreadId() throw()
	{
		return this->threadId;
	}

	/**
	 * スレッドの終了待機
	 * @param waitTime スレッドの終了待機時間(ミリ秒)。デフォルトは無限
	 * @exception TimeoutException 待機時間を過ぎてもスレッドが終了し
	 * なかった場合
	 * @exception ThreadExcpetion 何らかの異常が発生した場合
	 */
	virtual unsigned int join()
		throw(ThreadException, TimeoutException)
	{
		assert(this->threadHandle != NULL);

		DWORD result = WaitForSingleObject(this->threadHandle, INFINITE);
		switch(result)
		{
		case WAIT_OBJECT_0:
			unsigned retValue;
			GetExitCodeThread(this->threadHandle, (DWORD*)&retValue);
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
	 * スレッドが例外を発行した場合の例外理由文字列を返す
	 * @return 例外理由
	 */
	std::string reason() const throw()
	{
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

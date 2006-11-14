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
 * Win32ベーススレッドクラス
 *
 * 継承し、仮想関数 run()をオーバーライドするか、
 * Runnableインタフェースを実装したクラスを用意して
 * コンストラクタに食わせてください。
 * @todo 各ロック処理のポリシー化。thisポインタロックにしたほうが効率よさげ
 * @todo 状態検査とシグナルチックな中断処理への変更
 * @todo RunnableホルダーもテンプレートにしとけばSmartPointerとか
 * AutoPtrとか使えるなぁ・・・
 */
class WinThread : public Runnable
{
	friend class ThreadGroup;

public:
	enum {
		/// 例外により停止をあらわす定数
		abort_by_exception = 0xffffffff,
		/// 親からのリクエストにより中断を表す定数
		abort_by_parent = 0xfffffffe
	};

	/**
	 * スレッド識別子型
	 */
	typedef unsigned thread_id_t;

private:
	/**
	 * 状態定数
	 */
	enum RunningStatus
	{
		created,
		running,
		suspend,
		stop
	};

	/**
	 * 実行対象
	 */
	Runnable* runningTarget;

	/**
	 * スレッド状態
	 */
	volatile RunningStatus status;

	/**
	 * スレッドハンドル
	 */
	HANDLE threadHandle;

	/**
	 * スレッド識別子
	 */
	thread_id_t ThreadId;

	/**
	 * 処理例外伝達用ポインタ
	 */
	ThreadException* transporter;

	/**
	 * 停止用フラグ
	 */
	volatile bool isAborting;

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

		if (isAbort())
		{
			isAborting = false;
			throw InterruptedException();
		}
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
	Runnable* getRunningTarget() const throw()
	{
		return runningTarget;
	}

	/**
	 * 新しいエントリポイントの設定
	 * @param runnable 新しいエントリポイント
	 * @note ロック機構備えてないので実行中に書き換えないよう注意。
	 * @todo 必要ならロック機構備えたほうがいいかも
	 */
	void setRunningTarget(Runnable* runnable) throw()
	{
		runningTarget = runnable;
	}

	/**
	 * システムコールバック用エントリポイント
	 * @todo クリティカルセクションロックを変更してロック影響範囲を
	 * もっと小さくする。
	 */
	virtual unsigned int callback() throw()
	{
		unsigned retValue = 0;

		if (this->runningTarget == NULL)
			this->runningTarget = this;

		{
			CriticalSection atomicOp(true);
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
			retValue = static_cast<unsigned>(abort_by_parent);
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
			CriticalSection atomicOp(true);
			this->runningTarget->dispose();
			this->status = stop;
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
		CriticalSection atomicOp(true);

		this->threadHandle = (HANDLE)_beginthreadex(
			NULL,
			0,
			(unsigned (__stdcall*)(void*))WinThread::CallbackDispatcher,
			this,
			createOnRun ? 0 : CREATE_SUSPENDED,
			&this->ThreadId);

		if (this->threadHandle == NULL ||
			this->threadHandle == INVALID_HANDLE_VALUE)
			throw ThreadException("Can not create thread.");

		status = created;
	}

	/**
	 * 停止判定
	 * @return 停止状態ならtrue
	 */
	bool isAbort() const throw()
	{
		return isAborting;
	}

public:
	/**
	 * デフォルトコンストラクタ
	 * @param createOnRun 作成と同時に実行開始するかを識別するフラグ
	 */
	WinThread(bool createOnRun = false) throw (ThreadException)
		: runningTarget(), status(), threadHandle(),
		  ThreadId(), transporter(NULL), isAborting(false)
	{
		runningTarget = this;
		create(createOnRun);
	}

	/**
	 * コンストラクタ
	 * @param runnableObject 実行エントリポイントオブジェクト
	 * @param createOnRun 作成と同時に実行開始するかを識別するフラグ
	 */
	WinThread(Runnable* runnable_,
			  bool createOnRun = false) throw (ThreadException)
		: runningTarget(runnable_), status(), threadHandle(),
		  ThreadId(), transporter(NULL), isAborting(false)
	{
		assert(runnable_ != NULL);
		create(createOnRun);
	}

	/**
	 * デストラクタ
	 */
	virtual ~WinThread() throw(ThreadException)
	{
		if (!(status == stop || status == created))
			throw ThreadException();

		assert(status == stop ||
			   status == created);

		if ((HANDLE)(this->threadHandle) != INVALID_HANDLE_VALUE ||
			this->threadHandle != 0)
			CloseHandle((HANDLE)this->threadHandle);

		if (transporter != NULL)
			delete transporter;
	}

	/**
	 * スレッドの実行
	 * @return レジュームレベル。0で実行開始、>0でサスペンド中
	 */
	virtual unsigned start() throw()
	{
		assert(this->threadHandle);

		CriticalSection atomicOp(true);
		assert(status == created ||
			status == suspend);

		status = running;

		DWORD resumeCount = ResumeThread((HANDLE)this->threadHandle);
		assert(resumeCount == 1 || resumeCount == 0);
		return resumeCount;
	}

	/**
	 * スレッドの実行
	 * @param entryPoint 実行場所を持つオブジェクトのポインタ
	 * @return レジュームレベル。0で実行開始、>0でサスペンド中
	 */
	virtual unsigned start(Runnable* entryPoint) throw()
	{
		assert(this->threadHandle);

		CriticalSection atomicOp(true);
		this->setRunningTarget(entryPoint);

		return start();
	}

	/**
	 * スレッドの実行状態の取得
	 * @return true: 実行又はサスペンド中, false: 停止中
	 */
	bool isRunning() throw()
	{
		CriticalSection atomicOp(true);
		if (status == running || 
			status == suspend)
			return true;
		else
			return false;
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
	void yield() throw(InterruptedException)
	{
		if (isAbort())
			throw InterruptedException();

		WinThread::sleep(0);
	}

	/**
	 * スレッドの実行を中止する。
	 * 停止したスレッドは内部処理でyield()を呼び出したときに
	 * InterruptedExceptionが発行される。
	 * @todo 停止処理Methodの増量
	 */
	void abort() throw()
	{
		assert(this->threadHandle != NULL);

		CriticalSection atmicOp(true);
		isAborting = true;
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
		return this->ThreadId;
	}

	/**
	 * スレッドの終了待機
	 * @param waitTime スレッドの終了待機時間(ミリ秒)。デフォルトは無限
	 * @exception TimeoutException 待機時間を過ぎてもスレッドが終了し
	 * なかった場合
	 * @exception ThreadExcpetion 何らかの異常が発生した場合
	 */
	virtual unsigned join(DWORD waitTime = INFINITE)
		throw(ThreadException, TimeoutException)
	{
		assert(this->threadHandle != NULL);

		DWORD result = WaitForSingleObject(this->threadHandle, waitTime);
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
		assert(status == stop);

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

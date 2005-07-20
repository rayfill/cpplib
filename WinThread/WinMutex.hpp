#ifndef WINMUTEX_HPP_
#define WINMUTEX_HPP_

#include <windows.h>
#include <cassert>
#include <string>

/**
 * 排他操作オブジェクト
 */
class WinMutex {
private:
	/**
	 * 排他オブジェクト名
	 */
	const std::string MutexName;
	
	/**
	 * 排他オブジェクトハンドル
	 */
	HANDLE hMutex;

	/**
	 * コピー防止用
	 */
	WinMutex& operator=(const WinMutex&);

	/**
	 * コピー防止用
	 */
	WinMutex(WinMutex&);

public:
	/**
	 * コンストラクタ
	 * @arg createOnLock 作成とロックを同時に行うかのフラグ
	 */
	WinMutex(bool createOnLock = true) throw()
		: MutexName("WinMutex"), hMutex()
	{
		hMutex = CreateMutex(NULL, TRUE, MutexName.c_str());
		if (hMutex == NULL) 
		{
			// TODO: change error handling.
			if (GetLastError() == ERROR_ALREADY_EXISTS)
				OutputDebugString("Mutex already exists.");

			if (createOnLock != false)
				lock();
		}
	}

	/**
	 * コンストラクタ
	 * @arg MutexName_ 排他オブジェクト名
	 * @arg createOnLock 作成とロックを同時に行うかのフラグ
	 */
	WinMutex(const char* MutexName_, bool createOnLock = true) throw()
		: MutexName(MutexName_), hMutex()
	{
		assert(MutexName.length() < MAX_PATH);

		hMutex = CreateMutex(NULL, TRUE, MutexName.c_str());
		if (hMutex == NULL) 
		{
			// TODO: エラーハンドリングの実装
			if (GetLastError() == ERROR_ALREADY_EXISTS)
				OutputDebugString("Mutex already exists.");

			if (createOnLock != false)
				lock();
		}
	}

	/**
	 * 排他オブジェクトによるロック操作
	 */
	void lock() throw()
	{
		WaitForSingleObject(hMutex, INFINITE);
	}

	/**
	 * タイムアウト付き排他オブジェクトによるロック操作
	 * @arg WaitTime タイムアウト時間(Millisecond単位)
	 */
	bool lock(unsigned long WaitTime) throw()
	{
		return WaitForSingleObject(hMutex, WaitTime) != WAIT_TIMEOUT;
	}

	/**
	 * ロック検査
	 * @return ロックされていればtrue, されていなければfalse.
	 */
	bool isLock() throw()
	{
		return WaitForSingleObject(hMutex, 0) != WAIT_OBJECT_0;
	}

	/**
	 * デストラクタ
	 */
	virtual ~WinMutex()
	{
		if (hMutex != NULL) {
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
		}
	}
};

typedef WinMutex Mutex;

#endif /* WINMUTEX_HPP_ */

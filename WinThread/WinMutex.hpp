#ifndef WINMUTEX_HPP_
#define WINMUTEX_HPP_

#include <windows.h>
#include <cassert>
#include <string>

/**
 * Win32排他操作用オブジェクト。
 */
class WinMutex 
{
private:
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

	/**
	 * ミューテックス作成ヘルパ
	 * @param name ミューテックス名。NULLの場合、無名ミューテックスになる。
	 * @param createOnLock 作成後に所有権を取得するかを示すフラグ。
	 * @return 作成されたミューテックスのハンドル。NULLの場合はミューテックス
	 * が作成できなかったことを表す。
	 */
	static HANDLE createMutex(const char* name) throw()
	{
		return  CreateMutex(NULL, FALSE, name);
	}

public:
	/**
	 * コンストラクタ
	 * 無名ミューテックスを作成する。ハンドルでの共有又はオブジェクト
	 * を受け渡しての共有で使用する。
	 * @param createOnLock 作成とロックを同時に行う
	 */
	explicit WinMutex()
		: hMutex()
	{
		hMutex = createMutex(NULL);
	}

	/**
	 * ロック開放
	 */
	void unlock() throw()
	{
		ReleaseMutex(hMutex);
	}

	/**
	 * 排他オブジェクトによるロック操作
	 */
	void lock() throw()
	{
		WaitForSingleObject(hMutex, INFINITE);
	}

	/**
	 * ロック取得テスト
	 * @return ロックが取得できた場合、true
	 */
	bool tryLock() throw()
	{
		DWORD result = WaitForSingleObject(hMutex, 0);
		return (result == WAIT_OBJECT_0);
	}

	/**
	 * デストラクタ
	 */
	virtual ~WinMutex()
	{}
};

typedef WinMutex Mutex;

#endif /* WINMUTEX_HPP_ */

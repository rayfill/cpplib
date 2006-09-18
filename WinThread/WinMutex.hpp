#ifndef WINMUTEX_HPP_
#define WINMUTEX_HPP_

#include <windows.h>
#include <cassert>
#include <string>

/**
 * Win32排他操作用オブジェクト。
 * @todo コピーコンストラクタをどうするかを決める。DuplicateHandleでコ
 * ピーはできるけど、ProcessHandleをどうするか・・・。
 * もともとプロセス間で無名Mutexを共有するためみたいだし無くてもいいか
 * な、とは思うが・・・
 */
class WinMutex {
private:
	/**
	 * 排他オブジェクト名
	 * @note toString()メソッドとか
	 * operator<<(std::ostream& out, const WinMutex& self)メソッド
	 * 用意しないんだったらいらないかも。
	 */
	const char* MutexName;
	
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
	 * ロックレベル
	 */
	int lockCount;

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

protected:

	/**
	 * 現在のロックレベルを取得する。
	 * @return ロックのネストレベル。1以上で所有している。
	 * 0で所有権を持っていない。
	 */
	int getLockLevel() const throw()
	{
		assert(lockCount >= 0);
		return lockCount;
	}

public:
	/**
	 * コンストラクタ
	 * 無名ミューテックスを作成する。ハンドルでの共有又はオブジェクト
	 * を受け渡しての共有で使用する。
	 * @param createOnLock 作成とロックを同時に行う
	 */
	explicit WinMutex(bool createOnLock = true) throw()
			: MutexName(NULL), hMutex(), lockCount()
	{
		hMutex = createMutex(MutexName);
		if (hMutex != NULL &&
			createOnLock != false)
			lock();
	}

	/**
	 * コンストラクタ
	 * @param MutexName_ 排他オブジェクト識別名。
	 * 同じ名前のMutex同士で排他制御される
	 * @param createOnLock 作成とロックを同時に行う
	 */
	explicit WinMutex(const char* MutexName_, bool createOnLock = true) throw()
			: MutexName(MutexName_), hMutex(), lockCount()
	{
		assert(MutexName != NULL);
		assert(std::string(MutexName).length() < MAX_PATH);
		assert(std::string(MutexName).find('\\') == std::string::npos);

		hMutex = createMutex(MutexName);
		if (hMutex != NULL &&
			createOnLock != false)
			lock();
	}

	/**
	 * ロック開放
	 */
	int unlock() throw()
	{
		assert(lockCount > 0);

		volatile int result = --lockCount;
		ReleaseMutex(hMutex);

		assert(result >= 0);
		return result;
	}

	/**
	 * 強制的なロック開放
	 */
	void forceUnlock() throw()
	{
		while(unlock() > 0);
	}

	/**
	 * 排他オブジェクトによるロック操作
	 * @note ロックを掛けた分の回数分だけunlockを実施しなければならない。
	 * コンストラクタ時のロックは明示的に開放しなくても良い。
	 */
	bool lock() throw()
	{
		lock(INFINITE);

		assert(lockCount > 0);

		return lockCount != 0;
	}

	/**
	 * タイムアウト付き排他オブジェクトによるロック操作
	 * @param WaitTime タイムアウト時間(Millisecond単位)
	 * @return ロックが取得できた場合、true
	 */
	bool lock(unsigned long WaitTime) throw()
	{
		bool result = WaitForSingleObject(hMutex, WaitTime) != WAIT_TIMEOUT;
		if (result)
			++lockCount;

		return result;
	}

	/**
	 * ロック検査
	 * @return ロックされていればtrue, されていなければfalse.
	 */
	bool isLock() throw()
	{
		return lockCount > 0;
	}

	/**
	 * デストラクタ
	 */
	virtual ~WinMutex()
	{
		if (hMutex != NULL) {
			if (isLock())
				forceUnlock();

			CloseHandle(hMutex);
			hMutex = NULL;
		}

		assert(getLockLevel() == 0);
	}
};

typedef WinMutex Mutex;

#endif /* WINMUTEX_HPP_ */

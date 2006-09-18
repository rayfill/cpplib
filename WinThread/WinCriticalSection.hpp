#ifndef WINCRITICALSECTION_HPP_
#define WINCRITICALSECTION_HPP_

#include <windows.h>
#include <util/Singleton.hpp>
#include <cassert>

/**
 * Win32用CriticalSectionクラス
 * @todo グローバルで単一のクリティカルセクションとなってしまうのでコ
 * ンストラクタから文字列でも食ってセクションの識別を行う実装に変更し
 * たい･･･ってそれやったらプロセスローカルMutexになっちまうな・・・素
 * 直にstatic宣言でお茶濁すか・・・ってそれだとデストラクタが動かん
 * か・・・
 */
class WinCriticalSection
{
private:
	/**
	 * クリティカルセクションハンドルマネージャ
	 */
	class InnerCriticalSectionObject
	{
	private:
		/**
		 * クリティカルセクションハンドル
		 */
		CRITICAL_SECTION sectionHandle;

	public:
		/**
		 * コンストラクタ
		 */
		InnerCriticalSectionObject() throw()
			: sectionHandle()
		{
			InitializeCriticalSection(&sectionHandle);
		}

		/**
		 * デストラクタ
		 */
		~InnerCriticalSectionObject() throw()
		{
			DeleteCriticalSection(&sectionHandle);
		}
		
		/**
		 * セクションハンドルの取得
		 * @return セクションハンドルの取得
		 */
		LPCRITICAL_SECTION get() throw()
		{
			return &sectionHandle;
		}
	};
	
	typedef Singleton<InnerCriticalSectionObject> CriticalSectionObject;

	/**
	 * セクションロック判別
	 */
	bool isLocked;

	/**
	 * non copyable
	 */
	WinCriticalSection(const WinCriticalSection&) throw() {}

public:
	/**
	 * コンストラクタ
	 * @param createOnLock 作成時にロックを行うかどうか
	 */
	WinCriticalSection(bool createOnLock = true) throw()
		: isLocked(false)
	{
		if (createOnLock != false)
			lock();
	}
	
	/**
	 * デストラクタ
	 * ロックしていた場合、解除する
	 */
	virtual ~WinCriticalSection() throw()
	{
		if (isLock())
			unlock();
	}

	/**
	 * セクションロック
	 */
	void lock() throw()
	{
		assert(isLocked == false);

		EnterCriticalSection(CriticalSectionObject::get()->get());
		isLocked = true;
	}
	
	/**
	 * セクションアンロック
	 */
	void unlock() throw()
	{
		assert(isLocked != false);

		isLocked = false;
		LeaveCriticalSection(CriticalSectionObject::get()->get());
	}

	BOOL TryEnterCriticalSection(LPCRITICAL_SECTION);
	/**
	 * セクションのロック(非ブロック)
	 * @return ロックされているかどうか. ロックされている: true, ロッ
	 * クされていない: false
	 * @note なんでかWindowsベースのコンパイラだと定義が無いって言われる･･･
	 */
	bool tryLock() throw()
	{
		assert(isLocked == false);

		BOOL result =
			TryEnterCriticalSection(CriticalSectionObject::get()->get());

		if (result == FALSE)
			return false;

		isLocked = true;
		return true;
	}

	/**
	 * セクション内にいるかの判定
	 * @return セクション内にいるならtrue, でなければfalse.
	 */
	bool isLock() const throw()
	{
		return isLocked;
	}
};

typedef WinCriticalSection CriticalSection;

#endif /* WINCRITICALSECTION_HPP_ */

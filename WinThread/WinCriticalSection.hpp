#ifndef WINCRITICALSECTION_HPP_
#define WINCRITICALSECTION_HPP_

#include <windows.h>
#include <cassert>

/**
 * Win32用CriticalSectionクラス
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
			::InitializeCriticalSection(&sectionHandle);
		}

		/**
		 * デストラクタ
		 */
		~InnerCriticalSectionObject() throw()
		{
			::DeleteCriticalSection(&sectionHandle);
		}
		
		/**
		 * セクションハンドルの取得
		 * @return セクションハンドルの取得
		 */
		LPCRITICAL_SECTION get() throw()
		{
			return &sectionHandle;
		}
	} section;
	
	/**
	 * セクションロック判別
	 */
	bool isLocked;

	/**
	 * non copyable
	 */
	WinCriticalSection(const WinCriticalSection&);
	WinCriticalsection& operator=(const WinCriticalSection&);
public:
	/**
	 * コンストラクタ
	 */
	WinCriticalSection() throw()
		: isLocked(false)
	{}
	
	/**
	 * デストラクタ
	 */
	virtual ~WinCriticalSection() throw()
	{}

	/**
	 * セクションロック
	 */
	void lock() throw()
	{
		assert(isLocked == false);

		EnterCriticalSection(section.get());
		isLocked = true;
	}
	
	/**
	 * セクションアンロック
	 */
	void unlock() throw()
	{
		assert(isLocked != false);

		isLocked = false;
		LeaveCriticalSection(section.get());
	}

	//BOOL TryEnterCriticalSection(LPCRITICAL_SECTION);
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
			::TryEnterCriticalSection(section.get());

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

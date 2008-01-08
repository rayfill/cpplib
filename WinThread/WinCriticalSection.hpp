#ifndef WINCRITICALSECTION_HPP_
#define WINCRITICALSECTION_HPP_

#include <windows.h>
#include <winbase.h>
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
	 * non copyable
	 */
	WinCriticalSection(const WinCriticalSection&);
	WinCriticalSection& operator=(const WinCriticalSection&);
public:
	/**
	 * コンストラクタ
	 */
	WinCriticalSection() throw(): section()
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
		EnterCriticalSection(section.get());
	}
	
	/**
	 * セクションアンロック
	 */
	void unlock() throw()
	{
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
		BOOL result =
			::TryEnterCriticalSection(section.get());

		if (result == FALSE)
			return false;

		return true;
	}
};

typedef WinCriticalSection CriticalSection;

#endif /* WINCRITICALSECTION_HPP_ */

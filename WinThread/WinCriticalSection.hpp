#ifndef WINCRITICALSECTION_HPP_
#define WINCRITICALSECTION_HPP_

#include <windows.h>
#include <util/Singleton.hpp>
#include <cassert>

#include <iostream>

class WinCriticalSection
{
private:
	class InnerCriticalSectionObject
	{
	private:
		CRITICAL_SECTION sectionHandle;
	public:
		InnerCriticalSectionObject() throw()
			: sectionHandle()
		{
			InitializeCriticalSection(&sectionHandle);
		}

		~InnerCriticalSectionObject() throw()
		{
			DeleteCriticalSection(&sectionHandle);
		}
		
		LPCRITICAL_SECTION get() throw()
		{
			return &sectionHandle;
		}
	};
	
	typedef Singleton<InnerCriticalSectionObject> CriticalSectionObject;

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

	/**
	 * セクションのロック(非ブロック)
	 * @return ロックされているかどうか. ロックされている: true, ロッ
	 * クされていない: false
	 */
	bool tryLock() throw()
	{
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

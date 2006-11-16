#ifndef TWINLOCK_HPP_
#define TWINLOCK_HPP_

#include <Thread/Thread.hpp>
/**
 * Threadの親子で同期をとるためのゲートクラス
 */
template <typename LockedObject>
class TwinLock
{
private:
	LockedObject parent;
	LockedObject child;

	/**
	 * 親ロックの取得
	 */
	void parentLock()
	{
		parent.lock();
	}

	/**
	 * 親ロックの開放
	 */
	void parentUnlock()
	{
		parent.unlock();
	}

	/**
	 * 子ロックの取得チェック
	 * @return 子のロック状況。取得されていればtrue
	 */
	bool isChildLocked()
	{
		return LockCheck<LockedObject>(child)();
	}

	/**
	 * 親ロックの取得チェック
	 * @return 親のロック状況。取得されていればtrue
	 */
	bool isParentLocked()
	{
		return LockCheck<LockedObject>(parent)();
	}

	/**
	 * 子の同期オブジェクトロック
	 */
	void childLock()
	{
		child.lock();
	}

	/**
	 * 子の同期オブジェクトアンロック
	 */
	void childUnlock()
	{
		child.unlock();
	}

public:
	/**
	 * コンストラクタ
	 * @note 親側でコンストラクトすること
	 */
	TwinLock():
		parent(), child()
	{
		parent.lock();
	}

	/**
	 * デストラクタ
	 */
	~TwinLock()
	{
		parent.unlock();
	}

	/**
	 * 親の同期待ちエントリ
	 */
	void waitFromParent()
	{
		// 1st. l w
		while (!isChildLocked())
			Thread::yield();

		// 2nd. r or
		parentUnlock();

		// 3rd.   w
		childLock();

		// 4th. w l
		parentLock();

		// 5th.
		childUnlock();
	}

	/**
	 * この同期待ちエントリ
	 */
	void waitFromChild()
	{
		// 2nd.w l
		parentLock();

		// 3rd.l r
		childUnlock();

		// 4th l or
		parentUnlock();

		// 5th.or or
		childLock();
	}

	void childPrepare()
	{
		childLock();
	}

	void childDispose()
	{
		childUnlock();
	}
};

#endif /* TWINLOCK_HPP */


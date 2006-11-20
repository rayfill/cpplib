#ifndef SCOPEDLOCK_HPP_
#define SCOPEDLOCK_HPP_

/**
 * スコープ束縛ロック
 * @note RAIIとしてコンストラクト時にロックを取得し、
 * デストラクト時にロックを開放する。
 */
template <typename LockedObject>
class ScopedLock
{
private:
	/**
	 * ロックオブジェクトへの参照
	 */

	LockedObject* object;

	/**
	 * ロックへルパ
	 */
	void lock()
	{
		object->lock();
	}

	/**
	 * アンロックヘルパ
	 */
	void unlock()
	{
		object->unlock();
	}

	ScopedLock(const ScopedLock&);
	ScopedLock& operator=(const ScopedLock&);
public:
	/**
	 * コンストラクタ
	 * @param target ロック対象のオブジェクト
	 */
	ScopedLock(LockedObject& target):
		object(&target)
	{
		lock();
	}

	/**
	 * デストラクタ
	 */
	~ScopedLock()
	{
		unlock();
	}
};

#endif /* SCOPEDLOCK_HPP_ */

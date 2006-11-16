#ifndef SYNCOPERATOR_HPP_
#define SYNCOPERATOR_HPP_

/**
 * ロック可能オブジェクトのロック判定
 */
template <typename LockedObject>
class LockCheck
{
private:
	/**
	 * ロックオブジェクト参照
	 */
	LockedObject* object;

	/**
	 * ロック所有権
	 */
	bool isLockOwner;

	/**
	 * ロックチャレンジ
	 * @return 取得できた(ロックされていなかった)場合true
	 */
	bool tryLock()
	{
		isLockOwner = object->tryLock();

		return isLockOwner;
	}

	/**
	 * ロック開放
	 * @note ロック所有権を持たない場合、何もしない
	 */
	void release()
	{
		if (isLockOwner != false)
			object->unlock();
	}
public:

	/**
	 * コンストラクタ
	 * @param target 検査対象のオブジェクト
	 */
	LockCheck(LockedObject& target):
		object(&target)
	{}

	/**
	 * デストラクタ
	 */
	~LockCheck()
	{
		release();
	}

	/**
	 * ロック判定
	 * @return 他のスレッドがロックオブジェクトの所有権を持っていた場合、true
	 */
	bool operator()()
	{
		return !tryLock();
	}
};

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

#endif /* SYNCOPERATOR_HPP_ */

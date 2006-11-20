#ifndef LOCKCHECK_HPP_
#define LOCKCHECK_HPP_

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

	LockCheck(const LockCheck&);
	LockCheck& operator=(const LockCheck&);
public:

	/**
	 * コンストラクタ
	 * @param target 検査対象のオブジェクト
	 */
	LockCheck(LockedObject& target):
		object(&target), isLockOwner()
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

#endif /* LOCKCHECK_HPP_ */

#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#include <stdexcept>
#include <Thread/Mutex.hpp>
#include <Thread/SyncOperator.hpp>

#include <map>

/**
 * 再入ポリシー
 */
class SingleThreadPolicy
{
public:
	void lock()
	{}
	void unlock()
	{}
};

/**
 * ロック対象が
 */
class MultiThreadPolicy
{
	typedef ScopedLock<Mutex> Locker;
	Mutex mutex;
	Locker* locker;
public:
	void lock()
	{
		locker = new Locker(mutex);
	}

	void unlock()
	{
		delete locker;
	}
};

/**
 * シングルトンオブジェクト
 */
template <typename ResultClass,
	typename Policy = SingleThreadPolicy>
class Singleton
{
private:
	Singleton();
	Singleton(const Singleton& );
public:

	/**
	 * シングルトンオブジェクトの取得
	 */
	static ResultClass* get()
	{
		/**
		 * @todo たしかstatic変数の初期化に関する
		 * rase conditionの規定はgcc以外なかったな・・・
		 */
		static Policy policy;

		policy.lock();
		static ResultClass body;
		policy.unlock();

		return &body;
	}
};

/**
 * キー値にマッピングされたSingletonオブジェクト
 * @param KeyType キーの型
 * @param ResultClass バインドされた値の型
 * @param Policy スレッド排他ポリシー
 */
template <typename KeyType,
		  typename  ResultClass,
		  typename Policy = SingleThreadPolicy>
class SingletonMapper 
{
	friend class SingletonMapperTest;

private:
	SingletonMapper();
	SingletonMapper(const SingletonMapper& );

	typedef std::map<KeyType, ResultClass> mapper_t;

	/**
	 * マップオブジェクトの取得
	 * @return マップオブジェクトの参照
	 */
	static ResultClass* getMappedObject(const KeyType& key)
	{
		static mapper_t mapObject;
		
		return &(mapObject[key]);
	}

public:
	/**
	 * シングルトンオブジェクトの取得
	 * @return マッピングされたシングルトンオブジェクト
	 * @param key オブジェクトにマップしたキーの値
	 */ 
	static ResultClass* get(const KeyType& key)
	{
		Policy policy;
		policy.lock();
		
		ResultClass* result = getMappedObject(key);

		policy.unlock();

		return result;
	}
};



#endif /* SINGLETON_HPP_ */

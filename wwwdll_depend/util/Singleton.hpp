#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#include <stdexcept>
#include <Thread/Mutex.hpp>

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

class MultiThreadPolicy
{
	Mutex* mutex;
public:
	void lock()
	{
		mutex = new Mutex("Singleton Creator");
	}
	void unlock()
	{
		delete mutex;
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
		Policy policy;
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

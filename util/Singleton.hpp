#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#include <stdexcept>
#include <Thread/Mutex.hpp>
#include <Thread/SyncOperator.hpp>

#include <map>

/**
 * �ē��|���V�[
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
 * ���b�N�Ώۂ�
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
 * �V���O���g���I�u�W�F�N�g
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
	 * �V���O���g���I�u�W�F�N�g�̎擾
	 */
	static ResultClass* get()
	{
		/**
		 * @todo ������static�ϐ��̏������Ɋւ���
		 * rase condition�̋K���gcc�ȊO�Ȃ������ȁE�E�E
		 */
		static Policy policy;

		policy.lock();
		static ResultClass body;
		policy.unlock();

		return &body;
	}
};

/**
 * �L�[�l�Ƀ}�b�s���O���ꂽSingleton�I�u�W�F�N�g
 * @param KeyType �L�[�̌^
 * @param ResultClass �o�C���h���ꂽ�l�̌^
 * @param Policy �X���b�h�r���|���V�[
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
	 * �}�b�v�I�u�W�F�N�g�̎擾
	 * @return �}�b�v�I�u�W�F�N�g�̎Q��
	 */
	static ResultClass* getMappedObject(const KeyType& key)
	{
		static mapper_t mapObject;
		
		return &(mapObject[key]);
	}

public:
	/**
	 * �V���O���g���I�u�W�F�N�g�̎擾
	 * @return �}�b�s���O���ꂽ�V���O���g���I�u�W�F�N�g
	 * @param key �I�u�W�F�N�g�Ƀ}�b�v�����L�[�̒l
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

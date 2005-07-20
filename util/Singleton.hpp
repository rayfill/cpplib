#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#include <stdexcept>
#include <Thread/Mutex.hpp>

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
	static ResultClass* get() throw(std::bad_alloc)
	{
		Policy policy;
		policy.lock();
		static ResultClass body;
		policy.unlock();

		return &body;
	}
};

#endif /* SINGLETON_HPP_ */

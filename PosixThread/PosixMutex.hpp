#ifndef POSIXMUTEX_HPP_
#define POSIXMUTEX_HPP_
#include <pthread.h>
#include <errno.h>

#ifndef NDEBUG
#include <stdexcept>
#endif /* NDEBUG */

/**
 * POSIXバージョン排他制御クラス
 */
class PosixMutex
{
	friend class PosixEvent;

private:
	/**
	 * 排他制御実態
	 */
	mutable pthread_mutex_t MutexId;

	/**
	 * コピー防止用
	 */
	PosixMutex& operator=(const PosixMutex&);

	/**
	 * コピー防止用
	 */
	PosixMutex& PosixPosixMutex(const PosixMutex&);

#ifndef NDEBUG
	mutable int lock_count;
	mutable pthread_t thread_id;
#endif

public:
	/**
	 * コンストラクタ
	 */
	PosixMutex(): MutexId()
#ifndef NDEBUG
				, lock_count(0), thread_id()
#endif
	{
		pthread_mutex_init(&this->MutexId, NULL);
	}

	/**
	 * デストラクタ
	 */
	~PosixMutex()
	{
		pthread_mutex_destroy(&this->MutexId);
	}

	/**
	 * ロック
	 * @note デバッグモード時はデッドロックしていた場合、
	 * std::logic_error例外をthrowする。
	 * @throw std::logic_error デバッグモードでコンパイルし、
	 * かつデッドロックしていた場合
	 */
	void lock()
	{
#ifndef NDEBUG
		if (thread_id == pthread_self() && lock_count > 0)
			throw std::logic_error("bad lock count.");
			
		int result =
#endif /* NDEBUG */
		pthread_mutex_lock(&this->MutexId);
#ifndef NDEBUG
		if (lock_count != 0)
			throw std::logic_error("bad lock count.");

		++lock_count;
		thread_id = pthread_self();
		if (lock_count != 1)
			throw std::logic_error("bad lock count.");

		if (EDEADLK == result)
			throw std::logic_error("mutex is dead locking...");
#endif /* NDEBUG */
	}

	bool tryLock()
	{
		return !(EBUSY == pthread_mutex_trylock(&this->MutexId));
	}
	/**
	 * アンロック
	 */
	void unlock()
	{
#ifndef NDEBUG
		--lock_count;
		thread_id = 0;
		if (lock_count != 0)
			throw std::logic_error("bad lock count.");
#endif
		
		pthread_mutex_unlock(&this->MutexId);
	}
};

typedef PosixMutex Mutex;
#endif /* POSIXMUTEX_HPP_ */

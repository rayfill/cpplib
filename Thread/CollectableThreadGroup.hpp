#ifndef RECOLLECTABLETHREADGROUP_HPP_
#define RECOLLECTABLETHREADGROUP_HPP_

#include <set>
#include <cassert>
#include <Thread/ThreadGroup.hpp>
#include <Thread/Mutex.hpp>
#include <util/Notification.hpp>

/**
 * 終了Threadの回収可能なThreadGroupクラス
 */
class CollectableThreadGroup :
	public ThreadGroup, public Observer
{
private:
	typedef std::set<Thread::thread_id_t> CollectableThreadIds;

	/**
	 * 回収可能となったThread ID を保持するコンテナ
	 */
	CollectableThreadIds collectable;

	/**
	 * ロックオブジェクト
	 */
	Mutex mutex;

	/**
	 * 通知ハンドラ
	 * @see Observable
	 */
	virtual void notify(Observable* notifier)
	{
		Thread* thread = dynamic_cast<Thread*>(notifier);
		assert(thread != NULL);

		collectable.insert(thread->getThreadId());
	}
	
public:
	/**
	 * デフォルトコンストラクタ
	 */
	CollectableThreadGroup() throw()
	: ThreadGroup(), Observer(), collectable(), mutex()
	{}

	/**
	 * デストラクタ
	 */
	virtual ~CollectableThreadGroup() throw()
	{}

	/**
	 * 回収可能なThreadのみを回収
	 * @exception ThreadException join()したThreadが例外終了した場合
	 */
	void join_recollectable() throw(ThreadException)
	{
		CollectableThreadIds::iterator itor;
		itor = collectable.begin();
		while(itor != collectable.end())
		{
			Thread* thread = this->detach(*itor);
			collectable.erase(itor++);
			thread->join();
			delete thread;
		}
	}

	/**
	 * 新しいスレッドの登録
	 * @param thread 登録する Thread クラスのポインタ
	 * @exception ThreadException 引数のthreadが不完全なスレッドの場合
	 * 発行される
	 */
	virtual void attach(Thread* thread) throw(ThreadException)
	{
		Observable* observable = dynamic_cast<Observable*>(thread);
		assert(observable != NULL);

		ScopedLock<Mutex> lock(mutex);
		observable->attachObserver(static_cast<Observer*>(this));

		ThreadGroup::attach(thread);
	}

	/**
	 * スレッドの登録解除
	 * @param id 登録解除するスレッドのThreadID
	 * @return 登録解除された Thread オブジェクトのポインタ
	 */
	virtual Thread* detach(const Thread::thread_id_t id) throw()
	{
		Thread* thread = ThreadGroup::detach(id);
		assert(thread != NULL);

		ScopedLock<Mutex> lock(mutex);
		Observable* observable = dynamic_cast<Observable*>(thread);
		assert(observable != NULL);

		observable->detachObserver(static_cast<Observer*>(this));

		return thread;
	}
};

/**
 * 回収可能なスレッドのテンプレート。ワーカーメソッド(run())をオーバー
 * ライドしたクラスを引数にして実体化し、利用する。
 */
template <typename BaseThreadClass>
class CollectableThread
	: public BaseThreadClass
{
protected:
	/**
	 * 後始末(C++だからfinal修飾ないけど継承しないでね)
	 */ 
	virtual void dispose() throw()
	{
		assert(dynamic_cast<Observable*>(this) != 0);
		dynamic_cast<Observable*>(this)->update();
	}
public:

	/**
	 * コンストラクタ
	 * コレクタ(CollectableThreadGroup)に登録しないまま終了する可
	 * 能性があるため、作成と同時に実行はできない
	 */
	CollectableThread() throw():
	BaseThreadClass(false)
	{
	}

	/**
	 * デストラクタ
	 */
	virtual ~CollectableThread() throw()
	{
	}
};
	
#endif /* RECOLLECTABLETHREADGROUP_HPP_ */

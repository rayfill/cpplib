#ifndef THREADMANAGER_HPP_
#define THREADMANAGER_HPP_

template <typename ThreadType>
class ThreadManager
{
public:
	typedef ThreadType thread_t;

public:
	ThreadManager()
	{}

	virtual ~ThreadManager()
	{}

	/**
	 * managerによって管理されたthreadを返す
	 * @return 新しいThreadオブジェクト(実行状態は停止)
	 */
	virtual thread_t* getThread() = 0;

	/**
	 * 使い終わったthreadをmanagerに返却する
	 * @param thread 返却するthreadオブジェクトへのポインタ
	 */
	virtual void releaseThread(thread_t* thread) = 0;
};

#endif /* THREADMANAGER_HPP_ */

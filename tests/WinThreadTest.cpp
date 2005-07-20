#include <Thread/ThreadGroup.hpp>
#include <sstream>
#include <iostream>
#include <stdlib.h>

class TestException : public ThreadException
{
public:
	TestException() : ThreadException("TestException.")
	{}
	TestException(const char* reason) : ThreadException(reason)
	{}
};

class MyThread : public WinThread 
{
	virtual unsigned run() throw(ThreadException)
	{
		srand((int)this->self());
		for (int count = 0; count < 10; ++count) 
		{
			MyThread::sleep((int)(1000 * ((double)rand() / RAND_MAX)));
			{
				CriticalSection lock;

				std::cout << "ThreadId = " << this->self()
						  << ":member thread id = " << this->getThreadId()
						  << ":hello world." << std::endl;
			}
		}

		std::stringstream ss;
		ss << "ThreadId = " << this->self()
		   << ": member threadId = " << this->getThreadId()
		   << std::ends;

		if (this->self() % 3 == 2)
			throw TestException(ss.str().c_str());

		return 1;
	}
};

int main() {
  ThreadGroup tg;
  WinThread* pThread;
  tg.attach(new MyThread());
  tg.attach(new MyThread());
  tg.attach(new MyThread());
  tg.attach(pThread = new MyThread());
  tg.attach(new MyThread());
  tg.attach(new MyThread());
  tg.attach(new MyThread());
  tg.attach(new MyThread());
  tg.attach(new MyThread());
  
  
  tg.start_all();
  {
	CriticalSection lock;
	std::cout << "start threads." << std::endl;
  }

  tg.detach(pThread->getThreadId());
  pThread->join();

  tg.force_join_all();
  {
	CriticalSection lock;
	std::cout << "join threads." << std::endl;
  }

  std::cout << "thread end reasons" << std::endl;
  for(size_t threads = 0; threads != tg.count(); ++threads)
  {
	  std::cout << threads << ":" << tg[threads]->reason() << std::endl;
  }
  std::cout << "all joined threads." << std::endl;


  OutputDebugString("Program terminate.");
  return 0;
}


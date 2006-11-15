#include <Thread/Thread.hpp>
#include <Thread/CriticalSection.hpp>
#include <iostream>

class MyThread : public Thread
{
	virtual unsigned int run() throw(ThreadException)
	{

		for (unsigned int index = 0; index < 10; ++index)
		{
			CriticalSection cs;
			std::cout << "hello thread: " << this->getThreadId() << std::endl;
			this->sleep(this->getThreadId());
		}
		return this->getThreadId();
	}
};

int main() 
{
	MyThread t1;
	MyThread t2;
	MyThread t3;

	t1.start();
	t2.start();
	t3.start();
	std::cout << "t1 join " << std::endl;
	std::cout << "returned by " << t1.join() << std::endl;
	std::cout << "t1 joined " << std::endl;
	std::cout << "t2 join " << std::endl;
	std::cout << "returned by " << t2.join() << std::endl;
	std::cout << "t2 joined " << std::endl;
	std::cout << "t3 join " << std::endl;
	std::cout << "returned by " << t3.join() << std::endl;
	std::cout << "t3 joined " << std::endl;

	std::cout << "program terminated." << std::endl;
	return 0;
}

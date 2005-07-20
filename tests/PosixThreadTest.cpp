#include <iostream>
#include <stdio.h>
#include <PosixThread/PosixThread.hpp>

class MyThread : public PosixThread {
public:
  int run() {
    {
      Mutex lock;
      std::cerr << "thread id = " <<  this->self() << std::endl;
      std::cerr << "Hello world." << std::endl;
    }
    return 100;
  }
};

int main()
{
  MyThread thr;
  MyThread thr2;
  thr.create();
  thr2.create();
  
  {
    Mutex lock;
    std::cerr << "isRunning to " << thr.isRunning() << std::endl;
    std::cerr << "main thread" << std::endl;
  }
  int thread1Value = thr.join();
  int thread2Value = thr2.join();
  {
    Mutex lock;
    std::cerr << "thread1 returns to " << thread1Value << std::endl;
    std::cerr << "thread2 returns to " << thread2Value << std::endl;
    std::cerr << "isRunning to " << thr.isRunning() << std::endl;
  }
  return 0;
}

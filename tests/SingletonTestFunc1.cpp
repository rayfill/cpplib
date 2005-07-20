#include "SingletonTest.hpp"

SingletonTestClass* Func1()
{
	return TestSingleton::get();
}

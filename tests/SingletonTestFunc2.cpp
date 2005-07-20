#include "SingletonTest.hpp"

SingletonTestClass* Func2()
{
	return TestSingleton::get();
}

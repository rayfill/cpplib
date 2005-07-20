#include <util/Singleton.hpp>

class SingletonTestClass
{
public:
	SingletonTestClass()
	{
	}
	virtual ~SingletonTestClass()
	{
	}
};

typedef Singleton<SingletonTestClass> TestSingleton;

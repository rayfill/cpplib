#include <cppunit/extensions/HelperMacros.h>
#include <util/Compression/LZSS.hpp>

class WindowFunctionTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(WindowFunctionTest);
	CPPUNIT_TEST(WindowSizeTest);
	CPPUNIT_TEST(SlideTest);
	CPPUNIT_TEST(SlidableTest);
	CPPUNIT_TEST(IsSlidableTest);
	CPPUNIT_TEST_SUITE_END();
public:

	typedef WindowFunction<> SlideWindow;

	void WindowSizeTest()
	{
		WindowFunction<2048> instance((char*)1, (char*)3);
		CPPUNIT_ASSERT(instance.getMaxWindowSize() == 2048);
		CPPUNIT_ASSERT(instance.getCurrentWindowSize() == 1);
		instance.slide();
		CPPUNIT_ASSERT(instance.getCurrentWindowSize() == 2);

		WindowFunction<4096> instance2((char*)1, (char*)3);
		CPPUNIT_ASSERT(instance2.getMaxWindowSize() == 4096);
		CPPUNIT_ASSERT(instance2.getCurrentWindowSize() == 1);
		instance2.slide();
		CPPUNIT_ASSERT(instance2.getCurrentWindowSize() == 2);
	}

	void SlideTest()
	{
		{
			SlideWindow instance((char*)1, (char*)3);
			instance.slide();

			CPPUNIT_ASSERT(!instance.isSlidable());
			CPPUNIT_ASSERT_THROW(instance.slide(), std::out_of_range);
		}

		{
			SlideWindow instance((char*)1, (char*)5);
			instance.slide(1);
			instance.slide(2);
			CPPUNIT_ASSERT(!instance.isSlidable());
			CPPUNIT_ASSERT_THROW(instance.slide(2), std::out_of_range);
		}
		
	}

	void IsSlidableTest()
	{
		{
			SlideWindow instance((char*)1, (char*)3);
			CPPUNIT_ASSERT(instance.isSlidable());
			instance.slide();
			CPPUNIT_ASSERT(!instance.isSlidable());
		}

		{
			SlideWindow instance((char*)1, (char*)5);

			CPPUNIT_ASSERT(instance.isSlidable());
			CPPUNIT_ASSERT(instance.isSlidable(3));
			CPPUNIT_ASSERT(!instance.isSlidable(4));
		
			instance.slide(1);
			CPPUNIT_ASSERT(instance.isSlidable());
			CPPUNIT_ASSERT(instance.isSlidable(2));
			CPPUNIT_ASSERT(!instance.isSlidable(3));

			instance.slide(2);
			CPPUNIT_ASSERT(!instance.isSlidable());
			CPPUNIT_ASSERT(!instance.isSlidable(2));
			CPPUNIT_ASSERT(!instance.isSlidable(3));
		}
	}

	void SlidableTest()
	{
		
		{
			SlideWindow instance((char*)1, (char*)4096+2);
			CPPUNIT_ASSERT(instance.getSlidableSize() == 4094+2);
		}

		{
			SlideWindow instance((char*)1, (char*)4096+4);

			CPPUNIT_ASSERT(instance.getSlidableSize() == 4094+4);
			for (int count = 4093+4; count >= 0; --count)
			{
				instance.slide();
				CPPUNIT_ASSERT(instance.getSlidableSize() == 
							   static_cast<size_t>(count));
			}

			CPPUNIT_ASSERT(instance.getSlidableSize() == 0);
		}
		
	}
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( WindowFunctionTest );


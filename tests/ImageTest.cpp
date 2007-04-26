#include <cppunit/extensions/HelperMacros.h>
#include <Image/YCbCr.hpp>
#include <Image/Color.hpp>
#include <Image/ColorSpace.hpp>
#include <iostream>
#include <malloc.h>

class ImageTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(ImageTest);
	CPPUNIT_TEST(YCbCrTest);
	CPPUNIT_TEST(ColorSpaceTest);
	CPPUNIT_TEST_SUITE_END();

	size_t getVectorLength(const YCbCr& lhs, const YCbCr& rhs) const throw()
	{
		int yVectorDiff = ((int)lhs.Y - (int)rhs.Y);
		int cbVectorDiff = ((int)lhs.Cb - (int)rhs.Cb);
		int crVectorDiff = ((int)lhs.Cr - (int)rhs.Cr);
		return
			yVectorDiff * yVectorDiff +
			cbVectorDiff * cbVectorDiff +
			crVectorDiff * crVectorDiff;
	}

	size_t getVectorLength(const Color& lhs, const Color& rhs) const throw()
	{
		int rVectorDiff = ((int)lhs.r - (int)rhs.r);
		int gVectorDiff = ((int)lhs.g - (int)rhs.g);
		int bVectorDiff = ((int)lhs.b - (int)rhs.b);
		return
			rVectorDiff * rVectorDiff +
			gVectorDiff * gVectorDiff +
			bVectorDiff * bVectorDiff;
	}

public:
	void ColorSpaceTest()
	{
		signed char test = -1;
		CPPUNIT_ASSERT((test >> 1) == -1);
		Color white(255, 255, 255);
		Color blackColor(0, 0, 0);
		YCbCr black(0, 128, 128);
		Color red(255, 0, 0);
		Color green(0, 255, 0);
		Color blue(0, 0, 255);

		LARGE_INTEGER start, end;
		QueryPerformanceCounter(&start);
		Color color(0,0,0);
		
		for (int i = 0; i < 640*480; ++i)
			ColorSpace::getYCbCr(color);

		QueryPerformanceCounter(&end);
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
	}

	void YCbCrTest()
	{
		YCbCr a;
		YCbCr b(0, 0, 0);
		CPPUNIT_ASSERT(a == b);
		a.Y = 1;
		CPPUNIT_ASSERT(!(a == b));

		YCbCr c = b;
		CPPUNIT_ASSERT(b == c);
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( ImageTest );

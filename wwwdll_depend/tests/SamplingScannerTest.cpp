#include <cppunit/extensions/HelperMacros.h>
#include <Win32/Bitmap.hpp>
#include <Image/SamplingScanner.hpp>
#include <Image/ColorSpace.hpp>
#include <iostream>

typedef PixelFormat<RGB24> PixelFormat24;
typedef DIBitmap<PixelFormat24> DIBitmap24;
typedef SamplingScanner<DIBitmap24,	4> TestDIB4;
typedef SamplingScanner<DIBitmap24,	2> TestDIB2;
typedef SamplingScanner<DIBitmap24,	1> TestDIB1;

class SamplingScannerTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(SamplingScannerTest);
	CPPUNIT_TEST(ConstantsTest);
	CPPUNIT_TEST(SamplePointTest);
	CPPUNIT_TEST(ColorSpaceTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void SamplePointTest()
	{
		DIBitmap24 bitmap;
		bitmap.setWidth(640);
		bitmap.setHeight(480);

		bool result = bitmap.createDIBSection();
		if (result == false)
		{
			std::cout << "\nerror code is " << GetLastError() << std::endl;
		}
		CPPUNIT_ASSERT(result);
			

		for(size_t y = 0; y < 256; ++y)
			for(size_t x = 0; x < 256; ++x)
				bitmap.getPixel(x, y)->setColor(Color(x, x, x));

		const TestDIB4 scanner4(bitmap);
		for(size_t y = 0; y < 256; ++y)
		{
			for(size_t x = 0; x < 256; ++x)
			{
				CPPUNIT_ASSERT(
					scanner4.getPixel(x, y)->getColor() ==
					bitmap.getPixel(x, y)->getColor());
			}
		}

		const TestDIB2 scanner2(bitmap);
		for(size_t y = 0; y < 256; ++y)
		{
			for(size_t x = 0; x < 128; ++x)
			{
				CPPUNIT_ASSERT(
					scanner2.getPixel(x, y)->getColor() ==
					bitmap.getPixel(x << 1, y)->getColor());
			}
		}


		TestDIB1 scanner1(bitmap);
		for(size_t y = 0; y < 128; ++y)
		{
			for(size_t x = 0; x < 128; ++x)
			{
				CPPUNIT_ASSERT(
					scanner1.getPixel(x, y)->getColor() ==
					bitmap.getPixel(x << 1, y << 1)->getColor());
			}
		}
	}

	void ColorSpaceTest()
	{
		DIBitmap24 bitmap;
		bitmap.setWidth(640);
		bitmap.setHeight(480);

		bool result = bitmap.createDIBSection();
		if (result == false)
		{
			std::cout << "\nerror code is " << GetLastError() << std::endl;
		}
		CPPUNIT_ASSERT(result);
			

		for(size_t y = 0; y < 256; ++y)
			for(size_t x = 0; x < 256; ++x)
				bitmap.getPixel(x, y)->setColor(Color(x, x, x));

		SamplingScanner<DIBitmap24, 1> quoterSampler(bitmap);
		for(size_t y = 0; y < 128; ++y)
			for(size_t x = 0; x < 128; ++x)
			{
				CPPUNIT_ASSERT(
					quoterSampler.getPixel(x, y)->getColor().r == (x << 1));
				CPPUNIT_ASSERT(
					quoterSampler.getPixel(x, y)->getColor().g == (x << 1));
				CPPUNIT_ASSERT(
					quoterSampler.getPixel(x, y)->getColor().b == (x << 1));

			}
	}

	void ConstantsTest()
	{
		CPPUNIT_ASSERT(TestDIB4::xShiftFactor == 0);
		CPPUNIT_ASSERT(TestDIB4::yShiftFactor == 0);

		CPPUNIT_ASSERT(TestDIB2::xShiftFactor == 1);
		CPPUNIT_ASSERT(TestDIB2::yShiftFactor == 0);

		CPPUNIT_ASSERT(TestDIB1::xShiftFactor == 1);
		CPPUNIT_ASSERT(TestDIB1::yShiftFactor == 1);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( SamplingScannerTest );

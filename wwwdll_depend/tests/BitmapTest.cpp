#include <cppunit/extensions/HelperMacros.h>
#include <Win32/Bitmap.hpp>
#include <iostream>
#include <malloc.h>

class BitmapTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(BitmapTest);
	CPPUNIT_TEST(Bitmap24Test);
	CPPUNIT_TEST(PixelFormatPointerTest);
	CPPUNIT_TEST(DIBitmapTest);
	CPPUNIT_TEST(SpeedCheck);
	CPPUNIT_TEST(StaticAssertion);
	CPPUNIT_TEST_SUITE_END();
	

public:
	void StaticAssertion()
	{
		CPPUNIT_ASSERT(sizeof(RGB24) == 3);
		CPPUNIT_ASSERT(sizeof(RGB32) == 4);
	}

	void SpeedCheck()
	{
		/* here working jobs. */
		BITMAPINFO* info = (BITMAPINFO*)_alloca(sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 2);
		info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		info->bmiHeader.biWidth = 640;
		info->bmiHeader.biHeight = 480;
		info->bmiHeader.biPlanes = 1;
		info->bmiHeader.biBitCount = 32;
		info->bmiHeader.biCompression = BI_BITFIELDS;
		info->bmiHeader.biSizeImage = 640 * 480 * 4;
		info->bmiHeader.biXPelsPerMeter = static_cast<LONG>(72 * 100 / 2.54);
		info->bmiHeader.biYPelsPerMeter = static_cast<LONG>(72 * 100 / 2.54);
		info->bmiHeader.biClrUsed = 0;
		info->bmiHeader.biClrImportant = 0;
		DWORD* ColorMask = reinterpret_cast<DWORD*>(info->bmiColors);
		ColorMask[0] = 0xff000000;
		ColorMask[1] = 0x00ff0000;
		ColorMask[2] = 0x0000ff00;

		unsigned char* ppvColors;
		HBITMAP hBitmap =
			::CreateDIBSection(NULL,
							   info,
							   DIB_RGB_COLORS,
							   reinterpret_cast<void**>(&ppvColors),
							   NULL,
							   0);

		assert(hBitmap);
							   
		const unsigned char* ppvColorsEnd =
			ppvColors + (640 * 480 * 4);

		LARGE_INTEGER start;
		QueryPerformanceCounter(&start);

		for(DWORD* itor = (DWORD*)ppvColors; itor < (DWORD*)ppvColorsEnd; ++itor)
		{
			*itor = 0x7f000000;
		}

		bool isSuccess = true;
		for(DWORD* itor = (DWORD*)ppvColors; itor < (DWORD*)ppvColorsEnd; ++itor)
		{
			isSuccess &= ((*itor & 0xff000000) == 0x7f000000 );
			isSuccess &= ((*itor & 0x00ff0000) == 0x00000000 );
			isSuccess &= ((*itor & 0x0000ff00) == 0x00000000 );
		}
		
		LARGE_INTEGER end;
		QueryPerformanceCounter(&end);
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);

		assert(isSuccess == true);
		::DeleteObject(hBitmap);

		double starttime, endtime, frequency;
		starttime =  start.LowPart;
		endtime =  end.LowPart;
		frequency =  freq.LowPart;

// 		std::cout << std::endl
// 				  << (endtime - starttime) / frequency << std::endl
// 				  << freq.LowPart << std::endl;

	}
	void DIBitmapTest()
	{
		typedef DIBitmap<PixelFormat<RGB24> > Bitmap;
		Bitmap bitmap;

		bitmap.setWidth(640);
		bitmap.setHeight(480);
		CPPUNIT_ASSERT(bitmap.createDIBSection());

		LARGE_INTEGER start;
		QueryPerformanceCounter(&start);
		Bitmap::iterator endor = bitmap.end();
		const Color dist(0, 0x7f, 0xff);

		for(Bitmap::iterator itor = bitmap.begin();
			itor != endor;
			++itor)
		{
			(*itor).setColor(dist);
		}
		LARGE_INTEGER interval;
		QueryPerformanceCounter(&interval);

		bool isSuccess = true;
		for(Bitmap::iterator itor = bitmap.begin();
			itor != endor;
			++itor)
		{
			isSuccess &= (*itor).getColor() == dist;
		}
		LARGE_INTEGER end;
		QueryPerformanceCounter(&end);

		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		double starttime, intervaltime, endtime, frequency;
		starttime =  start.LowPart;
		intervaltime =  interval.LowPart;
		endtime =  end.LowPart;
		frequency =  freq.LowPart;

// 		std::cout << std::endl
// 				  << (intervaltime - starttime) / frequency << std::endl
// 				  << (endtime - intervaltime) / frequency << std::endl
// 				  << freq.LowPart << std::endl;

	}

	void Bitmap24Test()
	{
		RGB24 rgb24;
		CPPUNIT_ASSERT(rgb24.isBitFieldFormat() == false);
		CPPUNIT_ASSERT(rgb24.getRedBitField() == 0);
		CPPUNIT_ASSERT(rgb24.getGreenBitField() == 0);
		CPPUNIT_ASSERT(rgb24.getBlueBitField() == 0);
	}

	void PixelFormatPointerTest()
	{
		typedef PixelFormat<RGB24>::iterator Pointer;
		Pointer ptr = 0;
		Pointer ptrIncl(ptr + 1);

		CPPUNIT_ASSERT(ptr++ == 0);
		CPPUNIT_ASSERT(ptr == ptrIncl);
		CPPUNIT_ASSERT(ptr-- == ptrIncl);
		CPPUNIT_ASSERT(++ptr == ptrIncl);
		CPPUNIT_ASSERT(--ptr == 0);
		CPPUNIT_ASSERT((ptr + 1) == ptrIncl);
		CPPUNIT_ASSERT((ptrIncl - 1) == ptr);
		CPPUNIT_ASSERT(ptr < ptrIncl);
		CPPUNIT_ASSERT(ptrIncl > ptr);
		CPPUNIT_ASSERT(ptr <= ptr);
		CPPUNIT_ASSERT(ptr >= ptr);
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( BitmapTest );

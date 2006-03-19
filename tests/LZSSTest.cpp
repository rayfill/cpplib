#include <cppunit/extensions/HelperMacros.h>
#include <util/Compression/LZSS.hpp>
#include <iostream>

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

class TokenPolicyTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(TokenPolicyTest);
	CPPUNIT_TEST(paramTest);
	CPPUNIT_TEST(lengthTest);
	CPPUNIT_TEST(positionTest);
 	CPPUNIT_TEST(characterTest);
	CPPUNIT_TEST(bitConvertTest);
	CPPUNIT_TEST(buildMatchTokenTest);
	CPPUNIT_TEST(buildNoMatchTokenTest);
	CPPUNIT_TEST_SUITE_END();

public:
	typedef TokenPolicy<12, 4> DefaultPolicy;
	typedef TokenPolicy<24, 8> LargePolicy;

	void paramTest()
	{
		CPPUNIT_ASSERT(DefaultPolicy::PositionSize == 12);
		CPPUNIT_ASSERT(DefaultPolicy::LengthSize == 4);
		CPPUNIT_ASSERT(DefaultPolicy::TokenLength == 2);

		CPPUNIT_ASSERT(LargePolicy::PositionSize == 24);
		CPPUNIT_ASSERT(LargePolicy::LengthSize == 8);
		CPPUNIT_ASSERT(LargePolicy::TokenLength == 4);
	}

	void lengthTest()
	{
		DefaultPolicy::token_data_t defVector(DefaultPolicy::TokenLength);
		LargePolicy::token_data_t largeVector(LargePolicy::TokenLength);

		CPPUNIT_ASSERT(defVector.size() == 2);
		CPPUNIT_ASSERT(largeVector.size() == 4);

		defVector[1] = 5;
		CPPUNIT_ASSERT(DefaultPolicy::getLength(defVector) == 5);
		
		largeVector[3] = 254;
		CPPUNIT_ASSERT(LargePolicy::getLength(largeVector) == 254);
	}

	void positionTest()
	{
		DefaultPolicy::token_data_t defVector(DefaultPolicy::TokenLength);
		LargePolicy::token_data_t largeVector(LargePolicy::TokenLength);

		CPPUNIT_ASSERT(defVector.size() == 2);
		CPPUNIT_ASSERT(largeVector.size() == 4);

		/// MSB ‚©‚ç12bit•ª
		/// 0x08c5(2245) << 4 = 0x8c45
		defVector[0] = 0x8c;
		defVector[1] = 0x51;
		CPPUNIT_ASSERT(DefaultPolicy::getPosition(defVector) == 2245);
		
		/// MSB ‚©‚ç24bit•ª
		/// 0xe4e1c0(15000000) << 8 = 0xe4e1c000
		largeVector[0] = 0xe4;
		largeVector[1] = 0xe1;
		largeVector[2] = 0xc0;
		largeVector[3] = 1;
		CPPUNIT_ASSERT(LargePolicy::getPosition(largeVector) == 15000000);
	}
	
	void characterTest()
	{
		DefaultPolicy::token_data_t defData =
			DefaultPolicy::buildNoMatchToken('z');
		LargePolicy::token_data_t largeData =
			LargePolicy::buildNoMatchToken('a');

		CPPUNIT_ASSERT(DefaultPolicy::getCharacter(defData) == 'z');
		CPPUNIT_ASSERT(LargePolicy::getCharacter(largeData) == 'a');
	}

	void bitConvertTest()
	{
		DefaultPolicy::token_data_t data(DefaultPolicy::TokenLength);

		data[0] = 0xAA; // (10101010b)
		data[1] = 0x55; // (01010101b)

		CPPUNIT_ASSERT(DefaultPolicy::tokenToBitReps(data) == 0xAA55);
	}

	void buildMatchTokenTest()
	{
		// 0x7b << 4 | 0x0a = 0x07ba
		DefaultPolicy::token_data_t defData =
			DefaultPolicy::buildMatchToken(123, 10);

		// 0x0400 << 8 | 0x05 = 0x00040005
		LargePolicy::token_data_t largeData =
			LargePolicy::buildMatchToken(1024, 5);

		typedef std::char_traits<char> char_t;

		CPPUNIT_ASSERT(defData[0] == char_t::to_char_type(0x07));
		CPPUNIT_ASSERT(defData[1] == char_t::to_char_type(0xba));
		
		CPPUNIT_ASSERT(largeData[0] == char_t::to_char_type(0x00));
		CPPUNIT_ASSERT(largeData[1] == char_t::to_char_type(0x04));
		CPPUNIT_ASSERT(largeData[2] == char_t::to_char_type(0x00));
		CPPUNIT_ASSERT(largeData[3] == char_t::to_char_type(0x05));
	}

	void buildNoMatchTokenTest()
	{}
};

class TokensTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(TokensTest);
	CPPUNIT_TEST(referenceValTokenToByteRepsTest);
	CPPUNIT_TEST(realValTokenToByteRepsTest);
	CPPUNIT_TEST_SUITE_END();
public:

	void referenceValTokenToByteRepsTest()
	{
		/*
		 * 1 << 4 | 2 == 16 + 2 = 18 == 0x0012
		 */
		ReferenceValueToken<TokenPolicy<12, 4> > rvt(1, 2);

		std::vector<char> bitReps = rvt.toByteReplesentation();
		
		CPPUNIT_ASSERT(bitReps.size() == 2);
		CPPUNIT_ASSERT(bitReps[0] == 0x00);
		CPPUNIT_ASSERT(bitReps[1] == 0x12);

		/*
		 * 1 << 2 | 2 == 4 + 2 == 6 == 0x0006
		 */
		ReferenceValueToken<TokenPolicy<14, 2> > rvt2(1, 2);

		bitReps = rvt2.toByteReplesentation();

		CPPUNIT_ASSERT(bitReps.size() == 2);
		CPPUNIT_ASSERT(bitReps[0] == 0x00);
		CPPUNIT_ASSERT(bitReps[1] == 0x06);
	}

	void realValTokenToByteRepsTest()
	{
		/*
		 * 'a'(0x61) << 4 | 0 == 0x0610
		 */
		RealValueToken<TokenPolicy<12, 4> > rvt('a');

		std::vector<char> bitReps = rvt.toByteReplesentation();

		CPPUNIT_ASSERT(bitReps.size() == 2);
		CPPUNIT_ASSERT(bitReps[0] == 0x06);
		CPPUNIT_ASSERT(bitReps[1] == 0x10);


		/*
		 * 'a'(0x61) << 2 | 0 == 0x0184
		 */
		RealValueToken<TokenPolicy<14, 2> > rvt2('a');

		bitReps = rvt2.toByteReplesentation();

		
		CPPUNIT_ASSERT(bitReps.size() == 2);
		CPPUNIT_ASSERT(bitReps[0] == 0x01);
		CPPUNIT_ASSERT(std::char_traits<char>::to_int_type(bitReps[1]) ==
					   0x84);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( WindowFunctionTest );
CPPUNIT_TEST_SUITE_REGISTRATION( TokenPolicyTest );
CPPUNIT_TEST_SUITE_REGISTRATION( TokensTest );



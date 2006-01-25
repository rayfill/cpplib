#include <iostream>
#include <iomanip>
#include <cppunit/extensions/HelperMacros.h>
#include <Cryptography/AES.hpp>

class RijndaelTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(RijndaelTest);
	CPPUNIT_TEST_SUITE_END();
public:
};

CPPUNIT_TEST_SUITE_REGISTRATION( RijndaelTest );

class AESTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(AESTest);
	CPPUNIT_TEST(SBOXTest);
	CPPUNIT_TEST(WordSubTest);
	CPPUNIT_TEST(ShiftRowTest);
	CPPUNIT_TEST(MixColumnTest);
//	CPPUNIT_TEST(productPrint);
//	CPPUNIT_TEST(rconPrint);
	CPPUNIT_TEST(KeySchedulingTest);
	CPPUNIT_TEST(cipherTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void cipherTest()
	{
		std::vector<AES<>::byte> data(16);
		std::vector<AES<>::byte> key(16);

		key[0] = 1;
		for (unsigned int i = 0; i < data.size(); ++i)
		{
			data[i] = static_cast<AES<>::byte>(i);
		}

		AES<> aes;
		aes.setKey(key);

		std::vector<AES<>::byte> ciphertext =
			aes.encrypt(data);

		std::vector<AES<>::byte> plaintext =
			aes.decrypt(ciphertext);

		CPPUNIT_ASSERT(ciphertext.size() == plaintext.size());
		CPPUNIT_ASSERT(data.size() == plaintext.size());
		
		for (unsigned int i = 0; i < ciphertext.size(); ++i)
		{
			CPPUNIT_ASSERT(data[i] == plaintext[i]);
		}
	}

	void KeySchedulingTest()
	{
		unsigned int scheduledKey[] = {
			0x00000001,
			0x00000000,
			0x00000000,
			0x00000000,
			0x63636363,
			0x63636363,
			0x63636363,
			0x63636363,
			0x9898989a,
			0xfbfbfbf9,
			0x9898989a,
			0xfbfbfbf9,
			0x01979791,
			0xfa6c6c68,
			0x62f4f4f2,
			0x990f0f0b,
			0x2a79e1ef,
			0xd0158d87,
			0xb2e17975,
			0x2bee767e,
			0xd988c9c7,
			0x099d4440,
			0xbb7c3d35,
			0x90924b4b,
			0x6ae88654,
			0x6375c214,
			0xd809ff21,
			0x489bb46a,
			0x68ba9299,
			0x0bcf508d,
			0xd3c6afac,
			0x9b5d1bc6,
			0xdcaedeb6,
			0xd7618e3b,
			0x04a72197,
			0x9ffa3a51,
			0x0d75f32d,
			0xda147d16,
			0xdeb35c81,
			0x414966d0,
			0x7df6c828,
			0xa7e2b53e,
			0x7951e9bf,
			0x38188f6f,
		};

		AES<> aes;
		std::vector<AES<>::byte> keySource(16);
		for (unsigned int i = 0; i < keySource.size(); ++i)
			keySource[i] = 0;
		keySource[0] = 1;

		std::vector<AES<>::word> scheduledKeyVector =
			aes.getScheduledKey(keySource);

		for (unsigned int i = 0; i < scheduledKeyVector.size(); ++i)
			CPPUNIT_ASSERT(scheduledKeyVector[i] == scheduledKey[i]);
	}

	void rconPrint()
	{
		std::cout << std::endl;
		std::cout.fill('0');

		AES<> aes;
		unsigned char RCon[16];

		RCon[1] = 1;
		for (unsigned int i = 2; i < 16; ++i)
		{
			RCon[i] = aes.multiply(i, RCon[i-1]);
		}
		for (unsigned int i = 1; i < 16; ++i)
		{
			std::cout << "RCon[" << i << "] = 0x" <<
				std::setw(2) << std::hex << 
				(unsigned short)(RCon[i]) << std::endl;
		}
	}

	void productPrint()
	{
		std::cout.fill('0');

		for (unsigned short i = 0; i < 16; ++i)
		{
			std::cout << "i=" << std::setw(2) << std::hex << i << std::endl;
			for (unsigned char p = 1; p <= 3; ++p)
			{
				std::cout << " p=" << std::setw(2) << std::hex <<
					(unsigned short)(p) << ": ";
				std::cout << std::setw(2) << std::hex << 
					(unsigned short)(AESConstant::productTable[i*256+p]) <<
					std::endl;
			}
		}
		std::cout << std::dec;
	}

	void MixColumnTest()
	{
		std::vector<AES<>::byte> state(16);
		for (unsigned int i = 0; i < 16; ++i)
			state[i] = static_cast<AES<>::byte>(i);
		
		/*
		 * state[] = {
		 *  0,  1,  2,  3,
		 *  4,  5,  6,  7,
		 *  8,  9,  a,  b,
		 *  c,  d,  e,  f,
		 * };
		 */
		AES<> aes;
		aes.mixColumn(state);

		AES<>::byte flipState[] = {
			0x02, 0x07, 0x00, 0x05,
			0x06, 0x03, 0x04, 0x01,
			0x0a, 0x0f, 0x08, 0x0d,
			0x0e, 0x0b, 0x0c, 0x09,
		};
		for (unsigned int i = 0; i < 16; ++i)
		{
			if (state[i] != flipState[i])
			{
				std::cout << std::endl;
				std::cout << "unmatch to " << i << std::endl;
			}
			CPPUNIT_ASSERT(state[i] == flipState[i]);
		}


		aes.invertMixColumn(state);
		
		for (unsigned int i = 0; i < 16; ++i)
			CPPUNIT_ASSERT(state[i] == static_cast<AES<>::byte>(i));
	}

	void ShiftRowTest()
	{
		std::vector<AES<>::byte> state(16);
// 		for (unsigned int i = 0; i < state.size(); ++i)
// 		{
// 			state[i] = static_cast<AES<>::byte>(i);
// 		}

// 		/*
// 		 * state[] = {
// 		 *  0,  1,  2,  3,
// 		 *  4,  5,  6,  7,
// 		 *  8,  9,  a,  b,
// 		 *  c,  d,  e,  f,
// 		 * };
// 		 */

 		AES<> aes;
// 		aes.shiftRow(state);
// 		/*
// 		 * state[] = {
// 		 *  0,  d,  a,  7,
// 		 *  4,  1,  e,  b,
// 		 *  8,  5,  2,  f,
// 		 *  c,  9,  6,  3,
// 		 * };
// 		 */
// 		CPPUNIT_ASSERT(state[0] == 0x00);
// 		CPPUNIT_ASSERT(state[1] == 0x0d);
// 		CPPUNIT_ASSERT(state[2] == 0x0a);
// 		CPPUNIT_ASSERT(state[3] == 0x07);

// 		CPPUNIT_ASSERT(state[4] == 0x04);
// 		CPPUNIT_ASSERT(state[5] == 0x01);
// 		CPPUNIT_ASSERT(state[6] == 0x0e);
// 		CPPUNIT_ASSERT(state[7] == 0x0b);

// 		CPPUNIT_ASSERT(state[8] == 0x08);
// 		CPPUNIT_ASSERT(state[9] == 0x05);
// 		CPPUNIT_ASSERT(state[10] == 0x02);
// 		CPPUNIT_ASSERT(state[11] == 0x0f);

// 		CPPUNIT_ASSERT(state[12] == 0x0c);
// 		CPPUNIT_ASSERT(state[13] == 0x09);
// 		CPPUNIT_ASSERT(state[14] == 0x06);
// 		CPPUNIT_ASSERT(state[15] == 0x03);

// 		aes.invertShiftRow(state);
// 		for (unsigned int i = 0; i < state.size(); ++i)
// 		{
// 			CPPUNIT_ASSERT(state[i] == static_cast<AES<>::byte>(i));
// 		}

		state[0] = 0x7c;
		state[1] = 0x7c;
		state[2] = 0x77;
		state[3] = 0x7b;
		state[4] = 0xf2;
		state[5] = 0x6b;
		state[6] = 0x6f;
		state[7] = 0xc5;
		state[8] = 0x30;
		state[9] = 0x01;
		state[10] = 0x67;
		state[11] = 0x2b;
		state[12] = 0xfe;
		state[13] = 0xd7;
		state[14] = 0xab;
		state[15] = 0x76;

		aes.shiftRow(state);
// 		std::cout << std::endl;
// 		for (unsigned int i = 0; i < state.size(); ++i)
// 		{
// 			std::cout << "state[" << std::dec << i << "]: " <<
// 				std::hex << (unsigned short)state[i] << std::endl;
// 		}
		CPPUNIT_ASSERT(state[0] == 0x7c);
		CPPUNIT_ASSERT(state[1] == 0x6b);
		CPPUNIT_ASSERT(state[2] == 0x67);
		CPPUNIT_ASSERT(state[3] == 0x76);
		CPPUNIT_ASSERT(state[4] == 0xf2);
		CPPUNIT_ASSERT(state[5] == 0x01);
		CPPUNIT_ASSERT(state[6] == 0xab);
		CPPUNIT_ASSERT(state[7] == 0x7b);
		CPPUNIT_ASSERT(state[8] == 0x30);
		CPPUNIT_ASSERT(state[9] == 0xd7);
		CPPUNIT_ASSERT(state[10] == 0x77);
		CPPUNIT_ASSERT(state[11] == 0xc5);
		CPPUNIT_ASSERT(state[12] == 0xfe);
		CPPUNIT_ASSERT(state[13] == 0x7c);
		CPPUNIT_ASSERT(state[14] == 0x6f);
		CPPUNIT_ASSERT(state[15] == 0x2b);
	}
	
	void WordSubTest()
	{
		AES<> aes;
		int test = 0x12345678;

		int subedInt = aes.wordSub(test);
		CPPUNIT_ASSERT(static_cast<unsigned char>((subedInt >> 24) & 0xff) == 
					   aes.byteSub(0x78));
		CPPUNIT_ASSERT(static_cast<unsigned char>((subedInt >> 16) & 0xff) == 
					   aes.byteSub(0x56));
		CPPUNIT_ASSERT(static_cast<unsigned char>((subedInt >> 8) & 0xff) == 
					   aes.byteSub(0x34));
		CPPUNIT_ASSERT(static_cast<unsigned char>((subedInt) & 0xff) == 
					   aes.byteSub(0x12));
	}

	void SBOXTest()
	{
		AES<> aes;
		for (int i = 0; i < 256; ++i)
		{
			CPPUNIT_ASSERT(
				static_cast<unsigned char>(i) ==
				aes.invertByteSub(
					aes.byteSub(
						static_cast<unsigned char>(i))));
		}
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( AESTest );

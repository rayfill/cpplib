#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <iomanip>

void print(std::string name, std::vector<unsigned char>& vec)
{
	std::cout << std::endl;
	std::cout.fill('0');
	std::cout << name << ": ";
	std::cout << std::hex;
	for (size_t i = 0; i < vec.size(); ++i)
	{
		std::cout << std::setw(2) << (unsigned short)vec[i] << " ";
	}
}

#include <Cryptography/Mode/CBC.hpp>

class NullCipher
{
public:
	size_t getBlockSize()
	{
		return 16;
	}

	void setKey(const std::vector<unsigned char>& )
	{}

	std::vector<unsigned char>
	encrypt(std::vector<unsigned char> source)
	{
		return std::vector<unsigned char>(source);
	}

	std::vector<unsigned char>
	decrypt(const std::vector<unsigned char>& source)
	{
		return std::vector<unsigned char>(source);
	}
};

class CBCTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(CBCTest);
	CPPUNIT_TEST(endecryptTest);
	CPPUNIT_TEST_SUITE_END();
public:

	void endecryptTest()
	{
		std::vector<unsigned char> initialVector(16), key(16), plaintext(16*3);
		for (size_t i = 0; i < plaintext.size(); ++i)
		{
			plaintext[i] = i;
		}
		for (size_t i = 0; i < key.size(); ++i)
		{
			key[i] = 0;
			initialVector[i] = 0xff;
		}

		CipherBlockChain<NullCipher> cbc;
		std::vector<unsigned char> ciphertext =
			cbc.encrypt(plaintext, key, initialVector);

//		std::cout << std::endl;
//		print("encrypt", ciphertext);

		for (size_t i = 0; i < 16; ++i)
		{
			CPPUNIT_ASSERT(ciphertext[i] == (i ^ 0xff));
			CPPUNIT_ASSERT(ciphertext[i + 16] ==
						   (ciphertext[i] ^ plaintext[i + 16]));
			CPPUNIT_ASSERT(ciphertext[i + 32] ==
						   (ciphertext[i + 16] ^ plaintext[i + 32]));
		}

		// decrypt
//		std::cout << std::endl;
//		print("IV", initialVector);

		std::vector<unsigned char> decrypttext =
			cbc.decrypt(ciphertext, key, initialVector);

		CPPUNIT_ASSERT(plaintext.size() == decrypttext.size());

//		std::cout << std::endl;
//		print("decrypt", decrypttext);

		for (size_t i = 0; i < decrypttext.size(); ++i)
			CPPUNIT_ASSERT(plaintext[i] == decrypttext[i]);
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( CBCTest );


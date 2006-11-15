#include <iostream>
#include <iomanip>
#include <cppunit/extensions/HelperMacros.h>
#include <Cryptography/RSA.hpp>
#include <Cryptography/Random.hpp>
#include <math/theory/AlgebraTheory.hpp>


class RSATest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(RSATest);
  	CPPUNIT_TEST(keyCreateTest);
//   	CPPUNIT_TEST(endecryptTest);
  	CPPUNIT_TEST(endecryptFor1024bitKeyTest);
  	CPPUNIT_TEST(stringConversionTest);
  	CPPUNIT_TEST(vectorConversionTest);
  	CPPUNIT_TEST(memoryConversionTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void memoryConversionTest()
	{
		Random random;

		std::vector<unsigned char> data = random.getRandomByteVector(100);
		MPInteger convertInt = RSA::memoryToMPI(&data[0], data.size());

		std::vector<unsigned char> result(data.size());
		size_t writed = result.size();
		RSA::mpiToMemory(convertInt, &result[0], writed);
		CPPUNIT_ASSERT(result.size() == writed);

		for (size_t index = 0; index < data.size(); ++index)
		{
			CPPUNIT_ASSERT_MESSAGE("unmatch converted.",
								   data[index] == result[index]);
		}

	}

	void vectorConversionTest()
	{
		Random random;

		std::vector<unsigned char> data = random.getRandomByteVector(100);
		MPInteger convertInt = RSA::vectorToMPI(data);

		std::vector<unsigned char> result = RSA::mpiToVector(convertInt);
		CPPUNIT_ASSERT(data.size() == result.size());
		for (size_t index = 0; index < data.size(); ++index)
		{
			if (data[index] != result[index])
				CPPUNIT_ASSERT_MESSAGE("unmatch converted.", false);
		}
	}

	void stringConversionTest()
	{
		std::string str = "HelloWorld.";
		MPInteger convertInt = RSA::stringToMPI(str);

		std::string result = RSA::mpiToString(convertInt);
		CPPUNIT_ASSERT_MESSAGE(result, str == result);
	}

	void keyCreateTest()
	{
		const unsigned int bitLength = 256;
		KeyPair pair = RSA::makeKeyPair(bitLength);
		
		CPPUNIT_ASSERT(
			pair.getPublicKey().getEncryptExponent().getBitLength() == 17);
		CPPUNIT_ASSERT(
			pair.getPublicKey().getModulus() ==
			pair.getPrivateKey().getModulus());
		
		PublicKey pubKey = pair.getPublicKey();
		PrivateKey privKey = pair.getPrivateKey();
	}

	void endecryptTest()
	{
		const unsigned int bitLength = 128;
		KeyPair pair = RSA::makeKeyPair(bitLength);
		
		PublicKey pubKey = pair.getPublicKey();
		PrivateKey privKey = pair.getPrivateKey();

		RSA cipher(pubKey, privKey);
		MPInteger plaintext("12fbff45836b");
		MPInteger ciphertext = cipher.encrypt(plaintext);
		MPInteger decrypttext = cipher.decrypt(ciphertext);

		CPPUNIT_ASSERT(plaintext == decrypttext);
	}

	void endecryptFor1024bitKeyTest()
	{
		MPInteger encryptExponent("00010001");
		MPInteger modulus(
			"7ef57a896736682c97adea5669df5ce8764c05e3f00f5e5b882d1"
			"1955e68ba46d61e65f97fac21df965e933157f269139a7a38078c"
			"3c2e595a7ee17fa23cb562c00b9336dbea41555dc8a577d193106"
			"4d4eff76e93dc5bdd4c531ac0603125d61cd7d91017adb97fc777"
			"741f7680206e65a648875a3e93ff12ad26781d0e56d1");

		MPInteger decryptExponent(
			"0cae5448f928340b9032ecdf28c008b5a76b3c8361ed070db9725"
			"6f9466ecb7c5bd2b978cc49d3305402aa4d196dbb151c2eecfdc4"
			"0216d711f4ee6d23355120a1e59074a2408f457b216b8d90dd809"
			"1947684d57ebff65c55bc2af13d7d84396de565c40513f02dce13"
			"5e075a26835acc0f367dd2e58c5e2f9e370584e02481");

		MPInteger plaintext = "abffe123f875b1f45da2b2ca";

		PublicKey pubkey(encryptExponent, modulus);
		PrivateKey privkey(decryptExponent, modulus);

		RSA cipher(pubkey);
		MPInteger ciphertext = cipher.encrypt(plaintext);

		CPPUNIT_ASSERT(plaintext != ciphertext);

		RSA decrypter(privkey);
		MPInteger decrypttext = decrypter.decrypt(ciphertext);
		
		CPPUNIT_ASSERT(plaintext == decrypttext);

	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( RSATest );

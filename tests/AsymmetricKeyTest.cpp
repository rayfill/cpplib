#include <iostream>
#include <cppunit/extensions/HelperMacros.h>
#include <Cryptography/AsymmetricKey.hpp>

class AsymmetricKeyTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(AsymmetricKeyTest);
	CPPUNIT_TEST(KeyPairTest);
	CPPUNIT_TEST_SUITE_END();
public:
	void KeyPairTest()
	{
		try
		{
			KeyPair pair(5U, 7U);
			CPPUNIT_ASSERT(pair.modulus.toString() == "0023");
			CPPUNIT_ASSERT(pair.encryptExponent.toString() == "00010001");

			MPInteger m(8U);
			MPInteger c = modulusExponential(m, pair.encryptExponent, pair.modulus);
			MPInteger p = modulusExponential(c, pair.decryptExponent, pair.modulus);
			p.adjust();
			CPPUNIT_ASSERT_MESSAGE(p.toString(), p == m);
		}
		catch (std::invalid_argument& e)
		{
			CPPUNIT_FAIL(e.what());
			CPPUNIT_FAIL("process in throws exception.");
		}
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( AsymmetricKeyTest );

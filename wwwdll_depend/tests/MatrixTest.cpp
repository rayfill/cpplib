#include <cppunit/extensions/HelperMacros.h>

class MatrixTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(Matrixtest);
	CPPUNIT_TEST();
	CPPUNIT_TEST_SUITE_END();

	Matrix matrix;

public:
	void setUp()
	{}
	void tearDown()
	{}

	void testInitialize()
	{
	}

	void testEqualMatrix()
	{}

	void testEqualScaler()
	{
		matrix.initalize();
		CPPUNIT_TEST_ASSERT();
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( MatrixTest );

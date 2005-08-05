#include <iostream>
#include <cppunit/extensions/HelperMacros.h>
#include <math/MPI.hpp>
#include <math/MersenneTwister.hpp>

class MPITest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(MPITest);
 	CPPUNIT_TEST(notEqualOpTest);
  	CPPUNIT_TEST(multiplyOpTest);
  	CPPUNIT_TEST(plusOpTest);
  	CPPUNIT_TEST(minusOpTest);
  	CPPUNIT_TEST(divideOpTest);
  	CPPUNIT_TEST(lessThanOpTest);
  	CPPUNIT_TEST(lessOrEqualOpTest);
  	CPPUNIT_TEST(equalOpTest);
  	CPPUNIT_TEST(equivalenceOpTest);
  	CPPUNIT_TEST(graterThanOpTest);
  	CPPUNIT_TEST(graterEqualOpTest);
  	CPPUNIT_TEST(constructerTest);
  	CPPUNIT_TEST(adjustTest);
  	CPPUNIT_TEST(divideTest);
  	CPPUNIT_TEST(bitLengthTest);
  	CPPUNIT_TEST(MaxColumnTest);
  	CPPUNIT_TEST(toStringTest);
 	CPPUNIT_TEST(makeNumberTest);
 	CPPUNIT_TEST(shiftTest);
 	CPPUNIT_TEST(fromConverterTest);
 	CPPUNIT_TEST(toConverterTest);
 	CPPUNIT_TEST(montgomeryTest);
 	CPPUNIT_TEST(unitBaseModulusTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void unitBaseModulusTest()
	{
		MPInteger a(
			"cb785082c998a3dbaaae00ca9f5f80af77765a466ba9976d60cb8af8"
			"900c146f9212a62c1ce20305ec5ed6f32041d23e68e5d4ca278881d1"
			"6bac0e28728044064c210d7b0e1a04d637c4f917b08877001c7b5038"
			"55912363442ed8bfbe7b962a6af48c66d981e5d4985a7dde976d8f9c"
			"3c0c0e2ab57106f170a690039880d01b");

		MPInteger b("efdc");

		CPPUNIT_ASSERT((a % b) == MPInteger(a.modulus(0xefdc)));
	}

	void montgomeryTest()
	{
		MPInteger a(7U);
		MPInteger e(10U);
		MPInteger n(13U);

		a.montgomeryModulusExponential(e, n);
		CPPUNIT_ASSERT_MESSAGE(a.toString(), a.toString() == "4");
	}

	void fromConverterTest()
	{
		std::vector<unsigned char> bigendianVector;
		bigendianVector.push_back(0x80);
		bigendianVector.push_back(0);
		bigendianVector.push_back(0);
		bigendianVector.push_back(0x01);
		CPPUNIT_ASSERT(bigendianVector[0] == 0x80);
		CPPUNIT_ASSERT(bigendianVector[1] == 0);
		CPPUNIT_ASSERT(bigendianVector[2] == 0);
		CPPUNIT_ASSERT(bigendianVector[3] == 0x01);

		MPInteger a;
		a.fromBigEndianMemory(bigendianVector);
		CPPUNIT_ASSERT_MESSAGE(a.toString(), a.toString() == "80000001");
	}

	void toConverterTest()
	{
		MPInteger a("80000001");
		CPPUNIT_ASSERT(a.getMaxColumn() == 1);
		std::vector<unsigned char> result = a.toBigEndianMemory();
		CPPUNIT_ASSERT(result.size() == 4);
		CPPUNIT_ASSERT(result[0] == 0x80);
		CPPUNIT_ASSERT(result[1] == 0);
		CPPUNIT_ASSERT(result[2] == 0);
		CPPUNIT_ASSERT(result[3] == 0x01);

		MPInteger b("100080000001");
		CPPUNIT_ASSERT(b.getMaxColumn() == 2);
		result = b.toBigEndianMemory();
		CPPUNIT_ASSERT(result.size() == 8);
		CPPUNIT_ASSERT(result[0] == 0);
		CPPUNIT_ASSERT(result[1] == 0);
		CPPUNIT_ASSERT(result[2] == 0x10);
		CPPUNIT_ASSERT(result[3] == 0);
		CPPUNIT_ASSERT(result[4] == 0x80);
		CPPUNIT_ASSERT(result[5] == 0);
		CPPUNIT_ASSERT(result[6] == 0);
		CPPUNIT_ASSERT(result[7] == 0x01);

	}

	void notEqualOpTest()
	{
		MPInteger a;
		for (int index = 0; index < 100; ++index)
		{
			a.value.push_back(index);
		}
		MPInteger b;
		for (int index = 0; index < 100; ++index)
		{
			b.value.push_back(a.value[index] ^ 0xffff);
		}
		b.value.push_back(1);
		
		CPPUNIT_ASSERT(!(a != a));
		CPPUNIT_ASSERT(a != b);
	}

 	void equalOpTest()
	{
		MPInteger a;
		for (int index = 0; index < 100; ++index)
		{
			a.value.push_back(index);
		}
		MPInteger b;
		for (int index = 0; index < 100; ++index)
		{
			b.value.push_back(a.value[index] ^ 0xffff);
		}
		
		CPPUNIT_ASSERT(a == a);
		CPPUNIT_ASSERT(!(a == b));

		b = a;
		a.value.push_back(0);
		CPPUNIT_ASSERT(a == b);
	}

 	void multiplyOpTest()
	{
		MPInteger a(0xffffU);
		MPInteger b(0xffffffffU);
		MPInteger result(a);
		result *= b;

		result.adjust();
		CPPUNIT_ASSERT_MESSAGE(result.toString(), result.toString() == "fffeffff0001");

		result = a * MPInteger(0U);
		result.adjust();
		CPPUNIT_ASSERT(result.toString() == "0");

		a = MPInteger("e69b42ac4aeaffa8904c554f5d7d3bd9b8181cd42eea336cad912faf1ac6d253cda309447c8cca6e10757dac6486a725f7e08fc18042320b01033fb9594415fd51de3d24e65104e4b79575f41bce66b5e7dd8e810d55d0fd23c5d34dac9c50423616e89f8d43b3f2d10bdd84930d40ceda2a1ebb4db651a1031af52e18b9deb0640bf47a000a1c2858a3680fd02d84e13fead399ff8deef3779ead793191f6269039f9a282a7bb53507681129ee6653ae8501ef2700a8586374161fa58553278249dfb9efedf3535f56b3050b4480a17cc987d535703c94e1a1218e208475b3e0e63c32b3c235e01f6152727a72e8aceefb72e59cf1a853fe3c70ed7cf81acf1faec1c21911db5da8a1b1f45a66374b8948dc654a4eb22fd1dbc6f913f30e356122a7ab8b253e502f24d11bbf3f08bdec720760cd881c0e202d912d422a76b395c36d7b7040220ee23d559797ef37f05a5c8635b02cff5a2797448ebcc663d3e84f33ba2e2fd32d82528d40e88daa1598f3dfaf0eb797200c92b82a52d0f9930ee875aff3624d93f63d4afca6c3d8e1446bf684c6eaff4663c61392c127414aa5e27bdf46896087637bf84d43bb3625028f54f36e5da4f8659e047da5f2c8607d9c250959963c4e7b4cdd72964abda2c6ca4a0e08aeb9598dce98914dfeb0a1f405e89b2500dfcff08ed88cf1112aeb42407a57582d111e1e917d5898dbf6130");
		b = MPInteger("9672f2e5cb4a522b4cb0edaf7d35667cd357fd23603f1f64d24fa4c1a1a5ec0bdc57fbfca6963d63f150493503e92b64e4b49e4e53ab24c63838ab0a94343c99a5d40f226099dea546604772accc74b054d63d5c13f1d456ba4548fbf0301e486092f4572ca1d876c3ef72400f13c9e1b7003995477e75f147b66659bcab6ae2edfb0079d344ade7d3f03406f210e2204a0f898027bd9739e0c59814afb69e0aaa9843ee08b48156c8946514150255689368e64879197a6c366b1747e5fc860923b5ea2578b174c255dab05ca7e54e0a8be367e72119bebf102196473f0b32dea9a611d06072480163a81db4e7f95462ff369647b9f701626c3f17f2e6e90c83bc4454dd408e605dea3fd7cbeeaf947c2bfd3b316f7b282a8c97afcec55ceeda70c0c2f0453bf16ab7ebd1208c52bc390353e201ba8f2ec63bf68f8a9256f92fa4404049402dd1362aafda20de2a13252c8cd0bd493c312140946fcb612bb00708eeea04807d038151a74a7aaabc507a96a4dfb61ea50fc948cc6105dc0fb9e0a338a27445f65c6b0b67c78864ae0a4156c688d9fd3aff32e5a8f3701e24cb2c32b2455c23065243f84da63ca869020a5fd7e6947104ce310f875ea01e7dc1ecd91af6916e6ca5991f8d57ad08e6e12a5128c0a82cbadd4bf643d9f86d31e95a89f73d115f909dd92f5c18fb64346b7a9a74383b20b9c7d34373c77e48d620");

		CPPUNIT_ASSERT(a * b > a);
	}

 	void plusOpTest()
	{
		MPInteger a("ffffffffffffffffffffffffffffffff");
		MPInteger b("1");

		MPInteger result = a + b;
		result.adjust();
		CPPUNIT_ASSERT_MESSAGE(result.toString(), 
							   result.toString() ==
							   "100000000000000000000000000000000");
		
		
		result = b + a;
		result.adjust();
		CPPUNIT_ASSERT_MESSAGE(
			result.toString().c_str(),
			result.toString() ==
					   "100000000000000000000000000000000");

		result = a + MPInteger(0U);
		result.adjust();
		CPPUNIT_ASSERT(result.toString() ==
					   "ffffffffffffffffffffffffffffffff");

		result = MPInteger(0U) + a;
		result.adjust();
		CPPUNIT_ASSERT_MESSAGE(
			result.toString().c_str(),
			result.toString() ==
			"ffffffffffffffffffffffffffffffff");
		
		result = MPInteger(0U) + MPInteger(0U);
		result.adjust();
		CPPUNIT_ASSERT(result.toString() ==
					   "0");

		MPInteger c = a.getNegateValue();
		result = a + c;
		CPPUNIT_ASSERT(result.isZero());

		result = b + c;
		CPPUNIT_ASSERT_MESSAGE(result.toString(),
							   result.toString() ==
							   "-fffffffffffffffffffffffffffffffe");

		result = c + b.negate();
		CPPUNIT_ASSERT_MESSAGE(result.toString(),
							   result.toString() ==
							   "-100000000000000000000000000000000");

		result = b + c.negate();
		CPPUNIT_ASSERT_MESSAGE(result.toString(),
							   result.toString() ==
							   "fffffffffffffffffffffffffffffffe");

		result = a + b.getNegateValue();
		CPPUNIT_ASSERT_MESSAGE(result.toString(),
							   result.toString() ==
							   "100000000000000000000000000000000");
	}

 	void minusOpTest()
	{
		MPInteger a("100000000000000000000");
		MPInteger b("1");

		MPInteger result = a - b;
		result.adjust();
		CPPUNIT_ASSERT(result.toString() == 
					   "ffffffffffffffffffff");

		result = result - MPInteger(0U);
		result.adjust();
		CPPUNIT_ASSERT(result.toString() ==
					   "ffffffffffffffffffff");

		CPPUNIT_ASSERT(
			(MPInteger(0U) - result).toString() ==
			"-ffffffffffffffffffff");

		result = result - result;
		result.adjust();
		CPPUNIT_ASSERT(result.toString() ==
					   "0");

		result = result - result;
		result.adjust();
		CPPUNIT_ASSERT(result.toString() == "0");

		a = MPInteger(1U);
		b = MPInteger("ffffffff");
		MPInteger c = b.getNegateValue();
		CPPUNIT_ASSERT((a-b).toString() == "-fffffffe");
		CPPUNIT_ASSERT((b-a).toString() == "fffffffe");

		CPPUNIT_ASSERT((a-c).toString() == "100000000");
		CPPUNIT_ASSERT((a.getNegateValue() - b).adjust().toString() ==
					   "-100000000");
		CPPUNIT_ASSERT((a.getNegateValue() - c).toString() == "fffffffe");


		a = MPInteger(1U);
		CPPUNIT_ASSERT((--a).toString() == "0");
	}

 	void divideOpTest()
	{
		PRNGen random;
		random.initialize(1234);
		std::vector<MPInteger::BaseUnit> val;

		const int length = 128;
		for (int count = 0; count < length * 3; ++count)
			val.push_back(static_cast<MPInteger::BaseUnit>(random.getNumber()));

 		MPInteger a(&val[0], &val[length*1]);
		MPInteger b(&val[length*1], &val[length*2]);
		MPInteger c(&val[length*2], &val[length*3]);

		CPPUNIT_ASSERT(a != b);
		CPPUNIT_ASSERT(b != c);
		CPPUNIT_ASSERT(c != a);

		MPInteger modulasTarget = a * b + c;

		MPInteger result = modulasTarget / b;
		MPInteger modResult = modulasTarget % b;
		CPPUNIT_ASSERT(modulasTarget == (result * b + modResult));
	}

 	void lessThanOpTest()
	{
		MPInteger a(1U);
		MPInteger b(123456U);
		MPInteger c("ffffffffffff");
		MPInteger d;

		CPPUNIT_ASSERT(a < b);
		CPPUNIT_ASSERT(!(a < a));
		CPPUNIT_ASSERT(!(b < a));
		CPPUNIT_ASSERT(a < c);
		CPPUNIT_ASSERT(b < c);
		CPPUNIT_ASSERT(!(d < d));
		CPPUNIT_ASSERT(d < a);
		CPPUNIT_ASSERT(!(a < d));
	}

 	void lessOrEqualOpTest()
	{
		MPInteger a(1U);
		MPInteger b(123456U);
		MPInteger c("ffffffffffff");
		MPInteger d;

		CPPUNIT_ASSERT(a <= b);
		CPPUNIT_ASSERT(a <= a);
		CPPUNIT_ASSERT(!(b <= a));
		CPPUNIT_ASSERT(a <= c);
		CPPUNIT_ASSERT(b <= c);
		CPPUNIT_ASSERT(d <= d);
		CPPUNIT_ASSERT(d <= a);
		CPPUNIT_ASSERT(!(a <= d));
	}

 	void equivalenceOpTest()
	{
		MPInteger a(1U);
		MPInteger b(123456U);
		MPInteger c("ffffffffffff");
		MPInteger d;

		CPPUNIT_ASSERT(a == a);
		CPPUNIT_ASSERT(b == b);
		CPPUNIT_ASSERT(c == c);
		CPPUNIT_ASSERT(d == d);
		CPPUNIT_ASSERT(!(a == b));
		CPPUNIT_ASSERT(!(a == d));
		CPPUNIT_ASSERT(!(c == d));
	}

 	void graterThanOpTest()
	{
		MPInteger a(1U);
		MPInteger b(123456U);
		MPInteger c("ffffffffffff");
		MPInteger d;

		CPPUNIT_ASSERT(!(a > b));
		CPPUNIT_ASSERT(!(a > a));
		CPPUNIT_ASSERT(b > a);
		CPPUNIT_ASSERT(!(a > c));
		CPPUNIT_ASSERT(!(b > c));
		CPPUNIT_ASSERT(!(d > d));
		CPPUNIT_ASSERT(!(d > a));
		CPPUNIT_ASSERT(a > d);
	}

 	void graterEqualOpTest()
	{
		MPInteger a(1U);
		MPInteger b(123456U);
		MPInteger c("ffffffffffff");
		MPInteger d;

		CPPUNIT_ASSERT(!(a >= b));
		CPPUNIT_ASSERT(a >= a);
		CPPUNIT_ASSERT(b >= a);
		CPPUNIT_ASSERT(!(a >= c));
		CPPUNIT_ASSERT(!(b >= c));
		CPPUNIT_ASSERT(d >= d);
		CPPUNIT_ASSERT(!(d >= a));
		CPPUNIT_ASSERT(a >= d);
	}

 	void constructerTest()
	{
		MPInteger a;
		CPPUNIT_ASSERT(a.value.size() == 0);
		
		MPInteger b(12345U);
		CPPUNIT_ASSERT(b.value.size() == 1 &&
					   b.value[0] == 12345);
		b.adjust();
		CPPUNIT_ASSERT(b.value.size() == 1);

		MPInteger c(1, 15);
		for (int index = 0; index < 15; ++index)
			CPPUNIT_ASSERT(c.value[index] == 1);

		MPInteger::BaseUnit temp[] = {1, 2, 3, 4, 5, 6, 7, 7};
		MPInteger d(temp, temp + 7);
		for (unsigned int index = 0;
			 index < 7;
			 ++index)
			CPPUNIT_ASSERT(d.value[index] == index + 1);

		MPInteger e("1b3f22fe00b1");
		CPPUNIT_ASSERT(e.value[1] == 0x1b3f);
		CPPUNIT_ASSERT(e.value[0] == 0x22fe00b1);
		CPPUNIT_ASSERT(e.isMinusSign == false);

		MPInteger f(e);
		CPPUNIT_ASSERT(f.value[1] == 0x1b3f);
		CPPUNIT_ASSERT(f.value[0] == 0x22fe00b1);
		CPPUNIT_ASSERT(f.isMinusSign == false);

		MPInteger g("-1b3f22fe00b1");
		CPPUNIT_ASSERT(g.value[1] == 0x1b3f);
		CPPUNIT_ASSERT(g.value[0] == 0x22fe00b1);
		CPPUNIT_ASSERT(g.isMinusSign == true);

		MPInteger h("-1b3");
		CPPUNIT_ASSERT(h.value[0] == 0x1b3);
		CPPUNIT_ASSERT(h.isMinusSign == true);

		CPPUNIT_ASSERT_THROW(MPInteger(""), TokenParseException);
	}

 	void adjustTest()
	{
		MPInteger a(0, 20);
		a.value[0] = 1;

		a.adjust();
		CPPUNIT_ASSERT(a.value.size() == 1);
	}

 	void divideTest()
	{
		MPInteger modulo;
		MPInteger modulas("ffffffff");
		MPInteger modulasZero(0U);
		MPInteger a("ffffffffffff");
		MPInteger b("12345678");
		MPInteger c(0U);

		CPPUNIT_ASSERT_THROW(c.divide(modulasZero, modulo),
							 ZeroDivideException);
		MPInteger result = a.divide(modulas, modulo);
		CPPUNIT_ASSERT_MESSAGE(result.toString().c_str(),
							   result == MPInteger("10000"));
		CPPUNIT_ASSERT_MESSAGE(modulo.toString().c_str(),
							   modulo == MPInteger("ffff"));
	}

 	void bitLengthTest()
	{
		MPInteger a(0, 20);
		a.value[1] = 2;
		
		CPPUNIT_ASSERT(a.getBitLength() == sizeof(MPInteger::BaseUnit) * 8 + 2);
	}

 	void MaxColumnTest()
	{
		MPInteger a(0, 20);
		a.value[1] = 1;
		
		CPPUNIT_ASSERT(a.getMaxColumn() == 2);
	}

 	void toStringTest()
	{
		MPInteger a(11, 20);
		CPPUNIT_ASSERT(a.toString() == std::string(
					   "b" // 1
					   "0000000b" // 2
					   "0000000b" // 3
					   "0000000b" // 4
					   "0000000b" // 5
					   "0000000b" // 6
					   "0000000b" // 7
					   "0000000b" // 8
					   "0000000b" // 9
					   "0000000b" //10
					   "0000000b" //11
					   "0000000b" //12
					   "0000000b" //13
					   "0000000b" //14
					   "0000000b" //15
					   "0000000b" //16
					   "0000000b" //17
					   "0000000b" //18
					   "0000000b" //19
					   "0000000b" //20
			));
		CPPUNIT_ASSERT(a.getBitLength() == 19 * sizeof(MPInteger::BaseUnit)*8 + 4);
	}

	void makeNumberTest()
	{
		MPInteger::BaseUnit
			sample[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		MPInteger a =
			MPInteger::makeNumberOfBitSafe(
				sample,
				sample + (sizeof(sample) / sizeof(MPInteger::BaseUnit)));

		CPPUNIT_ASSERT_MESSAGE(a.toString().c_str(),
							   a.toString() ==
							   "80000000000000"
							   "00000000000000"
							   "00000000000000"
							   "00000000000000"
							   "00000000000000"
							   "00000000000000"
							   "00000000000000"
							   "00000000000001");
	}

	void shiftTest()
	{
		MPInteger a("AAAAAAAAAAAA");
		MPInteger b(a);
		b >>= 1;
		CPPUNIT_ASSERT(b.toString() == "555555555555");
		b >>= 1;
		CPPUNIT_ASSERT_MESSAGE(b.toString(),
							   b.toString() == "2aaaaaaaaaaa");
		b >>= 9;
		CPPUNIT_ASSERT(b.toString() == "1555555555");
		

		MPInteger c(a);
		c <<= 1;

		CPPUNIT_ASSERT_MESSAGE(c.toString(), c.toString() == "1555555555554");
		c <<= 8;
		CPPUNIT_ASSERT(c.toString() == "155555555555400");
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( MPITest );

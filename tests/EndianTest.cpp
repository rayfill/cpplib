#include <cppunit/extensions/HelperMacros.h>
#include <IO/Endian.hpp>
#include <typeinfo>
#include <iostream>

class EndianTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(EndianTest);
	CPPUNIT_TEST(endianCheckTest);
	CPPUNIT_TEST(littleEndianTest);
	CPPUNIT_TEST(userDefinedEndianConverterTest);
	CPPUNIT_TEST_SUITE_END();
public:

	void endianCheckTest()
	{
		int endianCheck = 1;
		char* p = reinterpret_cast<char*>(&endianCheck);

		EndianConverter conv;
		CPPUNIT_ASSERT_MESSAGE(typeid(*conv.endian.get()).name(),
							   typeid(*conv.endian.get()) ==
							   (*p != 0 ? typeid(LittleEndian) : typeid(BigEndian)));
	}

	void littleEndianTest()
	{
		EndianConverter conv(new LittleEndian());
 		long long ll = 0x01234567;
		ll <<= 32;
		ll += 0x89abcdef;

 		unsigned long long ull = 0xfedcba98;
		ull <<= 32;
		ull += 0x76543210;

		long l = 0x12345678;
		unsigned long ul = 0xfedcba98;

		int i = 0x12345678;
		unsigned int ui = 0xfedcba98;

		short s = 0x1234;
		unsigned short us = 0xfedc;

		char c = 0x12;
		unsigned char uc = 0xfe;
				
		CPPUNIT_ASSERT(((conv.toBig(ll)>>32) & 0xffffffff) == 0xefcdab89);
		CPPUNIT_ASSERT((conv.toBig(ll) & 0xffffffff) == 0x67452301);
		CPPUNIT_ASSERT((conv.toBig(ull)>>32) == 0x10325476);
		CPPUNIT_ASSERT((conv.toBig(ull) & 0xffffffff) == 0x98badcfe);
		CPPUNIT_ASSERT(conv.toBig(l) == 0x78563412);
		CPPUNIT_ASSERT(conv.toBig(ul) == 0x98badcfe);
		CPPUNIT_ASSERT(conv.toBig(i) == 0x78563412);
		CPPUNIT_ASSERT(conv.toBig(ui) == 0x98badcfe);
		CPPUNIT_ASSERT(conv.toBig(s) == 0x3412);
		CPPUNIT_ASSERT(conv.toBig(us) == 0xdcfe);
		CPPUNIT_ASSERT(conv.toBig(c) == c);
		CPPUNIT_ASSERT(conv.toBig(uc) == uc);

		CPPUNIT_ASSERT(conv.toLittle(ll) == ll);
		CPPUNIT_ASSERT(conv.toLittle(ull) == ull);
		CPPUNIT_ASSERT(conv.toLittle(l) == l);
		CPPUNIT_ASSERT(conv.toLittle(ul) == ul);
		CPPUNIT_ASSERT(conv.toLittle(i) == i);
		CPPUNIT_ASSERT(conv.toLittle(ui) == ui);
		CPPUNIT_ASSERT(conv.toLittle(s) == s);
		CPPUNIT_ASSERT(conv.toLittle(us) == us);
		CPPUNIT_ASSERT(conv.toLittle(c) == c);
		CPPUNIT_ASSERT(conv.toLittle(uc) == uc);

		CPPUNIT_ASSERT(((conv.fromBig(ll)>>32) & 0xffffffff) == 0xefcdab89);
		CPPUNIT_ASSERT((conv.fromBig(ll) & 0xffffffff) == 0x67452301);
		CPPUNIT_ASSERT((conv.fromBig(ull)>>32) == 0x10325476);
		CPPUNIT_ASSERT((conv.fromBig(ull) & 0xffffffff) == 0x98badcfe);
		CPPUNIT_ASSERT(conv.fromBig(l) == 0x78563412);
		CPPUNIT_ASSERT(conv.fromBig(ul) == 0x98badcfe);
		CPPUNIT_ASSERT(conv.fromBig(i) == 0x78563412);
		CPPUNIT_ASSERT(conv.fromBig(ui) == 0x98badcfe);
		CPPUNIT_ASSERT(conv.fromBig(s) == 0x3412);
		CPPUNIT_ASSERT(conv.fromBig(us) == 0xdcfe);
		CPPUNIT_ASSERT(conv.fromBig(c) == c);
		CPPUNIT_ASSERT(conv.fromBig(uc) == uc);

		CPPUNIT_ASSERT(conv.fromLittle(ll) == ll);
		CPPUNIT_ASSERT(conv.fromLittle(ull) == ull);
		CPPUNIT_ASSERT(conv.fromLittle(l) == l);
		CPPUNIT_ASSERT(conv.fromLittle(ul) == ul);
		CPPUNIT_ASSERT(conv.fromLittle(i) == i);
		CPPUNIT_ASSERT(conv.fromLittle(ui) == ui);
		CPPUNIT_ASSERT(conv.fromLittle(s) == s);
		CPPUNIT_ASSERT(conv.fromLittle(us) == us);
		CPPUNIT_ASSERT(conv.fromLittle(c) == c);
		CPPUNIT_ASSERT(conv.fromLittle(uc) == uc);
	}

	void userDefinedEndianConverterTest()
	{
		int endianCheck = 1;
		char* p = reinterpret_cast<char*>(&endianCheck);

		UserDefinedEndianConverter converter(*p != 1 ?
					UserDefinedEndianConverter::bigEndian :
					UserDefinedEndianConverter::littleEndian);

		CPPUNIT_ASSERT(converter.to(0x12345678) == 0x12345678);
		CPPUNIT_ASSERT(converter.from(0x12345678) == 0x12345678);

		UserDefinedEndianConverter converter2(*p != 1 ?
					UserDefinedEndianConverter::littleEndian :
					UserDefinedEndianConverter::bigEndian);

		CPPUNIT_ASSERT(converter2.to(0x12345678) == 0x78563412);
		CPPUNIT_ASSERT(converter2.from(0x12345678) == 0x78563412);
	}
};


CPPUNIT_TEST_SUITE_REGISTRATION( EndianTest );

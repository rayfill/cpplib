#include <iostream>
#include <cppunit/extensions/HelperMacros.h>
#include <util/format/Exif.hpp>
#include <IO/Endian.hpp>
#include <iostream>

class ExifTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(ExifTest);
	CPPUNIT_TEST(factoryTest);
	CPPUNIT_TEST(exifHeaderParseTest);
	CPPUNIT_TEST(tiffHeaderParseTest);
	CPPUNIT_TEST(fullExifHeaderParseTest);
	CPPUNIT_TEST(tagParserTest);
	CPPUNIT_TEST_SUITE_END();
public:
	void tagParserTest()
	{
		using namespace Exif;

		TagInfo tag(1, 2, 4, ('A' << 24) +  ('B'<< 16) +  ('C' << 8));
		char memory[] = {
			'H', 'e', 'l', 'l', 'o', ' ',
			'W', 'o', 'r', 'l', 'd', 0,
			0x00, 0x00, 0x00, 0x02,
			0x00, 0x00, 0x00, 0x05,
			0x00, 0x00, 0x00, 0x04,
			0x00, 0x00, 0x00, 0x0a,
		};
		TagInfo tag2(1, 2, 12, 0);
		TagInfo rationalTag(1, 5, 2, 12);

		UserDefinedEndianConverter conv(bigEndian);
		TagInfoParser parser(memory, memory + sizeof(memory), conv);
		CPPUNIT_ASSERT_MESSAGE(parser.getString(tag).c_str(),
							   parser.getString(tag) == "ABC");
		CPPUNIT_ASSERT_MESSAGE(parser.getString(tag2).c_str(),
							   parser.getString(tag2) == "Hello World");
		std::vector<Rational<unsigned int> > result =
			parser.getUnsignedRationals(rationalTag);

		CPPUNIT_ASSERT(result.size() == 2);
		CPPUNIT_ASSERT(result[0] == result[1]);
	}

	void factoryTest()
	{
		char memory[15] = { 0x00, 0x01,
							0x00, 0x02,
							0x00, 0x00, 0x00, 0x03,
							0x00, 0x00, 0x00, 0x04,
							0xff, 0xff, 0xff};

		using namespace Exif;
		UserDefinedEndianConverter conv(bigEndian); // big endian converter.
		TagInfo tag = TagInfo::parseMemoryToTagInfo(memory, conv);

		CPPUNIT_ASSERT(tag.getTag() == 1);
		CPPUNIT_ASSERT(tag.getTagType() == 2);
		CPPUNIT_ASSERT(tag.getNumberOfValues() == 3);
		CPPUNIT_ASSERT(tag.getValueOrOffset() == 4);
	}

	void exifHeaderParseTest()
	{
		char memory[] = {
			0xff, 0xd8, // SOI
			0x00, 0x00, // dummy
			0xff, 0xe1, // APP1
			0x00, 0x11, // APP1 chunk size
			'E', 'x', 'i', 'f', 0, 0, // Exif identifier code
			'M', 'M', // endian order identifier
			0x00, 0x2a, // tiff identifier code
			0x00, 0x00, 0x00, 0x00, // 0th IFD pointer to offset
			0x00 };

		using namespace Exif;

		ExifHeaderParser parser(memory, memory + sizeof(memory));

		CPPUNIT_ASSERT(parser.isValidHeader());

		CPPUNIT_ASSERT(parser.getTiffHeaderBegin() ==
					   memory + 12 + 2 /*dummy padding's offset*/);

		CPPUNIT_ASSERT(reinterpret_cast<const unsigned char*>(
						   parser.getTiffHeaderEnd()) ==
					   reinterpret_cast<const unsigned char*>(
						   memory + sizeof(memory)));

		char memory2[] = {
			0xff, 0xd8, // SOI
			0xff, 0xe1, // APP1
			'E', 'x', 'i', 'f', 0, 0, // Exif identifier code
			'M', 'M', // endian order identifier
			0x00, 0x2a, // tiff identifier code
			0x00, 0x00, 0x00, 0x00, // 0th IFD pointer to offset
			0x00 }; // bad chunk data.
		ExifHeaderParser parser2(memory2, memory2 + sizeof(memory2));

		CPPUNIT_ASSERT(!parser2.isValidHeader());
	}

	void tiffHeaderParseTest()
	{
		using namespace Exif;
		char memory[] = {
			0xff, 0xd8, // SOI
			0xff, 0xe1, // APP1
			0x00, 0x44, // APP1 chunk size
			'E', 'x', 'i', 'f', 0, 0, // Exif identifier code
			'M', 'M', // endian order identifier
			0x00, 0x2a, // tiff identifier code
			0x00, 0x00, 0x00, 0x08, // 0th IFD pointer to offset
			// Image File Directory
			0x00, 0x04, // number of directory entries.
			// single dir entry.
			0x00, 0x01,
			0x00, 0x01,
			0x00, 0x00, 0x00, 0x01,
			0x00, 0x00, 0x00, 0x02,

			0x00, 0x02,
			0x00, 0x02,
			0x00, 0x00, 0x00, 0x03,
			0x00, 0x00, 0x00, 0x04,

			0x00, 0x03,
			0x00, 0x03,
			0x00, 0x00, 0x00, 0x05,
			0x00, 0x00, 0x00, 0x06,

			0x00, 0x04,
			0x00, 0x04,
			0x00, 0x00, 0x00, 0x07,
			0x00, 0x00, 0x00, 0x08,

			// next IFD pointer offset.
			0x00, 0x00 };

		TiffHeaderParser parser(memory + 12, memory + sizeof(memory));
		CPPUNIT_ASSERT(parser.isValidHeader());

		unsigned int index = 1;
		for (TiffHeaderParser::iterator itor = parser.begin();
			 itor != parser.end();
			 ++itor)
		{
			CPPUNIT_ASSERT(itor->getTag() == index);
			CPPUNIT_ASSERT(itor->getTagType() == index);
			CPPUNIT_ASSERT(itor->getNumberOfValues() == index * 2 - 1);
			CPPUNIT_ASSERT(itor->getValueOrOffset() == index * 2);
			++index;
		}
	}

	void fullExifHeaderParseTest()
	{
		using namespace Exif;

		char memory[] = {
			0xff, 0xd8, // SOI
			0xff, 0xe1, // APP1
			0x00, 0x44, // APP1 chunk size
			'E', 'x', 'i', 'f', 0, 0, // Exif identifier code
			'M', 'M', // endian order identifier
			0x00, 0x2a, // tiff identifier code
			0x00, 0x00, 0x00, 0x08, // 0th IFD pointer to offset
			// Image File Directory
			0x00, 0x04, // number of directory entries.
			// single dir entry.
			0x00, 0x01,
			0x00, 0x01,
			0x00, 0x00, 0x00, 0x01,
			0x00, 0x00, 0x00, 0x02,

			0x00, 0x02,
			0x00, 0x02,
			0x00, 0x00, 0x00, 0x03,
			0x00, 0x00, 0x00, 0x04,

			0x00, 0x03,
			0x00, 0x03,
			0x00, 0x00, 0x00, 0x05,
			0x00, 0x00, 0x00, 0x06,

			0x00, 0x04,
			0x00, 0x04,
			0x00, 0x00, 0x00, 0x07,
			0x00, 0x00, 0x00, 0x08,

			// next IFD pointer offset.
			0x00, 0x00 };

		ExifHeaderParser exifParser(memory, memory + sizeof(memory));

		CPPUNIT_ASSERT(exifParser.isValidHeader());
		CPPUNIT_ASSERT(exifParser.getTiffHeaderBegin() != NULL);
		
		TiffHeaderParser tiffParser(exifParser.getTiffHeaderBegin(),
									exifParser.getTiffHeaderEnd());

		CPPUNIT_ASSERT(tiffParser.isValidHeader());
		
		unsigned int index = 1;
		for (TiffHeaderParser::iterator itor = tiffParser.begin();
			 itor != tiffParser.end();
			 ++itor)
		{
			CPPUNIT_ASSERT(itor->getTag() == index);
			CPPUNIT_ASSERT(itor->getTagType() == index);
			CPPUNIT_ASSERT(itor->getNumberOfValues() == index * 2 - 1);
			CPPUNIT_ASSERT(itor->getValueOrOffset() == index * 2);
			++index;
		}
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ExifTest );

#include <cppunit/extensions/HelperMacros.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <text/transcode/Unicode.hpp>
#include <text/xml/XMLParser.hpp>

class XMLParserTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(XMLParserTest);
	CPPUNIT_TEST(TokenizerTest);
	CPPUNIT_TEST(TagElementTest);
	CPPUNIT_TEST(createElementTest);
	CPPUNIT_TEST(XMLPathTokenizerTest);
//	CPPUNIT_TEST(XMLPathTraversalTest);
	CPPUNIT_TEST(StrToIntTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void StrToIntTest()
	{
		XMLPath path(L"/hoge");

		CPPUNIT_ASSERT(path.strToInt(L"") == 0);
		CPPUNIT_ASSERT(path.strToInt(L"65535") == 65535);
		CPPUNIT_ASSERT_THROW(path.strToInt(L" "), std::invalid_argument);
	}

	void XMLPathTraversalTest()
	{
		typedef std::map<std::wstring, std::wstring> AttributesType;

//		std::cout << std::hex << std::endl;
		TagElement rootTag(L"root");
//		std::cout << "root pointer: " << &rootTag << std::endl;
		TagElement childTag1(L"child1");
//		std::cout << "childTag1 pointer: " << &childTag1 << std::endl;
		TagElement childTag1_(L"child1");
//		std::cout << "childTag1_ pointer: " << &childTag1_ << std::endl;
		TagElement childTag2(L"child2");
//		std::cout << "childTag2 pointer: " << &childTag2 << std::endl;
		rootTag.addChild(&childTag1);
		rootTag.addChild(&childTag2);
		rootTag.addChild(&childTag1_);

		CPPUNIT_ASSERT(rootTag.children.size() == 3);
		CPPUNIT_ASSERT(dynamic_cast<TagElement*>(rootTag.
												 getChildElement(L"child1"))->
					   getTagName() == L"child1");
		CPPUNIT_ASSERT(dynamic_cast<TagElement*>(rootTag.
												 getChildElement(L"child2"))->
					   getTagName() == L"child2");

		XMLPath path_root(L"/root");
		std::vector<Element*> result = path_root.evaluate(&rootTag);
		CPPUNIT_ASSERT(result.size() == 1);
		CPPUNIT_ASSERT(result[0] == &rootTag);

		XMLPath path_roots(L"/root[]");
		result = path_roots.evaluate(&rootTag);
		CPPUNIT_ASSERT(result.size() == 1);
		CPPUNIT_ASSERT(result[0] == &rootTag);

		XMLPath path_child1(L"/root/child1");
		result = path_child1.evaluate(&rootTag);
		CPPUNIT_ASSERT(result.size() == 1);
		CPPUNIT_ASSERT(result[0] == &childTag1);
		
		XMLPath path_child1s(L"/root/child1[]");
		result = path_child1s.evaluate(&rootTag);
		CPPUNIT_ASSERT(result.size() == 2);
		CPPUNIT_ASSERT(result[0] == &childTag1);
		CPPUNIT_ASSERT(result[1] == &childTag1_);
		
	}

	void XMLPathTokenizerTest()
	{
		CPPUNIT_ASSERT_THROW(new XMLPath(std::wstring(L"hoge")),
							 std::invalid_argument);

		XMLPath path(L"/hoge/fuga[0]/hara");

		std::vector<std::wstring> tokens = path.tokenize();

		CPPUNIT_ASSERT(tokens.size() == 3);
		CPPUNIT_ASSERT(tokens[0] == L"hoge");
		CPPUNIT_ASSERT(tokens[1] == L"fuga[0]");
		CPPUNIT_ASSERT(tokens[2] == L"hara");
	}

	void createElementTest()
	{
		std::wstring source = L"<?xml\n version=\"1.0\" \n"
			"standalone=\"yes\"?>"
			"<test><hoge /><!-- comment --><fuga/><fuga attr=\"atr\">test</fuga></test>";

		XMLParser parser;
		XMLDocument* document = parser.parse(source);

// 		std::cout << std::endl;
// 		std::cout << Transcoder::UTF16toUTF8(document->toString()) << std::endl;

		delete document;
	}

	void TagElementTest()
	{
		std::wstring testTag = L"tagname hoge=\"fuga\" ";
		TagElement tag(testTag);

		CPPUNIT_ASSERT(tag.tagName.length() == 7);

		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(tag.tagName.c_str()),
							   tag.tagName == L"tagname");
		CPPUNIT_ASSERT(tag.attributes.size() == 1);
		CPPUNIT_ASSERT(tag.attributes[L"hoge"] == L"fuga");

		testTag = std::wstring(L"tagname2");
		TagElement tag2(testTag);
		CPPUNIT_ASSERT(tag2.tagName.length() == 8);

		CPPUNIT_ASSERT(tag2.tagName == L"tagname2");
		CPPUNIT_ASSERT(tag2.attributes.size() == 0);

		CPPUNIT_ASSERT_THROW(TagElement(L"hoge fuga=123"), InvalidTagException);
		CPPUNIT_ASSERT_THROW(TagElement(L"hoge fuga="), InvalidTagException);
		CPPUNIT_ASSERT_THROW(TagElement(L"hoge fuga\"123\""), InvalidTagException);

		TagElement tag3(L"hoge fuga=\"\"");
		CPPUNIT_ASSERT(tag3.attributes.size() == 1);
		CPPUNIT_ASSERT(tag3.attributes[L"fuga"] == L"");

		std::string utf8str = Transcoder::UTF16toUTF8(tag3.toString());

		const size_t pos = utf8str.find('\n');
		if (pos != std::string::npos)
			utf8str.erase(pos);

		CPPUNIT_ASSERT_MESSAGE(utf8str,
							   utf8str == "<hoge fuga=\"\" />");
	}

	void TokenizerTest()
	{
		const wchar_t* xmlText =
			L"<token1><token2 />\nhello world.\nhello xml.</token1>";
		XMLParser::Tokenizer tokenizer(xmlText, xmlText + wcslen(xmlText));

		std::wstring result = tokenizer.getToken();
		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(result),
							   result == L"<token1>");

		result = tokenizer.getToken();
		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(result),
							   result == L"<token2 />");

		result = tokenizer.getToken();
		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(result),
							   result == L"\nhello world.\nhello xml.");

		result = tokenizer.getToken();
		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(result),
							   result == L"</token1>");

		CPPUNIT_ASSERT(tokenizer.isEof());

		result = tokenizer.getToken();
		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(result),
							   result == L"");

		CPPUNIT_ASSERT(tokenizer.isEof());
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( XMLParserTest );

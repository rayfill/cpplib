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
	CPPUNIT_TEST(XMLPathTraversalTest);
	CPPUNIT_TEST(StrToIntTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void StrToIntTest()
	{
		XMLPath<wchar_t> path(L"/hoge");

		CPPUNIT_ASSERT(path.strToInt(L"") == 0);
		CPPUNIT_ASSERT(path.strToInt(L"65535") == 65535);
		CPPUNIT_ASSERT_THROW(path.strToInt(L" "), std::invalid_argument);
	}

	void XMLPathTraversalTest()
	{
		typedef std::map<std::basic_string<wchar_t>,
			std::basic_string<wchar_t> > AttributesType;

		TagElement<wchar_t> rootTag(L"root");
		TagElement<wchar_t> childTag1(L"child1");
		TagElement<wchar_t> childTag1_(L"child1");
		TagElement<wchar_t> childTag2(L"child2");
		StringElement<wchar_t> stringTag(L"string");
		CommentElement<wchar_t> commentTag(L"comment");

		rootTag.addChild(&childTag1);
		rootTag.addChild(&childTag2);
		rootTag.addChild(&childTag1_);
		childTag1.addChild(&stringTag);
		childTag1_.addChild(&commentTag);

		CPPUNIT_ASSERT(rootTag.children.size() == 3);
		CPPUNIT_ASSERT(dynamic_cast<TagElement<wchar_t>*>
					   (rootTag.getChildElement(L"child1"))->getTagName() ==
					   L"child1");
		CPPUNIT_ASSERT(dynamic_cast<TagElement<wchar_t>*>
					   (rootTag.getChildElement(L"child2"))->getTagName() ==
					   L"child2");

		std::vector<Element<wchar_t>*> result;

		XMLPath<wchar_t> path_root(L"/root");
		result = path_root.evaluate(&rootTag);
		CPPUNIT_ASSERT(result.size() == 1);
		CPPUNIT_ASSERT(result[0] == &rootTag);

		XMLPath<wchar_t> path_roots(L"/root[]");
		result = path_roots.evaluate(&rootTag);
		CPPUNIT_ASSERT(result.size() == 1);
		CPPUNIT_ASSERT(result[0] == &rootTag);

		XMLPath<wchar_t> path_child1(L"/root/child1");
		result = path_child1.evaluate(&rootTag);
		CPPUNIT_ASSERT(result.size() == 1);
		CPPUNIT_ASSERT(result[0] == &childTag1);

		XMLPath<wchar_t> path_child1s(L"/root/child1[]");
		result = path_child1s.evaluate(&rootTag);
		CPPUNIT_ASSERT(result.size() == 2);
		CPPUNIT_ASSERT(result[0] == &childTag1);
		CPPUNIT_ASSERT(result[1] == &childTag1_);

		XMLPath<wchar_t> path_text(L"/root/child1[]/#text");
		result = path_text.evaluate(&rootTag);
		CPPUNIT_ASSERT(result.size() == 1);
		CPPUNIT_ASSERT(result[0] == &stringTag);

		XMLPath<wchar_t> path_comment(L"/root/child1[]/#comment");
		result = path_comment.evaluate(&rootTag);
		CPPUNIT_ASSERT(result.size() == 1);
		CPPUNIT_ASSERT(result[0] == &commentTag);

		rootTag.removeChild(&childTag1);
		rootTag.removeChild(&childTag2);
		rootTag.removeChild(&childTag1_);
		childTag1.removeChild(&stringTag);
		childTag1_.removeChild(&commentTag);
	}

	void XMLPathTokenizerTest()
	{
		CPPUNIT_ASSERT_THROW(new XMLPath<wchar_t>
							 (std::basic_string<wchar_t>(L"hoge")),
							 std::invalid_argument);

		XMLPath<wchar_t> path(L"/hoge/fuga[0]/hara");

		std::vector<std::basic_string<wchar_t> > tokens = path.tokenize();

		CPPUNIT_ASSERT(tokens.size() == 3);
		CPPUNIT_ASSERT(tokens[0] == L"hoge");
		CPPUNIT_ASSERT(tokens[1] == L"fuga[0]");
		CPPUNIT_ASSERT(tokens[2] == L"hara");
	}

	void createElementTest()
	{
		std::basic_string<utf16_t> source =
			Transcoder::UTF8toUTF16("<?xml\n version=\"1.0\" \n"
			"standalone=\"yes\"?>"
			"<test><hoge /><!-- comment -->"
			"<fuga><![CDATA[<hoge><fuga>"
			"]]><fuga attr=\"atr\"/>test</fuga></test> ");

		XMLParser<utf16_t> parser;
		XMLDocument<utf16_t>* document = parser.parse(source);
		XMLPath<utf16_t> path(Transcoder::UTF8toUTF16("/test/fuga/#text"));
		std::vector<Element<utf16_t>*> elements = path.evaluate(document);
		CPPUNIT_ASSERT(elements.size() == 1);
		CPPUNIT_ASSERT_MESSAGE(
			Transcoder::UTF16toUTF8(
				dynamic_cast<const CDATAElement<utf16_t>*>(
					elements[0])->getString()),
			Transcoder::UTF16toUTF8(
				dynamic_cast<const CDATAElement<utf16_t>*>(
					elements[0])->getString()) == "<hoge><fuga>");

		delete document;
	}

	void TagElementTest()
	{
		std::basic_string<utf16_t> testTag = Transcoder::UTF8toUTF16("tagname hoge=\"fuga\" ");
		TagElement<utf16_t> tag(testTag);

		CPPUNIT_ASSERT(tag.tagName.length() == 7);

		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(tag.tagName.c_str()),
							   tag.tagName == Transcoder::UTF8toUTF16("tagname"));
		CPPUNIT_ASSERT(tag.attributes.size() == 1);
		CPPUNIT_ASSERT(tag.attributes[Transcoder::UTF8toUTF16("hoge")] ==
				Transcoder::UTF8toUTF16("fuga"));

		testTag = std::basic_string<utf16_t>(Transcoder::UTF8toUTF16("tagname2"));
		TagElement<utf16_t> tag2(testTag);
		CPPUNIT_ASSERT(tag2.tagName.length() == 8);

		CPPUNIT_ASSERT(tag2.tagName == Transcoder::UTF8toUTF16("tagname2"));
		CPPUNIT_ASSERT(tag2.attributes.size() == 0);

		CPPUNIT_ASSERT_THROW(TagElement<utf16_t>(Transcoder::UTF8toUTF16("hoge fuga=123")),
							 InvalidTagException);
		CPPUNIT_ASSERT_THROW(TagElement<utf16_t>(Transcoder::UTF8toUTF16("hoge fuga=")),
							 InvalidTagException);
		CPPUNIT_ASSERT_THROW(TagElement<utf16_t>(Transcoder::UTF8toUTF16("hoge fuga\"123\"")),
							 InvalidTagException);

		TagElement<utf16_t> tag3(Transcoder::UTF8toUTF16("hoge fuga=\"\""));
		CPPUNIT_ASSERT(tag3.attributes.size() == 1);
		CPPUNIT_ASSERT(tag3.attributes[Transcoder::UTF8toUTF16("fuga")] == Transcoder::UTF8toUTF16(""));

		std::string utf8str = Transcoder::UTF16toUTF8(tag3.toString());

		const size_t pos = utf8str.find('\n');
		if (pos != std::string::npos)
			utf8str.erase(pos);

		CPPUNIT_ASSERT_MESSAGE(utf8str,
							   utf8str == "<hoge fuga=\"\" />");
	}

	void TokenizerTest()
	{
		const std::basic_string<utf16_t> xmlText =
			Transcoder::UTF8toUTF16("<token1><token2 />\nhello world.\nhello xml.</token1>");
		XMLParser<utf16_t>::Tokenizer tokenizer(xmlText.c_str(), xmlText.c_str() + xmlText.size());

		std::basic_string<utf16_t> result = tokenizer.getToken();
		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(result),
							   result == Transcoder::UTF8toUTF16("<token1>"));

		result = tokenizer.getToken();
		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(result),
							   result == Transcoder::UTF8toUTF16("<token2 />"));

		result = tokenizer.getToken();
		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(result),
							   result == Transcoder::UTF8toUTF16("\nhello world.\nhello xml."));

		result = tokenizer.getToken();
		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(result),
							   result == Transcoder::UTF8toUTF16("</token1>"));

		CPPUNIT_ASSERT(tokenizer.isEof());

		result = tokenizer.getToken();
		CPPUNIT_ASSERT_MESSAGE(Transcoder::UTF16toUTF8(result),
							   result == Transcoder::UTF8toUTF16(""));

		CPPUNIT_ASSERT(tokenizer.isEof());
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( XMLParserTest );

#include <fstream>
#include <iostream>
#include <iomanip>
#include <text/transcode/Unicode.hpp>
#include <text/xml/XMLParser.hpp>
#include <util/SmartPointer.hpp>

int main()
{
	XMLParser parser;
	std::ifstream ifs;
	ifs.open("ipData.xml", std::ios::binary);

	if (!ifs)
		return -1;

	const std::size_t fileSize =
		ifs.seekg(0, std::ios_base::end).tellg();
	ifs.seekg(0, std::ios_base::beg);

	SmartArray<char> xmlDocs(new char[fileSize+1]);
	ifs.read(xmlDocs.get(), fileSize);
	ifs.close();
	xmlDocs[fileSize] = 0;
	std::wstring xmlDocument = Transcoder::UTF8toUTF16(xmlDocs.get());
	
	XMLDocument* document = parser.parse(xmlDocument, true);
	std::cout << Transcoder::UTF16toUTF8(document->toString()) << std::endl;

	for (int index = 0; index + 
	XMLPath path(L"")
	
	delete document;

	return 0;
}

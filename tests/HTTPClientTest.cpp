#include <iostream>
#include <fstream>
#include <net/HTTPClient.hpp>
#include <util/hash/SHA1.hpp>
#include <text/LexicalCast.hpp>
#include <iomanip>

int usage(int argc, char** argv)
{
	std::cout << "usage: " << argv[0] << " +[target_url]" << std::endl;
	return 1;
}

int main(int argc, char** argv)
{
	if (argc < 3)
		return usage(argc, argv);

	const long timeout = lexicalCast<long>(argv[1]);

	SocketModule Module;
	try 
	{
		for (int args = 2; args < argc; ++args)
		{
			std::string targetResource(argv[args]);

			HTTPClient client;

			client.setKeepAliveTime(300);
			client.setTimeout(timeout, 1);
			client.setUserAgent("Mozilla/4.0 (compatible; MSIE 6.0; "
								"Windows NT 5.1; SV1; .NET CLR 1.0.3705; "
								".NET CLR 1.1.4322)");

			client.setAcceptEncoding("");
			client.addAcceptLanguage("ja");
			client.addAcceptLanguage("en");
			client.addCookie("afg=0");
			client.setRange(499, 1000);

			HTTPResult<> result = client.getResource(targetResource.c_str());

			std::vector<unsigned char> resource = result.getResource();

			std::ofstream ofs("savefile.html",
							  std::ios::out |
							  std::ios::binary |
							  std::ios::trunc);
			std::vector<unsigned char>::const_iterator itor =
				resource.begin();
			while (itor != resource.end())
				ofs << *itor++;

			ofs.close();
		}
	} catch (ResponseError& e) {
		std::cout << " raise exception. reason: " <<
			e.what() << std::endl;
	} catch (ConnectionClosedException& e) {
		std::cout << "connection closed by forign host." << std::endl;
	} catch (SocketException& e) {
		std::cout << "target server is not found. ";
	} catch (std::exception& e) {
		std::cout << "raise exception. reason: " <<
			e.what() << std::endl;
	}


	return 0;
}

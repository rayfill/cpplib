#ifndef COMMANDLINE_HPP_
#define COMMANDLINE_HPP_

#include <string>

class CommandLine
{
	friend class CommandLineTest;

private:
	std::vector<std::string> arguments;

	static std::vector<std::string> parseArguments(std::string args)
	{
		std::vector<std::string> result;
		std::string::size_type lastPos = 0;

		while (lastPos != std::string::npos)
		{
			std::string::size_type headPos =
				args.find_first_not_of(" 	", lastPos);

			if (headPos == std::string::npos)
				break;

			lastPos = args.find_first_of(" 	", headPos);

			result.push_back(args.substr(headPos, lastPos - headPos));
		}

		return result;
	}

	static std::vector<std::string> parseArguments(int argc, char* argv[])
	{
		std::vector<std::string> result;

		for (int offset = 0; offset < argc; ++offset)
			result.push_back(argv[offset]);

		return result;
	}

public:
	CommandLine(const std::string& args):
			arguments(parseArguments(args))
	{}

	CommandLine(int argc, char* argv[]):
			arguments(parseArguments(argc, argv))
	{}

	~CommandLine()
	{}
	
	size_t size() const
	{
		return arguments.size();
	}

	typedef std::vector<std::string>::iterator iterator;
	typedef std::vector<std::string>::const_iterator const_iterator;
	
	iterator begin()
	{
		return arguments.begin();
	}

	const_iterator begin() const
	{
		return arguments.begin();
	}

	iterator end()
	{
		return arguments.end();
	}

	const_iterator end() const
	{
		return arguments.end();
	}
};

#endif /* COMMANDLINE_HPP_ */

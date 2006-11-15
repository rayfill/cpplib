#include <vector>
#include <iostream>
#include <string>

#include <stdexcept>

class Test
{
private:
	std::vector<int> vec;
	std::string member;

public:
	Test():
			vec(),
			member()
	{}

	void setMember(const char* newString)
	{
		member = newString;
	}

	const char* getMember() const
	{
		return member.c_str();
	}

	virtual ~Test()
	{}
};

void test(const std::string& reason)
{
	Test test2;

	throw std::runtime_error((std::string("aaaabbbbcccc") + reason).c_str());
}

void nested(const int nest)
{
	if (nest == 0)
		test("testtest");
	else
		nested(nest - 1);
}

int main()
{
	try
	{
		nested(10000);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

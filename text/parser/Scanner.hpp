#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <vector>
#include <string>

template <typename CharType>
class Scanner
{
public:
	typedef CharType char_type;
	typedef Scanner self_type;
	typedef std::basic_string<char_type> string_type;

private:
	std::vector<typename string_type::iterator> stateStack;
	typename string_type::iterator current;
	typename string_type::iterator last;

public:
	Scanner(typename string_type::iterator head_,
			typename string_type::iterator last_):
		stateStack(), current(head_), last(last_)
	{}

	void save()
	{
		stateStack.push_back(current);
	}

	void rollback()
	{
		current = stateStack.back();
		stateStack.pop_back();
	}

	void commit()
	{
		stateStack.pop_back();
	}

	int read()
	{
		if (current == last)
			return -1;
		return std::char_traits<char>::to_int_type(*current++);
	}

	int readAhead()
	{
		if (current == last)
			return -1;
		return std::char_traits<char>::to_int_type(*current);
	}

	string_type getRemainString() const
	{
		return string_type(current, last);
	}

	void skip(size_t skipCount)
	{
		current += skipCount;
	}
	
	bool isEos() const
	{
		return (current == last);
	}
};

#endif /* SCANNER_HPP_ */

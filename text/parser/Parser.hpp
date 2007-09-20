#ifndef LLPARSER_HPP_
#define LLPARSER_HPP_

#include <text/regex/RegexCompile.hpp>

template <typename CharType>
class RegexParser
{
public:
	typedef RegexParser self_type;
	typedef CharType char_type;
	typedef std::char_traits<char_type> traits_type;
	typedef std::basic_string<char_type> string_type;

private:
	typedef RegexCompiler<char_type> regex_compiler_t;
	typedef RegexMatch<char_type> regex_match_t;

	regex_match_t matcher;
public:
	RegexParser(const char_type* pattern):
		matcher(regex_compiler_t().compile(pattern))
	{}

	RegexParser(const RegexParser& source):
		matcher(source.matcher)
	{}

	~RegexParser()
	{}

	template <typename scanner_t>
	bool parse(scanner_t& scanner) const
	{
		regex_match_t matcher_(matcher);

		if (matcher_.match(scanner.getRemainString()))
		{
			std::pair<size_t, size_t> position = matcher_.getCapture(0);
			if (position.first != 0)
				return false;

			scanner.skip(position.second);
			return true;
		}

		return false;
	}
	
};

template <typename CharType>
class StringParser
{
public:
	typedef StringParser self_type;
	typedef CharType char_type;
	typedef std::char_traits<char_type> traits_type;
	typedef std::basic_string<char_type> string_type;

private:
	string_type literal;

public:
	StringParser(const char_type* source):
		literal(source)
	{}

	StringParser(const StringParser& source):
		literal(source.literal)
	{}

	template <typename scanner_t>
	bool parse(scanner_t& scanner) const
	{
		scanner.save();
		
		for (typename string_type::const_iterator itor = literal.begin();
			 itor != literal.end(); ++itor)
		{
			const int val = scanner.read();
			if (val == -1 ||
				traits_type::to_char_type(val) != *itor)
			{
				scanner.rollback();
				return false;
			}
		}
		scanner.commit();
		return true;
	}
};

template <typename CharType>
class CharacterParser
{
public:
	typedef CharacterParser self_type;
	typedef CharType char_type;
	typedef std::char_traits<char_type> traits_type;

private:
	char_type character;

public:
	CharacterParser(const char_type ch):
		character(ch)
	{}

	CharacterParser(const CharacterParser& source):
		character(source.character)
	{}

	template <typename scanner_t>
	bool parse(scanner_t& scanner) const
	{
		scanner.save();
		const int value = scanner.read();
		if (value != -1 &&
			traits_type::to_char_type(value) == character)
		{
			scanner.commit();
			return true;
		}
		
		scanner.rollback();
		return false;
	}
};

template <typename CharType>
class RangeParser
{
public:
	typedef CharType char_type;
	typedef std::char_traits<char_type> traits_type;

private:
	char_type small;
	char_type large;

public:
	RangeParser(char_type small_, char_type large_):
		small(small_), large(large_)
	{
		assert (small <= large);
	}

	~RangeParser()
	{}

	template <typename scanner_t>
	bool parse(scanner_t& scanner) const
	{
		scanner.save();

		const int input = scanner.read();
		if (input != -1 &&
			traits_type::to_char_type(input) >= small &&
			traits_type::to_char_type(input) <= large)
		{
			scanner.commit();
			return true;
		}

		scanner.rollback();
		return false;
	}
};

template <typename LHSType, typename RHSType>
class ChooseParser
{
public:
	typedef ChooseParser self_type;
	typedef LHSType left_type;
	typedef RHSType right_type;

private:
	left_type lhs;
	right_type rhs;

public:
	ChooseParser(left_type lhs_,
				 right_type rhs_):
		lhs(lhs_), rhs(rhs_)
	{}

	ChooseParser(const ChooseParser& source):
		lhs(source.lhs), rhs(source.rhs)
	{}

	template <typename scanner_t>
	bool parse(scanner_t& scanner) const
	{
		scanner.save();
		if (lhs.parse(scanner) ||
			rhs.parse(scanner))
		{
			scanner.commit();
			return true;
		}
		scanner.rollback();
		return false;
	}
};

template <typename StubType>
class AnyParser
{
public:
	typedef AnyParser self_type;
	typedef StubType stub_type;

private:
	stub_type stub;

public:
	AnyParser(stub_type stub_):
		stub(stub_)
	{}

	AnyParser(const AnyParser& source):
		stub(source.stub)
	{}

	template <typename scanner_t>
	bool parse(scanner_t& scanner) const
	{
		while (stub.parse(scanner));

		return true;
	}
};

template <typename StubType>
class RequiredParser
{
public:
	typedef RequiredParser self_type;
	typedef StubType stub_type;

private:
	stub_type stub;

public:
	RequiredParser(stub_type stub_):
		stub(stub_)
	{}

	RequiredParser(const RequiredParser& source):
		stub(source.stub)
	{}

	template <typename scanner_t>
	bool parse(scanner_t& scanner) const
	{
		scanner.save();
		if (!stub.parse(scanner))
		{
			scanner.rollback();
			return false;
		}

		while (stub.parse(scanner));
		return true;
	}
};

template <typename StubType>
class OptionalParser
{
public:
	typedef OptionalParser self_type;
	typedef StubType stub_type;

private:
	stub_type stub;

public:
	OptionalParser(stub_type stub_):
		stub(stub_)
	{}

	OptionalParser(const OptionalParser& source):
		stub(source.stub)
	{}

	template <typename scanner_t>
	bool parse(scanner_t& scanner) const
	{
		scanner.save();
		if (!stub.parse(scanner))
			scanner.rollback();

		return true;
	}
};

template <typename LHSType, typename RHSType>
class ConcatenateParser
{
public:
	typedef ConcatenateParser self_type;
	typedef LHSType left_type;
	typedef RHSType right_type;

private:
	left_type lhs;
	right_type rhs;

public:
	ConcatenateParser(left_type lhs_,
					  right_type rhs_):
		lhs(lhs_), rhs(rhs_)
	{}

	virtual ~ConcatenateParser()
	{}

	template <typename scanner_t>
	bool parse(scanner_t& scanner) const
	{
		scanner.save();
		if (lhs.parse(scanner))
		{
			if (rhs.parse(scanner))
			{
				scanner.commit();
				return true;
			}
		}
		scanner.rollback();
		return false;
	}
};

template <typename left_type, typename right_type>
ChooseParser<left_type, right_type>
createChooseParser(left_type lhs, right_type rhs)
{
	return ChooseParser<left_type, right_type>(lhs, rhs);
}

template <typename left_type, typename right_type>
ConcatenateParser<left_type, right_type>
createConcatenateParser(left_type lhs, right_type rhs)
{
	return ConcatenateParser<left_type, right_type>(lhs, rhs);
}

template <typename left_type, typename right_type>
ConcatenateParser<left_type, right_type> 
operator>>(left_type lhs, right_type rhs)
{
	return createConcatenateParser(lhs, rhs);
}

template <typename left_type, typename right_type>
ChooseParser<left_type, right_type>
operator|(left_type lhs, right_type rhs)
{
	return createChooseParser(lhs, rhs);
}

template <typename ParserType, typename ScannerType, typename SkiperType>
bool parse(ParserType parser, ScannerType scanner, SkiperType skiper)
{
	bool result = false;

	while (!scanner.isEos())
	{

	}

	return true;
}

#endif /* LLPARSER_HPP_ */

#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <string>
#include <text/parser/ScannerTraits.hpp>

class Token
{
private:
	std::string literal;
	ScannerSymbols type;

public:
	Token(ScannerSymbols type_ = undefine, const char* literal_ = ""):
		type(type_), literal(literal_)
	{}
	
	std::string getLiteral() const
	{
		return literal;
	}

	const ScannerSymbols getSymbol() const
	{
		return type;
	}
};

tempalte <typename Traits = ScannerTrait>
class Scanner
{
	friend class ScannerTest;

private:
	Iterator current;
	Iterator last;

public:
	tempalte <typename Iterator>
	Scanner(Iterator first_, Iterator last_):
		current(first_), last(last_)
	{}
};

#endif /* SCANNER_HPP_ */


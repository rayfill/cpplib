#ifndef SCANNER_TRAITS_HPP_
#define SCANNER_TRAITS_HPP_

#include <string>
#include <util/FSM.hpp>

enum ScannerSymbols
{
	undefine = 0,
	literal,
	symbol,
	reserved
};

/**
 * scanning action traits.
 */
class ScannerTrait
{
public:
	typedef FSM<char> tokenizer_t;

private:
	int line;

protected:
	tokenizer_t literal;
	tokenizer_t symbol;
	tokenizer_t reserved;
	
public:
	ScannerTrait():line(0) {}

	int getLineCount() const
	{
		return line;
	}

	void lineCount(

	
	
};

#endif /* SCANNER_TRAINTS_HPP_ */

#ifndef SCANNER_TRAITS_HPP_
#define SCANNER_TRAITS_HPP_

#include <string>

enum ScannerSymbols
{
	alphabet,
	symbol,
	number,
	undisplayable_symbol,
	
};

/**
 * scanning action traits.
 */
class ScannerTrait
{
private:
	int line;

public:
	ScannerTrait():line(0) {}

	int getLineCount() const
	{
		return line;
	}

	
	
};

#endif /* SCANNER_TRAINTS_HPP_ */

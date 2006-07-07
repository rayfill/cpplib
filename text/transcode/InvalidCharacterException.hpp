#ifndef INVALIDCHARACTEREXCEPTION_HPP_
#define INVALIDCHARACTEREXCEPTION_HPP_

#include <stdexcept>

class InvalidCharacterException : public std::runtime_error
{
public:
	InvalidCharacterException(const char* reason =
							  "invalid character found to converting."):
		std::runtime_error(reason)
	{}
};

#endif /* INVALIDCHARACTEREXCEPTION_HPP_ */

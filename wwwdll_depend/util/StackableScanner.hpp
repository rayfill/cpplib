#ifndef STACKABLESCANNER_HPP_
#define STACKABLESCANNER_HPP_

#include <stack>
#include <util/Scanner.hpp>
#include <cassert>

template <
	typename CharType,
	typename CharTrait = std::char_traits<CharType>
>
class StackableScanner : public Scanner<CharType, CharTrait>
{
public:
	typedef Scanner<CharType> BaseType;
	typedef typename BaseType::token_t token_t;
	typedef typename BaseType::iterator_t iterator_t;
	typedef typename BaseType::char_t char_t;

protected:
	typedef typename BaseType::char_trait_t char_trait_t;

private:
	std::stack<token_t> tokenStack;

public:
	StackableScanner(iterator_t head_,
					 iterator_t last_):
		Scanner<CharType>(head_, last_), tokenStack()
	{}

	~StackableScanner()
	{}

	token_t scan()
	{
		if (tokenStack.size() != 0)
		{
			token_t result = tokenStack.top();
			tokenStack.pop();

			return result;
		}

		while (true)
		{
			token_t result = BaseType::scan();
			if (result == token_t::COMMENTS ||
				result == token_t::IGNORE_SPACES)
				continue;
				
			return result;
		}

		assert(false);
	}

	void backFail(token_t token)
	{
		tokenStack.push(token);
	}
};

#endif /* STACKABLESCANNER_HPP_ */

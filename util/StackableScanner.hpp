#ifndef STACKABLESCANNER_HPP_
#define STACKABLESCANNER_HPP_

#include <stack>
#include <util/Scanner.hpp>

template <typename CharType>
class StackableScanner : public Scanner<CharType>
{
public:
	typedef Scanner<CharType> BaseType;

private:
	std::stack<typename BaseType::token_t> tokenStack;

public:
	StackableScanner(typename BaseType::iterator_t head_,
					 typename BaseType::iterator_t last_):
		Scanner<CharType>(head_, last_), tokenStack()
	{}

	~StackableScanner()
	{}

	typename BaseType::token_t scan()
	{
		if (tokenStack.size() != 0)
		{
			typename BaseType::token_t result = tokenStack.top();
			tokenStack.pop();

			return result;
		}

		while (true)
		{
			typename BaseType::token_t result = BaseType::scan();
			if (result == BaseType::token_t::COMMENTS ||
				result == BaseType::token_t::IGNORE_SPACES)
				continue;
				
			return result;
		}

		assert(false);
	}

	void backFail(typename BaseType::token_t token)
	{
		tokenStack.push(token);
	}
};

#endif /* STACKABLESCANNER_HPP_ */

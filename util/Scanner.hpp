#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <util/FSM.hpp>
#include <text/LexicalCast.hpp>
#include <string>
#include <stdexcept>

class ParseException : public std::runtime_error
{
private:
	int line;
	int column;

public:
	ParseException(const char* reason = "parsing exception."):
		std::runtime_error(reason), line(-1), column(-1)
	{}

	ParseException(const unsigned int line_, const unsigned int column_):
		std::runtime_error("parsing exception: " +
						   stringCast(line_) + "," +
						   stringCast(column_)),
		line(line_), column(column_)
	{}

	virtual ~ParseException() throw()
	{}

	int getLine() const
	{
		return line;
	}

	int getColumn() const
	{
		return column;
	}
};

template <
	typename CharType,
	typename CharTrait = std::char_traits<CharType> >
class Token
{
public:
	typedef enum
	{
		NONE,
		LITERAL,
		IF,
		ELSE,
		WHILE,
		FOR,
		CONTINUE,
		BREAK,
	} TokenType;
private:
	std::basic_string<CharType> token;
	TokenType typeId;

public:
	Token(const TokenType& typeId_,
		  const std::basic_string<CharType>& token_ =
		  std::basic_string<CharType>()):
		token(token_), typeId(typeId_)
	{}

	std::basic_string<CharType> getToken()
	{
		return token;
	}

	const TokenType getId()
	{
		return typeId;
	}
};



template <
	typename CharType,
	typename CharTrait = std::char_traits<CharType> >
class Scanner
{
	friend class ScannerTest;

public:
	typedef typename std::basic_string<CharType> ScannerString;
	typedef typename ScannerString::iterator iterator;

	typedef Token<CharType> token_t;

private:
	typedef FiniteStateMachine<CharType> fsm_t;
	typedef CharTrait char_trait_t;

	fsm_t literalFsm;
	fsm_t symbolFsm;

	static void literalFsmInit(fsm_t& fsm)
	{
		// literal is "^[_a-zA-Z][_a-zA-Z0-9]*$"(reglar expression reps).
		typename fsm_t::state_t* state = fsm.getHeadState(); // get root state.
		typename fsm_t::state_t* secondLiteral =
			new typename fsm_t::state_t();
		
		// small alphabet 'a' to 'z'
		for (CharType smallAlphabet = char_trait_t::to_char_type('a');
			 smallAlphabet <= char_trait_t::to_char_type('z');
			 ++smallAlphabet)
		{
			state->setTransit(smallAlphabet, secondLiteral);
			secondLiteral->setTransit(smallAlphabet, secondLiteral);
		}

		// big alphabet 'A' to 'Z'
		for (CharType bigAlphabet = char_trait_t::to_char_type('A');
			 bigAlphabet <= char_trait_t::to_char_type('Z');
			 ++bigAlphabet)
		{
			state->setTransit(bigAlphabet, secondLiteral);
			secondLiteral->setTransit(bigAlphabet, secondLiteral);
		}

		// number '0' to '9'
		for (CharType number = char_trait_t::to_char_type('0');
			 number <= char_trait_t::to_char_type('9');
			 ++number)
		{
			secondLiteral->setTransit(number, secondLiteral);
		}

		state->setId(token_t::LITERAL);
		secondLiteral->setId(token_t::LITERAL);
	}

	static void symbolFsmInit(fsm_t& fsm)
	{
		const std::string If("if");
		fsm.add(If.begin(), If.end(), token_t::IF);

		const std::string Else("else");
		fsm.add(Else.begin(), Else.end(), token_t::ELSE);

		const std::string While("while");
		fsm.add(While.begin(), While.end(), token_t::WHILE);

		const std::string For("for");
		fsm.add(For.begin(), For.end(), token_t::FOR);

		const std::string Continue("continue");
		fsm.add(Continue.begin(), Continue.end(), token_t::CONTINUE);

		const std::string Break("break");
		fsm.add(Break.begin(), Break.end(), token_t::BREAK);
	}

public:
	Scanner():
		literalFsm(), symbolFsm()
	{
		literalFsmInit(literalFsm);
		symbolFsmInit(symbolFsm);
	}

	template <typename Iterator>
	std::vector<token_t> scan(Iterator first, Iterator last)
	{
		std::vector<token_t> result;

		typename fsm_t::state_t* symbolState = symbolFsm.getHeadState();
		typename fsm_t::state_t* literalState = literalFsm.getHeadState();
		std::basic_string<CharType> findLiteral;
		unsigned int line = 0;
		unsigned int column = 0;

		for (; first != last; ++first)
		{
			if (*first == '\n')
			{
				++line;
				column = 0;
				continue;
			}
			else
				++column;
			
			// skip speces.
			if (*first == ' ' ||
				*first == '\t')
			{
				if (findLiteral.length() != 0)
					findLiteral = "";

				continue;
			}

			findLiteral += *first;

			// symbol processing.
			if (symbolState != NULL)
			{
				symbolState = symbolState->getTransit(*first);
				
				if (symbolState != NULL &&
					!symbolState->isTransitionable(*(first+1)) &&
					symbolState->getId() != token_t::NONE)
				{
					result.push_back(
						token_t(
							static_cast<typename token_t::TokenType>(
								symbolState->getId())));
					symbolState = symbolFsm.getHeadState();
					literalState = literalFsm.getHeadState();
					continue;
				}
			}

			// literal processing.
			if (literalState != NULL)
			{
				literalState = literalState->getTransit(*first);
				if (literalState != NULL &&
					!literalState->isTransitionable(*(first+1)) &&
					literalState->getId() != token_t::NONE)
				{
					result.push_back(
						token_t(
							static_cast<typename token_t::TokenType>(
								literalState->getId()),	findLiteral));
					symbolState = symbolFsm.getHeadState();
					literalState = literalFsm.getHeadState();
					continue;
				}
			}

			if (symbolState == NULL &&
				literalState == NULL)
				throw ParseException(line, column);
		}

		return result;
	}
};

#endif /* SCANNER_HPP_ */

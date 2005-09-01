#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <util/FSM.hpp>
#include <text/LexicalCast.hpp>
#include <text/StringTraits.hpp>
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
		STRING,
		CHARACTER,
		IF,
		ELSE,
		WHILE,
		FOR,
		CONTINUE,
		BREAK,
		// symbols
		PLUS,
		MINUS,
		STAR,
		SLASH,
		EQUAL,
		PERCENT,
		AMPERSAND,
		LESS_THAN,
		GRATER_THAN,

		LESS_EQUAL,
		GRATER_EQUAL,
		NOT_EQUAL,
		PLUS_EQUAL,
		MINUS_EQUAL,
		STAR_EQUAL,
		SLASH_EQAUL,
		PERCENT_EQUAL,

		TILDE,
		HAT,
		EXCLAMATION,
		QUESTION,
		OPEN_PAREN,
		CLOSE_PAREN,
		OPEN_BLACKET,
		CLOSE_BLACKET,
		OPEN_BLACE,
		CLOSE_BLACE,
		
		// end of stream.
		END_OF_STREAM,

		// ignore spaces.
		IGNORE_SPACES,
		// COMMENTS
		COMMENTS,

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

	bool operator==(const TokenType type)
	{
		return this->getId() == type;
	}

	bool operator!=(const TokenType type)
	{
		return this->getId() != type;
	}
};

template <
	typename CharType,
	typename CharTrait = std::char_traits<CharType>,
	typename Iterator = typename std::basic_string<CharType>::iterator>
class Scanner
{
	friend class ScannerTest;

public:
	typedef typename std::basic_string<CharType> ScannerString;
	typedef typename ScannerString::iterator iterator;
	typedef CharType char_t;

	typedef Token<CharType> token_t;
	typedef Iterator iterator_t;

private:
	typedef FiniteStateMachine<CharType> fsm_t;
	typedef CharTrait char_trait_t;

	fsm_t fsm;

	iterator_t current;
	iterator_t last;
	unsigned int line;
	unsigned int column;

	static void addState(fsm_t& fsm,
						 const std::basic_string<char_t>& str,
						 typename token_t::TokenType type)
	{
		fsm.add(str.begin(), str.end(), type);
	}

	static void literalFsmInit(fsm_t& fsm)
	{
		// literal is "^[_a-zA-Z][_a-zA-Z0-9]*$"(regular expression reps).
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

	static void keywordFsmInit(fsm_t& fsm)
	{
		addState(fsm, StringTraits<char_t>("if"), token_t::IF);
		addState(fsm, StringTraits<char_t>("else"), token_t::ELSE);
		addState(fsm, StringTraits<char_t>("while"), token_t::WHILE);
		addState(fsm, StringTraits<char_t>("for"), token_t::FOR);
		addState(fsm, StringTraits<char_t>("continue"), token_t::CONTINUE);
		addState(fsm, StringTraits<char_t>("break"), token_t::BREAK);
	}

	static void stringsFsmInit(fsm_t& fsm)
	{
		typename fsm_t::state_t* state = fsm.getHeadState(); // get root state.

		typename fsm_t::state_t* quotation =
			new typename fsm_t::state_t();
		typename fsm_t::state_t* singleQuotation =
			new typename fsm_t::state_t();
		typename fsm_t::state_t* metaProcessing = 
			new typename fsm_t::state_t();
		typename fsm_t::state_t* singleMetaProcessing = 
			new typename fsm_t::state_t();
		typename fsm_t::state_t* hexadecimalProcessing =
			new typename fsm_t::state_t();
		typename fsm_t::state_t* hexadecimalProcessing2 =
			new typename fsm_t::state_t();
		typename fsm_t::state_t* singleHexadecimal =
			new typename fsm_t::state_t();
		typename fsm_t::state_t* singleHexadecimal2 =
			new typename fsm_t::state_t();
		typename fsm_t::state_t* endQuotation =
			new typename fsm_t::state_t();
		typename fsm_t::state_t* endCharacter =
			new typename fsm_t::state_t();
		typename fsm_t::state_t* endSingleQuotation =
			new typename fsm_t::state_t();

		state->setTransit('"', quotation);
		state->setTransit('\'', singleQuotation);

		quotation->setDefaultTransit(quotation);
		quotation->setTransit('\\', metaProcessing);

		singleQuotation->setDefaultTransit(endCharacter);
		singleQuotation->setTransit('\\', singleMetaProcessing);

		singleMetaProcessing->setTransit('\\', endCharacter);
		singleMetaProcessing->setTransit('n', endCharacter);
		singleMetaProcessing->setTransit('t', endCharacter);
		singleMetaProcessing->setTransit('\'', endCharacter);
		singleMetaProcessing->setTransit('x', singleHexadecimal);
		
		metaProcessing->setTransit('\\', quotation);
		metaProcessing->setTransit('n', quotation);
		metaProcessing->setTransit('t', quotation);
		metaProcessing->setTransit('"', quotation);
		metaProcessing->setTransit('x', hexadecimalProcessing);

		for (CharType c = char_trait_t::to_char_type('0');
			 c <= char_trait_t::to_char_type('9');
			 ++c)
		{
			hexadecimalProcessing->setTransit(c, hexadecimalProcessing2);
			hexadecimalProcessing2->setTransit(c, quotation);
			singleHexadecimal->setTransit(c, singleHexadecimal2);
			singleHexadecimal2->setTransit(c, endCharacter);
		}
		for (CharType c = char_trait_t::to_char_type('a');
			 c <= char_trait_t::to_char_type('f');
			 ++c)
		{
			hexadecimalProcessing->setTransit(c, hexadecimalProcessing2);
			hexadecimalProcessing2->setTransit(c, quotation);
			singleHexadecimal->setTransit(c, singleHexadecimal2);
			singleHexadecimal2->setTransit(c, endCharacter);
		}
		for (CharType c = char_trait_t::to_char_type('A');
			 c <= char_trait_t::to_char_type('F');
			 ++c)
		{
			hexadecimalProcessing->setTransit(c, hexadecimalProcessing2);
			hexadecimalProcessing2->setTransit(c, quotation);
			singleHexadecimal->setTransit(c, singleHexadecimal2);
			singleHexadecimal2->setTransit(c, endCharacter);
		}

		quotation->setTransit('"', endQuotation);
		endCharacter->setTransit('\'', endSingleQuotation);

		endQuotation->setId(token_t::STRING);
		endSingleQuotation->setId(token_t::CHARACTER);
	}

	static void symbolFsmInit(fsm_t& fsm)
	{
		addState(fsm, StringTraits<char_t>("+"), token_t::PLUS);
		addState(fsm, StringTraits<char_t>("-"), token_t::MINUS);
		addState(fsm, StringTraits<char_t>("*"), token_t::STAR);
		addState(fsm, StringTraits<char_t>("/"), token_t::SLASH);
		addState(fsm, StringTraits<char_t>("="), token_t::EQUAL);
		addState(fsm, StringTraits<char_t>("%"), token_t::PERCENT);
		addState(fsm, StringTraits<char_t>("&"), token_t::AMPERSAND);
		addState(fsm, StringTraits<char_t>("<"), token_t::LESS_THAN);
		addState(fsm, StringTraits<char_t>(">"), token_t::GRATER_THAN);
		addState(fsm, StringTraits<char_t>("<="), token_t::LESS_EQUAL);
		addState(fsm, StringTraits<char_t>(">="), token_t::GRATER_EQUAL);
		addState(fsm, StringTraits<char_t>("!="), token_t::NOT_EQUAL);
		addState(fsm, StringTraits<char_t>("+="), token_t::PLUS_EQUAL);
		addState(fsm, StringTraits<char_t>("-="), token_t::MINUS_EQUAL);
		addState(fsm, StringTraits<char_t>("*="), token_t::STAR_EQUAL);
		addState(fsm, StringTraits<char_t>("/="), token_t::SLASH_EQAUL);
		addState(fsm, StringTraits<char_t>("&="), token_t::PERCENT_EQUAL);

		addState(fsm, StringTraits<char_t>("~"), token_t::TILDE);
		addState(fsm, StringTraits<char_t>("^"), token_t::HAT);
		addState(fsm, StringTraits<char_t>("!"), token_t::EXCLAMATION);
		addState(fsm, StringTraits<char_t>("?"), token_t::QUESTION);

		addState(fsm, StringTraits<char_t>("("), token_t::OPEN_PAREN);
		addState(fsm, StringTraits<char_t>(")"), token_t::CLOSE_PAREN);
		addState(fsm, StringTraits<char_t>("["), token_t::OPEN_BLACKET);
		addState(fsm, StringTraits<char_t>("]"), token_t::CLOSE_BLACKET);
		addState(fsm, StringTraits<char_t>("{"), token_t::OPEN_BLACE);
		addState(fsm, StringTraits<char_t>("}"), token_t::CLOSE_BLACE);

	}

	static void ignoreFsmInit(fsm_t& fsm)
	{}

	static void init(fsm_t& fsm)
	{
		literalFsmInit(fsm);
		keywordFsmInit(fsm);
		symbolFsmInit(fsm);
		stringsFsmInit(fsm);
		ignoreFsmInit(fsm);

		fsm.removeOrphanedStates();
	}

public:
	Scanner(iterator_t head_, iterator_t last_):
		fsm(),
		current(head_), last(last_),
		line(0), column(0)
	{
		init(fsm);
	}

	token_t scan()
	{
		typename fsm_t::state_t* fsmState = fsm.getHeadState();
		std::basic_string<CharType> findLiteral;

		for (; current != last; ++current)
		{
			if (*current == '\n')
			{
				++line;
				column = 0;
			}
			else
				++column;
			
			// skip speces.
			if (*current == ' ' ||
				*current == '\t' ||
				*current == '\n')
			{
				if (findLiteral.length() != 0)
					findLiteral += *current;

				continue;
			}

			findLiteral += *current;

			// literal processing.
			if (fsmState != NULL)
			{
				fsmState = fsmState->getTransit(*current);
				if (fsmState != NULL &&
					fsmState->getId() != token_t::NONE &&
					(current+1 == last ||
					 !fsmState->isTransitionable(*(current+1))))
				{
					++current;
					return token_t(
						static_cast<typename token_t::TokenType>(
							fsmState->getId()),	findLiteral);
				}
			}

			if (fsmState == NULL)
				throw ParseException(line, column);
		}

		if ((fsmState == fsm.getHeadState()) &&
			(current == last))
		return token_t(token_t::END_OF_STREAM, "");
		
			
		throw ParseException(line, column);
	}
};

#endif /* SCANNER_HPP_ */

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

	fsm_t fsm;

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
		const std::string plus = "+";
		fsm.add(plus.begin(), plus.end(), token_t::PLUS);
		const std::string minus = "-";
		fsm.add(minus.begin(), minus.end(), token_t::MINUS);
		const std::string star = "*";
		fsm.add(star.begin(), star.end(), token_t::STAR);
		const std::string slash = "/";
		fsm.add(slash.begin(), slash.end(), token_t::SLASH);
		const std::string equal = "=";
		fsm.add(equal.begin(), equal.end(), token_t::EQUAL);
		const std::string percent = "%";
		fsm.add(percent.begin(), percent.end(), token_t::PERCENT);
		const std::string ampersand = "&";
		fsm.add(ampersand.begin(), ampersand.end(), token_t::AMPERSAND);
		const std::string less_than = "<";
		fsm.add(less_than.begin(), less_than.end(), token_t::LESS_THAN);
		const std::string grater_than = ">";
		fsm.add(grater_than.begin(), grater_than.end(), token_t::GRATER_THAN);
		const std::string less_equal = "<=";
		fsm.add(less_equal.begin(), less_equal.end(), token_t::LESS_EQUAL);
		const std::string grater_equal = ">=";
		fsm.add(grater_equal.begin(), grater_equal.end(), token_t::GRATER_EQUAL);
		const std::string not_equal = "!=";
		fsm.add(not_equal.begin(), not_equal.end(), token_t::NOT_EQUAL);
		const std::string plus_equal = "+=";
		fsm.add(plus_equal.begin(), plus_equal.end(), token_t::PLUS_EQUAL);
		const std::string minus_equal = "-=";
		fsm.add(minus_equal.begin(), minus_equal.end(), token_t::MINUS_EQUAL);
		const std::string star_equal = "*=";
		fsm.add(star_equal.begin(), star_equal.end(), token_t::STAR_EQUAL);
		const std::string slash_equal = "/=";
		fsm.add(slash_equal.begin(), slash_equal.end(), token_t::SLASH_EQAUL);
		const std::string percent_equal = "&=";
		fsm.add(percent_equal.begin(), percent_equal.end(), token_t::PERCENT_EQUAL);

		const std::string tilde = "~";
		fsm.add(tilde.begin(), tilde.end(), token_t::TILDE);
		const std::string hat = "^";
		fsm.add(hat.begin(), hat.end(), token_t::HAT);
		const std::string exclamation = "!";
		fsm.add(exclamation.begin(), exclamation.end(), token_t::EXCLAMATION);
		const std::string question = "?";
		fsm.add(question.begin(), question.end(), token_t::QUESTION);

		const std::string open_paren = "(";
		fsm.add(open_paren.begin(), open_paren.end(), token_t::OPEN_PAREN);
		const std::string close_paren = ")";
		fsm.add(close_paren.begin(), close_paren.end(), token_t::CLOSE_PAREN);
		const std::string open_blacket = "[";
		fsm.add(open_blacket.begin(), open_blacket.end(), token_t::OPEN_BLACKET);
		const std::string close_blacket = "]";
		fsm.add(close_blacket.begin(), close_blacket.end(), token_t::CLOSE_BLACKET);
		const std::string open_blace = "{";
		fsm.add(open_blace.begin(), open_blace.end(), token_t::OPEN_BLACE);
		const std::string close_blace = "}";
		fsm.add(close_blace.begin(), close_blace.end(), token_t::CLOSE_BLACE);

		fsm.removeOrphanedStates();
	}

	static void init(fsm_t& fsm)
	{
		literalFsmInit(fsm);
		keywordFsmInit(fsm);
		symbolFsmInit(fsm);
		stringsFsmInit(fsm);

		fsm.removeOrphanedStates();
	}

public:
	Scanner():
		fsm()
	{
		init(fsm);
	}

	template <typename Iterator>
	std::vector<token_t> scan(Iterator first, Iterator last)
	{
		std::vector<token_t> result;

		typename fsm_t::state_t* fsmState = fsm.getHeadState();
		std::basic_string<CharType> findLiteral;
		unsigned int line = 0;
		unsigned int column = 0;

		for (; first != last; ++first)
		{
			if (*first == '\n')
			{
				++line;
				column = 0;
			}
			else
				++column;
			
			// skip speces.
			if (*first == ' ' ||
				*first == '\t' ||
				*first == '\n')
			{
				if (findLiteral.length() != 0)
					findLiteral += *first;

				continue;
			}

			findLiteral += *first;

			// literal processing.
			if (fsmState != NULL)
			{
				fsmState = fsmState->getTransit(*first);
				if (fsmState != NULL &&
					!fsmState->isTransitionable(*(first+1)) &&
					fsmState->getId() != token_t::NONE)
				{
					result.push_back(
						token_t(
							static_cast<typename token_t::TokenType>(
								fsmState->getId()),	findLiteral));
					fsmState = fsm.getHeadState();
					findLiteral = "";
					continue;
				}
			}

			if (fsmState == NULL)
				throw ParseException(line, column);
		}

		return result;
	}
};

#endif /* SCANNER_HPP_ */

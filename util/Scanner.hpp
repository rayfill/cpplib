#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <util/FSM.hpp>
#include <text/LexicalCast.hpp>
#include <text/StringTraits.hpp>
#include <string>
#include <stdexcept>

/**
 * 構文解析例外
 */
class ParseException : public std::runtime_error
{
private:
	/**
	 * 例外位置の行数
	 */

	int line;
	/**
	 * 例外位置のカラム数
	 */
	int column;

public:
	/**
	 * コンストラクタ
	 * @param reason 例外理由
	 */
	ParseException(const char* reason = "parsing exception."):
		std::runtime_error(reason), line(-1), column(-1)
	{}

	/**
	 * コンストラクタ
	 * @param line_ 行数
	 * @param column_ 列数
	 */
	ParseException(const unsigned int line_, const unsigned int column_):
			std::runtime_error("parsing exception"),
			line(line_), column(column_)
	{}

	/**
	 * デストラクタ
	 */
	virtual ~ParseException() throw()
	{}

	/**
	 * 行の取得
	 * @return 例外位置の行数
	 */
	int getLine() const
	{
		return line;
	}

	/**
	 * 列の取得
	 * @return 例外位置の列数
	 */
	int getColumn() const
	{
		return column;
	}
};

/**
 * 汎用トークンクラス
 * @param CharType 文字型
 * @param CharTrait 文字列特徴クラス
 * @todo 汎用にするにはもう少し設計いじらないとダメか･･･
 */
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
		INTEGER,
		REAL,
		IF,
		ELSE,
		WHILE,
		FOR,
		CONTINUE,
		BREAK,
		// symbols
		SEMI_COLON,
		COLON,

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
		
		INCREMENT,
		DECREMENT,

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
		COMMENTS

	} TokenType;
private:
	/**
	 * トークン文字列
	 */
	std::basic_string<CharType> token;

	/**
	 * トークン識別子
	 */
	TokenType typeId;

public:
	/**
	 * コンストラクタ
	 * @param typeId トークン識別子 
	 * @param token_ トークン文字列
	 */
	Token(const TokenType& typeId_,
		  const std::basic_string<CharType>& token_ =
		  std::basic_string<CharType>()):
		token(token_), typeId(typeId_)
	{}

	/**
	 * トークン文字列の取得
	 * @return トークン文字列
	 */
	std::basic_string<CharType> getToken()
	{
		return token;
	}

	/**
	 * トークン識別子の取得
	 * @return トークン識別子
	 */
	const TokenType getId()
	{
		return typeId;
	}

	/**
	 * トークン等価比較
	 * @param type 比較対象トークン
	 * @return 一致しているならtrue
	 */
	bool operator==(const TokenType type)
	{
		return this->getId() == type;
	}

	/**
	 * トークン等価比較
	 * @param type 比較対象トークン
	 * @return 一致していないならtrue
	 */
	bool operator!=(const TokenType type)
	{
		return this->getId() != type;
	}
};

/**
 * スキャナクラス
 * @todo 解析用FSM構築のための構文とそのパーサルーチンの作成。
 * 正規表現クラス完成させてNFA->DFA変換作れば何とかなるか？
 */
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

protected:
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
		addState(fsm, StringTraits<char_t>().stringTraits("if"),
				 token_t::IF);
		addState(fsm, StringTraits<char_t>().stringTraits("else"),
				 token_t::ELSE);
		addState(fsm, StringTraits<char_t>().stringTraits("while"),
				 token_t::WHILE);
		addState(fsm, StringTraits<char_t>().stringTraits("for"),
				 token_t::FOR);
		addState(fsm, StringTraits<char_t>().stringTraits("continue"),
				 token_t::CONTINUE);
		addState(fsm, StringTraits<char_t>().stringTraits("break"),
				 token_t::BREAK);
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
		addState(fsm, StringTraits<char_t>().stringTraits(";"),
				 token_t::SEMI_COLON);
		addState(fsm, StringTraits<char_t>().stringTraits(":"),
				 token_t::COLON);
		
		addState(fsm, StringTraits<char_t>().stringTraits("+"),
				 token_t::PLUS);
		addState(fsm, StringTraits<char_t>().stringTraits("-"),
				 token_t::MINUS);
		addState(fsm, StringTraits<char_t>().stringTraits("*"),
				 token_t::STAR);
		addState(fsm, StringTraits<char_t>().stringTraits("/"),
				 token_t::SLASH);
		addState(fsm, StringTraits<char_t>().stringTraits("="),
				 token_t::EQUAL);
		addState(fsm, StringTraits<char_t>().stringTraits("%"),
				 token_t::PERCENT);
		addState(fsm, StringTraits<char_t>().stringTraits("&"),
				 token_t::AMPERSAND);
		addState(fsm, StringTraits<char_t>().stringTraits("<"),
				 token_t::LESS_THAN);
		addState(fsm, StringTraits<char_t>().stringTraits(">"),
				 token_t::GRATER_THAN);
		addState(fsm, StringTraits<char_t>().stringTraits("<="),
				 token_t::LESS_EQUAL);
		addState(fsm, StringTraits<char_t>().stringTraits(">="),
				 token_t::GRATER_EQUAL);
		addState(fsm, StringTraits<char_t>().stringTraits("!="),
				 token_t::NOT_EQUAL);
		addState(fsm, StringTraits<char_t>().stringTraits("+="), 
				 token_t::PLUS_EQUAL);
		addState(fsm, StringTraits<char_t>().stringTraits("-="),
				 token_t::MINUS_EQUAL);
		addState(fsm, StringTraits<char_t>().stringTraits("*="),
				 token_t::STAR_EQUAL);
		addState(fsm, StringTraits<char_t>().stringTraits("/="),
				 token_t::SLASH_EQAUL);
		addState(fsm, StringTraits<char_t>().stringTraits("&="),
				 token_t::PERCENT_EQUAL);

		addState(fsm, StringTraits<char_t>().stringTraits("++"),
				 token_t::INCREMENT);
		addState(fsm, StringTraits<char_t>().stringTraits("--"),
				 token_t::DECREMENT);

		addState(fsm, StringTraits<char_t>().stringTraits("~"),
				 token_t::TILDE);
		addState(fsm, StringTraits<char_t>().stringTraits("^"),
				 token_t::HAT);
		addState(fsm, StringTraits<char_t>().stringTraits("!"),
				 token_t::EXCLAMATION);
		addState(fsm, StringTraits<char_t>().stringTraits("?"),
				 token_t::QUESTION);

		addState(fsm, StringTraits<char_t>().stringTraits("("),
				 token_t::OPEN_PAREN);
		addState(fsm, StringTraits<char_t>().stringTraits(")"),
				 token_t::CLOSE_PAREN);
		addState(fsm, StringTraits<char_t>().stringTraits("["),
				 token_t::OPEN_BLACKET);
		addState(fsm, StringTraits<char_t>().stringTraits("]"),
				 token_t::CLOSE_BLACKET);
		addState(fsm, StringTraits<char_t>().stringTraits("{"),
				 token_t::OPEN_BLACE);
		addState(fsm, StringTraits<char_t>().stringTraits("}"),
				 token_t::CLOSE_BLACE);

	}

	static void numberFsmInit(fsm_t& fsm)
	{
		typedef typename fsm_t::state_t state_t;
		
		state_t* state = fsm.getHeadState();

		state_t* integerBody = new state_t();
		state_t* integerZero = new state_t();
		state_t* decimalPoint = new state_t();
		state_t* decimalFraction = new state_t();

		CharType period = char_trait_t::to_char_type('.');
		integerZero->setTransit(period, decimalPoint);
		integerBody->setTransit(period, decimalPoint);

		CharType zero = char_trait_t::to_char_type('0');

		state->setTransit(zero, integerZero);

		integerBody->setTransit(zero, integerBody);
		decimalPoint->setTransit(zero, decimalFraction);
		decimalFraction->setTransit(zero, decimalFraction);

		for (CharType c = char_trait_t::to_char_type('1');
			 c <= char_trait_t::to_char_type('9');
			 ++c)
		{
			state->setTransit(c, integerBody);
			integerBody->setTransit(c, integerBody);
			decimalPoint->setTransit(c, decimalFraction);
			decimalFraction->setTransit(c, decimalFraction);
		}

		// goal
		integerZero->setId(token_t::INTEGER);
		integerBody->setId(token_t::INTEGER);
		decimalPoint->setId(token_t::REAL);
		decimalFraction->setId(token_t::REAL);
	}

	static void ignoreFsmInit(fsm_t& fsm)
	{
		typedef typename fsm_t::state_t state_t;

		state_t* head = fsm.getHeadState();

		state_t* comment_start = new state_t();

		state_t* open_star = new state_t();
		state_t* close_star = new state_t();
		state_t* close_slash = new state_t();

		state_t* open_slash = new state_t();
		state_t* close_return = new state_t();

		CharType slash = char_trait_t::to_char_type('/');
		CharType star = char_trait_t::to_char_type('*');
		CharType return_character = char_trait_t::to_char_type('\n');


		head->setTransit(slash, comment_start);
		comment_start->setTransit(slash, open_slash);

		// single line comment.
		open_slash->setDefaultTransit(open_slash);
		open_slash->setTransit(return_character, close_return);
		close_return->setId(token_t::COMMENTS);

		// multi line comment.
		comment_start->setTransit(star, open_star);
		open_star->setDefaultTransit(open_star);
		open_star->setTransit(star, close_star);
		close_star->setDefaultTransit(open_star);
		close_star->setTransit(star, close_star);
		close_star->setTransit(slash, close_slash);
		close_slash->setId(token_t::COMMENTS);


		// spaces.
		state_t* skippers = new state_t();
		CharType space = char_trait_t::to_char_type(' ');
		CharType tab = char_trait_t::to_char_type('\t');
		head->setTransit(space, skippers);
		head->setTransit(tab, skippers);
		head->setTransit(return_character, skippers);
		skippers->setTransit(space, skippers);
		skippers->setTransit(tab, skippers);
		skippers->setTransit(return_character, skippers);
		skippers->setId(token_t::IGNORE_SPACES);
	}

	static void init(fsm_t& fsm)
	{
		literalFsmInit(fsm);
		stringsFsmInit(fsm);
		numberFsmInit(fsm);
		ignoreFsmInit(fsm);

		keywordFsmInit(fsm);
		symbolFsmInit(fsm);

		fsm.removeOrphanedStates();
	}

	Scanner(const Scanner& rhs);
	Scanner& operator=(const Scanner& rhs);
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
		return token_t(token_t::END_OF_STREAM,
					   StringTraits<char_t>().stringTraits(""));
		
			
		throw ParseException(line, column);
	}
};

#endif /* SCANNER_HPP_ */

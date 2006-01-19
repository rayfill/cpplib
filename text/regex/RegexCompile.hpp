#ifndef REGEXCOMPILE_HPP_
#define REGEXCOMPILE_HPP_

#include <string>
#include <stdexcept>
#include <iterator>
#include <sstream>
#include <vector>
#include <list>
#include <utility>
#include <set>

#include <cassert>

class CompileError : public std::runtime_error
{
public:
	CompileError(const char* reason): std::runtime_error(reason)
	{}

	virtual ~CompileError() throw()
	{}

	template <typename iterator_t>
	static CompileError newInstance(iterator_t base,
									iterator_t offset)
	{
		const size_t distance =
			static_cast<size_t>(std::distance(base, offset));

		std::stringstream ss;
		ss << distance;

		return CompileError("Compile error near than " + ss.str());
	}
};

template <typename CharType>
class RegexScanner
{
public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> char_trait_t;
	typedef std::basic_string<char_t> string_t;
	typedef typename string_t::iterator internal_iterator_t;

private:
	RegexScanner();
	RegexScanner(const RegexScanner&);
	RegexScanner& operator=(const RegexScanner&);

	internal_iterator_t head;
	internal_iterator_t current;
	internal_iterator_t last;

public:
	RegexScanner(internal_iterator_t head_,
				 internal_iterator_t last_):
		head(head_), current(head_), last(last_)
	{}

	size_t getPosition() const
	{
		return static_cast<size_t>(std::distance(head, current));
	}

	typename char_trait_t::int_type scan()
	{
		if (current != last)
			return char_trait_t::to_int_type(*current++);

		return static_cast<typename char_trait_t::int_type>(-1);
	}

	typename char_trait_t::int_type read()
	{
		if (current != last)
			return char_trait_t::to_int_type(*current);

		return static_cast<typename char_trait_t::int_type>(-1);
	}

	typename char_trait_t::int_type readAhead()
	{
		if (current + 1 != last)
			return char_trait_t::to_int_type(*(current + 1));

		return static_cast<typename char_trait_t::int_type>(-1);
	}

	void backTrack()
	{
		if (current != head)
			--current;
		else
			throw std::runtime_error("under flow exception.");
	}

	void reset()
	{
		current = head;
	}
};

template <typename CharType>
class RegexResult
{
	friend class RegexResultTest;

public:
	typedef typename std::basic_string<CharType>::size_type offset_t;
	typedef CharType char_t;

	struct OffsetPair
	{
	private:
		offset_t head;
		offset_t last;

		typedef std::basic_string<CharType> string_t;

	public:
		OffsetPair(offset_t head_ = 0, offset_t last_ = string_t::npos)
			: head(head_), last(last_)
		{
			if (this->getHead() > this->getLast())
				throw std::invalid_argument("head after than last offset.");
		}

		OffsetPair(const OffsetPair& source)
			: head(source.head), last(source.last)
		{}

		OffsetPair& operator=(const OffsetPair& source)
		{
			if (this != &source)
			{
				this->head = source.head;
				this->last = source.last;
			}
			return *this;
		}

		void setHead(const offset_t newHead)
		{
			head = newHead;
		}

		void setLast(const offset_t newLast)
		{
			last = newLast;
		}

		offset_t getHead() const
		{
			return head;
		}

		offset_t getLast() const
		{
			return last;
		}

		string_t getString(const string_t& str) const
		{
			if (this->getLast() > str.size())
				throw std::out_of_range("index is out of str.");

			const offset_t distance = this->getLast() - this->getHead();
			return str.substr(this->getHead(), distance);
		}
	};

private:
	std::vector<OffsetPair> captures;

	typedef int group_offset_t;
public:
	void setCapture(group_offset_t captureGroupNumber, const OffsetPair offsets)
	{
		assert(captures.size() >= static_cast<offset_t>(captureGroupNumber));

		if (captureGroupNumber >= captures.size())
			captures.resize(captureGroupNumber + 1);

		captures[captureGroupNumber] = offsets;
	}

	void setCaptureHead(group_offset_t captureGroupNumber, const offset_t head)
	{
		assert(captures.size() >= static_cast<offset_t>(captureGroupNumber));

		if (static_cast<offset_t>(captureGroupNumber) >= captures.size())
			captures.resize(captureGroupNumber + 1);

		captures[captureGroupNumber] = OffsetPair(head);
	}

	void setCaptureLast(group_offset_t captureGroupNumber, const offset_t last)
	{
		assert (static_cast<offset_t>(captureGroupNumber) < captures.size());

		captures[captureGroupNumber] =
			OffsetPair(captures[captureGroupNumber].getHead(), last);
	}

	std::basic_string<char_t> getString(
		group_offset_t captureGroupNumber,
		const std::basic_string<char_t>& sourceStr) const
	{
		if (static_cast<offset_t>(captureGroupNumber) >= captures.size())
			throw std::out_of_range("not found capture group.");

		return captures[captureGroupNumber].getString(sourceStr);
	}

};

template <typename CharType>
class RegexToken
{
public:
	typedef CharType char_t;
	typedef RegexToken* pointer_t;
	typedef RegexToken<char_t> token_t;

	static pointer_t getInvalidPointer()
	{
		return pointer_t();
	}

	typedef std::pair<token_t*, token_t*> token_pair_t;

private:
	pointer_t next;

protected:
	std::list<pointer_t> epsilons;

public:
	RegexToken():
		next(),	epsilons()
	{}

	RegexToken(const RegexToken& token):
		next(token.next), epsilons(token.epsilons)
	{}

	RegexToken(pointer_t next_):
		next(next_), epsilons()
	{}

	virtual ~RegexToken() {}

	virtual pointer_t transit(char_t /*character*/) const
	{
		return getInvalidPointer();
	}

	virtual std::list<pointer_t> epsilonTransit() const
	{
		return epsilons;
	}

	virtual void eval(RegexScanner<CharType>& scanner,
					  RegexResult<CharType>& result)
	{}

	virtual void setNext(pointer_t newNext)
	{
		next = newNext;
	}

	virtual pointer_t getNext() const
	{
		return next;
	}

	void addEpsilon(pointer_t newEpsilon)
	{
		epsilons.push_back(newEpsilon);
	}

	void removeEpsilon(pointer_t removeValue)
	{
		epsilons.remove(removeValue);
	}

	virtual void traverse(std::set<token_t*>& alreadyList)
	{
		if (alreadyList.find(this) != alreadyList.end())
			return;
		alreadyList.insert(this);

		token_t* next_ = getNext();
		if (next_ != getInvalidPointer())
			next_->traverse(alreadyList);

		std::list<pointer_t> epsilons = epsilonTransit();
		for (typename std::list<pointer_t>::iterator itor = epsilons.begin();
			 itor != epsilons.end();
			 ++itor)
			(*itor)->traverse(alreadyList);
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;

	virtual bool isAccept(scanner_t& scanner, result_t& result)
	{
		assert(!"not implement.");
	}
};

template <typename CharType>
class EpsilonToken : public RegexToken<CharType>
{
public:
	typedef RegexToken<CharType> base_t;
	typedef base_t* pointer_t;
	typedef CharType char_t;

public:
	EpsilonToken():
		base_t()
	{}

	EpsilonToken(const EpsilonToken& token):
		base_t(token)
	{}

	EpsilonToken(pointer_t next):
		base_t(next)
	{}

	virtual ~EpsilonToken()
	{}

	virtual void setNext(pointer_t newNext)
	{
		if (base_t::getNext() != base_t::getInvalidPointer())
			base_t::removeEpsilon(base_t::getNext());

		base_t::setNext(newNext);
		if (newNext != base_t::getInvalidPointer())
			base_t::addEpsilon(newNext);
	}

	virtual pointer_t transit(char_t) const
	{
		return base_t::getInvalidPointer();
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	virtual bool isAccept(scanner_t& scanner, result_t& result)
	{
		std::list<pointer_t> epsilons = this->epsilonTransit();
		for (typename std::list<pointer_t>::iterator itor = epsilons.begin();
			 itor != epsilons.end();
			 ++itor)
			if ((*itor)->isAccept(scanner, result))
				return true;

		return false;
	}
};

template <typename CharType>
class GroupHeadToken : public EpsilonToken<CharType>
{
public:
	typedef EpsilonToken<CharType> base_t;
	typedef base_t* pointer_t;
	typedef CharType char_t;

private:
	int groupNumber;

public:
	GroupHeadToken(const int number):
		base_t(), groupNumber(number)
	{}

	virtual ~GroupHeadToken()
	{}

	int getGroupNumber() const
	{
		return groupNumber;
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	virtual bool isAccept(scanner_t& scanner, result_t& result)
	{
		result.setCaptureHead(this->getGroupNumber(),
							  scanner.getPosition());

		std::list<pointer_t> epsilons = this->epsilonTransit();
		assert(epsilons.size() == 1);

		return epsilons.front()->isAccept(scanner, result);
	}

};

template <typename CharType>
class GroupTailToken : public EpsilonToken<CharType>
{
public:
	typedef EpsilonToken<CharType> base_t;
	typedef base_t* pointer_t;
	typedef CharType char_t;

private:
	int groupNumber;

public:
	GroupTailToken(const int number):
		base_t(), groupNumber(number)
	{}

	virtual ~GroupTailToken()
	{}

	int getGroupNumber() const
	{
		return groupNumber;
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	virtual bool isAccept(scanner_t& scanner, result_t& result)
	{
		// set result tail position.
		assert(false);
	}
};

template <typename CharType>
class RegexHead : public GroupHeadToken<CharType>
{
public:
	typedef GroupHeadToken<CharType> base_t;
	typedef base_t* pointer_t;
	typedef CharType char_t;
	
	RegexHead():
		base_t(0)
	{}

	virtual ~RegexHead()
	{}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	virtual bool isAccept(scanner_t& scanner, result_t& result)
	{
		// set result head position. already result number of zero.
		// sub token is accept failed then repeat scanner read token,
		// repeat call sub token::isAccept().
		assert(false);
	}

};

template <typename CharType>
class RegexTail : public GroupTailToken<CharType>
{
public:
	typedef GroupTailToken<CharType> base_t;
	typedef base_t* pointer_t;
	typedef CharType char_t;

	RegexTail():
		base_t(0)
	{}

	virtual ~RegexTail()
	{}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	virtual bool isAccept(scanner_t& scanner, result_t& result)
	{
		// set result tail position. already result number of zero.
		assert(false);
	}

};

template <typename CharType>
class CharacterToken : public RegexToken<CharType>
{
public:
	typedef RegexToken<CharType> base_t;
	typedef base_t* pointer_t;
	typedef CharType char_t;

private:
	const char_t acceptCharacter;
	
public:
	CharacterToken(char_t character):
		base_t(), acceptCharacter(character)
	{}

	CharacterToken(const CharacterToken& token):
		base_t(token), acceptCharacter(token.acceptCharacter)
	{}

	CharacterToken(char_t character, pointer_t next):
		base_t(next), acceptCharacter(character)
	{}

	virtual ~CharacterToken()
	{}

	virtual pointer_t transit(char_t input) const
	{
		if (acceptCharacter == input)
			return base_t::getNext();

		return base_t::getInvalidPointer();
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	virtual bool isAccept(scanner_t& scanner, result_t& result)
	{
		// if accept then scanner step ahead.
		// but not accept scanner rewind and return false.
		assert(false);
	}

};

template <typename CharType>
class RangeToken : public RegexToken<CharType>
{
public:
	typedef RegexToken<CharType> base_t;
	typedef base_t* pointer_t;
	typedef CharType char_t;

private:
	const char_t acceptMin;
	const char_t acceptMax;
	
public:
	RangeToken(char_t min_, char_t max_):
		base_t(), acceptMin(min_), acceptMax(max_)
	{}

	RangeToken(const RangeToken& token):
		base_t(token),
		acceptMin(token.acceptMin),
		acceptMax(token.acceptMax)
	{}

	RangeToken(char_t min_, char_t max_, pointer_t next):
		base_t(next), acceptMin(min_), acceptMax(max_)
	{}

	virtual ~RangeToken()
	{}

	virtual pointer_t transit(char_t input) const
	{
		if (acceptMin <= input &&
			acceptMax >= input)
			return base_t::getNext();

		return base_t::getInvalidPointer();
	}
};

template <typename CharType>
class SetToken : public RegexToken<CharType>
{
public:
	typedef RegexToken<CharType> base_t;
	typedef base_t* pointer_t;
	typedef CharType char_t;

private:
	std::set<char_t> acceptSet;
	
public:
	SetToken():
		base_t(), acceptSet()
	{}

	SetToken(const std::set<char_t>& sets):
		base_t(), acceptSet(sets)
	{}

	SetToken(pointer_t next):
		base_t(next), acceptSet()
	{}

	SetToken(const std::set<char_t>& sets, pointer_t next):
		base_t(next), acceptSet(sets)
	{}

	virtual ~SetToken()
	{}

	void addAccept(const char_t character)
	{
		acceptSet.insert(character);
	}

	virtual pointer_t transit(char_t input) const
	{
		if (std::find(acceptSet.begin(), acceptSet.end(), input) !=
			acceptSet.end())
			return base_t::getNext();

		return base_t::getInvalidPointer();
	}

};

template <typename CharType>
class RegexAutomatonManager
{
	friend class RegexAutomatonManagerTest;

public:
	typedef CharType char_t;
	typedef RegexToken<char_t> token_t;
	typedef token_t* pointer_t;
	typedef std::pair<pointer_t, pointer_t> token_pair_t;

protected:
	typedef EpsilonToken<char_t> epsilon_token_t;
	typedef CharacterToken<char_t> char_token_t;
	typedef RangeToken<char_t> range_token_t;
	typedef SetToken<char_t> set_token_t;

	static token_pair_t createLiteral(const char_t character)
	{
		token_t* result = new char_token_t(character);

		return std::make_pair(result, result);
	}

	static token_pair_t concatinate(token_t* first, token_t* second)
	{
		assert(first != NULL);
		assert(second != NULL);

		first->setNext(second);

		return std::make_pair(first, second);
	}

	static token_pair_t concatinate(token_pair_t first, token_pair_t second)
	{
		concatinate(first.second, second.first);

		return std::make_pair(first.first, second.second);
	}

	static token_pair_t select(token_t* first, token_t* second)
	{
		return select(
			std::make_pair(first, first),
			std::make_pair(second, second));
	}

	static token_pair_t select(token_pair_t first, token_pair_t second)
	{
		assert(first.second->getNext() == token_t::getInvalidPointer());
		assert(second.second->getNext() == token_t::getInvalidPointer());

		token_t* selecter = new epsilon_token_t();
		token_t* terminater = new epsilon_token_t();

		selecter->addEpsilon(first.first);
		selecter->addEpsilon(second.first);

		first.second->setNext(terminater);
		second.second->setNext(terminater);

		return std::make_pair(selecter, terminater);
	}

	static token_pair_t group(token_t* left, token_t* right)
	{
		/*
		 * g -> T -> t -> T::next
		 *
		 * g: grouper, T: token, t: terminater
		 */
		token_t* grouper = new epsilon_token_t();
		token_t* terminater = new epsilon_token_t();

		grouper->setNext(left);
		right->setNext(terminater);

		return std::make_pair(grouper, terminater);
	}

	static token_pair_t group(token_pair_t token)
	{
		return group(token.first, token.second);
	}

	static token_pair_t kleene(token_t* left, token_t* right)
	{
		/*     -b-
		 *   /     \
		 * k -> T -> l -> t -> T::next
		 *   \           /
		 *      -   -  -
		 * k: kleene, T: token, l: loopPoint, b: back loop, t: terminater
		 */
		token_t* kleeneClosure = new epsilon_token_t();
		token_t* loopPoint = new epsilon_token_t();
		token_t* terminater = new epsilon_token_t();

		kleeneClosure->setNext(left);
		kleeneClosure->addEpsilon(terminater);

		right->setNext(loopPoint);

		// set default by loop back.
		loopPoint->addEpsilon(kleeneClosure);
		loopPoint->setNext(terminater);

		return std::make_pair(kleeneClosure, terminater);
	}

	static token_pair_t kleene(token_pair_t token)
	{
		return kleene(token.first, token.second);
	}

	static token_pair_t kleenePlus(token_t* left, token_t* right)
	{
		token_t* kleenePlusClosure = new epsilon_token_t();
		token_t* terminater = new epsilon_token_t();

		/*     -b-
		 *   /     \
		 * k -> T -> t -> T::next
		 *
		 * k: kleenePlus, T: token, t: terminater, b: back loop
		 */
		kleenePlusClosure->setNext(left);

		// set default by loop back.
		terminater->addEpsilon(kleenePlusClosure);
		right->setNext(terminater);

		return std::make_pair(kleenePlusClosure, terminater);
	}

	static token_pair_t kleenePlus(token_pair_t token)
	{
		return kleenePlus(token.first, token.second);
	}

	static token_pair_t kleeneQuestion(token_t* left, token_t* right)
	{
		/*     -f-
		 *   /     \
		 * k -> T -> t -> T::next
		 *
		 * k: kleeneQuestion, T: token, t: terminater, f: forward shortcut
		 */
		token_t* kleeneQuestionClosure = new epsilon_token_t();
		token_t* terminater = new epsilon_token_t();

		kleeneQuestionClosure->setNext(left);
		kleeneQuestionClosure->addEpsilon(terminater);

		right->setNext(terminater);

		return std::make_pair(kleeneQuestionClosure, terminater);
	}

	static token_pair_t kleeneQuestion(token_pair_t token)
	{
		return kleeneQuestion(token.first, token.second);
	}

public:
	RegexAutomatonManager()
	{}

	~RegexAutomatonManager()
	{}
};

template <typename CharType>
class RegexCompiler : public RegexAutomatonManager<CharType>
{
	friend class RegexCompilerTest;

private:
	typedef CharType char_t;
	typedef RegexAutomatonManager<CharType> base_t;
	typedef base_t* pointer_t;

	typedef RegexToken<char_t> token_t;
	typedef EpsilonToken<char_t> epsilon_token_t;
	typedef CharacterToken<char_t> char_token_t;
	typedef RangeToken<char_t> range_token_t;
	typedef SetToken<char_t> set_token_t;

public:
	typedef std::pair<token_t*, token_t*> token_pair_t;

private:
	bool isKleeneCharacter(const char_t character) const
	{
		switch (character)
		{
		case '*':
			return true;

		case '+':
		case '?':
			assert(false); // not support yet.
		}

		return false;
	}


	typedef RegexScanner<char_t> scanner_t;
	typedef std::char_traits<char_t> char_trait_t;

	token_pair_t compileLiteral(typename char_trait_t::int_type currentRead,
								scanner_t& scanner)
	{
		token_pair_t literal = base_t::createLiteral(currentRead);
		
		if (isKleeneCharacter(char_trait_t::to_char_type(scanner.read())))
		{
			literal = compileStar(literal);
			scanner.scan();
		}

		return literal;
	}

	token_pair_t compileStar(token_pair_t tokenPair)
	{
		return base_t::kleene(tokenPair);
	}

	token_pair_t compilePlus(token_pair_t tokenPair)
	{
		return base_t::kleenePlus(tokenPair);
	}

	token_pair_t compileQuestion(token_pair_t tokenPair)
	{
		return base_t::kleeneQuestion(tokenPair);
	}

	token_pair_t selectCompile(scanner_t& scanner, token_pair_t leftToken)
	{
		return base_t::select(leftToken, subCompile(scanner));
	}

	token_pair_t groupCompile(scanner_t& scanner)
	{
		return base_t::group(subCompile(scanner));
	}

	token_pair_t concatinateTokens(std::vector<token_pair_t>& parseStack) const
	{
		assert(parseStack.size() != 0);

		if (parseStack.size() == 1)
		{
			token_pair_t token = parseStack.back();
			parseStack.pop_back();
			return token;
		}

		token_pair_t right = parseStack.back();
		parseStack.pop_back();

		do
		{
			token_pair_t left = parseStack.back();
			parseStack.pop_back();
			right = concatinate(left, right);
		}
		while (parseStack.size() != 0);

		return right;
	}

	token_pair_t subCompile(scanner_t& scanner)
	{
		std::vector<token_pair_t> parseStack;

		typename char_trait_t::int_type currentRead;
		while((currentRead = scanner.scan()) != -1)
		{

			switch (currentRead)
			{
				case '*':
				{
					token_pair_t target = parseStack.back();
					parseStack.pop_back();
					parseStack.push_back(compileStar(target));
					break;
				}

				case '+':
				{
					token_pair_t target = parseStack.back();
					parseStack.pop_back();
					parseStack.push_back(compilePlus(target));
					break;
				}

				case '?':
				{
					token_pair_t target = parseStack.back();
					parseStack.pop_back();
					parseStack.push_back(compileQuestion(target));
					break;
				}

				case '|':
					return selectCompile(
						scanner, concatinateTokens(parseStack));

				case '(':
					++parenCount;
					parseStack.push_back(groupCompile(scanner));
					break;

				case ')':
					--parenCount;
					return concatinateTokens(parseStack);

// 				case '[':
// 					setCompile(scanner);
// 					break;

				default:
					parseStack.push_back(
						compileLiteral(currentRead, scanner));
					break;
			}
		}

		return concatinateTokens(parseStack);
	}

public:
	token_pair_t compile(std::string pattern)
	{
		scanner_t scanner(pattern.begin(), pattern.end());

		parenCount = 0;

		token_pair_t pair = subCompile(scanner);

		if (parenCount != 0)
			throw CompileError("not match paren pairs.");

		return pair;
	}

	int parenCount;

	RegexCompiler():
		base_t(), parenCount(0)
	{}

	RegexCompiler(const std::string& pattern):
		base_t(), parenCount(0)
	{}
	
};

#endif /* REGEXCOMPILE_HPP_ */

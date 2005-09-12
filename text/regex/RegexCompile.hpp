#ifndef REGEXCOMPILE_HPP_
#define REGEXCOMPILE_HPP_

#include <string>
#include <stdexcept>
#include <iterator>
#include <sstream>
#include <vector>
#include <list>
#include <utility>

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

protected:
	typedef EpsilonToken<char_t> epsilon_token_t;
	typedef CharacterToken<char_t> char_token_t;
	typedef RangeToken<char_t> range_token_t;
	typedef SetToken<char_t> set_token_t;
	typedef std::pair<token_t*, token_t*> token_pair_t;

	std::vector<token_t*> automatonList;

	token_pair_t createLiteral(const char_t character)
	{
		token_t* result = new char_token_t(character);
		automatonList.push_back(result);

		return std::make_pair(result, result);
	}

	token_t* const head;
	token_t* const last;

	bool isStart(token_t* token) const
	{
		return head == token;
	}

	bool isEnd(token_t* token) const
	{
		return last == token;
	}


	static token_pair_t concatinate(token_t* first, token_t* second)
	{
		assert(first != NULL);
		assert(second != NULL);

		first->setNext(second);

		return std::make_pair(first, second);
	}

	static token_pair_t select(token_t* first, token_t* second)
	{
		assert(first->getNext() == token_t::getInvalidPointer());
		assert(second->getNext() == token_t::getInvalidPointer());

		token_t* selecter = new epsilon_token_t();
		token_t* terminater = new epsilon_token_t();

		selecter->addEpsilon(first);
		selecter->addEpsilon(second);

		first->setNext(terminater);
		second->setNext(terminater);

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

	static token_pair_t kleene(token_t* left, token_t* right)
	{
		token_t* kleeneClosure = new epsilon_token_t();
		token_t* terminater = new epsilon_token_t();

		/*     -b-
		 *   /     \
		 * k -> T -> t -> T::next
		 *
		 * k: kleene, T: token, t: terminater, b: back loop
		 */
		kleeneClosure->setNext(left);

		// set default by loop back.
		terminater->addEpsilon(kleeneClosure);
		right->setNext(terminater);

		return std::make_pair(kleeneClosure, terminater);
	}

public:
	RegexAutomatonManager():
		automatonList(),
		head(new token_t()), last(new token_t())
	{
		automatonList.push_back(head);
		automatonList.push_back(last);
	}

	~RegexAutomatonManager()
	{
		delete head;
		delete last;
	}
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
	typedef std::pair<token_t*, token_t*> token_pair_t;

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
								scanner_t& scanner,
								token_t* previous = NULL)
	{
		token_pair_t literal = base_t::createLiteral(currentRead);
		
		if (isKleeneCharacter(char_trait_t::to_char_type(scanner.read())))
		{
			literal = compileStar(literal);
			scanner.scan();
		}

		if (previous != NULL)
		{
			base_t::concatinate(previous, literal.first);
			return std::make_pair(previous, literal.second);
		}

		return literal;
	}

	token_pair_t compileStar(token_pair_t tokenPair)
	{
		return base_t::kleene(tokenPair.first,
							  tokenPair.second);
	}

	token_pair_t subCompile(scanner_t& scanner)
	{
		token_pair_t lastToken =
			std::make_pair(static_cast<token_t*>(NULL),
						   static_cast<token_t*>(NULL));

		typename char_trait_t::int_type currentRead;
		while((currentRead = scanner.scan()) != -1)
		{

			switch (currentRead)
			{
			case '*':
				lastToken =
					compileStar(lastToken);
				break;

//			case '+':

// 			case '|':
// 				selectCompile(scanner);
// 				break;

// 			case '(':
// 				groupCompile(scanner);
// 				break;

// 			case '[':
// 				setCompile(scanner);
// 				break;

// 			case ')':
// 				break;

			default:
				lastToken = compileLiteral(currentRead,
										   scanner,
										   lastToken.second);
			
			}
		}

		return lastToken;
	}

	token_pair_t compile(std::string pattern)
	{
		scanner_t scanner(pattern.begin(), pattern.end());

		return subCompile(scanner);
	}

public:
	RegexCompiler():
		base_t()
	{}

	RegexCompiler(const std::string& pattern):
		base_t()
	{}
	
};

#endif /* REGEXCOMPILE_HPP_ */

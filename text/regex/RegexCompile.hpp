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

template <typename CharType, typename PointerT>
class RegexToken
{
public:
	typedef CharType char_t;
	typedef PointerT pointer_t;

	static pointer_t getInvalidPointer()
	{
		return pointer_t();
	}

private:
	pointer_t next;

protected:
	std::list<pointer_t> epsilons;

public:
	RegexToken():
//		before(),
		next(),	epsilons()
	{}

	RegexToken(const RegexToken& token):
//		before(token.before),
		next(token.next), epsilons(token.epsilons)
	{}

	RegexToken(/*pointer_t before_,*/ pointer_t next_):
//		before(before_),
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
};

template <typename CharType, typename PointerT>
class EpsilonToken : public RegexToken<CharType, PointerT>
{
public:
	typedef RegexToken<CharType, PointerT> base_t;
	typedef PointerT pointer_t;
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
		assert(newNext != pointer_t());

		if (base_t::getNext() != pointer_t())
			base_t::removeEpsilon(base_t::getNext());

		base_t::setNext(newNext);
		base_t::addEpsilon(newNext);
	}

	virtual pointer_t getNext() const
	{
		if (base_t::getNext() == pointer_t())
			throw std::out_of_range("Yet not set for next pointer.");

		return base_t::getNext();
	}

	virtual pointer_t transit(char_t) const
	{
		return base_t::getInvalidPointer();
	}
};

template <typename CharType, typename PointerT>
class CharacterToken : public RegexToken<CharType, PointerT>
{
public:
	typedef RegexToken<CharType, PointerT> base_t;
	typedef PointerT pointer_t;
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

	CharacterToken(char_t character,/* pointer_t before,*/ pointer_t next):
		base_t(/*before,*/ next), acceptCharacter(character)
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

template <typename CharType, typename PointerT>
class RangeToken : public RegexToken<CharType, PointerT>
{
public:
	typedef RegexToken<CharType, PointerT> base_t;
	typedef PointerT pointer_t;
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

	RangeToken(char_t min_, char_t max_,/* pointer_t before,*/ pointer_t next):
		base_t(/*before,*/ next), acceptMin(min_), acceptMax(max_)
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

template <typename CharType, typename PointerT>
class SetToken : public RegexToken<CharType, PointerT>
{
public:
	typedef RegexToken<CharType, PointerT> base_t;
	typedef PointerT pointer_t;
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

	SetToken(/*pointer_t before,*/ pointer_t next):
		base_t(/*before,*/ next), acceptSet()
	{}

	SetToken(const std::set<char_t>& sets,/* pointer_t before,*/ pointer_t next):
		base_t(/*before,*/ next), acceptSet(sets)
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

template <typename CharType, typename PointerT>
class RegexAutomatonManager
{
	friend class RegexAutomatonManagerTest;

public:
	typedef CharType char_t;
	typedef PointerT pointer_t;

	typedef RegexToken<char_t, pointer_t> token_t;

private:
	typedef EpsilonToken<char_t, pointer_t> epsilon_token_t;
	typedef CharacterToken<char_t, pointer_t> char_token_t;
	typedef RangeToken<char_t, pointer_t> range_token_t;
	typedef SetToken<char_t, pointer_t> set_token_t;
	typedef std::pair<pointer_t, pointer_t> token_pair_t;

	std::vector<token_t*> automatonList;

	pointer_t lookupPointer(token_t* token) const
	{
		assert(token != NULL);
		
		typename std::vector<token_t*>::const_iterator findPos =
			std::find(automatonList.begin(),
					  automatonList.end(),
					  token);

		assert(findPos != automatonList.end());

		return static_cast<pointer_t>(
			std::distance(automatonList.begin(), findPos));
	}

	token_t* lookupRealPointer(pointer_t pointer) const
	{
		assert(pointer != pointer_t());

		return automatonList[pointer];
	}

	void release()
	{
		for (typename std::vector<token_t*>::iterator itor =
				 automatonList.begin();
			 itor != automatonList.end();
			 ++itor)
		{
			delete *itor;
		}
	}

	token_pair_t concatinate(token_t* first, token_t* second)
	{
		assert(first != NULL);
		assert(second != NULL);

		first->setNext(lookupPointer(second));

		return std::make_pair(lookupPointer(first),
							  lookupPointer(second));
	}

	token_pair_t select(token_t* first, token_t* second)
	{
		assert(first->getNext() == token_t::getInvalidPointer());
		assert(second->getNext() == token_t::getInvalidPointer());

		token_t* selecter = new epsilon_token_t();
		token_t* terminater = new epsilon_token_t();
		automatonList.push_back(selecter);
		automatonList.push_back(terminater);

		selecter->addEpsilon(lookupPointer(first));
		selecter->addEpsilon(lookupPointer(second));

		first->setNext(lookupPointer(terminater));
		second->setNext(lookupPointer(terminater));

		return std::make_pair(lookupPointer(selecter),
							  lookupPointer(terminater));
	}

	token_pair_t group(token_t* token)
	{
		/*
		 * g -> T -> t -> T::next
		 *
		 * g: grouper, T: token, t: terminater
		 */
		token_t* grouper = new epsilon_token_t();
		token_t* terminater = new epsilon_token_t();
		automatonList.push_back(grouper);
		automatonList.push_back(terminater);

		grouper->setNext(lookupPointer(token));
		terminater->setNext(token->getNext());
		token->setNext(lookupPointer(terminater));

		return std::make_pair(lookupPointer(grouper),
							  lookupPointer(terminater));
	}

	token_pair_t kleene(token_t* token)
	{
		token_t* kleeneClosure = new epsilon_token_t();
		token_t* terminater = new epsilon_token_t();
		automatonList.push_back(kleeneClosure);
		automatonList.push_back(terminater);

		/*     -b-
		 *   /     \
		 * k -> T -> t -> T::next
		 *
		 * k: kleene, T: token, t: terminater, b: back loop
		 */
		kleeneClosure->setNext(lookupPointer(token));

		// set default by loop back.
		terminater->addEpsilon(lookupPointer(kleeneClosure));
		terminater->setNext(token->getNext());

		token->setNext(lookupPointer(terminater));

		return std::make_pair(lookupPointer(kleeneClosure),
							  lookupPointer(terminater));
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
		release();
	}
};


#endif /* REGEXCOMPILE_HPP_ */

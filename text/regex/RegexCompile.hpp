#ifndef REGEXCOMPILE_HPP_
#define REGEXCOMPILE_HPP_

#include <util/SmartPointer.hpp>
#include <string>
#include <stdexcept>
#include <iterator>
#include <sstream>
#include <vector>
#include <list>
#include <utility>
#include <set>
#include <algorithm>

#include <cassert>

/**
 * コンパイルエラー例外オブジェクト
 */
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

/**
 * 正規表現マッチストリーム入力クラス
 */
template <typename CharType>
class RegexScanner
{
public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> char_trait_t;
	typedef std::basic_string<char_t> string_t;
	typedef typename string_t::const_iterator internal_iterator_t;

private:
	RegexScanner();
	RegexScanner(const RegexScanner&);
	RegexScanner& operator=(const RegexScanner&);

	internal_iterator_t head;
	internal_iterator_t current;
	internal_iterator_t last;
	int overrun;

public:
	RegexScanner(const string_t& str):
		head(str.begin()), current(str.begin()), last(str.end()), overrun(0)
	{}

	RegexScanner(internal_iterator_t head_,
				 internal_iterator_t last_):
		head(head_), current(head_), last(last_), overrun(0)
	{}

	size_t getPosition() const
	{
		return static_cast<size_t>(std::distance(head, current));
	}

	typename char_trait_t::int_type scan()
	{
		if (current < last)
			return char_trait_t::to_int_type(*current++);

		++overrun;
		return static_cast<typename char_trait_t::int_type>(-1);
	}

	typename char_trait_t::int_type read()
	{
		if (current != last)
			return char_trait_t::to_int_type(*current);

		return static_cast<typename char_trait_t::int_type>(-1);
	}

	void advance(const size_t count)
	{
		assert((current + count) <= last);
		current += count;
	}

	typename char_trait_t::int_type readAhead()
	{
		if (current + 1 != last && current != last)
			return char_trait_t::to_int_type(*(current + 1));

		return static_cast<typename char_trait_t::int_type>(-1);
	}

	void backTrack()
	{
		if (current == head)
			throw std::runtime_error("under flow exception.");

		if (overrun > 0)
			--overrun;
		else
			--current;

		assert(overrun >= 0);
	}

	void reset()
	{
		current = head;
	}
};

/**
 * 正規表現マッチ結果クラス
 */
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
		OffsetPair(offset_t head_ = 0)
			: head(head_), last(head_)
		{}

		OffsetPair(offset_t head_, offset_t last_)
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
	RegexResult():
		captures()
	{}

	RegexResult(const RegexResult& rhs):
		captures(rhs.captures)
	{}

	RegexResult& operator=(const RegexResult& rhs)
	{
		captures = rhs.captures;
	}

	~RegexResult()
	{}

	void setCapture(group_offset_t captureGroupNumber,
					const OffsetPair offsets)
	{
		assert(captures.size() >= static_cast<offset_t>(captureGroupNumber));

		if (static_cast<size_t>(captureGroupNumber) >= captures.size())
			captures.resize(captureGroupNumber + 1);

		captures[captureGroupNumber] = offsets;
	}

	OffsetPair getCapture(group_offset_t captureGroupNumber) const
	{
		assert(captures.size() > static_cast<offset_t>(captureGroupNumber));

		return captures[captureGroupNumber];
	}

	void setCaptureHead(group_offset_t captureGroupNumber, const offset_t head)
	{
		if (static_cast<offset_t>(captureGroupNumber) >= captures.size())
			captures.resize(captureGroupNumber + 1);

		captures[captureGroupNumber] = OffsetPair(head);
	}

	void setCaptureTail(group_offset_t captureGroupNumber, const offset_t tail)
	{
		assert(static_cast<offset_t>(captureGroupNumber) < captures.size());

		captures[captureGroupNumber] = 
			OffsetPair(captures[captureGroupNumber].getHead(), tail);
	}

	void remove(group_offset_t captureGroupNumber)
	{
		assert(static_cast<offset_t>(captureGroupNumber) < captures.size());

		captures.erase(captures.begin() + captureGroupNumber, captures.end());
	}

	size_t size() const
	{
		return captures.size();
	}

	void clear()
	{
		captures.clear();
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

/**
 * isAcceptメソッドを軽量化するためのヘルパ構造体
 */
template <typename CharType>
struct AcceptArgument
{
public:
	typedef CharType char_t;
	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;

	scanner_t& scanner;
	result_t& result;
	bool ignoreCase;

	AcceptArgument(scanner_t& scanner_,
				   result_t& result_,
				   const bool ignoreCase_):
		scanner(scanner_),
		result(result_),
		ignoreCase(ignoreCase_)
	{}

	AcceptArgument(const AcceptArgument& rhs):
		scanner(rhs.scanner),
		result(rhs.result),
		ignoreCase(rhs.ignoreCase)
	{}

	~AcceptArgument()
	{}

	AcceptArgument& operator=(const AcceptArgument& rhs)
	{
		this->scanner = rhs.scanner;
		this->result = rhs.result;
		this->ignoreCase = rhs.ignoreCase;

		return *this;
	}
};

/**
 * 有限オートマトン基底クラス
 */
template <typename CharType>
class RegexToken
{
	friend class RegexTokenTest;

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
	std::vector<pointer_t> epsilons;

	char_t lowerCase(char_t character) const
	{
		if (character >= 'A' &&
			character <= 'Z')
			return character + 0x20;
		
		return character;
	}

	char_t upperCase(char_t character) const
	{
		if (character >= 'a' &&
			character <= 'z')
			return character - 0x20;
		
		return character;
	}

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

	pointer_t transit(char_t /*character*/, bool ignoreCase) const
	{
		return getInvalidPointer();
	}

	const std::vector<pointer_t>& epsilonTransit() const
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
		assert(epsilons.size() <= 2);
	}

	void removeEpsilon(pointer_t removeValue)
	{
		typename std::vector<pointer_t>::iterator itor =
			std::find(epsilons.begin(), epsilons.end(), removeValue);

		assert(itor != epsilons.end());

		epsilons.erase(itor);
	}

	virtual void traverse(std::set<token_t*>& alreadyList)
	{
		if (alreadyList.find(this) != alreadyList.end())
			return;
		alreadyList.insert(this);

		token_t* next_ = getNext();
		if (next_ != getInvalidPointer())
			next_->traverse(alreadyList);

		const std::vector<pointer_t>& epsilons = epsilonTransit();
		for (typename std::vector<pointer_t>::const_iterator itor =
				 epsilons.begin(); itor != epsilons.end(); ++itor)
			(*itor)->traverse(alreadyList);
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;

	virtual bool isAccept(AcceptArgument<char_t>& /*argument*/) const
	{
		assert(!"not implement.");
		return false;
	}
};

/**
 * イプシロン遷移クラス
 */
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

	pointer_t transit(char_t, bool ignoreCase) const
	{
		return base_t::getInvalidPointer();
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;

	/**
	 * イプシロン遷移なので受諾/拒否ともにスキャナは操作しない。
	 * リザルトも操作しない
	 */
	virtual bool isAccept(AcceptArgument<char_t>& argument) const
	{
		const std::vector<pointer_t>& epsilons = this->epsilonTransit();
		for (typename std::vector<pointer_t>::const_iterator itor =
				 epsilons.begin(); itor != epsilons.end(); ++itor)
			if ((*itor)->isAccept(argument))
				return true;

		return false;
	}
};

/**
 * グループ用ヘッドトークンクラス
 */
template <typename CharType>
class GroupHeadToken : public EpsilonToken<CharType>
{
public:
	typedef EpsilonToken<CharType> base_t;
	typedef typename base_t::pointer_t pointer_t;
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
	/**
	 * 開き括弧遷移なのでスキャナは操作しない
	 * リザルトは受諾時のみ現在位置の保存を行う
	 */
	virtual bool isAccept(AcceptArgument<char_t>& argument) const
	{
		const size_t currentPosition = argument.scanner.getPosition();

		argument.result.setCaptureHead(this->getGroupNumber(), 0);
		if (this->epsilons.front()->isAccept(argument))
		{
			const size_t currentLast =
				argument.result.getCapture(this->getGroupNumber()).getLast();
			argument.result.
				setCapture(this->getGroupNumber(),
						   typename result_t::OffsetPair(currentPosition,
														 currentLast));
			return true;
		}

		return false;
	}
};

/**
 * グループ用テイルトークンクラス
 */
template <typename CharType>
class GroupTailToken : public EpsilonToken<CharType>
{
public:
	typedef EpsilonToken<CharType> base_t;
	typedef RegexToken<CharType>* pointer_t;
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

	/**
	 * 受諾時、リザルトに終端位置を追加する。
	 * 拒否時にはそのままスルーする。
	 */
	virtual bool isAccept(AcceptArgument<char_t>& argument) const
	{
		const std::vector<pointer_t>& epsilons = this->epsilonTransit();
		assert(epsilons.size() == 1);

		const size_t scanPosition = argument.scanner.getPosition();
		if (epsilons.front()->isAccept(argument))
		{
			argument.result.setCaptureTail(this->getGroupNumber(),
										   scanPosition);
			return true;
		}
		return false;
	}
};

/**
 * 正規表現開始トークンクラス
 */
template <typename CharType>
class RegexHead : public GroupHeadToken<CharType>
{
public:
	typedef GroupHeadToken<CharType> base_t;
	typedef RegexToken<CharType>* pointer_t;
	typedef CharType char_t;
	
	RegexHead():
		base_t(0)
	{}

	virtual ~RegexHead()
	{}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	virtual bool isAccept(AcceptArgument<char_t>& argument) const
	{
		if (argument.scanner.read() == -1)
			return false;

		do
		{
			argument.result.setCaptureHead(this->getGroupNumber(),
										   argument.scanner.getPosition());

			if (this->epsilons.front()->isAccept(argument))
				return true;

		} while (argument.scanner.scan() != -1);

		return false;
	}
};

/**
 * 正規表現終端トークンクラス
 */
template <typename CharType>
class RegexTail : public GroupTailToken<CharType>
{
public:
	typedef GroupTailToken<CharType> base_t;
	typedef RegexToken<CharType>* pointer_t;
	typedef CharType char_t;

	RegexTail():
		base_t(0)
	{}

	virtual ~RegexTail()
	{}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	/**
	 * リザルトに終端位置を追加する。
	 */
	virtual bool isAccept(AcceptArgument<char_t>& argument) const
	{
		argument.result.setCaptureTail(this->getGroupNumber(),
									   argument.scanner.getPosition());
		return true;
	}
};

/**
 * キャラクタトークンクラス
 */
template <typename CharType>
class CharacterToken : public RegexToken<CharType>
{
public:
	typedef RegexToken<CharType> base_t;
	typedef RegexToken<CharType>* pointer_t;
	typedef CharType char_t;
	typedef std::char_traits<char_t> trait_t;

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

	pointer_t transit(char_t input, bool ignoreCase) const
	{
		if (ignoreCase)
		{
			if (acceptCharacter == base_t::lowerCase(input) ||
				acceptCharacter == base_t::upperCase(input))
				return base_t::getNext();
		}
		else if (acceptCharacter == input)
			return base_t::getNext();

		return base_t::getInvalidPointer();
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	/**
	 * scannerから一文字取り出して自己と比較
	 */
	virtual bool isAccept(AcceptArgument<char_t>& argument) const
	{
		const int character = argument.scanner.scan();
		if (character >= 0)
		{
			pointer_t next =this->transit(trait_t::to_char_type(character),
										  argument.ignoreCase);
			if (next != base_t::getInvalidPointer() &&
				next->isAccept(argument))
				return true;
		}
		argument.scanner.backTrack();
		return false;
	}
};

/**
 * 改行以外マッチトークンクラス
 */
template <typename CharType>
class AnyMatchToken : public RegexToken<CharType>
{
public:
	typedef RegexToken<CharType> base_t;
	typedef RegexToken<CharType>* pointer_t;
	typedef CharType char_t;
	typedef std::char_traits<char_t> trait_t;

private:
	
public:
	AnyMatchToken():
		base_t()
	{}

	AnyMatchToken(const AnyMatchToken& token):
		base_t(token)
	{}

	AnyMatchToken(pointer_t next):
		base_t(next)
	{}

	virtual ~AnyMatchToken()
	{}

	pointer_t transit(char_t /*input*/, bool /*ignoreCase*/) const
	{
		return base_t::getNext();
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	/**
	 * scannerから一文字取り出して終端でなければマッチ
	 */
	virtual bool isAccept(AcceptArgument<char_t>& argument) const
	{
		const int character = argument.scanner.scan();
		if (character >= 0 &&
			character != '\r' &&
			character != '\n')
		{
			pointer_t next =this->transit(trait_t::to_char_type(character),
										  argument.ignoreCase);
			if (next->isAccept(argument))
				return true;
		}
		argument.scanner.backTrack();
		return false;
	}
};

/**
 * 範囲トークンクラス
 */
template <typename CharType>
class RangeToken : public RegexToken<CharType>
{
public:
	typedef RegexToken<CharType> base_t;
	typedef RegexToken<CharType>* pointer_t;
	typedef CharType char_t;
	typedef std::char_traits<char_t> trait_t;

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

	pointer_t transit(char_t input, bool ignoreCase) const
	{
		if (acceptMin <= input &&
			acceptMax >= input)
			return base_t::getNext();

		return base_t::getInvalidPointer();
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	virtual bool isAccept(AcceptArgument<char_t>& argument) const
	{
		const int character = argument.scanner.scan();
		if (character >= 0)
		{
			pointer_t next =this->transit(trait_t::to_char_type(character),
										  argument.ignoreCase);
			if (next != base_t::getInvalidPointer() &&
				next->isAccept(argument))
				return true;
		}
		argument.scanner.backTrack();
		return false;
	}
};

/**
 * 複数文字を含むトークンクラス
 */
template <typename CharType>
class SetToken : public RegexToken<CharType>
{
public:
	typedef RegexToken<CharType> base_t;
	typedef RegexToken<CharType>* pointer_t;
	typedef CharType char_t;
	typedef std::char_traits<char_t> trait_t;

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

	pointer_t transit(char_t input, bool ignoreCase) const
	{
		if (ignoreCase)
		{
			if (std::find(acceptSet.begin(),
						  acceptSet.end(),
						  base_t::lowerCase(input)) != acceptSet.end() ||
				std::find(acceptSet.begin(),
						  acceptSet.end(),
						  base_t::upperCase(input)) != acceptSet.end())
				return base_t::getNext();
		}
		else if (std::find(acceptSet.begin(), acceptSet.end(), input) !=
			acceptSet.end())
			return base_t::getNext();

		return base_t::getInvalidPointer();
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	virtual bool isAccept(AcceptArgument<char_t>& argument) const
	{
		const int character = argument.scanner.scan();
		if (character >= 0)
		{
			pointer_t next = this->transit(trait_t::to_char_type(character),
										   argument.ignoreCase);
			if (next != base_t::getInvalidPointer() &&
				next->isAccept(argument))
				return true;
		}

		argument.scanner.backTrack();
		return false;
	}
};

/**
 * 複数文字を含まないトークンクラス
 */
template <typename CharType>
class NotSetToken : public RegexToken<CharType>
{
public:
	typedef RegexToken<CharType> base_t;
	typedef RegexToken<CharType>* pointer_t;
	typedef CharType char_t;
	typedef std::char_traits<char_t> trait_t;

private:
	std::set<char_t> rejectSet;
	
public:
	NotSetToken():
		base_t(), rejectSet()
	{}

	NotSetToken(const std::set<char_t>& sets):
		base_t(), rejectSet(sets)
	{}

	NotSetToken(pointer_t next):
		base_t(next), rejectSet()
	{}

	NotSetToken(const std::set<char_t>& sets, pointer_t next):
		base_t(next), rejectSet(sets)
	{}

	virtual ~NotSetToken()
	{}

	void addReject(const char_t character)
	{
		rejectSet.insert(character);
	}

	pointer_t transit(char_t input, bool ignoreCase) const
	{
		if (ignoreCase)
		{
			if (std::find(rejectSet.begin(),
						  rejectSet.end(),
						  base_t::lowerCase(input)) != rejectSet.end() ||
				std::find(rejectSet.begin(),
						  rejectSet.end(),
						  base_t::upperCase(input)) != rejectSet.end())
				return base_t::getInvalidPointer();
		}
		else if (std::find(rejectSet.begin(), rejectSet.end(), input) !=
			rejectSet.end())
			return base_t::getInvalidPointer();

		return base_t::getNext();
	}

	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;
	virtual bool isAccept(AcceptArgument<char_t>& argument) const
	{
		const int character = argument.scanner.scan();
		if (character >= 0)
		{
			pointer_t next = this->transit(trait_t::to_char_type(character),
										   argument.ignoreCase);
			if (next != base_t::getInvalidPointer() &&
				next->isAccept(argument))
				return true;
		}

		argument.scanner.backTrack();
		return false;
	}
};

/**
 * 正規表現トークン管理クラス
 */
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
	typedef AnyMatchToken<char_t> any_token_t;
	typedef CharacterToken<char_t> char_token_t;
	typedef RangeToken<char_t> range_token_t;
	typedef SetToken<char_t> set_token_t;
	typedef GroupHeadToken<char_t> group_head_t;
	typedef GroupTailToken<char_t> group_tail_t;

	int parenCount;

	static token_pair_t createAny()
	{
		token_t* result = new any_token_t();

		return std::make_pair(result, result);
	}

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

	static token_pair_t group(token_t* left, token_t* right,
							  const int groupNumber)
	{
		/*
		 * g -> T -> t -> T::next
		 *
		 * g: grouper, T: token, t: terminater
		 */
		token_t* grouper = new group_head_t(groupNumber);
		token_t* terminater = new group_tail_t(groupNumber);

		grouper->setNext(left);
		right->setNext(terminater);

		return std::make_pair(grouper, terminater);
	}

	static token_pair_t group(token_pair_t token, const int groupNumber)
	{
		return group(token.first, token.second, groupNumber);
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
	RegexAutomatonManager(): parenCount(0)
	{}

	~RegexAutomatonManager()
	{}
};


/**
 * RegexMatch内のheadを削除するためのファンクタ
 */
template <typename CharType>
class TokenRemover
{
public:
	/**
	 * 削除ハンドラ
	 * @param pointer 削除オブジェクトのポインタ
	 */
	static void remove(RegexToken<CharType>* pointer)
	{
		typedef std::set<RegexToken<CharType>*> objects_t;

		objects_t automatons;
		pointer->traverse(automatons);

		for (typename objects_t::iterator itor = automatons.begin();
			 itor != automatons.end(); ++itor)
			delete *itor;

		automatons.clear();
	}
};

/**
 * 正規表現マッチクラス
 * @todo matchしたらマッチ個所のオブジェクトセットを返して
 * matchedStringとかmatchedReplaceとかは別にヘルパ用意した設計の方が自然かも
 */
template <typename CharType>
class RegexMatch
{
	friend class RegexMatchTest;

public:
	typedef CharType char_t;
	typedef RegexResult<char_t> result_t;
	typedef RegexScanner<char_t> scanner_t;
	typedef std::basic_string<char_t> string_t;
	typedef SmartPointer<RegexToken<char_t>, TokenRemover<char_t> > smart_t;

private:
	result_t result;
	smart_t head;
	bool isMatched;

public:
		
	RegexMatch(RegexToken<char_t>* first):
		result(), head(first), isMatched(false)
	{}

	RegexMatch(const RegexMatch& rhs):
		result(rhs.result), head(rhs.head), isMatched(rhs.isMatched)
	{}

	RegexMatch& operator=(const RegexMatch& rhs)
	{
		result = rhs.result;
		head = rhs.head;
		isMatched = rhs.isMatched;

		return *this;
	}

	~RegexMatch()
	{}

	bool match(const string_t& source, bool ignoreCase = false)
	{
		scanner_t scanner(source.begin(), source.end());
		result.clear();

		AcceptArgument<char_t> arguments(scanner, result, ignoreCase);
		isMatched = head->isAccept(arguments);
		return isMatched;
	}

	bool nextMatch(const string_t& source, bool ignoreCase = false)
	{
		assert(isMatched == true);

		const size_t offset = result.getCapture(0).getLast();
		scanner_t scanner(source.begin(), source.end());
		scanner.advance(offset);

		result.clear();

		AcceptArgument<char_t> arguments(scanner, result, ignoreCase);
		isMatched = head->isAccept(arguments);

		return isMatched;
		
	}

	string_t matchedString(const string_t& source,
						   const int groupNumber = 0) const
	{
		assert(isMatched == true);
		assert(static_cast<size_t>(groupNumber) < result.size());

		return result.getString(groupNumber, source);
	}

	string_t matchedReplace(const string_t& source,
							const string_t& replaceString,
							const int groupNumber = 0) const
	{
		assert(isMatched == true);
		assert(static_cast<size_t>(groupNumber) < result.size());

		typename result_t::OffsetPair offsetPair =
			result.getCapture(groupNumber);

		return source.substr(0, offsetPair.getHead()) +
			replaceString + 
			source.substr(offsetPair.getLast(), string_t::npos);
	}

	void reset()
	{
		isMatched = false;
		result.clear();
	}

	size_t size() const
	{
		return result.size();
	}

	std::pair<size_t, size_t> getCapture(const int groupNumber) const
	{
		assert(groupNumber < this->size());

		typename result_t::OffsetPair pair = result.getCapture(groupNumber);
		
		return std::make_pair(pair.getHead(), pair.getLast());
	}
};

/**
 * 正規表現コンパイラ
 * 正規表現から有限オートマトンへのコンパイラ
 */
template <typename CharType>
class RegexCompiler : public RegexAutomatonManager<CharType>
{
	friend class RegexCompilerTest;

private:
	typedef CharType char_t;
	typedef std::basic_string<char_t> string_t;
	typedef RegexAutomatonManager<CharType> base_t;
	typedef base_t* pointer_t;

	typedef RegexToken<char_t> token_t;
	typedef EpsilonToken<char_t> epsilon_token_t;
	typedef CharacterToken<char_t> char_token_t;
	typedef RangeToken<char_t> range_token_t;
	typedef SetToken<char_t> set_token_t;
	typedef NotSetToken<char_t> notset_token_t;

	int maxParenCount;

public:
	typedef std::pair<token_t*, token_t*> token_pair_t;

private:
	bool isKleeneCharacter(const char_t character) const
	{
		switch (character)
		{
		case '*':
		case '+':
		case '?':
			return true;

		}

		return false;
	}


	typedef RegexScanner<char_t> scanner_t;
	typedef std::char_traits<char_t> char_trait_t;

	token_pair_t compileAny()
	{
		return base_t::createAny();
	}

	token_pair_t compileLiteral(typename char_trait_t::int_type currentRead,
								scanner_t& scanner)
	{
		token_pair_t literal =
			base_t::createLiteral(char_trait_t::to_char_type(currentRead));
		
		if (isKleeneCharacter(char_trait_t::to_char_type(scanner.read())))
		{
			literal = compileStar(literal);
			scanner.scan();
		}

		return literal;
	}

	std::set<char_t> compileGroups(typename string_t::const_iterator itor,
								   typename string_t::const_iterator end)
	{
		std::set<char_t> groups;
		typename string_t::const_iterator first = itor;

		while (itor != end)
		{
			if (*itor == '-' &&
				itor != first &&
				(itor + 1) != end)
			{
				char current = *(itor - 1);
				char last = *(++itor);
				if (current >= last)
					throw CompileError("range first greater than second.");

				while (current != last)
					groups.insert(current++);
			}
			else
			{
				groups.insert(*itor++);
			}
		}

		return groups;
	}

	token_pair_t compileSet(scanner_t& scanner)
	{
		typedef std::char_traits<char_t> trait_t;

		string_t setString;

		int temp = scanner.scan();
		if (temp == -1)
			throw CompileError("unmatched [] bracket.");

		setString += trait_t::to_char_type(temp);

		while ((temp = scanner.scan()) >= 0)
		{
			if (trait_t::to_char_type(temp) == ']')
				break;

			setString += trait_t::to_char_type(temp);
		}

		if (trait_t::to_char_type(temp) != ']')
			throw CompileError("unmatched [] bracket.");

		const bool isReject = setString[0] == '^';
		typename string_t::const_iterator itor = 
			isReject ? setString.begin() + 1 : setString.begin();

		std::set<char_t> groups = compileGroups(itor, setString.end());
		
		token_t* token = isReject ?
			static_cast<token_t*>(new notset_token_t(groups)) :
			static_cast<token_t*>(new set_token_t(groups));

		return std::make_pair(token, token);
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

	token_pair_t groupCompile(scanner_t& scanner, const int groupNumber)
	{
		return base_t::group(subCompile(scanner), groupNumber);
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

	/**
	 * @todo バックスラッシュによるエスケープ
	 */
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
					return selectCompile(scanner,
										 concatinateTokens(parseStack));

				case '(':
					++base_t::parenCount;
					++maxParenCount;
					assert(base_t::parenCount > 0);
					assert(maxParenCount > 0);
					parseStack.push_back(groupCompile(scanner,
													  maxParenCount));
					break;

				case ')':
					if (base_t::parenCount <= 0)
						throw CompileError("not match paren count.");
					--base_t::parenCount;
					return concatinateTokens(parseStack);

				case '[':
					parseStack.push_back(compileSet(scanner));
					break;

				case '.':
					parseStack.push_back(compileAny());
					break;

				case '\\':
					if (scanner.read() == -1)
						throw CompileError("escape character not found.");
					currentRead = scanner.scan();
					if (currentRead == 'n')
						currentRead = '\n';
					else if (currentRead == 't')
						currentRead = '	'; // as \t(tab)
					else if (currentRead == 'r')
						currentRead = '\r';
						
				default:
					parseStack.push_back(compileLiteral(currentRead, scanner));
					break;
			}
		}

		return concatinateTokens(parseStack);
	}

	token_pair_t compileInternal(std::string pattern)
	{
		if (pattern.begin() == pattern.end())
		{
			RegexHead<char_t>* head = new RegexHead<char_t>();
			RegexTail<char_t>* tail = new RegexTail<char_t>();

			concatinate(head, tail);
			return std::make_pair(head, tail);
		}
			
		scanner_t scanner(pattern.begin(), pattern.end());

		base_t::parenCount = 0;
		maxParenCount = 0;

		token_pair_t pair = subCompile(scanner);

		if (base_t::parenCount != 0)
			throw CompileError("not match paren pairs.");

		// attach RegexHead and RegexTail.
		RegexHead<char_t>* head = new RegexHead<char_t>();
		RegexTail<char_t>* tail = new RegexTail<char_t>();

		concatinate(head, pair.first);
		concatinate(pair.second, tail);

		return std::make_pair(head, tail);
	}

public:
	RegexCompiler():
		base_t(), maxParenCount()
	{}

	~RegexCompiler()
	{}

	RegexMatch<char_t> compile(std::string pattern)
	{
		token_pair_t tokenPair = compileInternal(pattern);

		return RegexMatch<char_t>(tokenPair.first);
	}
};

#endif /* REGEXCOMPILE_HPP_ */

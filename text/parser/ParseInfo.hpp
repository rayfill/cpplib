#ifndef PARSE_INFO_HPP_
#define PARSE_INFO_HPP_

#include <string>
#include <text/transcode/Unicode.hpp>

template <typename CharType>
class NodeInfo
{
public:
	typedef CharType char_type;
	typedef std::basic_string<char_type> stirng_type;

	virtual ~NodeInfo()
	{}

	virtual string_type getNodeType() const
	{
		return Transcoder::transcode("NodeInfo");
	}

	virtual string_type toString() const
	{
		return string_type(Transcoder::transcode("(")) +
			getNodeType() + Transcoder::transcode(")");
	}
};

template <typename CharType>
class ConcatenateNode : public NodeInfo<CharType>
{
public:
	typedef CharType char_type;
	typedef NodeInfo<char_type> node_type;
	typedef std::basic_string<char_type> stirng_type;

private:
	node_type* leftNode;
	node_type* rightNode;

public:
	ConcatenateNode(node_type* left, node_type* right):
		leftNode(left), rightNode(right)
	{}

	virtual ~ConcatenateNode()
	{}

	virtual string_type getNodeType() const
	{
		return Transcoder::transcode("ConcatenateNode");
	}

	virtual string_type toString() const
	{
		return string_type(Transcoder::transcode("(")) +
			getNodeType() + Transcoder::transcode(" ") +
			leftNode->toString() + Transcoder::transcode(" ") +
			rightNode->toString() + Transcoder::transcode(")");
	}
};

template <typename CharType>
class TokenNode : public NodeInfo<CharType>
{
public:
	typedef CharType char_type;
	typedef std::basic_string<char_type> string_type;

private:
	string_type token;

public:
	TokenNode(const string_type& str):
		token(str)
	{}

	string_type getToken() const
	{
		return token;
	}

	virtual string_type getNodeType() const
	{
		return Transcoder::transcode("TokenNode");
	}

	virtual string_type toString() const
	{
		return string_type(Transcoder::transcode("(")) +
			getNodeType() + Transcoder::transcode(" ") +
			getToken() + Transcoder::transcode(")");
	}
};



template <typename CharType>
class ParseInfo
{
public:
	typedef CharType char_type;
	typedef NodeInfo<char_type> node_type;

private:
	bool isAccept;
	node_type* root;

public:
	ParseInfo():
		isAccept(false),
		root()
	{}

};

#endif /* PARSE_INFO_HPP_ */

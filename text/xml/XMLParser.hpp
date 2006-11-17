#ifndef XMLPARSER_HPP_
#define XMLPARSER_HPP_

#include <cassert>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <text/xml/XMLException.hpp>
#include <text/transcode/Unicode.hpp>

/**
 * XMLノードクラス
 */
template <typename CharType, typename NodeType>
class XMLNode
{
public:
	friend class XMLNodeTest;
	typedef CharType char_t;
	typedef NodeType node_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;
	typedef std::vector<node_t*> nodes_t;

protected:
	/**
	 * 親ノードへのポインタ
	 */
	node_t* parent;

	/**
	 * 子ノードのコレクション
	 */
	nodes_t children;

public:
	/**
	 * コンストラクタ
	 * @param parent 親クラスへのポインタ
	 */
	XMLNode(node_t* parent = NULL):
		parent(parent),
		children()
	{}

	/**
	 * デストラクタ
	 */
	virtual ~XMLNode() throw()
	{
		for (typename nodes_t::iterator itor = children.begin();
			 itor != children.end();
			 ++itor)
			delete *itor;
	}

	/**
	 * 次の兄弟レベルのノードの探索
	 * @return 見つかった次の兄弟ノードのポインタ。次が無い場合はNULLがかえる
	 */
	node_t* getNextSibling() const throw(std::logic_error)
	{
		if (parent == NULL)
			return NULL;

		typename nodes_t::iterator itor =
			std::find(parent->children.begin(),
					  parent->children.end(),
					  this);
		
		if (itor == parent->children.end())
			throw std::logic_error("bad index exception.");

		if (itor+1 == parent->children.end())
			return NULL;

		return *++itor;
	}

	/**
	 * 前の兄弟レベルのノードの探索
	 * @return 前の兄弟レベルノード
	 */
	node_t* getPrevSibling() const throw(std::logic_error)
	{
		if (parent == NULL)
			return NULL;

		typename nodes_t::iterator itor =
			std::find(parent->children.begin(),
					  parent->children.end(),
					  this);

		if (itor == children.end())
			throw std::logic_error("bad index exception.");

		if (itor+1 == parent->children.begin())
			return NULL;

		return *--itor;
	}

	/**
	 * 子ノードの追加
	 * @param newChild 新しい子供ノード
	 */
	void addChild(node_t* newChild)
	{
		newChild->parent = dynamic_cast<node_t*>(this);
		children.push_back(newChild);
	}

	/**
	 * 子ノードの削除
	 * @param child 削除する子供ノード
	 * ノードが見つからない場合、何もしません
	 */
	void removeChild(node_t* child)
	{
		typename nodes_t::iterator itor =
			std::find(children.begin(), children.end(), child);

		if (itor != children.end())
		{
			(*itor)->parent = NULL;
			children.erase(itor);
		}
	}

	/**
	 * 親ノードへの接合
	 * @param newParent 親ノード
	 * @todo スコープprivateにして親からの接合しかできないように
	 * した方がいいかも。
	 */
	void setParent(node_t* newParent) throw()
	{
		this->parent = newParent;
	}

	/**
	 * 親ノードの取得
	 * @return 親ノード
	 */
	node_t* getParent() const throw()
	{
		return this->parent;
	}

	/**
	 * 子ノードの取得
	 * @param index 0ベースインデックスとした子供ノードの位置。
	 * インデックスが無効な場合NULLを返す。
	 */
	node_t* getChild(const size_t index) const throw()
	{
		if (index < children.size())
			return children[index];

		return NULL;
	}

	/**
	 * 子ノード達の取得
	 * @return すべての子ノード
	 */
	std::vector<node_t*> getChildren() const throw()
	{
		return children;
	}

	/**
	 * 文字列変換
	 * @param indentLevel プリティプリント用インデントレベル
	 * @return 要素の文字列表現
	 */
	virtual string_t toString(const size_t indentLevel = 0) const throw() = 0;
};

/**
 * ノード名からDOMノードを見つけるためのファンクタ
 * @param BaseType 基底ノード型
 * @param DeriverdType 見つけるノード型
 * @todo これをコマンドパターンに変更してXPathのサポート述語の拡大を・・・
 */
template <typename BaseType, typename DeriverdType, typename CharType>
class FindNamePredicate
{
public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;

private:
	/**
	 * 見つけるノード名
	 */
	string_t findName;

public:
	/**
	 * コンストラクタ
	 * @param name 見つけるノード名
	 */
	FindNamePredicate(const string_t& name)
		: findName(name)
	{}

	/**
	 * デストラクタ
	 */
	~FindNamePredicate() throw()
	{}

	/**
	 * エントリポイント
	 * @param child ノードのポインタ
	 * @return 名前が同じならtrue 違うならfalse
	 */
	bool operator()(BaseType* child) const throw()
	{
		DeriverdType* tag = dynamic_cast<DeriverdType*>(child);
		if (tag != NULL &&
			tag->getTagName() == this->findName)
			return true;

		return false;
	}
};

template <typename CharType> class TagElement;

/**
 * XML要素クラス
 */
template <typename CharType>
class Element : public XMLNode<CharType, Element<CharType> >
{
private:
	Element(const Element& );
	Element& operator=(const Element&);

public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;
	typedef XMLNode<char_t, Element<char_t> > base_t;
	typedef typename base_t::nodes_t node_t;
	typedef TagElement<char_t> tag_element_t;

	/**
	 * コンストラクタ
	 */
	Element() throw()
	{}

	/**
	 * デストラクタ
	 */
	virtual ~Element() throw()
	{}

	/**
	 * 名前で指定した子要素の取得
	 * @param name タグの名前
	 * @return 見つかったXML要素のポインタ。見つからない場合はNULL
	 */
	virtual Element* getChildElement(const string_t& name)
	{
		typename node_t::iterator itor =
			std::find_if(base_t::children.begin(),
						 base_t::children.end(),
						 FindNamePredicate<base_t, tag_element_t, char_t>(name));

		if (itor != base_t::children.end())
			return dynamic_cast<Element*>(*itor);

		return NULL;
	}

	/**
	 * 名前で指定した子要素の取得
	 * @param name タグの名前
	 * @return 見つかったXML要素のコレクション。
	 * 見つからない場合は空のコレクション。
	 */
	virtual std::vector<Element*> getChildrenElements(const string_t& name)
	{
		FindNamePredicate<base_t, tag_element_t, char_t> predicate(name);
		std::vector<Element*> result;

		for (typename node_t::iterator itor = base_t::children.begin();
			 itor != base_t::children.end(); ++itor)
		{
			if (predicate(*itor))
				result.push_back(dynamic_cast<Element*>(*itor));
		}

		return result;
	}

};

/**
 * XMLタグ要素
 */
template <typename CharType>
class TagElement : public Element<CharType>
{
	friend class XMLParserTest;

public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;
	typedef XMLNode<char_t, Element<char_t> > node_t;
	typedef Element<char_t> element_t;


protected:
	/**
	 * タグの名前
	 */
	string_t tagName;

	/**
	 * タグの属性
	 */
	std::map<string_t, string_t> attributes;

	TagElement():
			tagName(),
			attributes()
	{}
public:
	/**
	 * コンストラクタ
	 * @param tagName_ タグの名前
	 * @param attrubutes_ タグの属性
	 */
	TagElement(const string_t& tagName_,
			   const std::map<string_t, string_t>& attributes_)
		:tagName(tagName_),
		 attributes(attributes_)
	{}

	/**
	 * コンストラクタ
	 * @param token タグ名を含んだパース結果文字列
	 */
	TagElement(const string_t& token) throw(InvalidTagException)
		:tagName(token.substr(0, token.find(' '))),
		 attributes()
	{
		// skip white space.
		typename string_t::size_type position =
			token.find_first_not_of(' ', tagName.length());

		string_t attributesName;
		string_t attributesValue;

		while (position < token.length())
		{
			// search attribute '=' token.
			const typename string_t::size_type
				equalPos = token.find('=', position + 1);

			// not found to quit.
			if (equalPos == string_t::npos)
			{
				if (token.substr(position + 1).find_first_not_of(' ') !=
					string_t::npos)
					throw InvalidTagException();
				break;
			}

			// found to get attributes name.
			attributesName =
				token.substr(position, equalPos - position);

			// equal token's next token is double quote.
			if (token[equalPos + 1] != '"')
				throw InvalidTagException();

			// search closed quote pos.
			const typename string_t::size_type
				quotePos = token.find('"', equalPos + 2);

			// not found to invalid tag.
			if (quotePos == string_t::npos)
				throw InvalidTagException();

			// found to get attributes value.
			attributesValue =
				token.substr(equalPos + 2, quotePos - (equalPos + 2));

			// set attributes pair.
			attributes[attributesName] = attributesValue;

			// set next iterate.
			position = token.find_first_not_of(' ', quotePos + 1);
		}
	}

	/**
	 * デストラクタ
	 */
	virtual ~TagElement() throw()
	{}

	virtual string_t toString(const size_t indentLevel = 0) const throw()
	{
		string_t result(indentLevel, ' ');
		result += '<';
		result += tagName;

		for (typename std::map<string_t, string_t>::const_iterator
				 itor = attributes.begin();
			 itor != attributes.end(); ++itor)
		{
			result += ' ';
			result += itor->first;
			result += '=';
			result += '"';
			result += itor->second;
			result += '"';
		}
		
		if (node_t::children.size() == 0)
		{
			result += ' ';
			result += '/';
			result += '>';
			result += '\n';
		}

		else
		{
			result += '>';
			result += '\n';

			size_t index = 0;
			node_t* currentChild = this->getChild(index++);
			while (currentChild != NULL)
			{
				result += 
					dynamic_cast<element_t*>(currentChild)->
					toString(indentLevel+1);
				currentChild = this->getChild(index++);
			}

			result += string_t(indentLevel, ' ');
			result += '<';
			result += '/';
			result += tagName;
			result += '>';
			result += '\n';
		}

		return result;
	}

	/**
	 * タグ名の取得
	 * @return タグ名
	 */
	string_t getTagName() const throw()
	{
		return tagName;
	}

	/**
	 * 属性セットの取得
	 * @return 属性のコレクション
	 */
	std::map<string_t, string_t> getAttributes() const throw()
	{
		return attributes;
	}

	/**
	 * 属性セットの設定
	 * @param newAttributes 新しい属性のコレクション
	 */
	void setAttributes(const std::map<string_t, string_t>& newAttributes)
		throw()
	{
		attributes = newAttributes;
	}

	/**
	 * 属性の取得
	 * @param attributeName 属性の名前
	 * @return 属性の値
	 */
	string_t getAttribute(const string_t& attributeName) const throw()
	{
		typename std::map<string_t, string_t>::const_iterator itor =
			attributes.find(attributeName);

		if (itor != attributes.end())
			return L"";

		return itor->second;
	}

	/**
	 * 属性の設定
	 * @param name 属性の名前
	 * @param value 属性の値
	 */
	void setAttribute(const string_t& name, const string_t& value) throw()
	{
		attributes[name] = value;
	}
};

/**
 * コメントを表す要素
 */
template <typename CharType>
class CommentElement : public TagElement<CharType>
{
	friend class CommentElementTest;

public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;

private:
	/**
	 * コメント
	 */
	string_t value;

public:
	/**
	 * コンストラクタ
	 * @param value_ コメント文字列
	 */
	CommentElement(const string_t& value_):
			TagElement<char_t>(), value(value_)
	{
		TagElement<char_t>::tagName += '#';
		TagElement<char_t>::tagName += 'c';
		TagElement<char_t>::tagName += 'o';
		TagElement<char_t>::tagName += 'm';
		TagElement<char_t>::tagName += 'm';
		TagElement<char_t>::tagName += 'e';
		TagElement<char_t>::tagName += 'n';
		TagElement<char_t>::tagName += 't';
	}

	/**
	 * デストラクタ
	 */
	virtual ~CommentElement() throw()
	{}

	virtual string_t toString(const size_t indentLevel = 0) const throw()
	{
		string_t commentPrefix;
		commentPrefix += '<';
		commentPrefix += '!';
		commentPrefix += '-';
		commentPrefix += '-';
		string_t commentSuffix;
		commentSuffix += '-';
		commentSuffix += '-';
		commentSuffix += '>';
		commentSuffix += '\n';
		return
			string_t(indentLevel, ' ') +
			commentPrefix +
			value +
			commentSuffix;
	}

	/**
	 * コメント文字列の取得
	 * @return コメント文字列
	 */
	string_t getComment() const throw()
	{
		return value;
	}

	/**
	 * コメント文字列の設定
	 * @param comment 新しいコメント文字列
	 */
	void setComment(const string_t& comment) throw()
	{
		value = comment;
	}
};

/**
 * CDATA文字列要素
 * @note 前後の空白文字をchopするヘルパがあってもいいかも
 */
template <typename CharType>
class StringElement : public TagElement<CharType>
{
	friend class StringElementTest;
public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;
	typedef XMLNode<char_t, Element<char_t> > base_t;

private:
	/**
	 * 文字列
	 */
	string_t value;

public:
	/**
	 * コンストラクタ
	 * @param value_ CDATA文字列
	 */
	StringElement(const string_t& value_):
			TagElement<char_t>(),  value(value_)
	{
		TagElement<char_t>::tagName += '#';
		TagElement<char_t>::tagName += 't';
		TagElement<char_t>::tagName += 'e';
		TagElement<char_t>::tagName += 'x';
		TagElement<char_t>::tagName += 't';
	}

	/**
	 * デストラクタ
	 */
	virtual ~StringElement() throw()
	{}
	
	virtual string_t toString(const size_t indentLevel = 0) const throw()
	{
		 string_t result = string_t(indentLevel, ' ');
		 result += value;
		 result += '\n';
		 return result;
	}
	
	/**
	 * CDATA文字列の取得
	 * @return 文字列
	 */
	string_t getString() const throw()
	{
		return value;
	}

	/**
	 * CDATA文字列の設定
	 * @param newString
	 * 新しいCDATA文字列
	 */
	void setString(const string_t& newString) throw()
	{
		value = newString;
	}
		
};

/**
 * METAタグ要素
 */
template <typename CharType>
class MetaElement : public TagElement<CharType>
{
	friend class XMLParserTest;

public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;
	typedef XMLNode<char_t, Element<char_t> > base_t;
	typedef TagElement<char_t> tag_element_t;

	/**
	 * コンストラクタ
	 * param token METAタグ名
	 */
	MetaElement(const string_t& token):
		TagElement<CharType>(token)
	{}

	/**
	 * デストラクタ
	 */
	virtual ~MetaElement() throw()
	{}

	virtual string_t toString(const size_t indentLevel = 0) const throw()
	{
		assert(base_t::children.size() == 0);

		string_t result(indentLevel, ' ');
		result += '<';
		result+= '?';
		result += tag_element_t::tagName;

		for (typename std::map<string_t, string_t>::const_iterator
				 itor = tag_element_t::attributes.begin();
			 itor != tag_element_t::attributes.end(); ++itor)
		{
			result += ' ';
			result += itor->first;
			result += '=';
			result += '"';
			result += itor->second;
			result += '"';
		}
		
		result += ' ';
		result += '?';
		result += '>';

		return result;
	}
};

/**
 * XMLドキュメントクラス
 */
template <typename CharType>
class XMLDocument : public Element<CharType>
{
public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;
	typedef XMLNode<char_t, Element<char_t> > base_t;
	typedef typename base_t::nodes_t node_t;

	virtual string_t toString(const size_t = 0) const throw()
	{
		string_t result;

		for (typename node_t::const_iterator itor = base_t::children.begin();
			 itor != base_t::children.end(); ++itor)
		{
			result +=
				dynamic_cast<const Element<char_t>*>(*itor)->toString();
			result += '\n';
		}

		return result;
	}

	/**
	 * コンストラクタ
	 */
	XMLDocument() throw()
	{}
	
	/**
	 * デストラクタ
	 */
	virtual ~XMLDocument() throw()
	{}
};

/**
 * XMLPathパーサ
 */
template <typename CharType>
class XMLPath
{
	friend class XMLParserTest;

public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;
	typedef Element<char_t> base_t;
	typedef Element<char_t> element_t;
	typedef TagElement<char_t> tag_element_t;
	typedef typename base_t::nodes_t node_t;

private:
	/**
	 * 検索パス
	 */
	string_t path;

	/**
	 * 文字列から整数値への変換ヘルパ
	 * @param value 文字列
	 * @return 整数値
	 */
	int strToInt(const string_t& value)	const
	{
		int result = 0;
		for (typename string_t::const_iterator itor = value.begin();
			 itor != value.end(); ++itor)
		{
			result *= 10;
			if (*itor >= '0' &&
				*itor <= '9')
			{
				result += static_cast<int>(*itor - '0');
			}
			else
				throw std::invalid_argument("invalid number format.");
		}

		return result;
	}

	/**
	 * インデックスオフセットの計算
	 * @param param XPathトークン
	 * @return インデックスオフセット
	 * @todo ブラケット内がexpressionだった場合の対応。
	 * XPathExpressionとか言うクラスでも作って評価式をオブジェクト化して
	 * Commandパターン使ってトラバースさせるとかかなぁ・・・
	 */
	int getIndexOf(const string_t& param)
		const throw(std::invalid_argument)
	{
		const typename string_t::size_type offset1 =
			param.find('[');

		if (offset1 == string_t::npos)
			return 0;

		const typename string_t::size_type offset2 =
			param.find(']');

		if (offset2 == string_t::npos)
			throw std::invalid_argument("");

		if (offset1 > offset2)
			throw std::invalid_argument("");

		if ((offset2 - offset1) == 1)
			return -1;

		if ((offset2 - offset1) > 1)
		{
			/**
			 * @todo expressionがはいった場合の対応とか。
			 * まぁ、その場合は基本部分の見直しが必要だろうけど・・・
			 */
			return
				strToInt(string_t(&param[offset1+1],
									  &param[offset2]));
		}

		throw std::invalid_argument("");
	}

	/**
	 * DOMツリーのトラバース
	 * @param element DOM要素の探査元
	 * @param nameList XPathからノード要素ごとに分解された文字列リスト
	 * @reutrn 見つかった要素のコレクション
	 */
	std::vector<Element<char_t>*> traverse(Element<char_t>* element,
										   std::vector<string_t> nameList)
	{
		assert(nameList.size() > 0);

		string_t name = nameList.back();
		nameList.pop_back();
		string_t canonicalizeName =
			name.substr(0, name.find('['));
		
		tag_element_t* tag = dynamic_cast<tag_element_t*>(element);
		if (tag == NULL)
			return std::vector<element_t*>();

		if (canonicalizeName != tag->getTagName())
			return std::vector<element_t*>();

		// enumerate children.
		std::vector<element_t*> results;
		if (nameList.size() > 0)
		{
			string_t childName = nameList.back();
			const int offset = getIndexOf(childName);
			if (offset < 0)
			{
				std::vector<base_t*> children = 
					element->getChildren();

				for (typename std::vector<base_t*>::iterator itor =
						 children.begin(); itor != children.end(); ++itor)
				{
					std::vector<element_t*> result =
						traverse(*itor, nameList);

					if (result.size() > 0)
						results.insert(results.end(),
									   result.begin(),
									   result.end());
				}
			}
			else
			{
				string_t canonicalizeChildName =
					childName.substr(0, childName.find('['));

				std::vector<element_t*> children =
					tag->getChildrenElements(canonicalizeChildName);

				element_t* elem = NULL;
				if (children.size() > static_cast<unsigned int>(offset))
					elem = dynamic_cast<element_t*>(children[offset]);

				std::vector<element_t*> result;
				if (elem != NULL)
					result = traverse(elem, nameList);

				if (result.size() > 0)
					results.insert(results.end(),
								   result.begin(),
								   result.end());
			}

			return results;
		}

		results.push_back(element);
		return results;
	}

	/**
	 * XPathのトークン分割
	 * @return 分割されたXPathトークン
	 */
	std::vector<string_t> tokenize() const
	{
		std::vector<string_t> result;
		string_t path(this->path);
		path.erase(path.begin());

		typename string_t::size_type newPosition;
		for(;;)
		{
			newPosition = path.find_first_of('/');
			result.push_back(string_t(path, 0, newPosition));
			if (newPosition == string_t::npos)
				break;

			path.erase(0, newPosition+1);
		}

		return result;
	}

	XMLPath(const XMLPath&);
	XMLPath& operator=(const XMLPath&);
public:

	/**
	 * コンストラクタ
	 * @param path_ XPath
	 */
	XMLPath(const string_t& path_)
		throw (std::invalid_argument)
		: path(path_)
	{
		if (path.size() == 0)
			throw std::invalid_argument("xml path is null test.");

		if (path[0] != '/')
			throw std::invalid_argument("please set absolute xml path.");
	}

	XMLPath() throw()
		: path()
	{}

	void setPath(const string_t& path_) throw (std::invalid_argument)
	{
		path = path_;

		if (path.size() == 0)
			throw std::invalid_argument("xml path is null test.");

		if (path[0] != '/')
			throw std::invalid_argument("please set absolute xml path.");
	}

	string_t getPath() const throw()
	{
		return path;
	}

	/**
	 * デストラクタ
	 */
	~XMLPath() throw()
	{}

	/**
	 * XPathの評価
	 * @param element 評価対象となるTagElement
	 * @return 評価結果となるTagElementのコレクション
	 * @todo documentRoot以外も受け付けるように変更
	 */
	std::vector<element_t*> evaluate(element_t* element)
	{

		std::vector<string_t> names = tokenize();
		std::vector<string_t> namelist =
			std::vector<string_t>(names.rbegin(), names.rend());

		if (typeid(*element) == typeid(XMLDocument<char_t>))
		{
			tag_element_t* documentRoot = NULL;
			std::vector<base_t*> children = element->getChildren();
			for (typename std::vector<base_t*>::iterator itor = 
					 children.begin(); itor != children.end(); ++itor)
			{
				if (dynamic_cast<MetaElement<char_t>*>(*itor) != NULL)
					continue;

				documentRoot = dynamic_cast<tag_element_t*>(*itor);
				if (documentRoot != NULL &&
					documentRoot->getTagName()[0] != '#')
					break;
			}
			if (documentRoot == NULL)
				return std::vector<element_t*>();

			element = documentRoot;
		}
		return traverse(element, namelist);
	}
};

/**
 * XMLパーサ
 * @todo 妥当性検証機能とか。
 */
template <typename CharType>
class XMLParser
{
	friend class XMLParserTest;
public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;
	typedef XMLNode<char_t, Element<char_t> > base_t;
	typedef Element<char_t> element_t;
	typedef typename base_t::nodes_t node_t;

private:
	/**
	 * トークナイザ
	 */
	class Tokenizer
	{
		friend class XMLParserTest;
	private:
		/**
		 * トークンの先頭位置
		 */
		const char_t* head;

		/**
		 * トークンの終端位置
		 */
		const char_t* const tail;

		/**
		 * ホワイトスペースの無視フラグ
		 */
		bool ignoreWhiteString;

	public:
		/**
		 * ホワイトスペース無視状態の取得
		 * @return ホワイトスペースを無視するか
		 */
		bool getIgnoreWhiteSpace() const throw()
		{
			return ignoreWhiteString;
		}

		/**
		 * ホワイトスペース無視状態の設定
		 * @param flag ホワイトスペース無視の設定値
		 */
		void setIgnoreWhiteSpace(const bool flag) throw()
		{
			ignoreWhiteString = flag;
		}

		/**
		 * コンストラクタ
		 * @param head_ トークナイズする文字列の先頭
		 * @param tail_ トークナイズする文字列の終端
		 */
		Tokenizer(const char_t* head_, const char_t* const tail_):
			head(head_), tail(tail_), ignoreWhiteString(false)
		{
			assert(head <= tail);
		}

		/**
		 * トークンの取得
		 * @return トークンの文字列表現
		 */
		string_t getToken() throw (WellformedException)
		{
			string_t token;
			string_t separetor;
			separetor += '\r';
			separetor += '\n';
			separetor += ' ';

			while (head != tail)
			{
				if (*head == '<' && token.length() != 0)
				{
					if (ignoreWhiteString &&
						token.find_first_not_of(separetor) ==
						string_t::npos)
					{
						token.clear();
						continue;
					}

					return token;
				}

				else if (*head == '>')
				{
					if (token[0] != '<')
						throw WellformedException();

					return token += *head++;
				}

				if (token[0] == '<')
					// remove \n character.
					std::remove(token.begin(), token.end(),
								traits_t::to_char_type('\n'));

				token += *head++;
			}

			return token;
		}

		/**
		 * ストリームが終端に達しているかの判定
		 * @return 終端に達した場合true
		 */
		bool isEof() const
		{
			assert(tail >= head);
			return head == tail;
		}
	};

	/**
	 * トークナイザへのポインタ
	 */
	Tokenizer* tokenizer;

	/**
	 * トークン種別
	 */
	enum TokenType 
	{
		OpenTag,
		CloseTag,
		SingleTag,
		CommentTag,
		Strings,
		MetaTag
	};

	/**
	 * トークンの識別
	 * @param token 切り出されたトークン
	 * @return 識別されたトークン種別
	 */
	TokenType getTokenType(const string_t& token)
		throw (InvalidTagException)
	{
		if (token.length() == 0 ||
			token[0] != '<')
			return Strings;

		assert(token.length() > 2);
		assert(token[token.length()-1] == '>');

		if (token.length() > 7 &&
			token[1] == '!' &&
			token[2] == '-' &&
			token[3] == '-' &&
			token[token.length()-2] == '-' &&
			token[token.length()-3] == '-')
			return CommentTag;


		if (token.length() > 2 &&
			isValidTagName(token.substr(1, token.length()-3)) &&
			token[token.length()-2] == '/')
			return SingleTag;

		if (token.length() > 2 &&
			isValidTagName(token.substr(1, token.length()-2)))
			return OpenTag;

		if (token.length() > 3 &&
			token[1] == '/' &&
			isValidTagName(token.substr(2, token.length()-3)))
			return CloseTag;

		if (token.length() > 4 &&
			token[1] == '?' &&
			token[token.length()-2] == '?' &&
			isValidTagName(token.substr(2, token.length()-4)))
			return MetaTag;

		throw InvalidTagException(
			Transcoder::toUTF8(token).c_str());
	}

	/**
	 * タグ名使用文字判定
	 * @param character タグ名の最初の一文字
	 * @return 使用可能文字ならtrue
	 */
	bool isFirstChar(const wchar_t character) const
	{
		if (character >= 0x0100)
		{
			if (!(character >= 0xf900 &&
				  character <= 0xfffe) ||
				character != 0xfb30)
				return true;
		}
		else if ((character >= 'A' &&
				  character <= 'Z') ||
				 (character >= 'a' &&
				  character <= 'z') ||
				 character == '_')
			return true;

		return false;
	}

	/**
	 * タグ名使用文字判定
	 * @param character タグ名の二文字目以降
	 * @return 使用可能文字ならtrue
	 */
	bool isSecondAfterChar(const wchar_t character) const
	{
		if (character >= 0x0100)
		{
			if (!(character >= 0xf900 &&
				  character <= 0xfffe) ||
				character != 0xfb30)
				return true;
		}
		else if ((character >= 'A' &&
				  character <= 'Z') ||
				 (character >= 'a' &&
				  character <= 'z') ||
				 character == '_' ||
				 (character >= '0' &&
				  character <= '9') ||
				 character == ':')
			return true;

		return false;
	}

	/**
	 * タグ名の妥当性検査
	 * @param tagName タグの名称
	 * @return タグ名として妥当ならtrue
	 */
	bool isValidTagName(const string_t& tagName) const
	{
		if (tagName.length() < 1)
			return false;

		if (isFirstChar(tagName[0]))
		{
			if (tagName.length() == 1)
				return true;

			for (typename string_t::const_iterator itor = tagName.begin();
				itor != tagName.end(); ++itor)
			{
				if (*itor == ' ')
					break;

				if (!isSecondAfterChar(*itor))
					return false;
			}

			return true;
		}

		return false;
	}

	/**
	 * XML要素の作成
	 * @param parent 親となるXML要素へのポインタ
	 * @return 新規に作成された要素へのポインタ
	 */
	element_t* createElement(element_t* parent)
	{
		const string_t token = tokenizer->getToken();
		if (token.length() == 0)
			return NULL;

		element_t* result = NULL;

		switch (getTokenType(token))
		{
			case Strings:
			{
				result = new StringElement<char_t>(token);
				parent->addChild(result);
				break;
			}

			case CommentTag:
			{
				result =
					new CommentElement<char_t>(token.substr(4,
															token.length()-7));
				parent->addChild(result);
				break;
			}

			case OpenTag:
			{
				result =
					new TagElement<char_t>(token.substr(1, token.length()-2));
				parent->addChild(result);
				while(createElement(result) != NULL)
					;
				break;
			}

			case CloseTag:
			{
				break;
			}

			case SingleTag:
			{
				result =
					new TagElement<char_t>(token.substr(1, token.length()-3));
				parent->addChild(result);
				break;
			}

			case MetaTag:
			{
				result =
					new MetaElement<char_t>(token.substr(2, token.length()-3));
				parent->addChild(result);
				break;
			}

			default:
				throw std::logic_error("unreachable position.");
				break;
		}

		return result;
	}

public:
	/**
	 * コンストラクタ
	 */
	XMLParser() throw():
		tokenizer(NULL)
	{}

	/**
	 * デストラクタ
	 */
	virtual ~XMLParser() throw()
	{
		delete tokenizer;
	}

	/**
	 * 構文解析
	 * @param document XMLドキュメント文字列
	 * @param ignoreFlag 解析に際してホワイトスペースを無視するかどうか
	 * @return 解析結果として構築されたDOMツリーへのポインタ
	 */
	XMLDocument<char_t>* parse(const string_t& document,
							   const bool ignoreFlag = false)
	{
		return parse(&document[0],
					 &document[document.length()],
					 ignoreFlag);
	}

	/**
	 * 構文解析
	 * @param head XMLドキュメント文字列先頭
	 * @param head XMLドキュメント文字列終端
	 * @param ignoreFlag 解析に際してホワイトスペースを無視するかどうか
	 * @return 解析結果として構築されたDOMツリーへのポインタ
	 */
	XMLDocument<char_t>* parse(const char_t* head,
							   const char_t* tail,
							   const bool ignoreFlag = false)
	{
		tokenizer = new Tokenizer(head, tail);
		tokenizer->setIgnoreWhiteSpace(ignoreFlag);

		XMLDocument<char_t>* theXMLDocument = new XMLDocument<char_t>();

		while(createElement(theXMLDocument) != NULL)
			;

		return theXMLDocument;
	}
};

#endif /* XMLPARSER_HPP_ */

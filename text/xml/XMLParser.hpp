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
class XMLNode
{
	friend class XMLNodeTest;
	typedef std::vector<XMLNode*> NodeType;

protected:
	/**
	 * 親ノードへのポインタ
	 */
	XMLNode* parent;

	/**
	 * 子ノードのコレクション
	 */
	NodeType childs;

public:
	/**
	 * コンストラクタ
	 * @param parent 親コンストクラタへのポインタ
	 */
	XMLNode(XMLNode* parent = NULL):
		parent(NULL),
		childs()
	{}

	/**
	 * デストラクタ
	 */
	virtual ~XMLNode() throw()
	{
		for (NodeType::iterator itor = childs.begin();
			 itor != childs.end();
			 ++itor)
			delete *itor;
	}

	/**
	 * 次の兄弟レベルのノードの探索
	 * @return 見つかった次の兄弟ノードのポインタ。次が無い場合はNULLがかえる
	 */
	XMLNode* getNextSibling() const throw(std::logic_error)
	{
		if (parent == NULL)
			return NULL;

		NodeType::iterator itor =
			std::find(parent->childs.begin(),
					  parent->childs.end(),
					  this);
		
		if (itor == parent->childs.end())
			throw std::logic_error("bad index exception.");

		if (itor+1 == parent->childs.end())
			return NULL;

		return *++itor;
	}

	/**
	 * 前の兄弟レベルのノードの探索
	 * @return 前の兄弟レベルノード
	 */
	XMLNode* getPrevSibling() const throw(std::logic_error)
	{
		if (parent == NULL)
			return NULL;

		NodeType::iterator itor =
			std::find(parent->childs.begin(),
					  parent->childs.end(),
					  this);

		if (itor == childs.end())
			throw std::logic_error("bad index exception.");

		if (itor+1 == parent->childs.begin())
			return NULL;

		return *--itor;
	}

	/**
	 * 子ノードの追加
	 * @param newChild 新しい子供ノード
	 */
	void addChild(XMLNode* newChild)
	{
		newChild->parent = this;
		childs.push_back(newChild);
	}

	/**
	 * 子ノードの削除
	 * @param child 削除する子供ノード
	 * ノードが見つからない場合、何もしません
	 */
	void removeChild(XMLNode* child)
	{
		NodeType::iterator itor =
			std::find(childs.begin(), childs.end(), child);

		if (itor != childs.end())
		{
			(*itor)->parent = NULL;
			childs.erase(itor);
		}
	}

	/**
	 * 親ノードへの接合
	 * @param newParent 親ノード
	 * @todo スコープprivateにして親からの接合しかできないようにした方がいいかも。
	 */
	void setParent(XMLNode* newParent) throw()
	{
		this->parent = newParent;
	}

	/**
	 * 親ノードの取得
	 * @return 親ノード
	 */
	XMLNode* getParent() const throw()
	{
		return this->parent;
	}

	/**
	 * 子ノードの取得
	 * @param index 0ベースインデックスとした子供ノードの位置。
	 * インデックスが無効な場合NULLを返す。
	 */
	XMLNode* getChild(const size_t index) const throw()
	{
		if (index < childs.size())
			return childs[index];

		return NULL;
	}

	/**
	 * 子ノード達の取得
	 * @return すべての子ノード
	 */
	std::vector<XMLNode*> getChildren() const throw()
	{
		return childs;
	}
};

/**
 * ノード名からDOMノードを見つけるためのファンクタ
 * @param BaseType 基底ノード型
 * @param DeriverdType 見つけるノード型
 */
template <typename BaseType, typename DeriverdType>
class FindNamePredicate
{
private:
	/**
	 * 見つけるノード名
	 */
	std::wstring findName;

public:
	/**
	 * コンストラクタ
	 * @param name 見つけるノード名
	 */
	FindNamePredicate(const std::wstring& name)
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

class TagElement;

/**
 * XML要素クラス
 */
class Element : public XMLNode
{
private:
	Element(const Element& );
	Element& operator=(const Element&);

public:
	/**
	 * 文字列変換
	 * @param indentLevel プリティプリント用インデントレベル
	 * @return 要素の文字列表現
	 */
	virtual std::wstring toString(const size_t indentLevel = 0) const throw() = 0;

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
	virtual Element* getChildElement(const std::wstring& name)
	{
		NodeType::iterator itor =
			std::find_if(childs.begin(),
						 childs.end(),
						 FindNamePredicate<XMLNode, TagElement>(name));

		if (itor != childs.end())
			return dynamic_cast<Element*>(*itor);

		return NULL;
	}

	/**
	 * 名前で指定した子要素の取得
	 * @param name タグの名前
	 * @return 見つかったXML要素のコレクション。見つからない場合は空のコレクション。
	 */
	virtual std::vector<Element*> getChildrenElements(const std::wstring& name)
	{
		FindNamePredicate<XMLNode, TagElement> predicate(name);
		std::vector<Element*> result;

		for (NodeType::iterator itor = childs.begin();
			 itor != childs.end();
			 ++itor)
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
class TagElement : public Element
{
	friend class XMLParserTest;

protected:
	/**
	 * タグの名前
	 */
	std::wstring tagName;

	/**
	 * タグの属性
	 */
	std::map<std::wstring, std::wstring> attributes;

public:
	/**
	 * コンストラクタ
	 * @param tagName_ タグの名前
	 * @param attrubutes_ タグの属性
	 */
	TagElement(const std::wstring& tagName_,
			   const std::map<std::wstring, std::wstring>& attributes_)
		:tagName(tagName_),
		 attributes(attributes_)
	{}

	/**
	 * コンストラクタ
	 * @param token タグ名を含んだパース結果文字列
	 */
	TagElement(const std::wstring& token) throw(InvalidTagException)
		:tagName(token.substr(0, token.find(L' '))),
		 attributes()
	{
		// skip white space.
		std::wstring::size_type position =
			token.find_first_not_of(L' ', tagName.length());

		std::wstring attributesName;
		std::wstring attributesValue;

		while (position < token.length())
		{
			// search attribute '=' token.
			const std::wstring::size_type
				equalPos = token.find(L'=', position + 1);

			// not found to quit.
			if (equalPos == std::wstring::npos)
			{
				if (token.substr(position + 1).find_first_not_of(L' ') ==
					std::wstring::npos)
					break;

				else
					throw InvalidTagException();
			}

			// found to get attributes name.
			attributesName =
				token.substr(position, equalPos - position);

			// equal token's next token is double quote.
			if (token[equalPos + 1] != L'"')
				throw InvalidTagException();

			// search closed quote pos.
			const std::wstring::size_type
				quotePos = token.find(L'"', equalPos + 2);

			// not found to invalid tag.
			if (quotePos == std::wstring::npos)
				throw InvalidTagException();

			// found to get attributes value.
			attributesValue =
				token.substr(equalPos + 2, quotePos - (equalPos + 2));

			// set attributes pair.
			attributes[attributesName] = attributesValue;

			// set next iterate.
			position = token.find_first_not_of(L' ', quotePos + 1);
		}
	}

	/**
	 * デストラクタ
	 */
	virtual ~TagElement() throw()
	{}

	virtual std::wstring toString(const size_t indentLevel = 0) const throw()
	{
		std::wstring result(indentLevel, L' ');
		result += L"<";
		result += tagName;

		for (std::map<std::wstring, std::wstring>::const_iterator
				 itor = attributes.begin();
			 itor != attributes.end();
			 ++itor)
			result += L" " + itor->first + L"=\"" + itor->second + L"\"";
		
		if (childs.size() == 0)
			result += L" />\n";

		else
		{
			result += L">\n";

			size_t index = 0;
			XMLNode* currentChild = this->getChild(index++);
			while (currentChild != NULL)
			{
				result += 
					dynamic_cast<Element*>(currentChild)->
					toString(indentLevel+1);
				currentChild = this->getChild(index++);
			}

			result +=
				std::wstring(indentLevel, L' ') +
				L"</" + tagName + L">\n";
		}

		return result;
	}

	/**
	 * タグ名の取得
	 * @return タグ名
	 */
	std::wstring getTagName() const throw()
	{
		return tagName;
	}

	/**
	 * 属性セットの取得
	 * @return 属性のコレクション
	 */
	std::map<std::wstring, std::wstring> getAttributes() const throw()
	{
		return attributes;
	}

	/**
	 * 属性セットの設定
	 * @param newAttributes 新しい属性のコレクション
	 */
	void setAttributes(const std::map<std::wstring, std::wstring>& newAttributes) throw()
	{
		attributes = newAttributes;
	}

	/**
	 * 属性の取得
	 * @param attributeName 属性の名前
	 * @return 属性の値
	 */
	std::wstring getAttribute(const std::wstring& attributeName) const throw()
	{
		std::map<std::wstring, std::wstring>::const_iterator itor =
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
	void setAttribute(const std::wstring& name, const std::wstring& value) throw()
	{
		attributes[name] = value;
	}
};

/**
 * コメントを表す要素
 */
class CommentElement : public Element
{
	friend class CommentElementTest;

private:
	/**
	 * コメント
	 */
	std::wstring value;

public:
	/**
	 * コンストラクタ
	 * @param value_ コメント文字列
	 */
	CommentElement(const std::wstring& value_): value(value_)
	{}

	/**
	 * デストラクタ
	 */
	virtual ~CommentElement() throw()
	{}

	virtual std::wstring toString(const size_t indentLevel = 0) const throw()
	{
		return
			std::wstring(indentLevel, L' ') +
			std::wstring(L"<!--") +
			value +
			std::wstring(L"-->\n");
	}

	/**
	 * コメント文字列の取得
	 * @return コメント文字列
	 */
	std::wstring getComment() const throw()
	{
		return value;
	}

	/**
	 * コメント文字列の設定
	 * @param comment 新しいコメント文字列
	 */
	void setComment(const std::wstring& comment) throw()
	{
		value = comment;
	}
};

/**
 * CDATA文字列要素
 */
class StringElement : public Element
{
	friend class StringElementTest;

private:
	/**
	 * 文字列
	 */
	std::wstring value;

public:
	/**
	 * コンストラクタ
	 * @param value_ CDATA文字列
	 */
	StringElement(const std::wstring& value_): value(value_)
	{}

	/**
	 * デストラクタ
	 */
	virtual ~StringElement() throw()
	{}
	
	virtual std::wstring toString(const size_t indentLevel = 0) const throw()
	{
		return std::wstring(indentLevel, L' ') + value + L"\n";
	}
	
	/**
	 * CDATA文字列の取得
	 * @return 文字列
	 */
	std::wstring getString() const throw()
	{
		return value;
	}

	/**
	 * CDATA文字列の設定
	 * @param newString
	 * 新しいCDATA文字列
	 */
	void setString(const std::wstring& newString) throw()
	{
		value = newString;
	}
		
};

/**
 * METAタグ要素
 */
class MetaElement : public TagElement
{
	friend class XMLParserTest;

public:
	/**
	 * コンストラクタ
	 * param token METAタグ名
	 */
	MetaElement(const std::wstring& token):
		TagElement(token)
	{}

	/**
	 * デストラクタ
	 */
	virtual ~MetaElement() throw()
	{}

	virtual std::wstring toString(const size_t indentLevel = 0) const throw()
	{
		assert(childs.size() == 0);

		std::wstring result(indentLevel, L' ');
		result += std::wstring(L"<?");
		result += tagName;

		for (std::map<std::wstring, std::wstring>::const_iterator
				 itor = attributes.begin();
			 itor != attributes.end();
			 ++itor)
			result += L" " + itor->first + L"=\"" + itor->second + L"\"";
		
		result += L" ?>";

		return result;
	}
};

/**
 * XMLドキュメントクラス
 */
class XMLDocument : public Element
{
public:
	virtual std::wstring toString(const size_t = 0) const throw()
	{
		std::wstring result;

		for (NodeType::const_iterator itor = childs.begin();
			 itor != childs.end();
			 ++itor)
			result += dynamic_cast<const Element*>(*itor)->toString() + L"\n";

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
class XMLPath
{
	friend class XMLParserTest;

private:
	/**
	 * 検索パス
	 */
	std::wstring path;

	/**
	 * 文字列から整数値への変換ヘルパ
	 * @param value 文字列
	 * @return 整数値
	 */
	int strToInt(const std::wstring& value)
		const throw(std::invalid_argument)
	{
		int result = 0;
		for (std::wstring::const_iterator itor = value.begin();
			 itor != value.end();
			 ++itor)
		{
			result *= 10;
			if (*itor >= L'0' &&
				*itor <= L'9')
			{
				result += static_cast<int>(*itor - '0');
			}
			else
				throw std::invalid_argument("");
		}

		return result;
	}

	/**
	 * インデックスオフセットの計算
	 * @param param XPathトークン
	 * @return インデックスオフセット
	 * @todo 実装がまだちゃんとすんでません・・・
	 * ブラケット内の数値をパースして戻り値にする必要があります
	 */
	int getIndexOf(const std::wstring& param)
		const throw(std::invalid_argument)
	{
		const std::wstring::size_type offset1 =
			param.find(L'[');

		if (offset1 == std::wstring::npos)
			return 0;

		const std::wstring::size_type offset2 =
			param.find(L']');

		if (offset2 == std::wstring::npos)
			throw std::invalid_argument("");

		if (offset1 > offset2)
			throw std::invalid_argument("");

		if ((offset2 - offset1) == 1)
			return -1;

		if ((offset2 - offset1) > 1)
		{
			std::wstring argument(
				&param[offset1+1],
				&param[offset2]);
		}

		throw std::invalid_argument("");
	}

	/**
	 * DOMツリーのトラバース
	 * @param element DOM要素の探査元
	 * @param nameList XPathからノード要素ごとに分解された文字列リスト
	 * @reutrn 見つかった要素のコレクション
	 */
	std::vector<Element*> traverse(Element* element,
					  std::vector<std::wstring> nameList)
	{
		assert(nameList.size() > 0);

		std::wstring name = nameList.back();
		nameList.pop_back();
		std::wstring canonicalizeName =
			name.substr(0, name.find(L'['));
		
		TagElement* tag = dynamic_cast<TagElement*>(element);
		if (tag == NULL)
			return std::vector<Element*>();

		if (canonicalizeName != tag->getTagName())
			return std::vector<Element*>();

		// enumerate children.
		std::vector<Element*> results;
		if (nameList.size() > 0)
		{
			std::wstring childName = nameList.back();
			const int offset = getIndexOf(childName);
			if (offset < 0)
			{
				std::vector<XMLNode*> children = 
					element->getChildren();

				for (std::vector<XMLNode*>::iterator itor =
						 children.begin();
					 itor != children.end();
					 ++itor)
				{
					TagElement* tagElem = dynamic_cast<TagElement*>(*itor);
					if (tagElem != NULL)
					{
						std::vector<Element*> result =
							traverse(tagElem, nameList);

						if (result.size() > 0)
							results.insert(results.end(),
										   result.begin(),
										   result.end());
					}
				}
			}
			else
			{
				std::wstring canonicalizeChildName =
					childName.substr(0, childName.find('['));

				std::vector<Element*> children =
					tag->getChildrenElements(canonicalizeChildName);

				Element* elem = NULL;
				if (children.size() > static_cast<unsigned int>(offset))
					elem = dynamic_cast<Element*>(children[offset]);

				std::vector<Element*> result;
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
	std::vector<std::wstring> tokenize() const
	{
		std::vector<std::wstring> result;
		std::wstring path(this->path);
		path.erase(path.begin());

		std::wstring::size_type newPosition;
		while (newPosition =
			   path.find_first_of(L'/'))
		{
			result.push_back(std::wstring(path, 0, newPosition));
			if (newPosition == std::wstring::npos)
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
	XMLPath(const std::wstring& path_)
		throw (std::invalid_argument)
		: path(path_)
	{
		if (path.size() == 0)
			throw std::invalid_argument("xml path is null test.");

		if (path[0] != L'/')
			throw std::invalid_argument("please set absolute xml path.");
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
	 */
	std::vector<Element*> evaluate(Element* element)
	{
		
		std::vector<std::wstring> names = tokenize();
		std::vector<std::wstring> namelist(names.rbegin(), names.rend());

		if (typeid(*element) == typeid(XMLDocument))
		{
			Element* documentRoot = NULL;
			std::vector<XMLNode*> children = element->getChildren();
			for (std::vector<XMLNode*>::iterator itor = children.begin();
				itor != children.end();
				++itor)
			{
				if (dynamic_cast<MetaElement*>(*itor) != NULL)
					continue;

				documentRoot = dynamic_cast<TagElement*>(*itor);
				if (documentRoot != NULL)
					break;
			}
			if (documentRoot == NULL)
				return std::vector<Element*>();

			element = documentRoot;
		}
		return traverse(element, namelist);
	}
};

/**
 * XMLパーサ
 */
class XMLParser
{
	friend class XMLParserTest;

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
		const wchar_t* head;

		/**
		 * トークンの終端位置
		 */
		const wchar_t* const tail;

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
		Tokenizer(const wchar_t* head_, const wchar_t* const tail_):
			head(head_), tail(tail_), ignoreWhiteString(false)
		{
			assert(head <= tail);
		}

		/**
		 * トークンの取得
		 * @return トークンの文字列表現
		 */
		std::wstring getToken() throw (WellformedException)
		{
			std::wstring token;
			while (head != tail)
			{
				if (*head == L'<' && token.length() != 0)
				{
					if (ignoreWhiteString &&
						token.find_first_not_of(L"\r\n ") ==
						std::wstring::npos)
					{
						token.clear();
						continue;
					}

					return token;
				}

				else if (*head == L'>')
				{
					if (token[0] != L'<')
						throw WellformedException();

					return token += *head++;
				}

				if (token[0] == L'<')
					// remove \n character.
					std::remove(token.begin(), token.end(), L'\n');

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
	TokenType getTokenType(const std::wstring& token)
		throw (InvalidTagException)
	{
		if (token.length() == 0 ||
			token[0] != L'<')
			return Strings;

		assert(token.length() > 2);
		assert(token[token.length()-1] == L'>');

		if (token.length() > 7 &&
			token[1] == L'!' &&
			token[2] == L'-' &&
			token[3] == L'-' &&
			token[token.length()-2] == L'-' &&
			token[token.length()-3] == L'-')
			return CommentTag;


		if (token.length() > 2 &&
			isValidTagName(token.substr(1, token.length()-3)) &&
			token[token.length()-2] == '/')
			return SingleTag;

		if (token.length() > 2 &&
			isValidTagName(token.substr(1, token.length()-2)))
			return OpenTag;

		if (token.length() > 3 &&
			token[1] == L'/' &&
			isValidTagName(token.substr(2, token.length()-3)))
			return CloseTag;

		if (token.length() > 4 &&
			token[1] == L'?' &&
			token[token.length()-2] == L'?' &&
			isValidTagName(token.substr(2, token.length()-4)))
			return MetaTag;

		throw InvalidTagException(
			Transcoder::UTF16toUTF8(token).c_str());
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
		else if ((character >= L'A' &&
				  character <= L'Z') ||
				 (character >= L'a' &&
				  character <= L'z') ||
				 character == L'_')
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
		else if ((character >= L'A' &&
				  character <= L'Z') ||
				 (character >= L'a' &&
				  character <= L'z') ||
				 character == L'_' ||
				 (character >= L'0' &&
				  character <= L'9'))
			return true;

		return false;
	}

	/**
	 * タグ名の妥当性検査
	 * @param tagName タグの名称
	 * @return タグ名として妥当ならtrue
	 */
	bool isValidTagName(const std::wstring& tagName) const
	{
		if (tagName.length() < 1)
			return false;

		if (isFirstChar(tagName[0]))
		{
			if (tagName.length() == 1)
				return true;

			for (std::wstring::const_iterator itor = tagName.begin();
				itor != tagName.end();
				++itor)
			{
				if (*itor == L' ')
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
	Element* createElement(Element* parent)
	{
		const std::wstring token = tokenizer->getToken();
		if (token == L"")
			return NULL;

		Element* result = NULL;

		switch (getTokenType(token))
		{
			case Strings:
			{
				result = new StringElement(token);
				parent->addChild(result);
				break;
			}

			case CommentTag:
			{
				result = new CommentElement(token.substr(4, token.length()-7));
				parent->addChild(result);
				break;
			}

			case OpenTag:
			{
				result = new TagElement(token.substr(1, token.length()-2));
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
				result = new TagElement(token.substr(1, token.length()-3));
				parent->addChild(result);
				break;
			}

			case MetaTag:
			{
				result = new MetaElement(token.substr(2, token.length()-3));
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
	XMLDocument* parse(const std::wstring& document,
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
	XMLDocument* parse(const wchar_t* head,
					   const wchar_t* tail,
					   const bool ignoreFlag = false)
	{
		tokenizer = new Tokenizer(head, tail);
		tokenizer->setIgnoreWhiteSpace(ignoreFlag);

		XMLDocument* theXMLDocument = new XMLDocument();

		while(createElement(theXMLDocument) != NULL)
			;

		return theXMLDocument;
	}
};

#endif /* XMLPARSER_HPP_ */

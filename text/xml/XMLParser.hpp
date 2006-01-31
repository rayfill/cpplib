#ifndef XMLPARSER_HPP_
#define XMLPARSER_HPP_

#include <cassert>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <text/xml/XMLException.hpp>
#include <text/transcode/Unicode.hpp>

class XMLNode
{
	friend class XMLNodeTest;
	typedef std::vector<XMLNode*> NodeType;

protected:
	XMLNode* parent;
	NodeType childs;

public:
	XMLNode(XMLNode* parent = NULL):
		parent(NULL),
		childs()
	{}

	virtual ~XMLNode() throw()
	{
		for (NodeType::iterator itor = childs.begin();
			 itor != childs.end();
			 ++itor)
			delete *itor;
	}

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

	void addChild(XMLNode* newChild)
	{
		newChild->parent = this;
		childs.push_back(newChild);
	}

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

	void setParent(XMLNode* newParent) throw()
	{
		this->parent = newParent;
	}

	XMLNode* getParent() const throw()
	{
		return this->parent;
	}

	XMLNode* getChild(const size_t index) const throw()
	{
		if (index < childs.size())
			return childs[index];

		return NULL;
	}

	std::vector<XMLNode*> getChildren() const throw()
	{
		return childs;
	}
};

template <typename BaseType, typename DeriverdType>
class FindNamePredicate
{
private:
	std::wstring findName;

public:
	FindNamePredicate(const std::wstring& name)
		: findName(name)
	{}

	~FindNamePredicate() throw()
	{}

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
class Element : public XMLNode
{
private:
	Element(const Element& );
	Element& operator=(const Element&);

public:
	virtual std::wstring toString(const size_t indentLevel = 0) const throw() = 0;

	Element() throw()
	{}

	virtual ~Element() throw()
	{}

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

class TagElement : public Element
{
	friend class XMLParserTest;

protected:
	std::wstring tagName;
	std::map<std::wstring, std::wstring> attributes;

public:
	TagElement(const std::wstring& tagName_,
			   const std::map<std::wstring, std::wstring>& attributes_)
		:tagName(tagName_),
		 attributes(attributes_)
	{}

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

	std::wstring getTagName() const throw()
	{
		return tagName;
	}

	std::map<std::wstring, std::wstring> getAttributes() const throw()
	{
		return attributes;
	}

	void setAttributes(const std::map<std::wstring, std::wstring>& newAttributes) throw()
	{
		attributes = newAttributes;
	}

	std::wstring getAttribute(const std::wstring& attributeName) const throw()
	{
		std::map<std::wstring, std::wstring>::const_iterator itor =
			attributes.find(attributeName);

		if (itor != attributes.end())
			return L"";

		return itor->second;
	}

	void setAttribute(const std::wstring& name, const std::wstring& value) throw()
	{
		attributes[name] = value;
	}
};


class CommentElement : public Element
{
	friend class CommentElementTest;

private:
	std::wstring value;

public:
	CommentElement(const std::wstring& value_): value(value_)
	{}

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

	std::wstring getComment() const throw()
	{
		return value;
	}

	void setComment(const std::wstring& comment) throw()
	{
		value = comment;
	}
};

class StringElement : public Element
{
	friend class StringElementTest;

private:
	std::wstring value;

public:
	StringElement(const std::wstring& value_): value(value_)
	{}

	virtual ~StringElement() throw()
	{}
	
	virtual std::wstring toString(const size_t indentLevel = 0) const throw()
	{
		return std::wstring(indentLevel, L' ') + value + L"\n";
	}
	
	std::wstring getString() const throw()
	{
		return value;
	}

	void setString(const std::wstring& newString) throw()
	{
		value = newString;
	}
		
};


class MetaElement : public TagElement
{
	friend class XMLParserTest;

public:
	MetaElement(const std::wstring& token):
		TagElement(token)
	{}

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

	XMLDocument() throw()
	{}
	
	virtual ~XMLDocument() throw()
	{}
};

class XMLPath
{
	friend class XMLParserTest;

private:
	std::wstring path;


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

	XMLPath(const std::wstring& path_)
		throw (std::invalid_argument)
		: path(path_)
	{
		if (path.size() == 0)
			throw std::invalid_argument("xml path is null test.");

		if (path[0] != L'/')
			throw std::invalid_argument("please set absolute xml path.");
	}

	~XMLPath() throw()
	{}

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

class XMLParser
{
	friend class XMLParserTest;

private:
	class Tokenizer
	{
		friend class XMLParserTest;
	private:
		const wchar_t* head;
		const wchar_t* const tail;
		bool ignoreWhiteString;

	public:
		bool getIgnoreWhiteSpace() const throw()
		{
			return ignoreWhiteString;
		}

		void setIgnoreWhiteSpace(const bool flag) throw()
		{
			ignoreWhiteString = flag;
		}

		Tokenizer(const wchar_t* head_, const wchar_t* const tail_):
			head(head_), tail(tail_), ignoreWhiteString(false)
		{
			assert(head <= tail);
		}

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

		bool isEof() const
		{
			assert(tail >= head);
			return head == tail;
		}
	};

	Tokenizer* tokenizer;

	enum TokenType 
	{
		OpenTag,
		CloseTag,
		SingleTag,
		CommentTag,
		Strings,
		MetaTag
	};

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
	XMLParser() throw():
		tokenizer(NULL)
	{}

	virtual ~XMLParser() throw()
	{
		delete tokenizer;
	}

	XMLDocument* parse(const std::wstring& document,
					   const bool ignoreFlag = false)
	{
		return parse(&document[0],
					 &document[document.length()],
					 ignoreFlag);
	}

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

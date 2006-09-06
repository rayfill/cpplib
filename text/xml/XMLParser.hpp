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
 * XML�m�[�h�N���X
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
	 * �e�m�[�h�ւ̃|�C���^
	 */
	node_t* parent;

	/**
	 * �q�m�[�h�̃R���N�V����
	 */
	nodes_t children;

public:
	/**
	 * �R���X�g���N�^
	 * @param parent �e�N���X�ւ̃|�C���^
	 */
	XMLNode(node_t* parent = NULL):
		parent(NULL),
		children()
	{}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~XMLNode() throw()
	{
		for (typename nodes_t::iterator itor = children.begin();
			 itor != children.end();
			 ++itor)
			delete *itor;
	}

	/**
	 * ���̌Z�탌�x���̃m�[�h�̒T��
	 * @return �����������̌Z��m�[�h�̃|�C���^�B���������ꍇ��NULL��������
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
	 * �O�̌Z�탌�x���̃m�[�h�̒T��
	 * @return �O�̌Z�탌�x���m�[�h
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
	 * �q�m�[�h�̒ǉ�
	 * @param newChild �V�����q���m�[�h
	 */
	void addChild(node_t* newChild)
	{
		newChild->parent = dynamic_cast<node_t*>(this);
		children.push_back(newChild);
	}

	/**
	 * �q�m�[�h�̍폜
	 * @param child �폜����q���m�[�h
	 * �m�[�h��������Ȃ��ꍇ�A�������܂���
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
	 * �e�m�[�h�ւ̐ڍ�
	 * @param newParent �e�m�[�h
	 * @todo �X�R�[�vprivate�ɂ��Đe����̐ڍ������ł��Ȃ��悤��
	 * �����������������B
	 */
	void setParent(node_t* newParent) throw()
	{
		this->parent = newParent;
	}

	/**
	 * �e�m�[�h�̎擾
	 * @return �e�m�[�h
	 */
	node_t* getParent() const throw()
	{
		return this->parent;
	}

	/**
	 * �q�m�[�h�̎擾
	 * @param index 0�x�[�X�C���f�b�N�X�Ƃ����q���m�[�h�̈ʒu�B
	 * �C���f�b�N�X�������ȏꍇNULL��Ԃ��B
	 */
	node_t* getChild(const size_t index) const throw()
	{
		if (index < children.size())
			return children[index];

		return NULL;
	}

	/**
	 * �q�m�[�h�B�̎擾
	 * @return ���ׂĂ̎q�m�[�h
	 */
	std::vector<node_t*> getChildren() const throw()
	{
		return children;
	}

	/**
	 * ������ϊ�
	 * @param indentLevel �v���e�B�v�����g�p�C���f���g���x��
	 * @return �v�f�̕�����\��
	 */
	virtual string_t toString(const size_t indentLevel = 0) const throw() = 0;
};

/**
 * �m�[�h������DOM�m�[�h�������邽�߂̃t�@���N�^
 * @param BaseType ���m�[�h�^
 * @param DeriverdType ������m�[�h�^
 * @todo ������R�}���h�p�^�[���ɕύX����XPath�̃T�|�[�g�q��̊g����E�E�E
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
	 * ������m�[�h��
	 */
	string_t findName;

public:
	/**
	 * �R���X�g���N�^
	 * @param name ������m�[�h��
	 */
	FindNamePredicate(const string_t& name)
		: findName(name)
	{}

	/**
	 * �f�X�g���N�^
	 */
	~FindNamePredicate() throw()
	{}

	/**
	 * �G���g���|�C���g
	 * @param child �m�[�h�̃|�C���^
	 * @return ���O�������Ȃ�true �Ⴄ�Ȃ�false
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
 * XML�v�f�N���X
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
	 * �R���X�g���N�^
	 */
	Element() throw()
	{}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~Element() throw()
	{}

	/**
	 * ���O�Ŏw�肵���q�v�f�̎擾
	 * @param name �^�O�̖��O
	 * @return ��������XML�v�f�̃|�C���^�B������Ȃ��ꍇ��NULL
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
	 * ���O�Ŏw�肵���q�v�f�̎擾
	 * @param name �^�O�̖��O
	 * @return ��������XML�v�f�̃R���N�V�����B
	 * ������Ȃ��ꍇ�͋�̃R���N�V�����B
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
 * XML�^�O�v�f
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
	 * �^�O�̖��O
	 */
	string_t tagName;

	/**
	 * �^�O�̑���
	 */
	std::map<string_t, string_t> attributes;

public:
	/**
	 * �R���X�g���N�^
	 * @param tagName_ �^�O�̖��O
	 * @param attrubutes_ �^�O�̑���
	 */
	TagElement(const string_t& tagName_,
			   const std::map<string_t, string_t>& attributes_)
		:tagName(tagName_),
		 attributes(attributes_)
	{}

	/**
	 * �R���X�g���N�^
	 * @param token �^�O�����܂񂾃p�[�X���ʕ�����
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
	 * �f�X�g���N�^
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
	 * �^�O���̎擾
	 * @return �^�O��
	 */
	string_t getTagName() const throw()
	{
		return tagName;
	}

	/**
	 * �����Z�b�g�̎擾
	 * @return �����̃R���N�V����
	 */
	std::map<string_t, string_t> getAttributes() const throw()
	{
		return attributes;
	}

	/**
	 * �����Z�b�g�̐ݒ�
	 * @param newAttributes �V���������̃R���N�V����
	 */
	void setAttributes(const std::map<string_t, string_t>& newAttributes)
		throw()
	{
		attributes = newAttributes;
	}

	/**
	 * �����̎擾
	 * @param attributeName �����̖��O
	 * @return �����̒l
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
	 * �����̐ݒ�
	 * @param name �����̖��O
	 * @param value �����̒l
	 */
	void setAttribute(const string_t& name, const string_t& value) throw()
	{
		attributes[name] = value;
	}
};

/**
 * �R�����g��\���v�f
 */
template <typename CharType>
class CommentElement : public Element<CharType>
{
	friend class CommentElementTest;

public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;

private:
	/**
	 * �R�����g
	 */
	string_t value;

public:
	/**
	 * �R���X�g���N�^
	 * @param value_ �R�����g������
	 */
	CommentElement(const string_t& value_): value(value_)
	{}

	/**
	 * �f�X�g���N�^
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
	 * �R�����g������̎擾
	 * @return �R�����g������
	 */
	string_t getComment() const throw()
	{
		return value;
	}

	/**
	 * �R�����g������̐ݒ�
	 * @param comment �V�����R�����g������
	 */
	void setComment(const string_t& comment) throw()
	{
		value = comment;
	}
};

/**
 * CDATA������v�f
 * @note �O��̋󔒕�����chop����w���p�������Ă���������
 */
template <typename CharType>
class StringElement : public Element<CharType>
{
	friend class StringElementTest;
public:
	typedef CharType char_t;
	typedef std::char_traits<char_t> traits_t;
	typedef std::basic_string<char_t> string_t;
	typedef XMLNode<char_t, Element<char_t> > base_t;

private:
	/**
	 * ������
	 */
	string_t value;

public:
	/**
	 * �R���X�g���N�^
	 * @param value_ CDATA������
	 */
	StringElement(const string_t& value_): value(value_)
	{}

	/**
	 * �f�X�g���N�^
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
	 * CDATA������̎擾
	 * @return ������
	 */
	string_t getString() const throw()
	{
		return value;
	}

	/**
	 * CDATA������̐ݒ�
	 * @param newString
	 * �V����CDATA������
	 */
	void setString(const string_t& newString) throw()
	{
		value = newString;
	}
		
};

/**
 * META�^�O�v�f
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
	 * �R���X�g���N�^
	 * param token META�^�O��
	 */
	MetaElement(const string_t& token):
		TagElement<CharType>(token)
	{}

	/**
	 * �f�X�g���N�^
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
 * XML�h�L�������g�N���X
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
	 * �R���X�g���N�^
	 */
	XMLDocument() throw()
	{}
	
	/**
	 * �f�X�g���N�^
	 */
	virtual ~XMLDocument() throw()
	{}
};

/**
 * XMLPath�p�[�T
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
	 * �����p�X
	 */
	string_t path;

	/**
	 * �����񂩂琮���l�ւ̕ϊ��w���p
	 * @param value ������
	 * @return �����l
	 */
	int strToInt(const string_t& value)
		const throw(std::invalid_argument)
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
	 * �C���f�b�N�X�I�t�Z�b�g�̌v�Z
	 * @param param XPath�g�[�N��
	 * @return �C���f�b�N�X�I�t�Z�b�g
	 * @todo �u���P�b�g����expression�������ꍇ�̑Ή��B
	 * XPathExpression�Ƃ������N���X�ł�����ĕ]�������I�u�W�F�N�g������
	 * Command�p�^�[���g���ăg���o�[�X������Ƃ����Ȃ��E�E�E
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
			 * @todo expression���͂������ꍇ�̑Ή��Ƃ��B
			 * �܂��A���̏ꍇ�͊�{�����̌��������K�v���낤���ǁE�E�E
			 */
			return
				strToInt(string_t(&param[offset1+1],
									  &param[offset2]));
		}

		throw std::invalid_argument("");
	}

	/**
	 * DOM�c���[�̃g���o�[�X
	 * @param element DOM�v�f�̒T����
	 * @param nameList XPath����m�[�h�v�f���Ƃɕ������ꂽ�����񃊃X�g
	 * @reutrn ���������v�f�̃R���N�V����
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
					tag_element_t* tagElem =
						dynamic_cast<tag_element_t*>(*itor);
					if (tagElem != NULL)
					{
						std::vector<element_t*> result =
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
	 * XPath�̃g�[�N������
	 * @return �������ꂽXPath�g�[�N��
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
	 * �R���X�g���N�^
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
	 * �f�X�g���N�^
	 */
	~XMLPath() throw()
	{}

	/**
	 * XPath�̕]��
	 * @param element �]���ΏۂƂȂ�TagElement
	 * @return �]�����ʂƂȂ�TagElement�̃R���N�V����
	 * @todo documentRoot�ȊO���󂯕t����悤�ɕύX
	 */
	std::vector<element_t*> evaluate(element_t* element)
	{
		
		std::vector<string_t> names = tokenize();
		std::vector<string_t> namelist(names.rbegin(), names.rend());

		if (typeid(*element) == typeid(XMLDocument<char_t>))
		{
			element_t* documentRoot = NULL;
			std::vector<base_t*> children = element->getChildren();
			for (typename std::vector<base_t*>::iterator itor = 
					 children.begin(); itor != children.end(); ++itor)
			{
				if (dynamic_cast<MetaElement<char_t>*>(*itor) != NULL)
					continue;

				documentRoot = dynamic_cast<tag_element_t*>(*itor);
				if (documentRoot != NULL)
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
 * XML�p�[�T
 * @todo �Ó������؋@�\�Ƃ��B
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
	 * �g�[�N�i�C�U
	 */
	class Tokenizer
	{
		friend class XMLParserTest;
	private:
		/**
		 * �g�[�N���̐擪�ʒu
		 */
		const char_t* head;

		/**
		 * �g�[�N���̏I�[�ʒu
		 */
		const char_t* const tail;

		/**
		 * �z���C�g�X�y�[�X�̖����t���O
		 */
		bool ignoreWhiteString;

	public:
		/**
		 * �z���C�g�X�y�[�X������Ԃ̎擾
		 * @return �z���C�g�X�y�[�X�𖳎����邩
		 */
		bool getIgnoreWhiteSpace() const throw()
		{
			return ignoreWhiteString;
		}

		/**
		 * �z���C�g�X�y�[�X������Ԃ̐ݒ�
		 * @param flag �z���C�g�X�y�[�X�����̐ݒ�l
		 */
		void setIgnoreWhiteSpace(const bool flag) throw()
		{
			ignoreWhiteString = flag;
		}

		/**
		 * �R���X�g���N�^
		 * @param head_ �g�[�N�i�C�Y���镶����̐擪
		 * @param tail_ �g�[�N�i�C�Y���镶����̏I�[
		 */
		Tokenizer(const char_t* head_, const char_t* const tail_):
			head(head_), tail(tail_), ignoreWhiteString(false)
		{
			assert(head <= tail);
		}

		/**
		 * �g�[�N���̎擾
		 * @return �g�[�N���̕�����\��
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
		 * �X�g���[�����I�[�ɒB���Ă��邩�̔���
		 * @return �I�[�ɒB�����ꍇtrue
		 */
		bool isEof() const
		{
			assert(tail >= head);
			return head == tail;
		}
	};

	/**
	 * �g�[�N�i�C�U�ւ̃|�C���^
	 */
	Tokenizer* tokenizer;

	/**
	 * �g�[�N�����
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
	 * �g�[�N���̎���
	 * @param token �؂�o���ꂽ�g�[�N��
	 * @return ���ʂ��ꂽ�g�[�N�����
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
	 * �^�O���g�p��������
	 * @param character �^�O���̍ŏ��̈ꕶ��
	 * @return �g�p�\�����Ȃ�true
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
	 * �^�O���g�p��������
	 * @param character �^�O���̓񕶎��ڈȍ~
	 * @return �g�p�\�����Ȃ�true
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
	 * �^�O���̑Ó�������
	 * @param tagName �^�O�̖���
	 * @return �^�O���Ƃ��đÓ��Ȃ�true
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
	 * XML�v�f�̍쐬
	 * @param parent �e�ƂȂ�XML�v�f�ւ̃|�C���^
	 * @return �V�K�ɍ쐬���ꂽ�v�f�ւ̃|�C���^
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
	 * �R���X�g���N�^
	 */
	XMLParser() throw():
		tokenizer(NULL)
	{}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~XMLParser() throw()
	{
		delete tokenizer;
	}

	/**
	 * �\�����
	 * @param document XML�h�L�������g������
	 * @param ignoreFlag ��͂ɍۂ��ăz���C�g�X�y�[�X�𖳎����邩�ǂ���
	 * @return ��͌��ʂƂ��č\�z���ꂽDOM�c���[�ւ̃|�C���^
	 */
	XMLDocument<char_t>* parse(const string_t& document,
							   const bool ignoreFlag = false)
	{
		return parse(&document[0],
					 &document[document.length()],
					 ignoreFlag);
	}

	/**
	 * �\�����
	 * @param head XML�h�L�������g������擪
	 * @param head XML�h�L�������g������I�[
	 * @param ignoreFlag ��͂ɍۂ��ăz���C�g�X�y�[�X�𖳎����邩�ǂ���
	 * @return ��͌��ʂƂ��č\�z���ꂽDOM�c���[�ւ̃|�C���^
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

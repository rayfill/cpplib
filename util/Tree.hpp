#ifndef TREE_HPP_
#define TREE_HPP_

#include <vector>
#include <algorithm>
#include <util/Predicate.hpp>

/**
 * 木の要素
 * @note メンバは用意してないので継承して増やしてください
 */
class Node
{
public:

private:
	Node* parent;
	std::vector<Node*> children;

	void removeChildren() 
	{
		std::for_each(children.begin(), children.end(),
					  Predicate::Remover<Node>());
	}

public:
	Node():
		parent(NULL),
		children()
	{}

	virtual ~Node()
	{
		removeChildren();
	}

public:
	bool removeChild(const Node* child)
	{
		std::vector<Node*>::iterator itor =
			std::find(children.begin(), children.end(), child);
		if (itor == children.end())
			return false;

		children.erase(itor);
		return true;
	}

	bool addChild(Node* child)
	{
		std::vector<Node*>::const_iterator itor = 
			std::find(children.begin(), children.end(), child);
		if (itor != children.end())
			return false;

		children.push_back(child);
		return true;
	}

	std::vector<Node*> getChildren() const
	{
		return children;
	}

	void setParent(Node* newParent)
	{
		if (parent != NULL)
			parent->removeChild(this);

		newParent->addChild(this);
		parent = newParent;
	}

	Node* getParent() const
	{
		return parent;
	}
};

/**
 * 木本体
 * @note ノード基底はテンプレートパラメタのNodeTypeに指定してください。
 * デフォルトでクラスNodeになってます。
 */
template <typename NodeType = Node>
class Tree
{
	friend class TreeTest;

public:
	typedef NodeType node_t;

private:
	node_t* root;

	class iterator
	{
	private:
		node_t* current;

		node_t* advance()
		{
			while (current != NULL)
			{
				if (current->getParent() == NULL)
					return NULL;

				std::vector<node_t*> children =
					current->getParent()->getChild();
				typename std::vector<node_t*>::iterator itor =
					std::find(children.begin(), children.end(), current);

				if (*itor == children.back())
				{
					current = current->getParent();
					continue;
				}
				else
				{
					++itor;
					while (itor->getChildren().size() != 0)
						itor = itor->getChildren().front();

					current = *itor;
				}
			}
			return current;
		}

	public:
		iterator():
			current()
		{}

		iterator(const iterator& source):
			current(source.current)
		{}

		iterator(node_t* cur):
			current(cur)
		{}

		~iterator()
		{}

		bool operator==(const iterator& rhs) const
		{
			return current == rhs.current;
		}

		iterator& operator++()
		{
			advance();
			return *this;
		}

		iterator operator++(int)
		{
			iterator result(*this);
			advance();
			return result;
		}

		node_t* operator*()
		{
			return current;
		}
	};

public:
	Tree():
		root(new node_t())
	{}

	~Tree()
	{
		delete root;
	}

	node_t* getRoot()
	{
		return root;
	}

	void setRoot(node_t* newRoot)
	{
		root = newRoot;
	}

	iterator begin()
	{
		if (root->getChildren().size() == 0)
			return end();

		typename std::vector<node_t*>::iterator itor = root->getChildren();
			
		while (itor->getChildren().size() != 0)
			itor = itor->getChildren().front();

		return iterator(*itor);
	}

	iterator end()
	{
		return iterator(root);
	}

};

#endif /* TREE_HPP_ */

#ifndef GRAPHICS_HPP_
#define GRAPHICS_HPP_

#include <list>
#include <iterator>
#include <math/Geometry.hpp>
#include <Win32/Bitmap.hpp>

/**
 * 描画可能をあらわすインタフェース
 */
class Drawable
{
	friend class GrapicsTest;

public:
	typedef DIBitmap<PixelFormat<RGB32> > Bitmap;

protected:
	Bitmap plane;

public:
	Drawable() throw():
		plane()
	{}

	virtual ~Drawable() throw()
	{
	}

	virtual void draw() = 0;

	const Bitmap* getImage() const
	{
		return &plane;
	}
};

/**
 * 移動可能をあらわすインタフェース
 */
class Movable
{
	friend class GraphicsTest;

private:
	Movable();

protected:
	geometry::Point<int> position;

public:
	Movable(int x, int y):
	  position(x, y)
	{}

	virtual ~Movable() throw()
	{}

	void move(const geometry::Point<int>& diff) throw()
	{
		position += diff;
	}

	void move(int xDiff, int yDiff) throw()
	{
		position += geometry::Point<int>(xDiff, yDiff);
	}

	geometry::Point<int> getPosition() const throw()
	{
		return position;
	}

	void setPosition(const geometry::Point<int>& newPosition) throw()
	{
		position = newPosition;
	}

};

/**
 * 選択可能をあらわすインタフェース
 */
class Selectable
{
	friend class GraphicsTest;

private:
	Selectable();

protected:
	geometry::Point<int> size;
	bool selected;

public:
	Selectable(int width, int height) throw()
		: size(width, height),
		  selected(false)
	{}

	virtual void select() throw()
	{
		selected = true;
	}

	virtual void unselect() throw()
	{
		selected = false;
	}

	bool isSelected() const throw()
	{
		return selected;
	}

	virtual void setSize(const geometry::Point<int>& newSize) throw()
	{
		size = newSize;
	}

	virtual geometry::Point<int> getSize() const throw()
	{
		return size;
	}

	virtual ~Selectable() throw()
	{}
};

/**
 * マウスピック可能をあらわすインタフェース
 */
class Pickable :
	public Movable,
	public Selectable
{
private:
	Pickable();

protected:

public:
	Pickable(int x, int y, int width, int height):
	  Movable(x, y),
	  Selectable(width, height)
	{}

	virtual ~Pickable() throw()
	{}

	virtual bool isBounds(const geometry::Point<int>& point) const throw()
	{
		return geometry::Rectangle<int>(
			position, position + size).isCollision(point);
	}
};

/**
 * 描画Zオーダのための順序リストクラス
 */
template <typename OrderedItemType>
class OrderList
{
	friend class GraphicsTest;

public:
	typedef OrderedItemType ItemType;

private:
	std::list<ItemType*> itemList;

public:
	typedef typename std::list<ItemType*>::iterator iterator;
	typedef typename std::list<ItemType*>::const_iterator const_iterator;
	typedef typename std::list<ItemType*>::reverse_iterator reverse_iterator;
	typedef typename std::list<ItemType*>::const_reverse_iterator const_reverse_iterator;

	OrderList() throw()
		: itemList()
	{}

	~OrderList() throw()
	{}

	void add(ItemType* item)
	{
		itemList.push_front(item);
	}

	void addToBack(ItemType* item)
	{
		itemList.push_back(item);
	}

	void remove(ItemType* item)
	{
		itemList.remove(item);
	}

	void moveToTop(ItemType* item)
	{
		remove(item);
		add(item);
	}

	void moveToBack(ItemType* item)
	{
		remove(item);
		addToBack(item);
	}

	ItemType* operator[](int offset) const throw()
	{
		const_iterator result = itemList.begin();
		std::advance(result, offset);
		return *result;
	}

	void cyclic(bool isClockwise = true) throw()
	{
		if (isClockwise)
		{
			itemList.push_back(itemList.front());
			itemList.pop_front();
		}
		else
		{
			itemList.push_front(itemList.back());
			itemList.pop_back();
		}
	}

	ItemType* getTop() const throw()
	{
		if (itemList.size() == 0)
			return NULL;

		return itemList.front();
	}

	ItemType* getBottom() const throw()
	{
		if (itemList.size() == 0)
			return NULL;

		return itemList.back();
	}

	iterator begin()
	{
		return itemList.begin();
	}

	const_iterator begin() const
	{
		return itemList.begin();
	}

	iterator end()
	{
		return itemList.end();
	}

	const_iterator end() const
	{
		return itemList.end();
	}

	reverse_iterator rbegin()
	{
		return itemList.rbegin();
	}

	const_reverse_iterator rbegin() const
	{
		return itemList.rbegin();
	}

	reverse_iterator rend()
	{
		return itemList.rend();
	}

	const_reverse_iterator rend() const
	{
		return itemList.rend();
	}
};

#endif /* GRAPHICS_HPP_ */

#ifndef DIRTYAREA_HPP_
#define DIRTYAREA_HPP_

#include <math/Geometry.hpp>

class DirtyArea
{
protected:
	geometry::Rectangle<int> area;

public:
	DirtyArea(): area()
	{}

	DirtyArea(const DirtyArea& source) throw()
		: area(source.area)
	{}

	virtual ~DirtyArea() throw()
	{}

	DirtyArea& operator=(const DirtyArea& source) throw()
	{
		if (this != &source)
			this->area = source.area;

		return *this;
	}


	void addDirtyArea(const geometry::Rectangle<int>& newDirty)
	{
		if (area == geometry::Rectangle<int>())
			area = newDirty;
		else
			area = area.getUnion(newDirty);
	}

	void reset() throw()
	{
		area = geometry::Rectangle<int>();
	}

	geometry::Rectangle<int> getDirtyArea() const throw()
	{
		return area;
	}
};

#endif /* DIRTYAREA_HPP_ */

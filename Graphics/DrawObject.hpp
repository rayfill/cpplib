#ifndef DRAWOBJECT_HPP_
#define DRAWOBJECT_HPP_


#include <Graphics/Graphics.hpp>

class DrawObject :
	public Drawable,
	public Pickable
{
public:
	DrawObject(int x, int y, int width, int height)
		: Drawable(), Pickable(x, y, width, height)
	{
		plane.setWidth(width);
		plane.setHeight(height);
		plane.setTopDown();
		plane.createDIBSection();
	}

	virtual void draw()
	{
		const Color color(isSelected() ? 0x7f : 0, 0, isSelected() ? 0 : 0x7f);
		for (Bitmap::iterator itor = plane.begin();
			itor != plane.end();
			++itor)
			itor->setColor(color);
	}
};

#endif /* DRAWOBJECT_HPP_ */

#ifndef PIXELSTORE_HPP_
#define PIXELSTORE_HPP_

#include <vector>

/**
 * @param ElementType GLRGB or GLRGBA or GLDepth or
 * RGB32 or RGBA32 and RGB24.
 */
template <typename ElementType>
class PixelStore
{
public:
	enum {
		StoreType = ElementType::StoreType,
		ColorType = ElementType::ColorType,
	};

private:
	unsigned int width;
	unsigned int height;

	std::vector<ElementType> elements;

public:
	PixelStore(const unsigned int width_, const unsigned int height_):
			width(width_), height(height_), elements(width_*height_)
	{}

	~PixelStore()
	{}

	unsigned int getWidth() const
	{
		return width;
	}

	unsigned int getHeight() const
	{
		return height;
	}

	typedef typename std::vector<ElementType>::iterator iterator;
	typedef typename std::vector<ElementType>::const_iterator const_iterator;

	iterator begin()
	{
		return elements.begin();
	}

	iterator end()
	{
		return elements.end();
	}

	const_iterator begin() const
	{
		return elements.begin();
	}

	const_iterator end() const
	{
		return elements.end();
	}

	/**
	 * 制限をもうちょっと強くしたほうがいいな。（ex. height, widthチェックとか）
	 * あと行頭、行末のイテレータは別のんで取れるようにしたほうがいいかも。
	 */
	iterator getPixel(const unsigned int x, const unsigned int y)
	{
		assert(x <= width);
		assert(y <= height);

		return elements.begin() + (x + y * width);
	}

	const_iterator getPixel(const unsigned int x, const unsigned int y) const
	{
		assert(x <= width);
		assert(y <= height);

		return elements.begin() + (x + y * width);
	}
};

#endif /* PIXELSTORE_HPP_ */


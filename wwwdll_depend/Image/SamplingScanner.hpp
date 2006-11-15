#ifndef SAMPLINGSCANNER_HPP_
#define SAMPLINGSCANNER_HPP_

#include <cassert>

template <typename BitmapClass, size_t scaleFactor>
class SamplingScanner
{
	friend class SamplingScannerTest;

private:
	enum
	{
		xShiftFactor = scaleFactor == 4 ? 0 : 1,
		yShiftFactor = scaleFactor == 1 ? 1 : 0
	};
	
	const BitmapClass& image;

	SamplingScanner() throw();

public:
	SamplingScanner(const BitmapClass& imageSource) throw():
		image(imageSource)
	{
		assert(scaleFactor == 4 || scaleFactor == 2 || scaleFactor == 1);
	}

	const typename BitmapClass::BaseType::BaseType*
	getPixel(size_t x, size_t y) const
	{
		return image.getPixel(x << xShiftFactor, y << yShiftFactor);
	}
};

#endif /* SAMPLINGSCANNER_HPP_ */

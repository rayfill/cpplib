#ifndef MATRIX_HPP_
#define MATRIX_HPP_
#include <vector>
#include <cassert>

/**
 * 行列クラス
 * @param width 行列の幅
 * @param height 行列の高さ
 * @param ElementType 要素の型
 * @todo 作成途中もいいとこなんで3D用途むけなどなら 
 * http://rayfill.dyndns.org:2481/opengl_lib/ のほうを使ってください
 * @see http://rayfill.dyndns.org:2481/opengl_lib/GL/Geometry.hpp
 */
template <size_t width, size_t height, typename ElementType = float>
class Matrix
{
private:
	ElementType matrix[width * height];

public:
	Matrix() throw()
		: matrix()
	{
		initialize(0);
	}

	void initialize(ElementType initialValue = static_cast<ElementType>(0))
		const throw()
	{
		for (size_t y = 0; y < height; ++y)
			for (size_t x = 0; x < width; ++x)
				matrix[x + y * width] = initialValue;
	}

	void initializeBase(ElementType initialValue = static_cast<ElementType>(0))
		const throw()
	{
		assert(width == height);

		for (size_t y = 0; y < height; ++y)
			for (size_t x = 0; x < width; ++x)
				matrix[x + y * width] = (x == y) ? initialValue : 0;
	}

	size_t getWidth() const throw()
	{
		return width;
	}

	size_t getHeight() const throw()
	{
		return height;
	}

	const ElementType operator()(const size_t x, const size_t y) const throw()
	{
		assert(x < width);
		assert(y < height);

		return matrix[y * width + x];
	}

	ElementType& operator()(size_t x, size_t y) throw()
	{
		assert(x < width);
		assert(y < height);

		return matrix[y * width + x];
	}

	Matrix operator*(Matrix dest) throw()
	{
		assert(this->width() == dest.height());
		assert(this->height() == dest.width());

		Matrix result;

		for (size_t y = 0; y < height; ++y)
		{
			for (size_t x = 0; x < width; ++x)
			{
				for (size_t k = 0; k < this->height(); ++k)
				{
					result(x, y) += (*this)(x, k) * dest(k, y);
				}
			}
		}

		return result;
	}

	Matrix operator=(ElementType number) const throw()
	{
		this->initializeBase(number)
	}
};

#endif /* MATRIX_HPP_ */

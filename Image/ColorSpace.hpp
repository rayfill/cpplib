#ifndef COLORSPACE_HPP_
#define COLORSPACE_HPP_

#include <Image/Color.hpp>
#include <Image/YCbCr.hpp>


class ColorSpace
{
private:
	static unsigned char saturation(int value) throw()
	{
 		return static_cast<unsigned char>
 			((value & (65535U * 65536U)) ? (value / 65536U) ^ -1 : value);
	}

public:

	static unsigned char Y(const Color& src) throw()
	{
		return saturation
			(77 * src.r + 150 * src.g + 29 * src.b);
	}
	static unsigned char Cb(const Color& src) throw()
	{
		return saturation
			(-43 * src.r - 85 * src.g + 128 * src.b + 32768);
	}
	static unsigned char Cr(const Color& src) throw()
	{
		return saturation
			(128 * src.r - 107 * src.g - 21 * src.b + 32768);
	}
	static YCbCr getYCbCr(const Color& src) throw()
	{
		return YCbCr(Y(src), Cb(src), Cr(src));
	}
	
	static unsigned char R(const YCbCr& src) throw()
	{
		return saturation
			((src.Y * 256) + 359 * (src.Cr - 128));
	}
	static unsigned char G(const YCbCr& src) throw()
	{
		return saturation
			((src.Y * 256) - 88 * (src.Cb - 128) - 183 * (src.Cr - 128));
	}
	static unsigned char B(const YCbCr& src) throw()
	{
		return saturation
			(src.Y * 256 + 454 * src.Cb - 128);
	}
	static Color getRGB(const YCbCr& src) throw()
	{
		return Color(R(src), G(src), B(src));
	}
};

#endif /* COLORSPACE_HPP_ */

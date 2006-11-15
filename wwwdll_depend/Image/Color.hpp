#ifndef COLOR_HPP_
#define COLOR_HPP_

#include <iostream>

struct Color
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;

	Color() throw():
		r(), g(), b()
	{}

	Color(unsigned char r_,
		  unsigned char g_,
		  unsigned char b_) throw():
			  r(r_), g(g_), b(b_)
	{}

	Color(const Color& src) throw():
		r(src.r), g(src.g), b(src.b)
	{}
		
	void setColor(unsigned char r_,
			 unsigned char g_,
			 unsigned char b_) throw()
	{
		r = r_;
		g = g_;
		b = b_;
	}

	bool operator==(const Color& dest) const throw()
	{
		return (r == dest.r) && (g == dest.g) && (b == dest.b);
	}

	bool operator!=(const Color& dest) const throw()
	{
		return !(this->operator==(dest));
	}

	Color operator^(const Color& dest) const throw()
	{
		return Color(this->r ^ dest.r, this->g ^ dest.g, this->b ^ dest.b);
	}

	friend std::ostream& operator<<(std::ostream& st, const Color& src)
	{
		st << "Red: " << (int)src.r <<
			", Green: " << (int)src.g <<
			", Blue: " << (int)src.b;
		return st;
	}
};


#endif /* COLOR_HPP */

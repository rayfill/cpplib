#ifndef COLOR_HPP_
#define COLOR_HPP_

#include <ostream>

struct Color
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	Color() throw():
		r(), g(), b(),a()
	{}

	Color(unsigned char r_,
		  unsigned char g_,
		  unsigned char b_,
		  unsigned char a_ = 255) throw():
		r(r_), g(g_), b(b_), a(a_)
	{}

	Color(const Color& src) throw():
			r(src.r), g(src.g), b(src.b), a(src.a)
	{}
		
	void setColor(unsigned char r_,
				  unsigned char g_,
				  unsigned char b_,
				  unsigned char a_ = 255) throw()
	{
		r = r_;
		g = g_;
		b = b_;
	}

	bool operator==(const Color& dest) const throw()
	{
		return
			(r == dest.r) &&
			(g == dest.g) &&
			(b == dest.b) &&
			(a == dest.a);
	}

	bool operator!=(const Color& dest) const throw()
	{
		return !(this->operator==(dest));
	}

	Color operator^(const Color& dest) const throw()
	{
		return Color(this->r ^ dest.r,
					 this->g ^ dest.g,
					 this->b ^ dest.b,
					 this->a ^ dest.a);
	}

	friend std::ostream& operator<<(std::ostream& st, const Color& src)
	{
		st << "Red: " << (int)src.r <<
			", Green: " << (int)src.g <<
			", Blue: " << (int)src.b <<
			", Alpha: " << (int)src.a;
		return st;
	}
};


#endif /* COLOR_HPP */

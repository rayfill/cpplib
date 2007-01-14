#ifndef YCBCR_HPP_
#define YCBCR_HPP_

#include <ostream>

struct YCbCr
{
public:
	unsigned char Y;
	unsigned char Cb;
	unsigned char Cr;

	YCbCr() throw()
		: Y(), Cb(), Cr()
	{}

	YCbCr(unsigned char Y_,
		  unsigned char Cb_,
		  unsigned char Cr_) throw()
		: Y(Y_), Cb(Cb_), Cr(Cr_)
	{}

	YCbCr(const YCbCr& src) throw()
		: Y(src.Y), Cb(src.Cb), Cr(src.Cr)
	{}

	void setYCbCr(unsigned char Y_,
		  unsigned char Cb_,
		  unsigned char Cr_) throw()
	{
		Y = Y_;
		Cb = Cb_;
		Cr = Cr_;
	}

	bool operator==(const YCbCr& src) const throw()
	{
		return (Y == src.Y) && (Cb == src.Cb) && (Cr == src.Cr);
	}

	bool operator!=(const YCbCr& src) const throw()
	{
		return !(this->operator==(src));
	}

	friend std::ostream& operator<<(std::ostream& st, const YCbCr& src)
	{
		st << "Y: " << (int)src.Y <<
			", Cb: " << (int)src.Cb <<
			", Cr: " << (int)src.Cr;
		return st;
	}
};


#endif /* YCBCR_HPP_ */

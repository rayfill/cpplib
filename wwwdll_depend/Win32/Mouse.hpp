#ifndef MOUSE_HPP_
#define MOUSE_HPP_

#include <windows.h>
#include <math/Geometry.hpp>

class Mouse
{
private:
	HWND owner;

public:
	Mouse():
		owner(static_cast<HWND>(INVALID_HANDLE_VALUE))
	{}

	~Mouse()
	{
		if (owner != INVALID_HANDLE_VALUE)
			release();
	}

	bool isCaptured() const throw()
	{
		return owner != INVALID_HANDLE_VALUE;
	}

	void capture(HWND owner_)
	{
		::SetCapture(owner_);
		owner = owner_;
	}

	void release()
	{
		if (owner != INVALID_HANDLE_VALUE)
		{
			::ReleaseCapture();
			owner = static_cast<HWND>(INVALID_HANDLE_VALUE);
		}
	}
		
};

#endif /* MOUSE_HPP_ */

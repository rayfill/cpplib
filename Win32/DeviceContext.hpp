#ifndef DEVICECONTEXT_HPP_
#define DEVICECONTEXT_HPP_
#include <windows.h>
#include <stdexcept>
#include <math/Geometry.hpp>

class DeviceContext
{
private:
	HWND deviceSource;

	HBITMAP stockBitmap;
	HBRUSH stockBrush;
	HFONT stockFont;
	HPEN stockPen;
	HRGN stockRegion;

	void restoreStockObjects()
	{
		if (stockBitmap != NULL)
		{
			SelectObject(deviceContext, stockBitmap);
			stockBitmap = NULL;
		}

		if (stockBrush != NULL)
		{
			SelectObject(deviceContext, stockBrush);
			stockBrush = NULL;
		}

		if (stockFont != NULL)
		{
			SelectObject(deviceContext, stockFont);
			stockFont = NULL;
		}

		if (stockPen != NULL)
		{
			SelectObject(deviceContext, stockPen);
			stockPen = NULL;
		}

		if (stockRegion != NULL)
		{
			SelectObject(deviceContext, stockRegion);
			stockRegion = NULL;
		}
	}

	void release()
	{
		if (deviceSource == INVALID_HANDLE_VALUE)
			// memory compatible.
			DeleteDC(deviceContext);
		else
			// source is hwnd.
			ReleaseDC(deviceSource, deviceContext);

		deviceSource = static_cast<HWND>(INVALID_HANDLE_VALUE);
		deviceContext = NULL;
	}

protected:
	HDC deviceContext;

public:
	DeviceContext(HWND source) throw(std::invalid_argument)
		: deviceContext(NULL),
		  deviceSource(source),
		  stockBitmap(NULL),
		  stockBrush(NULL),
		  stockFont(NULL),
		  stockPen(NULL),
		  stockRegion(NULL)
	{
		deviceContext = ::GetDC(source);
		if (deviceContext == (HDC)NULL)
			throw std::invalid_argument("invalid hWnd value");
	}

	DeviceContext(HDC source = NULL) throw(std::invalid_argument)
		: deviceContext(NULL),
		  deviceSource((HWND)INVALID_HANDLE_VALUE),
		  stockBitmap(NULL),
		  stockBrush(NULL),
		  stockFont(NULL),
		  stockPen(NULL),
		  stockRegion(NULL)
	{
		deviceContext = CreateCompatibleDC(source);
		if (deviceContext == NULL)
			throw std::invalid_argument(
				"invalid memory device context handle.");
	}

	virtual ~DeviceContext()
	{
		restoreStockObjects();
		release();
	}

	void attach(HDC deviceContext_)
	{
		restoreStockObjects();
		release();

		deviceContext = deviceContext_;
	}

	HDC detach()
	{
		HDC result = deviceContext;
		deviceContext = NULL;

		return result;
	}

	HDC getDeviceContext() const throw()
	{
		return deviceContext;
	}

	bool blockTransfer(const DeviceContext& dest,
					   const geometry::Rectangle<int>& sourcePosition,
					   const geometry::Point<int>& destPosition) const throw()
	{
		return ::BitBlt(this->deviceContext,
						destPosition.getX(),
						destPosition.getY(),
						sourcePosition.getWidth(),
						sourcePosition.getHeight(),
						dest.getDeviceContext(),
						sourcePosition.getLeft(),
						sourcePosition.getTop(),
						SRCCOPY) == TRUE;
	}

	void setBitmap(HBITMAP bitmap) throw()
	{
		if (stockBitmap == NULL)
			stockBitmap =
				static_cast<HBITMAP>(SelectObject(deviceContext, bitmap));
		else
			SelectObject(deviceContext, bitmap);
	}

	void setBrush(HBRUSH brush) throw()
	{
		if (stockBrush == NULL)
			stockBrush =
				static_cast<HBRUSH>(SelectObject(deviceContext, brush));
		else
			SelectObject(deviceContext, brush);
	}

	void setFont(HFONT font) throw()
	{
		if (stockFont == NULL)
			stockFont =
				static_cast<HFONT>(SelectObject(deviceContext, font));
		else
			SelectObject(deviceContext, font);
	}

	void setPen(HPEN pen) throw()
	{
		if (stockPen == NULL)
			stockPen =
				static_cast<HPEN>(SelectObject(deviceContext, pen));
		else
			SelectObject(deviceContext, pen);
	}

	void setRegion(HRGN region) throw()
	{
		if (stockRegion == NULL)
			stockRegion =
				static_cast<HRGN>(SelectObject(deviceContext, region));
		else
			SelectObject(deviceContext, region);
	}

};

class PaintDeviceContext : public DeviceContext
{
private:
	HDC backupDeviceContext;
	HWND targetWindow;
	PAINTSTRUCT paintStruct;

	PaintDeviceContext();
	PaintDeviceContext(const PaintDeviceContext&);
	PaintDeviceContext& operator=(const PaintDeviceContext&);
public:
	PaintDeviceContext(HWND target):
		DeviceContext(),
		backupDeviceContext(NULL),
		targetWindow(target),
		paintStruct()
	{
		backupDeviceContext = deviceContext;
		deviceContext = 
			::BeginPaint(targetWindow, &paintStruct);
	}

	~PaintDeviceContext() throw()
	{
		deviceContext = backupDeviceContext;
		::EndPaint(targetWindow, &paintStruct);
	}

	geometry::Rectangle<int> getInvalidateRectangle() const throw()
	{
		return 
			geometry::Rectangle<int>(
				paintStruct.rcPaint.left,
				paintStruct.rcPaint.top,
				paintStruct.rcPaint.right,
				paintStruct.rcPaint.bottom);
	}
};

#endif /* DEVICECONTEXT_HPP_ */

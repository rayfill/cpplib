#ifndef BITMAP_HPP_
#define BITMAP_HPP_

#include <windows.h>
#include <cassert>
#include <stdexcept>
#include <cstddef>
#include <memory.h>
#include <Image/Color.hpp>
#include <math/Geometry.hpp>

/**
 * PixelFormatBaseの32bit bit fieldの実装
 */
class RGB32
{
private:
	unsigned char pixel[4];

public:
	enum
	{
		offsetSize = 4 /// 画素あたりのサイズ
	};

	unsigned char getRed() const throw()
	{
		return pixel[3];
	}
	unsigned char getGreen() const throw()
	{
		return pixel[2];
	}
	unsigned char getBlue() const throw()
	{
		return pixel[1];
	}
	Color getColor() const throw()
	{
		return Color(getRed(),
					 getGreen(),
					 getBlue());
	}

	void setRed(const unsigned char r) throw()
	{
		pixel[3] = r;
	}
	void setGreen(const unsigned char g) throw()
	{
		pixel[2] = g;
	}
	void setBlue(const unsigned char b) throw()
	{
		pixel[1] = b;
	}
	void setColor(const unsigned char r,
				  const unsigned char g,
				  const unsigned char b) throw()
	{
		*reinterpret_cast<DWORD*>(pixel) =
			(r << 24) | (g << 16) | (b << 8);
	}

	void setColor(const Color& color) throw()
	{
		*reinterpret_cast<DWORD*>(pixel) =
			(color.r << 24) | (color.g << 16) | (color.b << 8);
	}

	bool operator==(const RGB32& dist) const throw()
	{
		return
			*reinterpret_cast<const DWORD*>(this->pixel) ==
			*reinterpret_cast<const DWORD*>(dist.pixel);
	}

	static bool isBitFieldFormat() throw()
	{
		return true;
	}
	static DWORD getRedBitField() throw()
	{
		return 0xff000000;
	}
	static DWORD getGreenBitField() throw()
	{
		return 0xff0000;
	}
	static DWORD getBlueBitField() throw()
	{
		return 0xff00;
	}
};


/**
 * PixelFormatBaseのR(8bit), G(8bit), B(8bit)の実装
 */
class RGB24
{
private:
	unsigned char pixel[3];
	
public:
	enum {
		offsetSize = 3 /// 画素あたりのサイズ
	};

	unsigned char getRed() const throw()
	{
		return pixel[2];
	}
	unsigned char getGreen() const throw()
	{
		return pixel[1];
	}
	unsigned char getBlue() const throw()
	{
		return pixel[0];
	}
	Color getColor() const throw()
	{
		return Color(pixel[2],
					 pixel[1],
					 pixel[0]);
	}

	void setRed(unsigned char r) throw()
	{
		pixel[2] = r;
	}
	void setGreen(unsigned char g) throw()
	{
		pixel[1] = g;
	}
	void setBlue(unsigned char b) throw()
	{
		pixel[0] = b;
	}
	void setColor(unsigned char r,
				  unsigned char g,
				  unsigned char b) throw()
	{
		pixel[0] = b;
		pixel[1] = g;
		pixel[2] = r;
	}

	void setColor(const Color& color) throw()
	{
		pixel[0] = color.b;
		pixel[1] = color.g;
		pixel[2] = color.r;
	}

	bool operator==(const RGB24& dist) const throw()
	{
		return
			(this->pixel[0] == dist.pixel[0]) &&
			(this->pixel[1] == dist.pixel[1]) &&
			(this->pixel[2] == dist.pixel[2]);
	}

	static bool isBitFieldFormat() throw()
	{
		return false;
	}
	static DWORD getRedBitField() throw()
	{
		return 0;
	}
	static DWORD getGreenBitField() throw()
	{
		return 0;
	}
	static DWORD getBlueBitField() throw()
	{
		return 0;
	}
};

/**
 * 画素の抽象化クラス
 */
template <typename PixelFormatBase>
class PixelFormat : public PixelFormatBase
{
public:
	typedef PixelFormatBase BaseType;
	typedef PixelFormatBase* iterator;
	typedef const PixelFormatBase* const_iterator;

	PixelFormat() throw()
	{
	}
	
	PixelFormat& operator=(const PixelFormat& src) throw()
	{
		setColor(src.getColor());
		return *this;
	}

	bool operator==(const PixelFormat<PixelFormatBase>& dist) const throw()
	{
		return PixelFormatBase::operator==(dist);
	}
};


template <typename PixelFormatClass, size_t DPISize = 96>
class DIBitmap
{
private:
	BITMAPINFOHEADER bitmapInfo;
	DWORD bitField[3];
	enum
	{
		red = 0,
		green = 1,
		blue = 2
	};

	BITMAPINFO* info;
	HBITMAP bitmapResource;
	PixelFormatClass* pixelPointer;
	bool isTopDown;

	const size_t getPointerLength() const throw() /// 画素の終端
	{
		return getLineLengthForByte() * this->getHeight();
	}

	const size_t getLineLengthForByte() const throw() /// 画素の行終端
	{
		return bitmapInfo.biWidth *
			PixelFormatClass::offsetSize + 3 & 0xfffffffc;
	}

	void bitmapInfoClose() throw()
	{
		free(info);
		info = NULL;
	}

	void bitmapInfoCreate() throw(std::bad_alloc)
	{
		assert(info == NULL);
		info = (BITMAPINFO*)calloc(
			sizeof(BITMAPINFOHEADER) + 
			(PixelFormatClass::isBitFieldFormat() == false ?
			 0 : sizeof(RGBQUAD) * 3), 1);
		
		if (info == NULL)
			throw std::bad_alloc();

		memcpy(info, &bitmapInfo, sizeof(bitmapInfo));
		if (PixelFormatClass::isBitFieldFormat())
		{
			info->bmiColors[red] =
				*reinterpret_cast<RGBQUAD*>(&bitField[red]);
			info->bmiColors[green] =
				*reinterpret_cast<RGBQUAD*>(&bitField[green]);
			info->bmiColors[blue] =
				*reinterpret_cast<RGBQUAD*>(&bitField[blue]);
		}
		else
		{
			info->bmiColors[0].rgbBlue = 0;
			info->bmiColors[0].rgbGreen = 0;
			info->bmiColors[0].rgbRed = 0;
			info->bmiColors[0].rgbReserved = 0;
		}
	}

	void closeBitmap()
	{
		assert(bitmapResource != 0);
		assert(pixelPointer != 0);

		::DeleteObject(bitmapResource);
		pixelPointer = 0;
	}

	void setColorPixelFormat()
	{
		bitmapInfo.biBitCount = 8 * PixelFormatClass::offsetSize;
		bitmapInfo.biCompression = BI_RGB;
	}

	void setBitFieldPixelFormat()
	{
		DWORD redBitReps = PixelFormatClass::getRedBitField();
		DWORD greenBitReps = PixelFormatClass::getGreenBitField();
		DWORD blueBitReps = PixelFormatClass::getBlueBitField();
		DWORD alphaBitReps = 0x00000000;

		assert(((redBitReps ^ greenBitReps) & redBitReps) == redBitReps);
		assert(((redBitReps ^ blueBitReps) & redBitReps) == redBitReps);
		assert(((redBitReps ^ alphaBitReps) & redBitReps) == redBitReps);
		assert(((greenBitReps ^ blueBitReps) & greenBitReps) == greenBitReps);
		assert(((greenBitReps ^ alphaBitReps) & greenBitReps) == greenBitReps);
		assert(((blueBitReps ^ alphaBitReps) & blueBitReps) == blueBitReps);
		
		assert(PixelFormatClass::offsetSize == 4 ||
			   PixelFormatClass::offsetSize == 2);

		bitmapInfo.biBitCount =
			static_cast<WORD>(8 * PixelFormatClass::offsetSize);
		bitmapInfo.biCompression = BI_BITFIELDS;

		bitField[red] = redBitReps;
		bitField[green] = greenBitReps;
		bitField[blue] = blueBitReps;
	}


public:
	typedef PixelFormatClass BaseType;
	typedef typename PixelFormatClass::iterator iterator;
	typedef typename PixelFormatClass::const_iterator const_iterator;

	DIBitmap() throw()
		: bitmapInfo(), bitField(), info(NULL),
		  bitmapResource(NULL), pixelPointer(NULL),
		  isTopDown(false)
	{
		bitmapInfo.biSize = sizeof(bitmapInfo);
		bitmapInfo.biPlanes = 1;
		bitmapInfo.biXPelsPerMeter = static_cast<int>(DPISize * 100 / 2.54);
		bitmapInfo.biYPelsPerMeter = static_cast<int>(DPISize * 100 / 2.54);
		bitmapInfo.biClrUsed = 0;
		bitmapInfo.biClrImportant = 0;

	}

	DIBitmap(const DIBitmap& source)
		: bitmapInfo(source.bitmapInfo), bitField(), info(NULL),
		  bitmapResource(NULL), pixelPointer(NULL),
		  isTopDown(source.isTopDown)
	{
		createDIBSection();
		std::copy(source.begin(), source.end(), this->begin());
	}

	virtual ~DIBitmap() throw()
	{
		close();
	}

	void close()
	{
		bitmapInfoClose();
		if (bitmapResource != NULL)
			closeBitmap();
	}

	const size_t getWidth() const
	{
		return bitmapInfo.biWidth;
	}
	void setWidth(size_t width)
	{
		bitmapInfo.biWidth = static_cast<LONG>(width);
	}

	const size_t getHeight() const
	{
		if (isTopDown)
			return bitmapInfo.biHeight * -1;

		return bitmapInfo.biHeight;
	}

	void setTopDown() throw()
	{
		isTopDown = true;
		if (bitmapInfo.biHeight > 0)
			bitmapInfo.biHeight = -bitmapInfo.biHeight;
	}

	void setBottomUp() throw()
	{
		isTopDown = false;
		if (bitmapInfo.biHeight < 0)
			bitmapInfo.biHeight = -bitmapInfo.biHeight;
	}

	void setHeight(size_t height)
	{
		if (isTopDown)
			bitmapInfo.biHeight = static_cast<LONG>(height) * -1;
		else
			bitmapInfo.biHeight = static_cast<LONG>(height);
	}

	bool createDIBSection()
	{
		assert(bitmapInfo.biWidth > 0);
		assert(bitmapInfo.biHeight != 0);
		assert(bitmapResource == NULL);
		assert(info == NULL);

		if (!PixelFormatClass::isBitFieldFormat())
			setColorPixelFormat();
		else
			setBitFieldPixelFormat();
		
		assert(bitmapInfo.biBitCount > 0);
		
		this->bitmapInfoCreate();

		bitmapResource = 
			::CreateDIBSection(NULL,
							   this->info,
							   DIB_RGB_COLORS,
							   reinterpret_cast<void**>(&this->pixelPointer),
							   NULL,
							   0);
		if (bitmapResource == NULL)
			return false;

		return true;
	}

	iterator begin() const
	{
		return pixelPointer;
	}

	iterator end() const
	{
		return pixelPointer + (getWidth() * getHeight());
	}

	iterator getPixel(size_t x, size_t y) throw()
	{
		return pixelPointer + (x + y * getWidth());
	}

	const_iterator getPixel(size_t x, size_t y) const throw()
	{
		return pixelPointer + (x + y * getWidth());
	}

	BITMAPINFO* getBitmapInfo()
	{
		assert(info);
		return info;
	}

	HBITMAP getBitmapHandle() const throw()
	{
		return this->bitmapResource;
	}

	bool loadFile(const TCHAR* filename)
	{
		HANDLE hFile =
			CreateFile(filename, GENERIC_READ, 0, 
					   NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		try 
		{
			DWORD readCount;
			BITMAPFILEHEADER bfh;
			if ((ReadFile(hFile, &bfh, sizeof(bfh),
						  &readCount, NULL) == FALSE) ||
				readCount != sizeof(bfh))
				throw std::runtime_error("bitmap file header invalid.");

			if (bfh.bfType != ('B' + 'M' * 256))
				throw std::runtime_error("bitmap file header invalid.");

			LONG currentPointer =
				SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
			if (SetFilePointer(hFile, 0, NULL, FILE_END) < bfh.bfSize)
				throw std::runtime_error("file is broken.");

			SetFilePointer(hFile, currentPointer, NULL, FILE_BEGIN);
			
			if ((ReadFile(hFile, &this->bitmapInfo,
						  sizeof(this->bitmapInfo),
						  &readCount, NULL) == FALSE) ||
				readCount != sizeof(this->bitmapInfo))
				throw std::runtime_error("bitmap info header invalid.");

			if (!this->createDIBSection())
				throw std::runtime_error("DIB section create failed.");

			SetFilePointer(hFile, bfh.bfOffBits, NULL, FILE_BEGIN);
			if ((ReadFile(hFile, this->begin(),
						  this->getPointerLength(),
						  &readCount, NULL) == FALSE) ||
				readCount != this->getPointerLength())
				throw std::runtime_error("file read error occured.");
		}
		catch(std::runtime_error& /*e*/)
		{
			CloseHandle(hFile);
			return false;
		}

		CloseHandle(hFile);
		return true;
	}

	bool saveFile(const TCHAR* filename)
	{
		HANDLE hFile =
			CreateFile(filename, GENERIC_WRITE, 0, NULL,
					   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		DWORD writeCount;
		/* BITMAPFILEHEADER */
		BITMAPFILEHEADER bfh;
		bfh.bfType = ('M' << 8) + 'B';
		bfh.bfSize =
			sizeof(BITMAPFILEHEADER) + // file header.
			sizeof(BITMAPINFOHEADER) + // info header.
			sizeof(DWORD) + // rgbquad.
			((this->end() - this->begin()) * 3); // pixel data.
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		bfh.bfOffBits = 
			sizeof(BITMAPFILEHEADER) + 
			sizeof(BITMAPINFOHEADER) +
			sizeof(DWORD);
		if(!WriteFile(hFile, (void*)&bfh,
					  sizeof(BITMAPFILEHEADER),
					  &writeCount, NULL))
			return false;

		/* BITMAPINFOHEADER */
		if(!WriteFile(hFile, reinterpret_cast<void*>(&this->bitmapInfo),
			sizeof(BITMAPINFOHEADER), &writeCount, NULL))
			return false;

		/* RGBQUAD(bitfield.) */
		if(!WriteFile(hFile,
					  reinterpret_cast<void*>(this->bitField),
					  sizeof(DWORD),
			&writeCount, NULL))
			return false;

		/* pixel data. */
		typename DIBitmap<PixelFormatClass, DPISize>::iterator
			itor = this->begin();

		if(!WriteFile(hFile, reinterpret_cast<void*>(itor), 
		sizeof(*itor) * getWidth() * getHeight(), &writeCount, NULL))
			return false;

		CloseHandle(hFile);

		return true;
	}
};

template <typename sourceType, typename destinationType>
class BlockTransfer
{
private:
	typedef sourceType SourceType;
	typedef destinationType DestinationType;

	const SourceType& source;
	DestinationType& destination;

	template <typename SrcIterator,
		typename DestIterator>
	void transferFragment(SrcIterator head, SrcIterator tail,
						DestIterator destHead)
	{
		for (; head != tail; ++head, ++destHead)
			destHead->setColor(head->getColor());
	}

public:
	BlockTransfer(const sourceType& src, destinationType& dest) throw()
		: source(src), destination(dest)
	{}

	~BlockTransfer() throw()
	{}

	template <typename pointType> 
	void transfer(
		const geometry::Rectangle<pointType>& sourceRect,
		const geometry::Point<pointType>& destPoint)
		throw(std::invalid_argument)
	{
		typedef pointType point_t;
		if (geometry::Rectangle<point_t>
			(0, 0, source.getWidth(), source.getHeight()).
			getIntersection(sourceRect) != sourceRect)
			throw std::invalid_argument("source rectangle is out of range.");

		const geometry::Rectangle<point_t>
			destRect(
				0,
				0,
				destination.getWidth(),
				destination.getHeight());

		const geometry::Rectangle<point_t>
			destTargetRect(
				destPoint.getX(),
				destPoint.getY(),
				destPoint.getX() + sourceRect.getWidth(),
				destPoint.getY() + sourceRect.getHeight());
		if (!destRect.isCollision(destTargetRect))
			return;

		geometry::Rectangle<point_t> transferRect = 
			destRect.getProjection(destTargetRect);
		
		const geometry::Point<point_t> projectionMove =
			sourceRect.getLeftTop() - destPoint;
		const geometry::Rectangle<point_t>
			srcTransferRect(transferRect + projectionMove);

		// srcTransferRect -> transferRect copy.
		// line transter
		for (point_t srcHeight = srcTransferRect.getTop(),
				 destHeight = transferRect.getTop();
			 srcHeight != srcTransferRect.getBottom();
			 ++srcHeight, ++destHeight)
			transferFragment(
				source.getPixel(srcTransferRect.getLeft(), srcHeight),
				source.getPixel(srcTransferRect.getRight(), srcHeight),
				destination.getPixel(transferRect.getLeft(), destHeight));
	}
};

template <class sourceType, class destinationType>
class TransparencyTransfer
{
private:
	typedef sourceType SourceType;
	typedef destinationType DestinationType;

	const SourceType& source;
	DestinationType& destination;
	const Color transparentColor;

	template <typename SrcIterator,
		typename DestIterator>
	void transferFragment(SrcIterator head, SrcIterator tail,
						DestIterator destHead)
	{
		for (; head != tail; ++head, ++destHead)
		{
			if (head->getColor() != transparentColor)
			destHead->setColor(head->getColor());
		}
	}

public:
	TransparencyTransfer(const sourceType& src,
						 destinationType& dest,
						 Color color) throw()
		: source(src), destination(dest), transparentColor(color)
	{}

	~TransparencyTransfer() throw()
	{}

	template <typename pointType> 
	void transfer(
		const geometry::Rectangle<pointType>& sourceRect,
		const geometry::Point<pointType>& destPoint)
		throw(std::invalid_argument)
	{
		typedef pointType point_t;
		if (geometry::Rectangle<point_t>
			(0, 0, source.getWidth(), source.getHeight()).
			getIntersection(sourceRect) != sourceRect)
			throw std::invalid_argument("source rectangle is out of range.");

		const geometry::Rectangle<point_t>
			destRect(
				0,
				0,
				destination.getWidth(),
				destination.getHeight());

		const geometry::Rectangle<point_t>
			destTargetRect(
				destPoint.getX(),
				destPoint.getY(),
				destPoint.getX() + sourceRect.getWidth(),
				destPoint.getY() + sourceRect.getHeight());
		if (!destRect.isCollision(destTargetRect))
			return;

		geometry::Rectangle<point_t> transferRect = 
			destRect.getProjection(destTargetRect);
		
		const geometry::Point<point_t> projectionMove =
			sourceRect.getLeftTop() - destPoint;
		const geometry::Rectangle<point_t>
			srcTransferRect(transferRect + projectionMove);

		// srcTransferRect -> transferRect copy.
		// line transter
		for (point_t srcHeight = srcTransferRect.getTop(),
				 destHeight = transferRect.getTop();
			 srcHeight != srcTransferRect.getBottom();
			 ++srcHeight, ++destHeight)
			transferFragment(
				source.getPixel(srcTransferRect.getLeft(), srcHeight),
				source.getPixel(srcTransferRect.getRight(), srcHeight),
				destination.getPixel(transferRect.getLeft(), destHeight));
	}
};

template <class sourceType, class destinationType>
class ExclusiveOrTransfer
{
private:
	typedef sourceType SourceType;
	typedef destinationType DestinationType;

	const SourceType& source;
	DestinationType& destination;
	const Color exclusiveColor;

	template <typename SrcIterator,
		typename DestIterator>
	void transferFragment(SrcIterator head, SrcIterator tail,
						DestIterator destHead)
	{
		for (; head != tail; ++head, ++destHead)
		{
			destHead->setColor(head->getColor() ^ exclusiveColor);
		}
	}

public:
	ExclusiveOrTransfer(const sourceType& src,
						 destinationType& dest,
						 Color color) throw()
		: source(src), destination(dest), exclusiveColor(color)
	{}

	~ExclusiveOrTransfer() throw()
	{}

	template <typename pointType> 
	void transfer(
		const geometry::Rectangle<pointType>& sourceRect,
		const geometry::Point<pointType>& destPoint)
		throw(std::invalid_argument)
	{
		typedef pointType point_t;
		if (geometry::Rectangle<point_t>
			(0, 0, source.getWidth(), source.getHeight()).
			getIntersection(sourceRect) != sourceRect)
			throw std::invalid_argument("source rectangle is out of range.");

		const geometry::Rectangle<point_t>
			destRect(
				0,
				0,
				destination.getWidth(),
				destination.getHeight());

		const geometry::Rectangle<point_t>
			destTargetRect(
				destPoint.getX(),
				destPoint.getY(),
				destPoint.getX() + sourceRect.getWidth(),
				destPoint.getY() + sourceRect.getHeight());
		if (!destRect.isCollision(destTargetRect))
			return;

		geometry::Rectangle<point_t> transferRect = 
			destRect.getProjection(destTargetRect);
		
		const geometry::Point<point_t> projectionMove =
			sourceRect.getLeftTop() - destPoint;
		const geometry::Rectangle<point_t>
			srcTransferRect(transferRect + projectionMove);

		// srcTransferRect -> transferRect copy.
		// line transter
		for (point_t srcHeight = srcTransferRect.getTop(),
				 destHeight = transferRect.getTop();
			 srcHeight != srcTransferRect.getBottom();
			 ++srcHeight, ++destHeight)
			transferFragment(
				source.getPixel(srcTransferRect.getLeft(), srcHeight),
				source.getPixel(srcTransferRect.getRight(), srcHeight),
				destination.getPixel(transferRect.getLeft(), destHeight));
	}
};

#endif /* BITMAP_HPP_ */

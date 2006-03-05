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
 * PixelFormatBase��32bit bit field�̎���
 * �z��Ƃ��Ĉ����̂Ńf�X�g���N�^�̗�O���S�����K�{�B
 * �܂��C�e���[�V�������̍����T�C�Y���N���X�^�Ɉˑ�����̂�
 * POD struct�ɋ߂��\���ɂ��Ă��������B
 * �܂��A����ł����߂Ȃ�POD�\���̂ŉ��Ƃ�������@�l���܂��B
 * @todo �K�v�Ȃ�16bit�ł��p�ӂ��邯�ǁE�E�E���܂��炢��Ȃ����?
 */
class RGB32
{
private:
	/**
	 * ��f�l�̓����\��
	 */
	unsigned char pixel[4];

public:
	enum
	{
		offsetSize = 4 /// ��f������̃T�C�Y
	};

	/**
	 * �ԗv�f�̎擾
	 * @return �ԗv�f�l
	 */
	unsigned char getRed() const throw()
	{
		return pixel[3];
	}

	/**
	 * �Ηv�f�̎擾
	 * @return �Ηv�f�l
	 */
	unsigned char getGreen() const throw()
	{
		return pixel[2];
	}

	/**
	 * �v�f�̎擾
	 * @return �v�f�l
	 */
	unsigned char getBlue() const throw()
	{
		return pixel[1];
	}

	/**
	 * �F�\���̂̎擾
	 * @return �F�\����
	 * ������g�����ƂŌ^�Ⴂ���m�ł̐F�R�s�[���������Ă܂��B
	 * �C�����C���W�J�y�эœK�������ƕ�����̃R�s�[�������܂肽���܂��
	 * �󂯎�葤��f = ����f;
	 * �̌`�ɂ܂ōœK������邱�Ƃő��x���オ�}��܂��B
	 * ����ȏ�ɂ���ƂȂ��DMA���ڑ���Ƃ��^���Ƃœ��ʉ�����
	 * Blitter�t�@���N�^�Ŋg�����߂�������Ƃ��ŉ��Ƃ����邵���E�E�E
	 */
	Color getColor() const throw()
	{
		return Color(getRed(),
					 getGreen(),
					 getBlue());
	}

	/**
	 * �ԗv�f�̐ݒ�
	 * @param r �ԗv�f
	 */
	void setRed(const unsigned char r) throw()
	{
		pixel[3] = r;
	}

	/**
	 * �Ηv�f�̐ݒ�
	 * @param g �Ηv�f
	 */
	void setGreen(const unsigned char g) throw()
	{
		pixel[2] = g;
	}

	/**
	 * �v�f�̐ݒ�
	 * @param b �v�f
	 */
	void setBlue(const unsigned char b) throw()
	{
		pixel[1] = b;
	}

	/**
	 * �F�̐ݒ�
	 * @param r �ԗv�f
	 * @param g �Ηv�f
	 * @param b �v�f
	 */
	void setColor(const unsigned char r,
				  const unsigned char g,
				  const unsigned char b) throw()
	{
		*reinterpret_cast<DWORD*>(pixel) =
			(r << 24) | (g << 16) | (b << 8);
	}

	/**
	 * �F�̐ݒ�
	 * @param color �F��\���\����
	 */
	void setColor(const Color& color) throw()
	{
		*reinterpret_cast<DWORD*>(pixel) =
			(color.r << 24) | (color.g << 16) | (color.b << 8);
	}

	/**
	 * ���l��r
	 * �F�����������true
	 */
	bool operator==(const RGB32& dist) const throw()
	{
		return
			*reinterpret_cast<const DWORD*>(this->pixel) ==
			*reinterpret_cast<const DWORD*>(dist.pixel);
	}

	/**
	 * DIB�\���̏ꍇ�Ƀr�b�g�t�B�[���h���K�v��
	 * @return �K�v�Ȃ̂ŏ��true
	 */
	static bool isBitFieldFormat() throw()
	{
		return true;
	}

	/**
	 * �ԗv�f�������r�b�g�t�B�[���h�}�X�N�l
	 * @return �ԗv�f�}�X�N
	 */
	static DWORD getRedBitField() throw()
	{
		return 0xff000000;
	}

	/**
	 * �Ηv�f�������r�b�g�t�B�[���h�}�X�N�l
	 * @return �Ηv�f�}�X�N
	 */
	static DWORD getGreenBitField() throw()
	{
		return 0xff0000;
	}

	/**
	 * �v�f�������r�b�g�t�B�[���h�}�X�N�l
	 * @return �v�f�}�X�N
	 */
	static DWORD getBlueBitField() throw()
	{
		return 0xff00;
	}
};


/**
 * PixelFormatBase��R(8bit), G(8bit), B(8bit)�̎���
 * ���\�b�h������RGB32�̕����Q��
 * @see RGB32
 */
class RGB24
{
private:
	unsigned char pixel[3];
	
public:
	enum {
		offsetSize = 3 /// ��f������̃T�C�Y
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
 * �s�N�Z���t�H�[�}�b�g�N���X
 * ��f�̒��ۉ�
 * �vPOD�\���B���z�֐��Ƃ��͋֎~�ł��B
 * @param PixelFormatBase ��f�N���X�^
 */
template <typename PixelFormatBase>
class PixelFormat : public PixelFormatBase
{
public:
	typedef PixelFormatBase BaseType;
	typedef PixelFormatBase* iterator;
	typedef const PixelFormatBase* const_iterator;

	/**
	 * �R���X�g���N�^
	 */
	PixelFormat() throw()
	{
	}
	
	/**
	 * ������Z�q
	 * @param �R�s�[���s�N�Z���t�H�[�}�b�g
	 * @return �����̎��g�ւ̎Q��
	 */
	PixelFormat& operator=(const PixelFormat& src) throw()
	{
		setColor(src.getColor());
		return *this;
	}

	/**
	 * ��r���Z�q
	 * @return �F�����������true
	 */
	bool operator==(const PixelFormat<PixelFormatBase>& dist) const throw()
	{
		return PixelFormatBase::operator==(dist);
	}
};


/**
 * �f�o�C�X�Ɨ��r�b�g�}�b�v�N���X
 * @param PixelFormatClass �s�N�Z���t�H�[�}�b�g�N���X
 * @param DPISize ��ʂ̉𑜓x 
 * (�܂��A�قƂ�ǈӖ����Ȃ��̂Ńf�t�H���g�̂܂܂�OK�ł��傤)
 * @todo ��f�s�̖��[�ł�4�o�C�g�A���C���ɂ��Ă�����
 * �Ώ����ĂȂ��̂ł��̑Ή����K�v
 */
template <typename PixelFormatClass, size_t DPISize = 96>
class DIBitmap
{
private:
	/**
	 * �r�b�g�}�b�v�C���t�H�w�b�_�\����
	 */
	BITMAPINFOHEADER bitmapInfo;

	/**
	 * 16/32�r�b�gDIB�p�}�X�N�e�[�u��
	 */
	DWORD bitField[3];

	enum
	{
		red = 0,
		green = 1,
		blue = 2
	};

	/**
	 * �r�b�g�}�b�v�C���t�H�\���̂ւ̃|�C���^
	 */
	BITMAPINFO* info;

	/**
	 * DIBSection�n���h��
	 */
	HBITMAP bitmapResource;

	/**
	 * ��f�ւ̃|�C���^
	 */
	PixelFormatClass* pixelPointer;

	/**
	 * DIB���g�b�v�_�E�����{�g���A�b�v��������킷�t���O
	 * true�Ńg�b�v�_�E��
	 */
	bool isTopDown;

	/**
	 * ��f�̏I�[
	 * @todo first = getPointer(), last = getPointer() + getPointerLength();
	 * �Ƃ������Ɗ�Ȃ����Ƃ�����̂Ń��\�b�h�Ȃ��������B
	 * blitter�o�R�Ȃ�s�R�s�[�������������J��Ԃ��̂ő��v�Ȃ񂾂��E�E�E
	 */
	const size_t getPointerLength() const throw()
	{
		return getLineLengthForByte() * this->getHeight();
	}

	/**
	 * ��f�̍s�I�[
	 */
	const size_t getLineLengthForByte() const throw()
	{
		return bitmapInfo.biWidth *
			PixelFormatClass::offsetSize + 3 & 0xfffffffc;
	}

	/**
	 * �r�b�g�}�b�v�C���t�H�\���̂̊J��
	 */
	void bitmapInfoClose() throw()
	{
		free(info);
		info = NULL;
	}

	/**
	 * �r�b�g�}�b�v�C���t�H�\���̂̍쐬
	 */
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

	/** 
	 * DIB�n���h���̊J��
	 */
	void closeBitmap()
	{
		assert(bitmapResource != 0);
		assert(pixelPointer != 0);

		::DeleteObject(bitmapResource);
		pixelPointer = 0;
	}

	/**
	 * ��f�̒l��RGB�l��\���ꍇ�̐ݒ�
	 */
	void setColorPixelFormat()
	{
		bitmapInfo.biBitCount = 8 * PixelFormatClass::offsetSize;
		bitmapInfo.biCompression = BI_RGB;
	}

	/**
	 * ��f�̒l���}�X�N�l�ɂ��RGB�l��\���ꍇ�̐ݒ�
	 */
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

	/**
	 * �R���X�g���N�^
	 */
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

	/**
	 * �R�s�[�R���X�g���N�^
	 * @param source ����DIBitmap�I�u�W�F�N�g
	 */
	DIBitmap(const DIBitmap& source)
		: bitmapInfo(source.bitmapInfo), bitField(), info(NULL),
		  bitmapResource(NULL), pixelPointer(NULL),
		  isTopDown(source.isTopDown)
	{
		createDIBSection();
		std::copy(source.begin(), source.end(), this->begin());
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~DIBitmap() throw()
	{
		close();
	}

	/**
	 * �Ǘ����\�[�X�̊J��
	 */
	void close()
	{
		bitmapInfoClose();
		if (bitmapResource != NULL)
			closeBitmap();
	}

	/**
	 * ��s�̉�f�����擾����
	 * @return ��s�̉�f��
	 */
	const size_t getWidth() const
	{
		return bitmapInfo.biWidth;
	}
	
	/**
	 * ��s�̉�f����ݒ肷��
	 * @param width ��s�̉�f��
	 */
	void setWidth(size_t width)
	{
		assert((width % 4) == 0);
		bitmapInfo.biWidth = static_cast<LONG>(width);
	}

	/**
	 * �s�̍������擾����
	 * @return DIB�̍s��
	 */
	const size_t getHeight() const
	{
		if (isTopDown)
			return bitmapInfo.biHeight * -1;

		return bitmapInfo.biHeight;
	}

	/**
	 * �g�b�v�_�E��DIB�ɂ���
	 */
	void setTopDown() throw()
	{
		isTopDown = true;
		if (bitmapInfo.biHeight > 0)
			bitmapInfo.biHeight = -bitmapInfo.biHeight;
	}

	/**
	 * �{�g���A�b�vDIB�ɂ���
	 */
	void setBottomUp() throw()
	{
		isTopDown = false;
		if (bitmapInfo.biHeight < 0)
			bitmapInfo.biHeight = -bitmapInfo.biHeight;
	}

	/**
	 * �����̐ݒ�
	 * @param height �����̒l
	 */
	void setHeight(size_t height)
	{
		if (isTopDown)
			bitmapInfo.biHeight = static_cast<LONG>(height) * -1;
		else
			bitmapInfo.biHeight = static_cast<LONG>(height);
	}

	/**
	 * DIBSeciton�̍쐬
	 */
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

	/**
	 * �擪��f�ւ̃C�e���[�^�̎擾
	 * @return �擪��f�̃C�e���[�^
	 */
	iterator begin() const
	{
		return pixelPointer;
	}

	/**
	 * �I�[��f�ւ̃C�e���[�^�̎擾
	 * @return �I�[��f�̃C�e���[�^
	 */
	iterator end() const
	{
		return pixelPointer + (getWidth() * getHeight());
	}

	/**
	 * �w��ʒu�̉�f�C�e���[�^�̎擾
	 * @return �w��ʒu��f�̃C�e���[�^
	 * @param x �摜��x���W
	 * @param y �摜��y���W
	 */
	iterator getPixel(size_t x, size_t y) throw()
	{
		return pixelPointer + (x + y * getWidth());
	}

	/**
	 * �w��ʒu�̉�f�C�e���[�^�̎擾
	 * @param x �摜��x���W
	 * @param y �摜��y���W
	 * @return �w��ʒu��f�̃C�e���[�^
	 */
	const_iterator getPixel(size_t x, size_t y) const throw()
	{
		return pixelPointer + (x + y * getWidth());
	}

	/**
	 * �r�b�g�}�b�v�C���t�H�\���̂̎擾
	 * @return �r�b�g�}�b�v�\���̂ւ̃|�C���^
	 */
	BITMAPINFO* getBitmapInfo()
	{
		assert(info);
		return info;
	}

	/**
	 * DIB�̃n���h���̎擾
	 * @return DIB�̃r�b�g�}�b�v�n���h��
	 */
	HBITMAP getBitmapHandle() const throw()
	{
		return this->bitmapResource;
	}

	/**
	 * �t�@�C������DIB�����[�h���ADIBitmap���쐬
	 * @param filename �r�b�g�}�b�v���\�[�X�ւ̃t���p�X
	 */
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

	/**
	 * �����\���r�b�g�}�b�v�̕ۑ�
	 * 
	 */
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

/**
 * ��`�r�b�g�}�b�v�]���t�@���N�^
 * @param sourceType �]�����̃r�b�g�}�b�v�C�e���[�^�^
 * @param destinationType �]����̃r�b�g�}�b�v�C�e���[�^�^
 * ���^�̏ꍇ�AC++�̃C�����C���W�J�ƍœK���ɂ�荂���ȃR�s�[���ł��܂��B
 * �^���Ⴄ�ꍇ�ł��������R�s�[�͂ł��܂����A���x�͗����܂��B
 */
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

/**
 * ���ߐF�t�r�b�g�}�b�v�]��
 */
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

/**
 * �r���I�_���a�]��
 */
template <class sourceType, class destinationType>
class ExclusiveOrTransfer
{
private:
	typedef sourceType SourceType;
	typedef destinationType DestinationType;

	/**
	 * �]����
	 */
	const SourceType& source;

	/**
	 * �]����
	 */
	DestinationType& destination;
	
	/**
	 * �}�X�N�J���[
	 */
	const Color exclusiveColor;

	/**
	 * ��񕪂̓]��
	 */
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
	/**
	 * �R���X�g���N�^
	 * @param src �]����
	 * @param dest �]����
	 * @param color �}�X�N�J���[
	 */
	ExclusiveOrTransfer(const sourceType& src,
						 destinationType& dest,
						 Color color) throw()
		: source(src), destination(dest), exclusiveColor(color)
	{}

	/**
	 * �f�X�g���N�^
	 */
	~ExclusiveOrTransfer() throw()
	{}

	/**
	 * �]���֐�
	 * @param sourceRect �]������`�͈�
	 * @param destPoint �]���捶��ʒu
	 */
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

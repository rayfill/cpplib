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
 * 配列として扱うのでデストラクタの例外安全性が必須。
 * またイテレーション時の差分サイズがクラス型に依存するので
 * POD structに近い構造にしてください。
 * まぁ、それでもだめならPOD構造体で何とかする方法考えます。
 * @todo 必要なら16bit版も用意するけど・・・いまさらいらないよね?
 */
class RGB32
{
private:
	/**
	 * 画素値の内部表現
	 */
	unsigned char pixel[4];

public:
	enum
	{
		offsetSize = 4 /// 画素あたりのサイズ
	};

	/**
	 * 赤要素の取得
	 * @return 赤要素値
	 */
	unsigned char getRed() const throw()
	{
		return pixel[3];
	}

	/**
	 * 緑要素の取得
	 * @return 緑要素値
	 */
	unsigned char getGreen() const throw()
	{
		return pixel[2];
	}

	/**
	 * 青要素の取得
	 * @return 青要素値
	 */
	unsigned char getBlue() const throw()
	{
		return pixel[1];
	}

	/**
	 * 色構造体の取得
	 * @return 色構造体
	 * これを使うことで型違い同士での色コピーを実現してます。
	 * インライン展開及び最適化されると複数回のコピー処理が折りたたまれて
	 * 受け取り側画素 = 元画素;
	 * の形にまで最適化されることで速度向上が図れます。
	 * これ以上にするとなるとDMA直接操作とか型ごとで特別化した
	 * Blitterファンクタで拡張命令つかったりとかで何とかするしか・・・
	 */
	Color getColor() const throw()
	{
		return Color(getRed(),
					 getGreen(),
					 getBlue());
	}

	/**
	 * 赤要素の設定
	 * @param r 赤要素
	 */
	void setRed(const unsigned char r) throw()
	{
		pixel[3] = r;
	}

	/**
	 * 緑要素の設定
	 * @param g 緑要素
	 */
	void setGreen(const unsigned char g) throw()
	{
		pixel[2] = g;
	}

	/**
	 * 青要素の設定
	 * @param b 青要素
	 */
	void setBlue(const unsigned char b) throw()
	{
		pixel[1] = b;
	}

	/**
	 * 色の設定
	 * @param r 赤要素
	 * @param g 緑要素
	 * @param b 青要素
	 */
	void setColor(const unsigned char r,
				  const unsigned char g,
				  const unsigned char b) throw()
	{
		*reinterpret_cast<DWORD*>(pixel) =
			(r << 24) | (g << 16) | (b << 8);
	}

	/**
	 * 色の設定
	 * @param color 色を表す構造体
	 */
	void setColor(const Color& color) throw()
	{
		*reinterpret_cast<DWORD*>(pixel) =
			(color.r << 24) | (color.g << 16) | (color.b << 8);
	}

	/**
	 * 等値比較
	 * 色が等しければtrue
	 */
	bool operator==(const RGB32& dist) const throw()
	{
		return
			*reinterpret_cast<const DWORD*>(this->pixel) ==
			*reinterpret_cast<const DWORD*>(dist.pixel);
	}

	/**
	 * DIB表現の場合にビットフィールドが必要か
	 * @return 必要なので常にtrue
	 */
	static bool isBitFieldFormat() throw()
	{
		return true;
	}

	/**
	 * 赤要素を現すビットフィールドマスク値
	 * @return 赤要素マスク
	 */
	static DWORD getRedBitField() throw()
	{
		return 0xff000000;
	}

	/**
	 * 緑要素を現すビットフィールドマスク値
	 * @return 緑要素マスク
	 */
	static DWORD getGreenBitField() throw()
	{
		return 0xff0000;
	}

	/**
	 * 青要素を現すビットフィールドマスク値
	 * @return 青要素マスク
	 */
	static DWORD getBlueBitField() throw()
	{
		return 0xff00;
	}
};


/**
 * PixelFormatBaseのR(8bit), G(8bit), B(8bit)の実装
 * メソッド説明はRGB32の方を参照
 * @see RGB32
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
 * ピクセルフォーマットクラス
 * 画素の抽象化
 * 要POD構造。仮想関数とかは禁止です。
 * @param PixelFormatBase 画素クラス型
 */
template <typename PixelFormatBase>
class PixelFormat : public PixelFormatBase
{
public:
	typedef PixelFormatBase BaseType;
	typedef PixelFormatBase* iterator;
	typedef const PixelFormatBase* const_iterator;

	/**
	 * コンストラクタ
	 */
	PixelFormat() throw()
	{
	}
	
	/**
	 * 代入演算子
	 * @param コピー元ピクセルフォーマット
	 * @return 代入後の自身への参照
	 */
	PixelFormat& operator=(const PixelFormat& src) throw()
	{
		setColor(src.getColor());
		return *this;
	}

	/**
	 * 比較演算子
	 * @return 色が等しければtrue
	 */
	bool operator==(const PixelFormat<PixelFormatBase>& dist) const throw()
	{
		return PixelFormatBase::operator==(dist);
	}
};


/**
 * デバイス独立ビットマップクラス
 * @param PixelFormatClass ピクセルフォーマットクラス
 * @param DPISize 画面の解像度 
 * (まぁ、ほとんど意味がないのでデフォルトのままでOKでしょう)
 * @todo 画素行の末端での4バイトアラインについてちゃんと
 * 対処してないのでその対応が必要
 */
template <typename PixelFormatClass, size_t DPISize = 96>
class DIBitmap
{
private:
	/**
	 * ビットマップインフォヘッダ構造体
	 */
	BITMAPINFOHEADER bitmapInfo;

	/**
	 * 16/32ビットDIB用マスクテーブル
	 */
	DWORD bitField[3];

	enum
	{
		red = 0,
		green = 1,
		blue = 2
	};

	/**
	 * ビットマップインフォ構造体へのポインタ
	 */
	BITMAPINFO* info;

	/**
	 * DIBSectionハンドル
	 */
	HBITMAP bitmapResource;

	/**
	 * 画素へのポインタ
	 */
	PixelFormatClass* pixelPointer;

	/**
	 * DIBがトップダウンかボトムアップかをあらわすフラグ
	 * trueでトップダウン
	 */
	bool isTopDown;

	/**
	 * 画素の終端
	 * @todo first = getPointer(), last = getPointer() + getPointerLength();
	 * とかやられると危ないことがあるのでメソッドなくすかも。
	 * blitter経由なら行コピーを高さ分だけ繰り返すので大丈夫なんだが・・・
	 */
	const size_t getPointerLength() const throw()
	{
		return getLineLengthForByte() * this->getHeight();
	}

	/**
	 * 画素の行終端
	 */
	const size_t getLineLengthForByte() const throw()
	{
		return bitmapInfo.biWidth *
			PixelFormatClass::offsetSize + 3 & 0xfffffffc;
	}

	/**
	 * ビットマップインフォ構造体の開放
	 */
	void bitmapInfoClose() throw()
	{
		free(info);
		info = NULL;
	}

	/**
	 * ビットマップインフォ構造体の作成
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
	 * DIBハンドルの開放
	 */
	void closeBitmap()
	{
		assert(bitmapResource != 0);
		assert(pixelPointer != 0);

		::DeleteObject(bitmapResource);
		pixelPointer = 0;
	}

	/**
	 * 画素の値がRGB値を表す場合の設定
	 */
	void setColorPixelFormat()
	{
		bitmapInfo.biBitCount = 8 * PixelFormatClass::offsetSize;
		bitmapInfo.biCompression = BI_RGB;
	}

	/**
	 * 画素の値がマスク値によるRGB値を表す場合の設定
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
	 * コンストラクタ
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
	 * コピーコンストラクタ
	 * @param source 元のDIBitmapオブジェクト
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
	 * デストラクタ
	 */
	virtual ~DIBitmap() throw()
	{
		close();
	}

	/**
	 * 管理リソースの開放
	 */
	void close()
	{
		bitmapInfoClose();
		if (bitmapResource != NULL)
			closeBitmap();
	}

	/**
	 * 一行の画素数を取得する
	 * @return 一行の画素数
	 */
	const size_t getWidth() const
	{
		return bitmapInfo.biWidth;
	}
	
	/**
	 * 一行の画素数を設定する
	 * @param width 一行の画素数
	 */
	void setWidth(size_t width)
	{
		assert((width % 4) == 0);
		bitmapInfo.biWidth = static_cast<LONG>(width);
	}

	/**
	 * 行の高さを取得する
	 * @return DIBの行数
	 */
	const size_t getHeight() const
	{
		if (isTopDown)
			return bitmapInfo.biHeight * -1;

		return bitmapInfo.biHeight;
	}

	/**
	 * トップダウンDIBにする
	 */
	void setTopDown() throw()
	{
		isTopDown = true;
		if (bitmapInfo.biHeight > 0)
			bitmapInfo.biHeight = -bitmapInfo.biHeight;
	}

	/**
	 * ボトムアップDIBにする
	 */
	void setBottomUp() throw()
	{
		isTopDown = false;
		if (bitmapInfo.biHeight < 0)
			bitmapInfo.biHeight = -bitmapInfo.biHeight;
	}

	/**
	 * 高さの設定
	 * @param height 高さの値
	 */
	void setHeight(size_t height)
	{
		if (isTopDown)
			bitmapInfo.biHeight = static_cast<LONG>(height) * -1;
		else
			bitmapInfo.biHeight = static_cast<LONG>(height);
	}

	/**
	 * DIBSecitonの作成
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
	 * 先頭画素へのイテレータの取得
	 * @return 先頭画素のイテレータ
	 */
	iterator begin() const
	{
		return pixelPointer;
	}

	/**
	 * 終端画素へのイテレータの取得
	 * @return 終端画素のイテレータ
	 */
	iterator end() const
	{
		return pixelPointer + (getWidth() * getHeight());
	}

	/**
	 * 指定位置の画素イテレータの取得
	 * @return 指定位置画素のイテレータ
	 * @param x 画像のx座標
	 * @param y 画像のy座標
	 */
	iterator getPixel(size_t x, size_t y) throw()
	{
		return pixelPointer + (x + y * getWidth());
	}

	/**
	 * 指定位置の画素イテレータの取得
	 * @param x 画像のx座標
	 * @param y 画像のy座標
	 * @return 指定位置画素のイテレータ
	 */
	const_iterator getPixel(size_t x, size_t y) const throw()
	{
		return pixelPointer + (x + y * getWidth());
	}

	/**
	 * ビットマップインフォ構造体の取得
	 * @return ビットマップ構造体へのポインタ
	 */
	BITMAPINFO* getBitmapInfo()
	{
		assert(info);
		return info;
	}

	/**
	 * DIBのハンドルの取得
	 * @return DIBのビットマップハンドル
	 */
	HBITMAP getBitmapHandle() const throw()
	{
		return this->bitmapResource;
	}

	/**
	 * ファイルからDIBをロードし、DIBitmapを作成
	 * @param filename ビットマップリソースへのフルパス
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
	 * 内部表現ビットマップの保存
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
 * 矩形ビットマップ転送ファンクタ
 * @param sourceType 転送元のビットマップイテレータ型
 * @param destinationType 転送先のビットマップイテレータ型
 * 同型の場合、C++のインライン展開と最適化により高速なコピーができます。
 * 型が違う場合でも正しいコピーはできますが、速度は落ちます。
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
 * 透過色付ビットマップ転送
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
 * 排他的論理和転送
 */
template <class sourceType, class destinationType>
class ExclusiveOrTransfer
{
private:
	typedef sourceType SourceType;
	typedef destinationType DestinationType;

	/**
	 * 転送元
	 */
	const SourceType& source;

	/**
	 * 転送先
	 */
	DestinationType& destination;
	
	/**
	 * マスクカラー
	 */
	const Color exclusiveColor;

	/**
	 * 一列分の転送
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
	 * コンストラクタ
	 * @param src 転送元
	 * @param dest 転送先
	 * @param color マスクカラー
	 */
	ExclusiveOrTransfer(const sourceType& src,
						 destinationType& dest,
						 Color color) throw()
		: source(src), destination(dest), exclusiveColor(color)
	{}

	/**
	 * デストラクタ
	 */
	~ExclusiveOrTransfer() throw()
	{}

	/**
	 * 転送関数
	 * @param sourceRect 転送元矩形範囲
	 * @param destPoint 転送先左上位置
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

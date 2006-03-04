#ifndef DEVICECONTEXT_HPP_
#define DEVICECONTEXT_HPP_
#include <windows.h>
#include <stdexcept>
#include <math/Geometry.hpp>

/**
 * Win32 デバイスコンテキストオブジェクトクラス
 * @param 削除方法がいまいちなので遅延関数呼び出しとか使って
 * 確保時に開放方法設定できるようにしたほうがいいかも
 * そうすればサブクラスでもいろいろできるし。
 */
class DeviceContext
{
private:
	/**
	 * デバイスコンテキストのソースとなるHWNDハンドル
	 */
	HWND deviceSource;

	/**
	 * 初期設定されているstock Bitmapハンドル
	 */
	HBITMAP stockBitmap;

	/**
	 * 初期設定されているstock brushハンドル
	 */
	HBRUSH stockBrush;

	/**
	 * 初期設定されているstock Fontハンドル
	 */
	HFONT stockFont;

	/**
	 * 初期設定されているstock Penハンドル
	 */
	HPEN stockPen;

	/**
	 * 初期設定されているstock Regionハンドル
	 */
	HRGN stockRegion;

	/**
	 * Stockオブジェクトの再設定
	 * デストラクション時に使う
	 */
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

	/**
	 * デバイスコンテキストの開放
	 */
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
	/**
	 * デバイスコンテキストハンドル
	 */
	HDC deviceContext;

public:
	/**
	 * コンストラクタ
	 * @param source デバイスコンテキスト取得元になるHWNDハンドル
	 * @exception std::invalid_argument sourceが不正なウィンドウハンドルの場合
	 */
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

	/**
	 * コンストラクタ
	 * 互換性を持ったデバイスコンテキストを作成する
	 * @param source 元になるデバイスコンテキストハンドル
	 * @exception source が不正なデバイスコンテキストハンドルの場合
	 */
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

	/**
	 * デストラクタ
	 * リソースは自動で開放される
	 */
	virtual ~DeviceContext()
	{
		restoreStockObjects();
		release();
	}

	/**
	 * デバイスコンテキストハンドルの割り当て
	 * @param deviceContext_ 割り当てるデバイスコンテキストハンドル
	 * @todo つかんだままrelease()されると変な動きするかも・・・
	 */
	void attach(HDC deviceContext_)
	{
		restoreStockObjects();
		release();

		deviceContext = deviceContext_;
	}

	/**
	 * 割り当てられているデバイスコンテキストの開放
	 * バインドされているGDIオブジェクトはそのままなので開放時は注意
	 * @return 割り当てられていたデバイスコンテキストのハンドル
	 */
	HDC detach()
	{
		HDC result = deviceContext;
		deviceContext = NULL;

		return result;
	}

	/**
	 * デバイスコンテキストハンドルの取得。主にWin32APIへのブリッジ用
	 * @return デバイスコンテキストハンドル
	 */
	HDC getDeviceContext() const throw()
	{
		return deviceContext;
	}

	/**
	 * 画像のブロック転送
	 * @param dest 転送先のデバイスコンテキストオブジェクト
	 * @param sourcePosition 転送元の矩形範囲
	 * @param destPosition 転送先の矩形範囲
	 */
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

	/**
	 * ビットマップハンドルのバインド
	 * @param bitmap バインドするビットマップハンドル
	 */
	void setBitmap(HBITMAP bitmap) throw()
	{
		if (stockBitmap == NULL)
			stockBitmap =
				static_cast<HBITMAP>(SelectObject(deviceContext, bitmap));
		else
			SelectObject(deviceContext, bitmap);
	}

	/**
	 * ブラシハンドルのバインド
	 * @param brush バインドするブラシハンドル
	 */
	void setBrush(HBRUSH brush) throw()
	{
		if (stockBrush == NULL)
			stockBrush =
				static_cast<HBRUSH>(SelectObject(deviceContext, brush));
		else
			SelectObject(deviceContext, brush);
	}

	/**
	 * フォントハンドルのバインド
	 * @param font バインドするフォントハンドル
	 */
	void setFont(HFONT font) throw()
	{
		if (stockFont == NULL)
			stockFont =
				static_cast<HFONT>(SelectObject(deviceContext, font));
		else
			SelectObject(deviceContext, font);
	}

	/**
	 * ペンハンドルのバインド
	 * @param pen バインドするペンハンドル
	 */
	void setPen(HPEN pen) throw()
	{
		if (stockPen == NULL)
			stockPen =
				static_cast<HPEN>(SelectObject(deviceContext, pen));
		else
			SelectObject(deviceContext, pen);
	}

	/**
	 * リージョンハンドルのバインド
	 * @param pen バインドするリージョンハンドル
	 */
	void setRegion(HRGN region) throw()
	{
		if (stockRegion == NULL)
			stockRegion =
				static_cast<HRGN>(SelectObject(deviceContext, region));
		else
			SelectObject(deviceContext, region);
	}

};

/**
 * WM_PAINT描画用のデバイスコンテキストオブジェクト
 */
class PaintDeviceContext : public DeviceContext
{
private:
	/**
	 * スーパークラスのバックアップ用
	 * @todo protectedな未初期化コンストラクタ用意したほうがいいかも
	 */
	HDC backupDeviceContext;

	/**
	 * 描画対象のウィンドウのハンドル
	 */
	HWND targetWindow;

	/**
	 * PAINTSTRUCT構造体のコピー
	 * @see PAINTSTRUCT
	 */
	PAINTSTRUCT paintStruct;

	PaintDeviceContext();
	PaintDeviceContext(const PaintDeviceContext&);
	PaintDeviceContext& operator=(const PaintDeviceContext&);
public:

	/**
	 * コンストラクタ
	 * @param target 描画対象のウィンドウハンドル
	 */
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

	/**
	 * デストラクタ
	 */
	~PaintDeviceContext() throw()
	{
		deviceContext = backupDeviceContext;
		::EndPaint(targetWindow, &paintStruct);
	}

	/**
	 * 無効領域の取得
	 * @return 無効領域をあらわす矩形オブジェクト
	 * @see geometry::Rectangle
	 */
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

#ifndef DEVICECONTEXT_HPP_
#define DEVICECONTEXT_HPP_
#include <windows.h>
#include <stdexcept>
#include <math/Geometry.hpp>

/**
 * Win32 �f�o�C�X�R���e�L�X�g�I�u�W�F�N�g�N���X
 * @param �폜���@�����܂����Ȃ̂Œx���֐��Ăяo���Ƃ��g����
 * �m�ێ��ɊJ�����@�ݒ�ł���悤�ɂ����ق�����������
 * ��������΃T�u�N���X�ł����낢��ł��邵�B
 */
class DeviceContext
{
private:
	/**
	 * �f�o�C�X�R���e�L�X�g�̃\�[�X�ƂȂ�HWND�n���h��
	 */
	HWND deviceSource;

	/**
	 * �����ݒ肳��Ă���stock Bitmap�n���h��
	 */
	HBITMAP stockBitmap;

	/**
	 * �����ݒ肳��Ă���stock brush�n���h��
	 */
	HBRUSH stockBrush;

	/**
	 * �����ݒ肳��Ă���stock Font�n���h��
	 */
	HFONT stockFont;

	/**
	 * �����ݒ肳��Ă���stock Pen�n���h��
	 */
	HPEN stockPen;

	/**
	 * �����ݒ肳��Ă���stock Region�n���h��
	 */
	HRGN stockRegion;

	/**
	 * Stock�I�u�W�F�N�g�̍Đݒ�
	 * �f�X�g���N�V�������Ɏg��
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
	 * �f�o�C�X�R���e�L�X�g�̊J��
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
	 * �f�o�C�X�R���e�L�X�g�n���h��
	 */
	HDC deviceContext;

public:
	/**
	 * �R���X�g���N�^
	 * @param source �f�o�C�X�R���e�L�X�g�擾���ɂȂ�HWND�n���h��
	 * @exception std::invalid_argument source���s���ȃE�B���h�E�n���h���̏ꍇ
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
	 * �R���X�g���N�^
	 * �݊������������f�o�C�X�R���e�L�X�g���쐬����
	 * @param source ���ɂȂ�f�o�C�X�R���e�L�X�g�n���h��
	 * @exception source ���s���ȃf�o�C�X�R���e�L�X�g�n���h���̏ꍇ
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
	 * �f�X�g���N�^
	 * ���\�[�X�͎����ŊJ�������
	 */
	virtual ~DeviceContext()
	{
		restoreStockObjects();
		release();
	}

	/**
	 * �f�o�C�X�R���e�L�X�g�n���h���̊��蓖��
	 * @param deviceContext_ ���蓖�Ă�f�o�C�X�R���e�L�X�g�n���h��
	 * @todo ���񂾂܂�release()�����ƕςȓ������邩���E�E�E
	 */
	void attach(HDC deviceContext_)
	{
		restoreStockObjects();
		release();

		deviceContext = deviceContext_;
	}

	/**
	 * ���蓖�Ă��Ă���f�o�C�X�R���e�L�X�g�̊J��
	 * �o�C���h����Ă���GDI�I�u�W�F�N�g�͂��̂܂܂Ȃ̂ŊJ�����͒���
	 * @return ���蓖�Ă��Ă����f�o�C�X�R���e�L�X�g�̃n���h��
	 */
	HDC detach()
	{
		HDC result = deviceContext;
		deviceContext = NULL;

		return result;
	}

	/**
	 * �f�o�C�X�R���e�L�X�g�n���h���̎擾�B���Win32API�ւ̃u���b�W�p
	 * @return �f�o�C�X�R���e�L�X�g�n���h��
	 */
	HDC getDeviceContext() const throw()
	{
		return deviceContext;
	}

	/**
	 * �摜�̃u���b�N�]��
	 * @param dest �]����̃f�o�C�X�R���e�L�X�g�I�u�W�F�N�g
	 * @param sourcePosition �]�����̋�`�͈�
	 * @param destPosition �]����̋�`�͈�
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
	 * �r�b�g�}�b�v�n���h���̃o�C���h
	 * @param bitmap �o�C���h����r�b�g�}�b�v�n���h��
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
	 * �u���V�n���h���̃o�C���h
	 * @param brush �o�C���h����u���V�n���h��
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
	 * �t�H���g�n���h���̃o�C���h
	 * @param font �o�C���h����t�H���g�n���h��
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
	 * �y���n���h���̃o�C���h
	 * @param pen �o�C���h����y���n���h��
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
	 * ���[�W�����n���h���̃o�C���h
	 * @param pen �o�C���h���郊�[�W�����n���h��
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
 * WM_PAINT�`��p�̃f�o�C�X�R���e�L�X�g�I�u�W�F�N�g
 */
class PaintDeviceContext : public DeviceContext
{
private:
	/**
	 * �X�[�p�[�N���X�̃o�b�N�A�b�v�p
	 * @todo protected�Ȗ��������R���X�g���N�^�p�ӂ����ق�����������
	 */
	HDC backupDeviceContext;

	/**
	 * �`��Ώۂ̃E�B���h�E�̃n���h��
	 */
	HWND targetWindow;

	/**
	 * PAINTSTRUCT�\���̂̃R�s�[
	 * @see PAINTSTRUCT
	 */
	PAINTSTRUCT paintStruct;

	PaintDeviceContext();
	PaintDeviceContext(const PaintDeviceContext&);
	PaintDeviceContext& operator=(const PaintDeviceContext&);
public:

	/**
	 * �R���X�g���N�^
	 * @param target �`��Ώۂ̃E�B���h�E�n���h��
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
	 * �f�X�g���N�^
	 */
	~PaintDeviceContext() throw()
	{
		deviceContext = backupDeviceContext;
		::EndPaint(targetWindow, &paintStruct);
	}

	/**
	 * �����̈�̎擾
	 * @return �����̈������킷��`�I�u�W�F�N�g
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

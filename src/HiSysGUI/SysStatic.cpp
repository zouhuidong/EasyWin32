#include <HiEasyX/HiSysGUI/SysStatic.h>
#include <HiEasyX/HiMiscUtils.h>
#include <HiEasyX/HiGraphicsUtils.h>
#include <tchar.h>

namespace HiEasyX
{
	void HiEasyX::SysStatic::RealCreate(HWND hParent)
	{
		m_type = SCT_Static;
		m_hWnd = CreateControl(
			hParent,
			HXStr("Static"),
			HXStr(""),
			WS_CHILD | WS_VISIBLE
		);

	}

	SysStatic::SysStatic()
	{
	}


	HiEasyX::SysStatic::SysStatic(HWND hParent, RECT rct, LPCTSTR strText)
	{
		Create(hParent, rct, strText);
	}
	HiEasyX::SysStatic::SysStatic(HWND hParent, int x, int y, int w, int h, LPCTSTR strText)
	{
		Create(hParent, x, y, w, h, strText);
	}


	void SysStatic::Center(bool center)
	{
		long style = GetWindowLong(GetHandle(), GWL_STYLE);
		if (center)
			style |= SS_CENTER;
		else
			style &= ~SS_CENTER;
		SetWindowLongPtr(GetHandle(), GWL_STYLE, style);
	}

	void SysStatic::Image(bool enable, IMAGE* img)
	{
		long style = GetWindowLong(GetHandle(), GWL_STYLE);
		if (enable)
			style |= SS_BITMAP;
		else
			style &= ~SS_BITMAP;
		SetWindowLongPtr(GetHandle(), GWL_STYLE, style);
		if (enable)
		{
			HBITMAP hBitmap = Image2Bitmap(img, false);
			HBITMAP hBitmap2 = (HBITMAP)SendMessage(GetHandle(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
			DeleteObject(hBitmap2);
		}
	}
}

#include <HiEasyX/HiSysGUI/SysButton.h>
#include <HiEasyX/HiMiscUtils.h>
#include <HiEasyX/HiGraphicsUtils.h>
#include <tchar.h>

namespace HiEasyX
{
	void SysButton::RealCreate(HWND hParent)
	{
		m_type = SCT_Button;
		m_hWnd = CreateControl(
			hParent,
			HXStr("Button"),
			HXStr(""),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON
			);
	}

	SysButton::SysButton()
	{
	}


	SysButton::SysButton(HWND hParent, RECT rct, LPCTSTR strText)
	{
		Create(hParent, rct, strText);
	}



	SysButton::SysButton(HWND hParent, int x, int y, int w, int h, LPCTSTR strText)
	{
		Create(hParent, x, y, w, h, strText);
	}


	LRESULT SysButton::UpdateMessage(UINT msg, WPARAM wParam, LPARAM lParam, bool& bRet)
	{
		if (msg == WM_COMMAND)
		{
			if (LOWORD(wParam) == GetID())
			{
				m_nClickCount++;
				if (m_pFunc)
					m_pFunc();
			}
		}

		bRet = false;
		return 0;
	}

	void SysButton::RegisterMessage(void(*pFunc)())
	{
		m_pFunc = pFunc;
	}

	void SysButton::Image(bool enable, IMAGE* img, bool reserve_text)
	{
		long style = GetWindowLong(GetHandle(), GWL_STYLE);
		if (enable)
			style |= BS_BITMAP;
		else
			style &= ~BS_BITMAP;
		if (!enable || (enable && !reserve_text))
		{
			SetWindowLongPtr(GetHandle(), GWL_STYLE, style);
		}
		if (enable)
		{
			HBITMAP hBitmap = Image2Bitmap(img, false);
			SendMessage(GetHandle(), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
			DeleteObject(hBitmap);
		}
	}

	int SysButton::GetClickCount()
	{
		int c = m_nClickCount;
		m_nClickCount = 0;
		return c;
	}

	bool SysButton::IsClicked()
	{
		return GetClickCount();
	}
}

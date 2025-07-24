#include <HiEasyX/HiSysGUI/SysGroupBox.h>
#include <tchar.h>

namespace HiEasyX
{
	void SysGroupBox::RealCreate(HWND hParent)
	{
		m_type = SCT_GroupBox;
		m_hWnd = CreateControl(
			hParent,
			HXStr("Button"),
			HXStr(""),
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX
		);

		HDC hdc = GetDC(m_hWnd);
		SetBkColor(hdc, RGB(255, 0, 0));
		ReleaseDC(m_hWnd, hdc);
	}

	SysGroupBox::SysGroupBox()
	{
	}


	SysGroupBox::SysGroupBox(HWND hParent, RECT rct, HXString strText)
	{
		Create(hParent, rct, strText);
	}
	SysGroupBox::SysGroupBox(HWND hParent, int x, int y, int w, int h, HXString strText)
	{
		Create(hParent, x, y, w, h, strText);
	}

}

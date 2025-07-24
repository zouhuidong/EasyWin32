#include <HiEasyX/HiSysGUI/SysControlBase.h>

#include <HiEasyX/HiWindow.h>

namespace HiEasyX
{
	void SysControlBase::Destory()
	{
		if (m_hFont)
		{
			DeleteFont(m_hFont);
			m_hFont = nullptr;
		}
		SendMessage(m_hParent, WM_SYSCTRL_DELETE, (WPARAM)this, 0);
	}

	HWND SysControlBase::CreateControl(HWND hParent, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle)
	{
		m_hParent = hParent;
		m_nID = AllocID();

		CREATESTRUCT c;
		c.lpCreateParams = 0;
		c.hInstance = 0;
		c.hMenu = (HMENU)(long long)GetID();
		c.hwndParent = hParent;
		c.cy = GetHeight();
		c.cx = GetWidth();
		c.y = GetY();
		c.x = GetX();
		c.style = dwStyle;
		c.lpszName = lpszWindowName;
		c.lpszClass = lpszClassName;
		c.dwExStyle = 0;

		return (HWND)SendMessage(hParent, WM_SYSCTRL_CREATE, (WPARAM)this, (LPARAM)&c);
	}

	SysControlBase::SysControlBase()
	{
	}

	SysControlBase::~SysControlBase()
	{
		Destory();
	}

	void SysControlBase::UpdateRect(RECT rctOld)
	{
		SetWindowPos(GetHandle(), 0, GetX(), GetY(), GetWidth(), GetHeight(), 0);
	}

	LRESULT SysControlBase::UpdateMessage(UINT msg, WPARAM wParam, LPARAM lParam, bool& bRet)
	{
		return LRESULT();
	}


	HWND SysControlBase::Create(HWND hParent, RECT rct, HXString strText)
	{
		if (!GetHandle())
		{
			SetRect(rct);
			RealCreate(hParent);
			SetText(strText);
		}
		return GetHandle();
	}
	HWND SysControlBase::Create(HWND hParent, int x, int y, int w, int h, HXString strText)
	{
		if (!GetHandle())
		{
			SetRect(x, y, w, h);
			RealCreate(hParent);
			SetText(strText);
		}
		return GetHandle();
	}


	void SysControlBase::Remove()
	{
		Destory();
		SendMessage(m_hWnd, WM_CLOSE, 0, 0);
	}

	bool SysControlBase::IsEnable()
	{
		return IsWindowEnabled(GetHandle());
	}

	void SysControlBase::Enable(bool enable)
	{
		EnableWindow(GetHandle(), enable);
	}

	bool SysControlBase::IsVisible()
	{
		return IsWindowVisible(GetHandle());
	}

	void SysControlBase::Show(bool show)
	{
		ShowWindow(GetHandle(), show ? SW_SHOW : SW_HIDE);
	}

	bool SysControlBase::IsFocused()
	{
		DWORD SelfThreadId = GetCurrentThreadId();						// ��ȡ�����߳� ID
		DWORD ForeThreadId = GetWindowThreadProcessId(m_hParent, NULL);	// ���ݴ��ھ����ȡ�߳� ID
		AttachThreadInput(ForeThreadId, SelfThreadId, true);			// ���ӵ��߳�
		HWND hWnd = GetFocus();											// ��ȡ�������뽹��Ĵ��ھ��
		AttachThreadInput(ForeThreadId, SelfThreadId, false);			// ȡ�����ӵ��߳�
		return hWnd == GetHandle();
	}

	void SysControlBase::SetFocus(bool focused)
	{
		SendMessage(GetHandle(), focused ? WM_SETFOCUS : WM_KILLFOCUS, 0, 0);
	}

	int SysControlBase::GetTextLength()
	{
		return GetWindowTextLength(GetHandle());;
	}


	HXString SysControlBase::GetText()
	{
		int len = GetTextLength();
		TCHAR* buf = new TCHAR[len + 1];
		ZeroMemory(buf, sizeof(TCHAR) * (len + 1));
		GetWindowText(GetHandle(), buf, len + 1);
		HXString str = buf;
		delete[] buf;
		return str;
	}
	void SysControlBase::SetText(HXString str)
	{
		SetWindowText(GetHandle(), str.c_str());
	}


	HFONT SysControlBase::GetFont()
	{
		return (HFONT)SendMessage(GetHandle(), WM_GETFONT, 0, 0);
	}


	void SysControlBase::SetFont(int h, int w, HXString typeface)
	{
		if (m_hFont)
		{
			DeleteFont(m_hFont);
			m_hFont = nullptr;
		}
		m_hFont = CreateFont(
			h, w,
			0, 0, 0, 0, 0, 0,
			GB2312_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_MODERN,
			typeface.c_str()
			);
		SendMessage(GetHandle(), WM_SETFONT, (WPARAM)m_hFont, 0);
		InvalidateRect(GetHandle(), nullptr, true);
	}


	int SysControlBase::GetID()
	{
		return m_nID;
	}

	int AllocID()
	{
		static int id = 10086;
		return id++;
	}
}

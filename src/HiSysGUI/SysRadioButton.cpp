#include "HiSysGUI/SysRadioButton.h"
#include <tchar.h>

namespace HiEasyX
{
	void SysRadioButton::RealCreate(HWND hParent)
	{
		m_type = SCT_RadioButton;
		m_hWnd = CreateControl(
			hParent,
			HXStr("Button"),
			HXStr(""),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON
		);
	}

	SysRadioButton::SysRadioButton()
	{
	}


	SysRadioButton::SysRadioButton(HWND hParent, RECT rct, HXString strText)
	{
		Create(hParent, rct, strText);
	}
	SysRadioButton::SysRadioButton(HWND hParent, int x, int y, int w, int h, HXString strText)
	{
		Create(hParent, x, y, w, h, strText);
	}


	LRESULT SysRadioButton::UpdateMessage(UINT msg, WPARAM wParam, LPARAM lParam, bool& bRet)
	{
		if (msg == WM_COMMAND)
		{
			// ֻҪ�ǰ���������Ϣ���ж�
			// ����ֻ�ж��Լ�����Ϣ����Ϊͬ���������ѡ��ѡ��ʱ���Լ��ղ�����Ϣ
			if (HIWORD(wParam) == BN_CLICKED)
			{
				bool checked = Button_GetCheck(GetHandle());
				if (m_pFunc && m_bChecked != checked)
					m_pFunc(checked);
				m_bChecked = checked;
			}
		}

		bRet = false;
		return 0;
	}

	void SysRadioButton::RegisterMessage(void(*pFunc)(bool checked))
	{
		m_pFunc = pFunc;
	}

	void SysRadioButton::Check(bool check)
	{
		Button_SetCheck(GetHandle(), check);
		m_bChecked = check;
	}
}

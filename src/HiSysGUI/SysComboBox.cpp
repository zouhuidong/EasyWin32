#include "HiSysGUI/SysComboBox.h"
#include <tchar.h>

namespace HiEasyX
{
	void SysComboBox::RealCreate(HWND hParent)
	{
		m_type = SCT_ComboBox;
		m_hWnd = CreateControl(
			hParent,
			HXStr("ComboBox"),
			HXStr(""),
			m_lBasicStyle
		);

		// CBS_SIMPLE Ĭ�Ͽ��Ա༭
		// ��Ҫ�������룬�����ȡ�༭��ľ��
		if (m_bSimple_No_Edit)
		{
			// ��ȡ���������������Ϊֻ��
			HWND combobox_edit = ChildWindowFromPoint(GetHandle(), { 3,3 });
			Edit_SetReadOnly(combobox_edit, true);
		}
	}

	SysComboBox::SysComboBox()
	{
	}


	SysComboBox::SysComboBox(HWND hParent, RECT rct, HXString strText)
	{
		Create(hParent, rct, strText);
	}
	SysComboBox::SysComboBox(HWND hParent, int x, int y, int w, int h, HXString strText)
	{
		Create(hParent, x, y, w, h, strText);
	}


	void SysComboBox::PreSetStyle(PreStyle pre_style)
	{
		if (pre_style.always_show_list)
		{
			m_lBasicStyle |= CBS_SIMPLE;
			m_lBasicStyle &= ~CBS_DROPDOWN;
			if (!pre_style.editable)
				m_bSimple_No_Edit = true;
		}
		else
		{
			if (pre_style.editable)
			{
				m_lBasicStyle |= CBS_DROPDOWN;
			}
			else
			{
				m_lBasicStyle |= CBS_DROPDOWNLIST;
			}
		}

		if (pre_style.sort)
			m_lBasicStyle |= CBS_SORT;
		else
			m_lBasicStyle &= ~CBS_SORT;
	}

	LRESULT SysComboBox::UpdateMessage(UINT msg, WPARAM wParam, LPARAM lParam, bool& bRet)
	{
		if (msg == WM_COMMAND)
		{
			if (LOWORD(wParam) == GetID())
			{
				switch (HIWORD(wParam))
				{
				case CBN_SELCHANGE:
					m_nSel = ComboBox_GetCurSel(GetHandle());
					m_bSelChanged = true;
					if (m_pFuncSel)
					{
						int len = ComboBox_GetLBTextLen(GetHandle(), m_nSel);
						TCHAR* buf = new TCHAR[len + 1];
						ZeroMemory(buf, (len + 1) * sizeof(TCHAR));
						ComboBox_GetLBText(GetHandle(), m_nSel, buf);
						m_pFuncSel(m_nSel, buf);
						delete[] buf;
					}
					break;

				case CBN_EDITUPDATE:
					m_bEdited = true;
					if (m_pFuncEdit)
						m_pFuncEdit(GetText());
					break;
				}
			}
		}

		bRet = false;
		return 0;
	}


	void SysComboBox::RegisterSelMessage(void(*pFunc)(int sel, HXString strSelText))
	{
		m_pFuncSel = pFunc;
	}
	void SysComboBox::RegisterEditMessage(void(*pFunc)(HXString strText))
	{
		m_pFuncEdit = pFunc;
	}

	
	void SysComboBox::SetSel(int sel)
	{
		m_nSel = sel;
		ComboBox_SetCurSel(GetHandle(), sel);
	}


	bool SysComboBox::SelectString(HXString strText)
	{
		return ComboBox_SelectString(GetHandle(), -1, strText.c_str()) != CB_ERR;
	}
	void SysComboBox::AddString(HXString strText)
	{
		ComboBox_AddString(GetHandle(), strText.c_str());
	}
	void SysComboBox::InsertString(int index, HXString strText)
	{
		ComboBox_InsertString(GetHandle(), index, strText.c_str());
	}


	void SysComboBox::DeleteString(int index)
	{
		ComboBox_DeleteString(GetHandle(), index);
	}

	int SysComboBox::GetCount()
	{
		return ComboBox_GetCount(GetHandle());
	}

	void SysComboBox::Clear()
	{
		ComboBox_ResetContent(GetHandle());
	}

	void SysComboBox::ShowDropdown(bool enable)
	{
		ComboBox_ShowDropdown(GetHandle(), enable);
	}

	bool SysComboBox::IsSelChanged()
	{
		bool r = m_bSelChanged;
		m_bSelChanged = false;
		return r;
	}

	bool SysComboBox::IsEdited()
	{
		bool r = m_bEdited;
		m_bEdited = false;
		return r;
	}
}

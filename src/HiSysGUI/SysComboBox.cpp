#include <HiEasyX/HiSysGUI/SysComboBox.h>
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

		// CBS_SIMPLE 默认可以编辑
		// 若要禁用输入，必须获取编辑框的句柄
		if (m_bSimple_No_Edit)
		{
			// 获取输入框句柄，并设置为只读
			HWND combobox_edit = ChildWindowFromPoint(GetHandle(), { 3,3 });
			Edit_SetReadOnly(combobox_edit, true);
		}
	}

	SysComboBox::SysComboBox()
	{
	}


	SysComboBox::SysComboBox(HWND hParent, RECT rct, LPCTSTR strText)
	{
		Create(hParent, rct, strText);
	}
	SysComboBox::SysComboBox(HWND hParent, int x, int y, int w, int h, LPCTSTR strText)
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
					{
						TCHAR* buf = new TCHAR[GetTextLength() + 1];
						memset(buf, 0, (GetTextLength() + 1) * sizeof(TCHAR));
						GetText(buf, GetTextLength() + 1);
						m_pFuncEdit(buf);
						delete[] buf;
					}
					break;
				}
			}
		}

		bRet = false;
		return 0;
	}


	void SysComboBox::RegisterSelMessage(void(*pFunc)(int sel, LPCTSTR strSelText))
	{
		m_pFuncSel = pFunc;
	}
	void SysComboBox::RegisterEditMessage(void(*pFunc)(LPCTSTR strText))
	{
		m_pFuncEdit = pFunc;
	}


	void SysComboBox::SetSel(int sel)
	{
		m_nSel = sel;
		ComboBox_SetCurSel(GetHandle(), sel);
	}


	bool SysComboBox::SelectString(LPCTSTR strText)
	{
		return ComboBox_SelectString(GetHandle(), -1, strText) != CB_ERR;
	}
	void SysComboBox::AddString(LPCTSTR strText)
	{
		ComboBox_AddString(GetHandle(), strText);
	}
	void SysComboBox::InsertString(int index, LPCTSTR strText)
	{
		ComboBox_InsertString(GetHandle(), index, strText);
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

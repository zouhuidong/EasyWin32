/**
 * @file	SysCheckBox.h
 * @brief	HiSysGUI �ؼ���֧����ѡ��
 * @author	huidong
*/

#pragma once

#include <HiEasyX/HiSysGUI/SysControlBase.h>

namespace HiEasyX
{
	/**
	 * @brief ϵͳ��ѡ��ؼ�
	*/
	class SysCheckBox : public SysControlBase
	{
	private:
		bool m_bChecked = false;
		void (*m_pFunc)(bool checked) = nullptr;

	protected:

		void RealCreate(HWND hParent) override;

	public:

		SysCheckBox();


		SysCheckBox(HWND hParent, RECT rct, LPCTSTR strText = _T(""));
		SysCheckBox(HWND hParent, int x, int y, int w, int h, LPCTSTR strText = _T(""));


		LRESULT UpdateMessage(UINT msg, WPARAM wParam, LPARAM lParam, bool& bRet) override;

		/**
		 * @brief ע������Ϣ
		 * @param[in] pFunc ��Ϣ��Ӧ����
		*/
		void RegisterMessage(void (*pFunc)(bool checked));

		/**
		 * @brief ��ȡѡ��״̬
		*/
		bool IsChecked() const { return m_bChecked; }

		void Check(bool check);
	};
}

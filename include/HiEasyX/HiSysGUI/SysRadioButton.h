/**
 * @file	SysRadioButton.h
 * @brief	HiSysGUI �ؼ���֧����ѡ��
 * @author	huidong
*/

#pragma once

#include "SysControlBase.h"

namespace HiEasyX
{
	/**
	 * @brief ϵͳ��ѡ��ؼ�
	*/
	class SysRadioButton : public SysControlBase
	{
	private:
		bool m_bChecked = false;
		void (*m_pFunc)(bool checked) = nullptr;

	protected:

		void RealCreate(HWND hParent) override;

	public:

		SysRadioButton();


		SysRadioButton(HWND hParent, RECT rct, HXString strText = HXStr(""));
		SysRadioButton(HWND hParent, int x, int y, int w, int h, HXString strText = HXStr(""));


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

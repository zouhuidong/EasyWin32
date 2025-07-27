/**
 * @file	SysButton.h
 * @brief	HiSysGUI �ؼ���֧����ť
 * @author	huidong
*/

#pragma once

#include <HiEasyX/HiSysGUI/SysControlBase.h>

#include <graphics.h>

namespace HiEasyX
{
	/**
	 * @brief ϵͳ��ť�ؼ�
	*/
	class SysButton : public SysControlBase
	{
	private:
		int m_nClickCount = 0;
		void (*m_pFunc)() = nullptr;

	protected:

		void RealCreate(HWND hParent) override;

	public:

		SysButton();


		SysButton(HWND hParent, RECT rct, LPCTSTR strText = _T(""));
		SysButton(HWND hParent, int x, int y, int w, int h, LPCTSTR strText = _T(""));


		LRESULT UpdateMessage(UINT msg, WPARAM wParam, LPARAM lParam, bool& bRet) override;

		/**
		 * @brief ע������Ϣ
		 * @param[in] pFunc ��Ϣ������
		*/
		void RegisterMessage(void (*pFunc)());

		/**
		 * @brief ����ͼƬ
		 * @param[in] enable		�Ƿ����ð�ťͼƬ
		 * @param[in] img			ͼƬ
		 * @param[in] reserve_text	�Ƿ�����ť�е�����
		*/
		void Image(bool enable, IMAGE* img = nullptr, bool reserve_text = false);

		/**
		 * @brief ��ȡ�������
		*/
		int GetClickCount();

		/**
		 * @brief <pre>
		 *		�ж��Ƿ�������
		 * 
		 *	��ע��
		 *		����ʹ�� GetClickCount��ʹ�ô˺������ܶ�ʧ���������Ϣ
		 * </pre>
		*/
		bool IsClicked();
	};
}

/**
 * @file	SysControlBase.h
 * @brief	HiSysGUI �ؼ���֧���ؼ�����
 * @author	huidong
*/

#pragma once

#include "../HiContainer.h"
#include "../HiString.h"
#include <windowsx.h>

namespace HiEasyX
{
	/**
	 * @brief ֧�ֵ�ϵͳ�ؼ�����
	*/
	enum SysControlType
	{
		SCT_Unknown,
		SCT_Group,
		SCT_GroupBox,
		SCT_Static,
		SCT_Button,
		SCT_CheckBox,
		SCT_RadioButton,
		SCT_Edit,
		SCT_ComboBox,
	};

	/**
	 * @brief ϵͳ�ؼ�����
	*/
	class SysControlBase : public Container
	{
	private:

		HFONT m_hFont = nullptr;

		/**
		 * @brief �ڲ���������
		*/
		void Destory();

	protected:

		HWND m_hWnd = nullptr;
		HWND m_hParent = nullptr;
		int m_nID = 0;
		SysControlType m_type = SCT_Unknown;

		/**
		 * @brief �����ؼ�
		 * @param[in] hParent			���ؼ� 
		 * @param[in] lpszClassName		��������
		 * @param[in] lpszWindowName	������
		 * @param[in] dwStyle			������ʽ
		 * @return ���ھ��
		*/
		HWND CreateControl(HWND hParent, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle);

		/**
		 * @brief ʵ�ʵ��õĴ����ؼ����������ֿؼ�ʵ�ֲ�ͬ�����ڲ������� CreateControl �����ؼ���
		 * @param[in] hParent ���ؼ����
		*/
		virtual void RealCreate(HWND hParent) = 0;

	public:

		SysControlBase();

		virtual ~SysControlBase();

		void UpdateRect(RECT rctOld) override;

		/**
		 * @brief ������Ϣ���˺��������û�����
		 * @param[in] msg		����Ϣ
		 * @param[in] wParam	����
		 * @param[in] lParam	����
		 * @param[out] bRet ����Ƿ񷵻�ֵ
		 * @return ��������ֵ
		*/
		virtual LRESULT UpdateMessage(UINT msg, WPARAM wParam, LPARAM lParam, bool& bRet);

		/**
		 * @brief <pre>
		 *		�����ؼ����ӿؼ����󴴽��� UI ʵ�壩
		 *
		 *	ע�⣺
		 *		ͬһ���ؼ������ܷ������� UI ʵ��
		 * </pre>
		 * 
		 * @param[in] hParent	�����ھ��
		 * @param[in] rct		�ؼ�����
		 * @param[in] strText	�ؼ��ı�
		 * @return �ؼ����ھ��
		*/

		HWND Create(HWND hParent, RECT rct, LPCTSTR strText = _T(""));
		HWND Create(HWND hParent, int x, int y, int w, int h, LPCTSTR strText = _T(""));


		/**
		 * @brief �Ƴ��ؼ�
		*/
		void Remove();

		HWND GetHandle() const { return m_hWnd; }

		/**
		 * @brief ��ȡ�˿ؼ�����
		*/
		SysControlType GetControlType() const { return m_type; }

		bool IsEnable();

		void Enable(bool enable);

		bool IsVisible();

		void Show(bool show);

		bool IsFocused();

		void SetFocus(bool focused);

		int GetTextLength();


		/**
		 * @param[in] pString		�����ַ����Ļ�����
		 * @param[in] nMaxCount		Ҫ���Ƶ�������������ַ��������� null �ַ�
		*/
		void GetText(LPTSTR pString, int nMaxCount);
		void SetText(LPCTSTR str);


		HFONT GetFont();


		void SetFont(int h, int w = 0, LPCTSTR typeface = _T(""));


		int GetID();

	};

	/**
	 * @brief �Զ�����ؼ� ID
	*/
	int AllocID();
}

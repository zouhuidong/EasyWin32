/**
 * @file	SysControlBase.h
 * @brief	HiSysGUI 控件分支：控件基础
 * @author	huidong
*/

#pragma once

#include <HiEasyX/HiContainer.h>
#include <HiEasyX/HiString.h>
#include <windowsx.h>

namespace HiEasyX
{
	/**
	 * @brief 支持的系统控件类型
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
	 * @brief 系统控件基础
	*/
	class SysControlBase : public Container
	{
	private:

		HFONT m_hFont = nullptr;

		/**
		 * @brief 内部析构函数
		*/
		void Destory();

	protected:

		HWND m_hWnd = nullptr;
		HWND m_hParent = nullptr;
		int m_nID = 0;
		SysControlType m_type = SCT_Unknown;

		/**
		 * @brief 创建控件
		 * @param[in] hParent			父控件 
		 * @param[in] lpszClassName		窗口类名
		 * @param[in] lpszWindowName	窗口名
		 * @param[in] dwStyle			窗口样式
		 * @return 窗口句柄
		*/
		HWND CreateControl(HWND hParent, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle);

		/**
		 * @brief 实际调用的创建控件函数（各种控件实现不同，但内部都调用 CreateControl 创建控件）
		 * @param[in] hParent 父控件句柄
		*/
		virtual void RealCreate(HWND hParent) = 0;

	public:

		SysControlBase();

		virtual ~SysControlBase();

		void UpdateRect(RECT rctOld) override;

		/**
		 * @brief 更新消息，此函数无需用户调用
		 * @param[in] msg		新消息
		 * @param[in] wParam	参数
		 * @param[in] lParam	参数
		 * @param[out] bRet 标记是否返回值
		 * @return 不定返回值
		*/
		virtual LRESULT UpdateMessage(UINT msg, WPARAM wParam, LPARAM lParam, bool& bRet);

		/**
		 * @brief <pre>
		 *		创建控件（从控件对象创建出 UI 实体）
		 *
		 *	注意：
		 *		同一个控件对象不能反复创建 UI 实体
		 * </pre>
		 * 
		 * @param[in] hParent	父窗口句柄
		 * @param[in] rct		控件区域
		 * @param[in] strText	控件文本
		 * @return 控件窗口句柄
		*/

		HWND Create(HWND hParent, RECT rct, LPCTSTR strText = _T(""));
		HWND Create(HWND hParent, int x, int y, int w, int h, LPCTSTR strText = _T(""));


		/**
		 * @brief 移除控件
		*/
		void Remove();

		HWND GetHandle() const { return m_hWnd; }

		/**
		 * @brief 获取此控件类型
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
		 * @param[in] pString		接收字符串的缓冲区
		 * @param[in] nMaxCount		要复制到缓冲区的最大字符数，包括 null 字符
		*/
		void GetText(LPTSTR pString, int nMaxCount);
		void SetText(LPCTSTR str);


		HFONT GetFont();


		void SetFont(int h, int w = 0, LPCTSTR typeface = _T(""));


		int GetID();

	};

	/**
	 * @brief 自动分配控件 ID
	*/
	int AllocID();
}

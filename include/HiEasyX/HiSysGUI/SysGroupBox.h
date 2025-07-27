/**
 * @file	SysGroupBox.h
 * @brief	HiSysGUI �ؼ���֧�������
 * @author	huidong
*/

#pragma once

#include <HiEasyX/HiSysGUI/SysControlBase.h>

namespace HiEasyX
{
	/**
	 * @brief <pre>
	 *		ϵͳ�����ؼ�
	 * 
	 *	ע�⣺
	 *		�����ؼ�ʱ��Ҫ���˿ؼ���Ϊ���ؼ��������޷������ؼ�
	 * </pre>
	 * 
	 * @bug
	 *		����򱳾������� Bug��������ʹ��
	*/
	class SysGroupBox : public SysControlBase
	{
	protected:

		void RealCreate(HWND hParent) override;

	public:

		SysGroupBox();


		SysGroupBox(HWND hParent, RECT rct, LPCTSTR strText = _T(""));
		SysGroupBox(HWND hParent, int x, int y, int w, int h, LPCTSTR strText = _T(""));

	};
}

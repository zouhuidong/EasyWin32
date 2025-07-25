/**
 * @file	SysStatic.h
 * @brief	HiSysGUI �ؼ���֧����̬�ؼ�
 * @author	huidong
*/

#pragma once

#include "SysControlBase.h"

#include <graphics.h>

namespace HiEasyX
{
	/**
	 * @brief ϵͳ��̬�ؼ�
	*/
	class SysStatic : public SysControlBase
	{
	protected:

		void RealCreate(HWND hParent) override;

	public:

		SysStatic();


		SysStatic(HWND hParent, RECT rct, LPCTSTR strText = _T(""));
		SysStatic(HWND hParent, int x, int y, int w, int h, LPCTSTR strText = _T(""));


		/**
		 * @brief �����ı�����
		 * @param[in] center �Ƿ����
		*/
		void Center(bool center);

		/**
		 * @brief ����ͼƬ
		 * @param[in] enable	�Ƿ�����ͼ��
		 * @param[in] img		ͼ��
		*/
		void Image(bool enable, IMAGE* img = nullptr);
	};
}

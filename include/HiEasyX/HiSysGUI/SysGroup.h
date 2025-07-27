/**
 * @file	SysGroup.h
 * @brief	HiSysGUI �ؼ���֧�������־
 * @author	huidong
*/

#pragma once

#include <HiEasyX/HiSysGUI/SysControlBase.h>

namespace HiEasyX
{
	/**
	 * @brief ϵͳ�ؼ������־
	*/
	class SysGroup : public SysControlBase
	{
	protected:

		void RealCreate(HWND hParent) override;

	public:

		SysGroup();

		SysGroup(HWND hParent);

		void Create(HWND hParent);
	};
}

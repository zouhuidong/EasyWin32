#include "HiSysGUI/SysGroup.h"
#include <tchar.h>

namespace HiEasyX
{
	void SysGroup::RealCreate(HWND hParent)
	{
		m_type = SCT_Group;
		m_hWnd = CreateControl(
			hParent,
			HXStr("Button"),
			HXStr(""),
			WS_CHILD | WS_GROUP
		);
	}

	SysGroup::SysGroup()
	{
	}

	SysGroup::SysGroup(HWND hParent)
	{
		Create(hParent);
	}

	void SysGroup::Create(HWND hParent)
	{
		RealCreate(hParent);
	}
}

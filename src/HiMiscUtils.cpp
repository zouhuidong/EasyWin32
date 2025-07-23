#include "HiMiscUtils.h"

#include <iostream>
#include <time.h>

#define RGBA(r, g, b, a)				(COLORREF)( ((b)<<16) | ((g)<<8) | (r) | ((a)<<24) )

ScreenSize GetScreenSize()
{
	int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	return { left,top,w,h };
}

void HpSleep(int ms)
{
	static clock_t oldclock = clock();		// 静态变量，记录上一次 tick
	static int c = CLOCKS_PER_SEC / 1000;

	oldclock += ms * c;						// 更新 tick

	if (clock() > oldclock)					// 如果已经超时，无需延时
		oldclock = clock();
	else
		while (clock() < oldclock)			// 延时
			Sleep(1);						// 释放 CPU 控制权，降低 CPU 占用率
	//			Sleep(0);						// 更高精度、更高 CPU 占用率
}

bool IsInRect(int x, int y, RECT rct)
{
	if (rct.left > rct.right)    std::swap(rct.left, rct.right);
	if (rct.top > rct.bottom)    std::swap(rct.top, rct.bottom);
	return x >= rct.left && x <= rct.right && y >= rct.top && y <= rct.bottom;
}





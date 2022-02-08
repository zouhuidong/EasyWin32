#include "EasyWin32.h"
#include <conio.h>

int main()
{
	EasyWin32::initgraph_win32();			// 初始化窗口

	outtextxy(20, 20, L"Hello EasyWin32");	// 绘制文本

	FLUSH_DRAW();							// EasyWin32 默认使用双缓冲绘图，此处输出绘图缓冲

	// 按任意键继续。注意：必须在此判断是否还存在窗口
	while (!_kbhit() && EasyWin32::isAnyWindow())
	{
		Sleep(10);
	}								

	EasyWin32::closegraph_win32();			// 关闭窗口
	return 0;
}

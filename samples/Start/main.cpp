#include "EasyWin32.h"
#include <conio.h>

int main()
{
	EasyWin32::initgraph_win32();			// 初始化窗口

	// 启动一个绘图任务
	// 由于只创建了一个窗口，所以不需要指定目标绘图窗口
	// 否则就需要调用 BEGIN_TASK_WND() 宏，指定目标绘图窗口
	BEGIN_TASK();

	// 绘制文本
	settextstyle(32, 0, L"Consolas");
	settextcolor(LIGHTGREEN);
	outtextxy(20, 70, L"Hello EasyWin32");
	settextstyle(26, 0, L"system");
	settextcolor(GREEN);
	outtextxy(20, 110, L"Any key to continue");

	// EasyWin32 默认使用双缓冲绘图，此处输出绘图缓冲
	// 注意：一段绘图任务结束，必须以此宏结尾（即 BEGIN_TASK 和 END_TASK 必须连用）
	END_TASK();

	// 不在窗口过程函数的 WM_PAINT 消息内绘图时，必须强制重绘
	// 由于没有自定义窗口过程函数，所以当然也要调用此宏强制重绘
	FLUSH_DRAW();

	while (true)
	{
		// 按任意键关闭窗口
		if (_kbhit())
		{
			EasyWin32::closegraph_win32();	// 关闭窗口
			break;
		}

		// 若窗口被关闭，退出程序
		// 注意：这个判断经常被忽略，判断窗口是否还存在是很必要的
		if (!EasyWin32::isAnyWindow())
		{
			break;
		}

		Sleep(10);
	}

	return 0;
}

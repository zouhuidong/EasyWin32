////////////////////////////////
//
//	EasyWin32 使用示例
//
//	1. 使用顺序代码结构
//	2. 创建了多个绘图窗口
//	3. 展示了鼠标消息的获取
//

#include "EasyWin32.h"
#include <time.h>

int main()
{
	// 设置随机种子
	srand((unsigned)time(NULL));

	// 创建两个绘图窗口
	HWND hWnd1 = EasyWin32::initgraph_win32(640, 480, false, L"字符阵");
	HWND hWnd2 = EasyWin32::initgraph_win32(640, 480, false, L"鼠标操作示例");

	// 设置窗口 1 绘图样式
	EasyWin32::SetWorkingWindow(hWnd1);
	settextstyle(16, 8, _T("Courier"));
	settextcolor(GREEN);

	while (true)
	{
		// 若窗口 1 还存在（未被关闭）
		if (EasyWin32::isAliveWindow(hWnd1))
		{
			// 对窗口 1 绘制
			EasyWin32::SetWorkingWindow(hWnd1);

			// 绘制内容：EasyX 官方示例“字符阵”（简化）
			cleardevice();
			for (int i = 0; i <= 200; i++)
			{
				// 在随机位置显示三个随机字母
				for (int j = 0; j < 3; j++)
				{
					int x = (rand() % 80) * 8;
					int y = (rand() % 20) * 24;
					char c = (rand() % 26) + 65;
					outtextxy(x, y, c);
				}
			}

			// EasyWin32 默认使用双缓冲，此处输出缓冲（请勿使用 FlushBatchDraw 系列函数）
			EasyWin32::FlushDrawing();
			EasyWin32::EnforceRedraw();	// 在顺序代码结构下，需要发送强制重绘消息
		}

		// 窗口 2
		if (EasyWin32::isAliveWindow(hWnd2))
		{
			EasyWin32::SetWorkingWindow(hWnd2);

			// 绘制内容：EasyX 官方示例“鼠标操作”（有改动）
			ExMessage m;
			if (EasyWin32::MouseHit_win32())
			{
				// 获取一条鼠标消息
				m = EasyWin32::GetMouseMsg_win32();
				switch (m.message)
				{
				case WM_MOUSEMOVE:
					// 鼠标移动的时候画红色的小点
					putpixel(m.x, m.y, RED);
					break;

				case WM_LBUTTONDOWN:
					// 如果点左键的同时按下了 Ctrl 键
					if (m.ctrl)
						// 画一个大方块
						rectangle(m.x - 10, m.y - 10, m.x + 10, m.y + 10);
					else
						// 画一个小方块
						rectangle(m.x - 5, m.y - 5, m.x + 5, m.y + 5);
					break;
				}
			}

			EasyWin32::FlushDrawing();
			EasyWin32::EnforceRedraw();
		}

		// 若所有窗口都被关闭，则结束程序
		if (!EasyWin32::isAnyWindow())
		{
			break;
		}

		// 降低 CPU 占用
		if(rand() % 777 == 0)
			Sleep(1);
	}

	return 0;
}

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

	// 定时绘制
	clock_t tRecord = 0;

	while (true)
	{
		// 若窗口 1 还存在（未被关闭）
		if (EasyWin32::isAliveWindow(hWnd1))
		{
			// 一段时间重绘一次
			if (clock() - tRecord >= 100)
			{
				// 设置窗口 1 为目标绘图窗口，并启动一个绘图任务
				BEGIN_TASK_WND(hWnd1);

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

				// EasyWin32 默认使用双缓冲绘图，此处输出绘图缓冲
				// 注意：一段绘图任务结束，必须以此宏结尾（即 BEGIN_TASK_WND 和 END_TASK 必须连用）
				END_TASK();

				// 不在窗口过程函数的 WM_PAINT 消息内绘图时，必须强制重绘
				// 由于没有自定义窗口过程函数，所以当然也要调用此宏强制重绘
				FLUSH_DRAW();
			}
		}

		// 窗口 2
		if (EasyWin32::isAliveWindow(hWnd2))
		{
			BEGIN_TASK_WND(hWnd2);

			// 绘制内容：EasyX 官方示例“鼠标操作”（有改动）
			ExMessage m;
			while (peekmessage(&m, EM_MOUSE))
			{
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

			END_TASK();
			FLUSH_DRAW();
		}

		// 若所有窗口都被关闭，则结束程序
		if (!EasyWin32::isAnyWindow())
		{
			break;
		}

		// 降低 CPU 占用
		Sleep(50);
	}

	return 0;
}

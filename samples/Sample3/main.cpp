///////////////////////////////////////////////////////////
//
//	EasyWin32 使用示例
//
//	1. 顺序代码结构绘图的同时，自定义了窗口过程函数
//	2. 展示了如何处理关闭窗口消息
//	
//	绘图部分源码来自 https://codebus.cn/dudugang/drawing
//

#include "EasyWin32.h"
#include <time.h>

int x;								// X 轴坐标
int y;								// y 轴坐标
int g_multiples = 0;				// 倍数：作用是对弧度角进行改变
float g_PI = (float)3.1415;			// 圆周率
float g_radianAngle;				// 起始角的弧度
int w = 830;						// 窗口宽
int h = 580;						// 窗口高

// 窗口过程函数
bool WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
	case WM_CLOSE:

		// 关闭窗口时提示是否确认关闭
		if (MessageBox(hwnd, L"确定关闭窗口？", L"提示", MB_OKCANCEL | MB_ICONWARNING) == IDOK)
		{
			return true;
		}
		else
		{
			return false;
		}
		break;
	default:	return true;
	}
}

// 实施画单个完整小球的函数
void DrawCell(
	int left_x,						// 左顶点的X轴坐标
	int top_y,						// 左顶点的y轴坐标
	float PI,						// 圆周率
	float radianAngle				// 起始角的弧度
)
{
	// 设置无边框的填充圆的颜色
	setfillcolor(RGB(250, 250, 250));

	// 画无边框的填充圆。
	solidcircle(left_x + 25, top_y + 25, 20);

	// 设置无边框的填充扇形颜色
	setfillcolor(RGB(80, 80, 80));

	// 画无边框的填充扇形
	solidpie(left_x + 5, top_y + 5, left_x + 50 - 5, top_y + 50 - 5, radianAngle, radianAngle + PI);

	// 设置无边填充圆的颜色
	setfillcolor(RGB(158, 2, 251));

	// 画无边填充圆
	solidcircle(left_x + 25, top_y + 25, 15);
}

int main()
{
	// 创建绘图窗口，并指定窗口过程函数
	EasyWin32::initgraph_win32(w, h, false, L"", WndProc);

	// 设置背景色
	setbkcolor(WHITE);

	while (true)
	{
		// 用背景色清空屏幕
		cleardevice();

		// 设置绘图色为背景颜色：因为在确定每一个小球的位置时要画一个有边框的矩形，所以设置如果去掉会显示矩形边框
		setcolor(RGB(185, 230, 0));

		// 设置无边框的填充矩形的颜色
		setfillcolor(RGB(185, 230, 0));

		// 画外层无边框的填充矩形，限制所有小球的范围
		solidrectangle(10, 10, 820, 570);

		// 控制行数
		for (size_t j = 0; j < 11; j++)
		{
			// 控制列数
			for (size_t i = 0; i < 16; i++)
			{
				x = 15 + 50 * i;							// X轴坐标
				y = 15 + 50 * j;							// y轴坐标
				g_radianAngle = 0 + g_multiples * g_PI / 4;	// 起始角的弧度

				i < 15 ? g_multiples++ : g_multiples = g_multiples;

				// 画有边矩形，确定单个小球的位置
				rectangle(x, y, x + 50, y + 50);

				// 画单个小球
				DrawCell(x, y, g_PI, g_radianAngle);
			}
		}

		// EasyWin32 默认使用双缓冲绘图，此处输出绘图缓冲
		FLUSH_DRAW();

		while (true)
		{
			// 窗口大小改变时重绘
			if (EasyWin32::isWindowSizeChanged())
			{
				break;
			}

			// 若窗口被关闭，则结束程序
			if (!EasyWin32::isAnyWindow())
			{
				return 0;
			}

			Sleep(10);
		}
	}
}


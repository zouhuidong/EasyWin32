////////////////////////////////////////////
//
//	EasyWin32 使用示例
//
//	1. 使用 win32 消息派发的代码结构
//	2. 创建了多个绘图窗口
//	3. 展示了 win32 控件与 EasyX 的结合
//

#include "EasyWin32.h"

// 控件 ID 记录
#define IDC_EDIT 100
#define IDC_BTN1 101
#define IDC_BTN2 101

// 编辑框控件句柄
HWND hEdit;

wchar_t str[512] = L"Hello, EasyX.";

// 窗口 1 的过程函数
bool WndProc1(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
	case WM_CREATE:

		// 创建输入框，按钮
		hEdit = CreateWindow(L"edit", L"Edit at here.",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			130, 50, 200, 20,
			hwnd, (HMENU)IDC_EDIT, hInstance, NULL);

		CreateWindow(L"button", L"Click Me!",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			330, 50, 100, 20,
			hwnd, (HMENU)IDC_BTN1, hInstance, NULL);

		EasyWin32::SetWorkingWindow(hwnd);			// 设置绘图窗口为自己
		if (EasyWin32::GetHWnd_win32() == hwnd)		// 判断是否成功获取当前绘图权
		{
			EasyWin32::ReadyToDraw();				// 每次进行绘图操作前都要调用此函数

			// 绘图状态设置
			setbkcolor(RGB(250, 250, 250));
			setbkmode(TRANSPARENT);
			setlinecolor(BLACK);
			setfillcolor(BLUE);
			settextcolor(BLACK);
			settextstyle(32, 0, L"system");

			// EasyWin32 默认使用双缓冲，此处输出缓冲（请勿使用 FlushBatchDraw 系列函数）
			EasyWin32::FlushDrawing();
		}

		break;

	case WM_PAINT:

		EasyWin32::SetWorkingWindow(hwnd);
		if (EasyWin32::GetHWnd_win32() == hwnd)
		{
			EasyWin32::ReadyToDraw();

			cleardevice();
			setlinestyle(0, 5);
			setlinecolor(LIGHTBLUE);
			fillcircle(100, getheight() - 100, 80);
			fillrectangle(getwidth() - 170, getheight() - 170, getwidth() - 30, getheight() - 30);
			outtextxy(130, 150, str);

			EasyWin32::FlushDrawing();
		}

		break;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDC_BTN1:	// 按下按钮

			// 得到输入框文本
			GetWindowText(hEdit, str, 512);

			// 设置绘图窗口为自己
			EasyWin32::SetWorkingWindow(hwnd);
			if (EasyWin32::GetHWnd_win32() == hwnd)
			{
				// 强制重绘
				EasyWin32::EnforceRedraw();
			}

			break;
		}

		break;

	default: return true; break;	// 使用默认方法处理其余消息
	}
	return false;
}

// 窗口 3 的过程函数
bool WndProc3(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
		// 此处创建单选框和复选框的代码来自 https://blog.csdn.net/siyacaodeai/article/details/114039773
	case WM_CREATE:

		// 组
		CreateWindowEx(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"Select Process Mode:",
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			10, 5,
			350, 100,
			hwnd,
			nullptr,
			hInstance, NULL);
		// 单选框
		CreateWindowEx(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"first radio button",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
			20, 25,
			300, 20,
			hwnd,
			nullptr,
			hInstance, NULL);
		CreateWindowEx(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"second radio button",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			20, 45,
			300, 20,
			hwnd,
			nullptr,
			hInstance, NULL);
		// 复选框
		CreateWindowEx(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"third radio button",
			WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_GROUP,
			20, 65,
			300, 20,
			hwnd,
			nullptr,
			hInstance, NULL);
		CreateWindowEx(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"forth radio button",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			20, 120,
			300, 20,
			hwnd,
			nullptr,
			hInstance, NULL);
		break;

	case WM_PAINT:
		EasyWin32::SetWorkingWindow(hwnd);
		if (EasyWin32::GetHWnd_win32() == hwnd)
		{
			EasyWin32::ReadyToDraw();

			// 设置背景色
			setbkcolor(RGB(240, 240, 240));
			cleardevice();

			EasyWin32::FlushDrawing();
		}
		break;

	default: return true; break;
	}
	return false;
}

// 窗口 2 的过程函数
bool WndProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
	case WM_CREATE:

		// 创建一个按钮
		CreateWindow(L"button", L"Settings...",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			20, 20, 90, 40,
			hwnd, (HMENU)IDC_BTN2, hInstance, NULL);

		// 创建一个文本框
		CreateWindow(L"edit", L"大大的输入框\r\n\r\n尽情输入吧~\r\n\r\nEasyWin32 by huidong",
			WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
			20, 80, 400, 200,
			hwnd, (HMENU)IDC_EDIT, hInstance, NULL);

		break;

	case WM_PAINT:
		EasyWin32::SetWorkingWindow(hwnd);
		if (EasyWin32::GetHWnd_win32() == hwnd)
		{
			EasyWin32::ReadyToDraw();

			// 设置背景色
			setbkcolor(RGB(240, 240, 240));
			cleardevice();

			// 在底部显示 "EasyX"
			settextcolor(BLUE);
			setbkmode(TRANSPARENT);
			settextstyle(72, 0, L"system");
			LPCTSTR str = L"EasyX";
			outtextxy((getwidth() - textwidth(str)) / 2, getheight() - 100, str);

			EasyWin32::FlushDrawing();
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BTN2:	// 按下按钮

			// 打开窗口 3，并将其作为自己的子窗口
			EasyWin32::initgraph_win32(400, 240, 0, L"Third", WndProc3, hwnd);

			break;
		}
		break;

	default: return true; break;
	}
	return false;
}

int main()
{
	// 创建绘图窗口
	EasyWin32::initgraph_win32(640, 480, 0, L"", WndProc1);
	EasyWin32::initgraph_win32(640, 480, 0, L"", WndProc2);

	// 在使用 win32 消息派发的代码结构时，需要调用此函数进行阻塞
	EasyWin32::init_end();

	return 0;
}

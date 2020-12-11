/////////////////////////////////////
//
//	EasyWin32.cpp
//	by huidong <mailkey@yeah.net>
//
//	基于EasyX图形库的Win32控件支持库
//	EasyX版本：2020.9.2版
//
//	创建时间：2020.12.6
//	最后修改：2020.12.11
//

#include "EasyWin32.h"
#include "AHGraphics.h"

// 窗口类
WNDCLASSEX WndClassEx;

// 窗口类名
wchar_t pszClassName[] = L"EasyX_Win32_Class";

HWND hWnd;      // 窗口句柄
MSG Msg;        // 消息结构体

// 用户的消息响应函数
bool(*funcWndProc)(HWND, UINT, WPARAM, LPARAM, HINSTANCE);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// 必须预先处理的一些消息
	switch (msg)
	{
	case WM_CREATE:
		// 配置绘图窗口
		InitDrawing(hwnd);
		break;

	case WM_SIZE:
		// 自动调整绘图区大小
		MainImageFitWindow();
		break;
	}

	bool r;
	if (funcWndProc)
		r = funcWndProc(hwnd, msg, wParam, lParam,GetModuleHandle(0));

	// 必须在用户处理消息后做的一些后续工作
	switch (msg)
	{
	case WM_PAINT:
		// 自动输出绘图缓存，且再次重绘win32窗口（控件等）
		FlushDrawingToWnd();
		DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}

	if (!r)
		return 0;

	// 后续处理
	switch (msg)
	{
	case WM_CLOSE:
		EndDrawing();
		DestroyWindow(hwnd);
		PostQuitMessage(NULL);
		exit(0);
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Redraw_win32()
{
	InvalidateRect(hWnd, NULL, false);
}

void initgraph_win32(int w, int h, int mode, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), LPCTSTR strWndTitle)
{
	// 隐藏cmd
	if (mode == 0)
		ShowWindow(GetConsoleWindow(), SW_HIDE);

	// 填写结构体
	WndClassEx.cbSize = sizeof(WNDCLASSEX);
	WndClassEx.style = CS_VREDRAW | CS_HREDRAW;
	WndClassEx.lpfnWndProc = WndProc;
	WndClassEx.cbClsExtra = 0;
	WndClassEx.cbWndExtra = 0;
	WndClassEx.hInstance = GetModuleHandle(0);
	WndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClassEx.lpszMenuName = NULL;
	WndClassEx.lpszClassName = pszClassName;
	WndClassEx.hIconSm = NULL;

	// 注册窗口类
	RegisterClassEx(&WndClassEx);

	// 设置消息响应函数
	funcWndProc = WindowProcess;

	// 创建窗口
	hWnd = CreateWindowEx(WS_EX_WINDOWEDGE, pszClassName, strWndTitle, WS_OVERLAPPEDWINDOW, 200, 200, w, h, NULL, NULL, GetModuleHandle(0), NULL);

	// 显示窗口
	ShowWindow(hWnd, SW_SHOWNORMAL);

	// 更新窗口
	UpdateWindow(hWnd);

	// 循环获得消息
	while (GetMessage(&Msg, NULL, NULL, NULL))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

HWND GetHWnd_win32()
{
	return hWnd;
}

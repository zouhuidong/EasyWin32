///////////////////////////////////////
//
//	main.cpp
//	by huidong <mailkey@yeah.net>
//	
//	EasyWin32库使用示例代码
//
//	创建时间：2020.12.6
//	最后修改：2020.12.11
//

#include "EasyWin32.h"

#define IDC_EDIT 100
#define IDC_BTN 101

HWND hEdit;
HWND hBtn;

wchar_t str[512] = L"Hello, EasyX.";

bool WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
	case WM_CREATE:

		// 创建一个输入框，一个按钮
		hEdit = CreateWindow(L"edit", L"Edit at here.",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			130, 50, 200, 20,
			hwnd, (HMENU)IDC_EDIT, hInstance, NULL);

		hBtn = CreateWindow(L"button", L"Click Me!",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			330, 50, 100, 20,
			hwnd, (HMENU)IDC_BTN, hInstance, NULL);

		// 在这里设置绘图属性
		setbkcolor(WHITE);
		setlinecolor(BLACK);
		setfillcolor(BLUE);
		settextcolor(GREEN);
		settextstyle(32, 0, L"system");

		break;

	case WM_PAINT:

		// 随便画点什么
		cleardevice();
		setlinestyle(0, 5);
		line(0, 0, 300, 100);
		line(0, 0, getwidth(), getheight());
		fillrectangle(200,200,600,400);
		outtextxy(130, 150, str);

		break;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{

		// 按下按钮
		case IDC_BTN:

			// 得到输入框文本并强制重绘
			GetWindowText(hEdit, str, 512);
			Redraw_win32();

			break;
		}

		break;

	default:
		// 需要系统来处理此消息
		return true;
		break;
	}

	// 已经处理过消息，无需系统再处理
	return false;
}

int main()
{
	// 创建一个支持win32控件的图形界面
	initgraph_win32(640, 480, 0, WndProc);
	return 0;
}

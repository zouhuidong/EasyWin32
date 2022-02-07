/////////////////////////////////////
//
//	EasyWin32.cpp
//	
//	详细信息见 EasyWin32.h
//

#include "EasyWin32.h"
#include "AHGraphics.h"
#include <thread>

EASY_WIN32_BEGIN

////////////****** 全局变量 ******////////////

// 窗口类
WNDCLASSEX WndClassEx;

// 窗口类名
LPCTSTR pszClassName = L"EasyX_Win32_Class";

// 正操作窗口
EasyWindow* pFocusWindow = NULL;

// 绘图任务是否忙碌中
bool isBusyDrawing = false;

// 窗口表（可能创建了多个窗口）
std::vector<EasyWindow> vecWindows;

// 系统分辨率
int sysW = 0, sysH = 0;

HWND hConsole;			// 控制台句柄

////////////****** 函数定义 ******////////////

// 通过句柄获得此窗口在窗口记录表中的索引
// 未找到返回 -1
int GetWindowByHWND(HWND hWnd)
{
	int index = -1;
	for (int i = 0; i < (int)vecWindows.size(); i++)
	{
		if (hWnd == vecWindows[i].hWnd)
		{
			index = i;
			break;
		}
	}
	return index;
}

// 按窗口索引进行关闭窗口
void closegraph_win32(int index)
{
	EasyWindow* pWnd = &vecWindows[index];

	// 若待关闭窗口正在绘图，必须等待绘图结束，防止内存越界
	while (pWnd == pFocusWindow && isBusyDrawing) { Sleep(1); };
	delete pWnd->pImg;
	delete pWnd->pBufferImg;
	pWnd->pImg = NULL;
	pWnd->pBufferImg = NULL;

	//DestroyWindow(pWnd->hWnd);
	PostQuitMessage(NULL);

	// 删除此窗口的记录
	vecWindows.erase(vecWindows.begin() + index);
}

void closegraph_win32(HWND hWnd)
{
	if (hWnd == NULL)	// close all
	{
		for (int i = 0; i < (int)vecWindows.size(); i++)
		{
			closegraph_win32(i);
		}
	}
	else
	{
		int index = GetWindowByHWND(hWnd);
		if (index == -1)
		{
			return;
		}
		closegraph_win32(index);
	}
}

void init_end()
{
	while (true)
	{
		if (vecWindows.empty())	// 若所有窗口都被关闭，则跳出
		{
			return;
		}

		Sleep(100);
	}
}

bool isAnyWindow()
{
	return !vecWindows.empty();
}

bool isInListWindow(HWND hWnd)
{
	return GetWindowByHWND(hWnd) == -1 ? false : true;
}

HWND GetHWnd_win32()
{
	return pFocusWindow->hWnd;
}

EasyWindow GetWorkingWindow()
{
	return *pFocusWindow;
}

bool SetWorkingWindow(HWND hWnd)
{
	int index = GetWindowByHWND(hWnd);
	if (index == -1)
	{
		return false;
	}
	pFocusWindow = &vecWindows[index];
	while (isBusyDrawing) { Sleep(1); };
	SetWorkingImage(pFocusWindow->pBufferImg);
	return true;
}

void EnforceRedraw()
{
	InvalidateRect(pFocusWindow->hWnd, NULL, false);
}

void ReadyToDraw()
{
	isBusyDrawing = true;
}

void FlushDrawing()
{
	*pFocusWindow->pImg = *pFocusWindow->pBufferImg;
	isBusyDrawing = false;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// 当前传入窗口
	EasyWindow* pWnd = NULL;
	bool isNeedDefaultProcess = true;		// 记录是否需要使用默认方法处理消息
	int indexWnd = GetWindowByHWND(hwnd);	// 该窗口在已记录列表中的索引
	if (indexWnd == -1)	// 出现未知窗口，则使用默认方法进行处理（此情况按理来说不会出现）
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	pWnd = &vecWindows[indexWnd];
	RECT rctWnd;
	GetWindowRect(hwnd, &rctWnd);		// 窗口矩形信息

	//** 开始处理窗口消息 **//

	// 由于 WM_CREATE 消息被吞噬，需要模拟发送此消息
	if (!pWnd->isSentCreateMsg)
	{
		pWnd->isSentCreateMsg = true;
		WndProc(hwnd, WM_CREATE, NULL, NULL);
	}

	// 必须预先处理的一些消息
	switch (msg)
	{
	case WM_CREATE:
		break;

	case WM_SIZE:

		// 对该窗口对应的画布进行调整
		POINT pWndSize = GetWindowSize(hwnd);
		pWnd->pImg->Resize(pWndSize.x, pWndSize.y);
		pWnd->pBufferImg->Resize(pWndSize.x, pWndSize.y);

		pWnd->isNewSize = true;

		break;

		// 鼠标消息
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:

		ExMessage msgMouse;
		msgMouse.message = msg;
		msgMouse.x = LOWORD(lParam);
		msgMouse.y = HIWORD(lParam);
		msgMouse.wheel = HIWORD(wParam);
		msgMouse.shift = LOWORD(wParam) & 0x04 ? true : false;
		msgMouse.ctrl = LOWORD(wParam) & 0x08 ? true : false;
		msgMouse.lbutton = LOWORD(wParam) & 0x01 ? true : false;
		msgMouse.mbutton = LOWORD(wParam) & 0x10 ? true : false;
		msgMouse.rbutton = LOWORD(wParam) & 0x02 ? true : false;

		// 记录
		pWnd->vecMouseMsg.push_back(msgMouse);

		break;

		// 键盘消息甩锅给控制台，实现对按键消息的支持
	case WM_KEYDOWN: case WM_KEYUP: case WM_CHAR:
	/*case WM_IME_KEYDOWN: case WM_IME_KEYUP: case WM_IME_CHAR:*/

		if (pWnd == pFocusWindow)	// 当前窗口得是焦点窗口，才接受此消息
		{
			SendMessage(hConsole, msg, wParam, lParam);
		}
		break;

	}

	// 若有独立的消息处理函数则调用
	if (pWnd->funcWndProc)
	{
		isNeedDefaultProcess = pWnd->funcWndProc(hwnd, msg, wParam, lParam, GetModuleHandle(0));
	}

	// 必须在用户处理消息后做的一些后续工作
	switch (msg)
	{
	case WM_PAINT:
		// 自动输出绘图缓存，且再次重绘窗口
		FlushDrawingToWnd(pWnd->pImg, pWnd->hWnd);
		DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_MOVE:
		// 由于移动窗口超出屏幕的话可能导致子窗口显示有问题，所以此时需要彻底重绘
		if (rctWnd.left <= 0 || rctWnd.top <= 0 || rctWnd.right >= sysW || rctWnd.bottom >= sysH)
		{
			InvalidateRect(hwnd, NULL, false);
		}
	}

	// 如需使用默认方法继续处理
	if (isNeedDefaultProcess)
	{
		switch (msg)
		{
		case WM_CLOSE:
			closegraph_win32(indexWnd);
			break;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	else
	{
		return 0;
	}
}

std::vector<EasyWindow> GetCreatedWindowList()
{
	return vecWindows;
}

bool isWindowSizeChanged()
{
	bool b = pFocusWindow->isNewSize;
	pFocusWindow->isNewSize = false;
	return b;
}

bool MouseHit_win32()
{
	if (pFocusWindow->nGetMouseMsgIndex < (int)pFocusWindow->vecMouseMsg.size() - 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

ExMessage GetMouseMsg_win32()
{
	while (!MouseHit_win32()) { Sleep(1); };
	ExMessage msg = pFocusWindow->vecMouseMsg[pFocusWindow->nGetMouseMsgIndex];
	if (pFocusWindow->nGetMouseMsgIndex < (int)pFocusWindow->vecMouseMsg.size())
	{
		pFocusWindow->nGetMouseMsgIndex++;
	}
	return msg;
}

bool PeekMouseMsg_win32(ExMessage* pMsg)
{
	std::vector<ExMessage>* p_vecMsg = &pFocusWindow->vecMouseMsg;
	if (p_vecMsg->empty())
	{
		return false;
	}

	// 若有新消息就按进度获取，否则获取最新一个
	if (MouseHit_win32())
	{
		*pMsg = GetMouseMsg_win32();
	}
	else
	{
		*pMsg = (*p_vecMsg)[p_vecMsg->size() - 1];
	}

	return true;
}

void FlushMouseMsg_win32()
{
	pFocusWindow->vecMouseMsg.clear();
	pFocusWindow->nGetMouseMsgIndex = 0;
}

void RegisterWndClass()
{
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
}

// 真正创建窗口的函数
void InitWindow(int w, int h, bool isCmd, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent, bool* isDone)
{
	static int nWndNum;		// 窗口计数
	std::wstring wstrTitle;	// 窗口标题

	// 未设置标题
	if (lstrlen(strWndTitle) == 0)
	{
		wstrTitle = L"EasyX Window";
		if (nWndNum != 0)
		{
			wstrTitle += L" (" + std::to_wstring(nWndNum + 1) + L")";
		}
	}
	else
	{
		wstrTitle = strWndTitle;
	}

	// 获取分辨率
	if (!sysW)
	{
		sysW = GetSystemMetrics(SM_CXSCREEN);
		sysH = GetSystemMetrics(SM_CYSCREEN);
	}

	// 第一次创建窗口
	if (nWndNum == 0)
	{
		// 注册窗口类
		RegisterWndClass();
		hConsole = GetConsoleWindow();
	}

	// 控制台
	if (hConsole)
	{
		ShowWindow(hConsole, isCmd ? SW_NORMAL : SW_HIDE);
	}

	// 初始化窗口信息
	EasyWindow wnd;
	wnd.hWnd = CreateWindowEx(
		WS_EX_WINDOWEDGE,
		pszClassName,
		wstrTitle.c_str(),
		WS_OVERLAPPEDWINDOW | CS_DBLCLKS,
		CW_USEDEFAULT, CW_USEDEFAULT,
		w, h,
		hParent,
		NULL,
		GetModuleHandle(0),
		NULL
	);
	wnd.pImg = new IMAGE(w, h);
	wnd.pBufferImg = new IMAGE(w, h);
	wnd.funcWndProc = WindowProcess;
	wnd.nGetMouseMsgIndex = 0;
	wnd.isNewSize = false;
	wnd.isSentCreateMsg = false;

	vecWindows.push_back(wnd);
	SetWorkingWindow(wnd.hWnd);

	ShowWindow(wnd.hWnd, SW_SHOWNORMAL);
	UpdateWindow(wnd.hWnd);

	nWndNum++;

	*isDone = true;

	MSG Msg;
	while (GetMessage(&Msg, NULL, NULL, NULL))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

HWND initgraph_win32(int w, int h, bool isCmd, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent)
{
	bool isDone = false;
	if (!hParent)
	{
		std::thread(InitWindow, w, h, isCmd, strWndTitle, WindowProcess, hParent, &isDone).detach();
		while (!isDone) { Sleep(10); };	// 等待窗口创建完成
		return pFocusWindow->hWnd;
	}
	else
	{
		EnableWindow(hParent, false);
		InitWindow(w, h, isCmd, strWndTitle, WindowProcess, hParent, &isDone);
		EnableWindow(hParent, true);
		SetForegroundWindow(hParent);
	
		return NULL;
	}
}


EASY_WIN32_END

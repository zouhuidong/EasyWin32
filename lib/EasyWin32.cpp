/////////////////////////////////////
//
//	EasyWin32.cpp
//	
//	详细信息见 EasyWin32.h
//

#include "EasyWin32.h"
#include "AHGraphics.h"
#include <thread>

// 最大鼠标消息积累量
//		若鼠标消息积累量达到此阈值，且消息都已经处理完，将自动清理鼠标消息积累
//		若尚未处理完所有鼠标消息，则不会自动清理鼠标消息积累
#define MAX_MOUSEMSG_SIZE 1000

EASY_WIN32_BEGIN

////////////****** 全局变量 ******////////////

// 窗口类
WNDCLASSEX WndClassEx;

// 窗口类名
wchar_t wstrClassName[] = L"EasyWin32_Class";

// 正操作窗口
EasyWindow* pFocusWindow = NULL;

// 绘图任务是否忙碌中
bool isBusyDrawing = false;

// 窗口表（可能创建了多个窗口）
std::vector<EasyWindow> vecWindows;

// 系统分辨率
int nSysW = 0, nSysH = 0;

// 系统标题栏宽高
int nFrameW = 0, nFrameH = 0;

// 控制台句柄
HWND hConsole;

// 是否使用自定义图标
bool isUseCustomAppIcon = false;

////////////****** 函数定义 ******////////////

// 阻塞等待当前绘图任务完成
void WaitForDrawing()
{
	while (isBusyDrawing)
	{
		HpSleep(1);
	}
}

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

// 删除窗口，释放内存
void DelWindow(int index)
{
	EasyWindow* pWnd = &vecWindows[index];

	delete pWnd->pImg;
	delete pWnd->pBufferImg;
	pWnd->pImg = NULL;
	pWnd->pBufferImg = NULL;

	DestroyWindow(pWnd->hWnd);
	PostQuitMessage(NULL);

	// 删除此窗口的记录
	vecWindows.erase(vecWindows.begin() + index);
}

// 按窗口索引进行关闭窗口
void closegraph_win32(int index)
{
	EasyWindow* pWnd = &vecWindows[index];

	// 防止和当前绘图任务冲突
	if (pWnd == pFocusWindow)
	{
		WaitForDrawing();
		isBusyDrawing = true;
		DelWindow(index);
		isBusyDrawing = false;
	}
	else
	{
		DelWindow(index);
	}
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
	else				// close single
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
	WaitForDrawing();
	SetWorkingImage(pFocusWindow->pBufferImg);
	return true;
}

void EnforceRedraw()
{
	InvalidateRect(pFocusWindow->hWnd, NULL, false);
}

void ReadyToDraw()
{
	WaitForDrawing();
	isBusyDrawing = true;
}

void FlushDrawing()
{
	*pFocusWindow->pImg = *pFocusWindow->pBufferImg;
	isBusyDrawing = false;
}

// 根据窗口大小重新调整该窗口画布大小
void ResizeWindowImage(EasyWindow* pWnd)
{
	RECT rcWnd;
	for (int i = 0; i < 2; i++)
	{
		if (GetClientRect(pWnd->hWnd, &rcWnd))
		{
			pWnd->pImg->Resize(rcWnd.right, rcWnd.bottom);
			pWnd->pBufferImg->Resize(rcWnd.right, rcWnd.bottom);
			pWnd->isNewSize = true;
			break;
		}
	}
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
		if (pWnd == pFocusWindow)
		{
			WaitForDrawing();		// 防止和当前绘图任务发生冲突
			isBusyDrawing = true;
			ResizeWindowImage(pWnd);
			isBusyDrawing = false;
		}
		else
		{
			ResizeWindowImage(pWnd);
		}
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
	{
		// 消息都已经处理完，且堆存消息数量达到阈值
		if (!MouseHit_win32() && pWnd->vecMouseMsg.size() >= MAX_MOUSEMSG_SIZE)
		{
			FlushMouseMsgBuffer_win32();
		}

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
	}
	break;

	// 键盘消息甩锅给控制台，实现对按键消息的支持
	case WM_KEYDOWN: case WM_KEYUP: case WM_CHAR:
		SendMessage(hConsole, msg, wParam, lParam);
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
		if (rctWnd.left <= 0 || rctWnd.top <= 0 || rctWnd.right >= nSysW || rctWnd.bottom >= nSysH)
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

bool GetIsUseCustomAppIcon()
{
	return isUseCustomAppIcon;
}

void SetIsUseCustomAppIcon(bool bUse)
{
	isUseCustomAppIcon = bUse;
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
	while (!MouseHit_win32()) { HpSleep(1); };
	ExMessage msg = pFocusWindow->vecMouseMsg[pFocusWindow->nGetMouseMsgIndex];
	if (pFocusWindow->nGetMouseMsgIndex < (int)pFocusWindow->vecMouseMsg.size())
	{
		pFocusWindow->nGetMouseMsgIndex++;
	}
	return msg;
}

bool PeekMouseMsg_win32(ExMessage* pMsg, bool bRemoveMsg)
{
	if (MouseHit_win32())
	{
		if (bRemoveMsg)
		{
			*pMsg = GetMouseMsg_win32();
		}
		else
		{
			*pMsg = pFocusWindow->vecMouseMsg[pFocusWindow->nGetMouseMsgIndex];
		}

		return true;
	}
	else
	{
		return false;
	}
}

void FlushMouseMsgBuffer_win32()
{
	pFocusWindow->vecMouseMsg.clear();
	pFocusWindow->nGetMouseMsgIndex = 0;
}

ExMessage getmessage_win32(BYTE filter)
{
	switch (filter)
	{
	case -1:
	case EM_MOUSE:		return GetMouseMsg_win32();		break;
	default:			return ExMessage();				break;
	}
}

void getmessage_win32(ExMessage* msg, BYTE filter)
{
	*msg = getmessage_win32(filter);
}

bool peekmessage_win32(ExMessage* msg, BYTE filter, bool removemsg)
{
	switch (filter)
	{
	case -1:
	case EM_MOUSE:		return PeekMouseMsg_win32(msg, removemsg);		break;
	default:			return false;									break;
	}
}

void flushmessage_win32(BYTE filter)
{
	switch (filter)
	{
	case -1:
	case EM_MOUSE:		FlushMouseMsgBuffer_win32();		break;
	default:												break;
	}
}

ExMessage To_ExMessage(MOUSEMSG msg)
{
	ExMessage msgEx;
	msgEx.message = msg.uMsg;
	msgEx.ctrl = msg.mkCtrl;
	msgEx.shift = msg.mkShift;
	msgEx.lbutton = msg.mkLButton;
	msgEx.mbutton = msg.mkMButton;
	msgEx.rbutton = msg.mkRButton;
	msgEx.x = msg.x;
	msgEx.y = msg.y;
	msgEx.wheel = msg.wheel;
	return msgEx;
}

MOUSEMSG To_MouseMsg(ExMessage msgEx)
{
	MOUSEMSG msg;
	msg.uMsg = msgEx.message;
	msg.mkCtrl = msgEx.ctrl;
	msg.mkShift = msgEx.shift;
	msg.mkLButton = msgEx.lbutton;
	msg.mkMButton = msgEx.mbutton;
	msg.mkRButton = msgEx.rbutton;
	msg.x = msgEx.x;
	msg.y = msgEx.y;
	msg.wheel = msgEx.wheel;
	return msg;
}

bool PeekMouseMsg_win32_old(MOUSEMSG* pMsg, bool bRemoveMsg)
{
	ExMessage msgEx;
	bool r = PeekMouseMsg_win32(&msgEx, bRemoveMsg);
	*pMsg = To_MouseMsg(msgEx);
	return r;
}

IMAGE GetDefaultAppIconImage()
{
	IMAGE* old = GetWorkingImage();
	IMAGE img(32, 32);
	SetWorkingImage(&img);

	setbkcolor(RED);
	setbkmode(TRANSPARENT);

	settextcolor(WHITE);
	settextstyle(48, 0, L"Consolas");

	setfillcolor(BLUE);
	setlinecolor(BLUE);

	cleardevice();
	fillcircle(16, 16, 16);
	outtextxy(4, -8, L'X');

	SetWorkingImage(old);
	return img;
}

// 获取默认窗口图标
HICON GetDefaultAppIcon()
{
	IMAGE img = GetDefaultAppIconImage();
	HBITMAP hBmp = GetImageHBitmap(&img);
	HICON hIcon = HICONFromHBitmap(hBmp);
	DeleteObject(hBmp);
	return hIcon;
}

void RegisterWndClass()
{
	srand((UINT)time(NULL));

	HICON hIcon;	// 程序图标
	if (isUseCustomAppIcon)
	{
		hIcon = LoadIcon(NULL, IDI_APPLICATION);
	}
	else
	{
		hIcon = GetDefaultAppIcon();
	}

	WndClassEx.cbSize = sizeof(WNDCLASSEX);
	WndClassEx.style = CS_VREDRAW | CS_HREDRAW;
	WndClassEx.lpfnWndProc = WndProc;
	WndClassEx.cbClsExtra = 0;
	WndClassEx.cbWndExtra = 0;
	WndClassEx.hInstance = GetModuleHandle(0);
	WndClassEx.hIcon = hIcon;
	WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClassEx.lpszMenuName = NULL;
	WndClassEx.lpszClassName = wstrClassName;
	WndClassEx.hIconSm = hIcon;

	// 注册窗口类
	if (!RegisterClassEx(&WndClassEx))
	{
		std::wstring str = std::to_wstring(GetLastError());
		MessageBox(NULL, (L"Error registing window class: " + str).c_str(), L"[Error]", MB_OK | MB_ICONERROR);
		exit(-1);
	}
}

// 真正创建窗口的函数
void InitWindow(int w, int h, bool isCmd, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent, int* nDoneFlag)
{
	static int nWndCount;		// 窗口计数
	std::wstring wstrTitle;	// 窗口标题

	// 未设置标题
	if (lstrlen(strWndTitle) == 0)
	{
		wstrTitle = L"EasyX Window";
		if (nWndCount != 0)
		{
			wstrTitle += L" (" + std::to_wstring(nWndCount + 1) + L")";
		}
	}
	else
	{
		wstrTitle = strWndTitle;
	}

	// 第一次创建窗口
	if (nWndCount == 0)
	{
		// 获取分辨率
		nSysW = GetSystemMetrics(SM_CXSCREEN);
		nSysH = GetSystemMetrics(SM_CYSCREEN);

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

	// 创建窗口
	for (int i = 0;; i++)
	{
		wnd.hWnd = CreateWindowEx(
			WS_EX_WINDOWEDGE,
			wstrClassName,
			wstrTitle.c_str(),
			WS_OVERLAPPEDWINDOW | CS_DBLCLKS,
			CW_USEDEFAULT, CW_USEDEFAULT,
			w, h,
			hParent,
			NULL,
			GetModuleHandle(0),
			NULL
		);

		if (wnd.hWnd)
		{
			break;
		}

		// 三次创建窗口失败，不再尝试
		else if (i == 2)
		{
			std::wstring str = std::to_wstring(GetLastError());
			MessageBox(NULL, (L"Error creating window: " + str).c_str(), L"[Error]", MB_OK | MB_ICONERROR);
			*nDoneFlag = -1;
			return;
		}
	}

	wnd.pImg = new IMAGE(w, h);
	wnd.pBufferImg = new IMAGE(w, h);
	wnd.funcWndProc = WindowProcess;
	wnd.vecMouseMsg.reserve(MAX_MOUSEMSG_SIZE);
	wnd.nGetMouseMsgIndex = 0;
	wnd.isNewSize = false;
	wnd.isSentCreateMsg = false;

	vecWindows.push_back(wnd);
	SetWorkingWindow(wnd.hWnd);

	// 获取边框大小，补齐绘图区大小
	if (nWndCount == 0)
	{
		RECT rcClient, rcWnd;
		GetClientRect(wnd.hWnd, &rcClient);
		GetWindowRect(wnd.hWnd, &rcWnd);
		nFrameW = (rcWnd.right - rcWnd.left) - rcClient.right;
		nFrameH = (rcWnd.bottom - rcWnd.top) - rcClient.bottom;
	}
	SetWindowPos(wnd.hWnd, HWND_TOP, 0, 0, w + nFrameW, h + nFrameH, SWP_NOMOVE);

	ShowWindow(wnd.hWnd, SW_SHOWNORMAL);
	UpdateWindow(wnd.hWnd);

	nWndCount++;

	*nDoneFlag = 1;

	MSG Msg;
	while (GetMessage(&Msg, NULL, NULL, NULL) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

HWND initgraph_win32(int w, int h, bool isCmd, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent)
{
	int nDoneFlag = 0;
	if (!hParent)
	{
		std::thread(InitWindow, w, h, isCmd, strWndTitle, WindowProcess, hParent, &nDoneFlag).detach();
		while (nDoneFlag == 0) { Sleep(10); };	// 等待窗口创建完成
		if (nDoneFlag == -1)	return NULL;
		else					return pFocusWindow->hWnd;
	}
	else
	{
		EnableWindow(hParent, false);
		InitWindow(w, h, isCmd, strWndTitle, WindowProcess, hParent, &nDoneFlag);
		EnableWindow(hParent, true);
		SetForegroundWindow(hParent);

		return NULL;
	}
}


EASY_WIN32_END



void HpSleep(int ms)
{
	static clock_t oldclock = clock();		// 静态变量，记录上一次 tick

	oldclock += ms * CLOCKS_PER_SEC / 1000;	// 更新 tick

	if (clock() > oldclock)					// 如果已经超时，无需延时
		oldclock = clock();
	else
		while (clock() < oldclock)			// 延时
			Sleep(1);						// 释放 CPU 控制权，降低 CPU 占用率
//			Sleep(0);						// 更高精度、更高 CPU 占用率
}

HBITMAP GetImageHBitmap(IMAGE* img)
{
	return CreateBitmap(img->getwidth(), img->getheight(), 1, 32, (void*)GetImageBuffer(img));
}

HICON HICONFromHBitmap(HBITMAP hBmp)
{
	BITMAP bmp;
	GetObject(hBmp, sizeof(BITMAP), &bmp);

	HBITMAP hbmMask = CreateCompatibleBitmap(GetDC(NULL), bmp.bmWidth, bmp.bmHeight);

	ICONINFO ii = { 0 };
	ii.fIcon = TRUE;
	ii.hbmColor = hBmp;
	ii.hbmMask = hbmMask;

	HICON hIcon = CreateIconIndirect(&ii);
	DeleteObject(hbmMask);

	return hIcon;
}

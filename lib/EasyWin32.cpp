/////////////////////////////////////
//
//	EasyWin32.cpp
//	
//	详细信息见 EasyWin32.h
//

#include "EasyWin32.h"

// 最大鼠标消息积累量
//		若鼠标消息积累量达到此阈值，且消息都已经处理完，将自动清理鼠标消息积累
//		若尚未处理完所有鼠标消息，则不会自动清理鼠标消息积累
#define MAX_MOUSEMSG_SIZE 100

EASY_WIN32_BEGIN

////////////****** 全局变量 ******////////////

// 窗口类
WNDCLASSEX WndClassEx;

// 窗口类名
wchar_t wstrClassName[] = L"EasyWin32_Class";

// 正操作窗口
EasyWindow* pFocusWindow = NULL;
int nFocusWindowIndex = -1;		// 注：该标识仅用于恢复窗口指针

// 窗口表（可能创建了多个窗口）
std::vector<EasyWindow> vecWindows;

// 系统分辨率
int nSysW = 0, nSysH = 0;

// 系统标题栏宽高
int nFrameW = 0, nFrameH = 0;

// 控制台句柄
HWND hConsole;

// 自定义程序图标，为 0 表示不使用
int nCustomIcon = 0;
int nCustomIconSm = 0;

// 当前是否处在任务中
bool bInTask = false;

////////////****** 函数定义 ******////////////

// 将绘制在 EasyX 中的内容显示到目标窗口上
void FlushDrawingToWnd(IMAGE* pImg, HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	HDC hdcImg = GetImageHDC(pImg);
	RECT rctWnd;
	GetClientRect(hWnd, &rctWnd);
	BitBlt(hdc, 0, 0, rctWnd.right, rctWnd.bottom, hdcImg, 0, 0, SRCCOPY);
	ReleaseDC(hWnd, hdc);
}

void WaitForTask(HWND hWnd)
{
	if (!hWnd || (pFocusWindow && pFocusWindow->hWnd == hWnd))
	{
		while (bInTask)
		{
			HpSleep(1);
		}
	}
}

void WaitForProcessing(EasyWindow* pWnd)
{
	while (pWnd && pWnd->isBusyProcessing)
	{
		HpSleep(1);
	}
}

// 通过句柄获得此窗口在窗口记录表中的索引
// 未找到返回 -1
int GetWindowID(HWND hWnd)
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

	// 必须先保存下图像指针以供 delete，不能在 vector 移除后使用 pWnd 的图像指针
	IMAGE* img[2] = { pWnd->pImg ,pWnd->pBufferImg };

	// 释放消息列表内存
	std::vector<ExMessage>().swap(pWnd->vecMessage);

	// 删除此窗口的记录
	vecWindows.erase(vecWindows.begin() + index);

	DestroyWindow(pWnd->hWnd);
	PostQuitMessage(NULL);

	// 为避免外部调用已经析构的指针，释放内存的操作在 vector 移除元素后执行
	for (int i = 0; i < 2; i++)
	{
		delete img[i];
	}
}

// 按窗口索引进行关闭窗口
void closegraph_win32(int index)
{
	EasyWindow* pWnd = &vecWindows[index];

	// 若已设置父窗口为模态窗口，则需要将父窗口恢复正常
	if (pWnd->hParent != NULL)
	{
		EnableWindow(pWnd->hParent, true);
		SetForegroundWindow(pWnd->hParent);
	}

	// 防止和当前绘图任务冲突
	WaitForTask(pWnd->hWnd);
	pWnd->isBusyProcessing = true;

	// 为避免产生歧义，活动窗口必须置空
	if (pWnd == pFocusWindow)
	{
		pFocusWindow = NULL;
		nFocusWindowIndex = -1;
	}

	DelWindow(index);

	// 关闭的是最后一个被创建的窗口，则需要重置部分内容，防止外界引用出错
	size_t size = vecWindows.size();
	if (index == size)
	{
		pWnd->hWnd = NULL;
		pWnd->isBusyProcessing = false;
	}
}

void closegraph_win32(HWND hWnd)
{
	// 关闭全部
	if (hWnd == NULL)
	{
		for (int i = 0; i < (int)vecWindows.size(); i++)
		{
			// 必须交由原线程销毁窗口，才能使窗口销毁
			// 特殊标记 wParam 为 1，表示程序命令销毁窗口
			SendMessage(vecWindows[i].hWnd, WM_DESTROY, 1, 0);
		}
	}
	else
	{
		SendMessage(hWnd, WM_DESTROY, 1, 0);
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

// 自动退出程序的实际执行函数
void AutoExit_main()
{
	init_end();
	exit(0);
}

void AutoExit()
{
	std::thread(AutoExit_main).detach();
}

bool isAnyWindow()
{
	return !vecWindows.empty();
}

bool isAliveWindow(HWND hWnd)
{
	if (hWnd)	return GetWindowID(hWnd) == -1 ? false : true;
	else		return pFocusWindow;
}

HWND GetHWnd_win32()
{
	return pFocusWindow ? pFocusWindow->hWnd : NULL;
}

EasyWindow GetWorkingWindow()
{
	return pFocusWindow ? *pFocusWindow : EasyWindow();
}

bool SetWorkingWindow(HWND hWnd)
{
	int index = GetWindowID(hWnd);
	if (index == -1)	return false;
	WaitForTask();
	WaitForProcessing(pFocusWindow);
	pFocusWindow = &vecWindows[index];
	nFocusWindowIndex = index;
	SetWorkingImage(pFocusWindow->pBufferImg);
	return true;
}

void QuickDraw(UINT nSkipPixels)
{
	pFocusWindow->nSkipPixels = nSkipPixels;
}

void EnforceRedraw()
{
	if (pFocusWindow)
	{
		InvalidateRect(pFocusWindow->hWnd, NULL, false);
	}
}

// 复制缓冲区
void FlushDrawing(EasyWindow* pWnd = pFocusWindow)
{
	int w = pWnd->pImg->getwidth();
	int h = pWnd->pImg->getheight();

	if (pWnd->nSkipPixels == 0)
	{
		// fastest
		memcpy(
			GetImageBuffer(pWnd->pImg),
			GetImageBuffer(pWnd->pBufferImg),
			sizeof(DWORD) * w * h
		);
		//*pFocusWindow->pImg = *pFocusWindow->pBufferImg;
	}
	else
	{
		int len = w * h;
		DWORD* buf[2] = { GetImageBuffer(pWnd->pImg) ,GetImageBuffer(pWnd->pBufferImg) };
		for (int i = 0; i < len; i++)
		{
			if (buf[0][i] == buf[1][i])
			{
				i += pWnd->nSkipPixels;
				continue;
			}
			buf[0][i] = buf[1][i];
			buf[0][i] = buf[1][i];
		}
	}
}

bool BeginTask()
{
	if (!bInTask && pFocusWindow)
	{
		WaitForTask();
		WaitForProcessing(pFocusWindow);
		bInTask = true;
	}
	return bInTask;
}

void EndTask()
{
	if (bInTask)
	{
		if (pFocusWindow && isAliveWindow(pFocusWindow->hWnd))
		{
			FlushDrawing();
		}

		bInTask = false;
	}
}

bool isInTask(HWND hWnd)
{
	return bInTask && (hWnd ? pFocusWindow && pFocusWindow->hWnd == hWnd : true);
}

// 根据窗口大小重新调整该窗口画布大小
void ResizeWindowImage(EasyWindow* pWnd)
{
	RECT rctWnd;
	for (int i = 0; i < 2; i++)
	{
		if (GetClientRect(pWnd->hWnd, &rctWnd))	// 客户区矩形
		{
			pWnd->pImg->Resize(rctWnd.right, rctWnd.bottom);
			pWnd->pBufferImg->Resize(rctWnd.right, rctWnd.bottom);
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
	int indexWnd = GetWindowID(hwnd);	// 该窗口在已记录列表中的索引
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
		// 防止和当前绘图任务发生冲突
		WaitForTask(pWnd->hWnd);
		pWnd->isBusyProcessing = true;
		ResizeWindowImage(pWnd);
		pWnd->isBusyProcessing = false;
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
		if (!MouseHit_win32(pWnd) && pWnd->vecMessage.size() >= MAX_MOUSEMSG_SIZE)
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
		pWnd->vecMessage.push_back(msgMouse);
	}
	break;

	// 键盘消息甩锅给控制台，实现对按键消息的支持
	case WM_KEYDOWN: case WM_KEYUP: case WM_CHAR:
		SendMessage(hConsole, msg, wParam, lParam);
		break;

	case WM_DESTROY:
		// WM_DESTROY 消息本无参数，若出现参数则是程序命令销毁窗口
		if (wParam)
		{
			closegraph_win32(indexWnd);
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
		FlushDrawingToWnd(pWnd->pImg, pWnd->hWnd);
		DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_MOVE:
		// 由于移动窗口超出屏幕的话可能导致子窗口显示有问题，所以此时需要彻底重绘
		if (rctWnd.left <= 0 || rctWnd.top <= 0 || rctWnd.right >= nSysW || rctWnd.bottom >= nSysH)
		{
			InvalidateRect(hwnd, NULL, false);
		}
		break;
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

bool isWindowSizeChanged(HWND hWnd)
{
	EasyWindow* pWnd = pFocusWindow;
	if (hWnd)
	{
		int id = GetWindowID(hWnd);
		if (id >= 0)	pWnd = &vecWindows[id];
		else			return false;
	}
	if (!pWnd)	return false;
	bool b = pWnd->isNewSize;
	pWnd->isNewSize = false;
	return b;
}

bool GetCustomIconState()
{
	return nCustomIcon;
}

void SetCustomIcon(int nIcon, int nIconSm)
{
	nCustomIcon = nIcon;
	nCustomIconSm = nIconSm;
}

bool MouseHit_win32(EasyWindow* pWnd)
{
	if (!pWnd)	pWnd = pFocusWindow;
	return pWnd && pWnd->nMessageIndex < (int)pWnd->vecMessage.size() - 1;
}

ExMessage GetMouseMsg_win32()
{
	while (!MouseHit_win32()) { HpSleep(1); };
	ExMessage msg = pFocusWindow->vecMessage[pFocusWindow->nMessageIndex];
	if (pFocusWindow->nMessageIndex < (int)pFocusWindow->vecMessage.size())
	{
		pFocusWindow->nMessageIndex++;
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
			*pMsg = pFocusWindow->vecMessage[pFocusWindow->nMessageIndex];
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
	pFocusWindow->vecMessage.clear();
	pFocusWindow->nMessageIndex = 0;
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

IMAGE GetDefaultIconImage()
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

long GetWindowStyle()
{
	return GetWindowLong(pFocusWindow->hWnd, GWL_STYLE);
}

long GetWindowExStyle()
{
	return GetWindowLong(pFocusWindow->hWnd, GWL_EXSTYLE);
}

int SetWindowStyle(long lNewStyle)
{
	return SetWindowLong(pFocusWindow->hWnd, GWL_STYLE, lNewStyle);
}

int SetWindowExStyle(long lNewExStyle)
{
	return SetWindowLong(pFocusWindow->hWnd, GWL_EXSTYLE, lNewExStyle);
}

// 获取默认窗口图标
HICON GetDefaultAppIcon()
{
	IMAGE img = GetDefaultIconImage();
	HBITMAP hBmp = GetImageHBitmap(&img);
	HICON hIcon = HICONFromHBitmap(hBmp);
	DeleteObject(hBmp);
	return hIcon;
}

void RegisterWndClass()
{
	HICON hIconDefault = GetDefaultAppIcon();
	HICON hIcon = hIconDefault;
	HICON hIconSm = hIconDefault;
	HINSTANCE hInstance = GetModuleHandle(0);

	if (nCustomIcon)
	{
		hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(nCustomIcon));
	}
	if (nCustomIconSm)
	{
		hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(nCustomIconSm));
	}

	WndClassEx.cbSize = sizeof(WNDCLASSEX);
	WndClassEx.style = CS_VREDRAW | CS_HREDRAW;
	WndClassEx.lpfnWndProc = WndProc;
	WndClassEx.cbClsExtra = 0;
	WndClassEx.cbWndExtra = 0;
	WndClassEx.hInstance = hInstance;
	WndClassEx.hIcon = hIcon;
	WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClassEx.lpszMenuName = NULL;
	WndClassEx.lpszClassName = wstrClassName;
	WndClassEx.hIconSm = hIconSm;

	// 注册窗口类
	if (!RegisterClassEx(&WndClassEx))
	{
		std::wstring str = std::to_wstring(GetLastError());
		MessageBox(NULL, (L"Error registing window class: " + str).c_str(), L"[Error]", MB_OK | MB_ICONERROR);
		exit(-1);
	}
}

// 真正创建窗口的函数（阻塞）
void InitWindow(int w, int h, int flag, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent, int* nDoneFlag)
{
	static int nWndCount;		// 窗口计数
	std::wstring wstrTitle;		// 窗口标题
	EasyWindow wnd;				// 窗口信息

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

		if (hConsole)
		{
			ShowWindow(hConsole, SW_HIDE);
		}
	}

	// 控制台
	if (hConsole && flag & EW_SHOWCONSOLE)
	{
		ShowWindow(hConsole, flag & SW_NORMAL);
	}

	int user_style = WS_OVERLAPPEDWINDOW;
	if (flag & EW_NOMINIMIZE)
	{
		user_style &= ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX;
	}
	/*if (flag & EW_NOCLOSE)
	{
		user_style &= ~WS_SYSMENU;
	}*/
	if (flag & EW_DBLCLKS)
	{
		user_style |= CS_DBLCLKS;
	}

	// 创建窗口
	for (int i = 0;; i++)
	{
		wnd.hWnd = CreateWindowEx(
			WS_EX_WINDOWEDGE,
			wstrClassName,
			wstrTitle.c_str(),
			user_style,
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

	// 设置窗口属性
	wnd.hParent = hParent;
	wnd.pImg = new IMAGE(w, h);
	wnd.pBufferImg = new IMAGE(w, h);
	wnd.funcWndProc = WindowProcess;
	wnd.vecMessage.reserve(MAX_MOUSEMSG_SIZE);
	wnd.nMessageIndex = 0;
	wnd.isNewSize = false;
	wnd.isSentCreateMsg = false;
	wnd.isBusyProcessing = false;
	wnd.nSkipPixels = 0;

	// 将窗口加入列表后，可能导致原先的活动窗口指针无效，需要重新定位
	vecWindows.push_back(wnd);
	if (nFocusWindowIndex != -1)
	{
		pFocusWindow = &vecWindows[nFocusWindowIndex];
	}

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

	nWndCount++;

	SetWindowPos(wnd.hWnd, HWND_TOP, 0, 0, w + nFrameW, h + nFrameH, SWP_NOMOVE);
	if (flag & EW_NOCLOSE)
	{
		HMENU hmenu = GetSystemMenu(wnd.hWnd, false);
		RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
	}

	ShowWindow(wnd.hWnd, SW_SHOWNORMAL);
	UpdateWindow(wnd.hWnd);

	*nDoneFlag = 1;

	MSG Msg;
	while (GetMessage(&Msg, NULL, NULL, NULL) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

HWND initgraph_win32(int w, int h, int flag, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent)
{
	// 标记是否已经完成窗口创建任务
	int nDoneFlag = 0;

	// 存在父窗口时，实现模态窗口
	if (hParent)
	{
		EnableWindow(hParent, false);	// 禁用父窗口
		// 该窗口被销毁后，父窗口将会被设置恢复正常
	}

	std::thread(InitWindow, w, h, flag, strWndTitle, WindowProcess, hParent, &nDoneFlag).detach();

	while (nDoneFlag == 0) { Sleep(10); };	// 等待窗口创建完成
	if (nDoneFlag == -1)
	{
		if (hParent)	// 创建子窗口失败，则须将父窗口恢复正常
		{
			EnableWindow(hParent, true);
		}
		return NULL;
	}
	else
	{
		return pFocusWindow->hWnd;
	}
}


EASY_WIN32_END

////////////****** 其他函数 ******////////////

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

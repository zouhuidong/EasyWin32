/////////////////////////////////////
//
//	EasyWin32.cpp
//
//	Ver 2.6.3
//
//

#include "EasyWin32.h"

// 最大鼠标消息积累量
//		若鼠标消息积累量达到此阈值，且消息都已经处理完，将自动清理鼠标消息积累
//		若尚未处理完所有鼠标消息，则不会自动清理鼠标消息积累
#define MAX_MOUSEMSG_SIZE 100

EASY_WIN32_BEGIN

////////////****** 全局变量 ******////////////


WNDCLASSEX g_WndClassEx;				// 窗口类
wchar_t g_lpszClassName[]				// 窗口类名
= L"EasyWin32_Class";

EasyWindow* g_pFocusWindow = NULL;		// 正操作窗口
int g_nFocusWindowIndex = -1;			// 注：该标识仅用于恢复窗口指针

std::vector<EasyWindow> g_vecWindows;	// 窗口表（管理多窗口）

bool g_bInTask = false;					// 当前是否处在任务中

int g_nSysW = 0, g_nSysH = 0;			// 系统分辨率

HWND g_hConsole;						// 控制台句柄
HINSTANCE g_hInstance;					// 程序实例

HICON g_hIconDefault;					// 默认程序图标
int g_nCustomIcon = 0;					// 自定义程序图标资源 ID，为 0 表示不使用
int g_nCustomIconSm = 0;
HICON g_hCustomIcon;					// 自定义程序图标
HICON g_hCustomIconSm;

bool g_isPreStyle = false;				// 是否预设窗口样式
bool g_isPrePos = false;				// 是否预设窗口位置
long g_lPreStyle;						// 创建窗口前的预设样式
POINT g_pPrePos;						// 创建窗口前的预设窗口位置

UINT g_uWM_TASKBARCREATED;				// 系统任务栏消息代码

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
	if (!hWnd || (g_pFocusWindow && g_pFocusWindow->hWnd == hWnd))
	{
		while (g_bInTask)
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
	for (int i = 0; i < (int)g_vecWindows.size(); i++)
	{
		if (hWnd == g_vecWindows[i].hWnd)
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
	EasyWindow* pWnd = &g_vecWindows[index];

	// 必须先保存下图像指针以供 delete，不能在 vector 移除后使用 pWnd 的图像指针
	IMAGE* img[2] = { pWnd->pImg ,pWnd->pBufferImg };

	// 释放消息列表内存
	std::vector<ExMessage>().swap(pWnd->vecMessage);

	// 删除此窗口的记录
	g_vecWindows.erase(g_vecWindows.begin() + index);

	DestroyWindow(pWnd->hWnd);
	PostQuitMessage(NULL);

	// 为避免外部调用已经析构的指针，释放内存的操作在 vector 移除元素后执行
	for (int i = 0; i < 2; i++)
	{
		delete img[i];
	}
}

// 此函数用于内部调用，按窗口索引关闭窗口
void closegraph_win32(int index)
{
	EasyWindow* pWnd = &g_vecWindows[index];

	// 若已设置父窗口为模态窗口，则需要将父窗口恢复正常
	if (pWnd->hParent != NULL)
	{
		EnableWindow(pWnd->hParent, true);
		SetForegroundWindow(pWnd->hParent);
	}

	// 卸载托盘
	DeleteTray(pWnd);

	// 防止和当前绘图任务冲突
	WaitForTask(pWnd->hWnd);
	pWnd->isBusyProcessing = true;

	// 为避免产生歧义，活动窗口必须置空
	if (pWnd == g_pFocusWindow)
	{
		g_pFocusWindow = NULL;
		g_nFocusWindowIndex = -1;
	}

	// 销毁窗口
	DelWindow(index);

	// 关闭的是最后一个被创建的窗口，则需要重置部分内容，防止外界引用出错
	size_t size = g_vecWindows.size();
	if (index == size)
	{
		pWnd->hWnd = NULL;
		pWnd->isBusyProcessing = false;
	}
}

// 此函数用于外部调用，只是向目标窗口线程发送关闭窗口消息
void closegraph_win32(HWND hWnd)
{
	// 关闭全部
	if (hWnd == NULL)
	{
		for (int i = 0; i < (int)g_vecWindows.size(); i++)
		{
			// 必须交由原线程销毁窗口，才能使窗口销毁
			// 特殊标记 wParam 为 1，表示程序命令销毁窗口
			SendMessage(g_vecWindows[i].hWnd, WM_DESTROY, 1, 0);
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
		if (g_vecWindows.empty())	// 若所有窗口都被关闭，则跳出
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
	return !g_vecWindows.empty();
}

bool isAliveWindow(HWND hWnd)
{
	if (hWnd)	return GetWindowID(hWnd) == -1 ? false : true;
	else		return g_pFocusWindow;
}

HWND GetHWnd_win32()
{
	return g_pFocusWindow ? g_pFocusWindow->hWnd : NULL;
}

EasyWindow GetWorkingWindow()
{
	return g_pFocusWindow ? *g_pFocusWindow : EasyWindow();
}

bool SetWorkingWindow(HWND hWnd)
{
	int index = GetWindowID(hWnd);
	if (index == -1)	return false;
	WaitForTask();
	WaitForProcessing(g_pFocusWindow);
	g_pFocusWindow = &g_vecWindows[index];
	g_nFocusWindowIndex = index;
	SetWorkingImage(g_pFocusWindow->pBufferImg);
	return true;
}

void QuickDraw(UINT nSkipPixels)
{
	g_pFocusWindow->nSkipPixels = nSkipPixels;
}

void EnforceRedraw()
{
	if (g_pFocusWindow)
	{
		InvalidateRect(g_pFocusWindow->hWnd, NULL, false);
	}
}

// 复制缓冲区
void FlushDrawing(EasyWindow* pWnd = g_pFocusWindow)
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
	if (!g_bInTask && g_pFocusWindow)
	{
		WaitForTask();
		WaitForProcessing(g_pFocusWindow);
		g_bInTask = true;
	}
	return g_bInTask;
}

void EndTask()
{
	if (g_bInTask)
	{
		if (g_pFocusWindow && isAliveWindow(g_pFocusWindow->hWnd))
		{
			FlushDrawing();
		}

		g_bInTask = false;
	}
}

bool isInTask(HWND hWnd)
{
	return g_bInTask && (hWnd ? g_pFocusWindow && g_pFocusWindow->hWnd == hWnd : true);
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

void ShowTray(NOTIFYICONDATA* nid)
{
	Shell_NotifyIcon(NIM_ADD, nid);
}

void CreateTray(LPCTSTR lpTrayName)
{
	static int id;
	EasyWindow* pWnd = g_pFocusWindow;

	HICON hIcon = g_hIconDefault;
	if (g_nCustomIconSm)		hIcon = g_hCustomIconSm;
	else if (g_nCustomIcon)		hIcon = g_hCustomIcon;

	pWnd->isUseTray = true;
	pWnd->nid.cbSize = sizeof(pWnd->nid);
	pWnd->nid.hWnd = pWnd->hWnd;
	pWnd->nid.uID = id++;
	pWnd->nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	pWnd->nid.uCallbackMessage = WM_TRAY;
	pWnd->nid.hIcon = hIcon;
	lstrcpy(pWnd->nid.szTip, lpTrayName);
	ShowTray(&pWnd->nid);
}

void DeleteTray(EasyWindow* pWnd)
{
	if (pWnd == NULL)	pWnd = g_pFocusWindow;
	if (pWnd->isUseTray)
	{
		pWnd->isUseTray = false;
		Shell_NotifyIcon(NIM_DELETE, &pWnd->nid);
	}
}

void SetTrayMenu(HMENU hMenu)
{
	EasyWindow* pWnd = g_pFocusWindow;
	pWnd->isUseTrayMenu = true;
	pWnd->hTrayMenu = hMenu;
}

void SetTrayMenuProcFunc(void(*pFunc)(UINT))
{
	EasyWindow* pWnd = g_pFocusWindow;
	pWnd->funcTrayMenuProc = pFunc;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	EasyWindow* pWnd = NULL;			// 当前传入窗口
	RECT rctWnd;						// 窗口矩形信息
	POINT ptMouse;						// 鼠标位置
	bool isNeedDefaultProcess = true;	// 记录是否需要使用默认方法处理消息
	int indexWnd = GetWindowID(hwnd);	// 该窗口在已记录列表中的索引
	if (indexWnd == -1)	// 出现未知窗口，则使用默认方法进行处理（这是非正常情况）
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	// 销毁窗口一定在此函数内进行，所以 pWnd 是不会突然被删除的
	pWnd = &g_vecWindows[indexWnd];

	GetWindowRect(hwnd, &rctWnd);
	GetCursorPos(&ptMouse);

	//** 开始处理窗口消息 **//

	// 由于 WM_CREATE 消息被未知原因吞噬，需要模拟发送此消息
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
		SendMessage(g_hConsole, msg, wParam, lParam);
		break;

		// 托盘消息
	case WM_TRAY:
		if (pWnd->isUseTray)
		{
			switch (lParam)
			{
				// 左键激活窗口
			case WM_LBUTTONDOWN:
				SetForegroundWindow(hwnd);
				break;

				// 右键打开菜单
			case WM_RBUTTONDOWN:
				if (pWnd->isUseTrayMenu)
				{
					SetForegroundWindow(hwnd);	// 激活一下窗口，防止菜单不消失

					// 显示菜单并跟踪
					int nMenuId = TrackPopupMenu(pWnd->hTrayMenu, TPM_RETURNCMD, ptMouse.x, ptMouse.y, NULL, hwnd, NULL);
					if (nMenuId == 0) PostMessage(hwnd, WM_LBUTTONDOWN, NULL, NULL);
					if (pWnd->funcTrayMenuProc)
					{
						pWnd->funcTrayMenuProc(nMenuId);
					}

				}
				break;

			default:
				break;
			}
		}
		break;

	case WM_DESTROY:
		// WM_DESTROY 消息本无参数，若出现参数则是程序命令销毁窗口
		if (wParam)
		{
			closegraph_win32(indexWnd);
		}
		break;

	default:
		// 系统任务栏重新创建，此时可能需要重新创建托盘
		if (msg == g_uWM_TASKBARCREATED)
		{
			if (pWnd->isUseTray)
			{
				ShowTray(&pWnd->nid);
			}
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
		if (rctWnd.left <= 0 || rctWnd.top <= 0 || rctWnd.right >= g_nSysW || rctWnd.bottom >= g_nSysH)
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
	return g_vecWindows;
}

bool isWindowSizeChanged(HWND hWnd)
{
	EasyWindow* pWnd = g_pFocusWindow;
	if (hWnd)
	{
		int id = GetWindowID(hWnd);
		if (id >= 0)	pWnd = &g_vecWindows[id];
		else			return false;
	}
	if (!pWnd)	return false;
	bool b = pWnd->isNewSize;
	pWnd->isNewSize = false;
	return b;
}

bool GetCustomIconState()
{
	return g_nCustomIcon;
}

void SetCustomIcon(int nIcon, int nIconSm)
{
	g_nCustomIcon = nIcon;
	g_nCustomIconSm = nIconSm;
	g_hCustomIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(g_nCustomIcon));
	g_hCustomIconSm = LoadIcon(g_hInstance, MAKEINTRESOURCE(g_nCustomIconSm));
}

bool MouseHit_win32(EasyWindow* pWnd)
{
	if (!pWnd)	pWnd = g_pFocusWindow;
	return pWnd && pWnd->nMessageIndex < (int)pWnd->vecMessage.size() - 1;
}

ExMessage GetMouseMsg_win32()
{
	while (!MouseHit_win32()) { HpSleep(1); };
	ExMessage msg = g_pFocusWindow->vecMessage[g_pFocusWindow->nMessageIndex];
	if (g_pFocusWindow->nMessageIndex < (int)g_pFocusWindow->vecMessage.size())
	{
		g_pFocusWindow->nMessageIndex++;
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
			*pMsg = g_pFocusWindow->vecMessage[g_pFocusWindow->nMessageIndex];
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
	g_pFocusWindow->vecMessage.clear();
	g_pFocusWindow->nMessageIndex = 0;
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

void PreSetWindowStyle(long lStyle)
{
	g_isPreStyle = true;
	g_lPreStyle = lStyle;
}

void PreSetWindowPos(int x, int y)
{
	g_isPrePos = true;
	g_pPrePos = { x,y };
}

long GetWindowStyle()
{
	return GetWindowLong(g_pFocusWindow->hWnd, GWL_STYLE);
}

long GetWindowExStyle()
{
	return GetWindowLong(g_pFocusWindow->hWnd, GWL_EXSTYLE);
}

int SetWindowStyle(long lNewStyle)
{
	return SetWindowLong(g_pFocusWindow->hWnd, GWL_STYLE, lNewStyle);
}

int SetWindowExStyle(long lNewExStyle)
{
	return SetWindowLong(g_pFocusWindow->hWnd, GWL_EXSTYLE, lNewExStyle);
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
	HICON hIcon = g_hIconDefault;
	HICON hIconSm = g_hIconDefault;
	if (g_nCustomIcon)		hIcon = g_hCustomIcon;
	if (g_nCustomIconSm)	hIconSm = g_hCustomIconSm;

	g_WndClassEx.cbSize = sizeof(WNDCLASSEX);
	g_WndClassEx.style = CS_VREDRAW | CS_HREDRAW;
	g_WndClassEx.lpfnWndProc = WndProc;
	g_WndClassEx.cbClsExtra = 0;
	g_WndClassEx.cbWndExtra = 0;
	g_WndClassEx.hInstance = g_hInstance;
	g_WndClassEx.hIcon = hIcon;
	g_WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	g_WndClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	g_WndClassEx.lpszMenuName = NULL;
	g_WndClassEx.lpszClassName = g_lpszClassName;
	g_WndClassEx.hIconSm = hIconSm;

	// 注册窗口类
	if (!RegisterClassEx(&g_WndClassEx))
	{
		std::wstring str = std::to_wstring(GetLastError());
		MessageBox(NULL, (L"Error registing window class: " + str).c_str(), L"[Error]", MB_OK | MB_ICONERROR);
		exit(-1);
	}
}

// 真正创建窗口的函数（阻塞）
void InitWindow(int w, int h, int flag, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent, int* nDoneFlag)
{
	static int nWndCount;		// 已创建窗口计数（用于生成窗口标题）
	std::wstring wstrTitle;		// 窗口标题
	EasyWindow wnd;				// 窗口信息
	int nFrameW, nFrameH;		// 窗口标题栏宽高（各个窗口可能不同）

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

	// 第一次创建窗口 --- 初始化各项数据
	if (nWndCount == 0)
	{
		// 获取分辨率
		g_nSysW = GetSystemMetrics(SM_CXSCREEN);
		g_nSysH = GetSystemMetrics(SM_CYSCREEN);

		// 默认程序图标
		g_hIconDefault = GetDefaultAppIcon();

		// 注册窗口类
		RegisterWndClass();
		g_hConsole = GetConsoleWindow();
		g_hInstance = GetModuleHandle(0);

		// 隐藏控制台
		if (g_hConsole)
		{
			ShowWindow(g_hConsole, SW_HIDE);
		}

		// 获取系统任务栏自定义的消息代码
		g_uWM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
	}

	// 控制台
	if (g_hConsole && flag & EW_SHOWCONSOLE)
	{
		ShowWindow(g_hConsole, flag & SW_NORMAL);
	}

	int user_style = WS_OVERLAPPEDWINDOW;
	if (flag & EW_NOMINIMIZE)	// 剔除最小化按钮
	{
		user_style &= ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX;
	}
	// 此方法不行，在下面处理此属性
	/*if (flag & EW_NOCLOSE)
	{
		user_style &= ~WS_SYSMENU;
	}*/
	if (flag & EW_DBLCLKS)		// 支持双击
	{
		user_style |= CS_DBLCLKS;
	}

	// 创建窗口
	for (int i = 0;; i++)
	{
		wnd.hWnd = CreateWindowEx(
			WS_EX_WINDOWEDGE,
			g_lpszClassName,
			wstrTitle.c_str(),
			g_isPreStyle ? g_lPreStyle : user_style,
			CW_USEDEFAULT, CW_USEDEFAULT,
			w, h,	// 宽高暂时这样设置，稍后获取边框大小后再调整
			hParent,
			NULL,
			g_hInstance,
			NULL
		);

		// 创建成功，跳出
		if (wnd.hWnd)
			break;

		// 三次创建窗口失败，不再尝试
		else if (i == 2)
		{
			std::wstring str = std::to_wstring(GetLastError());
			MessageBox(NULL, (L"Error creating window: " + str).c_str(), L"[Error]", MB_OK | MB_ICONERROR);
			*nDoneFlag = -1;
			return;
		}
	}

	// 剔除关闭按钮
	if (flag & EW_NOCLOSE)
	{
		HMENU hmenu = GetSystemMenu(wnd.hWnd, false);
		RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
	}

	// 初始化窗口属性
	wnd.hParent = hParent;
	wnd.pImg = new IMAGE(w, h);
	wnd.pBufferImg = new IMAGE(w, h);
	wnd.funcWndProc = WindowProcess;
	wnd.vecMessage.reserve(MAX_MOUSEMSG_SIZE);
	wnd.nMessageIndex = 0;
	wnd.isUseTray = false;
	wnd.nid = { 0 };
	wnd.isUseTrayMenu = false;
	wnd.hTrayMenu = NULL;
	wnd.funcTrayMenuProc = NULL;
	wnd.isNewSize = false;
	wnd.isSentCreateMsg = false;
	wnd.isBusyProcessing = false;
	wnd.nSkipPixels = 0;

	// 将窗口加入列表后，可能导致原先的活动窗口指针无效，需要重新定位
	g_vecWindows.push_back(wnd);
	if (g_nFocusWindowIndex != -1)
	{
		g_pFocusWindow = &g_vecWindows[g_nFocusWindowIndex];
	}

	// 抢夺焦点
	SetWorkingWindow(wnd.hWnd);

	// 窗口创建完毕
	nWndCount++;

	// 注意：
	//	必须在显示窗口前标记已经完成创建窗口。
	//	因为可以在自定义过程函数中创建子窗口，若是不在显示窗口前标记窗口创建完成，
	//	就会导致父窗口过程函数阻塞，接下来显示窗口就会阻塞，进而导致整个窗口假死。
	*nDoneFlag = 1;		

	//** 显示窗口等后续处理 **//

	// 获取边框大小，补齐绘图区大小
	RECT rcClient, rcWnd;
	GetClientRect(wnd.hWnd, &rcClient);
	GetWindowRect(wnd.hWnd, &rcWnd);
	nFrameW = (rcWnd.right - rcWnd.left) - rcClient.right;
	nFrameH = (rcWnd.bottom - rcWnd.top) - rcClient.bottom;

	int px = 0, py = 0;
	if (g_isPrePos)
	{
		px = g_pPrePos.x;
		py = g_pPrePos.y;
	}
	SetWindowPos(
		wnd.hWnd,
		HWND_TOP,
		px, py,
		w + nFrameW, h + nFrameH,
		g_isPrePos ? 0 : SWP_NOMOVE
	);

	g_isPreStyle = false;
	g_isPrePos = false;

	ShowWindow(wnd.hWnd, SW_SHOWNORMAL);
	UpdateWindow(wnd.hWnd);

	// 消息派发，阻塞
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
		return g_pFocusWindow->hWnd;
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

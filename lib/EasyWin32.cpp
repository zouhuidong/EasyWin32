/////////////////////////////////////
//
//	EasyWin32.cpp
//	
//	��ϸ��Ϣ�� EasyWin32.h
//

#include "EasyWin32.h"

// ��������Ϣ������
//		�������Ϣ�������ﵽ����ֵ������Ϣ���Ѿ������꣬���Զ����������Ϣ����
//		����δ���������������Ϣ���򲻻��Զ����������Ϣ����
#define MAX_MOUSEMSG_SIZE 100

EASY_WIN32_BEGIN

////////////****** ȫ�ֱ��� ******////////////


WNDCLASSEX g_WndClassEx;				// ������
wchar_t g_lpszClassName[]				// ��������
= L"EasyWin32_Class";

EasyWindow* g_pFocusWindow = NULL;		// ����������
int g_nFocusWindowIndex = -1;			// ע���ñ�ʶ�����ڻָ�����ָ��

std::vector<EasyWindow> g_vecWindows;	// ���ڱ�����ര�ڣ�

bool g_bInTask = false;					// ��ǰ�Ƿ���������

int g_nSysW = 0, g_nSysH = 0;			// ϵͳ�ֱ���

HWND g_hConsole;						// ����̨���
HINSTANCE g_hInstance;					// ����ʵ��

HICON g_hIconDefault;					// Ĭ�ϳ���ͼ��
int g_nCustomIcon = 0;					// �Զ������ͼ����Դ ID��Ϊ 0 ��ʾ��ʹ��
int g_nCustomIconSm = 0;
HICON g_hCustomIcon;					// �Զ������ͼ��
HICON g_hCustomIconSm;

bool g_isPreStyle = false;				// �Ƿ�Ԥ�贰����ʽ
bool g_isPrePos = false;				// �Ƿ�Ԥ�贰��λ��
long g_lPreStyle;						// ��������ǰ��Ԥ����ʽ
POINT g_pPrePos;						// ��������ǰ��Ԥ�贰��λ��

UINT g_uWM_TASKBARCREATED;				// ϵͳ��������Ϣ����

////////////****** �������� ******////////////

// �������� EasyX �е�������ʾ��Ŀ�괰����
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

// ͨ�������ô˴����ڴ��ڼ�¼���е�����
// δ�ҵ����� -1
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

// ɾ�����ڣ��ͷ��ڴ�
void DelWindow(int index)
{
	EasyWindow* pWnd = &g_vecWindows[index];

	// �����ȱ�����ͼ��ָ���Թ� delete�������� vector �Ƴ���ʹ�� pWnd ��ͼ��ָ��
	IMAGE* img[2] = { pWnd->pImg ,pWnd->pBufferImg };

	// �ͷ���Ϣ�б��ڴ�
	std::vector<ExMessage>().swap(pWnd->vecMessage);

	// ɾ���˴��ڵļ�¼
	g_vecWindows.erase(g_vecWindows.begin() + index);

	DestroyWindow(pWnd->hWnd);
	PostQuitMessage(NULL);

	// Ϊ�����ⲿ�����Ѿ�������ָ�룬�ͷ��ڴ�Ĳ����� vector �Ƴ�Ԫ�غ�ִ��
	for (int i = 0; i < 2; i++)
	{
		delete img[i];
	}
}

// �˺��������ڲ����ã������������رմ���
void closegraph_win32(int index)
{
	EasyWindow* pWnd = &g_vecWindows[index];

	// �������ø�����Ϊģ̬���ڣ�����Ҫ�������ڻָ�����
	if (pWnd->hParent != NULL)
	{
		EnableWindow(pWnd->hParent, true);
		SetForegroundWindow(pWnd->hParent);
	}

	// ж������
	DeleteTray(pWnd);

	// ��ֹ�͵�ǰ��ͼ�����ͻ
	WaitForTask(pWnd->hWnd);
	pWnd->isBusyProcessing = true;

	// Ϊ����������壬����ڱ����ÿ�
	if (pWnd == g_pFocusWindow)
	{
		g_pFocusWindow = NULL;
		g_nFocusWindowIndex = -1;
	}

	// ���ٴ���
	DelWindow(index);

	// �رյ������һ���������Ĵ��ڣ�����Ҫ���ò������ݣ���ֹ������ó���
	size_t size = g_vecWindows.size();
	if (index == size)
	{
		pWnd->hWnd = NULL;
		pWnd->isBusyProcessing = false;
	}
}

// �˺��������ⲿ���ã�ֻ����Ŀ�괰���̷߳��͹رմ�����Ϣ
void closegraph_win32(HWND hWnd)
{
	// �ر�ȫ��
	if (hWnd == NULL)
	{
		for (int i = 0; i < (int)g_vecWindows.size(); i++)
		{
			// ���뽻��ԭ�߳����ٴ��ڣ�����ʹ��������
			// ������ wParam Ϊ 1����ʾ�����������ٴ���
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
		if (g_vecWindows.empty())	// �����д��ڶ����رգ�������
		{
			return;
		}

		Sleep(100);
	}
}

// �Զ��˳������ʵ��ִ�к���
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

// ���ƻ�����
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

// ���ݴ��ڴ�С���µ����ô��ڻ�����С
void ResizeWindowImage(EasyWindow* pWnd)
{
	RECT rctWnd;
	for (int i = 0; i < 2; i++)
	{
		if (GetClientRect(pWnd->hWnd, &rctWnd))	// �ͻ�������
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
	EasyWindow* pWnd = NULL;			// ��ǰ���봰��
	RECT rctWnd;						// ���ھ�����Ϣ
	POINT ptMouse;						// ���λ��
	bool isNeedDefaultProcess = true;	// ��¼�Ƿ���Ҫʹ��Ĭ�Ϸ���������Ϣ
	int indexWnd = GetWindowID(hwnd);	// �ô������Ѽ�¼�б��е�����
	if (indexWnd == -1)	// ����δ֪���ڣ���ʹ��Ĭ�Ϸ������д������Ƿ����������
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	// ���ٴ���һ���ڴ˺����ڽ��У����� pWnd �ǲ���ͻȻ��ɾ����
	pWnd = &g_vecWindows[indexWnd];

	GetWindowRect(hwnd, &rctWnd);
	GetCursorPos(&ptMouse);

	//** ��ʼ��������Ϣ **//

	// ���� WM_CREATE ��Ϣ��δ֪ԭ�����ɣ���Ҫģ�ⷢ�ʹ���Ϣ
	if (!pWnd->isSentCreateMsg)
	{
		pWnd->isSentCreateMsg = true;
		WndProc(hwnd, WM_CREATE, NULL, NULL);
	}

	// ����Ԥ�ȴ����һЩ��Ϣ
	switch (msg)
	{
	case WM_CREATE:
		break;

	case WM_SIZE:
		// ��ֹ�͵�ǰ��ͼ��������ͻ
		WaitForTask(pWnd->hWnd);
		pWnd->isBusyProcessing = true;
		ResizeWindowImage(pWnd);
		pWnd->isBusyProcessing = false;
		break;

		// �����Ϣ
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
		// ��Ϣ���Ѿ������꣬�ҶѴ���Ϣ�����ﵽ��ֵ
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

		// ��¼
		pWnd->vecMessage.push_back(msgMouse);
	}
	break;

	// ������Ϣ˦��������̨��ʵ�ֶ԰�����Ϣ��֧��
	case WM_KEYDOWN: case WM_KEYUP: case WM_CHAR:
		SendMessage(g_hConsole, msg, wParam, lParam);
		break;

		// ������Ϣ
	case WM_TRAY:
		if (pWnd->isUseTray)
		{
			switch (lParam)
			{
				// ��������
			case WM_LBUTTONDOWN:
				SetForegroundWindow(hwnd);
				break;

				// �Ҽ��򿪲˵�
			case WM_RBUTTONDOWN:
				if (pWnd->isUseTrayMenu)
				{
					SetForegroundWindow(hwnd);	// ����һ�´��ڣ���ֹ�˵�����ʧ

					// ��ʾ�˵�������
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
		// WM_DESTROY ��Ϣ���޲����������ֲ������ǳ����������ٴ���
		if (wParam)
		{
			closegraph_win32(indexWnd);
		}
		break;

	default:
		// ϵͳ���������´�������ʱ������Ҫ���´�������
		if (msg == g_uWM_TASKBARCREATED)
		{
			if (pWnd->isUseTray)
			{
				ShowTray(&pWnd->nid);
			}
		}
		break;
	}

	// ���ж�������Ϣ�����������
	if (pWnd->funcWndProc)
	{
		isNeedDefaultProcess = pWnd->funcWndProc(hwnd, msg, wParam, lParam, GetModuleHandle(0));
	}

	// �������û�������Ϣ������һЩ��������
	switch (msg)
	{
	case WM_PAINT:
		FlushDrawingToWnd(pWnd->pImg, pWnd->hWnd);
		DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_MOVE:
		// �����ƶ����ڳ�����Ļ�Ļ����ܵ����Ӵ�����ʾ�����⣬���Դ�ʱ��Ҫ�����ػ�
		if (rctWnd.left <= 0 || rctWnd.top <= 0 || rctWnd.right >= g_nSysW || rctWnd.bottom >= g_nSysH)
		{
			InvalidateRect(hwnd, NULL, false);
		}
		break;
	}

	// ����ʹ��Ĭ�Ϸ�����������
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

// ��ȡĬ�ϴ���ͼ��
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

	// ע�ᴰ����
	if (!RegisterClassEx(&g_WndClassEx))
	{
		std::wstring str = std::to_wstring(GetLastError());
		MessageBox(NULL, (L"Error registing window class: " + str).c_str(), L"[Error]", MB_OK | MB_ICONERROR);
		exit(-1);
	}
}

// �����������ڵĺ�����������
void InitWindow(int w, int h, int flag, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent, int* nDoneFlag)
{
	static int nWndCount;		// �Ѵ������ڼ������������ɴ��ڱ��⣩
	std::wstring wstrTitle;		// ���ڱ���
	EasyWindow wnd;				// ������Ϣ
	int nFrameW, nFrameH;		// ���ڱ�������ߣ��������ڿ��ܲ�ͬ��

	// δ���ñ���
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

	// ��һ�δ������� --- ��ʼ����������
	if (nWndCount == 0)
	{
		// ��ȡ�ֱ���
		g_nSysW = GetSystemMetrics(SM_CXSCREEN);
		g_nSysH = GetSystemMetrics(SM_CYSCREEN);

		// Ĭ�ϳ���ͼ��
		g_hIconDefault = GetDefaultAppIcon();

		// ע�ᴰ����
		RegisterWndClass();
		g_hConsole = GetConsoleWindow();
		g_hInstance = GetModuleHandle(0);

		// ���ؿ���̨
		if (g_hConsole)
		{
			ShowWindow(g_hConsole, SW_HIDE);
		}

		// ��ȡϵͳ�������Զ������Ϣ����
		g_uWM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
	}

	// ����̨
	if (g_hConsole && flag & EW_SHOWCONSOLE)
	{
		ShowWindow(g_hConsole, flag & SW_NORMAL);
	}

	int user_style = WS_OVERLAPPEDWINDOW;
	if (flag & EW_NOMINIMIZE)	// �޳���С����ť
	{
		user_style &= ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX;
	}
	// �˷������У������洦�������
	/*if (flag & EW_NOCLOSE)
	{
		user_style &= ~WS_SYSMENU;
	}*/
	if (flag & EW_DBLCLKS)		// ֧��˫��
	{
		user_style |= CS_DBLCLKS;
	}

	// ��������
	for (int i = 0;; i++)
	{
		wnd.hWnd = CreateWindowEx(
			WS_EX_WINDOWEDGE,
			g_lpszClassName,
			wstrTitle.c_str(),
			g_isPreStyle ? g_lPreStyle : user_style,
			CW_USEDEFAULT, CW_USEDEFAULT,
			w, h,	// �����ʱ�������ã��Ժ��ȡ�߿��С���ٵ���
			hParent,
			NULL,
			g_hInstance,
			NULL
		);

		// �����ɹ�������
		if (wnd.hWnd)
			break;

		// ���δ�������ʧ�ܣ����ٳ���
		else if (i == 2)
		{
			std::wstring str = std::to_wstring(GetLastError());
			MessageBox(NULL, (L"Error creating window: " + str).c_str(), L"[Error]", MB_OK | MB_ICONERROR);
			*nDoneFlag = -1;
			return;
		}
	}

	// �޳��رհ�ť
	if (flag & EW_NOCLOSE)
	{
		HMENU hmenu = GetSystemMenu(wnd.hWnd, false);
		RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
	}

	// ��ʼ����������
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

	// �����ڼ����б�󣬿��ܵ���ԭ�ȵĻ����ָ����Ч����Ҫ���¶�λ
	g_vecWindows.push_back(wnd);
	if (g_nFocusWindowIndex != -1)
	{
		g_pFocusWindow = &g_vecWindows[g_nFocusWindowIndex];
	}

	// ���ό��
	SetWorkingWindow(wnd.hWnd);

	// ���ڴ������
	nWndCount++;
	g_isPreStyle = false;
	g_isPrePos = false;

	// ע�⣺
	//	��������ʾ����ǰ����Ѿ���ɴ������ڡ�
	//	��Ϊ�������Զ�����̺����д����Ӵ��ڣ����ǲ�����ʾ����ǰ��Ǵ��ڴ�����ɣ�
	//	�ͻᵼ�¸����ڹ��̺�����������������ʾ���ھͻ����������������������ڼ�����
	*nDoneFlag = 1;		

	//** ��ʾ���ڵȺ������� **//

	// ��ȡ�߿��С�������ͼ����С
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

	ShowWindow(wnd.hWnd, SW_SHOWNORMAL);
	UpdateWindow(wnd.hWnd);

	// ��Ϣ�ɷ�������
	MSG Msg;
	while (GetMessage(&Msg, NULL, NULL, NULL) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

HWND initgraph_win32(int w, int h, int flag, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent)
{
	// ����Ƿ��Ѿ���ɴ��ڴ�������
	int nDoneFlag = 0;

	// ���ڸ�����ʱ��ʵ��ģ̬����
	if (hParent)
	{
		EnableWindow(hParent, false);	// ���ø�����
		// �ô��ڱ����ٺ󣬸����ڽ��ᱻ���ûָ�����
	}

	std::thread(InitWindow, w, h, flag, strWndTitle, WindowProcess, hParent, &nDoneFlag).detach();

	while (nDoneFlag == 0) { Sleep(10); };	// �ȴ����ڴ������
	if (nDoneFlag == -1)
	{
		if (hParent)	// �����Ӵ���ʧ�ܣ����뽫�����ڻָ�����
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

////////////****** �������� ******////////////

void HpSleep(int ms)
{
	static clock_t oldclock = clock();		// ��̬��������¼��һ�� tick

	oldclock += ms * CLOCKS_PER_SEC / 1000;	// ���� tick

	if (clock() > oldclock)					// ����Ѿ���ʱ��������ʱ
		oldclock = clock();
	else
		while (clock() < oldclock)			// ��ʱ
			Sleep(1);						// �ͷ� CPU ����Ȩ������ CPU ռ����
//			Sleep(0);						// ���߾��ȡ����� CPU ռ����
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

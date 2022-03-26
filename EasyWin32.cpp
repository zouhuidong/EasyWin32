/////////////////////////////////////
//
//	EasyWin32.cpp
//	
//	��ϸ��Ϣ�� EasyWin32.h
//

#include "EasyWin32.h"
#include <thread>

// ��������Ϣ������
//		�������Ϣ�������ﵽ����ֵ������Ϣ���Ѿ������꣬���Զ����������Ϣ����
//		����δ���������������Ϣ���򲻻��Զ����������Ϣ����
#define MAX_MOUSEMSG_SIZE 100

EASY_WIN32_BEGIN

////////////****** ȫ�ֱ��� ******////////////

// ������
WNDCLASSEX WndClassEx;

// ��������
wchar_t wstrClassName[] = L"EasyWin32_Class";

// ����������
EasyWindow* pFocusWindow = NULL;

// ��ͼ�����Ƿ�æµ��
bool isInTask = false;

// ���ڱ������ܴ����˶�����ڣ�
std::vector<EasyWindow> vecWindows;

// ϵͳ�ֱ���
int nSysW = 0, nSysH = 0;

// ϵͳ����������
int nFrameW = 0, nFrameH = 0;

// ����̨���
HWND hConsole;

// �Ƿ�ʹ���Զ���ͼ��
bool isUseCustomAppIcon = false;

////////////****** �������� ******////////////

// �������� EasyX �е�������ʾ��Ŀ�괰����
void FlushDrawingToWnd(IMAGE* pImg, HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	HDC hdcImg = GetImageHDC(pImg);
	RECT rctWnd;
	GetClientRect(hWnd, &rctWnd);
	BitBlt(hdc, 0, 0, rctWnd.right, rctWnd.bottom, hdcImg, 0, 0, SRCCOPY);
}

// �ȴ���ǰ���������
void WaitForTask()
{
	while (isInTask)
	{
		HpSleep(1);
	}
}

// ͨ�������ô˴����ڴ��ڼ�¼���е�����
// δ�ҵ����� -1
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

// ɾ�����ڣ��ͷ��ڴ�
void DelWindow(int index)
{
	EasyWindow* pWnd = &vecWindows[index];

	delete pWnd->pImg;
	delete pWnd->pBufferImg;
	pWnd->pImg = NULL;
	pWnd->pBufferImg = NULL;

	DestroyWindow(pWnd->hWnd);
	PostQuitMessage(NULL);

	// ����ô������Ӵ��ڣ�����������ģ̬���ڣ���Ҫ�������ڻָ�����
	if (pWnd->hParent != NULL)
	{
		EnableWindow(pWnd->hParent, true);
		SetForegroundWindow(pWnd->hParent);
	}

	// ɾ���˴��ڵļ�¼
	vecWindows.erase(vecWindows.begin() + index);
}

// �������������йرմ���
void closegraph_win32(int index)
{
	EasyWindow* pWnd = &vecWindows[index];

	// ��ֹ�͵�ǰ��ͼ�����ͻ
	if (pWnd == pFocusWindow)
	{
		WaitForTask();
		isInTask = true;
		DelWindow(index);
		isInTask = false;
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
		int index = GetWindowID(hWnd);
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
		if (vecWindows.empty())	// �����д��ڶ����رգ�������
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
	return !vecWindows.empty();
}

bool isInListWindow(HWND hWnd)
{
	return GetWindowID(hWnd) == -1 ? false : true;
}

bool isAliveWindow(HWND hWnd)
{
	return isInListWindow(hWnd);
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
	int index = GetWindowID(hWnd);
	if (index == -1)
	{
		return false;
	}
	pFocusWindow = &vecWindows[index];
	WaitForTask();
	SetWorkingImage(pFocusWindow->pBufferImg);
	return true;
}

void EnforceRedraw()
{
	InvalidateRect(pFocusWindow->hWnd, NULL, false);
}

void FlushDrawing()
{
	*pFocusWindow->pImg = *pFocusWindow->pBufferImg;
}

void BeginTask()
{
	isInTask = true;
}

void EndTask()
{
	FlushDrawing();
	isInTask = false;
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// ��ǰ���봰��
	EasyWindow* pWnd = NULL;
	bool isNeedDefaultProcess = true;		// ��¼�Ƿ���Ҫʹ��Ĭ�Ϸ���������Ϣ
	int indexWnd = GetWindowID(hwnd);	// �ô������Ѽ�¼�б��е�����
	if (indexWnd == -1)	// ����δ֪���ڣ���ʹ��Ĭ�Ϸ������д����������������˵������֣�
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	pWnd = &vecWindows[indexWnd];
	RECT rctWnd;
	GetWindowRect(hwnd, &rctWnd);		// ���ھ�����Ϣ

	//** ��ʼ����������Ϣ **//

	// ���� WM_CREATE ��Ϣ�����ɣ���Ҫģ�ⷢ�ʹ���Ϣ
	if (!pWnd->isSentCreateMsg)
	{
		pWnd->isSentCreateMsg = true;
		WndProc(hwnd, WM_CREATE, NULL, NULL);
	}

	// ����Ԥ�ȴ�����һЩ��Ϣ
	switch (msg)
	{
	case WM_CREATE:
		break;

	case WM_SIZE:
		if (pWnd == pFocusWindow)
		{
			WaitForTask();		// ��ֹ�͵�ǰ��ͼ��������ͻ
			isInTask = true;
			ResizeWindowImage(pWnd);
			isInTask = false;
		}
		else
		{
			ResizeWindowImage(pWnd);
		}
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

		// ��¼
		pWnd->vecMouseMsg.push_back(msgMouse);
	}
	break;

	// ������Ϣ˦��������̨��ʵ�ֶ԰�����Ϣ��֧��
	case WM_KEYDOWN: case WM_KEYUP: case WM_CHAR:
		SendMessage(hConsole, msg, wParam, lParam);
		break;

	}

	// ���ж�������Ϣ�������������
	if (pWnd->funcWndProc)
	{
		isNeedDefaultProcess = pWnd->funcWndProc(hwnd, msg, wParam, lParam, GetModuleHandle(0));
	}

	// �������û�������Ϣ������һЩ��������
	switch (msg)
	{
	case WM_PAINT:
		// �Զ������ͼ���棬���ٴ��ػ洰��
		FlushDrawingToWnd(pWnd->pImg, pWnd->hWnd);
		DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_MOVE:
		// �����ƶ����ڳ�����Ļ�Ļ����ܵ����Ӵ�����ʾ�����⣬���Դ�ʱ��Ҫ�����ػ�
		if (rctWnd.left <= 0 || rctWnd.top <= 0 || rctWnd.right >= nSysW || rctWnd.bottom >= nSysH)
		{
			InvalidateRect(hwnd, NULL, false);
		}
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

long GetWindowStyle()
{
	return GetWindowLong(pFocusWindow->hWnd, GWL_STYLE);
}

int SetWindowStyle(long lNewStyle)
{
	return SetWindowLong(pFocusWindow->hWnd, GWL_STYLE, lNewStyle);
}

// ��ȡĬ�ϴ���ͼ��
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

	HICON hIcon;	// ����ͼ��
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

	// ע�ᴰ����
	if (!RegisterClassEx(&WndClassEx))
	{
		std::wstring str = std::to_wstring(GetLastError());
		MessageBox(NULL, (L"Error registing window class: " + str).c_str(), L"[Error]", MB_OK | MB_ICONERROR);
		exit(-1);
	}
}

// �����������ڵĺ�����������
void InitWindow(int w, int h, bool isCmd, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent, int* nDoneFlag)
{
	static int nWndCount;		// ���ڼ���
	std::wstring wstrTitle;	// ���ڱ���

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

	// ��һ�δ�������
	if (nWndCount == 0)
	{
		// ��ȡ�ֱ���
		nSysW = GetSystemMetrics(SM_CXSCREEN);
		nSysH = GetSystemMetrics(SM_CYSCREEN);

		// ע�ᴰ����
		RegisterWndClass();
		hConsole = GetConsoleWindow();
	}

	// ����̨
	if (hConsole)
	{
		ShowWindow(hConsole, isCmd ? SW_NORMAL : SW_HIDE);
	}

	// ��ʼ��������Ϣ
	EasyWindow wnd;

	// ��������
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

		// ���δ�������ʧ�ܣ����ٳ���
		else if (i == 2)
		{
			std::wstring str = std::to_wstring(GetLastError());
			MessageBox(NULL, (L"Error creating window: " + str).c_str(), L"[Error]", MB_OK | MB_ICONERROR);
			*nDoneFlag = -1;
			return;
		}
	}

	wnd.hParent = hParent;
	wnd.pImg = new IMAGE(w, h);
	wnd.pBufferImg = new IMAGE(w, h);
	wnd.funcWndProc = WindowProcess;
	wnd.vecMouseMsg.reserve(MAX_MOUSEMSG_SIZE);
	wnd.nGetMouseMsgIndex = 0;
	wnd.isNewSize = false;
	wnd.isSentCreateMsg = false;

	vecWindows.push_back(wnd);
	SetWorkingWindow(wnd.hWnd);

	// ��ȡ�߿��С�������ͼ����С
	if (nWndCount == 0)
	{
		RECT rcClient, rcWnd;
		GetClientRect(wnd.hWnd, &rcClient);
		GetWindowRect(wnd.hWnd, &rcWnd);
		nFrameW = (rcWnd.right - rcWnd.left) - rcClient.right;
		nFrameH = (rcWnd.bottom - rcWnd.top) - rcClient.bottom;
	}

	nWndCount++;
	*nDoneFlag = 1;

	SetWindowPos(wnd.hWnd, HWND_TOP, 0, 0, w + nFrameW, h + nFrameH, SWP_NOMOVE);

	ShowWindow(wnd.hWnd, SW_SHOWNORMAL);
	UpdateWindow(wnd.hWnd);

	MSG Msg;
	while (GetMessage(&Msg, NULL, NULL, NULL) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

HWND initgraph_win32(int w, int h, bool isCmd, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent)
{
	// ����Ƿ��Ѿ���ɴ��ڴ�������
	int nDoneFlag = 0;

	// ���ڸ�����ʱ��ʵ��ģ̬����
	if (hParent)
	{
		EnableWindow(hParent, false);	// ���ø�����
		// �ô��ڱ����ٺ󣬸����ڽ��ᱻ���ûָ�����
	}

	std::thread(InitWindow, w, h, isCmd, strWndTitle, WindowProcess, hParent, &nDoneFlag).detach();
	while (nDoneFlag == 0) { Sleep(10); };	// �ȴ����ڴ������
	if (nDoneFlag == -1)	return NULL;
	else					return pFocusWindow->hWnd;
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
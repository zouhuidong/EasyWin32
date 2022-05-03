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

// ������
WNDCLASSEX WndClassEx;

// ��������
wchar_t wstrClassName[] = L"EasyWin32_Class";

// ����������
EasyWindow* pFocusWindow = NULL;
int nFocusWindowIndex = -1;		// ע���ñ�ʶ�����ڻָ�����ָ��

// ���ڱ����ܴ����˶�����ڣ�
std::vector<EasyWindow> vecWindows;

// ϵͳ�ֱ���
int nSysW = 0, nSysH = 0;

// ϵͳ���������
int nFrameW = 0, nFrameH = 0;

// ����̨���
HWND hConsole;

// �Զ������ͼ�꣬Ϊ 0 ��ʾ��ʹ��
int nCustomIcon = 0;
int nCustomIconSm = 0;

// ��ǰ�Ƿ���������
bool bInTask = false;

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

	// �����ȱ�����ͼ��ָ���Թ� delete�������� vector �Ƴ���ʹ�� pWnd ��ͼ��ָ��
	IMAGE* img[2] = { pWnd->pImg ,pWnd->pBufferImg };

	// �ͷ���Ϣ�б��ڴ�
	std::vector<ExMessage>().swap(pWnd->vecMessage);

	// ɾ���˴��ڵļ�¼
	vecWindows.erase(vecWindows.begin() + index);

	DestroyWindow(pWnd->hWnd);
	PostQuitMessage(NULL);

	// Ϊ�����ⲿ�����Ѿ�������ָ�룬�ͷ��ڴ�Ĳ����� vector �Ƴ�Ԫ�غ�ִ��
	for (int i = 0; i < 2; i++)
	{
		delete img[i];
	}
}

// �������������йرմ���
void closegraph_win32(int index)
{
	EasyWindow* pWnd = &vecWindows[index];

	// �������ø�����Ϊģ̬���ڣ�����Ҫ�������ڻָ�����
	if (pWnd->hParent != NULL)
	{
		EnableWindow(pWnd->hParent, true);
		SetForegroundWindow(pWnd->hParent);
	}

	// ��ֹ�͵�ǰ��ͼ�����ͻ
	WaitForTask(pWnd->hWnd);
	pWnd->isBusyProcessing = true;

	// Ϊ����������壬����ڱ����ÿ�
	if (pWnd == pFocusWindow)
	{
		pFocusWindow = NULL;
		nFocusWindowIndex = -1;
	}

	DelWindow(index);

	// �رյ������һ���������Ĵ��ڣ�����Ҫ���ò������ݣ���ֹ������ó���
	size_t size = vecWindows.size();
	if (index == size)
	{
		pWnd->hWnd = NULL;
		pWnd->isBusyProcessing = false;
	}
}

void closegraph_win32(HWND hWnd)
{
	// �ر�ȫ��
	if (hWnd == NULL)
	{
		for (int i = 0; i < (int)vecWindows.size(); i++)
		{
			// ���뽻��ԭ�߳����ٴ��ڣ�����ʹ��������
			// ������ wParam Ϊ 1����ʾ�����������ٴ���
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

// ���ƻ�����
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
	if (indexWnd == -1)	// ����δ֪���ڣ���ʹ��Ĭ�Ϸ������д��������������˵������֣�
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	pWnd = &vecWindows[indexWnd];
	RECT rctWnd;
	GetWindowRect(hwnd, &rctWnd);		// ���ھ�����Ϣ

	//** ��ʼ��������Ϣ **//

	// ���� WM_CREATE ��Ϣ�����ɣ���Ҫģ�ⷢ�ʹ���Ϣ
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
		SendMessage(hConsole, msg, wParam, lParam);
		break;

	case WM_DESTROY:
		// WM_DESTROY ��Ϣ���޲����������ֲ������ǳ����������ٴ���
		if (wParam)
		{
			closegraph_win32(indexWnd);
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
		if (rctWnd.left <= 0 || rctWnd.top <= 0 || rctWnd.right >= nSysW || rctWnd.bottom >= nSysH)
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

	// ע�ᴰ����
	if (!RegisterClassEx(&WndClassEx))
	{
		std::wstring str = std::to_wstring(GetLastError());
		MessageBox(NULL, (L"Error registing window class: " + str).c_str(), L"[Error]", MB_OK | MB_ICONERROR);
		exit(-1);
	}
}

// �����������ڵĺ�����������
void InitWindow(int w, int h, int flag, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent, int* nDoneFlag)
{
	static int nWndCount;		// ���ڼ���
	std::wstring wstrTitle;		// ���ڱ���
	EasyWindow wnd;				// ������Ϣ

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

		if (hConsole)
		{
			ShowWindow(hConsole, SW_HIDE);
		}
	}

	// ����̨
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

	// ��������
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

		// ���δ�������ʧ�ܣ����ٳ���
		else if (i == 2)
		{
			std::wstring str = std::to_wstring(GetLastError());
			MessageBox(NULL, (L"Error creating window: " + str).c_str(), L"[Error]", MB_OK | MB_ICONERROR);
			*nDoneFlag = -1;
			return;
		}
	}

	// ���ô�������
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

	// �����ڼ����б�󣬿��ܵ���ԭ�ȵĻ����ָ����Ч����Ҫ���¶�λ
	vecWindows.push_back(wnd);
	if (nFocusWindowIndex != -1)
	{
		pFocusWindow = &vecWindows[nFocusWindowIndex];
	}

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
		return pFocusWindow->hWnd;
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

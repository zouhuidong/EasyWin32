/////////////////////////////////////
//
//	EasyWin32.cpp
//	
//	��ϸ��Ϣ�� EasyWin32.h
//

#include "EasyWin32.h"
#include "AHGraphics.h"
#include <thread>

EASY_WIN32_BEGIN

// ������
WNDCLASSEX WndClassEx;

// ��������
LPCTSTR pszClassName = L"EasyX_Win32_Class";

// ����������
EasyWindow* pFocusWindow = NULL;

// ��ͼ�����Ƿ�æµ��
bool isBusyDrawing = false;

// ���ڱ����ܴ����˶�����ڣ�
std::vector<EasyWindow> vecWindows;

// ϵͳ�ֱ���
int sysW = 0, sysH = 0;

// ͨ�������ô˴����ڴ��ڼ�¼���е�����
// δ�ҵ����� -1
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

// �������������йرմ���
void closegraph_win32(int index)
{
	EasyWindow* pWnd = &vecWindows[index];

	delete pWnd->pImg;
	delete pWnd->pBufferImg;
	pWnd->pImg = NULL;
	pWnd->pBufferImg = NULL;

	DestroyWindow(pWnd->hWnd);
	PostQuitMessage(NULL);

	// ɾ���˴��ڵļ�¼
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
		if (vecWindows.empty())	// �����д��ڶ����رգ�������
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
	while (isBusyDrawing) { Sleep(10); };
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
	isBusyDrawing = false;
	*pFocusWindow->pImg = *pFocusWindow->pBufferImg;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// ��ǰ���봰��
	EasyWindow* pWnd = NULL;
	bool isNeedDefaultProcess = true;		// ��¼�Ƿ���Ҫʹ��Ĭ�Ϸ���������Ϣ
	int indexWnd = GetWindowByHWND(hwnd);	// �ô������Ѽ�¼�б��е�����
	if (indexWnd == -1)	// ����δ֪���ڣ���ʹ��Ĭ�Ϸ������д��������������˵������֣�
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	pWnd = &vecWindows[indexWnd];
	RECT rctWnd = GetWindowRect(hwnd);		// ���ھ�����Ϣ

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

		// �Ըô��ڶ�Ӧ�Ļ������е���
		POINT pWndSize = GetWindowSize(hwnd);
		pWnd->pImg->Resize(pWndSize.x, pWndSize.y);
		pWnd->pBufferImg->Resize(pWndSize.x, pWndSize.y);

		pWnd->isNewSize = true;

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
		// �Զ������ͼ���棬���ٴ��ػ洰��
		FlushDrawingToWnd(pWnd->pImg, pWnd->hWnd);
		DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_MOVE:
		// �����ƶ����ڳ�����Ļ�Ļ����ܵ����Ӵ�����ʾ�����⣬���Դ�ʱ��Ҫ�����ػ�
		if (rctWnd.left <= 0 || rctWnd.top <= 0 || rctWnd.right >= sysW || rctWnd.bottom >= sysH)
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

	// ��������Ϣ�Ͱ����Ȼ�ȡ�������ȡ����һ��
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
	// ��д�ṹ��
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

	// ע�ᴰ����
	RegisterClassEx(&WndClassEx);
}

// �����������ڵĺ���
void InitWindow(int w, int h, bool isCmd, LPCTSTR strWndTitle, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE), HWND hParent, bool* isDone)
{
	static int nWndNum;		// ���ڼ���
	std::wstring wstrTitle;	// ���ڱ���

	// δ���ñ���
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

	// ����̨
	HWND hConsole = GetConsoleWindow();
	if (hConsole)
	{
		if (!isCmd)
		{
			ShowWindow(GetConsoleWindow(), SW_HIDE);
		}
		else
		{
			ShowWindow(GetConsoleWindow(), SW_NORMAL);
		}
	}

	// ��ȡ�ֱ���
	if (!sysW)
	{
		sysW = GetSystemMetrics(SM_CXSCREEN);
		sysH = GetSystemMetrics(SM_CYSCREEN);
	}

	if (nWndNum == 0)
	{
		RegisterWndClass();
	}

	// ��ʼ��������Ϣ
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
		while (!isDone) { Sleep(10); };	// �ȴ����ڴ������
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

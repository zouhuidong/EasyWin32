/////////////////////////////////////
//
//	EasyWin32.cpp
//
//	Ver 3.1.1
//
//

#include "EasyWin32.h"

// Ԥ����Ϣ�ռ�
#define MSG_RESERVE_SIZE 100

// �޴���ʱ������
#define NO_WINDOW_INDEX -1


EASY_WIN32_BEGIN

////////////****** ȫ�ֱ��� ******////////////


WNDCLASSEX				g_WndClassEx;								// ������
wchar_t					g_lpszClassName[] = L"EasyWin32";			// ��������
int						g_nSysW = 0, g_nSysH = 0;					// ϵͳ�ֱ���
HWND					g_hConsole;									// ����̨���
HINSTANCE				g_hInstance = GetModuleHandle(0);			// ����ʵ��

std::vector<EasyWindow>	g_vecWindows;								// ���ڱ�����ര�ڣ�
int						g_nFocusWindowIndex = NO_WINDOW_INDEX;		// ��ǰ�������㴰������

const int				g_nTypesNum = 4;							// ��Ϣ��������
BYTE					g_pMsgTypes[g_nTypesNum] = {				// ��Ϣ��������
	EM_MOUSE, EM_KEY, EM_CHAR, EM_WINDOW
};

bool					g_isInTask = false;							// ��Ǵ���������

HICON					g_hIconDefault;								// Ĭ�ϳ���ͼ��
int						g_nCustomIcon = 0;							// �Զ������ͼ����Դ ID��Ϊ 0 ��ʾ��ʹ��
int						g_nCustomIconSm = 0;
HICON					g_hCustomIcon;								// �Զ������ͼ��
HICON					g_hCustomIconSm;

bool					g_isPreStyle = false;						// �Ƿ�Ԥ�贰����ʽ
bool					g_isPrePos = false;							// �Ƿ�Ԥ�贰��λ��
long					g_lPreStyle;								// ��������ǰ��Ԥ����ʽ
POINT					g_pPrePos;									// ��������ǰ��Ԥ�贰��λ��

UINT					g_uWM_TASKBARCREATED;						// ϵͳ��������Ϣ����

////////////****** �������� ******////////////

// ���鴰�������Ƿ�Ϸ�
inline bool isValidWindowIndex(int index)
{
	return index >= 0 && index < (int)g_vecWindows.size();
}

// ��ǰ�Ƿ���ڲ������㴰�ڣ������ڣ���һ���ǻ�ڣ�
inline bool isFocusWindowExisted()
{
	return isValidWindowIndex(g_nFocusWindowIndex);
}

// ��ȡ��ǰ�������㴰��
inline EasyWindow& GetFocusWindow()
{
	static EasyWindow wndEmpty;
	if (isFocusWindowExisted())
	{
		return g_vecWindows[g_nFocusWindowIndex];
	}
	else
	{
		wndEmpty = {};
		return wndEmpty;
	}
}

// ͨ�������ô˴����ڴ��ڼ�¼���е�����
// ���� NULL ����ǰ�����
// δ�ҵ����� NO_WINDOW_INDEX
inline int GetWindowIndex(HWND hWnd)
{
	if (hWnd == NULL)
	{
		return g_nFocusWindowIndex;
	}
	int index = NO_WINDOW_INDEX;
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

bool isAnyWindow()
{
	for (EasyWindow& i : g_vecWindows)
		if (i.isAlive)
			return true;
	return false;
}

bool isAliveWindow(HWND hWnd)
{
	if (hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (isValidWindowIndex(index))
		{
			return g_vecWindows[index].isAlive;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return isFocusWindowExisted();
	}
}

inline bool isAliveWindow(int index)
{
	return isValidWindowIndex(index) ? g_vecWindows[index].isAlive : false;
}

// �ȴ������ڲ���Ϣ�������
inline void WaitForProcessing(int index)
{
	// �����ڿ����������٣��ʲ��� isAliveWindow
	if (isValidWindowIndex(index))
	{
		while (g_vecWindows[index].isBusyProcessing)
		{
			HpSleep(1);
		}
	}
}

// �������� EasyX �е�������ʾ��Ŀ�괰����
inline void FlushDrawingToWnd(IMAGE* pImg, HWND hWnd)
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
	// δ���þ��ʱֻ��Ҫ�ȴ���������������Ҫ�жϸþ���Ƿ��Ӧ�����
	if (!hWnd || (isFocusWindowExisted() && GetFocusWindow().hWnd == hWnd))
	{
		while (g_isInTask)
		{
			HpSleep(1);
		}
	}
}

// ���ٴ��ڣ��ͷ��ڴ�
void DelWindow(int index)
{
	if (!isValidWindowIndex(index))
	{
		return;
	}

	// �ͷŻ�ͼ����
	if (g_vecWindows[index].pImg)
	{
		delete g_vecWindows[index].pImg;
		g_vecWindows[index].pImg = NULL;
	}
	if (g_vecWindows[index].pBufferImg)
	{
		delete g_vecWindows[index].pBufferImg;
		g_vecWindows[index].pBufferImg = NULL;
	}

	// �ͷ���Ϣ�б��ڴ�
	std::vector<ExMessage>().swap(g_vecWindows[index].vecMessage);

	DestroyWindow(g_vecWindows[index].hWnd);
	PostQuitMessage(NULL);
}

// �˺��������ڲ����ã������������رմ���
void closegraph_win32(int index)
{
	if (!isAliveWindow(index))
	{
		return;
	}

	// �ȴ��������
	WaitForTask(g_vecWindows[index].hWnd);
	g_vecWindows[index].isBusyProcessing = true;
	g_vecWindows[index].isAlive = false;

	// �������ø�����Ϊģ̬���ڣ�����Ҫ�������ڻָ�����
	if (g_vecWindows[index].hParent != NULL)
	{
		EnableWindow(g_vecWindows[index].hParent, true);
		SetForegroundWindow(g_vecWindows[index].hParent);
	}

	// ж������
	DeleteTray(g_vecWindows[index].hWnd);

	// �������ڱ����٣�����Ҫ���û��������
	if (index == g_nFocusWindowIndex)
	{
		g_nFocusWindowIndex = NO_WINDOW_INDEX;
	}

	// ���ٴ���
	DelWindow(index);
	g_vecWindows[index].isBusyProcessing = false;
}

// �˺��������ⲿ���ã�ֻ����Ŀ�괰���̷߳��͹رմ�����Ϣ
void closegraph_win32(HWND hWnd)
{
	// �ر�ȫ��
	if (hWnd == NULL)
	{
		for (int i = 0; i < (int)g_vecWindows.size(); i++)
		{
			if (g_vecWindows[i].isAlive)
			{
				// ���뽻��ԭ�߳����ٴ��ڣ�����ʹ��������
				// ���� WM_DESTROY ʱ������ wParam Ϊ 1����ʾ�����������ٴ���
				SendMessage(g_vecWindows[i].hWnd, WM_DESTROY, 1, 0);
			}
		}
	}
	else if (isAliveWindow(hWnd))
	{
		SendMessage(hWnd, WM_DESTROY, 1, 0);
	}
}

void init_end()
{
	// ������ֱ�����д��ڶ����ر�
	while (isAnyWindow())
	{
		Sleep(100);
	}
}

void AutoExit()
{
	std::thread([]() {
		init_end();
		exit(0);
		}).detach();
}

HWND GetHWnd_win32()
{
	return isFocusWindowExisted() ? GetFocusWindow().hWnd : NULL;
}

EasyWindow GetWorkingWindow()
{
	return GetFocusWindow();
}

bool SetWorkingWindow(HWND hWnd)
{
	int index = GetWindowIndex(hWnd);
	if (isAliveWindow(index))
	{
		WaitForTask();
		WaitForProcessing(index);
		g_nFocusWindowIndex = index;
		SetWorkingImage(GetFocusWindow().pBufferImg);
		return true;
	}
	else
	{
		return false;
	}
}

void QuickDraw(UINT nSkipPixels)
{
	GetFocusWindow().nSkipPixels = nSkipPixels;
}

void EnforceRedraw()
{
	if (isFocusWindowExisted())
	{
		InvalidateRect(GetFocusWindow().hWnd, NULL, false);
	}
}

// ���ƻ�����
void FlushDrawing(int index)
{
	if (isAliveWindow(index))
	{
		int w = g_vecWindows[index].pImg->getwidth();
		int h = g_vecWindows[index].pImg->getheight();

		if (g_vecWindows[index].nSkipPixels == 0)
		{
			// fastest
			memcpy(
				GetImageBuffer(g_vecWindows[index].pImg),
				GetImageBuffer(g_vecWindows[index].pBufferImg),
				sizeof(DWORD) * w * h
			);
		}
		else
		{
			int len = w * h;
			DWORD* buf[2] = {
				GetImageBuffer(g_vecWindows[index].pImg) ,
				GetImageBuffer(g_vecWindows[index].pBufferImg)
			};
			for (int i = 0; i < len; i++)
			{
				if (buf[0][i] == buf[1][i])
				{
					i += g_vecWindows[index].nSkipPixels;
					continue;
				}
				buf[0][i] = buf[1][i];
				buf[0][i] = buf[1][i];
			}
		}
	}
}

bool BeginTask()
{
	// ��������ƥ���жϣ�ֻ�����Ƿ���������
	if (!g_isInTask && isFocusWindowExisted())
	{
		WaitForProcessing(g_nFocusWindowIndex);
		g_isInTask = true;
	}
	return g_isInTask;
}

void EndTask()
{
	if (g_isInTask)
	{
		if (isFocusWindowExisted())
		{
			FlushDrawing(g_nFocusWindowIndex);
		}

		g_isInTask = false;
	}
}

bool isInTask(HWND hWnd)
{
	return g_isInTask && (hWnd ? GetFocusWindow().hWnd == hWnd : true);
}

// ���ݴ��ڴ�С���µ����ô��ڻ�����С
void ResizeWindowImage(int index)
{
	if (isAliveWindow(index))
	{
		RECT rctWnd;
		for (int i = 0; i < 2; i++)
		{
			if (GetClientRect(g_vecWindows[index].hWnd, &rctWnd))	// �ͻ�������
			{
				g_vecWindows[index].pImg->Resize(rctWnd.right, rctWnd.bottom);
				g_vecWindows[index].pBufferImg->Resize(rctWnd.right, rctWnd.bottom);
				g_vecWindows[index].isNewSize = true;
				break;
			}
		}
	}
}

void ShowTray(NOTIFYICONDATA* nid)
{
	Shell_NotifyIcon(NIM_ADD, nid);
}

void CreateTray(LPCTSTR lpTrayName)
{
	static int id = 0;

	HICON hIcon = g_hIconDefault;
	if (g_nCustomIconSm)		hIcon = g_hCustomIconSm;
	else if (g_nCustomIcon)		hIcon = g_hCustomIcon;

	GetFocusWindow().isUseTray = true;
	GetFocusWindow().nid.cbSize = sizeof(GetFocusWindow().nid);
	GetFocusWindow().nid.hWnd = GetFocusWindow().hWnd;
	GetFocusWindow().nid.uID = id++;
	GetFocusWindow().nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	GetFocusWindow().nid.uCallbackMessage = WM_TRAY;
	GetFocusWindow().nid.hIcon = hIcon;
	lstrcpy(GetFocusWindow().nid.szTip, lpTrayName);
	ShowTray(&GetFocusWindow().nid);
}

void DeleteTray(HWND hWnd)
{
	int index;
	if (hWnd == NULL)
	{
		if (!isFocusWindowExisted())
		{
			return;
		}
		else
		{
			index = g_nFocusWindowIndex;
		}
	}
	else
	{
		index = GetWindowIndex(hWnd);

		// ������ɾ��ʱ����øú��������Բ��жϴ�������
		if (!isValidWindowIndex(index))
		{
			return;
		}
	}
	if (g_vecWindows[index].isUseTray)
	{
		g_vecWindows[index].isUseTray = false;
		Shell_NotifyIcon(NIM_DELETE, &g_vecWindows[index].nid);
	}
}

void SetTrayMenu(HMENU hMenu)
{
	if (isFocusWindowExisted())
	{
		GetFocusWindow().isUseTrayMenu = true;
		GetFocusWindow().hTrayMenu = hMenu;
	}
}

void SetTrayMenuProcFunc(void(*pFunc)(UINT))
{
	GetFocusWindow().funcTrayMenuProc = pFunc;
}

bool isWindowSizeChanged(HWND hWnd)
{
	int index = GetWindowIndex(hWnd);
	if (isValidWindowIndex(index))
	{
		bool b = g_vecWindows[index].isNewSize;
		g_vecWindows[index].isNewSize = false;
		return b;
	}
	else
	{
		return false;
	}
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

UINT GetExMessageType(ExMessage msg)
{
	switch (msg.message)
	{
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
		return EM_MOUSE;
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return EM_KEY;
		break;
	case WM_CHAR:
		return EM_CHAR;
		break;
	case WM_ACTIVATE:
	case WM_MOVE:
	case WM_SIZE:
		return EM_WINDOW;
		break;
	default:
		return 0;
		break;
	}
}

// ��ȡĳ����Ϣ�� vector
std::vector<ExMessage>& GetMsgVector()
{
	return GetFocusWindow().vecMessage;
}

// �Ƴ���ǰ��Ϣ
void RemoveMessage()
{
	GetMsgVector().erase(GetMsgVector().begin());
}

// �����Ϣ
// ֧�ֻ����Ϣ����
void ClearMessage(BYTE filter)
{
	for (size_t i = 0; i < GetMsgVector().size(); i++)
		if (filter & GetExMessageType(GetMsgVector()[i]))
			GetMsgVector().erase(GetMsgVector().begin() + i--);
}

// �Ƿ�������Ϣ
// ֧�ֻ����Ϣ����
bool isNewMessage(BYTE filter)
{
	for (auto element : GetMsgVector())
		if (filter & GetExMessageType(element))
			return true;
	return false;
}

// �����Ϣ��ֱ����ȡ���������͵���Ϣ
// ֧�ֻ����Ϣ����
ExMessage GetNextMessage(BYTE filter)
{
	if (isNewMessage(filter))
	{
		for (size_t i = 0; i < GetMsgVector().size(); i++)
		{
			if (filter & GetExMessageType(GetMsgVector()[i]))
			{
				for (size_t j = 0; j < i; j++)
				{
					RemoveMessage();
				}
				return GetMsgVector()[0];
			}
		}
	}
	return {};
}

ExMessage getmessage_win32(BYTE filter)
{
	while (!isNewMessage(filter))	HpSleep(1);
	ExMessage msg = GetNextMessage(filter);
	RemoveMessage();
	return msg;
}

void getmessage_win32(ExMessage* msg, BYTE filter)
{
	ExMessage msgEx = getmessage_win32(filter);
	if (msg)	*msg = msgEx;
}

bool peekmessage_win32(ExMessage* msg, BYTE filter, bool removemsg)
{
	if (isNewMessage(filter))
	{
		if (msg)		*msg = GetNextMessage(filter);
		if (removemsg)	RemoveMessage();
		return true;
	}
	return false;
}

void flushmessage_win32(BYTE filter)
{
	ClearMessage(filter);
}

bool MouseHit_win32()
{
	return isNewMessage(EM_MOUSE);
}

MOUSEMSG GetMouseMsg_win32()
{
	ExMessage msgEx = GetNextMessage(EM_MOUSE);
	return To_MouseMsg(msgEx);
}

bool PeekMouseMsg_win32(MOUSEMSG* pMsg, bool bRemoveMsg)
{
	ExMessage msgEx;
	bool r = peekmessage_win32(&msgEx, EM_MOUSE, bRemoveMsg);
	*pMsg = To_MouseMsg(msgEx);
	return r;
}

void FlushMouseMsgBuffer_win32()
{
	ClearMessage(EM_MOUSE);
}

ExMessage To_ExMessage(MOUSEMSG msg)
{
	ExMessage msgEx = {};
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
	MOUSEMSG msg = {};
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
	return GetWindowLong(GetFocusWindow().hWnd, GWL_STYLE);
}

long GetWindowExStyle()
{
	return GetWindowLong(GetFocusWindow().hWnd, GWL_EXSTYLE);
}

int SetWindowStyle(long lNewStyle)
{
	return SetWindowLong(GetFocusWindow().hWnd, GWL_STYLE, lNewStyle);
}

int SetWindowExStyle(long lNewExStyle)
{
	return SetWindowLong(GetFocusWindow().hWnd, GWL_EXSTYLE, lNewExStyle);
}

// ��ȡĬ�ϴ���ͼ��
HICON GetDefaultAppIcon()
{
	IMAGE img = GetDefaultIconImage();
	HBITMAP hBmp = Image2Bitmap(&img);
	HICON hIcon = Bitmap2Icon(hBmp);
	DeleteObject(hBmp);
	return hIcon;
}

// ���ڹ��̺���
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rctWnd;							// ���ھ�����Ϣ
	POINT ptMouse;							// ���λ��
	bool isNeedDefaultProc = true;			// ��¼�Ƿ���Ҫʹ��Ĭ�Ϸ���������Ϣ
	int indexWnd = GetWindowIndex(hwnd);	// �ô������Ѽ�¼�б��е�����

	// ����δ֪���ڣ���ʹ��Ĭ�Ϸ������д������Ƿ����������
	// ������Ҳ���ܵ��ù��̺���
	if (!isValidWindowIndex(indexWnd))
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	GetWindowRect(hwnd, &rctWnd);
	GetCursorPos(&ptMouse);

	//** ��ʼ��������Ϣ **//

	// ����Ԥ�ȴ����һЩ��Ϣ
	switch (msg)
	{
	case WM_CREATE:
		break;

	case WM_SIZE:
		// ��ֹ�͵�ǰ��ͼ��������ͻ
		WaitForTask(g_vecWindows[indexWnd].hWnd);
		g_vecWindows[indexWnd].isBusyProcessing = true;
		ResizeWindowImage(indexWnd);
		g_vecWindows[indexWnd].isBusyProcessing = false;
		break;

		// ������Ϣ
	case WM_TRAY:
		if (g_vecWindows[indexWnd].isUseTray)
		{
			switch (lParam)
			{
				// ��������
			case WM_LBUTTONDOWN:
				SetForegroundWindow(hwnd);
				break;

				// �Ҽ��򿪲˵�
			case WM_RBUTTONDOWN:
				if (g_vecWindows[indexWnd].isUseTrayMenu)
				{
					SetForegroundWindow(hwnd);	// ����һ�´��ڣ���ֹ�˵�����ʧ

					// ��ʾ�˵�������
					int nMenuId = TrackPopupMenu(g_vecWindows[indexWnd].hTrayMenu, TPM_RETURNCMD, ptMouse.x, ptMouse.y, NULL, hwnd, NULL);
					if (nMenuId == 0) PostMessage(hwnd, WM_LBUTTONDOWN, NULL, NULL);
					if (g_vecWindows[indexWnd].funcTrayMenuProc)
					{
						g_vecWindows[indexWnd].funcTrayMenuProc(nMenuId);
					}

				}
				break;

			default:
				break;
			}
		}
		break;

	default:
		// ϵͳ���������´�������ʱ������Ҫ���´�������
		if (msg == g_uWM_TASKBARCREATED)
		{
			if (g_vecWindows[indexWnd].isUseTray)
			{
				ShowTray(&g_vecWindows[indexWnd].nid);
			}
		}
		break;
	}

	// ��Ϣ��¼
	switch (msg)
	{
		// EM_MOUSE
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
		ExMessage msgMouse = {};
		msgMouse.message = msg;
		msgMouse.x = LOWORD(lParam);
		msgMouse.y = HIWORD(lParam);
		msgMouse.wheel = HIWORD(wParam);
		msgMouse.shift = LOWORD(wParam) & 0x04 ? true : false;
		msgMouse.ctrl = LOWORD(wParam) & 0x08 ? true : false;
		msgMouse.lbutton = LOWORD(wParam) & 0x01 ? true : false;
		msgMouse.mbutton = LOWORD(wParam) & 0x10 ? true : false;
		msgMouse.rbutton = LOWORD(wParam) & 0x02 ? true : false;
		g_vecWindows[indexWnd].vecMessage.push_back(msgMouse);
	}
	break;

	// EM_KEY
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	{
		// code from MSDN
		WORD vkCode = LOWORD(wParam);                                 // virtual-key code
		WORD keyFlags = HIWORD(lParam);
		WORD scanCode = LOBYTE(keyFlags);                             // scan code
		BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED; // extended-key flag, 1 if scancode has 0xE0 prefix

		if (isExtendedKey)
			scanCode = MAKEWORD(scanCode, 0xE0);

		BOOL repeatFlag = (keyFlags & KF_REPEAT) == KF_REPEAT;        // previous key-state flag, 1 on autorepeat
		WORD repeatCount = LOWORD(lParam);                            // repeat count, > 0 if several keydown messages was combined into one message
		BOOL upFlag = (keyFlags & KF_UP) == KF_UP;                    // transition-state flag, 1 on keyup
		
		// ���ܼ�������������
		// if we want to distinguish these keys:
		//switch (vkCode)
		//{
		//case VK_SHIFT:   // converts to VK_LSHIFT or VK_RSHIFT
		//case VK_CONTROL: // converts to VK_LCONTROL or VK_RCONTROL
		//case VK_MENU:    // converts to VK_LMENU or VK_RMENU
		//	vkCode = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
		//	break;
		//}

		ExMessage msgKey = {};
		msgKey.message = msg;
		msgKey.vkcode = (BYTE)vkCode;
		msgKey.scancode = (BYTE)scanCode;
		msgKey.extended = isExtendedKey;
		msgKey.prevdown = repeatFlag;

		g_vecWindows[indexWnd].vecMessage.push_back(msgKey);

		// ������̨��һ�ݣ�֧�� _getch() ϵ�к���
		PostMessage(g_hConsole, msg, wParam, lParam);
	}
	break;

	// EM_CHAR
	case WM_CHAR:
	{
		ExMessage msgChar = {};
		msgChar.message = msg;
		msgChar.ch = wParam;
		g_vecWindows[indexWnd].vecMessage.push_back(msgChar);

		// ֪ͨ����̨
		PostMessage(g_hConsole, msg, wParam, lParam);
	}
	break;

	// EM_WINDOW
	case WM_ACTIVATE:
	case WM_MOVE:
	case WM_SIZE:
	{
		ExMessage msgWindow = {};
		msgWindow.message = msg;
		msgWindow.wParam = wParam;
		msgWindow.lParam = lParam;
		g_vecWindows[indexWnd].vecMessage.push_back(msgWindow);
	}
	break;

	}

	// ���ж�������Ϣ�����������
	if (g_vecWindows[indexWnd].funcWndProc)
	{
		isNeedDefaultProc = g_vecWindows[indexWnd].funcWndProc(hwnd, msg, wParam, lParam, g_hInstance);
	}

	// �������û�������Ϣ������һЩ��������
	switch (msg)
	{
		// ӳ���ͼ���浽����
	case WM_PAINT:
		FlushDrawingToWnd(g_vecWindows[indexWnd].pImg, g_vecWindows[indexWnd].hWnd);
		DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_MOVE:
		// �ƶ����ڳ�����Ļʱ���ܵ����Ӵ�����ʾ�����⣬���Դ�ʱ��Ҫ�����ػ�
		if (rctWnd.left <= 0 || rctWnd.top <= 0 || rctWnd.right >= g_nSysW || rctWnd.bottom >= g_nSysH)
		{
			InvalidateRect(hwnd, NULL, false);
		}
		break;

	case WM_DESTROY:
		// WM_DESTROY ��Ϣ���޲����������ֲ������ǳ����������ٴ���
		if (wParam)
		{
			closegraph_win32(indexWnd);
		}
		break;
	}

	// ����ֵ
	LRESULT lResult = 0;

	// ����ʹ��Ĭ�Ϸ�����������
	// ע�⣬�˹����в��ܷ��أ������ں���ĩβ����
	if (isNeedDefaultProc)
	{
		switch (msg)
		{
			// ����Ϣ�û��������أ���Ĭ�ϴ���ʱֱ�����ٴ���
		case WM_CLOSE:
			closegraph_win32(indexWnd);
			break;

		default:
			lResult = DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}

	return lResult;
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
	static int nWndCount = 0;	// �Ѵ������ڼ������������ɴ��ڱ��⣩
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
	wnd.isAlive = true;
	wnd.hParent = hParent;
	wnd.pImg = new IMAGE(w, h);
	wnd.pBufferImg = new IMAGE(w, h);
	wnd.funcWndProc = WindowProcess;
	wnd.vecMessage.reserve(MSG_RESERVE_SIZE);
	wnd.isUseTray = false;
	wnd.nid = { 0 };
	wnd.isUseTrayMenu = false;
	wnd.hTrayMenu = NULL;
	wnd.funcTrayMenuProc = NULL;
	wnd.isNewSize = false;
	wnd.isBusyProcessing = false;
	wnd.nSkipPixels = 0;

	g_vecWindows.push_back(wnd);

	// ���ό��
	SetWorkingWindow(wnd.hWnd);

	// ���ڴ������
	nWndCount++;

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

	g_isPreStyle = false;
	g_isPrePos = false;

	ShowWindow(wnd.hWnd, SW_SHOWNORMAL);
	UpdateWindow(wnd.hWnd);

	// ���� WM_CREATE ��Ϣ��δ֪ԭ�����ɣ�������Ҫģ�ⷢ�ʹ���Ϣ
	WndProc(wnd.hWnd, WM_CREATE, NULL, NULL);

	// ��Ϣ�ɷ�������
	// �������ٺ���Զ��˳�
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
		// ���ø����ڣ��ô��ڱ����ٺ󣬸����ڽ���ָ�������
		EnableWindow(hParent, false);
	}

	std::thread(InitWindow, w, h, flag, strWndTitle, WindowProcess, hParent, &nDoneFlag).detach();

	while (nDoneFlag == 0)	Sleep(10);		// �ȴ����ڴ������
	if (nDoneFlag == -1)
	{
		if (hParent)						// �����Ӵ���ʧ�ܣ���ʹ�����ڻָ�����
		{
			EnableWindow(hParent, true);
		}
		return NULL;
	}
	else
	{
		return GetHWnd_win32();
	}
}

////////////****** �������� ******////////////

HBITMAP Image2Bitmap(IMAGE* img)
{
	return CreateBitmap(img->getwidth(), img->getheight(), 1, 32, (void*)GetImageBuffer(img));
}

HICON Bitmap2Icon(HBITMAP hBmp)
{
	BITMAP bmp = {};
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

void outtextxy_format(int x, int y, int _Size, const wchar_t* _Format, ...)
{
	va_list list;
	va_start(list, _Format);
	wchar_t* buf = new wchar_t[_Size];
	vswprintf_s(buf, _Size, _Format, list);
	va_end(list);
	outtextxy(x, y, buf);
	delete buf;
}

EASY_WIN32_END

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


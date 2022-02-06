////////////////////////////////////////////
//
//	EasyWin32 ʹ��ʾ��
//
//	1. ʹ�� win32 ��Ϣ�ɷ��Ĵ���ṹ
//	2. �����˶����ͼ����
//	3. չʾ�� win32 �ؼ��� EasyX �Ľ��
//

#include "EasyWin32.h"

// �ؼ� ID ��¼
#define IDC_EDIT 100
#define IDC_BTN1 101
#define IDC_BTN2 101

// �༭��ؼ����
HWND hEdit;

wchar_t str[512] = L"Hello, EasyX.";

// ���� 1 �Ĺ��̺���
bool WndProc1(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
	case WM_CREATE:

		// ��������򣬰�ť
		hEdit = CreateWindow(L"edit", L"Edit at here.",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			130, 50, 200, 20,
			hwnd, (HMENU)IDC_EDIT, hInstance, NULL);

		CreateWindow(L"button", L"Click Me!",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			330, 50, 100, 20,
			hwnd, (HMENU)IDC_BTN1, hInstance, NULL);

		EasyWin32::SetWorkingWindow(hwnd);			// ���û�ͼ����Ϊ�Լ�
		if (EasyWin32::GetHWnd_win32() == hwnd)		// �ж��Ƿ�ɹ���ȡ��ǰ��ͼȨ
		{
			EasyWin32::ReadyToDraw();				// ÿ�ν��л�ͼ����ǰ��Ҫ���ô˺���

			// ��ͼ״̬����
			setbkcolor(RGB(250, 250, 250));
			setbkmode(TRANSPARENT);
			setlinecolor(BLACK);
			setfillcolor(BLUE);
			settextcolor(BLACK);
			settextstyle(32, 0, L"system");

			// EasyWin32 Ĭ��ʹ��˫���壬�˴�������壨����ʹ�� FlushBatchDraw ϵ�к�����
			EasyWin32::FlushDrawing();
		}

		break;

	case WM_PAINT:

		EasyWin32::SetWorkingWindow(hwnd);
		if (EasyWin32::GetHWnd_win32() == hwnd)
		{
			EasyWin32::ReadyToDraw();

			cleardevice();
			setlinestyle(0, 5);
			setlinecolor(LIGHTBLUE);
			fillcircle(100, getheight() - 100, 80);
			fillrectangle(getwidth() - 170, getheight() - 170, getwidth() - 30, getheight() - 30);
			outtextxy(130, 150, str);

			EasyWin32::FlushDrawing();
		}

		break;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDC_BTN1:	// ���°�ť

			// �õ�������ı�
			GetWindowText(hEdit, str, 512);

			// ���û�ͼ����Ϊ�Լ�
			EasyWin32::SetWorkingWindow(hwnd);
			if (EasyWin32::GetHWnd_win32() == hwnd)
			{
				// ǿ���ػ�
				EasyWin32::EnforceRedraw();
			}

			break;
		}

		break;

	default: return true; break;	// ʹ��Ĭ�Ϸ�������������Ϣ
	}
	return false;
}

// ���� 3 �Ĺ��̺���
bool WndProc3(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
		// �˴�������ѡ��͸�ѡ��Ĵ������� https://blog.csdn.net/siyacaodeai/article/details/114039773
	case WM_CREATE:

		// ��
		CreateWindowEx(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"Select Process Mode:",
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			10, 5,
			350, 100,
			hwnd,
			nullptr,
			hInstance, NULL);
		// ��ѡ��
		CreateWindowEx(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"first radio button",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
			20, 25,
			300, 20,
			hwnd,
			nullptr,
			hInstance, NULL);
		CreateWindowEx(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"second radio button",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			20, 45,
			300, 20,
			hwnd,
			nullptr,
			hInstance, NULL);
		// ��ѡ��
		CreateWindowEx(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"third radio button",
			WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_GROUP,
			20, 65,
			300, 20,
			hwnd,
			nullptr,
			hInstance, NULL);
		CreateWindowEx(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"forth radio button",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			20, 120,
			300, 20,
			hwnd,
			nullptr,
			hInstance, NULL);
		break;

	case WM_PAINT:
		EasyWin32::SetWorkingWindow(hwnd);
		if (EasyWin32::GetHWnd_win32() == hwnd)
		{
			EasyWin32::ReadyToDraw();

			// ���ñ���ɫ
			setbkcolor(RGB(240, 240, 240));
			cleardevice();

			EasyWin32::FlushDrawing();
		}
		break;

	default: return true; break;
	}
	return false;
}

// ���� 2 �Ĺ��̺���
bool WndProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
	case WM_CREATE:

		// ����һ����ť
		CreateWindow(L"button", L"Settings...",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			20, 20, 90, 40,
			hwnd, (HMENU)IDC_BTN2, hInstance, NULL);

		// ����һ���ı���
		CreateWindow(L"edit", L"���������\r\n\r\n���������~\r\n\r\nEasyWin32 by huidong",
			WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
			20, 80, 400, 200,
			hwnd, (HMENU)IDC_EDIT, hInstance, NULL);

		break;

	case WM_PAINT:
		EasyWin32::SetWorkingWindow(hwnd);
		if (EasyWin32::GetHWnd_win32() == hwnd)
		{
			EasyWin32::ReadyToDraw();

			// ���ñ���ɫ
			setbkcolor(RGB(240, 240, 240));
			cleardevice();

			// �ڵײ���ʾ "EasyX"
			settextcolor(BLUE);
			setbkmode(TRANSPARENT);
			settextstyle(72, 0, L"system");
			LPCTSTR str = L"EasyX";
			outtextxy((getwidth() - textwidth(str)) / 2, getheight() - 100, str);

			EasyWin32::FlushDrawing();
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BTN2:	// ���°�ť

			// �򿪴��� 3����������Ϊ�Լ����Ӵ���
			EasyWin32::initgraph_win32(400, 240, 0, L"Third", WndProc3, hwnd);

			break;
		}
		break;

	default: return true; break;
	}
	return false;
}

int main()
{
	// ������ͼ����
	EasyWin32::initgraph_win32(640, 480, 0, L"", WndProc1);
	EasyWin32::initgraph_win32(640, 480, 0, L"", WndProc2);

	// ��ʹ�� win32 ��Ϣ�ɷ��Ĵ���ṹʱ����Ҫ���ô˺�����������
	EasyWin32::init_end();

	return 0;
}

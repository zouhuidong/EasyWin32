////////////////////////////
//
//	EasyWin32 ʹ��ʾ��
//
//	���ٴ�������ͼ��
//

#include "EasyWin32.h"

// �������̲˵���ѡ�� ID
#define IDC_A	101
#define IDC_B	102
#define IDC_C	103
#define IDC_D	104

// �Զ�����̺���
bool WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	// ��ʶ�Ƿ񵥻���������ͼ��
	static bool flag = false;

	switch (msg)
	{
		// �����ͼ��Ϣ
	case WM_PAINT:
		if (EasyWin32::SetWorkingWindow(hWnd))
		{
			EasyWin32::BeginTask();
			setbkcolor(SKYBLUE);
			cleardevice();
			settextstyle(32, 0, L"system");
			settextcolor(BLACK);
			outtextxy(20, 20, L"Windows Tray Icon Creation Sample");
			if (flag)
			{
				settextstyle(26, 0, L"system");
				settextcolor(PURPLE);
				outtextxy(20, 60, L"You pressed the tray icon.");
			}
			EasyWin32::EndTask();
		}
		break;

		// ����������Ϣ
	case WM_TRAY:
	{
		switch (lParam)
		{
			// �����������ͼ��
		case WM_LBUTTONDOWN:
			flag = true;					// ��ǰ���
			EasyWin32::EnforceRedraw();		// �����ػ�
			break;
		}
	}
	break;
	}

	return true;
}

// �������̲˵��¼�
void OnMenu(UINT uId)
{
	switch (uId)
	{
	case IDC_A:
		MessageBox(GetHWnd(), L"A", L"Tip", MB_OK);
		break;
	case IDC_C:
		MessageBox(GetHWnd(), L"C", L"Tip", MB_OK);
		break;
	case IDC_D:
		MessageBox(GetHWnd(), L"D", L"Tip", MB_OK);
		break;
	}
}

int main()
{
	// ����һ���Զ�����̺����Ļ�ͼ����
	HWND hWnd1 = EasyWin32::initgraph_win32(640, 480, EW_NORMAL, L"EasyWin32 & Tray", WndProc);

	// �����˵�
	HMENU hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, IDC_A, L"ѡ�� A");
	AppendMenu(hMenu, MF_STRING, IDC_B, L"ѡ�� B");
	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);				// �ָ���
	AppendMenu(hMenu, MF_STRING, IDC_C, L"ѡ�� C");
	EnableMenuItem(hMenu, IDC_B, MF_GRAYED);				// ����һ��ѡ��

	EasyWin32::CreateTray(L"The first");					// ��������
	EasyWin32::SetTrayMenu(hMenu);							// �������̲˵�
	EasyWin32::SetTrayMenuProcFunc(OnMenu);					// �������̲˵���Ӧ����

	//
	// ע�⣺
	//	��ʾ����Ϊ����ʾ���������������ڣ��ֱ���һ������ͼ�ꡣ
	//	����ʵ��Ӧ���м���������������һ���������һ�����̾͹��ˡ�
	//	��������ͼ���ѹ̶�����Ϊ����ͼ�ꡣ
	//	EasyWin32 �������̹���ֻ�����˼򵥷�װ������и�����Ҫ��Ҳ��������ʵ�֡�
	//

	// �����ڶ�������
	EasyWin32::initgraph_win32(200, 120, EW_NORMAL, L"The Second Window");

	// ��ʱ�����������ڣ�Ϊ��ֹ��ͻ����������
	EasyWin32::BeginTask();
	{
		setbkcolor(LIGHTSKYBLUE);
		cleardevice();
		settextcolor(BLACK);
		outtextxy(20, 20, L"The Second Window");

		// �����˵�
		HMENU hMenu2 = CreatePopupMenu();
		AppendMenu(hMenu2, MF_STRING, IDC_D, L"ѡ�� D");

		EasyWin32::CreateTray(L"The second");				// ��������
		EasyWin32::SetTrayMenu(hMenu2);						// �������̲˵�
		EasyWin32::SetTrayMenuProcFunc(OnMenu);				// �������̲˵���Ӧ����
	}
	EasyWin32::EndTask();
	EasyWin32::EnforceRedraw();

	EasyWin32::SetWorkingWindow(hWnd1);						// ������Ȩ������һ������
	SetForegroundWindow(hWnd1);

	EasyWin32::init_end();									// �����Ⱥ�
	return 0;
}


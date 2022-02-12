///////////////////////////////////////////////////////////
//
//	EasyWin32 ʹ��ʾ��
//
//	1. ˳�����ṹ��ͼ��ͬʱ���Զ����˴��ڹ��̺���
//	2. չʾ����δ���رմ�����Ϣ
//	
//	��ͼ����Դ������ https://codebus.cn/dudugang/drawing
//

#include "EasyWin32.h"
#include <time.h>

int x;								// X ������
int y;								// y ������
int g_multiples = 0;				// �����������ǶԻ��Ƚǽ��иı�
float g_PI = (float)3.1415;			// Բ����
float g_radianAngle;				// ��ʼ�ǵĻ���
int w = 830;						// ���ڿ�
int h = 580;						// ���ڸ�

// ���ڹ��̺���
bool WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
	case WM_CLOSE:

		// �رմ���ʱ��ʾ�Ƿ�ȷ�Ϲر�
		if (MessageBox(hwnd, L"ȷ���رմ��ڣ�", L"��ʾ", MB_OKCANCEL | MB_ICONWARNING) == IDOK)
		{
			return true;
		}
		else
		{
			return false;
		}
		break;

	default:	return true;	break;
	}
}

int main()
{
	// ������ͼ���ڣ���ָ�����ڹ��̺���
	EasyWin32::initgraph_win32(w, h, false, L"", WndProc);

	// ���ñ���ɫ
	setbkcolor(WHITE);

	while (true)
	{
		// ����һ����ͼ����
		// ����ֻ������һ�����ڣ����Բ���Ҫָ��Ŀ���ͼ����
		// �������Ҫ���� BEGIN_TASK_WND() �ָ꣬��Ŀ���ͼ����
		BEGIN_TASK();

		// ����һЩ���ƣ����ڵ��õĶ��� EasyX ��ͼ����������ע��
		cleardevice();
		setcolor(RGB(185, 230, 0));
		setfillcolor(RGB(185, 230, 0));
		solidrectangle(10, 10, 820, 570);
		for (size_t j = 0; j < 11; j++)
		{
			for (size_t i = 0; i < 16; i++)
			{
				x = 15 + 50 * i;
				y = 15 + 50 * j;
				g_radianAngle = 0 + g_multiples * g_PI / 4;
				i < 15 ? g_multiples++ : g_multiples = g_multiples;
				rectangle(x, y, x + 50, y + 50);
				setfillcolor(RGB(250, 250, 250));
				solidcircle(x + 25, y + 25, 20);
				setfillcolor(RGB(80, 80, 80));
				solidpie(x + 5, y + 5, x + 50 - 5, y + 50 - 5, g_radianAngle, g_radianAngle + g_PI);
				setfillcolor(RGB(158, 2, 251));
				solidcircle(x + 25, y + 25, 15);
			}
		}

		// EasyWin32 Ĭ��ʹ��˫�����ͼ���˴������ͼ����
		// ע�⣺һ�λ�ͼ��������������Դ˺��β���� BEGIN_TASK �� END_TASK �������ã�
		END_TASK();

		// ���ڴ��ڹ��̺����� WM_PAINT ��Ϣ�ڻ�ͼʱ������ǿ���ػ�
		// ����û���Զ��崰�ڹ��̺��������Ե�ȻҲҪ���ô˺�ǿ���ػ�
		FLUSH_DRAW();

		while (true)
		{
			// ���ڴ�С�ı�ʱ�ػ�
			if (EasyWin32::isWindowSizeChanged())
			{
				break;
			}

			// �����ڱ��رգ����������
			if (!EasyWin32::isAnyWindow())
			{
				return 0;
			}

			Sleep(10);
		}
	}
}


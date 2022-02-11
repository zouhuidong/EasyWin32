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
	default:	return true;
	}
}

// ʵʩ����������С��ĺ���
void DrawCell(
	int left_x,						// �󶥵��X������
	int top_y,						// �󶥵��y������
	float PI,						// Բ����
	float radianAngle				// ��ʼ�ǵĻ���
)
{
	// �����ޱ߿�����Բ����ɫ
	setfillcolor(RGB(250, 250, 250));

	// ���ޱ߿�����Բ��
	solidcircle(left_x + 25, top_y + 25, 20);

	// �����ޱ߿�����������ɫ
	setfillcolor(RGB(80, 80, 80));

	// ���ޱ߿���������
	solidpie(left_x + 5, top_y + 5, left_x + 50 - 5, top_y + 50 - 5, radianAngle, radianAngle + PI);

	// �����ޱ����Բ����ɫ
	setfillcolor(RGB(158, 2, 251));

	// ���ޱ����Բ
	solidcircle(left_x + 25, top_y + 25, 15);
}

int main()
{
	// ������ͼ���ڣ���ָ�����ڹ��̺���
	EasyWin32::initgraph_win32(w, h, false, L"", WndProc);

	// ���ñ���ɫ
	setbkcolor(WHITE);

	while (true)
	{
		// �ñ���ɫ�����Ļ
		cleardevice();

		// ���û�ͼɫΪ������ɫ����Ϊ��ȷ��ÿһ��С���λ��ʱҪ��һ���б߿�ľ��Σ������������ȥ������ʾ���α߿�
		setcolor(RGB(185, 230, 0));

		// �����ޱ߿�������ε���ɫ
		setfillcolor(RGB(185, 230, 0));

		// ������ޱ߿�������Σ���������С��ķ�Χ
		solidrectangle(10, 10, 820, 570);

		// ��������
		for (size_t j = 0; j < 11; j++)
		{
			// ��������
			for (size_t i = 0; i < 16; i++)
			{
				x = 15 + 50 * i;							// X������
				y = 15 + 50 * j;							// y������
				g_radianAngle = 0 + g_multiples * g_PI / 4;	// ��ʼ�ǵĻ���

				i < 15 ? g_multiples++ : g_multiples = g_multiples;

				// ���б߾��Σ�ȷ������С���λ��
				rectangle(x, y, x + 50, y + 50);

				// ������С��
				DrawCell(x, y, g_PI, g_radianAngle);
			}
		}

		// EasyWin32 Ĭ��ʹ��˫�����ͼ���˴������ͼ����
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


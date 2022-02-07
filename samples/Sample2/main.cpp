////////////////////////////////
//
//	EasyWin32 ʹ��ʾ��
//
//	1. ʹ��˳�����ṹ
//	2. �����˶����ͼ����
//	3. չʾ�������Ϣ�Ļ�ȡ
//

#include "EasyWin32.h"
#include <time.h>

int main()
{
	// �����������
	srand((unsigned)time(NULL));

	// ����������ͼ����
	HWND hWnd1 = EasyWin32::initgraph_win32(640, 480, false, L"�ַ���");
	HWND hWnd2 = EasyWin32::initgraph_win32(640, 480, false, L"������ʾ��");

	// ���ô��� 1 ��ͼ��ʽ
	EasyWin32::SetWorkingWindow(hWnd1);
	settextstyle(16, 8, _T("Courier"));
	settextcolor(GREEN);

	while (true)
	{
		// ������ 1 �����ڣ�δ���رգ�
		if (EasyWin32::isAliveWindow(hWnd1))
		{
			// ���ô��� 1 ΪĿ���ͼ����
			EasyWin32::SetWorkingWindow(hWnd1);

			// �������ݣ�EasyX �ٷ�ʾ�����ַ��󡱣��򻯣�
			cleardevice();
			for (int i = 0; i <= 200; i++)
			{
				// �����λ����ʾ���������ĸ
				for (int j = 0; j < 3; j++)
				{
					int x = (rand() % 80) * 8;
					int y = (rand() % 20) * 24;
					char c = (rand() % 26) + 65;
					outtextxy(x, y, c);
				}
			}

			// EasyWin32 Ĭ��ʹ��˫�����ͼ���˴������ͼ����
			FLUSH_DRAW();
		}

		// ���� 2
		if (EasyWin32::isAliveWindow(hWnd2))
		{
			EasyWin32::SetWorkingWindow(hWnd2);

			// �������ݣ�EasyX �ٷ�ʾ���������������иĶ���
			ExMessage m;
			if (EasyWin32::MouseHit_win32())
			{
				// ��ȡһ�������Ϣ
				m = EasyWin32::GetMouseMsg_win32();
				switch (m.message)
				{
				case WM_MOUSEMOVE:
					// ����ƶ���ʱ�򻭺�ɫ��С��
					putpixel(m.x, m.y, RED);
					break;

				case WM_LBUTTONDOWN:
					// ����������ͬʱ������ Ctrl ��
					if (m.ctrl)
						// ��һ���󷽿�
						rectangle(m.x - 10, m.y - 10, m.x + 10, m.y + 10);
					else
						// ��һ��С����
						rectangle(m.x - 5, m.y - 5, m.x + 5, m.y + 5);
					break;
				}
			}

			FLUSH_DRAW();
		}

		// �����д��ڶ����رգ����������
		if (!EasyWin32::isAnyWindow())
		{
			break;
		}

		// ���� CPU ռ��
		if (rand() % 777 == 0)
			Sleep(1);
	}

	return 0;
}

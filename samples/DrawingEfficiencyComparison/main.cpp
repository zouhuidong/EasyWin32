///////////////////////////////////////////////////////////
//
//	���򣺱Ƚ�ԭ�� EasyX ��ʹ�� EasyWin32 ��Ļ�ͼЧ��
//	���ۣ�ԭ�� EasyX �� EasyWin32 ��ͼЧ�ʸ���
//	
//	���ߣ�huidong<huidong_mail@163.com>
//	������VisualStudio 2022 | EasyX_20220116 | Windows 10
//	���ڣ�2022.4.16
//

// �Ƿ������ EASYX ���������Ƿ���ԭ�� EasyX �ķ�ʽ����
#define EASYX

#ifdef EASYX
#include <easyx.h>
#include <time.h>
#include <math.h>
#define BEGIN_TASK()
#define END_TASK()
#define TITLE L"A moving ball [ EasyX ]"
#else
#include "EasyWin32.h"
#define TITLE L"A moving ball [ EasyWin32 ]"
#endif // EASYX

int main()
{
	int w = 640, h = 480;
	initgraph(w, h);
	BeginBatchDraw();
	BEGIN_TASK();
	setfillcolor(RGB(20, 150, 40));
	END_TASK();

	clock_t t = clock();
	int count = 0;
	wchar_t strFPS[64] = TITLE;
	for (float i = 0, k = (float)0.1, r = 0; i < w; i += k)
	{
		r = (float)pow(i / 5, 1.1);
		BEGIN_TASK();
		cleardevice();
		outtextxy(0, 0, strFPS);
		solidcircle((int)i, h / 2, (int)r);
		END_TASK();
		FlushBatchDraw();
		if (i + r >= w - 1 || (i <= 1 && k < 0))	k *= -1;
		if ((double)(clock() - t) / CLOCKS_PER_SEC >= 1)
		{
			wsprintf(strFPS, TITLE L" | %d fps", count);
			t = clock();
			count = 0;
		}
		count++;
	}

	closegraph();
	return 0;
}

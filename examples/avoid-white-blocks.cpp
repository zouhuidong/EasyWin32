//////////////////////////////////////////////
//
//	示例代码来自：
//	https://codebus.cn/funcoding/chapter3
//	童晶老师《C 和 C++ 游戏趣味编程》
//	第3章 别碰白块
//
//	编译环境：HiEasyX (Traditional) Ver0.5.1 | EasyX_20240601 | Visual Studio 2022
//

#include <hieasyx.h>  
#define _kbhit()	/* 仅用于兼容，不推荐使用 */ (HiEasyX::peekmessageHX(nullptr, EX_CHAR, false, nullptr))
#define _getch()	/* 仅用于兼容，不推荐使用 */ (HiEasyX::getmessageHX(EX_CHAR, nullptr).value.ch)
#define kbhit		/* 仅用于兼容，不推荐使用 */ _kbhit
#define getch		/* 仅用于兼容，不推荐使用 */ _getch

#include <stdio.h>
#include <cstdlib>  

int main()
{
	float width, height, gravity; 
	float ball_x, ball_y, ball_vy, radius; 
	float rect_left_x, rect_top_y, rect_width, rect_height, rect_vx; 
	int score = 0; 
	int isBallOnFloor = 1; 

	width = 600.0f;  
	height = 400.0f; 
	gravity = 0.6f;  
	initgraph(static_cast<int>(width), static_cast<int>(height)); 

	radius = 20.0f; 
	ball_x = width / 4.0f; 
	ball_y = height - radius;  
	ball_vy = 0.0f;  

	rect_height = 100.0f; 
	rect_width = 20.0f; 
	rect_left_x = width * 3.0f / 4.0f; 
	rect_top_y = height - rect_height; 
	rect_vx = -3.0f; 

	while (1) 
	{
		if (kbhit()) 
		{
			TCHAR input = _getch(); 
			if (input == _T(' ') && isBallOnFloor == 1) 
			{
				ball_vy = -17.0f; 
				isBallOnFloor = 0; 
			}
		}

		ball_vy = ball_vy + gravity;  
		ball_y = ball_y + ball_vy;    
		if (ball_y >= height - radius)  
		{
			ball_vy = 0.0f;  
			ball_y = height - radius;  
			isBallOnFloor = 1; 
		}

		rect_left_x = rect_left_x + rect_vx; 
		if (rect_left_x <= 0.0f) 
		{
			rect_left_x = width; 
			score = score + 1; 
			
			rect_height = static_cast<float>(rand() % static_cast<int>(height / 4.0f)) + height / 4.0f;
			
			rect_vx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.0f - 7.0f;
		}
		
		if ((rect_left_x <= ball_x + radius)
			&& (rect_left_x + rect_width >= ball_x - radius)
			&& (height - rect_height <= ball_y + radius))
		{
			Sleep(50); 
			score = 0; 
		}

		cleardevice();  
		fillcircle(static_cast<int>(ball_x), static_cast<int>(ball_y), static_cast<int>(radius));  
		
		fillrectangle(
			static_cast<int>(rect_left_x),
			static_cast<int>(height - rect_height),
			static_cast<int>(rect_left_x + rect_width),
			static_cast<int>(height)
		);
		TCHAR s[20]; 
		_stprintf_s(s, _T("%d"), score); 
		settextstyle(40, 0, _T("宋体")); 
		outtextxy(50, 30, s); 
		Sleep(10);  
	}
	closegraph();
	return 0;
}

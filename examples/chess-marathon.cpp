////////////////////////////////////////
// 程序：象棋马拉松
// 作者：Gary
// 编译环境：HiEasyX (Traditional) Ver0.5.1 | EasyX_20240601 | Visual Studio 2022
// 编写日期：2021-7-14

////////////////////////////// 游戏进程头文件 //////////////////////////////

# include <math.h>
# include <hieasyx.h>
# include <string>
# include <time.h>
# include <stdlib.h>

// 头文件
// 头文件编译开始
#ifndef CIRCLE_H
#define CIRCLE_H

// 定义变量
static double pi = acos(-1.0);			// 圆周率π
static HWND hOut;						// 画布

// 定义一个结构体，格子
struct Node1
{
	int boxnum;							// 格子编号
	LPTSTR text;						// 刻字
	COLORREF color;						// 颜色
	int piecenum;						// 棋子类型参数
	int posx, posy;						// 坐标参数
	int type;							// 格子激活状态，有无棋子坐落
};

// 定义一个结构体，可抵达格子
struct Node2
{
	int type;							// 格子激活状态，可否抵达
};

// 定义一个结构体，危险格子
struct Node3
{
	int type;							// 格子激活状态，是否危险
};

// 定义一个结构体，按钮
struct Node4
{
	int posx1, posy1, posx2, posy2;		// 坐标
	double r;							// 原按钮半径
	LPTSTR text;						// 按钮文本
	int type;							// 按钮激活状态
};

// 定义一个类，game
class game
{
public:
	// 函数
	int carry_main();													// 游戏进程主循函数
	void draw_piece(int posx, int posy, LPTSTR text, COLORREF color);	// 棋子绘制函数
	void draw_button();												// 按钮绘制函数
	void creact_piece(int floor);										// 棋子生成函数
	void check();														// 判断函数
	void check_main();													// 判断主函数
	void carry_prepare();												// 准备进程主循函数
	void initialization_prepare();										// 准备进程初始化函数	
	void draw_scene_prepare();											// 准备进程绘制函数
	void draw_word(double size, int posx, int posy, LPTSTR text);		// 标题绘制函数
	void carry_start();												// 开始进程主循函数
	void initialization_start();										// 开始进程初始化函数
	void draw_scene_start();											// 开始进程绘制函数
	void draw_bk();													// 背景绘制函数
	void draw_tip();													// 提示绘制函数
	// 结构体
	Node1 box[16][7];													// 格子
	Node2 box1[16][7];													// 可达格子
	Node3 box2[16][7];													// 危险格子
	Node4 boxm[20];														// 按钮，预制二十个

	// 变量
	int num_button;														// 按钮数量
	int exit_carry;														// 主进程主循函数控制参数
	int exit_start;														// 开始进程主循函数控制参数
	int mod_start;														// 游戏模式参数
	int flag;															// 当前控制移动格子参数
	int tip_or_not;														// 是否提示参数
	int selected_or_not;												// 闯关模式是否选定移动目标棋子
	int start_life;														// 生命条
	int start_record;													// 记录
	int start_eat;														// 食子数
};

// 头文件编译结束
#endif


////////////////////////////// 主程序源文件 //////////////////////////////


// 窗口初始化
void initialization()
{
	// 窗口定义，整个程序仅定义一次
	hOut = initgraph(450, 650);
	pi = acos(-1.0);
	// 窗口标题
	SetWindowText(hOut, _T("象棋马拉松"));
}

// 总进程主循函数
void carry()
{
	// 总进程参数初始化
	int exit_carry = 0;
	// 进程控制
	BeginBatchDraw();
	// 创建游戏类对象
	game G;
	// 游戏进程
	while (exit_carry == 0)
	{
		// 执行游戏总进程，并接收游戏进程结束时返回值
		// 准备进程时，点击退出按钮结束返回 1
		// 开始进程时，点击返回按钮结束返回 0
		exit_carry = G.carry_main();
	}
	EndBatchDraw();
	// 关闭窗口
	closegraph();
}

// 主函数
int main(void)
{
	// 初始化
	initialization();
	// 总进程主循函数
	carry();
	return 0;
}

////////////////////////////// 游戏进程源文件 //////////////////////////////



// 游戏进程主循函数
int game::carry_main()
{
	// 游戏进程参数初始化
	exit_carry = 0;
	// 随机种子初始化
	srand((unsigned)time(NULL));
	// 准备进程初始化函数
	initialization_prepare();
	// 准备进程主循函数
	carry_prepare();
	// 准备进程结束
	// 准备进程时，点击退出按钮结束，游戏进程参数为 1
	// 准备进程时，正常进入游戏，游戏进程参数为 0
	if (exit_carry == 0)
	{
		// 开始进程初始化函数
		initialization_start();
		// 开始进程主循函数
		carry_start();
	}
	// 开始进程结束
	// 点击返回按钮结束，或者正常游戏结束，游戏进程参数不变
	return exit_carry;
}

// 按钮绘制函数
void game::draw_button()
{
	// 根据颜色结构体设置参数
	setlinecolor(RGB(125, 125, 125));
	setfillcolor(BLACK);
	setbkcolor(BLACK);
	settextcolor(WHITE);
	settextstyle(30, 15, _T("Consolas"));
	setlinestyle(PS_SOLID, 10);

	// 根据按钮数量参数绘制
	for (int i = 0; i < num_button; i++)
	{
		if (boxm[i].type == 0)
		{
			// 边框
			fillroundrect(boxm[i].posx1, boxm[i].posy1, boxm[i].posx2, boxm[i].posy2, 25, 25);
			// 文字
			outtextxy(boxm[i].posx1 + (boxm[i].posx2 - boxm[i].posx1) / 2 - textwidth(boxm[i].text) / 2, boxm[i].posy1 + 15, boxm[i].text);
		}
	}
	FlushBatchDraw();
}


////////////////////////////// 准备进程源文件 //////////////////////////////


// 准备进程主循函数
void game::carry_prepare()
{
	int i;
	// 鼠标定义
	ExMessage m;
	// 准备进程控制参数初始化
	int	exit_prepare = 0;
	// 绘制界面
	draw_scene_prepare();
	// 主循开始
	while (exit_prepare == 0)
	{
		if (peekmessage(&m, EM_MOUSE | EM_KEY))
		{
			// 左键单击判断
			if (m.message == WM_LBUTTONDOWN)
			{
				// 判断是否点击了按钮，按钮状态是否为激活
				for (i = 0; i < num_button; i++)
				{
					if (m.x > boxm[i].posx1 && m.y > boxm[i].posy1 && m.x < boxm[i].posx2 && m.y < boxm[i].posy2 && boxm[i].type == 0)
					{
						break;
					}
				}

				// 开始游戏按钮
				if (i == 0 || i == 1 || i == 3)
				{
					// 游戏类型储存
					mod_start = i;
					// 准备进程控制参数置一，结束主循
					exit_prepare = 1;
					// 开始进程控制参数置零，进入开始进程
					exit_start = 0;
				}
				// 退出按钮
				else if (i == 2)
				{
					// 总进程控制参数置一，跳过开始进程
					exit_carry = 1;
					// 准备进程控制参数置一，结束主循
					exit_prepare = 1;
				}

				if (i < 3 && exit_prepare == 0)
				{
					// 点击按钮后，根据按钮激活参数，重新绘制界面
					draw_scene_prepare();
					FlushBatchDraw();
				}
			}
		}
	}
}

// 准备进程初始化函数	
void game::initialization_prepare()
{
	// 按钮设置初始化
	// 按钮数量参数初始化
	num_button = 3;

	// 按钮结构体参数初始化
	boxm[0].posx1 = 140; boxm[0].posy1 = 120; boxm[0].posx2 = 310; boxm[0].posy2 = 180; boxm[0].text = _T("无尽模式"); boxm[0].type = 0;
	boxm[1].posx1 = 140; boxm[1].posy1 = 220; boxm[1].posx2 = 310; boxm[1].posy2 = 280; boxm[1].text = _T("闯关模式"); boxm[1].type = 0;
	boxm[2].posx1 = 140; boxm[2].posy1 = 420; boxm[2].posx2 = 310; boxm[2].posy2 = 480; boxm[2].text = _T("退出"); boxm[2].type = 0;
}

// 准备进程绘制函数
void game::draw_scene_prepare()
{
	// 画布绘制
	setbkcolor(WHITE);
	cleardevice();
	// 按钮绘制
	draw_button();
	// 标题绘制
	draw_word(50, 130, 30, _T("象棋马拉松"));
	FlushBatchDraw();
}

// 文字绘制函数
void game::draw_word(double size, int posx, int posy, LPTSTR text)
{
	int i, j;

	// 参数设置，填充透明
	setbkmode(TRANSPARENT);
	settextstyle(int(size), int(size / 5 * 2), _T("Consolas"));

	// 背景色的反色
	settextcolor(BLACK);
	// 范围绘制，构造阴影
	for (i = posx - 3; i <= posx + 3; i++)
	{
		for (j = posy - 3; j <= posy + 3; j++)
		{
			outtextxy(i, j, text);
		}
	}

	// 背景色
	settextcolor(WHITE);
	// 在阴影中绘制
	outtextxy(posx, posy, text);

	// 恢复填充
	setbkmode(OPAQUE);
	FlushBatchDraw();
}

////////////////////////////// 开始进程源文件 //////////////////////////////


// 开始进程主循函数
void game::carry_start()
{
	// 鼠标定义
	ExMessage m;
	int i, j, k, x, y, a;
	// 判断一下，避免返回不初始化格子判断参数
	check_main();
	// 记录初始化
	start_record = 0;
	start_eat = 0;
	// 是否选定参数
	selected_or_not = 0;
	// 生命初始化，无尽模式初始一条命
	start_life = 1;
	// 绘制
	draw_scene_start();
	// 无尽模式
	while (exit_start == 0 && mod_start == 0)
	{
		if (peekmessage(&m, EM_MOUSE | EM_KEY))
		{
			// 左键单击判断
			if (m.message == WM_LBUTTONDOWN)
			{
				if (m.x < 350)
				{
					i = m.x / 50 + m.y / 50 * 7 + 7;

					// 是否是可抵达位置
					if (box1[i / 7][i % 7].type == 1)
					{
						// 交换落点
						if (box[i / 7][i % 7].type == 1) { start_eat++; }

						box[i / 7][i % 7].type = box[flag / 7][flag % 7].type;
						box[i / 7][i % 7].text = box[flag / 7][flag % 7].text;
						box[i / 7][i % 7].piecenum = box[flag / 7][flag % 7].piecenum;
						box[i / 7][i % 7].color = box[flag / 7][flag % 7].color;
						box[flag / 7][flag % 7].type = 0;

						flag = i;

						// 是否是安全位置
						// 安全位置
						if (box2[i / 7][i % 7].type == 0)
						{
							// 移动视角
							if (i / 7 < 11)
							{
								x = 11 - i / 7;
								start_record += x;
								for (j = 13; j >= 0; j--)
								{
									for (k = 0; k < 7; k++)
									{
										if (box[j][k].type != 0)
										{
											box[j + x][k].type = box[j][k].type;
											box[j + x][k].text = box[j][k].text;
											box[j + x][k].piecenum = box[j][k].piecenum;
											box[j + x][k].color = box[j][k].color;

											box[j][k].type = 0;
										}
									}
								}
								flag += x * 7;
								creact_piece(0);
								// 移动视角后的卷屏动画
								// 棋子已经整体下移 x 行了，要把格子整体上移 x 行，慢慢向原值靠拢
								a = tip_or_not;
								tip_or_not = 0;
								for (y = x * 50; y >= 0; y -= 50)
								{
									for (k = 0; k < 16; k++)
									{
										for (j = 0; j < 7; j++)
										{
											box[k][j].posy = -25 + 50 * k - y;
										}
									}
									draw_scene_start();
									Sleep(200);
								}
								tip_or_not = a;
							}

							// 清理棋子
							for (j = 0; j < 7; j++)
							{
								box[14][j].type = 0;
								box[15][j].type = 0;
							}
							// 重新判断
							check_main();
						}
						// 危险位置
						else if (box2[i / 7][i % 7].type == 1)
						{
							// 失败动画
							tip_or_not = 1;
							for (k = 0; k < 5; k++)
							{
								box[i / 7][i % 7].type = 0;
								draw_scene_start();
								Sleep(200);
								box[i / 7][i % 7].type = 2;
								draw_scene_start();
								Sleep(200);
							}

							exit_start = 1;
						}

						draw_scene_start();

					}
				}
				else
				{
					// 判断是否点击了按钮
					for (i = 0; i < num_button; i++)
					{
						if (m.x > boxm[i].posx1 && m.y > boxm[i].posy1 && m.x < boxm[i].posx2 && m.y < boxm[i].posy2 && boxm[i].type == 0)
						{
							break;
						}
					}
					if (i == 0)
					{
						exit_start = 1;

					}
				}
			}

			// 右键打开提示
			else if (m.message == WM_RBUTTONDOWN)
			{
				tip_or_not = tip_or_not == 0 ? 1 : 0;
				draw_scene_start();
			}
		}
	}


	// 生命初始化，闯关模式初始三条命
	start_life = 3;
	// 绘制
	draw_scene_start();
	// 闯关模式
	while (exit_start == 0 && mod_start == 1)
	{
		if (peekmessage(&m, EM_MOUSE | EM_KEY))
		{
			// 左键单击判断
			if (m.message == WM_LBUTTONDOWN)
			{
				if (m.x < 350)
				{
					i = m.x / 50 + m.y / 50 * 7 + 7;
					// 未选定状态，点击自己的棋子
					if (selected_or_not == 0 && box[i / 7][i % 7].type == 2)
					{
						selected_or_not = 1;
						flag = i;
						// 更换棋子类型
						for (k = 0; k < 16; k++)
						{
							for (j = 0; j < 7; j++)
							{
								if (box[k][j].type == 2)
								{
									box[k][j].type = 3;
								}
							}
						}
						box[flag / 7][flag % 7].type = 2;
						check_main();
						draw_scene_start();
					}
					// 选定状态，点击选定的棋子
					else if (selected_or_not == 1 && i == flag)
					{
						selected_or_not = 0;
						for (k = 0; k < 16; k++)
						{
							for (j = 0; j < 7; j++)
							{
								if (box[k][j].type == 3)
								{
									box[k][j].type = 2;
								}
							}
						}
						check_main();
						draw_scene_start();
					}
					// 选定状态，点击其他的己方棋子，切换选择
					else if (selected_or_not == 1 && box[i / 7][i % 7].type == 3)
					{
						box[i / 7][i % 7].type = 2;
						box[flag / 7][flag % 7].type = 3;
						flag = i;
						check_main();
						draw_scene_start();
					}
					// 选定状态，点击其他的格子
					else if (selected_or_not == 1 && i != flag)
					{
						// 是否是可抵达位置
						if (box1[i / 7][i % 7].type == 1)
						{

							selected_or_not = 0;

							// 交换落点
							if (box[i / 7][i % 7].type == 1) { start_eat++; }

							box[i / 7][i % 7].type = box[flag / 7][flag % 7].type;
							box[i / 7][i % 7].text = box[flag / 7][flag % 7].text;
							box[i / 7][i % 7].piecenum = box[flag / 7][flag % 7].piecenum;
							box[i / 7][i % 7].color = box[flag / 7][flag % 7].color;
							box[flag / 7][flag % 7].type = 0;


							for (k = 0; k < 16; k++)
							{
								for (j = 0; j < 7; j++)
								{
									if (box[k][j].type == 3)
									{
										box[k][j].type = 2;
									}
								}
							}
							// 重新判断
							check_main();

							// 是否成功过关，三个子都抵达最上面
							x = 0;
							for (k = 0; k < 16; k++)
							{
								for (j = 0; j < 7; j++)
								{
									if (box[k][j].type == 2 && k > 1)
									{
										x = 1;
									}
								}
							}

							// 三个字在不在危险位置
							for (k = 0; k < 16; k++)
							{
								for (j = 0; j < 7; j++)
								{
									if (box[k][j].type == 2 && box2[k][j].type == 1 && x == 1)
									{
										tip_or_not = 1;
										// 失败动画
										for (y = 0; y < 5; y++)
										{
											box[k][j].type = 0;
											draw_scene_start();
											Sleep(200);
											box[k][j].type = 2;
											draw_scene_start();
											Sleep(200);
										}
										tip_or_not = 0;
										// 减生命
										start_life--;
										// 生命减没了，结束
										if (start_life == 0)
										{
											exit_start = 1;
										}
									}
								}
							}

							draw_scene_start();

							// 成功过关
							if (x == 0)
							{
								start_record += 1;
								start_life++;
								// 初始化
								initialization_start();

								// 不出现两个子及以上同时危险的情况
								y = 0;
								if (box2[11][1].type == 1) { y++; }
								if (box2[11][3].type == 1) { y++; }
								if (box2[11][5].type == 1) { y++; }
								while (y >= 2)
								{
									// 初始化
									initialization_start();
									y = 0;
									if (box2[11][1].type == 1) { y++; }
									if (box2[11][3].type == 1) { y++; }
									if (box2[11][5].type == 1) { y++; }
								}
							}
						}
						draw_scene_start();
					}
				}
				else
				{
					// 判断是否点击了按钮
					for (i = 0; i < num_button; i++)
					{
						if (m.x > boxm[i].posx1 && m.y > boxm[i].posy1 && m.x < boxm[i].posx2 && m.y < boxm[i].posy2 && boxm[i].type == 0)
						{
							break;
						}
					}
					// 返回按钮
					if (i == 0)
					{
						exit_start = 1;
					}
					// 刷新按钮
					else if (i == 1)
					{
						// 初始化
						initialization_start();

						// 不出现两个子及以上同时危险的情况
						y = 0;
						if (box2[11][1].type == 1) { y++; }
						if (box2[11][3].type == 1) { y++; }
						if (box2[11][5].type == 1) { y++; }

						while (y >= 2)
						{
							// 初始化
							initialization_start();
							y = 0;
							if (box2[11][1].type == 1) { y++; }
							if (box2[11][3].type == 1) { y++; }
							if (box2[11][5].type == 1) { y++; }
						}

						// 重新绘制
						draw_scene_start();
					}
				}
			}

			// 右键切换提示
			else if (m.message == WM_RBUTTONDOWN)
			{
				tip_or_not = tip_or_not == 0 ? 1 : 0;
				check_main();
				draw_scene_start();
			}
		}
	}

	flushmessage(EM_MOUSE);
}


// 开始进程初始化函数
void game::initialization_start()
{
	int i, j, k;
	// 格子初始化
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 7; j++)
		{
			box[i][j].boxnum = i * 7 + j;
			box[i][j].color = WHITE;
			box[i][j].piecenum = 0;
			box[i][j].text = _T("");
			box[i][j].posx = 25 + 50 * j;
			box[i][j].posy = -25 + 50 * i;
			box[i][j].type = 0;
		}
	}
	// 提示参数初始化
	tip_or_not = 0;
	// 无尽模式
	if (mod_start == 0)
	{
		box[11][3].type = 2;
		box[11][3].piecenum = 3;
		box[11][3].text = _T("马");
		box[11][3].color = RED;
		flag = 80;
		creact_piece(1);

		// 判断初始化
		check_main();
	}
	// 闯关模式
	else if (mod_start == 1)
	{
		k = 2;
		while (k >= 2)
		{
			// 格子初始化
			for (i = 0; i < 16; i++)
			{
				for (j = 0; j < 7; j++)
				{
					box[i][j].color = WHITE;
					box[i][j].piecenum = 0;
					box[i][j].text = _T("");
					box[i][j].type = 0;
				}
			}
			// 红色车马炮
			box[11][1].type = 2;
			box[11][1].piecenum = 4;
			box[11][1].text = _T("车");
			box[11][1].color = RED;

			box[11][3].type = 2;
			box[11][3].piecenum = 3;
			box[11][3].text = _T("马");
			box[11][3].color = RED;

			box[11][5].type = 2;
			box[11][5].piecenum = 6;
			box[11][5].text = _T("炮");
			box[11][5].color = RED;

			flag = 82;
			// 黑色障碍
			for (i = 1; i < 6; i++)
			{
				creact_piece(i);
				creact_piece(i);
			}

			// 判断初始化
			check_main();

			k = 0;
			if (box2[11][1].type == 1) { k++; }
			if (box2[11][3].type == 1) { k++; }
			if (box2[11][5].type == 1) { k++; }
		}
	}

	// 按钮设置初始化
	num_button = 2;
	boxm[0].posx1 = 360; boxm[0].posy1 = 120; boxm[0].posx2 = 440; boxm[0].posy2 = 180; boxm[0].text = _T("返回"); boxm[0].type = 0;
	boxm[1].posx1 = 360; boxm[1].posy1 = 220; boxm[1].posx2 = 440; boxm[1].posy2 = 280; boxm[1].text = _T("刷新"); boxm[1].type = 1;

	// 闯关模式则打开刷新按钮
	if (mod_start == 1) { boxm[1].type = 0; }

}


// 开始进程绘制函数
void game::draw_scene_start()
{
	int i, j;

	// 背景绘制
	draw_bk();
	// 绘制按钮
	draw_button();
	// 棋子绘制
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 7; j++)
		{
			if (box[i][j].type != 0)
			{
				draw_piece(box[i][j].posx, box[i][j].posy, box[i][j].text, box[i][j].color);
			}
		}
	}
	if (tip_or_not == 1)
	{
		draw_tip();
	}
	// 闯关模式，选定状态，显示小箭头
	if (mod_start == 1 && selected_or_not == 1)
	{
		i = flag / 7;
		j = flag % 7;
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 2);

		line(box[i][j].posx + 20, box[i][j].posy + 20, box[i][j].posx + 26, box[i][j].posy + 26);
		line(box[i][j].posx + 20, box[i][j].posy + 20, box[i][j].posx + 20, box[i][j].posy + 26);
		line(box[i][j].posx + 20, box[i][j].posy + 20, box[i][j].posx + 26, box[i][j].posy + 20);

		line(box[i][j].posx + 20, box[i][j].posy - 20, box[i][j].posx + 26, box[i][j].posy - 26);
		line(box[i][j].posx + 20, box[i][j].posy - 20, box[i][j].posx + 20, box[i][j].posy - 26);
		line(box[i][j].posx + 20, box[i][j].posy - 20, box[i][j].posx + 26, box[i][j].posy - 20);

		line(box[i][j].posx - 20, box[i][j].posy + 20, box[i][j].posx - 26, box[i][j].posy + 26);
		line(box[i][j].posx - 20, box[i][j].posy + 20, box[i][j].posx - 20, box[i][j].posy + 26);
		line(box[i][j].posx - 20, box[i][j].posy + 20, box[i][j].posx - 26, box[i][j].posy + 20);

		line(box[i][j].posx - 20, box[i][j].posy - 20, box[i][j].posx - 26, box[i][j].posy - 26);
		line(box[i][j].posx - 20, box[i][j].posy - 20, box[i][j].posx - 20, box[i][j].posy - 26);
		line(box[i][j].posx - 20, box[i][j].posy - 20, box[i][j].posx - 26, box[i][j].posy - 20);

	}
	// 文字绘制
	TCHAR s[20];
	// 记录
	if (start_record > 0) { draw_word(30, 350, 380, _T("记录：")); }
	_stprintf_s(s, _T("%0.0d"), start_record);
	draw_word(30, 410, 380, s);

	// 食子
	if (start_eat > 0) { draw_word(30, 350, 430, _T("食子：")); }
	_stprintf_s(s, _T("%0.0d"), start_eat);
	draw_word(30, 410, 430, s);

	// 生命
	if (start_life > 0) { draw_word(30, 350, 480, _T("生命：")); }
	_stprintf_s(s, _T("%0.0d"), start_life);
	draw_word(30, 410, 480, s);


	FlushBatchDraw();
}


// 背景绘制函数
void game::draw_bk()
{
	// 填充
	int i = 225;
	setbkcolor(RGB(215 - 45.0 / 180.0 * double(i), 161 - 39.0 / 180.0 * double(i), 114 - 32.0 / 180.0 * double(i)));
	cleardevice();

	// 底色
	for (i = 225; i > 0; i--)
	{
		setlinecolor(RGB(215 - 45.0 / 180.0 * double(i), 161 - 39.0 / 180.0 * double(i), 114 - 32.0 / 180.0 * double(i)));
		setfillcolor(RGB(215 - 45.0 / 180.0 * double(i), 161 - 39.0 / 180.0 * double(i), 114 - 32.0 / 180.0 * double(i)));
		fillcircle(175, 325, 145 + i);
	}

	// 底色
	for (i = 0; i < 145; i++)
	{
		setlinecolor(RGB(215 + 10.0 / 145.0 * double(i), 161 + 7.0 / 145.0 * double(i), 114 + 5.0 / 145.0 * double(i)));
		setfillcolor(RGB(215 + 10.0 / 145.0 * double(i), 161 + 7.0 / 145.0 * double(i), 114 + 5.0 / 145.0 * double(i)));
		fillcircle(175, 325, 145 - i);
	}

	// 格子线
	setlinestyle(PS_SOLID, 3);
	setlinecolor(RGB(154, 101, 49));

	// 横线
	for (i = 0; i < 13; i++)
	{
		line(25, 25 + 50 * i, 325, 25 + 50 * i);
	}

	if (mod_start == 1)
	{
		i = 0;
		setlinecolor(YELLOW);
		line(25, 25 + 50 * i, 325, 25 + 50 * i);
	}
	// 竖线
	setlinecolor(RGB(154, 101, 49));
	for (i = 0; i < 7; i++)
	{
		line(25 + 50 * i, 0, 25 + 50 * i, 650);
	}
	line(15, 0, 15, 650);
	line(335, 0, 335, 650);

	FlushBatchDraw();
}


// 判断主函数
void game::check_main()
{
	int i, j, k, x, y;
	// 临时储存两个结构体的值
	int eat1[16][7], eat2[16][7];
	// 参数重置
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 7; j++)
		{
			box1[i][j].type = 0;
			box2[i][j].type = 0;
		}
	}
	// 先整体判断一次
	check();
	// 存起来
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 7; j++)
		{
			eat1[i][j] = box1[i][j].type;
			eat2[i][j] = box2[i][j].type;
		}
	}

	// 纠正，对两个值都是一的格子进行纠正，每个格子，要么是可抵达且安全，要么是危险
	// 纠正的原因是“炮”子导致的
	if (selected_or_not == 1)
	{
		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 7; j++)
			{
				// 对可抵达且危险的格子进行纠正
				if (eat1[i][j] == 1 && eat2[i][j] == 1)
				{
					// 先假设选定棋子（flag）走到该格
					k = box[i][j].type;
					box[i][j].type = 2;
					box[flag / 7][flag % 7].type = 0;

					// 参数重置
					for (x = 0; x < 16; x++)
					{
						for (y = 0; y < 7; y++)
						{
							box1[x][y].type = 0;
							box2[x][y].type = 0;
						}
					}

					// 重新判断
					check();

					// 依旧是危险格，则是危险，但不改变可抵达状态
					if (box2[i][j].type == 1)
					{
						// 若改变为不可抵达，则不能落子到危险格了，避免了操作失误
						// eat1[i][j]=0;
					}
					// 否则就是安全格
					else
					{
						eat2[i][j] = 0;
					}

					// 棋子还原
					box[i][j].type = k;
					box[flag / 7][flag % 7].type = 2;
				}
			}
		}

		// 纠正，反取
		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 7; j++)
			{
				box1[i][j].type = eat1[i][j];
				box2[i][j].type = eat2[i][j];
			}
		}
	}
}


// 判定落点，安全点函数
void game::check()
{
	int i, j, k;

	for (i = 1; i < 14; i++)
	{
		for (j = 0; j < 7; j++)
		{
			if (box[i][j].type == 1)
			{
				switch (box[i][j].piecenum)
				{
					// 兵
				case 1:
				{
					// 下方
					box2[i + 1][j].type = 1;
					// 左方
					if (j > 0) { box2[i][j - 1].type = 1; }
					// 右方
					if (j < 6) { box2[i][j + 1].type = 1; }
					break;
				}
				// 相
				case 2:
				{
					// 不能憋象腿，四角
					if ((j) > 1 && box[i - 1][j - 1].type == 0) { box2[i - 2][j - 2].type = 1; }
					if ((j) > 1 && box[i + 1][j - 1].type == 0) { box2[i + 2][j - 2].type = 1; }
					if ((j) < 5 && box[i - 1][j + 1].type == 0) { box2[i - 2][j + 2].type = 1; }
					if ((j) < 5 && box[i + 1][j + 1].type == 0) { box2[i + 2][j + 2].type = 1; }
					break;
				}
				// 马
				case 3:
				{
					// 不能憋马腿，八角
					if (j > 1 && box[i][j - 1].type == 0) { box2[i - 1][j - 2].type = 1; box2[i + 1][j - 2].type = 1; }
					if (j < 5 && box[i][j + 1].type == 0) { box2[i - 1][j + 2].type = 1; box2[i + 1][j + 2].type = 1; }

					if (box[i - 1][j].type == 0 && j > 0) { box2[i - 2][j - 1].type = 1; }
					if (box[i - 1][j].type == 0 && j < 6) { box2[i - 2][j + 1].type = 1; }

					if (box[i + 1][j].type == 0 && j > 0) { box2[i + 2][j - 1].type = 1; }
					if (box[i + 1][j].type == 0 && j < 6) { box2[i + 2][j + 1].type = 1; }
					break;
				}
				// 车
				case 4:
				{
					// 四条直线，阻碍则止
					// 左
					k = j;
					while ((k - 1) >= 0 && box[i][k - 1].type != 1) { box2[i][k - 1].type = 1; k--; }
					if ((k - 1) >= 0 && box[i][k - 1].type == 1) { box2[i][k - 1].type = 1; }
					// 右
					k = j;
					while ((k + 1) <= 6 && box[i][k + 1].type != 1) { box2[i][k + 1].type = 1; k++; }
					if ((k + 1) <= 6 && box[i][k + 1].type == 1) { box2[i][k + 1].type = 1; }
					// 上
					k = i;
					while ((k - 1) >= 0 && box[k - 1][j].type != 1) { box2[k - 1][j].type = 1; k--; }
					if ((k - 1) >= 0 && box[k - 1][j].type == 1) { box2[k - 1][j].type = 1; }
					// 下
					k = i;
					while ((k + 1) <= 13 && box[k + 1][j].type != 1) { box2[k + 1][j].type = 1; k++; }
					if ((k + 1) <= 13 && box[k + 1][j].type == 1) { box2[k + 1][j].type = 1; }
					break;
				}
				// 士
				case 5:
				{
					// 四角
					if (j > 0) { box2[i + 1][j - 1].type = 1; box2[i - 1][j - 1].type = 1; }
					if (j < 6) { box2[i + 1][j + 1].type = 1; box2[i - 1][j + 1].type = 1; }
					break;
				}
				// 炮
				case 6:
				{
					// 架炮后与车一样
					// 四条直线，遇阻碍则开始，再遇则结束
					// 左
					k = j;
					while ((k - 1) >= 0 && box[i][k - 1].type == 0) { k--; }
					if ((k - 1) >= 0 && box[i][k - 1].type != 0)
					{
						k--;
						while ((k - 1) >= 0 && box[i][k - 1].type != 1) { box2[i][k - 1].type = 1; k--; }
						if ((k - 1) >= 0 && box[i][k - 1].type == 1) { box2[i][k - 1].type = 1; }
					}
					// 右
					k = j;
					while ((k + 1) <= 6 && box[i][k + 1].type == 0) { k++; }
					if ((k + 1) <= 6 && box[i][k + 1].type != 0)
					{
						k++;
						while ((k + 1) <= 6 && box[i][k + 1].type != 1) { box2[i][k + 1].type = 1; k++; }
						if ((k + 1) <= 6 && box[i][k + 1].type == 1) { box2[i][k + 1].type = 1; }
					}
					// 上
					k = i;
					while ((k - 1) >= 0 && box[k - 1][j].type == 0) { k--; }
					if ((k - 1) >= 0 && box[k - 1][j].type != 0)
					{
						k--;
						while ((k - 1) >= 0 && box[k - 1][j].type != 1) { box2[k - 1][j].type = 1; k--; }
						if ((k - 1) >= 0 && box[k - 1][j].type == 1) { box2[k - 1][j].type = 1; }
					}
					// 下
					k = i;
					while ((k + 1) <= 13 && box[k + 1][j].type == 0) { k++; }
					if ((k + 1) <= 13 && box[k + 1][j].type != 0)
					{
						k++;
						while ((k + 1) <= 13 && box[k + 1][j].type != 1) { box2[k + 1][j].type = 1; k++; }
						if ((k + 1) <= 13 && box[k + 1][j].type == 1) { box2[k + 1][j].type = 1; }
					}
					break;
				}
				default:break;
				}
			}
			else if (box[i][j].type == 2)
			{
				switch (box[i][j].piecenum)
				{
					// 兵
				case 1:
				{
					// 下方
					box1[i - 1][j].type = 1;
					// 左方
					if (j > 0) { box1[i][j - 1].type = 1; }
					// 右方
					if (j < 6) { box1[i][j + 1].type = 1; }
					break;
				}
				// 相
				case 2:
				{
					// 不能憋象腿，四角
					if ((j) > 1 && box[i - 1][j - 1].type == 0) { box1[i - 2][j - 2].type = 1; }
					if ((j) > 1 && box[i + 1][j - 1].type == 0) { box1[i + 2][j - 2].type = 1; }
					if ((j) < 5 && box[i - 1][j + 1].type == 0) { box1[i - 2][j + 2].type = 1; }
					if ((j) < 5 && box[i + 1][j + 1].type == 0) { box1[i + 2][j + 2].type = 1; }
					break;
				}
				// 马
				case 3:
				{
					// 不能憋马腿，八角
					if (j > 1 && box[i][j - 1].type == 0) { box1[i - 1][j - 2].type = 1; box1[i + 1][j - 2].type = 1; }
					if (j < 5 && box[i][j + 1].type == 0) { box1[i - 1][j + 2].type = 1; box1[i + 1][j + 2].type = 1; }

					if (box[i - 1][j].type == 0 && j > 0) { box1[i - 2][j - 1].type = 1; }
					if (box[i - 1][j].type == 0 && j < 6) { box1[i - 2][j + 1].type = 1; }

					if (box[i + 1][j].type == 0 && j > 0) { box1[i + 2][j - 1].type = 1; }
					if (box[i + 1][j].type == 0 && j < 6) { box1[i + 2][j + 1].type = 1; }
					break;
				}
				// 车
				case 4:
				{
					// 四条直线，阻碍则止
					// 左
					k = j;
					while ((k - 1) >= 0 && box[i][k - 1].type == 0) { box1[i][k - 1].type = 1; k--; }
					if ((k - 1) >= 0 && box[i][k - 1].type == 1) { box1[i][k - 1].type = 1; }
					// 右
					k = j;
					while ((k + 1) <= 6 && box[i][k + 1].type == 0) { box1[i][k + 1].type = 1; k++; }
					if ((k + 1) <= 6 && box[i][k + 1].type == 1) { box1[i][k + 1].type = 1; }
					// 上
					k = i;
					while ((k - 1) >= 0 && box[k - 1][j].type == 0) { box1[k - 1][j].type = 1; k--; }
					if ((k - 1) >= 0 && box[k - 1][j].type == 1) { box1[k - 1][j].type = 1; }
					// 下
					k = i;
					while ((k + 1) <= 13 && box[k + 1][j].type == 0) { box1[k + 1][j].type = 1; k++; }
					if ((k + 1) <= 13 && box[k + 1][j].type == 1) { box1[k + 1][j].type = 1; }
					break;
				}
				// 士
				case 5:
				{
					// 四角
					if (j > 0) { box1[i + 1][j - 1].type = 1; box1[i - 1][j - 1].type = 1; }
					if (j < 6) { box1[i + 1][j + 1].type = 1; box1[i - 1][j + 1].type = 1; }
					break;
				}
				// 炮
				case 6:
				{
					// 架炮前与车一样
					// 四条直线，阻碍则可以打下一个阻碍
					// 左
					k = j;
					while ((k - 1) >= 0 && box[i][k - 1].type == 0) { box1[i][k - 1].type = 1; k--; }
					if ((k - 1) >= 0 && box[i][k - 1].type != 0)
					{
						k--;
						while ((k - 1) >= 0 && box[i][k - 1].type == 0) { k--; }
						if ((k - 1) >= 0 && box[i][k - 1].type == 1)
						{
							box1[i][k - 1].type = 1;
						}
					}
					// 右
					k = j;
					while ((k + 1) <= 6 && box[i][k + 1].type == 0) { box1[i][k + 1].type = 1; k++; }
					if ((k + 1) <= 6 && box[i][k + 1].type != 0)
					{
						k++;
						while ((k + 1) <= 6 && box[i][k + 1].type == 0) { k++; }
						if ((k + 1) <= 6 && box[i][k + 1].type == 1)
						{
							box1[i][k + 1].type = 1;
						}
					}
					// 上
					k = i;
					while ((k - 1) >= 0 && box[k - 1][j].type == 0) { box1[k - 1][j].type = 1; k--; }
					if ((k - 1) >= 0 && box[k - 1][j].type != 0)
					{
						k--;
						while ((k - 1) >= 0 && box[k - 1][j].type == 0) { k--; }
						if ((k - 1) >= 0 && box[k - 1][j].type == 1)
						{
							box1[k - 1][j].type = 1;
						}
					}
					// 下
					k = i;
					while ((k + 1) <= 13 && box[k + 1][j].type == 0) { box1[k + 1][j].type = 1; k++; }
					if ((k + 1) <= 13 && box[k + 1][j].type != 0)
					{
						k++;
						while ((k + 1) <= 13 && box[k + 1][j].type == 0) { k++; }
						if ((k + 1) <= 13 && box[k + 1][j].type == 1)
						{
							box1[k + 1][j].type = 1;
						}
					}
					break;
				}
				default:break;
				}
			}
		}
	}
}


// 提示绘制函数
void game::draw_tip()
{

	int i, j;

	for (i = 1; i < 14; i++)
	{
		for (j = 0; j < 7; j++)
		{
			// 是可达位，或危险位
			if (box2[i][j].type == 1 || box1[i][j].type == 1)
			{
				// 优先危险显示
				if (box2[i][j].type == 1)
				{
					setlinecolor(BLACK);
				}
				// 可抵达显示
				else if (box1[i][j].type == 1)
				{
					setlinecolor(LIGHTGREEN);
				}
				setlinestyle(PS_SOLID, 2);
				// 空位显示架空十字架
				if (box[i][j].type == 0)
				{
					line(box[i][j].posx + 5, box[i][j].posy + 5, box[i][j].posx + 5, box[i][j].posy + 10);
					line(box[i][j].posx + 5, box[i][j].posy + 5, box[i][j].posx + 10, box[i][j].posy + 5);

					line(box[i][j].posx + 5, box[i][j].posy - 5, box[i][j].posx + 5, box[i][j].posy - 10);
					line(box[i][j].posx + 5, box[i][j].posy - 5, box[i][j].posx + 10, box[i][j].posy - 5);

					line(box[i][j].posx - 5, box[i][j].posy + 5, box[i][j].posx - 5, box[i][j].posy + 10);
					line(box[i][j].posx - 5, box[i][j].posy + 5, box[i][j].posx - 10, box[i][j].posy + 5);

					line(box[i][j].posx - 5, box[i][j].posy - 5, box[i][j].posx - 5, box[i][j].posy - 10);
					line(box[i][j].posx - 5, box[i][j].posy - 5, box[i][j].posx - 10, box[i][j].posy - 5);
				}
				// 非空位显示小方框
				else
				{
					line(box[i][j].posx + 15, box[i][j].posy + 15, box[i][j].posx + 15, box[i][j].posy + 10);
					line(box[i][j].posx + 15, box[i][j].posy + 15, box[i][j].posx + 10, box[i][j].posy + 15);

					line(box[i][j].posx + 15, box[i][j].posy - 15, box[i][j].posx + 15, box[i][j].posy - 10);
					line(box[i][j].posx + 15, box[i][j].posy - 15, box[i][j].posx + 10, box[i][j].posy - 15);

					line(box[i][j].posx - 15, box[i][j].posy + 15, box[i][j].posx - 15, box[i][j].posy + 10);
					line(box[i][j].posx - 15, box[i][j].posy + 15, box[i][j].posx - 10, box[i][j].posy + 15);

					line(box[i][j].posx - 15, box[i][j].posy - 15, box[i][j].posx - 15, box[i][j].posy - 10);
					line(box[i][j].posx - 15, box[i][j].posy - 15, box[i][j].posx - 10, box[i][j].posy - 15);
				}
			}
		}
	}
	FlushBatchDraw();
}


// 棋子绘制
void game::draw_piece(int posx, int posy, LPTSTR text, COLORREF color)
{
	int i, j;

	setlinestyle(PS_SOLID, 1);
	// 阴影
	setlinecolor(RGB(121, 75, 52));
	setfillcolor(RGB(121, 75, 52));
	fillcircle(posx, posy + 7, 20);

	// 侧面
	setlinecolor(RGB(212, 158, 111));
	setfillcolor(RGB(212, 158, 111));
	for (i = 0; i < 3; i++)
	{
		fillcircle(posx, posy + i + 4, 18);
	}

	// 正面
	setlinecolor(RGB(246, 203, 155));
	setfillcolor(RGB(246, 203, 155));
	fillcircle(posx, posy, 15);

	// 刻字
	setlinecolor(color);

	setlinestyle(PS_SOLID, 2);
	setfillstyle(BS_NULL);
	fillcircle(posx, posy, 14);
	setfillstyle(BS_SOLID);


	settextcolor(color);
	setbkmode(TRANSPARENT);
	settextstyle(22, 11, _T("Consolas"));
	for (i = 0; i < 1; i++)
	{
		for (j = 0; j < 2; j++)
		{
			outtextxy(posx - 10 + i, posy - 10 + j, text);
		}
	}
	setbkmode(OPAQUE);

	FlushBatchDraw();
}


// 棋子生成函数，已知行数
void game::creact_piece(int floor)
{
	int i, k;

	// 随机列数
	do
	{
		i = rand() % 7;
	} while (box[floor][i].type != 0);

	box[floor][i].type = 1;
	box[floor][i].color = BLACK;

	// 随机棋子
	k = rand() % 6 + 1;
	box[floor][i].piecenum = k;

	switch (k)
	{
	case 1:box[floor][i].text = _T("兵"); break;
	case 2:box[floor][i].text = _T("相"); break;
	case 3:box[floor][i].text = _T("马"); break;
	case 4:box[floor][i].text = _T("车"); break;
	case 5:box[floor][i].text = _T("士"); break;
	case 6:box[floor][i].text = _T("炮"); break;
	case 7:box[floor][i].text = _T(""); break;
	default:break;
	}
}


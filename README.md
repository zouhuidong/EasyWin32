# EasyWin32
![License](https://img.shields.io/github/license/zouhuidong/EasyWin32)
![Downloads](https://img.shields.io/github/downloads/zouhuidong/EasyWin32/total)
![GitHub Version](https://img.shields.io/github/v/release/zouhuidong/EasyWin32)

EasyX 库的 Win32 拓展版：解锁多窗口、可以使用 Win32 控件

## Table of Contents

* [EasyWin32](#easywin32)
  * [介绍](#介绍)
  * [编译环境](#编译环境)
  * [配置此库](#配置此库)
  * [开始使用](#开始使用)
    * [Win32 消息派发式](#win32-消息派发式)
    * [顺序代码结构](#顺序代码结构)
  * [注意事项](#注意事项)
  * [更新日志](#更新日志)


## 介绍

这个库实现了 EasyX 创建多窗口，以及对 Win32 控件的支持。

您觉得这不可思议吗？其实背后的原理很简单。

这个拓展库的原理是：创建 Win32 窗口，然后将 EasyX 的绘制内容刷新到 Win32 窗口上，以实现在 Win32 应用上进行 EasyX 绘图。

其中使用了我之前写的一个简陋的库 AHGraphics，项目地址：https://github.com/zouhuidong/AHGraphics

但是受限于 EasyX 的绘图机制，也就是每次只能对同一个 IMAGE 对象进行绘制，拖慢了多窗口绘图的效率。但是 EasyX 之所以 Easy，也肯定没必要搞那么复杂，所以这点小事算不了什么。

各位看官着急试试了吧？那么先上图看看效果吧：

![示例图片](https://github.com/zouhuidong/EasyX_Win32Ctrl/blob/main/screenshot/2.png)

![示例图片](https://github.com/zouhuidong/EasyX_Win32Ctrl/blob/main/screenshot/3.png)

图片对应的示例程序可以在 samples 文件夹中找到，包括源码和可执行程序，源码中的注释除了 EasyX 绘图函数的调用之外都写的比较详细了。

## 编译环境

VisualStudio 2022 | EasyX_20220116 | Windows 10

*您必须已经配置 EasyX 图形库，如未配置，请访问图形库官网 https://easyx.cn

## 配置此库

配置此库很简单，只需要将 lib 文件夹中的所有文件都复制到您的项目目录下，然后添加到您的项目中即可。

在程序中包含头文件 `EasyWin32.h` 来使用此库，并且此库使用了命名空间 `EasyWin32`。

一个最简单的完整示例（位于 ./Samples/Start）：
```cpp
#include "EasyWin32.h"
#include <conio.h>

int main()
{
	EasyWin32::initgraph_win32();			// 初始化窗口

	outtextxy(20, 20, L"Hello EasyWin32");	// 绘制文本

	FLUSH_DRAW();							// EasyWin32 默认使用双缓冲绘图，此处输出绘图缓冲

	_getch();								// 按任意键继续

	EasyWin32::closegraph_win32();			// 关闭窗口
	return 0;
}

```

## 开始使用

该库支持您使用顺序代码结构和 Win32 消息派发的代码结构。

顺序代码结构的具体例子：https://github.com/zouhuidong/EasyWin32/blob/main/samples/Sample2/main.cpp

Win32 消息派发的代码结构，其对应的具体例子：https://github.com/zouhuidong/EasyWin32/blob/main/samples/Sample1/main.cpp

例子中对 EasyWin32 的调用做了详细的说明。

### Win32 消息派发式

如果您希望在程序中使用 Win32 控件，则您需要写一个简化版的 Win32 过程函数，就像下面这样：

```cpp
#include "EasyWin32.h"

// 窗口过程函数
bool WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
	default: return true; break;	// 使用默认方法处理其余消息
	}
	return false;
}

int main()
{
	EasyWin32::initgraph_win32(640, 480, 0, L"", WndProc);	// 创建窗口，并指定窗口过程函数
	EasyWin32::init_end();									// 在 Win32 消息派发的代码结构下，创建完窗口后必须用此函数阻塞
	return 0;
}

```

窗口过程函数的返回值是布尔类型，标识是否需要系统默认方法处理该消息。

需要注意的是：

1. 接受 WM_CREATE 消息时，wParam 和 lParam 是空的，你无法获得 CREATESTRUCT 结构体信息。

2. 如果自行处理 WM_CLOSE 消息，则无需编写关闭窗口的代码，但最后函数必须返回 true，否则可能造成内存泄漏。

然后在 `WndProc` 中创建按钮（可以是其他控件，需要运用 Win32 的知识，这里以按钮举例）：

```cpp
#include "EasyWin32.h"

// 控件 ID
#define IDC_BTN 100

// 窗口过程函数
bool WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
	case WM_CREATE:

		// 创建按钮
		CreateWindow(L"button", L"Button",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			200, 100, 100, 60,
			hwnd, (HMENU)IDC_BTN, hInstance, NULL);

		break;
	default: return true; break;	// 使用默认方法处理其余消息
	}
	return false;
}

int main()
{
	EasyWin32::initgraph_win32(640, 480, 0, L"", WndProc);	// 创建窗口，并指定窗口过程函数
	EasyWin32::init_end();									// 在 Win32 消息派发的代码结构下，创建完窗口后必须用此函数阻塞
	return 0;
}

```

创建按钮完毕，接下来为其添加按键响应：

```cpp
#include "EasyWin32.h"

// 控件 ID
#define IDC_BTN 100

// 存储文本
wchar_t str[128] = { 0 };

// 窗口过程函数
bool WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
	case WM_CREATE:

		// 创建按钮
		CreateWindow(L"button", L"Button",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			200, 100, 100, 60,
			hwnd, (HMENU)IDC_BTN, hInstance, NULL);

		break;

	case WM_PAINT:

		BEGIN_DRAW(hwnd);		// 将绘图窗口设为自己，并启动一次绘图任务
		setbkcolor(0xf0f0f0);	// 设置背景色
		settextcolor(BLUE);		// 设置文本色
		cleardevice();			// 清屏
		outtextxy(20, 20, str);	// 输出文字
		END_DRAW();				// 结束此次绘图任务

		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BTN:	// 按下按钮

			BEGIN_DRAW(hwnd);
			wsprintf(str, L"按下了按钮");
			
			// 在非 WM_PAINT 消息区域，如需要立即刷新，需要强制重绘
			EasyWin32::EnforceRedraw();
			
			END_DRAW();

			break;
		}

	default: return true; break;	// 使用默认方法处理其余消息
	}
	return false;
}

int main()
{
	EasyWin32::initgraph_win32(640, 480, 0, L"", WndProc);	// 创建窗口，并指定窗口过程函数
	EasyWin32::init_end();									// 在 Win32 消息派发的代码结构下，创建完窗口后必须用此函数阻塞
	return 0;
}
```

执行效果：

![示例图片](https://github.com/zouhuidong/EasyX_Win32Ctrl/blob/main/screenshot/4.png)

需要注意的是：在窗口过程函数中绘图，每次绘图操作，无论是设置绘图属性还是绘图，都需要在每次操作前使用 `BEGIN_DRAW(窗口句柄)`，并在每次操作结束时使用 `END_DRAW()`。

这样做是为了协调多个窗口同时存在的情况，多个窗口可能抢占绘图权，所以需要您在每次绘图前后调用这些宏，使得多个窗口有序绘图。

注意，如果编写 Win32 消息派发式的代码，可能需要您具备一些 Win32 编程基础。

### 顺序代码结构

这应该是大家喜闻乐见的写法，下面的代码来自 https://github.com/zouhuidong/EasyWin32/blob/main/samples/Sample2/main.cpp:
```cpp
////////////////////////////////
//
//	EasyWin32 使用示例
//
//	1. 使用顺序代码结构
//	2. 创建了多个绘图窗口
//	3. 展示了鼠标消息的获取
//

#include "EasyWin32.h"
#include <time.h>

int main()
{
	// 设置随机种子
	srand((unsigned)time(NULL));

	// 创建两个绘图窗口
	HWND hWnd1 = EasyWin32::initgraph_win32(640, 480, false, L"字符阵");
	HWND hWnd2 = EasyWin32::initgraph_win32(640, 480, false, L"鼠标操作示例");

	// 设置窗口 1 绘图样式
	EasyWin32::SetWorkingWindow(hWnd1);
	settextstyle(16, 8, _T("Courier"));
	settextcolor(GREEN);

	while (true)
	{
		// 若窗口 1 还存在（未被关闭）
		if (EasyWin32::isAliveWindow(hWnd1))
		{
			// 设置窗口 1 为目标绘图窗口
			EasyWin32::SetWorkingWindow(hWnd1);

			// 绘制内容：EasyX 官方示例“字符阵”（简化）
			cleardevice();
			for (int i = 0; i <= 200; i++)
			{
				// 在随机位置显示三个随机字母
				for (int j = 0; j < 3; j++)
				{
					int x = (rand() % 80) * 8;
					int y = (rand() % 20) * 24;
					char c = (rand() % 26) + 65;
					outtextxy(x, y, c);
				}
			}

			// EasyWin32 默认使用双缓冲绘图，此处输出绘图缓冲
			FLUSH_DRAW();
		}

		// 窗口 2
		if (EasyWin32::isAliveWindow(hWnd2))
		{
			EasyWin32::SetWorkingWindow(hWnd2);

			// 绘制内容：EasyX 官方示例“鼠标操作”（有改动）
			ExMessage m;
			if (EasyWin32::MouseHit_win32())
			{
				// 获取一条鼠标消息
				m = EasyWin32::GetMouseMsg_win32();
				switch (m.message)
				{
				case WM_MOUSEMOVE:
					// 鼠标移动的时候画红色的小点
					putpixel(m.x, m.y, RED);
					break;

				case WM_LBUTTONDOWN:
					// 如果点左键的同时按下了 Ctrl 键
					if (m.ctrl)
						// 画一个大方块
						rectangle(m.x - 10, m.y - 10, m.x + 10, m.y + 10);
					else
						// 画一个小方块
						rectangle(m.x - 5, m.y - 5, m.x + 5, m.y + 5);
					break;
				}
			}

			FLUSH_DRAW();
		}

		// 若所有窗口都被关闭，则结束程序
		if (!EasyWin32::isAnyWindow())
		{
			break;
		}

		// 降低 CPU 占用
		if (rand() % 777 == 0)
			Sleep(1);
	}

	return 0;
}

```

代码其实很简单，注释也很详细，这种“顺序代码结构”与上面那种 Win32 式的代码结构最大的区别就是不需要创建 `WndProc`，即窗口过程函数。

接下来我为大家剖析这段代码。

**判断窗口存在**

首先，由于创建了两个窗口，那么在主循环的绘图中，您需要分两个 `if` 语句分别判断窗口是否还存在，因为窗口可能被用户关闭。若窗口还存在，才继续绘图。

---

**设置绘图窗口、输出绘图缓冲**

在 `if` 语句内部，首先需要调用 `EasyWin32::SetWorkingWindow()` 来设置当前的绘图窗口，由于是顺序代码结构，不会出现两个绘图任务抢占绘图权的情况，所以不需要使用 `BEGIN_DRAW()` 和 `END_DRAW()` 宏。在设置绘图窗口后，可以尽情绘图，绘图结束后需要调用 `FLUSH_DRAW()` 输出您的绘图缓冲。

需要注意的是，EasyX 原生的 `BeginBatchDraw` 一系列函数都是面向绘图窗口的，也就是说，如果处在 IMAGE 对象内部，`BeginBatchDraw` 系列函数会出错，所以请不要在 EasyWin32 中使用这些函数。

但是考虑到兼容性问题，我将他们都重新进行了宏定义，`BeginBatchDraw` 和 `EndBatchDraw` 都将无意义，而 `FlushBatchDraw` 则等同于 `FLUSH_DRAW`。

区别于 EasyX，EasyWin32 中的 `BEGIN_DRAW` 和 `END_DRAW` 组成一套，用在 Win32 式代码结构中，表示进行一个绘图任务；而 `FLUSH_DRAW` 用于顺序代码结构，表示完成绘图，并输出绘图缓冲。

---

**鼠标消息、键盘消息**

鼠标消息方面，虽然使用的是 ExMessage 结构，但是目前只提供获取鼠标消息的方法，暂不提供其他类型消息的获取功能（如 EM_KEY, EM_WINDOW 等）。

由于只能获取鼠标消息，故函数名称来自 MOUSEMSG 系列函数，这是旧版 EasyX 中的函数。

按键消息支持直接使用 `_getch` 一系列函数获取。

---

**容易忽略的程序结束判定**

在程序主循环的末尾，调用了 `EasyWin32::isAnyWindow()` 判断了是否还存在已经被创建的窗口，这条判断很容易被忽略，如果所有窗口都被关闭，则跳出循环并关闭程序。

如果不对是否还存在窗口进行判断的话，所有窗口都被关闭后，`main` 函数仍会继续运行，但是不在 Windows 任务栏中显示，会残留在后台进程中。

## 注意事项

一部分需要注意的内容都在“开始使用”中结合例子描述完毕，需要补充的是，由于 EasyWin32 绕开了 EasyX 原生的 `initgraph` 函数，所以在调用需要 `IMAGE` 对象指针的函数时，不可以传入 `NULL`，而需要传入您的窗口的 `IMAGE` 对象指针。

诸如以下函数都默认会传入 `IMAGE* pImg = NULL`：`GetImageBuffer`，`SetWorkingImage`，`GetImageHDC` 等等。

## 更新日志

**Ver2.1** (2022.02.07)

1. 新增对创建多窗口的支持，且可以直接创建模态窗口

2. 修复 Win32 控件显示不稳定的 Bug

3. 新增对 `_getch` 等一系列控制台输入函数的支持

4. 新增对获取鼠标消息 `ExMessage` 的支持

**Ver1.0** (2020.12.06)

First version.

支持在 EasyX 中使用 Win32 控件。


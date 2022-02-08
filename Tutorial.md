# 开始使用 EasyWin32

EasyWin32 相对 EasyX 的新增函数并不多，易于上手。

该库支持您使用顺序代码结构和 Win32 消息派发的代码结构。

顺序代码结构的具体例子：https://github.com/zouhuidong/EasyWin32/blob/main/samples/Sample2/main.cpp

Win32 消息派发的代码结构，其对应的具体例子：https://github.com/zouhuidong/EasyWin32/blob/main/samples/Sample1/main.cpp

## Win32 消息派发式

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
	EasyWin32::init_end();					// 在 Win32 消息派发的代码结构下，创建完窗口后必须用此函数阻塞
	return 0;
}

```

窗口过程函数的返回值是布尔类型，标识是否需要使用系统默认方法处理该消息。

需要注意的是：

1. 接受 WM_CREATE 消息时，wParam 和 lParam 是空的，你无法获得 CREATESTRUCT 结构体信息。

2. 如果自行处理 WM_CLOSE 消息，则无需编写关闭窗口的代码，但最后函数必须返回 true，否则可能造成内存泄漏。

由于 `main` 函数中调用了 `EasyWin32::init_end()` 函数进行阻塞，这个函数会自动判断窗口是否还存在，所以不需要再使用 `EasyWin32::isAnyWindow()` 进行判断。

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

注意，如果编写 Win32 消息派发式的代码，您需要具备一些 Win32 编程基础。

## 顺序代码结构

这应该是大家喜闻乐见的写法，因为通常使用 EasyX 的程序都是这样编写的。下面的代码来自 https://github.com/zouhuidong/EasyWin32/blob/main/samples/Sample2/main.cpp:
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
			if (peekmessage(&m, EM_MOUSE))	// 若成功获取一条鼠标消息
			{
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

鼠标消息兼容旧版 MOUSEMSG，同时支持 ExMessage（ExMessage 系列函数暂时只能获取 EM_MOUSE 消息，即鼠标消息）。

按键消息支持直接使用 `_getch` 一系列函数获取。

---

**容易忽略的程序结束判定**

在程序主循环的末尾，调用了 `EasyWin32::isAnyWindow()` 判断了是否还存在已经被创建的窗口，如果所有窗口都被关闭，则跳出循环并关闭程序。

这条判断很容易被忽略，如果不对是否还存在窗口进行判断的话，所有窗口都被关闭后，`main` 函数仍会继续运行，但是不在 Windows 任务栏中显示，会残留在后台进程中。

或者您可以使用阻塞函数 `EasyWin32::init_end()`，这个函数在“Win32 消息派发式”的示例代码中已经讲过。

## 其他注意事项

一部分需要注意的内容都在“开始使用”中结合例子描述完毕，需要补充的是，由于 EasyWin32 绕开了 EasyX 原生的 `initgraph` 函数，所以在调用需要 `IMAGE` 对象指针的函数时，不可以传入 `NULL`，而需要传入您的窗口的 `IMAGE` 对象指针。

诸如以下函数都默认会传入 `IMAGE* pImg = NULL`：`GetImageBuffer`，`SetWorkingImage`，`GetImageHDC` 等等。

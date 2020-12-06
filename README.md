# EasyX_Win32Ctrl
Let easyx lib supports win32 ctrl. 让EasyX库支持Win32控件。

---

EasyX 在2020.12.6之前一直不支持Win32控件（据说明年会支持？），我就写了这样一个库来使使用EasyX的同时使用Win32控件。

原理是将EasyX绘制的图像BitBlt到自己创建的win32程序上，这样Win32程序就和EasyX共存了。

效果：
![image](https://github.com/zouhuidong/EasyX_Win32Ctrl/blob/main/screenshot/scrshot.png)

<br>

这里面用到我之前写的一个AHGraphics.h，详见http://huidong.xyz/?mode=2&id=116


## 使用示例

创建窗口时，请使用`initgraph_win32()`：
```
int main()
{
	// 创建一个支持win32控件的图形界面
	initgraph_win32(640, 480, 0, WndProc);
	return 0;
}
```

函数原型：
```
// 创建支持win32的绘图窗口
// w,h				窗口大小
// mode				窗口模式（为0表示隐藏cmd，不为0表示显示cmd）
// WindowProcess	窗口消息处理函数的指针（bool类型，返回true表示由系统处理该消息，返回false表示系统不需要再处理该消息）
// strWndTitle		窗口标题
void initgraph_win32(int w = 640, int h = 480, int mode = 0, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE) = NULL, LPCTSTR strWndTitle = L"EasyX with Win32");
```

正如上述代码所示，`main`函数中，在`initgraph_win32()`之后，就不要写代码了，因为它是阻塞的。

参数中的`WndProc`是函数名，表示用来处理窗口消息的函数，它的函数类型和参数必须是这样：

`bool WindowProcess (HWND, UINT, WPARAM, LPARAM, HINSTANCE);`

<br>

这个函数和win32原生的WndProc函数很像，只需要处理你需要的消息，然后返回true或false表示是否需要系统以默认方式来处理该消息即可。

<br>
<br>

main.cpp 中有完整的示例代码：
```
#include "EasyWin32.h"
#include <conio.h>

#define IDC_EDIT 100
#define IDC_BTN 101

HWND hEdit;
HWND hBtn;

wchar_t str[512] = L"Hello, EasyX.";

bool WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (msg)
	{
	case WM_CREATE:

		// 创建一个输入框，一个按钮
		hEdit = CreateWindow(L"edit", L"Edit at here.",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			130, 50, 200, 20,
			hwnd, (HMENU)IDC_EDIT, hInstance, NULL);

		hBtn = CreateWindow(L"button", L"Click Me!",
			WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			330, 50, 100, 20,
			hwnd, (HMENU)IDC_BTN, hInstance, NULL);

		// 在这里设置绘图属性
		setbkcolor(WHITE);
		setlinecolor(BLACK);
		setfillcolor(BLUE);
		settextcolor(GREEN);
		settextstyle(32, 0, L"system");

		break;

	case WM_PAINT:

		// 随便画点什么
		cleardevice();
		setlinestyle(0, 5);
		line(0, 0, 300, 100);
		line(0, 0, getwidth(), getheight());
		fillrectangle(200,200,600,400);
		outtextxy(130, 150, str);

		break;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{

		// 按下按钮
		case IDC_BTN:

			// 得到输入框文本并强制重绘
			GetWindowText(hEdit, str, 512);
			Redraw_win32();

			break;
		}

		break;

	default:
		// 需要系统来处理此消息
		return true;
		break;
	}

	// 已经处理过消息，无需系统再处理
	return false;
}

int main()
{
	// 创建一个支持win32控件的图形界面
	initgraph_win32(640, 480, 0, WndProc);
	return 0;
}

```

<br>

*注意：*

*不要使用easyx的GetMouseMsg等鼠标相关函数*

*不要使用SetWorkingImage(NULL);*

*不要使用GetHWnd()，请使用GetHWnd_win32()*

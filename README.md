# EasyWin32
![License](https://img.shields.io/github/license/zouhuidong/EasyWin32)
![Downloads](https://img.shields.io/github/downloads/zouhuidong/EasyWin32/total)
![GitHub Version](https://img.shields.io/github/v/release/zouhuidong/EasyWin32)

EasyX 库的 Win32 拓展版：解锁多窗口、可以使用 Win32 控件

## 介绍

这个库实现了将 EasyX 扩展出多窗口的支持，以及对 Win32 控件的支持。

您觉得这不可思议吗？其实背后的原理很简单。

这个拓展库的原理是：创建 Win32 窗口，然后将 EasyX 的绘制内容刷新到 Win32 窗口上，以实现在 Win32 应用上进行 EasyX 绘图。

其中使用了我之前写的一个简陋的库 AHGraphics，项目地址：https://github.com/zouhuidong/AHGraphics

但是受限于 EasyX 的绘图机制，也就是每次只能对同一个 IMAGE 对象进行绘制，拖慢了多窗口绘图的效率。但是毕竟 EasyX 是面向我们广大初学者的图形库，这一点其实也不那么重要了。

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

**两种代码结构的显著区别**

如果您希望在程序中使用 Win32 控件，则您需要写一个简化版的 Win32 过程函数，

<br>

*注意：*

*不要使用easyx的GetMouseMsg等鼠标相关函数*

*不要使用SetWorkingImage(NULL);*

*不要使用GetHWnd()，请使用GetHWnd_win32()*

# EasyWin32
![GitHub Version](https://img.shields.io/github/v/release/zouhuidong/EasyWin32)
[![Blog](https://img.shields.io/badge/blog-huidong.xyz-green.svg)](http://huidong.xyz)
[![EasyX](https://img.shields.io/badge/graphics-EasyX-orange.svg)](https://easyx.cn)
![Stars](https://img.shields.io/github/stars/zouhuidong/EasyWin32)
![Forks](https://img.shields.io/github/forks/zouhuidong/EasyWin32)
![Downloads](https://img.shields.io/github/downloads/zouhuidong/EasyWin32/total)

EasyX 库的 Win32 拓展版：解锁多窗口（同时创建多个绘图窗口）、可以使用 Win32 控件、可以拉伸窗口大小

对原生 EasyX 兼容性超强~~ 只需稍稍改动您的代码，就可以在原有项目上应用 EasyWin32。

> [查看教程](./Tutorial.md)

> [更新日志](./Changelog.md)

## 介绍

这个库实现了 EasyX 创建多窗口，以及对 Win32 控件的支持。

这个拓展库的原理是：创建 Win32 窗口，然后将 EasyX 的绘制内容刷新到 Win32 窗口上，以实现在 Win32 应用上进行 EasyX 绘图。

*受限于每次只能对同一个 IMAGE 对象进行绘制的机制，多窗口绘图的效率会受到微弱的影响。*

程序示例：

![示例图片](./screenshot/5.png)
![示例图片](./screenshot/6.png)
![示例图片](./screenshot/7.png)

图片对应的示例程序可以在 samples 文件夹中找到，包括源码和可执行程序，源码内附详细注释。

## 编译环境

VisualStudio 2022 | EasyX_20220116 | Windows 10

*您必须已经配置 EasyX 图形库，如未配置，请访问图形库官网 https://easyx.cn*

**注意：暂不支持 MingW 编译器，请使用 VisualStudio 编译项目。**

## 配置此库

配置此库很简单，只需要将 lib 文件夹中的所有文件都复制到您的项目目录下，然后添加到您的项目中即可。

在程序中包含头文件 `EasyWin32.h` 来使用此库，并且此库使用了命名空间 `EasyWin32`。

一个很简单的[完整示例](./samples/Start/main.cpp)：
```cpp
#include "EasyWin32.h"
#include <conio.h>

int main()
{
	EasyWin32::initgraph_win32();			// 初始化窗口

	// 启动一个绘图任务
	// 由于只创建了一个窗口，所以不需要指定目标绘图窗口
	// 否则就需要调用 BEGIN_TASK_WND() 宏，指定目标绘图窗口
	BEGIN_TASK();

	// 绘制文本
	settextstyle(32, 0, L"Consolas");
	settextcolor(LIGHTGREEN);
	outtextxy(20, 70, L"Hello EasyWin32");
	settextstyle(26, 0, L"system");
	settextcolor(GREEN);
	outtextxy(20, 110, L"Any key to continue");

	// EasyWin32 默认使用双缓冲绘图，此处输出绘图缓冲
	// 注意：一段绘图任务结束，必须以此宏结尾（即 BEGIN_TASK 和 END_TASK 必须连用）
	END_TASK();

	// 不在窗口过程函数的 WM_PAINT 消息内绘图时，必须强制重绘
	// 由于没有自定义窗口过程函数，所以当然也要调用此宏强制重绘
	FLUSH_DRAW();

	while (true)
	{
		// 按任意键关闭窗口
		if (_kbhit())
		{
			EasyWin32::closegraph_win32();	// 关闭窗口
			break;
		}

		// 若窗口被关闭，退出程序
		// 注意：这个判断经常被忽略，判断窗口是否还存在是很必要的
		if (!EasyWin32::isAnyWindow())
		{
			break;
		}

		Sleep(10);
	}

	return 0;
}

```

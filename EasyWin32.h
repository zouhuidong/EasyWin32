/////////////////////////////////////
//
//	EasyWin32.h
//	by huidong <mailkey@yeah.net>
//
//	基于EasyX图形库的Win32控件支持库
//	EasyX版本：2020.9.2版
//
//	创建时间：2020.12.6
//	最后修改：2020.12.6
//

#pragma once

#include <windows.h>
#include <easyx.h>

// 创建支持win32的绘图窗口
// w,h				窗口大小
// mode				窗口模式（为0表示隐藏cmd，不为0表示显示cmd）
// WindowProcess	窗口消息处理函数的指针（bool类型，返回true表示由系统处理该消息，返回false表示系统不需要再处理该消息）
// strWndTitle		窗口标题
void initgraph_win32(int w = 640, int h = 480, int mode = 0, bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE) = NULL, LPCTSTR strWndTitle = L"EasyX with Win32");

// 得到窗口句柄
HWND GetHWnd_win32();

// 强制重绘窗口（正常在WM_PAINT消息内绘图不需要使用此函数）
void Redraw_win32();

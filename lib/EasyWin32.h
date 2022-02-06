/////////////////////////////////////////////////////////////////////
//
//	EasyWin32.h
//	基于 EasyX 图形库的 Win32 控件支持库
//
//	作　　者：huidong <huidong_mail@163.com>
//	版　　本：Ver 2.0
//	编译环境：VisualStudio 2022 | EasyX_20220116 | Windows 10 
//	创建日期：2020.12.06
//	最后修改：2022.02.06
//

#pragma once

#include <easyx.h>
#include <vector>
#include <string>


#define EASY_WIN32_BEGIN	namespace EasyWin32 {
#define EASY_WIN32_END		};

EASY_WIN32_BEGIN

////////////****** 结构体定义 ******////////////

// 窗口
struct EasyWindow
{
	HWND hWnd;			// 窗口句柄
	IMAGE* pImg;		// 窗口图像
	IMAGE* pBufferImg;	// 缓冲区
	bool(*funcWndProc)(HWND, UINT, WPARAM, LPARAM, HINSTANCE);	// 窗口消息处理函数
	std::vector<ExMessage> vecMouseMsg;	// 鼠标消息队列
	int nGetMouseMsgIndex;	// 获取鼠标消息的进度索引（现在获取到了数组中的哪一条）
	bool isNewSize;			// 窗口大小是否改变

	bool isSentCreateMsg;	// 是否发送了 WM_CREATE 的消息
};


////////////****** 窗体相关函数 ******////////////

// 创建支持 win32 的绘图窗口（默认支持窗口双击消息）
// w, h				窗口大小
// isCmd			是否显示 cmd 窗口（如果是控制台应用程序）
// strWndTitle		窗口标题
// WindowProcess	窗口消息处理函数的指针，为空表示使用默认消息处理函数（详见头文件中“窗口消息处理函数规范”）
// hParent			父窗口句柄，为空则表示该窗口独立存在（若填写，该窗口将作为模态窗口存在）
HWND initgraph_win32(
	int w = 640,
	int h = 480,
	bool isCmd = 0,
	LPCTSTR strWndTitle = L"",
	bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE) = NULL,
	HWND hParent = NULL
);

//
// 窗口消息处理函数规范
// 
// 函数标准形态：bool WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance);
// 
// 返回值：
// true		表示使用系统默认方法处理该消息
// false	表示不再需要系统默认方法处理该消息
// 
// 注意事项：
// 1. 接受 WM_CREATE 消息时，wParam 和 lParam 是空的，你无法获得 CREATESTRUCT 结构体信息
// 2. 如果自行处理 WM_CLOSE 消息，则无需编写关闭窗口的代码，但最后函数必须返回 true，否则可能造成内存泄漏
//

// 关闭某一绘图窗口，若句柄为 NULL 则关闭所有绘图窗口
void closegraph_win32(HWND hWnd = NULL);

// 结束初始化窗口（阻塞）
void init_end();

// 是否还存在未销毁的绘图窗口
bool isAnyWindow();

// 判断一窗口是否在已创建的窗口列表中（不含已被关闭的窗口）
bool isInListWindow(HWND hWnd);

// 判断一窗口是否还存在
// 同 isInListWindow
inline bool isAliveWindow(HWND hWnd)
{
	return isInListWindow(hWnd);
}

// 得到当前绘图窗口的句柄
HWND GetHWnd_win32();

// 得到当前绘图窗口的详细信息
EasyWindow GetWorkingWindow();

// 设置当前绘图窗口（同时设置绘图对象为窗口对应的 IMAGE 对象），返回是否设置成功
// 若有绘图任务进行中，则等待当前绘图任务完成
bool SetWorkingWindow(HWND hWnd);

// 强制重绘当前绘图窗口（正常在 WM_PAINT 消息内绘图不需要使用此函数）
void EnforceRedraw();

// 宣告开始一次绘制
void ReadyToDraw();

// 输出绘图缓冲，并表示当前绘图任务告一段落
void FlushDrawing();

// 获取已创建的窗口的数组（不含已被关闭的窗口）
std::vector<EasyWindow> GetCreatedWindowList();

// 绘图窗口大小是否改变
bool isWindowSizeChanged();

////////////****** 鼠标消息相关函数 ******////////////

// 检查是否存在鼠标消息
bool MouseHit_win32();

// 阻塞等待，直到获取到一个新的鼠标消息
ExMessage GetMouseMsg_win32();

// 获取一个鼠标消息，并立即返回
bool PeekMouseMsg_win32(ExMessage* pMsg);

// 清空鼠标消息
void FlushMouseMsg_win32();

//
//	鼠标消息相关备注：
//
//	虽然使用的是 ExMessage 结构，但是目前只提供获取鼠标消息的方法。
//	暂不提供其他类型消息的获取功能（如 EM_KEY, EM_WINDOW 等）
// 
//	由于只能获取鼠标消息，故函数名称来自 MOUSEMSG 系列函数，这是旧版 EasyX 中的函数。
//


////////////****** EasyX 原生函数的宏替换 ******////////////

#define initgraph(w, h)			initgraph_win32(w, h)
#define closegraph				closegraph_win32

#define BeginBatchDraw
#define FlushBatchDraw
#define EndBatchDraw

// 获取当前绘图窗口句柄
#define GetHWnd GetHWnd_win32

EASY_WIN32_END


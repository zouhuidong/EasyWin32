/////////////////////////////////////////////////////////////////////
//
//	EasyWin32.h
//	基于 EasyX 图形库的 Win32 拓展库
//
//	作　　者：huidong <mailhuid@163.com>
//	版　　本：Ver 3.1.0
//	编译环境：VisualStudio 2022 | EasyX_20220610 | Windows 10 
//	项目地址：https://github.com/zouhuidong/EasyWin32
//	创建日期：2020.12.06
//	最后修改：2022.07.15
//

#pragma once

#define _EASYWIN32_

#include <graphics.h>
#include <stdarg.h>
#include <vector>
#include <string>
#include <thread>

// 补充绘图窗口初始化参数
// 普通窗口
#define EW_NORMAL 0

// 托盘消息
#define WM_TRAY	(WM_USER + 100)


#define EASY_WIN32_BEGIN	namespace EasyWin32 {
#define EASY_WIN32_END		};

EASY_WIN32_BEGIN

////////////****** 类型定义 ******////////////

// 窗口
struct EasyWindow
{
	bool isAlive;							// 窗口是否存在

	HWND hWnd;								// 窗口句柄
	HWND hParent;							// 父窗口句柄

	IMAGE* pImg;							// 窗口图像
	IMAGE* pBufferImg;						// 图像缓冲区

	bool(*funcWndProc)						// 窗口消息处理函数
		(HWND, UINT, WPARAM, LPARAM, HINSTANCE);

	std::vector<ExMessage> vecMessage;		// 模拟 EasyX 窗口消息队列

	bool isUseTray;							// 是否使用托盘
	NOTIFYICONDATA nid;						// 托盘信息
	bool isUseTrayMenu;						// 是否使用托盘菜单
	HMENU hTrayMenu;						// 托盘菜单
	void(*funcTrayMenuProc)(UINT);			// 托盘菜单消息处理函数
		// 给出此函数是为了方便响应托盘的菜单消息
		// 如需响应完整的托盘消息，请自定义窗口过程函数并处理 WM_TRAY 消息

	bool isNewSize;							// 窗口大小是否改变
	bool isBusyProcessing;					// 是否正忙于处理内部消息（指不允许用户启动任务的情况）

	int nSkipPixels;						// 绘制时跳过的像素点数量（降质性速绘）
};

////////////****** 窗体相关函数 ******////////////

// 创建支持 win32 的绘图窗口（默认支持窗口双击消息）
// w, h				窗口大小
// flag				窗口样式标识
// strWndTitle		窗口标题
// WindowProcess	窗口消息处理函数的指针，为空表示使用默认消息处理函数（详见头文件中“窗口消息处理函数规范”）
// hParent			父窗口句柄，为空则表示该窗口独立存在（若填写，该窗口将成为模态窗口）
HWND initgraph_win32(
	int w = 640,
	int h = 480,
	int flag = EW_NORMAL,
	LPCTSTR strWndTitle = L"",
	bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE) = NULL,
	HWND hParent = NULL
);

//
// 窗口消息处理函数规范
// 
//	函数标准形态：
//		bool WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance);
// 
//	注意：
//		相比于标准的 Win32 窗口过程函数，增加了一个 HINSTANCE 类型形参。
//		返回值的意义也不相同，见下文。
// 
//	返回值：
//		true	表示使用系统默认方法处理该消息
//		false	表示不再需要系统默认方法处理该消息
// 
//	注意事项：
//		1. 接受 WM_CREATE 消息时，wParam 和 lParam 是空的，你无法获得 CREATESTRUCT 结构体信息
//		2. 接受 WM_CLOSE 消息时，返回 true 或 false 表示是否关闭窗口，但如果关闭窗口，您无需编写销毁窗口的代码
//

// 关闭某一绘图窗口，若句柄为 NULL 则关闭所有绘图窗口
void closegraph_win32(HWND hWnd = NULL);

// 结束初始化窗口（阻塞）
void init_end();

// 设置：当窗口都被销毁时，自动退出程序
void AutoExit();

// 是否还存在未销毁的绘图窗口
bool isAnyWindow();

// 判断一窗口是否还存在（不含已被关闭的窗口）
// 传入空句柄可以标识当前活动窗口
bool isAliveWindow(HWND hWnd = NULL);

// 得到当前绘图窗口的句柄
HWND GetHWnd_win32();

// 阻塞等待绘图任务完成
// 如果传入句柄，则只有该句柄窗口是活动窗口时才等待
void WaitForTask(HWND hWnd = NULL);

// 得到当前绘图窗口的详细信息
EasyWindow GetWorkingWindow();

// 等待当前任务完成并设置活动窗口，返回是否设置成功
bool SetWorkingWindow(HWND hWnd);

// 设置加速绘制跳过多少像素点
// 此加速效果是有损的，加速效果与跳过的像素点数正相关。
void QuickDraw(UINT nSkipPixels);

// 强制重绘当前绘图窗口（在 WM_PAINT 消息内绘图不需要使用此函数）
void EnforceRedraw();

// 为当前活动窗口启动任务，返回是否启动成功（若已在任务中也返回 true）
// 调用 EasyX 函数进行绘图或获取消息时，都应该先启动任务再进行调用。
bool BeginTask();

// 终止当前任务，输出绘图缓冲
void EndTask();

// 判断当前是否有任务在进行
// 若传入句柄，则额外判断它是否为活动窗口
bool isInTask(HWND hWnd = NULL);

// 判断某窗口的大小是否改变
// 传入空句柄可以标识当前活动窗口
bool isWindowSizeChanged(HWND hWnd = NULL);

// 为当前窗口创建一个托盘
// 传入托盘提示文本
// 注意：每个窗口仅能稳定占有一个托盘
void CreateTray(LPCTSTR lpTrayName);

// 删除某窗口的托盘，传入空指针可以标识当前活动窗口
void DeleteTray(HWND hWnd = NULL);

// 设置托盘菜单
void SetTrayMenu(HMENU hMenu);

// 设置托盘菜单消息处理函数
void SetTrayMenuProcFunc(void(*pFunc)(UINT));

// 判断自定义程序图标的启用状态
bool GetCustomIconState();

// 使用图标资源作为程序图标
// 参数传入图标资源 ID（大图标和小图标）
// 注：必须在第一次创建窗口前就调用该函数才能生效。默认情况下，程序将自绘 EasyX 程序图标
void SetCustomIcon(int nIcon, int nIconSm);

// 获取 EasyWin32 自绘默认窗口图标的 IMAGE
IMAGE GetDefaultIconImage();

// 在创建窗口前设置窗口样式，仅对此操作后首个新窗口生效
// 注意：新窗口的所有样式都将被当前样式覆盖
void PreSetWindowStyle(long lStyle);

// 在创建窗口前设置窗口位置，仅对此操作后首个新窗口生效
void PreSetWindowPos(int x, int y);

// 获取当前窗口样式
long GetWindowStyle();

// 设置当前窗口样式
int SetWindowStyle(long lNewStyle);

// 获取当前窗口扩展样式
long GetWindowExStyle();

// 设置当前窗口扩展样式
int SetWindowExStyle(long lNewExStyle);

////////////****** 消息相关函数 ******////////////

//// MOUSEMSG 式函数（兼容）

// 检查是否存在鼠标消息
bool MouseHit_win32();

// 阻塞等待，直到获取到一个新的鼠标消息
MOUSEMSG GetMouseMsg_win32();

// 获取一个新的鼠标消息，立即返回是否获取成功
bool PeekMouseMsg_win32(MOUSEMSG* pMsg, bool bRemoveMsg = true);

// 清空鼠标消息
void FlushMouseMsgBuffer_win32();

//// ExMessage 式函数

// 阻塞等待，直到获取到一个新消息
ExMessage getmessage_win32(BYTE filter = -1);

// 阻塞等待，直到获取到一个新消息
void getmessage_win32(ExMessage* msg, BYTE filter = -1);

// 获取一个消息，立即返回是否获取成功
bool peekmessage_win32(ExMessage* msg, BYTE filter = -1, bool removemsg = true);

// 清除所有消息记录
void flushmessage_win32(BYTE filter = -1);

//// 转换

// MOUSEMSG 转 ExMessage
ExMessage To_ExMessage(MOUSEMSG msg);

// ExMessage 转 MOUSEMSG
MOUSEMSG To_MouseMsg(ExMessage msgEx);

//// 消息类型判断

// 获取 ExMessage 的 EM 消息类型
UINT GetExMessageType(ExMessage msg);


EASY_WIN32_END

////////////****** 任务指令宏定义 ******////////////

// 启动一段（绘图）任务（绘制到当前绘图窗口）
#define BEGIN_TASK()\
	if (EasyWin32::isAliveWindow())\
	{\
		if (EasyWin32::BeginTask())\
		{(0)	/* 此处强制要求加分号 */

// 启动一段（绘图）任务（指定目标绘图窗口）
#define BEGIN_TASK_WND(hWnd)\
	/* 设置工作窗口时将自动等待任务 */\
	if (EasyWin32::SetWorkingWindow(hWnd))\
	{\
		if (EasyWin32::BeginTask())\
		{(0)

// 结束一段（绘图）任务，并输出绘图缓存（须与 BEGIN_TASK 连用）
#define END_TASK()\
			EasyWin32::EndTask();\
		}\
	}(0)

// 要求窗口重绘
#define FLUSH_DRAW()			EasyWin32::EnforceRedraw()

////////////****** 窗口样式宏定义 ******////////////

// 是否禁用当前窗口改变大小
#define DisableResizing(state)	(state ? EasyWin32::SetWindowStyle(EasyWin32::GetWindowStyle() & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX) :\
								EasyWin32::SetWindowStyle(EasyWin32::GetWindowStyle() | WS_SIZEBOX | WS_MAXIMIZEBOX))

// 是否禁用当前窗口的系统菜单
#define DisableSystemMenu(state)	(state ? EasyWin32::SetWindowStyle(EasyWin32::GetWindowStyle() & ~WS_SYSMENU) :\
									EasyWin32::SetWindowStyle(EasyWin32::GetWindowStyle() | WS_SYSMENU))

// 开启 / 关闭当前窗口的工具栏样式
#define EnableToolWindowStyle(state)	(state ? EasyWin32::SetWindowExStyle(EasyWin32::GetWindowExStyle() | WS_EX_TOOLWINDOW) :\
										EasyWin32::SetWindowExStyle(EasyWin32::GetWindowExStyle() & ~WS_EX_TOOLWINDOW))

////////////****** 键盘消息宏定义 ******////////////

// 判断当前活动窗口是否接受到某按键消息
#define KEY_DOWN(VK_NONAME) (GetForegroundWindow() == EasyWin32::GetHWnd_win32() && (GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

////////////****** EasyX 原生函数的宏替换 ******////////////

// 若使用 EasyX 原生函数创建窗口，则关闭窗口时自动退出程序
#define initgraph(...)			EasyWin32::initgraph_win32(__VA_ARGS__);\
								EasyWin32::AutoExit()

#define closegraph				EasyWin32::closegraph_win32

// 默认使用双缓冲，故 BeginBatchDraw 无意义
#define BeginBatchDraw()
#define FlushBatchDraw()		FLUSH_DRAW()
#define EndBatchDraw()			FLUSH_DRAW()

#define GetHWnd					EasyWin32::GetHWnd_win32

#define getmessage				EasyWin32::getmessage_win32
#define peekmessage				EasyWin32::peekmessage_win32
#define flushmessage			EasyWin32::flushmessage_win32

#define MouseHit				EasyWin32::MouseHit_win32
#define GetMouseMsg				EasyWin32::GetMouseMsg_win32
#define PeekMouseMsg			EasyWin32::PeekMouseMsg_win32
#define FlushMouseMsgBuffer		EasyWin32::FlushMouseMsgBuffer_win32

////////////****** 其他宏定义 ******////////////

#define rectangle_RECT(rct)			rectangle(rct.left,rct.top,rct.right,rct.bottom);
#define fillrectangle_RECT(rct)		fillrectangle(rct.left,rct.top,rct.right,rct.bottom);
#define solidrectangle_RECT(rct)	solidrectangle(rct.left,rct.top,rct.right,rct.bottom);


////////////****** 其他函数 ******////////////


// 精确延时函数(可以精确到 1ms，精度 ±1ms)
// by yangw80<yw80@qq.com>, 2011-5-4
void HpSleep(int ms);

// 在屏幕指定位置输出格式化文本
void outtextxy_format(int x, int y, int _Size, LPCTSTR lpFormat, ...);

// 得到 IMAGE 对象的 HBITMAP
HBITMAP GetImageHBitmap(IMAGE* img);

// HBITMAP 转 HICON
HICON HICONFromHBitmap(HBITMAP hBmp);

// 常用色彩扩展
enum COLORS {
	DARKBLUE = RGB(0x00, 0x00, 0x8B),
	DARKCYAN = RGB(0x00, 0x8B, 0x8B),
	DARKGOLDENROD = RGB(0xB8, 0x86, 0x0B),
	DARKGREEN = RGB(0x00, 0x64, 0x00),
	DARKRED = RGB(0x8B, 0x00, 0x00),
	DEEPPINK = RGB(0xFF, 0x14, 0x93),
	DEEPSKYBLUE = RGB(0x00, 0xBF, 0xFF),
	FORESTGREEN = RGB(0x22, 0x8B, 0x22),
	GOLD = RGB(0xFF, 0xD7, 0x00),
	GRAY = RGB(0x80, 0x80, 0x80),
	GREENYELLOW = RGB(0xAD, 0xFF, 0x2F),
	LIGHTPINK = RGB(0xFF, 0xB6, 0xC1),
	LIGHTSKYBLUE = RGB(0x87, 0xCE, 0xFA),
	LIGHTYELLOW = RGB(0xFF, 0xFF, 0xE0),
	DARKYELLOW = RGB(255, 201, 14),
	ORANGE = RGB(0xFF, 0xA5, 0x00),
	ORANGERED = RGB(0xFF, 0x45, 0x00),
	PINK = RGB(0xFF, 0xC0, 0xCB),
	PINKWHITE = RGB(255, 230, 250),
	PURPLE = RGB(0x80, 0x00, 0x80),
	SKYBLUE = RGB(0x87, 0xCE, 0xEB),
	SNOW = RGB(0xFF, 0xFA, 0xFA),
	SPRINGGREEN = RGB(0x00, 0xFF, 0x7F),
	STEELBLUE = RGB(0x46, 0x82, 0xB4),
	TOMATO = RGB(0xFF, 0x63, 0x47),
	WHITESMOKE = RGB(0xF5, 0xF5, 0xF5),
	YELLOWGREEN = RGB(0x9A, 0xCD, 0x32),

	// Windows 经典灰
	CLASSICGRAY = RGB(0xF0, 0xF0, 0xF0)
};


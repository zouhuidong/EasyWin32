////////////////////////////////////////////////////////////////////
//
//	AHGraphics.h (All HWnd graphics with EasyX)
//	任意绘制库：可使用 EasyX 在任何窗口上绘制
// 
//	作　　者：huidong <huidong_mail@163.com>
//	版　　本：Ver 2.0
//	编译环境：VisualStudio 2022 | EasyX_20220116 | Windows 10 
//	创建日期：2020.07.08
//	最后修改：2022.01.28
//

#pragma once
#include <easyx.h>

// 得到当前绘图目标窗口
HWND GetWorkingHWnd();

// 得到当前绘图目标HDC
HDC GetWorkingHDC();

// 得到主画布
IMAGE* GetMainImage();

// 将绘制在easyx中的内容显示到目标窗口上
// pImg 要显示的绘图对象，默认为主画布
// wnd 目标绘图窗口，默认为主窗口
void FlushDrawingToWnd(IMAGE* pImg = NULL, HWND wnd = NULL);

// 一直更新绘图内容（堵塞性的）
//
// delay 每次更新的延时（ms）
// flag 保持运作的标志，为false时停止
void KeepFlushDrawing(int delay, bool* flag);

// 使主IMAGE画布大小适应目标绘图窗口大小
void MainImageFitWindow();

// 对某一窗口绘图前的初始化
// wnd 窗口句柄
void InitDrawing(HWND wnd);

// 对某一HDC绘图前的初始化
// hdc DC句柄
void InitDrawing(HDC hdc);

// 开启 / 关闭 自动输出绘图内容（无需在绘图后手动FlushDrawingToWnd）
// 注意：除非必要，否则不要开启自动输出绘图内容
//
// isAuto 关闭还是开启自动输出，默认为true
void AutoFlushDrawing(bool isAuto = true);
/*

在例如cmd的窗口中，无法接受窗口WM_PAINT消息来FlushDrawingToWnd，就有必要开启AutoFlushDrawing，否则绘图内容容易被擦除

*/

// 结束绘图
// 处理一些结束绘图的后续工作
void EndDrawing();


//////////////////// 其它函数

// 得到鼠标相对于窗口的位置
POINT GetMousePoint();

// 得到窗口位置、区域
RECT GetWindowRect(HWND wnd);

// 得到窗口大小
POINT GetWindowSize(HWND wnd);

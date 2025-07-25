/**
 * @file		HiEasyX.h
 * @brief		HiEasyX (Traditional) 库头文件
 * @note		HiEasyX 是基于 EasyX 的扩展库，支持创建多窗口、透明抗锯齿绘图、系统 UI 组件等等。
 *
 * @version		Ver 0.5.1 (dev)
 * @envir		VisualStudio 2022 | EasyX_20240601 | Windows 11
 * @site		https://github.com/zouhuidong/HiEasyX
 *
 * @author		huidong
 * @qq			1442701429
 * @email		mailhuid@163.com
 *
 * @assistant	Alan-CRL
 * @qq			2685549821
 * @email		alan-crl@foxmail.com
 *
 * @create		2022.07.20
 * @update		2025.07.24
 *
 * @attention	如果使用MinGW和TDM等编译器，则需要添加链接选项:-leasyx -lmsimg32 -lgdiplus -lgdi32 -lwinmm
 * 				需要使用 C++14 及以上标准，编译时需选择 ISO C++14 (或以上标准)标准或添加编译选项:-std=c++14 (或以上标准)
*/

/********************************************************************
 * @attention <pre>
 *	注意：
 *		如果将基于原生 EasyX 编写的代码移植到 HiEasyX，
 *		必须使这些源代码都包含 HiEasyX.h 头文件，
 *		而不必包含 graphics.h，easyx.h 等原生 EasyX 头文件。
 *		否则，可能导致 GetImageBuffer(nullptr) 错误、GetHWnd() 错误等问题。
 * </pre>
 ********************************************************************/


#pragma once

#define __HIEASYX_H__

#include <HiEasyX/HiDef.h>
#include <HiEasyX/HiMacro.h>
#include <HiEasyX/HiMiscUtils.h>
#include <HiEasyX/HiFPS.h>
#include <HiEasyX/HiDrawingProperty.h>
#include <HiEasyX/HiMouseDrag.h>
#include <HiEasyX/HiMusicMCI.h>
#include <HiEasyX/HiGif.h>
#include <HiEasyX/HiGdiplus.h>
#include <HiEasyX/HiCanvas.h>
 //#include <HiEasyX/HiLayer.h>		// 暂时移除
 //#include <HiEasyX/HiGUI/HiGUI.h>	// 目前不可用

#include <HiEasyX/HiWindow.h>				// 不再支持 _NATIVE_EASYX_
#include <HiEasyX/HiSysGUI/HiSysGUI.h>

// HiEasyX 命名空间缩写
namespace hiex = HiEasyX;


#ifndef HIEASYX_LIB_EXPORT
#ifdef _UNICODE
	#ifdef _DEBUG
		#pragma comment(lib, "HiEasyXwd.lib")
	#else
		#pragma comment(lib, "HiEasyXw.lib")
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "HiEasyXad.lib")
	#else
		#pragma comment(lib, "HiEasyXa.lib")
	#endif
#endif
#endif


///////////////////////////////////////
//  EasyX 兼容性宏定义
///////////////////////////////////////

// 窗口相关函数
// 若使用 EasyX 原生函数创建窗口，则启用兼容性设置
#define initgraph				HiEasyX::initgraphCompatible
#define closegraph				HiEasyX::closegraphHX
#define GetHWnd					HiEasyX::GetHWndHX
#define Sleep					HiEasyX::SleepHX

// 绘图相关函数 
// 以下对支持传入 IMAGE* = nullptr 的函数进行重新定义，以解决无法定位到当前活动窗口的问题

#define GetImageBuffer			HiEasyX::GetImageBufferHX
#define GetImageHDC				HiEasyX::GetImageHDCHX
#define SetWorkingImage			HiEasyX::SetWorkingImageHX
// 无需重定义 GetWorkingImage 函数，因为他不传入 IMAGE* 参数

// 只对窗口画布有效
#define setorigin				HiEasyX::setoriginHX
#define getorigin				HiEasyX::getoriginHX
// 只对窗口画布有效
#define setaspectratio			HiEasyX::setaspectratioHX
#define getaspectratio			HiEasyX::getaspectratioHX

// 双缓冲相关函数
#define BeginBatchDraw			HiEasyX::BeginBatchDrawHX
#define FlushBatchDraw			HiEasyX::FlushBatchDrawHX
#define EndBatchDraw			HiEasyX::EndBatchDrawHX

// 消息相关函数
#define getmessage				HiEasyX::getmessageHX
#define peekmessage				HiEasyX::peekmessageHX
#define flushmessage			HiEasyX::flushmessageHX
#define MouseHit				HiEasyX::MouseHitHX
#define GetMouseMsg				HiEasyX::GetMouseMsgHX
#define PeekMouseMsg			HiEasyX::PeekMouseMsgHX
#define FlushMouseMsgBuffer		HiEasyX::FlushMouseMsgBufferHX

// 注意：不建议使用 _getch，_kbhit 系列函数，会导致 HiEasyX 窗口消息循环被阻塞，窗口卡死
// 如果一定要兼容老旧项目，可以定义下面的宏，且不要在程序中再包含 conio.h：
//		#define _kbhit()	/* 仅用于兼容，不推荐使用 */ (HiEasyX::peekmessageHX(nullptr, EX_CHAR, false, nullptr))
//		#define _getch()	/* 仅用于兼容，不推荐使用 */ (HiEasyX::getmessageHX(EX_CHAR, nullptr).value.ch)
//		#define kbhit		/* 仅用于兼容，不推荐使用 */ _kbhit
//		#define getch		/* 仅用于兼容，不推荐使用 */ _getch
// 使用这些宏会降低程序的可维护性，在项目变大时甚至会导致很多问题，
// 所以建议直接将代码中的 _getch，_kbhit 系列函数进行文本替换，以避免使用这些宏。

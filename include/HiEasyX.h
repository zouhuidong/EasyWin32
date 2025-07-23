/**
 * @file		HiEasyX.h
 * @brief		HiEasyX 库头文件
 * @note		HiEasyX 是基于 EasyX 的扩展库，支持创建多窗口、透明抗锯齿绘图、系统 UI 组件等等。
 *
 * @version		Ver 0.5.0 (dev)
 * @envir		VisualStudio 2022 | EasyX_20220901 | Windows 11
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
 * @update		2025.07.22
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

#include "HiDef.h"
#include "HiMacro.h"
#include "HiMiscUtils.h"
#include "HiFPS.h"
#include "HiDrawingProperty.h"
#include "HiMouseDrag.h"
#include "HiMusicMCI.h"
#include "HiGif.h"
#include "HiGdiplus.h"
#include "HiCanvas.h"
 //#include "HiEasyX/HiLayer.h"		// 暂时移除
 //#include "HiEasyX/HiGUI/HiGUI.h"	// 目前不可用

#include "HiWindow.h"				// 不再支持 _NATIVE_EASYX_
#include "HiSysGUI/HiSysGUI.h"

// 若使用 EasyX 原生函数创建窗口，则关闭窗口时自动退出程序
#define initgraph(...)			(HiEasyX::AutoExit(), HiEasyX::initgraphHX(__VA_ARGS__))
#define closegraph()			HiEasyX::closegraphHX()

// 默认使用双缓冲，故 BeginBatchDraw 无意义
#define BeginBatchDraw()
#define FlushBatchDraw()		HiEasyX::FlushWindowBuffer()
#define EndBatchDraw()			HiEasyX::FlushWindowBuffer()

#define GetHWnd					HiEasyX::GetHWndHX
#define GetImageBuffer			HiEasyX::GetImageBufferHX

#define getmessage				HiEasyX::getmessageHX
#define peekmessage				HiEasyX::peekmessageHX
#define flushmessage			HiEasyX::flushmessageHX

#define MouseHit				HiEasyX::MouseHitHX
#define GetMouseMsg				HiEasyX::GetMouseMsgHX
#define PeekMouseMsg			HiEasyX::PeekMouseMsgHX
#define FlushMouseMsgBuffer		HiEasyX::FlushMouseMsgBufferHX


// HiEasyX 命名空间缩写
namespace hiex = HiEasyX;
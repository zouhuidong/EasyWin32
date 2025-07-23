/**
 * @file		HiEasyX.h
 * @brief		HiEasyX ��ͷ�ļ�
 * @note		HiEasyX �ǻ��� EasyX ����չ�⣬֧�ִ����ര�ڡ�͸������ݻ�ͼ��ϵͳ UI ����ȵȡ�
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
 * @attention	���ʹ��MinGW��TDM�ȱ�����������Ҫ�������ѡ��:-leasyx -lmsimg32 -lgdiplus -lgdi32 -lwinmm
 * 				��Ҫʹ�� C++14 �����ϱ�׼������ʱ��ѡ�� ISO C++14 (�����ϱ�׼)��׼����ӱ���ѡ��:-std=c++14 (�����ϱ�׼)
*/

/********************************************************************
 * @attention <pre>
 *	ע�⣺
 *		���������ԭ�� EasyX ��д�Ĵ�����ֲ�� HiEasyX��
 *		����ʹ��ЩԴ���붼���� HiEasyX.h ͷ�ļ���
 *		�����ذ��� graphics.h��easyx.h ��ԭ�� EasyX ͷ�ļ���
 *		���򣬿��ܵ��� GetImageBuffer(nullptr) ����GetHWnd() ��������⡣
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
 //#include "HiEasyX/HiLayer.h"		// ��ʱ�Ƴ�
 //#include "HiEasyX/HiGUI/HiGUI.h"	// Ŀǰ������

#include "HiWindow.h"				// ����֧�� _NATIVE_EASYX_
#include "HiSysGUI/HiSysGUI.h"

// ��ʹ�� EasyX ԭ�������������ڣ���رմ���ʱ�Զ��˳�����
#define initgraph(...)			(HiEasyX::AutoExit(), HiEasyX::initgraphHX(__VA_ARGS__))
#define closegraph()			HiEasyX::closegraphHX()

// Ĭ��ʹ��˫���壬�� BeginBatchDraw ������
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


// HiEasyX �����ռ���д
namespace hiex = HiEasyX;
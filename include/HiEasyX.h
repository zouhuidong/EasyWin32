/**
 * @file		HiEasyX.h
 * @brief		HiEasyX (Traditional) ��ͷ�ļ�
 * @note		HiEasyX �ǻ��� EasyX ����չ�⣬֧�ִ����ര�ڡ�͸������ݻ�ͼ��ϵͳ UI ����ȵȡ�
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
 //#include <HiEasyX/HiLayer.h>		// ��ʱ�Ƴ�
 //#include <HiEasyX/HiGUI/HiGUI.h>	// Ŀǰ������

#include <HiEasyX/HiWindow.h>				// ����֧�� _NATIVE_EASYX_
#include <HiEasyX/HiSysGUI/HiSysGUI.h>

// HiEasyX �����ռ���д
namespace hiex = HiEasyX;


#ifndef HIEASYX_LIB_EXPORT
#ifdef _UNICODE
#pragma comment(lib, "HiEasyXw.lib")
#else
#pragma comment(lib, "HiEasyXa.lib")
#endif
#endif


///////////////////////////////////////
//  EasyX �����Ժ궨��
///////////////////////////////////////

// ������غ���
// ��ʹ�� EasyX ԭ�������������ڣ������ü���������
#define initgraph				HiEasyX::initgraphCompatible
#define closegraph				HiEasyX::closegraphHX
#define GetHWnd					HiEasyX::GetHWndHX
#define Sleep					HiEasyX::SleepHX

// ��ͼ��غ��� 
// ���¶�֧�ִ��� IMAGE* = nullptr �ĺ����������¶��壬�Խ���޷���λ����ǰ����ڵ�����

#define GetImageBuffer			HiEasyX::GetImageBufferHX
#define GetImageHDC				HiEasyX::GetImageHDCHX
#define SetWorkingImage			HiEasyX::SetWorkingImageHX
// �����ض��� GetWorkingImage ��������Ϊ�������� IMAGE* ����

// ֻ�Դ��ڻ�����Ч
#define setorigin				HiEasyX::setoriginHX
#define getorigin				HiEasyX::getoriginHX
// ֻ�Դ��ڻ�����Ч
#define setaspectratio			HiEasyX::setaspectratioHX
#define getaspectratio			HiEasyX::getaspectratioHX

// ˫������غ���
#define BeginBatchDraw			HiEasyX::BeginBatchDrawHX
#define FlushBatchDraw			HiEasyX::FlushBatchDrawHX
#define EndBatchDraw			HiEasyX::EndBatchDrawHX

// ��Ϣ��غ���
#define getmessage				HiEasyX::getmessageHX
#define peekmessage				HiEasyX::peekmessageHX
#define flushmessage			HiEasyX::flushmessageHX
#define MouseHit				HiEasyX::MouseHitHX
#define GetMouseMsg				HiEasyX::GetMouseMsgHX
#define PeekMouseMsg			HiEasyX::PeekMouseMsgHX
#define FlushMouseMsgBuffer		HiEasyX::FlushMouseMsgBufferHX

// ע�⣺������ʹ�� _getch��_kbhit ϵ�к������ᵼ�� HiEasyX ������Ϣѭ�������������ڿ���
// ���һ��Ҫ�����Ͼ���Ŀ�����Զ�������ĺ꣬�Ҳ�Ҫ�ڳ������ٰ��� conio.h��
//		#define _kbhit()	/* �����ڼ��ݣ����Ƽ�ʹ�� */ (HiEasyX::peekmessageHX(nullptr, EX_CHAR, false, nullptr))
//		#define _getch()	/* �����ڼ��ݣ����Ƽ�ʹ�� */ (HiEasyX::getmessageHX(EX_CHAR, nullptr).value.ch)
//		#define kbhit		/* �����ڼ��ݣ����Ƽ�ʹ�� */ _kbhit
//		#define getch		/* �����ڼ��ݣ����Ƽ�ʹ�� */ _getch
// ʹ����Щ��ή�ͳ���Ŀ�ά���ԣ�����Ŀ���ʱ�����ᵼ�ºܶ����⣬
// ���Խ���ֱ�ӽ������е� _getch��_kbhit ϵ�к��������ı��滻���Ա���ʹ����Щ�ꡣ

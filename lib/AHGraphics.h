////////////////////////////////////////////////////////////////////
//
//	AHGraphics.h (All HWnd graphics with EasyX)
//	������ƿ⣺��ʹ�� EasyX ���κδ����ϻ���
// 
//	�������ߣ�huidong <huidong_mail@163.com>
//	�桡������Ver 2.0
//	���뻷����VisualStudio 2022 | EasyX_20220116 | Windows 10 
//	�������ڣ�2020.07.08
//	����޸ģ�2022.01.28
//

#pragma once
#include <easyx.h>

// �õ���ǰ��ͼĿ�괰��
HWND GetWorkingHWnd();

// �õ���ǰ��ͼĿ��HDC
HDC GetWorkingHDC();

// �õ�������
IMAGE* GetMainImage();

// ��������easyx�е�������ʾ��Ŀ�괰����
// pImg Ҫ��ʾ�Ļ�ͼ����Ĭ��Ϊ������
// wnd Ŀ���ͼ���ڣ�Ĭ��Ϊ������
void FlushDrawingToWnd(IMAGE* pImg = NULL, HWND wnd = NULL);

// һֱ���»�ͼ���ݣ������Եģ�
//
// delay ÿ�θ��µ���ʱ��ms��
// flag ���������ı�־��Ϊfalseʱֹͣ
void KeepFlushDrawing(int delay, bool* flag);

// ʹ��IMAGE������С��ӦĿ���ͼ���ڴ�С
void MainImageFitWindow();

// ��ĳһ���ڻ�ͼǰ�ĳ�ʼ��
// wnd ���ھ��
void InitDrawing(HWND wnd);

// ��ĳһHDC��ͼǰ�ĳ�ʼ��
// hdc DC���
void InitDrawing(HDC hdc);

// ���� / �ر� �Զ������ͼ���ݣ������ڻ�ͼ���ֶ�FlushDrawingToWnd��
// ע�⣺���Ǳ�Ҫ������Ҫ�����Զ������ͼ����
//
// isAuto �رջ��ǿ����Զ������Ĭ��Ϊtrue
void AutoFlushDrawing(bool isAuto = true);
/*

������cmd�Ĵ����У��޷����ܴ���WM_PAINT��Ϣ��FlushDrawingToWnd�����б�Ҫ����AutoFlushDrawing�������ͼ�������ױ�����

*/

// ������ͼ
// ����һЩ������ͼ�ĺ�������
void EndDrawing();


//////////////////// ��������

// �õ��������ڴ��ڵ�λ��
POINT GetMousePoint();

// �õ�����λ�á�����
RECT GetWindowRect(HWND wnd);

// �õ����ڴ�С
POINT GetWindowSize(HWND wnd);

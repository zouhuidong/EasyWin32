/////////////////////////////////////////////////////////////////////
//
//	EasyWin32.h
//	���� EasyX ͼ�ο�� Win32 �ؼ�֧�ֿ�
//
//	�������ߣ�huidong <huidong_mail@163.com>
//	�桡������Ver 2.0
//	���뻷����VisualStudio 2022 | EasyX_20220116 | Windows 10 
//	�������ڣ�2020.12.06
//	����޸ģ�2022.02.06
//

#pragma once

#include <easyx.h>
#include <vector>
#include <string>


#define EASY_WIN32_BEGIN	namespace EasyWin32 {
#define EASY_WIN32_END		};

EASY_WIN32_BEGIN

////////////****** �ṹ�嶨�� ******////////////

// ����
struct EasyWindow
{
	HWND hWnd;			// ���ھ��
	IMAGE* pImg;		// ����ͼ��
	IMAGE* pBufferImg;	// ������
	bool(*funcWndProc)(HWND, UINT, WPARAM, LPARAM, HINSTANCE);	// ������Ϣ������
	std::vector<ExMessage> vecMouseMsg;	// �����Ϣ����
	int nGetMouseMsgIndex;	// ��ȡ�����Ϣ�Ľ������������ڻ�ȡ���������е���һ����
	bool isNewSize;			// ���ڴ�С�Ƿ�ı�

	bool isSentCreateMsg;	// �Ƿ����� WM_CREATE ����Ϣ
};


////////////****** ������غ��� ******////////////

// ����֧�� win32 �Ļ�ͼ���ڣ�Ĭ��֧�ִ���˫����Ϣ��
// w, h				���ڴ�С
// isCmd			�Ƿ���ʾ cmd ���ڣ�����ǿ���̨Ӧ�ó���
// strWndTitle		���ڱ���
// WindowProcess	������Ϣ��������ָ�룬Ϊ�ձ�ʾʹ��Ĭ����Ϣ�����������ͷ�ļ��С�������Ϣ�������淶����
// hParent			�����ھ����Ϊ�����ʾ�ô��ڶ������ڣ�����д���ô��ڽ���Ϊģ̬���ڴ��ڣ�
HWND initgraph_win32(
	int w = 640,
	int h = 480,
	bool isCmd = 0,
	LPCTSTR strWndTitle = L"",
	bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE) = NULL,
	HWND hParent = NULL
);

//
// ������Ϣ�������淶
// 
// ������׼��̬��bool WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance);
// 
// ����ֵ��
// true		��ʾʹ��ϵͳĬ�Ϸ����������Ϣ
// false	��ʾ������ҪϵͳĬ�Ϸ����������Ϣ
// 
// ע�����
// 1. ���� WM_CREATE ��Ϣʱ��wParam �� lParam �ǿյģ����޷���� CREATESTRUCT �ṹ����Ϣ
// 2. ������д��� WM_CLOSE ��Ϣ���������д�رմ��ڵĴ��룬����������뷵�� true�������������ڴ�й©
//

// �ر�ĳһ��ͼ���ڣ������Ϊ NULL ��ر����л�ͼ����
void closegraph_win32(HWND hWnd = NULL);

// ������ʼ�����ڣ�������
void init_end();

// �Ƿ񻹴���δ���ٵĻ�ͼ����
bool isAnyWindow();

// �ж�һ�����Ƿ����Ѵ����Ĵ����б��У������ѱ��رյĴ��ڣ�
bool isInListWindow(HWND hWnd);

// �ж�һ�����Ƿ񻹴���
// ͬ isInListWindow
inline bool isAliveWindow(HWND hWnd)
{
	return isInListWindow(hWnd);
}

// �õ���ǰ��ͼ���ڵľ��
HWND GetHWnd_win32();

// �õ���ǰ��ͼ���ڵ���ϸ��Ϣ
EasyWindow GetWorkingWindow();

// ���õ�ǰ��ͼ���ڣ�ͬʱ���û�ͼ����Ϊ���ڶ�Ӧ�� IMAGE ���󣩣������Ƿ����óɹ�
// ���л�ͼ��������У���ȴ���ǰ��ͼ�������
bool SetWorkingWindow(HWND hWnd);

// ǿ���ػ浱ǰ��ͼ���ڣ������� WM_PAINT ��Ϣ�ڻ�ͼ����Ҫʹ�ô˺�����
void EnforceRedraw();

// ���濪ʼһ�λ���
void ReadyToDraw();

// �����ͼ���壬����ʾ��ǰ��ͼ�����һ����
void FlushDrawing();

// ��ȡ�Ѵ����Ĵ��ڵ����飨�����ѱ��رյĴ��ڣ�
std::vector<EasyWindow> GetCreatedWindowList();

// ��ͼ���ڴ�С�Ƿ�ı�
bool isWindowSizeChanged();

////////////****** �����Ϣ��غ��� ******////////////

// ����Ƿ���������Ϣ
bool MouseHit_win32();

// �����ȴ���ֱ����ȡ��һ���µ������Ϣ
ExMessage GetMouseMsg_win32();

// ��ȡһ�������Ϣ������������
bool PeekMouseMsg_win32(ExMessage* pMsg);

// ��������Ϣ
void FlushMouseMsg_win32();

//
//	�����Ϣ��ر�ע��
//
//	��Ȼʹ�õ��� ExMessage �ṹ������Ŀǰֻ�ṩ��ȡ�����Ϣ�ķ�����
//	�ݲ��ṩ����������Ϣ�Ļ�ȡ���ܣ��� EM_KEY, EM_WINDOW �ȣ�
// 
//	����ֻ�ܻ�ȡ�����Ϣ���ʺ����������� MOUSEMSG ϵ�к��������Ǿɰ� EasyX �еĺ�����
//


////////////****** EasyX ԭ�������ĺ��滻 ******////////////

#define initgraph(w, h)			initgraph_win32(w, h)
#define closegraph				closegraph_win32

#define BeginBatchDraw
#define FlushBatchDraw
#define EndBatchDraw

// ��ȡ��ǰ��ͼ���ھ��
#define GetHWnd GetHWnd_win32

EASY_WIN32_END


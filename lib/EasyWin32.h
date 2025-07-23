/////////////////////////////////////////////////////////////////////
//
//	EasyWin32.h
//	���� EasyX ͼ�ο�� Win32 ��չ��
//
//	�������ߣ�huidong <mailhuid@163.com>
//	�桡������Ver 3.1.1
//	���뻷����VisualStudio 2022 | EasyX_20220610 | Windows 10 
//	��Ŀ��ַ��https://github.com/zouhuidong/EasyWin32
//	�������ڣ�2020.12.06
//	����޸ģ�2022.07.16
//

#pragma once

#define _EASYWIN32_

#include <graphics.h>
#include <stdarg.h>
#include <vector>
#include <string>
#include <thread>

// �����ͼ���ڳ�ʼ������
// ��ͨ����
#define EW_NORMAL 0

// ������Ϣ
#define WM_TRAY	(WM_USER + 100)


#define EASY_WIN32_BEGIN	namespace EasyWin32 {
#define EASY_WIN32_END		};

EASY_WIN32_BEGIN

////////////****** ���Ͷ��� ******////////////

// ����
struct EasyWindow
{
	bool isAlive;							// �����Ƿ����

	HWND hWnd;								// ���ھ��
	HWND hParent;							// �����ھ��

	IMAGE* pImg;							// ����ͼ��
	IMAGE* pBufferImg;						// ͼ�񻺳���

	bool(*funcWndProc)						// ������Ϣ������
		(HWND, UINT, WPARAM, LPARAM, HINSTANCE);

	std::vector<ExMessage> vecMessage;		// ģ�� EasyX ������Ϣ����

	bool isUseTray;							// �Ƿ�ʹ������
	NOTIFYICONDATA nid;						// ������Ϣ
	bool isUseTrayMenu;						// �Ƿ�ʹ�����̲˵�
	HMENU hTrayMenu;						// ���̲˵�
	void(*funcTrayMenuProc)(UINT);			// ���̲˵���Ϣ������
		// �����˺�����Ϊ�˷�����Ӧ���̵Ĳ˵���Ϣ
		// ������Ӧ������������Ϣ�����Զ��崰�ڹ��̺��������� WM_TRAY ��Ϣ

	bool isNewSize;							// ���ڴ�С�Ƿ�ı�
	bool isBusyProcessing;					// �Ƿ���æ�ڴ����ڲ���Ϣ��ָ�������û���������������

	int nSkipPixels;						// ����ʱ���������ص��������������ٻ棩
};

// ����϶���Ϣ
// ���÷�����
//	��Ҫ�������Ϣѭ����ÿ�ζ����� UpdateMessage ���������Ϣ
//	���� isLeftDrag��isMiddleDrag��isRightDrag �����ж������϶�����갴��
//	���� GetDragX��GetDragY ��ȡ����϶�ʱ�������ı仯��
class MouseDrag
{
private:
	ExMessage old, msg;
	int dx, dy;
	bool lbtn = false, mbtn = false, rbtn = false;
	bool newmsg = false;

	bool UpdateDragInfo(bool& btn, int msgid_down, int msgid_up)
	{
		if (newmsg)
		{
			if (btn)
			{
				dx = msg.x - old.x;
				dy = msg.y - old.y;
				old = msg;
				if (msg.message == msgid_up)	btn = false;
				if (dx != 0 || dy != 0)			return true;
				else							return false;
			}
			else
			{
				if (msg.message == msgid_down)
				{
					btn = true;
					old = msg;
				}
				return false;
			}
			newmsg = false;
		}
		else
		{
			return false;
		}
	}

public:

	void UpdateMessage(ExMessage m)
	{
		msg = m;
		newmsg = true;
	}

	bool isLeftDrag() { return UpdateDragInfo(lbtn, WM_LBUTTONDOWN, WM_LBUTTONUP); }
	bool isMiddleDrag() { return UpdateDragInfo(mbtn, WM_MBUTTONDOWN, WM_MBUTTONUP); }
	bool isRightDrag() { return UpdateDragInfo(rbtn, WM_RBUTTONDOWN, WM_RBUTTONUP); }

	int GetDragX() { return dx; }
	int GetDragY() { return dy; }
};

////////////****** ������غ��� ******////////////

// ����֧�� win32 �Ļ�ͼ���ڣ�Ĭ��֧�ִ���˫����Ϣ��
// w, h				���ڴ�С
// flag				������ʽ��ʶ
// strWndTitle		���ڱ���
// WindowProcess	������Ϣ��������ָ�룬Ϊ�ձ�ʾʹ��Ĭ����Ϣ�����������ͷ�ļ��С�������Ϣ�������淶����
// hParent			�����ھ����Ϊ�����ʾ�ô��ڶ������ڣ�����д���ô��ڽ���Ϊģ̬���ڣ�
HWND initgraph_win32(
	int w = 640,
	int h = 480,
	int flag = EW_NORMAL,
	LPCTSTR strWndTitle = L"",
	bool(*WindowProcess)(HWND, UINT, WPARAM, LPARAM, HINSTANCE) = NULL,
	HWND hParent = NULL
);

//
// ������Ϣ�������淶
// 
//	������׼��̬��
//		bool WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance);
// 
//	ע�⣺
//		����ڱ�׼�� Win32 ���ڹ��̺�����������һ�� HINSTANCE �����βΡ�
//		����ֵ������Ҳ����ͬ�������ġ�
// 
//	����ֵ��
//		true	��ʾʹ��ϵͳĬ�Ϸ����������Ϣ
//		false	��ʾ������ҪϵͳĬ�Ϸ����������Ϣ
// 
//	ע�����
//		1. ���� WM_CREATE ��Ϣʱ��wParam �� lParam �ǿյģ����޷���� CREATESTRUCT �ṹ����Ϣ
//		2. ���� WM_CLOSE ��Ϣʱ������ true �� false ��ʾ�Ƿ�رմ��ڣ�������رմ��ڣ��������д���ٴ��ڵĴ���
//

// �ر�ĳһ��ͼ���ڣ������Ϊ NULL ��ر����л�ͼ����
void closegraph_win32(HWND hWnd = NULL);

// ������ʼ�����ڣ�������
void init_end();

// ���ã������ڶ�������ʱ���Զ��˳�����
void AutoExit();

// �Ƿ񻹴���δ���ٵĻ�ͼ����
bool isAnyWindow();

// �ж�һ�����Ƿ񻹴��ڣ������ѱ��رյĴ��ڣ�
// ����վ�����Ա�ʶ��ǰ�����
bool isAliveWindow(HWND hWnd = NULL);

// �õ���ǰ��ͼ���ڵľ��
HWND GetHWnd_win32();

// �����ȴ���ͼ�������
// �������������ֻ�иþ�������ǻ����ʱ�ŵȴ�
void WaitForTask(HWND hWnd = NULL);

// �õ���ǰ��ͼ���ڵ���ϸ��Ϣ
EasyWindow GetWorkingWindow();

// �ȴ���ǰ������ɲ����û���ڣ������Ƿ����óɹ�
bool SetWorkingWindow(HWND hWnd);

// ���ü��ٻ��������������ص�
// �˼���Ч��������ģ�����Ч�������������ص�������ء�
void QuickDraw(UINT nSkipPixels);

// ǿ���ػ浱ǰ��ͼ���ڣ��� WM_PAINT ��Ϣ�ڻ�ͼ����Ҫʹ�ô˺�����
void EnforceRedraw();

// Ϊ��ǰ������������񣬷����Ƿ������ɹ���������������Ҳ���� true��
// ���� EasyX �������л�ͼ���ȡ��Ϣʱ����Ӧ�������������ٽ��е��á�
bool BeginTask();

// ��ֹ��ǰ���������ͼ����
void EndTask();

// �жϵ�ǰ�Ƿ��������ڽ���
// ����������������ж����Ƿ�Ϊ�����
bool isInTask(HWND hWnd = NULL);

// �ж�ĳ���ڵĴ�С�Ƿ�ı�
// ����վ�����Ա�ʶ��ǰ�����
bool isWindowSizeChanged(HWND hWnd = NULL);

// Ϊ��ǰ���ڴ���һ������
// ����������ʾ�ı�
// ע�⣺ÿ�����ڽ����ȶ�ռ��һ������
void CreateTray(LPCTSTR lpTrayName);

// ɾ��ĳ���ڵ����̣������ָ����Ա�ʶ��ǰ�����
void DeleteTray(HWND hWnd = NULL);

// �������̲˵�
void SetTrayMenu(HMENU hMenu);

// �������̲˵���Ϣ������
void SetTrayMenuProcFunc(void(*pFunc)(UINT));

// �ж��Զ������ͼ�������״̬
bool GetCustomIconState();

// ʹ��ͼ����Դ��Ϊ����ͼ��
// ��������ͼ����Դ ID����ͼ���Сͼ�꣩
// ע�������ڵ�һ�δ�������ǰ�͵��øú���������Ч��Ĭ������£������Ի� EasyX ����ͼ��
void SetCustomIcon(int nIcon, int nIconSm);

// ��ȡ EasyWin32 �Ի�Ĭ�ϴ���ͼ��� IMAGE
IMAGE GetDefaultIconImage();

// �ڴ�������ǰ���ô�����ʽ�����Դ˲������׸��´�����Ч
// ע�⣺�´��ڵ�������ʽ��������ǰ��ʽ����
void PreSetWindowStyle(long lStyle);

// �ڴ�������ǰ���ô���λ�ã����Դ˲������׸��´�����Ч
void PreSetWindowPos(int x, int y);

// ��ȡ��ǰ������ʽ
long GetWindowStyle();

// ���õ�ǰ������ʽ
int SetWindowStyle(long lNewStyle);

// ��ȡ��ǰ������չ��ʽ
long GetWindowExStyle();

// ���õ�ǰ������չ��ʽ
int SetWindowExStyle(long lNewExStyle);

////////////****** ��Ϣ��غ��� ******////////////

//// MOUSEMSG ʽ���������ݣ�

// ����Ƿ���������Ϣ
bool MouseHit_win32();

// �����ȴ���ֱ����ȡ��һ���µ������Ϣ
MOUSEMSG GetMouseMsg_win32();

// ��ȡһ���µ������Ϣ�����������Ƿ��ȡ�ɹ�
bool PeekMouseMsg_win32(MOUSEMSG* pMsg, bool bRemoveMsg = true);

// ��������Ϣ
void FlushMouseMsgBuffer_win32();

//// ExMessage ʽ����

// �����ȴ���ֱ����ȡ��һ������Ϣ
ExMessage getmessage_win32(BYTE filter = -1);

// �����ȴ���ֱ����ȡ��һ������Ϣ
void getmessage_win32(ExMessage* msg, BYTE filter = -1);

// ��ȡһ����Ϣ�����������Ƿ��ȡ�ɹ�
bool peekmessage_win32(ExMessage* msg, BYTE filter = -1, bool removemsg = true);

// ���������Ϣ��¼
void flushmessage_win32(BYTE filter = -1);

//// ת��

// MOUSEMSG ת ExMessage
ExMessage To_ExMessage(MOUSEMSG msg);

// ExMessage ת MOUSEMSG
MOUSEMSG To_MouseMsg(ExMessage msgEx);

//// ��Ϣ�����ж�

// ��ȡ ExMessage �� EM ��Ϣ����
UINT GetExMessageType(ExMessage msg);


////////////****** ͼ������غ��� ******////////////

// �õ� IMAGE ����� HBITMAP
HBITMAP Image2Bitmap(IMAGE* img);

// HBITMAP ת HICON
HICON Bitmap2Icon(HBITMAP hBmp);

// ����Ļָ��λ�������ʽ���ı�
void outtextxy_format(int x, int y, int _Size, LPCTSTR lpFormat, ...);


EASY_WIN32_END

////////////****** ����ָ��궨�� ******////////////

// ����һ�Σ���ͼ�����񣨻��Ƶ���ǰ��ͼ���ڣ�
#define BEGIN_TASK()\
	if (EasyWin32::isAliveWindow())\
	{\
		if (EasyWin32::BeginTask())\
		{(0)	/* �˴�ǿ��Ҫ��ӷֺ� */

// ����һ�Σ���ͼ������ָ��Ŀ���ͼ���ڣ�
#define BEGIN_TASK_WND(hWnd)\
	/* ���ù�������ʱ���Զ��ȴ����� */\
	if (EasyWin32::SetWorkingWindow(hWnd))\
	{\
		if (EasyWin32::BeginTask())\
		{(0)

// ����һ�Σ���ͼ�����񣬲������ͼ���棨���� BEGIN_TASK ���ã�
#define END_TASK()\
			EasyWin32::EndTask();\
		}\
	}(0)

// Ҫ�󴰿��ػ�
#define FLUSH_DRAW()			EasyWin32::EnforceRedraw()

////////////****** ������ʽ�궨�� ******////////////

// �Ƿ���õ�ǰ���ڸı��С
#define DisableResizing(state)	(state ? EasyWin32::SetWindowStyle(EasyWin32::GetWindowStyle() & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX) :\
								EasyWin32::SetWindowStyle(EasyWin32::GetWindowStyle() | WS_SIZEBOX | WS_MAXIMIZEBOX))

// �Ƿ���õ�ǰ���ڵ�ϵͳ�˵�
#define DisableSystemMenu(state)	(state ? EasyWin32::SetWindowStyle(EasyWin32::GetWindowStyle() & ~WS_SYSMENU) :\
									EasyWin32::SetWindowStyle(EasyWin32::GetWindowStyle() | WS_SYSMENU))

// ���� / �رյ�ǰ���ڵĹ�������ʽ
#define EnableToolWindowStyle(state)	(state ? EasyWin32::SetWindowExStyle(EasyWin32::GetWindowExStyle() | WS_EX_TOOLWINDOW) :\
										EasyWin32::SetWindowExStyle(EasyWin32::GetWindowExStyle() & ~WS_EX_TOOLWINDOW))

////////////****** ������Ϣ�궨�� ******////////////

// �жϵ�ǰ������Ƿ���ܵ�ĳ������Ϣ
#define KEY_DOWN(VK_NONAME) (GetForegroundWindow() == EasyWin32::GetHWnd_win32() && (GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

////////////****** EasyX ԭ�������ĺ��滻 ******////////////

// ��ʹ�� EasyX ԭ�������������ڣ���رմ���ʱ�Զ��˳�����
#define initgraph(...)			EasyWin32::initgraph_win32(__VA_ARGS__);\
								EasyWin32::AutoExit()

#define closegraph				EasyWin32::closegraph_win32

// Ĭ��ʹ��˫���壬�� BeginBatchDraw ������
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

////////////****** �����궨�� ******////////////

#define rectangle_RECT(rct)			rectangle(rct.left,rct.top,rct.right,rct.bottom);
#define fillrectangle_RECT(rct)		fillrectangle(rct.left,rct.top,rct.right,rct.bottom);
#define solidrectangle_RECT(rct)	solidrectangle(rct.left,rct.top,rct.right,rct.bottom);


////////////****** �������� ******////////////

// ��ȷ��ʱ����(���Ծ�ȷ�� 1ms������ ��1ms)
// by yangw80<yw80@qq.com>, 2011-5-4
void HpSleep(int ms);

// ����ɫ����չ
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

	// Windows �����
	CLASSICGRAY = RGB(0xF0, 0xF0, 0xF0)
};


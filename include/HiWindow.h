/**
 * @file	HiWindow.h
 * @brief	HiEasyX ��Ĵ���ģ��
 * @author	huidong
*/

#pragma once

#include "HiDef.h"
#include "HiString.h"
#include "HiMiscUtils.h"
#include "HiCanvas.h"
#include <windowsx.h>
#include <WinUser.h>
#include <vector>
#include <thread>

#ifdef _MSC_VER
#pragma comment (lib, "Msimg32.lib")
#endif

#define __HIWINDOW_H__

// �����ͼ���ڳ�ʼ������
// ��ͨ����
#define EW_NORMAL							0

// �޴���ʱ������
#define NO_WINDOW_INDEX						-1

// �����ã�ֱ��ʹ�� DefWindowProc ��������
//// ���ڹ��̺���Ĭ�Ϸ���ֵ
//#define HIWINDOW_DEFAULT_PROC				(LRESULT)(-10000)

// ������Ϣ
#define WM_TRAY								(WM_USER + 9337)

// ϵͳ�ؼ�������Ϣ
// wParam ���� SysControlBase*
// lParam ���� CREATESTRUCT*
#define WM_SYSCTRL_CREATE					(WM_USER + 9338)

// ϵͳ�ؼ�������Ϣ
// wParam ���� SysControlBase*
#define WM_SYSCTRL_DELETE					(WM_USER + 9339)

//// �û��ػ���Ϣ���������
//// �û����� RedrawWindow ���ػ洰��ʱ�ᷢ�ʹ���Ϣ���� WN_PAINT
//#define WM_USER_REDRAW						(WM_USER + 9340)

namespace HiEasyX
{
	class Canvas;
	class SysControlBase;

	////////////****** ���Ͷ��� ******////////////

	/**
	 * @brief	����
	 * @note	�� InitWindowStruct �����г�ʼ���˽ṹ��
	*/
	struct EasyWindow
	{
		bool isWindowAlive;							///< �����Ƿ����

		HWND hWnd;									///< ���ھ��
		HWND hParent;								///< �����ھ��

		/**
		 * @note <pre> 
		 *		��ҪΪ���ڱ���һ�� IMAGE �����ڴ�ֱ��ʹ�� Canvas�����ڴ洢�������ݣ�ԭ�����£�
		 *		1. ���û��� EasyX �߼���д���룬������ WM_PAINT ��Ϣ���򴰿���Ҫ�ػ�ʱ����Ӵ˴��� IMAGE �ж�ȡ
		 *		2. �������ϸ�ͼ����ֻ��ֱ�ӻ��Ƶ����� HDC��������Ҫǿ���޸� IMAGE ����Ļ���Ŀ�꣬���ǲ����ѣ����ǰ���
		 * </pre>
		*/
		Canvas* pCanvas;							///< ����ͼ��
		
		// �����ã�ֱ��ʹ�� pCanvas
		//IMAGE* pImg;								///< ����ͼ��
		//bool isNeedFlush;							///< �Ƿ���Ҫ�����ͼ����

		WNDPROC funcWndProc;						///< ������Ϣ������

		std::vector<ExMessage> vecMessage;			///< ģ�� EasyX ������Ϣ����

		bool isUseTray;								///< �Ƿ�ʹ������
		NOTIFYICONDATA nid;							///< ������Ϣ
		bool isUseTrayMenu;							///< �Ƿ�ʹ�����̲˵�
		HMENU hTrayMenu;							///< ���̲˵�

		/**
		 * @brief <pre>
		 *		���̲˵���Ϣ������ָ��
		 *
		 * ��ע��
		 *		�����˺�����Ϊ�˷�����Ӧ���̵Ĳ˵���Ϣ
		 *		������Ӧ������������Ϣ�����Զ��崰�ڹ��̺��������� WM_TRAY ��Ϣ
		 * </pre>
		*/
		void(*funcTrayMenuProc)(UINT);

		bool isNewSize;								///< ���ڴ�С�Ƿ�ı�
		//bool isBusyProcessing;						///< �Ƿ���æ�ڴ����ڲ���Ϣ��ָ�������û���������������

		std::vector<SysControlBase*> vecSysCtrl;	///< ��¼������ϵͳ�ؼ�
		bool bHasCtrl = false;						///< �Ƿ񴴽���ϵͳ�ؼ�
	};

	/**
	 * @brief ����
	*/
	class Window
	{
	private:

		int m_nWindowIndex = NO_WINDOW_INDEX;
		//bool m_isCreated = false;

		bool m_isPreStyle = false;
		bool m_isPreStyleEx = false;
		bool m_isPrePos = false;
		bool m_isPreShowState = false;

		long m_lPreStyle = 0;
		long m_lPreStyleEx = 0;
		POINT m_pPrePos = { 0, 0};
		int m_nPreCmdShow = 0;

	public:

		Window();

		Window(
			int w,
			int h,
			int flag = EW_NORMAL,
			LPCTSTR lpszWndTitle = HXStr(""),
			WNDPROC WindowProcess = nullptr,
			HWND hParent = nullptr
		);

		virtual ~Window();

		operator HWND() const {
			return GetHandle();
		}

		/**
		 * @brief ��������
		 * @return ���ش����Ĵ��ھ��������δ���ر�ʱ�ظ�����������������ԭ�򴴽�ʧ��ʱ���� nullptr��
		*/
		HWND Create(
			int w = 640,
			int h = 480,
			int flag = EW_NORMAL,
			LPCTSTR lpszWndTitle = HXStr(""),
			WNDPROC WindowProcess = nullptr,
			HWND hParent = nullptr
		);

		/**
		 * @brief closegraph
		*/
		void Close();

		void SetProcFunc(WNDPROC WindowProcess);

		/**
		 * @note <pre>
		 *		��ʹ�� GetHWnd Ϊ����������Ϊ GetHWnd ���궨��Ϊ GetHWndHX�����������Ҫ���鷳��
		 *		�� SysGUI ģ����Ҳʹ�� GetHandle ��һ�ƺ���
		 * </pre>
		*/
		HWND GetHandle() const;
		EasyWindow GetInfo() const;
		bool IsAlive();

		//IMAGE* GetImage();
		Canvas* GetCanvas() const;
		//void BindCanvas(Canvas* pCanvas);

		//void WaitMyTask();
		bool SetWorkingWindow();

		/**
		 * @brief ˢ�´��ڻ�ͼ����
		*/
		void FlushBuffer();

		// ������
		///**
		// * @brief <pre>
		// *		���´��ڵ�˫����
		// *
		// *	ע�⣺
		// *		�����ڴ��������ڵ��ô˺�������� hiex::FlushDrawing
		// * </pre>
		//*/
		//void FlushDrawing(RECT rct = { 0 });

		/*bool BeginTask();
		void EndTask(bool flush = true);
		bool IsInTask();*/

		bool IsSizeChanged();

		void CreateTray(LPCTSTR lpszTrayName);
		void DeleteTray();
		void SetTrayMenu(HMENU hMenu);
		void SetTrayMenuProcFunc(void(*pFunc)(UINT));

		void PreSetStyle(long lStyle);
		void PreSetStyleEx(long lStyleEx);
		void PreSetPos(int x, int y);
		void PreSetShowState(int nCmdShow);

		long GetStyle();
		int SetStyle(long lNewStyle);
		long GetExStyle();
		int	SetExStyle(long lNewExStyle);

		POINT GetPos();

		/**
		 * @brief ��ȡ�������ڵĴ�С
		*/
		SIZE GetWindowSize();

		/**
		 * @brief ��ȡ�������ڵĿ��
		*/
		int GetWindowWidth();

		/**
		 * @brief ��ȡ�������ڵĸ߶�
		*/
		int GetWindowHeight();

		/**
		 * @brief ��ȡ�ͻ������
		*/
		int GetClientWidth();

		/**
		 * @brief ��ȡ�ͻ����߶�
		*/
		int GetClientHeight();

		void Move(int x, int y);
		void MoveRel(int dx, int dy);

		void Resize(int w, int h);
		void SetTransparent(bool enable, int alpha = 0xFF);

		void SetTitle(LPCTSTR lpszTitle);

		/**
		 * @brief �жϴ˴����Ƿ�Ϊ�û�����ʹ�õĴ���
		*/
		bool IsForegroundWindow();

		Optional<ExMessage> GetMsg(BYTE filter = -1);
		bool GetMsg(ExMessage* msg, BYTE filter = -1);
		bool PeekMsg(ExMessage* msg, BYTE filter = -1, bool removemsg = true);
		void FlushMsg(BYTE filter = -1);
	};

	////////////****** ������غ��� ******////////////

	/**
	 * @brief <pre>
	 *		���� Win32 ��ͼ���ڣ�����ԭ�� EasyX ���ڣ�
	 *
	 *	��ע��
	 *		����Ĭ��֧��˫����Ϣ��������С��ʹ�� EnableResizing �������Ƿ���Ե�����С��
	 * </pre>
	 *
	 * @param[in] w					���ڿ�
	 * @param[in] h					���ڸ�
	 * @param[in] flag				������ʽ��EW_ ϵ�к꣬Ĭ��Ϊ EW_NORMAL��
	 * @param[in] lpszWndTitle		���ڱ���
	 * @param[in] WindowProcess		���ڹ��̺���
	 * @param[in] hParent			�����ھ��
	 * @return �����Ĵ��ھ��
	 *
	 * @bug
	 *		���ɰ棩�����������������ͼ���ڣ������Ҫ�����ʵ������ʱ��������ܵ���δ֪���⡣
	 *		Ver0.5.0 �Ժ󣬵��̴߳�������Ӧ�ò�����ִ�����
	 *
	 * @par ���ڹ��̺����淶 <pre>
	 *
	 *		����ǩ����
	 *			LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	 *
	 *		��ע��
	 *			Ver0.5.0 �Ժ���Ҫʹ�� HIWINDOW_DEFAULT_PROC ��� DefWindowProc()��
	 * 			ֱ�Ӱ��մ�ͳ�� Win32 WndProc д����д�˺������ɡ�
	 *
	 *		ʾ��������
	 * @code
				LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
				{
					switch (msg)
					{
						// ע�⣬����� WM_PAINT �б�д���룬��ֱ��ʹ�� EasyX ��ʽ��ͼ
						// ����Ҫʹ�� Win32 ��ʽ��BeginPaint(), EndPaint()��
					case WM_PAINT:
						SetWorkingWindow(hWnd);	// ���õ�ǰ��������Ϊ hWnd
						circle(100, 100, 70);
						break;

					case WM_CLOSE:
						if (MessageBox(hWnd, _T("ȷ��Ҫ�رմ�����"), _T("ȷ�Ϲر�"), MB_YESNO | MB_ICONQUESTION) == IDYES)
						{
							DestroyWindow(hWnd);  // ���ٴ��ڣ������յ� WM_DESTROY ��Ϣ
						}
						break;

					case WM_DESTROY:
						// TODO: �ڴ˴��ͷ�������ڴ�
						PostQuitMessage(0);		// ��ϵͳָʾ�߳���������ֹ���˳���
						break;

					default:						
						return DefWindowProc(hWnd, msg, wParam, lParam);

						// �����õķ�ʽ��Ver0.5.0 ��ǰ���ã���
						//return HIWINDOW_DEFAULT_PROC;	// ��ʶʹ��Ĭ����Ϣ��������������
					}

					return 0;
				}
	 * @endcode
	 * </pre>
	*/
	HWND initgraphHX(
		int w = 640,
		int h = 480,
		int flag = EW_NORMAL,
		LPCTSTR lpszWndTitle = HXStr(""),
		WNDPROC WindowProcess = nullptr,
		HWND hParent = nullptr
	);

	bool init_console();	/// ��ʼ�������д��ڣ����ֻά��һ�������д��ڣ�
	bool hide_console();	/// ���������д��ڣ�Win11 �²���ֻ����С����
	bool close_console();	/// �ر������д��ڣ�ֱ�����г���ʱ���Թر������У������� VS �е���ʱ�����޷��رգ�

	/**
	 * @brief �ر�ĳһ��ͼ����
	 * @param[in] hWnd ���ھ����Ϊ�ձ�ʶ��ǰ����ڣ�
	 * @attention <pre>
	 *		��ͼ���ڹرպ���Ҫ���� SetWorkingImage / SetWorkingWindow �ٻ��ƣ����򽫳���
	 *		HiEasyX �ṩ�ĺ��������Ҫ���� HWND���򶼿��Դ��� nullptr �Ա�ʾ��ǰ���ڣ�����Ϊ�˼��ݴ�ͳ EasyX ���룬
	 *		���ǵ����ĳ���ӵ�в�ֻһ������ʱ�����ÿ�ζ���ȷָ��Ҫ������ HWND���������Ա���ܶ��鷳��
	 *		���⣬����ֱ��ͨ�� Window ��� Canvas ������������Ĳ���������ȷҪ�����Ĵ��ڡ���������
	 * </pre>
	 * @note �ú����ڲ����� DestroyWindow() �����ٴ��ڣ��û����Զ����˴��ڹ��̺����������յ� WM_DESTROY ��Ϣ�������ڴ�ʱ�ͷų�����ڴ档
	*/
	void closegraphHX(HWND hWnd = nullptr);

	/**
	 * @brief �ر����л�ͼ����
	*/
	void closeallgraph();

	/**
	 * @brief ����ĳ���ڵĹ��̺���
	 * @param[in] hWnd ���ھ����Ϊ�ձ�ʶ��ǰ����ڣ�
	 * @param[in] WindowProcess �µĹ��̺���
	*/
	void SetWndProcFunc(HWND hWnd, WNDPROC WindowProcess);

	/**
	 * @brief HiEasyX �ڲ���Ϣѭ������
	 * @note <pre>
	 *		��Ҫ�ڳ�����ѭ���е��ô˺��������򴰿��޷���Ӧ��Ϣ
	 *		HiEasyX::getmessageHX(), HiEasyX::SleepHX(), HiEasyX::HpSleepHX() �ڲ��ȴ���Ϣʱ���Զ����ô˺�������ά�ִ�����Ϣ��Ӧ
	 * </pre>
	*/
	void MsgLoopHX();

	/**
	 * @brief ˯�ߺ�����˯���ڼ�ά�� HiEasyX ��Ϣѭ����
	 * @param[in] ms ����ʱ������λ�����룩
	 * @param[in] interval ��ѯ�������λ�����룬Ĭ��Ϊ 10 ���룬��ϵͳ���ƾ���ֻ�� 10~15 ms��
	 * @note �ú���ÿ�� interval ʱ����ѯһ����Ϣ���У�ֱ�� ms ʱ��������
	*/
	void SleepHX(int ms, int interval = 10);

	/**
	 * @brief ��ȷ˯�ߺ����������� SleepHX���ú�������ʱ�лῼ�ǳ�����ִ�еĺ�ʱ����˸���ȷ��
	 * @param[in] ms ����ʱ������λ�����룩
	 * @param[in] interval ��ѯ�������λ�����룬Ĭ��Ϊ 1 ���룬���ڽϸߵľ��ȣ�
	 * @note <pre>
	 *		�ú�������ʱ����Ȼ��ά�� HiEasyX ��Ϣѭ����������ɾ����½����粻ϣ��������Ϣѭ������ʹ�� HpSleep
	 * </pre>
	*/
	void HpSleepHX(int ms, int interval = 1);

	/**
	 * @brief �õ���ǰ���ͼ���ڵľ��
	*/
	HWND GetHWndHX();

	// ������
	///**
	// * @brief ��ʼ�����ڽ����󣬿����ô˺��������ȴ�Ŀ�괰�ڱ��رգ�Ȼ��������
	// * @param[in] hWnd Ŀ�괰�ڣ�Ϊ�մ������д��ڣ�
	//*/
	//void init_end(HWND hWnd = nullptr);

	/**
	 * @brief �����д��ڶ�������ʱ���Զ��˳�����
	 * @note �ڴ�����һ������֮ǰҲ���Ե��ô˺�����Ver0.5.0 ��ǰ���ܲ��У�
	*/
	void AutoExit();

	/**
	 * @brief �Ƿ񻹴���δ���ٵĻ�ͼ����
	*/
	bool IsAnyWindow();

	/**
	 * @brief �ж�һ�����Ƿ񻹴��ڣ�δ���رգ�
	 * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	 * @return �Ƿ����
	*/
	bool IsWindowExists(HWND hWnd = nullptr);

	// �����ã���ֱ��ʹ�� GetWindowCanvas
	///**
	// * @brief ��ȡĳ���ڵ�ͼ��ָ��
	// * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	// * @return ������ͼ��ָ��
	//*/
	//IMAGE* GetWindowImage(HWND hWnd = nullptr);

	/**
	 * @brief ��ȡ���ڻ���ָ��
	 * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	 * @return ����ָ�룬��δ�󶨻����򷵻ؿ�
	*/
	Canvas* GetWindowCanvas(HWND hWnd = nullptr);

	/**
	 * @brief ��ȡͼ�񻺳���ָ�루��� EasyX ԭ�� GetImageBuffer ������
	 * @param[in] img��ͼ��ָ�룩
	 * @return ͼ�񻺳���ָ��
	 * @attention <pre>
	 *		ʹ�� HiWindow ʱ����ԭ���� GetImageBuffer �������� nullptr �޷���Ӧ�õ���ǰ����ڵ�ͼ�񻺳�����
	 *		���ҿ��ܵ��³����������˽���������ʾָ��Ҫ��ȡ��ͼ��ָ�롣
	 * 
	 *		��ʵ�ϣ�������Ĵ����к���һЩ������ȫ���� HiEasyX �Ĵ��루���� EasyX ԭ�����룩��
	 *		����п��ܰ��� GetImageBuffer(nullptr) �����Ĵ��롣����Ҫ����Щ�ļ����� HiEasyX.h��
	 *		��Ϊ�� HiEasyX.h �У�GetImageBuffer ���궨��Ϊ HiEasyX::GetImageBufferHX���Դ˱�������Ĳ�����
	 * </pre>
	 * @throw ������� nullptr ����ǰû�л���ڣ����׳� runtime_error �쳣��
	*/
	DWORD* GetImageBufferHX(IMAGE* img);

	// ������
	///**
	// * @brief <pre>
	// *		�󶨴��ڻ���ָ��
	// *
	// *	��ע��
	// *		�󶨺�ʹ�û�����ͼʱ���Զ��������������û��������������Զ�ˢ����Ļ
	// * </pre>
	// *
	// * @param[in] pCanvas ����ָ��
	// * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	//*/
	//void BindWindowCanvas(Canvas* pCanvas, HWND hWnd = nullptr);

	/**
	 * @brief �õ���ǰ��ͼ���ڵ���ϸ��Ϣ
	*/
	EasyWindow GetWorkingWindow();

	/**
	 * @brief ���û���ڣ�����ھ�����ȡ��Ϣ�����Ƶ�Ŀ�괰�ڣ�
	 * @param[in] hWnd �µĻ���ھ��
	 * @return �Ƿ����óɹ�����ָ���Ĵ��ڲ����ڻ��ѱ����٣���ʧ�ܣ�
	 * @note �� hWnd Ϊ�գ����������� Working Image ����ǰ�Ļ����
	*/
	bool SetWorkingWindow(HWND hWnd);

	/**
	 * @brief ˢ��ָ����ͼ���ڻ�����
	 * @param[in] hWnd Ҫˢ�µĴ��ڣ�Ϊ�ձ�ʾ��ǰ����ڣ�
	 * @param[in] bInstant �Ƿ�����ˢ�£�����ȴ�������Ϣ���п���ʱˢ�£�
	 * @note <pre>
	 *		HiEasyX ����Ȼ˫����ģ��� HiEasyX �жԴ��ڵĻ��ƶ�ֻ�ǻ��浽�� Canvas �ϣ�
	 *		�����Ҫ�� Canvas ������ˢ�µ������ϣ�����ô˺�����
	 *		HiEasyX ���û����� WM_PAINT ��Ϣ֮��Ҳ���Զ�ˢ�´��ڻ��壬��������� WM_PAINT �б�д��ͼ���룬��������ô˺�����
	 * 
	 *		����������Ҫ����һ�ݴ��ڻ������ݵĸ����Ա��ڴ����ػ棨WM_PAINT��ʱʹ�ã�
	 *		����ֱ�ӻ��Ƶ����� HDC �϶��� HiEasyX ʵ�ֶര�ڲ�û��ʲô�ô���
	 *		��� HiEasyX Ҳ������д EasyX �ײ㡣
	 * </pre>
	 * @attention <pre>
	 *		����Ҫ�� WM_PAINT �л�ͼ���벻Ҫʹ�� Win32 ��ͳ�ķ�����BeginPaint(), EndPaint()��
	 *		ֱ���� HiEasyX �ķ�ʽ��ͼ���ɣ�Ҳ���ǻ��Ƶ����� Canvas �ϣ���
	 * </pre>
	*/
	void FlushWindowBuffer(HWND hWnd = nullptr, bool bInstant = false);

	/**
	 * @brief ˢ�����л�ͼ���ڻ�����
	 * @param[in] bInstant �Ƿ�����ˢ�£�����ȴ�������Ϣ���п���ʱˢ�£�
	*/
	void FlushAllWindowBuffer(bool bInstant = false);

	// ������
	///**
	// * @brief <pre>
	// *		���µ�ǰ����ڵ�˫����
	// *
	// *	ע�⣺
	// *		���ڰ�ȫ�����⣬�����ڴ��������ڵ��ô˺��������򲻻����˫���塣
	// *
	// *	��ע��
	// *		��Ҫ�ػ洰����ʹ�� RedrawWindow
	// *
	// *	ʾ����
	// * @code
	//		BEGIN_TASK();
	//		hiex::FlushDrawing({ 200,200,300,300 });
	//		END_TASK(false);	// ע�⣬��������ʱ��� false ��ʾ������˫���壬��Ϊ�����Ѿ����¹���
	//		REDRAW_WINDOW();
	// * @endcode
	// * </pre>
	// *
	// * @param[in] rct	˫��������������궼Ϊ 0 ��ʾȫ������
	//*/
	//void FlushDrawing(RECT rct = { 0 });

	// ������
	///**
	// * @brief <pre>
	// *		�Ƿ������Զ�ˢ��˫����
	// *
	// *	��ע��
	// *		Ĭ����������Զ�ˢ��˫����ģ���ÿ�ν�����������ʱ��EndTask ����ݴ���Ĳ�����
	// *		����Ҫ��Ҫ��ǡ���Ҫˢ��˫���塱����Ǻ󣬴��ڽ�������һ�������ػ���Ϣ��ʱ��ˢ��˫���塣
	// *
	// *		���ǣ���Ƶ���ػ������£����ڶ��߳�Э�����⣬�Զ�ˢ�µ�Ч�ʿ��ܻ��͡�
	// *		��������Թر��Զ�ˢ��˫���壬��Ӧ�أ�����Ҫʹ�� FlushDrawing �����ֶ�ˢ��˫���塣
	// * </pre>
	//*/
	//void EnableAutoFlush(bool enable);

	// ������
	///**
	// * @brief <pre>
	// *		Ϊ��ǰ�������������
	// *
	// *	��ע��
	// *		���� EasyX �������л�ͼ���ȡ��Ϣʱ����Ӧ����������
	// * </pre>
	// *
	// * @return �Ƿ������ɹ���������������Ҳ���� true��
	//*/
	//bool BeginTask();

	// ������
	///**
	// * @brief ��ֹ��ǰ��������
	// * @param[in] flush �Ƿ�����Ҫ����˫���壨�������Զ�ˢ�´��ڣ�
	//*/
	//void EndTask(bool flush = true);

	// ������
	///**
	// * @brief <pre>
	// *		�ж�ĳ�����Ƿ���������
	// *
	// *	��ע��
	// *		���������Ƕ���ʽ�ģ�ֻ�л���ڿ��ܴ��������С�
	// *		�������봰�ڲ��ǻ���ڣ���ֱ�ӷ��� false��
	// * </pre>
	// *
	// * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	// * @return �Ƿ���������
	//*/
	//bool IsInTask(HWND hWnd = nullptr);

	// ������
	///**
	// * @brief �����ȴ�ĳ�����������
	// * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	//*/
	//void WaitForTask(HWND hWnd = nullptr);

	/**
	 * @brief �ж�ĳ���ڵĴ�С�Ƿ�ı�
	 * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	 * @return ���ڵĴ�С�Ƿ�ı�
	*/
	bool IsWindowSizeChanged(HWND hWnd = nullptr);

	/**
	 * @brief <pre>
	 *		Ϊ���ڴ���һ������
	 *
	 *	ע�⣺
	 *		�� HiEasyX �У�ÿ�����ڽ����ȶ�ռ��һ������
	 * </pre>
	 *
	 * @param[in] lpszTrayName ������ʾ�ı�
	 * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	*/
	void CreateTray(LPCTSTR lpszTrayName, HWND hWnd = nullptr);

	/**
	 * @brief ɾ��ĳ���ڵ�����
	 * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	*/
	void DeleteTray(HWND hWnd = nullptr);

	/**
	 * @brief �������̲˵����������κ�ʱ�����ã�
	 * @param[in] hMenu	�˵�
	 * @param[in] hWnd	���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	*/
	void SetTrayMenu(HMENU hMenu, HWND hWnd = nullptr);

	/**
	 * @brief �������̲˵���Ϣ������
	 * @param[in] pFunc	��Ϣ������
	 * @param[in] hWnd	���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	*/
	void SetTrayMenuProcFunc(void(*pFunc)(UINT), HWND hWnd = nullptr);

	/**
	 * @brief ��ȡ�Զ������ͼ�������״̬
	*/
	bool GetCustomIconState();

	/**
	 * @brief <pre>
	 *		ʹ���Զ���ͼ����Դ��Ϊ����ͼ��
	 *
	 *	��ע��
	 *		�����ڵ�һ�δ�������ǰ�͵��øú���������Ч��
	 *		ʹ�� MAKEINTRESOURCE ����Խ���Դ ID תΪ�ַ�����
	 * </pre>
	 *
	 * @param[in] lpszIcon		��ͼ����Դ
	 * @param[in] lpszIconSm	Сͼ����Դ
	*/
	void SetCustomIcon(LPCTSTR lpszIcon, LPCTSTR lpszIconSm);

	/**
	 * @brief <pre>
	 *		�ڴ�������ǰ���ô�����ʽ�����Դ˲������׸��´�����Ч
	 *
	 *	ע�⣺
	 *		�´��ڵ�������ͨ��ʽ��������ǰ��ʽ����
	 * </pre>
	 *
	 * @param[in] lStyle ����ʽ
	*/
	void PreSetWindowStyle(long lStyle);

	/**
	 * @brief <pre>
	 *		�ڴ�������ǰ���ô�����չ��ʽ�����Դ˲������׸��´�����Ч
	 *
	 *	ע�⣺
	 *		�´��ڵ�������չ��ʽ��������ǰ��ʽ����
	 * </pre>
	 *
	 * @param[in] lStyleEx ����ʽ
	*/
	void PreSetWindowStyleEx(long lStyleEx);

	/**
	 * @brief �ڴ�������ǰ���ô���λ�ã����Դ˲������׸��´�����Ч
	 * @param[in] x	λ��
	 * @param[in] y	λ��
	*/
	void PreSetWindowPos(int x, int y);

	/**
	 * @brief �ڴ�������ǰ���ô�����ʾ״̬�����Դ˲������׸��´�����Ч
	 * @param[in] nCmdShow ��ʾ״̬���� ShowWindow �÷�һ�£�
	*/
	void PreSetWindowShowState(int nCmdShow);

	/**
	 * @brief ����ĳ������ʽ
	 * @param[in] lNewStyle ����ʽ
	 * @param[in] hWnd		���ھ����Ϊ�մ���ǰ����ڣ�
	 * @return ������һ�����õĴ�����ʽ��ʧ�ܷ��� 0
	*/
	int SetWindowStyle(long lNewStyle, HWND hWnd = nullptr);

	/**
	 * @brief ����ĳ������չ��ʽ
	 * @param[in] lNewExStyle	����ʽ
	 * @param[in] hWnd 			���ھ����Ϊ�մ���ǰ����ڣ�
	 * @return ������һ�����õĴ�����ʽ��ʧ�ܷ��� 0
	*/
	int SetWindowExStyle(long lNewExStyle, HWND hWnd = nullptr);

	/**
	 * @brief ��ȡ����λ��
	 * @param[in] hWnd ���ھ����Ϊ�մ���ǰ����ڣ�
	 * @return ����λ��
	*/
	POINT GetWindowPos(HWND hWnd = nullptr);

	/**
	 * @brief ��ȡ���ڴ�С
	 * @param[in] hWnd ���ھ����Ϊ�մ���ǰ����ڣ�
	 * @return ���ڴ�С
	*/
	SIZE GetWindowSize(HWND hWnd = nullptr);

	/**
	 * @brief �ƶ�����
	 * @param[in] x		λ��
	 * @param[in] y		λ��
	 * @param[in] hWnd	���ھ����Ϊ�մ���ǰ����ڣ�
	*/
	void MoveWindow(int x, int y, HWND hWnd = nullptr);

	/**
	 * @brief ����ƶ�����
	 * @param[in] dx	���λ��
	 * @param[in] dy	���λ��
	 * @param[in] hWnd	���ھ����Ϊ�մ���ǰ����ڣ�
	*/
	void MoveWindowRel(int dx, int dy, HWND hWnd = nullptr);

	/**
	 * @brief ���贰�ڴ�С
	 * @param[in] w		���ڿ�
	 * @param[in] h		���ڸ�
	 * @param[in] hWnd	���ھ����Ϊ�մ���ǰ����ڣ�
	*/
	void ResizeWindow(int w, int h, HWND hWnd = nullptr);

	/**
	 * @brief ���ô��ڱ����ı�
	 * @param[in] lpszTitle		�µĴ��ڱ���
	 * @param[in] hWnd			���ھ����Ϊ�մ���ǰ����ڣ�
	*/
	void SetWindowTitle(LPCTSTR lpszTitle, HWND hWnd = nullptr);

	/**
	 * @brief ���ô���͸����
	 * @param[in] HWnd ���ھ��
	 * @param[in] enable �Ƿ����ô���͸����
	 * @param[in] alpha ����͸����ֵ 0-255
	*/
	void SetWindowTransparent(HWND HWnd, bool enable, int alpha = 0xFF);

	////////////****** ��Ϣ��غ��� ******////////////

	//// ExMessage ʽ����

	/**
	 * @brief �����ȴ�����Ϣ
	 * @param[in] filter	��Ϣɸѡ��ʽ
	 * @param[in] hWnd		���ھ����Ϊ�մ���ǰ����ڣ�
	 * @return ��ȡ������Ϣ�����ȴ��д��ڱ��رգ��򷵻ص� Optional ֵ isSet Ϊ�գ�
	*/
	Optional<ExMessage> getmessageHX(BYTE filter = -1, HWND hWnd = nullptr);

	/**
	 * @brief �����ȴ�����Ϣ
	 * @param[out] msg	���ػ�ȡ������Ϣ
	 * @param[in] filter	��Ϣɸѡ��ʽ
	 * @param[in] hWnd		���ھ����Ϊ�մ���ǰ����ڣ�
	 * @return �Ƿ��ȡ����Ϣ�����ȴ��д��ڱ��رգ��򷵻� false��
	*/
	bool getmessageHX(ExMessage* msg, BYTE filter = -1, HWND hWnd = nullptr);

	/**
	 * @brief ��ȡһ����Ϣ�����������Ƿ��ȡ�ɹ�
	 * @param[out] msg	���ػ�ȡ������Ϣ
	 * @param[in] filter	��Ϣɸѡ��ʽ
	 * @param[in] removemsg	��ȡ��Ϣ���Ƿ����Ƴ�
	 * @param[in] hWnd		���ھ����Ϊ�մ���ǰ����ڣ�
	 * @return �Ƿ��ȡ����Ϣ
	*/
	bool peekmessageHX(ExMessage* msg, BYTE filter = -1, bool removemsg = true, HWND hWnd = nullptr);

	/**
	 * @brief ���������Ϣ��¼
	 * @param[in] filter	��Ϣɸѡ��ʽ
	 * @param[in] hWnd		���ھ����Ϊ�մ���ǰ����ڣ�
	*/
	void flushmessageHX(BYTE filter = -1, HWND hWnd = nullptr);

	//// MOUSEMSG ʽ���������ݣ�

	/**
	 * @brief ����Ƿ���������Ϣ
	 * @param[in] hWnd ���ھ����Ϊ�մ���ǰ����ڣ�
	 * @return �Ƿ���������Ϣ
	*/
	bool MouseHitHX(HWND hWnd = nullptr);

	/**
	 * @brief �����ȴ���ֱ����ȡ��һ���µ������Ϣ
	 * @param[in] hWnd ���ھ����Ϊ�մ���ǰ����ڣ�
	 * @return �����Ϣ�����ȴ��д��ڱ��رգ��򷵻ص� Optional ֵ isSet Ϊ�գ�
	*/
	Optional<MOUSEMSG> GetMouseMsgHX(HWND hWnd = nullptr);

	/**
	 * @brief ��ȡһ���µ������Ϣ�����������Ƿ��ȡ�ɹ�
	 * @param[out] pMsg		���ػ�ȡ������Ϣ
	 * @param[in] bRemoveMsg	��ȡ��Ϣ���Ƿ����Ƴ�
	 * @param[in] hWnd			���ھ����Ϊ�մ���ǰ����ڣ�
	 * @return �Ƿ��ȡ����Ϣ
	*/
	bool PeekMouseMsgHX(MOUSEMSG* pMsg, bool bRemoveMsg = true, HWND hWnd = nullptr);

	/**
	 * @brief ��������Ϣ
	 * @param[in] hWnd ���ھ����Ϊ�մ���ǰ����ڣ�
	*/
	void FlushMouseMsgBufferHX(HWND hWnd = nullptr);

	//// ת��

	/**
	 * @brief MOUSEMSG ת ExMessage
	 * @param[in] msg MOUSEMSG ��Ϣ
	 * @return ExMessage ��Ϣ
	*/
	ExMessage To_ExMessage(MOUSEMSG msg);

	/**
	 * @brief <pre>
	 *		ExMessage ת MOUSEMSG
	 *
	 *	��ע��
	 *		ExMessage ��Ϣ���������� EM_MOUSE���򷵻ؿ�
	 * </pre>
	 *
	 * @param[in] msgEx ExMessage ��Ϣ
	 * @return MOUSEMSG ��Ϣ
	*/
	MOUSEMSG To_MouseMsg(ExMessage msgEx);

	/**
	 * @brief ��ȡ ExMessage ����Ϣ����
	 * @param[in] msg ��Ϣ
	 * @return EM_ ��Ϣ�����е�һ�֣���ʧ�ܷ��� 0
	*/
	UINT GetExMessageType(ExMessage msg);
}

////////////****** ������ʽ�궨�� ******////////////

// ��ģ��꡿���� / ����ĳ����
// hwnd			Ŀ�괰��
// state		���û��ǽ���
// isExStyle	�Ƿ�Ϊ Ex ����
// styleCode	���Դ���
#define EnableSomeStyle(hwnd, state, isExStyle, styleCode)\
			(isExStyle ?\
				(state ?\
					HiEasyX::SetWindowExStyle(\
						(long)GetWindowExStyle(hwnd ? hwnd : HiEasyX::GetHWndHX()) | (styleCode),\
						hwnd\
					) :\
					HiEasyX::SetWindowExStyle(\
						(long)GetWindowExStyle(hwnd ? hwnd : HiEasyX::GetHWndHX()) & ~(styleCode),\
						hwnd\
					)\
				) :\
				(state ?\
					HiEasyX::SetWindowStyle(\
						(long)GetWindowStyle(hwnd ? hwnd : HiEasyX::GetHWndHX()) | (styleCode),\
						hwnd\
					) :\
					HiEasyX::SetWindowStyle(\
						(long)GetWindowStyle(hwnd ? hwnd : HiEasyX::GetHWndHX()) & ~(styleCode),\
						hwnd\
					)\
				)\
			)

// �Ƿ�����ĳ���ڸı��С
#define EnableResizing(hwnd, state)				EnableSomeStyle(hwnd, state, false, WS_SIZEBOX | WS_MAXIMIZEBOX)

// �Ƿ�����ĳ���ڵ�ϵͳ��������ť
#define EnableSystemMenu(hwnd, state)			EnableSomeStyle(hwnd, state, false, WS_SYSMENU)

// �Ƿ����õ�ǰ���ڵĹ�������ʽ
#define EnableToolWindowStyle(hwnd, state)		EnableSomeStyle(hwnd, state, true, WS_EX_TOOLWINDOW)

////////////****** ������Ϣ�궨�� ******////////////

// �ж�ϵͳȫ�ְ���״̬
#define KEY_DOWN_GOLBAL(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

// �ж�ָ�������Ƿ���ܵ�ĳ������Ϣ
// ���ھ��Ϊ�մ��� HiEasyX �Ļ����
#define KEY_DOWN_WND(hWnd, VK_NONAME) (GetForegroundWindow() == (hWnd ? hWnd : HiEasyX::GetHWndHX()) && KEY_DOWN_GOLBAL(VK_NONAME))

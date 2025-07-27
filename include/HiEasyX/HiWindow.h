/**
 * @file	HiWindow.h
 * @brief	HiEasyX ��Ĵ���ģ��
 * @author	huidong
*/

/**
 * ˵����
 *		���� HWND ���� nulllptr �ģ�����ǰ����ڣ�
 *		���� IMAGE* ���� nullptr �ģ�����ǰ����ڵ�ͼ��ָ�루�����ǵ�ǰ�ͼ�񣩡�
 *
 *		����������Ϊ�˼��� EasyX ԭ�����趨��
 *		Ϊ�˱��ⲻ��Ҫ���鷳������������ȷָ��Ҫ�����Ĵ��ڻ�ͼ��
 */


#pragma once

#include <HiEasyX/HiDef.h>
#include <HiEasyX/HiString.h>
#include <HiEasyX/HiMiscUtils.h>
#include <HiEasyX/HiCanvas.h>
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
	 * @brief HiEasyX Ԥ�贰����ʽ��ͨ�� SetWindowStyleHX ���ã�
	 * @sa SetWindowStyleHX, HasWindowStyleHX
	 * @note <pre> 
	 *		�˴�Ԥ��Ĵ�����ʽ���ǿ���ͨ�� Win32 API SetWindowLong ʵ�ֵģ�
	 *		ͨ�����ṩһ����Ϊ��ݵ� Win32 ���� Style ����;����
	 * </pre>
	 * 
	 * @note WindowStyle ��Ҫ���ó��� 64 ��ö����
	 * @sa EasyWindow::bStylesUsage
	 */
	enum WindowStyle
	{
		Resizable,	
		Maximizable,
		SystemMenu,
		ToolWindow,

		/**
		 * @note <pre>
		 *		HiEasyX ���Թ������ṩһ������ʵ�֣����ṩ���ӽӿڣ������ù�����λ�õȡ�
		 * 
		 *		����������һ����Ϊ��Ϊ��ʹ���ڴ��ڵĻ��������ڴ�������ʾ�����ҿ������û����ɹ����鿴��
		 *		��ˣ�������������ͬʱ��������� AutoResizeWindowCanvas(false); �Է����ڻ������洰������仯��
		 * 
		 *		���⣬���й�����������£�Ӧ����ÿ���ֶ��������ڻ�����С֮����� UpdateScrollInfo��
		 *		������������ܼ�ʱ��Ӧ��������С�ĸı䡣�ú����ڹ���������ʱ����������ʱ�ᱻ�Զ����á�
		 * 
		 *		���ϣ�����ڲ�Ҫ���û����쵽���ڻ����ߴ磬����ܻ���Ҫ���� SetWindowSizeLim �����ô��ڴ�С���ơ�
		 * 
		 *		��ע���������Ĺ�����������������������������ϵ� SysGUI ϵͳ�ؼ�����Ϊ�ǲ��ֵ�ʵ����Ҫ���Ǻܶ����⣬
		 *		���ô���������ֻ��Ϊ���ṩһ�ֹ������ڵļ�㷽������Ը�漰̫���ӵĶ�����
		 * </pre>
		 */
		VScroll,
		HScroll
	};

	/**
	 * @brief HiEasyX ����
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
		POINT m_pPrePos = { 0, 0 };
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
		 *		�˴���ʹ�� GetHWnd Ϊ����������Ϊ GetHWnd ���궨��Ϊ GetHWndHX�����������Ҫ���鷳��
		 *		���� GetHandle ��Ϊ������������Ҳ�� SysGUI ģ��ͳһ��
		 * </pre>
		*/
		HWND GetHandle() const;
		bool Exist();

		Canvas* GetCanvas() const;

		///< ���ô��ڻ���ԭ��
		void SetOrigin(int x, int y) const;
		void GetOrigin(int* x, int* y) const;

		///< ���ô��ڻ������ű���
		void SetAspectRatio(float xasp, float yasp) const;
		void GetAspectRatio(float* xasp, float* yasp) const;

		//void WaitMyTask();
		bool SetWorkingWindow();

		/**
		 * @brief ˢ�´��ڻ�ͼ����
		*/
		void FlushBuffer();

		bool IsSizeChanged();
		void SetSizeLim(Optional<SIZE> sizeMin, Optional<SIZE> sizeMax);
		void AutoResizeCanvas(bool enable);

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

		void SetStyleHX(WindowStyle style, bool bEnable);
		bool HasStyleHX(WindowStyle style);

		void UpdateScrollInfo();

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

	/**
	 * @brief ���� EasyX ��ͼ���ڣ�������һϵ�м�����ѡ���ģ��ԭʼ�� EasyX ���ã�
	*/
	HWND initgraphCompatible(
		int w,
		int h,
		int flag = EW_NORMAL
	);

	bool init_console();	/// ��ʼ�������д��ڣ����ֻά��һ�������д��ڣ�
	bool hide_console();	/// ���������д���
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
	 * @brief �����д��ڶ�������ʱ���Զ��˳�����Ĭ�ϲ�������
	 * @note <pre>
	 *		�ڴ�����һ������֮ǰҲ���Ե��ô˺�����Ver0.5.0 ��ǰ���ܲ��У�
	 *
	 *		ԭ��
	 *		���� WM_DESTROY ���жϵ�ǰ�������ٺ��Ƿ��д��ڴ��ڣ����û���� PostQuitMessage()��
	 *		Ȼ�� MsgLoopHX() ���յ� WM_QUIT ��Ϣ��ִ�� exit() �˳�����
	 *		�������ô˺����������д��ڱ����ٺ󣬳���Ҳ����ֱ���˳�������������£��û���Ҫ���� IsAnyWindow() �����Ƿ��˳�����
	 *
	 *		�ú�����Ҫ���ڼ���ԭ�� EasyX ���롣
	 * </pre>
	*/
	void AutoExit(bool enable);

	/**
	 * @brief ֻ����һ����ͼ���ڴ��ڣ��ظ�������ͼ����ʱ�ر���ǰ�Ĵ��ڣ�Ĭ�ϲ�������
	 * @note �ú�����Ҫ���ڼ���ԭ�� EasyX ����
	*/
	void SingleGraphWindow(bool enable);

	/**
	 * @brief ����ĳ���ڵĹ��̺���
	 * @param[in] hWnd ���ھ����Ϊ�ձ�ʶ��ǰ����ڣ�
	 * @param[in] WindowProcess �µĹ��̺���
	*/
	void SetWndProcFunc(HWND hWnd, WNDPROC WindowProcess);

	/**
	 * @brief HiEasyX �ڲ���Ϣѭ����������������
	 * @note <pre>
	 *		��Ҫ�ڳ�����ѭ���е��ô˺��������򴰿��޷���Ӧ��Ϣ
	 *
	 *		���Զ����� MsgLoopHX() �ĺ�����
	 *		* HiEasyX::getmessageHX(), HiEasyX::peekmessageHX() ��������Ϣ��غ�����
	 *		* HiEasyX::SleepHX(), HiEasyX::HpSleepHX()
	 *
	 *		�����������Զ�������Ϣѭ������������ʱ����Ҫ��ȡ��Ϣʱ������Ϣѭ�����У���ά�ֳ�������������
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

	///**
	// * @brief �õ���ǰ��ͼ���ڵ���ϸ��Ϣ
	//*/
	//EasyWindow GetWorkingWindow();

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

	/**
	 * @brief ��ȡ���ڻ���ָ��
	 * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	 * @return ����ָ�룬��δ�󶨻����򷵻ؿ�
	*/
	Canvas* GetWindowCanvas(HWND hWnd = nullptr);

	/**
	 * @brief ��ȡͼ�񻺳���ָ�루��� EasyX ԭ�� GetImageBuffer ������
	 * @param[in] img��ͼ��ָ�룬nullptr ��ʾ��ǰ�����ͼ��ָ�룩
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
	DWORD* GetImageBufferHX(IMAGE* img = nullptr);

	/**
	 * @brief ��ȡͼ�� HDC����� EasyX ԭ�� GetImageHDC ������������ ReleaseDC
	 * @param[in] img��ͼ��ָ�룬nullptr ��ʾ��ǰ�����ͼ��ָ�룩
	 * @return ͼ�񻺳���ָ��
	 * @attention ����ʹ�øú������ EasyX ԭ�� GetImageHDC ��������ԭ��μ� GetImageBufferHX ������˵����
	 * @throw ������� nullptr ����ǰû�л���ڣ����׳� runtime_error �쳣��
	*/
	HDC GetImageHDCHX(IMAGE* img = nullptr);

	/**
	 * @brief ���û IMAGE ������� EasyX ԭ�� SetWorkingImage ������
	 * @param[in] img��ͼ��ָ�룬nullptr ��ʾ��ǰ�����ͼ��ָ�룩
	 * @attention ����ʹ�øú������ EasyX ԭ�� SetWorkingImage ��������ԭ���� GetImageBufferHX �������ơ�
	 * @throw ��ʹ����ʧ�ܣ�Ҳ�����׳��쳣
	*/
	void SetWorkingImageHX(IMAGE* img = nullptr);

	/**
	 * @brief ��ȡ� IMAGE ������� EasyX ԭ�� GetWorkingImage ������
	*/
	IMAGE* GetWorkingImageHX();

	/**
	 * @brief ���ô��ڻ����Ļ���ԭ��
	 * @param[in] x ԭ�� x ����
	 * @param[in] x ԭ�� y ����
	 * @param[in] hwnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	 * @attention �ú���ֻ�Դ��ڻ�����Ч
	 * @note <pre>
	 *		�˺���ԭ��Ϊ���� GDI ���� SetViewportOrgEx() ����ԭ�㣬��� GDI �������ݶ��������õ�ԭ������Ϊ��׼��
	 *		�������������Ժ�IMAGE ����ʵ���Ͻ�����㲻���� (0, 0) ���� (-x_org, -y_org)�������Ҫ��ȷ�����ͼ��
	 *		��Ҫ�����ʱ�ر�ָ��ԭͼ������ԭ�㡣
	 *		Ȼ����EasyX �� putimage �Ȳ���֧�ִ����ã�������Ϊ�������ظ��ƣ�������޷����ʵ��ԭ������Ч����
	 *		��ˣ��� setaspectratioHX ����һ�����˺���Ҳ���Դ��ڻ�����Ч��
	 * </pre>
	*/
	void setoriginHX(int x, int y, HWND hwnd = nullptr);

	/**
	 * @brief ��ȡ���ڻ�����ԭ������
	*/
	void getoriginHX(int* px, int* py, HWND hwnd = nullptr);

	/**
	 * @brief ���ô��ڻ��������ű���
	 * @param[in] xasp x �����ű���������Ϊ��ֵ��
	 * @param[in] yasp y �����ű���������Ϊ��ֵ��
	 * @param[in] hwnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	 * @attention �ú���ֻ�Դ��ڻ�����Ч
	 * @note <pre>
	 *		�ú���ԭ���ǽ����ڻ�����С����Ϊ w/xasp * h/yasp��������ʱ���겢�������仯��
	 *		���ڴ���ˢ�»���ʱ�����ڻ�������Ϊ w * h ˢ�µ���Ļ�ϣ����ʵ������Ч����
	 *
	 *		�ɴ˿ɼ����ú��������� setorigin ������һ�� IMAGE �����ã���Ϊһ��� putimage ������ֱ�Ӱ����ص㸴�ƣ�
	 *		���������������һ�������š����ڻ����Ļ������ڿ����� WM_PAINT ʱ���ô����ŵķ�ʽ���������ʵ�ִ˹��ܡ�
	 * </pre>
	*/
	void setaspectratioHX(float xasp, float yasp, HWND hwnd = nullptr);

	/**
	 * @brief ��ȡ���ڻ��������ű���
	*/
	void getaspectratioHX(float* pxasp, float* pyasp, HWND hwnd = nullptr);

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

	/**
	 * @brief �Զ�ˢ�»�ͼ���ڻ����������� MsgLoopHX() ���Զ�ִ�� FlushAllWindowBuffer()����Ĭ�ϲ�������
	 * @note <pre>
	 *		�ú�����Ҫ���ڼ���ԭ�� EasyX ����
	 *
	 *		���� HiEasyX ����ƣ��û�Ӧ�ڳ�����ѭ���е��� FlushWindowBuffer() �����������ͼ���壬
	 *		������ MsgLoopHX() ����ά�ִ�����Ϣ����
	 *
	 *		Ϊ�˼���ԭ�� EasyX ���룬�Լ�Ӧ�� getmessage ��������������ڶ�������£�
	 *		HiEasyX �ڲ����Զ����� MsgLoopHX() ��ά����Ϣѭ������� MsgLoopHX ����ע�ͣ���
	 *		���ԣ���ʹ�û�����ʽ���� MsgLoopHX()��һ��ֻҪ�����е��� getmessage, Sleep �Ⱥ��������궨�����������Ҳ�����������С�
	 *		���ǣ�HiEasyX һ�㲢�����ڲ����� FlushWindowBuffer()������û������ڻ�����ɺ���� FlushWindowBuffer()��
	 *		�����Ǻܿ��ܿ��������ƽ����
	 *
	 *		HiEasyX Ϊ�˼���ԭ�� EasyX ���룬ͬʱ���������������������
	 *		1. ԭ����˫���庯�� FLushBatchDraw �� EndBatchDraw ���궨��Ϊˢ�´��ڻ��岢������Ϣѭ����
	 *		2. �ṩ AutoFlushWindowBuffer() ���������� MsgLoopHX() ���Զ����� FlushAllWindowBuffer()��
	 *
	 *		���ڱ���ʹ����˫����� EasyX ���룬������ȫ����Ҫ�޸ľͿ����� HiEasyX �����С�
	 *		����ԭ��û��ʹ��˫����� EasyX ���룬����Ҫ�ڹؼ�λ����� FlushWindowBuffer()�����鷳����
	 *		����ֱ�ӿ��� AutoFlushWindowBuffer()�������Ϳ��Կ�����ͼЧ����
	 *
	 *		����һ�ּ�����������ԭ EasyX ������û�е����κο����Զ����� MsgLoopHX() �ĺ������
	 *		���� getmessage, Sleep, FlushBatchDraw���ȣ���ô�����ܻ��� HiEasyX ������ʱ������
	 *		��ʱ�����Դ����б�Ҫ���޸ġ���������������Ǻ��ټ��ġ�
	 * </pre>
	*/
	void AutoFlushWindowBuffer(bool enable);

	void BeginBatchDrawHX();	///< �����ڼ��� EasyX �� BeginBatchDraw ����
	void FlushBatchDrawHX();	///< �����ڼ��� EasyX �� FlushBatchDraw ����
	void EndBatchDrawHX();		///< �����ڼ��� EasyX �� EndBatchDraw ����

	/**
	 * @brief �ж�ĳ���ڵĴ�С�Ƿ�ı�
	 * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	 * @return ���ڵĴ�С�Ƿ�ı�
	*/
	bool IsWindowSizeChanged(HWND hWnd = nullptr);

	/**
	 * @brief ���ô��ڵ���������
	 * @param sizeMin ��С�ߴ磨����Ϊ {} �༴ Optional<SIZE>{} ��ȡ����С�ߴ�߽磩
	 * @param sizeMax ���ߴ磨ͬ�ϣ�������Ϊ {} ȡ�����ߴ�߽磩
	 * @param hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	 */
	void SetWindowSizeLim(Optional<SIZE> sizeMin, Optional<SIZE> sizeMax, HWND hWnd = nullptr);

	/**
	 * @brief �����Ƿ��ڴ�������ʱ�Զ��������ڻ�����������С��Ĭ�Ͽ�����
	 * @param[in] enable �Ƿ���
	 * @param[in] hWnd ���ھ����Ϊ�ձ�ʾ��ǰ����ڣ�
	 */
	void AutoResizeWindowCanvas(bool enable, HWND hWnd = nullptr);

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
	 * @brief ���ô�����ʽ��WindowStyle ��Ԥ�����ʽ��
	 * @param style ������ʽ
	 * @param bEnable �Ƿ�����
	 * @param hWnd ���ھ����Ϊ�մ���ǰ����ڣ�
	 * @sa HasWindowStyleHX
	 */
	void SetWindowStyleHX(WindowStyle style, bool bEnable, HWND hWnd = nullptr);

	/**
	 * @brief �򵥵��ж�һ�������Ƿ�����ĳ��ʽ��WindosStyle ��Ԥ�����ʽ�������ܲ��ܷ�ӳ��ʵ�����
	 * @param style ������ʽ
	 * @param hWnd ���ھ����Ϊ�մ���ǰ����ڣ�
	 * @return ָ������ʽ�Ƿ�����
	 * 
	 * @attention �ú��������ڼ��ж�ĳ��ʽ�Ŀ������
	 * @note <pre>
	 *		WindowStyle �п�����Щѡ������ص��Ĳ������� HasWindowStyleHX ��Ȼ����Щ��ʽ����Ϊ�����ģ�
	 *		���磬������ Resizable ֮����ʵ�� Maximizable Ҳ�Ѿ���������ֻҪ�û�û���ֶ����� Maximizable ���ã�
	 *		HasWindowStyleHX ����Ϊ���ǹرյġ�
	 * 
	 *		��ʵ�ϣ��ú���Ŀǰ�� HiEasyX �ڲ���Ҫֻ�������ж� VScroll �� HScroll �Ŀ��������
	 * </pre>
	 */
	bool HasWindowStyleHX(WindowStyle style, HWND hWnd = nullptr);

	/**
	 * @brief ���¹������ķ�Χ��ҳ�ߴ�����ԣ�����ֶ������˴��ڻ�����С��������ô˺����Լ�ʱ���¹��������ԣ�
	 * @param hWnd �������������ھ��
	*/
	void UpdateScrollInfo(HWND hWnd = nullptr);

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

	/**
	 * ע��
	 *		��Ϣ��غ����ڵ���ʱ�������ڲ����� MsgLoopHX()���Ա���Ի�ȡ�����µ���Ϣ��
	*/

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


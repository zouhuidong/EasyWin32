#include <HiEasyX/HiWindow.h>

#include <HiEasyX/HiMacro.h>
#include <HiEasyX/HiIcon.h>
#include <HiEasyX/HiGdiplus.h>
#include <HiEasyX/HiSysGUI/SysControlBase.h>

#include <cassert>
#include <stdexcept>

#include <chrono>
using StdClock = std::chrono::high_resolution_clock;

// Ԥ����Ϣ�ռ�
#define MSG_RESERVE_SIZE		100

// Ԥ���ؼ��ռ�
#define SYSCTRL_RESERVE_SIZE	100


namespace HiEasyX
{
	////////////****** ���Ͷ��� ******////////////

	/**
	 * @brief WindowStyle ��Ӧ����ϸ��Ϣ�ṹ��
	 * @sa WindowStyle, GetWindowStyleHX_Internal
	 */
	struct WindowStyleInfo
	{
		WindowStyle style_name;
		bool is_ex;
		long style_code;
	};

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
		float xasp;									///< ˢ�´��ڻ���ʱʹ�õ����ű�����x �ᣩ
		float yasp;									///< ˢ�´��ڻ���ʱʹ�õ����ű�����y �ᣩ
		bool bAutoResizeCanvas;						///< �Ƿ��ڴ�������ʱ�Զ��������ڻ�����������С��Ĭ���ǣ�

		bool bStylesUsage[64];						///< ��־ WindowStyle Ԥ����ʽ���������

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
		Optional<SIZE> sizeMin;						///< ��С���ڳߴ�
		Optional<SIZE> sizeMax;						///< ��󴰿ڳߴ�

		std::vector<SysControlBase*> vecSysCtrl;	///< ��¼������ϵͳ�ؼ�
		bool bHasCtrl = false;						///< �Ƿ񴴽���ϵͳ�ؼ�
	};


	////////////****** ȫ�ֱ��� ******////////////

	static WNDCLASSEX				g_WndClassEx;								///< ������
	static TCHAR					g_lpszClassName[] = _T("HiEasyX");			///< ��������
	static ScreenSize				g_screenSize;								///< ��ʾ����Ϣ
	static HINSTANCE				g_hInstance = GetModuleHandle(0);			///< ����ʵ��

	static std::vector<EasyWindow>	g_vecWindows;								///< �����б�����ര�ڣ�������һ����򴰿ں��٣������� map��
	static int						g_nFocusWindowIndex = NO_WINDOW_INDEX;		///< ��ǰ�������㴰������
	static bool						g_bSingleWindow = false;					///< ����Ƿ������д��ڱ�����ʱ�Զ��˳�����
	static bool						g_bAutoExit = false;						///< ����Ƿ������д��ڱ�����ʱ�Զ��˳�����
	static bool						g_bAutoFlush = false;						///< ����Ƿ��Զ�ˢ�����д��ڻ���
	//static bool					g_isInTask = false;							///< ��Ǵ���������

	static HICON					g_hIconDefault;								///< Ĭ�ϳ���ͼ��
	static LPCTSTR					g_lpszCustomIcon = nullptr;					///< �Զ������ͼ����Դ��Ϊ�ձ�ʾ��ʹ��
	static LPCTSTR					g_lpszCustomIconSm = nullptr;
	static HICON					g_hCustomIcon;								///< �Զ������ͼ��
	static HICON					g_hCustomIconSm;

	static bool						g_isPreStyle = false;						///< �Ƿ�Ԥ�贰����ʽ
	static bool						g_isPreStyleEx = false;						///< �Ƿ�Ԥ�贰����չ��ʽ
	static bool						g_isPrePos = false;							///< �Ƿ�Ԥ�贰��λ��
	static bool						g_isPreShowState = false;					///< �Ƿ�Ԥ�贰����ʾ״̬����Ԥ�贴�����ں� ShowWindow �Ĳ�����
	static long						g_lPreStyle;								///< ��������ǰ��Ԥ����ʽ
	static long						g_lPreStyleEx;								///< ��������ǰ��Ԥ����չ��ʽ
	static POINT					g_pPrePos;									///< ��������ǰ��Ԥ�贰��λ��
	static int						g_nPreCmdShow;								///< ��������ǰ��Ԥ����ʾ״̬

	//static DrawMode					g_fDrawMode = DM_Normal;				///< ȫ�ֻ���ģʽ
	//static bool						g_bAutoFlush = true;					///< �Ƿ��Զ�ˢ��˫����

	static UINT						g_uWM_TASKBARCREATED;						///< ϵͳ��������Ϣ����

	////////////****** �������� ******////////////

	// static ����Ϊ�ڲ��������ⲿ�޷�����
	// ���ຯ��Ϊ����ӿں���

	// ���鴰�������Ƿ�Ϸ������жϴ����Ƿ����٣�����Ҫ��ʹ�� IsWindowExists��
	static bool IsValidWindowIndex(int index)
	{
		return index >= 0 && index < (int)g_vecWindows.size();
	}

	// ��ǰ�Ƿ���ڻ���ڣ��߼��ϻ���ڶ��� Alive �ģ�
	static bool HasFocusedWindow()
	{
		return IsValidWindowIndex(g_nFocusWindowIndex);
	}

	// ��ȡ��ǰ�������㴰��
	static EasyWindow& GetFocusedWindow()
	{
		static EasyWindow wndEmpty;
		if (HasFocusedWindow())
		{
			return g_vecWindows[g_nFocusWindowIndex];
		}
		else
		{
			wndEmpty = {};
			return wndEmpty;
		}
	}

	// ͨ�������ô˴����ڴ��ڼ�¼���е�����
	// ���� nullptr ����ǰ�����
	// δ�ҵ����� NO_WINDOW_INDEX
	static int GetWindowIndex(HWND hWnd)
	{
		if (hWnd == nullptr)
		{
			return g_nFocusWindowIndex;
		}
		int index = NO_WINDOW_INDEX;
		for (int i = 0; i < (int)g_vecWindows.size(); i++)
		{
			if (hWnd == g_vecWindows[i].hWnd)
			{
				index = i;
				break;
			}
		}
		return index;
	}

	bool IsAnyWindow()
	{
		for (auto& i : g_vecWindows)
			if (i.isWindowAlive)
				return true;
		return false;
	}

	bool IsWindowExists(HWND hWnd)
	{
		if (hWnd)
		{
			int index = GetWindowIndex(hWnd);
			if (IsValidWindowIndex(index))
			{
				return g_vecWindows[index].isWindowAlive;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return HasFocusedWindow();
		}
	}

	// �ж������Ƿ�Ϸ������жϴ����Ƿ���ڣ�δ�����٣�
	static bool IsWindowExists(int index)
	{
		return IsValidWindowIndex(index) && g_vecWindows[index].isWindowAlive;
	}

	//// �ȴ������ڲ���Ϣ�������
	//void WaitForProcessing(int index)
	//{
	//	// �����ڿ����������٣��ʲ��� isAliveWindow
	//	if (IsValidWindowIndex(index))
	//	{
	//		while (g_vecWindows[index].isBusyProcessing)
	//		{
	//			HpSleep(1);
	//		}
	//	}
	//}

	//void WaitForTask(HWND hWnd)
	//{
	//	// δ���þ��ʱֻ��Ҫ�ȴ���������������Ҫ�жϸþ���Ƿ��Ӧ�����
	//	if (!hWnd || (HasFocusedWindow() && GetFocusedWindow().hWnd == hWnd))
	//	{
	//		while (g_isInTask)
	//		{
	//			HpSleep(1);
	//		}
	//	}
	//}

	// �ͷŴ����ڴ�
	static void FreeWindow(int index)
	{
		if (!IsValidWindowIndex(index))
		{
			return;
		}

		// �ͷŻ�ͼ����
		if (g_vecWindows[index].pCanvas)
		{
			delete g_vecWindows[index].pCanvas;
			g_vecWindows[index].pCanvas = nullptr;
		}
		/*if (g_vecWindows[index].pBufferImg)
		{
			delete g_vecWindows[index].pBufferImg;
			g_vecWindows[index].pBufferImg = nullptr;
		}*/

		// �ͷ���Ϣ�б��ڴ�
		std::vector<ExMessage>().swap(g_vecWindows[index].vecMessage);

		//DestroyWindow(g_vecWindows[index].hWnd);
		//PostQuitMessage(0);
	}

	void closegraphHX(HWND hWnd)
	{
		// ������
		//// ���뽻��ԭ�߳̽��� DestroyWindow
		//// ���� WM_DESTROY ʱ������ wParam Ϊ 1����ʾ�����������ٴ���
		//SendMessage(g_vecWindows[index].hWnd, WM_DESTROY, 1, 0);

		DestroyWindow(hWnd);
	}

	void closeallgraph()
	{
		for (const auto& wnd : g_vecWindows)
		{
			if (wnd.isWindowAlive)
			{
				DestroyWindow(wnd.hWnd);
			}
		}
	}

	void SetWndProcFunc(HWND hWnd, WNDPROC WindowProcess)
	{
		int index = GetWindowIndex(hWnd);
		if (IsWindowExists(index))
		{
			g_vecWindows[index].funcWndProc = WindowProcess;
		}
	}

	void MsgLoopHX()
	{
		if (g_bAutoFlush)
		{
			FlushAllWindowBuffer(false);
		}

		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) // PM_REMOVE: ��������Ϣ�������Ƴ�
		{
			if (msg.message == WM_QUIT)	// ������ʽ��鲢�˳�
			{
				exit((int)msg.wParam);
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			//printf("processing\n");
		}
	}

	void SleepHX(int ms, int interval)
	{
		auto target = StdClock::now() + std::chrono::milliseconds(ms);
		while (StdClock::now() < target)
		{
			MsgLoopHX();
			std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		}
	}

	void HpSleepHX(int ms, int interval)
	{
		static auto oldclock = StdClock::now();		// ��̬��������¼��һ�� tick
		oldclock += std::chrono::milliseconds(ms);	// ���� tick
		while (StdClock::now() < oldclock)			// ��ʱ
		{
			MsgLoopHX();
			std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		}
		oldclock = StdClock::now();
	}

	Canvas* GetWindowCanvas(HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (IsWindowExists(index))
		{
			return g_vecWindows[index].pCanvas;
		}
		else
		{
			return nullptr;
		}
	}

	DWORD* GetImageBufferHX(IMAGE* img)
	{
		if (img)
		{
			return ::GetImageBuffer(img);
		}
		else if (HasFocusedWindow())
		{
			return ::GetImageBuffer(g_vecWindows[g_nFocusWindowIndex].pCanvas);
		}
		else
		{
			throw std::runtime_error("GetImageBufferHX(nullptr) called, but no working window assigned.");
		}
	}

	HDC GetImageHDCHX(IMAGE* img)
	{
		if (img)
		{
			return ::GetImageHDC(img);
		}
		else if (HasFocusedWindow())
		{
			return ::GetImageHDC(g_vecWindows[g_nFocusWindowIndex].pCanvas);
		}
		else
		{
			throw std::runtime_error("GetImageHDCHX(nullptr) called, but no working window assigned.");
		}
	}

	void SetWorkingImageHX(IMAGE* img)
	{
		if (img)
		{
			::SetWorkingImage(img);
		}
		else if (HasFocusedWindow())
		{
			::SetWorkingImage(g_vecWindows[g_nFocusWindowIndex].pCanvas);
		}
		else
		{
			//throw std::runtime_error("SetWorkingImageHX(nullptr) called, but no working window assigned.");
		}
	}

	IMAGE* GetWorkingImageHX()
	{
		return ::GetWorkingImage();
	}

	void setoriginHX(int x, int y, HWND hwnd)
	{
		// �˺���ԭ��μ� OnPaint ����ע��

		int index = GetWindowIndex(hwnd);
		if (index == NO_WINDOW_INDEX)
		{
			return;
		}

		// EasyX ԭ�� setorigin ������ԭ����ܲ����õ� SetViewportOrgEx����֮�������޷�ʹ��
		//setorigin(x, y);

		// �˴�ֱ�Ӳ��� GDI ��ʽʵ��
		SetViewportOrgEx(GetImageHDCHX(g_vecWindows[index].pCanvas), x, y, nullptr);
	}

	void getoriginHX(int* px, int* py, HWND hwnd)
	{
		int index = GetWindowIndex(hwnd);
		if (index == NO_WINDOW_INDEX)
		{
			return;
		}
		POINT pt;
		GetViewportOrgEx(GetImageHDCHX(g_vecWindows[index].pCanvas), &pt);
		if (px)
		{
			*px = pt.x;
		}
		if (py)
		{
			*py = pt.y;
		}
	}

	void setaspectratioHX(float xasp, float yasp, HWND hwnd)
	{
		// �˺���ԭ��μ� OnPaint ����ע��

		int index = GetWindowIndex(hwnd);
		if (index == NO_WINDOW_INDEX)
		{
			return;
		}

		// EasyX ԭ�� setaspectratio ������ԭ����
		// 1. �˿̵���ʱ���� IAMGE ������С����Ϊ w_old/xasp * h_old/yasp
		// 2. �Ժ� IMAGE::Resize(w, h) ʱ��ʵ�ʻ�����С������Ϊ w/xasp * h/yasp
		// 3. ����ʱ����ʵ����û�б仯
		// 4. �ú��������� IMAGE ����Ч
		// 5. EasyX ��Ӧ WM_PAINT ʱ��������Ӧ����HiEasyX ��Ӧ�Ĵ����� OnPaint ������
		setaspectratio(xasp, yasp);

		g_vecWindows[index].xasp = xasp;
		g_vecWindows[index].yasp = yasp;
	}

	void getaspectratioHX(float* pxasp, float* pyasp, HWND hwnd)
	{
		int index = GetWindowIndex(hwnd);
		if (index == NO_WINDOW_INDEX)
		{
			return;
		}
		if (pxasp)
		{
			*pxasp = g_vecWindows[index].xasp;
		}
		if (pyasp)
		{
			*pyasp = g_vecWindows[index].yasp;
		}
	}

	void AutoExit(bool enable)
	{
		g_bAutoExit = enable;
	}

	void SingleGraphWindow(bool enable)
	{
		g_bSingleWindow = enable;
	}

	HWND GetHWndHX()
	{
		return HasFocusedWindow() ? GetFocusedWindow().hWnd : nullptr;
	}

	bool SetWorkingWindow(HWND hWnd)
	{
		//if (!hWnd || GetFocusedWindow().hWnd == hWnd)
		//{
		//	if (GetWorkingImageHX() != GetFocusedWindow().pCanvas)
		//	{
		//		SetWorkingImage(GetFocusedWindow().pCanvas);
		//	}
		//	return true;
		//}

		int index = GetWindowIndex(hWnd);
		if (IsWindowExists(index))
		{
			//WaitForTask();
			//WaitForProcessing(index);
			g_nFocusWindowIndex = index;
			SetWorkingImageHX(GetFocusedWindow().pCanvas);
			return true;
		}
		else
		{
			return false;
		}
	}

	void FlushWindowBuffer(HWND hWnd, bool bInstant)
	{
		if (!IsWindowExists(hWnd))
		{
			return;
		}
		if (!hWnd)
		{
			hWnd = GetFocusedWindow().hWnd;
		}

		// ��Ǵ�������Ϊ��Ч���ȴ�����ʱ��ˢ��
		InvalidateRect(hWnd, nullptr, false); // ����������Ҫ��Ϊ false �Բ������������Է� WM_PAINT ��˸

		// �����ػ�
		if (bInstant)
		{
			//SendMessage(hWnd, WM_USER_REDRAW, 0, 0);
			UpdateWindow(hWnd);							// ��鴰�ڵĸ�����������Ϊ������������ WM_PAINT ��Ϣ�����ػ�
		}
		else
		{
			//PostMessage(hWnd, WM_USER_REDRAW, 0, 0);
		}

		// ��ʱ���룬������
		//// �ú����Ὣ����ȫ��������Ϊ��Ч������Ϣ���п���ʱ��ϵͳ���Զ����� WM_PAINT ��Ϣ�����ػ�
		//// false ��ʾ���ػ汳��
		//InvalidateRect(hWnd, 0, false);
		//// �ú�����鴰�ڵĸ�����������Ϊ������������ WM_PAINT ��Ϣ
		//UpdateWindow(hWnd);
	}

	void FlushAllWindowBuffer(bool bInstant)
	{
		for (const auto& wnd : g_vecWindows)
		{
			if (wnd.isWindowAlive)
			{
				FlushWindowBuffer(wnd.hWnd, bInstant);
			}
		}
	}

	void AutoFlushWindowBuffer(bool enable)
	{
		g_bAutoFlush = enable;
	}

	void BeginBatchDrawHX()
	{
		HiEasyX::AutoFlushWindowBuffer(false);
		HiEasyX::MsgLoopHX();
	}

	void FlushBatchDrawHX()
	{
		HiEasyX::FlushAllWindowBuffer(false);
		HiEasyX::MsgLoopHX();
	}

	void EndBatchDrawHX()
	{
		HiEasyX::FlushAllWindowBuffer(false);
		HiEasyX::MsgLoopHX();
	}

	// ���µ������ڻ�����С
	static void ResizeWindowImage(int index, RECT rct)
	{
		if (IsWindowExists(index))
		{
			g_vecWindows[index].pCanvas->Resize(rct.right, rct.bottom);
			//g_vecWindows[index].pBufferImg->Resize(rct.right, rct.bottom);
			g_vecWindows[index].isNewSize = true;
		}
	}

	static void ShowTray(NOTIFYICONDATA* nid)
	{
		Shell_NotifyIcon(NIM_ADD, nid);
	}

	void CreateTray(LPCTSTR lpszTrayName, HWND hWnd)
	{
		static int id = 0;

		int index = GetWindowIndex(hWnd);
		if (IsWindowExists(index))
		{
			HICON hIcon = g_hIconDefault;
			if (g_lpszCustomIconSm)
				hIcon = g_hCustomIconSm;
			else if (g_lpszCustomIcon)
				hIcon = g_hCustomIcon;

			g_vecWindows[index].isUseTray = true;
			g_vecWindows[index].nid.cbSize = sizeof(NOTIFYICONDATA);
			g_vecWindows[index].nid.hWnd = g_vecWindows[index].hWnd;
			g_vecWindows[index].nid.uID = id++;
			g_vecWindows[index].nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			g_vecWindows[index].nid.uCallbackMessage = WM_TRAY;
			g_vecWindows[index].nid.hIcon = hIcon;
			lstrcpy(g_vecWindows[index].nid.szTip, lpszTrayName);
			ShowTray(&g_vecWindows[index].nid);
		}
	}

	void DeleteTray(HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);

		// ������ɾ��ʱ����øú��������Բ��жϴ������ֻ��Ҫ�жϴ����Ƿ����
		if (IsValidWindowIndex(index))
		{
			if (g_vecWindows[index].isUseTray)
			{
				g_vecWindows[index].isUseTray = false;
				Shell_NotifyIcon(NIM_DELETE, &g_vecWindows[index].nid);
			}
		}
	}

	void SetTrayMenu(HMENU hMenu, HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (IsWindowExists(index))
		{
			g_vecWindows[index].isUseTrayMenu = true;
			g_vecWindows[index].hTrayMenu = hMenu;
		}
	}

	void SetTrayMenuProcFunc(void(*pFunc)(UINT), HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (IsWindowExists(index))
		{
			g_vecWindows[index].funcTrayMenuProc = pFunc;
		}
	}

	bool IsWindowSizeChanged(HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (IsValidWindowIndex(index))
		{
			bool b = g_vecWindows[index].isNewSize;
			g_vecWindows[index].isNewSize = false;
			return b;
		}
		else
		{
			return false;
		}
	}

	void SetWindowSizeLim(Optional<SIZE> sizeMin, Optional<SIZE> sizeMax, HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (IsValidWindowIndex(index))
		{
			g_vecWindows[index].sizeMin = sizeMin;
			g_vecWindows[index].sizeMax = sizeMax;
		}
	}

	void AutoResizeWindowCanvas(bool enable, HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (IsValidWindowIndex(index))
		{
			g_vecWindows[index].bAutoResizeCanvas = enable;
		}
	}

	bool GetCustomIconState()
	{
		return g_lpszCustomIcon;
	}

	void SetCustomIcon(LPCTSTR lpszIcon, LPCTSTR lpszIconSm)
	{
		g_lpszCustomIcon = lpszIcon;
		g_lpszCustomIconSm = lpszIconSm;
		g_hCustomIcon = LoadIcon(g_hInstance, lpszIcon);
		g_hCustomIconSm = LoadIcon(g_hInstance, lpszIconSm);
	}

	// ��ȡ��Ϣ����
	static std::vector<ExMessage>& GetMsgVector(HWND hWnd)
	{
		static std::vector<ExMessage> vec;
		int index = GetWindowIndex(hWnd);
		if (IsWindowExists(index))
		{
			return g_vecWindows[index].vecMessage;
		}
		else
		{
			vec.clear();
			return vec;
		}
	}

	// �Ƴ���ǰ��Ϣ
	static void RemoveMessage(HWND hWnd)
	{
		if (GetMsgVector(hWnd).size())
		{
			GetMsgVector(hWnd).erase(GetMsgVector(hWnd).begin());
		}
	}

	// �����Ϣ
	// ֧�ֻ����Ϣ����
	static void ClearMessage(BYTE filter, HWND hWnd)
	{
		for (size_t i = 0; i < GetMsgVector(hWnd).size(); i++)
			if (filter & GetExMessageType(GetMsgVector(hWnd)[i]))
				GetMsgVector(hWnd).erase(GetMsgVector(hWnd).begin() + i--);
	}

	// �Ƿ�������Ϣ
	// ֧�ֻ����Ϣ����
	static bool IsNewMessage(BYTE filter, HWND hWnd)
	{
		for (auto& element : GetMsgVector(hWnd))
			if (filter & GetExMessageType(element))
				return true;
		return false;
	}

	// �����Ϣ��ֱ����ȡ���������͵���Ϣ
	// ֧�ֻ����Ϣ����
	static ExMessage GetNextMessage(BYTE filter, HWND hWnd)
	{
		if (IsNewMessage(filter, hWnd))
		{
			for (size_t i = 0; i < GetMsgVector(hWnd).size(); i++)
			{
				if (filter & GetExMessageType(GetMsgVector(hWnd)[i]))
				{
					for (size_t j = 0; j < i; j++)
					{
						RemoveMessage(hWnd);
					}
					return GetMsgVector(hWnd)[0];
				}
			}
		}
		return {};
	}

	// ����������ȡ��Ϣ��������������ʽ��������Ϣ��������ô˺���
	// MsgLoopHX() ֻ�ڸ���������Ϣ�������н���
	Optional<ExMessage> getmessageHX(BYTE filter, HWND hWnd)
	{
		MsgLoopHX();
		while (!IsNewMessage(filter, hWnd))
		{
			if (!IsWindowExists(hWnd))	// �ȴ�������Ҫ���ⴰ���Ƿ񻹴���
			{
				return {};
			}
			SleepHX(10);	// �ں���Ϣѭ��
		}
		ExMessage msg = GetNextMessage(filter, hWnd);
		RemoveMessage(hWnd);
		return msg;
	}

	bool getmessageHX(ExMessage* msg, BYTE filter, HWND hWnd)
	{
		Optional<ExMessage> msgEx = getmessageHX(filter, hWnd);
		if (msgEx.isSet)
		{
			if (msg)
			{
				*msg = msgEx;
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	// �����������ȡ��Ϣ��������������ʽ�ķ�������Ϣ��������ô˺���
	bool peekmessageHX(ExMessage* msg, BYTE filter, bool removemsg, HWND hWnd)
	{
		MsgLoopHX();
		if (IsNewMessage(filter, hWnd))
		{
			if (msg)		*msg = GetNextMessage(filter, hWnd);
			if (removemsg)	RemoveMessage(hWnd);
			return true;
		}
		return false;
	}

	// ���������Ϣ��������������ʽ��������Ϣ��������ô˺���
	void flushmessageHX(BYTE filter, HWND hWnd)
	{
		MsgLoopHX();
		ClearMessage(filter, hWnd);
	}

	// MouseMsg ���еĶ����ж�����Ϣ������
	bool MouseHitHX(HWND hWnd)
	{
		MsgLoopHX();
		return IsNewMessage(EM_MOUSE, hWnd);
	}

	Optional<MOUSEMSG> GetMouseMsgHX(HWND hWnd)
	{
		Optional<ExMessage> msgEx = getmessageHX(EM_MOUSE, hWnd);
		if (msgEx.isSet)
		{
			return To_MouseMsg(msgEx);
		}
		else
		{
			return {};
		}
	}

	bool PeekMouseMsgHX(MOUSEMSG* pMsg, bool bRemoveMsg, HWND hWnd)
	{
		ExMessage msgEx;
		bool r = peekmessageHX(&msgEx, EM_MOUSE, bRemoveMsg, hWnd);
		*pMsg = To_MouseMsg(msgEx);
		return r;
	}

	void FlushMouseMsgBufferHX(HWND hWnd)
	{
		flushmessageHX(EM_MOUSE, hWnd);
	}

	ExMessage To_ExMessage(MOUSEMSG msg)
	{
		ExMessage msgEx = {};
		msgEx.message = msg.uMsg;
		msgEx.ctrl = msg.mkCtrl;
		msgEx.shift = msg.mkShift;
		msgEx.lbutton = msg.mkLButton;
		msgEx.mbutton = msg.mkMButton;
		msgEx.rbutton = msg.mkRButton;
		msgEx.x = msg.x;
		msgEx.y = msg.y;
		msgEx.wheel = msg.wheel;
		return msgEx;
	}

	MOUSEMSG To_MouseMsg(ExMessage msgEx)
	{
		MOUSEMSG msg = {};
		if (GetExMessageType(msgEx) == EM_MOUSE)
		{
			msg.uMsg = msgEx.message;
			msg.mkCtrl = msgEx.ctrl;
			msg.mkShift = msgEx.shift;
			msg.mkLButton = msgEx.lbutton;
			msg.mkMButton = msgEx.mbutton;
			msg.mkRButton = msgEx.rbutton;
			msg.x = msgEx.x;
			msg.y = msgEx.y;
			msg.wheel = msgEx.wheel;
		}
		return msg;
	}

	UINT GetExMessageType(ExMessage msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
			return EM_MOUSE;
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			return EM_KEY;
			break;
		case WM_CHAR:
			return EM_CHAR;
			break;
		case WM_ACTIVATE:
		case WM_MOVE:
		case WM_SIZE:
			return EM_WINDOW;
			break;
		default:
			return 0;
			break;
		}
	}

	void PreSetWindowStyle(long lStyle)
	{
		g_isPreStyle = true;
		g_lPreStyle = lStyle;
	}

	void PreSetWindowStyleEx(long lStyleEx)
	{
		g_isPreStyleEx = true;
		g_lPreStyleEx = lStyleEx;
	}

	void PreSetWindowPos(int x, int y)
	{
		g_isPrePos = true;
		g_pPrePos = { x,y };
	}

	void PreSetWindowShowState(int nCmdShow)
	{
		g_isPreShowState = true;
		g_nPreCmdShow = nCmdShow;
	}

	int SetWindowStyle(long lNewStyle, HWND hWnd)
	{
		if (hWnd == nullptr)	hWnd = GetFocusedWindow().hWnd;
		return SetWindowLong(hWnd, GWL_STYLE, lNewStyle);
	}

	int SetWindowExStyle(long lNewExStyle, HWND hWnd)
	{
		if (hWnd == nullptr)	hWnd = GetFocusedWindow().hWnd;
		return SetWindowLong(hWnd, GWL_EXSTYLE, lNewExStyle);
	}

	/**
	 * @brief ��ȡ WindowStyle Ԥ����ʽ����ϸ��Ϣ
	 */
	static WindowStyleInfo GetWindowStyleHX_Internal(WindowStyle style)
	{
		WindowStyleInfo info = {};
		info.style_name = style;
		switch (style)
		{
		case Resizable:
			info.is_ex = false;
			info.style_code = WS_SIZEBOX | WS_MAXIMIZEBOX;
			break;
		case Maximizable:
			info.is_ex = false;
			info.style_code = WS_MAXIMIZEBOX;
			break;
		case SystemMenu:
			info.is_ex = false;
			info.style_code = WS_SYSMENU;
			break;
		case ToolWindow:
			info.is_ex = true;
			info.style_code = WS_EX_TOOLWINDOW;
			break;
		case VScroll:
			info.is_ex = false;
			info.style_code = WS_VSCROLL;
			break;
		case HScroll:
			info.is_ex = false;
			info.style_code = WS_HSCROLL;
			break;
		}
		return info;
	}

	// �ڲ����ô�����ʽ�ĺ�����������ɺ� Win32 API �����Ĳ��֣�
	// hwnd			Ŀ�괰��
	// state		���û��ǽ���
	// isExStyle	�Ƿ�Ϊ Ex ����
	// styleCode	���Դ���
	static void SetWindowStyleHX_Internal(HWND hwnd, bool state, bool isExStyle, long styleCode)
	{
		hwnd = hwnd ? hwnd : GetHWndHX();
		if (isExStyle)
		{
			long exstyle = GetWindowExStyle(hwnd);
			if (state)
			{
				exstyle |= styleCode;
			}
			else
			{
				exstyle &= ~styleCode;
			}

			SetWindowExStyle(exstyle, hwnd);
		}
		else
		{
			long style = GetWindowStyle(hwnd);
			if (state)
			{
				style |= styleCode;
			}
			else
			{
				style &= ~styleCode;
			}

			SetWindowStyle(style, hwnd);
		}
	}

	void SetWindowStyleHX(WindowStyle style, bool bEnable, HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (!IsValidWindowIndex(index))
		{
			return;
		}

		WindowStyleInfo info = GetWindowStyleHX_Internal(style);
		SetWindowStyleHX_Internal(g_vecWindows[index].hWnd, bEnable, info.is_ex, info.style_code);
		g_vecWindows[index].bStylesUsage[style] = bEnable;

		// ��������������
		if (style == VScroll || style == HScroll)
		{
			// �������Ŀ����͹ر���Ҫʹ�� SWP_FRAMECHANGED ˢ��
			SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

			// ���¹���������
			UpdateScrollInfo(g_vecWindows[index].hWnd);
		}
	}

	bool HasWindowStyleHX(WindowStyle style, HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (!IsValidWindowIndex(index))
		{
			return false;
		}

		// NOTE
		// ʹ�� Win32 �ķ��������ж������δ֪���⣬���ƺ��ڴ��ڵ� Canvas IMAGE::Resize ǰ��
		// GetWindowStyle �ķ���ֵ������ͬ�������ж�����Ŀǰԭ������

		/*WindowStyleInfo info = GetWindowStyleHX_Internal(style);
		long current_style = 0;
		if (info.is_ex)
		{
			current_style = GetWindowExStyle(g_vecWindows[index].hWnd);
		}
		else
		{
			current_style = GetWindowStyle(g_vecWindows[index].hWnd);
		}

		long test = GetWindowStyle(g_vecWindows[index].hWnd);

		return (current_style & info.style_code) == info.style_code;*/

		// Ŀǰֻ���ֶ��洢һ�� Style ���������
		return g_vecWindows[index].bStylesUsage[style];
	}

	void UpdateScrollInfo(HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (!IsValidWindowIndex(index))
		{
			return;
		}

		// �ͻ�����С
		RECT rctWnd;
		GetClientRect(g_vecWindows[index].hWnd, &rctWnd);	// ���� LT(0, 0)

		// ���Թ����ķ�Χ��С
		int max_x = g_vecWindows[index].pCanvas->getwidth();
		int max_y = g_vecWindows[index].pCanvas->getheight();
		if (max_x < 0)
		{
			max_x = 0;
		}
		if (max_y < 0)
		{
			max_y = 0;
		}

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;

		// y ����
		if (HasWindowStyleHX(VScroll, g_vecWindows[index].hWnd))
		{
			// �洢�ɵ� ScrollInfo
			GetScrollInfo(g_vecWindows[index].hWnd, SB_VERT, &si);
			int oldPageSize = si.nPage;
			int oldPos = si.nPos;

			// �����µ� ScrollInfo
			si.nMin = 0;
			si.nMax = max_y;
			si.nPage = rctWnd.bottom;	// һҳ�ĳߴ磬��ֵ��Ҫ�Ǳ��������Լ��� OnScroll ʱ�õ�
			SetScrollInfo(g_vecWindows[index].hWnd, SB_VERT, &si, true);

			// ���� nPage �ĸı���Ӧ�ع������ڣ����ǶԿؼ��Ĺ��������ǶԻ�ͼ���ݵģ�
			// ���ڿؼ���������������
			// 1. ��ԭ�ȵ���ͼ���Ѿ��ܿ���ҳ��ײ���oldPos + oldPageSize >= max_y��
			// 2. ���ڸ߶ȼ������ӣ��� PageSize ������si.nPage - oldPageSize��������
			// ��ô�������еĿؼ���Ҫ�����¹����¹�������ڵĴ��ڵĸ߶�����
			int scroll_amount = si.nPage - oldPageSize;
			if (scroll_amount > 0 && oldPos + oldPageSize >= max_y)
			{
				// ע�����ÿؼ���������Ϊ�кܶ�����������ô����첻��֧��
				//ScrollWindow(g_vecWindows[index].hWnd, 0, scroll_amount, NULL, NULL);

				// ���ã��������
				//UpdateWindow(g_vecWindows[index].hWnd);
			}
		}
		// x ����
		if (HasWindowStyleHX(HScroll, g_vecWindows[index].hWnd))
		{
			GetScrollInfo(g_vecWindows[index].hWnd, SB_HORZ, &si);
			int oldPageSize = si.nPage;
			int oldPos = si.nPos;

			si.nMin = 0;
			si.nMax = max_x;
			si.nPage = rctWnd.right;
			SetScrollInfo(g_vecWindows[index].hWnd, SB_HORZ, &si, true);

			int scroll_amount = si.nPage - oldPageSize;
			if (scroll_amount > 0 && oldPos + oldPageSize >= max_x)
			{
				//ScrollWindow(g_vecWindows[index].hWnd, scroll_amount, 0, NULL, NULL);
			}
		}
	}

	POINT GetWindowPos(HWND hWnd)
	{
		if (!hWnd)	hWnd = GetFocusedWindow().hWnd;
		RECT rct;
		GetWindowRect(hWnd, &rct);
		return { rct.left, rct.top };
	}

	SIZE GetWindowSize(HWND hWnd)
	{
		if (!hWnd)	hWnd = GetFocusedWindow().hWnd;
		RECT rct;
		GetWindowRect(hWnd, &rct);
		return { rct.right - rct.left, rct.bottom - rct.top };
	}

	void MoveWindow(int x, int y, HWND hWnd)
	{
		if (!hWnd)	hWnd = GetFocusedWindow().hWnd;
		SetWindowPos(hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
	}

	void MoveWindowRel(int dx, int dy, HWND hWnd)
	{
		if (!hWnd)	hWnd = GetFocusedWindow().hWnd;
		POINT pos = GetWindowPos(hWnd);
		SetWindowPos(hWnd, HWND_TOP, pos.x + dx, pos.y + dy, 0, 0, SWP_NOSIZE);
	}

	void ResizeWindow(int w, int h, HWND hWnd)
	{
		if (!hWnd)	hWnd = GetFocusedWindow().hWnd;
		SetWindowPos(hWnd, HWND_TOP, 0, 0, w, h, SWP_NOMOVE);
	}

	void SetWindowTitle(LPCTSTR lpszTitle, HWND hWnd)
	{
		if (!hWnd)	hWnd = GetFocusedWindow().hWnd;
		SetWindowText(hWnd, lpszTitle);
	}

	void SetWindowTransparent(HWND HWnd, bool enable, int alpha)
	{
		//���Խ��ۣ��˺��������� UpdateLayeredWindow һͬʹ��

		LONG nRet = ::GetWindowLong(HWnd, GWL_EXSTYLE);
		nRet |= WS_EX_LAYERED;
		::SetWindowLong(HWnd, GWL_EXSTYLE, nRet);

		if (!enable) alpha = 0xFF;
		SetLayeredWindowAttributes(HWnd, 0, alpha, LWA_ALPHA);
	}

	// ��ȡĬ�ϴ���ͼ��
	static HICON GetDefaultAppIcon()
	{
		static HBITMAP hBmp = Image2Bitmap(GetIconImage(), true);
		static HICON hIcon = Bitmap2Icon(hBmp);
		static bool init = false;
		if (!init)
		{
			DeleteObject(hBmp);
			init = true;
		}
		return hIcon;
	}

	static Optional<LRESULT> OnSize(int indexWnd, WPARAM wParam, LPARAM lParam)
	{
		RECT rctWnd;
		GetClientRect(g_vecWindows[indexWnd].hWnd, &rctWnd);

		// �Զ�����������������С
		if (g_vecWindows[indexWnd].bAutoResizeCanvas)
		{
			ResizeWindowImage(indexWnd, rctWnd);
		}

		// ���������
		if (HasWindowStyleHX(VScroll) || HasWindowStyleHX(HScroll))
		{
			UpdateScrollInfo(g_vecWindows[indexWnd].hWnd);
		}

		return 0;
	}

	static Optional<LRESULT> OnGetMinMaxInfo(int indexWnd, LPARAM lParam)
	{
		MINMAXINFO* lpMMI = (MINMAXINFO*)lParam;

		RECT rectClient;
		GetClientRect(g_vecWindows[indexWnd].hWnd, &rectClient);
		RECT rectWindow;
		GetWindowRect(g_vecWindows[indexWnd].hWnd, &rectWindow);

		// ����������ͱ߿�ߴ�
		int nWidthOverhead = rectWindow.right - rectWindow.left - rectClient.right + rectClient.left;
		int nHeightOverhead = rectWindow.bottom - rectWindow.top - rectClient.bottom + rectClient.top;

		// ������С���ٳߴ�
		if (g_vecWindows[indexWnd].sizeMin.isSet)
		{
			lpMMI->ptMinTrackSize.x = g_vecWindows[indexWnd].sizeMin.value.cx + nWidthOverhead;
			lpMMI->ptMinTrackSize.y = g_vecWindows[indexWnd].sizeMin.value.cy + nHeightOverhead;
		}
		// ���������ٳߴ�
		if (g_vecWindows[indexWnd].sizeMax.isSet)
		{
			lpMMI->ptMaxTrackSize.x = g_vecWindows[indexWnd].sizeMax.value.cx + nWidthOverhead;
			lpMMI->ptMaxTrackSize.y = g_vecWindows[indexWnd].sizeMax.value.cy + nHeightOverhead;
		}

		return 0;
	}

	/**
	 * @brief ���� WM_VSCROLL �� WM_HSCROLL ��Ϣ
	 * @note �����ڴ����û���ק���������¼��������ڴ��������쵼�µĹ��������£����� OnSize ����ɣ�
	 */
	static Optional<LRESULT> OnScroll(int indexWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		HWND hwnd = g_vecWindows[indexWnd].hWnd;

		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS;
		GetScrollInfo(hwnd, SB_HORZ, &si);
		int xPos_old = si.nPos;
		GetScrollInfo(hwnd, SB_VERT, &si);
		int yPos_old = si.nPos;			// Save the position for comparison later on.

		//RECT rctWnd;
		//GetClientRect(hwnd, &rctWnd);	// left and top will always be zero

		switch (msg)
		{
		case WM_VSCROLL:
		{
			// Get all the vertial scroll bar information.
			si.cbSize = sizeof(si);
			si.fMask = SIF_ALL;
			GetScrollInfo(hwnd, SB_VERT, &si);

			switch (LOWORD(wParam))
			{

				// User clicked the HOME keyboard key.
			case SB_TOP:
				si.nPos = si.nMin;
				break;

				// User clicked the END keyboard key.
			case SB_BOTTOM:
				si.nPos = si.nMax;
				break;

				// User clicked the top arrow.
			case SB_LINEUP:
				si.nPos -= 10;
				break;

				// User clicked the bottom arrow.
			case SB_LINEDOWN:
				si.nPos += 10;
				break;

				// User clicked the scroll bar shaft above the scroll box.
				// ע������� PAGEUP, PAGEDOWN ���û���������������ϡ��·��Ŀհ�λ��ʱ����
			case SB_PAGEUP:
				si.nPos -= si.nPage;
				break;

				// User clicked the scroll bar shaft below the scroll box.
			case SB_PAGEDOWN:
				si.nPos += si.nPage;
				break;

				// User dragged the scroll box.
			case SB_THUMBTRACK:
				si.nPos = si.nTrackPos;
				break;

			default:
				break;
			}

			// Set the position and then retrieve it.  Due to adjustments
			// by Windows it may not be the same as the value set.
			si.fMask = SIF_POS;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			GetScrollInfo(hwnd, SB_VERT, &si);

			// If the position has changed, scroll window and update it.
			if (si.nPos != yPos_old)
			{
				// ���������ʵ�ִ����ϵĿؼ���������ƶ�
				// ���ѽ���
				//ScrollWindow(hwnd, 0, yPos_old - si.nPos, NULL, NULL);
				//UpdateWindow(hwnd);

				// �����ػ棨�����Ϳ��Ա��϶��߿����϶�Ч����
				FlushWindowBuffer(g_vecWindows[indexWnd].hWnd, true);
			}

			return 0;
			break;
		}

		case WM_HSCROLL:
		{
			si.cbSize = sizeof(si);
			si.fMask = SIF_ALL;
			GetScrollInfo(hwnd, SB_HORZ, &si);

			switch (LOWORD(wParam))
			{
			case SB_TOP:
				si.nPos = si.nMin;
				break;

			case SB_BOTTOM:
				si.nPos = si.nMax;
				break;

			case SB_LINEUP:
				si.nPos -= 10;
				break;

			case SB_LINEDOWN:
				si.nPos += 10;
				break;

			case SB_PAGEUP:
				si.nPos -= si.nPage;
				break;

			case SB_PAGEDOWN:
				si.nPos += si.nPage;
				break;

			case SB_THUMBTRACK:
				si.nPos = si.nTrackPos;
				break;

			default:
				break;
			}

			si.fMask = SIF_POS;
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			GetScrollInfo(hwnd, SB_HORZ, &si);

			if (si.nPos != xPos_old)
			{
				//ScrollWindow(hwnd, xPos_old - si.nPos, 0, NULL, NULL);

				FlushWindowBuffer(g_vecWindows[indexWnd].hWnd, true);
			}

			return 0;
			break;
		}

		default:
			return {};	// δ����
			break;
		}
	}

	static Optional<LRESULT> OnTray(int indexWnd, LPARAM lParam)
	{
		if (g_vecWindows[indexWnd].isUseTray)
		{
			HWND hWnd = g_vecWindows[indexWnd].hWnd;
			POINT ptMouse;
			GetCursorPos(&ptMouse);

			switch (lParam)
			{
				// ��������
			case WM_LBUTTONDOWN:
				SetForegroundWindow(hWnd);
				break;

				// �Ҽ��򿪲˵�
			case WM_RBUTTONDOWN:
				if (g_vecWindows[indexWnd].isUseTrayMenu)
				{
					SetForegroundWindow(hWnd);	// ����һ�´��ڣ���ֹ�˵�����ʧ

					// ��ʾ�˵�������
					int nMenuId = TrackPopupMenu(g_vecWindows[indexWnd].hTrayMenu, TPM_RETURNCMD, ptMouse.x, ptMouse.y, 0, hWnd, nullptr);
					if (nMenuId == 0) PostMessage(hWnd, WM_LBUTTONDOWN, 0, 0);
					if (g_vecWindows[indexWnd].funcTrayMenuProc)
					{
						g_vecWindows[indexWnd].funcTrayMenuProc(nMenuId);
					}
				}
				break;

			default:
				break;
			}

			return 0;
		}
		else
		{
			return {};	// δ�������Ϣ
		}
	}

	static Optional<LRESULT> OnTaskBarCreated(int indexWnd)
	{
		if (g_vecWindows[indexWnd].isUseTray)
		{
			ShowTray(&g_vecWindows[indexWnd].nid);
			return 0;
		}
		else
		{
			return {}; // δ����
		}
	}

	// �Ǽ� ExMessage ��Ϣ
	static Optional<LRESULT> RegisterExMessage(int indexWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// ��¼��Ϣ�¼�
		switch (msg)
		{
			// EM_MOUSE
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		{
			ExMessage msgMouse = {};
			msgMouse.message = msg;
			msgMouse.x = GET_X_LPARAM(lParam);
			msgMouse.y = GET_Y_LPARAM(lParam);
			msgMouse.wheel = GET_WHEEL_DELTA_WPARAM(wParam);
			msgMouse.shift = LOWORD(wParam) & 0x04 ? true : false;
			msgMouse.ctrl = LOWORD(wParam) & 0x08 ? true : false;
			msgMouse.lbutton = LOWORD(wParam) & 0x01 ? true : false;
			msgMouse.mbutton = LOWORD(wParam) & 0x10 ? true : false;
			msgMouse.rbutton = LOWORD(wParam) & 0x02 ? true : false;

			// �й�����Ϣʱ���õ�����������Ļ���꣬��Ҫת��
			if (msgMouse.wheel)
			{
				POINT p = { msgMouse.x ,msgMouse.y };
				ScreenToClient(g_vecWindows[indexWnd].hWnd, &p);
				msgMouse.x = (short)p.x;
				msgMouse.y = (short)p.y;
			}

			// ���ھ��й�����ʱ����ȡ��������Ҫת��
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS;
			if (HasWindowStyleHX(HScroll, g_vecWindows[indexWnd].hWnd))
			{
				GetScrollInfo(g_vecWindows[indexWnd].hWnd, SB_HORZ, &si);
				msgMouse.x += si.nPos;
			}
			if (HasWindowStyleHX(VScroll, g_vecWindows[indexWnd].hWnd))
			{
				GetScrollInfo(g_vecWindows[indexWnd].hWnd, SB_VERT, &si);
				msgMouse.y += si.nPos;
			}

			g_vecWindows[indexWnd].vecMessage.push_back(msgMouse);

			return 0;
		}
		break;

		// EM_KEY
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		{
			// code from MSDN
			WORD vkCode = LOWORD(wParam);                                 // virtual-key code
			WORD keyFlags = HIWORD(lParam);
			WORD scanCode = LOBYTE(keyFlags);                             // scan code
			BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED; // extended-key flag, 1 if scancode has 0xE0 prefix

			if (isExtendedKey)
				scanCode = MAKEWORD(scanCode, 0xE0);

			BOOL repeatFlag = (keyFlags & KF_REPEAT) == KF_REPEAT;        // previous key-state flag, 1 on autorepeat
			WORD repeatCount = LOWORD(lParam);                            // repeat count, > 0 if several keydown messages was combined into one message
			BOOL upFlag = (keyFlags & KF_UP) == KF_UP;                    // transition-state flag, 1 on keyup

			// ���ܼ�������������
			// if we want to distinguish these keys:
			//switch (vkCode)
			//{
			//case VK_SHIFT:   // converts to VK_LSHIFT or VK_RSHIFT
			//case VK_CONTROL: // converts to VK_LCONTROL or VK_RCONTROL
			//case VK_MENU:    // converts to VK_LMENU or VK_RMENU
			//	vkCode = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
			//	break;
			//}

			ExMessage msgKey = {};
			msgKey.message = msg;
			msgKey.vkcode = (BYTE)vkCode;
			msgKey.scancode = (BYTE)scanCode;
			msgKey.extended = isExtendedKey;
			msgKey.prevdown = repeatFlag;

			g_vecWindows[indexWnd].vecMessage.push_back(msgKey);

			// ������̨��һ�ݣ���֧�� _getch() ϵ�к���
			// ��������û�������� _getch()����ᵼ�´�����Ϣ���������ڿ��������ʲ�������
			PostMessage(GetConsoleWindow(), msg, wParam, lParam);

			return 0;
		}
		break;

		// EM_CHAR
		case WM_CHAR:
		{
			ExMessage msgChar = {};
			msgChar.message = msg;
			msgChar.ch = (TCHAR)wParam;
			g_vecWindows[indexWnd].vecMessage.push_back(msgChar);

			// ֪ͨ����̨
			PostMessage(GetConsoleWindow(), msg, wParam, lParam);

			return 0;
		}
		break;

		// EM_WINDOW
		case WM_ACTIVATE:
		case WM_MOVE:
		case WM_SIZE:
		{
			ExMessage msgWindow = {};
			msgWindow.message = msg;
			msgWindow.wParam = wParam;
			msgWindow.lParam = lParam;
			g_vecWindows[indexWnd].vecMessage.push_back(msgWindow);

			return 0;
		}
		break;

		// δ����
		default:
		{
			return {};
		}

		}
	}

	// �����û�����
	static void OnPaint(int indexWnd, HDC hdc)
	{
		const EasyWindow& wnd_info = g_vecWindows[indexWnd];

		// ����ͼ������������� HDC
		RECT rctWnd;
		GetClientRect(wnd_info.hWnd, &rctWnd);
		//CopyImageToHDC(wnd_info.pCanvas, hdc, rctWnd);

		// ��ȡ���ڻ����Ļ���ԭ�㣬�Ժ�Ӧ setoriginHX
		HDC hdcCanvas = GetImageHDCHX(wnd_info.pCanvas);
		POINT ptCanvasOrg;
		GetViewportOrgEx(hdcCanvas, &ptCanvasOrg);

		// ��������˹�����������ݹ�����λ�����޸Ļ���ԭ��
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS; // ֻ��ȡλ����Ϣ
		if (HasWindowStyleHX(HScroll, wnd_info.hWnd))
		{
			GetScrollInfo(wnd_info.hWnd, SB_HORZ, &si);
			int scroll_x = si.nPos;
			ptCanvasOrg.x -= scroll_x;
		}
		if (HasWindowStyleHX(VScroll, wnd_info.hWnd))
		{
			GetScrollInfo(wnd_info.hWnd, SB_VERT, &si);
			int scroll_y = si.nPos;
			ptCanvasOrg.y -= scroll_y;
		}

		if (wnd_info.xasp != 1 || wnd_info.yasp != 1)
		{
			if (fabs(wnd_info.xasp) > 1e-4 && fabs(wnd_info.yasp) > 1e-4)
			{
				StretchBlt(
					hdc,	/* Ŀ���豸 */
					rctWnd.left, rctWnd.top, rctWnd.right, rctWnd.bottom, /* Ŀ��������� */
					hdcCanvas, /* Դ�豸 */
					(int)(-ptCanvasOrg.x / wnd_info.xasp), /* Դ������� */
					(int)(-ptCanvasOrg.y / wnd_info.yasp),
					(int)((rctWnd.right - rctWnd.left) / wnd_info.xasp),
					(int)((rctWnd.bottom - rctWnd.top) / wnd_info.yasp),
					SRCCOPY
				);
			}
			/**
			 * ע�⣺
			 *		EasyX setaspectratio ��ԭ���ǽ�������С����Ϊ w_old / xasp �� h_old / yasp��
			 *		Ȼ���ڻ����ϻ���ʱ�������κδ���������ԭ�����ƣ�����ڴ��ڻ���ʱ��
			 *		�� (w_old / xasp, h_old / yasp) ��С�Ļ�������ѹ���� (w_old, h_old) ��С��
			 *		�������������Ч������ x ����Ļ������ݶ������� xasp ������y ����Ļ������ݶ������� yasp ������
			 *
			 *		EasyX ԭ���� setaspectratio ���������� IMAGE ����Ч��Ч�����ǵ���������СΪ�����õ� (1/xasp, 1/yasp) ����
			 *		���������û������ݲ�Խ�硣���磬xasp = 0.5 ʱ���û������ڸ������������ϻ������ݣ����Ի�����Ҫ����
			*/
		}
		else
		{
			BitBlt(hdc,	/* Ŀ���豸 */
				rctWnd.left, rctWnd.top, rctWnd.right, rctWnd.bottom, /* Ŀ��������� */
				hdcCanvas, /* Դ�豸 */
				-ptCanvasOrg.x, -ptCanvasOrg.y, /* Դ������� */
				SRCCOPY);
			/**
			 * ע�⣺
			 *		�� setoriginHX �е����� SetViewportOrgEx �������û����Ļ���ԭ�㣬
			 *		�˺󻭲���Сû�䣬���Ǵ���Ļ������궼��������ԭ�����ƫ�ƣ�
			 *		�˴� BitBlt ��ָ��Դ�������ʱ������Ҳ���ܵ���ƫ�Ƶ�Ӱ�죬
			 *		���Ҫ��ȡԴ������ԭ�����꣬Ȼ����� -1 ����Ϊ�������롣
			*/
		}
	}

	static Optional<LRESULT> OnMove(HWND hWnd)
	{
		//RECT rctWnd;
		//GetWindowRect(hWnd, &rctWnd);

		//// �ƶ����ڳ�����Ļʱ���ܵ����Ӵ�����ʾ�����⣬���Դ�ʱ��Ҫ�����ػ�
		//// ����û�����һֱ��ǿ���ػ棬��˲������ࡣ
		//if (rctWnd.left < g_screenSize.left || rctWnd.top < g_screenSize.top
		//	|| rctWnd.right > g_screenSize.left + g_screenSize.w
		//	|| rctWnd.bottom > g_screenSize.top + g_screenSize.h)
		//{
		//	EnforceRedraw(hWnd);
		//}

		return 0;
	}

	static Optional<LRESULT> OnDestroy(int indexWnd, WPARAM wParam)
	{
		if (!IsWindowExists(indexWnd))
		{
			return {};	// δ����
		}
		// ������
		//// �����ô����������ٱ�ʶæµ���ȴ��������
		//g_vecWindows[index].isWindowAlive = false;
		//g_vecWindows[index].isBusyProcessing = true;
		//WaitForTask(g_vecWindows[index].hWnd);

		g_vecWindows[indexWnd].isWindowAlive = false;

		// ������
		//// �������ø�����Ϊģ̬���ڣ�����Ҫ�������ڻָ�����
		//if (g_vecWindows[index].hParent != nullptr)
		//{
		//	EnableWindow(g_vecWindows[index].hParent, true);
		//	SetForegroundWindow(g_vecWindows[index].hParent);
		//}

		// ж������
		DeleteTray(g_vecWindows[indexWnd].hWnd);

		// �������ڱ����٣�����Ҫ���û��������
		if (indexWnd == g_nFocusWindowIndex)
		{
			// Լ�������ڱ����ٺ��ÿջ����
			// ���ÿգ����ܵ����û���������Դ��ڵĲ�����������
			// HiWindow �ڲ�����Ҳ���ڴ˱�д
			g_nFocusWindowIndex = NO_WINDOW_INDEX;
		}

		// �ͷŴ����ڴ�
		FreeWindow(indexWnd);

		//// �ر�æµ��ʶ
		//g_vecWindows[index].isBusyProcessing = false;

		// ����رմ˴��ں󲻴����κδ���
		if (!IsAnyWindow())
		{
			// �ر� GDI+ ��ͼ����
			Gdiplus_Shutdown();

			// �����Ҫ�Զ��˳�����
			if (g_bAutoExit)
			{
				PostQuitMessage(0);
			}
		}

		return 0;
	}

	static HWND OnSysCtrlCreate(int indexWnd, WPARAM wParam, LPARAM lParam)
	{
		CREATESTRUCT* c = (CREATESTRUCT*)lParam;
		HWND hWnd = CreateWindow(
			c->lpszClass,
			c->lpszName,
			c->style,
			c->x, c->y,
			c->cx, c->cy,
			c->hwndParent,
			c->hMenu,
			GetModuleHandle(0),
			c->lpCreateParams
		);

		// ��¼
		g_vecWindows[indexWnd].vecSysCtrl.push_back((SysControlBase*)wParam);
		return hWnd;
	}

	// ����ϵͳ�ؼ���Ϣ
	// bRet ����������Ƿ�ֱ�ӷ���
	static Optional<LRESULT> SysCtrlProc(int indexWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			// ����ϵͳ�ؼ�
		case WM_SYSCTRL_CREATE:
		{
			g_vecWindows[indexWnd].bHasCtrl = true;
			return (LRESULT)OnSysCtrlCreate(indexWnd, wParam, lParam);
			break;
		}

		// ����ϵͳ�ؼ�
		case WM_SYSCTRL_DELETE:
		{
			// �������Ŀؼ�ָ����Ϊ��
			for (size_t i = 0; i < g_vecWindows[indexWnd].vecSysCtrl.size(); i++)
			{
				if (g_vecWindows[indexWnd].vecSysCtrl[i] == (SysControlBase*)wParam)
				{
					g_vecWindows[indexWnd].vecSysCtrl[i] = nullptr;
				}
			}

			return 0;
			break;
		}
		}

		// ���ڿؼ�ʱ���ɷ���Ϣ
		if (g_vecWindows[indexWnd].bHasCtrl)
		{
			bool bCtrlRet = false;
			LRESULT lr = 0;
			for (auto& pCtrl : g_vecWindows[indexWnd].vecSysCtrl)
			{
				if (pCtrl)
				{
					LRESULT lr = pCtrl->UpdateMessage(msg, wParam, lParam, bCtrlRet);
					if (bCtrlRet)
					{
						return lr;
					}
				}
			}
		}

		return {};
	}

	static void OnCreate(int indexWnd, HWND hWnd, LPARAM lParam)
	{
	}

	// ���ڹ��̺���
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		int indexWnd = GetWindowIndex(hWnd);		// ��������
		Optional<LRESULT> retVal = 0;			// ��¼����ֵ

		// ���ô��ڲ��ڴ����б���
		if (!IsValidWindowIndex(indexWnd))
		{
			// ����� WM)CREATE ��Ϣ��������Ϊ�������ڴ���������û�д洢�ڴ����б��У��Դ����⴦��
			if (msg == WM_CREATE)
			{
				// �������ڴ����Ĵ��ڣ������б����Ѿ�Ϊ�ô���ռλ��ֻ�ǻ�δ���� hWnd
				// ���ֻ��Ҫ�ҵ������б��е����һ���
				int indexReal = (int)g_vecWindows.size() - 1;
				assert(indexReal >= 0);
				OnCreate(indexReal, hWnd, lParam);
				WNDPROC proc = g_vecWindows[indexReal].funcWndProc;
				if (proc)
				{
					proc(hWnd, msg, wParam, lParam);
				}
			}

			// ����������������Ĵ���ȴ�����˴˹��̺�������ʹ��Ĭ�Ϸ������д���
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		// hWnd �ڴ����б���
		else
		{
			//////////// Begin of HiEasyX MsgLoopHX Process ////////////

			switch (msg)
			{
				//	// ˢ�� Canvas ���浽����
				//case WM_USER_REDRAW:
				//{
				//	HDC hdc = GetDC(hWnd);
				//	OnPaint(indexWnd, hdc);
				//	ReleaseDC(hWnd, hdc);
				//	return 0;
				//	break;
				//}

			case WM_MOVE:
				retVal = OnMove(hWnd);;
				break;

				// WM_CLOSE ���û�����رմ��ڣ��������ͬ�⣬����� DestroyWindow() ���ٴ��ڣ�
				// ���ʹ Windows �ͷŴ�����Դ�������Ӵ��ڣ��Լ�������� WM_DESTROY ��Ϣ��
				// 
				// HiEasyX ���������Ϣ���ȴ��û�����
				// ע��DefWindowProc �����Դ���Ϣ�Ĵ����ǵ��� DestroyWindow()��
			case WM_CLOSE:
				break;

				// WM_DESTROY ��Ϣ��ζ��ϵͳ�Ѿ������ٴ��ڣ���Ҫ�����ͷ��Լ����ڴ档
				// ע��WM_DESTROY ������ζ�ų����˳��������ں�̨�������У������ǵ��� PostQuitMessage()��
			case WM_DESTROY:
				retVal = OnDestroy(indexWnd, wParam);
				break;

			case WM_SIZE:
				retVal = OnSize(indexWnd, wParam, lParam);
				break;

				// ���ô��ڵ������С��Χ
			case WM_GETMINMAXINFO:
				retVal = OnGetMinMaxInfo(indexWnd, lParam);
				break;

			case WM_VSCROLL:
			case WM_HSCROLL:
				retVal = OnScroll(indexWnd, msg, wParam, lParam);
				break;

				// ������Ϣ
			case WM_TRAY:
				retVal = OnTray(indexWnd, lParam);
				break;

			default:
				// ϵͳ���������´�������ʱ������Ҫ���´�������
				if (msg == g_uWM_TASKBARCREATED)
				{
					retVal = OnTaskBarCreated(indexWnd);
				}
				break;
			}

			// �Ǽ� ExMessage ��Ϣ
			retVal = RegisterExMessage(indexWnd, msg, wParam, lParam);

			// ����ϵͳ�ؼ���Ϣ
			Optional<LRESULT> lrSysCtrl = SysCtrlProc(indexWnd, msg, wParam, lParam);
			if (lrSysCtrl.isSet)
			{
				return lrSysCtrl;
			}

			//////////// End of HiEasyX MsgLoopHX Process ////////////

			// �����û���Ϣ������
			if (g_vecWindows[indexWnd].funcWndProc)
			{
				retVal = g_vecWindows[indexWnd].funcWndProc(hWnd, msg, wParam, lParam);
			}

			// ���Ϲ��̶�û�д������Ϣ����ʹ��Ĭ�Ϸ�������
			// ע��WM_PAINT ���뵥���������ܵ��� DefWindowProc
			if (retVal.isSet == false && msg != WM_PAINT)
			{
				retVal = DefWindowProc(hWnd, msg, wParam, lParam);
			}

			// �ر�� WM_PAINT ��Ϣ���к���
			// �涨�û��� WM_PAINT �в�Ҫ���� BeginPaint() �� EndPaint() ���л��ƣ�����ֱ��ͨ�� EasyX ��ʽ��ͼ������ Canvas
			// �������������Ҫ������������ݵ����� HDC ��
			//
			// ��ԻΪʲô�������û��� WM_PAINT ��ʹ�� Win32 ������ͼ��Ȩ����
			// �������Ļ��Ҿ�û��������������壨�Է��������� Win32 �������Ƶ����ݣ������Ȩ��֮�£�����������Ȩ����
			if (msg == WM_PAINT)
			{
				HDC			hdc;
				PAINTSTRUCT	ps;
				hdc = BeginPaint(hWnd, &ps);
				OnPaint(indexWnd, hdc);
				EndPaint(hWnd, &ps);

				// ����
				//DefWindowProc(hWnd, WM_PAINT, 0, 0);

				//printf("\twm_paint\n");

				retVal = 0;
			}

			return retVal;
		}
	}

	static void RegisterWndClass()
	{
		HICON hIcon = g_hIconDefault;
		HICON hIconSm = g_hIconDefault;
		if (g_lpszCustomIcon)
			hIcon = g_hCustomIcon;
		if (g_lpszCustomIconSm)
			hIconSm = g_hCustomIconSm;

		g_WndClassEx.cbSize = sizeof(WNDCLASSEX);
		g_WndClassEx.style = CS_VREDRAW | CS_HREDRAW;
		g_WndClassEx.lpfnWndProc = WndProc;
		g_WndClassEx.cbClsExtra = 0;
		g_WndClassEx.cbWndExtra = 0;
		g_WndClassEx.hInstance = g_hInstance;
		g_WndClassEx.hIcon = hIcon;
		g_WndClassEx.hIconSm = hIconSm;
		g_WndClassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
		g_WndClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		g_WndClassEx.lpszMenuName = nullptr;
		g_WndClassEx.lpszClassName = g_lpszClassName;

		// ע�ᴰ����
		if (!RegisterClassEx(&g_WndClassEx))
		{
			HXString str = ToHXString(GetLastError());
			MessageBox(nullptr, (HXStr("Error registing window class: ") + str).c_str(), HXStr("[Error]"), MB_OK | MB_ICONERROR);
			exit(-1);
		}
	}

	// ��ʼ�����ڽṹ�壨���Ϊ�գ�
	static EasyWindow& InitWindowStruct(EasyWindow& wnd, HWND hParent, int w, int h, WNDPROC WindowProcess)
	{
		wnd.isWindowAlive = true;
		wnd.hWnd = nullptr;
		wnd.hParent = hParent;
		wnd.pCanvas = new Canvas(w, h);
		wnd.xasp = 1;
		wnd.yasp = 1;
		wnd.bAutoResizeCanvas = true;
		memset(wnd.bStylesUsage, 0, sizeof wnd.bStylesUsage);
		wnd.funcWndProc = WindowProcess;
		wnd.vecMessage.reserve(MSG_RESERVE_SIZE);
		wnd.isUseTray = false;
		wnd.nid = { 0 };
		wnd.isUseTrayMenu = false;
		wnd.hTrayMenu = nullptr;
		wnd.funcTrayMenuProc = nullptr;
		wnd.isNewSize = false;
		//wnd.isBusyProcessing = false;
		//wnd.nSkipPixels = 0;
		wnd.vecSysCtrl.reserve(SYSCTRL_RESERVE_SIZE);
		return wnd;
	}

	/*void InitRenderStartScene(HWND hWnd, int w, int h, int nPreCmdShow, bool& nStartAnimation)
	{
		RenderStartScene(hWnd, w, h, nPreCmdShow);
		nStartAnimation = true;
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}*/

	// �ڲ����ڴ������ڵĺ���
	// ʧ��ʱ���� nullptr
	static HWND CreateWindowInternal(int w, int h, int flag, LPCTSTR lpszWndTitle, WNDPROC WindowProcess, HWND hParent)
	{
		static int nWndCount = 0;	// �Ѵ������ڼ������������ɴ��ڱ��⣩

		HXString strTitle;		// ���ڱ���
		EasyWindow wnd;				// ������Ϣ
		int nFrameW, nFrameH;		// ���ڱ�������ߣ��������ڿ��ܲ�ͬ��
		int nIndexWnd = nWndCount;	// ��¼������ڵ� id

		// ���ܶ������ͬʱ�ڴ�����Ϊ�˷�ֹԤ�贰�����Խ��棬�ȱ������ݣ��ó�ȫ�ֱ���
		bool isPreStyle = g_isPreStyle;
		bool isPreStyleEx = g_isPreStyleEx;
		bool isPrePos = g_isPrePos;
		bool isPreShowState = g_isPreShowState;
		long lPreStyle = g_lPreStyle;
		long lPreStyleEx = g_lPreStyleEx;
		POINT pPrePos = g_pPrePos;
		int nPreCmdShow = g_nPreCmdShow;

		//bool start_animation = false;

		g_isPreStyle = false;
		g_isPreStyleEx = false;
		g_isPrePos = false;
		g_isPreShowState = false;

		// δ���ñ���
		if (lstrlen(lpszWndTitle) == 0)
		{
			strTitle = HXStr("EasyX_") + (HXString)GetEasyXVer() + HXStr(" HiEasyX (") + _HIEASYX_VER_STR_ + HXStr(")");
			//if (nWndCount != 0)
			//{
			//	strTitle += HXStr(" ( WindowID: ") + ToHXString(nWndCount) + HXStr(" )");
			//}
		}
		else
		{
			strTitle = lpszWndTitle;
		}

		// ��һ�δ������� --- ��ʼ����������
		if (nWndCount == 0)
		{
			// ��ȡ�ֱ���
			g_screenSize = GetScreenSize();

			// Ĭ�ϳ���ͼ��
			g_hIconDefault = GetDefaultAppIcon();

			// ע�ᴰ����
			RegisterWndClass();
			//g_hConsole = GetConsoleWindow();

			//// ���ؿ���̨
			//if (g_hConsole)
			//{
			//	ShowWindow(g_hConsole, SW_HIDE);
			//}

			// ��ȡϵͳ�������Զ������Ϣ����
			g_uWM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));

			//#ifndef _DEBUG
			//#ifndef __DEBUG__
			//#ifndef DEBUG
			//#ifndef _NO_START_ANIMATION_
			//
			//			if (!(isPreShowState && nPreCmdShow == SW_HIDE) && w >= 640 && h >= 480)
			//				start_animation = true;
			//
			//#endif
			//#endif
			//#endif
			//#endif
		}

		// ������ڲ������κδ���
		if (!IsAnyWindow())
		{
			// ��ʼ�� GDI+ ��ͼ����
			Gdiplus_Try_Starup();
		}

		// ����̨
		if (flag & EW_SHOWCONSOLE)
		{
			init_console();
		}
		else
		{
			hide_console();
		}

		// �û��ڴ�������ʱ���õĴ�������
		long user_style = WS_OVERLAPPEDWINDOW;
		if (flag & EW_NOMINIMIZE)	// �޳���С����ť
		{
			user_style &= ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX;
		}
		// �˷������У������洦�������
		/*if (flag & EW_NOCLOSE)
		{
			user_style &= ~WS_SYSMENU;
		}*/
		if (flag & EW_DBLCLKS)		// ֧��˫��
		{
			user_style |= CS_DBLCLKS;
		}

		// CreateWindow �����ڲ���ֱ�ӷ� WM_CREATE �� WndProc������ʱ CreateWindow ��û�з��ش��ھ��
		// ��ˣ��˴��ڴ�������ǰ�����ڼ���������Ԥ����Ϊ�գ�������̺������� WM_CREATE ��Ϣ
		InitWindowStruct(wnd, hParent, w, h, WindowProcess);
		g_vecWindows.push_back(wnd);

		// ��������
		for (int i = 0;; i++)
		{
			// ����ȷ��ʹ�õĴ�����ʽ
			long final_style = user_style;
			if (isPreStyle)
				final_style = lPreStyle;
			final_style |= WS_CLIPCHILDREN;	// ����������ʽ

			// ����ȷ��ʹ�õĴ�����չ��ʽ
			long final_style_ex = WS_EX_WINDOWEDGE;
			if (isPreStyleEx)
				final_style_ex = lPreStyleEx;

			wnd.hWnd = CreateWindowEx(
				final_style_ex,
				g_lpszClassName,
				strTitle.c_str(),
				final_style,
				CW_USEDEFAULT, CW_USEDEFAULT,
				w, h,	// ��������������ã��Ժ��ȡ�߿��С���ٵ���
				hParent,
				nullptr,
				g_hInstance,
				nullptr
			);

			if (wnd.hWnd)
			{
				// �������ڳɹ������´����б��м�¼�� hWnd
				g_vecWindows.back().hWnd = wnd.hWnd;
				break;
			}

			// ���δ�������ʧ�ܣ����ٳ���
			else if (i == 2)
			{
				HXString str = ToHXString(GetLastError());
				MessageBox(nullptr, (HXStr("Error creating window: ") + str).c_str(), HXStr("[Error]"), MB_OK | MB_ICONERROR);
				return nullptr;
			}
		}

		// �޳��رհ�ť
		if (flag & EW_NOCLOSE)
		{
			HMENU hmenu = GetSystemMenu(wnd.hWnd, false);
			RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
		}

		// ���ڴ������
		nWndCount++;

		// ������
		//// ע�⣺
		////	��������ʾ����ǰ����Ѿ���ɴ������ڡ�
		////	��Ϊ�������Զ�����̺����д����Ӵ��ڣ����ǲ�����ʾ����ǰ��Ǵ��ڴ�����ɣ�
		////	�ͻᵼ�¸����ڹ��̺�����������������ʾ���ھͻ����������������������ڼ�����
		//*nDoneFlag = 1;
		//if (!start_animation) *nStartAnimation = true;

		//** ��ʾ���ڵȺ������� **//

		// ��ȡ�߿��С�������ͼ����С
		RECT rcClient, rcWnd;
		GetClientRect(wnd.hWnd, &rcClient);
		GetWindowRect(wnd.hWnd, &rcWnd);
		nFrameW = (rcWnd.right - rcWnd.left) - rcClient.right;
		nFrameH = (rcWnd.bottom - rcWnd.top) - rcClient.bottom;

		int px = 0, py = 0;
		if (isPrePos)
		{
			px = pPrePos.x;
			py = pPrePos.y;
		}
		SetWindowPos(
			wnd.hWnd,
			HWND_TOP,
			px, py,
			w + nFrameW, h + nFrameH,
			isPrePos ? 0 : SWP_NOMOVE
		);

		/*if (!start_animation)
		{
			ShowWindow(wnd.hWnd, isPreShowState ? nPreCmdShow : SW_SHOWNORMAL);
			UpdateWindow(wnd.hWnd);
		}*/
		ShowWindow(wnd.hWnd, isPreShowState ? nPreCmdShow : SW_SHOWNORMAL);

		// һ�㴴���괰�ڻ����� Update һ�£������ᷢ��һ�� WM_PAINT ��Ϣ���������ػ�
		UpdateWindow(wnd.hWnd);

		// ������
		//// ����ģʽ����Ⱦ��������
		//if (start_animation == true)
		//{
		//	// ��Ⱦ��������
		//	std::thread([&]() {
		//		InitRenderStartScene(wnd.hWnd, w, h, isPreShowState ? nPreCmdShow : SW_SHOWNORMAL, *nStartAnimation);
		//		}).detach();
		//}

		return wnd.hWnd;
	}

	HWND initgraphHX(int w, int h, int flag, LPCTSTR lpszWndTitle, WNDPROC WindowProcess, HWND hParent)
	{
		HWND hWnd = nullptr;

		// �����ã�������Ȼ���� Win32 ��������ʱ����ָ���ĸ����ڣ����ǲ��ٶ����ṩ��ģ̬���ڡ�ʵ��
		//// ���ڸ�����ʱ��ʵ��ģ̬����
		//if (hParent)
		//{
		//	// ���ø����ڣ��ô��ڱ����ٺ󣬸����ڽ���ָ�������
		//	EnableWindow(hParent, false);
		//}

		hWnd = CreateWindowInternal(w, h, flag, lpszWndTitle, WindowProcess, hParent);

		// ������ģʽ
		// ������ CreateWindowInternal ֮ǰֱ�� closeallgraph�������ᴥ�� AutoExit��������õĻ���
		if (g_bSingleWindow)
		{
			for (const auto& wnd : g_vecWindows)
			{
				if (wnd.isWindowAlive && wnd.hWnd != hWnd)
				{
					DestroyWindow(wnd.hWnd);
				}
			}
		}

		// �µĴ��ڻ��Զ���ù�������
		SetWorkingWindow(hWnd);

		//if (!hWnd)
		//{
		//	if (hParent)						// �����Ӵ���ʧ�ܣ���ʹ�����ڻָ�����
		//	{
		//		EnableWindow(hParent, true);
		//	}
		//	return nullptr;
		//}
		//if (hWnd)
		//{
		//	//while (nStartAnimation == false)	Sleep(50);		// �ȴ���ʼ�������
		//	
		//	// Ԥ�豳��ɫ
		//	if (SetWorkingWindow(hWnd) && BeginTask())
		//	{
		//		setbkcolor(CLASSICGRAY);
		//		settextcolor(BLACK);
		//		setlinecolor(BLACK);
		//		setfillcolor(BLACK);
		//		cleardevice();
		//		EndTask();
		//		RedrawWindow();
		//	}
		//}

		return hWnd;
	}

	HWND initgraphCompatible(int w, int h, int flag)
	{
		// Compatible Settings
		AutoExit(true);
		AutoFlushWindowBuffer(true);
		SingleGraphWindow(true);

		HWND wnd = initgraphHX(w, h, flag);
		SetWindowStyleHX(Resizable, false, wnd);

		return wnd;
	}

	bool init_console()
	{
		if (GetConsoleWindow() == NULL) AllocConsole();
		if (GetConsoleWindow() != NULL)
		{
			ShowWindow(GetConsoleWindow(), SW_SHOW);
			return true;
		}
		return false;
	}

	bool hide_console()
	{
		if (GetConsoleWindow() != NULL)
		{
			ShowWindow(GetConsoleWindow(), SW_HIDE);
			return true;
		}
		return false;
	}

	bool close_console()
	{
		if (GetConsoleWindow() != NULL)
		{
			ShowWindow(GetConsoleWindow(), SW_HIDE);
			FreeConsole();
			return true;
		}
		return false;
	}

	/////////////////////////////////////
	// Window Class Functions
	/////////////////////////////////////

	HiEasyX::Window::Window()
	{
	}

	Window::Window(int w, int h, int flag, LPCTSTR lpszWndTitle, WNDPROC WindowProcess, HWND hParent)
	{
		Create(w, h, flag, lpszWndTitle, WindowProcess, hParent);
	}

	Window::~Window()
	{
	}

	HWND Window::Create(int w, int h, int flag, LPCTSTR lpszWndTitle, WNDPROC WindowProcess, HWND hParent)
	{
		// ����δ���ر�ʱ���޷������´���
		if (IsWindowExists(m_nWindowIndex))
		{
			return nullptr;
		}

		// Ԥ�贰������
		if (m_isPreStyle)		PreSetWindowStyle(m_lPreStyle);
		if (m_isPreStyleEx)		PreSetWindowStyleEx(m_lPreStyleEx);
		if (m_isPrePos)			PreSetWindowPos(m_pPrePos.x, m_pPrePos.y);
		if (m_isPreShowState)	PreSetWindowShowState(m_nPreCmdShow);

		HWND hwnd = initgraphHX(w, h, flag, lpszWndTitle, WindowProcess, hParent);
		int index = GetWindowIndex(hwnd);
		m_nWindowIndex = index;
		//m_isCreated = true;
		return hwnd;
	}

	void Window::Close()
	{
		closegraphHX(g_vecWindows[m_nWindowIndex].hWnd);
	}

	void Window::SetProcFunc(WNDPROC WindowProcess)
	{
		SetWndProcFunc(g_vecWindows[m_nWindowIndex].hWnd, WindowProcess);
	}

	HWND Window::GetHandle() const
	{
		if (!IsWindowExists(m_nWindowIndex))
		{
			return nullptr;
		}
		else
		{
			return g_vecWindows[m_nWindowIndex].hWnd;
		}
	}

	bool Window::Exist()
	{
		return IsWindowExists(m_nWindowIndex);
	}

	/*IMAGE* Window::GetImage()
	{
		return g_vecWindows[m_nWindowIndex].pBufferImg;
	}*/

	Canvas* Window::GetCanvas() const
	{
		return g_vecWindows[m_nWindowIndex].pCanvas;
	}

	void Window::SetOrigin(int x, int y) const
	{
		setoriginHX(x, y, GetHandle());
	}

	void Window::GetOrigin(int* x, int* y) const
	{
		getoriginHX(x, y, GetHandle());
	}

	void Window::SetAspectRatio(float xasp, float yasp) const
	{
		setaspectratioHX(xasp, yasp, GetHandle());
	}

	void Window::GetAspectRatio(float* xasp, float* yasp) const
	{
		getaspectratioHX(xasp, yasp, GetHandle());
	}

	/*void Window::BindCanvas(Canvas* pCanvas)
	{
		BindWindowCanvas(pCanvas, g_vecWindows[m_nWindowIndex].hWnd);
	}*/

	/*void Window::WaitMyTask()
	{
		WaitForTask(g_vecWindows[m_nWindowIndex].hWnd);
	}*/

	bool Window::SetWorkingWindow()
	{
		return HiEasyX::SetWorkingWindow(g_vecWindows[m_nWindowIndex].hWnd);
	}

	/*void Window::FlushDrawing(RECT rct)
	{
		if (IsInTask())
		{
			HiEasyX::FlushDrawing(rct);
		}
	}*/

	/*bool Window::BeginTask()
	{
		if (SetWorkingWindow())
		{
			return HiEasyX::BeginTask();
		}
		else
		{
			return false;
		}
	}

	void Window::EndTask(bool flush)
	{
		HiEasyX::EndTask(flush);
	}

	bool Window::IsInTask()
	{
		return HiEasyX::IsInTask(g_vecWindows[m_nWindowIndex].hWnd);
	}*/

	bool Window::IsSizeChanged()
	{
		return IsWindowSizeChanged(g_vecWindows[m_nWindowIndex].hWnd);
	}

	void Window::SetSizeLim(Optional<SIZE> sizeMin, Optional<SIZE> sizeMax)
	{
		SetWindowSizeLim(sizeMin, sizeMax, *this);
	}

	void Window::AutoResizeCanvas(bool enable)
	{
		AutoResizeWindowCanvas(enable, *this);
	}

	void Window::CreateTray(LPCTSTR lpszTrayName)
	{
		HiEasyX::CreateTray(lpszTrayName, g_vecWindows[m_nWindowIndex].hWnd);
	}

	void Window::DeleteTray()
	{
		HiEasyX::DeleteTray(g_vecWindows[m_nWindowIndex].hWnd);
	}

	void Window::SetTrayMenu(HMENU hMenu)
	{
		HiEasyX::SetTrayMenu(hMenu, g_vecWindows[m_nWindowIndex].hWnd);
	}

	void Window::SetTrayMenuProcFunc(void(*pFunc)(UINT))
	{
		HiEasyX::SetTrayMenuProcFunc(pFunc, g_vecWindows[m_nWindowIndex].hWnd);
	}

	void Window::PreSetStyle(long lStyle)
	{
		m_isPreStyle = true;
		m_lPreStyle = lStyle;
	}

	void Window::PreSetStyleEx(long lStyleEx)
	{
		m_isPreStyleEx = true;
		m_lPreStyleEx = lStyleEx;
	}

	void Window::PreSetPos(int x, int y)
	{
		m_isPrePos = true;
		m_pPrePos = { x,y };
	}

	void Window::PreSetShowState(int nCmdShow)
	{
		m_isPreShowState = true;
		m_nPreCmdShow = nCmdShow;
	}

	void Window::FlushBuffer()
	{
		FlushWindowBuffer(g_vecWindows[m_nWindowIndex].hWnd);
	}

	long Window::GetStyle()
	{
		return GetWindowStyle(g_vecWindows[m_nWindowIndex].hWnd);
	}

	int Window::SetStyle(long lNewStyle)
	{
		return SetWindowStyle(lNewStyle, g_vecWindows[m_nWindowIndex].hWnd);
	}

	long Window::GetExStyle()
	{
		return GetWindowExStyle(g_vecWindows[m_nWindowIndex].hWnd);
	}

	int Window::SetExStyle(long lNewExStyle)
	{
		return SetWindowExStyle(lNewExStyle, g_vecWindows[m_nWindowIndex].hWnd);
	}

	void Window::SetStyleHX(WindowStyle style, bool bEnable)
	{
		SetWindowStyleHX(style, bEnable, *this);
	}

	bool Window::HasStyleHX(WindowStyle style)
	{
		return HasWindowStyleHX(style, *this);
	}

	void Window::UpdateScrollInfo()
	{
		HiEasyX::UpdateScrollInfo(*this);
	}

	POINT Window::GetPos()
	{
		return GetWindowPos(g_vecWindows[m_nWindowIndex].hWnd);
	}

	SIZE Window::GetWindowSize()
	{
		return HiEasyX::GetWindowSize(g_vecWindows[m_nWindowIndex].hWnd);
	}

	void Window::Move(int x, int y)
	{
		MoveWindow(x, y, g_vecWindows[m_nWindowIndex].hWnd);
	}

	int Window::GetWindowWidth()
	{
		return GetWindowSize().cx;
	}

	int Window::GetWindowHeight()
	{
		return  GetWindowSize().cy;
	}

	void Window::MoveRel(int dx, int dy)
	{
		MoveWindowRel(dx, dy, g_vecWindows[m_nWindowIndex].hWnd);
	}

	void Window::Resize(int w, int h)
	{
		ResizeWindow(w, h, g_vecWindows[m_nWindowIndex].hWnd);
	}

	void Window::SetTransparent(bool enable, int alpha)
	{
		// ���Խ��ۣ��˺��������� UpdateLayeredWindow һͬʹ��

		LONG nRet = ::GetWindowLong(g_vecWindows[m_nWindowIndex].hWnd, GWL_EXSTYLE);
		nRet |= WS_EX_LAYERED;
		::SetWindowLong(g_vecWindows[m_nWindowIndex].hWnd, GWL_EXSTYLE, nRet);

		if (!enable) alpha = 0xFF;
		SetLayeredWindowAttributes(g_vecWindows[m_nWindowIndex].hWnd, 0, alpha, LWA_ALPHA);
	}

	void Window::SetTitle(LPCTSTR lpszTitle)
	{
		SetWindowTitle(lpszTitle, g_vecWindows[m_nWindowIndex].hWnd);
	}

	bool Window::IsForegroundWindow()
	{
		return GetForegroundWindow() == g_vecWindows[m_nWindowIndex].hWnd;
	}

	int Window::GetClientWidth()
	{
		//return g_vecWindows[m_nWindowIndex].pBufferImg->getwidth();
		RECT rctWnd;
		GetClientRect(g_vecWindows[m_nWindowIndex].hWnd, &rctWnd);
		return rctWnd.right;
	}

	int Window::GetClientHeight()
	{
		//return g_vecWindows[m_nWindowIndex].pBufferImg->getheight();
		RECT rctWnd;
		GetClientRect(g_vecWindows[m_nWindowIndex].hWnd, &rctWnd);
		return rctWnd.bottom;
	}

	Optional<ExMessage> Window::GetMsg(BYTE filter)
	{
		return getmessageHX(filter, g_vecWindows[m_nWindowIndex].hWnd);
	}

	bool Window::GetMsg(ExMessage* msg, BYTE filter)
	{
		return getmessageHX(msg, filter, g_vecWindows[m_nWindowIndex].hWnd);
	}

	bool Window::PeekMsg(ExMessage* msg, BYTE filter, bool removemsg)
	{
		return peekmessageHX(msg, filter, removemsg, g_vecWindows[m_nWindowIndex].hWnd);
	}

	void Window::FlushMsg(BYTE filter)
	{
		flushmessageHX(filter, g_vecWindows[m_nWindowIndex].hWnd);
	}
}

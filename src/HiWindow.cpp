#include <HiEasyX/HiWindow.h>

#include <HiEasyX/HiMacro.h>
#include <HiEasyX/HiIcon.h>
#include <HiEasyX/HiGdiplus.h>
#include <HiEasyX/HiSysGUI/SysControlBase.h>

#include <cassert>
#include <stdexcept>

#include <chrono>
using StdClock = std::chrono::high_resolution_clock;

// 预留消息空间
#define MSG_RESERVE_SIZE		100

// 预留控件空间
#define SYSCTRL_RESERVE_SIZE	100


namespace HiEasyX
{
	////////////****** 类型定义 ******////////////

	/**
	 * @brief WindowStyle 对应的详细信息结构体
	 * @sa WindowStyle, GetWindowStyleHX_Internal
	 */
	struct WindowStyleInfo
	{
		WindowStyle style_name;
		bool is_ex;
		long style_code;
	};

	/**
	 * @brief	窗口
	 * @note	在 InitWindowStruct 函数中初始化此结构体
	*/
	struct EasyWindow
	{
		bool isWindowAlive;							///< 窗口是否存在

		HWND hWnd;									///< 窗口句柄
		HWND hParent;								///< 父窗口句柄

		/**
		 * @note <pre>
		 *		需要为窗口保留一个 IMAGE 对象（在此直接使用 Canvas）用于存储绘制内容，原因如下：
		 *		1. 若用户按 EasyX 逻辑编写代码，不处理 WM_PAINT 消息，则窗口需要重绘时，需从此处的 IMAGE 中读取
		 *		2. 若不加上该图像，则只能直接绘制到窗口 HDC，那样需要强制修改 IMAGE 对象的绘制目标，若非不得已，还是罢了
		 * </pre>
		*/
		Canvas* pCanvas;							///< 窗口图像
		float xasp;									///< 刷新窗口缓冲时使用的缩放比例（x 轴）
		float yasp;									///< 刷新窗口缓冲时使用的缩放比例（y 轴）
		bool bAutoResizeCanvas;						///< 是否在窗口拉伸时自动调整窗口缓冲区画布大小（默认是）

		bool bStylesUsage[64];						///< 标志 WindowStyle 预设样式的启用情况

		WNDPROC funcWndProc;						///< 窗口消息处理函数

		std::vector<ExMessage> vecMessage;			///< 模拟 EasyX 窗口消息队列

		bool isUseTray;								///< 是否使用托盘
		NOTIFYICONDATA nid;							///< 托盘信息
		bool isUseTrayMenu;							///< 是否使用托盘菜单
		HMENU hTrayMenu;							///< 托盘菜单

		/**
		 * @brief <pre>
		 *		托盘菜单消息处理函数指针
		 *
		 * 备注：
		 *		给出此函数是为了方便响应托盘的菜单消息
		 *		如需响应完整的托盘消息，请自定义窗口过程函数并处理 WM_TRAY 消息
		 * </pre>
		*/
		void(*funcTrayMenuProc)(UINT);

		bool isNewSize;								///< 窗口大小是否改变
		Optional<SIZE> sizeMin;						///< 最小窗口尺寸
		Optional<SIZE> sizeMax;						///< 最大窗口尺寸

		std::vector<SysControlBase*> vecSysCtrl;	///< 记录创建的系统控件
		bool bHasCtrl = false;						///< 是否创建过系统控件
	};


	////////////****** 全局变量 ******////////////

	static WNDCLASSEX				g_WndClassEx;								///< 窗口类
	static TCHAR					g_lpszClassName[] = _T("HiEasyX");			///< 窗口类名
	static ScreenSize				g_screenSize;								///< 显示器信息
	static HINSTANCE				g_hInstance = GetModuleHandle(0);			///< 程序实例

	static std::vector<EasyWindow>	g_vecWindows;								///< 窗口列表（管理多窗口）（由于一般程序窗口很少，故无需 map）
	static int						g_nFocusWindowIndex = NO_WINDOW_INDEX;		///< 当前操作焦点窗口索引
	static bool						g_bSingleWindow = false;					///< 标记是否在所有窗口被销毁时自动退出程序
	static bool						g_bAutoExit = false;						///< 标记是否在所有窗口被销毁时自动退出程序
	static bool						g_bAutoFlush = false;						///< 标记是否自动刷新所有窗口缓冲
	//static bool					g_isInTask = false;							///< 标记处于任务中

	static HICON					g_hIconDefault;								///< 默认程序图标
	static LPCTSTR					g_lpszCustomIcon = nullptr;					///< 自定义程序图标资源，为空表示不使用
	static LPCTSTR					g_lpszCustomIconSm = nullptr;
	static HICON					g_hCustomIcon;								///< 自定义程序图标
	static HICON					g_hCustomIconSm;

	static bool						g_isPreStyle = false;						///< 是否预设窗口样式
	static bool						g_isPreStyleEx = false;						///< 是否预设窗口扩展样式
	static bool						g_isPrePos = false;							///< 是否预设窗口位置
	static bool						g_isPreShowState = false;					///< 是否预设窗口显示状态（即预设创建窗口后 ShowWindow 的参数）
	static long						g_lPreStyle;								///< 创建窗口前的预设样式
	static long						g_lPreStyleEx;								///< 创建窗口前的预设扩展样式
	static POINT					g_pPrePos;									///< 创建窗口前的预设窗口位置
	static int						g_nPreCmdShow;								///< 创建窗口前的预设显示状态

	//static DrawMode					g_fDrawMode = DM_Normal;				///< 全局绘制模式
	//static bool						g_bAutoFlush = true;					///< 是否自动刷新双缓冲

	static UINT						g_uWM_TASKBARCREATED;						///< 系统任务栏消息代码

	////////////****** 函数定义 ******////////////

	// static 函数为内部函数，外部无法调用
	// 其余函数为对外接口函数

	// 检验窗口索引是否合法（不判断窗口是否被销毁，如需要请使用 IsWindowExists）
	static bool IsValidWindowIndex(int index)
	{
		return index >= 0 && index < (int)g_vecWindows.size();
	}

	// 当前是否存在活动窗口（逻辑上活动窗口都是 Alive 的）
	static bool HasFocusedWindow()
	{
		return IsValidWindowIndex(g_nFocusWindowIndex);
	}

	// 获取当前操作焦点窗口
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

	// 通过句柄获得此窗口在窗口记录表中的索引
	// 传入 nullptr 代表当前活动窗口
	// 未找到返回 NO_WINDOW_INDEX
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

	// 判断索引是否合法，且判断窗口是否存在（未被销毁）
	static bool IsWindowExists(int index)
	{
		return IsValidWindowIndex(index) && g_vecWindows[index].isWindowAlive;
	}

	//// 等待窗口内部消息处理完成
	//void WaitForProcessing(int index)
	//{
	//	// 死窗口可能正在销毁，故不用 isAliveWindow
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
	//	// 未设置句柄时只需要等待，若设置了则需要判断该句柄是否对应活动窗口
	//	if (!hWnd || (HasFocusedWindow() && GetFocusedWindow().hWnd == hWnd))
	//	{
	//		while (g_isInTask)
	//		{
	//			HpSleep(1);
	//		}
	//	}
	//}

	// 释放窗口内存
	static void FreeWindow(int index)
	{
		if (!IsValidWindowIndex(index))
		{
			return;
		}

		// 释放绘图缓冲
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

		// 释放消息列表内存
		std::vector<ExMessage>().swap(g_vecWindows[index].vecMessage);

		//DestroyWindow(g_vecWindows[index].hWnd);
		//PostQuitMessage(0);
	}

	void closegraphHX(HWND hWnd)
	{
		// 已弃用
		//// 必须交由原线程进行 DestroyWindow
		//// 发送 WM_DESTROY 时特殊标记 wParam 为 1，表示程序命令销毁窗口
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
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) // PM_REMOVE: 处理后从消息队列中移除
		{
			if (msg.message == WM_QUIT)	// 必须显式检查并退出
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
		static auto oldclock = StdClock::now();		// 静态变量，记录上一次 tick
		oldclock += std::chrono::milliseconds(ms);	// 更新 tick
		while (StdClock::now() < oldclock)			// 延时
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
		// 此函数原理参见 OnPaint 函数注释

		int index = GetWindowIndex(hwnd);
		if (index == NO_WINDOW_INDEX)
		{
			return;
		}

		// EasyX 原生 setorigin 函数的原理可能不是用的 SetViewportOrgEx，总之经测试无法使用
		//setorigin(x, y);

		// 此处直接采用 GDI 方式实现
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
		// 此函数原理参见 OnPaint 函数注释

		int index = GetWindowIndex(hwnd);
		if (index == NO_WINDOW_INDEX)
		{
			return;
		}

		// EasyX 原生 setaspectratio 函数的原理是
		// 1. 此刻调用时，将 IAMGE 画布大小调整为 w_old/xasp * h_old/yasp
		// 2. 以后 IMAGE::Resize(w, h) 时，实际画布大小被调整为 w/xasp * h/yasp
		// 3. 绘制时坐标实际上没有变化
		// 4. 该函数对任意 IMAGE 均有效
		// 5. EasyX 相应 WM_PAINT 时进行了相应处理，HiEasyX 对应的代码在 OnPaint 函数中
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

		// 标记窗口区域为无效，等待空闲时候刷新
		InvalidateRect(hWnd, nullptr, false); // 第三个参数要设为 false 以不擦除背景，以防 WM_PAINT 闪烁

		// 立即重绘
		if (bInstant)
		{
			//SendMessage(hWnd, WM_USER_REDRAW, 0, 0);
			UpdateWindow(hWnd);							// 检查窗口的更新区域，若不为空则立即发送 WM_PAINT 消息进行重绘
		}
		else
		{
			//PostMessage(hWnd, WM_USER_REDRAW, 0, 0);
		}

		// 临时代码，已弃用
		//// 该函数会将窗口全部区域标记为无效，当消息队列空闲时，系统会自动发送 WM_PAINT 消息进行重绘
		//// false 表示不重绘背景
		//InvalidateRect(hWnd, 0, false);
		//// 该函数检查窗口的更新区域，若不为空则立即发送 WM_PAINT 消息
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

	// 重新调整窗口画布大小
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

		// 死窗口删除时会调用该函数，所以不判断窗口死活，只需要判断窗口是否存在
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

	// 获取消息容器
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

	// 移除当前消息
	static void RemoveMessage(HWND hWnd)
	{
		if (GetMsgVector(hWnd).size())
		{
			GetMsgVector(hWnd).erase(GetMsgVector(hWnd).begin());
		}
	}

	// 清空消息
	// 支持混合消息类型
	static void ClearMessage(BYTE filter, HWND hWnd)
	{
		for (size_t i = 0; i < GetMsgVector(hWnd).size(); i++)
			if (filter & GetExMessageType(GetMsgVector(hWnd)[i]))
				GetMsgVector(hWnd).erase(GetMsgVector(hWnd).begin() + i--);
	}

	// 是否有新消息
	// 支持混合消息类型
	static bool IsNewMessage(BYTE filter, HWND hWnd)
	{
		for (auto& element : GetMsgVector(hWnd))
			if (filter & GetExMessageType(element))
				return true;
		return false;
	}

	// 清除消息，直至获取到符合类型的消息
	// 支持混合消息类型
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

	// 对外阻塞获取消息基函数，其他形式的阻塞消息函数会调用此函数
	// MsgLoopHX() 只在各个对外消息基函数中进行
	Optional<ExMessage> getmessageHX(BYTE filter, HWND hWnd)
	{
		MsgLoopHX();
		while (!IsNewMessage(filter, hWnd))
		{
			if (!IsWindowExists(hWnd))	// 等待过程中要留意窗口是否还存在
			{
				return {};
			}
			SleepHX(10);	// 内含消息循环
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

	// 对外非阻塞获取消息基函数，其他形式的非阻塞消息函数会调用此函数
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

	// 对外清除消息基函数，其他形式的清理消息函数会调用此函数
	void flushmessageHX(BYTE filter, HWND hWnd)
	{
		MsgLoopHX();
		ClearMessage(filter, hWnd);
	}

	// MouseMsg 特有的对外判断新消息基函数
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
	 * @brief 获取 WindowStyle 预设样式的详细信息
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

	// 内部设置窗口样式的函数（负责完成和 Win32 API 交互的部分）
	// hwnd			目标窗口
	// state		启用还是禁用
	// isExStyle	是否为 Ex 属性
	// styleCode	属性代码
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

		// 滚动条特殊配置
		if (style == VScroll || style == HScroll)
		{
			// 滚动条的开启和关闭需要使用 SWP_FRAMECHANGED 刷新
			SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

			// 更新滚动条配置
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
		// 使用 Win32 的方法进行判定会出现未知问题，即似乎在窗口的 Canvas IMAGE::Resize 前后，
		// GetWindowStyle 的返回值并不相同，导致判定出错。目前原因不明。

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

		// 目前只好手动存储一下 Style 的启用情况
		return g_vecWindows[index].bStylesUsage[style];
	}

	void UpdateScrollInfo(HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (!IsValidWindowIndex(index))
		{
			return;
		}

		// 客户区大小
		RECT rctWnd;
		GetClientRect(g_vecWindows[index].hWnd, &rctWnd);	// 必有 LT(0, 0)

		// 可以滚动的范围大小
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

		// y 方向
		if (HasWindowStyleHX(VScroll, g_vecWindows[index].hWnd))
		{
			// 存储旧的 ScrollInfo
			GetScrollInfo(g_vecWindows[index].hWnd, SB_VERT, &si);
			int oldPageSize = si.nPage;
			int oldPos = si.nPos;

			// 设置新的 ScrollInfo
			si.nMin = 0;
			si.nMax = max_y;
			si.nPage = rctWnd.bottom;	// 一页的尺寸，该值主要是保留下来自己在 OnScroll 时用的
			SetScrollInfo(g_vecWindows[index].hWnd, SB_VERT, &si, true);

			// 根据 nPage 的改变相应地滚动窗口（这是对控件的滚动，而非对绘图内容的）
			// 窗口控件滚动触发条件：
			// 1. 在原先的视图中已经能看到页面底部（oldPos + oldPageSize >= max_y）
			// 2. 窗口高度继续增加，即 PageSize 增量（si.nPage - oldPageSize）大于零
			// 那么，窗口中的控件就要跟随下滚，下滚距离等于的窗口的高度增量
			int scroll_amount = si.nPage - oldPageSize;
			if (scroll_amount > 0 && oldPos + oldPageSize >= max_y)
			{
				// 注：禁用控件滚动，因为有很多其它情况不好处理，遂不做支持
				//ScrollWindow(g_vecWindows[index].hWnd, 0, scroll_amount, NULL, NULL);

				// 无用，无需添加
				//UpdateWindow(g_vecWindows[index].hWnd);
			}
		}
		// x 方向
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
		//测试结论：此函数不能与 UpdateLayeredWindow 一同使用

		LONG nRet = ::GetWindowLong(HWnd, GWL_EXSTYLE);
		nRet |= WS_EX_LAYERED;
		::SetWindowLong(HWnd, GWL_EXSTYLE, nRet);

		if (!enable) alpha = 0xFF;
		SetLayeredWindowAttributes(HWnd, 0, alpha, LWA_ALPHA);
	}

	// 获取默认窗口图标
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

		// 自动调整缓冲区画布大小
		if (g_vecWindows[indexWnd].bAutoResizeCanvas)
		{
			ResizeWindowImage(indexWnd, rctWnd);
		}

		// 处理滚动条
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

		// 计算标题栏和边框尺寸
		int nWidthOverhead = rectWindow.right - rectWindow.left - rectClient.right + rectClient.left;
		int nHeightOverhead = rectWindow.bottom - rectWindow.top - rectClient.bottom + rectClient.top;

		// 设置最小跟踪尺寸
		if (g_vecWindows[indexWnd].sizeMin.isSet)
		{
			lpMMI->ptMinTrackSize.x = g_vecWindows[indexWnd].sizeMin.value.cx + nWidthOverhead;
			lpMMI->ptMinTrackSize.y = g_vecWindows[indexWnd].sizeMin.value.cy + nHeightOverhead;
		}
		// 设置最大跟踪尺寸
		if (g_vecWindows[indexWnd].sizeMax.isSet)
		{
			lpMMI->ptMaxTrackSize.x = g_vecWindows[indexWnd].sizeMax.value.cx + nWidthOverhead;
			lpMMI->ptMaxTrackSize.y = g_vecWindows[indexWnd].sizeMax.value.cy + nHeightOverhead;
		}

		return 0;
	}

	/**
	 * @brief 处理 WM_VSCROLL 和 WM_HSCROLL 消息
	 * @note 仅用于处理用户拖拽滚动条等事件，不用于处理窗口拉伸导致的滚动条更新（放在 OnSize 中完成）
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
				// 注：这里的 PAGEUP, PAGEDOWN 在用户点击滚动条滑块上、下方的空白位置时触发
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
				// 该语句用于实现窗口上的控件随滚动条移动
				// 现已禁用
				//ScrollWindow(hwnd, 0, yPos_old - si.nPos, NULL, NULL);
				//UpdateWindow(hwnd);

				// 立即重绘（这样就可以边拖动边看到拖动效果）
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
			return {};	// 未处理
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
				// 左键激活窗口
			case WM_LBUTTONDOWN:
				SetForegroundWindow(hWnd);
				break;

				// 右键打开菜单
			case WM_RBUTTONDOWN:
				if (g_vecWindows[indexWnd].isUseTrayMenu)
				{
					SetForegroundWindow(hWnd);	// 激活一下窗口，防止菜单不消失

					// 显示菜单并跟踪
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
			return {};	// 未处理此消息
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
			return {}; // 未处理
		}
	}

	// 登记 ExMessage 消息
	static Optional<LRESULT> RegisterExMessage(int indexWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// 记录消息事件
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

			// 有滚轮消息时，得到的坐标是屏幕坐标，需要转换
			if (msgMouse.wheel)
			{
				POINT p = { msgMouse.x ,msgMouse.y };
				ScreenToClient(g_vecWindows[indexWnd].hWnd, &p);
				msgMouse.x = (short)p.x;
				msgMouse.y = (short)p.y;
			}

			// 窗口具有滚动条时，获取的坐标需要转换
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

			// 功能键：不区分左右
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

			// 给控制台发一份，以支持 _getch() 系列函数
			// 但是如果用户真的用了 _getch()，则会导致窗口消息阻塞，窗口卡死……故不建议用
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

			// 通知控制台
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

		// 未处理
		default:
		{
			return {};
		}

		}
	}

	// 绘制用户内容
	static void OnPaint(int indexWnd, HDC hdc)
	{
		const EasyWindow& wnd_info = g_vecWindows[indexWnd];

		// 将绘图内容输出到窗口 HDC
		RECT rctWnd;
		GetClientRect(wnd_info.hWnd, &rctWnd);
		//CopyImageToHDC(wnd_info.pCanvas, hdc, rctWnd);

		// 获取窗口画布的绘制原点，以呼应 setoriginHX
		HDC hdcCanvas = GetImageHDCHX(wnd_info.pCanvas);
		POINT ptCanvasOrg;
		GetViewportOrgEx(hdcCanvas, &ptCanvasOrg);

		// 如果开启了滚动条，则根据滚动条位置再修改绘制原点
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS; // 只获取位置信息
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
					hdc,	/* 目标设备 */
					rctWnd.left, rctWnd.top, rctWnd.right, rctWnd.bottom, /* 目标绘制区域 */
					hdcCanvas, /* 源设备 */
					(int)(-ptCanvasOrg.x / wnd_info.xasp), /* 源拷贝起点 */
					(int)(-ptCanvasOrg.y / wnd_info.yasp),
					(int)((rctWnd.right - rctWnd.left) / wnd_info.xasp),
					(int)((rctWnd.bottom - rctWnd.top) / wnd_info.yasp),
					SRCCOPY
				);
			}
			/**
			 * 注解：
			 *		EasyX setaspectratio 的原理是将画布大小调整为 w_old / xasp 和 h_old / yasp，
			 *		然后在画布上绘制时，不做任何处理，仅仅按原样绘制，最后在窗口绘制时，
			 *		将 (w_old / xasp, h_old / yasp) 大小的画布重新压缩回 (w_old, h_old) 大小，
			 *		这样最后看起来的效果就是 x 方向的绘制内容都乘以了 xasp 比例，y 方向的绘制内容都乘以了 yasp 比例。
			 *
			 *		EasyX 原生的 setaspectratio 函数对所有 IMAGE 均有效，效果即是调整画布大小为已设置的 (1/xasp, 1/yasp) 倍。
			 *		这样可以让绘制内容不越界。例如，xasp = 0.5 时，用户可以在更广阔的坐标上绘制内容，所以画布需要调大。
			*/
		}
		else
		{
			BitBlt(hdc,	/* 目标设备 */
				rctWnd.left, rctWnd.top, rctWnd.right, rctWnd.bottom, /* 目标绘制区域 */
				hdcCanvas, /* 源设备 */
				-ptCanvasOrg.x, -ptCanvasOrg.y, /* 源拷贝起点 */
				SRCCOPY);
			/**
			 * 注解：
			 *		在 setoriginHX 中调用了 SetViewportOrgEx 函数设置画布的绘制原点，
			 *		此后画布大小没变，但是传入的绘制坐标都会基于这个原点进行偏移，
			 *		此处 BitBlt 在指定源绘制起点时，坐标也会受到此偏移的影响，
			 *		因此要获取源画布的原点坐标，然后乘以 -1 再作为参数传入。
			*/
		}
	}

	static Optional<LRESULT> OnMove(HWND hWnd)
	{
		//RECT rctWnd;
		//GetWindowRect(hWnd, &rctWnd);

		//// 移动窗口超出屏幕时可能导致子窗口显示有问题，所以此时需要彻底重绘
		//// 如果用户代码一直在强制重绘，则此操作多余。
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
			return {};	// 未处理
		}
		// 已弃用
		//// 先设置窗口死亡，再标识忙碌，等待任务结束
		//g_vecWindows[index].isWindowAlive = false;
		//g_vecWindows[index].isBusyProcessing = true;
		//WaitForTask(g_vecWindows[index].hWnd);

		g_vecWindows[indexWnd].isWindowAlive = false;

		// 已弃用
		//// 若已设置父窗口为模态窗口，则需要将父窗口恢复正常
		//if (g_vecWindows[index].hParent != nullptr)
		//{
		//	EnableWindow(g_vecWindows[index].hParent, true);
		//	SetForegroundWindow(g_vecWindows[index].hParent);
		//}

		// 卸载托盘
		DeleteTray(g_vecWindows[indexWnd].hWnd);

		// 如果活动窗口被销毁，则需要重置活动窗口索引
		if (indexWnd == g_nFocusWindowIndex)
		{
			// 约定：窗口被销毁后置空活动窗口
			// 不置空，可能导致用户代码出错，对窗口的操作发生混乱
			// HiWindow 内部代码也基于此编写
			g_nFocusWindowIndex = NO_WINDOW_INDEX;
		}

		// 释放窗口内存
		FreeWindow(indexWnd);

		//// 关闭忙碌标识
		//g_vecWindows[index].isBusyProcessing = false;

		// 如果关闭此窗口后不存在任何窗口
		if (!IsAnyWindow())
		{
			// 关闭 GDI+ 绘图环境
			Gdiplus_Shutdown();

			// 如果需要自动退出程序
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

		// 记录
		g_vecWindows[indexWnd].vecSysCtrl.push_back((SysControlBase*)wParam);
		return hWnd;
	}

	// 处理系统控件消息
	// bRet 传出，标记是否直接返回
	static Optional<LRESULT> SysCtrlProc(int indexWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			// 创建系统控件
		case WM_SYSCTRL_CREATE:
		{
			g_vecWindows[indexWnd].bHasCtrl = true;
			return (LRESULT)OnSysCtrlCreate(indexWnd, wParam, lParam);
			break;
		}

		// 析构系统控件
		case WM_SYSCTRL_DELETE:
		{
			// 被析构的控件指针标记为空
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

		// 存在控件时，派发消息
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

	// 窗口过程函数
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		int indexWnd = GetWindowIndex(hWnd);		// 窗口索引
		Optional<LRESULT> retVal = 0;			// 记录返回值

		// 调用窗口不在窗口列表内
		if (!IsValidWindowIndex(indexWnd))
		{
			// 如果是 WM)CREATE 消息，则是因为窗口正在创建，其句柄没有存储在窗口列表中，对此特殊处理
			if (msg == WM_CREATE)
			{
				// 对于正在创建的窗口，窗口列表中已经为该窗口占位，只是还未填入 hWnd
				// 因此只需要找到窗口列表中的最后一项即可
				int indexReal = (int)g_vecWindows.size() - 1;
				assert(indexReal >= 0);
				OnCreate(indexReal, hWnd, lParam);
				WNDPROC proc = g_vecWindows[indexReal].funcWndProc;
				if (proc)
				{
					proc(hWnd, msg, wParam, lParam);
				}
			}

			// 其余情况，是外来的窗口却调用了此过程函数，则使用默认方法进行处理
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		// hWnd 在窗口列表内
		else
		{
			//////////// Begin of HiEasyX MsgLoopHX Process ////////////

			switch (msg)
			{
				//	// 刷新 Canvas 缓存到窗口
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

				// WM_CLOSE 是用户请求关闭窗口，如果程序同意，则调用 DestroyWindow() 销毁窗口，
				// 这会使 Windows 释放窗口资源并销毁子窗口，以及向程序发送 WM_DESTROY 消息。
				// 
				// HiEasyX 不处理此消息，等待用户处理。
				// 注：DefWindowProc 函数对此消息的处理是调用 DestroyWindow()。
			case WM_CLOSE:
				break;

				// WM_DESTROY 消息意味着系统已经在销毁窗口，需要程序释放自己的内存。
				// 注：WM_DESTROY 并不意味着程序退出（可以在后台继续运行），除非调用 PostQuitMessage()。
			case WM_DESTROY:
				retVal = OnDestroy(indexWnd, wParam);
				break;

			case WM_SIZE:
				retVal = OnSize(indexWnd, wParam, lParam);
				break;

				// 设置窗口的拉伸大小范围
			case WM_GETMINMAXINFO:
				retVal = OnGetMinMaxInfo(indexWnd, lParam);
				break;

			case WM_VSCROLL:
			case WM_HSCROLL:
				retVal = OnScroll(indexWnd, msg, wParam, lParam);
				break;

				// 托盘消息
			case WM_TRAY:
				retVal = OnTray(indexWnd, lParam);
				break;

			default:
				// 系统任务栏重新创建，此时可能需要重新创建托盘
				if (msg == g_uWM_TASKBARCREATED)
				{
					retVal = OnTaskBarCreated(indexWnd);
				}
				break;
			}

			// 登记 ExMessage 消息
			retVal = RegisterExMessage(indexWnd, msg, wParam, lParam);

			// 处理系统控件消息
			Optional<LRESULT> lrSysCtrl = SysCtrlProc(indexWnd, msg, wParam, lParam);
			if (lrSysCtrl.isSet)
			{
				return lrSysCtrl;
			}

			//////////// End of HiEasyX MsgLoopHX Process ////////////

			// 调用用户消息处理函数
			if (g_vecWindows[indexWnd].funcWndProc)
			{
				retVal = g_vecWindows[indexWnd].funcWndProc(hWnd, msg, wParam, lParam);
			}

			// 以上过程都没有处理此消息，则使用默认方法处理
			// 注：WM_PAINT 必须单独处理，不能调用 DefWindowProc
			if (retVal.isSet == false && msg != WM_PAINT)
			{
				retVal = DefWindowProc(hWnd, msg, wParam, lParam);
			}

			// 特别对 WM_PAINT 消息进行后处理
			// 规定用户在 WM_PAINT 中不要调用 BeginPaint() 和 EndPaint() 进行绘制，而是直接通过 EasyX 方式绘图到窗口 Canvas
			// 这样在这里就需要输出缓冲区内容到窗口 HDC 中
			//
			// 或曰为什么不保留用户在 WM_PAINT 中使用 Win32 方法绘图的权利？
			// 答：那样的话我就没法在这里输出缓冲（以防覆盖他们 Win32 方法绘制的内容），因此权衡之下，剥夺了这项权利。
			if (msg == WM_PAINT)
			{
				HDC			hdc;
				PAINTSTRUCT	ps;
				hdc = BeginPaint(hWnd, &ps);
				OnPaint(indexWnd, hdc);
				EndPaint(hWnd, &ps);

				// 无需
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

		// 注册窗口类
		if (!RegisterClassEx(&g_WndClassEx))
		{
			HXString str = ToHXString(GetLastError());
			MessageBox(nullptr, (HXStr("Error registing window class: ") + str).c_str(), HXStr("[Error]"), MB_OK | MB_ICONERROR);
			exit(-1);
		}
	}

	// 初始化窗口结构体（句柄为空）
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

	// 内部用于创建窗口的函数
	// 失败时返回 nullptr
	static HWND CreateWindowInternal(int w, int h, int flag, LPCTSTR lpszWndTitle, WNDPROC WindowProcess, HWND hParent)
	{
		static int nWndCount = 0;	// 已创建窗口计数（用于生成窗口标题）

		HXString strTitle;		// 窗口标题
		EasyWindow wnd;				// 窗口信息
		int nFrameW, nFrameH;		// 窗口标题栏宽高（各个窗口可能不同）
		int nIndexWnd = nWndCount;	// 记录这个窗口的 id

		// 可能多个窗口同时在创建，为了防止预设窗口属性交叉，先备份数据，让出全局变量
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

		// 未设置标题
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

		// 第一次创建窗口 --- 初始化各项数据
		if (nWndCount == 0)
		{
			// 获取分辨率
			g_screenSize = GetScreenSize();

			// 默认程序图标
			g_hIconDefault = GetDefaultAppIcon();

			// 注册窗口类
			RegisterWndClass();
			//g_hConsole = GetConsoleWindow();

			//// 隐藏控制台
			//if (g_hConsole)
			//{
			//	ShowWindow(g_hConsole, SW_HIDE);
			//}

			// 获取系统任务栏自定义的消息代码
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

		// 如果现在不存在任何窗口
		if (!IsAnyWindow())
		{
			// 初始化 GDI+ 绘图环境
			Gdiplus_Try_Starup();
		}

		// 控制台
		if (flag & EW_SHOWCONSOLE)
		{
			init_console();
		}
		else
		{
			hide_console();
		}

		// 用户在创建窗口时设置的窗口属性
		long user_style = WS_OVERLAPPEDWINDOW;
		if (flag & EW_NOMINIMIZE)	// 剔除最小化按钮
		{
			user_style &= ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX;
		}
		// 此方法不行，在下面处理此属性
		/*if (flag & EW_NOCLOSE)
		{
			user_style &= ~WS_SYSMENU;
		}*/
		if (flag & EW_DBLCLKS)		// 支持双击
		{
			user_style |= CS_DBLCLKS;
		}

		// CreateWindow 函数内部会直接发 WM_CREATE 给 WndProc，而此时 CreateWindow 还没有返回窗口句柄
		// 因此，此处在创建窗口前将窗口加入容器，预设句柄为空，方便过程函数接收 WM_CREATE 消息
		InitWindowStruct(wnd, hParent, w, h, WindowProcess);
		g_vecWindows.push_back(wnd);

		// 创建窗口
		for (int i = 0;; i++)
		{
			// 最终确定使用的窗口样式
			long final_style = user_style;
			if (isPreStyle)
				final_style = lPreStyle;
			final_style |= WS_CLIPCHILDREN;	// 必须加入此样式

			// 最终确定使用的窗口扩展样式
			long final_style_ex = WS_EX_WINDOWEDGE;
			if (isPreStyleEx)
				final_style_ex = lPreStyleEx;

			wnd.hWnd = CreateWindowEx(
				final_style_ex,
				g_lpszClassName,
				strTitle.c_str(),
				final_style,
				CW_USEDEFAULT, CW_USEDEFAULT,
				w, h,	// 宽高现在这样设置，稍后获取边框大小后再调整
				hParent,
				nullptr,
				g_hInstance,
				nullptr
			);

			if (wnd.hWnd)
			{
				// 创建窗口成功，更新窗口列表中记录的 hWnd
				g_vecWindows.back().hWnd = wnd.hWnd;
				break;
			}

			// 三次创建窗口失败，不再尝试
			else if (i == 2)
			{
				HXString str = ToHXString(GetLastError());
				MessageBox(nullptr, (HXStr("Error creating window: ") + str).c_str(), HXStr("[Error]"), MB_OK | MB_ICONERROR);
				return nullptr;
			}
		}

		// 剔除关闭按钮
		if (flag & EW_NOCLOSE)
		{
			HMENU hmenu = GetSystemMenu(wnd.hWnd, false);
			RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
		}

		// 窗口创建完毕
		nWndCount++;

		// 已弃用
		//// 注意：
		////	必须在显示窗口前标记已经完成创建窗口。
		////	因为可以在自定义过程函数中创建子窗口，若是不在显示窗口前标记窗口创建完成，
		////	就会导致父窗口过程函数阻塞，接下来显示窗口就会阻塞，进而导致整个窗口假死。
		//*nDoneFlag = 1;
		//if (!start_animation) *nStartAnimation = true;

		//** 显示窗口等后续处理 **//

		// 获取边框大小，补齐绘图区大小
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

		// 一般创建完窗口会立即 Update 一下，这样会发送一个 WM_PAINT 消息引发立即重绘
		UpdateWindow(wnd.hWnd);

		// 已弃用
		//// 发布模式下渲染开场动画
		//if (start_animation == true)
		//{
		//	// 渲染开场动画
		//	std::thread([&]() {
		//		InitRenderStartScene(wnd.hWnd, w, h, isPreShowState ? nPreCmdShow : SW_SHOWNORMAL, *nStartAnimation);
		//		}).detach();
		//}

		return wnd.hWnd;
	}

	HWND initgraphHX(int w, int h, int flag, LPCTSTR lpszWndTitle, WNDPROC WindowProcess, HWND hParent)
	{
		HWND hWnd = nullptr;

		// 已弃用，我们依然会在 Win32 创建窗口时传入指定的父窗口，但是不再额外提供「模态窗口」实现
		//// 存在父窗口时，实现模态窗口
		//if (hParent)
		//{
		//	// 禁用父窗口（该窗口被销毁后，父窗口将会恢复正常）
		//	EnableWindow(hParent, false);
		//}

		hWnd = CreateWindowInternal(w, h, flag, lpszWndTitle, WindowProcess, hParent);

		// 单窗口模式
		// 不能在 CreateWindowInternal 之前直接 closeallgraph，那样会触发 AutoExit（如果启用的话）
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

		// 新的窗口会自动获得工作焦点
		SetWorkingWindow(hWnd);

		//if (!hWnd)
		//{
		//	if (hParent)						// 创建子窗口失败，则使父窗口恢复正常
		//	{
		//		EnableWindow(hParent, true);
		//	}
		//	return nullptr;
		//}
		//if (hWnd)
		//{
		//	//while (nStartAnimation == false)	Sleep(50);		// 等待初始动画完成
		//	
		//	// 预设背景色
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
		// 窗口未被关闭时，无法创建新窗口
		if (IsWindowExists(m_nWindowIndex))
		{
			return nullptr;
		}

		// 预设窗口属性
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
		// 测试结论：此函数不能与 UpdateLayeredWindow 一同使用

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

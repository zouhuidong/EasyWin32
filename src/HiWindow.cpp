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

	// 检验窗口索引是否合法
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

	// 已弃用，直接使用 GetWindowCanvas
	//IMAGE* GetWindowImage(HWND hWnd)
	//{
	//	int index = GetWindowIndex(hWnd);
	//	if (IsWindowExists(index))
	//	{
	//		//return g_vecWindows[index].pBufferImg;
	//		return g_vecWindows[index].pCanvas;
	//	}
	//	else
	//	{
	//		return nullptr;
	//	}
	//}

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
			return GetImageBuffer(img);
		}
		else if (HasFocusedWindow())
		{
			return GetImageBuffer(g_vecWindows[g_nFocusWindowIndex].pCanvas);
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
			return GetImageHDC(img);
		}
		else if (HasFocusedWindow())
		{
			return GetImageHDC(g_vecWindows[g_nFocusWindowIndex].pCanvas);
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
			SetWorkingImage(img);
		}
		else if (HasFocusedWindow())
		{
			SetWorkingImage(g_vecWindows[g_nFocusWindowIndex].pCanvas);
		}
		else
		{
			//throw std::runtime_error("SetWorkingImageHX(nullptr) called, but no working window assigned.");
		}
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

	/*void BindWindowCanvas(Canvas* pCanvas, HWND hWnd)
	{
		int index = GetWindowIndex(hWnd);
		if (IsWindowExists(index))
		{
			g_vecWindows[index].pBufferImgCanvas = pCanvas;
			pCanvas->BindToWindow(g_vecWindows[index].hWnd, g_vecWindows[index].pBufferImg);
		}
	}*/

	/*void init_end(HWND hWnd)
	{
		if (hWnd)
		{
			int index = GetWindowIndex(hWnd);
			while (IsWindowExists(index))
				Sleep(100);
		}
		else
			while (IsAnyWindow())
				Sleep(100);
	}*/

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

	EasyWindow GetWorkingWindow()
	{
		return GetFocusedWindow();
	}

	bool SetWorkingWindow(HWND hWnd)
	{
		//if (!hWnd || GetFocusedWindow().hWnd == hWnd)
		//{
		//	if (GetWorkingImage() != GetFocusedWindow().pCanvas)
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

	//void QuickDraw(UINT nSkipPixels, HWND hWnd)
	//{
	//	int index = GetWindowIndex(hWnd);
	//	if (IsWindowExists(index))
	//		g_vecWindows[index].nSkipPixels = nSkipPixels;
	//}

	//DrawMode GetDrawMode()
	//{
	//	return g_fDrawMode;
	//}

	//void SetDrawMode(DrawMode mode)
	//{
	//	g_fDrawMode = mode;
	//}

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

		// 已弃用
		//switch (g_fDrawMode)
		//{
		//case DM_Real:
		//	RedrawInternal(hWnd);
		//	break;

		//case DM_Normal:
		//	RedrawInternal(hWnd);
		//	
		//	// 这个太慢了
		//	//InvalidateRect(hWnd, nullptr, false);
		//	break;

		//case DM_Fast:
		//	if (!(clock() % 2))
		//		SendUserRedrawMsg(hWnd);
		//	break;

		//case DM_VeryFast:
		//	if (!(clock() % 5))
		//		SendUserRedrawMsg(hWnd);
		//	break;

		//case DM_Fastest:
		//	if (!(clock() % 9))
		//		SendUserRedrawMsg(hWnd);
		//	break;
		//}
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
		HiEasyX::FlushWindowBuffer(false);
		HiEasyX::MsgLoopHX();
	}

	void EndBatchDrawHX()
	{
		HiEasyX::FlushWindowBuffer(false);
		HiEasyX::MsgLoopHX();
	}

	// 已弃用
	//// 更新窗口画布的双缓冲
	//// rct 更新区域（坐标都为 0 表示全部区域）
	/*
	void FlushDrawing(int index, RECT rct = {0})
	{
		if (!IsWindowExists(index))
		{
			return;
		}

		int w = g_vecWindows[index].pImg->getwidth();
		int h = g_vecWindows[index].pImg->getheight();

		// 是否全部更新
		bool isAllFlush = !(rct.left && rct.top && rct.right && rct.bottom);

		// 双缓冲的两层画布
		DWORD* dst = GetImageBuffer(g_vecWindows[index].pImg);
		DWORD* src = GetImageBuffer(g_vecWindows[index].pBufferImg);

		// 部分重绘时，修正重绘区域
		RECT rctCorrected = rct;
		if (!isAllFlush)
		{
			if (rct.left < 0)		rctCorrected.left = 0;
			if (rct.top < 0)		rctCorrected.top = 0;
			if (rct.right > w)		rctCorrected.right = w;
			if (rct.bottom > h)		rctCorrected.bottom = h;
		}

		// 不跳过像素的模式
		if (g_vecWindows[index].nSkipPixels == 0)
		{
			// 全部更新
			if (isAllFlush)
			{
				// fastest
				memcpy(dst, src, sizeof(DWORD) * w * h);
			}
			// 部分更新
			else
			{
				for (int x = rctCorrected.left; x < rctCorrected.right; x++)
				{
					for (int y = rctCorrected.top; y < rctCorrected.bottom; y++)
					{
						int index = x + y * w;
						dst[index] = src[index];
					}
				}
			}
		}
		// 跳过像素的模式
		else
		{
			// 全部更新
			if (isAllFlush)
			{
				int len = w * h;
				for (int i = 0; i < len; i++)		// 线性遍历画布
				{
					if (dst[i] == src[i])			// 若两画布某位置色彩重叠，则跳过接下来的 n 个像素点
					{
						i += g_vecWindows[index].nSkipPixels;
						continue;
					}
					dst[i] = src[i];
				}
			}
			// 部分更新
			else
			{
				for (int y = rctCorrected.top; y < rctCorrected.bottom; y++)	// 在矩形区域内遍历画布
				{
					for (int x = rctCorrected.left; x < rctCorrected.right; x++)
					{
						int index = x + y * w;
						if (dst[index] == src[index])	// 若两画布某位置色彩重叠，则在 x 方向上跳过接下来的 n 个像素点
						{
							x += g_vecWindows[index].nSkipPixels;
							continue;
						}
						dst[index] = src[index];
					}
				}
			}
		}
	}// FlushDrawing
	*/

	// 已弃用
	//// 提供给用户的接口
	//void FlushDrawing(RECT rct)
	//{
	//	// 为了防止用户更新双缓冲时窗口拉伸导致画布冲突，必须在窗口任务内调用此函数
	//	if (IsInTask())
	//	{
	//		FlushDrawing(g_nFocusWindowIndex, rct);
	//	}
	//}

	/*void EnableAutoFlush(bool enable)
	{
		g_bAutoFlush = enable;
	}*/

	//bool BeginTask()
	//{
	//	// 不做窗口匹配判断，只检验是否处于任务中
	//	if (!g_isInTask && HasFocusedWindow())
	//	{
	//		WaitForProcessing(g_nFocusWindowIndex);
	//		g_isInTask = true;
	//	}
	//	return g_isInTask;
	//}

	//void EndTask(bool flush)
	//{
	//	if (g_isInTask)
	//	{
	//		if (flush && HasFocusedWindow())
	//		{
	//			GetFocusedWindow().isNeedFlush = true;
	//			//FlushDrawing(g_nFocusWindowIndex);
	//		}

	//		g_isInTask = false;
	//	}
	//}

	/*bool IsInTask(HWND hWnd)
	{
		return g_isInTask && (hWnd ? GetFocusedWindow().hWnd == hWnd : true);
	}*/

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

	static void OnSize(int indexWnd)
	{
		RECT rctWnd;
		GetClientRect(g_vecWindows[indexWnd].hWnd, &rctWnd);

		//WaitForProcessing(indexWnd);
		//g_vecWindows[indexWnd].isBusyProcessing = true;		// 不能再启动任务
		//WaitForTask(g_vecWindows[indexWnd].hWnd);			// 等待最后一个任务完成

		ResizeWindowImage(indexWnd, rctWnd);

		//g_vecWindows[indexWnd].isBusyProcessing = false;
	}

	static void OnTray(int indexWnd, LPARAM lParam)
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
		}
	}

	static void OnTaskBarCreated(int indexWnd)
	{
		if (g_vecWindows[indexWnd].isUseTray)
		{
			ShowTray(&g_vecWindows[indexWnd].nid);
		}
	}

	// 登记 ExMessage 消息
	static void RegisterExMessage(int indexWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

			g_vecWindows[indexWnd].vecMessage.push_back(msgMouse);
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
		}
		break;
		}
	}

	// 绘制用户内容
	static void OnPaint(int indexWnd, HDC hdc)
	{
		// 已弃用
		//// 在开启自动刷新双缓冲的情况下，处理双缓冲的刷新任务
		//if (g_bAutoFlush && g_vecWindows[indexWnd].isNeedFlush)
		//{
		//	WaitForProcessing(indexWnd);
		//	g_vecWindows[indexWnd].isBusyProcessing = true;		// 不能再启动任务
		//	WaitForTask(g_vecWindows[indexWnd].hWnd);			// 等待最后一个任务完成

		//	// 更新双缓冲
		//	FlushDrawing(indexWnd);
		//	g_vecWindows[indexWnd].isNeedFlush = false;

		//	g_vecWindows[indexWnd].isBusyProcessing = false;
		//}

		// 将绘图内容输出到窗口 HDC
		RECT rctWnd;
		GetClientRect(g_vecWindows[indexWnd].hWnd, &rctWnd);
		//CopyImageToHDC(g_vecWindows[indexWnd].pCanvas, hdc, rctWnd);

		// 获取窗口画布的绘制原点，以呼应 setoriginHX
		HDC hdcCanvas = GetImageHDCHX(g_vecWindows[indexWnd].pCanvas);
		POINT ptCanvasOrg;
		GetViewportOrgEx(hdcCanvas, &ptCanvasOrg);

		if (g_vecWindows[indexWnd].xasp != 1 || g_vecWindows[indexWnd].yasp != 1)
		{
			if (fabs(g_vecWindows[indexWnd].xasp) > 1e-4 && fabs(g_vecWindows[indexWnd].yasp) > 1e-4)
			{
				StretchBlt(
					hdc,	/* 目标设备 */
					rctWnd.left, rctWnd.top, rctWnd.right, rctWnd.bottom, /* 目标绘制区域 */
					hdcCanvas, /* 源设备 */
					(int)(-ptCanvasOrg.x / g_vecWindows[indexWnd].xasp), /* 源拷贝起点 */
					(int)(-ptCanvasOrg.y / g_vecWindows[indexWnd].yasp),
					(int)((rctWnd.right - rctWnd.left) / g_vecWindows[indexWnd].xasp),
					(int)((rctWnd.bottom - rctWnd.top) / g_vecWindows[indexWnd].yasp),
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

	static void OnMove(HWND hWnd)
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
	}

	static void OnDestroy(int indexWnd, WPARAM wParam)
	{
		if (!IsWindowExists(indexWnd))
		{
			return;
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
	static LRESULT SysCtrlProc(int indexWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& bRet)
	{
		switch (msg)
		{
			// 创建系统控件
		case WM_SYSCTRL_CREATE:
		{
			g_vecWindows[indexWnd].bHasCtrl = true;
			bRet = true;
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

			bRet = true;
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
						bRet = true;
						return lr;
					}
				}
			}
		}

		bRet = false;
		return 0;
	}

	static void OnCreate(int indexWnd, HWND hWnd, LPARAM lParam)
	{
	}

	// 窗口过程函数
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		int indexWnd = GetWindowIndex(hWnd);		// 窗口索引

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
				OnMove(hWnd);
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
				OnDestroy(indexWnd, wParam);
				break;

			case WM_SIZE:
				OnSize(indexWnd);
				break;

				// 托盘消息
			case WM_TRAY:
				OnTray(indexWnd, lParam);
				break;

			default:
				// 系统任务栏重新创建，此时可能需要重新创建托盘
				if (msg == g_uWM_TASKBARCREATED)
				{
					OnTaskBarCreated(indexWnd);
				}
				break;
			}

			// 登记 ExMessage 消息
			RegisterExMessage(indexWnd, msg, wParam, lParam);

			// 处理系统控件消息
			bool bRetSysCtrl = false;
			LRESULT lrSysCtrl = SysCtrlProc(indexWnd, msg, wParam, lParam, bRetSysCtrl);
			if (bRetSysCtrl)
				return lrSysCtrl;

			//////////// End of HiEasyX MsgLoopHX Process ////////////

			LRESULT resultProc = 0;		// 记录返回值

			// 调用用户消息处理函数
			if (g_vecWindows[indexWnd].funcWndProc)
			{
				resultProc = g_vecWindows[indexWnd].funcWndProc(hWnd, msg, wParam, lParam);
			}

			// 否则使用默认方法处理（WM_PAINT 单独处理）
			else if (msg != WM_PAINT)
			{
				resultProc = DefWindowProc(hWnd, msg, wParam, lParam);
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
			}

			return resultProc;
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
		//wnd.pBufferImg = new IMAGE(w, h);
		//wnd.pBufferImgCanvas = nullptr;
		//wnd.isNeedFlush = false;
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
		EnableResizing(GetHWndHX(), false);

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

	EasyWindow Window::GetInfo() const
	{
		return g_vecWindows[m_nWindowIndex];
	}

	bool Window::IsAlive()
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

/**
 * @file	HiWindow.h
 * @brief	HiEasyX 库的窗口模块
 * @author	huidong
*/

/**
 * 说明：
 *		凡是 HWND 传入 nulllptr 的，代表当前活动窗口；
 *		凡是 IMAGE* 传入 nullptr 的，代表当前活动窗口的图像指针（而不是当前活动图像）。
 * 
 *		以上设置是为了兼容 EasyX 原生的设定，
 *		为了避免不必要的麻烦，建议总是明确指定要操作的窗口或图像。
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

// 补充绘图窗口初始化参数
// 普通窗口
#define EW_NORMAL							0

// 无窗口时的索引
#define NO_WINDOW_INDEX						-1

// 已弃用，直接使用 DefWindowProc 函数即可
//// 窗口过程函数默认返回值
//#define HIWINDOW_DEFAULT_PROC				(LRESULT)(-10000)

// 托盘消息
#define WM_TRAY								(WM_USER + 9337)

// 系统控件创建消息
// wParam 传入 SysControlBase*
// lParam 传入 CREATESTRUCT*
#define WM_SYSCTRL_CREATE					(WM_USER + 9338)

// 系统控件析构消息
// wParam 传入 SysControlBase*
#define WM_SYSCTRL_DELETE					(WM_USER + 9339)

//// 用户重绘消息，无需参数
//// 用户调用 RedrawWindow 以重绘窗口时会发送此消息而非 WN_PAINT
//#define WM_USER_REDRAW						(WM_USER + 9340)

namespace HiEasyX
{
	class Canvas;
	class SysControlBase;

	////////////****** 类型定义 ******////////////

	/**
	 * @brief 窗口
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
		 * @brief 创建窗口
		 * @return 返回创建的窗口句柄（窗口未被关闭时重复创建，或由于其他原因创建失败时返回 nullptr）
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
		 *		不使用 GetHWnd 为函数名，因为 GetHWnd 被宏定义为 GetHWndHX，会带来不必要的麻烦。
		 *		在 SysGUI 模块中也使用 GetHandle 这一称呼。
		 * </pre>
		*/
		HWND GetHandle() const;
		bool Exist();

		//IMAGE* GetImage();
		Canvas* GetCanvas() const;
		//void BindCanvas(Canvas* pCanvas);

		///< 设置窗口画布原点
		void SetOrigin(int x, int y) const;
		void GetOrigin(int* x, int* y) const;
		
		///< 设置窗口画布缩放比例
		void SetAspectRatio(float xasp, float yasp) const;
		void GetAspectRatio(float* xasp, float* yasp) const;

		//void WaitMyTask();
		bool SetWorkingWindow();

		/**
		 * @brief 刷新窗口绘图缓冲
		*/
		void FlushBuffer();

		// 已弃用
		///**
		// * @brief <pre>
		// *		更新窗口的双缓冲
		// *
		// *	注意：
		// *		必须在窗口任务内调用此函数，详见 hiex::FlushDrawing
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
		 * @brief 获取整个窗口的大小
		*/
		SIZE GetWindowSize();

		/**
		 * @brief 获取整个窗口的宽度
		*/
		int GetWindowWidth();

		/**
		 * @brief 获取整个窗口的高度
		*/
		int GetWindowHeight();

		/**
		 * @brief 获取客户区宽度
		*/
		int GetClientWidth();

		/**
		 * @brief 获取客户区高度
		*/
		int GetClientHeight();

		void Move(int x, int y);
		void MoveRel(int dx, int dy);

		void Resize(int w, int h);
		void SetTransparent(bool enable, int alpha = 0xFF);

		void SetTitle(LPCTSTR lpszTitle);

		/**
		 * @brief 判断此窗口是否为用户正在使用的窗口
		*/
		bool IsForegroundWindow();

		Optional<ExMessage> GetMsg(BYTE filter = -1);
		bool GetMsg(ExMessage* msg, BYTE filter = -1);
		bool PeekMsg(ExMessage* msg, BYTE filter = -1, bool removemsg = true);
		void FlushMsg(BYTE filter = -1);
	};

	////////////****** 窗体相关函数 ******////////////

	/**
	 * @brief <pre>
	 *		创建 Win32 绘图窗口（异于原生 EasyX 窗口）
	 *
	 *	备注：
	 *		窗口默认支持双击消息、调整大小（使用 EnableResizing 宏设置是否可以调整大小）
	 * </pre>
	 *
	 * @param[in] w					窗口宽
	 * @param[in] h					窗口高
	 * @param[in] flag				窗口样式（EW_ 系列宏，默认为 EW_NORMAL）
	 * @param[in] lpszWndTitle		窗口标题
	 * @param[in] WindowProcess		窗口过程函数
	 * @param[in] hParent			父窗口句柄
	 * @return 创建的窗口句柄
	 *
	 * @bug
	 *		（旧版）不建议大批量创建绘图窗口，如果必要，请适当添加延时，否则可能导致未知问题。
	 *		Ver0.5.0 以后，单线程创建窗口应该不会出现此问题
	 *
	 * @par 窗口过程函数规范 <pre>
	 *
	 *		函数签名：
	 *			LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	 *
	 *		备注：
	 *			Ver0.5.0 以后不需要使用 HIWINDOW_DEFAULT_PROC 替代 DefWindowProc()，
	 * 			直接按照传统的 Win32 WndProc 写法编写此函数即可。
	 *
	 *		示例函数：
	 * @code
				LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
				{
					switch (msg)
					{
						// 注意，如果在 WM_PAINT 中编写代码，请直接使用 EasyX 方式绘图
						// 而不要使用 Win32 方式（BeginPaint(), EndPaint()）
					case WM_PAINT:
						SetWorkingWindow(hWnd);	// 设置当前工作窗口为 hWnd
						circle(100, 100, 70);
						break;

					case WM_CLOSE:
						if (MessageBox(hWnd, _T("确定要关闭窗口吗？"), _T("确认关闭"), MB_YESNO | MB_ICONQUESTION) == IDYES)
						{
							DestroyWindow(hWnd);  // 销毁窗口，程序将收到 WM_DESTROY 消息
						}
						break;

					case WM_DESTROY:
						// TODO: 在此处释放申请的内存
						PostQuitMessage(0);		// 向系统指示线程已请求终止（退出）
						break;

					default:						
						return DefWindowProc(hWnd, msg, wParam, lParam);

						// 已弃用的方式（Ver0.5.0 以前采用）：
						//return HIWINDOW_DEFAULT_PROC;	// 标识使用默认消息处理函数继续处理
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
	 * @brief 创建 EasyX 绘图窗口（将开启一系列兼容性选项，以模拟原始的 EasyX 设置）
	*/
	HWND initgraphCompatible(
		int w,
		int h,
		int flag = EW_NORMAL
	);

	bool init_console();	/// 初始化命令行窗口（最多只维持一个命令行窗口）
	bool hide_console();	/// 隐藏命令行窗口
	bool close_console();	/// 关闭命令行窗口（直接运行程序时可以关闭命令行，但是在 VS 中调试时可能无法关闭）

	/**
	 * @brief 关闭某一绘图窗口
	 * @param[in] hWnd 窗口句柄（为空标识当前活动窗口）
	 * @attention <pre>
	 *		绘图窗口关闭后，需要重新 SetWorkingImage / SetWorkingWindow 再绘制，否则将出错。
	 *		HiEasyX 提供的函数如果需要传入 HWND，则都可以传入 nullptr 以表示当前窗口，这是为了兼容传统 EasyX 代码，
	 *		但是当您的程序拥有不只一个窗口时，最好每次都明确指定要操作的 HWND，这样可以避免很多麻烦。
	 *		此外，建议直接通过 Window 类和 Canvas 类进行面向对象的操作，以明确要操作的窗口、画布对象。
	 * </pre>
	 * @note 该函数内部调用 DestroyWindow() 以销毁窗口，用户若自定义了窗口过程函数，则会接收到 WM_DESTROY 消息，可以在此时释放程序的内存。
	*/
	void closegraphHX(HWND hWnd = nullptr);

	/**
	 * @brief 关闭所有绘图窗口
	*/
	void closeallgraph();

	/**
	 * @brief 当所有窗口都被销毁时，自动退出程序
	 * @note <pre>
	 *		在创建第一个窗口之前也可以调用此函数（Ver0.5.0 以前可能不行）
	 * 
	 *		原理：
	 *		即在 WM_DESTROY 中判断当前窗口销毁后是否还有窗口存在，如果没有则 PostQuitMessage()，
	 *		然后 MsgLoopHX() 接收到 WM_QUIT 消息，执行 exit() 退出程序。
	 *		若不调用此函数，则所有窗口被销毁后，程序也不会直接退出，在这种情况下，用户需要根据 IsAnyWindow() 决定是否退出程序。
	 * 
	 *		该函数主要用于兼容原生 EasyX 代码。
	 * </pre>
	*/
	void AutoExit(bool enable);

	/**
	 * @brief 只允许一个绘图窗口存在，重复创建绘图窗口时关闭先前的窗口
	 * @note 该函数主要用于兼容原生 EasyX 代码
	*/
	void SingleGraphWindow(bool enable);

	/**
	 * @brief 设置某窗口的过程函数
	 * @param[in] hWnd 窗口句柄（为空标识当前活动窗口）
	 * @param[in] WindowProcess 新的过程函数
	*/
	void SetWndProcFunc(HWND hWnd, WNDPROC WindowProcess);

	/**
	 * @brief HiEasyX 内部消息循环函数（非阻塞）
	 * @note <pre>
	 *		需要在程序主循环中调用此函数，否则窗口无法响应消息
	 * 
	 *		会自动调用 MsgLoopHX() 的函数：
	 *		* HiEasyX::getmessageHX(), HiEasyX::peekmessageHX() 等所有消息相关函数；
	 *		* HiEasyX::SleepHX(), HiEasyX::HpSleepHX()
	 * 
	 *		上述函数会自动调用消息循环，以在阻塞时或需要获取消息时驱动消息循环运行，以维持程序正常运作。
	 * </pre>
	*/
	void MsgLoopHX();

	/**
	 * @brief 睡眠函数（睡眠期间维持 HiEasyX 消息循环）
	 * @param[in] ms 休眠时长（单位：毫秒）
	 * @param[in] interval 轮询间隔（单位：毫秒，默认为 10 毫秒，受系统限制精度只有 10~15 ms）
	 * @note 该函数每隔 interval 时长轮询一次消息队列，直到 ms 时长结束。
	*/
	void SleepHX(int ms, int interval = 10);

	/**
	 * @brief 精确睡眠函数（区别于 SleepHX，该函数在延时中会考虑程序本身执行的耗时，因此更精确）
	 * @param[in] ms 休眠时长（单位：毫秒）
	 * @param[in] interval 轮询间隔（单位：毫秒，默认为 1 毫秒，以期较高的精度）
	 * @note <pre>
	 *		该函数在延时中依然会维持 HiEasyX 消息循环，可能造成精度下降。如不希望进行消息循环，请使用 HpSleep
	 * </pre>
	*/
	void HpSleepHX(int ms, int interval = 1);

	/**
	 * @brief 得到当前活动绘图窗口的句柄
	*/
	HWND GetHWndHX();

	///**
	// * @brief 得到当前绘图窗口的详细信息
	//*/
	//EasyWindow GetWorkingWindow();

	/**
	 * @brief 是否还存在未销毁的绘图窗口
	*/
	bool IsAnyWindow();

	/**
	 * @brief 判断一窗口是否还存在（未被关闭）
	 * @param[in] hWnd 窗口句柄（为空表示当前活动窗口）
	 * @return 是否存在
	*/
	bool IsWindowExists(HWND hWnd = nullptr);

	/**
	 * @brief 获取窗口画布指针
	 * @param[in] hWnd 窗口句柄（为空表示当前活动窗口）
	 * @return 画布指针，若未绑定画布则返回空
	*/
	Canvas* GetWindowCanvas(HWND hWnd = nullptr);

	/**
	 * @brief 获取图像缓冲区指针（替代 EasyX 原生 GetImageBuffer 函数）
	 * @param[in] img（图像指针，nullptr 表示当前活动窗口图像指针）
	 * @return 图像缓冲区指针
	 * @attention <pre>
	 *		使用 HiWindow 时，向原生的 GetImageBuffer 函数传入 nullptr 无法对应得到当前活动窗口的图像缓冲区，
	 *		并且可能导致程序崩溃。因此建议总是显示指定要获取的图像指针。
	 * 
	 *		事实上，如果您的代码中含有一些不是完全基于 HiEasyX 的代码（例如 EasyX 原生代码），
	 *		则很有可能包含 GetImageBuffer(nullptr) 这样的代码。必须要求这些文件包含 HiEasyX.h，
	 *		因为在 HiEasyX.h 中，GetImageBuffer 被宏定义为 HiEasyX::GetImageBufferHX，以此避免问题的产生。
	 * </pre>
	 * @throw 如果传入 nullptr 但当前没有活动窗口，则抛出 runtime_error 异常。
	*/
	DWORD* GetImageBufferHX(IMAGE* img = nullptr);

	/**
	 * @brief 获取图像 HDC（替代 EasyX 原生 GetImageHDC 函数），无需 ReleaseDC
	 * @param[in] img（图像指针，nullptr 表示当前活动窗口图像指针）
	 * @return 图像缓冲区指针
	 * @attention 必须使用该函数替代 EasyX 原生 GetImageHDC 函数，其原因参见 GetImageBufferHX 函数的说明。
	 * @throw 如果传入 nullptr 但当前没有活动窗口，则抛出 runtime_error 异常。
	*/
	HDC GetImageHDCHX(IMAGE* img = nullptr);

	/**
	 * @brief 设置活动 IMAGE 对象（替代 EasyX 原生 SetWorkingImage 函数）
	 * @param[in] img（图像指针，nullptr 表示当前活动窗口图像指针）
	 * @attention 建议使用该函数替代 EasyX 原生 SetWorkingImage 函数，其原因与 GetImageBufferHX 函数类似。
	 * @throw 即使设置失败，也不会抛出异常
	*/
	void SetWorkingImageHX(IMAGE* img = nullptr);

	/**
	 * @brief 设置窗口画布的绘制原点
	 * @param[in] x 原点 x 坐标
	 * @param[in] x 原点 y 坐标
	 * @param[in] hwnd 窗口句柄（为空表示当前活动窗口）
	 * @attention 该函数只对窗口画布有效
	 * @note <pre>
	 *		此函数原理为调用 GDI 函数 SetViewportOrgEx() 设置原点，随后 GDI 绘制内容都会以设置的原点坐标为基准。
	 *		但是这样设置以后，IMAGE 的真实左上角坐标便不再是 (0, 0) 而是 (-x_org, -y_org)，因此若要正确输出此图像，
	 *		需要在输出时特别指定原图像的输出原点。
	 *		然而，EasyX 的 putimage 等并不支持此设置（可能因为是逐像素复制），因此无法配合实现原点设置效果。
	 *		因此，和 setaspectratioHX 函数一样，此函数也仅对窗口画布有效。
	 * </pre>
	*/
	void setoriginHX(int x, int y, HWND hwnd = nullptr);

	/**
	 * @brief 获取窗口画布的原点坐标
	*/
	void getoriginHX(int* px, int* py, HWND hwnd = nullptr);

	/**
	 * @brief 设置窗口画布的缩放比例
	 * @param[in] xasp x 轴缩放比例（可以为负值）
	 * @param[in] yasp y 轴缩放比例（可以为负值）
	 * @param[in] hwnd 窗口句柄（为空表示当前活动窗口）
	 * @attention 该函数只对窗口画布有效
	 * @note <pre>
	 *		该函数原理是将窗口画布大小调整为 w/xasp * h/yasp，而绘制时坐标并不发生变化，
	 *		仅在窗口刷新缓冲时将窗口画布缩放为 w * h 刷新到屏幕上，因此实现缩放效果。
	 * 
	 *		由此可见，该函数不能像 setorigin 那样对一切 IMAGE 均适用，因为一般的 putimage 函数是直接按像素点复制，
	 *		并不能配合完成最后一步的缩放。窗口画布的绘制由于可以在 WM_PAINT 时采用带缩放的方式，因而可以实现此功能。
	 * </pre>
	*/
	void setaspectratioHX(float xasp, float yasp, HWND hwnd = nullptr);

	/**
	 * @brief 获取窗口画布的缩放比例
	*/
	void getaspectratioHX(float* pxasp, float* pyasp, HWND hwnd = nullptr);

	/**
	 * @brief 设置活动窗口（活动窗口决定获取消息、绘制的目标窗口）
	 * @param[in] hWnd 新的活动窗口句柄
	 * @return 是否设置成功（若指定的窗口不存在或已被销毁，则将失败）
	 * @note 若 hWnd 为空，则将重新设置 Working Image 到先前的活动窗口
	*/
	bool SetWorkingWindow(HWND hWnd);

	/**
	 * @brief 刷新指定绘图窗口缓冲区
	 * @param[in] hWnd 要刷新的窗口（为空表示当前活动窗口）
	 * @param[in] bInstant 是否立即刷新（否则等待窗口消息队列空闲时刷新）
	 * @note <pre>
	 *		HiEasyX 是天然双缓冲的，在 HiEasyX 中对窗口的绘制都只是缓存到了 Canvas 上，
	 *		如果需要将 Canvas 的内容刷新到窗口上，请调用此函数。
	 *		HiEasyX 在用户处理 WM_PAINT 消息之后也会自动刷新窗口缓冲，因此您若在 WM_PAINT 中编写绘图代码，则无需调用此函数。
	 * 
	 *		由于总是需要保存一份窗口绘制内容的副本以便在窗口重绘（WM_PAINT）时使用，
	 *		所以直接绘制到窗口 HDC 上对于 HiEasyX 实现多窗口并没有什么好处，
	 *		因此 HiEasyX 也不必重写 EasyX 底层。
	 * </pre>
	 * @attention <pre>
	 *		若需要在 WM_PAINT 中绘图，请不要使用 Win32 传统的方法（BeginPaint(), EndPaint()）
	 *		直接以 HiEasyX 的方式绘图即可（也就是绘制到窗口 Canvas 上）。
	 * </pre>
	*/
	void FlushWindowBuffer(HWND hWnd = nullptr, bool bInstant = false);

	/**
	 * @brief 刷新所有绘图窗口缓冲区
	 * @param[in] bInstant 是否立即刷新（否则等待窗口消息队列空闲时刷新）
	*/
	void FlushAllWindowBuffer(bool bInstant = false);

	/**
	 * @brief 自动刷新绘图窗口缓冲区（即在 MsgLoopHX() 中自动执行 FlushAllWindowBuffer()）
	 * @note <pre>
	 *		该函数主要用于兼容原生 EasyX 代码
	 * 
	 *		按照 HiEasyX 的设计，用户应在程序主循环中调用 FlushWindowBuffer() 函数以输出绘图缓冲，
	 *		并调用 MsgLoopHX() 函数维持窗口消息处理。
	 * 
	 *		为了兼容原生 EasyX 代码，以及应对 getmessage 等阻塞的情况，在多种情况下，
	 *		HiEasyX 内部将自动调用 MsgLoopHX() 以维持消息循环（详见 MsgLoopHX 函数注释）。
	 *		所以，即使用户不显式调用 MsgLoopHX()，一般只要他们有调用 getmessage, Sleep 等函数（被宏定义过），程序也可以正常运行。
	 *		但是，HiEasyX 一般并不在内部调用 FlushWindowBuffer()，如果用户忘记在绘制完成后调用 FlushWindowBuffer()，
	 *		则他们很可能看不到绘制结果。
	 * 
	 *		HiEasyX 为了兼容原生 EasyX 代码，同时给出以下两个解决方案：
	 *		1. 原本的双缓冲函数 FLushBatchDraw 和 EndBatchDraw 被宏定义为刷新窗口缓冲并进行消息循环。
	 *		2. 提供 AutoFlushWindowBuffer() 函数，即在 MsgLoopHX() 中自动调用 FlushAllWindowBuffer()。
	 * 
	 *		对于本来使用了双缓冲的 EasyX 代码，几乎完全不需要修改就可以在 HiEasyX 上运行。
	 *		对于原本没有使用双缓冲的 EasyX 代码，则需要在关键位置添加 FlushWindowBuffer()（较麻烦），
	 *		或者直接开启 AutoFlushWindowBuffer()，这样就可以看到绘图效果。
	 * 
	 *		还有一种极端情况，如果原 EasyX 代码中没有调用任何可以自动进行 MsgLoopHX() 的函数或宏
	 *		（如 getmessage, Sleep, FlushBatchDraw）等，则该代码可能会在 HiEasyX 上运行时卡死，
	 *		此时必须对源码进行必要的修改。不过，这种情况是很少见的。
	 * </pre>
	*/
	void AutoFlushWindowBuffer(bool enable);

	void BeginBatchDrawHX();	///< 仅用于兼容 EasyX 的 BeginBatchDraw 函数
	void FlushBatchDrawHX();	///< 仅用于兼容 EasyX 的 FlushBatchDraw 函数
	void EndBatchDrawHX();		///< 仅用于兼容 EasyX 的 EndBatchDraw 函数

	/**
	 * @brief 判断某窗口的大小是否改变
	 * @param[in] hWnd 窗口句柄（为空表示当前活动窗口）
	 * @return 窗口的大小是否改变
	*/
	bool IsWindowSizeChanged(HWND hWnd = nullptr);

	/**
	 * @brief <pre>
	 *		为窗口创建一个托盘
	 *
	 *	注意：
	 *		在 HiEasyX 中，每个窗口仅能稳定占有一个托盘
	 * </pre>
	 *
	 * @param[in] lpszTrayName 托盘提示文本
	 * @param[in] hWnd 窗口句柄（为空表示当前活动窗口）
	*/
	void CreateTray(LPCTSTR lpszTrayName, HWND hWnd = nullptr);

	/**
	 * @brief 删除某窗口的托盘
	 * @param[in] hWnd 窗口句柄（为空表示当前活动窗口）
	*/
	void DeleteTray(HWND hWnd = nullptr);

	/**
	 * @brief 设置托盘菜单（允许在任何时候设置）
	 * @param[in] hMenu	菜单
	 * @param[in] hWnd	窗口句柄（为空表示当前活动窗口）
	*/
	void SetTrayMenu(HMENU hMenu, HWND hWnd = nullptr);

	/**
	 * @brief 设置托盘菜单消息处理函数
	 * @param[in] pFunc	消息处理函数
	 * @param[in] hWnd	窗口句柄（为空表示当前活动窗口）
	*/
	void SetTrayMenuProcFunc(void(*pFunc)(UINT), HWND hWnd = nullptr);

	/**
	 * @brief 获取自定义程序图标的启用状态
	*/
	bool GetCustomIconState();

	/**
	 * @brief <pre>
	 *		使用自定义图标资源作为程序图标
	 *
	 *	备注：
	 *		必须在第一次创建窗口前就调用该函数才能生效。
	 *		使用 MAKEINTRESOURCE 宏可以将资源 ID 转为字符串。
	 * </pre>
	 *
	 * @param[in] lpszIcon		大图标资源
	 * @param[in] lpszIconSm	小图标资源
	*/
	void SetCustomIcon(LPCTSTR lpszIcon, LPCTSTR lpszIconSm);

	/**
	 * @brief <pre>
	 *		在创建窗口前设置窗口样式，仅对此操作后首个新窗口生效
	 *
	 *	注意：
	 *		新窗口的所有普通样式都将被当前样式覆盖
	 * </pre>
	 *
	 * @param[in] lStyle 新样式
	*/
	void PreSetWindowStyle(long lStyle);

	/**
	 * @brief <pre>
	 *		在创建窗口前设置窗口扩展样式，仅对此操作后首个新窗口生效
	 *
	 *	注意：
	 *		新窗口的所有扩展样式都将被当前样式覆盖
	 * </pre>
	 *
	 * @param[in] lStyleEx 新样式
	*/
	void PreSetWindowStyleEx(long lStyleEx);

	/**
	 * @brief 在创建窗口前设置窗口位置，仅对此操作后首个新窗口生效
	 * @param[in] x	位置
	 * @param[in] y	位置
	*/
	void PreSetWindowPos(int x, int y);

	/**
	 * @brief 在创建窗口前设置窗口显示状态，仅对此操作后首个新窗口生效
	 * @param[in] nCmdShow 显示状态（和 ShowWindow 用法一致）
	*/
	void PreSetWindowShowState(int nCmdShow);

	/**
	 * @brief 设置某窗口样式
	 * @param[in] lNewStyle 新样式
	 * @param[in] hWnd		窗口句柄（为空代表当前活动窗口）
	 * @return 返回上一次设置的窗口样式，失败返回 0
	*/
	int SetWindowStyle(long lNewStyle, HWND hWnd = nullptr);

	/**
	 * @brief 设置某窗口扩展样式
	 * @param[in] lNewExStyle	新样式
	 * @param[in] hWnd 			窗口句柄（为空代表当前活动窗口）
	 * @return 返回上一次设置的窗口样式，失败返回 0
	*/
	int SetWindowExStyle(long lNewExStyle, HWND hWnd = nullptr);

	/**
	 * @brief 获取窗口位置
	 * @param[in] hWnd 窗口句柄（为空代表当前活动窗口）
	 * @return 窗口位置
	*/
	POINT GetWindowPos(HWND hWnd = nullptr);

	/**
	 * @brief 获取窗口大小
	 * @param[in] hWnd 窗口句柄（为空代表当前活动窗口）
	 * @return 窗口大小
	*/
	SIZE GetWindowSize(HWND hWnd = nullptr);

	/**
	 * @brief 移动窗口
	 * @param[in] x		位置
	 * @param[in] y		位置
	 * @param[in] hWnd	窗口句柄（为空代表当前活动窗口）
	*/
	void MoveWindow(int x, int y, HWND hWnd = nullptr);

	/**
	 * @brief 相对移动窗口
	 * @param[in] dx	相对位移
	 * @param[in] dy	相对位移
	 * @param[in] hWnd	窗口句柄（为空代表当前活动窗口）
	*/
	void MoveWindowRel(int dx, int dy, HWND hWnd = nullptr);

	/**
	 * @brief 重设窗口大小
	 * @param[in] w		窗口宽
	 * @param[in] h		窗口高
	 * @param[in] hWnd	窗口句柄（为空代表当前活动窗口）
	*/
	void ResizeWindow(int w, int h, HWND hWnd = nullptr);

	/**
	 * @brief 设置窗口标题文本
	 * @param[in] lpszTitle		新的窗口标题
	 * @param[in] hWnd			窗口句柄（为空代表当前活动窗口）
	*/
	void SetWindowTitle(LPCTSTR lpszTitle, HWND hWnd = nullptr);

	/**
	 * @brief 设置窗口透明度
	 * @param[in] HWnd 窗口句柄
	 * @param[in] enable 是否启用窗口透明度
	 * @param[in] alpha 窗口透明度值 0-255
	*/
	void SetWindowTransparent(HWND HWnd, bool enable, int alpha = 0xFF);

	////////////****** 消息相关函数 ******////////////

	/**
	 * 注：
	 *		消息相关函数在调用时都会在内部进行 MsgLoopHX()，以便可以获取到最新的消息。
	*/

	//// ExMessage 式函数

	/**
	 * @brief 阻塞等待新消息
	 * @param[in] filter	消息筛选方式
	 * @param[in] hWnd		窗口句柄（为空代表当前活动窗口）
	 * @return 获取到的消息（若等待中窗口被关闭，则返回的 Optional 值 isSet 为空）
	*/
	Optional<ExMessage> getmessageHX(BYTE filter = -1, HWND hWnd = nullptr);

	/**
	 * @brief 阻塞等待新消息
	 * @param[out] msg	返回获取到的消息
	 * @param[in] filter	消息筛选方式
	 * @param[in] hWnd		窗口句柄（为空代表当前活动窗口）
	 * @return 是否获取到消息（若等待中窗口被关闭，则返回 false）
	*/
	bool getmessageHX(ExMessage* msg, BYTE filter = -1, HWND hWnd = nullptr);

	/**
	 * @brief 获取一个消息，立即返回是否获取成功
	 * @param[out] msg	返回获取到的消息
	 * @param[in] filter	消息筛选方式
	 * @param[in] removemsg	获取消息后是否将其移除
	 * @param[in] hWnd		窗口句柄（为空代表当前活动窗口）
	 * @return 是否获取到消息
	*/
	bool peekmessageHX(ExMessage* msg, BYTE filter = -1, bool removemsg = true, HWND hWnd = nullptr);

	/**
	 * @brief 清除所有消息记录
	 * @param[in] filter	消息筛选方式
	 * @param[in] hWnd		窗口句柄（为空代表当前活动窗口）
	*/
	void flushmessageHX(BYTE filter = -1, HWND hWnd = nullptr);

	//// MOUSEMSG 式函数（兼容）

	/**
	 * @brief 检查是否存在鼠标消息
	 * @param[in] hWnd 窗口句柄（为空代表当前活动窗口）
	 * @return 是否存在鼠标消息
	*/
	bool MouseHitHX(HWND hWnd = nullptr);

	/**
	 * @brief 阻塞等待，直到获取到一个新的鼠标消息
	 * @param[in] hWnd 窗口句柄（为空代表当前活动窗口）
	 * @return 鼠标消息（若等待中窗口被关闭，则返回的 Optional 值 isSet 为空）
	*/
	Optional<MOUSEMSG> GetMouseMsgHX(HWND hWnd = nullptr);

	/**
	 * @brief 获取一个新的鼠标消息，立即返回是否获取成功
	 * @param[out] pMsg		返回获取到的消息
	 * @param[in] bRemoveMsg	获取消息后是否将其移除
	 * @param[in] hWnd			窗口句柄（为空代表当前活动窗口）
	 * @return 是否获取到消息
	*/
	bool PeekMouseMsgHX(MOUSEMSG* pMsg, bool bRemoveMsg = true, HWND hWnd = nullptr);

	/**
	 * @brief 清空鼠标消息
	 * @param[in] hWnd 窗口句柄（为空代表当前活动窗口）
	*/
	void FlushMouseMsgBufferHX(HWND hWnd = nullptr);

	//// 转换

	/**
	 * @brief MOUSEMSG 转 ExMessage
	 * @param[in] msg MOUSEMSG 消息
	 * @return ExMessage 消息
	*/
	ExMessage To_ExMessage(MOUSEMSG msg);

	/**
	 * @brief <pre>
	 *		ExMessage 转 MOUSEMSG
	 *
	 *	备注：
	 *		ExMessage 消息类型若不是 EM_MOUSE，则返回空
	 * </pre>
	 *
	 * @param[in] msgEx ExMessage 消息
	 * @return MOUSEMSG 消息
	*/
	MOUSEMSG To_MouseMsg(ExMessage msgEx);

	/**
	 * @brief 获取 ExMessage 的消息类型
	 * @param[in] msg 消息
	 * @return EM_ 消息类型中的一种，若失败返回 0
	*/
	UINT GetExMessageType(ExMessage msg);
}

////////////****** 窗口样式宏定义 ******////////////

// 【模板宏】启用 / 禁用某属性
// hwnd			目标窗口
// state		启用还是禁用
// isExStyle	是否为 Ex 属性
// styleCode	属性代码
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

// 是否允许某窗口改变大小
#define EnableResizing(hwnd, state)				EnableSomeStyle(hwnd, state, false, WS_SIZEBOX | WS_MAXIMIZEBOX)

// 是否启用某窗口的系统标题栏按钮
#define EnableSystemMenu(hwnd, state)			EnableSomeStyle(hwnd, state, false, WS_SYSMENU)

// 是否启用当前窗口的工具栏样式
#define EnableToolWindowStyle(hwnd, state)		EnableSomeStyle(hwnd, state, true, WS_EX_TOOLWINDOW)

////////////****** 键盘消息宏定义 ******////////////

// 判断系统全局按键状态
#define KEY_DOWN_GOLBAL(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

// 判断指定窗口是否接受到某按键消息
// 窗口句柄为空代表 HiEasyX 的活动窗口
#define KEY_DOWN_WND(hWnd, VK_NONAME) (GetForegroundWindow() == (hWnd ? hWnd : HiEasyX::GetHWndHX()) && KEY_DOWN_GOLBAL(VK_NONAME))

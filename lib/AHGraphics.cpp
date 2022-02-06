//////////////////////////////
//
//	AHGraphics.cpp
//	
//	详细信息见 AHGraphics.h
//

#include "AHGraphics.h"

//////////////////// 全局变量 ////////////////////

// 内部变量
// 当前绘图窗口 HWND
HWND m_wnd = NULL;

// 内部变量
// 当前绘图 HDC
HDC m_hdc = NULL;

// 内部变量
// 当前绘图主 IMAGE 画布
IMAGE m_img;

// 内部变量
// 自动输出绘图内容的线程的句柄
HANDLE m_hThAutoFlush = NULL;

// 内部变量
// 和自动输出绘图内容的线程的通讯标志
// 指定是否自动绘图
bool m_isAutoFlush = false;


//////////////////// 函数定义 ////////////////////

//////////////////// 其它函数

// 得到鼠标相对于窗口的位置
POINT GetMousePoint()
{
	POINT point;    // 保存鼠标位置
	GetCursorPos(&point);            // 获取鼠标指针位置（屏幕坐标）
	ScreenToClient(GetWorkingHWnd(), &point);    // 将鼠标指针位置转换为窗口坐标
	return point;
}

// 得到窗口位置、区域
RECT GetWindowRect(HWND wnd)
{
	RECT rcClient;
	GetClientRect(wnd, &rcClient);
	return rcClient;
}

// 得到窗口大小
POINT GetWindowSize(HWND wnd)
{
	POINT size;
	RECT rct;

	rct = GetWindowRect(wnd);
	size = { rct.right - rct.left,rct.bottom - rct.top };

	return size;
}

//////////////////// 设置参数类函数

// 设置当前绘图目标窗口
// 请勿直接调用此函数，请使用 InitDrawing
void SetWorkingHWnd(HWND wnd)
{
	m_wnd = wnd;
	m_hdc = GetDC(m_wnd);
}

// 得到当前绘图目标窗口
HWND GetWorkingHWnd()
{
	return m_wnd;
}

// 设置当前绘图目标HDC
// 请勿直接调用此函数，请使用 InitDrawing
void SetWorkingHDC(HDC hdc)
{
	m_hdc = hdc;
	SetWorkingHWnd(WindowFromDC(hdc));
}

// 得到当前绘图目标HDC
HDC GetWorkingHDC()
{
	return m_hdc;
}

// 得到主画布
IMAGE* GetMainImage()
{
	return &m_img;
}


//////////////////// 绘图函数

// 将绘制在 EasyX 中的内容显示到目标窗口上
// pImg 要显示的绘图对象，默认为主画布
// wnd 目标绘图窗口，默认为主窗口
void FlushDrawingToWnd(IMAGE* pImg, HWND wnd)
{
	HDC me_hdc;
	HDC img_hdc;
	HWND hwnd;

	if (pImg == NULL)
	{
		img_hdc = GetImageHDC(GetMainImage());
	}
	else
	{
		img_hdc = GetImageHDC(pImg);
	}

	if (wnd == NULL)
	{
		me_hdc = GetWorkingHDC();
		hwnd = GetWorkingHWnd();
	}
	else
	{
		me_hdc = GetDC(wnd);
		hwnd = wnd;
	}

	POINT wnd_size = GetWindowSize(hwnd);

	BitBlt(
		me_hdc,			// 目标绘图设备 
		0,				// 目标区域左上角x坐标 
		0,				// 目标区域左上角y坐标 
		wnd_size.x,		// 目标区域宽度 
		wnd_size.y,		// 目标区域高度 
		img_hdc,		// 源绘图设备 
		0,				// 源区域左上角x坐标 
		0,				// 源区域左上角y坐标 
		SRCCOPY			// 操作码。常用SRCCOPY（复制）
	);
}


// 一直更新绘图内容（堵塞性的）
//
// delay 每次更新的延时（ms）
// flag 保持运作的标志，为 false 时停止
void KeepFlushDrawing(int delay = 60, bool* flag = &m_isAutoFlush)
{
	while (true)
	{
		if (*flag == false)
		{
			return;
		}

		FlushDrawingToWnd();
		Sleep(delay);
	}
}



//////////////////// 绘图环境初始化类函数


// 使主 IMAGE 画布大小适应目标绘图窗口大小
void MainImageFitWindow()
{
	POINT wnd_size = GetWindowSize(GetWorkingHWnd());
	GetMainImage()->Resize(wnd_size.x, wnd_size.y);
}


// 对某一窗口绘图前的初始化
// wnd 窗口句柄
void InitDrawing(HWND wnd)
{
	SetWorkingHWnd(wnd);
	SetWorkingImage(GetMainImage());

	MainImageFitWindow();
}

// 对某一 HDC 绘图前的初始化
// hdc DC 句柄
void InitDrawing(HDC hdc)
{
	SetWorkingHDC(hdc);
	SetWorkingImage(GetMainImage());

	MainImageFitWindow();
}

// 开启 / 关闭 自动输出绘图内容（无需在绘图后手动 FlushDrawingToWnd）
// 注意：除非必要，否则不要开启自动输出绘图内容
//
// isAuto 关闭还是开启自动输出，默认为 true
void AutoFlushDrawing(bool isAuto)
{
	if (isAuto)
	{
		m_isAutoFlush = true;
		m_hThAutoFlush = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)KeepFlushDrawing/* 函数名 */, 0, 0, NULL);
	}
	else
	{
		m_isAutoFlush = false;

		// 关闭自动输出绘图内容的线程
		if (m_hThAutoFlush != NULL)
		{
			CloseHandle(m_hThAutoFlush);
		}
	}

}

// 结束绘图
// 处理一些结束绘图的后续工作
void EndDrawing()
{
	// 关闭自动输出绘图内容
	AutoFlushDrawing(false);

	// 无需设置 HWND 为 NULL，因为设置 HDC 会直接将 HWND 清空
	SetWorkingHDC(NULL);
}

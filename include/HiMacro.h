/**
 * @file	HiMacro.h
 * @brief	HiEasyX 库的常用宏定义模块
 * @author	huidong
*/

#pragma once

#include <graphics.h>

////// 透明相关

// 显存 DWORD 中的颜色存储方式：ARGB
// EasyX 绘图函数使用的 COLORREF 的颜色存储方式：ABGR
// EasyX 原生绘图函数不支持 Alpha 值，如果传入的 COLORREF 值 & 0xFF000000 != 0，可能会绘制失败

// 以 ABGR 混合颜色，每个通道取值 0~255
#define ABGR(a, b, g, r)				(COLORREF)( ((BYTE)(a)<<24) | ((BYTE)(b)<<16) | ((BYTE)(g)<<8) | (BYTE)(r) )

// 以 ARGB 混合颜色，每个通道取值 0~255
#define ARGB(a, r, g, b)				(COLORREF)( ((BYTE)(a)<<24) | ((BYTE)(r)<<16) | ((BYTE)(g)<<8) | (BYTE)(b) )

// 获取 ARGB 或 ABGR 色值的 Alpha 通道值
#define GetAValue(color)				(BYTE)( ((color)>>24) & 0xFF )

// 移除 ARGB 或 ABGR 色值的透明度（设为 0，以传入 EasyX 原生函数）
#define ZERO_ALPHA(color)				(COLORREF)( color & 0x00FFFFFF )

// 设置颜色值的透明度
#define SET_ALPHA(color, a)				(COLORREF)( ((color) & 0xFFFFFF) | ((BYTE)(a)<<24) )

// 获取某灰度对应的 rgb 色值
#define RGB_GRAY(gray)					RGB(gray, gray, gray)


////// EasyX 绘图函数宏定义

#define rectangle_RECT(rct)				rectangle(rct.left,rct.top,rct.right,rct.bottom);
#define fillrectangle_RECT(rct)			fillrectangle(rct.left,rct.top,rct.right,rct.bottom);
#define solidrectangle_RECT(rct)		solidrectangle(rct.left,rct.top,rct.right,rct.bottom);

////// 数学

// 圆周率
#define PI 3.1415926535

////// 矩形

// 将 RECT 边框扩大 d 像素
#define EXPAND_RECT(rct, d)				(rct).left		-= d;\
										(rct).top		-= d;\
										(rct).right		+= d;\
										(rct).bottom	+= d

// 将 RECT 边框减小 d 像素
#define REDUCE_RECT(rct, d)				(rct).left		+= d;\
										(rct).top		+= d;\
										(rct).right		-= d;\
										(rct).bottom	-= d

// 是否包含矩形
#define IS_INCLUDE_RECT(rctParent, rctChild)	(	(rctChild).left		>= (rctParent).left		\
												&&	(rctChild).right	<= (rctParent).right	\
												&&	(rctChild).top		>= (rctParent).top		\
												&&	(rctChild).bottom	<= (rctParent).bottom	)

// 移动矩形
#define MOVE_RECT(rct, dx, dy)			(rct).left		+= dx;\
										(rct).top		+= dy;\
										(rct).right		+= dx;\
										(rct).bottom	+= dy

/**
 * @brief 常用色彩扩展
*/
enum EXTRA_COLORS
{
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

	CLASSICGRAY = RGB(0xF0, 0xF0, 0xF0),			///< Windows 经典灰

	MODERN_BORDER_GRAY = 0xadadad,					///< 现代边框灰
	MODERN_FILL_GRAY = 0xe1e1e1,					///< 现代填充灰

	MODERN_BORDER_BLUE = 0xd77800,					///< 现代边框蓝
	MODERN_FILL_BLUE = 0xfbf1e5,					///< 现代填充蓝

	MODERN_BORDER_PRESSED_BLUE = 0x995400,			///< 现代边框蓝（按下）
	MODERN_FILL_PRESSED_BLUE = 0xf7e4cc,			///< 现代填充蓝（按下）
};


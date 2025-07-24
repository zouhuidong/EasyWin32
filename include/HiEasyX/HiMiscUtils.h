/**
 * @file	HiMiscUtils.h
 * @brief	HiEasyX 库的常用杂项函数
 * @author	huidong
*/

#pragma once

#include <Windows.h>
#include <WinUser.h>
#include <graphics.h>

namespace HiEasyX
{

	/**
	 * @brief 存储整个屏幕的大小信息（多显示器）
	*/
	struct ScreenSize
	{
		int left;	///< 多显示器的左上角 x 坐标
		int top;	///< 多显示器的左上角 y 坐标
		int w;		///< 多显示器的总和宽度
		int h;		///< 多显示器的总和高度
	};

	/**
	 * @brief 获取多显示器大小信息
	*/
	ScreenSize GetScreenSize();

	/**
	 * @brief 精确延时函数（可以精确到 1ms，精度 ±1ms）
	 * @author yangw80 <yw80@qq.com>
	 * @date 2011-5-4
	 * @param[in] ms 延时长度（单位：毫秒）
	*/
	void HpSleep(int ms);

	/**
	 * @brief 点是否位于矩形内
	 * @param[in] x		位置
	 * @param[in] y		位置
	 * @param[in] rct		矩形
	 * @return 点是否位于矩形内
	*/
	bool IsInRect(int x, int y, RECT rct);


	// 可选参数
	template<typename T>
	struct Optional
	{
		T value{};          ///< 参数值
		bool isSet = false; ///< 是否设置了参数

		// 默认构造函数
		Optional() = default;

		// 用值初始化（标记为已设置）
		Optional(const T& val) : value(val), isSet(true) {}

		// 隐式转换为原始类型（未设置时返回默认值）
		operator T() const
		{
			return isSet ? value : T{};
		}
	};
}

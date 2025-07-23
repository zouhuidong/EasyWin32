/**
 * @file	HiGraphicsUtils.h
 * @brief	HiEasyX 库的绘图实用工具模块
 * @author	huidong
*/

#pragma once

#include <graphics.h>
#include <gdiplus.h>

namespace HiEasyX
{
	/**
	 * @brief <pre>
	 *		获取图像尺寸
	 *
	 *	备注：
	 *		可以方便地处理 IMAGE 指针为空，即指向主绘图窗口的情况
	 * </pre>
	 *
	 * @param[in] pImg			目标图像
	 * @param[out] width		返回图像宽
	 * @param[out] height		返回图像高
	*/
	void GetImageSize(IMAGE* pImg, int* width, int* height);

	/**
	 * @brief <pre>
	 *		反转图像 Alpha 值
	 *
	 *	备注：
	 *		将 alpha 值不为 0 的一切像素的 alpha 设为 0，
	 *		同时将 alpha 值为 0 的一切像素的 alpha 设为 255。
	 * </pre>
	 *
	 * @param[in, out] pBuf		显存指针
	 * @param[in] size			显存大小
	 * @return 显存指针（和原来一样）
	*/
	DWORD* ReverseAlpha(DWORD* pBuf, int size);

	/**
	 * @brief <pre>
	 *		创建指定尺寸及颜色的图像
	 *
	 *	备注：
	 *		color 可以带有透明度。
	 *		enable_alpha 若为 false 则会将返回图像透明度设为 255。
	 * </pre>
	 *
	 * @param[in] w					返回图像宽度
	 * @param[in] h					返回图像高度
	 * @param[in] color				返回图像填充颜色
	 * @param[in] enable_alpha		是否允许图像的 alpha 信息
	 * @return 显存指针（和原来一样）
	*/
	IMAGE CreateImageColor(int w, int h, COLORREF color, bool enable_alpha);

	/**
	 * @brief <pre>
	 *		设置图像的背景颜色
	 *
	 *	备注：
	 *		color 可以带有透明度。
	 *		enable_alpha 若为 false 则会将图像透明度设为 255。
	 * </pre>
	 *
	 * @param[in] img				设置的图像
	 * @param[in] color				图像填充颜色
	 * @param[in] enable_alpha		是否允许颜色的 alpha 信息
	 * @return 显存指针（和原来一样）
	*/
	void SetImageColor(IMAGE& img, COLORREF color, bool enable_alpha);

	/**
	 * @brief 得到 IMAGE 对象的 HBITMAP
	 * @param[in] img			目标图像
	 *
	 * @param[in] enable_alpha <pre>
	 *		是否允许图像的 alpha 信息
	 *
	 *	注意：
	 *		若图像 alpha 值全为 0，则表示不启用透明混合
	 * </pre>
	 *
	 * @return 转换得到的位图句柄
	*/
	HBITMAP Image2Bitmap(IMAGE* img, bool enable_alpha);

	/**
	 * @brief 得到 HBITMAP 对象的 IMAGE
	 * @param[in] img			目标图像
	 *
	 * @param[in] enable_alpha <pre>
	 *		是否允许图像的 alpha 信息
	 *
	 *	注意：
	 *		若图像 alpha 值全为 0，则表示不启用透明混合
	 * </pre>
	 *
	 * @return 转换得到的位图句柄
	*/
	IMAGE Bitmap2Image(HBITMAP* hBitmap, bool enable_alpha);

	/**
	 * @brief HBITMAP 转 HICON
	 * @param[in] hBmp 位图句柄
	 * @return 图标句柄
	*/
	HICON Bitmap2Icon(HBITMAP hBmp);

	/**
	 * @brief 绘制图像（可包含透明通道）
	 * @param[in] Dstimg 指向目标位图的指针（如果直接绘制到窗口中则填入 hiex::GetWindowImage() ）
	 * @param[in] DstimgX 目标位图上绘制的左上角横坐标
	 * @param[in] DstimgY 目标位图上绘制的左上角纵坐标
	 * @param[in] Srcimg 指向源位图的指针
	 * @param[in] transparency 叠加透明度
	 *
	 *	注意：
	 *		绘制出来的位图大小将与 Srcimg 中的图像一致
	 * </pre>
	*/
	void TransparentImage(IMAGE* Dstimg, int DstimgX, int DstimgY, IMAGE* Srcimg, int transparency = 255);

	/**
	 * @brief 绘制图像（可包含透明通道）
	 * @param[in] Dstimg 指向目标位图的指针（如果直接绘制到窗口中则填入 hiex::GetWindowImage() ）
	 * @param[in] DstimgX 目标位图上绘制的左上角横坐标
	 * @param[in] DstimgY 目标位图上绘制的左上角纵坐标
	 * @param[in] DstimgWidth 目标位图上绘制的宽度
	 * @param[in] DstimgHeight 目标位图上绘制的高度
	 * @param[in] Srcimg 指向源位图的指针
	 * @param[in] SrcimgX 源位图中要绘制的区域的左上角横坐标
	 * @param[in] SrcimgY 源位图中要绘制的区域的左上角纵坐标
	 * @param[in] SourceWidth 源位图中要绘制的区域的宽度
	 * @param[in] SourceHeight 源位图中要绘制的区域的高度
	 * @param[in] transparency 叠加透明度
	 *
	 *	注意：
	 *		若 目标位图上绘制的宽高 不等于 源位图中要绘制的区域的宽高，那么源位图中的区域将会被拉伸或压缩以适应目标位图上的绘制区域
	 * </pre>
	*/
	void TransparentImage(IMAGE* Dstimg, int DstimgX, int DstimgY, int DstimgWidth, int DstimgHeight, IMAGE* Srcimg, int SrcimgX, int SrcimgY, int SourceWidth, int SourceHeight, int transparency = 255);

	/**
	 * @brief 融合（叠加）图像透明度
	 * @param[in] img 指向目源位图的指针
	 * @param[in] transparency 需要融合（叠加）的透明度
	 * </pre>
	*/
	void OverlayImageTransparency(IMAGE* img, int transparency);

	/**
	 * @brief 移除图像透明通道
	 * @param[in] img 指向目源位图的指针
	 * </pre>
	*/
	void RemoveImageTransparency(IMAGE* img);

	/**
	 * @brief 根据透明度混合颜色
	 * @param[in] cDst		原位置像素
	 * @param[in] cSrc		待绘制像素（根据其透明度混合颜色）
	 *
	 * @param[in] isCalculated <pre>
	 *		待绘制像素点是否已经乘以它的透明度
	 *
	 *	备注：
	 *		此参数用于一些特殊情况，例如透明 png 图像中的像素就是已经乘过透明度的。
	 * </pre>
	 *
	 * @param[in] alpha		叠加在 src 上的透明度（默认为 255，即不叠加）
	 * @return 混合后的颜色（不含 alpha 值）
	*/
	COLORREF MixAlphaColor(COLORREF cDst, COLORREF cSrc, bool isCalculated, BYTE alpha = 255);

	/**
	 * @brief <pre>
	 *		快速复制图像（可开启透明通道）
	 *
	 *	备注：
	 *		若未启用任何透明通道，等同于直接复制图像。此时将保留原图像的透明度信息，否则不保留透明度信息。
	 * </pre>
	 *
	 * @param[in] x					图像输出 x 坐标
	 * @param[in] y					图像输出 y 坐标
	 * @param[in] pDst				载体图像指针
	 * @param[in] wDst				载体图像宽
	 * @param[in] hDst				载体图像高
	 * @param[in] pSrc				待输出图像指针
	 * @param[in] wSrc				待输出图像宽
	 * @param[in] hSrc				待输出图像高
	 * @param[in] crop				待输出图像裁剪区域（right 或 bottom 为 0 表示不裁剪）
	 * @param[in] alpha				叠加透明度（透明 0 ~ 255 不透明）
	 *
	 * @param[in] bUseSrcAlpha <pre>
	 *		是否使用待输出图像透明度进行混合（须保证 IMAGE 中含有透明度信息）
	 *
	 *	备注：
	 *		EasyX 中的图像一般无透明度（默认设为 0，即全透明），故一般不使用原图透明度。
	 *		通常只有 png 图像，或是特地生成的图像才含有透明度信息。
	 * </pre>
	 *
	 * @param[in] isCalculated <pre>
	 *		标记待输出图像是否已经计算好混合后的颜色（启用图像透明度时有效）
	 *
	 *	注意：
	 *		png 图像像素颜色都已进行过混合运算。
	 *		开启后，原图像便不再计算混合颜色，只有载体图像参与计算。
	 * </pre>
	*/
	void CopyImage_Alpha(
		int x,
		int y,
		DWORD* pDst, int wDst, int hDst,
		DWORD* pSrc, int wSrc, int hSrc,
		RECT crop = { 0 },
		BYTE alpha = 255,
		bool bUseSrcAlpha = false,
		bool isCalculated = false
	);

	/**
	 * @brief 旋转图像（保留透明信息，自适应大小）
	 * @param[in] pImg			原图像
	 * @param[in] radian		旋转弧度
	 * @param[in] bkcolor		背景填充颜色
	 * @return	旋转后的图像
	*/
	IMAGE RotateImage_Alpha(IMAGE* pImg, double radian, COLORREF bkcolor = BLACK);

	/**
	 * @brief	缩放图像（粗糙的、即不插值的缩放，保留透明度信息）
	 * @param[in] srcimg		原图像
	 * @param[in] width			目标宽度
	 * @param[in] height		目标高度（为 0 则根据宽度按比例缩放）
	 * @return 缩放后的图像
	*/
	IMAGE ZoomImage_Rough_Alpha(IMAGE* srcimg, int width, int height = 0);

	/**
	 * @brief	缩放图像（双线性插值，保留透明度信息）
	 * @param[in] srcimg		原图像
	 * @param[in] width			目标宽度
	 * @param[in] height		目标高度（为 0 则根据宽度按比例缩放）
	 * @return 缩放后的图像
	*/
	IMAGE ZoomImage_Alpha(IMAGE* srcimg, int width, int height = 0);

	/**
	 * @brief	图像缩放（基于 Win32 API，比较快，保留透明度信息）
	 * @param[in] srcimg		原图像
	 * @param[in] width			目标宽度
	 * @param[in] height		目标高度（为 0 则根据宽度按比例缩放）
	 * @return 缩放后的图像
	*/
	IMAGE ZoomImage_StretchBlt_Alpha(IMAGE* srcimg, int width, int height = 0);

	/**
	 * @brief 拉伸图像（可包含透明通道）
	 * @param[in] img 指向目源位图的指针
	 * @param[in] w 指定拉伸过后的宽度（为 0 则按高度比例缩放）
	 * @param[in] h 指定拉伸过后的高度（为 0 则按宽度比例缩放）
	 *
	 * @note <pre>
	 *	注意：
	 *		拉伸过后图像地址不变，可不按照比例拉伸
	 * </pre>
	 * 
	 * @note 该函数效果尚不清楚
	*/
	bool ZoomImage_AlphaBlend_Alpha(IMAGE* img, int w, int h = 0);
}


////////////////********* 宏定义 *********////////////////

//
// 准备绘制透明图形（先绘制图形到临时画布中，然后再输出到需要绘制的地方）
// nGraphW, nGraphH		所绘制透明图形的宽高
//
// 注意：需要配合 DRAW_TNS_RENDER_TO 宏使用
//
// 使用方法：
//		在 DRAW_TNS_INIT_GRAPHICS 宏和 DRAW_TNS_RENDER_TO 宏之间，插入一个代码块。
//		在这个代码块中使用 Canvas 变量 graphics 进行绘制。
//		绘制时调用 Canvas 的普通绘图函数即可，无需 GDI+ 系列封装函数（带 "GP_" 前缀的函数）
//		或者直接使用原生 EasyX 函数进行绘制也可以。
//
// 使用示例：
/*
	// 准备绘制透明图形（设置图形的宽高）
	DRAW_TNS_INIT_GRAPHICS(201, 201);
	{
		// 在代码块中使用 Canvas 的普通绘图函数进行绘制即可
		graphics.SetLineThickness(5);
		graphics.FillRoundRect(0, 0, 200, 200, 20, 20, true, GREEN, PURPLE);

		// 像这样使用 EasyX 原生函数绘制也可以
		line(20, 20, 50, 50);
	}
	// 最后选择将这个透明图形绘制到哪里，并设置绘制透明度
	DRAW_TNS_RENDER_TO(120, 120, yourImagePointer, 100);
*/
//
#define DRAW_TNS_INIT_GRAPHICS(nGraphW, nGraphH) \
	{\
		hiex::Canvas graphics(nGraphW, nGraphH);\
		graphics.BeginBatchDrawing();(0)

//
// 完成绘制透明图形，并输出绘制的图形
// nRenderX		输出位置 X 坐标
// nRenderY		输出位置 Y 坐标
// pDstImg		透明图形输出的目标画布（IMAGE*）
// alpha		输出图形时使用的透明度（完全透明 0 ~ 255 不透明）
//
// 注意：需要配合 DRAW_TNS_INIT_GRAPHICS 宏使用，具体用法见 DRAW_TNS_INIT_GRAPHICS 宏的注释
//
#define DRAW_TNS_RENDER_TO(nRenderX, nRenderY, pDstImg, alpha) \
		graphics.EndBatchDrawing();\
		ReverseAlpha(graphics.GetBuffer(), graphics.GetBufferSize());\
		graphics.RenderTo(nRenderX, nRenderY, pDstImg, { 0 }, alpha, true);\
	}(0)
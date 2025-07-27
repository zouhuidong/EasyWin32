/**
 * @file	HiCanvas.h
 * @brief	HiEasyX 库的画布模块
 * @author	huidong
*/

#pragma once

#include <HiEasyX/HiString.h>
#include <HiEasyX/HiMiscUtils.h>
#include <HiEasyX/HiGraphicsUtils.h>
#include <vector>
#include <list>
#include <string>

namespace HiEasyX
{
	/**
	 * @brief 画布（小写绘图函数内部调用 EasyX 原生函数进行绘制，大写函数则不使用 EasyX 原生函数）
	*/
	class Canvas : public IMAGE
	{
		// 已弃用
	//public:

	//	// 允许此函数调用 BindToWindow 函数
	//	friend void BindWindowCanvas(Canvas*, HWND);

	protected:

		/////// 变量 ///////

		IMAGE* m_pImgPrevious;					///< 转移 Working Image 之前的绘图对象

		DWORD* m_pBuf = nullptr;			///< 图像内存指针
		HDC m_hdc = nullptr;				///< 图像内存指针
		int m_w = 0;
		int m_h = 0;

		//bool m_bBindToImgPointer;			///< 该画布是否绑定到图像指针
		//IMAGE* m_pImg;						///< 画布绑定的图像指针（若画布绑定到指针）

		//bool m_bBatchDraw;					///< 是否启用了批量绘制

		COLORREF m_cGPLineColor = WHITE;	///< GDI+ 绘图时使用的线条颜色
		COLORREF m_cGPFillColor = WHITE;	///< GDI+ 绘图时使用的填充颜色
		float m_fGPLineWidth = 1.f;			///< GDI+ 绘图时的线条宽度
		bool m_bGPAlpha = false;			///< GDI+ 绘图时是否启用透明度
		Gdiplus::SmoothingMode m_enuSmoothingMode = Gdiplus::SmoothingModeAntiAlias;	///< GDI+ 抗锯齿模式

		//HWND m_hBindWindow;					///< 绑定到的窗口
		//bool m_bAutoMarkFlushWindow = true;	///< 绑定到窗口时，标记是否在绘制后自动设置需要更新双缓冲

		/////// 函数 ///////

		/**
		 * @brief 清空设置
		*/
		void CleanUpSettings();

		/**
		 * @brief 调用 EasyX 绘图函数前，设置该画布为目标绘图画布
		 * @return 是否设置成功
		*/
		bool BeginDrawing();

		/**
		 * @brief 调用 EasyX 绘图函数完毕，恢复先前的绘图状态
		*/
		void EndDrawing();

		// 已弃用
		///**
		// * @brief <pre>
		// *		将画布绑定到窗口（画布大小随窗口自动调整）
		// *
		// *	备注：
		// *		此函数只应该被 BindWindowCanvas 函数调用
		// * </pre>
		// *
		// * @param[in] hWnd 目标窗口
		// * @param[in] pImg 窗口图像缓冲区
		// * @return 此画布
		//*/
		//Canvas& BindToWindow(HWND hWnd, IMAGE* pImg);

	private:

		// 由于 EasyX 的 BUG 必须重写此函数
		// 详见 https://qa.codebus.cn/question/2791
		void SetDefault() override;

	public:

		/////// 画布操作函数 ///////

		Canvas();

		Canvas(int w, int h, COLORREF cBk = BLACK);

		/**
		 * @brief 复制图像内容
		 * @param[in] pImg 原图像
		*/
		Canvas(IMAGE* pImg);
		Canvas(IMAGE img);

		/**
		 * @brief 复制图像内容
		 * @param[in] pImg 原图像
		*/
		Canvas& operator= (IMAGE* pImg);
		Canvas& operator= (IMAGE img);

		// 已弃用
		///**
		// * @brief <pre>
		// *		重新加载图像尺寸信息
		// *
		// * 备注：
		// *		若绑定了图像指针，当外部调整图像大小后，须调用此函数
		// * </pre>
		//*/
		//void UpdateSizeInfo();

		/**
		 * @brief 重设画布大小（不会清空画布）
		 * @param[in] w 目标宽度
		 * @param[in] h 目标高度
		*/
		void Resize(int w, int h) override;

		// 已弃用
		///**
		// * @brief <pre>
		// *		绑定到图像指针
		// *
		// *	注意：
		// *		绑定到图像指针后，如果在外部调整了图像大小，则需要调用 UpdateSizeInfo 重新加载图像信息
		// * </pre>
		// *
		// * @param[in] pImg 目标图像指针
		// * @return 此画布
		//*/
		//Canvas& BindToImage(IMAGE* pImg);

		// 已弃用
		///**
		// * @brief <pre>
		// *		获取画布 IMAGE 指针
		// *
		// *	注意：
		// *		有的时候画布绑定了别的 IMAGE，所以绘图时不能直接使用 this，必须调用此函数。
		// * </pre>
		//*/
		//IMAGE* GetImagePointer() { return m_bBindToImgPointer ? m_pImg : this; }

		// 已弃用
		///**
		// * @brief 等价于 GetImagePointer()
		//*/
		//IMAGE* Pt() { return m_bBindToImgPointer ? m_pImg : this; }

		/**
		 * @brief 获取图像缓冲区指针（直接操作数组）
		*/
		DWORD* GetBuffer() const { return m_pBuf; }

		/**
		 * @brief 获取图像缓冲区大小，即图像面积（宽 * 高）
		*/
		int GetBufferSize() const { return getwidth() * getheight(); }

		/**
		 * @brief 获取画布的 HDC
		*/
		HDC GetHDC() const { return m_hdc; }

		// 已弃用
		///**
		// * @brief <pre>
		// *		绑定到窗口时，设置是否在每次绘制后都自动标记刷新窗口双缓冲
		// *
		// *	备注：
		// *		标记刷新窗口双缓冲并不意味着即时刷新。
		// *		标记后，窗口将会在下一次接受到绘制消息时更新双缓冲。
		// * </pre>
		//*/
		//void EnableAutoMarkFlushWindow(bool enable);
		//bool IsEnableAutoMarkFlushWindow() const { return m_bAutoMarkFlushWindow; }

		/////// 绘图状态设置函数 ///////

		// 已弃用
		///**
		// * @brief <pre>
		// *		开始大批量绘制（该函数并非用于开启双缓冲）
		// *
		// *	备注：
		// *		调用该函数后，当前绘图目标将转移到该画布，此后每次绘制不会恢复绘图目标
		// * </pre>
		//*/
		//void BeginBatchDrawing();

		///**
		// * @brief <pre>
		// *		结束批量绘制
		// *
		// *	备注：
		// *		绘图目标将恢复到批量绘制前的状态
		// * </pre>
		//*/
		//void EndBatchDrawing();

		/////// EasyX 风格的基础绘图函数 ///////

		/**
		 * @brief 判断某点是否位于图像中
		 * @param[in] x 坐标
		 * @param[in] y 坐标
		 * @param[out] pIndex 返回该点数组索引
		 * @return 是否位于图像中
		*/
		bool IsPointInside(int x, int y, int* pIndex = nullptr);

		void cleardevice(Optional<COLORREF> bkcolor = {});

		/**
		 * @brief 用背景色清空画布（区别于 Clear 函数，此函数默认保留背景色中的透明度）
		*/
		void Clear(Optional<COLORREF> bkcolor = {}, bool ignore_alpha = false);

		LINESTYLE getlinestyle();
		void setlinestyle(LINESTYLE style);
		void setlinestyle(int style, int thickness = 1, const DWORD* puserstyle = nullptr, DWORD userstylecount = 0);
		void setlinethickness(int thickness);
		int getlinethickness();

		FILLSTYLE getfillstyle();
		void setfillstyle(FILLSTYLE style);
		void setfillstyle(int style, long hatch = 0, IMAGE* ppattern = nullptr);
		void setfillstyle(BYTE* ppattern8x8);

		int getrop2();
		void setrop2(int mode);

		int getpolyfillmode();
		void setpolyfillmode(int mode);

		COLORREF getlinecolor();
		void setlinecolor(COLORREF color);

		COLORREF gettextcolor();
		void settextcolor(COLORREF color);

		COLORREF getfillcolor();
		void setfillcolor(COLORREF color);

		COLORREF getbkcolor();
		void setbkcolor(COLORREF color);

		int getbkmode();
		void setbkmode(int mode);

		/**
		 * @brief 设置绘图状态为原始状态
		*/
		//void SetDefault();

		COLORREF getpixel(int x, int y);
		void putpixel(int x, int y, COLORREF c);

		/**
		 * @brief  直接操作显存获取点（无越界检查）
		*/
		COLORREF GetPixel_Direct(int x, int y);

		/**
		 * @brief 直接操作显存绘制点（无越界检查）
		*/
		void PutPixel_Direct(int x, int y, COLORREF c);

		/**
		 * @brief 直接操作显存绘制带有透明度的点（使用 COLORREF 中的透明度）（无越界检查）
		*/
		void PutPixel_Direct_SupAlpha(int x, int y, COLORREF c);

		void line(int x1, int y1, int x2, int y2, Optional<COLORREF> c = {});
		void line(POINT pt1, POINT pt2, Optional<COLORREF> c = {});

		void rectangle(int left, int top, int right, int bottom, Optional<COLORREF> c = {});
		void rectangle(RECT rct, Optional<COLORREF> c = {});
		void fillrectangle(int left, int top, int right, int bottom, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void fillrectangle(RECT rct, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void solidrectangle(int left, int top, int right, int bottom, Optional<COLORREF> c = {});
		void solidrectangle(RECT rct, Optional<COLORREF> c = {});
		void clearrectangle(int left, int top, int right, int bottom);
		void clearrectangle(RECT rct);

		void circle(int x, int y, int radius, Optional<COLORREF> c = {});
		void fillcircle(int x, int y, int radius, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void solidcircle(int x, int y, int radius, Optional<COLORREF> c = {});
		void clearcircle(int x, int y, int radius);

		void ellipse(int left, int top, int right, int bottom, Optional<COLORREF> c = {});
		void ellipse(RECT rct, Optional<COLORREF> c = {});
		void fillellipse(int left, int top, int right, int bottom, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void fillellipse(RECT rct, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void solidellipse(int left, int top, int right, int bottom, Optional<COLORREF> c = {});
		void solidellipse(RECT rct, Optional<COLORREF> c = {});
		void clearellipse(int left, int top, int right, int bottom);
		void clearellipse(RECT rct);

		void roundrect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> c = {});
		void roundrect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> c = {});
		void fillroundrect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void fillroundrect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void solidroundrect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> c = {});
		void solidroundrect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> c = {});
		void clearroundrect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight);
		void clearroundrect(RECT rct, int ellipsewidth, int ellipseheight);

		void arc(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c = {});
		void arc(RECT rct, double stangle, double endangle, Optional<COLORREF> c = {});
		void pie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c = {});
		void pie(RECT rct, double stangle, double endangle, Optional<COLORREF> c = {});
		void fillpie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void fillpie(RECT rct, double stangle, double endangle, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void solidpie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c = {});
		void solidpie(RECT rct, double stangle, double endangle, Optional<COLORREF> c = {});
		void clearpie(int left, int top, int right, int bottom, double stangle, double endangle);
		void clearpie(RECT rct, double stangle, double endangle);

		void polyline(const POINT* points, int num, Optional<COLORREF> c = {});
		void polygon(const POINT* points, int num, Optional<COLORREF> c = {});
		void fillpolygon(const POINT* points, int num, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void solidpolygon(const POINT* points, int num, Optional<COLORREF> c = {});
		void clearpolygon(const POINT* points, int num);

		void polybezier(const POINT* points, int num, Optional<COLORREF> c = {});

		/**
		 * @brief 填充某区域
		 * @param[in] x				填充起始位置
		 * @param[in] y				填充起始位置
		 * @param[in] color			填充颜色
		 *
		 * @param[in] filltype <pre>
		 *		填充模式，有以下两种选择：
		 *		FLOODFILLBORDER		指定 color 为填充边界颜色，即遇到此颜色后停止填充
		 *		FLOODFILLSURFACE	指定 color 为填充表面颜色，即只填充此颜色
		 * </pre>
		 *
		 * @param[in] cFill			填充颜色
		*/
		void floodfill(int x, int y, COLORREF color, int filltype = FLOODFILLBORDER, Optional<COLORREF> cFill = {});

		/**
		 * @brief 在指定位置输出文本
		 * @param[in] x				位置
		 * @param[in] y				位置
		 * @param[in] lpszText		文本
		 * @param[in] c				文本颜色
		 * @return 文本像素宽度
		*/
		int outtextxy(int x, int y, LPCTSTR lpszText, Optional<COLORREF> c = {});

		int outtextxy(int x, int y, TCHAR ch, Optional<COLORREF> c = {});

		/**
		 * @brief 在指定位置输出格式化文本
		 * @param[in] x			位置
		 * @param[in] y			位置
		 * @param[in] _Size		格式化文本最大长度
		 * @param[in] _Format		格式化字符串
		 * @param[in]				不定参数
		 * @return 文本像素宽度
		*/
		int outtextxy_format(int x, int y, int _Size, LPCTSTR _Format, ...);

		/**
		 * @brief 获取文本像素宽度
		 * @param[in] lpszText 文本
		 * @return 获取文本像素宽度
		*/
		int textwidth(LPCTSTR lpszText);

		int textwidth(TCHAR c);
		int textheight(LPCTSTR lpszText);
		int textheight(TCHAR c);
		int drawtext(LPCTSTR str, RECT* pRect, UINT uFormat, Optional<COLORREF> c = {});
		int drawtext(TCHAR ch, RECT* pRect, UINT uFormat, Optional<COLORREF> c = {});

		/**
		 * @brief 在某区域居中输出文字
		 * @param[in] lpszText			文本
		 * @param[in] rct				输出区域，默认为画布区域
		 * @param[in] isSetColor		是否设置颜色
		 * @param[in] c					文本颜色
		*/
		void centertext(LPCTSTR lpszText, Optional<RECT> rct = {}, Optional<COLORREF> c = {});

		/**
		 * @brief 居中输出格式化文本
		 * @param[in] _Size			格式化文本最大长度
		 * @param[in] _Format		格式化字符串
		 * @param[in]				不定参数
		*/
		void centertext_format(int _Size, LPCTSTR _Format, ...);

		LOGFONT gettextstyle();
		void settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace);
		void settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut);
		void settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut, BYTE fbCharSet, BYTE fbOutPrecision, BYTE fbClipPrecision, BYTE fbQuality, BYTE fbPitchAndFamily);
		void settextstyle(LOGFONT font);

		/**
		 * @brief 设置字体大小
		 * @param[in] nHeight	高度
		 * @param[in] nWidth	宽度（为 0 时，自动与高度匹配）
		*/
		void setfont(int nHeight, int nWidth = 0);

		/**
		 * @brief 设置使用字体的名称
		 * @param[in] lpsz 字体名称
		*/
		void settypeface(LPCTSTR lpsz);

		/**
		 * @brief 设置字符串的书写角度（单位 0.1 度）
		 * @param[in] lfEscapement 角度
		*/
		void settextescapement(LONG lfEscapement);

		/**
		 * @brief 设置每个字符的书写角度（单位 0.1 度）
		 * @param[in] lfOrientation 角度
		*/
		void settextorientation(LONG lfOrientation);

		/**
		 * @brief 设置字符的笔画粗细（范围 默认 0 ~ 1000 最粗）
		 * @param[in] lfWeight 粗细
		*/
		void settextweight(LONG lfWeight);

		/**
		 * @brief 设置字体是否为斜体
		 * @param[in] lfItalic 是否使用斜体
		*/
		void settextitalic(bool lfItalic);

		/**
		 * @brief 设置字体是否有下划线
		 * @param[in] lfUnderline 是否使用下划线
		*/
		void settextunderline(bool lfUnderline);

		/**
		 * @brief 设置字体是否有删除线
		 * @param[in] lfStrikeOut 是否使用删除线
		*/
		void settextstrikeout(bool lfStrikeOut);

		/**
		 * @brief 获取前景色
		*/
		COLORREF getcolor();

		/**
		 * @brief 设置前景色
		 * @param[in] color 前景色
		*/
		void setcolor(COLORREF color);

		int getx();
		int gety();

		void moveto(int x, int y);
		void moverel(int dx, int dy);

		void lineto(int x, int y, Optional<COLORREF> c = {});
		void linerel(int dx, int dy, Optional<COLORREF> c = {});

		void outtext(LPCTSTR lpszText, Optional<COLORREF> c = {});
		void outtext(TCHAR ch, Optional<COLORREF> c = {});

		/**
		 * @brief 输出格式化文本
		 * @param[in] _Size			格式化文本最大长度
		 * @param[in] _Format		格式化字符串
		 * @param[in]				不定参数
		 * @return 文本像素宽度
		*/
		int outtext_format(int _Size, LPCTSTR _Format, ...);

		void loadimage(LPCTSTR pImgFile, int nWidth = 0, int nHeight = 0, bool bResize = false);
		void loadimage(LPCTSTR pResType, LPCTSTR pResName, int nWidth = 0, int nHeight = 0, bool bResize = false);

		/**
		 * @brief <pre>
		 *		加载图片文件到画布
		 *
		 *	备注：
		 *		若开启透明通道，则复制到画布上的内容不会保留原图像的透明度信息
		 * </pre>
		 *
		 * @param[in] lpszImgFile		图像文件路径
		 * @param[in] x					输出到画布的位置
		 * @param[in] y					输出到画布的位置
		 * @param[in] bResize			是否调整画布大小以正好容纳图像（对于无宽高的画布会自动调整大小）
		 * @param[in] nWidth			图像目标拉伸尺寸，为 0 表示不拉伸
		 * @param[in] nHeight			图像目标拉伸尺寸，为 0 表示不拉伸
		 * @param[in] alpha				叠加透明度
		 * @param[in] bUseSrcAlpha		是否使用原图的透明度信息进行混合（仅支持有透明度信息的 png 图像）
		 * @param[in] isCalculated		原图是否已经混合透明度
		*/
		void LoadImage_SupAlpha(
			LPCTSTR lpszImgFile,
			int x = 0,
			int y = 0,
			bool bResize = false,
			int nWidth = 0,
			int nHeight = 0,
			BYTE alpha = 255,
			bool bUseSrcAlpha = false,
			bool isCalculated = false
		);

		void saveimage(LPCTSTR pImgFile);
		void getimage(IMAGE* pDstImg, int srcX, int srcY, int srcWidth, int srcHeight);

		void putimage_in(int dstX, int dstY, const IMAGE* pSrcImg, DWORD dwRop = SRCCOPY);

		///< 可以指定裁剪区域的 putimage 函数
		void putimage_in(int dstX, int dstY, int dstWidth, int dstHeight, const IMAGE* pSrcImg, int srcX, int srcY, DWORD dwRop = SRCCOPY);

		void putimage_out(IMAGE* pDstImg, int dstX, int dstY, DWORD dwRop = SRCCOPY);
		void putimage_out(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, DWORD dwRop = SRCCOPY);

		/**
		 * @brief 绘制图像到该画布（支持 Alpha 通道）
		 * @param[in] x					图像输入位置
		 * @param[in] y					图像输入位置
		 * @param[in] pImg				待输入图像
		 * @param[in] crop				裁剪区域
		 * @param[in] alpha				叠加透明度
		 * @param[in] bUseSrcAlpha		是否使用原图透明度
		 * @param[in] isCalculated		原图是否已经混合透明度
		*/
		void PutImageIn_SupAlpha(
			int x,
			int y,
			IMAGE* pImg,
			Optional<RECT> crop = {},
			BYTE alpha = 255,
			bool bUseSrcAlpha = false,
			bool isCalculated = false
		);

		/**
		 * @brief 将该画布的图像绘制到另一画布中
		 * @param[in] x				绘制位置
		 * @param[in] y				绘制位置
		 * @param[in] pImg			目标绘制画布
		 * @param[in] crop			裁剪区域（默认不裁剪）
		 * @param[in] alpha			叠加透明度
		 * @param[in] bUseSrcAlpha	是否使用此画布透明度
		 * @param[in] isCalculated	画布像素是否已经透明混合
		*/
		void PutImageOut_SupAlpha(
			int x,
			int y,
			IMAGE* pImg = nullptr,
			Optional<RECT> crop = {},
			BYTE alpha = 255,
			bool bUseSrcAlpha = false,
			bool isCalculated = false
		);

		/////// GDI+ 相关绘图函数 ///////

		////////////////////////////////////////////////////////////////////
		//
		// 注意：
		//		GDI+ 绘图函数不和 EasyX 原生函数共享同样的绘图颜色，
		//		这是因为 GDI+ 的绘图函数支持透明，而 EasyX 原生函数不支持。
		//
		//		可以使用 RGBA 或 SET_ALPHA 宏设置带透明度的颜色
		//
		////////////////////////////////////////////////////////////////////

		void GP_SetLineColor(COLORREF color);
		void GP_SetFillColor(COLORREF color);
		void GP_SetLineWidth(float width);

		COLORREF GP_GetLineColor() const { return m_cGPLineColor; }
		COLORREF GP_GetFillColor() const { return m_cGPFillColor; }
		float GP_GetLineWidth() const { return m_fGPLineWidth; }

		/**
		 * @brief 设置 GDI+ 绘制时是否使用透明度（默认不使用）
		*/
		void GP_EnableAlpha(bool enable);

		/**
		 * @brief 设置 GDI+ 抗锯齿模式
		*/
		void GP_SetSmoothingMode(Gdiplus::SmoothingMode smoothing_mode);

		bool GP_IsEnbaleAlpha() const { return m_bGPAlpha; }
		Gdiplus::SmoothingMode GP_GetSmoothingMode() const { return m_enuSmoothingMode; }

		void GP_Line(float x1, float y1, float x2, float y2, Optional<COLORREF> linecolor = {});

		void GP_Polygon(int points_num, POINT* points, Optional<COLORREF> linecolor = {});
		void GP_SolidPolygon(int points_num, POINT* points, Optional<COLORREF> fillcolor = {});
		void GP_FillPolygon(int points_num, POINT* points, Optional<COLORREF> linecolor = {}, Optional<COLORREF> fillcolor = {});

		void GP_Rectangle(float x, float y, float w, float h, Optional<COLORREF> linecolor = {});
		void GP_SolidRectangle(float x, float y, float w, float h, Optional<COLORREF> fillcolor = {});
		void GP_FillRectangle(float x, float y, float w, float h, Optional<COLORREF> linecolor = {}, Optional<COLORREF> fillcolor = {});

		void GP_RoundRect(float x, float y, float w, float h, float ellipsewidth, float ellipseheight, Optional<COLORREF> linecolor = {});
		void GP_SolidRoundRect(float x, float y, float w, float h, float ellipsewidth, float ellipseheight, Optional<COLORREF> fillcolor = {});
		void GP_FillRoundRect(float x, float y, float w, float h, float ellipsewidth, float ellipseheight, Optional<COLORREF> linecolor = {}, Optional<COLORREF> fillcolor = {});

		void GP_Ellipse(float x, float y, float w, float h, Optional<COLORREF> linecolor = {});
		void GP_SolidEllipse(float x, float y, float w, float h, Optional<COLORREF> fillcolor = {});
		void GP_FillEllipse(float x, float y, float w, float h, Optional<COLORREF> linecolor = {}, Optional<COLORREF> fillcolor = {});

		void GP_Pie(float x, float y, float w, float h, float stangle, float endangle, Optional<COLORREF> linecolor = {});
		void GP_SolidPie(float x, float y, float w, float h, float stangle, float endangle, Optional<COLORREF> fillcolor = {});
		void GP_FillPie(float x, float y, float w, float h, float stangle, float endangle, Optional<COLORREF> linecolor = {}, Optional<COLORREF> fillcolor = {});

		void GP_Arc(float x, float y, float w, float h, float stangle, float endangle, Optional<COLORREF> linecolor = {});
	};

}


// End of file

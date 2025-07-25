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
	 * @brief 画布
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

		int GetWidth() const { return getwidth(); }
		int GetHeight() const { return getheight(); }

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

		/**
		 * @brief <pre>
		 *		用背景色清空画布
		 *
		 *	备注：
		 *		此函数将忽略背景色的透明度，并直接对画布填入 255 的透明度（即不透明）。
		 * </pre>
		*/
		void Clear(Optional<COLORREF> bkcolor = {});

		/**
		 * @brief 用背景色清空画布（区别于 Clear 函数，此函数默认保留背景色中的透明度）
		*/
		void Clear_Alpha(Optional<COLORREF> bkcolor = {}, bool ignore_alpha = false);

		LINESTYLE GetLineStyle();
		void SetLineStyle(LINESTYLE style);
		void SetLineStyle(int style, int thickness = 1, const DWORD* puserstyle = nullptr, DWORD userstylecount = 0);
		void SetLineThickness(int thickness);
		int GetLineThickness();

		FILLSTYLE GetFillStyle();
		void SetFillStyle(FILLSTYLE style);
		void SetFillStyle(int style, long hatch = 0, IMAGE* ppattern = nullptr);
		void SetFillStyle(BYTE* ppattern8x8);

		int GetRop2();
		void SetRop2(int mode);

		int GetPolyFillMode();
		void SetPolyFillMode(int mode);

		COLORREF GetLineColor();
		void SetLineColor(COLORREF color);

		COLORREF GetTextColor();
		void SetTextColor(COLORREF color);

		COLORREF GetFillColor();
		void SetFillColor(COLORREF color);

		COLORREF GetBkColor();
		void SetBkColor(COLORREF color);

		int GetBkMode();
		void SetBkMode(int mode);

		/**
		 * @brief 设置绘图状态为原始状态
		*/
		void SetDefault();

		COLORREF GetPixel(int x, int y);
		void PutPixel(int x, int y, COLORREF c);

		/**
		 * @brief  直接操作显存获取点（坐标越界会抛出异常）
		 * @throws std::out_of_range 当传入越界坐标时抛出
		*/
		COLORREF GetPixel_Direct(int x, int y);

		/**
		 * @brief 直接操作显存绘制点（坐标越界会抛出异常）
		 * @throws std::out_of_range 当传入越界坐标时抛出
		*/
		void PutPixel_Direct(int x, int y, COLORREF c);

		/**
		 * @brief 直接操作显存绘制带有透明度的点（使用 COLORREF 中的透明度）（坐标越界会抛出异常）
		 * @throws std::out_of_range 当传入越界坐标时抛出
		*/
		void PutPixel_Direct_Alpha(int x, int y, COLORREF c);

		void Line(int x1, int y1, int x2, int y2, Optional<COLORREF> c = {});
		void Line(POINT pt1, POINT pt2, Optional<COLORREF> c = {});

		void Rectangle(int left, int top, int right, int bottom, Optional<COLORREF> c = {});
		void Rectangle(RECT rct, Optional<COLORREF> c = {});
		void FillRectangle(int left, int top, int right, int bottom, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void FillRectangle(RECT rct, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void SolidRectangle(int left, int top, int right, int bottom, Optional<COLORREF> c = {});
		void SolidRectangle(RECT rct, Optional<COLORREF> c = {});
		void ClearRectangle(int left, int top, int right, int bottom);
		void ClearRectangle(RECT rct);

		void Circle(int x, int y, int radius, Optional<COLORREF> c = {});
		void FillCircle(int x, int y, int radius, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void SolidCircle(int x, int y, int radius, Optional<COLORREF> c = {});
		void ClearCircle(int x, int y, int radius);

		void Ellipse(int left, int top, int right, int bottom, Optional<COLORREF> c = {});
		void Ellipse(RECT rct, Optional<COLORREF> c = {});
		void FillEllipse(int left, int top, int right, int bottom, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void FillEllipse(RECT rct, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void SolidEllipse(int left, int top, int right, int bottom, Optional<COLORREF> c = {});
		void SolidEllipse(RECT rct, Optional<COLORREF> c = {});
		void ClearEllipse(int left, int top, int right, int bottom);
		void ClearEllipse(RECT rct);

		void RoundRect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> c = {});
		void RoundRect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> c = {});
		void FillRoundRect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void FillRoundRect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void SolidRoundRect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> c = {});
		void SolidRoundRect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> c = {});
		void ClearRoundRect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight);
		void ClearRoundRect(RECT rct, int ellipsewidth, int ellipseheight);

		void Arc(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c = {});
		void Arc(RECT rct, double stangle, double endangle, Optional<COLORREF> c = {});
		void Pie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c = {});
		void Pie(RECT rct, double stangle, double endangle, Optional<COLORREF> c = {});
		void FillPie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void FillPie(RECT rct, double stangle, double endangle, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void SolidPie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c = {});
		void SolidPie(RECT rct, double stangle, double endangle, Optional<COLORREF> c = {});
		void ClearPie(int left, int top, int right, int bottom, double stangle, double endangle);
		void ClearPie(RECT rct, double stangle, double endangle);

		void Polyline(const POINT* points, int num, Optional<COLORREF> c = {});
		void Polygon(const POINT* points, int num, Optional<COLORREF> c = {});
		void FillPolygon(const POINT* points, int num, Optional<COLORREF> cLine = {}, Optional<COLORREF> cFill = {});
		void SolidPolygon(const POINT* points, int num, Optional<COLORREF> c = {});
		void ClearPolygon(const POINT* points, int num);

		void PolyBezier(const POINT* points, int num, Optional<COLORREF> c = {});

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
		 * @param[in] isSetColor		是否设置填充颜色
		 * @param[in] cFill			填充颜色
		*/
		void FloodFill(int x, int y, COLORREF color, int filltype = FLOODFILLBORDER, Optional<COLORREF> cFill = {});

		/**
		 * @brief 在指定位置输出文本
		 * @param[in] x				位置
		 * @param[in] y				位置
		 * @param[in] lpszText			文本
		 * @param[in] isSetColor		是否设置颜色
		 * @param[in] c				文本颜色
		 * @return 文本像素宽度
		*/
		int OutTextXY(int x, int y, LPCTSTR lpszText, Optional<COLORREF> c = {});

		int OutTextXY(int x, int y, TCHAR ch, Optional<COLORREF> c = {});

		/**
		 * @brief 在指定位置输出格式化文本
		 * @param[in] x			位置
		 * @param[in] y			位置
		 * @param[in] _Size		格式化文本最大长度
		 * @param[in] _Format		格式化字符串
		 * @param[in]				不定参数
		 * @return 文本像素宽度
		*/
		int OutTextXY_Format(int x, int y, int _Size, LPCTSTR _Format, ...);

		/**
		 * @brief 获取文本像素宽度
		 * @param[in] lpszText 文本
		 * @return 获取文本像素宽度
		*/
		int TextWidth(LPCTSTR lpszText);

		int TextWidth(TCHAR c);
		int TextHeight(LPCTSTR lpszText);
		int TextHeight(TCHAR c);
		int Draw_Text(LPCTSTR str, RECT* pRect, UINT uFormat, Optional<COLORREF> c = {});
		int Draw_Text(TCHAR ch, RECT* pRect, UINT uFormat, Optional<COLORREF> c = {});

		/**
		 * @brief 在某区域居中输出文字
		 * @param[in] lpszText			文本
		 * @param[in] rct				输出区域，默认为画布区域
		 * @param[in] isSetColor		是否设置颜色
		 * @param[in] c					文本颜色
		*/
		void CenterText(LPCTSTR lpszText, Optional<RECT> rct = {}, Optional<COLORREF> c = {});

		/**
		 * @brief 居中输出格式化文本
		 * @param[in] _Size			格式化文本最大长度
		 * @param[in] _Format		格式化字符串
		 * @param[in]				不定参数
		*/
		void CenterText_Format(int _Size, LPCTSTR _Format, ...);

		LOGFONT GetTextStyle();
		void SetTextStyle(int nHeight, int nWidth, LPCTSTR lpszFace);
		void SetTextStyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut);
		void SetTextStyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut, BYTE fbCharSet, BYTE fbOutPrecision, BYTE fbClipPrecision, BYTE fbQuality, BYTE fbPitchAndFamily);
		void SetTextStyle(LOGFONT font);

		/**
		 * @brief 设置字体大小
		 * @param[in] nHeight	高度
		 * @param[in] nWidth	宽度（为 0 时，自动与高度匹配）
		*/
		void SetFont(int nHeight, int nWidth = 0);

		/**
		 * @brief 设置使用字体的名称
		 * @param[in] lpsz 字体名称
		*/
		void SetTypeface(LPCTSTR lpsz);

		/**
		 * @brief 设置字符串的书写角度（单位 0.1 度）
		 * @param[in] lfEscapement 角度
		*/
		void SetTextEscapement(LONG lfEscapement);

		/**
		 * @brief 设置每个字符的书写角度（单位 0.1 度）
		 * @param[in] lfOrientation 角度
		*/
		void SetTextOrientation(LONG lfOrientation);

		/**
		 * @brief 设置字符的笔画粗细（范围 默认 0 ~ 1000 最粗）
		 * @param[in] lfWeight 粗细
		*/
		void SetTextWeight(LONG lfWeight);

		/**
		 * @brief 设置字体是否为斜体
		 * @param[in] lfItalic 是否使用斜体
		*/
		void SetTextItalic(bool lfItalic);

		/**
		 * @brief 设置字体是否有下划线
		 * @param[in] lfUnderline 是否使用下划线
		*/
		void SetTextUnderline(bool lfUnderline);

		/**
		 * @brief 设置字体是否有删除线
		 * @param[in] lfStrikeOut 是否使用删除线
		*/
		void SetTextStrikeOut(bool lfStrikeOut);

		/**
		 * @brief 获取前景色
		*/
		COLORREF GetColor();

		/**
		 * @brief 设置前景色
		 * @param[in] color 前景色
		*/
		void SetColor(COLORREF color);

		int GetX();
		int GetY();

		void MoveTo(int x, int y);
		void MoveRel(int dx, int dy);

		void LineTo(int x, int y, Optional<COLORREF> c = {});
		void LineRel(int dx, int dy, Optional<COLORREF> c = {});

		void OutText(LPCTSTR lpszText, Optional<COLORREF> c = {});
		void OutText(TCHAR ch, Optional<COLORREF> c = {});

		/**
		 * @brief 输出格式化文本
		 * @param[in] _Size			格式化文本最大长度
		 * @param[in] _Format		格式化字符串
		 * @param[in]				不定参数
		 * @return 文本像素宽度
		*/
		int OutText_Format(int _Size, LPCTSTR _Format, ...);

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
		 * @return 读取到的 IMAGE 对象
		*/
		IMAGE Load_Image_Alpha(
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

		/**
		 * @brief 绘制图像到该画布
		 * @param[in] x					图像输入位置
		 * @param[in] y					图像输入位置
		 * @param[in] pImg				待输入图像
		*/
		void PutImageIn(
			int x,
			int y,
			IMAGE* pImg
		);

		/**
		 * @brief 绘制图像到该画布
		 * @param[in] x					图像输入位置
		 * @param[in] y					图像输入位置
		 * @param[in] pImg				待输入图像
		 * @param[in] crop				裁剪区域
		 * @param[in] alpha				叠加透明度
		 * @param[in] bUseSrcAlpha		是否使用原图透明度
		 * @param[in] isCalculated		原图是否已经混合透明度
		*/
		void PutImageIn_Alpha(
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
		void RenderTo(
			int x,
			int y,
			IMAGE* pImg = nullptr,
			Optional<RECT> crop = {},
			BYTE alpha = 255,
			bool bUseSrcAlpha = false,
			bool isCalculated = false
		);

		/**
		 * @brief EasyX 原生旋转函数
		 * @param[in] radian		旋转弧度
		 * @param[in] bkcolor		填充背景色
		 * @param[in] autosize		是否自适应旋转图像大小
		 * @param[in] highquality	高质量
		*/
		void RotateImage(double radian, COLORREF bkcolor = BLACK, bool autosize = false, bool highquality = true);

		/**
		 * @brief 旋转图像（保留 Alpha 信息）
		 * @param[in] radian	旋转弧度
		 * @param[in] bkcolor	填充背景色
		*/
		void RotateImage_Alpha(double radian, COLORREF bkcolor = BLACK);

		/**
		 * @brief 缩放图像（粗糙的、即不插值的缩放，保留透明度信息）
		 * @param[in] nW	目标宽度
		 * @param[in] nH	目标高度（为 0 则根据宽度按比例缩放）
		*/
		void ZoomImage_Rough_Alpha(int nW, int nH = 0);

		/**
		 * @brief 缩放图像（双线性插值，保留透明度信息）
		 * @param[in] nW	目标宽度
		 * @param[in] nH	目标高度（为 0 则根据宽度按比例缩放）
		*/
		void ZoomImage_Alpha(int nW, int nH = 0);

		/**
		 * @brief 缩放图像（基于 Win32 API，比较快，保留透明度信息）
		 * @param[in] nW	目标宽度
		 * @param[in] nH	目标高度（为 0 则根据宽度按比例缩放）
		*/
		void ZoomImage_StretchBlt_Alpha(int nW, int nH = 0);

		/**
		 * @brief 缩放图像（基于 Win32 API，比较快，保留透明度信息）
		 * @param[in] nW	目标宽度
		 * @param[in] nH	目标高度（为 0 则根据宽度按比例缩放）
		*/
		void ZoomImage_AlphaBlend_Alpha(int nW, int nH = 0);

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

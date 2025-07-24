#include <HiEasyX/HiCanvas.h>

#include <HiEasyX/HiMacro.h>
#include <HiEasyX/HiWindow.h>
#include <HiEasyX/HiGdiplus.h>

#include <stdexcept>

namespace HiEasyX
{
	COLORREF MixAlphaColor(COLORREF cDst, COLORREF cSrc, bool isCalculated, BYTE alpha)
	{
		float fSrc = GetAValue(cSrc) / 255.0f;	// 待绘制像素的透明度
		if (alpha != 255)						// 叠加透明度
			fSrc *= alpha / 255.0f;
		if (fSrc == 0.0f)						// 绘制透明度为 0 时不做任何处理
			return cDst;
		float fDst = 1 - fSrc;					// 原位置像素应乘透明度

		if (isCalculated)
		{
			return RGB(
				GetRValue(cSrc) + GetRValue(cDst) * fDst,
				GetGValue(cSrc) + GetGValue(cDst) * fDst,
				GetBValue(cSrc) + GetBValue(cDst) * fDst
			);
		}
		else
		{
			return RGB(
				GetRValue(cSrc) * fSrc + GetRValue(cDst) * fDst,
				GetGValue(cSrc) * fSrc + GetGValue(cDst) * fDst,
				GetBValue(cSrc) * fSrc + GetBValue(cDst) * fDst
			);
		}
	}

	void CopyImage_Alpha(int x, int y, DWORD* pDst, int wDst, int hDst, DWORD* pSrc, int wSrc, int hSrc, RECT crop, BYTE alpha, bool bUseSrcAlpha, bool isCalculated)
	{
		// 对待输出图像的裁剪起点或终点无效
		if (crop.left > wSrc || crop.top > hSrc || crop.right < crop.left || crop.bottom < crop.top)
			return;

		// 限制原图宽高
		int limit_w = ((crop.right && crop.right < wSrc) ? crop.right : wSrc);
		int limit_h = ((crop.bottom && crop.bottom < hSrc) ? crop.bottom : hSrc);

		// 标记不使用任何透明通道
		bool bNoAlpha = false;
		float fTransparent = alpha / 255.0f;
		if (alpha == 255 && !bUseSrcAlpha)	bNoAlpha = true;

		// 保证只有在使用原图透明度时，已计算标记才能开启
		if (!bUseSrcAlpha)	isCalculated = false;

		// i j -> 原图索引（若输出位置为负，则略过超出范围部分）
		// nx ny -> 载体图像索引
		for (int i = (x < 0 ? -x : 0) + crop.left, nx = i + x; i < limit_w && nx < wDst; i++, nx++)
		{
			for (int j = (y < 0 ? -y : 0) + crop.top, ny = j + y; j < limit_h && ny < hDst; j++, ny++)
			{
				int indexSrc = j * wSrc + i;
				int indexDst = ny * wDst + nx;
				if (indexSrc < 0 || indexDst < 0)
				{
					continue;
				}
				if (bNoAlpha)
				{
					pDst[indexDst] = pSrc[indexSrc];
				}
				else
				{
					pDst[indexDst] = MixAlphaColor(
						pDst[indexDst],
						bUseSrcAlpha ? pSrc[indexSrc] : SET_ALPHA(pSrc[indexSrc], 255),
						isCalculated,
						alpha
					);
				}
			}
		}
	}

	IMAGE RotateImage_Alpha(IMAGE* pImg, double radian, COLORREF bkcolor)
	{
		radian = -radian;														// 由于 y 轴翻转，旋转角度需要变负
		float fSin = (float)sin(radian), fCos = (float)cos(radian);				// 存储三角函数值
		float fNSin = (float)sin(-radian), fNCos = (float)cos(-radian);
		int left = 0, top = 0, right = 0, bottom = 0;							// 旋转后图像顶点
		int w, h;
		GetImageSize(pImg, &w, &h);
		DWORD* pBuf = GetImageBufferHX(pImg);
		POINT points[4] = { { 0, 0 },{ w, 0 },{ 0, h },{ w, h } };				// 存储图像顶点
		for (int j = 0; j < 4; j++)												// 旋转图像顶点，搜索旋转后的图像边界
		{
			points[j] = {
				(int)(points[j].x * fCos - points[j].y * fSin),
				(int)(points[j].x * fSin + points[j].y * fCos)
			};
			if (points[j].x < points[left].x)	left = j;
			if (points[j].y > points[top].y)	top = j;
			if (points[j].x > points[right].x)	right = j;
			if (points[j].y < points[bottom].y)	bottom = j;
		}

		int nw = points[right].x - points[left].x;								// 旋转后的图像尺寸
		int nh = points[top].y - points[bottom].y;
		int nSize = nw * nh;
		int offset_x = points[left].x < 0 ? points[left].x : 0;					// 旋转后图像超出第一象限的位移（据此调整图像位置）
		int offset_y = points[bottom].y < 0 ? points[bottom].y : 0;

		IMAGE img(nw, nh);
		DWORD* pNewBuf = GetImageBufferHX(&img);
		if (bkcolor != BLACK)													// 设置图像背景色
			for (int i = 0; i < nSize; i++)
				pNewBuf[i] = BGR(bkcolor);

		for (int i = offset_x, ni = 0; ni < nw; i++, ni++)						// i 用于映射原图像坐标，ni 用于定位旋转后图像坐标
		{
			for (int j = offset_y, nj = 0; nj < nh; j++, nj++)
			{
				int nx = (int)(i * fNCos - j * fNSin);							// 从旋转后的图像坐标向原图像坐标映射
				int ny = (int)(i * fNSin + j * fNCos);
				if (nx >= 0 && nx < w && ny >= 0 && ny < h)						// 若目标映射在原图像范围内，则拷贝色值
					pNewBuf[nj * nw + ni] = pBuf[ny * w + nx];
			}
		}

		return img;
	}

	IMAGE ZoomImage_Rough_Alpha(IMAGE* srcimg, int width, int height)
	{
		int src_width, src_height;
		GetImageSize(srcimg, &src_width, &src_height);

		// 自动缩放
		if (height == 0)
		{
			height = width * src_height / src_width;
		}

		IMAGE dstImage(width, height);
		IMAGE* dstimg = &dstImage;

		DWORD* dst = GetImageBufferHX(dstimg);
		DWORD* src = GetImageBufferHX(srcimg);

		float w_scale_rate = (float)src_width / width;
		float h_scale_rate = (float)src_height / height;

		for (int iy = 0; iy < height; iy++)
		{
			for (int ix = 0; ix < width; ix++)
			{
				// 计算原图相应坐标
				int x_scale = (int)(w_scale_rate * ix);
				int y_scale = (int)(h_scale_rate * iy);
				dst[ix + iy * width] = src[(int)(x_scale + y_scale * src_width)];
			}
		}

		return dstImage;
	}

	IMAGE ZoomImage_Alpha(IMAGE* srcimg, int width, int height)
	{
		int old_w, old_h;
		GetImageSize(srcimg, &old_w, &old_h);

		// 自动缩放
		if (height == 0)
		{
			height = width * old_h / old_w;
		}

		IMAGE output(width, height);
		DWORD* src = GetImageBufferHX(srcimg);
		DWORD* dst = GetImageBufferHX(&output);

		// 双线性插值（因为向下取样，所以 w, h 都要减一避免越界）
		for (int i = 0; i < height - 1; i++)
		{
			for (int j = 0; j < width - 1; j++)
			{
				int xt = j * old_w / width;		// 新图坐标映射到原图上的位置
				int yt = i * old_h / height;

				// 实现逐行加载图片
				byte r = (
					GetRValue(src[xt + yt * old_w]) +
					GetRValue(src[xt + yt * old_w + 1]) +
					GetRValue(src[xt + (yt + 1) * old_w]) +
					GetRValue(src[xt + (yt + 1) * old_w + 1])) / 4;
				byte g = (
					GetGValue(src[xt + yt * old_w]) +
					GetGValue(src[xt + yt * old_w] + 1) +
					GetGValue(src[xt + (yt + 1) * old_w]) +
					GetGValue(src[xt + (yt + 1) * old_w]) + 1) / 4;
				byte b = (
					GetBValue(src[xt + yt * old_w]) +
					GetBValue(src[xt + yt * old_w] + 1) +
					GetBValue(src[xt + (yt + 1) * old_w]) +
					GetBValue(src[xt + (yt + 1) * old_w + 1])) / 4;

				// 赋值，保留 alpha
				dst[i * width + j] = ARGB(GetAValue(src[xt + yt * old_w]), r, g, b);
			}
		}

		return output;
	}

	IMAGE ZoomImage_StretchBlt_Alpha(IMAGE* srcimg, int width, int height)
	{
		int w, h;
		GetImageSize(srcimg, &w, &h);

		// 自动缩放
		if (height == 0)
		{
			height = width * h / w;
		}

		IMAGE img(width, height);
		StretchBlt(
			GetImageHDCHX(&img), 0, 0, width, height,
			GetImageHDCHX(srcimg), 0, 0,
			w, h, SRCCOPY
		);
		return img;
	}

	void Canvas::CleanUpSettings()
	{
		//m_property = {};
		m_pImgPrevious = nullptr;
		m_pBuf = nullptr;
		//getwidth() = 0;
		//getheight() = 0;
		//m_nBufSize = 0;
		//m_bBindToImgPointer = false;
		//m_pImg = nullptr;
		//m_bBatchDraw = false;
		//m_hBindWindow = nullptr;
	}

	//bool Canvas::BeginWindowTask()
	//{
	//	if (m_hBindWindow)
	//	{
	//		if (HiEasyX::SetWorkingWindow(m_hBindWindow))
	//		{
	//			if (HiEasyX::BeginTask())
	//			{
	//				return true;
	//			}
	//		}
	//		return false;
	//	}
	//	else
	//	{
	//		// 内部总是调用此函数，故未绑定窗口时直接返回 true
	//		return true;
	//	}
	//}

	//void Canvas::EndWindowTask()
	//{
	//	if (m_hBindWindow)
	//	{
	//		HiEasyX::EndTask(m_bAutoMarkFlushWindow);
	//	}
	//}

	bool Canvas::BeginDrawing()
	{
		/*IMAGE* p = GetImagePointer();
		if (GetWorkingImage() != p)
		{
			m_property.SaveWorkingImageOnly();
			SetWorkingImageHX(p);
		}
		return BeginWindowTask();*/

		m_pImgPrevious = GetWorkingImage();
		SetWorkingImageHX(this);
		return true;
	}

	void Canvas::EndDrawing()
	{
		/*if (!m_bBatchDraw)
		{
			if (m_property.IsSaved())
			{
				m_property.ApplyWorkingImageOnly();
				m_property.Reset();
			}

			EndWindowTask();
		}*/

		SetWorkingImageHX(m_pImgPrevious);
	}

	Canvas::Canvas()
	{
		CleanUpSettings();
		//UpdateSizeInfo();
	}

	Canvas::Canvas(int w, int h, COLORREF cBk)
	{
		CleanUpSettings();
		Resize(w, h);
		SetBkColor(cBk);
		Clear();
	}

	Canvas::Canvas(IMAGE* pImg)
	{
		operator=(pImg);
	}

	Canvas::Canvas(IMAGE img)
	{
		operator=(img);
	}

	Canvas& Canvas::operator= (IMAGE* pImg)
	{
		CleanUpSettings();
		int w, h;
		GetImageSize(pImg, &w, &h);

		//if (BeginWindowTask())
		//{
		Resize(w, h);
		//	UpdateSizeInfo();

		// 不使用 IMAGE::operator=，因为它不支持指针
		memcpy(m_pBuf, GetImageBufferHX(pImg), GetBufferSize() * sizeof(COLORREF));

		//	EndWindowTask();
		//}

		return *this;
	}

	Canvas& Canvas::operator= (IMAGE img)
	{
		return operator=(&img);
	}

	/*void Canvas::UpdateSizeInfo()
	{
		m_pBuf = GetImageBufferHX(GetImagePointer());

		if (m_bBindToImgPointer)
		{
			GetImageSize(m_pImg, &getwidth(), &getheight());
		}
		else
		{
			getwidth() = IMAGE::getwidth();
			getheight() = IMAGE::getheight();
		}

		m_nBufSize = getwidth() * getheight();
	}*/

	void Canvas::Resize(int w, int h)
	{
		/*if (BeginWindowTask())
		{
			if (!(w * h))
			{
				w = h = 1;
			}

			if (m_bBindToImgPointer)
			{
				::Resize(m_pImg, w, h);
			}
			else
			{
				IMAGE::Resize(w, h);
			}

			EndWindowTask();
			UpdateSizeInfo();
		}*/

		IMAGE::Resize(w, h);
		m_pBuf = GetImageBufferHX(this);
		m_hdc = GetImageHDCHX(this);
	}

	/*Canvas& Canvas::BindToImage(IMAGE* pImg)
	{
		CleanUpSettings();
		m_bBindToImgPointer = true;
		m_pImg = pImg;
		UpdateSizeInfo();
		return *this;
	}

	Canvas& Canvas::BindToWindow(HWND hWnd, IMAGE* pImg)
	{
		BindToImage(pImg);
		m_hBindWindow = hWnd;
		return *this;
	}

	void Canvas::EnableAutoMarkFlushWindow(bool enable)
	{
		m_bAutoMarkFlushWindow = enable;
	}*/

	/*void Canvas::BeginBatchDrawing()
	{
		if (BeginDrawing())
		{
			m_bBatchDraw = true;
		}
	}

	void Canvas::EndBatchDrawing()
	{
		if (m_bBatchDraw)
		{
			m_bBatchDraw = false;
			EndDrawing();
		}
	}*/

	bool Canvas::IsPointInside(int x, int y, int* pIndex)
	{
		if (x < 0 || y < 0 || x >= getwidth() || y >= getheight())
			return false;
		if (pIndex)	*pIndex = y * getwidth() + x;
		return true;
	}

	void Canvas::RenderTo(int x, int y, IMAGE* pImg, RECT crop, BYTE alpha, bool bUseSrcAlpha, bool isCalculated)
	{
		int w, h;	// 目标输出画布尺寸
		GetImageSize(pImg, &w, &h);
		//if (BeginWindowTask())
		//{
		CopyImage_Alpha(
			x, y,
			GetImageBufferHX(pImg), w, h,
			m_pBuf, getwidth(), getheight(),
			crop,
			alpha, bUseSrcAlpha, isCalculated
		);

		//	EndWindowTask();
		//}
	}

	LINESTYLE Canvas::GetLineStyle()
	{
		if (BeginDrawing())
		{
			LINESTYLE s;
			getlinestyle(&s);
			EndDrawing();
			return s;
		}
		return {};
	}

	void Canvas::Clear(Optional<COLORREF> bkcolor)
	{
		Clear_Alpha(bkcolor, true);
	}

	void Canvas::Clear_Alpha(Optional<COLORREF> bkcolor, bool ignore_alpha)
	{
		if (BeginDrawing())
		{
			if (bkcolor.isSet)	setbkcolor(bkcolor);
			DWORD bk_bgr = BGR(getbkcolor());

			if (ignore_alpha)		// 设置背景透明度为 255（不透明）
				bk_bgr |= 0xFF000000;

			for (int i = 0; i < GetBufferSize(); i++)
				m_pBuf[i] = bk_bgr;

			EndDrawing();
		}
	}

	void Canvas::SetLineStyle(LINESTYLE style)
	{
		if (BeginDrawing())
		{
			setlinestyle(&style);
			EndDrawing();
		}
	}

	void Canvas::SetLineStyle(int style, int thickness, const DWORD* puserstyle, DWORD userstylecount)
	{
		if (BeginDrawing())
		{
			setlinestyle(style, thickness, puserstyle, userstylecount);
			EndDrawing();
		}
	}

	void Canvas::SetLineThickness(int thickness)
	{
		LINESTYLE s = GetLineStyle();
		s.thickness = thickness;
		SetLineStyle(s);
	}

	int Canvas::GetLineThickness()
	{
		return GetLineStyle().thickness;
	}

	FILLSTYLE Canvas::GetFillStyle()
	{
		if (BeginDrawing())
		{
			FILLSTYLE s;
			getfillstyle(&s);
			EndDrawing();
			return s;
		}
		return {};
	}

	void Canvas::SetFillStyle(FILLSTYLE style)
	{
		if (BeginDrawing())
		{
			setfillstyle(&style);
			EndDrawing();
		}
	}

	void Canvas::SetFillStyle(int style, long hatch, IMAGE* ppattern)
	{
		if (BeginDrawing())
		{
			setfillstyle(style, hatch, ppattern);
			EndDrawing();
		}
	}

	void Canvas::SetFillStyle(BYTE* ppattern8x8)
	{
		if (BeginDrawing())
		{
			setfillstyle(ppattern8x8);
			EndDrawing();
		}
	}

	int Canvas::GetRop2()
	{
		if (BeginDrawing())
		{
			int r = getrop2();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::SetRop2(int mode)
	{
		if (BeginDrawing())
		{
			setrop2(mode);
			EndDrawing();
		}
	}

	int Canvas::GetPolyFillMode()
	{
		if (BeginDrawing())
		{
			int r = getpolyfillmode();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::SetPolyFillMode(int mode)
	{
		if (BeginDrawing())
		{
			setpolyfillmode(mode);
			EndDrawing();
		}
	}

	void Canvas::SetDefault()
	{
		if (BeginDrawing())
		{
			graphdefaults();
			EndDrawing();
		}
	}

	COLORREF Canvas::GetLineColor()
	{
		if (BeginDrawing())
		{
			COLORREF r = getlinecolor();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::SetLineColor(COLORREF color)
	{
		if (BeginDrawing())
		{
			setlinecolor(color);
			EndDrawing();
		}
	}

	COLORREF Canvas::GetTextColor()
	{
		if (BeginDrawing())
		{
			COLORREF r = gettextcolor();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::SetTextColor(COLORREF color)
	{
		if (BeginDrawing())
		{
			settextcolor(color);
			EndDrawing();
		}
	}

	COLORREF Canvas::GetFillColor()
	{
		if (BeginDrawing())
		{
			COLORREF r = getfillcolor();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::SetFillColor(COLORREF color)
	{
		if (BeginDrawing())
		{
			setfillcolor(color);
			EndDrawing();
		}
	}

	COLORREF Canvas::GetBkColor()
	{
		if (BeginDrawing())
		{
			COLORREF r = getbkcolor();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::SetBkColor(COLORREF color)
	{
		if (BeginDrawing())
		{
			setbkcolor(color);
			EndDrawing();
		}
	}

	int Canvas::GetBkMode()
	{
		if (BeginDrawing())
		{
			COLORREF r = getbkmode();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::SetBkMode(int mode)
	{
		if (BeginDrawing())
		{
			setbkmode(mode);
			EndDrawing();
		}
	}

	COLORREF Canvas::GetPixel(int x, int y)
	{
		if (BeginDrawing())
		{
			COLORREF c = getpixel(x, y);
			EndDrawing();
			return c;
		}
		return 0;
	}

	void Canvas::PutPixel(int x, int y, COLORREF c)
	{
		if (BeginDrawing())
		{
			putpixel(x, y, c);
			EndDrawing();
		}
	}

	COLORREF Canvas::GetPixel_Direct(int x, int y)
	{
		//if (BeginWindowTask())
		//{
		int index = 0;
		if (IsPointInside(x, y, &index))
		{
			return BGR(m_pBuf[index]);
		}
		else
		{
			throw std::out_of_range("Coordinates out of bounds.");
		}
		//EndWindowTask();
		//return c;
		//}
		//return 0;
	}

	void Canvas::PutPixel_Direct(int x, int y, COLORREF c)
	{
		//if (BeginWindowTask())
		//{
		int index = 0;
		if (IsPointInside(x, y, &index))
		{
			m_pBuf[index] = BGR(c);
		}
		else
		{
			throw std::out_of_range("Coordinates out of bounds.");
		}

		//EndWindowTask();
		//}
	}

	void Canvas::PutPixel_Direct_Alpha(int x, int y, COLORREF c)
	{
		//if (BeginWindowTask())
		//{
		int index = 0;
		if (IsPointInside(x, y, &index))
		{
			m_pBuf[index] = MixAlphaColor(m_pBuf[index], BGR(c), false);
		}
		else
		{
			throw std::out_of_range("Coordinates out of bounds.");
		}
		//EndWindowTask();
		//}
	}

	void Canvas::Line(int x1, int y1, int x2, int y2, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setlinecolor(c);
			line(x1, y1, x2, y2);
			EndDrawing();
		}
	}

	void Canvas::Line(POINT pt1, POINT pt2, Optional<COLORREF> c)
	{
		Line(pt1.x, pt1.y, pt2.x, pt2.y, c);
	}

	void Canvas::Rectangle(int left, int top, int right, int bottom, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setlinecolor(c);
			rectangle(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::Rectangle(RECT rct, Optional<COLORREF> c)
	{
		Rectangle(rct.left, rct.top, rct.right, rct.bottom, c);
	}

	void Canvas::FillRectangle(int left, int top, int right, int bottom, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	setlinecolor(cLine);
			if (cFill.isSet)	setfillcolor(cFill);


			fillrectangle(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::FillRectangle(RECT rct, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		FillRectangle(rct.left, rct.top, rct.right, rct.bottom, cLine, cFill);
	}

	void Canvas::SolidRectangle(int left, int top, int right, int bottom, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setfillcolor(c);
			solidrectangle(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::SolidRectangle(RECT rct, Optional<COLORREF> c)
	{
		SolidRectangle(rct.left, rct.top, rct.right, rct.bottom, c);
	}

	void Canvas::ClearRectangle(int left, int top, int right, int bottom)
	{
		if (BeginDrawing())
		{
			clearrectangle(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::ClearRectangle(RECT rct)
	{
		ClearRectangle(rct.left, rct.top, rct.right, rct.bottom);
	}

	void Canvas::Circle(int x, int y, int radius, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setlinecolor(c);
			circle(x, y, radius);
			EndDrawing();
		}
	}

	void Canvas::FillCircle(int x, int y, int radius, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	setlinecolor(cLine);
			if (cFill.isSet)	setfillcolor(cFill);

			fillcircle(x, y, radius);
			EndDrawing();
		}
	}

	void Canvas::SolidCircle(int x, int y, int radius, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setfillcolor(c);
			solidcircle(x, y, radius);
			EndDrawing();
		}
	}

	void Canvas::ClearCircle(int x, int y, int radius)
	{
		if (BeginDrawing())
		{
			clearcircle(x, y, radius);
			EndDrawing();
		}
	}

	void Canvas::Ellipse(int left, int top, int right, int bottom, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setlinecolor(c);
			ellipse(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::Ellipse(RECT rct, Optional<COLORREF> c)
	{
		Ellipse(rct.left, rct.top, rct.right, rct.bottom, c);
	}

	void Canvas::FillEllipse(int left, int top, int right, int bottom, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	setlinecolor(cLine);
			if (cFill.isSet)	setfillcolor(cFill);

			fillellipse(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::FillEllipse(RECT rct, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		FillEllipse(rct.left, rct.top, rct.right, rct.bottom, cLine, cFill);
	}

	void Canvas::SolidEllipse(int left, int top, int right, int bottom, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setfillcolor(c);
			solidellipse(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::SolidEllipse(RECT rct, Optional<COLORREF> c)
	{
		SolidEllipse(rct.left, rct.top, rct.right, rct.bottom, c);
	}

	void Canvas::ClearEllipse(int left, int top, int right, int bottom)
	{
		if (BeginDrawing())
		{
			clearellipse(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::ClearEllipse(RECT rct)
	{
		ClearEllipse(rct.left, rct.top, rct.right, rct.bottom);
	}

	void Canvas::RoundRect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setlinecolor(c);
			roundrect(left, top, right, bottom, ellipsewidth, ellipseheight);
			EndDrawing();
		}
	}

	void Canvas::RoundRect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> c)
	{
		RoundRect(rct.left, rct.top, rct.right, rct.bottom, ellipsewidth, ellipseheight, c);
	}

	void Canvas::FillRoundRect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	setlinecolor(cLine);
			if (cFill.isSet)	setfillcolor(cFill);

			fillroundrect(left, top, right, bottom, ellipsewidth, ellipseheight);
			EndDrawing();
		}
	}

	void Canvas::FillRoundRect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		FillRoundRect(rct.left, rct.top, rct.right, rct.bottom, ellipsewidth, ellipseheight, cLine, cFill);
	}

	void Canvas::SolidRoundRect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setfillcolor(c);
			solidroundrect(left, top, right, bottom, ellipsewidth, ellipseheight);
			EndDrawing();
		}
	}

	void Canvas::SolidRoundRect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> c)
	{
		SolidRoundRect(rct.left, rct.top, rct.right, rct.bottom, ellipsewidth, ellipseheight, c);
	}

	void Canvas::ClearRoundRect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight)
	{
		if (BeginDrawing())
		{
			clearroundrect(left, top, right, bottom, ellipsewidth, ellipseheight);
			EndDrawing();
		}
	}

	void Canvas::ClearRoundRect(RECT rct, int ellipsewidth, int ellipseheight)
	{
		ClearRoundRect(rct.left, rct.top, rct.right, rct.bottom, ellipsewidth, ellipseheight);
	}

	void Canvas::Arc(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setlinecolor(c);
			arc(left, top, right, bottom, stangle, endangle);
			EndDrawing();
		}
	}

	void Canvas::Arc(RECT rct, double stangle, double endangle, Optional<COLORREF> c)
	{
		Arc(rct.left, rct.top, rct.right, rct.bottom, stangle, endangle, c);
	}

	void Canvas::Pie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setlinecolor(c);
			pie(left, top, right, bottom, stangle, endangle);
			EndDrawing();
		}
	}

	void Canvas::Pie(RECT rct, double stangle, double endangle, Optional<COLORREF> c)
	{
		Pie(rct.left, rct.top, rct.right, rct.bottom, stangle, endangle, c);
	}

	void Canvas::FillPie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	setlinecolor(cLine);
			if (cFill.isSet)	setfillcolor(cFill);

			fillpie(left, top, right, bottom, stangle, endangle);
			EndDrawing();
		}
	}

	void Canvas::FillPie(RECT rct, double stangle, double endangle, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		FillPie(rct.left, rct.top, rct.right, rct.bottom, stangle, endangle, cLine, cFill);
	}

	void Canvas::SolidPie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setfillcolor(c);
			solidpie(left, top, right, bottom, stangle, endangle);
			EndDrawing();
		}
	}

	void Canvas::SolidPie(RECT rct, double stangle, double endangle, Optional<COLORREF> c)
	{
		SolidPie(rct.left, rct.top, rct.right, rct.bottom, stangle, endangle, c);
	}

	void Canvas::ClearPie(int left, int top, int right, int bottom, double stangle, double endangle)
	{
		if (BeginDrawing())
		{
			clearpie(left, top, right, bottom, stangle, endangle);
			EndDrawing();
		}
	}

	void Canvas::ClearPie(RECT rct, double stangle, double endangle)
	{
		ClearPie(rct.left, rct.top, rct.right, rct.bottom, stangle, endangle);
	}

	void Canvas::Polyline(const POINT* points, int num, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setlinecolor(c);
			polyline(points, num);
			EndDrawing();
		}
	}

	void Canvas::Polygon(const POINT* points, int num, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setlinecolor(c);
			polygon(points, num);
			EndDrawing();
		}
	}

	void Canvas::FillPolygon(const POINT* points, int num, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	setlinecolor(cLine);
			if (cFill.isSet)	setfillcolor(cFill);

			fillpolygon(points, num);
			EndDrawing();
		}
	}

	void Canvas::SolidPolygon(const POINT* points, int num, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setfillcolor(c);
			solidpolygon(points, num);
			EndDrawing();
		}
	}

	void Canvas::ClearPolygon(const POINT* points, int num)
	{
		if (BeginDrawing())
		{
			clearpolygon(points, num);
			EndDrawing();
		}
	}

	void Canvas::PolyBezier(const POINT* points, int num, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setlinecolor(c);
			polybezier(points, num);
			EndDrawing();
		}
	}

	void Canvas::FloodFill(int x, int y, COLORREF color, int filltype, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cFill.isSet)	setfillcolor(cFill);
			floodfill(x, y, color, filltype);
			EndDrawing();
		}
	}

	int Canvas::OutTextXY(int x, int y, LPCTSTR lpszText, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	settextcolor(c);
			outtextxy(x, y, lpszText);
			int width = textwidth(lpszText);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::OutTextXY(int x, int y, TCHAR ch, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	settextcolor(c);
			outtextxy(x, y, ch);
			int width = textwidth(ch);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::OutTextXY_Format(int x, int y, int _Size, LPCTSTR _Format, ...)
	{
		if (BeginDrawing())
		{

			va_list list;
			va_start(list, _Format);
			TCHAR* buf = new TCHAR[_Size];

#ifdef _MSC_VER
			t_vsnprintf_s(buf, _Size, _TRUNCATE, _Format, list);
#else
			t_vsnprintf(buf, _Size, _Format, list);
#endif

			va_end(list);

			outtextxy(x, y, buf);
			int width = textwidth(buf);
			delete[] buf;

			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::TextWidth(LPCTSTR lpszText)
	{
		if (BeginDrawing())
		{
			int width = textwidth(lpszText);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::TextWidth(TCHAR c)
	{
		if (BeginDrawing())
		{
			int width = textwidth(c);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::TextHeight(LPCTSTR lpszText)
	{
		if (BeginDrawing())
		{
			int width = textheight(lpszText);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::TextHeight(TCHAR c)
	{
		if (BeginDrawing())
		{
			int width = textheight(c);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::Draw_Text(LPCTSTR str, RECT* pRect, UINT uFormat, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	settextcolor(c);
			int width = drawtext(str, pRect, uFormat);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::Draw_Text(TCHAR ch, RECT* pRect, UINT uFormat, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	settextcolor(c);
			int width = drawtext(ch, pRect, uFormat);
			EndDrawing();
			return width;
		}
		return 0;
	}

	void Canvas::CenterText(LPCTSTR lpszText, Optional<RECT> rct, Optional<COLORREF> c)
	{
		if (!rct.isSet)	rct = RECT{ 0,0,getwidth(),getheight() };
		Draw_Text(lpszText, &rct.value, DT_CENTER | DT_VCENTER | DT_SINGLELINE, c);
	}

	void Canvas::CenterText_Format(int _Size, LPCTSTR _Format, ...)
	{

		va_list list;
		va_start(list, _Format);
		TCHAR* buf = new TCHAR[_Size];

#ifdef _MSC_VER
		t_vsnprintf_s(buf, _Size, _TRUNCATE, _Format, list);
#else
		t_vsnprintf(buf, _Size, _Format, list);
#endif

		va_end(list);

		CenterText(buf);
		delete[] buf;
	}

	LOGFONT Canvas::GetTextStyle()
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			gettextstyle(&font);
			EndDrawing();
			return font;
		}
		return {};
	}

	void Canvas::SetTextStyle(int nHeight, int nWidth, LPCTSTR lpszFace)
	{
		if (BeginDrawing())
		{
			settextstyle(nHeight, nWidth, lpszFace);
			EndDrawing();
		}
	}

	void Canvas::SetTextStyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut)
	{
		if (BeginDrawing())
		{
			settextstyle(nHeight, nWidth, lpszFace, nEscapement, nOrientation, nWeight, bItalic, bUnderline, bStrikeOut);
			EndDrawing();
		}
	}

	void Canvas::SetTextStyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut, BYTE fbCharSet, BYTE fbOutPrecision, BYTE fbClipPrecision, BYTE fbQuality, BYTE fbPitchAndFamily)
	{
		if (BeginDrawing())
		{
			settextstyle(nHeight, nWidth, lpszFace, nEscapement, nOrientation, nWeight, bItalic, bUnderline, bStrikeOut, fbCharSet, fbOutPrecision, fbClipPrecision, fbQuality, fbPitchAndFamily);
			EndDrawing();
		}
	}

	void Canvas::SetTextStyle(LOGFONT font)
	{
		if (BeginDrawing())
		{
			settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::SetFont(int nHeight, int nWidth)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			gettextstyle(&font);
			font.lfHeight = nHeight;
			font.lfWidth = nWidth;
			settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::SetTypeface(LPCTSTR lpsz)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			gettextstyle(&font);
			lstrcpy(font.lfFaceName, lpsz);
			settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::SetTextEscapement(LONG lfEscapement)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			gettextstyle(&font);
			font.lfEscapement = lfEscapement;
			settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::SetTextOrientation(LONG lfOrientation)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			gettextstyle(&font);
			font.lfOrientation = lfOrientation;
			settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::SetTextWeight(LONG lfWeight)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			gettextstyle(&font);
			font.lfWeight = lfWeight;
			settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::SetTextItalic(bool lfItalic)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			gettextstyle(&font);
			font.lfItalic = lfItalic;
			settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::SetTextUnderline(bool lfUnderline)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			gettextstyle(&font);
			font.lfUnderline = lfUnderline;
			settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::SetTextStrikeOut(bool lfStrikeOut)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			gettextstyle(&font);
			font.lfStrikeOut = lfStrikeOut;
			settextstyle(&font);
			EndDrawing();
		}
	}

	COLORREF Canvas::GetColor()
	{
		if (BeginDrawing())
		{
			int r = getcolor();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::SetColor(COLORREF color)
	{
		if (BeginDrawing())
		{
			setcolor(color);
			EndDrawing();
		}
	}

	int Canvas::GetX()
	{
		if (BeginDrawing())
		{
			int r = getx();
			EndDrawing();
			return r;
		}
		return 0;
	}

	int Canvas::GetY()
	{
		if (BeginDrawing())
		{
			int r = gety();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::MoveTo(int x, int y)
	{
		if (BeginDrawing())
		{
			moveto(x, y);
			EndDrawing();
		}
	}

	void Canvas::MoveRel(int dx, int dy)
	{
		if (BeginDrawing())
		{
			moverel(dx, dy);
			EndDrawing();
		}
	}

	void Canvas::LineTo(int x, int y, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setcolor(c);
			lineto(x, y);
			EndDrawing();
		}
	}

	void Canvas::LineRel(int dx, int dy, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setcolor(c);
			linerel(dx, dy);
			EndDrawing();
		}
	}

	void Canvas::OutText(LPCTSTR lpszText, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setcolor(c);
			outtext(lpszText);
			EndDrawing();
		}
	}

	void Canvas::OutText(TCHAR ch, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	setcolor(c);
			outtext(ch);
			EndDrawing();
		}
	}

	int Canvas::OutText_Format(int _Size, LPCTSTR _Format, ...)
	{
		if (BeginDrawing())
		{

			va_list list;
			va_start(list, _Format);
			TCHAR* buf = new TCHAR[_Size];

#ifdef _MSC_VER
			t_vsnprintf_s(buf, _Size, _TRUNCATE, _Format, list);
#else
			t_vsnprintf(buf, _Size, _Format, list);
#endif
			va_end(list);

			outtext(buf);
			int width = textwidth(buf);
			delete[] buf;

			EndDrawing();
			return width;
		}
		return 0;
	}

	IMAGE Canvas::Load_Image_Alpha(LPCTSTR lpszImgFile, int x, int y, bool bResize, int nWidth, int nHeight, BYTE alpha, bool bUseSrcAlpha, bool isCalculated)
	{
		// 标记到底要不要调整图像大小
		bool resize_flag = bResize;
		if (getwidth() == 0 && getheight() == 0)
		{
			resize_flag = true;
		}

		// 如果超出画布范围而又不调整图像大小，则直接返回
		if (!resize_flag && (x > getwidth() || y > getheight()))	return {};

		// 加载图像
		IMAGE img;
		loadimage(&img, lpszImgFile, nWidth, nHeight, true);
		int w = img.getwidth(), h = img.getheight();

		// 如果图像在左侧或上方越界，那么不用输出了
		if (x <= -w || y <= -h)	return {};

		// 调整图像大小
		if (resize_flag)
		{
			int nw = x + w;
			int nh = y + h;
			Resize(nw, nh);
		}

		// 输出图像
		PutImageIn_Alpha(x, y, &img, { 0 }, alpha, bUseSrcAlpha, isCalculated);
		return img;
	}

	void Canvas::PutImageIn_Alpha(int x, int y, IMAGE* pImg, RECT crop, BYTE alpha, bool bUseSrcAlpha, bool isCalculated)
	{
		int w, h;	// 原图像尺寸
		GetImageSize(pImg, &w, &h);
		//if (BeginWindowTask())
		//{
		CopyImage_Alpha(
			x, y,
			m_pBuf, getwidth(), getheight(),
			GetImageBufferHX(pImg), w, h,
			crop,
			alpha, bUseSrcAlpha, isCalculated
		);

		//EndWindowTask();
		//}
	}

	void Canvas::RotateImage(double radian, COLORREF bkcolor, bool autosize, bool highquality)
	{
		//if (BeginWindowTask())
		//{
		//	IMAGE* p = GetImagePointer();
		rotateimage(this, this, radian, bkcolor, autosize, highquality);
		//BeginWindowTask();
		//}
	}

	void Canvas::RotateImage_Alpha(double radian, COLORREF bkcolor)
	{
		operator=(HiEasyX::RotateImage_Alpha(this, radian, bkcolor));
	}

	void Canvas::ZoomImage_Rough_Alpha(int nW, int nH)
	{
		/*if (m_bBindToImgPointer)
		{
			if (BeginWindowTask())
			{
				*m_pImg = HiEasyX::ZoomImage_Rough_Alpha(m_pImg, nW, nH);
				UpdateSizeInfo();

				EndWindowTask();
			}
		}
		else
		{*/
		operator=(HiEasyX::ZoomImage_Rough_Alpha(this, nW, nH));
		//}
	}

	void Canvas::ZoomImage_Alpha(int nW, int nH)
	{
		/*if (m_bBindToImgPointer)
		{
			if (BeginWindowTask())
			{
				*m_pImg = HiEasyX::ZoomImage_Alpha(m_pImg, nW, nH);
				UpdateSizeInfo();

				EndWindowTask();
			}
		}
		else
		{*/
		operator=(HiEasyX::ZoomImage_Alpha(this, nW, nH));
		//}
	}

	void Canvas::ZoomImage_StretchBlt_Alpha(int nW, int nH)
	{
		/*if (m_bBindToImgPointer)
		{
			if (BeginWindowTask())
			{
				*m_pImg = HiEasyX::ZoomImage_StretchBlt_Alpha(m_pImg, nW, nH);
				UpdateSizeInfo();
			}
		}
		else
		{*/
		operator=(HiEasyX::ZoomImage_StretchBlt_Alpha(this, nW, nH));
		//}
	}

	void Canvas::ZoomImage_AlphaBlend_Alpha(int nW, int nH)
	{
		/*if (m_bBindToImgPointer)
		{
			if (BeginWindowTask())
			{
				*m_pImg = HiEasyX::ZoomImage_AlphaBlend_Alpha(m_pImg, nW, nH);
				UpdateSizeInfo();
			}
		}
		else
		{*/
		operator=(HiEasyX::ZoomImage_AlphaBlend_Alpha(this, nW, nH));
		//}
	}

	void Canvas::GP_SetLineColor(COLORREF color)
	{
		m_cGPLineColor = color;
	}

	void Canvas::GP_SetFillColor(COLORREF color)
	{
		m_cGPFillColor = color;
	}

	void Canvas::GP_SetLineWidth(float width)
	{
		m_fGPLineWidth = width;
	}

	void Canvas::GP_EnableAlpha(bool enable)
	{
		m_bGPAlpha = enable;
	}

	void Canvas::GP_SetSmoothingMode(Gdiplus::SmoothingMode smoothing_mode)
	{
		m_enuSmoothingMode = smoothing_mode;
	}

	void Canvas::GP_Line(float x1, float y1, float x2, float y2, Optional<COLORREF> linecolor)
	{
		//if (BeginWindowTask())
		//{
		if (linecolor.isSet)	GP_SetLineColor(linecolor);
		EasyX_Gdiplus_Line(x1, y1, x2, y2, m_cGPLineColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);
		//EndWindowTask();
		//}
	}

	void Canvas::GP_Polygon(int points_num, POINT* points, Optional<COLORREF> linecolor)
	{
		//if (BeginWindowTask())
		//{

		if (linecolor.isSet)	GP_SetLineColor(linecolor);
		EasyX_Gdiplus_Polygon(points_num, points, m_cGPLineColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_SolidPolygon(int points_num, POINT* points, Optional<COLORREF> fillcolor)
	{
		//if (BeginWindowTask())
		//{

		if (fillcolor.isSet)	GP_SetFillColor(fillcolor);
		EasyX_Gdiplus_SolidPolygon(points_num, points, m_cGPFillColor, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_FillPolygon(int points_num, POINT* points, Optional<COLORREF> linecolor, Optional<COLORREF> fillcolor)
	{
		//if (BeginWindowTask())
	//{

		if (linecolor.isSet)			GP_SetLineColor(linecolor);
		if (fillcolor.isSet)			GP_SetFillColor(fillcolor);

		EasyX_Gdiplus_FillPolygon(points_num, points, m_cGPLineColor, m_cGPFillColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_Rectangle(float x, float y, float w, float h, Optional<COLORREF> linecolor)
	{
		//if (BeginWindowTask())
	//{

		if (linecolor.isSet)	GP_SetLineColor(linecolor);
		EasyX_Gdiplus_Rectangle(x, y, w, h, m_cGPLineColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_SolidRectangle(float x, float y, float w, float h, Optional<COLORREF> fillcolor)
	{
		//if (BeginWindowTask())
	//{

		if (fillcolor.isSet)	GP_SetFillColor(fillcolor);
		EasyX_Gdiplus_SolidRectangle(x, y, w, h, m_cGPFillColor, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_FillRectangle(float x, float y, float w, float h, Optional<COLORREF> linecolor, Optional<COLORREF> fillcolor)
	{
		//if (BeginWindowTask())
	//{

		if (linecolor.isSet)			GP_SetLineColor(linecolor);
		if (fillcolor.isSet)			GP_SetFillColor(fillcolor);

		EasyX_Gdiplus_FillRectangle(x, y, w, h, m_cGPLineColor, m_cGPFillColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_RoundRect(float x, float y, float w, float h, float ellipsewidth, float ellipseheight, Optional<COLORREF> linecolor)
	{
		//if (BeginWindowTask())
	//{

		if (linecolor.isSet)	GP_SetLineColor(linecolor);
		EasyX_Gdiplus_RoundRect(x, y, w, h, ellipsewidth, ellipseheight, m_cGPLineColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_SolidRoundRect(float x, float y, float w, float h, float ellipsewidth, float ellipseheight, Optional<COLORREF> fillcolor)
	{
		//if (BeginWindowTask())
	//{

		if (fillcolor.isSet)	GP_SetFillColor(fillcolor);
		EasyX_Gdiplus_SolidRoundRect(x, y, w, h, ellipsewidth, ellipseheight, m_cGPFillColor, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_FillRoundRect(float x, float y, float w, float h, float ellipsewidth, float ellipseheight, Optional<COLORREF> linecolor, Optional<COLORREF> fillcolor)
	{
		//if (BeginWindowTask())
	//{

		if (linecolor.isSet)			GP_SetLineColor(linecolor);
		if (fillcolor.isSet)			GP_SetFillColor(fillcolor);

		EasyX_Gdiplus_FillRoundRect(x, y, w, h, ellipsewidth, ellipseheight, m_cGPLineColor, m_cGPFillColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_Ellipse(float x, float y, float w, float h, Optional<COLORREF> linecolor)
	{
		//if (BeginWindowTask())
	//{

		if (linecolor.isSet)	GP_SetLineColor(linecolor);
		EasyX_Gdiplus_Ellipse(x, y, w, h, m_cGPLineColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_SolidEllipse(float x, float y, float w, float h, Optional<COLORREF> fillcolor)
	{
		//if (BeginWindowTask())
	//{

		if (fillcolor.isSet)	GP_SetFillColor(fillcolor);
		EasyX_Gdiplus_SolidEllipse(x, y, w, h, m_cGPFillColor, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_FillEllipse(float x, float y, float w, float h, Optional<COLORREF> linecolor, Optional<COLORREF> fillcolor)
	{
		//if (BeginWindowTask())
	//{

		if (linecolor.isSet)			GP_SetLineColor(linecolor);
		if (fillcolor.isSet)			GP_SetFillColor(fillcolor);

		EasyX_Gdiplus_FillEllipse(x, y, w, h, m_cGPLineColor, m_cGPFillColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_Pie(float x, float y, float w, float h, float stangle, float endangle, Optional<COLORREF> linecolor)
	{
		//if (BeginWindowTask())
	//{

		if (linecolor.isSet)	GP_SetLineColor(linecolor);
		EasyX_Gdiplus_Pie(x, y, w, h, stangle, endangle, m_cGPLineColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_SolidPie(float x, float y, float w, float h, float stangle, float endangle, Optional<COLORREF> fillcolor)
	{
		//if (BeginWindowTask())
	//{

		if (fillcolor.isSet)	GP_SetFillColor(fillcolor);
		EasyX_Gdiplus_SolidPie(x, y, w, h, stangle, endangle, m_cGPFillColor, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_FillPie(float x, float y, float w, float h, float stangle, float endangle, Optional<COLORREF> linecolor, Optional<COLORREF> fillcolor)
	{
		//if (BeginWindowTask())
	//{

		if (linecolor.isSet)			GP_SetLineColor(linecolor);
		if (fillcolor.isSet)			GP_SetFillColor(fillcolor);

		EasyX_Gdiplus_FillPie(x, y, w, h, stangle, endangle, m_cGPLineColor, m_cGPFillColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}

	void Canvas::GP_Arc(float x, float y, float w, float h, float stangle, float endangle, Optional<COLORREF> linecolor)
	{
		//if (BeginWindowTask())
	//{

		if (linecolor.isSet)	GP_SetLineColor(linecolor);
		EasyX_Gdiplus_Arc(x, y, w, h, stangle, endangle, m_cGPLineColor, m_fGPLineWidth, m_bGPAlpha, m_enuSmoothingMode, this);

		//EndWindowTask\(\);
		//}
	}
}

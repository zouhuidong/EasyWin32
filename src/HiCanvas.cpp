#include <HiEasyX/HiCanvas.h>

#include <HiEasyX/HiMacro.h>
#include <HiEasyX/HiWindow.h>
#include <HiEasyX/HiGdiplus.h>

#include <stdexcept>

namespace HiEasyX
{

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
		if (GetWorkingImageHX() != p)
		{
			m_property.SaveWorkingImageOnly();
			SetWorkingImageHX(p);
		}
		return BeginWindowTask();*/

		m_pImgPrevious = GetWorkingImageHX();
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

	void Canvas::SetDefault()
	{
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
		setbkcolor(cBk);
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
		m_w = w;
		m_h = h;
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

	void Canvas::PutImageOut_SupAlpha(int x, int y, IMAGE* pImg, Optional<RECT> crop, BYTE alpha, bool bUseSrcAlpha, bool isCalculated)
	{
		int w, h;	// 目标输出画布尺寸
		GetImageSize(pImg, &w, &h);
		//if (BeginWindowTask())
		//{
		CopyImage_SupAlpha(
			x, y,
			GetImageBufferHX(pImg), w, h,
			m_pBuf, getwidth(), getheight(),
			crop,
			alpha, bUseSrcAlpha, isCalculated
		);

		//	EndWindowTask();
		//}
	}

	LINESTYLE Canvas::getlinestyle()
	{
		if (BeginDrawing())
		{
			LINESTYLE s;
			::getlinestyle(&s);
			EndDrawing();
			return s;
		}
		return {};
	}

	void Canvas::cleardevice(Optional<COLORREF> bkcolor)
	{
		if (BeginDrawing())
		{
			if (bkcolor.isSet) ::setbkcolor(bkcolor);
			::cleardevice();
		}
	}

	void Canvas::Clear(Optional<COLORREF> bkcolor, bool ignore_alpha)
	{
		if (BeginDrawing())
		{
			if (bkcolor.isSet)	::setbkcolor(bkcolor);
			DWORD bk_bgr = BGR(::getbkcolor());

			if (ignore_alpha)		// 设置背景透明度为 255（不透明）
				bk_bgr |= 0xFF000000;

			for (int i = 0; i < GetBufferSize(); i++)
				m_pBuf[i] = bk_bgr;

			EndDrawing();
		}
	}

	void Canvas::setlinestyle(LINESTYLE style)
	{
		if (BeginDrawing())
		{
			::setlinestyle(&style);
			EndDrawing();
		}
	}

	void Canvas::setlinestyle(int style, int thickness, const DWORD* puserstyle, DWORD userstylecount)
	{
		if (BeginDrawing())
		{
			::setlinestyle(style, thickness, puserstyle, userstylecount);
			EndDrawing();
		}
	}

	void Canvas::setlinethickness(int thickness)
	{
		LINESTYLE s = getlinestyle();
		s.thickness = thickness;
		setlinestyle(s);
	}

	int Canvas::getlinethickness()
	{
		return getlinestyle().thickness;
	}

	FILLSTYLE Canvas::getfillstyle()
	{
		if (BeginDrawing())
		{
			FILLSTYLE s;
			::getfillstyle(&s);
			EndDrawing();
			return s;
		}
		return {};
	}

	void Canvas::setfillstyle(FILLSTYLE style)
	{
		if (BeginDrawing())
		{
			::setfillstyle(&style);
			EndDrawing();
		}
	}

	void Canvas::setfillstyle(int style, long hatch, IMAGE* ppattern)
	{
		if (BeginDrawing())
		{
			::setfillstyle(style, hatch, ppattern);
			EndDrawing();
		}
	}

	void Canvas::setfillstyle(BYTE* ppattern8x8)
	{
		if (BeginDrawing())
		{
			::setfillstyle(ppattern8x8);
			EndDrawing();
		}
	}

	int Canvas::getrop2()
	{
		if (BeginDrawing())
		{
			int r = getrop2();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::setrop2(int mode)
	{
		if (BeginDrawing())
		{
			setrop2(mode);
			EndDrawing();
		}
	}

	int Canvas::getpolyfillmode()
	{
		if (BeginDrawing())
		{
			int r = ::getpolyfillmode();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::setpolyfillmode(int mode)
	{
		if (BeginDrawing())
		{
			::setpolyfillmode(mode);
			EndDrawing();
		}
	}

	/*void Canvas::SetDefault()
	{
		if (BeginDrawing())
		{
			::graphdefaults();
			EndDrawing();
		}
	}*/

	COLORREF Canvas::getlinecolor()
	{
		if (BeginDrawing())
		{
			COLORREF r = ::getlinecolor();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::setlinecolor(COLORREF color)
	{
		if (BeginDrawing())
		{
			::setlinecolor(color);
			EndDrawing();
		}
	}

	COLORREF Canvas::gettextcolor()
	{
		if (BeginDrawing())
		{
			COLORREF r = ::gettextcolor();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::settextcolor(COLORREF color)
	{
		if (BeginDrawing())
		{
			::settextcolor(color);
			EndDrawing();
		}
	}

	COLORREF Canvas::getfillcolor()
	{
		if (BeginDrawing())
		{
			COLORREF r = ::getfillcolor();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::setfillcolor(COLORREF color)
	{
		if (BeginDrawing())
		{
			::setfillcolor(color);
			EndDrawing();
		}
	}

	COLORREF Canvas::getbkcolor()
	{
		if (BeginDrawing())
		{
			COLORREF r = ::getbkcolor();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::setbkcolor(COLORREF color)
	{
		if (BeginDrawing())
		{
			::setbkcolor(color);
			EndDrawing();
		}
	}

	int Canvas::getbkmode()
	{
		if (BeginDrawing())
		{
			COLORREF r = ::getbkmode();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::setbkmode(int mode)
	{
		if (BeginDrawing())
		{
			::setbkmode(mode);
			EndDrawing();
		}
	}

	COLORREF Canvas::getpixel(int x, int y)
	{
		if (BeginDrawing())
		{
			COLORREF c = ::getpixel(x, y);
			EndDrawing();
			return c;
		}
		return 0;
	}

	void Canvas::putpixel(int x, int y, COLORREF c)
	{
		if (BeginDrawing())
		{
			::putpixel(x, y, c);
			EndDrawing();
		}
	}

	COLORREF Canvas::GetPixel_Direct(int x, int y)
	{
		//if (BeginWindowTask())
		//{
		return BGR(m_pBuf[y * m_w + x]);
		//EndWindowTask();
		//return c;
		//}
		//return 0;
	}

	void Canvas::PutPixel_Direct(int x, int y, COLORREF c)
	{
		//if (BeginWindowTask())
		//{
		/*int index = 0;
		if (IsPointInside(x, y, &index))
		{
			m_pBuf[index] = BGR(c);
		}
		else
		{
			throw std::out_of_range("Coordinates out of bounds.");
		}*/
		m_pBuf[y * m_w + x] = BGR(c);

		//EndWindowTask();
		//}
	}

	void Canvas::PutPixel_Direct_SupAlpha(int x, int y, COLORREF c)
	{
		//if (BeginWindowTask())
		//{
		/*int index = 0;
		if (IsPointInside(x, y, &index))
		{
			m_pBuf[index] = MixAlphaColor(m_pBuf[index], BGR(c), false);
		}
		else
		{
			throw std::out_of_range("Coordinates out of bounds.");
		}*/
		int index = y * m_w + x;
		m_pBuf[index] = MixAlphaColor(m_pBuf[index], BGR(c), false);
		//EndWindowTask();
		//}
	}

	void Canvas::line(int x1, int y1, int x2, int y2, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setlinecolor(c);
			::line(x1, y1, x2, y2);
			EndDrawing();
		}
	}

	void Canvas::line(POINT pt1, POINT pt2, Optional<COLORREF> c)
	{
		line(pt1.x, pt1.y, pt2.x, pt2.y, c);
	}

	void Canvas::rectangle(int left, int top, int right, int bottom, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setlinecolor(c);
			::rectangle(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::rectangle(RECT rct, Optional<COLORREF> c)
	{
		rectangle(rct.left, rct.top, rct.right, rct.bottom, c);
	}

	void Canvas::fillrectangle(int left, int top, int right, int bottom, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	::setlinecolor(cLine);
			if (cFill.isSet)	::setfillcolor(cFill);

			::fillrectangle(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::fillrectangle(RECT rct, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		fillrectangle(rct.left, rct.top, rct.right, rct.bottom, cLine, cFill);
	}

	void Canvas::solidrectangle(int left, int top, int right, int bottom, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setfillcolor(c);
			::solidrectangle(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::solidrectangle(RECT rct, Optional<COLORREF> c)
	{
		solidrectangle(rct.left, rct.top, rct.right, rct.bottom, c);
	}

	void Canvas::clearrectangle(int left, int top, int right, int bottom)
	{
		if (BeginDrawing())
		{
			::clearrectangle(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::clearrectangle(RECT rct)
	{
		clearrectangle(rct.left, rct.top, rct.right, rct.bottom);
	}

	void Canvas::circle(int x, int y, int radius, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setlinecolor(c);
			::circle(x, y, radius);
			EndDrawing();
		}
	}

	void Canvas::fillcircle(int x, int y, int radius, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	::setlinecolor(cLine);
			if (cFill.isSet)	::setfillcolor(cFill);

			::fillcircle(x, y, radius);
			EndDrawing();
		}
	}

	void Canvas::solidcircle(int x, int y, int radius, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setfillcolor(c);
			::solidcircle(x, y, radius);
			EndDrawing();
		}
	}

	void Canvas::clearcircle(int x, int y, int radius)
	{
		if (BeginDrawing())
		{
			::clearcircle(x, y, radius);
			EndDrawing();
		}
	}

	void Canvas::ellipse(int left, int top, int right, int bottom, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setlinecolor(c);
			::ellipse(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::ellipse(RECT rct, Optional<COLORREF> c)
	{
		ellipse(rct.left, rct.top, rct.right, rct.bottom, c);
	}

	void Canvas::fillellipse(int left, int top, int right, int bottom, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	::setlinecolor(cLine);
			if (cFill.isSet)	::setfillcolor(cFill);

			::fillellipse(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::fillellipse(RECT rct, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		fillellipse(rct.left, rct.top, rct.right, rct.bottom, cLine, cFill);
	}

	void Canvas::solidellipse(int left, int top, int right, int bottom, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setfillcolor(c);
			::solidellipse(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::solidellipse(RECT rct, Optional<COLORREF> c)
	{
		solidellipse(rct.left, rct.top, rct.right, rct.bottom, c);
	}

	void Canvas::clearellipse(int left, int top, int right, int bottom)
	{
		if (BeginDrawing())
		{
			::clearellipse(left, top, right, bottom);
			EndDrawing();
		}
	}

	void Canvas::clearellipse(RECT rct)
	{
		clearellipse(rct.left, rct.top, rct.right, rct.bottom);
	}

	void Canvas::roundrect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setlinecolor(c);
			::roundrect(left, top, right, bottom, ellipsewidth, ellipseheight);
			EndDrawing();
		}
	}

	void Canvas::roundrect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> c)
	{
		roundrect(rct.left, rct.top, rct.right, rct.bottom, ellipsewidth, ellipseheight, c);
	}

	void Canvas::fillroundrect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	::setlinecolor(cLine);
			if (cFill.isSet)	::setfillcolor(cFill);

			::fillroundrect(left, top, right, bottom, ellipsewidth, ellipseheight);
			EndDrawing();
		}
	}

	void Canvas::fillroundrect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		fillroundrect(rct.left, rct.top, rct.right, rct.bottom, ellipsewidth, ellipseheight, cLine, cFill);
	}

	void Canvas::solidroundrect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setfillcolor(c);
			::solidroundrect(left, top, right, bottom, ellipsewidth, ellipseheight);
			EndDrawing();
		}
	}

	void Canvas::solidroundrect(RECT rct, int ellipsewidth, int ellipseheight, Optional<COLORREF> c)
	{
		solidroundrect(rct.left, rct.top, rct.right, rct.bottom, ellipsewidth, ellipseheight, c);
	}

	void Canvas::clearroundrect(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight)
	{
		if (BeginDrawing())
		{
			::clearroundrect(left, top, right, bottom, ellipsewidth, ellipseheight);
			EndDrawing();
		}
	}

	void Canvas::clearroundrect(RECT rct, int ellipsewidth, int ellipseheight)
	{
		clearroundrect(rct.left, rct.top, rct.right, rct.bottom, ellipsewidth, ellipseheight);
	}

	void Canvas::arc(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setlinecolor(c);
			::arc(left, top, right, bottom, stangle, endangle);
			EndDrawing();
		}
	}

	void Canvas::arc(RECT rct, double stangle, double endangle, Optional<COLORREF> c)
	{
		arc(rct.left, rct.top, rct.right, rct.bottom, stangle, endangle, c);
	}

	void Canvas::pie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setlinecolor(c);
			::pie(left, top, right, bottom, stangle, endangle);
			EndDrawing();
		}
	}

	void Canvas::pie(RECT rct, double stangle, double endangle, Optional<COLORREF> c)
	{
		pie(rct.left, rct.top, rct.right, rct.bottom, stangle, endangle, c);
	}

	void Canvas::fillpie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	::setlinecolor(cLine);
			if (cFill.isSet)	::setfillcolor(cFill);

			::fillpie(left, top, right, bottom, stangle, endangle);
			EndDrawing();
		}
	}

	void Canvas::fillpie(RECT rct, double stangle, double endangle, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		fillpie(rct.left, rct.top, rct.right, rct.bottom, stangle, endangle, cLine, cFill);
	}

	void Canvas::solidpie(int left, int top, int right, int bottom, double stangle, double endangle, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setfillcolor(c);
			::solidpie(left, top, right, bottom, stangle, endangle);
			EndDrawing();
		}
	}

	void Canvas::solidpie(RECT rct, double stangle, double endangle, Optional<COLORREF> c)
	{
		solidpie(rct.left, rct.top, rct.right, rct.bottom, stangle, endangle, c);
	}

	void Canvas::clearpie(int left, int top, int right, int bottom, double stangle, double endangle)
	{
		if (BeginDrawing())
		{
			::clearpie(left, top, right, bottom, stangle, endangle);
			EndDrawing();
		}
	}

	void Canvas::clearpie(RECT rct, double stangle, double endangle)
	{
		clearpie(rct.left, rct.top, rct.right, rct.bottom, stangle, endangle);
	}

	void Canvas::polyline(const POINT* points, int num, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setlinecolor(c);
			::polyline(points, num);
			EndDrawing();
		}
	}

	void Canvas::polygon(const POINT* points, int num, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setlinecolor(c);
			::polygon(points, num);
			EndDrawing();
		}
	}

	void Canvas::fillpolygon(const POINT* points, int num, Optional<COLORREF> cLine, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cLine.isSet)	::setlinecolor(cLine);
			if (cFill.isSet)	::setfillcolor(cFill);

			::fillpolygon(points, num);
			EndDrawing();
		}
	}

	void Canvas::solidpolygon(const POINT* points, int num, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setfillcolor(c);
			::solidpolygon(points, num);
			EndDrawing();
		}
	}

	void Canvas::clearpolygon(const POINT* points, int num)
	{
		if (BeginDrawing())
		{
			::clearpolygon(points, num);
			EndDrawing();
		}
	}

	void Canvas::polybezier(const POINT* points, int num, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setlinecolor(c);
			::polybezier(points, num);
			EndDrawing();
		}
	}

	void Canvas::floodfill(int x, int y, COLORREF color, int filltype, Optional<COLORREF> cFill)
	{
		if (BeginDrawing())
		{
			if (cFill.isSet)	::setfillcolor(cFill);
			::floodfill(x, y, color, filltype);
			EndDrawing();
		}
	}

	int Canvas::outtextxy(int x, int y, LPCTSTR lpszText, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::settextcolor(c);
			::outtextxy(x, y, lpszText);
			int width = ::textwidth(lpszText);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::outtextxy(int x, int y, TCHAR ch, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::settextcolor(c);
			::outtextxy(x, y, ch);
			int width = ::textwidth(ch);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::outtextxy_format(int x, int y, int _Size, LPCTSTR _Format, ...)
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

			::outtextxy(x, y, buf);
			int width = ::textwidth(buf);
			delete[] buf;

			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::textwidth(LPCTSTR lpszText)
	{
		if (BeginDrawing())
		{
			int width = ::textwidth(lpszText);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::textwidth(TCHAR c)
	{
		if (BeginDrawing())
		{
			int width = ::textwidth(c);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::textheight(LPCTSTR lpszText)
	{
		if (BeginDrawing())
		{
			int width = ::textheight(lpszText);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::textheight(TCHAR c)
	{
		if (BeginDrawing())
		{
			int width = ::textheight(c);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::drawtext(LPCTSTR str, RECT* pRect, UINT uFormat, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::settextcolor(c);
			int width = ::drawtext(str, pRect, uFormat);
			EndDrawing();
			return width;
		}
		return 0;
	}

	int Canvas::drawtext(TCHAR ch, RECT* pRect, UINT uFormat, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::settextcolor(c);
			int width = ::drawtext(ch, pRect, uFormat);
			EndDrawing();
			return width;
		}
		return 0;
	}

	void Canvas::centertext(LPCTSTR lpszText, Optional<RECT> rct, Optional<COLORREF> c)
	{
		if (!rct.isSet)	rct = RECT{ 0,0,getwidth(),getheight() };
		drawtext(lpszText, &rct.value, DT_CENTER | DT_VCENTER | DT_SINGLELINE, c);
	}

	void Canvas::centertext_format(int _Size, LPCTSTR _Format, ...)
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

		centertext(buf);
		delete[] buf;
	}

	LOGFONT Canvas::gettextstyle()
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			::gettextstyle(&font);
			EndDrawing();
			return font;
		}
		return {};
	}

	void Canvas::settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace)
	{
		if (BeginDrawing())
		{
			::settextstyle(nHeight, nWidth, lpszFace);
			EndDrawing();
		}
	}

	void Canvas::settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut)
	{
		if (BeginDrawing())
		{
			::settextstyle(nHeight, nWidth, lpszFace, nEscapement, nOrientation, nWeight, bItalic, bUnderline, bStrikeOut);
			EndDrawing();
		}
	}

	void Canvas::settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut, BYTE fbCharSet, BYTE fbOutPrecision, BYTE fbClipPrecision, BYTE fbQuality, BYTE fbPitchAndFamily)
	{
		if (BeginDrawing())
		{
			::settextstyle(nHeight, nWidth, lpszFace, nEscapement, nOrientation, nWeight, bItalic, bUnderline, bStrikeOut, fbCharSet, fbOutPrecision, fbClipPrecision, fbQuality, fbPitchAndFamily);
			EndDrawing();
		}
	}

	void Canvas::settextstyle(LOGFONT font)
	{
		if (BeginDrawing())
		{
			::settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::setfont(int nHeight, int nWidth)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			::gettextstyle(&font);
			font.lfHeight = nHeight;
			font.lfWidth = nWidth;
			::settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::settypeface(LPCTSTR lpsz)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			::gettextstyle(&font);
			::lstrcpy(font.lfFaceName, lpsz);
			::settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::settextescapement(LONG lfEscapement)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			::gettextstyle(&font);
			font.lfEscapement = lfEscapement;
			::settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::settextorientation(LONG lfOrientation)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			::gettextstyle(&font);
			font.lfOrientation = lfOrientation;
			::settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::settextweight(LONG lfWeight)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			::gettextstyle(&font);
			font.lfWeight = lfWeight;
			::settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::settextitalic(bool lfItalic)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			::gettextstyle(&font);
			font.lfItalic = lfItalic;
			::settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::settextunderline(bool lfUnderline)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			::gettextstyle(&font);
			font.lfUnderline = lfUnderline;
			::settextstyle(&font);
			EndDrawing();
		}
	}

	void Canvas::settextstrikeout(bool lfStrikeOut)
	{
		if (BeginDrawing())
		{
			LOGFONT font;
			::gettextstyle(&font);
			font.lfStrikeOut = lfStrikeOut;
			::settextstyle(&font);
			EndDrawing();
		}
	}

	COLORREF Canvas::getcolor()
	{
		if (BeginDrawing())
		{
			int r = ::getcolor();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::setcolor(COLORREF color)
	{
		if (BeginDrawing())
		{
			::setcolor(color);
			EndDrawing();
		}
	}

	int Canvas::getx()
	{
		if (BeginDrawing())
		{
			int r = ::getx();
			EndDrawing();
			return r;
		}
		return 0;
	}

	int Canvas::gety()
	{
		if (BeginDrawing())
		{
			int r = ::gety();
			EndDrawing();
			return r;
		}
		return 0;
	}

	void Canvas::moveto(int x, int y)
	{
		if (BeginDrawing())
		{
			::moveto(x, y);
			EndDrawing();
		}
	}

	void Canvas::moverel(int dx, int dy)
	{
		if (BeginDrawing())
		{
			::moverel(dx, dy);
			EndDrawing();
		}
	}

	void Canvas::lineto(int x, int y, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setcolor(c);
			::lineto(x, y);
			EndDrawing();
		}
	}

	void Canvas::linerel(int dx, int dy, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setcolor(c);
			::linerel(dx, dy);
			EndDrawing();
		}
	}

	void Canvas::outtext(LPCTSTR lpszText, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setcolor(c);
			::outtext(lpszText);
			EndDrawing();
		}
	}

	void Canvas::outtext(TCHAR ch, Optional<COLORREF> c)
	{
		if (BeginDrawing())
		{
			if (c.isSet)	::setcolor(c);
			::outtext(ch);
			EndDrawing();
		}
	}

	int Canvas::outtext_format(int _Size, LPCTSTR _Format, ...)
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

			::outtext(buf);
			int width = ::textwidth(buf);
			delete[] buf;

			EndDrawing();
			return width;
		}
		return 0;
	}

	void Canvas::loadimage(LPCTSTR pImgFile, int nWidth, int nHeight, bool bResize)
	{
		::loadimage(this, pImgFile, nWidth, nHeight, bResize);
	}

	void Canvas::loadimage(LPCTSTR pResType, LPCTSTR pResName, int nWidth, int nHeight, bool bResize)
	{
		::loadimage(this, pResType, pResName, nWidth, nHeight, bResize);
	}

	void Canvas::LoadImage_SupAlpha(LPCTSTR lpszImgFile, int x, int y, bool bResize, int nWidth, int nHeight, BYTE alpha, bool bUseSrcAlpha, bool isCalculated)
	{
		// 标记到底要不要调整图像大小
		bool resize_flag = bResize;
		if (getwidth() == 0 && getheight() == 0)
		{
			resize_flag = true;
		}

		// 如果超出画布范围而又不调整图像大小，则直接返回
		if (!resize_flag && (x > getwidth() || y > getheight()))	return;

		// 加载图像
		IMAGE img;
		::loadimage(&img, lpszImgFile, nWidth, nHeight, true);
		int w = img.getwidth(), h = img.getheight();

		// 如果图像在左侧或上方越界，那么不用输出了
		if (x <= -w || y <= -h)	return;

		// 调整图像大小
		if (resize_flag)
		{
			int nw = x + w;
			int nh = y + h;
			Resize(nw, nh);
		}

		// 输出图像
		PutImageIn_SupAlpha(x, y, &img, {}, alpha, bUseSrcAlpha, isCalculated);
	}

	void Canvas::getimage(IMAGE* pDstImg, int srcX, int srcY, int srcWidth, int srcHeight)
	{
		if (BeginDrawing())
		{
			::getimage(pDstImg, srcX, srcY, srcWidth, srcHeight);
			EndDrawing();
		}
	}

	void Canvas::saveimage(LPCTSTR pImgFile)
	{
		::saveimage(pImgFile, this);
	}

	void Canvas::putimage_in(int dstX, int dstY, const IMAGE* pSrcImg, DWORD dwRop)
	{
		if (BeginDrawing())
		{
			::putimage(dstX, dstY, pSrcImg, dwRop);
			EndDrawing();
		}
	}

	void Canvas::putimage_in(int dstX, int dstY, int dstWidth, int dstHeight, const IMAGE* pSrcImg, int srcX, int srcY, DWORD dwRop)
	{
		if (BeginDrawing())
		{
			::putimage(dstX, dstY, dstWidth, dstHeight, pSrcImg, srcX, srcY, dwRop);
			EndDrawing();
		}
	}

	void Canvas::putimage_out(IMAGE* pDstImg, int dstX, int dstY, DWORD dwRop)
	{
		IMAGE* pOld = GetWorkingImageHX();
		SetWorkingImageHX(pDstImg);
		::putimage(dstX, dstY, this, dwRop);
		::SetWorkingImage(pOld);
	}

	void Canvas::putimage_out(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, DWORD dwRop)
	{
		IMAGE* pOld = GetWorkingImageHX();
		SetWorkingImageHX(pDstImg);
		::putimage(dstX, dstY, dstWidth, dstHeight, this, srcX, srcY, dwRop);
		::SetWorkingImage(pOld);
	}

	void Canvas::PutImageIn_SupAlpha(int x, int y, IMAGE* pImg, Optional<RECT> crop, BYTE alpha, bool bUseSrcAlpha, bool isCalculated)
	{
		int w, h;	// 原图像尺寸
		GetImageSize(pImg, &w, &h);
		//if (BeginWindowTask())
		//{
		CopyImage_SupAlpha(
			x, y,
			m_pBuf, getwidth(), getheight(),
			GetImageBufferHX(pImg), w, h,
			crop,
			alpha, bUseSrcAlpha, isCalculated
		);

		//EndWindowTask();
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

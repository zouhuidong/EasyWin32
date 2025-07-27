/**
 * @file	HiCanvas.h
 * @brief	HiEasyX ��Ļ���ģ��
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
	 * @brief ������Сд��ͼ�����ڲ����� EasyX ԭ���������л��ƣ���д������ʹ�� EasyX ԭ��������
	*/
	class Canvas : public IMAGE
	{
		// ������
	//public:

	//	// ����˺������� BindToWindow ����
	//	friend void BindWindowCanvas(Canvas*, HWND);

	protected:

		/////// ���� ///////

		IMAGE* m_pImgPrevious;					///< ת�� Working Image ֮ǰ�Ļ�ͼ����

		DWORD* m_pBuf = nullptr;			///< ͼ���ڴ�ָ��
		HDC m_hdc = nullptr;				///< ͼ���ڴ�ָ��
		int m_w = 0;
		int m_h = 0;

		//bool m_bBindToImgPointer;			///< �û����Ƿ�󶨵�ͼ��ָ��
		//IMAGE* m_pImg;						///< �����󶨵�ͼ��ָ�루�������󶨵�ָ�룩

		//bool m_bBatchDraw;					///< �Ƿ���������������

		COLORREF m_cGPLineColor = WHITE;	///< GDI+ ��ͼʱʹ�õ�������ɫ
		COLORREF m_cGPFillColor = WHITE;	///< GDI+ ��ͼʱʹ�õ������ɫ
		float m_fGPLineWidth = 1.f;			///< GDI+ ��ͼʱ���������
		bool m_bGPAlpha = false;			///< GDI+ ��ͼʱ�Ƿ�����͸����
		Gdiplus::SmoothingMode m_enuSmoothingMode = Gdiplus::SmoothingModeAntiAlias;	///< GDI+ �����ģʽ

		//HWND m_hBindWindow;					///< �󶨵��Ĵ���
		//bool m_bAutoMarkFlushWindow = true;	///< �󶨵�����ʱ������Ƿ��ڻ��ƺ��Զ�������Ҫ����˫����

		/////// ���� ///////

		/**
		 * @brief �������
		*/
		void CleanUpSettings();

		/**
		 * @brief ���� EasyX ��ͼ����ǰ�����øû���ΪĿ���ͼ����
		 * @return �Ƿ����óɹ�
		*/
		bool BeginDrawing();

		/**
		 * @brief ���� EasyX ��ͼ������ϣ��ָ���ǰ�Ļ�ͼ״̬
		*/
		void EndDrawing();

		// ������
		///**
		// * @brief <pre>
		// *		�������󶨵����ڣ�������С�洰���Զ�������
		// *
		// *	��ע��
		// *		�˺���ֻӦ�ñ� BindWindowCanvas ��������
		// * </pre>
		// *
		// * @param[in] hWnd Ŀ�괰��
		// * @param[in] pImg ����ͼ�񻺳���
		// * @return �˻���
		//*/
		//Canvas& BindToWindow(HWND hWnd, IMAGE* pImg);

	private:

		// ���� EasyX �� BUG ������д�˺���
		// ��� https://qa.codebus.cn/question/2791
		void SetDefault() override;

	public:

		/////// ������������ ///////

		Canvas();

		Canvas(int w, int h, COLORREF cBk = BLACK);

		/**
		 * @brief ����ͼ������
		 * @param[in] pImg ԭͼ��
		*/
		Canvas(IMAGE* pImg);
		Canvas(IMAGE img);

		/**
		 * @brief ����ͼ������
		 * @param[in] pImg ԭͼ��
		*/
		Canvas& operator= (IMAGE* pImg);
		Canvas& operator= (IMAGE img);

		// ������
		///**
		// * @brief <pre>
		// *		���¼���ͼ��ߴ���Ϣ
		// *
		// * ��ע��
		// *		������ͼ��ָ�룬���ⲿ����ͼ���С������ô˺���
		// * </pre>
		//*/
		//void UpdateSizeInfo();

		/**
		 * @brief ���軭����С��������ջ�����
		 * @param[in] w Ŀ����
		 * @param[in] h Ŀ��߶�
		*/
		void Resize(int w, int h) override;

		// ������
		///**
		// * @brief <pre>
		// *		�󶨵�ͼ��ָ��
		// *
		// *	ע�⣺
		// *		�󶨵�ͼ��ָ���������ⲿ������ͼ���С������Ҫ���� UpdateSizeInfo ���¼���ͼ����Ϣ
		// * </pre>
		// *
		// * @param[in] pImg Ŀ��ͼ��ָ��
		// * @return �˻���
		//*/
		//Canvas& BindToImage(IMAGE* pImg);

		// ������
		///**
		// * @brief <pre>
		// *		��ȡ���� IMAGE ָ��
		// *
		// *	ע�⣺
		// *		�е�ʱ�򻭲����˱�� IMAGE�����Ի�ͼʱ����ֱ��ʹ�� this��������ô˺�����
		// * </pre>
		//*/
		//IMAGE* GetImagePointer() { return m_bBindToImgPointer ? m_pImg : this; }

		// ������
		///**
		// * @brief �ȼ��� GetImagePointer()
		//*/
		//IMAGE* Pt() { return m_bBindToImgPointer ? m_pImg : this; }

		/**
		 * @brief ��ȡͼ�񻺳���ָ�루ֱ�Ӳ������飩
		*/
		DWORD* GetBuffer() const { return m_pBuf; }

		/**
		 * @brief ��ȡͼ�񻺳�����С����ͼ��������� * �ߣ�
		*/
		int GetBufferSize() const { return getwidth() * getheight(); }

		/**
		 * @brief ��ȡ������ HDC
		*/
		HDC GetHDC() const { return m_hdc; }

		// ������
		///**
		// * @brief <pre>
		// *		�󶨵�����ʱ�������Ƿ���ÿ�λ��ƺ��Զ����ˢ�´���˫����
		// *
		// *	��ע��
		// *		���ˢ�´���˫���岢����ζ�ż�ʱˢ�¡�
		// *		��Ǻ󣬴��ڽ�������һ�ν��ܵ�������Ϣʱ����˫���塣
		// * </pre>
		//*/
		//void EnableAutoMarkFlushWindow(bool enable);
		//bool IsEnableAutoMarkFlushWindow() const { return m_bAutoMarkFlushWindow; }

		/////// ��ͼ״̬���ú��� ///////

		// ������
		///**
		// * @brief <pre>
		// *		��ʼ���������ƣ��ú����������ڿ���˫���壩
		// *
		// *	��ע��
		// *		���øú����󣬵�ǰ��ͼĿ�꽫ת�Ƶ��û������˺�ÿ�λ��Ʋ���ָ���ͼĿ��
		// * </pre>
		//*/
		//void BeginBatchDrawing();

		///**
		// * @brief <pre>
		// *		������������
		// *
		// *	��ע��
		// *		��ͼĿ�꽫�ָ�����������ǰ��״̬
		// * </pre>
		//*/
		//void EndBatchDrawing();

		/////// EasyX ���Ļ�����ͼ���� ///////

		/**
		 * @brief �ж�ĳ���Ƿ�λ��ͼ����
		 * @param[in] x ����
		 * @param[in] y ����
		 * @param[out] pIndex ���ظõ���������
		 * @return �Ƿ�λ��ͼ����
		*/
		bool IsPointInside(int x, int y, int* pIndex = nullptr);

		void cleardevice(Optional<COLORREF> bkcolor = {});

		/**
		 * @brief �ñ���ɫ��ջ����������� Clear �������˺���Ĭ�ϱ�������ɫ�е�͸���ȣ�
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
		 * @brief ���û�ͼ״̬Ϊԭʼ״̬
		*/
		//void SetDefault();

		COLORREF getpixel(int x, int y);
		void putpixel(int x, int y, COLORREF c);

		/**
		 * @brief  ֱ�Ӳ����Դ��ȡ�㣨��Խ���飩
		*/
		COLORREF GetPixel_Direct(int x, int y);

		/**
		 * @brief ֱ�Ӳ����Դ���Ƶ㣨��Խ���飩
		*/
		void PutPixel_Direct(int x, int y, COLORREF c);

		/**
		 * @brief ֱ�Ӳ����Դ���ƴ���͸���ȵĵ㣨ʹ�� COLORREF �е�͸���ȣ�����Խ���飩
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
		 * @brief ���ĳ����
		 * @param[in] x				�����ʼλ��
		 * @param[in] y				�����ʼλ��
		 * @param[in] color			�����ɫ
		 *
		 * @param[in] filltype <pre>
		 *		���ģʽ������������ѡ��
		 *		FLOODFILLBORDER		ָ�� color Ϊ���߽���ɫ������������ɫ��ֹͣ���
		 *		FLOODFILLSURFACE	ָ�� color Ϊ��������ɫ����ֻ������ɫ
		 * </pre>
		 *
		 * @param[in] cFill			�����ɫ
		*/
		void floodfill(int x, int y, COLORREF color, int filltype = FLOODFILLBORDER, Optional<COLORREF> cFill = {});

		/**
		 * @brief ��ָ��λ������ı�
		 * @param[in] x				λ��
		 * @param[in] y				λ��
		 * @param[in] lpszText		�ı�
		 * @param[in] c				�ı���ɫ
		 * @return �ı����ؿ��
		*/
		int outtextxy(int x, int y, LPCTSTR lpszText, Optional<COLORREF> c = {});

		int outtextxy(int x, int y, TCHAR ch, Optional<COLORREF> c = {});

		/**
		 * @brief ��ָ��λ�������ʽ���ı�
		 * @param[in] x			λ��
		 * @param[in] y			λ��
		 * @param[in] _Size		��ʽ���ı���󳤶�
		 * @param[in] _Format		��ʽ���ַ���
		 * @param[in]				��������
		 * @return �ı����ؿ��
		*/
		int outtextxy_format(int x, int y, int _Size, LPCTSTR _Format, ...);

		/**
		 * @brief ��ȡ�ı����ؿ��
		 * @param[in] lpszText �ı�
		 * @return ��ȡ�ı����ؿ��
		*/
		int textwidth(LPCTSTR lpszText);

		int textwidth(TCHAR c);
		int textheight(LPCTSTR lpszText);
		int textheight(TCHAR c);
		int drawtext(LPCTSTR str, RECT* pRect, UINT uFormat, Optional<COLORREF> c = {});
		int drawtext(TCHAR ch, RECT* pRect, UINT uFormat, Optional<COLORREF> c = {});

		/**
		 * @brief ��ĳ��������������
		 * @param[in] lpszText			�ı�
		 * @param[in] rct				�������Ĭ��Ϊ��������
		 * @param[in] isSetColor		�Ƿ�������ɫ
		 * @param[in] c					�ı���ɫ
		*/
		void centertext(LPCTSTR lpszText, Optional<RECT> rct = {}, Optional<COLORREF> c = {});

		/**
		 * @brief ���������ʽ���ı�
		 * @param[in] _Size			��ʽ���ı���󳤶�
		 * @param[in] _Format		��ʽ���ַ���
		 * @param[in]				��������
		*/
		void centertext_format(int _Size, LPCTSTR _Format, ...);

		LOGFONT gettextstyle();
		void settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace);
		void settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut);
		void settextstyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut, BYTE fbCharSet, BYTE fbOutPrecision, BYTE fbClipPrecision, BYTE fbQuality, BYTE fbPitchAndFamily);
		void settextstyle(LOGFONT font);

		/**
		 * @brief ���������С
		 * @param[in] nHeight	�߶�
		 * @param[in] nWidth	��ȣ�Ϊ 0 ʱ���Զ���߶�ƥ�䣩
		*/
		void setfont(int nHeight, int nWidth = 0);

		/**
		 * @brief ����ʹ�����������
		 * @param[in] lpsz ��������
		*/
		void settypeface(LPCTSTR lpsz);

		/**
		 * @brief �����ַ�������д�Ƕȣ���λ 0.1 �ȣ�
		 * @param[in] lfEscapement �Ƕ�
		*/
		void settextescapement(LONG lfEscapement);

		/**
		 * @brief ����ÿ���ַ�����д�Ƕȣ���λ 0.1 �ȣ�
		 * @param[in] lfOrientation �Ƕ�
		*/
		void settextorientation(LONG lfOrientation);

		/**
		 * @brief �����ַ��ıʻ���ϸ����Χ Ĭ�� 0 ~ 1000 ��֣�
		 * @param[in] lfWeight ��ϸ
		*/
		void settextweight(LONG lfWeight);

		/**
		 * @brief ���������Ƿ�Ϊб��
		 * @param[in] lfItalic �Ƿ�ʹ��б��
		*/
		void settextitalic(bool lfItalic);

		/**
		 * @brief ���������Ƿ����»���
		 * @param[in] lfUnderline �Ƿ�ʹ���»���
		*/
		void settextunderline(bool lfUnderline);

		/**
		 * @brief ���������Ƿ���ɾ����
		 * @param[in] lfStrikeOut �Ƿ�ʹ��ɾ����
		*/
		void settextstrikeout(bool lfStrikeOut);

		/**
		 * @brief ��ȡǰ��ɫ
		*/
		COLORREF getcolor();

		/**
		 * @brief ����ǰ��ɫ
		 * @param[in] color ǰ��ɫ
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
		 * @brief �����ʽ���ı�
		 * @param[in] _Size			��ʽ���ı���󳤶�
		 * @param[in] _Format		��ʽ���ַ���
		 * @param[in]				��������
		 * @return �ı����ؿ��
		*/
		int outtext_format(int _Size, LPCTSTR _Format, ...);

		void loadimage(LPCTSTR pImgFile, int nWidth = 0, int nHeight = 0, bool bResize = false);
		void loadimage(LPCTSTR pResType, LPCTSTR pResName, int nWidth = 0, int nHeight = 0, bool bResize = false);

		/**
		 * @brief <pre>
		 *		����ͼƬ�ļ�������
		 *
		 *	��ע��
		 *		������͸��ͨ�������Ƶ������ϵ����ݲ��ᱣ��ԭͼ���͸������Ϣ
		 * </pre>
		 *
		 * @param[in] lpszImgFile		ͼ���ļ�·��
		 * @param[in] x					�����������λ��
		 * @param[in] y					�����������λ��
		 * @param[in] bResize			�Ƿ����������С����������ͼ�񣨶����޿�ߵĻ������Զ�������С��
		 * @param[in] nWidth			ͼ��Ŀ������ߴ磬Ϊ 0 ��ʾ������
		 * @param[in] nHeight			ͼ��Ŀ������ߴ磬Ϊ 0 ��ʾ������
		 * @param[in] alpha				����͸����
		 * @param[in] bUseSrcAlpha		�Ƿ�ʹ��ԭͼ��͸������Ϣ���л�ϣ���֧����͸������Ϣ�� png ͼ��
		 * @param[in] isCalculated		ԭͼ�Ƿ��Ѿ����͸����
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

		///< ����ָ���ü������ putimage ����
		void putimage_in(int dstX, int dstY, int dstWidth, int dstHeight, const IMAGE* pSrcImg, int srcX, int srcY, DWORD dwRop = SRCCOPY);

		void putimage_out(IMAGE* pDstImg, int dstX, int dstY, DWORD dwRop = SRCCOPY);
		void putimage_out(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, DWORD dwRop = SRCCOPY);

		/**
		 * @brief ����ͼ�񵽸û�����֧�� Alpha ͨ����
		 * @param[in] x					ͼ������λ��
		 * @param[in] y					ͼ������λ��
		 * @param[in] pImg				������ͼ��
		 * @param[in] crop				�ü�����
		 * @param[in] alpha				����͸����
		 * @param[in] bUseSrcAlpha		�Ƿ�ʹ��ԭͼ͸����
		 * @param[in] isCalculated		ԭͼ�Ƿ��Ѿ����͸����
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
		 * @brief ���û�����ͼ����Ƶ���һ������
		 * @param[in] x				����λ��
		 * @param[in] y				����λ��
		 * @param[in] pImg			Ŀ����ƻ���
		 * @param[in] crop			�ü�����Ĭ�ϲ��ü���
		 * @param[in] alpha			����͸����
		 * @param[in] bUseSrcAlpha	�Ƿ�ʹ�ô˻���͸����
		 * @param[in] isCalculated	���������Ƿ��Ѿ�͸�����
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

		/////// GDI+ ��ػ�ͼ���� ///////

		////////////////////////////////////////////////////////////////////
		//
		// ע�⣺
		//		GDI+ ��ͼ�������� EasyX ԭ����������ͬ���Ļ�ͼ��ɫ��
		//		������Ϊ GDI+ �Ļ�ͼ����֧��͸������ EasyX ԭ��������֧�֡�
		//
		//		����ʹ�� RGBA �� SET_ALPHA �����ô�͸���ȵ���ɫ
		//
		////////////////////////////////////////////////////////////////////

		void GP_SetLineColor(COLORREF color);
		void GP_SetFillColor(COLORREF color);
		void GP_SetLineWidth(float width);

		COLORREF GP_GetLineColor() const { return m_cGPLineColor; }
		COLORREF GP_GetFillColor() const { return m_cGPFillColor; }
		float GP_GetLineWidth() const { return m_fGPLineWidth; }

		/**
		 * @brief ���� GDI+ ����ʱ�Ƿ�ʹ��͸���ȣ�Ĭ�ϲ�ʹ�ã�
		*/
		void GP_EnableAlpha(bool enable);

		/**
		 * @brief ���� GDI+ �����ģʽ
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

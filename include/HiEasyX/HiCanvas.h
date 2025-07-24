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
	 * @brief ����
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

		int GetWidth() const { return getwidth(); }
		int GetHeight() const { return getheight(); }

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

		/**
		 * @brief <pre>
		 *		�ñ���ɫ��ջ���
		 *
		 *	��ע��
		 *		�˺��������Ա���ɫ��͸���ȣ���ֱ�ӶԻ������� 255 ��͸���ȣ�����͸������
		 * </pre>
		*/
		void Clear(Optional<COLORREF> bkcolor = {});

		/**
		 * @brief �ñ���ɫ��ջ����������� Clear �������˺���Ĭ�ϱ�������ɫ�е�͸���ȣ�
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
		 * @brief ���û�ͼ״̬Ϊԭʼ״̬
		*/
		void SetDefault();

		COLORREF GetPixel(int x, int y);
		void PutPixel(int x, int y, COLORREF c);

		/**
		 * @brief  ֱ�Ӳ����Դ��ȡ�㣨����Խ����׳��쳣��
		 * @throws std::out_of_range ������Խ������ʱ�׳�
		*/
		COLORREF GetPixel_Direct(int x, int y);

		/**
		 * @brief ֱ�Ӳ����Դ���Ƶ㣨����Խ����׳��쳣��
		 * @throws std::out_of_range ������Խ������ʱ�׳�
		*/
		void PutPixel_Direct(int x, int y, COLORREF c);

		/**
		 * @brief ֱ�Ӳ����Դ���ƴ���͸���ȵĵ㣨ʹ�� COLORREF �е�͸���ȣ�������Խ����׳��쳣��
		 * @throws std::out_of_range ������Խ������ʱ�׳�
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
		 * @param[in] isSetColor		�Ƿ����������ɫ
		 * @param[in] cFill			�����ɫ
		*/
		void FloodFill(int x, int y, COLORREF color, int filltype = FLOODFILLBORDER, Optional<COLORREF> cFill = {});

		/**
		 * @brief ��ָ��λ������ı�
		 * @param[in] x				λ��
		 * @param[in] y				λ��
		 * @param[in] lpszText			�ı�
		 * @param[in] isSetColor		�Ƿ�������ɫ
		 * @param[in] c				�ı���ɫ
		 * @return �ı����ؿ��
		*/
		int OutTextXY(int x, int y, LPCTSTR lpszText, Optional<COLORREF> c = {});

		int OutTextXY(int x, int y, TCHAR ch, Optional<COLORREF> c = {});

		/**
		 * @brief ��ָ��λ�������ʽ���ı�
		 * @param[in] x			λ��
		 * @param[in] y			λ��
		 * @param[in] _Size		��ʽ���ı���󳤶�
		 * @param[in] _Format		��ʽ���ַ���
		 * @param[in]				��������
		 * @return �ı����ؿ��
		*/
		int OutTextXY_Format(int x, int y, int _Size, LPCTSTR _Format, ...);

		/**
		 * @brief ��ȡ�ı����ؿ��
		 * @param[in] lpszText �ı�
		 * @return ��ȡ�ı����ؿ��
		*/
		int TextWidth(LPCTSTR lpszText);

		int TextWidth(TCHAR c);
		int TextHeight(LPCTSTR lpszText);
		int TextHeight(TCHAR c);
		int Draw_Text(LPCTSTR str, RECT* pRect, UINT uFormat, Optional<COLORREF> c = {});
		int Draw_Text(TCHAR ch, RECT* pRect, UINT uFormat, Optional<COLORREF> c = {});

		/**
		 * @brief ��ĳ��������������
		 * @param[in] lpszText			�ı�
		 * @param[in] rct				�������Ĭ��Ϊ��������
		 * @param[in] isSetColor		�Ƿ�������ɫ
		 * @param[in] c					�ı���ɫ
		*/
		void CenterText(LPCTSTR lpszText, Optional<RECT> rct = {}, Optional<COLORREF> c = {});

		/**
		 * @brief ���������ʽ���ı�
		 * @param[in] _Size			��ʽ���ı���󳤶�
		 * @param[in] _Format		��ʽ���ַ���
		 * @param[in]				��������
		*/
		void CenterText_Format(int _Size, LPCTSTR _Format, ...);

		LOGFONT GetTextStyle();
		void SetTextStyle(int nHeight, int nWidth, LPCTSTR lpszFace);
		void SetTextStyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut);
		void SetTextStyle(int nHeight, int nWidth, LPCTSTR lpszFace, int nEscapement, int nOrientation, int nWeight, bool bItalic, bool bUnderline, bool bStrikeOut, BYTE fbCharSet, BYTE fbOutPrecision, BYTE fbClipPrecision, BYTE fbQuality, BYTE fbPitchAndFamily);
		void SetTextStyle(LOGFONT font);

		/**
		 * @brief ���������С
		 * @param[in] nHeight	�߶�
		 * @param[in] nWidth	��ȣ�Ϊ 0 ʱ���Զ���߶�ƥ�䣩
		*/
		void SetFont(int nHeight, int nWidth = 0);

		/**
		 * @brief ����ʹ�����������
		 * @param[in] lpsz ��������
		*/
		void SetTypeface(LPCTSTR lpsz);

		/**
		 * @brief �����ַ�������д�Ƕȣ���λ 0.1 �ȣ�
		 * @param[in] lfEscapement �Ƕ�
		*/
		void SetTextEscapement(LONG lfEscapement);

		/**
		 * @brief ����ÿ���ַ�����д�Ƕȣ���λ 0.1 �ȣ�
		 * @param[in] lfOrientation �Ƕ�
		*/
		void SetTextOrientation(LONG lfOrientation);

		/**
		 * @brief �����ַ��ıʻ���ϸ����Χ Ĭ�� 0 ~ 1000 ��֣�
		 * @param[in] lfWeight ��ϸ
		*/
		void SetTextWeight(LONG lfWeight);

		/**
		 * @brief ���������Ƿ�Ϊб��
		 * @param[in] lfItalic �Ƿ�ʹ��б��
		*/
		void SetTextItalic(bool lfItalic);

		/**
		 * @brief ���������Ƿ����»���
		 * @param[in] lfUnderline �Ƿ�ʹ���»���
		*/
		void SetTextUnderline(bool lfUnderline);

		/**
		 * @brief ���������Ƿ���ɾ����
		 * @param[in] lfStrikeOut �Ƿ�ʹ��ɾ����
		*/
		void SetTextStrikeOut(bool lfStrikeOut);

		/**
		 * @brief ��ȡǰ��ɫ
		*/
		COLORREF GetColor();

		/**
		 * @brief ����ǰ��ɫ
		 * @param[in] color ǰ��ɫ
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
		 * @brief �����ʽ���ı�
		 * @param[in] _Size			��ʽ���ı���󳤶�
		 * @param[in] _Format		��ʽ���ַ���
		 * @param[in]				��������
		 * @return �ı����ؿ��
		*/
		int OutText_Format(int _Size, LPCTSTR _Format, ...);

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
		 * @return ��ȡ���� IMAGE ����
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
		 * @brief ����ͼ�񵽸û���
		 * @param[in] x					ͼ������λ��
		 * @param[in] y					ͼ������λ��
		 * @param[in] pImg				������ͼ��
		 * @param[in] crop				�ü�����
		 * @param[in] alpha				����͸����
		 * @param[in] bUseSrcAlpha		�Ƿ�ʹ��ԭͼ͸����
		 * @param[in] isCalculated		ԭͼ�Ƿ��Ѿ����͸����
		*/
		void PutImageIn_Alpha(
			int x,
			int y,
			IMAGE* pImg,
			RECT crop = { 0 },
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
		void RenderTo(
			int x,
			int y,
			IMAGE* pImg = nullptr,
			RECT crop = { 0 },
			BYTE alpha = 255,
			bool bUseSrcAlpha = false,
			bool isCalculated = false
		);

		/**
		 * @brief EasyX ԭ����ת����
		 * @param[in] radian		��ת����
		 * @param[in] bkcolor		��䱳��ɫ
		 * @param[in] autosize		�Ƿ�����Ӧ��תͼ���С
		 * @param[in] highquality	������
		*/
		void RotateImage(double radian, COLORREF bkcolor = BLACK, bool autosize = false, bool highquality = true);

		/**
		 * @brief ��תͼ�񣨱��� Alpha ��Ϣ��
		 * @param[in] radian	��ת����
		 * @param[in] bkcolor	��䱳��ɫ
		*/
		void RotateImage_Alpha(double radian, COLORREF bkcolor = BLACK);

		/**
		 * @brief ����ͼ�񣨴ֲڵġ�������ֵ�����ţ�����͸������Ϣ��
		 * @param[in] nW	Ŀ����
		 * @param[in] nH	Ŀ��߶ȣ�Ϊ 0 ����ݿ�Ȱ��������ţ�
		*/
		void ZoomImage_Rough_Alpha(int nW, int nH = 0);

		/**
		 * @brief ����ͼ��˫���Բ�ֵ������͸������Ϣ��
		 * @param[in] nW	Ŀ����
		 * @param[in] nH	Ŀ��߶ȣ�Ϊ 0 ����ݿ�Ȱ��������ţ�
		*/
		void ZoomImage_Alpha(int nW, int nH = 0);

		/**
		 * @brief ����ͼ�񣨻��� Win32 API���ȽϿ죬����͸������Ϣ��
		 * @param[in] nW	Ŀ����
		 * @param[in] nH	Ŀ��߶ȣ�Ϊ 0 ����ݿ�Ȱ��������ţ�
		*/
		void ZoomImage_StretchBlt_Alpha(int nW, int nH = 0);

		/**
		 * @brief ����ͼ�񣨻��� Win32 API���ȽϿ죬����͸������Ϣ��
		 * @param[in] nW	Ŀ����
		 * @param[in] nH	Ŀ��߶ȣ�Ϊ 0 ����ݿ�Ȱ��������ţ�
		*/
		void ZoomImage_AlphaBlend_Alpha(int nW, int nH = 0);

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

/**
 * @file	HiGdiPlus.h
 * @brief	HiEasyX ��� GDI+ ��ͼģ��
 * @author	huidong
*/

#pragma once

#include <graphics.h>
#include <gdiplus.h>

namespace HiEasyX
{
	/////// GDI+ �������� ///////

	//
	//	��ע��
	//		��������Ļ�ͼ����ʱ�����Զ��������� GDI+��
	//
	//		���� HiWindow ��ͼ����ʱҲ���Զ����� GDI+�����һ����ͼ���ڹر�ʱ��GDI+ ���Զ��رա�
	//		���û�д�����ͼ���ڣ�����Ҫ�ֶ��ر� GDI+��
	//

	/**
	 * @brief ���� GDI+������Ѿ�������ֱ�ӷ���
	*/
	void Gdiplus_Try_Starup();

	/**
	 * @brief �ر� GDI+
	*/
	void Gdiplus_Shutdown();

	/////// GDI+ ������װ ///////

	/**
	 * @brief ����
	*/
	void Gdiplus_Line(
		HDC hdc,
		float x1,
		float y1,
		float x2,
		float y2,
		Gdiplus::Color linecolor,
		float linewidth,
		Gdiplus::SmoothingMode smoothing_mode
	);

	/**
	 * @brief �������
	*/
	void Gdiplus_Polygon(
		HDC hdc,
		int points_num,
		Gdiplus::PointF* points,
		Gdiplus::Color linecolor,
		float linewidth,
		Gdiplus::SmoothingMode smoothing_mode
	);

	/**
	 * @brief ���ޱ߿��������
	*/
	void Gdiplus_SolidPolygon(
		HDC hdc,
		int points_num,
		Gdiplus::PointF* points,
		Gdiplus::Color fillcolor,
		Gdiplus::SmoothingMode smoothing_mode
	);

	/**
	 * @brief ������
	*/
	void Gdiplus_Rectangle(
		HDC hdc,
		float x,
		float y,
		float w,
		float h,
		Gdiplus::Color linecolor,
		float linewidth,
		Gdiplus::SmoothingMode smoothing_mode
	);

	/**
	 * @brief ���ޱ߿�������
	*/
	void Gdiplus_SolidRectangle(
		HDC hdc,
		float x,
		float y,
		float w,
		float h,
		Gdiplus::Color fillcolor,
		Gdiplus::SmoothingMode smoothing_mode
	);

	/**
	 * @brief ����Բ
	*/
	void Gdiplus_Ellipse(
		HDC hdc,
		float x,
		float y,
		float w,
		float h,
		Gdiplus::Color linecolor,
		float linewidth,
		Gdiplus::SmoothingMode smoothing_mode
	);

	/**
	 * @brief ���ޱ߿������Բ
	*/
	void Gdiplus_SolidEllipse(
		HDC hdc,
		float x,
		float y,
		float w,
		float h,
		Gdiplus::Color fillcolor,
		Gdiplus::SmoothingMode smoothing_mode
	);

	/**
	 * @brief ����״ͼ������˳ʱ��Ƕȣ�
	*/
	void Gdiplus_Pie(
		HDC hdc,
		float x,
		float y,
		float w,
		float h,
		float stangle,
		float sweepangle,
		Gdiplus::Color linecolor,
		float linewidth,
		Gdiplus::SmoothingMode smoothing_mode
	);

	/**
	 * @brief ���ޱ߿�����״ͼ������˳ʱ��Ƕȣ�
	*/
	void Gdiplus_SolidPie(
		HDC hdc,
		float x,
		float y,
		float w,
		float h,
		float stangle,
		float sweepangle,
		Gdiplus::Color fillcolor,
		Gdiplus::SmoothingMode smoothing_mode
	);

	/**
	 * @brief ��Բ��������˳ʱ��Ƕȣ�
	*/
	void Gdiplus_Arc(
		HDC hdc,
		float x,
		float y,
		float w,
		float h,
		float stangle,
		float sweepangle,
		Gdiplus::Color linecolor,
		float linewidth,
		Gdiplus::SmoothingMode smoothing_mode
	);

	/////// EasyX ���� GDI+ ��װ ///////

	////////////////////////////////////////////////////////////////
	//
	// ע������ EasyX ���ӿ���
	//
	//		enable_alpha	��ʾ�Ƿ�ʹ�ô�����ɫ�� alpha ֵ
	//		enable_aa		��ʾ�Ƿ��������
	//		pImg			��ʾĿ����ƻ���
	//
	////////////////////////////////////////////////////////////////

	/**
	 * @brief ת�� COLORREF �� Gdiplus::Color
	 * @param[in] color				ԭ��ɫ
	 * @param[in] reserve_alpha		�Ƿ��� COLORREF �е� alpha ֵ
	 * @return ת����� Gdiplus::Color ɫֵ
	*/
	Gdiplus::Color ConvertToGdiplusColor(COLORREF color, bool reserve_alpha = false);

	/**
	 * @brief ��ֱ��
	*/
	void EasyX_Gdiplus_Line(
		float x1,
		float y1,
		float x2,
		float y2,
		COLORREF linecolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief �������
	*/
	void EasyX_Gdiplus_Polygon(
		int points_num,
		POINT* points,
		COLORREF linecolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ���ޱ߿��������
	*/
	void EasyX_Gdiplus_SolidPolygon(
		int points_num,
		POINT* points,
		COLORREF fillcolor,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ���б߿��������
	*/
	void EasyX_Gdiplus_FillPolygon(
		int points_num,
		POINT* points,
		COLORREF linecolor,
		COLORREF fillcolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ������
	*/
	void EasyX_Gdiplus_Rectangle(
		float x,
		float y,
		float w,
		float h,
		COLORREF linecolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ���ޱ߿�������
	*/
	void EasyX_Gdiplus_SolidRectangle(
		float x,
		float y,
		float w,
		float h,
		COLORREF fillcolor,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ���б߿�������
	*/
	void EasyX_Gdiplus_FillRectangle(
		float x,
		float y,
		float w,
		float h,
		COLORREF linecolor,
		COLORREF fillcolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ��Բ�Ǿ���
	*/
	void EasyX_Gdiplus_RoundRect(
		float x,
		float y,
		float w,
		float h,
		float ellipsewidth,
		float ellipseheight,
		COLORREF linecolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ���ޱ߿����Բ�Ǿ���
	*/
	void EasyX_Gdiplus_SolidRoundRect(
		float x,
		float y,
		float w,
		float h,
		float ellipsewidth,
		float ellipseheight,
		COLORREF fillcolor,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ���б߿����Բ�Ǿ���
	*/
	void EasyX_Gdiplus_FillRoundRect(
		float x,
		float y,
		float w,
		float h,
		float ellipsewidth,
		float ellipseheight,
		COLORREF linecolor,
		COLORREF fillcolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ����Բ
	*/
	void EasyX_Gdiplus_Ellipse(
		float x,
		float y,
		float w,
		float h,
		COLORREF linecolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ���ޱ߿������Բ
	*/
	void EasyX_Gdiplus_SolidEllipse(
		float x,
		float y,
		float w,
		float h,
		COLORREF fillcolor,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ���б߿������Բ
	*/
	void EasyX_Gdiplus_FillEllipse(
		float x,
		float y,
		float w,
		float h,
		COLORREF linecolor,
		COLORREF fillcolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ����״ͼ��������ʱ��Ƕȣ�
	*/
	void EasyX_Gdiplus_Pie(
		float x,
		float y,
		float w,
		float h,
		float stangle,
		float endangle,
		COLORREF linecolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ���ޱ߿�����״ͼ��������ʱ��Ƕȣ�
	*/
	void EasyX_Gdiplus_SolidPie(
		float x,
		float y,
		float w,
		float h,
		float stangle,
		float endangle,
		COLORREF fillcolor,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ���б߿�����״ͼ��������ʱ��Ƕȣ�
	*/
	void EasyX_Gdiplus_FillPie(
		float x,
		float y,
		float w,
		float h,
		float stangle,
		float endangle,
		COLORREF linecolor,
		COLORREF fillcolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief ��Բ����������ʱ��Ƕȣ�
	*/
	void EasyX_Gdiplus_Arc(
		float x,
		float y,
		float w,
		float h,
		float stangle,
		float endangle,
		COLORREF linecolor,
		float linewidth = 1,
		bool enable_alpha = false,
		Gdiplus::SmoothingMode smoothing_mode = Gdiplus::SmoothingModeHighQuality,
		IMAGE* pImg = nullptr
	);

	/**
	 * @brief �� RECT ת��Ϊ RectF
	*/
	Gdiplus::RectF RECTToRectF(RECT x);

	/**
	 * @brief �� RectF ת��Ϊ RECT
	*/
	RECT RectFToRECT(Gdiplus::RectF x);
};

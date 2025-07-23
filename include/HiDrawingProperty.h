/**
 * @file	HiDrawingProperty.h
 * @brief	HiEasyX ��Ļ�ͼ���Դ洢ģ��
 * @author	huidong
*/

#pragma once

#include <graphics.h>

namespace HiEasyX
{
	/**
	 * @brief <pre>
	 *		��ͼ�����ܿص�Ԫ
	 *
	 *	��ע��
	 *		EasyX Ŀǰ�޷���ȡ setorigin �� setcliprgn �����õ�ֵ
	 * </pre>
	*/
	class DrawingProperty
	{
	private:
		bool m_isSaved = false;

	public:
		IMAGE* m_pImg;
		float m_xasp, m_yasp;
		COLORREF m_cBk;
		int m_nBkMode;
		COLORREF m_cFill;
		FILLSTYLE m_fillstyle;
		COLORREF m_cLine;
		LINESTYLE m_linestyle;
		int m_nPolyFillMode;
		int m_nRop2Mode;
		COLORREF m_cText;
		LOGFONT m_font;

		/**
		 * @brief ���浱ǰ���еĻ�ͼ����
		*/
		void SaveProperty();

		/**
		 * @brief ֻ���浱ǰ��ͼ����
		*/
		void SaveWorkingImageOnly();

		/**
		 * @brief Ӧ�ñ�������л�ͼ����
		*/
		void ApplyProperty();

		/**
		 * @brief ֻ�ָ���ͼ����
		*/
		void ApplyWorkingImageOnly();

		/**
		 * @brief �ж��Ƿ񱣴��˻�ͼ����
		*/
		bool IsSaved();

		/**
		 * @brief ���ñ���״̬
		*/
		void Reset();
	};

};

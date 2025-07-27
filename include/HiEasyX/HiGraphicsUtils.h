/**
 * @file	HiGraphicsUtils.h
 * @brief	HiEasyX ��Ļ�ͼʵ�ù���ģ��
 * @author	huidong
*/

#pragma once

#include <graphics.h>
#include <gdiplus.h>
#include <HiEasyX/HiMiscUtils.h>


namespace HiEasyX
{
	/**
	 * @brief <pre>
	 *		��ȡͼ��ߴ�
	 *
	 *	��ע��
	 *		���Է���ش��� IMAGE ָ��Ϊ�գ���ָ������ͼ���ڵ����
	 * </pre>
	 *
	 * @param[in] pImg			Ŀ��ͼ��
	 * @param[out] width		����ͼ���
	 * @param[out] height		����ͼ���
	*/
	void GetImageSize(IMAGE* pImg, int* width, int* height);

	/**
	 * @brief <pre>
	 *		��תͼ�� Alpha ֵ
	 *
	 *	��ע��
	 *		�� alpha ֵ��Ϊ 0 ��һ�����ص� alpha ��Ϊ 0��
	 *		ͬʱ�� alpha ֵΪ 0 ��һ�����ص� alpha ��Ϊ 255��
	 * </pre>
	 *
	 * @param[in, out] pBuf		�Դ�ָ��
	 * @param[in] size			�Դ��С
	 * @return �Դ�ָ�루��ԭ��һ����
	*/
	DWORD* ReverseAlpha(DWORD* pBuf, int size);

	/**
	 * @brief <pre>
	 *		����ָ���ߴ缰��ɫ��ͼ��
	 *
	 *	��ע��
	 *		color ���Դ���͸���ȡ�
	 *		enable_alpha ��Ϊ false ��Ὣ����ͼ��͸������Ϊ 255��
	 * </pre>
	 *
	 * @param[in] w					����ͼ����
	 * @param[in] h					����ͼ��߶�
	 * @param[in] color				����ͼ�������ɫ
	 * @param[in] enable_alpha		�Ƿ�����ͼ��� alpha ��Ϣ
	 * @return �Դ�ָ�루��ԭ��һ����
	*/
	IMAGE CreateImageColor(int w, int h, COLORREF color, bool enable_alpha);

	/**
	 * @brief <pre>
	 *		����ͼ��ı�����ɫ
	 *
	 *	��ע��
	 *		color ���Դ���͸���ȡ�
	 *		enable_alpha ��Ϊ false ��Ὣͼ��͸������Ϊ 255��
	 * </pre>
	 *
	 * @param[in] img				���õ�ͼ��
	 * @param[in] color				ͼ�������ɫ
	 * @param[in] enable_alpha		�Ƿ�������ɫ�� alpha ��Ϣ
	 * @return �Դ�ָ�루��ԭ��һ����
	*/
	void SetImageColor(IMAGE& img, COLORREF color, bool enable_alpha);

	/**
	 * @brief �õ� IMAGE ����� HBITMAP
	 * @param[in] img			Ŀ��ͼ��
	 *
	 * @param[in] enable_alpha <pre>
	 *		�Ƿ�����ͼ��� alpha ��Ϣ
	 *
	 *	ע�⣺
	 *		��ͼ�� alpha ֵȫΪ 0�����ʾ������͸�����
	 * </pre>
	 *
	 * @return ת���õ���λͼ���
	*/
	HBITMAP Image2Bitmap(IMAGE* img, bool enable_alpha);

	/**
	 * @brief �õ� HBITMAP ����� IMAGE
	 * @param[in] img			Ŀ��ͼ��
	 *
	 * @param[in] enable_alpha <pre>
	 *		�Ƿ�����ͼ��� alpha ��Ϣ
	 *
	 *	ע�⣺
	 *		��ͼ�� alpha ֵȫΪ 0�����ʾ������͸�����
	 * </pre>
	 *
	 * @return ת���õ���λͼ���
	*/
	IMAGE Bitmap2Image(HBITMAP* hBitmap, bool enable_alpha);

	/**
	 * @brief HBITMAP ת HICON
	 * @param[in] hBmp λͼ���
	 * @return ͼ����
	*/
	HICON Bitmap2Icon(HBITMAP hBmp);

	/**
	 * @brief ����ͼ�񣨿ɰ���͸��ͨ����
	 * @param[in] Dstimg ָ��Ŀ��λͼ��ָ�루���ֱ�ӻ��Ƶ������������� hiex::GetWindowImage() ��
	 * @param[in] DstimgX Ŀ��λͼ�ϻ��Ƶ����ϽǺ�����
	 * @param[in] DstimgY Ŀ��λͼ�ϻ��Ƶ����Ͻ�������
	 * @param[in] Srcimg ָ��Դλͼ��ָ��
	 * @param[in] transparency ����͸����
	 *
	 *	ע�⣺
	 *		���Ƴ�����λͼ��С���� Srcimg �е�ͼ��һ��
	 * </pre>
	*/
	void TransparentImage(IMAGE* Dstimg, int DstimgX, int DstimgY, IMAGE* Srcimg, int transparency = 255);

	/**
	 * @brief ����ͼ�񣨿ɰ���͸��ͨ����
	 * @param[in] Dstimg ָ��Ŀ��λͼ��ָ�루���ֱ�ӻ��Ƶ������������� hiex::GetWindowImage() ��
	 * @param[in] DstimgX Ŀ��λͼ�ϻ��Ƶ����ϽǺ�����
	 * @param[in] DstimgY Ŀ��λͼ�ϻ��Ƶ����Ͻ�������
	 * @param[in] DstimgWidth Ŀ��λͼ�ϻ��ƵĿ��
	 * @param[in] DstimgHeight Ŀ��λͼ�ϻ��Ƶĸ߶�
	 * @param[in] Srcimg ָ��Դλͼ��ָ��
	 * @param[in] SrcimgX Դλͼ��Ҫ���Ƶ���������ϽǺ�����
	 * @param[in] SrcimgY Դλͼ��Ҫ���Ƶ���������Ͻ�������
	 * @param[in] SourceWidth Դλͼ��Ҫ���Ƶ�����Ŀ��
	 * @param[in] SourceHeight Դλͼ��Ҫ���Ƶ�����ĸ߶�
	 * @param[in] transparency ����͸����
	 *
	 * @note <pre>
	 *	ע�⣺
	 *		�� Ŀ��λͼ�ϻ��ƵĿ�� ������ Դλͼ��Ҫ���Ƶ�����Ŀ�ߣ���ôԴλͼ�е����򽫻ᱻ�����ѹ������ӦĿ��λͼ�ϵĻ�������
	 * </pre>
	*/
	void TransparentImage(IMAGE* Dstimg, int DstimgX, int DstimgY, int DstimgWidth, int DstimgHeight, IMAGE* Srcimg, int SrcimgX, int SrcimgY, int SourceWidth, int SourceHeight, int transparency = 255);

	/**
	 * @brief �ںϣ����ӣ�ͼ��͸����
	 * @param[in] img ָ��ĿԴλͼ��ָ��
	 * @param[in] transparency ��Ҫ�ںϣ����ӣ���͸����
	 * </pre>
	*/
	void OverlayImageTransparency(IMAGE* img, int transparency);

	/**
	 * @brief �Ƴ�ͼ��͸��ͨ��
	 * @param[in] img ָ��ĿԴλͼ��ָ��
	 * </pre>
	*/
	void RemoveImageTransparency(IMAGE* img);

	/**
	 * @brief ����͸���Ȼ����ɫ
	 * @param[in] cDst		ԭλ������
	 * @param[in] cSrc		���������أ�������͸���Ȼ����ɫ��
	 *
	 * @param[in] isCalculated <pre>
	 *		���������ص��Ƿ��Ѿ���������͸����
	 *
	 *	��ע��
	 *		�˲�������һЩ�������������͸�� png ͼ���е����ؾ����Ѿ��˹�͸���ȵġ�
	 * </pre>
	 *
	 * @param[in] alpha		������ src �ϵ�͸���ȣ�Ĭ��Ϊ 255���������ӣ�
	 * @return ��Ϻ����ɫ������ alpha ֵ��
	*/
	COLORREF MixAlphaColor(COLORREF cDst, COLORREF cSrc, bool isCalculated, BYTE alpha = 255);

	/**
	 * @brief <pre>
	 *		���ٸ���ͼ�񣨿ɿ���͸��ͨ����
	 *
	 *	��ע��
	 *		��δ�����κ�͸��ͨ������ͬ��ֱ�Ӹ���ͼ�񡣴�ʱ������ԭͼ���͸������Ϣ�����򲻱���͸������Ϣ��
	 * </pre>
	 *
	 * @param[in] x					ͼ����� x ����
	 * @param[in] y					ͼ����� y ����
	 * @param[in] pDst				����ͼ��ָ��
	 * @param[in] wDst				����ͼ���
	 * @param[in] hDst				����ͼ���
	 * @param[in] pSrc				�����ͼ��ָ��
	 * @param[in] wSrc				�����ͼ���
	 * @param[in] hSrc				�����ͼ���
	 * @param[in] crop				�����ͼ��ü�����δ���ñ�ʾ���ü���
	 * @param[in] alpha				����͸���ȣ�͸�� 0 ~ 255 ��͸����
	 *
	 * @param[in] bUseSrcAlpha <pre>
	 *		�Ƿ�ʹ�ô����ͼ��͸���Ƚ��л�ϣ��뱣֤ IMAGE �к���͸������Ϣ��
	 *
	 *	��ע��
	 *		EasyX �е�ͼ��һ����͸���ȣ�Ĭ����Ϊ 0����ȫ͸��������һ�㲻ʹ��ԭͼ͸���ȡ�
	 *		ͨ��ֻ�� png ͼ�񣬻����ص����ɵ�ͼ��ź���͸������Ϣ��
	 * </pre>
	 *
	 * @param[in] isCalculated <pre>
	 *		��Ǵ����ͼ���Ƿ��Ѿ�����û�Ϻ����ɫ������ͼ��͸����ʱ��Ч��
	 *
	 *	ע�⣺
	 *		png ͼ��������ɫ���ѽ��й�������㡣
	 *		������ԭͼ��㲻�ټ�������ɫ��ֻ������ͼ�������㡣
	 * </pre>
	*/
	void CopyImage_SupAlpha(
		int x,
		int y,
		DWORD* pDst, int wDst, int hDst,
		DWORD* pSrc, int wSrc, int hSrc,
		Optional<RECT> crop = {},
		BYTE alpha = 255,
		bool bUseSrcAlpha = false,
		bool isCalculated = false
	);

	/**
	 * @brief ��תͼ�񣨱���͸����Ϣ������Ӧ��С��
	 * @param[in] pImg			ԭͼ��
	 * @param[in] radian		��ת����
	 * @param[in] bkcolor		���������ɫ
	 * @return	��ת���ͼ��
	*/
	void RotateImage_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, double radian, COLORREF bkcolor = BLACK);

	/**
	 * @brief	����ͼ�񣨴ֲڵġ�������ֵ�����ţ�����͸������Ϣ��
	 * @param[in] srcimg		ԭͼ��
	 * @param[in] width			Ŀ����
	 * @param[in] height		Ŀ��߶ȣ�Ϊ 0 ����ݿ�Ȱ��������ţ�
	 * @return ���ź��ͼ��
	*/
	void ZoomImage_Raw_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, int width, int height = 0);

	/**
	 * @brief	����ͼ��˫���Բ�ֵ������͸������Ϣ��
	 * @param[in] srcimg		ԭͼ��
	 * @param[in] width			Ŀ����
	 * @param[in] height		Ŀ��߶ȣ�Ϊ 0 ����ݿ�Ȱ��������ţ�
	 * @return ���ź��ͼ��
	*/
	void ZoomImage_Interp_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, int width, int height = 0);

	/**
	 * @brief	ͼ�����ţ����� Win32 API���ȽϿ죬����͸������Ϣ��
	 * @param[in] srcimg		ԭͼ��
	 * @param[in] width			Ŀ����
	 * @param[in] height		Ŀ��߶ȣ�Ϊ 0 ����ݿ�Ȱ��������ţ�
	 * @return ���ź��ͼ��
	*/
	void ZoomImage_StretchBlt_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, int width, int height = 0);

	/**
	 * @brief ����ͼ�񣨿ɰ���͸��ͨ����
	 * @param[in] img ָ��ĿԴλͼ��ָ��
	 * @param[in] w ָ���������Ŀ�ȣ�Ϊ 0 �򰴸߶ȱ������ţ�
	 * @param[in] h ָ���������ĸ߶ȣ�Ϊ 0 �򰴿�ȱ������ţ�
	*/
	void ZoomImage_AlphaBlend_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, int w, int h = 0);
}


////////////////********* �궨�� *********////////////////

//
// ׼������͸��ͼ�Σ��Ȼ���ͼ�ε���ʱ�����У�Ȼ�����������Ҫ���Ƶĵط���
// nGraphW, nGraphH		������͸��ͼ�εĿ��
//
// ע�⣺��Ҫ��� DRAW_TNS_RENDER_TO ��ʹ��
//
// ʹ�÷�����
//		�� DRAW_TNS_INIT_GRAPHICS ��� DRAW_TNS_RENDER_TO ��֮�䣬����һ������顣
//		������������ʹ�� Canvas ���� graphics ���л��ơ�
//		����ʱ���� Canvas ����ͨ��ͼ�������ɣ����� GDI+ ϵ�з�װ�������� "GP_" ǰ׺�ĺ�����
//		����ֱ��ʹ��ԭ�� EasyX �������л���Ҳ���ԡ�
//
// ʹ��ʾ����
/*
	// ׼������͸��ͼ�Σ�����ͼ�εĿ�ߣ�
	DRAW_TNS_INIT_GRAPHICS(201, 201);
	{
		// �ڴ������ʹ�� Canvas ����ͨ��ͼ�������л��Ƽ���
		graphics.SetLineThickness(5);
		graphics.FillRoundRect(0, 0, 200, 200, 20, 20, true, GREEN, PURPLE);

		// ������ʹ�� EasyX ԭ����������Ҳ����
		line(20, 20, 50, 50);
	}
	// ���ѡ�����͸��ͼ�λ��Ƶ���������û���͸����
	DRAW_TNS_RENDER_TO(120, 120, yourImagePointer, 100);
*/
//
#define DRAW_TNS_INIT_GRAPHICS(nGraphW, nGraphH) \
	{\
		hiex::Canvas graphics(nGraphW, nGraphH);\
		graphics.BeginBatchDrawing();(0)

//
// ��ɻ���͸��ͼ�Σ���������Ƶ�ͼ��
// nRenderX		���λ�� X ����
// nRenderY		���λ�� Y ����
// pDstImg		͸��ͼ�������Ŀ�껭����IMAGE*��
// alpha		���ͼ��ʱʹ�õ�͸���ȣ���ȫ͸�� 0 ~ 255 ��͸����
//
// ע�⣺��Ҫ��� DRAW_TNS_INIT_GRAPHICS ��ʹ�ã������÷��� DRAW_TNS_INIT_GRAPHICS ���ע��
//
#define DRAW_TNS_RENDER_TO(nRenderX, nRenderY, pDstImg, alpha) \
		graphics.EndBatchDrawing();\
		ReverseAlpha(graphics.GetBuffer(), graphics.GetBufferSize());\
		graphics.RenderTo(nRenderX, nRenderY, pDstImg, { 0 }, alpha, true);\
	}(0)
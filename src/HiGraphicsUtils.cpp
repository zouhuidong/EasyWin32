#include <HiEasyX/HiGraphicsUtils.h>
#include <HiEasyX/HiWindow.h>
#include <HiEasyX/HiMacro.h>

#ifdef _MSC_VER
#pragma comment (lib, "Msimg32.lib")
#endif

namespace HiEasyX
{
	// ��ȡͼ��ߴ�
	// ���Է���ش��� IMAGE ָ��Ϊ�գ���ָ������ͼ���ڵ����
	void GetImageSize(IMAGE* pImg, int* width, int* height)
	{
		pImg = pImg ? pImg : GetWorkingImageHX();
		*width = pImg->getwidth();
		*height = pImg->getheight();
	}

	DWORD* ReverseAlpha(DWORD* pBuf, int size)
	{
		for (int i = 0; i < size; i++)
			if (pBuf[i] & 0xff000000)
				pBuf[i] &= 0x00ffffff;
			else
				pBuf[i] |= 0xff000000;
		return pBuf;
	}

	IMAGE CreateImageColor(int w, int h, COLORREF color, bool enable_alpha)
	{
		IMAGE img(w, h);
		DWORD* pMem = GetImageBufferHX(&img);
		DWORD bgColor = (((enable_alpha ? color >> 24 : 255) << 24) | (GetRValue(color) << 16) | (GetGValue(color) << 8) | GetBValue(color));
		for (int i = 0; i < w * h; i++) pMem[i] = bgColor;
		return img;
	}

	void SetImageColor(IMAGE& img, COLORREF color, bool enable_alpha)
	{
		int w = img.getwidth();
		int h = img.getheight();
		DWORD* pMem = GetImageBufferHX(&img);
		DWORD bgColor = (((enable_alpha ? color >> 24 : 255) << 24) | (GetRValue(color) << 16) | (GetGValue(color) << 8) | GetBValue(color));
		for (int i = 0; i < w * h; i++) pMem[i] = bgColor;
	}

	HBITMAP Image2Bitmap(IMAGE* img, bool enable_alpha)
	{
		// ���Խ���
		// ��ͼ�������κ����� alpha ��Ϊ 0�������� alpha
		// ��ͼ�� alpha ȫ��Ϊ 0�����ʾ��ȫ��͸��

		DWORD* pBuf = GetImageBufferHX(img);
		if (!enable_alpha)
			for (int i = 0; i < img->getwidth() * img->getheight(); i++)
				pBuf[i] &= 0x00ffffff;
		return CreateBitmap(img->getwidth(), img->getheight(), 1, 32, pBuf);
	}

	IMAGE Bitmap2Image(HBITMAP* hBitmap, bool enable_alpha)
	{
		BITMAP bm;
		GetObject(*hBitmap, sizeof(bm), &bm);
		int width = bm.bmWidth;
		int height = bm.bmHeight;

		IMAGE img(width, height);
		DWORD* pBuf = GetImageBufferHX(&img);

		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;

		HDC hMemDC = CreateCompatibleDC(NULL);
		GetDIBits(hMemDC, *hBitmap, 0, height, pBuf, &bmi, DIB_RGB_COLORS);
		DeleteDC(hMemDC);

		if (!enable_alpha)
			for (int i = 0; i < width * height; i++)
				pBuf[i] |= 0xFF000000;

		return img;
	}

	HICON Bitmap2Icon(HBITMAP hBmp)
	{
		BITMAP bmp = {};
		GetObject(hBmp, sizeof(BITMAP), &bmp);

		HBITMAP hbmMask = CreateCompatibleBitmap(GetDC(nullptr), bmp.bmWidth, bmp.bmHeight);

		ICONINFO ii = { 0 };
		ii.fIcon = TRUE;
		ii.hbmColor = hBmp;
		ii.hbmMask = hbmMask;

		HICON hIcon = CreateIconIndirect(&ii);
		DeleteObject(hbmMask);

		return hIcon;
	}

	void TransparentImage(IMAGE* Dstimg, int DstimgX, int DstimgY, IMAGE* Srcimg, int transparency)
	{
		HDC dstDC = GetImageHDCHX(Dstimg);
		HDC srcDC = GetImageHDCHX(Srcimg);
		int w = Srcimg->getwidth();
		int h = Srcimg->getheight();

		// �ṹ��ĵ�������Ա��ʾ�����͸���ȣ�0 ��ʾȫ͸����255 ��ʾ��͸����
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)transparency, AC_SRC_ALPHA };
		// ʹ�� Windows GDI ����ʵ�ְ�͸��λͼ
		AlphaBlend(dstDC, DstimgX, DstimgY, w, h, srcDC, 0, 0, w, h, bf);
	}

	void TransparentImage(IMAGE* Dstimg, int DstimgX, int DstimgY, int DstimgWidth, int DstimgHeight, IMAGE* Srcimg, int SrcimgX, int SrcimgY, int SourceWidth, int SourceHeight, int transparency)
	{
		HDC dstDC = GetImageHDCHX(Dstimg);
		HDC srcDC = GetImageHDCHX(Srcimg);

		// �ṹ��ĵ�������Ա��ʾ�����͸���ȣ�0 ��ʾȫ͸����255 ��ʾ��͸����
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)transparency, AC_SRC_ALPHA };
		// ʹ�� Windows GDI ����ʵ�ְ�͸��λͼ
		AlphaBlend(dstDC, DstimgX, DstimgY, DstimgWidth, DstimgHeight, srcDC, SrcimgX, SrcimgY, SourceWidth, SourceHeight, bf);
	}

	COLORREF MixAlphaColor(COLORREF cDst, COLORREF cSrc, bool isCalculated, BYTE alpha)
	{
		float fSrc = GetAValue(cSrc) / 255.0f;	// ���������ص�͸����
		if (alpha != 255)						// ����͸����
			fSrc *= alpha / 255.0f;
		if (fSrc == 0.0f)						// ����͸����Ϊ 0 ʱ�����κδ���
			return cDst;
		float fDst = 1 - fSrc;					// ԭλ������Ӧ��͸����

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

	void CopyImage_SupAlpha(int x, int y, DWORD* pDst, int wDst, int hDst, DWORD* pSrc, int wSrc, int hSrc, Optional<RECT> crop, BYTE alpha, bool bUseSrcAlpha, bool isCalculated)
	{
		RECT rctSrcWhole = RECT{ 0, 0, wSrc, hSrc };
		if (!crop.isSet)
		{
			crop = rctSrcWhole;
		}
		else
		{
			IntersectRect(&crop.value, &crop.value, &rctSrcWhole);	// ȷ���ü�������ԭͼ��Χ��
		}

		int crop_w = crop.value.right - crop.value.left;	// �ü�������
		int crop_h = crop.value.bottom - crop.value.top;	// �ü�����߶�

		int availble_w = wDst - x;	// ���ÿ��
		int availble_h = hDst - y;	// ���ø߶�

		int limit_w = min(crop_w, availble_w);	// ʵ�ʿ��ÿ��
		int limit_h = min(crop_h, availble_h);	// ʵ�ʿ��ø߶�

		int src_end_x = x + limit_w;
		int src_end_y = y + limit_h;

		// ��ǲ�ʹ���κ�͸��ͨ��
		bool bNoAlpha = false;
		float fTransparent = alpha / 255.0f;
		if (alpha == 255 && !bUseSrcAlpha)	bNoAlpha = true;

		// ��ֻ֤����ʹ��ԭͼ͸����ʱ���Ѽ����ǲ��ܿ���
		if (!bUseSrcAlpha)	isCalculated = false;

		// i j -> ԭͼ�����������λ��Ϊ�������Թ�������Χ���֣�
		// nx ny -> ����ͼ������
		for (int j = 0; j < limit_h; j++)
		{
			int indexSrc = (y + j) * wSrc + x;
			int indexDst = (crop.value.top + j) * wDst + crop.value.left;
			for (int i = 0; i < limit_w; i++)
			{
				indexSrc++;
				indexDst++;

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

	void RotateImage_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, double radian, COLORREF bkcolor)
	{
		radian = -radian;														// ���� y �ᷭת����ת�Ƕ���Ҫ�为
		float fSin = (float)sin(radian), fCos = (float)cos(radian);				// �洢���Ǻ���ֵ
		float fNSin = (float)sin(-radian), fNCos = (float)cos(-radian);
		int left = 0, top = 0, right = 0, bottom = 0;							// ��ת��ͼ�񶥵�
		int w, h;
		GetImageSize(pSrcImg, &w, &h);
		DWORD* pBuf = GetImageBufferHX(pSrcImg);
		POINT points[4] = { { 0, 0 },{ w, 0 },{ 0, h },{ w, h } };				// �洢ͼ�񶥵�
		for (int j = 0; j < 4; j++)												// ��תͼ�񶥵㣬������ת���ͼ��߽�
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

		int nw = points[right].x - points[left].x;								// ��ת���ͼ��ߴ�
		int nh = points[top].y - points[bottom].y;
		int nSize = nw * nh;
		int offset_x = points[left].x < 0 ? points[left].x : 0;					// ��ת��ͼ�񳬳���һ���޵�λ�ƣ��ݴ˵���ͼ��λ�ã�
		int offset_y = points[bottom].y < 0 ? points[bottom].y : 0;

		pDstImg->Resize(nw, nh);
		DWORD* pNewBuf = GetImageBufferHX(pDstImg);
		if (bkcolor != BLACK)													// ����ͼ�񱳾�ɫ
			for (int i = 0; i < nSize; i++)
				pNewBuf[i] = BGR(bkcolor);

		for (int i = offset_x, ni = 0; ni < nw; i++, ni++)						// i ����ӳ��ԭͼ�����꣬ni ���ڶ�λ��ת��ͼ������
		{
			for (int j = offset_y, nj = 0; nj < nh; j++, nj++)
			{
				int nx = (int)(i * fNCos - j * fNSin);							// ����ת���ͼ��������ԭͼ������ӳ��
				int ny = (int)(i * fNSin + j * fNCos);
				if (nx >= 0 && nx < w && ny >= 0 && ny < h)						// ��Ŀ��ӳ����ԭͼ��Χ�ڣ��򿽱�ɫֵ
					pNewBuf[nj * nw + ni] = pBuf[ny * w + nx];
			}
		}
	}

	void ZoomImage_Raw_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, int width, int height)
	{
		int src_width, src_height;
		GetImageSize(pSrcImg, &src_width, &src_height);

		// �Զ�����
		if (height == 0)
		{
			height = width * src_height / src_width;
		}

		pDstImg ->Resize(width, height);

		DWORD* dst = GetImageBufferHX(pDstImg);
		DWORD* src = GetImageBufferHX(pSrcImg);

		float w_scale_rate = (float)src_width / width;
		float h_scale_rate = (float)src_height / height;

		for (int iy = 0; iy < height; iy++)
		{
			for (int ix = 0; ix < width; ix++)
			{
				// ����ԭͼ��Ӧ����
				int x_scale = (int)(w_scale_rate * ix);
				int y_scale = (int)(h_scale_rate * iy);
				dst[ix + iy * width] = src[(int)(x_scale + y_scale * src_width)];
			}
		}
	}

	void ZoomImage_Interp_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, int width, int height)
	{
		int old_w, old_h;
		GetImageSize(pSrcImg, &old_w, &old_h);

		// �Զ�����
		if (height == 0)
		{
			height = width * old_h / old_w;
		}

		pDstImg->Resize(width, height);
		DWORD* src = GetImageBufferHX(pSrcImg);
		DWORD* dst = GetImageBufferHX(pDstImg);

		// ˫���Բ�ֵ����Ϊ����ȡ�������� w, h ��Ҫ��һ����Խ�磩
		for (int i = 0; i < height - 1; i++)
		{
			for (int j = 0; j < width - 1; j++)
			{
				int xt = j * old_w / width;		// ��ͼ����ӳ�䵽ԭͼ�ϵ�λ��
				int yt = i * old_h / height;

				// ʵ�����м���ͼƬ
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

				// ��ֵ������ alpha
				dst[i * width + j] = ARGB(GetAValue(src[xt + yt * old_w]), r, g, b);
			}
		}
	}

	void ZoomImage_StretchBlt_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, int width, int height)
	{
		int w, h;
		GetImageSize(pSrcImg, &w, &h);

		// �Զ�����
		if (height == 0)
		{
			height = width * h / w;
		}

		IMAGE img(width, height);
		StretchBlt(
			GetImageHDCHX(pDstImg), 0, 0, width, height,
			GetImageHDCHX(pSrcImg), 0, 0,
			w, h, SRCCOPY
		);
	}

	void ZoomImage_AlphaBlend_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, int w, int h)
	{
		int DstW = pDstImg->getwidth();
		int DstH = pDstImg->getheight();

		if (w == 0) w = int(DstW * (double(h) / double(DstH)));
		if (h == 0) h = int(DstH * (double(w) / double(DstW)));

		HDC DstDC = GetImageHDCHX(pDstImg);

		Resize(pDstImg, w, h);
		SetImageColor(*pDstImg, 0, true);
		HDC SrcDC = GetImageHDCHX(pDstImg);

		// �ṹ��ĵ�������Ա��ʾ�����͸���ȣ�0 ��ʾȫ͸����255 ��ʾ��͸����
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		// ʹ�� Windows GDI ����ʵ�ְ�͸��λͼ
		AlphaBlend(SrcDC, 0, 0, w, h, DstDC, 0, 0, DstW, DstH, bf);
	}

	void OverlayImageTransparency(IMAGE* img, int transparency)
	{
		int width = img->getwidth();
		int height = img->getheight();

		DWORD* pBuf = GetImageBufferHX(img);
		for (int i = 0; i < width * height; i++)
		{
			int alpha = (pBuf[i] >> 24) * transparency / 255;
			pBuf[i] = (pBuf[i] & 0x00FFFFFF) | (alpha << 24);
		}
	}

	void RemoveImageTransparency(IMAGE* img)
	{
		DWORD* pBuf = GetImageBufferHX(img);
		for (int i = 0; i < img->getwidth() * img->getheight(); i++)
			pBuf[i] |= 0xFF000000;
	}
}

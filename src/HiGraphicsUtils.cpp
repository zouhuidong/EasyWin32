#include <HiEasyX/HiGraphicsUtils.h>
#include <HiEasyX/HiWindow.h>
#include <HiEasyX/HiMacro.h>

#ifdef _MSC_VER
#pragma comment (lib, "Msimg32.lib")
#endif

namespace HiEasyX
{
	// 获取图像尺寸
	// 可以方便地处理 IMAGE 指针为空，即指向主绘图窗口的情况
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
		// 测试结论
		// 若图像中有任何像素 alpha 不为 0，则启用 alpha
		// 若图像 alpha 全部为 0，则表示完全不透明

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

		// 结构体的第三个成员表示额外的透明度，0 表示全透明，255 表示不透明。
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)transparency, AC_SRC_ALPHA };
		// 使用 Windows GDI 函数实现半透明位图
		AlphaBlend(dstDC, DstimgX, DstimgY, w, h, srcDC, 0, 0, w, h, bf);
	}

	void TransparentImage(IMAGE* Dstimg, int DstimgX, int DstimgY, int DstimgWidth, int DstimgHeight, IMAGE* Srcimg, int SrcimgX, int SrcimgY, int SourceWidth, int SourceHeight, int transparency)
	{
		HDC dstDC = GetImageHDCHX(Dstimg);
		HDC srcDC = GetImageHDCHX(Srcimg);

		// 结构体的第三个成员表示额外的透明度，0 表示全透明，255 表示不透明。
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)transparency, AC_SRC_ALPHA };
		// 使用 Windows GDI 函数实现半透明位图
		AlphaBlend(dstDC, DstimgX, DstimgY, DstimgWidth, DstimgHeight, srcDC, SrcimgX, SrcimgY, SourceWidth, SourceHeight, bf);
	}

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

	void CopyImage_SupAlpha(int x, int y, DWORD* pDst, int wDst, int hDst, DWORD* pSrc, int wSrc, int hSrc, Optional<RECT> crop, BYTE alpha, bool bUseSrcAlpha, bool isCalculated)
	{
		RECT rctSrcWhole = RECT{ 0, 0, wSrc, hSrc };
		if (!crop.isSet)
		{
			crop = rctSrcWhole;
		}
		else
		{
			IntersectRect(&crop.value, &crop.value, &rctSrcWhole);	// 确保裁剪区域在原图范围内
		}

		int crop_w = crop.value.right - crop.value.left;	// 裁剪区域宽度
		int crop_h = crop.value.bottom - crop.value.top;	// 裁剪区域高度

		int availble_w = wDst - x;	// 可用宽度
		int availble_h = hDst - y;	// 可用高度

		int limit_w = min(crop_w, availble_w);	// 实际可用宽度
		int limit_h = min(crop_h, availble_h);	// 实际可用高度

		int src_end_x = x + limit_w;
		int src_end_y = y + limit_h;

		// 标记不使用任何透明通道
		bool bNoAlpha = false;
		float fTransparent = alpha / 255.0f;
		if (alpha == 255 && !bUseSrcAlpha)	bNoAlpha = true;

		// 保证只有在使用原图透明度时，已计算标记才能开启
		if (!bUseSrcAlpha)	isCalculated = false;

		// i j -> 原图索引（若输出位置为负，则略过超出范围部分）
		// nx ny -> 载体图像索引
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
		radian = -radian;														// 由于 y 轴翻转，旋转角度需要变负
		float fSin = (float)sin(radian), fCos = (float)cos(radian);				// 存储三角函数值
		float fNSin = (float)sin(-radian), fNCos = (float)cos(-radian);
		int left = 0, top = 0, right = 0, bottom = 0;							// 旋转后图像顶点
		int w, h;
		GetImageSize(pSrcImg, &w, &h);
		DWORD* pBuf = GetImageBufferHX(pSrcImg);
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

		pDstImg->Resize(nw, nh);
		DWORD* pNewBuf = GetImageBufferHX(pDstImg);
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
	}

	void ZoomImage_Raw_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, int width, int height)
	{
		int src_width, src_height;
		GetImageSize(pSrcImg, &src_width, &src_height);

		// 自动缩放
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
				// 计算原图相应坐标
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

		// 自动缩放
		if (height == 0)
		{
			height = width * old_h / old_w;
		}

		pDstImg->Resize(width, height);
		DWORD* src = GetImageBufferHX(pSrcImg);
		DWORD* dst = GetImageBufferHX(pDstImg);

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
	}

	void ZoomImage_StretchBlt_SupAlpha(IMAGE* pDstImg, IMAGE* pSrcImg, int width, int height)
	{
		int w, h;
		GetImageSize(pSrcImg, &w, &h);

		// 自动缩放
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

		// 结构体的第三个成员表示额外的透明度，0 表示全透明，255 表示不透明。
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		// 使用 Windows GDI 函数实现半透明位图
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

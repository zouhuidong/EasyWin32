#include <HiEasyX/HiGraphicsUtils.h>
#include <HiEasyX/HiWindow.h>

#ifdef _MSC_VER
#pragma comment (lib, "Msimg32.lib")
#endif

namespace HiEasyX
{
	// 获取图像尺寸
	// 可以方便地处理 IMAGE 指针为空，即指向主绘图窗口的情况
	void GetImageSize(IMAGE* pImg, int* width, int* height)
	{
		pImg = pImg ? pImg : GetWorkingImage();
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

	bool ZoomImage_AlphaBlend_Alpha(IMAGE* img, int w, int h)
	{
		if (w == 0 && h == 0) return false;

		int DstW = img->getwidth();
		int DstH = img->getheight();

		if (w == 0) w = int(DstW * (double(h) / double(DstH)));
		if (h == 0) h = int(DstH * (double(w) / double(DstW)));

		HDC DstDC = GetImageHDCHX(img);

		Resize(img, w, h);
		SetImageColor(*img, 0, true);
		HDC SrcDC = GetImageHDCHX(img);

		// 结构体的第三个成员表示额外的透明度，0 表示全透明，255 表示不透明。
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		// 使用 Windows GDI 函数实现半透明位图
		AlphaBlend(SrcDC, 0, 0, w, h, DstDC, 0, 0, DstW, DstH, bf);

		return true;
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

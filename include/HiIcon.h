/**
 * @file	HiIcon.h
 * @brief	HiEasyX ���ͼ��ģ��
 * @note	�洢 HiEasyX ͼ��
 * @author	huidong
*/

#pragma once

#include <graphics.h>

// ͼ���С
#define HIICON_WIDTH	64
#define HIICON_HEIGHT	64

namespace HiEasyX
{
	/**
	 * @brief ��ȡͼ���Դ�
	 * @see ͼ���С HIICON_WIDTH, HIICON_HEIGHT
	 * @return �Դ�ָ��
	*/
	DWORD* GetIconImageBuffer();

	/**
	 * @brief ��ȡͼ������Դ�Ŀ�����
	*/
	IMAGE* GetIconImage();
};

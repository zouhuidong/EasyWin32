/**
 * @file	HiMiscUtils.h
 * @brief	HiEasyX ��ĳ��������
 * @author	huidong
*/

#pragma once

#include <Windows.h>
#include <WinUser.h>
#include <graphics.h>

namespace HiEasyX
{

	/**
	 * @brief �洢������Ļ�Ĵ�С��Ϣ������ʾ����
	*/
	struct ScreenSize
	{
		int left;	///< ����ʾ�������Ͻ� x ����
		int top;	///< ����ʾ�������Ͻ� y ����
		int w;		///< ����ʾ�����ܺͿ��
		int h;		///< ����ʾ�����ܺ͸߶�
	};

	/**
	 * @brief ��ȡ����ʾ����С��Ϣ
	*/
	ScreenSize GetScreenSize();

	/**
	 * @brief ��ȷ��ʱ���������Ծ�ȷ�� 1ms������ ��1ms��
	 * @author yangw80 <yw80@qq.com>
	 * @date 2011-5-4
	 * @param[in] ms ��ʱ���ȣ���λ�����룩
	*/
	void HpSleep(int ms);

	/**
	 * @brief ���Ƿ�λ�ھ�����
	 * @param[in] x		λ��
	 * @param[in] y		λ��
	 * @param[in] rct		����
	 * @return ���Ƿ�λ�ھ�����
	*/
	bool IsInRect(int x, int y, RECT rct);


	// ��ѡ����
	template<typename T>
	struct Optional
	{
		T value{};          ///< ����ֵ
		bool isSet = false; ///< �Ƿ������˲���

		// Ĭ�Ϲ��캯��
		Optional() = default;

		// ��ֵ��ʼ�������Ϊ�����ã�
		Optional(const T& val) : value(val), isSet(true) {}

		// ��ʽת��Ϊԭʼ���ͣ�δ����ʱ����Ĭ��ֵ��
		operator T() const
		{
			return isSet ? value : T{};
		}
	};
}

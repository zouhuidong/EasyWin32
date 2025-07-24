/**
 * @file	HiFPS.h
 * @brief	HiEasyX ���֡��ģ��
 * @author	huidong
*/

#pragma once

#include <HiEasyX/HiMiscUtils.h>

#include <ctime>
#include <thread>

namespace HiEasyX
{
	struct tDelayFPS
	{
		clock_t tRecord;
		tDelayFPS();  // �������캯�������彫�� HiFPS.cpp �н���
	};

	/**
	 * @brief ����Ŀ��֡����ʱ
	 * @param[in] fps			֡��
	 * @param[in] wait_long		�Ƿ񳤵ȴ�������ռ�ã�
	*/

	// DelayFPS �� 24/01/31 ����
	// �˴θ��¶�����������иĶ�����Ŀ���ǽ�����߳�ʹ��ʱ���������⣬����߾���
	// ͬʱ�� HpSleep(delay); ��Ϊ std::this_thread::sleep_for(std::chrono::milliseconds(delay)); �����ڽ�һ����߾���
	// this_thread::sleep_for �����������⣬�� https://developercommunity.visualstudio.com/t/Modifying-the-system-time-to-the-past-s/10476559 �У�һ�Դ˺��������޸�����ȷ�� MSVC �汾�Ա������
	// ʹ��ģ��
	/*
		static hiex::tDelayFPS recond;
		hiex::DelayFPS(recond, 24);
	*/
	// ������ѭ����һ������ hiex::tDelayFPS recond; ��Ҫȷ��ÿ���߳������Լ��� tDelayFPS������̲߳���ʹ��ͬһ�� tDelayFPS
	void DelayFPS(tDelayFPS& Record, int fps, bool wait_long = false);
};

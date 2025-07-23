#include "HiFPS.h"
#include "HiWindow.h"

namespace HiEasyX
{
	tDelayFPS::tDelayFPS() : tRecord(0) {}

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
	void DelayFPS(tDelayFPS& Record, int fps, bool wait_long)
	{
		if (wait_long)
		{
			SleepHX(500);
			return;
		}

		clock_t tNow = clock();
		if (Record.tRecord)
		{
			int delay = 1000 / fps - (tNow - Record.tRecord);
			if (delay > 0)
			{
				SleepHX(delay);
				//std::this_thread::sleep_for(std::chrono::milliseconds(delay));
				//HpSleep(delay);
			}
		}
		Record.tRecord = clock();
	}
};
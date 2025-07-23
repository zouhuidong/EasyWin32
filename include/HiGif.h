/**
 * @file	HiGif.h
 * @brief	HiEasyX ��Ķ�ͼģ��
 * @author	��ϡ_yixy��huidong���޸ģ�
*/

#pragma once

#include <graphics.h>
#include <gdiplus.h>
#include <time.h>
#include <stdio.h>

namespace HiEasyX
{
	/**
	 * @brief <pre>
	 *		Gif ��ͼ
	 *
	 *	ʹ�÷�����
	 *		1. load ����ͼ��
	 *		2. bind ����� HDC
	 *		3. ����ѡ��setPos ����λ��
	 *		4. ����ѡ��setSize �������Ŵ�С��Ϊ 0 ��ʾԭͼ��С��
	 *		5. play ��ʼ����
	 *		6. draw ���Ƶ� HDC
	 * </pre>
	 * 
	 * @bug <pre>
	 *		1. �ͷ�ʱ���ܱ���
	 *		2. �԰󶨵� HDC ������С���ܵ��±���
	 * </pre>
	*/
	class Gif
	{
	private:

		int x, y;
		int width, height;
		int frameCount;					///< ֡��

		HDC hdc;						///< �豸���
		Gdiplus::Graphics* graphics;	///< ͼ�ζ���

		Gdiplus::Bitmap* gifImage;		///< gif ͼ��
		Gdiplus::PropertyItem* pItem;	///< ֡��ʱ����

		int curFrame;					///< ��ǰ֡
		clock_t pauseTime;				///< ��ͣʱ��

		clock_t	frameBaseTime;			///< ֡��׼ʱ��
		clock_t	curDelayTime;			///< ��ǰ֡���Ѳ���ʱ��
		clock_t	frameDelayTime;			///< ��ǰ֡������ʱʱ��

		bool playing;					///< �Ƿ񲥷�
		bool visible;					///< �Ƿ�ɼ�

	public:

		Gif(const WCHAR* gifFileName = nullptr, HDC hdc = GetImageHDC());
		Gif(const Gif& gif);

		virtual ~Gif();

		Gif& operator=(const Gif& gif);

		/**
		 * @brief ����ͼ��
		 * @param[in] gifFileName ͼ���ļ�·��
		*/
		void load(const WCHAR* gifFileName);

		/**
		 * @brief ���豸
		 * @param[in] hdc ��ͼ�豸
		*/
		void bind(HDC hdc);

		/**
		 * @brief ���ͼ��
		*/
		void clear();

		// λ��
		void setPos(int x, int y);
		void setSize(int width, int height);

		int getX() const { return x; }
		int getY() const { return y; }

		// ���ú��ͼ���С��Ϊ 0 ��ʾʹ��ԭͼ��С��
		int getWidth() const { return width; }
		int getHeight() const { return height; }

		// ԭͼ��С
		int getOrginWidth() const;
		int getOrginHeight() const;

		// ֡��Ϣ
		int getFrameCount() const { return frameCount; }
		int getCurFrame() const { return curFrame; }

		// ��ʱʱ���ȡ������
		int getDelayTime(int frame) const;
		void setDelayTime(int frame, long time_ms);
		void setAllDelayTime(long time_ms);

		// ����ʱ�䣬���㵱ǰ֡
		void updateTime();

		// ���Ƶ�ǰ֡��ָ��֡
		void draw();
		void draw(int x, int y);
		void drawFrame(int frame);
		void drawFrame(int frame, int x, int y);

		/**
		 * @brief ��ȡͼ��
		 * @param[in] pimg		����
		 * @param[in] frame	֡����
		*/
		void getimage(IMAGE* pimg, int frame);

		// ����״̬����
		void play();
		void pause();
		void toggle();

		bool IsPlaying()const { return playing; }

		void setVisible(bool enable) { visible = enable; }
		bool IsVisible() const { return visible; }

		bool IsAnimation() const { return frameCount > 1; }

		/**
		 * @brief ���ò���״̬
		*/
		void resetPlayState();

		void info() const;

	private:

		/**
		 * @brief ��ʼ��
		*/
		void init();

		/**
		 * @brief ��ȡͼ����Ϣ
		*/
		void read();

		void copy(const Gif& gif);
	};

};

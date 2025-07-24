/**
 * @file	HiMouseDrag.h
 * @brief	HiEasyX �������϶�����ģ��
 * @author	huidong
*/

#pragma once

#include <graphics.h>

namespace HiEasyX
{
	/**
	 * @brief <pre>
	 *		����϶��¼�������
	 *	
	 *	���÷�����
	 *		1. ���� UpdateMessage ���������Ϣ
	 *		2. ���� isLeftDrag��isMiddleDrag��isRightDrag �����ж������϶�����갴��
	 *		3. ���� GetDragX��GetDragY ��ȡ����϶�ʱ�������ı仯��
	 * </pre>
	*/
	class MouseDrag
	{
	private:
		ExMessage old = { 0 }, msg = { 0 };
		int dx = 0, dy = 0;
		bool lbtn = false, mbtn = false, rbtn = false;
		bool ldrag = false, mdrag = false, rdrag = false;
		bool newmsg = false;

		bool UpdateDragInfo(bool& btn, int msgid_down, int msgid_up);
		
	public:

		void UpdateMessage(ExMessage m);	///< ���������Ϣ

		bool IsLeftDrag();					///< �������Ƿ��϶�
		bool IsMiddleDrag();				///< ����м��Ƿ��϶�
		bool IsRightDrag();					///< ����Ҽ��Ƿ��϶�

		int GetDragX();						///< ��ȡ�϶��� x ����ƫ����
		int GetDragY();						///< ��ȡ�϶��� y ����ƫ����
	};

};

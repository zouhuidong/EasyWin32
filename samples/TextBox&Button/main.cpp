////////////////////////////////////////
// ���򣺻��� EasyX ���ı��� + ��ť�ؼ�
// ԭ���ߣ�BestAns
// ���뻷����VS2019��EasyX_20211109
// ��д���ڣ�2021-10-28
// ����޸ģ�2021-11-20
// 
// �޸���: huidong <mailhuid@163.com>
// �޸����ڣ�2022-07-15
// �޸�����: ������Ӧ���� EasyWin32
//

#include "EasyWin32.h"

// ʵ���ı���ؼ�
class EasyTextBox
{
private:
	int left = 0, top = 0, right = 0, bottom = 0;	// �ؼ�����
	wchar_t* text = NULL;							// �ؼ�����
	size_t maxlen = 0;									// �ı���������ݳ���

public:
	void Create(int x1, int y1, int x2, int y2, int max)
	{
		maxlen = max;
		text = new wchar_t[maxlen];
		text[0] = 0;
		left = x1, top = y1, right = x2, bottom = y2;

		// �����û�����
		Show();
	}

	~EasyTextBox()
	{
		if (text != NULL)
			delete[] text;
	}

	wchar_t* Text()
	{
		return text;
	}

	bool Check(int x, int y)
	{
		return (left <= x && x <= right && top <= y && y <= bottom);
	}

	// ���ƽ���
	void Show()
	{
		BEGIN_TASK();

		// ���ݻ���ֵ
		int oldlinecolor = getlinecolor();
		int oldbkcolor = getbkcolor();
		int oldfillcolor = getfillcolor();

		setlinecolor(LIGHTGRAY);		// ���û�����ɫ
		setbkcolor(0xeeeeee);			// ���ñ�����ɫ
		setfillcolor(0xeeeeee);			// ���������ɫ
		fillrectangle(left, top, right, bottom);
		outtextxy(left + 10, top + 5, text);

		// �ָ�����ֵ
		setlinecolor(oldlinecolor);
		setbkcolor(oldbkcolor);
		setfillcolor(oldfillcolor);

		END_TASK();
		FLUSH_DRAW();
	}

	void OnMessage()
	{
		EasyWin32::BeginTask();

		// ���ݻ���ֵ
		int oldlinecolor = getlinecolor();
		int oldbkcolor = getbkcolor();
		int oldfillcolor = getfillcolor();

		setlinecolor(BLACK);			// ���û�����ɫ
		setbkcolor(WHITE);				// ���ñ�����ɫ
		setfillcolor(WHITE);			// ���������ɫ
		fillrectangle(left, top, right, bottom);
		outtextxy(left + 10, top + 5, text);

		int width = textwidth(text);	// �ַ����ܿ��
		int counter = 0;				// �����˸������
		bool binput = true;				// �Ƿ�������

		EasyWin32::EndTask();
		EasyWin32::EnforceRedraw();

		ExMessage msg;
		while (binput)
		{
			BEGIN_TASK();
			while (binput && peekmessage(&msg, EM_MOUSE | EM_CHAR, false))	// ��ȡ��Ϣ����������Ϣ�����ó�
			{
				if (msg.message == WM_LBUTTONDOWN)
				{
					// ���������ı������棬�����ı�����
					if (msg.x < left || msg.x > right || msg.y < top || msg.y > bottom)
					{
						binput = false;
						break;
					}
				}
				else if (msg.message == WM_CHAR)
				{
					size_t len = wcslen(text);
					switch (msg.ch)
					{
					case '\b':				// �û����˸����ɾ��һ���ַ�
						if (len > 0)
						{
							text[len - 1] = 0;
							width = textwidth(text);
							counter = 0;
							clearrectangle(left + 10 + width, top + 1, right - 1, bottom - 1);
						}
						break;
					case '\r':				// �û����س����������ı�����
					case '\n':
						binput = false;
						break;
					default:				// �û����������������ı�����
						if (len < maxlen - 1)
						{
							text[len++] = msg.ch;
							text[len] = 0;
							clearrectangle(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);	// ������Ĺ��
							width = textwidth(text);				// ���¼����ı�����
							counter = 0;
							outtextxy(left + 10, top + 5, text);		// ����µ��ַ���
						}
					}
				}
				peekmessage(NULL, EM_MOUSE | EM_CHAR);				// ����Ϣ���������ոմ������һ����Ϣ
				END_TASK();
				FLUSH_DRAW();

				Sleep(10);
			}

			BEGIN_TASK();

			// ���ƹ�꣨�����˸����Ϊ 20ms * 32��
			counter = (counter + 1) % 32;
			if (counter < 16)
				line(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);				// �����
			else
				clearrectangle(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);		// �����

			END_TASK();
			FLUSH_DRAW();

			// ��ʱ 20ms
			Sleep(20);
		}

		BEGIN_TASK();
		clearrectangle(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);	// �����

		// �ָ�����ֵ
		setlinecolor(oldlinecolor);
		setbkcolor(oldbkcolor);
		setfillcolor(oldfillcolor);

		END_TASK();
		FLUSH_DRAW();

		Show();
	}
};



// ʵ�ְ�ť�ؼ�
class EasyButton
{
private:
	int left = 0, top = 0, right = 0, bottom = 0;	// �ؼ�����
	wchar_t* text = NULL;							// �ؼ�����
	void (*userfunc)() = NULL;						// �ؼ���Ϣ

public:
	void Create(int x1, int y1, int x2, int y2, const wchar_t* title, void (*func)())
	{
		text = new wchar_t[wcslen(title) + 1];
		wcscpy_s(text, wcslen(title) + 1, title);
		left = x1, top = y1, right = x2, bottom = y2;
		userfunc = func;

		// �����û�����
		Show();
	}

	~EasyButton()
	{
		if (text != NULL)
			delete[] text;
	}

	bool Check(int x, int y)
	{
		return (left <= x && x <= right && top <= y && y <= bottom);
	}

	// ���ƽ���
	void Show()
	{
		BEGIN_TASK();

		int oldlinecolor = getlinecolor();
		int oldbkcolor = getbkcolor();
		int oldfillcolor = getfillcolor();

		setlinecolor(BLACK);			// ���û�����ɫ
		setbkcolor(WHITE);				// ���ñ�����ɫ
		setfillcolor(WHITE);			// ���������ɫ
		fillrectangle(left, top, right, bottom);
		outtextxy(left + (right - left - textwidth(text) + 1) / 2, top + (bottom - top - textheight(text) + 1) / 2, text);

		setlinecolor(oldlinecolor);
		setbkcolor(oldbkcolor);
		setfillcolor(oldfillcolor);

		END_TASK();
		FLUSH_DRAW();
	}

	void OnMessage()
	{
		if (userfunc != NULL)
			userfunc();
	}
};



// ����ؼ�
EasyTextBox txtName;
EasyTextBox txtPwd;
EasyButton btnOK;



// ��ť btnOK �ĵ���¼�
void On_btnOk_Click()
{
	if (wcscmp(L"123", txtPwd.Text()))
		MessageBox(GetHWnd(), L"�������", L"����", MB_OK);
	else
	{
		wchar_t s[100] = L"Hello, ";
		wcscat_s(s, 100, txtName.Text());
		MessageBox(GetHWnd(), s, L"Hello", MB_OK);
	}
}



// ������
int main()
{
	// ����ͼ�δ���
	initgraph(640, 480);

	BEGIN_TASK();

	// �򵥻��ƽ���
	setbkcolor(0xeeeeee);
	cleardevice();
	settextcolor(BLACK);
	outtextxy(50, 55, L"�û�����");
	txtName.Create(120, 50, 400, 75, 10);						// �����û����ı���ؼ�
	outtextxy(50, 105, L"�ܡ��룺");
	txtPwd.Create(120, 100, 400, 125, 10);						// ���������ı���ؼ�
	btnOK.Create(320, 150, 400, 175, L"OK", On_btnOk_Click);	// ������ť�ؼ�

	END_TASK();
	FLUSH_DRAW();

	ExMessage msg;
	while (true)
	{
		msg = getmessage(EM_MOUSE);			// ��ȡ��Ϣ����

		if (msg.message == WM_LBUTTONDOWN)
		{
			// �жϿؼ�
			if (txtName.Check(msg.x, msg.y))	txtName.OnMessage();

			// �жϿؼ�
			if (txtPwd.Check(msg.x, msg.y))		txtPwd.OnMessage();

			// �жϿؼ�
			if (btnOK.Check(msg.x, msg.y))		btnOK.OnMessage();
		}
	}

	// �رջ�ͼ����
	closegraph();
	return 0;
}

#include "HiEasyX.h"
using namespace HiEasyX;

int main()
{
	Window wnd;
	wnd.Create();
	wnd.GetCanvas()->CenterText(HXStr("Hello World"));
	wnd.FlushBuffer();
	wnd.GetMsg(EX_KEY);
	return 0;
}
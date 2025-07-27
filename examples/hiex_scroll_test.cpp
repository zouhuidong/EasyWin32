#include <HiEasyX.h>

int main()
{
	hiex::Window wnd(520, 520);

	// settings
	wnd.SetStyleHX(hiex::Resizable, true);
	wnd.AutoResizeCanvas(false);
	wnd.GetCanvas()->Resize(600, 600);

	// UI
	hiex::SysButton btnScroll(wnd, 10, 10, 120, 40, _T("Toggle Scroll"));
	hiex::SysButton btnLim(wnd, 10, 60, 120, 40, _T("Toggle Limitation"));

	// drawing
	//wnd.GetCanvas()->line(500, 0, 500, 100, RED);
	wnd.GetCanvas()->circle(300, 300, 200, RED);
	wnd.GetCanvas()->rectangle(0, 0, 600 - 1, 600 - 1, BLUE);
	wnd.GetCanvas()->setfont(64);
	wnd.GetCanvas()->centertext(_T("HiEasyX"), {}, YELLOW);
	wnd.FlushBuffer();

	// maintaining
	bool scroll_enabled = false;
	bool lim_enabled = false;
	while (wnd.Exist())
	{
		if (btnScroll.IsClicked())
		{
			scroll_enabled = !scroll_enabled;
			wnd.SetStyleHX(hiex::VScroll, scroll_enabled);
			wnd.SetStyleHX(hiex::HScroll, scroll_enabled);
		}
		if (btnLim.IsClicked())
		{
			lim_enabled = !lim_enabled;
			wnd.SetStyleHX(hiex::Maximizable, !lim_enabled);
			if (lim_enabled)
			{
				wnd.SetSizeLim({}, SIZE{ 600,600 });
			}
			else
			{
				wnd.SetSizeLim({}, {});
			}
		}

		hiex::SleepHX(100);
	}

	return 0;
}

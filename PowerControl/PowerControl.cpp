#include "PowerControl.h"

bool CheckMultiRun()
{
	DStringW sWindowName(100);
	LoadStringW(GetModuleHandle(NULL), IDS_APP_TITLE, sWindowName, 100);
	HWND hPreviousWindow = IsWindowsRunning(sWindowName + L" Class", sWindowName);
	if (!hPreviousWindow) return false;
	ShowWindow(hPreviousWindow, SW_SHOW);
	SetForegroundWindow(hPreviousWindow);
	return true;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (CheckMultiRun()) return 0;

	DACStrings sCommandLines = DGetCommandLines();
	if (sCommandLines.size() >= 2)
	{
		if (sCommandLines[1].CompareNoCase(L"/onboot"))
			nCmdShow = SW_HIDE;
	}

	DWPCBK MainWindow;
	DRFailRet(MainWindow.Create(nCmdShow));
	return EnterMsgLoop(LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_POWERCONTROL)));
}

DResult DWPC::Create(PDWnd Parent)
{
	DCreateDWnd CreateInfo;
	CreateInfo.WindowName = L"PowerControlUI";
	if (nVStyle == VStyle1)
		CreateInfo.dwExStyle = WS_EX_TRANSPARENT;
	CreateInfo.dwStyle = WS_POPUP;
	CreateInfo.nCmdShow = SW_HIDE;
	CreateInfo.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	CreateInfo.Parent = Parent;
	DRFailRet(CreateDWnd(CreateInfo));
	if (bHorizontal)
	{
		MoveWindowToCenter(HWnd(), nHButtonW * 4, nHButtonH, true);
		ShutdownButton.Create(this, DRect(0, 0, nHButtonW, nHButtonH));
		RestartButton.Create(this, DRect(nHButtonW, 0, nHButtonW, nHButtonH));
		SleepButton.Create(this, DRect(nHButtonW * 2, 0, nHButtonW, nHButtonH));
		CancelButton.Create(this, DRect(nHButtonW * 3, 0, nHButtonW, nHButtonH));
	}
	else
	{
		switch (nVStyle)
		{
		case VStyle1:
		{
						const int nVButtonOffsetY = nVButtonH + nVButtonPadding;
						MoveWindowToCenter(HWnd(), nVButtonW, nVButtonOffsetY * 3 + nVButtonH, true);
						ShutdownButton.Create(this, DRect(0, 0, nVButtonW, nVButtonH));
						RestartButton.Create(this, DRect(0, nVButtonOffsetY, nVButtonW, nVButtonH));
						SleepButton.Create(this, DRect(0, nVButtonOffsetY * 2, nVButtonW, nVButtonH));
						CancelButton.Create(this, DRect(0, nVButtonOffsetY * 3, nVButtonW, nVButtonH));
						break;
		}
		case VStyle2:
		{
						MoveWindowToCenter(HWnd(), nVButtonW, nVButtonH * 4, true);
						ShutdownButton.Create(this, DRect(0, 0, nVButtonW, nVButtonH));
						RestartButton.Create(this, DRect(0, nVButtonH, nVButtonW, nVButtonH));
						SleepButton.Create(this, DRect(0, nVButtonH * 2, nVButtonW, nVButtonH));
						CancelButton.Create(this, DRect(0, nVButtonH * 3, nVButtonW, nVButtonH));
						break;
		}
		}
	}
	SetTimer(HWnd(), 0, 100, NULL);
	return DROk;
}
LRESULT DWPC::OnDestroy()
{
	PostQuitMessage(0);
	return 0;
}
LRESULT DWPC::Proc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:
	{
						  return TRUE;
	}
	case WM_TIMER:
	{
					 if (wParam == 0)
					 {
						 if (IsKeyDown(VK_SHIFT) != bPressingShift)
						 {
							 bPressingShift ^= true;
							 InvalidateRect(ShutdownButton, NULL, TRUE);
							 InvalidateRect(RestartButton, NULL, TRUE);
							 InvalidateRect(SleepButton, NULL, TRUE);
							 InvalidateRect(CancelButton, NULL, TRUE);
						 }
					 }
					 break;
	}
	case WM_KEYDOWN:
	case DWM_PROCESSSHOTCUT:
	{
							   switch (wParam)
							   {
							   case 0x41://A-Advance Restart
								   //if (IsKeyShiftDown())
								   bPressingShift = true;
								   SendMessage(RestartButton, WM_LBUTTONUP, 0, 0);
								   break;
							   case 0x43://C-Cancel
							   case VK_ESCAPE://ESC
								   SendMessage(CancelButton, WM_LBUTTONUP, 0, 0);
								   break;
							   case 0x48://H-Hibernate
								   //if (IsKeyShiftDown())
								   bPressingShift = true;
								   SendMessage(SleepButton, WM_LBUTTONUP, 0, 0);
								   break;
							   case 0x52://R-Restart
								   //if (!IsKeyShiftDown())
								   bPressingShift = false;
								   SendMessage(RestartButton, WM_LBUTTONUP, 0, 0);
								   break;
							   case 0x53://S-Shutdown
								   //if (!IsKeyShiftDown())
								   bPressingShift = false;
								   SendMessage(ShutdownButton, WM_LBUTTONUP, 0, 0);
								   break;
							   case 0x54://T-Totally Shutdown
								   //if (IsKeyShiftDown())
								   bPressingShift = true;
								   SendMessage(ShutdownButton, WM_LBUTTONUP, 0, 0);
								   break;
							   case 0x5A://Z-Standby
								   //if (!IsKeyShiftDown())
								   bPressingShift = false;
								   SendMessage(SleepButton, WM_LBUTTONUP, 0, 0);
								   break;
							   }
							   break;
	}
		/*
		case WM_KEYDOWN:
		if (bPressingShift) break;
		case WM_KEYUP:
		{
		bPressingShift ^= true;
		if (wParam == VK_SHIFT)
		{
		InvalidateRect(ShutdownButton, NULL, TRUE);
		InvalidateRect(RestartButton, NULL, TRUE);
		InvalidateRect(SleepButton, NULL, TRUE);
		InvalidateRect(CancelButton, NULL, TRUE);
		}
		break;
		}
		*/
	}
	return DWnd::Proc(message, wParam, lParam);
}

DResult DWPCBK::Create(int nCmdShow)
{
	DCreateDWnd CreateInfo;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	CreateInfo.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
	CreateInfo.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICONSMALL));
	CreateInfo.WindowName.allocate(100);
	LoadString(hInstance, IDS_APP_TITLE, CreateInfo.WindowName, 100);
#ifdef _DEBUG
	CreateInfo.dwExStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW;//
#else
	CreateInfo.dwExStyle = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW;//
#endif
	CreateInfo.dwStyle = WS_POPUP;
	CreateInfo.nCmdShow = nCmdShow;
	CreateInfo.Pos = GetDesktopRect();
	CreateInfo.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	DRFailRet(CreateDWnd(CreateInfo));
	PowerControl.Create(this);
	ShowWindow(PowerControl, nCmdShow);
	SetLayeredWindowAttributes(HWnd(), 0, 150, LWA_ALPHA);
	RegisterHotKey(HWnd(), 0, MOD_CONTROL | MOD_ALT | MOD_SHIFT, 'P');
	return DROk;
}
LRESULT DWPCBK::OnDestroy()
{
	PostQuitMessage(0);
	return 0;
}
LRESULT DWPCBK::Proc(UINT message, WPARAM wParam, LPARAM lParam)
{
	ProcessShotcut(PowerControl, message, wParam, lParam);
	switch (message)
	{
	case WM_KEYDOWN:
		break;
	case WM_HOTKEY:
	{
					  if (IsWindowVisible(HWnd()))
					  {
						  ShowWindow(HWnd(), SW_HIDE);
					  }
					  else
					  {
						  ShowWindow(HWnd(), SW_SHOW);
						  ::SetForegroundWindow(HWnd());
					  }
					  break;
	}
	case WM_SHOWWINDOW:
		if (wParam) {
			ShowWindow(PowerControl, SW_SHOW);
		}
		else {
			ShowWindow(PowerControl, SW_HIDE);
		}
		break;
	}
	return DWnd::Proc(message, wParam, lParam);
}
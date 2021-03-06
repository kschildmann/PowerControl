#include "PowerControl.h"

const DFont BTextFont(L"Microsoft Sans Serif", -1, (nHButtonH - nHBTextY) / 2);
bool bHorizontal = false;
VerticalStyle nVStyle = VStyle1;//Vertical

inline bool InitializeImage(DImage& Image, WORD ImageID)
{
	/*
	DImage OriginalImage;
	OriginalImage.Load(GetModuleHandle(NULL), MAKEINTRESOURCE(ImageID), L"PNG");
	OriginalImage.bTransparent = true;
	Image.Create(OriginalImage.GetWidth(), OriginalImage.GetHeight());
	FillRect(Image, DRect(0, 0, Image.GetWidth(), Image.GetHeight()), GetStockBrush(WHITE_BRUSH));
	return OriginalImage.Draw(Image);
	*/
	Image.Load(GetModuleHandle(NULL), MAKEINTRESOURCE(ImageID), L"PNG");
	Image.bTransparent = true;
	return true;
}
inline bool InitializeForPaint(PDWnd pDWnd, HDC hdc, WRect& Rect, UINT& Format, DImage& Image)
{
	HBRUSH hBrush = CreateSolidBrush(pDWnd->GetBackgroundColor());
	if (bHorizontal)
	{
		Rect = WRect(0, nHBTextY, nHButtonW, nHButtonH);
		Format = DT_CENTER | DT_VCENTER;// | DT_BOTTOM;
		DDDImage CBImage(Image.GetWidth(), Image.GetHeight());
		FillRect(CBImage, WRect(0, 0, Image.GetWidth(), Image.GetHeight()), hBrush);
		Image.Draw(CBImage);
		SetStretchBltMode(hdc, HALFTONE);
		SetBrushOrgEx(hdc, 0, 0, nullptr);
		CBImage.StretchBlt(hdc, DRect(0, 0, nHButtonW, nHBTextY), DRect(0, 0, -1, -1), SRCCOPY);
		//Image.Draw(hdc, DRect(0, 0, nHButtonW, nHBTextY));
	}
	else
	{
		Rect = WRect(nVBTextX, 0, nVButtonW, nVButtonH);
		Format = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
		DDDImage CBImage(Image.GetWidth(), Image.GetHeight());
		FillRect(CBImage, WRect(0, 0, Image.GetWidth(), Image.GetHeight()), hBrush);
		Image.Draw(CBImage);
		SetStretchBltMode(hdc, HALFTONE);
		SetBrushOrgEx(hdc, 0, 0, nullptr);
		CBImage.StretchBlt(hdc, DRect(nVBtnIconPadding, nVBtnIconPadding, nVBtnIconSize, nVBtnIconSize), DRect(0, 0, -1, -1), SRCCOPY);
		if (nVStyle == VStyle2)
		{
			const int nLineWidth = 1;
			const int nOffsetY = nVButtonH - 1;
			MoveToEx(hdc, 0, nOffsetY, NULL);
			LineTo(hdc, nVButtonW, nOffsetY);
		}
		//Image.Draw(hdc, DRect(0, 0, nVButtonH, nVButtonH));
	}
	DeleteObject(hBrush);
	return true;
}
inline void ExitAfterClick()
{
	PostQuitMessage(0);
}
inline HINSTANCE Shutdown(DStringA Command)
{
	return ShellExecuteA(NULL, "open", "shutdown.exe", Command, NULL, SW_HIDE);
	//return system("start \"\" //b \"shutdown.exe\" " + Command + "&pause");
}
bool bPressingShift = false;
inline bool IsKeyShiftDown()
{
	return bPressingShift;
}
const int DWM_PROCESSSHOTCUT = WM_USER + 1;
inline LRESULT ProcessShotcut(HWND pProcessor, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	{
					   switch (wParam)
					   {
					   case 0x41://A
					   case 0x43://C
					   case 0x48://H
					   case 0x52://R
					   case 0x53://S
					   case 0x54://T
					   case 0x5A://Z
					   case VK_ESCAPE://ESC
						   return SendMessage(pProcessor, DWM_PROCESSSHOTCUT, wParam, lParam);
					   }
					   break;
	}
	}
	return 0;
}

static DWPCBK& GetMainWindow();

class DWBShutdown : public DWButton
{
protected:
	virtual LRESULT OnCreate()
	{
		SetBkColor(RGB(50, 150, 255), RGB(70, 190, 250), RGB(255, 255, 255));
		InitializeImage(Image, IDB_SHUTDOWN);
		return 0;
	}
	virtual LRESULT OnClick(WPARAM wParam, LPARAM lParam)
	{
		if (IsKeyShiftDown()) Shutdown("/s /t 0");//system("shutdown /s /t 0");
		else Shutdown("/s /hybrid /t 0");//system("shutdown /s /hybrid /t 0");
		ExitAfterClick();
		return 0;
	}
	virtual LRESULT OnPaint(HDC hdc)
	{
		WRect Rect;
		UINT Format = 0;
		InitializeForPaint(this, hdc, Rect, Format, Image);
		::SetBkColor(hdc, GetBackgroundColor());
		SelectObject(hdc, BTextFont);
		if (IsKeyShiftDown())
		{
			if (bHorizontal) DrawTextW(hdc, GetStringAndSizeFromStr(L"Totally\nShutdown"), &Rect, Format);
			else DrawTextW(hdc, GetStringAndSizeFromStr(L"Totally Shutdown (T)"), &Rect, Format);
		}
		else DrawTextW(hdc, GetStringAndSizeFromStr(L"Shutdown (S)"), &Rect, DT_SINGLELINE | Format);
		return 0;
	}
	virtual LRESULT Proc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		ProcessShotcut(hParent(), message, wParam, lParam);
		/*
		switch (message)
		{
		case WM_KEYDOWN:
		{
		if (wParam == VK_SHIFT) SendMessage(hParent(), message, wParam, lParam);
		break;
		}
		case WM_KEYUP:
		{
		if (wParam == VK_SHIFT) SendMessage(hParent(), message, wParam, lParam);
		break;
		}
		}*/
		return DWButton::Proc(message, wParam, lParam);
	}
private:
	DImage Image;
};
class DWBRestart : public DWButton
{
protected:
	virtual LRESULT OnCreate()
	{
		SetBkColor(RGB(50, 150, 255), RGB(70, 190, 250), RGB(255, 255, 255));
		InitializeImage(Image, IDB_RESTART);
		return 0;
	}
	virtual LRESULT OnClick(WPARAM wParam, LPARAM lParam)
	{
		if (IsKeyShiftDown()) Shutdown("/r /o /t 0");//system("shutdown /r /o /t 0");
		else Shutdown("/r /t 0");//system("shutdown /r /t 0");
		ExitAfterClick();
		return 0;
	}
	virtual LRESULT OnPaint(HDC hdc)
	{
		WRect Rect;
		UINT Format = 0;
		InitializeForPaint(this, hdc, Rect, Format, Image);
		::SetBkColor(hdc, GetBackgroundColor());
		SelectObject(hdc, BTextFont);
		if (IsKeyShiftDown())
		{
			if (bHorizontal) DrawTextW(hdc, GetStringAndSizeFromStr(L"Advance\nRestart"), &Rect, Format);
			else DrawTextW(hdc, GetStringAndSizeFromStr(L"Advance Restart (A)"), &Rect, Format);
		}
		else DrawTextW(hdc, GetStringAndSizeFromStr(L"Restart (R)"), &Rect, DT_SINGLELINE | Format);
		return 0;
	}
	virtual LRESULT Proc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		ProcessShotcut(hParent(), message, wParam, lParam);
		/*
		switch (message)
		{
		case WM_KEYDOWN:
		{
		if (wParam == VK_SHIFT) SendMessage(hParent(), message, wParam, lParam);
		break;
		}
		case WM_KEYUP:
		{
		if (wParam == VK_SHIFT) SendMessage(hParent(), message, wParam, lParam);
		break;
		}
		}*/
		return DWButton::Proc(message, wParam, lParam);
	}
private:
	DImage Image;
};
class DWBSleep : public DWButton
{
protected:
	virtual LRESULT OnCreate()
	{
		SetBkColor(RGB(50, 150, 255), RGB(70, 190, 250), RGB(255, 255, 255));
		InitializeImage(Image, IDB_SLEEP);
		return 0;
	}
	virtual LRESULT OnClick(WPARAM wParam, LPARAM lParam)
	{
		if (IsKeyShiftDown())
			Shutdown("/h");//system("shutdown /h");
		else SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
		ExitAfterClick();
		return 0;
	}
	virtual LRESULT OnPaint(HDC hdc)
	{
		WRect Rect;
		UINT Format = 0;
		InitializeForPaint(this, hdc, Rect, Format, Image);
		::SetBkColor(hdc, GetBackgroundColor());
		SelectObject(hdc, BTextFont);
		if (IsKeyShiftDown())
			DrawTextW(hdc, GetStringAndSizeFromStr(L"Hibernate (H)"), &Rect, DT_SINGLELINE | Format);
		else DrawTextW(hdc, GetStringAndSizeFromStr(L"Standby (Z)"), &Rect, DT_SINGLELINE | Format);
		return 0;
	}
	virtual LRESULT Proc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		ProcessShotcut(hParent(), message, wParam, lParam);
		/*
		switch (message)
		{
		case WM_KEYDOWN:
		{
		if (wParam == VK_SHIFT) SendMessage(hParent(), message, wParam, lParam);
		break;
		}
		case WM_KEYUP:
		{
		if (wParam == VK_SHIFT) SendMessage(hParent(), message, wParam, lParam);
		break;
		}
		}*/
		return DWButton::Proc(message, wParam, lParam);
	}
private:
	DImage Image;
};
class DWBCancel : public DWButton
{
protected:
	virtual LRESULT OnCreate()
	{
		SetBkColor(RGB(50, 150, 255), RGB(70, 190, 250), RGB(255, 255, 255));
		InitializeImage(Image, IDB_CANCEL);
		return 0;
	}
	virtual LRESULT OnClick(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnPaint(HDC hdc)
	{
		WRect Rect;
		UINT Format = 0;
		InitializeForPaint(this, hdc, Rect, Format, Image);
		::SetBkColor(hdc, GetBackgroundColor());
		SelectObject(hdc, BTextFont);
		DrawTextW(hdc, GetStringAndSizeFromStr(L"Cancel (C)"), &Rect, DT_SINGLELINE | Format);
		return 0;
	}
	virtual LRESULT Proc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		ProcessShotcut(hParent(), message, wParam, lParam);
		/*
		switch (message)
		{
		case WM_KEYDOWN:
		{
		if (wParam == VK_SHIFT) SendMessage(hParent(), message, wParam, lParam);
		break;
		}
		case WM_KEYUP:
		{
		if (wParam == VK_SHIFT) SendMessage(hParent(), message, wParam, lParam);
		break;
		}
		}*/
		return DWButton::Proc(message, wParam, lParam);
	}
private:
	DImage Image;
};

class DWPC : public DWnd
{
public:
	DResult Create(PDWnd Parent);
protected:
	virtual LRESULT OnDestroy();
	virtual LRESULT Proc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	DWBShutdown ShutdownButton;
	DWBRestart RestartButton;
	DWBSleep SleepButton;
	DWBCancel CancelButton;
};
class DWPCBK : public DWnd
{
public:
	DResult Create(int nCmdShow);
protected:
	virtual LRESULT OnDestroy();
	virtual LRESULT Proc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	DWPC PowerControl;
};

LRESULT DWBCancel::OnClick(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(GetMainWindow(), SW_HIDE);
	return 0;
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
								   //SendMessage(CancelButton, WM_LBUTTONUP, 0, 0);
								   ExitAfterClick();
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


static DWPCBK MainWindow;
static DWPCBK& GetMainWindow()
{
	return MainWindow;
}
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (CheckMultiRun(DLoadResString(GetModuleHandle(NULL), IDS_APP_TITLE, 100)))
		return 0;

	DACStrings sCommandLines = DGetCommandLines();
	if (sCommandLines.size() >= 2)
	{
		if (sCommandLines[1].CompareNoCase(L"/onboot"))
			nCmdShow = SW_HIDE;
	}

	DRFailRet(MainWindow.Create(nCmdShow));
	return EnterMsgLoop(LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_POWERCONTROL)));
}
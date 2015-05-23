// Freeze.cpp : "замораживание" главного окна программы

#include "stdafx.h"
#include "core5.h"
#include "Freeze.h"
#include "action_main.h"

_ainfo_base::arg CFreezeInfo::s_pargs[] =
{
	// "заморозка" - TRUE, "разморозка" - FALSE
	{"WantToFreeze",	szArgumentTypeInt, "1", true, false },
	{"Name",	szArgumentTypeString, "", true, false },
	{0, 0, 0, false, false },
};

static HWND hWndFreeze = 0;

static HDC hDCMem;

static HBITMAP	hDIBSection;

static _bstr_t bstrLastFreezeName="";

FARPROC lpfnOldWndFreezeProc;

LRESULT FAR PASCAL WndFreezeFunc(HWND hWnd,
							 UINT Message,
							 WPARAM wParam,
							 LONG lParam)
{
	// Блокируем пропуск кликов в главное окно
	if (Message == WM_NCHITTEST)
	{
		return	HTCLIENT;
	}

	if (Message == WM_PAINT)
	{
		PAINTSTRUCT ps;
		HDC hDC;
		RECT rc;
        hDC = BeginPaint(hWnd, &ps);
		::GetWindowRect(hWnd, &rc);
        HBITMAP hBitmapOld = (HBITMAP)::SelectObject(hDCMem, hDIBSection);
		::BitBlt(hDC, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hDCMem, 0, 0, SRCCOPY);
        SelectObject(hDCMem, hBitmapOld);
        EndPaint(hWnd, &ps);
        return 0; 
	}
	
	return CallWindowProc((WNDPROC)lpfnOldWndFreezeProc, hWnd, Message, wParam, lParam);

}

/////////////////////////////////////////////////////////////////////////////
CFreeze::CFreeze()
{
}

//---------------------------------------------------------------------------
CFreeze::~CFreeze()
{
}

//---------------------------------------------------------------------------
HRESULT CFreeze::DoInvoke()
{

	HWND hWndMain;
	HDC hDCMain, hDCFreeze;
	HCURSOR hCursorFreeze;
	RECT rc;

	int WantToFreeze = GetArgLong("WantToFreeze");
	_bstr_t bstrName = GetArgString("Name");
	
	try
	{
		if (WantToFreeze && (!hWndFreeze))
		{
			IApplicationPtr ptrApp(GetAppUnknown());
			ptrApp->GetMainWindowHandle(&hWndMain);
			if (ptrApp == NULL) return S_FALSE;

			::GetWindowRect(hWndMain, &rc);

			hDCMain = ::GetWindowDC(hWndMain);

			BITMAPINFOHEADER	bmih;
			ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
			bmih.biBitCount = 24;
			bmih.biHeight	= rc.bottom - rc.top;
			bmih.biWidth	= rc.right - rc.left;
			bmih.biSize		= sizeof(BITMAPINFOHEADER);
			bmih.biPlanes	= 1;

			byte	*pdibBits = 0;
			hDIBSection = ::CreateDIBSection(hDCMain, (BITMAPINFO*)&bmih, DIB_RGB_COLORS,
				(void**)&pdibBits, NULL, NULL);

			hDCMem = ::CreateCompatibleDC(hDCMain);
			HBITMAP hBitmapOld = (HBITMAP)::SelectObject(hDCMem, hDIBSection);

			// Сохраняем содержимое главного окна в битмап
			BitBlt(hDCMem, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hDCMain, 0, 0, SRCCOPY);
			ReleaseDC(hWndMain, hDCMain);

			// Создаем фиктивное окно
			hWndFreeze = CreateWindow("STATIC", "", WS_POPUP,
				rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
				hWndMain, NULL, app::handle(), NULL);
			if (!hWndFreeze) return S_FALSE;

			// Замещаем оконную процедуру
			lpfnOldWndFreezeProc = (FARPROC)SetWindowLongPtr(hWndFreeze, GWLP_WNDPROC, (LONG_PTR)WndFreezeFunc);
			
			ShowWindow(hWndFreeze, SW_SHOWDEFAULT);

			// Устанавливаем курсор в виде песочных часов
			hCursorFreeze = LoadCursor(NULL, IDC_WAIT);
			SetClassLongPtr(hWndFreeze, GCLP_HCURSOR, (LONG_PTR)hCursorFreeze);

			hDCFreeze = GetWindowDC(hWndFreeze);
			// Отображаем в фиктивном окне сохраненный битмап  
			BitBlt(hDCFreeze, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hDCMem, 0, 0, SRCCOPY);
			
			::SelectObject(hDCMem, hBitmapOld);
			ReleaseDC(hWndFreeze, hDCFreeze);

			bstrLastFreezeName = bstrName;
		}
		if (hWndFreeze && (!WantToFreeze) &&
			(bstrName==_bstr_t("") || bstrName==bstrLastFreezeName) )
		{
			IApplicationPtr ptrApp(GetAppUnknown());
			ptrApp->GetMainWindowHandle(&hWndMain);
			::SetWindowPos(hWndMain, HWND_TOP,0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
			HCURSOR hCursorArrow = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtr(hWndFreeze, GCLP_HCURSOR, (LONG_PTR)hCursorArrow);
			ShowWindow(hWndFreeze, SW_HIDE);

			::DeleteObject(hDIBSection);
			::DeleteDC(hDCMem);
			if (!hWndFreeze) return S_FALSE;
			
			// Восстанавливаем оконную процедуру по умолчанию
			SetWindowLongPtr(hWndFreeze, GWLP_WNDPROC, (LONG_PTR)lpfnOldWndFreezeProc);
			::SetWindowPos(hWndMain, HWND_TOP,0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);


			if (!DestroyWindow(hWndFreeze)) return S_FALSE;

			SetCursor(hCursorArrow);


			hWndFreeze = 0;
		}
	}
	catch(...)
	{
	}

	return S_OK;

}
// PrinterSetup.cpp: implementation of the CPrinterSetup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PrinterSetup.h"
#include "Print.h"

#include <winspool.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define szPrinterDataFile "printer.dat"

// vanek - 09.10.2003
//////////////////////////////////////////////////////////////////////
UINT_PTR CALLBACK PrintSetupHookProc(
  HWND hdlg,      // handle to dialog box
  UINT uiMsg,     // message identifier
  WPARAM wParam,  // message parameter
  LPARAM lParam   // message parameter
)
{
	if ( uiMsg ==  WM_INITDIALOG )
	{
		RECT rtPrinter = {0}, rtOrientation = {0}, rtOK = {0}, rtCancel = {0}, rtNet = {0}, rtDlg = {0};
		HWND	hwndPrinterFrame = ::GetDlgItem( hdlg, IDC_PRINTER_FRAME ),
				hwndOrFrame = ::GetDlgItem( hdlg, IDC_ORIENTATION_FRAME ),
				hwndOK = ::GetDlgItem( hdlg, IDOK ),
				hwndNetBtn = ::GetDlgItem( hdlg, IDC_NETWORK_BUTTON ),
				hwndCancel = ::GetDlgItem( hdlg, IDCANCEL );

		::GetWindowRect( hwndPrinterFrame, &rtPrinter );
		::GetWindowRect( hwndOrFrame, &rtOrientation );
		::GetWindowRect( hwndOK, &rtOK );
		::GetWindowRect( hwndNetBtn, &rtNet );
        ::GetWindowRect( hwndCancel, &rtCancel );
		::GetWindowRect( hdlg, &rtDlg );

		long dy = 0;
		if( hwndOrFrame ) 
			dy = rtOrientation.bottom - rtPrinter.bottom;

		::ScreenToClient( hdlg, (LPPOINT)&rtOK );
		::ScreenToClient( hdlg, (LPPOINT)&rtOK + 1 );

		::ScreenToClient( hdlg, (LPPOINT)&rtCancel );
		::ScreenToClient( hdlg, (LPPOINT)&rtCancel + 1 );

		::ScreenToClient( hdlg, (LPPOINT)&rtNet );
		::ScreenToClient( hdlg, (LPPOINT)&rtNet + 1 );


		if( hwndOK ) ::MoveWindow( hwndOK, rtOK.left, rtOK.top - dy, rtOK.right - rtOK.left, rtOK.bottom - rtOK.top, TRUE );
		if( hwndCancel ) ::MoveWindow( hwndCancel, rtCancel.left, rtOK.top - dy, rtCancel.right - rtCancel.left, rtCancel.bottom - rtCancel.top, TRUE );
		if( hwndNetBtn ) ::MoveWindow( hwndNetBtn, rtNet.left, rtOK.top - dy, rtNet.right - rtNet.left, rtNet.bottom - rtNet.top, TRUE );		
		if( hdlg ) ::MoveWindow( hdlg, rtDlg.left, rtDlg.top, rtDlg.right - rtDlg.left, rtDlg.bottom - rtDlg.top - dy, TRUE); 

		int arr_wnds[9] = {	IDC_PAPER_FRAME , IDC_SIZE_STATIC, IDC_SIZE_COMBO, IDC_SOURCE_STATIC, IDC_SOURCE_COMBO,
							IDC_ORIENTATION_FRAME, IDC_PAGE_STATIC, IDC_PORTRAIT_RADIOBTN, IDC_LANDSCAPE_RADIOBTN };

		HWND hwnd = 0;
		for( int i = 0; i < 9; i++ )
		{
			hwnd = ::GetDlgItem( hdlg, arr_wnds[i] );
			if( hwnd )
				::SetWindowPos( hwnd, 0, 0, rtDlg.bottom - rtDlg.top + dy, 0, 0, SWP_NOSIZE );
		}
	}

    return 0;
}

/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrinterSetup::CPrinterSetup()
{
	m_hDevMode = m_hDevNames = NULL;	

	m_bNeedRecalculateMargins = true;
	m_rcMargins = NORECT;
}

CPrinterSetup::~CPrinterSetup()
{
	if (m_hDevMode)  GlobalFree(m_hDevMode);
	if (m_hDevNames) GlobalFree(m_hDevNames);

}

HDC CPrinterSetup::GetHDC( IUnknown* punk_report )
{
	CWaitCursor wait;

	if( !(m_hDevMode && m_hDevNames) )
		InitDefaults();

	if( !(m_hDevMode && m_hDevNames) )
		return NULL;

	if( !IsPrinterAvailable() )
	{
		InitDefaults();
		if( !(m_hDevMode && m_hDevNames) )
			return NULL;
		if( !IsPrinterAvailable() )
			return NULL;
	}


	DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(m_hDevNames);
	DEVMODE*  pDevMode  = (DEVMODE* )GlobalLock(m_hDevMode);

	if( !pDevNames || !pDevMode )
		return NULL;


	short dm_old_orientation = pDevMode->dmOrientation;


	if( punk_report && ::GetValueInt( ::GetAppUnknown(), "\\Print", "PaperOrientationFromReport", 1L ) == 1L )
	{
		IReportFormPtr ptr_report = punk_report;
		if( ptr_report )
		{
			int nOrient = 0;
			ptr_report->GetPaperOrientation( &nOrient );
			if( nOrient == 0 )//Portrait
				pDevMode->dmOrientation = DMORIENT_PORTRAIT;
			else//Landscape
				pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
		}
	}

	LPSTR szDevice=NULL, szDriver=NULL, szOutput=NULL;

	szDriver = (LPSTR) pDevNames + pDevNames->wDriverOffset;
    szDevice = (LPSTR) pDevNames + pDevNames->wDeviceOffset;
    szOutput = (LPSTR) pDevNames + pDevNames->wOutputOffset;

	HDC hDC = CreateDC( szDriver, szDevice, szOutput, pDevMode );

	if( pDevMode )
		pDevMode->dmOrientation = dm_old_orientation;	

	if( m_hDevNames )
		GlobalUnlock(m_hDevNames);	

	if( m_hDevMode )
		GlobalUnlock(m_hDevMode);

	return hDC;
}

/////////////////////////////////////////////////////////////////////////////
CRect CPrinterSetup::GetPrinterMargins( IUnknown* punk_report )
{
	if( !m_bNeedRecalculateMargins )
		return m_rcMargins;

	CRect rectMargins(0,0,0,0);

	HDC hDC = GetHDC( punk_report );

	if( hDC == NULL )
		return rectMargins;


	CDC prnDC;
	prnDC.Attach( hDC );

	CDC* pDC = &prnDC;

//	POINT pt;

	// Start by getting the dimensions of the unprintable part of the
	// page (in device units). GETPRINTINGOFFSET will tell us the left
	// and upper unprintable area.

	rectMargins.left = pDC->GetDeviceCaps( PHYSICALOFFSETX );
	rectMargins.top  = pDC->GetDeviceCaps( PHYSICALOFFSETY );

	rectMargins.right  = pDC->GetDeviceCaps( PHYSICALWIDTH ) - 
		pDC->GetDeviceCaps(HORZRES) - rectMargins.left;
	rectMargins.bottom = pDC->GetDeviceCaps( PHYSICALHEIGHT ) - 
		pDC->GetDeviceCaps(VERTRES) - rectMargins.top;

	
	
	
	

	/*
	pDC->Escape(GETPRINTINGOFFSET, 0, NULL, &pt);
	rectMargins.left = pt.x;
	rectMargins.top  = pt.y;

	// To get the right and lower unprintable area, we need to take
	// the entire width and height of the paper (GETPHYSPAGESIZE) and
	// subtract everything else.

	pDC->Escape(GETPHYSPAGESIZE, 0, NULL, &pt);

	rectMargins.right  = pt.x                     // total paper width
						  - pDC->GetDeviceCaps(HORZRES) // printable width //pixel
						  - rectMargins.left;   // left unprtable margin

	rectMargins.bottom = pt.y                     // total paper height
						  - pDC->GetDeviceCaps(VERTRES) // printable ht
						  - rectMargins.top;    // rt unprtable margin

  */
	// At this point, rectMargins contains the widths of the
	// unprintable regions on all four sides of the page in device units.

	
	double fZoomX, fZoomY;

	double fPaperWidthPx   = (double)pDC->GetDeviceCaps(HORZRES);
	double fPaperHeightPx  = (double)pDC->GetDeviceCaps(VERTRES);

	double fPaperWidthMM   = (double)pDC->GetDeviceCaps(HORZSIZE);
	double fPaperHeightMM  = (double)pDC->GetDeviceCaps(VERTSIZE);

	fZoomX = fPaperWidthPx / fPaperWidthMM;
	fZoomY = fPaperHeightPx / fPaperHeightMM;
	

	rectMargins.left	= ((LONG) ((double)rectMargins.left) / fZoomX );
	rectMargins.right	= ((LONG) ((double)rectMargins.right) / fZoomX );
	rectMargins.top		= ((LONG) ((double)rectMargins.top) / fZoomY );
	rectMargins.bottom	= ((LONG) ((double)rectMargins.bottom) / fZoomY );
	

	::DeleteDC( hDC );

	m_bNeedRecalculateMargins = false;
	
	m_rcMargins = rectMargins;

	return rectMargins;
}

CWnd* _GetActiveView();

/////////////////////////////////////////////////////////////////////////////
BOOL CPrinterSetup::ChooseAnother( CWnd* pWnd )
{
	BOOL bFailed =  FALSE;
	BOOL bRes = _ChooseAnother( pWnd, bFailed );
	if( bFailed )
	{
		InitDefaults();
		bRes = _ChooseAnother( pWnd, bFailed );
	}
	return bRes;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CPrinterSetup::_ChooseAnother( CWnd* pWnd, BOOL& bFailed )
{
	bFailed = FALSE;

	if( pWnd == NULL )
		pWnd = _GetActiveView();

	CPrintDialog pd(
	  TRUE, PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION,
	  pWnd
	  );

	// Make sure we don't accidentally create a device context
	pd.m_pd.Flags &= ~PD_RETURNDC;   // Reset flag set by constructor.

	// vanek - 09.10.2003
	pd.m_pd.Flags |= PD_ENABLESETUPHOOK;
	pd.m_pd.lpfnSetupHook = PrintSetupHookProc;
	
	// Force the CPrintDialog to use our device mode & name.
	if(m_hDevMode)
		pd.m_pd.hDevMode  = CopyHandle(m_hDevMode);
	if(m_hDevNames)
		pd.m_pd.hDevNames = CopyHandle(m_hDevNames);

	// Display the dialog box and let the user make their selection.

	int nRes = pd.DoModal();
	if ( nRes == IDOK)	{
		// The user clicked OK
		// (and POSSIBLY changed printers).
		// In any case, the CPrintDialog logic made a copy of the original
		// DEVMODE/DEVNAMES that we passed it and applied the user's
		// changes to that copy and discarded the original copy we passed
		// it. (NOTE: If the user had clicked CANCEL instead, the original
		// values we passed would have been returned unchanged).
		if(m_hDevMode)
			GlobalFree(m_hDevMode);                      // Free old copies.
		if(m_hDevNames)
			GlobalFree(m_hDevNames);                     // Free old copies.
		if(pd.m_pd.hDevMode)
			m_hDevMode  = CopyHandle(pd.m_pd.hDevMode);  // Write new copies.
		if(pd.m_pd.hDevNames)
			m_hDevNames = CopyHandle(pd.m_pd.hDevNames); // Write new copies.	

	}
	else
	{
		if( 0 != CommDlgExtendedError() )
		{
			bFailed = TRUE;
		}
	}

	

		// Regardless of whether the user clicked OK or CANCEL,
	// we need to ALWAYS do a GlobalFree of CPrintDialog's
	// m_pd.hDevMode and m_pd.hDevNames upon return from
	// DoModal in order to prevent a resource leak.
	GlobalFree(pd.m_pd.hDevMode);   // Because DoModal was called,
	GlobalFree(pd.m_pd.hDevNames);  // we need to free these.

	m_bNeedRecalculateMargins = true;

	return nRes == IDOK;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPrinterSetup::InitDefaults()
{
	m_bNeedRecalculateMargins = true;

	PRINTDLG pd;
	::ZeroMemory( &pd, sizeof(PRINTDLG) );
	// Get MFC's printer
	if(AfxGetApp()->GetPrinterDeviceDefaults(&pd) ) {
		// Make a copy
		if (m_hDevMode)  GlobalFree(m_hDevMode);
		if (m_hDevNames) GlobalFree(m_hDevNames);

		m_hDevNames = CopyHandle(pd.hDevNames);
		m_hDevMode  = CopyHandle(pd.hDevMode);		
		
		return TRUE;		
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
HANDLE CPrinterSetup::CopyHandle(HANDLE h)
{
	// Return a handle to a copy of the data
	// that the passed handle was for.
	if (!h) return NULL;
	DWORD  dwLen = GlobalSize(h);
	HANDLE hCopy = GlobalAlloc(GHND, dwLen);
	if(hCopy) {
		BYTE* lpCopy = (BYTE*)GlobalLock(hCopy);
		BYTE* lp     = (BYTE*)GlobalLock(h);
		CopyMemory(lpCopy,lp,dwLen);
		GlobalUnlock(hCopy);
		GlobalUnlock(h);
	}
	return hCopy;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPrinterSetup::IsPrinterAvailable()
{
	if( m_hDevNames == NULL )
		return FALSE;
	
	DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(m_hDevNames);
	LPSTR szDevice=NULL;
	szDevice = (LPSTR) pDevNames + pDevNames->wDeviceOffset;

	HANDLE hPrinter = 0;
	if (OpenPrinter( szDevice, &hPrinter, NULL) == FALSE)
	{		
		GlobalUnlock(m_hDevNames);
		return FALSE;
	}	

	GlobalUnlock( m_hDevNames );
	ClosePrinter( hPrinter );	

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////

void CPrinterSetup::WriteString(CString str, CString strKey)
{
	WritePrivateProfileString(
	  "Printer",    // pointer to section name
	  strKey,  // pointer to key name
	  str,   // pointer to string to add
	  m_strFileName  // pointer to initialization filename
	);
	
}

CString CPrinterSetup::ReadString(CString strKey)
{
	CString strBuf;
	char szBuf[255];
	GetPrivateProfileString(
	  "Printer",        // points to section name
	  strKey,		  // points to key name
	  "",        // points to default string
	  szBuf,  // points to destination buffer
	  255,              // size of destination buffer
	  m_strFileName// points to initialization filename
	);

	strBuf = szBuf;

	return strBuf;  
}

void CPrinterSetup::WriteDWORD(DWORD dw, CString strKey)
{
	CString str;
	str.Format("%d", dw );
	WritePrivateProfileString(
	  "Printer",  // pointer to section name
	  strKey,  // pointer to key name
	  str,   // pointer to string to add
	  m_strFileName  // pointer to initialization filename
	);
}

DWORD CPrinterSetup::ReadDWORD( CString strKey)
{
	return (DWORD) GetPrivateProfileInt(
		  "Printer",  // address of section name
		  strKey,  // address of key name
		  0,       // return value if key name is not found
		  m_strFileName  // address of initialization filename
		); 
}



BOOL CPrinterSetup::Read( bool bForceWrite )
{		
	CString strDir = GetReportDatDir();
	m_strFileName = strDir +  szPrinterDataFile;

	{
		CFileFind ff;
		if( !ff.FindFile( m_strFileName ) )
		{
			//Write default printers setting 
			if( bForceWrite )
				Write();
		}
	}

	CFile file;
	if( file.Open( strDir + szPrinterDataFile, CFile::modeRead | CFile::typeBinary ) )
	{		
		char szPref[2];
		file.Read( szPref, sizeof(char) * 2 );
		if( !( szPref[0] == 'P' && szPref[1] == 'D' ) )
		{
			InitDefaults();
			if( bForceWrite )
				Write();

			return FALSE;
		}

		DWORD dwDevModeLen	= 0;
		DWORD dwDevNamesLen	= 0;

		file.Read( &dwDevModeLen, sizeof(DWORD) );
		file.Read( &dwDevNamesLen, sizeof(DWORD) );
		
		if( !( dwDevModeLen >= sizeof(DEVMODE) && dwDevNamesLen >= sizeof(DEVNAMES) ) )
		{
			InitDefaults();
			if( bForceWrite )
				Write();

			return FALSE;
		}
			
		m_hDevMode  = GlobalAlloc( GHND, dwDevModeLen );
		m_hDevNames = GlobalAlloc( GHND, dwDevNamesLen );

		DEVMODE*  pDevMode  = (DEVMODE* )GlobalLock(m_hDevMode);
		DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(m_hDevNames);

		file.Read( pDevMode, dwDevModeLen );
		file.Read( pDevNames, dwDevNamesLen );

		file.Close();
	}

	if( m_hDevMode )
		GlobalUnlock(m_hDevMode);

	if( m_hDevNames )
		GlobalUnlock(m_hDevNames);	


	if( bForceWrite && !IsPrinterAvailable() )
	{
		InitDefaults();
		if( bForceWrite )
			Write();
	}

	return TRUE;


	/*

	DWORD dwDevModeSize = ReadDWORD("DevModeSize");
	if( dwDevModeSize < sizeof( DEVMODE ) )//Default
		dwDevModeSize = sizeof( DEVMODE );


	DWORD dwDevNamesSize = ReadDWORD("DevNamesSize");
	if( dwDevNamesSize < sizeof( DEVNAMES ) )//Default
		dwDevNamesSize = sizeof( DEVNAMES );


	if(m_hDevMode)  GlobalFree(m_hDevMode);
	if(m_hDevNames) GlobalFree(m_hDevNames);

	m_hDevMode  = GlobalAlloc(GHND, dwDevModeSize );
	m_hDevNames = GlobalAlloc(GHND, dwDevNamesSize);


	DEVMODE*  pDevMode  = (DEVMODE* )GlobalLock(m_hDevMode);
	DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(m_hDevNames);

	::ZeroMemory( pDevMode, sizeof(DEVMODE) );
	::ZeroMemory( pDevNames, sizeof(DEVNAMES) );

	CString str;
    str = ReadString("DeviceName");	 
	strcpy( (CHAR*)pDevMode->dmDeviceName, str );	


    pDevMode->dmSpecVersion	 = (WORD)ReadDWORD("SpecVersion");
    pDevMode->dmDriverVersion = (WORD)ReadDWORD("DriverVersion");
    pDevMode->dmSize = (WORD)ReadDWORD("Size");
    pDevMode->dmDriverExtra = (WORD)ReadDWORD("DriverExtra");
    pDevMode->dmFields = ReadDWORD("Fields");
    pDevMode->dmOrientation = (short)ReadDWORD("Orientation");
    pDevMode->dmPaperSize = (short)ReadDWORD("PaperSize");
    pDevMode->dmPaperLength = (short)ReadDWORD("PaperLength");
    pDevMode->dmPaperWidth = (short)ReadDWORD("PaperWidth");
	pDevMode->dmPosition.x = (LONG)ReadDWORD("Positionx");
	pDevMode->dmPosition.y = (LONG)ReadDWORD("Positiony");
    pDevMode->dmScale = (short)ReadDWORD("Scale");
    pDevMode->dmCopies = (short)ReadDWORD("Copies");
    pDevMode->dmDefaultSource = (short)ReadDWORD("DefaultSource");
    pDevMode->dmPrintQuality = (short)ReadDWORD("PrintQuality");
    pDevMode->dmColor = (short)ReadDWORD("Color");
    pDevMode->dmDuplex = (short)ReadDWORD("Duplex");
    pDevMode->dmYResolution = (short)ReadDWORD("YResolution");
    pDevMode->dmTTOption = (short)ReadDWORD("TTOption");
    pDevMode->dmCollate = (short)ReadDWORD("Collate");    
		
    str = ReadString("FormName");
	strcpy( (CHAR*)pDevMode->dmFormName, str );	

    pDevMode->dmLogPixels = (WORD)ReadDWORD("LogPixels");
    pDevMode->dmBitsPerPel = ReadDWORD("BitsPerPel");
    pDevMode->dmPelsWidth = ReadDWORD("PelsWidth");
    pDevMode->dmPelsHeight = ReadDWORD("PelsHeight");
    pDevMode->dmDisplayFlags = ReadDWORD("DisplayFlags");
    pDevMode->dmDisplayFrequency = ReadDWORD("DisplayFrequency");

	pDevNames->wDriverOffset = (WORD)ReadDWORD("DriverOffset");
	pDevNames->wDeviceOffset = (WORD)ReadDWORD("DeviceOffset");
	pDevNames->wOutputOffset = (WORD)ReadDWORD("OutputOffset");
	pDevNames->wDefault = (WORD)ReadDWORD("Default");

	
	CHAR* ptr = (CHAR*)pDevNames;
	CHAR* pdwDriveOffset  = (CHAR*)pDevNames + pDevNames->wDriverOffset;
	CHAR* pdwDeviceOffset = (CHAR*)pDevNames + pDevNames->wDeviceOffset;
	CHAR* pdwOutputOffset = (CHAR*)pDevNames + pDevNames->wOutputOffset;

    str = ReadString("Driver");	
	strcpy( pdwDriveOffset, str );	

    str = ReadString("Device");	
	strcpy( pdwDeviceOffset, str );	

    str = ReadString("Output");	
	strcpy( pdwOutputOffset, str );	  

	DWORD dwDriverDataLen = GlobalSize( m_hDevMode ) - sizeof( DEVMODE );
	if( dwDriverDataLen > 0 )
	{
		CFile file;
		if( file.Open( strDir + szPrinterDataFile, CFile::modeRead | CFile::typeBinary ) )
		{
			if( file.GetLength() == dwDriverDataLen )
			{
				BYTE* pData = (BYTE*)pDevMode + (BYTE)dwDriverDataLen;
				file.Read( pData, dwDriverDataLen );
			}			
			
			file.Close();
		}

		
	}

	*/
}								 


/////////////////////////////////////////////////////////////////////////////
BOOL CPrinterSetup::Write()
{
	if( !m_hDevMode || !m_hDevNames )
		return false;
	CString strDir = GetReportDatDir();
	m_strFileName = strDir +  szPrinterDataFile;

	DEVMODE*  pDevMode  = (DEVMODE* )GlobalLock(m_hDevMode);
	DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(m_hDevNames);

	DWORD dwDevModeLen	= GlobalSize( m_hDevMode );
	DWORD dwDevNamesLen	= GlobalSize( m_hDevNames );

	CFile file;
	if( file.Open( strDir + szPrinterDataFile, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary ) )
	{		
		file.Write( "PD", sizeof(char) * 2 );
		file.Write( &dwDevModeLen, sizeof(DWORD) );
		file.Write( &dwDevNamesLen, sizeof(DWORD) );
		
		file.Write( pDevMode, dwDevModeLen );
		file.Write( pDevNames, dwDevNamesLen );

		file.Close();
	}


	GlobalUnlock(m_hDevMode);
	GlobalUnlock(m_hDevNames);	


	return TRUE;


	/*


	WriteDWORD(GlobalSize(m_hDevMode), "DevModeSize");
	WriteDWORD(GlobalSize(m_hDevNames), "DevNamesSize");


    WriteString(CString(pDevMode->dmDeviceName), "DeviceName");
    WriteDWORD(pDevMode->dmSpecVersion, "SpecVersion");
    WriteDWORD(pDevMode->dmDriverVersion, "DriverVersion");
    WriteDWORD(pDevMode->dmSize, "Size");
    WriteDWORD(pDevMode->dmDriverExtra, "DriverExtra");
    WriteDWORD(pDevMode->dmFields, "Fields");
    WriteDWORD(pDevMode->dmOrientation, "Orientation");
    WriteDWORD(pDevMode->dmPaperSize, "PaperSize");
    WriteDWORD(pDevMode->dmPaperLength, "PaperLength");
    WriteDWORD(pDevMode->dmPaperWidth, "PaperWidth");
	WriteDWORD(pDevMode->dmPosition.x, "Positionx");
	WriteDWORD(pDevMode->dmPosition.y, "Positiony");
    WriteDWORD(pDevMode->dmScale, "Scale");
    WriteDWORD(pDevMode->dmCopies, "Copies");
    WriteDWORD(pDevMode->dmDefaultSource, "DefaultSource");
    WriteDWORD(pDevMode->dmPrintQuality, "PrintQuality");
    WriteDWORD(pDevMode->dmColor, "Color");
    WriteDWORD(pDevMode->dmDuplex, "Duplex");
    WriteDWORD(pDevMode->dmYResolution, "YResolution");
    WriteDWORD(pDevMode->dmTTOption, "TTOption");
    WriteDWORD(pDevMode->dmCollate, "Collate");
    WriteString(CString(pDevMode->dmFormName), "FormName");
    WriteDWORD(pDevMode->dmLogPixels, "LogPixels");
    WriteDWORD(pDevMode->dmBitsPerPel, "BitsPerPel");
    WriteDWORD(pDevMode->dmPelsWidth, "PelsWidth");
    WriteDWORD(pDevMode->dmPelsHeight, "PelsHeight");
    WriteDWORD(pDevMode->dmDisplayFlags, "DisplayFlags");
    WriteDWORD(pDevMode->dmDisplayFrequency, "DisplayFrequency");


	LPSTR szDevice=NULL, szDriver=NULL, szOutput=NULL;

	szDriver = (LPSTR) pDevNames + pDevNames->wDriverOffset;
    szDevice = (LPSTR) pDevNames + pDevNames->wDeviceOffset;
    szOutput = (LPSTR) pDevNames + pDevNames->wOutputOffset;
   

	WriteString( szDriver, "Driver" );
	WriteString( szDevice, "Device" );
	WriteString( szOutput, "Output" );


	WriteDWORD(pDevNames->wDriverOffset, "DriverOffset");
	WriteDWORD(pDevNames->wDeviceOffset, "DeviceOffset");
	WriteDWORD(pDevNames->wOutputOffset, "OutputOffset");
	WriteDWORD(pDevNames->wDefault, "Default");		


	DWORD dwDriverDataLen = GlobalSize( m_hDevMode ) - sizeof( DEVMODE );
	if( dwDriverDataLen > 0 )
	{
		CFile file;
		if( file.Open( strDir + szPrinterDataFile, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary ) )
		{
			BYTE* pData = (BYTE*)pDevMode + (BYTE)dwDriverDataLen;
			file.Write( pData, dwDriverDataLen );

			file.Close();
		}

		
	}


	GlobalUnlock(m_hDevMode);
	GlobalUnlock(m_hDevNames);	


	return TRUE;

  	*/

}


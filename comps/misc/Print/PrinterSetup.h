// PrinterSetup.h: interface for the CPrinterSetup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINTERSETUP_H__E03CFAC5_0FF2_11D4_8140_0000E8DF68C3__INCLUDED_)
#define AFX_PRINTERSETUP_H__E03CFAC5_0FF2_11D4_8140_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// vanek - 09.10.2003
// Prints Setup Dialog Controls IDs
#define IDC_PAPER_FRAME			0x431
#define IDC_SIZE_STATIC			0x441
#define IDC_SIZE_COMBO			0x471
#define IDC_SOURCE_STATIC		0x442
#define IDC_SOURCE_COMBO		0x472

#define IDC_ORIENTATION_FRAME	0x430
#define IDC_PAGE_STATIC			0x43C
#define IDC_PORTRAIT_RADIOBTN	0x420
#define IDC_LANDSCAPE_RADIOBTN	0x421

#define IDC_PRINTER_FRAME	0x433

#define IDC_NETWORK_BUTTON		0x40D
//

class CPrinterSetup  
{
public:
	CPrinterSetup();
	virtual ~CPrinterSetup();
	
	BOOL ChooseAnother( CWnd* pWnd = NULL );
	BOOL _ChooseAnother( CWnd* pWnd, BOOL& bFailed );

	BOOL InitDefaults();

	void SetFileName(CString strFileName){ m_strFileName = strFileName; }

	BOOL Read( bool bForceWrite = true );
	BOOL Write();	

	CRect GetPrinterMargins( IUnknown* punk_report );

	HDC GetHDC( IUnknown* punk_report );

protected:
	CString m_strFileName;
	HANDLE	m_hDevMode;
	HANDLE	m_hDevNames;	

protected:
	HANDLE  CopyHandle(HANDLE h);
	BOOL IsPrinterAvailable();

	void WriteString(CString str, CString strKey);
	CString ReadString(CString strKey);

	void WriteDWORD(DWORD dw, CString strKey);
	DWORD ReadDWORD(CString strKey);


	bool	m_bNeedRecalculateMargins;
	CRect	m_rcMargins;

};




#endif // !defined(AFX_PRINTERSETUP_H__E03CFAC5_0FF2_11D4_8140_0000E8DF68C3__INCLUDED_)

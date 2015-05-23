#if !defined(AFX_CLIPBOARDDLG_H__E5DFD9C1_BD67_436E_8E5D_79D86A63CFDD__INCLUDED_)
#define AFX_CLIPBOARDDLG_H__E5DFD9C1_BD67_436E_8E5D_79D86A63CFDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClipboardDlg.h : header file
//

#include "Reposition.h"

class CClipboardDIB
{
public:
	HGLOBAL hMem;
	LPBITMAPINFOHEADER lpbi;
	LPBYTE pData;
	CClipboardDIB()
	{
		Open();
	}
	~CClipboardDIB()
	{
		Close();
	}
	void Open()
	{
		if (OpenClipboard(NULL))
		{
			hMem = GetClipboardData(CF_DIB);
			if (hMem)
			{
				lpbi = (LPBITMAPINFOHEADER)GlobalLock(hMem);
				int nClrUsed = lpbi->biClrUsed==0?lpbi->biBitCount==8?256:0:lpbi->biClrUsed;
				pData = (LPBYTE)(((RGBQUAD *)(lpbi+1))+nClrUsed);
			}
			else lpbi = NULL;
		}
		else lpbi = NULL;
	}
	void Close()
	{
		if (hMem)
			GlobalUnlock(hMem);
		CloseClipboard();
	}
};

/////////////////////////////////////////////////////////////////////////////
// CClipboardDlg dialog

class CDriverClipboard;
class CClipboardDlg : public CDialog, public CRepositionManager
{
// Construction
	CDriverClipboard *m_pDrv;
	CRect m_rcBase;
	int m_nEnableOk;
	bool RepositionWindow();
	CSize m_sz;
public:
	CClipboardDlg(CDriverClipboard *pDrv, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CClipboardDlg)
	enum { IDD = IDD_CLIPBOARD_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClipboardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClipboardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIPBOARDDLG_H__E5DFD9C1_BD67_436E_8E5D_79D86A63CFDD__INCLUDED_)

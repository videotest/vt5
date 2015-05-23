// DirDlg.h: interface for the CDirDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRDLG_H__66F1A742_6668_11D1_B30B_004095453388__INCLUDED_)
#define AFX_DIRDLG_H__66F1A742_6668_11D1_B30B_004095453388__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include <string>
//using namespace std;

class CDirDialog  
{
public:
	CDirDialog();
	virtual ~CDirDialog();

public: 
    CString m_Path;
    CString m_InitDir;
    CString m_Title;
    int     m_ImageIndex;

public:
    int DoBrowse ();
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);


};

#endif // !defined(AFX_DIRDLG_H__66F1A742_6668_11D1_B30B_004095453388__INCLUDED_)

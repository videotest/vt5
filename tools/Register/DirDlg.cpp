// DirDlg.cpp: implementation of the CDirDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirDlg.h"
#include <shlobj.h>
//#include <string>
//using namespace std;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirDialog::CDirDialog()
{

}

CDirDialog::~CDirDialog()
{

}

int CDirDialog::DoBrowse ()
{
	char temp[MAX_PATH];


    LPMALLOC pMalloc;

    if (SHGetMalloc(&pMalloc) != NOERROR) 
        return FALSE;

    BROWSEINFO bInfo;
    LPITEMIDLIST pidl;
    memset((void*)&bInfo, 0, sizeof(BROWSEINFO));

    bInfo.hwndOwner = NULL;
    bInfo.pszDisplayName = temp;
    bInfo.lpszTitle = (m_Title.IsEmpty()) ? "Open" : m_Title;
    bInfo.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | BIF_BROWSEINCLUDEFILES;
	bInfo.lpfn = (BFFCALLBACK)BrowseCallbackProc;
	bInfo.lParam = (LPARAM)this;

    if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL) 
	{
        return FALSE;
    }
    m_ImageIndex = bInfo.iImage;

    if (::SHGetPathFromIDList(pidl, temp) == FALSE) 
	{
        pMalloc->Free(pidl);
        pMalloc->Release();
        return FALSE;
    }
	m_Path = CString(temp);

    pMalloc->Free(pidl);
    pMalloc->Release();

    return TRUE;
}


int CALLBACK CDirDialog::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	CDirDialog* pdlg = (CDirDialog*)lpData;
	if (uMsg == BFFM_INITIALIZED && !pdlg->m_InitDir.IsEmpty())
	{
//	    LPMALLOC pMalloc;
		LPITEMIDLIST  pidl;
        OLECHAR       olePath[MAX_PATH];
        ULONG         chEaten;
        ULONG         dwAttributes;
        HRESULT       hr;
        LPSHELLFOLDER pDesktopFolder;

        // // Get a pointer to the Desktop's IShellFolder interface. //
        if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder))) 
		{
            // IShellFolder::ParseDisplayName requires the file name be in Unicode.
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pdlg->m_InitDir, -1, olePath, MAX_PATH);
            // Convert the path to an ITEMIDLIST.
            hr = pDesktopFolder->ParseDisplayName(NULL, NULL, olePath, &chEaten, &pidl, &dwAttributes);

            if (FAILED(hr)) 
			{
//              pMalloc->Free(pidl);
//              pMalloc->Release();
                return 0;
            }
			SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)FALSE, (LPARAM)pidl);
        }
	}
	return 0;
}

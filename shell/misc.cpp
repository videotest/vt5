#include "stdafx.h"
#include "misc.h"

#include "shell.h"
#include "mainfrm.h"
#include "scriptdocint.h"



CMainFrame *GetMainFrame()
{
	CWnd	*pwnd = AfxGetMainWnd();

	ASSERT( pwnd );
	ASSERT_VALID( pwnd );

	ASSERT_KINDOF(CMainFrame, pwnd );

	return (CMainFrame*)pwnd;
}


void ExecuteObjectScript( IUnknown *punkDoc, const char *psz, const char *psz_doc_name )
{
	IUnknown *punkObj = ::GetObjectByName( punkDoc, psz );
	if( CheckInterface( punkObj, IID_IScriptDataObject ) )
	{
		sptrIScriptDataObject	sptrS( punkObj );
		BSTR	bstrScriptText = 0;
		//execute the script
		sptrS->GetText( &bstrScriptText );
		::ExecuteScript( bstrScriptText, psz_doc_name, false, GetObjectKey( punkDoc ) );
		::SysFreeString( bstrScriptText );
	}

	if( punkObj )
		punkObj->Release();
}

CPrioritiesMgr::CPrioritiesMgr()
{
}

CPrioritiesMgr::~CPrioritiesMgr()
{
	for (int i = 0; i < GetSize(); i++)
		delete GetAt(i);
	RemoveAll();
}

void CPrioritiesMgr::ReadPriority(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault, CWnd *pWnd)
{
	ASSERT(pWnd);
	int nPriority = ::GetValueInt(GetAppUnknown(), lpstrSec, lpstrEntry, nDefault);
	if (nPriority >= GetSize())
		SetSize(nPriority+1);
	if (GetAt(nPriority) == NULL)
		SetAt(nPriority, new CPriorityArr);
	CPriorityArr *p = GetAt(nPriority);
	p->Add(pWnd);
}

void CPrioritiesMgr::Reposition()
{
	for (int i = (int)GetSize()-1; i >= 0; i--)
	{
		CPriorityArr *p = GetAt(i);
		if (p)
		{
			for (int j = 0; j < p->GetSize(); j++)
			{
				CWnd *pWnd = p->GetAt(j);
				pWnd->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			}
		}
	}
}


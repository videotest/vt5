#include "stdafx.h"
#include "actions.h"
#include "window5.h"
#include "apportionment.h"
#include "ArrangeDlg.h"

//////////////////////////////////////////////////////////////////////
//CActionShowApportionment
/*
HRESULT CActionShowApportionment::DoInvoke()
{
	if( m_ptrTarget == NULL )
		return E_FAIL;

	sptrIFrameWindow	sptrF( m_ptrTarget );
	if( sptrF == NULL )
		return E_FAIL;

	IUnknown	*punkSplitter = 0;

	sptrF->GetSplitter( &punkSplitter );
	if( !punkSplitter )
		return E_FAIL;

	sptrISplitterWindow	sptrS( punkSplitter );
	punkSplitter->Release();

	if( sptrS == NULL )
		return E_FAIL;	
		
	int	nRow, nCol;

	_bstr_t	bstrViewProgID( szAViewProgID );
	sptrS->GetActivePane( &nRow, &nCol );
	sptrS->ChangeViewType( nRow, nCol, bstrViewProgID );

	IUnknown	*punkView = 0;
	sptrS->GetViewRowCol( nCol, nRow, &punkView );

	if( !punkView )return E_FAIL;
	punkView->Release();

	return S_OK;
}
*/


//////////////////////////////////////////////////////////////////////
//CActionClassesArrange
_ainfo_base::arg	CActionClassesArrangeInfo::s_pargs[] = 
{	
	{"ClassList",	szArgumentTypeClassList, 0, true, true },
	{0, 0, 0, false, false },
};

HRESULT CActionClassesArrange::DoInvoke()
{
	arg* parg = find(_bstr_t("ClassList"));
	if(!parg)
		return S_FALSE;

	_bstr_t bstrName(parg->value);

	IUnknown* punkClassList = GetObjectByName(m_ptrTarget, bstrName, _bstr_t(szArgumentTypeClassList));

	if(!punkClassList)
		punkClassList = GetActiveObjectFromContext(m_ptrTarget, szArgumentTypeClassList);

	if(!punkClassList)
		return S_FALSE;
	
	
	CArrangeDlg dlg(punkClassList);
	if(dlg.DoModal() == IDOK)
	{
	}


	punkClassList->Release();
	
	return S_OK;
}

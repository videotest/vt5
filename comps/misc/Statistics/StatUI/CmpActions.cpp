#include "StdAfx.h"
#include "StatUI.h"
#include "CmpStatObjectView.h"
#include "CmpActions.h"

#include "statui_consts.h"
#include "misc_utils.h"

//////////////////////////////////////////////////////////////////////
//
//	class CShowStatCmpView
//
//////////////////////////////////////////////////////////////////////
namespace ViewSpace
{
namespace ViewActions
{

/**********************************************************************************/
bool SubViewAvaibleBase::_is_avaible( IUnknownPtr sptrT )
{
	if( sptrT == 0 )
		return false;

	IStatObjectViewPtr sptrV = sptrT;

	if( sptrV == 0 )
		return false;

	IUnknown *punkO = GetActiveObjectFromContext( sptrV, szTypeCmpStatObject );

	if( !punkO )
		return false;

	punkO->Release();

	return true;
}
bool SubViewAvaibleBase::_is_checked() { return false;}
/**********************************************************************************/
_ainfo_base::arg CActionChartCmpOnOffInfo::s_pargs[] =
{
	{"Show",szArgumentTypeInt, "-1", true, false },
	{0, 0, 0, false, false },
};
CActionChartCmpOnOff::CActionChartCmpOnOff()
{
}

//////////////////////////////////////////////////////////////////////
CActionChartCmpOnOff::~CActionChartCmpOnOff()
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionChartCmpOnOff::DoInvoke()
{
	if( m_ptrTarget == 0 )
		return E_FAIL;

	long lShow = GetArgLong( "Show" );

	IStatObjectViewPtr sptrView = m_ptrTarget;

	DWORD dwViews = 0;
	sptrView->GetViewVisibility( &dwViews );

	if( lShow == -1 )
		sptrView->ShowView( dwViews ^ vtChartView );
	else
	{
		if( dwViews & vtChartView && !lShow )
			sptrView->ShowView( dwViews & ~vtChartView );
		if( !( dwViews & vtChartView ) && lShow )
			sptrView->ShowView( dwViews | vtChartView );		  
	}

	INotifyControllerPtr	sptr = ::GetAppUnknown();

	if( sptr== 0 )
		return E_FAIL;

	_bstr_t	bstrEvent( szEventNewSettingsView );
	sptr->FireEvent( bstrEvent, 0, 0, 0, 0 );
							  
	return S_OK;
}
HRESULT CActionChartCmpOnOff::GetActionState( DWORD *pdwState )
{
	HRESULT hr = __super::GetActionState( pdwState );

	if( !_is_avaible( m_ptrTarget ) ) 
		*pdwState &= ~afEnabled;

	if( _is_checked() ) 
		*pdwState |= afChecked;

	return hr;
}
bool CActionChartCmpOnOff::_is_checked() 
{ 
	if( _is_avaible( m_ptrTarget ) ) 
	{
		IStatObjectViewPtr sptrView = m_ptrTarget;

		DWORD dwViews = 0;
		sptrView->GetViewVisibility( &dwViews );

		if( dwViews & vtChartView )
			return true;
	}
	return false;
};
/**********************************************************************************/
_ainfo_base::arg CActionLegendCmpOnOffInfo::s_pargs[] =
{
	{"Show",szArgumentTypeInt, "-1", true, false },
	{0, 0, 0, false, false },
};																  
CActionLegendCmpOnOff::CActionLegendCmpOnOff()
{
}

//////////////////////////////////////////////////////////////////////
CActionLegendCmpOnOff::~CActionLegendCmpOnOff()
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionLegendCmpOnOff::DoInvoke()
{
	if( m_ptrTarget == 0 )
		return E_FAIL;									   

	long lShow = GetArgLong( "Show" );

	IStatObjectViewPtr sptrView = m_ptrTarget;

	DWORD dwViews = 0;
	sptrView->GetViewVisibility( &dwViews );
														  
	if( lShow == -1 )
		sptrView->ShowView( dwViews ^ vtLegendView );
	else
	{
		if( dwViews & vtLegendView && !lShow )
			sptrView->ShowView( dwViews & ~vtLegendView );		  
		if( !( dwViews & vtLegendView ) && lShow )
			sptrView->ShowView( dwViews | vtLegendView );
	}

	INotifyControllerPtr	sptr = ::GetAppUnknown();

	if( sptr== 0 )
		return E_FAIL;

	_bstr_t	bstrEvent( szEventNewSettingsView );
	sptr->FireEvent( bstrEvent, 0, 0, 0, 0 );

	return S_OK;

}
HRESULT CActionLegendCmpOnOff::GetActionState( DWORD *pdwState )
{
	HRESULT hr = __super::GetActionState( pdwState );

	if( !_is_avaible( m_ptrTarget ) ) 
		*pdwState &= ~afEnabled;

	if( _is_checked() ) 
		*pdwState |= afChecked;

	return hr;
}
bool CActionLegendCmpOnOff::_is_checked() 
{ 
	if( _is_avaible( m_ptrTarget ) ) 
	{
		IStatObjectViewPtr sptrView = m_ptrTarget;

		DWORD dwViews = 0;
		sptrView->GetViewVisibility( &dwViews );

		if( dwViews & vtLegendView )
			return true;
	}
	return false;
};
/**********************************************************************************/
_ainfo_base::arg CActionTableCmpOnOffInfo::s_pargs[] =
{
	{"Show",szArgumentTypeInt, "-1", true, false },
	{0, 0, 0, false, false },
};
CActionTableCmpOnOff::CActionTableCmpOnOff()
{
}

//////////////////////////////////////////////////////////////////////
CActionTableCmpOnOff::~CActionTableCmpOnOff()
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionTableCmpOnOff::DoInvoke()
{
	if( m_ptrTarget == 0 )
		return E_FAIL;

	long lShow = GetArgLong( "Show" );

	IStatObjectViewPtr sptrView = m_ptrTarget;

	DWORD dwViews = 0;
	sptrView->GetViewVisibility( &dwViews );

	if( lShow == -1 )
		sptrView->ShowView( dwViews ^ vtTableView );
	else
	{
		if( dwViews & vtTableView && !lShow )
			sptrView->ShowView( dwViews & ~vtTableView );
		if( !( dwViews & vtTableView ) && lShow )
			sptrView->ShowView( dwViews | vtTableView );
	}

	INotifyControllerPtr	sptr = ::GetAppUnknown();

	if( sptr== 0 )
		return E_FAIL;

	_bstr_t	bstrEvent( szEventNewSettingsView );
	sptr->FireEvent( bstrEvent, 0, 0, 0, 0 );


	return S_OK;
}
HRESULT CActionTableCmpOnOff::GetActionState( DWORD *pdwState )
{
	HRESULT hr = __super::GetActionState( pdwState );

	if( !_is_avaible( m_ptrTarget ) ) 
		*pdwState &= ~afEnabled;

	if( _is_checked() ) 
		*pdwState |= afChecked;

	return hr;
}
bool CActionTableCmpOnOff::_is_checked() 
{ 
	if( _is_avaible( m_ptrTarget ) ) 
	{
		IStatObjectViewPtr sptrView = m_ptrTarget;

		DWORD dwViews = 0;
		sptrView->GetViewVisibility( &dwViews );

		if( dwViews & vtTableView )
			return true;
	}
	return false;
}

HRESULT CShowStatCmpView::DoInvoke()
{
	CActionShowView::DoInvoke();
	IUnknownPtr	punkView;
	{
		sptrIFrameWindow	sptrF( m_ptrTarget );
		IUnknownPtr punkSplitter;
		sptrF->GetSplitter( &punkSplitter );
		sptrISplitterWindow	sptrS( punkSplitter );
		int	nRow, nCol;
		_bstr_t	bstrViewProgID = GetViewProgID();
		sptrS->GetActivePane( &nRow, &nCol );
		sptrS->ChangeViewType( nRow, nCol, bstrViewProgID );
		sptrS->GetViewRowCol( nRow, nCol, &punkView );
	}
	HRESULT hr=S_FALSE;
	if(IStatObjectViewPtr pStatViewI=punkView){
		if(IUnknownPtr punkO = GetActiveObjectFromContext(punkView,szTypeCmpStatObject))
		{
			CCmpStatObjectView* pCmpStatObjectView=(CCmpStatObjectView*)(IStatObjectView*)pStatViewI;
			pCmpStatObjectView->m_bReadyToShow=true;
			if(pCmpStatObjectView->m_pCmpStatObject)
			{
				CRect rcClient;
				::GetClientRect(pCmpStatObjectView->hwnd(),&rcClient);
				SIZE sizeClient={rcClient.Width(),rcClient.Height()};
				SIZE size =pCmpStatObjectView->_recalc_layout(sizeClient);
				hr=S_OK;
			}
		}
	}
	return hr;
}

/**********************************************************************************/
} // ViewActions
} // ViewSpace

#include "StdAfx.h"
#include "StatUI.h"
#include "statobjectview.h"
#include "actions.h"

#include "statui_consts.h"
#include "misc_utils.h"

//////////////////////////////////////////////////////////////////////
//
//	class CShowStatObjectView
//
//////////////////////////////////////////////////////////////////////
namespace ViewSpace
{
namespace ViewActions
{

/**********************************************************************************/
bool _SubViewAviableBase::_is_avaible( IUnknownPtr sptrT )
{
	if( sptrT == 0 )
		return false;

	IStatObjectViewPtr sptrV = sptrT;

	if( sptrV == 0 )
		return false;

	IUnknown *punkO = GetActiveObjectFromContext( sptrV, szTypeStatObject );

	if( !punkO )
		return false;

	punkO->Release();

	return true;
}
bool _SubViewAviableBase::_is_checked() { return false;}
/**********************************************************************************/
_ainfo_base::arg CActionChartViewOnOffInfo::s_pargs[] =
{
	{"Show",szArgumentTypeInt, "-1", true, false },
	{0, 0, 0, false, false },
};
CActionChartViewOnOff::CActionChartViewOnOff()
{
}

//////////////////////////////////////////////////////////////////////
CActionChartViewOnOff::~CActionChartViewOnOff()
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionChartViewOnOff::DoInvoke()
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
HRESULT CActionChartViewOnOff::GetActionState( DWORD *pdwState )
{
	HRESULT hr = __super::GetActionState( pdwState );

	if( !_is_avaible( m_ptrTarget ) ) 
		*pdwState &= ~afEnabled;

	if( _is_checked() ) 
		*pdwState |= afChecked;

	return hr;
}
bool CActionChartViewOnOff::_is_checked() 
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
_ainfo_base::arg CActionLegendViewOnOffInfo::s_pargs[] =
{
	{"Show",szArgumentTypeInt, "-1", true, false },
	{0, 0, 0, false, false },
};																  
CActionLegendViewOnOff::CActionLegendViewOnOff()
{
}

//////////////////////////////////////////////////////////////////////
CActionLegendViewOnOff::~CActionLegendViewOnOff()
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionLegendViewOnOff::DoInvoke()
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
HRESULT CActionLegendViewOnOff::GetActionState( DWORD *pdwState )
{
	HRESULT hr = __super::GetActionState( pdwState );

	if( !_is_avaible( m_ptrTarget ) ) 
		*pdwState &= ~afEnabled;

	if( _is_checked() ) 
		*pdwState |= afChecked;

	return hr;
}
bool CActionLegendViewOnOff::_is_checked() 
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
_ainfo_base::arg CActionTableViewOnOffInfo::s_pargs[] =
{
	{"Show",szArgumentTypeInt, "-1", true, false },
	{0, 0, 0, false, false },
};
CActionTableViewOnOff::CActionTableViewOnOff()
{
}

//////////////////////////////////////////////////////////////////////
CActionTableViewOnOff::~CActionTableViewOnOff()
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionTableViewOnOff::DoInvoke()
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
HRESULT CActionTableViewOnOff::GetActionState( DWORD *pdwState )
{
	HRESULT hr = __super::GetActionState( pdwState );

	if( !_is_avaible( m_ptrTarget ) ) 
		*pdwState &= ~afEnabled;

	if( _is_checked() ) 
		*pdwState |= afChecked;

	return hr;
}
bool CActionTableViewOnOff::_is_checked() 
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

HRESULT CShowStatObjectView::DoInvoke()
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
		if(IUnknownPtr punkO = GetActiveObjectFromContext(punkView,szTypeStatObject))
		{
			CStatObjectView* pCStatObjectView=(CStatObjectView*)(IStatObjectView*)pStatViewI;
			pCStatObjectView->m_bReadyToShow=true;
			if(pCStatObjectView->m_list_attached.count())
			{
				using namespace ObjectSpace;
				IStatObjectPtr sptr_stat_object = pCStatObjectView->m_list_attached.get
					(pCStatObjectView->m_list_attached.head());
				CStatisticObject* rso=(CStatisticObject*)((IStatObject*)sptr_stat_object);
				hr=rso->SetValidateLock(0);

				CRect rcClient;
				::GetClientRect(pCStatObjectView->hwnd(),&rcClient);
				SIZE sizeClient={rcClient.Width(),rcClient.Height()};
				SIZE size =pCStatObjectView->_recalc_layout(sizeClient);
			}
		}
	}
	return hr;
}

/**********************************************************************************/
} // ViewActions
} // ViewSpace

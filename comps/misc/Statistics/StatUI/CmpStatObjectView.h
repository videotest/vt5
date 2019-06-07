#pragma once

#define WM_USER_MIN_SIZE		WM_USER + 0x125

#include "statistics.h"
#include "win_view.h"
#include "CmpStatObject.h"

#include "CmpTablewnd.h"
#include "CmpLegendwnd.h"
#include "CmpChartwnd.h"
#include "PropBagImpl.h"
#include "SplitView.h"
#include "ScrollView.h"

namespace ViewSpace 
{

	class CCmpStatObjectView : public CWinViewBase, 
					public IPrintView,
					public IPersist,
					public IStatObjectView,
					public CNamedObjectImpl,
					public CNamedPropBagImpl,
					public _dyncreate_t<CCmpStatObjectView>,
					public CHelpInfoImpl,
					public CView<CCmpStatObjectView>
	{
		bool m_bError;
		CString m_strErrorDescr;

		int m_nSplitterType; // Вид сплиттера

		int m_nPane0_Data; // Данные в пане 0
		int m_nPane1_Data; // Данные в пане 1
		int m_nPane2_Data; // Данные в пане 2
		bool m_bPrintMode, m_bShowContext;
	private:

		RECT m_rcPrev;

		CRect m_rcChartView, m_rcLegendView, m_rcTableView;

		RECT m_rcVertSplitter;
		RECT m_rcHorzSplitter;
		

		double m_fVertSplitter, m_fHorzSplitter;
		// [vanek] BT2000:4232 - 29.12.2004
		long m_dx_error, m_dy_error;

		int m_nSplitterHalfWidth;
		bool m_bShowen;

		bool m_bLockRecalc;
	public:
		ObjectSpace::CCmpStatObject* m_pCmpStatObject;
		CCmpChartWnd m_wndChart;
		CCmpTableWnd m_wndTable;
		CCmpLegendWnd m_wndLegend;
		CSplitterLtdWindow m_wndVertSplitter;
		CHorSplitterLtdWindow m_wndHorzSplitter;
		DWORD m_dwViews;

		CCmpStatObjectView();
		virtual ~CCmpStatObjectView();
		void set_modify();
		virtual IUnknown*	DoGetInterface( const IID &iid );
		
		route_unknown();
//		_list_t<IUnknownPtr> m_list_attached;

		// IView
		com_call GetObjectFlags( BSTR bstrObjectType, DWORD *pdwFlags /*ObjectFlags*/)
		{
			HRESULT hr = CWinViewBase::GetObjectFlags( bstrObjectType, pdwFlags );
            
			if( _bstr_t(bstrObjectType) == _bstr_t(szTypeCmpStatObject) )
				*pdwFlags |= ofAllSelected;

			return hr;
		}

	protected:
		virtual void		DoAttachObjects();	

		void default_init();

		void load_from_ndata(  INamedDataPtr sptrND );
		void store_to_ndata( INamedDataPtr sptrND );

		//mesage handlers
		virtual LRESULT		DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );

		virtual long		on_create( CREATESTRUCT *pcs );
		virtual long		on_destroy();
		virtual long		on_paint();
		virtual long		on_erase_background( HDC hDC );		//WM_ERASEBKGND
		virtual long		on_size( int cx, int cy );			//WM_SIZE	
		virtual long		on_context_menu( HWND hWnd, int x, int y );	//WM_CONTEXTMENU

		//interface overriden
		com_call			GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch );	
		virtual void		OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

		//Print support
		com_call			GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX );
		com_call			GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY );
		com_call			Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags );

		//Persist Impl
		com_call			GetClassID(CLSID *pClassID ); 

		com_call			GetFirstVisibleObjectPosition(TPOS *plpos);
		com_call			GetNextVisibleObject(IUnknown ** ppunkObject, TPOS *plPos);

		const RECT _calc_min_view_size();

//		const RECT get_min_rect(void){return ;

		bool _is_ndata_empty( IUnknownPtr sptrND );
		void attach_data();
		void _add_attach_data( IUnknownPtr sptrO )
		{
			if(ICmpStatObjectDispPtr pCmpStatDisp=sptrO)
			{
				m_pCmpStatObject=0;
				if(CCmpStatObject* rcso=(CCmpStatObject*)((ICmpStatObjectDisp*)pCmpStatDisp))
				{
					if(rcso->_statObjects.size()>0)
					{
						m_pCmpStatObject=rcso;
						m_wndLegend._add_attach_data(sptrO);
					}
				}
			}
		}

		//IStatObjectView
		com_call ShowView( DWORD dwView );
		com_call GetViewVisibility( DWORD *pdwView );
		bool _check_for_error( IUnknownPtr sptrData );
		//bool _check_for_error()
		//{
		//	for( LPOS lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
		//	{
		//		IUnknownPtr ptr_object = m_list_attached.get( lpos_lst );
		//		if( ptr_object )
		//			if( _check_for_error( ptr_object ) )
		//				return true;
		//	}
		//	return false;
		//}

		void OnChange();

		void _clear_attached();
	public:
		bool m_bReadyToShow;
		const SIZE _recalc_layout(const SIZE sizeRect);
		HWND m_hwSplitRoot;
	private:
		HWND SetSplitterStruct(void);
	};
}
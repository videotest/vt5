#pragma once

#include "mics_class.h"
#include "idio_int.h"
#include "idiodbase_int.h"
#include "carioint.h"

using namespace GallerySpace;

const int _BORDER = 10;
const int _CX = 2 * _BORDER + 15;
const int _CY = 2 * _BORDER + 30;

////////////////////////////////////////////////////////////////////////////
class CGalleryIdioItem : public CGalleryItemBase
{
	int m_nIdioWidth;
	bool m_bShowNums;
public:
	CGalleryIdioItem()
	{
		m_nIdioWidth  = ::GetValueInt( ::GetAppUnknown(), KARIO_IDIODB_PREVIEW, KARYO_IDIOWIDTH, 20 );
		m_bShowNums = ::GetValueInt( ::GetAppUnknown(), KARIO_IDIODB_PREVIEW, CARIO_SHOW_NUMS, 0 ) != 0;
	}
	bool GetShowNums() { return m_bShowNums; }
	void DoDraw( CDC *pDc, CWnd *pWnd );
};
////////////////////////////////////////////////////////////////////////////
class CGalleryIdioCell : public CGalleryCellBase< CGalleryIdioItem >
{
	CString m_strText;
	LOGFONT m_lfClassText;
	long m_nClassNum;
public:
	CGalleryIdioCell()
	{
		::ZeroMemory( &m_lfClassText, sizeof( m_lfClassText ) );
		m_nClassNum = -1;

		m_lfClassText.lfHeight = 16;
		m_lfClassText.lfCharSet = 1;

		strcpy( m_lfClassText.lfFaceName, "Tahoma" );

	}
	void DoDraw( CDC *pDc, CWnd *pWnd )
	{
		int br = 10;
		CRect rc = GetRect();

		if( rc == NORECT )
			return;

		rc = ::ConvertToWindow( pWnd->GetControllingUnknown(), rc );

		CPoint pt = pDc->MoveTo( rc.left, rc.bottom - br );
		pDc->LineTo( rc.left, rc.bottom );
		pDc->LineTo( rc.right, rc.bottom );
		pDc->LineTo( rc.right, rc.bottom - br );

		pDc->MoveTo( pt );

		rc.top = rc.bottom + 5;
		rc.bottom = rc.top + _CY;

		CFont *pOldFont = 0, fnt;

		if( pWnd )
		{
			IScrollZoomSitePtr sptrZoom = pWnd->GetControllingUnknown();

			double fZoom = 0;
			sptrZoom->GetZoom( &fZoom );

			m_lfClassText.lfHeight = long( 16 * fZoom );
		}

		fnt.CreateFontIndirect( &m_lfClassText );

		pOldFont = pDc->SelectObject( &fnt );

		pDc->DrawText( m_strText, &rc, DT_TOP | DT_CENTER | DT_NOCLIP );


		for( TPOS lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next( lPos ) )
		{
			CGalleryIdioItem *pItem = m_pDataArray.get( lPos );
			pItem->DoDraw( pDc, pWnd );
		}

		pDc->SelectObject( pOldFont );
	}
	CString GetText() { return m_strText; }
	void SetText( CString str ) { m_strText = str; }

	long GetClassNum() { return m_nClassNum; }
	void SetClassNum( long num ) { m_nClassNum = num; }
	

	virtual void Store( INamedDataPtr pINamedData, CString strSection )
	{
		__super::Store( pINamedData, strSection );

		pINamedData->SetupSection( _bstr_t( strSection ) ); 

		pINamedData->SetValue( _bstr_t( "CellText" ), _variant_t( _bstr_t(  m_strText ) ) );
		pINamedData->SetValue( _bstr_t( "CellNum" ), _variant_t( m_nClassNum ) );
	}
	virtual void Load( INamedDataPtr pINamedData, CString strSection )
	{
		__super::Load( pINamedData, strSection );

		pINamedData->SetupSection( _bstr_t( strSection ) ); 

		_variant_t var;
		pINamedData->GetValue( _bstr_t( "CellText" ), &var );
		if( var.vt == VT_BSTR )
			m_strText = (char *)_bstr_t( var.bstrVal );

		_variant_t _var( long( 0 ) );
		
		pINamedData->GetValue( _bstr_t( "CellNum" ), &_var );
		m_nClassNum = _var.lVal;
	}
};

class CGalleryIdioLine : public CGalleryLineBase< CGalleryIdioCell >
{
public:
	CGalleryIdioLine()
	{
	}
	void DoDraw( CDC *pDc, CWnd *pWnd )
	{
		for (TPOS lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next(lPos))
		{
			CGalleryIdioCell *pItem = m_pDataArray.get( lPos );
			pItem->DoDraw( pDc, pWnd );
		}
	}
};
////////////////////////////////////////////////////////////////////////////
class CGalleryIdioStorage : public CGalleryStorageBase< CGalleryIdioLine >
{
public:
	CGalleryIdioStorage()
	{
	}
	void DoDraw( CDC *pDc, CWnd *pWnd )
	{
		for (TPOS lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next(lPos))
		{
			CGalleryIdioLine *pItem = m_pDataArray.get( lPos );
			pItem->DoDraw( pDc, pWnd );
		}
	}
};
////////////////////////////////////////////////////////////////////////////
class CIdioDBView : public CGalleryViewBase
{
	friend class CGalleryIdioItem;

	CGalleryIdioStorage m_storage;
	int m_nIdioWidth, m_nIdioHeight;
	bool m_bShowNums, m_bNoContext;
	double m_fMaxLen;

	double			m_centr;
	SIZE			m_size;

	GUARD_DECLARE_OLECREATE(CIdioDBView)
	DECLARE_DYNCREATE(CIdioDBView)
	PROVIDE_GUID_INFO( )
	ENABLE_MULTYINTERFACE();
public:

	CIdioDBView();
// Operations
public:
	//{{AFX_VIRTUAL(CIdioDBView)
	public:
	virtual void OnFinalRelease();
	protected:
	//}}AFX_VIRTUAL
protected:
	virtual ~CIdioDBView();
public:
	virtual DWORD GetMatchType( const char *szType );

	virtual IUnknown *GetObjectByPoint( CPoint point );
	virtual void DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state );
	virtual void DoDraw( CDC *pDc );
	virtual void OnEditCtrlChange( CString str );
	virtual bool	is_have_print_data();
protected:
	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam );

	//{{AFX_MSG(CIdioDBView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CIdioDBView)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART( View, IIdioDBaseView ) 
		com_call SelectByText( BSTR bstr );
		com_call GetActiveObject( IUnknown **punk );
		com_call EnableWindowMessages( BOOL *pbEnable );
		com_call EnableContextMenu( BOOL bEnable );
	END_INTERFACE_PART( View )

	virtual CWnd *GetWindow(){ return this; }

	void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	virtual void attach_data( bool bHasChange = false, IUnknown *punkObjectToRemove = 0 );
	virtual void clear_data();

	void set_layout();
	IUnknown *_get_object_by_key( IUnknownPtr punkDB, GuidKey key );
	CGalleryIdioItem *_get_object( IUnknownPtr sptrO );
	const char* GetViewMenuName();
};

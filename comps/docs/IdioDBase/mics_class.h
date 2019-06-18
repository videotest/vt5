#pragma once

#include "\vt5\awin\misc_list.h"

#define GELLERY_COMMENT_VALUE "CommentVal"
#define GELLERY_COUNT "Count"
#define GELLERY_ITEM_OBJECT "Guid"

const int _BORDER_ = 5;

namespace GallerySpace
{
/////////////////////////////////////////////////////////////////
	class CGalleryItemBase
	{
		IUnknownPtr m_sptrObject;
		GuidKey m_guid;
		
		SIZE m_szText;
		POINT m_ptOffsetText;
		
		CString m_strComment;
	public:
		CGalleryItemBase()
		{
			m_szText = CSize( 0, 0 );
			m_ptOffsetText = CPoint( 0, 0 );
		};
		virtual void DoDraw( CDC *pDc, CWnd *pWnd ) = 0;

		// For Object
		IUnknownPtr GetObject() { return m_sptrObject; }
		void SetObject( IUnknownPtr sptr ) { m_sptrObject = sptr; }

		void SetObjectKey( GuidKey key ) { m_guid = key; }
		GuidKey GetObjectKey() { return m_guid; }

		void SetOffset( int x, int y )
		{
			POINT ptOffset = { x, y };
			SetOffset( ptOffset );
		}
		void SetOffset( POINT ptOffset )
		{ 
			CRect rc = GetItemRect(); 
			SetItemRect( CRect( ptOffset, rc.TopLeft() ) );
		}
		CPoint GetOffset() 
		{
			return GetItemRect().TopLeft(); 
		}

		void SetTextOffset( POINT ptOffset ){ m_ptOffsetText = ptOffset; }
		void SetText( CString str ){ m_strComment = str; }
		CString GetText() { return m_strComment; }
		
		CRect GetItemRect()
		{
			CRect rc;
			
			IRectObjectPtr ptrRect = m_sptrObject;
			ptrRect->GetRect( &rc );

			return rc;
		};
		
		void SetItemRect( CRect rc ) 
		{ 
			IRectObjectPtr ptrRect = m_sptrObject;
			ptrRect->SetRect( rc );
		}

		CRect GetTextRect() { return CRect( /*CPoint( GetOffset() ) + */CPoint( m_ptOffsetText ), m_szText ); }
		void SetTextRect( CRect rc ) 
		{ 
			m_ptOffsetText = rc.TopLeft(); 
			m_szText.cx = rc.Width(); 
			m_szText.cy = rc.Height(); 
		}

		CRect GetRect() 
		{ 
			CRect rc3; 
			rc3.UnionRect( &GetItemRect(), &GetTextRect() ); 
			return rc3; 
		};

		virtual void Store( INamedDataPtr pINamedData, CString strSection )
		{
			if( pINamedData == 0 )
				return;

			pINamedData->SetupSection( _bstr_t( strSection ) );

			m_guid = ::GetObjectKey( m_sptrObject );
			BSTR bstr = 0;
			StringFromCLSID( m_guid, &bstr );
			pINamedData->SetValue( _bstr_t( GELLERY_ITEM_OBJECT ), _variant_t( bstr ) );
			::CoTaskMemFree( bstr );	bstr = 0;

			pINamedData->SetValue( _bstr_t( GELLERY_COMMENT_VALUE ), _variant_t( _bstr_t( m_strComment ) ) );
		}
		virtual void Load( INamedDataPtr pINamedData, CString strSection )
		{
			if( pINamedData == 0 )
				return;

			pINamedData->SetupSection( _bstr_t( strSection ) );

			_variant_t var;

			pINamedData->GetValue( _bstr_t( GELLERY_ITEM_OBJECT ), &var );
			if( var.vt == VT_BSTR )
				CLSIDFromString( var.bstrVal, &m_guid );

			_variant_t comment;
			pINamedData->GetValue( _bstr_t( GELLERY_COMMENT_VALUE ), &comment );

			if( comment.vt == VT_BSTR )
				m_strComment = (char *)_bstr_t( comment.bstrVal );
		}

	protected:
		~CGalleryItemBase(){};
	};
/////////////////////////////////////////////////////////////////
	template < class TType >class CGalleryCellBase
	{
		CRect m_rcCell;
	public:
		CGalleryCellBase()
		{
		}
		
		virtual void DoDraw( CDC *pDc, CWnd *pWnd ) = 0;

		virtual void Store( INamedDataPtr pINamedData, CString strSection )
		{
			if( pINamedData == 0 )
				return;

			pINamedData->SetupSection( _bstr_t( strSection ) );
			pINamedData->SetValue( _bstr_t( GELLERY_COUNT ), _variant_t( m_pDataArray.count() ) );

			for( long lPos = m_pDataArray.head(), i = 0; lPos; lPos = m_pDataArray.next( lPos ),i++ )
			{
				TType *pObj = m_pDataArray.get( lPos );

				CString str;
				str.Format( "%d", i );

				pObj->Store( pINamedData, strSection + "\\Item" + str );
			}
		}
		virtual void Load( INamedDataPtr pINamedData, CString strSection )
		{
			if( pINamedData == 0 )
				return;

			pINamedData->SetupSection( _bstr_t( strSection ) );

			_variant_t count ( long( 0 ) );
			pINamedData->GetValue( _bstr_t( GELLERY_COUNT ), &count );

			for( long i = 0; i < count.lVal; i++ )
			{
				TType *pObj = new TType;

				CString str;
				str.Format( "%d", i );

				pObj->Load( pINamedData, strSection + "\\Item" + str );
				m_pDataArray.add_tail( pObj );
			}
		}
		void SetRect( CRect rc ) { m_rcCell = rc; }
		CRect GetRect() { return m_rcCell; }
		void CalcRect()
		{
			m_rcCell = NORECT;

			for( long lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next( lPos ) )
			{
				TType *pObj = m_pDataArray.get( lPos );
				
				CRect rc = pObj->GetRect();
				m_rcCell.UnionRect( m_rcCell, rc );
			}
			if( m_rcCell != NORECT )
				m_rcCell.InflateRect( _BORDER_, _BORDER_, _BORDER_, _BORDER_ );
		}
		
		void AddItem( TType *pObj ) { m_pDataArray.add_tail( pObj ); }
		void RemoveAll( bool bDelete = true ) { _clear_data( bDelete ); }
		long GetCount( ) { return m_pDataArray.count(); }
		long Remove( long lPos ) 
		{
			long lNext = m_pDataArray.next( lPos );
			m_pDataArray.remove( lPos );
			return lNext;
		}

		long GetFirst() { return m_pDataArray.head(); }
		long GetNext( long lPos ) { return m_pDataArray.next( lPos ); }
		TType *Get( long lPos ) { return m_pDataArray.get( lPos ); }
	protected:
		_list_t<TType *>	m_pDataArray;
		~CGalleryCellBase()
		{
			_clear_data();
		}
	private:
		void _clear_data( bool bDelete = true )
		{
			for( long lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next( lPos ) )
			{
				TType *pObj = m_pDataArray.get( lPos );
				if( bDelete )
					delete pObj;
			}
			m_pDataArray.clear();
		}
	};
/////////////////////////////////////////////////////////////////
	template <class TType>class CGalleryLineBase
	{
		CRect m_rcLine;
	public:
		CGalleryLineBase()
		{
		}
		
		virtual void DoDraw( CDC *pDc, CWnd *pWnd ) = 0;

		virtual void Store( INamedDataPtr pINamedData, CString strSection )
		{
			if( pINamedData == 0 )
				return;

			pINamedData->SetupSection( _bstr_t( strSection ) );
			pINamedData->SetValue( _bstr_t( GELLERY_COUNT ), _variant_t( m_pDataArray.count() ) );

			for( long lPos = m_pDataArray.head(), i = 0; lPos; lPos = m_pDataArray.next( lPos ),i++ )
			{
				TType *pObj = m_pDataArray.get( lPos );

				CString str;
				str.Format( "%d", i );

				pObj->Store( pINamedData, strSection + "\\Cell" + str );
			}
		}
		virtual void Load( INamedDataPtr pINamedData, CString strSection )
		{
			if( pINamedData == 0 )
				return;

			pINamedData->SetupSection( _bstr_t( strSection ) );

			_variant_t count ( long( 0 ) );
			pINamedData->GetValue( _bstr_t( GELLERY_COUNT ), &count );

			for( long i = 0; i < count.lVal; i++ )
			{
				TType *pObj = new TType;

				CString str;
				str.Format( "%d", i );

				pObj->Load( pINamedData, strSection + "\\Cell" + str );
				m_pDataArray.add_tail( pObj );
			}
		}
		void SetRect( CRect rc ) { m_rcLine = rc; }
		CRect GetRect() { return m_rcLine; }
		void CalcRect()
		{
			m_rcLine = NORECT;
			for( long lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next( lPos ) )
			{
				TType *pObj = m_pDataArray.get( lPos );
				
				CRect rc = pObj->GetRect();
				m_rcLine.UnionRect( m_rcLine, rc );
			}
			if( m_rcLine != NORECT )
				m_rcLine.InflateRect( _BORDER_, _BORDER_, _BORDER_, _BORDER_ );
		}

		void AddItem( TType *pObj ) { m_pDataArray.add_tail( pObj ); }
		void RemoveAll( bool bDelete = true ) { _clear_data( bDelete ); }
		long GetCount( ) { return m_pDataArray.count(); }
		long Remove( long lPos ) 
		{
			long lNext = m_pDataArray.next( lPos );
			m_pDataArray.remove( lPos );
			return lNext;
		}

		long GetFirst() { return m_pDataArray.head(); }
		long GetNext( long lPos ) { return m_pDataArray.next( lPos ); }
		TType *Get( long lPos ) { return m_pDataArray.get( lPos ); }
	protected:
		_list_t< TType *>m_pDataArray;
		~CGalleryLineBase()
		{
			_clear_data();
		}
	private:
		void _clear_data( bool bDelete = true )
		{
			for( long lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next( lPos ) )
			{
				TType *pObj = m_pDataArray.get( lPos );
				if( bDelete )
					delete pObj;
			}
			m_pDataArray.clear();
		}
	};

	template <class TType > class CGalleryStorageBase
	{
		CRect m_rcFull;
	public:
		CGalleryStorageBase()
		{
		}
		virtual void Store( IUnknownPtr sptrObject, CString strSection )
		{
			INamedDataPtr pINamedData = sptrObject;
			if( pINamedData == 0 )
			{
				INamedDataObject2Ptr sptrO = sptrObject;
				sptrO->InitAttachedData();
				
				pINamedData = sptrObject;
				if( pINamedData == 0 )
					return;
			}

			pINamedData->DeleteEntry( _bstr_t( strSection ) );
			pINamedData->SetupSection( _bstr_t( strSection ) );
			pINamedData->SetValue( _bstr_t( GELLERY_COUNT ), _variant_t( m_pDataArray.count() ) );

			for( long lPos = m_pDataArray.head(), i = 0; lPos; lPos = m_pDataArray.next( lPos ),i++ )
			{
				TType *pObj = m_pDataArray.get( lPos );

				CString str;
				str.Format( "%d", i );

				pObj->Store( pINamedData, strSection + "\\Line" + str );
			}
		}
		virtual void Load( IUnknownPtr sptrObject, CString strSection )
		{
			INamedDataPtr pINamedData = sptrObject;
			if( pINamedData == 0 )
			{
				INamedDataObject2Ptr sptrO = sptrObject;
				sptrO->InitAttachedData();
				
				pINamedData = sptrObject;
				if( pINamedData == 0 )
					return;
			}

			pINamedData->SetupSection( _bstr_t( strSection ) );

			_variant_t count ( long( 0 ) );
			pINamedData->GetValue( _bstr_t( GELLERY_COUNT ), &count );

			for( long i = 0; i < count.lVal; i++ )
			{
				TType *pObj = new TType;

				CString str;
				str.Format( "%d", i );

				pObj->Load( pINamedData, strSection + "\\Line" + str );
				m_pDataArray.add_tail( pObj );
			}
		}
		void SetRect( CRect rc ) { m_rcFull = rc; }
		CRect GetRect() { return m_rcFull; }
		void CalcRect()
		{
			m_rcFull = NORECT;
			for( long lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next( lPos ) )
			{
				TType *pObj = m_pDataArray.get( lPos );
				
				CRect rc = pObj->GetRect();
				m_rcFull.UnionRect( m_rcFull, rc );
			}
			if( m_rcFull != NORECT )
				m_rcFull.InflateRect( _BORDER_, _BORDER_, _BORDER_, _BORDER_ );
		}

		void AddItem( TType *pObj ) { m_pDataArray.add_tail( pObj ); }
		void RemoveAll( bool bDelete = true ) { _clear_data( bDelete ); }
		long GetCount( ) { return m_pDataArray.count(); }
		long Remove( long lPos ) 
		{
			long lNext = m_pDataArray.next( lPos );
			m_pDataArray.remove( lPos );
			return lNext;
		}

		long GetFirst() { return m_pDataArray.head(); }
		long GetNext( long lPos ) { return m_pDataArray.next( lPos ); }
		TType *Get( long lPos ) { return m_pDataArray.get( lPos ); }

	protected:
		_list_t< TType *> m_pDataArray;
		~CGalleryStorageBase()
		{
			_clear_data();
		}
	private:
		void _clear_data( bool bDelete = true )
		{
			for( long lPos = m_pDataArray.head(); lPos; lPos = m_pDataArray.next( lPos ) )
			{
				TType *pObj = m_pDataArray.get( lPos );
				if( bDelete )
					delete pObj;
			}
			m_pDataArray.clear();
		}
	};
	
	class CEditCtrl;
	class CGalleryViewBase : public CViewBase, public CEasyPreviewImpl
	{
		ENABLE_MULTYINTERFACE();
	protected:
		IUnknownPtr m_sptrContext;
		CString m_strTextSearch;
	public:
		CGalleryViewBase();
		~CGalleryViewBase();

		static CEditCtrl* _stEdit;

		bool GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX );
		bool GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY );
		virtual void Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags );
		virtual bool	is_have_print_data() { return false; }

		//{{AFX_MSG(CGalleryViewBase)
		afx_msg void OnPaint();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		//}}AFX_MSG
		virtual void DoDraw( CDC *pDc ) = 0;
		virtual void OnEditCtrlChange( CString str ) = 0;

		virtual CRect GetObjectRect( IUnknown *punkObj );
		virtual void SetObjectOffset( IUnknown *punkObj, CPoint ptOffset );
		virtual bool HitTest( CPoint point, INamedDataObject2 *pNamedObject );
		virtual bool PtInObject( IUnknown *punkObject, CPoint point );
		virtual IUnknown *GetObjectByPoint( CPoint point ) = 0;

		virtual void AttachActiveObjects();
		void _AttachObjects( IUnknown *punkContextFrom );

		DECLARE_MESSAGE_MAP();
		DECLARE_INTERFACE_MAP();
	protected:
		COLORREF m_clBack;
		CPoint m_pt;
		virtual void attach_data( bool bHasChange = false, IUnknown *punkObjectToRemove = 0 ) = 0;
		virtual void clear_data() = 0;
	public:
	};

	class CEditCtrl : public CEdit
	{
		bool m_bCtrlPressed;
	// Construction		   
	public:
		CEditCtrl() :m_x(0),m_y(0),m_bEscapeKey(FALSE) { m_bCtrlPressed = false; }
		void SetCtrlFont( CFont* pFont );
	// Attributes
	protected:
		int m_x;
		int m_y;
		int m_width;
		int m_height;
		BOOL m_bEscapeKey;
		CFont m_Font;

	// Operations
	public:
	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEditCtrl)
		//}}AFX_VIRTUAL	
	// Implementation
	public:
		virtual ~CEditCtrl() {};			
		// Generated message map functions
	protected:
		//{{AFX_MSG(CLVEdit)	
		afx_msg void OnKillfocus();
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
	protected:	
		virtual void PostNcDestroy();
		virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	};
}

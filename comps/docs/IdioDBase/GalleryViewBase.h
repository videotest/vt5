#pragma once
/*
namespace GallerySpace
{

	#include "\vt5\awin\misc_list.h"
	class CGalleryItemBase
	{
		POINT m_ptOffset;
		SIZE m_szText;
		POINT m_ptOffsetText;
		CString m_strComment;
	public:
		CGalleryItemBase()
		{
			m_sz.cx = 0;
			m_sz.cy = 0;
			m_ptOffsetText = m_ptOffset = CPoint( 0, 0 );
			m_szText = CSize( 0, 0 );
		};
		virtual void DoDraw( CDC *pDc, bool bSelected, CWnd *pWnd ) = 0;
		void SetOffset( POINT ptOffset )
		{
			m_ptOffset = ptOffset;
		}
		void SetTextOffset( POINT ptOffset )
		{
			m_ptOffsetText = ptOffset;
		}
		void SetOffset( int x, int y )
		{
			POINT ptOffset = { x, y };
			SetOffset( ptOffset );
		}
		CPoint GetOffset() { return m_ptOffset; }
		void SetText( CString str ){ m_strComment = str; }
		CString GetText() { return m_strComment; }
		SIZE GetSize()	{ return m_sz; }
		
		CRect GetItemRect() {  return CRect( m_ptOffset, m_sz ); };
		
		void SetItemRect( CRect rc ) 
		{ 
			m_ptOffset = rc.TopLeft(); 
			m_sz.cx = rc.Width(); 
			m_sz.cy = rc.Height(); 
		}

		CRect GetTextRect() { return CRect( CPoint( m_ptOffset ) + CPoint( m_ptOffsetText ), m_szText ); }
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
		
		virtual void Destroy() = 0;
	protected:
		~CGalleryItemBase(){};
		SIZE m_sz;
	};

	class CGalleryViewBase : public CViewBase, public CEasyPreviewImpl
	{
		ENABLE_MULTYINTERFACE();
	public:
		CGalleryViewBase();
		~CGalleryViewBase();

		//{{AFX_MSG(CIdioDBView)
		afx_msg void OnPaint();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		//}}AFX_MSG
		virtual void DoDraw( CDC *pDc );
		virtual void DrawCell( CDC *pDc, const CRect &rc ) = 0;
		virtual void OnEditCtrlChange(CString str) = 0;

		virtual CRect GetObjectRect( IUnknown *punkObj );
		virtual void SetObjectOffset( IUnknown *punkObj, CPoint ptOffset );
		virtual bool HitTest( CPoint point, INamedDataObject2 *pNamedObject );
		virtual bool PtInObject( IUnknown *punkObject, CPoint point );

		DECLARE_MESSAGE_MAP();
		DECLARE_INTERFACE_MAP();
	protected:
		COLORREF m_clBack;
		_list_t<CGalleryItemBase *>	m_pDataArray;
		_list_t<CRect> m_rcCellArr;
		_list_t<CString> m_strCellText;
		_list_t<CRect> m_rcCellTextArr;
		CRect m_rcClient; 
		int m_nActiveItem;
		double m_fZoom;
		POINT m_ptScroll;
		CPoint m_ptStart,m_offsetStart;

		virtual void set_layout() = 0;  // Setting item's position
		virtual void attach_data() = 0; // Used to assign dataobject content to gallery item
		void clear_data();
		
		void _update_scroll_zoom();
	public:
		CPoint convert_to_wnd( CPoint pt );
		CRect convert_to_wnd( CRect rc );

		CPoint convert_to_view( CPoint pt );
		CRect convert_to_view( CRect rc );
	};
}
*/
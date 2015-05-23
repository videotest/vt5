#if !defined(AFX_GALLERYVIEW_H__A39A558F_CAB6_4D0E_A1C3_B1848DC0ED62__INCLUDED_)
#define AFX_GALLERYVIEW_H__A39A558F_CAB6_4D0E_A1C3_B1848DC0ED62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GalleryView.h : header file
//

#include "DBaseListenerImpl.h"
#include "FlterViewImpl.h"
#include "wndmisc5.h"
#include "docmiscint.h"


/*
***********************									Border
						  BORDER_DISTANCE
  00000000000000000000
  00000000000000000000
  00000000000000000000
  00000000000000000000
  00000000000000000000
  00000000000000000000									Picture
  00000000000000000000
  00000000000000000000
  00000000000000000000
  00000000000000000000
  00000000000000000000
						   BORDER_DISTANCE
***********************									Border						
						   TEXT_TO_PICTURE_DISTANCE
----------------------	   TEXT_BORDER_DISTANCE
  TTTTTTTTTTTTTTTTTTTT									Text
----------------------		TEXT_BORDER_DISTANCE				
						
*/

#define TEXT_TO_PICTURE_DISTANCE	10
#define ITEM_DISTANCE				15

/////////////////////////////////////////////////////////////////////////////
// CGalleryView window

class CGalleryView :	public CViewBase, 
						public CDBaseListenerImpl,
						public CFlterViewImpl
{	
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CGalleryView)	
	PROVIDE_GUID_INFO( )

	//CScrollZoomSiteImpl

// Construction
public:
	CGalleryView();

// Attributes
public:

// Operations
public:
	void OnRecordsetChange();

	sptrISelectQuery GetActiveQuery();

public:	
	virtual void OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGalleryView)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGalleryView();
	DWORD GetMatchType( const char *szType );

//print functions
	virtual bool GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX ); 
	virtual bool GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY );
	virtual void Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags );

	// Generated message map functions
protected:
	//{{AFX_MSG(CGalleryView)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CGalleryView)
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CGalleryView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()


//Scroll size
protected:
	CSize m_sizeScroll;			 
	void SetScrollSize( CSize size );
	CSize GetScrollSize( ){ return m_sizeScroll;}
	void CalcScrollSize();

//Item dimension
protected:
	CSize m_sizeItem; 
	CSize m_sizePicture;
	int m_nItemCount;	
public:
	void SetPictureSize( int nX, int nY, bool bForceRepain = true  );
	CSize GetPictureSize(){ return m_sizePicture;}
	CSize GetItemSize( ){ return m_sizeItem;};

	int GetHorzItemCount( );
	int GetVertItemCount( );

	void SetItemCount( int nCount, bool bForceRepain = true );	
	int GetItemCount(){ return m_nItemCount;}

//helper functions
public:
	int GetItemNumberFromPoint( CPoint pt );
	
	int GetNextVisibleItem( CRect rect, int nPrevIndex );

	int ConvertToLinearIndex( int nHorzPos, int nVertPos );
	void ConvertToHorzVertIndex( int nLinearIndex, int& nHorzPos, int& nVertPos );
	

	CRect GetItemRect( int nIndex );
	bool IsPermissibleIndex( int nIndex );
	virtual DWORD OnGetViewFlags() {return View_AutoActivateObjects;}

//Selection
protected:
	int m_nSelectedItem;
public:
	void SetSelectedItem( int nSelectedItem, bool bForceRepaint = true );
	int GetSelectedItem();

	long GetRecordCount();
	

//Print calculation
	int GetRowCount();
	int GetColumnCount();

	//int GetColumnToPrint( int nColumn, int& nReturnWidth );
	//int GetRowToPrint( int nRow, int& nReturnHeight );

protected:
	BEGIN_INTERFACE_PART(GalleryVw, IDBGalleryView)
		com_call Stub();
	END_INTERFACE_PART(GalleryVw);


	BEGIN_INTERFACE_PART(ScrollZoomView, IScrollZoomView)
		com_call SetZoom( double fZoom, BOOL* pbWasProcess, double* pReturnZoom );
		com_call SetScrollPos( POINT ptPos,	BOOL* pbWasProcess, POINT* pptReturnPos );
		com_call SetClientSize( SIZE sizeNew, BOOL* pbWasProcess, SIZE* pReturnsizeNew );
	END_INTERFACE_PART(ScrollZoomView);

	BEGIN_INTERFACE_PART(ClipboardProvider, IClipboardProvider)
		com_call GetActiveObject( IUnknown** ppunkObject );
	END_INTERFACE_PART(ClipboardProvider);
protected:
	IDocument* m_pIDocument;

	bool CreateFont( CFont& font );
	int m_nFontHeight;
public:

public:


//Drawing
	void DrawItem( int nIndex, CDC* pDC, bool bSelected, CString strCaption, void* pTumbnail, bool bPrinting = false );

//Helper func for drawing
	CSize GetBorderSize();	
	CSize GetTextSize();	


protected:
	COLORREF	m_colorBk;
	
	COLORREF	m_colorItemBk;
	COLORREF	m_colorText;
	COLORREF	m_colorTextBk;

	COLORREF	m_colorItemBkSelection;
	COLORREF	m_colorTextSelection;
	COLORREF	m_colorTextBkSelection;

	void InitColor();


	CRect ConvertToWindow( CRect rect );
	CRect ConvertToClient( CRect rect );		
	CPoint ConvertToClient( CPoint point );
	CPoint GetScrollPos( );


	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );
//Filter Support
protected:
	virtual bool GetActiveField( CString& strTable, CString& strField );
	virtual bool GetActiveFieldValue( CString& strTable, CString& strField, CString& strValue );

//Filter property page
public:
	virtual void OnActivateView( bool bActivate, IUnknown *punkOtherView );
	bool ShowFilterPropPage( bool bShow );

	void ScrollToCurrentRecord();

protected:
	bool GotoRecordSlow( long lNewCurrentRecord );
	long m_lCurrentTimerRecord;
	bool m_bTimerActivate;

	UINT m_nTimerID;
	void StartTimer();
	void StopTimer();
	long m_lRecordStartTickCount;

	long GetCurrentRecord();

	bool ForceGoToRecord( );

protected:
	//virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, const _variant_t var );

	bool ActivateGalleryObjectInContext();

	CString FindMatchViewProgID( IUnknown* punkObj );
	bool ChangeViewType( IUnknown* punkObj );

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GALLERYVIEW_H__A39A558F_CAB6_4D0E_A1C3_B1848DC0ED62__INCLUDED_)

#if !defined(AFX_DBCONTROL_H__7471F689_2720_47C7_8B09_34AD33E9E6CB__INCLUDED_)
#define AFX_DBCONTROL_H__7471F689_2720_47C7_8B09_34AD33E9E6CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBControl.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDBControlImpl			   

#include "docview5.h"
#include <msstkppg.h>


class CDBControlImpl : public CImplBase
{
protected:

	CDBControlImpl();
	~CDBControlImpl();

	CString m_strTableName;
	CString m_strFieldName;
	BOOL m_bEnableEnumeration;
	BOOL m_bAutoLabel;
	short m_nLabelPosition;
	BOOL m_bReadOnly;

	void InitDefaults();

	BOOL IDBControl_Serialize( CArchive &ar );

	sptrIDBaseDocument m_spDBaseDocument;

	bool m_bWasRegister;

	IUnknown* m_punkControl;

	//For setWindowText
	bool m_bCanRecieveNotify;

	//View Mode
	BlankViewMode m_BlankViewMode;	
	ViewMode m_ReportViewMode;

	void DrawBorder( CDC* pdc, CRect rcBorder, bool bFocused );
public:
	BlankViewMode GetBlankViewMode(){ return m_BlankViewMode;}
	ViewMode GetReportViewMode(){ return m_ReportViewMode;}


	//sptrIBlankView GetBlankView( CWnd* pCtrlWnd );

	virtual void RefreshAppearance() = 0;


protected:
	sptrISelectQuery GetActiveQuery();

	CFont m_Font;

	bool IsActiveDBaseField();
	void SetAciveDBaseField();
	int  GetFieldIndexInQuery();

	virtual void _RepaintCtrl() = 0;


protected:
	BEGIN_INTERFACE_PART(DBControl, IDBControl2)
		// IDBControl 
		com_call GetTableName( BSTR* pbstrTableName );
		com_call SetTableName( BSTR   bstrTableName );

		com_call GetFieldName( BSTR* pbstrFieldName );
		com_call SetFieldName( BSTR   bstrFieldName );

		com_call GetEnableEnumeration( BOOL* pbEnable );
		com_call SetEnableEnumeration( BOOL   bEnable );

		com_call GetAutoLabel( BOOL* pbAutoLabel );
		com_call SetAutoLabel( BOOL   bAutoLabel );

		com_call GetValue( tagVARIANT *pvar );
		com_call SetValue( const tagVARIANT var );

		com_call GetLabelPosition( short* pnLabelLeft );
		com_call SetLabelPosition( short  nLabelLeft );
		
		com_call GetDBaseDocument( IUnknown** ppunkDBaseDocument );

		com_call ChangeValue( const tagVARIANT var );

		com_call SetBlankViewMode( short oldBlankViewMode, short newBlankMode );
		com_call SetReportViewMode( short oldReportViewMode, short newReportMode );

		// IDBControl2
		com_call GetReadOnly( BOOL* pbReadOnly );
		com_call SetReadOnly( BOOL	bReadOnly );

	END_INTERFACE_PART(DBControl)

	BEGIN_INTERFACE_PART(DBaseListener, IDBaseListener)
		com_call OnNotify( BSTR bstrEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, tagVARIANT var );
	END_INTERFACE_PART(DBaseListener)

protected:
	virtual CString IDBControl_GetTableName( );
	virtual void IDBControl_SetTableName( CString strTableName );
	virtual void IDBControl_OnSetTableName(  ) = 0;

	virtual CString IDBControl_GetFieldName( );
	virtual void IDBControl_SetFieldName( CString strFieldName );
	virtual void IDBControl_OnSetFieldName(  ) = 0;

	virtual BOOL IDBControl_GetEnableEnumeration( );
	virtual void IDBControl_SetEnableEnumeration( BOOL bEnable );
	virtual void IDBControl_OnSetEnableEnumeration( ) = 0;

	virtual BOOL IDBControl_GetAutoLabel( );
	virtual void IDBControl_SetAutoLabel( BOOL bEnable );
	virtual void IDBControl_OnSetAutoLabel( ) = 0;

	virtual void IDBControl_BuildAppearanceAfterLoad( ) = 0;

	virtual void IDBControl_GetValue( tagVARIANT *pvar ) = 0;
	virtual void IDBControl_SetValue( const tagVARIANT var ) = 0;

	virtual short IDBControl_GetLabelPosition( );
	virtual void IDBControl_SetLabelPosition( short nLabelPosition );
	virtual void IDBControl_OnSetLabelPosition( ) = 0;

	virtual void IDBControl_ChangeValue( const tagVARIANT var ) = 0;

	virtual void OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var ) = 0;

	virtual void IDBControl_OnChangeBlankViewMode( BlankViewMode oldBlankViewMode, BlankViewMode newBlankMode );
	virtual void IDBControl_OnChangeReportViewMode( ViewMode oldReportViewMode, ViewMode newReportMode );

	virtual BOOL IDBControl2_GetReadOnly( );
	virtual void IDBControl2_SetReadOnly( BOOL bReadOnly );
	virtual void IDBControl2_OnSetReadOnly( ) = 0;

	virtual void SetEnableWindow( bool bEnable ) = 0;
	virtual void EnterViewMode( BlankViewMode newBlankMode ) = 0;

	virtual void RegisterDB();
	virtual void UnRegisterDB();	

//Helper
	CString GetCaption();

	_bstr_t	m_bstr_def_font;
	void	init_default_font( FONTDESC* pdescr, const char* psz_def_section = 0 );

};

void operator<<(CArchive &ar, FONTDESC &X);
void operator>>(CArchive &ar, FONTDESC &X);


#define CORRECT_MFC_OnSetObjectRects(class_name)								\
BOOL class_name::OnSetObjectRects(LPCRECT lprcPosRect, LPCRECT lprcClipRect)	\
{																				\
	ASSERT(lprcPosRect != NULL);												\
																				\
	m_rcPos = *lprcPosRect;														\
																				\
	CRect rectPos = m_rcPos;													\
	if (m_bUIActive && m_pRectTracker != NULL)									\
	{																			\
		if (lprcClipRect != NULL)												\
			m_pRectTracker->m_rectClip = *lprcClipRect;							\
																				\
		CRect rectTmp = rectPos;												\
		rectTmp.OffsetRect(-rectTmp.left, -rectTmp.top);						\
		m_pRectTracker->m_rect = rectTmp;										\
																				\
		UINT nHandleSize = m_pRectTracker->m_nHandleSize - 1;					\
		rectPos.InflateRect(nHandleSize, nHandleSize);							\
	}																			\
																				\
	CRect rectClip = *lprcPosRect;												\
	m_ptOffset.x = rectClip.left;												\
	m_ptOffset.y = rectClip.top;												\
																				\
	if (!m_bInPlaceSiteWndless)													\
	{																			\
		CWnd* pWndOuter = GetOuterWindow();										\
		if (pWndOuter != NULL)													\
			::MoveWindow(pWndOuter->m_hWnd, rectClip.left, rectClip.top,		\
				rectClip.Width(), rectClip.Height(), TRUE);						\
		if (pWndOuter != this)													\
			MoveWindow(m_ptOffset.x, m_ptOffset.y, rectPos.Width(), rectPos.Height());	 \
	}																					 \
																						 \
	return TRUE;																		 \
}

inline bool CorrectHimetricsRect( CRect* prc, CRect* prc_max )
{
	if( !prc || !prc_max )	return false;
	if( prc->left < prc_max->left )		prc->left = prc_max->left;
	if( prc->right > prc_max->right )	prc->right = prc_max->right;
	if( prc->bottom < prc_max->top )	prc->bottom = prc_max->top;
	if( prc->top > prc_max->bottom )	prc->top = prc_max->bottom;	
	return true;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBCONTROL_H__7471F689_2720_47C7_8B09_34AD33E9E6CB__INCLUDED_)

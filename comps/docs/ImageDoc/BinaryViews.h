#if !defined(AFX_BINARYVIEWS_H__6B9F9804_2AE9_49A7_95A3_EFA9B00FD545__INCLUDED_)
#define AFX_BINARYVIEWS_H__6B9F9804_2AE9_49A7_95A3_EFA9B00FD545__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BinaryViews.h : header file
//


#include "ImageSourceView.h"

#undef SupportLayers
/////////////////////////////////////////////////////////////////////////////
// CBinaryViewBase command target

class CBinaryViewBase : public CImageSourceView
{
public:
	CBinaryViewBase();
	~CBinaryViewBase();

	virtual void DrawLayers(CDC* pdc, BITMAPINFOHEADER* pbmih, byte* pdibBits, CRect rectPaint, bool bUseScrollZoom);
	virtual IUnknown* __GetObjectByPoint( CPoint point);
	virtual void	_AttachObjects( IUnknown *punkContext );
	virtual DWORD GetMatchType( const char *szType );
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	virtual CString get_section( ) { return "\\Binary"; }
protected:
	bool _get_center_mode() { return false; };
	CTypedPtrList<CPtrList, IUnknown*>m_listBinaries;

	POSITION	m_posActive;

	virtual bool	is_have_print_data();
	virtual void CalcClientSize();
};


class CBinaryForeView : public CBinaryViewBase
{
	DECLARE_DYNCREATE(CBinaryForeView);
	GUARD_DECLARE_OLECREATE(CBinaryForeView);
	PROVIDE_GUID_INFO( )
public:
	CBinaryForeView();

public:
	virtual const char* GetViewMenuName();

};

class CBinaryBackView : public CBinaryViewBase
{
	DECLARE_DYNCREATE(CBinaryBackView);
	GUARD_DECLARE_OLECREATE(CBinaryBackView);
	PROVIDE_GUID_INFO( )
public:
	CBinaryBackView();

public:
	virtual const char* GetViewMenuName();

};

class CBinaryView : public CBinaryViewBase
{
	DECLARE_DYNCREATE(CBinaryView);
	GUARD_DECLARE_OLECREATE(CBinaryView);
	PROVIDE_GUID_INFO( )
public:
	CBinaryView();

public:
	virtual const char* GetViewMenuName();
	virtual DWORD GetMatchType( const char *szType );
};

class CBinaryContourView : public CBinaryViewBase
{
	DECLARE_DYNCREATE(CBinaryContourView);
	GUARD_DECLARE_OLECREATE(CBinaryContourView);
	PROVIDE_GUID_INFO( )
public:
	CBinaryContourView();

public:
	virtual const char* GetViewMenuName();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BINARYVIEWS_H__6B9F9804_2AE9_49A7_95A3_EFA9B00FD545__INCLUDED_)

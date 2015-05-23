#if !defined(AFX_MEASVIEW_H__51B4E2FC_10BC_4C01_8282_5AB04B46B491__INCLUDED_)
#define AFX_MEASVIEW_H__51B4E2FC_10BC_4C01_8282_5AB04B46B491__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeasView.h : header file
//

#include "GridView.h"
#include "Utils.h"
#include "Classes5.h"
#include "VTGridCtrl.h"
#include "menuconst.h"
#include "PropBagImpl.h"
#include "\vt5\common2\class_cache.h"

enum MeasViewFlags
{
	mvfStretchSmallImages = 1 << 0,
};

/////////////////////////////////////////////////////////////////////////////
// CMeasView command target

class CMeasView;
class CVTImageCell;

class CVTImageCell : public CVTGridCell
{
	friend class CVTGridCtrl;

    DECLARE_DYNCREATE(CVTImageCell)
public:
	CVTImageCell();
	virtual ~CVTImageCell();
public:
	bool	HasImage()	{return m_image != 0;}

	virtual void SetObject( IUnknown *punkObject );
    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
	static bool DrawImage( HDC hDC, IImage2 *pimage, const RECT rect, COLORREF crBack, COLORREF crText,
			BOOL bForceStretch );	// [vanek]: BT610 - 19.11.2003
protected:
	IImage2Ptr	m_image;	

public:
	IImage2Ptr	GetRawImage(){ return m_image; }
};




class CMeasView : public CGridViewBase, public CNamedPropBagImpl
{
	DECLARE_DYNCREATE(CMeasView)
	GUARD_DECLARE_OLECREATE(CMeasView)
	DECLARE_INTERFACE_MAP()
	ENABLE_MULTYINTERFACE();
	PROVIDE_GUID_INFO( )

	friend class CVTImageCell;
public:
	CMeasView();           // protected constructor used by dynamic creation
	virtual ~CMeasView();

	virtual LPUNKNOWN GetInterfaceHook(const void* p)
	{
		if (*(IID*)p == IID_INamedPropBag || *(IID*)p == IID_INamedPropBagSer)
			return (INamedPropBagSer*)this;
		else return __super::GetInterfaceHook(p);
	}

// Operations
public:
	virtual int	rowMeasureUnit()	const {return 1;}
	virtual int	rowFirstObject()	const {return rowMeasureUnit()+1;}

	virtual int	colNumber()			const {return 0;}
	virtual int	colClass()			const {return m_bShowClass?m_bShowClass:-1;}
	virtual int	colImage()			const {return m_bShowImage?m_bShowClass+m_bShowImage:-1;}
	virtual int	colFirstParameter()	const {return m_bShowClass+m_bShowImage+1;}
	
    virtual DWORD GetMatchType( const char *szType );
	virtual COLORREF GetCellBackColor(int nRow, int nCol);
	void AttachActiveObjects();

	virtual	DWORD GetViewFlags();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeasView)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BEGIN_INTERFACE_PART(MeasView, IMeasureView)
		com_call GetObjectList(IUnknown ** ppunkList);
		com_call GetActiveObject(IUnknown ** ppunkObject);
		com_call SetActiveObject(IUnknown * punkObject);
		com_call GetActiveParam(long * plParamKey);
		com_call SetActiveParam(long lParamKey);
		com_call AlignByImage();
		com_call CopyToClipboard( DWORD dwFlags );
	END_INTERFACE_PART(MeasView)

	BEGIN_INTERFACE_PART(MenuInitializer, IMenuInitializer)
		com_call OnInitPopup( BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu );
	END_INTERFACE_PART(MenuInitializer)
	
	virtual bool DefaultInit();

	virtual LPCTSTR	GetObjectType();

	bool FillClassArray(CStringArray & sa);

	// Generated message map functions
	//{{AFX_MSG(CMeasView)
	//}}AFX_MSG

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	DECLARE_MESSAGE_MAP()

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	virtual void OnActivateView( bool bActivate, IUnknown *punkOtherView );
	virtual bool OnGetDispInfoRow( long row );

	// [vanek] SBT: 864 - 26.03.2004
	virtual bool DefineParameter( ParameterContainer *pc, long col );

	virtual bool LoadState();
	virtual const char *GetMenuName()	{return szMeasurementMenuUN;};
	virtual CGridCellBase* CreateCell(int nRow, int nCol);
	virtual void StoreColumnWidth( long col, long width );

	IUnknown *_object_from_cell( long row, long &col, bool bChParam = false );

protected:
	bool	m_bShowClass, m_bShowImage,
			m_bStretchSmallImages,	// [vanek]: BT610 - 19.11.2003
			m_bEditableGrid,		// [vanek] SBT:794 - 02.03.2004
			// [vanek] SBT: 864 - 26.03.2004
			m_bEditableStandartParams,
			m_bEditableUniqueParams;
	CClassCache m_ClassCache;
			
	
private:
	bool m_bShowUnknown;
	int m_iSkip;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEASVIEW_H__51B4E2FC_10BC_4C01_8282_5AB04B46B491__INCLUDED_)

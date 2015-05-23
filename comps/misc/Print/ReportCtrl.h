// ReportCtrl.h: interface for the CReportCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTCTRL_H__C0655455_9A77_45C4_98FB_D42C4248F9E3__INCLUDED_)
#define AFX_REPORTCTRL_H__C0655455_9A77_45C4_98FB_D42C4248F9E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\..\\misc\\EditorView\\axctrlbase.h"


class CReportCtrl : public CAXCtrlBase
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CReportCtrl);
	GUARD_DECLARE_OLECREATE(CReportCtrl);

	DECLARE_INTERFACE_MAP();

public:
	CReportCtrl();
	~CReportCtrl();
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );

protected:
	BEGIN_INTERFACE_PART(ReportCtrl, IReportCtrl)
		com_call IsViewCtrl( BOOL* bResult );
		com_call GetDesignOwnerPage(int* pnPage);
		com_call SetDesignOwnerPage(int nPage);
		com_call GetDesignPosition( RECT *pRect );
		com_call SetDesignPosition( RECT Rect );
		
		com_call GetDesignColRow( int* nCol, int* nRow );
		com_call SetDesignColRow( int nCol, int nRow );

		com_call GetPrevFormPosition( RECT *pRect );
		com_call SetPrevFormPosition( RECT Rect );


		com_call GetPreviewColRow( int* nCol, int* nRow );
		com_call SetPreviewColRow( int nCol, int nRow );

		com_call GetPreviewPosition( RECT *pRect );
		com_call SetPreviewPosition( RECT Rect );

		com_call GetHorzScale( BOOL *pbHorzScale );
		com_call SetHorzScale( BOOL bHorzScale  );

		com_call GetVertScale( BOOL *pbVertScale );
		com_call SetVertScale( BOOL bVertScale   );

		com_call GetDPI( double *pfDPI );
		com_call SetDPI( double fDPI   );

		com_call GetZoom( double *pfZoom );
		com_call SetZoom( double fZoom   );

		com_call GetFlag( DWORD *pdwFlag );
		com_call SetFlag( DWORD   dwFlag );

		com_call GetCloneObject( BOOL *pbCloneObject );
		com_call SetCloneObject( BOOL bCloneObject );


	END_INTERFACE_PART(ReportCtrl)
protected:
	int m_nDesignOwnerPage;
	CRect m_designPosition;	

	CRect m_prevFormPosition;
	int m_nDesignCol;
	int m_nDesignRow;

	int m_nPreviewCol;
	int m_nPreviewRow;
	CRect m_previewPosition;

	DWORD m_dwFlag;

	BOOL m_bHorzScale, m_bVertScale;
	double m_fZoom, m_fDPI;	
	BOOL m_bCloneObject;

};


class CReportFrom : public CAXFormBase
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CReportFrom);
	GUARD_DECLARE_OLECREATE(CReportFrom);

	DECLARE_INTERFACE_MAP();

public:
	CReportFrom();
	~CReportFrom();

public:
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );
	virtual bool CanBeBaseObject();

protected:
	BEGIN_INTERFACE_PART(ReportForm, IReportForm)
		com_call GetPaperName( BSTR *pbstrPaperName );
		com_call SetPaperName( BSTR bstrPaperName );

		com_call GetPaperOrientation(int* pnPaperOriantation);
		com_call SetPaperOrientation(int nPaperOriantation);

		com_call GetPaperSize( SIZE* pSize );
		com_call SetPaperSize( SIZE size );

		com_call GetPaperField( RECT *pRect );
		com_call SetPaperField( RECT rect );	
			
		com_call GetPaperAlign( RECT *pRect );
		com_call SetPaperAlign( RECT rect );	

		com_call GetMeasurementSystem(int* pnSystem);
		com_call SetMeasurementSystem(int nSystem);	

		com_call GetDesignCurentPage(int* pnCurPage);
		com_call SetDesignCurentPage(int nCurPage);

		com_call GetDesignPageCount(int* pnPageCount);
		com_call SetDesignPageCount(int nPageCount);

		com_call GetMode(int* nMode);

		com_call GetSimpleMode(BOOL* pbSimpleMode);
		com_call SetSimpleMode(BOOL  bSimpleMode);	

		com_call GetUsePageNum(BOOL* pbUsePageNum);
		com_call SetUsePageNum(BOOL  bUsePageNum);	

		com_call GetVertPageNumPos(int* pnVertPos);
		com_call SetVertPageNumPos(int  nVertPos);	

		com_call GetHorzPageNumPos(int* pnHorzPos);
		com_call SetHorzPageNumPos(int  nHorzPos);	

		com_call GetPageNumFont(LOGFONT* pLFONT);
		com_call SetPageNumFont(LOGFONT  LFONT);	

		com_call SetMode(int nMode );

		com_call GetBuildMode(int* pnMode);
		com_call SetBuildMode(int  nMode);	


	END_INTERFACE_PART(ReportForm)

protected:
	CString m_strPaperName;
	int	m_nOrientation;
	CSize m_paperSize;
	CRect m_paperField;
	CRect m_paperAlign;
	int m_nMeasurementSystem;


	int m_nDesignPageCount;
	int m_nDesignCurPage;

	double m_fDesignZoomFactor;

	BOOL m_bSimpleMode;

	BOOL m_bUsePageNum;

	int m_VertPageNumPos;
	int m_HorzPageNumPos;

	LOGFONT m_lfPageNum;

	int m_nMode;

	int m_nBuildMode;

};


class CReportFileFilter : public CFileFilterBase  
{
	DECLARE_DYNCREATE(CReportFileFilter);
	GUARD_DECLARE_OLECREATE(CReportFileFilter);
public:
	CReportFileFilter();
	virtual ~CReportFileFilter();

	virtual bool InitNew(){return true;}
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );

	virtual bool BeforeReadFile();
	virtual bool AfterReadFile();
};


#endif // !defined(AFX_REPORTCTRL_H__C0655455_9A77_45C4_98FB_D42C4248F9E3__INCLUDED_)

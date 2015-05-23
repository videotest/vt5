// ReportCtrl.cpp: implementation of the CReportCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "print.h"
#include "ReportCtrl.h"

#include "afxpriv2.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// {E9294ADE-C3AB-408e-BF37-59AE7E210ED4}
GUARD_IMPLEMENT_OLECREATE(CReportCtrl, "Report.ReportCtrl", 
0xe9294ade, 0xc3ab, 0x408e, 0xbf, 0x37, 0x59, 0xae, 0x7e, 0x21, 0xe, 0xd4);

// {A93CD2C0-FD52-4bdd-9039-F0322CDD10CC}
GUARD_IMPLEMENT_OLECREATE(CReportFrom, "Report.ReportForm", 
0xa93cd2c0, 0xfd52, 0x4bdd, 0x90, 0x39, 0xf0, 0x32, 0x2c, 0xdd, 0x10, 0xcc);

// {7CE64735-1B3C-4413-A219-D8B869317053}
static const GUID clsidReportCtrlDataObjectInfo = 
{ 0x7ce64735, 0x1b3c, 0x4413, { 0xa2, 0x19, 0xd8, 0xb8, 0x69, 0x31, 0x70, 0x53 } };

// {BEC3B9AF-26D3-4243-BA46-9C5D964A0E7A}
static const GUID clsidReportFormDataObjectInfo = 
{ 0xbec3b9af, 0x26d3, 0x4243, { 0xba, 0x46, 0x9c, 0x5d, 0x96, 0x4a, 0xe, 0x7a } };

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@




DATA_OBJECT_INFO_FULL(IDS_REPORTCTRL_TYPE, CReportCtrl, szTypeReportCtrl, szTypeReportForm, clsidReportCtrlDataObjectInfo,IDI_THUMBNAIL_ICON_REPORT)
DATA_OBJECT_INFO(IDS_REPORTFORM_TYPE, CReportFrom, szTypeReportForm, clsidReportFormDataObjectInfo,IDI_THUMBNAIL_ICON_REPORT)

IMPLEMENT_DYNCREATE(CReportCtrl, CAXCtrlBase);
IMPLEMENT_DYNCREATE(CReportFrom, CAXFormBase);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
BEGIN_INTERFACE_MAP(CReportCtrl, CAXCtrlBase)
	INTERFACE_PART(CReportCtrl, IID_IReportCtrl, ReportCtrl)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CReportCtrl, ReportCtrl)


BEGIN_INTERFACE_MAP(CReportFrom, CAXFormBase)
	INTERFACE_PART(CReportFrom, IID_IReportForm, ReportForm)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CReportFrom, ReportForm)
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@








////////////////////////////////////////////////////////////////////////////////////////////////////
CReportCtrl::CReportCtrl()
{
	m_nDesignOwnerPage = -1;
	m_designPosition = NORECT;
	
	m_prevFormPosition	= NORECT;
	m_nDesignCol = -1;
	m_nDesignRow = -1;

	m_previewPosition	= NORECT;
	m_nPreviewCol = -1;
	m_nPreviewRow = -1;

	m_bHorzScale = FALSE;
	m_bVertScale = FALSE;
	m_fZoom = 100.0;
	m_fDPI  = 100.0;


	m_dwFlag	= 0;

	m_bCloneObject = FALSE;

}

CReportCtrl::~CReportCtrl()
{

}

bool CReportCtrl::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{	

	CAXCtrlBase::SerializeObject( ar, pparams );

	long	lVersion = 7;

	if( ar.IsStoring() )
	{
		ar<<lVersion;
		ar<<m_nDesignOwnerPage;
		ar<<m_designPosition;
		ar<<m_prevFormPosition;
		ar<<m_nDesignCol;
		ar<<m_nDesignRow;
		ar<<m_previewPosition;
		ar<<m_nPreviewCol;
		ar<<m_nPreviewRow;
		ar<<m_bHorzScale;
		ar<<m_bVertScale;
		ar<<m_fZoom;
		ar<<m_fDPI;
		ar<<m_bCloneObject;
	}
	else
	{
		ar>>lVersion;
		if( lVersion >= 2 )
			ar>>m_nDesignOwnerPage;
		if( lVersion >= 3 )
			ar>>m_designPosition;
		if( lVersion >= 4 )
		{
			ar>>m_prevFormPosition;
			ar>>m_nDesignCol;
			ar>>m_nDesignRow;
		}

		if( lVersion >= 5 )
		{
			ar>>m_previewPosition;
			ar>>m_nPreviewCol;
			ar>>m_nPreviewRow;
		}

		if( lVersion >= 6 )
		{
			ar>>m_bHorzScale;
			ar>>m_bVertScale;
			ar>>m_fZoom;
			ar>>m_fDPI;
		}

		if( lVersion >= 7 )
			ar>>m_bCloneObject;

	}

	return true;

}

HRESULT CReportCtrl::XReportCtrl::IsViewCtrl( BOOL* bResult )
{
	_try_nested( CReportCtrl, ReportCtrl, IsViewCtrl )
	{
		*bResult = TRUE;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportCtrl::XReportCtrl::GetDesignOwnerPage(int* pnPage)
{
	_try_nested( CReportCtrl, ReportCtrl, GetDesignOwnerPage )
	{
		*pnPage = pThis->m_nDesignOwnerPage;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportCtrl::XReportCtrl::SetDesignOwnerPage(int nPage)
{
	_try_nested( CReportCtrl, ReportCtrl, SetDesignOwnerPage )
	{
		pThis->m_nDesignOwnerPage = nPage;
		return S_OK;
	}
	_catch_nested;
}
								

HRESULT CReportCtrl::XReportCtrl::GetDesignPosition( RECT *pRect )
{
	_try_nested( CReportCtrl, ReportCtrl, GetDesignPosition )
	{	
		pRect->left		= pThis->m_designPosition.left;
		pRect->top		= pThis->m_designPosition.top;
		pRect->right	= pThis->m_designPosition.right;
		pRect->bottom	= pThis->m_designPosition.bottom;

		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportCtrl::XReportCtrl::SetDesignPosition( RECT rect )
{
	_try_nested( CReportCtrl, ReportCtrl, SetDesignPosition )
	{	
		pThis->m_designPosition = rect;

		if( pThis->m_designPosition.Width() < 100 )
			pThis->m_designPosition.right = pThis->m_designPosition.left + 100;

		if( pThis->m_designPosition.Height() < 100 )
			pThis->m_designPosition.bottom = pThis->m_designPosition.top + 100;

		return S_OK;
	}
	_catch_nested;
}


HRESULT CReportCtrl::XReportCtrl::SetDesignColRow( int nCol, int nRow )
{
	_try_nested( CReportCtrl, ReportCtrl, SetDesignColRow )
	{		
		pThis->m_nDesignCol = nCol;
		pThis->m_nDesignRow = nRow;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportCtrl::XReportCtrl::GetDesignColRow( int* nCol, int* nRow )
{
	_try_nested( CReportCtrl, ReportCtrl, GetDesignColRow )
	{		
		*nCol = pThis->m_nDesignCol;
		*nRow = pThis->m_nDesignRow;
		return S_OK;
	}
	_catch_nested;
}


HRESULT CReportCtrl::XReportCtrl::GetPrevFormPosition( RECT *pRect )
{
	_try_nested( CReportCtrl, ReportCtrl, GetPrevFormPosition )
	{	
		pRect->left		= pThis->m_prevFormPosition.left;
		pRect->top		= pThis->m_prevFormPosition.top;
		pRect->right	= pThis->m_prevFormPosition.right;
		pRect->bottom	= pThis->m_prevFormPosition.bottom;
		
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportCtrl::XReportCtrl::SetPrevFormPosition( RECT rect )
{
	_try_nested( CReportCtrl, ReportCtrl, SetPrevFormPosition )
	{	
		pThis->m_prevFormPosition = rect;

		return S_OK;
	}
	_catch_nested;
}



//////////
HRESULT CReportCtrl::XReportCtrl::GetPreviewPosition( RECT *pRect )
{
	_try_nested( CReportCtrl, ReportCtrl, GetPreviewPosition )
	{	
		pRect->left		= pThis->m_previewPosition.left;
		pRect->top		= pThis->m_previewPosition.top;
		pRect->right	= pThis->m_previewPosition.right;
		pRect->bottom	= pThis->m_previewPosition.bottom;

		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportCtrl::XReportCtrl::SetPreviewPosition( RECT rect )
{
	_try_nested( CReportCtrl, ReportCtrl, SetPreviewPosition )
	{		
		pThis->m_previewPosition = rect;

		if( pThis->m_previewPosition.Width() < 100 )
			pThis->m_previewPosition.right = pThis->m_previewPosition.left + 100;

		if( pThis->m_previewPosition.Height() < 100 )
			pThis->m_previewPosition.bottom = pThis->m_previewPosition.top + 100;



		return S_OK;
	}
	_catch_nested;
}


HRESULT CReportCtrl::XReportCtrl::SetPreviewColRow( int nCol, int nRow )
{
	_try_nested( CReportCtrl, ReportCtrl, SetPreviewColRow )
	{		
		pThis->m_nPreviewCol = nCol;
		pThis->m_nPreviewRow = nRow;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportCtrl::XReportCtrl::GetPreviewColRow( int* nCol, int* nRow )
{
	_try_nested( CReportCtrl, ReportCtrl, GetPreviewColRow )
	{		
		*nCol = pThis->m_nPreviewCol;
		*nRow = pThis->m_nPreviewRow;
		return S_OK;
	}
	_catch_nested;
}




///////////
HRESULT CReportCtrl::XReportCtrl::SetHorzScale( BOOL bHorzScale  )
{
	_try_nested( CReportCtrl, ReportCtrl, SetHorzScale )
	{		
		pThis->m_bHorzScale = bHorzScale;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportCtrl::XReportCtrl::GetHorzScale( BOOL *pbHorzScale )
{
	_try_nested( CReportCtrl, ReportCtrl, GetHorzScale )
	{		
		*pbHorzScale = pThis->m_bHorzScale;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportCtrl::XReportCtrl::SetVertScale( BOOL bVertScale   )
{
	_try_nested( CReportCtrl, ReportCtrl, SetVertScale )
	{		
		pThis->m_bVertScale = bVertScale;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportCtrl::XReportCtrl::GetVertScale( BOOL *pbVertScale )
{
	_try_nested( CReportCtrl, ReportCtrl, GetVertScale )
	{		
		*pbVertScale = pThis->m_bVertScale;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportCtrl::XReportCtrl::SetDPI( double fDPI   )
{
	_try_nested( CReportCtrl, ReportCtrl, SetDPI )
	{		
		pThis->m_fDPI = fDPI;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportCtrl::XReportCtrl::GetDPI( double *pfDPI )
{
	_try_nested( CReportCtrl, ReportCtrl, GetDPI )
	{		
		*pfDPI = pThis->m_fDPI;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportCtrl::XReportCtrl::SetZoom( double fZoom   )
{
	_try_nested( CReportCtrl, ReportCtrl, SetZoom )
	{		
		pThis->m_fZoom = fZoom;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportCtrl::XReportCtrl::GetZoom( double *pfZoom )
{
	_try_nested( CReportCtrl, ReportCtrl, GetZoom )
	{		
		*pfZoom = pThis->m_fZoom;
		return S_OK;
	}
	_catch_nested;
}

//////////

HRESULT CReportCtrl::XReportCtrl::SetFlag( DWORD   dwFlag )
{
	_try_nested( CReportCtrl, ReportCtrl, SetFlag )
	{		
		pThis->m_dwFlag = dwFlag;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportCtrl::XReportCtrl::GetFlag( DWORD *pdwFlag )
{
	_try_nested( CReportCtrl, ReportCtrl, GetFlag )
	{		
		*pdwFlag = pThis->m_dwFlag;
		return S_OK;
	}
	_catch_nested;
}

//////////
HRESULT CReportCtrl::XReportCtrl::GetCloneObject( BOOL *pbCloneObject )
{
	_try_nested( CReportCtrl, ReportCtrl, GetCloneObject )
	{		
		*pbCloneObject = pThis->m_bCloneObject;
		return S_OK;
	}
	_catch_nested;
}


HRESULT CReportCtrl::XReportCtrl::SetCloneObject( BOOL bCloneObject )
{
	_try_nested( CReportCtrl, ReportCtrl, SetCloneObject )
	{		
		pThis->m_bCloneObject = bCloneObject;
		return S_OK;
	}
	_catch_nested;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CReportFrom::CReportFrom()
{
	m_strPaperName	= "A4";
	m_nOrientation	= 0;
	m_paperSize		= CSize(210*100, 297*100);
	m_paperField		= CRect(20*100, 20*100, 20*100, 20*100);
	m_paperAlign		= CRect(5*100,5*100,5*100,5*100);
	
	m_nMeasurementSystem = 0;

	m_nDesignPageCount	= 0;
	m_nDesignCurPage	= 0;

	m_fDesignZoomFactor		= 1.0;

	m_bSimpleMode		= TRUE;

	m_bUsePageNum		= TRUE;

	m_VertPageNumPos	= 1;//Down
	m_HorzPageNumPos	= 1;//Center


    m_lfPageNum.lfHeight		= -12;
    m_lfPageNum.lfWidth			= 0;
    m_lfPageNum.lfEscapement	= 0;
    m_lfPageNum.lfOrientation	= 0;
    m_lfPageNum.lfWeight		= FW_THIN;
    m_lfPageNum.lfItalic		= 0;
    m_lfPageNum.lfUnderline		= 0;
    m_lfPageNum.lfStrikeOut		= 0;
    m_lfPageNum.lfCharSet		= DEFAULT_CHARSET;
    m_lfPageNum.lfOutPrecision	= OUT_DEFAULT_PRECIS;
    m_lfPageNum.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
    m_lfPageNum.lfQuality		= DEFAULT_QUALITY;
    m_lfPageNum.lfPitchAndFamily= DEFAULT_PITCH;
	strcpy( m_lfPageNum.lfFaceName, "Arial" );    	

	m_nMode = 0;

	m_nBuildMode = 0;

}

CReportFrom::~CReportFrom()
{

}

bool CReportFrom::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{	

	CAXFormBase::SerializeObject( ar, pparams );

	long	lVersion = 5;

	if( ar.IsStoring() )
	{
		ar<<lVersion;
		ar<<m_paperSize<<m_paperField<<m_paperAlign;
		ar<<m_strPaperName;
		ar<<m_nOrientation;
		ar<<m_nMeasurementSystem;
		ar<<m_nDesignPageCount;
		ar<<m_nDesignCurPage;
		ar<<m_fDesignZoomFactor;
		ar<<m_bSimpleMode;
		ar<<m_bUsePageNum;
		ar<<m_VertPageNumPos;
		ar<<m_HorzPageNumPos;
		ar.Write( &m_lfPageNum, sizeof(LOGFONT) );
		ar<<m_nMode;
		ar<<m_nBuildMode;
	}
	else
	{
		ar>>lVersion;
		ar>>m_paperSize>>m_paperField>>m_paperAlign;
		if( lVersion >= 2 )
		{
			ar>>m_strPaperName;
			ar>>m_nOrientation;
			ar>>m_nMeasurementSystem;
			ar>>m_nDesignPageCount;
			ar>>m_nDesignCurPage;
			ar>>m_fDesignZoomFactor;
		}
		if( lVersion >= 3 )
		{
			ar>>m_bSimpleMode;
			ar>>m_bUsePageNum;
			ar>>m_VertPageNumPos;
			ar>>m_HorzPageNumPos;
			ar.Read( &m_lfPageNum, sizeof(LOGFONT) );
		}

		if( lVersion >= 4 )
			ar>>m_nMode;

		if( lVersion >= 5 )
			ar>>m_nBuildMode;
		
	}

	return true;

}

bool CReportFrom::CanBeBaseObject()
{
	return true;
}


HRESULT CReportFrom::XReportForm::GetPaperName( BSTR *pbstrPaperName )
{
	_try_nested( CReportFrom, ReportForm, GetPaperName )
	{	
		*pbstrPaperName = pThis->m_strPaperName.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::SetPaperName( BSTR bstrPaperName )
{
	_try_nested( CReportFrom, ReportForm, SetPaperName )
	{	
		pThis->m_strPaperName = bstrPaperName;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::GetPaperOrientation(int* pnPaperOriantation)
{
	_try_nested( CReportFrom, ReportForm, GetPaperOrientation )
	{		
		*pnPaperOriantation = pThis->m_nOrientation;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportFrom::XReportForm::SetPaperOrientation(int nPaperOriantation)
{
	_try_nested( CReportFrom, ReportForm, SetPaperOrientation )
	{	
		pThis->m_nOrientation = nPaperOriantation;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::GetPaperSize( SIZE* pSize )
{
	_try_nested( CReportFrom, ReportForm, GetPageSize )
	{		
		pSize->cx = pThis->m_paperSize.cx;
		pSize->cy = pThis->m_paperSize.cy;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::SetPaperSize( SIZE size )
{
	_try_nested( CReportFrom, ReportForm, SetPaperSize )
	{	
		pThis->m_paperSize = size;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportFrom::XReportForm::GetPaperField( RECT *pRect )
{
	_try_nested( CReportFrom, ReportForm, GetPaperField )
	{	
		pRect->left		= pThis->m_paperField.left;
		pRect->top		= pThis->m_paperField.top;
		pRect->right	= pThis->m_paperField.right;
		pRect->bottom	= pThis->m_paperField.bottom;

		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportFrom::XReportForm::SetPaperField( RECT rect )
{
	_try_nested( CReportFrom, ReportForm, SetPaperField )
	{		
		pThis->m_paperField = rect;

		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportFrom::XReportForm::GetPaperAlign( RECT *pRect )
{
	_try_nested( CReportFrom, ReportForm, GetPaperAlign )
	{	
		pRect->left		= pThis->m_paperAlign.left;
		pRect->top		= pThis->m_paperAlign.top;
		pRect->right	= pThis->m_paperAlign.right;
		pRect->bottom	= pThis->m_paperAlign.bottom;
		
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportFrom::XReportForm::SetPaperAlign( RECT rect )
{
	_try_nested( CReportFrom, ReportForm, SetPaperAlign )
	{	
		pThis->m_paperAlign = rect;

		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportFrom::XReportForm::GetDesignCurentPage(int* nCurPage)
{
	_try_nested( CReportFrom, ReportForm, GetDesignCurentPage )
	{		
		*nCurPage = pThis->m_nDesignCurPage;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportFrom::XReportForm::SetDesignCurentPage(int nCurPage)
{
	_try_nested( CReportFrom, ReportForm, SetDesignCurentPage )
	{		
		pThis->m_nDesignCurPage = nCurPage;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportFrom::XReportForm::GetDesignPageCount(int* pnPageCount)
{
	_try_nested( CReportFrom, ReportForm, GetDesignPageCount )
	{		
		*pnPageCount = pThis->m_nDesignPageCount;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::SetDesignPageCount(int nPageCount)
{
	_try_nested( CReportFrom, ReportForm, SetDesignPageCount )
	{		
		pThis->m_nDesignPageCount = nPageCount;
		return S_OK;
	}
	_catch_nested;
}



HRESULT CReportFrom::XReportForm::SetMeasurementSystem(int nSystem)
{
	_try_nested( CReportFrom, ReportForm, SetMeasurementSystem )
	{		
		pThis->m_nMeasurementSystem = nSystem;
		return S_OK;
	}
	_catch_nested;
}
HRESULT CReportFrom::XReportForm::GetMeasurementSystem(int* pnSystem)
{
	_try_nested( CReportFrom, ReportForm, GetMeasurementSystem )
	{		
		*pnSystem = pThis->m_nMeasurementSystem;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::GetMode(int* nMode)
{
	_try_nested( CReportFrom, ReportForm, GetMode )
	{		
		*nMode = pThis->m_nMode;
		return S_OK;
	}
	_catch_nested;
}



HRESULT CReportFrom::XReportForm::GetSimpleMode(BOOL* pbSimpleMode)
{
	_try_nested( CReportFrom, ReportForm, GetSimpleMode )
	{	
		*pbSimpleMode = pThis->m_bSimpleMode;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::SetSimpleMode(BOOL  bSimpleMode)	
{
	_try_nested( CReportFrom, ReportForm, SetSimpleMode )
	{		
		pThis->m_bSimpleMode = bSimpleMode;
		return S_OK;
	}
	_catch_nested;
}




HRESULT CReportFrom::XReportForm::GetUsePageNum(BOOL* pbUsePageNum)
{
	_try_nested( CReportFrom, ReportForm, GetUsePageNum )
	{		
		*pbUsePageNum = pThis->m_bUsePageNum;		
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::SetUsePageNum(BOOL  bUsePageNum)	
{
	_try_nested( CReportFrom, ReportForm, SetUsePageNum )
	{		
		pThis->m_bUsePageNum = bUsePageNum;
		return S_OK;
	}
	_catch_nested;
}



HRESULT CReportFrom::XReportForm::GetVertPageNumPos(int* pnVertPos)
{
	_try_nested( CReportFrom, ReportForm, GetVertPageNumPos )
	{		
		*pnVertPos = pThis->m_VertPageNumPos;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::SetVertPageNumPos(int  nVertPos)	
{
	_try_nested( CReportFrom, ReportForm, SetVertPageNumPos )
	{		
		pThis->m_VertPageNumPos = nVertPos;
		return S_OK;
	}
	_catch_nested;
}



HRESULT CReportFrom::XReportForm::GetHorzPageNumPos(int* pnHorzPos)
{
	_try_nested( CReportFrom, ReportForm, GetHorzPageNumPos )
	{	
		*pnHorzPos = pThis->m_HorzPageNumPos;		
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::SetHorzPageNumPos(int  nHorzPos)
{
	_try_nested( CReportFrom, ReportForm, SetHorzPageNumPos )
	{			
		pThis->m_HorzPageNumPos = nHorzPos;
		return S_OK;
	}
	_catch_nested;
}



HRESULT CReportFrom::XReportForm::GetPageNumFont(LOGFONT* pLFONT)
{
	_try_nested( CReportFrom, ReportForm, GetPageNumFont )
	{	
		memcpy( pLFONT, &pThis->m_lfPageNum, sizeof(LOGFONT) );		
		return S_OK;
	}
	_catch_nested;
}


HRESULT CReportFrom::XReportForm::SetPageNumFont(LOGFONT  LFONT)
{
	_try_nested( CReportFrom, ReportForm, SetPageNumFont )
	{			
		memcpy( &pThis->m_lfPageNum, &LFONT, sizeof(LOGFONT) );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::SetMode(int nMode)
{
	_try_nested( CReportFrom, ReportForm, SetMode )
	{		
		pThis->m_nMode = nMode;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::GetBuildMode(int* pnMode)
{
	_try_nested( CReportFrom, ReportForm, GetBuildMode )
	{		
		//pThis->m_nMode = nMode;
		*pnMode = pThis->m_nBuildMode;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CReportFrom::XReportForm::SetBuildMode(int nMode)
{
	_try_nested( CReportFrom, ReportForm, SetBuildMode )
	{		
		pThis->m_nBuildMode = nMode;
		return S_OK;
	}
	_catch_nested;
}



/////////////////////////////////////////////////////////////////////////////
// CReportFileFilter

// Report file filter (for printing)
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CReportFileFilter, CCmdTargetEx)

// {9CE87E77-DC02-418a-A5CF-705785CD6DF8}
GUARD_IMPLEMENT_OLECREATE(CReportFileFilter, "FileFilters.ReportFileFilter", 
0x9ce87e77, 0xdc02, 0x418a, 0xa5, 0xcf, 0x70, 0x57, 0x85, 0xcd, 0x6d, 0xf8);


CReportFileFilter::CReportFileFilter()
{
	_OleLockApp( this );
	m_strDocumentTemplate = "Print Template";
	m_strFilterName.LoadString( IDS_REPORT_FILTERNAME );
	m_strFilterExt = _T(".rpt");
	AddDataObjectType(szArgumenReportForm);
}

CReportFileFilter::~CReportFileFilter()
{
	_OleUnlockApp( this );
}


bool CReportFileFilter::ReadFile( const char *pszFileName )
{
	IUnknown	*punk = 0;
	try
	{
		CString strType;
		if(m_sptrINamedData)
			strType = GetDataObjectType(0);	
		else
			strType = szTypeReportForm;
		punk = CreateNamedObject( strType );
		// get object name from path and check it's exists
		CString	strObjName = ::GetObjectNameFromPath(pszFileName);
		_bstr_t bstrName(strObjName);
		long	NameExists = 0;
	
		if(m_sptrINamedData)
			m_sptrINamedData->NameExists(bstrName, &NameExists);
		// if new name not exists allready

		if( CheckInterface(punk, IID_ISerializableObject))
		{
			CFile	file( pszFileName, CFile::modeRead );
			sptrISerializableObject	sptrO( punk );
			{
				SerializeParams	params;
				ZeroMemory( &params, sizeof( params ) );

				CArchive		ar( &file, CArchive::load );
				CArchiveStream	ars( &ar );
				sptrO->Load( &ars, &params );
			}
		}

		if( !NameExists && punk )
		{
			//m_sptrAttachedObject
			// set this name to object my
			INamedObject2Ptr sptrObj(punk);
			if( sptrObj )
			{
				sptrObj->SetName(bstrName);
			}
		}

		
		if( NameExists && punk )
		{
			INamedObject2Ptr sptrObj(punk);
			if( sptrObj )
				sptrObj->GenerateUniqueName( bstrName );
		}

		INumeredObjectPtr ptrNO( punk );
		if( ptrNO )
			ptrNO->GenerateNewKey( 0 );
		


		punk->Release();
		punk = 0;
	}
	catch(CException* e)
	{
		e->ReportError();
		e->Delete();
		if(punk)
			punk->Release();
		return false;
	}
	return true;
}

bool CReportFileFilter::WriteFile( const char *pszFileName )
{
	IUnknown* punk = 0;
	try
	{
		// convert object type to BSTR
		_bstr_t bstrType( GetDataObjectType(0) );

		// get active object in which type == m_strObjKind
		if (!(m_bObjectExists && m_sptrAttachedObject != NULL) && m_sptrIDataContext != NULL)
			m_sptrIDataContext->GetActiveObject( bstrType, &punk );
		else
		{
			punk = (IUnknown*)m_sptrAttachedObject.GetInterfacePtr();
			punk->AddRef();
		}
	
		if( !punk )
			return false;

		if(CheckInterface(punk, IID_ISerializableObject))
		{
			CFile	file( pszFileName, CFile::modeCreate|CFile::modeWrite );
			sptrISerializableObject	sptrO( punk );
			{
				SerializeParams	params;
				ZeroMemory( &params, sizeof( params ) );

				CArchive		ar( &file, CArchive::store );
				CArchiveStream	ars( &ar );
				sptrO->Store( &ars, &params );
			}
		}
		punk->Release();
		punk = 0;

	}
	catch(CException* e)
	{
		e->ReportError();
		e->Delete();
		if(punk)
			punk->Release();
		return false;
	}
	return true;
}

bool CReportFileFilter::BeforeReadFile()
{
	return CFileFilterBase::BeforeReadFile();
}

bool CReportFileFilter::AfterReadFile()
{
	return CFileFilterBase::AfterReadFile();
}


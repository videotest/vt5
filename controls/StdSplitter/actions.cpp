#include "stdafx.h"
#include "actions.h"
#include "resource.h"
#include "SplitterTabbed.h"
#include "dialogs.h"
#include "convert_help.h"




//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionCompareScreen, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSaveViewState, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionLoadViewState, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRenameView, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRemoveView, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAddView, CCmdTargetEx);

// olecreate 

// {6503C4B1-3BF7-4597-91B4-FABD2F5057BF}
GUARD_IMPLEMENT_OLECREATE(CActionCompareScreen, "StdSplitter.CompareScreen",
0x6503c4b1, 0x3bf7, 0x4597, 0x91, 0xb4, 0xfa, 0xbd, 0x2f, 0x50, 0x57, 0xbf);
// {D1AADA7B-5FB9-11d3-9999-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionSaveViewState, ".SaveViewState",
0xd1aada7b, 0x5fb9, 0x11d3, 0x99, 0x99, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {D1AADA77-5FB9-11d3-9999-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionLoadViewState, ".LoadViewState",
0xd1aada77, 0x5fb9, 0x11d3, 0x99, 0x99, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {D1AADA73-5FB9-11d3-9999-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionRenameView, ".RenameView",
0xd1aada73, 0x5fb9, 0x11d3, 0x99, 0x99, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {D1AADA6F-5FB9-11d3-9999-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionRemoveView, ".RemoveView",
0xd1aada6f, 0x5fb9, 0x11d3, 0x99, 0x99, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {D1AADA6B-5FB9-11d3-9999-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionAddView, ".AddView",
0xd1aada6b, 0x5fb9, 0x11d3, 0x99, 0x99, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);

// guidinfo 

// {5790E688-C55E-402b-B632-0DEEAFD2A0C4}
static const GUID guidCompareScreen =
{ 0x5790e688, 0xc55e, 0x402b, { 0xb6, 0x32, 0xd, 0xee, 0xaf, 0xd2, 0xa0, 0xc4 } };
// {D1AADA79-5FB9-11d3-9999-0000B493A187}
static const GUID guidSaveViewState =
{ 0xd1aada79, 0x5fb9, 0x11d3, { 0x99, 0x99, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {D1AADA75-5FB9-11d3-9999-0000B493A187}
static const GUID guidLoadViewState =
{ 0xd1aada75, 0x5fb9, 0x11d3, { 0x99, 0x99, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {D1AADA71-5FB9-11d3-9999-0000B493A187}
static const GUID guidRenameView =
{ 0xd1aada71, 0x5fb9, 0x11d3, { 0x99, 0x99, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {D1AADA6D-5FB9-11d3-9999-0000B493A187}
static const GUID guidRemoveView =
{ 0xd1aada6d, 0x5fb9, 0x11d3, { 0x99, 0x99, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {D1AADA69-5FB9-11d3-9999-0000B493A187}
static const GUID guidAddView =
{ 0xd1aada69, 0x5fb9, 0x11d3, { 0x99, 0x99, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };


//[ag]6. action info

ACTION_INFO_FULL(CActionCompareScreen, IDS_ACTION_COMPARESCREEN, IDS_MENU_WINDOW, -1, guidCompareScreen);
ACTION_INFO_FULL(CActionSaveViewState, IDS_ACTION_SAVEVIEWSTATE, IDS_MENU_WINDOW, -1, guidSaveViewState);
ACTION_INFO_FULL(CActionLoadViewState, IDS_ACTION_LOADVIEWSTATE, IDS_MENU_WINDOW, -1, guidLoadViewState);
ACTION_INFO_FULL(CActionRenameView, IDS_ACTION_RENAMEVIEW, IDS_MENU_WINDOW, -1, guidRenameView);
ACTION_INFO_FULL(CActionRemoveView, IDS_ACTION_REMOVEVIEW, IDS_MENU_WINDOW, -1, guidRemoveView);
ACTION_INFO_FULL(CActionAddView, IDS_ACTION_ADDVIEW, IDS_MENU_WINDOW, -1, guidAddView);

//[ag]7. targets

ACTION_TARGET(CActionCompareScreen, szTargetFrame);
ACTION_TARGET(CActionSaveViewState, szTargetFrame);
ACTION_TARGET(CActionLoadViewState, szTargetFrame);
ACTION_TARGET(CActionRenameView, szTargetFrame);
ACTION_TARGET(CActionRemoveView, szTargetFrame);
ACTION_TARGET(CActionAddView, szTargetFrame);

//[ag]8. arguments
ACTION_ARG_LIST(CActionCompareScreen)
	ARG_STRING("ImageName", "CompareScreen")
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionSaveViewState)
	ARG_STRING("FileName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionLoadViewState)
	ARG_STRING("FileName", 0)
END_ACTION_ARG_LIST()


ACTION_ARG_LIST(CActionAddView)
	ARG_STRING("Name", 0)
END_ACTION_ARG_LIST()


ACTION_ARG_LIST(CActionRenameView)
	ARG_STRING("Name", 0)
END_ACTION_ARG_LIST()


//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionCompareScreen implementation
BEGIN_INTERFACE_MAP(CActionCompareScreen, CActionBase)
	INTERFACE_PART(CActionCompareScreen, IID_ILongOperation, Long)
END_INTERFACE_MAP()

CActionCompareScreen::CActionCompareScreen() : m_ptOrigin0(0, 0), m_ptOrigin1(0, 0), m_nWidth0(0), m_sizeImg(0, 0)
{
	m_sptrImg0 = 0;
	m_sptrImg1 = 0;

	m_strCC0 = "";
	m_strCC1 = "";

	m_fZoom0 = 1.;
	m_fZoom1 = 1.;
}

CActionCompareScreen::~CActionCompareScreen()
{
}

IUnknown* CActionCompareScreen::_GetView(int nRow)
{
	sptrIFrameWindow sptrFrame(m_punkTarget);
	if(sptrFrame == 0)
		return 0;

	IUnknown* punkSplitter = 0;
	sptrFrame->GetSplitter(&punkSplitter);

	ISplitterWindowPtr sptrSplitter(punkSplitter);
	if(punkSplitter)
		punkSplitter->Release();

	if(sptrSplitter == 0)
		return 0;

	IUnknown* punkV = 0;
	sptrSplitter->GetViewRowCol(0, nRow, &punkV);

	return punkV;
}

bool CActionCompareScreen::_GetImages()
{

	IUnknown* punkIV0 = _GetView(0);
	IUnknown* punkIV1 = _GetView(1);
	IImageViewPtr	sptrIV0(punkIV0);
	IImageViewPtr	sptrIV1(punkIV1);
	if(punkIV0)
		punkIV0->Release();
	if(punkIV1)
		punkIV1->Release();

	if(sptrIV0 == 0 || sptrIV1 == 0)
		return false;

	IUnknown* punkImg0 = 0;
	IUnknown* punkImg1 = 0;

	sptrIV0->GetActiveImage(&punkImg0, 0);
	sptrIV1->GetActiveImage(&punkImg1, 0);

	m_sptrImg0 = punkImg0;
	m_sptrImg1 = punkImg1;

	if(punkImg0)
		punkImg0->Release();
	if(punkImg1)
		punkImg1->Release();

	if(m_sptrImg0 == 0 || m_sptrImg1 == 0)
		return false;

	return true;
}

bool CActionCompareScreen::_CalcBounds()
{
	IUnknown* punkIV0 = _GetView(0);
	IUnknown* punkIV1 = _GetView(1);
	IScrollZoomSitePtr	sptrSZS0(punkIV0);
	IScrollZoomSitePtr	sptrSZS1(punkIV1);
	if(punkIV0)
		punkIV0->Release();
	if(punkIV1)
		punkIV1->Release();
	
	if(sptrSZS0 == 0 || sptrSZS1 == 0)
		return false;

	sptrSZS0->GetScrollPos(&m_ptOrigin0);
	sptrSZS1->GetScrollPos(&m_ptOrigin1);

	sptrSZS0->GetZoom(&m_fZoom0);
	sptrSZS1->GetZoom(&m_fZoom1);
	
	m_ptOrigin0.x /= m_fZoom0;
	m_ptOrigin0.y /= m_fZoom0;
	m_ptOrigin1.x /= m_fZoom1;
	m_ptOrigin1.y /= m_fZoom1;

	IWindowPtr sptrW0(sptrSZS0);

	if(sptrW0 == 0)
		return false;

	long nImgWidth = 0;
	long nImgHeight = 0;

	HWND	hwnd = 0;
	CRect	rcClient0 = NORECT;
	sptrW0->GetHandle((HANDLE*)&hwnd);
	::GetClientRect(hwnd, &rcClient0);

	m_nWidth0 = rcClient0.Width()/m_fZoom0 + m_ptOrigin0.x;

	CSize size1(0, 0);
	sptrSZS1->GetClientSize(&size1);
	nImgWidth = m_nWidth0 + size1.cx - m_ptOrigin1.x;

	CSize size0(0, 0);
	sptrSZS0->GetClientSize(&size0);

	CRect rc0(0, 0, 100, size0.cy);
	CRect rc1(0, 0, 100, size1.cy);

	rc0.OffsetRect(CPoint(0, -m_ptOrigin0.y));
	rc1.OffsetRect(CPoint(0, -m_ptOrigin1.y));
	//rc0 = ::ConvertToWindow(sptrSZS0, rc0);
	//rc1 = ::ConvertToWindow(sptrSZS1, rc1);

	rc0.IntersectRect(&rc0, &rc1);

	nImgHeight = rc0.Height();

	
	m_sizeImg = CSize(nImgWidth, nImgHeight);

	if(m_ptOrigin1.y > m_ptOrigin0.y)
	{
		m_ptOrigin1.y -= m_ptOrigin0.y;
		m_ptOrigin0.y = 0;
	}
	else
	{
		m_ptOrigin0.y -= m_ptOrigin1.y;
		m_ptOrigin1.y = 0;
	}

	return (m_sizeImg.cx > 0 && m_sizeImg.cy > 0);

}

bool CActionCompareScreen::DoUndo()
{
	m_changes.DoUndo(m_sptrDoc);
	return true;
}

bool CActionCompareScreen::DoRedo()
{
	m_changes.DoRedo(m_sptrDoc);
	return true;
}

bool CActionCompareScreen::Invoke()
{
	CString	strImageName = GetArgumentString(_T("ImageName"));

	if(!_GetImages())
		return false;

	if(!_CalcBounds())
		return false;

	int	nSplitterWidth = ::GetValueInt(  GetAppUnknown(), "\\SplitterCompare", "CompareResultSpace", 1 );
	COLORREF clrSplitter = ::GetValueColor(  GetAppUnknown(), "\\SplitterCompare", "CompareResultColor", RGB(0,0,0) );

	m_sizeImg.cx += nSplitterWidth;
	m_nWidth0 += nSplitterWidth;

	CImageWrp  imageDest;

	
	CImageWrp  imageSrc0(m_sptrImg0);
	CImageWrp  imageSrc1(m_sptrImg1);

	if( !imageDest.CreateNew(m_sizeImg.cx, m_sizeImg.cy, imageSrc0.GetColorConvertor()) )
		return false;
	
	::SetObjectName(imageDest, strImageName);
	
	//set to doc data
	sptrIFrameWindow sptrFrame(m_punkTarget);
	if(sptrFrame == 0)
		return false;

	long colors = max(imageSrc0.GetColorsCount(), imageSrc1.GetColorsCount());


	StartNotification(m_sizeImg.cy, 2);

	int	nWidth = imageSrc0.GetWidth();
	int	cbImage1Data = min( m_nWidth0, nWidth)*sizeof(color);
	int	nSpace = m_nWidth0-nWidth-nSplitterWidth;

	_convert	convert;
	convert.init( imageSrc0 );
	convert.set_rgb( ::GetValueColor(GetAppUnknown(), "\\Editor", "Back", RGB(0,0,0) ) );
	_convert	convertSplitter;
	convertSplitter.init( imageSrc0 );
	convertSplitter.set_rgb( clrSplitter );

	//fill image data
	long	x, y;
	for(y = m_ptOrigin0.y; y < m_sizeImg.cy+m_ptOrigin0.y; y++)
	{
		for(long c = 0; c < imageSrc0.GetColorsCount(); c++)
		{
			color *pRowSrc = imageSrc0.GetRow(y, c);
			color *pRowDest = imageDest.GetRow(y-m_ptOrigin0.y, c);

			memcpy(pRowDest, pRowSrc, cbImage1Data);

			if( nSpace )
			{
				pRowDest+=nWidth;
				color	n = convert.get_color( c );
				for( x = 0; x < nSpace; x++, pRowDest++ )
					*pRowDest = n;
			}
		}
		Notify(y-m_ptOrigin0.y);
	}

	if( nSplitterWidth )
		for(y = 0; y < m_sizeImg.cy; y++)
			for(long c = 0; c < imageSrc0.GetColorsCount(); c++)
			{
				color *pRowDest = imageDest.GetRow(y, c)+nWidth+nSpace;
				color	n = convertSplitter.get_color( c );
				for( x = 0; x < nSplitterWidth; x++, pRowDest++ )
					*pRowDest = n;
			}

	

	NextNotificationStage();

	IUnknown* punkCCDest = 0;
	IUnknown* punkCCSrc = 0;
	imageDest->GetColorConvertor(&punkCCDest);
	imageSrc1->GetColorConvertor(&punkCCSrc);

	IColorConvertor3Ptr sptrCCDest(punkCCDest);
	IColorConvertor3Ptr sptrCCScr(punkCCSrc);

	if(punkCCDest)
		punkCCDest->Release();
	if(punkCCSrc)
		punkCCSrc->Release();

	if(sptrCCDest == 0 || sptrCCScr == 0)
		return false;

	color **ppRowS = new color*[colors]; //source with it's origin CC
	color **ppRowSS = new color*[colors];//source with imageDest CC

	long nWidth1 = m_sizeImg.cx - m_nWidth0;

	ASSERT(nWidth1 > 0);


	long nOrigWidth1 = imageSrc1.GetWidth();

	double* pLab = new double[nOrigWidth1*3];

	bool bNeedConvert = imageSrc0.GetColorConvertor() != imageSrc1.GetColorConvertor();

	for(y = m_ptOrigin1.y; y < m_sizeImg.cy+m_ptOrigin1.y; y++)
	{
		for(long c = 0; c < colors; c++)
		{
			ppRowS[c] = imageSrc1.GetRow(y, c);
			if(bNeedConvert)
				ppRowSS[c] = new color[nOrigWidth1];
		}

		if(bNeedConvert)
		{
			sptrCCScr->ConvertImageToLAB(ppRowS, pLab, nOrigWidth1);
			sptrCCDest->ConvertLABToImage(pLab, ppRowSS, nOrigWidth1);
		}


		for(c = 0; c < imageSrc0.GetColorsCount(); c++)
		{
			color* pRowSrc = 0;
			if(bNeedConvert)
				pRowSrc = ppRowSS[c];
			else
				pRowSrc = ppRowS[c];

			color *pRowDest = imageDest.GetRow(y-m_ptOrigin1.y, c);

			pRowSrc += (color)m_ptOrigin1.x;
			pRowDest += (color)m_nWidth0;

			memcpy(pRowDest, pRowSrc, nWidth1*sizeof(color));

			if(bNeedConvert)
				delete ppRowSS[c];
		}

		Notify(y-m_ptOrigin1.y);
	}

	FinishNotification();


	IUnknown* punkDoc = 0;
	sptrFrame->GetDocument(&punkDoc);

	m_sptrDoc = punkDoc;
	if(punkDoc)
	{
		//::SetValue(punkDoc, 0, 0, _variant_t(imageDest));
		m_changes.SetToDocData(m_sptrDoc, imageDest);
		punkDoc->Release();

		m_lTargetKey =  ::GetObjectKey(m_sptrDoc);
	}



	delete ppRowS;
	delete ppRowSS;
	delete pLab;
	
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionSaveViewState implementation
CActionSaveViewState::CActionSaveViewState()
{
}

CActionSaveViewState::~CActionSaveViewState()
{
}


bool CActionSaveViewState::ExecuteSettings( CWnd *pwndParent )
{
	if (!m_punkTarget)
		return false;

	sptrIFrameWindow sptrFrame(m_punkTarget);
	IUnknown* punk = 0;
	sptrFrame->GetSplitter(&punk);
	if (!CheckInterface(punk, IID_ISplitterTabbed))
	{
		if(punk)
			punk->Release();
		return false;
	}
	if(punk)
			punk->Release();

	m_strFileName = GetArgumentString( "FileName" );

	if( ForceNotShowDialog() )
	{
		if( m_strFileName.IsEmpty() )
			m_strFileName = "default.splitter";
		return true;
	}

	if( m_strFileName.IsEmpty() || ForceShowDialog() )
	{
		CString	strPath = ::AfxGetApp()->GetProfileString( "general", "LastOpenPath" );

		if( !strPath.IsEmpty() ) 
			::SetCurrentDirectory( strPath );

		CString	str;
		str.LoadString( IDS_FILTER_STATE );

		CFileDialog	dlg( FALSE, ".splitter", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, str, 0, _FILE_OPEN_SIZE_ );


		if( dlg.DoModal() == IDOK )
			m_strFileName = dlg.GetPathName();
		else
			return false;

		char	szPath[_MAX_PATH];
		::GetCurrentDirectory( _MAX_PATH, szPath );

		::AfxGetApp()->WriteProfileString( "general", "LastOpenPath", szPath );

		SetArgument( "FileName", COleVariant( m_strFileName ) );
	}
	return true;
}


bool CActionSaveViewState::Invoke()
{
	if (!m_punkTarget)
		return false;

	_try(CActionLoadViewState, Invoke)
	{
		ASSERT(m_punkTarget);
		sptrIFrameWindow sptrF(m_punkTarget);

		IUnknown* punk;
		sptrF->GetSplitter(&punk);

		sptrIFileDataObject	sptrFile = 0;
		_bstr_t	bstrFileName = m_strFileName;
		if(!m_strFileName.IsEmpty())
			sptrFile.CreateInstance( szNamedDataProgID );
				
		sptrISplitterTabbed sptrSplt(punk);
		if(punk)
			punk->Release();
		if(sptrFile != 0 && sptrSplt != 0)
		{
			if( S_OK != sptrSplt->SaveState(sptrFile) )
				return false;

			// [vanek]: save to file - 07.04.2004
			sptrFile->SaveFile( bstrFileName );
		}

	}
	_catch
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionLoadViewState implementation
CActionLoadViewState::CActionLoadViewState()
{
}

CActionLoadViewState::~CActionLoadViewState()
{
}


bool CActionLoadViewState::ExecuteSettings( CWnd *pwndParent )
{
	if (!m_punkTarget)
		return false;

	sptrIFrameWindow sptrFrame(m_punkTarget);
	IUnknown* punk = 0;
	sptrFrame->GetSplitter(&punk);
	if (!CheckInterface(punk, IID_ISplitterTabbed))
	{
		if(punk)
			punk->Release();
		return false;
	}
	if(punk)
		punk->Release();

	m_strFileName = GetArgumentString( "FileName" );

	if( ForceNotShowDialog() )
	{
		if( m_strFileName.IsEmpty() )
			m_strFileName = "default.splitter";
		return true;
	}

	if( m_strFileName.IsEmpty() || ForceShowDialog() )
	{
		CString	strPath = ::AfxGetApp()->GetProfileString( "general", "LastOpenPath" );

		if( !strPath.IsEmpty() ) 
			::SetCurrentDirectory( strPath );

		CString	str;
		str.LoadString( IDS_FILTER_STATE );

		CFileDialog	dlg( TRUE, ".splitter", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, str, 0, _FILE_OPEN_SIZE_ );


		if( dlg.DoModal() == IDOK )
			m_strFileName = dlg.GetPathName();
		else
			return false;

		char	szPath[_MAX_PATH];
		::GetCurrentDirectory( _MAX_PATH, szPath );

		::AfxGetApp()->WriteProfileString( "general", "LastPath", szPath );

		SetArgument( "FileName", COleVariant( m_strFileName ) );
	}
	return true;
}



bool CActionLoadViewState::Invoke()
{
	if (!m_punkTarget)
		return false;

	_try(CActionLoadViewState, Invoke)
	{
		ASSERT(m_punkTarget);
		sptrIFrameWindow sptrF(m_punkTarget);

		IUnknown* punk;
		sptrF->GetSplitter(&punk);
		
		sptrIFileDataObject	sptrFile = 0;
		_bstr_t	bstrFileName = m_strFileName;
		if(!m_strFileName.IsEmpty())
		{
			sptrFile.CreateInstance( szNamedDataProgID );

			if (sptrFile != 0)
				sptrFile->LoadFile( bstrFileName );
		}
		sptrISplitterTabbed sptrSplt(punk);
		if(punk)
			punk->Release();
		if(sptrSplt != 0)
			sptrSplt->LoadState(sptrFile);			
	}
	_catch
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionRenameView implementation
CActionRenameView::CActionRenameView()
{
}

CActionRenameView::~CActionRenameView()
{
}

bool CActionRenameView::Invoke()
{
	if (!m_punkTarget)
		return false;

	_try(CActionRenameView, Invoke)
	{
		ASSERT(m_punkTarget);
		sptrIFrameWindow sptrF(m_punkTarget);

		IUnknown* punk = 0;
		sptrF->GetSplitter(&punk);
		sptrISplitterTabbed sptrSplt(punk);
		if(punk)
			punk->Release();
		if(sptrSplt != 0)
			sptrSplt->RenameView(_bstr_t(m_strName));
		punk->Release();
		
	}
	_catch
	{
		return false;
	}
	return true;
}

bool CActionRenameView::ExecuteSettings( CWnd *pwndParent )
{
	if (!m_punkTarget)
		return false;

	sptrIFrameWindow sptrFrame(m_punkTarget);
	IUnknown* punk = 0;
	sptrFrame->GetSplitter(&punk);
	if (!CheckInterface(punk, IID_ISplitterTabbed))
	{
		if(punk)
			punk->Release();
		return false;
	}

	// [vanek]: store argument - 06.04.2004
	m_strName = GetArgumentString(_T("Name"));
	
	// both names must be valid
	if (ForceNotShowDialog() && m_strName.IsEmpty())
		return true;

	if (m_strName.IsEmpty() || ForceShowDialog())
	{
		////////////////////////////
		CNameDialog	dlg(pwndParent);
		dlg.m_strTitle = "Rename view";
		sptrISplitterTabbed sptrSplt(punk);
		if(punk)
			punk->Release();
		if(sptrSplt != 0)
		{
			BSTR bstrName = 0;
			sptrSplt->GetActiveViewName(&bstrName);
			dlg.m_strName = bstrName;
			::SysFreeString(bstrName);
		}

			
		if (dlg.DoModal() != IDOK)
			return false;

		m_strName = dlg.m_strName;
	}
	SetArgument(_T("Name"), _variant_t(m_strName));

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionRemoveView implementation
CActionRemoveView::CActionRemoveView()
{
}

CActionRemoveView::~CActionRemoveView()
{
}

bool CActionRemoveView::Invoke()
{
	if (!m_punkTarget)
		return false;

	sptrIFrameWindow sptrFrame(m_punkTarget);
	IUnknown* punk = 0;
	sptrFrame->GetSplitter(&punk);
	if (!CheckInterface(punk, IID_ISplitterTabbed))
	{
		if(punk)
			punk->Release();
		return false;
	}

	_try(RemoveView, Invoke)
	{
		sptrISplitterTabbed sptrSplt(punk);
		if(punk)
			punk->Release();
		if(sptrSplt != 0)
			sptrSplt->RemoveActiveView();
	}
	_catch
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionAddView implementation
CActionAddView::CActionAddView()
{
}

CActionAddView::~CActionAddView()
{
}

bool CActionAddView::ExecuteSettings( CWnd *pwndParent )
{
	if (!m_punkTarget)
		return false;

	sptrIFrameWindow sptrFrame(m_punkTarget);
	IUnknown* punk = 0;
	sptrFrame->GetSplitter(&punk);
	if (!CheckInterface(punk, IID_ISplitterTabbed))
	{
		if(punk)
			punk->Release();
		return false;
	}
	if(punk)
		punk->Release();

	// [vanek]: store argument - 06.04.2004
	m_strName = GetArgumentString(_T("Name"));
	
	// both names must be valid
	if (ForceNotShowDialog() && m_strName.IsEmpty())
		return true;

	if (m_strName.IsEmpty() || ForceShowDialog())
	{
			////////////////////////////
			CNameDialog	dlg(pwndParent);

			dlg.m_strTitle = "Add view";
				
			if (dlg.DoModal() != IDOK)
				return false;

			m_strName = dlg.m_strName;
	}

	SetArgument(_T("Name"), _variant_t(m_strName));

	return true;
}


bool CActionAddView::Invoke()
{
	if (!m_punkTarget)
		return false;

	_try(CActionAddView, Invoke)
	{
		ASSERT(m_punkTarget);
		sptrIFrameWindow sptrF(m_punkTarget);

		IUnknown* punk = 0;
		sptrF->GetSplitter(&punk);
		sptrISplitterTabbed sptrSplt(punk);
		if(punk)
			punk->Release();
		if(sptrSplt != 0)
			sptrSplt->AddView(_bstr_t(m_strName));
	}
	_catch
	{
		return false;
	}
	return true;
}






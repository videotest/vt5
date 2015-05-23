#include "stdafx.h"
#include "ImageDoc.h"
#include "phaseviews.h"
#include <math.h>

int _GetNamedDataInt(IUnknown *punk, _bstr_t bstrSect, _bstr_t bstrKey, int nDefault)
{
	INamedDataObject2Ptr ptrNDO(punk);
	if (ptrNDO == 0) return -1;
	INamedDataPtr sptrNDO(punk);
	if (sptrNDO == 0) return -1;
	sptrNDO->SetupSection(bstrSect);
	_variant_t var;
	sptrNDO->GetValue(bstrKey, &var);
	if (var.vt == VT_I2)
		return var.iVal;
	else if (var.vt == VT_I4)
		return var.lVal;
	else
		return -1;
}

void DrawPhaseRect(CBinImageWrp pbin, BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs,
	RECT rectDest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwBackColor,
	BOOL bUseScrollZoom, DWORD dwShowFlags, int nTransparency, int nShowPhase,
	COLORREF *pColors)
{
	if( !pdibBits )
		return;
	bool bFore = dwShowFlags & isfBinaryFore || dwShowFlags & isfBinary;
	bool bBack = dwShowFlags & isfBinaryBack || dwShowFlags & isfBinary;
	bool bContour = (dwShowFlags & isfBinaryContour) == isfBinaryContour;
	if(!bFore && !bBack && !bContour)
		return;
	COLORREF clrContour = 0;
	if(bContour)
		clrContour = ::GetValueColor(GetAppUnknown(), "Binary", "ContourColor", RGB(0, 0, 0));
	if( !pbi || pbin==0 )return;
	if(bUseScrollZoom == FALSE)
	{
		fZoom = 1;
		ptScroll.x = 0;
		ptScroll.y = 0;
	}
	int		cxImg, cyImg;
	pbin->GetSizes( &cxImg, &cyImg );
	POINT	pointImageOffest = {0, 0};
	pbin->GetOffset(&pointImageOffest);
	int		x, y, xImg, yImg, yBmp;
	byte	fillR = GetRValue(dwFillColor);
	byte	fillG = GetGValue(dwFillColor);
	byte	fillB = GetBValue(dwFillColor);
	byte	backR = GetRValue(dwBackColor);
	byte	backG = GetGValue(dwBackColor);
	byte	backB = GetBValue(dwBackColor);
	byte	cntrR = GetRValue(clrContour);
	byte	cntrG = GetGValue(clrContour);
	byte	cntrB = GetBValue(clrContour);
	int nOpacity = 100-nTransparency;
	byte	*p, *pmask, *pmaskPrev, *pmaskNext;
	int		cx4Bmp = (pbi->biWidth*3+3)/4*4;
	int	xBmpStart = max( int(ceil(( pointImageOffest.x)*fZoom-ptScroll.x)), rectDest.left );				\
	int	xBmpEnd = min( int(floor((pointImageOffest.x+cxImg)*fZoom-ptScroll.x+.5)), rectDest.right );		\
	int	yBmpStart = max( int(ceil((pointImageOffest.y)*fZoom-ptScroll.y)), rectDest.top );					\
	int	yBmpEnd = min( int(floor((pointImageOffest.y+cyImg)*fZoom-ptScroll.y+.5)), rectDest.bottom );		\
	int	yImgOld = -1;
	int	yImgPrev = -1;
	int	yImgNext = -1;
	int	yImgPrevOld = -1;
	int	yImgNextOld = -1;
	int	t, cx = max(xBmpEnd-xBmpStart, 0), cy = max(yBmpEnd-yBmpStart, 0);									\
	int	*pxofs = new int[cx];																				\
	int	*pyofs = new int[cy];																				\
	for( t = 0; t < xBmpEnd-xBmpStart; t++ )																\
		pxofs[t] = int((t+xBmpStart+ptScroll.x)/fZoom-pointImageOffest.x);									\
	for( t = 0; t < yBmpEnd-yBmpStart; t++ )																\
		pyofs[t] = int((t+yBmpStart+ptScroll.y)/fZoom-pointImageOffest.y);									\
	for( y = yBmpStart, yBmp = yBmpStart; y < yBmpEnd; y++, yBmp++ )
	{
		p = pdibBits+(pbi->biHeight-yBmp-1+ptBmpOfs.y)*cx4Bmp+(xBmpStart-ptBmpOfs.x)*3;
		yImg =pyofs[y-yBmpStart];
		if( yImgOld != yImg )
		{
			pbin->GetRow(&pmask, yImg, FALSE);
			yImgOld = yImg;
		}
		pmaskPrev = 0;
		pmaskNext = 0;
		if(bContour)
		{
			yImg =pyofs[y-yBmpStart-1];
			yImg =pyofs[y-yBmpStart+1];
			if(yImgPrev >= 0 && yImgPrev < cyImg && yImgPrevOld != yImgPrev)
			{
				pbin->GetRow(&pmaskPrev, yImgPrev, FALSE);
				yImgPrevOld = yImgPrev;
			}
			if(yImgNext >= 0 && yImgNext < cyImg && yImgNextOld != yImgNext)
			{
				pbin->GetRow(&pmaskNext, yImgNext, FALSE);
				yImgNextOld = yImgNext;
			}
		}
		for( x = xBmpStart; x < xBmpEnd; x++ )
		{
			xImg = pxofs[x-xBmpStart];

			if(pmask[xImg]<0x80||pColors&&nShowPhase>=0&&nShowPhase!=pmask[xImg]-0x80)
			{
				if(bBack)
				{
					*p = byte((backB*nOpacity+*p*nTransparency)/100); p++;
					*p = byte((backG*nOpacity+*p*nTransparency)/100); p++;
					*p = byte((backR*nOpacity+*p*nTransparency)/100); p++;
				}
				else if(!bContour)
				{
					p++;p++;p++;
				}
			}
			else 
			{
				if(bFore)
				{
					byte	fillR,fillG,fillB;
					if (pColors==0)
					{
						fillR = GetRValue(dwFillColor);
						fillG = GetGValue(dwFillColor);
						fillB = GetBValue(dwFillColor);
					}
					else
					{
						int nPhase = pmask[xImg]-0x80;
						fillR = GetRValue(pColors[nPhase]);
						fillG = GetGValue(pColors[nPhase]);
						fillB = GetBValue(pColors[nPhase]);
					}
          
					*p = byte((fillB*nOpacity+*p*nTransparency)/100); p++;
					*p = byte((fillG*nOpacity+*p*nTransparency)/100); p++;
					*p = byte((fillR*nOpacity+*p*nTransparency)/100); p++;
				}
				else if(!bContour)
				{
					p++;p++;p++;
				}
			}
			if(bContour)
			{
				long nXPrev = x-1;
				xImg = pxofs[nXPrev-xBmpStart];
				long nXNext = x+1;
				xImg = pxofs[nXNext-xBmpStart];
				if((pmask[xImg] !=0) && 
				  ((nXPrev >= 0 && (pmask[nXPrev] == 0)) ||
				   (nXNext < cxImg && (pmask[nXNext]==0))||
				   (pmaskPrev && (pmaskPrev[xImg]==0))   || 
				   (pmaskNext && (pmaskNext[xImg]==0))))
				{
					*p = cntrB; p++;
					*p = cntrG; p++;
					*p = cntrR; p++;
				}
				else
				{
					p++;p++;p++;
				}
			}
		}
	}
	delete pxofs;delete pyofs;
}

IMPLEMENT_DYNCREATE(CPhaseView, CCmdTarget)
// {213501A4-AAA3-467d-A05D-AD5B18671504}
GUARD_IMPLEMENT_OLECREATE(CPhaseView, "ImageDoc.Phase", 
0x213501a4, 0xaaa3, 0x467d, 0xa0, 0x5d, 0xad, 0x5b, 0x18, 0x67, 0x15, 0x4);

CPhaseViewBase::CPhaseViewBase(void)
{
}

CPhaseViewBase::~CPhaseViewBase(void)
{
}

void CPhaseViewBase::InitPhaseColors()
{
	int nPhases = ::GetValueInt(::GetAppUnknown(false), "Phase", "PhasesCount", 0);
	m_arrColors.SetSize(max(nPhases,128));
	m_clrSync = GetValueColor(GetAppUnknown(), "\\Binary", "SyncColor", RGB( 255,255,0 ) );
	for (int i = 0; i < nPhases; i++)
	{
		CString sEntry;
		sEntry.Format("Phase%dColor", i+1);
		COLORREF clr = ::GetValueColor(::GetAppUnknown(false), "Phase", sEntry, m_clrSync);
		m_arrColors[i] = clr;
	}
	for (; i < 128; i++)
		m_arrColors[i] = m_clrSync;
	m_nTransparency = ::GetValueInt(::GetAppUnknown(false), "Phase", "Transparency", 0);
	m_nTransparency = max(min(m_nTransparency,100),0);
}


void CPhaseViewBase::DrawLayers(CDC* pdc, BITMAPINFOHEADER* pbmih, byte* pdibBits, CRect rectPaint, bool bUseScrollZoom)
{
	InitPhaseColors();
	DWORD dwBackColor = ::GetValueColor(GetAppUnknown(), "Binary", "BackColor", RGB(0, 0, 0));
	int nShowPhase = -1;
	_variant_t varPhase;
	CNamedPropBagImpl::GetProperty(_bstr_t("Phase"), &varPhase);
	if (varPhase.vt == VT_I4)
		nShowPhase = varPhase.lVal;
	POSITION pos = m_listBinaries.GetHeadPosition();
	while(pos)
	{
		CBinImageWrp pBin = m_listBinaries.GetNext(pos);
		int nPhase = _GetNamedDataInt(pBin, _bstr_t("Phase"), _bstr_t("PhaseNum"), -2);
		if (nPhase == -1)
		{
			DrawPhaseRect(pBin, pbmih, pdibBits, rectPaint.TopLeft(), rectPaint, ::GetZoom(GetControllingUnknown()),
				m_pointScroll, 0, dwBackColor, bUseScrollZoom, m_dwShowFlags, m_nTransparency,
				nShowPhase,	m_arrColors.GetData());
			break;
		}
		if (nShowPhase != -1 && nPhase != nShowPhase) continue;
		COLORREF clr;
		if (nPhase < 0 || nPhase >= m_arrColors.GetSize())
			clr = m_clrSync;
		else
			clr = m_arrColors[nPhase];
		DrawPhaseRect(pBin, pbmih, pdibBits, rectPaint.TopLeft(), rectPaint, ::GetZoom(GetControllingUnknown()),
			m_pointScroll, clr, dwBackColor, bUseScrollZoom, m_dwShowFlags, m_nTransparency, -1, NULL);
	}
}

void CPhaseViewBase::_AttachObjects( IUnknown *punkContextFrom )
{
	POSITION pos = m_listBinaries.GetHeadPosition();
	while(pos)
		m_listBinaries.GetNext(pos)->Release();
	m_listBinaries.RemoveAll();
	bool bChangeList = punkContextFrom == GetControllingUnknown();
	CImageSourceView::_AttachObjects(punkContextFrom);
	//attach binary image
	IDataContext2Ptr sptrContext(punkContextFrom);
	if(sptrContext != 0)
	{
		LONG_PTR nPos = 0;
		sptrContext->GetFirstSelectedPos(_bstr_t(szTypeBinaryImage), &nPos);
		while(nPos)
		{
			IUnknown* punkObject = 0;
			sptrContext->GetNextSelected(_bstr_t(szTypeBinaryImage), &nPos, &punkObject);
			if(punkObject)
			{
				m_listBinaries.AddTail(punkObject);
				//punkObject->Release();
				IRectObjectPtr	ptrRect( punkObject );
				CRect	rect;
				ptrRect->GetRect( &rect );
				m_size_client.cx = max( m_size_client.cx, rect.right );
				m_size_client.cy = max( m_size_client.cy, rect.bottom );
			}
		}
	}
	if( bChangeList )
	{
		POSITION pos = m_listBinaries.GetHeadPosition();
		while(pos)
			m_listObjects.AddTail(m_listBinaries.GetNext(pos));
	}
	_UpdateScrollSizes();
}

/////////////////////////////////////////////////////////////////////////////////////////
//CPhaseView implementation
CPhaseView::CPhaseView()
{
	m_dwShowFlags = isfBinaryFore;
	m_sName = "PhaseView";
	m_sUserName.LoadString(IDS_PHASEVIEWNAME);
}

const char* CPhaseView::GetViewMenuName()
{return "PhaseView";}



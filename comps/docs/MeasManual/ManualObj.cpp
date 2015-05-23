#include "StdAfx.h"
#include "MeasManual.h"
#include "ManualObj.h"

#include "NameConsts.h"
#include "contourapi.h"

#include <math.h>
#include <Limits.h>
#include <Float.h>
#include "\vt5\common2\misc_calibr.h"
#include "\vt5\common2\class_utils.h"

const char *CManualMeasObject::c_szType = szTypeManualMeasObject;

IMPLEMENT_DYNCREATE(CManualMeasObject, CCmdTargetEx);

// {F44EE81B-6ADF-485b-AD2D-8AB0B9B3DE18}
static const GUID clsidManualMeasObject = 
{ 0xf44ee81b, 0x6adf, 0x485b, { 0xad, 0x2d, 0x8a, 0xb0, 0xb9, 0xb3, 0xde, 0x18 } };

// {FFF2043D-22A1-47a1-80F2-030EE4CB3D46}
GUARD_IMPLEMENT_OLECREATE(CManualMeasObject, "MeasManual.ManualMeasObject", 
0xfff2043d, 0x22a1, 0x47a1, 0x80, 0xf2, 0x3, 0xe, 0xe4, 0xcb, 0x3d, 0x46);


BEGIN_INTERFACE_MAP(CManualMeasObject, CDataObjectBase)
	INTERFACE_PART(CManualMeasObject, IID_IManualMeasureObject,	ManualObj)
	INTERFACE_PART(CManualMeasObject, IID_IManualMeasureObject2,	ManualObj)
	INTERFACE_PART(CManualMeasObject, IID_IDrawObject, Draw)
	INTERFACE_PART(CManualMeasObject, IID_IDrawObject2, Draw)
	INTERFACE_PART(CManualMeasObject, IID_IClonableObject, Clone)
	INTERFACE_PART(CManualMeasObject, IID_IRectObject, Rect)
	INTERFACE_PART(CManualMeasObject, IID_INotifyObject, Notify)
	INTERFACE_PART(CManualMeasObject, IID_INotifyObject2, Notify)
	INTERFACE_PART(CManualMeasObject, IID_ICalibr, Calibr)
END_INTERFACE_MAP()


// NOTE: ManualMeasObj's container is MeasureObject !!!
DATA_OBJECT_INFO_FULL(IDS_MEAS_MANUAL_TYPE, CManualMeasObject, CManualMeasObject::c_szType, szTypeObject, clsidManualMeasObject, IDI_THUMBNAIL_MEAS );

IMPLEMENT_UNKNOWN(CManualMeasObject, ManualObj);
IMPLEMENT_UNKNOWN(CManualMeasObject, Clone);
IMPLEMENT_UNKNOWN(CManualMeasObject, Rect);
IMPLEMENT_UNKNOWN(CManualMeasObject, Notify);
IMPLEMENT_UNKNOWN(CManualMeasObject, Calibr);


CManualMeasObject::CManualMeasObject()
{
	m_bCalculated = false;
	m_lParamKey = -1;
	m_dwParamType = 0;
	m_dwObjType = (DWORD)emotUndefined;
	m_fValue = 0;
	m_arrPoints.RemoveAll();

	m_fCalibr = 1;

	m_fXOffset = m_fYOffset = 0.;

	m_nCrossOffset = CROSS_OFFSET;
	m_nArcSize = ARC_SIZE;
	m_nSensZoneLen = SENSZONE_LENGHT;

	m_ptOffset = CPoint(0, 0);
	m_pt1 = m_pt2 = CPoint(0, 0);
}

CManualMeasObject::~CManualMeasObject()
{
	m_arrPoints.RemoveAll();
}

HRESULT CManualMeasObject::XNotify::NotifyCreate()
{
	METHOD_PROLOGUE_EX(CManualMeasObject, Notify)
	return S_OK;
}
HRESULT CManualMeasObject::XNotify::NotifyDestroy()
{
	METHOD_PROLOGUE_EX(CManualMeasObject, Notify)
	return S_OK;
}
HRESULT CManualMeasObject::XNotify::NotifyActivate( bool bActivate )
{
	METHOD_PROLOGUE_EX(CManualMeasObject, Notify)
	return S_OK;
}
HRESULT CManualMeasObject::XNotify::NotifySelect( bool bSelect )
{
	METHOD_PROLOGUE_EX(CManualMeasObject, Notify)

	if( bSelect )
	{
		INamedDataObject2Ptr	ptrParent( pThis->m_pParent );
		if( ptrParent ==0 )return S_FALSE;
		IUnknown	*punk = 0;
		ptrParent->GetParent( &punk );
		if( punk ==0 )return S_FALSE;
		ICalcObjectContainerPtr	ptrContainer( punk );
		punk->Release();
		if( ptrContainer==0 )return S_FALSE;
		long	lKey = pThis->m_lParamKey;
		ParameterContainer *p = 0;
		ptrContainer->ParamDefByKey( lKey, &p );
		if( p )ptrContainer->SetCurentPosition( p->lpos );

		POSITION	lpos = 0;
		INamedDataObject2Ptr	ptrList( ptrContainer );
		ptrParent->GetObjectPosInParent( &lpos ); 
		ptrList->SetActiveChild( lpos );
	}
	return S_OK;
}

HRESULT CManualMeasObject::XNotify::NotifyDataListChanged(int nCode, IUnknown *punkHint)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, Notify)
	if (nCode == 1)
	{
		if (GetObjectKey(punkHint) != GetObjectKey(pThis->GetControllingUnknown()))
		{
			long lChildren = 0;
			INamedDataObject2Ptr sptrNDO2(punkHint);
			sptrNDO2->GetChildsCount(&lChildren);
			if (lChildren < 2)
				pThis->CalcParallelLinesMatrix(NULL, punkHint);
		}
	}
	else
		pThis->CalcParallelLinesMatrix(NULL, NULL);
	return S_OK;
}

// native interface
HRESULT CManualMeasObject::XManualObj::GetParamInfo(long * plParamKey, DWORD * pdwType)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		HRESULT hr = S_OK;
		if (plParamKey)
			*plParamKey = pThis->m_lParamKey;
		
		if (pdwType)
			*pdwType = pThis->m_dwParamType;
		
		return hr;
	}
}

HRESULT CManualMeasObject::XManualObj::SetParamInfo(long * plParamKey, DWORD * pdwType)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		if (plParamKey)
			pThis->m_lParamKey = *plParamKey;

		if (pdwType)
			pThis->m_dwParamType = *pdwType;

		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::GetObjectType(DWORD * pdwType)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		if (!pdwType)
			return E_INVALIDARG;

		*pdwType = pThis->m_dwObjType;
		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::SetObjectType(DWORD dwType)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		pThis->m_dwObjType = dwType;
		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::SetValue(double fVal)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		return pThis->SetValue(fVal) ? S_OK : E_FAIL;
	}
}

HRESULT CManualMeasObject::XManualObj::GetValue(double * pfVal)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		if (!pfVal)
			return E_INVALIDARG;

		return pThis->GetValue(*pfVal) ? S_OK : E_FAIL;
	}
}

HRESULT CManualMeasObject::XManualObj::ExchangeData( unsigned long * pdwData, long bRead )
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		return pThis->ExchangeData(pdwData, bRead) ? S_OK : E_FAIL;
	}
}

HRESULT CManualMeasObject::XManualObj::CalcValue(double * pfVal)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		if (!pfVal)
			return pThis->CalcSetValues();
		else
			return pThis->CalcValue(*pfVal) ? S_OK : E_FAIL;
	}
}

HRESULT CManualMeasObject::XManualObj::UpdateParent()
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		pThis->UpdateParent();
		return S_OK;
	}
}

//paul : next 2 func. should be killed as posible
HRESULT CManualMeasObject::XManualObj::StartStopMeasure( long bStart )
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		//pThis->m_bMeasureRun = bStart == TRUE;
		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::IsProcessMeasure( long * pbRun )
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		if (!pbRun)
			return E_INVALIDARG;
		
		//*pbRun = pThis->m_bMeasureRun;
		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::SetOffset(POINT pt)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		return pThis->SetOffset(CPoint(pt)) ? S_OK : E_FAIL;
	}
}

HRESULT CManualMeasObject::XManualObj::CrossPoint( struct tagPOINT pt, long * pbReturn )
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		CPoint point(pt);
		if (!pbReturn)
			return E_INVALIDARG;

		*pbReturn = pThis->CrossZone(point);
		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::GetParametersCount(long *plParamsCount)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		if (pThis->m_dwObjType == emotParallelLines)
		{
			*plParamsCount = pThis->m_arrKeys.GetSize();
		}
		else
			*plParamsCount = 1;
		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::SetParametersCount(long lParamsCount)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		if (pThis->m_dwObjType == emotParallelLines)
		{
			pThis->m_arrKeys.SetSize(lParamsCount);
			pThis->m_arrValues.SetSize(lParamsCount);
		}
		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::GetParamKey(long lParam, long *plParamKey)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		if (pThis->m_dwObjType == emotParallelLines)
			*plParamKey = pThis->m_arrKeys[lParam];
		else
			*plParamKey = pThis->m_lParamKey;
		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::SetParamKey(long lParam, long lParamKey)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		if (pThis->m_dwObjType == emotParallelLines)
			pThis->m_arrKeys[lParam] = lParamKey;
		else
			pThis->m_lParamKey = lParamKey;
		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::SetValue2(long lParam, double fVal)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		if (pThis->m_dwObjType == emotParallelLines)
			pThis->m_arrValues[lParam] = fVal;
		else
			pThis->m_fValue = fVal;
		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::GetValue2(long lParam, double *pfVal)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		if (pThis->m_dwObjType == emotParallelLines)
			*pfVal = pThis->m_arrValues[lParam];
		else
			*pfVal = pThis->m_fValue;
		return S_OK;
	}
}

HRESULT CManualMeasObject::XManualObj::CalcAllValues()
{
	METHOD_PROLOGUE_EX(CManualMeasObject, ManualObj)
	{
		return S_OK;
	}
}

HRESULT CManualMeasObject::XClone::Clone(IUnknown** ppunkCloned)
{
	METHOD_PROLOGUE_EX(CManualMeasObject, Clone)
	{
		if (!ppunkCloned)
			return E_INVALIDARG;
		*ppunkCloned = pThis->Clone();
		return S_OK;
	}
}

	
//overrided virtuals
IUnknown* CManualMeasObject::Clone()
{
	IUnknown* punkCloned = CreateTypedObject(szTypeManualMeasObject);
	ICalibrPtr sptrCalibr(punkCloned);
	if (sptrCalibr != 0)
		sptrCalibr->SetCalibration(m_fCalibr, &m_guidC);
	IManualMeasureObjectPtr sptrMM(punkCloned);
	if(sptrMM != 0)
	{
		sptrMM->SetParamInfo(&m_lParamKey, &m_dwParamType);
		sptrMM->SetObjectType(m_dwObjType);
		sptrMM->SetValue(m_fValue);

		TExchangeMeasData Data;
		GetData(&Data);
		sptrMM->ExchangeData((DWORD*)&Data, FALSE);
		sptrMM->SetOffset(m_ptOffset);

	}
	return punkCloned;
}

DWORD CManualMeasObject::GetNamedObjectFlags()
{
	return nofNormal|nofStoreByParent|nofDefineParent;
	if( m_pParent )
	{
		ICalcObjectPtr	ptrCalc( m_pParent );
		long	lExist = 0;
		ptrCalc->GetFirstValuePos( &lExist );
		if( lExist != 0 )ptrCalc->GetNextValue( &lExist, 0, 0 );

		if( lExist )
			return nofNormal|nofStoreByParent;

		IMeasureObjectPtr	ptrMO( m_pParent );

		if( ptrMO != 0 )
		{
			IUnknown	*punkI = 0;
			ptrMO->GetImage( &punkI );
			if( punkI )
			{
				punkI->Release();
				return nofNormal|nofStoreByParent;
			}
		}

		long	lCount = 0;
		m_pParent->GetChildsCount( &lCount );

		if( lCount > 1 )
			return nofNormal|nofStoreByParent;			
	}

	return nofNormal|nofStoreByParent|nofDefineParent;
}

bool CManualMeasObject::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	if (!CDataObjectBase::SerializeObject(ar, pparams))
		return false;

	if (ar.IsStoring())
	{
		ar << (long)6;
		ar << m_lParamKey;
		ar << m_dwParamType;
		ar << m_dwObjType;
		ar << m_fValue;
		if (m_dwObjType == emotParallelLines)
		{
			long lKeysNum = m_arrKeys.GetSize();
			ar << lKeysNum;
			for (long i = 0; i < lKeysNum; i++)
			{
				long lKey = m_arrKeys[i];
				ar << lKey;
			}
			long lValuesNum = m_arrValues.GetSize();
			ar << lValuesNum;
			for (i = 0; i < lValuesNum; i++)
			{
				double dValue = m_arrValues[i];
				ar << dValue;
			}
		}
		ar << (long)m_bCalculated;
		ar << m_str;
		ar << m_fCalibr;
		ar.Write( &m_guidC, sizeof(m_guidC));
		SerializeData(ar);
	}
	else
	{
		long lVersion = 1;
		ar >> lVersion;
		if (lVersion > 0)
		{
			ar >> m_lParamKey;
			ar >> m_dwParamType;
			ar >> m_dwObjType;
		}
		if (lVersion > 1)
		{
			ar >> m_fValue;
			if (lVersion >= 6 && m_dwObjType == emotParallelLines)
			{
				long lKeysNum;
				ar >> lKeysNum;
				m_arrKeys.SetSize(lKeysNum);
				for (long i = 0; i < lKeysNum; i++)
				{
					long lKey;
					ar >> lKey;
					m_arrKeys[i] = lKey;
				}
				long lValuesNum;
				ar >> lValuesNum;
				m_arrValues.SetSize(lValuesNum);
				for (i = 0; i < lValuesNum; i++)
				{
					double dValue;
					ar >> dValue;
					m_arrValues[i] = dValue;
				}
			}

			if (lVersion > 2)
			{
				long	l;
				ar>>l;
				m_bCalculated = l != 0;
			}
			if( lVersion > 3 )
				ar >> m_str;
			if( lVersion > 4 )
			{
				ar >> m_fCalibr;
				ar.Read( &m_guidC, sizeof(m_guidC));
			}
			SerializeData(ar);
		}
	}
	return true;
}

bool  CManualMeasObject::SetOffset(CPoint pt)
{
//	CPoint delta(pt.x - m_ptOffset.x, pt.y - m_ptOffset.y);
//	m_ptOffset = pt;
	for (int i = 0; i < m_arrPoints.GetSize(); i++)
	{
		//paul 18.04.2003, cos m_arrPoints[3] - it's left, right angle BT 3103
		if( m_dwObjType == emotAngle3Point && i == 3 )
			continue;

		m_arrPoints[i].fX += (double)pt.x;
		m_arrPoints[i].fY += (double)pt.y;
	}

	m_pt1 += pt;
	m_pt2 += pt;

	return true;
}

bool CManualMeasObject::CrossZone(CPoint & pt)
{
	CRect rc = DoGetRect();
	rc.InflateRect( 5, 5 );

	if( !rc.PtInRect( pt ) )
		return false;

	int nWidth	= rc.Width();
	int nHeight	= rc.Height();

	if( nWidth < 1 || nHeight < 1 )
		return false;

	CDC* pDC = 0;
	CDC dc;
	CClientDC dcScreen( 0 );

	if( !dc.CreateCompatibleDC( &dcScreen ) )
		return false;

	pDC = &dc;//&dcScreen - for test purpose
	

	CBitmap bitmap;
	if( !bitmap.CreateCompatibleBitmap( &dcScreen, nWidth, nHeight ) )
		return false;

	CBitmap* pOldBitmap = (CBitmap*)pDC->SelectObject( &bitmap );
	pDC->FillRect( &CRect( 0, 0, nWidth, nHeight ), &CBrush( RGB( 0, 0, 0 ) ) );	


	//rc = ::ConvertToWindow( m_punkDrawTarget, rc );
	pDC->SetWindowOrg( rc.left, rc.top );	

	IUnknown* punkSaveTarget = m_punkDrawTarget;
	
	m_punkDrawTarget = 0;

	COLORREF clrTest = RGB( 255, 255, 255 );

	CPen pen( PS_SOLID, 5, clrTest );
	CPen * pOldPen = (CPen*)pDC->SelectObject( &pen );

	DoDraw( *pDC );

	CPoint ptTest = pt;//= ::ConvertToWindow( punkSaveTarget, pt );

	COLORREF clrRes = pDC->GetPixel( ptTest );

	m_punkDrawTarget = punkSaveTarget;

	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldBitmap );

	return clrRes!=0;
	return false;
}



bool  CManualMeasObject::GetValue(double & rfVal)
{
	rfVal = m_fValue;
	return true;
}

bool  CManualMeasObject::SetValue(double fVal)
{
	m_fValue = fVal;
	return true;
}

bool  CManualMeasObject::ExchangeData( unsigned long * pdwData, long bRead )
{
	if (!pdwData)
		return false;

	TExchangeMeasData * pData = (TExchangeMeasData*)pdwData;
	
	if (pData->dwObjectType != m_dwObjType)
		return false;

	return bRead ? GetData(pData) : SetData(pData);
}

void  CManualMeasObject::DoDraw(CDC &dc)
{
	m_nCrossOffset = ::GetValueInt(::GetAppUnknown(false), szManualMeasure, szMMCrossOffset, CROSS_OFFSET);
	m_nArcSize = ::GetValueInt(::GetAppUnknown(false), szManualMeasure, szMMArcSize, ARC_SIZE);

	if (m_dwObjType == emotUndefined || !m_arrPoints.GetSize())
		return;

	switch (m_dwObjType)
	{
	case emotLine: 
	case emotParallelLines:
		DrawLine(dc);
		break;

	case emotBrokenLine: 
		DrawBrokenLine(dc);
		break;

	case emotFreeLine:
		DrawFreeLine(dc);
		break;

	case emotSpline:
		DrawSpline(dc);
		break;

	case emotCycle3:
		DrawCycle3(dc);
		break;

	case emotCycle2:
		DrawCycle2(dc);
		break;

	case emotAngle2Line:
		DrawAngle2Line(dc);
		break;

	case emotAngle3Point:
		DrawAngle3Point(dc);
		break;

	case emotCount:
		DrawCount(dc);
		break;
	}

	//get the current pen
	if( ::GetValueInt(::GetAppUnknown(false), szIVImageView, szIVShowObjectNumber, 0) &&
		m_dwObjType != emotCount )
	{
		COLORREF clWhite = ::GetValueColor( ::GetAppUnknown(), "\\ImageView", "N_And_N_Color", RGB(255, 255, 255) );
		BOOL bAsClassColor = ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "N_And_N_AsClassColor", 0 ) == 1;

		HPEN	h = (HPEN)::SelectObject( dc, ::GetStockObject( BLACK_PEN ) );
		::SelectObject( dc, h );

		
		LOGPEN	pen;
		::GetObject( h, sizeof( pen ), &pen );
		
		CRect	rect( m_pt1.x, m_pt1.y, m_pt1.x, m_pt1.y );
		rect = ::ConvertToWindow(m_punkDrawTarget, rect );

		CFont	*pnew_font = 0;
		
		{
			// [vanek] BT2000:3814 используем экранное dc для пересчета - 06.07.2004
			CClientDC	dc_display( 0 );
			pnew_font = create_font( true, 
							-MulDiv( ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "N_And_N_FontSize", 10 ),
							dc_display.GetDeviceCaps( LOGPIXELSY ), 72) );	// [vanek] BT2000: 3560 - 12.01.2004
		}

		CString	s = get_number();
		dc.SetBkMode( TRANSPARENT );

		if( bAsClassColor )
			dc.SetTextColor( pen.lopnColor );
		else
			dc.SetTextColor( clWhite );

		CFont	*pold = dc.SelectObject( pnew_font );
		dc.DrawText( get_number(), &rect, DT_NOCLIP|DT_TOP|DT_SINGLELINE );
		dc.SelectObject( pold );

		if( pnew_font )
			delete pnew_font; pnew_font = 0;
	}
}


CRect CManualMeasObject::DoGetRect()
{
	CRect rc = NORECT;

	if (!m_dwObjType || !m_arrPoints.GetSize())
		return rc;

	m_nCrossOffset = ::GetValueInt(::GetAppUnknown(false), szManualMeasure, szMMCrossOffset, CROSS_OFFSET);
	m_nArcSize = ::GetValueInt(::GetAppUnknown(false), szManualMeasure, szMMArcSize, ARC_SIZE);

	switch (m_dwObjType)
	{
	case emotLine: 
	case emotBrokenLine: 
	case emotFreeLine:
	case emotCount:
	case emotAngle2Line:
	case emotParallelLines:
		CalcDrawRectLine(rc);
		break;

	case emotAngle3Point:
		CalcDrawRectAngle3Point(rc);
		break;

	case emotSpline:
		CalcDrawRectSpline(rc);
		break;

	case emotCycle3:
		CalcDrawRectCycle3(rc);
		break;

	case emotCycle2:
		CalcDrawRectCycle2(rc);
		break;

	default:
		return rc;
	}

	if (m_dwObjType == emotAngle3Point)
	{
		//paul 18.04.2003
		//rc.InflateRect(m_nArcSize, m_nArcSize, m_nArcSize, m_nArcSize);
	}
	else 
		rc.InflateRect(m_nCrossOffset, m_nCrossOffset, m_nCrossOffset, m_nCrossOffset);

	if( ::GetValueInt(::GetAppUnknown(false), szIVImageView, szIVShowObjectNumber, 0) &&
		m_dwObjType !=emotCount )
	{
		CRect	rc1 = CalcTextRect();
		rc.left = min( rc.left, rc1.left );
		rc.right = max( rc.right, rc1.right );
		rc.top = min( rc.top, rc1.top );
		rc.bottom = max( rc.bottom, rc1.bottom );
	}


	return rc;
}

bool CManualMeasObject::CalcDrawRectAngle3Point(CRect & rc)
{
	// find min & max points
	TMeasPoint min, max;

	min = m_arrPoints[0];
	max = m_arrPoints[0];
	if (m_arrPoints.GetSize() > 2)
	{
		for (int i = 1; i < 3; i++)
		{
			min.fX = (min.fX > m_arrPoints[i].fX) ? m_arrPoints[i].fX : min.fX;
			min.fY = (min.fY > m_arrPoints[i].fY) ? m_arrPoints[i].fY : min.fY;

			max.fX = (max.fX < m_arrPoints[i].fX) ? m_arrPoints[i].fX : max.fX;
			max.fY = (max.fY < m_arrPoints[i].fY) ? m_arrPoints[i].fY : max.fY;
		}
	}

	// transform it with corresponding calibration
	rc = CRect((int)min.fX, (int)min.fY, (int)max.fX, (int)max.fY);
	
	//paul 18.04.2003
	//rc.InflateRect(m_nArcSize, m_nArcSize, m_nArcSize, m_nArcSize);
	
	m_pt1 = m_pt2 = rc.CenterPoint();
	return true;
}


bool CManualMeasObject::CalcDrawRectLine(CRect & rc)
{
	// find min & max points
	TMeasPoint min, max;

	min = m_arrPoints[0];
	max = m_arrPoints[0];
	if (m_arrPoints.GetSize() > 1)
	{
		for (int i = 1; i < m_arrPoints.GetSize(); i++)
		{
			min.fX = (min.fX > m_arrPoints[i].fX) ? m_arrPoints[i].fX : min.fX;
			min.fY = (min.fY > m_arrPoints[i].fY) ? m_arrPoints[i].fY : min.fY;

			max.fX = (max.fX < m_arrPoints[i].fX) ? m_arrPoints[i].fX : max.fX;
			max.fY = (max.fY < m_arrPoints[i].fY) ? m_arrPoints[i].fY : max.fY;
		}
	}

	// transform it with corresponding calibration
	rc = CRect((int)min.fX, (int)min.fY, (int)max.fX, (int)max.fY);

	if( m_dwObjType == emotCount )
		rc.InflateRect( 0, 0, 15, 15 );

	if( m_dwObjType != emotAngle2Line &&
		m_dwObjType != emotCount )
	{
		if( m_arrPoints.GetSize() > 0 )
		{
			m_pt1.x = int( m_arrPoints[0].fX );
			m_pt1.y = int( m_arrPoints[0].fY );
			m_pt2.x = int( m_arrPoints[m_arrPoints.GetSize()-1].fX );
			m_pt2.y = int( m_arrPoints[m_arrPoints.GetSize()-1].fY );

			if( m_pt1.x > m_pt2.x )
			{
				CPoint	pt = m_pt1;
				m_pt1 = m_pt2;
				m_pt2 = pt;
			}
		}
	}
	else
	{
		m_pt1 = m_pt2 = rc.CenterPoint();
	}

	return true;
}

bool CManualMeasObject::CalcDrawRectSpline(CRect & rc)
{
	if (m_arrPoints.GetSize() < 2)
		return true;

	m_pt1.x = int( m_arrPoints[0].fX );
	m_pt1.y = int( m_arrPoints[0].fY );
	m_pt2.x = int( m_arrPoints[m_arrPoints.GetSize()-1].fX );
	m_pt2.y = int( m_arrPoints[m_arrPoints.GetSize()-1].fY );

	TMeasPoint minPt, maxPt;

	long n = 0;
	long nContourSize = m_arrPoints.GetSize();
	int	nSize = 20 * (nContourSize + 1);

	TMeasPoint * ptrPts = (TMeasPoint*)new TMeasPoint [nSize];
	if (!ptrPts)
		return false;

	int	 nPtsCount = 0;
	int	 nMaxIndex = nContourSize + 1;

	for ( ; n < nMaxIndex; n++)
	{
		//add spline points
		int	idx1 = n - 2;
		int	idx2 = n - 1;
		int	idx3 = n;
		int	idx4 = n + 1;

		if (idx1 < 0)
			idx1 = 0;
		if (idx2 < 0)
			idx2 = 0;
		if (idx3 >= nContourSize)
			idx3 = nContourSize - 1;
		if (idx4 >= nContourSize)
			idx4 = nContourSize - 1;

		ASSERT(idx1 >= 0); ASSERT(idx1 < nContourSize);
		ASSERT(idx3 >= 0); ASSERT(idx3 < nContourSize);
		ASSERT(idx4 >= 0); ASSERT(idx4 < nContourSize);

		double	a0, a1, a2, a3;
		double	b0, b1, b2, b3;
		double	xa, xb, xc, xd;
		double	ya, yb, yc, yd;

		xa = m_arrPoints[idx1].fX;
		xb = m_arrPoints[idx2].fX;
		xc = m_arrPoints[idx3].fX;
		xd = m_arrPoints[idx4].fX;

		ya = m_arrPoints[idx1].fY;
		yb = m_arrPoints[idx2].fY;
		yc = m_arrPoints[idx3].fY;
		yd = m_arrPoints[idx4].fY;

		a3 = (-xa + 3 * (xb - xc) + xd) / 6.0;
		a2 = (xa - 2 * xb + xc) / 2.0;
		a1 = (xc - xa) / 2.0;
		a0 = (xa + 4 * xb + xc) / 6.0;

		b3 = (-ya + 3 * (yb - yc) + yd) / 6.0;
		b2 = (ya - 2 * yb + yc) / 2.0;
		b1 = (yc - ya) / 2.0;
		b0 = (ya + 4 * yb + yc) / 6.0;

		for (double t = 0; t < 1.0; t += 0.05)
		{
			ptrPts[nPtsCount].fX = ((a3 * t + a2) * t + a1) * t + a0;
			ptrPts[nPtsCount].fY = ((b3 * t + b2) * t + b1) * t + b0;
			nPtsCount++;
		}

		// add length of piece to common length
		TMeasPoint min, max;

		minPt.fX = min.fX = ptrPts[0].fX;
		minPt.fY = min.fY = ptrPts[0].fY;

		maxPt.fX = max.fX = ptrPts[0].fX;
		maxPt.fY = max.fY = ptrPts[0].fY;

		for (int i = 1; i < nPtsCount; i++)
		{
			min.fX = (min.fX > ptrPts[i].fX) ? ptrPts[i].fX : min.fX;
			min.fY = (min.fY > ptrPts[i].fY) ? ptrPts[i].fY : min.fY;

			max.fX = (max.fX < ptrPts[i].fX) ? ptrPts[i].fX : max.fX;
			max.fY = (max.fY < ptrPts[i].fY) ? ptrPts[i].fY : max.fY;
		}

		minPt.fX = (minPt.fX > min.fX) ? min.fX : minPt.fX;
		minPt.fY = (minPt.fY > min.fY) ? min.fY : minPt.fY;

		maxPt.fX = (maxPt.fX < max.fX) ? max.fX : maxPt.fX;
		maxPt.fY = (maxPt.fY < max.fY) ? max.fY : maxPt.fY;
	}
	delete [] ptrPts;
	
	rc.left   = (int)minPt.fX;
	rc.top    = (int)minPt.fY;
	rc.right  = (int)maxPt.fX;
	rc.bottom = (int)maxPt.fY;

	return true;
}

bool CManualMeasObject::CalcDrawRectCycle3(CRect & rc)
{
	CPoint ptc;
	bool bRet = CalcCycleRect(rc, ptc);
	m_pt1 = m_pt2 = rc.CenterPoint();
	return bRet;
}

bool CManualMeasObject::CalcDrawRectCycle2(CRect & rc)
{
	if (m_arrPoints.GetSize() < 2)
		return false;

	CPoint pt1((int)m_arrPoints[0].fX, (int)m_arrPoints[0].fY);

	CPoint pt2((int)m_arrPoints[1].fX, (int)m_arrPoints[1].fY);

	CPoint ptc((pt1.x + pt2.x) >> 1, (pt1.y + pt2.y) >> 1);

	double dx = (m_arrPoints[0].fX - m_arrPoints[1].fX) * m_fCalibr;
	double dy = (m_arrPoints[0].fY - m_arrPoints[1].fY) * m_fCalibr;

	double fRadius = ::sqrt(dx * dx + dy * dy) / 2.;

	rc.left   = int(ptc.x - fRadius / m_fCalibr - .5);
	rc.right  = int(ptc.x + fRadius / m_fCalibr - .5);
	rc.top    = int(ptc.y - fRadius / m_fCalibr + .5);
	rc.bottom = int(ptc.y + fRadius / m_fCalibr + .5);

	m_pt1 = m_pt2 = rc.CenterPoint();

	return true;
}

bool CManualMeasObject::GetData(TExchangeMeasData * pData)
{
	if (!pData)
		return false;

	pData->lNumPoints = 0;
	pData->pPoints = 0;
	pData->dwObjectType = m_dwObjType;

	int nCount = m_arrPoints.GetSize();
	if (!nCount)
		return true;

	pData->pPoints = (TMeasPoint*)CoTaskMemAlloc(nCount * sizeof(TMeasPoint));
	if (!pData->pPoints)
		return false;
 
	pData->lNumPoints = nCount;
	
	for (int i = 0; i < nCount; i++)
		pData->pPoints[i] = m_arrPoints[i];

	return true;
}

bool CManualMeasObject::SetData(TExchangeMeasData * pData)
{
	if (!pData)
		return false;

	m_arrPoints.RemoveAll();
	int nCount = (int)pData->lNumPoints;
	if (nCount <= 0)
		return true;

	if (!pData->pPoints)
		return false;

	m_arrPoints.SetSize(nCount);

	for (int i = 0; i < nCount; i++)
		m_arrPoints.SetAt(i, pData->pPoints[i]);

	CoTaskMemFree((LPVOID)pData->pPoints);
	pData->pPoints = 0;
	pData->lNumPoints = 0;

	if( m_dwObjType != emotAngle2Line &&
		m_dwObjType != emotCount &&
		m_arrPoints.GetSize() > 0 )
	{
		m_pt1.x = int( m_arrPoints[0].fX );
		m_pt1.y = int( m_arrPoints[0].fY );
		m_pt2.x = int( m_arrPoints[m_arrPoints.GetSize()-1].fX );
		m_pt2.y = int( m_arrPoints[m_arrPoints.GetSize()-1].fY );
	}
	
	return CalcValue(m_fValue);
}

void CManualMeasObject::SerializeData(CStreamEx & ar)
{
	if (ar.IsStoring())
	{
		int nCount = m_arrPoints.GetSize();
		ar << nCount;
		for (int i = 0; i < nCount; i++)
		{
			ar << m_arrPoints[i].fX;
			ar << m_arrPoints[i].fY;
		}
	}
	else
	{
		m_arrPoints.RemoveAll();
		int nCount;
		ar >> nCount;
		if (nCount > 0)
		{
			m_arrPoints.SetSize(nCount);
			for (int i = 0; i < nCount; i++)
			{
				ar >> m_arrPoints[i].fX;
				ar >> m_arrPoints[i].fY;
			}
		}
	}
}

bool  CManualMeasObject::CalcValue(double & rfVal)
{
	m_bCalculated =  true;

	switch (m_dwObjType)
	{
	case emotLine: 
	case emotBrokenLine: 
	case emotFreeLine:
	case emotParallelLines:
		return CalcLength(rfVal);
		break;

	case emotSpline:
		return CalcSpline(rfVal);
		break;

	case emotCycle3:
	case emotCycle2:
		return CalcRadius(rfVal);
		break;

	case emotAngle2Line:
		return CalcAngle2Line(rfVal);
		break;

	case emotAngle3Point:
		return CalcAngle3Point(rfVal);
		break;

	case emotCount:
		return CalcCount(rfVal);
		break;
	}

	return false;
}

bool  CManualMeasObject::CalcSetValues()
{
	ICalcObjectPtr sptrCOPar(m_pParent);
	if (sptrCOPar == 0) return false;
	if (m_dwObjType == emotParallelLines)
		CalcParallelLinesMatrix(sptrCOPar,NULL);
	else
	{
		double d;
		if (CalcValue(d))
			sptrCOPar->SetValue(m_lParamKey, d);
	}
	return true;
}

bool CManualMeasObject::CalcAngle2Line(double & rfVal)
{
	TMeasPoint pt1, pt2, pt3, pt4;
	if (m_arrPoints.GetSize() < 4 || m_dwObjType != emotAngle2Line)
		return false;
	
	pt1 = m_arrPoints[0];
	pt2 = m_arrPoints[1];
	pt3 = m_arrPoints[2];
	pt4 = m_arrPoints[3];

	rfVal = 0;
/*	int nSz = 4;
	for(int i = 0;i < nSz; i++)
	{
		for(int j = 0;j < nSz; j++)
		{
			if( j == i )
				continue;

			if( m_arrPoints[i].fX == m_arrPoints[j].fX && m_arrPoints[i].fY == m_arrPoints[j].fY )
				return true;
		}
	}
*/
	double dx1 = (pt2.fX - pt1.fX) * m_fCalibr;
	double dy1 = (pt2.fY - pt1.fY) * m_fCalibr;
	double dx2 = (pt4.fX - pt3.fX) * m_fCalibr;
	double dy2 = (pt4.fY - pt3.fY) * m_fCalibr;

	double fA1 = ::atan2(dy1, dx1);
	double fA2 = ::atan2(dy2, dx2);
	
	rfVal = fA2 - fA1;

	if (rfVal < 0.) 
		rfVal += 2 * PI;

	if (rfVal > 2 * PI) 
		rfVal -= 2 * PI;
	
	if (rfVal > PI) 
		rfVal = -2 * PI + rfVal;

	rfVal = ::fabs(rfVal);

	//rfVal = 180. * rfVal / PI;
	return true;
}

bool CManualMeasObject::CalcAngle3Point(double & rfVal)
{
	TMeasPoint pt1, pt2, pt3;
	
	if (m_arrPoints.GetSize() < 4 || m_dwObjType != emotAngle3Point)
		return false;

	pt1 = m_arrPoints[0];
	pt2 = m_arrPoints[1];
	pt3 = m_arrPoints[2];

	rfVal = 0;
	int nSz = 3;
	for(int i = 0;i < nSz; i++)
	{
		for(int j = 0;j < nSz; j++)
		{
			if( j == i )
				continue;

			if( m_arrPoints[i].fX == m_arrPoints[j].fX && m_arrPoints[i].fY == m_arrPoints[j].fY )
				return true;
		}
	}

	bool bFlag = m_arrPoints[3].fX > 0.; 

	double dx1 = (pt1.fX - pt2.fX) * m_fCalibr;
	double dy1 = (pt1.fY - pt2.fY) * m_fCalibr;
	double dx2 = (pt3.fX - pt2.fX) * m_fCalibr;
	double dy2 = (pt3.fY - pt2.fY) * m_fCalibr;

	double fA1 = ::atan2(dy1, dx1);
	double fA2 = ::atan2(dy2, dx2);
	
	rfVal = fA2 - fA1;

	if (rfVal < 0.) 
		rfVal += 2 * PI;

	if (rfVal > 2 * PI) 
		rfVal -= 2 * PI;

	if (!bFlag)
		rfVal = 2 * PI - rfVal;

	//rfVal = 180. * rfVal / PI;
//	TRACE ("Angle : %d\n", (int)rfVal);

	return true;
}

double CManualMeasObject::CalcAngles(double & rfA1, double & rfA2)
{
	TMeasPoint pt1, pt2, pt3;
	
	if (m_arrPoints.GetSize() < 3)
		return 0;

	pt1 = m_arrPoints[0];
	pt2 = m_arrPoints[1];
	pt3 = m_arrPoints[2];

	bool bFlag = m_arrPoints[3].fX > 0.; 
	double dx1 = (pt1.fX - pt2.fX) * m_fCalibr;
	double dy1 = (pt1.fY - pt2.fY) * m_fCalibr;
	double dx2 = (pt3.fX - pt2.fX) * m_fCalibr;
	double dy2 = (pt3.fY - pt2.fY) * m_fCalibr;

	rfA1 = ::atan2(dy1, dx1);
	rfA2 = ::atan2(dy2, dx2);
	
	double fVal = rfA2 - rfA1;

	if (fVal < 0.) 
		fVal += 2 * PI;

	if (fVal > 2 * PI) 
		fVal -= 2 * PI;

	if (!bFlag)
		fVal = 2 * PI - fVal;

	return fVal;
}

bool CManualMeasObject::CalcRadius(double & rfVal)
{
	if (m_dwObjType == emotCycle3)
		return CalcRadius3Point(rfVal);

	else if (m_dwObjType == emotCycle2)
		return CalcRadius2Point(rfVal);

	return false;
}

bool CManualMeasObject::CalcRadius2Point(double & rfVal)
{
	if (m_arrPoints.GetSize() < 2)
		return false;

	rfVal = 0;

	TMeasPoint pt1, pt2;
	pt1 = m_arrPoints[0];
	pt2 = m_arrPoints[1];

	if (pt1.fX == pt2.fX && pt1.fY == pt2.fY)
		return true;

	double dx = (pt1.fX - pt2.fX) * m_fCalibr;
	double dy = (pt1.fY - pt2.fY) * m_fCalibr;
	rfVal = ::sqrt(dx * dx + dy * dy) / 2.;//????????????????????
//	TRACE ("Radius %f\n", rfVal);
	return true;
}

bool CManualMeasObject::CalcRadius3Point(double & rfVal)
{
	if (m_arrPoints.GetSize() < 3)
		return false;

	rfVal = 0;

	TMeasPoint pt1, pt2, pt3;

	pt1 = m_arrPoints[0];
	pt2 = m_arrPoints[1];
	pt3 = m_arrPoints[2];


	if (pt1.fX == pt2.fX && pt1.fY == pt2.fY || 
		pt2.fX == pt3.fX && pt2.fY == pt3.fY || 
		pt1.fX == pt3.fX && pt1.fY == pt3.fY)
	{
		rfVal = 0;
		return true;
	}

	double x1, x2, x3, y1, y2, y3; //Координаты в объекте
	double x1c, x2c, xc, y1c, y2c, yc; //Координаты центров хорд и окружности
	double a1, b1, c1, a2, b2, c2;  //параметры уравнений прямых
	double a1p, b1p, c1p, a2p, b2p, c2p;  //параметры уравнений перпендик. прямых
	double delta, deltaX, deltaY; //Определители
 
//Определение координат точек в объекте
	x1 = pt1.fX * m_fCalibr;
	x2 = pt2.fX * m_fCalibr;
	x3 = pt3.fX * m_fCalibr;
	y1 = pt1.fY * m_fCalibr;
	y2 = pt2.fY * m_fCalibr;
	y3 = pt3.fY * m_fCalibr;

	x1c = (x1 + x2) / 2.0;
	x2c = (x3 + x2) / 2.0;
	y1c = (y1 + y2) / 2.0;
	y2c = (y3 + y2) / 2.0;
//Определение уравнения прямых, проходящих через эти точки, вида ax+by+c = 0
	if (::fabs(x1 - x2) > fabs(y1 - y2))
	{
		b1 = 1.0;
		a1 = -(y2 - y1) / (x2 - x1);
		c1 = -a1 * x1 - b1 * y1;
	}
	else
	{
		a1 = 1.0;
		b1 = -(x2 - x1) / (y2 - y1);
		c1 = -a1 * x1 - b1 * y1;
	}
 
	if (::fabs(x3 - x2) > fabs(y3 - y2))
	{
		b2 = 1;
		a2 = -(y2 - y3) / (x2 - x3);
		c2 = -a2 * x3 - b2 * y3;
	}
	else
	{
		a2 = 1;
		b2 = -(x2 - x3) / (y2 - y3);
		c2 = -a2 * x3 - b2 * y3;
	}
//Определение уравнений прямых, проходящих через центры хорд и перпендикулярных им
	a1p = -b1;
	b1p = a1;
	c1p =  -(a1p * x1c + b1p * y1c);
 
	a2p = -b2;
	b2p = a2;
	c2p =  -(a2p * x2c + b2p * y2c);
//Центр окружности лежит на пересечении этих прямых => ищем точку пересечения
	delta = a1p * b2p - a2p * b1p;
	deltaX = -c1p * b2p + c2p * b1p;
	deltaY = -a1p * c2p + a2p * c1p;
 
	xc = deltaX / delta;
	yc = deltaY / delta;

//Это и есть радиус
	double fx = (xc - x1); //????????????????????????????
	double fy = (yc - y1); //????????????????????????????
	rfVal =  ::sqrt(fx * fx + fy * fy);

	return true;
}

bool CManualMeasObject::CalcLength(double & rfVal)
{
	rfVal = 0.;
	if (m_arrPoints.GetSize() < 2)
		return true;

	double	f = 0;

	TMeasPoint pt = m_arrPoints[0];
	for (int i = 1; i < m_arrPoints.GetSize(); i++)
	{
		TMeasPoint ptNext = m_arrPoints[i];
		double dx = (ptNext.fX - pt.fX);
		double dy = (ptNext.fY - pt.fY);

		f+=::sqrt(dx * dx + dy * dy);;

		dx *= m_fCalibr;dy *= m_fCalibr;
		rfVal += ::sqrt(dx * dx + dy * dy);
		
		pt = ptNext;
	}

	::SetValue(::GetAppUnknown(false), szManualMeasure, "LastPixelLength", f);
	
	return true;
}

bool CManualMeasObject::CalcCount(double & rfVal)
{
	rfVal = (double)m_arrPoints.GetSize();
	return true;
}

bool CManualMeasObject::CalcSpline(double & rfVal)
{
	rfVal = 0;

	if (m_arrPoints.GetSize() < 2)
		return true;

	long n = 0;
	long nContourSize = m_arrPoints.GetSize();
	int	 nSize = 20; 

	TMeasPoint * ptrPts = (TMeasPoint*)new TMeasPoint [nSize+1];
	if (!ptrPts)
		return false;

	int	 nPtsCount = 0;
	int	 nMaxIndex = nContourSize + 1;

	for ( ; n < nMaxIndex; n++)
	{
		//add spline points
		int	idx1 = n - 2;
		int	idx2 = n - 1;
		int	idx3 = n;
		int	idx4 = n + 1;

		if (idx1 < 0)
			idx1 = 0;
		if (idx2 < 0)
			idx2 = 0;
		if (idx3 >= nContourSize)
			idx3 = nContourSize - 1;
		if (idx4 >= nContourSize)
			idx4 = nContourSize - 1;

		ASSERT(idx1 >= 0); ASSERT(idx1 < nContourSize);
		ASSERT(idx3 >= 0); ASSERT(idx3 < nContourSize);
		ASSERT(idx4 >= 0); ASSERT(idx4 < nContourSize);

		double	a0, a1, a2, a3;
		double	b0, b1, b2, b3;
		double	xa, xb, xc, xd;
		double	ya, yb, yc, yd;

		xa = m_arrPoints[idx1].fX;
		xb = m_arrPoints[idx2].fX;
		xc = m_arrPoints[idx3].fX;
		xd = m_arrPoints[idx4].fX;

		ya = m_arrPoints[idx1].fY;
		yb = m_arrPoints[idx2].fY;
		yc = m_arrPoints[idx3].fY;
		yd = m_arrPoints[idx4].fY;

		a3 = (-xa + 3 * (xb - xc) + xd) / 6.0;
		a2 = (xa - 2 * xb + xc) / 2.0;
		a1 = (xc - xa) / 2.0;
		a0 = (xa + 4 * xb + xc) / 6.0;

		b3 = (-ya + 3 * (yb - yc) + yd) / 6.0;
		b2 = (ya - 2 * yb + yc) / 2.0;
		b1 = (yc - ya) / 2.0;
		b0 = (ya + 4 * yb + yc) / 6.0;

		nPtsCount = 0;

		int	idx_p = 0;
		double t = 0;
		for( ; idx_p <= nSize; t += 1./nSize, idx_p++ )
		{
			ptrPts[nPtsCount].fX = ((a3 * t + a2) * t + a1) * t + a0;
			ptrPts[nPtsCount].fY = ((b3 * t + b2) * t + b1) * t + b0;
			nPtsCount++;
		}


		// add length of piece to common length

		TMeasPoint ptStart = ptrPts[0];
		double fRes = 0;

		for (int i = 1; i < nPtsCount; i++)
		{
			double dx = (ptrPts[i].fX - ptStart.fX) * m_fCalibr;
			double dy = (ptrPts[i].fY - ptStart.fY) * m_fCalibr;
			fRes += ::sqrt(dx * dx + dy * dy);

			ptStart = ptrPts[i];
		}
		rfVal += fRes;

	}
	delete [] ptrPts;

	return true;
}

// drawing operation
void CManualMeasObject::DrawLine(CDC & dc)
{
	if (m_arrPoints.GetSize() < 2)
		return;

	// start point
	CPoint pointStart;
	pointStart.x = (int)m_arrPoints[0].fX;//TransformValue(m_arrPoints[0].fX, m_fCalibr, m_fXOffset);
	pointStart.y = (int)m_arrPoints[0].fY;//TransformValue(m_arrPoints[0].fY, m_fCalibr, m_fYOffset);


	pointStart = ::ConvertToWindow(m_punkDrawTarget, pointStart);
	DrawCross(dc, pointStart);

	// draw next point marker and line
	for (int i = 1; i < m_arrPoints.GetSize(); i++)
	{
		CPoint point;
		point.x = (int)m_arrPoints[i].fX;//TransformValue(m_arrPoints[i].fX, m_fCalibr, m_fXOffset);
		point.y = (int)m_arrPoints[i].fY;//TransformValue(m_arrPoints[i].fY, m_fCalibr, m_fYOffset);

		point = ::ConvertToWindow(m_punkDrawTarget, point);
		DrawCross(dc, point);

		dc.MoveTo(pointStart);
		dc.LineTo(point);

		pointStart = point;
	}
}

void CManualMeasObject::DrawBrokenLine(CDC & dc)
{
	if (m_arrPoints.GetSize() < 2)
		return;

	// start point
	CPoint pointStart;
	pointStart.x = (int)m_arrPoints[0].fX;//TransformValue(m_arrPoints[0].fX, m_fCalibr, m_fXOffset);
	pointStart.y = (int)m_arrPoints[0].fY;//TransformValue(m_arrPoints[0].fY, m_fCalibr, m_fYOffset);
	// convert point's coordinates & draw marker for start point
	//if (m_punkDrawTarget)
	{
		pointStart = ::ConvertToWindow(m_punkDrawTarget, pointStart);
		DrawCross(dc, pointStart);
	}

	// draw next point marker and line from start point to next point
	for (int i = 1; i < m_arrPoints.GetSize(); i++)
	{
		CPoint point;
		point.x = (int)m_arrPoints[i].fX;//TransformValue(m_arrPoints[i].fX, m_fCalibr, m_fXOffset);
		point.y = (int)m_arrPoints[i].fY;//TransformValue(m_arrPoints[i].fY, m_fCalibr, m_fYOffset);

		//if (m_punkDrawTarget)
		{
			point = ::ConvertToWindow(m_punkDrawTarget, point);
			DrawCross(dc, point);

			dc.MoveTo(pointStart);
			dc.LineTo(point);
		}
		// set next point to start point
		pointStart = point;
	}
}

void CManualMeasObject::DrawFreeLine(CDC & dc)
{
	if (m_arrPoints.GetSize() < 2)
		return;

	// start point
	CPoint pointStart;
	pointStart.x = (int)m_arrPoints[0].fX;//TransformValue(m_arrPoints[0].fX, m_fCalibr, m_fXOffset);
	pointStart.y = (int)m_arrPoints[0].fY;//TransformValue(m_arrPoints[0].fY, m_fCalibr, m_fYOffset);
	// convert point's coordinates & draw marker for start point
	//if (m_punkDrawTarget)
	{
		pointStart = ::ConvertToWindow(m_punkDrawTarget, pointStart);
		DrawCross(dc, pointStart);
	}

	// draw line from start point to next point
	for (int i = 1; i < m_arrPoints.GetSize(); i++)
	{
		CPoint point;
		point.x = (int)m_arrPoints[i].fX;//TransformValue(m_arrPoints[i].fX, m_fCalibr, m_fXOffset);
		point.y = (int)m_arrPoints[i].fY;//TransformValue(m_arrPoints[i].fY, m_fCalibr, m_fYOffset);

		//if (m_punkDrawTarget)
		{
			point = ::ConvertToWindow(m_punkDrawTarget, point);

			dc.MoveTo(pointStart);
			dc.LineTo(point);
		}
		// set next point to start point
		pointStart = point;
	}
	// draw las point marker
	//if (m_punkDrawTarget)
		DrawCross(dc, pointStart);
}

void CManualMeasObject::DrawSpline(CDC & dc)
{
	if (m_arrPoints.GetSize() < 2)
		return;

	Contour * pContur = ContourCreate();
	if (!pContur)
		return;

	for (int i = 0; i < m_arrPoints.GetSize(); i++)
	{

		CPoint point;
		point.x = (int)m_arrPoints[i].fX;//TransformValue(m_arrPoints[i].fX, m_fCalibr, m_fXOffset);
		point.y = (int)m_arrPoints[i].fY;//TransformValue(m_arrPoints[i].fY, m_fCalibr, m_fYOffset);

		ContourAddPoint(pContur, point.x, point.y);
		
		//if (m_punkDrawTarget)
		{
			point = ::ConvertToWindow(m_punkDrawTarget, point);
			DrawCross(dc, point);
		}
	}

	ContourDraw(dc, pContur, m_punkDrawTarget, cdfSpline);//|cdfClosed);
	ContourDelete(pContur);
}

void CManualMeasObject::DrawCycle3(CDC & dc)
{
	if (m_arrPoints.GetSize() < 3)
		return;

	CPoint pt1((int)m_arrPoints[0].fX, (int)m_arrPoints[0].fY);
//	CPoint pt1(TransformValue(m_arrPoints[0].fX, m_fCalibr, m_fXOffset), 
//			   TransformValue(m_arrPoints[0].fY, m_fCalibr, m_fYOffset));

	CPoint pt2((int)m_arrPoints[1].fX, (int)m_arrPoints[1].fY);
//	CPoint pt2(TransformValue(m_arrPoints[1].fX, m_fCalibr, m_fXOffset), 
//			   TransformValue(m_arrPoints[1].fY, m_fCalibr, m_fYOffset));

	CPoint pt3((int)m_arrPoints[2].fX, (int)m_arrPoints[2].fY);
//	CPoint pt3(TransformValue(m_arrPoints[2].fX, m_fCalibr, m_fXOffset), 
//			   TransformValue(m_arrPoints[2].fY, m_fCalibr, m_fYOffset));

	CPoint ptc;

	//if (m_punkDrawTarget)
	{
		pt1 = ::ConvertToWindow(m_punkDrawTarget, pt1);
		pt2 = ::ConvertToWindow(m_punkDrawTarget, pt2);
		pt3 = ::ConvertToWindow(m_punkDrawTarget, pt3);

		DrawCross(dc, pt1);
		DrawCross(dc, pt2);
		DrawCross(dc, pt3);

		CRect	rect;
		if (!CalcCycleRect(rect, ptc))
			return;
		
		ptc	 = ::ConvertToWindow(m_punkDrawTarget, ptc);
		rect = ::ConvertToWindow(m_punkDrawTarget, rect);

		dc.Ellipse(rect);
		DrawCross(dc, ptc);

		// draw line
		dc.MoveTo(ptc);
		dc.LineTo(pt3);
	}
}

bool CManualMeasObject::CalcCycleRect(CRect & rect, CPoint & ptc)
{
	double m_fRadius = 0; 
	ptc = CPoint(-1, -1);
	rect.SetRectEmpty();

	if (m_arrPoints.GetSize() < 3)
		return false;

	TMeasPoint pt1, pt2, pt3;

	pt1 = m_arrPoints[0];
	pt2 = m_arrPoints[1];
	pt3 = m_arrPoints[2];


	if (pt1.fX == pt2.fX && pt1.fY == pt2.fY || 
		pt2.fX == pt3.fX && pt2.fY == pt3.fY || 
		pt1.fX == pt3.fX && pt1.fY == pt3.fY)
		return false;

	double x1, x2, x3, y1, y2, y3; //Координаты в объекте
	double x1c, x2c, xc, y1c, y2c, yc; //Координаты центров хорд и окружности
	double a1, b1, c1, a2, b2, c2;  //параметры уравнений прямых
	double a1p, b1p, c1p, a2p, b2p, c2p;  //параметры уравнений перпендик. прямых
	double delta, deltaX, deltaY; //Определители
 
//Определение координат точек в объекте
	x1 = pt1.fX * m_fCalibr;
	x2 = pt2.fX * m_fCalibr;
	x3 = pt3.fX * m_fCalibr;
	y1 = pt1.fY * m_fCalibr;
	y2 = pt2.fY * m_fCalibr;
	y3 = pt3.fY * m_fCalibr;

	x1c = (x1 + x2) / 2.0;
	x2c = (x3 + x2) / 2.0;
	y1c = (y1 + y2) / 2.0;
	y2c = (y3 + y2) / 2.0;
//Определение уравнения прямых, проходящих через эти точки, вида ax+by+c = 0
	if (::fabs(x1 - x2) > fabs(y1 - y2))
	{
		b1 = 1.0;
		a1 = -(y2 - y1) / (x2 - x1);
		c1 = -a1 * x1 - b1 * y1;
	}
	else
	{
		a1 = 1.0;
		b1 = -(x2 - x1) / (y2 - y1);
		c1 = -a1 * x1 - b1 * y1;
	}
 
	if (::fabs(x3 - x2) > fabs(y3 - y2))
	{
		b2 = 1;
		a2 = -(y2 - y3) / (x2 - x3);
		c2 = -a2 * x3 - b2 * y3;
	}
	else
	{
		a2 = 1;
		b2 = -(x2 - x3) / (y2 - y3);
		c2 = -a2 * x3 - b2 * y3;
	}
//Определение уравнений прямых, проходящих через центры хорд и перпендикулярных им
	a1p = -b1;
	b1p = a1;
	c1p =  -(a1p * x1c + b1p * y1c);
 
	a2p = -b2;
	b2p = a2;
	c2p =  -(a2p * x2c + b2p * y2c);
//Центр окружности лежит на пересечении этих прямых => ищем точку пересечения
	delta = a1p * b2p - a2p * b1p;
	deltaX = -c1p * b2p + c2p * b1p;
	deltaY = -a1p * c2p + a2p * c1p;
 
	xc = deltaX / delta;
	yc = deltaY / delta;

//Это и есть центр окружности и ее радиус
	double dx = (xc - x1);
	double dy = (yc - y1);
	m_fRadius =  ::sqrt(dx * dx + dy * dy);

	ptc.x = int(xc / m_fCalibr);
	ptc.y = int(yc / m_fCalibr);
 
	rect.left	= int(ptc.x - m_fRadius / m_fCalibr);
	rect.right	= int(ptc.x + m_fRadius / m_fCalibr);
	rect.top	= int(ptc.y - m_fRadius / m_fCalibr);
	rect.bottom = int(ptc.y + m_fRadius / m_fCalibr);

	return true;
}

void CManualMeasObject::DrawCycle2(CDC & dc)
{
	if (m_arrPoints.GetSize() < 2)
		return;

	CRect	rect;

	CPoint pt1((int)m_arrPoints[0].fX, (int)m_arrPoints[0].fY);
//	CPoint pt1(TransformValue(m_arrPoints[0].fX, m_fCalibr, m_fXOffset), 
//			   TransformValue(m_arrPoints[0].fY, m_fCalibr, m_fYOffset));

	CPoint pt2((int)m_arrPoints[1].fX, (int)m_arrPoints[1].fY);
//	CPoint pt2(TransformValue(m_arrPoints[1].fX, m_fCalibr, m_fXOffset), 
//			   TransformValue(m_arrPoints[1].fY, m_fCalibr, m_fYOffset));

	CPoint ptc((pt1.x + pt2.x) >> 1, (pt1.y + pt2.y) >> 1);

	double dx = (m_arrPoints[0].fX - m_arrPoints[1].fX) * m_fCalibr;
	double dy = (m_arrPoints[0].fY - m_arrPoints[1].fY) * m_fCalibr;
	double fRadius = ::sqrt(dx * dx + dy * dy) / 2.;

	//if (m_punkDrawTarget)
	{
		//??????????????????????????????????????????????????
		rect.left = int(ptc.x - fRadius / m_fCalibr + .5);
		rect.right = int(ptc.x + fRadius / m_fCalibr + .5);
		rect.top = int(ptc.y - fRadius / m_fCalibr + .5);
		rect.bottom = int(ptc.y + fRadius / m_fCalibr + .5);

		pt1 = ::ConvertToWindow(m_punkDrawTarget, pt1);
		pt2 = ::ConvertToWindow(m_punkDrawTarget, pt2);
		ptc = ::ConvertToWindow(m_punkDrawTarget, ptc);

		rect = ::ConvertToWindow(m_punkDrawTarget, rect);

		dc.Ellipse(rect);

		DrawCross(dc, pt1);
		DrawCross(dc, pt2);
		DrawCross(dc, ptc);

		dc.MoveTo(ptc);
		dc.LineTo(pt2);
	}
}

void CManualMeasObject::DrawAngle2Line(CDC & dc)
{
	if (m_arrPoints.GetSize() < 4)
		return;

	CPoint pt1((int)m_arrPoints[0].fX, (int)m_arrPoints[0].fY);
	CPoint pt2((int)m_arrPoints[1].fX, (int)m_arrPoints[1].fY);
	CPoint pt3((int)m_arrPoints[2].fX, (int)m_arrPoints[2].fY);
	CPoint pt4((int)m_arrPoints[3].fX, (int)m_arrPoints[3].fY);

	//if (m_punkDrawTarget)
	{
		pt1 = ::ConvertToWindow(m_punkDrawTarget, pt1);
		pt2 = ::ConvertToWindow(m_punkDrawTarget, pt2);
		pt3 = ::ConvertToWindow(m_punkDrawTarget, pt3);
		pt4 = ::ConvertToWindow(m_punkDrawTarget, pt4);

		DrawCross(dc, pt1);
		DrawCross(dc, pt2);
		DrawCross(dc, pt3);
		DrawCross(dc, pt4);

		dc.MoveTo(pt1);
		dc.LineTo(pt2);

		dc.MoveTo(pt3);
		dc.LineTo(pt4);
	}
}

void CManualMeasObject::DrawAngle3Point(CDC & dc)
{
	if (m_arrPoints.GetSize() < 4)
		return;

	CPoint pt1((int)m_arrPoints[0].fX, (int)m_arrPoints[0].fY);
	CPoint pt2((int)m_arrPoints[1].fX, (int)m_arrPoints[1].fY);
	CPoint pt3((int)m_arrPoints[2].fX, (int)m_arrPoints[2].fY);

	bool bFlag = m_arrPoints[3].fX > 0;

	//if (m_punkDrawTarget)
	{
		pt1 = ::ConvertToWindow(m_punkDrawTarget, pt1);
		pt2 = ::ConvertToWindow(m_punkDrawTarget, pt2);
		pt3 = ::ConvertToWindow(m_punkDrawTarget, pt3);

		DrawCross(dc, pt1);
		DrawCross(dc, pt2);
		DrawCross(dc, pt3);

		dc.MoveTo(pt1);
		dc.LineTo(pt2);

		dc.MoveTo(pt2);
		dc.LineTo(pt3);

		// draw angle arc
		double	d1, d2, d;
		int	x1, x2, x3, x4, y1, y2, y3, y4;
				
		d = CalcAngles(d1, d2);

		x1 = pt2.x + m_nArcSize;
		x2 = pt2.x - m_nArcSize;
		y1 = pt2.y + int(m_nArcSize );
		y2 = pt2.y - int(m_nArcSize );

		if (d > 5. / 180. * PI)	//Меньше 5 градусов не рисовать
		{
			if (bFlag)
			{
				d = d1;
				d1 = d2;
				d2 = d;
			}

			x3 = pt2.x + int(m_nArcSize * cos(d1));
			y3 = pt2.y + int(m_nArcSize * sin(d1) );
			x4 = pt2.x + int(m_nArcSize * cos(d2));
			y4 = pt2.y + int(m_nArcSize * sin(d2)  );

			CPoint ptB(x3, y3);
			CPoint ptE(x4, y4);
			if (ptB != ptE)
				dc.Arc(x1, y1, x2, y2, x3, y3, x4, y4);
		}
	}
}


void CManualMeasObject::DrawCount(CDC & dc)
{
	LOGFONT	lf;
	ZeroMemory( &lf, sizeof( lf ) );
	lf.lfCharSet = 1;
	lf.lfHeight = 15;
	lf.lfWeight = FW_BOLD;
	strcpy( lf.lfFaceName, "Arial" );

	CFont	font;
	font.CreateFontIndirect( &lf );
	CFont	*pold = dc.SelectObject( &font );
	dc.SetBkMode( TRANSPARENT );


	for (int i = 0; i < m_arrPoints.GetSize(); i++)
	{
		CPoint point;
		point.x = (int)m_arrPoints[i].fX;//TransformValue(m_arrPoints[i].fX, m_fCalibr, m_fXOffset);
		point.y = (int)m_arrPoints[i].fY;//TransformValue(m_arrPoints[i].fY, m_fCalibr, m_fYOffset);

		point = ::ConvertToWindow(m_punkDrawTarget, point);
		DrawCross(dc, point);

		CRect	rect( point.x, point.y, point.x, point.y );
		CString	str;
		str.Format( "%d", i+1 );
		dc.DrawText( str, &rect, DT_NOCLIP );
	}
	dc.SelectObject( pold );
}

void CManualMeasObject::DrawCross(CDC & dc, const CPoint & pt)
{
	// [vanek] BT2000: 3680 - use mask
	double fzoom = -1.;
	if( m_pOptions && m_pOptions->dwMask & domZoomDC )
        fzoom = m_pOptions->dZoomDC;

	if( fzoom < 0 )
		fzoom = GetZoom( m_punkDrawTarget );
	
    dc.MoveTo(pt.x - m_nCrossOffset * fzoom, pt.y - m_nCrossOffset * fzoom );		//  "\ "
	dc.LineTo(pt.x + m_nCrossOffset * fzoom, pt.y + m_nCrossOffset * fzoom );		//  " \"


	dc.MoveTo(pt.x + m_nCrossOffset * fzoom, pt.y - m_nCrossOffset * fzoom );		//  " /"
	dc.LineTo(pt.x - m_nCrossOffset * fzoom, pt.y + m_nCrossOffset * fzoom );		//  "/ "
}

HRESULT CManualMeasObject::XRect::SetRect(RECT rc)
{
	METHOD_PROLOGUE_EX(CManualMeasObject,Rect);
	return E_NOTIMPL;
}

HRESULT CManualMeasObject::XRect::GetRect(RECT* prc)
{
	METHOD_PROLOGUE_EX(CManualMeasObject,Rect);
	*prc = pThis->DoGetRect();
	return S_OK;
}

HRESULT CManualMeasObject::XRect::HitTest( POINT	point, long *plHitTest )
{
	METHOD_PROLOGUE_EX(CManualMeasObject,Rect);
	*plHitTest = pThis->CrossZone( CPoint(point) );
	return S_OK;
}
HRESULT CManualMeasObject::XRect::Move( POINT point )
{
	METHOD_PROLOGUE_EX(CManualMeasObject,Rect);

	CRect	rect;
	GetRect( &rect );
	CPoint	ptDelta( point.x-rect.left, point.y-rect.top );
	return pThis->SetOffset(ptDelta) ? S_OK : E_FAIL;
}



void CManualMeasObject::UpdateParent()
{
	INamedDataObject2Ptr sptrObj = GetControllingUnknown();
	if (sptrObj == 0)
		return;

	IUnknown *punkParent = 0;
	sptrObj->GetParent(&punkParent);

	IMeasureObjectPtr sptrParent = punkParent;
	if (punkParent)
		punkParent->Release();

	if (sptrParent)
		sptrParent->UpdateParent();
}

struct _ParallelLineInfo
{
	double dX1,dY1,dX2,dY2;
	long lKey;
	IManualMeasureObject *punkMMObj;
	IUnknown *punkObj;
};

double _GetDistance(double x1_1, double y1_1, double x2_1, double y2_1,
	double x1_2, double y1_2, double x2_2, double y2_2)
{
	double	a_1, b_1, c_1;
	a_1 = y2_1-y1_1;
	b_1 = x1_1-x2_1;
	c_1 = x1_1*a_1+y1_1*b_1;
	double	a_2, b_2, c_2;
	a_2 = y2_2-y1_2;
	b_2 = x1_2-x2_2;
	c_2 = x1_2*a_2+y1_2*b_2;
	double	a_p, b_p, c_p;
	a_p = b_1;
	b_p = -a_1;
	c_p = 0;
	double	dx_1, dy_1, d_1;
	dx_1 = c_1*b_p-c_p*b_1;
	dy_1 = a_1*c_p-a_p*c_1;
	d_1 = a_1*b_p-a_p*b_1;
	double	dx_2, dy_2, d_2;
	dx_2 = c_2*b_p-c_p*b_2;
	dy_2 = a_2*c_p-a_p*c_2;
	d_2 = a_2*b_p-a_p*b_2;
	double	xp_1, yp_1, xp_2, yp_2;
	xp_1 = dx_1/d_1;
	yp_1 = dy_1/d_1;
	xp_2 = dx_2/d_2;
	yp_2 = dy_2/d_2;
	return sqrt( (xp_1-xp_2)*(xp_1-xp_2)+(yp_1-yp_2)*(yp_1-yp_2) );
}

bool _GetMMObjectInfo(IUnknown *punkObj, _ParallelLineInfo &Info)
{
	INamedDataObject2Ptr sptrObj(punkObj);
	TPOS lPosChild;
	sptrObj->GetFirstChildPosition(&lPosChild);
	while (lPosChild)
	{
		IUnknownPtr punkMM;
		sptrObj->GetNextChild(&lPosChild,&punkMM);
		IManualMeasureObjectPtr sptrMMObj(punkMM);
		if (sptrMMObj != 0)
		{
			DWORD lType = 0;
			if (sptrMMObj->GetObjectType(&lType) == S_OK && lType == emotParallelLines)
			{
				TExchangeMeasData Data;
				memset(&Data, 0, sizeof(Data));
				Data.dwObjectType = (DWORD)emotParallelLines;
				if (SUCCEEDED(sptrMMObj->ExchangeData((DWORD*)&Data, TRUE)))
				{
					if (Data.lNumPoints == 2)
					{
						Info.dX1 = Data.pPoints[0].fX;
						Info.dY1 = Data.pPoints[0].fY;
						Info.dX2 = Data.pPoints[1].fX;
						Info.dY2 = Data.pPoints[1].fY;
						long lKey;
						DWORD dwType;
						sptrMMObj->GetParamInfo(&lKey, &dwType);
						Info.lKey = lKey;
						if (Data.pPoints)
							CoTaskMemFree(Data.pPoints);
						Info.punkMMObj = sptrMMObj;
						Info.punkObj = punkObj;
						return true;
					}
					if (Data.pPoints)
						CoTaskMemFree(Data.pPoints);
				}
			}
		}
	}
	return false;
}

void CManualMeasObject::CalcParallelLinesMatrix(IUnknown *punkParent, IUnknown *punkExclude)
{
	IUnknownPtr punkPar(punkParent);
	if (punkPar == 0)
	{
		INamedDataObject2Ptr sptrMM(GetControllingUnknown());
		if (sptrMM == 0) return;
		sptrMM->GetParent(&punkPar);
		if (punkPar == 0) return;
	}
	GuidKey keyExclude;
	if (punkExclude != 0)
		keyExclude = GetObjectKey(punkExclude);
	INamedDataObject2Ptr sptrCurObj(punkPar);
	IUnknownPtr punkList;
	sptrCurObj->GetParent(&punkList);
	CObjectListWrp sptrObjList(punkList);
	if (sptrObjList == 0) return;
	// Gather information about parallel line objects in object list
	GuidKey keyThis = GetObjectKey(punkPar);
	_ParallelLineInfo CurLine;
	_GetMMObjectInfo(sptrCurObj, CurLine);
	CArray<_ParallelLineInfo,_ParallelLineInfo&> arrLines;
	int nChildNo = 0, nThis = -1;
	POSITION pos = sptrObjList.GetFirstObjectPosition();
	while(pos)
	{
		IUnknownPtr punkObj(sptrObjList.GetNextObject(pos),false);
		GuidKey keyObj = GetObjectKey(punkObj);
		if (punkExclude != NULL && keyObj == keyExclude) continue; // zombie object !!!
		_ParallelLineInfo Info;
		if (_GetMMObjectInfo(punkObj, Info))
		{
			arrLines.Add(Info);
			if (keyObj == keyThis)
				nThis = nChildNo;
		}
		nChildNo++;
	}
	ICalcObjectContainerPtr sptrCOCObjLst(sptrObjList);
	int k;
	// Check for existance of necessary keys in container
	long lpos = 0;
	int nKeys = min(arrLines.GetSize(), m_arrKeys.GetSize());
	IUnknownPtr pgroup(GetMeasManualGroup(),false);
	IMeasParamGroupPtr	ptrGroup = pgroup;
	if (ptrGroup != 0)
	{
		for (k = 0; k < nKeys; k++)
		{
			ParameterContainer *pCont = NULL;
			if (FAILED(sptrCOCObjLst->ParamDefByKey(m_arrKeys[k], &pCont)) || pCont == NULL)
			{
				sptrCOCObjLst->DefineParameter(m_arrKeys[k], etLinear, ptrGroup, 0);
				if (SUCCEEDED(sptrCOCObjLst->ParamDefByKey(m_arrKeys[k], &pCont)) && pCont)
					sptrCOCObjLst->MoveParameterAfter(lpos, pCont);
			}
			if (pCont)
				lpos = pCont->lpos;
		}
	}
	// Calculate distances from this line to others an set it to current object
	IDataObjectListPtr sptrDOList(sptrObjList);
	ICalcObjectPtr sptrCOCur(sptrCurObj);
	for (int i = 0; i < arrLines.GetSize(); i++)
	{
		ICalcObjectPtr sptrObj1(arrLines[i].punkObj);
		for (int j = i+1; j < arrLines.GetSize(); j++)
		{
			ICalcObjectPtr sptrObj2(arrLines[j].punkObj);
			double dDistance = _GetDistance(arrLines[i].dX1, arrLines[i].dY1,
				arrLines[i].dX2, arrLines[i].dY2, arrLines[j].dX1, arrLines[j].dY1,
				arrLines[j].dX2, arrLines[j].dY2);
			dDistance *= m_fCalibr;
			if (j < m_arrKeys.GetSize()) sptrObj1->SetValue(m_arrKeys[j], dDistance);
			if (i < m_arrKeys.GetSize()) sptrObj2->SetValue(m_arrKeys[i], dDistance);
			TRACE("Object %d, key %d, Object %d, key %d, value %f, \n", i, 
				j<m_arrKeys.GetSize()?(int)m_arrKeys[j]:-1,
				j, i<m_arrKeys.GetSize()?(int)m_arrKeys[i]:-1, dDistance);
		}
		if (i < m_arrKeys.GetSize())
		{
			sptrObj1->SetValue(m_arrKeys[i], 0.);
			DWORD dwObjType = etLinear;//emotParallelLines;
			long lParamKey = m_arrKeys[i];
			arrLines[i].punkMMObj->SetParamInfo(&lParamKey, &dwObjType);
			TRACE("Object %d, main key %d\n", i, (int)m_arrKeys[i]);

		}
		// Remove unneeded keys from object
		if (m_arrKeys.GetSize() > arrLines.GetSize())
		{
			for (k = arrLines.GetSize(); k < m_arrKeys.GetSize(); k++)
			{
				sptrObj1->RemoveValue(m_arrKeys[k]);
				TRACE("Remove key %d from object %d\n", (int)m_arrKeys[k], i);
			}
		}
		sptrDOList->UpdateObject(sptrObj1);
	}
	// Remove extra keys from container
	if (m_arrKeys.GetSize() > arrLines.GetSize())
	{
		for (k = arrLines.GetSize(); k < m_arrKeys.GetSize(); k++)
		{
			sptrCOCObjLst->RemoveParameter(m_arrKeys[k]);
			TRACE("Remove key %d from object list\n", (int)m_arrKeys[k]);
		}
	}
}

int __cdecl _compare(const void *p1, const void *p2)
{
	DWORD dw1 = *(DWORD*)p1;
	DWORD dw2 = *(DWORD*)p2;
	return dw1<dw2?-1:dw1==dw2?0:1;
}

void CManualMeasObject::ResetParallelLinesKeys(IUnknown *punkNewParent)
{
}

void CManualMeasObject::OnSetParent( IUnknown *punkNewParent )
{
	if( punkNewParent )
	{
		if( !m_bCalculated )return;
		ICalcObjectPtr	ptrCalc( punkNewParent );
		if( ptrCalc == 0 )return;

		IManualMeasureObjectPtr	ptrM( GetControllingUnknown() );
		if( ptrM == 0 )return;
		{
			long	lkey = -1;
			double	fval = 0;
			DWORD	type;
			ptrM->GetParamInfo( &lkey, &type );
			if (m_dwObjType != emotParallelLines)
			{
				ptrM->GetValue( &fval );
				ptrCalc->SetValue( lkey, fval );
			}

			//ensure key is registred
			INamedDataObject2Ptr	ptrNamedObject( punkNewParent );
			IUnknown	*punkList = 0;
			ptrNamedObject->GetParent( &punkList );
			ICalcObjectContainerPtr	ptrContainer( punkList );
			if( punkList )punkList->Release();
			if( ptrContainer == 0 )return;
			ParameterContainer	*pc = 0;
			ptrContainer->ParamDefByKey( lkey, &pc );
			if( !pc && m_dwObjType != emotParallelLines) //define parameter
			{ // for emotParallelLines special processing in CalcParallelLinesMatrix
				IUnknown	*pgroup = GetMeasManualGroup();
				if( pgroup )
				{
					IMeasParamGroupPtr	ptrGroup = pgroup;
					ptrContainer->DefineParameter( lkey, (ParamType)type, ptrGroup, 0 );
					pgroup->Release();
				}
			}
			long ClassKey = get_object_class( ptrCalc );
			if( ptrCalc != 0)
				set_object_class( ptrCalc, ClassKey );
//			if (m_dwObjType == emotParallelLines)
//				CalcParallelLinesMatrix(punkNewParent,NULL);
		}
	}
	else
	{
//		if (m_dwObjType == emotParallelLines)
//			CalcParallelLinesMatrix(NULL,m_pParent);
		//remove value from parent 
		ICalcObjectPtr	ptrCalc( m_pParent );
		if( ptrCalc == 0 )return;
		ptrCalc->RemoveValue( m_lParamKey );

	}
}

CFont	*CManualMeasObject::create_font( bool bZoom, long lfontsize /*= 15*/ )
{
	LOGFONT	lf;
	ZeroMemory( &lf, sizeof( lf ) );
	lf.lfCharSet = 1;
	lf.lfHeight = lfontsize; // [vanek] BT2000: 3560 - 12.01.2004
	// [vanek] BT2000: 3560 - 22.12.2003
	if( bZoom )
	{
		// [vanek] BT2000: 3680 - use mask
		double fzoom = -1.;
		if( m_pOptions && m_pOptions->dwMask & domZoomDC )
            fzoom = m_pOptions->dZoomDC;

		if( fzoom < 0 )
			fzoom = GetZoom( m_punkDrawTarget );

		// [vanek] SBT: 755 - 29.03.2004
		lf.lfHeight = long( lf.lfHeight * fzoom );
		if( !lf.lfHeight )
			lf.lfHeight = -1;
	}

	lf.lfWeight = FW_BOLD;
	strcpy( lf.lfFaceName, "Arial" );

	if( m_pt1.y != m_pt2.y )
	{
		double	a = -::atan2( (double)(m_pt2.y - m_pt1.y), (double)(m_pt2.x - m_pt1.x) );
		lf.lfOrientation = int(a/PI*1800);
		lf.lfEscapement = lf.lfOrientation;
	}

	CFont	*p=  new CFont;
	p->CreateFontIndirect( &lf );
	return p;
}
CString	CManualMeasObject::get_number()
{
	long	lNumber = 0;
	CString	str = "?";

	INamedDataObject2Ptr	ptrNamedObject( m_pParent );
	if( ptrNamedObject )
	{
		/*IUnknown	*punkList = 0;
		ptrNamedObject->GetParent( &punkList );
		ICalcObjectContainerPtr	ptrContainer( punkList );
		if( punkList )punkList->Release();
		if( ptrContainer != 0 )
		{
			ParameterContainer	*pc = 0;
			ptrContainer->ParamDefByKey( m_lParamKey, &pc );
			str = pc->pDescr->bstrUserName;
		}*/

		IMeasureObjectPtr	ptrM( m_pParent );
		ptrM->GetDrawingNumber( &lNumber );
		
		//m_str.Format( "%d/%s", lNumber, (const char*)str );
		m_str.Format( "%d", lNumber );
	}

	return m_str;
}

CRect	CManualMeasObject::CalcTextRect()
{
	if( m_pt1.x > m_pt2.x )
	{
		CPoint	pt = m_pt1;
		m_pt1 = m_pt2;
		m_pt2 = pt;
	}
	CClientDC	dc( 0 );
	CFont	*pfont = create_font( false, 
								-MulDiv( ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "N_And_N_FontSize", 10 ),
								dc.GetDeviceCaps( LOGPIXELSY ), 72) );	// [vanek] BT2000: 3560 - 12.01.2004
	CFont	*pold = dc.SelectObject( pfont );
	CRect	rect( m_pt1.x, m_pt1.y, m_pt1.x, m_pt1.y );

	dc.DrawText( get_number(), &rect, DT_CALCRECT|DT_TOP|DT_SINGLELINE );
	dc.SelectObject( pold );

	delete pfont;

	//rotate points
	if( m_pt1.y != m_pt2.y )
	{
		double	a = -::atan2( (double)(m_pt2.y - m_pt1.y), (double)(m_pt2.x - m_pt1.x) );
		double	cosa = ::cos( a );
		double	sina = ::sin( a );

		double	x1, x2, x3, x4, y1, y2, y3, y4;
		double	x1s = 0, x2s = rect.Width(), y1s = 0, y2s = rect.Height();

		x1 = rect.left+x1s*cosa+y1s*sina;
		x2 = rect.left+x2s*cosa+y1s*sina;
		x3 = rect.left+x1s*cosa+y2s*sina;
		x4 = rect.left+x2s*cosa+y2s*sina;

		y1 = rect.top+y1s*cosa-x1s*sina;
		y2 = rect.top+y2s*cosa-x1s*sina;
		y3 = rect.top+y1s*cosa-x2s*sina;
		y4 = rect.top+y2s*cosa-x2s*sina;

		rect.left = int( min( min( x1, x2 ), min( x3, x4 ) ) );
		rect.right = int( max( max( x1, x2 ), max( x3, x4 ) ) );
		rect.top = int( min( min( y1, y2 ), min( y3, y4 ) ) );
		rect.bottom = int( max( max( y1, y2 ), max( y3, y4 ) ) );
	}
		

	return rect;
}



int		TransformValue(double fVal, double fCalibr, double fOffset)
{
	if (fabs(fCalibr) <= DBL_EPSILON)
		return (fCalibr * (fVal/* - fOffset*/) > 0) ? INT_MAX : INT_MIN;

	return (int)((fVal/* - fOffset*/) / fCalibr);
}

double	TransformValue(int nVal, double fCalibr, double fOffset)
{
	return nVal * fCalibr/* + fOffset*/;
}

HRESULT CManualMeasObject::XCalibr::GetCalibration( double *pfPixelPerMeter, GUID *pguidC )
{
	METHOD_PROLOGUE_EX(CManualMeasObject, Calibr);
	if( pfPixelPerMeter )*pfPixelPerMeter = pThis->m_fCalibr;
	if( pguidC )*pguidC = pThis->m_guidC;
	return S_OK;
}

HRESULT CManualMeasObject::XCalibr::SetCalibration( double fPixelPerMeter, GUID * pguidC )
{
	METHOD_PROLOGUE_EX(CManualMeasObject, Calibr);
	pThis->m_guidC = *pguidC;
	pThis->m_fCalibr = fPixelPerMeter;
	return S_OK;
}
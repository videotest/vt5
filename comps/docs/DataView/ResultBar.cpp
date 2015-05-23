// ResultBar.cpp : implementation file
//

#include "stdafx.h"
#include "DataViewDef.h"
#include "DataView.h"
#include "ResultBar.h"
#include "DataViewDef.h"
#include "GridView.h"
#include "MemDC.h"

#include "Params.h"
#include "NameConsts.h"
#include "Classes5.h"

#include <math.h>
#include <ap.h>

#define IDC_RESULT_GRID 101010
#define RESBAR_CLASSNAME _T("ResultBarCtrl")

double CalcNum(CDoubleArray & arr);
double CalcAver(CDoubleArray & arr);
double CalcDev(CDoubleArray & arr);

static const double st[] = 
{
	0.0,	12.706,	4.303,	3.182,	2.776,	2.571,	2.447,
	2.365,	2.306,	2.262,	2.228,	2.201,	2.179,	2.16,
	2.145,	2.131,	2.12,	2.11,	2.101,	2.093,	2.086,
	2.08,	2.074,	2.069,	2.064,	2.06,	2.056,	2.052,
	2.048,	2.045,	2.042,	2.037,	2.035,	2.033,	2.031,	
	2.031,	2.029,	2.027,	2.025,	2.023,	2.021
};
 
double student(int n)
{
	if (n < 41) 
		return st[n];

	if (n < 61)
		return 2.0;
	
	if (n < 121) 
		return 1.98;

	return 1.96;
}
 
double lg(double x)
{
	if (x <= 0) 
		return 0.;

	return log10(x);
}


// result param 
void CResultParam::SetParamRect(int nIndex, CRect & cellRect)
{
	if (!(nIndex < 0 || nIndex >= m_arrValue.GetSize()))
		m_arrValue[nIndex].rcRect = cellRect;
}

CRect CResultParam::GetParamRect(int nIndex)
{
	CRect rc;
	rc.SetRectEmpty();

	if (!(nIndex < 0 || nIndex >= m_arrValue.GetSize()))
		rc = m_arrValue[nIndex].rcRect;

	return rc;
}

void CResultParam::SetParamValue(int nIndex, double fVal)
{
	if (!(nIndex < 0 || nIndex >= m_arrValue.GetSize()))
		m_arrValue[nIndex].fValue = fVal;
}

void CResultParam::SetParamFormat(int nIndex, const char *pszFormat )
{
	if (!(nIndex < 0 || nIndex >= m_arrValue.GetSize()))
		m_arrValue[nIndex].strFormat = pszFormat;
}

double CResultParam::GetParamValue(int nIndex)
{
	if (!(nIndex < 0 || nIndex >= m_arrValue.GetSize()))
		return m_arrValue[nIndex].fValue;

	return 0.;
}

const char *CResultParam::GetParamFormat(int nIndex)
{
	if (!(nIndex < 0 || nIndex >= m_arrValue.GetSize()))
		return m_arrValue[nIndex].strFormat;

	return "%g";
}

bool CResultParam::IsValueEmpty(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_arrValue.GetSize())
		return true; 
	
	return m_arrValue[nIndex].bEmpty;

}

void CResultParam::SetValueEmpty(int nIndex, bool bEmpty)
{
	if (!(nIndex < 0 || nIndex >= m_arrValue.GetSize()))
		m_arrValue[nIndex].bEmpty = bEmpty;
}
/*
bool CResultParam::CalcValue(int nIndex, CDoubleArray & arr)
{
	if (nIndex < 0 || nIndex >= m_arrValue.GetSize())
		return false;
	
	if (!arr.GetSize())
	{
		m_arrValue[nIndex].fValue = 0;
		m_arrValue[nIndex].bEmpty = true;
		return true;
	}

	bool bRet = true;

	switch (m_lParamKey)
	{
	case KEY_NUM:
		m_arrValue[nIndex].fValue = CalcNum(arr);
		break;

	case KEY_MEAN:
		m_arrValue[nIndex].fValue = CalcAver(arr);
		break;

	case KEY_DEV:
		m_arrValue[nIndex].fValue = CalcDev(arr);
		break;
	}
	m_arrValue[nIndex].bEmpty = false;

	return true;
}
*/
/*
#define KEY_MEAN	(USERKEY+10)	// среднее значение
#define KEY_DEV		(USERKEY+11)	// СКО
#define KEY_VAR		(USERKEY+12)	// коэффициент вариации
#define KEY_ERROR	(USERKEY+13)	// абсолютная погрешность
#define KEY_MIN		(USERKEY+14)	// min
#define KEY_MAX		(USERKEY+15)	// max
#define KEY_RA		(USERKEY+16)	//
#define KEY_REA		(USERKEY+17)	//
#define KEY_RN		(USERKEY+18)	//
#define KEY_REN		(USERKEY+19)	//
#define KEY_NUM		(USERKEY+20)	// количество
#define KEY_MEANOBJ		(USERKEY+21)
#define KEY_PERCENT		(USERKEY+22)
#define KEY_SUMMA		(USERKEY+23)// сумма значений
*/
/*
double CalcNum(CDoubleArray & arr)
{
	return (double)arr.GetSize();
}

double CalcAver(CDoubleArray & arr)
{
	if (!arr.GetSize())
		return 0;

	double fSum = 0;

	for (int i = 0; i < arr.GetSize(); i++)
		fSum += arr[i];
	return fSum / arr.GetSize();
}

double CalcDev(CDoubleArray & arr)
{
	if (!arr.GetSize())
		return 0;

	double fAver = CalcAver(arr);
	double fDev = 0;
	for (int i = 0; i < arr.GetSize(); i++)
		fDev += (arr[i] - fAver)*(arr[i] - fAver);

	if ((arr.GetSize() - 1) > 0)
		fDev /= arr.GetSize() - 1;

	return ::sqrt(fDev);
}
*/
/////////////////////////////////////////////////////////////////////////////
// CResultBar
IMPLEMENT_DYNAMIC(CResultBar, CStatic);

CResultBar::CResultBar()
{
	m_ParamRc.SetRectEmpty();
	m_GridRc.SetRectEmpty();
	m_nTextMargin = 0;
	m_pView = 0;
	m_bDrawLine = true;
	m_nDefCellHeight = 16;
    RegisterWindowClass();
	m_bEnabled = true;

}

CResultBar::~CResultBar()
{
	Clear();
}

void CResultBar::Clear()
{
	//set column count
	for (int i = 0; i < m_arrParams.GetSize(); i++)
		m_arrParams[i].m_arrValue.RemoveAll();


//	m_arrParams.RemoveAll();
	if (GetSafeHwnd() && ::IsWindow(GetSafeHwnd()))
	{
		InvalidateRect(NULL);
//		UpdateWindow();
	}
}

BEGIN_MESSAGE_MAP(CResultBar, CStatic)
	//{{AFX_MSG_MAP(CResultBar)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultBar message handlers
// Register the window class if it has not already been registered.
BOOL CResultBar::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();
   // HINSTANCE hInst = AfxGetResourceHandle();

    if (!(::GetClassInfo(hInst, RESBAR_CLASSNAME, &wndcls)))
    {

		if (!::GetClassInfo(NULL, "STATIC", &wndcls))
			return false;
        wndcls.hInstance        = hInst;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.lpszClassName    = RESBAR_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return false;
        }
    }
    return true;
}

BOOL CResultBar::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CLIPCHILDREN;
	dwStyle &= ~WS_BORDER;

    if (!CWnd::Create(GRIDCTRL_CLASSNAME, lpszText, dwStyle, rect, pParentWnd, nID))
        return false;

	return true;	
}

int CResultBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CWnd * pParent = GetParent();
	ASSERT (pParent->IsKindOf(RUNTIME_CLASS(CGridViewBase)) != 0);
	if (!pParent || !pParent->IsKindOf(RUNTIME_CLASS(CGridViewBase)))
		return -1;

	m_pView = (CGridViewBase*)pParent;

	ModifyStyleEx(0, WS_EX_WINDOWEDGE);

	CalcDefCellHeight();
	CRect rc = CalcGridRect();

	return 0;
}

void CResultBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CRect PrevRc = m_GridRc;
	CalcGridRect();

	SyncParamColumn();

	
	PrevRc.left = PrevRc.left < m_GridRc.left ? PrevRc.left : m_GridRc.left;
	PrevRc.top = PrevRc.top < m_GridRc.top ? PrevRc.top : m_GridRc.top;
	PrevRc.right = PrevRc.right > m_GridRc.right ? PrevRc.right : m_GridRc.right;
	PrevRc.bottom = PrevRc.bottom < m_GridRc.bottom ? PrevRc.bottom : m_GridRc.bottom;
	InvalidateRect(PrevRc.Width() ? PrevRc : NULL);

//	UpdateWindow();
	
//	CStatic::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CResultBar::OnSize(UINT nType, int cx, int cy) 
{
	CRect rc = m_GridRc;
	CWnd::OnSize(nType, cx, cy);

	CalcGridRect();
	SyncParamColumn();

	rc.left = rc.left < m_GridRc.left ? rc.left : m_GridRc.left;
	rc.top = rc.top < m_GridRc.top ? rc.top : m_GridRc.top;
	rc.right = rc.right > m_GridRc.right ? rc.right : m_GridRc.right;
	rc.bottom = rc.bottom < m_GridRc.bottom ? rc.bottom : m_GridRc.bottom;

	InvalidateRect(NULL);
//	UpdateWindow();
}

void CResultBar::UpdateSize() 
{
	CRect rc = m_GridRc;

	CalcGridRect();
	SyncParamColumn();

	rc.left = rc.left < m_GridRc.left ? rc.left : m_GridRc.left;
	rc.top = rc.top < m_GridRc.top ? rc.top : m_GridRc.top;
	rc.right = rc.right > m_GridRc.right ? rc.right : m_GridRc.right;
	rc.bottom = rc.bottom < m_GridRc.bottom ? rc.bottom : m_GridRc.bottom;

	InvalidateRect(NULL);
}

void CResultBar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CResultBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	//CMemDC DC(&dc);
	Draw(&dc);
}

BOOL CResultBar::OnEraseBkgnd(CDC* pDC) 
{
	return 1; 
}

bool CResultBar::LoadState()
{
	if (m_strSection.IsEmpty())
		return false;

	m_arrParams.RemoveAll();

	// Number
	CString strParam;
	strParam.Format("%s%d", szShowResult, KEY_NUM);
	int nShow = ::GetValueInt(::GetAppUnknown(false), m_strSection, strParam, (long)1);
	if (nShow)
	{
		CResultParam param(KEY_NUM, GetDefRowName(KEY_NUM) );
		m_arrParams.Add(param);
	}

	// summ
	strParam.Format("%s%d", szShowResult, KEY_SUMMA);
	nShow = ::GetValueInt(::GetAppUnknown(false), m_strSection, strParam, (long)1);
	if (nShow)
	{
		CResultParam param(KEY_SUMMA, GetDefRowName(KEY_SUMMA)  );
		m_arrParams.Add(param);
	}

	// average
	strParam.Format("%s%d", szShowResult, KEY_MEAN);
	nShow = ::GetValueInt(::GetAppUnknown(false), m_strSection, strParam, (long)1);
	if (nShow)
	{
		CResultParam param(KEY_MEAN, GetDefRowName(KEY_MEAN)  );
		m_arrParams.Add(param);
	}

	// deviation (SKO)
	strParam.Format("%s%d", szShowResult, KEY_DEV);
	nShow = ::GetValueInt(::GetAppUnknown(false), m_strSection, strParam, (long)1);
	if (nShow)
	{
		CResultParam param(KEY_DEV, GetDefRowName(KEY_DEV)  );
		m_arrParams.Add(param);
	}

	// Abs error
	strParam.Format("%s%d", szShowResult, KEY_ERROR);
	nShow = ::GetValueInt(::GetAppUnknown(false), m_strSection, strParam, (long)1);
	if (nShow)
	{
		CResultParam param(KEY_ERROR, GetDefRowName(KEY_ERROR) );
		m_arrParams.Add(param);
	}

	// Variatioin coefficient
	strParam.Format("%s%d", szShowResult, KEY_VAR);
	nShow = ::GetValueInt(::GetAppUnknown(false), m_strSection, strParam, (long)1);
	if (nShow)
	{
		CResultParam param(KEY_VAR, GetDefRowName(KEY_VAR) );
		m_arrParams.Add(param);
	}

	// min
	strParam.Format("%s%d", szShowResult, KEY_MIN);
	nShow = ::GetValueInt(::GetAppUnknown(false), m_strSection, strParam, (long)1);
	if (nShow)
	{
		CResultParam param(KEY_MIN, GetDefRowName(KEY_MIN) );
		m_arrParams.Add(param);
	}

	// max
	strParam.Format("%s%d", szShowResult, KEY_MAX);
	nShow = ::GetValueInt(::GetAppUnknown(false), m_strSection, strParam, (long)1);
	if (nShow)
	{
		CResultParam param(KEY_MAX, GetDefRowName(KEY_MAX) );
		m_arrParams.Add(param);
	}

	m_bDrawLine = ::GetValueInt(::GetAppUnknown(false), m_strSection, szShowStatLine, (long)0) == 1;

	return true;
}

bool CResultBar::SaveState()
{
	if (m_strSection.IsEmpty())
		return false;

	for (int i = 0; i < m_arrParams.GetSize(); i++)
	{
		CString strParam;
		strParam.Format("%s%d", szShowResult, m_arrParams[i].m_lParamKey);
		::SetValue(::GetAppUnknown(false), m_strSection, strParam, (long)1);

		SetDefRowName(m_arrParams[i].m_lParamKey, m_arrParams[i].m_strName, false);
	}

	::SetValue(::GetAppUnknown(false), m_strSection, szShowStatLine, (long)m_bDrawLine);
	
	return true;
}

/*
int	CResultBar::GetDefParamCount()
{
	return 8;
}
*/

void CResultBar::SetDefRowName(long lKey, LPCTSTR szName, bool bNeedUpdate)
{
	CString strName = szName;
	if (lKey != KEY_MEAN && lKey != KEY_DEV && lKey != KEY_MIN && lKey != KEY_MAX && 
		lKey != KEY_VAR && lKey != KEY_ERROR && lKey != KEY_NUM && lKey != KEY_SUMMA)
		return;

	if (!m_strSection.IsEmpty())
	{
		CString strParam = _T("");
		strParam.Format("%s%d", szResultName, lKey);
		::SetValue(::GetAppUnknown(false), m_strSection, strParam, strName);
	}

	// update current bar if need
	if (bNeedUpdate && GetSafeHwnd() && ::IsWindow(GetSafeHwnd()))
	{
		for (int i = 0; i < m_arrParams.GetSize(); i++)
		{
			if (m_arrParams[i].m_lParamKey == lKey)
			{
				m_arrParams[i].m_strName = strName;
				InvalidateRect(NULL);
//				UpdateWindow();
				break;
			}
		}
	}
}

/*
#define szShowResult		"ShowResult"
#define szResultName		"ParamName"
#define szResultFormat		"ParamFormat"
*/

CString CResultBar::GetDefRowName(long lKey)
{
	if (lKey != KEY_MEAN && lKey != KEY_DEV && lKey != KEY_MIN && lKey != KEY_MAX && 
		lKey != KEY_VAR && lKey != KEY_ERROR && lKey != KEY_NUM && lKey != KEY_SUMMA)
		return "";

	CString strName;
	switch (lKey)
	{
	case KEY_MEAN:
		strName = _T("Average");
		break;
	case KEY_DEV:
		strName = _T("Std.Dev");
		break;
	case KEY_MIN:
		strName = _T("Min");
		break;
	case KEY_MAX:
		strName = _T("Max");
		break;
	case KEY_VAR:
		strName = _T("Var.coef.");
		break;
	case KEY_ERROR:
		strName = _T("Abs.error");
		break;
	case KEY_NUM:
		strName = _T("Count");
		break;
	case KEY_SUMMA:
		strName = _T("Sum");
		break;
	}
	if (!m_strSection.IsEmpty())
	{
		CString strParam = _T("");
		strParam.Format("%s%d", szResultName, lKey);
		strName = ::GetValueString(::GetAppUnknown(false), m_strSection, strParam, strName);
	}

	return strName;
}

bool CResultBar::UpdateParams()
{
	for (int i = 0; i < m_arrParams.GetSize(); i++)
		m_arrParams[i].m_arrValue.RemoveAll();

//	m_arrParams.RemoveAll();

	if ( m_objects == 0 || !m_bEnabled )
		return true;

	int nRowCount = m_arrParams.GetSize();
	if (!nRowCount )
		return true;


	long	nCount = 0;
	ICalcObjectContainerPtr	ptrContainer( m_objects );
	ptrContainer->GetParameterCount( &nCount );

	//set column count
	for (i = 0; i < nRowCount; i++)
		m_arrParams[i].m_arrValue.SetSize(nCount);


	CRect rc = CalcGridRect();
	SyncParamColumn();

	Recalc();
	
	return true;
}

bool CResultBar::Recalc()
{
	if( !m_bEnabled )return false;

	ICalcObjectContainerPtr	ptrContainer( m_objects );
	if( ptrContainer == 0  )return false;


	LONG_PTR	lpos, n = 0;
	ptrContainer->GetFirstParameterPos( &lpos );
	while( lpos )
	{
		ParameterContainer	*pcont = 0;
		ptrContainer->GetNextParameter( &lpos, &pcont );
		UpdateParam(  n++, pcont );
	}

	InvalidateRect( m_GridRc );
	return true;
}

bool CResultBar::UpdateParam( long pos, ParameterContainer *pc )
{
	if( !pc || !pc->pDescr )	return false;

	long lParamKey = pc->pDescr->lKey;

	if (!m_pView || lParamKey == -1)
		return false;

	CDoubleArray arr;
	arr.SetSize( 0, 100 );

	long	lcount = m_pView->m_rows.GetSize();


	for( long row = m_pView->rowFirstObject(); row < lcount; row++ )
	{
		ICalcObject	*p = m_pView->m_rows[row]->pcalc;
		if( !p )
			continue;
		double	fValue = 0;
		if( p->GetValue( lParamKey, &fValue ) != S_OK )
			continue;

		fValue *= pc->pDescr->fCoeffToUnits;

		arr.Add( fValue );
	}
	CString	s = pc->pDescr->bstrDefFormat;
	return CalcStats( pos, arr, s );
}

bool CResultBar::CalcStats( int nPos, CDoubleArray & arr, const char *pszFmt )
{
	int nCount = arr.GetSize();
	double fAver = 0;
	double fSum = 0;
	double fDev = 0;
	double fAbsErr = 0;
	double fVar = 0;
	double fMin = 0;
	double fMax = 0;

	bool bEmpty = false;
	if (nCount)
	{
		// set first values for min amd max
		fMin = fMax = arr[0];
		double fVal = 0;
		for (int i = 0; i < arr.GetSize(); i++)
		{
			fVal = arr[i];
			fSum += fVal;
			fMin = fMin > fVal ? fVal : fMin;
			fMax = fMax < fVal ? fVal : fMax;
		}
		
		fAver = fSum / nCount;
		if (nCount < 2)
		{
			fDev = fAver;
			fAbsErr = 0;
			fVar = 0;
		}
		else // we have more than one value 
		{
			double fVal = 0;	
			double fSum2 = 0;
			for (int i = 0; i < arr.GetSize(); i++)
			{
				fVal = arr[i];
				fSum2 += (fAver - fVal) * (fAver - fVal);
			}               
			if (fSum2)
			{
				double fSigma = fSum2 / ((double)nCount - 1);
				fDev = sqrt(fSigma);
				fAbsErr = (fDev / sqrt(double(nCount)) * invstudenttdistribution(nCount-1, 0.5+0.5*0.95) );
				if( fabs( fAver ) < 0.0000001 )
					fVar = 0;
				else
					fVar = fabs(fDev / fAver * 100.);
			}		
			else                                                
			{
				fDev = 0;
				fAbsErr = 0;
				fVar = 0;
			}		
		}
	}
	else 
		bEmpty = true;

	for (int i = 0; i < m_arrParams.GetSize(); i++)
	{
		switch (m_arrParams[i].m_lParamKey)
		{
		case KEY_NUM:
			m_arrParams[i].SetParamValue(nPos, nCount);
			m_arrParams[i].SetParamFormat(nPos, "%0.0f");
			break;

		case KEY_SUMMA:
			m_arrParams[i].SetParamValue(nPos, fSum);
			m_arrParams[i].SetParamFormat(nPos, pszFmt);
			break;

		case KEY_MEAN:
			m_arrParams[i].SetParamValue(nPos, fAver);
			m_arrParams[i].SetParamFormat(nPos, pszFmt);
			break;

		case KEY_DEV:
			m_arrParams[i].SetParamValue(nPos, fDev);
			m_arrParams[i].SetParamFormat(nPos, pszFmt);
			break;

		case KEY_MIN:
			m_arrParams[i].SetParamValue(nPos, fMin);
			m_arrParams[i].SetParamFormat(nPos, pszFmt);
			break;

		case KEY_MAX:
			m_arrParams[i].SetParamValue(nPos, fMax);
			m_arrParams[i].SetParamFormat(nPos, pszFmt);
			break;

		case KEY_VAR:
			m_arrParams[i].SetParamValue(nPos, fVar);
			m_arrParams[i].SetParamFormat(nPos, pszFmt);
			break;

		case KEY_ERROR:
			m_arrParams[i].SetParamValue(nPos, fAbsErr);
			m_arrParams[i].SetParamFormat(nPos, pszFmt);
			break;
		}
		m_arrParams[i].SetValueEmpty(nPos, bEmpty);
	}
	return true;
}

bool CResultBar::UpdateParams(INamedDataObject2Ptr ptrNamed)
{
	if (m_objects != ptrNamed)
	{
		Clear();
		m_objects = ptrNamed;
	}

	return UpdateParams();
}

bool CResultBar::Init()
{
	CalcDefCellHeight();

	CRect WindowRect;
	GetWindowRect(&WindowRect);

	if (WindowRect.Height() != GetHeight())
	{
		SetWindowPos(0, WindowRect.left, WindowRect.top + (WindowRect.Height() - GetHeight()), 
					 WindowRect.Width(), GetHeight(), SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_SHOWWINDOW);
	}

	// calc and set up rectangle for params draw
	CRect rc = CalcGridRect();
	SyncParamColumn();

	return true;
}

int CResultBar::GetHeight()
{
	int nCount = m_arrParams.GetSize();
//	if (nCount <= 0)
//		nCount = GetDefParamCount();
	return nCount * GetDefCellHeight() + 2 *::GetSystemMetrics(SM_CYBORDER);
}

void CResultBar::CalcDefCellHeight()
{
	if (!m_pView)
		return;

    CDC* pDC = GetDC();
    if (!pDC)
		return;

    CFont * pOldFont = pDC->SelectObject(m_pView->m_Grid.GetFont());

    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);

    pDC->SelectObject(pOldFont);

    ReleaseDC(pDC);

    m_nDefCellHeight = tm.tmHeight + tm.tmExternalLeading;// + (tm.tmHeight / 4);
}


bool CResultBar::LoadParamInfo()
{
/*	if (!m_arrParams.GetSize())
		return true;
	IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IParamGroupManager, szStatistic);
	if (!punk)
		return false;

	IParamGroupManagerPtr	sptrMan = punk;
	punk->Release();

	if (sptrMan == 0)
		return false;

	// check manager contains groups
	int nCount = 0;
	sptrMan->GetGroupCount(&nCount);
	if (!nCount)
		return true;

	bool bRet = true;
	for (int i = 0; i < m_arrParams.GetSize(); i++)
	{
		long lParamKey = m_arrParams[i].m_lParamKey;

		bool bFinded = false;
		long lGroupPos = 0;
		if (FAILED(sptrMan->GetFirstPos(&lGroupPos)))
			return false;

		// for all groups
		while (lGroupPos)
		{
			IUnknown *punkGroup = 0;
			if (FAILED(sptrMan->GetNextGroup(&lGroupPos, &punkGroup)))
				return false;

			sptrIMeasParamGroup sptrGroup = punkGroup;
		
			if (punkGroup)
				punkGroup->Release();

			if (sptrGroup == 0)
				continue;

			long lParamPos = -1;
			if (FAILED(sptrGroup->GetPosByKey(lParamKey, &lParamPos)) || lParamPos == 0)
				continue;
			
			long lKey = -1;
			BSTR bstrName = 0;
			BSTR bstrFormat = 0;
			DWORD dwFlag = 0;
			if (FAILED(sptrGroup->GetParamInfo(lParamPos, &lKey, &bstrName, &bstrFormat, &dwFlag)))
				continue;
				
			_bstr_t bstrParamName(bstrName, false);
			_bstr_t bstrParamFormat(bstrFormat, false);

			m_arrParams[i].m_strFormat = (LPCTSTR)bstrParamFormat;
			m_arrParams[i].m_strName = (LPCTSTR)bstrParamName;
			bFinded = true;
			break;
		} // for all groups

		if (!bFinded)
			bRet = false;
	}

	return bRet;*/
	return true;
}

void CResultBar::Draw(CDC *pDC)
{
	if (!pDC)
		return;

	CRect GridRc, ClientRc, ClipRect;
	GetClientRect(&ClientRc);
	pDC->GetClipBox(&ClipRect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&ClientRc);
	pDC->SelectClipRgn(&rgn);
	rgn.DeleteObject();


//	TRACE("Draw rect:(%d, %d, %d, %d)\n", ClipRect.left, ClipRect.top, ClipRect.right, ClipRect.bottom);

	COLORREF clBk = ::GetSysColor(COLOR_BTNFACE);
	{
		CBrush brush;
		brush.CreateSolidBrush(clBk);
		pDC->FillRect(&ClientRc, &brush);
		brush.DeleteObject();
	}

	// calc and draw param names
	int nCount = m_arrParams.GetSize();
	if (!nCount)
	{
		return;
	}
	int nHeight = m_arrParams.GetSize() * GetDefCellHeight();
//	if (m_ParamRc.IsRectEmpty())
	m_ParamRc = CalcParamNameRect();
	CRect VisRect = CalcGridRect();

	CFont* pOldFont = pDC->SelectObject(m_pView->m_Grid.GetFont());

	CRect TextRc;
	TextRc = m_ParamRc;
	TextRc.bottom = TextRc.top + GetDefCellHeight();

	int nOldMkMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clOldColor = pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT)); 
	COLORREF clOldBkColor = pDC->SetBkColor(clBk);
	CPen darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW));

	pDC->Draw3dRect(ClientRc.left, ClientRc.top, ClientRc.right, ClientRc.bottom, 
	  				::GetSysColor(COLOR_3DHIGHLIGHT), ::GetSysColor(COLOR_3DDKSHADOW));

	for (int i = 0; i < m_arrParams.GetSize(); i++)
	{
		pDC->DrawText(m_arrParams[i].m_strName, &TextRc, DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
		if (m_bDrawLine)
		{
			if (i != m_arrParams.GetSize() - 1)
			{
				CPen * pOldPen = pDC->SelectObject(&darkpen);
				pDC->MoveTo(ClientRc.left + 3, TextRc.bottom);

				if (m_arrParams[i].m_arrValue.GetSize())
				{
					CRect ParamRc = m_arrParams[i].GetParamRect(0); 
					if (ParamRc.left > 0)
						pDC->LineTo(ParamRc.left - 2, TextRc.bottom);
					else
						pDC->LineTo(ClientRc.right - 3, TextRc.bottom);
				}
				else
					pDC->LineTo(ClientRc.right - 3, TextRc.bottom);

				pDC->SelectObject(pOldPen);
			}
		}
		TextRc.top += GetDefCellHeight();
		TextRc.bottom = TextRc.top + GetDefCellHeight();

		// draw all params 
		for (int nCnt = 0; nCnt < m_arrParams[i].m_arrValue.GetSize(); nCnt++)
		{
			int nOffset = 0;
			int nLast = 0;
			CalcOffset(pDC, nCnt, nOffset, nLast);
			DrawParam(pDC, m_arrParams[i].GetParamRect(nCnt), 
					  m_arrParams[i].GetParamFormat(nCnt),
					  m_arrParams[i].GetParamValue(nCnt),
					  m_arrParams[i].IsValueEmpty(nCnt), nOffset, nLast);
		}

	}
	darkpen.DeleteObject();
/*
	if (VisRect.Width() != 0)
	{
		pDC->DrawEdge(VisRect, BDR_SUNKENINNER, BF_RECT);
	}
*/

	pDC->SetBkColor(clOldBkColor); 
	pDC->SetTextColor(clOldColor); 
	pDC->SetBkMode(nOldMkMode);

	pDC->SelectObject(pOldFont);
}

void CResultBar::DrawParam(CDC *pDC, CRect & rc, CString strFormat, 
						   double fValue, bool bEmpty, int nOffset, int nLast)
{
	if (!pDC)
		return;

	if (!rc.Width())
		return;

	CRect innerRc = rc;
	// draw edge rect
	pDC->DrawEdge(rc, BDR_SUNKENOUTER/*EDGE_SUNKEN*/, BF_RECT);
	innerRc.DeflateRect(2, 2);
	int nLimX = pDC->GetTextExtent(".").cx;
	if (innerRc.Width() < nLimX)
		return;

	CString str = _T("-");
	bool bInt = false;
	if (!bEmpty)
	{
		if (strFormat.Find("%d") != -1)
		{
			str.Format(strFormat, (int)fValue);
			bInt = true;
		}
		else
			str.Format(strFormat, fValue);
	}
	
	CRect rect(0,0,0,0);
	pDC->DrawText(str, rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_CALCRECT);
	
	int nDotX = (innerRc.Width() - nLast) / 2;
	nDotX += innerRc.left;
	nDotX += nLast >= nOffset ? nLast - nOffset : 0;

	int nW = rect.Width();

	if (bEmpty)
	{
		rect.top = innerRc.top;
		rect.bottom = innerRc.bottom;
		rect.left = nDotX - nW / 2 - 1;
		rect.right = rect.left + nW + 1;
	}
	else if (bInt)
	{
		rect.top = innerRc.top;
		rect.bottom = innerRc.bottom;

		rect.left = nDotX - nW - 1;
		rect.right = nDotX + 1;
	}
	else
	{
		rect.top = innerRc.top;
		rect.bottom = innerRc.bottom;

		rect.left = nDotX - (nW - nOffset) - 1;
		rect.right = nDotX + nOffset + 1;
	}
	if (rect.left < innerRc.left)
		rect.left = innerRc.left;

	if (rect.right > innerRc.right)
		rect.right = innerRc.right;

	if (rect.Width() > nLimX)
		pDC->DrawText(str, rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER/*|DT_END_ELLIPSIS*/);

}

void CResultBar::ParamRect(CDC *pDC, CRect & rc, CString strFormat, double fValue, bool bEmpty)
{
	if (!pDC)
		return;

	rc.SetRectEmpty();
	
	if (!bEmpty)
	{
		CString str;
		if (strFormat.Find("%d") != -1)
			str.Format(strFormat, (int)fValue);
		else
			str.Format(strFormat, fValue);
		pDC->DrawText(str, rc, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_CALCRECT);
	}
}


void CResultBar::CalcOffset(CDC *pDC, int nIndex, int & nOffset, int & nLast)
{
	nOffset = nLast = 0;
	if (!pDC)
		return;

	CRect rect(0, 0, 0, 0);
	pDC->DrawText(_T(".000"), &rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_CALCRECT);
	nOffset = rect.Width();

	rect.SetRectEmpty();
	// draw all params 
	for (int nCnt = 0; nCnt < m_arrParams.GetSize(); nCnt++)
	{
		CRect rc(0, 0, 0, 0);
		ParamRect(pDC, rc, m_arrParams[nCnt].GetParamFormat(nIndex),
				  m_arrParams[nCnt].GetParamValue(nIndex),
				  m_arrParams[nCnt].IsValueEmpty(nIndex));

		nLast = nLast > rc.Width() ? nLast : rc.Width();
	}
}


void CResultBar::Show(bool bShow)
{
	m_bEnabled = bShow;

	if (!::IsWindow(GetSafeHwnd()))
		return;

	if (bShow)
	{
		CalcDefCellHeight();

		CRect rc = CalcGridRect();
		if (m_pView)m_pView->CalcLayout();
	}
	else
	{
		Clear();
		if (m_pView)m_pView->CalcLayout();
	}
}

void CResultBar::SetColumnWidth(int nCol, int nWidth)
{
	if (!m_arrParams.GetSize())
		return;

	CRect rc = m_GridRc;
	CalcGridRect();
	SyncParamColumn();

	rc.left = rc.left < m_GridRc.left ? rc.left : m_GridRc.left;
	rc.right = rc.right > m_GridRc.right ? rc.right : m_GridRc.right;
	InvalidateRect(rc);
//	UpdateWindow();
}

bool CResultBar::SyncParamColumn()
{
	if( !m_pView|| !m_bEnabled||m_objects == 0 )
		return false;

	int nFirst = m_pView->colFirstParameter();

	bool bRet = true;
	for (int i = nFirst; i < m_pView->m_Grid.GetColumnCount(); i++)
	{
		CRect cellRect;
		cellRect.SetRectEmpty();

		if (m_pView->m_Grid.GetCellRect(0, i, &cellRect))
		{
			m_pView->m_Grid.ClientToScreen(&cellRect);
			ScreenToClient(&cellRect);
		}
		else
			bRet = false;

		if (cellRect.left <= m_GridRc.left && cellRect.right <= m_GridRc.left)
		{
			cellRect.left = cellRect.right = 0;
		}
		else if (cellRect.left >= m_GridRc.right && cellRect.right >= m_GridRc.right)
		{
			cellRect.left = cellRect.right = 0;
		}
		else
		{
			/*if (cellRect.left <= m_GridRc.left)
				cellRect.left = m_GridRc.left;

			if (cellRect.right < m_GridRc.left + 4)
				cellRect.right = m_GridRc.left;

			if (cellRect.right >= m_GridRc.right)
				cellRect.right = m_GridRc.right;

			if (cellRect.left >= m_GridRc.right - 4)
				cellRect.right = m_GridRc.right;*/
		}

		cellRect.top = m_GridRc.top;
		cellRect.bottom = cellRect.top + GetDefCellHeight();

		
		for (int nCnt = 0; nCnt < m_arrParams.GetSize(); nCnt++)
		{
			m_arrParams[nCnt].SetParamRect(i - nFirst, cellRect);
			cellRect.top += GetDefCellHeight();
			cellRect.bottom += GetDefCellHeight();
		}
	}

	InvalidateRect(m_GridRc);

	return bRet;
}

int CResultBar::GetDefCellHeight()
{
	return m_nDefCellHeight;
}

CRect CResultBar::CalcParamNameRect()
{
	CRect ParamRc, ClientRc;
	GetClientRect(&ClientRc);
	ParamRc = ClientRc;

	CDC *pDC = GetDC();
	if (pDC)
	{
		CFont* pOldFont = pDC->SelectObject(m_pView->m_Grid.GetFont());

		// clac max values for param names width & height
		CSize sizeMax(0, 0);
		for (int i = 0; i < m_arrParams.GetSize(); i++)
		{
			CRect rc;
			rc.SetRectEmpty();
			pDC->DrawText(m_arrParams[i].m_strName, &rc, DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_CALCRECT);
			sizeMax.cx = (sizeMax.cx < rc.Width()) ? rc.Width() : sizeMax.cx;
			sizeMax.cy = (sizeMax.cy < rc.Height()) ? rc.Height() : sizeMax.cy;
		}

		m_nTextMargin = pDC->GetTextExtent("1").cx;
		int nHeight = m_arrParams.GetSize() * GetDefCellHeight();//m_Grid.GetDefCellHeight();

		ParamRc.left += m_nTextMargin;
		ParamRc.right = ParamRc.left + m_nTextMargin + sizeMax.cx;
		ParamRc.bottom -= (ClientRc.Height() - nHeight) / 2;
		ParamRc.top = ParamRc.bottom - nHeight;

		pDC->SelectObject(pOldFont);

		ReleaseDC(pDC);
	}
	if (m_pView && m_pView->m_Grid.GetColumnCount())
	{
		CRect	rect;
		m_pView->m_Grid.GetCellRect( 0, m_pView->m_Grid.GetFixedColumnCount()-1, &rect );

		ParamRc.left = rect.left+3;
		ParamRc.right = rect.right-3;

		/*int nW = m_pView->m_Grid.GetColumnWidth(0);
		ParamRc.left += nW;
		ParamRc.right += nW;
		if (ParamRc.right > ClientRc.right - 3)
			ParamRc.right = ClientRc.right - 3;*/
	}

	return ParamRc;
}

CRect CResultBar::CalcGridRect()
{
	m_ParamRc = CalcParamNameRect();

	CRect GridRc, ClientRc;
	GetClientRect(&ClientRc);

	m_GridRc = m_ParamRc;
	m_GridRc.left = m_ParamRc.right;
	m_GridRc.right = ClientRc.right;

	//if (!m_pView)
		return m_GridRc;

/*	int nFirstPos = m_pView->GetFirstParamPos();
	if (nFirstPos >= m_pView->m_Grid.GetColumnCount())
		return m_GridRc;

	// get first param column x pos
	CPoint FirstPt, LastPt;

	if (!m_pView->m_Grid.GetCellOrigin(0, nFirstPos, &FirstPt))
		return m_GridRc;
	
	FirstPt.y = 0;
	m_pView->m_Grid.ClientToScreen(&FirstPt);
	ScreenToClient(&FirstPt);

	if (FirstPt.x >= (ClientRc.right - m_nTextMargin))
		return m_GridRc;

	else if (FirstPt.x < m_ParamRc.right)
		FirstPt.x = m_ParamRc.right;

	// get last x pos, param column 
	if (!m_pView->m_Grid.GetCellOrigin(0, m_pView->m_Grid.GetColumnCount() - 1, &LastPt))
	{
		LastPt.y = 0;
		LastPt.x = ClientRc.right - m_nTextMargin;
	}
	else
	{
		LastPt.y = 0;
		LastPt.x += m_pView->m_Grid.GetColumnWidth(m_pView->m_Grid.GetColumnCount() - 1);
		m_pView->m_Grid.ClientToScreen(&LastPt);
		ScreenToClient(&LastPt);
	}

	if (LastPt.x <= m_ParamRc.right)
		return m_GridRc;
	

	m_GridRc.left = FirstPt.x;
	m_GridRc.right = LastPt.x;

	return m_GridRc;*/
}

void CResultBar::PasteParams( IStream* pi_stream, int nstart_tabs_count )
{
	if( !pi_stream )	return;

	char sz_buf[1024];	sz_buf[0] = 0;
	DWORD dw_size = 0;
	HRESULT hr = S_OK;

	for (int i = 0; i < m_arrParams.GetSize(); i++)
	{
		//write sciping tabs
		for( int ntabs=0; ntabs<nstart_tabs_count; ntabs++ )
		{
			strcpy( sz_buf, "\t" );
			dw_size = strlen( sz_buf );
			hr = pi_stream->Write( sz_buf, dw_size, &dw_size );
		}
		//write param name
		dw_size = m_arrParams[i].m_strName.GetLength();
		hr = pi_stream->Write( (const char*)m_arrParams[i].m_strName, dw_size, &dw_size );
		
		//write tab
		strcpy( sz_buf, "\t" );
		dw_size = strlen( sz_buf );
		hr = pi_stream->Write( sz_buf, dw_size, &dw_size );

		//write values
		for (int nCnt = 0; nCnt < m_arrParams[i].m_arrValue.GetSize(); nCnt++)
		{
			double fvalue = m_arrParams[i].GetParamValue(nCnt);
			const char* psz_format = m_arrParams[i].GetParamFormat(nCnt);
			sprintf( sz_buf, psz_format, fvalue );
			if( m_arrParams[i].IsValueEmpty(nCnt) )
				strcpy( sz_buf, "-" );

			dw_size = strlen( sz_buf );
			hr = pi_stream->Write( sz_buf, dw_size, &dw_size );
			//write tab
			if( nCnt != m_arrParams[i].m_arrValue.GetSize() - 1 )
			{					
				strcpy( sz_buf, "\t" );
				dw_size = strlen( sz_buf );
				hr = pi_stream->Write( sz_buf, dw_size, &dw_size );
			}
		}
		
		//write new line
		if( i != m_arrParams.GetSize() - 1 )
		{
			strcpy( sz_buf, "\r\n" );
			dw_size = strlen( sz_buf );
			hr = pi_stream->Write( sz_buf, dw_size, &dw_size );
		}
	}
}


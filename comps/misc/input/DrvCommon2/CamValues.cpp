// CamValues.cpp: implementation of the CBaseDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "StdProfile.h"
#include "CamValues.h"
#include "Utils.h"
#include "ImageUtil.h"

const int s_nChainsCount = 16;

CCamValue *CCamValue::m_apFirst[s_nChainsCount] = {0};
int CCamValue::m_nActiveChain = 0;


CCamValue::CCamValue(int nChain, DWORD dwFlags)
{
	m_pNext = m_apFirst[nChain];
	m_apFirst[nChain] = this;
	m_bInit = false;
	m_bWriteToReg = (dwFlags&WriteToReg)!= 0;
	m_bIgnoreMethodic = (dwFlags&IgnoreMethodic)!= 0;
	m_bSkipReinitCamera = (dwFlags&SkipReinitCamera)!=0;
//	m_bFreeData = (dwFlags&FreeData)!=0;
	memset(&m_vh, 0, sizeof(m_vh));
	memset(&m_ReprData, 0, sizeof(m_ReprData));
}

void CCamValue::AddControl(IDrvControl *pControl)
{
	m_arrCtrls.Add(pControl);
}

void CCamValue::RemoveControl(IDrvControl *pControl)
{
	for (int i = m_arrCtrls.GetSize()-1; i >= 0; i--)
		if (m_arrCtrls[i] == pControl)
			m_arrCtrls.RemoveAt(i);
}

void CCamValue::GetValueInfo(VALUEINFOEX2 *pvie2)
{
	pvie2->nType = 0;
	pvie2->pDrvCtrlSite = this;
	pvie2->pHdr = GetHdr();
	pvie2->pInfo = GetVI();
	pvie2->pReprData = GetRepr();
}

void CCamValue::ResetControls()
{
	for (int i = m_arrCtrls.GetSize()-1; i >= 0; i--)
		m_arrCtrls[i]->Reset();
}

void CCamValue::ReinitControls()
{
	for (int i = m_arrCtrls.GetSize()-1; i >= 0; i--)
		m_arrCtrls[i]->Reinit();
}

void CCamValue::_Init(bool bForce)
{
	if (!m_bInit || bForce)
		Init();
	m_bInit = true;
}

void CCamValue::InitChain(int nChain, bool bForce)
{
	for (CCamValue *p = m_apFirst[nChain]; p != NULL; p = p->m_pNext)
		p->_Init(bForce);
}

CCamValue *CCamValue::FindByName(LPCTSTR lpstrSec, LPCTSTR lpstrEntry)
{
	int nChain = m_nActiveChain;
	for (CCamValue *p = m_apFirst[nChain]; p != NULL; p = p->m_pNext)
	{
		if (p->GetHdr()->pszSection != NULL && !_tcsicmp(lpstrSec, p->GetHdr()->pszSection) &&
			p->GetHdr()->pszEntry != NULL && !_tcsicmp(lpstrEntry, p->GetHdr()->pszEntry))
			return p;
	}
	return NULL;
}

CCamValue *CCamValue::FindById(int nDlgCtrlId)
{
	int nChain = m_nActiveChain;
	for (CCamValue *p = m_apFirst[nChain]; p != NULL; p = p->m_pNext)
	{
		if (p->GetRepr()->nDlgCtrlId == nDlgCtrlId)
			return p;
	}
	return NULL;
}

bool CCamValue::CheckByName(LPCTSTR lpstrSec, LPCTSTR lpstrEntry)
{
	if (GetHdr()->pszSection != NULL && !_tcsicmp(lpstrSec, GetHdr()->pszSection) &&
		GetHdr()->pszEntry != NULL && !_tcsicmp(lpstrEntry, GetHdr()->pszEntry))
		return true;
	else
		return false;
}

bool CCamValue::CheckByName(BSTR bstrSec, BSTR bstrEntry)
{
	_bstr_t bstrSec1(bstrSec);
	_bstr_t bstrEntry1(bstrEntry);
	return CheckByName((LPCTSTR)bstrSec1, (LPCTSTR)bstrEntry1);
}

void CValuesGroup::InitSection(LPCTSTR lpstrSecName)
{
	for (int i = 0; i < m_arrValues.GetSize(); i++)
	{
		CCamValue *p = m_arrValues.GetAt(i);
		p->GetHdr()->pszSection = lpstrSecName;
		p->_Init(true);
	}
}


CCamStringValue::CCamStringValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, LPCTSTR lpstrDef, int nBuffSize,
	CReprType ReprType, int nChain,	int nDlgCtrl, DWORD dwFlags) :
	CCamValue(nChain,dwFlags)
{
	m_vh.pszSection = lpstrSec;
	m_vh.pszEntry = lpstrEntry;
	m_vi.pszDefault = lpstrDef;
	m_vi.pszBuffer = new char[nBuffSize+1];
	m_vi.nMaxBuffer = nBuffSize;
	strncpy(m_vi.pszBuffer, lpstrDef, m_vi.nMaxBuffer);
	m_vi.nType = ValueType_Text;
	m_ReprData.Type = ReprType;
	m_ReprData.nDlgCtrlId = nDlgCtrl;
}

CCamStringValue::~CCamStringValue()
{
	delete m_vi.pszBuffer;
}

void CCamStringValue::Init()
{
	if (m_vh.pszSection != NULL && m_vh.pszEntry != NULL)
	{
		CString s = CStdProfileManager::m_pMgr->GetProfileString(m_vh.pszSection, m_vh.pszEntry,
			m_vi.pszDefault, m_bWriteToReg, m_bIgnoreMethodic);
		strncpy(m_vi.pszBuffer, (LPCTSTR)s, m_vi.nMaxBuffer);
	}
}

void CCamStringValue::Save()
{
	if (m_vh.pszSection != NULL && m_vh.pszEntry != NULL)
	{
		CStdProfileManager::m_pMgr->WriteProfileString(m_vh.pszSection, m_vh.pszEntry, m_vi.pszBuffer,
			m_bIgnoreMethodic);
	}
}

const char *CCamStringValue::operator =(const char *p)
{
	SetValueString(p, true);
	return m_vi.pszBuffer;
}

void CCamStringValue::SetValueString(const char *p, bool bReset)
{
	strncpy(m_vi.pszBuffer, p, m_vi.nMaxBuffer);
	Save();
	if (bReset) ResetControls();
}


CCamIntValue::CCamIntValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault, int nMin,
	int nMax, CReprType ReprType, int nChain, int nDlgCtrl, DWORD dwFlags) :
	CCamValue(nChain,dwFlags)
{
	m_vh.pszSection = lpstrSec;
	m_vh.pszEntry = lpstrEntry;
	m_vi.nMin = nMin;
	m_vi.nMax = nMax;
	m_vi.nDefault = nDefault;
	m_vi.nCurrent = nDefault;
	m_vi.nType = ValueType_Int;
	m_ReprData.Type = ReprType;
	m_ReprData.nDlgCtrlId = nDlgCtrl;
}


CCamIntValue::operator int()
{
	_Init();
	return m_vi.nCurrent;
}

int CCamIntValue::operator=(int n)
{
	SetValueInt(n, true);
	return m_vi.nCurrent;
}

void CCamIntValue::SetValueInt(int n, bool bReset)
{
	m_vi.nCurrent = Range(n, m_vi.nMin, m_vi.nMax);
	Save();
	if (bReset) ResetControls();
}

void CCamIntValue::Init()
{
	if (m_vh.pszSection != NULL && m_vh.pszEntry != NULL)
		m_vi.nCurrent = CStdProfileManager::m_pMgr->GetProfileInt(m_vh.pszSection, m_vh.pszEntry,
			m_vi.nDefault, m_bWriteToReg, m_bIgnoreMethodic);
	if (m_vi.nCurrent < m_vi.nMin || m_vi.nCurrent > m_vi.nMax)
		m_vi.nCurrent = m_vi.nDefault;
}

void CCamIntValue::Save()
{
	if (m_vh.pszSection != NULL && m_vh.pszEntry != NULL)
		CStdProfileManager::m_pMgr->WriteProfileInt(m_vh.pszSection, m_vh.pszEntry, m_vi.nCurrent,
			m_bIgnoreMethodic);
}


void CCamIntValue::InitScope(int nMin, int nMax, int nDef)
{
	m_vi.nMax = nMax;
	m_vi.nMin = nMin;
	m_vi.nDefault = nDef;
	if (m_vi.nCurrent < m_vi.nMin) m_vi.nCurrent = m_vi.nMin;
	if (m_vi.nCurrent > m_vi.nMax) m_vi.nCurrent = m_vi.nMax;
}


CCamIntValue2::CCamIntValue2(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault, int nMin,
	int nMax, int nStep1, int nStep2, int nChain, int nDlgCtrl, DWORD dwFlags) :
	CCamIntValue(lpstrSec, lpstrEntry, nDefault, nMin, nMax, IRepr_SmartIntEdit2, nChain,
	nDlgCtrl, dwFlags)
{
	m_SIRD2.Type = IRepr_SmartIntEdit2;
	m_SIRD2.nDlgCtrlId = nDlgCtrl;
	m_SIRD2.nStep1 = nStep1;
	m_SIRD2.nStep2 = nStep2;
}

static void _RightMinMax(int &nMin, int &nMax)
{
	bool bSetMin = true;
	bool bSetMax = false;
	for (int n = 1; ; n *= 10)
	{
		if (bSetMin && n >= nMin)
		{
			nMin = n;
			bSetMin = false;
			bSetMax = true;
		}
		if (bSetMax && n*10 > nMax)
		{
			nMax = n;
			break;
		}
	}
}

void CCamIntValue2::InitScope(int nMin, int nMax, int nDef)
{
	_RightMinMax(nMin, nMax);
	m_vi.nMax = nMax;
	m_vi.nMin = nMin;
	if (nMax/nMin>500)
		m_SIRD2.nStep1 = nMax/100;
	else
		m_SIRD2.nStep1 = nMax/10;
	m_SIRD2.nStep2 = nMin;
	m_vi.nDefault = min(max(nMin,nDef),nMax);
	if (m_vi.nCurrent < m_vi.nMin) m_vi.nCurrent = m_vi.nMin;
	if (m_vi.nCurrent > m_vi.nMax) m_vi.nCurrent = m_vi.nMax;
}

CCamBoolValue::CCamBoolValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, bool bDefault, int nChain,
	int nDlgCtrl, DWORD dwFlags) : CCamIntValue(lpstrSec, lpstrEntry, bDefault?1:0, 0, 1,
	IRepr_Checkbox, nChain, nDlgCtrl, dwFlags)
{
}

CCamBoolValue::operator bool()
{
	_Init();
	return m_vi.nCurrent!=0;
}

bool CCamBoolValue::operator=(bool b)
{
	SetValueInt(b?1:0, true);
	return b;
}

void CCamBoolValue::SetValueInt(int n, bool bReset)
{
	m_vi.nCurrent = n>0?1:0;
	Save();
	if (bReset) ResetControls();
}

CCamIntComboValue::CCamIntComboValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, INTCOMBOITEMDESCR *pArray,
	int nValuesCount, int nValueDef, int nChain, int nDlgCtrl, DWORD dwFlags) :
	CCamIntValue(lpstrSec, lpstrEntry, 0, 0, nValuesCount-1, IRepr_Combobox, nChain, nDlgCtrl, dwFlags)
{
	m_CBoxRData.Type = IRepr_Combobox;
	m_CBoxRData.nDlgCtrlId = nDlgCtrl;
	m_CBoxRData.ComboType = CBox_Integer;
	m_CBoxRData.nValues = nValuesCount;
	m_CBoxRData.pnValues = pArray;
	m_vi.nDefault = IndexByValue(nValueDef);
}

CCamIntComboValue::operator int()
{
	_Init();
	return ValueByIndex(m_vi.nCurrent);
}

int CCamIntComboValue::ValueByIndex(int nIndex)
{
	int n = min(max(0,nIndex),m_CBoxRData.nValues-1);
	return m_CBoxRData.pnValues[n].nValue;
}

int CCamIntComboValue::IndexByValue(int nValue)
{
	for (int i = 0; i < m_CBoxRData.nValues; i++)
	{
		if (m_CBoxRData.pnValues[i].nValue == nValue)
			return i;
	}
	return 0;
}

CCamIntComboValue2::CCamIntComboValue2(LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
	int nValueDef, int nChain, int nDlgCtrl, DWORD dwFlags) :
	CCamIntComboValue(lpstrSec, lpstrEntry, NULL, 0, nValueDef, nChain, nDlgCtrl, dwFlags)
{
}

CCamIntComboValue2::~CCamIntComboValue2()
{
	for (int i = 0; i < m_Data.GetSize(); i++)
	{
		if (m_Data.ElementAt(i).lpstrValue != NULL)
			free((void*)m_Data.ElementAt(i).lpstrValue);
	}
}

void CCamIntComboValue2::Add(int nVal, LPCTSTR lpstrString, int idString)
{
	m_Data.SetSize(m_Data.GetSize()+1);
	int n = m_Data.GetSize()-1;
	m_Data.ElementAt(n).nValue = nVal;
	m_Data.ElementAt(n).lpstrValue = lpstrString==NULL?NULL:_tcsdup(lpstrString);
	m_Data.ElementAt(n).idString = idString;
	m_CBoxRData.nValues = m_Data.GetSize();
	m_CBoxRData.pnValues = m_Data.GetData();
	m_vi.nMin = 0;
	m_vi.nMax = m_Data.GetSize()-1;
}

CCamDoubleValue::CCamDoubleValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, double dDefault, 
	double dMin, double dMax, CReprType ReprType, int nChain, int nDlgCtrl, DWORD dwFlags) :
	CCamValue(nChain,dwFlags)
{
	m_vh.pszSection = lpstrSec;
	m_vh.pszEntry = lpstrEntry;
	m_vi.dMin = dMin;
	m_vi.dMax = dMax;
	m_vi.dDefault = dDefault;
	m_vi.dCurrent = dDefault;
	m_vi.nType = ValueType_Double;
	m_ReprData.Type = ReprType;
	m_ReprData.nDlgCtrlId = nDlgCtrl;
}

CCamDoubleValue::operator double()
{
	_Init();
	return m_vi.dCurrent;
}

double CCamDoubleValue::operator=(double d)
{
	SetValueDouble(d, true);
	return m_vi.dCurrent;
}

void CCamDoubleValue::SetValueDouble(double d, bool bReset)
{
	m_vi.dCurrent = RangeD(d, m_vi.dMin, m_vi.dMax);
	Save();
	if (bReset) ResetControls();
}

void CCamDoubleValue::Init()
{
	m_vi.dCurrent = CStdProfileManager::m_pMgr->GetProfileDouble(m_vh.pszSection, m_vh.pszEntry,
		m_vi.dDefault, NULL, m_bWriteToReg, m_bIgnoreMethodic);
	if (m_vi.dCurrent < m_vi.dMin || m_vi.dCurrent > m_vi.dMax)
		m_vi.dCurrent = m_vi.dDefault;
}

void CCamDoubleValue::Save()
{
	if (m_vh.pszSection != NULL && m_vh.pszEntry != NULL)
		CStdProfileManager::m_pMgr->WriteProfileDouble(m_vh.pszSection, m_vh.pszEntry,
			m_vi.dCurrent, m_bIgnoreMethodic);
}

void CCamDoubleValue::InitScope(double dMin, double dMax, double dDef)
{
	m_vi.dMin = dMin;
	m_vi.dMax = dMax;
	m_vi.dDefault = dDef;
	if (m_vi.dCurrent < m_vi.dMin) m_vi.dCurrent = m_vi.dMin;
	if (m_vi.dCurrent > m_vi.dMax) m_vi.dCurrent = m_vi.dMax;
}

CCamValueRect *CCamValueRect::m_apFirstRect[s_nChainsCount];

CCamValueRect::CCamValueRect(LPCTSTR lpstrSec, int nChain, int nDlgCtrl, DWORD dwFlags) :
	CCamBoolValue(lpstrSec, _T("Enable"), false, nChain, nDlgCtrl, dwFlags)
{
	m_pNextRect = m_apFirstRect[nChain];
	m_apFirstRect[nChain] = this;
}

RECT CCamValueRect::operator=(RECT rc)
{
	m_rc = rc;
	Save();
	return m_rc;
}

CCamValueRect::operator RECT()
{
	return m_rc;
}

void CCamValueRect::Init()
{
	CCamBoolValue::Init();
	if (m_vi.nCurrent > 0)
	{
		int l = CStdProfileManager::m_pMgr->GetProfileInt(m_vh.pszSection, _T("Left"), INT_MIN, false, m_bIgnoreMethodic);
		int t = CStdProfileManager::m_pMgr->GetProfileInt(m_vh.pszSection, _T("Top"), INT_MIN, false, m_bIgnoreMethodic);
		int r = CStdProfileManager::m_pMgr->GetProfileInt(m_vh.pszSection, _T("Right"), INT_MIN, false, m_bIgnoreMethodic);
		int b = CStdProfileManager::m_pMgr->GetProfileInt(m_vh.pszSection, _T("Bottom"), INT_MIN, false, m_bIgnoreMethodic);
		if (l == INT_MIN || t == INT_MIN || r == INT_MIN || b == INT_MIN)
			m_rc = CRect(100,100,200,200);
		else
		{
			m_rc.left   = min(l,r);
			m_rc.top    = min(t,b);
			m_rc.right  = max(l,r);
			m_rc.bottom = max(t,b);
		}
	}
	if (m_vi.nCurrent == 0)
		m_rc = CRect(0,0,0,0);
}

void CCamValueRect::SetValueInt(int n, bool bReset)
{
	m_vi.nCurrent = n;
	CCamIntValue::Save();
	Init();
}


void CCamValueRect::Save()
{
	CCamBoolValue::Save();
	if (IsEnabled())
	{
		CStdProfileManager::m_pMgr->WriteProfileInt(m_vh.pszSection, _T("Left"), m_rc.left, m_bIgnoreMethodic);
		CStdProfileManager::m_pMgr->WriteProfileInt(m_vh.pszSection, _T("Top"), m_rc.top, m_bIgnoreMethodic);
		CStdProfileManager::m_pMgr->WriteProfileInt(m_vh.pszSection, _T("Right"), m_rc.right, m_bIgnoreMethodic);
		CStdProfileManager::m_pMgr->WriteProfileInt(m_vh.pszSection, _T("Bottom"), m_rc.bottom, m_bIgnoreMethodic);
	}
}

bool CCamValueRect::IsEnabled()
{
	return (bool)(*this);
}

void CCamValueRect::Enable(bool bEnable)
{
	CCamBoolValue::operator =(bEnable);
}

CRect CCamValueRect::SafeGetRect(int cx, int cy)
{
	_Init();
	if (!IsEnabled())
		return CRect(0,0,cx,cy);
	else
	{
		int l = min(m_rc.left,cx);
		int r = min(m_rc.right,cx);
		int t = min(m_rc.top,cy);
		int b = min(m_rc.bottom,cy);
		return CRect(l,t,r,b);
	}
}

void CCamValueRect::MapAllRects(LPBITMAPINFOHEADER lpbi)
{
	for (CCamValueRect *p = m_apFirstRect[m_nActiveChain]; p != NULL; p = p->m_pNextRect)
	{
		if (p->NeedDrawRect())
		{
			int l = max(0,min(p->m_rc.left,lpbi->biWidth-1));
			int r = max(0,min(p->m_rc.right,lpbi->biWidth-1));
			int t = max(0,min(p->m_rc.top,lpbi->biHeight-1));
			int b = max(0,min(p->m_rc.bottom,lpbi->biHeight-1));
			MapRectOnDIB(lpbi, CRect(l,t,r,b), RGB(255,255,255));
			if (r-l>2 && b-t>2 && l < lpbi->biWidth-1 && t < lpbi->biHeight-1)
				MapRectOnDIB(lpbi, CRect(l+1,t+1,r-1,b-1), RGB(0,0,0));
		}
	}
}

void CCamValueRect::DrawAllRects(CDC *pDC, LPRECT prcSrc, LPRECT prcDst)
{
	double dZoomX = double(prcDst->right-prcDst->left)/double(prcSrc->right-prcSrc->left);
	double dZoomY = double(prcDst->bottom-prcDst->top)/double(prcSrc->bottom-prcSrc->top);
	int nOffsX = prcDst->left-(int)(prcSrc->left*dZoomX);
	int nOffsY = prcDst->top-(int)(prcSrc->top*dZoomY);
	for (CCamValueRect *p = m_apFirstRect[m_nActiveChain]; p != NULL; p = p->m_pNextRect)
	{
		if (p->NeedDrawRect())
		{
			int l = int(p->m_rc.left*dZoomX)+nOffsX;
			int t = int(p->m_rc.top*dZoomY)+nOffsY;
			int r = int(p->m_rc.right*dZoomX)+nOffsX;
			int b = int(p->m_rc.bottom*dZoomY)+nOffsY;
			if (r-l > 2 && b-t > 2)
			{
				CPen *pOldPen = pDC->SelectObject(CPen::FromHandle((HPEN)GetStockObject(WHITE_PEN)));
				pDC->MoveTo(l,t);
				pDC->LineTo(r,t);
				pDC->LineTo(r,b);
				pDC->LineTo(l,b);
				pDC->LineTo(l,t);
				if (r-l>4 && b-t>4)
				{
					
					
					pDC->SelectObject(CPen::FromHandle((HPEN)GetStockObject(BLACK_PEN)));

                    pDC->MoveTo(l+1,t+1);
					pDC->LineTo(r-1,t+1);
					pDC->LineTo(r-1,b-1);
					pDC->LineTo(l+1,b-1);
					pDC->LineTo(l+1,t+1);
				}
				pDC->SelectObject(pOldPen);
			}
		}
	}
}


CCamIntValue g_CamRectZoneDist(_T("General"), _T("RectZoneSize"), 5, 1, 20);



CRectZone CCamValueRect::RectZone(CPoint pt)
{
	int n = g_CamRectZoneDist;
	int rzx = 0, rzy = 0;
	if (pt.x >= m_rc.left-n && pt.x <= min(m_rc.left+n,(m_rc.left+m_rc.right)/2))
		rzx = 1;
	else if (pt.x >= max(m_rc.right-n,(m_rc.left+m_rc.right)/2) && pt.x <= m_rc.right+n)
		rzx = 2;
	else if (pt.x > m_rc.left && pt.x < m_rc.right)
		rzx = 3;
	if (pt.y >= m_rc.top-n && pt.y <= min(m_rc.top+n,(m_rc.top+m_rc.bottom)/2))
		rzy = 4;
	else if (pt.y >= max(m_rc.bottom-n,((m_rc.top+m_rc.bottom)/2)) && pt.y <= m_rc.bottom+n)
		rzy = 8;
	else if (pt.y >= m_rc.top && pt.y <= m_rc.bottom)
		rzy = 12;
	if (rzx == 0 || rzy == 0)
		return Rect_None;
	else
		return (CRectZone)(rzx|rzy);
}

bool CCamValueRect::FindRect(CPoint pt, CRectZone *prz, CCamValueRect **ppVal)
{
	CRectZone rz = Rect_None;
	CCamValueRect *pVal = NULL;
	for (CCamValueRect *p = m_apFirstRect[m_nActiveChain]; p != NULL; p = p->m_pNextRect)
	{
		if (p->NeedDrawRect())
		{
			CRectZone rz1 = p->RectZone(pt);
			if (rz1 == Rect_Move)
			{
				if (rz == Rect_None)
				{
					pVal = p;
					rz = rz1;
				}
			}
			else if (rz1 != Rect_None)
			{
				*prz = rz1;
				*ppVal = p;
				return true;
			}
		}
	}
	*prz = rz;
	*ppVal = pVal;
	return rz!=Rect_None;
}

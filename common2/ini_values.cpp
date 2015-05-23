#include "stdafx.h"
#include "ini_values.h"
#include "misc_utils.h"
//#include "misc5.h"
//#include "data5.h"
//#include "stdio.h"
#include "com_main.h"
//#include "helper.h"
//#include "core5.h"
//#include "image5.h"
//#include "docview5.h"
//#include "action5.h"
//#include "nameconsts.h"
//#include "misc_list.h"
//#include "PropBag.h"
//#include "alloc.h"




CIniValue *CIniValue::s_pFirst = NULL;

CIniValue::CIniValue(LPCTSTR lpSection, LPCTSTR lpEntry)
{
	m_lpSection = lpSection;
	m_lpEntry = lpEntry;
	m_bInit = false;
	m_pNext = s_pFirst;
	s_pFirst = this;
}

CIniValue::~CIniValue()
{
	if (s_pFirst == this)
	{
		s_pFirst = m_pNext;
		return;
	}
	dbg_assert(s_pFirst->m_pNext != NULL);
	for (CIniValue *pPrev = s_pFirst, *p = s_pFirst->m_pNext; p; pPrev = p, p = p->m_pNext)
	{
		if (p == this)
		{
			dbg_assert(pPrev->m_pNext == p);
			pPrev->m_pNext = p->m_pNext;
			return;
		}
	}
}


void CIniValue::OnReset()
{
	m_bInit = false;
}

void CIniValue::Reset()
{
	for (CIniValue *p = s_pFirst; p; p = p->m_pNext)
		p->OnReset();
}


CIntIniValue::CIntIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, int nDefault) : CIniValue(lpSection,lpEntry)
{
	m_nValue = nDefault;
}

CIntIniValue::operator int()
{
	if (!m_bInit)
	{
		m_nValue = ::GetValueInt(::GetAppUnknown(), m_lpSection, m_lpEntry, m_nValue);
		m_bInit = true;
	}
	return m_nValue;
}

CColorIniValue::CColorIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF clrDefault) : CIniValue(lpSection,lpEntry)
{
	m_clr = clrDefault;
}

CColorIniValue::operator COLORREF()
{
	if (!m_bInit)
	{
		m_clr = ::GetValueColor(::GetAppUnknown(), m_lpSection, m_lpEntry, m_clr);
		m_bInit = true;
	}
	return m_clr;
}

#include "StdAfx.h"
#include "CamDefaults.h"
#include "StdProfile.h"
#include "Utils.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCamValues::CCamValues()
{
	m_pValueInfos = NULL;
	m_nValueInfosCount = 0;
}

HRESULT CCamValues::GetValueInfo(int nValue, VALUEINFOEX *pValueInfo)
{
	*pValueInfo = m_pValueInfos[nValue];
	return S_OK;
}

HRESULT CCamValues::GetValueInfoByName(const char *pszSection, const char *pszEntry, VALUEINFOEX *pValueInfo)
{
	if (m_pValueInfos)
	{
		for (int i = 0; i < m_nValueInfosCount; i++)
		{
			if (!strcmp(m_pValueInfos[i].Hdr.pszSection,pszSection) &&
				!strcmp(m_pValueInfos[i].Hdr.pszEntry,pszEntry))
			{
				*pValueInfo = m_pValueInfos[i];
				int n = CStdProfileManager::m_pMgr->GetProfileInt(pszSection, pszEntry, pValueInfo->Info.nDefault);
				pValueInfo->Info.nCurrent = Range(n,pValueInfo->Info.nMin,pValueInfo->Info.nMax);
				return S_OK;
			}
		}
	}
	return E_INVALIDARG;
}



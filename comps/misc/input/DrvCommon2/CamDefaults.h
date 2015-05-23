#if !defined(__CamDefaults_H__)
#define __CamDefaults_H__

#include "Input.h"

class CCamValues
{
protected:
	VALUEINFOEX *m_pValueInfos;
	int m_nValueInfosCount;
public:
	CCamValues();
	HRESULT GetValuesCount(int *pnValues)
	{
		*pnValues = m_nValueInfosCount;
		return S_OK;
	}
	HRESULT GetValueInfo(int nValue, VALUEINFOEX *pValueInfo);
	HRESULT GetValueInfoByName(const char *pszSection, const char *pszEntry, VALUEINFOEX *pValueInfo);
};

#endif

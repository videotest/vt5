#include "stdafx.h"
#include "FilterPropBase.h"
#include "resource.h"

int  CFilterPropBase::_GetPropertyCount()
{
	return 6;
}

void CFilterPropBase::_GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx)
{

	char strName[255];
	DWORD dwFlags = 0;
	DWORD dwGroup = 0;
	_variant_t var;
	switch (nNum)
	{
	case 0:
		dwGroup = 1;
		var = (long)m_nWidth;
		::LoadString(App::handle(), IDS_WIDTH, (char*)strName, 255);
		break;
	case 1:
		dwGroup = 1;
		var = (long)m_nHeight;
		::LoadString(App::handle(), IDS_HEIGHT, (char*)strName, 255);
		break;
	case 2:
		dwGroup = 2;
		var = (long)m_nDepth;
		dwFlags = ffpfTextAfterProp;
		::LoadString(App::handle(), IDS_DEPTH, (char*)strName, 255);
		break;
	case 3:
		dwGroup = 2;
		var = (long)m_nColors;
		dwFlags = ffpfTextAfterProp|ffpfNeedBrackets;
		::LoadString(App::handle(), IDS_COLORS, (char*)strName, 255);
		break;
	case 4:
		var = m_strCompression;
		::LoadString(App::handle(), IDS_COMPRESS, (char*)strName, 255);
		break;
	case 5:
		var = (long)m_nNumPages;
		::LoadString(App::handle(), IDS_PAGES, (char*)strName, 255);
		break;
	}
	if(pbstrName)
		*pbstrName = _bstr_t(strName);
	if(pGroupIdx)
		*pGroupIdx = dwGroup;
	if(pdwFlags)
		*pdwFlags = dwFlags;
	if(pvarVal)
		*pvarVal = var.Detach();
}

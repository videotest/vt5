#include "stdafx.h"
#include "common.h"
#include "misc_utils.h"
#include "PropBag.h"

/////////////////////////////////////////////////////////////////////////////
std_dll bool RecursiveWriteEntry( INamedData* ptrSrc, INamedData* ptrTarget, LPCSTR pszEntry, bool bOvewrite )
{
	if( !ptrSrc || !ptrTarget || !pszEntry )
		return false;

	_bstr_t bstrPath( pszEntry );

	// set section
	if( S_OK != ptrSrc->SetupSection( bstrPath ) )
		return false;
	
	// get count of entries
	long EntryCount = 0;
	ptrSrc->GetEntriesCount( &EntryCount );

	// for all entries
	for( int i = 0;i<(int)EntryCount;i++ )
	{
		BSTR bstr = NULL;
		// get entry name
		
		ptrSrc->GetEntryName( i, &bstr );
		_bstr_t bstrEntryName = bstr;
		::SysFreeString( bstr );	bstr = 0;
		
		// format a full path for entry
		_bstr_t bstrPathNew;
		int nLen = bstrPath.length();

		if( nLen > 0 && ((char*)bstrPath)[nLen-1] == '\\' )
			bstrPathNew = bstrPath + bstrEntryName;
		else
			bstrPathNew = bstrPath + "\\" + bstrEntryName;

		// get value
		_variant_t var;
		ptrSrc->GetValue( bstrPathNew, &var );

		bool bCanSetValue = true;
		if( !bOvewrite )
		{
			_variant_t _var;
			ptrTarget->GetValue( bstrPathNew, &_var );

			bCanSetValue = ( _var.vt == VT_EMPTY );				
		}
			
		if( bCanSetValue )
		{
			ptrTarget->SetupSection( bstrPathNew );
			// A.M. fix. BT 4050
			if( var.vt == VT_BITS )
			{
				byte	*pdata = (byte*)var.lVal;
				long	lSize = *(long*)pdata;
				byte	*pdata1 = new byte[sizeof( lSize )+lSize];
				//*(long*)pdata1 = lSize;
				memcpy( pdata1, pdata, lSize+sizeof( lSize ) );
				var = _variant_t((long)pdata1);
				var.vt = VT_BITS;
			}
			ptrTarget->SetValue( bstrPathNew, var );
		}
		
		// if this entry has children => we want to walk to them
		if( EntryCount > 0 )
			RecursiveWriteEntry( ptrSrc, ptrTarget, bstrPathNew, bOvewrite );

		// for next entry on this level restore Section
		ptrSrc->SetupSection( bstrPath );
	}
		
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CopyObjectNamedData( IUnknown* punk_src, IUnknown* punk_target, LPCSTR pszEntry, bool bOvewrite )
{
	INamedDataPtr ptr_src_nd( punk_src );
	if( ptr_src_nd == 0 )
		return false;

	INamedDataPtr ptr_target_nd( punk_target );
	if( ptr_target_nd == 0 )
	{
		INamedDataObject2Ptr ptr_target_ndo2( punk_target );
		if( ptr_target_ndo2 )				
			ptr_target_ndo2->InitAttachedData();
		
		ptr_target_nd = punk_target;
	}

	if( ptr_target_nd == 0 )
		return false;
		
	return RecursiveWriteEntry( ptr_src_nd, ptr_target_nd, pszEntry, bOvewrite );
}

std_dll int GetImagePaneCount( IUnknown *punkImage )
{
	if( !punkImage )
		return -1;

	IImage4Ptr sptrImage = punkImage;

	if( sptrImage == 0 )
		return -1;

	int nCount = -1;
	
	sptrImage->GetPanesCount( &nCount );
	return nCount;
}

std_dll void CopyPropertyBag(  IUnknown* punk_src, IUnknown* punk_target, bool bOvewrite )
{
	INamedPropBagPtr sptrNPGSrc(punk_src);
	INamedPropBagPtr sptrNPGTrg(punk_target);
    if(sptrNPGSrc==0 || sptrNPGTrg==0) return; // мало ли - avi, например, пропбэга не имеет
	long lpos = 0;
	sptrNPGSrc->GetFirstPropertyPos(&lpos);
	while (lpos)
	{
		_variant_t var;
		_bstr_t bstrName;
		sptrNPGSrc->GetNextProperty(bstrName.GetAddress(), &var, &lpos);
		if (!bOvewrite)
		{
			_variant_t var1;
			sptrNPGTrg->GetProperty(bstrName, &var1);
			if (var1.vt != VT_EMPTY)
				continue;
		}
		sptrNPGTrg->SetProperty(bstrName, var);
	}
}

std_dll RECT GetObjectRectangle(IUnknown *punkObject, int nMode)
{
	CRect rc(0,0,0,0);
	IRectObjectPtr sptrRO(punkObject);
	if (sptrRO != 0)
	{
		CRect rc1;
		if (sptrRO->GetRect(&rc1) == S_OK)
			rc = rc1;
	}
	if (nMode == 0) return rc;
	INamedDataObject2Ptr sptrNDO2Obj(punkObject);
	if (sptrNDO2Obj == 0)
		return rc;
	POSITION lpos = 0;
	sptrNDO2Obj->GetFirstChildPosition(&lpos);
	while (lpos)
	{
		IUnknownPtr punk;
		sptrNDO2Obj->GetNextChild(&lpos, &punk);
		CRect rcChild = GetObjectRectangle(punk, nMode);
		if (rcChild != NORECT)
		{
			if (rc == NORECT)
				rc = rcChild;
			else
			{
				rc.left = min(rc.left,rcChild.left);
				rc.right = max(rc.right,rcChild.right);
				rc.top = min(rc.top, rcChild.top);
				rc.bottom = max(rc.bottom, rcChild.bottom);
			}
		}
	}
	return rc;
}


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
	ASSERT(s_pFirst->m_pNext != NULL);
	for (CIniValue *pPrev = s_pFirst, *p = s_pFirst->m_pNext; p; pPrev = p, p = p->m_pNext)
	{
		if (p == this)
		{
			ASSERT(pPrev->m_pNext == p);
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

CDoubleIniValue::CDoubleIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, double fDefault) : CIniValue(lpSection,lpEntry)
{
	m_fValue = fDefault;
}

CDoubleIniValue::operator double()
{
	if (!m_bInit)
	{
		m_fValue = ::GetValueDouble(::GetAppUnknown(), m_lpSection, m_lpEntry, m_fValue);
		m_bInit = true;
	}
	return m_fValue;
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


CDisableAutoSelect::CDisableAutoSelect()
{
	m_bPrev = ::GetValueInt(::GetAppUnknown(), "DataContext", "AutoSelectOnEmpty", TRUE);
	::SetValue(::GetAppUnknown(), "DataContext", "AutoSelectOnEmpty", (long)FALSE);
}

CDisableAutoSelect::~CDisableAutoSelect()
{
	::SetValue(::GetAppUnknown(), "DataContext", "AutoSelectOnEmpty", (long)m_bPrev);
}

bool IsNeedSaveBackup( LPCTSTR pszFileName )
{
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(pszFileName, NULL, NULL, NULL, szExt);
	TCHAR *pExt = szExt[0]==_T('.')?szExt+1:szExt;
	if (pExt[0] && GetValueInt(GetAppUnknown(), "\\SaveBackups", pExt, 0))
		return true;
	else
		return false;
}



// clipboard.cpp: implementation of the CClipboardImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "common.h"
#include "clipboard.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_UNKNOWN_BASE(CClipboardImpl, Clipboard);


CClipboardImpl::CClipboardImpl()
{
	m_bSupportPaste = true;
}

CClipboardImpl::~CClipboardImpl()
{
	m_arrAvailFormats.RemoveAll();
}

bool CClipboardImpl::CheckFormat(UINT nFormat)
{
	for(int i = 0; i < m_arrAvailFormats.GetSize(); i++)
	{
		if(m_arrAvailFormats[i] == nFormat)
		{
			return true;
		}
	}
	return false;
}
		
//////////////////////////////////////////////////////////////////////
// IClipboard implementation
HRESULT CClipboardImpl::XClipboard::GetFirstFormatPosition(long *pnPos)
{
	_try_nested_base(CClipboardImpl, Clipboard, GetFirstFormatPosition)
	{
		if(pnPos)
		{
			*pnPos = pThis->m_arrAvailFormats.GetSize();
			return S_OK;
		}
		else
			return E_INVALIDARG;
	}
	_catch_nested;
}

HRESULT CClipboardImpl::XClipboard::GetNextFormat(UINT* pnFormat, long *pnPosNext)
{
	_try_nested_base(CClipboardImpl, Clipboard, GetNextFormat)
	{
		if(*pnPosNext < 0 || pThis->m_arrAvailFormats.GetSize() < *pnPosNext)
			return E_INVALIDARG;
		if(pnFormat)
		{
			*pnFormat = pThis->m_arrAvailFormats[(*pnPosNext)-1];
			(*pnPosNext)--;
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CClipboardImpl::XClipboard::Copy(UINT nFormat, HANDLE* pHandle)
{
	_try_nested_base(CClipboardImpl, Clipboard, Copy)
	{
		if(pThis->CheckFormat(nFormat))
		{
			return pThis->Copy(nFormat, pHandle)?S_OK:E_INVALIDARG;
			
		}
		else
			return E_INVALIDARG;
	}
	_catch_nested;
}

HRESULT CClipboardImpl::XClipboard::Paste(UINT nFormat, HANDLE Handle)
{
	_try_nested_base(CClipboardImpl, Clipboard, Paste)
	{
		ASSERT(pThis->m_bSupportPaste == true);

		if(pThis->CheckFormat(nFormat))
		{
			return pThis->Paste(nFormat, Handle)?S_OK:E_INVALIDARG;
		}
		else
			return E_INVALIDARG;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CClipboardImpl::XClipboard::IsSupportPaste(long* pboolSupportPaste)
{
	_try_nested_base(CClipboardImpl, Clipboard, Paste)
	{
		if(pboolSupportPaste)
		{
			*pboolSupportPaste = pThis->m_bSupportPaste;
			return S_OK;
		}
		else
			return E_INVALIDARG;
	}
	_catch_nested;
}

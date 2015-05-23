// clipboard.h: interface for the CClipboardImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIPBOARD_H__2A374D65_711F_49F5_80AD_E931C638AE80__INCLUDED_)
#define AFX_CLIPBOARD_H__2A374D65_711F_49F5_80AD_E931C638AE80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "misc5.h"


class std_dll CClipboardImpl : public CImplBase

{
public:
	CClipboardImpl();
	virtual ~CClipboardImpl();
	
	virtual bool Copy(UINT nFormat, HANDLE* pHandle){return false;};
	virtual bool Paste(UINT nFormat, HANDLE Handle){return false;};

protected:
	BEGIN_INTERFACE_PART_EXPORT(Clipboard, IClipboard)
		com_call GetFirstFormatPosition(long *pnPos);
		com_call GetNextFormat(UINT* pnFormat, long *pnPosNext);
		com_call Copy(UINT nFormat, HANDLE* pHandle);
		com_call Paste(UINT nFormat, HANDLE Handle);
		com_call IsSupportPaste(long * pboolSupportPaste);
	END_INTERFACE_PART(Clipboard)

	bool CheckFormat(UINT nFormat);

protected:
	CDWordArray	m_arrAvailFormats;
	bool		m_bSupportPaste;
};

#endif // !defined(AFX_CLIPBOARD_H__2A374D65_711F_49F5_80AD_E931C638AE80__INCLUDED_)

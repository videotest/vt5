#include "stdafx.h"
#include "win_dlg.h"
#include "com_main.h"
#include "core5.h"
#include "nameconsts.h"

class _DialogSplitHelper
{
public:
	// Constants used in DLGINIT resources for OLE control containers
	// NOTE: These are NOT real Windows messages they are simply tags
	// used in the control resource and are never used as 'messages'
	enum
	{
		WM_OCC_LOADFROMSTREAM = 0x0376,
		WM_OCC_LOADFROMSTORAGE = 0x0377,
		WM_OCC_INITNEW = 0x0378,
		WM_OCC_LOADFROMSTREAM_EX = 0x037A,
		WM_OCC_LOADFROMSTORAGE_EX = 0x037B,
		DISPID_DATASOURCE = 0x80010001,
		DISPID_DATAFIELD = 0x80010002,
	};

//local struct used for implementation
#pragma pack(push, 1)
	struct DLGINITSTRUCT
	{
		WORD nIDC;
		WORD message;
		DWORD dwSize;
	};
	struct DLGTEMPLATEEX
	{
		WORD dlgVer;
		WORD signature;
		DWORD helpID;
		DWORD exStyle;
		DWORD style;
		WORD cDlgItems;
		short x;
		short y;
		short cx;
		short cy;

		// Everything else in this structure is variable length,
		// and therefore must be determined dynamically

		// sz_Or_Ord menu;			// name or ordinal of a menu resource
		// sz_Or_Ord windowClass;	// name or ordinal of a window class
		// WCHAR title[titleLen];	// title string of the dialog box
		// short pointsize;			// only if DS_SETFONT is set
		// short weight;			// only if DS_SETFONT is set
		// short bItalic;			// only if DS_SETFONT is set
		// WCHAR font[fontLen];		// typeface name, if DS_SETFONT is set
	};
	struct DLGITEMTEMPLATEEX
	{
		DWORD helpID;
		DWORD exStyle;
		DWORD style;
		short x;
		short y;
		short cx;
		short cy;
		DWORD id;

		// Everything else in this structure is variable length,
		// and therefore must be determined dynamically

		// sz_Or_Ord windowClass;	// name or ordinal of a window class
		// sz_Or_Ord title;			// title string or ordinal of a resource
		// WORD extraCount;			// bytes following creation data
	};
#pragma pack(pop)

	static BOOL IsDialogEx(const DLGTEMPLATE* pTemplate)
	{
		return ((DLGTEMPLATEEX*)pTemplate)->signature == 0xFFFF;
	}

	inline static WORD& DlgTemplateItemCount(DLGTEMPLATE* pTemplate)
	{
		if (IsDialogEx(pTemplate))
			return reinterpret_cast<DLGTEMPLATEEX*>(pTemplate)->cDlgItems;
		else
			return pTemplate->cdit;
	}

	inline static const WORD& DlgTemplateItemCount(const DLGTEMPLATE* pTemplate)
	{
		if (IsDialogEx(pTemplate))
			return reinterpret_cast<const DLGTEMPLATEEX*>(pTemplate)->cDlgItems;
		else
			return pTemplate->cdit;
	}

	static DLGITEMTEMPLATE* FindFirstDlgItem(const DLGTEMPLATE* pTemplate)
	{
		BOOL bDialogEx = IsDialogEx(pTemplate);

		WORD* pw;
		DWORD dwStyle;
		if (bDialogEx)
		{
			pw = (WORD*)((DLGTEMPLATEEX*)pTemplate + 1);
			dwStyle = ((DLGTEMPLATEEX*)pTemplate)->style;
		}
		else
		{
			pw = (WORD*)(pTemplate + 1);
			dwStyle = pTemplate->style;
		}

		// Check for presence of menu and skip it if there is one
		// 0x0000 means there is no menu
		// 0xFFFF means there is a menu ID following
		// Everything else means that this is a NULL terminated Unicode string
		// which identifies the menu resource
		if (*pw == 0xFFFF)
			pw += 2;				// Has menu ID, so skip 2 words
		else
			while (*pw++);			// Either No menu, or string, skip past terminating NULL

		// Check for presence of class name string
		// 0x0000 means "Use system dialog class name"
		// 0xFFFF means there is a window class (atom) specified
		// Everything else means that this is a NULL terminated Unicode string
		// which identifies the menu resource
		if (*pw == 0xFFFF)
			pw += 2;				// Has class atom, so skip 2 words
		else
			while (*pw++);			// Either No class, or string, skip past terminating NULL

		// Skip caption string
		while (*pw++);

		// If we have DS_SETFONT, there is extra font information which we must now skip
		if (dwStyle & DS_SETFONT)
		{
			// If it is a regular DLGTEMPLATE there is only a short for the point size
			// and a string specifying the font (typefacename).  If this is a DLGTEMPLATEEX
			// then there is also the font weight, and bItalic which must be skipped
			if (bDialogEx)
				pw += 3;			// Skip font size, weight, (italic, charset)
			else
				pw += 1;			// Skip font size
			while (*pw++);			// Skip typeface name
		}

		// Dword-align and return
		return (DLGITEMTEMPLATE*)(((DWORD_PTR)pw + 3) & ~3);
	}

	// Given the current dialog item and whether this is an extended dialog
	// return a pointer to the next DLGITEMTEMPLATE*
	static DLGITEMTEMPLATE* FindNextDlgItem(DLGITEMTEMPLATE* pItem, BOOL bDialogEx)
	{
		WORD* pw;

		// First skip fixed size header information, size of which depends
		// if this is a DLGITEMTEMPLATE or DLGITEMTEMPLATEEX
		if (bDialogEx)
			pw = (WORD*)((DLGITEMTEMPLATEEX*)pItem + 1);
		else
			pw = (WORD*)(pItem + 1);

		if (*pw == 0xFFFF)			// Skip class name ordinal or string
			pw += 2; // (WORDs)
		else
			while (*pw++);

		if (*pw == 0xFFFF)			// Skip title ordinal or string
			pw += 2; // (WORDs)
		else
			while (*pw++);

		WORD cbExtra = *pw++;		// Skip extra data

		// Dword-align and return
		return (DLGITEMTEMPLATE*)(((DWORD_PTR)pw + cbExtra + 3) & ~3);
	}

	// Find the initialization data (Stream) for the control specified by the ID
	// If found, return the pointer into the data and the length of the data
	static DWORD FindCreateData(DWORD dwID, BYTE* pInitData, BYTE** pData)
	{
		while (pInitData)
		{
			// Read the DLGINIT header
			WORD nIDC = *((UNALIGNED WORD*)pInitData);
			pInitData += sizeof(WORD);
			WORD nMsg = *((UNALIGNED WORD*)pInitData);
			pInitData += sizeof(WORD);
			DWORD dwLen = *((UNALIGNED DWORD*)pInitData);
			pInitData += sizeof(DWORD);

			// If the header is for the control specified get the other info
			if (nIDC == dwID)
			{
				DWORD cchLicKey = *((UNALIGNED DWORD*)pInitData);
				pInitData += sizeof(DWORD);
				dwLen -= sizeof(DWORD);
				if (cchLicKey > 0)
				{
					//_bstr_t	bstrLicKey(LPCOLESTR)pInitData, cchLicKey);

					pInitData += cchLicKey * sizeof(OLECHAR);
					dwLen -= cchLicKey * sizeof(OLECHAR);
				}

				// Extended (DATABINDING) stream format is not supported,
				// we reject databinding info but preserve other information
				if (nMsg == WM_OCC_LOADFROMSTREAM_EX ||
					nMsg == WM_OCC_LOADFROMSTORAGE_EX)
				{
					// Read the size of the section
					ULONG cbOffset = *(UNALIGNED ULONG*)pInitData;

					// and simply skip past it
					*pData = pInitData + cbOffset;
					dwLen = dwLen - cbOffset;
					return dwLen;
				}
				if (nMsg == WM_OCC_LOADFROMSTREAM)
					*pData = pInitData;
				return dwLen;
			}

			// It's not the right control, skip past data
			pInitData += dwLen;
		}
		return 0;
	}

	// Convert MSDEV (MFC) style DLGTEMPLATE with controls to regular DLGTEMPLATE
	// Changing all ActiveX Controls to use ATL AxWin hosting code
	static DLGTEMPLATE* SplitDialogTemplate(DLGTEMPLATE* pTemplate, BYTE* pInitData)
	{
		char	szAxContainerWinClassName1[] = "Button";
		//_bstr_t	bstrWindowClass = szAxContainerWinClassName;
		 //= bstrWindowClass;
		wchar_t	lpstrAxWndClassNameW[128];
		//MultiByteToWideChar( 1, 0, szAxContainerWinClassName, strlen( szAxContainerWinClassName )+1, lpstrAxWndClassNameW, 128 );
		MultiByteToWideChar( 1, 0, szAxContainerWinClassName, (int)strlen( szAxContainerWinClassName )+1, lpstrAxWndClassNameW, 128 );

		 _bstr_t	bstrTest=  lpstrAxWndClassNameW;

		// Calculate the size of the DLGTEMPLATE for allocating the new one
		DLGITEMTEMPLATE* pFirstItem = FindFirstDlgItem(pTemplate);
		ULONGLONG cbHeader = (BYTE*)pFirstItem - (BYTE*)pTemplate;
		ULONGLONG cbNewTemplate = cbHeader;

		BOOL bDialogEx = IsDialogEx(pTemplate);

		int iItem;
		int nItems = (int)DlgTemplateItemCount(pTemplate);
#ifndef OLE2ANSI
		LPWSTR pszClassName;
#else
		LPSTR pszClassName;
#endif
		BOOL bHasOleControls = FALSE;

		// Make first pass through the dialog template.  On this pass, we're
		// interested in determining:
		//    1. Does this template contain any ActiveX Controls?
		//    2. If so, how large a buffer is needed for a template containing
		//       only the non-OLE controls?

		DLGITEMTEMPLATE* pItem = pFirstItem;
		DLGITEMTEMPLATE* pNextItem = pItem;
		for (iItem = 0; iItem < nItems; iItem++)
		{
			pNextItem = FindNextDlgItem(pItem, bDialogEx);

			pszClassName = bDialogEx ?
#ifndef OLE2ANSI
				(LPWSTR)(((DLGITEMTEMPLATEEX*)pItem) + 1) :
				(LPWSTR)(pItem + 1);
#else
				(LPSTR)(((DLGITEMTEMPLATEEX*)pItem) + 1) :
				(LPSTR)(pItem + 1);
#endif

			// Check if the class name begins with a '{'
			// If it does, that means it is an ActiveX Control in MSDEV (MFC) format
#ifndef OLE2ANSI
			if (pszClassName[0] == L'{')
#else
			if (pszClassName[0] == '{')
#endif
			{
				bHasOleControls = TRUE;
				pItem = pNextItem;
				continue;
				// Item is an ActiveX control.

				cbNewTemplate += (bDialogEx ? sizeof(DLGITEMTEMPLATEEX) : sizeof(DLGITEMTEMPLATE));

				// Length of className including NULL terminator
				cbNewTemplate += (lstrlenW(lpstrAxWndClassNameW) + 1) * sizeof(WCHAR);
				
				// Add length for the title CLSID in the form "{00000010-0000-0010-8000-00AA006D2EA4}"
				// plus room for terminating NULL and an extra WORD for cbExtra
				cbNewTemplate += 78;

				// Get the Control ID
				DWORD wID = bDialogEx ? ((DLGITEMTEMPLATEEX*)pItem)->id : pItem->id;
				BYTE* pData;

				cbNewTemplate+=sizeof( WORD );	
				//for 9x and 2000 - add extra bytes /
				cbNewTemplate+=10;
				

				cbNewTemplate += FindCreateData(wID, pInitData, &pData);
				
				// Align to next DWORD
				cbNewTemplate = ((cbNewTemplate + 3) & ~3);
			}
			else
			{
				// Item is not an ActiveX Control: make room for it in new template.
				cbNewTemplate += (BYTE*)pNextItem - (BYTE*)pItem;
			}

			pItem = pNextItem;
		}

		// No OLE controls were found, so there's no reason to go any further.
		if (!bHasOleControls)
			return pTemplate;

		// Copy entire header into new template.
		BYTE* pNew = (BYTE*)GlobalAlloc(GMEM_FIXED, cbNewTemplate);
		DLGTEMPLATE* pNewTemplate = (DLGTEMPLATE*)pNew;
		memcpy(pNew, pTemplate, cbHeader);
		pNew += cbHeader;

		// Initialize item count in new header to zero.
		DlgTemplateItemCount(pNewTemplate) = 0;

		pItem = pFirstItem;
		pNextItem = pItem;

		// Second pass through the dialog template.  On this pass, we want to:
		//    1. Copy all the non-OLE controls into the new template.
		//    2. Build an array of item templates for the OLE controls.

		for (iItem = 0; iItem < nItems; iItem++)
		{
			pNextItem = FindNextDlgItem(pItem, bDialogEx);

			pszClassName = bDialogEx ?
#ifndef OLE2ANSI
				(LPWSTR)(((DLGITEMTEMPLATEEX*)pItem) + 1) :
				(LPWSTR)(pItem + 1);

			if (pszClassName[0] == L'{')
#else
				(LPSTR)(((DLGITEMTEMPLATEEX*)pItem) + 1) :
				(LPSTR)(pItem + 1);

			if (pszClassName[0] == '{')
#endif
			{
				pItem = pNextItem;
				continue;
				// Item is OLE control: add it to template as custom control

				// Copy the dialog item template
				DWORD nSizeElement = bDialogEx ? sizeof(DLGITEMTEMPLATEEX) : sizeof(DLGITEMTEMPLATE);
				memcpy(pNew, pItem, nSizeElement);
				pNew += nSizeElement;

				// Copy ClassName
				DWORD nClassName = (lstrlenW(lpstrAxWndClassNameW) + 1) * sizeof(WCHAR);
				memcpy(pNew, lpstrAxWndClassNameW, nClassName);
				pNew += nClassName;

				// Title (CLSID)
				memcpy(pNew, pszClassName, 78);
				pNew += 78; // sizeof(L"{00000010-0000-0010-8000-00AA006D2EA4}") - A CLSID

				DWORD wID = bDialogEx ? ((DLGITEMTEMPLATEEX*)pItem)->id : pItem->id;
				BYTE* pData;
				nSizeElement = FindCreateData(wID, pInitData, &pData);

				// cbExtra
				//??
				//
				if( GetVersion() & 0x80000000 )
				{
					//9x clones - CreateData doesn't include own size
					*((WORD*)pNew) = (WORD) nSizeElement+2;
					pNew += sizeof(WORD);
					*((WORD*)pNew) = (WORD) nSizeElement;
					pNew += sizeof(WORD);
				}
				else
				{
					//NT - CreateData includes own size
					*((WORD*)pNew) = (WORD) nSizeElement;
					pNew += sizeof(WORD);
				}
				

				memcpy(pNew, pData, nSizeElement);
				pNew += nSizeElement;
				//Align to DWORD
				pNew = (byte*)(((DWORD)pNew + 3) & ~3);

				// Incrememt item count in new header.
				++DlgTemplateItemCount(pNewTemplate);
			}
			else
			{
				// Item is not an OLE control: copy it to the new template.
				size_t cbItem = (BYTE*)pNextItem - (BYTE*)pItem;
				dbg_assert(cbItem >= (size_t)(bDialogEx ?
					sizeof(DLGITEMTEMPLATEEX) :
					sizeof(DLGITEMTEMPLATE)));
				memcpy(pNew, pItem, cbItem);
				pNew += cbItem;

				// Incrememt item count in new header.
				++DlgTemplateItemCount(pNewTemplate);
			}

			pItem = pNextItem;
		}
		//ppOleDlgItems[nItems] = (DLGITEMTEMPLATE*)(-1);

		return pNewTemplate;
	}

	static bool CreateAXControls( DLGTEMPLATE* pTemplate, BYTE* pInitData, HWND hwndDialog )
	{
		DLGITEMTEMPLATE* pFirstItem = FindFirstDlgItem(pTemplate);
		size_t cbHeader = (BYTE*)pFirstItem - (BYTE*)pTemplate;

		BOOL bDialogEx = IsDialogEx(pTemplate);

		int iItem;
		int nItems = (int)DlgTemplateItemCount(pTemplate);

		DLGITEMTEMPLATE* pItem = pFirstItem;
		DLGITEMTEMPLATE* pNextItem = pItem;
		for (iItem = 0; iItem < nItems; iItem++)
		{
			pNextItem = FindNextDlgItem(pItem, bDialogEx);

			LPWSTR	pszClassName = bDialogEx ?
#ifndef OLE2ANSI
				(LPWSTR)(((DLGITEMTEMPLATEEX*)pItem) + 1) :
				(LPWSTR)(pItem + 1);
#else
				(LPSTR)(((DLGITEMTEMPLATEEX*)pItem) + 1) :
				(LPSTR)(pItem + 1);
#endif

			// Check if the class name begins with a '{'
			// If it does, that means it is an ActiveX Control in MSDEV (MFC) format
#ifndef OLE2ANSI
			if (pszClassName[0] == L'{')
#else
			if (pszClassName[0] == '{')
#endif
			{

				DWORD wID = bDialogEx ? ((DLGITEMTEMPLATEEX*)pItem)->id : pItem->id;
				_bstr_t	bstrCLSID = pszClassName;

				// Add length for the title CLSID in the form "{00000010-0000-0010-8000-00AA006D2EA4}"
				// plus room for terminating NULL and an extra WORD for cbExtra
				
				BYTE* pData;
				int nCreateSize = FindCreateData(wID, pInitData, &pData);
				byte	*pCreateData = new byte[nCreateSize+4];
				*(WORD*)pCreateData = nCreateSize;
				memcpy( pCreateData+2, pData, nCreateSize );

								

				if( bDialogEx )
				{
					DLGITEMTEMPLATEEX	*pItemEx = (DLGITEMTEMPLATEEX*)pItem;
					_rect	rect( pItemEx->x, pItemEx->y, pItemEx->x+pItemEx->cx, pItemEx->y+pItemEx->cy );
					::MapDialogRect( hwndDialog, &rect );
					
					::CreateWindowEx( pItemEx->exStyle, szAxContainerWinClassName, bstrCLSID, pItemEx->style,
							rect.left, rect.top, rect.width(), rect.height(),
							hwndDialog, (HMENU)pItemEx->id, App::handle(), (void*)pCreateData );
				}
				else
				{
					_rect	rect( pItem->x, pItem->y, pItem->x+pItem->cx, pItem->y+pItem->cy );
					::MapDialogRect( hwndDialog, &rect );

					::CreateWindowEx( 0, szAxContainerWinClassName, bstrCLSID, pItem->style,
							rect.left, rect.top, rect.width(), rect.height(),
							hwndDialog, (HMENU)pItem->id, App::handle(), (void*)pCreateData );
				}

				delete pCreateData;

			}
			pItem = pNextItem;
		}
		return true;
	}
};






CDialogImpl::CDialogImpl()
{
	m_bInsideDialogMessage = false;
	m_pTemplate = 0;
	m_pInitData = 0;
	m_hTemplate = 0;
	m_hInitData = 0;
	m_pDialogTemplateSrc = 0;
}

CDialogImpl::~CDialogImpl()
{
	FreeTemplate();
}


void CDialogImpl::Subclass( HWND hwnd )
{
	m_hwnd = hwnd;

	char	szClassName[64];
	::GetClassName( m_hwnd, szClassName, 64 );
	
	m_thunk.Init( (DWORD_PTR)WorkWindowProc, (IMessageFilter*)this );
	m_pDefHandler = (WNDPROC)::GetWindowLongPtr( m_hwnd , GWLP_WNDPROC );

	CWndClass	*pclass = App::instance()->FindWindowClass( szClassName );
	
	::SetWindowLongPtr( m_hwnd, DWLP_DLGPROC, (LONG_PTR)m_thunk.GetCodeAddress() );

	m_pDefHandler = 0;
}

HRESULT CDialogImpl::CreateWnd( HANDLE hwndParent, RECT rect, DWORD dwStyles, UINT nID )
{
	if( !LoadTemplate() )
		return S_FALSE;

	//m_pTemplate->style &= ~WS_CHILD;
	//m_pTemplate->style &= ~WS_OVERLAPPED;
	//m_pTemplate->style |= WS_POPUP;
	m_pTemplate->style = dwStyles;

	if( CreateDialogIndirectParam( App::handle(), m_pTemplate, (HWND)hwndParent, InitDlgProc, (LPARAM)this ) == 0 )
		App::instance()->handle_error( GetLastError() );

	return S_OK;
}

HRESULT CDialogImpl::DestroyWindow()
{
	if( !m_hwnd )return 0;
	if( !::DestroyWindow( m_hwnd ) )return S_FALSE;

	return S_OK;
}

void CDialogImpl::DoPaint()
{
	PAINTSTRUCT	paint;
	::BeginPaint( m_hwnd, &paint );
	::EndPaint( m_hwnd, &paint );
}


LRESULT	CDialogImpl::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bInsideDialogMessage )
	{
		MSG	msg;
		msg.hwnd = m_hwnd;
		msg.lParam = lParam;
		msg.wParam = wParam;
		msg.message = nMsg;

		m_bInsideDialogMessage = true;
		if( IsDialogMessage( m_hwnd, &msg ) )
			return 0;
		m_bInsideDialogMessage = false;
	}

	if( nMsg == WM_INITDIALOG )	
	{
		_DialogSplitHelper::CreateAXControls( m_pDialogTemplateSrc, m_pInitData, m_hwnd );
		return DoInitDialog();
	}

	LONG_PTR lRes = CWinImpl::DoMessage( nMsg, wParam, lParam );

	// set result if message was handled
	if( lRes )
	{
		switch (nMsg)
		{
/*		case WM_COMPAREITEM:
		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_INITDIALOG:
		case WM_QUERYDRAGICON:
		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:*/
		case WM_GETINTERFACE:
			::SetWindowLongPtr( m_hwnd, DWLP_MSGRESULT, lRes );
			break;
		}
		
		return lRes;
	}

	return lRes;//DefDlgProc( m_hwnd, nMsg, wParam, lParam );
}

#define RT_DLGINIT  MAKEINTRESOURCE(240)


bool CDialogImpl::LoadTemplate()
{
	if( m_hTemplate )
		return true;
	HRSRC hDlg = ::FindResource( App::handle(), MAKEINTRESOURCE( idd() ), (LPSTR)RT_DIALOG);
	HRSRC hInit = ::FindResource(App::handle(), MAKEINTRESOURCE( idd() ), (LPSTR)RT_DLGINIT );

	m_hTemplate = ::LoadResource( App::handle(), hDlg );
	if (hInit) m_hInitData = ::LoadResource( App::handle(), hInit );

	m_pDialogTemplateSrc = (DLGTEMPLATE*)::LockResource( m_hTemplate );
	if (m_hInitData) m_pInitData = (byte*) LockResource( m_hInitData );

	m_pTemplate = _DialogSplitHelper::SplitDialogTemplate( m_pDialogTemplateSrc, m_pInitData );
	//m_pTemplate = pDialogTemplateSrc;

	return true;
}

void CDialogImpl::FreeTemplate()
{
	if( m_hTemplate )
	{
		UnlockResource( m_hTemplate );
		::FreeResource( m_hTemplate );
		m_hTemplate = 0;
	}
	if( m_hInitData )
	{
		UnlockResource( m_hInitData );
		::FreeResource( m_hInitData );
		m_hInitData = 0;
	}

	if( m_pTemplate !=  m_pDialogTemplateSrc )
		::GlobalFree( ::GlobalHandle( m_pTemplate ) );
	m_pTemplate = 0;
	m_pDialogTemplateSrc = 0;
}

INT_PTR CDialogImpl::DoModal()
{
	if(!LoadTemplate())
		return -1;

	HWND hwndOwner = 0;
	IApplicationPtr sptrApp(GetAppUnknown());
	if(sptrApp != 0)
		sptrApp->GetMainWindowHandle(&hwndOwner);
	return DialogBoxIndirectParam( App::handle(), m_pTemplate, hwndOwner, InitDlgProc, (LPARAM)this );
}

void CDialogImpl::DoCommand( UINT nCmd )
{
	if(nCmd == IDOK)
		::EndDialog(hwnd(), IDOK);
	else if(nCmd == IDCANCEL)
		::EndDialog(hwnd(), IDCANCEL);
}
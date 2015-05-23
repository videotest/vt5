// PropertyBrowserPage.cpp: implementation of the CPropertyBrowserPage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include "PropertyBrowserPage.h"
#include "PBDib.h"
#include <commdlg.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
Property Browser Page v1.0
Template class that provides automatic property page for ATL-based ActiveX Controls
Copyrights, (c) 1999 Vladimir Scherbakov (rise@alt.ru)

*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CPBProperty::SetNewText(BSTR  bstrNewText)
	{
	_ASSERT(m_vt != VT_NULL);
	CComVariant v_text,v_value;
	if(bstrNewText && m_bstrText.m_str)
		if(wcscmp(bstrNewText,m_bstrText.m_str) == 0)
			return TRUE;
	if(bstrNewText)
		for(ULONG i = 0;i < m_castr.cElems;i++)
			if(wcscmp(m_castr.pElems[i],bstrNewText) == 0)
				{
				m_value = m_pvPredefinedValues[i];
				m_bstrText = bstrNewText;
				m_bDirty = TRUE;
				return TRUE;
				}

	v_text = bstrNewText;

	///!!!May by it is simple to use _VarChangeType?
	if(VariantChangeType(&v_value,&v_text,0,m_vt) != S_OK)
		return FALSE;
	if(!m_bstrText.m_str && ::SysStringLen(bstrNewText) == 0)
		return TRUE;

	m_bstrText = bstrNewText;
	m_bDirty = TRUE;
	m_value = v_value;
	for(ULONG i = 0;i < m_castr.cElems;i++)
		{
		CComVariant v(m_pvPredefinedValues[i]);
		if(v == m_value)
			{
			m_bstrText = m_castr.pElems[i];
			return TRUE;
			}
		}
	return TRUE;
	}

BOOL CPBProperty::SetNewValue(VARIANT * pVar)
	{
	CComVariant v_text;ULONG i=0;
	HRESULT hr;
	m_value = *pVar;
	m_vt = pVar->vt;
	switch(m_DisplayType)
		{
		case PBT_STR:
		for(i = 0;i < m_castr.cElems;i++)
			{
			CComVariant v(m_pvPredefinedValues[i]);
			if(v == m_value)
				{
				m_bstrText = m_castr.pElems[i];
				return TRUE;
				}
			}
		///!!!May by it is simple to use _VarChangeType?
		if(VariantChangeType(&v_text,pVar,0,VT_BSTR) != S_OK)
			{
			ATLTRACE(_T("[CPBProperty::SetNewValue] - failed to convert to text\n"));
			}
		else
			m_bstrText = V_BSTR(&v_text);

		break;
		case PBT_FONT:
			{
			CComDispatchDriver dd;
			if(pVar->vt == VT_DISPATCH)
				dd = pVar->pdispVal;
			else if(pVar->vt == (VT_DISPATCH | VT_BYREF))
				dd = *(pVar->ppdispVal);
			else
				{
				// no ideas where is this font
				_ASSERT(FALSE);
				return FALSE;
				}
			if(!dd)
				{
				m_bstrText = L"(None)";
				return TRUE;
				}
			CComVariant v;
			hr = dd.GetProperty(DISPID_FONT_NAME,&v);
			if(SUCCEEDED(hr))
				{
				m_bstrText = V_BSTR(&v);
				}
			else
				{
				_ASSERT(FALSE);
				return FALSE;
				}

			}
			break;
		case PBT_COLOR:
			{
			_ASSERT(V_VT(pVar) == VT_UI4 || V_VT(pVar) == VT_I4 );
			TCHAR szText[32];
			_stprintf(szText,_T("&H%08X&"),pVar->lVal);
			m_bstrText = szText;
			}
			break;
		case PBT_ENUM:
			{
			for(i = 0;i < m_castr.cElems;i++)
				{
				CComVariant v(m_pvPredefinedValues[i]);
				if(v == m_value)
					{
					m_bstrText = m_castr.pElems[i];
					return TRUE;
					}
				}
			//TODO:
			}
			break;
		case PBT_PICTURE:
			{
			CComDispatchDriver dd;
			if(pVar->vt == VT_DISPATCH)
				dd = pVar->pdispVal;
			else if(pVar->vt == (VT_DISPATCH | VT_BYREF))
				dd = *(pVar->ppdispVal);
			else
				{
				_ASSERT(FALSE);
				return FALSE;
				}
			if(!dd)
				{
				m_bstrText = L"(None)";
				return TRUE;
				}
			CComVariant v;
			hr = dd.GetProperty(DISPID_PICT_TYPE,&v);
			if(SUCCEEDED(hr))
				{
				switch(v.iVal)
					{
					case PICTYPE_BITMAP:
						m_bstrText = L"(Bitmap)";
						break;
					case PICTYPE_ICON:
						m_bstrText = L"(Icon)";
						break;
					case PICTYPE_METAFILE:
					case PICTYPE_ENHMETAFILE:
						m_bstrText = L"(Metafile)";
						break;
					default:
						m_bstrText = L"(None)";
						break;
					}
				}
			else
				{
				_ASSERT(FALSE);
				return FALSE;
				}

			}
			break;
		case PBT_BOOL:
		if(m_value.boolVal)
			m_bstrText = m_castr.pElems[1];
		else
			m_bstrText = m_castr.pElems[0];
			break;
		default:
			// unknow type found

			_ASSERT(FALSE);
			break;
		}
	return TRUE;
	}


////////////////////// CPropBrowseWnd /////////////////////////////
int CPropBrowseWnd::m_nDeltaX = 2;


void CPropBrowseWnd::AddProperty(CPBProperty * pProp)
	{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT(pProp);
	_ASSERT(pProp->m_bstrName.m_str);
	int nWidth = GetItemWidth(pProp);
	if(nWidth > m_nColWidth)
		m_nColWidth = nWidth;
	int nIdx = (int)SendMessage(LB_ADDSTRING,0,(LPARAM)pProp);
	SendMessage(LB_SETITEMDATA,nIdx,(LPARAM)pProp);
	}

LRESULT CPropBrowseWnd::DrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	USES_CONVERSION;
	DRAWITEMSTRUCT* di = (DRAWITEMSTRUCT*)lParam;
	HDC hDC = di->hDC;
	RECT rcItem;
	::CopyRect(&rcItem,&di->rcItem);
	rcItem.right = rcItem.left + m_nColWidth;
	switch(di->itemAction)
		{
		case  ODA_SELECT:
		case  ODA_DRAWENTIRE:
			{
			SIZE sizeText;
			CPBProperty * pProp = (CPBProperty*)di->itemData;
			_ASSERT(pProp);
			LPTSTR szText = OLE2T(pProp->m_bstrName.m_str);
			::SetBkColor(hDC,di->itemState & ODS_SELECTED ? m_clrHighlite:m_clrBack);
			::SetTextColor(hDC,di->itemState & ODS_SELECTED ? m_clrHighliteText:m_clrText);
			::GetTextExtentPoint32(hDC,szText,pProp->m_bstrName.Length(),&sizeText);
			::ExtTextOut(hDC,
						rcItem.left + m_nDeltaX,
						rcItem.top + (rcItem.bottom - rcItem.top -sizeText.cy)/2 +1,
						ETO_OPAQUE | ETO_CLIPPED, &rcItem,
						szText,
						pProp->m_bstrName.Length(),
						NULL);
			// Draw property text
			::CopyRect(&rcItem,&di->rcItem);
			rcItem.left += m_nColWidth;
			if(NeedArrow(pProp))
				rcItem.right -= rcItem.bottom - rcItem.top;
			if(pProp->m_bstrText.m_str)
				szText = OLE2T(pProp->m_bstrText.m_str);
			else
				szText = _T("");

			::SetBkColor(hDC,m_clrBack);
			::SetTextColor(hDC,m_clrText);
			if(pProp->m_DisplayType == CPBProperty::PBPropTypes::PBT_COLOR)
				{
				// special case for color
				RECT rcClr;
				OLE_COLOR oc = pProp->m_value.lVal;
				COLORREF clr;
				OleTranslateColor(oc,NULL,&clr);
				::CopyRect(&rcClr,&rcItem);
				rcClr.left += m_nDeltaX;
				rcItem.left += rcClr.bottom - rcClr.top + m_nDeltaX*2;
				rcClr.bottom --;
				rcClr.right = rcClr.left + rcClr.bottom - rcClr.top;
				::InflateRect(&rcClr,-1,-1);
				HPEN hOldPen = (HPEN)::SelectObject(hDC,::GetStockObject(BLACK_PEN));
				HBRUSH hBr = ::CreateSolidBrush(clr);
				HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC,hBr);
				::Rectangle(hDC,rcClr.left,rcClr.top,rcClr.right,rcClr.bottom);
				::SelectObject(hDC,hOldBrush);
				::DeleteObject(hBr);
				::SelectObject(hDC,hOldPen);
				}

			::GetTextExtentPoint32(hDC,szText,pProp->m_bstrText.Length(),&sizeText);
			if(pProp->m_bDirty)
				::SetTextColor(hDC,m_clrDirtyText);
			::ExtTextOut(hDC,
						rcItem.left + m_nDeltaX,
						rcItem.top + (rcItem.bottom - rcItem.top -sizeText.cy)/2 +1,
						ETO_OPAQUE | ETO_CLIPPED, &rcItem,
						szText,
						pProp->m_bstrText.Length(),
						NULL);
			HPEN oldPen = (HPEN)::SelectObject(hDC,m_hGridPen);
			::MoveToEx(hDC,di->rcItem.left + m_nColWidth,di->rcItem.top,NULL);
			::LineTo(hDC,di->rcItem.left + m_nColWidth,di->rcItem.bottom);
			CopyRect(&rcItem,&di->rcItem);
			::MoveToEx(hDC,rcItem.left,rcItem.bottom-1,NULL);
			::LineTo(hDC,rcItem.right,rcItem.bottom-1);
			::SelectObject(hDC,oldPen);
			DrawItemArrow(hDC,&di->rcItem,pProp,di->itemState & ODS_SELECTED);
			}
			break;
		}
	return 0;
	}
LRESULT CPropBrowseWnd::OnKillFocusEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	bHandled = FALSE;
	if(GetFocus() == m_wndLBox.m_hWnd)
		return 0;
	m_wndEdit.ShowWindow(SW_HIDE);
	m_wndLBox.ShowWindow(SW_HIDE);
	if(m_pEditProp)
		{
		//still not handled property change, do it now
		CComBSTR bstrText;
		m_wndEdit.GetWindowText(bstrText.m_str);
		if(m_pEditProp->SetNewText(bstrText) == FALSE)
			// it is unsafe to display Message Box inside this message handler
			PostMessage(WM_PB_WARNING);
		else
			if(m_pEditProp->m_bDirty)
			::PostMessage(GetParent(),WM_PB_PROPCHANGE,0,0);
		m_pEditProp = NULL;
		}
	return 0;
	}
LRESULT CPropBrowseWnd::OnDisplayWarning(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	MessageBox(_T("Invalid property value!"),_T("Error"),MB_OK | MB_ICONSTOP);
	return 0;
	}

// custom message handler used to display in-place edit window 
LRESULT CPropBrowseWnd::OnStartEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	USES_CONVERSION;
	_ASSERT(m_pEditProp);

		if(m_pEditProp->m_bstrText.m_str)
			m_wndEdit.SetWindowText(OLE2CT(m_pEditProp->m_bstrText.m_str));
		else
			m_wndEdit.SetWindowText(_T(""));
		if(m_pEditProp->m_castr.cElems > 0)
			m_rcEdit.right -= m_rcEdit.bottom - m_rcEdit.top;
		m_bEditReadOnly = m_pEditProp->m_DisplayType == CPBProperty::PBPropTypes::PBT_STR ?
						FALSE : TRUE;
		m_wndEdit.SetWindowPos(HWND_TOP,&m_rcEdit,SWP_SHOWWINDOW);
		m_wndEdit.SetFocus();
	return 0;
	}
LRESULT CPropBrowseWnd::OnKeyDownEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	bHandled = FALSE;
	int nVirtKey = (int)wParam;
	if(!m_pEditProp)
		return 0;
	if(nVirtKey == VK_DOWN || nVirtKey == VK_UP)
		{
		if(m_wndLBox.IsWindowVisible())
			{
			int nIdx = (int)m_wndLBox.m_wndList.SendMessage(LB_GETCURSEL);
			int nCnt = (int)m_pEditProp->m_castr.cElems;
			if(nIdx != LB_ERR)
				{
				USES_CONVERSION;
				if(nVirtKey == VK_DOWN)
					if(nIdx < nCnt - 1)
						nIdx++;
					else
						nIdx = 0;
				else
					if(nIdx) nIdx--;
					else
						nIdx = nCnt - 1;
				m_wndLBox.m_wndList.SendMessage(LB_SETCURSEL,nIdx);
				m_wndEdit.SetWindowText(OLE2T(m_pEditProp->m_castr.pElems[nIdx]));
				m_wndEdit.PostMessage(EM_SETSEL,0,(LPARAM)(INT)-1);
				}
			}
		}
	return 0;
	}
LRESULT CPropBrowseWnd::OnSysKeyDownEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	bHandled = FALSE;
	int nVirtKey = (int)wParam;
	if(!m_pEditProp)
		return 0;
	if(nVirtKey == VK_DOWN && m_pEditProp->m_castr.cElems > 0)
		{
		RECT r;
		m_wndEdit.GetWindowRect(&r);
		m_wndLBox.Display(&r,m_pEditProp,m_wndEdit.m_hWnd);
		bHandled = TRUE;
		}
	return 0;
	}

LRESULT CPropBrowseWnd::OnLDoubleClickEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	bHandled = FALSE;
	if(m_pEditProp == NULL)
		{
		_ASSERT(FALSE);
		return 0;
		}
	TCHAR szText[128];
	m_wndEdit.GetWindowText(szText,127);
	USES_CONVERSION;
	LPOLESTR szNextText = m_pEditProp->GetNextPredefinedString(szText);
	if(szNextText)
		{
		m_wndEdit.SetWindowText(OLE2T(szNextText));
		bHandled = TRUE;
		}
	return 0;
	}
LRESULT CPropBrowseWnd::OnLButtonDoubleClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	POINT pt;
	pt.x = (int)LOWORD(lParam);pt.y = (int) HIWORD(lParam);
	bHandled = FALSE;
	DWORD dw = SendMessage(LB_ITEMFROMPOINT,0,lParam);
	if(HIWORD(dw) != 0) // no item hit?
		return 0;
	int nIdx = LOWORD(dw);
	if(SendMessage(LB_GETSEL,nIdx,0) <= 0)
		return 0;// item is not selected
	RECT rcItem;
	SendMessage(LB_GETITEMRECT,nIdx,(LPARAM)&rcItem);
	if(::PtInRect(&rcItem,pt))
		{
		CPBProperty * pProp =(CPBProperty *) SendMessage(LB_GETITEMDATA,nIdx,0);
		if(!pProp)
			{
			_ASSERT(pProp);
			return 0;
			}
		switch(pProp->m_DisplayType)
			{
			case CPBProperty::PBPropTypes::PBT_FONT:
			case CPBProperty::PBPropTypes::PBT_PICTURE:
				PostMessage(WM_PB_SHOWPOPUP,(WPARAM)pProp,0);
				break;
			default:			
			USES_CONVERSION;
			LPOLESTR szNextText = pProp->GetNextPredefinedString(OLE2T(pProp->m_bstrText));
			if(szNextText)
				{
				CComBSTR bstr(szNextText);
				pProp->SetNewText(bstr);
				if(pProp->m_bDirty)
					{
					::PostMessage(GetParent(),WM_PB_PROPCHANGE,0,0);
					InvalidateRect(&rcItem);
					}
				}
			}
		}
	return 0;
	}
LRESULT CPropBrowseWnd::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	USES_CONVERSION;
	POINT pt;
	pt.x = (int)LOWORD(lParam);pt.y = (int) HIWORD(lParam);
	// get item hit
	bHandled = FALSE;
	DWORD dw = SendMessage(LB_ITEMFROMPOINT,0,lParam);
	if(HIWORD(dw) != 0) // no item hit?
		return 0;
	int nIdx = LOWORD(dw);
	if(SendMessage(LB_GETSEL,nIdx,0) <= 0)
		return 0;// item is not selected
	CPBProperty * pProp =(CPBProperty *) SendMessage(LB_GETITEMDATA,nIdx,0);
	if(!pProp)
		{
		_ASSERT(pProp);
		return 0;
		}

	RECT rcItem,rc;
	SendMessage(LB_GETITEMRECT,nIdx,(LPARAM)&rcItem);
	::CopyRect(&rc,&rcItem);
	rc.left = rc.right - (rc.bottom - rc.top);
	// check for popup window activation
	if(NeedArrow(pProp))
		{
		if(::PtInRect(&rc,pt))
			{
			::CopyRect(&m_rcEdit,&rcItem);
			m_rcEdit.left += m_nColWidth;
			bHandled = TRUE;
			PostMessage(WM_PB_SHOWPOPUP,(WPARAM)pProp,MAKELPARAM(rc.right,rc.bottom));
			return 0;
			}
		}
	// for text properties
	if(pt.x > m_nColWidth)
		{

		rc.left = rcItem.left + m_nColWidth;
		rc.bottom --;
		m_pEditProp = pProp;
		::CopyRect(&m_rcEdit,&rc);
		switch(pProp->m_DisplayType )
			{
			case CPBProperty::PBPropTypes::PBT_BOOL:
			case CPBProperty::PBPropTypes::PBT_ENUM:
			case CPBProperty::PBPropTypes::PBT_STR:
				bHandled = TRUE;
				PostMessage(WM_PB_STARTEDIT,0,0);
			default:
				break;
			}
		// can't activate edit control and set the focus inside this message handler
		// so post a custom message to do it
		}
	return 0;
	}
LRESULT CPropBrowseWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	bHandled = FALSE;
	RECT rcEdit;
	::SetRectEmpty(&rcEdit);
	rcEdit.right = rcEdit.bottom = 10;
	if(m_wndEdit.Create(m_hWnd,rcEdit,_T(""),WS_CHILD | ES_AUTOHSCROLL ,0,_ID_EDIT) == NULL)
		{
		_ASSERT(FALSE);
		return 0;
		}
	if(m_wndLBox.Create(NULL,rcEdit,_T(""),WS_BORDER | WS_POPUP,WS_EX_TOOLWINDOW) == NULL)
		{
		_ASSERT(FALSE);
		return 0;
		}
	return 0;
	}

BOOL CPropBrowseWnd::Create(HWND hWndParent, LPRECT prcPos,UINT nID)
	{
	DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_VISIBLE | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT
		| LBS_HASSTRINGS;

	if(CWindowImpl<CPropBrowseWnd>::Create(hWndParent,*prcPos,_T(""),dwStyle,
				WS_EX_CLIENTEDGE ,nID) == NULL)
		return FALSE;
	HFONT hFont = (HFONT)::SendMessage(hWndParent,WM_GETFONT,0,0);
	SetFont(hFont);
	m_wndEdit.SetFont(hFont);
	m_wndLBox.m_wndList.SetFont(hFont);
	if(!m_wndColor.CreateWnd(this))
		{
		_ASSERT(FALSE);
		return 0;
		}
	m_wndColor.SetFont(hFont);
	return TRUE;
	}

int CPropBrowseWnd::GetItemWidth(CPBProperty * pProp)
	{
	USES_CONVERSION;
	if(!::IsWindow(m_hWnd))
		return 0;
	SIZE sizeText;
	HDC hdc = GetWindowDC();
	HFONT hOldFont = (HFONT)SelectObject(hdc,GetFont());
	GetTextExtentPoint32(hdc,OLE2T(pProp->m_bstrName),pProp->m_bstrName.Length(),
			&sizeText);
	sizeText.cx += m_nDeltaX * 2;
	SelectObject(hdc,hOldFont);
	ReleaseDC(hdc);
	return sizeText.cx;
	}

BOOL CPropBrowseWnd::NeedArrow(CPBProperty * pProp)
	{
	_ASSERT(pProp);
	if(pProp->m_castr.cElems > 0) return TRUE;
	if(pProp->m_DisplayType == CPBProperty::PBPropTypes::PBT_STR)
		return FALSE;
	return TRUE;
	}

void CPropBrowseWnd::DrawItemArrow(HDC hDC,LPRECT prcItem, CPBProperty * pProp,BOOL bSelected)
	{
	RECT rcBtn;
	::CopyRect(&rcBtn,prcItem);
	rcBtn.left = rcBtn.right - (rcBtn.bottom - rcBtn.top);
	if(bSelected)
	switch(pProp->m_DisplayType)
		{
		case CPBProperty::PBPropTypes::PBT_STR:
			if(pProp->m_castr.cElems <= 0)
				break;
		case CPBProperty::PBPropTypes::PBT_COLOR:
		case CPBProperty::PBPropTypes::PBT_ENUM:
		case CPBProperty::PBPropTypes::PBT_BOOL:
			::DrawFrameControl(hDC,&rcBtn,DFC_SCROLL,DFCS_SCROLLCOMBOBOX);
			break;
		case CPBProperty::PBPropTypes::PBT_PICTURE:
		case CPBProperty::PBPropTypes::PBT_FONT:
			{
			::DrawFrameControl(hDC,&rcBtn,DFC_BUTTON,DFCS_BUTTONPUSH);
			COLORREF clrText = ::SetTextColor(hDC,m_clrText);
			int nBackMode = ::SetBkMode(hDC,TRANSPARENT);
			::DrawText(hDC,_T("..."),3,&rcBtn,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			::SetTextColor(hDC,clrText);
			::SetBkMode(hDC,nBackMode);
			}
			break;
		default:
			break;
		}
	else
		{
		rcBtn.bottom -= 1;
		COLORREF clrBack = ::SetBkColor(hDC,m_clrBack);
		::ExtTextOut(hDC,0,0,ETO_OPAQUE | ETO_CLIPPED,&rcBtn,_T(""),0,NULL);
		::SetBkColor(hDC,clrBack);
		}

	}
LRESULT CPropBrowseWnd::OnShowPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	CPBProperty *pProp = (CPBProperty*)wParam;
	_ASSERT(pProp);
	HRESULT hr = S_FALSE;
	switch(pProp->m_DisplayType)
		{
		case CPBProperty::PBPropTypes::PBT_FONT:
			hr = SelectFont(pProp);
			break;
		case CPBProperty::PBPropTypes::PBT_COLOR:
			{
			ClientToScreen(&m_rcEdit);
			hr = m_wndColor.PickColor(pProp,&m_rcEdit);
			}
			break;
		case CPBProperty::PBPropTypes::PBT_PICTURE:
			hr = SelectPicture(pProp);
			break;
		case CPBProperty::PBPropTypes::PBT_ENUM:
		case CPBProperty::PBPropTypes::PBT_BOOL:
		case CPBProperty::PBPropTypes::PBT_STR:
			{
			RECT r;
			m_pEditProp = pProp;
			CopyRect(&r,&m_rcEdit);
			if(m_pEditProp->m_castr.cElems > 0)
				m_rcEdit.right -= m_rcEdit.bottom - m_rcEdit.top;
			BOOL bDummy=TRUE;
			OnStartEdit(0,0,0,bDummy);
			ClientToScreen(&r);
			m_wndLBox.Display(&r,m_pEditProp,m_wndEdit.m_hWnd);
			}
			break;

		}
	if(hr==S_OK)
		::PostMessage(GetParent(),WM_PB_PROPCHANGE,0,0);
	RedrawWindow();
	return 0;
	}
HRESULT CPropBrowseWnd::AddIconFile(CPBProperty * pProp, LPCTSTR lpszFile)
	{
	HICON hIcon = (HICON)::ExtractIcon(NULL,lpszFile,0);
	if(hIcon == NULL || hIcon == (HICON) 1)
		{
		MessageBox(_T("Error reading icon file!"));
		return E_FAIL;
		}
	PICTDESC pd;
	ZeroMemory(&pd,sizeof(pd));
	//	pd.cbSizeOfStruct = sizeof(PICTDESC);
	pd.picType = PICTYPE_ICON;
	pd.icon.hicon = hIcon;
	CComPtr<IPictureDisp> spPictDisp;
	HRESULT hr = OleCreatePictureIndirect(&pd,IID_IPictureDisp,
		TRUE,(void**) &spPictDisp);
	if(FAILED(hr))
		{
		MessageBox(_T("Error creating picture object!"));
		return hr;
		}
	CComVariant v(spPictDisp);
	if(pProp->SetNewValue(&v))
		pProp->m_bDirty = TRUE;
	return S_OK;
	}

HRESULT CPropBrowseWnd::AddBitmapFile(CPBProperty * pProp, LPCTSTR lpszFile)
	{
	PBDib dib;
	TCHAR szErrorText[] = _T("Error reading picture file!");
	BOOL bOpen = dib.ReadDIBFile(lpszFile);
	if(!bOpen)
		{
		MessageBox(szErrorText);
		return E_FAIL;
		}
	HPALETTE hpal = dib.ClonePalette();
	HBITMAP bmp;
	bmp = dib.CreateBitmap();
	if(!bmp)
		{
		MessageBox(szErrorText);
		return E_FAIL;
		}
	PICTDESC pd;
	ZeroMemory(&pd,sizeof(pd));
	//			pd.cbSizeOfStruct = sizeof(PICTDESC);
	pd.picType = PICTYPE_BITMAP;
	pd.bmp.hbitmap = bmp;
	pd.bmp.hpal = hpal;
	CComPtr<IPictureDisp> spPictDisp;
	HRESULT hr = OleCreatePictureIndirect(&pd,IID_IPictureDisp,
		TRUE,(void**) &spPictDisp);
	if(FAILED(hr))
		{
		::DeleteObject(bmp);
		if(hpal)
			::GlobalFree(hpal);
		MessageBox(szErrorText);
		return hr;
		}
	CComVariant v(spPictDisp);
	if(pProp->SetNewValue(&v))
		pProp->m_bDirty = TRUE;
	return S_OK;
	}

HRESULT CPropBrowseWnd::AddMetaFile(CPBProperty * pProp, LPCTSTR szFileName)
	{
	HENHMETAFILE hemf = GetEnhMetaFile(szFileName);
	if(!hemf)
		return E_FAIL;

	PICTDESC pd;
	ZeroMemory(&pd,sizeof(pd));
	//	pd.cbSizeOfStruct = sizeof(PICTDESC);
	pd.picType = PICTYPE_ENHMETAFILE;
	pd.emf.hemf = hemf;
	CComPtr<IPictureDisp> spPictDisp;
	HRESULT hr = OleCreatePictureIndirect(&pd,IID_IPictureDisp,
		TRUE,(void**) &spPictDisp);
	if(FAILED(hr))
		{
		MessageBox(_T("Error creating picture object!"));
		DeleteEnhMetaFile(hemf); 
		return hr;
		}
	CComVariant v(spPictDisp);
	if(pProp->SetNewValue(&v))
		pProp->m_bDirty = TRUE;
	return S_OK;
	}


HRESULT CPropBrowseWnd::SelectPicture(CPBProperty * pProp)
	{
	_ASSERT(pProp->m_DisplayType == CPBProperty::PBPropTypes::PBT_PICTURE);
	OPENFILENAME ofn = {0};
	TCHAR szFile[512]=_T("");
	ofn.lStructSize = _FILE_OPEN_SIZE_;//sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.hInstance = _Module.m_hInst;
	ofn.lpstrFilter = 
		_T("Image Files\0*.BMP;*.ICO;*.EMF\0Bitmap Files\0*.BMP\0Icon Files\0*.ICO\0Windows Metafiles\0*.EMF\0\0");
	ofn.lpstrCustomFilter = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = 511;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	BOOL bRes = GetOpenFileName(&ofn);
	if(!bRes)
		return S_FALSE;
	if(_tcsicmp(szFile + ofn.nFileExtension,_T("ICO")) == 0)
		return AddIconFile(pProp,szFile);
	else if(_tcsicmp(szFile + ofn.nFileExtension,_T("BMP")) == 0)
		return AddBitmapFile(pProp,szFile);
	else if(_tcsicmp(szFile + ofn.nFileExtension,_T("EMF")) == 0)
		return AddMetaFile(pProp,szFile);
	return S_FALSE;
	}


HRESULT CPropBrowseWnd::SelectFont(CPBProperty * pProp)
	{
	_ASSERT(pProp->m_DisplayType == CPBProperty::PBPropTypes::PBT_FONT);
	LOGFONT lf;	
	HRESULT hr = S_OK;
	CComPtr<IDispatch> pDisp;
	if(pProp->m_value.vt == VT_DISPATCH)
		pDisp = pProp->m_value.pdispVal;
	else if(pProp->m_value.vt == (VT_DISPATCH | VT_BYREF))
		pDisp = *(pProp->m_value.ppdispVal);
	if(!pDisp)
		::ZeroMemory(&lf,sizeof(lf));
	else
		{
		CComQIPtr<IFont,&IID_IFont> pFont(pDisp);
		HFONT hFont = NULL;
		if(pFont)
			{
			hr = pFont->get_hFont(&hFont);
			if(SUCCEEDED(hr))
				::GetObject(hFont,sizeof(lf),&lf);
			}
		}

	CHOOSEFONT cf;
	::ZeroMemory(&cf,sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = GetParent();
	cf.lpLogFont = &lf;
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
	if(!ChooseFont(&cf))
		return S_FALSE;

	USES_CONVERSION;
	FONTDESC fd;
	::ZeroMemory(&fd,sizeof(fd));
//	fd.cbSizeOfStruct = sizeof(fd); //<- UnComment this line if you have new header files
	fd.lpstrName = T2OLE(lf.lfFaceName);
	fd.cySize.Lo = cf.iPointSize*1000; // 
	fd.sWeight = (short)lf.lfWeight;
	fd.sCharset = lf.lfCharSet;
	fd.fItalic = lf.lfItalic;
	fd.fUnderline = lf.lfUnderline;
	fd.fStrikethrough = lf.lfStrikeOut;
	CComPtr<IFontDisp> pFontDisp;
	hr = OleCreateFontIndirect(&fd,IID_IFontDisp,(void**)&pFontDisp);
	if(FAILED(hr))
		return hr;
	CComVariant v(pFontDisp);
	if(pProp->SetNewValue(&v))
		pProp->m_bDirty = TRUE;
	return S_OK;
	}

HRESULT CPropBrowseWnd::TranslateAccelerator(MSG * lpmsg)
	{
	if(m_wndEdit.IsWindowVisible() && lpmsg->message == WM_KEYDOWN)
		{
		if(LOWORD(lpmsg->wParam) == VK_ESCAPE)
			{
			m_pEditProp = NULL; // to prevent apply property during KillFocus
			m_wndEdit.ShowWindow(SW_HIDE);
			return S_OK;
			}
		else if(LOWORD(lpmsg->wParam) == VK_RETURN)
			{
			m_wndEdit.ShowWindow(SW_HIDE);
			return S_OK;
			}
		if(m_bEditReadOnly)
			switch(LOWORD(lpmsg->wParam))
				{
				case VK_UP:
				case VK_DOWN:
					break;
				default:
				return S_OK;
				}
		}
	return S_FALSE;
	}

/////////  Color Picker's Window Implementation /////////////////////
PBSystemColorEntry CPBColorPicker::m_SysColors[]=
	{
		{0x80000000,_T("Scroll Bars")},
		{0x80000001,_T("Desktop")},
		{0x80000002,_T("Active Title Bar")},
		{0x80000003,_T("Inactive Title Bar")},
		{0x80000004,_T("Menu Bar")},
		{0x80000005,_T("Window Background")},
		{0x80000006,_T("Window Frame")},
		{0x80000007,_T("Menu Text")},
		{0x80000008,_T("Window Text")},
		{0x80000009,_T("Active Title Bar Text")},
		{0x8000000A,_T("Active Border")},
		{0x8000000B,_T("Inactive Border")},
		{0x8000000C,_T("Application Workspace")},
		{0x8000000D,_T("Highlight")},
		{0x8000000E,_T("Highlight Text")},
		{0x8000000F,_T("Button Face")},
		{0x80000010,_T("Button Shadow")},
		{0x80000011,_T("Disabled Text")},
		{0x80000012,_T("Button Text")},
		{0x80000013,_T("Inactive Title Bar Text")},
		{0x80000014,_T("Button Highlight")},
		{0x80000015,_T("Button Dark Shadow")},
		{0x80000016,_T("Button Light Shadow")},
		{0x80000017,_T("ToolTip Text")},
		{0x80000018,_T("ToolTip")},
		{0x00000000,NULL}

	};
OLE_COLOR	CPBColorPicker::m_clrPalette[]=
{
0x00FFFFFF,0x00C0C0ff,0x00C0E0ff,0x00C0FFFF,0x00C0FFC0,0x00FFFFC0,0x00FFC0C0,0x00FFC0FF,
0x00E0E0E0,0x008080FF,0x0080C0FF,0x0080FFFF,0x0080FF80,0x00FFFF80,0x00FF8080,0x00FF80FF,
0x00C0C0C0,0x000000FF,0x000080FF,0x0000FFFF,0x0000FF00,0x00FFFF00,0x00FF0000,0x00FF00FF,
0x00808080,0x000000C0,0x000040C0,0x0000C0C0,0x0000C000,0x00C0C000,0x00C00000,0x00C000C0,
0x00404040,0x00000080,0x00004080,0x00008080,0x00008000,0x00808000,0x00800000,0x00800080,
0x00000000,0x00000040,0x00404080,0x00004040,0x00004000,0x00404000,0x00400000,0x00400040,
0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,
0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF
};
BOOL CPBColorPicker::CreateWnd(CPropBrowseWnd * pParent)
	{
	_ASSERT(pParent);
	m_pParent = pParent;
	RECT r;
	SetRectEmpty(&r);
	r.right = r.bottom = 10;// 'll be updated soon
	if(Create(NULL,r,_T(""),WS_POPUP,WS_EX_TOOLWINDOW |WS_EX_DLGMODALFRAME) == NULL)
		{
		_ASSERT(FALSE);//creation failed
		return FALSE;
		}
	SetFont(pParent->GetFont());
	m_wndTab.SetFont(pParent->GetFont());
	m_wndList.SetFont(pParent->GetFont());
	TC_ITEM tci;
	::ZeroMemory(&tci,sizeof(tci));
	tci.mask = TCIF_TEXT;
	tci.pszText = _T("Palette");
	m_wndTab.SendMessage(TCM_INSERTITEM,0,(LPARAM)&tci);
	tci.pszText = _T("System");
	m_wndTab.SendMessage(TCM_INSERTITEM,1,(LPARAM)&tci);
	for(int i=0;m_SysColors[i].c;i++)
		{
		int nIdx = m_wndList.SendMessage(LB_ADDSTRING,0,(LPARAM)_T(""));
		m_wndList.SendMessage(LB_SETITEMDATA,nIdx,i);
		}
	CalcSizes();
	return TRUE;
	}

LRESULT CPBColorPicker::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	UINT nCtrl = (UINT)wParam;
	DRAWITEMSTRUCT *di = (DRAWITEMSTRUCT *)lParam;
	if(nCtrl == 2)
		DrawPalette(di);
	else if(nCtrl == 3)
		DrawListItem(di);
	return 0;
	}
LRESULT CPBColorPicker::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	bHandled = FALSE;
	RECT rc;
	GetClientRect(&rc);
	if(m_wndTab.Create(m_hWnd,rc,_T(""),
		WS_CHILD | WS_VISIBLE | TCS_SINGLELINE | TCS_FOCUSNEVER,0,1) == NULL)
		{
		_ASSERT(FALSE);
		}

	if(m_wndPalette.Create(m_hWnd,rc,_T(""),WS_CHILD | BS_NOTIFY | WS_VISIBLE | BS_OWNERDRAW,0,2)==NULL)
		{
		_ASSERT(FALSE);
		}
	if(m_wndList.Create(m_hWnd,rc,_T(""),WS_CHILD | WS_VSCROLL | LBS_NOTIFY |
		LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT,0,3) == NULL)
		{
		_ASSERT(FALSE);
		}
	return 0;
	}
void CPBColorPicker::CalcSizes()
	{
	int nSizeX = 0;
	HDC hdc = GetDC();
	HFONT hFont = (HFONT)::SelectObject(hdc,m_wndList.GetFont());
	for(int i=0;m_SysColors[i].szText;i++)
		{
		SIZE sizeText;
		::GetTextExtentPoint32(hdc,m_SysColors[i].szText,lstrlen(m_SysColors[i].szText),
			&sizeText);
		if(nSizeX < sizeText.cx + sizeText.cy)
			nSizeX = sizeText.cx + sizeText.cy;
		}

	::SelectObject(hdc,hFont);
	::ReleaseDC(m_hWnd,hdc);
	nSizeX += GetSystemMetrics(SM_CXVSCROLL);// add scrollbar
	nSizeX += GetSystemMetrics(SM_CXBORDER)*2;
	m_nCellSize = MulDiv(nSizeX,100,800);
	RECT r;
	r.left = r.top = 0;
	r.bottom =	r.right = m_nCellSize*8;
	m_wndTab.SendMessage(TCM_ADJUSTRECT,(WPARAM)TRUE,(LPARAM)&r);
	if(r.left < 0)
		{
		r.right -= r.left;
		r.left = 0;
		}
	if(r.top < 0)
		{
		r.bottom -= r.top;
		r.top = 0;
		}
	r.bottom += ::GetSystemMetrics(SM_CYEDGE)*2+2;
	r.right += ::GetSystemMetrics(SM_CXEDGE)*2+2;

	SetWindowPos(m_hWnd,&r,SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	GetClientRect(&r);
	m_wndTab.MoveWindow(&r,FALSE);
 	m_wndTab.SendMessage(TCM_ADJUSTRECT,(WPARAM)FALSE,(LPARAM)&r);
	::CopyRect(&m_rcDisplay,&r);
	m_wndList.MoveWindow(&r,FALSE);
	m_wndPalette.MoveWindow(&r,FALSE);
	}



HRESULT CPBColorPicker::PickColor(CPBProperty * pProp,RECT* r)
	{
	RECT rcWnd;
	m_pProp = pProp;
	GetWindowRect(&rcWnd);
	int x,y,h,w;
	h = rcWnd.bottom - rcWnd.top;
	w = rcWnd.right - rcWnd.left;
	x = r->right - w;
	if(x < 0) x = 0;
	if(r->bottom + h >= ::GetSystemMetrics(SM_CYSCREEN))
		y = r->top -  h; //drop Up
	else
		y = r->bottom;

	m_clr = (DWORD)m_pProp->m_value.lVal;
	SetStartColor(m_clr);
	BOOL bPicked = FALSE;
	SetWindowPos(HWND_TOP,x,y,w,h,SWP_SHOWWINDOW);
	SetFocus();
	m_nChooseColor = -1;
	for(;;)
		{
		MSG msg;
		::GetMessage(&msg, NULL, 0, 0);
		HWND hCaptureWnd = ::GetCapture();
		if (hCaptureWnd && (hCaptureWnd != m_hWnd && ::GetParent(hCaptureWnd) != m_hWnd))
			break;
		switch (msg.message)
			{
			case WM_ENDPICK:
				bPicked = msg.wParam;
				goto ExitLoop;
			case WM_RBUTTONDOWN:
				goto ExitLoop;
				break;
			case WM_KEYDOWN:
				if (msg.wParam == VK_ESCAPE)
					goto ExitLoop;
			default:
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				break;
			}

		}
	ExitLoop:
	ShowWindow(SW_HIDE);
	if(m_nChooseColor > 47)
		{
		CHOOSECOLOR cc;
		::ZeroMemory(&cc,sizeof(cc));
		cc.lStructSize = sizeof(cc);
		cc.hwndOwner = GetParent();
		cc.hInstance = NULL;
		cc.rgbResult = m_clrPalette[m_nChooseColor];
		cc.lpCustColors = &m_clrPalette[48];
		cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
		if(ChooseColor(&cc))
			{
			bPicked = TRUE;
			m_clr = cc.rgbResult;
			}
		}
	if(bPicked)
		{
		VARIANT v;
		v.vt = VT_UI4;
		v.lVal = m_clr;
		if(pProp->SetNewValue(&v) == FALSE)
			{
			_ASSERT(FALSE);
			return E_FAIL;
			}
		pProp->m_bDirty = TRUE;
		}
	return bPicked ? S_OK : S_FALSE;
	}

void CPBColorPicker::DrawPalette(LPDRAWITEMSTRUCT di)
	{
	HDC hdc = ::CreateCompatibleDC(di->hDC);
	_ASSERT(hdc);
	HBITMAP hbmp = ::CreateCompatibleBitmap(di->hDC,
		di->rcItem.right - di->rcItem.left,
		di->rcItem.bottom - di->rcItem.top);

	_ASSERT(hbmp);
	HBITMAP hbmp_old = (HBITMAP)::SelectObject(hdc,hbmp);
	HBRUSH hbr = (HBRUSH)::SelectObject(hdc,::GetStockObject(GRAY_BRUSH));
	::Rectangle
			(hdc,
			0,0,
			di->rcItem.right - di->rcItem.left,
			di->rcItem.bottom - di->rcItem.top
			);
	::SelectObject(hdc,hbr);
	HPEN hOldPen = (HPEN)::SelectObject(hdc,::GetStockObject(BLACK_PEN));
	for(int i=0;i < sizeof(m_clrPalette) / sizeof(m_clrPalette[0]);i++)
		{
		RECT r;COLORREF clr;
		r.left = m_nCellSize*(i%8);
		r.top = m_nCellSize*(i/8);
		r.right = r.left + m_nCellSize;
		r.bottom = r.top + m_nCellSize;
		clr = m_clrPalette[i];// in this case OLE_COLOR just 0x00RRGGBB == RGB()
		HBRUSH hBr;
		hBr = ::CreateSolidBrush(clr);
		r.right --;
		r.bottom --;
		if(m_nTrackIndex == i)
			{
			::SelectObject(hdc,::GetStockObject(WHITE_PEN));
			::Rectangle(hdc,r.left,r.top,r.right,r.bottom);
			::SelectObject(hdc,::GetStockObject(BLACK_PEN));
			}
		HBRUSH  hOldBr= (HBRUSH)::SelectObject(hdc,hBr);
		::InflateRect(&r,-1,-1);
		::Rectangle(hdc,r.left,r.top,r.right,r.bottom);
		::SelectObject(hdc,hOldBr);
		::DeleteObject(hBr);
		}
	::BitBlt(di->hDC,
			di->rcItem.left,
			di->rcItem.top,
			di->rcItem.right - di->rcItem.left,
			di->rcItem.bottom - di->rcItem.top,
			hdc,
			0,0,SRCCOPY);
	::SelectObject(hdc,hOldPen);
	::DeleteObject(::SelectObject(hdc,hbmp_old));
	::DeleteDC(hdc);
	}
void CPBColorPicker::DrawListItem(LPDRAWITEMSTRUCT di)
	{
	HDC hdc = di->hDC;
	RECT &rcItem = di->rcItem;
	RECT r;
	int i = (int)(di->itemData);
	::CopyRect(&r,&rcItem);
	r.right = r.left + r.bottom - r.top;
	::InflateRect(&r,-1,-1);
	COLORREF clr;
	OleTranslateColor(m_SysColors[i].c,NULL,&clr);
	COLORREF clrBack = ::GetBkColor(hdc);
	COLORREF clrText = ::GetTextColor(hdc);
	HPEN hPen= (HPEN)::SelectObject(hdc,GetStockObject(BLACK_PEN));
	HBRUSH hBr = ::CreateSolidBrush(clr);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hdc,hBr);
	::Rectangle(hdc,r.left,r.top,r.right,r.bottom);
	::SelectObject(hdc,hOldBrush);
	::DeleteObject(hBr);
	::CopyRect(&r,&rcItem);
	r.left += r.bottom - r.top;
	if(di->itemState & ODS_SELECTED)
		{
		::SetBkColor(hdc,::GetSysColor(COLOR_HIGHLIGHT));
		::SetTextColor(hdc,::GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
	else
		{
		::SetBkColor(hdc,::GetSysColor(COLOR_WINDOW));
		::SetTextColor(hdc,::GetSysColor(COLOR_WINDOWTEXT));
		}
	::ExtTextOut(hdc,r.left,r.top,
			ETO_OPAQUE | ETO_CLIPPED, &r,
			m_SysColors[i].szText,
			_tcslen(m_SysColors[i].szText),
			NULL);
	::SetBkColor(hdc,clrBack);
	::SetTextColor(hdc,clrText);
	::SelectObject(hdc,hPen);
	}
LRESULT CPBColorPicker::OnLButtonDownPalette(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	POINT pt;
	m_wndPalette.SetCapture();
	pt.x = (int)LOWORD(lParam);pt.y = (int) HIWORD(lParam);
	m_bTrackPaletteColor = FALSE;
	int nHit = HitTestPalette(pt);
	m_nTrackIndex = nHit;
	if(nHit < 0)
		return 0;
	m_bTrackPaletteColor = TRUE;
	m_clr = m_clrPalette[nHit];
	m_wndPalette.Invalidate();
	return 0;
	}
LRESULT CPBColorPicker::OnMouseMovePalette(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	POINT pt;
	pt.x = (int)LOWORD(lParam);pt.y = (int) HIWORD(lParam);
	if(!m_bTrackPaletteColor)
		return 0;
	int nHit = HitTestPalette(pt);
	if(nHit >= 0 && m_nTrackIndex != nHit)
		{
		m_clr = m_clrPalette[nHit];
		m_nTrackIndex = nHit;
		m_wndPalette.Invalidate();
		}
	m_nTrackIndex = nHit;
	return 0;
	}

LRESULT CPBColorPicker::OnLButtonUpPalette(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	POINT pt;
	if(::GetCapture() == m_wndPalette.m_hWnd)
		::ReleaseCapture();
	pt.x = (int)LOWORD(lParam);pt.y = (int) HIWORD(lParam);
	int nHit = HitTestPalette(pt);
	if(m_bTrackPaletteColor && nHit >= 0)
		{
		m_clr = m_clrPalette[nHit];
		PostMessage(WM_ENDPICK,1);
		}
	m_bTrackPaletteColor = FALSE;
	return 0;
	}
int CPBColorPicker::HitTestPalette(POINT p)
	{
	RECT r;
	m_wndPalette.GetClientRect(&r);
	int x = r.left,y = r.top;
	for(int i=0;i < sizeof(m_clrPalette) / sizeof(m_clrPalette[0]);i++)
		{
		r.left = x + m_nCellSize*(i%8);
		r.top = y + m_nCellSize*(i/8);
		r.right = r.left + m_nCellSize -1;
		r.bottom = r.top + m_nCellSize -1;
		if(::PtInRect(&r,p))
			return i;
		}
	return -1;
	}
// rect is relative to palette window
int CPBColorPicker::GetPaletteItemRect(int nIdx, RECT * r)
	{
	if(nIdx < 0 || nIdx >= sizeof(m_clrPalette) / sizeof(m_clrPalette[0]))
		{
		_ASSERT(FALSE);
		return -1;
		}
	m_wndPalette.GetClientRect(r);
	r->left += m_nCellSize*(nIdx%8);
	r->top  += m_nCellSize*(nIdx/8);
	r->right = r->left + m_nCellSize -1;
	r->bottom = r->top + m_nCellSize -1;
	return nIdx;
	}

void CPBColorPicker::SetStartColor(OLE_COLOR c)
	{
	m_wndTab.SendMessage(TCM_SETCURSEL,0);
	m_wndList.ShowWindow(SW_HIDE);
	m_wndPalette.ShowWindow(SW_SHOW);
	for(int i=0;m_SysColors[i].c;i++)
		if(c == m_SysColors[i].c)
			{
			m_wndList.SendMessage(LB_SETCURSEL,i);
			m_wndTab.SendMessage(TCM_SETCURSEL,1);
			m_wndList.ShowWindow(SW_SHOW);
			m_wndPalette.ShowWindow(SW_HIDE);
			m_nTrackIndex = -1;
			return;
			}
	m_wndList.SendMessage(LB_SETCURSEL,-1);
	for(i = 0;i < sizeof(m_clrPalette) / sizeof(m_clrPalette[0]);i++)
		if(c == m_clrPalette[i])
			{
			m_nTrackIndex = i;
			return;
			}
	for(i = 48;i < sizeof(m_clrPalette) / sizeof(m_clrPalette[0]);i++)
		if(m_clrPalette[i] == 0x00FFFFFF)
			{
			m_clrPalette[i] = c;
			m_nTrackIndex = i;
			return;
			}
	int nLastChanceIdx = sizeof(m_clrPalette) / sizeof(m_clrPalette[0])-1;
	m_clrPalette[nLastChanceIdx] = c;
	m_nTrackIndex = nLastChanceIdx;
	}

LRESULT CPBDropListBox::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	bHandled = FALSE;
	RECT rc;
	GetClientRect(&rc);
	if(m_wndList.Create(m_hWnd,rc,_T(""),WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY |
		LBS_NOINTEGRALHEIGHT,0,1) == NULL)
		{
		_ASSERT(FALSE);
		}
	return 0;
	}

int CPBDropListBox::LoadStrings(CPBProperty * pProp)
	{
	USES_CONVERSION;
	_ASSERT(::IsWindow(m_wndList.m_hWnd));
	_ASSERT(pProp);
	m_wndList.SendMessage(LB_RESETCONTENT);
	for(ULONG i=0;i < pProp->m_castr.cElems;i++)
		m_wndList.SendMessage(LB_ADDSTRING,0,(LPARAM)OLE2T(pProp->m_castr.pElems[i]));
	return pProp->m_castr.cElems;
	}

void CPBDropListBox::Display(RECT * r,CPBProperty *pProp,HWND hwndEdit)
	{
	int x,y,h,w;
	m_hwndBuddyEdit = hwndEdit;
	m_pProp = pProp;
	h = r->bottom - r->top;
	w = r->right - r->left;
	x = r->right - w;
	LoadStrings(pProp);
	TCHAR szText[128];
	::GetWindowText(hwndEdit,szText,127);
	int nHit = (int)m_wndList.SendMessage(LB_FINDSTRING,-1,(LPARAM)szText);
	if(nHit != LB_ERR)
		m_wndList.SendMessage(LB_SETCURSEL,nHit,0);
	h = (int)m_wndList.SendMessage(LB_GETITEMHEIGHT)*min(8,pProp->m_castr.cElems);
	h += GetSystemMetrics(SM_CYBORDER)*2;
	if(h == 0)
		return;
	if(x < 0) x = 0;
	if(r->bottom + h >= ::GetSystemMetrics(SM_CYSCREEN))
		y = r->top -  h; //drop Up
	else
		y = r->bottom;
	SetWindowPos(HWND_TOP,x,y,w,h,SWP_NOACTIVATE | SWP_SHOWWINDOW);
	RECT rcClient;
	GetClientRect(&rcClient);
	m_wndList.MoveWindow(&rcClient);
	}

LPOLESTR CPBProperty::GetNextPredefinedString(LPCTSTR szText)
	{
	LPOLESTR szNext = NULL;
	USES_CONVERSION;
	for(ULONG i=0;i < m_castr.cElems;i++)
		if(wcscmp(m_castr.pElems[i],T2COLE(szText)) == 0)
			if(i < m_castr.cElems - 1)
				return m_castr.pElems[i+1];
			else
				return  m_castr.pElems[0];
	return NULL;
	}



// PropertyBrowserPage.h: interface for the CPropertyBrowserPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTYBROWSERPAGE_H__12892D12_03FF_11D3_84F5_00C04FD1E2CA__INCLUDED_)
#define AFX_PROPERTYBROWSERPAGE_H__12892D12_03FF_11D3_84F5_00C04FD1E2CA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <COMMCTRL.H>

/*
Property Browser Page v1.0
Template class that provides automatic property page for ATL-based ActiveX Controls
Copyrights, (c) 1999 Vladimir Scherbakov (rise@alt.ru)

*/


/*
 CPropBrowseWnd custom messages
 */
#define WM_PB_STARTEDIT		(WM_APP + 12)
#define WM_PB_WARNING		(WM_APP + 14)
#define WM_PB_PROPCHANGE	(WM_APP + 16)
#define WM_PB_SHOWPOPUP		(WM_APP + 15)
/*
 Color picker custom message
 */
#define WM_ENDPICK			(WM_USER + 11) 
class CPropBrowseWnd;
class CPBProperty;



// used in color picker's 'System' tab
typedef struct
	{
	OLE_COLOR c;
	TCHAR *szText;
	} PBSystemColorEntry;

class CPBColorPicker : public CWindowImpl<CPBColorPicker>
	{
	public:
		OLE_COLOR m_clr;
 
		DECLARE_WND_CLASS(_T("PBColorPicker_1_0"))
		HRESULT PickColor(CPBProperty *pProp,RECT* r);
		BOOL CreateWnd(CPropBrowseWnd * pParent);
		CPBColorPicker():m_wndTab(WC_TABCONTROL,this,1),m_wndPalette(_T("BUTTON"),this,2),
			m_wndList(_T("LISTBOX"),this,3)
			{
			SetRectEmpty(&m_rcDisplay);
			m_bTrackPaletteColor = FALSE;
			m_nTrackIndex = -1;
			}
		~CPBColorPicker()
			{
			}
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		WORD fActive = LOWORD(wParam);
		if(fActive == WA_INACTIVE)
			{
			ShowWindow(SW_HIDE);// hide window now (if drag other window's caption)
			PostMessage(WM_ENDPICK);
			}
		return 0;
		}


	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		HWND hwndGetFocus = (HWND) wParam; 
		if(::GetParent(hwndGetFocus) != m_hWnd)
			PostMessage(WM_ENDPICK,0,0);
		return 0;
		}
	LRESULT OnKillFocusChild(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
		{
		HWND hwndGetFocus = GetFocus();
		if(hwndGetFocus != m_hWnd && ::GetParent(hwndGetFocus) != m_hWnd)
			PostMessage(WM_ENDPICK,0,0);
		return 0;
		}

	LRESULT OnTabSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
		{
		int nSel = TabCtrl_GetCurSel(m_wndTab.m_hWnd);
		m_wndPalette.ShowWindow(!nSel);
		m_wndList.ShowWindow(nSel);
		return 0;
		};

	LRESULT OnSelChangeList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
		{
		int nSel = m_wndList.SendMessage(LB_GETCURSEL);
		if(nSel >= 0)
			{
			m_clr = m_SysColors[nSel].c;
			PostMessage(WM_ENDPICK,1);
			}
		return 0;
		}

	LRESULT OnLButtonDownPalette(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUpPalette(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMovePalette(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkGndPalette(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		return 1;
		}
	LRESULT OnParentNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		WORD fwEvent = LOWORD(wParam);  // event flags 
		WORD idChild = HIWORD(wParam);  // identifier of child window 
		if(fwEvent == WM_RBUTTONDOWN && m_wndPalette.IsWindowVisible())
			{
			POINT pt;pt.x = (int)LOWORD(lParam);pt.y = (int)HIWORD(lParam);
			ClientToScreen(&pt);
			m_wndPalette.ScreenToClient(&pt);
			int nIdx = HitTestPalette(pt);
			if(nIdx > 47)
				m_nChooseColor = nIdx;
			}
		return 0;
		}


	BEGIN_MSG_MAP(CPBColorPicker)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DRAWITEM,OnDrawItem)
		MESSAGE_HANDLER(WM_KILLFOCUS,OnKillFocus)
		MESSAGE_HANDLER(WM_ACTIVATE,OnActivate)
		MESSAGE_HANDLER(WM_PARENTNOTIFY,OnParentNotify)

		NOTIFY_HANDLER(1,TCN_SELCHANGE,OnTabSelChange)
		COMMAND_HANDLER(3,LBN_KILLFOCUS,OnKillFocusChild)
		COMMAND_HANDLER(2,BN_KILLFOCUS,OnKillFocusChild)
		COMMAND_HANDLER(3,LBN_SELCHANGE,OnSelChangeList)

	ALT_MSG_MAP(1)
	ALT_MSG_MAP(2)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkGndPalette)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDownPalette)
		MESSAGE_HANDLER(WM_LBUTTONUP,OnLButtonUpPalette)
		MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMovePalette)

	ALT_MSG_MAP(3)
	END_MSG_MAP()
	protected:
		int m_nChooseColor;
		void SetStartColor(OLE_COLOR c);
		int m_nTrackIndex;
		int GetPaletteItemRect(int nIdx,RECT *r);
		int HitTestPalette(POINT p);
		void DrawListItem(LPDRAWITEMSTRUCT di);
		void DrawPalette(LPDRAWITEMSTRUCT di);
		void CalcSizes();
		int m_nCellSize;
		BOOL  m_bTrackPaletteColor;
		CContainedWindow m_wndTab;
		CContainedWindow m_wndPalette;
		CContainedWindow m_wndList;
		RECT m_rcDisplay;
		CPropBrowseWnd * m_pParent;
		CPBProperty *m_pProp;
	static PBSystemColorEntry m_SysColors[];
	static OLE_COLOR m_clrPalette[];
	};


class CPBProperty
	{
	public:
		LPOLESTR GetNextPredefinedString(LPCTSTR szText);
		BOOL SetNewValue(VARIANT *pVar);
		CComBSTR m_bstrText;
		BOOL m_bDirty;
		BOOL SetNewText(BSTR bstrNewText);
		CALPOLESTR     m_castr;
		CADWORD        m_cadw; 
		VARIANT * m_pvPredefinedValues;
	CPBProperty()
		{
		::ZeroMemory(&m_castr,sizeof(m_castr));
		::ZeroMemory(&m_cadw,sizeof(m_cadw));
		m_bDirty = FALSE;
		m_ulHelpContext = 0;
		m_DisplayType = PBT_ERROR;
		m_id = 0;
		m_vt = VT_NULL;
		}
	virtual ~CPBProperty()
		{
		ClearPredefinedValues();
		}

	void ClearPredefinedValues()
		{
		if(m_cadw.cElems > 0)
			{
			for(ULONG i=0;i < m_cadw.cElems;i++)
				VariantClear(m_pvPredefinedValues + i);
			CoTaskMemFree((void *)m_cadw.pElems);
			if(m_pvPredefinedValues)
				CoTaskMemFree((void *)m_pvPredefinedValues);
			m_pvPredefinedValues = NULL;
			m_cadw.cElems = 0;
			}
		if(m_castr.cElems > 0)
			{
			for (ULONG i=0; i < m_castr.cElems; i++)
				CoTaskMemFree((void *)m_castr.pElems[i]);  
			CoTaskMemFree((void *)m_castr.pElems); 
			m_castr.cElems = 0;
			}

		}
	HRESULT InitPredefinedValues(IDispatch * pDisp)
		{
		ULONG i = 0;HRESULT hr = S_OK;
		IPerPropertyBrowsing *pPropBrowse = NULL;
		ClearPredefinedValues();
		if(m_castr.cElems <=0 && m_DisplayType == PBT_BOOL)
			{
			m_castr.cElems = 2;
			m_castr.pElems = (LPOLESTR *)CoTaskMemAlloc(2*sizeof(LPOLESTR));
			m_cadw.cElems = 2;
			m_cadw.pElems = (DWORD FAR*)CoTaskMemAlloc(2*sizeof(DWORD FAR*));
			if (NULL==m_castr.pElems || NULL==m_cadw.pElems)
				{
				hr = E_OUTOFMEMORY;goto error;
				}
			for (int i = 0; i < 2; i++)
				{
				m_castr.pElems[i] = (LPOLESTR)CoTaskMemAlloc(128);
				if (!m_castr.pElems[i])
					{
					hr = E_OUTOFMEMORY;goto error;
					}
				}
			ocscpy(m_castr.pElems[0],L"False");m_cadw.pElems[0] = 0;
			ocscpy(m_castr.pElems[1],L"True");m_cadw.pElems[1] = 1;
			m_pvPredefinedValues = (VARIANT*) CoTaskMemAlloc(m_cadw.cElems*sizeof(VARIANT));
			m_pvPredefinedValues[0].vt = VT_BOOL;m_pvPredefinedValues[0].boolVal = FALSE;
			m_pvPredefinedValues[1].vt = VT_BOOL;m_pvPredefinedValues[1].boolVal = -1;
			return S_OK;
			}
		hr = pDisp->QueryInterface(IID_IPerPropertyBrowsing,(void**)&pPropBrowse);
		if(FAILED(hr)) goto error;
		hr = pPropBrowse->GetPredefinedStrings(m_id,&m_castr,&m_cadw);
		if(FAILED(hr)) goto error;
		if(m_cadw.cElems > 0)
			{
			m_pvPredefinedValues = (VARIANT*) CoTaskMemAlloc(m_cadw.cElems*sizeof(VARIANT));
			if(!m_pvPredefinedValues) { hr = E_OUTOFMEMORY;goto error;}
			}
		for(i=0;i < m_cadw.cElems;i++)
			{
			VariantInit(m_pvPredefinedValues + i);
			hr = pPropBrowse->GetPredefinedValue(m_id,m_cadw.pElems[i],m_pvPredefinedValues + i);
			if(FAILED(hr))
				goto error;
			}
		pPropBrowse->Release();
		return hr;
		error:
		ClearPredefinedValues();
		if(pPropBrowse) pPropBrowse->Release();
		return hr;
		}
	enum PBPropTypes
		{
		PBT_ERROR	=0,
		PBT_STR		=1,
		PBT_COLOR	=2,
		PBT_FONT	=3,
		PBT_ENUM	=4,
		PBT_PICTURE	=5,
		PBT_BOOL	=6
		} m_DisplayType ;

	static CPBProperty::PBPropTypes GetPropType(LPTYPEINFO pTypeInfo,TYPEDESC* lptd,VARTYPE *vt)
		{
		LPTYPEINFO lpUserTI=NULL;
		TYPEATTR* ta=NULL;
		PBPropTypes PropType = PBT_ERROR;
		if(vt)
			*vt = lptd->vt;
		switch(lptd->vt)
			{
				// simple types supported
			case VT_UI1:
			case VT_UI2:
			case VT_UI4:
			case VT_UI8:
			case VT_INT:
			case VT_UINT:
			case VT_I8:
			case VT_I1:
			case VT_I2:
			case VT_I4:
			case VT_R4:
			case VT_R8:
			case VT_BSTR:
			case VT_DATE:
			case VT_CY:
				return PBT_STR;
			case VT_BOOL:
				return PBT_BOOL;
			case VT_USERDEFINED:
				{
				// looks for OLE_COLOR and user-defined enums
				HRESULT hr;
		        hr = pTypeInfo->GetRefTypeInfo(lptd->hreftype,
					&lpUserTI);
		        if (FAILED(hr)) goto error;
				hr = lpUserTI->GetTypeAttr(&ta);
		        if (FAILED(hr)) goto error;
				switch(ta->typekind)
					{
					case TKIND_ALIAS:
						if(IsEqualGUID(ta->guid,GUID_COLOR))
							PropType = PBT_COLOR;
						else
						PropType= GetPropType(lpUserTI,&ta->tdescAlias,vt);
						break;
					case TKIND_ENUM:

						if(IsEqualGUID(ta->guid,GUID_COLOR))
							PropType = PBT_COLOR;
						else
						PropType = PBT_ENUM;
						break;
					case TKIND_DISPATCH:
						if(IsEqualGUID(ta->guid,IID_IFontDisp))
							PropType = PBT_FONT;
						else if(IsEqualGUID(ta->guid,IID_IPictureDisp))
							PropType = PBT_PICTURE;
						else PropType = PBT_ERROR;
						break;
					default:
						PropType = PBT_ERROR;
					}
				lpUserTI->ReleaseTypeAttr(ta);
				lpUserTI->Release();
				}
				break;
			case VT_PTR:
				// looks for IFontDisp* and IPictureDisp*
				PropType = GetPropType(pTypeInfo,lptd->lptdesc,vt);
				break;
			default:
				// any others are unsupported
				ATLTRACE(_T("[CPropertyBrowserPage]:Unsupported property type\n"));
				goto error;
			}
		return PropType;
		error:
		if(ta && lpUserTI)
			lpUserTI->ReleaseTypeAttr(ta);
		if(lpUserTI)
			lpUserTI->Release();
		return PBT_ERROR;
		}
	static HRESULT Create(ITypeInfo *pTypeInfo,unsigned int nIdx, CPBProperty ** ppProp)
		{
		CPBProperty* pProp = new CPBProperty();
		VARDESC *fd=NULL;
		FUNCDESC *fd2=NULL;
		PBPropTypes propType = PBT_ERROR;
		LPTYPEINFO ptinfoUserDefined = NULL;
		if(!pProp)
			return E_OUTOFMEMORY;
		_ASSERT(pTypeInfo);
		HRESULT hr = pTypeInfo->GetVarDesc(nIdx,&fd);
		bool bUseFunc = false;
		if(FAILED(hr)) 
		{
			bUseFunc = true;
			HRESULT hr = pTypeInfo->GetFuncDesc(nIdx,&fd2);
			if(FAILED(hr))
				goto error;
		}

		if(bUseFunc)
		{
			pProp->m_id = fd2->memid;
			//pProp->m_invkind = fd2->invkind;
			hr = pTypeInfo->GetDocumentation(pProp->m_id,
										&pProp->m_bstrName.m_str,
										&pProp->m_bstrDocumentation.m_str,
										&pProp->m_ulHelpContext,
										&pProp->m_bstrHelpFile
										);

			if(FAILED(hr)) goto error;
			if(fd2->cParams != 1)
				{
				hr = E_INVALIDARG;
				goto error;
				}
				
			propType = GetPropType(pTypeInfo,&fd2->lprgelemdescParam[0].tdesc,&pProp->m_vt);
			if(propType == PBT_ERROR)
				{
				hr = E_INVALIDARG;
				goto error;
				}
			pProp->m_DisplayType = propType;

			pTypeInfo->ReleaseFuncDesc(fd2);
		}
		else
		{
			pProp->m_id = fd->memid;
			//pProp->m_invkind = fd->varkind;
			hr = pTypeInfo->GetDocumentation(pProp->m_id,
										&pProp->m_bstrName.m_str,
										&pProp->m_bstrDocumentation.m_str,
										&pProp->m_ulHelpContext,
										&pProp->m_bstrHelpFile
										);

			if(FAILED(hr)) goto error;
			/*if(fd->cParams != 1)
				{
				hr = E_INVALIDARG;
				goto error;
				}
				*/
			propType = GetPropType(pTypeInfo,&fd->elemdescVar.tdesc,&pProp->m_vt);
			if(propType == PBT_ERROR)
				{
				hr = E_INVALIDARG;
				goto error;
				}
			pProp->m_DisplayType = propType;

			pTypeInfo->ReleaseVarDesc(fd);
		}
		(*ppProp) = pProp;
		return S_OK;
		error:
		if(pProp) delete pProp;
		if(fd) pTypeInfo->ReleaseVarDesc(fd);
		if(fd2)pTypeInfo->ReleaseFuncDesc(fd2);
		if(ptinfoUserDefined) ptinfoUserDefined->Release();
		(*ppProp) = NULL;
		return hr;
		}
	MEMBERID m_id;
	//VARKIND m_invkind;
	VARTYPE m_vt;
	CComVariant	m_value; 
	CComBSTR m_bstrName;
	CComBSTR m_bstrDocumentation;
	CComBSTR m_bstrHelpFile;
	unsigned long m_ulHelpContext;
	protected:
	};
static const int nAllocSize = 32;

class CPBPropertyHolder
	{
	public:
		CPBPropertyHolder()
			{
			m_ppProp = NULL;
			m_nCount = 0;
			m_nAllocCount = 0;
			}
		virtual ~CPBPropertyHolder()
			{
			CleanUp();
			}
		BOOL Init(IDispatch *pDisp)
			{
			ITypeInfo *pTypeInfo = NULL;

			TYPEATTR* ta;
			HRESULT hr =  pDisp->GetTypeInfo(0,NULL,&pTypeInfo);
			if(FAILED(hr)) {_ASSERT(FALSE); return FALSE;}
			hr =pTypeInfo->GetTypeAttr(&ta);
			if(FAILED(hr)) {_ASSERT(FALSE); return FALSE;}
			for(int i=0;i < ta->cVars;i++)
				AddFuncProperty(pTypeInfo,i,false);
			for(i=0;i < ta->cFuncs;i++)
				AddFuncProperty(pTypeInfo,i,true);
			pTypeInfo->ReleaseTypeAttr(ta);
			pTypeInfo->Release();
			pTypeInfo = NULL;
			CComDispatchDriver dd(pDisp);
			for(i = 0;i < m_nCount;i++)
				{
				CComVariant v;
				hr = dd.GetProperty(m_ppProp[i]->m_id,&v);
				_ASSERT(SUCCEEDED(hr));
				m_ppProp[i]->InitPredefinedValues(pDisp);
				m_ppProp[i]->SetNewValue(&v);
				}
			return TRUE;
			}
		int GetCount() {return m_nCount;}
		CPBProperty* operator[](int nIdx)
			{
			if(nIdx < 0 || nIdx >= m_nCount)
				{
				_ASSERT(FALSE);
				return NULL;
				}
			return m_ppProp[nIdx];
			}
		void CleanUp()
			{
			if(m_ppProp)
				{
				for(int i=0;i < m_nCount;i++)
					delete m_ppProp[i];
				delete[] m_ppProp;
				m_ppProp = NULL;
				m_nCount=0;
				m_nAllocCount = 0;
				}
			}
	protected:
		void AddFuncProperty(ITypeInfo * pTypeInfo,int nIdx, bool bUseFunc)
			{
			HRESULT hr;
			VARDESC* fd;
			FUNCDESC* fd2;
			if(bUseFunc)
				hr = pTypeInfo->GetFuncDesc(nIdx,&fd2);
			else
				hr = pTypeInfo->GetVarDesc(nIdx,&fd);
			if(FAILED(hr))
			{
				_ASSERT(FALSE); 
				return;
			}
			if(bUseFunc)
			{
				if(fd2->invkind == INVOKE_PROPERTYPUT || fd2->invkind == INVOKE_PROPERTYPUTREF)
				{
				// don't show hidden and non-browsable elements
				if(fd2->wFuncFlags & (FUNCFLAG_FHIDDEN | FUNCFLAG_FNONBROWSABLE))
					return;
				if(fd2->cParams != 1)
					return;
				if(CPBProperty::GetPropType(pTypeInfo,&fd2->lprgelemdescParam[0].tdesc,NULL) == CPBProperty::PBPropTypes::PBT_ERROR)
					return;
				if(!Find(fd2->memid))
					_Add(pTypeInfo,nIdx);
				}
			}
			else
			{
			// looking for a property (not read-only)
			//if(fd->varkind == INVOKE_PROPERTYPUT || fd->varkind == INVOKE_PROPERTYPUTREF)
				{
				// don't show hidden and non-browsable elements
				//if(fd->wFuncFlags & (FUNCFLAG_FHIDDEN | FUNCFLAG_FNONBROWSABLE))
				//	return;
				//if(fd->cParams != 1)
				//	return;
				if(CPBProperty::GetPropType(pTypeInfo,&fd->elemdescVar.tdesc,NULL) == CPBProperty::PBPropTypes::PBT_ERROR)
					return;
				if(!Find(fd->memid))
					_Add(pTypeInfo,nIdx);
				}
			}
			}
		CPBProperty** m_ppProp;
		int m_nCount;
		int m_nAllocCount;
		//dumb finder
		CPBProperty* Find(MEMBERID mid)
			{
			for(int i=0;i < m_nCount;i++)
				if(m_ppProp[i]->m_id == mid)
					return m_ppProp[i];
			return NULL;
			}
		static inline int _CmpPropNames(CPBProperty * px,CPBProperty* py)
			{
			return _wcsicmp(px->m_bstrName.m_str,py->m_bstrName.m_str);
			}
		HRESULT _Add(ITypeInfo * pTypeInfo,int nIdx)
			{
			int i=0;
			if(m_nAllocCount == m_nCount)
				if(!_AllocMore())
					return FALSE;
			CPBProperty* pProp = NULL;
			HRESULT hr = CPBProperty::Create(pTypeInfo,nIdx,&pProp);
			if(FAILED(hr)) goto error;
			for(i=0;i < m_nCount;i++)
				{
				if(_CmpPropNames(m_ppProp[i],pProp) < 0)
					continue;
				memmove(m_ppProp + i + 1,m_ppProp+i,sizeof(m_ppProp[0])*(m_nCount-i));
				m_ppProp[i] = pProp;
				m_nCount++;
				return S_OK;
				}
			m_ppProp[m_nCount] = pProp;
			m_nCount++;
			return S_OK;
			error:
			if(pProp)
				delete pProp;
			return hr;
			}
		BOOL _AllocMore()
			{
			CPBProperty** ppTemp = NULL;
			ppTemp = new CPBProperty*[m_nAllocCount + nAllocSize];
			if(!ppTemp)
				{
				_ASSERT(ppTemp);// out of memory?
				return FALSE;
				}
			_ASSERT(m_nCount <= m_nAllocCount);
			m_nAllocCount += nAllocSize;
			for(int i=0;i < m_nCount;i++)
				ppTemp[i]=m_ppProp[i];
			if(m_ppProp)
				delete[] m_ppProp;
			m_ppProp = ppTemp;
			return TRUE;
			}
	};

class CPBDropListBox : public CWindowImpl<CPBDropListBox>
	{
	enum { _ID_LIST=1};
	public:
		void Display(RECT *r,CPBProperty *pProp,HWND hwndEdit);
		int LoadStrings(CPBProperty * pProp);
		CPBDropListBox() : m_wndList(_T("LISTBOX"),this,1)
			{
			m_hwndBuddyEdit = NULL;
			m_pProp = NULL;
			}

		~CPBDropListBox()
			{
			}
	CContainedWindow m_wndList;
	HWND m_hwndBuddyEdit;
	CPBProperty* m_pProp;
	LRESULT OnSelChangeList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
		{
		int nSel = m_wndList.SendMessage(LB_GETCURSEL);
		if(nSel >= 0)
			{
			TCHAR szText[128];
			m_wndList.SendMessage(LB_GETTEXT,nSel,(LPARAM)szText);
			_ASSERT(m_hwndBuddyEdit);
			::SendMessage(m_hwndBuddyEdit,WM_SETTEXT,0,(LPARAM)szText);
			_ASSERT(m_pProp);
//			m_pProp->m_bDirty = TRUE;
			}
		ShowWindow(SW_HIDE);
		return 0;
		}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	BEGIN_MSG_MAP(CPBDropListBox)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		COMMAND_HANDLER(1,LBN_SELCHANGE,OnSelChangeList)

	ALT_MSG_MAP(1)
	END_MSG_MAP()
	};


class CPropBrowseWnd : public CWindowImpl<CPropBrowseWnd>
	{
	enum {_ID_EDIT=1};
	public:
		HRESULT TranslateAccelerator(MSG* pMsg);
		BOOL Create(HWND hWndParent,LPRECT prcPos,UINT nId);
		void AddProperty(CPBProperty * pProp);
		CPropBrowseWnd():m_wndEdit(_T("EDIT"),this,1)
			{
			m_pEditProp = NULL;
			m_bEditReadOnly = FALSE;
			m_nColWidth = 0;
			m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
			m_clrBack = ::GetSysColor(COLOR_WINDOW);
			m_clrHighlite = ::GetSysColor(COLOR_HIGHLIGHT);
			m_clrHighliteText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			m_hGridPen = CreatePen(PS_SOLID,1,::GetSysColor(COLOR_3DLIGHT));

			m_clrDirtyText = RGB(0xC0,0x00,0x00);// red color
			}
		virtual ~CPropBrowseWnd()
			{
			if(m_hGridPen)
				::DeleteObject(m_hGridPen);
			if(::IsWindow(m_wndColor.m_hWnd))
				m_wndColor.DestroyWindow();
			if(::IsWindow(m_wndLBox.m_hWnd))
				m_wndLBox.DestroyWindow();
			}
	DECLARE_WND_SUPERCLASS(_T("PropBrowseWnd_1_0"),_T("LISTBOX"))

	LRESULT DrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDoubleClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// mesage handler for start edit
	LRESULT OnStartEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// message handler to safe display a message box 
	LRESULT OnDisplayWarning(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnKillFocusEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLDoubleClickEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysKeyDownEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDownEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCutPasteEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		bHandled = m_bEditReadOnly;
		return 0;
		}


BEGIN_MSG_MAP(CPropBrowseWnd)
	MESSAGE_HANDLER(WM_CREATE,OnCreate)
	MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
	MESSAGE_HANDLER(WM_PB_STARTEDIT,OnStartEdit)
	MESSAGE_HANDLER(WM_PB_WARNING,OnDisplayWarning)
	MESSAGE_HANDLER(WM_LBUTTONDBLCLK,OnLButtonDoubleClick)
	MESSAGE_HANDLER(WM_PB_SHOWPOPUP,OnShowPopup)
ALT_MSG_MAP(1)
	MESSAGE_HANDLER(WM_KILLFOCUS,OnKillFocusEdit)
	MESSAGE_HANDLER(WM_PASTE,OnCutPasteEdit)
	MESSAGE_HANDLER(WM_CUT,OnCutPasteEdit)
	MESSAGE_HANDLER(WM_LBUTTONDBLCLK,OnLDoubleClickEdit)
	MESSAGE_HANDLER(WM_SYSKEYDOWN,OnSysKeyDownEdit)
	MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDownEdit)
END_MSG_MAP()
	protected:
		HRESULT AddMetaFile(CPBProperty * pProp,LPCTSTR szFileName);
		HRESULT AddBitmapFile(CPBProperty * pProp,LPCTSTR szFileName);
		HRESULT AddIconFile(CPBProperty * pProp,LPCTSTR szFileName);
		HRESULT SelectPicture(CPBProperty * pProp);
		BOOL m_bEditReadOnly;
		HRESULT SelectFont(CPBProperty *pProp);
		void DrawItemArrow(HDC hDC,LPRECT prcItem,CPBProperty * pProp,BOOL bSelected);
		BOOL NeedArrow(CPBProperty *pProp);
		static int m_nDeltaX;
		int GetItemWidth(CPBProperty* pProp);
		int m_nColWidth;
		COLORREF	m_clrText;
		COLORREF	m_clrBack;
		COLORREF	m_clrHighlite;
		COLORREF	m_clrHighliteText;
		COLORREF	m_clrDirtyText;
		HPEN		m_hGridPen;
		CPBProperty * m_pEditProp;	// current editing property
		CContainedWindow m_wndEdit;
		CPBColorPicker m_wndColor;
		CPBDropListBox m_wndLBox;
		RECT m_rcEdit;
	};


template <class T> class CPropertyBrowserPage :
				public IPropertyPageImpl<T>,
				public CDialogImpl<T>

{
public:
	CPropertyBrowserPage()
		{
		}
	virtual ~CPropertyBrowserPage()
		{
		}

	STDMETHOD(TranslateAccelerator)( MSG* pMsg )
		{
		HRESULT hr = m_wndList.TranslateAccelerator(pMsg);
		if(hr != S_OK)
			return IPropertyPageImpl<T>::TranslateAccelerator(pMsg);
		else
			return hr;
		}
	STDMETHOD(SetObjects)( ULONG nObjects, IUnknown** ppUnk )
		{
		HRESULT hr = IPropertyPageImpl<T>::SetObjects(nObjects,ppUnk );
		if(FAILED(hr))
			return hr;
		IDispatch * pDisp;

		m_Array.CleanUp();

		if(nObjects > 0)
			{
			hr = ppUnk[0]->QueryInterface(IID_IDispatch,(void**)&pDisp);
			if(FAILED(hr))	{_ASSERT(FALSE);return E_FAIL;}
			m_Array.Init(pDisp);
			pDisp->Release();
			}
		if(::IsWindow(m_wndList.m_hWnd))
			{
			m_wndList.SendMessage(LB_RESETCONTENT);// clear list box
			for(int i=0;i < m_Array.GetCount();i++)
				m_wndList.AddProperty(m_Array[i]);
			}
		
		return S_OK;
		}

	enum {_ID_LISTWND = 110};
	LRESULT OnPropertyChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		// receive notification from listbox about property has been changed
		SetDirty(TRUE);
		return 0;
		}

// looks like ATL's implementation has a bug so implement the very own one
	void SetDirty(BOOL bDirty)
		{
		T* pT = static_cast<T*>(this);
		if (!pT->m_bDirty && bDirty)
			{
			pT->m_bDirty = bDirty;// this line should be added to fix the problem
			// (to provide subsequent IsPageDirty() call with valid m_bDirty value)
			pT->m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY | PROPPAGESTATUS_VALIDATE);
			}
		pT->m_bDirty = bDirty;
		}

	STDMETHOD(Apply)(void)
		{
		ATLTRACE(_T("CPropertyBrowserPage::Apply\n"));
		for (UINT i = 0; i < m_nObjects; i++)
			{
			CComQIPtr<IDispatch,&IID_IDispatch> pDisp(m_ppUnk[i]);
			if(!pDisp)
				{
				ATLTRACE(_T("Failed to obtain a IDispatch pointer!\n"));
				continue;
				}
			CComDispatchDriver dd(pDisp);
			for(int j=0;j < m_Array.GetCount();j++)
				{
				HRESULT hr = dd.PutProperty(m_Array[j]->m_id,&(m_Array[j]->m_value));
				if(FAILED(hr))
					{
					MessageBox(_T("Failed to apply properties!"),_T("Error"),MB_OK | MB_ICONSTOP);
					}
				m_Array[j]->m_bDirty = FALSE;
				}
			}
		m_bDirty = FALSE;
		m_wndList.RedrawWindow();
		return S_OK;
		}

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		// just delegate
		return m_wndList.DrawItem(uMsg,wParam,lParam,bHandled);
		}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		RECT r,rd;
		GetClientRect(&r);
		::SetRectEmpty(&rd);
		rd.right = rd.bottom = 4;
		::MapDialogRect(m_hWnd,&rd);
		r.left += rd.right;
		r.top += rd.bottom;
		r.right -= rd.right;
		r.bottom -= rd.bottom;
		BOOL bRet = m_wndList.Create(m_hWnd,&r,_ID_LISTWND);
		_ASSERT(bRet);
			for(int i=0;i < m_Array.GetCount();i++)
				m_wndList.AddProperty(m_Array[i]);
		return 0;
		}
protected:
	CPropBrowseWnd m_wndList;
	CPBPropertyHolder m_Array;
BEGIN_MSG_MAP(CPropertyBrowserPage)
	MESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)
	MESSAGE_HANDLER(WM_DRAWITEM,OnDrawItem)
	MESSAGE_HANDLER(WM_PB_PROPCHANGE,OnPropertyChange)
END_MSG_MAP()
};

#endif // !defined(AFX_PROPERTYBROWSERPAGE_H__12892D12_03FF_11D3_84F5_00C04FD1E2CA__INCLUDED_)

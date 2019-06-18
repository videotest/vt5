#include "stdafx.h"
#include "Classes.h"
#include "ClassActions.h"
#include "ChooseClassDlg.h"
#include "Utils.h"

#include "Resource.h"
#include "popupint.h"
#include "\vt5\common2\class_utils.h"


//////////////////////////////////////////////////////////////////////
//
//	Class CSubMenu
//
//////////////////////////////////////////////////////////////////////
class CSubMenu : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CSubMenu);
	
protected:
	CSubMenu();
	~CSubMenu();

	UINT	m_uiStartItemID;
	UINT	m_uiItemIDCounter;
	CObjectListWrp	m_listClasses;

	void Init();

	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(SubMenu,ISubMenu)
		com_call SetStartItemID( UINT uiItemID );
		com_call SetSingleObjectName( BSTR bstrObjectName );
		com_call GetFirstItemPos( long* plPos );
		com_call GetNextItem(	
				UINT* puiFlags, 
				UINT* puiItemID, 
				BSTR* pbstrText,				
				UINT* puiParentID,
				long* plPos );
		com_call OnCommand( UINT uiCmd );	
	END_INTERFACE_PART(SubMenu)
public:
	
};

//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSubMenu, CCmdTargetEx)
IMPLEMENT_UNKNOWN(CSubMenu, SubMenu)

BEGIN_INTERFACE_MAP(CSubMenu, CCmdTargetEx)
	INTERFACE_PART(CSubMenu, IID_ISubMenu, SubMenu)
END_INTERFACE_MAP()

//////////////////////////////////////////////////////////////////////
CSubMenu::CSubMenu()
{			
	m_uiStartItemID = -1;
	m_uiItemIDCounter = -1;
	
}

//////////////////////////////////////////////////////////////////////
CSubMenu::~CSubMenu()
{	
	//DestroyMenuDiscription();
}

//////////////////////////////////////////////////////////////////////
HRESULT CSubMenu::XSubMenu::SetStartItemID( UINT uiItemID )
{
	METHOD_PROLOGUE_EX(CSubMenu, SubMenu)
	pThis->m_uiStartItemID = uiItemID;
	pThis->m_uiItemIDCounter = uiItemID;
	pThis->Init();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CSubMenu::XSubMenu::SetSingleObjectName( BSTR bstrObjectName )
{
	METHOD_PROLOGUE_EX(CSubMenu, SubMenu)
	bstrObjectName  = ::SysAllocString(L"SingleObjectName");
	return S_OK;
}



//////////////////////////////////////////////////////////////////////
HRESULT CSubMenu::XSubMenu::GetFirstItemPos( long* plPos )
{
	METHOD_PROLOGUE_EX(CSubMenu, SubMenu)
	*plPos = (long)pThis->m_listClasses.GetFirstObjectPosition();
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CSubMenu::XSubMenu::GetNextItem(	
										UINT* puiFlags, 
										UINT* puiItemID, 
										BSTR* pbstrText, 							
										UINT* puiParentID,
										long* plPos 
										)
{
	METHOD_PROLOGUE_EX(CSubMenu, SubMenu)
	
	POSITION pos = (POSITION)(*plPos);
	IUnknown* punkClass = pThis->m_listClasses.GetNextObject(pos);
	*plPos = (long)pos;
	if(punkClass)
	{
		*puiFlags = MF_STRING|MF_ENABLED;
		*puiItemID = pThis->m_uiItemIDCounter++;
		*pbstrText = ::GetObjectName(punkClass).AllocSysString();
		*puiParentID = -1;
	}
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CSubMenu::XSubMenu::OnCommand( UINT uiCmd )
{
	METHOD_PROLOGUE_EX(CSubMenu, SubMenu)

	long nClassNumber = uiCmd - pThis->m_uiStartItemID;
	long nClassCounter = 0;
	POSITION pos = pThis->m_listClasses.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkClass = pThis->m_listClasses.GetNextObject(pos);
		if(nClassCounter == nClassNumber)
		{
			::ExecuteAction("ObjectSetClass", CString("\"")+::GetObjectName(punkClass)+CString("\""), 0);
			break;
		}
		nClassCounter++;
	}

	return S_OK;
}

void CSubMenu::Init()
{
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp == NULL )
		return;
	IUnknown* punkDoc = NULL;
	ptrApp->GetActiveDocument(&punkDoc);
	if(!punkDoc)
		return;	

	
	IUnknown* punkList = 0;
	punkList = ::GetActiveObjectFromDocument(punkDoc, szTypeClassList);
	if(!punkList)
		punkList = ::GetActiveObjectFromContext(punkDoc, szTypeClassList);

	m_listClasses.Attach(punkList, false);

	punkDoc->Release();
}


// inner helper functions
bool CheckNameExists(IUnknown * punkData, LPCTSTR szName);


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSetActiveClass, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectSetClassDirect, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionClassDelete,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionClassCreate,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionClassRecolor,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionClassRename,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectSetClass,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCalcStat,		CCmdTargetEx);

//[ag]2. olecreate


// {8BC9D507-4B89-4513-A669-65204FB6206E}
GUARD_IMPLEMENT_OLECREATE(CActionSetActiveClass, "Classes.SetActiveClass",
0x8bc9d507, 0x4b89, 0x4513, 0xa6, 0x69, 0x65, 0x20, 0x4f, 0xb6, 0x20, 0x6e);
// {A9E0562A-0EDC-4d54-AA24-EC630BF725DE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectSetClassDirect, "Classes.ObjectSetClassDirect",
0xa9e0562a, 0xedc, 0x4d54, 0xaa, 0x24, 0xec, 0x63, 0xb, 0xf7, 0x25, 0xde);
// {0374125E-9321-41d3-A39A-4F59F34ECC39}
GUARD_IMPLEMENT_OLECREATE(CActionClassDelete, "Classes.ClassDelete",
0x374125e, 0x9321, 0x41d3, 0xa3, 0x9a, 0x4f, 0x59, 0xf3, 0x4e, 0xcc, 0x39);
// {13AD0617-3E08-424a-9750-D80FD56E194F}
GUARD_IMPLEMENT_OLECREATE(CActionClassCreate, "Classes.ClassCreate",
0x13ad0617, 0x3e08, 0x424a, 0x97, 0x50, 0xd8, 0xf, 0xd5, 0x6e, 0x19, 0x4f);
// {3979CE65-5657-49d7-8EBA-AB376673EC6F}
GUARD_IMPLEMENT_OLECREATE(CActionClassRecolor, "Classes.ClassRecolor",
0x3979ce65, 0x5657, 0x49d7, 0x8e, 0xba, 0xab, 0x37, 0x66, 0x73, 0xec, 0x6f);
// {D01C1B9E-13AA-41d3-BFF5-F055B0AD8327}
GUARD_IMPLEMENT_OLECREATE(CActionClassRename, "Classes.ClassRename",
0xd01c1b9e, 0x13aa, 0x41d3, 0xbf, 0xf5, 0xf0, 0x55, 0xb0, 0xad, 0x83, 0x27);
// {9C8EEEBF-6C5B-4469-B99A-404D4DFF9EEC}
GUARD_IMPLEMENT_OLECREATE(CActionObjectSetClass, "Classes.ObjectSetClass",
0x9c8eeebf, 0x6c5b, 0x4469, 0xb9, 0x9a, 0x40, 0x4d, 0x4d, 0xff, 0x9e, 0xec);
// {20A77B88-DA68-486e-AE19-166EA715CC22}
GUARD_IMPLEMENT_OLECREATE(CActionCalcStat, "Classes.CalcStat",
0x20a77b88, 0xda68, 0x486e, 0xae, 0x19, 0x16, 0x6e, 0xa7, 0x15, 0xcc, 0x22);
//[ag]5. guidinfo 

// {CEC9F71B-9C78-4a5b-B9AA-B93CC5ED0A8B}
static const GUID guidSetActiveClass =
{ 0xcec9f71b, 0x9c78, 0x4a5b, { 0xb9, 0xaa, 0xb9, 0x3c, 0xc5, 0xed, 0xa, 0x8b } };
// {B3B40E12-AC4D-4a49-BBD1-8F6E3620A073}
static const GUID guidObjectSetClassDirect =
{ 0xb3b40e12, 0xac4d, 0x4a49, { 0xbb, 0xd1, 0x8f, 0x6e, 0x36, 0x20, 0xa0, 0x73 } };
// {902D1E9B-F2CE-47b5-B580-AB850551E272}
static const GUID guidClassDelete =
{ 0x902d1e9b, 0xf2ce, 0x47b5, { 0xb5, 0x80, 0xab, 0x85, 0x5, 0x51, 0xe2, 0x72 } };
// {89759C3A-0CCC-4457-B7A0-2B540BFFCA1F}
static const GUID guidClassCreate =
{ 0x89759c3a, 0xccc, 0x4457, { 0xb7, 0xa0, 0x2b, 0x54, 0xb, 0xff, 0xca, 0x1f } };
// {5A3489C1-963D-498f-B7B6-EFAF05E51707}
static const GUID guidClassRecolor =
{ 0x5a3489c1, 0x963d, 0x498f, { 0xb7, 0xb6, 0xef, 0xaf, 0x5, 0xe5, 0x17, 0x7 } };
// {84E527D1-5739-4eff-9A7C-81AA33287D19}
static const GUID guidClassRename =
{ 0x84e527d1, 0x5739, 0x4eff, { 0x9a, 0x7c, 0x81, 0xaa, 0x33, 0x28, 0x7d, 0x19 } };
// {8AB0C3DD-5079-4306-ACA1-7E81D822B0D6}
static const GUID guidObjectSetClass =
{ 0x8ab0c3dd, 0x5079, 0x4306, { 0xac, 0xa1, 0x7e, 0x81, 0xd8, 0x22, 0xb0, 0xd6 } };
// {39BFEB0A-0C51-4d2c-98C6-C33A8C579280}
static const GUID guidCalcStat =
{ 0x39bfeb0a, 0xc51, 0x4d2c, { 0x98, 0xc6, 0xc3, 0x3a, 0x8c, 0x57, 0x92, 0x80 } };

//[ag]6. action info

ACTION_INFO_FULL(CActionSetActiveClass, IDS_ACTION_SETACTIVECLASS, -1, -1, guidSetActiveClass);
ACTION_INFO_FULL(CActionObjectSetClassDirect, IDS_ACTION_OBJECTSETCLASSDIRECT, -1, -1, guidObjectSetClassDirect);
ACTION_INFO_FULL(CActionClassDelete, IDS_ACTION_CLASS_DELETE, IDS_MENU_EDIT, -1, guidClassDelete);
ACTION_INFO_FULL(CActionClassCreate, IDS_ACTION_CLASS_CREATE, IDS_MENU_EDIT, -1, guidClassCreate);
ACTION_INFO_FULL(CActionClassRecolor, IDS_ACTION_CLASS_RECOLOR, IDS_MENU_EDIT, -1, guidClassRecolor);
ACTION_INFO_FULL(CActionClassRename, IDS_ACTION_CLASS_RENAME, IDS_MENU_EDIT, -1, guidClassRename);
ACTION_INFO_FULL(CActionObjectSetClass, IDS_ACTION_OBJECTSETCLASS, IDS_MENU_EDIT, IDS_TB_CLASSES, guidObjectSetClass);
ACTION_INFO_FULL(CActionCalcStat, IDS_ACTION_CALCSTAT, IDS_MENU_EDIT, IDS_TB_CLASSES, guidCalcStat);
//[ag]7. targets

ACTION_TARGET(CActionSetActiveClass,	szTargetAnydoc);
ACTION_TARGET(CActionObjectSetClassDirect, szTargetAnydoc);
ACTION_TARGET(CActionClassDelete,		szTargetAnydoc);
ACTION_TARGET(CActionClassCreate,		szTargetAnydoc);
ACTION_TARGET(CActionClassRecolor,		szTargetAnydoc);
ACTION_TARGET(CActionClassRename,		szTargetAnydoc);
ACTION_TARGET(CActionObjectSetClass,	szTargetAnydoc);
ACTION_TARGET(CActionCalcStat,			szTargetAnydoc);

//[ag]8. arguments
ACTION_ARG_LIST(CActionSetActiveClass)
	ARG_STRING("ClassName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionClassDelete)
	ARG_STRING("ClassName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionClassCreate)
	ARG_STRING("ClassName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionClassRecolor)
	ARG_STRING("ClassName", 0)
	ARG_INT("Color", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionClassRename)
	ARG_STRING("OldName", 0)
	ARG_STRING("NewName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectSetClass)
	ARG_STRING("ClassName", 0)
END_ACTION_ARG_LIST()

ACTION_UI(CActionObjectSetClass, CSubMenu)

ACTION_ARG_LIST(CActionObjectSetClassDirect)
	ARG_STRING("ObjectName", 0)
	ARG_STRING("ClassName", 0)
END_ACTION_ARG_LIST()


//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionSetActiveClass implementation
CActionSetActiveClass::CActionSetActiveClass()
{
	m_strClassName = _T("");
}

CActionSetActiveClass::~CActionSetActiveClass()
{
}

bool CActionSetActiveClass::ExecuteSettings(CWnd *pwndParent)
{
	if (!m_punkTarget)
		return false;

	m_strClassName = GetArgumentString(_T("ClassName"));

	// get classes list
	m_listClasses.Attach(::GetActiveObjectFromDocument(m_punkTarget, szTypeClassList), false);

	if (m_listClasses == 0)
		return false;

	// sure classlist exists
	ASSERT(m_listClasses != 0);

	// if list has not any class yet 
	if (!m_listClasses.GetCount())
		return false;

	// if it's need show dialog  for choose class
	if (ForceNotShowDialog() && m_strClassName.IsEmpty())
		return false;

	if (m_strClassName.IsEmpty() || ForceShowDialog())
	{
		CChooseClassDlg dlg(m_listClasses, pwndParent);
		
		CString strDlgTitle;
		strDlgTitle.LoadString(IDS_CHOOSECLASSTOACTIVATE);

		dlg.SetDialogTitle(strDlgTitle);
		dlg.SetClassName(m_strClassName);

		if (dlg.DoModal() != IDOK)
			return false;

		m_strClassName = dlg.GetClassName();
	}
	
	if (m_strClassName.IsEmpty()) 
		return false;

	SetArgument(_T("ClassName"), _variant_t(m_strClassName));

	return true;
}

bool CActionSetActiveClass::Invoke()
{
	ASSERT(m_punkTarget);

	if (!m_punkTarget)
		return false;

	// get key for selected class
	POSITION pos = m_listClasses.GetFirstObjectPosition();
	while (pos)
	{
		POSITION PrevPos = pos;

		IUnknown *punk = m_listClasses.GetNextObject(pos);
		if (!punk)
			continue;

		INamedObject2Ptr sptrObj(punk);
		punk->Release();

		BSTR bstrName = 0;
		if (SUCCEEDED(sptrObj->GetName(&bstrName)))
		{	
			CString strName = bstrName;
			::SysFreeString(bstrName);

			if (m_strClassName == strName)
			{
				m_listClasses.SetCurPosition(PrevPos);
				return true;
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionObjectSetClassDirect implementation
CActionObjectSetClassDirect::CActionObjectSetClassDirect()
{
	m_lPrevClassKey = -1;
	m_lNewClassKey = -1;
//	m_strClassName = _T("");
//	m_strObjectName = _T("");
//	m_changes;	//undo
//	m_sptrObj;
}

CActionObjectSetClassDirect::~CActionObjectSetClassDirect()
{
	m_sptrObj = 0;
}

bool CActionObjectSetClassDirect::ExecuteSettings(CWnd *pwndParent)
{
	ASSERT(m_punkTarget);
	if (!m_punkTarget)
		return false;

// get class & object by its names
	CString strObjectName = GetArgumentString(_T("ObjectName"));
	CString strClassName = GetArgumentString(_T("ClassName"));

	if (strObjectName.IsEmpty() || strClassName.IsEmpty())
		return false;

	// find object
	IUnknown * punkObj = ::FindObjectByName(m_punkTarget, strObjectName);
	if (!punkObj)
		return false;

	m_sptrObj = punkObj;
	punkObj->Release();
	if (m_sptrObj == 0)
		return false;

	// find class
	IUnknown * punkClass = ::FindObjectByName(m_punkTarget, strClassName);
	if (!punkClass)
		return false;

	if (!CheckInterface(punkClass, IID_IClassObject))
	{
		punkClass->Release();
		m_sptrObj == 0;
		return false;
	}

	// get class key from class
	m_lNewClassKey = get_object_class(ICalcObjectPtr(punkClass));
	punkClass->Release();

	// get object's prev class key
	m_lPrevClassKey = get_object_class( m_sptrObj );

	return true;
}

bool CActionObjectSetClassDirect::Invoke()
{
	ASSERT(m_punkTarget);
	if (!m_punkTarget)
		return false;

	if (m_sptrObj == 0)
		return false;

	if (m_lNewClassKey == m_lPrevClassKey)
		return true;

	set_object_class( m_sptrObj, m_lNewClassKey );
	INamedDataObject2Ptr sptr = m_sptrObj;

	// notify througth parent abouot object is changed 
	NotifyParent();

	return true;
}

//undo/redo
bool CActionObjectSetClassDirect::DoUndo()
{
	ASSERT(m_punkTarget);
	ASSERT(m_sptrObj != 0);
	if (m_sptrObj == 0)
		return false;

	set_object_class( m_sptrObj, m_lPrevClassKey );
	NotifyParent();
//	m_changes.DoUndo(m_punkTarget);
	return true;
}

bool CActionObjectSetClassDirect::DoRedo()
{
	ASSERT(m_punkTarget);
	ASSERT(m_sptrObj != 0);
	if (m_sptrObj == 0)
		return false;

	set_object_class( m_sptrObj, m_lNewClassKey );
	NotifyParent();
//	m_changes.DoRedo(m_punkTarget);
	return true;
}

// notify througth parent abouot object is changed 
void CActionObjectSetClassDirect::NotifyParent()
{
	INamedDataObject2Ptr sptr = m_sptrObj;
	if (sptr == 0)
		return;

	// notify througth parent abouot object is changed 
	IUnknown *punkParent = 0;
	if (FAILED(sptr->GetParent(&punkParent)) || !punkParent)
		return;

	IDataObjectListPtr sptrParent = punkParent;
	punkParent->Release();

	if (sptrParent)
		sptrParent->UpdateObject(sptr);
}

//////////////////////////////////////////////////////////////////////
//CActionClassDelete implementation
CActionClassDelete::CActionClassDelete()
{
	m_strClassName = _T("");
	m_punkObj = 0;
}

CActionClassDelete::~CActionClassDelete()
{
	if (m_punkObj)
		m_punkObj->Release();
	m_punkObj = 0;
}

bool CActionClassDelete::ExecuteSettings(CWnd *pwndParent)
{
	ASSERT(m_punkTarget);
	if (!m_punkTarget)
		return false;

	m_strClassName = GetArgumentString(_T("ClassName"));

	// get selected (active) class list
	IUnknown * punkList = ::GetActiveObjectFromDocument(m_punkTarget, szTypeClassList);
	
	m_punkObj = ::FindObjectByName(m_punkTarget, m_strClassName);
	if (!m_punkObj)
		return false;

	return true;
}

bool CActionClassDelete::Invoke()
{
	ASSERT(m_punkTarget);
	ASSERT(m_punkObj);

	if (!m_punkTarget || !m_punkObj)
		return false;

	m_changes.RemoveFromDocData(m_punkTarget, m_punkObj);
	m_punkObj->Release();
	m_punkObj = 0;

	return true;
}

//undo/redo
bool CActionClassDelete::DoUndo()
{
	ASSERT(m_punkTarget);
	m_changes.DoUndo(m_punkTarget);
	return true;
}

bool CActionClassDelete::DoRedo()
{
	ASSERT(m_punkTarget);
	m_changes.DoRedo(m_punkTarget);
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionClassCreate implementation
CActionClassCreate::CActionClassCreate()
{
	m_strClassName = _T("");
}

CActionClassCreate::~CActionClassCreate()
{
}

bool CActionClassCreate::ExecuteSettings(CWnd *pwndParent)
{
	if (!m_punkTarget)
		return false;

	m_strClassName = GetArgumentString(_T("ClassName"));

	// get selected (active) class list
	IUnknown * punkList = ::GetActiveObjectFromDocument(m_punkTarget, szTypeClassList);
	
	if (!punkList)
	{
		punkList = ::CreateTypedObject(szTypeClassList);
		if (!punkList)
		{
			SetError(IDS_ERR_CANTCREATECLASSLIST);
			return false;
		}
		m_changes.SetToDocData(m_punkTarget, punkList);
	}

	m_listWrp.Attach(punkList, false);

	// sure classlist exists
	ASSERT(m_listWrp != 0);
	if (m_listWrp == 0)
		return false;

	SetArgument(_T("ClassName"), _variant_t(m_strClassName));

	return true;
}

bool CActionClassCreate::Invoke()
{
	ASSERT(m_punkTarget);
	ASSERT(CheckInterface(m_punkTarget, IID_INamedData));

	if (!m_punkTarget)
		return false;

	IUnknown * punkObj = ::CreateTypedObject(szTypeClass);
	if (!punkObj)
		return false;

	INamedObject2Ptr sptrN = punkObj;
	punkObj->Release();
	if (sptrN == 0)
		return false;
	
	_bstr_t bstrName = m_strClassName;
	sptrN->SetName(bstrName);

	INamedDataObject2Ptr sptrObj = sptrN;
	if (sptrObj == 0)
		return false;

	DWORD dwFlags = 0;
	sptrObj->GetObjectFlags(&dwFlags);
	sptrObj->SetParent(m_listWrp, dwFlags);

	m_changes.SetToDocData(m_punkTarget, sptrObj);
	
	return true;
}

//undo/redo
bool CActionClassCreate::DoUndo()
{
	ASSERT(m_punkTarget);
	m_changes.DoUndo(m_punkTarget);
	return true;
}

bool CActionClassCreate::DoRedo()
{
	ASSERT(m_punkTarget);
	m_changes.DoRedo(m_punkTarget);
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionClassRecolor implementation
CActionClassRecolor::CActionClassRecolor()
{
	m_clOld = m_clNew = ::get_class_color(0);
	IClassObjectPtr	m_sptrClass;
}

CActionClassRecolor::~CActionClassRecolor()
{
}

bool CActionClassRecolor::ExecuteSettings(CWnd *pwndParent)
{
	ASSERT(m_punkTarget);
	if (!m_punkTarget)
		return false;

	// get arguments
	CString strName = GetArgumentString(_T("ClassName"));
	m_clNew = (COLORREF)GetArgumentInt(_T("Color"));

	IUnknown * punkObj = ::FindObjectByName(m_punkTarget, strName);
	if (!punkObj)
		return false;

	m_sptrClass = punkObj;
	punkObj->Release();
	
	if (m_sptrClass == 0)
		return false;

	DWORD dwColor = (DWORD)m_clOld;
	if (FAILED(m_sptrClass->GetColor(&dwColor)))
		return false;

	m_clOld = (COLORREF)dwColor;

	return true;
}

bool CActionClassRecolor::Invoke()
{
	if (m_sptrClass == 0)
		return false;

	if (m_clOld == m_clNew)
		return true;

	if (FAILED(m_sptrClass->SetColor((DWORD)m_clNew)))
		return false;

	return true;
}

//undo/redo
bool CActionClassRecolor::DoUndo()
{
	if (m_sptrClass == 0)
		return false;

	m_sptrClass->SetColor((DWORD)m_clOld);

	return true;
}

bool CActionClassRecolor::DoRedo()
{
	if (m_sptrClass == 0)
		return false;

	m_sptrClass->SetColor((DWORD)m_clNew);
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionClassRename implementation
CActionClassRename::CActionClassRename()
{
	m_strOldName = _T("");
	m_strNewName = _T("");
}

CActionClassRename::~CActionClassRename()
{
}

bool CActionClassRename::ExecuteSettings(CWnd *pwndParent)
{
	ASSERT(m_punkTarget);
	if (!m_punkTarget)
		return false;

	// get old and new names
	m_strOldName = GetArgumentString(_T("OldName"));
	m_strNewName = GetArgumentString(_T("NewName"));

	if (m_strOldName.IsEmpty() || m_strNewName.IsEmpty())
		return false;

	// check "newName" is not exists
	if (CheckNameExists(m_punkTarget, m_strNewName))
		return false;
	
	// find object 
	IUnknown *punkObj = ::FindObjectByName(m_punkTarget, m_strOldName);
	if (!punkObj)
		return false;

	m_sptrObject = punkObj;
	punkObj->Release();

	return true;
}

bool CActionClassRename::Invoke()
{
	ASSERT(m_punkTarget);
	ASSERT(m_sptrObject != 0);
	if (m_sptrObject == 0)
		return false;

	return ::RenameObject(m_punkTarget, m_sptrObject, m_strNewName);
}

//undo/redo
bool CActionClassRename::DoUndo()
{
	if (!m_punkTarget || m_sptrObject == 0)
		return false;
	
	return ::RenameObject(m_punkTarget, m_sptrObject, m_strOldName);
}

bool CActionClassRename::DoRedo()
{
	if (!m_punkTarget || m_sptrObject == 0)
		return false;

	return ::RenameObject(m_punkTarget, m_sptrObject, m_strNewName);
}


//////////////////////////////////////////////////////////////////////
//CActionObjectSetClass implementation
CActionObjectSetClass::CActionObjectSetClass()
{
	m_lClassKey = -1;
}

void CActionObjectSetClass::Clear()
{
	for (int i = 0; i < m_arrObjects.GetSize(); i++)
	{
		((IUnknown*)m_arrObjects[i])->Release();
	}
	m_arrObjects.RemoveAll();
	m_arrKeys.RemoveAll();
}

CActionObjectSetClass::~CActionObjectSetClass()
{
	Clear();
}

bool CActionObjectSetClass::ExecuteSettings(CWnd *pwndParent)
{
	Clear();

	m_strClassName = GetArgumentString(_T("ClassName"));
	_bstr_t bstrClassName(m_strClassName);

	if (!m_punkTarget)
		return false;

	// get selected (active) object(s)
	sptrIDocumentSite	sptrS(m_punkTarget);
	IUnknown* punkV = 0;
	sptrS->GetActiveView(&punkV);
	if (!punkV) 
		return false;

	sptrIView	sptrV(punkV );
	punkV->Release();
	
	if (sptrV)
	{
		IUnknown* punkMF = 0;
		sptrV->GetMultiFrame(&punkMF, false);
		sptrIMultiSelection sptrMF(punkMF);
		if (punkMF)
			punkMF->Release();
		if (sptrMF)
		{
			DWORD nObjects = 0;	
			sptrMF->GetObjectsCount(&nObjects);
			for(int i = 0; i < nObjects; i++)
			{
				IUnknown* punk = 0;
				sptrMF->GetObjectByIdx(i, &punk);
				if(punk)
				{
					m_arrObjects.Add(punk);
				}
			}
		}
	}

	if (m_arrObjects.GetSize() < 1)
		return false;
	
	// get active object list
	if (m_listObjects == 0)
		m_listObjects.Attach(GetActiveList(szTypeObjectList), false);

	if (m_listObjects == 0)
	{
		Clear();
		return false;
	}

	// sure objectlist exists
	ASSERT(m_listObjects != 0);

	sptrINamedData	sptrD(m_punkTarget);
	// get classes list
	if (m_listClasses == 0)
		m_listClasses.Attach(GetActiveList(szTypeClassList), false);

	if (m_listClasses == 0)
	{
		//create a classes list
		IUnknown	*punkObjects = ::CreateTypedObject(szTypeClassList);
		if (!punkObjects)
		{
			SetError(IDS_ERR_CANTCREATECLASSLIST);
			Clear();
			return false;
		}

		m_changes.SetToDocData(m_punkTarget, punkObjects);
		m_listClasses.Attach(punkObjects, false);
	}
	// sure classlist exists
	ASSERT(m_listClasses != 0);

//	::GetObjectByName();
	// if list has not any class yet we must exit(create one)
	if (!m_listClasses.GetCount())
	{
		Clear();
		return false;
	}
/*	{
		IUnknown* punk = ::CreateTypedObject(szTypeClass);
		if (!punk)
			return false;

		// set default name
		INamedObject2Ptr strN(punk);
		_bstr_t bstrNewName(szTypeClass);
		strN->GenerateUniqueName(bstrNewName);

		m_listClasses.Add(punk);
		m_changes.SetToDocData(m_punkTarget, punk);

		punk->Release();
	}
*/
	// if it's need show dialog  for choose class
	if (ForceNotShowDialog() && m_strClassName.IsEmpty())
	{
		Clear();
		return false;
	}

	if (m_strClassName.IsEmpty() || ForceShowDialog())
	{
		CChooseClassDlg dlg(m_listClasses, pwndParent);
		
		CString strDlgTitle;
		strDlgTitle.LoadString(IDS_CHOOSECLASSTOSELECT);

		dlg.SetDialogTitle(strDlgTitle);
		dlg.SetClassName(m_strClassName);

		if (dlg.DoModal() != IDOK)
		{
			Clear();
			return false;
		}

		m_strClassName = dlg.GetClassName();
	}
	
	if (m_strClassName.IsEmpty()) 
	{
		Clear();
		return false;
	}

	SetArgument(_T("ClassName"), _variant_t(m_strClassName));

	return true;
}

/*

получить активный список объектов и выбранные объекты (если нет выбранных объектов - берутся все объекты).
получить  активный список классов( если его нет - создать )
передать список классов в диалог выбора классов
получить classKey для выбранного класса.
назначить его всем выбранным объектам из списка объектов (или всему списку).



в диалоге выбора классов:
перечислить классы и заполнить ими диалог клаасов( в диалоге)
	(если нет ни одного класса - создать один)




*/

long CActionObjectSetClass::GetClassKey(LPCTSTR szSelectedClassName)
{
	// enumerate all objects in list and fill listbox
	POSITION pos = m_listClasses.GetFirstObjectPosition();
	while (pos)
	{
		IUnknown *punk = m_listClasses.GetNextObject(pos);
		if (!punk)
			continue;

		INamedObject2Ptr sptrObj(punk);
		punk->Release();

		BSTR bstrName = 0;
		if (SUCCEEDED(sptrObj->GetName(&bstrName)))
		{	
			CString strName = bstrName;
			::SysFreeString(bstrName);

			if (m_strClassName == strName)
			{
				return get_object_class( ICalcObjectPtr( sptrObj ) );
			}
		}
	}

	return -1;
}


IUnknown* CActionObjectSetClass::GetActiveList(LPCTSTR szObjectType)
{
	if (CheckInterface(m_punkTarget, IID_IDocument))
		return ::GetActiveObjectFromDocument(m_punkTarget, szObjectType);
	else
		return ::GetActiveObjectFromContext(m_punkTarget, szObjectType);
}

bool CActionObjectSetClass::Invoke()
{
	ASSERT( m_punkTarget );
	ASSERT( CheckInterface( m_punkTarget, IID_INamedData ) );

	if (!m_punkTarget)
		return false;

//	sptrINamedData	sptrD( m_punkTarget );

	// get key for selected class
	m_lClassKey = GetClassKey(m_strClassName);

	if (m_arrObjects.GetSize() < 1)
		return false;

	for (int i = 0; i < m_arrObjects.GetSize();	i++)
	{
		m_arrKeys.SetAtGrow(i, -1);
		ICalcObjectPtr sptrC((IUnknown*)m_arrObjects[i]);
		if (sptrC != 0)
		{
			long	lClassKey = get_object_class( sptrC );
			if( lClassKey != -1 )
				m_arrKeys.SetAtGrow( i, lClassKey );

			set_object_class( sptrC, m_lClassKey );

			::FireEventNotify( m_punkTarget, szEventChangeObjectList, m_listObjects, sptrC, cncChange);
		}
	}
	return true;
}

//undo/redo
bool CActionObjectSetClass::DoUndo()
{
	ASSERT(m_punkTarget);

	for (int i = 0; i < m_arrObjects.GetSize(); i++)
	{
		ICalcObjectPtr sptrC((IUnknown*)m_arrObjects[i]);
		if (sptrC != 0)set_object_class( sptrC, m_arrKeys[i] );
	}

	m_changes.DoUndo(m_punkTarget);
	return true;
}

bool CActionObjectSetClass::DoRedo()
{
	ASSERT(m_punkTarget);

	m_changes.DoRedo(m_punkTarget);

	for (int i = 0; i < m_arrObjects.GetSize(); i++)
	{
		ICalcObjectPtr sptrC((IUnknown*)m_arrObjects[i]);
		if (sptrC != 0)set_object_class( sptrC, m_lClassKey );
	}

	return true;
}

//extended UI
bool CActionObjectSetClass::IsAvaible()
{
	sptrIDocumentSite	sptrS(m_punkTarget);
	IUnknown* punkV = 0;
	sptrS->GetActiveView(&punkV);
	if (!punkV)
		return false;
	sptrIView	sptrV( punkV );
	punkV->Release();

	IUnknown* punkMF = 0;
	sptrV->GetMultiFrame(&punkMF, false);
	sptrIMultiSelection sptrMF(punkMF);
	if(!punkMF)
		return false;
	punkMF->Release();

	DWORD nObjects = 0;	
	sptrMF->GetObjectsCount(&nObjects);

	return nObjects > 0;
}

//////////////////////////////////////////////////////////////////////
//CActionCalcStat implementation
CActionCalcStat::CActionCalcStat()
{
}

CActionCalcStat::~CActionCalcStat()
{
}


bool CActionCalcStat::Invoke()
{
	INamedDataObject2Ptr	ptrClasses; // active classes list
	CCompManager	managerMeasurement( szStatistic );
	managerMeasurement.Init();


	
	// get active object list
	
	IUnknown	*punkClasses = GetActiveList(szTypeClassList);
	if( !punkClasses )return false;
	ptrClasses = punkClasses;
	punkClasses->Release();
	int	nGroupCount = managerMeasurement.GetComponentCount();

	for( int nGroup = 0; nGroup < nGroupCount; nGroup++ )
	{
		IMeasParamGroupPtr	ptrGroup = managerMeasurement.GetComponent( nGroup, false );
		ptrGroup->InitializeCalculation( punkClasses );

		long	lpos = 0;
		ptrClasses->GetFirstChildPosition( &lpos );

		while( lpos )
		{
			IUnknown	*punkCalcObject = 0;
			ptrClasses->GetNextChild( &lpos, &punkCalcObject );
			ptrGroup->CalcValues( punkCalcObject, 0 );
			punkCalcObject->Release();
		}

		ptrGroup->FinalizeCalculation();
	}

	return true;
}

//extended UI
bool CActionCalcStat::IsAvaible()
{
	if (!m_punkTarget)
		return false;

	CObjectListWrp list;
	list.Attach(GetActiveList(szTypeClassList), false);
	return list.GetCount() != 0;
}

IUnknown* CActionCalcStat::GetActiveList(LPCTSTR szObjectType)
{
	if (CheckInterface(m_punkTarget, IID_IDocument))
		return ::GetActiveObjectFromDocument(m_punkTarget, szObjectType);
	else
		return ::GetActiveObjectFromContext(m_punkTarget, szObjectType);
}


// determine name of object is exists in namedData
bool CheckNameExists(IUnknown * punkData, LPCTSTR szName)
{
	if (!punkData || !lstrlen(szName))
		return false; 

	IUnknown * punk = ::FindObjectByName(punkData, szName);
	if (!punk)
		return false;
	punk->Release();

	return true;
}

// AviFillter.cpp: implementation of the CAviFillter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AviFillter.h"
#include "AviBase.h"


#include "nameconsts.h"
#include "resource.h"
#include "misc5.h"

#include "misc_utils.h"
#include "action5.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAviFillter::CAviFillter()
{
	m_bstrDocumentTemplate = _bstr_t("Image");
	char szBuff[256];
	::LoadString(app::instance()->m_hInstance, IDS_AVI_FILES, szBuff, 256);
	m_bstrFilterName = szBuff;
	m_bstrFilterExt = _bstr_t(".avi");
	AddDataObjectType(szArgumentTypeImage);

}

//////////////////////////////////////////////////////////////////////
CAviFillter::~CAviFillter()
{

}

//////////////////////////////////////////////////////////////////////
bool CAviFillter::ReadFile( const char *pszFileName )
{
	IUnknown* punk = 0;
	punk = CreateAviObject();
	if( !punk )
		return false;

	IAviObjectPtr ptrAvi( punk );
	punk->Release();	punk = 0;

	if( ptrAvi == 0 )
		return false;
		
	if (FAILED(ptrAvi->CreateLinkToFile( _bstr_t( pszFileName ) )))
		return false;

	TCHAR szName[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_PATH];
	TCHAR ext[_MAX_PATH];
	_splitpath(pszFileName, drive, dir, szName, ext);
	_bstr_t bstrName(szName);
	long	NameExists = 0;
	if( m_sptrINamedData != 0 )
		m_sptrINamedData->NameExists(bstrName, &NameExists);
	// if new name not exists allready
	if (!NameExists && punk)
	{
		// set this name to object
		INamedObject2Ptr sptrObj(punk);
		sptrObj->SetName(bstrName);
	}


	ptrAvi->AddRef();
	m_CreatedObjects.AddToTail( ptrAvi );	

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAviFillter::WriteFile( const char *pszFileName )
{
	VTMessageBox( IDS_WARNING_CANT_SAVE, App::handle(), szDllName, MB_OK | MB_ICONSTOP );
	return false;
}

//////////////////////////////////////////////////////////////////////
bool CAviFillter::_InitNew()
{
	return false;
}

HRESULT CAviFillter::IsFileFormatReadonly( BOOL *pbIsReadOnly )
{
	*pbIsReadOnly = TRUE;
	return S_OK;
}

HRESULT CAviFillter::ShowSaveNotSupportedMessage()
{
	VTMessageBox( IDS_WARNING_CANT_SAVE, App::handle(), szDllName, MB_OK | MB_ICONSTOP );
	return S_OK;
}


// AuditTrailMan.cpp : implementation file
//

#include "stdafx.h"
#include "AuditTrail.h"
#include "AuditTrailMan.h"
#include "CreateATdlg.h"
#include "CreateATdlg.h"
#include "misc5.h"
#include "undoint.h"



template<>UINT AFXAPI HashKey(GuidKey&key)
{
	return key.Data1;
}

/////////////////////////////////////////////////////////////////////////////
// CAuditTrailMan

IMPLEMENT_DYNCREATE(CAuditTrailMan, CCmdTargetEx)

CAuditTrailMan::CAuditTrailMan()
{
	_OleLockApp( this );

	m_bInitializedEL = false;
	m_bOnAT = false;
	m_sptrLastAction = 0;
}

CAuditTrailMan::~CAuditTrailMan()
{
	_OleUnlockApp( this );
}


void CAuditTrailMan::OnFinalRelease()
{
	if(m_bInitializedEL)
	{
		UnRegister(GetAppUnknown());
		m_bInitializedEL = false;
	}

	_CleanUp();

	CCmdTargetEx::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CAuditTrailMan, CCmdTargetEx)
	//{{AFX_MSG_MAP(CAuditTrailMan)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CAuditTrailMan, CCmdTargetEx)
	INTERFACE_PART(CAuditTrailMan, IID_IEventListener,	EvList)	
	INTERFACE_PART(CAuditTrailMan, IID_IAuditTrailMan,	ATman)
END_INTERFACE_MAP()

// {245C04F4-BCAC-4d99-ABBA-B7E52E2B3212}
GUARD_IMPLEMENT_OLECREATE(CAuditTrailMan, "AuditTrail.AuditTrailMan", 
0x245c04f4, 0xbcac, 0x4d99, 0xab, 0xba, 0xb7, 0xe5, 0x2e, 0x2b, 0x32, 0x12);

IMPLEMENT_UNKNOWN(CAuditTrailMan,	ATman);

HRESULT CAuditTrailMan::XATman::ToggleOnOffFlag()
{
	METHOD_PROLOGUE_EX(CAuditTrailMan, ATman);
	pThis->m_bOnAT = !pThis->m_bOnAT;
	if(pThis->m_bOnAT)
		pThis->ProcessAction(pThis->m_sptrLastAction);
	return S_OK;
}

HRESULT CAuditTrailMan::XATman::GetCreateFlag(BOOL* pbWillCreate)
{
	METHOD_PROLOGUE_EX(CAuditTrailMan, ATman);
	if(pbWillCreate)
	{
		*pbWillCreate = pThis->m_bOnAT;
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT CAuditTrailMan::XATman::GetAuditTrail(IUnknown* punkImage, IUnknown** ppunkAT)
{
	METHOD_PROLOGUE_EX(CAuditTrailMan, ATman);
	if(punkImage && ppunkAT)
	{
		*ppunkAT = pThis->GetATbyImage(punkImage);
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT CAuditTrailMan::XATman::RescanActionsList()
{
	METHOD_PROLOGUE_EX(CAuditTrailMan, ATman);
	pThis->_FillActionList();
	return S_OK;
}




/////////////////////////////////////////////////////////////////////////////
// CAuditTrailMan message handlers

static bool s_bCreateAT = false;
static IUnknown* s_punkDoc = 0;
static char szAction[255] = "";
static char szActionParams[512] = "";
void CAuditTrailMan::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if(strcmp(pszEvent, szEventAfterOpen) == 0)
	{
		s_bCreateAT = true;
		s_punkDoc = punkFrom;
		strcpy(szAction, "FileOpen");
		strcpy(szActionParams, (char*)pdata);
	}
	else if(strcmp(pszEvent, szEventAfterLoadObject) == 0)
	{
		if(GetObjectKind(punkFrom) == szArgumentTypeImage)
			CreateAT(punkFrom, "ObjectLoad", _GetActionParams(punkHit));
	}
	else if(strcmp(pszEvent, szEventAfterInputImage) == 0)
	{
		if(GetObjectKind(punkFrom) == szArgumentTypeImage)
			CreateAT(punkFrom, "InputImageInCurrentDoc", _GetActionParams(punkHit));
	}
	else if(strcmp(pszEvent, szEventBeforeClose) == 0)
	{
		_CleanUp(punkFrom);
	}
	else if(strcmp( pszEvent, szAppCreateView) == 0)
	{
		if(s_bCreateAT && s_punkDoc != 0)
		{
			s_bCreateAT = false;
			CreateAT(s_punkDoc, szAction, szActionParams);
			s_punkDoc = 0;
			_strnset(szAction, '\0', 255);

		}
	}
	else if(strcmp(pszEvent, szEventBeforeActionExecute) == 0)
	{
		if(GetActionName(punkHit) == "Undo")
		{
			IActionPtr	sptrA(punkHit);
			if(sptrA != 0)
			{
				IUnknown* punkTarget = 0;
				sptrA->GetTarget(&punkTarget);//it's document in this case
				if(punkTarget)
				{
					GuidKey giudDoc = ::GetObjectKey(punkTarget);
					ATinfo* pATinfo = 0;
					GuidKey giud;
					GuidKey keyImage;
					POSITION pos = m_mapImageToATinfo.GetStartPosition();	
					while(pos)
					{
						m_mapImageToATinfo.GetNextAssoc(pos, giud, pATinfo);
						if(::GetObjectKey(pATinfo->sptrDoc) == giudDoc)
						{
							IAuditTrailObjectPtr sptrATobj(pATinfo->sptrObject);
							if(sptrATobj != 0)
							{
								IUndoListPtr	ptrUndo(punkTarget);
								if(ptrUndo != 0)
								{
									IUnknown* punkAction = 0;
									ptrUndo->GetLastUndoAction(&punkAction);
									if(punkAction)
									{
										if(sptrATobj->UndoAction(&::GetObjectKey(punkAction), &keyImage) == S_OK)
										{	
											GuidKey keyEmpty;
											ASSERT(keyImage != keyEmpty);
											IFilterActionPtr sptrFilterAction(punkAction);
											if(sptrFilterAction != 0)
											{
												TPOS nPos = 0;
												sptrFilterAction->GetFirstArgumentPosition(&nPos);
												IUnknown* punkDataObject = 0;
												bool bFound = false;
												while(nPos)
												{
													BSTR bstrArgType = 0;
													BOOL bOut = FALSE;
													sptrFilterAction->GetArgumentInfo(nPos, &bstrArgType, 0, &bOut);
													sptrFilterAction->GetNextArgument(&punkDataObject, &nPos);
													CString strType(bstrArgType);
													::SysFreeString(bstrArgType);

													if(bOut == FALSE && strType == szTypeImage)
													{
														if(punkDataObject)
														{
															sptrINamedDataObject2	sptrN(punkDataObject);
															if(sptrN != 0)
															{
																IUnknown* punkP = 0;
																sptrN->GetParent(&punkP);
																if(punkP)
																{
																	punkDataObject->Release();
																	punkDataObject = punkP;
																}
															}

															if(GetObjectKey(punkDataObject) == keyImage)
															{
																bFound = true;
																pATinfo->sptrImage = punkDataObject;
																m_mapImageToATinfo.RemoveKey(giud);
																m_mapImageToATinfo.SetAt(keyImage, pATinfo);
															}
														}
													}
													if(punkDataObject)
														punkDataObject->Release();
													punkDataObject = 0;
												}
												//ASSERT(bFound);
											}
										}
										punkAction->Release();
									}
								}
							}	
						}
					}
					punkTarget->Release();
				}
			}
		}
	}
	else if(strcmp(pszEvent, szEventAfterActionExecute) == 0)
	{
		ProcessAction(punkHit);
	}	
}

BOOL CAuditTrailMan::OnCreateAggregates()
{
	Register(GetAppUnknown());

	m_bInitializedEL = true;

	_FillActionList();
		
	return CCmdTargetEx::OnCreateAggregates();
}

	
void CAuditTrailMan::CreateAT(IUnknown* punkDoc, CString strAction, CString strParams)
{
	if(!m_bOnAT)
		return;

	CCreateATdlg dlg;

	bool bReleaseDoc = false;

	if(strAction == "ObjectLoad" || strAction == "InputImageInCurrentDoc")
	{
		dlg.m_arrImages.Add(::GetObjectName(punkDoc));
		INamedDataObject2Ptr sptrNDO(punkDoc);
		sptrNDO->GetData(&punkDoc);
		bReleaseDoc = true;
	}
	else
	{
		IDataContext2Ptr	sptrContext(punkDoc);
		if(sptrContext == 0)
			return;	

		LONG_PTR lPos = 0;
		sptrContext->GetFirstObjectPos(_bstr_t(szArgumentTypeImage), &lPos);
		while(lPos)
		{
			IUnknown* punkObject = 0;
			sptrContext->GetNextObject(_bstr_t(szArgumentTypeImage), &lPos, &punkObject);
			if(punkObject)
			{
				sptrINamedDataObject2	sptrN(punkObject);
				punkObject->Release();
				if(sptrN != 0)
				{
					IUnknown* punkParent = 0;
					sptrN->GetParent(&punkParent);
					if(punkParent)
						punkParent->Release();
					else
						dlg.m_arrImages.Add(::GetObjectName(sptrN));
				}
				
			}
		}
	}
	if(dlg.m_arrImages.GetSize() > 0 && dlg.DoModal() == IDOK)
	{
		long nATCount = dlg.m_arrImages.GetSize();
		
		for(int i = 0; i < nATCount; i++)
		{
			IUnknown* punkBaseImage = GetObjectByName(punkDoc, dlg.m_arrImages[i], 0);
			ASSERT(punkBaseImage);
			ATinfo* pATinfo = 0;

			
			if(m_mapImageToATinfo.Lookup(GetObjectKey(punkBaseImage), pATinfo) )
				continue;

			IUnknown	*punkObjects = ::CreateTypedObject(szTypeAuditTrailObject);

			pATinfo = new ATinfo;
			pATinfo->sptrImage = punkBaseImage;
			pATinfo->sptrDoc = punkDoc;
			pATinfo->sptrObject = punkObjects;

			
			if(punkObjects)
			{
				SetBaseObject(punkObjects, punkBaseImage, punkDoc);
				INamedObject2Ptr	sptrNO(punkObjects);
				if(sptrNO != 0)
				{
					CString str;
					str.LoadString( IDS_AUDIT_FOR );
					str += GetObjectName(punkBaseImage);
					_MakeUniqueName(punkDoc, str);
					sptrNO->SetName(_bstr_t(str));
				}

				m_changes.SetToDocData(punkDoc, punkObjects);

				punkObjects->Release();
			}

			m_mapImageToATinfo.SetAt(GetObjectKey(punkBaseImage), pATinfo);

			IUnknown* punkInfo = 0;
			IUnknown* punkAM = 0;
			IApplicationPtr sptrApp(GetAppUnknown());
			sptrApp->GetActionManager(&punkAM);
			IActionManagerPtr	sptrAM(punkAM);
			if(punkAM)
				punkAM->Release();

			if(sptrAM != 0)
			{
				sptrAM->GetActionInfo(_bstr_t(strAction), &punkInfo);

				if(punkInfo)
				{
					CreateThumbnailAT(pATinfo, punkInfo, strParams, true);
					punkInfo->Release();
				}

			}
		
			if(punkBaseImage)
				punkBaseImage->Release();
		}
	}
	if(bReleaseDoc)
		punkDoc->Release();

}

void CAuditTrailMan::_CleanUp(IUnknown* punkDoc)
{
	ATinfo* pATinfo = 0;
	
	GuidKey giud;
	POSITION pos = m_mapImageToATinfo.GetStartPosition();	
	while(pos)
	{
		m_mapImageToATinfo.GetNextAssoc(pos, giud, pATinfo);
		if(!punkDoc || ::GetObjectKey(pATinfo->sptrDoc) == ::GetObjectKey(punkDoc))
		{
			if(pATinfo)
			{
				delete pATinfo;
				pATinfo = 0;
			}
			m_mapImageToATinfo.RemoveKey(giud);
		}
	}

	IActionPtr	ptrA = m_sptrLastAction;
	if( ptrA != 0 )
	{
		//[AY] later we have to remove m_sptrLastAction pointer and execute "Turn On Audit Trail" action
		//!!!it is not good idea, but elsewere system will trap on shutdown
		m_sptrLastAction = 0;
		/*IUnknown	*punkTarget = 0;
		
		ptrA->GetTarget( &punkTarget );

		if( punkTarget )
		{
			GuidKey	guid = GetObjectKey( punkTarget );
			punkTarget->Release();
			if( guid == ::GetObjectKey(punkDoc) )
				m_sptrLastAction = 0;
		}*/

		
	}
			
}

void CAuditTrailMan::_MakeUniqueName(IUnknown* punkDoc, CString& strName)
{
	IDataContext2Ptr	sptrContext(punkDoc);
	if(sptrContext == 0)
		return;

	CStringArray strArray;
	bool bPresentSameName = false;
	LONG_PTR lPos = 0;
	sptrContext->GetFirstObjectPos(_bstr_t(szTypeAuditTrailObject), &lPos);
	while(lPos)
	{
		IUnknown* punkObject = 0;
		sptrContext->GetNextObject(_bstr_t(szTypeAuditTrailObject), &lPos, &punkObject);
		if(punkObject)
		{
			CString str = ::GetObjectName(punkObject);
			if(str == strName)
				bPresentSameName = true;
			else
				strArray.Add(str);
			punkObject->Release();
		}
	}
	if(bPresentSameName)
	{
		long nCount = 0;
		long nSize = strArray.GetSize();
		while(nCount <= nSize)
		{
			nCount++;
			CString strCurName;
			strCurName.Format("%s_%d", strName, nCount);
			bool bFound = false;
			long nCurSize = strArray.GetSize();
			for(int i = 0; i < nCurSize; i++)
			{
				if(strCurName == strArray[i])
				{
					strArray.RemoveAt(i);
					bFound = true;
					break;
				}
			}

			if(!bFound)
			{
				strName = strCurName;
				break;
			}
		}
	}
}

void CAuditTrailMan::CreateThumbnailAT(IUnknown* punkImage, IUnknown* punkAction, CString strParams)
{
	ATinfo* pATinfo = 0;
	if(m_mapImageToATinfo.Lookup(GetObjectKey(punkImage), pATinfo) == TRUE)
	{
		CreateThumbnailAT(pATinfo, punkAction, strParams);
	}
	else
	{
		//ASSERT(FALSE);
	}
}

void CAuditTrailMan::CreateThumbnailAT(ATinfo* pATinfo, IUnknown* punkAction, CString strParams, bool bRender)
{
	if(!CheckAuditAlive(pATinfo))
		return;

	IAuditTrailObjectPtr sptrATobj(pATinfo->sptrObject);
	if(sptrATobj == 0)
		return;

	sptrATobj->CreateThumbnail(pATinfo->sptrDoc, pATinfo->sptrImage, punkAction, _bstr_t(strParams), bRender);
}

IUnknown* CAuditTrailMan::GetATbyImage(IUnknown* punkImage)
{
	ATinfo* pATinfo = 0;
	IUnknown* punkAT = 0;
	if(m_mapImageToATinfo.Lookup(GetObjectKey(punkImage), pATinfo) == TRUE)
	{
		if(CheckAuditAlive(pATinfo))
			punkAT = pATinfo->sptrObject;
	}

	if(punkAT)
		punkAT->AddRef();

	return punkAT;
}

CString CAuditTrailMan::GetActionName(IUnknown* punkAction)
{
	CString strName;
	IActionPtr sptrAction(punkAction);
	if(sptrAction != 0)
	{
		IUnknown* punkAI = 0;
		sptrAction->GetActionInfo(&punkAI);
		IActionInfoPtr sptrAI(punkAI);
		if(punkAI)
			punkAI->Release();
		BSTR	bstrActionName = 0;
		sptrAI->GetCommandInfo( &bstrActionName, 0, 0, 0); 
		strName = bstrActionName;
		SysFreeString(bstrActionName);
	}
	return strName;
}

bool CAuditTrailMan::IsNeedToCreateThumbnailAT(IUnknown* punkAction)
{
	bool b = false;
	CString strAction = GetActionName(punkAction);
	if(strAction == "Undo")
		return false;

	m_sptrLastAction = 0;

	if((m_mapActionsToProcess.Lookup(strAction, b) == TRUE))
		m_sptrLastAction = punkAction;

	return (m_sptrLastAction != 0 && m_bOnAT);
}


void CAuditTrailMan::ProcessAction(IUnknown* punkHit)
{
	if(!punkHit)
		return;

	IApplicationPtr sptrApp(GetAppUnknown());
	IUnknown* pActiveDoc = 0;
	IUnknown* punkActiveImage = 0;
	sptrApp->GetActiveDocument(&pActiveDoc);
	bool bReleaseAction = false;

	CString	strActionName = GetActionName(punkHit);
	if(pActiveDoc)
	{
		punkActiveImage = ::GetActiveObjectFromDocument(pActiveDoc, szTypeImage);
		if( strActionName == "Redo")
		{
			IUndoListPtr	ptrUndo(pActiveDoc);
			ptrUndo->GetLastUndoAction(&punkHit);
			bReleaseAction = true;
		}
		pActiveDoc->Release();
	}
	
	IFilterActionPtr sptrFilterAction(punkHit);
	if(sptrFilterAction != 0)
	{

		//change target image
		TPOS nPos = 0;
		sptrFilterAction->GetFirstArgumentPosition(&nPos);
		IUnknown* punkDataObject = 0;
		bool bFoundInAT = false;
		IUnknown* punkInImage = 0;
		while(nPos)
		{
			BSTR bstrArgType = 0;
			BOOL bOut = FALSE;
			sptrFilterAction->GetArgumentInfo(nPos, &bstrArgType, 0, &bOut);
			sptrFilterAction->GetNextArgument(&punkDataObject, &nPos);
			CString strType(bstrArgType);
			::SysFreeString(bstrArgType);

			if(bOut == FALSE && strType == szTypeImage)
			{
				if(punkDataObject)
				{
					sptrINamedDataObject2	sptrN(punkDataObject);
					if(sptrN != 0)
					{
						IUnknown* punkP = 0;
						sptrN->GetParent(&punkP);
						if(punkP)
						{
							punkDataObject->Release();
							punkDataObject = punkP;
						}
					}
					ATinfo* pATinfo = 0;
					GuidKey key = GetObjectKey(punkDataObject);
					if(m_mapImageToATinfo.Lookup(key, pATinfo) == TRUE)
					{
						if(bFoundInAT)
						{
							//delete this associate for audit trail
							if(pATinfo)
							{
								delete pATinfo;
								pATinfo = 0;
							}
							m_mapImageToATinfo.RemoveKey(key);
						}
						else
						{
							punkInImage = punkDataObject;
							punkInImage->AddRef();
							bFoundInAT = true;
						}
					}
				}
			}
			if(punkDataObject)
				punkDataObject->Release();
			punkDataObject = 0;
		}

		if(punkInImage)
		{
			sptrFilterAction->GetFirstArgumentPosition(&nPos);

			while(nPos)
			{
				BSTR bstrArgType = 0;
				BOOL bOut = FALSE;
				sptrFilterAction->GetArgumentInfo(nPos, &bstrArgType, 0, &bOut);
				sptrFilterAction->GetNextArgument(&punkDataObject, &nPos);
				CString strType(bstrArgType);
				::SysFreeString(bstrArgType);

				if(bOut == TRUE && strType == szTypeImage)
				{
					if(punkDataObject)
					{
						sptrINamedDataObject2	sptrN(punkDataObject);
						if(sptrN != 0)
						{
							IUnknown* punkP = 0;
							sptrN->GetParent(&punkP);
							if(punkP)
							{
								punkDataObject->Release();
								punkDataObject = punkP;
							}
						}
						ATinfo* pATinfo = 0;
						GuidKey key = GetObjectKey(punkInImage);
						VERIFY(m_mapImageToATinfo.Lookup(key, pATinfo) == TRUE);
						pATinfo->sptrImage = punkDataObject;
						m_mapImageToATinfo.RemoveKey(key);
						m_mapImageToATinfo.SetAt(GetObjectKey(punkDataObject), pATinfo);

						if(punkActiveImage)
							punkActiveImage->Release();
						punkActiveImage = punkDataObject;
						//punkActiveImage->AddRef();
						break;
					}
				}
				if(punkDataObject)
					punkDataObject->Release();
				punkDataObject = 0;
			}
			punkInImage->Release();
		}
	}		
	
	if(punkActiveImage)
	{
		
		IUnknown	*punkParent = 0;
		sptrINamedDataObject2	sptrN(punkActiveImage);
		if(sptrN != 0)
		{
			sptrN->GetParent(&punkParent);
			if(punkParent)
			{
				if(punkActiveImage)
				{
					punkActiveImage->Release();
				}
				punkActiveImage = punkParent;
			}
		}

		
		bool bRedo = GetActionName(punkHit) == "Redo";
		
		if((IsNeedToCreateThumbnailAT(punkHit) || bRedo) && punkActiveImage)
		{
			CreateThumbnailAT(punkActiveImage, punkHit, _GetActionParams(punkHit));

			m_sptrLastAction = 0;
		}
			
		punkActiveImage->Release();
	}

	if(bReleaseAction && punkHit)
		punkHit->Release();

}

CString CAuditTrailMan::_GetActionParams(IUnknown* punkAction)
{
	CString strParams;
	IActionPtr sptrA(punkAction);
	if(sptrA != 0)
	{
		BSTR bstr = 0;
		sptrA->StoreParameters(&bstr);
		strParams = bstr;
		::SysFreeString(bstr);
	}

	return strParams;
}

GuidKey CAuditTrailMan::GetObjectKey(IUnknown* punkObject)
{
	GuidKey key;

	INamedDataObject2Ptr sptrNDO(punkObject);
	if(sptrNDO != 0)
	{
		sptrNDO->GetBaseKey(&key);
	}
	
	return key;
}

void CAuditTrailMan::_FillActionList()
{
	m_mapActionsToProcess.RemoveAll();
	long nActionsCount = 0;
	while(true)
	{
		CString strParam;
		strParam.Format("%s%d", "Action_", nActionsCount);
		CString strAction = ::GetValueString(GetAppUnknown(), "AuditTrail", strParam, "");
		if(strAction.IsEmpty())
			break;

		m_mapActionsToProcess.SetAt(strAction, true);

		nActionsCount++;
	}
}

bool CAuditTrailMan::CheckAuditAlive(ATinfo* pATinfo)
{
	if(!pATinfo)
		return false;

	IDataContext2Ptr sptrContext(pATinfo->sptrDoc);
	if(sptrContext != 0)
	{
		LONG_PTR lPos = 0;
		sptrContext->GetFirstObjectPos(_bstr_t(szTypeAuditTrailObject), &lPos);
		while(lPos)
		{
			IUnknown* punkObject = 0;
			sptrContext->GetNextObject(_bstr_t(szTypeAuditTrailObject), &lPos, &punkObject);
			if(punkObject)
			{
				if(::GetObjectKey(punkObject) == ::GetObjectKey(pATinfo->sptrObject))
				{
					punkObject->Release();
					return true;
				}
				punkObject->Release();
			}
		}
	}

	//kill AT
	m_mapImageToATinfo.RemoveKey(GetObjectKey(pATinfo->sptrImage));
	delete pATinfo;
	pATinfo = 0;
			
	return false;
}

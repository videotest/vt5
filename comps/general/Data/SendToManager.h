#if !defined __sendtomanager_h__
#define __sendtomanager_h__

#include "SendTo.h"

/*
enum CreateType
{
	ctCreateAll = 0,
	ctFirstSelected = 1,
	ct

}
*/





/////////////////////////////////////////////////////////////////////////////
// CSendToManager class
class CSendToManager : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CSendToManager);
	GUARD_DECLARE_OLECREATE(CSendToManager);
	DECLARE_INTERFACE_MAP();
protected:
	CSendToManager();
	virtual ~CSendToManager();

protected:
	
	BEGIN_INTERFACE_PART(SendToManager, ISendToManager)
		//Input
		com_call SetSourceDoc( IUnknown* punkDoc, UINT uiStartItemID );
		com_call SetSingleObjectName( BSTR bstrObjectName );
		com_call AddObject( IUnknown* punkObject );
		com_call CreateTargets( );
		//Output
		com_call GetFirstItemPos( long* plPos );
		com_call GetNextItem(	
							UINT* puiFlags, 
							UINT* puiItemID, 
							BSTR* pbstrText,			
							UINT* puiParentID,
							long* plPos
							);
		//Event
		com_call OnCommand( UINT uiCommand );

		//com_call GetSourceDocGuid( GUID* pDocGuid );
		//com_call GetFirstObjectPos( long* plPos );
		//com_call GetNextObjectPos( GUID* pObjGuid, long* plPos );

		//Info
		//com_call GetItemInfo( UINT uiItemID, CItemInfo** ppii );

	END_INTERFACE_PART(SendToManager)

protected:

	CString m_strSingleObjectName;
	//bool		m_bContentSync;
	//Input params
	UINT		m_uiStartItemID;
	GuidKey		m_guidSourceDoc;	
	CPtrList	m_arGuidSourceObject;//GuidKey*

	//Output
	CPtrList	m_arGuidTargetDocs;
	CPtrList	m_arItems;

	void Init();
	void DeInit();

	CString		m_strTitle;
	UINT		m_uiItemID;
	UINT		m_uiParentID;
	UINT		m_uiFlags;	
	ItemInfo	m_itemInfo;
	GuidKey		m_guidDoc;
	GuidKey		m_guidObj;


	CItemInfo* AddItem( CString	strTitle, CString strTable, CString strField, 
						UINT uiFlags, ItemInfo itemInfo, UINT uiItemID, UINT uiParentID, 
						GuidKey guidDoc, GuidKey guidObj );

	void AddObjectList( UINT uiParent, UINT& uiCommand, GuidKey guidDocKey );
//	void CheckItemsForSingleName();
	void CheckAvailableItems();
	void MakeBaseObjectsArray(IUnknown *punkSrcDoc, CStringArray &sa);
	bool HasChildren(CItemInfo *p);
//	bool CheckForDBDoc(IUnknown *punkSrcDoc);
	void DisableAll();
public:
	//some helper
	static IUnknownPtr GetDocumentByKey( GuidKey guidDoc );
	static IUnknownPtr GetObject( GuidKey guidDoc, GuidKey guidObject );	

protected:
	void GetFieldsFromDBaseDoc( IUnknown* punkDoc, CStringArray& arTable, CStringArray& arField, CStringArray& arCaption );
	
	CString GetObjectNameFromPath( CString strPath );

	CItemInfo* FindByID( UINT uiItemID );

};

bool GetDBActiveField(IUnknown *punkSrcDoc, short &nFieldType, CString &sVTObjType);


#endif//__sendtomanager_h__

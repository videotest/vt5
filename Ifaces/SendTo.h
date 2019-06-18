#ifndef __sendto_h__
#define __sendto_h__

enum ItemInfo
{
	iiDocTitle				= 0,
	iiDBFieldTitle			= 1,
	iiDBPutToDocument		= 2,
	iiSeparator				= 3,
	iiAllObject				= 4,
	iiSingleObject			= 5,	
	iiNewDocument			= 6,
	iiUndefine				= 7,
};

class CItemInfo
{
public:
	CItemInfo()
	{
		m_uiItemID		= -1;		
		m_uiParentID	= -1;
		m_uiFlags		= -1;
		m_itemInfo		= iiUndefine;
		m_guidDoc		= INVALID_KEY;
		m_guidDoc		= INVALID_KEY;
	}	
	CString		m_strTitle;
	UINT		m_uiItemID;
	UINT		m_uiParentID;
	UINT		m_uiFlags;	
	ItemInfo	m_itemInfo;
	GuidKey		m_guidDoc;
	GuidKey		m_guidObj;
	CString		m_strTable;
	CString		m_strField;


};


interface ISendToManager : public IUnknown
{	
	//Input
	com_call SetSourceDoc( IUnknown* punkDoc, UINT uiStartItemID ) = 0;		
	com_call SetSingleObjectName( BSTR bstrObjectName ) = 0;		
	com_call AddObject( IUnknown* punkObject ) = 0;		
	com_call CreateTargets( ) = 0;		
	//Output
	com_call GetFirstItemPos( long* plPos ) = 0;
	com_call GetNextItem(	
			UINT* puiFlags, 
			UINT* puiItemID, 
			BSTR* pbstrText,			
			UINT* puiParentID,
			long* plPos ) = 0;	
	//Event
	com_call OnCommand( UINT uiCommand ) = 0;

	//Output
	//com_call GetSourceDocGuid( GUID* pDocGuid ) = 0;
	//com_call GetFirstObjectPos( long* plPos ) = 0;
	//com_call GetNextObjectPos( GUID* pObjGuid, long* plPos ) = 0;

	//Info
	//com_call GetItemInfo( UINT uiItemID, CItemInfo** ppii ) = 0;
};


declare_interface( ISendToManager, "1BA35E9F-5164-4ee4-9B0E-3EFEB7DE65B3");



////////////////////////////////////////////////////////////////
//4 Registry
#define szSendToManagerProgID_4Reg "Data.SendToManager"
#define szSendToManagerProgID szSendToManagerProgID_4Reg

#endif __sendto_h__

#ifndef __oleitem_actions_h__
#define __oleitem_actions_h__

#include "resource.h"
#include "constant.h"
#include "OleData.h"

#include "docobj.h"

#include "axdoccontainer.h"

#include "idispatchimpl.h"
#ifdef _DEBUG
	#import "\vt5\vt5\debug\comps\OleClient.tlb" no_namespace raw_interfaces_only named_guids 
#else
	#import "\vt5\vt5\release\comps\OleClient.tlb" no_namespace raw_interfaces_only named_guids 
#endif


//////////////////////////////////////////////////////////////////////
//
//	class COleItemData
//
//
class COleItemData : public CObjectBase,
				public IOleObjectData,
				public IOleObjectUI,				
				 public IDispatchImpl< IOleItemDataDisp >,
				public _dyncreate_t<COleItemData>
{
	friend class CAxDocContainer;

	route_unknown();

public:

	com_call IsModified( BOOL *pbModified );

	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return clsidOleItemData; }

	virtual IUnknown *DoGetInterface( const IID &iid )
	{
		if( iid == IID_IOleObjectData )return (IOleObjectData*)this;
		else if( iid == IID_IOleObjectUI )return (IOleObjectUI*)this;
		else if( iid == IID_IDispatch )return (IDispatch*)this;
		else return CObjectBase::DoGetInterface( iid );
	}

protected:
	void Free();
	void InitDefaults();

public:	
	com_call AttachData( IUnknown *punkNamedData );	//called from NamedData
protected:
	IUnknownPtr m_ptrDocument;

public:
	COleItemData();
	virtual ~COleItemData();	

	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );
	//INamedDataObject2
	com_call IsBaseObject(BOOL * pbFlag);


	//IOleObjectData
	com_call CreateItem( OLEUIINSERTOBJECT* pio );
	com_call CreateFromClipboard( );	
	com_call GetItemInfo( BSTR* bstrFileName, short* pCreationType, BSTR* bstrAppProgID );

	//IOleObjectUI
	com_call Activate( HWND hWndView );
	com_call Deactivate( HWND hWndView );
	com_call ActivateUI( HWND hWndView );
	com_call DeactivateUI( HWND hWndView );
	com_call OnSize( int cx, int cy );

	com_call IsObjectCreated( BOOL* pbCreated );
	com_call CanProcessDrawig( HWND hWnd, BOOL* pbProcess );

	com_call GetInPlaceEditHwnd( HWND* phWnd );
	com_call GetExtent( SIZEL* psizel );

	com_call GetState( short* pnState );
	
	com_call GetScrollSize( SIZE* pSize, HWND hWnd  );

	//call by ShellFrame
	com_call OnFrameResizeBorder( RECT rcBorder );
	com_call OnFrameActivate( BOOL bActivate );

	//Run verb
	com_call DoVerb( UINT uiVerb );


	com_call DrawMeta( int nDrawAspect, HWND hWnd, HDC hdc, RECT rcTarget, POINT ptOffset, SIZE sizeClip );	

	com_call ForceUpdate();	
	
	com_call OnOwnerActivate();


	com_call IsEmpty( /*[out, retval]*/ VARIANT_BOOL *plPos );

protected:
	//Item data
	_bstr_t			m_bstrFileName;
	_bstr_t			m_bstrAppName;
	OleCreationType	m_creationType;
	GUID			m_appGuid;


	//Item state
	BOOL			m_bEmpty;	
	BOOL			m_bSucceededReadStorage;
	BOOL			m_bSucceededCreateServer;

	

	HWND			m_hwndView;

protected:

	CAxDocContainer*	m_pContainer;

	bool			FireEventObjectWasChange();
	
};

class COleItemDataInfo : public CDataInfoBase,
					public _dyncreate_t<COleItemDataInfo>
{
public:
	COleItemDataInfo()	: CDataInfoBase( clsidOleItemData, szTypeOleItem, 0, IDI_OLEITEMIMAGE )
	{	}

	com_call GetUserName( BSTR *pbstr );
};



#endif //__oleitem_actions_h__

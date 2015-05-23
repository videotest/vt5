#pragma once

#include "idiodbase_int.h"
#include <vector>
class CIdioDBaseDataObject : public CDataObjectListBase
{
	std::vector<IUnknownPtr> m_vIdio;
	std::vector<CString> m_vIdioText;

	PROVIDE_GUID_INFO()

	DECLARE_DYNCREATE(CIdioDBaseDataObject)
	GUARD_DECLARE_OLECREATE(CIdioDBaseDataObject)

	CIdioDBaseDataObject();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptDataObject)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CIdioDBaseDataObject();

	//{{AFX_MSG(CIdioDBaseDataObject)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	//{{AFX_DISPATCH(CIdioDBaseDataObject)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
/*
	BEGIN_INTERFACE_PART( DataObject, IIdioDBase )
		com_call AddItem( IUnknown *punkIdio );
		com_call GetItem( long lID, IUnknown **punkIdio );
		com_call AddItemAfter( IUnknown *punkIdio, long lID );
		
		com_call SetItemText( long lID, BSTR bstr );
		com_call GetItemText( long lID, BSTR *bstr );

		com_call GetCount( long *lCount );
		com_call Remove( long lID );
		com_call RemoveAll();
	END_INTERFACE_PART( DataObject );

	BEGIN_INTERFACE_PART( DataStorage, IIdioDBaseStorage )
		com_call ReadFile( BSTR bstr );
		com_call WriteFile( BSTR bstr );
	END_INTERFACE_PART( DataStorage );
*/
protected:
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );

	virtual bool IsModified();
	virtual void SetModifiedFlag( bool bSet = true );

	virtual bool CanBeBaseObject();
public:
	static char	*m_szType;
};
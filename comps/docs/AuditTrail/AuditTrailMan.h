#if !defined(AFX_AUDITTRAILMAN_H__DE176256_0E6D_4EAE_8824_CB389A7A20B4__INCLUDED_)
#define AFX_AUDITTRAILMAN_H__DE176256_0E6D_4EAE_8824_CB389A7A20B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuditTrailMan.h : header file
//

#include <AfxTempl.h>
#include "auditint.h"

struct ATinfo
{
	IUnknownPtr				sptrDoc;
	IImagePtr				sptrImage;
	INamedDataObject2Ptr	sptrObject;
};


typedef CMap <GuidKey, GuidKey&, ATinfo*, ATinfo*> CATMap; //map image object key to ATinfo struct
template<>UINT AFXAPI HashKey(GuidKey&key);


/////////////////////////////////////////////////////////////////////////////
// CAuditTrailMan command target

class CAuditTrailMan : public CCmdTargetEx,
					   public CEventListenerImpl
{
	ENABLE_MULTYINTERFACE()
	DECLARE_DYNCREATE(CAuditTrailMan)
	GUARD_DECLARE_OLECREATE(CAuditTrailMan)
	DECLARE_INTERFACE_MAP()
	
	CAuditTrailMan();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuditTrailMan)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL


	BEGIN_INTERFACE_PART(ATman, IAuditTrailMan)
		com_call ToggleOnOffFlag();
		com_call GetCreateFlag(BOOL* pbWillCreate);
		com_call GetAuditTrail(IUnknown* punkImage, IUnknown** ppunkAT);
		com_call RescanActionsList();
	END_INTERFACE_PART(ATman)

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	virtual BOOL OnCreateAggregates();

	IUnknown* GetATbyImage(IUnknown* punkImage);

	void _FillActionList();

	bool CheckAuditAlive(ATinfo* pATinfo);

// Implementation
protected:
	virtual ~CAuditTrailMan();

	// Generated message map functions
	//{{AFX_MSG(CAuditTrailMan)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	GuidKey GetObjectKey(IUnknown* punkObject);

	void CreateAT(IUnknown* punkDoc, CString strAction, CString strParams);
	void _CleanUp(IUnknown* punkDoc = 0);
	void _MakeUniqueName(IUnknown* punkDoc, CString& strName);
	void CreateThumbnailAT(ATinfo* pATinfo, IUnknown* punkAction, CString strParams, bool bRender = false);
	void CreateThumbnailAT(IUnknown* punkImage, IUnknown* punkAction, CString strParams);
	bool IsNeedToCreateThumbnailAT(IUnknown* punkAction);
	CString GetActionName(IUnknown* punkAction);
	void ProcessAction(IUnknown* punkHit);
	CString _GetActionParams(IUnknown* punkAction);

	IUnknownPtr	m_sptrLastAction;


private:
	bool	m_bInitializedEL;
	bool	m_bOnAT;
	CObjectListUndoRecord m_changes;
	CATMap m_mapImageToATinfo; 
	CMap<CString, LPCSTR, bool, bool> m_mapActionsToProcess;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AuditTrailMan_H__DE176256_0E6D_4EAE_8824_CB389A7A20B4__INCLUDED_)

#if !defined(AFX_OLEEVENTPROVIDER_H__AC919313_2E3D_4B4D_BA65_64F1F9E60CFA__INCLUDED_)
#define AFX_OLEEVENTPROVIDER_H__AC919313_2E3D_4B4D_BA65_64F1F9E60CFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OleEventProvider.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COleEventProvider window
#include "CmdTargEx.h"

class COleEventProvider : public CCmdTargetEx,
						  public CNamedObjectImpl,
						  public COleEventSourceImpl
{
	ENABLE_MULTYINTERFACE();

	DECLARE_DYNCREATE(COleEventProvider)
	GUARD_DECLARE_OLECREATE(COleEventProvider)
	DECLARE_OLETYPELIB(COleEventProvider)

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_EVENT_MAP()

// Construction
public:
	COleEventProvider();
	virtual ~COleEventProvider();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COleEventProvider)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(COleEventProvider)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG


	BEGIN_INTERFACE_PART(MeasEvent, IMeasureEventProvider)
		com_call FireEventBeforeMeasure(BSTR bstrParamType);
		com_call FireEventAfterMeasure(BSTR bstrParamType, double fVal);
	END_INTERFACE_PART(MeasEvent)


	//{{AFX_EVENT(COleEventProvider)
	void FireAfterManualMeasure(LPCTSTR szParamType, double fValue)
		{FireEvent(eventidAfterManualMeasure,EVENT_PARAM(VTS_BSTR  VTS_R8), szParamType, fValue);}
	void FireBeforeManualMeasure(LPCTSTR szParamType)
		{FireEvent(eventidBeforeManualMeasure,EVENT_PARAM(VTS_BSTR), szParamType);}
	//}}AFX_EVENT
	enum {
	//{{AFX_DISP_ID(COleEventProvider)
	eventidAfterManualMeasure = 1L,
	eventidBeforeManualMeasure = 2L,
	//}}AFX_DISP_ID
	};
	
	ROUTE_OLE_VIRTUALS()

};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OLEEVENTPROVIDER_H__AC919313_2E3D_4B4D_BA65_64F1F9E60CFA__INCLUDED_)

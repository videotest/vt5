// OleEventProvider.cpp : implementation file
//

#include "stdafx.h"
#include "MeasManual.h"
#include "OleEventProvider.h"

/////////////////////////////////////////////////////////////////////////////
// COleEventProvider

IMPLEMENT_DYNCREATE(COleEventProvider, CCmdTargetEx)
IMPLEMENT_OLETYPELIB(COleEventProvider, IID_ITypeLibID, 1, 0)


// {43B1743F-B5C4-4c68-B5BA-D21E9F2A6D00}
GUARD_IMPLEMENT_OLECREATE(COleEventProvider, "MeasManual.ManualMeasureEventProvider", 
0x43b1743f, 0xb5c4, 0x4c68, 0xb5, 0xba, 0xd2, 0x1e, 0x9f, 0x2a, 0x6d, 0x0);
// {4010031B-2B07-4721-84D8-584D7656F3DF}
static const IID IID_IOleEventProviderDisp =
{ 0x4010031b, 0x2b07, 0x4721, { 0x84, 0xd8, 0x58, 0x4d, 0x76, 0x56, 0xf3, 0xdf } };
// {FB940961-F89A-4807-AC94-002AC088DC48}
static const GUID IID_IOleEventProviderEvents = 
{ 0xfb940961, 0xf89a, 0x4807, { 0xac, 0x94, 0x0, 0x2a, 0xc0, 0x88, 0xdc, 0x48 } };


BEGIN_MESSAGE_MAP(COleEventProvider, CCmdTargetEx)
	//{{AFX_MSG_MAP(COleEventProvider)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COleEventProvider, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(COleEventProvider)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IOleEventProviderDisp to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.


BEGIN_INTERFACE_MAP(COleEventProvider, CCmdTargetEx)
	INTERFACE_PART(COleEventProvider, IID_IOleEventProviderDisp,		Dispatch)
	INTERFACE_PART(COleEventProvider, IID_IMeasureEventProvider,		MeasEvent)
	INTERFACE_PART(COleEventProvider, IID_INamedObject2,				Name)
	INTERFACE_PART(COleEventProvider, IID_IConnectionPointContainer,	ConnPtContainer)
	INTERFACE_PART(COleEventProvider, IID_IProvideClassInfo,			ProvideClassInfo)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(COleEventProvider, MeasEvent);

BEGIN_EVENT_MAP(COleEventProvider, COleEventSourceImpl)
	//{{AFX_EVENT_MAP(COleEventProvider)
	EVENT_CUSTOM("AfterManualMeasure", FireAfterManualMeasure, VTS_BSTR  VTS_R8)
	EVENT_CUSTOM("BeforeManualMeasure", FireBeforeManualMeasure, VTS_BSTR)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()



COleEventProvider::COleEventProvider()
{
	m_piidEvents = &IID_IOleEventProviderEvents;
	m_piidPrimary = &IID_IOleEventProviderDisp;


	EnableAutomation();
	EnableAggregation();
	EnableConnections();

	m_sName = _T("ManualMeasure");
}

COleEventProvider::~COleEventProvider()
{
}

void COleEventProvider::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTargetEx::OnFinalRelease();
}



/////////////////////////////////////////////////////////////////////////////
// COleEventProvider message handlers

HRESULT COleEventProvider::XMeasEvent::FireEventBeforeMeasure(BSTR bstrParamType)
{
	METHOD_PROLOGUE_EX(COleEventProvider, MeasEvent)
	{
		CString str = bstrParamType;
		pThis->FireBeforeManualMeasure(str);
		return S_OK;
	}
}

HRESULT COleEventProvider::XMeasEvent::FireEventAfterMeasure(BSTR bstrParamType, double fVal)
{
	METHOD_PROLOGUE_EX(COleEventProvider, MeasEvent)
	{
		CString str = bstrParamType;
		pThis->FireAfterManualMeasure(str, fVal);
		return S_OK;
	}
}




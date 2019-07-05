// NotifyContr.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
//#include "common.h"
#include "NotifyContr.h"
#include "Utils.h"
#include "Factory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CNotifyController : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CNotifyController);
	GUARD_DECLARE_OLECREATE(CNotifyController);
	DECLARE_INTERFACE_MAP();
protected:
	CNotifyController();
	virtual ~CNotifyController();
protected:
	BEGIN_INTERFACE_PART(Contr, INotifyController2)
		com_call FireEvent( BSTR szEventDesc, IUnknown *pHint, IUnknown *pFrom, void *pdata, long cbSize  );
		com_call RegisterEventListener( BSTR szEventDesc, IUnknown *pListener );
		com_call UnRegisterEventListener( BSTR szEventDesc, IUnknown *pListener );
		com_call IsRegisteredEventListener( BSTR szEventDesc, IUnknown *pListener, long* pbFlag);

		com_call AddSource(IUnknown * punkSrc);
		com_call RemoveSource(IUnknown * punkSrc);
	END_INTERFACE_PART(Contr)

	CTypedPtrList<CPtrList,IEventListener*>	m_list;
	CTypedPtrList<CPtrList,IEventListener*>	m_listSrc;
};

IMPLEMENT_DYNCREATE(CNotifyController, CCmdTargetEx);


// {2B5929A4-1E6F-11d3-A5D1-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CNotifyController, "NotifyContr.NotifyController",
0x2b5929a4, 0x1e6f, 0x11d3, 0xa5, 0xd1, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);


CNotifyController::CNotifyController()
{
	_OleLockApp( this );
	EnableAggregation();
	m_listSrc.RemoveAll();
}

CNotifyController::~CNotifyController()
{
	m_listSrc.RemoveAll();
	for(POSITION pos=m_list.GetHeadPosition(); !!pos;)
	{
		try{
			IEventListener* pEL=m_list.GetNext(pos);
			pEL->Release();
		}
		catch(...)
		{
			ASSERT(!"Access Violation");
		}
	}
	_OleUnlockApp( this );
}



BEGIN_INTERFACE_MAP(CNotifyController, CCmdTargetEx)
	INTERFACE_PART(CNotifyController, IID_INotifyController2, Contr)
	INTERFACE_PART(CNotifyController, IID_INotifyController, Contr)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CNotifyController, Contr);

HRESULT CNotifyController::XContr::FireEvent( BSTR szEventDesc, IUnknown *pHint, IUnknown *pFrom, void *pdata, long cbSize  )
{
	static long s_lCallLevel = 0;
	_try_nested(CNotifyController, Contr, FireEvent)
	{
		s_lCallLevel++;

		POSITION	pos = pThis->m_list.GetHeadPosition();

		while( pos )
		{
			POSITION	posOld = pos;
			IEventListener*	sp=pThis->m_list.GetNext(pos);

			//if( !punk && s_lCallLevel == 1 )
			//{
			//	pThis->m_list.RemoveAt( posOld );
			//	punk->Release();
			//	continue;
			//}

			// skip listener if it's in listSrc
			if (pThis->m_listSrc.Find(sp));
			{
				continue;
			}

			if( sp )sp->Notify( szEventDesc, pHint, pFrom, pdata, cbSize );

		}

		s_lCallLevel--;

		return S_OK;
	}
	_catch_nested;
	s_lCallLevel--;
}

void testUnkListener(IUnknown* pUnkListener)
{
		IEventListenerPtr pListener(pUnkListener);
		IUnknownPtr pUnk(pListener);
		ASSERT(pUnkListener==(IUnknown*)pUnk);
}
HRESULT CNotifyController::XContr::RegisterEventListener( BSTR szEventDesc, IUnknown *pUnkListener )
{
	_try_nested(CNotifyController, Contr, RegisterEventListener)
	{
		testUnkListener(pUnkListener);
		IEventListenerPtr pListener(pUnkListener);
		if( !pListener )
		{
			ASSERT( FALSE );
			return E_INVALIDARG;
		}

		if (pThis->m_list.Find( (IEventListener*)pListener ))
			return S_OK;
		
		pThis->m_list.AddTail( pListener );
		pUnkListener->AddRef();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CNotifyController::XContr::UnRegisterEventListener( BSTR szEventDesc, IUnknown *pUnkListener )
{
	_try_nested(CNotifyController, Contr, UnRegisterEventListener)
	{
		testUnkListener(pUnkListener);
		IEventListenerPtr pListener(pUnkListener);
		POSITION pos = pThis->m_list.Find((IEventListener*)pListener);
		if(0!=pos)
		{
			TRACE( "Remove pListener %p pThis %p \n", pListener, pThis );
			pThis->m_list.RemoveAt( pos );
			pUnkListener->Release();
		}
		else
		{
			TRACE( "Remove Not found pListener %p pThis %p \n", pListener, pThis );
			ASSERT(!"Remove Not found pListener");
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CNotifyController::XContr::IsRegisteredEventListener( BSTR szEventDesc, IUnknown *pUnkListener, long * pbFlag)
{
	_try_nested(CNotifyController, Contr, IsRegisteredEventListener)
	{
		if (!pbFlag || !pUnkListener)
			return E_INVALIDARG;

		testUnkListener(pUnkListener);

		IEventListenerPtr pListener(pUnkListener);
		*pbFlag = pThis->m_list.Find((IEventListener*)pListener) != NULL;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CNotifyController::XContr::AddSource(IUnknown* pUnkSrcListener)
{
	_try_nested(CNotifyController, Contr, AddSource)
	{
		testUnkListener(pUnkSrcListener);
		IEventListenerPtr pSrc(pUnkSrcListener);
		if (!pSrc)
			return S_OK; // nothing
		
		POSITION pos = pThis->m_listSrc.Find((IEventListener*)pSrc);
		if (pos)
			return S_OK; // already in list 

		pos = pThis->m_listSrc.AddTail(pSrc);
		return pos ? S_OK : E_FAIL;
	}
	_catch_nested;
}

HRESULT CNotifyController::XContr::RemoveSource(IUnknown * punkSrc)
{
	METHOD_PROLOGUE_EX(CNotifyController, Contr)
	{
		testUnkListener(punkSrc);
		IEventListenerPtr pSrc(punkSrc);
		POSITION pos = pThis->m_listSrc.Find((IEventListener*)pSrc);
		if (!pos)
			return E_INVALIDARG;

		pThis->m_listSrc.RemoveAt(pos);

		return S_OK;
	}
}



//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CNotifyContrApp

BEGIN_MESSAGE_MAP(CNotifyContrApp, CWinApp)
	//{{AFX_MSG_MAP(CNotifyContrApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNotifyContrApp construction

CNotifyContrApp::CNotifyContrApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CNotifyContrApp::~CNotifyContrApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNotifyContrApp object

CNotifyContrApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNotifyContrApp initialization

BOOL CNotifyContrApp::InitInstance()
{
	VERIFY(LoadVTClass(this) == true);
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleObjectFactory::RegisterAll();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NeedComponentInfo(rclsid, riid))
		return GetComponentInfo(ppv);
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();

	CCompRegistrator	rd( szPluginApplicationAggr	 ), 
						ra( szPluginDocumentsAggr ),
						rv( szPluginViewAggr );

	rd.RegisterComponent( "NotifyContr.NotifyController" );
	ra.RegisterComponent( "NotifyContr.NotifyController" );
	rv.RegisterComponent( "NotifyContr.NotifyController" );

	return S_OK;
}

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CCompRegistrator	rd( szPluginApplicationAggr ), 
						ra( szPluginDocumentsAggr ),
						rv( szPluginViewAggr );

	rd.UnRegisterComponent( "NotifyContr.NotifyController" );
	ra.UnRegisterComponent( "NotifyContr.NotifyController" );
	rv.UnRegisterComponent( "NotifyContr.NotifyController" );


	COleObjectFactory::UpdateRegistryAll( FALSE );
	return S_OK;
}


#ifndef __misc5_h__
#define __misc5_h__

#ifdef _DEBUG
#import <debug\misc.tlb> exclude ("IUnknown", "GUID", "_GUID") no_namespace raw_interfaces_only named_guids 
#else
#import <release\misc.tlb> exclude ("IUnknown", "GUID", "_GUID") no_namespace raw_interfaces_only named_guids 
#endif

/*#include "defs.h"
#include "window5.h"

//main purpose of it interface is support standard mechanizm for
//using named item's
interface INamedObject2 : public IUnknown
{
	com_call GetName( BSTR *pbstr ) = 0;
	com_call GetUserName( BSTR *pbstr ) = 0;
	com_call SetName( BSTR bstr ) = 0;
	com_call SetUserName( BSTR bstr ) = 0;
	com_call GenerateUniqueName( BSTR bstrBase ) = 0;
};


//purpose of this interface is support unique number.
interface INumeredObject : public IUnknown
{
	com_call AssignKey( GUID Key ) = 0;
	com_call GetKey( GUID *pKey ) = 0;
	com_call GenerateNewKey() = 0;
	com_call GetCreateNum( long *plNum ) = 0;
	com_call GenerateNewKey( GUID * pKey ) = 0;
};


//interface of component, who requires IUnknown from application
interface IRootedObject : public IUnknown
{
	com_call AttachParent( IUnknown *punkParent ) = 0;
	com_call GetParent( IUnknown **punkParent ) = 0;
};
//interface IStoredObject. Components, who implement this interface,
//should also support INamedObject inetrface. The standard implementation of
//this interface supposrts also INumeredObject


//compenent management
interface ICompManager : public IUnknown
{
	com_call GetCount( int *piCount ) = 0;
	com_call LoadComponents() = 0;
	com_call FreeComponents() = 0;

	com_call GetComponentUnknown( int idx, IUnknown **ppunk ) = 0;
	com_call GetComponentUnknown( const IID iid, IUnknown **ppunk ) = 0;

	com_call AddComponent( IUnknown *punk, int *piIdx ) = 0;
	com_call RemoveComponent( int idx ) = 0;
};

//compenent management
interface ICompRegistrator : public IUnknown
{
	com_call GetCount( int *piCount ) = 0;
	com_call GetCompInfo( int idx, BSTR *pbstr ) = 0;
//		com_call GetSection
//		com_call RegisterComp( BSTR bstr, DWORD dwComp );
//		com_call UnRegisterComp( BSTR bstr, DWORD dwComp );
};

interface IEventListener : public IUnknown
{
	com_call Notify( BSTR szEventDesc, IUnknown *pHint, IUnknown *pFrom, VARIANT data ) = 0;
	com_call IsInNotify(BOOL *pbFlag) = 0;

};


interface INotifyController : public IUnknown
{
	com_call FireEvent( BSTR szEventDesc, IUnknown *pHint, IUnknown *pFrom, VARIANT data ) = 0;
	com_call RegisterEventListener( BSTR szEventDesc, IUnknown *pListener ) = 0;
	com_call UnRegisterEventListener( BSTR szEventDesc, IUnknown *pListener ) = 0;
	com_call IsRegisteredEventListener( BSTR szEventDesc, IUnknown *pListener, BOOL * pbFlag) = 0;
};

interface INotifyController2 : public INotifyController
{
	com_call AddSource(IUnknown * punkSrc) = 0;
	com_call RemoveSource(IUnknown * punkSrc) = 0;
};


interface IHelpInfo : public IUnknown
{
	com_call GetHelpInfo( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags ) = 0;
};

interface IPropertySheet : public IUnknown
{
	com_call IncludePage(int nPage) = 0;
	com_call ExcludePage(int nPage) = 0;
	
	com_call GetPagesCount(int* pNumPages) = 0;
	com_call GetPage( int nPage, IUnknown **ppunkPage ) = 0;

	com_call ActivatePage( int nPage ) = 0;
	com_call GetActivePage( int *pnPage ) = 0;
	com_call SetModified( BOOL bSetModified ) = 0;
};

interface IStatusBar : public IUnknown
{
	com_call IncludePane(IUnknown* punkPane, int* pNum) = 0;
	com_call ExcludePane(int nNum) = 0;
	com_call SaveState(IUnknown* punkNamedData, BSTR bstrSection) = 0;
	com_call LoadState(IUnknown* punkNamedData, BSTR bstrSection) = 0;
	com_call GetHandle( HANDLE *phWnd ) = 0;
};



interface IProvideColors : public IUnknown
{
	com_call GetColorsInfo(long* pnCount, DWORD** ppdwColors) = 0;
};


interface IAuditTrailMan : public IUnknown
{
	com_call ToggleOnOffFlag() = 0;
	com_call GetCreateFlag(BOOL* pbWillCreate) = 0;
	com_call GetAuditTrail(IUnknown* punkImage, IUnknown** ppunkAT) = 0;
	com_call RescanActionsList() = 0;
};

enum AuditTrailDrawFlags
{
	atdfVert = 1,			//vertical view (thumbnails)
	atdfHorz = 1<<1,		//horizontal view (thumbnails)
	//if niether vertical nor horizontal flag set, than draw single object view with original DIB size
};


declare_interface( INotifyController2, "EEB71132-0F95-41f6-A851-8E985B8A7076" );
declare_interface( INamedObject2, "51CE67A8-2220-11d3-A5D6-0000B493FF1B" );
declare_interface( INumeredObject, "51CE67A9-2220-11d3-A5D6-0000B493FF1B" );
declare_interface( IStoredObject, "51CE67AA-2220-11d3-A5D6-0000B493FF1B" );
declare_interface( IRootedObject, "51CE67AB-2220-11d3-A5D6-0000B493FF1B" );
declare_interface( ICompManager, "51CE67AC-2220-11d3-A5D6-0000B493FF1B" );
declare_interface( ICompRegistrator, "51CE67AD-2220-11d3-A5D6-0000B493FF1B" );
declare_interface( IEventListener, "51CE67AE-2220-11d3-A5D6-0000B493FF1B" );
declare_interface( INotifyController, "51CE67B3-2220-11d3-A5D6-0000B493FF1B" );
declare_interface( IHelpInfo, "911C6B01-5AEB-11d3-A635-0090275995FE" );
declare_interface( IPropertySheet, "59C0F710-7FE5-11d3-A50C-0000B493A187" );
declare_interface( IStatusBar, "1362B112-8AE6-11d3-A523-0000B493A187" );
declare_interface( IProvideColors, "AB4386E1-FE60-11d3-A0C5-0000B493A187");
declare_interface( IClipboard, "D1EFC4A4-F6F0-4667-96B5-957BBCC2C4B8");*/

#endif // __misc5_h__
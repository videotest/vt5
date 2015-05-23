// GalleryAviPage.h: interface for the CGalleryAviPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GALLERYAVIPAGE_H__3089AD60_F776_4832_837D_0FC53B9B3BF2__INCLUDED_)
#define AFX_GALLERYAVIPAGE_H__3089AD60_F776_4832_837D_0FC53B9B3BF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "win_page.h"
#include "com_main.h"
#include "misc_utils.h"
#include "resource.h"
#include "ax_events.h"
#include "impl_help.h"
#include "\vt5\ifaces\avi_int.h"




class CGalleryAviPage : public ComObjectBase, 
							public CAxEventMap,
							public CPageImpl, 
							public CHelpInfoImpl,
							public CNamedObjectImpl,
							public IPersist,							
							public IAviGalleryPropPage,
							public _dyncreate_t<CGalleryAviPage>
{
protected:
	virtual UINT idd()
	{	return IDD_AVI_GALLERY_PP;}

protected:
	route_unknown();

	IUnknown *DoGetInterface( const IID &iid );

	com_call GetClassID( CLSID *pclsid );

	virtual bool DoInitDialog();		
	virtual CWinImpl *GetTarget();

	//IAviGalleryPropPage
	com_call ReadInfo( IUnknown* punkAvi );

public:
	CGalleryAviPage();
	virtual ~CGalleryAviPage();

};

#endif // !defined(AFX_GALLERYAVIPAGE_H__3089AD60_F776_4832_837D_0FC53B9B3BF2__INCLUDED_)

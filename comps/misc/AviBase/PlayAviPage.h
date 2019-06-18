// PlayAviPage.h: interface for the CPlayAviPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYAVIPAGE_H__8840DE1A_290A_448D_B0A1_86C146B2061B__INCLUDED_)
#define AFX_PLAYAVIPAGE_H__8840DE1A_290A_448D_B0A1_86C146B2061B__INCLUDED_

#include "win_page.h"
#include "com_main.h"
#include "misc_utils.h"
#include "resource.h"
#include "ax_events.h"
#include "impl_help.h"
#include "\vt5\ifaces\avi_int.h"


#import "stdole2.tlb" rename_namespace("OcxSpace") exclude("GUID", "DISPPARAMS", "EXCEPINFO", \
	"OLE_XPOS_PIXELS", "OLE_YPOS_PIXELS", "OLE_XSIZE_PIXELS", "OLE_YSIZE_PIXELS", "OLE_XPOS_HIMETRIC", \
	"OLE_YPOS_HIMETRIC", "OLE_XSIZE_HIMETRIC", "OLE_YSIZE_HIMETRIC", "OLE_XPOS_CONTAINER", \
	"OLE_YPOS_CONTAINER", "OLE_XSIZE_CONTAINER", "OLE_YSIZE_CONTAINER", "OLE_HANDLE", "OLE_OPTEXCLUSIVE", \
	"OLE_CANCELBOOL", "OLE_ENABLEDEFAULTBOOL", "FONTSIZE", "OLE_COLOR", \
	"IUnknown", "IDispatch", "IEnumVARIANT", "IFont", "IPicture")

//ViewAX button referense
#ifdef _DEBUG
#import "..\\..\\..\\vt5\\debug\\comps\\VTControls.ocx" rename_namespace("OcxSpace") 
#else
#import "..\\..\\..\\vt5\\Release\\comps\\VTControls.ocx" rename_namespace("OcxSpace") 
#endif //_DEBUG

using namespace OcxSpace; 



class CPlayAviPage : public ComObjectBase, 
							public CAxEventMap,
							public CPageImpl, 
							public CHelpInfoImpl,
							public IPersist,
							public IAviPlayPropPage,
							public _dyncreate_t<CPlayAviPage>
{
protected:
	virtual UINT idd()
	{	return IDD_AVI_PLAY_PP;}

public:
	CPlayAviPage();
	virtual ~CPlayAviPage();

protected:
	route_unknown();

	IUnknown *DoGetInterface( const IID &iid );

	com_call GetClassID( CLSID *pclsid );

	virtual bool DoInitDialog();		
	virtual CWinImpl *GetTarget();
    

	void OnPlay();
	void OnStop();
	void OnPause();	
	void OnPrev();	
	void OnNext();	
	void OnFirst();	
	void OnLast();	
	void OnSetActiveFrame();	

	void OnSliderChange();


	//IAviPlayPropPage
	com_call AttachAction( IUnknown* punk );
	com_call UpdatePosition( long lFrame, long lFrameCount, double fFramesPerSecond );
	com_call UpdateCtrls();
protected:	

	_DPushButtonPtr	m_ptrPlayBtn;
	_DPushButtonPtr	m_ptrStopBtn;
	_DPushButtonPtr	m_ptrPauseBtn;	
	_DPushButtonPtr	m_ptrPrevBtn;	
	_DPushButtonPtr	m_ptrNextBtn;	
	_DPushButtonPtr	m_ptrFirstBtn;	
	_DPushButtonPtr	m_ptrLastBtn;	
	_DPushButtonPtr	m_ptrSetActiveBtn;

	_DVTSliderPtr	m_ptrSlider;


	// [vanek] - 14.09.2004
    IUnknown* GetActiveView();
	IAviImagePtr GetActiveAviImage();
};

#endif // !defined(AFX_PLAYAVIPAGE_H__8840DE1A_290A_448D_B0A1_86C146B2061B__INCLUDED_)

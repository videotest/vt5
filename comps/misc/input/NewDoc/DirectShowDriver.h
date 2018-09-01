// DirectShowDriver.h: interface for the CDirectShowDriver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTSHOWDRIVER_H__82922346_3B8B_42F4_BF6E_A6E9B997D744__INCLUDED_)
#define AFX_DIRECTSHOWDRIVER_H__82922346_3B8B_42F4_BF6E_A6E9B997D744__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DShow.h"
#include "qedit.h"
#include "Driver.h"
#include "NewDocVT5Profile.h"

_COM_SMARTPTR_TYPEDEF(IGraphBuilder, __uuidof(IGraphBuilder));
_COM_SMARTPTR_TYPEDEF(ICaptureGraphBuilder2, __uuidof(ICaptureGraphBuilder2));
_COM_SMARTPTR_TYPEDEF(IBaseFilter, __uuidof(IBaseFilter));
_COM_SMARTPTR_TYPEDEF(IPin, __uuidof(IPin));
_COM_SMARTPTR_TYPEDEF(IEnumPins, __uuidof(IEnumPins));
_COM_SMARTPTR_TYPEDEF(ISampleGrabber, __uuidof(ISampleGrabber));
_COM_SMARTPTR_TYPEDEF(IMediaControl, __uuidof(IMediaControl));
_COM_SMARTPTR_TYPEDEF(IMediaEvent, __uuidof(IMediaEvent));
_COM_SMARTPTR_TYPEDEF(IAMStreamConfig, __uuidof(IAMStreamConfig));
_COM_SMARTPTR_TYPEDEF(IVideoWindow, __uuidof(IVideoWindow));
_COM_SMARTPTR_TYPEDEF(IBasicVideo, __uuidof(IBasicVideo));
_COM_SMARTPTR_TYPEDEF(IFileSinkFilter, __uuidof(IFileSinkFilter));
_COM_SMARTPTR_TYPEDEF(ICreateDevEnum, __uuidof(ICreateDevEnum));
_COM_SMARTPTR_TYPEDEF(IEnumMoniker, __uuidof(IEnumMoniker));
_COM_SMARTPTR_TYPEDEF(IMoniker, __uuidof(IMoniker));
_COM_SMARTPTR_TYPEDEF(IPropertyBag, __uuidof(IPropertyBag));
_COM_SMARTPTR_TYPEDEF(ISpecifyPropertyPages, __uuidof(ISpecifyPropertyPages));
_COM_SMARTPTR_TYPEDEF(IAMVfwCompressDialogs, __uuidof(IAMVfwCompressDialogs));
_COM_SMARTPTR_TYPEDEF(IAMDroppedFrames, __uuidof(IAMDroppedFrames));

class CDShowDevInfo
{
public:
	CString m_sName;
	IMonikerPtr m_pMon;
	CDShowDevInfo()
	{
	}
	~CDShowDevInfo()
	{
		try
		{
			m_pMon = 0;
		}
		catch(...)
		{
			TRACE("Exception in %s\n", (const char *)m_sName);
		};
	}
	CDShowDevInfo(LPCTSTR lpstrName)
	{
		m_sName = lpstrName;
	}
	CDShowDevInfo(CDShowDevInfo &t)
	{
		m_sName = t.m_sName;
		m_pMon = t.m_pMon;
	}
	CDShowDevInfo(IMonikerPtr pMoniker)
	{
		IPropertyBagPtr pProp;
		pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pProp);
		COleVariant varName;
		VariantInit(&varName); // Try to match the friendly name.
		HRESULT hr = pProp->Read(L"FriendlyName", &varName, 0); 
		if (SUCCEEDED(hr))
		{
			m_sName.LoadString(IDS_DIRECTSHOW);
			if (!m_sName.IsEmpty())
				m_sName += CString(_T(":"));
			m_sName += CString(varName.bstrVal);
		}
		m_pMon = pMoniker;
	}
};

class CDSState
{
public:
	int  m_nGraphMode; //0 - no graph built, 1 - preview graph built, 2 - capture graph built
	int  m_nPreviewMode; //0-no preview, 1-preview
	int  m_nDev;
	HWND m_hwndParent;
	CDSState()
	{
	}
	CDSState(CDSState &s)
	{
		m_nGraphMode = s.m_nGraphMode;
		m_nPreviewMode = s.m_nPreviewMode;
		m_nDev = s.m_nDev;
		m_hwndParent = s.m_hwndParent;
	}
};

class CDirectShowDriver : public CCmdTargetEx, public CNamedObjectImpl, public CDSState, 
	public CDriver, public IInputWindow, public IDSCompressionSite
{
	DECLARE_DYNCREATE(CDirectShowDriver);
	ENABLE_MULTYINTERFACE();
	GUARD_DECLARE_OLECREATE(CDirectShowDriver);

	bool m_bRestoreSettings;
	CArray<CDShowDevInfo,CDShowDevInfo&> m_Devices;
	CArray<CDShowDevInfo,CDShowDevInfo&> m_Compressors;
	CArray<CDSState,CDSState&> m_States;
//	int m_nCompressor;
	IBaseFilterPtr m_CurFilter;
	IGraphBuilderPtr m_GraphBuilder;
	ICaptureGraphBuilder2Ptr m_CaptureGraphBuilder;
	ISampleGrabberPtr m_pGrabber;
	bool m_bInit;
//	int  m_nGraphMode; //0 - no graph built, 1 - preview graph built, 2 - capture graph built
//	int  m_nPreviewMode; //0-no preview, 1-preview
//	int  m_nDev;
//	HWND m_hwndParent;
	bool Initialize();
	void MakeDevices();
	void MakeCompressors();
	IBaseFilterPtr GetFilter(int nDev);
	IBaseFilterPtr GetCompressor(int nDev);
	bool BuildPreviewGraph(int nDev, int nGraphMode);
	void DestroyGraph();
	void StartPreview(int nDev, HWND hwndParent, int nMode); // 1- preview, 2 - preview and capture
	void StopPreview();
//	bool IsPreviewGraph() {return m_CurFilter!=0;}
//	bool IsPreview() {return m_MediaCtrl!= 0;}
	bool DoInputNative(IUnknown *pUnkImage, IUnknown *punkTarget, int nDevice, BOOL bFromSettings);
	void ShowDialog(IUnknown *punk, HWND hwndParent, bool bStopPreview);
	void SaveState();
	void RestoreState();
public:
	CDirectShowDriver();
	virtual ~CDirectShowDriver();

	com_call InputNative(IUnknown *pUnkImage, IUnknown *punkTarget, int nDevice, BOOL bFromSettings);
	com_call InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode);
	com_call ExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput);
	com_call GetFlags(DWORD *pdwFlags);
	com_call GetShortName(BSTR *pbstrShortName);
	com_call GetLongName(BSTR *pbstrShortName);
	com_call ExecuteDriverDialog(int nDev, LPCTSTR lpszDlgName);
	com_call GetDevicesCount(int *pnCount);
	com_call GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory);
	com_call GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue);

	// IInputWindow
	com_call CreateInputWindow(int nDev, HWND hwndParent, long *phCreated);
	com_call DestroyInputWindow(long lCap);
	com_call OnSize(long lCap, HWND hwndParent, SIZE sz);
	com_call GetSize(long lCap, short *pdx, short *pdy);
	com_call SetInputWindowPos(long lCap, int x, int y, int cx, int cy, DWORD dwFlags);

	// IDSCompressionSite
	virtual int GetCompressorsCount();
	virtual BSTR GetCompressorName(int iCompr);
	virtual void SetCompressor(int iCompr);
	virtual void CompressorDlg(int iCompr);
	virtual bool HasCompressorDialog(int iCompr);


	virtual LPUNKNOWN GetInterfaceHook(const void*);
};

extern CVT5ProfileManager g_DSProfile;


#endif // !defined(AFX_DIRECTSHOWDRIVER_H__82922346_3B8B_42F4_BF6E_A6E9B997D744__INCLUDED_)

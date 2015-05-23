// DirectShowDriver.cpp: implementation of the CDirectShowDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewDoc.h"
#include "DirectShowDriver.h"
#include "DirectShowDialog.h"
#include "DSComprDialog.h"
#include "mtype.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CVT5ProfileManager g_DSProfile(_T("DirectShow"),false);
void ConvertAVIFile(const char *pszAVIFileName, DWORD dwNewRate, DWORD dwNewScale, int id);


// {9A45C4CE-5CB6-47FD-8976-6738F35C8C7A}
GUARD_IMPLEMENT_OLECREATE(CDirectShowDriver, "IDriver.DriverDirectShow", 
0x9a45c4ce, 0x5cb6, 0x47fd, 0x89, 0x76, 0x67, 0x38, 0xf3, 0x5c, 0x8c, 0x7a);

IMPLEMENT_DYNCREATE(CDirectShowDriver, CCmdTargetEx)

static void _AddPages(IUnknown *punk, CArray<GUID,GUID&> &arrGuids)
{
	ISpecifyPropertyPagesPtr sptrPages(punk);
	if (sptrPages != 0)
	{
		CAUUID cauuid;
		if (SUCCEEDED(sptrPages->GetPages(&cauuid)))
		{
			for (unsigned i = 0; i < cauuid.cElems; i++)
				arrGuids.Add(cauuid.pElems[i]);
			CoTaskMemFree(cauuid.pElems);
		}
	}
};

static IUnknownPtr _FindInterface(ICaptureGraphBuilder2 *pBuilder, IBaseFilter *pCurFilter, REFIID iid, GUID guidCategory = PIN_CATEGORY_CAPTURE)
{
	IUnknownPtr punk;
	HRESULT hr = pBuilder->FindInterface(&guidCategory, &MEDIATYPE_Interleaved, pCurFilter,
		iid, (void **)&punk);
	if(hr != NOERROR)
		hr = pBuilder->FindInterface(&guidCategory, &MEDIATYPE_Video, pCurFilter,
			iid, (void **)&punk);
	return punk;
}

static void _ShowDialog(IUnknown *punk, HWND hwndParent)
{
	if (!punk) return;
	CArray<GUID,GUID&> arrGuids;
	_AddPages(punk, arrGuids);
	if (arrGuids.GetSize() == 0) return;
	HRESULT hr = OleCreatePropertyFrame(hwndParent, 30, 30, NULL, 1, &punk, arrGuids.GetSize(),
		arrGuids.GetData(), 0, 0, NULL);
}

static void NukeDownstream(IBaseFilter *pf, IGraphBuilder *pFg) 
{
	IPin *pP, *pTo;
	ULONG u;
	IEnumPins *pins = NULL;
	PIN_INFO pininfo;
	HRESULT hr = pf->EnumPins(&pins);
	pins->Reset();
	while(hr == NOERROR)
	{
		hr = pins->Next(1, &pP, &u);
		if(hr == S_OK && pP)
		{
			pP->ConnectedTo(&pTo);
			if(pTo)
			{
				hr = pTo->QueryPinInfo(&pininfo);
				if(hr == NOERROR)
				{
					if(pininfo.dir == PINDIR_INPUT)
					{
						NukeDownstream(pininfo.pFilter, pFg);
						pFg->Disconnect(pTo);
						pFg->Disconnect(pP);
						pFg->RemoveFilter(pininfo.pFilter);
					}
					pininfo.pFilter->Release();
				}
				pTo->Release();
			}
			pP->Release();
		}
	}
	if(pins)
		pins->Release();
}

HRESULT GetPin(IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
	IEnumPinsPtr pEnum;
	*ppPin = NULL;
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if(FAILED(hr)) 
		return hr;
	ULONG ulFound;
	IPin *pPin;
	hr = E_FAIL;
	while(S_OK == pEnum->Next(1, &pPin, &ulFound))
	{
		PIN_DIRECTION pindir = (PIN_DIRECTION)3;
		pPin->QueryDirection(&pindir);
		if(pindir == dirrequired)
		{
			if(iNum == 0)
			{
				*ppPin = pPin;
				// Found requested pin, so clear error
				hr = S_OK;
				break;
			}
			iNum--;
		} 
		pPin->Release();
	} 
	return hr;
}


IPin *GetInPin( IBaseFilter * pFilter, int Num)
{
	IPinPtr pComPin;
	GetPin(pFilter, PINDIR_INPUT, Num, &pComPin);
	return pComPin;
}


IPin *GetOutPin( IBaseFilter * pFilter, int Num)
{
	IPinPtr pComPin;
	GetPin(pFilter, PINDIR_OUTPUT, Num, &pComPin);
	return pComPin;
}


// Note: this object is a SEMI-COM object, and can only be created statically.
// We use this little semi-com object to handle the sample-grab-callback,
// since the callback must provide a COM interface. We could have had an interface
// where you provided a function-call callback, but that's really messy, so we
// did it this way. You can put anything you want into this C++ object, even
// a pointer to a CDialog. Be aware of multi-thread issues though.
//
class CSampleGrabberCB : public ISampleGrabberCB 
{
public:
	// these will get set by the main thread below. We need to
	// know this in order to write out the bmp
	long lWidth;
	long lHeight;
	int nSamplesPerPixel;
	CDirectShowDriver *pOwner;
	double dblSampleTime;
	long lBufferSize;
	BYTE *pBuffer;
	CSampleGrabberCB( )
	{
		pOwner = NULL;
		pBuffer = NULL;
		lWidth = lHeight = lBufferSize = 0;
		nSamplesPerPixel = 3;
	}   
	~CSampleGrabberCB()
	{
		if (pBuffer)
			delete pBuffer;
	}
	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
	{
		if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) 
		{
			*ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
			return NOERROR;
		}    
		return E_NOINTERFACE;
	}
	// we don't implement this interface for this example
	//
	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample )
	{
		return 0;
	}
	// The sample grabber is calling us back on its deliver thread.
	// This is NOT the main app thread!
	//
	//           !!!!! WARNING WARNING WARNING !!!!!
	//
	// On Windows 9x systems, you are not allowed to call most of the 
	// Windows API functions in this callback.  Why not?  Because the
	// video renderer might hold the global Win16 lock so that the video
	// surface can be locked while you copy its data.  This is not an
	// issue on Windows 2000, but is a limitation on Win95,98,98SE, and ME.
	// Calling a 16-bit legacy function could lock the system, because 
	// it would wait forever for the Win16 lock, which would be forever
	// held by the video renderer.
	//
	// As a workaround, copy the bitmap data during the callback,
	// post a message to our app, and write the data later.
	//
	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize )
	{
		// Since we can't access Windows API functions in this callback, just
		// copy the bitmap data to a global structure for later reference.
		dblSampleTime = dblSampleTime;
		this->lBufferSize   = lBufferSize;
		// If we haven't yet allocated the data buffer, do it now.
		// Just allocate what we need to store the new bitmap.
		if (!this->pBuffer)
			this->pBuffer = new BYTE[lBufferSize];
		// Copy the bitmap data into our global buffer
		if (this->pBuffer)
			memcpy(this->pBuffer, pBuffer, lBufferSize);
		return 0;
	}
	void FreeBuffer()
	{
		delete pBuffer;
		pBuffer = NULL;
		lWidth = lHeight = lBufferSize = 0;
	}
};

// this semi-COM object will receive sample callbacks for us
//
CSampleGrabberCB mCB;


void check_hr(HRESULT hr)
{
}

GUID GetFormatSelected(IUnknownPtr punk)
{
	IAMStreamConfigPtr pAMConfig(punk);
	if (pAMConfig == 0) return MEDIASUBTYPE_RGB24;
	AM_MEDIA_TYPE *pmt;
	pAMConfig->GetFormat(&pmt);
	GUID ret;
	if (pmt->subtype == MEDIASUBTYPE_RGB8)
		ret = pmt->subtype;
	else
		ret = MEDIASUBTYPE_RGB24;
	DeleteMediaType(pmt);
	return ret;
}

struct _FormatRecord
{
	GUID guid;
	LPCTSTR lpstrName;
	_FormatRecord(const GUID guid, LPCTSTR lpstrName)
	{
		this->guid = guid;
		this->lpstrName = lpstrName;
	}
};


_FormatRecord FormatRecords[] = 
{
	_FormatRecord(MEDIASUBTYPE_YVU9, _T("YVU9")),
	_FormatRecord(MEDIASUBTYPE_Y411, _T("Y411")),
	_FormatRecord(MEDIASUBTYPE_Y41P, _T("Y41P")),
	_FormatRecord(MEDIASUBTYPE_YUY2, _T("YUY2")),
	_FormatRecord(MEDIASUBTYPE_YVYU, _T("YVYU")),
	_FormatRecord(MEDIASUBTYPE_UYVY, _T("UYVY")),
	_FormatRecord(MEDIASUBTYPE_Y211, _T("Y211")),
	_FormatRecord(MEDIASUBTYPE_CLJR, _T("CLJR")),
	_FormatRecord(MEDIASUBTYPE_IF09, _T("IF09")),
	_FormatRecord(MEDIASUBTYPE_CPLA, _T("CPLA")),
	_FormatRecord(MEDIASUBTYPE_MJPG, _T("MJPG")),
	_FormatRecord(MEDIASUBTYPE_TVMJ, _T("TVMJ")),
	_FormatRecord(MEDIASUBTYPE_WAKE, _T("WAKE")),
	_FormatRecord(MEDIASUBTYPE_CFCC, _T("CFCC")),
	_FormatRecord(MEDIASUBTYPE_IJPG, _T("IJPG")),
	_FormatRecord(MEDIASUBTYPE_Plum, _T("Plum")),
	_FormatRecord(MEDIASUBTYPE_RGB1, _T("RGB1")),
	_FormatRecord(MEDIASUBTYPE_RGB4, _T("RGB4")),
	_FormatRecord(MEDIASUBTYPE_RGB8, _T("RGB8")),
	_FormatRecord(MEDIASUBTYPE_RGB565, _T("RGB565")),
	_FormatRecord(MEDIASUBTYPE_RGB555, _T("RGB555")),
	_FormatRecord(MEDIASUBTYPE_RGB24, _T("RGB24")),
	_FormatRecord(MEDIASUBTYPE_RGB32, _T("RGB32")),
	_FormatRecord(MEDIASUBTYPE_ARGB32, _T("ARGB32")),
	_FormatRecord(MEDIASUBTYPE_Overlay, _T("Overlay")),
	_FormatRecord(MEDIASUBTYPE_QTMovie, _T("QTMovie")),
	_FormatRecord(MEDIASUBTYPE_QTRpza, _T("QTRpza")),
	_FormatRecord(MEDIASUBTYPE_QTSmc, _T("QTSmc")),
	_FormatRecord(MEDIASUBTYPE_QTRle, _T("QTRle")),
	_FormatRecord(MEDIASUBTYPE_QTJpeg, _T("QTJpeg")),
	_FormatRecord(MEDIASUBTYPE_dvsd, _T("dvsd")),
	_FormatRecord(MEDIASUBTYPE_dvhd, _T("dvhd")),
	_FormatRecord(MEDIASUBTYPE_dvsl, _T("dvsl")),
	_FormatRecord(MEDIASUBTYPE_MPEG1Packet, _T("MPEG1Packet")),
	_FormatRecord(MEDIASUBTYPE_MPEG1Payload, _T("MPEG1Payload")),
	_FormatRecord(MEDIASUBTYPE_VPVideo, _T("VPVideo")),
	_FormatRecord(MEDIASUBTYPE_VPVBI, _T("VPVBI")),
};

const int nFormats = sizeof(FormatRecords)/sizeof(FormatRecords[0]);

static GUID _FormatFromString(LPCTSTR lpszName)
{
	for (int i = 0; i < nFormats; i++)
	{
		if (_tcsicmp(FormatRecords[i].lpstrName, lpszName) == 0)
			return FormatRecords[i].guid;
	}
	return GUID_NULL;
}

static CString _FormatToString(GUID guidFormat)
{
	for (int i = 0; i < nFormats; i++)
	{
		if (FormatRecords[i].guid == guidFormat)
			return CString(FormatRecords[i].lpstrName);
	}
	return CString(_T(""));
}

void RestoreFormat(ICaptureGraphBuilder2 *punkBuilder, IBaseFilter *punkCapFilter, LPCTSTR lpszDriverName, bool bRestoreAll)
{
	CString sDrvName = g_DSProfile.GetProfileString(_T("Format"), _T("DriverName"), NULL);
	if (sDrvName != CString(lpszDriverName)) return;
	IAMStreamConfigPtr sptrAMCfg(_FindInterface(punkBuilder,punkCapFilter,IID_IAMStreamConfig));
	IAMStreamConfigPtr sptrAMCfgPrv(_FindInterface(punkBuilder,punkCapFilter,IID_IAMStreamConfig, PIN_CATEGORY_PREVIEW));
	AM_MEDIA_TYPE *pmt;
	HRESULT hr = sptrAMCfg->GetFormat(&pmt);
	if (hr == S_OK)
	{
		if (pmt->formattype == FORMAT_VideoInfo)
		{
			int nFPS = g_DSProfile.GetProfileInt(_T("VideoDlg"), _T("FPS"), 15);
			int nRateScale = g_DSProfile.GetProfileInt(_T("VideoDlg"), _T("RateScale"), 0);
			VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
			if (nRateScale == 1)
				pvi->AvgTimePerFrame = (LONGLONG)(60*10000000/nFPS);// frames per minute
			else if (nRateScale == 2)
				pvi->AvgTimePerFrame = (LONGLONG)(60i64*60i64*10000000i64/nFPS);// frames per hour
			else
				pvi->AvgTimePerFrame = (LONGLONG)(10000000 / nFPS);
			if (bRestoreAll)
			{
				CString sFormatType = g_DSProfile.GetProfileString(_T("Format"), _T("FormatType"), NULL);
				if (!sFormatType.IsEmpty())
				{
					GUID guidFmt = _FormatFromString(sFormatType);
					if (guidFmt != GUID_NULL) 
					{
						pmt->subtype = guidFmt;
						pvi->bmiHeader.biWidth = g_DSProfile.GetProfileInt(_T("Format"), _T("Width"), 640);
						pvi->bmiHeader.biHeight = g_DSProfile.GetProfileInt(_T("Format"), _T("Height"), 480);
						pvi->bmiHeader.biBitCount = g_DSProfile.GetProfileInt(_T("Format"), _T("BPP"), 24);
						pvi->bmiHeader.biCompression = g_DSProfile.GetProfileInt(_T("Format"), _T("Compression"), 0);
						pmt->lSampleSize = g_DSProfile.GetProfileInt(_T("Format"), _T("SampleSize"), 640*480*3);
					}
				}
			}
			hr = sptrAMCfg->SetFormat(pmt);
			hr = sptrAMCfgPrv->SetFormat(pmt);
		}
        DeleteMediaType(pmt);
	}
}

void SaveFormat(ICaptureGraphBuilder2 *punkBuilder, IBaseFilter *punkCapFilter, LPCTSTR lpszDriverName)
{
	g_DSProfile.WriteProfileString(_T("Format"), _T("DriverName"), lpszDriverName);
	IAMStreamConfigPtr sptrAMCfg(_FindInterface(punkBuilder,punkCapFilter,IID_IAMStreamConfig));
	AM_MEDIA_TYPE *pmt;
	HRESULT hr = sptrAMCfg->GetFormat(&pmt);
	if (hr == S_OK)
	{
		if (pmt->formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
			g_DSProfile.WriteProfileInt(_T("Format"), _T("Width"), pvi->bmiHeader.biWidth);
			g_DSProfile.WriteProfileInt(_T("Format"), _T("Height"), pvi->bmiHeader.biHeight);
			g_DSProfile.WriteProfileInt(_T("Format"), _T("BPP"), pvi->bmiHeader.biBitCount);
			g_DSProfile.WriteProfileInt(_T("Format"), _T("SampleSize"), pmt->lSampleSize);
			CString sFormatType = _FormatToString(pmt->subtype);
			g_DSProfile.WriteProfileString(_T("Format"), _T("FormatType"), sFormatType);
			g_DSProfile.WriteProfileInt(_T("Format"), _T("Compression"), pvi->bmiHeader.biCompression);
		}
        DeleteMediaType(pmt);
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectShowDriver::CDirectShowDriver()
{
	m_bInit = false;
	m_nGraphMode = 0;
	m_nPreviewMode = 0;
	m_nDev = -1;
	m_hwndParent = NULL;
	m_bRestoreSettings = g_DSProfile.GetProfileInt(_T("Settings"), _T("SaveFormat"), 0, true, true)?true:false;
}

CDirectShowDriver::~CDirectShowDriver()
{
	ASSERT(m_States.GetSize()==0);
	try // Protection against access violations in drivers
	{
		m_GraphBuilder = 0;
		m_CaptureGraphBuilder = 0;
		m_CurFilter = 0;
		m_pGrabber = 0;
		m_Devices.RemoveAll();
		m_Compressors.RemoveAll();
	}
	catch(...)
	{
	}
}

void CDirectShowDriver::MakeCompressors()
{
	ICreateDevEnumPtr DevEnum;
	DevEnum.CreateInstance(CLSID_SystemDeviceEnum);
	IEnumMonikerPtr pEnumCat;
	DevEnum->CreateClassEnumerator(CLSID_VideoCompressorCategory, &pEnumCat, 0);
	IMonikerPtr pMoniker;
	ULONG cFetched;
	m_Compressors.RemoveAll();
	CString sNone;
	sNone.LoadString(IDS_NO_COMPRESSION);
	m_Compressors.Add(CDShowDevInfo(((LPCTSTR)sNone)));
	while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		m_Compressors.Add(CDShowDevInfo(pMoniker));
}

void CDirectShowDriver::MakeDevices()
{
	ICreateDevEnumPtr DevEnum;
	DevEnum.CreateInstance(CLSID_SystemDeviceEnum);
	IEnumMonikerPtr pEnumCat;
	DevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);
	if (pEnumCat == 0) return;
	IMonikerPtr pMoniker;
	ULONG cFetched;
	m_Devices.RemoveAll();
	while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		m_Devices.Add(CDShowDevInfo(pMoniker));
}

bool CDirectShowDriver::Initialize()
{
	// Make devices list
	MakeDevices();
	MakeCompressors();
 	// Init basic components
	if (m_bInit) return true;
	m_GraphBuilder.CreateInstance(CLSID_FilterGraph);
	m_CaptureGraphBuilder.CreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC);
	ISampleGrabberPtr pGrabber;
	pGrabber.CreateInstance(CLSID_SampleGrabber);
	if (m_GraphBuilder == 0 || m_CaptureGraphBuilder == 0 || pGrabber == 0)
	{
		m_Devices.RemoveAll();
		m_Compressors.RemoveAll();
		return false;
	}
	m_CaptureGraphBuilder->SetFiltergraph(m_GraphBuilder);
	m_bInit = true;
	return true;
}

IBaseFilterPtr CDirectShowDriver::GetFilter(int nDev)
{
	if (nDev < 0 || nDev >= m_Devices.GetSize())
		return 0;
	IBaseFilterPtr sptrFilter;
	m_Devices[nDev].m_pMon->BindToObject(0,0, IID_IBaseFilter, (void**)&sptrFilter);
	return sptrFilter;
}

IBaseFilterPtr CDirectShowDriver::GetCompressor(int nDev)
{
	if (nDev < 0 || nDev >= m_Compressors.GetSize() || m_Compressors[nDev].m_pMon == 0)
		return 0;
	IBaseFilterPtr sptrCompr;
	m_Compressors[nDev].m_pMon->BindToObject(0,0, IID_IBaseFilter, (void**)&sptrCompr);
	return sptrCompr;
}

bool CDirectShowDriver::BuildPreviewGraph(int nDev, int nGraphMode)
{
	if (m_nGraphMode != 0)
		DestroyGraph();
	// Select filter and add it to graph. If filter was saved, use it (used after forma dlg)
	if (m_CurFilter == 0 || m_nDev != nDev) m_CurFilter = GetFilter(nDev);
	m_nDev = nDev;
	m_GraphBuilder->AddFilter(m_CurFilter, L"Video Capture");
	// Render stream
	if (nGraphMode == 3 || nGraphMode == 4)
	{
		check_hr(m_pGrabber.CreateInstance(CLSID_SampleGrabber));
		IBaseFilterPtr pGrabBase(m_pGrabber);
		CMediaType VideoType;
		VideoType.SetType(&MEDIATYPE_Video);
		GUID subtype = GetFormatSelected(_FindInterface(m_CaptureGraphBuilder,m_CurFilter,IID_IAMStreamConfig));
		VideoType.SetSubtype(&subtype);
		check_hr(m_pGrabber->SetMediaType(&VideoType)); // shouldn't fail
		check_hr(m_GraphBuilder->AddFilter(pGrabBase, L"Grabber"));
	    IPin *pCapOut = GetOutPin(m_CurFilter, 0);
		IPin *pGrabIn = GetInPin(pGrabBase, 0);
		check_hr(m_GraphBuilder->Connect(pCapOut, pGrabIn));
		IPin *pGrabOut = GetOutPin(pGrabBase, 0);
		if (nGraphMode == 3)
			check_hr(m_GraphBuilder->Render(pGrabOut));
		AM_MEDIA_TYPE mt;
		check_hr(m_pGrabber->GetConnectedMediaType(&mt));
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*)mt.pbFormat;
		mCB.pOwner = this;
		mCB.lWidth  = vih->bmiHeader.biWidth;
		mCB.lHeight = vih->bmiHeader.biHeight;
		mCB.nSamplesPerPixel = vih->bmiHeader.biBitCount/8;
		FreeMediaType(mt);
		m_pGrabber->SetBufferSamples(FALSE);
		m_pGrabber->SetOneShot(FALSE);
		m_pGrabber->SetCallback(&mCB, 1);
	}
	else if (nGraphMode == 2)
	{
		IBaseFilterPtr pFile;
		IFileSinkFilterPtr pFileSink;
		_bstr_t s = g_DSProfile.GetProfileString(_T("VideoDlg"), _T("FileName"), _T("c:\\capture.avi"));
		m_CaptureGraphBuilder->SetOutputFileName(&MEDIASUBTYPE_Avi, (wchar_t*)s, &pFile, &pFileSink);
		if (g_DSProfile._GetProfileInt(_T("VideoDlg"), _T("AllocFile"), FALSE))
		{
			unsigned nSizeMB = g_DSProfile._GetProfileInt(_T("VideoDlg"), _T("FileSizeMB"), 256);
			DWORDLONG dwlSize = ((DWORDLONG)nSizeMB)*1024*1024;
			m_CaptureGraphBuilder->AllocCapFile((wchar_t*)s, dwlSize);
		}
		IBaseFilterPtr sptrCompressor(GetCompressor(g_DSProfile.GetProfileInt(_T("VideoDlg"), _T("Compression"), 0)));
		if (sptrCompressor != 0)
			m_GraphBuilder->AddFilter(sptrCompressor, L"Compressor");
		HRESULT hr = m_CaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved,
			m_CurFilter, NULL, pFile);
		if (hr != S_OK && hr != VFW_S_NOPREVIEWPIN) 
			hr = m_CaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_CurFilter, sptrCompressor, pFile);
		if (hr != S_OK) 
			hr = m_CaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_CurFilter, NULL, pFile);
		hr = m_CaptureGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Interleaved,
			m_CurFilter, NULL, NULL);
		if (hr != S_OK && hr != VFW_S_NOPREVIEWPIN) 
			hr = m_CaptureGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_CurFilter, NULL, NULL);
		/*IAMStreamConfigPtr sptrAMCfg(_FindInterface(m_CaptureGraphBuilder,m_CurFilter,IID_IAMStreamConfig));
		AM_MEDIA_TYPE *pmt;
		hr = sptrAMCfg->GetFormat(&pmt);
		if (hr == S_OK)
		{
			if (pmt->formattype == FORMAT_VideoInfo)
			{
				int nFPS = g_DSProfile.GetProfileInt(_T("VideoDlg"), _T("FPS"), 15);
				int nRateScale = g_DSProfile.GetProfileInt(_T("VideoDlg"), _T("RateScale"), 0);
				VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
				if (nRateScale == 1)
					pvi->AvgTimePerFrame = (LONGLONG)(60*10000000/nFPS);// frames per minute
				else if (nRateScale == 2)
					pvi->AvgTimePerFrame = (LONGLONG)(60i64*60i64*10000000i64/nFPS);// frames per hour
				else
					pvi->AvgTimePerFrame = (LONGLONG)(10000000 / nFPS);
				hr = sptrAMCfg->SetFormat(pmt);
			}
            DeleteMediaType(pmt);
		}*/
	}
	else
	{
		HRESULT hr = m_CaptureGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Interleaved,
			m_CurFilter, NULL, NULL);
		if (hr != S_OK && hr != VFW_S_NOPREVIEWPIN) 
			hr = m_CaptureGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_CurFilter, NULL, NULL);
	}
	RestoreFormat(m_CaptureGraphBuilder,m_CurFilter,m_Devices[nDev].m_sName,m_bRestoreSettings);
	m_nGraphMode = nGraphMode;
	return true;
}

void CDirectShowDriver::DestroyGraph()
{
	if (m_nPreviewMode != 0)
		StopPreview();
	if (m_bRestoreSettings)
		SaveFormat(m_CaptureGraphBuilder, m_CurFilter, m_Devices[m_nDev].m_sName);
	if (m_nGraphMode != 0)
		NukeDownstream(m_CurFilter,m_GraphBuilder);
//		m_CurFilter.Release();
//	m_nDev = -1;
	m_nGraphMode = 0;
}

void CDirectShowDriver::StartPreview(int nDev, HWND hwndParent, int nPreviewMode)
{
	if (m_nPreviewMode != 0)
		StopPreview();
	int nGraphMode = nPreviewMode==1?g_DSProfile.GetProfileInt(NULL, _T("UseOverlay"), true, true, true)?1:3:2;
	if (m_nGraphMode != nGraphMode || m_nDev != nDev)
		BuildPreviewGraph(nDev,nGraphMode);
	IVideoWindowPtr VWnd = m_GraphBuilder;
	if (VWnd != 0)
	{
		if (hwndParent)
		{
			VWnd->put_Owner((OAHWND)hwndParent);
			VWnd->put_WindowStyle(WS_CHILD);
			VWnd->put_Left(0);
			VWnd->put_Top(0);
			VWnd->put_Visible(OATRUE);
		}
		else
			VWnd->put_Visible(OAFALSE);
	}
	if (nPreviewMode == 2)
	{
		IMediaEventPtr MediaEvent = m_GraphBuilder;
		MediaEvent->CancelDefaultHandling(EC_REPAINT);
	}
	IMediaControlPtr MediaCtrl = m_GraphBuilder;
	if (FAILED(MediaCtrl->Run()))
		MediaCtrl->Stop();
	m_nPreviewMode = nPreviewMode;
	m_hwndParent = hwndParent;
}

void CDirectShowDriver::StopPreview()
{
	long lDropped=-1,lNotDropped=-1;
	if (m_nPreviewMode == 2)
	{
		IAMDroppedFramesPtr sptrDroppedFrames(m_CurFilter);
		if (sptrDroppedFrames != 0)
		{
			sptrDroppedFrames->GetNumDropped(&lDropped);
			sptrDroppedFrames->GetNumNotDropped(&lNotDropped);
		}
		IMediaEventPtr MediaEvent = m_GraphBuilder;
		MediaEvent->RestoreDefaultHandling(EC_REPAINT);
	}
	IMediaControlPtr MediaCtrl = m_GraphBuilder;
	if (MediaCtrl != 0) MediaCtrl->Stop();
	if (m_nPreviewMode == 2 && lDropped >= 0 && lNotDropped >= 0 && g_DSProfile.GetProfileInt(NULL, _T("ShowDroppedInfo"), FALSE, true, true))
	{
		CString s;
		s.Format(_T("Dropped %d files, written %d"), lDropped, lNotDropped);
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
	}
	m_nPreviewMode = 0;
	m_hwndParent = NULL;
}

void CDirectShowDriver::ShowDialog(IUnknown *punk, HWND hwndParent, bool bStopPreview)
{
	if (!punk) return;
	CArray<GUID,GUID&> arrGuids;
	_AddPages(punk, arrGuids);
	if (arrGuids.GetSize() == 0) return;
	int nPreviewMode = m_nPreviewMode;
	HWND hwnd = m_hwndParent;
	int nDev = m_nDev;
	if (nPreviewMode != 0 && bStopPreview)
		DestroyGraph();
//		StopPreview();
	HRESULT hr = OleCreatePropertyFrame(hwndParent, 30, 30, NULL, 1, &punk, arrGuids.GetSize(),
		arrGuids.GetData(), 0, 0, NULL);
	if (m_bRestoreSettings) SaveFormat(m_CaptureGraphBuilder, m_CurFilter, m_Devices[m_nDev].m_sName);
	if (nPreviewMode != 0 && bStopPreview)
		StartPreview(nDev, hwnd, 1);
}

bool CDirectShowDriver::DoInputNative(IUnknown *pUnkImage, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	CImageWrp ImgWrp(pUnkImage);
	ImgWrp.CreateNew(mCB.lWidth, mCB.lHeight, mCB.nSamplesPerPixel==3?"RGB":"Gray");
	DWORD dwRow = (mCB.lWidth*mCB.nSamplesPerPixel+3)/4*4;
	for (int i = 0; i < mCB.lHeight; i++)
	{
		BYTE *pRowSrc = mCB.pBuffer+(mCB.lHeight-i-1)*dwRow;
		if (mCB.nSamplesPerPixel==3)
		{
			color *pRowDstR = ImgWrp.GetRow(i,0);
			color *pRowDstG = ImgWrp.GetRow(i,1);
			color *pRowDstB = ImgWrp.GetRow(i,2);
			for (int j = 0; j < mCB.lWidth; j++,pRowSrc+=3,pRowDstR++,pRowDstG++,pRowDstB++)
			{
				*pRowDstR = color(pRowSrc[2])<<8;
				*pRowDstG = color(pRowSrc[1])<<8;
				*pRowDstB = color(pRowSrc[0])<<8;
			}
		}
		else
		{
			color *pRowDst = ImgWrp.GetRow(i,0);
			for (int j = 0; j < mCB.lWidth; j++,pRowSrc++,pRowDst++)
				*pRowDst = color(*pRowSrc)<<8;
		}
	}
	return true;
}

void CDirectShowDriver::SaveState()
{
	if (m_nGraphMode != 0)
		m_States.Add(*(CDSState*)this);
}

void CDirectShowDriver::RestoreState()
{
	if (m_States.GetSize() > 0)
	{
		CDSState State = m_States[m_States.GetSize()-1];
		m_States.RemoveAt(m_States.GetSize()-1);
		BuildPreviewGraph(State.m_nDev, State.m_nGraphMode);
		if (State.m_nPreviewMode != 0)
			StartPreview(State.m_nDev, State.m_hwndParent, State.m_nPreviewMode);
	}
}

LPUNKNOWN CDirectShowDriver::GetInterfaceHook(const void* p)
{
	IID iid = *(IID*)p;
	if (iid == IID_IDriver || iid == IID_IDriver2 || iid == IID_IDriver3 || iid == IID_IDriver4 || iid == IID_IUnknown)
		return (IDriver4*)this;
	else if (iid == IID_IInputWindow)
		return (IInputWindow*)this;
	else
		return CCmdTargetEx::GetInterfaceHook(p);
}

HRESULT CDirectShowDriver::InputNative(IUnknown *pUnkImage, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	if ((bFromSettings || m_nPreviewMode != 0) && mCB.pBuffer && mCB.lWidth && mCB.lHeight)
	{
		DoInputNative(pUnkImage,punkTarget,nDevice,bFromSettings);
		mCB.FreeBuffer();
	}
	else
	{
		SaveState();
		BuildPreviewGraph(nDevice,4);
		IMediaControlPtr MediaCtrl = m_GraphBuilder;
		MediaCtrl->Run();
		Sleep(g_DSProfile.GetProfileInt(NULL, _T("WaitTimeMs"), 1000, true, true));
		MediaCtrl->Stop();
		DoInputNative(pUnkImage,punkTarget,nDevice,0);
		DestroyGraph();
		mCB.FreeBuffer();
		RestoreState();
	}
	return E_UNEXPECTED;
}

HRESULT CDirectShowDriver::InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nMode == -1)
	{
		StartPreview(nDevice, m_hwndParent, 2);
		return S_OK;
	}
	if (pbstrVideoFile && *pbstrVideoFile)
	{
		CString s(*pbstrVideoFile);
		g_DSProfile.WriteProfileString(_T("VideoDlg"), _T("FileName"), s);
	}
	DirectShowDialog dlg((IDriver4*)this, nDevice, DirectShowDialog::ForAVI);
	if (dlg.DoModal()==IDOK)
	{
		int iFPS = g_DSProfile._GetProfileInt(_T("VideoDlg"), _T("FPS"), 25);
		int iTime = g_DSProfile._GetProfileInt(_T("VideoDlg"), _T("Time"), 1);
		int nRateScale = g_DSProfile._GetProfileInt(_T("VideoDlg"), _T("RateScale"), 0);
		if (nRateScale < 0 || nRateScale > 3)
			nRateScale = 0;
		int iPlaybackFPS = g_DSProfile._GetProfileInt(_T("VideoDlg"), _T("PlaybackFPS"), -1);
		if (iPlaybackFPS == -1)
			iPlaybackFPS = iFPS;
		int nPlaybackRateScale = g_DSProfile._GetProfileInt(_T("VideoDlg"), _T("PlaybackRateScale"), -1);
		if (nPlaybackRateScale < 0 || nPlaybackRateScale > 3)
			nPlaybackRateScale = nRateScale;
		CString sName = g_DSProfile.GetProfileString(_T("VideoDlg"), _T("FileName"), _T("c:\\capture.avi"));
		if (iFPS != iPlaybackFPS || nRateScale != nPlaybackRateScale)
			ConvertAVIFile(sName, iPlaybackFPS, nPlaybackRateScale==1?60:nPlaybackRateScale==2?3600:1, IDS_ERROR_CONVERT_AVI);
		if (pbstrVideoFile)
			*pbstrVideoFile = sName.AllocSysString();
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CDirectShowDriver::ExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	if (nMode == 0) return S_OK;
	DirectShowDialog dlg((IDriver4*)this, nDevice, DirectShowDialog::ForInput, CWnd::FromHandle(hwndParent));
	return dlg.DoModal()==IDOK?S_OK:S_FALSE;
}


HRESULT CDirectShowDriver::GetFlags(DWORD *pdwFlags)
{
	*pdwFlags = FG_SUPPORTSVIDEO;
	return S_OK;
}

HRESULT CDirectShowDriver::GetShortName(BSTR *pbstrShortName)
{
	CString s; s.LoadString(IDS_DIRECTSHOW);
	*pbstrShortName = s.AllocSysString();
	return S_OK;
}

HRESULT CDirectShowDriver::GetLongName(BSTR *pbstrShortName)
{
	CString s; s.LoadString(IDS_DIRECTSHOW);
	*pbstrShortName = s.AllocSysString();
	return S_OK;
}

HRESULT CDirectShowDriver::ExecuteDriverDialog(int nDev, LPCTSTR lpszDlgName)
{
	if (lpszDlgName && lpszDlgName[0] == _T('?'))
	{
		lpszDlgName++;
		if (_tcscmp(lpszDlgName,_T("Compression"))==0)
			return S_OK;
		IUnknownPtr punk;
		if (_tcscmp(lpszDlgName,_T("Source"))==0)
			punk = m_CurFilter;
		else if (_tcscmp(lpszDlgName,_T("Format"))==0)
			punk = _FindInterface(m_CaptureGraphBuilder,m_CurFilter,IID_IAMStreamConfig);
		else if (_tcscmp(lpszDlgName,_T("Crossbar"))==0)
			punk = _FindInterface(m_CaptureGraphBuilder,m_CurFilter,IID_IAMCrossbar);
		else if (_tcscmp(lpszDlgName,_T("Tuner"))==0)
			punk = _FindInterface(m_CaptureGraphBuilder,m_CurFilter,IID_IAMTVTuner);
		return punk==0?S_FALSE:S_OK;
	}
	if (_tcscmp(lpszDlgName,_T("Source"))==0)
		ShowDialog(m_CurFilter, m_hwndParent, false);
	else if (_tcscmp(lpszDlgName,_T("Format"))==0)
		ShowDialog(_FindInterface(m_CaptureGraphBuilder,m_CurFilter,IID_IAMStreamConfig), m_hwndParent, true);
	else if (_tcscmp(lpszDlgName,_T("Crossbar"))==0)
		ShowDialog(_FindInterface(m_CaptureGraphBuilder,m_CurFilter,IID_IAMCrossbar), m_hwndParent, true);
	else if (_tcscmp(lpszDlgName,_T("Tuner"))==0)
		ShowDialog(_FindInterface(m_CaptureGraphBuilder,m_CurFilter,IID_IAMTVTuner), m_hwndParent, true);
	else if (_tcscmp(lpszDlgName,_T("Compression"))==0)
	{
		CDSComprDialog dlg(this, CWnd::FromHandle(m_hwndParent));
		dlg.DoModal(); // Graph will be rectreated during capture stage
	}
	return S_OK;
}

HRESULT CDirectShowDriver::GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue)
{
	CString s(bstrName);
	if (_tcsnicmp(s, _T("Is"), 2) == 0)
	{
		pValue->vt = VT_I4;
		if (s.CompareNoCase(_T("IsSource")) || s.CompareNoCase(_T("IsFormat")) ||
			s.CompareNoCase(_T("IsCrossbar")) || s.CompareNoCase(_T("IsTuner")) ||
			s.CompareNoCase(_T("IsCompression")))
		{
			CString s1 = CString(_T("?"))+s.Mid(2);
			pValue->lVal = ExecuteDriverDialog(nCurDev,s1)==S_OK?1:0;
		}
		else
			pValue->lVal = 0;
		return S_OK;
	}
	else
		return CDriver::GetValue(nCurDev, bstrName, pValue);
}


HRESULT CDirectShowDriver::GetDevicesCount(int *pnCount)
{
	if (!Initialize()) return E_UNEXPECTED;
	if (pnCount) *pnCount = m_Devices.GetSize();
	return S_OK;
}

HRESULT CDirectShowDriver::GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory)
{
	if (nDev < 0 || nDev >= m_Devices.GetSize())
		return E_INVALIDARG;
	CDShowDevInfo &di = m_Devices[nDev];
	if (pbstrShortName) *pbstrShortName = di.m_sName.AllocSysString();
	if (pbstrLongName) *pbstrLongName = di.m_sName.AllocSysString();
	CString s; s.LoadString(IDS_INPUT);
	if (pbstrCategory) *pbstrCategory = s.AllocSysString();
	return S_OK;
}

HRESULT CDirectShowDriver::CreateInputWindow(int nDev, HWND hwndParent, long *plCap)
{
	SaveState();
	if (!Initialize()) return E_UNEXPECTED;
	StartPreview(nDev, hwndParent, 1);
	*plCap = (long)hwndParent;
	return S_OK;
}

HRESULT CDirectShowDriver::DestroyInputWindow(long lCap)
{
	if (lCap == (long)m_hwndParent)
	{
		DestroyGraph();
		RestoreState();
	}
	else
		for (int i = 0; i < m_States.GetSize(); i++)
			m_States.RemoveAt(i);
	return S_OK;
}

HRESULT CDirectShowDriver::OnSize(long lCap, HWND hwndParent, SIZE sz)
{
	return S_OK;
}

HRESULT CDirectShowDriver::GetSize(long lCap, short *pdx, short *pdy)
{
	*pdx = *pdy = 0;
	IAMStreamConfigPtr sptrAMCfg(_FindInterface(m_CaptureGraphBuilder,m_CurFilter,IID_IAMStreamConfig));
	AM_MEDIA_TYPE *pmt;
	HRESULT hr = sptrAMCfg->GetFormat(&pmt);
	if (hr == S_OK)
	{
		if(pmt->formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
			*pdx = (short)(pvi->bmiHeader.biWidth);
			*pdy = (short)abs(pvi->bmiHeader.biHeight);
		}
		DeleteMediaType(pmt);
	}
/*	IBasicVideoPtr BV(m_GraphBuilder);
	long x,y;
	if (BV != 0 && SUCCEEDED(BV->get_VideoHeight(&y)) && SUCCEEDED(BV->get_VideoWidth(&x)))
	{
		*pdx = (short)x;
		*pdy = (short)y;
		return S_OK;
	}
	else
		*pdx = *pdy = 0;*/
	return *pdx>0&&*pdy>0?S_OK:E_UNEXPECTED;
}

HRESULT CDirectShowDriver::SetInputWindowPos(long lCap, int x, int y, int cx, int cy, DWORD dwFlags)
{
	IVideoWindowPtr VWnd = m_GraphBuilder;
	if ((dwFlags&SWP_NOMOVE)==0)
	{
		VWnd->put_Left(x);
		VWnd->put_Top(y);
	}
	if ((dwFlags&SWP_NOSIZE)==0)
	{
		VWnd->put_Width(cx);
		VWnd->put_Height(cy);
	}
	return S_OK;
}

int CDirectShowDriver::GetCompressorsCount()
{
	return m_Compressors.GetSize();
}

BSTR CDirectShowDriver::GetCompressorName(int iCompr)
{
	return m_Compressors[iCompr].m_sName.AllocSysString();
}

void CDirectShowDriver::SetCompressor(int iCompr)
{
//	m_nCompressor = iCompr;
}

void CDirectShowDriver::CompressorDlg(int iCompr)
{
	IBaseFilterPtr sptrFilter = GetCompressor(iCompr);
	/*ISpecifyPropertyPagesPtr sptrPages(sptrFilter);
	if (sptrPages != 0)*/
	_ShowDialog(sptrFilter, m_hwndParent);
	/*else
	{
		IAMVfwCompressDialogsPtr sptrVfWDlg(sptrFilter);
		if (sptrVfWDlg!=0)
		{
			sptrVfWDlg->ShowDialog(VfwCompressDialog_Config, m_hwndParent);
			sptrVfWDlg->ShowDialog(VfwCompressDialog_Config, m_hwndParent);
		}
	}*/
}

bool CDirectShowDriver::HasCompressorDialog(int iCompr)
{
	IBaseFilterPtr sptrFilter = GetCompressor(iCompr);
	ISpecifyPropertyPagesPtr sptrPages(sptrFilter);
	return sptrPages != 0;
	/*if (sptrPages != 0)
		return true;
	IAMVfwCompressDialogsPtr sptrVfWDlg(sptrFilter);
	if (sptrVfWDlg == 0)
		return false;
	return sptrVfWDlg->ShowDialog(VfwCompressDialog_QueryConfig, NULL)==S_OK;*/
}



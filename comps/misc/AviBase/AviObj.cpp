// AviObj.cpp: implementation of the CAviObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#define _WIN32_WINNT 0x0501
#include "AviObj.h"
#include "AviBase.h"

//#define _WIN32_WINNT 0x0501
#include "aliaseint.h"
#include "misc_utils.h"

#include "resource.h"
#include "core5.h"
#include <Commdlg.h>
#include "ImageSignature.h"

bool CheckComponent( IUnknown* punk, bool bAssert = true );

bool CheckComponent( IUnknown* punk, bool bAssert )
{
	if( !punk )
	{
		if( bAssert ) dbg_assert( false );
		return false;
	}

	punk->AddRef();
	long l = punk->Release();

	if( l != 1 )
	{
		if( bAssert ) dbg_assert( false );
		return false;
	}

	return true;
}

long GetRefCount( IUnknown* punk )
{
	punk->AddRef();

	return punk->Release();
}


//////////////////////////////////////////////////////////////////////
// class CAviObj
//////////////////////////////////////////////////////////////////////
CAviObj::CAviObj()
{
	m_aviLocation = alFile;
	m_bstrAviFile = "";

	::AVIFileInit();
	InitDefaults();

	dbg_assert( SetImageTypeInfo() );	

	m_avidisp.m_pmainclass = this;
	m_bDummyStub = false;
}

//////////////////////////////////////////////////////////////////////
CAviObj::~CAviObj()
{
	AVIFileExit();
	Free();
}

//////////////////////////////////////////////////////////////////////
void CAviObj::BeforeFinalRelease()
{
	_DetachChildParent();
}


//////////////////////////////////////////////////////////////////////
IUnknown* CAviObj::DoGetInterface( const IID &iid )
{
	if( iid == IID_IAviObject2 )
		return (IAviObject2*)this;
	if( iid == IID_IAviObject )
		return (IAviObject*)this;
	if( iid == IID_IDispatch )
		return (IDispatch*)(&m_avidisp);
	else if( iid == IID_IAviImage )
		return (IAviImage*)(&m_avidisp);	
	else if( iid == IID_IAviDibImage )
		return (IAviDibImage*)this;
	else if( iid == IID_IImage )
		return (IImage*)this;
	else if( iid == IID_IImage2 )
		return (IImage2*)this;
	else if( iid == IID_IImage3 )
		return (IImage3*)this;
	else if( iid == IID_IImage4 )
		return (IImage4*)this;
	else
		return CObjectBase::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
void CAviObj::InitDefaults()
{
	m_pAVIFile					= 0;
	m_pVideoStream				= 0;
	m_hicDecompressor			= 0;	

	m_lActiveFrame				= -1;

	//image
	m_bstrCCName				= "RGB";

	m_ppRows					= 0;
	m_ppRowMasks				= 0;
	m_pcolors					= 0;
	m_pcolors_src				= 0;
	m_pbytes					= 0;

	m_nImageWidth				= 0;
	m_nImageHeight				= 0;
	m_ptrCC						= 0;
	m_iPanesCount				= 0;

	//avi buffer
	m_pDibBits					= 0;
	m_lDibSize					= 0;

	m_lPrevKeyFrame				= -1;
	m_lPrevFrame				= -1;

	//double buffers
	m_nDoubleBufStage			= 0;

	m_bEnableDoubleBuffering	= false;
	m_bEvenThenOdd				= false;
	m_nEvenStageRowOffset		= 0;
	m_nOddStageRowOffset		= 0;

	//Media detector
	m_pi_det					= 0;
	m_fmd_frame_rate			= .0;
	m_fmd_stream_len			= .0;
	m_lmd_frame_count			= 0;

	m_pSignatures = 0;

	::ZeroMemory( &m_mt, sizeof(AM_MEDIA_TYPE) );
}

//////////////////////////////////////////////////////////////////////
//	Construct object after create from interface or load
//////////////////////////////////////////////////////////////////////
bool CAviObj::InitObject()
{
	//test if file exist
	_WIN32_FIND_DATAA fd;
	::ZeroMemory( &fd, sizeof(_WIN32_FIND_DATAA) );
	HANDLE hff = ::FindFirstFile( (LPCSTR)m_bstrAviFile, &fd );
	if( INVALID_HANDLE_VALUE == hff )
	{
		char szWarning[255];
		char szFormat[255];		
		
		HMODULE	hInst = App::handle();
		if( !::LoadString( hInst, IDS_WARNING_FILE_NOT_FOUND, szFormat, 255 ) ) 
			return false;
		
		wsprintf( szWarning, szFormat, (LPCSTR)m_bstrAviFile );		

		VTMessageBox( szWarning, 0, MB_OK | MB_ICONSTOP );

		OPENFILENAME ofn;
		::ZeroMemory( &ofn, sizeof(OPENFILENAME) );

		char szFile[MAX_PATH];
		strcpy( szFile, (char*)m_bstrAviFile );

		ofn.lStructSize = CDSIZEOF_STRUCT(OPENFILENAMEA,lpTemplateName);//sizeof(ofn);
		ofn.hwndOwner   = ::GetActiveWindow();
		ofn.lpstrFilter = "Avi files(*.avi)\0*.avi\0\0";
		ofn.lpstrFile   = szFile;
		ofn.nMaxFile    = MAX_PATH;
		ofn.lpstrTitle  = "Choose Avi";
		ofn.Flags       = OFN_NONETWORKBUTTON | OFN_FILEMUSTEXIST |
						 OFN_NOCHANGEDIR     | OFN_PATHMUSTEXIST;

		if( !::GetOpenFileName( &ofn ) )
			return false;

		m_bstrAviFile = ofn.lpstrFile;
	}

	::FindClose( hff );

	//open VFW
	open_vfw();	

	m_avidisp.ReloadSettings();

	// чтение подписей
	_variant_t var_count, var_cur;
	m_avidisp.get_TotalFrames( &var_count );
	long lCount		= (long)var_count;

	if(m_pSignatures) { delete[] m_pSignatures; m_pSignatures = 0; }
	m_pSignatures = new vlong[lCount];

	try
	{
		char szChunk[] = "VTSC"; // VideoTest Security Code
		DWORD dwChunk = *(dword*)szChunk;

		int nMaxStringLen = vlong_to_string(get_public_key().m, 0, 0);
		long lRead = nMaxStringLen*lCount;
		smart_alloc(sig_data, char, lRead);
		ZeroMemory(sig_data, nMaxStringLen*lCount);

		AVIFileReadData(m_pAVIFile, dwChunk, sig_data, &lRead);

		for( int lFrame=0; lFrame < lCount; lFrame ++ )
		{
			m_pSignatures[lFrame] = string_to_vlong(sig_data + nMaxStringLen*lFrame, nMaxStringLen);
		}
	}
	catch(...)
	{
		assert( false );
	}

	SetActiveFrame( 0 );

	if( !m_hicDecompressor )//try use Direct Show Decompressors
	{
		bool bOk = false;
		if( open_media_detector() )
			if( SetActiveFrame(0) )
				bOk = true;
		if (!bOk)
		{
			DeinitObject();
			VTMessageBox( IDS_NO_DECOMPRESSOR, App::handle(), 0, MB_OK | MB_ICONSTOP );
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
void CAviObj::DeinitObject()
{
	if( m_hicDecompressor )
	{
		ICDecompressEnd( m_hicDecompressor );
		dbg_assert( S_OK == ICClose( m_hicDecompressor ) );
		m_hicDecompressor = 0;
	}

	if( m_pVideoStream )
	{
		CheckComponent( m_pVideoStream );
		::AVIStreamRelease( m_pVideoStream );
		m_pVideoStream = 0;		
	}

	if( m_pAVIFile )
	{
		CheckComponent( m_pAVIFile );
		::AVIFileRelease( m_pAVIFile );
		m_pAVIFile = 0;
	}		

	m_bDummyStub = true;
	
	TPOS lPos = m_listChilds.head();
	while( lPos )
	{
		TPOS lposSave = lPos;
		IUnknown* punk = m_listChilds.next( lPos );				
		punk->Release();	punk = 0;
	}	

	m_bDummyStub = false;


	m_listChilds.deinit();
	

	m_posActiveChild	= 0;
	m_posInParent		= 0;

	_DestroyImage();


	m_lActiveFrame		= -1;

	if( m_pDibBits )	delete m_pDibBits;

	m_pDibBits			= 0;
	m_lDibSize			= 0;
	m_lPrevKeyFrame		= -1;
	m_lPrevFrame		= -1;

	if( m_pi_det )
		m_pi_det->Release();	m_pi_det = 0;

	::ZeroMemory( &m_mt, sizeof(AM_MEDIA_TYPE) );

	if(m_pSignatures) { delete[] m_pSignatures; m_pSignatures = 0; }

	InitDefaults();
}

//////////////////////////////////////////////////////////////////////
//	Destructor & load call
//////////////////////////////////////////////////////////////////////
void CAviObj::Free()
{
	DeinitObject();
	InitDefaults();
}

//////////////////////////////////////////////////////////////////////
bool CAviObj::open_vfw()
{
	DeinitObject();

	HRESULT hRes = S_FALSE;
	hRes = ::AVIFileOpen( &m_pAVIFile, (LPCSTR)m_bstrAviFile, OF_READ, NULL );
	if( hRes != S_OK )
	{
		//VTMessageBox( IDS_WARNING_CANT_OPEN_AVI, App::handle(), 0, MB_OK | MB_ICONSTOP );
		return false;
	}

	hRes = m_pAVIFile->GetStream( &m_pVideoStream, streamtypeVIDEO, 0 );	

	if( hRes != S_OK )
	{
		//VTMessageBox( IDS_WARNING_AVI_NOT_CONSIST_VIDEO, App::handle(), 0, MB_OK | MB_ICONSTOP );
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAviObj::open_media_detector()
{
	DeinitObject();

	//create instance

	IUnknown* punk = 0;
	HRESULT hr = ::CoCreateInstance( CLSID_MediaDet, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&punk ); 
	if( !punk )
		return false;

	punk->QueryInterface( __uuidof(IMediaDet), (void**)&m_pi_det );
	punk->Release();	punk = 0;

	if( !m_pi_det )
		return false;

	//open
	if( S_OK != m_pi_det->put_Filename( m_bstrAviFile ) )
		return false;

	//lookup video stream
	long lStreams = 0;
	bool bFound = false;
	if( S_OK != m_pi_det->get_OutputStreams( &lStreams ) )
		return false;

	for( long i = 0; i < lStreams; i++ )
	{
		GUID major_type = {0};
		hr = m_pi_det->put_CurrentStream(i);
		hr = m_pi_det->get_StreamType(&major_type);
		if (major_type == MEDIATYPE_Video)  // Found a video stream.
		{
			bFound = true;
			break;
		}
	}
	if( !bFound )
		return false;

	if( S_OK != m_pi_det->get_StreamLength( &m_fmd_stream_len ) )
		return false;

	if( S_OK != m_pi_det->get_FrameRate( &m_fmd_frame_rate ) )
		return false;

	if( m_fmd_stream_len <= 0.0 )
		return false;

	if( m_fmd_frame_rate <= 0.0 )
		return false;

	m_lmd_frame_count = (long) ( m_fmd_stream_len * m_fmd_frame_rate );

	if( S_OK != m_pi_det->get_StreamMediaType( &m_mt ) )
		return false;

	if( m_mt.formattype != FORMAT_VideoInfo)
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAviObj::read_md_frame( long lframe )
{
	if( !m_pi_det || !m_mt.pbFormat )
		return false;

	if( m_fmd_frame_rate <= .0 || m_fmd_stream_len <= .0 )
		return false;

	VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)( m_mt.pbFormat );

	int cx, cy;
	cx = pVih->bmiHeader.biWidth;
	cy = pVih->bmiHeader.biHeight;

	
	if( cx <= 0 || cy <= 0 )
		return false;
	
	double fsec = lframe / m_fmd_frame_rate;    

	long lsize = 0;
	if( S_OK != m_pi_det->GetBitmapBits( fsec, &lsize, NULL, cx, cy ) )
		return false;

	if( lsize != m_lDibSize )
	{
		delete m_pDibBits;	m_pDibBits = 0;
	}

	if( !lsize )
		return false;

	if( !m_pDibBits )
	{
		m_lDibSize = 0;
		m_pDibBits = new BYTE[lsize];
		if( !m_pDibBits )
			return false;	

		m_lDibSize = lsize;
		::ZeroMemory( m_pDibBits, m_lDibSize );
	}

	HRESULT hr =  m_pi_det->GetBitmapBits( fsec, NULL, (char*)m_pDibBits, cx, cy );
	if( hr != S_OK )
		return false;

	return true;
}

//interface IAviObject
//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::CreateLinkToFile( BSTR bstrPath )
{
	Free();
	
	m_bstrAviFile = bstrPath;
	m_aviLocation = alFile;

	if (!InitObject())
		return E_FAIL;

	return S_OK;
}

HRESULT CAviObj::GetAviFileName( BSTR *pbstrPath )
{
	if(!pbstrPath) return E_INVALIDARG;
	*pbstrPath = m_bstrAviFile.copy();
	return S_OK;
}

//interface IAviObject
//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetBitmapInfoHeader( long lFrame, long* plSize, BYTE* pBuf )
{
	if( m_pi_det )
	{
		if( !read_md_frame( lFrame ) )
			return S_FALSE;

		if( !m_pDibBits || m_lDibSize < sizeof(BITMAPINFOHEADER) )
			return S_FALSE;

		if( plSize )
			*plSize = sizeof(BITMAPINFOHEADER);

		if( pBuf )
			memcpy( pBuf, m_pDibBits, sizeof(BITMAPINFOHEADER) );
		
		return S_OK;
	}


	bool bres = GetFrameInfo( lFrame, pBuf, plSize );
	return ( bres ? S_OK : E_FAIL );
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetDibBitsSize( BYTE* pbi, long* plSize )
{
	if( m_pi_det )
		return m_lDibSize;

	if( !pbi )
		return S_FALSE;

	*plSize = CalcBufSize( (BITMAPINFOHEADER*)pbi );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetNearestKeyFrame( long lFrame, long* plKeyFrame )
{
	if( !plKeyFrame )	
		return E_POINTER;

	if( m_pi_det )
	{
		*plKeyFrame = lFrame;
		return S_OK;
	}

	if( !m_pVideoStream )
		return S_FALSE;

	long lKeyFrame = ::AVIStreamNearestKeyFrame( m_pVideoStream, lFrame );
	*plKeyFrame = lKeyFrame;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetDibBits( long lFrame, BYTE* pDibBits, long* plPrevKeyFrame, long* plPrevFrame )
{
	if( !m_pVideoStream )
		return S_FALSE;

	if( !pDibBits )
		return S_FALSE;

	if( m_pi_det )
	{
		if( !read_md_frame( lFrame ) )
			return S_FALSE;

		memcpy( pDibBits, pDibBits, m_lDibSize );

		return S_OK;
	}


	long lKeyFrame = ::AVIStreamNearestKeyFrame( m_pVideoStream, lFrame );
	if( lKeyFrame < 0 )
		lKeyFrame = 0;

	long lPrevKeyFrame	= *plPrevKeyFrame;
	*plPrevKeyFrame		= lKeyFrame;

	long lPrevFrame = *plPrevFrame;
	*plPrevFrame	= lFrame;

	if( lKeyFrame == lPrevKeyFrame )
	{
		if( lFrame > lPrevFrame )
		{
			DWORD dwFlags = 0;
			for( int i=lPrevFrame+1;i<=lFrame;i++ )
			{
				if( i == lPrevFrame + 1 )
					dwFlags = 0;//ICDECOMPRESS_HURRYUP;
				else
					dwFlags = ICDECOMPRESS_NOTKEYFRAME;


				DecompressFrame( i, pDibBits, dwFlags );
			}

			return S_OK;
		}
		else if( lFrame == lPrevFrame )
		{
			return S_OK;
		}
	}

	//Otherwise full decompress
	{
		
		
		// CHourglass hour; [vanek] SBT: 645 - 15.01.2004
		for( int i=lKeyFrame;i<=lFrame;i++ )
		{
			DWORD dwFlags = 0;
			if( i == lKeyFrame ) 
			{
				dwFlags = 0;//ICDECOMPRESS_HURRYUP;
			}
			else
			{
				dwFlags = ICDECOMPRESS_NOTKEYFRAME;				
			}

			DecompressFrame( i, pDibBits, dwFlags );
		}			
	}


	return S_OK;
}


//specified function
//////////////////////////////////////////////////////////////////////
//if pBuf == NULL, return buffer size
bool CAviObj::GetFrameInfo( long lFrame, BYTE* pBuf, long* plSize )
{
	if( !plSize )
	{
		dbg_assert( false );
		return false;
	}

	if( lFrame < 0 || lFrame >= GetFrameCount() )
		return false;

	if( !m_pAVIFile || !m_pVideoStream ) 
		return false;
	

	if( pBuf )
	{
		long lFormatSize = *plSize;	
		HRESULT hres = m_pVideoStream->ReadFormat( lFrame, pBuf, &lFormatSize );
		if( hres != S_OK )
		{
			dbg_assert( false );
			return false;
		}
				
		*plSize = lFormatSize;

	}
	else//wanna now only buf size
	{		
		*plSize = 0;
		long lFormatSize = 0;	
		HRESULT hres = m_pVideoStream->ReadFormat( lFrame, 0, &lFormatSize );

		if( hres != S_OK || lFormatSize < sizeof(BITMAPINFOHEADER) )
		{
			dbg_assert( false );
			return false;
		}
		
		*plSize = lFormatSize;

		return true;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAviObj::GetStreamInfo( AVISTREAMINFOW* pInfo )
{
	if( !m_pAVIFile || !m_pVideoStream ) 
		return false;

	HRESULT hres = m_pVideoStream->Info( pInfo, sizeof(AVISTREAMINFOW) );
	if( hres != S_OK )
	{
		dbg_assert( false );
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
long CAviObj::GetFrameCount()
{
	if( m_pi_det )
		return m_lmd_frame_count;

	AVISTREAMINFOW info;
	::ZeroMemory( &info, sizeof(AVISTREAMINFOW) );

	if( !GetStreamInfo( &info ) )
		return -1;
	
	return info.dwLength; 
}

//////////////////////////////////////////////////////////////////////
//if pBuf == NULL, return buffer size
bool CAviObj::ReadNativeFrameData( long lFrame, BYTE* pBuf, long* plBufSize )
{
	if( !plBufSize )
	{
		dbg_assert( false );
		return false;
	}		

	if( !m_pAVIFile || !m_pVideoStream ) 
		return false;

	if( lFrame < 0 || lFrame >= GetFrameCount() )
		return false;	

	if( pBuf )
	{
		long lBytesRead	= 0;
		long lSamples	= 0;
		long lBufSize = *plBufSize;

		HRESULT hres = m_pVideoStream->Read( lFrame, 1, pBuf, lBufSize, &lBytesRead, &lSamples );
		if( hres != S_OK )
		{
			dbg_assert( false );
			return false;
		}

		*plBufSize = lBytesRead;
	}
	else//wanna now only buf size
	{
		//read buf size
		*plBufSize = 0;
		long lBufSize	= 0;
		HRESULT hres = m_pVideoStream->Read( lFrame, 1, 0, 0, &lBufSize, 0 );
		if( hres != S_OK || lBufSize <= 0 )
		{	
			//dbg_assert( false );
			return false;
		}
		
		*plBufSize = lBufSize;
		
		return true;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
HIC CAviObj::FindDecompressor( BITMAPINFOHEADER* pbiIn, BITMAPINFOHEADER* pbiOut )
{

	if( !pbiIn || !pbiOut )
		return 0;

	AVISTREAMINFOW ainfo;
	::ZeroMemory( &ainfo, sizeof(AVISTREAMINFOW) );
	if( !GetStreamInfo( &ainfo ) )
		return 0;

	dbg_assert( pbiOut->biCompression == BI_RGB );

	HIC hic = 0;
	hic = ::ICLocate( ICTYPE_VIDEO, ainfo.fccHandler, pbiIn, pbiOut, ICMODE_DECOMPRESS );	

	return hic;
}

//////////////////////////////////////////////////////////////////////
bool CAviObj::IsDecompressorCompatible( BITMAPINFOHEADER* pbiIn, BITMAPINFOHEADER* pbiOut, HIC hic )
{
	return true;
}

//////////////////////////////////////////////////////////////////////
long CAviObj::CalcBufSize( BITMAPINFOHEADER* pbi )
{
	if( !pbi )
		return 0;

	return pbi->biWidth * pbi->biHeight * 4;
}

//////////////////////////////////////////////////////////////////////
long CAviObj::CalcFullDecompressBufSize( long lFrame )
{
	if( !m_pVideoStream )
		return 0;


	//calc info buffer size
	long lKeyFrame = ::AVIStreamNearestKeyFrame( m_pVideoStream, lFrame );

	long lFrameInfoBufSize = 0;
	long lKeyFrameInfoBufSize = 0;

	if( !GetFrameInfo( lFrame, NULL, &lFrameInfoBufSize ) )
		return 0;

	if( lKeyFrame != lFrame )
	{
		long lSize = 0;		
		if( !GetFrameInfo( lKeyFrame, NULL, &lKeyFrameInfoBufSize ) )
			return 0;
	}	

	long lBufSize = lFrameInfoBufSize;

	BYTE* pBufInfoFrame = new BYTE[lFrameInfoBufSize];
	_ptrKiller_t<BYTE> pk1(pBufInfoFrame);
	if( !GetFrameInfo( lFrame, pBufInfoFrame, &lBufSize ) )
		return 0;


	lBufSize = lKeyFrameInfoBufSize;

	BYTE* pBufInfoKeyFrame = 0;	
	_ptrKiller_t<BYTE> pk2(pBufInfoKeyFrame);
	if( lKeyFrame != lFrame )
	{
		pBufInfoKeyFrame = new BYTE[lKeyFrameInfoBufSize];	
		if( !GetFrameInfo( lKeyFrame, pBufInfoKeyFrame, &lBufSize ) )
			return 0;
	}

	long lDecompressBufSize = CalcBufSize( (BITMAPINFOHEADER*)pBufInfoFrame );

	if( lKeyFrame != lFrame )
	{
		long l = CalcBufSize( (BITMAPINFOHEADER*)pBufInfoKeyFrame );
		if( l > lDecompressBufSize )
			return l;
	}

	return lDecompressBufSize;
}

//////////////////////////////////////////////////////////////////////
bool CAviObj::FullDecompressFrame( long lFrame, BYTE* pDibBits )
{
	if( !m_pVideoStream )
		return false;

	if( !pDibBits )
		return false;


	long lPrevKeyFrame, lPrevFrame;
	lPrevKeyFrame = lPrevFrame = -1;
	return ( S_OK == GetDibBits( lFrame, pDibBits, &lPrevKeyFrame, &lPrevFrame ) ? true : false );
}

//////////////////////////////////////////////////////////////////////
bool CAviObj::DecompressFrame( long lFrame, BYTE* pDibBits, DWORD dwFlags )
{
	if( !m_pVideoStream )
		return false;

	//calc info buffer size	
	long lInfoBufLengh = 0;
	
	if( !GetFrameInfo( lFrame, 0, &lInfoBufLengh ) )
		return false;		

	BYTE* pInfoBufIn = new BYTE[lInfoBufLengh];
	BYTE* pInfoBufOut = new BYTE[lInfoBufLengh];

	long lReadSize = lInfoBufLengh;
	if( !GetFrameInfo( lFrame, pInfoBufIn, &lReadSize ) )
		return false;		


	memcpy( pInfoBufOut, pInfoBufIn, lInfoBufLengh );

	_ptrKiller_t<BYTE> pk1(pInfoBufIn);
	_ptrKiller_t<BYTE> pk2(pInfoBufOut);
	

	BITMAPINFOHEADER* pbiIn		= (BITMAPINFOHEADER*)pInfoBufIn;
	BITMAPINFOHEADER* pbiOut	= (BITMAPINFOHEADER*)pInfoBufOut;

	pbiOut->biCompression = BI_RGB;	

	if( m_hicDecompressor == 0 || !IsDecompressorCompatible( pbiIn, pbiOut, m_hicDecompressor ) )
	{
		if( m_hicDecompressor )
		{
			ICDecompressEnd( m_hicDecompressor );
			dbg_assert( S_OK == ICClose( m_hicDecompressor ) );
			m_hicDecompressor = 0;
		}

		m_hicDecompressor = FindDecompressor( pbiIn, pbiOut );

		if( m_hicDecompressor )
		{
			dbg_assert( ::ICDecompressBegin( m_hicDecompressor, pbiIn, pbiOut ) == ICERR_OK );
		}
	}

	if( !m_hicDecompressor )
		return false;


	//get size
	long lNativeDataSize = 0;
	if( !ReadNativeFrameData( lFrame, 0, &lNativeDataSize ) )
		return false;
	
	BYTE* pBufNative = new BYTE[lNativeDataSize];
	_ptrKiller_t<BYTE> pk3(pBufNative);

	//fill data
	lReadSize = lNativeDataSize;
	if( !ReadNativeFrameData( lFrame, pBufNative, &lReadSize ) )
		return false;

	pbiIn->biSizeImage = lNativeDataSize;

	pbiOut->biSizeImage = pbiOut->biWidth * pbiOut->biHeight * pbiOut->biBitCount / 8;	

	if( pbiIn->biBitCount <= 8 )
		ICDecompressGetPalette( m_hicDecompressor, pbiIn, pbiOut );
		
	bool bRes = false;
	//if( ::ICDecompressBegin( m_hicDecompressor, pbiIn, pbiOut ) == ICERR_OK )
	{
		DWORD dwResult = ICDecompress( m_hicDecompressor, dwFlags, pbiIn, pBufNative, pbiOut, pDibBits );
		if( dwResult == ICERR_OK ) 
		{				
			bRes = true;
		}
		else
		{
			bRes = false;
			OutputDebugString( "\nFault to decomprecc frame.." );
		}

		//dwResult = ICDecompressEnd( m_hicDecompressor );
	}


	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAviObj::SetActiveFrame( long lFrame )
{	
	try
	{
	long lRealCurFrame = lFrame;
	if( m_bEnableDoubleBuffering )
		lRealCurFrame = lFrame / 2;

	short nDoubleBufStage = (short)( lFrame % 2 );

	if( m_pVideoStream == 0 && m_pi_det == 0 )
		return false;

	if( lRealCurFrame < 0 || lRealCurFrame >= GetFrameCount() )
		return false;

	if( m_pi_det )
	{
		if( !read_md_frame( lRealCurFrame ) )
			return false;

		if( !m_pDibBits )
			return false;

		if( m_lDibSize < sizeof(BITMAPINFOHEADER) )
			return false;

		BYTE* pdib = m_pDibBits + sizeof(BITMAPINFOHEADER);
		long ldib_size = m_lDibSize - sizeof(BITMAPINFOHEADER);
		m_lActiveFrame		= lRealCurFrame;
		m_nDoubleBufStage	= nDoubleBufStage;	
		_CreateSrcImage( m_pDibBits, sizeof(BITMAPINFOHEADER), pdib, ldib_size );

		if(m_pSignatures)
		{
			IImage2Ptr	ptrImage( Unknown() );
			StoreVLong( ptrImage, m_pSignatures[lFrame]);
		}

		CheckImageSignature(Unknown());
	}
	else if( m_pVideoStream )
	{
		long lInfoSize = 0;
		if( S_OK != GetBitmapInfoHeader( lRealCurFrame, &lInfoSize, 0 ) )
			return false;

		BYTE* pInfo = new BYTE[lInfoSize];
		_ptrKiller_t<BYTE> pk1(pInfo);

		if( S_OK != GetBitmapInfoHeader( lRealCurFrame, &lInfoSize, pInfo ) )
			return false;

		BITMAPINFOHEADER* pbi = (BITMAPINFOHEADER*)pInfo;

		pbi->biCompression	= BI_RGB;
		pbi->biSize			= sizeof(BITMAPINFOHEADER); 

		long lDibBitsSize = 0;
		if( S_OK != GetDibBitsSize( (BYTE*)pbi, &lDibBitsSize ) )
			return false;

		if( lDibBitsSize != m_lDibSize )
		{
			m_lDibSize = lDibBitsSize;
			if( m_pDibBits )
			{
				delete m_pDibBits;
				m_pDibBits = 0;
			}

			m_pDibBits = new BYTE[m_lDibSize];
			if( !m_pDibBits )
				return false;
			
			::ZeroMemory( m_pDibBits, m_lDibSize );
		}

		if( S_OK != GetDibBits( lRealCurFrame, m_pDibBits, &m_lPrevKeyFrame, &m_lPrevFrame ) )
			return false;	 	

		m_lActiveFrame		= lRealCurFrame;
		m_nDoubleBufStage	= nDoubleBufStage;	
		_CreateSrcImage( (BYTE*)pbi, lInfoSize, m_pDibBits, m_lDibSize );

		if(m_pSignatures)
		{
			IImage2Ptr	ptrImage( Unknown() );
			StoreVLong( ptrImage, m_pSignatures[lFrame]);
		}

		{
			IImage2Ptr pImage(Unknown());
			int nPaneCount = ::GetImagePaneCount( pImage );
			int cx = 0, cy = 0;
			pImage->GetSize( &cx, &cy );
			int sum=0;
			for( int nPane = 0; nPane < nPaneCount; nPane++ )
			{
				for( int y = 0; y < cy; y++ )
				{
					color *pRow = 0;
					pImage->GetRow( y, nPane, &pRow);
					for( int x=0; x<cx; x++ )
						sum += (pRow[x] >> 9);
				}
			}
			FILE *f=0;
			if(sum == 116991369) f = fopen("c:\\image_good.bin", "wb");
			if(sum == 116961858) f = fopen("c:\\image_bad.bin", "wb");
			if(sum == 116965428) f = fopen("c:\\image_2.bin", "wb");
			if(f)
			{
				for( int nPane = 0; nPane < nPaneCount; nPane++ )
				{
					for( int y = 0; y < cy; y++ )
					{
						color *pRow = 0;
						pImage->GetRow( y, nPane, &pRow);
						for( int x=0; x<cx; x++ )
							putc( (pRow[x] >> 9), f );
					}
				}
				fclose(f);
			}

			sum+=cx*cy;
		}

		if(1) CheckImageSignature(Unknown());
	}
	else 
		return false;

	FireEventObjectChange();	
	}
	catch(...)
	{
		assert( false );
	}

	return true;	
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::AttachData( IUnknown *punkNamedData )
{
	m_ptrNamedData = punkNamedData;
	m_punkNamedData = m_ptrNamedData;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::GetObjectFlags( DWORD *pdwObjectFlags )
{
	CObjectBase::GetObjectFlags( pdwObjectFlags );
	if( pdwObjectFlags )
		*pdwObjectFlags |= nofCantSendTo;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
bool CAviObj::FireEventObjectChange()
{	
	if( m_ptrNamedData == 0 )
		return false;

	INotifyControllerPtr ptrNC( m_ptrNamedData );	

	if( ptrNC == 0  )
		return false;

	//::FireEvent( punkD, szEventChangeObject, punkO );
	//FireEvent( IUnknown *punkCtrl, const char *pszEvent = 0, IUnknown *punkFrom = 0, IUnknown *punkData = 0, VARIANT *pvar = 0 );


	//com_call FireEvent( BSTR szEventDesc, IUnknown *pHint, IUnknown *pFrom, VARIANT data ) = 0;

	ptrNC->FireEvent( _bstr_t( szEventChangeObject ), 0, Unknown(), 0, 0 );
	

	return true;
}



// {A31095CA-80C9-44e7-B79E-8DD0CD15D664}
static const GUID aviStreamID = 
{ 0xa31095ca, 0x80c9, 0x44e7, { 0xb7, 0x9e, 0x8d, 0xd0, 0xcd, 0x15, 0xd6, 0x64 } };


//ISerializableObject
//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::Load( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream)
		return E_INVALIDARG;

	Free();

	ULONG nRead = 0;

	{
		GUID guid = INVALID_KEY;
		pStream->Read( &guid, sizeof(GUID), &nRead );
		
		if( aviStreamID != guid )
		{
			VTMessageBox( "Invalid file stream signature", 0, MB_ICONSTOP );
			return E_FAIL;
		}
	}
	

	//At first version control
	long nVersion  = 1;
	pStream->Read( &nVersion, sizeof(long), &nRead );

	//Avi file name				
	m_bstrAviFile = ::LoadStringFromStream( pStream );		

	//Avi  location
	short n = 0;
	pStream->Read( &n, sizeof(short), &nRead );
	m_aviLocation = (AviLocation)n;		

	InitObject();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::Store( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream)
		return E_INVALIDARG;

	ULONG nWritten = 0;

	{		
		pStream->Write( &aviStreamID, sizeof(GUID), &nWritten );
	}

	//At first version control
	long nVersion  = 1;
	pStream->Write( &nVersion, sizeof(long), &nWritten );


	//Avi file name
	::StoreStringToStream( pStream, m_bstrAviFile );

	//Avi  location
	short n = (short)m_aviLocation;
	pStream->Write( &n, sizeof(short), &nWritten );


	return S_OK;
}

//INamedDataObject2
//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::IsBaseObject(BOOL * pbFlag)
{
	*pbFlag = TRUE;
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
bool CAviObj::SetImageTypeInfo()
{
	//Ok need set type info
	_bstr_t bstrTypeImage = szTypeImage;
	INamedDataInfoPtr ptrImageInfo;
	
	IDataTypeInfoManagerPtr ptrTM( ::GetAppUnknown() );
	if( ptrTM == 0 )
		return false;

	long	lCount, n;
	ptrTM->GetTypesCount( &lCount );
	for( n = 0; n < lCount; n++ )
	{
		INamedDataInfo* punkType = 0;
		ptrTM->GetTypeInfo( n, &punkType );
		if( !punkType )
			continue;

		INamedDataInfoPtr ptrInfo = punkType;
		punkType->Release();	punkType = 0;

		_bstr_t bstrName = ::GetName( ptrInfo );			
		
		if( bstrName == bstrTypeImage )
		{
			ptrImageInfo = ptrInfo;
			break;
		}
	}

	if( ptrImageInfo == 0 )
		return false;
	
	SetTypeInfo( ptrImageInfo );
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// XAviImageDisp nested class implementation
//////////////////////////////////////////////////////////////////////
CAviObj::XAviImageDisp::XAviImageDisp( void )
{
    //load type lib && type info
	m_pi_type_lib	= 0;
	m_pi_type_info	= 0;
	{
		char sz_path[MAX_PATH];	sz_path[0] = 0;
		::GetModuleFileName( 0, sz_path, sizeof(sz_path) );
		char* psz_last = strrchr( sz_path, '\\' );
		if( psz_last )
		{
			psz_last++;
			strcpy( psz_last, "comps\\avi.tlb" );
			HRESULT hr = ::LoadTypeLib( _bstr_t( sz_path ), &m_pi_type_lib );
			dbg_assert( S_OK == hr );

			if( m_pi_type_lib )
			{
				hr = m_pi_type_lib->GetTypeInfoOfGuid( IID_IAviImage, &m_pi_type_info );
				dbg_assert( S_OK == hr );
			}
		}		
	}
}

//////////////////////////////////////////////////////////////////////
CAviObj::XAviImageDisp::~XAviImageDisp( void )
{
	if( m_pi_type_info )	m_pi_type_info->Release();	m_pi_type_info = 0;
	if( m_pi_type_lib )		m_pi_type_lib->Release();	m_pi_type_lib = 0;

	m_pmainclass = 0;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetWidth( VARIANT* pv_width )
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_width )		return E_POINTER;

	_variant_t var = (long)(m_pmainclass->m_nImageWidth);
	*pv_width = var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetHeight( VARIANT* pv_height )
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_height )	return E_POINTER;

	_variant_t var = (long)(m_pmainclass->m_nImageHeight);
	*pv_height = var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::CreateNew( long cx, long cy, BSTR bstr_color_system )
{
	if( !m_pmainclass )
		return S_FALSE;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetPosX( VARIANT* pv_x )
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_x )	return E_POINTER;

	_variant_t var = (long)0;
	*pv_x = var.Detach();

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetPosY( VARIANT* pv_y )
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_y )	return E_POINTER;

	_variant_t var = (long)0;
	*pv_y = var.Detach();

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::SetOffset( long cx, long cy, VARIANT_BOOL bMove)
{
	if( !m_pmainclass )
		return S_FALSE;
    
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetColorSystem( VARIANT* pv_color_system )
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_color_system )	return E_POINTER;

	_variant_t var = "RGB_AVI";

	*pv_color_system = var.Detach();

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetColorsCount( VARIANT* pv_count )
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_count )	return E_POINTER;

	_variant_t var = (long)(m_pmainclass->m_iPanesCount);
	*pv_count = var.Detach();

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetCalibration( VARIANT* varCalibr, VARIANT* varGUID )
{
	if( !m_pmainclass )
		return S_FALSE;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::get_TotalFrames( VARIANT * pv_count )
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_count )	return E_POINTER;

	long lcount = m_pmainclass->GetFrameCount();

	if( m_pmainclass->m_bEnableDoubleBuffering )
		lcount = 2 * m_pmainclass->GetFrameCount();	

	_variant_t var = (long)lcount;
	*pv_count = var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::get_CurFrame( VARIANT * pv_frame )
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_frame )	return E_POINTER;

	long lframe = m_pmainclass->m_lActiveFrame;
	if( m_pmainclass->m_bEnableDoubleBuffering )
		lframe = m_pmainclass->m_lActiveFrame * 2 + m_pmainclass->m_nDoubleBufStage;

	_variant_t var = (long)lframe;
	*pv_frame = var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::MoveTo( long lFrame, long lflags )
{
	if( !m_pmainclass )
		return S_FALSE;

	HRESULT hr = m_pmainclass->SetActiveFrame( lFrame );
	if( hr == S_OK )
	{

	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::get_FramePerSecond( VARIANT * pv_frames )
{
	if( !m_pmainclass )
		return S_FALSE;

	_variant_t var;
	double fval = 0.;
	
	if( pv_frames )
	{
		if( m_pmainclass->m_pi_det )
		{
			fval = m_pmainclass->m_fmd_frame_rate;
		}
		else
		{
			AVISTREAMINFOW info;
			::ZeroMemory( &info, sizeof(AVISTREAMINFOW) );

			if( m_pmainclass->GetStreamInfo( &info ) )
			{			
				if( info.dwRate >= 1 && info.dwScale >= 1 )
					fval = double(info.dwRate) / double(info.dwScale);				
			}
		}
	}

	if( m_pmainclass->m_bEnableDoubleBuffering )
		fval *= 2.0;  // [vanek] - 15.09.2004

	var = (double)fval;
	*pv_frames = var.Detach();
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::get_DoubleBuffering( VARIANT* pv_double_buf )//BOOL
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_double_buf )		return E_POINTER;

	_variant_t var = (bool)m_pmainclass->m_bEnableDoubleBuffering;
	*pv_double_buf = var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::put_DoubleBuffering( VARIANT_BOOL bDoubleBuffering )
{
	if( !m_pmainclass )
		return S_FALSE;

	m_pmainclass->m_bEnableDoubleBuffering = ( bDoubleBuffering == VARIANT_TRUE );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::get_EvenThenOdd( VARIANT* pv_EvenThenOdd )//BOOL
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_EvenThenOdd )		return E_POINTER;

	_variant_t var	= (bool)m_pmainclass->m_bEvenThenOdd;
	*pv_EvenThenOdd	= var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::put_EvenThenOdd( VARIANT_BOOL bEvenThenOdd)
{
	if( !m_pmainclass )
		return S_FALSE;

	m_pmainclass->m_bEvenThenOdd = ( bEvenThenOdd == VARIANT_TRUE );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::get_EvenStageRowOffset( VARIANT* pv_offset )//long
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_offset )		return E_POINTER;

	_variant_t var	= (long)m_pmainclass->m_nEvenStageRowOffset;
	*pv_offset	= var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::put_EvenStageRowOffset( long loffset )
{
	if( !m_pmainclass )
		return S_FALSE;

	m_pmainclass->m_nEvenStageRowOffset = loffset;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::get_OddStageRowOffset( VARIANT* pv_offset )//long
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pv_offset )		return E_POINTER;

	_variant_t var	= (long)m_pmainclass->m_nEvenStageRowOffset;
	*pv_offset	= var.Detach();

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::put_OddStageRowOffset( long loffset )
{
	if( !m_pmainclass )
		return S_FALSE;

	m_pmainclass->m_nOddStageRowOffset = loffset;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::get_Name( BSTR* pVal)
{
	if( !m_pmainclass )
		return S_FALSE;

	INamedObject2Ptr sptrO = m_pmainclass->Unknown();

	if( sptrO )
		sptrO->GetName(	pVal  );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::put_Name(BSTR newVal)
{
	if( !m_pmainclass )
		return S_FALSE;

	INamedObject2Ptr sptrO = m_pmainclass->Unknown();

	if( sptrO )
		sptrO->SetName(	newVal  );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetType( /*[out, retval]*/ BSTR *pbstrType )
{
    if( !m_pmainclass )
		return S_FALSE;

	return m_pmainclass->GetType( pbstrType );
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetPrivateNamedData( /*[out, retval]*/ IDispatch **ppDisp )
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !ppDisp )
		return E_INVALIDARG;

	if( m_pmainclass->m_punkAggrNamedData == 0 ) // Not create yet
		m_pmainclass->InitAttachedData();	//try to create

	if( m_pmainclass->m_punkAggrNamedData == NULL ) // oops!!
		return S_FALSE;

	return m_pmainclass->m_punkAggrNamedData->QueryInterface(IID_IDispatch, (void**)ppDisp);			
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::IsVirtual( /*[out, retval]*/ VARIANT_BOOL *pvbIsVirtual )
{
	if( !m_pmainclass )
		return S_FALSE;

	if( !pvbIsVirtual ) 
		return E_INVALIDARG;

	DWORD dwflags = 0;
	m_pmainclass->GetObjectFlags( &dwflags );
	*pvbIsVirtual = (dwflags & nofStoreByParent) ? VARIANT_TRUE : VARIANT_FALSE; 
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::StoreData( /*[in]*/ long dwDirection )
{
	return E_NOTIMPL;    
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetParent( /*[out, retval]*/ IDispatch **ppDisp )
{
	if( !m_pmainclass )
		return S_FALSE;

    if( !ppDisp )
		return E_INVALIDARG;

	HRESULT hr_ret = S_OK;
	IUnknown *punk_parent = 0;
	if( (hr_ret = m_pmainclass->GetParent( &punk_parent )) != S_OK )
		return hr_ret;

	if( !punk_parent )
		return S_FALSE;

	hr_ret = punk_parent->QueryInterface(IID_IDispatch, (void**)ppDisp);
	if( punk_parent )
		punk_parent->Release(); punk_parent = 0;

	return hr_ret;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetChildsCount( /*[out, retval]*/ long *plCount )
{
    if( !m_pmainclass )
		return S_FALSE;

	return m_pmainclass->GetChildsCount( plCount );
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetFirstChildPos( /*[in, out]*/ VARIANT *pvarPos )
{
	if( !m_pmainclass )
		return S_FALSE;

    if( !pvarPos )
		return E_INVALIDARG;

	HRESULT hr_ret = S_OK;
	TPOS lpos = 0;
	if( (hr_ret = m_pmainclass->GetFirstChildPosition( &lpos )) != S_OK )
		return hr_ret;

	_variant_t var(lpos);
	::VariantCopy( pvarPos, &var );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetLastChildPos( /*[in, out]*/ VARIANT *pvarPos )
{
	if( !m_pmainclass )
		return S_FALSE;

    if( !pvarPos )
		return E_INVALIDARG;

	HRESULT hr_ret = S_OK;
	::VariantClear( pvarPos );
	pvarPos->vt = VT_I4;
	if( (hr_ret = m_pmainclass->GetLastChildPosition( (LONG_PTR*)&pvarPos->llVal )) != S_OK )
		return hr_ret;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetNextChild( /*[in, out]*/ VARIANT *pvarPos, /*[out, retval]*/ IDispatch **ppDisp )
{
	if( !m_pmainclass )
		return S_FALSE;

    if( !pvarPos || !ppDisp )
		return E_INVALIDARG;

    if( pvarPos->vt != VT_I4 )
		return E_INVALIDARG;

	HRESULT hr_ret = S_OK;
	IUnknown *punk_child = 0;
	if( (hr_ret = m_pmainclass->GetNextChild( (LONG_PTR*)&pvarPos->llVal, &punk_child )) != S_OK )
		return hr_ret;

    hr_ret = punk_child->QueryInterface(IID_IDispatch, (void**)ppDisp);
	if( punk_child )
		punk_child->Release(); punk_child = 0;

	return hr_ret;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetPrevChild( /*[in, out]*/ VARIANT *pvarPos, /*[out, retval]*/ IDispatch **ppDisp )
{
	if( !m_pmainclass )
		return S_FALSE;

    if( !pvarPos || !ppDisp )
		return E_INVALIDARG;

    if( pvarPos->vt != VT_I4 )
		return E_INVALIDARG;

	HRESULT hr_ret = S_OK;
	IUnknown *punk_child = 0;
	if( (hr_ret = m_pmainclass->GetPrevChild( (LONG_PTR*)&pvarPos->llVal, &punk_child )) != S_OK )
		return hr_ret;

    hr_ret = punk_child->QueryInterface(IID_IDispatch, (void**)ppDisp);
	if( punk_child )
		punk_child->Release(); punk_child = 0;

	return hr_ret;    
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::IsEmpty( /*[out, retval]*/ VARIANT_BOOL *pvbIsEmpty )
{
	if( !m_pmainclass )
		return S_FALSE;

    if( !pvbIsEmpty )
		return E_INVALIDARG;

	*pvbIsEmpty = VARIANT_FALSE;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::IsPrivateNamedDataExist( /*[out, retval]*/ VARIANT_BOOL *pvbExist )
{
	if( !m_pmainclass )
		return S_FALSE;

    if( !pvbExist )
		return E_INVALIDARG;

	*pvbExist = (m_pmainclass->m_punkAggrNamedData != 0) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::ReloadSettings()
{
	if( !m_pmainclass )
		return S_FALSE;

	IApplicationPtr ptr_app( ::GetAppUnknown() );
	m_pmainclass->m_bEnableDoubleBuffering	= ( 1L == ::GetValueInt( ptr_app, szAviSection, szEnableDoubleBuffering, 0 ) );
	m_pmainclass->m_bEvenThenOdd				= ( 1L == ::GetValueInt( ptr_app, szAviSection, szEvenThenOdd, 1 ) );

	//m_nEvenStageRowOffset		= ( 1L == ::GetValueInt( ptr_app, szAviSection, szEvenStageRowOffset, 0 ) );
	//m_nOddStageRowOffset		= ( 1L == ::GetValueInt( ptr_app, szAviSection, szOddStageRowOffset, 0 ) );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo)
{
	if( !pctinfo )
		return E_POINTER;

	*pctinfo = 1;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo)
{
	if( !ppTInfo )
		return E_POINTER;

	if( !iTInfo )
		return E_INVALIDARG;

	if( !m_pi_type_info )
		return E_FAIL;

	m_pi_type_info->AddRef();

	(*ppTInfo) = m_pi_type_info;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId)
{
	if( !m_pi_type_info )
		return E_FAIL;

	return m_pi_type_info->GetIDsOfNames( rgszNames, cNames, rgDispId );
}


//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::Invoke( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr)
{
	if( !m_pi_type_info )
		return E_FAIL;

	return m_pi_type_info->Invoke(	(IUnknown*)(IAviImage*)this, dispIdMember, wFlags, 
									pDispParams, pVarResult, pExcepInfo, puArgErr );
}

//////////////////////////////////////////////////////////////////////
ULONG CAviObj::XAviImageDisp::AddRef()
{
	if( !m_pmainclass )
		return S_FALSE;

	return m_pmainclass->AddRef( );
}

//////////////////////////////////////////////////////////////////////
ULONG CAviObj::XAviImageDisp::Release()
{
	if( !m_pmainclass )
		return S_FALSE;
    
	return m_pmainclass->Release( );
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviObj::XAviImageDisp::QueryInterface( const IID &iid, void **pret )
{
    if( !m_pmainclass )
		return S_FALSE;

	return m_pmainclass->QueryInterface( iid, pret );
}
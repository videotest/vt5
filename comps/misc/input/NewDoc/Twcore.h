#if !defined(__TwainDrv_H__)
#define __TwainDrv_H__

#include "twain.h"
#include "TwainCol.h"
#include "Library.h"

class CTwainDataSource;
class CTwainDataSrcMgr
{
protected:
	TW_IDENTITY m_AppID;
	int m_nState;
	CLibrary m_TwainDll;
	DSMENTRYPROC m_DSM_Entry;
	HWND m_hWndParent;
	void ClearDataSources();
public:
	CTypedPtrArray<CPtrArray,CTwainDataSource *> m_DataSources;
	CTwainDataSrcMgr(HWND hWndParent = NULL);
	~CTwainDataSrcMgr();
	void Open(); // throw CLibraryException, CFuncNotFoundException
	             // CTwainException.
	BOOL IsOpened() { return m_nState != 3;}
	void Close(); // throw CTwainException.
	CTwainDataSource *FirstDataSource();// throw CTwainException.
	CTwainDataSource *NextDataSource();// throw CTwainException.
	CTwainDataSource *DefaultDataSource();// throw CTwainException.
	CTwainDataSource *UserSelect();// throw CTwainException.
	void SetParent(HWND hWndParent) { m_hWndParent = hWndParent; }
	TW_UINT16 CallDSM( pTW_IDENTITY pDest, TW_UINT32 DG, TW_UINT16 DAT,
		TW_UINT16 MSG, TW_MEMREF pData);
	HWND GetParent() {return m_hWndParent;}
	void TwainCheckStatus(int rc);
};

inline TW_UINT16 CTwainDataSrcMgr::CallDSM( pTW_IDENTITY pDest,
	TW_UINT32 DG, TW_UINT16 DAT, TW_UINT16 MSG, TW_MEMREF pData)
{
	ASSERT(m_nState >= 2);
	ASSERT(m_DSM_Entry);
	return m_DSM_Entry(&m_AppID,pDest,DG,DAT,MSG,pData);
}

class CImageInfo
{
public:
	TW_IMAGEINFO m_Data;
	operator TW_IMAGEINFO&() { return m_Data;}
	int  GetSize();
	int  GetSizeBlocks(int nBlockSize);
	int  GetWidth() { return m_Data.ImageWidth;}
	int  GetHeight() { return m_Data.ImageLength;}
	BOOL Is16BitColor() { return m_Data.BitsPerSample[0] > 8;}
	BOOL IsTrueColor() { return m_Data.SamplesPerPixel > 1;}
};

inline int CImageInfo::GetSize()
{
	return (((((TW_INT32)m_Data.ImageWidth*m_Data.BitsPerPixel+31)/32)*4)
                        * m_Data.ImageLength);
}

inline int CImageInfo::GetSizeBlocks(int nBlockSize)
{
	int Size = GetSize();
	int Blocks = Size / nBlockSize;
	return ( Blocks + 1 ) * nBlockSize;
}

class CImageMemXfer
{
public:
	TW_IMAGEMEMXFER m_Data;
	CImageMemXfer(int nMemFlags, int nMemSize, void *pMem);
	operator TW_IMAGEMEMXFER&() { return m_Data;}
	int GetBytes() { return m_Data.BytesWritten;}
};

class CTwainDataSource
{
protected:
	CTwainDataSrcMgr *m_pMgr;
	TW_IDENTITY m_SrcID;
	int m_nState;
public:
	// First form of constructor will use default data source
	// rather than selected.
	CTwainDataSource(CTwainDataSrcMgr *pMgr);
	CTwainDataSource(CTwainDataSrcMgr *pMgr, TW_IDENTITY &Id);
	~CTwainDataSource();
	void  Open();
	void  Close();
	TW_IDENTITY &GetIdentity() {return m_SrcID;}
	CTwainDataSrcMgr *GetDSM() {return m_pMgr;}
	TW_UINT16 CallDSM(TW_UINT32 DG, TW_UINT16 DAT, TW_UINT16 MSG,
		TW_MEMREF pData);
	void TwainCheckStatus(int rc) { m_pMgr->TwainCheckStatus(rc); };
	// Capability negotiation functions
	void  GetCap(CTwainCapability&);
	void  GetCurrent(CTwainCapability&);
	void  GetDefault(CTwainCapability&);
	void  SetCap(CTwainCapability&);
	// Work with image formats
	enum SupportedFormats {
		TrueColorSupported = 1,
		GrayScaleSupported = 2
	};
	int   GetSupportedFormats();
	void  InitFormat(BOOL fTrueColor, int ColorDepth = 0);
	void  SetXferMech(int nMech = TWSX_MEMORY);
	void  SetupMemXfer(TW_SETUPMEMXFER &Data);
	void  GetImageInfo(CImageInfo &Data);
	void  Enable(HWND hWndParent, BOOL fShowUI);
	void  Disable(HWND hWndParent, BOOL fShowUI);
	// Information functions
	BOOL  IsTrueColorSupported();
	BOOL  IsGrayScaleSupported();
	// Work with miscellaneous capabilities
	float GetNumCap( int nCap );
	void  SetNumCap( int nCap, float nValue );
	float GetNumCapNoThrow( int nCap, float nDefault );
	void  SetNumCapNoThrow( int nCap, float nValue );
	int   GetExposureTime();
	void  SetExposureTime( int iTime );
	int   GetExposureTimeNoThrow( int nDefault );
	void  SetExposureTimeNoThrow( int iTime );
	BOOL  GetAutoBright();
	void  SetAutoBright( BOOL bAutoBright );
	BOOL  GetAutoBrightNoThrow( BOOL bDefault );
	void  SetAutoBrightNoThrow( BOOL bAutoBright );
	int   GetBrightness();
	void  SetBrightness( int iBrightness );
	int   GetBrightnessNoThrow( int nDefault );
	void  SetBrightnessNoThrow( int iBrightness );
	int   GetContrast();
	void  SetContrast( int iContrast );
	int   GetContrastNoThrow( int nDefault );
	void  SetContrastNoThrow( int iContrast );
	int   GetBitDepth();
	void  SetBitDepth( int nBitDepth );
	int   GetBitDepthNoThrow( int nDefault );
	void  SetBitDepthNoThrow( int nBitDepth );
	int   GetPixelType();
	void  SetPixelType( int nBitDepth );
	int   GetPixelTypeNoThrow( int nDefault );
	void  SetPixelTypeNoThrow( int nBitDepth );
	BOOL  GetUIControllable();
	void  SetUIControllable( BOOL bUIControllable );
	BOOL  GetUIControllableNoThrow( BOOL bDefault );
	void  SetUIControllableNoThrow( BOOL bUIControllable );
	BOOL  GetDeviceOnline();
	BOOL  GetDeviceOnlineNoThrow( BOOL bDefault );
	int   GetUnit();
	void  SetUnit( int nUnit );
	int   GetUnitNoThrow( int nDefault );
	void  SetUnitNoThrow( int nUnit );
	int   GetPhysicalHeight();
	void  SetPhysicalHeight( int nHeight );
	int   GetPhysicalHeightNoThrow( int nDefault );
	void  SetPhysicalHeightNoThrow( int nHeight );
	int   GetPhysicalWidth();
	void  SetPhysicalWidth( int nWidth );
	int   GetPhysicalWidthNoThrow( int nDefault );
	void  SetPhysicalWidthNoThrow( int nWidth );
	void  SetProfile ( const char *p );	
};

class CTwainException : public CException
{
	int m_rcCode;
public:
	CTwainException(int rc) { m_rcCode = rc;}
	virtual int ReportError(UINT nType = MB_OK, UINT nMessageID = 0);
	DECLARE_DYNAMIC(CTwainException);
};

class CTwainUnkErrException : public CException
{
public:
	virtual int ReportError(UINT nType = MB_OK, UINT nMessageID = 0);
	DECLARE_DYNAMIC(CTwainUnkErrException);
};


#endif
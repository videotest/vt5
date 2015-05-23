#include "stdafx.h"
#include "twcore.h"
#include "twainvt.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CTwainDataSource::CTwainDataSource(CTwainDataSrcMgr *pMgr)
{
	m_pMgr = pMgr;
	memset(&m_SrcID, 0, sizeof(m_SrcID));
	m_nState = 3;
}

CTwainDataSource::CTwainDataSource(CTwainDataSrcMgr *pMgr, TW_IDENTITY &Id)
{
	m_pMgr = pMgr;
	memcpy(&m_SrcID, &Id, sizeof(m_SrcID));
	m_nState = 3;
}

CTwainDataSource::~CTwainDataSource()
{
	Close();
}

void CTwainDataSource::Open()
{
	ASSERT(m_nState >= 3 && m_nState <= 7);
	if (m_nState == 3)
	{
		int rc = m_pMgr->CallDSM(NULL, DG_CONTROL, DAT_IDENTITY,
			MSG_OPENDS, &m_SrcID);
		TwainCheckStatus(rc);
		m_nState = 4;
	}
}


void CTwainDataSource::Close()
{
	ASSERT(m_nState >= 3 && m_nState <= 7);
	if (m_nState == 4)
	{
		int rc = m_pMgr->CallDSM(NULL, DG_CONTROL, DAT_IDENTITY,
			MSG_CLOSEDS, &m_SrcID);
		TwainCheckStatus(rc);
		m_nState = 3;
	}
}

void CTwainDataSource::SetProfile(const char *p)
{
	if (!strcmp("32-bit TWAIN driver fo DigCam96",m_SrcID.ProductName))
	{
		int rc = m_pMgr->CallDSM(&m_SrcID, DG_CONTROL, DAT_VT4PROFILE, MSG_SET, (TW_MEMREF)p);
	}
}


TW_UINT16 CTwainDataSource::CallDSM(TW_UINT32 DG, TW_UINT16 DAT, TW_UINT16 MSG,
	TW_MEMREF pData)
{
	return m_pMgr->CallDSM(&m_SrcID, DG, DAT, MSG, pData);
}


void CTwainDataSource::GetCap(CTwainCapability& cap)
{
	ASSERT(cap.IsEmpty());
	int rc = m_pMgr->CallDSM(&m_SrcID, DG_CONTROL, DAT_CAPABILITY,
		MSG_GET, (TW_MEMREF)&cap.GetData());
	TwainCheckStatus(rc);
	cap.CreateContainer();
}

void CTwainDataSource::GetCurrent(CTwainCapability& cap)
{
	ASSERT(cap.IsEmpty());
	int rc = m_pMgr->CallDSM(&m_SrcID, DG_CONTROL, DAT_CAPABILITY,
		MSG_GETCURRENT, (TW_MEMREF)&cap.GetData());
	TwainCheckStatus(rc);
	cap.CreateContainer();
}

void CTwainDataSource::GetDefault(CTwainCapability& cap)
{
	ASSERT(cap.IsEmpty());
	int rc = m_pMgr->CallDSM(&m_SrcID, DG_CONTROL, DAT_CAPABILITY,
		MSG_GETDEFAULT, (TW_MEMREF)&cap.GetData());
	TwainCheckStatus(rc);
	cap.CreateContainer();
}

void CTwainDataSource::SetCap(CTwainCapability& cap)
{
	ASSERT(!cap.IsEmpty());
	int rc = m_pMgr->CallDSM(&m_SrcID, DG_CONTROL, DAT_CAPABILITY,
		MSG_SET, (TW_MEMREF)&cap.GetData());
	TwainCheckStatus(rc);
}

int  CTwainDataSource::GetSupportedFormats()
{
	int nPixelType;
	int nSuppFmts;
	CTwainCapability CapPixelType(ICAP_PIXELTYPE);
	try {
		GetCap(CapPixelType);
	} catch(CException *e) {
		e->Delete();
		return 0;
	} catch (...) {
		return 0;
	}
	if (CapPixelType.GetConType() == TWON_ONEVALUE)
	{
		nPixelType = (int)CapPixelType.AsOneValue()->GetValue();
		nSuppFmts = nPixelType == TWPT_GRAY ? GrayScaleSupported :
			TrueColorSupported;
		return nSuppFmts;
	}
	else  if (CapPixelType.GetConType() == TWON_ENUMERATION)
	{
		nSuppFmts = 0;
		if (CapPixelType.AsEnum()->Find(TWPT_GRAY)!=-1)
			nSuppFmts |= GrayScaleSupported;
		if (CapPixelType.AsEnum()->Find(TWPT_RGB)!=-1)
			nSuppFmts |= TrueColorSupported;
	}
	else nSuppFmts = 0;
	return nSuppFmts;
}

void CTwainDataSource::InitFormat(BOOL fTrueColor, int nColorDepth /* = 0*/)
{
	BOOL fNeedSetPT;
	int nPixelType;
	BOOL fNeedSetBD;
	int nBitDepth;
	CTwainCapability CapPixelType(ICAP_PIXELTYPE);
	CTwainCapability CapBitDepth(ICAP_BITDEPTH);
	// First, negotiate needed pixel type.
	GetCap(CapPixelType);
	if (CapPixelType.GetConType() == TWON_ONEVALUE)
	{
		nPixelType = (int)CapPixelType.AsOneValue()->GetValue();
		if (fTrueColor && nPixelType != TWPT_RGB ||
			!fTrueColor && nPixelType != TWPT_GRAY)
			throw new CTwainNotImplException("Pixel types other, "
				"than TWPT_RGB and TWPT_GRAY");
		fNeedSetPT = FALSE;
	}
	else if (CapPixelType.GetConType() == TWON_ENUMERATION)
	{
		if (fTrueColor)
			nPixelType = TWPT_RGB;
		else
			nPixelType = TWPT_GRAY;
		if (CapPixelType.AsEnum()->Find((float)nPixelType) == -1)
			throw new CTwainNotImplException("Pixel types other, "
				"than TWPT_RGB and TWPT_GRAY");
		// Needed pixel type supported, but it may be, one has not
		// been selected as current.
		CTwainCapability CapPTCur(ICAP_PIXELTYPE);
		GetCurrent(CapPTCur);
		if (CapPTCur.GetConType() != TWON_ONEVALUE)
			fNeedSetPT = TRUE;
		else
			fNeedSetPT = CapPTCur.AsOneValue()->GetValue() != nPixelType;
	}
	else 
		throw new CTwainNotImplException("Pixel types containers other, "
			"than TWON_ONEVALUE and TWON_ENUMERATION");
	if (fNeedSetPT)
	{
		CTwainCapability CapPTSet(ICAP_PIXELTYPE, TWON_ONEVALUE,
			new CTwainOneValue((float)nPixelType));
		SetCap(CapPTSet);
	};
	// Second, negotiate bit depth
	int nNeedBD = fTrueColor ? nColorDepth * 3 : nColorDepth;
	char szErrBuff[50];
	sprintf(szErrBuff, "bit depth less than %d", nNeedBD);
	GetCap(CapBitDepth);
	if (CapBitDepth.GetConType() == TWON_ONEVALUE)
	{
		nBitDepth = (int)CapBitDepth.AsOneValue()->GetValue();
		// if nNeedBD == 0, available any bit depth
		if (nBitDepth < nNeedBD)
			throw new CTwainNotImplException(szErrBuff);
		fNeedSetBD = FALSE;
	}
	else if (CapBitDepth.GetConType() == TWON_ENUMERATION)
	{
		int pos;
		if (nNeedBD)
		{
			if (( pos = CapBitDepth.AsEnum()->FindMoreOrEqual((float)nNeedBD)) == -1)
				throw new CTwainNotImplException(szErrBuff);
		}
		else
			pos = CapBitDepth.AsEnum()->FindMax();
		// Needed bit depth supported, but it may be, one has not
		// been selected as current.
		nBitDepth = (int)CapBitDepth.AsEnum()->GetAt(pos);
		CTwainCapability CapBDCur(ICAP_BITDEPTH);
		GetCurrent(CapBDCur);
		if (CapBDCur.GetConType() != TWON_ONEVALUE)
			throw new CTwainNotImplException("Aggregate current "
				"values for bit depth");
		fNeedSetBD = CapBDCur.AsOneValue()->GetValue() != nBitDepth;
	}
	else 
		throw new CTwainNotImplException("Bit depth containers other, "
			"than TWON_ONEVALUE and TWON_ENUMERATION");
	if (fNeedSetBD)
	{
		CTwainCapability CapBDSet(ICAP_BITDEPTH, TWON_ONEVALUE,
			new CTwainOneValue((float)nBitDepth));
		SetCap(CapBDSet);
	}
}

void CTwainDataSource::SetXferMech(int nMech)
{
	CTwainCapability CapXferMech(ICAP_XFERMECH, TWON_ONEVALUE, 
		new CTwainOneValue((float)nMech));
	SetCap(CapXferMech);
}

void CTwainDataSource::SetupMemXfer(TW_SETUPMEMXFER &Data)
{
	int rc = m_pMgr->CallDSM(&m_SrcID, DG_CONTROL, DAT_SETUPMEMXFER,
		MSG_GET, (TW_MEMREF)&Data);
	TwainCheckStatus(rc);
}

void CTwainDataSource::GetImageInfo(CImageInfo &Data)
{
	int rc = m_pMgr->CallDSM(&m_SrcID, DG_IMAGE, DAT_IMAGEINFO,
		MSG_GET, (TW_MEMREF)&Data.m_Data);
	TwainCheckStatus(rc);
}

void  CTwainDataSource::Enable(HWND hWndParent, BOOL fShowUI)
{
	TW_USERINTERFACE UI;
	UI.ShowUI  = fShowUI;
	UI.ModalUI = TRUE;
	UI.hParent = fShowUI ? hWndParent : NULL;
	int rc = m_pMgr->CallDSM(&m_SrcID, DG_CONTROL, DAT_USERINTERFACE,
		MSG_ENABLEDS, (TW_MEMREF)&UI);
	TwainCheckStatus(rc);
//	if (hWndParent)
//		::EnableWindow(hWndParent,FALSE);
}

void  CTwainDataSource::Disable(HWND hWndParent, BOOL fShowUI)
{
	TW_USERINTERFACE UI;
	UI.ShowUI  = fShowUI;
	UI.ModalUI = FALSE;
	UI.hParent = fShowUI ? hWndParent : NULL;
//	if (hWndParent)
//		::EnableWindow(hWndParent,TRUE);
	int rc = m_pMgr->CallDSM(&m_SrcID, DG_CONTROL, DAT_USERINTERFACE,
		MSG_DISABLEDS, (TW_MEMREF)&UI);
	TwainCheckStatus(rc);
}

float CTwainDataSource::GetNumCap( int nCap)
{
	CTwainCapability Cap(nCap);
	GetCurrent(Cap);
	if (Cap.GetConType() != TWON_ONEVALUE)
		throw new CTwainNotImplException("");
	return Cap.AsOneValue()->GetValue();
}

float CTwainDataSource::GetNumCapNoThrow( int nCap, float Default)
{
	float Value = Default;
	try {
		Value = GetNumCap(nCap);
	} catch(CException *e) {
		e->Delete();
	} catch (...) {
	}
	return Value;
}

void CTwainDataSource::SetNumCap( int nCap, float Value )
{
	CTwainCapability Cap(nCap);
	GetCap(Cap);
	int pos;
	float NewValue;
	switch (Cap.GetConType())
	{
	case TWON_ONEVALUE:
		if (Cap.AsOneValue()->GetValue() == Value)
			return;
		NewValue = Value;
		break;
	case TWON_ENUMERATION:
		pos = Cap.AsEnum()->FindMoreOrEqual(Value);
		if (pos != -1)
			NewValue = Cap.AsEnum()->GetAt(pos);
		else
			return;
		break;
	case TWON_RANGE:
		NewValue = Cap.AsRange()->FindMoreOrEqual(Value);
		break;
	}
	CTwainCapability CapSet(nCap, TWON_ONEVALUE, 
		new CTwainOneValue(NewValue));
	SetCap(CapSet);
}

void CTwainDataSource::SetNumCapNoThrow( int nCap, float Value )
{
	try {
		SetNumCap(nCap,Value);
	} catch(CException *e) {
		e->Delete();
	} catch (...) {
	}
}

int CTwainDataSource::GetExposureTime()
{
	return (int)ceil(GetNumCap(ICAP_EXPOSURETIME));
}

void CTwainDataSource::SetExposureTime(int nExposureTime)
{
	SetNumCap(ICAP_EXPOSURETIME,(float)nExposureTime);
}

int CTwainDataSource::GetExposureTimeNoThrow(int nDefault)
{
	return (int)ceil(GetNumCapNoThrow(ICAP_EXPOSURETIME,(float)nDefault));
}

void CTwainDataSource::SetExposureTimeNoThrow(int nExposureTime)
{
	SetNumCapNoThrow(ICAP_EXPOSURETIME,(float)nExposureTime);
}

int CTwainDataSource::GetBrightness()
{
	return (int)ceil(GetNumCap(ICAP_BRIGHTNESS));
}

void CTwainDataSource::SetBrightness(int nBrightness)
{
	if (nBrightness > 1000)  nBrightness = 1000;
	if (nBrightness < -1000) nBrightness = -1000;
	SetNumCap(ICAP_BRIGHTNESS,(float)nBrightness);
}

int CTwainDataSource::GetBrightnessNoThrow(int nDefault)
{
	return (int)ceil(GetNumCapNoThrow(ICAP_BRIGHTNESS,(float)nDefault));
}

void CTwainDataSource::SetBrightnessNoThrow(int nBrightness)
{
	if (nBrightness > 1000)  nBrightness = 1000;
	if (nBrightness < -1000) nBrightness = -1000;
	SetNumCapNoThrow(ICAP_BRIGHTNESS,(float)nBrightness);
}

BOOL CTwainDataSource::GetAutoBright()
{
	return GetNumCap(ICAP_AUTOBRIGHT) == 0 ? FALSE : TRUE;
}

void CTwainDataSource::SetAutoBright(BOOL bAutoBright)
{
	SetNumCap(ICAP_AUTOBRIGHT,(float)bAutoBright);
}

BOOL CTwainDataSource::GetAutoBrightNoThrow(BOOL bDefault)
{
	return GetNumCapNoThrow(ICAP_AUTOBRIGHT,(float)bDefault) == 0 ?	FALSE : TRUE;
}

void CTwainDataSource::SetAutoBrightNoThrow(BOOL bAutoBright)
{
	SetNumCapNoThrow(ICAP_AUTOBRIGHT,(float)bAutoBright);
}

int CTwainDataSource::GetContrast()
{
	return (int)ceil(GetNumCap(ICAP_CONTRAST));
}

void CTwainDataSource::SetContrast(int nContrast)
{
	if (nContrast > 1000)  nContrast = 1000;
	if (nContrast < -1000) nContrast = -1000;
	SetNumCap(ICAP_CONTRAST,(float)nContrast);
}

int CTwainDataSource::GetContrastNoThrow(int nDefault)
{
	return (int)ceil(GetNumCapNoThrow(ICAP_CONTRAST,(float)nDefault));
}

void CTwainDataSource::SetContrastNoThrow(int nContrast)
{
	if (nContrast > 1000)  nContrast = 1000;
	if (nContrast < -1000) nContrast = -1000;
	SetNumCapNoThrow(ICAP_CONTRAST,(float)nContrast);
}

int CTwainDataSource::GetBitDepth()
{
	return (int)GetNumCap(ICAP_BITDEPTH);
}

void CTwainDataSource::SetBitDepth(int nBitDepth)
{
	SetNumCap(ICAP_BITDEPTH,(float)nBitDepth);
}

int CTwainDataSource::GetBitDepthNoThrow(int nDefault)
{
	return (int)GetNumCapNoThrow(ICAP_BITDEPTH,(float)nDefault);
}

void CTwainDataSource::SetBitDepthNoThrow(int nBitDepth)
{
	SetNumCapNoThrow(ICAP_BITDEPTH,(float)nBitDepth);
}

int CTwainDataSource::GetPixelType()
{
	return (int)GetNumCap(ICAP_PIXELTYPE);
}

void CTwainDataSource::SetPixelType(int nPixelType)
{
	SetNumCap(ICAP_PIXELTYPE,(float)nPixelType);
}

int CTwainDataSource::GetPixelTypeNoThrow(int nDefault)
{
	return (int)GetNumCapNoThrow(ICAP_PIXELTYPE,(float)nDefault);
}

void CTwainDataSource::SetPixelTypeNoThrow(int nPixelType)
{
	SetNumCapNoThrow(ICAP_PIXELTYPE,(float)nPixelType);
}

BOOL CTwainDataSource::GetUIControllable()
{
	return GetNumCap(CAP_UICONTROLLABLE) == 0 ? FALSE : TRUE;
}

void CTwainDataSource::SetUIControllable(BOOL bUIControlable)
{
	SetNumCap(CAP_UICONTROLLABLE,(float)bUIControlable);
}

BOOL CTwainDataSource::GetUIControllableNoThrow(BOOL bDefault)
{
	return (int)GetNumCapNoThrow(CAP_UICONTROLLABLE,(float)bDefault) == 0 ?	FALSE : TRUE;
}

void CTwainDataSource::SetUIControllableNoThrow(BOOL bUIControlable)
{
	SetNumCapNoThrow(CAP_UICONTROLLABLE,(float)bUIControlable);
}

BOOL CTwainDataSource::GetDeviceOnline()
{
	return (BOOL)GetNumCap(CAP_DEVICEONLINE);
}

BOOL CTwainDataSource::GetDeviceOnlineNoThrow( BOOL bDefault )
{
	return (BOOL)GetNumCapNoThrow(CAP_DEVICEONLINE,(float)bDefault) == 0 ? FALSE : TRUE;
}

int CTwainDataSource::GetUnit()
{
	return (int)GetNumCap(ICAP_UNITS);
}

void CTwainDataSource::SetUnit(int nUnit)
{
	SetNumCap(ICAP_UNITS,(float)nUnit);
}

int CTwainDataSource::GetUnitNoThrow(int nDefault)
{
	return (int)GetNumCapNoThrow(ICAP_UNITS,(float)nDefault);
}

void CTwainDataSource::SetUnitNoThrow(int nUnit)
{
	SetNumCapNoThrow(ICAP_UNITS,(float)nUnit);
}

int CTwainDataSource::GetPhysicalHeight()
{
	return (int)GetNumCap(ICAP_PHYSICALHEIGHT);
}

void CTwainDataSource::SetPhysicalHeight(int nHeight)
{
	SetNumCap(ICAP_PHYSICALHEIGHT,(float)nHeight);
}

int CTwainDataSource::GetPhysicalHeightNoThrow(int nDefault)
{
	return (int)GetNumCapNoThrow(ICAP_PHYSICALHEIGHT,(float)nDefault);
}

void CTwainDataSource::SetPhysicalHeightNoThrow(int nHeight)
{
	SetNumCapNoThrow(ICAP_PHYSICALHEIGHT,(float)nHeight);
}

int CTwainDataSource::GetPhysicalWidth()
{
	return (int)GetNumCap(ICAP_PHYSICALWIDTH);
}

void CTwainDataSource::SetPhysicalWidth(int nWidth)
{
	SetNumCap(ICAP_PHYSICALWIDTH,(float)nWidth);
}

int CTwainDataSource::GetPhysicalWidthNoThrow(int nDefault)
{
	return (int)GetNumCapNoThrow(ICAP_PHYSICALWIDTH,(float)nDefault);
}

void CTwainDataSource::SetPhysicalWidthNoThrow(int nWidth)
{
	SetNumCapNoThrow(ICAP_PHYSICALWIDTH,(float)nWidth);
}


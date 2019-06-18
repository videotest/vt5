#include "stdafx.h"
#include "twcore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CTwainException,CException);

int CTwainException::ReportError(UINT nType /*= MB_OK*/, UINT nMessageID /*= 0*/)
{
	char buffer[50];
	wsprintf(buffer, "Twain error, code %d", m_rcCode);
	AfxMessageBox(buffer, nType);
	return 0;
}

IMPLEMENT_DYNAMIC(CTwainUnkErrException,CException);

int CTwainUnkErrException::ReportError(UINT nType /*= MB_OK*/, UINT nMessageID /*= 0*/)
{
	char buffer[50];
	wsprintf(buffer, "Twain unknown error");
	AfxMessageBox(buffer, nType);
	return 0;
}

void CTwainDataSrcMgr::TwainCheckStatus(int rc)
{
	if (rc != TWRC_SUCCESS)
		throw new CTwainException(rc);
}

void CTwainDataSrcMgr::ClearDataSources()
{
	int nSize = m_DataSources.GetSize() - 1;
	for (; nSize >= 0; nSize--)
	{
		CTwainDataSource *p = m_DataSources.GetAt(nSize);
		delete p;
		m_DataSources.RemoveAt(nSize);
	}
}

CTwainDataSrcMgr::CTwainDataSrcMgr(HWND hWndParent)
{
	m_nState = 1;
	m_hWndParent = hWndParent;
}

CTwainDataSrcMgr::~CTwainDataSrcMgr()
{
	Close();
}

#pragma optimize( "", off )
void CTwainDataSrcMgr::Open()
{
	// First, put DSM into state 2(DSM loaded).
	if (m_nState == 1)
	{
		m_TwainDll.LoadLibrary("twain_32.dll");
		(FARPROC &)m_DSM_Entry = m_TwainDll.GetProcAddress("DSM_Entry");
		m_nState = 2;
	}
	// Now DSM is in state 2. Put it into state 3 (DSM opened)
	if (m_nState == 2)
	{
		m_AppID.Id = 0;	// Initialize to 0 (Source Manager will assign real value)
		m_AppID.Version.MajorNum = 3;	//Your application's version number
		m_AppID.Version.MinorNum = 5;
		m_AppID.Version.Language = TWLG_USA;
		m_AppID.Version.Country  = TWCY_USA;
		lstrcpy (m_AppID.Version.Info, "Your Application's Version String");
		m_AppID.ProtocolMajor = TWON_PROTOCOLMAJOR;
		m_AppID.ProtocolMinor = TWON_PROTOCOLMINOR;
		m_AppID.SupportedGroups = DG_IMAGE | DG_CONTROL;
		lstrcpy (m_AppID.Manufacturer, "Ista-VideoTesT");
		lstrcpy (m_AppID.ProductFamily, "Videotest");
		lstrcpy (m_AppID.ProductName, "Videotest");
		TW_UINT16  rc;
		rc = (*m_DSM_Entry) (&m_AppID, NULL, DG_CONTROL, DAT_PARENT,
			MSG_OPENDSM, (TW_MEMREF) &m_hWndParent);
		TwainCheckStatus(rc);
		m_nState = 3;
	}
}

void CTwainDataSrcMgr::Close()
{
	// First, close DSM and put it into state 2.
	ASSERT(m_nState <= 3 && m_nState > 0);
	ClearDataSources();
	if (m_nState >= 3)
	{
		int rc = (*m_DSM_Entry) (&m_AppID, NULL, DG_CONTROL, DAT_PARENT,
			MSG_CLOSEDSM, (TW_MEMREF) &m_hWndParent);
		TwainCheckStatus(rc);
		m_nState = 2;
	}
	// Secondly, free library and put DSM into state 1.
	if (m_nState == 2)
	{
		m_TwainDll.FreeLibrary();
		m_DSM_Entry = NULL;
		m_nState = 1;
	}
}

#pragma optimize( "", on )

CTwainDataSource *CTwainDataSrcMgr::FirstDataSource()
{
	ASSERT(m_nState == 3);
	TW_IDENTITY SrcId;
	int rc = m_DSM_Entry(&m_AppID, NULL, DG_CONTROL, DAT_IDENTITY,
		MSG_GETFIRST, &SrcId);
	if (rc == TWRC_FAILURE) return NULL;
	TwainCheckStatus(rc);
	CTwainDataSource *p = new CTwainDataSource(this,SrcId);
	ClearDataSources();
	m_DataSources.Add(p);
	return p;
}

CTwainDataSource *CTwainDataSrcMgr::NextDataSource()
{
	ASSERT(m_nState == 3);
	TW_IDENTITY SrcId;
	int rc = m_DSM_Entry(&m_AppID, NULL, DG_CONTROL, DAT_IDENTITY,
		MSG_GETNEXT, &SrcId);
	if (rc == TWRC_ENDOFLIST) return NULL;
	TwainCheckStatus(rc);
	CTwainDataSource *p = new CTwainDataSource(this,SrcId);
	m_DataSources.Add(p);
	return p;
}

CTwainDataSource *CTwainDataSrcMgr::DefaultDataSource()
{
	ASSERT(m_nState == 3);
	TW_IDENTITY SrcId;
	int rc = m_DSM_Entry(&m_AppID, NULL, DG_CONTROL, DAT_IDENTITY,
		MSG_GETDEFAULT, &SrcId);
	TwainCheckStatus(rc);
	CTwainDataSource *p = new CTwainDataSource(this,SrcId);
	return p;
}

CTwainDataSource *CTwainDataSrcMgr::UserSelect()
{
	TW_IDENTITY SrcId;
	int rc = (*m_DSM_Entry) (&m_AppID, NULL, DG_CONTROL, DAT_IDENTITY,
		MSG_USERSELECT, (TW_MEMREF) &SrcId);
	TwainCheckStatus(rc);
	CTwainDataSource *p = new CTwainDataSource(this,SrcId);
	return p;
}


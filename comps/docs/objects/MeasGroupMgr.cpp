// MeasGroupMgr.cpp : implementation file
//

#include "stdafx.h"
#include "Objects.h"
#include "MeasGroupMgr.h"
#include "NameConsts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeasurementManager

IMPLEMENT_DYNCREATE(CMeasurementManager, CCmdTargetEx)

// {F71103BA-5014-4b2b-B682-3EBFE453C01F}
static const IID IID_IMeasGroupMgrDisp =
{ 0xf71103ba, 0x5014, 0x4b2b, { 0xb6, 0x82, 0x3e, 0xbf, 0xe4, 0x53, 0xc0, 0x1f } };
// {91678E7E-B56C-4c98-8671-84C40C65268F}
GUARD_IMPLEMENT_OLECREATE(CMeasurementManager, "Objects.MeasGroupMgr", 
0x91678e7e, 0xb56c, 0x4c98, 0x86, 0x71, 0x84, 0xc4, 0xc, 0x65, 0x26, 0x8f);


BEGIN_INTERFACE_MAP(CMeasurementManager, CCmdTargetEx)
	INTERFACE_PART(CMeasurementManager, IID_IMeasGroupMgrDisp,	Dispatch)
	INTERFACE_PART(CMeasurementManager, IID_INamedObject2,	Name)
	INTERFACE_PART(CMeasurementManager, IID_IRootedObject,	Rooted)
	INTERFACE_PART(CMeasurementManager, IID_ICompManager,	CompMan)
	INTERFACE_PART(CMeasurementManager, IID_IMeasureManager,	MeasManager)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CMeasurementManager, MeasManager)

CMeasurementManager::CMeasurementManager()
{
	m_bInConstructor = true;
	_OleLockApp( this );

	SetName(szMeasurement);
	AttachComponentGroup(szPluginMeasurement);
	SetSectionName(szPluginMeasurement);

	EnableAutomation();
	EnableAggregation();
	
	Init();
	m_bInConstructor = false;
}

CMeasurementManager::~CMeasurementManager()
{
	_OleUnlockApp( this );
}


void CMeasurementManager::OnFinalRelease()
{
	CCmdTargetEx::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CMeasurementManager, CCmdTargetEx)
	//{{AFX_MSG_MAP(CMeasurementManager)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMeasurementManager, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CMeasurementManager)
	DISP_FUNCTION(CMeasurementManager, "LoadState", LoadState, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMeasurementManager, "SaveState", SaveState, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeasurementManager message handlers

void CMeasurementManager::DeInit()
{
	if (!m_bInConstructor)
		SaveState();
	CCmdTargetEx::DeInit();
}

bool CMeasurementManager::Init()
{
	bool bRes = CCompManager::Init();
	sort_groups( );
	return bRes;
}

LPCTSTR CMeasurementManager::GetGroupLoadParamString()
{
	return "";
}

void CMeasurementManager::LoadState()
{
	for( int i = 0; i < GetComponentCount(); i++ )
	{
		IMeasParamGroupPtr	ptr( GetComponent( i ) );
		if( ptr ==0 )continue;
		//reload group states
		ptr->InitializeCalculation( 0 );
		ptr->FinalizeCalculation();
	}
}

void CMeasurementManager::SaveState()
{
}

long	CMeasurementManager::get_groups_priority( IUnknown *punk_group )
{	// get groups priority
	if( !punk_group )
		return 0;
	IPriority *pprior = 0;
	long lPriority = 0;

	if( S_OK != punk_group->QueryInterface( IID_IPriority, (void**)&pprior ) )
		return 0;
	
	if( !pprior )
		return 0;

	if( S_OK != pprior->GetPriority( &lPriority ) )
		lPriority = 0;

    pprior->Release( );
	pprior = 0;

	return lPriority;
}

void	CMeasurementManager::sort_groups( )
{	// sort groups by Desc

    long lCount = m_ptrs.GetCount( );

	if( lCount != m_strs.GetCount( ) )
		return;

	for( long i = 0; i < lCount; i++ )
	{
		long lID = -1;
		long lPriority_i = get_groups_priority( (IUnknown *) m_ptrs[ i ] );

		for( long j = i; j < lCount; j++ )
		{	
			long lPriority_j = get_groups_priority( (IUnknown *) m_ptrs[ j ] );
			if( lPriority_j > lPriority_i )
			{
				lPriority_i = lPriority_j;
				lID = j;
			}
		}
		
		if( lID != -1 )
		{		
			IUnknown *p = (IUnknown *) m_ptrs[ i ];
			CString str = m_strs[ i ];

			m_ptrs[ i ] = m_ptrs[ lID ];
			m_strs[ i ] = m_strs[ lID ];

            m_ptrs[ lID ] = p;
			m_strs[ lID ] = str;
		}
	}
}



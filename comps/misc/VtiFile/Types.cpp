#include "StdAfx.h"
#include "resource.h"
#include "Types.h"
#include "AppDefs.h"
//#include "IniValue.h"
#include "MeasArg.h"
//#include "AccCtrl.h"
//#include "Params.h"
//#include "Clbrman.h"
//#include "ProgLog.h"
//#include "FuncInt.h"
//#include "unkutils.h"
#include "obnative.h"
#include "vt4params.h"
//#include "NlsFloat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
//Variables
CTypeListManager	g_TypesManager;

IMPLEMENT_SERIAL( CArg, CObject, 1 );
//IMPLEMENT_SERIAL( CArgDbl, CArg, 1 );
//IMPLEMENT_SERIAL( CArgInt, CArg, 1 );
IMPLEMENT_SERIAL( CTypeInfo, CObject, 1 );
IMPLEMENT_DYNAMIC( CTypeListManager, CObList );

void CArg::Serialize( CArchive &ar )
{
	if( ar.IsStoring() )
	{
		ar<<m_nKey;
		ar<<m_value;
	}
	else
	{
		ar>>m_nKey;
		ar>>m_value;
	}
}

/*CArgInt::CArgInt( int iData, WORD nKey ):CArg( nKey )
{
	m_iData = iData;
}
void CArgInt::Serialize( CArchive &ar )
{
	CArg::Serialize( ar );
	if( ar.IsStoring() )
		ar<<m_iData;
	else
		ar>>m_iData;
}

CArgDbl::CArgDbl( double fData, WORD nKey):CArg( nKey )
{
	m_fData = fData;
}

void CArgDbl::Serialize( CArchive &ar )
{
	CArg::Serialize( ar );
	if( ar.IsStoring() )
		ar<<m_fData;
	else
		ar>>m_fData;
} */

///////////////////////////////////////////////////////////////////////////////
//CTypeInfo
//Called by table TB_WANTINFO Message

CString CTypeInfo::GetTextValue( CArg *parg )
{
	static CString	str;

	if( !parg||parg->GetType() == VT_EMPTY )
	{
		str.LoadString( IDS_NOVALUE );
		return str;
	}

	double	fVal = MAX_FLOAT_VAL;
	
	if( parg->GetType() & VT_I4 )
		fVal = parg->m_value.iVal;
		
	if( parg->GetType() & VT_R8 )
		fVal = parg->m_value.dblVal;

	if( fVal == MAX_FLOAT_VAL ||
		fVal == -MAX_FLOAT_VAL )
		return "?";

	str.Format( GetFormat(), fVal );
	// Check for -0
	double d = atof(str);
	if (d == 0.)
		str.Format( GetFormat(), double(0.) );

//	str = __c2nls(str);
	return str;
}
/*void CTypeInfo::GetInfo( TABLEINFO *pTI, CArg *pArg )
{
	CString	str;
	if( !pArg )
	{
		str.LoadString( IDS_NOVALUE );
		strcpy( pTI->buf, str );
		return;
	}
								
	if( pArg->GetType() == VT_EMPTY )
	{
		str.LoadString( IDS_NOVALUE );
		strcpy( pTI->buf, str );
		return;
	}

	if( pArg->GetType() & VT_I4 )
		sprintf( pTI->buf, GetFormat(), (int)*pArg );
	if( pArg->GetType() & VT_R8 )
		sprintf( pTI->buf, GetFormat(), (double)*pArg );
}*/

/*void CTypeInfo::GetHeadInfo( TABLEINFO *pTI )
{
	strcpy( pTI->buf, GetLongName() );
} */

void CTypeInfo::Serialize( CArchive &ar )
{
	if( ar.IsStoring() )
	{
		ar<<m_strName;
		ar<<m_nKey;
		ar<<(int)m_info;
		ar<<m_iUnits;
	}
	else
	{
		int	info;
		ar>>m_strName;
		ar>>m_nKey;
		ar>>info;
		ar>>m_iUnits;
		m_info = (TYPE_INFO)info;
	}
}

///////////////////////////////////////////////////////////////////////////////
//CTypeListManager

CTypeListManager::CTypeListManager()
	:CTypeInfoList()
{
	m_nLastKey = KEY_BASE_USER;
	m_iMinSize = 10;
}

POSITION	CTypeListManager::AddTail(CTypeInfo *p)
{
	if (p)
		return CTypeInfoList::AddTail(p);
	else
		return 0;
}

void CTypeListManager::Init()
{
#if 0
	//...
	//Add all standard measurement group
	//...

	AddGroup( new CTypeGroupInfo( TG_AREA, IDS_PAR_AREA ) );
	AddGroup( new CTypeGroupInfo( TG_PERIMETER, IDS_PAR_PERIMETER ) );
	AddGroup( new CTypeGroupInfo( TG_SIZES, IDS_PAR_SIZES ) );
	AddGroup( new CTypeGroupInfo( TG_DIAMETERS, IDS_PAR_DIAMETERS ) );
	AddGroup( new CTypeGroupInfo( TG_FORM, IDS_PAR_FORM ) );
	AddGroup( new CTypeGroupInfo( TG_PLACEMENT, IDS_PAR_PLACEMENT ) );
	AddGroup( new CTypeGroupInfo( TG_OPTICAL, IDS_PAR_OPTICAL ) );
	AddGroup( new CTypeGroupInfo( TG_COLOR, IDS_PAR_COLOR ) );
	AddGroup( new CTypeGroupInfo( TG_MOVE, IDS_PAR_MOVE ) );
	AddGroup( new CTypeGroupInfo( TG_USER, IDS_PAR_USER ) );

	//...
	//Add all standard measurement
	//...
	int iUnitsDef=AddUnitsName( IDS_UNITS_DEF );
	int iUnitsLen=AddUnitsName( IDS_UNITS_LEN );
	int iUnitsArea=AddUnitsName( IDS_UNITS_AREA );
	int iUnitsRel=AddUnitsName( IDS_UNITS_RELATIVE );
	int iUnitsDeg=AddUnitsName( IDS_UNITS_DEG );
	int iUnitsBri=AddUnitsName( IDS_UNITS_BRI );
	int iUnitsSpeed=AddUnitsName( IDS_UNITS_SPEED );
	int iUnitsTime=AddUnitsName( IDS_UNITS_TIME );

	
//TG_AREA
	AddTail( ConstructTI( TI_DBL, IDS_AREA, iUnitsArea, TG_AREA, KEY_AREA ) );
	AddTail( ConstructTI( TI_DBL, IDS_INT_AREA, iUnitsArea, TG_AREA, KEY_INT_AREA ) );
//TG_PERIMETER
	AddTail( ConstructTI( TI_DBL, IDS_PERIMETER, iUnitsLen, TG_PERIMETER, KEY_PERIMETER ) );
	AddTail( ConstructTI( TI_DBL, IDS_INT_PERIMETER, iUnitsLen, TG_PERIMETER, KEY_INT_PERIMETER ) );
	AddTail( ConstructTI( TI_DBL, IDS_PCONCAVE, iUnitsLen, TG_PERIMETER, KEY_PCONCAVE ) );
	AddTail( ConstructTI( TI_DBL, IDS_PCONVEX, iUnitsLen, TG_PERIMETER, KEY_PCONVEX ) );
//TG_SIZES
	AddTail( ConstructTI( TI_DBL, IDS_P_LENGTH, iUnitsLen, TG_SIZES, KEY_LENGTH ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_WIDTH, iUnitsLen, TG_SIZES, KEY_WIDTH ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_DIM, iUnitsLen, TG_SIZES, KEY_DIM ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_XLEN, iUnitsLen, TG_SIZES, KEY_XLEN ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_YLEN, iUnitsLen, TG_SIZES, KEY_YLEN ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_AVRCHORD, iUnitsLen, TG_SIZES, KEY_AVRCHORD ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_FEREMAX, iUnitsLen, TG_SIZES, KEY_FEREMAX ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_FEREMIN, iUnitsLen, TG_SIZES, KEY_FEREMIN ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_FEREAVRG, iUnitsLen, TG_SIZES, KEY_FEREAVRG ) );

	AddTail( ConstructTI( TI_DBL, IDS_P_DECV, iUnitsLen, TG_DIAMETERS, KEY_DECV ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_ELPSMAX, iUnitsLen, TG_DIAMETERS, KEY_ELPSMAX ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_ELPSMIN, iUnitsLen, TG_DIAMETERS, KEY_ELPSMIN ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_LINEL, iUnitsLen, TG_DIAMETERS, KEY_LINEL ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_LINEW, iUnitsLen, TG_DIAMETERS, KEY_LINEW ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_RIBBONL, iUnitsLen, TG_DIAMETERS, KEY_RIBBONL ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_RIBBONW, iUnitsLen, TG_DIAMETERS, KEY_RIBBONW ) );

	AddTail( ConstructTI( TI_DBL, IDS_P_CIRCLE, iUnitsRel, TG_FORM, KEY_CIRCLE ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_ELLIPSE, iUnitsRel, TG_FORM, KEY_ELLIPSE ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_ROUNDISH, iUnitsRel, TG_FORM, KEY_ROUNDISH ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_NOUN, iUnitsRel, TG_FORM, KEY_NOUN ) );
	
	AddTail( ConstructTI( TI_DBL, IDS_P_XCOORD, iUnitsLen, TG_PLACEMENT, KEY_XCOORD ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_YCOORD, iUnitsLen, TG_PLACEMENT, KEY_YCOORD ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_ANGLE, iUnitsDeg, TG_PLACEMENT, KEY_ANGLE ) );
	
	AddTail( ConstructTI( TI_DBL, IDS_P_BRIAVR, iUnitsBri, TG_OPTICAL, KEY_BRIAVR ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_BRIDEV, iUnitsBri, TG_OPTICAL, KEY_BRIDEV ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_BRIMIN, iUnitsBri, TG_OPTICAL, KEY_BRIMIN ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_BRIMAX, iUnitsBri, TG_OPTICAL, KEY_BRIMAX ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_BRIINT, iUnitsBri, TG_OPTICAL, KEY_BRIINT ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_BRIINTEGRAL, iUnitsBri, TG_OPTICAL, KEY_BRIINTEGRAL ) );

	AddTail( ConstructTI( TI_DBL, IDS_P_RED, iUnitsBri, TG_COLOR, KEY_RED ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_GREEN, iUnitsBri, TG_COLOR, KEY_GREEN ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_BLUE, iUnitsBri, TG_COLOR, KEY_BLUE ) );
	AddTail( ConstructTI( TI_DBL, IDS_P_HUE, iUnitsBri, TG_COLOR, KEY_HUE ) ); // 38

	AddTail( ConstructTI( TI_DBL, IDS_SPEED, iUnitsSpeed, TG_MOVE, KEY_SPEED ) ); // 41
	AddTail( ConstructTI( TI_DBL, IDS_P_TIME, iUnitsTime, TG_MOVE, KEY_TIME ) ); // 42

	AddTail( ConstructTI( TI_DBL, IDS_P_OBJID, iUnitsDef, TG_USER, KEY_OBJID ) ); // 43
	AddTail( ConstructTI( TI_DBL, IDS_CURVE_LENGTH, iUnitsLen, TG_USER ) ); // 39
	AddTail( ConstructTI( TI_DBL, IDS_AREA_COEF, iUnitsRel, TG_USER ) ); // 40

	AddTail( ConstructTI( TI_DBL, IDS_P_OD, iUnitsBri, TG_OPTICAL, KEY_OPTICAL_DENS ) ); // 38
	AddTail( ConstructTI( TI_DBL, IDS_P_IOD, iUnitsBri, TG_OPTICAL, KEY_INT_OD ) ); // 38

	//...
	//Add all standard manual measurement
	//...

	AddTail( ConstructTI( TI_DBL, IDS_LENGTH, iUnitsLen, TG_MANUAL, KEY_MANUAL_LENGTH ) );
	AddTail( ConstructTI( TI_DBL, IDS_FREELEN, iUnitsLen, TG_MANUAL, KEY_MANUAL_FREE_LENGTH ) );
	AddTail( ConstructTI( TI_DBL, IDS_RADIUS, iUnitsLen, TG_MANUAL, KEY_MANUAL_RADIUS ) );
	AddTail( ConstructTI( TI_DBL, IDS_ANGLE, iUnitsDeg, TG_MANUAL, KEY_MANUAL_ANGLE ) );
	
	
	ScanDLL();	//add-in support ( in future );

	ReadRegistry();
#endif
}

void CTypeListManager::Destroy()
{
	BEGIN_LOG_METHOD(Destroy,2);
	WriteRegistry();
	POSITION pos = GetHeadPosition();

	for( int i = 0; i<m_array.GetSize(); i++ )
		delete m_array[i];
	m_array.RemoveAll();

	for( i = 0; i < m_arrFunc.GetSize(); i++ )
		delete m_arrFunc[i];
	m_arrFunc.RemoveAll();

	while( pos ) delete GetNext( pos );
	RemoveAll();

//	m_man.FreeComponents();

	END_LOG_METHOD();
}

void CTypeListManager::AddGroup(CTypeGroupInfo *p)
{
	m_array.Add(p);
}

CTypeInfo *CTypeListManager::ConstructTI( TYPE_INFO info, CString str, int iUnits, int group, WORD wKey )
{
	wKey = CheckKey( wKey );

	return new CTypeInfo( wKey, info, str, group, iUnits );
}

CTypeInfo *CTypeListManager::ConstructTI( TYPE_INFO info, UINT iDD, int iUnits, int group, WORD wKey )
{
	CString str;
	str.LoadString( iDD );
	CTypeInfo *pinfo= ConstructTI( info, str, iUnits, group, wKey );
	pinfo->SetID( iDD );
	return pinfo;
}

void CTypeListManager::ScanDLL()
{
#if 0
	CDataDescription	*pDesc;
	int	iCount = 0;

	for( int i = 0; i < g_pFuncArray->GetSize(); i++ )
	{
		CDLLFuncInfo *pinfo = (CDLLFuncInfo *)g_pFuncArray->GetAt( i );
		int	iGrp;

		if( pinfo->GetType() != FUNCTYPE_MEAS )
			continue;

		FUNCMEAS	funcMeas = (FUNCMEAS)pinfo->GetLibrary()->GetProcAddress( pinfo->GetIntFuncInfo().m_pszIntFuncName );
					   
		ASSERT( funcMeas );

		iGrp = TG_DLL+iCount;//m_array.GetSize();
		m_array.Add( new CTypeGroupInfo( iGrp, pinfo->GetName() ) );
		iCount++;

		char	*pszname = 0;
		pDesc = (CDataDescription	*)funcMeas( mfaGetCalcValues, 0, 0, &pszname );

		int	idxBase = m_listUnits.GetSize();
		int	*piUnits = 0;

		if( pDesc->pszUnits )
		{
			int	iC = 0;
			for( int j = 0; pDesc->pszUnits[j]; j++ )
				iC++;
			piUnits = new int[iC];

			for( j = 0; pDesc->pszUnits[j]; j++ )
				piUnits[j] = AddUnitsName( pDesc->pszUnits[j] );
		}


		for( int j= 0; j < pDesc->m_iCount; j++ )
		{
			int	iUnits = pDesc->pDataDesc[j].iUnits;

			try{
			if( piUnits )
				iUnits = piUnits[iUnits];
			}
			catch(...){
				iUnits = 0;
			}
			
			CTypeInfo *pinfo = ConstructTI( TI_DBL, pDesc->pDataDesc[j].szName, iUnits, iGrp, pDesc->pDataDesc[j].wKey );
			pDesc->pDataDesc[j].wKey = pinfo->GetKey();
			AddTail( pinfo );
		}

		if( piUnits )
			delete piUnits;

		funcMeas( mfaSetKeys, 0, 0, 0 );

		if( pszname )
		{
			FUNCDOMEAS	pfnCallBack = (FUNCDOMEAS)pinfo->GetLibrary()->GetProcAddress( pszname );
			if( pfnCallBack )
				m_arrFunc.Add( new CFuncContainer( pfnCallBack ) );
		}
		iGrp++;
	}

	m_man.SetSection( szParameters );
	m_man.LoadComponents();

	for( i = 0; i < m_man.GetComponentCount(); i++ )
	{
		IParamCalculator	*punkCalc = (IParamCalculator*)m_man[i];

		BSTR	bstr = 0;
		int	iCnt = 0;

		call( punkCalc->GetName( &bstr ) );
		call( punkCalc->GetParamsCount( &iCnt ) );

		CString	sName( bstr );

		::SysFreeString( bstr );

		int iGrp = TG_DLL+iCount;//m_array.GetSize();
		m_array.Add( new CTypeGroupInfo( iGrp, sName ) );
		iCount++;

		
		for( int j = 0; j < iCnt; j++ )
		{
			BSTR	b1 = 0, b2 = 0;

			IParamTypeInfo	*ppti = 0;
			WORD		wKey = 0;

			call( punkCalc->GetParamTypeInfo( j, (IUnknown**)&ppti ) );

			call( ppti->GetName( &b1 ) );
			call( ppti->GetUnitName( &b2 ) );
			call( ppti->GetKey( (short *)&wKey ) );

			CString	sName( b1 );
			CString	sUnits( b2 );

			ppti->Release();

			::SysFreeString( b1 );
			::SysFreeString( b2 );
			

			int	iUnits = AddUnitsName( sUnits );

			CTypeInfo *pinfo = ConstructTI( TI_DBL, sName, iUnits, iGrp, wKey );
			AddTail( pinfo );

			wKey  = pinfo->GetKey();
			
			call( punkCalc->SetParamsKey( j, wKey ) );
		}
	}
#endif
}

CTypeInfo *CTypeListManager::Find( CString str )
{
	POSITION pos = GetHeadPosition();
	CTypeInfo *pInfo;

	while( pos )
	{
		pInfo = GetNext( pos );

		if( pInfo->GetName() == str )
			return pInfo;
	}

//	throw EXCPT_INTERNBREAK;

	return 0;
}

CTypeInfo *CTypeListManager::Find( WORD nKey )
{
	POSITION pos = GetHeadPosition();
	CTypeInfo *pInfo;

	while( pos )
	{
		pInfo = GetNext( pos );

		if( pInfo->GetKey() == nKey )
			return pInfo;
	}

//	ASSERT( FALSE );
	return 0;
}

char	szMinSize[] = "MinObjectSize";

void CTypeListManager::WriteRegistry()
{
#if 0
	POSITION	pos = GetHeadPosition();

	Setval( szMinSize, m_iMinSize );

	while( pos )
	{
		CTypeInfo *pinfo = GetNext( pos );
		Setval( pinfo->GetName(), pinfo->IsEnable() );
		Setval( pinfo->GetName()+"-Separate", pinfo->IsEnable1() );
		Setval( pinfo->GetName()+"-Name", pinfo->GetLongName() );
		Setval( pinfo->GetName()+"-Format", pinfo->GetFormat() );
	}
#endif
}

extern BOOL g_bUseSmooth;
extern BOOL g_bDrawLabels;
extern BOOL g_bRotateIcons;

void CTypeListManager::ReadRegistry()
{
#if 0
	::Setval( "SmoothMeasurement", 
		g_bUseSmooth = ::Getval( "SmoothMeasurement", g_bUseSmooth ) );
	::Setval( "ShowLabels", 
		g_bDrawLabels = ::Getval( "ShowLabels", g_bDrawLabels ) );
	::Setval( "RotateIcons", 
		g_bRotateIcons = ::Getval( "RotateIcons", g_bRotateIcons ) );

		

	POSITION	pos = GetHeadPosition();

	m_iMinSize = Getval( szMinSize, 5 );

	int	i1, i2;

	i1 = Getval( szMinSize, 1 );
	i2 = Getval( szMinSize, 2 );


	BOOL bSomeFound = i1==i2;

	while( pos )
	{
		CTypeInfo *pinfo = GetNext( pos );

		BOOL	bEnable = Getval( pinfo->GetName(), !bSomeFound );
		BOOL	bEnable1 = Getval( pinfo->GetName()+"-Separate", !bSomeFound );
		CString	str = Getval( pinfo->GetName()+"-Name", pinfo->GetName() );
		CString	strFormat = Getval( pinfo->GetName()+"-Format", pinfo->GetFormat() );

		pinfo->SetLongName( str );
		pinfo->SetFormat( strFormat );

		if( pinfo->GetGroup() != TG_MANUAL )
		{
			pinfo->Enable( bEnable );
			pinfo->Enable1( bEnable1 );
		}
		else
		{
			pinfo->Enable( TRUE );
			pinfo->Enable1( FALSE );
		}
	}
#endif
}

void CTypeListManager::ExecuteExternMeasure( CObjNative *pobj, CObjNativeArray *parr )
{
#if 0
	try
	{
		for( int i = 0; i<m_arrFunc.GetSize(); i++ )
			(*m_arrFunc[i]->GetFunct())( pobj, parr );

		for( i = 0; i < m_man.GetComponentCount(); i++ )
		{
			IParamCalculator	*punkCalc = (IParamCalculator*)m_man[i];

			IUnknown	*punk = 0;
			pobj->QueryInterface( IID_IUnknown, (void**)&punk );

			IParamCalculator2 *punkCalc2 = NULL;
			if (SUCCEEDED(punkCalc->QueryInterface(IID_IParamCalculator2, (void **)&punkCalc2)) && punkCalc2)
			{
				IUnknown	*punkArr = 0;
				parr->QueryInterface( IID_IUnknown, (void**)&punkArr );
				punkCalc2->Calc(punk, punkArr, 0);
				punkArr->Release();
				punkCalc2->Release();
			}
			else
				punkCalc->Calc( punk, 0 );

			punk->Release();
		}
	}
	catch( ... )
	{
		TRACE( "External measure error" );
	}
#endif
}

BOOL CTypeListManager::IsUnitsAvaible( int idx )
{
	return ( idx < 0 ) || ( m_listUnits.GetSize() > idx );
}

CString CTypeListManager::GetUnitsName( int idx )
{
	if( !IsUnitsAvaible( idx ) )
	{
		CString	str;
		str.LoadString( IDS_UNITS_DEF );
		return str;
	}
	CString str = m_listUnits.GetAt( idx );

//	str.Format( str, (LPCSTR)g_CalibrManager.GetUnits(),
//		(LPCSTR)g_CalibrManager.GetUnits() );

	return str;
}

CString CTypeListManager::GetRawUnitsName( int idx )
{
	if( !IsUnitsAvaible( idx ) )
	{
		CString	str;
		str.LoadString( IDS_UNITS_DEF );
		return str;
	}
	CString str = m_listUnits.GetAt( idx );

	return str;
}

int CTypeListManager::AddUnitsName( CString str )
{
	for( int i = 0; i<m_listUnits.GetSize(); i++ )
		if( m_listUnits[i] == str )
			return i;

	return m_listUnits.Add( str );
}

int CTypeListManager::AddUnitsName( UINT nIDD )
{
	CString	str;

	str.LoadString( nIDD );

	return AddUnitsName( str );
}

// IUnknown
HRESULT CTypeInfo::QueryInterface(const IID& iid, void** ppvObject)
{
	*ppvObject = 0;
	if( iid == IID_IUnknown )
		*ppvObject = (IParamTypeInfo*)this;
	if( iid == IID_IParamTypeInfo )
		*ppvObject = (IParamTypeInfo*)this;

	if( !*ppvObject )
		return E_NOINTERFACE;
	((IUnknown*)*ppvObject)->AddRef();

	return S_OK;
}

ULONG CTypeInfo::AddRef()
{
	return 1;
}

ULONG CTypeInfo::Release()
{
	return 1;
}
// IParamTypeInfo
HRESULT CTypeInfo::GetName( BSTR *strName )
{
	*strName = m_strName.AllocSysString();
	return S_OK;
}

HRESULT CTypeInfo::SetName( BSTR strName )
{
	m_strName = strName;
	return S_OK;
}

HRESULT CTypeInfo::GetLongName( BSTR *strName )
{
	*strName = m_strLongName.IsEmpty() ? m_strName.AllocSysString() : m_strLongName.AllocSysString();
	return S_OK;
}

HRESULT CTypeInfo::SetLongName( BSTR strName )
{
	m_strLongName = strName;
	return S_OK;
}

HRESULT CTypeInfo::GetKey( short *nMeasKey ) // nMeasKey - unique ID of measurement parameter
{
	*nMeasKey = m_nKey;
	return S_OK;
}

HRESULT CTypeInfo::Enable1_( BOOL bEnable )
{
	Enable1( bEnable );
	return S_OK;
}

HRESULT CTypeInfo::IsEnable1( BOOL *bEnable )
{
	*bEnable = IsEnable1();
	return S_OK;
}

HRESULT CTypeInfo::GetUnitName( BSTR *strName )
{
	*strName = g_TypesManager.GetRawUnitsName( m_iUnits ).AllocSysString();
	return S_OK;
}

WORD CTypeListManager::CheckKey( WORD wKey )
{
	if( wKey != NOKEY )
	{
		POSITION	pos = GetHeadPosition();
		bool		bFound = false;

		while( pos )
		{
			CTypeInfo	*pinfo = GetNext( pos );

			if( pinfo->GetKey() == wKey )
			{
				bFound = true;
				break;
			}
		}

		if( !bFound )
			return wKey;
	}

	return m_nLastKey++;
}
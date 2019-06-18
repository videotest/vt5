#include "StdAfx.h"
#include "param.h"
#include "statistics.h"

#include "calc_int.h"
#include "misc.h"
#include "statisticobject.h"
#include "float.h"
#include <ap.h>

long _get_count( long lCount )
{
	if( lCount <= 75 )
		return 5;
	return ( lCount  <= 500 ) ? (lCount / 15 ) : 30;
}

namespace {
	double FrvNormOld(double x, double eps)
	{
		double x2=x*x, y=exp(-x2/2.)*0.3989422804014,
						sum, term, r, rho, t, s;

		long lCount = 0;
		if (x == 0) return 0.5;
		x2=1./x2; term=sum=y/fabs(x); r=s=0; rho=1;
		do {
				r+=x2; rho=1./(1+r*rho); term*=rho-1;
				t=s; s=sum; sum+=term;
		  
			if( lCount > 10000 )
				break;

			lCount++;
		} while(fabs(s-t) > eps || fabs(s-sum) > eps);
		return (x > 0 ? 1-sum : sum);
	}
	
	double Norm(double x)
	{
	//* M_2_SQRTPI - 2/sqrt(pi)
	//* M_SQRT2    - sqrt(2)
	//* M_SQRT1_2  - 1/sqrt(2)
		double dInteg=0.;
		double sum=x, xx=-x*x/2.;
		double term=x;
		double sum1=0.;
		for(int i=1; sum1!=sum; ++i){
			term *= xx/i;
			sum1=sum;
			sum += term /(2.*i+1.);
		}
		dInteg = M_2_SQRTPI*M_SQRT1_2*sum;
		return 0.5+0.5*dInteg;
	}
}

double FuncNormDistrib(double x)
{
	if(fabs(x)<3.)
		return Norm(x);
	else 
		return FrvNormOld(x,0.00001);
}

#define BEGIN_GROUP() \
		for(set<NumImg>::const_iterator itImg=m_pStat->m_setImages.begin(); \
			itImg!=m_pStat->m_setImages.end(); ++itImg) \
				{ \
			NumImg lCountGroup = *itImg; \
			_OBJECT_LIST& objsImg = *SubClass(lCountGroup).m_pObjList;

#define BEGIN_ROW() \
	long lCount=0; \
	for(_OBJECT_LIST::const_iterator it=m_Class.m_pObjList->begin(); it!=m_Class.m_pObjList->end(); ++it){ \
		XObjectInfo *info=*it; \
				double fVal = info->fValue;  \
		lCount++;

#define END_ROW() \
			} \

#define END_GROUP() \
		} \

#define BEGIN_CALC()\
	long  lCount = 0; \
	for(_OBJECT_LIST::const_iterator it=m_Class.m_pObjList->begin(); it!=m_Class.m_pObjList->end(); ++it){ \
		XObjectInfo *info=*it; \
				double fVal = info->fValue;  \
	lCount++; 

#define END_CALC() }


namespace ObjectSpace
{

namespace ParamSpace
{

typedef CStatisticObject::ClassMap ClassMap;

ParDscr::ParDscr()
{
// Default standard parameters description
	add(new CStatParamDscr(PARAM_MX,CStringW((LPCSTR)IDS_PARAM_MX),3));
	add(new CStatParamDscr(PARAM_MX_GEOM,CStringW((LPCSTR)IDS_PARAM_GEOM),3));
	add(new CStatParamDscr(PARAM_MX_GORM,CStringW((LPCSTR)IDS_PARAM_GORM),3));
	add(new CStatParamDscr(PARAM_MIN,CStringW((LPCSTR)IDS_PARAM_MIN),3));
	add(new CStatParamDscr(PARAM_MAX,CStringW((LPCSTR)IDS_PARAM_MAX),3));
	add(new CStatParamDscr(PARAM_VAR,CStringW((LPCSTR)IDS_PARAM_VAR),3));
	add(new CStatParamDscr(PARAM_DISP,CStringW((LPCSTR)IDS_PARAM_DISP),3));
	add(new CStatParamDscr(PARAM_STDDEV,CStringW((LPCSTR)IDS_PARAM_STDDEV),3));
	add(new CStatParamDscr(PARAM_MODE,CStringW((LPCSTR)IDS_PARAM_MODE),3));
	add(new CStatParamDscr(PARAM_MEDIANE,CStringW((LPCSTR)IDS_PARAM_MEDIANE),3));
	add(new CStatParamDscr(PARAM_ASSIMETRY,CStringW((LPCSTR)IDS_PARAM_ASSIMETRY),3));
	add(new CStatParamDscr(PARAM_EXCESS,CStringW((LPCSTR)IDS_PARAM_EXCESS),3));
	add(new CStatParamDscr(PARAM_COEF_VAR,CStringW((LPCSTR)IDS_PARAM_COEFVAR),3));
//	add(new CStatParamDscr(PARAM_MXLOG,CStringW(),0));
//	add(new CStatParamDscrBase(PARAM_SUMLOG));
//	add(new CStatParamDscr(PARAM_DISPLOG,CStringW(),0));
	add(new CStatParamDscr(PARAM_COUNT,CStringW((LPCSTR)IDS_PARAM_COUNT),3));
	add(new CStatParamDscr(PARAM_AREA,CStringW((LPCSTR)IDS_PARAM_AREA),3));
	add(new CStatParamDscr(PARAM_LENGTH,CStringW((LPCSTR)IDS_PARAM_LENGTH),3));
	add(new CStatParamDscr(PARAM_USEDAREA,CStringW((LPCSTR)IDS_PARAM_USEDAREA),3));
	add(new CStatParamDscr(PARAM_SPECAREA,CStringW((LPCSTR)IDS_PARAM_SPECAREA),3));
	add(new CStatErrorDscr(PARAM_SPECAREA_ERR,CStringW((LPCSTR)IDS_PARAM_SPECAREA_ERR),3));
	add(new CStatParamDscr(PARAM_AREAPERC,CStringW((LPCSTR)IDS_PARAM_AREAPERC),3));
	add(new CStatErrorDscr(PARAM_AREAPERC_ERR,CStringW((LPCSTR)IDS_PARAM_AREAPERC_ERR),3));
	add(new CStatNormalParamDscr(PARAM_NORMAL_AREA,CStringW((LPCSTR)IDS_PARAM_NORMAL_AREA),3));
	add(new CStatParamDscr(PARAM_SPECCOUNT,CStringW((LPCSTR)IDS_PARAM_SPECCOUNT),3));
	add(new CStatErrorDscr(PARAM_SPECCOUNT_ERR,CStringW((LPCSTR)IDS_PARAM_SPECCOUNT_ERR),3));
	add(new CStatParamDscr(PARAM_COUNTPERC,CStringW((LPCSTR)IDS_PARAM_COUNTPERC),3));
	add(new CStatErrorDscr(PARAM_COUNTPERC_ERR,CStringW((LPCSTR)IDS_PARAM_COUNTPERC_ERR),3));
	add(new CStatNormalParamDscr(PARAM_NORMAL_COUNT,CStringW((LPCSTR)IDS_PARAM_NORMAL_COUNT),3));
	add(new CStatParamDscr(PARAM_SPECLENGTH,CStringW((LPCSTR)IDS_PARAM_SPECLEN),3));
	add(new CStatErrorDscr(PARAM_SPECLENGTH_ERR,CStringW(),3));
	add(new CStatParamDscr(PARAM_3DPART,CStringW((LPCSTR)IDS_PARAM_3DPART),3));
	add(new CStatParamDscr(PARAM_MX_ERR,CStringW((LPCSTR)IDS_PARAM_MXERR),3));
	add(new CStatErrorDscr(PARAM_MX_PROB,CStringW((LPCSTR)IDS_PARAM_MXPROB),3));
	add(new CStatParamDscr(PARAM_CHI_SQUARE,CStringW((LPCSTR)IDS_PARAM_CHI_SQUARE),0));
	add(new CStatParamDscr(PARAM_CHI_SQUARE_LOG,CStringW((LPCSTR)IDS_PARAM_CHI_SQUARE_LOG),3));
	add(new CStatParamDscr(PARAM_IMAGE_COUNT,CStringW((LPCSTR)IDS_PARAM_IMAGE_COUNT),3));
	add(new CStatParamDscr(PARAM_FULL_AREA,CStringW((LPCSTR)IDS_PARAM_FULLAREA),3));
	add(new CStatParamDscr(PARAM_PERIMETR,CStringW((LPCSTR)IDS_PARAM_PERIMETR),3));
	add(new CStatParamDscr(PARAM_SURF_AREA,CStringW((LPCSTR)IDS_PARAM_SURF_AREA),3));
	add(new CStatParamDscr(PARAM_MDL_CHORD,CStringW((LPCSTR)IDS_PARAM_MDL_CHORD),3));
	add(new CStatParamDscr(PARAM_MDL_DIST,CStringW((LPCSTR)IDS_PARAM_MDL_DIST),3));
	add(new CStatParamDscr(PARAM_SURFAREAPERC,CStringW((LPCSTR)IDS_PARAM_SURFAREAPERC),3));
	add(new CStatParamDscr(PARAM_SPEC_SURF,CStringW((LPCSTR)IDS_PARAM_SPEC_SURF),3));
	add(new CStatParamDscr(PARAM_SPEC_SURF_PERC,CStringW((LPCSTR)IDS_PARAM_SPEC_SURF_PERC),3));
	add(new CStatParamDscr(PARAM_STD_DEV_LOG,CStringW((LPCSTR)IDS_PARAM_STDDEV_LOG),0));
	add(new CStatParamDscr(PARAM_MX_ERR_LOG,CStringW(),0));
	add(new CStatErrorDscr(PARAM_MX_PROB_LOG,CStringW((LPCSTR)IDS_PARAM_MXPROB_LOG),0));
	add(new CStatParamDscr(PARAM_MODE_LOG,CStringW((LPCSTR)IDS_PARAM_MODE_LOG),0));
	add(new CStatParamDscr(PARAM_ASSIMETRY_LOG,CStringW((LPCSTR)IDS_PARAM_ASSIMETRY_LOG),0));
	add(new CStatParamDscr(PARAM_EXCESS_LOG,CStringW((LPCSTR)IDS_PARAM_EXCESS_LOG),0));
	add(new CStatParamDscr(PARAM_COEF_VAR_LOG,CStringW((LPCSTR)IDS_PARAM_COEFVAR_LOG),0));
	add(new CStatParamDscr(PARAM_DXLOG,CStringW((LPCSTR)IDS_PARAM_DISP_LOG),0));
	add(new CStatParamDscr(PARAM_MATHXLOG,CStringW((LPCSTR)IDS_PARAM_MX_LOG),0));
	add(new CStatParamDscr(PARAM_MX_LOGNORM,CStringW((LPCSTR)IDS_PARAM_MX_LOGNORM),3));
	add(new CStatParamDscr(PARAM_DISP_LOGNORM,CStringW((LPCSTR)IDS_PARAM_DISP_LOGNORM),3));
	add(new CStatParamDscr(PARAM_MODE_LOGNORM,CStringW((LPCSTR)IDS_PARAM_MODE_LOGNORM),3));
	add(new CStatParamDscr(PARAM_ASSIMETRY_LOGNORM,CStringW((LPCSTR)IDS_PARAM_ASSIMETRY_LOGNORM),3));
	add(new CStatParamDscr(PARAM_EXCESS_LOGNORM,CStringW((LPCSTR)IDS_PARAM_EXCESS_LOGNORM),3));
	add(new CStatErrorDscr(PARAM_CHI2_CRITIC,CStringW((LPCSTR)IDS_PARAM_CHI2_CRITIC),3));
	add(new CStatParamDscr(PARAM_WEIGHTED_AVERAGE,CStringW((LPCSTR)IDS_PARAM_WEIGHTED_AVERAGE),0));
	add(new CStatParamDscr(PARAM_CHI2_PROB,CStringW((LPCSTR)IDS_PARAM_CHI2_PROB),0));
	add(new CStatParamDscr(PARAM_CHI2_PROB_LOG,CStringW((LPCSTR)IDS_PARAM_CHI2_PROB_LOG),0));
}

bool CStatParamBase::is_any_objects()
{
	BEGIN_GROUP();
		//if( nimage != lCountGroup )
		//	continue;
		BEGIN_ROW();
			return true;
		END_ROW();
	END_GROUP();
	return false;
}

void _delete_info( XObjectInfo *pObject ) { delete pObject; };
void _delete_list_info( _OBJECT_LIST *pObject )
{
	pObject->clear();
	delete pObject;
}
bool _is_zero( double fVal, double fEps = 1e-48 )
{
	if( fabs( fVal ) <= fEps )
		return true;
	return false;
}

void CStatUserParamDscr::LoadSettings( INamedDataPtr sptrND )
{
	if( sptrND == 0 )
		return;

	_variant_t var;

	//m_lBaseKey = GetValueInt( sptrND, SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY, m_lBaseKey );

	CStringW str;
	str.Format( L"%ld", key );
	sptrND->SetupSection(_bstr_t(SECT_STATUI_STAT_PARAM_ROOT) + L"\\" + str.GetString());

	var = _bstr_t( m_szName );
	sptrND->GetValue( _bstr_t( PARAM_NAME ), &var );
	if( var.vt != VT_EMPTY && var.vt == VT_BSTR )
		m_szName=var.bstrVal;

	var = _bstr_t( m_szFormat );
	sptrND->GetValue( _bstr_t( FORMAT_STRING ), &var );
	if( var.vt != VT_EMPTY && var.vt == VT_BSTR )
		m_szFormat=var.bstrVal;

	if( m_szFormat.length() == 0 )
		m_szFormat=L"%lf";

	var.Clear();

	sptrND->GetValue( _bstr_t( DESIGNATION ), &var );
	if( var.vt != VT_EMPTY && var.vt == VT_BSTR )
		m_szDesignation=var;
	var.Clear();
	
	sptrND->GetValue( _bstr_t( MODULE_OF_DESIGN ), &var );
	if( var.vt != VT_EMPTY && var.vt == VT_BSTR )
		m_szModuleOfDesign=var.bstrVal;
	var.Clear();

	sptrND->GetValue( _bstr_t( EXPRESSION_STRING ), &var );
	if( var.vt != VT_EMPTY && var.vt == VT_BSTR )
		m_szExpression=var;
	var.Clear();

	var = 0L;
	sptrND->GetValue( _bstr_t( CALC_METHOD ), &var );
	if( var.vt != VT_EMPTY )
		m_lCalcMethod = var;
	var.Clear();

	var = -1L;
	sptrND->GetValue( _bstr_t( PARAM_GREEK_SYMB ), &var );
	var.Clear();

	var = m_lVisibility;
	sptrND->GetValue( _bstr_t( PARAM_VISIBILITY ), &var );
	if( var.vt != VT_EMPTY )
		m_lVisibility = var;
	var.Clear();

	var = 0L;
	sptrND->GetValue( _bstr_t( PARAM_TABLE_ORDER ), &var );
	if( var.vt != VT_EMPTY )
		m_lTableOrder = var;
	var.Clear();

	var = 0L;
	sptrND->GetValue( _bstr_t( PARAM_LEGEND_ORDER ), &var );
	if( var.vt != VT_EMPTY )
		m_lLegendOrder = var;
	var.Clear();

}

void CStatUserParamDscr::StoreSettings( INamedDataPtr sptrND )
{
	if( sptrND == 0 )
		return;

	CStringW str;
	str.Format( L"%ld", key );
	sptrND->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT) + L"\\" + str.GetString());

	sptrND->SetValue( _bstr_t( PARAM_NAME ), _variant_t( _bstr_t( m_szName ) ) );
	sptrND->SetValue( _bstr_t( FORMAT_STRING ), _variant_t( _bstr_t( m_szFormat ) ) );
	sptrND->SetValue( _bstr_t( DESIGNATION ), _variant_t( _bstr_t( m_szDesignation ) ) );
	sptrND->SetValue( _bstr_t( MODULE_OF_DESIGN ), _variant_t( _bstr_t( m_szModuleOfDesign ) ) );

	sptrND->SetValue( _bstr_t( EXPRESSION_STRING ), _variant_t( _bstr_t( m_szExpression ) ) );
	sptrND->SetValue( _bstr_t( CALC_METHOD ), _variant_t( (long)m_lCalcMethod ) );
	sptrND->SetValue( _bstr_t( PARAM_VISIBILITY ), _variant_t( (long)m_lVisibility ) );

	sptrND->SetValue( _bstr_t( PARAM_TABLE_ORDER ), _variant_t( (long)m_lTableOrder ) );
	sptrND->SetValue( _bstr_t( PARAM_LEGEND_ORDER ), _variant_t( (long)m_lLegendOrder ) );
}

CStatParamUserDefined::CStatParamUserDefined(CClass& cls, INamedDataPtr sptrND, long lKey )
	:CStatParamBase(cls), m_Computing(0)
{
	CStatUserParamDscr*& rpParDscr=(CStatUserParamDscr*&)GetParDscr(lKey);
	m_lKey = lKey;
	if(rpParDscr == NULL)
	{
		rpParDscr=new CStatUserParamDscr(lKey);
		rpParDscr->LoadSettings(sptrND);
	}
	pUserParDscr=rpParDscr;
}

bool CStatParamUserDefined::IsAvailable() 
{ 
	if(pUserParDscr->m_lCalcMethod>=0){
		return ObjectSpace::Class(pUserParDscr->m_lCalcMethod)==m_Class.m_lClass.cls;
	}else if(-1==pUserParDscr->m_lCalcMethod){
		return m_Class.m_lClass.cls == AllClasses;
	}else if(-2==pUserParDscr->m_lCalcMethod){
		return true;
	}else{
		return false;
	}
}

namespace {
	IClassFactory* GetCalcFactory(){
		IClassFactory* pcf;
		HRESULT hr=::CoGetClassObject( clsidCalculatingMachine, CLSCTX_INPROC, 0, IID_IClassFactory, (LPVOID*)&pcf);
		return pcf;
	}
}

bool CStatParamUserDefined::Calculate()
{
	if( IsCalculated() )
		return true;
	if( !IsAvailable() )
		return false;
	if(1==m_Computing){
		m_pStat->m_Warning=CStatisticObject::UserParamRecursion;
		return false;
	}
	static IClassFactoryPtr s_pCalcFactory(GetCalcFactory(),false);
	ICalcExpressionPtr ptrCalcExpr;
	HRESULT hr=s_pCalcFactory->CreateInstance(NULL,IID_ICalcExpression,(void**)&ptrCalcExpr);
	if( ptrCalcExpr == 0 )
		return false;

	_bstr_t bstrExpr( pUserParDscr->m_szExpression ), bstrErr;

	DWORD dwFlag = 0;

	if( ptrCalcExpr->GetState( &dwFlag ) != S_OK  ) 
		return false;

	//if( !( dwFlag & osParsed ) )
	{
		if( ptrCalcExpr->SetString( bstrExpr ) != S_OK  ) 
			return false;
		if( ptrCalcExpr->Parse( bstrErr.GetAddress() ) != S_OK )
			return false;
	}

	// get params
	long lParamsSize = 0;

	// get size
	if( ptrCalcExpr->ReturnNecessaryCalcParams( 0, &lParamsSize ) != S_OK ) 
		return false;

	struct BstrArray:public std::valarray<BSTR>{
		BstrArray(int size):std::valarray<BSTR>((BSTR)NULL,size){}
		~BstrArray(){
			for( size_t i = 0; i < size(); ++i ){
				BSTR& bstr=std::valarray<BSTR>::operator[](i);
				if(bstr)
					::SysFreeString(bstr);
			}
		}
	};

	BstrArray pbstrParams(lParamsSize);

	// fill array
	if( lParamsSize && ptrCalcExpr->ReturnNecessaryCalcParams( &pbstrParams[0], &lParamsSize ) != S_OK ) 
	{
		return false;
	}

	// [vanek] BT2000:4236 - 04.01.2005
	bool bcalc_in_curr_units = false;
	INamedPropBagPtr sptrProp = m_pStat;
	if( sptrProp )
	{
		_variant_t var;
		sptrProp->GetProperty( _bstr_t( "_calc_user_params_in_curr_units_" ), &var );                
		bcalc_in_curr_units = (0L != (long)(var));
	}

	bool bRes = true;

	if( lParamsSize > 0 )
	{
		dblArray pParamsValues(lParamsSize);

		// calculate parameters of expr
		for( long lIndex = 0; lIndex < lParamsSize; lIndex++ )
		{
			CString strName = (char*)_bstr_t( pbstrParams[lIndex] );

			long lParamKey = 0;
			ObjectSpace::Class lClass;

			int nPos = -1;
			if( ( nPos = strName.Find( ':', 0 ) ) != -1 )
			{
				CString strKey = strName.Left( nPos );
				CString strClass = strName.Right( strName.GetLength() - nPos - 1 );

				lParamKey = atol( strKey );
				lClass = ObjectSpace::Class(atol( strClass ));
			}
			else
			{
				lParamKey = atol( strName );
				lClass = m_Class.m_lClass.cls;
			}

			IStatObjectDispPtr sptrD = m_pStat;

			HRESULT hr = 0;

			if( lParamKey == PARAM_STUDENT_IMG )
			{
				hr = S_OK;
				if( m_pStat->m_setImages.size() > 1 )
					pParamsValues[lIndex] = invstudenttdistribution( m_pStat->m_setImages.size() - 1, 0.5+0.5*pParErrDscr->m_fProb);
				else
					hr = S_FALSE;
			}
			else if( lParamKey == PARAM_STUDENT_OB )
			{
				int N=Class(ObjectSpace::Class(lClass),AllImages).N();
				hr = S_OK;
				if( N > 1 )
					pParamsValues[lIndex] = invstudenttdistribution(N - 1, 0.5+0.5*pParErrDscr->m_fProb );
				else
					hr = S_FALSE;
			}
			else
			{
				CStatParamBase& param=Class(ObjectSpace::Class(lClass),AllImages).Param(lParamKey);
				if(&param){
					m_Computing=1;
					bool bReturn=param.Calculate();
					m_Computing=0;
					if(!bReturn){
						return false;
					}
					pParamsValues[lIndex] = (double)param;
					if(bcalc_in_curr_units){
						pParamsValues[lIndex] *= m_pStat->_get_coef(lParamKey,NULL);
					}
				}else{
					return false;
				}
			}

			if( hr != S_OK )
			{
				bRes = false;
				break;
			}
		}


		if( bRes )
			bRes = ptrCalcExpr->SetParamsValues( &pParamsValues[0], lParamsSize ) != S_OK ? false : true;
	}

	if( bRes )
	{
		long lParamCount = 0;
		ptrCalcExpr->ReturnNecessaryCalcArrayParams( 0, &lParamCount );

		if( lParamCount > 0 )
		{
			BstrArray pbstrArrayParamsNames(lParamCount);

			//			if( pbstrArrayParamsNames )
			{
				if( ptrCalcExpr->ReturnNecessaryCalcArrayParams( &pbstrArrayParamsNames[0], &lParamCount ) == S_OK )
				{
					ArrayParamValues *parrValue = new ArrayParamValues[lParamCount];
					for( long i = 0; i < lParamCount; i++ )
					{
						CString strName = (char*)_bstr_t( pbstrArrayParamsNames[i] );

						int nPos = -1;
						if( ( nPos = strName.Find( ':', 0 ) ) != -1 )
						{
							CString strKey = strName.Left( nPos );
							CString strClass = strName.Right( strName.GetLength() - nPos - 1 );

							long lParamKey = atol( strKey );
							long lClass = atol( strClass );

							IStatObjectDispPtr sptrD = m_pStat;

							HRESULT hr = 0;

							double fValue = 0;
							if( lParamKey == PARAM_STUDENT_IMG )
							{
								hr = S_OK;
								if( m_pStat->m_setImages.size() > 1 )
									fValue = invstudenttdistribution(m_pStat->m_setImages.size() - 1, 0.5+0.5*pUserParDscr->m_fProb );
								else
									hr = S_FALSE;
							}
							else if( lParamKey == PARAM_STUDENT_OB )
							{
								int N=Class(ObjectSpace::Class(lClass),AllImages).N();
								hr = S_OK;
								if( N > 1 )
									fValue = invstudenttdistribution(N - 1, 0.5+0.5*pUserParDscr->m_fProb );
								else
									hr = S_FALSE;
							}
							else
							{
								CStatParamBase& param=Class(ObjectSpace::Class(lClass),AllImages).Param(lParamKey);

								m_Computing=1;
								bool bReturn=param.Calculate();
								m_Computing=0;
								if(!bReturn){
									hr = S_FALSE;
								}else{
									fValue = (double)param;
									if(bcalc_in_curr_units)
										fValue *= m_pStat->_get_coef(lParamKey,NULL);
								}
							}

							if( hr != S_OK )
							{
								bRes = false;
								break;
							}

							parrValue[i].lsize = m_pStat->m_setImages.size();
							parrValue[i].pfvalues = new double[parrValue[i].lsize];

							for( int j = 0; j < parrValue[i].lsize; j++ )
								parrValue[i].pfvalues[j] = fValue;
						}
						else
						{								  
							long lParamKey = atol( strName );
							IStatObjectPtr sptrSO = m_pStat;

							{
								parrValue[i].lsize = m_pStat->m_setImages.size();
								parrValue[i].pfvalues = new double[parrValue[i].lsize];

								long lCounter = parrValue[i].lsize;
								int j=0;
								for(set<NumImg>::const_iterator it=m_pStat->m_setImages.begin();
									it!=m_pStat->m_setImages.end(); ++it, ++j)
								{
									long iImage=*it;
									std::auto_ptr<CStatParamBase>pObject(
										m_pStat->_create_param_by_key(m_Class, m_pStat, lParamKey));

									if( pObject.get() ){
										if( !pObject->Calculate() )
										{
											lCounter--;
											parrValue[i].pfvalues[j] = 0;
										}
										else
										{
											parrValue[i].pfvalues[j] = pObject->GetValue();

											// [vanek] BT2000:4236 - 04.01.2005
											if( bcalc_in_curr_units )
											{
												if( sptrSO != 0 )
												{
													_bstr_t bstr_unit;
													double fcoeff = 1.;
													sptrSO->GetParamInfo( pObject->GetKey(), &fcoeff, bstr_unit.GetAddress() );
													parrValue[i].pfvalues[j] *= fcoeff;
												}
											}
										}
									}
									else
									{
										_ASSERTE(!"Can't create parameter by key");
									}
								}
								if( !lCounter )
									bRes = false;
							}
						}
					}
					ptrCalcExpr->SetArrayParamsValues( parrValue, lParamCount );
					for( long i = 0; i < lParamCount; i++ )
						if( parrValue[i].pfvalues )
							delete []parrValue[i].pfvalues;
					delete []parrValue; parrValue = 0;
				}
			}
		}
	}

	if( bRes )
	{
		_bstr_t bstrErrDescr;
		try {
			double fValue;
			bRes = ptrCalcExpr->Calculate( &fValue, bstrErrDescr.GetAddress() ) == S_OK ? true : false;
			if(bRes)m_fValue=fValue;
		}catch (...){
			bRes=false;
		}
	}

	return bRes;
}
/****/

CStatParamDscr*& CStatParamBase::GetParDscr(long key)
{
	return (CStatParamDscr*&)m_pStat->parDscrs[key];
}

CStatParamBase::CStatParamBase(CClass& cls):m_Class(cls),m_pStat(&(cls.stat))
	,m_fValue(std::numeric_limits<double>::quiet_NaN())
{
	// find matching CClass in ClassMap, if not create it
	if(m_pStat->m_pObjectList)
			m_Class.ChooseObjects();
}

void CStatParamDscr::LoadSettings( INamedDataPtr sptrND )
{
	if( sptrND == 0 )
		return;

	CStringW str;
	str.Format( L"%ld", key );
	sptrND->SetupSection( _bstr_t(SECT_STATUI_STAT_PARAM_ROOT) + L"\\" + str.GetString() );

	_variant_t var;

	var.Clear(); sptrND->GetValue( _bstr_t( PARAM_NAME ), &var );
	if( var.vt == VT_BSTR )
		m_szName=var.bstrVal;

	var.Clear(); sptrND->GetValue( _bstr_t( FORMAT_STRING ), &var );
	if( var.vt == VT_BSTR )
		m_szFormat=var.bstrVal;

	if( m_szFormat.length() == 0 )
		m_szFormat=L"%lf";

	var.Clear(); sptrND->GetValue( _bstr_t( DESIGNATION ), &var );
	if( var.vt == VT_BSTR )
		m_szDesignation=var.bstrVal;
	
	var.Clear(); sptrND->GetValue( _bstr_t( MODULE_OF_DESIGN ), &var );
	if( var.vt == VT_BSTR )
		m_szModuleOfDesign=var;

	var.Clear(); sptrND->GetValue( _bstr_t( PARAM_VISIBILITY ), &var );
	if( var.vt == VT_I4 )
		m_lVisibility = var;

	var = -1L;
	sptrND->GetValue( _bstr_t( PARAM_GREEK_SYMB ), &var );

	var = 0L;
	sptrND->GetValue( _bstr_t( CALC_FROM ), &var );
	if( var.vt != VT_EMPTY )
		m_lCalcFrom = var;

	var = 0L;
	sptrND->GetValue( _bstr_t( PARAM_TABLE_ORDER ), &var );
	if( var.vt != VT_EMPTY )
		m_lTableOrder = var;

	var = 0L;
	sptrND->GetValue( _bstr_t( PARAM_LEGEND_ORDER ), &var );
	if( var.vt != VT_EMPTY )
		m_lLegendOrder = var;

}

void CStatParamDscr::StoreSettings( INamedDataPtr sptrND )
{
	if( sptrND == 0 )
		return;

	_variant_t var(key); var.ChangeType(VT_BSTR);
	sptrND->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT) + L"\\" + var.bstrVal );

	sptrND->SetValue( _bstr_t( PARAM_NAME ), _variant_t( m_szName ) );
	sptrND->SetValue( _bstr_t( FORMAT_STRING ), _variant_t(m_szFormat) );
	sptrND->SetValue( _bstr_t( DESIGNATION ), _variant_t(m_szDesignation) );
	sptrND->SetValue( _bstr_t( MODULE_OF_DESIGN ), _variant_t(m_szModuleOfDesign) );
	sptrND->SetValue( _bstr_t( PARAM_VISIBILITY ), _variant_t(m_lVisibility) );

	sptrND->SetValue( _bstr_t( PARAM_TABLE_ORDER ), _variant_t(m_lTableOrder) );
	sptrND->SetValue( _bstr_t( PARAM_LEGEND_ORDER ), _variant_t(m_lLegendOrder) );


/*
	sptrND->SetValue( _bstr_t( SHOW_ON_GRAPH ), _variant_t( (long)m_lShowOnGraph ) );
*/

	sptrND->SetValue( _bstr_t( CALC_FROM ), _variant_t( m_lCalcFrom ) );
}

long CStatParamBase::GetSerializeSize() 
{ 
	return sizeof( long ) + sizeof( double ) + sizeof( bool );
}

void CStatParamBase::Store( IStream *pStream, SerializeParams *pparams )
{
	long lVersion = 1;
	ULONG nWritten = 0;

	pStream->Write( &lVersion, sizeof( long ), &nWritten );
	pStream->Write( &m_fValue, sizeof( double ), &nWritten );
	bool bCalcutated=!_isnan(m_fValue);
	pStream->Write( &bCalcutated, sizeof( bool ), &nWritten );
}

void CStatParamBase::Load( IStream *pStream, SerializeParams *pparams )
{
	long lVersion = 1;
	ULONG nWritten = 0;

	pStream->Read( &lVersion, sizeof( long ), &nWritten );
	pStream->Read( &m_fValue, sizeof( double ), &nWritten );
	bool bCalcutated=false;
	pStream->Read( &bCalcutated, sizeof( bool ), &nWritten );
	if(!bCalcutated) m_fValue=std::numeric_limits<double>::quiet_NaN();
}

CClass& CStatParamBase::Class(const ObjectSpace::Class cls, const NumImg image)
{
	return m_pStat->m_ClassMap[ClassID(cls,image)];
}

// calc Mx per image with empty images
//bool CStatParamBase2::ImageXXEmpty(long parKey, double& Mx, double& fStdDev)
//{
//	int cImg=0; 
//	bool bReturn=true;
//	double y=0., y2=0.;
//	for(set<NumImg>::const_iterator it=m_pStat->m_setImages.begin();
//		it!=m_pStat->m_setImages.end(); ++it)
//	{
//		NumImg image=*it;
//		CStatParamBase& param=SubClass(image).Param(parKey);
//		if( param.Calculate() )
//		{
//			double val=param;
//			y += val;
//			y2 += val*val;
//			++cImg;
//		}else{
//			bReturn=false;
//		}
//	}
//	if( bReturn && cImg ){
//		Mx = y/cImg; 
//		if(&fStdDev)
//			fStdDev = cImg>1 ? sqrt((y2-y*y/cImg)/(cImg-1)) : -1.;
//		return true;
//	}else
//		return false;
//}

// calc Mx per image without empty images
bool CStatParamBase2::ImageXX(long parKey, long CalcMethod, double& Mx, double& fStdDev, long& nImg)
{
	int cImg=0; 
	double y=0., y2=0.;
	for(set<NumImg>::const_iterator it=m_pStat->m_setImages.begin();
		it!=m_pStat->m_setImages.end(); ++it)
	{
		NumImg image=*it;
		CClass& clsImg=SubClass(image);
		CStatParamBase2& param=(CStatParamBase2&)clsImg.Param(parKey);
		if(param.Calculate2(0)){
			double val=param.GetValue2(0);
			y += val;
			y2 += val*val;
			++cImg;
		}else if(0==clsImg.N()){
			;
		}else{
			return false;
		}
	}
	if(&nImg)
		nImg=cImg;
	if( cImg ){
		Mx=y/cImg; 
		if(cImg>1){
			double S2=(y2-y*y/cImg)/(cImg-1);
			fStdDev =  S2<0? 0. : sqrt(S2);
		}else{
			fStdDev = -1.;
		}
		return true;
	}else
		return false;
}


bool CStatParam_Count::Calculate()
{
	if( IsCalculated() )
		return true;

	long lImgCount = m_pStat->m_setImages.size();

	m_fValue=N();
	if( m_Class.m_lClass.img == AllImages )
	{
		if( lImgCount <= 1 ){
			m_fValueDiv = -1;
		}else{
			double y=0., y2=0.;
			for( set<NumImg>::const_iterator it=m_pStat->m_setImages.begin(); it!=m_pStat->m_setImages.end(); it++ )
			{
				double dVal=SubClass(*it).N();
				y +=dVal;
				y2 += dVal*dVal;
			}
			_ASSERT(m_fValue==y);
			double M = y/lImgCount;
			double S2 = (y2 - lImgCount*M*M)/(lImgCount-1);
			m_fValueDiv = sqrt( S2 );
		}
	}

	return true;
}

// Parameter 196624
bool CStatParam_Area::Calculate()
{
	if( IsCalculated() )
		return true;
	if(0==N())
	{
		CClass& cAll=Class(AllClasses,AllImages);
		if(m_Class==cAll){
			return false;
		}else{
			CStatParamBase& area=cAll.Par<CStatParamBase>(GetKey());
			if(area.Calculate()){
				m_fValue=0.;
				return true;
			}else{
				return false;
			}
		}
	}
	double fValue = 0.;
	bool bReturn=false;
	BEGIN_ROW();
		if(!_isnan(info->fValueArea)){
			fValue += info->fValueArea;
			bReturn=true;
		}
	END_ROW();
	if(bReturn)
		m_fValue = fValue;
	return bReturn;
}

// 196625
bool CStatParam_Length::Calculate()
{
	if( IsCalculated() )
		return true;
	if(0==N())
	{
		CClass& cAll=Class(AllClasses,AllImages);
		CStatParamBase& area=cAll.Par<CStatParamBase>(GetKey());
		if(m_Class==cAll){
			return false;
		}else{
			if(area.Calculate()){
				m_fValue=0.;
				return true;
			}else{
				return false;
			}
		}
	}
	double fValue = 0.;
	BEGIN_ROW()
		if(_isnan(info->fValueLength))
			return false;
		fValue += info->fValueLength;
	END_ROW()
	m_fValue = fValue;
	return true;
}

// Parameter 196626
bool CStatParam_UsedArea::Calculate()
{
	if( IsCalculated() )
		return true;

	if( m_Class.m_lClass.cls != AllClasses )
		return false;

	_bstr_t strName=L"_used_area_";
	if( m_Class.m_lClass.img != AllImages )
		strName += _bstr_t(m_Class.m_lClass.img);

	_variant_t var;
	m_pStat->GetProperty( strName, &var );
	HRESULT hr=VariantChangeType(&var,&var,0,VT_R8);
	_ASSERT(S_OK==hr);
	if(S_OK==hr){
		m_fValue = (double)var;
		if(m_fValue<=0.){
			m_fValue=std::numeric_limits<double>::quiet_NaN();
			return false;
		}else
			return true;
	}else{
		return false;
	}
}

// 196645
bool CStatParam_FullArea::Calculate()
{
	if( IsCalculated() )
		return true;

	if( m_Class.m_lClass.cls != AllClasses )
		return false;

	m_fValue = 0;

	_variant_t var;
	m_pStat->GetProperty( _bstr_t(L"_full_area_"), &var );
	
	m_fValue = (double)var;
	if(m_fValue<=0.){
		m_fValue=std::numeric_limits<double>::quiet_NaN();
		return false;
	}else
		return true;
}

// 196644
bool CStatParam_ImageCount::Calculate()
{
	if( IsCalculated() )
		return true;

	INamedPropBagPtr sptrProp = m_pStat;

	_variant_t var;
	sptrProp->GetProperty( _bstr_t( "_image_count_" ), &var );
	
	m_fValue = (double)var;
	return true;
}

bool CStatParam_ChiSquare::Calculate()
{
	if( IsCalculated() )
		return true;

	if( m_Class.m_lClass.img != AllImages )
		return false;
	long lCount=N();
	if(lCount<50)
		return false;

	double fMX, fStdDev;
	
 	IStatObjectDispPtr sptrDisp = m_pStat;
	if( GetKey() == PARAM_CHI_SQUARE )
	{
		CStatParam_MX& mx=m_Class.Par<CStatParam_MX>(PARAM_MX);
		if(!mx.Calculate2(0))
			return false;
		CStatParam_StdDev& dx=m_Class.Par<CStatParam_StdDev>(PARAM_STDDEV);
		if(!dx.Calculate2(0))
			return false;
		fMX = mx.GetValue2(0);
		fStdDev = dx.GetValue2(0);
	}
	else
	{
		CStatParamBase& mx=m_Class.Param(PARAM_MATHXLOG);
		if(!mx.Calculate())
			return false;
		CStatParamBase& dx=m_Class.Param(PARAM_DXLOG);
		if(!dx.Calculate())
			return false;
		fMX = log((double)mx);
		fStdDev = M_LN10*sqrt((double)dx);
	}

 	int nCouter = _get_count(lCount);
	double fRange = 1. / nCouter;

	valarray<double> plFreq(0.,nCouter);
	valarray<double> plPi(0.,nCouter);

	valarray<double> intervals(nCouter-1);
	for(unsigned i=0; i<intervals.size(); ++i)
	{
		intervals[i]=invnormaldistribution(fRange*(i+1))*fStdDev + fMX;
		if( GetKey() == PARAM_CHI_SQUARE_LOG ){
			intervals[i]=exp(intervals[i]);
		}
	}

	{BEGIN_CALC();
		unsigned i;
		for( i = 0; i < intervals.size(); ++i )
		{
			if(fVal <= intervals[i])
			{
				plFreq[i]++;
				goto IntervalDefined;
			}
		}
		++plFreq[i];
		IntervalDefined: ;
	END_CALC();}

  for( long i = 0; i < nCouter; i++ )
	{
 		plPi[i] = double(lCount)/double(nCouter);
	}

	double fRetVal = 0.;
	for( long i = 0; i < nCouter; i++ ){
		double sqr=plFreq[i] - plPi[i];
		fRetVal += sqr * sqr / plPi[i];
	}
   
	if( !_finite( fRetVal ) )
		return false;
	m_fValue =  fRetVal;
	return true;
}

bool CStatParam_Chi2Prob::Calculate()
{
	if( IsCalculated() )
		return true;

	if( m_Class.m_lClass.img != AllImages )
		return false;
	if(N()<50)
		return false;

 	int nCouter = _get_count( N() );
	CStatParam_ChiSquare& chi2 = m_Class.Par<CStatParam_ChiSquare>(PARAM_CHI_SQUARE);
	if( !chi2.Calculate() )
		return false;

	// Число степеней свободы для распределения хи2 = nCouter-3
	m_fValue = 1. - chisquaredistribution(nCouter-3, chi2);

	return true;
}

bool CStatParam_Chi2ProbLog::Calculate()
{
	if( IsCalculated() )
		return true;

	if( m_Class.m_lClass.img != AllImages )
		return false;
	if(N()<50)
		return false;

 	int nCouter = _get_count( N() );
	CStatParam_ChiSquareLog& chi2 = m_Class.Par<CStatParam_ChiSquareLog>(PARAM_CHI_SQUARE_LOG);
	if( !chi2.Calculate() )
		return false;

	// Число степеней свободы для распределения хи2 = nCouter-3
	m_fValue =   1. - chisquaredistribution(nCouter-3, chi2);

	return true;
}

bool CStatParam_Chi2Critic::Calculate()
{
	if( IsCalculated() )
		return true;

	if( m_Class.m_lClass.img != AllImages )
		return false;
	if(N()<50)
		return false;

 	int nCouter = _get_count( N() );

	// Число степеней свободы для распределения хи2 = nCouter-3
	m_fValue =  invchisquaredistribution(nCouter-3, pParErrDscr->m_fProb);

	return true;
}

// 196627 Удельная площадь
bool CStatParam_SpecificArea::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if(!CalcMethod || AllImages != m_Class.m_lClass.img)
	{
		CStatParamBase& UsedArea=Class(AllClasses,m_Class.m_lClass.img).Param(PARAM_USEDAREA);
		if( !UsedArea.Calculate() )
			return false;
		double fValueUsedArea = UsedArea.GetValue();

		CStatParamBase& area=m_Class.Param(PARAM_AREA);
		if( !area.Calculate() )
			return false;
		double fValueArea = area.GetValue();

		if( _is_zero( fValueUsedArea ) )
			return false;

		SetValue2(CalcMethod, fValueArea / fValueUsedArea);
		return true;
	}
	else
	{
		double fMxPerImage, fStdDev;
		long nImgs;
		if(ImageXX(PARAM_SPECAREA,CalcMethod,fMxPerImage,fStdDev, nImgs)){
			SetValue2(CalcMethod, fMxPerImage);
			m_ImgCnt=nImgs;
			if(fStdDev>=0.){
				m_fValueDiv=fStdDev/sqrt(double(nImgs)); 
			}
			return true;
		}else
			return false;
	}
}

// 196628 
bool CStatParam_SpecificAreaError::Calculate()
{
	if( IsCalculated() )
		return true;
	if(!pParDscr->m_lCalcFrom)
		return false;
	CStatParamBase2& m_SpecArea=(CStatParamBase2&)m_Class.Param(PARAM_SPECAREA);
	if( !m_SpecArea.Calculate2(1) )
		return false;
	double fValue = m_SpecArea.GetValueEx();
	if(fValue<0.)
		return false;
	long lCount=m_SpecArea.m_ImgCnt;
	m_fValue = invstudenttdistribution( lCount - 1, 0.5+0.5*pParErrDscr->m_fProb ) * fValue;
        
	return true;
}

// 196629
bool CStatParam_AreaPerc::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if(!CalcMethod || AllImages != m_Class.m_lClass.img)
	{
		CStatParamBase& area2=Class(CalcForRelativeClasses,m_Class.m_lClass.img).Param(PARAM_AREA);
		if( !area2.Calculate() )
				return false;
		double fAreaAll=area2;

		if( _is_zero( fAreaAll ) )
			return false;

		CStatParamBase& area=m_Class.Param(PARAM_AREA);
		if( !area.Calculate() )
				return false;
		double fArea = area.GetValue();

		SetValue2(CalcMethod, fArea / fAreaAll * 100);
		return true;
	}
	else   
	{
		double Mx, fStdDev;
		long nImgs;
		if(ImageXX(PARAM_AREAPERC,CalcMethod,Mx, fStdDev, nImgs)){
			SetValue2(CalcMethod, Mx);
			m_ImgCnt=nImgs;
			if(fStdDev>=0.){
				m_fValueDiv=fStdDev/sqrt(double(nImgs)); 
			}
			return true;
		}else{
			return false;
		}
	}
}

// 196630
bool CStatParam_AreaPercError::Calculate()
{
	if( IsCalculated() )
		return true;
	if(!pParDscr->m_lCalcFrom)
		return false;

	CStatParam_AreaPerc& m_AreaPerc = m_Class.Par<CStatParam_AreaPerc>(PARAM_AREAPERC);
	if( !m_AreaPerc.Calculate2(1) )
		return false;
	double fValue = m_AreaPerc.GetValueEx();
	if(fValue<0.)
		return false;
	long lCount=m_AreaPerc.m_ImgCnt;
	m_fValue = invstudenttdistribution( lCount - 1, 0.5+0.5*pParErrDscr->m_fProb ) * fValue;
	return true;
}

// 196631 Норм. площадь
bool CStatParam_NormalArea::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	CStatParamBase2& m_area=(CStatParamBase2&)m_Class.Param(PARAM_SPECAREA);
	if( !m_area.Calculate2(CalcMethod) )
		return false;
	SetValue2(CalcMethod, m_area.GetValue2(CalcMethod) * pNormalParDscr->m_fNormal);
	return true;
}

// 196632
bool CStatParam_SpecificCount::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if(!CalcMethod || AllImages != m_Class.m_lClass.img)
	{
		CStatParamBase& usedArea=Class(AllClasses,m_Class.m_lClass.img).Param(PARAM_USEDAREA);
		if( !usedArea.Calculate() )
			return false;
		double fValueUsedArea = usedArea.GetValue();

		if( _is_zero( fValueUsedArea ))
			return false;
		SetValue2(CalcMethod, m_Class.N() / usedArea);
		return true;
	}
	else			  
	{
		double Mx, fStdDev;
		long nImgs;
		if(ImageXX(PARAM_SPECCOUNT,CalcMethod,Mx, fStdDev, nImgs)){
			SetValue2(CalcMethod, Mx);
			m_ImgCnt=nImgs;
			if(fStdDev>=0.){
				m_fValueDiv=fStdDev/sqrt(double(nImgs)); 
			}
			return true;
		}else{
			return false;
		}
	}
}

// 196633
bool CStatParam_SpecificCountError::Calculate()
{
	if( IsCalculated() )
		return true;
	if(!pParDscr->m_lCalcFrom)
		return false;

	CStatParamBase2& m_SpecCount=m_Class.Par<CStatParam_SpecificCount>(PARAM_SPECCOUNT);
	if( !m_SpecCount.Calculate2(1) )
		return false;
	double fValue = m_SpecCount.GetValueEx();
	if(fValue<0.)
		return false;
	long lCount=m_SpecCount.m_ImgCnt;
	m_fValue = invstudenttdistribution( lCount - 1, 0.5+0.5*pParErrDscr->m_fProb ) * fValue;
        
	return true;
}

// 196634 Процент количества
bool CStatParam_CountPerc::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;
	if(!CalcMethod || AllImages != m_Class.m_lClass.img)
	{
		double fValueC=N();
		CClass& cRelative=Class(CalcForRelativeClasses,m_Class.m_lClass.img);
 		double fValueCAll = cRelative.N();

		if( _is_zero( fValueCAll ) )
			return false;
		SetValue2(CalcMethod, 100. * N() / fValueCAll);
		return true;
	}
	else
	{
		double Mx, fStdDev;
		long nImgs;
		if(ImageXX(PARAM_COUNTPERC,CalcMethod,Mx, fStdDev, nImgs)){
			SetValue2(CalcMethod, Mx);
			m_ImgCnt=nImgs;
			if(fStdDev>=0.){
				m_fValueDiv=fStdDev/sqrt(double(nImgs)); 
			}
			return true;
		}else{
			return false;
		}
	}
}

// 196635
bool CStatParam_CountPercError::Calculate()
{
	if( IsCalculated() )
		return true;
	if(!pParDscr->m_lCalcFrom)
		return false;

	CStatParamBase2& m_CountPerc=m_Class.Par<CStatParam_CountPerc>(PARAM_COUNTPERC);
	if( !m_CountPerc.Calculate2(1) )
		return false;
	double fValue = m_CountPerc.GetValueEx();
	if(fValue<0.)
		return false;
	long lCount=m_CountPerc.m_ImgCnt;
	m_fValue = invstudenttdistribution( lCount - 1, 0.5+0.5*pParErrDscr->m_fProb ) * fValue;
        
	return true;
}

// 196636 Норм. кол-во
bool CStatParam_NormalCount::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	CStatParamBase2& m_area=(CStatParamBase2&)m_Class.Param(PARAM_SPECCOUNT);
	if( !m_area.Calculate2(CalcMethod) )
		return false;
	SetValue2(CalcMethod, m_area.GetValue2(CalcMethod) * pNormalParDscr->m_fNormal);
	return true;
}

// 196637
bool CStatParam_SpecificLength::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if(!CalcMethod || AllImages != m_Class.m_lClass.img)
	{
		CStatParamBase& m_UsedArea=Class(AllClasses,m_Class.m_lClass.img).Param(PARAM_USEDAREA);
		if( !m_UsedArea.Calculate() )
			return false;
		double fValueUsedArea = m_UsedArea;

		CStatParamBase& m_length=m_Class.Param(PARAM_LENGTH);
		if( !m_length.Calculate() )
			return false;
		double fValueLength = m_length;

		if( _is_zero( fValueUsedArea ) )
			return false;

		SetValue2(CalcMethod, fValueLength / fValueUsedArea);
		return true;
	}
	else
	{
		double Mx, fStdDev;
		long nImgs;
		if(ImageXX(PARAM_SPECLENGTH,CalcMethod,Mx, fStdDev, nImgs)){
			SetValue2(CalcMethod, Mx);
			m_ImgCnt=nImgs;
			if(fStdDev>=0.){
				m_fValueDiv=fStdDev/sqrt(double(nImgs)); 
			}
			return true;
		}else{
			return false;
		}
	}
}

// 196638
bool CStatParam_SpecificLengthError::Calculate()
{
	if( IsCalculated() )
		return true;
	if(!pParDscr->m_lCalcFrom)
		return false;

	CStatParamBase2& m_SpecLength = (CStatParamBase2&)m_Class.Param(PARAM_SPECLENGTH);
	if( !m_SpecLength.Calculate2(1) )
		return false;
	double fValue = m_SpecLength.GetValueEx();
	if(fValue<0.)
		return false;
	long lCount=m_SpecLength.m_ImgCnt;
	m_fValue = invstudenttdistribution( lCount - 1, 0.5+0.5*pParErrDscr->m_fProb ) * fValue;
        
	return true;
}

// 196639
bool CStatParam_3DPart::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	CStatParamBase2& m_SpecArea=(CStatParamBase2&)m_Class.Param(PARAM_SPECAREA);
	if( !m_SpecArea.Calculate2(CalcMethod) )
		return false;
	SetValue2(CalcMethod, m_SpecArea.GetValue2(CalcMethod));
        
	return true;
}

// 196610
bool CStatParam_MX::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	double fValue = 0.;

	if(!CalcMethod || AllImages != m_Class.m_lClass.img)
	{
		long lFullCount = N();
			BEGIN_ROW();
				fValue += fVal;
			END_ROW();
        
		if( !lFullCount )
			return false;

		SetValue2(CalcMethod, fValue/lFullCount);
		return true;
	}
	else
	{
		double Mx;
		if(ImageXX(PARAM_MX,CalcMethod,Mx, m_fValueDiv)){
			SetValue2(CalcMethod, Mx);
			return true;
		}else{
			return false;
		}
	}
}

// 196640
bool CStatParam_MXError::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if(!CalcMethod || m_Class.m_lClass.img != AllImages)
	{
		CStatParamBase2& m_dx=(CStatParamBase2&)m_Class.Param(PARAM_DISP);
		if( !m_dx.Calculate2(CalcMethod) )
			return false;
		double fValueDX = m_dx.GetValue2(CalcMethod);

		SetValue2(CalcMethod, sqrt(fValueDX/N()));
		return true;
	}
	else
	{
		double fMxPerImage=0.;
		double fStdDev=-1;
		long Nimages;
		if(ImageXX(PARAM_MX, CalcMethod, fMxPerImage, fStdDev, Nimages))
		{
			if(fStdDev>=0.)
			{
				SetValue2(CalcMethod, fStdDev/sqrt((double)Nimages));
				return true;
			}
		}
		return false;
	}
}

// 196654
bool CStatParam_MXErrorLog::Calculate()
{
 	if( IsCalculated() )
		return true;

	CStatParamBase& m_dx=m_Class.Param(PARAM_DXLOG);
	if( !m_dx.Calculate() )
		return false;
	double fValueDX = m_dx.CStatParamBase::GetValue();

	m_fValue = sqrt( fValueDX / N() );

	return true;
}

// 196641
bool CStatParam_MXProb::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if(!CalcMethod || m_Class.m_lClass.img != AllImages)
	{
		CStatParamBase& m_dx=m_Class.Param(PARAM_DISP);
		if( !m_dx.Calculate() )
				return false;
		double fStdDev = sqrt( m_dx.GetValue() );

		int Nobjects=N();
		if(Nobjects>1){
			SetValue2(CalcMethod, invstudenttdistribution(Nobjects-1, 0.5+0.5*pParErrDscr->m_fProb ) * fStdDev / sqrt((double)Nobjects));
			return true;
		}
		return false;
	}
	else
	{
		double fStdDev=-1.;
		double fMxPerImage=0.;
		long Nimages;
		if(ImageXX(PARAM_MX, CalcMethod,fMxPerImage,fStdDev,Nimages))
		{
			if(fStdDev>=0.)
			{
				SetValue2(CalcMethod, invstudenttdistribution(Nimages-1, 0.5+0.5*pParErrDscr->m_fProb) * fStdDev / sqrt((double)Nimages));
				return true;
			}
		}
		return false;
	}
}

// 196655
bool CStatParam_MXProbLog::Calculate()
{
	if( IsCalculated() )
		return true;

	CStatParamBase& m_dx=m_Class.Param(PARAM_DXLOG);
	if( !m_dx.Calculate() )
		return false;

	double fValueSigma = sqrt( m_dx.CStatParamBase::GetValue() );

	CStatParamBase& m_count=m_Class.Param(PARAM_COUNT);
	if( !m_count.Calculate() )
		return false;

	double fValueCount = m_count.GetValue();
	if( fValueCount <= 1 )
		return false;

	m_fValue = invstudenttdistribution((int)fValueCount - 1, 0.5+0.5*pUserParDscr->m_fProb ) * fValueSigma / sqrt( fValueCount );

	return true;
}

// 196611
bool CStatParam_MXGeom::Calculate()
{
	if( IsCalculated() )
		return true;
	CStatParamBase& m_Count=m_Class.Param(PARAM_COUNT);
	if( !m_Count.Calculate() )
		return false;
	double fCount = m_Count.GetValue();
	if( !fCount )
		return false;

	m_fValue = 1.;

	BEGIN_ROW()
			m_fValue *= pow( fVal, 1 / fCount );
	END_ROW()

	return true;
}

// 196612
bool CStatParam_MXGorm::Calculate()
{
	if( IsCalculated() )
		return true;

	m_fValue = 0;

	BEGIN_ROW()
		if(0.==fVal)
			return false;
		m_fValue += 1. / fVal;
	END_ROW()

	if( _is_zero( m_fValue ))
		return false;

	m_fValue = N() / m_fValue;

	return true;
}

// 196613
bool CStatParam_Min::Calculate()
{
	if( IsCalculated() )
		return true;

	if( !N() )
		return false;

	BEGIN_ROW()
		if(_isnan(fVal)){
			return false;
		}else if(1==lCount || fVal < m_fValue)
			m_fValue = fVal;
	END_ROW()

	return true;
}

// 196614
bool CStatParam_Max::Calculate()
{
	if( IsCalculated() )
		return true;

	if( !N() )
		return false;

	BEGIN_ROW()
		if(_isnan(fVal)){
			return false;
		}else if(1==lCount || fVal > m_fValue)
			m_fValue = fVal;
	END_ROW()
	return true;
}

// 196615
bool CStatParam_Var::Calculate()
{
	if( IsCalculated() )
		return true;

	CStatParamBase& parMin=m_Class.Param(PARAM_MIN);
	if(!parMin.Calculate())
		return false;

	CStatParamBase& parMax=m_Class.Param(PARAM_MAX);
	if(!parMax.Calculate())
		return false;

	m_fValue = (double)parMax - (double)parMin;
	return true;
}

// 196616
bool CStatParam_Disp::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if( !CalcMethod || m_Class.m_lClass.img != AllImages )
	{
		unsigned lFullCount=N();
		if( lFullCount <= 1 )
			return false;

		double x=0., x2=0.;
		BEGIN_ROW()
			x += fVal;
			x2 += fVal*fVal;
		END_ROW()

		SetValue2(CalcMethod, (x2 - x*x/lFullCount) / ( lFullCount - 1 ));
		return true;
	}
	else
	{
		double fMxPerImage, fStandardDeviation=-1.;
		int Nimages=m_pStat->m_setImages.size();
		if(Nimages>1 && ImageXX(PARAM_MX, CalcMethod, fMxPerImage, fStandardDeviation))
		{
			if(fStandardDeviation>=0.)
			{
				SetValue2(CalcMethod, fStandardDeviation * fStandardDeviation);
				return true;
			}
		}
		return false;
	}
}

// 196617
bool CStatParam_StdDev::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	CStatParam_Disp& m_DX=(CStatParam_Disp&)m_Class.Param(PARAM_DISP);
	if( !m_DX.Calculate2(CalcMethod) )
		return false;
	double fValueDX = m_DX.GetValue2(CalcMethod);

	SetValue2(CalcMethod,sqrt(fValueDX));

	return true;
}

// 196653
bool CStatParam_StdDevLog::Calculate()
{
	if( IsCalculated() )
		return true;

	CStatParamBase& m_dx_log = m_Class.Param(PARAM_DXLOG);
 	if( !m_dx_log.Calculate() )
		return false;

	double fValueDX = m_dx_log.CStatParamBase::GetValue();

	m_fValue = sqrt( fValueDX );

	return true;
}

// 196618
bool CStatParam_Mode::Calculate()
{
	if( IsCalculated() )
		return true;

	unsigned lFullCount=N();
	if( lFullCount <= 0 )
		return false;

	CStatParamBase& parMin=m_Class.Param(PARAM_MIN);
	if(!parMin.Calculate())
		return false;

	CStatParamBase& parMax=m_Class.Param(PARAM_MAX);
	if(!parMax.Calculate())
		return false;

	double fMin = parMin.GetValue();
	double fMax = parMax.GetValue();

 	int nCouter = _get_count( lFullCount );

	double fRange = ( fMax - fMin ) / nCouter;

	valarray<long> plFreq(0L,nCouter);
	{
		BEGIN_ROW();
			for( long i = 1; i < nCouter; i++ )
			{
				if( fVal <= ( fMin + fRange * i) )
					{
						plFreq[i-1]++;
					goto Next;
				}
			}
			(plFreq[nCouter-1])++;
Next:;
		END_ROW()
	}

	long nID = 0;
	{
		long lMaxVal = plFreq[0];
		for( long i = 1; i < nCouter; i++ )
		{
			if( plFreq[i] > lMaxVal )
			{
				lMaxVal = plFreq[i];
				nID = i;
			}
		}
	}

	if( nID > 0 && nID < nCouter - 1 ){
		long curv=2*plFreq[nID]-plFreq[nID-1]-plFreq[nID+1];
		if(0==curv){
			if(plFreq[nID-1]>plFreq[nID+1])
				m_fValue = fMin + fRange * (nID-1);
			else
				m_fValue = fMin + fRange * (nID+1);
		}else{
			m_fValue = fMin + fRange * nID + fRange * ( plFreq[nID] - plFreq[nID-1] ) / double(curv);
		}
	}else if( nID == 0 )
		m_fValue = fMin + fRange * nID + 0.5 * fRange;
	else if( nID == nCouter - 1 )
		m_fValue = fMin + fRange * nID + 0.5 * fRange;

	return true;
}

// 196656
bool CStatParam_ModeLog::Calculate()
{
	if( IsCalculated() )
		return true;

	unsigned lCount=N();
	if( lCount <= 0 )
		return false;

	CStatParamBase& parMin=m_Class.Param(PARAM_MIN);
	if(!parMin.Calculate())
		return false;

	CStatParamBase& parMax=m_Class.Param(PARAM_MAX);
	if(!parMax.Calculate())
		return false;

	double fMin = parMin.GetValue();
	if(fMin<=0.)
		return false;
	fMin = log10(fMin);
	double fMax = parMax.GetValue();
	fMax=log10(fMax);

 	int nCouter = _get_count( lCount );

	double fRange = ( fMax - fMin ) / nCouter;

	valarray<long> plFreq(0L,nCouter);

	{
		BEGIN_ROW();
			for( long i = 1; i < nCouter; i++ )
			{
				double fvalue = log10( fVal );

				if( fvalue <= ( fMin + fRange * i) )
				{
					(plFreq[i-1])++;
					goto Next;
				}
			}
			(plFreq[nCouter-1])++;
Next:;
			END_ROW();
	}

	long lMaxVal = plFreq[0];
	long nID = 0;

	for( long i = 1; i < nCouter; i++ )
	{
		if( plFreq[i] > lMaxVal )
		{
			lMaxVal = plFreq[i];
			nID = i;
		}
	}

	if( nID > 0 && nID < nCouter - 1 )
	{
		long curv=2*plFreq[nID]-plFreq[nID-1]-plFreq[nID+1];
		if(0==curv){
			if(plFreq[nID-1]>plFreq[nID+1])
				m_fValue = fMin + fRange * (nID-1);
			else
				m_fValue = fMin + fRange * (nID+1);
		}else
		{
			m_fValue = fMin + fRange * nID + fRange * ( plFreq[nID] - plFreq[nID-1] ) / double(curv);
		}
	}
	else if( nID == 0 )
		m_fValue = fMin + fRange * nID + 0.5 * fRange;
	else if( nID == nCouter - 1 )
		m_fValue = fMin + fRange * nID + 0.5 * fRange;

	return true;
}

bool CStatParam_Mediane::Calculate()
{
	if( IsCalculated() )
		return true;

	CStatParamBase& parMin=m_Class.Param(PARAM_MIN);
	if(!parMin.Calculate())
		return false;

	CStatParamBase& parMax=m_Class.Param(PARAM_MAX);
	if(!parMax.Calculate())
		return false;

	double fMin = parMin.GetValue();
	double fMax = parMax.GetValue();

	long larr_count = N();
	
	double *pArr = new double[larr_count];
	// [vanek] : fill with zeros - 04.01.2005
	::ZeroMemory( pArr, sizeof(double) * larr_count );

	{
		// [vanek] - 04.01.2005
		long larr_idx = 0;
			BEGIN_ROW();
				if( larr_idx >= 0 && larr_idx < larr_count )
					pArr[larr_idx] = fVal;
				larr_idx ++;
			END_ROW();
	}
	
//	sort_templ( pArr, larr_count, 1 );
	std::sort(pArr, pArr+larr_count, std::greater<double>( ) );

	long lID = larr_count / 2;

	if( !( larr_count % 2 ) )
		m_fValue = ( pArr[lID] + pArr[lID-1] ) / 2;
	else
		m_fValue = pArr[lID];
	
	delete []pArr;

	return true;
}

bool CStatParam_Moment::Calculate()
{
	if( IsCalculated() )
		return true;

	CStatParam_MX& mx=m_Class.Par<CStatParam_MX>(PARAM_MX);
	if(!mx.Calculate2(0))
		return false;
	double fMX = mx.GetValue2(0);

	double fValue = 0.;

	unsigned lFullCount=N();
	if( lFullCount <= 0 )
		return false;
	BEGIN_ROW();
		fValue += pow( fVal - fMX, m_lMoment );
	END_ROW();

	m_fValue = fValue / double( lFullCount ); 

	return true;
}

bool CStatParam_MomentLog::Calculate()
{
	if( IsCalculated() )
		return true;

	CStatParamBase& m_MX=m_Class.Param(PARAM_MATHXLOG);
	if( !m_MX.Calculate() )
		return false;

	double fMX = m_MX.CStatParamBase::GetValue();

	m_fValue = 0;

	unsigned lFullCount=N();
	if( lFullCount <= 0 )
		return false;
	BEGIN_ROW()
			m_fValue += pow( log10( fVal ) - fMX, m_lMoment );
	END_ROW()

	m_fValue *= 1 / double( lFullCount ); 

	return true;
}

bool CStatParam_Assimetry::Calculate()
{
	if( IsCalculated() )
		return true;

	CStatParamBase& m_count=m_Class.Param(PARAM_COUNT);
	if( !m_count.Calculate() )
		return false;
	double fCount = m_count.GetValue();

	if( fCount <= 2 )
		return false;

	if( !m_Moment.Calculate() )
		return false;

	double fMoment = m_Moment.GetValue();

	m_fValue = 0;

	CStatParam_StdDev& m_StdDev=(CStatParam_StdDev&)m_Class.Param(PARAM_STDDEV);
	if( !m_StdDev.Calculate2(0) )
		return false;
	double fSigma = m_StdDev.GetValue2(0);

	if( _is_zero( fSigma ) )
		return false;

	m_fValue = fCount * fCount * fMoment / pow( fSigma, 3 ) / ( fCount - 1 ) / ( fCount - 2 ); 

	return true;
}

bool CStatParam_AssimetryLog::Calculate()
{
	if( IsCalculated() )
		return true;

	CStatParamBase& m_count=m_Class.Param(PARAM_COUNT);
	if( !m_count.Calculate() )
			return false;
	double fCount = m_count.GetValue();

	if( fCount <= 2 )
		return false;

	if( !m_Moment.Calculate() )
		return false;

	double fMoment = m_Moment.CStatParamBase::GetValue();

	m_fValue = 0;

	CStatParam_StdDev& m_StdDev=(CStatParam_StdDev&)m_Class.Param(PARAM_STD_DEV_LOG);
	if( !m_StdDev.Calculate() )
					return false;

	double fSigma = m_StdDev.CStatParamBase::GetValue();

	if( _is_zero( fSigma ) )
			return false;

	m_fValue = fCount * fCount * fMoment / pow( fSigma, 3 ) / ( fCount - 1 ) / ( fCount - 2 ); 

	return true;
}

bool CStatParam_Excess::Calculate()
{
	if( IsCalculated() )
		return true;

	CStatParamBase& m_count=m_Class.Param(PARAM_COUNT);
	if( !m_count.Calculate() )
		return false;
	double fCount = m_count.GetValue();

	if( fCount <= 3 )
		return false;

	if( !m_Moment.Calculate() )
		return false;

	double fMoment = m_Moment.GetValue();

	m_fValue = 0;

	CStatParam_StdDev& m_StdDev=(CStatParam_StdDev&)m_Class.Param(PARAM_STDDEV);
	if( !m_StdDev.Calculate2(0) )
		return false;
	double fSigma = m_StdDev.GetValue2(0);

	if( _is_zero( fSigma ) )
		return false;

	m_fValue = fCount * fCount * ( fCount + 1 )* fMoment / pow( fSigma, 4 ) / ( fCount - 1 ) / ( fCount - 2 ) / ( fCount - 3 ) - 3 * ( fCount - 1) * ( fCount - 1) / ( fCount - 2 ) / ( fCount - 3 ); 

	return true;
}

bool CStatParam_ExcessLog::Calculate()
{
	if( IsCalculated() )
		return true;

	CStatParamBase& m_count=m_Class.Param(PARAM_COUNT);
	if( !m_count.Calculate() )
			return false;
	double fCount = m_count.GetValue();

	if( fCount <= 3 )
		return false;

	if( !m_Moment.Calculate() )
		return false;

 	double fMoment = m_Moment.CStatParamBase::GetValue();

	m_fValue = 0;

	CStatParam_StdDevLog& stdDevLog=m_Class.Par<CStatParam_StdDevLog>(PARAM_STD_DEV_LOG);
	if( !stdDevLog.Calculate() )
					return false;

	double fSigma = stdDevLog.CStatParamBase::GetValue();

	if( _is_zero( fSigma ) )
			return false;

	m_fValue = fCount * fCount * ( fCount + 1 )* fMoment / pow( fSigma, 4 ) / ( fCount - 1 ) / ( fCount - 2 ) / ( fCount - 3 ) - 3 * ( fCount - 1) * ( fCount - 1) / ( fCount - 2 ) / ( fCount - 3 ); 

	return true;
}

// 196622
bool CStatParam_VarCoeff::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	CStatParamBase2& m_MX=(CStatParamBase2&)m_Class.Param(PARAM_MX);
	if( !m_MX.Calculate2(CalcMethod) )
		return false;
	double fMX = m_MX.GetValue2(CalcMethod);

	if( _is_zero( fMX ) )
		return false;

	CStatParam_StdDev& m_StdDev=(CStatParam_StdDev&)m_Class.Param(PARAM_STDDEV);
	if( !m_StdDev.Calculate2(CalcMethod) )
		return false;
	double fSigma = m_StdDev.GetValue2(CalcMethod);

	SetValue2(CalcMethod, fSigma / fMX * 100); 

	return true;
}

bool CStatParam_VarCoeffLog::Calculate()
{
	if( IsCalculated() )
		return true;

	CStatParamBase& m_MXLog2=m_Class.Param(PARAM_MATHXLOG);
	if( !m_MXLog2.Calculate() )
		return false;

	double fMX = m_MXLog2.CStatParamBase::GetValue();

	if( _is_zero( fMX ) )
		return false;

	CStatParamBase& m_StdDevLog=m_Class.Param(PARAM_STD_DEV_LOG);
	if( !m_StdDevLog.Calculate() )
		return false;

	double fSigma = m_StdDevLog.CStatParamBase::GetValue();

	m_fValue = fSigma / fMX * 100.; 

	return true;
}

_OBJECT_LIST::~_OBJECT_LIST()
{
//	for(iterator it=begin(); it!=end(); ++it) delete *it;
}

}

// ParamSpace

//PARAMS_LIST::~PARAMS_LIST(){for(iterator it=begin(); it!=end(); ++it) delete it->second;}

void PARAMS_LIST::clear(){
			for(iterator it=begin(); it!=end(); ++it) delete it->second;
	map<long,ParamSpace::CStatParamBase*>::clear();
}

bool PARAMS_LIST::add_tail(ParamSpace::CStatParamBase* pParam)
{
	return insert(value_type(pParam->GetKey(),pParam)).second;
}

// find matching CClass in ClassMap, if not create it
CClass& CClass::TrieveClass(CStatisticObject& stat, ClassID ClassId)
{
	std::pair<CStatisticObject::ClassMap::iterator,bool> 
		res_pair=stat.m_ClassMap.insert(CStatisticObject::ClassMap::value_type(ClassId,NULL));
	ClassMap::iterator itClass=res_pair.first;
	if(res_pair.second){
		itClass->second=new CClass(stat, ClassId);
	}
	_ASSERT(itClass->second);
	if(!itClass->second->m_pObjList && stat.m_pObjectList)
	{
		itClass->second->ChooseObjects();
	}
	return *itClass->second;
}

CClass& CClass::TrieveSub(const NumImg image)
{
	_ASSERT(AllImages==m_lClass.img);
	ClassID classId=ClassID(m_lClass.cls, image);
	return TrieveClass(stat,classId);
}

ParamSpace::CStatParamBase&  CClass::Param(long paramKey)
{
	std::pair<PARAMS_LIST::iterator,bool> 
		res_pair=insert(PARAMS_LIST::value_type(paramKey,NULL));
	PARAMS_LIST::iterator it=res_pair.first;
	if(res_pair.second){
		it->second=stat.CreateStatParam(*this, ::GetAppUnknown(), paramKey);
		if(NULL==it->second){
			erase(it);
			return *(ParamSpace::CStatParamBase*)NULL;
		}
	}
	_ASSERTE(it->second);
	return *it->second;
}
	
class eq //:public std::unary_function<ParamSpace::_OBJECT_LIST::value_type,bool>
{
	long cls;
public:
	eq(long cls):cls(cls){}
	bool operator()(const ParamSpace::_OBJECT_LIST::value_type& obj){
		return obj->lClass==cls;
	}
};
	
class inImage //:public std::unary_function<ParamSpace::_OBJECT_LIST::value_type,bool>
{
	long img;
public:
	inImage(long img):img(img){}
	bool operator()(const ParamSpace::_OBJECT_LIST::value_type& obj){
		return obj->iImage==img;
	}
};

class includes//:public std::unary_function<ParamSpace::_OBJECT_LIST::value_type,bool>
{
	std::valarray<bool> cls;
public:
	includes(const vector<long>& classes){
		cls.resize(classes[classes.size()-1]+1);
		cls=false;
		long ll=(long)classes.size();
		for(long i=0; i<(long)classes.size(); ++i){
			long ic=classes[i];
			_ASSERT(0<=classes[i] && classes[i]<(long)cls.size());
			cls[classes[i]]=true;
		}
	}
	bool operator()(const ParamSpace::_OBJECT_LIST::value_type& obj){
		_ASSERT(0<=obj->lClass && obj->lClass<(long)cls.size());
		return cls[obj->lClass];
	}
};

class inClassSet
{
	const set<long> classes;
public:
	inClassSet(const set<long>& classes) : classes(classes){}
	bool operator()(const ParamSpace::_OBJECT_LIST::value_type& obj){
		set<long>::const_iterator it=classes.find(obj->lClass);
		return 0==classes.size() || it!=classes.end();
	}
};

template< class _Pr> inline
void CClass::FindIfObjects(const ParamSpace::_OBJECT_LIST& objectList, _Pr _Pred)
{
	_ASSERT(&objectList);
	for(ParamSpace::_OBJECT_LIST::const_iterator it=objectList.begin();it!=objectList.end();++it){
		it=std::find_if(it,objectList.end(), _Pred);
		if(it==objectList.end())
			break;
		m_pObjList->push_back(*it);
	}
}

void CClass::ChooseObjects()
{
	if(m_pObjList)
		return;
	_ASSERT(stat.m_pObjectList);
	if(!stat.m_pObjectList)
		return;
	if(!m_pObjList)
		m_pObjList=new ParamSpace::_OBJECT_LIST();

	NumImg img=m_lClass.img;
	Class nClass=m_lClass.cls;
	if(AllImages==img){
		const ParamSpace::_OBJECT_LIST& objectList=*stat.m_pObjectList;
		if(nClass>AllClasses){
			FindIfObjects(objectList,eq(nClass));
		}else if(AllClasses==nClass){
			FindIfObjects(objectList,inClassSet(stat.m_setClassesForAll));
		}else if(CalcForRelativeClasses==nClass){
			FindIfObjects(objectList, inClassSet(stat.m_setClassesRelative));
		//}else if(CalcForAllClasses==nClass){
		//	FindIfObjects(objectList,inClassSet(stat.m_setClassesForAll));
		}
	}else{
		ClassID classId=ClassID(m_lClass.cls, AllImages);
		CClass& clsBase=TrieveClass(stat,classId);
		const ParamSpace::_OBJECT_LIST& objectList=*clsBase.m_pObjList;
		FindIfObjects(objectList, inImage(img));
	}
}

}


// 196646
bool CStatParam_Perimeter::Calculate()
{
	if( IsCalculated() )
		return true;
	if(0==N())
	{
		CClass& cAll=Class(AllClasses,AllImages);
		if(m_Class==cAll){
			return false;
		}else{
			CStatParamBase& area=cAll.Par<CStatParamBase>(GetKey());
			if(area.Calculate()){
				m_fValue=0.;
				return true;
			}else{
				return false;
			}
		}
	}
	double fValue = 0.;

	BEGIN_ROW()
		if(_isnan(info->fValuePerim))
			return false;
		fValue += info->fValuePerim;
	END_ROW()
	m_fValue=fValue;

	return true;
}

// 196647
bool CStatParam_SurfArea::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if(!CalcMethod || m_Class.m_lClass.img != AllImages )
	{
		CStatParamBase& areaUsed=Class(AllClasses,m_Class.m_lClass.img).Param(PARAM_USEDAREA);
		if( !areaUsed.Calculate() )
			return false;
		double fAreaUsed = areaUsed;

		CStatParamBase& m_perim=m_Class.Param(PARAM_PERIMETR);
		if( !m_perim.Calculate() )
			return false;
		double fPerim = m_perim;

		if( _is_zero( fAreaUsed ) )
			return false;
		SetValue2(CalcMethod, 4 / M_PI * fPerim / fAreaUsed); 
		return true;
	}
	else
	{
		double fMxPerImage,fValueDiv=-1.;
		if(ImageXX(PARAM_SURF_AREA,CalcMethod,fMxPerImage,fValueDiv)){
			SetValue2(CalcMethod, fMxPerImage); 
			return true;
		}else
			return false;
	}
}

// 196648
bool CStatParam_MdlChord::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if( !CalcMethod || m_Class.m_lClass.img != AllImages )
	{
		CStatParamBase2& m_Vv = (CStatParamBase2&)m_Class.Param(PARAM_3DPART);
		if( !m_Vv.Calculate2(0) )
			return false;
		double fVv = m_Vv.GetValue2(0);

		CStatParamBase2& m_Sv = (CStatParamBase2&)m_Class.Param(PARAM_SURF_AREA);
		if( !m_Sv.Calculate2(0) )
			return false;
		double fSv = m_Sv.GetValue2(0);

		if( _is_zero( fSv ) )
			return false;


		SetValue2(CalcMethod, 4 * fVv / fSv); 
		return true;
	}
	else
	{
		double fMxPerImage,fValueDiv=-1.;
		if(ImageXX(PARAM_MDL_CHORD,CalcMethod,fMxPerImage,fValueDiv)){
			SetValue2(CalcMethod, fMxPerImage); 
			return true;
		}else
			return false;
	}
}

// 196649
bool CStatParam_MdlDist::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if( !CalcMethod || m_Class.m_lClass.img != AllImages )
	{
		CStatParamBase2& m_Vv = (CStatParamBase2&)m_Class.Param(PARAM_3DPART);
		if( !m_Vv.Calculate2(0) )
			return false;
		double fVv = m_Vv.GetValue2(0);

		CStatParamBase2& m_Sv = (CStatParamBase2&)m_Class.Param(PARAM_SURF_AREA);
		if( !m_Sv.Calculate2(0) )
			return false;
		double fSv = m_Sv.GetValue2(0);

		if( _is_zero( fSv ) )
			return false;

		SetValue2(CalcMethod, fabs( 4. * ( 1. - fVv ) / fSv )); 
		return true;
	}
	else
	{
		double fMxPerImage,fValueDiv=-1.;
		if(ImageXX(PARAM_MDL_DIST,CalcMethod,fMxPerImage,fValueDiv)){
			SetValue2(CalcMethod, fMxPerImage); 
			return true;
		}else
			return false;
	}
}

// 196650
bool CStatParam_SurfAreaPerc::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if(!CalcMethod || m_Class.m_lClass.img != AllImages)
	{
		CStatParamBase& surfArea=(CStatParamBase&)Class(CalcForRelativeClasses,m_Class.m_lClass.img).Param(PARAM_PERIMETR);
		if( !surfArea.Calculate() )
				return false;
		double fAreaAll = surfArea.GetValue();

		if( _is_zero( fAreaAll ) )
			return false;

		CStatParamBase& SArea=(CStatParamBase&)m_Class.Param(PARAM_PERIMETR);
		if( !SArea.Calculate() )
				return false;
		double fArea = SArea.GetValue();

		SetValue2(CalcMethod, fArea / fAreaAll * 100);
		return true;
	}
	else
	{
		double fMxPerImage,fValueDiv=-1.;
		if(ImageXX(PARAM_SURFAREAPERC,CalcMethod,fMxPerImage,fValueDiv)){
			SetValue2(CalcMethod, fMxPerImage); 
			return true;
		}else
			return false;
	}
}

// 196651
bool CStatParam_SpecSurf::Calculate2(long CalcMethod)
{
 	if( IsCalculated2(CalcMethod) )
		return true;

	if(!CalcMethod || m_Class.m_lClass.img != AllImages)
	{
		CStatParamBase2& m_Sv = (CStatParamBase2&)m_Class.Param(PARAM_SURF_AREA);
		if( !m_Sv.Calculate2(0) )
			return false;
		double fSv = m_Sv.GetValue2(0);

		if( _is_zero( fSv ) )
			return false;

		CStatParamBase2& m_Vv = (CStatParamBase2&)m_Class.Param(PARAM_3DPART);
		if( !m_Vv.Calculate2(0) )
			return false;
		double fVv = m_Vv.GetValue2(0);

		SetValue2(CalcMethod, fSv / fVv); 
		return true;
	}
	else
	{
		double fMxPerImage,fValueDiv=-1.;
		if(ImageXX(PARAM_SPEC_SURF,CalcMethod,fMxPerImage,fValueDiv)){
			SetValue2(CalcMethod, fMxPerImage); 
			return true;
		}else
			return false;
	}
}

// 196652 Процент удельной поверхности
bool CStatParam_SpecSurfPerc::Calculate2(long CalcMethod)
{
	if( IsCalculated2(CalcMethod) )
		return true;

	if(!CalcMethod || m_Class.m_lClass.img != AllImages)
	{
		CStatParamBase2& SSurf2=(CStatParamBase2&)Class(CalcForRelativeClasses,m_Class.m_lClass.img).Param(PARAM_SPEC_SURF);
		if( !SSurf2.Calculate2(0) )
			return false;
		double fAreaAll=SSurf2.GetValue2(0);

		if( _is_zero( fAreaAll ) )
			return false;
		CStatParamBase2& SSurf=(CStatParamBase2&)m_Class.Param(PARAM_SPEC_SURF);
		if( !SSurf.Calculate2(0) )
				return false;
		double fArea = SSurf.GetValue2(0);

		SetValue2(CalcMethod, 100. * fArea / fAreaAll);
		return true;
	}
	else
	{
		double fMxPerImage,fValueDiv=-1.;
		if(ImageXX(PARAM_SPEC_SURF_PERC,CalcMethod,fMxPerImage,fValueDiv)){
			SetValue2(CalcMethod, fMxPerImage); 
			return true;
		}else
			return false;
	}
}

bool CStatParam_MXLog2::Calculate()
{
	if( IsCalculated() )
	return true;

	long N = m_Class.N();
	if( N < 1 )
		return false;

	double y=0.; 
	BEGIN_ROW()
			if(fVal<=0.)
			return false;
			double dlog=log10(fVal);
			y += dlog;
	END_ROW()
	double M = y/N;
	m_fValue=M;

	return true;
}

bool CStatParam_DispLog2::Calculate()
{
	if( IsCalculated() )
		return true;

	long N = CStatParamBase::N();
	if( N < 2 )
		return false;

	double y=0.; double y2=0.;
	BEGIN_ROW();
		if(fVal<=0.)
			return false;
		double dlog=log10(fVal);
		y += dlog;
		y2 += dlog*dlog;
	END_ROW();
	_ASSERT(N==lCount);

	double M = y/N;
	double S2 = (y2 - N*M*M)/(N-1);
	if(S2<0.)
		S2=0.;
	m_fValue=S2;
	return true;
}

bool CStatParam_MXLogNorm::Calculate()
{
	if( IsCalculated() )
		return true;

	long N = CStatParamBase::N();
	if( N < 2 )
		return false;

	CStatParamBase& dx=m_Class.Param(PARAM_DXLOG);
	if(!dx.Calculate())
		return false;
	CStatParamBase& mx=m_Class.Param(PARAM_MATHXLOG);
	if(!mx.Calculate())
		return false;
	double M = log((double)mx);
	double S2 = M_LN10*M_LN10*dx;
	
	m_fValue=exp(M + S2/2.);

	return true;
}

bool CStatParam_DispLogNorm::Calculate()
{
	if( IsCalculated() )
		return true;

	long N = CStatParamBase::N();
	if( N < 2 )
		return false;

	CStatParamBase& dx=m_Class.Param(PARAM_DXLOG);
	if(!dx.Calculate())
		return false;
	CStatParamBase& mx=m_Class.Param(PARAM_MATHXLOG);
	if(!mx.Calculate())
		return false;
	double M = log((double)mx);
	double S2 = M_LN10*M_LN10*dx;
	
	m_fValue=exp(2.*M + S2)*(exp(S2)-1.);

	return true;
}

bool CStatParam_ModeLogNorm::Calculate()
{
	if( IsCalculated() )
		return true;

	long N = CStatParamBase::N();
	if( N < 2 )
		return false;

	CStatParamBase& dx=m_Class.Param(PARAM_DXLOG);
	if(!dx.Calculate())
		return false;
	CStatParamBase& mx=m_Class.Param(PARAM_MATHXLOG);
	if(!mx.Calculate())
		return false;
	double M = log((double)mx);
	double S2 = M_LN10*M_LN10*dx;
	
	m_fValue=exp(M - S2);

	return true;
}

bool CStatParam_AssimetryLogNorm::Calculate()
{
	if( IsCalculated() )
		return true;

	long N = CStatParamBase::N();
	if( N < 2 )
		return false;

	CStatParamBase& dx=m_Class.Param(PARAM_DXLOG);
	if(!dx.Calculate())
		return false;
	CStatParamBase& mx=m_Class.Param(PARAM_MATHXLOG);
	if(!mx.Calculate())
		return false;
	double M = log((double)mx);
	double S2 = M_LN10*M_LN10*dx;
	
	double dExpS2=exp(S2);
	m_fValue=sqrt(dExpS2-1.)*(2.+dExpS2);

	return true;
}

bool CStatParam_ExcessLogNorm::Calculate()
{
	if( IsCalculated() )
		return true;

	long N = CStatParamBase::N();
	if( N < 2 )
		return false;

	CStatParamBase& dx=m_Class.Param(PARAM_DXLOG);
	if(!dx.Calculate())
		return false;
	CStatParamBase& mx=m_Class.Param(PARAM_MATHXLOG);
	if(!mx.Calculate())
		return false;
	double M = log((double)mx);
	double S2 = M_LN10*M_LN10*dx;
	
	double dExpS2=exp(S2);
	m_fValue=(((dExpS2 +2.)*dExpS2)+3.)*dExpS2*dExpS2 - 6.;

	return true;
}

bool CStatParam_WeightedAverage::Calculate()
{
	if( IsCalculated() )
		return true;

	if( AllClasses == m_Class.m_lClass.cls &&  AllImages==m_Class.m_lClass.img)
	{
		long N = CStatParamBase::N();

		if(N<=0)
			return false;

		long lClassCount=0;	
		m_pStat->GetClassCount( &lClassCount );
		m_fValue=0.;
		for(int it = 0; it < lClassCount; ++it)
		{
			long lClass = -1;
			m_pStat->GetClassValue( it, &lClass ); 
			if(lClass>=0){
				CClass& cls=Class(ObjectSpace::Class(lClass),AllImages);
				int i=lClass+1;
				m_fValue += cls.N() * i;
			}
		}
		m_fValue /= N;
		return true;
	}
	else
	{
		return false;
	}
}

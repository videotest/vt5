#include "StdAfx.h"


#include "Params.h"
#include "compositemeasgroup.h"
#include "PropBag.h"


#define LOWEST_PRIORITY -11

IMPLEMENT_DYNCREATE(CCompositeMeasGroup, CMeasParamGroupBase)

// {BED57AEA-E4BB-4f6b-ABC4-16BF42D557E8}
static const GUID clsidCompositeMeasGroup = 
{ 0xbed57aea, 0xe4bb, 0x4f6b, { 0xab, 0xc4, 0x16, 0xbf, 0x42, 0xd5, 0x57, 0xe8 } };


// {C6BA000B-E679-4682-8949-21B874811D06}
IMPLEMENT_GROUP_OLECREATE(CCompositeMeasGroup, "Composite.CompositeMeasGroup", 
						  0xc6ba000b, 0xe679, 0x4682, 0x89, 0x49, 0x21, 0xb8, 0x74, 0x81, 0x1d, 0x6);

 

CCompositeMeasGroup::CCompositeMeasGroup(void)
{
	_OleLockApp( this );
	m_sName = "Composite";
}

CCompositeMeasGroup::~CCompositeMeasGroup(void)
{

	_OleUnlockApp( this );

	clear();
}


void CCompositeMeasGroup::GetClassID( CLSID* pClassID )
{
	memcpy( pClassID, &guid, sizeof(CLSID) );
}
void CCompositeMeasGroup::add_value(CTreeNode* tn, CList<double>* values)
{

	POSITION pos;
	tn->GetFirstChildPosition(pos);
	while(pos)
	{
		CTreeNode* tn1;
		tn->GetNextChild(pos, &tn1);
		add_value(tn1, values);

	}
	ICalcObject2Ptr co(tn->GetData());

	int i=0;
	POSITION posParam = m_Params.GetHeadPosition();
	while(posParam)
	{
		PARAMETER* par = (PARAMETER*)m_Params.GetNext(posParam);
		if(get_object_class(tn->GetData())!= par->ClassNum) {i++; continue;}
		if(par->Operation == opCount) 
		{
			values[i].AddTail(0.0);
			i++;
			continue;
		}
		LPOS posValue;
		co->GetValuePos(par->BaseParamKey, &posValue);
		if(posValue)
		{
			double value;
			long key;
			co->GetNextValue(&posValue, &key, &value);
			ASSERT(key == par->BaseParamKey);
			values[i].AddTail(value);
		}
		i++;
	}
}

bool CCompositeMeasGroup::is_suitable_object(IUnknown* object)
{
	INamedPropBagPtr bag(object);
	variant_t flag;
	bag->GetProperty( bstr_t("MainObject"), &flag );
	if(flag.vt==VT_EMPTY) return false;
	return flag.lVal==1;
}
//void CCompositeMeasGroup::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
//{
//	 CMeasParamGroupBase::OnNotify(pszEvent, punkHit, punkFrom, pdata, cbSize );
//}
bool CCompositeMeasGroup::CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource )
{	
	if(!is_suitable_object(punkCalcObject)) return false;

	INamedDataObject2Ptr mainObject(punkCalcObject);
	IUnknown* unkParent;
	mainObject->GetParent(&unkParent);

	ICompositeObjectPtr compositeList(unkParent);
	if(unkParent) unkParent->Release();
	else return false;
	long bC;
	compositeList->IsComposite(&bC);
	if(!bC) return false;


	CTreeNotion* tn =0;
	CTreeNode* treeNode =0;
	if(compositeList)
	{
		compositeList->GetTreeNotion((void**) &tn);
		if(tn)
		{
			treeNode = tn->Find(punkCalcObject,0);

		}
	}
	

	int n = m_Params.GetCount();
	ICalcObject2Ptr co(punkCalcObject);
	CList<double>* values = new  CList<double>[n]; 

	POSITION pos;
	treeNode->GetFirstChildPosition(pos);
	while(pos)
	{
		CTreeNode* tnode1;
		treeNode->GetNextChild(pos, &tnode1);
		add_value(tnode1, values);
	}

	int i=0;
	double result;
	POSITION posParam = m_Params.GetHeadPosition();
	while(posParam)
	{
		PARAMETER* par = (PARAMETER*)m_Params.GetNext(posParam);
		switch(par->Operation)
		{
		case opSum ://суммирование 
			result = sum(values[i]);
			break;

		case opAverage://усреднение 
			result = leverage(values[i]);
			break;
		case opCount://кол-во объектов класса 
			result = values[i].GetCount();
			break;
		default:
			result =0;
		}

		co->SetValue(par->lKey, result);
		i++;
	}

	delete[] values; 
	return true;
}


bool CCompositeMeasGroup::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeffToUnits )
{
	POSITION pos = m_Params.GetHeadPosition();
	fCoeffToUnits =0;
	while(pos)
	{
		PARAMETER* par = (PARAMETER*)m_Params.GetNext(pos);
		if(par->lKey == lKey)
		{
			if(par->Operation==opCount)
			{
				CString s =	GetValueString(GetAppUnknown(), "Units\\Count", "Name", "-");
				bstrType= s.AllocSysString();
				fCoeffToUnits = 1.;
			}
			else
			{
				bstrType= SysAllocString( par->bstrUnit);
				fCoeffToUnits =par->fCoeffToUnits;
			}
			return true;
		}
	}

	return false;
}
bool CCompositeMeasGroup::LoadCreateParam()
{
	ReadNamedData(sMainMeasParamsSection);
	define_params();
	return true;
}
IUnknown*  CCompositeMeasGroup::GetInterfaceHook(const void* p)
{
	if (*(IID*)p == IID_ICompositeMeasureGroup)
		return (ICompositeMeasureGroup*)this;
	else return __super::GetInterfaceHook(p);
}

bool CCompositeMeasGroup::ReloadState()
{
	ReadNamedData(sMainMeasParamsSection);
	define_params();
	/*__super::ReloadState();*/
	return true;
}

bool CCompositeMeasGroup::OnInitCalculation()
{
	ReadNamedData(sMainMeasParamsSection);
	define_params();
	return true;
}
HRESULT CCompositeMeasGroup::GetPriority(long *plPriority)
{
	*plPriority = -9;
	return S_OK;
}
HRESULT CCompositeMeasGroup::UpdateClassName( struct ParameterContainer* pc)
{
	classfile = GetValueString(GetAppUnknown(), _T("Classes"),_T("CompositeObjectsClassFile"),0);
	INamedDataPtr nd(GetAppUnknown());

	int classNum =-1;
	int r,g,b;
	TCHAR buffer[128];
	TCHAR sz[5]; 
	//POSITION pos = m_Params.GetHeadPosition();
	//PARAMETER* par =0;
	//while(pos)
	//{
	//	par= (PARAMETER*)m_Params.GetNext(pos);
	//	if(par->lKey == pc->pDescr->lKey) break;

	//}
	bstr_t Key("Key");
	CString s;
	bstr_t bstrSection(sMainMeasParamsSection);
	CString entry;
	if(nd)
	{
		nd->SetupSection(bstrSection);
		long count;
		nd->GetEntriesCount( &count );
		for(long i=0; i<count; i++)
		{
			variant_t var;
			CString s = ::GetEntryName( ::GetAppUnknown(), sMainMeasParamsSection, i );
			entry = sMainMeasParamsSection;
			entry += _T("\\") + s;
			bstr_t bstrEName( entry);
			nd->SetupSection(bstrEName);
			nd->GetValue(Key, &var);
			long key = var.lVal;
			var.Clear();

			if(key==pc->pDescr->lKey)
			{
				bstr_t parName("ClassNum");
				nd->GetValue(parName, &var);
				classNum = var.iVal;
				var.Clear();
				break;
			}
		}
	}
	if(classNum ==-1) return E_FAIL;

	_itot(classNum,sz,10);
	::GetPrivateProfileString( "Classes", sz, "", buffer, sizeof( TCHAR )*128, classfile );
	sscanf( buffer, "(%d,%d,%d),%[^;]s", &r, &g, &b, buffer );
	CString sTemp(pc->pDescr->bstrUserName);
	s.Format(sTemp,buffer);
	SysFreeString(pc->pDescr->bstrUserName);
	pc->pDescr->bstrUserName = s.AllocSysString();
	return S_OK;
}



void CCompositeMeasGroup::ReadNamedData(LPCTSTR section = sMainMeasParamsSection)
{
	clear();
	INamedDataPtr nd(GetAppUnknown());

	classfile = GetValueString(GetAppUnknown(), _T("Classes"),_T("CompositeObjectsClassFile"),0);
	if(classfile=="")
	{
		classfile = GetValueString(GetAppUnknown(), _T("Classes"),_T("ClassFile"),0);		
	}
	bstr_t Key("Key");
	CString s = section;
	bstr_t bstrSection(section);
	CString entry;
	if(nd)
	{
		nd->SetupSection(bstrSection);
		long count;
		nd->GetEntriesCount( &count );
		for(long i=0; i<count; i++)
		{
			variant_t var;
			CString s = ::GetEntryName( ::GetAppUnknown(), section, i );
			entry = section;
			entry += _T("\\") + s;
			bstr_t bstrEName( entry);
			nd->SetupSection(bstrEName);
			nd->GetValue(Key, &var);
			long key = var.lVal;
			var.Clear();

			if(key<=KEY_END_COMPOSITE && key>=KEY_BASE_COMPOSITE)
			{


				bstr_t parName("Enable");
				nd->GetValue(parName, &var);
				if(var.iVal == 0) continue;

				PARAMETER* par =new PARAMETER;
				ZeroMemory(par, sizeof(PARAMETER));
				par->cbSize = sizeof(PARAMETER);
				par->lEnabled = var.iVal;
				var.Clear();

				par->lKey = key;

				parName = "UserName";
				nd->GetValue(parName, &var);
				par->bstrUserName =SysAllocString(var.bstrVal);
				SysFreeString(var.bstrVal);
				var.Clear();

				parName="ClassNum";
				nd->GetValue(parName, &var);
				par->ClassNum = var.iVal;
				var.Clear();

				//////{filling templates %s in the UserName string
				{
					int r,g,b;
					TCHAR buffer[128];
					TCHAR sz[5]; 
					_itot(par->ClassNum,sz,10);
					::GetPrivateProfileString( "Classes", sz, "", buffer, sizeof( TCHAR )*128, classfile );
					sscanf( buffer, "(%d,%d,%d),%[^;]s", &r, &g, &b, buffer );
					CString sTemp(par->bstrUserName);
					CString s;
					s.Format(sTemp,buffer);
					SysFreeString(par->bstrUserName);
					par->bstrUserName = s.AllocSysString();
				}
				//////}

				parName="BaseParamKey";
				nd->GetValue(parName, &var);
				par->BaseParamKey = var.intVal;
				var.Clear();

				parName="Operation";
				nd->GetValue(parName, &var);
				par->Operation = var.lVal;
				var.Clear();

				parName="Format";
				nd->GetValue(parName, &var);
				par->bstrDefFormat = SysAllocString(var.bstrVal);
				SysFreeString(var.bstrVal);
				var.Clear();

				par->bstrName = s.AllocSysString();
				if(par->Operation == opCount)
				{
					CString s =	GetValueString(GetAppUnknown(), "Units\\Count", "Name", "-");
					par->bstrUnit = s.AllocSysString();
					par->fCoeffToUnits = 1.;
					par->dwPanesMask =0;
					for(int i=0;i<4;i++)
						par->adwPhasesMask[i] = 0;
					par->pos =(long)m_Params.AddTail( par );
					continue;
				}
				ParameterDescriptor* pd = find_parameter_desc(par->BaseParamKey);
				if(pd)
				{
					par->bstrUnit = SysAllocString(pd->bstrUnit);
					par->fCoeffToUnits = pd->fCoeffToUnits;
					par->dwPanesMask = pd->dwPanesMask;
					for(int i=0;i<4;i++)
						par->adwPhasesMask[i] = pd->adwPhasesMask[i];
					par->pos =(long)m_Params.AddTail( par );
				}
				else
				{
					SysFreeString(par->bstrDefFormat);
					SysFreeString(par->bstrUserName);
					delete par;
				}
			}
		}		
	}

}

void CCompositeMeasGroup::clear()
{
	POSITION pos = m_Params.GetHeadPosition();
	while(pos)
	{
		PARAMETER* par = (PARAMETER*)m_Params.GetNext(pos);
		SysFreeString(par->bstrDefFormat);
		SysFreeString(par->bstrName);

		SysFreeString(par->bstrUnit);
		SysFreeString(par->bstrUserName);
		delete par;
	}
	m_Params.RemoveAll();

}
ParameterDescriptor* CCompositeMeasGroup::find_parameter_desc(long key)
{
	IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
	ICompManagerPtr ptrManager = punk ;
	if( punk )punk->Release();
	if(ptrManager ==0) return 0;
	int	nGroup, nGroupCount;
	ICompositeMeasureGroupPtr cmg;
	ptrManager->GetCount( &nGroupCount );
	for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
	{
		IUnknownPtr	ptrG = 0;
		ptrManager->GetComponentUnknownByIdx( nGroup, &ptrG );
		IMeasParamGroupPtr	ptrGroup( ptrG );

		if( ptrGroup == 0 )continue;
		LONG_PTR pos;

		ptrGroup->GetPosByKey(key,&pos);
		if(pos)
		{
			ParameterDescriptor* ppd;
			ptrGroup->GetNextParam(&pos,&ppd);
			return ppd;
		}
	}
	return 0;

}

double CCompositeMeasGroup::sum(CList<double>& values)
{
	double result =0;
	POSITION pos = values.GetHeadPosition();
	while(pos)
	{
		result+=values.GetNext(pos);
	}
	return result;
}
double CCompositeMeasGroup::leverage(CList<double>& values)
{
	if(values.GetCount()==0) return 0;
	return sum(values)/values.GetCount();

}
int CCompositeMeasGroup::get_object_class(IUnknown* object)
{
	if( !object )return -1;
	INamedPropBagPtr bag(object);
	if(bag!=0)
	{
		_variant_t	var;
		int pos = classfile.ReverseFind('\\')+1;
		TCHAR* buffer = classfile.GetBuffer();
		buffer+=pos;
		bag->GetProperty(bstr_t(buffer), &var);
		if(var.vt == VT_I4) return var.lVal;
	}
	return -1;
}

void CCompositeMeasGroup::define_params()
{

	undefine_parameters();
	POSITION pos = m_Params.GetHeadPosition();
	while(pos)
	{
		PARAMETER* par = (PARAMETER*)m_Params.GetNext(pos);
		LONG_PTR p,pp;
		IMeasParamGroupPtr group(this); 
		group->GetPosByKey ( par->lKey, &p );
		pp=p;
		/*if(!m_mapKeyToPosition.Lookup(par->lKey, p)) p=0;*/
		ParameterDescriptor	*pdescr;
		if(p==0)
		{
			pdescr = new ParameterDescriptor();
		}
		else
		{
			/*pdescr = (ParameterDescriptor*)*/group->GetNextParam(&p, &pdescr );
			if(pdescr->bstrName) SysFreeString(pdescr->bstrName);	
			if(pdescr->bstrUserName) SysFreeString(pdescr->bstrUserName);	
			if(pdescr->bstrDefFormat) SysFreeString(pdescr->bstrDefFormat);
			if(pdescr->bstrUnit)SysFreeString(pdescr->bstrUnit);
			p = pdescr->pos;
		}

		
		::ZeroMemory( pdescr, sizeof(ParameterDescriptor) );
		pdescr->pos =p;
		pdescr->cbSize = sizeof(ParameterDescriptor);
		pdescr->bstrName = SysAllocString(par->bstrName);
		pdescr->bstrUserName = SysAllocString(par->bstrUserName);
		pdescr->bstrDefFormat = SysAllocString(par->bstrDefFormat);
		pdescr->lKey = par->lKey;
		pdescr->lEnabled = par->lEnabled;
		pdescr->fCoeffToUnits = par->fCoeffToUnits;
		pdescr->bstrUnit = SysAllocString(par->bstrUnit);

		if(pp==0)
		{
			pdescr->pos = (long)m_listParamDescr.AddTail( pdescr );
			m_mapKeyToPosition[par->lKey] = (POSITION)pdescr->pos;
			
		}

		CString	sMainSection = "\\measurement\\parameters\\";
		CString sParamsSection = sMainSection+CString(par->bstrName);
		::SetValue( ::GetAppUnknown(), sParamsSection, "Key", par->lKey ); 
	}

}
void CCompositeMeasGroup::undefine_parameters(void)
{
	/*m_mapKeyToPosition.RemoveAll( );*/
	POSITION pos = m_listParamDescr.GetHeadPosition();
	while(pos)
	{
		ParameterDescriptor* pdescr = (ParameterDescriptor*) m_listParamDescr.GetNext(pos);
		pdescr->lEnabled =0;

		/*if(pdescr->bstrName) SysFreeString(pdescr->bstrName);	
		if(pdescr->bstrUserName) SysFreeString(pdescr->bstrUserName);	
		if(pdescr->bstrDefFormat) SysFreeString(pdescr->bstrDefFormat);
		if(pdescr->bstrUnit)SysFreeString(pdescr->bstrUnit);

		delete pdescr;*/

    }
	/*m_listParamDescr.RemoveAll();*/

}

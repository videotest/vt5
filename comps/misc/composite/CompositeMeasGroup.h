#pragma once
#include "ClassBase.h"
#include <list>
#include "objectlist.h"
using namespace std;


struct PARAMETER: public ParameterDescriptor	
{
	short	ClassNum;
	short	Operation;
	int		BaseParamKey;
} ; 

enum 
{
	opSum = 0,
	opAverage =1, 
	opCount = 2
} Operation;

class CCompositeMeasGroup :
	public CMeasParamGroupBase,
	public ICompositeMeasureGroup
{

	DECLARE_DYNCREATE(CCompositeMeasGroup)
	DECLARE_GROUP_OLECREATE(CCompositeMeasGroup);


	ENABLE_MULTYINTERFACE();
public:
	CCompositeMeasGroup(void);
	virtual ~CCompositeMeasGroup(void);
	virtual IUnknown* GetInterfaceHook(const void*);
	virtual bool CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource );
	virtual bool GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeffToUnits );
	virtual bool LoadCreateParam();
	void GetClassID( CLSID* pClassID );
	virtual bool ReloadState();
	virtual bool OnInitCalculation();
	void ReadNamedData(LPCTSTR section);
	com_call GetPriority(long *plPriority);	
	com_call UpdateClassName( struct ParameterContainer* pc); 
	//virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
private:
	void publicate_parameters();
	ParameterDescriptor* find_parameter_desc(long key);
	void clear();
	//int  m_nClass;
	//long m_lKey;
	CString classfile;
	void add_value(CTreeNode* tn, CList<double>* values);
	int get_object_class(IUnknown* object);
	bool is_suitable_object(IUnknown* object);
	//операции
	double sum(CList<double>& values);
	double leverage(CList<double>& values);
	//

	CPtrList m_Params;
	void define_params();


protected:
	void undefine_parameters(void);
};

#pragma once
#include "statui_int.h"
#include "resource.h"
#include "misc_utils.h"
#include "measure5.h"
#include <math.h>

#include "\vt5\awin\misc_list.h"
#include "\vt5\comps\misc\Calculator\Calculator.h"
#include "\vt5\awin\misc_map.h"

namespace ObjectSpace
{
	class CStatisticObject;

	namespace ParamSpace{
		class CStatParamBase;
		struct XObjectInfo;

		class _OBJECT_LIST:public vector<XObjectInfo*>{
		public:
			~_OBJECT_LIST();
			void add_tail(value_type pObjInfo){push_back(pObjInfo);}
		};
	}

	class PARAMS_LIST:public map<long,ParamSpace::CStatParamBase*>{
	public:
		virtual ~PARAMS_LIST(){};
		void clear();
		bool add_tail(ParamSpace::CStatParamBase* pParam);
	};

	enum Class {
		CalcForGroups=-10,						// для каких изображений расчитывать
		AllClasses=-1,
		CalcForAllClasses	= -2,				// для каких классов расчитывать ВСЕГО
		CalcForRelativeClasses = -4, 	// для каких классов расчитывать относительные параметры
		MaxClassId = 9999
	};
	enum NumImg {
		AllImages=-1,
		MaxImageId=9999
	};
	inline NumImg operator++( NumImg &rs, int )
	{
		return rs = (NumImg)(rs + 1);
	}

	struct ClassID {
		Class cls;
		NumImg img;
		ClassID(Class cls, NumImg img):cls(cls),img(img){}
	};
	struct lessClassID {
		bool operator()(const ClassID& classId1,const ClassID& classId2) const
		{
			return classId1.cls < classId2.cls 
				|| classId1.cls == classId2.cls
				&& classId1.img < classId2.img;
		}
	};
	
	class CClass:public PARAMS_LIST
	{
	public:
		CClass(CStatisticObject& stat, ClassID lClass, const PARAMS_LIST& paramsList=PARAMS_LIST())
			:stat(stat), m_lClass(lClass), m_pObjList(NULL), PARAMS_LIST(paramsList){}
		~CClass(){delete m_pObjList;}
		ClassID m_lClass;
//		operator const long*(){return &m_lClass;}
		const long N(){return m_pObjList->size();}
		ParamSpace::_OBJECT_LIST* m_pObjList;
		void ChooseObjects();
	private:
		template< class _Pr> inline	
			void FindIfObjects(const ParamSpace::_OBJECT_LIST&, _Pr _Pred);
	public:

	// find matching CClass in ClassMap, if not create it
		static CClass& TrieveClass(CStatisticObject& stat, ClassID ClassId);

		CClass& TrieveSub(const NumImg image);

		ParamSpace::CStatParamBase& Param(const long paramKey);

		template<class ParamType> ParamType& Par(const long paramKey)
		{
			return (ParamType&)Param(paramKey);
		}
		CStatisticObject& stat;
	};

	typedef CClass* ClassPtr;

	class classless{
	public:
		bool operator()(const ClassPtr& class1,const ClassPtr& class2) const
		{
			return class1->m_lClass.cls < class2->m_lClass.cls 
				|| class1->m_lClass.cls == class2->m_lClass.cls
				&& class1->m_lClass.img < class2->m_lClass.img;
		}
	};

	typedef set<ClassPtr,classless> ListClass;


namespace ParamSpace
{
	struct XObjectInfo
	{
		NumImg iImage;
		double fValue;
		long lClass;

		double fValueArea;
		double fValueLength;
		double fValuePerim;
		
		XObjectInfo()
		{
			iImage = (NumImg)0;
			fValue = 0;
			lClass = 0;

			fValueArea = 0;
			fValueLength = 0;
			fValuePerim = 0;
		}

		XObjectInfo( NumImg iImage, double fVal, long lCla, double fArea, double fLength, double fPerim ) : 
			iImage(iImage),fValue(fVal),lClass(lCla), fValueArea(fArea)
			,fValueLength(fLength), fValuePerim(fPerim)
		{
		}
//		XObjectInfo& operator *(){return this;}
	};
//	inline double operator +(double a, std::auto_ptr<XObjectInfo>pObj){return a + pObj->fValue;}

	inline long cmp_guid( GUID g, GUID g2 )
	{	return memcmp( &g, &g2, sizeof( GUID) ); }

	void _delete_info( XObjectInfo *pObject );
	
	void _delete_list_info( _OBJECT_LIST *pObject );
	//typedef _list_t<_OBJECT_LIST *, _delete_list_info > _OBJECT_LISTs;
			


struct CStatParamDscrBase{
	long key;
	CStatParamDscrBase(long key):key(key){}
	virtual ~CStatParamDscrBase(){}
	virtual void LoadSettings(INamedDataPtr sptrND){}
	virtual void StoreSettings(INamedDataPtr sptrND){}
};

struct CStatParamDscr:CStatParamDscrBase
{
//	CStatParamDscr(void):m_lVisibility(0){};
	CStatParamDscr(long key,	CStringW& wsName, long lVisibility)
		:CStatParamDscrBase(key)
		,m_szName(wsName), m_lVisibility(lVisibility),m_lCalcFrom(0), m_ColWidth(100)
	{};
	virtual ~CStatParamDscr(){}
	long m_lVisibility;
	_bstr_t m_szName; // Название
	_bstr_t m_szFormat; // Определение формата отображения. Пример: %0.3f
	_bstr_t m_szDesignation; // Символьное обозначение
	_bstr_t m_szModuleOfDesign; // Единицы измерений
//	long m_lShowOnGraph; // Возможность показывать на графике.
	long m_lLegendOrder, m_lTableOrder;

	long m_lCalcFrom; // Способ расчета. Значение: [0(по объектам);1(по изображениям)].

	virtual void SetCalcFrom( long lStat ) { m_lCalcFrom = lStat; }
	void LoadSettings( INamedDataPtr sptrND );
	void StoreSettings( INamedDataPtr sptrND );
private:
	unsigned long m_ColWidth;
};

struct CStatUserParamDscr:public CStatParamDscr
{
	double m_fProb;
	CStatUserParamDscr(long key,	CStringW& wsName=CStringW(), long lVisibility=0)
		:CStatParamDscr(key,wsName,lVisibility),m_fProb(0.95){}
	virtual ~CStatUserParamDscr(){}
	_bstr_t m_szExpression; // Расчетная формула
	long m_lCalcMethod; // По какому классу считать
	void LoadSettings( INamedDataPtr sptrND );
	void StoreSettings( INamedDataPtr sptrND );
};

struct CStatErrorDscr:public CStatParamDscr
{
	double m_fProb;
	CStatErrorDscr(long key,	CStringW& wsName, long lVisibility)
		:CStatParamDscr(key,wsName,lVisibility),m_fProb(0.95){}
	virtual ~CStatErrorDscr(){}
	void LoadSettings( INamedDataPtr sptrND ){
		__super::LoadSettings(sptrND);
		m_fProb = ::GetValueDouble( sptrND, SECT_STATUI_STAT_PARAM_ROOT, MX_PROPABILITY, 0.95 );
	}
};

struct CStatNormalParamDscr:public CStatParamDscr
{
	double m_fNormal;
	CStatNormalParamDscr(long key,	CStringW& wsName, long lVisibility)
		:m_fNormal(1.)
		,CStatParamDscr(key,wsName,lVisibility){}
	virtual ~CStatNormalParamDscr(){}
	void LoadSettings( INamedDataPtr sptrND ){
		__super::LoadSettings(sptrND);
		m_fNormal = ::GetValueDouble( sptrND, SECT_STATUI_STAT_PARAM_ROOT, NORMAL_AREA, 1 );
		if( !m_fNormal )
			m_fNormal = 1;
	}
};

struct ParDscr:map<long,CStatParamDscrBase*>{
	ParDscr();
	~ParDscr(){
		for(const_iterator it=begin(); it!=end(); ++it) delete it->second;
	}
	CStatParamDscrBase*& operator[](long parKey){
		_ASSERTE(parKey);
		std::pair<iterator,bool> ib=insert(value_type(parKey,(CStatParamDscrBase*)NULL));
		return ib.first->second;
	}
	bool add(CStatParamDscrBase* pParDscr){
		_ASSERTE(pParDscr->key);
		_ASSERTE(pParDscr);
		std::pair<iterator,bool> ib=insert(value_type(pParDscr->key,pParDscr));
		return ib.second;
	}
};

class CStatParamBase
{
protected:
	double m_fValue; // Значение параметра
	CStatParamDscr*& GetParDscr(long key);
public:
	friend CStatisticObject;
	CStatisticObject* m_pStat;
	CClass& m_Class;
	const long N(){return m_Class.m_pObjList->size();}
	CStatParamBase(CClass& cls);
	virtual ~CStatParamBase() {}
public:
	virtual bool Calculate()
	{ 	return false;	}

	bool is_any_objects();

	virtual long GetKey() { return 0; }
	virtual double GetValue() { return m_fValue; }
	virtual void SetValue(double Val){
		m_fValue = Val;
	}
	operator double(){return GetValue();}
	template<class ParType> operator ParType&(){return *this;}
	virtual double GetValueEx(int nParam = -1) { return -1; }
	virtual long GetSerializeSize();

	virtual void Store( IStream *pStream, SerializeParams *pparams );
	virtual void Load( IStream *pStream, SerializeParams *pparams );

	virtual bool IsCalculated() { return !_isnan(m_fValue); }
	virtual bool IsAvailable() { return true; }
public:

	CClass& Class(const Class cls, const NumImg image/*=AllImages*/);
	CClass& SubClass(const NumImg image)
	{
		return m_Class.TrieveSub(image);
	}
	union{
		CStatParamDscr* pParDscr;
		CStatErrorDscr* pParErrDscr;
		CStatNormalParamDscr* pNormalParDscr;
		CStatUserParamDscr*	pUserParDscr;
	};
};

class CStatParamBase2:public CStatParamBase
{
protected:
	double m_fAlternateVal;
	double m_fValueDiv;
public:
	long m_ImgCnt;
	CStatParamBase2(CClass& cls)
		:m_fAlternateVal(std::numeric_limits<double>::quiet_NaN()) 
		,CStatParamBase(cls),m_fValueDiv(-1.){}
	virtual bool Calculate2(long CalcMethod)=0;
	virtual bool IsCalculated2(long CalcMethod) {
		bool bAlternateVal=(CalcMethod!=pParDscr->m_lCalcFrom 
			&& AllImages==m_Class.m_lClass.img);
		double& target = bAlternateVal ? m_fAlternateVal : m_fValue;
		return !_isnan(target); 
	}
	virtual double GetValue2(long CalcMethod){
		bool bAlternateVal=(CalcMethod!=pParDscr->m_lCalcFrom 
			&& AllImages==m_Class.m_lClass.img);
		return bAlternateVal ? m_fAlternateVal : m_fValue;
	}
	virtual void SetValue2(long CalcMethod, double Val){
		bool bAlternateVal=(CalcMethod!=pParDscr->m_lCalcFrom 
			&& AllImages==m_Class.m_lClass.img);
		double& target = bAlternateVal ? m_fAlternateVal : m_fValue;
		target = Val;
	}
	bool Calculate(){
		return Calculate2(pParDscr->m_lCalcFrom);
	}
	double GetValueEx()
	{
		return m_fValueDiv; 
	}
protected:
	bool ImageXX(long parKey, long calcMethod, double& Mx, double& Dx, long& nImg=*(long*)NULL);
};

class CStatParamUserDefined : public CStatParamBase // Пользовательский
{
	long m_lKey;
	int m_Computing;
//	IUnknownPtr m_sptrCalculator;
public:
	CStatParamUserDefined(CClass& cls, INamedDataPtr sptrND, long lKey );
	virtual ~CStatParamUserDefined()
	{
	}

	virtual long GetKey() { return m_lKey; }
	bool Calculate();
	virtual bool IsAvailable();
};

class CStatParam_Count : public CStatParamBase // кол-во.
{
	double m_fValueDiv;
public:
	CStatParam_Count(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey());
	}

	virtual double GetValueEx(int nParam = -1) { return m_fValueDiv; }

	bool Calculate();
	virtual long GetKey() { return PARAM_COUNT; }
};

class CStatParam_Area : public CStatParamBase // площ.
{
public:
	CStatParam_Area(CClass& cls, INamedDataPtr sptrND=NULL )
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_AREA; }
};

class CStatParam_Length : public CStatParamBase // длина.
{
public:
	CStatParam_Length(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_LENGTH; }
};

class CStatParam_UsedArea : public CStatParamBase // обработанная площадь
{
public:
	CStatParam_UsedArea(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}
	bool Calculate();
	virtual long GetKey() { return PARAM_USEDAREA; }
};

class CStatParam_FullArea : public CStatParamBase // общая площадь
{
public:
	CStatParam_FullArea(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}
	bool Calculate();
	virtual long GetKey() { return PARAM_FULL_AREA; }
};

class CStatParam_ImageCount : public CStatParamBase // колво изображений
{
public:
	CStatParam_ImageCount(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	virtual bool IsAvailable() 
	{ 
		if( m_Class.m_lClass.cls==AllClasses ) 
			return true; 
		return false; 
	}
	bool Calculate();
	virtual long GetKey() { return PARAM_IMAGE_COUNT; }
};

class CStatParam_ChiSquare : public CStatParamBase // хи квадрат
{
public:
	CStatParam_ChiSquare(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	virtual bool IsAvailable() { if( m_Class.m_lClass.cls==AllClasses ) return true; return false; }
	bool Calculate();
	virtual long GetKey() { return PARAM_CHI_SQUARE; }
};

class CStatParam_ChiSquareLog : public CStatParam_ChiSquare // хи квадрат log
{
public:
	CStatParam_ChiSquareLog(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParam_ChiSquare(cls, sptrND)
	{ 
	}

	virtual bool IsAvailable() { if( m_Class.m_lClass.cls==AllClasses ) return true; return false; }
	virtual long GetKey() { return PARAM_CHI_SQUARE_LOG; }
};

class CStatParam_Chi2Critic : public CStatParamBase // хи квадрат критический
{
public:
	CStatParam_Chi2Critic(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	virtual bool IsAvailable() 
	{
		if( m_Class.m_lClass.cls==AllClasses ) 
			return true; 
		return false;
	}
	bool Calculate();
	virtual long GetKey() { return PARAM_CHI2_CRITIC; }
};

class CStatParam_Chi2Prob : public CStatParamBase
{
public:
	CStatParam_Chi2Prob(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	virtual bool IsAvailable() { if( m_Class.m_lClass.cls==AllClasses ) return true; return false; }
	bool Calculate();
	virtual long GetKey() { return PARAM_CHI2_PROB; }
};

class CStatParam_Chi2ProbLog : public CStatParamBase
{
public:
	CStatParam_Chi2ProbLog(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	virtual bool IsAvailable() { if( m_Class.m_lClass.cls==AllClasses ) return true; return false; }
	bool Calculate();
	virtual long GetKey() { return PARAM_CHI2_PROB_LOG; }
};

// Удельная площадь
class CStatParam_SpecificArea : public CStatParamBase2
{
public:
	CStatParam_SpecificArea( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)	
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}
	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_SPECAREA; }
};

class CStatParam_SpecificAreaError : public CStatParamBase // Ошибка удельной площади
{
public:
	CStatParam_SpecificAreaError(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_SPECAREA_ERR; }
};

class CStatParam_AreaPerc : public CStatParamBase2 // % площади
{
public:
	CStatParam_AreaPerc(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}
	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_AREAPERC; }
};

class CStatParam_AreaPercError : public CStatParamBase // ошибка % площади
{
public:
	CStatParam_AreaPercError(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}
	virtual void SetCalcFrom( long lStat ) 
	{
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_AREAPERC_ERR; }
};

class CStatParam_NormalArea : public CStatParamBase2 // Норм площади
{
public:
	CStatParam_NormalArea(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_NORMAL_AREA; }
};

// 196632 Удельное кол-во
class CStatParam_SpecificCount : public CStatParamBase2 
{
public:
	CStatParam_SpecificCount(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}
	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_SPECCOUNT; }
};

class CStatParam_SpecificCountError : public CStatParamBase // Ошибка норм площади
{
public:
	CStatParam_SpecificCountError(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_SPECCOUNT_ERR; }
};

class CStatParam_CountPerc : public CStatParamBase2 // % площади
{
public:
	CStatParam_CountPerc(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_COUNTPERC; }
};

class CStatParam_CountPercError : public CStatParamBase // ошибка % площади
{
public:
	CStatParam_CountPercError(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_COUNTPERC_ERR; }
};

class CStatParam_NormalCount : public CStatParamBase2 // Норм кол-во
{
public:
	CStatParam_NormalCount(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_NORMAL_COUNT; }
};

class CStatParam_SpecificLength : public CStatParamBase2 // Удельная длина
{
public:
	CStatParam_SpecificLength( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}
	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_SPECLENGTH; }
};

class CStatParam_SpecificLengthError : public CStatParamBase // Ошибка удельной площади
{
public:
	CStatParam_SpecificLengthError( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_SPECLENGTH_ERR; }
};

class CStatParam_3DPart : public CStatParamBase2 // Ошибка удельной площади
{
public:
	CStatParam_3DPart( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_3DPART; }
};

// 196610 среднее арифметич.
class CStatParam_MX : public CStatParamBase2 
{
	double m_fValueDiv;
public:
	CStatParam_MX( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}
	bool Calculate2(long lCalcFrom);
	virtual long GetKey() { return PARAM_MX; }
};

class CStatParam_MXLog2 : public CStatParamBase // log среднее арифметич.
{
public:
	CStatParam_MXLog2( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MATHXLOG; }
	double GetValue() { return pow( 10., CStatParamBase::GetValue() ); }
};

class CStatParam_DispLog2 : public CStatParamBase // log sigma.
{
public:
	CStatParam_DispLog2( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_DXLOG; }
};

// 196653
class CStatParam_StdDevLog : public CStatParamBase // log std dev.
{
public:
	CStatParam_StdDevLog( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_STD_DEV_LOG; }
//	double GetValue() { return pow( 10, CStatParamBase::GetValue() ); }
};

class CStatParam_MXGeom : public CStatParamBase // среднее геометрич.
{
public:
	CStatParam_MXGeom( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MX_GEOM; }
};

class CStatParam_MXGorm : public CStatParamBase // среднее гармоничное
{
public:
	CStatParam_MXGorm( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MX_GORM; }
};

class CStatParam_Min : public CStatParamBase // минимум
{
public:
	CStatParam_Min( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MIN; }
};

class CStatParam_Max : public CStatParamBase // максимум
{
public:
	CStatParam_Max( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MAX; }
};

class CStatParam_Var : public CStatParamBase // вариация
{
public:
	CStatParam_Var( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_VAR; }
};

class CStatParam_Disp : public CStatParamBase2 // дисперсия
{
public:
	CStatParam_Disp( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}
	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_DISP; }
};

class CStatParam_MXError: public CStatParamBase2 // ошибка среднего арифметич.
{
public:
	CStatParam_MXError( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_MX_ERR; }
};

class CStatParam_MXErrorLog: public CStatParamBase // log ошибка среднего арифметич.
{
public:
	CStatParam_MXErrorLog( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MX_ERR_LOG; }
	// double GetValue() { return pow( 10., CStatParamBase::GetValue() ); }
};

// 196641 доверительный среднего арифметич.
class CStatParam_MXProb: public CStatParamBase2
{
public:
	CStatParam_MXProb( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_MX_PROB; }
};

class CStatParam_MXProbLog: public CStatParamBase // доверительный интервал среднего арифметич.
{
	double m_fProb;
public:
	CStatParam_MXProbLog( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MX_PROB_LOG; }
//	double GetValue() { return pow( 10, CStatParamBase::GetValue() ); }
};

// 196617 СКО среднеквадр. откл
class CStatParam_StdDev : public CStatParamBase2
{
public:
	CStatParam_StdDev( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_STDDEV; }
};


class CStatParam_Mode : public CStatParamBase // Мода
{
public:
	CStatParam_Mode( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MODE; }
};

class CStatParam_ModeLog : public CStatParamBase // Мода log 
{
public:
	CStatParam_ModeLog( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MODE_LOG; }
	double GetValue() { return pow( 10., CStatParamBase::GetValue() ); }
};


class CStatParam_Mediane : public CStatParamBase // медиана
{
public:
	CStatParam_Mediane( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MEDIANE; }
};

class CStatParam_Moment : public CStatParamBase // момент
{
	long m_lMoment;
public:
	CStatParam_Moment( CClass& cls, INamedDataPtr sptrND, int moment)
		:CStatParamBase(cls)
	{ 
//		pParDscr=GetParDscr(GetKey()); 
		m_lMoment = moment; 
	}

	bool Calculate();
};

class CStatParam_MomentLog : public CStatParamBase // момент
{
	long m_lMoment;
public:
	CStatParam_MomentLog( CClass& cls, INamedDataPtr sptrND, int moment)
		:CStatParamBase(cls)
	{ 
//		pParDscr=GetParDscr(GetKey()); 
		m_lMoment = moment; 
	}
	bool Calculate();
};

class CStatParam_Assimetry : public CStatParamBase // ассиметрия
{
	CStatParam_Moment m_Moment;
public:
	CStatParam_Assimetry( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
		, m_Moment(cls,sptrND,3)
	{ 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_ASSIMETRY; }
};

class CStatParam_AssimetryLog : public CStatParamBase // ассиметрия log
{
	CStatParam_MomentLog m_Moment;
public:
	CStatParam_AssimetryLog( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
		,  m_Moment(cls,sptrND, 3 )
	{ 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_ASSIMETRY_LOG; }
};


class CStatParam_Excess : public CStatParamBase // эксцесс
{
	CStatParam_Moment m_Moment;
public:
	CStatParam_Excess( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	,  m_Moment(cls,sptrND, 4 )
	{ 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_EXCESS; }
};

class CStatParam_ExcessLog : public CStatParamBase // эксцесс log
{
	CStatParam_MomentLog m_Moment;
public:
	CStatParam_ExcessLog( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
		, m_Moment( cls,sptrND, 4 ){}
	bool Calculate();
	virtual long GetKey() { return PARAM_EXCESS_LOG; }
};

class CStatParam_VarCoeff : public CStatParamBase2 // Коэф. вар.
{
public:
	CStatParam_VarCoeff( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_COEF_VAR; }
};

class CStatParam_VarCoeffLog : public CStatParamBase // Коэф. вар.   log
{
public:
	CStatParam_VarCoeffLog( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_COEF_VAR_LOG; }
};

class CStatParam_Perimeter : public CStatParamBase // периметр
{
public:
	CStatParam_Perimeter( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_PERIMETR; }
};

class CStatParam_SurfArea : public CStatParamBase2 // площадь поверхн.
{
public:
	CStatParam_SurfArea( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_SURF_AREA; }

};

class CStatParam_MdlChord : public CStatParamBase2
{
public:
	CStatParam_MdlChord( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_MDL_CHORD; }

};

class CStatParam_MdlDist : public CStatParamBase2 // площадь поверхн.
{
public:
	CStatParam_MdlDist( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_MDL_DIST; }

};

class CStatParam_SurfAreaPerc : public CStatParamBase2 // % площади поверхности
{
public:
	CStatParam_SurfAreaPerc( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_SURFAREAPERC; }
};

class CStatParam_SpecSurf : public CStatParamBase2 // площадь поверхн.
{
public:
	CStatParam_SpecSurf( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_SPEC_SURF; }

};

class CStatParam_SpecSurfPerc : public CStatParamBase2 // % площади поверхности
{
public:
	CStatParam_SpecSurfPerc( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase2(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}
	bool Calculate2(long CalcMethod);
	virtual long GetKey() { return PARAM_SPEC_SURF_PERC; }
};

class CStatParam_MXLogNorm : public CStatParamBase
{
public:
	CStatParam_MXLogNorm(CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MX_LOGNORM; }
};

class CStatParam_DispLogNorm : public CStatParamBase
{
public:
	CStatParam_DispLogNorm( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_DISP_LOGNORM; }
};

class CStatParam_ModeLogNorm : public CStatParamBase
{
public:
	CStatParam_ModeLogNorm( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_MODE_LOGNORM; }
};

class CStatParam_AssimetryLogNorm : public CStatParamBase
{
public:
	CStatParam_AssimetryLogNorm( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_ASSIMETRY_LOGNORM; }
};

class CStatParam_ExcessLogNorm : public CStatParamBase
{
public:
	CStatParam_ExcessLogNorm( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_EXCESS_LOGNORM; }
};

class CStatParam_WeightedAverage : public CStatParamBase
{
public:
	CStatParam_WeightedAverage( CClass& cls, INamedDataPtr sptrND=NULL)
		:CStatParamBase(cls)
	{ 
		pParDscr=GetParDscr(GetKey()); 
	}

	bool Calculate();
	virtual long GetKey() { return PARAM_WEIGHTED_AVERAGE; }
};


} // ParamSpace
} // ObjectSpace
double student( double p, int n );
double FuncNormDistrib(double x);

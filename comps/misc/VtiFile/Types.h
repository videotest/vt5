#ifndef __TYPES_H__
#define __TYPES_H__


#include "MeasArg.h"
#include "corestat.h"
//#include "ComponentManager.h"
#define NOKEY	0xFFFF
#define USERKEY	0xF000

class CObjNativeArray;

enum TYPE_INFO
{ 
	TI_INT,
	TI_DBL,
	TI_UNKNOWN
};

enum TYPE_GROUP
{
	TG_USER = -1,
	TG_AREA = 1,	//area
	TG_PERIMETER = 2,	//hole params
	TG_DIAMETERS = 3,
	TG_SIZES = 4,
	TG_ORIENTATION = 5,
	TG_OPTICAL = 6,	//optical params
	TG_PLACEMENT = 7,
	TG_FORM = 8,
	TG_COLOR = 9,
	TG_MOVE = 10,
	TG_DLL = 100,
	TG_MANUAL=0	//manual measurement
};

class CTypeGroupInfo : public CObject
{
protected:
	int	m_tg;
	CString		m_strName;
public:
	CTypeGroupInfo( int tg, UINT strIdd )
	{
		m_strName.LoadString( strIdd );
		m_tg = tg;
	};

	CTypeGroupInfo( int tg, CString str )
	{
		m_strName = str;
		m_tg = tg;
	};
	CString	GetName(){return m_strName;};
	int GetGroup(){return m_tg;}

};


//Classes
class CArg;			//base argumnent class 
class CArgInt;		//integer storage
class CArgDbl;		//double storage
//in future - text string storage, picture, e t. c.
class CTypeInfo;	//Type define class - have info about type, name of arg e t. c.
class CTypeListManager;

///////////////////////////////////////////////////////////////////////////////
//Global variables

extern CTypeListManager	g_TypesManager;

///////////////////////////////////////////////////////////////////////////////
//classes declaration
class CArg : public CObject
{
	DECLARE_SERIAL(CArg);
public:
	WORD	m_nKey;
	COleVariant	m_value;
public:
	CArg()
	{m_nKey = NOKEY;
	}

	CArg( CArg &sample )
	{
		m_nKey = sample.m_nKey;
		m_value = sample.m_value;
	}



	virtual void Serialize( CArchive & );
public:
	VARTYPE GetType(){return m_value.vt;}
	WORD GetKey(){return m_nKey;}
	void SetKey( WORD wKey ){m_nKey = wKey;}
	COleVariant GetValue(){return m_value;}
	void SetValue( COleVariant val ){m_value = val;}
public:
	virtual operator double(){return m_value.dblVal;}
	virtual operator int(){return m_value.iVal;}
};

//declare somre lists 
class CArgList: public CTypedPtrList<CObList,CArg*>
{
	DECLARE_SERIAL(CArgList);
};


/*class CArgInt: public CArg
{
	DECLARE_SERIAL(CArgInt);
public:
	CArgInt( int iData = 0, WORD nKey = NOKEY );
	virtual void Serialize( CArchive & );
public:
	virtual TYPE_INFO GetType() {return TI_INT;};
	virtual operator double(){return m_iData;}
	virtual operator int(){return m_iData;}
};

class CArgDbl: public CArg
{
	DECLARE_SERIAL(CArgDbl)
public:
	CArgDbl( double fData = 0, WORD nKey = NOKEY );
	virtual void Serialize( CArchive & );
public:
	virtual TYPE_INFO GetType() {return TI_DBL;};
	virtual operator double(){return m_fData;}
	virtual operator int(){return (int)m_fData;}
};*/

//Information about colun  in table
class CTypeInfo: public CObject, public IParamTypeInfo
{
	DECLARE_SERIAL( CTypeInfo )
protected:
	int		m_iUnits;
	WORD	m_nKey;
	CString	m_strName;
	TYPE_INFO	m_info;
	BOOL	m_bEnable;
	BOOL	m_bEnable1;
	int		m_group;
	CString	m_strLongName;
	CString	m_strFormat;

	UINT	m_nID;
public:
	CTypeInfo( WORD nKey = NOKEY, TYPE_INFO info = TI_UNKNOWN, CString str = CString(), int group = -1, int iUnits = 0  )
	{
		m_nID = 0;
		m_strName = str;
		m_nKey = nKey;
		m_info = info;
		m_bEnable = TRUE;
		m_bEnable1 = FALSE;
		m_group = group;
		m_strFormat = "%0.3f";
		m_iUnits = iUnits;
	}
public:
//	void GetInfo( TABLEINFO *pTI, CArg *pArg );	//TB_WANTINFO
//	void GetHeadInfo( TABLEINFO *pTI );

	void SetID( UINT n )
	{m_nID=n;}
	UINT GetID(){return m_nID;}

	CString GetName(){ return m_strName;}
	WORD GetKey(){return m_nKey;}
	void SetName( CString str ){m_strName = str;}
	int	GetGroup(){return m_group;}
	BOOL IsEnable(){return m_bEnable||IsEnable1();};
	void Enable( BOOL b ){m_bEnable = b;};
	BOOL IsEnable1(){return m_bEnable1;};
	void Enable1( BOOL b ){m_bEnable1 = b;};
	TYPE_INFO GetTI() { return m_info; }
	CString GetLongName(){return m_strLongName.IsEmpty()?m_strName:m_strLongName;}
	void SetLongName( CString str ){ m_strLongName = str; }

	LPCSTR GetFormat(){return m_strFormat;}
	void SetFormat( CString str ) {m_strFormat = str;}

	int	GetUnits(){return m_iUnits;}

	CString GetTextValue( CArg *parg );
public:
	void Serialize( CArchive & );

// IUnknown
	virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
// IParamTypeInfo
	virtual HRESULT __stdcall GetName( BSTR *strName );
	virtual HRESULT __stdcall SetName( BSTR strName );
	virtual HRESULT __stdcall GetLongName( BSTR *strName );
	virtual HRESULT __stdcall SetLongName( BSTR strName );
	virtual HRESULT __stdcall GetKey( short *nMeasKey ); // nMeasKey - unique ID of measurement parameter
	virtual HRESULT __stdcall Enable1_( BOOL bEnable );
	virtual HRESULT __stdcall IsEnable1( BOOL *bEnable );
	virtual HRESULT __stdcall GetUnitName( BSTR *strName );
};

class CObjNative;

class CFuncContainer : public CObject
{
protected:
	FUNCDOMEAS	m_pfnCallBack;
public:
	CFuncContainer( FUNCDOMEAS p )
	{ m_pfnCallBack = p; }
public:
	FUNCDOMEAS GetFunct()
	{	ASSERT( m_pfnCallBack );return m_pfnCallBack;}
};

typedef CTypedPtrArray<CObArray, CTypeGroupInfo*> CGroupInfoArray;
typedef CTypedPtrArray<CObArray, CFuncContainer*> CFuncContainerArray;
typedef CTypedPtrList<CObList, CTypeInfo*> CTypeInfoList;


class CTypeListManager: public CTypeInfoList
{
	DECLARE_DYNAMIC(CTypeListManager);
protected:
	WORD	m_nLastKey;
	CGroupInfoArray	m_array;
	CFuncContainerArray	m_arrFunc;
	int		m_iMinSize;
	CStringArray	m_listUnits;
//	CComponentManager	m_man;

	POSITION	AddTail(CTypeInfo *p);
public:
	CTypeListManager();
	void Init();	
	void Destroy();

	void ReadRegistry();
	void WriteRegistry();
public:
	CTypeInfo *ConstructTI( TYPE_INFO info, CString str, int iUnits, int group, WORD wKey = NOKEY  );
	CTypeInfo *ConstructTI( TYPE_INFO info, UINT iDD, int iUnits, int group, WORD wKey = NOKEY );
	void ScanDLL();

	void ExecuteExternMeasure( CObjNative *, CObjNativeArray *parr );

	CTypeInfo	*Find( CString str );
	CTypeInfo	*Find( WORD nKey );
public:
	CGroupInfoArray	&GetGroupsArray(){return m_array;}
	void SetMinSize( int iS ){m_iMinSize = iS;}
	int GetMinSize(){return m_iMinSize;}
public:
	BOOL IsUnitsAvaible( int idx );
	CString GetUnitsName( int idx );
	CString GetRawUnitsName( int idx );
	int AddUnitsName( CString str );
	int AddUnitsName( UINT nIDD );

	WORD CheckKey( WORD wKey );
	CGroupInfoArray &GetGroupInfoArray() {return m_array;}
private:
	void AddGroup(CTypeGroupInfo *p);
};

#endif //__TYPES_H__
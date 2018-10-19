#ifndef __CORESTAT_H__
#define __CORESTAT_H__

// temporary interfaces need to be implemented in old VT4 to support new statistic

interface IStatDoc : public IUnknown // supports IDocument, IDocData, INotifyController, ITypesManager
{
	virtual HRESULT __stdcall GetClassManager( IUnknown **pp ) = 0;
	virtual HRESULT __stdcall GetShotArray( IUnknown **pp ) = 0;
	virtual HRESULT __stdcall GetCurParam( short *nMeasKey ) = 0;
	virtual HRESULT __stdcall SetCurParam( short nMeasKey ) = 0;
};


interface IShotArray : public IUnknown
{
	virtual HRESULT __stdcall GetCount( short *nCount ) = 0;
	virtual HRESULT __stdcall GetShot( short nIndex, IUnknown **ppShot ) = 0; // IShot
	virtual HRESULT __stdcall DelShot( IUnknown *pShot ) = 0; // IShot
};


interface IShot : public IUnknown
{
	virtual HRESULT __stdcall GetArea( double *fArea ) = 0;
	virtual HRESULT __stdcall GetName( BSTR *strName ) = 0;
	virtual HRESULT __stdcall IsEnabled( BOOL *bEnabled ) = 0;
	virtual HRESULT __stdcall Enable_( BOOL bEnable ) = 0;
	virtual HRESULT __stdcall GetObjArray( IUnknown **ppArray ) = 0;
	virtual HRESULT __stdcall GetShotSource( IUnknown **ppSource ) = 0; // NOT implemented
};

interface IParamTypeInfo : public IUnknown
{
	virtual HRESULT __stdcall GetName( BSTR *strName ) = 0;
	virtual HRESULT __stdcall SetName( BSTR strName ) = 0;
	virtual HRESULT __stdcall GetLongName( BSTR *strName ) = 0;
	virtual HRESULT __stdcall SetLongName( BSTR strName ) = 0;
	virtual HRESULT __stdcall GetKey( short *nMeasKey ) = 0; // nMeasKey - unique ID of measurement parameter
	virtual HRESULT __stdcall Enable1_( BOOL bEnable ) = 0;
	virtual HRESULT __stdcall IsEnable1( BOOL *bEnable ) = 0;
	virtual HRESULT __stdcall GetUnitName( BSTR *strName ) = 0;
};

interface IParamTypeInfo2 : public IParamTypeInfo
{
	virtual HRESULT __stdcall GetFormat( BSTR *pbstrFormat ) = 0;
};

interface ITypesManager : public IUnknown
{
	virtual HRESULT __stdcall FindTypeInfo( short nMeasKey, IUnknown **ppTypeInfo ) = 0;
	virtual HRESULT __stdcall FindTypeInfo( BSTR strShortName, IUnknown **ppTypeInfo ) = 0;
	virtual HRESULT __stdcall GetFirstTypeInfoPosition( long *lPosRef ) = 0;
	virtual HRESULT __stdcall GetNextTypeInfo( long *lPosRef, IUnknown **ppTypeInfo ) = 0; // IParamTypeInfo
};


interface IClassInfo : public IUnknown // supports ILimitList
{
	virtual HRESULT __stdcall GetName( BSTR *strName ) = 0;
	virtual HRESULT __stdcall SetName( BSTR strName ) = 0;
	virtual HRESULT __stdcall GetColor( OLE_COLOR *color ) = 0;
	virtual HRESULT __stdcall SetColor_( OLE_COLOR color ) = 0;
	virtual HRESULT __stdcall TestObject( IUnknown *pObject, BOOL *bBelong ) = 0; // Only for internal use
	virtual HRESULT __stdcall SetAttributes( long l ) = 0;
	virtual HRESULT __stdcall GetAttributes( long *l ) = 0;
	virtual HRESULT __stdcall GetKey( long *lKey ) = 0;
	virtual HRESULT __stdcall SetUserVal( void *pVal ) = 0;
	virtual HRESULT __stdcall GetUserVal( void **ppVal ) = 0;
};


enum ClassAttributes{ caEnabled=1, caRelative=2 };

interface IClassManager : public IUnknown
{
	virtual HRESULT __stdcall GetFirstClassPosition( long *lPosRef ) = 0;
	virtual HRESULT __stdcall GetNextClass( long *lPosRef, IUnknown **ppClassInfo ) = 0; // IClassInfo
	virtual HRESULT __stdcall CreateAddClass( IUnknown **ppClassInfo ) = 0;
	virtual HRESULT __stdcall DeleteClass( LPOS lPos ) = 0;
	virtual HRESULT __stdcall TestObject( IUnknown *pObject, IUnknown **ppClassInfoRet ) = 0;
	virtual HRESULT __stdcall GetClassByNum( int iNum, IUnknown **ppClassInfoRet ) = 0;
};

interface ILimit : public IUnknown
{
	virtual HRESULT __stdcall SetKey( short nMeasKey ) = 0;
	virtual HRESULT __stdcall GetKey( short *nMeasKey ) = 0;
	virtual HRESULT __stdcall TestObject( IUnknown *pObject, BOOL *bConfirm ) = 0;
	virtual HRESULT __stdcall SetLoBound( double fVal ) = 0;
	virtual HRESULT __stdcall SetHiBound( double fVal ) = 0;
	virtual HRESULT __stdcall GetBounds( double *fLo, double *fHi ) = 0;
	virtual HRESULT __stdcall IsValid( BOOL *bValid ) = 0;
};


interface ILimitList : public IUnknown // supports IClassInfo
{
	virtual HRESULT __stdcall GetLimitsCount( short *nCount ) = 0;
	virtual HRESULT __stdcall GetLimit( short nMeasKey, IUnknown **ppLimit ) = 0; // Ilimit
	virtual HRESULT __stdcall CreateAddLimit( IUnknown **ppLimit ) = 0;
	virtual HRESULT __stdcall GetFirstLimitPosition( long *lPosRef ) = 0;
	virtual HRESULT __stdcall GetNextLimit( long *lPosRef, IUnknown **ppLimit ) = 0; // Ilimit
	virtual HRESULT __stdcall DeleteLimit( LPOS lPos ) = 0;
};

enum StatNotifyCause
{
	sncUnknown = -1,
	sncAddClass = 1,
	sncDeleteClass = 2,
	sncChangeClassData = 3,
	sncChangeSepTypes = 4,
	sncChangeStatParam = 5,
	sncChangeStatSettings = 6,
	sncGeneral = 7,
	sncChangeCalibr = 8,
	sncOther = 0
};

interface IStatHintInfo : public IUnknown
{
	virtual HRESULT __stdcall GetCause( int *pdwCause ) = 0;
	virtual HRESULT __stdcall GetModifiedObject( IUnknown **punk ) = 0;
	virtual HRESULT __stdcall GetModifiedObjectKey( DWORD *pdwKey ) = 0;
};



extern GUID IID_ILimitList; // {32792D9A-F497-11d1-B802-000001123207}
extern GUID IID_ILimit; // {32792D99-F497-11d1-B802-000001123207}
extern GUID IID_IStatDoc;	// {32792D94-F497-11d1-B802-000001123207}
extern GUID IID_IShotArray;	// {32792D95-F497-11d1-B802-000001123207}
extern GUID IID_IShot;		// {32792D96-F497-11d1-B802-000001123207}
extern GUID IID_IParamTypeInfo;	// {32792D97-F497-11d1-B802-000001123207}
extern GUID IID_IParamTypeInfo2; // {39C7DA03-DB13-44CC-B745-98A6697C146A}
extern GUID IID_ITypesManager;	// {32792D98-F497-11d1-B802-000001123207}
extern GUID IID_IClassInfo;		// {32792D9B-F497-11d1-B802-000001123207}
extern GUID IID_IClassManager;	// {32792D9D-F497-11d1-B802-000001123207}
extern GUID IID_IStatHintInfo;	// {32792D9D-F497-11d1-B802-000001123207}

#endif //__CORESTAT_H__
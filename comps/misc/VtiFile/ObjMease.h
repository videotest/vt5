#ifndef __ObjMeas_h__
#define __ObjMeas_h__

#include "Object.h"
#include "Types.h"

void CreateMeaseImage( C8Image &imgSrc, C8Image &img, CRect rc );


enum OBJ_TYPE
{
	OBJ_HANDY = 0,
	OBJ_USERCALC,
	OBJ_AUTOCALC,
};

class CImgDoc;
				  

//for measure
class CObjMeasure : public CObjMove
{
	DECLARE_SERIAL(CObjMeasure);
protected:
//required for statistic
	double	m_fArea;
	double	m_fAngle;
	CArgList	m_ArgList;
	CImgDoc	*m_pStoreDoc;
	BOOL	m_bAngleReady;
	CMapStringToPtr	m_map;
	CObjArray *m_parr;

	virtual void DoAllocCopy();
public:
	CObjMeasure();
	virtual ~CObjMeasure();
	CObjNative *Clone();

	virtual void Attach( CObjNative & );
	virtual void AttachData( CObjNative & );
	virtual void DeInit();
	virtual void DoFree();

	virtual void DoPaint( CDC &theDC, double iZoom, int iType, BOOL bActive );

	virtual void Serialize( CArchive & );
	virtual int GetClass();

	virtual void SetValue( WORD wKey, double fValue );
	virtual COleVariant GetValue( WORD wKey );

	virtual double GetArea(){return m_fArea;};
	virtual double GetAngle();
	virtual void SetAngle( double f ){m_fAngle = f;}
public:
	
	//CArgList &GetArray(){return m_ArgList;}
	void AddArg( CArg *pArg ){ m_ArgList.AddTail( pArg ); };
	inline void SetArg( CTypeInfo *pInfo, double f );
	inline void SetArg( WORD wKey, double f );
	POSITION GetFirstArgPosition(){return m_ArgList.GetHeadPosition();};
	CArg* GetNextArg( POSITION &pos ){return m_ArgList.GetNext( pos ); }
	inline CArg *GetArg( WORD wKey );
	int GetArgsCount(){return m_ArgList.GetCount();}

	void AttachDataTable( CObjMeasure & );

	

public:	//measure function
	void PaintFrame( CDC &theDC, double iZoom, BOOL bActive );
	virtual void DoMeasure();
public:
	void SetValueToDocument( UINT iddStr, double fVal );
	virtual BOOL GetObjType(){return OBJ_AUTOCALC;};

	virtual void BeginMeasure( CDocument *pdoc, CObjArray *parr );
	virtual void EndMeasure()
	{	m_pStoreDoc = 0; m_parr = NULL; }
	CObjArray *GetArray() {return m_parr;}

	virtual void GetCurrentCalibr( double &fByX, double &fByY );

	virtual HRESULT __stdcall GetParamCount( int *picount );
	virtual HRESULT __stdcall GetParamKey( int ipos, WORD *pwKey );
	virtual HRESULT __stdcall GetParamName( int ipos, BSTR *pbstr );
	virtual HRESULT __stdcall IsParamEnabled( WORD wKey );

	virtual HRESULT __stdcall SetUserData( BSTR lpcszName, LPCVARIANT pcvarData );
	virtual HRESULT __stdcall GetUserData( BSTR lpcszName, LPVARIANT pvarData );
	virtual HRESULT __stdcall SetClass( int iKey, BOOL bDirect );
protected:
	void EmptyMap();
	void SerializeMap( CArchive &ar );
};


#endif //__ObjMeas_h__
#ifndef __ObNative_h__
#define __ObNative_h__

#include "AfxTempl.h"
#include "SPoint.h"
#include "AddInObj.h"


#define CLASS_AUTO	-1

#ifndef NORECT	//no-vt4 ussage ( dll possible )

#define NORECT	CRect(0, 0, 0, 0)
//memory allocation function
inline LPVOID AllocMem( DWORD dwSize )
{	LPVOID p = ::malloc( dwSize ); memset(p, 0, dwSize); return p; }

#define FreeMem( p ) if ( ( p ) ) ::free(  p ); p = 0;


#endif //NORECT

class CArg;
class CContourArray;

//contour class 
class CCntrNative : public CObject
{
//	DECLARE_SERIAL( CCntrNative );
protected:
	int	m_iSize, m_iMaxSize;
	CshortPoint	*m_ptArray;
public:
	__int32 *m_pSingulPts;
	double  *m_pSingPtDir;
	RECT    *m_pSingulRects;
	int m_nSingulPtsNum;
	CRect	m_rc;
	BOOL m_bFork;
	int m_nSupLen;
public:
	CCntrNative();
	CCntrNative( CCntrNative& );

	virtual ~CCntrNative();

		//serialize interface
	virtual void Serialize( CArchive & ){};
public:
	virtual void Realloc(){};
	virtual void OnAddPoint( CPoint ){};
	virtual void AddSegment( CPoint ){};		//добавить сегмент

public:
	void DeInit();					//clear data

	virtual void Walk( BITMAPINFOHEADER *pbi, CPoint ptStart, int idir=-1){};
	virtual void FillFast( BITMAPINFOHEADER *pbi, BYTE byteExtMask, BYTE byteIntMask, CContourArray &array ){};

	void Add( CPoint );				//добавить точку
	void AddDirect( CPoint );		//добавить точку без проверок
	

	BOOL IsEmpty(){return m_iSize == 0l;}		//Не пустой?
	CshortPoint *GetPtArray(){return m_ptArray;}
public:
	//operators
	CshortPoint &operator[]( int index );
	//attach external point's array to contour
	void Attach( POINTS	*p, int iSize );
	void Attach( CCntrNative & );
	void Attach( __int32 *p, double *d, RECT *r, int iNum );
public:
	//interface
	DWORD	GetSize(){return m_iSize;}			//возвратить размер
	CRect	GetRect(){return m_rc;}				//Охватывающий прямоугольник
	virtual DWORD CalcBytesSize() const;
private:int	m_cRefs;
public:
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	virtual HRESULT __stdcall QueryInterface( REFIID iid , void** ppvObject );

	virtual HRESULT __stdcall Size( int *pisize );
	virtual HRESULT __stdcall MakeEmpty();
	virtual HRESULT __stdcall IsEmpty( BOOL *pbIs );

	virtual HRESULT __stdcall GetPoint( int idx, POINT *pt );
	virtual HRESULT __stdcall SetPoint( int idx, POINT pt );
	virtual HRESULT __stdcall AddPoint( POINT pt );
	virtual HRESULT __stdcall AddPointDirect( POINT pt );
	virtual HRESULT __stdcall AddPointLine( POINT pt );
};

inline CshortPoint &CCntrNative::operator[]( int index )
	{ASSERT(index>=0 && index<m_iSize);return m_ptArray[index];};

inline DWORD CCntrNative::CalcBytesSize() const
{
	return m_iMaxSize*sizeof(CshortPoint)+m_nSingulPtsNum*(sizeof(__int32)+sizeof(double))+
		sizeof(CCntrNative);
};


//array class
class CContourArray : public CTypedPtrArray<CObArray, CCntrNative *>, public IContourArray
{
//	DECLARE_SERIAL( CContourArray );
public:
	CContourArray(){m_cRefs=1;};

	void Attach( CContourArray & );
	void DeInit();
public:
//	CCntrNative &operator []( int idx ){return *GetAt( idx );}
	virtual DWORD CalcBytesSize() const;
private:int	m_cRefs;
public:
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	virtual HRESULT __stdcall QueryInterface( REFIID iid , void** ppvObject );

	virtual HRESULT __stdcall Size( int *pisize );

	virtual HRESULT __stdcall GetContour( int idx, IUnknown **pcntr );
	virtual HRESULT __stdcall SetContour( int idx, IUnknown **pcntr );
	virtual HRESULT __stdcall AddContour( IUnknown *pcntr );

	virtual HRESULT __stdcall CreateNewContour( IUnknown **pcntr );
};


class CImageBase;
class CObjArray;

//object class - used for data storage
class CBaseDoc;
class CObjNative : public CObject, public IObject3
{
//	DECLARE_SERIAL( CObjNative );
friend class CObjMove;
protected:
		//array of pointer to contour object
	CContourArray	m_contours;
		//frame rectangle
	CRect			m_rc;		
		//pointers to binary mask and other image
	int	 m_iClassNo;
	UINT m_nObjSchema;
	__int64	m_iCode;
	void GetSchema(CArchive &arch)
	{
		if (arch.IsLoading())
		{
			UINT nSchema = arch.GetObjectSchema();
			if (nSchema != (UINT)-1)
				m_nObjSchema = nSchema;
		}
	};
public:
	CObjNative():CObject(){m_cRefs=1; m_iClassNo = CLASS_AUTO; m_nObjSchema = (UINT)-1;};
	virtual ~CObjNative(){};
	virtual CObjNative *Clone() {ASSERT( FALSE); return NULL;}
	virtual void DeInit(){ASSERT( FALSE );};
		//serialize interface
	virtual void Serialize( CArchive & ){};
	virtual void Attach( CObjNative & ){ASSERT( FALSE );};
	virtual void DoFree(){ASSERT( FALSE );};
	virtual void AddContour( CCntrNative *p ){m_contours.Add( p );};
	virtual void CreateMask( CCntrNative *p ) {};
	virtual void FindContours( BOOL bFindHoles = FALSE ){};
	virtual void AttachImg( void * ) {};
	virtual void AttachHostBin( CImageBase * ) {};
		//some virtual functions
	virtual void DoMeasure(){};
	virtual void DoPaint( CDC &theDC, double iZoom, int iType, BOOL bActive ){};
	virtual void AttachData( CObjNative & ){};
	virtual CArg *GetArg( WORD ){return 0;};
	virtual void SetValue( WORD wKey, double fValue ){};
	virtual COleVariant GetValue( WORD wKey ){COleVariant var;return var;};

	virtual void BeginMeasure( CDocument *pdoc, CObjArray *parr ){};
	virtual void EndMeasure(){};

	virtual void CreateBIN( CSize size, BOOL bMapContours = FALSE ){};
	virtual void MapBackground(CBaseDoc *pDoc, CObjNative *pExc = NULL) {};
public:
	virtual CCntrNative	*CreateContour(){return 0;};
public:
	//interface
	virtual int	GetClass(){return m_iClassNo;}
	virtual void SetRect( CRect rc ){m_rc = rc;}
	CRect GetRect(){return m_rc;}

	CContourArray &GetCntrArray(){return m_contours;}

	virtual LPBITMAPINFOHEADER GetBinBI(){ASSERT( FALSE );return 0;}
	virtual LPBITMAPINFOHEADER GetImgBI(){ASSERT( FALSE );return 0;}
	virtual LPBITMAPINFOHEADER GetIconBI(){ASSERT( FALSE );return 0;}

	void SetCode( __int64 iNew )
	{
		m_iCode = iNew;
	};
	__int64 GetCode(){return m_iCode;};
	virtual CRect CalcInvalidateRect() {return GetRect();}
	virtual void OnUpdate(CDocument *pDoc, CObjNative* pSender, LPARAM lHint, CObject* pHint) {}
public:
	virtual double GetAngle(){return 0;}
	virtual void SetAngle( double f ){};

	virtual void GetCurrentCalibr( double &fByX, double &fByY ){fByX=fByY=1;}
	virtual DWORD CalcBytesSize() const {return m_contours.CalcBytesSize() + sizeof(CObjNative);}
private:int	m_cRefs;
public:
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	virtual HRESULT __stdcall QueryInterface( REFIID iid , void** ppvObject );

	virtual HRESULT __stdcall SetVal( WORD wKey, double fValue );
	virtual HRESULT __stdcall GetVal( WORD wKey, double *pfValue );

	virtual HRESULT __stdcall GetContourArray( IUnknown **parray );
	virtual HRESULT __stdcall SetContourArray( void *parray );

	virtual HRESULT __stdcall GetBinBI( LPBITMAPINFOHEADER *ppbi );
	virtual HRESULT __stdcall GetImgBI( LPBITMAPINFOHEADER *ppbi );
	virtual HRESULT __stdcall GetIconBI( LPBITMAPINFOHEADER *ppbi );
	virtual HRESULT __stdcall SetBinBI( LPBITMAPINFOHEADER ppbi );
	virtual HRESULT __stdcall SetImgBI( LPBITMAPINFOHEADER ppbi );


	virtual HRESULT __stdcall GetObjAngle( double *pf );
	virtual HRESULT __stdcall SetObjAngle( double f );

	virtual HRESULT __stdcall GetCurrentClbr( double *pfByX, double *pfByY );

	virtual HRESULT __stdcall IsManual( BOOL *pbIs )
	{ *pbIs = false;return S_OK;}
	virtual HRESULT __stdcall GetClass( int *piClass );
	virtual HRESULT __stdcall GetRect( LPRECT prect )
	{ prect->top = m_rc.top;prect->left = m_rc.left;
	prect->right = m_rc.right;prect->bottom = m_rc.bottom; return S_FALSE; }
	//some global support
	virtual HRESULT __stdcall GetParamCount( int *picount );
	virtual HRESULT __stdcall GetParamKey( int ipos, WORD *pwKey );
	virtual HRESULT __stdcall GetParamName( int ipos, BSTR *pbstr );
	virtual HRESULT __stdcall IsParamEnabled( WORD wKey ){return S_FALSE;};

	virtual HRESULT __stdcall SetUserData( BSTR lpcszName, LPCVARIANT pcvarData )
		{ return E_NOTIMPL; }
	virtual HRESULT __stdcall GetUserData( BSTR lpcszName, LPVARIANT pvarData )
		{ return E_NOTIMPL; }
	virtual HRESULT __stdcall SetClass( int iKey, BOOL bDirect )
	{ m_iClassNo = iKey; return S_OK; }
	virtual HRESULT __stdcall CloneObject(IObject2 **ppCopy, BOOL bCopyContent);
	virtual HRESULT __stdcall FreeObject();
	virtual HRESULT __stdcall MoveTo(int x, int y) {return S_OK;}
	virtual HRESULT __stdcall GetObjectCode(__int64 *pnCode) {*pnCode = m_iCode; return S_OK;};
};

//array class for object
class CObjNativeArray : public CTypedPtrArray<CObArray, CObjNative *>, public IObjArray2
{
//	DECLARE_SERIAL( CObjNativeArray );
public:
	CObjNativeArray()
	{m_cRefs = 1;}	

	~CObjNativeArray();
public:
	virtual void Attach( CObjNativeArray *parr ){};
	virtual void DeInit();
public:
	virtual CObjNative *CreateObjNative(){return 0;};
	virtual CObjNative *CreateObjMove(){return 0;};
	virtual CObjNative *CreateObjMeasure(){return 0;};
	virtual DWORD CalcBytesSize() const;
	virtual CObjNativeArray *OnClone(BOOL bCopyContent);
	virtual void OnAdd(CObjNativeArray *parr);
private:int	m_cRefs;
public:
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	virtual HRESULT __stdcall QueryInterface( REFIID iid , void** ppvObject );

	virtual HRESULT __stdcall Size( int *pisize );

	virtual HRESULT __stdcall GetObject( int idx, IUnknown **pobj );
	virtual HRESULT __stdcall SetObject( int idx, void *pobj );
	virtual HRESULT __stdcall AddObject( int idx, void *pobj );

	virtual HRESULT __stdcall CloneArray(IObjArray **pCopy, BOOL bCopyContent); // Make objarray same type and with same content
	virtual HRESULT __stdcall AddArray(IObjArray *pAdd); // Add copies of all objects
	virtual HRESULT __stdcall FreeArray();
	virtual HRESULT __stdcall SetObject2(int idx, IObject2 *pobj); // GetObject and IObject2::CloneObject give 
	virtual HRESULT __stdcall AddObject2(IObject2 *pobj); // IUnknown(IObject)*. It's difficult cast it to void*
};


#include "ObNative.inl"

#endif //__ObNative_h__
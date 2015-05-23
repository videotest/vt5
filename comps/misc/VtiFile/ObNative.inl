#ifndef __ObNative_inl__
#define __ObNative_inl__

//ccntrNative

#ifndef __VT4__
inline CCntrNative *CreateNewContour()
{	return new CCntrNative;}
#else
CCntrNative *CreateNewContour();
#endif //__VT4__


//конструктор. Инициализация переменных
inline CCntrNative::CCntrNative()
{
	m_cRefs = 1;

	m_iSize = 0;
	m_iMaxSize = 0;
	m_ptArray = 0;
	m_rc = NORECT;
	m_bFork = FALSE;
	m_nSupLen = 0;
	m_pSingulPts = NULL;
	m_pSingulRects = NULL;
	m_pSingPtDir = NULL;
	m_nSingulPtsNum = 0;
}

inline CCntrNative::CCntrNative( CCntrNative &cntr )	
{
	m_iSize = 0;
	m_iMaxSize = 0;
	m_ptArray = 0;
	m_rc = NORECT;
	m_bFork = FALSE;
	m_nSupLen = 0;
	m_pSingulPts = NULL;
	m_pSingPtDir = NULL;
	m_pSingulRects = NULL;
	m_nSingulPtsNum = 0;

	Attach( cntr );
}


//деструктор. Освобождение памяти
inline CCntrNative::~CCntrNative()	
{
	FreeMem(m_ptArray);
	FreeMem(m_pSingulPts);
	FreeMem(m_pSingPtDir);
	FreeMem(m_pSingulRects);
}


inline void CCntrNative::Attach( POINTS *p, int iSize )
{
	if( m_ptArray )
		FreeMem( m_ptArray );
	m_ptArray = (CshortPoint *)p;
	m_iMaxSize = m_iSize = iSize;
}

inline void CCntrNative::Attach( __int32 *p, double *d, RECT *r, int iNum )
{
	if( m_pSingulPts )
		FreeMem( m_pSingulPts );
	m_pSingulPts = p;
	m_pSingPtDir = d;
	m_pSingulRects = r;
	m_nSingulPtsNum = iNum;
}

inline void CCntrNative::Attach( CCntrNative &c )
{
	DWORD	dwSize = sizeof( __int32)*c.m_nSingulPtsNum;

	if (dwSize)
	{
		__int32 *p = (__int32 *)AllocMem( dwSize );
		memcpy( p, c.m_pSingulPts, dwSize );
		double *d = (double *)AllocMem(sizeof(double)*c.m_nSingulPtsNum);
		memcpy( d, c.m_pSingPtDir,sizeof(double)*c.m_nSingulPtsNum);
		RECT *r = (RECT *)AllocMem(sizeof(RECT)*c.m_nSingulPtsNum);
		memcpy( r, c.m_pSingulRects,sizeof(RECT)*c.m_nSingulPtsNum);
		Attach( p, d, r, c.m_nSingulPtsNum );
	}

	dwSize = sizeof( POINTS)*c.m_iMaxSize;

	if (dwSize)
	{
		POINTS *p = (POINTS *)AllocMem( dwSize );
		memcpy( p, c.m_ptArray, dwSize );
		Attach( p, c.m_iSize );
	}

	m_rc = c.m_rc;
}

//функция Clear. очистка данных
inline void CCntrNative::DeInit()
{
	m_iMaxSize = 0;
	m_iSize = 0;

	if(m_ptArray)
		FreeMem(m_ptArray);
	m_ptArray = 0;
	m_rc = NORECT;
}

//функция Add. добавить точку
inline void CCntrNative::Add(CPoint pt)
{
	if(m_iSize == m_iMaxSize)
		Realloc();
	if(m_iSize)
	{
		if(m_ptArray[m_iSize-1] == pt)return;
		m_rc.left = min(m_rc.left, pt.x);
		m_rc.right = max(m_rc.right, pt.x+1);
		m_rc.top = min(m_rc.top, pt.y);
		m_rc.bottom = max(m_rc.bottom, pt.y+1);
	}
	else
	{
		m_rc.left = pt.x;
		m_rc.right = pt.x+1;
		m_rc.top = pt.y;
		m_rc.bottom = pt.y+1;
	}
	m_ptArray[m_iSize] = pt;
	m_iSize++;
	OnAddPoint(pt);
}

//функция AddDirect. добавить точку без проверок на наличие
inline void CCntrNative::AddDirect(CPoint pt)
{
	if(m_iSize == m_iMaxSize)
		Realloc();
	if(m_iSize)
	{
		m_rc.left = min(m_rc.left, pt.x);
		m_rc.right = max(m_rc.right, pt.x+1);
		m_rc.top = min(m_rc.top, pt.y);
		m_rc.bottom = max(m_rc.bottom, pt.y+1);
	}
	else
	{
		m_rc.left = pt.x;
		m_rc.right = pt.x+1;
		m_rc.top = pt.y;
		m_rc.bottom = pt.y+1;
	}
	m_ptArray[m_iSize] = pt;
	m_iSize++;
	OnAddPoint(pt);
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
/////////////////////CObjNativeArray 

inline CObjNativeArray::~CObjNativeArray()
{
	DeInit();
}

inline void CObjNativeArray::DeInit()
{
	for( int i = 0; i<GetSize(); i++ )
		delete GetAt( i );
	RemoveAll();
}

inline void CContourArray::DeInit()
{
	for( int i = 0; i<GetSize(); i++ )
		delete GetAt( i );
	RemoveAll();
}

inline void CContourArray::Attach( CContourArray &arr )
{
	for( int i = 0; i < arr.GetSize(); i++ )
	{
		CCntrNative	*s = arr.GetAt( i );
		CCntrNative	*p = (CCntrNative	*)s->GetRuntimeClass()->CreateObject();
		p->Attach( *arr.GetAt( i ) );
		Add( p );
	}
}

////////////////////////////////////////////////////
/////////////////CContourArray
inline DWORD CContourArray::CalcBytesSize() const
{
	DWORD dwBytesSize = 0;
	for (int i = 0; i < GetSize(); i++)
	{
		CCntrNative *p = (CCntrNative *)GetAt(i);
		dwBytesSize += p->CalcBytesSize();
	}
	return dwBytesSize;
}

////////////////////////////////////////////////////
/////////////////CObjNativeArray
inline DWORD CObjNativeArray::CalcBytesSize() const
{
	DWORD dwBytesSize = 0;
	for (int i = 0; i < GetSize(); i++)
	{
		CObjNative *p = (CObjNative *)GetAt(i);
		dwBytesSize += p->CalcBytesSize();
	}
	return dwBytesSize;
}


//implementation of interface
inline ULONG CCntrNative::AddRef()
{	return ++m_cRefs; }

inline ULONG CCntrNative::Release()
{	return --m_cRefs; }

inline HRESULT CCntrNative::QueryInterface( REFIID iid , void** ppvObject )
{	
	*ppvObject = 0;

	if( iid == IID_IUnknown||
		iid == IID_IContour )
	{
		*ppvObject = (IContour*)this;
		((IUnknown*)this)->AddRef();
		return S_OK;
	}
	return E_NOINTERFACE; 
}

inline HRESULT CCntrNative::Size( int *pisize )
{ *pisize = GetSize(); return S_OK; }

inline HRESULT CCntrNative::MakeEmpty()
{ DeInit(); return S_OK; }

inline HRESULT CCntrNative::IsEmpty( BOOL *pbIs )
{ *pbIs = GetSize() == 0; return S_OK; }

inline HRESULT CCntrNative::GetPoint( int idx, POINT *ppt )
{ *ppt = (*this)[idx]; return S_OK; }

inline HRESULT CCntrNative::SetPoint( int idx, POINT pt )
{ (*this)[idx]=pt; return S_OK; }

inline HRESULT CCntrNative::AddPoint( POINT pt )
{ Add( pt ); return S_OK; }

inline HRESULT CCntrNative::AddPointDirect( POINT pt )
{ AddDirect( pt ); return S_OK; }

inline HRESULT CCntrNative::AddPointLine( POINT pt )
{ Add( pt ); return S_OK; }


//IContourArray
inline ULONG CContourArray::AddRef()
{	return ++m_cRefs; }

inline ULONG CContourArray::Release()
{	return --m_cRefs; }

inline HRESULT CContourArray::QueryInterface( REFIID iid , void** ppvObject )
{	
	*ppvObject = 0;

	if( iid == IID_IUnknown||
		iid == IID_IContourArray )
	{
		*ppvObject = (IContourArray*)this;
		((IUnknown*)this)->AddRef();
		return S_OK;
	}
	return E_NOINTERFACE; 
}

inline HRESULT CContourArray::Size( int *pisize )
{ *pisize = GetSize(); return S_OK; }

inline HRESULT CContourArray::GetContour( int idx, IUnknown **pcntr )
{ 
	*pcntr = 0;
	return GetAt( idx )->QueryInterface( IID_IContour, (void **)pcntr );
}

inline HRESULT CContourArray::SetContour( int idx, IUnknown **pcntr )
{ SetAt( idx, (CCntrNative*)pcntr ); return S_OK; }

inline HRESULT CContourArray::AddContour( IUnknown *pcntr )
{ Add( (CCntrNative*)pcntr ); return S_OK; }

inline HRESULT CContourArray::CreateNewContour( IUnknown **pcntr )
{
	CCntrNative *pnew = ::CreateNewContour();

	*pcntr = (IContour*)pnew;

	return S_OK;
}

//IObject
inline ULONG CObjNative::AddRef()
{	return ++m_cRefs; }

inline ULONG CObjNative::Release()
{	return --m_cRefs; }

inline HRESULT CObjNative::QueryInterface( REFIID iid , void** ppvObject )
{	
	*ppvObject = 0;

	if( iid == IID_IUnknown|| iid == IID_IObject || iid == IID_IObject2 || iid == IID_IObject3)
	{
		*ppvObject = (IObject3*)this;
		((IUnknown*)this)->AddRef();
		return S_OK;
	}
	return E_NOINTERFACE; 
}

inline HRESULT CObjNative::SetVal( WORD wKey, double fValue )
{ SetValue( wKey, fValue ); return S_OK; }
			
inline HRESULT CObjNative::GetVal( WORD wKey, double *pfValue )
{
	*pfValue = 0;
	COleVariant var = GetValue( wKey );
	if( var.vt == VT_EMPTY )
		return S_FALSE;
	*pfValue = GetValue( wKey ).dblVal;
	return S_OK; 
}

inline HRESULT CObjNative::GetContourArray( IUnknown **pparray )
{
	*pparray = 0;
	return m_contours.QueryInterface( IID_IContourArray, (void **)pparray );
}

inline HRESULT CObjNative::SetContourArray( void *parray )
{ return S_OK; }

inline HRESULT CObjNative::GetBinBI( LPBITMAPINFOHEADER *ppbi )
{ *ppbi = GetBinBI(); return S_OK; }

inline HRESULT CObjNative::GetImgBI( LPBITMAPINFOHEADER *ppbi )
{ *ppbi = GetImgBI(); return S_OK; }
inline HRESULT CObjNative::GetIconBI( LPBITMAPINFOHEADER *ppbi )
{ *ppbi = GetIconBI(); return S_OK; }

inline HRESULT CObjNative::SetBinBI( LPBITMAPINFOHEADER ppbi )
{ return S_OK; }
inline HRESULT CObjNative::SetImgBI( LPBITMAPINFOHEADER ppbi )
{ return S_OK; }

inline HRESULT CObjNative::GetObjAngle( double *pf )
{ *pf = GetAngle(); return S_OK; }

inline HRESULT CObjNative::SetObjAngle( double f )
{ SetAngle( f ); return S_OK; }

inline HRESULT CObjNative::GetCurrentClbr( double *pfByX, double *pfByY )
{ GetCurrentCalibr( *pfByX, *pfByY ); return S_OK; }

inline HRESULT CObjNative::GetParamCount( int *picount )
{	return S_OK;}

inline HRESULT CObjNative::GetParamKey( int ipos, WORD *pwKey )
{	return S_OK;}

inline HRESULT CObjNative::GetParamName( int ipos, BSTR *pbstr )
{	return S_OK; }
inline HRESULT CObjNative::GetClass( int *piClass )
{ *piClass = GetClass(); return S_OK; }

inline HRESULT CObjNative::CloneObject(IObject2 **ppCopy, BOOL bCopyContent)
{
	CObjNative *p = (CObjNative *)GetRuntimeClass()->CreateObject();
	if (p && bCopyContent)
		p->Attach(*this);
	*ppCopy = (IObject2 *)p;
	return p?S_OK:E_OUTOFMEMORY;
}

inline HRESULT CObjNative::FreeObject()
{
	delete this;
	return S_OK;
}


//IObjArray
inline ULONG CObjNativeArray::AddRef()
{	return ++m_cRefs; }

inline ULONG CObjNativeArray::Release()
{	return --m_cRefs; }

inline HRESULT CObjNativeArray::QueryInterface( REFIID iid , void** ppvObject )
{	
	*ppvObject = 0;

	if( iid == IID_IUnknown||
		iid == IID_IObjArray )
	{
		*ppvObject = (IObjArray*)this;
		((IUnknown*)this)->AddRef();
		return S_OK;
	}
	if( iid == IID_IObjArray2 )
	{
		*ppvObject = (IObjArray2*)this;
		((IUnknown*)this)->AddRef();
		return S_OK;
	}
	return E_NOINTERFACE; 
}

inline HRESULT CObjNativeArray::Size( int *pisize )
{ *pisize = GetSize(); return S_OK; }

inline HRESULT CObjNativeArray::GetObject( int idx, IUnknown **pcntr )
{ 
	*pcntr = 0;
	return GetAt( idx )->QueryInterface( IID_IObject, (void **)pcntr ); 
}

inline HRESULT CObjNativeArray::SetObject( int idx, void *pcntr )
{ SetAt( idx, (CObjNative*)pcntr ); return S_OK; }

inline HRESULT CObjNativeArray::AddObject( int idx, void *pcntr )
{ Add( (CObjNative*)pcntr ); return S_OK; }


inline CObjNativeArray *CObjNativeArray::OnClone(BOOL bCopyContent)
{
	CObjNativeArray *p = new CObjNativeArray;
	if (p && bCopyContent)
		p->Attach(this);
	return p;
}

inline void CObjNativeArray::OnAdd(CObjNativeArray *parr)
{
	for (int i = 0; i < parr->GetSize(); i++)
	{
		CObjNative *pobj = (CObjNative *)parr->GetAt(i);
		CObjNative *pcpy = (CObjNative *)pobj->GetRuntimeClass()->CreateObject();
		pcpy->Attach(*pobj);
		Add(pcpy);
	}
}

inline HRESULT CObjNativeArray::CloneArray(IObjArray **ppCopy, BOOL bCopyContent)
{
	*ppCopy = OnClone(bCopyContent);
	return *ppCopy?S_OK:E_OUTOFMEMORY;
}

inline HRESULT CObjNativeArray::AddArray(IObjArray *pAdd)
{
	OnAdd((CObjNativeArray *)pAdd);
	return S_OK;
}

inline HRESULT CObjNativeArray::FreeArray()
{
	delete this;
	return S_OK;
}

inline HRESULT CObjNativeArray::SetObject2(int idx, IObject2 *pobj)
{
	CObjNative *p = (CObjNative *)pobj;
	SetAt(idx, p);
	return S_OK;
}

inline HRESULT CObjNativeArray::AddObject2(IObject2 *pobj)
{
	CObjNative *p = (CObjNative *)pobj;
	Add(p);
	return S_OK;
}

#endif //__ObNative_inl__
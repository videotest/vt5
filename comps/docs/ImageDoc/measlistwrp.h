#ifndef __measlistwrp_h__
#define __measlistwrp_h__

#ifndef SUPPORT_COMPOSITE
#define SUPPORT_COMPOSITE

class CSupportComposite
{
	int m_nPurpose;
	ICompositeObjectPtr m_ptrComp;
	bool m_bComp;
public:
	CSupportComposite(IUnknown* unk)
	{
		m_bComp = false;
		m_ptrComp = unk;

		if(m_ptrComp)
		{		
			long bComp;
			m_ptrComp->IsComposite(&bComp);
			if(!bComp)
			{
				m_ptrComp->SetPurpose(0);
				return;
			}
			m_bComp = bComp!=0;
			m_ptrComp->BuildTree(-1);
			m_ptrComp->GetPurpose(&m_nPurpose);
			m_ptrComp->SetPurpose(1);
		}
	};
	~CSupportComposite()
	{
		if(m_ptrComp) m_ptrComp->SetPurpose(m_nPurpose);
	};
	ICompositeObject* operator->()
	{
		return m_ptrComp;
	}
	bool IsComposite()
	{
		return m_bComp;
	}
	operator INamedDataObject2*()
	{
		INamedDataObject2Ptr ndo(m_ptrComp);
		return ndo;
	}

	
};
#endif //SUPPORT_COMPOSITE

class CMeasObjectWrp
{
public:
	CMeasObjectWrp( IUnknown *punkObject = 0 );
	void Attach( IUnknown *punkObject );
	~CMeasObjectWrp();
public:
	CRect	GetImageRect()				{return m_rectImage;}
	CImageWrp &GetImage()				{return m_image;}
	IUnknown*& operator =(IUnknown *punk)	{Attach( punk );}
	operator IUnknown*()		{return (IUnknown*)m_ptrObject;}

	long		get_class();
	CRect		GetTotalRect(IUnknown * punkView = 0);
public:
	CRect		m_rectTotal;
	CRect		m_rectImage;
	bool		m_bHasManualMeasurement;
	bool		m_bTotalRectCalculated;

	CImageWrp				m_image;
	ICalcObjectPtr			m_ptrCalc;
	INamedDataObject2Ptr	m_ptrObject;
	IMeasureObjectPtr		m_ptrMeas;
	GuidKey					m_nKey;
	long					m_class;
	int						m_nZOrder;
};

class CMeasObjListWrp : public CTypedPtrList<CPtrList, CMeasObjectWrp*>
{
public:
	CMeasObjListWrp( IUnknown *punk = 0 );
	~CMeasObjListWrp();

	void Attach( IUnknown *punk );
	void DeInit();

	inline void operator =(IUnknown*punk)	{Attach( punk );}
	operator IUnknown*()				{return (IUnknown*)m_ptrObjectList;}
public:
	POSITION	GetCurPosition();
	POSITION	GetFirstObjectPosition();
	IUnknown	*GetNextObject( POSITION &rPos );

	void	DoProcessNotify( const char *pszEvent, IUnknown *punkFrom, IUnknown *punkHit, long lHint );
	POSITION	FindObject( IUnknown *punkObject );
protected:
	void _Cache( IUnknown *punkO );
	void _UnCache( IUnknown *punkO );
	void _Update( IUnknown *punkO );
protected:
	INamedDataObject2Ptr	m_ptrObjectList;
	CMap<IUnknown*, IUnknown *&, POSITION, POSITION&>
							m_mapPtrToPos;
	CList<long> m_sorted_list;

	void _sort_list();
};

#endif //__measlistwrp_h__
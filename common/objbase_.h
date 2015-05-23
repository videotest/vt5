#ifndef __objbase_h__
#define __objbase_h__

#include "object5.h"
#include "data5.h"
#include "utils.h"

	
class std_dll CDrawObjectImpl  : public CImplBase
{
protected:
	CDrawObjectImpl();
	virtual ~CDrawObjectImpl();
protected:
	BEGIN_INTERFACE_PART(Draw, IDrawObject2)
		com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
		com_call GetRect( RECT *prect );
		com_call SetTargetWindow( IUnknown *punkTarget );
		com_call PaintEx(HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache, DRAWOPTIONS *pOptions);
		//com_call SetInvalidRect(RECT rect);
	END_INTERFACE_PART(Draw);
protected:	//overrideable
	virtual void DoDraw( CDC &dc );
	virtual CRect DoGetRect();
protected:
	virtual CRect GetTargetRect();

	virtual void _Draw( CDC *pdc = 0 );
protected:
	IUnknown			*m_punkDrawTarget;	//without AddRef
	CRect				m_rcInvalid;
	BITMAPINFOHEADER	*m_pbi;
	byte				*m_pdib;
	DRAWOPTIONS         *m_pOptions;

	CRect				m_rectAction;
};


/////////////////////////////////////////////////////////////////////////////////////////
//helper for implementation undo/redo records foe r one object
class std_dll CObjectUndoRecord
{
protected:
	GuidKey m_lParentKey;
	bool	m_bObjectIsVirtual;
	bool	m_bObjectIsAdded;
	bool	m_bMap;
	sptrINamedDataObject2	m_object;
	bool	m_bChangeDocData;
public:
	CObjectUndoRecord( IUnknown *punkObject, bool bObjectAdded, bool bMapDataToParent = false );
	void SetObject( IUnknown *punkObject );
public:
	void DoUndo( IUnknown *punkDocData );
	void DoRedo( IUnknown *punkDocData, bool  bCleanVirtualsBack=false );
protected:
	void AddObject( IUnknown *punkDocData );
	void RemoveObject( IUnknown *punkDocData, bool  bCleanVirtualsBack = false );
public:
	bool IsAdded() const			{return m_bObjectIsAdded;}
	IUnknown *GetObject() const		{return m_object;}
	bool IsDataChange() const		{return m_bChangeDocData;}
	bool IsVirtual() const			{return m_bObjectIsVirtual;}
};

/////////////////////////////////////////////////////////////////////////////////////////
//helper for store modified flag
class std_dll CModifiedUndoRecord
{
public:
	CModifiedUndoRecord();
public:
	void SetModified( IUnknown *punk, bool bSet = true );
	void ResetModified( IUnknown *punk );
protected:
	bool	m_bOldState;
	bool	m_bFirstCall;
};

#define UF_NOT_GENERATE_UNIQ_NAME	(1<<0)
#define UF_NOT_ACTIVATE_OBJECT		(1<<1)
#define UF_ACTIVATE_DEPENDENT       (1<<2)

/////////////////////////////////////////////////////////////////////////////////////////
//helper class for do/undo/redo changes in NamedData
class std_dll CObjectListUndoRecord : 
		public CTypedPtrList<CPtrList, CObjectUndoRecord*>
{
public:
	CObjectListUndoRecord();
	virtual ~CObjectListUndoRecord();

	void DeInit();

	bool m_bCleanVirtualsBack;

public:
	void SetToDocData( IUnknown *punkDocData, IUnknown *punkObject, DWORD dwFlags = 0 );		//performs adding object to the named data
	void RemoveFromDocData( IUnknown *punkDocData, IUnknown *punkObject, bool bMapData = true, bool bCleanVirtualsBack = false );	//preform removing object from named data

	void DoUndo( IUnknown *punkDocData );
	void DoRedo( IUnknown *punkDocData );
protected:
	CObjectUndoRecord *AddObject( IUnknown *punk, bool bObjectAdded, bool bMapData );

};



#endif // __objbase_h__
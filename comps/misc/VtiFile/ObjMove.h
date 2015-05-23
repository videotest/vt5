#ifndef __ObjMove_h__
#define __ObjMove_h__


#include "Object.h"

class CBaseDoc;


class CObjMoveEx : public CObjMove
{
	DECLARE_SERIAL(CObjMoveEx);
protected:
	C8Image	*m_pimgToMove;
	C8Image	*m_pimgToDraw;
	OVERLAY_KIND	m_overlay;
	BOOL	m_bDragging;
	CPoint	m_ptOffset;

public:	//const/destr.
	CObjMoveEx();
	~CObjMoveEx(){DeInit();};
public:	//interface
	virtual void AttachData( CObjNative & );
	virtual void Attach( CObjNative & );
	virtual void DeInit();
	virtual void DoFree();
	virtual void DoPaint( CDC &theDC, double iZoom, int iType );
	virtual void PaintFrameNext( CDC &theDC, double iZoom );
	virtual void AttachImg( void * );

	virtual void Serialize( CArchive &ar );
public:	
	void InitMoveImage( OVERLAY_KIND ok );
	void DeInitMoveImage();
	void Overlay();
	void Map( CBaseDoc *pDoc );
	void EndMove();

	BOOL IsReadyToDrag()
	{	return (m_pimg != 0) && (m_pimgToMove != 0 );}
	BOOL IsDraggingNow()
	{	return m_bDragging;}
	CPoint GetOffset()
	{	return m_ptOffset;}
	void SetOffset( CPoint pt )
	{	m_ptOffset = pt;}
	void SetMoveImage( C8Image *p )
	{	safedelete( m_pimgToMove );
		m_pimgToMove = p; }
	void SetDrawImage( C8Image *p )
	{	m_pimgToDraw = p; }
	void SetOverlay( OVERLAY_KIND ok )
	{	m_overlay = ok;}
	C8Image	*GetImgToDraw()
	{	return m_pimgToDraw;	}
};

#endif //__ObjMove_h__
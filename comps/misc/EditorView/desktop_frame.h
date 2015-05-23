#ifndef __desktop_frame_h__
#define __desktop_frame_h__


class DLL CDesktopFrame : public CAxBaseFrame
{
	DECLARE_DYNCREATE(CDesktopFrame);

	CDesktopFrame();
	/*
public:
	virtual void	Draw(CDC& dc, CRect rectPaint, BITMAPINFOHEADER *pbih, byte *pdibBits );
	virtual void	Redraw( bool bOnlyPoints = false, bool bUpdate = false );

	virtual void OnBeginDragDrop();
	virtual void OnEndDragDrop();

	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoEnterMode(CPoint point);
	void RedrawSelectRect();	
	*/

	virtual CRect ConvertToDeskTop( CRect rc, bool bCorrectOffset = true );

};


#endif __desktop_frame_h__

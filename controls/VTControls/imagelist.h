#ifndef __imagelist_h__
#define __imagelist_h__

class CImageListEx
{
	enum ShadowType	{Solid, Hatch};
	enum DrawFlag	{Normal, Transparent, Stretch, Left, Right, Top, Bottom, HCenter. VCenter};
public:
	CImageListEx()

	Create( int cx, int cy, int nBitmapsCount );
	Create( CImageList &imageList );
	Create( CBitmap &bitmap );

	virtual Serialize();

	Draw( dc, CRect rect, int nImage, drwFlags );
public:
	int		GetCount();
	CSize	GetSize();
public:
	void SetBkColor( COLORREF );
	
	virtual int Add( CBitmap &bitmap, bool bStretch = false );		//resize if required

	virtual HBITMAP	Get( int nPosition );	//for all operations
	virtual HIMAGELIST Get( int nPosition );
public:	//operations
	int	AddGrayBitmap( int nSourceIdx );
	int	AddShadowBitmap( int nSourceIdx, int nShadowOffser, ShadowType );
protected:
	void _Realloc( int nNewSize );
protected:

	CSize	m_size;	
	CBitmap	m_bitmap;
	int		m_nSizeGrow;
};

#endif //__imagelist_h__
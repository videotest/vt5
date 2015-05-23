#ifndef __image_h__
#define __image_h__

#include "clonedata.h"
#include "calibrint.h"
#include "PropBagImpl.h"

//the most important class in new version
//It implements image functionaly and gives IImage interface
class CImage : public CDataObjectBase,
			   public CClipboardImpl,
			   public CNamedPropBagImpl
		
{
	PROVIDE_GUID_INFO( )
	ENABLE_MULTYINTERFACE();
public:
	DECLARE_DYNCREATE(CImage);
	GUARD_DECLARE_OLECREATE(CImage);

	virtual LPUNKNOWN GetInterfaceHook(const void* p)
	{
		if (*(IID*)p == IID_INamedPropBag || *(IID*)p == IID_INamedPropBagSer)
			return (INamedPropBagSer*)this;
		else return __super::GetInterfaceHook(p);
	}

	static char *c_szType;
public:
	CImage();
	virtual ~CImage();
//overrided functions
	virtual void DeInit( bool bRemoveMask = true );		//de-initialize the image. Free the memory
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );	//store/restore the image to archive
	virtual void Detach();	//called when parent detached

//data exchange between parent and child
	virtual bool  ExchangeDataWithParent( DWORD flags );
	virtual DWORD GetNamedObjectFlags();	//return the data object flags
//creation
	bool _CreateNew( int cx, int cy, 
		const char *szColorConvertorName, int nPaneCount, 
		bool bGetCalibr = false  );					//create a new image with specified size and color ussage
	bool _CreateVirtual( CRect	rc, bool bGetCalibr = false );				//create a "virtual" image
	void _FreeImage();								//free the image
	color *_GetRow( int nRow, int nPane );			//return the image row
	byte *_GetRowMask( int nRow );					//return the mask of specified row
	void _SetOffset( CPoint pt, bool bMoveImage );	//return the image rectangle
	void _InitRowMasks();							//initialze the row masks

	void _InitContours();							//initialize the contour array, walk the image
	void _FreeContours();							//reset the contour array
	void _InsertContour( long nPos, Contour *p );	//insert contour to the specified location

	void _FreeContour( long nPos );					//free the specified contour

	HRESULT _AddPane(int nPane );
	HRESULT _RemovePane( int nPane );
	HRESULT _GetPanesCount(int *nSz );

	
	bool IsVirtual()
	{	return m_bVirtual;	}
	
	virtual bool CanBeBaseObject()
	{	return m_bVirtual ? false : true;	}


	virtual bool Copy(UINT nFormat, HANDLE* pHandle);
	virtual bool Paste(UINT nFormat, HANDLE Handle);
		

protected:
	DECLARE_DISPATCH_MAP();
	//class wizard support
	//{{AFX_DISPATCH(CImage)
	afx_msg long GetWidth();
	afx_msg long GetHeight();
	afx_msg void CreateNew(long cx, long cy, LPCTSTR szColor);
	afx_msg void CreateNewEx(long cx, long cy, LPCTSTR szColor, long lPaneNum);
	afx_msg long GetPosX();
	afx_msg long GetPosY();
	afx_msg void SetOffset(long cx, long cy, BOOL bMove);
	afx_msg BSTR GetColorSystem();
	afx_msg long GetColorsCount();
	afx_msg void GetCalibration(VARIANT FAR* varCalibr, VARIANT FAR* varGUID);
	afx_msg void SetCalibration( double fPixelPerMeter, LPCTSTR bstr_guid );
	afx_msg void CalcMinMaxBrightness();
	afx_msg long GetMinBrightness();
	afx_msg long GetMaxBrightness();
	afx_msg long GetAverageBrightness();
	afx_msg bool SetBagProperty( LPCTSTR, const VARIANT FAR& );
	afx_msg VARIANT GetBagProperty( LPCTSTR );
	//}}AFX_DISPATCH
	//{{AFX_VIRTUAL(CImage)
	//}}AFX_VIRTUAL
	//{{AFX_MSG(CImage)
	//}}AFX_MSG
	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(Image, IImage4)
	//IImage
		com_call CreateImage( int cx, int cy, BSTR bstrCCNamed, int nPaneNum );
		com_call FreeImage();
		com_call GetColorConvertor( IUnknown **ppunk );
		com_call GetRow( int nRow, int nPane, color **ppcolor );
		com_call GetSize( int *pnCX, int *pnCY );
	//IImage2
		com_call CreateVirtual( RECT rect );
		com_call InitRowMasks();
		com_call GetRowMask( int nRow, BYTE **ppByte );

		com_call GetOffset( POINT *pt );
		com_call SetOffset( POINT pt, BOOL bMoveImage );
		com_call GetImageFlags( DWORD *pdwFlags );
	//IImage3
		com_call InitContours();
		com_call FreeContours();

		com_call GetContoursCount( int *piCount );
		com_call GetContour( int nPos, Contour **ppContour );
		com_call SetContour( int nPos, Contour *pContour );
		com_call AddContour( Contour *pContour );

	// IImage4
		com_call AddPane(int nPane );
		com_call RemovePane( int nPane );
		com_call GetPanesCount(int *nSz );
	END_INTERFACE_PART(Image)

	BEGIN_INTERFACE_PART(CompatibleObject, ICompatibleObject3)
		com_call CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize );
		com_call Synchronize( );	
		com_call IsEqual( IUnknown *punkObjSource );	
	END_INTERFACE_PART(CompatibleObject)

	BEGIN_INTERFACE_PART(Info, IPaintInfo)
		com_call GetInvalidRect( RECT* pRect );
	END_INTERFACE_PART(Info);

	BEGIN_INTERFACE_PART(Calibr, ICalibr)
		com_call GetCalibration( double *pfPixelPerMeter, GUID *pguidC );
		com_call SetCalibration( double fPixelPerMeter, GUID *pguidC );
	END_INTERFACE_PART(Calibr);
	BEGIN_INTERFACE_PART(Rect, IRectObject)
		com_call SetRect(RECT rc);
		com_call GetRect(RECT* prc);
		com_call HitTest( POINT	point, long *plHitTest );
		com_call Move( POINT point );
	END_INTERFACE_PART(Rect);

protected:
	int		m_cx, m_cy;		//image sizes
	color	**m_ppRows;		//"virtual" pointers to the image rows
	byte	**m_ppRowMasks;	//"virtual" pointers to the image masks
	color	*m_pcolors;		//"real" pointer to the image data
	byte	*m_pbytes;		//"real" pointer to the image data
	CPtrArray	m_arrContours;	//the contour array
	CPoint		m_ptOffset;
	int		m_iPanesCount;	//cache the panes count value from IColorConvertor
	sptrIColorConvertor2	m_sptrCC2;	//image's Color Convertor
	bool	m_bVirtual;
	//calibration
	void InitCalibr(bool bForce = false);
	bool    m_bCalibrOk;
	double	m_fCalibr;
	GuidKey	m_guidC;
//Object clone
protected:
	virtual HRESULT CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize );	
	HRESULT Synchronize( );
	HRESULT IsEqualImage( IUnknown *punkSource );

	virtual bool IsObjectEmpty();

// For dispatches CalMinMaxBrigntness,...
protected:
	bool m_bMinMaxBriInit;
	byte m_byMinBri,m_byMaxBri,m_byAvrBri;
};

#endif // __image_h__

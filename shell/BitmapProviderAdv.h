#pragma once
#include "action5.h"
#include "\vt5\awin\misc_map.h"

inline long cmp_cstrings( CString s1, CString s2 )
{	return s1.Compare( s2 ); };

class CAdvancedBitmapProvider : public CCmdTargetEx,
								public CNumeredObjectImpl
{
	CBitmap		m_bmp;
	COLORREF	m_crback;

	struct 	XCmdInfo
	{
		DWORD	m_dwResCmd;		// resource identifier
		DWORD	m_dwDllCode;	// module code
        long	m_lContIdx;		// index in image container

		XCmdInfo() : m_dwResCmd(-1), m_dwDllCode(0), m_lContIdx(-1){}
	};

	static void XCmdInfo_free( XCmdInfo *pinfo ) { if( pinfo ) delete pinfo; }
    _list_map_t<XCmdInfo*, CString, cmp_cstrings, XCmdInfo_free> m_mapname2info;
    	
protected:
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();

public:
	CAdvancedBitmapProvider();
	~CAdvancedBitmapProvider();

    virtual void SetBackColor( COLORREF crBack );
	virtual COLORREF GetBackColor( );
	virtual bool Load( );		
	virtual long GetBitmapCount();
	virtual HANDLE GetBitmap( int idx );
	virtual long GetBmpIndexFromName( LPCTSTR lpstr_name, long *plBitmap );
	virtual bool CatchActionInfo( IUnknown *punkActionInfo );

protected:
	void	free();

protected:
	BEGIN_INTERFACE_PART(AdvBmps, IAdvancedBitmapProvider)
		// IBitmapProvider
		com_call GetBitmapCount( long *piCount );
		com_call GetBitmap( int idx, HANDLE *phBitmap );
		com_call GetBmpIndexFromCommand( UINT nCmd, DWORD dwDllCode, long *plIndex, long *plBitmap );		
		// IAdvancedBitmapProvider
		com_call SetBackColor( COLORREF crBack );
		com_call GetBackColor( COLORREF *pcrBack );
        com_call Load( );
		com_call GetBmpIndexFromName( BSTR bstrName, long *plIndex, long *plBitmap );
		com_call CatchActionInfo( IUnknown *punkActionInfo );	
	END_INTERFACE_PART(AdvBmps)
};


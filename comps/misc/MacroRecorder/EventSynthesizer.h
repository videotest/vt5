#pragma once

#include "ScriptGenerator.h"
#include "MacroRecorder_int.h"

class CMacroHelper;

///////////////////////////////////////////////////////////////////////////////////////////////
inline void Normalize( POINT *point )
{
	// convert point to normalized point
    if( !point )
		return;

	static int	cx_screen = ::GetSystemMetrics(SM_CXSCREEN), 
				cy_screen = ::GetSystemMetrics(SM_CYSCREEN);

	point->x = (long)( (point->x * 65536) / (double)(cx_screen) + 0.5 );
	point->y = (long)( (point->y * 65536) / (double)(cy_screen) + 0.5 );
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////
inline void	CorrectCursorPos( POINT point )
{
	POINT	pt_curr = {0};

	::GetCursorPos( &pt_curr );
	if( pt_curr.x != point.x || pt_curr.y != point.y )
	{
		//::SetCursorPos( point.x, point.y );
		Normalize( &point );
		::mouse_event( MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, point.x, point.y, 0, 0 );
	}
}

bool CompareClassNames(const char *pszClsName1, const char *pszClsName2);
bool DoEditRectangle(CEditedRectangleInfo &RInfo);


// WorkConditions struct
struct WorkConditions
{
	CString strWndClass;
	BOOL	bGoodData;
	_rect   rcWnd;
	HWND    hWndSaved;

	WorkConditions( ) { init(); }
	~WorkConditions( ) { strWndClass.Empty(); }

    WorkConditions( const WorkConditions &stcond ) 
	{
		init( );
		*this = stcond;
	}

	void init( )
	{
        strWndClass = _T("");
		rcWnd = _rect(0,0,0,0);
		hWndSaved = NULL;
		bGoodData = FALSE;
	}

	void from_point( POINT pt )
	{
		HWND hwnd = ::WindowFromPoint( pt );
		from_window( hwnd );
	}

	void from_window( HWND hwnd )
	{
        init( );

        strWndClass.Preallocate( MAX_PATH );
		::GetClassName( hwnd, strWndClass.GetBuffer( strWndClass.GetAllocLength( ) ), strWndClass.GetAllocLength( ) );
		strWndClass.ReleaseBuffer( );
		::GetWindowRect(hwnd, &rcWnd);
		hWndSaved = hwnd;
				
		bGoodData = TRUE;		        
	}

	void operator=( const WorkConditions &stcond )
	{
		strWndClass = stcond.strWndClass;
		bGoodData = stcond.bGoodData;
		rcWnd = stcond.rcWnd;
		hWndSaved = stcond.hWndSaved;
	}

	bool operator==( const WorkConditions &stcond )
	{
		if( bGoodData != stcond.bGoodData )
			return false;
		if( strWndClass != stcond.strWndClass )
			return false;

        return true;
	}

	bool operator!=( const WorkConditions &stcond )
	{
		return !operator==( stcond );
	}


};

// CEventSynthesizer class
class CEventSynthesizer
{
protected:
	DWORD	m_dwdelay ;

public:
	CEventSynthesizer(void);
	~CEventSynthesizer(void);

	void SetDelay( DWORD dwDelay ) { m_dwdelay  = dwDelay; }
	UINT GetDelay( ) { return m_dwdelay ; }

	// virtuals
	virtual BOOL	FireEvent( WorkConditions  *pcurr_cond, BOOL bplay_sound );
	virtual int	CanFireEvent( WorkConditions  *pcurr_cond, DWORD dwStartTime, long ltimeout );

protected:
	virtual BOOL DoFireEvent( WorkConditions  *pcurr_cond ){ return TRUE; }
	virtual BOOL DoCanFireEvent( WorkConditions curr_cond ) { return TRUE; }
	virtual ULONG GetSoundResourceID( ) { return (ULONG)(-1); }
};

// CMessageEventSynthesizer class
class CMessageEventSynthesizer : public CEventSynthesizer
{
protected:
	UINT	m_message;
	
public:
	CMessageEventSynthesizer(void);
	~CMessageEventSynthesizer(void);

	void SetMessage( UINT message ) { m_message = message; }
	UINT GetMessage( ) { return m_message; }

	virtual int	CanFireEvent( WorkConditions  *pcurr_cond, DWORD dwStartTime, long ltimeout );
};


// CMouseSynthesizer class
class CMouseSynthesizer : public CMessageEventSynthesizer
{
	POINT	m_point;	// in screen coordinaties
	
public:
	CMouseSynthesizer(void);
	~CMouseSynthesizer(void);

	void SetPoint( POINT pt ); 
	void SetPoint( int nX, int nY ); 
	POINT GetPoint( );

	virtual BOOL	FireEvent( WorkConditions  *pcurr_cond, BOOL bplay_sound );

 protected:   
    // virtuals override
    virtual BOOL DoCanFireEvent( WorkConditions curr_cond );
	virtual BOOL DoFireEvent( WorkConditions  *pcurr_cond );    
	virtual ULONG GetSoundResourceID( );

	bool	is_dblclick( ) { return m_message == WM_LBUTTONDBLCLK || m_message == WM_RBUTTONDBLCLK; }
};


// CKeybdSynthesizer class
class CKeybdSynthesizer : public CMessageEventSynthesizer
{
	long	m_lVk; // virtual-key code
	// [vanek] SBT: 818 - 24.02.2004
	BOOL	m_bextended_key; // extended key flag

public:
	CKeybdSynthesizer(void);
	~CKeybdSynthesizer(void);

	void SetKey( long lVk, BOOL bExtended = FALSE ); 
	long GetKey( );
	BOOL IsExtendedKey( );

protected:   
    // virtuals override
	virtual BOOL DoCanFireEvent( WorkConditions curr_cond );
	virtual BOOL DoFireEvent( WorkConditions  *pcurr_cond );    
	virtual ULONG GetSoundResourceID( );
};


// CConditionsSynthesizer class :)
class CConditionsSynthesizer : public CEventSynthesizer
{
	WorkConditions	m_stcond;
	CMacroHelper	*m_pMacroHelper;
	CCmdScriptGenerator *m_pScriptGenerator;

public:
	CConditionsSynthesizer(void);
	~CConditionsSynthesizer(void);

	void	SetConditions( WorkConditions stnew );
	BOOL	GetConditions( WorkConditions *pcond );

	void			SetHelper( CMacroHelper	*pMH );
    CMacroHelper	*GetHelper(void);

	void SetScriptGenerator(CCmdScriptGenerator *pScriptGenerator);

protected:   
    // virtuals override
	virtual BOOL DoCanFireEvent( WorkConditions curr_cond ); 
	virtual BOOL DoFireEvent( WorkConditions  *pcurr_cond );    
};


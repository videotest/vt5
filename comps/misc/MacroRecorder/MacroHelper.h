#pragma once

#include "MacroRecorder_int.h"
#include "idispatchimpl.h"
#include "impl_misc.h"
#include "EventSynthesizer.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\win_base.h"

#define WM_NCMOUSEFIRST WM_NCMOUSEMOVE
#define WM_NCMOUSELAST  WM_NCMBUTTONDBLCLK

inline void free_event_synth( void *p )
{
	delete (CEventSynthesizer*)p;
}

inline void free_sg( void *p )
{
	delete (CCmdScriptGenerator*)p;
}

inline void free_e_r_i( void *p )
{
	if (((CEditedRectangleInfo*)p)->bstrWndClass != 0)
		::SysFreeString(((CEditedRectangleInfo*)p)->bstrWndClass);
	delete (CEditedRectangleInfo*)p;
}

class CMacroHelper: public ComObjectBase,
					public CNamedObjectImpl,
					public IDispatchImpl< IMacroHelperDisp >,
					public IMacroHelper,
					public win_impl // for event synthesizing 
{
	//friend void CALLBACK MacroHelperTimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
	friend DWORD WINAPI MacroHelperThreadProc( LPVOID lpParameter ); 

	_list_t<CEventSynthesizer*, free_event_synth> m_list_pevents;
//	_list_t<CCmdScriptGenerator*, free_sg> m_list_pevents_new;
	_list_t<CEditedRectangleInfo*, free_e_r_i> m_edited_rectangles;
	WorkConditions	m_stcurr_cond;
    BOOL	m_bfiring_event,
			m_bneed_save_conditions,
			m_bskipped_previous_msg,	// флаг необработанности предыдущего сообщения
			m_bkill_all_events,
			m_bslowing_delay,
			m_bplay_sound;		// флаг проигрывания звука при синтезировании сообщений
	bool m_ballow_terminate;

	long	m_ltimeout;				// время между попытками (ms)
    double	m_fdelay_factor;		// коэффициент замедления воспроизведения макро-последовательности
	long	m_lmin_slowing_delay;	// минимальная задержка при замедлении (ms)

	DWORD	m_dwLastProcessedMessageTime;	// время последнего обработанного(записанного) сообщения 
	DWORD	m_dwFirstSkippedMessageDelay;	// временной интервал для первого необработанного сообщения
	DWORD	m_dwStartFiring;
	
	long	m_lScriptRunningAfterStart;		// флаг состояния "запуска из скрипта" перед стартом проигрывания 
											// макропоследовательности

	BOOL   m_bManualCorr;
	BOOL   m_bDoNotAddToScript; // Не добавлять в регенерируемый скрипт

	CRITICAL_SECTION m_crs_events;	 // используется для доступа к m_list_pevents

	//// поток, отвечающий за генерирование событий:
	HANDLE	m_hthread;				// дескриптор потока
	DWORD	m_dwthread_id;			// идентификатор потока
	BOOL volatile	m_bstop_thread;	// флаг завершения(останова) потока
	////

	HANDLE	m_hdo_synth;	// дескриптор события "Надо производить генерацию событий":
							// устанавливается при появлении события в списке событий m_list_pevents;
							// сбрасывается, когда список событий становиться пустым

	HANDLE	m_hprotect_mode;	// дескриптор события "Защищенный режим"

	long volatile	m_lposted_count;	// количество сообщений (WM_USER+777), посланных PostMessage

public: 
	CMacroHelper(void);
	virtual ~CMacroHelper(void);

	route_unknown( );
	virtual IUnknown *DoGetInterface( const IID &iid );

    // IMacroHelperDisp Interface
	com_call SetEventsWindow( /*[in]*/ BSTR bstrWndClass );
	// nX, nY - in screen coordinaties
	com_call MouseMove( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay );
	com_call LButtonDown( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay );
    com_call LButtonUp( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay );
    com_call LButtonDblClk( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay );
	com_call RButtonDown( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay );
	com_call RButtonUp( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay );
	com_call RButtonDblClk( /*[in]*/ int nX, /*[in]*/ int nY, /*[in]*/ DWORD dwDelay );
	com_call KeyDown( /*[in]*/ long lKeyCode, /*[in]*/ long lIsExtendedKey, /*[in]*/ DWORD dwDelay );
	com_call KeyUp( /*[in]*/ long lKeyCode, /*[in]*/ long lIsExtendedKey, /*[in]*/ DWORD dwDelay );	
	com_call WaitMacroEnd( );
	com_call SetScriptRunning( /*[in]*/ long lSet );	
    com_call SetEventsWindow2( /*[in]*/ BSTR bstrWndClass, /*[in]*/ int nX, /*[in]*/ int nY,
		/*[in]*/ int nCX, /*[in]*/ int nCY, /*[in]*/ long lHwnd );
	
	// IMacroHelper Interface
	com_call ConvertMessageToMacro( /*[in]*/ MSG *pMsg, /*[in]*/ BOOL bStartRecord, /*[out]*/ BSTR *pbstrScript );
	com_call KillAllEvents( );   // [vanek] SBT: 788 - 19.02.2004
	com_call GetManualCorrection(BOOL *pbManualCorr);
	com_call SetManualCorrection(BOOL bManualCorr);
	com_call ClearListOfExecuted();
	com_call FindEditedRectangle(CEditedRectangleInfo *pInfo);

	void	SetCurrCond( WorkConditions st_wc );
	BOOL	GetCurrCond( WorkConditions *pst_wc );

	void	AddChangedRect(CEditedRectangleInfo &RInfo);

protected:
	void	init_values_for_synth( );

	BOOL	is_mouse_msg( UINT message );
	BOOL	is_keybrd_msg( UINT message );

	BOOL	process_msg( MSG *pMsg, CString *pstrScript, DWORD dwDelay, BOOL *bSkipped );
	BOOL	process_mouse_msg( MSG *pMsg, CString *pstrScript, DWORD dwDelay, BOOL *bSkipped );
	BOOL	process_keybrd_msg( MSG *pMsg, CString *pstrScript, DWORD dwDelay, BOOL *bSkipped );

	BOOL	update_conditions( BOOL bUseForegroundWindow, int nX, int nY );

	void	on_after_first_msg( CString *pstr_script );
	void	on_start_firing(void);
	void	on_end_firing(void);

	// начало/окончание 
	void	start_synth(void);
	void	stop_synth(void);

	// находимся ли в режиме генерирования событий
	bool	is_synthesizing( )	{ return m_dwStartFiring != (DWORD)(-1); }

public:
	bool	have_events( );
	void	synth_events( bool bin_protected_mode = false );
	
// override  
public:
	virtual long handle_message( UINT m, WPARAM w, LPARAM l );
};

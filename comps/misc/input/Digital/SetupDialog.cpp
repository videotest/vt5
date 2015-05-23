// SetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SetupDialog.h"
#include "InputThread.h"
#include "GrabImg.h"
#include "mmsystem.h"
//#include "..\\common2\HelpUtils.h"
//#include "fgdrv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog dialog

extern int  
	iSkip, 
	iSkip2, 
	iFirstFrame, 
	iAll, 
	iAddr;
extern int
	ulNewCol, 
	ulNewColSrc, 
	ulNewRow, 
	ulFullRow;
extern int iDelay, iI2C0_zero,iI2C1_ampl, iAuto, iStep, iStartAutoH, iShutter, iSuper, iGamma ;
extern int iIntefaceSimple,iShowGammaWindow;
extern CPoint	ptScroll;
extern CSize	sizeImage;
extern int iLEVEL0;
extern int iLimitProcMIN, iLimitProcMAX;
extern float	fProcMAX, fProcMIN;

extern int imax, imin;
extern float A;
extern CCriticalSection	csMinMaxAccess;
extern void DAC_Write(char  DAC_num, char  data );
CRectTracker *crtFrame;
int iShowFrame=0;
int iL,iR,iT,iB, iChange=0;
RECT rc;
extern int Even( int  );
	   

int iAdr[6] = {0x300,0x308,0x310,0x318,0x320};

#define MIN_SIZE 240

void SetRect()
{
	crtFrame->m_rect.NormalizeRect();
	
	crtFrame->m_rect.IntersectRect  (crtFrame->m_rect,CRect(0,0,ulNewCol,ulFullRow-1));
	crtFrame->m_rect.left = min(crtFrame->m_rect.left, ulNewCol - MIN_SIZE);
	crtFrame->m_rect.UnionRect   (crtFrame->m_rect,CRect (crtFrame->m_rect.left,crtFrame->m_rect.top,
									crtFrame->m_rect.left+MIN_SIZE,crtFrame->m_rect.bottom));
	crtFrame->m_rect.IntersectRect  (crtFrame->m_rect,CRect(0,0,ulNewCol,ulFullRow-1));
	iL = Even(crtFrame->m_rect.left);
	crtFrame->m_rect.left =iL;
	iR = Even(crtFrame->m_rect.right);
	crtFrame->m_rect.right = iR;
	iT = Even(crtFrame->m_rect.top);
	crtFrame->m_rect.top = iT;
	iB = Even(crtFrame->m_rect.bottom);
	crtFrame->m_rect.bottom = iB;
	if (iB-iT < 4)
		iShowFrame = 0;
	iChange=0;
}

BITMAPINFOHEADER *CSetupDialog::m_pbi;


CSetupDialog::CSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupDialog)
	m_iFrames = iDelay;
	m_LimMin = 0;
	m_LimMax = 0;
	m_iZeroLevel = 0;
	m_bSuper = iSuper;
	m_bGamma = iGamma;
	//}}AFX_DATA_INIT
	m_iAmp =  iI2C0_zero;
	m_iZero = iI2C1_ampl;

	m_ptrd = 0;
	iShowFrame = 0;	

	m_bSomeChanged = FALSE;
	
	if (iDelay>1)  iShutter = 0;
	m_LimMin = iLimitProcMIN;
	m_LimMax = iLimitProcMAX;

	StoreDefVals();
}

CSetupDialog::~CSetupDialog()
{
	if( m_hEvt )
		::SetEvent( m_hEvt );
	::ResetEvent( m_ptrd->m_hEventStopper );
	::WaitForSingleObject( m_hEvt1, INFINITE );

	delete []m_pbi;
	m_pbi = NULL;

	::CloseHandle( m_hEvt );
	::CloseHandle( m_hEvt1 );

	delete crtFrame;
	crtFrame = NULL;
}


void CSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupDialog)
	DDX_Control(pDX, IDC_SHUTTER, m_comboShutter);
	DDX_Control(pDX, IDC_ADDR, m_comboAddr);
	DDX_Control(pDX, IDC_ZLEVEL, m_zlev);
	DDX_Control(pDX, IDC_FRAMES_SPIN, m_spinFrames);
	DDX_Control(pDX, IDC_AMP, m_amp);
	DDX_Text(pDX, IDC_FRAMES, m_iFrames);
	DDX_Text(pDX, IDC_EDIT_0, m_LimMin);
	DDV_MinMaxInt(pDX, m_LimMin, 0, 100);
	DDX_Text(pDX, IDC_EDIT_255, m_LimMax);
	DDV_MinMaxInt(pDX, m_LimMax, 0, 100);
	DDX_Check(pDX, IDC_SUPER, m_bSuper);
	DDX_Check(pDX, IDC_GAMMA, m_bGamma);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupDialog, CDialog)
	//{{AFX_MSG_MAP(CSetupDialog)
	ON_BN_CLICKED(IDC_TECH_SHOW_HIDE, OnTechShowHide)
	ON_CBN_SELCHANGE(IDC_ADDR, OnSomeChanged)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_TUNE, OnTune)
	ON_EN_CHANGE(IDC_EDIT_0, OnChangeEdit0)
	ON_EN_CHANGE(IDC_EDIT_255, OnChangeEdit255)
	ON_CBN_SELCHANGE(IDC_SHUTTER, OnSelchangeShutter)
	ON_BN_CLICKED(IDC_SUPER, OnSuper)
	ON_BN_CLICKED(IDC_INTERFACE_SIMPLE, OnInterfaceSimple)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_MEM, OnMem)
	ON_BN_CLICKED(IDC_SET_ZERO, OnSetZero)
	ON_BN_CLICKED(IDC_GAMMA, OnGamma)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONDOWN()
	ON_EN_CHANGE(IDC_COLS, OnSomeChanged)
	ON_EN_CHANGE(IDC_FRAMES, OnSomeChanged)
	ON_BN_CLICKED(IDC_INVERT, OnSomeChanged)
	ON_EN_CHANGE(IDC_ROWS, OnSomeChanged)
	ON_EN_CHANGE(IDC_SKIP1, OnSomeChanged)
	ON_EN_CHANGE(IDC_SKIP2, OnSomeChanged)
	ON_BN_CLICKED(IDC_FULL, OnSomeChanged)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER+102, OnSetupMinMax)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog message handlers

static bool bShowTechnical = false;

BOOL CSetupDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
//	m_View.Attach(WS_CHILD|WS_VISIBLE, this, IDC_IMAGE);
	m_spinFrames.SetBuddy( GetDlgItem( IDC_FRAMES ) );

	m_spinFrames.SetRange( 1, 200 );

	m_amp.SetRange( 0, 63 );
	for (int i=0;i<63;i+=10)
		m_amp.SetTic( i);

	m_zlev.SetRange( 0, 63 );
	for (i=0;i<63;i+=10)
		m_zlev.SetTic( i); 		

	InitVals();

	EnableControls();
	EnableInterface();
	CheckDlgButton(IDC_INTERFACE_SIMPLE,iIntefaceSimple);

	m_pbi = (BITMAPINFOHEADER *)new BYTE[1000*1000+1024+40];
	m_hEvt = ::CreateEvent( 0, true, false, 0 );
	m_hEvt1= ::CreateEvent( 0, true, false, 0 );

	m_ptrd = new CInputThread( m_pbi, (HANDLE)m_hWnd, m_hEvt, m_hEvt1 );
	m_ptrd->CreateThread();
	m_ptrd->SetThreadPriority( THREAD_PRIORITY_NORMAL );
	
	crtFrame = new CRectTracker;
	//crtFrame->m_rect  = CRect(100,100,200,200);
	crtFrame->m_nStyle = CRectTracker::resizeInside | CRectTracker::dottedLine ;
	crtFrame->m_nHandleSize = 8;
	crtFrame->m_sizeMin = 10;

	return true;
}

void CSetupDialog::OnTechShowHide() 
{
	bShowTechnical ^= true;

	EnableControls();
	
}

void CSetupDialog::EnableControls()
{
	int	iShowCmd = bShowTechnical ? SW_SHOW:SW_HIDE;

	GetDlgItem( IDC_STATIC_ADDR )->ShowWindow( iShowCmd );
	GetDlgItem( IDC_ADDR )->ShowWindow( iShowCmd );

	GetDlgItem( IDC_STATIC_OVERFLOW )->ShowWindow( iShowCmd );
	GetDlgItem( IDC_STATIC_0 )->ShowWindow( iShowCmd );
	GetDlgItem( IDC_STATIC_255 )->ShowWindow( iShowCmd );
	GetDlgItem( IDC_EDIT_0 )->ShowWindow( iShowCmd );
	GetDlgItem( IDC_EDIT_255 )->ShowWindow( iShowCmd );
	GetDlgItem( IDC_INTERFACE_SIMPLE )->ShowWindow( iShowCmd );
	GetDlgItem( IDC_STATIC_SETUP )->ShowWindow( iShowCmd   );
	GetDlgItem( IDC_SUPER )->ShowWindow( iShowCmd   );
	GetDlgItem( IDC_MEM )->ShowWindow( iShowCmd   );
	GetDlgItem( IDC_SET_ZERO )->ShowWindow( iShowCmd   );	
	GetDlgItem( IDC_GAMMA )->ShowWindow( iShowCmd  && iShowGammaWindow );		

	CString	str;

	if( bShowTechnical )
		str = "Установки <<";
	else
		str = "Установки >>";

	GetDlgItem( IDC_TECH_SHOW_HIDE )->SetWindowText( str );
}



void CSetupDialog::OnOK() 
{
	if( !UpdateData() )
		return;
	if( m_comboAddr.GetCurSel() == -1 )
		return;

	StoreVals();
	iShowFrame = 0;	
	CDialog::OnOK();
}



void CSetupDialog::StoreVals()
{
	if( !m_bSomeChanged )
		return;
	m_bSomeChanged = FALSE;

	if( m_comboAddr.GetCurSel() == -1 )
		return;

	if( !UpdateData() )
		return;

	InterlockedExchange( (LPLONG)&iDelay, m_iFrames );

	InterlockedExchange( (LPLONG)&iLimitProcMIN, m_LimMin );
	InterlockedExchange( (LPLONG)&iLimitProcMAX, m_LimMax );

	InterlockedExchange( (LPLONG)&iI2C0_zero, m_LimMin );
	InterlockedExchange( (LPLONG)&iI2C1_ampl, m_LimMax );

	InterlockedExchange( (LPLONG)&iAddr, m_iAdr );

	InterlockedExchange( (LPLONG)&iI2C0_zero, m_zlev.GetPos() );
	InterlockedExchange( (LPLONG)&iI2C1_ampl, m_amp.GetPos() );


	int isel = m_comboShutter.GetCurSel();
	InterlockedExchange( (LPLONG)&iShutter, isel);

	InterlockedExchange( (LPLONG)&iSuper, m_bSuper);	
	InterlockedExchange( (LPLONG)&iGamma, m_bGamma);	

}

void CSetupDialog::OnSomeChanged() 
{
	if( !m_ptrd )
		return;
	m_bSomeChanged = TRUE;
	if (m_iFrames>1) 
	{
		iShutter = 0;
		m_comboShutter.SetCurSel(0);
	}
	int	isel = m_comboAddr.GetCurSel();
	m_iAdr = iAdr[isel];
	StoreVals();
}

void CSetupDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	OnSomeChanged();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

long	g_storel[3];
int	g_storei[10];

void CSetupDialog::StoreDefVals()
{
	g_storel[0] = ulNewColSrc;
	g_storel[1]=  ulNewRow;
	g_storei[0] = iDelay;
	g_storei[1] = iSkip;
	g_storei[2] = iSkip2;
	g_storei[3] = iFirstFrame;
	g_storei[4] = iAddr;
	g_storei[5] = iI2C0_zero;
	g_storei[6] = iI2C1_ampl;
	g_storei[7] = iShutter;
}

void CSetupDialog::RestoreDefVals()
{
	InterlockedExchange( (LPLONG)&ulNewColSrc, g_storel[0] );
	InterlockedExchange( (LPLONG)&ulNewRow, g_storel[1] );
	InterlockedExchange( (LPLONG)&iDelay, g_storei[0] );
	InterlockedExchange( (LPLONG)&iSkip, g_storei[1] );
	InterlockedExchange( (LPLONG)&iSkip2, g_storei[2] );
	InterlockedExchange( (LPLONG)&iFirstFrame, g_storei[3] );
	InterlockedExchange( (LPLONG)&iAddr, g_storei[4] );
	InterlockedExchange( (LPLONG)&iI2C0_zero, g_storei[5] );
	InterlockedExchange( (LPLONG)&iI2C1_ampl, g_storei[6] );
	InterlockedExchange( (LPLONG)&iShutter, g_storei[7] );	

	InitVals();
	OnSomeChanged();	
}

void CSetupDialog::InitVals()
{
	m_zlev.SetPos( iI2C0_zero );
	m_amp.SetPos( iI2C1_ampl );

							   
	m_comboShutter.SetCurSel( iShutter );

	for (int i=0;i<5;i++)
		if( iAddr == iAdr[i]) 
			m_comboAddr.SetCurSel( i );


	m_iFrames = iDelay;
	m_iAmp =  iI2C0_zero;
	m_iZero = iI2C1_ampl;

	m_LimMin = iLimitProcMIN;
	m_LimMax = iLimitProcMAX;
}

void CSetupDialog::OnCancel() 
{
	RestoreDefVals();
	CDialog::OnCancel();
}

void CSetupDialog::BreakCureentFrame()
{
	if( !m_ptrd )
		return;
	::ResetEvent( m_ptrd->m_hEventStopper );
	::WaitForSingleObject( m_ptrd->m_hEventStopper, INFINITE );
}

#define START_STEP 23
#define START_STEP2 4
#define DIVIDER 2.72


inline bool TestKeyb( CWnd *pwnd )
{
	MSG msg;

	if( !PeekMessage( &msg, 
		pwnd->GetSafeHwnd(), 
		WM_KEYDOWN,
		WM_KEYDOWN,
		PM_REMOVE ) )
		return false;
	return msg.wParam == VK_ESCAPE;
}



extern CCriticalSection cs;

void CSetupDialog::OnTune() 
{
	BreakCureentFrame();

	CSingleLock lock( &cs , true);	

	GetDlgItem( IDC_TUNE )->SetWindowText( "ESC - стоп" );
	CWaitCursor	w;

	iStartAutoH = 0;                    
	iStep=START_STEP;
	iI2C0_zero=0;
	iI2C1_ampl=0;
	iShutter = 0; m_comboShutter.SetCurSel(0);

	bool bstop = false;

	m_zlev.SetPos( iI2C0_zero );
	m_amp.SetPos( iI2C1_ampl );

	iAuto = 10;
	int iFinish=0;

	while( !bstop )
	{
		::GrabImage( m_ptrd->m_pbi, m_ptrd->m_hEventStopper, 0/*FAST*/ );
		m_ptrd->PaintImage();

		if( TestKeyb( this ) )
			break;

		switch (iAuto)
		{
			
			case 10:
	            if (fProcMAX > iLimitProcMAX)
	            {
					if (iDelay>1) 
					{ 
						iDelay = m_iFrames = 1; 
						continue;  
					}
					
					iShutter++;
					if (iShutter >=7)
					{
	                 	iAuto = iStartAutoH = iI2C1_ampl = 0;
	                 	bstop = true;
						break;
					}
				}
				else
				{
					iI2C1_ampl=20;
					iAuto = 12;
				}
				
				break;
			case 12:
	            if (fProcMIN >= 0 && iShutter==0 && fProcMAX <= iLimitProcMAX)
					if (iDelay<=50)  
					{ 
						iDelay++;
						m_iFrames = iDelay;
					}
					else 
					{
						iAuto = iI2C1_ampl = 1;
						iDelay--;
						m_iFrames = iDelay;
					}

				else iAuto = iI2C1_ampl = 1;
				break;
			case 1:
	            if (fProcMAX<=iLimitProcMAX)
	            {
	                iStartAutoH = 1;                    
	                if ( iI2C1_ampl>=63 || iStep<3)	//iStep==0
	                {
						iAuto = 2;
	                 	iStartAutoH = 0;
	                 	iStep = START_STEP;
	                 	break;
	                }
	                iI2C1_ampl += iStep;
	                iI2C1_ampl = min(63,iI2C1_ampl);
	            }	
	            else
	            {
	                if (!iStartAutoH) 
	                {
	                	iAuto = iStartAutoH = 0;
	                	bstop = true;
						break;
	                }
	                if (iStep<3) 	//iStep==0
	                {
	                 	iStep = START_STEP2;
						iAuto = 2;
	                 	break;
	                }
	                iI2C1_ampl -= iStep;            	                	
	                iI2C0_zero = max(0,iI2C0_zero);
	                iStep = (int)(iStep/DIVIDER);
	            }
	
				break;
			case 2:   
	            if (fProcMIN<=iLimitProcMIN && iI2C1_ampl < 63)
	            {
	                iStartAutoH = 1;                    
	                if ( iI2C0_zero>=63 || iStep<3 )  //iStep==0
	                {
	                 	iAuto = 3;
	                 	iStartAutoH = 0;         
	                 	iStep = START_STEP2;       
	                }
	                iI2C0_zero += 5;
	                iI2C0_zero = min(63,iI2C0_zero);
	                if (fProcMAX<=iLimitProcMAX) 
	                {
	                 	iAuto = 3;
	                 	iStep = START_STEP2;       
	                 	break;
	                }         
	            }			  				
	            if ( iI2C1_ampl < 63)
	            {
                 	iAuto = 3;
                 	iStep = START_STEP2;       
                 	break;					
                }        
            	iAuto = iStartAutoH = 0;
            	break;					
			case 3:   
	            if (fProcMAX<=iLimitProcMAX)
	            {
	                iStartAutoH = 1;                    
	                if ( iI2C1_ampl>=63 || iStep<2)  //iStep==0
	                {
	                 	if (fProcMIN<=iLimitProcMIN && iI2C0_zero < 63)
	                 	{
	                 		iAuto = 2;
							break;
	                 	}
	                 	iAuto = iStartAutoH = 0;
	                 	bstop = true;
						break;
	                }
	                iI2C1_ampl += iStep;
	                iI2C1_ampl = min(63,iI2C1_ampl);
	            }	
	            else
	            {
	                if (iStep<2) //iStep==0
	                {
	                	iAuto = iStartAutoH = 0;
	                	bstop = true;
	                 	break;
	                }
	                if (!iStartAutoH ||  iI2C1_ampl>=63) 
	                {
	                	iAuto = iStartAutoH = 0;
	                	bstop = true;
						break;
	                }
	                iI2C1_ampl -= iStep;
					iI2C1_ampl = max(0,iI2C1_ampl);
	                iStep /= 2; 	                	
	            }
				break;					
		}
		m_zlev.SetPos( iI2C0_zero );
		m_amp.SetPos( iI2C1_ampl );
		m_comboShutter.SetCurSel(iShutter);
		m_iFrames = iDelay;
		
		char str[10];
		sprintf(str,"%d", iDelay );
		GetDlgItem( IDC_FRAMES )->SetWindowText( str );
		
		if (iFinish++ > 100)
		{
	        iAuto = iStartAutoH = 0;
	        bstop = true;
		}
		if( !iAuto )
			bstop = true;

		OnSetupMinMax( 0, 0 );
	}
	GetDlgItem( IDC_TUNE )->SetWindowText( "Настройка" );
}



void CSetupDialog::OnChangeEdit0() 
{
	if( !m_ptrd )
		return;
	m_bSomeChanged = TRUE;
	StoreVals();
}

void CSetupDialog::OnChangeEdit255() 
{
	if( !m_ptrd )
		return;
	m_bSomeChanged = TRUE;
	StoreVals();
}

LRESULT CSetupDialog::OnSetupMinMax( WPARAM, LPARAM )
{
	char str[10];
	static DWORD dwPrevTime=0;
	DWORD dwTime=0, dwCurrTime=0;

	csMinMaxAccess.Lock();

	
	sprintf(str,"%d", imin );
	GetDlgItem( IDC_VAL_MIN )->SetWindowText( str );
	sprintf(str,"%d", imax );
	GetDlgItem( IDC_VAL_MAX )->SetWindowText( str );
	sprintf(str,"%3.1f", A );
	GetDlgItem( IDC_VAL_AVER )->SetWindowText( str );

	sprintf(str,"%d", iI2C0_zero );
	GetDlgItem( IDC_STATIC_ZERO_LEVEL )->SetWindowText( str );
					  
	sprintf(str,"%d", iI2C1_ampl );
	GetDlgItem( IDC_STATIC_AMPL )->SetWindowText( str );

	dwTime = ::timeGetTime(); 
	dwCurrTime =  dwTime - dwPrevTime;
	sprintf(str,"%.3f", (float)dwCurrTime/1000 );
	GetDlgItem( IDC_TIMER )->SetWindowText( str );
	dwPrevTime = dwTime ;

	csMinMaxAccess.Unlock();

	return 0;
}


void CSetupDialog::OnSelchangeShutter() 
{
	if( !m_ptrd )
		return;
	m_bSomeChanged = TRUE;
	int	isel = m_comboShutter.GetCurSel();
	if (isel) 
	{
		iDelay = m_iFrames = 1;
		GetDlgItem( IDC_FRAMES )->SetWindowText( "1" );
	}

	StoreVals();
}

void CSetupDialog::OnSuper() 
{
	m_bSuper = IsDlgButtonChecked(IDC_SUPER);
	m_bSomeChanged = TRUE;
	OnSomeChanged();
}

void CSetupDialog::OnInterfaceSimple() 
{
	iIntefaceSimple ^= true;	
	EnableInterface();
}

void CSetupDialog::EnableInterface() 
{
	int iShowSimple = iIntefaceSimple ? SW_SHOW:SW_HIDE;

	GetDlgItem( IDC_KADROV )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_FRAMES )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_FRAMES_SPIN )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_ST_AMPL )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_STATIC_AMPL )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_AMP )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_ST_ZERO )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_STATIC_ZERO_LEVEL )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_ZLEVEL )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_STATIC_SHUTTER )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_SHUTTER )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_STATIC_MIN )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_STATIC_MAX )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_VAL_MIN )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_VAL_MAX )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_VAL_AVER )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_STATIC_AVER )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_ST_ZERO2 )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_ST_AMPL2 )->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_STATIC_TIMER)->ShowWindow( iShowSimple   );
	GetDlgItem( IDC_TIMER)->ShowWindow( iShowSimple   );
}


BOOL CSetupDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if( !pHelpInfo->iCtrlId /*||
		!pHelpInfo->dwContextId*/ )
		return false;

	int	arr[4];
	int	icount = 0;

	arr[icount++] = pHelpInfo->iCtrlId;
	arr[icount++] = pHelpInfo->dwContextId?pHelpInfo->dwContextId:pHelpInfo->iCtrlId|0x10000;
	arr[icount++] = 0;
	arr[icount++] = 0;


	CString	sHelpFileName = AfxGetApp()->m_pszHelpFilePath;
	if (!::WinHelp( (HWND)pHelpInfo->hItemHandle, "digital.hlp", HELP_WM_HELP, (DWORD)(void*)arr))
		AfxMessageBox(AFX_IDP_FAILED_TO_LAUNCH_HELP);
/*
	if (!::WinHelp( (HWND)pHelpInfo->hItemHandle, "digital.hlp", HELP_CONTEXT, (DWORD)(void*)arr))
		AfxMessageBox(AFX_IDP_FAILED_TO_LAUNCH_HELP);
  */
	
	return true;
}	

void CSetupDialog::OnMem() 
{
	char str[10];

	iLimitProcMIN = m_LimMin = (int)(fProcMIN+.5);
	sprintf(str,"%d", m_LimMin );
	GetDlgItem( IDC_EDIT_0)->SetWindowText( str );

	iLimitProcMAX = m_LimMax = (int)(fProcMAX+.5);
	sprintf(str,"%d", m_LimMax );
	GetDlgItem( IDC_EDIT_255 )->SetWindowText( str );


	//OnSomeChanged();
}

void CSetupDialog::OnSetZero() 
{
	char str[10];

	iLimitProcMIN = m_LimMin = 0;
	sprintf(str,"%d", m_LimMin );
	GetDlgItem( IDC_EDIT_0)->SetWindowText( str );

	iLimitProcMAX = m_LimMax = 0;
	sprintf(str,"%d", m_LimMax );
	GetDlgItem( IDC_EDIT_255 )->SetWindowText( str );
}

void CSetupDialog::OnGamma() 
{
	m_bGamma = IsDlgButtonChecked(IDC_GAMMA);
	m_bSomeChanged = TRUE;
	OnSomeChanged()	;
}

void CSetupDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
		
	BreakCureentFrame();

	CSingleLock lock( &cs , true);	

	if (iShowGammaWindow)
	{
		if (iShowFrame==0 || crtFrame->HitTest(point) == -1 )		
		{
			crtFrame->m_rect.left = point.x-4;
			crtFrame->m_rect.top = point.y-4;
			crtFrame->m_rect.right = point.x;
			crtFrame->m_rect.bottom  = point.y;
			iShowFrame = 1;
		}				 
		if (iShowFrame )
		{
			CWnd *cwndImage;
			cwndImage = GetDlgItem( IDC_IMAGE );
			crtFrame->Track(this, point, TRUE ,this);	
			if (abs(crtFrame->m_rect.right - crtFrame->m_rect.left) <10 ||
				abs(crtFrame->m_rect.top - crtFrame->m_rect.bottom) < 10)
				iShowFrame = 0;
		}
		if (iShowFrame)
			iChange=1;

		rc.left = ulNewCol;
		rc.right = ulNewCol+200;
		rc.top = 0;
		rc.bottom = 600;
		InvalidateRect(&rc);
		//Invalidate(TRUE);
		UpdateWindow();	
	}
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CSetupDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (iShowFrame && pWnd == this && crtFrame->SetCursor(this, nHitTest))
		return TRUE;                        
	
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CSetupDialog::OnRButtonDown(UINT nFlags, CPoint point) 
{
	iShowFrame = 0;	
	CDialog::OnRButtonDown(nFlags, point);
}


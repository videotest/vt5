// ColorBtnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vtcontrols2.h"
#include "ColorBtnDlg.h"
#include "VTColorPickerCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorBtnDlg dialog


// The color table, initialized to windows' 30 static colors
/*
COLORREF CColorBtnDlg::colors[30] =
{
    RGB(0,0,0),
    RGB(128,0,0),
    RGB(0,128,0),
    RGB(128,128,0),
    RGB(0,0,128),
    RGB(128,0,128),
    RGB(0,128,128),
    RGB(192,192,192),
    RGB(192,220,192),
    RGB(166,202,240),
    RGB(255,251,240),
    RGB(160,160,164),
    RGB(128,128,128),
    RGB(255,0,0),
    RGB(0,255,0),
    RGB(255,255,0),
    RGB(0,0,255),
    RGB(255,0,255),
    RGB(0,255,255),
    RGB(255,255,255),
	// -------------- Custom Colors ---------------
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255)
};
*/
// MRU table. See notes for Reset()


/////////////////////////////////////////////////////////////////////////////
// CColorBtnDlg dialog


CColorBtnDlg::CColorBtnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorBtnDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorBtnDlg)
	//}}AFX_DATA_INIT
	used[0]= 1;
    used[1]= 3;
    used[2]= 5;
    used[3]= 7;
    used[4]= 9;
    used[5]= 11;
    used[6]= 13;
    used[7]= 15;
    used[8]= 17;
    used[9]= 19;
    used[10]= 20;
    used[11]= 18;
    used[12]= 16;
    used[13]= 14;
    used[14]= 12;
    used[15]= 10;
    used[16]= 8;
    used[17]= 6;
    used[18]= 4;
    used[19]= 2;
	used[20]= 10;
	used[21]= 9;
	used[22]= 8;
	used[23]= 7;
	used[24]= 6;
	used[25]= 5;
	used[26]= 4;
	used[27]= 3;
	used[28]= 2;
	used[29]= 1;
	colorindex=0;
	m_bPalette = false;
	parent = 0;
}


void CColorBtnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorBtnDlg)
	DDX_Control(pDX, IDC_COLOR9, m_color9);
	DDX_Control(pDX, IDC_COLOR8, m_color8);
	DDX_Control(pDX, IDC_COLOR7, m_color7);
	DDX_Control(pDX, IDC_COLOR6, m_color6);
	DDX_Control(pDX, IDC_COLOR5, m_color5);
	DDX_Control(pDX, IDC_COLOR4, m_color4);
	DDX_Control(pDX, IDC_COLOR30, m_color30);
	DDX_Control(pDX, IDC_COLOR3, m_color3);
	DDX_Control(pDX, IDC_COLOR29, m_color29);
	DDX_Control(pDX, IDC_COLOR28, m_color28);
	DDX_Control(pDX, IDC_COLOR27, m_color27);
	DDX_Control(pDX, IDC_COLOR26, m_color26);
	DDX_Control(pDX, IDC_COLOR25, m_color25);
	DDX_Control(pDX, IDC_COLOR24, m_color24);
	DDX_Control(pDX, IDC_COLOR23, m_color23);
	DDX_Control(pDX, IDC_COLOR22, m_color22);
	DDX_Control(pDX, IDC_COLOR21, m_color21);
	DDX_Control(pDX, IDC_COLOR20, m_color20);
	DDX_Control(pDX, IDC_COLOR19, m_color19);
	DDX_Control(pDX, IDC_COLOR18, m_color18);
	DDX_Control(pDX, IDC_COLOR17, m_color17);
	DDX_Control(pDX, IDC_COLOR16, m_color16);
	DDX_Control(pDX, IDC_COLOR15, m_color15);
	DDX_Control(pDX, IDC_COLOR14, m_color14);
	DDX_Control(pDX, IDC_COLOR13, m_color13);
	DDX_Control(pDX, IDC_COLOR12, m_color12);
	DDX_Control(pDX, IDC_COLOR11, m_color11);
	DDX_Control(pDX, IDC_COLOR10, m_color10);
	DDX_Control(pDX, IDC_COLOR2, m_color2);
	DDX_Control(pDX, IDC_COLOR1, m_color1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorBtnDlg, CDialog)
	//{{AFX_MSG_MAP(CColorBtnDlg)
	ON_WM_LBUTTONDOWN()	
    ON_WM_DRAWITEM()	
    ON_BN_CLICKED(IDC_OTHER, OnOther)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
    ON_COMMAND_RANGE(IDC_COLOR1,IDC_COLOR30,OnColor)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CColorBtnDlg message handlers

BOOL CColorBtnDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    RECT r,r2;
	
	parent->GetWindowRect(&r);
	
   // Move the dialog to be below the button

    SetWindowPos(NULL,r.left,r.bottom,0,0,SWP_NOSIZE|SWP_NOZORDER);

    GetWindowRect(&r2);

	int binHeight = 43;

	if(((CVTColorPickerCtrl*)parent)->IsBinary())
	{
		SetWindowPos(NULL,0,0,r2.right-r2.left,binHeight,SWP_NOMOVE|SWP_NOZORDER);
		for(int i=0; i<10; i++)
		{
			HWND hwnd = (GetDlgItem(IDC_COLOR21+i))->GetSafeHwnd();
			::EnableWindow(hwnd,false);
			hwnd = GetDlgItem(IDC_OTHER)->GetSafeHwnd();
			::EnableWindow(hwnd,false);
		}
	}
	else
	{
		for(int i=0; i<10; i++)
		{
			HWND hwnd = (GetDlgItem(IDC_COLOR21+i))->GetSafeHwnd();
			::EnableWindow(hwnd,true);
			hwnd = GetDlgItem(IDC_OTHER)->GetSafeHwnd();
			::EnableWindow(hwnd,true);
		}
	}

	GetWindowRect(&r2);

    // Check to see if the dialog has a portion outside the
    // screen, if so, adjust.
    
    if (r2.bottom > GetSystemMetrics(SM_CYSCREEN))
    {   
        r2.top = r.top-(r2.bottom-r2.top);        
    }

    if (r2.right > GetSystemMetrics(SM_CXSCREEN))
    {
        r2.left = GetSystemMetrics(SM_CXSCREEN) - (r2.right-r2.left);
    }

    SetWindowPos(NULL,r2.left,r2.top,0,0,SWP_NOSIZE|SWP_NOZORDER);

    // Capture the mouse, this allows the dialog to close when
    // the user clicks outside.

    // Remember that the dialog has no "close" button.

    SetCapture();

	m_currentcustom=0;
	
	return TRUE; 
}



void CColorBtnDlg::EndDialog( int nResult )
{
    ReleaseCapture();

    CDialog::EndDialog(nResult);
}


void CColorBtnDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
    RECT r;

    POINT p;
    p.x = point.x;
    p.y = point.y;

    ClientToScreen(&p);

    GetWindowRect(&r);

    // The user clicked...

	

    if (!PtInRect(&r,p))
    {
        //  ...outside the dialog, close.

        EndDialog(IDCANCEL);
    }
    else
    {
        //  ...inside the dialog. Since this window
        //     has the mouse captured, its children
        //     get no messages. So, check to see
        //     if the click was in one of its children
        //     and tell him.

        //     If the user clicks inside the dialog
        //     but not on any of the controls,
        //     ChildWindowFromPoint returns a
        //     pointer to the dialog. In this
        //     case we do not resend the message
        //     (obviously) because it would cause
        //     a stack overflow.
        
        CWnd *child = ChildWindowFromPoint(point);

        if (child && child != this)
            child->SendMessage(WM_LBUTTONDOWN,0,0l);
    }
	
	CDialog::OnLButtonDown(nFlags, point);
}


void CColorBtnDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpd) 
{
	CDC dc;
    CPen nullpen;
    CBrush brush;
    CPen *oldpen;
    CBrush *oldbrush;

    // Draw the wells using the current color table

    nullpen.CreateStockObject(NULL_PEN);
	if (m_bPalette)
		brush.CreateSolidBrush(m_Palette[GetGValue(colors[nIDCtl-IDC_COLOR1])]);
	else
		brush.CreateSolidBrush(colors[nIDCtl-IDC_COLOR1]);

    dc.Attach(lpd->hDC);

    oldpen = dc.SelectObject(&nullpen);
    oldbrush = dc.SelectObject(&brush);

    lpd->rcItem.right++;
    lpd->rcItem.bottom++;

    dc.Rectangle(&lpd->rcItem);

    dc.SelectObject(oldpen);
    dc.SelectObject(oldbrush);

    dc.Detach();
	
	CDialog::OnDrawItem(nIDCtl, lpd);
}


void CColorBtnDlg::OnColor(UINT id)
{

    // A well has been clicked, set the color index
    // and close.

    colorindex = id-IDC_COLOR1;
    
    int i;

    // This color is now the MRU

    for (i=0;i<30;i++)
    {
        if (used[colorindex] > used[i])
        {
            used[i]++;
        }
    }

    used[colorindex] = 1;

    EndDialog(IDOK);

}

void CColorBtnDlg::OnOther() 
{
	int i;
	COLORREF newcolor;

    // The "Other" button.

    ReleaseCapture();

//	CBCGColorDialog dlg;
	CVTColorPickerCtrl *btn = (CVTColorPickerCtrl*) parent;

	dlg.m_bColor = !(btn->IsBlackAndWhite());
	if (!dlg.m_bColor)
		dlg.SetDrawPalette(m_bPalette?m_Palette:NULL);
	dlg.SetCurrentColor( btn->currentcolor );
	

 //   dlg.m_cc.Flags |= CC_FULLOPEN;


    if (dlg.DoModal() == IDOK)
    {
        // The user clicked OK.
        // set the color and close
        
        newcolor = dlg.GetColor();            

		//colors[20+m_currentcustom]=newcolor;
		//colorindex=m_currentcustom;
		//IncCustom();

        // Check to see if the selected color is
        // already in the table.

        colorindex = -1;

/*        for (i=20;i<30;i++)
        {
            if (colors[i] == newcolor)
            {
                colorindex = i;
				EndDialog(IDOK);
				return;
                
            }
        }*/

        // If the color was not found,
        // replace the LRU with this color
 
     if (colorindex == -1)
        {
         for (i=20;i<30;i++)
            {
                if (used[i] == 10)
                {
                    colors[i] = newcolor;
                    colorindex = i;                                 
                    break;
                }
            }
        }

        // This is the new MRU

        for (i=20;i<30;i++)
        {
            if (used[colorindex] > used[i])
            {
                used[i]++;
            }         
        }

        used[colorindex] = 1;

        EndDialog(IDOK);

        return;
    }

    // If the user clicked "Cancel" reclaim the mouse capture.
	EndDialog(IDCANCEL);


    //SetCapture();        	
}


void CColorBtnDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{

    // See notes for OnLButtonDown.
    
    CWnd *child = ChildWindowFromPoint(point,CWP_ALL);
    
    if (child && child != this)
        child->SendMessage(WM_LBUTTONDOWN,0,0l);	
	
	CDialog::OnLButtonUp(nFlags, point);
}


void CColorBtnDlg::IncCustom()
{
  if(m_currentcustom<9) m_currentcustom++;
  else m_currentcustom=0;
}


/////////////////////////////////////////////////////////////////////////////
// CColorBtnDlg message handlers

void CColorBtnDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWnd *child = ChildWindowFromPoint(point);

        if (child && child != this)
            child->SendMessage(WM_MOUSEMOVE,nFlags,MAKELPARAM(point.x,point.y));
	
	CDialog::OnMouseMove(nFlags, point);
}

void CColorBtnDlg::OnClose() 
{
  EndDialog(IDCANCEL);
}

void CColorBtnDlg::OnCancel()
{
  EndDialog(IDCANCEL);
}

void CColorBtnDlg::ResetColors()
{
	int i;
	m_bPalette = false;
	if(((CVTColorPickerCtrl*)parent)->IsBlackAndWhite())
	{
		if (((CVTColorPickerCtrl*)parent)->m_bPalette)
		{
			m_bPalette = true;
			memcpy(&m_Palette,&((CVTColorPickerCtrl*)parent)->m_Palette,sizeof(m_Palette));
		}
		for(i=0; i<20; i++)
			colors[i]=RGB(i*255/20,i*255/20,i*255/20);
		for(i=20; i<30; i++)
			colors[i]=RGB(GetGValue(colors[i]),GetGValue(colors[i]),GetGValue(colors[i]));
		((CVTColorPickerCtrl*)parent)->currentcolor=RGB(GetGValue(((CVTColorPickerCtrl*)parent)->currentcolor),GetGValue(((CVTColorPickerCtrl*)parent)->currentcolor),GetGValue(((CVTColorPickerCtrl*)parent)->currentcolor));
	}
	else
		{
			colors[0]= RGB(0,0,0);
			colors[1]= RGB(128,0,0);
			colors[2]= RGB(0,128,0);
			colors[3]= RGB(128,128,0);
			colors[4]= RGB(0,0,128);
			colors[5]= RGB(128,0,128);
			colors[6]= RGB(0,128,128);
			colors[7]= RGB(192,192,192);
			colors[8]= RGB(192,220,192);
			colors[9]= RGB(166,202,240);
			colors[10]= RGB(255,251,240);
			colors[11]= RGB(160,160,164);
			colors[12]= RGB(128,128,128);
			colors[13]= RGB(255,0,0);
			colors[14]= RGB(0,255,0);
			colors[15]= RGB(255,255,0);
			colors[16]= RGB(0,0,255);
			colors[17]= RGB(255,0,255);
			colors[18]= RGB(0,255,255);
			colors[19]= RGB(255,255,255);
		}
}

void CColorBtnDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_INACTIVE && ContinueModal())
		PostMessage(WM_CLOSE);
}

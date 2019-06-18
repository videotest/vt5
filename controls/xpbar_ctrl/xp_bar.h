#ifndef __xp_bar_h__
#define __xp_bar_h__
#include "commctrl.h"

#define XPBAR_CLASS	"xpbar"
#define XPSCROLL_CLASS "xpscroll"

#define XPF_TEXT		(1<<0)
#define XPF_IMAGE		(1<<1)
#define XPF_HEIGHT		(1<<2)
#define XPF_PARAM		(1<<3)
#define XPF_IMAGELIST	(1<<4)
#define XPF_STYLE		(1<<5)
#define XPF_IMAGELIST_CAPTION	(1<<6)

#define XPPS_UNKNOWN	0
#define XPPS_TOOLBAR	1
#define XPPS_STATUS		2
#define XPPS_WINDOW		3

#define XPPS_HILIGHT	(1<<8)

#define XPPS_STYLE_MASK	0x0000000f

struct XPPANEITEM
{
	unsigned	mask;
	char		*pszText;
	size_t		cchMaxText;
	int			nImage;
	int			nHeight;
	unsigned	lParam;
	HIMAGELIST	hImageList, hImageListCaption;
	unsigned	style;
};

#define XPBF_TEXT	(1<<0)
#define XPBF_IMAGE	(1<<1)
#define XPBF_STYLE	(1<<2)

#define XPBS_ENABLED	(1<<0)
#define XPBS_CHECKED	(1<<1)
#define XPBS_PRESSED	(1<<2)

struct XPBUTTON
{
	DWORD		mask;
	int			nImage;
	unsigned	command;
	char		*string;
	size_t		cchMax;
	unsigned	style;
};

struct XPINSERTITEM
{
	XPPANEITEM	item;
	long		insert_pos;	//insert_after
};

#define XPHT_CAPTION		1
#define XPHT_CLIENT			2
#define XPHT_SUBITEM		3
#define XPHT_CAPTIONBUTTON	4
#define XPHT_BUTTON			5

struct XPHITTEST
{
	unsigned	flags;

	RECT		rect_invalidate;
	bool		*pbhilight;

	long		lparam;
};

#define XPSM_HWND	(1<<1)
#define XPSM_GUID	(1<<2)
#define XPSM_IMAGE	(1<<3)
#define XPSM_TEXT	(1<<4)
#define XPSM_STYLE	(1<<5)
#define XPSM_ROWCOUNT	(1<<6)
#define XPSM_ID			(1<<7)
#define XPSM_INITREFCOUNT	(1<<8)


#define	XPSS_BOLD	(1<<0)

struct XP_STATUS
{
	DWORD	mask;
	HWND	hwnd;
	GUID	guid;
	HICON	hIcon;
	char	*pszText;
	int		cbMaxText;
	DWORD	dwStyle;
	int		nRows, nRefCounter;
	unsigned	nID;
	
};

#define XPCBM_IMAGE	(1<<0)
#define XPCBM_STATE	(1<<1)

struct	XP_CAPTION_BUTTON
{
	DWORD		mask;
	UINT		nCmd;
	int			iImage;
	DWORD		dwState;
};

struct XP_TIMER_PARAMS
{
	size_t	cb_size;
	int	stages_count, timer_speed;
};
	

///////////////////
//messages
#define	XPB_INSERTITEM		(WM_USER+1)
//l - XPINSERTITEM pointer, ret - lpos

#define XPB_EXPANDITEM		(WM_USER+2)	
//w - lpos, l = expand (0, 1, 2)

#define XPB_SETIMAGELIST	(WM_USER+3)
//l = himagelist

#define XPB_ADDBUTTON		(WM_USER+4)
//wparam - lpos,
//lparam - pointer to XPBUTTON

#define XPB_SETBUTTON		(WM_USER+5)

#define XPB_HITTEST			(WM_USER+6)
//wparam - lpos,
//lparam - pointer to XPHITTEST, may be 0

#define XPB_REMOVEITEM		(WM_USER+7)
//wparam - lpos

#define XPB_SETWINDOW		(WM_USER+8)
//wparam - lpos
//lparam -  HWND


#define XPB_ADDSTATUSPANE		(WM_USER+9)	
//wparam	lpos_status
//lparam	XP_STATUS ptr

#define XPB_SETSTATUSPANE		(WM_USER+10)	
//wparam	lpos_status
//lparam	XP_STATUS ptr

#define XBP_REMOVESTATUSPANE	(WM_USER+11)
//wparam	lpos_status
//lparam	GUID *

#define XBP_ADDCAPTIONBUTTON	(WM_USER+12)
//wparam	lpos_pane
//lparam	XP_CAPTION_BUTTON*

#define XBP_REMOVECAPTIONBUTTON	(WM_USER+13)
//wparam	lpos_pane
//lparam	id

#define XBP_SETCAPTIONBUTTON	(WM_USER+14)
//wparam	lpos_pane
//lparam	XP_CAPTION_BUTTON*

#define XBP_ENSUREVISIBLE		(WM_USER+15)
//wparam	lpos

#define XPB_GETDEFPARAMS		(WM_USER+16)
//always return width

#define XPB_GETITEM				(WM_USER+17)
//wParam - lpos
//l - pointer to XPPANEITEM

#define XPB_SETITEM				(WM_USER+18)
//wParam - lpos
//l - pointer to XPPANEITEM

#define XPB_GETFIRSTITEM		(WM_USER+19)
//

#define XPB_GETNEXTITEM			(WM_USER+20)
//wParam - lpos

#define XPB_GETFIRSTBUTTON		(WM_USER+21)
//w - lpos of item

#define XPB_GETNEXTBUTTON		(WM_USER+22)
//w - lpos of item
//l - lpos of button

#define XPB_GETBUTTONBYPOS		(WM_USER+23)
//w - lpos of button
//l - XPBUTTON ptr

#define XPB_SETBUTTONBYPOS		(WM_USER+24)
//w - lpos of button
//l - XPBUTTON ptr

#define XPB_SETTIMERPARAMS		(WM_USER+25)
//l- XP_TIMER_PARAMS ptr

////////////////////implementatiom


#endif //__xp_bar_h__
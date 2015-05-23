#ifndef __button_messages_h__
#define __button_messages_h__

#define IDAPPLY		542
#define WIZB_NEXT	543
#define	WIZB_BACK	544
#define WIZB_FINISH	545

//это сообщение должна посылать кнопка wizard своему parent
#define WM_SETSHEETBUTTON	(WM_USER+124)

//wparam - id кнопки

//lparam - флаг
#define SHEET_BUTTON_CREATED	0x00000000
#define SHEET_BUTTON_ENABLED	0x00000001
#define SHEET_BUTTON_DISABLED	0x00000002




#endif //__button_messages_h__

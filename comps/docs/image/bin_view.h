#ifndef __binview_h__
#define __binview_h__

#include "win_view.h"
#include "binimageint.h"


class CBinaryView : public CWinViewBase, 
				public _dyncreate_t<CBinaryView>
{
	route_unknown();
protected:
	virtual LRESULT		DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual void		DoAttachObjects();

	virtual LRESULT OnPaint();							//WM_PAINT
	virtual LRESULT OnLButtonDown( WPARAM, LPARAM );	//WM_PAINT
	virtual LRESULT OnEraseBackground( HDC hDC );		//WM_ERASEBKGND

	//interface overriden
	com_call GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch );

protected:
	IBinaryImagePtr	m_ptrBinary;
};

#endif //__binview_h__
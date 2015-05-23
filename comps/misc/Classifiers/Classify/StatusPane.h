#if !defined(__StatusPane_H__)
#define __StatusPane_H__

#include "impl_misc.h"
#include "win_main.h"

class CStatusPaneClassify : public ComObjectBase, public CWinImpl, public CNamedObjectImpl, public IStatusPane
{
	HFONT m_ft;
public:
	route_unknown();
	CStatusPaneClassify()
	{
		m_ft = NULL;
	}
	virtual IUnknown *DoGetInterface( const IID &iid );
	virtual void DoPaint();
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	com_call GetWidth( int *pWidth );
	com_call GetSide( DWORD *pdwSide );
};



#endif
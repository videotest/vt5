#ifndef __win_page_h__
#define __win_page_h__

#include "win_dlg.h"
#include "misc_classes.h"

class CPageImpl : public CDialogImpl, 
				public IOptionsPage
{
public:
	CPageImpl();
	virtual ~CPageImpl();

	virtual IUnknown	*DoGetDlgInterface( const IID &iid );
protected:
	com_call GetSize( SIZE *psize );
	com_call GetCaption( BSTR *pbstrCaption );
	com_call OnSetActive();
	com_call LoadSettings();
	com_call StoreSettings();

	virtual void UpdateControls(){}

	_size	m_size;
	_bstr_t	m_bstrCaption;
};


#endif	//__win_page_h__
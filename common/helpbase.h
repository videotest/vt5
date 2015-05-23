#ifndef __helpbase_h__
#define __helpbase_h__

#include "utils.h"
#include "misc5.h"

class std_dll CHelpInfoImpl : public CImplBase
{
public:
	CHelpInfoImpl();

	BEGIN_INTERFACE_PART_EXPORT(Help, IHelpInfo2)
		com_call GetHelpInfo( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags );
		com_call GetHelpInfo2( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags, BSTR *pbstrHelpPrefix );
	END_INTERFACE_PART(Help)

	virtual const char *GetHelpTopic();
protected:
	HINSTANCE	m_hHelpInstance;
};

std_dll void HelpDisplay( IUnknown *punkHelpInfo );

std_dll bool HelpRegisterWindow( const char *pszType, const char *pszCaption );
std_dll bool HelpLoadCollection( const char *pszWinType );
std_dll bool HelpDisplayPopup( const char *pszFileName, const char *pszPrefix, const char *pszName, HWND hwnd );
std_dll bool HelpDisplayTopic( const char *pszFileName, const char *pszPrefix, const char *pszName, const char *pszWinType );
std_dll bool HelpDisplay( const char *pszFileName );


#endif // __helpbase_h__
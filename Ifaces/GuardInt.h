#ifndef __guardint_h__
#define __guardint_h__

#include "Defs.h"

interface IVTApplication : public IUnknown
{
	com_call InitApp( BYTE* pBuf, DWORD dwData ) = 0;
	// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
	com_call GetData(DWORD * pKeyGUID, BYTE ** pTable, BSTR * pbstrSuffix, DWORD * pdwImito) = 0;
	// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
	com_call SetData(DWORD * pKeyGUID, BYTE ** pTable, BSTR * pbstrSuffix, DWORD * pdwImito) = 0;
	// get mode (register or not)
	com_call GetMode(BOOL * pbRegister) = 0;

	// Add guid to table
	com_call AddEntry(DWORD * pIntGUID, DWORD * pExtGUID, BSTR bstrIntProgID, DWORD dwData) = 0;
	// Remove guid from table
	com_call RemoveEntry(DWORD * pExtGUID, BSTR bstrIntProgID) = 0;
	// get inner guid from table by extern guid	: mode 0
	// get extern guid by inner guid			: mode 1
	com_call GetEntry(DWORD dwMode, DWORD * pSrcGUID, DWORD * pDstGUID, DWORD * pdwData) = 0;

	// Get External GUID from internal/external ProgID : mode 0
	// Get internal GUID from internal/external ProgID : mode 1
	com_call GetModeData(DWORD dwMode, DWORD * pGUID, BSTR bstrProgID) = 0;
	// translations	
	// get external ProgID from internal ProgID : mode 0
	// get internal ProgID from external ProgID : mode 1
	com_call GetDataValue(DWORD dwMode, BSTR bstrSrcProgID, BSTR * pbstrDstProgID) = 0;

	com_call GetNames(BSTR * pbstrAppName, BSTR * pbstrCompanyName) = 0;
	com_call GetRegKey(BSTR * pbstrRegKey) = 0;
};

interface IVTClass : public IUnknown
{
	com_call VTCreateObject(LPUNKNOWN, LPUNKNOWN, REFIID, BSTR, LPVOID*) = 0;
	com_call VTRegister(BOOL bRegister) = 0;
	com_call VTGetProgID(BSTR * pbstrProgID) = 0;
};

declare_interface(IVTApplication, "96D0A52E-117C-4e3a-A217-6CEF5E2347F9");
declare_interface(IVTClass,		  "63610F00-66D5-4b60-95DD-40DA4B675384");

#endif //__guardint_h__



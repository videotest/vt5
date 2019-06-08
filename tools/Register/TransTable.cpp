// TransTable.cpp: implementation of the CTranslateTable class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#ifdef _DEBUG
#define new new
#endif
#include "GuardFunc.h"
#include "NameConsts.h"

#include "TransTable.h"
#include "GuardInt.h"

#pragma warning (disable : 4786)

extern bool bReportErrors;

void WriteLogLine(const char *lpszFormat, ...);

// таблица перестановок
static BYTE STable[128] = 
{
	0x07,	0x08,	0x0d,	0x0f,	0x0c,	0x0b,	0x0e,	0x03,	0x0a,	0x09,	0x00,	0x02,	0x05,	0x06,	0x01,	0x04,
	0x0f,	0x06,	0x07,	0x0d,	0x09,	0x0e,	0x08,	0x05,	0x02,	0x01,	0x03,	0x0b,	0x0c,	0x0a,	0x00,	0x04,
	0x04,	0x09,	0x05,	0x01,	0x0f,	0x0a,	0x03,	0x0b,	0x00,	0x0e,	0x0c,	0x06,	0x08,	0x07,	0x02,	0x0d,
	0x01,	0x0b,	0x0a,	0x09,	0x06,	0x03,	0x00,	0x08,	0x05,	0x0d,	0x0f,	0x07,	0x0e,	0x02,	0x0c,	0x04,
	0x0f,	0x04,	0x03,	0x05,	0x0a,	0x01,	0x08,	0x07,	0x0b,	0x02,	0x00,	0x0e,	0x09,	0x0d,	0x06,	0x0c,
	0x08,	0x0c,	0x03,	0x0f,	0x06,	0x04,	0x0b,	0x02,	0x05,	0x0a,	0x00,	0x0e,	0x07,	0x0d,	0x01,	0x09,
	0x06,	0x0c,	0x0d,	0x0e,	0x00,	0x0b,	0x05,	0x0f,	0x08,	0x02,	0x03,	0x0a,	0x04,	0x07,	0x01,	0x09,
	0x04,	0x09,	0x06,	0x01,	0x0e,	0x03,	0x0d,	0x0b,	0x00,	0x05,	0x02,	0x08,	0x0a,	0x0c,	0x0f,	0x07
};


const char * szGUARDFILESIG = "VTG1";

//////////////////////////////////////////////////////////////////////
// return entry size
inline long GetEntrySize(TTranslateEntry * pEntry)
{
	if (!pEntry)
		return 0;
	return (sizeof(GUID) << 1) + sizeof(DWORD) + lstrlen(pEntry->szProgID) + 1 + sizeof(int);
}

// load entry content from buffer
inline void EntryFromBuffer(TTranslateEntry * pEntry, BYTE ** ppBuffer)
{
	if (ppBuffer && *ppBuffer && pEntry)
	{
		BYTE * pBufPtr = *ppBuffer;
	// extern guid
		// copy guid
		memcpy((void*)&(pEntry->GuidExtern), (void*)pBufPtr, sizeof(GUID));
		// and advance buffer ptr
		pBufPtr += sizeof(GUID); 

	// string ProgID
		// delete prev string (if any)
		if (pEntry->szProgID)
			delete [] pEntry->szProgID, pEntry->szProgID = 0;
	
		// read progID string size
		int nSize = *((long*)pBufPtr); 
		// and advance ptr;
		pBufPtr += sizeof(int);

		// if size of string is positive value
		if (nSize > 1)
		{
			// alloc memore to string
			pEntry->szProgID = new TCHAR [nSize];
			// copy string from buffer
			memcpy((void*)pEntry->szProgID, (void*)pBufPtr, nSize);
			// and advance buffer ptr
			pBufPtr += nSize;
		}
	// inner guid
		// copy guid
		memcpy((void*)&(pEntry->GuidInner), (void*)pBufPtr, sizeof(GUID));
		// and advance buffer ptr
		pBufPtr += sizeof(GUID);

	// dword data
		// copy dword
		memcpy((void*)&(pEntry->dwData), (void*)pBufPtr, sizeof(DWORD));
		// and advance buffer ptr
		pBufPtr += sizeof(DWORD);
		*ppBuffer = pBufPtr;
	}
}

// save entry content to buffer
inline void EntryToBuffer(TTranslateEntry * pEntry, BYTE ** ppBuffer)
{
	if (ppBuffer && *ppBuffer && pEntry)
	{
	// extern guid
		BYTE * pBufPtr = *ppBuffer;
		// copy extern guid
		memcpy((void*)pBufPtr, (void*)&(pEntry->GuidExtern), sizeof(GUID));
		// and advance buffer ptr
		pBufPtr += sizeof(GUID);

	// string progID
		// get string size
		int nSize = lstrlen(pEntry->szProgID) + 1;
		// save it to buffer
		*((int*)pBufPtr) = nSize;
		// and advance buffer ptr
		pBufPtr += sizeof(int);

		// copy ProgID string 
		memcpy((void*)pBufPtr, pEntry->szProgID, nSize);
		// and advance buffer ptr
		pBufPtr += nSize;

	// inner guid
		memcpy((void*)pBufPtr, (void*)&(pEntry->GuidInner), sizeof(GUID));
		// and advance buffer ptr
		pBufPtr += sizeof(GUID);

	// dword
		memcpy((void*)pBufPtr, (void*)&(pEntry->dwData), sizeof(DWORD));
		pBufPtr += sizeof(DWORD);
		*ppBuffer = pBufPtr;
	}
}



//////////////////////////////////////////////////////////////////////
// Translate entry structure
TTranslateEntry::TTranslateEntry(GuidKey & guidExt, GuidKey & guidInn, LPCTSTR szString, DWORD dw)
{
	GuidExtern = guidExt;
	GuidInner = guidInn;
	dwData = dw;
	long lSize = szString ? lstrlen(szString) : 0;
	szProgID = lSize ? new TCHAR [lSize + 1] : 0;
	if (szString)
		lstrcpy(szProgID, szString);
}

TTranslateEntry::~TTranslateEntry()
{
	delete [] szProgID;
}



//////////////////////////////////////////////////////////////////////
// Translate table class

CTranslateTable::CTranslateTable()//LPCTSTR szFile, GuidKey & key, DWORD dwImito)
{
//	m_szFile = 0;
	m_dwImito = 0;
//	SetKey(key, dwImito);
//	Load(szFile);
}

CTranslateTable::~CTranslateTable()
{
//	if (m_szFile)
//		delete [] m_szFile, m_szFile = 0;

	Free();
}

void CTranslateTable::SetKey(GuidKey & rKey, DWORD dwImito)
{
	m_GuidKey = rKey;
	m_dwImito = dwImito ? dwImito : m_dwImito;
	if (m_GuidKey != INVALID_KEY)
		m_CryptEngine.SetKey((BYTE*)&m_GuidKey, sizeof(GUID));
}


void CTranslateTable::Free()
{
	m_mapGuid.clear();
	m_mapIntGuid.clear();
	m_mapProgID.clear();
	
	CEntryList::iterator le = m_listEntries.end();
	for (CEntryList::iterator lp = m_listEntries.begin(); lp != le; lp++)
		delete *lp;

	m_listEntries.clear();
}

GuidKey CTranslateTable::Get(GUID & External)
{
	CGuidMap::iterator fi = m_mapGuid.size() ? m_mapGuid.find(External) : m_mapGuid.end();
	return DecryptEntry(fi != m_mapGuid.end() ? fi->second : 0);
}

GuidKey CTranslateTable::Get(LPCTSTR szProgID)
{
	CProgIDMap::iterator fi = m_mapProgID.size() ? m_mapProgID.find(szProgID) : m_mapProgID.end();
	return DecryptEntry(fi != m_mapProgID.end() ? fi->second : 0);
}

GuidKey CTranslateTable::GetExtern(LPCTSTR szProgID)
{
	CProgIDMap::iterator fi = m_mapProgID.size() ? m_mapProgID.find(szProgID) : m_mapProgID.end();
	return (fi != m_mapProgID.end() && fi->second) ? fi->second->GuidExtern : INVALID_KEY;
}

GuidKey CTranslateTable::GetExtern(GUID & Internal)
{
	CGuidMap::iterator fi = m_mapIntGuid.size() ? m_mapIntGuid.find(Internal) : m_mapIntGuid.end();
	return (fi != m_mapIntGuid.end() && fi->second) ? fi->second->GuidExtern : INVALID_KEY;
}

DWORD CTranslateTable::GetData(GUID & External)
{
	CGuidMap::iterator fi = m_mapGuid.size() ? m_mapGuid.find(External) : m_mapGuid.end();
	return (fi != m_mapGuid.end() && fi->second) ? fi->second->dwData : (DWORD)-1;
}

DWORD CTranslateTable::GetData(LPCTSTR szProgID)
{
	CProgIDMap::iterator fi = m_mapProgID.size() ? m_mapProgID.find(szProgID) : m_mapProgID.end();
	return (fi != m_mapProgID.end() && fi->second) ? fi->second->dwData : (DWORD)-1;
}

bool CTranslateTable::Remove(TTranslateEntry * pEntry)
{
	if (!pEntry)
		return false;

	m_mapGuid.erase(pEntry->GuidExtern);
	m_mapIntGuid.erase(DecryptEntry(pEntry));
	m_mapProgID.erase(pEntry->szProgID);
	m_listEntries.remove(pEntry);
	delete 	pEntry;
	return true;
}

TTranslateEntry * CTranslateTable::GetEntry(LPCTSTR szProgID)
{
	CProgIDMap::iterator fi = m_mapProgID.size() ? m_mapProgID.find(szProgID) : m_mapProgID.end();
	return fi == m_mapProgID.end() ? 0 : fi->second;
}

TTranslateEntry * CTranslateTable::GetEntry(GUID & External)
{
	CGuidMap::iterator fi = m_mapGuid.size() ? m_mapGuid.find(External) : m_mapGuid.end();
	return fi == m_mapGuid.end() ? 0 : fi->second;
}

bool CTranslateTable::Save(LPCTSTR szFile)
{
	if (lstrlen(szFile) && lstrcmp(m_strFileName, szFile))
		m_strFileName = szFile;

	if (m_strFileName.IsEmpty() || m_GuidKey == INVALID_KEY)
		return false;

	// get functions
	HMODULE hModule = GetModuleHandle(0);
	if (!hModule)
		return false;

	typedef void (*pfnGUARDGETNSKINFO)(DWORD*);
	typedef void (*pfnGUARDGETAPPNAME)(char*, LONG*);
	typedef void (*pfnGUARDGETCOMPANYNAME)(char*, LONG*);

	pfnGUARDGETNSKINFO pfnGetNSKInfo = (pfnGUARDGETNSKINFO)GetProcAddress(hModule, "GuardGetNSKInfo");
	pfnGUARDGETAPPNAME pfnGetAppName = (pfnGUARDGETAPPNAME)GetProcAddress(hModule, "GuardGetAppName");
	pfnGUARDGETCOMPANYNAME pfnGetCompanyName = (pfnGUARDGETCOMPANYNAME)GetProcAddress(hModule, "GuardGetCompanyName");
	pfnGUARDGETAPPNAME pfnGetSuffix = (pfnGUARDGETAPPNAME)GetProcAddress(hModule, "GuardGetSuffix");

	if (!pfnGetNSKInfo || !pfnGetAppName || !pfnGetCompanyName)
		return false;

	// get size of array
	DWORD dwSize = 0;
	CEntryList::iterator le = m_listEntries.end();
	CEntryList::iterator lp = m_listEntries.begin();
	for (lp = m_listEntries.begin(); lp != le; lp++)
		dwSize += (DWORD)::GetEntrySize(*lp);
	
	if (!dwSize)
	{
		GuardSetErrorCode(guardInvalidTableSize);
		return false;
	}

	BYTE * pBuffer = 0;

	try 
	{
		pBuffer = new BYTE [dwSize];
		BYTE * pBufPtr = pBuffer;
		bool bRet = true;
		if (!pBuffer)
		{
			GuardSetErrorCode(guardMemory);
			return false;
		}
		// save entries to array
		for (lp = m_listEntries.begin(); lp != le && pBufPtr < (pBuffer + dwSize); lp++)
			::EntryToBuffer(*lp, &pBufPtr);

//		pBufPtr = 0;

		// crypt file
		if (!m_CryptEngine.Crypt(pBuffer, dwSize, m_dwImito))
		{
			if (pBuffer)
				delete [] pBuffer, pBuffer = 0;
			GuardSetErrorCode(guardDecryptFile);
//			return false;
		}

		// now we have buffer that contains crypted contents of translation_table
		// we need generate index
		srand(m_dwImito); // set seed dependent from imito
		int nIndex = rand() % dwSize;

		// be sure index < buffer size
		while (nIndex == dwSize || nIndex == 0)
			nIndex = rand() % dwSize;

	// create file
		CFile file(m_strFileName, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite);

		// save signature
		file.Write((LPVOID)szGUARDFILESIG, lstrlen(szGUARDFILESIG));

		// Get app_name
		LONG lNameSize = 0;
		pfnGetAppName(0, &lNameSize);

		// save count of app_name_string
		file.Write((LPVOID)&lNameSize, sizeof(LONG));
		if (lNameSize)
		{
			CString strAppName(DEF_APP_NAME);
			pfnGetAppName(strAppName.GetBuffer(lNameSize + 1), &lNameSize);
			strAppName.ReleaseBuffer();
			if (strAppName.GetLength() > lNameSize)
				strAppName.SetAt(lNameSize, '\0');

			// save app_name_string
			file.Write(((LPVOID)(LPCSTR)strAppName), lNameSize);
		}

		// get NSK int 
		DWORD dwKeyId = 0;
		pfnGetNSKInfo(&dwKeyId);

		// save NSK info
		file.Write((LPVOID)&dwKeyId, sizeof(DWORD));

		// read size of app_suffix string
		LONG lSufSize = 0;
		pfnGetSuffix(0, &lSufSize);

		// save count of app_suffix_string
		file.Write((LPVOID)&lSufSize, sizeof(LONG));
		if (lSufSize)
		{
			CString strSuf;
			pfnGetCompanyName(strSuf.GetBuffer(lSufSize + 1), &lSufSize);
			strSuf.ReleaseBuffer();
			if (strSuf.GetLength() > lSufSize)
				strSuf.SetAt(lSufSize, '\0');

			// save app_suffix_string
			file.Write(((LPVOID)(LPCSTR)strSuf), lSufSize);
		}
	
		// save index
		file.Write((LPVOID)&nIndex, sizeof(int));

		// save first part fo buffer
		file.Write((LPVOID)pBuffer, nIndex);

		// save imito
		file.Write((LPVOID)&m_dwImito, sizeof(DWORD));

		// save last part of buffer
		file.Write((LPVOID)&(pBuffer[nIndex]), dwSize - nIndex);

		if (pBuffer)
			delete [] pBuffer, pBuffer = 0;
	}
	catch(CFileException * e)
	{
		CString strError;
		e->GetErrorMessage(strError.GetBuffer(1024), 1024);
		TRACE ("%s\n", strError);

		switch (e->m_cause)
		{
		case CFileException::tooManyOpenFiles:

		case CFileException::genericException:
		case CFileException::hardIO:
			GuardSetErrorCode(guardInvalidGuardFile);
			break;

		case CFileException::fileNotFound:
		case CFileException::badPath:
		case CFileException::invalidFile:
			GuardSetErrorCode(guardInvalidGuardFile);
			break;

		case CFileException::accessDenied:
		case CFileException::sharingViolation:
		case CFileException::lockViolation:
		case CFileException::badSeek:
		case CFileException::endOfFile:
			GuardSetErrorCode(guardWriteGuardFile);
			break;

		case CFileException::directoryFull:
		case CFileException::diskFull:
			GuardSetErrorCode(guardWriteGuardFile);
			break;
		}
		ASSERT(e->ReportError()== IDOK);
		e->Delete();
		if (pBuffer)
			delete [] pBuffer, pBuffer = 0;
	}
	return true;
}

bool CTranslateTable::SaveText(LPCTSTR szFile)
{
	if (lstrlen(szFile) && lstrcmp(m_strFileName, szFile))
		m_strFileName = szFile;

	if (m_strFileName.IsEmpty() || m_GuidKey == INVALID_KEY)
		return false;

	// get functions
	HMODULE hModule = GetModuleHandle(0);
	if (!hModule)
		return false;

	typedef void (*pfnGUARDGETNSKINFO)(DWORD*);
	typedef void (*pfnGUARDGETAPPNAME)(char*, LONG*);
	typedef void (*pfnGUARDGETCOMPANYNAME)(char*, LONG*);

	pfnGUARDGETNSKINFO pfnGetNSKInfo = (pfnGUARDGETNSKINFO)GetProcAddress(hModule, "GuardGetNSKInfo");
	pfnGUARDGETAPPNAME pfnGetAppName = (pfnGUARDGETAPPNAME)GetProcAddress(hModule, "GuardGetAppName");
	pfnGUARDGETCOMPANYNAME pfnGetCompanyName = (pfnGUARDGETCOMPANYNAME)GetProcAddress(hModule, "GuardGetCompanyName");
	pfnGUARDGETAPPNAME pfnGetSuffix = (pfnGUARDGETAPPNAME)GetProcAddress(hModule, "GuardGetSuffix");

	if (!pfnGetNSKInfo || !pfnGetAppName || !pfnGetCompanyName)
		return false;

	// get size of array
	DWORD dwSize = 0;
	CEntryList::iterator le = m_listEntries.end();
	CEntryList::iterator lp = m_listEntries.begin();

	{
		FILE* file = fopen( m_strFileName, "w" );

		// Get app_name
		LONG lNameSize = 0;
		pfnGetAppName(0, &lNameSize);
		{
			CString strAppName(DEF_APP_NAME);
			if (lNameSize)
			{
				pfnGetAppName(strAppName.GetBuffer(lNameSize + 1), &lNameSize);
				strAppName.ReleaseBuffer();
				if (strAppName.GetLength() > lNameSize)
					strAppName.SetAt(lNameSize, '\0');

			}
			// save app_name_string
			fprintf(file, "APP_NAME %s\n",(LPCSTR)strAppName);
		}

		// read size of app_suffix string
		{		
			LONG lSufSize = 0;
			CString strSuf;
			if (lSufSize)
			{
				pfnGetSuffix(strSuf.GetBuffer(lSufSize + 1), &lSufSize);
				strSuf.ReleaseBuffer();
				if (strSuf.GetLength() > lSufSize)
					strSuf.SetAt(lSufSize, '\0');
			}
			// save app_suffix_string
			fprintf(file, "Suffix %s\n", (LPCSTR)strSuf);
		}

		// read size of app_suffix string
		{		
			LONG lSufSize = 0;
			pfnGetCompanyName(0, &lSufSize);
			CString strSuf;
			if (lSufSize)
			{
				pfnGetCompanyName(strSuf.GetBuffer(lSufSize + 1), &lSufSize);
				strSuf.ReleaseBuffer();
				if (strSuf.GetLength() > lSufSize)
					strSuf.SetAt(lSufSize, '\0');
			}
			// save app_suffix_string
			fprintf(file, "CompanyName %s\n", (LPCSTR)strSuf);
		}
	
		fprintf(file, "NumberOfEntries %u\n", (DWORD)m_listEntries.size());
	
		for (lp = m_listEntries.begin(); lp != le; lp++)
		{
			TTranslateEntry* e = *lp;
			fprintf(file, "%s %s %u %s\n", 
				g2s(e->GuidExtern), g2s(e->GuidInner), e->dwData, e->szProgID);
		}
		fclose(file);
	}
	return true;
}

GuidKey s2g(LPCSTR s)
{
	_bstr_t cs=s;
	GuidKey key;
	HRESULT hr=CLSIDFromString(cs, &key);
	return key;
}

int scanValue(FILE* file,const CString& sFmt, CString& sValue)
{
	LONG len = 81;
	char szAppName[81];
	fgets(szAppName,len,file);
	int nf=sscanf(szAppName,sFmt+"%s\n", sValue.GetBuffer(len));
	sValue.ReleaseBuffer();
	return nf;
}

bool CTranslateTable::LoadText(LPCTSTR szFile)
{
	//if (lstrlen(szFile) && lstrcmp(m_strFileName, szFile))
	//	m_strFileName = szFile;

	//if (m_strFileName.IsEmpty() || m_GuidKey == INVALID_KEY)
	//	return false;

	// free old tables
	Free();

	if (m_GuidKey == INVALID_KEY)
		return false;

	// get functions
	HMODULE hModule = GetModuleHandle(0);
	if (!hModule)
		return false;

	typedef void (*pfnGUARDSETVALUE)(const char*);
	pfnGUARDSETVALUE pfnSetAppName = (pfnGUARDSETVALUE)GetProcAddress(hModule, "GuardSetAppName");
	pfnGUARDSETVALUE pfnSetSuffix  = (pfnGUARDSETVALUE)GetProcAddress(hModule, "GuardSetSuffix");
	pfnGUARDSETVALUE pfnSetComName = (pfnGUARDSETVALUE)GetProcAddress(hModule, "GuardSetCompanyName");

	bool bRet = false;
	int nf;
	try
	{
		FILE* file = fopen( szFile, "r" );
		if(!file)
			return bRet;
		// read size of app_name string
		{
			CString strAppName;
			scanValue(file, "APP_NAME",strAppName);
			if(strAppName.IsEmpty())
				strAppName=DEF_APP_NAME;
			pfnSetAppName( strAppName );			
		}

		// read count of app_suffix_string
		{
			CString strSuf;
			scanValue(file, "Suffix",strSuf);
			pfnSetSuffix( strSuf );			
		}

		// read size of app_suffix string
		{
			CString strCompanyName;
			scanValue(file, "CompanyName",strCompanyName);
			pfnSetComName( strCompanyName );			
		}

		// read imito
		m_dwImito =16;

		DWORD dwNEntries=0;
		nf=fscanf(file, "NumberOfEntries %u\n", &dwNEntries);

		for (DWORD ie=0; ie < dwNEntries; ++ie)
		{
			char sgExt[42], sgInn[42], szProgId[81];
			DWORD dwData=0;
			size_t nSize=0;

			bRet=false;
			nf=fscanf_s(file, "%s %s %u %[^\n ]\n", 
				sgExt, 41, sgInn, 41, &dwData, szProgId, 80);
			if(nf==4)
			{
				nSize=strlen(szProgId);
				if(nSize>0)
				{
					TTranslateEntry* e = new TTranslateEntry;
					e->GuidExtern = s2g(sgExt);
					e->GuidInner = s2g(sgInn);
					e->szProgID = new TCHAR [nSize+1];
					memcpy((void*)e->szProgID, szProgId , nSize+1);
					e->dwData = dwData;
					if(!Add(e))
					{
						delete e;
						break;
					}
					bRet=true;
				}
			}
		}
		fclose(file);
	}
	catch(CFileException * e)
	{
		CString strError;
		e->GetErrorMessage(strError.GetBuffer(1024), 1024);
		TRACE ("%s\n", strError);
		switch (e->m_cause)
		{
		case CFileException::tooManyOpenFiles:

		case CFileException::genericException:
		case CFileException::hardIO:
			GuardSetErrorCode(guardInvalidGuardFile);
			break;

		case CFileException::fileNotFound:
		case CFileException::badPath:
		case CFileException::invalidFile:
			GuardSetErrorCode(guardInvalidGuardFile);
			break;

		case CFileException::accessDenied:
		case CFileException::sharingViolation:
		case CFileException::lockViolation:
		case CFileException::badSeek:
		case CFileException::endOfFile:
			GuardSetErrorCode(guardReadGuardFile);
			break;
		}

		if( bReportErrors )e->ReportError();
		e->Delete();

		return false;
	}

	return bRet;
}

bool CTranslateTable::Load(LPCTSTR szFile)
{
	if (lstrlen(szFile) && lstrcmp(m_strFileName, szFile))
		m_strFileName = szFile;

	if (m_strFileName.IsEmpty() || m_GuidKey == INVALID_KEY)
		return false;

	// free old tables
	Free();

	if (m_GuidKey == INVALID_KEY)
		return false;

	// get functions
	HMODULE hModule = GetModuleHandle(0);
	if (!hModule)
		return false;

	typedef void (*pfnGUARDSETVALUE)(const char*);
	pfnGUARDSETVALUE pfnSetAppName = (pfnGUARDSETVALUE)GetProcAddress(hModule, "GuardSetAppName");
	pfnGUARDSETVALUE pfnSetComName = (pfnGUARDSETVALUE)GetProcAddress(hModule, "GuardSetCompanyName");

	BYTE * pBuffer = 0;
	bool bRet = true;
	try
	{
		CFile file(m_strFileName, CFile::modeRead|CFile::shareDenyRead);
		DWORD dwDiff = 0;
	
		// read signature
		int nSigSize = lstrlen(szGUARDFILESIG);
		CString strSig;

		file.Read(strSig.GetBuffer(nSigSize + 1), nSigSize);
		strSig.ReleaseBuffer( nSigSize );
		if (strSig.GetLength() > nSigSize)
			strSig.SetAt(nSigSize, '\0');

		if (lstrcmp(strSig, szGUARDFILESIG))
		{
			GuardSetErrorCode(guardInvalidGuardFile);
			return false;
		}
		dwDiff += nSigSize;

		// read size of app_name string
		LONG lNameSize = 0;
		file.Read(&lNameSize, sizeof(LONG));
		dwDiff += sizeof(LONG);

		CString strAppName(DEF_APP_NAME);
		if (lNameSize)
		{
			// save app_name_string
			file.Read(strAppName.GetBuffer(lNameSize + 1), lNameSize);
			strAppName.ReleaseBuffer( lNameSize );
			if (strAppName.GetLength() > lNameSize)
				strAppName.SetAt(lNameSize, '\0');

			dwDiff += lNameSize;
		}

		if( pfnSetAppName )
			pfnSetAppName( strAppName );			


		// Read NSK info 
		DWORD wSerNum = 0;
		BYTE bProg = 0;

		file.Read(&wSerNum, sizeof(DWORD));
		dwDiff += sizeof(DWORD);
		//file.Read(&bProg, sizeof(BYTE));
		//dwDiff += sizeof(BYTE);
//		pfnSetNSKInfo(&wSerNum, &bProg);
	
		// read size of app_suffix string
		LONG lSufSize = 0;
		file.Read(&lSufSize, sizeof(LONG));
		dwDiff += sizeof(LONG);

		// read count of app_suffix_string
		if (lSufSize)
		{
			// read app_suffix_string
			CString strSuf;
			file.Read(strSuf.GetBuffer(lSufSize + 1), lSufSize);
			strSuf.ReleaseBuffer( lSufSize );

			if (strSuf.GetLength() > lSufSize)
				strSuf.SetAt(lSufSize, '\0');

			dwDiff += lSufSize;
//			pfnSetSuffix(strSuf);

			if( pfnSetComName )
				pfnSetComName( strSuf );			

		}


		// add to diff sizeof(index) && sizeof(imito)
		dwDiff += sizeof(DWORD) + sizeof(int);

		// get file size
		DWORD dwSize = (DWORD)(file.GetLength() - dwDiff);
		// alloc memory to buffer
		pBuffer = new BYTE [dwSize];
		if (!pBuffer)
		{
			GuardSetErrorCode(guardMemory);
			return false;
		}

		int nIndex = 0;
		DWORD dwAllRead = 0;
		// read index
		file.Read(&nIndex, sizeof(int));

		// read first part of buffer
		dwAllRead += file.Read(pBuffer, nIndex);

		// read imito
		file.Read(&m_dwImito, sizeof(DWORD));

		// read last part of buffer
		dwAllRead += file.Read(&(pBuffer[nIndex]), dwSize - nIndex);

		// buffer size is not equal w/ read size
		if (dwAllRead != dwSize)
		{
			GuardSetErrorCode(guardReadGuardFile);
			if (pBuffer)
				delete [] pBuffer, pBuffer = 0;
			return false;
		}

		// decrypt file
		DWORD dwImito = 0;
		if (!m_CryptEngine.Decrypt(pBuffer, dwSize, dwImito))
		{
			GuardSetErrorCode(guardDecryptFile);
			if (pBuffer)
				delete [] pBuffer, pBuffer = 0;
			return false;
		}

		// imito is not valid
		if (m_dwImito != dwImito)
		{
			GuardSetErrorCode(guardInvalidGuardFile);
			if (pBuffer)
				delete [] pBuffer, pBuffer = 0;
			return false;
		}

		// calc end of buffer
		BYTE * pBufPtr = pBuffer;
		// read add all entries to table
		TTranslateEntry * pEntry = 0;
		while (pBufPtr < (pBuffer + dwSize))
		{
			pEntry = new TTranslateEntry;
			::EntryFromBuffer(pEntry, &pBufPtr);
			if (!Add(pEntry))
			{
				delete pEntry;
				bRet = false;
			}
			pEntry = 0;
		}
		delete [] pBuffer, pBuffer = 0;

	}
	catch(CFileException * e)
	{
		CString strError;
		e->GetErrorMessage(strError.GetBuffer(1024), 1024);
		TRACE ("%s\n", strError);
		switch (e->m_cause)
		{
		case CFileException::tooManyOpenFiles:

		case CFileException::genericException:
		case CFileException::hardIO:
			GuardSetErrorCode(guardInvalidGuardFile);
			break;

		case CFileException::fileNotFound:
		case CFileException::badPath:
		case CFileException::invalidFile:
			GuardSetErrorCode(guardInvalidGuardFile);
			break;

		case CFileException::accessDenied:
		case CFileException::sharingViolation:
		case CFileException::lockViolation:
		case CFileException::badSeek:
		case CFileException::endOfFile:
			GuardSetErrorCode(guardReadGuardFile);
			break;

//		case CFileException::directoryFull:
//		case CFileException::diskFull:
//			GuardSetErrorCode(guardWriteGuardFile);
//			break;
		}

		if( bReportErrors )e->ReportError();
		e->Delete();

		if (pBuffer)
			delete [] pBuffer, pBuffer = 0;
		return false;
	}

	return bRet;
}

CStringA g2s(GuidKey k)
{
	CStringW s; s.GetBufferSetLength(41); StringFromGUID2(k, s.GetBuffer(), 41);
	CStringA sa = s;
	return sa;
}

bool CTranslateTable::Add(TTranslateEntry * pEntry)
{
	if (!pEntry)
		return false;

	CGuidMap::iterator it = m_mapGuid.size() ? m_mapGuid.find(pEntry->GuidExtern) : m_mapGuid.end();
	if (it != m_mapGuid.end())
	{
		ASSERT(0);
		return false;
	}

	m_listEntries.push_back(pEntry);
	m_mapGuid[pEntry->GuidExtern] = pEntry;
	GuidKey guidCreapted = pEntry->GuidExtern;
	GuidKey guidDecreapted = DecryptEntry(pEntry);
	m_mapIntGuid[DecryptEntry(pEntry)] = pEntry;
	m_mapProgID[pEntry->szProgID] = pEntry;
	//WriteLogLine("guidCreapted = %s, guidDecreapted = %s, pEntry->GuidInner = %s, pEntry->szProgID = %s"
	//	, g2s(guidCreapted), g2s(guidDecreapted), g2s(pEntry->GuidInner), pEntry->szProgID);
	return true;
}

bool CTranslateTable::Add(GuidKey & External, GuidKey & Inner, LPCTSTR szProgID, DWORD dwData)
{
	CGuidMap::iterator it = m_mapGuid.size() ? m_mapGuid.find(External) : m_mapGuid.end();
	if (it != m_mapGuid.end())
	{
		ASSERT(0);
		return false;
	}

	TTranslateEntry * pEntry = new TTranslateEntry(External, Inner, szProgID, dwData);
	if (!pEntry)
		return false;

	if (!CryptEntry(pEntry))
	{
		delete pEntry;
		return false;
	}

	m_listEntries.push_back(pEntry);
	m_mapGuid[pEntry->GuidExtern] = pEntry;
	m_mapIntGuid[Inner] = pEntry;
	m_mapProgID[pEntry->szProgID] = pEntry;

	return true;
}

bool CTranslateTable::Remove(GuidKey & External)
{
	CGuidMap::iterator it = m_mapGuid.size() ? m_mapGuid.find(External) : m_mapGuid.end();
	if (it == m_mapGuid.end())
		return true;

	TTranslateEntry * pEntry = it->second;
	if (pEntry)
	{
		m_mapGuid.erase(pEntry->GuidExtern);
		m_mapIntGuid.erase(DecryptEntry(pEntry));
		m_mapProgID.erase(pEntry->szProgID);
		m_listEntries.remove(pEntry);
		delete 	pEntry;
	}

	return true;
}

bool CTranslateTable::Remove(LPCTSTR szProgID)
{
	CProgIDMap::iterator it = m_mapProgID.size() ? m_mapProgID.find(szProgID) : m_mapProgID.end();

	if (it  == m_mapProgID.end())
		return true;
	
	TTranslateEntry * pEntry = it->second;
	if (pEntry)
	{
		m_mapGuid.erase(pEntry->GuidExtern);
		m_mapIntGuid.erase(DecryptEntry(pEntry));
		m_mapProgID.erase(pEntry->szProgID);
		m_listEntries.remove(pEntry);
		delete 	pEntry;
	}

	return true;
}




bool CTranslateTable::CryptEntry(TTranslateEntry * pEntry)
{
	return true;
}

GuidKey CTranslateTable::DecryptEntry(TTranslateEntry * pEntry)
{
	return pEntry->GuidInner;
}

//////////////////////////////////////////////////////////////////////
// class CCryptEngine

UINT CCryptEngine::s_uMaxKeyItems = 128;

CCryptEngine::CCryptEngine()
{
	m_pchTable = 0;
	m_pKey = 0;
	m_pKeyImito = 0;

//	// обнуляем синхропосылки
//	m_Synchro[0] = 0L;
//	m_Synchro[1] = 0L;
//
//	m_Imito[0] = 0L;
//	m_Imito[1] = 0L;

	m_nKeyRepeat	= 4;   // Число повторений ключа
	m_nKeyMask		= 8;   // Стандартная маска 0001
	m_nKeySize		= 32;  // Число раундов шифрования
	m_nImitoKeySize	= 16;  // Число раундов имитовставки


	LoadExchTable();
}

CCryptEngine::~CCryptEngine()
{
	if (m_pchTable)
		delete [] m_pchTable, m_pchTable = 0;

	if (m_pKey)
		delete [] m_pKey, m_pKey = 0;

	m_pKeyImito = 0;
}

bool CCryptEngine::Crypt(BYTE * pSource, DWORD dwSize, DWORD & dwImito)
{
	CrItem	Synchro[2]	= {	0L, 0L	};// синхропосылка
	CrItem	Imito[2]	= {	0L, 0L	};

	if (!pSource || !dwSize)
		return false;

	// align needed size to 8 bytes
	DWORD dwBufSize = (dwSize + 7UL) & 0xFFFFFFF8UL;
	// alloc buffer
	BYTE * pBuffer = new BYTE [dwBufSize];
	if (!pBuffer)
		return false;

// crypting
	ULONG nQuant = (ULONG)dwBufSize;
	ULONG nBlocks = dwBufSize / sizeof(CrBlock);
	if (dwSize < dwBufSize)
	{
		nQuant = dwSize;
		nBlocks = (nQuant + sizeof(CrBlock) - 1) / sizeof(CrBlock);
	}

	// copy source content to buffer
	memcpy(pBuffer, pSource, nQuant);

	// zerous additional memory
	if (dwSize < dwBufSize)
		memset((void*)(pBuffer + dwSize), 0, dwBufSize - dwSize);

	// calc imito
	imito(m_pKeyImito, Imito, m_pchTable, pBuffer, nBlocks, m_nImitoKeySize);
	dwImito = Imito[1];

	// gamming crypt
	gammaLE(m_pKey, Synchro, m_pchTable, pBuffer, nBlocks, m_nKeySize);

	// copy crypted to source
	memcpy(pSource, pBuffer, nQuant);

	// delete buffer
	delete [] pBuffer;

	return true;
}

bool CCryptEngine::Decrypt(BYTE * pSource, DWORD dwSize, DWORD & dwImito)
{
	CrItem	Synchro[2]	= {	0L, 0L	};// синхропосылка
	CrItem	Imito[2]	= {	0L, 0L	};

	if (!pSource || !dwSize)
		return false;

		// align needed size to 8 bytes
	DWORD dwBufSize = (dwSize + 7UL) & 0xFFFFFFF8UL;
	// alloc buffer
	BYTE * pBuffer = new BYTE [dwBufSize];
	if (!pBuffer)
		return false;

// crypting
	ULONG nQuant = (ULONG)dwBufSize;
	ULONG nBlocks = dwBufSize / sizeof(CrBlock);
	if (dwSize < dwBufSize)
	{
		nQuant = dwSize;
		nBlocks = (nQuant + sizeof(CrBlock) - 1) / sizeof(CrBlock);
	}

	// copy source content to buffer
	memcpy(pBuffer, pSource, nQuant);

	// gamming decrypt
	gammaLD(m_pKey, Synchro, m_pchTable, pBuffer, nBlocks, m_nKeySize);

	// zerous additional memory
	if (dwSize < dwBufSize)
		memset((void*)(pBuffer + dwSize), 0, dwBufSize - dwSize);

	// calc imito
	imito(m_pKeyImito, Imito, m_pchTable, pBuffer, nBlocks, m_nImitoKeySize);
	dwImito = Imito[1];

	// copy crypted to source
	memcpy(pSource, pBuffer, nQuant);

	// delete buffer
	delete [] pBuffer;

	return true;
}

// set key 
bool CCryptEngine::SetKey(BYTE * pKey, DWORD dwKeySize)
{
	m_nKeyRepeat	= 4;   // Число повторений ключа
	m_nKeyMask		= 8;   // Стандартная маска 0001
	m_nKeySize		= 32;  // Число раундов шифрования
	m_nImitoKeySize	= 16;  // Число раундов имитовставки

	// delete old key
	if (m_pKey)
		delete [] m_pKey, m_pKey = 0;

	// check key's size is valid
	if (!pKey || !dwKeySize || dwKeySize % sizeof(CrItem))
	{
		TRACE ("bad key\n");
		return false;
	}

	if (dwKeySize / sizeof (CrItem) * m_nKeyRepeat > s_uMaxKeyItems)
		return false;

	// calc size of extended key
	m_nKeySize = dwKeySize / sizeof (CrItem) * m_nKeyRepeat;
	//calc size of imito key
	m_nImitoKeySize = 1 + (m_nKeyRepeat > 2);

	// alloc mem for new key
	m_pKey = (CrItem*)new BYTE [(m_nImitoKeySize + m_nKeyRepeat) * dwKeySize];
	if (!m_pKey)
		return false;

	// set ptr on imito Key to valid value
	m_pKeyImito = (CrItem*)((BYTE*)m_pKey + dwKeySize * m_nKeyRepeat);

	// read key
	memcpy(m_pKeyImito, pKey, dwKeySize);

	// repeate loaded key
	if (m_nImitoKeySize == 2)
		memcpy((BYTE*)m_pKeyImito + dwKeySize, m_pKeyImito, dwKeySize);

	m_nImitoKeySize *= dwKeySize / sizeof(CrItem);

	ExpandKey(m_pKeyImito, m_pKey, dwKeySize / sizeof (CrItem), m_nKeyRepeat, m_nKeyMask);
	
	return true;
}

// 	load and expand substitution's  table 
bool CCryptEngine::LoadExchTable()
{
	if (m_pchTable)
		delete [] m_pchTable, m_pchTable = 0;

	m_pchTable = new BYTE [1024];
	if (!m_pchTable)
		return false;

//	memset(m_pchTable, 0, sizeof(BYTE) * 1024);
	// expand table of substitutions
	ExpCht((CrItem*)STable, (CrItem*)m_pchTable);

	return true;
}


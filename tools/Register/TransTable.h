// TransTable.h: interface for the CTranslateTable class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GUARD_TRANSLATE_H
#define __GUARD_TRANSLATE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


///////////////////////////////////////////////////////////////////////////////
// class CCryptEngine

class CCryptEngine
{
public:
	CCryptEngine();
	virtual ~CCryptEngine();

	bool Crypt(BYTE * pSource, DWORD dwSize, DWORD & dwImito);
	bool Decrypt(BYTE * pSource, DWORD dwSize, DWORD & dwImito);

	bool SetKey(BYTE * pKey, DWORD dwKeySize);

protected:
	bool LoadExchTable();

protected:
	BYTE *		m_pchTable;	// table of exchange
	CrItem *	m_pKey;		// адрес ключа шифрования
	CrItem *	m_pKeyImito;// адрес ключа выработки

	int	m_nKeyRepeat;   // Число повторений ключа
	int	m_nKeyMask;   // Стандартная маска 0001
	int	m_nKeySize;  // Число раундов шифрования
	int	m_nImitoKeySize;  // Число раундов имитовставки

	static UINT s_uMaxKeyItems;

};

///////////////////////////////////////////////////////////////////////////////
// struct TTranslateEntry

struct TTranslateEntry
{
	GuidKey GuidExtern;	// extern guid
	GuidKey GuidInner;		// inner guid
	TCHAR *	szProgID;		// inner ProgID
	DWORD	dwData;		// additional data

	TTranslateEntry(GuidKey & guidExt = INVALID_KEY, GuidKey & guidInn = INVALID_KEY, LPCTSTR szString = 0, DWORD dw = 0);
	virtual ~TTranslateEntry();
};

// typedefs and helper structs
#pragma warning (disable : 4786)
#include <map>
#include <list>
//using namespace std;

struct ProgID_cmp
{
	bool operator()(const char* s1, const char* s2) const
	{	return strcmp(s1, s2) < 0;	}
};

struct Guid_cmp
{
	bool operator()(const GuidKey & g1, const GuidKey & g2) const
	{	return (lo_guid(&g1) - lo_guid(&g2)) < 0;	}
};

typedef std::map<const char*, TTranslateEntry *, ProgID_cmp> CProgIDMap;
typedef std::map<GuidKey, TTranslateEntry *, Guid_cmp>		CGuidMap;
typedef std::list<TTranslateEntry *>							CEntryList;

///////////////////////////////////////////////////////////////////////////////
// class CTranslateTable

class CTranslateTable
{
public:
	CTranslateTable();
	virtual ~CTranslateTable();
	void Free();

public:
// Guid operation
	GuidKey Get(GUID & External);
	GuidKey Get(LPCTSTR szProgID);
	GuidKey GetExtern(LPCTSTR szProgID);
	GuidKey GetExtern(GUID & Internal);

	DWORD	GetData(LPCTSTR szProgID);
	DWORD	GetData(GUID & External);

// save/load operations
	bool Save(LPCTSTR szFile);
	bool Load(LPCTSTR szFile);

	void SetKey(GuidKey & rKey, DWORD dwImito = 0);
	void GetKey(GuidKey & rKey, DWORD & dwImito)	{	rKey = m_GuidKey; dwImito = m_dwImito;	}


	bool	Add(GuidKey & External, GuidKey & Inner, LPCTSTR szProgID, DWORD dwData);
	bool	Add(TTranslateEntry * pEntry);
	bool	Remove(GuidKey & External);
	bool	Remove(LPCTSTR szProgID);
	bool	Remove(TTranslateEntry * pEntry);

	TTranslateEntry *	GetEntry(LPCTSTR szProgID);
	TTranslateEntry *	GetEntry(GUID & External);

	bool	CryptEntry(TTranslateEntry * pEntry);
	GuidKey	DecryptEntry(TTranslateEntry * pEntry);

protected:
	CCryptEngine	m_CryptEngine;

private:
	GuidKey		m_GuidKey;		// crypt_key
	DWORD		m_dwImito;

	CString		m_strFileName;		// TranslateTable file name
	IUnknownPtr m_sptrApp;		// pointer to application unknown
	CGuidMap	m_mapGuid;		// map w/ associations ExternalGuid <-> TranslateEntry
	CGuidMap	m_mapIntGuid;		// map w/ associations InternalGuid <-> TranslateEntry
	CProgIDMap	m_mapProgID;	// map w/ associations ProgID <-> TranslateEntry
	CEntryList	m_listEntries;	// list of Translation entries

};


extern const char * szGUARDFILESIG;





#endif // __GUARD_TRANSLATE_H

#include "stdafx.h"
#include <map>
#include <vector>
#include <atlstr.h>
#include "data_main.h"
#include "class_utils.h"
#include "ColorsNames.h"

void CColorsNames::UpdateColorsNames(const char *szClassifierShortName)
{
	if(ParseClassifier(full_classifiername(szClassifierShortName)))
	{
		Save();
	}
	else if(0==_mapDscrClass.size())
	{
		LoadColorsNames();
	}
	else
	{
//		_ASSERTE(!"Color and Name data for classifier not found");
	}
}

const char* CColorsNames::get_class_name(const LPOS lpos)
{
	const std::map<int,ColorName>::const_iterator it=_mapDscrClass.find(int(lpos));
	if(it!=_mapDscrClass.end())
		return (LPCSTR)it->second._Name;
	else{
		static _bstr_t sUnknown;
		sUnknown=GetValueString( GetAppUnknown(), "\\Classes", "UnkClassName", "Unknown" );
		return (LPCSTR)sUnknown;
	}
}

const COLORREF CColorsNames::get_class_color(const LPOS lpos)
{
	const std::map<int,ColorName>::const_iterator it=_mapDscrClass.find(int(lpos));
	if(it!=_mapDscrClass.end())
		return it->second._clr;
	else
		return GetValueColor( GetAppUnknown(), "\\Classes", "UnkClassColor", RGB( 255, 255, 0 ) );;
}

bool CColorsNames::testClass(const LPOS lpos)
{
	const std::map<int,ColorName>::const_iterator it=_mapDscrClass.find(int(lpos));
	if(it!=_mapDscrClass.end())
		return true;
	else
		return false;
}

bool CColorsNames::ParseClassifier(const char *szClassifierName)
{
	_mapDscrClass.clear();
	char psz_text[4096];

	DWORD dwCharsRead=::GetPrivateProfileString("Classes", NULL, ""
		, psz_text, 4096, szClassifierName);
	for (char* psz=psz_text; psz<psz_text+dwCharsRead; psz += strlen(psz)+1)
	{
		int iClassNo;
		if(sscanf(psz,"%d",&iClassNo))
		{
			char sz_ClsName[96];
			::GetPrivateProfileString("Classes", psz, ""
				, sz_ClsName, 96, szClassifierName);
			int	r = 255, g = 0, b = 0;
			int nFields=::sscanf( sz_ClsName, "(%d,%d,%d),%[^\0]", &r, &g, &b, sz_ClsName );
			if(nFields>=0){
				if(nFields<=3){
					sprintf(sz_ClsName,"Class%d",iClassNo);
				}
				ColorName colorName(RGB(r,g,b), ATL::CString(sz_ClsName));
				_mapDscrClass[iClassNo]=colorName;
			}
		}
	}
	return _mapDscrClass.size()>0;
}

bool CColorsNames::LoadColorsNames(INamedData* pNamedData)
{
	_mapDscrClass.clear();
	INamedDataPtr pNd=(LPUNKNOWN)this;
	if(!(bool)pNd)
		pNd=INamedDataPtr((LPUNKNOWN)this);

	pNd->SetupSection(CLASSIFIER_CLASSES);
	long cnt=0; pNd->GetEntriesCount(&cnt);
	for (long iEntry=0; iEntry<cnt; ++iEntry)
	{
		ATL::CComBSTR entry;

		pNd->GetEntryName(iEntry, &entry);
		ATL::CComVariant var=entry;
		if(SUCCEEDED(var.ChangeType(VT_I4)))
		{
			int iClassNo=var.intVal;
			WCHAR wsz_ClsName[96];
			HRESULT hr=pNd->GetValue(entry, &var);
			if(SUCCEEDED(hr) && VT_BSTR==var.vt)
			{
				int	r = 255, g = 0, b = 0;
				int nFields=::swscanf( var.bstrVal, L"(%d,%d,%d),%[^\0]S", &r, &g, &b, wsz_ClsName );
				if(nFields>=0){
					if(nFields<=3){
						swprintf(wsz_ClsName,L"Class%d",iClassNo);
					}
					ColorName colorName(RGB(r,g,b), ATL::CString(wsz_ClsName));
					_mapDscrClass[iClassNo]=colorName;
				}
			}
		}
	}
	return _mapDscrClass.size()>0;
}

bool CColorsNames::Save()
{
	INamedDataPtr pNd=(LPUNKNOWN)this;
	pNd->DeleteEntry(CLASSIFIER_CLASSES);
	pNd->SetupSection(CLASSIFIER_CLASSES);
	for (	std::map<int,ColorName>::const_iterator it=_mapDscrClass.begin();
		it!=_mapDscrClass.end(); ++it)
	{
		int iClassNo=it->first;
		const ColorName& colorName=it->second;
		WCHAR wsz_ClsName[96];
		int	r = GetRValue(colorName._clr),
			g = GetGValue(colorName._clr), 
			b = GetBValue(colorName._clr);
		int nFields=::swprintf(wsz_ClsName, L"(%d,%d,%d),%s", r, g, b
			, (LPCWSTR)ATL::CStringW(colorName._Name));
		ATL::CComVariant vr=long(iClassNo);
		vr.ChangeType(VT_BSTR);
		pNd->SetValue(vr.bstrVal,ATL::CComVariant(wsz_ClsName));
	}
	return _mapDscrClass.size()>0;
}

bool CColorsNames::SetClasses(const std::vector<long>& aVecClasses, INamedData* pNamedData)
{
	_mapDscrClass.clear();
	INamedDataPtr pNd=pNamedData;
	if(!(bool)pNd)
		pNd=INamedDataPtr((LPUNKNOWN)this);
	pNd->SetupSection(CLASSIFIER_CLASSES);
	for (	std::vector<long>::const_iterator it=aVecClasses.begin();
		it!=aVecClasses.end(); ++it)
	{
		int iClassNo=*it;
		ATL::CComVariant vClassNo=iClassNo;
		if(SUCCEEDED(vClassNo.ChangeType(VT_BSTR)))
		{
			ATL::CComVariant var;
			pNd->GetValue(vClassNo.bstrVal, &var);
			if(VT_BSTR==var.vt)
			{
				int	r=255, g=0, b=0;
				WCHAR wsz_ClsName[96];
				int nFields=::swscanf( var.bstrVal, L"(%d,%d,%d),%[^\0]S", &r, &g, &b, wsz_ClsName );
				if(nFields>=0){
					if(nFields<=3){
						swprintf(wsz_ClsName,L"Class%d",iClassNo);
					}
					ColorName colorName( RGB(r,g,b), ATL::CString(wsz_ClsName));
					_mapDscrClass[iClassNo]=colorName;
				}
			}
		}
	}
	return _mapDscrClass.size()>0;
}

// read names and colors only for given classes from current classifier
bool CColorsNames::RestoreByClassifier(const std::vector<long>& aVecClasses)
{
	_mapDscrClass.clear();
	const char* pszClassifierName=full_classifiername(short_classifiername(0));
	for (	std::vector<long>::const_iterator it=aVecClasses.begin();
		it!=aVecClasses.end(); ++it)
	{
		int iClassNo=*it;
		char szClassNo[10];
		if(sprintf(szClassNo,"%d",iClassNo))
		{
			char sz_ClsName[96];
			::GetPrivateProfileString("Classes", szClassNo, ""
				, sz_ClsName, 96, pszClassifierName);
			int	r = 255, g = 0, b = 0;
			int nFields=::sscanf( sz_ClsName, "(%d,%d,%d),%[^\0]", &r, &g, &b, sz_ClsName );
			if(nFields>=0){
				if(nFields<=3){
					strcpy(sz_ClsName,"Class");
					strcat(sz_ClsName,szClassNo);
				}
				ColorName colorName(RGB(r,g,b), sz_ClsName);
				_mapDscrClass[iClassNo]=colorName;
			}
		}
	}
	return _mapDscrClass.size()>0;
}

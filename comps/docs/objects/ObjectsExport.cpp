#include "stdafx.h"
#include "ObjectsExport.h"
#include "resource.h"
#include "math.h"
#include "NameConsts.h"
#include "objectlist.h"
#include "float.h"
#include "\vt5\common2\class_utils.h"

#define EXPORT_TO_FILE				0
#define EXPORT_TO_CLIPBOARD			1

class CClipboard
{
	BOOL m_bOpened;
public:
	CClipboard()
	{
		m_bOpened = Open();
	}
	~CClipboard()
	{
		Close();
	}
	BOOL Open()
	{
		return OpenClipboard(NULL);
	}
	void Close()
	{
		CloseClipboard();
	}
	BOOL IsOpened()
	{
		return m_bOpened;
	}
};



IMPLEMENT_DYNCREATE(CActionObjectsExport, CCmdTargetEx);

// {3969CCAB-13D8-4178-B145-0D6D48D3FE29}
GUARD_IMPLEMENT_OLECREATE(CActionObjectsExport, "Objects.ObjectsExport", 
0x3969ccab, 0x13d8, 0x4178, 0xb1, 0x45, 0xd, 0x6d, 0x48, 0xd3, 0xfe, 0x29);

// {FC60FE5A-C767-4ad1-9520-5A0C5BAB3511}
static const GUID clsidObjectsExport = 
{ 0xfc60fe5a, 0xc767, 0x4ad1, { 0x95, 0x20, 0x5a, 0xc, 0x5b, 0xab, 0x35, 0x11 } };

ACTION_INFO_FULL(CActionObjectsExport, IDS_ACTION_OBJECTS_EXPORT, -1, -1, clsidObjectsExport);

ACTION_TARGET(CActionObjectsExport, szTargetAnydoc);

ACTION_ARG_LIST(CActionObjectsExport)
	ARG_STRING("ObjectList", 0)
	ARG_STRING("FileName", 0)
	ARG_INT("ExportType", 0)
	ARG_INT("AddParamNames", 1)
	ARG_STRING("DecimalSeparator", 0)
END_ACTION_ARG_LIST()

CActionObjectsExport::CActionObjectsExport()
{
}

CActionObjectsExport::~CActionObjectsExport()
{
}

bool CActionObjectsExport::Invoke()
{
	CString sListName = GetArgumentString("ObjectList");
	IUnknownPtr punkList;
	if (sListName.IsEmpty())
		punkList = IUnknownPtr(::GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList),
			false);
	else
		punkList = IUnknownPtr(::GetObjectByName(m_punkTarget, sListName, szTypeObjectList),
			false);
	//kir -- Composite support
	long bC = 0;
	ICompositeObjectPtr cs(punkList);
	if(cs!=0)
	{
		cs->IsComposite(&bC);
		if(bC)
		{
			IUnknown* unk;
			cs->GetMainLevelObjects(&unk);
			if(unk)
			{
				punkList = unk;				
			}
			
		}
	}
	//
	CObjectListWrp ObjList(punkList);
	if (ObjList == 0)
		return false;

	int nExpType = GetArgumentInt("ExportType");
	BOOL bAddParamNames = GetArgumentInt("AddParamNames");
	CString sFileName = GetArgumentString("FileName");
	CString sDecSep = GetArgumentString("DecimalSeparator");
	
	CWnd *pWndActive = CWnd::GetActiveWindow();
	if (nExpType == EXPORT_TO_FILE)
	{
		if (sFileName.IsEmpty())
		{
			OPENFILENAME ofn;
			::ZeroMemory( &ofn, sizeof(OPENFILENAME) );
			CString sTitle,sFilter;
			sTitle.LoadString(IDS_EXPORT_TITLE);
			sFilter.LoadString(IDS_EXPORT_FILTER);
			CFileDialog FileDlg(FALSE, _T(".txt"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
				sFilter, pWndActive);
			if (FileDlg.DoModal() != IDOK)
				return false;
			sFileName = FileDlg.GetPathName();
		}

		CFile file;
		CFileException exc;
		if (!file.Open(sFileName, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite, &exc))
		{
			exc.ReportError();
			return false;
		}
		if (!ExportToFile(ObjList, &file, sDecSep, bAddParamNames, TRUE))
			return false;
		return true;
	}
	else if (nExpType == EXPORT_TO_CLIPBOARD)
	{
		CClipboard Clipboard;
		if (!Clipboard.IsOpened())
			return false;
//		if( !EmptyClipboard() )
//			return false;
		CMemFile file(256*1024);
		if (!ExportToFile(ObjList, &file, sDecSep, bAddParamNames, FALSE))
			return false;
		SIZE_T nSize = (SIZE_T)file.GetLength();
		HGLOBAL hg = GlobalAlloc(GPTR,nSize);
		if (hg != NULL)
		{
			if (!EmptyClipboard())
			{
				GlobalFree(hg);
				return false;
			}
			LPBYTE lpData = file.Detach();
			memcpy(hg, lpData, nSize);
			free(lpData);
		}
		if (!::SetClipboardData(CF_UNICODETEXT, hg))
			return false;
		return true;
	}
	else
		return false;
	return true;
}

bool CActionObjectsExport::IsAvaible()
{
	return true;
}

static bool FormatValueFromCalcObject(ParameterContainer *pc, ICalcObject *pcalc, CString &strValue, LPCTSTR lpstrDecSep)
{
	CString	strFmt;
	double	val = 0.;
	if( !pc || !pc->pDescr )	return false;
	if( pcalc->GetValue( pc->pDescr->lKey, &val ) == S_OK )
	{
		strFmt = pc->pDescr->bstrDefFormat;
		strValue.Format( strFmt, val*pc->pDescr->fCoeffToUnits );
	}
	else
		strValue = _T("-");
	if (lpstrDecSep)
	{
//		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDec, 5);
		int n = strValue.Find(_T('.'));
		if (n > -1)
		{
			CString s1 = strValue.Left(n);
			CString s2 = strValue.Mid(n+1);
			strValue = s1+CString(lpstrDecSep)+s2;
		}
	}
	return true;
}


bool CActionObjectsExport::ExportToFile(CObjectListWrp &ObjList, CFile *pFile,
	LPCTSTR lpstrDecSep, BOOL bAddParamNames, BOOL bByteOrderMarker)
{
	CArchive ar(pFile, CArchive::store);
	if (bByteOrderMarker )// UTF-16, little endian
	{
		unsigned char hdr[2];
		hdr[0] = 0xFF;
		hdr[1] = 0xFE;
		ar.Write(hdr, 2);
	}
	_bstr_t bstrTab(_T("\t"));
	_bstr_t bstrNewLine(_T("\r\n"));
	_bstr_t bstrComma(_T(","));
	ICalcObjectContainerPtr sptrCOC(ObjList);
	INamedDataObject2Ptr objects(ObjList);
	long lpos;
	if (bAddParamNames)
	{
		CStringW sClass;
		sClass.LoadString(IDS_CLASS_CAPTION);
		ar.Write((const wchar_t*)sClass,sClass.GetLength()*sizeof(wchar_t));
		ar.Write((wchar_t*)bstrTab,bstrTab.length()*sizeof(wchar_t));
		sptrCOC->GetFirstParameterPos(&lpos);
		while (lpos)
		{
			ParameterContainer *pc = 0;
			sptrCOC->GetNextParameter(&lpos, &pc);
			_bstr_t bstrName(pc->pDescr->bstrUserName);
			bstrName += bstrComma;
			bstrName += pc->pDescr->bstrUnit;
			ar.Write((wchar_t*)bstrName,bstrName.length()*sizeof(wchar_t));
			if (lpos)
				ar.Write((wchar_t*)bstrTab,bstrTab.length()*sizeof(wchar_t));
		}
		ar.Write((wchar_t*)bstrNewLine,bstrNewLine.length()*sizeof(wchar_t));
	}
	long lposObj;
	objects->GetFirstChildPosition(&lposObj);
	while (lposObj) 
	{
		IUnknownPtr punk;
		objects->GetNextChild(&lposObj, &punk);
		ICalcObjectPtr sptrCO(punk);
		if (sptrCO != 0)
		{
			long lClass = get_object_class(sptrCO);
			CStringW sClassName(get_class_name(lClass));
			ar.Write((const wchar_t*)sClassName, sClassName.GetLength()*sizeof(wchar_t));
			ar.Write((wchar_t*)bstrTab,bstrTab.length()*sizeof(wchar_t));
			sptrCOC->GetFirstParameterPos(&lpos);
			while (lpos)
			{
				ParameterContainer *pc = 0;
				sptrCOC->GetNextParameter(&lpos, &pc);
				CString strVal(_T(" "));
				FormatValueFromCalcObject(pc, sptrCO, strVal, lpstrDecSep);
				_bstr_t bstrVal(strVal);
				ar.Write((wchar_t*)bstrVal, bstrVal.length()*sizeof(wchar_t));
				if (lpos)
					ar.Write((wchar_t*)bstrTab,bstrTab.length()*sizeof(wchar_t));
			}
			if (lposObj)
				ar.Write((wchar_t*)bstrNewLine,bstrNewLine.length()*sizeof(wchar_t));
		}
	}

	return true;
}



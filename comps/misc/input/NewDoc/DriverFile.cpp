// DriverFile.cpp: implementation of the CDriverFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "newdoc.h"
#include "DriverFile.h"
#include "Utils.h"
#include "InputUtils.h"
#include <ctype.h>
#include <io.h>
#include <fcntl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IUnknownPtr _GetObjectByName( IUnknown *punkFrom, const BSTR bstrObject, const BSTR bstrType, bool bThrowException = true)
{
	_bstr_t	bstrObjectName = bstrObject;
	_bstr_t	bstrTypeName = bstrType;
	sptrIDataTypeManager	ptrDataType( punkFrom );

	long	nTypesCounter = 0;
	ptrDataType->GetTypesCount( &nTypesCounter );

	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		BSTR	bstrCurTypeName = 0;
		ptrDataType->GetType( nType, &bstrCurTypeName );

		_bstr_t	str( bstrCurTypeName, false );

		if( bstrType && str != bstrTypeName )
			continue;

		IUnknown	*punkObj = 0;
		LONG_PTR	dwPos = 0;

		ptrDataType->GetObjectFirstPosition( nType, &dwPos );

		while( dwPos )
		{
			punkObj = 0;
			ptrDataType->GetNextObject( nType, &dwPos, &punkObj );

			if (bstrObjectName.length() == 0 || ::GetObjectName( punkObj ) == (char*)bstrObjectName)
			{
				IUnknownPtr sptr(punkObj);
				if (punkObj) punkObj->Release();
				return sptr;
			}

			INamedDataObject2Ptr	ptrN( punkObj );
			punkObj->Release();

			POSITION lpos = 0;
			ptrN->GetFirstChildPosition( &lpos );

			while( lpos )
			{
				IUnknown	*punkObject = 0;
				ptrN->GetNextChild( &lpos, &punkObject );

				if (bstrObjectName.length() == 0 || ::GetObjectName( punkObject ) == (char*)bstrObjectName)
				{
					IUnknownPtr sptrO(punkObject);
					punkObject->Release();
					return sptrO;
				}

				punkObject->Release();


			}

			
		}
	}
	return 0;
}

void _CopyImage(CImageWrp &imgDst, CImageWrp &imgSrc)
{
	int nWidth = imgSrc.GetWidth();
	int nHeight = imgSrc.GetHeight();
	CString sCC = imgSrc.GetColorConvertor();
	imgDst.CreateNew(nWidth, nHeight, sCC);
	int nPanes = imgSrc.GetColorsCount();
	ASSERT(nPanes == imgDst.GetColorsCount());
	for (int i = 0; i < nPanes; i++)
	{
		for (int y = 0; y < nHeight; y++)
		{
			color *psrc = imgSrc.GetRow(y, i);
			color *pdst = imgDst.GetRow(y, i);
			memcpy(pdst, psrc, nWidth*sizeof(color));
		}
	}
}

bool _DoOpenFile(LPCTSTR lpszPathName, IUnknown *pImage, IUnknown *pTarget)
{
	_bstr_t	bstrFileName(lpszPathName);
	sptrIDocument sptrDoc(pTarget);
	sptrIDocumentSite sptrDS(pTarget);
	if (!bool(sptrDoc)) return false;
	if (!bool(sptrDS)) return false;
	IUnknown *punk = FindDocTemplate(_T("Image"));
	if (!punk) return false;
	sptrIShellDocTemplate sptrSDT(punk);
	punk->Release();
	sptrIFileDataObject	sptrD("Data.NamedData");
	if (sptrD == 0)
		return false;
	sptrSDT->GetFileFilterByFile(bstrFileName, &punk);
	if( !punk )	//It is native document format
	{
		if (sptrD->LoadFile(bstrFileName) != S_OK)
			return false;
	}
	else
	{
		sptrIFileFilter	sptrIFilter(punk);
		sptrIFileDataObject	sptrIFile(punk);
		punk->Release();
		sptrIFilter->AttachNamedData(sptrD, 0); //initialize FileFilter
		bool bSucceded = sptrIFile->LoadFile(bstrFileName) == S_OK; //and read data
		sptrIFilter->AttachNamedData(0, 0);
		if (!bSucceded)	return false;
	}
	_bstr_t bstrImageType(szArgumentTypeImage);
	IUnknownPtr ptrImgSrc = _GetObjectByName(sptrD, NULL, bstrImageType);
	CImageWrp imgSrc(ptrImgSrc);
	CImageWrp imgDst(pImage);
	_CopyImage(imgDst, imgSrc);
	TCHAR szFName[_MAX_FNAME];
	_tsplitpath(lpszPathName, NULL, NULL, szFName, NULL);
	if (szFName[0] != 0)
	{
		INamedObject2Ptr sptrNO2Img(imgDst);
		sptrNO2Img->SetName(_bstr_t(szFName));
	}
	return true;
}


//extern CString g_sName;
void _PreformCopy( INamedData *pdest, INamedData *psrc, const char *szSecDst, const char *szSecSrc,
	LPCTSTR lpstrName)
{
	_bstr_t	bstrSecDst = szSecDst;
	_bstr_t	bstrSecSrc = szSecSrc;
	psrc->SetupSection( bstrSecSrc );
	pdest->SetupSection( bstrSecDst );

	long	nCount, nEntry;
	psrc->GetEntriesCount( &nCount );

	for( nEntry = 0; nEntry < nCount; nEntry++ )
	{
		BSTR	bstrTmp = 0;
		psrc->GetEntryName( nEntry, &bstrTmp );
		_bstr_t bstrNameSrc(bstrTmp);
		_variant_t	var;
		psrc->GetValue( bstrNameSrc, &var );
		_bstr_t bstrNameDstTpl;
		if (!lpstrName || !lpstrName[0])
			bstrNameDstTpl = bstrNameSrc;
		else
		{
			bstrNameDstTpl = lpstrName;
//			lpstrName = NULL;
		}
		_bstr_t bstrNameDst(bstrNameDstTpl);

		long lExist;
		pdest->NameExists(bstrNameDst, &lExist);
		if (lExist)
		{
			_bstr_t bstrSrc;
			char *pszNamVar = new char[bstrNameSrc.length()+10];
			for (int nNamVar = 0; lExist && nNamVar < 1000; nNamVar++)
			{
				sprintf(pszNamVar, "%s%d", (const char *)bstrNameDstTpl, nNamVar);
				bstrNameDst = pszNamVar;
				pdest->NameExists(bstrNameDst, &lExist);
			}
			delete pszNamVar;
		}
		if (lExist)
			continue;

		if( var.vt == VT_BITS )
		{
			byte	*pdata = (byte*)var.lVal;
			long	lSize = *(long*)pdata;
			byte	*pdata1 = new byte[sizeof( lSize )+lSize];
			//*(long*)pdata1 = lSize;
			memcpy( pdata1, pdata, lSize+sizeof( lSize ) );

			var = _variant_t((long)pdata1);
			var.vt = VT_BITS;
		}
		pdest->SetValue( bstrNameDst, var );

		CString	strSrc(szSecSrc);
		if( strSrc != "\\" )
			strSrc += "\\";
		strSrc += (char*)bstrNameSrc;
		CString	strDst(szSecDst);
		if( strDst != "\\" )
			strDst += "\\";
		strDst += (char*)bstrNameDst;

		_PreformCopy( pdest, psrc, strDst, strSrc, lpstrName);
		
		psrc->SetupSection( bstrSecSrc );
		pdest->SetupSection( bstrSecDst );
	}
}

//append file suffix helper
static void _AppendFileSuffix( CString &strFilter, const char *szName, CString strExt )
{
	CString strExts;
	strFilter += szName;
	strFilter += " (";
	int	idx = 0;
	CString	strAll = strExt;
	while( idx != -1 )
	{
		idx = strAll.Find( "\n" );
		CString	strExt;
		if( idx == -1 )	 
			strExt = strAll;
		else
		{
			strExt = strAll.Left( idx );
			strAll = strAll.Right( strAll.GetLength()-1-idx );
		}
		strExts += '*' + strExt + ';';
	}
	strExts.Delete(strExts.GetLength()-1);	
	strFilter += strExts;
	strFilter += ")";
	strFilter += '\0';
	strFilter += strExts;
	strFilter += '\0';

}

static bool __IsImageFilter(IUnknown *punkFilter)
{
	sptrIFileFilter2 spFilter2(punkFilter);
	if (spFilter2 == 0)
		return true;
	int nObjectTypes;
	spFilter2->GetObjectTypesCount(&nObjectTypes);
	for (int i = 0; i < nObjectTypes; i++)
	{
		BSTR bstrType;
		spFilter2->GetObjectType(i, &bstrType);
		CString sType = bstrType;
		::SysFreeString(bstrType);
		if (sType.CompareNoCase(szTypeImage) == 0)
			return true;
	}
	return false;
}

/*static BOOL IsGoodExt( CString &strExt )
{
	if( strExt.IsEmpty() )
		return true;
	BOOL bUseFilterTemplate = ::GetValueInt( GetAppUnknown(), "\\FileOpenDialog", "UseTemplate", 0 );
	if( bUseFilterTemplate )
	{
		CString strFilterTemplate = ::GetValueString( GetAppUnknown(), "\\FileOpenDialog", "FilterTemplate", "" );

		if( strFilterTemplate.IsEmpty() )
			return true;

		if( strExt.Find( "\n", 0 ) == -1 )
		{
			if( strFilterTemplate.Find( strExt.Right( strExt.GetLength() - 1 ), 0 ) == - 1 )
				return false;
		}
		else
		{
			CString ResultStr = "";
			int nSz = strExt.GetLength();
			int nBrID = -1;
			for(int i = 0; i < nSz;i++ )
			{
				if( strExt[i] == '\n' )
				{
					CString Res;
					
					if( nBrID >= 0 )
					{
						Res = strExt.Right( nBrID );
						Res = Res.Left( i - nBrID );
					}
					else
						Res = strExt.Left( i );
					
					nBrID = i;

					if( strFilterTemplate.Find( Res.Right( Res.GetLength() - 1 ), 0 ) != - 1 )
					{
						if( !ResultStr.IsEmpty() )
							ResultStr += "\n" + Res;
						else
							ResultStr = Res;
					}
				}
			}

			{
				CString Res;
				
				if( nBrID >= 0 )
					Res = strExt.Right( strExt.GetLength() - nBrID - 1 );
				else
					Res = strExt.Left( i );
				
				nBrID = i;

				if( strFilterTemplate.Find( Res.Right( Res.GetLength() - 1 ), 0 ) != - 1 )
				{
					if( !ResultStr.IsEmpty() )
						ResultStr += "\n" + Res;
					else
						ResultStr = Res;
				}
			}


			if( ResultStr.IsEmpty() )
				return false;

			strExt = ResultStr;

		}
	}

	return true;

}*/


static bool __AppendFilterSuffix(CString &strFilter, OPENFILENAME *pofn, CString *pstrDefault, bool bOpenFile)
{
	CString sDocTemplName(_T("Image"));
	// Find image doc template
	IUnknown *punk = FindDocTemplate(sDocTemplName);
	if (!punk) return false;
	sptrIDocTemplate sptrDT(punk);
	ICompManagerPtr sptrCM(punk);
	// Load filters for image document
	CCompManager manFilters;
	manFilters.AttachComponentGroup(sDocTemplName+"\\"+szPluginExportImport);
	manFilters.SetRootUnknown(punk);
	manFilters.Init();
	punk->Release();
	// Obtain document string
	BSTR bstr = NULL;
	HRESULT hr = sptrDT->GetDocTemplString(-1, &bstr);
	if (FAILED(hr) || !bstr) return false;
	CString sDocString(bstr);
	::SysFreeString(bstr);
	// Get filter name and extension
	CString	strExt;
	CString	strName;
	int i;
	AfxExtractSubString(strName, sDocString, CDocTemplate::filterName);
	if (strName.IsEmpty()) return false;
	AfxExtractSubString(strExt, sDocString, CDocTemplate::filterExt);
	strName.MakeLower();
	// Make filter string
	if(bOpenFile)
	{
		//Make common filter fo template
		CString strFilterName;
		CString strFilterExt;
		strFilterName.Format(IDS_ALLFILTER_NAME_FORMAT, (LPCTSTR)strName);
//		strFilterName = strName + " (";
		if (!strExt.IsEmpty() && IsGoodExt(strExt,bOpenFile))
			strFilterExt = '*' + strExt + ';';
		// Append add-in filters extension
		for( i = 0; i  <manFilters.GetComponentCount(); i++ )
		{
			IUnknown *punk = manFilters.GetComponent( i );
			sptrIFileFilter	spFilter(punk);
			punk->Release();
			if (!__IsImageFilter(spFilter))
				continue;
			BSTR	bstrFilter, bstrExt;
			spFilter->GetFilterParams(&bstrExt, &bstrFilter, 0);
			strExt = bstrExt;
			strName = bstrFilter;
			::SysFreeString( bstrExt );
			::SysFreeString( bstrFilter );
			int	idx = 0;
			CString	strAll = strExt;
			while (idx != -1)
			{
				idx = strAll.Find( "\n" );
				CString	strExt;
				if( idx == -1 )	 
					strExt = strAll;
				else
				{
					strExt = strAll.Left( idx );
					strAll = strAll.Right( strAll.GetLength()-1-idx );
				}
				if (IsGoodExt(strExt,bOpenFile))
					strFilterExt += '*' + strExt + ';';
			}
		}
		// Complite filter string
		strFilterExt.Delete(strFilterExt.GetLength()-1);
		if (strFilterExt.Find(';') != -1)
		{
			strFilterName += strFilterExt + ")" + '\0' + strFilterExt + '\0'; 
			CString str = strFilter;
			strFilter = strFilterName;
			strFilter += str;
			if( pofn )
				pofn->nMaxCustFilter++;
		}
	}
	// Default extension will be one from document
	if( pstrDefault )
		*pstrDefault = strExt;
	// Append file suffix for "Save As" dialog
//	if(!bOpenFile)
	{
		AfxExtractSubString(strName, sDocString, CDocTemplate::filterName);
		AfxExtractSubString(strExt, sDocString, CDocTemplate::filterExt);
		if (!strExt.IsEmpty() && IsGoodExt(strExt,bOpenFile))
			_AppendFileSuffix(strFilter, strName, strExt);
	}

	for(i = 0; i < manFilters.GetComponentCount(); i++ )
	{
		IUnknown *punk = manFilters.GetComponent(i);
		sptrIFileFilter	spFilter(punk);
		punk->Release();
		if (!__IsImageFilter(spFilter))
			continue;
		BSTR	bstrFilter, bstrExt;
		spFilter->GetFilterParams( &bstrExt, &bstrFilter, 0 );
		strExt = bstrExt;
		strName = bstrFilter;
		::SysFreeString( bstrExt );
		::SysFreeString( bstrFilter );
		if(!bOpenFile)
		{
			int idx = strExt.Find("\n");
			if(idx >= 0)
				strExt = strExt.Left(idx);
		}
		if (IsGoodExt(strExt,bOpenFile))
		{
			_AppendFileSuffix(strFilter, strName, strExt);
			if( pofn )
				pofn->nMaxCustFilter++;
		}
	}
	return true;
}

static bool _CheckFile(CString strFilter, LPCTSTR lpstrFileName)
{
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(lpstrFileName, NULL, NULL, NULL, szExt);
	_tcslwr(szExt);
	strFilter.MakeLower();
	int n = strFilter.GetLength();
	LPTSTR lpstrBuf = strFilter.GetBuffer(-1);
	for (int i = 0; i < n; i++)
	{
		if (lpstrBuf[i] == 0)
			lpstrBuf[i] = _T(' ');
	}
	return strFilter.Find(szExt)!=-1;
}

bool DoPromptFileName(CString &fileName)
{
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
//	ofn.Flags = lFlags;
	CString strFilter;
	CString strDefault;
	__AppendFilterSuffix(strFilter, &ofn, &strDefault, true);
	CString allFilter;
	CString title;
	VERIFY(title.LoadString(IDS_OPEN_FILE));
	if( ::GetValueInt( ::GetAppUnknown(), "\\FileOpen", "UseShowAllFilter", 1 ) != 0 )
	{
		VERIFY(allFilter.LoadString(IDS_ALLFILTER));
		strFilter += allFilter + (TCHAR)'\0' + _T("*.*") + (TCHAR)'\0';
		ofn.nMaxCustFilter++;
	}
	ofn.lpstrFilter = strFilter;
	ofn.lpstrTitle = title;
	ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);
	ofn.nMaxFile = _MAX_PATH;
	BOOL res = ExecuteFileDialog(TRUE, ofn);
	fileName.ReleaseBuffer();
	if (!res) return false;
	if (!_CheckFile(strFilter, fileName))
	{
		CString sMsg;
		sMsg.Format(IDS_FILE_CAN_NOT_BE_OPENED, (LPCTSTR)fileName);
		AfxMessageBox(sMsg, MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}

bool DoOpenFile(LPCTSTR lpszPathName, IUnknown *pTarget, LPCTSTR lpstrName)
{
	_bstr_t	bstrFileName(lpszPathName);
	sptrIDocument sptrDoc(pTarget);
	sptrIDocumentSite sptrDS(pTarget);
	if (!bool(sptrDoc)) return false;
	if (!bool(sptrDS)) return false;
//	::FireEvent( GetAppUnknown(), szEventBeforeOpen, pTarget, 0, &_variant_t(bstrFileName) );
	if (sptrDoc->LoadNativeFormat(bstrFileName) == S_OK)
		return true;
	IUnknown *punk = FindDocTemplate(_T("Image"));
//	sptrDS->GetDocumentTemplate(&punk);
	if (!punk) return false;
	sptrIShellDocTemplate sptrSDT(punk);
	punk->Release();
	sptrSDT->GetFileFilterByFile(bstrFileName, &punk);
	if( !punk )	//It is native document format
	{
		sptrIFileDataObject	sptr( "Data.NamedData" );
		if( sptr == 0 )
			return false;
		if( sptr->LoadFile(bstrFileName) != S_OK )
			return false;
		_PreformCopy( INamedDataPtr(pTarget), INamedDataPtr(sptr), "\\", "\\", lpstrName); 
		return true;
	}
	sptrIFileFilter	sptrIFilter(punk);
	sptrIFileDataObject	sptrIFile(punk);
	punk->Release();
	sptrIFileDataObject	sptrD("Data.NamedData");
//	sptrINamedData	sptrD(pTarget);
	sptrIFilter->AttachNamedData(sptrD, 0); //initialize FileFilter
	bool bSucceded = sptrIFile->LoadFile(bstrFileName) == S_OK; //and read data
	sptrIFilter->AttachNamedData(0, 0);
	_PreformCopy(INamedDataPtr(pTarget), INamedDataPtr(sptrD), "\\", "\\", lpstrName); 
	if (!bSucceded)	return false;
	return true;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDriverFile, CDriverBase)

CDriverFile::CDriverFile()
{
	EnableAutomation();
	_OleLockApp( this );
	m_strDriverName.LoadString(IDS_FILE_SEQUENCE);
	m_dwFlags = 0;//FG_DOCUMENT;
}

CDriverFile::~CDriverFile()
{
	_OleUnlockApp( this );
}

BEGIN_MESSAGE_MAP(CDriverFile, CDriverBase)
	//{{AFX_MSG_MAP(CDriverFile)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDriverFile, CDriverBase)
	//{{AFX_DISPATCH_MAP(CDriverFile)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDriverFile to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {EB8A279D-DF06-4AFA-A95D-1A9E00338EAD}
static const IID IID_IDriverFile = 
{ 0xeb8a279d, 0xdf06, 0x4afa, { 0xa9, 0x5d, 0x1a, 0x9e, 0x0, 0x33, 0x8e, 0xad } };

BEGIN_INTERFACE_MAP(CDriverFile, CDriverBase)
	INTERFACE_PART(CDriverFile, IID_IDriverFile, Dispatch)
//	INTERFACE_PART(CDriverFile, IID_IInputPreview, InpPrv)
END_INTERFACE_MAP()

// {6067DC07-BA55-41BB-B699-F6FD93D8C9DE}
GUARD_IMPLEMENT_OLECREATE(CDriverFile, "IDriver.DriverFile", 0x6067dc07, 0xba55, 0x41bb, 0xb6, 0x99, 0xf6, 0xfd, 0x93, 0xd8, 0xc9, 0xde);

void CDriverFile::ParseFileName()
{
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(m_fileName, szDrive, szDir, szFName, szExt);
	int nLen = _tcslen(szFName);
	int nRad = nLen;
	while(nRad > 0 && _istdigit(szFName[nRad-1])) nRad--;
	int n = nRad<nLen?_ttoi(&szFName[nRad]):0;
	szFName[nRad] = 0;
	m_sDrive = szDrive;
	m_sDir = szDir;
	m_sFName = szFName;
	m_sExt = szExt;
	m_nFile = n+1;
}

void CDriverFile::MakeNextFileName(CString &fileName)
{
	fileName.Empty();
	TCHAR szPath[_MAX_PATH];
	for (int i = 0; i < 1000; i++)
	{
		CString s(m_sFName);
		TCHAR szNum[_MAX_FNAME];
		_itot(m_nFile, szNum, 10);
		s += CString(szNum);
		_tmakepath(szPath, m_sDrive, m_sDir, s, m_sExt);
		m_nFile++;
		int h = _topen(szPath, _O_RDONLY|_O_BINARY);
		if (h != -1)
		{
			_close(h);
			fileName = szPath;
			break;
		}
	}
}

void CDriverFile::OnInputImage(IUnknown *pUnk, IUnknown *punkTarget, int nDevice, BOOL bDialog)
{
	if (!m_fileName.IsEmpty()) _DoOpenFile(m_fileName, pUnk, punkTarget);
}

bool CDriverFile::DoPromptAndParse()
{
	bool b = DoPromptFileName(m_fileName);
	if (!b)
	{
		m_fileName.Empty();
		return false;
	}
	ParseFileName();
	return true;
}

bool CDriverFile::OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	if (nMode == 0) return true;
	if (bFirst)
		return DoPromptAndParse();
	else
	{
		MakeNextFileName(m_fileName);
		if (m_fileName.IsEmpty())
			return DoPromptAndParse();
/*		{
			AfxMessageBox(IDS_LAST_FILE_IN_SEQUENCE, MB_OK|MB_ICONEXCLAMATION);
			return false;
		}*/
	}
	return true;
}



/////////////////////////////////////////////////////////////////////////////
// CDriverFile message handlers


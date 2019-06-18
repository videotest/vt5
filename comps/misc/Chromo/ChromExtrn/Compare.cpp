#include "stdafx.h"
#include "Compare.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
#include "misc_utils.h"
#include "Classifiers.h"
#include "core5.h"
#include "docview5.h"
#include "stdio.h"
#include "ChildArray.h"
#include "Classes5.h"
#include "StdioFile.h"
#include "class_utils.h"


class CMessageException
{
public:
	_bstr_t m_bstrMessage;
	CMessageException(_bstr_t bstrMessage)
	{
		m_bstrMessage = bstrMessage;
	};
};

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
		long	dwPos = 0;

		ptrDataType->GetObjectFirstPosition( nType, &dwPos );

		while( dwPos )
		{
			punkObj = 0;
			ptrDataType->GetNextObject( nType, &dwPos, &punkObj );

			if (bstrObjectName.length() == 0 || ::GetName( punkObj ) == bstrObjectName)
			{
				IUnknownPtr sptr(punkObj);
				if (punkObj) punkObj->Release();
				return sptr;
			}

			INamedDataObject2Ptr	ptrN( punkObj );
			punkObj->Release();

			long	lpos = 0;
			ptrN->GetFirstChildPosition( &lpos );

			while( lpos )
			{
				IUnknown	*punkObject = 0;
				ptrN->GetNextChild( &lpos, &punkObject );

				if (bstrObjectName.length() == 0 || ::GetName( punkObject ) == bstrObjectName)
				{
					IUnknownPtr sptrO(punkObject);
					punkObject->Release();
					return sptrO;
				}

				punkObject->Release();


			}

			
		}
	}
	// Object not found. Throw exception if needed.
	if (bThrowException)
	{
		char szBuffer[500];
		_bstr_t bstrDocName = ::GetName(punkFrom);
		if (bstrDocName.length() <= 0)
			bstrDocName = "document";
		if (bstrTypeName.length() > 0)
			sprintf(szBuffer, "Object %s (%s) not found in %s", (const char *)bstrObjectName, (const char *)bstrTypeName,
				(const char *)bstrDocName);
		else
			sprintf(szBuffer, "Object %s not found in %s", (const char *)bstrObjectName, (const char *)bstrDocName);
		throw new CMessageException(szBuffer);
	}
	return 0;
}


IUnknownPtr _GetObjectByIID(IUnknown *punkFrom, REFIID iid, const char *pszDescr, bool bThrowException = true)
{
	sptrIDataTypeManager	ptrDataType( punkFrom );
	long	nTypesCounter = 0;
	ptrDataType->GetTypesCount( &nTypesCounter );
	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		IUnknown	*punkObj = 0;
		long	dwPos = 0;
		ptrDataType->GetObjectFirstPosition( nType, &dwPos );
		while( dwPos )
		{
			punkObj = 0;
			ptrDataType->GetNextObject( nType, &dwPos, &punkObj );
			IUnknown *punkInt;
			HRESULT hr = punkObj->QueryInterface(iid, (void **)&punkInt);
			if (SUCCEEDED(hr))
			{
				IUnknownPtr sptr(punkObj);
				if (punkInt) punkInt->Release();
				if (punkObj) punkObj->Release();
				return sptr;
			}

			INamedDataObject2Ptr	ptrN( punkObj );
			punkObj->Release();
			long	lpos = 0;
			ptrN->GetFirstChildPosition( &lpos );
			while( lpos )
			{
				IUnknown	*punkObject = 0;
				ptrN->GetNextChild( &lpos, &punkObject );
				IUnknown *punkInt;
				hr = punkObj->QueryInterface(iid, (void **)&punkInt);
				if (SUCCEEDED(hr))
				{
					IUnknownPtr sptrO(punkObject);
					if (punkInt) punkInt->Release();
					if (punkObject) punkObject->Release();
					return sptrO;
				}
				punkObject->Release();
			}
		}
	}
	// Object not found. Throw exception if needed.
	if (bThrowException)
	{
		char szBuffer[500];
		_bstr_t bstrDocName = ::GetName(punkFrom);
		if (bstrDocName.length() <= 0)
			bstrDocName = "document";
		if (pszDescr == NULL)
			pszDescr = "unknown";
		sprintf(szBuffer, "%s object not found in %s", pszDescr, (const char *)bstrDocName);
		throw new CMessageException(szBuffer);
	}
	return 0;
}

void _SearchObjectLists(IUnknownPtr &ObjLst1, IUnknownPtr &ObjLst2, _bstr_t &bstrFileName1, _bstr_t &bstrFileName2)
{
	bool bList1Found = false;
	_bstr_t bstrTypeObjectsList(szTypeObjectList);
	IApplicationPtr sptrApp(::GetAppUnknown());
	long lPosTempl,lPosDoc;
	sptrApp->GetFirstDocTemplPosition(&lPosTempl);
	while(lPosTempl)
	{
		sptrApp->GetFirstDocPosition(lPosTempl,&lPosDoc);
		while (lPosDoc)
		{
			IUnknownPtr sptrDoc;
			sptrApp->GetNextDoc(lPosTempl, &lPosDoc, &sptrDoc);
			bool bDoc = sptrDoc;
			if (bDoc)
			{
				IDocumentSitePtr sptrDS(sptrDoc);
				BSTR bstr;
				sptrDS->GetPathName(&bstr);
				IUnknownPtr sptrObjList = _GetObjectByName(sptrDoc, NULL, bstrTypeObjectsList, false);
				bool bObjList = sptrObjList;
				if (bObjList)
				{
					IFileDataObject2Ptr sptrFDO(sptrDoc);
					if (sptrFDO != 0)
					{
						sptrFDO->SetModifiedFlag(false);
					}
					if (bList1Found)
					{
						ObjLst2 = sptrObjList;
						bstrFileName2 = bstr;
						return ;//true;
					}
					else
					{
						ObjLst1 = sptrObjList;
						bstrFileName1 = bstr;
						bList1Found = true;
					}
				}
				::SysFreeString(bstr);
			}
		}
		BSTR bstrTemplName = NULL;
		IUnknown *punkTempl = NULL;
		sptrApp->GetNextDocTempl(&lPosTempl,&bstrTemplName,&punkTempl);
		if (bstrTemplName)
			::SysFreeString(bstrTemplName);
		if (punkTempl)
			punkTempl->Release();
	}
	// Object not found. Throw exception if needed.
	throw new CMessageException("Files to compare not found");
};

const int MaxClass = 24;
static int g_nErrors;
static int g_nObjects;
static int g_anClsErrors[MaxClass];
static int g_anClsObjects[MaxClass];
static int g_annClsCfied[MaxClass][MaxClass];

static void _make_detailed_message(char *szBuff, int nErrors, int *anErroneous)
{
	if (nErrors == 0)
		sprintf(szBuff, "%d errors", nErrors);
	else
		sprintf(szBuff, "%d errors \n (", nErrors);
	for (int i = 0; i < nErrors; i++)
	{
		char b[20];
		if (i < nErrors-1)
			sprintf(b, "%d,", anErroneous[i]+1);
		else
			sprintf(b, "%d)", anErroneous[i]+1);
		strcat(szBuff, b);
	}
};

_ainfo_base::arg	CActionCompareInfo::s_pargs[] = 
{
	{"Silent",szArgumentTypeInt, 0, true, false },
	{"Mode",szArgumentTypeInt, 0, true, false },
	{0, 0, 0, false, false },
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionClassify
////////////////////////////////////////////////////////////////////////////////////////////////////
CActionCompare::CActionCompare()
{
}

HRESULT CActionCompare::DoInvoke()
{
	BOOL bMessages = ::GetValueInt(::GetAppUnknown(), "Classification", "Messages", 0);
	try
	{
		int bSilent = GetArgLong("Silent");
		int nMode = GetArgLong("Mode");
		if (nMode == 1)
		{
			g_nErrors = g_nObjects = 0;
			memset(g_anClsErrors, 0, MaxClass*sizeof(int));
			memset(g_anClsObjects, 0, MaxClass*sizeof(int));
			memset(g_annClsCfied, 0, MaxClass*MaxClass*sizeof(int));
			unlink("c:\\chromres.txt");
			return S_OK;
		}
		else if (nMode == 2)
		{
			double dProc = 100.*g_nErrors/g_nObjects;
			char szBuffer[255];
			sprintf(szBuffer, "%d errors from %d (%.1f%%)", g_nErrors, g_nObjects, dProc);
			MessageBox(NULL, szBuffer, "ChromExtrn", MB_OK);
			_StdioFile file("c:\\chromres.txt", "at");
			fprintf(file.f, "%s\n", szBuffer);
			for (int i = 0; i < MaxClass; i++)
			{
				double dProc = 100.*double(g_anClsErrors[i])/double(g_anClsObjects[i]);
				fprintf(file.f, "Class %d : %d errors / %d, %.1f%%\n", i+1, g_anClsErrors[i],
					g_anClsObjects[i], dProc);
			}
			fprintf(file.f, "\n\n");
			fprintf(file.f, "Class ");
			for (int j = 0; j < MaxClass; j++)
			{
				fprintf(file.f, "%3d", j+1);
			}
			fprintf(file.f, "\n");
			for (i = 0; i < MaxClass; i++)
			{
				fprintf(file.f, "%3d   ", i+1);
				for (int j = 0; j < MaxClass; j++)
				{
					if (g_annClsCfied[i][j])
						fprintf(file.f, "%3d", g_annClsCfied[i][j]);
					else
						fprintf(file.f, "  .");
				}
				fprintf(file.f, "%3d\n", g_anClsObjects[i]);
			}
			return S_OK;
		}
		_StdioFile file("c:\\chromres.txt", "at");
		IUnknownPtr ObjLst1, ObjLst2;
		_bstr_t bstrFName1,bstrFName2;
		_SearchObjectLists(ObjLst1, ObjLst2, bstrFName1, bstrFName2);
		fprintf(file.f, "Files: \"%s\" and \"%s\"\n", (const char *)bstrFName1, (const char *)bstrFName2);
		sptrINamedDataObject2 sptrN1(ObjLst1); // Object list
		sptrINamedDataObject2 sptrN2(ObjLst2); // Object list
		long lObjects1,lObjects2; // Number of objects
		sptrN1->GetChildsCount(&lObjects1);
		sptrN2->GetChildsCount(&lObjects2);
		if (lObjects1 != lObjects2)
			throw new CMessageException("Different sizes of objects");
		_ptr_t2<int> anErroneous(lObjects1);
		int nErrors = 0;
		long pos1 = 0, pos2 = 0;
		int iObj = 0;
		sptrN1->GetFirstChildPosition((long*)&pos1);
		sptrN2->GetFirstChildPosition((long*)&pos2);
		fprintf(file.f, "Errors:");
		while (pos1 && pos2)
		{
			IUnknownPtr sptr1,sptr2;
			ICalcObjectPtr sptrObj1,sptrObj2;
			long pos1s = pos1, pos2s = pos2;
			sptrN1->GetNextChild((long*)&pos1, &sptr1);
			sptrN2->GetNextChild((long*)&pos2, &sptr2);
			sptrObj1 = sptr1;
			sptrObj2 = sptr2;
			bool bOk1 = sptrObj1;
			bool bOk2 = sptrObj2;
			if (bOk1 && bOk2)
			{
				int nClass1 = get_object_class( sptrObj1 );
				int nClass2 = get_object_class( sptrObj2 );
				if (nClass1 != nClass2)
				{
					anErroneous[nErrors++] = iObj;
					if (nClass2 >= 0 && nClass2 < MaxClass)
						g_anClsErrors[nClass2]++;
					fprintf(file.f, "%d(%d,%d)", iObj+1, nClass1+1, nClass2+1);
				}
				if (nClass2 >= 0 && nClass2 < MaxClass)
				{
					g_anClsObjects[nClass2]++;
					if (nClass1 >= 0 && nClass1 < MaxClass)
						g_annClsCfied[nClass2][nClass1]++;
				}
			}
			else
			{
				char szBuff[255];
				sprintf(szBuff, "Strange object %d", iObj);
				MessageBox(NULL, szBuff, "ChromExtrn", MB_OK);
			}
			iObj++;
		}
		fprintf(file.f, "\n%d errors\n", nErrors);
		if (!bSilent)
		{
			char szBuff[255];
//			sprintf(szBuff, "%d errors", nErrors);
			_make_detailed_message(szBuff, nErrors, anErroneous);
			MessageBox(NULL, szBuff, "ChromExtrn", MB_OK);
		}
		g_nErrors += nErrors;
		g_nObjects += lObjects1;
	}
	catch(CMessageException *e)
	{
		if (bMessages)
			::MessageBox(NULL, e->m_bstrMessage, "Classification", MB_OK|MB_ICONEXCLAMATION);
	}
	return S_OK;
}
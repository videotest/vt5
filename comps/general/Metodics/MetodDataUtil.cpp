// ������� ��� ������������� � ���������:
// ���������� ��������� (���� �������� ����/��������) � NamedData

#include "StdAfx.h"
#include "\vt5\awin\misc_dbg.h"
#include "\vt5\awin\misc_map.h"
#include "MethodDataUtil.h"
#include "MethodStep.h"
#include "method_int.h"
#include "mtd_utils.h"
#include "TreeFilter.h"
#include <fstream>

inline long cmp_guid( const GuidKey guid1, const GuidKey guid2 )
{
	return memcmp( &guid1, &guid2, sizeof( GUID ) );
}

//---------------------------------------------------------------------
// ������� ��� ���������� shell.data

bool CompareWildcard(const char* szName, const char* &szWild)
{	// �������� ���� � named data � ��������
	// true, ���� szName ��������� � �������� ��� ���������
	// ������ ������-�������
	// � ������� ��������� ������ '*' - ������������� ������ ����������
	// �������� �� ����� ������ ��� �� ������� '\'
	// �������:
	// \measurement\parameters\Orientation\UserName �������� ��� �����
	// \measurement\parameters\Orientation\UserName
	// \measurement\parameters\Orientation\User*
	// \measurement\*\*\UserName
	// \measurement
	// � �� �������� ��� ����� \measurement\*\UserName
	if(!szWild) return false;
	while(*szWild!=0 && *szWild!='\n' && *szWild!='\r')
	{
		if(*szWild=='*')
		{ // * - ��������� �� ���������� ����� ��� ����� ������
			while(*szName!='\\' && *szName!=0) szName++;
		}
		else
		{
			if(*szName!=*szWild)
			{
				while(*szWild!=0 && *szWild!='\n' && *szWild!='\r') szWild++;
				szWild++;
				return false;
			}
			szName++;
		}
		szWild++;
	}
	return true;
}

bool CompareWildcards(const char* szName, const char* szWild)
{	// �������� ���� � named data � ���������
	// ������ ������ � ��������� ������, ������ ��������� CR �/��� LF
	// true, ���� szName ��������� � ����� �� �������� ��� ���������
	// ������ ������-�������
	if(!szWild) return false;
	while(1)
	{
		while(*szWild=='\n' || *szWild=='\r') szWild++; // ��������� ������� ����� ������

		if(*szWild==0) return false; // ���� ��� � �� ����� ����������� - ���

		const char *szName1 = szName;
		if(CompareWildcard(szName1, szWild)) return true;
	}
}

//---------------------------------------------------------------------
// ������� ��� ����������/�������������� ��������� ��������� (����)

typedef struct
{
	long num;
	long parent_num;
} _pair_t;

inline long cmp_pair( _pair_t p1, _pair_t p2 )
{
	if( p2.parent_num>p1.parent_num ) return 1;
	if( p2.parent_num<p1.parent_num ) return -1;
	if( p2.num>p1.num ) return 1;
	if( p2.num<p1.num ) return -1;
	return 0;
}

void StoreSelection(IUnknown* punkDoc, IUnknown* punkView, const char *szSection, IUnknown* punkNamedData, bool bSimple = false)
{	// ��������, ����� ���� ����������� ������� � ���������
	::DeleteEntry(punkNamedData,szSection); // ����� ������� ������, ���� ������� �� ��������

	IDataContext2Ptr sptrDC(punkView); // �������� �����
	IDataContext2Ptr sptrDocDC(punkDoc); // �������� ���������
	if(sptrDC==0 || sptrDocDC==0) return; // ������

	long nTypeCount = 0;
	sptrDC->GetObjectTypeCount(&nTypeCount);

	for (long nType = 0; nType < nTypeCount; nType++)
	{	// for all types
		BSTR strType = 0;
		if (FAILED(sptrDC->GetObjectTypeName(nType, &strType)) || !strType)
			continue;
		_bstr_t bstrType(strType,false);

        // �������� ���� - ������� �� ������ ������� ������� ������ � IUnknownPtr
		_list_map_t<IUnknownPtr, _pair_t, cmp_pair> mapNumberToUnknownPtr;
		{
			long lPos = 0;
			sptrDocDC->GetFirstObjectPos( bstrType, &lPos );
			while( lPos )
			{
				_pair_t pr={-1,-1};

				IUnknownPtr ptr = 0;
				sptrDocDC->GetNextObject( bstrType, &lPos, &ptr );
				INumeredObjectPtr sptrNum(ptr);
				if(sptrNum!=0) sptrNum->GetCreateNum(&pr.num);

				INamedDataObject2Ptr sptrNDO(ptr);
				IUnknownPtr ptrParent = 0;
				if(sptrNDO) sptrNDO->GetParent(&ptrParent);
				INumeredObjectPtr sptrNumParent(ptrParent);
				if(sptrNumParent!=0) sptrNumParent->GetCreateNum(&pr.parent_num);

				// ������������� ��������:
				if(pr.parent_num==-1)
				{
					pr.parent_num = pr.num;
					pr.num = -1; // ��� ����, ����� ������ �� ��������� ��� ���������
					//pr.num = INT_MAX; // ����� ������ �� ��������� ��� ������
				}
				mapNumberToUnknownPtr.set(ptr, pr);
			}
		}

		// ������ �������� ���� - ������� �� long(punk) � ������ ������� ������� ������
		_map_t<long, long, cmp_long> mapUnkToIndex;
		{
			long nIndex=0;
			long nParentIndex=0, nSubIndex=0;
			long nPrevParentNum=-1;
			for(long lPos = mapNumberToUnknownPtr.tail(); lPos!=0; lPos = mapNumberToUnknownPtr.prev(lPos))
			{
				IUnknown *punk = mapNumberToUnknownPtr.get(lPos);
				_pair_t pr = mapNumberToUnknownPtr.get_key(lPos);

				//mapUnkToIndex.set(nIndex, long(punk));

				//if( pr.num == INT_MAX )
				if( pr.parent_num != nPrevParentNum )
				{
					nParentIndex++;
					nSubIndex = 0;
					nPrevParentNum = pr.parent_num;
				}

				mapUnkToIndex.set(nParentIndex*1000+nSubIndex, long(punk));
				nSubIndex++;

				nIndex++;
			}
		}

		// TODO: � simple ������ ��������� ����� ���� ������ - #1000.

		long lPos = 0;
		sptrDC->GetFirstSelectedPos( bstrType, &lPos );
		long nCount=0; // ������� ������������� ��������
		while( lPos )
		{
			IUnknown* punk = 0;
			if(bSimple)
			{ // ��� ���������� �������� ������ ������ ��������� �������� ����� 1 ������� - ��������� ��������� (� �������� 1000)
				long lPos1 = mapNumberToUnknownPtr.tail();
				if(lPos1)
					punk = mapNumberToUnknownPtr.get(lPos1);
				if(punk) punk->AddRef();
				lPos = 0; // �� ����� ������ ��������
			}
			else
			{
				sptrDC->GetNextSelected( bstrType, &lPos, &punk );
			}

			long lMapPos = mapUnkToIndex.find(long(punk));

			GuidKey base_key;
			INamedDataObject2Ptr sptrNDO(punk);
			if(sptrNDO != 0) sptrNDO->GetBaseKey(&base_key);
			{ // ���� ���� parent - ������� ��� Base Key, �� ����
				IUnknownPtr ptrParent = 0;
				if(sptrNDO) sptrNDO->GetParent(&ptrParent);
				INamedDataObject2Ptr sptrNDOParent(ptrParent);
				if(sptrNDOParent!=0) sptrNDOParent->GetBaseKey(&base_key);
			}

			if(punk!=0) punk->Release();

			if(lMapPos!=0) // �������� �� ������ ������
			{
				long lNum = mapUnkToIndex.get(lMapPos);
				CString strName = (char*)(bstrType);
				strName.AppendFormat("_%u",unsigned(nCount));
				::SetValue(punkNamedData,szSection,strName, lNum);

				CString strBaseKey;
				strBaseKey.Format("%I64X-%I64X", lo_guid(&base_key), hi_guid(&base_key) );
				::SetValue(punkNamedData,szSection,strName+"_base", strBaseKey);

				nCount++;
			}
		}
		CString strName = CString((char*)(bstrType)) + "_count";
		::SetValue(punkNamedData,szSection,strName, nCount);
	}
}

void StoreSelection(const char *szSection, IUnknown* punkNamedData, bool bSimple = false)
{	// ��������, ����� ���� ����������� ������� � ��������� �������� �����
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	IUnknownPtr ptrDoc;
	if(ptrApp) ptrApp->GetActiveDocument(&ptrDoc);
	IDocumentSitePtr sptrDoc(ptrDoc);
	IUnknownPtr ptrView;
	if(sptrDoc!=0) sptrDoc->GetActiveView(&ptrView);
	StoreSelection(ptrDoc, ptrView, szSection, punkNamedData, bSimple);
}

void SelectLastImage(IUnknown* punkDoc, IUnknown* punkView)
{

}

void LoadSelection(IUnknown* punkDoc, IUnknown* punkView, const char *szSection, IUnknown* punkNamedData)
{	// ������������ ��� ������ �������
	IDataContext2Ptr sptrDC(punkView); // �������� �����
	IDataContext2Ptr sptrDocDC(punkDoc); // �������� ���������
	if(sptrDC==0 || sptrDocDC==0) return; // ������

	long nTypeCount = 0;
	sptrDC->GetObjectTypeCount(&nTypeCount);

	// �������� ������ �����
	smart_alloc(types, _bstr_t, nTypeCount);

	for (long nType = 0; nType < nTypeCount; nType++)
	{	// for all types
		BSTR strType = 0;
		types[nType] = "";
		sptrDC->GetObjectTypeName(nType, &strType);
		if(strType!=0)
			types[nType] = _bstr_t(strType,false);
		if(types[nType]==_bstr_t("Image"))
		{ // ���� ���������� �� "Image" - ������ ��� � ������ ������
			types[nType] = types[0];
			types[0] = "Image";
		}
	}

	// ����������� ���, ��� �����
	for (long nType = 0; nType < nTypeCount; nType++)
		sptrDC->UnselectAll(types[nType]);
	
	// ���� � ���� �������� BaseKey, ���������� � ��������, ��������� - ������ ��������� � ��������
	_map_t<GuidKey, GuidKey, cmp_guid> mapMethodBaseKeyToBaseKey;
	long nSelectionError = 0;

	for (long nType = 0; nType < nTypeCount; nType++)
	{	// for all types
		// ��� ������ ������� ��� ���������
		sptrDC->UnselectAll(types[nType]);

		// ������ ���������� �������

		// �������� ���� - ������� �� ������ ������� ������� ������ � IUnknownPtr
		_list_map_t<IUnknownPtr, _pair_t, cmp_pair> mapNumberToUnknownPtr;
		{
			long lPos = 0;
			sptrDocDC->GetFirstObjectPos( types[nType], &lPos );
			while( lPos )
			{
				_pair_t pr={-1,-1};

				IUnknownPtr ptr = 0;
				sptrDocDC->GetNextObject( types[nType], &lPos, &ptr );
				INumeredObjectPtr sptrNum(ptr);
				if(sptrNum!=0) sptrNum->GetCreateNum(&pr.num);

				INamedDataObject2Ptr sptrNDO(ptr);
				IUnknownPtr ptrParent = 0;
				if(sptrNDO) sptrNDO->GetParent(&ptrParent);
				INumeredObjectPtr sptrNumParent(ptrParent);
				if(sptrNumParent!=0) sptrNumParent->GetCreateNum(&pr.parent_num);

				// ������������� ��������:
				if(pr.parent_num==-1)
				{
					pr.parent_num = pr.num;
					pr.num = -1; // ��� ����, ����� ������ �� ��������� ��� ���������
					//pr.num = INT_MAX; // ����� ������ �� ��������� ��� ������
				}

				mapNumberToUnknownPtr.set(ptr, pr);
			}
		}

		// ������ �������� ���� - ������� �� ������� � ������ ������� ������
		_map_t<IUnknownPtr, long, cmp_long> mapIndexToUnknownPtr;
		{
			long nIndex=0;
			long nParentIndex=0, nSubIndex=0;
			long nPrevParentNum=-1;
			for(long lPos = mapNumberToUnknownPtr.tail(); lPos!=0; lPos = mapNumberToUnknownPtr.prev(lPos))
			{
				IUnknown *punk = mapNumberToUnknownPtr.get(lPos);
				_pair_t pr = mapNumberToUnknownPtr.get_key(lPos);

				//mapUnkToIndex.set(nIndex, long(punk));

				//if( pr.num == INT_MAX )
				if( pr.parent_num != nPrevParentNum )
				{
					nParentIndex++;
					nSubIndex = 0;
					nPrevParentNum = pr.parent_num;
				}

				mapIndexToUnknownPtr.set(punk, nParentIndex*1000+nSubIndex);
				nSubIndex++;

				nIndex++;
			}
		}

		CString strName = CString((char*)(types[nType])) + "_count";
		long nSelectedCount = ::GetValueInt(punkNamedData,szSection,strName);
		for(long nCount=0; nCount<nSelectedCount; nCount++)
		{
			CString strName = (char*)(types[nType]);
			strName.AppendFormat("_%u",unsigned(nCount));
			long index = ::GetValueInt(punkNamedData,szSection,strName, -1);
			long lpos = mapIndexToUnknownPtr.find(index);
			if(lpos)
			{
				IUnknownPtr ptrSel=mapIndexToUnknownPtr.get(lpos);

				// ��������, ��������� �� ����
				GuidKey base_key;
				INamedDataObject2Ptr sptrNDO(ptrSel);
				if(sptrNDO != 0) sptrNDO->GetBaseKey(&base_key);
				{ // ���� ���� parent - ������� ��� Base Key, �� ����
					IUnknownPtr ptrParent = 0;
					if(sptrNDO) sptrNDO->GetParent(&ptrParent);
					INamedDataObject2Ptr sptrNDOParent(ptrParent);
					if(sptrNDOParent!=0) sptrNDOParent->GetBaseKey(&base_key);
				}
				CString strBaseKey;
				strBaseKey.Format("%I64X-%I64X", lo_guid(&base_key), hi_guid(&base_key) );

				CString strMethodBaseKey = 
					::GetValueString(punkNamedData,szSection,strName+"_base", "0000000000000000-0000000000000000");
				GuidKey method_base_key;
				::sscanf( strMethodBaseKey.GetBuffer(), "%I64X-%I64X",
					&lo_guid(&method_base_key), &hi_guid(&method_base_key) );

				long lpos_guid = mapMethodBaseKeyToBaseKey.find(method_base_key);
				if(lpos_guid)
				{
					GuidKey base_key2 = 
						mapMethodBaseKeyToBaseKey.get(lpos_guid);
					if( base_key != base_key2 )
					{ // ������, ����! ���� ������ ��������� - � �� ���������
						nSelectionError = 2;
						continue;
					}
				}
				else
				{ // �������� ������������ � ����
					mapMethodBaseKeyToBaseKey.set(base_key, method_base_key);
				}
				sptrDC->SetObjectSelect(ptrSel,true);
			}
			else
			{ // ������ - ������ ���������
				nSelectionError = 1;
				continue;
			}
		}
	}
	if(nSelectionError)
	{
		_variant_t vars[1] = { nSelectionError };
		::FireScriptEvent("OnSelectionError", 1, vars);
	}
}

void LoadSelection(const char *szSection, IUnknown* punkNamedData)
{	// ������������ ��� ������ ������� � �������� �����
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	IUnknownPtr ptrDoc;
	if(ptrApp) ptrApp->GetActiveDocument(&ptrDoc);
	IDocumentSitePtr sptrDoc(ptrDoc);
	IUnknownPtr ptrView;
	if(sptrDoc!=0) sptrDoc->GetActiveView(&ptrView);
	LoadSelection(ptrDoc, ptrView, szSection, punkNamedData);
}

void StoreViewProgID(IUnknown* punkView, const char *szSection, IUnknown* punkNamedData)
{ // �������� ���� � �����
	CString strViewProgID = "";
	IPersistPtr sptrPersistView(punkView);
	if(sptrPersistView)
	{
		CLSID clsidView = INVALID_KEY;
		sptrPersistView->GetClassID(&clsidView);

		BSTR bstrProgID = 0;
		::ProgIDFromCLSID( clsidView, &bstrProgID );
		strViewProgID = bstrProgID;
		::CoTaskMemFree( bstrProgID );	bstrProgID = 0;
	}
	//AfxMessageBox(strViewProgID);
	::SetValue(punkNamedData,szSection,"ActiveViewProgID", strViewProgID);
	// ��������� ProgID View � shell.data - ������ ���������� �����
	// ������� ��������� ��������� shell.data � ��������� ��� �����
}

void StoreViewProgID(const char *szSection, IUnknown* punkNamedData)
{ // �������� ���� �� �������� �����
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	IUnknownPtr ptrDoc;
	if(ptrApp) ptrApp->GetActiveDocument(&ptrDoc);
	IDocumentSitePtr sptrDoc(ptrDoc);
	IUnknownPtr ptrView;
	if(sptrDoc!=0) sptrDoc->GetActiveView(&ptrView);
	StoreViewProgID(ptrView, szSection, punkNamedData);
}

void LoadViewProgID(IUnknown* punkSplitter, int row, int col, const char *szSection, IUnknown* punkNamedData)
{ // ������������ ����� �� shell.data
	_bstr_t bstrViewName = ::GetValueString( punkNamedData,szSection,"ActiveViewProgID", "");
	if( bstrViewName == _bstr_t("") ) return; // �� ����������
	ISplitterWindowPtr sptrSplitter(punkSplitter);
	if(sptrSplitter!=0)
		sptrSplitter->ChangeViewType(row, col, bstrViewName);
}

void LoadViewProgID(const char *szSection, IUnknown* punkNamedData)
{ // ������������ �������� ����� �� shell.data
	_bstr_t bstrViewName = ::GetValueString( punkNamedData,szSection,"ActiveViewProgID", "");
	if( bstrViewName != _bstr_t("") )
		::ExecuteScript( _bstr_t("ShowView \"") + bstrViewName + "\"" );
}

void StoreScrollZoom(IUnknown* punkView, const char *szSection, IUnknown* punkNamedData)
{ // �������� ���� � �����
	double fZoom=1;
	POINT ptPos={0,0};
	IScrollZoomSitePtr sptrSZ(punkView);
	if(sptrSZ)
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&ptPos);
	}
	::SetValue(punkNamedData,szSection,"ViewZoom", fZoom);
	::SetValue(punkNamedData,szSection,"ViewPosX", ptPos.x);
	::SetValue(punkNamedData,szSection,"ViewPosY", ptPos.y);
}

void StoreScrollZoom(const char *szSection, IUnknown* punkNamedData)
{ // �������� ���� (zoom,pos) �� �������� �����
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	IUnknownPtr ptrDoc;
	if(ptrApp) ptrApp->GetActiveDocument(&ptrDoc);
	IDocumentSitePtr sptrDoc(ptrDoc);
	IUnknownPtr ptrView;
	if(sptrDoc!=0) sptrDoc->GetActiveView(&ptrView);
	StoreScrollZoom(ptrView, szSection, punkNamedData);
}

void LoadScrollZoom(IUnknown* punkView, const char *szSection, IUnknown* punkNamedData)
{ // ������������ zoom/pos ����� �� shell.data
	double fZoom = ::GetValueDouble(punkNamedData,szSection,"ViewZoom", 1.0);
	POINT ptPos={0,0};
	ptPos.x = ::GetValueInt(punkNamedData,szSection,"ViewPosX", 0);
	ptPos.y = ::GetValueInt(punkNamedData,szSection,"ViewPosY", 0);
	IScrollZoomSitePtr sptrSZ(punkView);
	if(sptrSZ)
	{
		sptrSZ->SetZoom(fZoom);
		sptrSZ->SetScrollPos(ptPos);
	}
}

void StoreSplitter(const char *szSection, IUnknown* punkNamedData, bool bSimple)
{ // ��������� �������� �������
	::DeleteEntry(punkNamedData,szSection); // ����� ������� ������, ���� ������� �� ��������

	IApplicationPtr sptrApp( ::GetAppUnknown() );
	IUnknownPtr ptrDoc;
	if( sptrApp ) sptrApp->GetActiveDocument(&ptrDoc);
	IDocumentSitePtr sptrDoc(ptrDoc);
	IUnknownPtr ptrView;
	if(sptrDoc!=0) sptrDoc->GetActiveView(&ptrView);
	IViewSitePtr sptrViewSite(ptrView);
	IUnknownPtr ptrFrame;
	if(sptrViewSite!=0) sptrViewSite->GetFrameWindow(&ptrFrame);
	IFrameWindowPtr sptrFrameWindow(ptrFrame);
	IUnknownPtr ptrSplitter;
	if(sptrFrameWindow!=0) sptrFrameWindow->GetSplitter(&ptrSplitter);
	ISplitterWindowPtr sptrSplitter(ptrSplitter);
	if(sptrSplitter!=0)
	{
		int nRows=0, nCols=0;
		sptrSplitter->GetRowColCount( &nRows, &nCols );
		::SetValue(punkNamedData,szSection,"SplitterRows", long(nRows));
		::SetValue(punkNamedData,szSection,"SplitterCols", long(nCols));

		int nActiveRow=0, nActiveCol=0;
		sptrSplitter->GetActivePane(&nActiveRow, &nActiveCol);
		::SetValue(punkNamedData,szSection,"ActiveRow", long(nActiveRow));
		::SetValue(punkNamedData,szSection,"ActiveCol", long(nActiveCol));

		for(int row=0; row<nRows; row++)
		{
			for(int col=0; col<nCols; col++)
			{
				IUnknownPtr ptrView1;
				sptrSplitter->GetViewRowCol(row, col, &ptrView1);
				if(ptrView1==0) continue;
                CString s = szSection;
                CString s1;
				s1.Format("\\View_%i_%i", row, col);
				s += s1;
				StoreSelection(ptrDoc, ptrView1, s, punkNamedData, bSimple); // ��������, ����� ���� ����������� ������� � ���������
				StoreViewProgID(ptrView1, s, punkNamedData); // �������� �����
				StoreScrollZoom(ptrView1, s, punkNamedData);
			}
		}
	}
}

void LoadSplitter(const char *szSection, IUnknown* punkNamedData, DWORD dwFlags)
{ // ������������ �������� �������
	bool bSwitchViews = (dwFlags & lssSwitchViews) != 0; // ����������� �����?
	bool bActivateObjects = (dwFlags & lssActivateObjects) != 0; // �������������� ������� � ���������?
	bool bRestoreZoom = (dwFlags & lssRestoreZoom) != 0; // ��������������� zoom � scroll pos ����.

	ISplitterWindowPtr sptrSplitter;
	IApplicationPtr sptrApp( ::GetAppUnknown() );
	IUnknownPtr ptrDoc;
	if( sptrApp ) sptrApp->GetActiveDocument(&ptrDoc);
	IDocumentSitePtr sptrDoc(ptrDoc);
	{
		IUnknownPtr ptrView;
		if(sptrDoc!=0) sptrDoc->GetActiveView(&ptrView);
		IViewSitePtr sptrViewSite(ptrView);
		IUnknownPtr ptrFrame;
		if(sptrViewSite!=0) sptrViewSite->GetFrameWindow(&ptrFrame);
		IFrameWindowPtr sptrFrameWindow(ptrFrame);
		IUnknownPtr ptrSplitter;
		if(sptrFrameWindow!=0) sptrFrameWindow->GetSplitter(&ptrSplitter);
		sptrSplitter = ptrSplitter;
	}	// ���������� �� ��������� - ������ ������� ����, ���� �������� ������ ����������

	if( sptrSplitter==0 ) return;

	int nRows = ::GetValueInt(punkNamedData,szSection,"SplitterRows", 0);
	int nCols = ::GetValueInt(punkNamedData,szSection,"SplitterCols", 0);

	int nActiveRow = ::GetValueInt(punkNamedData,szSection,"ActiveRow", 0);
	int nActiveCol = ::GetValueInt(punkNamedData,szSection,"ActiveCol", 0);

	if(dwFlags & lssActiveViewOnly)
	{ // ���� ���� ������� ������ �������� �����
		int row=0, col=0;
		sptrSplitter->GetActivePane(&row, &col);

		CString s = szSection;
		CString s1;
		s1.Format("\\View_%i_%i", nActiveRow, nActiveCol); // ���� ����� �� ������, ��������������� "���������" �� shell.data
		s += s1;
		if( bSwitchViews )
			LoadViewProgID(sptrSplitter, row, col, s, punkNamedData); // ����������� �����
		IUnknownPtr ptrView1;
		sptrSplitter->GetViewRowCol(row, col, &ptrView1);
		if(ptrView1==0) return;
		if( bActivateObjects )
			LoadSelection(ptrDoc, ptrView1, s, punkNamedData); // ����������� selection � ��������� �����
		if( bRestoreZoom )
			LoadScrollZoom(ptrView1, s, punkNamedData);
		return;
	}

	// � ��� ��� ��� "�����������" ������ - ����� ���� ������������ ��� �����

	// ���� �������� �� ������� - "����������"
	int nRows0=0, nCols0=0;
	sptrSplitter->GetRowColCount( &nRows0, &nCols0 );
	if(nRows0!=nRows || nCols0!=nCols)
		sptrSplitter->Split( nRows, nCols );

	for(int row=0; row<nRows; row++)
	{
		for(int col=0; col<nCols; col++)
		{
			CString s = szSection;
			CString s1;
			s1.Format("\\View_%i_%i", row, col);
			s += s1;
			if( bSwitchViews )
				LoadViewProgID(sptrSplitter, row, col, s, punkNamedData); // ����������� �����
			IUnknownPtr ptrView1;
			sptrSplitter->GetViewRowCol(row, col, &ptrView1);
			if(ptrView1==0) continue;
			if( bActivateObjects )
				LoadSelection(ptrDoc, ptrView1, s, punkNamedData); // ����������� selection � ��������� �����
			if( bRestoreZoom )
				LoadScrollZoom(ptrView1, s, punkNamedData);
		}
	}

	IUnknownPtr ptrView1;
	sptrSplitter->GetViewRowCol( nActiveRow, nActiveCol, &ptrView1 );
	sptrDoc->SetActiveView( ptrView1 );

	HWND	hwndSplitter = 0;
	sptrIWindow	sptrWSplitter( sptrSplitter );
	if(sptrWSplitter) sptrWSplitter->GetHandle( (HANDLE*)&hwndSplitter );

	// 19.10.2005 build 92
	// BT 4679 - �� ������������ ���� � ���������
	HWND	hwnd = 0;
	sptrIWindow	sptrW( ptrView1 );
	if(sptrW) sptrW->GetHandle( (HANDLE*)&hwnd );
	while(hwnd && hwnd!=hwndSplitter)
	{ // ��������� ������ �����, ��� ������� � ��� ����� - ���� �� ������� � ��������
		::SetFocus(hwnd);
		hwnd = GetParent(hwnd);
	}
}

// ����������: ������������� ���, ����� pszDisabledKeys, �� ����� NamedData
// � ������; ��� ���� ��������� ��� ���� � ������������ � pSubstMap (���
// ������, ������������ � ������ pSubstMap �������� �� �������� pSubstMap).
bool RecursiveAddEntry( INamedData* ptrDst, INamedData* ptrSrc, LPCSTR pszEntry,
					   char* pszDisabledKeys, CSubstMap* pSubstMap )
{
	if( !ptrSrc || !pszEntry )
		return false;

	_bstr_t bstrPath( pszEntry );

	// set section
	if( S_OK != ptrSrc->SetupSection( bstrPath ) )
		return false;

	// get count of entries
	long EntryCount = 0;
	ptrSrc->GetEntriesCount( &EntryCount );

	// for all entries
	for( int i = 0; i<(int)EntryCount; i++ )
	{
		_bstr_t bstrEntryName;
		ptrSrc->GetEntryName( i, bstrEntryName.GetAddress() );

		if(bstrEntryName.length()==0) continue; // ������ ����� ��������, � � ������? ������!
		
		// format a full path for entry
		_bstr_t bstrPathNew;
		int nLen = bstrPath.length();

		if( nLen > 0 && ((char*)bstrPath)[nLen-1] == '\\' )
			bstrPathNew = bstrPath + bstrEntryName;
		else
			bstrPathNew = bstrPath + "\\" + bstrEntryName;

		if(!CompareWildcards(bstrPathNew, pszDisabledKeys))
		{
			// get value
			_variant_t var;
			ptrSrc->GetValue( bstrPathNew, &var );

			// �������� �� ������������ �������� �� ������
			if( VT_BSTR == var.vt )
			{
				CString str(var);
				long lPosMap = pSubstMap->head();
				while(lPosMap)
				{
					CString strFrom = pSubstMap->get_key(lPosMap);
					CString strTo = pSubstMap->get(lPosMap);
					lPosMap = pSubstMap->next(lPosMap);

					long n = strFrom.GetLength();
					if( 0 == strFrom.CompareNoCase(str.Left(n)) )
					{
						str.Delete(0, n);
						str.Insert(0, strTo);
						var = str;
					}
				}
			}

			ptrDst->SetValue( bstrPathNew, var );
			
			// if this entry has children => we want to walk to them
			if( EntryCount > 0 )
				RecursiveAddEntry( ptrDst, ptrSrc, bstrPathNew, pszDisabledKeys, pSubstMap );
		}

		// for next entry on this level restore Section
		ptrSrc->SetupSection( bstrPath );
	}
		
	return true;
}

bool ImportShellData( IUnknown* punkSrc, char* pszDisabledKeys )
{	// ������ shell.data, �� ����������� DisabledKeys � � ��������
	// ���� ����� �� [Paths] �� �������
	INamedDataPtr sptrDst(::GetAppUnknown());
	if(sptrDst==0) return false;

	INamedDataPtr sptrSrc(punkSrc);
	if(sptrSrc==0) return false;

	CSubstMap map; // ���������� ������ �����������

	_bstr_t bstrPathSrc;
	_bstr_t bstrPathDst;
	sptrSrc->GetCurrentSection(bstrPathSrc.GetAddress());
	sptrDst->GetCurrentSection(bstrPathDst.GetAddress());

	bool bRes = false;

	do
	{
		if( S_OK != sptrDst->SetupSection( _bstr_t("\\Paths") ) )
			break;
		if( S_OK != sptrSrc->SetupSection( _bstr_t("\\Paths") ) )
			break;

		// get count of entries
		long EntryCount = 0;
		sptrSrc->GetEntriesCount( &EntryCount );

		// for all entries
		for( long i = 0; i<EntryCount; i++ )
		{
			// get entry name
			_bstr_t bstr;
			sptrSrc->GetEntryName( i, bstr.GetAddress() );
			CString strEntryName(bstr.GetBSTR());

			// get value
			_variant_t var;
			sptrSrc->GetValue( bstr, &var );

			if(var.vt==VT_BSTR)
			{ // ��� ���������� ������ ������, ������ ������ ������������� ������� - ������ ����
				CString strFrom(var);
				if(strFrom.GetLength()>=3)
				{ // �������� �� ������������ ������� ?:*\ 
					if(':'==strFrom[1] && '\\'==strFrom[strFrom.GetLength()-1])
					{
						if(strEntryName != "CurrentPath") // CurrentPath �� ����������! �� ����� ���� �����.
						{
							CString strTo = ::GetValueString(sptrDst, "\\Paths", strEntryName, strFrom);
							map.set(strTo, strFrom);
						}
					}
				}
			}        
		}

		RecursiveAddEntry(sptrDst, sptrSrc, "\\",
			pszDisabledKeys, &map);

		bRes = true; // ������� ���������
	}
	while(0); // ��������� 1 ��� - ���� ������ ���� break

	sptrDst->SetupSection( bstrPathDst ); // ������ ���, ��� ����
	sptrSrc->SetupSection( bstrPathSrc );
	return bRes;
}

bool ImportShellData2( IUnknown* punkSrc )
{	// ������ shell.data � ������ MergeKeys � � ��������
	// ���� ����� �� [Paths] �� �������
	INamedDataPtr sptrDst(::GetAppUnknown());
	if(sptrDst==0) return false;

	INamedDataPtr sptrSrc(punkSrc);
	if(sptrSrc==0) return false;

	CSubstMap map; // ���������� ������ �����������

	_bstr_t bstrPathSrc;
	_bstr_t bstrPathDst;
	sptrSrc->GetCurrentSection(bstrPathSrc.GetAddress());
	sptrDst->GetCurrentSection(bstrPathDst.GetAddress());

	bool bRes = false;

	do
	{
		if( S_OK != sptrDst->SetupSection( _bstr_t("\\Paths") ) )
			break;
		if( S_OK != sptrSrc->SetupSection( _bstr_t("\\Paths") ) )
			break;

		// get count of entries
		long EntryCount = 0;
		sptrSrc->GetEntriesCount( &EntryCount );

		// for all entries
		for( long i = 0; i<EntryCount; i++ )
		{
			// get entry name
			_bstr_t bstr;
			sptrSrc->GetEntryName( i, bstr.GetAddress() );
			CString strEntryName(bstr.GetBSTR());

			// get value
			_variant_t var;
			sptrSrc->GetValue( bstr, &var );

			if(var.vt==VT_BSTR)
			{ // ��� ���������� ������ ������, ������ ������ ������������� ������� - ������ ����
				CString strFrom(var);
				if(strFrom.GetLength()>=3)
				{ // �������� �� ������������ ������� ?:*\ 
					if(':'==strFrom[1] && '\\'==strFrom[strFrom.GetLength()-1])
					{
						if(strEntryName != "CurrentPath") // CurrentPath �� ����������! �� ����� ���� �����.
						{
							CString strTo = ::GetValueString(sptrDst, "\\Paths", strEntryName, strFrom);
							map.set(strTo, strFrom);
						}
					}
				}
			}        
		}

		// ������ ���������� ������
		CTreeFilter t;
		CString	strMergeKeysFile = ::MakeAppPathName( "merge.method.keys" );
		if(!PathFileExists(strMergeKeysFile))
		{ // ���� ��� ����� - ��������� ������� ��� �� disabled.method.keys
			CString	strDisabledKeysFile = ::MakeAppPathName( "disabled.method.keys" );
			if(PathFileExists(strDisabledKeysFile))
			{
				ifstream in(strDisabledKeysFile);
				in.imbue(std::locale(""));
				ofstream out(strMergeKeysFile);
				out.imbue(std::locale(""));

				out << "<" << endl;
				while(!in.eof())
				{
					std::string s;
					getline(in, s);
					if( !in.good() ) break;
					out << ">" << s << endl;
				}
			}
		}
		t.Load(strMergeKeysFile);
		t.Set(L"\\_method", CTreeFilter::tlUseNone);
		t.Set(L"\\_step", CTreeFilter::tlUseNone);
		t.Set(L"\\StateBeforeAction", CTreeFilter::tlUseNone);
		t.Set(L"\\StateAfterAction", CTreeFilter::tlUseNone);
		t.Set(L"\\Paths", CTreeFilter::tlUseRight);
		INamedDataPtr sptrND2;
		sptrND2.CreateInstance( szNamedDataProgID );
		t.Merge(sptrSrc, INamedDataPtr(::GetAppUnknown()), sptrND2, CTreeFilter::tlUseLeft, NULL, &map);
		// ����� shell.data � NamedData �������� � sptrND2

		//!!!debug
		//CTreeFilter t2;
		//IFileDataObject2Ptr	sptrF_sd;
		//sptrF_sd.CreateInstance( szNamedDataProgID );
		//t2.Merge(INamedDataPtr(::GetAppUnknown()), INamedDataPtr(::GetAppUnknown()), INamedDataPtr(sptrF_sd), CTreeLevel::tlUseLeft);
		//sptrF_sd->SaveTextFile(_bstr_t("d:\\shell.data"));
		//
		//IFileDataObject2Ptr	sptrF_mt;
		//sptrF_mt.CreateInstance( szNamedDataProgID );
		//t2.Merge(sptrNamedData, sptrNamedData, INamedDataPtr(sptrF_mt), CTreeLevel::tlUseLeft);
		//sptrF_mt->SaveTextFile(_bstr_t("d:\\method.data"));
		//
		//IFileDataObject2Ptr	sptrF2(sptrND2);
		//sptrF2->SaveTextFile(_bstr_t("d:\\merged.data"));

		::CopyNamedData(::GetAppUnknown(), sptrND2);

		bRes = true; // ������� ���������
	}
	while(0); // ��������� 1 ��� - ���� ������ ���� break

	sptrDst->SetupSection( bstrPathDst ); // ������ ���, ��� ����
	sptrSrc->SetupSection( bstrPathSrc );
	return bRes;
}

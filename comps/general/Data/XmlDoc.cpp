#include "stdafx.h"
#import <msxml3.dll>
#include "data.h"
#include "ContentDlg.h"
namespace{
	MSXML2::IXMLDOMDocumentPtr dom;
}

void CContentDlg::OnExport2Xml()
{
	CStringW strDataSourceName;
	if( m_nDataSource == 0 )
	{
		m_ptrData = GetAppUnknown();
		strDataSourceName = L"Shell.xml";
	}
	else
	{
		IApplicationPtr	ptrApp = GetAppUnknown();
		IUnknownPtr punkDoc;
		ptrApp->GetActiveDocument(&punkDoc);
		if( punkDoc )
		{
			m_ptrData = punkDoc;
			IDocumentSitePtr	ptrDocSite( punkDoc );
			BSTR	bstrFileName;
			ptrDocSite->GetPathName( &bstrFileName );
			strDataSourceName = bstrFileName;
			::SysFreeString(bstrFileName);
			int pos=strDataSourceName.ReverseFind(L'.');
			strDataSourceName=strDataSourceName.Left(pos)+L".xml";
		}
	}

	if( m_ptrData==0 )
	{
		_ASSERTE(!"no active documents...");
	}
	else
	{
		dom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		MSXML2::IXMLDOMElementPtr el=dom->createElement("ggg");
		dom->documentElement=el;

		m_ptrData->SetupSection( 0 );

		m_pNode=el;
		MSXML2::IXMLDOMNodePtr elel=FillNode(el, m_ptrData, 0, 0 );
		//if(elel)
		//	el->appendChild(elel);

		dom->save((LPCWSTR)strDataSourceName);
		el=NULL;
		dom=NULL;
	}
}

void Decorate(_bstr_t& bstr){
		CStringW	str = (LPCWSTR)bstr;
		str.Replace(' ','_');
		str.Replace(')','_');
		str.Replace('(','_');
		str.Replace('{','_');
		str.Replace('}','_');
		if(str[0]>='0' && str[0]<='9' || str[0]=='-')
			str.Insert(0,'_');
		bstr=str;
} 

MSXML2::IXMLDOMNode* CContentDlg::FillNode(MSXML2::IXMLDOMNode* pParentNode
	, INamedData *pIData, const char *szPath, const char *szEntryName )
{
	MSXML2::IXMLDOMNodePtr node=pParentNode;
	_bstr_t	bstrCurrentSection = szPath;
	pIData->SetupSection( bstrCurrentSection );

	_variant_t	var;
	_bstr_t		bstrEntry = szEntryName;
	pIData->GetValue( bstrCurrentSection, &var );
	int	nImage = -1;

	_bstr_t bsDecoratedEntry=bstrEntry;
	Decorate(bsDecoratedEntry);
	long nCount = 0;
	pIData->GetEntriesCount( &nCount );
	
	CString	strValue = szEntryName;
	if( strValue.IsEmpty() )
		strValue = "no name";
	else
	{
		switch(var.vt){
		default:
			{
				//if(MSXML2::IXMLDOMElementPtr elParent=node)
				//	elParent->setAttribute(bsDecoratedEntry,_bstr_t(var));
				//else
				//	MSXML2::IXMLDOMElementPtr el=dom->createElement(bsDecoratedEntry);
				MSXML2::IXMLDOMElementPtr el=dom->createElement("Section");
				el->setAttribute("name",bstrEntry);
				el->text=_bstr_t(var);
				node=el;
				pParentNode->appendChild(node);
			}break;
		case VT_EMPTY:
			{
				//strValue += ":Empty";
				MSXML2::IXMLDOMElementPtr el=dom->createElement("Section");
				el->setAttribute("name",bstrEntry);
				node=el;
				pParentNode->appendChild(node);
			}break;
		case VT_NULL:
			{
				//strValue += ":Null";
				MSXML2::IXMLDOMElementPtr el=dom->createElement("Entry");
				el->setAttribute("name",bstrEntry);
				el->setAttribute("Default","0");
				node=el;
				pParentNode->appendChild(node);
			}break;
		case VT_UNKNOWN:
		case VT_DISPATCH:
			{
				//strValue += ":Data object=";
				//strValue += GetObjectName( var.punkVal );
				MSXML2::IXMLDOMElementPtr el=dom->createElement("Object");
				el->setAttribute("name",bstrEntry);
				el->setAttribute("Unknown",_bstr_t(GetObjectName( var.punkVal )));
				if(INamedDataPtr ptrNDPrivate=var.punkVal)
				{
					FillNode(el, ptrNDPrivate, 0, 0 );
				}
				node=el;
				pParentNode->appendChild(node);
			}break;
		case VT_BITS:
			{
				MSXML2::IXMLDOMElementPtr el=dom->createElement("Object");
				el->setAttribute("name",bstrEntry);
				el->setAttribute("Pointer",_bstr_t(var.lVal));
				node=el;
				pParentNode->appendChild(node);
			}break;
		}
	}

	CStringArray	strs;

	for( int nEntry = 0; nEntry < nCount; nEntry++ )
	{
		_bstr_t	bstr;
		pIData->GetEntryName( nEntry, bstr.GetAddress() );
		strs.Add( bstr );
	}

	for( nEntry = 0; nEntry < nCount; nEntry++ )
	{
		CString	strPath = szPath;
		strPath += "\\";
		strPath += strs[nEntry];

		FillNode(node, pIData, strPath, strs[nEntry] );
	}

	return 0;
}

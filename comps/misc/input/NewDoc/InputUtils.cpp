#include "StdAfx.h"
#include "InputUtils.h"


void DoExecute(sptrIActionManager &pActionMan, LPCTSTR pActionName, LPCTSTR pParams)
{
	CString sActionName(pActionName);
	CString sParams(pParams);
	BSTR bstrActionName = sActionName.AllocSysString();
	BSTR bstrParams = sParams.AllocSysString();
	pActionMan->ExecuteAction(bstrActionName, bstrParams, 0);
	::SysFreeString(bstrActionName);
	::SysFreeString(bstrParams);
};

IUnknown *FindDocTemplate(LPCTSTR lpTemplateName)
{
	sptrIApplication sptrA(GetAppUnknown());
	CString s2(lpTemplateName);
	LONG_PTR lPosTemplate,lPosTemplFnd = 0;
	HRESULT hr = sptrA->GetFirstDocTemplPosition(&lPosTemplate);
	while (SUCCEEDED(hr) && lPosTemplate)
	{
		IUnknown *punkTemplate;
		BSTR bstrName;
		hr = sptrA->GetNextDocTempl(&lPosTemplate, &bstrName, &punkTemplate);
		CString s(bstrName);
		::SysFreeString(bstrName);
		if (s2 == s)
			return punkTemplate;
		if (punkTemplate)
			punkTemplate->Release();
	}
	return NULL;
}

void __EnlistAllDocs(CArray<GUID, GUID&>&uiaFoundDocs, LPCTSTR lpTemplateName)
{
	uiaFoundDocs.RemoveAll();
	sptrIApplication sptrA(GetAppUnknown());
	IUnknown *punkImageDoc = NULL;
	CString s2(lpTemplateName);
//	CString s2(_T("Image"));
	LONG_PTR lPosTemplate,lPosTemplFnd = 0;
	HRESULT hr = sptrA->GetFirstDocTemplPosition(&lPosTemplate);
	while (SUCCEEDED(hr) && lPosTemplate)
	{
		IUnknown *punkTemplate;
		BSTR bstrName;
		LONG_PTR lPrevTempl = lPosTemplate;
		hr = sptrA->GetNextDocTempl(&lPosTemplate, &bstrName, &punkTemplate);
		CString s(bstrName);
		::SysFreeString(bstrName);
		if (punkTemplate)
			punkTemplate->Release();
		if (s2 == s)
		{
			lPosTemplFnd = lPrevTempl;
			break;
		}
	}
	if (lPosTemplFnd)
	{
		LONG_PTR lPosDoc;
		hr = sptrA->GetFirstDocPosition(lPosTemplFnd, &lPosDoc);
		while (SUCCEEDED(hr) && lPosDoc)
		{
			IUnknown *punkDoc;
			hr = sptrA->GetNextDoc(lPosTemplFnd, &lPosDoc, &punkDoc);
			if (SUCCEEDED(hr) && punkDoc)
			{
				INumeredObjectPtr sptrNO(punkDoc);
				GuidKey l;
				sptrNO->GetKey(&l);
				uiaFoundDocs.Add(l);
			}
			if (punkDoc)
				punkDoc->Release();
		}
	}
}

static bool __WhetherDocInList(CArray<GUID, GUID&> &uiaFoundDocs, GuidKey l)
{
	for (int i = 0; i < uiaFoundDocs.GetSize(); i++)
	{
		if (l == uiaFoundDocs.GetAt(i))
			return true;
	}
	return false;
}

IUnknown *__FindNewDocs(CArray<GUID, GUID&>&uiaFoundDocs, GuidKey &lDocKey, LPCTSTR lpTemplateName)
{
	sptrIApplication sptrA(GetAppUnknown());
	IUnknown *punkImageDoc = NULL;
	CString s2(lpTemplateName);
//	CString s2(_T("Image"));
	LONG_PTR lPosTemplate,lPosTemplFnd = 0;
	HRESULT hr = sptrA->GetFirstDocTemplPosition(&lPosTemplate);
	while (SUCCEEDED(hr) && lPosTemplate)
	{
		IUnknown *punkTemplate;
		BSTR bstrName;
		LONG_PTR lPrevTempl = lPosTemplate;
		hr = sptrA->GetNextDocTempl(&lPosTemplate, &bstrName, &punkTemplate);
		CString s(bstrName);
		::SysFreeString(bstrName);
		if (punkTemplate)
			punkTemplate->Release();
		if (s2 == s)
		{
			lPosTemplFnd = lPrevTempl;
			break;
		}
	}
	if (lPosTemplFnd)
	{
		LONG_PTR lPosDoc;
		hr = sptrA->GetFirstDocPosition(lPosTemplFnd, &lPosDoc);
		while (SUCCEEDED(hr) && lPosDoc)
		{
			IUnknown *punkDoc;
			hr = sptrA->GetNextDoc(lPosTemplFnd, &lPosDoc, &punkDoc);
			if (SUCCEEDED(hr) && punkDoc)
			{
				INumeredObjectPtr sptrNO(punkDoc);
				GuidKey l;
				sptrNO->GetKey(&l);
				if (!__WhetherDocInList(uiaFoundDocs, l))
				{
					lDocKey = l;
					return punkDoc;
				}
			}
			if (punkDoc)
				punkDoc->Release();
		}
	}
	return NULL;
}

IUnknown *__FindDocByKey(GuidKey lDocKey, LPCTSTR lpTemplateName)
{
	sptrIApplication sptrA(GetAppUnknown());
	IUnknown *punkImageDoc = NULL;
	CString s2(lpTemplateName);
//	CString s2(_T("Image"));
	LONG_PTR lPosTemplate,lPosTemplFnd = 0;
	HRESULT hr = sptrA->GetFirstDocTemplPosition(&lPosTemplate);
	while (SUCCEEDED(hr) && lPosTemplate)
	{
		IUnknown *punkTemplate;
		BSTR bstrName;
		LONG_PTR lPrevTempl = lPosTemplate;
		hr = sptrA->GetNextDocTempl(&lPosTemplate, &bstrName, &punkTemplate);
		CString s(bstrName);
		::SysFreeString(bstrName);
		if (punkTemplate)
			punkTemplate->Release();
		if (s2 == s)
		{
			lPosTemplFnd = lPrevTempl;
			break;
		}
	}
	if (lPosTemplFnd)
	{
		LONG_PTR lPosDoc;
		hr = sptrA->GetFirstDocPosition(lPosTemplFnd, &lPosDoc);
		while (SUCCEEDED(hr) && lPosDoc)
		{
			IUnknown *punkDoc;
			hr = sptrA->GetNextDoc(lPosTemplFnd, &lPosDoc, &punkDoc);
			if (SUCCEEDED(hr) && punkDoc)
			{
				INumeredObjectPtr sptrNO(punkDoc);
				GuidKey l;
				sptrNO->GetKey(&l);
				if (l == lDocKey)
					return punkDoc;
			}
			if (punkDoc)
				punkDoc->Release();
		}
	}
	return NULL;
}

IUnknown *__FindDocByName(LPCTSTR pDocName, LPCTSTR lpTemplateName)
{
	sptrIApplication sptrA(GetAppUnknown());
	IUnknown *punkImageDoc = NULL;
	CString s2(lpTemplateName);
	LONG_PTR lPosTemplate,lPosTemplFnd = 0;
	HRESULT hr = sptrA->GetFirstDocTemplPosition(&lPosTemplate);
	while (SUCCEEDED(hr) && lPosTemplate)
	{
		IUnknown *punkTemplate;
		BSTR bstrName;
		LONG_PTR lPrevTempl = lPosTemplate;
		hr = sptrA->GetNextDocTempl(&lPosTemplate, &bstrName, &punkTemplate);
		CString s(bstrName);
		::SysFreeString(bstrName);
		if (punkTemplate)
			punkTemplate->Release();
		if (s2 == s)
		{
			lPosTemplFnd = lPrevTempl;
			break;
		}
	}
	if (lPosTemplFnd)
	{
		LONG_PTR lPosDoc;
		hr = sptrA->GetFirstDocPosition(lPosTemplFnd, &lPosDoc);
		while (SUCCEEDED(hr) && lPosDoc)
		{
			IUnknown *punkDoc;
			hr = sptrA->GetNextDoc(lPosTemplFnd, &lPosDoc, &punkDoc);
			if (SUCCEEDED(hr) && punkDoc)
			{
				sptrIDocumentSite sptrDS(punkDoc);
				BSTR bstr;
				sptrDS->GetPathName(&bstr);
				CString sPathName(bstr);
				::SysFreeString(bstr);
				if (!sPathName.IsEmpty() && sPathName == pDocName)
					return punkDoc;
			}
			if (punkDoc)
				punkDoc->Release();
		}
	}
	return NULL;
}

void _SectionAndEntry(LPCTSTR lpName, CString &sSec, CString &sEntry)
{
	CString s(lpName);
	int n = s.Find("\\/");
	if (n == -1)
	{
//		sSec = "Settings";
		sSec = _T("");
		sEntry = s;
	}
	else
	{
		sSec = s.Left(n);
		sEntry = s.Mid(n+1);
	}
}

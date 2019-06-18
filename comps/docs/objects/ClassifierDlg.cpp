// ClassifierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "objects.h"
#include "ClassifierDlg.h"
#include "SelectParamDlg.h"
#include "ClassNameDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CClassifierDlg dialog


CClassifierDlg::CClassifierDlg(CWnd* pParent /*=NULL*/, CString strSectionName)
	: CDialog(CClassifierDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClassifierDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_strSectionName = strSectionName;
	m_keyPrevClassList = INVALID_KEY;
}

CClassifierDlg::~CClassifierDlg()
{
	for(long i = 0; i < m_arrClassLists.GetSize(); i++)
		if(m_arrClassLists[i]) m_arrClassLists[i]->Release();
	m_arrClassLists.RemoveAll();

}



void CClassifierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClassifierDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClassifierDlg, CDialog)
	//{{AFX_MSG_MAP(CClassifierDlg)
	ON_CBN_SELENDOK(IDC_COMBO_CLASSES, OnSelendokComboClasses)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PARAM, OnButtonAddParam)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_PARAM, OnButtonRemoveParam)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_CLASS, OnButtonRemoveClass)
	ON_BN_CLICKED(IDC_BUTTON_ADD_CLASS, OnButtonAddClass)
	ON_BN_CLICKED(IDC_BUTTON_RENAME_CLASS, OnButtonRenameClass)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassifierDlg message handlers

BOOL CClassifierDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComboBox* pClassCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CLASSES);
	
	//enum all classes lists, fill combo
	IApplicationPtr sptrA(GetAppUnknown());
	long	lPosTemplate = 0;
	sptrA->GetFirstDocTemplPosition(&lPosTemplate);
	while(lPosTemplate)
	{
		long	lPosDoc = 0;
		sptrA->GetFirstDocPosition(lPosTemplate, &lPosDoc);
		while(lPosDoc)
		{
			IUnknown *punkDoc = 0;
			sptrA->GetNextDoc(lPosTemplate, &lPosDoc, &punkDoc);
			IDataContext2Ptr sptrContext(punkDoc);
			if(punkDoc) punkDoc->Release();
			if(sptrContext != 0)
			{
				long nClassPos = 0;
				sptrContext->GetFirstObjectPos(_bstr_t(szTypeClassList), &nClassPos);
				while(nClassPos)
				{
					IUnknown* punkClassList = 0;
					sptrContext->GetNextObject(_bstr_t(szTypeClassList), &nClassPos, &punkClassList);
					
					if(punkClassList)
					{
						m_arrClassLists.Add(punkClassList);
						if(pClassCombo) pClassCombo->AddString(::GetObjectName(punkClassList));
						//punkClassList->Release();
					}
				}
			}
		}
		sptrA->GetNextDocTempl(&lPosTemplate, 0, 0);
	}

	CRect rcGrid;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(&rcGrid);
	ScreenToClient(&rcGrid);

	m_grid.Create(rcGrid, this,  IDC_GRID);	

	CString strClass;

	strClass.LoadString(IDS_CLASSIFIER_CLASS);
	
	m_grid.InsertColumn(strClass);		
	
	m_grid.SetColumnWidth(0, rcGrid.Width()/4);
	
	m_grid.EnableSelection(FALSE);
	m_grid.SetFixedRowCount(1);	
	m_grid.SetFixedColumnCount(1);	
	m_grid.EnableDragColumn(FALSE);

	m_grid.EnableDragAndDrop(FALSE);

	_GetAllParams();


	if(pClassCombo) 
	{
		pClassCombo->SetCurSel(0);
		OnSelendokComboClasses();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClassifierDlg::_GetAllParams()
{
	m_mapParams1.RemoveAll();
	m_mapParams2.RemoveAll();

	IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
	if( !punk )return;
	ICompManagerPtr sptrMan = punk;
	punk->Release();
	if (sptrMan == 0)return;
	
	int nCount = 0;
	sptrMan->GetCount( &nCount );
	for( int i = 0; i < nCount; i++ )
	{
		IUnknown *punkGroup = 0;
		if (SUCCEEDED(sptrMan->GetComponentUnknownByIdx( i, &punkGroup)))
		{
			IMeasParamGroupPtr sptrGroup(punkGroup);
			if(sptrGroup != 0)
			{
				long lParamPos = 0;
				if (FAILED(sptrGroup->GetFirstPos(&lParamPos)))
					continue;

				for (; lParamPos != 0;)
				{
					ParameterDescriptor	*pdescr;
					// get param info 
					if (FAILED(sptrGroup->GetNextParam( &lParamPos, &pdescr ) ) );
						break;
					
					CString strParamName( pdescr->bstrUserName );
					_bstr_t bstrParamFormat( pdescr->bstrDefFormat );

					// if param's key is invalid -> continue to next param
					if (pdescr->lKey == -1)
						continue;

					if( pdescr->lEnabled )
					{
						m_mapParams1.SetAt(pdescr->lKey, strParamName);
						m_mapParams2.SetAt(strParamName, pdescr->lKey);
					}
				}
			}
			if(punkGroup)
				punkGroup->Release();
		}
	}
}

void CClassifierDlg::OnSelendokComboClasses() 
{
	m_mapClassesByRow.RemoveAll();
	m_mapPresentParams.RemoveAll();

	CComboBox* pClassCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CLASSES);
	if(!pClassCombo) return;
	int nCurSel = pClassCombo->GetCurSel();

	if(nCurSel < 0 || m_keyPrevClassList == ::GetObjectKey(m_arrClassLists[nCurSel]))
		return;

	m_keyPrevClassList = ::GetObjectKey(m_arrClassLists[nCurSel]);
	
	m_grid.SetRowCount(1);
    m_grid.SetColumnCount(1);
	
	CObjectListWrp classList(m_arrClassLists[nCurSel]);

	POSITION pos = classList.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkClass = classList.GetNextObject(pos);
		IClassObjectPtr sptrClass(punkClass);
		if(punkClass)
			punkClass->Release();
		if(sptrClass != 0)
		{
			int nClassRow = m_grid.InsertRow(::GetObjectName(sptrClass));

			m_mapClassesByRow.SetAt(nClassRow, sptrClass);

			long nParamPos = 0;
			sptrClass->GetFirstParamLimitsPos(&nParamPos);
			while(nParamPos)
			{
				long lParamKey = 0;
				double fLo = 0., fHi = 0.;
				sptrClass->GetNextParamLimits(&nParamPos, &lParamKey, &fLo, &fHi);

				CString strParamName = "";
				if(m_mapParams1.Lookup(lParamKey, strParamName) == TRUE)
				{
					long nCol = 0, nColLo = 0, nColHi = 0;

					bool bUpdateRows = false;

					if(m_mapPresentParams.Lookup(strParamName, nCol) == TRUE)
					{
						nColLo = nCol;
						nColHi = nCol+1;
					}
					else
					{
						nColLo = m_grid.InsertColumn(strParamName + " Lo");
						nColHi = m_grid.InsertColumn(strParamName + " Hi");
						
						bUpdateRows = true;

						m_mapPresentParams.SetAt(strParamName, nColLo);
					}

					CString strLo;
					strLo.Format("%f", fLo);
					m_grid.SetItemText(nClassRow, nColLo, strLo);
				
					CString strHi;
					strHi.Format("%f", fHi);
					m_grid.SetItemText(nClassRow, nColHi, strHi);

					if(bUpdateRows)
						_UpdateRows(nColLo);
				}
			}
		}
	}
	
}

BOOL CClassifierDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if(wParam == IDC_GRID)
	{
		NM_GRIDVIEW* pnmgv = (NM_GRIDVIEW*)lParam;
		if(pnmgv)
		{
			if(pnmgv->hdr.code == GVN_ENDLABELEDIT)
			{
				int nRow = pnmgv->iRow;	
				int nCol = pnmgv->iColumn;
				if(nRow > 0 && nCol > 0)
				{
					CString strVal = m_grid.GetItemText(nRow, nCol);
					double fVal = atof(strVal);
					strVal.Format("%f", fVal);
					m_grid.SetItemText(nRow, nCol, strVal);

					IUnknown* punkClass = 0;
					if(m_mapClassesByRow.Lookup(nRow, punkClass) == TRUE)
					{
						IClassObjectPtr sptrCO(punkClass);
						if(sptrCO != 0)
						{			
							CString strVal = m_grid.GetItemText(0, nCol);
							CString strParamName = strVal.Left(strVal.ReverseFind(' '));		
							long nParam = 0;
							if(m_mapParams2.Lookup(strParamName, nParam) == FALSE)
							{
								ASSERT(FALSE);
							}
							else
							{
								double fLo = 0., fHi = 0.;
								sptrCO->GetParamLimits(nParam, &fLo, &fHi);
								CString strLoHi = strVal.Right(strVal.GetLength() - strVal.ReverseFind(' ')-1);
								if(strLoHi == "Lo")
								{
									sptrCO->SetParamLimits(nParam, fVal, fHi);
								}
								else
								{
									sptrCO->SetParamLimits(nParam, fLo, fVal);
								}
							}
						}
					}
				}
			}
		}
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CClassifierDlg::OnButtonAddParam() 
{
	CSelectParamDlg dlg;

	POSITION pos = m_mapParams1.GetStartPosition();
	while(pos)
	{
		long nParam = 0, nCol = 0;
		CString strParamName = "";
		m_mapParams1.GetNextAssoc(pos, nParam, strParamName);

		if(m_mapPresentParams.Lookup(strParamName, nCol) == FALSE)
			dlg.AddParam(strParamName);
	}

	if(dlg.DoModal() == IDOK)
	{
		CString strParamName = dlg.GetSelectedParamName();
		if(!strParamName.IsEmpty())
		{
			int nColLo = m_grid.InsertColumn(strParamName + " Lo");
			int nColHi = m_grid.InsertColumn(strParamName + " Hi");
			
			_UpdateRows(nColLo);

			m_mapPresentParams.SetAt(strParamName, nColLo);


			POSITION pos = m_mapClassesByRow.GetStartPosition();
			while(pos)
			{
				long nRow = 0;
				IUnknown* punkClass = 0;
				m_mapClassesByRow.GetNextAssoc(pos, nRow, punkClass);
				IClassObjectPtr sptrCO(punkClass);
				if(sptrCO != 0)
				{			
							
					long nParam = 0;
					if(m_mapParams2.Lookup(strParamName, nParam) == FALSE)
					{
						ASSERT(FALSE);
					}
					else
						sptrCO->SetParamLimits(nParam, 0., 0.);
				}
			}

		}

	}
}

void CClassifierDlg::_UpdateRows(long nColLo)
{
	for(long nRow = 1; nRow < m_grid.GetRowCount(); nRow++)
	{
		CString strVal = m_grid.GetItemText(nRow, nColLo);
		double fVal = atof(strVal);
		strVal.Format("%f", fVal);
		m_grid.SetItemText(nRow, nColLo, strVal);

		strVal = m_grid.GetItemText(nRow, nColLo+1);
		fVal = atof(strVal);
		strVal.Format("%f", fVal);
		m_grid.SetItemText(nRow, nColLo+1, strVal);
	}

	m_grid.AutoSizeColumn(nColLo);
	m_grid.AutoSizeColumn(nColLo+1);
}

void CClassifierDlg::OnButtonRemoveParam() 
{
	long nRow = m_grid.GetFocusCell().row;
	long nCol = m_grid.GetFocusCell().col;

	if(nRow < 0 || nCol < 1)
		return;

	CString strVal = m_grid.GetItemText(0, nCol);
	CString strParamName = strVal.Left(strVal.ReverseFind(' '));
	CString strLoHi = strVal.Right(strVal.GetLength() - strVal.ReverseFind(' ')-1);

	m_mapPresentParams.RemoveKey(strParamName);

	POSITION pos = m_mapClassesByRow.GetStartPosition();
	while(pos)
	{
		long nRow = 0;
		IUnknown* punkClass = 0;
		m_mapClassesByRow.GetNextAssoc(pos, nRow, punkClass);
		IClassObjectPtr sptrCO(punkClass);
		if(sptrCO != 0)
		{			
					
			long nParam = 0;
			if(m_mapParams2.Lookup(strParamName, nParam) == FALSE)
			{
				ASSERT(FALSE);
			}
			else
				sptrCO->RemoveParamLimits(nParam);
		}
	}


	if(strLoHi == "Lo")
	{
		m_grid.DeleteColumn(nCol+1);
		m_grid.DeleteColumn(nCol);
	}
	else
	{
		m_grid.DeleteColumn(nCol);
		m_grid.DeleteColumn(nCol-1);
	}

	m_grid.Refresh();
}

void CClassifierDlg::OnButtonRemoveClass() 
{
	long nRow = m_grid.GetFocusCell().row;
	long nCol = m_grid.GetFocusCell().col;

	if(nRow < 1 || nCol < 0)
		return;

	CString strClassName = m_grid.GetItemText(nRow, 0);

	m_grid.DeleteRow(nRow);
	m_grid.Refresh();

	CComboBox* pClassCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CLASSES);
	if(!pClassCombo) return;
	int nCurSel = pClassCombo->GetCurSel();

	if(nCurSel < 0)
		return;

	CObjectListWrp classList(m_arrClassLists[nCurSel]);

	IUnknown* punkClass = 0;
	if(m_mapClassesByRow.Lookup(nRow, punkClass) == TRUE)
	{
		classList.Remove(punkClass);
		m_mapClassesByRow.RemoveKey(nRow);
	}
	

	/*CObjectListWrp classList(m_arrClassLists[nCurSel]);
	POSITION pos = classList.GetFirstObjectPosition();
	while(pos)
	}{
		POSITION posSave = pos;
		IUnknown* punkClass = classList.GetNextObject(pos);
		if(punkClass)
		{
			if(::GetObjectName(punkClass) == strClassName)
			{
				classList.RemoveAt(posSave);
				punkClass->Release();
				break;
			}

			punkClass->Release();
		}
	}
	*/
}

void CClassifierDlg::OnButtonAddClass() 
{
	CComboBox* pClassCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CLASSES);
	if(!pClassCombo) return;
	int nCurSel = pClassCombo->GetCurSel();
	if(nCurSel < 0)
		return;

	CClassNameDlg dlg;
	if(dlg.DoModal() != IDOK)
		return;
	CString strClassName = dlg.GetName();

	if(strClassName.IsEmpty())
	{
		INamedDataObject2Ptr sptrNDO(m_arrClassLists[nCurSel]);
		if(sptrNDO != 0)
		{
			IUnknown* punkNamedData = 0;
			sptrNDO->GetData(&punkNamedData);
			if(punkNamedData)
			{
				strClassName = ::CreateUniqueName(punkNamedData, szTypeClass);
				punkNamedData->Release();
			}
		}
	}

	CObjectListWrp classList(m_arrClassLists[nCurSel]);
	IUnknown* punkClass = ::CreateTypedObject(szTypeClass);
	if(punkClass)
	{
		::SetObjectName(punkClass, strClassName);

		classList.Add(punkClass);
		
		int nClassRow = m_grid.InsertRow(::GetObjectName(punkClass));

		m_mapClassesByRow.SetAt(nClassRow, punkClass);

		for(long nCol = 1; nCol < m_grid.GetColumnCount(); nCol++)
		{
			CString strVal = m_grid.GetItemText(nClassRow, nCol);
			double fVal = atof(strVal);
			strVal.Format("%f", fVal);
			m_grid.SetItemText(nClassRow, nCol, strVal);
		}

		punkClass->Release();
	}
}

void CClassifierDlg::OnButtonRenameClass() 
{
	long nRow = m_grid.GetFocusCell().row;
	long nCol = m_grid.GetFocusCell().col;

	if(nRow < 1 || nCol < 0)
		return;

	IUnknown* punkClass = 0;
	if(m_mapClassesByRow.Lookup(nRow, punkClass) == TRUE)
	{
		CClassNameDlg dlg(NULL, ::GetObjectName(punkClass));
		if(dlg.DoModal() == IDOK)
		{
			m_grid.SetItemText(nRow, 0, dlg.GetName());
			::SetObjectName(punkClass, dlg.GetName());
			m_grid.Refresh();
		}
	}
}

void CClassifierDlg::OnOK() 
{
	INamedDataPtr	sptrData(GetAppUnknown());

	sptrData->DeleteEntry(_bstr_t(m_strSectionName));

	/*sptrData->SetupSection(_bstr_t(m_strSectionName));

	long nEntries = 0, i = 0;
	BSTR bstrName = 0;
	sptrData->GetEntriesCount(&nEntries);
	for(i = nEntries-1; i >= 0; i--)
	{
		sptrData->GetEntryName(i, &bstrName);
		sptrData->DeleteEntry(bstrName);
		::SysFreeString(bstrName);
	}
	*/

	int nCols = m_grid.GetColumnCount();

	long nParamsCount = m_mapPresentParams.GetCount();

	POSITION pos = m_mapClassesByRow.GetStartPosition();
	while(pos)
	{
		long nRow = 0;
		IUnknown* punkClass = 0;
		m_mapClassesByRow.GetNextAssoc(pos, nRow, punkClass);
		IClassObjectPtr sptrCO(punkClass);

		if(sptrCO != 0)
		{			
					
			long nParamsAdded = 0;
			for(int i = 1; i < nCols; i+=2)
			{
				CString strValLo = m_grid.GetItemText(nRow, i);
				CString strValHi = m_grid.GetItemText(nRow, i+1);
				double fLo = atof(strValLo);
				double fHi = atof(strValHi);

				CString strVal = m_grid.GetItemText(0, i);
				CString strParamName = strVal.Left(strVal.ReverseFind(' '));
				CString strParam;
				long nParam = 0;
				if(m_mapParams2.Lookup(strParamName, nParam) == FALSE)
				{
					ASSERT(FALSE);
				}
				
				strParam.Format("%d", nParam);
				if(fLo != 0. || fHi != 0.)
				{
					sptrCO->SetParamLimits(nParam, fLo, fHi);

					BSTR bstrGUID = 0;
					StringFromCLSID(::GetObjectKey(punkClass), &bstrGUID);
					CString strGUID(bstrGUID);
					::CoTaskMemFree(bstrGUID);

					::SetValue(::GetAppUnknown(), m_strSectionName + "\\" + strGUID, "qq", (long)1);
					
					/*CString strSection = m_strSectionName + "\\" + strGUID + "\\" + strParam;
					::SetValue(::GetAppUnknown(), strSection, "lo", fLo);
					::SetValue(::GetAppUnknown(), strSection, "hi", fHi);*/
				}
				else
				{
					sptrCO->RemoveParamLimits(nParam);
				}
			}
		}
	}
		
	CDialog::OnOK();
}

void CClassifierDlg::OnHelp() 
{
	HelpDisplay( "ClassifierDlg" );	
}

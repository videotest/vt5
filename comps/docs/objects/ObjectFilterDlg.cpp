// ObjectFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "objects.h"
#include "ObjectFilterDlg.h"
#include "../common2/class_utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#pragma comment( lib, "Controls_d.lib" )
#else
#pragma comment( lib, "Controls.lib" )
#endif//

const long g_nColsCount = 3;

static void _GetMinMaxValue(long nParamKey, double &dMin, double &dMax)
{
	dMin = 0.;
	dMax = 100.;
	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return;
	IUnknown* punkDoc = NULL;
	sptrApp->GetActiveDocument( &punkDoc );
	if( punkDoc == NULL )
		return;
	sptrIDocument sptrDoc( punkDoc );
	punkDoc->Release();
	if( sptrDoc == NULL )
		return;
	IUnknown *pList = ::GetActiveObjectFromDocument(sptrDoc, szTypeObjectList);
	sptrINamedDataObject2 sptrObjects(pList);
	ICalcObjectContainerPtr sptrCntr(pList);
	pList->Release();
	ParameterContainer *pParamCont;
	sptrCntr->ParamDefByKey(nParamKey, &pParamCont);
	long pos = 0;
	bool bFirst = true;
	sptrObjects->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		IUnknownPtr sptr;
		sptrObjects->GetNextChild((long*)&pos, &sptr);
		ICalcObjectPtr sptrObj(sptr);
		long lClass = get_object_class(sptrObj );
		if (lClass >= 0)
		{
			double dVal;
			HRESULT hr = sptrObj->GetValue(nParamKey, &dVal);
			if (SUCCEEDED(hr))
			{
				dVal *= pParamCont->pDescr->fCoeffToUnits;
				if (bFirst)
				{
					dMin = dMax = dVal;
					bFirst = false;
				}
				else
				{
					if (dVal < dMin)
						dMin = dVal;
					if (dVal > dMax)
						dMax = dVal;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CObjectFilterDlg dialog


CObjectFilterDlg::CObjectFilterDlg(CWnd* pParent /*=NULL*/, CString strSection)
	: CDialog(CObjectFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectFilterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_strNameDataSection = strSection;
	m_strFirstParam = "";
}

CObjectFilterDlg::~CObjectFilterDlg()
{
}

void CObjectFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectFilterDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CObjectFilterDlg)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectFilterDlg message handlers

BOOL CObjectFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rcGrid;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(&rcGrid);
	ScreenToClient(&rcGrid);

	// vanek - disable drag column - 22.09.2003
	m_grid.EnableDragColumn( FALSE );

	m_grid.Create(rcGrid, this,  IDC_GRID );	

	CString strParam, strMinBound, strMaxBound;

	strParam.   LoadString(IDS_OBJECT_FILTER_PARAM);
	strMinBound.LoadString(IDS_OBJECT_FILTER_MIN_BOUND);
	strMaxBound.LoadString(IDS_OBJECT_FILTER_MAX_BOUND);
	
	m_grid.InsertColumn(strParam, DT_CENTER|DT_VCENTER);		
	m_grid.InsertColumn(strMinBound, DT_CENTER|DT_VCENTER);
	m_grid.InsertColumn(strMaxBound, DT_CENTER|DT_VCENTER);

	m_grid.SetColumnWidth(0, rcGrid.Width()/2 - 1);
	m_grid.SetColumnWidth(1, rcGrid.Width()/4 - 1);
	m_grid.SetColumnWidth(2, rcGrid.Width()/4 - 1);
	
	m_grid.EnableSelection(true);
	m_grid.SetFixedRowCount(1);	

	_LoadParamsToGrid();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CObjectFilterDlg::_LoadParamsToGrid()
{
	m_mapParams1.RemoveAll();
	m_mapParams2.RemoveAll();
	m_strFirstParam = "";

	//get all params
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
					if (FAILED(sptrGroup->GetNextParam( &lParamPos, &pdescr ) ) )
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
						m_mapParams1Unit.SetAt(pdescr->lKey, pdescr->fCoeffToUnits);
					}
				}
			}
			if(punkGroup)
				punkGroup->Release();
		}
	}

	//fill table from app data
	INamedDataPtr	sptrData(GetAppUnknown());
	sptrData->SetupSection(_bstr_t(m_strNameDataSection));

	CString strLo, strHi;	
	CStringArray arrParamsSections;
	double fVal = 0.;


	long nEntries = 0;
	sptrData->GetEntriesCount(&nEntries);
	for(i = 0; i < nEntries; i++)
	{
		BSTR bstrName = 0;
		sptrData->GetEntryName(i, &bstrName);
		CString str(bstrName);
		int nParam = atoi(str);
		if(m_mapParams1.Lookup(nParam, str) == TRUE)
			arrParamsSections.Add(bstrName);
		::SysFreeString(bstrName);
	}
	
	for(i = 0; i < nEntries; i++)
	{
		//paul 21.04.2003
		if( i >= arrParamsSections.GetSize() )	continue;

		if(arrParamsSections[i].IsEmpty())
			continue;

		CString str(arrParamsSections[i]);
		int nParamID = atoi(str);

		double fCoeffToUnits=1;
		m_mapParams1Unit.Lookup(nParamID, fCoeffToUnits);

		fVal = ::GetValueDouble(GetAppUnknown(), m_strNameDataSection+"\\"+arrParamsSections[i], "lo", 0.0);
		strLo.Format("%f", fVal*fCoeffToUnits); //!!! *fCoeffToUnits - перевод из метров в юниты

		fVal = ::GetValueDouble(GetAppUnknown(), m_strNameDataSection+"\\"+arrParamsSections[i], "hi", 100.0);
		strHi.Format("%f", fVal*fCoeffToUnits);	

		int nRow = m_grid.InsertRow(0);
		if (nRow > 0)
		{
			m_grid.SetCellType(nRow, 0, RUNTIME_CLASS(CGridCellCombo));
			int nNum = atoi(arrParamsSections[i]);
			CString strParameterName = "";
			m_mapParams1.Lookup(nNum, strParameterName);
			m_grid.SetItemText(nRow, 0, strParameterName);
			m_grid.SetItemText(nRow, 1, strLo);
			m_grid.SetItemText(nRow, 2, strHi);
			SetComboBox(nRow);
		}
	}

	CWnd* pwnd = GetDlgItem(IDC_BUTTON_REMOVE);
	if(pwnd)pwnd->EnableWindow(m_grid.GetRowCount() > 1);
}

BOOL CObjectFilterDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if(wParam == IDC_GRID)
	{
		NM_GRIDVIEW* pnmgv = (NM_GRIDVIEW*)lParam;
		if(pnmgv)
		{
			if(pnmgv->hdr.code == GVN_SELCHANGED)
			{				
				if(pnmgv->iColumn == 0)
					SetComboBox(pnmgv->iRow);
			}
			else
			if(pnmgv->hdr.code == GVN_ENDLABELEDIT)
			{
				int nRow = pnmgv->iRow;	
				int nCol = pnmgv->iColumn;
				if(nRow > 0 && nCol > 0)
				{
					CString strVal = m_grid.GetItemText(nRow, nCol);
					double fVal = atof(strVal);
					strVal.Format("%.6f", fVal);
					m_grid.SetItemText(nRow, nCol, strVal);
				}
				else if (nRow > 0)
				{
					ASSERT(nCol == 0);
					double dMin = 0., dMax = 100.;
					CString sParamName = m_grid.GetItemText(nRow, nCol);
					long nParam;
					if (m_mapParams2.Lookup(sParamName,nParam))
						_GetMinMaxValue(nParam, dMin, dMax);
					CString s;
					s.Format("%.6f", dMin);
					m_grid.SetItemText(nRow, 1, s);
					s.Format("%.6f", dMax+0.000001);
					m_grid.SetItemText(nRow, 2, s);
					m_grid.Invalidate();
				}
			}
		}
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CObjectFilterDlg::SetComboBox(int nRow)
{
	if( nRow < 1 )
		return;

	if( !m_grid.IsValid(nRow, 0))
		return;

	CStringArray options;

	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return;

	IUnknown* punkDoc = NULL;
	sptrApp->GetActiveDocument( &punkDoc );
	if( punkDoc == NULL )
		return;

	sptrIDocument sptrDoc( punkDoc );
	punkDoc->Release();

	if( sptrDoc == NULL )
		return;

	//set combo boxes context

	POSITION pos = m_mapParams1.GetStartPosition();
	long nKey = 0;
	CString strParamName = "";
	while(pos)
	{
		m_mapParams1.GetNextAssoc(pos, nKey, strParamName);
		options.Add(strParamName);
	}

	CGridCellCombo *pCell = (CGridCellCombo*) m_grid.GetCell(nRow, 0);
	
	pCell->SetOptions(options);

	pCell->SetStyle(CBS_DROPDOWNLIST);
	
}

void CObjectFilterDlg::OnButtonAdd() 
{
	int nRow = m_grid.InsertRow(0);
	if (nRow > 0)
	{
		m_grid.SetCellType(nRow, 0, RUNTIME_CLASS(CGridCellCombo));
		m_grid.SetItemText(nRow, 0, m_strFirstParam);
		m_grid.SetItemText(nRow, 1, "0.0");
		m_grid.SetItemText(nRow, 2, "100.0");

		m_grid.SetFocusCell(nRow, 0);

		CWnd* pwnd = GetDlgItem(IDC_BUTTON_REMOVE);
		if(pwnd)pwnd->EnableWindow(TRUE);
	}	
}

void CObjectFilterDlg::OnButtonRemove() 
{
	long nRow = m_grid.GetFocusCell().row;
	long nCol = m_grid.GetFocusCell().col;

	if(nRow < 0 || nCol < 0)
		return;

	if(m_grid.IsCellFixed(nRow, nCol) == TRUE)
		nRow++;
	m_grid.DeleteRow(nRow);

	m_grid.Refresh();

	if(m_grid.GetRowCount() > 1)
	{
		m_grid.SetFocusCell(max(1, nRow-1), nCol);
	}
	else
	{
		CWnd* pwnd = GetDlgItem(IDC_BUTTON_REMOVE);
		if(pwnd)pwnd->EnableWindow(FALSE);
	}
}

void CObjectFilterDlg::OnOK() 
{
	//store to app data
	INamedDataPtr	sptrData(GetAppUnknown());
	sptrData->SetupSection(_bstr_t(m_strNameDataSection));

	long nEntries = 0, i = 0;
	BSTR bstrName = 0;
	sptrData->GetEntriesCount(&nEntries);
	for(i = nEntries-1; i >= 0; i--)
	{
		sptrData->GetEntryName(i, &bstrName);
		sptrData->DeleteEntry(bstrName);
		::SysFreeString(bstrName);
	}

	int nRows = m_grid.GetRowCount();
	for(i = 1; i < nRows; i++)
	{
		CString strParamName = m_grid.GetItemText(i, 0);
		CString strLo = m_grid.GetItemText(i, 1);
		CString strHi = m_grid.GetItemText(i, 2);
		double fLo = atof(strLo);
		double fHi = atof(strHi);
		long nParamID = 0;
		m_mapParams2.Lookup(strParamName, nParamID);
		strParamName.Format("%d", nParamID);
		double fCoeffToUnits=1;
		m_mapParams1Unit.Lookup(nParamID, fCoeffToUnits);
		CString strSection = m_strNameDataSection + "\\" + strParamName;
		::SetValue(::GetAppUnknown(), strSection, "lo", fLo/fCoeffToUnits); //!!! /fCoeffToUnits - чтобы из юнитов перевести в метры
		::SetValue(::GetAppUnknown(), strSection, "hi", fHi/fCoeffToUnits);
	}
	
	CDialog::OnOK();
}

void CObjectFilterDlg::OnHelp() 
{
	HelpDisplay( "ObjectFilterDlg" );
}

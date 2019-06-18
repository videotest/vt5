#include "stdafx.h"
#include "ArrangeDlg.h"
#include "COMMCTRL.H"
#include "object5.H"

void _AddString(HWND hwndList, long nIdx, _bstr_t bstr)
{
	LVITEM item;
	item.mask = LVIF_TEXT;
	item.iItem = nIdx;
	item.iSubItem = 0;
	item.pszText = (char*)bstr;
	item.state = 0;
	item.stateMask = 0;
	item.iImage = 0;
	item.lParam = 0;
	int n = ::SendMessage(hwndList, LVM_INSERTITEM, 0, (LPARAM)&item);
}

CArrangeDlg::CArrangeDlg(IUnknown* punkClassList)
{
	m_sptrClassList = punkClassList;
}

CArrangeDlg::~CArrangeDlg()
{
	long nPos = m_listClassesKeys.head();
	while(nPos)
		delete m_listClassesKeys.next(nPos);

	m_listClassesKeys.deinit();
}

bool CArrangeDlg::DoInitDialog()
{
	HWND hwndList = GetDlgItem(hwnd(), IDC_LIST_CLASSES);

	ListView_SetExtendedListViewStyle(hwndList, LVS_EX_FULLROWSELECT);

	_rect rcList = NORECT;
	GetWindowRect(hwndList, &rcList);

	//insert col
	LVCOLUMN column;
	column.mask = LVCF_FMT|LVCF_WIDTH;
	column.pszText = 0;
	column.fmt = LVCFMT_LEFT;
	column.cx = rcList.width()-20;
	::SendMessage(hwndList, LVM_INSERTCOLUMN, 0, (LPARAM)&column);

	long nIdx = 0;
	if(m_sptrClassList != 0)
	{
		long lPos = 0;
		m_sptrClassList->GetFirstChildPosition(&lPos);
		while(lPos)
		{
			IUnknown* punkChild = 0;
			m_sptrClassList->GetNextChild(&lPos, &punkChild);

			m_listClassesKeys.insert(new GuidKey(GetKey(punkChild)));

			/*BSTR bstr;
			::StringFromCLSID(GetKey(punkChild), &bstr);
			_AddString(hwndList, nIdx++, bstr);
			::CoTaskMemFree(bstr);*/

			_AddString(hwndList, nIdx++, GetName(punkChild));

			if(punkChild) punkChild->Release();
		}
	}

	ListView_SetSelectionMark(hwndList, 0);
	ListView_SetItemState(hwndList, 0, LVIS_SELECTED|LVIS_FOCUSED , 0xFFFF);
	return true;
}

void CArrangeDlg::DoCommand( UINT nCmd )
{
	CDialogImpl::DoCommand(nCmd);

	if(nCmd == IDHELP )
	{
		HelpDisplay( "ArrangeDlg" );
		return;
	}
	if(nCmd == ID_BUTTON_UP || nCmd == ID_BUTTON_DOWN)
	{
		HWND hwndList = GetDlgItem(hwnd(), IDC_LIST_CLASSES);
		IDataObjectListPtr sptrList(m_sptrClassList);

		int nSelect =  ListView_GetSelectionMark(hwndList);
		if(nSelect >= 0 && sptrList != 0)
		{
			long nCounter = 0;
			long posPrev = 0, posObj = 0, posNext = 0;
			long pos = m_listClassesKeys.head();
			GuidKey* pkey;
			while(pos)
			{
				posObj = pos;
				pkey = m_listClassesKeys.next(pos);
				if(nCounter == nSelect)
				{
					posNext = pos;
					break;
				}
				posPrev = posObj;
				nCounter++;
			}
			char szName[255];
			ListView_GetItemText(hwndList, nSelect, 0, szName, 255);

			
			
			if(nCmd == ID_BUTTON_UP)
			{
				//to head side
				if(posPrev && posObj)
				{
					m_listClassesKeys.remove(posObj);
					m_listClassesKeys.insert_before(pkey, posPrev);

					ListView_DeleteItem(hwndList, nSelect);
					
					_AddString(hwndList, nSelect-1, _bstr_t(szName));

					ListView_SetSelectionMark(hwndList, nSelect-1);
					ListView_SetItemState(hwndList, nSelect-1, LVIS_SELECTED|LVIS_FOCUSED , 0xFFFF);

					sptrList->MoveObject(*pkey, TRUE);
				}
			}
			else
			{
				//to tail side
				if(posNext && posObj && posNext != posObj)
				{
					m_listClassesKeys.remove(posObj);
					m_listClassesKeys.insert(pkey, posNext);

					ListView_DeleteItem(hwndList, nSelect);
					_AddString(hwndList, nSelect+1, _bstr_t(szName));

					ListView_SetSelectionMark(hwndList, nSelect+1);
					ListView_SetItemState(hwndList, nSelect+1, LVIS_SELECTED|LVIS_FOCUSED , 0xFFFF);

					sptrList->MoveObject(*pkey, FALSE);
				}
			}
		}
		
	}
}
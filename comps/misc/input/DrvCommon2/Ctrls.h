#if !defined(__CCamCtrls_H__)
#define __CCamCtrls_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Ctrls.h : header file
//

#include "input.h"

CWnd *CreateControlForDialog(HWND hwndDlg, int nId, HWND hwndCtrl);

void InitSlider(HWND hwnd, int id, int nMin, int nMax, int nPos, int nTicFreq, BOOL bEnable = TRUE);
void InitSpin(HWND hwnd, int id, int nMin, int nMax, int nPos, BOOL bEnable = TRUE);
void InitSmartEdit(HWND hwnd, int idSlider, int idSpin, int idEdit, int nMin, int nMax, int nPos, int nTicFreq, BOOL bEnable = TRUE);
void InitListBox(HWND hwnd, int id, int nCurSel, COMBOBOXREPRDATA *pReprData = NULL);
void InitComboBox(HWND hwnd, int id, int nCurSel, COMBOBOXREPRDATA *pReprData = NULL);
void InitComboBoxValue(HWND hwnd, int id, int nValue);
void ResetComboBox(HWND hwnd, int id);
void AddComboBoxString(HWND hwnd, int id, LPCTSTR lpsz);
void AddComboBoxString(HWND hwnd, int id, int nId);
bool IsSmartEdit(HWND hwnd, int idSlider, int idEdit);
inline bool IsControl(HWND hwnd, int id) {return ::GetDlgItem(hwnd, id) != 0;}
int  GetListBoxValue(HWND hwnd, int id);
int  GetComboBoxValue(HWND hwnd, int id);
void ChangeValue(HWND hwnd, int idSlider, int idSpin, int idEdit, int nValue);
void SetSliderPos(HWND hwnd, int id, int nPos);
int  GetSliderPos(HWND hwnd, int id);
void SetSpinPos(HWND hwnd, int id, int nPos);

void SetDlgItemDouble(HWND hWnd, int nID, double dVal, int nDigits = 3);
double GetDlgItemDouble(HWND hWnd, int nID);


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

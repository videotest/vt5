// Print.h : main header file for the PRINT DLL
//

#if !defined(AFX_PRINT_H__59DB65AA_F472_40F1_AF1C_436E717320C7__INCLUDED_)
#define AFX_PRINT_H__59DB65AA_F472_40F1_AF1C_436E717320C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "constant.h"

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "PrinterSetup.h"

IUnknown* CreatePrintPreviewData( IUnknown* punkDoc, BOOL bForceDelete, CString& strTemplate,
								 IUnknown** punkReportDocFind);

BOOL ProcessReport( IUnknown** punkReport, IUnknown* punkDoc );

void WriteActiveObject();

CString GetReportDir();
CString GetReportDatDir();


#define PRINT_PREVIEW			"\\PrintPreview"

#define USE_PRESET_TRANSORMATION	"Use preset transformation"
#define PRESET_TRANSORMATION		"Preset transformation"

#define USE_PRESET_ZOOM				"Use preset zoom"
#define PRESET_ZOOM					"Preset zoom"

#define USE_PRESET_MARGINS			"Use preset margins"

#define ACTIVE_VIEW_MARGINS			"\\PrintPreview\\ActiveViewMargins"
#define ONE_OBJECT_PER_PAGE_MARGINS	"\\PrintPreview\\OneObjectPerPageMargins"
#define VERTICAL_AS_IS_MARGINS		"\\PrintPreview\\VerticalAsIsMargins"
#define GALLERY_MARGINS				"\\PrintPreview\\GalleryMargins"
#define FROM_SPLITTER_MARGINS		"\\PrintPreview\\FromSplitterMargins"

#define PRINT_PREVIEW_READ_ONLY		"ReadOnly"
#define PRINT_SHOWTEMPLATESETUPBTN	"ShowTemplateSetupBtn"

#define PRINT_PREVIEW_TEMPL			"\\PrintPreview\\Templates"
#define PRINT_SHOWACTIVEVIEWTEMPL	"ShowActiveViewTempl"
#define PRINT_SHOWONEELEMENTTEMPL	"ShowOneElementTempl"
#define PRINT_SHOWVERTICALTEMPL		"ShowVerticalTempl"
#define PRINT_SHOWGALLERYTEMPL		"ShowGalleryTempl"
#define PRINT_SHOWFROMSPLITTER		"ShowFromSplitterTempl"
#define PRINT_OTHER_DOC_TEMPL		"UseOtherDocTempl"
#define PRINT_RPT_TEMPL				"UseRptDirTempl"



bool UsePresetTransformation();
long GetPresetTransformation();

bool UsePresetZoom();
double GetPresetZoom();

bool UsePresetMargins();


void SetGlobalPaperMargins( CString strSection, CRect rc );
CRect GetGlobalPaperMargins( CString strSection );


extern CPrinterSetup g_prnSetup;

/////////////////////////////////////////////////////////////////////////////
// CPrintApp
// See Print.cpp for the implementation of this class
//

class CPrintApp : public CWinApp
{
public:
	CPrintApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPrintApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINT_H__59DB65AA_F472_40F1_AF1C_436E717320C7__INCLUDED_)


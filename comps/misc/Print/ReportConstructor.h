// ReportConstructor.h: interface for the CReportConstructor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTCONSTRUCTOR_H__7221CA4D_1755_495A_9CF2_A7246595D8BB__INCLUDED_)
#define AFX_REPORTCONSTRUCTOR_H__7221CA4D_1755_495A_9CF2_A7246595D8BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "types.h"

class CReportConstructor;
//////////////////////////////////////////////////////////////////////
// class CViewObjectList
//////////////////////////////////////////////////////////////////////
class CViewObjectList{
public:
	CViewObjectList();

	CString m_strType;
	CString m_strObjectName;
	bool	m_bUse;
	int		m_nNum;
	bool	m_bNewGroup;

	void CopyFrom( CViewObjectList* pvo );
};

//////////////////////////////////////////////////////////////////////
// class CViewDescription
//////////////////////////////////////////////////////////////////////
class CViewDescription{
public:
	CViewDescription();
	~CViewDescription();
	bool Init( IUnknown* punkView, IUnknown* punkDocumentSite );
	void DeInit();

	CString GetViewProgID() { return m_strViewProgID;	}
	int GetCol()			{ return m_nCol;			}
	int GetRow()			{ return m_nRow;			}
	CSize GetViewSize()		{ return m_sizeView;		}
	double GetZoom()		{ return m_fZoom;			}
	
protected:	

	CArray<CViewObjectList*, CViewObjectList*> m_arViewObject;
	CString m_strViewProgID;

	int		m_nCol;
	int		m_nRow;
	CSize	m_sizeView;
	CSize	m_sizeClient;
	double	m_fZoom;
	int		m_nDPI;

	friend class CReportConstructor;
};
//////////////////////////////////////////////////////////////////////

class CViewInfo{
public:
	CString m_strProgID;
	IViewPtr ptrView;
};

class CReportConstructor : public CObject  
{

public:
	bool _CreateAllView();
protected:
	bool _DestroyAllView();
	CArray<CViewInfo*, CViewInfo* > m_arAllView;
	IUnknown* GetViewByProgID( CString strProgID );
	void GetViewProgIDSupportByType( CArray<CString,CString>& arViewProgID, CString strType );
	

public:
	CReportConstructor();
	CReportConstructor( IUnknown* punkDoc, bool bGetSettingsFromDocument = true );
	virtual ~CReportConstructor();
	void AttachDocument( IUnknown* punkDoc );
	void Init();	
	void DeInit();

	IUnknown* GetDocumentReportCopyFrom();
	
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//	AutoTemplateAlghoritm == ataActiveView
	//	Active view description
public:
	bool CreateActiveViewDescription( );		

	CViewDescription* GetActiveViewDescription() { return &m_ActiveViewDescription; }
protected:
	CViewDescription m_ActiveViewDescription;
	void DeInitActiveViewDescription();		
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//	AutoTemplateAlghoritm == ataFromSplitter
	//	Splitter view description
public:
	bool CreateSplitterViewDescription( );
	int GetSplitterRowCount(){ return m_nSplitterRowCount; }
	int GetSplitterColCount(){ return m_nSplitterColCount; }
	CSize GetSplitterWndSize(){ return m_sizeSplitterWnd; }

	INT_PTR GetSplitterViewDescriptionCount();
	CViewDescription* GetSplitterViewDescriptionItem( int nIndex );

protected:
	CArray<CViewDescription*, CViewDescription*> m_arSplitterViewDescription;
	CSize	m_sizeSplitterWnd;
	int		m_nSplitterRowCount;
	int		m_nSplitterColCount;

	void DeInitSplitterViewDescription();		
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//	AutoTemplateAlghoritm == ataOneObjectPerPage || ataOneObjectPerPage || ataGallery
	//	Active object
public:
	bool CreateActiveObjectList();
	INT_PTR GetActiveObjectListCount();
	CViewObjectList* GetActiveObjectListItem( int nIndex );

protected:
	CArray<CViewObjectList*, CViewObjectList*> m_arActiveObject;

	bool CreateGroupViewObjectList( CArray<CViewObjectList*, CViewObjectList*>& arOL );
	bool CreateGroupViewObjectListUseBaseGroup( CArray<CViewObjectList*, CViewObjectList*>& arOL );

	void DeInitActiveObjectList();
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



protected:
	sptrIDocumentSite		m_sptrDocSite;
	sptrIDocumentSite		m_sptrDocSiteCopyFrom;

	TemplateSource			m_templateSource;
	CString					m_strTemplateName;

	AutoTemplateAlghoritm	m_autoTemplateAlghoritm;
	TemplateGenerateTarget	m_templateGenerateTarget;

	CString					m_strNewTemplateObjectName;
	CString					m_strNewTemplateFileName;	

public:
	//////////////////////////////////////////////////////////////////////
	TemplateSource GetTemplateSource()			{ return m_templateSource;							}
	void SetTemplateSource(TemplateSource ts )	{ m_templateSource = ts;							}

	//////////////////////////////////////////////////////////////////////
	CString GetTemplateName()					{ return m_strTemplateName;							}
	void SetTemplateName( CString str )			{ m_strTemplateName = str;							}

	//////////////////////////////////////////////////////////////////////
	AutoTemplateAlghoritm GetAutoTemplateAlghoritm()	{ return m_autoTemplateAlghoritm;			}
	void SetAutoTemplateAlghoritm( AutoTemplateAlghoritm ata )	{ m_autoTemplateAlghoritm = ata;	}

	//////////////////////////////////////////////////////////////////////
	TemplateGenerateTarget GetTemplateGenerateTarget() { return m_templateGenerateTarget;			}
	void SetTemplateGenerateTarget( TemplateGenerateTarget tgt ) { m_templateGenerateTarget = tgt;	}

	//////////////////////////////////////////////////////////////////////
	CString GetNewTemplateObjectName()			{ return m_strNewTemplateObjectName;				}
	void SetNewTemplateObjectName( CString str)	{ m_strNewTemplateObjectName = str;					}

	//////////////////////////////////////////////////////////////////////
	CString GetNewTemplateFileName()			{ return m_strNewTemplateFileName;					}
	void SetNewTemplateFileName( CString str )	{ m_strNewTemplateFileName = str;					}
	
public:
	void GetSettingsFromDocument();
	bool StoreCtrlToForm( IUnknown* punkCtrl, IUnknown* punkForm );


public:
	static bool GetAllObjectTypes( CArray<CString, CString>& arTypes );
	static bool GetAvailableViewProgID( CArray<CString, CString>& arViewList );

	//helpers

	IUnknown* GenerateReport();

	IUnknown* CreateReport();
	IUnknown* CreateViewAXCtrl();
	IUnknown* CreateViewAXDataCtrl();
	IUnknown* GetAXCtrlFromDataCtrl( IUnknown* punkViewAXDataCtrl );
	bool SetAXCtrlToDataCtrl( IUnknown* punkViewAXDataCtrl, IUnknown* punkViewAXCtrl  );

	bool GetMatchViewProgID( CArray<CString, CString>& arrViewProgID, CString strObjectType );

	CRect GetReportWorkArea( IUnknown* punkReport );
	
	bool CreateControl( IUnknown** ppunkData, IUnknown** ppunkCtrl );


	//Generating report
	bool FillOut_UseExist( IUnknown** ppunkReport, IUnknown** ppunkReportDocFind );
	bool FillOut_ActiveView( sptrIReportForm form );
	bool FillOut_OneObjectPerPage( sptrIReportForm form );
	bool FillOut_VerticalAsIs( sptrIReportForm form );
	bool FillOut_Gallery( sptrIReportForm form );
	bool FillOut_FromSplitter( sptrIReportForm form );

protected:
	bool ProcessExistReport( IUnknown* punkReport );


	bool GetObjectSizeVTPixel( CString strObjectName, CSize& size, double& fDPI );

	double GetShellDataZoom();
	

};
EXTERN_C __declspec(dllexport) BOOL GenerateAutoReport(int autoMode);
IUnknown* GetSplitterFromDocumentSite( IUnknown* punkDS );

#endif // !defined(AFX_REPORTCONSTRUCTOR_H__7221CA4D_1755_495A_9CF2_A7246595D8BB__INCLUDED_)

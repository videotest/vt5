#pragma once
#include "\vt5\common\ax_ctrl_misc.h"
// ViewLoaderCtrl.h : Declaration of the CViewLoaderCtrl ActiveX Control class.


// CViewLoaderCtrl : See ViewLoaderCtrl.cpp for implementation.
//__declspec(dllimport) BOOL GenerateAutoReport(int autoMode);

class CViewLoaderCtrl : public COleControl
{
	DECLARE_DYNCREATE(CViewLoaderCtrl)
	DECLARE_AX_DATA_SITE(CViewLoaderCtrl)

	IUnknownPtr				m_ptrApp, 
							m_ptrSite;
	BOOL m_PropertyAXStatus;
	long oldPrompt; 
	HMODULE hLib;
	BOOL(*procAutoRep)(int) ;
// Constructor
public:
	CViewLoaderCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();


	void Serialize(CArchive &ar);

// Implementation
protected:
	~CViewLoaderCtrl();

	DECLARE_OLECREATE_EX(CViewLoaderCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CViewLoaderCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CViewLoaderCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CViewLoaderCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL GetPropertyAXStatus();
	afx_msg void SetPropertyAXStatus(BOOL bNewValue);
// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()
	short m_TemplateAlgorithm;


	void OnSerialize();

	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(PrintCtrl, IVTPrintCtrl)				
		com_call Draw( HDC hDC, SIZE sizeVTPixel );
		com_call FlipHorizontal(  );
		com_call FlipVertical(  );
		com_call SetPageInfo( int nCurPage, int nPageCount );
		com_call NeedUpdate( BOOL* pbUpdate );
	END_INTERFACE_PART(PrintCtrl)

		BEGIN_INTERFACE_PART(ActiveXCtrl, IVtActiveXCtrl)
	com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
	com_call GetName( BSTR *pbstrName );	//such as Editor1
		END_INTERFACE_PART(ActiveXCtrl)

	enum 
	{
		dispidTemplateAlgorithm = 3
	};
	SHORT GetTemplateAlgorithm(void);
	void SetTemplateAlgorithm(SHORT newVal);



};


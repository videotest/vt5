#if !defined(AFX_MACRORECORDER_H__8E8E8348_6B33_11D3_A652_0090275995FE__INCLUDED_)
#define AFX_MACRORECORDER_H__8E8E8348_6B33_11D3_A652_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MacroRecorder.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CMacroRecorder command target

class CMacroRecorder : public CCmdTargetEx,
							CEventListenerImpl,
							CRootedObjectImpl
{
	ENABLE_MULTYINTERFACE()
//static pointer
public:
	static CMacroRecorder	*s_pMacroRecorderInstance;

	DECLARE_DYNCREATE(CMacroRecorder)

	CMacroRecorder();           // protected constructor used by dynamic creation
// Attributes
public:
	bool GetRecording() const
	{	return m_bRecording;	}
// Operations
public:
	void Record();
	bool Finalize( IUnknown *punkDD );
	void Cancel();
public:
//virtuals
	virtual void DeInit();
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	virtual void OnChangeParent();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMacroRecorder)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMacroRecorder();

	// Generated message map functions
	//{{AFX_MSG(CMacroRecorder)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CMacroRecorder)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CMacroRecorder)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

protected:	//variables
	CStringArrayEx	m_commands;
	bool			m_bRecording;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MACRORECORDER_H__8E8E8348_6B33_11D3_A652_0090275995FE__INCLUDED_)

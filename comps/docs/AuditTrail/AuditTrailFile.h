#if !defined(AFX_AUDITTRAILFILE_H__D904E4B5_0CC0_4DF8_A66F_77D8A221FA02__INCLUDED_)
#define AFX_AUDITTRAILFILE_H__D904E4B5_0CC0_4DF8_A66F_77D8A221FA02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuditTrailFile.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CAuditTrailFile command target

class CAuditTrailFile : public CFileFilterBase
{
	DECLARE_DYNCREATE(CAuditTrailFile)
	GUARD_DECLARE_OLECREATE(CAuditTrailFile)

public:
	CAuditTrailFile();
	~CAuditTrailFile();

	virtual bool InitNew(){return true;}
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDITTRAILFILE_H__D904E4B5_0CC0_4DF8_A66F_77D8A221FA02__INCLUDED_)

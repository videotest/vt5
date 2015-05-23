// AXfile.h: interface for the CImageFileFilterBase class.
//
//////////////////////////////////////////////////////////////////////

class CAXFileFilter : public CFileFilterBase  
{
	DECLARE_DYNCREATE(CAXFileFilter);
	GUARD_DECLARE_OLECREATE(CAXFileFilter);
	CMap< CString, LPCTSTR, CString, LPCTSTR> m_mapFileNameToTitle;
public:
	CAXFileFilter();
	virtual ~CAXFileFilter();

	virtual bool InitNew(){return true;}
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );

	virtual bool BeforeReadFile();
	virtual bool AfterReadFile();
};


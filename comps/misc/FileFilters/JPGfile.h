
class CJPGFileFilter : public CImageFileFilterBase
{
	DECLARE_DYNCREATE(CJPGFileFilter);
	GUARD_DECLARE_OLECREATE(CJPGFileFilter);
public:
	CJPGFileFilter();
	virtual ~CJPGFileFilter();

	//virtual bool InitNew(){return true;}
	//virtual bool ReadFile( const char *pszFileName );
	//virtual bool WriteFile( const char *pszFileName );
	virtual void OnSetHandlers();
};



class CTIFileFilter : public CImageFileFilterBase
{
	DECLARE_DYNCREATE(CTIFileFilter);
	GUARD_DECLARE_OLECREATE(CTIFileFilter);
public:
	CTIFileFilter();
	virtual ~CTIFileFilter();

	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );

	virtual void OnSetHandlers(){};
};




class CPCXFileFilter : public CImageFileFilterBase
{
	DECLARE_DYNCREATE(CPCXFileFilter);
	GUARD_DECLARE_OLECREATE(CPCXFileFilter);
public:
	CPCXFileFilter();
	virtual ~CPCXFileFilter();

	virtual void OnSetHandlers();
};


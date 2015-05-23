// GIFfile.h: interface for the GIFfile class.
//
//////////////////////////////////////////////////////////////////////

class CGIFileFilter : public CImageFileFilterBase
{
	DECLARE_DYNCREATE(CGIFileFilter);
	GUARD_DECLARE_OLECREATE(CGIFileFilter);
public:
	CGIFileFilter();
	virtual ~CGIFileFilter();

	virtual void OnSetHandlers();
};



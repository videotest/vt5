#pragma once

/////////////////////////////////////////////////////////////////////////////
//
//	class CVirtDoc
//
/////////////////////////////////////////////////////////////////////////////

class CVirtDoc : public CCmdTargetEx, public CNumeredObjectImpl
{
public:
	CVirtDoc();
	~CVirtDoc();

	bool	init();
	void	deinit();

protected:
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();

	IUnknown* m_punk_data;
	IUnknown* m_punk_context;
};

/////////////////////////////////////////////////////////////////////////////
//
//	class CFilterObjectList
//
/////////////////////////////////////////////////////////////////////////////
class CFilterObjectList : public CFilterBase
{
public:
	CFilterObjectList();
	virtual ~CFilterObjectList();
	ENABLE_UNDO();

 	DECLARE_DYNCREATE(CFilterObjectList)
	GUARD_DECLARE_OLECREATE(CFilterObjectList)

	bool			InvokeFilter();

	IUnknownPtr		extract_image_from_list( IUnknown* punk_ol );	
	bool			filter_images( IUnknown* punk_src, IUnknown* punk_target );
	bool			place_image_to_list( IUnknown* punk_image, IUnknown* punk_list );
	bool			_copy_image( IImage3* pimg_src, IImage3* pimg_target, CRect* prc_src, CPoint* ppt_target );
	//get src
	IUnknownPtr		get_src_ol();
	//create target
	bool			create_compatible_ol( IUnknown* punk_src, IUnknown* punk_target );
	//get aaction by arg
	IUnknownPtr		get_action( const char* psz_action );

	//undo changes
	CObjectListUndoRecord	m_changes;	

	CVirtDoc*		m_pvirt_doc;
};
// ActionCreateImageByList.h: interface for the CActionCreateImageByList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIONCREATEIMAGEBYLIST_H__60AAC643_E996_4CEB_AAF5_4B560E2E9F85__INCLUDED_)
#define AFX_ACTIONCREATEIMAGEBYLIST_H__60AAC643_E996_4CEB_AAF5_4B560E2E9F85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CActionCreateImageByList : public CFilterBase  
{
	DECLARE_DYNCREATE(CActionCreateImageByList)
	GUARD_DECLARE_OLECREATE(CActionCreateImageByList)
	BOOL m_bUseColor;
public:
	CActionCreateImageByList();
	virtual ~CActionCreateImageByList();

	virtual bool InvokeFilter();
protected:
	bool _create_image_copy( IUnknown *punkResult, IUnknown *punkSource);
	bool _fill_bk_color( IUnknown *punkImage );
	bool _copy_image( IUnknown *punkResult, IUnknown *punkObject );
	bool CanDeleteArgument( CFilterArgument *pa ) 
	{ 
		if( pa->m_strArgName == "Objects" )
			return false; 
		return true;
	}
};

#endif // !defined(AFX_ACTIONCREATEIMAGEBYLIST_H__60AAC643_E996_4CEB_AAF5_4B560E2E9F85__INCLUDED_)

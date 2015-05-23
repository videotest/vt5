#ifndef __binaryview_h__
#define __binaryview_h__

#include "imagesourceview.h"

/*class CImageViewBase : public CImageSourceView
{
protected:
	MatchView GetMatchType( const char *szType )
	{
		if( !strcmp( szType, szTypeImage ) )
			return mvFull;
		else if( !strcmp( szType, szTypeObjectList ) )
			return mvPartial;
		else if( !strcmp( szType, szDrawingObjectList ) )
			return mvPartial;
		else
			return mvNone;
	};
	virtual void	_AttachObjects( IUnknown *punkContext );
};

class CBinaryViewBase : public CImageSourceView
{
protected:
	MatchView GetMatchType( const char *szType )
	{
		if( !strcmp( szType, szDrawingObjectList ) )
			return mvPartial;
		else if( !strcmp( szType, szTypeBinaryImage ) )
			return mvFull;
		else
			return mvNone;
	};
	virtual void	_AttachObjects( IUnknown *punkContext );
	
};
*/

/*class CObjectsView : public CImageViewBase
{
	DECLARE_DYNCREATE(CObjectsView);
	GUARD_DECLARE_OLECREATE(CObjectsView);
	PROVIDE_GUID_INFO( )
public:
	CObjectsView();

public:
	virtual const char* GetViewMenuName();
	
};

class CMasksView : public CImageViewBase
{
	DECLARE_DYNCREATE(CMasksView);
	GUARD_DECLARE_OLECREATE(CMasksView);
	PROVIDE_GUID_INFO( )
public:
	CMasksView();

public:
	virtual const char* GetViewMenuName();

};

class CSourceView : public CImageViewBase
{
	DECLARE_DYNCREATE(CSourceView);
	GUARD_DECLARE_OLECREATE(CSourceView);
	PROVIDE_GUID_INFO( )
public:
	CSourceView();

public:
	virtual const char* GetViewMenuName();
	bool IsActiveSelectionMode()		{return true;}
};

class CPseudoView : public CImageViewBase
{
	DECLARE_DYNCREATE(CPseudoView);
	GUARD_DECLARE_OLECREATE(CPseudoView);
	PROVIDE_GUID_INFO( )
public:
	CPseudoView();

public:
	virtual const char* GetViewMenuName();

};

class CBinaryForeView : public CBinaryViewBase
{
	DECLARE_DYNCREATE(CBinaryForeView);
	GUARD_DECLARE_OLECREATE(CBinaryForeView);
	PROVIDE_GUID_INFO( )
public:
	CBinaryForeView();

public:
	virtual const char* GetViewMenuName();

};

class CBinaryBackView : public CBinaryViewBase
{
	DECLARE_DYNCREATE(CBinaryBackView);
	GUARD_DECLARE_OLECREATE(CBinaryBackView);
	PROVIDE_GUID_INFO( )
public:
	CBinaryBackView();

public:
	virtual const char* GetViewMenuName();

};

class CBinaryView : public CBinaryViewBase
{
	DECLARE_DYNCREATE(CBinaryView);
	GUARD_DECLARE_OLECREATE(CBinaryView);
	PROVIDE_GUID_INFO( )
public:
	CBinaryView();

public:
	virtual const char* GetViewMenuName();

};

class CBinaryContourView : public CBinaryViewBase
{
	DECLARE_DYNCREATE(CBinaryContourView);
	GUARD_DECLARE_OLECREATE(CBinaryContourView);
	PROVIDE_GUID_INFO( )
public:
	CBinaryContourView();

public:
	virtual const char* GetViewMenuName();

};

*/



#endif //__binaryview_h__
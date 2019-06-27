#pragma once
#include "defs.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_map.h"
#include <atlstr.h>
#include <commctrl.h>
#pragma comment (lib, "comctl32.lib")

#define	GALLERIES_SECTION			"Galleries"
#define	COUNT_KEY					"Count"
#define	FILENAME_KEY				"FileName"
#define	ICONWIDTH_KEY				"IconWidth"
#define	TRANSPARENT_COLOR_KEY		"TransparentColor"
#define	GALLERY_KEY_FORMAT_STRING	"Gallery_%d"
#define	ICON_KEY_FORMAT_STRING		"Icon_%d"

class std_dll CImageContainer
{
public:
	static long cmp_cstring( CString s1, CString s2 )
	{	return s1.Compare( s2 ); }

protected:
	class XContainerItem
	{
		HIMAGELIST	m_himglst;
		_list_map_t<int, CString, CImageContainer::cmp_cstring>	m_mapname2idx;

	public:
        XContainerItem();
		virtual ~XContainerItem();

		bool	Load( LPCTSTR lpfile_name, LPCTSTR lpsection_name );
		bool	GetIconSize( SIZE *psize );
		bool	GetIcon( LPCTSTR lpimage_name, HICON *phicon );

		long	GetImageCount() { return m_mapname2idx.count(); }
		bool	GetByIdx( long lidx, CString *pstr_name, HICON *phicon );
		    
	protected:
		void	free();
	};

	static void _xci_free( void *pv ){	delete (XContainerItem*)(pv); }
	_list_t< XContainerItem*, CImageContainer::_xci_free > m_lstitems;
public:
	CImageContainer();
	~CImageContainer();
    
	bool	Load( LPCTSTR lpfile_name );
    bool	GetIcon( SIZE size_icon, LPCTSTR lpicon_name, HICON *phicon );
	
	long	GetImageCount();
	bool	GetImageInfoByIdx( long lidx, CString *pstr_name, SIZE *psize );
	bool	GetIconByIdx( long lidx, HICON *phicon );

	void	GetMaxSize( SIZE *psize );

protected:
	void	free();
};
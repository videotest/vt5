#pragma once

// dialog resize map macros
#define BEGIN_DLGRESIZE_MAP(thisClass) \
	static const _DlgResizeMap* get_dlg_resize_map() \
	{ \
		static const _DlgResizeMap theMap[] = \
		{

#define END_DLGRESIZE_MAP() \
			{ -1, 0 }, \
		}; \
		return theMap; \
	}

#define DLGRESIZE_CONTROL(id, flags ) \
		{ id, flags },

#define BEGIN_DLGRESIZE_GROUP() \
		{ -1, _DLG_BEGIN_GROUP },

#define END_DLGRESIZE_GROUP() \
		{ -1, _DLG_END_GROUP },

#include "\vt5\awin\misc_map.h"

#define IMPLEMENT_RESIZE( _handle_ ) \
	HWND get_handle() { return _handle_; }

template < class TMain >
class resizable_dlg_impl
{
	struct FRect
	{
		double l,t,r,b;
	};

	static void _del_struct( FRect *data )
	{
		delete data;
	}

	_map_t< FRect *, long, cmp_long, resizable_dlg_impl::_del_struct > m_mapCoord;
	long m_cxPrev, m_cyPrev;
public:
	enum
	{
		DLG_SIZE_X		= 0x00000001,
		DLG_SIZE_Y		= 0x00000002,
		DLG_MOVE_X		= 0x00000004,
		DLG_MOVE_Y		= 0x00000008,

		// internal use only
		_DLG_BEGIN_GROUP	= 0x00001000,
		_DLG_END_GROUP		= 0x00002000,
	};

	struct _DlgResizeMap
	{
		int nCtlID;
		DWORD dwResizeFlags;
	};

	void init_resize()
	{
		TMain* pT = static_cast<TMain*>(this);

		RECT _rectClient = {0};
		::GetClientRect( pT->get_handle(), &_rectClient );

		m_cxPrev = _rectClient.right  - _rectClient.left;
		m_cyPrev = _rectClient.bottom - _rectClient.top;
	}

	void resize_control( short cx, short cy )
	{
		TMain* pT = static_cast<TMain*>(this);
		const _DlgResizeMap *pMap =  pT->get_dlg_resize_map();

		RECT _rectClient = {0};
 		::GetClientRect( pT->get_handle(), &_rectClient );

		long cxPrev = m_cxPrev;
		long cyPrev = m_cyPrev;

		int iDX = cx - cxPrev;
		int iDY = cy - cyPrev;

		if( pMap )
		{
			int i = 0;
			
			int nGroupSz = 1;
			while( pMap[i].nCtlID != -1 || pMap[i].dwResizeFlags != 0 )
			{
				if( pMap[i].dwResizeFlags == _DLG_END_GROUP )
				{
					nGroupSz = 1;
					i++;
					continue;
				}

				if( pMap[i].dwResizeFlags == _DLG_BEGIN_GROUP )
				{
					nGroupSz = 0;
					int z = i + 1;
					while( pMap[z].nCtlID != -1 || pMap[z].dwResizeFlags != _DLG_END_GROUP )
					{
						z++;
						nGroupSz++;
					}
					i++;
				}

				HWND hWnd = ::GetDlgItem( pT->get_handle(), pMap[i].nCtlID );

				if( !hWnd )
				{
					i++;
					continue;
				}

				TPOS lpos = m_mapCoord.find( pMap[i].nCtlID );

				if( !lpos )
				{
					RECT rcCtrl = {0};

					::GetWindowRect( hWnd, &rcCtrl );
					::ScreenToClient( pT->get_handle(), (POINT *)&rcCtrl );
					::ScreenToClient( pT->get_handle(), (POINT *)(&rcCtrl) + 1 );

					FRect *frc = new FRect;
					frc->l = rcCtrl.left;
					frc->t = rcCtrl.top;
					frc->b = rcCtrl.bottom;
					frc->r = rcCtrl.right;
					
					m_mapCoord.set( frc, pMap[i].nCtlID );
				}
				
				lpos = m_mapCoord.find( pMap[i].nCtlID );

				FRect *frc = m_mapCoord.get( lpos );

				RECT rc_prev = { int( frc->l ), int( frc->t ), int( frc->r ), int( frc->b ) };
				RECT rc_inv_l = { 0 };
				RECT rc_inv_t = { 0 };
				RECT rc_inv_r = { 0 };
				RECT rc_inv_b = { 0 };

				bool binvalidate = false;
				
				if( pMap[i].dwResizeFlags & DLG_SIZE_X )
					frc->r += iDX / (double)nGroupSz;

				if( pMap[i].dwResizeFlags & DLG_SIZE_Y )
					frc->b += iDY / (double)nGroupSz;

 				if( pMap[i].dwResizeFlags & DLG_MOVE_X )
				{
					binvalidate = true;
					
					frc->l += iDX / (double)nGroupSz;
					frc->r += iDX / (double)nGroupSz;

					if( iDX > 0 )
					{
						rc_inv_l = rc_prev;
						rc_inv_l.right = int( frc->l );
					}
					else
					{
						rc_inv_r = rc_prev;
						rc_inv_r.left = int( frc->r );
					}
				}

  				if( pMap[i].dwResizeFlags & DLG_MOVE_Y )
				{
					binvalidate = true;

					frc->t += iDY / (double)nGroupSz;
					frc->b += iDY / (double)nGroupSz;

					if( iDY > 0 )
					{
						rc_inv_t = rc_prev;
						rc_inv_t.bottom = int( frc->t );
					}
					else
					{
						rc_inv_b = rc_prev;
						rc_inv_b.top = int( frc->b );
					}
				}

				RECT rc_new = { int( frc->l ), int( frc->t ), int( frc->r ), int( frc->b ) };

				if( binvalidate )
				{
					if( !::IsRectEmpty( &rc_inv_l ) )
						::InvalidateRect( pT->get_handle(), &rc_inv_l, true );

					if( !::IsRectEmpty( &rc_inv_r ) )
						::InvalidateRect( pT->get_handle(), &rc_inv_r, true );

					if( !::IsRectEmpty( &rc_inv_t ) )
						::InvalidateRect( pT->get_handle(), &rc_inv_t, true );

					if( !::IsRectEmpty( &rc_inv_b ) )
						::InvalidateRect( pT->get_handle(), &rc_inv_b, true );
				}

 				::MoveWindow( hWnd, rc_new.left, rc_new.top, rc_new.right - rc_new.left, rc_new.bottom - rc_new.top, TRUE );
				
				i++;
			}
		}

  		m_cxPrev = cx; 
		m_cyPrev = cy; 
	}
};
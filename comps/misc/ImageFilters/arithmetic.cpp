#include "stdafx.h"
#include "arithmetic.h"
#include "image5.h"

#include "win_dlg.h"
#include "commctrl.h"
#include "misc_utils.h"

_ainfo_base::arg	CCombineFilterInfo::s_pargs[] = 
{	
	{"ShowInterface",	szArgumentTypeInt, "1", true, false },
	{"A",	szArgumentTypeDouble, "1", true, false },
	{"B",	szArgumentTypeDouble, "0", true, false },
	{"C",	szArgumentTypeDouble, "0", true, false },
	{"img1",		szArgumentTypeImage, 0, true, true },
	{"img2",		szArgumentTypeImage, 0, true, true },
	{"result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};
_ainfo_base::arg	CAddFilterInfo::s_pargs[] = 
{	
	{"img1",		szArgumentTypeImage, 0, true, true },
	{"img2",		szArgumentTypeImage, 0, true, true },
	{"result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};
_ainfo_base::arg	CSubFilterInfo::s_pargs[] = 
{	
	{"img1",		szArgumentTypeImage, 0, true, true },
	{"img2",		szArgumentTypeImage, 0, true, true },
	{"result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};
_ainfo_base::arg	CSubFilterExInfo::s_pargs[] = 
{	
	{"img1",		szArgumentTypeImage, 0, true, true },
	{"img2",		szArgumentTypeImage, 0, true, true },
	{"result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};
_ainfo_base::arg	CSubLightFilterInfo::s_pargs[] = 
{	
	{"img1",		szArgumentTypeImage, 0, true, true },
	{"img2",		szArgumentTypeImage, 0, true, true },
	{"result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};
_ainfo_base::arg	CDiffFilterInfo::s_pargs[] = 
{	
	{"img1",		szArgumentTypeImage, 0, true, true },
	{"img2",		szArgumentTypeImage, 0, true, true },
	{"result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};

bool CCombineFilterBase::InvokeFilter()
{
	if( !InitCoefficients() )
		return false;

	IImage3Ptr	image1( GetDataArgument("img1") );
	IImage3Ptr	image2( GetDataArgument("img2") );
	IImage3Ptr	image( GetDataResult() );

	if( image1==0||image2==0 )
	{
		//"no argument
		return false;
	}

	int	cx1, cy1, cx2, cy2, cx, cy;
	image1->GetSize( &cx1, &cy1 );
	image2->GetSize( &cx2, &cy2 );

	cx = min( cx1, cx2 );
	cy = min( cy1, cy2 );

	IUnknownPtr	ptrCC1, ptrCC2;

	image1->GetColorConvertor( &ptrCC1 );
	image2->GetColorConvertor( &ptrCC2 );

	if( ptrCC1 != ptrCC2 )
	{
		//different color system
		VTMessageBox(IDS_DIFFERENT_MODEL, app::get_instance()->handle(), "", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	IColorConvertor2Ptr	ptrCC( ptrCC1 );
	BSTR	bstrCCName;
	ptrCC->GetCnvName( &bstrCCName );

	if( image->CreateImage( cx, cy, bstrCCName, ::GetImagePaneCount( image1 ) ) != S_OK )
	{
		//can't create image
		return false;
	}
	::SysFreeString( bstrCCName );

	int	panes = ::GetImagePaneCount( image1 );

	int	c, x, y;

	StartNotification( cy, panes );

	for( c = 0; c < panes; c++ )
	{
		if( c != 0 )NextNotificationStage();

		for( y = 0; y < cy; y++ )
		{
			color	*p1, *p2, *p;
			image1->GetRow( y, c, &p1 );
			image2->GetRow( y, c, &p2 );
			image->GetRow( y, c, &p );

			for( x = 0; x < cx; x++, p1++, p2++, p++ )
				*p = (color)max( 0, min( colorMax, *p1*m_k1+*p2*m_k2+colorMax*m_add ) );

			Notify( y );
		}
	}

	FinishNotification();

	return true;
}


class CCombineDlg : public CDialogImpl
{
	dummy_unknown();
protected:
	virtual UINT idd()	{return IDD_COMBINE_IMAGES;}

	virtual bool DoInitDialog()		
	{
		::SetDlgItemInt( hwnd(), IDC_A, m_a, true );
		::SetDlgItemInt( hwnd(), IDC_B, m_b, true );
		::SetDlgItemInt( hwnd(), IDC_C, m_c, true );

		::SendMessage( ::GetDlgItem( hwnd(), IDC_A_SPIN ), UDM_SETRANGE, 0, (LPARAM)MAKELONG( (short)-100, (short)100 ) );
		::SendMessage( ::GetDlgItem( hwnd(), IDC_B_SPIN ), UDM_SETRANGE, 0, (LPARAM)MAKELONG( (short)-100, (short)100 ) );
		::SendMessage( ::GetDlgItem( hwnd(), IDC_C_SPIN ), UDM_SETRANGE, 0, (LPARAM)MAKELONG( (short)-1000, (short)1000 ) );

		return true;
	};
	virtual void DoCommand( UINT nCmd )
	{
		if(nCmd == IDOK)
		{
			BOOL	sucess = true;
			m_a = (int)::GetDlgItemInt( hwnd(), IDC_A, &sucess, true );
			m_b = (int)::GetDlgItemInt( hwnd(), IDC_B, &sucess, true );
			m_c = (int)::GetDlgItemInt( hwnd(), IDC_C, &sucess, true );
		}
		else if( nCmd == IDHELP )
			HelpDisplay( "CombineDlg" );
		CDialogImpl::DoCommand( nCmd );
	}
public:
	int m_a, m_b, m_c;
};

bool CCombineFilter::InitCoefficients()
{
	CCombineDlg	dialog;

	m_k1 = GetArgDouble( "A" );
	m_k2 = GetArgDouble( "B" );
	m_add = GetArgDouble( "C" );

	if( GetArgLong( "ShowInterface" ) )
	{
		m_k1 = GetValueDouble( GetAppUnknown(), "\\Combine", "A", m_k1 );
		m_k2 = GetValueDouble( GetAppUnknown(), "\\Combine", "B", m_k2 );
		m_add = GetValueDouble( GetAppUnknown(), "\\Combine", "C", m_add );

		dialog.m_a = int( m_k1*100 +.5 );
		dialog.m_b = int( m_k2*100 +.5 );
		dialog.m_c = int( m_add*256 +.5 );

		if( dialog.DoModal() != IDOK )
			return false;

		m_k1 = dialog.m_a/100.;
		m_k2 = dialog.m_b/100.;
		m_add = dialog.m_c/256.;

		SetValue( GetAppUnknown(), "\\Combine", "A", m_k1 );
		SetValue( GetAppUnknown(), "\\Combine", "B", m_k2 );
		SetValue( GetAppUnknown(), "\\Combine", "C", m_add );
	}

	return true;
}

CCombineFilterBase::CCombineFilterBase()
{
	//OutputDebugString( "CCombineFilterBase\n" );
}


bool CMulFilter::InvokeFilter()
{
	IImage3Ptr	image1( GetDataArgument("img1") );
	IImage3Ptr	image2( GetDataArgument("img2") );
	IImage3Ptr	image( GetDataResult() );

	double	fMul = GetArgDouble( "mul" );


	if( image1==0||image2==0 )
	{
		//"no argument
		return false;
	}

	int	cx1, cy1, cx2, cy2, cx, cy;
	image1->GetSize( &cx1, &cy1 );
	image2->GetSize( &cx2, &cy2 );

	cx = min( cx1, cx2 );
	cy = min( cy1, cy2 );

	IUnknownPtr	ptrCC1, ptrCC2;

	image1->GetColorConvertor( &ptrCC1 );
	image2->GetColorConvertor( &ptrCC2 );

	if( ptrCC1 != ptrCC2 )
	{
		//different color system
		return false;
	}
	IColorConvertor2Ptr	ptrCC( ptrCC1 );
	BSTR	bstrCCName;
	ptrCC->GetCnvName( &bstrCCName );

	if( image->CreateImage( cx, cy, bstrCCName, ::GetImagePaneCount( image1 ) ) != S_OK )
	{
		//can't create image
		return false;
	}
	::SysFreeString( bstrCCName );

	int	panes = ::GetImagePaneCount( image1 );

	int	c, x, y;

	StartNotification( cy, panes );

	for( c = 0; c < panes; c++ )
	{
		if( c != 0 )NextNotificationStage();

		for( y = 0; y < cy; y++ )
		{
			color	*p1, *p2, *p;
			image1->GetRow( y, c, &p1 );
			image2->GetRow( y, c, &p2 );
			image->GetRow( y, c, &p );

			for( x = 0; x < cx; x++, p1++, p2++, p++ )
				*p = (color)max( 0, min( colorMax, *p1*fMul* *p2 ) );

			Notify( y );
		}
	}

	FinishNotification();

	return true;
}

bool CDivFilter::InvokeFilter()
{
	IImage3Ptr	image1( GetDataArgument("img1") );
	IImage3Ptr	image2( GetDataArgument("img2") );
	IImage3Ptr	image( GetDataResult() );

	double	fMul = GetArgDouble( "mul" );


	if( image1==0||image2==0 )
	{
		//"no argument
		return false;
	}

	int	cx1, cy1, cx2, cy2, cx, cy;
	image1->GetSize( &cx1, &cy1 );
	image2->GetSize( &cx2, &cy2 );

	cx = min( cx1, cx2 );
	cy = min( cy1, cy2 );

	IUnknownPtr	ptrCC1, ptrCC2;

	image1->GetColorConvertor( &ptrCC1 );
	image2->GetColorConvertor( &ptrCC2 );

	if( ptrCC1 != ptrCC2 )
	{
		//different color system
		return false;
	}
	IColorConvertor2Ptr	ptrCC( ptrCC1 );
	BSTR	bstrCCName;
	ptrCC->GetCnvName( &bstrCCName );

	if( image->CreateImage( cx, cy, bstrCCName, ::GetImagePaneCount( image1 ) ) != S_OK )
	{
		//can't create image
		return false;
	}
	::SysFreeString( bstrCCName );

	int	panes = ::GetImagePaneCount( image1 );

	int	c, x, y;

	StartNotification( cy, panes );

	for( c = 0; c < panes; c++ )
	{
		if( c != 0 )NextNotificationStage();

		for( y = 0; y < cy; y++ )
		{
			color	*p1, *p2, *p;
			image1->GetRow( y, c, &p1 );
			image2->GetRow( y, c, &p2 );
			image->GetRow( y, c, &p );

			for( x = 0; x < cx; x++, p1++, p2++, p++ )
			{
				if( *p2 )*p = (color)max( 0, min( colorMax, *p1*fMul / *p2 ) );
				else *p = 0;
			}

			Notify( y );
		}
	}

	FinishNotification();

	return true;
}

bool CSubFilterEx::InvokeFilter()
{
	if( !InitCoefficients() )
		return false;

	IImage3Ptr	image1( GetDataArgument("img1") );
	IImage3Ptr	image2( GetDataArgument("img2") );
	IImage3Ptr	image( GetDataResult() );

	if( image1==0||image2==0 )
	{
		//"no argument
		return false;
	}

	int	cx1, cy1, cx2, cy2, cx, cy;
	image1->GetSize( &cx1, &cy1 );
	image2->GetSize( &cx2, &cy2 );

	cx = min( cx1, cx2 );
	cy = min( cy1, cy2 );

	IUnknownPtr	ptrCC1, ptrCC2;

	image1->GetColorConvertor( &ptrCC1 );
	image2->GetColorConvertor( &ptrCC2 );

	if( ptrCC1 != ptrCC2 )
	{
		//different color system
		VTMessageBox(IDS_DIFFERENT_MODEL, app::get_instance()->handle(), "", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	IColorConvertor2Ptr	ptrCC( ptrCC1 );
	BSTR	bstrCCName;
	ptrCC->GetCnvName( &bstrCCName );

	if( image->CreateImage( cx, cy, bstrCCName, ::GetImagePaneCount( image1 ) ) != S_OK )
	{
		//can't create image
		return false;
	}
	::SysFreeString( bstrCCName );

	int	panes = ::GetImagePaneCount( image1 );

	int	c, x, y;

	StartNotification( cy, panes );

	for( c = 0; c < panes; c++ )
	{
		if( c != 0 )NextNotificationStage();

		for( y = 0; y < cy; y++ )
		{
			color	*p1, *p2, *p;
			image1->GetRow( y, c, &p1 );
			image2->GetRow( y, c, &p2 );
			image->GetRow( y, c, &p );

			for( x = 0; x < cx; x++, p1++, p2++, p++ )
				*p = (color)max( 0, min( colorMax, abs( *p1*m_k1+*p2*m_k2+colorMax*m_add ) ) );

			Notify( y );
		}
	}

	FinishNotification();

	return true;
}
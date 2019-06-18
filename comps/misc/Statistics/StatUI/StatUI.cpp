// StatUI.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "misc_const.h"
#include "statui_consts.h"

#include "statisticobjectfilter.h"
#include "statisticobject.h"
#include "StatUI.h"
#include "statobjectview.h"
#include "CmpStatObjectView.h"
#include "CmpStatObject.h"
#include "actions.h"
#include "CmpActions.h"

#include "MenuRegistrator.h"

#include "\vt5\awin\misc_module.h"

using namespace ObjectSpace;
using namespace ObjectSpace::ObjectActions;
using namespace ViewSpace;
using namespace ViewSpace::ViewActions;


#ifdef _DEBUG
#pragma comment (lib, "\\vt5\\common2\\debug\\common2.lib")
#pragma comment (lib, "\\vt5\\Controls2\\debug\\controls2.lib")
#else
#pragma comment (lib, "\\vt5\\common2\\release\\common2.lib")
#pragma comment (lib, "\\vt5\\Controls2\\release\\controls2.lib")
#endif//_DEBUG

namespace{
	struct CStatUIModule:CAtlDllModuleT<CStatUIModule>{}_AtlModule;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		static module	_module( 0 );
		_module.init( (HINSTANCE)hModule );

		new App;
		App::instance()->Init( (HINSTANCE)hModule );

		CMenuRegistrator rcm;
		rcm.RegisterMenu( szStatObjectViewNameMenu, 0, szStatObjectViewNameMenuUI );
		
		//Register Statistic Object
		App::instance()->ObjectInfo( _dyncreate_t<CStatisticObjectInfo>::CreateObject, clsidStatObjectInfo, szStatObjectInfoProgID, "named data objects" );
		App::instance()->ObjectInfo( _dyncreate_t<CStatisticObject>::CreateObject, clsidStatObject, szStatObjectProgID );				

		App::instance()->ObjectInfo( _dyncreate_t<CCmpStatObjectInfo>::CreateObject, clsidCmpStatObjectInfo, szCmpStatObjectInfoProgID, "named data objects" );
		App::instance()->ObjectInfo( _dyncreate_t<CCmpStatObject>::CreateObject, clsidCmpStatObject, szCmpStatObjectProgID);

		//Register StatObject File Filter
		App::instance()->ObjectInfo( _dyncreate_t<CStatisticObjectFilter>::CreateObject, clsidStatObjectFileFilter, szStatObjectFileFilterProgID, "Image\\export-import" );					

		//Register StatObjectView
		App::instance()->ObjectInfo( _dyncreate_t<CStatObjectView>::CreateObject, clsidStatObjectView, szStatObjectViewProgID, "Image\\Views" );

		//Register CmpStatObjectView
		App::instance()->ObjectInfo( _dyncreate_t<CCmpStatObjectView>::CreateObject, clsidCmpStatObjectView, szCmpStatObjectViewProgID, "Image\\Views" );
	
		//Register Action ShowStatObjectView
		App::instance()->ObjectInfo( _dyncreate_t<CShowStatObjectViewInfo>::CreateObject, clsidShowStatObject, szShowStatObjectProgID, pszActionSection );

		App::instance()->ObjectInfo( _dyncreate_t<CActionChartViewOnOffInfo>::CreateObject, clsidChartViewOnOff, szActionChartViewOnOffProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionLegendViewOnOffInfo>::CreateObject, clsidLegendViewOnOff, szActionLegendViewOnOffProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionTableViewOnOffInfo>::CreateObject, clsidTableViewOnOff, szActionTableViewOnOffProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionCreateStatObjectInfo>::CreateObject, clsidActionCreateStatObject, szActionCreateStatObjectProgID, pszActionSection );

		App::instance()->ObjectInfo( _dyncreate_t<CActionAssignVirtualClassesInfo>::CreateObject, clsidActionAssignVirtualClasses, szActionAssignVirtualClassesProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionShowVirtualClassesInfo>::CreateObject, clsidActionShowVirtualClasses, szActionShowVirtualClassesProgID, pszActionSection );

		App::instance()->ObjectInfo( _dyncreate_t<CShowStatCmpViewInfo>::CreateObject, clsidShowStatCmp, szShowStatCmpProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionChartCmpOnOffInfo>::CreateObject, clsidChartCmpOnOff, szActionChartCmpOnOffProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionLegendCmpOnOffInfo>::CreateObject, clsidLegendCmpOnOff, szActionLegendCmpOnOffProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionTableCmpOnOffInfo>::CreateObject, clsidTableCmpOnOff, szActionTableCmpOnOffProgID, pszActionSection );

		ClassFactory* pfactory = (ClassFactory*)ClassFactory::CreateObject();
		pfactory->LoadObjectInfo();
		pfactory->Release();
	}
	else if( ul_reason_for_call == DLL_PROCESS_DETACH )
	{
		App::instance()->Deinit();
		App::instance()->Release();
	}
    return TRUE;
}

implement_com_exports();

namespace ViewSpace 
{

	void ErrorMessage( HWND hwnd, UINT nID )
	{
		CString str, str2;
		str.LoadString( nID );
		str2.LoadString( IDS_ERROR_TITLE );

		::MessageBox( hwnd, str, str2, MB_OK | MB_ICONERROR );
	}

	double Fnormal(double x, double a, double b )
	{ return exp(-(x-a)*(x-a)/(b*2))/(b*sqrt(PI*2.)); } 

	double FNormalMax(double sigma )
	{ return 1./(sqrt(PI*2)*sigma); } 

	double FLogNormal(double x, double M, double S )
	{
		double y = log(x) - M;
		return exp(-y*y/(2.*S*S)) * M_1_SQRT_2_PI / (S*x); 
	} 

	CFontObj::CFontObj()
		:LOGFONT(lfDefault)
		,m_hFont(0)
		,m_lfHeight(0)
		,m_clrText(0)
	{
	}

	CFontObj::CFontObj(LOGFONT& lf,COLORREF& clrText)
		:LOGFONT(lfDefault)
		,m_hFont(0)
		,m_lfHeight(0)
		,m_clrText(clrText)
	{
	}

	CFontObj::~CFontObj()
	{
		if(m_hFont)::DeleteObject(m_hFont);
	}

	void CFontObj::Delete()
	{
		if(m_hFont)::DeleteObject(m_hFont);
		m_hFont = 0;
		m_lfHeight = 0;
	}

	// Calcuates __min height for given width and font size 
	// and resize rect by these height and width
	int CFontObj::MeasureString(Gdiplus::Graphics& g, CStringW& text, LPRECT lpRect, UINT uFormat)
	{
		//		RectF rf((float)lpRect->left,(float)lpRect->top,float(lpRect->right-lpRect->left),float(lpRect->bottom-lpRect->top));
		RectF rf(0.,0.,0.,0.);

		StringFormat stringFormat;
		StringAlignment align;
		UINT uHorizontAlign = uFormat & 0x3; // Only horizontal formats
		if(DT_LEFT == uHorizontAlign)
			align=StringAlignmentNear;
		if(DT_CENTER == uHorizontAlign)
			align=StringAlignmentCenter;
		if(DT_RIGHT == uHorizontAlign)
			align=StringAlignmentFar;
		stringFormat.SetAlignment(align);

		//		HFONT hf=(HFONT)::SelectObject(g.GetHDC(),m_hFont);
		HDC hdc=g.GetHDC(); Gdiplus::Font f(hdc,m_hFont); g.ReleaseHDC(hdc);
		Gdiplus::Status fs=f.GetLastStatus();
		BOOL b=f.IsAvailable();
		Gdiplus::Status s=g.MeasureString(CStringW(text), -1, &f, rf,
			&stringFormat, 
			&rf);
		lpRect->right = lpRect->left + (int)ceil(rf.Width);
		lpRect->bottom = lpRect->top + (int)ceil(rf.Height);
		return s;
	}

	int CFontObj::DrawString(Gdiplus::Graphics& g, const CStringW& text, LPRECT lpRect, UINT uFormat)
	{
		return DrawString(g, text, lpRect, uFormat, m_clrText);
	}
	int CFontObj::DrawString(Gdiplus::Graphics& g, const CStringW& text, LPRECT lpRect, UINT uFormat, COLORREF clrText)
	{
		RectF rf((float)lpRect->left,(float)lpRect->top,float(lpRect->right-lpRect->left),float(lpRect->bottom-lpRect->top));
		//		HFONT hf=(HFONT)::SelectObject(g.GetHDC(),m_hFont);
		HDC hdc=g.GetHDC(); Gdiplus::Font f(hdc,m_hFont); g.ReleaseHDC(hdc);
		//		Gdiplus::Status fs=f.GetLastStatus();
		//		_ASSERTE(f.IsAvailable());

		StringFormat stringFormat;
		StringAlignment align;
		UINT uHorizontAlign = uFormat & 0x3; // Only horizontal formats
		if(DT_LEFT == uHorizontAlign)
			align=StringAlignmentNear;
		if(DT_CENTER == uHorizontAlign)
			align=StringAlignmentCenter;
		if(DT_RIGHT == uHorizontAlign)
			align=StringAlignmentFar;
		stringFormat.SetAlignment(align);
		if(DT_VCENTER & uFormat){
			stringFormat.SetLineAlignment(StringAlignmentCenter);
		}
		if(0x10000000 & uFormat){
			RectF rotate(0., 0., rf.Height, rf.Width);
			GraphicsContainer gContainer=g.BeginContainer();
			g.TranslateTransform(rf.X, rf.GetBottom());
			g.RotateTransform(-90.);
			Gdiplus::Status s=g.DrawString(text, -1, &f, rotate,
				&stringFormat, 
				&SolidBrush(Color(GetRValue(clrText),GetGValue(clrText),GetBValue(clrText))));
			g.EndContainer(gContainer);
			return s;
		}else{
			Gdiplus::Status s=g.DrawString(CStringW(text), -1, &f, rf,
				&stringFormat, 
				&SolidBrush(Color(GetRValue(clrText),GetGValue(m_clrText),GetBValue(clrText))));
			return s;
		}
	}

	int CFontObj::SetHeight(int Height)
	{
		_ASSERT(Height!=0);
		if(m_lfHeight!=Height)
		{
			m_lfHeight = Height;
			if(m_hFont)::DeleteObject(m_hFont);
			LOGFONT lf = *this;
			lf.lfHeight=m_lfHeight;
			_ASSERT(Height!=0);
			m_hFont = ::CreateFontIndirect( &lf );
		}
		_ASSERTE(m_hFont);
		return m_lfHeight;
	}

	LRESULT CTitleTableWnd::OnPaint(HDC hdc){

		CPaintDC dc(m_hWnd);
		if(!_strTitle.IsEmpty())
		{
			RECT rc;
			GetClientRect(&rc);
			WTL::CMemoryDC dcMem((HDC)dc,rc);
			::FillRect(dcMem,&rc,::GetSysColorBrush(COLOR_BTNFACE));
			::SetBkMode(dcMem,TRANSPARENT);
			LOGFONT lf = m_lfFontTitle;
			lf.lfHeight = long(-MulDiv( lf.lfHeight, GetDeviceCaps( dcMem, LOGPIXELSY), 72));
			::SetTextColor( dcMem, m_clTextColorTitle );

			HFONT hFontTitle = ::CreateFontIndirect( &lf );
			HFONT hFontOld = (HFONT)::SelectObject(dcMem, hFontTitle);
			::DrawText( dcMem, _strTitle, _strTitle.GetLength(), 
				&rc, DT_CENTER | DT_NOCLIP );
			::SelectObject( dcMem, hFontOld );
			::DeleteObject( hFontTitle );
		}
		return 0;
	}

}
#include "stdafx.h"
#include "CommonPropPage.h"
#include "misc_utils.h"

CEditorPropBase::CEditorPropBase(UINT nIDTemplate) : CPropertyPageBase(nIDTemplate)
{
	m_pbWidth = 0;
	m_arrWidths = 0;
	m_nWidthButtons = 0;
	m_bLineWidthWithBase = false;
	m_bColor = false;
	m_bPalette = false;
	m_nVisibleButtons = 0;
}

void CEditorPropBase::OnFinalRelease() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPageBase::OnFinalRelease();
}


void CEditorPropBase::_Init(CString strBase, bool bLineWidthWithBase)
{
	m_bLineWidthWithBase = bLineWidthWithBase;
	m_strBase = strBase;
	m_nActiveWidth = ::GetValueInt(GetAppUnknown(), "Editor_pages", strBase+"WidthActiveBtn", 0);
	m_nActiveWidth = max(0, min(m_nActiveWidth, m_nVisibleButtons-1));
	CString str;
	for(long i = 0; i < m_nWidthButtons; i++)
	{
		str.Format("%sWidth_%d", strBase, i);
		m_arrWidths[i] = ::GetValueInt(GetAppUnknown(), "Editor_pages", str, (i+1)*(i+1));
		m_pbWidth[i].SetWidth(m_arrWidths[i]);
	}
	_ManageWidthButtons();
}

void CEditorPropBase::_DeInit()
{
	::SetValue(GetAppUnknown(), "Editor_pages", m_strBase+"WidthActiveBtn", _variant_t(m_nActiveWidth));
	CString str;
	for(long i = 0; i < m_nWidthButtons; i++)
	{
		str.Format("%sWidth_%d", m_strBase, i);
		::SetValue(GetAppUnknown(), "Editor_pages", str, _variant_t(m_arrWidths[i]));
	}


	
	if(m_pbWidth)
	{
		//[paul] 18.12.2001
		for(long i = 0; i < m_nWidthButtons; i++)
		{
			if( m_pbWidth[i] && m_pbWidth[i].GetSafeHwnd() )
				m_pbWidth[i].DestroyWindow();
				

		}
		delete [] m_pbWidth;
		m_pbWidth = 0;
	}
	if(m_arrWidths)
	{
		delete m_arrWidths;
		m_arrWidths = 0;
	}
}

void CEditorPropBase::_ManageWidthButtons(bool bManageCheck)
{
	for(long i = 0; i < m_nWidthButtons; i++)
	{
		if(m_pbWidth[i].GetChecked() == TRUE)
			m_pbWidth[i].SetChecked(FALSE);
	}
	m_pbWidth[m_nActiveWidth].SetChecked(TRUE);
	::SetValue(GetAppUnknown(), "Editor", m_bLineWidthWithBase ? m_strBase+"LineWidth" : "LineWidth", _variant_t(m_arrWidths[m_nActiveWidth]));
	UpdateAction();
}

void CEditorPropBase::OnWidth(long nBtn)
{
	m_arrWidths[nBtn] = m_pbWidth[nBtn].GetWidth();
	if(m_nActiveWidth == nBtn)
		_ManageWidthButtons(false);
}

void CEditorPropBase::MakeButtons(long nCount, long lVisibleCount)
{
	m_nWidthButtons = nCount;
	ASSERT(m_nWidthButtons > 0);
	m_pbWidth = new CVTWidth[m_nWidthButtons];
	m_arrWidths = new long[m_nWidthButtons];
	m_nVisibleButtons = lVisibleCount<=0?m_nWidthButtons:min(lVisibleCount,m_nWidthButtons);
}

void CEditorPropBase::SetColor(COLORREF color, bool bFore)
{
	if(m_bColor)
		::SetValueColor(GetAppUnknown(), "Editor", bFore?"Fore":"Back", color);
	else
		::SetValueColor(GetAppUnknown(), "Editor", bFore?"Fore gray":"Back gray", color);
	IUnknown* punkChooser = ::GetColorChooser();
	IChooseColorPtr	sptrChooser(punkChooser);
	if(punkChooser)
		punkChooser->Release();
	if(sptrChooser != 0)
	{
		if(bFore)
			sptrChooser->SetLButtonColor((OLE_COLOR)color);
		else
			sptrChooser->SetRButtonColor((OLE_COLOR)color);
	}	
}

int CEditorPropBase::GetColorMode(COLORREF *pPalette)
{
	int bColor = 1;

	sptrIApplication	sptrA( GetAppUnknown() );
	IUnknown		*punkAM = 0;
	sptrA->GetActionManager( &punkAM );
	sptrIActionManager	sptrAM( punkAM );
	if(punkAM)
		punkAM->Release();
	
	if(sptrAM != 0)
	{
		IUnknown* punkAction = 0;
		sptrAM->GetRunningInteractiveAction(&punkAction);
		IActionPtr sptrAction(punkAction);
		if(punkAction)
			punkAction->Release();
		if(sptrAction != 0)
		{
			IUnknown* punkTarget = 0;
			sptrAction->GetTarget(&punkTarget);
			IImageViewPtr sptrIV(punkTarget);
			if(punkTarget)
				punkTarget->Release();

			if(sptrIV != 0)
			{
				IUnknown* punkImage = 0;
				sptrIV->GetActiveImage(&punkImage, 0);
				IImagePtr sptrImage(punkImage);
				if(punkImage)
					punkImage->Release();
				if(sptrImage != 0)
				{
					IUnknownPtr punkCC;
					sptrImage->GetColorConvertor(&punkCC);
					IColorConvertor6Ptr sptrCC(punkCC);
					if (sptrCC != 0)
					{
						sptrCC->GetEditMode(sptrImage, &bColor);
						if (bColor == 2 && pPalette != 0)
							sptrCC->GetPalette(sptrImage, sptrIV, pPalette, 256);
					}
					else
						bColor = ::GetImagePaneCount( sptrImage ) > 1;
				}
			}
		}
	}
	return bColor;
}


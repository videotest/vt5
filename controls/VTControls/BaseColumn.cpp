// BaseColumn.cpp: implementation of the CBaseColumn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VTControls.h"
#include "BaseColumn.h"
#include "ListBaseCtrl.h"

#include <olectl.h>

#include "VTList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// 
// 
// 
// 
//		Cell for BaseColumn
// 
// 
// 
// 
// 
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CBaseCell, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseCell::CBaseCell( )
{	
	m_nPointFontSize = 0;
	m_crFontColor = 0;
	m_bImageListHere	= false;

	m_pListBase			= 0;

	m_ForeColor			= ::GetSysColor(COLOR_WINDOWTEXT);
	m_BackColor			= ::GetSysColor(COLOR_WINDOW);	
	
	m_strSubject		= _T("Subject");
	
	m_Color				= ::GetSysColor(COLOR_WINDOW);	
	
	m_strValue			= _T("Value");	
	
	m_bValue			= TRUE;	

	m_strFontString		= _T("Font Test String");	
	

	CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );

	
	pFont->GetLogFont( &m_logFont );
	//pFont->Detach();

	m_strFontString = m_logFont.lfFaceName;


	m_dwTextAllign		= DT_LEFT;
	

	m_bValue			= 0;
		
	m_showLogicalAsText = FALSE;

	m_idxImage			= -1;
	
	m_strKeyValue		= "";

	m_shadingType		= -1;

}

CBaseCell::~CBaseCell()
{
}


void CBaseCell::SetIconFileName(CString sz_str)
{	

	HICON hIcon = NULL;

	hIcon = (HICON) ::LoadImage( NULL, sz_str, IMAGE_ICON,
								16, 16,
								LR_LOADFROMFILE );
	ASSERT( m_pListBase );

	if( hIcon && m_pListBase )
	{
		if( m_idxImage != -1 )
			m_pListBase->m_imageList.Remove( m_idxImage );
		
		m_idxImage = m_pListBase->m_imageList.Add( hIcon );
		::DestroyIcon( hIcon );
	}	

	if( sz_str == "" )
	{
		if( m_idxImage != -1 )
			m_pListBase->m_imageList.Remove( m_idxImage );

		m_idxImage = -1;
	}

}

void CBaseCell::SetListBase( CListBaseCtrl* pListBase )
{
	m_pListBase = pListBase;

	if( m_imageList.m_hImageList )
	{
		if( m_pListBase && m_idxImage != -1 )
		{
			HICON hIcon = m_imageList.ExtractIcon( m_idxImage );
			m_idxImage = -1;
			if( hIcon )
			{
				m_idxImage = m_pListBase->m_imageList.Add( hIcon );
			}
		}		

		m_imageList.DeleteImageList();
	}

}


void CBaseCell::Serialize(CArchive& ar)
{
	CObject::Serialize( ar );
	
	//ASSERT( m_pListBase );
	
	long	nVersion = 7;

	if( ar.IsStoring( ) )
	{	
		ar << nVersion;

		//imageList.Write( &ar );		
		ar.Write( &m_logFont, sizeof(LOGFONT) );		

		ar << m_ForeColor;
		ar << m_BackColor;
		ar << m_Color;
		ar << m_strSubject;
		ar << m_strValue;
		ar << m_bValue;
		ar << m_showLogicalAsText;		
		ar << m_strFontString;
		ar << m_dwTextAllign;
		ar << m_idxImage;
		ar << m_strKeyValue;

		ar << m_nPointFontSize;
		ar << m_crFontColor;
		
		ar << m_shadingType;

	}else
	{		
		ar >> nVersion;
		

		if( nVersion < 4 )
		{			
			m_imageList.Create(16,16,ILC_COLOR4|ILC_MASK, 2, 1);		
			m_imageList.DeleteImageList( );
			m_imageList.Read( &ar );

		}

		ar.Read( &m_logFont, sizeof(LOGFONT) );

		m_logFont.lfCharSet = DEFAULT_CHARSET;

		ar >> m_ForeColor;
		ar >> m_BackColor;
		ar >> m_Color;
		ar >> m_strSubject;
		ar >> m_strValue;
		ar >> m_bValue;
		ar >> m_showLogicalAsText;		
		ar >> m_strFontString;
		ar >> m_dwTextAllign;
		ar >> m_idxImage;
		ar >> m_strKeyValue;

		if( nVersion >= 5 )
			ar >> m_nPointFontSize;
		if( nVersion >= 6 )
			ar >> m_crFontColor;
		if( nVersion >=7)
			ar >> m_shadingType;
	}	
}



CString CBaseCell::GetAsString( datatype_e datatype )
{	
	switch(datatype)
	{
	case TYPE_SUBJECT:
		return GetAsSubject();
		break;
	case TYPE_LOGICAL:
		return GeAsLogicalText();
		break;
	case TYPE_COLOR:
		return _T("Color");
		break;
	case TYPE_FONT:
		return GetAsFontString();
		break;
	case TYPE_VALUE	:
	case TYPE_VALUE_COMBO:
		return GetAsValue();
		break;
	case TYPE_SHADING:
		return _T("Shading");
		break;
	default:
		break;
	}

	return "Not supported";


}
//////////////////////////////////////////////////////////////////////
// 
// 
// 
// 
//		Base Column for ListCtrl
// 
// 
// 
// 
// 
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CBaseColumn, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseColumn::CBaseColumn( )
{
	m_pListBase = 0;
	
	m_dwCaptionAllign		= LVCFMT_LEFT;
	m_strCaption	= _T("");
	m_nColWidth		= 0;

	m_datatype		= TYPE_VALUE;	


	CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );

	
	pFont->GetLogFont( &m_def_logFont );
	//pFont->Detach();


	m_b_use_def_Font		= TRUE;

	m_def_dwTextAllign		= DT_LEFT;
	m_b_use_def_TextAllign	= TRUE;

	m_def_ForeColor			= ::GetSysColor(COLOR_WINDOWTEXT);
	m_b_use_def_ForeColor	= TRUE;

	m_def_BackColor			= ::GetSysColor(COLOR_WINDOW);
	m_b_use_def_BackColor	= TRUE;	

	m_def_showLogicalAsText	= FALSE;

	m_idxTrue = m_idxFalse	= m_idxGray = -1;

	m_bReadOnly				= FALSE;

	m_bUseImage				= FALSE;

}


CBaseColumn::~CBaseColumn()
{
	for( int i=0;i<m_columnValues.GetSize();i++ )
		delete m_columnValues[i];

	m_columnValues.RemoveAll( );
}


void CBaseColumn::AddColumn( int iCol, datatype_e datatype, CString strCaption, 
						CListBaseCtrl* pListCtrl, int colWidth, DWORD dwCaptionAlign )
{	

	m_strCaption		= strCaption;
	m_dwCaptionAllign	= dwCaptionAlign;
	m_nColWidth			= colWidth;

	m_datatype			= datatype;	

	if( pListCtrl->IsWasCreated() )
		AddColumnToListCtrl(  iCol,  pListCtrl );
		
}

BOOL CBaseColumn::AddColumnToListCtrl( int iCol, CListBaseCtrl* pListCtrl )
{
	ASSERT(pListCtrl);
	return pListCtrl->InsertColumn( iCol, m_strCaption, LVCFMT_CENTER, /*m_dwCaptionAllign*/
			m_nColWidth);	

}


BOOL CBaseColumn::AddRowToListCtrl( int iCol, int iRow, CListBaseCtrl* pListCtrl )
{
	ASSERT(pListCtrl);
	LVITEM lvitem;		
	ZeroMemory( &lvitem, sizeof(lvitem) );


	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = iRow;

	lvitem.pszText = "test";

	if (iCol == 0){
		lvitem.iSubItem = 0;
		return pListCtrl->InsertItem(&lvitem);

	}else{
		lvitem.iSubItem = iCol;
		return pListCtrl->SetItem(&lvitem);

	}
	

}


void CBaseColumn::AddRow(  int iCol, int iRow, CListBaseCtrl* pListCtrl )
{
	CBaseCell* pCell = new CBaseCell;
	pCell->SetListBase( pListCtrl );

	if( pListCtrl->IsWasCreated() )
		AddRowToListCtrl( iCol, iRow,  pListCtrl );

	
	//If def values set then set from column to cell
	if(m_b_use_def_Font)
	{	
		pCell->SetFont( m_def_logFont );
	}

	   
	if( m_b_use_def_TextAllign )
		pCell->m_dwTextAllign = m_def_dwTextAllign;	
		 
	
	if( m_b_use_def_ForeColor ){
		pCell->m_ForeColor = m_def_ForeColor;		
	}

	if( m_def_showLogicalAsText ){
		pCell->m_showLogicalAsText = m_def_showLogicalAsText;
	}


	

	if( m_b_use_def_BackColor )
		pCell->m_BackColor = m_def_BackColor;
	


	m_columnValues.Add( pCell );



}


void CBaseColumn::DeleteRow( int iRow )
{
	if (( iRow < m_columnValues.GetSize( )) && (iRow >=0))
	{	delete m_columnValues[iRow];
		m_columnValues.RemoveAt( iRow );
	}
		
}

LRESULT CBaseColumn::DrawItem(int iRow,  CDC* pDC, CRect rc, 
							  BOOL b_SelectedItem, BOOL b_SelectedRow,
							   CListBaseCtrl* pListCtrl)
{
	

	ASSERT(pDC);
	ASSERT(pListCtrl);
	//ASSERT( m_pListBase );

	CRect	rectSource = rc;

	if ((iRow < m_columnValues.GetSize()) &&(iRow >= 0))
	{
		CBaseCell* pCell = m_columnValues[iRow];
		ASSERT( pCell );
		
		BOOL b_needsFillFullRow = FALSE;

		if( pListCtrl->UseFullRowSelect() && b_SelectedRow )
			b_needsFillFullRow = TRUE;

		
		switch(m_datatype)
		{
			case TYPE_SUBJECT:								
			{	
				int nIconAlign = 0;
				
				if(m_bUseImage)				
					nIconAlign = VT_LIST_ICON_SIZE+2;							

				DrawText( pListCtrl, pDC, pCell, pCell->GetAsSubject(), rc, 
					b_needsFillFullRow, b_SelectedItem, nIconAlign );

				if(m_bUseImage)				
				{
					if( pCell->m_idxImage >= 0 && m_pListBase )
					{
						

						m_pListBase->m_imageList.Draw( 
									pDC, pCell->m_idxImage, 
									CPoint( 
										rc.left+2,
										rc.top+rc.Height()/2 - VT_LIST_ICON_SIZE/2
										), 
									ILD_TRANSPARENT );
						
					}					
				}

				
				break;
			}

			
			case TYPE_FONT:
			{
				/*
				if( b_SelectedItem )
				{
					CBrush brush;
					brush.CreateSolidBrush( ::GetSysColor(COLOR_HIGHLIGHT) );
					pDC->FillRect( rc, &brush );
				}
				*/
				
				CRect rcSource = rc;
  
				rc.right -= 20;

				CRect rectBtn = rcSource;
				rectBtn.left = rectBtn.right - 21;

				rectBtn.top		+=2;
				rectBtn.bottom	-=2;
				rectBtn.right	-=2;				

				

				DrawText( pListCtrl, pDC, pCell, pCell->GetAsFontString(), rc, 
					b_needsFillFullRow, b_SelectedItem,0, &rcSource);

				
				pDC->DrawFrameControl(&rectBtn, DFC_BUTTON, DFCS_BUTTONPUSH );

				HICON hIcon;

				hIcon = AfxGetApp()->LoadIcon( IDI_BROWSE );
				
				::DrawIconEx(
							pDC->GetSafeHdc(), 
							rectBtn.right - 16, rectBtn.bottom - 16, 
							hIcon, 16, 16, 1, NULL, DI_NORMAL);


				DestroyIcon( hIcon );

				rc = rcSource;



				
				break;
			}
			case TYPE_VALUE:
			case TYPE_VALUE_COMBO:
			{
				DrawText( pListCtrl, pDC, pCell, pCell->GetAsValue(), rc, 
					b_needsFillFullRow, b_SelectedItem);

					CRect ArrowRect;

				if( m_datatype == TYPE_VALUE_COMBO )
				{
					CRect rcSource = rc;				

					{
						rc.top		+=1;
						rc.bottom	-=1;
						rc.right	-=2;

						CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));
						int nArrowWidth = max(::GetSystemMetrics(SM_CXHTHUMB), 5*MarginSize.cx);
						int nArrowHeight = max(::GetSystemMetrics(SM_CYVTHUMB), 5*MarginSize.cy);
						CSize ArrowSize(max(nArrowWidth, nArrowHeight), max(nArrowWidth, nArrowHeight));

						ArrowRect.SetRect(rcSource.right - ArrowSize.cx - MarginSize.cx, 
							rcSource.top + MarginSize.cy, rcSource.right - MarginSize.cx,
							rcSource.bottom - MarginSize.cy);



						pDC->DrawFrameControl(&ArrowRect, DFC_SCROLL, DFCS_SCROLLDOWN );
					}
					rc = rcSource;
				}

				break;
			}


			
			case TYPE_LOGICAL:
			{				

				if( !pCell->IsShowLogicalAsText() && m_pListBase )
				{
					int index;

					BOOL bState = pCell->GetAsLogical();
					
					if( bState == TRUE )
						index = m_idxTrue;
					else
					if( bState == FALSE )
						index = m_idxFalse;
					else
					if( bState == 2 )
						index = m_idxGray;


					
					DrawText( pListCtrl, pDC, pCell, "", rc, 
						b_needsFillFullRow, b_SelectedItem, 0 );


					m_pListBase->m_imageList.Draw( 
									pDC, index, 
									CPoint( 
										rc.left+rc.Width()/2 - 8,
										rc.top+rc.Height()/2 - 
										VT_LIST_ICON_SIZE/2
										), 
									ILD_TRANSPARENT );
				}else
				{
					DrawText( pListCtrl, pDC, pCell, pCell->GeAsLogicalText(), rc, 
						b_needsFillFullRow, b_SelectedItem);
					break;
				}



				break;
			}
			
			case TYPE_COLOR:
			{	
				CRect rcSource = rc;				


				//if( !b_SelectedItem )
				{
					rc.top		+=1;
					rc.bottom	-=1;
					rc.right	-=2;
					

					CBrush brush( 
									pListCtrl->IsWindowEnabled() ? 
											pCell->GetAsColor() : 
											::GetSysColor( COLOR_BTNFACE ) 
								);
					pDC->FillRect( rcSource, &brush );

					CRect ArrowRect;

					CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));
					int nArrowWidth = max(::GetSystemMetrics(SM_CXHTHUMB), 5*MarginSize.cx);
					int nArrowHeight = max(::GetSystemMetrics(SM_CYVTHUMB), 5*MarginSize.cy);
					CSize ArrowSize(max(nArrowWidth, nArrowHeight), max(nArrowWidth, nArrowHeight));

				    ArrowRect.SetRect(rcSource.right - ArrowSize.cx - MarginSize.cx, 
                        rcSource.top + MarginSize.cy, rcSource.right - MarginSize.cx,
                        rcSource.bottom - MarginSize.cy);



					pDC->DrawFrameControl(&ArrowRect, DFC_SCROLL, DFCS_SCROLLDOWN );

				}

				
				rc = rcSource;

				break;
			}
			
			case TYPE_SHADING:
			{	
  				CRect rcSource = rc;	 	 		

				rc.top		+=1;
				rc.bottom	-=1;
				rc.right	-=2; 
					
				CRect ArrowRect;
				CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));
				int nArrowWidth = max(::GetSystemMetrics(SM_CXHTHUMB), 5*MarginSize.cx);
				int nArrowHeight = max(::GetSystemMetrics(SM_CYVTHUMB), 5*MarginSize.cy);
				CSize ArrowSize(max(nArrowWidth, nArrowHeight), max(nArrowWidth, nArrowHeight));

			    ArrowRect.SetRect(rcSource.right - ArrowSize.cx - MarginSize.cx, 
                       rcSource.top + MarginSize.cy, rcSource.right - MarginSize.cx,
                       rcSource.bottom - MarginSize.cy);

				CBrush brush( pListCtrl->IsWindowEnabled() ?/*::GetSysColor(COLOR_3DLIGHT)*/COLORREF(RGB(255,255,255)) : ::GetSysColor(COLOR_BTNFACE));  
				pDC->FillRect( rcSource, &brush ); 
				if(pCell->GetShadingType()>=0) 
				{
					for(int i=0;;i++)
					{
						if(rc.left+2+16*(i+1)<=ArrowRect.left)
						{
							HICON hShadingIcon;
							hShadingIcon = AfxGetApp()->LoadIcon( IDI_SHADING1+pCell->GetShadingType() );
							::DrawIconEx( pDC->GetSafeHdc(), rc.left+2+16*i, rc.top-1 ,	hShadingIcon, 16, 16, 1, 
										NULL, DI_NORMAL);
							DestroyIcon( hShadingIcon );
						}
						else
							break;
					}
				}

				pDC->DrawFrameControl(&ArrowRect, DFC_SCROLL, DFCS_SCROLLDOWN );
				
				rc = rcSource;

				break;
			}
			
			
			default:				
				break;
		}

		DrawSelection( rc, pDC, b_SelectedItem );

	}else
	{
		TRACE("\nWarning: Row index out of range. Point:CBaseColumn DrawItem");
		//return -1;
		//CBrush brush( ::GetSysColor( COLOR_WINDOW ) );
		//pDC->FillRect( rc, &brush );

	}

	return 1;


}			

		 
void CBaseColumn::DrawText(CListBaseCtrl* pListCtrl, CDC* pDC, CBaseCell* pCell, 
							CString str, CRect rc, BOOL b_needsFillFullRow, 
							BOOL b_curSelectedItem, int iIconAlign, CRect* pCellRect)
{
	
	ASSERT(pDC);
	ASSERT(pCell);

	//Draw background
	CBrush brush;
	if( b_needsFillFullRow )		
	{
		//if(b_curSelectedItem)
		//{
			if( pListCtrl && pListCtrl->GetParent() )
				brush.CreateSolidBrush( ((COleControl*)pListCtrl->GetParent())->TranslateColor( pListCtrl->GetActiveCellBackColor() ) );
			else
				brush.CreateSolidBrush( ::GetSysColor(COLOR_HIGHLIGHT) );
		//}
		/*else
		{
			if( pListCtrl->IsWindowEnabled() )
				brush.CreateSolidBrush( pCell->GetBackColor() );
			else			
				brush.CreateSolidBrush( ::GetSysColor( COLOR_BTNFACE ) );
		}*/
	}
	else
	{
		if(b_curSelectedItem)
		{
			if( pListCtrl && pListCtrl->GetParent() )
				brush.CreateSolidBrush( ((COleControl*)pListCtrl->GetParent())->TranslateColor( pListCtrl->GetActiveCellBackColor() ) );
			else
                brush.CreateSolidBrush( ::GetSysColor(COLOR_HIGHLIGHT) );
		}
		else
		{
			if( pListCtrl->IsWindowEnabled() )
				brush.CreateSolidBrush( pCell->GetBackColor() );
			else			
				brush.CreateSolidBrush( ::GetSysColor( COLOR_BTNFACE ) );
		}
	}
		
	
	if( pCellRect )
		pDC->FillRect( *pCellRect, &brush );
	else
		pDC->FillRect( rc, &brush );
	
	COLORREF crColor;
	
	//Select font color:	
	if( b_needsFillFullRow )	
	{
		if( pListCtrl && pListCtrl->GetParent() )
			crColor = pDC->SetTextColor( ((COleControl*)pListCtrl->GetParent())->TranslateColor( pListCtrl->GetActiveCellTextColor() ) );
		else
			crColor = pDC->SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
	}
	else	
	{
		if(b_curSelectedItem)
		{
			if( pListCtrl && pListCtrl->GetParent() )
				crColor = pDC->SetTextColor( ((COleControl*)pListCtrl->GetParent())->TranslateColor( pListCtrl->GetActiveCellTextColor() ) );
			else
            	crColor = pDC->SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
		}
		else
		{
			crColor = pDC->SetTextColor( pCell->GetForeColor() );
		}
	}

	if( pListCtrl->GetSafeHwnd() && !pListCtrl->IsWindowEnabled() )
		pDC->SetTextColor( ::GetSysColor( COLOR_GRAYTEXT ) );
	
	//Now draw text whith style
	DWORD dw_style = 0;

	dw_style = pCell->m_dwTextAllign | DT_END_ELLIPSIS | DT_VCENTER | DT_SINGLELINE;


	CFont font;
	//if( pListCtrl->m_pParent->_GetUseSystemFont() )
		GetFontFromDispatch( pListCtrl->m_pParent, font, true );
	//else
	//	font.CreateFontIndirect( pCell->GetAsFont( ) );

	CFont* pOldFont = pDC->SelectObject( &font );
	
	rc.left		+= iIconAlign;
	rc.left		+= 2 * VT_LIST_SELECTION_ALIGN; 

	rc.top		+= 2 * VT_LIST_SELECTION_ALIGN; 
	rc.bottom	-= 2 * VT_LIST_SELECTION_ALIGN; 

	pDC->DrawText( str , rc, dw_style );
	pDC->SelectObject( pOldFont );


	//font.Detach( );

	pDC->SetTextColor( crColor );

}

void CBaseColumn::DrawSelection(CRect rc, CDC* pDC, BOOL b_selectedItem)
{
	if( b_selectedItem )
	{						  
		CPen* pOldPen = (CPen*)pDC->SelectStockObject( BLACK_PEN );		
		CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( HOLLOW_BRUSH );		
		
		rc.left		+= 1;
		rc.right	-= 1;
		rc.top		+= 1;
		rc.bottom	-= 1;
		
		pDC->DrawFocusRect( &rc );
		pDC->SelectObject( pOldBrush );		
		pDC->SelectObject( pOldPen );		


	}
}




void CBaseColumn::SetListBase( CListBaseCtrl* pListBase )
{
	m_pListBase = pListBase;

	if( m_imageList.m_hImageList )
	{
		if( m_pListBase )
		{
			if( m_idxTrue != -1 )
			{
				HICON hIcon = m_imageList.ExtractIcon( m_idxTrue );
				m_idxTrue = -1;
				if( hIcon )
				{
					m_idxTrue = m_pListBase->m_imageList.Add( hIcon );
				}
			}
			if( m_idxFalse != -1 )
			{
				HICON hIcon = m_imageList.ExtractIcon( m_idxFalse );
				m_idxFalse = -1;
				if( hIcon )
				{
					m_idxFalse = m_pListBase->m_imageList.Add( hIcon );
				}
			}
			if( m_idxGray != -1 )
			{
				HICON hIcon = m_imageList.ExtractIcon( m_idxGray );
				m_idxGray = -1;
				if( hIcon )
				{
					m_idxGray = m_pListBase->m_imageList.Add( hIcon );
				}
			}
		}


		m_imageList.DeleteImageList();
	}


	for( int i=0;i<m_columnValues.GetSize();i++ )
	{
		m_columnValues[i]->SetListBase( m_pListBase );	
	}

}

void CBaseColumn::Serialize(CArchive& ar)
{
	CObject::Serialize( ar );

	//ASSERT( m_pListBase );

	m_columnValues.Serialize( ar );

	long	nVersion = 5;

	if( ar.IsStoring( ) )
	{	

		ar << nVersion;

		//m_imageListForLogical.Write( &ar );		

		ar.Write( &m_def_logFont, sizeof(LOGFONT) );		
		
		ar << m_bReadOnly;
		ar << m_bUseImage;
		ar << m_strCaption;
		ar << m_dwCaptionAllign;
		ar << m_nColWidth;		
		ar << m_b_use_def_Font;
		ar << m_def_dwTextAllign;	
		ar << m_b_use_def_TextAllign;
		ar << m_def_ForeColor;
		ar << m_b_use_def_ForeColor;
		ar << m_def_BackColor;
		ar << m_b_use_def_BackColor;
		ar << m_def_showLogicalAsText;
		ar << DWORD(m_datatype);
		ar << m_idxTrue;
		ar << m_idxFalse;
		ar << m_idxGray;
		

	}else
	{		
		ar >> nVersion;

		if( nVersion < 5 )
		{			
			m_imageList.Create(16,16,ILC_COLOR4|ILC_MASK, 2, 1);		
			m_imageList.DeleteImageList( );
			m_imageList.Read( &ar );

		}


		//m_imageListForLogical.DeleteImageList( );
		//m_imageListForLogical.Read( &ar );
		
		ar.Read( &m_def_logFont, sizeof(LOGFONT) );		

		DWORD dwDataType;
		
		ar >> m_bReadOnly;
		ar >> m_bUseImage;
		ar >> m_strCaption;
		ar >> m_dwCaptionAllign;
		ar >> m_nColWidth;		
		ar >> m_b_use_def_Font;
		ar >> m_def_dwTextAllign;	
		ar >> m_b_use_def_TextAllign;
		ar >> m_def_ForeColor;
		ar >> m_b_use_def_ForeColor;
		ar >> m_def_BackColor;
		ar >> m_b_use_def_BackColor;
		ar >> m_def_showLogicalAsText;
		ar >> dwDataType;
		m_datatype = ((datatype_e)(dwDataType));
		ar >> m_idxTrue;
		ar >> m_idxFalse;

		if( nVersion >= 4 )
		{
			ar >> m_idxGray;
		}

	}


}


short CBaseColumn::ConvertFromEnumToShort( datatype_e dt )
{
	if( dt == TYPE_SUBJECT ) return 0;
	if( dt == TYPE_LOGICAL ) return 1;
	if( dt == TYPE_COLOR ) return 2;
	if( dt == TYPE_FONT ) return 3;
	if( dt == TYPE_VALUE ) return 4;
	if( dt == TYPE_VALUE_COMBO) return 5;
	if( dt == TYPE_SHADING ) return 6;
	
	if( dt == TYPE_NOT_SUPPORTED ) return -1;	
	
	return -1;
}

datatype_e  CBaseColumn::ConvertFromShortToEnum( short index )
{
	if( index == 0) return TYPE_SUBJECT;
	if( index == 1) return TYPE_LOGICAL;
	if( index == 2) return TYPE_COLOR;
	if( index == 3) return TYPE_FONT;
	if( index == 4) return TYPE_VALUE;	
	if( index == 5) return TYPE_VALUE_COMBO;
	if( index == 6) return TYPE_SHADING;
	
	return TYPE_NOT_SUPPORTED;
}

short CBaseColumn::ConvertFromDWORDToShort_Align( DWORD dw_align )
{
	if( dw_align == DT_LEFT   ) return 0;
	if( dw_align == DT_CENTER ) return 1;
	if( dw_align == DT_RIGHT  ) return 2;

	return -1;

}

DWORD CBaseColumn::ConvertFromShortToDWORD_Align( short index )
{
	if( index == 0 ) return DT_LEFT;
	if( index == 1 ) return DT_CENTER;
	if( index == 2 ) return  DT_RIGHT;
	return -1;
}


void CBaseColumn::SetColumnAlign(short nAlign)
{
	DWORD dw_align = ConvertFromShortToDWORD_Align( nAlign );
	m_def_dwTextAllign = dw_align;

	for(int i=0;i<m_columnValues.GetSize();i++)
	{	if(m_columnValues[i])
		{
			m_columnValues[i]->SetTextAlign( m_def_dwTextAllign );
		}
	}
}


short CBaseColumn::GetColumnType( )
{
	  return ConvertFromEnumToShort( m_datatype );
}

short CBaseColumn::GetColumnAlign( )
{
	return ConvertFromDWORDToShort_Align(m_def_dwTextAllign);
}

void CBaseColumn::SetColumnBackColor(OLE_COLOR color)
{	
	m_def_BackColor = color;

	for(int i=0;i<m_columnValues.GetSize();i++)
	{	if(m_columnValues[i])
		{
			m_columnValues[i]->SetBackColor( m_def_BackColor );
		}
	}

}

void CBaseColumn::SetColumnForeColor(OLE_COLOR color)
{
	m_def_ForeColor = color;

	for(int i=0;i<m_columnValues.GetSize();i++)
	{	if(m_columnValues[i])
		{
			m_columnValues[i]->SetForeColor( m_def_ForeColor );
		}
	}
}

void CBaseColumn::SetColumnType(short nType)
{
	SetDataType( ConvertFromShortToEnum(nType) );

	for(int i=0;i<m_columnValues.GetSize();i++)
	{	if(m_columnValues[i])
		{
			m_columnValues[i]->SetFont( m_def_logFont );
		}
	}
}

void CBaseColumn::SetColumnWidth(short nWidth)
{
	m_nColWidth = nWidth;
}

void CBaseColumn::SetColumnCaption(CString strCaption)
{
	m_strCaption = strCaption;
}

void CBaseColumn::SetColumnFont(LOGFONT& lf)
{
	memcpy( &m_def_logFont, &lf, sizeof(LOGFONT) );
	 
	
	for(int i=0;i<m_columnValues.GetSize();i++)
	{	if(m_columnValues[i])
		{
			m_columnValues[i]->SetFont( lf );
		}
	}
}



BOOL CBaseColumn::GetDisplayLogicalAsText( )
{
	return m_def_showLogicalAsText;
}


void CBaseColumn::SetDisplayLogicalAsText( BOOL bValue )
{
	m_def_showLogicalAsText = bValue;

	for(int i=0;i<m_columnValues.GetSize();i++)
	{	if(m_columnValues[i])
		{
			m_columnValues[i]->SetShowLogicalAsText( m_def_showLogicalAsText );
		}
	}
}


BOOL CBaseColumn::IsColumnUseIcon( )
{
	return m_bUseImage;
}


void CBaseColumn::SetColumnUseIcon( BOOL bValue )
{
	m_bUseImage = bValue;
}


LPLOGFONT CBaseColumn::GetDefFont( )
{			
	return &m_def_logFont;
}

void CBaseColumn::SetDefFont(LPLOGFONT lpLF)
{			

	memcpy( &m_def_logFont, lpLF, sizeof(LOGFONT) );

	for( int i=0;i< m_columnValues.GetSize( );i++ )
	{
		if( m_columnValues[i] )
		{
			m_columnValues[i]->SetFont( *lpLF );
		}
	}
}

	
void CBaseColumn::SetIconFileNameForLogical(CString &sz_str, BOOL b_forValue)
{	

	if( !m_pListBase )
		return;


	HICON hIcon = NULL;

	hIcon = (HICON) ::LoadImage( NULL, sz_str, IMAGE_ICON,
								16, 16,
								LR_LOADFROMFILE/* | LR_LOADTRANSPARENT*/);
	if( hIcon )
	{

		int img_index = -1;
		if( b_forValue == 0 )
			img_index = m_idxFalse;
		else
		if( b_forValue == 1 )
			img_index = m_idxTrue;
		else
		if( b_forValue == 2 )
			img_index = m_idxGray;
		
		if( img_index != -1)
		{
			img_index = m_pListBase->m_imageList.Replace( img_index, hIcon );
		}
		else
		{
			img_index = m_pListBase->m_imageList.Add( hIcon );
			::DestroyIcon( hIcon );
		}

		if( b_forValue == 0 )
			m_idxFalse= img_index;
		else
		if( b_forValue == 1 )
			m_idxTrue = img_index;
		else
		if( b_forValue == 2 )
			m_idxGray = img_index;
	}	
}



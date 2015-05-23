// ActionCutArea.cpp: implementation of the CActionCutArea class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CutArea.h"
#include "ActionCutArea.h"
#include <math.h>
#include "misc_utils.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CActionCutArea, CCmdTargetEx);

// {FBA175C1-2BA1-11d6-BA8D-0002B38ACE2C}
GUARD_IMPLEMENT_OLECREATE(CActionCutArea, "CutArea.ImposingDivision", 
0xfba175c1, 0x2ba1, 0x11d6, 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c);

// {FBA175C2-2BA1-11d6-BA8D-0002B38ACE2C}
static const GUID CutAreaAction = 
{ 0xfba175c2, 0x2ba1, 0x11d6, { 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c } };

ACTION_INFO_FULL(CActionCutArea, IDS_MAKE_CUT_AREA, -1, -1, CutAreaAction);
ACTION_TARGET(CActionCutArea, szTargetViewSite);

CActionCutArea::CActionCutArea()
{
	COLORREF Color = ::GetValueColor( ::GetAppUnknown(), "\\Chromos\\Scissors", "Color", RGB(0,0,255) );
	m_pPen = new CPen( PS_SOLID, 1, Color );
	m_bLBtnDown = false;
	m_Type = 0;
	m_bNeedToContinue = false;
	m_hcurOld = m_hcurScissors = 0;
}

CActionCutArea::~CActionCutArea()
{
	if(m_pPen)
		delete m_pPen;

	if( m_hcurScissors )
		::DestroyCursor( m_hcurScissors );
}

bool CActionCutArea::Initialize()
{
	if( !CInteractiveActionBase::Initialize() ) return false;

	if( !m_hcurScissors )
	{
		m_hcurOld = ::GetCursor();
		m_hcurScissors = AfxGetApp()->LoadCursor( IDC_CUT_AREA );
		m_hcurActive = m_hcurScissors;
	}
	return true;
}

void CActionCutArea::Finalize()
{
	if( m_hcurOld ) 
		::SetCursor( m_hcurOld );
	
	if( m_bLBtnDown )
	{
		m_bLBtnDown = false;
		ReleaseCapture();
	}
	CInteractiveActionBase::Finalize();
}

bool CActionCutArea::Invoke()
{
	return _run();
}

bool CActionCutArea::DoLButtonDown( CPoint pt )
{
	if( IsAvaible() )
	{
		m_bLBtnDown = true;

		CWnd *pWnd = GetWindowFromUnknown( m_punkTarget );

		if( !pWnd )
			return false;

		pWnd->SetCapture( );

		m_Points.push_back( pt );
		m_DispPoints.push_back( pt );

		m_ptMax = m_ptMin = pt;
		return true;
	}
	return false; 
}

bool CActionCutArea::DoLButtonUp( CPoint pt )
{
	if( m_bLBtnDown )
	{
		CWnd *pWnd = GetWindowFromUnknown( m_punkTarget );

		if( !pWnd )
			return false;

		ReleaseCapture();

		m_bLBtnDown = false;

		Finalize();
	}
	return false;
}

void CActionCutArea::DoDraw(CDC &dc)
{
	int nSz = m_DispPoints.size();

	if(!nSz)
		return;

	dc.SelectObject(m_pPen);
	dc.SetROP2(R2_COPYPEN);

	CPoint ptOffset( int( ::GetZoom( m_punkTarget ) / 2.0 + 0.5 ), int( ::GetZoom( m_punkTarget ) / 2.0 + 0.5 ) );

 	dc.MoveTo( CPoint( ::ConvertToWindow(m_punkTarget, m_DispPoints[0]) + ptOffset ) );

	for (int i = 1; i < nSz;i++)
		dc.LineTo( CPoint( ::ConvertToWindow(m_punkTarget, m_DispPoints[i]) + ptOffset ) );
}

bool CActionCutArea::IsAvaible()
{
	if( !CheckInterface( m_punkTarget, IID_IImageView ) ) 
		return false;

	IImageViewPtr pstI = m_punkTarget;

	DWORD dwShow = 0;
	pstI->GetImageShowFlags( &dwShow );

	if( dwShow & isfSourceImage )
		return false;
	

	IUnknown *punkList = 0;
	punkList = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	if( punkList )
	{
		INamedDataObject2Ptr sptrL = punkList;
		punkList->Release();

		long lCount = 0;
        sptrL->GetChildsCount( &lCount );

		if( !lCount )
			return false;

		return true;
	}
	return false;
}

bool CActionCutArea::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

bool CActionCutArea::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionCutArea::DoMouseMove(CPoint point)
{
	if( m_bLBtnDown )
	{
		if( m_DispPoints[ m_DispPoints.size() - 1] != point )
			m_DispPoints.push_back( point );

		POINT pt = m_Points[m_Points.size() - 1];
		if( abs( pt.x - point.x ) > 1 || abs( pt.y - point.y ) > 1 )
			_fill_points( m_Points, pt, point );
		else
		{
			if( m_Points[ m_Points.size() - 1] != point )
				m_Points.push_back( point );
		}

		if(point.x < m_ptMin.x) m_ptMin.x = point.x;
		if(point.y < m_ptMin.y) m_ptMin.y = point.y;
		if(point.x > m_ptMax.x) m_ptMax.x = point.x;
		if(point.y > m_ptMax.y) m_ptMax.y = point.y;

//		Invalidate( m_DispPoints[0], point );		
		CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
		if( pwnd )
			pwnd->InvalidateRect( 0 );

	}
//	::SetCursor( m_hcurScissors );
	return false;
}

void CActionCutArea::Invalidate(CPoint Pt1,CPoint Pt2)
{
	CRect rect(min(Pt1.x,Pt2.x) - 1,
		       min(Pt1.y,Pt2.y) - 1,
			   max(Pt1.x,Pt2.x) + 1,
			   max(Pt1.y,Pt2.y) + 1);

	CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
	if( pwnd )
		pwnd->InvalidateRect( ::ConvertToWindow( m_punkTarget, rect ) );
}

bool CActionCutArea::cut_area(sptrIDocument sptrD)
{
	IUnknown *punkDocList = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	bool bFounded = false;

	if( !punkDocList )
		return false;

	INamedDataObject2Ptr	ptrListObject = punkDocList;
	punkDocList->Release();

	BOOL bHasCross;
	do
	{
		bHasCross = false;
		std::vector<IUnknown *>ListPUNKObjects;

		TPOS lpos = 0;
		ptrListObject->GetFirstChildPosition((POSITION*)&lpos);

		while (lpos)
		{
			IUnknown *punkChild = 0;
			ptrListObject->GetNextChild((POSITION*)&lpos, &punkChild);
			
			if( punkChild )
				ListPUNKObjects.push_back( punkChild );
		}
		
		long nSize = ListPUNKObjects.size();
		for(long Pos = 0; Pos < nSize; Pos++ )
		{
			IUnknown *punkImage = 0;

			IMeasureObjectPtr pObject = ListPUNKObjects[Pos];

			if( pObject == 0 )
				continue;
		
			pObject->GetImage( &punkImage );

			if( !punkImage )
				continue;

			IImage3Ptr ptrImage = punkImage;
			punkImage->Release();

			RECT rcImage;
			_get_img_rect( punkImage , rcImage );

			if( _is_true_object( rcImage ) )
			{
				bool bContinue = true;

				int w = rcImage.right - rcImage.left;
				int h = rcImage.bottom - rcImage.top;

				LPBYTE *pSourceImgMask = new LPBYTE[h];
				LPBYTE *pDestImgMask[2];
				
				pDestImgMask[0] = new LPBYTE[h];
				pDestImgMask[1] = new LPBYTE[h];

				for( int j = 0; j < h; j++ )
				{
					pSourceImgMask[j]  = new BYTE[w];
					pDestImgMask[0][j] = new BYTE[w];
					pDestImgMask[1][j] = new BYTE[w];

					LPBYTE tmp;
					ptrImage->GetRowMask( j, &tmp );
					
					for( int i = 0; i < w; i++ )
					{
						pSourceImgMask[j][i] = tmp[i];
						pDestImgMask[0][j][i] = 0;
						pDestImgMask[1][j][i] = 0;
					}
				}

				{
					int sz = m_Points.size();

					bContinue = false;
					for(int i = 0; i < sz;i++)
					{
						int iy = m_Points[i].y - rcImage.top;
						int ix = m_Points[i].x - rcImage.left;

						if( iy >= 0 && iy < h && ix >= 0 && ix < w )
						{
							if( pSourceImgMask[iy][ix] )
							{
								bContinue = true;
								break;
							}
						}
					}
				}

				int nTopID = -1;
				IUnknown *punkNewImage[2];
				punkNewImage[0] = 0;
				punkNewImage[1] = 0;
				if( bContinue )
				{
					if( !fill_dests_img( pDestImgMask, pSourceImgMask, w, h, rcImage ,punkImage, nTopID, punkNewImage ) )
					{
						bContinue = false;
						if( m_bNeedToContinue )
						{
 							m_bNeedToContinue = false;
							bFounded = bHasCross = true;
						}
					}
				}
				
				if( nTopID == -1 )
					bContinue = false;

				if( !punkNewImage[0] && !punkNewImage[1] )
					bContinue = false;

				if( bContinue )
				{
					for( int k = 0; k < 2; k++ )
					{
						if( punkNewImage[k] )
						{
							RECT rcNewImage;
							_get_img_rect( punkNewImage[k], rcNewImage );

							int w = rcNewImage.right - rcNewImage.left - 1;
							int h = rcNewImage.bottom - rcNewImage.top - 1;
							
							rcNewImage.left -= rcImage.left;
							rcNewImage.top -= rcImage.top;

							rcNewImage.right = rcNewImage.left + w;
							rcNewImage.bottom = rcNewImage.top + h;

							_copy_image_data( punkNewImage[k], punkImage, pDestImgMask[k], rcNewImage );
						}
					}
				}
				if( bContinue )
				{
					_bstr_t s(szTypeObject);
					IUnknown	*punk[2];

					punk[0] = punkNewImage[0] ? ::CreateTypedObject(s) : 0;
					punk[1] = punkNewImage[1] ? ::CreateTypedObject(s) : 0;

					if( !punk[0] && !punk[1])
						bContinue = false;

					if( bContinue )
					{
						int nIDs[2] = { !nTopID, nTopID };

						for( int k = 0; k < 2; k++ )
						{
							if( punk[nIDs[k]] )
							{
								long lMinSquare = ::GetValueInt( GetAppUnknown(), "ObjectEditor", "MinSquare", 100 );
								if( _get_square( punkNewImage[nIDs[k]] ) <= lMinSquare )
								{
									punk[nIDs[k]]->Release();
									punk[nIDs[k]] = 0;
									continue;
								}

								IMeasureObjectPtr	ptrO = punk[nIDs[k]];

								INamedDataObject2Ptr ptrObject = ptrO;
								ptrO->SetImage( punkNewImage[nIDs[k]] );
								IImage3Ptr pImg = punkNewImage[nIDs[k]];
								
								punkNewImage[nIDs[k]]->Release();
								punkNewImage[nIDs[k]] = 0;
								
								if( !_is_virtual( punkImage ) )
								{
									DWORD dwFlags = 0;
									ptrObject->GetObjectFlags( &dwFlags );
									ptrObject->SetParent( ptrListObject, dwFlags );
								}
								else
								{
									DWORD dwFlags = sdfAttachParentData | apfNotifyNamedData;
									ptrObject->SetParent( ptrListObject, dwFlags );
								}

								ptrListObject->SetActiveChild( 0 );

								m_changes.SetToDocData( sptrD, ptrObject );

								bool bRecalc = ::GetValueInt(GetAppUnknown(), "\\measurement", "DynamicRecalc", 1) == 1;

								if( bRecalc && ptrObject != 0 )
									::CalcObject( ptrObject, 0, true, 0 );

								punk[nIDs[k]]->Release();
								punk[nIDs[k]] = 0;
								bHasCross = true;
							}
						}

						m_changes.RemoveFromDocData( sptrD, pObject );
						ListPUNKObjects[Pos]->Release();
						ListPUNKObjects[Pos] = 0;
					}
					
				}

				for( int k = 0; k < 2;k++ )
				{
					for( int j = 0; j < h;j++ )
						delete []pDestImgMask[k][j];
					delete [] pDestImgMask[k];
				}

				for( j = 0; j < h;j++ )
					delete []( pSourceImgMask[j] );
				delete [] pSourceImgMask;

				if( !bFounded )
					bFounded = bContinue;

				for( int k = 0; k < 2;k++ )
				{
					if( punkNewImage[k] )
					{
						punkNewImage[k]->Release();
						punkNewImage[k] = 0;
					}
				}
			}
		}

		for( int i = 0; i < ListPUNKObjects.size(); i++ )
		{
			if( ListPUNKObjects[i] )
			{
				ListPUNKObjects[i]->Release();
				ListPUNKObjects[i] = 0;
			}
		}

		ListPUNKObjects.clear();

	} while( bHasCross );
	return bFounded;
}

void CActionCutArea::_get_img_rect(IUnknown *punkImage,RECT &rc)
{
	IImage3Ptr ptrImage = punkImage;

	POINT ptImgOffset;
	ptrImage->GetOffset( &ptImgOffset );

	int w = 0,h = 0;
	ptrImage->GetSize( &w,&h );

	rc.left = ptImgOffset.x;
	rc.top = ptImgOffset.y;

	rc.right = rc.left + w;
	rc.bottom = rc.top + h;
}


BOOL CActionCutArea::_is_true_object(RECT &rc)
{
	int nSz = m_Points.size();
	if(!nSz)
		return false;

	for(int i = 0;i < nSz; i++)
		if( ::PtInRect( &rc, m_Points[i] ) )
			  return true;

	return false;
}

double CActionCutArea::_get_y_by_x(double x,double x1,double y1,double x2, double y2)
{
	if( fabs( x2 - x1 ) < EPS )
		return 0;
	return ( ( x - x1 ) / ( x2 - x1 ) ) *( y2 - y1 ) + y1;
}

IUnknown *CActionCutArea::_create_image( int w,int h, POINT Offset, _bstr_t ColorConvertorName, IUnknown *punkDevided )
{
	_bstr_t s2( szTypeImage );
	IUnknown *punk2 = ::CreateTypedObject( s2 );
	if( !punk2 )
		return 0;

	IImage3Ptr	ptrI = punk2;

	INamedDataObject2Ptr ptrImg = ptrI;


	IImage3Ptr	ptrDevided = punkDevided;
	bool bIsVirtual = _is_virtual( punkDevided );

	if( !bIsVirtual )
	{
		ptrI->CreateImage( w, h, ColorConvertorName, ::GetImagePaneCount( punkDevided ) );
		ptrI->InitRowMasks();
		ptrI->SetOffset( Offset,true );
	}
	else
	{
		CRect rect = CRect( Offset, CSize( w, h ) );

		ptrI->CreateVirtual( rect );
		ptrI->InitRowMasks();

		INamedDataObject2Ptr imageNDO = ptrDevided;

		IUnknown *punkParent = 0;
	
		if( imageNDO )
			imageNDO->GetParent( &punkParent );

		IImage3Ptr imageParent = punkParent;
		if( punkParent != 0 ) 
			punkParent->Release();

		punkParent = 0;

		DWORD dwFlags = sdfAttachParentData | apfNotifyNamedData;
		ptrImg->SetParent( imageParent,  dwFlags );
	}

	return punk2;
}

BOOL CActionCutArea::_copy_image_data(IUnknown *punkNewImage, IUnknown *punkImage, LPBYTE *pDestImgMask, RECT &rcImage )
{																					
	IImage3Ptr ptrNewImage = punkNewImage, ptrOldImage = punkImage;
	int nPanes = ::GetImagePaneCount( ptrOldImage );

	for(int j = rcImage.top; j <= rcImage.bottom; j++ )
	{
		LPBYTE NewImgRow = 0;
		ptrNewImage->GetRowMask( j - rcImage.top, &NewImgRow );

		for (int k = 0; k < nPanes; k++)
		{
			color *SrcRowColor = 0;
			color *DestRowColor = 0;

			ptrOldImage->GetRow( j, k, &SrcRowColor );
			ptrNewImage->GetRow( j - rcImage.top, k, &DestRowColor );
					 
			for(int i = rcImage.left; i <= rcImage.right; i++ )
				DestRowColor[i - rcImage.left] = SrcRowColor[i];
		}

		for(int i = rcImage.left; i <= rcImage.right; i++ )
			NewImgRow[i - rcImage.left] = pDestImgMask[j][i];
	}
	return true;
}

_bstr_t CActionCutArea::_GetCCName(IUnknown *punkImage)
{
	IImage3Ptr pimg = punkImage;

	if( pimg == 0 )
		return _bstr_t("");


	IUnknown	*punkCC1 = 0;
	pimg->GetColorConvertor(&punkCC1);

	if (punkCC1 == 0)
		return _bstr_t("");

	IColorConvertor2Ptr sptrCC1 = punkCC1;
	punkCC1->Release();

	if (sptrCC1 == 0)
		return _bstr_t("");
	
	BSTR bstr;
	sptrCC1->GetCnvName(&bstr);
	_bstr_t bstrN1(bstr);
	::SysFreeString(bstr);
	return bstrN1;
}

BOOL CActionCutArea::fill_dests_img(LPBYTE **pDestMask, LPBYTE *pSrcMask, int w, int h, RECT &rcImage, IUnknown *punkImage, int &nTopID, IUnknown **punkNewImage)
{
	int nSz = m_Points.size();
	
	std::vector<CPoint>TransfPoints;

	for( int i = 0; i < nSz;i++ )
		TransfPoints.push_back( CPoint( m_Points[i].x - rcImage.left, rcImage.bottom - m_Points[i].y ) );

	IImage3Ptr ptrImage = punkImage;
	int nContoursSize = 0;
	ptrImage->GetContoursCount( &nContoursSize );

	if( !nContoursSize)
		return false;

	int nStartID = -1, nEndID = -1;

	int nLastID = -1;
	int nFrstID = 0;
	while( nFrstID < nSz )
	{
		for( int j = nFrstID; j < nSz; j++ )
		{
			if( ::PtInRect( &rcImage, m_Points[j] ) )
			{
				int ix = m_Points[j].x - rcImage.left;
				int iy = m_Points[j].y - rcImage.top;

				if(nStartID == -1)
				{
					if( pSrcMask[iy][ix] == 255 )
						nLastID = nStartID = j;
				}
				else if(nEndID == -1)
				{
					if( pSrcMask[iy][ix] != 255)
					{
						nEndID = nLastID;
						break;
					}
					else if( pSrcMask[iy][ix] == 255)
						nLastID = j;
				}
			}
		}

		if( nStartID != -1 && nEndID == -1 && nLastID != -1)
			nEndID = nLastID;

		if( nStartID == -1 || nEndID == -1 )
			return false;

		{ /********************************************************************************/
			if( m_MinInter.empty() )
			{
				m_MinInter.push_back( nStartID );
				m_MaxInter.push_back( nEndID );
				break;
			}
			else
			{
				int nSize = m_MinInter.size();
				BOOL bFnd = false;
				for( int k = 0; k < nSize; k++ )
				{
					if( nStartID == m_MinInter[k] || nEndID == m_MaxInter[k] )
					{
						bFnd = true;
						break;
					}
				}

				if( !bFnd )
				{
					m_MinInter.push_back( nStartID );
					m_MaxInter.push_back( nEndID );

					break;
				}
				else
				{
					nFrstID = nEndID + 1;

					nStartID = -1;
					nEndID = -1;
					nLastID = -1;
				}
			}

		} /********************************************************************************/
	}

	if( nStartID == -1 || nEndID == -1 )
		return false;

	int nContIDs[2] = { -1, -1 };
	int nIDs[2] = { -1, -1 };

	{
		double len1,len2;
		len1 = len2 = 10000;

		for(int j = 0; j < nContoursSize; j++ )
		{
			Contour *imgContour = 0;
			ptrImage->GetContour( j, &imgContour );
			
			int nContSize = imgContour->nContourSize;

			for( int i = 0; i < nContSize;i++ )
			{
				int ix = imgContour->ppoints[i].x - rcImage.left;
				int iy =  rcImage.bottom - imgContour->ppoints[i].y;

				{
					double len = _hypot( TransfPoints[nStartID].x - ix, TransfPoints[nStartID].y - iy );
					if( len < len1 ) 
					{
						len1 = len;
						nIDs[0] = i;
						nContIDs[0] = j;
					}
				}

				{
					double len = _hypot( TransfPoints[nEndID].x - ix, TransfPoints[nEndID].y - iy );
				 	if( len <= len2) 
					{
						len2 = len;
						nIDs[1] = i;
						nContIDs[1] = j;
					}
				}
			}
		}
	}

	if( nIDs[0] == nIDs[1] && nContIDs[0] == nContIDs[1] && nIDs[0] != -1 )
	{
		m_bNeedToContinue = true;
		return false;
	}


	CMatrix mtSystem;
	{
		CVector vPtInSystem,vPtInWorld, vOrient;
						  
		vOrient[0] = TransfPoints[nEndID].x - TransfPoints[nStartID].x;
		vOrient[1] = TransfPoints[nEndID].y - TransfPoints[nStartID].y;

		vOrient._normalize();

		mtSystem[0][0] = vOrient[0]; // X vector orient 
		mtSystem[0][1] = vOrient[1]; // normalized

		vOrient = _cross_product(CVector(0,0,1),vOrient);
		vOrient._normalize();

		mtSystem[1][0] = vOrient[0];	 // Y vector orient 
		mtSystem[1][1] = vOrient[1];	 // normalized

		mtSystem[3][0] = TransfPoints[nStartID].x; // System position
		mtSystem[3][1] = TransfPoints[nStartID].y; // 

		mtSystem[2][2] = mtSystem[3][3] = 1.0f;

		mtSystem[0][2] = mtSystem[0][3] = 
		mtSystem[1][2] = mtSystem[1][3] = 
		mtSystem[2][0] = mtSystem[2][1] = 
		mtSystem[2][3] = mtSystem[3][2] = 0.0f;

		
		{
			int q = 0;
			for(i = nStartID;i < nEndID;i++)
			{
				if( _inverse_transform( CVector( TransfPoints[i].x,TransfPoints[i].y ),mtSystem )[1] < 0 )
					q++;
			}

			if( nEndID - nStartID != 0 )
			{
				if( q / (double)(nEndID - nStartID) > 0.5f )
				{
					mtSystem[1][0] *= -1;	 // Invert Y axis direction 
					mtSystem[1][1] *= -1;

					mtSystem[2][2] *= -1; 

				}
			}
		}
	}

	BOOL bClockWise = true;

	{
		std::vector<CPoint> ptS;
		for( int j = nStartID; j <= nEndID; j++ )
			ptS.push_back( CPoint( m_Points[j].x, m_Points[j].y ) );
		_fill_points( ptS, ptS[ptS.size() - 1], ptS[0] );

		_check_contour( ptS );

		Contour *pContNew = new Contour;
		pContNew->nContourSize = ptS.size() - 1;
		pContNew->nAllocatedSize = pContNew->nContourSize;
		pContNew->ppoints = new ContourPoint[pContNew->nContourSize];

		for( int i = 0; i < pContNew->nContourSize; i++ )
		{
			pContNew->ppoints[i].x = ptS[i].x;
			pContNew->ppoints[i].y = ptS[i].y;
		}

		bClockWise = _clockwise_direction( pContNew );
		delete []pContNew->ppoints;
		delete pContNew;
	}

	if( nContIDs[0] != nContIDs[1] )
	{
		for(int j = 0; j < h; j++ )
			for(int i = 0; i < w; i++ )
				pDestMask[0][j][i] = pSrcMask[j][i];

		nTopID = 0;
		Contour *cOldCont[2];
		ptrImage->GetContour( nContIDs[0], &cOldCont[0] );
		ptrImage->GetContour( nContIDs[1], &cOldCont[1] );

		Contour *cNewCont = new Contour;
		int nNewSz;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		std::vector<CPoint> PPoints;

//-------------------

		if( !m_Type )
		{
			for( j = nStartID; j <= nEndID; j++ )
			{
				if(  _inverse_transform( CVector( m_Points[j].x - rcImage.left,  rcImage.bottom - m_Points[j].y ), mtSystem )[1] >= 0 )
						PPoints.push_back( CPoint( m_Points[j].x, m_Points[j].y ) );
				else
				{
					for( int t = j; t <= nEndID; t++ )
						if( _inverse_transform( CVector( m_Points[t].x - rcImage.left, rcImage.bottom - m_Points[t].y ), mtSystem )[1] >= 0 )
							break;

					CPoint pt1 = CPoint( m_Points[j].x, m_Points[j].y );
					CPoint pt2 = CPoint( m_Points[t - 1].x, m_Points[t - 1].y );

					_fill_points( PPoints, pt1, pt2 );

					if( t < nEndID )
						j = t - 1;
					else
						break;
				}
			}
		}
		else
		{
			for( j = nStartID; j <= nEndID; j++ )
				PPoints.push_back( CPoint( m_Points[j].x, m_Points[j].y ) );
		}
//-------------------
		BOOL bToInner = cOldCont[0]->lFlags == cfExternal && cOldCont[1]->lFlags != cfExternal;
		BOOL bTwoInternal =  cOldCont[0]->lFlags != cfExternal && cOldCont[1]->lFlags != cfExternal;
		BOOL bFirstDir, bSecondDir; // True -- прямое направление

		if( !bTwoInternal )
		{
			if( bToInner )
			{
				if( !bClockWise )
				{
					if( _clockwise_direction( cOldCont[1] ) )
						bFirstDir  = true;
					else
						bFirstDir  = false;

					if( _clockwise_direction( cOldCont[0] ) )
						bSecondDir = false;
					else
						bSecondDir = true;
				}
				else
				{
					if( !_clockwise_direction( cOldCont[1] ) )
						bFirstDir  = true;
					else
						bFirstDir  = false;

					if( !_clockwise_direction( cOldCont[0] ) )
						bSecondDir = false;
					else
						bSecondDir = true;
				}
			}
			else
			{
				if( bClockWise )
				{
					if( _clockwise_direction( cOldCont[1] ) )
						bFirstDir  = true;
					else
						bFirstDir  = false;

					if( _clockwise_direction( cOldCont[0] ) )
						bSecondDir = false;
					else
						bSecondDir = true;
				}
				else
				{
					if( !_clockwise_direction( cOldCont[1] ) )
						bFirstDir  = true;
					else
						bFirstDir  = false;

					if( !_clockwise_direction( cOldCont[0] ) )
						bSecondDir = false;
					else
						bSecondDir = true;
				}
			}
		}
		else
		{
			if( bClockWise )
			{
				if( !_clockwise_direction( cOldCont[1] ) )
					bFirstDir  = true;
				else
					bFirstDir  = false;

				if( _clockwise_direction( cOldCont[0] ) )
					bSecondDir = false;
				else
					bSecondDir = true;
			}
			else
			{
				if( _clockwise_direction( cOldCont[1] ) )
					bFirstDir  = true;
				else
					bFirstDir  = false;

				if( !_clockwise_direction( cOldCont[0] ) )
					bSecondDir = false;
				else
					bSecondDir = true;
			}
		}

		if( !bFirstDir )
		{
			for( j = nIDs[1]; j >= 0; j-- )
				PPoints.push_back( CPoint( cOldCont[1]->ppoints[j].x, cOldCont[1]->ppoints[j].y ) );

			for( j = cOldCont[1]->nContourSize - 1; j >= nIDs[1]; j-- )
				PPoints.push_back( CPoint( cOldCont[1]->ppoints[j].x, cOldCont[1]->ppoints[j].y ) );
		}
		else
		{
			for( j = nIDs[1]; j < cOldCont[1]->nContourSize; j++ )
				PPoints.push_back( CPoint( cOldCont[1]->ppoints[j].x, cOldCont[1]->ppoints[j].y ) );

			for( j = 0; j <= nIDs[1]; j++ )
				PPoints.push_back( CPoint( cOldCont[1]->ppoints[j].x, cOldCont[1]->ppoints[j].y ) );
		}
//-------------------

		if( !m_Type )
		{
			for( j = nEndID; j >= nStartID; j-- )
			{
				if( _inverse_transform( CVector( m_Points[j].x - rcImage.left, rcImage.bottom - m_Points[j].y ), mtSystem )[1] <= 0 )
							PPoints.push_back( CPoint( m_Points[j].x, m_Points[j].y ) );
				else
				{
					for( int t = j; t >= nStartID; t-- )
						if( _inverse_transform( CVector( m_Points[t].x - rcImage.left, rcImage.bottom - m_Points[t].y ), mtSystem )[1] <= 0 )
							break;

					CPoint pt1 = CPoint( m_Points[j].x, m_Points[j].y );
					CPoint pt2 = CPoint( m_Points[t + 1].x, m_Points[t + 1].y );

					_fill_points( PPoints, pt1, pt2 );

					if( t > nStartID )
						j = t + 1;
					else
						break;
				}
			}
		}
		else
		{
			for( j = nEndID; j >= nStartID; j-- )
				PPoints.push_back( CPoint( m_Points[j].x, m_Points[j].y ) );
		}

//-------------------
		if( bSecondDir )
		{
			for( j = nIDs[0]; j < cOldCont[0]->nContourSize; j++ )
				PPoints.push_back( CPoint( cOldCont[0]->ppoints[j].x, cOldCont[0]->ppoints[j].y ) );

			for( j = 0; j <= nIDs[0]; j++ )
				PPoints.push_back( CPoint( cOldCont[0]->ppoints[j].x, cOldCont[0]->ppoints[j].y ) );
		}
		else
		{
			for( j = nIDs[0]; j >= 0; j-- )
				PPoints.push_back( CPoint( cOldCont[0]->ppoints[j].x, cOldCont[0]->ppoints[j].y ) );

			for( j = cOldCont[0]->nContourSize - 1; j >= nIDs[0]; j-- )
				PPoints.push_back( CPoint( cOldCont[0]->ppoints[j].x, cOldCont[0]->ppoints[j].y ) );
		}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		_check_contour( PPoints );

		nNewSz = PPoints.size();

		cNewCont->nAllocatedSize = cNewCont->nContourSize = nNewSz;
		cNewCont->lReserved = 0;
		
		if( cOldCont[0]->lFlags == cfExternal || cOldCont[1]->lFlags == cfExternal )
			cNewCont->lFlags = cfExternal;
		else
			cNewCont->lFlags = cOldCont[0]->lFlags;

		cNewCont->ppoints = new ContourPoint[ nNewSz ];

		for( i = 0; i < nNewSz; i++ )
		{
			cNewCont->ppoints[i].x = PPoints[i].x;
			cNewCont->ppoints[i].y = PPoints[i].y;
		}

		if( cOldCont[0]->lFlags == cfExternal || cOldCont[1]->lFlags == cfExternal )
		{
			for( int t = 0; t < 2; t++ )
			{
				Contour *imgContour = 0;
				ptrImage->GetContour( nContIDs[t], &imgContour );

				if( imgContour->lFlags == cfExternal )
					continue;

				
				POINT *ptCont;
				int nCnt;

				nCnt = imgContour->nContourSize;

				ptCont = new POINT[nCnt];
				int k = 0;
				for( i = 0; i < nCnt; i++ )
				{
					int ix = imgContour->ppoints[i].x - rcImage.left;
					int iy =  imgContour->ppoints[i].y - rcImage.top;
					ptCont[k++] = CPoint( ix, iy );
				}

				for(j = 0; j < h; j++ )
					for( int i = 0; i < w;i++ )
						if( /*!_is_pt_on_countor( CPoint( i, j ), ptCont, nCnt ) && */_point_in_polygon( ptCont, nCnt, i, j, false ) )
							pDestMask[0][j][i] = 0;

//				_fill_contour( pDestMask[0], w, h, ptCont, nCnt, rcImage, 0, false );
				delete []ptCont;

			}
		}

		punkNewImage[0] = _create_image(rcImage.right - rcImage.left, rcImage.bottom - rcImage.top, CPoint( rcImage.left, rcImage.top ), _GetCCName( punkImage ), punkImage );

		if( !punkNewImage[0] )
		{
			delete []cNewCont->ppoints;
			delete cNewCont;
			return false;
		}
		
		IImage3Ptr ptrNewImg = punkNewImage[0];

		if( _is_contour_correct( pDestMask[0], w, h, cNewCont, rcImage.left, rcImage.top ) )
			ptrNewImg->AddContour( cNewCont );
		else
			ptrNewImg->InitContours();

		for(j = 0; j < nContoursSize; j++ )
		{
			if( j != nContIDs[0] && j != nContIDs[1] )
			{
				Contour *imgContour = 0;
				ptrImage->GetContour( j, &imgContour );

				Contour *imgNewContour = new Contour;
				imgNewContour->nContourSize = imgContour->nContourSize;
				imgNewContour->nAllocatedSize = imgContour->nAllocatedSize;
				imgNewContour->lFlags = imgContour->lFlags;
				imgNewContour->lReserved = imgContour->lReserved;
				imgNewContour->ppoints = new ContourPoint[imgContour->nContourSize];

				for( i = 0; i < imgContour->nContourSize; i++ )
				{
					imgNewContour->ppoints[i].x = imgContour->ppoints[i].x;
					imgNewContour->ppoints[i].y = imgContour->ppoints[i].y;
				}

				if( _is_contour_correct( pDestMask[0], w, h, imgNewContour, rcImage.left, rcImage.top  ) )
					ptrNewImg->AddContour( imgNewContour );
				else
					ptrNewImg->InitContours();
			}
		}
		
	}
	else 
	{
			Contour *imgContour;
			ptrImage->GetContour( nContIDs[0], &imgContour );

			if( !imgContour )
				return false;

			int nContSize = imgContour->nContourSize;

			BOOL bOK = false;
			POINT *ptCont[2];
			int nCnt[2] = { 0, 0 };

			nTopID = 0;

			BOOL bInner;

			{
				std::vector<CPoint> PPoints;
				if( !m_Type )
				{
					for( int j = nStartID; j <= nEndID; j++ )
					{
						if(  _inverse_transform( CVector( m_Points[j].x - rcImage.left,  rcImage.bottom - m_Points[j].y ), mtSystem )[1] >= 0 )
								PPoints.push_back( CPoint( m_Points[j].x, m_Points[j].y ) );
						else
						{
							for( int t = j; t <= nEndID; t++ )
								if( _inverse_transform( CVector( m_Points[t].x - rcImage.left, rcImage.bottom - m_Points[t].y ), mtSystem )[1] >= 0 )
									break;

							CPoint pt1 = CPoint( m_Points[j].x, m_Points[j].y );
							CPoint pt2 = CPoint( m_Points[t-1].x, m_Points[t-1].y );

							_fill_points( PPoints, pt1, pt2 );

							if( t < nEndID )
								j = t - 1;
							else
								break;
						}
					}
				}
				else
				{
					for( int j = nStartID; j <= nEndID; j++ )
						PPoints.push_back( CPoint( m_Points[j].x, m_Points[j].y ) );
				}

				bInner = _fill_points( PPoints, imgContour, nIDs , bClockWise );

				_check_contour( PPoints );

				nCnt[0] = PPoints.size();

				if( PPoints.empty() )
					return false;

				ptCont[0] = new POINT[nCnt[0]];
				
				if( !ptCont[0] )
					return false;

				for(int k = 0; k < nCnt[0]; k++ )
				{
					ptCont[0][k].x = PPoints[k].x;
					ptCont[0][k].y = PPoints[k].y;
				}

			}

			{
				std::vector<CPoint> PPoints;

				if( !m_Type )
				{
					for( int j = nEndID; j >= nStartID; j-- )
					{
						if( _inverse_transform( CVector( m_Points[j].x - rcImage.left, rcImage.bottom - m_Points[j].y ), mtSystem )[1] <= 0 )
									PPoints.push_back( CPoint( m_Points[j].x, m_Points[j].y ) );
						else
						{
							for( int t = j; t >= nStartID; t-- )
								if( _inverse_transform( CVector( m_Points[t].x - rcImage.left, rcImage.bottom - m_Points[t].y ), mtSystem )[1] <= 0 )
									break;

							CPoint pt1 = CPoint( m_Points[j].x, m_Points[j].y );
							CPoint pt2 = CPoint( m_Points[t+1].x, m_Points[t+1].y );

							_fill_points( PPoints, pt1, pt2 );

							if( t > nStartID )
								j = t + 1;
							else
								break;
						}
					}
				}
				else
				{
					for( int j = nEndID; j >= nStartID; j-- )
						PPoints.push_back( CPoint( m_Points[j].x, m_Points[j].y ) );
				}

				_fill_points( PPoints, imgContour, nIDs , bClockWise, bInner ,false );


				_check_contour( PPoints );

				nCnt[1] = PPoints.size();

				if( PPoints.empty() )
				{
					delete []ptCont[0];
					return false;
				}

				ptCont[1] = new POINT[nCnt[1]];

				if( !ptCont[1] )
				{
					delete []ptCont[0];
					return false;
				}


				for( int k = 0; k < nCnt[1]; k++ )
				{
					ptCont[1][k].x = PPoints[k].x;
					ptCont[1][k].y = PPoints[k].y;
				}

			}

			if( imgContour->lFlags == cfExternal )
			{

//				_fill_contour( pDestMask[0], w, h, ptCont[0], nCnt[0], rcImage, 255 );
//				_fill_contour( pDestMask[1], w, h, ptCont[1], nCnt[1], rcImage, 255 );

				for(int j = 0; j < h; j++ )
				{
					for( int i = 0; i < w;i++ )
					{
						if( pSrcMask[j][i] == 255 )
						{
							if( _point_in_polygon( ptCont[0], nCnt[0], rcImage.left + i, rcImage.top + j ) )
								pDestMask[0][j][i] = 255;

							if( _point_in_polygon( ptCont[1], nCnt[1], rcImage.left + i, rcImage.top + j ) )
								pDestMask[1][j][i] = 255;
						}
					}
				}

			
			}
			else
			{
//				_fill_contour( pDestMask[0], w, h, ptCont[0], nCnt[0], rcImage, 255 );
//				_fill_contour( pDestMask[1], w, h, ptCont[1], nCnt[1], rcImage, 255 );

//				if( !m_Type )
//					_fill_contour( pDestMask[1], w, h, ptCont[0], nCnt[0], rcImage, 255 );

				for(int j = 0; j < h; j++ )
				{
					for( int i = 0; i < w;i++ )
					{
						if( pSrcMask[j][i] == 255 )
						{
							if( _point_in_polygon( ptCont[0], nCnt[0], rcImage.left + i, rcImage.top + j ) ) 
								pDestMask[0][j][i] = 255;

							if( _point_in_polygon( ptCont[1], nCnt[1], rcImage.left + i, rcImage.top + j ) ) 
								pDestMask[1][j][i] = 255;

							if( !m_Type )
							{
								if( _point_in_polygon( ptCont[0], nCnt[0], rcImage.left + i, rcImage.top + j ) ) 
									pDestMask[1][j][i] = 255;
							}
						}
					}
				}
			}


			for( int k = 0; k < 2; k++ )
			{
				BOOL bContinue = true;

				RECT rcNewImage;
				rcNewImage.left = rcNewImage.right = rcNewImage.top = rcNewImage.bottom = -1;

				for( int j = 0; j < h; j++ )
				{
					for( int i = 0; i < w; i++ )
					{
						if( pDestMask[k][j][i] )
						{
							if( rcNewImage.left == -1 || i < rcNewImage.left )
								rcNewImage.left = i;

							if( rcNewImage.right == -1 || i > rcNewImage.right )
								rcNewImage.right = i;

							if( rcNewImage.top == -1 || j + 1 < rcNewImage.top )
								rcNewImage.top = j;

							if( rcNewImage.bottom == -1 || j > rcNewImage.bottom )
								rcNewImage.bottom = j;
						}
					}
				}

				if( rcNewImage.top == -1 || rcNewImage.left == -1 )
					bContinue = false;

				if( bContinue )
					punkNewImage[k] = _create_image(rcNewImage.right - rcNewImage.left + 1, rcNewImage.bottom - rcNewImage.top + 1, CPoint( rcImage.left + rcNewImage.left, rcImage.top + rcNewImage.top), _GetCCName( punkImage ), punkImage );

				if( punkNewImage[k] && nCnt[k] > 0)
				{
					Contour *pContNew = new Contour;
					pContNew->nContourSize = nCnt[k];
					pContNew->nAllocatedSize = pContNew->nContourSize;
					pContNew->lFlags = imgContour->lFlags;
					pContNew->lReserved = imgContour->lReserved;
					pContNew->ppoints = new ContourPoint[pContNew->nContourSize];

					for( int i = 0; i < pContNew->nContourSize; i++ )
					{
						pContNew->ppoints[i].x = ptCont[k][i].x;
						pContNew->ppoints[i].y = ptCont[k][i].y;
					}

					IImage3Ptr ptrNewImage = punkNewImage[k];
					if( ptrNewImage == 0 )
					{
						delete []pContNew->ppoints;
						delete pContNew;
					}
					else
					{
						if( _is_contour_correct( pDestMask[k], w, h, pContNew, rcNewImage.left, rcNewImage.top   ) )
							ptrNewImage->AddContour( pContNew );
						else
							ptrNewImage->InitContours();
					}
				}
			}

			for( int i = 0; i < nContoursSize; i++ )
			{
				if( i == nContIDs[0] )
					continue;

				Contour *pCont = 0;
				ptrImage->GetContour( i, &pCont );

				int k = -1;
				for( int j = 0; j < pCont->nContourSize;j++ )
				{
					CPoint pt( pCont->ppoints[i].x, pCont->ppoints[i].y );
					if( _point_in_polygon( ptCont[0], nCnt[0], pt.x, pt.y ) ) 
						k = 0;

					if( _point_in_polygon( ptCont[1], nCnt[1], pt.x, pt.y ) ) 
						k = 1;

					if( pCont->lFlags == cfExternal && imgContour->lFlags != cfExternal )
						k = 1;

					if( k != -1 )
						break;
				}
				if( k != -1 && punkNewImage[k] )
				{
					Contour *pContNew = new Contour;
					
					pContNew->nContourSize   = pCont->nContourSize;
					pContNew->nAllocatedSize = pContNew->nContourSize;
					pContNew->lFlags         = pCont->lFlags;
					pContNew->lReserved      = pCont->lReserved;
					pContNew->ppoints        = new ContourPoint[pContNew->nContourSize];

					for( int i = 0; i < pContNew->nContourSize; i++ )
					{
						pContNew->ppoints[i].x = pCont->ppoints[i].x;
						pContNew->ppoints[i].y = pCont->ppoints[i].y;
					}

					IImage3Ptr ptrNewImage = punkNewImage[k];
					if( _is_contour_correct( pDestMask[k], w, h, pContNew, rcImage.left, rcImage.top   ) )
						ptrNewImage->AddContour( pContNew );
					else
						ptrNewImage->InitContours();
				}
			}
			
			delete []ptCont[0];
			delete []ptCont[1];


	}

	return true;
}

void CActionCutArea::_fill_points(std::vector<CPoint>&Points,POINT ptLast, POINT ptThis)
{
	if( abs(ptThis.x - ptLast.x) >= abs(ptThis.y - ptLast.y) )
	{
		if( ( ptThis.x - ptLast.x ) != 0)
		{
			if( ptLast.x < ptThis.x )
			{
				for(int i = ptLast.x; i <= ptThis.x;i++)
				{
					int y = _get_y_by_x(i,ptLast.x,ptLast.y,ptThis.x,ptThis.y);

					if( i == ptLast.x && ptLast.y == y )
						continue;

					if( i == ptThis.x && ptThis.y == y )
						continue;

					if( !Points.empty() )
					{
						if( Points[ Points.size() - 1 ] != CPoint (i, y ))
							Points.push_back( CPoint( i, y ) );
					}
					else
						Points.push_back( CPoint( i, y ) );

				}
			}
			else
			{
				for(int i = ptLast.x; i >= ptThis.x;i--)
				{
					int y = _get_y_by_x(i,ptLast.x,ptLast.y,ptThis.x,ptThis.y);

					if( i == ptLast.x && ptLast.y == y )
						continue;

					if( i == ptThis.x && ptThis.y == y )
						continue;

					if( !Points.empty() )
					{
						if( Points[ Points.size() - 1 ] != CPoint (i, y ))
							Points.push_back( CPoint( i, y ) );
					}
					else
						Points.push_back( CPoint( i, y ) );

				}
			}
		}
	}
	else if( abs(ptThis.x - ptLast.x) < abs(ptThis.y - ptLast.y) )
	{
		if( ( ptThis.y - ptLast.y ) != 0)
		{
			if( ptLast.y < ptThis.y )
			{
				for(int i = ptLast.y; i <= ptThis.y;i++)
				{
					int x = _get_x_by_y(i,ptLast.x,ptLast.y,ptThis.x,ptThis.y);

					if( i == ptLast.y && ptLast.x == x )
						continue;
					
					if( i == ptThis.y && ptThis.x == x )
						continue;

					if( !Points.empty() )
					{
						if( Points[ Points.size() - 1 ] != CPoint (x, i ) )
							Points.push_back( CPoint( x, i ) );
					}
					else
						Points.push_back( CPoint( x, i ) );
				}
			}
			else
			{
				for(int i = ptLast.y; i >= ptThis.y;i--)
				{
					int x = _get_x_by_y(i,ptLast.x,ptLast.y,ptThis.x,ptThis.y);

					if( i == ptLast.y && ptLast.x == x )
						continue;

					if( i == ptThis.y && ptThis.x == x )
						continue;

					if( !Points.empty() )
					{
						if( Points[ Points.size() - 1 ] != CPoint (x, i ) )
							Points.push_back( CPoint( x, i ) );
					}
					else
						Points.push_back( CPoint( x, i ) );
				}
			}
		}
	}

	if( !Points.empty() )
	{
		if( Points[ Points.size() - 1 ] != CPoint (ptThis.x, ptThis.y ) )
			Points.push_back( CPoint (ptThis.x, ptThis.y ) );
	}
	else
		Points.push_back( CPoint (ptThis.x, ptThis.y ) );
}

double CActionCutArea::_get_x_by_y(double y,double x1,double y1,double x2, double y2)
{
	if( fabs( y2 - y1 ) < EPS )
		return 0;

	return ( ( y - y1 ) / ( y2 - y1 ) ) * ( x2 - x1 ) + x1;
}

double CActionCutArea::_canonic(double x1, double y1,double x2, double y2,double x, double y,int sign)
{
	if( fabs( x2 - x1 ) < EPS )
		return 0;

	if(x1 != x2 && y1 != y2)
		return sign * x * ( y2 - y1 ) / ( x2 - x1 ) + sign*( -x1 * ( y2 - y1 ) / ( x2 - x1 ) + y1 ) - sign * y;
	else if(x1 == x2 && y1 != y2)
	{
		if(x1 == x)
			return 0;
		else if(x > x1)
			return sign;
		else 
			return -sign;
	}
	else if(x1 != x2 && y1 == y2)
	{
		if(y1 == y)
			return 0;
		else if(y > y1)
			return sign;
		else 
			return -sign;
	}
	return 0;
}

BOOL CActionCutArea::_is_pt_on_countor(POINT pt,LPPOINT Pts, int nSz)
{
	for( int i = 1; i < nSz; i++ )
		if( _is_point_on_section( pt, Pts[i - 1], Pts[i] ) )
			return true;

	if( _is_point_on_section( pt, Pts[nSz - 1], Pts[0] ) )
		return true;

	return false;
}

BOOL CActionCutArea::_is_point_on_section(POINT pt, POINT ptA, POINT ptB)
{
	double eps = 0.000001f;

	double tmp = _canonic(ptA.x,ptA.y,ptB.x,ptB.y,pt.x,pt.y,1);
	if(!(tmp < eps && tmp > -eps))
		return false;


	double MinX, MaxX, MinY, MaxY;
	
	if(ptA.x < ptB.x)
	{ MinX = ptA.x; MaxX = ptB.x;	}
	else
	{ MinX = ptB.x; MaxX = ptA.x;	}

	if(ptA.y < ptB.y)
	{ MinY = ptA.y; MaxY = ptB.y;	}
	else
	{ MinY = ptB.y; MaxY = ptA.y;	}

	double dx = pt.x - MinX,dx1 = pt.x - MaxX;
	double dy = pt.y - MinY,dy1 = pt.y - MaxY;

	if(!((pt.x > MinX || (dx < eps && dx > -eps)) && (pt.x < MaxX || (dx1 < eps && dx1 > -eps)) && 
		 (pt.y > MinY || (dy < eps && dy > -eps)) && (pt.y < MaxY || (dy1 < eps && dy1 > -eps))))
		 return false;

	return true;
}


BOOL CActionCutArea::_same_direction( Contour *cFirstCont, Contour *cSecondCont )
{

	CVector vRes[2];
	Contour	*Contrs[2];
	BOOL bDir[2] = { false, false };

	Contrs[0] = cFirstCont;
	Contrs[1] = cSecondCont;


	for( int j = 0; j < 2; j++ )
		bDir[j] = _clockwise_direction( Contrs[j] );
	
	if( bDir[0] == bDir[1] )
		return true;
	return false;
}

BOOL CActionCutArea::_clockwise_direction( Contour *pContour )
{
	CVector vRes;

	for( int i = 0; i < pContour->nContourSize-1; i++ )
	{
		CVector vA( -pContour->ppoints[i].x, -pContour->ppoints[i].y, 0 );
		CVector vB( -pContour->ppoints[i+1].x, -pContour->ppoints[i+1].y, 0 );

		CVector vCrossProduct = _cross_product( vA, vB );

		vRes[0] += vCrossProduct[0]; 
		vRes[1] += vCrossProduct[1]; 
		vRes[2] += vCrossProduct[2]; 
	}

	{
		CVector vA( -pContour->ppoints[pContour->nContourSize - 1].x, -pContour->ppoints[pContour->nContourSize - 1].y, 0 );
		CVector vB( -pContour->ppoints[0].x, -pContour->ppoints[0].y, 0 );

		CVector vCrossProduct = _cross_product( vA, vB );

		vRes[0] += vCrossProduct[0]; 
		vRes[1] += vCrossProduct[1]; 
		vRes[2] += vCrossProduct[2]; 
	}

	if( vRes[2] > 0 )
		return true;
	return false;
}


CVector _inverse_transform(CVector vPtInWorld,CMatrix mtSys)
{
	return vPtInWorld * mtSys.Invert();
}
CVector _transform(CVector vPtInSys,CMatrix mtSys)
{
	return vPtInSys * mtSys;
}


CVector operator*(CMatrix &matr, CVector &vect)
{
	CVector ret;

    double x = vect[0]*matr[0][0] + vect[1]*matr[1][0] + vect[2]* matr[2][0] + matr[3][0];
    double y = vect[0]*matr[0][1] + vect[1]*matr[1][1] + vect[2]* matr[2][1] + matr[3][1];
    double z = vect[0]*matr[0][2] + vect[1]*matr[1][2] + vect[2]* matr[2][2] + matr[3][2];
    double w = vect[0]*matr[0][3] + vect[1]*matr[1][3] + vect[2]* matr[2][3] + matr[3][3];

	if( fabs( w ) < EPS )
		w = 1;

    ret[0] = x / w;
    ret[1] = y / w;
    ret[2] = z / w;

    return ret;
}

CVector operator*(CVector &vect,CMatrix &matr)
{
	return matr * vect;
}

CVector _cross_product(CVector vA,CVector vB)
{
	CVector ret;
	
	ret[0] = vA[1] * vB[2] - vA[2] * vB[1];
	ret[1] = vA[2] * vB[0] - vA[0] * vB[2];
	ret[2] = vA[0] * vB[1] - vA[1] * vB[0];

	return ret;
}

BOOL CActionCutArea::_fill_points(std::vector<CPoint>&Points,Contour *pContour, int *nIDs, BOOL bClockwise, BOOL bIn , BOOL bFirst)
{
	BOOL bDirection = _clockwise_direction( pContour );
	BOOL bInner = false;

	if( pContour->lFlags == cfExternal )
	{
		if( bFirst )
		{
			if( bClockwise )
			{
				if( bDirection )
				{
					if( nIDs[1] > nIDs[0])
					{
						for( int j = nIDs[1]; j < pContour->nContourSize; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );

						for( j = 0; j <= nIDs[0]; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = false;
					}
					else
					{
						for( int j = nIDs[1]; j <= nIDs[0]; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = true;
					}
				}
				else
				{
					if( nIDs[1] > nIDs[0])
					{
						for( int j = nIDs[1]; j >= nIDs[0]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = true;
					}
					else
					{
						for( int j = nIDs[1]; j >= 0; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );

						for( j = pContour->nContourSize - 1; j >= nIDs[0]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = false;
					}
				}
			}
			else
			{
				if( bDirection )
				{
					if( nIDs[1] > nIDs[0])
					{
						for( int j = nIDs[1]; j >= nIDs[0]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = true;
					}
					else
					{
						for( int j = nIDs[1]; j >= 0; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );

						for( j = pContour->nContourSize - 1; j >= nIDs[0]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = false;
					}
				}
				else
				{
					if( nIDs[1] > nIDs[0])
					{
						for( int j = nIDs[1]; j < pContour->nContourSize; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );

						for( j = 0; j <= nIDs[0]; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = false;
					}
					else
					{
						for( int j = nIDs[1]; j <= nIDs[0]; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = true;
					}
				}
			}
		}
		else
		{
			if( bClockwise )
			{
				if( !bDirection )
				{
					if( nIDs[0] > nIDs[1] )
					{
/*						if( bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}
*/
						for( int j = nIDs[0]; j >= nIDs[1]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					}
					else
					{
/*						if( !bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}

*/

						for( int j = nIDs[0]; j >= 0; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );

						for( j = pContour->nContourSize - 1; j >= nIDs[1]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					}
				}
				else
				{
					if( nIDs[0] > nIDs[1] )
					{
/*						if( !bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}

*/

						for( int j = nIDs[0]; j < pContour->nContourSize; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );

						for( j = 0; j <= nIDs[1]; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					}
					else
					{
/*						if( bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}

*/

						for( int j = nIDs[0]; j <= nIDs[1]; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					}
				}
			}
			else
			{
				if( bDirection )
				{
					if( nIDs[0] > nIDs[1] )
					{
/*						if( bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}

*/
						for( int j = nIDs[0]; j >= nIDs[1]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					}
					else
					{
/*						if( !bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}

*/						for( int j = nIDs[0]; j >= 0; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						
						for( j = pContour->nContourSize - 1; j >= nIDs[1]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					}
				}
				else
				{
					if( nIDs[0] > nIDs[1] )
					{
						if( !bIn )
						{
							for( int j = nIDs[0]; j >= nIDs[1]; j-- )
								Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						}
						else
						{
							for( int j = nIDs[0]; j < pContour->nContourSize; j++ )
								Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );

							for( j = 0; j <= nIDs[1]; j++ )
								Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						}
					}
					else
					{
/*						if( bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}
*/
						for( int j = nIDs[0]; j <= nIDs[1]; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					}
				}
			}
		}
	}
	else
	{
		if( bFirst )
		{
			if( !bClockwise )
			{
				if( !bDirection )
				{
					if( nIDs[0] > nIDs[1] )
					{
						for( int j = nIDs[1]; j >= 0; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						for( j = pContour->nContourSize - 1; j >= nIDs[0]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = false;
					}
					else
					{
						for( int j = nIDs[1]; j >= nIDs[0]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = true;
					}

				}
				else
				{
					for( int j = nIDs[1]; j <= nIDs[0]; j++ )
						Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					bInner = true;
				}
			}
			else
			{
				if( !bDirection )
				{
					if( nIDs[1] < nIDs[0])
					{
						for( int j = nIDs[1]; j <= nIDs[0]; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = true;
					}
					else
					{
						for( int j = nIDs[1]; j < pContour->nContourSize; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						for( j = 0; j <= nIDs[0]; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						bInner = false;
					}
				}
				else
				{
					for( int j = nIDs[1]; j >= nIDs[0]; j-- )
						Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					bInner = true;
				}
			}
		}
		else
		{
			if( !bClockwise )
			{
				if( !bDirection )
				{	
					if( nIDs[0] < nIDs[1])
					{
/*						if( !bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}
*/
						for( int j = nIDs[0]; j >= 0; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						for( j = pContour->nContourSize - 1; j >= nIDs[1]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					}
					else
					{
/*						if( bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}

*/
						for( int j = nIDs[0]; j >= nIDs[1]; j-- )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					}
				}
				else
				{
/*					if( !bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}

*/
					for( int j = nIDs[0]; j < pContour->nContourSize; j++ )
						Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					for( j = 0; j <= nIDs[1]; j++ )
						Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
				}
			}
			else
			{
				if( !bDirection )
				{

					if( nIDs[0] > nIDs[1])
					{
/*						if( !bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}

*/
						for( int j = nIDs[0]; j < pContour->nContourSize; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
						for( j = 0; j <= nIDs[1]; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					}
					else
					{
/*						if( bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}

*/
						for( int j = nIDs[0]; j <= nIDs[1]; j++ )
							Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					}
				}
				else
				{
/*					if( !bIn )
						{
							CString Str;
							Str.Format( "%d, %d, %d, %d", bClockwise, bDirection, nIDs[0], nIDs[1]  );
							AfxMessageBox( Str );
						}
*/

					for( int j = nIDs[0]; j >= 0; j-- )
						Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
					for( j = pContour->nContourSize - 1; j >= nIDs[1]; j-- )
						Points.push_back( CPoint( pContour->ppoints[j].x, pContour->ppoints[j].y ) );
				}
			}
		}
	}

	return bInner;
}

void CActionCutArea::_check_contour( std::vector<CPoint> &PPoints )
{
	int nSz = PPoints.size();

	std::vector<CPoint> NewPPoints;

	NewPPoints.push_back( PPoints[0] );

	for(int i = 1; i < nSz; i++ )
	{
		if( abs( PPoints[i - 1].x - PPoints[i].x ) > 1 || abs( PPoints[i - 1].y - PPoints[i].y ) > 1 )
			_fill_points( NewPPoints, PPoints[i - 1], PPoints[i] );
		else
			NewPPoints.push_back( PPoints[i] );
	}

	PPoints.clear();
	nSz = NewPPoints.size();
	for( i = 0; i < nSz; i++ )
		PPoints.push_back( NewPPoints[i] );
}

/*void CActionCutArea::_fill_contour( LPBYTE *pDestMask, int w, int h, LPPOINT Pts, int nSz, RECT &rcImage, BYTE Val, bool bIncludeBorder )
{
	int nX, nY;

	BYTE Border;
	LPBYTE *pTempMask;
	pTempMask = new LPBYTE[h];
	for(int j = 0; j < h; j++ )
		pTempMask[j] = new BYTE[w];

	if( Val + 1 <= 255 )
		Border = Val + 1;
	else
		Border = Val - 1;

	if( !Border )
		Border += 10;

	for(j = 0; j < h; j++ )
		for( int i = 0; i < w;i++ )
			pTempMask[j][i] = 0;


	for(j = 0; j < nSz; j++ )
	{
		int x = Pts[j].x - rcImage.left;
		int y = Pts[j].y - rcImage.top;

		pTempMask[y][x] = Border;
	}


	for(j = 0; j < nSz; j++ )
	{
		int x = Pts[j].x;
		int y = Pts[j].y;

		if( _point_in_polygon( Pts, nSz, x + 1, y, false ) )
		{
			nX = x + 1;
			nY = y;
			break;
		}
		if( _point_in_polygon( Pts, nSz, x - 1, y, false  ) )
		{
			nX = x - 1;
			nY = y;
			break;
		}
		if( _point_in_polygon( Pts, nSz, x , y - 1, false  ) )
		{
			nX = x;
			nY = y - 1;
			break;
		}
		if( _point_in_polygon( Pts, nSz, x , y + 1, false  ) )
		{
			nX = x;
			nY = y + 1;
			break;
		}
	}


	int x = nX - rcImage.left;
	int y = nY - rcImage.top;

/*
	FILE *fl = fopen( "c:\\1.txt", "wt" );

	for(j = 0; j < h; j++ )
	{
		for( int i = 0; i < w;i++ )
		{
			if( i == x && j == y )
				fprintf( fl, "#" );
			else
				fprintf( fl, "%c", ( pTempMask[j][i] == Border ) ? 'x' : '.' );
		}
		fprintf( fl, "\n" );
	}

	fclose( fl );

	_line_fill( pTempMask, w, h, x, y, 1,  x, x, Border, Val );

	if( !bIncludeBorder )
	{
		for(j = 0; j < nSz; j++ )
		{
			int x = Pts[j].x - rcImage.left;
			int y = Pts[j].y - rcImage.top;

			pTempMask[y][x] = 0;
		}
	}
	else
	{
		for(j = 0; j < nSz; j++ )
		{
			int x = Pts[j].x - rcImage.left;
			int y = Pts[j].y - rcImage.top;

			pTempMask[y][x] = Val;
		}
	}

	for(j = 0; j < h; j++ )
		for( int i = 0; i < w;i++ )
		{
			if( pTempMask[j][i] )
				pDestMask[j][i] = pTempMask[j][i];
		}




	for(j = 0; j < h; j++ )
		delete []pTempMask[j];
	delete []pTempMask;
}

int CActionCutArea::_line_fill( LPBYTE *pSrcMask, int w, int h, int x, int y, int dir, int PrevXl, int PrevXr, BYTE Color, BYTE Bk ) 
{
	int xl = x, xr = x, c;
	
	do c = pSrcMask[y][--xl];
	while( c != Bk && c != Color );

	do c = pSrcMask[y][++xr];
	while( c != Bk && c != Color );

	xl++; xr--;

	for( x = xl; x <= xr; x++ )
		pSrcMask[y][x] = Color;

	for( x = xl; x <= xr; x++ )
	{
		c = pSrcMask[y + dir][x];
		if( c != Bk && c != Color )
			x = _line_fill( pSrcMask, w, h, x, y + dir, dir, xl, xr, Bk, Color );
	}

	for( x = xl; x < PrevXl; x++ )
	{
		c = pSrcMask[y - dir][x];
		if( c != Bk && c != Color )
			x = _line_fill( pSrcMask, w, h, x, y - dir, -dir, xl, xr, Bk, Color );
	}

	for( x = PrevXr; x < xr; x++ )
	{
		c = pSrcMask[y - dir][x];
		if( c != Bk && c != Color )
			x = _line_fill( pSrcMask, w, h, x, y - dir, -dir, xl, xr, Bk, Color );
	}

	return xr;
}
*/
BOOL CActionCutArea::_point_in_polygon(LPPOINT Points, int Count, int x, int y, bool bIncludeBorder ) 
{ 
    int Counter = 0; 
    for ( int i = 0; i < Count; i++ ) 
    { 
        int next = ( i + 1 ) % Count; 
        
		int x1 = Points[next].x; 
        int y1 = Points[next].y; 
        int x2 = Points[i].x; 
        int y2 = Points[i].y; 

        if( ( y1 < y ) && ( y2 < y ) ) 
			continue; 

        if( x1 == x ) 
		{
            if( y1 == y ) 
				return bIncludeBorder; 
            else if( x2 == x ) 
			{
                if( ( y1 <= y ) || ( y2 <= y ) ) 
					return bIncludeBorder; 
                else 
					continue; 
			}
		}

        if( ( ( x2 <= x ) && ( x1 <= x ) ) || ( ( x2 > x ) && ( x1 > x ) ) )
			continue; 

        if( ( y1 > y ) && ( y2 > y ) )
			Counter++; 
        else 
		{ 
            int dx = x2 - x1; 
            int dy = y2 - y1; 

			if( !dx || !dy )
				return false;

            double p = y1 + (x - x1) / double( dx * dy ); 
            if( p >  y)
				Counter++; 
            else if (p == y) 
				return bIncludeBorder; 
        } 
    } 
    
	if (Counter % 2 == 0) 
		return false; 
    else 
		return true; 

	return false; 
}

bool CActionCutArea::_run()
{
	if( !IsAvaible() )
		return false;

	CWaitCursor cursor;

	//get the target document
	IUnknown	*punkDoc = 0;
	sptrIView	sptrV = m_punkTarget;
	sptrV->GetDocument( &punkDoc );
	sptrIDocument	sptrD = punkDoc;

	if( !punkDoc )
		return false;

	punkDoc->Release();


	bool bCutted = cut_area(sptrD);

	m_lTargetKey = ::GetObjectKey( sptrD );

	RECT rc;
	
	int nSz = m_Points.size();

	if( nSz )
	{
		rc.left = rc.right = m_Points[0].x;
		rc.top = rc.bottom = m_Points[0].y;
	}

	for( int i = 0; i < nSz; i++ )
	{
		if( m_Points[i].x < rc.left )
			rc.left = m_Points[i].x;

		if( m_Points[i].x > rc.right )
			rc.right = m_Points[i].x;

		if( m_Points[i].y < rc.top )
			rc.top = m_Points[i].y;

		if( m_Points[i].y > rc.bottom )
			rc.bottom = m_Points[i].y;
	}

	Invalidate( CPoint( rc.left, rc.top ), CPoint( rc.right, rc.bottom ) );

	m_Points.clear();
	m_DispPoints.clear();
	m_MinInter.clear();
	m_MaxInter.clear();


	return bCutted;
}

BOOL CActionCutArea::_is_contour_correct(LPBYTE *pDestMask, int w, int h, Contour *pContour, int sx, int sy )
{
	return true;


	for( int i = 0; i < pContour->nContourSize; i++ )
	{
		int x = pContour->ppoints[i].x - sx,
			y = pContour->ppoints[i].y - sy;

		if(  x >= 0 && x < w && y >= 0 && y < h )
		{
			if( !pDestMask[y][x] )
				return false;
		}
	}

	return true;
}

bool CActionCutArea::DoRButtonDown( CPoint pt ) 
{ 
	_activate_object( pt );
	return false;
}

void CActionCutArea::_activate_object( CPoint pt )
{
	IUnknown *punkDocList = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	bool bFounded = false;

	if( !punkDocList )
		return;

	INamedDataObject2Ptr	ptrListObject = punkDocList;
	punkDocList->Release();


	TPOS posStart = 0, lPrevPos = 0;
	
	ptrListObject->GetFirstChildPosition((POSITION*)&posStart);

	while( posStart )
	{
		IUnknown *punkObject = 0;
		lPrevPos = posStart;
		ptrListObject->GetNextChild((POSITION*)&posStart, &punkObject);

		if( punkObject )
		{
			IMeasureObjectPtr pObject = punkObject;
			punkObject->Release();

			if( pObject == 0 )
				continue;
		
			IUnknown *punkImage = 0;
			pObject->GetImage( &punkImage );

			if( !punkImage )
				continue;

			IImage3Ptr ptrImage = punkImage;
			punkImage->Release();

			RECT rcImage;
			_get_img_rect( punkImage , rcImage );

			if( ::PtInRect( &rcImage, pt ) )
			{
				BYTE *pByte = 0;
				ptrImage->GetRowMask( pt.y - rcImage.top, &pByte );

				if( pByte[pt.x - rcImage.left] )
				{
					ptrListObject->SetActiveChild( lPrevPos );
					return;
				}
			}
		}
	}
}

bool CActionCutArea::_is_virtual( IUnknown *punkImage )
{
	IImage3Ptr	ptrI = punkImage;
	bool bIsVirtual = 0;
	
	if( ptrI )
	{
		DWORD dwFlags = 0;
		ptrI->GetImageFlags( &dwFlags );
		bIsVirtual = dwFlags & ifVirtual;
	}

	return bIsVirtual;
}

long CActionCutArea::_get_square( IUnknown *punkImage )
{
	long lVal = 0;

	if( !punkImage )
		return lVal;

	IImage3Ptr ptrImage = punkImage;

	if( ptrImage == 0 )
		return lVal;

	int w = 0, h = 0;

	ptrImage->GetSize( &w, &h );

	for( int j = 0;j < h;j++ )
	{
		LPBYTE pDestMask = 0;
		ptrImage->GetRowMask( j, &pDestMask );

		if( pDestMask )
		{
			for( int i = 0;i < w;i++ )
			{
				if( pDestMask[i] == 0xFF )
					lVal++;
			}
		}
	}

//	lVal = sqrt( w * h ) ;
	return lVal;
}

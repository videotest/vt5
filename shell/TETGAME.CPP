#include "stdlib.h"
#include "stdafx.h"
#include <time.h>
#include "string.h"

#include "tetgame.h"

#define SIZE_TETRIS_ADD	0
 
DWORD dwLevel[10] = {600, 500, 400, 350, 300, 200, 150, 100, 70, 50};
int	iScore[6]={1, 4, 100, 1, 2, 50};
int	iLevel = 0;

FigImage theFigImages[7] =
{
	{
		{
			1, 0, 0, 0,
			1, 0, 0, 0,
			1, 0, 0, 0,
			1, 0, 0, 0
		},
		1, 0, 1, 0, TET_RED
	
	},
	{
		{
			0, 0, 0, 1,
			0, 0, 1, 1,
			0, 0, 1, 0,
			0, 0, 0, 0
		},
		1, 2, 0, 1, TET_CYAN
	
	},
	{
		{
			0, 0, 1, 0,
			0, 0, 1, 1,
			0, 0, 0, 1,
			0, 0, 0, 0
		},
		1, 2, 0, 1, TET_MAGETTA
	
	},
	{
		{
			0, 0, 1, 0,
			0, 0, 1, 0,
			0, 0, 1, 1,
			0, 0, 0, 0
		},
		1, 2, 0, 1, TET_GREEN
	
	},
	{
		{
			0, 0, 1, 1,
			0, 0, 1, 0,
			0, 0, 1, 0,
			0, 0, 0, 0
		},
		1, 2, 0, 1, TET_YELLOW
	
	},
	{
		{
			0, 0, 1, 0,
			0, 0, 1, 1,
			0, 0, 1, 0,
			0, 0, 0, 0
		},
		1, 2, 0, 0, TET_BROWN
	
	},
	{
		{
			0, 0, 1, 1,
			0, 0, 1, 1,
			0, 0, 0, 0,
			0, 0, 0, 0
		},
		0, 2, 1, 1, TET_BLUE
	}
};

#ifndef NORECT
#define NORECT	CRect( 0, 0, 0, 0 )
#endif //NORECT




CTetGame::CTetGame(CWnd *pWnd)
{
	m_pwnd = pWnd;

	srand( (unsigned)time( NULL ) );
	m_pFigure = 0;
	m_rc = NORECT;
	m_bPreview = FALSE;
	SetSize(10, 20);
	m_placeNext = TET_NOFIGURE;
	EmptyArray();
}

CTetGame::~CTetGame()
{
	if(m_pFigure)delete m_pFigure;
}


TET_PLACE CTetGame::GetNext()
{
	TET_PLACE place = m_placeNext;
	m_placeNext = TET_PLACE(int((double)rand()/RAND_MAX*7.0+1));
	return place;
}
void CTetGame::OnTimer()
{
	DWORD	dwCurrentTick = GetTickCount();
	if( m_dwMoveTickCount && dwCurrentTick >= m_dwMoveTickCount  )
	{
		m_dwMoveTickCount = dwCurrentTick + dwLevel[m_iLevel];
		MoveDown();
	}
}

void CTetGame::NewGame()
{
	if(m_pFigure)delete m_pFigure;
	EmptyArray();
	GetNext();
	m_pFigure = new CFigure(GetNext(), this);
	m_pFigure->CanMove();//check

	m_pwnd->Invalidate();
	UpdateValue();

	m_dwMoveTickCount = GetTickCount() + dwLevel[m_iLevel];
}


void CTetGame::SetRect(RECT rc)
{
	m_rcBorder=rc;
	m_rc = CRect(rc.left+SIZE_TETRIS_ADD, rc.top+SIZE_TETRIS_ADD, rc.right-SIZE_TETRIS_ADD, rc.bottom-SIZE_TETRIS_ADD);
	m_iDX = (m_rc.right-m_rc.left)/m_iSizeX;
	m_iDY = (m_rc.bottom-m_rc.top)/m_iSizeY;
	m_rc.right = m_rc.left+m_iDX*m_iSizeX;
	m_rc.bottom = m_rc.top+m_iDY*m_iSizeY;
	
}

void CTetGame::OnDraw(CDC *pDC)
{
	if(m_rc == NORECT)
		return;
	
	CRect	rc = m_rc;
	InflateRect( rc, 1, 1 );
	pDC->DrawEdge( rc, BDR_SUNKENOUTER|BDR_SUNKENINNER, BF_RECT );
	DoDraw(pDC);
}

void CTetGame::DoDraw(CDC *pDC)
{
	for(int i = 0; i< m_iSizeX; i++)
		for(int j = 0; j< m_iSizeY; j++)	
			DrawItemRect(pDC, i, j, m_placeArray[i][j]);
			
	if(m_pFigure)m_pFigure->Draw(pDC);
}

void CTetGame::DrawItemRect(CDC *pDC, int i, int j, TET_PLACE place, int xx, int yy)
{
	if(j<0)return;
	
	if(xx == -1 || yy == -1)
	{
		xx = m_rc.left;
		yy = m_rc.top;
	}
	
	CRect	rc = CRect(m_iDX*i+xx, m_iDY*j+yy,  m_iDX*(i+1)+xx, m_iDY*(j+1)+yy);
	COLORREF	vcNo;
	

	
	switch(place)
	{
		case TET_NOFIGURE:
		{
			pDC->FillRect( rc, &CBrush(RGB(0, 0, 0)) );
			return;
		}
		case TET_GREEN:
		{	
			vcNo = RGB(128, 128, 255 );
			break;
		}
		case TET_YELLOW:
		{	
			vcNo = RGB(0, 255, 255 );
			break;
		}
		case TET_CYAN:
		{	
			vcNo = RGB(255, 128, 255 );
			break;
		}
		case TET_MAGETTA:
		{	
			vcNo = RGB(255, 255, 128 );
			break;
		}
		case TET_RED:
		{	
			vcNo = RGB(255, 128, 128 );
			break;
		}
		case TET_BLUE:
		{	
			vcNo = RGB(128, 255, 255 );
			break;
		}
		case TET_BROWN:
		{	
			vcNo = RGB(255, 0, 255 );
			break;
		}
	}

	pDC->FillRect( rc, &CBrush( vcNo ) );
	pDC->DrawEdge( rc, BDR_RAISEDINNER, BF_RECT );
}	

void CTetGame::DelLines()
{
	int	xs = m_rc.left;
	int	xe = m_rc.right;
	CTime time;

	for(int i = 0; i< m_iSizeY; i++)
	{
		if(m_pDelLines[i])
		{
			m_iLines++;
			m_iScore+=iScore[m_bPreview?2:5];
		}
	}
}
					


void CTetGame::SetSize(int cx, int cy)
{
	m_iSizeX = cx;
	m_iSizeY = cy;
}

void CTetGame::EmptyArray()
{
	for(int i = 0; i<m_iSizeX; i++)
		for(int j = 0; j<m_iSizeY; j++)
			m_placeArray[i][j] = TET_NOFIGURE;
	m_iLines = 0;
	m_iLevel = iLevel;
	m_iScore = 0;
	UpdateValue();
}

void CTetGame::MoveLeft()
{
	if(m_pFigure)
		m_pFigure->MoveLeft();
}

void CTetGame::MoveRight()
{
	if(m_pFigure)
		m_pFigure->MoveRight();
}



void CTetGame::Rotate()
{
	if(m_pFigure)
		m_pFigure->Rotate();
}

void CTetGame::MoveDown()
{
	if(m_pFigure)
	{
		BOOL	b = m_pFigure->MoveDown();
		m_pwnd->Invalidate();

		if(!b)
		{
			BOOL bDraw = m_pFigure->Map();
			delete m_pFigure;
			m_pFigure = 0;
			
			if(bDraw)
			{
				DelLines();
				
				if(m_iLines >=10 && m_iLevel <1)ChangeLevel(1);
				if(m_iLines >=20 && m_iLevel <2)ChangeLevel(2);				
				if(m_iLines >=30 && m_iLevel <3)ChangeLevel(3);
				if(m_iLines >=40 && m_iLevel <4)ChangeLevel(4);
				if(m_iLines >=50 && m_iLevel <5)ChangeLevel(5);
				if(m_iLines >=60 && m_iLevel <6)ChangeLevel(6);
				if(m_iLines >=70 && m_iLevel <7)ChangeLevel(7);
				if(m_iLines >=80 && m_iLevel <8)ChangeLevel(8);
				
				m_dwMoveTickCount = GetTickCount()+dwLevel[m_iLevel];
			}
			UpdateValue();

			m_pFigure = new CFigure(GetNext(), this);
			if(!m_pFigure->CanMove())
			{
				Finish();
			}
		}
		m_pwnd->UpdateWindow();
	}
	m_iScore+=iScore[m_bPreview?0:3];
}


void CTetGame::Finish()
{
	m_dwMoveTickCount = 0;
	
	/*Table tbl;
	tbl.strPlayerName[0]=0;
	tbl.iScore = m_iScore;
	tbl.iLines = m_iLines;
	tbl.iLevel = m_iLevel;*/

	delete m_pFigure;
	m_pFigure = 0;
	
	::AfxMessageBox("End of game detected");
	
	m_pwnd->GetParent()->SendMessage( WM_TET_SCOPES, 1 );

	m_placeNext = TET_NOFIGURE;
	m_iScore = 0;
	m_iLines = 0;
	m_iLevel = iLevel;
	UpdateValue();
}

void CTetGame::ChangeLevel(int iLev)
{
	m_iLevel = iLev;
	
	if(m_iLines>=iLev*10)
		m_dwMoveTickCount = GetTickCount()+dwLevel[iLev];
	else
	{
		m_iLines = 0;
		m_iScore = 0;
		UpdateValue();
	}
}

void CTetGame::UpdateValue()
{
	if( m_pwnd->GetSafeHwnd() )
		m_pwnd->GetParent()->SendMessage( WM_USER+100 );
	/*if(m_pwnd->m_pFigure)
	{
		delete m_pwnd->m_pFigure;
		m_pwnd->m_pFigure=0;
	}
	
	if(m_placeNext != TET_NOFIGURE && m_bPreview)
		m_pwnd->m_pFigure = new CFigure(m_placeNext, this);
	m_pwnd->Update();*/
}


void CTetGame::Drop()
{
	if(m_pFigure)
	{
		while(m_pFigure->MoveDown())m_iScore+=iScore[m_bPreview?4:1];
		MoveDown();
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
//CFigure


CFigure::CFigure(TET_PLACE place, CTetGame *pGame)
{
	m_iX = pGame->m_iSizeX/2;
	m_iY = 0;
	m_pTetGame = pGame;
	m_placeType = place;
	CreateImage();
}

CFigure::~CFigure()
{
}

void CFigure::Draw(CDC *pDC)
{
	
	for(int j = 0; j< 4; j++)
		for(int i = 0; i< 4; i++)
		{
			if(m_figImage.bImage[i][j])	
				m_pTetGame->DrawItemRect(pDC, m_iX-m_figImage.iCX+i, m_iY-m_figImage.iCY+j, m_placeType);
		}
}

void CFigure::Draw(CDC *pDC, int x, int y)
{
	for(int j = 0; j< 4; j++)
		for(int i = 0; i< 4; i++)
		{
			if(m_figImage.bImage[i][j])	
				m_pTetGame->DrawItemRect(pDC, -m_figImage.iCX+i, -m_figImage.iCY+j, m_placeType, x, y);
		}
}

void CFigure::DrawMask(FigImage *pFig, int iX, int iY)
{
/*	int	x, y, i, j;
	
	for(i = 0; i<4; i++)
		for(j = 0; j<4; j++)
		{	
			if(!m_figImage.bImage[i][j])continue;
			x = i-iX+pFig->iCX+m_iX-m_figImage.iCX;
			y = j-iY+pFig->iCY+m_iY-m_figImage.iCY;
			if(x>=0&&y>=0&&x<4&&y<4)
				if(pFig->bImage[x][y])continue;
			m_pTetGame->DrawItemRect(&dc, m_iX-m_figImage.iCX+i, m_iY-m_figImage.iCY+j, m_placeType);
		}
	for(i = 0; i<4; i++)
		for(j = 0; j<4; j++)
		{	
			if(!pFig->bImage[i][j])continue;
			x = i+iX-pFig->iCX-m_iX+m_figImage.iCX;
			y = j+iY-pFig->iCY-m_iY+m_figImage.iCY;
			if(x>=0&&y>=0&&x<4&&y<4)
				if(m_figImage.bImage[x][y])continue;
			m_pTetGame->DrawItemRect(&dc, iX-pFig->iCX+i, iY-pFig->iCY+j, TET_NOFIGURE);
		}*/

	m_pTetGame->GetWindow()->Invalidate();
	m_pTetGame->GetWindow()->UpdateWindow();
}

void CFigure::MoveLeft()
{
	FigImage	theOldFig;
	int	ix, iy;
	
	::memcpy(&theOldFig, &m_figImage, sizeof(FigImage));
	ix = m_iX;
	iy = m_iY;
	
	m_iX--;
	
	if(CanMove())
	{
		DrawMask(&theOldFig, ix, iy);
	}
	else
	{
		::memcpy(&m_figImage, &theOldFig, sizeof(FigImage));
		m_iX = ix;
		m_iY = iy;
	}
}

void CFigure::MoveRight()
{
	FigImage	theOldFig;
	int	ix, iy;
	
	::memcpy(&theOldFig, &m_figImage, sizeof(FigImage));
	ix = m_iX;
	iy = m_iY;
	
	m_iX++;
	
	if(CanMove())
	{
		DrawMask(&theOldFig, ix, iy);
	}
	else
	{
		::memcpy(&m_figImage, &theOldFig, sizeof(FigImage));
		m_iX = ix;
		m_iY = iy;
	}
}

void CFigure::Rotate()
{
	
	FigImage	theOldFig;
	int	ix, iy;
	
	::memcpy(&theOldFig, &m_figImage, sizeof(FigImage));
	ix = m_iX;
	iy = m_iY;
	
	for(int i = 0; i<4; i++)
		for(int j = 0; j<4; j++)
		      m_figImage.bImage[i][j] = theOldFig.bImage[3-j][i];

	m_figImage.iCX = theOldFig.iCY;
	m_figImage.iCY = 3-theOldFig.iCX-theOldFig.bCX;
	m_figImage.bCX = theOldFig.bCY;
	m_figImage.bCY = theOldFig.bCX;
	
	if(CanMove())
	{
		DrawMask(&theOldFig, ix, iy);
	}
	else
	{
		::memcpy(&m_figImage, &theOldFig, sizeof(FigImage));
		m_iX = ix;
		m_iY = iy;
	}

}

BOOL CFigure::MoveDown()
{
	FigImage	theOldFig;
	BOOL	b;
	int	ix, iy;
	
	::memcpy(&theOldFig, &m_figImage, sizeof(FigImage));
	ix = m_iX;
	iy = m_iY;
	
	m_iY++;
	
	b = CanMove();
	
	if(b)
	{
		DrawMask(&theOldFig, ix, iy);
	}
	else
	{
		::memcpy(&m_figImage, &theOldFig, sizeof(FigImage));
		m_iX = ix;
		m_iY = iy;
	}
	
	return b;
}


void CFigure::CreateImage()
{
	bool	bFound = false;
	
	for(int i = 0; i<7; i++)
	{
		if(theFigImages[i].place == m_placeType)
		{
			bFound = true;
			break;
		}
	}
	ASSERT(bFound);
	::memcpy(&m_figImage, &theFigImages[i], sizeof(FigImage));
	CanMove();
}

BOOL CFigure::CanMove()
{
	BOOL	bRet = TRUE;
	int	x, y;
	BOOL	bFound = false;
	
	for(int i = 0; i<4; i++)
	{
		for(int j = 0; j<4; j++)
		{
			if(!m_figImage.bImage[i][j])continue;
			bFound = true;
			x = m_iX - m_figImage.iCX + i;
			y = m_iY - m_figImage.iCY + j;
			
			if(y<0)continue;
			if(x<0||x>=m_pTetGame->m_iSizeX || y>=m_pTetGame->m_iSizeY)	
				return FALSE;

			if( y < 0 )
				continue;
			if(m_pTetGame->m_placeArray[x][y])return FALSE;
		}
	}

	ASSERT(bFound);
	return TRUE;
}

BOOL CFigure::Map()
{
	int	x, y, i, j;
	
	for(i = 0; i<4; i++)
	{
		for(j = 0; j<4; j++)
		{
			if(!m_figImage.bImage[i][j])continue;
			x = m_iX - m_figImage.iCX + i;
			y = m_iY - m_figImage.iCY + j;
			m_pTetGame->m_placeArray[x][y] =m_placeType;
		}
	}
	
	BOOL bLineSet;
	y = m_pTetGame->m_iSizeY-1;
	
	for(j = m_pTetGame->m_iSizeY-1; j>=0; j--) 
	{
		 bLineSet = TRUE;
		 
		 for(i = 0; i< m_pTetGame->m_iSizeX; i++)
		 	if(!m_pTetGame->m_placeArray[i][j])
		 		bLineSet=FALSE;
		 		
		 if(bLineSet)
		 {
		 	m_pTetGame->m_pDelLines[j] = TRUE;
		 	continue;
		 }
		 m_pTetGame->m_pDelLines[j] = FALSE;
		 for(i = 0; i< m_pTetGame->m_iSizeX; i++)
		 	m_pTetGame->m_placeArray[i][y]=m_pTetGame->m_placeArray[i][j];
		 y--;
	}
	
	BOOL bRet = y != j;
	
	for(j = 0; j<y;j++)
		for(i = 0; i< m_pTetGame->m_iSizeX; i++)
			 	m_pTetGame->m_placeArray[i][j]=TET_NOFIGURE;
	
	return bRet;
}
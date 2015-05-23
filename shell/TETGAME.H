#ifndef __TETGAME_H__
#define __TETGAME_H__

#define WM_TET_SCOPES	(WM_USER+100)

enum TET_PLACE {TET_NOFIGURE=0, 
			TET_GREEN, TET_YELLOW, 
			TET_CYAN, TET_MAGETTA, 
			TET_RED, TET_BLUE, TET_BROWN};

struct FigImage
{
	BOOL	bImage[4][4];
	int		iCX, iCY;
	BOOL	bCX, bCY;
	TET_PLACE	place;
};


class CTime;
class CWnd;


class CTetGame
{
friend class CFigure;
public:
	BOOL	m_bPreview;
public:
	CTetGame(CWnd *);
	~CTetGame();
public:
	void SetRect(RECT rc);
	void OnDraw(CDC *);
	void DoDraw(CDC *);
	void DrawItemRect(CDC *, int, int, TET_PLACE, int = -1, int = -1);
	void DelLines();
	void NewGame();
	void ChangeLevel(int iLev);
	void UpdateValue();
	void Finish();
	TET_PLACE GetNext();
	
	void OnTimer();
public:
	void SetSize(int, int);	
	void EmptyArray();
	
	void MoveLeft();
	void MoveRight();
	void Rotate();
	void MoveDown();
	void Drop();

	CWnd	*GetWindow()	const {return m_pwnd;}
	int	GetLines()			const {return m_iLines;}
	int	GetLevel()			const {return m_iLevel;}
	int	GetScope()			const {return m_iScore;}
protected:
	DWORD	m_dwMoveTickCount;
	CWnd	*m_pwnd;
	CFigure	*m_pFigure;
	CRect	m_rc, m_rcBorder;
	int		m_iSizeX, m_iSizeY;
	int		m_iDX, m_iDY;
	int		m_iLines;
	int		m_iScore;
	int		m_iLevel;
	TET_PLACE	m_placeNext;
	TET_PLACE	m_placeArray[20][20];
	BOOL		m_pDelLines[20];	
};

class CFigure
{
protected:
	FigImage	m_figImage;
    CTetGame	*m_pTetGame;
	int	m_iX, m_iY; 
	TET_PLACE	m_placeType;
public:
	CFigure(TET_PLACE, CTetGame *);
	~CFigure();
public:	
    void Draw(CDC *);
    void Draw(CDC *, int x, int y);
    void DrawMask(FigImage*, int, int);
    void CreateImage();
    BOOL CanMove();
    BOOL Map();
public:
	void MoveLeft();
	void MoveRight();
	void Rotate();
	BOOL MoveDown();
};

#endif //__TETGAME_H__

#ifndef __comments_h__
#define __comments_h__


class CCommentsObjectList : public CDataObjectListBase,
							public CDrawObjectImpl
{
	PROVIDE_GUID_INFO()
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CCommentsObjectList);
	GUARD_DECLARE_OLECREATE(CCommentsObjectList);
	DECLARE_INTERFACE_MAP();

	CCommentsObjectList(){};
	~CCommentsObjectList()
	{
		
	};
	virtual void DoDraw( CDC &dc );
public:
	static const char *c_szType;
};



class CCommentsObject : public  CDataObjectBase,
						public  CDrawObjectImpl
{
	PROVIDE_GUID_INFO()
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CCommentsObject);
	GUARD_DECLARE_OLECREATE(CCommentsObject);
public:
	CCommentsObject();
	~CCommentsObject();

	static const char *c_szType;

	BEGIN_INTERFACE_PART(CommObj, ICommentObj)
		com_call GetText(BSTR *pbstrText);
		com_call SetText(BSTR bstrText);
		com_call GetCoordinates(RECT* prcText, POINT* pPtArrow, BOOL bFull = 0 );
		com_call SetCoordinates(RECT rcText, POINT ptArrow);
		com_call SetEditMode(BOOL bEdit);
		com_call LockDraw(BOOL bLock);
		com_call UpdateObject();
	END_INTERFACE_PART(CommObj)

	BEGIN_INTERFACE_PART(FontPage, IFontPropPage)
		com_call GetFont(LOGFONT*	pLogFont, int* pnAlign);
		com_call GetColorsAndStuff(OLE_COLOR* pcolorText, OLE_COLOR* pcolorBack, BOOL* pbBack, BOOL* pbArrow);
		com_call SetFont(LOGFONT*	pLogFont, int nAlign);
		com_call SetColorsAndStuff(OLE_COLOR colorText, OLE_COLOR colorBack, BOOL bBack, BOOL bArrow);
	END_INTERFACE_PART(FontPage)

	BEGIN_INTERFACE_PART(Rect, IRectObject)
		com_call SetRect(RECT rc);
		com_call GetRect(RECT* prc);
		com_call HitTest( POINT	point, long *plHitTest );
		com_call Move( POINT point );
	END_INTERFACE_PART(Rect)
//ovverrided virtuals
	virtual DWORD GetNamedObjectFlags();
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );
	virtual void DoDraw( CDC &dc );
	virtual CRect DoGetRect();
	void UpdateObject();
	void _SetRectByText();

	DECLARE_INTERFACE_MAP();
protected:
	CString			m_strText;
	CRect			m_rcText;
	CPoint			m_ptArrow;
	long			m_nID;
	//byte			m_pColors[3];
	//byte			m_pBackColors[3];
	COLORREF		m_color;
	COLORREF		m_backColor;
	BOOL			m_bEditMode;
	bool			m_bTransparent;
	CFont			m_fontText;
	CRect			m_rcLastDraw;
	CSize			m_sizeTextWindow;


	//font support
	int				m_nHeight;
	bool			m_bBold;
	bool			m_bUnderline;
	bool			m_bItalic;
	byte			m_nFamily;
	CString			m_strName;
	bool			m_bDrawArrow;
	UINT			m_nAlign;

	LOGFONT			m_logFont;

	BOOL			m_bLockDraw;
};

#endif //__comments_h__
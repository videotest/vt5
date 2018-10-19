// BaseColumn.h: interface for the CBaseColumn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASECOLUMN_H__26459A24_D454_11D3_80E4_0000E8DF68C3__INCLUDED_)
#define AFX_BASECOLUMN_H__26459A24_D454_11D3_80E4_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define VT_LIST_SELECTION_ALIGN		1

#define VT_LIST_ICON_SIZE			16

#define VT_LIST_DW_VERSION_INFO		1

#define VT_BORDER_WIDTH				2





class CBaseColumn;
class CListBaseCtrl;

enum datatype_e{
	TYPE_SUBJECT, 
	TYPE_LOGICAL,
	TYPE_COLOR,
	TYPE_FONT,
	TYPE_VALUE,	
	TYPE_VALUE_COMBO,
	TYPE_SHADING,
	TYPE_NOT_SUPPORTED
};  


class CBaseCell : public CObject
{	    
	friend class CListBaseCtrl;
	DECLARE_SERIAL(CBaseCell)


	CImageList m_imageList;

public:
	CBaseCell();

	virtual ~CBaseCell();
	
	virtual void Serialize(CArchive& ar);

	void SetListBase( CListBaseCtrl* pListBase );
private:	
	CListBaseCtrl*	m_pListBase;
	bool			m_bImageListHere;
				 
	OLE_COLOR	m_ForeColor;
	OLE_COLOR	m_BackColor;
	
	//COLOR
	OLE_COLOR	m_Color;	
	
	//SUBJECT
	CString		m_strSubject;

	//VALUE
	CString		m_strValue;
	
	//BOOL
	BOOL		m_bValue;
	BOOL		m_showLogicalAsText;

	//FONT
	//CFont		m_Font;
	LOGFONT		m_logFont;
	CString		m_strFontString;

	//LOGFONT		m_logFont;

	//Text align
	DWORD		m_dwTextAllign;	

	//Icons list for cell	
	//Icon index in image list
	int			m_idxImage;

	//KeyValue for this cell
	CString		m_strKeyValue;

	CStringArray	m_strComboValue;

	friend CBaseColumn;

	//SHADING
	short m_shadingType;

public:
	UINT		m_nPointFontSize;
	COLORREF m_crFontColor;

	int GetComboSize() {
		return (int)m_strComboValue.GetSize();
	}

	CString GetComboValue( int nItem ) {
		return m_strComboValue[nItem];
	}
	void AddComboValue( CString str ) {
		m_strComboValue.Add( str );
	}

	BOOL RemoveComboValue(  int nItem  ) 
	{
		if( nItem < m_strComboValue.GetSize() )
		{
			m_strComboValue.RemoveAt( nItem );
			return 1;
		}
		return 0;
	}

	void ClearCombo() {
		m_strComboValue.RemoveAll();
	}


	CString GetKeyValue(){
		return m_strKeyValue;
	}

	void SetKeyValue(CString strKeyValue){
		m_strKeyValue = strKeyValue;
	}
	
	COLORREF GetForeColor(){
		COLORREF cr = RGB(0x00,0x00,0x00);
		::OleTranslateColor(m_ForeColor, NULL, &cr);
		return cr;
	}

	void SetForeColor(OLE_COLOR color){
		m_ForeColor = color;
	}

	COLORREF GetBackColor(){
		COLORREF cr = RGB(0x00,0x00,0x00);
		::OleTranslateColor(m_BackColor, NULL, &cr);
		return cr;
	}

	void SetBackColor(OLE_COLOR color){
		m_BackColor = color;
	}


	//For Value dataType
	CString GetAsValue(){
		return m_strValue;
	}

	void SetValue(CString strText){
		m_strValue = strText;
	}
	 
	//For Color dataType
	COLORREF GetAsColor(){
		COLORREF cr = RGB(0x00,0x00,0x00);
		::OleTranslateColor(m_Color, NULL, &cr);
		return cr;
	}

	void SetColor(COLORREF cr){
		m_Color = (OLE_COLOR)cr;
	}

	//For SUBJECT dataType
	CString GetAsSubject(){
		return m_strSubject;
	}

	void SetSubject(CString strText){
		m_strSubject = strText;
	}

	//For LOGICAL dataType
	BOOL GetAsLogical(){
		return m_bValue;
	}

	void SetLogical(BOOL bValue){
		m_bValue = bValue;
	}

	CString GeAsLogicalText(){
		return (m_bValue ? _T("TRUE") : _T("FALSE") );
	}

	BOOL IsShowLogicalAsText(){
		return m_showLogicalAsText;
	}

	void SetShowLogicalAsText(BOOL b_showAsText){
		m_showLogicalAsText = b_showAsText;
	}


	//For FONT dataType
	CString GetAsFontString(){
		return m_strFontString;
	}


	LOGFONT* GetAsFont(){
		return &m_logFont;
	}
				 
	void SetFont(LOGFONT &lf){
		memcpy( &m_logFont, &lf, sizeof(LOGFONT) );		
		m_logFont.lfCharSet = DEFAULT_CHARSET;
		m_strFontString = lf.lfFaceName;
	}

	void SetTextAlign(DWORD dw_align){
		m_dwTextAllign = dw_align;
	}

	//Set icon for this cell
	void SetIconFileName(CString sz_str);

	//For Row property page. Use in row list
	CString GetAsString( datatype_e datatype );

	//For SHADING dataType
	void SetShadingType(short iValue)
	{
		m_shadingType = iValue;
	}
	short GetShadingType()
	{
		return m_shadingType;
	}

	
};


class CBaseColumn : public CObject  
{
	DECLARE_SERIAL(CBaseColumn)

	CImageList m_imageList;

public:	
	CBaseColumn( );
	virtual ~CBaseColumn();
	virtual void Serialize(CArchive& ar);
	

	//Column Manipulation.

	
	virtual void AddColumn( int iCol, datatype_e datatype, CString strCaption, CListBaseCtrl* pListCtrl, int colWidth, DWORD dwCaptionAlign );	
	virtual void AddRow( int iCol, int iRowCount, CListBaseCtrl* pListCtrl);	
	virtual void DeleteRow( int iRow );

	virtual LRESULT DrawItem(int iRow, CDC* pDC, CRect rc, BOOL b_SelectedItem,
							BOOL b_SelectedRow,
							 CListBaseCtrl* pListCtrl);

	void DrawText(CListBaseCtrl* pListCtrl, CDC* pDC, CBaseCell* pCell, CString str, CRect rc, 
		BOOL b_neesFillFullRow, BOOL b_curSelectedItem, int iIconAlign = 0 , CRect* pCellRect = NULL);

	void DrawSelection(CRect rc, CDC* pDC, BOOL b_selectedItem);

	void SetListBase( CListBaseCtrl* pListBase );
protected:
	
	CListBaseCtrl*	m_pListBase;

	//Rows array for this column
	CTypedPtrArray <CObArray, CBaseCell*> m_columnValues;
	
	//Use icons whith text?
	BOOL	m_bUseImage;

	int m_idxTrue;
	int m_idxFalse;
	int m_idxGray;

	//Column header text
	CString m_strCaption;
	//Column header align
	DWORD	m_dwCaptionAllign;	
	
	//Column width
	int		m_nColWidth;

	//Type of column
	datatype_e m_datatype;	


	//Default font for column
	//CFont		m_def_Font;
	LOGFONT		m_def_logFont;
	BOOL		m_b_use_def_Font;

	//Default text align for column
	DWORD		m_def_dwTextAllign;	
	BOOL		m_b_use_def_TextAllign;

	//Default fore color for column
	OLE_COLOR	m_def_ForeColor;
	BOOL		m_b_use_def_ForeColor;

	//Default back color for column
	OLE_COLOR	m_def_BackColor;
	BOOL		m_b_use_def_BackColor;

	
	BOOL		m_def_showLogicalAsText;

	BOOL		m_bReadOnly;

public:

	//Add Column
	BOOL AddColumnToListCtrl( int iCol, CListBaseCtrl* pListCtrl );
	BOOL AddRowToListCtrl( int iCol, int iRow,  CListBaseCtrl* pListCtrl );


	CBaseCell* GetCell( int iRow){
		if ((iRow < m_columnValues.GetSize()) && (iRow >= 0))
			return m_columnValues[iRow];
		else
			return NULL;
	}


	datatype_e GetDataType( ){
		return m_datatype;
	}

	void SetDataType( datatype_e datatype ){
		m_datatype = datatype;
	}


	//Properties support
	OLE_COLOR GetColumnBackColor( ){
		return m_def_BackColor;
	}

	OLE_COLOR GetColumnForeColor( ){
		return m_def_ForeColor;
	}

	CString GetColumnCaption(){
		return m_strCaption;
	}

	short static ConvertFromEnumToShort( datatype_e dt );	
	datatype_e  static ConvertFromShortToEnum( short index );	

	short static ConvertFromDWORDToShort_Align( DWORD dw_align );	
	DWORD  static ConvertFromShortToDWORD_Align( short index );	

	short GetColumnType();	

	short GetColumnAlign();	

	void SetColumnAlign(short nAlign);
	void SetColumnBackColor(OLE_COLOR color);
	void SetColumnForeColor(OLE_COLOR color);
	void SetColumnType(short nType);
	void SetColumnWidth(short nWidth);
	void SetColumnCaption(CString strCaption);
	void SetColumnFont(LOGFONT& lf);
	

	BOOL GetDisplayLogicalAsText();
	void SetDisplayLogicalAsText( BOOL bValue );

	BOOL IsColumnUseIcon( );
	void SetColumnUseIcon( BOOL bValue );

	
	LPLOGFONT GetDefFont( );	
	void SetDefFont( LPLOGFONT lpLF );		
	

	int GetColumnWidth(){
		return m_nColWidth;
	}

	void SetColumnWidth(int nWidth){
		m_nColWidth = nWidth;
	}

	void SetIconFileNameForLogical(CString &sz_str, BOOL b_forValue);

	BOOL IsReadOnly(){
		return m_bReadOnly;
	}

	void SetReadOnly(BOOL bReadOnly){
		m_bReadOnly = bReadOnly;
	}

	friend class CListBaseCtrl;

};


#endif // !defined(AFX_BASECOLUMN_H__26459A24_D454_11D3_80E4_0000E8DF68C3__INCLUDED_)

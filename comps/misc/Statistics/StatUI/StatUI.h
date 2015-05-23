// StatUI.h : main header file for the StatUI DLL
//

#pragma once

#include "\vt5\awin\misc_map.h"

namespace ViewSpace 
{
	void ErrorMessage( HWND hwnd, UINT nID );

	namespace {
		const double M_1_SQRT_2_PI =  M_2_SQRTPI * 0.5 * M_SQRT1_2;// - sqrt(pi*2)
	}

	double Fnormal(double x, double a, double b );
	double FNormalMax(double sigma);
	double FLogNormal(double x, double M, double S );

	class CFontObj:public LOGFONT
	{
		HFONT m_hFont;
		int m_lfHeight;
	public:
		COLORREF m_clrText;
		CFontObj();
		CFontObj(LOGFONT& lf,COLORREF& m_clrText);
		virtual ~CFontObj();
		int SetHeight(int Height);
		int MeasureString(Gdiplus::Graphics& g, CStringW& text, LPRECT lpRect, UINT uFormat);
		int DrawString(Gdiplus::Graphics& g, const CStringW& text, LPRECT lpRect, UINT uFormat);
		int DrawString(Gdiplus::Graphics& g, const CStringW& text, LPRECT lpRect, UINT uFormat, COLORREF clrText);
		void Delete(void);
	};

	class CTitleTableWnd:public CWindowImpl<CTitleTableWnd>
	{
		BEGIN_MSG_MAP(CTitleTableWnd)
			MSG_WM_PAINT(OnPaint)
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		END_MSG_MAP()
		LRESULT OnPaint(HDC hdc);
	public:
		LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			return 1;
		}
		CString _strTitle;
		LOGFONT m_lfFontTitle;
		COLORREF m_clTextColorTitle;
	};

	template<class T>
	class matrix:public valarray<T>{
	public:
		matrix():_nCols(0){};
		matrix(size_t mRows, size_t _nCols):valarray<T>(mRows*_nCols),_nCols(_nCols){}
		matrix(T* arr,size_t mRows,size_t _nCols):valarray<T>(arr,mRows*_nCols),_nCols(_nCols){}
		T* operator[](size_t row) {
			_ASSERTE(row < size()/_nCols);
			return &(valarray<T>::operator[](_nCols*row));
		}
		const T* operator[](size_t row) const {
			_ASSERTE(row < size()/_nCols);
			return &(valarray<T>::operator[](_nCols*row));
		}
		~matrix(){};
		matrix& operator=(const T& val) {valarray<T>::operator=(val);return *this;}
		void resize(size_t m_Rows, size_t n_Cols) {
			_nCols=n_Cols;
			valarray<T>::resize(m_Rows*n_Cols);
		}
		//slice_array<T>& row(size_t aRow) {return this[slice(aRow*_nCols, _nCols, 1)]};
		//slice_array<T>& col(size_t aCol) {return this[slice(aCol, size()/_nCols, _nCols)]};
		int GetColsCount()const{return _nCols;}
		int GetRowsCount()const{return _nCols ? size()/_nCols : 0;}
	private:
		matrix(const matrix&):_nCols(0){};
		size_t _nCols;
	};

	typedef matrix<CString> StringMatrix;


	typedef _map_t< CString, long, cmp_long  > ROW_TO_VALUE;
	typedef _map_t< long, long, cmp_long  > ROW_TO_VALUE2;
	inline void _delete_map( ROW_TO_VALUE *pObject ) { delete pObject; }
	typedef _map_t< ROW_TO_VALUE *, long, cmp_long, _delete_map> COL_TO_ROW;
}

namespace{	
	static LOGFONT lfDefault={
		10,		//LONG lfHeight
		0,		//LONG lfWidth
		0,		//LONG lfEscapement
		0,		//LONG lfOrientation
		400,	//LONG lfWeight
		0,		//BYTE lfItalic
		0,		//BYTE lfUnderline
		0,		//BYTE lfStrikeOut
		DEFAULT_CHARSET,	//BYTE lfCharSet
		0,		//BYTE lfOutPrecision
		0,		//BYTE lfClipPrecision
		0,		//BYTE lfQuality
		FF_SWISS,	//BYTE lfPitchAndFamily
		_T("Arial")	//WCHAR lfFaceName[ 32 ]
	};
}

#include "stdafx.h"
#include "addidiobend.h"
#include "resource.h"
#include "carioint.h"
#include "math.h"
#include "idio_int.h"
#include "chromosome.h"

#include "carioimpl.h"

#define IDIC_DEFAULT IDIC_WHITE

void CActionIdioEditBase::_get_parts( CString str, CString &sA, CString &sB, CString &sC, CString &sD )
{
	if( str.IsEmpty() ) 
		return;

	int nPos = str.Find( ".", 0 );
	CString token;
	
	if( nPos != -1 )
		token = str.Left( nPos );
	else
		token = str;

	sA = token.Left(1);
	
	sB = CString( token ).Right( strlen( token ) - 1 );

	if( nPos >= 0 )
		token = str.Tokenize( ".", nPos );
	else
		token = "";

	sC = token;

	if( nPos >= 0 )
		token = str.Tokenize( ".", nPos );
	else
		token = "";

	sD = token;
}

CString CActionIdioEditBase::_set_parts( CString sA, CString sB, CString sC, CString sD )
{
	CString ret;

	ret = sA + sB;

	if( !sC.IsEmpty() )
		ret += "." + sC;

	if( !sD.IsEmpty() )
		ret += "." + sD;

	return ret;
}


void CActionIdioEditBase::_correct_numbers( IIdioBendsPtr ptrBends, CString strNew, CString strPrev, int nMain, bool bSkipTargetBend )
{
	long lSz = 0;
	ptrBends->GetBengsCount( &lSz );
	double fMin = 0, fMax = 0;

	bool bCentr = false;

	for( long i = 0; i <= nMain; i++ )
	{
		int nFlag = 0;
		ptrBends->GetBendFlag( i, &nFlag );

		if( nFlag == IDIB_CENTROMERE )
		{
			bCentr = true;
			break;
		}
	}


	CString sA, sB, sC, sD;

	_get_parts( strNew, sA, sB, sC, sD );

	bool bA = false, bB = false, bC = false, bD = false;
	int nA = 0, nB = 0, nC = 0, nD = 0;

	CString __sA, __sB, __sC, __sD;

	{
		_get_parts( strPrev, __sA, __sB, __sC, __sD );

		bA = bool( __sA != sA );
		bB = bool( __sB != sB );
		bC = bool( __sC != sC );
		bD = bool( __sD != sD );

		if( bA && !sA.IsEmpty() && !__sA.IsEmpty() )
		{
			if( isdigit_ru( sA[0] ) && isdigit_ru( __sA[0] ) )
				nA = sA[0] - __sA[0];
			else
			{
				if( !isdigit_ru( sA[0] ) && isdigit_ru( __sA[0] ) )
					nA = ( sA[0] - 'A' + 1 ) - atoi( __sA );
				else if( isdigit_ru( sA[0] ) && !isdigit_ru( __sA[0] ) )
					nA = atoi( sA ) - ( __sA[0] - 'A' + 1 );
				else if( !isdigit_ru( sA[0] ) && !isdigit_ru( __sA[0] ) )
					nA = ( sA[0] - 'A' + 1 ) - ( __sA[0] - 'A' + 1 );
			}
		}

		if( !nA )
			 bA = 0;

		if( bB && !sB.IsEmpty() && !__sB.IsEmpty() )
			nB = atoi( sB ) - atoi( __sB );

		if( !nB )
			 bB = 0;

		if( bC && !sC.IsEmpty() && !__sC.IsEmpty() )
			nC = atoi( sC ) - atoi( __sC );
		else if( bC && !sC.IsEmpty() && __sC.IsEmpty() )
			nC = atoi( sC );

		if( !nC )
			 bC = 0;

		if( bD && !sD.IsEmpty() && !__sD.IsEmpty() )
			nD = atoi( sD ) - atoi( __sD );
		else if( bD && !sD.IsEmpty() && __sD.IsEmpty() )
			nD = atoi( sD );

		if( !nD )
			 bD = 0;
	}

	long nStart = 0, nEnd = 0, nStep = 0;

	if( !bCentr )
	{
		nStart = nMain;

		if( bSkipTargetBend )
			nStart--;

		nStep = -1;
	}
	else
	{
		nStart = nMain;

		if( bSkipTargetBend )
			nStart++;

		nEnd = lSz;
		nStep = 1;
	}

	for( int i = nStart;; i += nStep )
	{
		if( !bCentr )
		{
			if( i < nEnd )
				break;
		}
		else
		{
			if( i >= nEnd )
				break;
		}

		_bstr_t bstr;
		ptrBends->GetBendNumber( i, bstr.GetAddress() );

		CString str = (char*)bstr;

		if( !str.IsEmpty() )
		{
			CString _sA, _sB, _sC, _sD, sNum;
			_get_parts( str, _sA, _sB, _sC, _sD );

			if( bA )
			{
				if( !isdigit_ru(  sA[0] ) )
					sNum = (char)( _sA[0] + nA );
				else
					sNum = sA;

			}
			else
			{
				if( isdigit_ru(  sA[0] ) )
					sNum = sA;
				else
				{
					if( !isdigit_ru( _sA[0] ) )
						sNum = _sA;
					else
					{
						sNum = (char)( 'A' + atoi( _sA ) - 1 );
					}
				}
			}

			if( !isdigit_ru( sNum[0] ) )
			{
				if( sNum[0] > 'D' )
					sNum = 'D';
			}
			else
			{
				if( sNum[0] > '4' )
					sNum = '4';
			}

			if( !isdigit_ru( sNum[0] ) )
			{
				if( sNum[0] < 'A' )
					sNum = 'A';
			}
			else
			{
				if( sNum[0] < '1' )
					sNum = '1';
			}

			if( bB && _sA == __sA )
			{
				if( nB )
				{
					char chs[10] = {0};

					if( atoi( _sB ) + nB >= 1 )
						_itoa( atoi( _sB ) + nB, chs, 10 );
					else
						_itoa( 1, chs, 10 );

					sNum += chs;
				}
			}
			else
				sNum += _sB;



			if( bC && _sA == __sA && _sB == __sB )
			{
				if( nC )
				{
					char chs[10] = {0};

					if( atoi( _sC ) + nC >= 1 )
						_itoa( atoi( _sC ) + nC, chs, 10 );
					else
						_itoa( 1, chs, 10 );

					sNum = sNum + "." + chs;
				}
			}
			else if( !_sC.IsEmpty() )
				sNum = sNum + "." + _sC;

			if( bD && _sA == __sA && _sB == __sB && _sC == __sC )
			{
				if( nD )
				{
					char chs[10] = {0};
					if( atoi( _sD ) + nD >= 1 )
						_itoa( atoi( _sD ) + nD, chs, 10 );
					else
						_itoa( 1, chs, 10 );

					sNum = sNum + "." + chs;
				}
			}
			else if( !_sD.IsEmpty() )
				sNum = sNum + "." + _sD;

			if( str != sNum )
				ptrBends->SetBendNumber( i, _bstr_t( sNum ) );
		}

	}
}

/***************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//
//CActionAddIdioBend2
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionAddIdioBend2, CCmdTargetEx);

// {151392DD-7816-4d75-9285-FB090F2273FE}
GUARD_IMPLEMENT_OLECREATE(CActionAddIdioBend2, "ChromosEx.AddIdioBend2", 
0x151392dd, 0x7816, 0x4d75, 0x92, 0x85, 0xfb, 0x9, 0xf, 0x22, 0x73, 0xfe);

// {CC61126C-7452-4900-B6CD-3464280B3EA3}
static const GUID guidAddIdioBend2 = 
{ 0xcc61126c, 0x7452, 0x4900, { 0xb6, 0xcd, 0x34, 0x64, 0x28, 0xb, 0x3e, 0xa3 } };

ACTION_INFO_FULL(CActionAddIdioBend2, IDS_ADD_BEND22, -1, -1, guidAddIdioBend2 );
ACTION_TARGET(CActionAddIdioBend2, szTargetViewSite);

ACTION_ARG_LIST(CActionAddIdioBend2)
	ARG_INT("ForAll", 0 )
END_ACTION_ARG_LIST()

CActionAddIdioBend2::CActionAddIdioBend2()
{

}

CActionAddIdioBend2::~CActionAddIdioBend2(void)
{
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend2::Invoke()
{
	{
		IUnknown	*punkDoc = 0;
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &punkDoc );
		sptrIDocument	sptrD( punkDoc );

		m_lTargetKey = ::GetObjectKey( sptrD );

		if( !punkDoc )
			return false;

		punkDoc->Release();
	}

	IUnknown *punk = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	
	if( !punk )
		return false;

	INamedDataObject2Ptr sptrList = punk;
	punk->Release();

	if( sptrList == 0 )
		return false;

	POSITION lPos = 0;

	bool bSingle = !( GetArgumentInt( "ForAll" ) != 0 );

	if( !bSingle )
		sptrList->GetFirstChildPosition( &lPos );
	else
		sptrList->GetActiveChild( &lPos );

	if( !lPos )
		return false;

	while( lPos )
	{
		IUnknown *punkObject = 0;
		sptrList->GetNextChild( &lPos, &punkObject );

		if( !punkObject && !bSingle )
			continue;
		else if( !punkObject && bSingle )
			return false;

		IMeasureObjectPtr sptrMeasure = punkObject;
		punkObject->Release();

		if( sptrMeasure == 0 && !bSingle )
			continue;
		else if( sptrMeasure == 0 && bSingle )
			return false;

		INamedDataObject2Ptr ptrNDO = sptrMeasure;
		
		if( ptrNDO == 0 && !bSingle )
			continue;
		else if( ptrNDO == 0 && bSingle  )
			return false;

		POSITION lPosCromo = 0;
		
		IIdiogramPtr sptrEditable;
		IChromosomePtr sptrChromo;

		ptrNDO->GetFirstChildPosition( &lPosCromo );
		while( lPosCromo )
		{
			IUnknown* punkChromo = 0;
			ptrNDO->GetNextChild( &lPosCromo, &punkChromo );
			if( !punkChromo && !bSingle )
				continue;
			else if( !punkChromo && bSingle )
				return false;


			IIdiogramPtr ptr_idio = punkChromo;

			if( ptr_idio == 0 )
				sptrChromo = punkChromo;

			punkChromo->Release(); punkChromo = 0;

			if( ptr_idio != 0 && sptrEditable == 0 )
			{
				unsigned uFlag = -1;
				ptr_idio->get_flags( &uFlag );

				if( uFlag == IDIT_EDITABLE )
					sptrEditable = ptr_idio;
			}
		}

		if( ( sptrEditable == 0 || sptrChromo == 0) && !bSingle )
			continue;
		else if( ( sptrEditable == 0 || sptrChromo == 0) && bSingle )
			return false;

		IIdioDrawObjectPtr sptrRect = sptrEditable;

		IUnknown *punkParent = 0;

		INamedDataObject2Ptr ptrND = sptrEditable;
 		ptrND->GetParent( &punkParent );

		sptrEditable->set_params( -1, -1 );

		FillUndoInfo( punkParent, sptrEditable );
		FillRedoInfo( punkParent, sptrEditable );

		if( punkParent )
			punkParent->Release();

		if( bSingle )
			break;
	}


	{
		CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
		if( pwnd )
			pwnd->InvalidateRect( 0 );
	}

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );

	IViewPtr sptrView = m_punkTarget;

	if( sptrView != 0 )
	{
		IUnknown *punkDoc = 0;

		sptrView->GetDocument( &punkDoc );
		::FireEvent( punkDoc, szEventCarioChange, 0, punkObj, 0, 0 );
	}

	if( punkObj )
		punkObj->Release();


	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend2::DoUndo()
{
	do_undo();

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	::FireEvent( m_punkTarget, szEventCarioChange, 0, punkObj, 0, 0 );

	if( punkObj )
		punkObj->Release();

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend2::DoRedo()
{
	do_redo();

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	::FireEvent( m_punkTarget, szEventCarioChange, 0, punkObj, 0, 0 );

	if( punkObj )
		punkObj->Release();

	return true;
}

/***************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//
//CActionAddIdioBend
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionAddIdioBend, CCmdTargetEx);

// {5B941ABD-D099-482f-8D90-A8BE10436B41}
GUARD_IMPLEMENT_OLECREATE(CActionAddIdioBend, "ChromosEx.AddIdioBend", 
0x5b941abd, 0xd099, 0x482f, 0x8d, 0x90, 0xa8, 0xbe, 0x10, 0x43, 0x6b, 0x41);

// {DB59FB40-E8ED-410a-B780-D0EE4DD87819}
static const GUID guidAddIdioBend = 
{ 0xdb59fb40, 0xe8ed, 0x410a, { 0xb7, 0x80, 0xd0, 0xee, 0x4d, 0xd8, 0x78, 0x19 } };

ACTION_INFO_FULL(CActionAddIdioBend, IDS_ADD_BEND2, -1, -1, guidAddIdioBend );
ACTION_TARGET(CActionAddIdioBend, szTargetViewSite);

CActionAddIdioBend::CActionAddIdioBend()
{
	m_bSputnic = false;
	m_bSkipInvoke = false;
	m_hDelZoneCursor = m_hCursor = m_hcurOld = 0;
	m_bDelZone = m_bLBtnDown = false;
	m_nOffset = 2;
	m_fHotZone = ::GetValueInt( ::GetAppUnknown(), CARIO_VIEW_ANALIZE , KARYO_IDIO_EDIT_HOTZONE , 2 );

	::ZeroMemory( &m_ptMice, sizeof( POINT ) );
}

CActionAddIdioBend::~CActionAddIdioBend(void)
{
	if( m_hCursor )
		::DestroyCursor( m_hCursor );

	if( m_hDelZoneCursor )
		::DestroyCursor( m_hDelZoneCursor );
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend::Invoke()
{
	{
		IUnknown	*punkDoc = 0;
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &punkDoc );
		sptrIDocument	sptrD( punkDoc );

		m_lTargetKey = ::GetObjectKey( sptrD );

		if( !punkDoc )
			return false;

		punkDoc->Release();
	}
	
	if( m_bSkipInvoke )
		return true;

	ICarioView2Ptr ptrView   = m_punkTarget;
	if( ptrView == 0 )
		return false;

	int nLine = -1, nCell = -1, nChromo = -1;
	ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

	if( nLine == -1 || nCell == -1 || nChromo == -1 )
		return false;

	IUnknown *punkIdio = 0;
	ptrView->GetIdioObject( nLine, nCell, nChromo, idtEditable, &punkIdio );

	if( !punkIdio )
		return false;

	IIdioBendsPtr ptrBends = punkIdio;
	IIdioDrawObjectPtr ptrIdioDraw = punkIdio;
	punkIdio->Release();

	IUnknown *punkParent = 0;

	INamedDataObject2Ptr ptrND = punkIdio;
	ptrND->GetParent( &punkParent );

	FillUndoInfo( punkParent, punkIdio );

	CRect rcIdio;

	ptrView->GetChromoRect( GCR_CHROMOSOME, nLine, nCell, nChromo, &rcIdio );
	rcIdio = ::ConvertToWindow(  ptrView, rcIdio );

	if( !rcIdio.PtInRect( m_ptFirst ) && !rcIdio.PtInRect( m_ptSecond ) )
		return false;


	long lSz = 0;
	ptrBends->GetBengsCount( &lSz );

	double nTopOffset = ( m_ptFirst.y - rcIdio.top ) / ::GetZoom( m_punkTarget );
	double fTopOffset = 0 ;

	double nTopOffset2 =  ( m_ptSecond.y - rcIdio.top )  / ::GetZoom( m_punkTarget );
	double fTopOffset2 = 0;

	fTopOffset = _to_chromo( m_punkTarget, nLine, nCell, nChromo, nTopOffset, ( m_ptFirst.x - rcIdio.left ) / ::GetZoom( m_punkTarget ) );
	
	if( nTopOffset != nTopOffset2 )
		fTopOffset2 = _to_chromo( m_punkTarget, nLine, nCell, nChromo, nTopOffset2, ( m_ptSecond.x - rcIdio.left )  / ::GetZoom( m_punkTarget ) );
	else
		fTopOffset2 = fTopOffset;

	if( fTopOffset2 < fTopOffset )
	{
		double a = fTopOffset2;
		fTopOffset2 = fTopOffset;
		fTopOffset = a;
	}

	if( m_ptFirst == m_ptSecond )
	{
		if( m_bDelZone )
		{
			m_bDelZone = false;

			double fMin = 0, fMax = 0;
			bool bCentr = false;
			for( long i = 0; i < lSz; i++ )
			{
				ptrBends->GetBendRange( i, &fMin, &fMax );

				int nFlag = 0;
				ptrBends->GetBendFlag( i, &nFlag );

				if( nFlag == IDIB_CENTROMERE )
					bCentr = true;

				if( fTopOffset >= fMin && fTopOffset < fMax )
				{
					double fMin2 = 0, fMax2 = 0;

					int nFlag = 0;
					ptrBends->GetBendFlag( i, &nFlag );

					if( fTopOffset - fMin < fMax - fTopOffset )
					{
						if( i - 1 >= 0 )
						{
							ptrBends->GetBendRange( i - 1, &fMin2, &fMax2 );
							ptrBends->SetBendRange( i - 1, fMin2, fMax );

							//ptrBends->SetBendFlag( i - 1, nFlag );
						}
					}
					else
					{
						if( i + 1 < lSz )
						{
							ptrBends->GetBendRange( i + 1, &fMin2, &fMax2 );
							ptrBends->SetBendRange( i + 1, fMin, fMax2 );

							ptrBends->SetBendFlag( i + 1, nFlag );
						}
					}


					if( !bCentr )
					{
						_bstr_t s_old, s_new;
						CString strNew, strPrev;
					
						int nFlag = 0;
						ptrBends->GetBendFlag( i + 1, &nFlag );

						if( nFlag == IDIB_CENTROMERE )
							i--; // [max] bt: 3458

						ptrBends->GetBendNumber( i, s_old.GetAddress() );
						ptrBends->GetBendNumber( i + 1, s_new.GetAddress() );

						strPrev = (char*)s_old;
						strNew = (char*)s_new;
	
						_correct_numbers( ptrBends, strNew, strPrev, i );

						if( nFlag == IDIB_CENTROMERE )
							i++; // [max] bt: 3458
					}
					else
					{
						_bstr_t s_old, s_new;
						CString strNew, strPrev;
					
						ptrBends->GetBendNumber( i + 1, s_old.GetAddress() );
						ptrBends->GetBendNumber( i, s_new.GetAddress() );

						strPrev = (char*)s_old;
						strNew = (char*)s_new;

						_correct_numbers( ptrBends, strNew, strPrev, i + 1 );
					}


					ptrBends->Remove( i );

					CPoint _pt;
					::GetCursorPos( &_pt );
					CWnd *pV = ::GetWindowFromUnknown( m_punkTarget );
					if( pV )
						pV->ScreenToClient( &_pt );

					_pt = ConvertToClient( m_punkTarget, _pt );
					DoMouseMove( _pt );
 					::SetCursor( m_hcurActive );


//					_Draw();

					break;
				}
			}
		}
		else
		{
			if( m_ptSecond.y >= rcIdio.top && m_ptSecond.y <= rcIdio.bottom )
			{
				if( !lSz )
				{
					ptrBends->Add( 0, fTopOffset, IDIC_DEFAULT, 0, IDIB_SIMPLE );
					ptrBends->Add( fTopOffset, 1, IDIC_DEFAULT, 0, IDIB_CENTROMERE );
				}
				else
				{
					double fMin = 0, fMax = 0;

					bool bCentr = false;
					for( long i = 0; i < lSz; i++ )
					{
						ptrBends->GetBendRange( i, &fMin, &fMax );

						int nFlag = 0;
						ptrBends->GetBendFlag( i, &nFlag );

						if( nFlag == IDIB_CENTROMERE )
							bCentr = true;

						if( fTopOffset >= fMin && fTopOffset <= fMax )
						{
							ptrBends->SetBendRange( i, fMin, fTopOffset );
							if( !bCentr )
							{
								_bstr_t bstrHint, bstrHint2;
								ptrBends->GetBendNumber( i, bstrHint.GetAddress() );

								ptrBends->AddAfter( i, fTopOffset, fMax, IDIC_DEFAULT, 0, IDIB_SIMPLE );
								ptrBends->SetBendNumber( i + 1, bstrHint );

								CString str2 = (char*)bstrHint;
								str2 = _incr_change( str2, -1, 1  );

								if( i >= 0 )
									_correct_numbers( ptrBends, str2, (char*)bstrHint, i );
							}
							else
							{
								_bstr_t bstrHint, bstrHint2;
								ptrBends->GetBendNumber( i, bstrHint.GetAddress() );

								if( i + 1 != lSz )
									ptrBends->GetBendNumber( i + 1, bstrHint2.GetAddress() );

								CString str = (char*)bstrHint;
								str = _incr_change( str, -1, 1  );

								ptrBends->AddAfter( i, fTopOffset, fMax, IDIC_DEFAULT, 0, IDIB_SIMPLE );
								ptrBends->SetBendNumber( i + 1, _bstr_t( str ) );

								if( i + 1 != lSz )
									_correct_numbers( ptrBends, str, (char*)bstrHint, i + 1, true );
							}

							CPoint _pt;
							::GetCursorPos( &_pt );
							CWnd *pV = ::GetWindowFromUnknown( m_punkTarget );
							if( pV )
								pV->ScreenToClient( &_pt );

							_pt = ConvertToClient( m_punkTarget, _pt );
 							DoMouseMove( _pt );
		 					::SetCursor( m_hcurActive );

							_Draw();
							break;
						}
					}
				}
			}
		}
	}
	else
	{
		int nMinID = -1, nMaxID = -1;
		double fMinVal = 0, fMaxVal = 0;
		bool bCentr = false;
		for( long i = 0; i < lSz; i++ )
		{
			double fMin = 0, fMax = 0;
			ptrBends->GetBendRange( i, &fMin, &fMax );

			int nFlag = 0;
			ptrBends->GetBendFlag( i, &nFlag );

			if( nFlag == IDIB_CENTROMERE )
				bCentr = true;

			if( fTopOffset >= fMin && fTopOffset <= fMax )
			{
				nMinID = i;
				fMinVal = fMin;
			}

			if( fTopOffset2 >= fMin && fTopOffset2 <= fMax )
			{
				nMaxID = i;
				fMaxVal = fMax;
			}

			if( nMinID != -1 && nMaxID != -1 )
				break;
		}

		if( nMinID != -1 && nMaxID != -1 )
		{
			ptrBends->SetBendRange( nMinID, fMinVal, fTopOffset );
			if( nMinID != nMaxID )
			{

				for( long i = nMaxID - 1, lCount = 0; i > nMinID; i--, lCount++ )
					ptrBends->Remove( i );

				
				ptrBends->SetBendRange( nMaxID - lCount, fTopOffset2, fMaxVal );
				ptrBends->SetBendFlag( nMaxID - lCount, IDIB_SPUTNIC );
			}
			else
			{
				_bstr_t bstr;
				int color = 0;
				ptrBends->GetBendNumber( nMinID, bstr.GetAddress() );
				ptrBends->GetBendFillStyle( nMinID, &color );

				ptrBends->AddAfter( nMinID, fTopOffset2, fMaxVal, color, bstr, IDIB_SPUTNIC );
				CString str = _incr_change( (char*)bstr, -1, 1  );
				
				if( bCentr )
					_correct_numbers( ptrBends, str, (char*)bstr, nMinID + 1  );
				else
					_correct_numbers( ptrBends, str, (char*)bstr, nMinID  );
			}
		}
	}
	FillRedoInfo( punkParent, punkIdio );

	if( punkParent )
		punkParent->Release();

	{
		CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
		if( pwnd )
			pwnd->InvalidateRect( rcIdio );

		ICarioView2Ptr sptrV = m_punkTarget;

		BOOL bState = 0;
		sptrV->GetShowAllChromoAddLines( &bState );
		sptrV->SetShowAllChromoAddLines( bState );
	}


	return true;
}
/////////////////////////////////////////////////////////////////////////////
double CActionAddIdioBend::_to_chromo( IUnknown *punkTarget, int nLine, int nCell, int nChromo, double fVal, double fVal2, double *pfLen, double *pY, FPoint *pPt1, FPoint *pPt2, int *pnIndex )
{
	ICarioView2Ptr ptrView = punkTarget;

	bool bErected = 0;
	ptrView->GetErectedState(  nLine, nCell, nChromo, &bErected );

	IUnknown *punkCh = 0;
	ptrView->GetChromoObject( nLine, nCell, nChromo, &punkCh );

	double fZoom = 0;
	long lOffset = 0;

	ptrView->GetChromoOffsets( nLine, nCell, nChromo, &fZoom, &lOffset );

	IChromosomePtr sptrChromo = punkCh;

	if( punkCh )
		punkCh->Release();
	
	long lSize = 0;
	BOOL bIsManual = TRUE;

	sptrChromo->GetAxisSize( AT_Mirror|AT_Rotated|AT_VisAngle|AT_LongAxis | ( bErected ? 0 : AT_Original ), &lSize, &bIsManual );
	if( lSize )
	{
		FPOINT* arPt = new FPOINT[lSize];
		sptrChromo->GetAxis( AT_AxisDouble | AT_Mirror|AT_Rotated|AT_VisAngle|AT_LongAxis | ( bErected ? 0 : AT_Original ), (POINT*)arPt );

		CPoint ptCenter( 0, 0 );
		sptrChromo->GetCentromereCoord( AT_Mirror|AT_Rotated|AT_VisAngle|AT_ShortAxis | ( bErected ? 0 : AT_Original ), &ptCenter, 0, &bIsManual );

		bool bInv = 0;

		if( arPt[0].y > arPt[lSize-1].y )
			bInv = 1;

		double nCentromereY = fVal / fZoom;
		double nCentromereX = fVal2 / fZoom;
		/*
		{
			ICarioView2Ptr ptrCView   = m_punkTarget;

			RECT rc, rc2;	 
			
			ptrCView->GetChromoRect( GCR_CHROMOSOME, nLine, nCell, nChromo, &rc );
			ptrCView->GetChromoRect( GCR_GRAPH, nLine, nCell, nChromo, &rc2 );

			rc = _correct( rc );
			rc2 = _correct( rc2 );

			nCentromereY = ( fVal ) * ( rc2.bottom - rc2.top ) / (double) ( rc.bottom - rc.top );
		}  
		*/

		//ходим вдоль центральной линии. заодно считаем ее пиксельную длину
		double chromo_length = 0, fCentrLength = 0;

		bool bFound = 0;
										  
		int nIndex = -1;

		if( !bInv )
		{
			double fLen = 1e308;
			for( int j = 0;j < lSize; j++ )
			{
				double fVal = _hypot( arPt[j].x - nCentromereX, arPt[j].y - nCentromereY );

				if( fVal < fLen )
				{
					fLen = fVal;
					nIndex = j;
				}
			}
		}
		else
		{
			double fLen = 1e307;
			for( int j = lSize - 1;j >= 0; j-- )
			{
				double fVal = _hypot( arPt[j].x - nCentromereX, arPt[j].y - nCentromereY );

				if( fVal < fLen )
				{
					fLen = fVal;
					nIndex = j;
				}
			}
		}

		if( nIndex == -1 )
			return 0;

		if( pnIndex )
			*pnIndex = nIndex;

		if( !bInv )
		{
			for( int j=0;j<lSize;j++ )
			{
				if( j > 0 )
					chromo_length += sqrt((arPt[j].x-arPt[j-1].x)*(arPt[j].x-arPt[j-1].x)
						+(arPt[j].y-arPt[j-1].y)*(arPt[j].y-arPt[j-1].y));

				if( !bFound )
				{
					if( nIndex != j )
						fCentrLength = chromo_length;
					else
					{
						fCentrLength = chromo_length;

						if( pfLen )
							*pfLen = fCentrLength;

						if( pY )
							*pY = arPt[j].y;

						if( pPt1 && pPt2 )
						{
							ICarioView2Ptr ptrCView   = punkTarget;
							ptrCView->GetShowChromoAddLinesCoord( nLine, nCell, nChromo, j, &pPt1->x, &pPt1->y, &pPt2->x, &pPt2->y );
						}
						bFound = 1;
					}
				}

			}
		}
		else
		{
			for( int j = lSize - 1; j > 0; j-- )
			{
				if( j > 0 )
					chromo_length += sqrt((arPt[j].x-arPt[j-1].x)*(arPt[j].x-arPt[j-1].x)
						+(arPt[j].y-arPt[j-1].y)*(arPt[j].y-arPt[j-1].y));

				if( !bFound )
				{
						if( nIndex != j )
							fCentrLength = chromo_length;
						else
						{
							fCentrLength = chromo_length;

							if( pfLen )
								*pfLen = fCentrLength;

							if( pY )
								*pY = arPt[j].y;
							if( pPt1 && pPt2 )
							{
								ICarioView2Ptr ptrCView   = punkTarget;
								ptrCView->GetShowChromoAddLinesCoord( nLine, nCell, nChromo, j, &pPt1->x, &pPt1->y, &pPt2->x, &pPt2->y );
							}
							bFound = 1;
						}
				}

			}
		}

		delete []arPt;	arPt = 0;

		return fCentrLength / chromo_length;
	}	
	return fVal;
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend::IsAvaible()
{
	ICarioView2Ptr ptrView   = m_punkTarget;
	if( ptrView == 0 )
		return false;

	int nLine = -1, nCell = -1, nChromo = -1;
	ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

	if( nLine == -1 || nCell == -1 || nChromo == -1 )
		return false;

	if( ptrView->IsEmpty() == S_OK )
		return false;

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( cvm != cvmCellar )
		return false;

	IUnknown *punkIdio = 0;
	ptrView->GetIdioObject( nLine, nCell, nChromo, idtEditable, &punkIdio );

	if( punkIdio )
	{
		punkIdio->Release();
		
		BOOL bShow = 0;
		ptrView->GetShowAllEditableIdio( &bShow );

		if( !bShow )
			return false;

		return true;
	}

	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend::DoUndo()
{
	do_undo();

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	::FireEvent( m_punkTarget, szEventCarioChange, 0, punkObj, 0, 0 );

	if( punkObj )
		punkObj->Release();

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend::DoRedo()
{
	do_redo();

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	::FireEvent( m_punkTarget, szEventCarioChange, 0, punkObj, 0, 0 );

	if( punkObj )
		punkObj->Release();

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend::Initialize()
{
	if( !__super::Initialize() ) 
		return false;

	::GetCursorPos( &m_ptMice );

	CWnd *pWnd = ::GetWindowFromUnknown( m_punkTarget );

	if( pWnd )
		pWnd->ScreenToClient( &m_ptMice );

	if( !m_hCursor )
	{
		m_hcurOld = AfxGetApp()->LoadCursor( IDC_ARROW );
		m_hCursor = AfxGetApp()->LoadCursor( IDC_IDIO_EDIT );
	}

	if( !m_hDelZoneCursor )
		m_hDelZoneCursor = AfxGetApp()->LoadCursor( IDC_IDIO_EDIT_CUT );

	m_mnuMain.LoadMenu( IDR_MENU_EDITBEND );
	m_pMenu = m_mnuMain.GetSubMenu( 0 );

	m_mnuMain2.LoadMenu( IDR_MENU_EDITBEND2 );
	m_pMenu2 = m_mnuMain2.GetSubMenu( 0 );

	{
		CPoint _pt;
		::GetCursorPos( &_pt );
		CWnd *pV = ::GetWindowFromUnknown( m_punkTarget );
		if( pV )
			pV->ScreenToClient( &_pt );

		_pt = ConvertToClient( m_punkTarget, _pt );
		DoMouseMove( _pt );
		::SetCursor( m_hcurActive );
	}


	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend::DoLButtonDown( CPoint pt )
{
	if( !_activate_chromosome( pt ) )
	{
		pt = _correct( pt );


		if( IsAvaible() )
		{
			m_bLBtnDown = true;

			CWnd *pWnd = GetWindowFromUnknown( m_punkTarget );

			if( !pWnd )
				return false;

			m_ptFirst = pt;

			if( !m_bSputnic )
			{
				ICarioView2Ptr ptrView   = m_punkTarget;
				if( ptrView == 0 )
					return false;

				int nLine = -1, nCell = -1, nChromo = -1;
				ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

				if( nLine == -1 || nCell == -1 || nChromo == -1 )
					return false;

				CRect rcIdio;

				ptrView->GetChromoRect( GCR_CHROMOSOME, nLine, nCell, nChromo, &rcIdio );
				rcIdio = ::ConvertToWindow(  ptrView, rcIdio );

				if( rcIdio.PtInRect( m_ptFirst) && rcIdio.PtInRect( m_ptSecond ) )
				{
					if( m_bDelZone )
						m_hcurActive = m_hDelZoneCursor;
					else
						m_hcurActive = m_hCursor;
				}
			}

		}
		return true;
	}
	else
		m_ptMice = pt;

	return false; 
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend::DoLButtonUp( CPoint pt )
{
	pt = _correct( pt );


	if( m_bLBtnDown )
	{
		CWnd *pWnd = GetWindowFromUnknown( m_punkTarget );

		if( !pWnd )
			return false;

		m_bLBtnDown = false;

		m_ptSecond = pt;

		if( !m_bSputnic )
		{
			ICarioView2Ptr ptrView   = m_punkTarget;
			if( ptrView == 0 )
				return false;

			int nLine = -1, nCell = -1, nChromo = -1;
			ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

			if( nLine == -1 || nCell == -1 || nChromo == -1 )
				return false;

			CRect rcIdio;

			ptrView->GetChromoRect( GCR_CHROMOSOME, nLine, nCell, nChromo, &rcIdio );
			rcIdio = ::ConvertToWindow(  ptrView, rcIdio );

			if( rcIdio.PtInRect( m_ptFirst) && rcIdio.PtInRect( m_ptSecond ) )
				Finalize();
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend::DoMouseMove(CPoint pt)
{
 	pt = _correct( pt );

	
	{
		ICarioView2Ptr ptrView   = m_punkTarget;
		if( ptrView == 0 )
			return false;

		int nLine = -1, nCell = -1, nChromo = -1;
		ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

		if( nLine == -1 || nCell == -1 || nChromo == -1 )
			return false;

		IUnknown *punkIdio = 0;
		ptrView->GetIdioObject( nLine, nCell, nChromo, idtEditable, &punkIdio );
	
		if( !punkIdio )
			return false;

		IIdioBendsPtr ptrBends = punkIdio;
		IIdioDrawObjectPtr ptrIdioDraw = punkIdio;
 		punkIdio->Release();
		   
		CRect rcIdio, rc2;
		ptrView->GetChromoRect( GCR_CHROMOSOME, nLine, nCell, nChromo, &rcIdio );
		ptrView->GetChromoRect( GCR_IDIOGRAM, nLine, nCell, nChromo, &rc2 );
		rcIdio = _correct( rcIdio );
		rc2 = _correct( rc2 );

		double fY = 0;
		double nTopOffset = ( pt.y - rcIdio.top ) / GetZoom( m_punkTarget );
 		double fTopOffset = _to_chromo( m_punkTarget, nLine, nCell, nChromo, nTopOffset, ( pt.x - rcIdio.left ) / GetZoom( m_punkTarget ), 0, &fY );

		long lSz = 0;
		ptrBends->GetBengsCount( &lSz );
		double fMin = 0, fMax = 0;

		double fIdioHeight = rcIdio.Height() / GetZoom( m_punkTarget );

		m_bDelZone = false;
		m_bSputnic = false;
		
		for( long i = 0; i < lSz; i++ )
		{
			ptrBends->GetBendRange( i, &fMin, &fMax );

  			if( fTopOffset >= fMin && fTopOffset < fMax )
			{
 				double fVal = 1;

				if( ( i > 0 && fabs( fTopOffset - fMin ) * fIdioHeight <= fVal ) || 
					i < lSz && fabs( fMax - fTopOffset ) * fIdioHeight <= fVal )
				{
					int nFlag = 0;
					ptrBends->GetBendFlag( i, &nFlag );

					int nFlag2 = 0;

					if( i + 1 < lSz ) 
						ptrBends->GetBendFlag( i + 1, &nFlag2 );

					if( nFlag == IDIB_CENTROMERE && fabs( fMax * fIdioHeight - nTopOffset ) <= fVal )
					{
						if( i + 1 < lSz )
							m_bDelZone = true;
					}
					else if(  nFlag != IDIB_CENTROMERE )
					{
						if( fabs( fMax - fTopOffset ) * fIdioHeight <= fVal && nFlag2 != IDIB_CENTROMERE )
						{
							if( i + 1 < lSz )
								m_bDelZone = true;
						}
						else if( fabs( fTopOffset - fMin ) * fIdioHeight <= fVal )
							m_bDelZone = true;
					}
					break;
				}
			}
		}

		m_bSputnic = false;
		for( long i = 1; i < lSz; i++ )
		{
			double _min, _max;
			ptrBends->GetBendRange( i - 1, &_min, &_max );
			ptrBends->GetBendRange( i, &fMin, &fMax );

			if( _max != fMin && fTopOffset >= _max && fTopOffset <= fMin )
				m_bSputnic = true;
		}



		if( !rcIdio.PtInRect( pt ) )
			m_bDelZone = 0;

	}

	{
		ICarioView2Ptr ptrView   = m_punkTarget;
		if( ptrView == 0 )
			return false;

		int nLine = -1, nCell = -1, nChromo = -1;
		ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

		CRect rcAll;
		ptrView->GetChromoRect( GCR_CHROMOSOME, nLine, nCell, nChromo, &rcAll );
		rcAll = _correct( rcAll );

		if( !m_bSputnic && rcAll.PtInRect( pt ) )
		{
			if( m_bDelZone )
				m_hcurActive = m_hDelZoneCursor;
			else
				m_hcurActive = m_hCursor;
		}
		else
			m_hcurActive = m_hcurOld;
	}

	if( m_ptMice != pt )
	{
		m_ptMice = pt;
		CWnd *pWnd = GetWindowFromUnknown( m_punkTarget );
		if( pWnd )
		{
			ICarioView2Ptr ptrView   = m_punkTarget;
			if( ptrView == 0 )
				return false;

			int nLine = -1, nCell = -1, nChromo = -1;
			ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

			CRect rcAll;
			ptrView->GetChromoRect( GCR_ALL, nLine, nCell, nChromo, &rcAll );
			rcAll = _correct( rcAll );

			pWnd->InvalidateRect( rcAll );
		}
	}	  

	if( m_hcurActive )
	 	::SetCursor( m_hcurActive );

	return false;
}
/////////////////////////////////////////////////////////////////////////////
void CActionAddIdioBend::DoDraw(CDC &dc)
{
	ICarioView2Ptr ptrCView   = m_punkTarget;
	if( ptrCView == 0 )
		return;

	int nLine = -1, nCell = -1, nChromo = -1;
	ptrCView->GetActiveChromo( &nLine, &nCell, &nChromo );

	if( nLine == -1 || nCell == -1 || nChromo == -1 )
		return;

	RECT rc, rc2, rc3;
	
	ptrCView->GetChromoRect( GCR_CHROMOSOME, nLine, nCell, nChromo, &rc );
	ptrCView->GetChromoRect( GCR_GRAPH, nLine, nCell, nChromo, &rc2 );
	ptrCView->GetChromoRect( GCR_IDIOGRAM, nLine, nCell, nChromo, &rc3 );

	rc = _correct( rc );
	rc2 = _correct( rc2 );
	rc3 = _correct( rc3 );

	int x1 = rc.left, x2 = rc.right, y = m_ptMice.y < rc.top ? rc.top : ( m_ptMice.y > rc.bottom ? rc.bottom : m_ptMice.y );

	double fLen = 0, fY = 0;
	FPoint pt1, pt2;
	double fCentr = _to_chromo( m_punkTarget, nLine, nCell, nChromo, (y  - rc.top)/ ::GetZoom( m_punkTarget ), (( m_ptMice.x < rc.left ? rc.left : ( m_ptMice.x > rc.right ? rc.right : m_ptMice.x ) )  - rc.left ) / ::GetZoom( m_punkTarget ), &fLen, &fY, &pt1, &pt2 );
	y = fY;

	fCentr = fLen / fCentr;
	int y2 = ( fLen ) * ( rc3.bottom - rc3.top ) / (double) ( fCentr ) + rc3.top;

	if( y2 < rc3.top )
		y2 = rc3.top;
								 
	if( y2 > rc3.bottom )
		y2 = rc3.bottom;

	_get_zoom();

	pt1.x *= m_fZoom;
	pt1.y *= m_fZoom;

	pt2.x *= m_fZoom;
	pt2.y *= m_fZoom;

	y += rc.top;

	CRect TmpRC = CRect( x1, y, x2, y );
	if( !m_rcOld.IsRectNull() )
	{
		if( !m_bPopUpMenu )
		{
			if( !m_bLBtnDown )
			{
				if( !m_bSputnic )
				{
					dc.MoveTo( pt1.x + rc.left, pt1.y + rc.top );
					dc.LineTo( pt2.x + rc.left, pt2.y + rc.top );

					dc.MoveTo( rc2.left, y2 );
					dc.LineTo( rc2.right, y2 );

					dc.MoveTo( rc3.left, y2 );
					dc.LineTo( rc3.right, y2 );
				}
			}
			else if( m_bLBtnDown && m_ptFirst != m_ptSecond )
			{
				FPoint _pt1, _pt2;
				int _y2 = 0;
				{
					int _y = m_ptFirst.y < rc.top ? rc.top : ( m_ptFirst.y > rc.bottom ? rc.bottom : m_ptFirst.y );

					double _fY = 0;
					double fLen = 0;
					double fCentr = _to_chromo( m_punkTarget, nLine, nCell, nChromo, (_y  - rc.top)/ ::GetZoom( m_punkTarget ), (( m_ptFirst.x < rc.left ? rc.left : ( m_ptFirst.x > rc.right ? rc.right : m_ptFirst.x ) )  - rc.left ) / ::GetZoom( m_punkTarget ), &fLen, &_fY, &_pt1, &_pt2 );
					_y = _fY;

					fCentr = fLen / fCentr;
					_y2 = ( fLen ) * ( rc3.bottom - rc3.top ) / (double) ( fCentr ) + rc3.top;

					if( _y2 < rc3.top )
						_y2 = rc3.top;

					if( _y2 > rc3.bottom )
						_y2 = rc3.bottom;
				}

				_pt1.x *= m_fZoom;
				_pt1.y *= m_fZoom;

				_pt2.x *= m_fZoom;
				_pt2.y *= m_fZoom;
				
				dc.MoveTo( pt1.x + rc.left, pt1.y + rc.top );
				dc.LineTo( pt2.x + rc.left, pt2.y + rc.top );

				dc.MoveTo( _pt1.x + rc.left, _pt1.y + rc.top );
				dc.LineTo( _pt2.x + rc.left, _pt2.y + rc.top );


				dc.MoveTo( rc2.left, y2 );
				dc.LineTo( rc2.right, y2 );

				dc.MoveTo( rc3.left, y2 );
				dc.LineTo( rc3.right, y2 );

				dc.MoveTo( rc2.left, _y2 );
				dc.LineTo( rc2.right, _y2 );

				dc.MoveTo( rc3.left, _y2 );
				dc.LineTo( rc3.right, _y2 );

				/*
				HBRUSH hBrush = ::CreateSolidBrush( ::GetSysColor( COLOR_HIGHLIGHT ) );
				
				::FillRect( dc, &rc, hBrush );
				::FillRect( dc, &rc2, hBrush );
				
				::DeleteObject( hBrush );
				*/
			}
		}
	}
	m_rcOld = CRect( x1, y, x2, y );
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend::DoRButtonDown( CPoint pt )
{
	_activate_chromosome( pt );
	m_ptMice = _correct( pt );

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CActionAddIdioBend::DoRButtonUp( CPoint pt )
{
	pt = _correct( pt );

	if( !m_bDelZone )
	{
		bool bSputnic = !m_bSputnic;

		CWnd *pWnd = GetWindowFromUnknown( m_punkTarget );

		if( !pWnd )
			return false;

		RECT rc;
		pWnd->GetWindowRect( &rc );
		m_ptSecond = pt;

		m_bPopUpMenu = true;
		_Draw();

		if( bSputnic )
			m_pMenu->TrackPopupMenu( TPM_RIGHTBUTTON, rc.left + pt.x, rc.top + pt.y, pWnd );
		else
			m_pMenu2->TrackPopupMenu( TPM_RIGHTBUTTON, rc.left + pt.x, rc.top + pt.y, pWnd );
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CActionAddIdioBend::OnListenMessage( MSG * pmsg, LRESULT *plResult )
{
	if( pmsg->message == WM_COMMAND)
	{
		UINT nColorID = (UINT)pmsg->wParam;

		ICarioView2Ptr ptrView   = m_punkTarget;
		if( ptrView == 0 )
			return false;

		int nLine = -1, nCell = -1, nChromo = -1;
		ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

		if( nLine == -1 || nCell == -1 || nChromo == -1 )
			return false;

		IUnknown *punkIdio = 0;
		ptrView->GetIdioObject( nLine, nCell, nChromo, idtEditable, &punkIdio );

		IUnknown *punkParent = 0;

		INamedDataObject2Ptr ptrND = punkIdio;
		ptrND->GetParent( &punkParent );

		FillUndoInfo( punkParent, punkIdio );


		if( !punkIdio )
			return false;

		IIdioBendsPtr ptrBends = punkIdio;
		IIdioDrawObjectPtr ptrIdioDraw = punkIdio;
		punkIdio->Release();

		CRect rcIdio;
		ptrView->GetChromoRect( GCR_CHROMOSOME, nLine, nCell, nChromo, &rcIdio );
		rcIdio = _correct( rcIdio );

		double nTopOffset = ( m_ptSecond.y - rcIdio.top ) / ::GetZoom( m_punkTarget );
		double fTopOffset = _to_chromo( m_punkTarget, nLine, nCell, nChromo, nTopOffset, ( m_ptSecond.x - rcIdio.left ) / ::GetZoom( m_punkTarget ) );

		long lSz = 0;
		ptrBends->GetBengsCount( &lSz );
		double fMin = 0, fMax = 0;

		for( long i = 0; i < lSz; i++ )
		{
			ptrBends->GetBendRange( i, &fMin, &fMax );

			if( fTopOffset >= fMin && fTopOffset < fMax )
			{
				switch( nColorID )
				{
				case IDS_BEND_CENTROMERE:
					ptrBends->SetBendFlag( i, IDIB_CENTROMERE );
					break;
				case IDS_BEND_SPUTNIC:
					ptrBends->SetBendFlag( i, IDIB_SPUTNIC );
					break;
				case IDS_BEND_LINK:
					ptrBends->SetBendFlag( i, IDIB_LINK );
					break;
				case IDS_BEND_SIMPLE:
					ptrBends->SetBendFlag( i, IDIB_SIMPLE );
					break;
				case ID_BEND_DELETE:
					if( lSz > 1 )
					{
						double fMin2 = 0, fMax2 = 0;
						if( i + 1 < lSz )
						{
							ptrBends->GetBendRange( i + 1, &fMin2, &fMax2 );
							ptrBends->SetBendRange( i + 1, fMin, fMax2 );
						}
						else
						{
							ptrBends->GetBendRange( i - 1, &fMin2, &fMax2 );
							ptrBends->SetBendRange( i - 1, fMin2, fMax );
						}
						ptrBends->Remove( i );
						FillRedoInfo( punkParent, punkIdio );

						if( punkParent )
							punkParent->Release();
						{
							CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
							if( pwnd )
								pwnd->InvalidateRect( rcIdio );
								ICarioView2Ptr sptrV = m_punkTarget;

								BOOL bState = 0;
								sptrV->GetShowAllChromoAddLines( &bState );
								sptrV->SetShowAllChromoAddLines( bState );
						}
						m_bSkipInvoke = true;
						Finalize();
						return __super::OnListenMessage( pmsg, plResult );
					}
					break;
					case ID_BEND_DELETE_ALL:
						ptrBends->Remove( -1 );
//						ptrBends->Add( 0, 1, IDIC_DEFAULT, 0, IDIB_SIMPLE );
						FillRedoInfo( punkParent, punkIdio );

						if( punkParent )
							punkParent->Release();
						{
							CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
							if( pwnd )
								pwnd->InvalidateRect( rcIdio );
							ICarioView2Ptr sptrV = m_punkTarget;

							BOOL bState = 0;
							sptrV->GetShowAllChromoAddLines( &bState );
							sptrV->SetShowAllChromoAddLines( bState );
						}
						m_bSkipInvoke = true;
						Finalize();
						return __super::OnListenMessage( pmsg, plResult );
					break;
				}
			}
		}

		if( nColorID == ID_SPUTNIC_DELETE )
		{
			if( lSz > 1 )
			{
				double fMin = 0, fMax = 0;
				ptrBends->GetBendRange( 0, &fMin, &fMax );
				double _prev = fMax;
				double _prev2 = fMin;

				for( long i = 1; i < lSz; i++ )
				{	
					ptrBends->GetBendRange( i, &fMin, &fMax );

					if( fTopOffset >= _prev && fTopOffset <= fMin )
					{
						ptrBends->SetBendRange( i - 1, _prev2, fMin );
						break;
					}

					_prev = fMax;
					_prev2 = fMin;
				}

				FillRedoInfo( punkParent, punkIdio );
				m_bSkipInvoke = true;
				Finalize();

				if( punkParent )
					punkParent->Release();
				{
					CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
					if( pwnd )
						pwnd->InvalidateRect( rcIdio );
						ICarioView2Ptr sptrV = m_punkTarget;

						BOOL bState = 0;
						sptrV->GetShowAllChromoAddLines( &bState );
						sptrV->SetShowAllChromoAddLines( bState );
				}
				return __super::OnListenMessage( pmsg, plResult );
			}
			}

		FillRedoInfo( punkParent, punkIdio );

		if( punkParent )
			punkParent->Release();

		{
			CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
			if( pwnd )
				pwnd->InvalidateRect( rcIdio );
		}

	}
	else if( pmsg->message == WM_EXITMENULOOP)
		m_bPopUpMenu = false;
	
	return __super::OnListenMessage( pmsg, plResult );
}
/********************************************************************************************************
*																										*
*	«акраска бендов																						*
*																										*
/********************************************************************************************************/
IMPLEMENT_DYNCREATE(CActionFillAndNumberIdioBend, CCmdTargetEx);

// {5F953A8D-9C0C-4a5c-A7DA-AB873E6CDE12}
GUARD_IMPLEMENT_OLECREATE(CActionFillAndNumberIdioBend, "ChromosEx.SetIdioFillAndNumber", 
0x5f953a8d, 0x9c0c, 0x4a5c, 0xa7, 0xda, 0xab, 0x87, 0x3e, 0x6c, 0xde, 0x12);

// {3C9D7AD8-3D99-4033-94C3-AFB1D6FE74AB}
static const GUID guidSetIdioFillAndNumber = 
{ 0x3c9d7ad8, 0x3d99, 0x4033, { 0x94, 0xc3, 0xaf, 0xb1, 0xd6, 0xfe, 0x74, 0xab } };

ACTION_INFO_FULL(CActionFillAndNumberIdioBend, IDS_SET_FILL_AND_NUMBER, -1, -1, guidSetIdioFillAndNumber );
ACTION_TARGET(CActionFillAndNumberIdioBend, szTargetViewSite);

CActionFillAndNumberIdioBend::CActionFillAndNumberIdioBend()
{
	m_pMenu = 0;
}

CActionFillAndNumberIdioBend::~CActionFillAndNumberIdioBend()
{
	if( m_pMenu )
	{
		m_pMenu->Destroy();
		delete m_pMenu;
	}
}

bool CActionFillAndNumberIdioBend::DoRButtonUp( CPoint pt )
{
	_activate_chromosome( pt );
	CPoint _pt = _correct( pt );
	::GetCursorPos( &pt );
	CWnd *pView = ::GetWindowFromUnknown( m_punkTarget );
	if( pView )
		pView->ScreenToClient( &pt );

	ICarioView2Ptr ptrView   = m_punkTarget;
	if( ptrView == 0 )
		return false;

	int nLine = -1, nCell = -1, nChromo = -1;
	ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

	if( nLine == -1 || nCell == -1 || nChromo == -1 )
		return false;

	IUnknown *punkIdio = 0;
	ptrView->GetIdioObject( nLine, nCell, nChromo, idtEditable, &punkIdio );

	if( !punkIdio )
		return false;

	IIdioBendsPtr ptrBends = punkIdio;
	IIdioDrawObjectPtr ptrIdioDraw = punkIdio;
	punkIdio->Release();

	CRect rcIdio;
	ptrView->GetChromoRect( GCR_IDIOGRAM, nLine, nCell, nChromo, &rcIdio );
	rcIdio = _correct( rcIdio );

	double nTopOffset = pt.y - rcIdio.top;
	double fTopOffset = nTopOffset / (double)rcIdio.Height();

	m_ptSecond = pt;
	pt = _pt;

	long lSz = 0;
	ptrBends->GetBengsCount( &lSz );
	double fMin = 0, fMax = 0;

	{

		for( long i = 0; i < lSz; i++ )
		{
			ptrBends->GetBendRange( i, &fMin, &fMax );

			if( fTopOffset >= fMin && fTopOffset < fMax )
			{
				int nFlag = 0;
				ptrBends->GetBendFlag( i, &nFlag );

				m_pMenu->CheckMenuItem( IDS_BEND_CENTROMERE, nFlag == IDIB_CENTROMERE ? MF_CHECKED : MF_UNCHECKED );
				m_pMenu->CheckMenuItem( IDS_BEND_SPUTNIC,    nFlag == IDIB_SPUTNIC    ? MF_CHECKED : MF_UNCHECKED );
				m_pMenu->CheckMenuItem( IDS_BEND_LINK,       nFlag == IDIB_LINK       ? MF_CHECKED : MF_UNCHECKED );
				m_pMenu->CheckMenuItem( IDS_BEND_SIMPLE,     nFlag == IDIB_SIMPLE     ? MF_CHECKED : MF_UNCHECKED );
			}
		}

		CWnd *pWnd = GetWindowFromUnknown( m_punkTarget );

		if( !pWnd )
			return false;

		RECT rc;
		pWnd->GetWindowRect( &rc );


		m_bPopUpMenu = true;
		m_pMenu->TrackPopupMenu( TPM_RIGHTBUTTON, rc.left + pt.x, rc.top + pt.y, pWnd );
	}
	return true;
}

template<UINT nID> class number_dlg : public dlg_impl
{
	CString m_strNum;
	bool m_bReverse;
public:
	number_dlg( CString strNum ) : dlg_impl( nID )
	{
		m_strNum = strNum;
		m_bReverse = false;
	}
	CString _get_str() { return m_strNum; }

	virtual LRESULT on_initdialog()
	{
		::SendMessage( ::GetDlgItem( handle(), IDC_SPIN1 ), UDM_SETRANGE, 0, MAKELPARAM( 4, 1 ) );
		::SendMessage( ::GetDlgItem( handle(), IDC_SPIN2 ), UDM_SETRANGE, 0, MAKELPARAM( 99, 1) );
		::SendMessage( ::GetDlgItem( handle(), IDC_SPIN3 ), UDM_SETRANGE, 0, MAKELPARAM( 99, 1) );
		::SendMessage( ::GetDlgItem( handle(), IDC_SPIN4 ), UDM_SETRANGE, 0, MAKELPARAM( 99, 1) );

		if( !m_strNum.IsEmpty() )
		{
			int nPos = m_strNum.Find( ".", 0 );
			CString token;
			
			if( nPos != -1 )
				token = m_strNum.Left( nPos );
			else
				token = m_strNum;

			char ch = token.Left( 1 )[0];
			if( !( ch >= '0' && ch <= '9' ) )
				m_bReverse = true;

			if( m_bReverse )
				::SendMessage(   ::GetDlgItem( handle(), IDC_CHECK7 ), BM_SETCHECK, 1, 0);


			::SetWindowText( ::GetDlgItem( handle(), IDC_EDIT1 ), token.Left( 1 ) );

			if( isdigit_ru( token.Left( 1 )[0] ) )
				::SendMessage( ::GetDlgItem( handle(), IDC_SPIN1 ), UDM_SETPOS, 0, (LPARAM) MAKELONG((short) atoi( token.Left( 1 ) ), 0) );
			else
				::SendMessage( ::GetDlgItem( handle(), IDC_SPIN1 ), UDM_SETPOS, 0, (LPARAM) MAKELONG((short) token.Left( 1 )[0] - 'A' + 1, 0) );
			
			if( token.GetLength() - 1 > 0 )
			{
				CString str = CString( token ).Right( (int)strlen( token ) - 1 );
				::SetWindowText( ::GetDlgItem( handle(), IDC_EDIT2 ),  str );
				::SendMessage(   ::GetDlgItem( handle(), IDC_CHECK_N2 ), BM_SETCHECK, 1, 0);

				::SendMessage( ::GetDlgItem( handle(), IDC_SPIN2 ), UDM_SETPOS, 0, (LPARAM) MAKELONG((short) atoi( str ), 0) );
			}
			else
			{
				::SetWindowText( ::GetDlgItem( handle(), IDC_EDIT2 ),    "1"  );
				::SendMessage(   ::GetDlgItem( handle(), IDC_CHECK_N2 ), BM_SETCHECK, 0, 0);

				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT4 ), false );

				::EnableWindow( ::GetDlgItem( handle(), IDC_CHECK_N3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_CHECK_N4 ), false );

				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN2 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN4 ), false );
			}
			
			if( nPos >= 0 )
				token = m_strNum.Tokenize( ".", nPos );
			else
			{
				token = "1";
				::SendMessage(   ::GetDlgItem( handle(), IDC_CHECK_N3 ), BM_SETCHECK, 0, 0);

				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT4 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_CHECK_N4 ), false );

				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN4 ), false );

			}

			if( nPos >= 0 )
				::SendMessage(   ::GetDlgItem( handle(), IDC_CHECK_N3 ), BM_SETCHECK, 1, 0);

			if( token )
			{
				::SetWindowText( ::GetDlgItem( handle(), IDC_EDIT3 ),    token );
				::SendMessage( ::GetDlgItem( handle(), IDC_SPIN3 ), UDM_SETPOS, 0, (LPARAM) MAKELONG((short) atoi( token ), 0) );
			}

			if( nPos >= 0 )
				token = m_strNum.Tokenize( ".", nPos );
			else
			{
				token = "1";
				::SendMessage(   ::GetDlgItem( handle(), IDC_CHECK_N4 ), BM_SETCHECK, 0, 0);

				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT4 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN4 ), false );
			}

			if( nPos >= 0 )
				::SendMessage(   ::GetDlgItem( handle(), IDC_CHECK_N4 ), BM_SETCHECK, 1, 0);

			if( token )
			{
				::SetWindowText( ::GetDlgItem( handle(), IDC_EDIT4 ),    token );
				::SendMessage( ::GetDlgItem( handle(), IDC_SPIN4 ), UDM_SETPOS, 0, (LPARAM) MAKELONG((short) atoi( token ), 0) );
			}
		}
		else
		{
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT2 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT4 ), false );

				::EnableWindow( ::GetDlgItem( handle(), IDC_CHECK_N3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_CHECK_N4 ), false );

				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN2 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN4 ), false );
		}
		return __super::on_initdialog();
	}

	LRESULT	handle_message(UINT m, WPARAM w, LPARAM l)
	{
		if( m == WM_COMMAND && LOWORD( w ) == IDC_CHECK_N2 && HIWORD( w ) == BN_CLICKED )
		{
			UINT nState = (UINT)::SendMessage(   ::GetDlgItem( handle(), IDC_CHECK_N2 ), BM_GETSTATE, 0, 0);

			if( nState & BST_CHECKED )
			{
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT2 ), true );
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT4 ), false );

				::EnableWindow( ::GetDlgItem( handle(), IDC_CHECK_N3 ), true );
				::EnableWindow( ::GetDlgItem( handle(), IDC_CHECK_N4 ), false );

				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN2 ), true );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN4 ), false );
			}
			else
			{
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT2 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT4 ), false );

				::EnableWindow( ::GetDlgItem( handle(), IDC_CHECK_N3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_CHECK_N4 ), false );

				::SendMessage(   ::GetDlgItem( handle(), IDC_CHECK_N3 ), BM_SETCHECK, 0, 0);
				::SendMessage(   ::GetDlgItem( handle(), IDC_CHECK_N4 ), BM_SETCHECK, 0, 0);

				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN2 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN4 ), false );
			}
		}
		else if( m == WM_COMMAND && LOWORD( w ) == IDC_CHECK_N3 && HIWORD( w ) == BN_CLICKED )
		{
			UINT nState = (UINT)::SendMessage( ::GetDlgItem( handle(), IDC_CHECK_N3 ), BM_GETSTATE, 0, 0 );

			if( nState & BST_CHECKED )
			{
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT3 ), true );
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT4 ), false );

				::EnableWindow( ::GetDlgItem( handle(), IDC_CHECK_N4 ), true );

				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN3 ), true );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN4 ), false );
			}
			else
			{
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT4 ), false );

				::EnableWindow( ::GetDlgItem( handle(), IDC_CHECK_N4 ), false );
				::SendMessage(   ::GetDlgItem( handle(), IDC_CHECK_N4 ), BM_SETCHECK, 0, 0);

				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN3 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN4 ), false );
			}
		}
		else if( m == WM_COMMAND && LOWORD( w ) == IDC_CHECK_N4 && HIWORD( w ) == BN_CLICKED )
		{
			UINT nState = (UINT)::SendMessage( ::GetDlgItem( handle(), IDC_CHECK_N4 ), BM_GETSTATE, 0, 0 ) - 520;

			if( nState & BST_CHECKED )
			{
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT4 ), true );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN4 ), true );
			}
			else
			{
				::EnableWindow( ::GetDlgItem( handle(), IDC_EDIT4 ), false );
				::EnableWindow( ::GetDlgItem( handle(), IDC_SPIN4 ), false );
			}
		}
		else if( m == WM_COMMAND && LOWORD( w ) == IDC_CHECK7 && HIWORD( w ) == BN_CLICKED )
		{
			UINT nState = (UINT)::SendMessage( ::GetDlgItem( handle(), IDC_CHECK7 ), BM_GETSTATE, 0, 0 ) - 8;

			m_bReverse = nState != 0;
			char str[MAX_PATH];
			GetWindowText( ::GetDlgItem( handle(), IDC_EDIT1 ), str, MAX_PATH );
			SetWindowText( ::GetDlgItem( handle(), IDC_EDIT1 ), str );
		}
		else if( m == WM_COMMAND && LOWORD( w ) == IDC_EDIT1 && HIWORD( w ) == EN_CHANGE )
		{
			static bool bSkip = false;

			if( !bSkip )
			{
				char str[MAX_PATH];
				GetWindowText( ::GetDlgItem( handle(), IDC_EDIT1 ), str, MAX_PATH );
				if( str[0] >= '0' && str[0] <= '9' && m_bReverse )
					str[0] = 'A' +  ( str[0] - '0' ) - 1;
				if( !( str[0] >= '0' && str[0] <= '9' ) && !m_bReverse )
					str[0] = '0' +  ( str[0] - 'A' ) + 1;

				bSkip = true;
				SetWindowText( ::GetDlgItem( handle(), IDC_EDIT1 ), str );
				bSkip = false;
			}
		}
		return __super::handle_message( m, w, l );
	}
    
	virtual void on_ok()
	{
		LPSTR pChs = new CHAR FAR[5];
		m_strNum = "";
		
		::GetWindowText( ::GetDlgItem( handle(), IDC_EDIT1 ), pChs, 5 );

		if( pChs[0] != '-' )
			m_strNum += CString( pChs );
		else
			m_strNum += CString( pChs + 1 );

		if( 1 )
		{
			::GetWindowText( ::GetDlgItem( handle(), IDC_EDIT2 ), pChs, 5 );
		
			if( pChs[0] != '-' )
				m_strNum += CString( pChs );
			else
				m_strNum += CString( pChs + 1 );		
		}

		if( ::SendMessage( ::GetDlgItem( handle(), IDC_CHECK_N3 ), BM_GETCHECK, 0, 0) & BST_CHECKED )
		{
			::GetWindowText( ::GetDlgItem( handle(), IDC_EDIT3 ), pChs, 5 );
	
			if( pChs[0] != '-' )
				m_strNum += "." + CString( pChs );
			else
				m_strNum += "." + CString( pChs + 1 );
		}

		if( ::SendMessage( ::GetDlgItem( handle(), IDC_CHECK_N4 ), BM_GETCHECK, 0, 0) & BST_CHECKED )
		{
			::GetWindowText( ::GetDlgItem( handle(), IDC_EDIT4 ), pChs, 5 );

			if( pChs[0] != '-' )
				m_strNum += "." + CString( pChs );
			else
				m_strNum += "." + CString( pChs + 1 );
		}

		__super::on_ok();
	}
};


bool CActionFillAndNumberIdioBend::DoLButtonDown( CPoint pt )
{
	_activate_chromosome( pt );
	CPoint _pt = _correct( pt );

	::GetCursorPos( &pt );
	CWnd *pView = ::GetWindowFromUnknown( m_punkTarget );
	if( pView )
		pView->ScreenToClient( &pt );

	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return false;

	int nLine = -1, nCell = -1, nChromo = -1;
	ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

	if( nLine == -1 || nCell == -1 || nChromo == -1 )
		return false;

	IUnknown *punkIdio = 0;
	ptrView->GetIdioObject( nLine, nCell, nChromo, idtEditable, &punkIdio );

	if( !punkIdio )
		return false;

	IIdioBendsPtr ptrBends = punkIdio;
	IIdioDrawObjectPtr ptrIdioDraw = punkIdio;
	punkIdio->Release();

	IUnknown *punkParent = 0;

	INamedDataObject2Ptr ptrND = punkIdio;
 	ptrND->GetParent( &punkParent );

	FillUndoInfo( punkParent, punkIdio );

	CRect rcIdio;
	ptrView->GetChromoRect( GCR_IDIOGRAM, nLine, nCell, nChromo, &rcIdio );
	rcIdio = _correct( rcIdio );

	double nTopOffset = pt.y - rcIdio.top;
	double fTopOffset = nTopOffset / (double)rcIdio.Height();
	pt = _pt;

	long lSz = 0;
	ptrBends->GetBengsCount( &lSz );
	double fMin = 0, fMax = 0;

	CString strPrev, strNew;
	int nMain = 0;
	bool bCentr = false;

	for( long i = 0; i < lSz; i++ )
	{
		int nFlag = 0;
		ptrBends->GetBendFlag( i, &nFlag );

		if( nFlag == IDIB_CENTROMERE )
			bCentr = true;

		ptrBends->GetBendRange( i, &fMin, &fMax );

		if( fTopOffset >= fMin && fTopOffset <= fMax )
		{
			_bstr_t bstr;
			ptrBends->GetBendNumber( i, bstr.GetAddress() );
			strPrev = (char*)bstr;

			number_dlg<IDD_IDIO_NUMS_DLG> mDlg( (char*)bstr );
			if( mDlg.do_modal( ::GetActiveWindow() ) == IDOK )
			{
				strNew = mDlg._get_str();

				CString sA, sB, sC, sD;
				_get_parts( strNew, sA, sB, sC, sD );

				CString _sA, _sB, _sC, _sD;
				_get_parts( strPrev, _sA, _sB, _sC, _sD );


				int nNew = 0, nOld = 0;

				if( !sA.IsEmpty() )
					nNew++;
				if( !sB.IsEmpty() )
					nNew++;
				if( !sC.IsEmpty() )
					nNew++;
				if( !sD.IsEmpty() )
					nNew++;


				if( !_sA.IsEmpty() )
					nOld++;
				if( !_sB.IsEmpty() )
					nOld++;
				if( !_sC.IsEmpty() )
					nOld++;
				if( !_sD.IsEmpty() )
					nOld++;

				if( nNew == nOld )
				{
					if( nFlag != IDIB_CENTROMERE )
					{
						_bstr_t bstr;
						if( bCentr )
						{
							if( ( i - 1 ) >= 0 )
								ptrBends->GetBendNumber( i - 1, bstr.GetAddress() );
						}
						else
						{
							if( ( i + 1 ) < lSz )
							{
								int nFlag = 0;
								ptrBends->GetBendFlag( i + 1, &nFlag );

								if( nFlag != IDIB_CENTROMERE )
									ptrBends->GetBendNumber( i + 1, bstr.GetAddress() );
							}
						}

						CString strVal = (char*)bstr;

						if( !strVal.IsEmpty() )
						{
							CString __sA, __sB, __sC, __sD;
							_get_parts( strVal, __sA, __sB, __sC, __sD );

							bool bOK = false, bChange = false;

							if( nNew == 4 && sA == __sA && sB == __sB && sC == __sC )
							{
								if( atoi( sD ) > atoi( __sD ) )
									bOK = true;
								else
								{
									sD.Format( "%d", atoi( __sD ) + 1 );
									bChange = true;
								}
							}

							if( !bOK && !bChange )
							{
								if( nNew == 3 && sA == __sA && sB == __sB )
								{
									if( atoi( sC ) > atoi( __sC ) )
										bOK = true;
									else
									{
										sC.Format( "%d", atoi( __sC ) + 1 );
										bChange = true;
									}
								}
							}

							if( !bOK && !bChange )
							{
								if( sA == __sA )
								{
									if( atoi( sB ) > atoi( __sB ) )
										bOK = true;
									else
									{
										sB.Format( "%d", atoi( __sB ) + 1 );
										bChange = true;
									}
								}
							}

							if( !bOK && !bChange )
							{
								if( sA != __sA )
								{
									if( isdigit_ru( sA[0] ) && isdigit_ru( __sA[0] ) )
									{
										if( atoi( sA ) > atoi( __sA ) )
											bOK = true;
										else
										{
											if( atoi( __sA ) + 1 <= 4 )
												sA.Format( "%d", atoi( __sA ) + 1 );
											else
												sA = "4";
											bChange = true;
										}
									}
									else
									{
										int nA = 0, __nA = 0;

										if( !isdigit_ru( sA[0] ) )
											nA = sA[0] - 'A' + 1;
										else
											nA = atoi( sA );


										if( !isdigit_ru( __sA[0] ) )
											__nA = __sA[0] - 'A' + 1;
										else
											__nA = atoi( __sA );


										if( nA >= __nA )
											bOK = true;
										else
										{
											sA.Format( "%d", atoi( __sA ) + 1 );
											bChange = true;
										}
									}
								}
							}

							if( !bOK )
								strNew = _set_parts( sA, sB, sC, sD );
						}
					}
				}

				ptrBends->SetBendNumber( i, _bstr_t( strNew ) );
			}
			nMain = i;

			break;
		}
	}

	if( !strNew.IsEmpty() &&  strPrev != strNew )
		_correct_numbers( punkIdio, strNew, strPrev, nMain, true );

	CWnd *pWnd = GetWindowFromUnknown( m_punkTarget );

	FillRedoInfo( punkParent, punkIdio );

	if( punkParent )
		punkParent->Release();
	Finalize();

	if( !pWnd )
		return false;

	pWnd->InvalidateRect( 0 );

	return true;
}
 

bool CActionFillAndNumberIdioBend::Invoke()
{
	{
		IUnknown	*punkDoc = 0;
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &punkDoc );
		sptrIDocument	sptrD( punkDoc );

		m_lTargetKey = ::GetObjectKey( sptrD );

		if( !punkDoc )
			return false;

		punkDoc->Release();
	}
	return true;
}

bool CActionFillAndNumberIdioBend::IsAvaible()
{
	ICarioView2Ptr ptrView   = m_punkTarget;
	if( ptrView == 0 )
		return false;

	int nLine = -1, nCell = -1, nChromo = -1;
	ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

	if( nLine == -1 || nCell == -1 || nChromo == -1 )
		return false;

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( cvm != cvmCellar )
		return false;

	if( ptrView->IsEmpty() == S_OK )
		return false;


	IUnknown *punkIdio = 0;
	ptrView->GetIdioObject( nLine, nCell, nChromo, idtEditable, &punkIdio );

	if( punkIdio )
	{
		punkIdio->Release();
		
		BOOL bShow = 0;
		ptrView->GetShowAllEditableIdio( &bShow );

		if( !bShow )
			return false;

		return true;
	}

	return false;
}

bool CActionFillAndNumberIdioBend::DoRedo()
{
	do_redo();

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	::FireEvent( m_punkTarget, szEventCarioChange, 0, punkObj, 0, 0 );

	if( punkObj )
		punkObj->Release();

	return true;
}

bool CActionFillAndNumberIdioBend::DoUndo()
{
	do_undo();

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	::FireEvent( m_punkTarget, szEventCarioChange, 0, punkObj, 0, 0 );

	if( punkObj )
		punkObj->Release();

	return true;
}
/////////////////////////////////////////////////////////////////////////////
CActionFillAndNumberIdioBend::XColorMenu::XColorMenu()
{
}

CActionFillAndNumberIdioBend::XColorMenu::~XColorMenu()
{
}

void CActionFillAndNumberIdioBend::XColorMenu::CreateMenu( IUnknown *punkTarget )
{
	if( !m_hMenu )
	{
		CreatePopupMenu();

		ICarioView2Ptr ptrView   = punkTarget;
		if( ptrView == 0 )
			return;

		int nLine = -1, nCell = -1, nChromo = -1;
		ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

		if( nLine == -1 || nCell == -1 || nChromo == -1 )
			return;

		IUnknown *punkIdio = 0;
		ptrView->GetIdioObject(  nLine, nCell, nChromo, idtEditable, &punkIdio );

		if( !punkIdio )
			return;

		IIdioDrawObjectPtr ptrIdioDraw = punkIdio;
		punkIdio->Release();

		HBRUSH hBrush = 0;

		int nTypes[] = { 0, 1, 2, 3, 4, 5, 6, 9, 7 };
		for( int i = 0; i < 9; i++ )
		{
			if( nTypes[i] == 7 )
				AppendMenu( MF_SEPARATOR );

			ptrIdioDraw->GetBrush( nTypes[i], &hBrush );
			CBitmap *pBitmap = new CBitmap;

			HBITMAP hBitmap = _make_icon( hBrush );
			pBitmap->Attach( hBitmap );

			m_cBitmaps.push_back( pBitmap );
			m_hBitmaps.push_back( hBitmap );

			AppendMenu( MF_BITMAP, nTypes[i], pBitmap );

			::DeleteObject( hBrush );
			hBrush = 0;
		}
	}
}

void CActionFillAndNumberIdioBend::XColorMenu::Destroy()
{
	int nSz = m_cBitmaps.size();

	for(int i = 0; i < nSz; i++ )
	{
		::DeleteObject( m_hBitmaps[i] );
		delete m_cBitmaps[i];
	}

	m_cBitmaps.clear();
	m_hBitmaps.clear();
	DestroyMenu();
}

HBITMAP CActionFillAndNumberIdioBend::XColorMenu::_make_icon( HBRUSH hBrush )
{
	const int nSz = 16;

	HBITMAP hBitmap = 0;

	CBrush br;
	br.Attach( hBrush );

	HDC hDC = GetDC( 0 );
	HDC hdcMem;
  	
	hdcMem = CreateCompatibleDC( hDC );
	hBitmap = CreateCompatibleBitmap( hDC, nSz, nSz );

	HBITMAP pOld = (HBITMAP)SelectObject( hdcMem, hBitmap );
	
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = rc.bottom = nSz;

	HBRUSH hOldBr = (HBRUSH)SelectObject( hdcMem, br );

	Rectangle( hdcMem, rc.left, rc.top, rc.right, rc.bottom );

	SelectObject( hdcMem, hOldBr );
	SelectObject( hdcMem, pOld );

	return hBitmap;
}

BOOL CActionFillAndNumberIdioBend::OnListenMessage( MSG * pmsg, LRESULT *plResult )
{
	if( pmsg->message == WM_COMMAND)
	{
		UINT nColorID = (UINT)pmsg->wParam;

		ICarioView2Ptr ptrView   = m_punkTarget;
		if( ptrView == 0 )
			return false;

		int nLine = -1, nCell = -1, nChromo = -1;
		ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

		if( nLine == -1 || nCell == -1 || nChromo == -1 )
			return false;

		IUnknown *punkIdio = 0;
		ptrView->GetIdioObject( nLine, nCell, nChromo, idtEditable, &punkIdio );

		IUnknown *punkParent = 0;

		INamedDataObject2Ptr ptrND = punkIdio;
		ptrND->GetParent( &punkParent );

		FillUndoInfo( punkParent, punkIdio );

		if( !punkIdio )
			return false;

		IIdioBendsPtr ptrBends = punkIdio;
		IIdioDrawObjectPtr ptrIdioDraw = punkIdio;
		punkIdio->Release();

		CRect rcIdio;
		ptrView->GetChromoRect( GCR_IDIOGRAM, nLine, nCell, nChromo, &rcIdio );
		rcIdio = _correct( rcIdio );

		double nTopOffset = m_ptSecond.y - rcIdio.top;
		double fTopOffset = nTopOffset / (double)rcIdio.Height();

		long lSz = 0;
		ptrBends->GetBengsCount( &lSz );
		double fMin = 0, fMax = 0;

		for( long i = 0; i < lSz; i++ )
		{
			ptrBends->GetBendRange( i, &fMin, &fMax );

			if( fTopOffset >= fMin && fTopOffset <= fMax )
			{
				ptrBends->SetBendFillStyle( i, nColorID );
			}
		}

		FillRedoInfo( punkParent, punkIdio );

		if( punkParent )
			punkParent->Release();

		{
			CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
			if( pwnd )
				pwnd->InvalidateRect( rcIdio );
		}
	   Finalize();
	}
	else if( pmsg->message == WM_EXITMENULOOP)
		m_bPopUpMenu = false;
	
	return __super::OnListenMessage( pmsg, plResult );
}

bool CActionFillAndNumberIdioBend::Initialize()
{
	if( !__super::Initialize() ) 
		return false;

	if( !m_pMenu )
	{	
		m_pMenu = new XColorMenu;
		m_pMenu->CreateMenu( m_punkTarget );
	} 
	return true;
}
/**************************************************************************/
/////////////////////////////////////////////////////////////////////////////
void CActionIdioEditBase::_get_zoom()
{
	IScrollZoomSitePtr site = m_punkTarget;

	if( site != 0 )
	{
		site->GetZoom( &m_fZoom );
		site->GetScrollPos( &m_ptScroll );
	}
}
/////////////////////////////////////////////////////////////////////////////
CPoint CActionIdioEditBase::_correct(CPoint pt)
{
	_get_zoom();

	pt.x *= m_fZoom;
	pt.y *= m_fZoom;

	pt.Offset( -m_ptScroll.x, -m_ptScroll.y );

	return pt;
}
/////////////////////////////////////////////////////////////////////////////
CRect CActionIdioEditBase::_correct(CRect rc)
{
	_get_zoom();

	rc.left *= m_fZoom;
	rc.top  *= m_fZoom;

	rc.right   *= m_fZoom;
	rc.bottom  *= m_fZoom;

	rc.OffsetRect( -m_ptScroll.x, -m_ptScroll.y );

	return rc;
}
/////////////////////////////////////////////////////////////////////////////
bool CActionIdioEditBase::_activate_chromosome( CPoint pt )
{
	ICarioView2Ptr	ptrView( m_punkTarget );
	if( ptrView == 0 )return false;

	int	l = -1, c = -1, p = -1;
	ptrView->GetActiveChromo( &l, &c, &p );

	int	line = -1, cell = -1, pos = -1;
	ptrView->GetCellByPoint( pt, &line, &cell, &pos, TRUE );

	if( line != l || cell != c || pos != p )
	{
		ptrView->SetActiveChromo( line, cell, pos );
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////
CString CActionIdioEditBase::_incr_change( CString str, int nPart, int nDelta )
{
	CString sA, sB, sC, sD, retStr;

	_get_parts( str, sA, sB, sC, sD );

	if( nPart == -1 )
	{
		if( !sA.IsEmpty() )
		{
			if( !sB.IsEmpty() )
			{
				if( !sC.IsEmpty() )
				{
					if( !sD.IsEmpty() )
						sD.Format( "%d", atoi( sD ) + nDelta );
					else
						sC.Format( "%d", atoi( sC ) + nDelta );
				}
				else
					sB.Format( "%d", atoi( sB ) + nDelta );
			}
			else
				sA.SetAt( 0, sA[0] + nDelta );
		}
	}
	else
	{
		if( nPart == 0 && !sA.IsEmpty() )
			sA.SetAt( 0, sA[0] + nDelta );

		if( nPart == 1 && !sB.IsEmpty() )
			sB.Format( "%d", atoi( sB ) + nDelta );

		if( nPart == 2 && !sC.IsEmpty() )
			sC.Format( "%d", atoi( sC ) + nDelta );

		if( nPart == 3 && !sD.IsEmpty() )
			sD.Format( "%d", atoi( sD ) + nDelta );
	}

	if( !sA.IsEmpty() )
	{
		retStr = sA;
		if( !sB.IsEmpty() )
		{
			retStr += sB;
			if( !sC.IsEmpty() )
			{
				retStr += "." + sC;
				if( !sD.IsEmpty() )
					retStr += "." + sD;
			}
		}
	}

	return retStr;
}
/////////////////////////////////////////////////////////////////////////////

#pragma once
///////////////////////////////////////////////////////////////////////////////
// This file is part of AWIN class library
// Copyright (c) 2001-2002 Andy Yamov
//
// Permission to use, copy, modify, distribute, and sell this software and
// its documentation for any purpose is hereby granted without fee
// 
// THE SOFTWARE IS PROVIDED_T("AS-IS") AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//

#ifndef __misc_tokenizer_h__
#define __misc_tokenizer_h__

#include "misc_utils.h"
//bool - true, false
//long	- <number>
//const _char * -_T("<string>"
//with arg name: arg=value
enum _ttype
{
	tt_other=0,
	tt_string,
	tt_dec,
	tt_hex,
	tt_fp,
	tt_yes,
	tt_no,
	tt_equ,
	tt_ob,
	tt_cb,
	tt_obr,
	tt_cbr,
	tt_coma,
};

enum	_tres
{
	tres_ok,
	tres_buffer_overflow,
	tres_end,
	tres_incorrect_stament,
	tres_unexpected_end,
	tres_invalid_num,
};

class string_tokenizer
{
public:
	struct	_token
	{
		int	type;	//0-other, 1 - dec, 2- hex, 3 - float, 4 - bool, 
		const _char *psz;
		size_t		cb;
	};
public:
	string_tokenizer( _char *psz_buffer, size_t cb_buffer )
	{m_buffer = psz_buffer;m_cb = cb_buffer, m_pos = 0;}
public:
	long		put_char( _char ch )	{*_psz() = ch; m_pos++;*_psz()=0;return tres_ok;}
	long		put_token( int type, const _char *pszdata = 0 );
	bool		get_token( _token *ptoken, bool fIgnoreComa = true ); 

	long		put_string( const _char *psz_val, const _char *pentry=0 );
	long		put_long( long lval, const _char *pentry=0 );
	long		put_bool( bool fval, const _char *pentry=0 );
	long		get_string( _char *psz, size_t cb_max, const _char *pentry=0 );
	long		get_long( long *plres, const _char *pentry=0 );
	long		get_bool( bool *pbres, const _char *pentry=0 );
protected:
	long		_put_entry( const _char *psz_entry );
	void		_correct()	{m_pos+=_tcslen(_psz());}; 
	size_t		_cb()		{return m_cb-m_pos;}
	_char		*_psz()		{return &m_buffer[m_pos];}
public:
	_char	*m_buffer;
	size_t	m_cb, m_pos;
};


inline long string_tokenizer::put_token( int type, const _char *psz_data )
{
	if( type == tt_other )
	{
		_put_entry( 0 );
		_tcsncatz( _psz(), psz_data, _cb() );
	}
	else if( type == tt_ob )
		_tcsncatz( _psz(), _T("("), _cb() );
	else if( type == tt_obr )
	{
		_put_entry( 0 );
		_tcsncatz( _psz(), _T("["), _cb() );
	}
	else if( type == tt_cb )
		_tcsncatz( _psz(), _T(")"), _cb() );
	else if( type == tt_cbr )
		_tcsncatz( _psz(), _T("]"), _cb() );

	_correct();
	return _cb()?tres_ok:tres_buffer_overflow;
}

inline long string_tokenizer::_put_entry( const _char *psz_entry )
{	
	if( m_pos != 0 && 
		m_buffer[m_pos-1] != '[' &&
		m_buffer[m_pos-1] != '(' &&
		m_buffer[m_pos-1] != 0 )
		_tcsncatz( _psz(), 
					_T(", "), 
					_cb() );

	if( psz_entry )
	{
		_tcsncatz( _psz(), psz_entry, _cb() );
		_tcsncatz( _psz(), _T("= "), _cb() );
	}
	_correct();

	return _cb()?tres_ok:tres_buffer_overflow;
}

inline long string_tokenizer::put_string( const _char *psz_val, const _char *pentry )
{
	_put_entry( pentry );
	_tcsncatz( _psz(), _T("\""), _cb() );
	_tcsncatz( _psz(), psz_val, _cb() );
	_tcsncatz( _psz(), _T("\""), _cb() );
	_correct();
	return _cb()?tres_ok:tres_buffer_overflow;
}

inline long string_tokenizer::put_long( long lval, const _char *pentry )
{
	_put_entry( pentry );
	__snprintf( _psz(), _cb(), _T("%d"), lval );
	_correct();
	return _cb()?tres_ok:tres_buffer_overflow;
}

inline long string_tokenizer::put_bool( bool fval, const _char *pentry )
{
	_put_entry( pentry );
	if( fval )_tcsncatz( _psz(), _T("yes"), _cb() );
	else _tcsncatz( _psz(), _T("no"), _cb() );
	_correct();
	return _cb()?tres_ok:tres_buffer_overflow;
}


inline long string_tokenizer::get_string( _char *psz, size_t cb_max, const _char *pentry )
{
	_token	token;
	if( !get_token( &token ) )
		return tres_end;


	//неименованый тип
	if( token.type == tt_string )
	{
		_tcsncpyz( psz, token.psz, min(token.cb, cb_max) );
		return tres_ok;
	}

	if( token.type != tt_other )//var name
		return tres_incorrect_stament;

	if( !get_token( &token ) )
		return tres_unexpected_end;

	if( token.type != tt_equ )
		return tres_incorrect_stament;

	if( !get_token( &token ) )
		return tres_unexpected_end;

	if( token.type == tt_string )
	{
		_tcsncpyz( psz, token.psz, min(token.cb, cb_max) );
		return tres_ok;
	}
	return tres_incorrect_stament;
}

inline long string_tokenizer::get_long( long *plres, const _char *pentry )
{
	_token	token;
	if( !get_token( &token ) )
		return tres_end;

	
	//неименованый тип
	if( token.type == tt_dec )
	{
		if( _sscanf( token.psz, _T("%d"), plres ) == 1 )return tres_ok;
		else return tres_invalid_num;
	}	
	if( token.type == tt_hex )
	{
		if( _sscanf( token.psz, _T("%x"), plres ) == 1 )return tres_ok;
		else return tres_invalid_num;
	}

	if( token.type != tt_other )//var name
		return tres_incorrect_stament;

	if( !get_token( &token ) )
		return tres_unexpected_end;

	if( token.type != tt_equ )
		return tres_incorrect_stament;

	if( !get_token( &token ) )
		return tres_unexpected_end;

	if( token.type == tt_dec )
	{
		if( _sscanf( token.psz, _T("%d"), plres ) == 1 )return tres_ok;
		else return tres_invalid_num;
	}	
	if( token.type == tt_hex )
	{
		if( _sscanf( token.psz, _T("%x"), plres ) == 1 )return tres_ok;
		else return tres_invalid_num;
	}

	return tres_incorrect_stament;
}

inline 	bool string_tokenizer::get_token( string_tokenizer::_token *ptoken, bool bIgnoreComa )
{
	const _char *psz = _psz();
	size_t *pinc = &m_pos;

	ptoken->psz = psz;

	while( *ptoken->psz == ' ' || 
		*ptoken->psz == '\t' || 
		*ptoken->psz == '\r' || 
		*ptoken->psz == '\n' ||
		bIgnoreComa && *ptoken->psz == ',' ) 
		ptoken->psz++;
	if( !*ptoken->psz )return false;

	struct
	{
		const _char *psz;
		int	type, len;
	}
	static tokens[]=
	{
		{_T("yes"), tt_yes, -1},
		{_T("no"), tt_no, -1},
		{_T("="), tt_equ, -1},
		{_T("("), tt_ob, -1},
		{_T(")"), tt_cb, -1},
		{_T("["), tt_obr, -1},
		{_T("]"), tt_cbr, -1},
		{_T("),"), tt_coma, -1},
		{0, -1, -1}
	};

	for( int idx = 0; tokens[idx].psz; idx++ )
	{
		if( tokens[idx].len == -1 )
			tokens[idx].len=_tcslen( tokens[idx].psz );
		if( !_tcsnicmp( ptoken->psz, tokens[idx].psz, tokens[idx].len ) )
		{
			ptoken->type = tokens[idx].type;
			ptoken->cb = tokens[idx].len;
			*pinc += (long)ptoken->psz-(long)psz+ptoken->cb;
			return true;
		}
	}

	//number
	const _char	*pszt = ptoken->psz;
	bool	fFloatFlag = false;
	bool	fHexFlag = false;
	
	while( *pszt == '.' || *pszt == '+' || *pszt == '-' ||
			(*pszt >= '0' && *pszt <= '9' )||
			(*pszt >= 'a'&&*pszt<='h')||
			(pszt != tokens[idx].psz && *pszt == 'h' ))
	{
		if( (*pszt >= 'a'&&*pszt<='f') )fHexFlag = true;
		if( *pszt == '.' )fFloatFlag = true;

		pszt++;

		if( *pszt == 'h' )
		{
			fHexFlag = true;
			break;
		}
	}

	if( pszt != ptoken->psz )
	{
		ptoken->cb = (long)pszt-(long)ptoken->psz;
		ptoken->type = fFloatFlag?tt_fp:(fHexFlag?tt_hex:tt_dec);
		*pinc += (long)ptoken->psz-(long)psz+ptoken->cb;
		return true;
	}

	if( *pszt == '"' )
	{
		pszt++;
		while( *pszt&&*pszt != '"' )pszt++;
		if( !*pszt )return false;
		ptoken->psz++;
		ptoken->cb=(long)pszt-(long)ptoken->psz+1;
		ptoken->type = tt_string;
		*pinc+=(long)pszt-(long)psz+1;
		return true;
	}

	while( *pszt >= 'A' && *pszt <= 'Z' ||
		   *pszt >= 'a' && *pszt <= 'z' ||
		   *pszt >= '0' && *pszt <= '9' )pszt++;

	if( pszt != ptoken->psz )
	{
		ptoken->type = tt_other;
		ptoken->cb = (long)pszt - (long)ptoken->psz;
		*pinc+=(long)pszt-(long)psz;
		return true;
	}
	ptoken->type = tt_other;
	ptoken->cb = 1;
	*pinc+=(long)pszt-(long)psz+1;
	return true;
}

inline long string_tokenizer::get_bool( bool *pbres, const _char *pentry )
{
	_token	token;
	if( !get_token( &token ) )
		return tres_end;

	
	//неименованый тип
	if( token.type == tt_yes )
	{
		*pbres = true;
		return tres_ok;
	}	
	if( token.type == tt_no )
	{
		*pbres = false;
		return tres_ok;
	}

	if( token.type != tt_other )//var name
		return tres_incorrect_stament;

	if( !get_token( &token ) )
		return tres_unexpected_end;

	if( token.type != tt_equ )
		return tres_incorrect_stament;

	if( !get_token( &token ) )
		return tres_unexpected_end;

	if( token.type == tt_yes )
	{
		*pbres = true;
		return tres_ok;
	}	
	if( token.type == tt_no )
	{
		*pbres = false;
		return tres_ok;
	}

	return tres_incorrect_stament;
}


#endif //__misc_tokenizer_h__
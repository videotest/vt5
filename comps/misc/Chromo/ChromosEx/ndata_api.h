#pragma once

class CNDataItem
{
	long lCount;
	BSTR *pbstrNames;
	_variant_t *pvars;
public:
	CNDataItem()
	{
		lCount = 0;
		pbstrNames = 0;
		pvars = 0;
	}

	~CNDataItem()
	{
		if( lCount > 0 )
		{
			for( int i = 0; i < lCount; i++ )
				::SysFreeString( pbstrNames[i] );

			delete []pbstrNames;
			delete []pvars;
		}

	}
	CNDataItem( const CNDataItem &item )
	{
		lCount = 0;
		pbstrNames = 0;
		pvars = 0;

		*this = item;
	}
	void operator=( const CNDataItem &item )
	{
		if( pbstrNames )
		{
			for( int i = 0; i < lCount; i++ )
				::SysFreeString( pbstrNames[i] );

			delete []pbstrNames;
			pbstrNames = 0;
		}
		if( pvars )
			delete []pvars;

		lCount = item.lCount;

		pbstrNames = new BSTR[lCount];
		pvars = new _variant_t[lCount];
		
		for( int i = 0; i < lCount; i++ )
		{
			pbstrNames[i] = CString( item.pbstrNames[i] ).AllocSysString();
			pvars[i] = item.pvars[i];
		}
	}
	void Load( INamedDataPtr &ptrData, CString str )
	{
		ptrData->SetupSection( _bstr_t( str ) );

		ptrData->GetEntriesCount( &lCount );

		pbstrNames = new BSTR[lCount];
		pvars = new _variant_t[lCount];
		
		for( int i = 0; i < lCount; i++ )
		{
			ptrData->GetEntryName( i, &pbstrNames[i] );
			ptrData->GetValue( pbstrNames[i], &pvars[i] );
		}
	}
	void Store( INamedDataPtr &ptrData, CString str )
	{
		ptrData->DeleteEntry( _bstr_t( str ) );
		ptrData->SetupSection( _bstr_t( str ) );
		for( int i = 0; i < lCount; i++ )
			ptrData->SetValue( pbstrNames[i], pvars[i] );
	}
	bool Find( _variant_t data )
	{
		for( int i = 0; i < lCount; i++ )
			if( pvars[i] == data )
				return true;
		return false;
	}
};

class CNDataCell
{
	_variant_t vCountFrom;
	CNDataItem *pItems;
public:
	CNDataCell()
	{
		pItems = 0;
	}
	~CNDataCell()
	{
		if( pItems )
			delete []pItems;
	}

	CNDataCell &operator =(CNDataCell &from)
	{
		vCountFrom = from.vCountFrom;
		if( vCountFrom.vt == VT_I4 && vCountFrom.lVal > 0 )
		{
			pItems = new CNDataItem[ vCountFrom.lVal ];
			for (int i = 0; i < vCountFrom.lVal; i++)
			{
				pItems[i] = from.pItems[i];
			}
		}
		return *this;
	}

	void Load( INamedDataPtr &ptrData, CString str )
	{
 		ptrData->SetupSection( _bstr_t( str ) );

		ptrData->GetValue( _bstr_t( CARIO_COUNT_PREFIX ), &vCountFrom );

		if( vCountFrom.vt == VT_I4 && vCountFrom.lVal > 0 )
		{
			pItems = new CNDataItem[ vCountFrom.lVal ];
			for( int i = 0; i < vCountFrom.lVal; i++ )
			{
				CString sInd;
				sInd.Format( "%d", i );

				CString strS = str + "\\" CARIO_CHROMO_PREFIX + sInd;
				pItems[i].Load( ptrData, strS );
			}
		}
		else
			vCountFrom = _variant_t( long( 0 ) );
	}
	void Store( INamedDataPtr &ptrData, CString str )
	{
		ptrData->DeleteEntry( _bstr_t( str ) );
		ptrData->SetupSection( _bstr_t( str ) );

		ptrData->SetValue( _bstr_t( CARIO_COUNT_PREFIX ), vCountFrom );
		if(  vCountFrom.vt == VT_I4 && vCountFrom.lVal > 0 )
		{
			for( int i = 0; i < vCountFrom.lVal; i++ )
			{
				CString sInd;
				sInd.Format( "%d", i );

				CString strS = str + "\\" CARIO_CHROMO_PREFIX + sInd;
				pItems[i].Store( ptrData, strS );
			}
		}
		else
			vCountFrom = _variant_t( long( 0 ) );

	}
	void Erase( int nInd )
	{
		_variant_t vCountTMP = vCountFrom;
		CNDataItem *pItemTMP = new CNDataItem[vCountTMP.lVal - 1];

		for( int i = 0; i < vCountFrom.lVal; i++ )
		{
			if( i < nInd )
				pItemTMP[i] = pItems[i];
			else if( i > nInd )
				pItemTMP[i - 1] = pItems[i];
		}
		vCountFrom.lVal--;

		if( pItems )
			delete []pItems;

		pItems = new CNDataItem[ vCountFrom.lVal ];
		for( i = 0; i < vCountFrom.lVal; i++ )
			pItems[i] = pItemTMP[i];
	}

	void Add( CNDataItem &item, int nInd )
	{
		_variant_t vCountTMP = vCountFrom;
		CNDataItem *pItemTMP = new CNDataItem[vCountTMP.lVal];

		for( int i = 0; i < vCountFrom.lVal; i++ )
			pItemTMP[i] = pItems[i];

		vCountFrom.lVal++;

		if( pItems )
			delete []pItems;

		pItems = new CNDataItem[ vCountFrom.lVal ];
		for( i = 0; i < vCountFrom.lVal; i++ )
		{
			if( i < nInd )
				pItems[i] = pItemTMP[i];
			else if( i == nInd )
				pItems[i] = item;
			else if( i > nInd )
				pItems[i] = pItemTMP[i-1];
		}
	}

	void Move( CNDataCell &From, int nIndFrom, int nIndTo)
	{
		Add( From.pItems[nIndFrom], nIndTo );
		From.Erase( nIndFrom );
	}
	
	bool Find( _variant_t var )
	{
		if( var.vt == VT_EMPTY )
			return false;
		for( int i = 0; i < vCountFrom.lVal; i++ )
		{	
			if( pItems[i].Find( var ) )
				return true;
		}
		return false;
	}
	CNDataItem & operator[]( int nID )
	{
		ASSERT( nID >= 0 && nID < vCountFrom.lVal );
		return pItems[nID];
	}
};

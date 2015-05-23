#pragma once

////////////////////////////////////////////////////////////////////////////
#include <winperf.h>

#define		PN_PROCESS                          TEXT("Process")
#define		PN_THREAD                           TEXT("Thread")
#define		PN_PROCESS_PRIVATE_PAGE             TEXT("Private Bytes")


#define		INDEX_STR_LEN       10
static		TCHAR       INDEX_PROCTHRD_OBJ[2*INDEX_STR_LEN] = TEXT("");

static		DWORD		PX_PROCESS = 0;
static		DWORD		PX_THREAD = 0;
static		DWORD		PX_PROCESS_PRIVATE_PAGE = 0;

typedef		PERF_DATA_BLOCK             PERF_DATA,      *PPERF_DATA;
typedef		PERF_OBJECT_TYPE            PERF_OBJECT,    *PPERF_OBJECT;
typedef		PERF_INSTANCE_DEFINITION    PERF_INSTANCE,  *PPERF_INSTANCE;
typedef		PERF_COUNTER_DEFINITION     PERF_COUNTER,   *PPERF_COUNTER;

static		HKEY        ghMachineKey = HKEY_LOCAL_MACHINE;  // get title index from this key
static		HKEY        ghPerfKey = HKEY_PERFORMANCE_DATA;


//*********************************************************************
//
//  GetPerfData
//
//      Get a new set of performance data.
//
//      *ppData should be NULL initially.
//      This function will allocate a buffer big enough to hold the
//      data requested by szObjectIndex.
//
//      *pDataSize specifies the initial buffer size.  If the size is
//      too small, the function will increase it until it is big enough
//      then return the size through *pDataSize.  Caller should
//      deallocate *ppData if it is no longer being used.
//
//      Returns ERROR_SUCCESS if no error occurs.
//
//      Note: the trial and error loop is quite different from the normal
//            registry operation.  Normally if the buffer is too small,
//            RegQueryValueEx returns the required size.  In this case,
//            the perflib, since the data is dynamic, a buffer big enough
//            for the moment may not be enough for the next. Therefor,
//            the required size is not returned.
//
//            One should start with a resonable size to avoid the overhead
//            of reallocation of memory.
//
inline DWORD   GetPerfData    (HKEY        hPerfKey,
                        LPTSTR      szObjectIndex,
                        PPERF_DATA  *ppData,
                        DWORD       *pDataSize)
{
DWORD   DataSize;
DWORD   dwR;
DWORD   Type;


    if (!*ppData)
        *ppData = (PPERF_DATA) LocalAlloc (LMEM_FIXED, *pDataSize);


    do  {
        DataSize = *pDataSize;
        dwR = RegQueryValueEx (hPerfKey,
                               szObjectIndex,
                               NULL,
                               &Type,
                               (BYTE *)*ppData,
                               &DataSize);

        if (dwR == ERROR_MORE_DATA)
            {
            LocalFree (*ppData);
            *pDataSize += 1024;
            *ppData = (PPERF_DATA) LocalAlloc (LMEM_FIXED, *pDataSize);
            }

        if (!*ppData)
            {
            LocalFree (*ppData);
            return ERROR_NOT_ENOUGH_MEMORY;
            }

        } while (dwR == ERROR_MORE_DATA);

    return dwR;
}

//*********************************************************************
//
//      RefreshPerfData
//
//  Get a new set of performance data.  pData should be NULL initially.
//
inline PPERF_DATA RefreshPerfData (HKEY        hPerfKey,
                            LPTSTR      szObjectIndex,
                            PPERF_DATA  pData,
                            DWORD       *pDataSize)
{
    if (GetPerfData (hPerfKey, szObjectIndex, &pData, pDataSize) == ERROR_SUCCESS)
        return pData;
    else
        return NULL;
}

//*********************************************************************
//
//  FirstObject
//
//      Returns pointer to the first object in pData.
//      If pData is NULL then NULL is returned.
//
inline PPERF_OBJECT FirstObject (PPERF_DATA pData)
{
    if (pData)
        return ((PPERF_OBJECT) ((PBYTE) pData + pData->HeaderLength));
    else
        return NULL;
}




//*********************************************************************
//
//  NextObject
//
//      Returns pointer to the next object following pObject.
//
//      If pObject is the last object, bogus data maybe returned.
//      The caller should do the checking.
//
//      If pObject is NULL, then NULL is returned.
//
inline PPERF_OBJECT NextObject (PPERF_OBJECT pObject)
{
    if (pObject)
        return ((PPERF_OBJECT) ((PBYTE) pObject + pObject->TotalByteLength));
    else
        return NULL;
}




//*********************************************************************
//
//  FindObject
//
//      Returns pointer to object with TitleIndex.  If not found, NULL
//      is returned.
//
inline PPERF_OBJECT FindObject (PPERF_DATA pData, DWORD TitleIndex)
{
PPERF_OBJECT pObject;
DWORD        i = 0;

    if (pObject = FirstObject (pData))
        while (i < pData->NumObjectTypes)
            {
            if (pObject->ObjectNameTitleIndex == TitleIndex)
                return pObject;

            pObject = NextObject (pObject);
            i++;
            }

    return NULL;
}




//*********************************************************************
//
//  FindObjectN
//
//      Find the Nth object in pData.  If not found, NULL is returned.
//      0 <= N < NumObjectTypes.
//
inline PPERF_OBJECT FindObjectN (PPERF_DATA pData, DWORD N)
{
PPERF_OBJECT pObject;
DWORD        i = 0;

    if (!pData)
        return NULL;
    else if (N >= pData->NumObjectTypes)
        return NULL;
    else
        {
        pObject = FirstObject (pData);

        while (i != N)
            {
            pObject = NextObject (pObject);
            i++;
            }

        return pObject;
        }
}

//*********************************************************************
//
//  FirstInstance
//
//      Returns pointer to the first instance of pObject type.
//      If pObject is NULL then NULL is returned.
//
inline PPERF_INSTANCE   FirstInstance (PPERF_OBJECT pObject)
{
    if (pObject)
        return (PPERF_INSTANCE)((PCHAR) pObject + pObject->DefinitionLength);
    else
        return NULL;
}




//*********************************************************************
//
//  NextInstance
//
//      Returns pointer to the next instance following pInst.
//
//      If pInst is the last instance, bogus data maybe returned.
//      The caller should do the checking.
//
//      If pInst is NULL, then NULL is returned.
//
inline PPERF_INSTANCE   NextInstance (PPERF_INSTANCE pInst)
{
PERF_COUNTER_BLOCK *pCounterBlock;

    if (pInst)
        {
        pCounterBlock = (PERF_COUNTER_BLOCK *)((PCHAR) pInst + pInst->ByteLength);
        return (PPERF_INSTANCE)((PCHAR) pCounterBlock + pCounterBlock->ByteLength);
        }
    else
        return NULL;
}




//*********************************************************************
//
//  FindInstanceN
//
//      Returns the Nth instance of pObject type.  If not found, NULL is
//      returned.  0 <= N <= NumInstances.
//

inline PPERF_INSTANCE FindInstanceN (PPERF_OBJECT pObject, DWORD N)
{
PPERF_INSTANCE pInst;
DWORD          i = 0;

    if (!pObject)
        return NULL;
    else if (N >= (DWORD)(pObject->NumInstances))
        return NULL;
    else
        {
        pInst = FirstInstance (pObject);

        while (i != N)
            {
            pInst = NextInstance (pInst);
            i++;
            }

        return pInst;
        }
}

//*********************************************************************
//
//  InstanceName
//
//      Returns the name of the pInst.
//
//      If pInst is NULL then NULL is returned.
//
inline BSTR InstanceName (PPERF_INSTANCE pInst)
{
    if (pInst)
        return (BSTR) ((PCHAR) pInst + pInst->NameOffset);
    else
        return NULL;
}



//*********************************************************************
//
//  GetPerfTitleSz
//
//      Retrieves the performance data title strings.
//
// 	  This call retrieves english version of the title strings.
//
// 	  For NT 3.1, the counter names are stored in the "Counters" value
// 	  in the ...\perflib\009 key.  For 3.5 and later, the 009 key is no
//      longer used.  The counter names should be retrieved from "Counter 009"
//      value of HKEY_PERFORMANCE_KEY.
//
//      Caller should provide two pointers, one for buffering the title
//      strings the other for indexing the title strings.  This function will
//      allocate memory for the TitleBuffer and TitleSz.  To get the title
//      string for a particular title index one would just index the TitleSz.
//      *TitleLastIdx returns the highest index can be used.  If TitleSz[N] is
//      NULL then there is no Title for index N.
//
//      Example:  TitleSz[20] points to titile string for title index 20.
//
//      When done with the TitleSz, caller should LocalFree(*TitleBuffer).
//
//      This function returns ERROR_SUCCESS if no error.
//
inline DWORD   GetPerfTitleSz (HKEY    hKeyMachine,
                        HKEY    hKeyPerf,
                        LPTSTR  *TitleBuffer,
                        LPTSTR  *TitleSz[],
                        DWORD   *TitleLastIdx)
{
HKEY	  hKey1;
HKEY    hKey2;
DWORD   Type;
DWORD   DataSize;
DWORD   dwR;
DWORD   Len;
DWORD   Index;
DWORD   dwTemp;
BOOL    bNT10;
LPTSTR  szCounterValueName;
LPTSTR  szTitle;




    // Initialize
    //
    hKey1        = NULL;
    hKey2        = NULL;
    *TitleBuffer = NULL;
    *TitleSz     = NULL;




    // Open the perflib key to find out the last counter's index and system version.
    //
    dwR = RegOpenKeyEx (hKeyMachine,
                        TEXT("software\\microsoft\\windows nt\\currentversion\\perflib"),
                        0,
                        KEY_READ,
                        &hKey1);
    if (dwR != ERROR_SUCCESS)
        goto done;



    // Get the last counter's index so we know how much memory to allocate for TitleSz
    //
    DataSize = sizeof (DWORD);
    dwR = RegQueryValueEx (hKey1, TEXT("Last Counter"), 0, &Type, (LPBYTE)TitleLastIdx, &DataSize);
    if (dwR != ERROR_SUCCESS)
        goto done;



    // Find system version, for system earlier than 1.0a, there's no version value.
    //
    dwR = RegQueryValueEx (hKey1, TEXT("Version"), 0, &Type, (LPBYTE)&dwTemp, &DataSize);

    if (dwR != ERROR_SUCCESS)
        // unable to read the value, assume NT 1.0
        bNT10 = TRUE;
    else
        // found the value, so, NT 1.0a or later
        bNT10 = FALSE;









    // Now, get ready for the counter names and indexes.
    //
    if (bNT10)
        {
        // NT 1.0, so make hKey2 point to ...\perflib\009 and get
        //  the counters from value "Counters"
        //
        szCounterValueName = TEXT("Counters");
        dwR = RegOpenKeyEx (hKeyMachine,
                            TEXT("software\\microsoft\\windows nt\\currentversion\\perflib\\009"),
                            0,
                            KEY_READ,
                            &hKey2);
        if (dwR != ERROR_SUCCESS)
            goto done;
        }
    else
        {
        // NT 1.0a or later.  Get the counters in key HKEY_PERFORMANCE_KEY
        //  and from value "Counter 009"
        //
        szCounterValueName = TEXT("Counter 009");
        hKey2 = hKeyPerf;
        }





    // Find out the size of the data.
    //
    dwR = RegQueryValueEx (hKey2, szCounterValueName, 0, &Type, 0, &DataSize);
    if (dwR != ERROR_SUCCESS)
        goto done;



    // Allocate memory
    //
    *TitleBuffer = (LPTSTR)LocalAlloc (LMEM_FIXED, DataSize);
    if (!*TitleBuffer)
        {
        dwR = ERROR_NOT_ENOUGH_MEMORY;
        goto done;
        }

    *TitleSz = (LPTSTR *)LocalAlloc (LPTR, (*TitleLastIdx+1) * sizeof (LPTSTR));
    if (!*TitleSz)
        {
        dwR = ERROR_NOT_ENOUGH_MEMORY;
        goto done;
        }





    // Query the data
    //
    dwR = RegQueryValueEx (hKey2, szCounterValueName, 0, &Type, (BYTE *)*TitleBuffer, &DataSize);
    if (dwR != ERROR_SUCCESS)
        goto done;




    // Setup the TitleSz array of pointers to point to beginning of each title string.
    // TitleBuffer is type REG_MULTI_SZ.
    //
    szTitle = *TitleBuffer;

    while (Len = lstrlen (szTitle))
        {
        Index = atoi (szTitle);

        szTitle = szTitle + Len +1;

        if (Index <= *TitleLastIdx)
            (*TitleSz)[Index] = szTitle;

        szTitle = szTitle + lstrlen (szTitle) +1;
        }



done:

    // Done. Now cleanup!
    //
    if (dwR != ERROR_SUCCESS)
        {
        // There was an error, free the allocated memory
        //
        if (*TitleBuffer) LocalFree (*TitleBuffer);
        if (*TitleSz)     LocalFree (*TitleSz);
        }

    // Close the hKeys.
    //
    if (hKey1) RegCloseKey (hKey1);
    if (hKey2 && hKey2 != hKeyPerf) RegCloseKey (hKey2);



    return dwR;

}

//*********************************************************************
//
//  FirstCounter
//
//      Find the first counter in pObject.
//
//      Returns a pointer to the first counter.  If pObject is NULL
//      then NULL is returned.
//
inline PPERF_COUNTER FirstCounter (PPERF_OBJECT pObject)
{
    if (pObject)
        return (PPERF_COUNTER)((PCHAR) pObject + pObject->HeaderLength);
    else
        return NULL;
}




//*********************************************************************
//
//  NextCounter
//
//      Find the next counter of pCounter.
//
//      If pCounter is the last counter of an object type, bogus data
//      maybe returned.  The caller should do the checking.
//
//      Returns a pointer to a counter.  If pCounter is NULL then
//      NULL is returned.
//
inline PPERF_COUNTER NextCounter (PPERF_COUNTER pCounter)
{
    if (pCounter)
        return (PPERF_COUNTER)((PCHAR) pCounter + pCounter->ByteLength);
    else
        return NULL;
}




//*********************************************************************
//
//  FindCounter
//
//      Find a counter specified by TitleIndex.
//
//      Returns a pointer to the counter.  If counter is not found
//      then NULL is returned.
//
inline PPERF_COUNTER FindCounter (PPERF_OBJECT pObject, DWORD TitleIndex)
{
PPERF_COUNTER pCounter;
DWORD         i = 0;

    if (pCounter = FirstCounter (pObject))
        while (i < pObject->NumCounters)
            {
            if (pCounter->CounterNameTitleIndex == TitleIndex)
                return pCounter;

            pCounter = NextCounter (pCounter);
            i++;
            }

    return NULL;

}




//*********************************************************************
//
//  CounterData
//
//      Returns counter data for an object instance.  If pInst or pCount
//      is NULL then NULL is returne.
//
inline PVOID CounterData (PPERF_INSTANCE pInst, PPERF_COUNTER pCount)
{
PPERF_COUNTER_BLOCK pCounterBlock;

    if (pCount && pInst)
        {
        pCounterBlock = (PPERF_COUNTER_BLOCK)((PCHAR)pInst + pInst->ByteLength);
        return (PVOID)((PCHAR)pCounterBlock + pCount->CounterOffset);
        }
    else
        return NULL;
}

//********************************************************
//
//  GetTitleIdx
//
//      Searches Titles[] for Name.  Returns the index found.
//
inline DWORD   GetTitleIdx (HWND hWnd, LPTSTR Title[], DWORD LastIndex, LPTSTR Name)
{
DWORD   Index;

    for (Index = 0; Index <= LastIndex; Index++)
        if (Title[Index])
            if (!lstrcmpi (Title[Index], Name))
                return Index;

    MessageBox (hWnd, Name, TEXT("Pviewer cannot find index"), MB_OK);
    return 0;
}


//********************************************************
//
//  SetPerfIndexes
//
//      Setup the perf data indexes.
//
inline void    SetPerfIndexes (HWND hWnd)
{
LPTSTR  TitleBuffer;
LPTSTR  *Title;
DWORD   Last;
TCHAR   szTemp[50];
DWORD   dwR;


    dwR = GetPerfTitleSz (ghMachineKey, ghPerfKey, &TitleBuffer, &Title, &Last);

    if (dwR != ERROR_SUCCESS)
        {
        wsprintf (szTemp, TEXT("Unable to retrieve counter indexes, ERROR -> %#x"), dwR);
        MessageBox (hWnd, szTemp, TEXT("Pviewer"), MB_OK|MB_ICONEXCLAMATION);
        return;
        }


    PX_PROCESS                       = GetTitleIdx (hWnd, Title, Last, PN_PROCESS);
	PX_PROCESS_PRIVATE_PAGE          = GetTitleIdx (hWnd, Title, Last, PN_PROCESS_PRIVATE_PAGE);
    PX_THREAD                        = GetTitleIdx (hWnd, Title, Last, PN_THREAD);

    wsprintf (INDEX_PROCTHRD_OBJ, TEXT("%ld %ld"), PX_PROCESS, PX_THREAD);

    LocalFree (TitleBuffer);
    LocalFree (Title);

}

////////////////////////////////////////////////////////////////////////////
inline long get_process_vm_size( const char* psz_process )
{
	SetPerfIndexes( 0 );

	long lvm_size = 0;
	DWORD           dw_perf_size = 50*1024;
	PPERF_DATA      pperf_data = 0;
	PPERF_OBJECT    pprocess_object = 0;

    pperf_data = RefreshPerfData( ghPerfKey, INDEX_PROCTHRD_OBJ, pperf_data, &dw_perf_size );
	if( pperf_data )
	{
		pprocess_object = FindObject( pperf_data, PX_PROCESS );
		if( pprocess_object )
		{
			PPERF_INSTANCE  pinst = 0;
			PPERF_INSTANCE  pinst_find = 0;
			int             instance_idx = 0;

			pinst = FirstInstance ( pprocess_object );
			while( pinst && instance_idx < pprocess_object->NumInstances )
			{
				BSTR bstr = InstanceName( pinst );
				if( bstr )
				{
					_bstr_t bstr_inst = bstr;
					char* psz_inst = (char*)bstr_inst;
					if( !stricmp( psz_inst, psz_process ) )
					{
						pinst_find = pinst;
						break;					
					}
				}
				pinst = NextInstance( pinst );
				instance_idx++;
			}

			if( pinst_find )
			{
				PPERF_COUNTER   pcounter = 0;
				DWORD*			pdw_data = 0;
				pcounter = FindCounter( pprocess_object, PX_PROCESS_PRIVATE_PAGE );
				if( pcounter )
				{
					pdw_data = (DWORD *) CounterData( pinst_find, pcounter );
					if( pdw_data )						
						lvm_size = *pdw_data;
				}
			}
		}
		::LocalFree( pperf_data );	pperf_data = 0;
	}
	return lvm_size;
}
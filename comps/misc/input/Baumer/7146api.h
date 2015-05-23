/*****************************************************************************
*    Program :          fgaa7146.dll                                         *
*    Version:           4.03 Beta                                            *
*    Module:            7146api.h                                            *
*    Compiler:          Visual C++ 6.0                                       *
*    Operating System:  Windows NT/2000 & Windows 95/98                      *
*    Purpose:           Declaration of exported API functions for use with   *
*                       the Baumer interface cards and some general defines  *
*    Notes:             This Modul needs an installed mapmem.sys driver for  *
*                       providing the necessary physical DMA and RPS memory  *
*                       Access to mapcon.sys driver is needed if functions   *   
*                       with support of virtual DMA memory will be used      *
*                                                                            *
*    Created:           02/21/97                                             *
*    Last Change        18/01/01                                             *
*    Developer:         D. Gottschalk                                        *
*    Copyright:         (c) Baumer Optronic GmbH                             *
*****************************************************************************/


#ifndef _7146_API_H_
#define _7146_API_H_



// Microsoft specific defines
#ifndef _WINDEF_
    typedef unsigned long       DWORD;
    typedef int                 BOOL;
    typedef unsigned char       BYTE;
    typedef unsigned short      WORD;
    typedef void                *PVOID;
#endif

// Calling Conventions 
#if defined(__BORLANDC__) || (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
    #ifndef _WINDEF_
        #define WINAPI __stdcall
    #endif
#else
    #define WINAPI
#endif


#if defined(__cplusplus)
extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////////
//////////////    defines for usage in user programs        //////////////////
//////////////////////////////////////////////////////////////////////////////

// subsystem ID's  of the Baumer interface cards
// get with API function GetInterfaceCardID


// PCI3 - Circuit 7146 Rev.V3
#define     FG_0101   0x0101      
#define     FG_0102   0x0102      
#define     FG_0103   0x0103      
#define     FG_0104   0x0104      
#define     FG_0105   0x0105      
#define     FG_0106   0x0106      
#define     FG_0107   0x0107      
#define     FG_0108   0x0108      
#define     FG_0111   0x0111      
#define     FG_0112   0x0112      
#define     FG_0113   0x0113      
#define     FG_0117   0x0117      

#define     FG_0203   0x0203      
#define     FG_0303   0x0303      
#define     FG_0313   0x0313      
#define     FG_0304   0x0304      
#define     FG_0305   0x0305      
#define     FG_0306   0x0306      
#define     FG_0308   0x0308      
    
// PCI3 - Circuit 7146 Rev.A
#define     FG_3101   0x3101      
#define     FG_3104   0x3104      
#define     FG_3105   0x3105      
#define     FG_3108   0x3108      
#define     FG_3109   0x3109      
#define     FG_3112   0x3112      
#define     FG_3306   0x3306      
#define     FG_3316   0x3316
#define     FG_3307   0x3307   
#define     FG_3308   0x3308      
#define     FG_3313   0x3313      
#define     FG_3326   0x3326      
#define     FG_3409   0x3409      

// PCI4 - Standard
#define     FG_4101   0x4101      
#define     FG_4105   0x4105      
#define     FG_4130   0x4130      
#define     FG_4230   0x4230      


// PCI4 - OEM
#define     FG_4180   0x4180      
#define     FG_4187   0x4187      
#define     FG_4188   0x4188      


// ID's of the interface cards for usage with API functions
// at present a maximum of two interface card at the same time is
// supported by the DLL
#define     FG_1        0                   // 1. interface card 
#define     FG_2        1                   // 2. interface card 
#define     FG_MAX_ANZ  2                   

// max. size of a macrofile
#define     MAX_MACROSIZE   0x10000          

// RPS ID's for usage in API functions
// There are two 'Register Program Sequencer' (RPS) in every interface card
// for the execution of the sequence of commands of a macro file
#define     RPS_1       0                   // 1. RPS 
#define     RPS_2       1                   // 2. RPS 


// image format ID's
// use GetLastScanSize to determine the image format in the DMA memory
#define     CHANNEL_FORMAT_0    0       // channels pixel by pixel
#define     CHANNEL_FORMAT_1    1       // channels line by line
#define     CHANNEL_FORMAT_2    2       // 1 channel from 4 sources for
                                        // testing purposes
                                        // 1. line: Pixel 0,4,8... 
                                        // 2. line: Pixel 1,5,9... 
                                        // 3. line: Pixel 2,6,10... 
                                        // 4. line: Pixel 3,7,11... 

// special format for FG_0117           // 2 channels per line, with mixed 
#define     CHANNEL_FORMAT_3    3       // 1 line RGB and 2 lines blind color
#define     WIDTH_FORMAT_3      1464    // fixed width for this format


// special format for FG_4180               // 3 channels per line
                                            // 16 Bit per channel, 14 Bit valid
                                            // (R, G - 8 Lines, B - 16 Lines)
#define     CHANNEL_FORMAT_4        4       // channels pixel by pixel
#define     WIDTH_FORMAT_4          10500   // fixed width for this format
#define     WIDTH_FORMAT_4_VALID     9922   // valid pixels starting from 0
                                         

// Max. size of hardware counters returned by some API-functions
// Note!
// Because of a bug in the Philips multimedia bridge SAA7146A correct line 
// counter values above 12 Bit are not guaranteed
// So the highest possible safe line counter is LINE_COUNTER_SAFE_MAX
#define     LINE_COUNTER_MAX        0x3FFFFF
#define     LINE_COUNTER_SAFE_MAX   0xFFF
#define     BLOCK_COUNTER_MAX       0x3FF
#define     SCAN_COUNTER_MAX        0x3FF


// Max. size of line counter written in DMA memory to the end of every scan
// line (2 Byte)
// only for interface cards FG_3104 and FG_0104 
#define     DMA_LINE_COUNTER_MAX   0xFFFF



// Available data sources for use with SetDataSource
// The effect of these data sources depends on the used interface card

#define     DATA_SOURCE_0   0   
#define     DATA_SOURCE_1   1


// different correction tables for use with WriteShadingTab/ReadShadingTab

// There are different kinds of shading tables
// BLACK_TABLE is used for correction of black referenz with offset values
// WHITE_TABLE is used for correction of white referenz with factor values
// TRANSFORMATION_TABLE is used for the following interface cards:
// FG_4180, FG_4187,FG_4188
// This table contains correction data for shading, geometrical distortion 
// and zoom

#define     BLACK_TABLE             0
#define     WHITE_TABLE             1
#define     TRANSFORMATION_TABLE    2 


// special typedef for interface card with ID 0x0104
// this typedef can be used to set or read the configuration
// of the current scan window (frame)

typedef struct _FRAME_STRUCT
{
    WORD BCNT;  // block counter (only read)
    WORD BHGT;  // block height
    WORD IEN;   // interrupt enable (not used)
    WORD OZ;    // line offset in pixel
    WORD LZ;    // line width in pixel

                // RAM_REG_FHG is only for compatibility
} FRAME_STRUCT, RAM_REG_FHG, *PFRAME_STRUCT, *PRAM_REG_FHG;



//////////////////////////////////////////////////////////////////////////////
// special defines for interface cards with ID 0x3307 and 0x3105 that uses the
// LoadPictureEx function and the LoopControl function
// These functions are only available under NT 4.0 and require access to the
// virtual memory manager mapcon.sys
//////////////////////////////////////////////////////////////////////////////

// maximum number of buffers for LoadPictureEx
// these number may be invalid, if the virtual memory manager mapcon.sys was
// permanently installed. In this case MAX_BUF_ANZ is handled in the Registry.
// See the Baumer API documentation for more information.

#define     MAX_BUF_ANZ     512

// number of buffers for Loop Macros
#define     LOOP_BUF_ANZ     4

// maximum of virtual memory, that can be used for one buffer
#define     MAX_MEMSIZE     0x400000 


// possible status of buffers in loop mode managed by the LoopControl function
#define  BUF_FREE           0
#define  BUF_LOCKED         1
#define  BUF_FILLED         16
#define  BUF_INVALID        0xFFFFFFFF


// Description structure of one virtual memory buffer
typedef struct 
{
    void      *vmem;        // virtuel memory pointer to buffer
    DWORD     vmemLen;      // buffer size in bytes
    DWORD     status;       // information about buffer status, only reading
                            // access
    DWORD     picNr;        // picture number if status is BUF_FILLED, only
                            // reading access 

}   MEM_BUF, *PMEM_BUF;
 

// Description structure of the virtual memory buffers for usage with
// LoadPictureEx and LoopControl
typedef struct _VMEM_STRUCT
{
        
    MEM_BUF  buffer[MAX_BUF_ANZ];  // field of buffer descriptors 

} VMEM, *PVMEM;


//////////////////////////////////////////////////////////////////////////////
///////////////  Available error codes returned by GetErrorNumber   //////////
//////////////////////////////////////////////////////////////////////////////

#define ERR_NONE                    0   // No error
#define ERR_OPEN_MEMMAP             1   // Couldn't open MemMap driver
#define ERR_DMA_BUFFER_ALLOCATION   2   // Mapping of DMA-Buffer failed
#define ERR_LOAD_DATA_TO_BUFFER     3   // Loading macro data to buffer failed
#define ERR_PARSING_MACRO           4   // Error in macro format
#define ERR_INVALID_PARAMETER       5   // Invalid API function parameter
#define ERR_NO_FRAMEGRABBER         6   // No valid PCI interface card found
#define ERR_FILE_NOT_FOUND          7   // Couldn't find a necessary file
#define ERR_TIMEOUT                 8   // Macro execution returned with
                                        // TIMEOUT 
#define ERR_ANOTHER_MACRO           9   // Macro execution is not possible
                                        // because an other macro is running
                                        // on the same RPS  
#define ERR_UNKNOWN_FG              10  // interface card with unknown ID
                                        // detected
#define ERR_START_MACRO             11  // Couldn't start macro
#define ERR_LOAD_MACRO              12  // Couldn't load macro
#define ERR_MEMORY                  13  // Not enough memory
#define ERR_MACRO_FORMAT            14  // Invalid macro format
#define ERR_INVALID_ID              15  // Invalid ID of the interface card
#define ERR_RPS_MEM                 16  // No RPS memory available
#define ERR_INVALID_RETURN          17  // One or more return parameter are
                                        // invalid
#define ERR_INVALID_CHANNELS        18  // Invalid channel number
#define ERR_READ_DEBI_PORT          19  // No read access to DEBI port
#define ERR_INVALID_FUNCTION        20  // Function call for the interface
                                        // card is not supported 
#define ERR_WRITE_DEBI_PORT         21  // No write access to DEBI port
#define ERR_FILE_READ_ERROR         22  // Error while reading a BITFILE
#define ERR_INVALID_TABLE_PTR       23  // Invalid pointer to data buffer for
                                        // shading or label table
#define ERR_INVALID_FILENAME        24  // Invalid filename
#define ERR_UNEXPECTED              25  // Unexpected error 
#define ERR_FG_NOT_SUPPORTED        26  // interface card is not supported

#define ERR_OPEN_DRIVER             30  // Couldn't open mapcon.sys driver
#define ERR_CREATE_PAGE_TABLE       31  // Couldn't allocate page table memory
#define ERR_FUNCTION_NO_ACCESS      32  // Function is already in use 
#define ERR_MACRO_SIZE              33  // Macro size is to large
#define ERR_BUFFER_NUMBER           34  // not enough buffers
#define ERR_INVALID_POINTER         35  // invalid pointer to data structure
                                        // or buffer
#define ERR_IMAGES_LOST             36  // lost images detect by LoopControl
#define ERR_BUFFER_NUMBERS          37  // mapcon driver can't handle this
                                        // number of buffers
#define ERR_INVALID_PAGETABLE       38  // Invalid calculation of pagetables
#define ERR_INVALID_BUFFER_NUMBER   39  // Buffer number for use with
                                        // LoadPictureEx not supported 
#define ERR_MAPCON_NOT_FOUND        40  // Couldn't find mapcon.sys driver
#define ERR_GET_PHYSICAL_MEMORY     41  // Couldn't get physical DMA memory
#define ERR_NUMBER_ICARDS           42  // Not enough interface card available
#define ERR_SIZE_DMA_MEMORY         43  // Invalid size of new DMA memory
#define ERR_SHADING_COMPARE         44  // A difference was detected, while 
                                        // comparing a loaded shading table
                                        // after writing the same shading table
#define ERR_LABEL_COMPARE           45  // A difference was detected, while 
                                        // comparing a loaded label table
                                        // after writing the same label table
#define ERR_ACCESS_DEBI_PORT        46  // Error accessing DEBI-port
                                        // Executing a RPS program failed
#define ERR_CALC_LINE_LENGTH        47  // Calculated Line Width has invalid
                                        // pixel
#define ERR_FUNC_NOT_AVAILABLE      48  // Function is not available for the 
                                        // current OS
#define ERR_DEVICE_NOT_AVAILABLE    49  // Function is not available for the 
                                        // current OS

#define ERR_LINE_COUNTER_UNSAFE     50  // Line counter values above 
                                        // LINE_COUNTER_SAFE_MAX maybe invalid


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// These declarations are only for compatibility with older versions
// of the Baumer API-DLL
// Please do not use these declarations for new projects 
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// same as ResetInterfaceCard
BOOL WINAPI ResetFramegrabber(int fgNr); 
// same as InitInterfaceCard
BOOL WINAPI InitFramegrabber(int fgNr, int rpsNr, int timeout);
// same as GetNumberOfInterfaceCards
int WINAPI GetNumberOfScanners(void);
// same as GetInterfaceCardID
DWORD  WINAPI GetScannerID(int fgNr);
// same as OpenDLLMonitor
void  WINAPI EditMemoryRegister(int fgNr);
// same as SetFrameSize
BOOL WINAPI SetRAMReg(RAM_REG_FHG *rregFhg, int fgNr);
// same as GetFrameSize
BOOL WINAPI GetRAMReg(RAM_REG_FHG *rregFhg, int fgNr);
// same as WriteShadingTab
int WINAPI LutWrite( DWORD *hg, DWORD tsize, int table , int source,
                               int timeout, int fgNr, int rpsNr, BOOL bCheck);
// same as ReadShadingTab
int WINAPI LutRead(DWORD *hg, DWORD tsize, int table, int source,
                                            int timeout, int fgNr, int rpsNr);



                                           
//////////////////////////////////////////////////////////////////////////////
//////////////////////////// Public API functions   //////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
////////////////////////////// General functions  ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*****************************************************************************
Functionname:       GetNumberOfInterfaceCards
Description:        Provides the number of all interface cards detected 
Parameters:
Return Value:        
    >=0             Number of interface cards  detected
Notes:              At present a maximum of two interface cards is supported
                    interface cards with identical ID numbers are possible 
*****************************************************************************/
int  WINAPI GetNumberOfInterfaceCards(void);


/*****************************************************************************
Functionname :      GetInterfaceCardID
Description:        Detects the ID number of the indicated interface card
Parameters:
    fgNr            Number of the interface card
Return Value:        
    !=0             Identification number
    0               if no valid interface card detected

  Notes:            Admissible ID numbers are fixed with the defines
                    in file "7146api.h".
*****************************************************************************/
DWORD WINAPI GetInterfaceCardID(int fgNr);


/*****************************************************************************
Functionname:       ResetInterfaceCard
Description:        Hardware reset of the indicated interface card
Parameters:
    fgNr            Number of the interface cards
Return Value:        
    TRUE            if successful
    FALSE           if the number of the interface cards was invalid

Notes:              Before calling this function you should check whether or
                    not an interface card exists as a PCI device using
                    GetNumberOfScanners.
                    Calling ResetInterfaceCard will reset the PCI-Controller
                    of the interface card to the same state as after a
                    power-on-reset.
                    For further usage of the interface card the API-function
                    InitInterfaceCard has to be called.
*****************************************************************************/
BOOL  WINAPI ResetInterfaceCard(int fgNr);


/*****************************************************************************
Functionname:       InitInterfaceCard
Description:        Initialization of the indicated baumer interface card
Parameters:
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    timeout         Timeout value in seconds
Return Value:        
    TRUE            if successful
    FALSE           if there was an error

Notes:              The initialization files predefined for this interface
                    card are loaded.
                    They have to be located in the start directory of the
                    Baumer API-DLL.
                    Before calling this function you should check whether or
                    not an interface card exists as a PCI device using
                    GetNumberOfInterfaceCards.
*****************************************************************************/
BOOL  WINAPI InitInterfaceCard(int fgNr, int rpsNr, int timeout);


/*****************************************************************************
Functionname:       InitInterfaceCardEx
Description:        Initialization of the indicated baumer interface card
Parameters:
    mcs             full path and file name of the PGA layout file
    mac1            full path and file name of the load macro 1
    mac2            full path and file name of the load macro 2
    mac3            full path and file name of the load macro 3
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    timeout         Timeout value in seconds
Return Value:        
    TRUE            if successful
    FALSE           if there was an error

Notes:              Before calling this function you should check whether or
                    not an interface card exists as a PCI device using
                    GetNumberOfInterfaceCards.
                    This function is only for experts.
                    You should be absolutely sure about the delivered files
                    and the identifier of the baumer interface card.
*****************************************************************************/
BOOL WINAPI InitInterfaceCardEx(char *mcs, char *mac1, char *mac2,
                                char *mac3, int fgNr, int rpsNr, int timeout);


/*****************************************************************************
Functionname:       GetDMAPtr
Description:        Returns the pointer on the start of the DMA memory that
                    is reserved for the interface card indicated. 
Parameters:
    fgNr            Number of the scanner card
    absolute        If TRUE the pointer returned is always the absolute start
                    of the DMA memory.
                    If FALSE the pointer may switch in LoopMode (default) 
Return Value:        
    !=NULL          A valid pointer to the start of the DMA memory
    NULL            No DMA memory available
Notes:              If a loop macro with double buffer is used for scanning
                    the pointer changes with every scan.
                    

*****************************************************************************/
PVOID WINAPI GetDMAPtr(int fgNr, BOOL absolute);


/*****************************************************************************
Functionname :      GetDMASize
Description:        Returns the size of the DMA storage area reserved for the
                    indicated interface card, if 'complete' is FALSE or if
                    only one interface card is available
                    If 'complete' is TRUE, always the complete size
                    is returned
Parameters:
    fgNr            Number of the interface card
    complete        If TRUE the whole size of the reserved statically DMA
                    memory is returnd
                    If FALSE (default), the amount of statically DMA memory
                    reserved for the interface card fgNr is returned
Return Value:        
    !=0             Size of the DMA storage area in BYTE
    0               if No DMA memory available
Notes:              The size is the quotient of the entire memory reserved
                    during the boot process by the kernel driver and the 
                    number of interface cards managed by the API-DLL.
*****************************************************************************/
DWORD WINAPI GetDMASize(int fgNr, BOOL complete);


/*****************************************************************************
Functionname:       GetRPSPtr
Description:        Returns the pointer on the start of the RPS memory that
                    is reserved for the interface card indicated. 
Parameters:
    fgNr            Number of the scanner card
    rpsNr           Number of the RPS (0 or 1) for macro execution
Return Value:        
    !=NULL          A valid pointer to the start of the RPS memory
    NULL            No DMA memory available
                   
*****************************************************************************/
PVOID WINAPI GetRPSPtr(int fgNr, int rpsNr);


/*****************************************************************************
Functionname :      GetRPSSize
Description:        Returns the size of the RPS storage area reserved for the
                    indicated interface card.
Parameters:
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
Return Value:        
    !=0             Size of the RPS storage area in BYTE
    0               if No RPS memory available
*****************************************************************************/
DWORD WINAPI GetRPSSize(int fgNr, int rpsNr);


/*****************************************************************************
Functionname :      GetAPIVersion
Description:        Determine the version of the Baumer API-DLL
Parameters:          
Return Value:        
    DWORD           Contains version information
Notes:              The version number is also shown in the property sheet
                    of the Baumer API-DLL (see OpenDLLMonitor)
*****************************************************************************/
DWORD WINAPI GetAPIVersion(void);


/*****************************************************************************
Functionname :      GetMapConVersion
Description:        Determine the version of the Baumer MapCon Driver
Parameters:          
Return Value:        
    DWORD           Version number
Notes:              The mapcon.sys driver is only used under Windows NT 
*****************************************************************************/
DWORD  WINAPI GetMapConVersion(void);


/*****************************************************************************
Functionname :      SetErrorOutput
Description:        Enable or disable the output of error messages
Parameters:
    enable          TRUE    - enable error output
                    FALSE   - disable error output
Return Value: 
Notes:              The standard setting is to have output of error messages
                    switched off
                    In this mode you can check the error status only with
                    the API-function "GetErrorNumber"
*****************************************************************************/
void WINAPI SetErrorOutput(BOOL enable);


/*****************************************************************************
Functionname :      GetErrorNumber
Description:        This function returns the error number that was the last
                    to appear
Parameters:
    
Return Value: 
    0               no errors
    != 0            error number

Notes:              If an API function gives the return values FALSE or NULL
                    (ZERO) or other inadmissible values this function can help
                    to search for the reason of the error.
                    Admissible error numbers are fixed with the error defines
                    above 
*****************************************************************************/
int WINAPI GetErrorNumber(void);



/*****************************************************************************
Functionname :      GetHomeDirectory
Description:        This function returns the directory path of the 
                    Baumer API-DLL
Parameters:
Return Value:        
    homeDir         String with the path

Notes:              This function is useful,if the current directory is not
                    the home directory of the API-DLL.
                    Some functions like LoadMacro needs the full pathname
                    of the necessary macrofile    
*****************************************************************************/
char* WINAPI GetHomeDirectory(void);


/*****************************************************************************
Functionname :      SetHomeDirectory
Description:        This function sets the directory path for the location
                    of the needed macro files for the Baumer API-DLL
Parameters:
    newHomeDir      String with the path

Return Value:        
    BOOL            FALSE, if the length of newHomeDir is  zero

Notes:              This function is useful, if the needed maco files are not
                    located in the home directory of the API-DLL
                    There is no validation of the directory path
*****************************************************************************/
BOOL WINAPI SetHomeDirectory(char *newHomeDir);


/*****************************************************************************
Functionname :      SetDataSource
Description:        Set the internal data source of the interface card hardware
Parameters:
    fgNr            Number of the scanner card
    rpsNr           Number of the RPS (0 or 1) for internal macro execution
    source          The selected data source
Return Value:        
    TRUE            If the function was successful
    FALSE           If there was an error
Notes:              This function can be used for switching between the
                    available data sources 
                    
                    The following interface cards are supported:
                    0x0303,0x0313,0x0304,0x0305,0x0306,0x0308,0x3306,0x3307,
                    0x3308,0x3313,0x3316,0x3326,0x4180,0x4187,0x4188

                    The meaning of the data sources should be explained in
                    the documentation of the interface card    
*****************************************************************************/
BOOL WINAPI SetDataSource(int fgNr, int rpsNr, int source); 



/*****************************************************************************
Functionname :      OpenDLLMonitor
Description:        Opens a five-page, "non-modal dialog" providing particular
                    options for access to and diagnosis of the hardware of the
                    available interface cards. 
Parameters:
    fgNr            Number of the interface card          
Return Value:        
Notes:              The following functions are available
                    Displaying, editing and setting all the memory registers
                    Displaying, setting and saving the DMA and the RPS 
                    strorage areas
                    Functions for working with RPS macros
                    Special testing functions
                    About dialog

                    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    This property sheet should only be used for the start-up
                    of the interface cards and for testing purposes.
                    This function should not be called in the user programmes.
                    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*****************************************************************************/
void WINAPI OpenDLLMonitor(int fgNr);



/*****************************************************************************
Functionname :      ShowDLLMonitor
Description:        Show or hide the DLL monitor
Parameters:
    show            if TRUE    -    shows the DLL monitor
                    if FALSE   -    hides the DLL monitor
Return Value:        
Notes:              Before using this function you should call OpenDLLMonitor
*****************************************************************************/
void WINAPI ShowDLLMonitor(BOOL show);



/*****************************************************************************
Functionname :      PumpDllMsg
Description:        handles messages from the main application 
Parameters:
    LPMSG lpMsg     Pointer to a message
 Return Value:          
    TRUE            If action was successful
    FALSE           If there was an error
Notes:              Call this function in your applications
                    PreTranslateMessage Function.
                    This function should be used, if the API function
                    OpenDLLMonitor (modeless dialog) will be called.
*****************************************************************************/
BOOL WINAPI PumpDllMsg(LPMSG lpMsg);




//////////////////////////////////////////////////////////////////////////////
///////////////// Functions for working with macro files /////////////////////
//////////////////////////////////////////////////////////////////////////////


/*****************************************************************************
Functionname :      LoadMacro
Description:        Loads the sequence of commands of a macro file to the
                    RPS memory
Parameters:
    macroName       Name of the  macro file
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    useRegs         TRUE - enable usage of interface card register (default)   
                    FALSE - disable usage of interface card register
Return Value:        
    TRUE            if successful
    FALSE           if there was an error
Notes:              set useRegs to FALSE, if the interface card hardware isn't
                    initialized
*****************************************************************************/
BOOL WINAPI LoadMacro(char *macroName, int fgNr, int rpsNr, BOOL useRegs);




/*****************************************************************************
Functionname :      InterpretMacro
Description:        interprets a sequence of commands of a macro buffer to the
                    RPS memory
Parameters:
    macroBuffer     Buffer to a NULL terminated macro buffer in ASCII-format
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    bDebug          TRUE - enable stepping of the macro commands
                           shows a message box for every macro line
                    FALSE- disable stepping
    bUseRegs        TRUE - enable usage of interface card register (default)   
                    FALSE - disable usage of interface card register
Return Value:        
    TRUE            if successful
    FALSE           if there was an error
Notes:              set useRegs to FALSE, if the interface card hardware isn't
                    initialized
*****************************************************************************/
BOOL WINAPI InterpretMacro(char *macroBuffer, int fgNr, int rpsNr, BOOL bDebug, BOOL bUseRegs);


/*****************************************************************************
Functionname :      StartMacro
Description:        Starts the execution of a macro that has been loaded
                    before. 
Parameters:
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    timeout         Timeout value in seconds
Return Value:        
    TRUE            if successful
    FALSE           if there was an error or timeout
Notes:              You have to make sure that the macro to be executed has
                    not been overwritten by calling other API-functions that
                    also use macros.
                    When starting an "endless" macro that do not stop their
                    execution automatically and the timeout value is > 0 then
                    the returned value is always FALSE.
                    In those cases you should select timeout=0 and  the
                    function does not wait for the finishing of the macro.
*****************************************************************************/
BOOL WINAPI StartMacro(int fgNr, int rpsNr, int timeout);


/*****************************************************************************
Functionname :      IsMacroActive
Description:        Validate if a macro is still being executed by the RPS 
                    or if it has already been terminated.
Parameters:
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
Return Value:        
    TRUE            if active
    FALSE           if not active
Notes:          
*****************************************************************************/
BOOL WINAPI IsMacroActive(int fgNr, int rpsNr);


/*****************************************************************************
Functionname :      StopMacro
Description:        A running macro may be interrupted by calling this
                    function.
Parameters:
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
Return Value:        
    TRUE            if the macro is interrupted 
    FALSE           if there was an error
Notes:              The value returned is also TRUE when no macro has been 
                    executed.
*****************************************************************************/
BOOL WINAPI StopMacro(int fgNr, int rpsNr);


/*****************************************************************************
Functionname :      ExecuteMacro
Description:        Loads a macro file into the RPS memory and subsequently
                    triggers the start of the macro's sequence of commands.
Parameters:
    macroName       Name of the  macro file
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    timeout         Timeout value in seconds
    useRegs         TRUE - enable usage of interface card register (default)   
                    FALSE - disable usage of interface card register
  Return Value:        
    TRUE            if the macro was executed
    FALSE           if there was an error
Notes:              This function is the internal summary of the functions
                    LoadMacro and StartMacro.
                    Set useRegs to FALSE, if the hardware of the interface card
                    isn't initialized
*****************************************************************************/
BOOL WINAPI ExecuteMacro(char *macroName, int fgNr,int rpsNr,int timeout,
                                                    BOOL useRegs);


//////////////////////////////////////////////////////////////////////////////
////////////////////// Functions for image scanning  /////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*****************************************************************************
Functionname :      LoadPicture
Description:        Starts the image reception for scanning an image
                    The scanned image is stored in the static DMA buffer
                    storage of the respective interface card
Parameters:
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    timeout         Timeout value in seconds
Return Value:        
    TRUE            if scanning was successful
    FALSE           if there was an error or timeout
Notes:              Using the interface card ID this function internally 
                    determines the macro file required for scanning and 
                    subsequently triggers its execution.
                    The pixel format used depends on the type of 
                    interface card used.
                    For special interface card the function SetFrameSize
                    can be used to set a user defined window area for
                    the scanned image.
*****************************************************************************/
BOOL WINAPI LoadPicture(int fgNr, int rpsNr, int timeout);



/*****************************************************************************
Functionname :      LoadPictureToVM
Description:        Starts the image reception for scanning an image
                    The scanned image is stored in the virtual buffer storage of
                    the respective interface card
Parameters:
    pVMem;          pointer to a global locked virtual memory buffer
    memLen          size of buffer to manage (max. 32 MB)   
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    timeout         Timeout value in seconds
Return Value:        
    TRUE            if scanning was successful
    FALSE           if there was an error or timeout
Notes:              For interface cards that are able to define
                    window areas for the scanning process the values set via
                    the SetFrameSize function are automatically adopted.
                    The function GetLastScanSize can be used for the 
                    determination of the pixel format.
                    Using the interface card ID this function internally
                    determines the macro file required for scanning and
                    subsequently triggers its execution.

                    The following Interface cards are supported:
                    0x3104,,0x3306, 0x3313,0x4180

*****************************************************************************/
BOOL WINAPI LoadPictureToVM(void  *pVMem, DWORD memLen,
                                            int fgNr, int rpsNr, int timeout);


/*****************************************************************************
Functionname :      LoadReference
Description:        This special function starts the image reception for
                    scanning a reference image for shading correction or
                    testing purposes
                    The scanned image is stored in the static DMA buffer
                    storage of the respective interface card
Parameters:
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    timeout         Timeout value in seconds
Return Value:        
    TRUE            If scanning was successful
    FALSE           If there was an error or timeout
Notes:              Using the interface card ID this function internally 
                    determines the macro file required for scanning
                    This function doesn't support all interface card ID's
                    There is no waiting for external events

                    The following Interface cards are supported:
                    0x3303,0x3313,0x0304,0x0305,0x0306,0x3306,0x3307,0x3308,
                    0x3313,0x3316,0x3326,0x4180,0x4187,0x4188
*****************************************************************************/
BOOL WINAPI LoadReference(int fgNr, int rpsNr, int timeout); 




/*****************************************************************************
Functionname :      LoadReferenceToVM
Description:        This special function starts the image reception for
                    scanning a reference image for shading correction or
                    testing purposes
                    The scanned image is stored in the virtual DMA buffer
                    storage of the respective interface card
Parameters:
    pVMem;          pointer to a global locked virtual memory buffer
    memLen          size of buffer to manage (max. 32 MB)   
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    timeout         Timeout value in seconds
Return Value:        
    TRUE            If scanning was successful
    FALSE           If there was an error or timeout
Notes:              Using the interface card ID this function internally 
                    determines the macro file required for scanning
                    This function doesn't support all interface card ID's
                    There is no waiting for external events

                    The following Interface cards are supported:
                    0x3313,0x4180
                    
*****************************************************************************/
BOOL WINAPI LoadReferenceToVM(void  *pVMem, DWORD memLen, int fgNr,
                                                    int rpsNr, int timeout); 



/*****************************************************************************
Functionname :      StartLoopMode
Description:        Starts an endless macro in the RPS rpsNr.
                    Thus, a continuous DMA transfer is started according to 
                    the parameters delivered.
Parameters:
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    timeout         Timeout value in seconds
Return Value:        
    TRUE            if the endless macro is activated
    FALSE           if there was an error
Notes:              For interface cards that are able to define
                    window areas for the scanning process the values set via
                    the SetFrameSize function are automatically adopted.
                    The function GetLastScanSize can be used for the 
                    determination of the pixel format.
                    Using the interface card ID this function internally
                    determines the macro file required for scanning and
                    subsequently triggers its execution.

                    The following Interface cards are supported:
                    0x0101,0x0102,0x0103,0x0104,0x0105,0x0106,0x0107,0x0108,
                    0x0111,0x0112,0x0113,0x0303,0x0313,0x0304,0x0305,0x0306,
                    0x0308,0x0117,0x3101,0x3104,0x3105,0x3108,0x3109,0x3112,
                    0x3306,0x3307,0x3308,0x3313,0x3316,0x3326,0x3409,0x4105,
                    0x4180,0x4187,0x4188

*****************************************************************************/
BOOL WINAPI StartLoopMode(int fgNr, int rpsNr, int timeout);


/*****************************************************************************
Functionname :      StartLoopModeToVM
Description:        Starts an endless macro in the RPS rpsNr.
                    Thus, a continuous DMA transfer to a virtual memory buffer
                    is started according to the parameters delivered.
Parameters:
    pVMem;          pointer to a global locked virtual memory buffer
    memLen          size of buffer to manage (max. 32 MB)   
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    timeout         Timeout value in seconds
Return Value:        
    TRUE            if the endless macro is activated
    FALSE           if there was an error
Notes:              For interface cards that are able to define
                    window areas for the scanning process the values set via
                    the SetFrameSize function are automatically adopted.
                    The function GetLastScanSize can be used for the 
                    determination of the pixel format.
                    Using the interface card ID this function internally
                    determines the macro file required for scanning and
                    subsequently triggers its execution.

                    The following Interface cards are supported:
                    0x3104,0x3306,0x3313,0x4180

*****************************************************************************/
BOOL WINAPI StartLoopModeToVM(void  *pVMem, DWORD memLen,
                                            int fgNr, int rpsNr, int timeout);


/*****************************************************************************
Functionname :      GetLastScanSize
Description:        Determine the size and the format of the last image
                    scanned
Parameters:
    fgNr            Number of the interface card
    frameWidth      Pointer to a variable for receiving the frameWidth value
    frameHeight     Pointer to a variable for receiving the frameHeight value
    channelsPerPix  Pointer to a variable for receiving the channel number 
    channelFormat   Pointer to a variable for receiving the channel format
                    value
Return Value:        
    TRUE            If values are valid
    FALSE           If there was an error
Notes:              The use of this function without prior loading of a scan
                    returns an undefined frameHeight value.
                    If the returned value in frameHeight 
                    is > LINE_COUNTER_SAFE_MAX, GetErrorNumber returns 
                    ERR_LINE_COUNTER_UNSAFE.
                    This is because of a bug in the SAA7146A circuit
                    For the application this means, that only the first
                    12 Bit of the frameHeight value are guaranteed                    
*****************************************************************************/
BOOL WINAPI GetLastScanSize(int fgNr, DWORD *frameWidth,
            DWORD *frameHeight, DWORD *channelsPerPix, DWORD *channelFormat);


/*****************************************************************************
Functionname :      GetCurrentScanLine
Description:        Determines the current line number of a running scan in
                    the DMA memory.
Parameters:
    fgNr            Number of the interface card
Return Value:        
    line number     only if the scan is running
Notes:              If there is no running DMA transfer the returned value 
                    is 0.
                    If the returned line number is > LINE_COUNTER_SAFE_MAX,
                    GetErrorNumber returns ERR_LINE_COUNTER_UNSAFE.
                    This is because of a bug in the SAA7146A circuit
                    For the application this means, that only the first
                    12 Bit of the frameHeight value are guaranteed                    
*****************************************************************************/
DWORD WINAPI GetCurrentScanLine(int fgNr);


/*****************************************************************************
Functionname :      GetPictureCounter
Description:        Returns the number of scans since the start of a scan
                    macro.
Parameters:
    fgNr            Number of the interface card
Return Value:        
    counter         After a normal scan the picture counter is always 1.
Notes:              This function is useful if endless macros for image
                    scanning are used.    
*****************************************************************************/
DWORD WINAPI GetPictureCounter(int fgNr);


/*****************************************************************************
Functionname :      GetBlockCounter
Description:        This function returns the number of block transfers 
                    since the start of a scan macro.
Parameters:
    fgNr            Number of the interface card
Return Value:        
    block counter   > 0 if the number scan lines is greater or equal than the
                    block height
Notes:              For special interface card the function SetFrameSize
                    can be used to set a block height for image scanning.
                    This function may be useful if endless macros for image
                    scanning or large images with small block size are used. 
*****************************************************************************/
DWORD WINAPI GetBlockCounter(int fgNr);


/*****************************************************************************
Functionname :      IsOverflow
Description:        This function checks the state of the FIFO overflow bits 
                    
Parameters:
    fgNr            Number of the interface card
    reset           If TRUE and the return value is > 0, the overflow bits are
                    set to zero 
                    FALSE (default)

Return Value:        
    0               No overflow occurs
    1               Video overflow
    2               DMA overflow
    3               Video and DMA overflow
                    
Notes:              Normally this function should return 0.
                    Use this function only for testing purposes.
                    The function returns a value > 0, if there are problems
                    in data transfer over the PCI bus.
*****************************************************************************/
int WINAPI IsOverflow(int fgNr, BOOL reset);




//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////////////////////////  specific API functions  ////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*****************************************************************************
Functionname :      SetDmaSize
Description:        This function changes the current DMA size of an interface
                    card
                    
Parameters:
    fgNr            Number of the interface card
    newDmaSize      New DMA size
Return Value:        
    TRUE            The setting of the new size was successful  
    FALSE           There was an error
Notes:              This function only works with Windows NT 4.0 and with two
                    interface cards 
                    The minimum of the new DMA size is 0.5 MB 
                    The other cards automatically get the rest of the 
                    available DMA memory
                    Is is recommended that the size of a DMA memory is
                    always a multiple of 1 MB
                    Other sizes may work, but are not guaranteed  
*****************************************************************************/
BOOL WINAPI SetDmaSize(int fgNr, DWORD newDmaSize);




//////////////////////////////////////////////////////////////////////////////
///////////// Functions for  hardware-supported shading correction ///////////
////////////////  only for interface cards with ID 0x03xx  ///////////////////
//////////////////////////////////////////////////////////////////////////////
    
/*****************************************************************************
Functionname :      ReadShadingTab
Description:        Reads out the current shading table 
Parameters:
    hg              Pointer to the storage space for saving the shading table
    tsize           Size of storage space
    table           Write BLACK_TABLE or WHITE_TABLE to interface card
                    hardware
    source          Select the data source that should be activated after 
                    reading the shading table
    timeout         Timeout value in seconds
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
 Return Value:          
    > 0             if the shading table was read successfully
    <= 0            if there was an error
Notes:              It has to be ensured that enough storage space has been 
                    reserved for the table data

                    The following Interface cards are supported:
                    0x0303,0x0313,0x0304,0x0305,0x0306,0x0308,0x3306,0x3307,
                    0x3308,0x3313,0x3316,0x3326,0x4180,0x4187,0x4188

*****************************************************************************/
int WINAPI ReadShadingTab(DWORD *hg, DWORD tsize, int table, int source,
                                            int timeout, int fgNr, int rpsNr);


/*****************************************************************************
Functionname :      WriteShadingTab
Description:        Writes a shading table to the indicated interface card
Parameters:
    hg              Pointer to the storage space with the shading table data
    tsize           Size of storage space
    table           Write BLACK_TABLE or WHITE_TABLE to interface card
                    hardware
    source          Select the data source that has to be activated after 
                    writing the shading table
    timeout         Timeout value in seconds
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    bCheck          TRUE forces a read after writing the shading table
                    If the loaded shading table is not identical to the
                    written shading table, the function returns with error
                    FALSE is the default value
Return Value:        
    > 0             if the shading table was written successfully
    <= 0            if there was an error
Notes:              It has to be ensured that enough storage space has been 
                    reserved for the table data

                    The following interface cards are supported:
                    0x0303,0x0313,0x0304,0x0305,0x0306,0x0308,0x3306,0x3307,
                    0x3308,0x3313,0x3316,0x3326,0x4180,0x4187,0x4188

*****************************************************************************/
int WINAPI WriteShadingTab( DWORD *hg, DWORD tsize, int table,
                int source, int timeout, int fgNr, int rpsNr, BOOL bCheck);



//////////////////////////////////////////////////////////////////////////////
//////////////// Functions for hardware-supported label table ////////////////
/////////////////  only for interface card with ID 0xX306, 0xX326 ////////////
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
Functionname :      ReadLabelTab
Description:        Reads out the current label table 
Parameters:
    lData           Pointer to the storage space for saving the label table
    tsize           Size of storage space
    source          Select the data source that has to be activated after 
                    reading the lable table
    timeout         Timeout value in seconds
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
 Return Value:          
    > 0             if the label table was read successfully
    <= 0            if there was an error
Notes:              It has to be ensured that enough storage space has been 
                    reserved for the table data

                    The following interface cards are supported:
                    0x0306,0x3306,0x3316,0x3326

*****************************************************************************/
int WINAPI ReadLabelTab(DWORD *lData, DWORD tsize, int source,
                                           int timeout, int fgNr, int rpsNr);

/*****************************************************************************
Functionname :      WriteLabelTab
Description:        Writes a label table to the indicated interface card
Parameters:
    lData           Pointer to the storage space with the label table data
    tsize           Size of storage space
    source          Select the data source that has to be activated after 
                    writing the lable table
    timeout         Timeout value in seconds
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    bCheck          TRUE forces a read after writing the lable table
                    If the loaded label table is not identical to the
                    written label table, the function returns with error
                     
                    
Return Value:        
    > 0             if the label table was written successfully
    <= 0            if there was an error
Notes:              It has to be ensured that enough storage space has been 
                    reserved for the table data

                    The following interface cards are supported:
                    0x0306,0x3306,0x3316,0x3326

*****************************************************************************/
int WINAPI  WriteLabelTab( DWORD *lData , DWORD tsize, int source, 
                                int timeout, int fgNr, int rpsNr, BOOL bCheck);



//////////////////////////////////////////////////////////////////////////////
///////// Functions for the definition of special image formats //////////////
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
Functionname :      GetFrameSize
Description:        Reading out the current values of the RAM register
                    structure.
Parameters:
    pFrame          Pointer to a FRAME_STRUCT variable
    fgNr            Number of the interface card
Return Value:        
    TRUE            if values are valid
    FALSE           if there was an error
Notes:              The function should only be executed if there is no
                    running scan.

                    The following interface cards are supported:
                    0x0104,0x0108,0x0304,0x0305,0x3104,0x3108,0x4180

*****************************************************************************/
BOOL WINAPI GetFrameSize(PFRAME_STRUCT pFrame, int fgNr);


/*****************************************************************************
Functionname :      SetFrameSize
Description:        Initializes the register of the indicated interface card 
                    with the current values of the FRAME_STRUCT variable
Parameters:
    pFrame          Pointer to a FRAME_STRUCT variable
    fgNr            Number of the interface card
Return Value:        
    TRUE            if values are valid
    FALSE           if there was an error
Notes:              There is no test on whether the transferred values make
                    sense.
                    The function should only be executed if there is no
                    running scan.
                    The following interface cards are supported:
                    0x0104,0x0108,0x0304,0x0305,0x3104,0x3108,0x4180,0x4187
                    0x4188
*****************************************************************************/
BOOL WINAPI SetFrameSize(PFRAME_STRUCT pFrame, int fgNr);


//////////////////////////////////////////////////////////////////////////////
///// Functions for image reception with up to MAX_BUF_ANZ virtual memory ////
//////// buffers only for interface cards with ID 0x3307 and 0x3105 //////////
//////////////////////////////////////////////////////////////////////////////


/*****************************************************************************
Functionname :      LoadPictureEx
Description:        Starts the image reception  for scanning images in
                    single mode or loop mode
                    The images are stored to the virtual memory addresses
                    given by the 'buffers'- data structure
Parameters:
    buffers         Data structure with a number of virtual memory blocks
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
    timeout         Timeout value in seconds
Return Value:        
    TRUE            If image reception or starting of loop mode was successful
    FALSE           If there was an error
Notes:              The data transfer directly goes to the allocated virtual 
                    memory blocks
                    For initialization of loop mode  LOOP_BUF_ANZ virtual
                    memory buffers are necessary.
                    This function is working  only under Windows NT and needs
                    an installed "mapcon.sys" driver
                    Valid buffer numbers are 1,2 and >= 4 ...MAX_BUF_ANZ-1

                    The following interface cards are supported:
                    0x0117,0x3105,0x3307,0x3308,0x4101,0x4105,0x4130,0x4230
                    0x4187,0x4188

*****************************************************************************/
BOOL WINAPI LoadPictureEx(PVMEM buffers, int fgNr, int rpsNr,
                                                            int timeout);


/*****************************************************************************
Functionname :      LoopControl
Description:        Controls the usage of memory buffers for image reception
                    in loop mode
Parameters:
    buffers         Data structure with virtual memory blocks and status
                    information
    freeBuf         Buffer number that should be freed for next 
                    image receptions  
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
Return Value:        
    TRUE            If action was successful
    FALSE           If there was an error
Notes:              The function needs a valid buffers structure with the
                    previously allocated virtual memory blocks.
                    freeBuf is only valid, if status is BUF_FILLED 

                    This function is working  only under Windows NT and needs
                    an installed "mapcon.sys" driver

                    The following interface cards are supported:
                    0x0117,0x3105,0x3307,0x3308,0x4101,0x4105,0x4130,0x4230
                    0x4187,0x4188

*****************************************************************************/
BOOL WINAPI LoopControl(PVMEM buffers, int freeBufNr, int fgNr,
                                                                   int rpsNr);


/*****************************************************************************
Functionname :      GetActiveLoopBuffer
Description:        Watching the current memory buffer for image reception
                    in loop mode
Parameters:
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
Return Value:        
 0...MAX_BUF_ANZ-1  Current loop buffer for image reception
    < 0             If there was an error
Notes:              This function is working  only under Windows NT and needs 
                    an installed "mapcon.sys" driver

                    The following interface cards are supported:
                    0x0117,0x3105,0x3307,0x3308,0x4101,0x4105,0x4130,0x4230
                    0x4187,0x4188

*****************************************************************************/
int WINAPI GetActiveLoopBuffer(int fgNr, int rpsNr);



//////////////////////////////////////////////////////////////////////////////
////////////////// Function for reading special data /////////////////////////
//////////////////////////////////////////////////////////////////////////////
    
/*****************************************************************************
Functionname :      ReadSpecialData
Description:        Reads out special image data 
Parameters:
    pBuffer         Pointer to the storage space for saving the data
    bufSize         Size of storage space
    timeout         Timeout value in seconds
    fgNr            Number of the interface card
    rpsNr           Number of the RPS (0 or 1) for macro execution
 Return Value:          
    TRUE            If action was successful
    FALSE           If there was an error
Notes:              It has to be ensured that enough storage space has been 
                    reserved for the data

                    The following interface cards are supported:
                    ID 0x3306,0x3316,0x3326 

*****************************************************************************/

BOOL WINAPI ReadSpecialData(DWORD *pBuffer, DWORD bufSize,
                                       int timeout, int fgNr, int rpsNr);



//////////////////////////////////////////////////////////////////////////////
/////// Function for access to the DEBI Port of the interface cards  /////////
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
Functionname :      ReadDebiPort
Description:        Function for accessing the DEBI-Port of the
                    7146 PCI controller
Parameters:
    fgNr            Number of the interface card
    dwValue         Pointer for receiving BYTE, WORD or DWORD
    numBytes        Number of Bytes (1...4) to receive 
    adr             Port address
    timeout         Timeout in ms
Return Value:        
    !=NULL          If the function was successful
    NULL            If there was an error
Notes:              The effect of this function depends on the used
                    image capturing system

*****************************************************************************/
BOOL WINAPI ReadDebiPort(int fgNr, DWORD *dwValue, DWORD nBytes, WORD adr,
                                                                DWORD timeout);

/*****************************************************************************
Functionname :      WriteDebiPort
Description:        Function for accessing the DEBI-Port of the 
                    7146 PCI controller
Parameters:
    fgNr            Number of the interface card
    dwValue         BYTE, WORD or DWORD data to send
    numBytes        Number of valid Bytes (1...4)
    adr             Port address
    timeout         Timeout in ms
Return Value:        
    !=NULL          If the function was successful
    NULL            If there was an error
Notes:              The effect of this function depends on the used
                    image capturing system
*****************************************************************************/
BOOL WINAPI WriteDebiPort(int fgNr, DWORD dwValue, DWORD nBytes, WORD adr,
                                                                DWORD timeout);

/*****************************************************************************
Functionname :      SendStringToDebiPort
Description:        Function for sending strings to the DEBI-Port of a 
                    Baumer interface card
Parameters:
    fgNr            Number of the interface card
    numBytes        Number of Bytes to send
    debiData        Pointer to a data buffer for sending
    adr             Port address
    bBlockTrans     If TRUE, sending of Data is performed in
                    a special block transfer mode 
    timeout         Timeout in ms
Return Value:        
    !=NULL          If the function was successful
    NULL            If there was an error
Notes:              
                    Ensure right allocation of the data buffer
                    The size is at least one DWORD
                    The return pointer should only be used for reading

                    The following interface cards are supported:
                    ID 0x4101,0x4130,0x4230,0x4180,0x4105,0x4187,0x4188

*****************************************************************************/
BYTE *WINAPI SendStringToDebiPort(int fgNr, DWORD nBytes,
                BYTE *debiData, WORD adr, BOOL bBlockTrans, DWORD timeout);


/*****************************************************************************
Functionname :      ReceiveStringFromDebiPort
Description:        Function for receiving strings from the  DEBI-Port of a 
                    Baumer interface card
Parameters:
    fgNr            Number of the interface card
    numBytes        Number of Bytes to send or receive
    debiData        Pointer to a DWORD aligned data buffer for receiving
    adr             Port address
    bBlockTrans     if TRUE, sending and receiving of Data is performed in
                    a special block transfer mode for the Baumer camera
                    interface
    timeout         Timeout in ms
Return Value:        
    !=NULL          If the function was successful
    NULL            If there was an error
Notes:              
                    Ensure right allocation of the data buffer
                    The size is at least one DWORD
                    The return pointer should only be used for reading

                    The following interface cards are supported:
                    ID 0x4101,0x4130,0x4230,0x4180,0x4105,0x4187,0x4188

*****************************************************************************/
BYTE *WINAPI ReceiveStringFromDebiPort(int fgNr, DWORD nBytes,
                BYTE *debiData, WORD adr, BOOL bBlockTrans, DWORD timeout);


/*****************************************************************************
Functionname :      ReadDebiRegister
Description:        Function for reading a 16 bit register from a 
                    Baumer Camera via DEBI-Port
Parameters:
    fgNr            Number of the interface card
    adr             Port address
    regNr           Number of the register
    regVal          WORD pointer for receiving the data
    timeout         Timeout in ms
Return Value:        
    TRUE            If the function was successful
    FALSE           If there was an error
Notes:              The following interface cards are supported:
                    ID 0x4101,0x4130,0x4230,0x4180,0x4105,0x4187,0x4188

*****************************************************************************/
BOOL WINAPI ReadDebiRegister(int fgNr, WORD adr, int regNr, WORD *regVal,
                                                            DWORD timeout);


/*****************************************************************************
Functionname :      WriteDebiRegister
Description:        Function for writing a 16 bit register from a
                    Baumer Camera via DEBI-Port
Parameters:
    fgNr            Number of the interface card
    adr             Port address
    regNr           Number of the register
    regVal          WORD value for receiving the data
    timeout         Timeout in ms
Return Value:        
    TRUE            If the function was successful
    FALSE           If there was an error
Notes:              The following interface cards are supported:
                    ID 0x4101,0x4130,0x4230,0x4180,0x4105,0x4187,0x4188

*****************************************************************************/
BOOL WINAPI WriteDebiRegister(int fgNr, WORD adr, int regNr, WORD regVal,
                                                            DWORD timeout);


//////////////////////////////////////////////////////////////////////////////
// Special function for access to serial Port of the interface cards 
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
Functionname :      ByteToSerialPort
Description:        Sends a byte value to the serial port of the corresponding
                    interface card
Parameters:
    fgNr            Number of the interface card
    adr             Address of the port
    value           Byte value to sent

Return Value:        
    TRUE            If the function was successful
    FALSE           If there was an error
Notes:              There is no test on whether the transferred parameters
                    make sense.
                    Byte serialization to address 0x0e for the following ID's:     
                    0x3307, 0x3308, 0x0105, 0x3112                                 
                    Sending string commands for the following ID's:
                    0x4187, 0x4188  
*****************************************************************************/
BOOL WINAPI ByteToSerialPort(int fgNr, WORD adr, BYTE value);


// include private functios 
#ifdef _PRIVATE_FUNCTIONS
#include "private.h"
#include "7146apiExt.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// some needed private functions (not documented) 
//////////////////////////////////////////////////////////////////////////////


// write DWORD to memory of an interface card
void  WINAPI WriteMemoryDWord(int fgNr, WORD offset, DWORD val);

// read DWORD from memory of an interface card
DWORD WINAPI ReadMemoryDWord(int fgNr, WORD offset);

// return line counter from last scan
DWORD  WINAPI GetLastScanLine(int fgNr);


#ifdef __cplusplus
}
#endif


#endif //_7146_API_H_


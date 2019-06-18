/*! \file Mexapi.h
 Mex Application Programming Interface (<b> MexAPI </b>) for <b> ProgResC10Plus and ProgResC12Plus. </b>
 Declaration of <b> MexAPI functions </b> usable by <b> CxxPlus.dll </b>.
 The <b> MexAPI </b> is delivered with a CxxWin.zip file containing compressed C/C++ source code
 and project files for CxxWin.exe. CxxWin.exe demonstrates how to use <b> MexAPI. </b>  
 The CxxPlus.dll has been created using Microsoft Visual C++ Version 6.0 (service pack 5, April 2001).
 For the C Runtime Library, option multithreaded has been selected. This DLL is specifically adapted 
 to allow its functions to be accessed from several threads.\n
 <b> This description covers applications that are to be  programmed for Windows-based PCs (Windows 2000 and above). </b>

\author Wolfgang Radecke Jenoptik, Laser Optik, Systeme GmbH \n

\version 2.5  \link page18  <b> Attention: </b> Type of colorchannels  changed. \endlink , \link page14 Minor changes for mexImageParams. \endlink
\date 2004/08/31 Primary creation.
\date 2004/10/20 First completion.

\date 2004/11/25
- There is a <b> new firmware for ProgResC10Plus and ProgResC12Plus </b> included as resource in CxxPlus.dll.
  The new firmware solves a problem with Windows XP servicepack 2 and some FireWire host adapters.
  The problem is that some FireWire host adapters does not correct support their duties as isochronous
  resource manager for FireWire data transfer. As a consequence follows that image transfer will fail.
  The new firmware contains a work around to fix the problem. To update the firmware please use the
  application CxxWin.exe and select in the menu <b> Firmeware/Update firmware with respect to version </b> .
  If - in some seldom cases - the update failed, try it again. <b> It is recommended for the update, that exactly
  one and only one camera is physical connected to the PC. </b>
- New function ::mexGetVersion added.

\date 2005/02/09
- Implementation of ::mexExit corrected, some calls to finish the session added. Previously some threads leave open.
  This was an reason for a crash on termination of a session.

\date 2005/03/16 
  - Implementation for <b> ProgResC14Plus </b> added.
  - Name decoration for ::mexCloseImageTransfer removed.

\date 2005/04/06
  - Type of mexImageParams::bcorrectColors changed. The 4-byte member bcorrectColors has been replaced by
    4 unsigned char members bcorrectColors, ciCode, ccdtransferCode, notifymask. The new bcorrectColors takes the
    original semantics  <b> See: </b> \ref page14.
\date 2005/05/02 
  - Implementation for monochrome camera <b> ProgResM14Plus </b> added.
  - Function 	::mexGetSerialNumberString added.
  - Function  :: \n added.
  - Structur CamType added.
\date 2005/05/26
  - Function ::mexActivateExposureControl added.
  - Function ::mexGetOptimalExposureTime added.
  - Function ::mexActivateGammaProcessing added.
  - Function ::mexSetFocusCallback added.
\date 2005/06/08
  - Function ::mexEnableExposureReports added.
\date 2005/06/14
- Behavior of ::mexGetSerialNumberString has been changed from version 1.9 to version 2.0 for the benefit of the use.

\date 2005/06/29 
- Function ::mexActivateSaturationControl added.

\date 2005/0707
- Color matrix operation accelerated. Image acquisitions is affected if mexImageParams::bcorrectColors ==1 or
  mexSaturationCtrl::bActive ==TRUE
- The <b> Pink Bug </b> fixed for ProgresC14Plus . <b> Pink Bug </b> stands for: Binned overexposed imageparts were pink instead of white. 

\date 2005/07/15
- Function ::mexModifyLive added.
- New value <b> 2 </b> for ::mexImageParams::bcorrectColors introduced. The new value activates a very fast 
  color matrix operation compared to the previous value <b> 1 </b> .
- New Value <b> 2  </b> for mexImageParams::ciCode introduced. The new value activates a very fast color interpolation 
  procedure compared to the previous value <b> 0 </b>
- The last two changes devote to <b> acceleration of live images </b>  to <b> the disadvantage of color quality </b>.

\date 2005/07/18
- A bug concerning ::mexExit fixed: If multiple cameras were open a call to mexExit could caused a crash. This bug was
  present since  2005/06/14 or later in connection with the implementation of ::dfcActivateExposureControl. 

\date 2005/07/19
- Functions	added
	- ::mexCalibrateColors 
	- ::mexSetColorMatrix
	- ::mexResetColorMatrix

\date 2005/08/19
- Semantic of ::mexSaturationCtrl::bActive modified, valid values are now 0,1,2 as for ::mexImageParams::bcorrectColors.
- In the same context a bug with respect to ::mexActivateSaturationControl fixed: Saturation control did not work
  for non-live modes if ::mexImageParams::bcorrectColors=0.
-  <b> The use of black reference and white referencedoes not work properly in case of mexImageParams::ccdtransferCode==2,3,4 </b> . That is so per design and it is not a bug.
   The implementation returns ::PARAMERR for these cases and does not work at all.

\date 2005/08/26
- Restriction removed for the use of black- and white references with respect to mexImageParams::ccdtransferCode=2,3,4 
  mentioned above (2005/08/19). The use of of black- and white references <b> is now possible and correct for
  mexImageParams::ccdtransferCode=0,1,2,3,4 </b>. Notice that this is essential for ProgResC14Plus and ProgResM14Plus.
  For ProgResC10Plus mexImageParams::ccdtransferCode is restricted to value 0. For ProgResC12Plus the values mexImageParams::ccdtransferCode=0,1 are allowed but the
  Restriction did not apply previously, because ProgResC12Plus does know only 16-bit image data transfer from ccd-sensor.

\date 2005/08/31
- A bug in camconj.dll fixed: ::mexCalibrateScanner did not work after using mexImageParams::ccdtransferCode=4

\date 2005/10/12
- A bug in camconj.dll fixed: ::mexCalibrateScanner did not work if ::mexImageParams::bcorrectColors>0 was specified.
- A bad side-effect fixed concerning simultaneous connected Kameras of type DFC500 and ProgResC14 :
  Trials to acquire multishot images cause an access violation.
- <b> Implementation of binning acquisition modes for the monochrom camera ProgResM14Plus modified </b>
  The new implementation now let the camera self realize the binning in both directions X and Y,
  while the previous implementation did only Y-binnig delegate to the camera and did in X-direction perform a subsampling.
  In turn CxxWin.exe adjusts  now the exposure time to the binning modes to reflect the new binning algorithm.
  For instance for a 5-fold binning 25 single pixels are accumulate. Hence the normal selected exposure time is divided by
  25 to achieve the same brightness. Before there was a division by 5 to achieve the same brightness.

\date 2005/10/14
  - Some corrections for <b> monochrome cameras of type ProgResM14Plus </b>:
		- Implementation of ::mexUpdateFirmware corrected to accept this type of cameras
		- Implementation of ::mexGetFirmwareVersionFromData corrected to accept this type of cameras.
		- Implementation of ::mexMSecToTicks corrected to accept this type of cameras.
		- Implementation of ::mexTicksToMSec corrected to accept this type of cameras.

\date 2005/10/19
- Support for \link page25 special camera models \endlink introduced.
- Acquisition mode ::mexc14_9Shot_colhalf for camera model <b> ProgRes CFScan </b> added.
- Interpretation of parameter <b> bActive </b> in function ::mexActivatePeltier modified.

\copydoc page18

\copydoc page1

\remarks
- The CxxPlus.dll is intended to use  \link page11 up to 4 cameras  in a working session. \endlink .   Hence almost all function have a 
global unique identifier for the wanted camera in the parameter list.

- The time interval from ::mexInit to ::mexExit is called <b> working session </b>.
Only in a working session the events on the FireWire bus will be reported (in code sample  via InitCB).
If as callback NULL was given, no reports about events on the FireWire bus comes in. 

- A working session with CxxPlus.dll is typically timely structured as follows :
  \code
  // C/C++ code fragment
    unsigned long Number=10;
    unsigned __int64 GUID_List[10];
  
    void __stdcall InitCB(  unsigned long status,
                           unsigned long UserValue,
                           unsigned __int64 CamGUID)
    {
      //... doing something with respect to unpluggin/ plugging of cameras
    }

    // Starts the working session with the CxxPlus.dll.
    long resinit=mexInit(InitCB,0); // no specific user value
  
    long res=-1;
    if(resinit==NOERR)
    {
      // Searching for avalable cameras
      res=mexFindCameras(&Number,GUID_List);
      if(res==NOERR && Number>0)
      {
        // Start a camera session.
        res=mexOpenCamera(GUID_List[0]); // open camera session (if wanted camera is identified by GUID_List[0]);
        if(res==NOERR)
        {
         // .... use other MexAPI functions
         // ....
         // ....GUID_List[0]
         // Finish a camera session.
         mexCloseCamera(GUID_List[0]);  // close the camera session with the specified camera
        }
      }
      // Finish the working session with CxxPlus.dll.
      mexExit();
    }
  \endcode
- The time interval from ::mexOpenCamera (camguid) to ::mexCloseCamera (camguid) ist called <b> camera session </b>.
A camera session is specific to an unique camera identified by the global unique identifier camguid.
Almost all functions of <b> MexAPI </b> are successfully callable <b> only in a camera session </b>. But some 
functions are ever successfully callable. See the description of the other functions.The up to 4 
camera sessions may be live in timely overlapping intervals.
- The <b> MexAPI </b> uses  in many cases the concept of <b> callback's (callback functions) </b>.
These are function with declared types in <a class="el" href="MeBase_8h.html">meBase.h</a>  but declared and implemented by the <b> caller </b> of the
MexAPI. The function pointer are passed at call of some MexAPI-Function and called by the implementation
of CxxPlus.dll.  Any such call is executed by CxxPlus-internal threads and in particular not from threads
of MexAPI-users. The implementation of callbacks should ensure short operation times for each call.
Any timely expensive task should be transferred to users own threads. Recalls from callbacks into
the MexAPI should be avoided. For seldom cases a recall is possible, but the MexAPI-user must test such cases
carefully.
- The callbacks have a so called <b> generic user value </b> in his parameter list. The intention of such a value is the 
identification of an object in users responsibility that processed the necessary actions if the callback comes in.
On any call of the callback the generic user value is unchanged propagated to the user. 
For instance this can be  a pointer to the processing object given from user at a call of an acquisition function.
Because the user knows the type of the object  he can recast the generic user value to the true C/C++ type and then
lets the object fulfill his work. <b> The implementation of CxxPlus.dll holds exactly one copy of the pair 
(::meInitCB,UserValue) in the working session.
- For the other callbacks the Implementation holds for each open camera exact one copy of the
  pairs (::mexImageFinalProc,UserValue) and (::mexImageProc,dwData)  </b>
- If a function returns ::NOTIMPLEMENTED it is not yet implemented.
- For all acquisition functions ::mexAcquisition, ::mexStartAcquisition, ::mexStartFastAcquisition and ::mexStartFastAcquisitionEx
   there is a important hint to the function ::mexCloseImageTransfer. This is important if in a working session multiple
   cameras are used. 
- All structure declaration are implemented by use of <b> #pragma pack(1) </b>. This ensures an equal alignment
  for CxxPlus.dll and for applications using the MexAPI.
- The same is not valid for structures declared in DfcBase.h because these have to be compatible
  with the previously known meapi.h.

*/
#ifndef _MEAPI
#define _MEAPI

#ifdef __cplusplus
 extern "C" {			/* allow use with C++ */
#endif

#ifndef CXXPLUS_ALTERNATIVE

#if defined CXXPLUS_EXPORTS
#define CXXPLUS_SPEC __declspec(dllexport)
#else
#define CXXPLUS_SPEC __declspec(dllimport)
#endif

#else

#define CXXPLUS_SPEC

#endif

#include "meBase.h"

//! Enumeration to use in ::mexFreeInternalMemory
//! in an OR combination.
typedef enum mexMemParts
{
  image_memory=1,   //!< Internal imagememory is to free.
  blackref_memory=2,//!< Internal memory for black reference is to free.
  whiteref_memory=4,//!< Internal memory for white reference is to free.
}mexMemParts;

//! Enumeration for acquisition mode. The <b> Shot1F_ </b> prefix stands for image acquisition from 1 half CCD-frame.
//! The <b> Shot2F_ </b> prefix stands for image acquisitio from 2 half CCD-frames: 1. half frame with  even 
//! numbered CCD-raws, 2. half frame with odd numbered CCD-raws. Because the 2 half frames are exposured
//! one after another moving image scenarios leads to mixed image scenes for Shot2F_ acquisition modes. 
//! Such images with moving scenes from distict half frames are bad,  but the software cannot do anything to
//! correct the scenes. Hence the user of the software is responsible for timely stable scenes.
//! To allow also image creation on timely more instable scenes the MexAPI gives the <b> Shot1F_ </b> acquisition modes
//! These modes reduces the required stabiltity of the scenes. In the opposite the <b> Shot2F_ </b> acquisition modes
//! leads to a higher pixel resolution on the scene because more sensor pixels are used.
//! Here is a blue sheet for camera types and supported acquisition modes:
//! <table border>
//! <tr bgcolor=#CCFFCC> <th> ::mexAcqMode </th> <th> Value </th> <th> ProgResC10Plus </th> <th>ProgResC12Plus </th> <th> ProgResC14Plus </th> <th> ProgResM14Plus </th> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexShot_Raw </td> <td align=right> 0  </td>          <td> <b>yes</b> 2080 x 1542 </td> <td> <b>yes</b> 2580 x 1944 </td>   <td> <b>yes</b> 1360 x 1024 </td> <td> <b>yes</b> 1360 x 1024 </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexShot1F_highspeed1 </td>    <td align=right> 1 </td>   <td> <b>yes</b> _346 x _256 </td> <td> <b>yes</b> _322 x _244 </td>   <td> <b>no</b> </td>  <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexShot1F_highspeed2 </td>    <td align=right> 2 </td>   <td> <b>yes</b> _692 x _512</td> <td>  <b>yes</b> _646 x _488 </td>   <td> <b>no</b> </td>  <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexShot2F_lowqualitycol </td> <td align=right> 3 </td> <td> <b>yes</b> 2080 x 1542</td> <td>  <b>yes</b> 2580 x 1944 </td>   <td> <b>no</b> </td>  <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexShot2F_highqualitycol </td><td align=right> 4 </td> <td> <b>yes</b> 2080 x 1542</td> <td>  <b>yes</b> 2580 x 1944 </td>   <td> <b>no</b> </td>  <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexShot2F_bin2col </td>       <td align=right> 5 </td> <td> <b>yes</b> 1040 x _770</td> <td>  <b>yes</b> 1920 x _972 </td>   <td> <b>no</b> </td>  <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexShot2F_bin3col </td>       <td align=right> 6 </td> <td> <b>yes</b> _692 x _514</td> <td>  <b>yes</b> _860 x _648 </td>   <td> <b>no</b> </td>  <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexShot2F_bin4col </td>       <td align=right> 7 </td> <td> <b>yes</b> _520 x _384</td> <td>  <b>yes</b> _644 x _468 </td>   <td> <b>no</b> </td>  <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexShot2F_bin5col </td>       <td align=right> 8 </td> <td> <b>yes</b> _416 x _308</td> <td>  <b>yes</b> _516 x _388 </td>   <td> <b>no</b> </td>  <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexShot1F_spec1 </td>         <td align=right> 9 </td> <td> <b>no</b> </td>             <td>  <b>yes</b> _346 x _256 </td>   <td> <b>no</b> </td>  <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexShot1F_spec2 </td>         <td align=right> 10 </td><td> <b>no</b> </td>             <td>  <b>yes</b> _692 x _512 </td>   <td> <b>no</b> </td>  <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexc14_Shot_colhighres</td>   <td align=right> 1 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>yes</b> 1360 x 1024 </td>    <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexc14_Shot_collowres</td>    <td align=right> 2 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>yes</b> _680 x _512 </td>    <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexc14_Shot_highspeed1</td>   <td align=right> 3 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>yes</b> _340 x _256 </td>    <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexc14_Shot_highspeed2</td>   <td align=right> 4 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>yes</b> _680 x _512 </td>    <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexc14_Shot_bin3col</td>      <td align=right> 5 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>yes</b> _453 x _340 </td>    <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexc14_Shot_bin5col</td>      <td align=right> 6 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>yes</b> _272 x _204 </td>    <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexc14_4Shot_col</td>         <td align=right> 7 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>yes</b> 1360 x 1024 </td>    <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexc14_9Shot_col</td>         <td align=right> 8 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>yes</b> 4080 x 3072 </td>    <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexc14_16Shot_col</td>        <td align=right> 9 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>yes</b> 2720 x 2048 </td>    <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexc14_36Shot_col</td>        <td align=right> 10 </td><td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>yes</b> 4080 x 3072 </td>    <td> <b>no</b> </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexm14_Shot_bwhighres</td>    <td align=right> 1 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>no </b> </td>  <td> <b>yes</b> 1360 x 1024  </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexm14_Shot_bwlowres</td>     <td align=right> 2 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>no </b> </td>  <td> <b>yes</b> _680 x _512  </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexm14_Shot_highspeed1</td>   <td align=right> 3 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>no </b> </td>  <td> <b>yes</b> _340 x _256  </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexm14_Shot_highspeed2</td>   <td align=right> 4 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>no </b> </td>  <td> <b>yes</b> _680 x _512  </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexm14_Shot_bin2bw</td>       <td align=right> 5 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>no </b> </td>  <td> <b>yes</b> _680 x _512  </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexm14_Shot_bin3bw</td>       <td align=right> 6 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>no </b> </td>  <td> <b>yes</b> _452 x _340  </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexm14_Shot_bin4bw</td>       <td align=right> 7 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>no </b> </td>  <td> <b>yes</b> _340 x _256  </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexm14_Shot_bin5bw</td>       <td align=right> 8 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>no </b> </td>  <td> <b>yes</b> _252 x _204  </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexm14_4Shot_bw</td>          <td align=right> 9 </td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>no </b> </td>  <td> <b>yes</b> 1360 x 1024  </td> </tr>
//! <tr bgcolor=#CCFFFF> <td> ::mexm14_9Shot_bw</td>          <td align=right> 10</td> <td> <b>no</b> </td>  <td> <b>no</b> </td>  <td> <b>no </b> </td>  <td> <b>yes</b> 4080 x 3072  </td> </tr>
//! </table>
//! \note
//! - An application can use ::mexIsAcquisitionModeSupported and ::mexGetImageInfo to discover at runtime the table above.
//! -  A specification of 2080 x 1542 means that the camera creates maximal 2080 x 1542 pixel for the given acquisition mode,
//!     where 2080 is the <b> pixel count </b> in a horizontal raw and 1542 is the <b> count of raws </b> in the image. 
//! - <b> ProgResC14Plus / ProgResM14Plus </b> are a so called <b> progressive scan camera.</b> Hence <b> no half frames </b> are created 
//!    but the whole sensor is read out in one step while an image aquisition. 
//! -  The enumeration uses different names for <b> ProgResC10Plus / ProgResC12Plus </b> on the one hand 
//! and <b> ProgResC14Plus / ProgResM14Plus </b> on the other hand,  but the range of values is shared for all types of cameras:
//!  <b> 0,1,2,3,4,5,6,7,8,9,10 </b> where the name ::mexShot_Raw=0 may be used by all types.
typedef enum mexAcqMode
{
  mexShot2F_Raw=0,		             //!< Raw data image (copy of sensor) (<b> all ProgRes...Plus cameras </b>).
  mexShot_Raw=0,                   //!< Raw data image (copy of sensor, the same value 0 as mexShot2F_Raw)
  mexShot1F_highspeed1=1,	         //!< Fast color image (<b> ProgResC10Plus and ProgResC12Plus </b>).
  mexShot1F_highspeed2=2,          //!< Fast color image (<b> ProgResC10Plus and ProgResC12Plus </b>).
  mexShot2F_lowqualitycol=3,	     //!< Low  quality interpolated color image(<b> ProgResC10Plus and ProgResC12Plus </b>).
  mexShot2F_highqualitycol=4,	     //!< High quality  interpolated color image (<b> ProgResC10Plus and ProgResC12Plus </b>).
  mexShot2F_bin2col=5,		         //!< 2-fold binning color image (<b> ProgResC10Plus and ProgResC12Plus </b>).
  mexShot2F_bin3col=6,		         //!< 3-fold binning color image (<b> ProgResC10Plus and ProgResC12Plus </b>).
  mexShot2F_bin4col=7,		         //!< 4-fold binning color image (<b> ProgResC10Plus and ProgResC12Plus </b>).
  mexShot2F_bin5col=8,		         //!< 5-fold binning color image (<b> ProgResC10Plus and ProgResC12Plus </b>).
  mexShot1F_spec1=9,               //!< Special interplolated color image (<b> ProgResC12Plus only </b>).
  mexShot1F_spec2=10,              //!< Special interplolated Color image (<b> ProgResC12Plus only </b>).
  
  mexc14_Shot_colhighres=1,     //!< Interpolated color image (<b> ProgResC14Plus only </b>).
  mexc14_Shot_collowres=2,      //!< Interpolated color image (<b> ProgResC14Plus only </b>).
  mexc14_Shot_highspeed1=3,     //!< special interpolated color image (<b> ProgResC14Plus only </b>).
  mexc14_Shot_highspeed2=4,     //!< special interpolated color image (<b> ProgResC14Plus only </b>).
  mexc14_Shot_bin3col=5,        //!< 3-fold binning color image (<b> ProgResC14Plus only </b>).
  mexc14_Shot_bin5col=6,        //!< 5-fold binning color image (<b> ProgResC14Plus only </b>).
  mexc14_4Shot_col=7,           //!<  4 Shot (2 x 2) piezo scan color image (<b> ProgResC14Plus only </b>).
  mexc14_9Shot_col=8,           //!<  9 Shot (3 x 3) piezo scan color image (<b> ProgResC14Plus only </b>).
  mexc14_16Shot_col=9,          //!< 16 Shot (4 x 4) piezo scan color image (<b> ProgResC14Plus only </b>).
  mexc14_36Shot_col=10,         //!< 36 Shot (6 x 6) piezo scan color image (<b> ProgResC14Plus only </b>).
  mexc14_9Shot_colhalf=11,      //!< 9 Shot (3 x 3) piezo scan color image half resolution (<b> ProgResCFscan only </b> )
  
  mexm14_Shot_bwhighres=1,      //!< black/white image (<b> ProgResM14Plus only </b>).
  mexm14_Shot_bwlowres=2,       //!< black/white image (<b> ProgResM14Plus only </b>).
  mexm14_Shot_highspeed1=3,     //!< fast black/white image (<b> ProgResM14Plus only </b>)
  mexm14_Shot_highspeed2=4,     //!< fast black/white image (<b> ProgResM14Plus only </b>)
  mexm14_Shot_bin2bw=5,         //!< 2-fold binning black/white image pixel (<b> ProgResM14Plus only </b>).
  mexm14_Shot_bin3bw=6,         //!< 3-fold binning black/white image (<b> ProgResM14Plus only </b>). 
  mexm14_Shot_bin4bw=7,         //!< 4-fold binning black/white image (<b> ProgResM14Plus only </b>).
  mexm14_Shot_bin5bw=8,         //!< 5-fold binning black/white image (<b> ProgResM14Plus only </b>).
  mexm14_4Shot_bw=9,            //!< 4 Shot (2 x 2) piezo scan black/white image (<b> ProgResM14Plus only </b>).
  mexm14_9Shot_bw=10,           //!< 9 Shot (3 x 3) piezo scan black/white image (<b> ProgResM14Plus only </b>).
  mexCount=12,                  //!< Total count of  available acquisition modes.
} mexAcqMode;			




#pragma pack(1)

//! Struct for image acquisition.
//! Any call of ::mexAcquisition, ::mexStartAcquisition, ::mexStartFastAcquisition, ::mexStartFastAcquisitionEx
//! takes a pointer to a ::mexImageParams object. The caller must provide a ::mexImageParams object with valid
//! settings for the members.
//! \attention
//! - Layout of mexImageParams has been changed since version 1.9. \n
//! \copydoc page14 
//!
//! - The member mexImageParams::rcimageBounds  refers to the <b> rect of interest </b> at the CCD-Sensor.
//! It <b> does not refers to the wanted image size </b> for an image acquisition. Only in case of some acquisiton modes
//! there is an <b> one-to-one correspondence </b> to the resulting image size. This is true for ::mexShot2F_Raw,
//! ::mexShot2F_lowqualitycol, ::mexShot2F_highqualitycol and ::mexShot1F_spec2, but also in these cases the values for the 
//! ::mexImageParams::rcimageBounds members must be <b> even </b> to fulfill the correspondence. In all other cases the resulting
//! image size must ask for by ::mexGetImageInfo. <b> Hence the safe way to know the resulting image size of an
//! acquisition request is, to ask for it by  ::mexGetImageInfo.</b>
//! The maximal rect of interest at CCD sensor is restricted by the size of the sensor. These dimensions
//! can ask for by ::mexGetCameraInfo as in the following code sample.
//! \code
//! // C/C++ source code fragment
//! // global array
//! void* colorchannels[3]={NULL,NULL,NULL};
//!
//! mexImageParams Par;
//! meCamInfo caminfo;
//! meImageInfo imginfo;
//! unsigned __int64 camguid=...
//! long res=mexGetCameraInfo(camguid,&caminfo);
//!  if(res==NOERR)
//!  {
//!   ::SetRect(&Par.rcimageBounds,0,0,caminfo.SensorX,caminfo.SensorY);
//!   Par.mode = mexShot2F_highqualitycol;
//!   Par.pImgFinalProc=ImgFinalProc;
//!   Par.exposureTicks=100;
//!   Par.UserValue=...
//!   //... other settings of Par
//!
//!   res=mexGetImageInfo(camguid,&Pars,&imginfo);
//!   if(res==NOERR)
//!   {
//!      unsigned long MemSizePerChannel=caminfo.SensorX*caminfo.SensorY*sizeof(short);
//!      for(int i=0; i<3; i++)
//!      {
//!        colorchannels[i]= new unsigned char[MemSizePerChannel];
//!      }
//!      res=mexStartAcquisition(camguid,&Pars,colorchannels,MemSizePerChannel);
//!     
//!     
//!     // anywhere at an other place in the source code the ImgFinalProc is called:
//!     // with parameter IMAGE_READY, and colorchannels. 
//!     // Resulting imagesize should be caminfo.SensorX x caminfo.SensorY
//!     // for instance                             2580 x            1944 for ProgResC12Plus.
//!   }
//!  }
//! 
//! \endcode
//!
//! \note 
//! - The use of black reference (useblackref=TRUE) and white reference ( usewhiteref=TRUE) 
//! is limited to the  <b> non binning </b> acquisition modes. More exactly the following restrictions apply:
//! <table border>
//! <tr bgcolor=#CCFFFF> <th> ::mexAcqMode </th> <th> black reference supported </th> <th> white reference supported</th> </tr>
//! <tr bgcolor=#FFCCFF> <td> ::mexShot2F_Raw </td> <td> NO </td> <td> NO </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexShot1F_highspeed1 </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexShot1F_highspeed2 </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexShot2F_lowqualitycol </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexShot2F_highqualitycol </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#FFCCFF> <td> ::mexShot2F_bin2col </td> <td> NO </td> <td> NO  </td> </tr>
//! <tr bgcolor=#FFCCFF> <td> ::mexShot2F_bin3col </td> <td> NO </td> <td> NO </td> </tr>
//! <tr bgcolor=#FFCCFF> <td> ::mexShot2F_bin4col </td> <td> NO </td> <td> NO </td> </tr>
//! <tr bgcolor=#FFCCFF> <td> ::mexShot2F_bin5col </td> <td> NO </td> <td> NO </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexShot1F_spec1 </td> <td> YES (ProgResC12Plus only) </td> <td> YES (ProgResC12Plus only) </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexShot1F_spec2 </td> <td> YES (ProgResC12Plus only) </td> <td> YES (ProgResC12Plus only) </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexc14_Shot_colhighres </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexc14_Shot_collowres </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexc14_Shot_highspeed1 </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexc14_Shot_highspeed2 </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#FFCCFF> <td> ::mexc14_Shot_bin3col </td> <td> NO </td> <td> NO </td> </tr>
//! <tr bgcolor=#FFCCFF> <td> ::mexc14_Shot_bin5col </td> <td> NO </td> <td> NO </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexc14_4Shot_col </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexc14_9Shot_col </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexc14_16Shot_col </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexc14_36Shot_col </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexm14_Shot_bwhighres </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexm14_Shot_bwlowres </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexm14_Shot_highspeed1 </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexm14_Shot_highspeed1 </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#FFCCFF> <td> ::mexm14_Shot_bin2bw </td> <td> NO </td> <td> NO </td> </tr>
//! <tr bgcolor=#FFCCFF> <td> ::mexm14_Shot_bin3bw </td> <td> NO </td> <td> NO </td> </tr>
//! <tr bgcolor=#FFCCFF> <td> ::mexm14_Shot_bin4bw </td> <td> NO </td> <td> NO </td> </tr>
//! <tr bgcolor=#FFCCFF> <td> ::mexm14_Shot_bin5bw </td> <td> NO </td> <td> NO </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexm14_4Shot_bw </td> <td> YES </td> <td> YES </td> </tr>
//! <tr bgcolor=#CCFFCC> <td> ::mexm14_9Shot_bw </td> <td> YES </td> <td> YES </td> </tr>
//! </table>
//! If an acquisition function is called with invalid ::mexImageParams::buseblackref or ::mexImageParams::busewhiteref 
//! the function returns the code ::PARAMERR . \n
//! - For the range of mexImageParams::bcorrectColors applies:
//!     - 0,1,2,3,4 for ProgResC14Plus as described.
//!     - 0,1 for ProgResC12Plus, where 1 means 16 bit data transfer from ccd-sensor with 18 MHz operating frequency but <b> without squrt-encoding/decoding </b>
//!     - 0 for ProgResC10Plus 
//! - The structure is compiled with <b> #pragma pack(1) . </b>
//! - \ref page13
typedef struct mexImageParams
{ 
  mexAcqMode mode;			          //!< Acquisition mode.
  RECT rcimageBounds;		          //!< Rect of interest with respect to sensor ( camera specific maximal rect  see also ::mexGetCameraInfo).
  mexImageFinalProc pImgFinalProc;//!< If image is ready Dll will call this callback, client of DLL can process color channels per own intention.
	unsigned long UserValue;        //!< UserValue for pImgFinalProc.
  unsigned long exposureTicks;    //!< Exposuretime [camera units] (see ::mexMSecToTicks). 
  BOOL buseblackref;		          //!< if TRUE perform dark correction with black reference.
  BOOL busewhiteref;		          //!< if TRUE perform  shading correction with white reference.
  BOOL bwhitebalance;		          //!< if TRUE perform white balance.
  unsigned char bcorrectColors;   //!< <b> 0 </b> : no color matrix operation  <b> 1 </b> : standard 3x3 colormatrix operation <b> 2 </b>: very fast color matrix operation. <b> See also: </b> \link page13a Options to accelerate image aquisition \endlink
  unsigned char ciCode;           //!< Code to select one of three algorithm for  color interpolation <b> 0 </b> for <b> fast low quality </b> , <b> 1 </b>  for <b> slow high quality </b> <b> 2 for very fast low quality </b> <b> See also: </b> \link page13a Options to accelerate image aquisition\endlink
  unsigned char ccdtransferCode;  //!< range 0,1,2,3,4:  <b> 0 </b> - 16 bits transfer, <b> 1 </b> - 8 bits squrt transfer, <b> 2 </b> -  8 low bits transfer, <b> 3 </b> 8 high bits transfer <b> 4 </b> 8 high bits 8 bits result \link page17  see also:Resulting image format. \endlink.
  unsigned char notifymask;       //!< Usable to request notifications from  image acqusition (only for ProgResC14Plus and ProgResM14Plus \ref page20)
  mexImageProc pImageproc;	      //!< Progress callback routine, may be NULL.
  DWORD dwData;						        //!< User Data for pImageproc. 
} mexImageParams;

//! Structure for positions of the piezo scanner.
//! Structure to control the piezo scanner of ProgResC14Plus and ProgResM14Plus.
//! \note 
//! - The structure serves only to get and set the scanner position for 
//! control of the camera if a multi shot image is ordered. The apllication developer
//! <b> should not set </b> explicitely the members of any structure instance. Instead of that use
//! the functions ::mexGetScannerPositions, ::mexSetScannerPositions and ::mexCalibrateScanner.  
//! - The structure is compiled with <b> #pragma pack(1) . </b>
typedef struct mexScannerPositions 						
{
  unsigned short ident;					  //!< Identifier 0x5213
  unsigned short checksumm;				//!< Sum of bytes form flag4 to the end of struktur.
  unsigned char	flag4;						//!< 0 not valid, else scan direction
  unsigned char	pos4[4][2];
  unsigned char	flag9;						//!< 0 not valid, else scan direction
  unsigned char	pos9[9][2];
  unsigned char	flag16;						//!< 0 not valid, else scan direction
  unsigned char	pos16[16][2];
  unsigned char	flag25;						//!< 0 not valid, else scan direction
  unsigned char	pos25[25][2];
  unsigned char	flag36;						//!< 0 not valid, else scan direction
  unsigned char	pos36[36][2];
  unsigned char	reserve[67];
}mexScannerPositions;


//! \struct mexPiezoCalibSet
//! Structure to set the properties for the calibration of the piezo scanner.
//! - Acquisition mode and exposure time comes from mexImageParams
//! - The calibration procedure works in a loop of steps from 1 to the IterCount.
//! - In the first step position specified by StartSource are used
//! - For the following steps the calulated positions from the previous step are used.
//! - After any step the maximal deviation for the position of X-Direction (DeviationX) and Y-direction (DeviationY)
//!   are reported. The <b> measuring unit </b> for the deviations is <b> pixel </b> .
//! - Desirable are <b> deviations <=0.03 </b> .
//! - The use of value 2 for mexPiezoCalibSet::StartSource is not recommended.
//! - The structure is compiled with <b> #pragma pack(1) . </b>
typedef struct mexPiezoCalibSet
{
  int IterCount;                //!< [in], Count of steps for iteration
  int StartSource;              //!< [in], 0: Default start positions, 1 : Start positions from EPROM, 2 : Own positions from this structure
  double ToleranceX;            //!< [in] Tolerance for DeviationX
  double ToleranceY;            //!< [in] Tolerance for DeviationY
  double  DeviationX;           //!< [out] resulting maximal deviation of X-Positions.
  double  DeviationY;						//!< [out] resulting maximal deviation of Y-Positions.
	int ScanDir;                  //!< [in]/[out],Scan direction
  unsigned char Pos[36][2];			//!< [in]/[out]: old/new x-positions
} mexPiezoCalibSet;

//! Structure for the <b> Equalizer </b> which works while a multi shot image is aquired.
//! \note 
//! - These properties of the Equalizer are stored at runtime of CxxPlus.dll per Camera
//!   for the time intervall of a camera session. 
//! - Default values are: <b> bEnable=TRUE, Limit=0.5 </b>.
//! - An application should set its preferred values by ::mexSetEqualizer at start of any camera session.
//! - The structure is compiled with <b> #pragma pack(1) . </b>
//!
//! \sa \ref page16
typedef struct mexEqualizer
{
  BOOL bEnable; //!< if TRUE then brightness differences will be compensated.
  double Limit; //!< value between 0.0 and 1.0. If differences are less than Limit*100 % the they will be compemsated.
} mexEqualizer;

//! Structur to activate/deactivate actions while image aquisition is running in camera
//! For up to three action points possibly outputs are generated to output pin 1.
//! - The array condition specifies the actions on the output pin 1 .
//!   - condition[0] refers to the start of the exposure
//!   - condition[1] refers to the end of the exposure and the start of the  image transfer
//!   - condition[2] refers to the end of the image transfer-
//! - Possible values of condition[i] are:
//!   - 0 : no action
//!   - 1 : signal to low at output pin 
//!   - 2 : signal high at output pin
//!   - 3 : toggle the signal at output pin
//! - The structure is compiled with <b> #pragma pack(1) . </b>
typedef struct mexTriggerOut
{
  unsigned int condition[3]; //!< values 0,1,2,3
} mexTriggerOut;

//! Structure for gamma-type image processing
//! \note
//! - The value of gamma apllies for all color channels uniquely. 
//! - The structure is compiled with <b> #pragma pack(1) . </b>
typedef struct mexGamma
{
  BOOL bActive; //!< ==TRUE if gamma-type image processing is wanted.
  double gamma; //!< 0.2<=gamma <=5.0
}mexGamma;

//! Structure for ::mexSetFocusCallback to calculate a sharpness measure in a ROI (rect of interest).
//! \note
//! - The roi (rectangle of interest) takes member values releative to the incoming image.
//!   The value of <b> roi.top </b> refers to the most top image row in the real szenario, if the camera is normal oriented.
//!   The value of <b> roi.left </b> refers to the most left image column in the real szenario, if the camera is normal oriented.
//! - For a color image the focus number is calculated as maximal value from all selected
//! color channels. 
//! - The condition bChannelSelect[i]=TRUE selects the color channel i into
//! the calculation of the sharpness. The mapping is i=0 : blue channel, i=1: green channel, i=2: red channel.
//! - For a monochrome image bChannelSelect is ignored. 
//! - Of course for more selected color channels the calucaltion time rises. Hence for high speed
//!   calculation of focus number it is advisable to select only one channel (for instance the green one:
//! bChannelSelect[0]=FALSE, bChannelSelect[1]=TRUE, bChannelselect[2]=FALSE)
//! - Time measurings for a  mexFocus::roi of 100 x 100 pixel have been resulted in execution times 
//!   from <b> 0.2 to 0.4 milliseconds </b> for a 2.66 GHz Pentium processor. 
//! - The focus number is calulated from a temporary generated histogram for 8 bits per pixel.
//!   Hence the execution time depends on required bit-shift-operations. If shifts are required 
//!   the execution time rises.
//! - Bit-shift-operations are  not required if ::mexImageParams::ccdtransferCode is 2 or 4. See also: \ref page17
//! - The structure is compiled with <b> #pragma pack(1) . </b>
typedef struct mexFocus
{
  mexFocusCB pFocusCallback; //!< The wanted callback function for mexSetFocusCallback, my be NULL
  RECT roi;                  //!< The wanted rect of interest relative to the incoming image.
  BOOL bChannelSelect[3];    //!< Selection of color channels from which the focus number should be taken.
}mexFocus;

//! Structure for automatic exposure control. Activate/Deactivate it by a call to ::mexActivateExposureControl.
//! \note
//! - The value of targetbrightness is the target value for the mean brightness of incoming images.
//! - If mexExposureCtrl::bActive==TRUE, then  for a running live mode the exposure time is adjusted so that
//! the mean brightness is approximately granted. For example the maximal color value for ProgResC10Plus is 4095.
//! If then targetbrightness=0.5 the mean brightness should be approximately 2048.
//! - The structure is compiled with <b> #pragma pack(1) . </b>
typedef struct mexExposureCtrl
{
  BOOL bActive;               //!< Exposure control active
  double targetbrightness;    //!< Relative brightness to achieve 0.0<= targetbrightness<=1.0
  RECT roi;                   //!< Rect of interest for exposure control
  mexExpCtrlCB pExpCallback;  //!< Callback function to report the exposure ticks while exposure control is active.
}mexExposureCtrl;

//! Structure to control saturation of color images by ::mexActivateSaturationControl
//! \note
//! - It is advisable to use the range <b>  -1.0 <= sc <= +1.0 </b> only because the colors for +1.0 < sc <= 3.0 are possibly bad.
typedef struct mexSaturationCtrl
{
  BOOL bActive;  //!< Saturation control active, <b> possible values 0,1,2,3 </b> as for mexImageParams::bcorrectColors. 
  double sc;     //!< Saturation control parameter -1.0 <= sc <= 3.0
}mexSaturationCtrl;

//! \brief Structur to use for color quality in calls to ::mexCalibrateColors
//! 
//! \note
//! - The structure is compiled with <b> #pragma pack(1) . </b>
typedef struct					
{
  double MinDiff;				//!< Min. difference to nominal values in LAB color space
  double MaxDiff;				//!< Max. difference to nominal values in LAB color space
  double MeanDiff;			//!< Mean difference to nominal values in LAB color space
} mexColorQuality;			

#pragma pack()

//! \brief Enumeration for ::mexMonitorType
//! 
typedef enum 
{
 mexMonitortypeEBU        = 0,     //!< Monitortype EBU-RGB
 mexMonitortypeSRGB       = 1,     //!< Monitortype sRGB
 mexMonitortypeADOBE      = 2,     //!< Monitortype Adobe-RGB
} mexMonitorType;


// exported functions of CxxPlus.dll ///////////////////////////////////////////////////////////////

//! Finds cameras currently connected to PC (identified by FireWire-GUID's)
//! The call finds only cameras of type ProgResC10Plus,ProgResC12Plus, ProgResC14Plus and ProgResM14Plus. 
//! This function may be called at any time (not only in a working session).
//! To find cameras these must be physical connected to the PC and power on camera must be on.
//! Furthermore the installation procedure for the driver <b> firecamj.sys </b> must be successful
//! completed before this function can find such a camera.
//! \remarks 
//! - The list element GUID_List[0] will be overwrite by the value of the first found camera.
//! - Before a call to ::mexFindCameras the caller must provide an unsigned int Number and a list of unsigned __int64 for
//! the global unique identifiers. The number must be initialized with a value N=sizeof(GUID_List)/sizeof(unsigned __int64).
//! After the call GUIDList is filled with identifiers of found cameras and the value of Number is the count of found cameras.
//! \code
//!  // C/C++ code sample:
//!  void __stdcall InitCB(  unsigned long status,
//!                         unsigned long UserValue,
//!                        unsigned __int64 CamGUID)
//!  {
//!    //... doing something with respect to unpluggin/ plugging of cameras
//!  }
//!
//! unsigned int Number=10;
//! unsigned int GUID_List[10];
//! long resinit=mexInit(InitCB,0);
//! long res=mexFindCameras(&Number,GUID_List)
//! if(res==NOERR && Number>0)
//! {
//!   
//!   res=mexOpenCamera(GUID_List[0]);
//! }
//! \endcode
//! The test Number>0 is also neccessary if res==NOERR.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameterlist is wrong (for instance GUID_List==NULL or *N==0).
CXXPLUS_SPEC long __stdcall mexFindCameras(unsigned int* pNumber,       //!< Pointer to an unsigned int ,with value n of length of GUID_List
                                         unsigned __int64 GUID_List[]   //!< Buffer to receive the global unique identifiers of found cameras.
                                         );

//! Finds cameras currently connected to PC by extended conditions.
//! To find cameras these must be physical connected to the PC and power on camera must be on.
//! Furthermore the installation procedure for the driver <b> firecamj.sys </b> must be successful
//! completed before this function can find such a camera.
//! The call finds also camera of strange types (not only CxxPlus).
//! This function may be called at any time (not only in a working session).
//! \param[in] GUID_Mask Mask to control the find procedure.
//! \param[in] pNumber Pointer to an unsigned int ,with value n of length of GUID_List
//! \param[out] pNumber Pointer to an unsigned int ,with count of found cameras.
//! \param[out] GUID_List Buffer to receive the global unique identifiers of found cameras.
//! \param[in]  GUID_List GUID_List[0] specifies a bit pattern for cameras to find.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR If something in the parameterlist is wrong (for instance pNumber==NULL).
//! \note
//! - The caller must provide 2 GUID-related values to configure the find procedure:
//!    - <b> GUID_Mask </b> specifies the bits to test while the function works.
//!    - <b> GUID_List[0] </b> specifies a pattern of GUID-bits to find.
//! - The exact test for a found camera with given global unique identifier <b> Guid </b> is given by: 
//!   - <b> (Guid&GUID_Mask)== (Guid&Guid_List[0]) </b>
//! - The list element GUID_List[0] will be overwrite by the value for the first found camera.
//! - There is an <b> interesting use case </b>: To test whether a camera with fixed given Guid is available on PC 
//!   write following simple code:
//! \code
//!   // C/C++ code fragment
//!   BOOL IsCameraAvailable(unsigned __int64 Guid)
//!   {
//!     unsigned __int64 Mask=0xFFFFFFFFFFFFFFFF; // test all bits
//!     unsigned int N=1;
//!     return (mexFindCamerasEx(Mask,&N,&Guid)==NOERR && N==1);
//!     // The call to dfcFindCamerasEx is cheap in terms of resources on PC and camera.
//!     // On the PC-side the driver FireCamJ.sys only is ultimately involved. No additional resoures
//!     // are created temporary. Of course the driver does something. 
//!   }
//!   // Imagine you have following concrete global unique indentifier regardless from which source.
//!   // No FindCamera.. operation was peformed before:
//!   unsigned __int64 Guid=0x00044C0000180010; 
//!   if(IsCameraAvailable(Guid))
//!   {
//!     // Camera is available
//!     // Do something with the camera, for example:
//!     mexOpenCamera(Guid); // Of course do so only if you have this camera not already opened before.
//!     meInfo info;
//!     mexGetInfo(Guid,&info);
//!     mexCloseCamera(Guid);
//!   }
//!   else
//!   {
//!     // Camera is not available
//!     // Do something else
//!   }
//!  // Of course IsCameraAvailable can be used also for an already opened camera to test whether
//!  // the connection to the camera is still intact.
//! \endcode 
CXXPLUS_SPEC long __stdcall mexFindCamerasEx(const unsigned __int64 GUID_Mask,
                                         unsigned int * pNumber, 
                                         unsigned __int64 GUID_List[]);
//! Get type of camera specified by CamGUID as value from enumeration ::CamType.
//! \param[in] CamGUID Global unique identifier of camera.
//! \retval ::camunknown If the camera does not belongs to the ProgResC/MxxPlus collection.
//! \retval ::cam10plus  For ProgresC10Plus.
//! \retval ::cam12plus  For ProgresC12Plus.
//! \retval ::cam14Cplus For ProgresC14Plus.
//! \retval ::cam14Mplus For ProgresM14Plus.
//! \note
//! - The type can be detemined by the value of CamGUID only. Hence the camera has not to be open for a successful
//!   call of this function.
CXXPLUS_SPEC CamType __stdcall mexGetCameraType(unsigned __int64 CamGUID);

//! Initialize CxxPlus.dll. The function opens a CxxPlus.dll working session. 
//! The function prepares the reception of messages from the operating system about plugging / unplugging
//! of cameras. These messages are forwarded to the caller by pCB. <b> Hence if pCB=NULL the caller receives no
//! plugging / unpluggin messages </b>. 
//! \param[in] pCB Callback function to report events on FireWire Bus.
//! \param[in] UserValue Generic user value.
//! \retval ::NOERR If OK.
//! \retval ::INITERR If something went wrong.
CXXPLUS_SPEC long __stdcall mexInit(meInitCB pCB,           //!< callback function to report events on FireWire Bus
                                    unsigned long UserValue //!< generic user value
                                   );

//! Exits from a working session on CxxPlus.dll and closes all open cameras.
//!
CXXPLUS_SPEC void __stdcall mexExit();


//! Opens a connection to the camera with FireWire-GUID  CamGUID
//! If successful a so called <b> logical connection </b> to the camera specified by CamGUID is established.
//! \param[in] CamGUID Global unique identifier camera of camera
//! \retval ::NOERR if successful opened
//! \retval ::TOOMANYCAMERASOPEN If already 4 cameras are open in the working session.
//! \retval ::INVALIDCAMERATYPE If CamGUID refers to an invalid type of camera or something other went wrong.
//! \note 
//! -# For a successful logical connection a correct physical connection of camera and PC is necessary.
//! The locical connection affects both Camera and PC. The operation called by ::mexOpenCamera is <b> very expensive </b> by 
//! creation of resources on PC and camera  (threads, memory, handles, FireWire resources,...). 
//! -# In the rest of documentation a <b> logical connection is shorter called a conncetion</b>.
//! By a connected camera is meant a logical connected camera.
//! \warning
//! Don't use frequently calls to pairs ::mexOpenCamera... ::mexCloseCamera to test whether a connection between
//! camera and PC is intact. There is a penalty for such software behavior in long test times and possibly
//! more required initialization tasks. To test for an intact connection use ::mexGetInfo or ::mexIsOpen for instance.
//! 
CXXPLUS_SPEC long __stdcall mexOpenCamera(unsigned __int64 CamGUID);
                                         
//! Creates a textual representation (zero terminated) of the binary coded serial number 
//! of the camera and fills it into szString. The string is the same as on the type plate of the camera body.
//! \param[in] CamGUID Global unique identifier camera of camera
//! \param[out] szString Character buffer to receive the textual representation.
//! \retval ::NOERR If OK.
//! \retval ::NOCAMERA If CamGuid specify a camera which is not available at all.
//! \note
//! - <b> 1805-00-0010 </b> is an instance of such a text.
//! - Behavior of this function has been changed from version 1.9 to version 2.0. Now the camera from which ask the
//! serial number <b> has not to be open </b> for a successful call. If the camera is not already open, the 
//! implementation opens it in a spare manner get the information for the serial number and closes it again 
//! immediately after this action.
//! The new behavior is useful to provide the user of an application at startup with information of available cameras.
//! An application can create the serial numbers of available cameras by ::mexFindCameras and ::mexGetSerialNumberString
//! and avoid for this purposes expensive calls to ::mexOpenCamera.
CXXPLUS_SPEC long __stdcall mexGetSerialNumberString(unsigned __int64 CamGUID,char szString[16]);                                     


//! Disconnects camera with FireWire-GUID  CameraGUID (if connected before).
//! Only the logical connection is affected.
//! \param[in] CamGUID Global unique identifier camera of camera
//! \note
//! A call to ::mexCloseCamera may be <b> also  wise </b> if the physical connection of camera and PC is defect.
//! In such cases only the resources on PC from a previously successful connection  are freed. Calls to 
//! ::mexCloseCamera may be riskless repeated without embedded ::mexOpenCamera/::mexCloseCamera calls. 
//!
CXXPLUS_SPEC void __stdcall mexCloseCamera(unsigned __int64 CamGUID);
                                          
//!Reports the open state of specified camera. 
//! \param[in] CamGUID Global unique identifier of camera.
//! \retval ::TRUE If camera is open.
//! \retval ::FALSE If camera is not open.
//! \note 
//! The returned open state is based only on a book keeping in CxxPlus.dll.
//! This book keeping can not detect other troubles of the connection of camera and PC.
//! A real test can be established by a call to some of the \link page5 query functions \endlink.
//! If such a call returns successful the connection from camera to PC is intact.
//!
CXXPLUS_SPEC BOOL __stdcall mexIsOpen(unsigned __int64 CamGUID);
                                     

//! Gets info from currently connected camera (compatible to C10.dll,C12.dll,C14.dll)
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[out] pmeInfo Pointer to a meInfo object to receive the information.
//! \retval ::NOERR If OK.
//! \retval ::PARAMERR If pmeInfo==NULL.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
CXXPLUS_SPEC long __stdcall mexGetInfo(unsigned __int64 CamGUID,
                                       meInfo *pmeInfo
                                      );

//! Gets camera info of currently connected camera (C10Plus-specific)
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[out] pCamInfo Pointer to a meInfo object to receive the information.
//! \retval ::NOERR If OK.
//! \retval ::PARAMERR If pCamInfo==NULL.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \note
//! This function can be used to to set mexImageParams::rcimageBounds to its maximal size:
//! \code
//! // C/C++ code fragment
//! unsigned __int64 Guid=...
//! meCamInfo caminfo;
//! mexImageParams ImgPars;
//! //....
//! long res=mexGetCameraInfo(Guid,&caminfo);
//! if(res==NOERR)
//! {
//!   // maximal possible rect of interest on the CCD-sensor
//!   ::SetRect(&ImgPars.rcimageBounds,0,0,caminfo.SensorX, caminfo.SensorY);
//! }
//! \endcode
CXXPLUS_SPEC long __stdcall mexGetCameraInfo(unsigned __int64 CamGUID,
                                             meCamInfo *pCamInfo 
                                            );

//! \brief Get image info for an image to order by *pImgPars.
//! 
//! \param[in] CamGUID  Global unique identifier of camera.
//! \param[in] pImgPars Pointer to a ::mexImageParams object with valid values from caller.
//! Only the members mexImageParams::mode, mexImageParams::rcimageBounds and 
//! mexImageParams::ccdtransferCode are relevant.
//! \param[out] pImgInfo Pointer to a ::meImageInfo object to receive the information.
//!
//! \retval ::NOERR if successful
//! \retval ::INITERR if there was a initial error
//! \retval ::PARAMERR if CamGUID invalid or pCamnfo==NULL.
//! \attention
//! - The use of this function is <b> very important </b> to allocate image buffers for colorchannels of appropriate size.
//! - This importance comes from the unforeseeable image sizes in case of use ::mexImageParams::rcimageBounds with
//! a real ROI compared to the full sensor rect. If you use a real ROI the resulting
//! image size is not evident for you. 
//! - The implementation of camconj.dll computes the real image sizes an reports them
//! via this function in the members DimX, DimX, PlaneBytes Channels of ::meImageInfo.
//! - For each colorchannel the amount of bytes for an image can be calculated by
//! \code
//! unsigned long Bytes= DimY*DimY*PlaneBytes.
//! // Or if the amount of usigned shorts is required
//! unsigned long Ushorts=DimX*DimY;
//! \endcode
//!
//! - For the role of this function for image acquisition see also : \ref page6a .
CXXPLUS_SPEC long __stdcall mexGetImageInfo(unsigned __int64 CamGUID, 
                                            mexImageParams* pImgPars,
                                            meImageInfo* pImgInfo); 
                                           

//! Acquire an image specified by *ImageParameters.
//! The function returns not until the image is complete, but the completion is announced by a call
//! to ::mexImageParams::pImgFinalProc. The memory for the colorchannels of images are provdied by the implementation internally.
//!  See also ::mexCloseImageTransfer.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pImgPars Pointer to a mexImageParams object with valid values from caller.
//! \retval ::NOERR if successful
//! \retval ::PARAMERR if something in the parameter lis is wrong, for instance if <b> ccdtransferCode=2,3,4 and (busewhiteref || buseblackref) </b>.
//!
//! \copydoc page18
CXXPLUS_SPEC long __stdcall mexAcquisition(unsigned __int64 CamGUID,
                                           mexImageParams* pImgPars);
                                           
//!  Provides information about support of an acquisition mode amode for a camera.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] amode Acquisition mode.
//! \retval ::TRUE if amode is supported
//! \retval ::FALSE if amode is not supported.
//! \note 
//!- The acquisition modes ::mexShot1F_spec1 and ::mexShot1F_spec2 are 
//!  <b> not supported for ProgResC10Plus. </b>
//!- For ProgResC14Plus and ProgResM14Plus there are possibly special conditions with respect
//!  to the scanning modes ::mexc14_4Shot_col, ::mexc14_9Shot_col, ::mexc14_16Shot_col, ::mexc14_36Shot_col, ::mexm14_4Shot_bw,
//!  ::mexm14_9Shot_bw. Even though these acquisition modes are supported in principle, for special sales models
//!  some of these modes may be disabled (not supported). Hence any software that uses MexAPI should careful 
//!  test the wanted acquisition modes.
CXXPLUS_SPEC  BOOL __stdcall mexIsAcquisitionModeSupported(unsigned __int64 CamGUID,
                                                           mexAcqMode amode);
                                                                                      

//! Starts fast continously image acquisition. This is also called the <b> CC-Live mode (Camera Controlled Live). </b>
//! The memory for the colorchannels of images are provdied by the implementation internally.
//! Any incoming image is announced for the caller by a call to the callback given in the
//! ::mexImageParams::pImgFinalProc from the object *ImageParameters. The member
//! ::mexImageParams::pImageproc for the progress callback will be ignored.  See also ::mexCloseImageTransfer.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pImgPars Pointer to a mexImageParams object with valid values from caller.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong, for instance if <b> ccdtransferCode=2,3,4 and (busewhiteref || buseblackref) </b>.
//! \note 
//! - In case of an image acquired by mexAcqMode::mexShot2F_Raw only the first channels <b> colorchannels[i][0] , i=0,1,2  </b> are used. The other channels should be  set to NULL.
//! - To realize a live image with high image rate only the acquisition modes  ::mexShot1F_highspeed1 and ::mexShot1F_highspeed2 are recommended.
//! - See also: \link page13  Camera controlled live (CC-Live)  and Application controlled live (AC-Live) \endlink
//!
//! \copydoc page18
CXXPLUS_SPEC long __stdcall mexStartFastAcquisition(unsigned __int64 CamGUID,
                                                    mexImageParams* pImgPars);
                                                   

//! Starts fast continuously image acquisition and takes image memory from caller. This is also a version of <b> CC-Live </b>.
//! The caller must provide a ring buffer for 3 images.
//! clorchannels[0][*] is the ring element for the first image, colorchannels[1][*] for the second image, colorchannels[2][*]
//! for the third image. After the third image the buffer using returns to the first ring element... and so on, * stands for indexes
//! 0,1,2 (blue,green,red channel).
//! The function returns immediately after the continuous running image acqusition is started. In the moment of return
//! usual no image is already complete. See also ::mexCloseImageTransfer.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pImgPars Pointer to a mexImageParams object with valid values from caller.
//! \param[in] colorchannels Pointer to 3 buffer - each of consist of 3 color channels 
//! in order blue, green,red - to receive color image data from camera.
//! \param[in] MemsizePerChannel Guaranteed memory size (<b> bytes </b>) of each color channel buffer 
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong, for instance if <b> ccdtransferCode=2,3,4 and (busewhiteref || buseblackref) </b>.
//! \note 
//! - In case of an image acquired by mexAcqMode::mexShot2F_Raw only the first channels 
//! <b> colorchannels[i][0] , i=0,1,2  </b> are used. The other channels should be set to NULL.
//! - See also:\link page13  Camera controlled live (CC-Live) and Application controlled live (AC-Live) \endlink
//!
//! - \copydoc page17
//!
//! \copydoc page18
CXXPLUS_SPEC long __stdcall mexStartFastAcquisitionEx
                                    (unsigned __int64 CamGUID,
                                     mexImageParams* pImgPars,
                                     void* colorchannels[3][3],
                                     unsigned long MemsizePerChannel);
                                    

//! Starts acquisition for a single image and take image memory from caller.
//! The completion of the image will be announced by a call to ImageParameters->pImgFinalProc.
//! The function returns immediately after the single image acqusition is started. In the moment of return
//! usual the image isn't already complete. See also ::mexCloseImageTransfer.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pImgPars Pointer to a mexImageParams object with valid values from caller.
//! \param[in] colorchannels Pointer to 3 buffers for 3 color channels 
//! in order blue, green,red - to receive color image data from camera.
//! \param[in] MemsizePerChannel Guaranteed memory size (<b> bytes </b>) of each color channel buffer. 
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong, for instance if <b> ccdtransferCode=2,3,4 and (busewhiteref || buseblackref) </b>.
//! \note 
//! - In case of an image acquired by mexAcqMode::mexShot2F_Raw only the first channel 
//! <b> colorchannels[0] </b> is used. The other channels should be  set to NULL.
//! - See also:\link page13  Camera controlled live (CC-Live)  and Application controlled live (AC-Live) \endlink
//!
//! \copydoc page18
CXXPLUS_SPEC long __stdcall mexStartAcquisition(unsigned __int64 CamGUID,
                                                mexImageParams *pImgPars,
                                                void* colorchannels[3],
                                                unsigned long MemsizePerChannel
                                               );

//! Abort started image acquisitions.
//! Any image acquisition started by ::mexStartFastAcquisition or ::mexStartFastAcquisitionEx must be aborted
//! by a call to this function. A call to ::mexStartAcquisition or ::mexAcquisition can be aborted by a call
//! to this function. <b> In case of ::mexAcqusition the abort call must be called by an other thread then the
//! acquisition call. </b> In case of aborting any image acquisition the rest of uncomplete images are invalid and
//! should be discarded. The caller must establish and test  his own logic for aborting image acquisition. 
//! Any following image acquisition after the abort must always be secured by properly ending the abort.
//! The abort action is so secured by CamConJ.dll and FireCamJ.dll, but in case of the <b> AC-Live procedure </b> 
//! mentioned in the description of ::mexStartAcquisition the caller must provide additional means in his own 
//! responsibility. For instance exactly bookkeeping on acquisition calls and received completion events to receive
//! the rest of sequence after the abort.
//! \param[in] CamGUID Global unique identifier of camera.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
//! \retval ::ABORTERR if camera is not physical connected to PC (perhaps if camera was unplugging)
//! \note In case of ABORTERR the abort action was at least at PC side sucessful.
CXXPLUS_SPEC long __stdcall mexAbortAcquisition(unsigned __int64 CamGUID);
                                               

//! Closes the image transfer to free transfer channel
//! \param[in] CamGUID Global unique identifier of camera.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
//! \note This function is useful only if more than one camera are used in a working session.
//! The reason is, that all cameras rivals each other for the available bandwidth on the FireWire bus.
//! Hence if one camera has already allocate some part of the available bandwidth, possible there is
//! not sufficient bandwidth available for other cameras. <b> In such a situation a call to some of the
//! aqcquisition function may fail for a secondary, third or fourth camera.</b> A call to this function 
//! can resolve the problem for the benefit of an other camera. Note also that timely parallel image acquisition
//! from two cameras is imaginable also in any of the Live modes. But than only one camera can aquire a full image and the
//! other camera must content onself with a small rect of interest. This imaginable feature is not yet testet.
//! Furthermore since the needed bandwidth is bad predictable, the feature is only to develope by experiments. \n 
//! <b> See also: </b> \link  page11  The use of multiple cameras in one CxxPlus-session.  \endlink
CXXPLUS_SPEC long __stdcall mexCloseImageTransfer(unsigned __int64 CamGUID);
                                               

//! Frees internal image memory of CxxPlus.dll allocated in consequence of calls to ::mexAcquisition,::mexStartFastAcquisition,
//! ::mexGetBlackRef, ::mexRestoreBlackRef, ::mexGetWhiteRef or ::mexSetWhiteTable.
//! If no memory is allocated internally the implementation does nothing.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] parts Code to specify which parts of internal memory should be affected (an OR combination of ::mexMemParts).
//! \warning
//! <b> The function must not called if an image acquisition is running that uses the affected memory as to: </b>
//! -# ::mexAcquisition, ::mexStartFastAcquisition if \n <b> Parts </b>&::image_memory !=0
//! -# All image acquisition functions if \n <b> Parts </b>  & ( ::blackref_memory | ::whiteref_memory))!=0 and ::mexImageParams::buseblackref ==TRUE or ::mexImageParams::busewhiteref ==TRUE.
//! -# Parts&::image_memory should be used for a currently open but unused camera to provide more memory space for an other
//! used camera. Later if the camera for which the ::image_memory was freed is reused the implementation reallocates 
//! the ::image_memory if nessesary.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
CXXPLUS_SPEC long __stdcall mexFreeInternalMemory(unsigned __int64 CamGUID,int parts);
                                                                                                 
//! \brief Modify aquisition parameters while live mode is running.
//!
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pImgPars Pointer to a ::mexImageParams object with new settings from caller compared to the settings
//!  from the most recently call to ::mexStartFastAcquisition or ::mexStartFastAcquisitionEx.
//! \retval ::NOERR If OK.
//! \retval ::NOCAMERA If CamGUID does not specify a currently open camera.
//! \retval ::PARAMERR If some of the values of the *pImgPars are illegal (<b> see notes below </b> ).
//! \retval ::CAMERANOTSTARTED If the camera is not in a running live state.
//! \note
//! - New values will be ignored for 
//!      - ::mexImageParams::pImgFinalProc
//!      - ::mexImageParams::UserValue
//!      - ::mexImageParams::pImageproc 
//!      - ::mexImageParams::dwData
//! - For the rest the new values must be so that no change for the image memory results from the new values compared
//!   to the old ones. In detail this results in:
//!      - For ::mexImageParams::ccdtransferCode changing from <b> 0 </b> to <b> !=0 </b> is invalid.
//!      - For ::mexImageParams::rcimageBounds changes are invalid if they would be result in different image memory size.
//!        Be careful to use ::mexGetImageInfo to validate such changes.
//!      - For mexImageParams::mode changes are invalid if they would be result in a different image memory size. 
//!        This is the most likely case. Hence as a thumb rule: <b> mode should not change </b>.
//! - This function provides a functional superset of ::mexModifyLiveParameters and ::mexModifyLiveBools. The latter are in fact
//!   retundant but remain in <b> mexAPI </b> for compatibility.
//! - This function can be used to modify ::mexImageParams::ciCode while a live mode is running. Up to now,
//!    before this function was available, this was impossible.
CXXPLUS_SPEC	long __stdcall mexModifyLive(unsigned __int64 CamGUID, mexImageParams* pImgPars);

//! Modify acquisition parameter while fast acquisition is running (live mode)
//! The ::mexImageParams::expsoureTicks can always be modified while fast image acquisitionis running.
//! For a modification of ::mexImageParams::rcimageBounds the following condition to take is
//! into account: <b> The resulting image pixel size (X x Y) and the required buffer size to store
//! the color channels must be unchanged by the changed rcimageBounds. </b>
//! If this condition is violated the function returns an error and the immage acquisition runs
//! under previous conditions. Then also the order to the modified exposure time is ignored. 
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] exposureTicks New exposure ticks to apply.
//! \param[in] prcimageBounds New rect of interest on the CCD-sensor.
//! \retval ::NOERR if successful.
//! \retval ::NOCAMERA If CamGUID does not specify a currently open camera.
//! \retval ::PARAMERR if something in the parameter list is wrong.
//! \note
//! - For a successful call the running live mode must been started with ::mexStartFastAcquisition or ::mexStartFastAcqisitionEx.
CXXPLUS_SPEC	long __stdcall mexModifyLiveParameters(unsigned __int64 CamGUID,
                                                     long exposureTicks,
                                                     RECT* prcimageBounds);
                                                    

//! Modify acquisition parameters while fast acquisition is running (<b> CC-Live/CC-LiveEx only </b>).
//! Boolean ::mexImageParams flags  buseblackref,busewhiteref,bwhitebalance,bcorrectcolors in this order
//! The condition for the modified uses of  the boolean flags must be fulfilled.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param b Four boolean values to modify ::mexImageParams flags <b> buseblackref,busewhiteref,bwhitebalance,bcorrectcolors in this order. </b>
//! \retval ::NOERR if successful.
//! \retval ::NOCAMERA If CamGUID does not specify a currently open camera.
//! \retval ::PARAMERR if something in the parameter list is wrong.
//! \note
//! - For a successful call the running live mode must been started with ::mexStartFastAcquisition or ::mexStartFastAcqisitionEx.
CXXPLUS_SPEC	long __stdcall mexModifyLiveBools(unsigned __int64 CamGUID,BOOL b[4]);
                                                
                                               

//! Creates an black reference an stores the values internally in the PC-RAM 
//! The black reference should be created while CCD sensor is shaded (no light should hits the sensor).
//! After creation of a black reference the referenc can be used in an image acquisition by 
//! setting of ::mexImageParams::buseblackref to TRUE .
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pCallBack Progress callback.
//! \param[in] dwData Generic user value.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
CXXPLUS_SPEC long __stdcall mexGetBlackRef(unsigned __int64 CamGUID,
                                           mexImageProc pCallBack,
                                           DWORD dwData);
                                          

//! Get length for memory to save black reference
//! Returns the required length of a memory buffer in PC-RAM to store a previously by ::mexGetBlackRef created
//! black reference. An user of ::mexSaveBlackRef must use the length to allocate a memory buffer.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param pLength Pointer to an unsigned int to receive the length in Bytes.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
CXXPLUS_SPEC long __stdcall mexGetBlackRefLength(unsigned __int64 CamGUID,
                                                 unsigned int* pLength);
                                                

//! Creates white reference from camera and stores it internally.
//! The white reference should be created with illumination conditions in the image scenario that are
//! presumably constant up to the nexts acquisitions. The white reference can be used in next acqusitions
//! \param[in] CamGUID Global unique identifier of camera.
//! \param pImgPars Pointer to a meImageParams object with valid values from the caller.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
//! by setting ::mexImageParams::busewhiteref to TRUE. 
CXXPLUS_SPEC long __stdcall mexGetWhiteRef(unsigned __int64 CamGUID,
                                           mexImageParams* pImgPars);
                                          

//! Get length of memory to save white reference
//! The length must be used in a call to ::meGetWhiteTable to provide an memory buffer of necessary size
//! \param[in] CamGUID Global unique identifier of camera.
//! \param pLength Pointer to an unsigned int to receive the length in Bytes.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
CXXPLUS_SPEC long __stdcall mexGetWhiteRefLength(unsigned __int64 CamGUID,
                                                 unsigned int* pLength); 
                                                

//! Get white reference from CxxPlus.dll created before by meGetWhiteRef(...)
//! The buffer pointed by table can be used to restore the internally white table in the futur 
//! by a call to ::mexSetWhiteTable
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[out] pTable Pointer to a buffer of length reported from ::mexGetWhiteRefLength to receive the data of white reference.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
//! \note The white reference depends on the quality ot light (spectrum, color temperature, shadowing effects, ...) of the scene.
//! Hence it should be renewed by ::mexGetWhiteRef if the quality light has changed.
CXXPLUS_SPEC long __stdcall mexGetWhiteTable(unsigned __int64 CamGUID,
                                             void * pTable);
                                            

//! Sets the white reference created before by ::mexGetWhiteTable into a camera object of CxxPlus.dll.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[out] pTable Pointer to a buffer of length reported from ::mexGetWhiteRefLength to receive the data of white reference.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
//! \note The content of memory referenced by <b> table </b> is copied into a  CxxPlus-internal memory object per camera.
//! Hence the caller can delete his own memory after this call.
//!
CXXPLUS_SPEC long __stdcall mexSetWhiteTable(unsigned __int64 CamGUID,
                                             void *pTable);
                                            

//! Set  image white balance to CxxPlus.dll. A sample from a nominal white (gray)  area should be taken to calculate the wanted
//! factors. 
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] Red Red meanvalue of samples from an intentionally white region in an image.
//! \param[in] Green Geen meanvalue of samples from an intentionally white region in an image.
//! \param[in] Blue  Blue meanvalue of samples from an intentionally white region in an image.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
//! \note
//! Only the relative values among each other are relevant. For instance the values can be normed so that the
//! some of the values is 1.0. and the others are >=1.0 to get nearly the same gray values for a nominal gray area.
//! No one of the values must be 0.0, because this would be result in a division by 0.0.
//! The given values are recalculated with the basic white balance values Set by ::mexSetWhiteBalance or get by
//! ::mexGetCurrentWhiteBalance.
CXXPLUS_SPEC long __stdcall mexSetImageWhiteBalance(unsigned __int64 CamGUID,
                                                    double Red,
                                                    double Green,
                                                    double Blue);
                                                   

//! Set basic white balance to CxxPlus.dll
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] Red Red meanvalue of samples from an intentionally white region in an image.
//! \param[in] Green Geen meanvalue of samples from an intentionally white region in an image.
//! \param[in] Blue  Blue meanvalue of samples from an intentionally white region in an image.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
//! \note
//! These values are processed multiplicatively with the source values from the camera without any normalizing.
CXXPLUS_SPEC long __stdcall mexSetWhiteBalance(unsigned __int64 CamGUID,
                                               double Red,
                                               double Green,
                                               double Blue);
                                              

//! Get currently valid white balance from CxxPlus.dll
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[out] pRed Pointer to a double to receive the current red balance value of DLL
//! \param[out] pGreen Pointer to a double to receive the current green balance value of DLL
//! \param[out] pBlue  Pointer to a double to receive the current blue balance value of DLL
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong.
//!
CXXPLUS_SPEC long __stdcall mexGetCurrentWhiteBalance(unsigned __int64 CamGUID,
                                                      double *pRed,
                                                      double *pGreen,
                                                      double *pBlue);
                                                     

//! Save black reference created by mexGetBlackRef(...) into a user buffer where pointer ref points to. 
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong (for instance ref==NULL).
//! \note Black references are very stable for ProgResC10Plus and ProgResC12Plus. Hence is is usual practice to 
//! save a black reference by this function an restore it on the next application start by ::mexRestoreBlackRef
//! also over days and months.
CXXPLUS_SPEC long __stdcall mexSaveBlackRef(unsigned __int64 CamGUID,  //!< Global unique identifier of camera
                                            void *ref //!< Pointer to a buffer of length obtained from ::mexGetBlackRefLength to receive the black reference data.
                                           );

//! Sets the black reference  previous created by mexSaveBlackRef into a camera object of CxxPlus.dll.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong (for instance ref==NULL).
//! \note The content of memory referenced by <b> ref </b> is copied into a CxxPlus-internal memory object per camera.
//! Hence the caller can delete his own memory after this call.
CXXPLUS_SPEC long __stdcall mexRestoreBlackRef(unsigned __int64 CamGUID,  //!< Global unique identifier of camera
                                               void *ref //!< Pointer to a buffer previous filled by a call to ::mexSaveBlackRef.
                                              );


//! Special function for multiple semantic.
//! Semantic is controlled by code,
//! parameters are taken from pV.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong(for instance code >2)
//!
//! The following values for code are supported:
//! - <b> code=0: </b> Setting of <b> FireWire transfer mode </b> for image data, pV must be  taken from ::meTRANS_MODE.
//! - <b> code=1: </b> Setting of the <b> automatic error handling </b> of the <b> CxxPlus.dll </b> if image data transfer errors come in, pV must be taken from a OR combination from ::XFERHANDLING.
//! - <b> code=2: </b> The use of code=2 has been disabled from version 1.9 to version 2.0 and later. The change of ccd-speed is now controlled by mexImageParams::ccdtransferCode. \n
//! - <b> code='DLAY': </b> Inserts a call of function <b> Sleep  </b> from Windows-API into the image processing line
//!   of camconj.dll at the end of the image processing for any image. The sleep time is to specify as in the code sample above:
//!   the Sleep distrubutes the processor time at the expense of camconj image processing thread to the benefit of other threads.
//! - <b> See also: </b> \ref page17, \ref page12
//! Here are sample codes for the use of ::mexDispose:
//! \code
//! // C/C++ source code fragment
//!  camguid=...;
//! // Set the FireWire transfer mode to "one channel and small packets": Recommended for problematic computers.
//! // Only TRANS_MODE_ONE_CHANNEL_SMALL_PACKET, and TRANS_MODE_ONE_CHANNEL_LARGE_PACKET are recommended.
//! //
//! unsigned long transmode=TRANS_MODE_ONE_CHANNEL_SMALL_PACKET;
//! long res1=mexDispose(camguid,reinterpret_cast<void*>(transmode),0); // code=0
//! 
//! // Transfer error handling: Automatic call of mexAbortAcquisition and Resurrection of Live Mode after
//! // an incoming mexImageFinalProc call with status=IMAGE_TRANSFER_ERR.
//! //
//! unsigned long errhandling=(abort_on_xferr | resurrect_live_on_xferr);
//! long res2=mexDispose(camguid,reinterpret_cast<void*>(errhandling),1);//code=1
//!
//! unsigned long sleep_time=1 // 1 millisecond
//! long res3=mexDispose(camguid,reinterpret_cast<void*>(sleep_time),'DLAY'); // code is specified by the for-byte-code 'DLAY'.
//! \endcode
//! \note The vehicle void* pV must be exactly used as in the samples: A <b> unsigned long </b> must cast to <b> void* </b>.
//! For instance in the first code sample you <b> must not write: </b>
//! \code
//! res=mexDispose(camguid,reinterpret_cast<void*>(&transmode),0); // the & is wrong
//! \endcode
CXXPLUS_SPEC	long __stdcall mexDispose(unsigned __int64 CamGUID,  //!< Global unique identifier of camera
                                        void* pV,                  //!< generic vehicle for  parameter transfer
                                        unsigned long code         //!< code for semantic
                                      );

//! Sets the working state of the peltier element(ProgResC10Plus, ProgResC12Plus only).
//! param[in] CamGuid Global unique identifier of camera.
//! param[in] value 0<=var <=255 0 - maximal, 255 - minimal.
//! param[in] bOn Cooling on if bOn=TRUE.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameter list is wrong( for instance CamGUID==0).
//! \note 
//! Not every camera of type ProgResC10Plus and ProgResC12Plus must be delivered with a cooling
CXXPLUS_SPEC	long __stdcall mexSetPeltier(unsigned __int64 CamGUID,
                                           unsigned char value,
                                           BOOL bOn
                                          );

//! Set the working state of the peltier element(ProgResC14Plus, ProgResM14Plus).
//! The working state of the ventilator is also affected.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] bActive Activation request.
//! \retval ::NOERR If successful.
//! \retval ::PARAMERR If something in the parameter list is wrong( for instance CamGUID==0).
//! \retval ::NOTSUPPORTED If the camera model does not support cooling (no built-in peltier element).
//! \note 
//! - The type BOOL of bActive is really an int (in WINDOWS-API)
//! - Acceptable values for bActive has been extended in version 2.7. Previously only the values FALSE=0, and TRUE=1
//!   were possible. Now three values are acceptable 
//!   - <b> bActive=0 </b> Deactivates the cooling.
//!   - <b> bActive!=0 </b> Activates the cooling.
//!   - <b> bActive='?' (63 decimal, 0x3F hex) </b> Ask for the presence of cooling , relevant for the \link page25 some camera models \endlink.
//!     If this value is used the state of a available cooling (peltier element, and ventilator) is not changed.
//!     Hence this value can be used to ask for presence of cooling at start of a camera session and inform the user
//!     about presence/absence of cooling.
CXXPLUS_SPEC	long __stdcall mexActivatePeltier(unsigned __int64 CamGUID,BOOL bActive);


//! Conversion of milliseconds to camera ticks for exposure time.
//! Any image acqusition function of <b> MexAPI </b> takes only camera ticks as values of the
//! member mexImageParams::exposureTicks. Because the <b> Tick scale </b> is camera specific and
//! not in the usual scope of MexAPI users, the conversion functions are provided.
//! Primary only the conversion from milliseconds to camera ticks is necessary but sometimes
//! also the inverse conversion from camera ticks to milliseconds ::mexTicksToMSec is useful. 
//! This function may be called at any time (not only in a camera session).
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] msec Milliseconds to convert.
//! \param[in] am Intended acquisition mode.
//! \param[in] speed_code   code: 0 - 12MHz: C10Plus, C12Plus, C14Plus, 1 - 18 MHz: C12Plus, 24 MHz: C14Plus, M14Plus
//! \return Amount of camera ticks to realize the intended  amount of milliseconds msec for exposure time.
//! The value is exact in sense of best approximation between the two scales.
//! \note Supported ccd operation frequencies are show by following table.
//! \copydoc page12
CXXPLUS_SPEC  unsigned long __stdcall mexMSecToTicks(unsigned __int64 CamGUID,
                                                     unsigned long msec,
                                                     mexAcqMode am,
                                                     unsigned long speed_code);
                                                    
//! Conversion of camera ticks to milliseconds for exposure time.                                      
//! Any image acqusition function of <b> MexAPI </b> takes only camera ticks as values of the
//! member mexImageParams::exposureTicks. Because the <b> tick scale </b> is camera specific and
//! not in the usual scope of an user of the <b> MexAPI </b>, the conversion functions are provided.
//! The inverse function ::mexMSecToTicks is more often usable, because the <b> milliseconds scale </b> is the usual
//! scale for MexAPI-users. This function may be called at any time (not only in a camera session).
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] ticks Ticks to convert.
//! \param[in] am Intended acquisition mode.
//! \param[in] speed_code   code: 0 - 12MHz: C10Plus, C12Plus, C14Plus, 1 - 18 MHz: C12Plus, 24 MHz: C14Plus, M14Plus
//! \return Amount of milliseconds to realize the intended  amount of ticks for exposure time.
//! The value is exact in sense of best approximation between the two scales.
//! \note   
//! - The <b> maximal valid value of camera ticks </b> is:
//!   -# <b> 1048575 </b> for ProgresC10Plus and ProgResC12Plus, <b> 4194303 </b> for ProgResC14Plus and ProgResM14Plus.
//!   -# The conversion functions ::mexTicksToMSec can be used to get the maximal exposure time in milliseconds.
//!   -# For <b> ProgResC10Plus </b> the maximal exposure time is for instance <b> 195243 milliseconds </b> for mexShot2F_Raw and 12 MHz.
//!   -# For <b> ProgResC12Plus </b> the maximal exposure time is for instance <b> 264955 milliseconds </b> for mexShot2F_Raw and 12 MHz.
//!   -# For <b> ProgResC12Plus </b> the maximal exposure time is for instance <b> 164636 milliseconds </b> for mexShot2F_Raw and 18 MHz.
//!   -# For <b> ProgResC14Plus </b> the maximal exposure time if for instance <b> 610987 milliseconds </b> for mexShot_Raw   and 12 Mhz
//!   -# For <b> ProgResC14Plus </b> the maximal exposure time if for instance <b> 305493 milliseconds </b> for mexShot_Raw   and 24 Mhz
//!
//! \note Supported ccd operation frequencies are show by following table.
//! \copydoc page12
CXXPLUS_SPEC  unsigned long __stdcall mexTicksToMSec(unsigned __int64 CamGUID,
                                                     unsigned long ticks,
                                                     mexAcqMode am,
                                                     unsigned long speed_code);
                                                    
//! Loads the firmware code which resides in the CxxPlus.dll as resource into the camera specified by CamGUID.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pCallBack Callback function to report progress state of update process. 
//! \param[in] UserValue Generic user value, used by any call of pCallBack.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something went wrong. In this case the value of <b> done </b> from the last call
//! to pCallBack is a hint to the failure. <b> Sometimes the update is still successful. Then try to unplug and plug again the camera
//! to test whether the camera works with your applictaion.</b>
//! \note  Recommendations: 
//! - Use this function if only <b> exact one camera </b> is physical connected to the PC.
//! - Call this function in a seperate thread.
//! - For an example code see ::CxxHost::OnUpdateFirmwareIfNecessary.
CXXPLUS_SPEC long  __stdcall mexUpdateFirmware(unsigned __int64 CamGUID,
                                             mexImageProc pCallBack,
                                             unsigned long UserValue);
                                           

//! returns in *pFWV  version of camera firmware specified by CamGUID coded in resource data of CxxPlus.dll
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[out] pFWV Pointer to a meVERSION object to receive the version infomation.
//! \retval ::NOERR if successful.
//! \retval ::DOWNLOADERR if something in the parameterlist is wrong (for instance pFWV==NULL).
//!
CXXPLUS_SPEC long  __stdcall mexGetFirmwareVersionFromData(unsigned __int64 CamGUID,
                                                          meVERSION* pFWV);
                                                         
																	

//! Returns in *pFWV version of firmware from camera specified by CamGUID
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[out] pFWV Pointer to a meVERSION object to receive the version infomation.
//! \retval ::NOERR if successful.
//! \retval ::PARAMERR if something in the parameterlist is wrong (for instance pFWV==NULL).
//!
CXXPLUS_SPEC long  __stdcall mexGetFirmwareVersionFromCamera(unsigned __int64 CamGUID,
                                                             meVERSION* pFWV);
                                                             

//! Get version from this dll and/or some other used components
//! \param[in] comp To take from enum ::meCOMPONENT to indicate from which component the version is to retrieve.
//! \param[out] pVer Pointer to a ::meVERSION object to receive the version infomation.
//! \retval ::NOERR If successful.
//! \retval ::PARAMERR If something in the parameterlist is wrong (for instance pVer==NULL).
//! \retval ::NOTIMPLEMENTED If the function supports some component not yet.
//! \retval ::NOCAMERA If no camera is open and the request demands an open camera.
//! \note
//! - For ::meCOMP_CXXDLL, ::meCOMP_CAMCONJ and ::meCOMP_FIRECAMJ no open camera is necessary for a successful call.
//! - For ::meCOMP_FIREDRIV, ::meCOMP_CAM1 and ::meCOMP_CAM2 there must be an open camera for a successful call.
CXXPLUS_SPEC long  __stdcall mexGetVersion(meCOMPONENT comp,
                                           meVERSION* pVer
                                          );
//! Set scanner positions for the piezoscanner. An application should only set scanner position obtained by ::mexGetScannerPositions.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pSP Pointer to an existing ::mexScannerPositions object with valid values.
//! \retval ::NOERR If OK.
//! \retval ::NOTIMPLEMENTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \retval ::PARAMERR If pSP==NULL.
//! \note This function is only supported by ProgResC14Plus.
CXXPLUS_SPEC long  __stdcall mexSetScannerPositions(unsigned __int64 CamGUID, //!< Global unique identifier of camera.
                                                    mexScannerPositions * pSP //!< Pointer to an existing ::mexScannerPositions object.
                                                   );
//! Get the actual scanner positions for the piezoscanner.
//! At start of a camera session actual scanner positions are such that are created by the manufacturer of the camera
//! and stored in the EPROM of the camera. After the use of ::mexSetScannerPositions these scanner positions are the actual ones.
//! The manufacturers scanner positions are not changable by <b> MexAPI </b>.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[out] pSP Pointer to an existing ::mexScannerPositions object to receive the current used scanner positions.
//! \retval ::NOERR If OK.
//! \retval ::NOTIMPLEMENTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \retval ::PARAMERR If pSP==NULL.
//! \note This function is only supported by ProgResC14Plus.
CXXPLUS_SPEC long  __stdcall mexGetScannerPositions(unsigned __int64 CamGUID,
                                                    mexScannerPositions * pSP
                                                   );
//! Reset the scanner position to that from EPROM of camera. 
//! \param[in] CamGUID Global unique identifier of camera.
//! \retval ::NOERR If OK.
//! \retval ::NOTIMPLEMENTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \note This function is only supported by ProgResC14Plus.
CXXPLUS_SPEC long  __stdcall mexResetScannerPositions(unsigned __int64 CamGUID); 
                                                   
//! Create new scanner position for the scanning mode specified by mexImagParams::mode, mexImageParams::exposureTicks
//!  and mexImageParams::rcimageBounds. The other values from ::mexImageParams are ignored.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pImgPars Infomation for the wanted acquisition mode, exposure time and rect of interest. All other values are ignored.
//! \param[out] pCalibSet Result of calibration procedure <b> and (before the call) </b> Positions to use for the calibration step.
//! \param[in] pCalibSet Position to use for the calibration
//! \param[in] pCB Pointer to a calibration callback function.
//! \param[in] User UserValue for the calibration callback.
//! \retval ::NOERR If OK.
//! \retval ::NOTIMPLEMENTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \retval ::PIEZOCALBADIMAGE If the image scenario is not applicable for piezo calibration 
//! (for instance if there is not enough structure or if the image is to sharp).
//! \note 
//! - Piezo calibration works stepwise Each step should call this function.
//! - For the first step the ScannerPositions from a first call to mexGetScannerPositions is a good selection.
//! - Only the green channel of the CCD-sensor is used.
//! - In the result mexPiezoCalibSet::DeviationX and mexPiezoCalibSet::DeviationY is a measure for the
//!   achieved precision of the result. Deviations <=0.3 should be realized.  
//! - The image szenario should be light defocussed to achieve best results.
//! - If in a step the wanted precision (deviations) is not realized, use the the computed ScannerPositions for the next step.
//! - This function is only supported by ProgResC14Plus.
//! - If anytime the user implemented callback ::mexPiezoCB returns TRUE, the calibration will be aborted.
//! \attention
//! - If an application want to use the new created scanner-positions, it <b> must copy these positions </b> into
//!   a ::mexScannerPositions object and then call ::mexSetScannerPositions. 
//! - The application must copy from array mexPiezoCalibSet::Pos into the appropriate array with respect to the 
//!   calibrated acquisition mode:
//!     - mexScannerPositions::pos4  for ::mexc14_4Shot_col / ::mexm14_4Shot_bw. 
//!     - mexScannerPositions::pos9  for ::mexc14_9Shot_col / ::mexm14_9Shot_bw.
//!     - mexScannerPositions::pos16 for ::mexc14_16Shot_col.
//!     - mexScannerPositions::pos36 for ::mexc14_36Shot_col.
//! - The flags mexScannerPositions::flag4 ...should not changed compared to the flags from a call to 
//!   ::mexGetScannerPositions. 
//! - The copy operation must take into acount the <b> correct number of elements to copy </b>, that is 
//!     -  4 * 2 positions for ::mexc14_4Shot_col / ::mexm14_4Shot_bw.
//!     -  9 * 2 positions for ::mexc14_9Shot_col / ::mexm14_9Shot_bw   
//!     - 16 * 2 positions for ::mexc14_16Shot_col.   
//!     - 36 * 2 positions for ::mexc14_36Shot_col.   
CXXPLUS_SPEC long  __stdcall mexCalibrateScanner(unsigned __int64 CamGUID,  
                                                 mexImageParams* pImgPars,
                                                 mexPiezoCalibSet* pCalibSet,
                                                 mexPiezoCB pCB, 
                                                 unsigned long User); 
                                                
//! Set properties for the Equalizer wich works if a multi shot image is aquired.
//! Only for ProgResC14Plus / ProgResM14Plus. 
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pEQ Pointer to an mexEqualizer object with valid settings;
//! \note 
//! \retval ::NOERR If OK.
//! \retval ::NOTIMPLEMENTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \retval ::PARAMERR If pEQ==NULL.
//! - The <b> Equalizer </b> is a software module of camconj.dll that tries to
//!   equal different brightness of the multiple images.
//! - Every time if a multishot image is ordered, camconj.dll must be provided with valid settings for its
//!   equalizer.  
//! - These properties of the Equalizer are stored at runtime of CxxPlus.dll per camera
//!   for the time intervall of a camera session. 
//! - Default values are: <b> bEnable=TRUE, Limit=0.5 </b>.
//! - An application should set its preferred values by ::mexSetEqualizer at least at start of any camera session.
//! \sa \ref page16
CXXPLUS_SPEC long  __stdcall mexSetEqualizer(unsigned __int64 CamGUID,
                                             mexEqualizer* pEQ);
                                            
//! Get properties for the Equalizer wich works if a multi shot image is aquired.
//! Only for ProgResC14Plus / ProgResM14Plus.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[out] pEQ Pointer to an mexEqualizer object with valid settings;
//! \retval ::NOERR If OK.
//! \retval ::NOTIMPLEMENTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \retval ::PARAMERR If pEQ==NULL.
//! \sa \ref page16
CXXPLUS_SPEC long  __stdcall mexGetEqualizer(unsigned __int64 CamGUID,
                                             mexEqualizer* pEQ);
                                            

//! Activates  triggered outputs while an image acquisition 
//! The activation applieds soonest for the next aquisition and up to an other activation request.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pTOut   Pointer to an mexTriggerOut object with valid settings.
//! \retval ::NOERR If OK.
//! \retval ::NOTIMPLEMENTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \retval ::PARAMERR If pTOut==NULL.
//! \note This function is only supported by ProgResC14Plus / ProgResM14Plus.
CXXPLUS_SPEC long  __stdcall mexActivateTriggerOut(unsigned __int64 CamGUID,
                                                   mexTriggerOut* pTOut);
                                                  

//! Set the output pin 1 to the given level. This action happens asynchron to any image acquisition.
//! An application should set the out pin  to an well-defined level before it uses mexActivateTriggerOut.
//! \param[in] CamGUID Global unique identifier of camera
//! \param[in] Level output level
//! \arg \c 0 low  
//! \arg \c 1 high
//! \retval ::NOERR If OK.
//! \retval ::NOTIMPLEMENTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \note This function is only supported by ProgResC14Plus / ProgResM14Plus.
CXXPLUS_SPEC long  __stdcall mexSetTriggerOut(unsigned __int64 CamGUID,
                                              unsigned int Level);
                                             

//! Get the actual level of output pin 1.
//! \param[in] CamGUID Global unique identifier of camera
//! \param[out] pLevel Pointer to an unsigned int, to receive the actual value of the output pin 1.
//! \retval ::NOERR If OK.
//! \retval ::NOTIMPLEMENTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \retval ::PARAMERR If pLevel==NULL.
//! \note This function is only supported by ProgResC14Plus / ProgResM14Plus.
CXXPLUS_SPEC long  __stdcall mexGetTriggerOut(unsigned __int64 CamGUID,
                                              unsigned int* pLevel);
                                             

//! Activates the external triggered image acquisition. The camera waits for an external signal
//! \param[in]  CamGUID Global unique identifier of camera.
//! \param[in]  active  Specifies the wanted trigger condition
//! \arg \c 0 Does not wait for any signal  to start the image acquisition
//! \arg \c 1 Wait for the low signal level to start the image acquisition
//! \arg \c 2 Wait for the high signal level to start the image acquisition
//! \arg \c 3 Wait for falling edge to start the image acquisition
//! \arg \c 4 Wait for rising edge to start the image acquisition
//! \arg \c 5 Wait for any  edge (falling or raising) to start the image acquisition
//! \retval ::NOERR If OK.
//! \retval ::NOTIMPLEMENTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \note This function is only supported by ProgResC14Plus / ProgResM14Plus.
CXXPLUS_SPEC long  __stdcall mexActivateTriggerIn(unsigned __int64 CamGUID,
                                                  unsigned int active); 
                                                 

//! Set the cameras gain to the specified value. The gain controls the analogous amplifier in the 
//! ccd-processing-line of the camera before the A/D-converter.
//! \param[in] CamGUID Global unique identifier of camera
//! \param[in]  Gain 1.0<= Gain <=8.0
//! \retval ::NOERR If OK.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \retval ::NOTSUPPORTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \note 
//! - This function is only supported by ProgResC14Plus / ProgResM14Plus.
//! - The implementaion adjusts the value of Gain if it is not into the range mentioned above.
//! - The implementation maps the value of <b> Gain </b> to the nearest one which the camera  can realize.
//!   Hence if ::mexGetGain is called after an successful call to ::mexSetGain, the reported
//!   realized gain is possibly different from the setted gain.
CXXPLUS_SPEC  long  __stdcall mexSetGain(unsigned __int64 CamGUID,double Gain);

//! Get the cameras currect used gain. The gain is a feature of the harware of the camera.
//! \param[in] CamGUID Global unique identifier of camera
//! \param[out] pGain  1<=gain && gain <=8.0
//! \retval ::NOERR If OK.
//! \retval ::NOCAMERA If CamGUID does not specify an open camera.
//! \retval ::PARAMERR If pGain==NULL.
//! \retval ::NOTSUPPORTED If camera is not of type ProgResC14Plus or ProgResM14Plus.
//! \note This function is only supported by ProgResC14Plus / ProgResM14Plus.
CXXPLUS_SPEC  long  __stdcall mexGetGain(unsigned __int64 CamGUID,double* pGain);

//! Activate/Deactivate gamma-type image processing
//! \param[in] CamGUID Global unique identifier of camera
//! \param[in] pGamma Pointer to a mexGamma object with valid values from caller.
//! \retval ::NOERR If OK
//! \retval ::NOCAMERA If CamGuid does not specify an open camera.
//! \retval ::PARAMERR If pGamma specifies invalid values or pGamma==NULL.
//! \note
//! - For gamma < 1.0 the color values are overrates compared to the linear characteristic.
//! - For gamma > 1.0 the color values are underrates compared to the linear characteristic.
//! - The value of mexGamma::gamma applies  for all color channels uniquely.
CXXPLUS_SPEC  long  __stdcall mexActivateGammaProcessing(unsigned __int64 CamGUID,mexGamma* pGamma);

//! Set a callback function to calculate and report a focus number for any subsequently requested image.
//! The focus number is a measure for the sharpness of the image. The focus number is higher for more sharper
//! images.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pFocus Pointer mexFocus object with valid settings from caller
//! \param[in] FocusUser Generic user value to propagate on any call of  mexFocus::pFocusCallback.
//! \note
//! - To deactivate  focus reports use the value NULL for  mexFocus::pFocusCallback. Then the values of 
//!   mexFocus::roi, mexFocus::bChannelSelect and <b> FocusUser </b>  are ignored.
//! - Take into account that the rectangle specified by mexFocus::roi must fit into all subsequent requested images. 
//!   Hence on the occasion of new acquisition requests possibly the rectangle must be adjusted to the new image format.
//! - Of course if for instance the exposure time only is changed , no adjustment of the rectangle is necessary. 
//! - The callback function is called immediately before ::mexImageFinalProc.
//! - Time measurings for a  mexFocus::roi of 100 x 100 pixel have been resulted in execution times 
//!   from <b> 0.2 to 0.4 milliseconds </b> for a 2.66 GHz Pentium processor. 
//! - The focus number is calulated from a temporary generated histogram for 8 bits per pixel.
//!   Hence the execution time depends on required bit-shift-operations. If shifts are required 
//!   the execution time rises.
//! - Bit-shift-operations are  not required if ::mexImageParams::ccdtransferCode is 2 or 4. See also: \ref page17
CXXPLUS_SPEC  long __stdcall  mexSetFocusCallback(unsigned __int64 CamGUID, 
                                                  mexFocus* pFocus, 
                                                  unsigned long FocusUser);
//! Calculates the optimal exposure time for images requested with parameter values specified by pImgPars.
//! The purpose of this function is to avoid overexposure and even so use nearly the full scale of pixelvalues.
//! The amout of exposure ticks found is propagate to the caller  by a call to <b> pExpCtrl->pExpCallback </b>.
//! There is <b> no automatic use of the found exposure ticks </b>.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pExpCtrl Pointer to a ::mexExposureCtrl object with valid settings from caller.
//! \param[in] pImgPars Pointer to a mexImageParams object with valid settings from caller.
//! \param[in] User Generic user value to propagate on any call of ::mexExpCtrlCB.
//! \retval ::NOERR If OK.
//! \retval ::NOCAMERA If CamGuid does not specify an open camera.
//! \retval ::PARAMERR If something in the parameter list of the call is wrong.
//! \retval ::NOTSUPPORTED if some of the multi-shot acquisition modes is specified for mexImageParams::mode or if a live mode is running. 
//! \note
//! - For ProgResC14Plus or ProGresM14Plus the call for multishot images make no sense because the images are 
//!   to big and this would require long acquisition times and long calculation times. Use instead of them 
//!   ::mexc14_Shot_collowres respectively ::mexm14_Shot_bwlowres.
//! - The call triggers in a single action  multiple image acquisitions to calculate the optimal value for the
//! exposure time. 
//! - On the call the value of mexImageParams::exposureTicks is ignored.
//! - If *pImgPars specifies a progress callback ::mexImageProc!=NULL this callback will be called while
//!   the procedure is running to report the progress state of the procedure.
//! - If *pImgPars specifies a ::mexImageFinalProc !=NULL this callback will be ignored while the procedure is running.
//! - No further actions on following acquisition requests are performed.
//! - The call can take some seconds to provide the result.
//! - The illumination should be <b> timely stable </b> while the action is running.
//! - The resultig exposure time is optimal for all images with nearly equal illumination.
//! - If the optimal exposure time is ordered for a binning mode (for example ::mexc14_Shot_bin3col), the
//!   result is not useful for other acquisition modes. The same is to say for the role of of ::mexImageParams::ccdtransferCode:
//!   If the found exposure ticks are used with different value of ccdtransferCode, this <b> does not result in
//!   optimal exposured images.</b>
CXXPLUS_SPEC  long __stdcall  mexGetOptimalExposureTime(unsigned __int64 CamGUID,
                                                        mexExposureCtrl* pExpCtrl,
                                                        mexImageParams* pImgPars,
                                                        unsigned long User);
//! Activate/Deactivate the automatic adjusting of exposuretime.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pExpCtrl Pointer to a ::mexExposureCtrl object.
//! \param[in] ExpCrtlUser Generic user value to propagate on any call of mexExposureCtrl::pExpCallback.
//! The value is ignored if the callback function is set to NULL.
//! \retval ::NOERR If OK.
//! \retval ::NOCAMERA If CamGuid does not specify an open camera.
//! \retval ::PARAMERR If pExpCtrl==NULL.
//! \note 
//! - The callback function mexExposureCtrl::pExpCallback can be set also (!=NULL) if bActive=FALSE. 
//!   Then for any incoming image the value for used exposure ticks is reported by the callback fuction.
//! - The callback function is called immediately before ::mexImageFinalProc.
//! - It is advisable to use a different callback function compared to ::mexEnableExposureReports..
CXXPLUS_SPEC  long __stdcall  mexActivateExposureControl(unsigned __int64 CamGUID,
                                                         mexExposureCtrl* pExpCtrl, 
                                                         unsigned long ExpCrtlUser);

//! Activate/Deactivate reports of exposure ticks for incoming images.
//! If pCB!=NULL the reports comes in with calls to pCB, else no reports comes in
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pCB Pointer to a callback fucntion of type ::mexExpCtrlCB:
//! \param[in] User Generic user value to propagate on any call of pCB.
//! \retval ::NOERR If OK.
//! \retval ::NOCAMERA If CamGuid does not specify an open camera.
//! \retval ::PARAMERR If pExpCtrl==NULL.
//! \note
//! - It is advisable to use a different callback function compared to ::mexActivateExposureControl.
CXXPLUS_SPEC long  __stdcall  mexEnableExposureReports(unsigned __int64 CamGUID,
                                                       mexExpCtrlCB pCB,
                                                       unsigned long User);

//! Activate/Deactivate saturation control for color images.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] pSC Pointer to a ::mexSaturationCtrl object with valid settings from caller.
//! \retval ::NOERR If OK.
//! \retval ::NOCAMERA If CamGuid does not specify an open camera.
//! \retval ::PARAMERR If pSC==NULL.
//! \note 
//! - An activation request (::mexSaturationCtrl::bActive==TRUE) will be ignored for black/white-images.
//!   If the next acquisition request comes in for a color image, then it will be fulfilled.
//! - The normal case for saturation is: mexSaturationCtrl::sc=<b> 0.0 </b>.
//! - Total desaturation is achieved for mexSaturationCtrl::sc=<b> -1.0 </b> .
//! - Highest saturation is achieved for mexSaturationCtrl::sc=<b> +1.0 </b> .
//! - The function accepts also values <b> 1.0 < sc <= 3.0 </b> but the resulting colors are bad for these values.
//! - The saturation parameter ::mexSaturationCtrl::sc can change while a \link page13 CC-Live mode \endlink (started by 
//!   ::mexStartFastAcquisition or ::mexStartFastAcquisitionEx) is running. The function CxxHost::OnScrollSaturation
//!   shows how to setup a call of ::mexModifyLiveBools to accomplish this task.
//! - For a running \link page13 AC-Live mode \endlink the changed saturation parameter is to take into account for the next call to
//!   ::mexStartAcquisition. Usual at this point is nothing to do to realize the changed saturation value.
//! \attention
//! - Activation of saturation caused the use of a color matrix for image processing just as mexImageParams::bcorrectColors.
//!   If both are activated a product matrix is used, hence the processing time is the same as for one matrix.
//!   <b> Only if both are deactivated processing time is saved.</b> 
//! - The range for ::mexSaturationCtrl::bActive has been change from <b> previously 0,1 </b> to <b> now 0,1,2,3 </b> to allow the control
//!   of used matrix algorithm also in case of ::mexImageParams::bcorrectColors=0.  
CXXPLUS_SPEC long  __stdcall  mexActivateSaturationControl(unsigned __int64 CamGUID,
                                                           mexSaturationCtrl* pSC);
//! \brief Function to compute a colormatrix by rgb samples from a 6  x 4 color calibration chart.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] MonitorType Target monitor type, taken from enumeration ::mexMonitorType.
//! \param[in] rgbchart array of color samples taken from a image of a 6 x 4 color calibration chart.
//! \param[out] colmat Resulting color matrix.
//! \param[out] pCQ Pointer to a mexColorQuality object for the resulting precision.
//! \retval NOERR if OK
//! \retval ::NOCAMERA If CamGuid does not specify an open camera.
//! \retval ::COLORCALBADVALUES If results are bad.
//! \retval ::COLORCALOVEREXPOSED If the values in rgbchart indicates an overexposured image.
//! \note 
//! - The values in rgbchart must be taken from a image of a <b> 6 x 4 color calibration chart </b> .
//! - The arrangement of the values in rgbchart[24][*]  <b> is  column by column </b>, that is first are 4 values for the first (left) column and so on...
//! - \copydoc page7a
//! - The arrangement for the colors in rgbchart  is <b> r,g,b </b> that is 
//!    - <b> rgbchart[0][0] </b> is the <b> red </b> value for the first color field on the chart
//!    - <b> rgbchart[0][1] </b> is the <b> green </b> value for the first color field on the chart.
//!    - <b> rgbchart[0][2] </b> is the <b> blue </b> value for the first color field on the chart.
//!    - and so on.
//! - The index of the <b> white  field </b> in the first column is <b> 3 </b>,  that is rgb[3][*] are the rgb values of the white field.
//! - The colors <b> must be white balanced </b> but without matrix processing (mexImageParams::bcorrectColors=0)
//!   and without gamma processing.
//!   The values in rgbchart should be taken from a image of a color calibration chart for the 24 color fields
//!   by averaging over a region of multiple pixels. The range must be the original range of the camera:
//!   for instance 0 ... 16383 for ProgResC14Plus, 0,...,4095 for ProgResC10Plus, ProgResC12Plus
//! - The resulting precision values in ::mexColorQuality should be at least in range
//!   <b> 0.03 </b> <= mexColorQuality::MaxDiff <= <b> 0.06 </b>.
//! - The quality space for ::mexColorQuality is the <b> LAB color space </b>
//! - For MonitorType the value ::mexMonitortypeSRGB is recommended if no other reasons come into the play.
CXXPLUS_SPEC long  __stdcall  mexCalibrateColors(unsigned __int64 CamGUID,
                                                 mexMonitorType MonitorType,
                                                 double rgbchart[24][3],
                                                 double colmat[3][3],
                                                 mexColorQuality* pCQ);

//! \brief Set the colormatrix to use if ::mexImageParams::bcorrectColors!=0
//!
//! The function replaces the internal standard color matrix to those taken from colmat.
//! \param[in] CamGUID Global unique identifier of camera.
//! \param[in] colmat Color matrix to set. The values should be taken from a call to ::mexCalibrateColors.
//! \retval ::NOERR if OK
//! \retval ::NOCAMERA If CamGuid does not specify an open camera.
//! \note 
//! - To work correct with white balance the row sum for any row of colmat shoud be 1.0
//! - colmat is organized <b> row by row </b>, that is: First are 3 values for the first row,... and so on.
CXXPLUS_SPEC long  __stdcall  mexSetColorMatrix(unsigned __int64 CamGUID,
                                                double colmat[3][3]);
//! \brief Reset the color matrix to the standard internal one.
//!
//! \param[in] CamGUID Global unique identifier of camera.
//! \retval ::NOERR if OK
//! \retval ::NOCAMERA If CamGuid does not specify an open camera.
CXXPLUS_SPEC  long  __stdcall mexResetColorMatrix(unsigned __int64 CamGUID);

#ifdef __cplusplus
	}
#endif

#endif

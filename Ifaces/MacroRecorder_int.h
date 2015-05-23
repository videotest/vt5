#pragma once

//MacroManager
#define szMacroManagerProgID "MacroRecorder.MacroManager"

//MacroHelper
#define szMacroHelperProgID "MacroRecorder.MacroHelper"

// new line (in script)
#define NEW_LINE				"\r\n"
#define MACROMAN_NAME			"MacroManager"
#define MACROHELPER_NAME		"MacroHelper"

#import <MacroRecorder.tlb> exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 

interface IMacroManager : IUnknown
{
	com_call StartRecord( ) = 0;
	com_call ResumeRecord( ) = 0;
	com_call StopRecord( ) = 0;
	com_call SetMacroScript( /*[in]*/ BSTR bstrScript ) = 0;
	com_call GetMacroScript( /*[out]*/ BSTR *pbstrScript ) = 0;
	com_call ExecuteScript( ) = 0; 
    com_call ProcessMessageHandler( /*[in]*/ MSG *pMsg ) = 0;	
	
	com_call IsRecording( /*[out]*/ BOOL *pbRecording ) = 0;
	com_call IsExecuting( /*[out]*/ BOOL *pbExecuting ) = 0;

};

struct CEditedRectangleInfo
{
	RECT rcOld;
	RECT rcNew;
	HWND hwndSaved;
	BSTR bstrWndClass;
};

interface IMacroHelper : IUnknown
{
    com_call ConvertMessageToMacro( /*[in]*/ MSG *pMsg, /*[in]*/ BOOL bStartRecord, /*[out]*/ BSTR *pbstrScript ) = 0;	
	com_call KillAllEvents( ) = 0;
	// If manual correction flag is TRUE then MacroHelper will on execution of each
	// SetEventsWindow2 command, MacroHelper will ask user to correct rectangle.
	// All corrected rectangles will be remembered in internal buffer.
	// Single rectangle in buffer can be obtained by FindEditedRectangle. This buffer can
	// be cleared by ClearListOfExecuted.
	com_call GetManualCorrection(BOOL *pbManualCorr) = 0;
	com_call SetManualCorrection(BOOL bManualCorr) = 0;
	com_call ClearListOfExecuted() = 0;
	com_call FindEditedRectangle(CEditedRectangleInfo *pInfo) = 0;
};


declare_interface( IMacroManager, "62E5D0C6-EE15-41b1-9C2C-DF31D3B353D5" );
declare_interface( IMacroHelper, "2C8FBFD3-EE19-485b-8DBE-EF82D4FDC8EA" );
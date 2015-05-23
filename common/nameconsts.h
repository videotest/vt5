#ifndef __nameconsts_h__
#define __nameconsts_h__

#define DEF_APP_NAME		"vt5"
#define DEF_COMPANY_NAME	"VideoTest"

#define VT_BITS				(VT_UI4)

//plugin directories
#define szPluginScriptNamespace				"script namespace"
#define szPluginAction						"actions"
#define szPluginObject						"named data objects"
#define szPluginApplication					"application"
#define szPluginDocs						"documents"
#define szPluginExportImport				"export-import"
#define szPluginView						"views"
#define szPluginDocument					"anydoc"
#define	szPluginDockableWindows				"dockwnd"
#define	szPluginSplitter					"splitter"
#define szPluginApplicationAggr				"application aggregate"
#define szPluginDocumentsAggr				"anydoc aggregate"
#define szPluginViewAggr					"view aggregate"
#define szPluginColorCnv					"color convertor"
#define szPluginPropertyPage				"property pages"
#define szPluginStatusPane					"statusbar panes"
#define	szPluginObjectPage					"options pages"
#define	szPluginStatistic					"statistics"
#define	szPluginMeasurement					"measurement"
#define	szPluginClasses						"Classes"
#define	szPluginRender						"Renders"
#define	szPluginDefaultRender				"DefaultRender"


#define	szLookup							"Lookup"
#define	szLookupAction						"LookupAction"

#define	szLookupTable						"LookupTable"
#define	szLookupState						"LookupState"

	
#define	szMeasurement						"MeasManager"
#define	szStatistic							"StatManager"
#define	szManualMeasure						"ManualMeasure"
#define	szShiftMode							"ManualMeasShiftMode"
#define	szMeasureColor						"ManualMeasColor"
#define	szMouseMode							"ManualMeasMouseMode"
#define	szMMCrossOffset						"ManualMeasCrossOffset"
#define	szMMArcSize							"ManualMeasArcSize"
#define	szMMSensZoneLen						"ManualMeasSensZoneLen"

#define szIVImageView						"ImageView"
#define szIVShowClassName					"ShowClassName"
#define szIVShowObjectNumber				"ShowObjectNumber"
#define szIVShowObjectNumberPos				"ShowObjectNumberPos"
#define szIVShowClassNamePos				"ShowClassNamePos"

#define	szCalibration						"Calibration"
#define	szCalibrX							"CalibrX"
#define	szCalibrY							"CalibrY"
#define	szMeasUnit							"MeasUnit"
#define	szOffsetX							"OffsetX"
#define	szOffsetY							"OffsetY"
#define	szCurrentUnitName					"Unit"
#define	szCurrentCalibrName					"Name"
#define	szMeterPerUnit						"MeterPerUnit"
#define	szCalibrList						"CalibrList"
#define	szUnitList							"UnitList"


#define szUnkClassName						"UnkClassName"
#define szUnkClassColor						"UnkClassColor"

#define szMultiFrameSignature				"MultiFrameSignature"		
									
#define szTargetAnydoc						"anydoc"
#define szTargetViewSite					"view site"
#define szTargetContext						szTargetViewSite
#define szTargetApplication					"app"
#define szTargetFrame						"frame"
#define szTargetMainFrame					"MainWnd"
//some other targets
#define szDocumentScript					"Configuration"
#define szDocumentImage						"Image"
#define szDocumentDBase						"Database"
#define szDocumentIdioDBase					"IdioDBase"

#define szMenuMain	"main"

#define szArgumentTypeBool					"bool"
#define szArgumentTypeInt					"int"
#define szArgumentTypeDouble				"double"
#define szArgumentTypeString				"string"
#define szArgumentTypeImage					"Image"
//#define szArgumentTypeBinaryImage			"binary"
#define szArgumentTypeObject				"Object"
#define szArgumentTypeScript				"Script"
#define szArgumentTypeObjectList			"Object List"
#define szArgumentTypeClass					"Class"
#define szArgumentTypeClassList				"Class List"
#define szArgumenAXCtrl						"ActiveXCtrl"
#define szArgumenAXForm						"ActiveXForm"	
#define szArgumentTypeCalcParam				"CalcParam"
#define szArgumentTypeManualMeasObject		"ManualMeasObject"
#define szArgumentTypeAuditTrailObject		"AuditTrailObject"
#define szArgumentTypeAuditTrailObjectList	"AuditTrailObjectList"
#define szArgumentTypeBinaryImage			"Binary"
#define szArgumentTypeObjectLight			"Light Object"
#define szArgumentTypeIdioDBase				"IdioDBase"
#define szArgumentTypeSewLI					"Sew Large Images"

#define szArgumenReportCtrl					"Report Control"
#define szArgumenReportForm					"Print Template"	


#define szArgumentTypeMeasParamGroup		"MeasParamGroup"
#define szArgumentTypeAverClassMeasGroup	"AverClassMeasGroup"
#define szArgumentTypeImageAreaMeasGroup	"ImageAreaMeasGroup"

#define szArgumentTypeMeasGroupMgr			"MeasGroupMgr"
#define szArgumentTypeStatGroupMgr			"StatGroupMgr"
										
#define szTypeImage							szArgumentTypeImage
#define szTypeObjectList					szArgumentTypeObjectList
#define szTypeObject						szArgumentTypeObject
#define szTypeClassList						szArgumentTypeClassList
#define szTypeClass							szArgumentTypeClass
#define szTypeAXCtrl						szArgumenAXCtrl
#define szTypeAXForm						szArgumenAXForm
#define szTypeManualMeasObject				szArgumentTypeManualMeasObject
#define szTypeAuditTrailObject				szArgumentTypeAuditTrailObject
#define szTypeBinaryImage					szArgumentTypeBinaryImage
#define szTypeLightObject					szArgumentTypeObjectLight
#define szTypeIdioDBase						szArgumentTypeIdioDBase
#define szTypeSewLI							szArgumentTypeSewLI
										
#define szTypeReportCtrl					szArgumenReportCtrl
#define szTypeReportForm					szArgumenReportForm
										
										
#define szTypeCalcParam						szArgumentTypeCalcParam
#define szTypeMeasParamGroup				szArgumentTypeMeasParamGroup
#define szTypeAverClassMeasGroup			szArgumentTypeAverClassMeasGroup
#define szTypeImageAreaMeasGroup			szArgumentTypeImageAreaMeasGroup

#define szTypeMeasGroupMgr					szArgumentTypeMeasGroupMgr
#define szTypeStatGroupMgr					szArgumentTypeStatGroupMgr

#define szTypeMarker						"Marker"
#define szTypeArrow							"Arrow"
#define szDrawingObject						"Drawing Object"
#define szDrawingObjectList					"Drawing Object List"

#define szStdSplitterProgID					"StdSplitter.SplitterWindow"
#define szNotifyCtrlProgID					"NotifyContr.NotifyController"
#define	szNamedDataProgID					"Data.NamedData"
#define	szContextProgID						"Data.DataContext"
#define	szPrintViewProgID					"Print.PrintView"

//event strings
#define	szEventBeforeSave			"BeforeSave"
#define	szEventAfterSave			"AfterSave"
#define	szEventBeforeOpen			"BeforeOpen"
#define	szEventAfterOpen			"AfterOpen"
#define	szEventAfterLoadObject		"AfterLoadObject"
#define	szEventBeforeClose			"BeforeClose"
#define szEventAfterActionExecute	"AfterActionExecute"
#define szEventBeforeActionExecute	"BeforeActionExecute"
#define szEventActiveObjectChange	"ActiveObjectChange"
#define szEventActiveContextChange	"ActiveContextChange"
#define szEventContextSelectionChange	L"ObjectSelectionChange"
#define szEventActiveContextChangeObjectKey		"ActiveContextChangeObjectKey"
#define szEventActiveAppChange		"ActiveAppChange"
#define szEventActiveMethodicChange	"ActiveMethodicChange"
#define szEventRenameMethodic		"RenameMethodic"
#define szEventRenameApp			"RenameApp"
#define szEventChangeObjectList		"ChangeObjectList"
#define	szBeforeAppTerminate		"BeforeAppTerminate"
#define	szActivateView				"ActivateView"
#define	szAppActivateView			"AppActivateView"
#define szAppCreateView				"AppCreateView"
#define szAppDestroyView			"AppDestroyView"
#define	szContextRescan				"ContextRescan"
#define	szAfterContextRescan		"AfterContextRescan"
#define	szActiveTargetChanged		"ActiveTargetChanged"
#define	szNeedUpdateView			"NeedUpdateView"
#define	szEventChangeProperties		"PropertiesChanged"
#define	szEventActivateObject		"ActivateObject"
#define szEventChangeObject			"ChangeObject"
#define szEventSetScriptActiveLine	"SetScriptActiveLine"
#define szEventCreateTypedObject	"CreateTypedObject"
#define szEventManageColorInterval	"ManageColorInterval"
#define	szEventScrollPosition		"EventScrollPosition"
#define	szEventManageParameter		"ManageParameter"
#define	szEventChangeBaseObject		"ChangeBaseObject"

#define	szEventBeforeTestExecute	"BeforeTestExecute"
#define	szEventAfterTestExecute		"AfterTestExecute"
#define	szEventBeginTestProcess		"BeginTestProcess"
#define	szEventEndTestProcess		"EndTestProcess"
#define szEventTestsChangeState		"TestsChangeState"
#define szEventTerminateTestProcess	"TerminateTestProcess"

#define szEventBeforeRecordMethodStep		"BeforeRecordMethodStep"
#define szEventAfterRecordMethodStep		"AfterRecordMethodStep"

#define szEventAppOnIdle		"AppOnIdle"
#define szEventAppOnMainLoop		"AppOnMainLoop"

#define	szEventAbortLongOperation			"AbortLongOperation"


//all objects should update our settings
#define szEventNewSettings			"NewSettings"	
#define szEventAXInsertControlDone	"AXInsertControlDone"	
#define szEventAfterInputImage		"AfterInputImage"
#define szEventNewSettingsView			"NewSettingsForView"	
//fired when user try to remove object from data, but it is not here
#define szEventNoDataObject			"ObjectNotInData"
// changed properties in Named property bag
#define szEventNamedPropsChanged    "NamedPropertiesChanged"
// changed method (changed active, add/delete/...)
#define szEventChangeMethod		"ChangeMethod"
// (reserved) changed method step in active method
#define szEventChangeMethodStep		"ChangeMethodStep"

#define szEventFormOnOk			"FormOnOk" // при нажатии ok в форме
#define szEventFormOnCancel		"FormOnCancel" // при нажатии cancel в форме
#define szEventFormOnInitialize	"FormOnInitialize" // при инициализации формы
#define szEventFormOnDestroy	"FormOnDestroy" // при смерти формы

#define szEventMainFrameOnEnterMode	"MainFrameOnEnterMode" // ѕри включении Settings Mode (показ формы)
#define szEventMainFrameOnLeaveMode	"MainFrameOnLeaveMode" // ѕри выключении Settings Mode (завершение формы)

#define szEventChangePreview	"ChangePreview" // when filter preview changed (fired in ActionHelper)

// flags for szEventActiveContextChange
#define ADD_OBJECT			0x01
#define REMOVE_OBJECT		0x02
#define ACTIVATE_OBJECT		0x04
#define DEACTIVATE_OBJECT	0x08// = ACTIVATE_OBJECT w/ object == NULL
#define SELECT_OBJECT		0x0f
#define UNSELECT_OBJECT		0x10
#define CHANGE_BASE_OBJECT	0x11
#define UPDATE_SELECTION	0x12
#define UPDATE_TREE			0xff



//WM_NOTIFYLONGOPERATION
//lparam is pointer to the ILongOperation unknown
//wparam is action
enum LongOperationAction
{
	loaStart,
	loaSetPos,
	loaTerminate,
	loaComplete
};

#define WM_ACTIVATEPANE			(WM_USER+777)
#define WM_NOTIFYLONGOPERATION	(WM_USER+778)	
#define WM_GETINTERFACE			(WM_USER+779)

#define ID_CMDMAN_BASE	1000
#define ID_CMDMAN_MAX	1999

#define ID_FIRST_DOCKBAR	(ID_CMDMAN_BASE + 1000)
#define ID_LAST_DOCKBAR		(ID_CMDMAN_BASE + 1099)
#define ID_FIRST_TOOLBAR	(ID_CMDMAN_BASE + 1100)
#define ID_LAST_TOOLBAR		(ID_CMDMAN_BASE + 1199)
#define ID_CMD_POPUP_MIN	(ID_CMDMAN_BASE + 1200)
#define ID_CMD_POPUP_MAX	(ID_CMDMAN_BASE + 1299)
#define ID_OLE_VERB_MIN		(ID_CMDMAN_BASE + 1300)
#define ID_OLE_VERB_MAX		(ID_CMDMAN_BASE + 1399)

#define ID_CLASSMENU_BASE   (ID_CMDMAN_BASE + 1400)
#define ID_CLASSMENU_MAX    (ID_CMDMAN_BASE + 1499)


#define GRIDCTRL_CLASSNAME    _T("MFCGridCtrl")  // Window class name
#define TITLETIP_CLASSNAME    _T("VTTitleTip")
#define CONTEXTCTRL_CLASSNAME    _T("MFCContextCtrl")  // Window class name
#define END_OF_MLS			"/end of string"


enum MatchFile
{
	mfNoAttempt,
	mfMaybeAttemptForeign,
	mfMaybeAttemptNative,
	mfYesAttemptForeign,
	mfYesAttemptNative,
	mfYesAlreadyOpen
};


//view's progid
#define szBinaryContourViewProgID "ImageDoc.BinaryContour"
#define szBinaryViewProgID "ImageDoc.Binary"
#define szBinaryBackViewProgID "ImageDoc.BinaryBack"
#define szBinaryForeViewProgID "ImageDoc.BinaryFore"
#define szObjectsViewProgID "ImageDoc.ObjectsView"
#define szMaskViewProgID "ImageDoc.MaskView"
#define szImageViewProgID "ImageDoc.ImageView"
#define szPseudoViewProgID "ImageDoc.PseudoView"
#define szSourceViewProgID "ImageDoc.SrcView"
#define szIdioDBViewProgID "IdioDBase.IdioDBView"


const char
 c_szCAuditTrailSingleView[] = "AuditTrailSingleView",
 c_szCAuditTrailTextView[] = "AuditTrailTextView",
 c_szCAuditTrailView[] = "AuditTrailView",
 c_szCAVIView[] = "Video_View",
 c_szCClassView[] = "Class_View",
 c_szCMeasPropPage[] = "Shift",
 c_szMeasView[] = "Meas_View",
 c_szCDBaseGridView[] = "DBaseGridView",
 c_szCFilterPropPage[] = "Filter_Conditions",
 c_szCGalleryView[] = "GalleryView",
 c_szCBinaryForeView[] = "BinaryForeView",
 c_szCBinaryBackView[] = "BinaryBackView",
 c_szCBinaryView[] = "BinaryView",
 c_szCBinaryContourView[] = "BinaryContourView",
 c_szCImagePage[] = "Image",
 c_szCImageSourceView[] = "SrcView",
 c_szCImageView[] = "ContourView",
 c_szCObjectsView[] = "ObjectsView",
 c_szCMasksView[] = "MaskView",
 c_szCPseudoView[] = "PseudoView",
 c_szCCropPageDlg[] = "Crop",
 c_szCMeasPage[] = "Manual_measure",
 c_szCObjectEditorPage[] = "Editor",
 c_szCFixedEllipsePage[] = "Fixed_Ellipse",
 c_szCFixedRectPage[] = "Fixed_Rect",
 c_szCMagickObjectPage[] = "Magic",
 c_szCEditObjectPage[] = "Object_Editor",
 c_szCObjectFreePage[] = "Free_Object",
 c_szCObjectGeneralPage[] = "Object_General",
 c_szCAXInsertPropPage[] = "Insert_ActiveX",
 c_szCAxView[] = "ActiveXView",
 c_szCCmpPage[] = "Compare_Properties",
 c_szCModePage[] = "Mode",
 c_szCFourierPage[] = "FourierPage",
 c_szCIPreviewView[] = "Preview_View",
 c_szCPrintView[] = "PrintView",
 c_szCArrowPage[] = "Arrows",
 c_szCCircle1PropPage[] = "Circle",
 c_szCCirclePropPage[] = "Circle_old",
 c_szCColorPickerPropPage[] = "Color_Picker",
 c_szCDodge1PropPage[] = "Dodge_Burn",
 c_szCCommentFontPropPage[] = "Font",
 c_szCLine1PropPage[] = "Line",
 c_szCRect1PropPage[] = "Rectangle",
 c_szCTextPropPage[] = "AutoText",
 c_szCSelPropPage[] = "Selection",
 c_szCZoomNavigatorPage[] = "Zoom_Navigator",
 c_szCFloodFillPropPage[] = "FloodFill",
 c_szCLineBinPropPage[] = "BinLine",
 c_szCompositeEditorPropPage[] = "CompositeEditor",
 c_szPhaseEditorPropPage[] = "PhaseEditor",
 c_szBinEditorPropPage[] = "BinarySelectEditor", 
 c_szCSelFixedRectPropPage[] = "FixedRectSelection";
	


#endif // __nameconsts_h__
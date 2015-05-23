#if !defined _constant_h
#define _constant_h

#define szViewAXProgID  "VIEWAX.ViewAXCtrl.1"


#define SD_ACTIVE_OBJECT "ActiveObject"
#define SD_ACTIVE_VIEW_TYPE "ActiveViewType"


#define ISOK(f)		VERIFY(SUCCEEDED(f))

extern char g_szPrintPreviewObject[255];

#define WM_PRINT_PROGRESS					WM_USER + 1000


#define REPORT_TEMPLATE_SECTION				"\\TemplateSettings"

#define REPORT_TEMPLATE_SOURCE				"TemplateSource"
#define REPORT_EXIST_TEMPLATE_NAME			"ExistTemplateName"

#define REPORT_AUTO_TEMPLATE_ALGHORITM		"AutoTemplateAlghoritm"


#define REPORT_NEW_TEMPLATE_TARGET			"TemplateGenerateTarget"

#define REPORT_NEW_TEMPLATE_FILENAME		"NewTemplateFileName"
#define REPORT_NEW_TEMPLATE_OBJECTNAME		"NewTemplateObjectName"

#define REPORT_CTRL_WORK_AREA_ALIGN_MM		5


#define GENERATE_REPORT_SECTION				"AutoReport"
#define GALLERY_HORZ_ITEM_COUNT				"GaleryHorzItemCount"




enum BuildMode
{
	bmActiveObject = 0,
	bmMultiSelection = 1
};




#endif //_constant_h 
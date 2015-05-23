#ifndef _DATAVIEW_DEF_H
#define _DATAVIEW_DEF_H

enum EGridCellType
{
	egctNumber = 0,
	egctClassColor,
	egctClassName,
	egctImage,
	egctRegularParam,
	egctManualParam,
	egctResult,
	egctText,
	egctMeasUnit
};

enum EResultValue
{
	ervAver = 0,
	ervNumber,
	ervSKO
};

// predefine 
#define szShowResultBar		"ShowResultBar"
#define szShowResult		"ShowResult"
#define szResultName		"ParamName"
#define szResultFormat		"ParamFormat"
#define szShowColumn		"ShowColumn"
#define szShowRow			"ShowRow"
#define szRowName			"RowName"
#define szShowStatLine		"ShowStatLine"

#define szHeightCount		"HeightCount"
#define szHeightArray		"HeightArray"
#define szWidthCount		"WidthCount"
#define szWidthArray		"WidthArray"
#define szViewMode			"ViewMode"
#define szDefCellHeight		"DefCellHeight"
#define szDefCellWidth		"DefCellWidth"

#define szLinearColor		"LinearColor"
#define szAngleColor		"AngleColor"
#define szCounterColor		"CounterColor"
#define szUndefineColor		"UndefineColor"
#define szBackColor			"BackColor"




#endif// _DATAVIEW_DEF_H
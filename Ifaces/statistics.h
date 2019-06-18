#ifndef __statistics_h__
#define __statistics_h__

#include "defs.h"

//////////////////////////////////////////////////////////////////////
// Statistics Table Object

//Object Type Name
//////////////////////////////////////////////////////////////////////
#define szTypeStatTable "StatTable"
#define szTypeStatObject "StatObject"


// parameter type info
//////////////////////////////////////////////////////////////////////
struct stat_param
{
	long	lkey;
};

// row type info
//////////////////////////////////////////////////////////////////////
struct stat_row
{
	IUnknown*	punk_image;
	GUID		guid_group;
	GUID		guid_object;
};

// value type info
//////////////////////////////////////////////////////////////////////
struct stat_value
{
	double	fvalue;
	bool	bwas_defined;
};

//send part
struct stat_group
{
	unsigned	size;
	unsigned	mask;	
	//sending guid and time
	GUID		guid_group;
	SYSTEMTIME	time_send;
	//document
	GUID		guid_doc;
	BSTR		bstr_doc_name;
	//image
	GUID		guid_image;
	BSTR		bstr_image_name;
	//object_list
	GUID		guid_object_list;
	BSTR		bstr_object_list_name;
	//user data
	BSTR		bstr_user_data;
};

#define SSIF_GUID_SEND	( 1 << 0 )
#define SSIF_TIME_SEND	( 1 << 1 )
#define SSIF_GUID_DOC	( 1 << 2 )
#define SSIF_DOC_NAME	( 1 << 3 )
#define SSIF_GUID_IMG	( 1 << 4 )
#define SSIF_IMG_NAME	( 1 << 5 )
#define SSIF_GUID_OL	( 1 << 6 )
#define SSIF_OL_NAME	( 1 << 7 )
#define SSIF_USER_DATA	( 1 << 8 )


//Base interface for Stat Table Object
interface IStatTable : public IUnknown
{
	//parameter info
	com_call GetFirstParamPos( long* plpos ) = 0;
	com_call GetNextParam( long* plpos, stat_param** ppparam ) = 0;
	com_call GetParamsCount( long* pl_count ) = 0;
	com_call AddParam( long lpos_insert_after, stat_param* pparam, long* pl_pos_new ) = 0;
	com_call DeleteParam( long lpos ) = 0;	
	com_call GetParamPosByKey( long lkey, long* pl_pos ) = 0;

	//group
	com_call GetFirstGroupPos( long* plpos ) = 0;
	com_call GetNextGroup( long* plpos, stat_group** ppgroup ) = 0;
	com_call AddGroup( long lpos_insert_after, stat_group* pgroup, long* pl_pos_new ) = 0;
	com_call DeleteGroup( long lpos, BOOL bsync_rows ) = 0;	
	com_call GetGroupCount( long* pl_count ) = 0;

	//row info
	com_call GetFirstRowPos( long* pl_pos ) = 0;
	com_call GetNextRow( long* pl_pos, stat_row** pprow ) = 0;	
	com_call GetRowCount( long* pl_count ) = 0;	
	com_call AddRow( long lpos_insert_after, stat_row* prow, long* pl_pos_new ) = 0;
	com_call DeleteRow( long lpos_row ) = 0;

	//value by param
	com_call GetValue( long lpos_row, long lpos_param, stat_value** ppvalue ) = 0;
	com_call SetValue( long lpos_row, long lpos_param, stat_value* pvalue ) = 0;
	
	//value by key
	com_call GetValueByKey( long lpos_row, long lkey, stat_value** ppvalue ) = 0;
	com_call SetValueByKey( long lpos_row, long lkey, stat_value* pvalue ) = 0;

	com_call ClearCache( ) = 0;
};

#define SF_SERIALIZE_NAMED_DATA	1

interface IStatTablePersist : public IUnknown
{
	com_call LoadData( IStream *pStream, long lparam ) = 0;
	com_call StoreData( IStream *pStream, long lparam ) = 0;
};

//Base parameters leave
//\BaseParameters\Image\{IMAGE_GUID}\....
//\BaseParameters\ObjectList\{OBJECTLIST_GUID}\....
//see \common\params.h
#define BASE_PARAMS_IMAGE		"Image"
#define BASE_PARAMS_OBJECTLIST	"ObjectList"

enum ViewType
{
	vtChartView = 1 << 0,
	vtLegendView = 1 << 1,
	vtTableView = 1 << 2
};

interface IStatObjectView : public IUnknown
{
	com_call ShowView( DWORD dwView ) = 0;
	com_call GetViewVisibility( DWORD *pdwView ) = 0;
};

//grid store/load state from StatTable NamedData
#define STAT_TABLE_ENTRY	"\\StatTable"
#define STAT_TABLE_GRID		"\\StatTable\\GridOptions"
#define SECTION_ORDER		"\\measurement\\Order"
#define ST_KEY_ORDER		"KeyOrder"

#define ST_COL_KEY			"Key"
#define ST_COL_WIDTH		"Width"
#define ST_COL_VISIBLE		"Visible"

#define STAT_TABLE_FILTER	"\\StatDataTable\\FilterStatTable"


interface IStatTableView : public IUnknown
{	
	com_call GetColumnsCount( long* plcount ) = 0;
	com_call GetActiveColumn( long* plcolumn ) = 0;
	com_call HideColumn( long lcolumn ) = 0;
	com_call ShowAllColumns( ) = 0;
	com_call CustomizeColumns( ) = 0;	
};
 

//////////////////////////////////////////////////////////////////////
// Statistics UI here...


//////////////////////////////////////////////////////////////////////
//Interface declarations
declare_interface( IStatObjectView, "4E2588E1-D6C3-40f3-9671-75BA621698EC" );
declare_interface( IStatTable, "512A0035-A17F-4f5f-B913-EBD5CC39B2B8" );
declare_interface( IStatTablePersist, "06F8F0A1-9CF4-49a8-A5D3-4408E4249144" );
declare_interface( IStatTableView, "B3795256-06C5-40ef-8452-682AE7086B07" );


interface IStatObject : public IUnknown
{	
	// Расчитать статистику
	com_call Calculate( IUnknown *punkO, long lClassDivision ) = 0;
	com_call GetParamInfo( long lKey, double *pfCoef, BSTR *pbstrUnit ) = 0;

	com_call SetLongAction( IUnknown *punkAction ) = 0;
	com_call SetValidateLock( BOOL bLOCK  ) = 0;
	com_call GetValidateLock( BOOL *pbLOCK  ) = 0;
};

enum	CalcFlags
{
	cfValidateIfNoClass = 1 << 0, // флаг расчета статистики, если нет ни одного отклассифицированного объекта
	cfCalcUserParamsInUnits = 1 << 1,// флаг расчета пользовательских стат. параметров в текущих единицах измерения (мм, микроны и т.д.),
									// если флаг не установлен - расчет прозводиться в метрах
};

interface IStatObject2 : public IStatObject
{
    // dwFlags - комбинация установок из CalcFlags 
	com_call Calculate2( IUnknown *punkO, long lClassDivision, DWORD dwFlags ) = 0;
};

declare_interface( IStatObject, "EBFB056E-AF77-404f-A367-0387410246AD" );
declare_interface( IStatObject2, "F58858AB-2ABF-4146-9753-B5526F29DF2B" );

#endif//__statistics_h__
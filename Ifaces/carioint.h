#ifndef __cario_int_h__
#define __cario_int_h__

//cario object section to save in object, that support INamedData iface.

#define	CARIO_ROOT						"\\Karyo"
#define	CARIO_WAS_BUILD					"KaryoWasBuild"
#define	CLASS_FILE_NAME					"ClassFileName"


#define	KARYO_IDIODB					"\\Karyo\\KaryoIdioDB\\Lines"
#define	CARIO_PERVIEW					"\\Karyo\\KaryoPreview\\Lines"
#define	CARIO_ANALIZE					"\\Karyo\\KaryoAnalize\\Lines"

#define CARIO_VIEW_SHARED				"\\Karyo\\KaryoViewSharedSettings"
#define KARIO_IDIODB_PREVIEW			"\\Karyo\\KaryoIdioDB\\Settings"
#define KARIO_IDIODB_PREVIEW_LINES		"\\Karyo\\KaryoIdioDB\\Settings\\LinesInfo"
#define CARIO_VIEW_PREVIEW				"\\Karyo\\KaryoPreview\\Settings"
#define CARIO_VIEW_ANALIZE				"\\Karyo\\KaryoAnalize\\Settings"
#define CARIO_VIEW_IDIOEDIT				"\\Karyo\\KaryoIdioEdit\\Settings"

#define CARIO_COUNT_PREFIX				"Count"
#define CARIO_LINE_PREFIX				"Line"
#define CARIO_CELL_PREFIX				"Cell"
#define CARIO_CELLS_PREFIX				"Cells"
#define CARIO_HIGHT_PREFIX				"Height"
#define CARIO_WIDTH_PREFIX				"Width"
#define CARIO_CELLAR_PREFIX				"Cellar"
#define CARIO_CHROMO_PREFIX				"Chromo"
#define CARIO_CHROMOS_PREFIX			"Chromos"
#define CARIO_CLASS_NUM_PREFIX			"ClassNum"
#define CARIO_OFFSETX_PREFIX			"OffsetX"
#define CARIO_OFFSETY_PREFIX			"OffsetY"
#define CARIO_GUID_CHROMO_PREFIX		"GuidChromo"
#define CARIO_GUID_OBJECT_LIST_PREFIX	"GuidObjectList"
#define CARIO_YOFS_OBJECT				"YOfsCentromere"
#define CARIO_ZOOM_OBJECT				"ZoomObject"
#define CARIO_SHOW_OBJECT				"ShowObject"
#define CARIO_SHOW_IDIO					"ShowIdio"
#define CARIO_SHOW_CGHBRIGHT			"ShowCGHBright"
#define CARIO_SHOW_CGHRATIO				"ShowCGHRatio"
#define CARIO_SHOW_CGHAMPL				"ShowCGHAmpl"

#define CARIO_POS_CHROME				"PosChromo"
#define CARIO_POS_BRIGHT				"PosBright"
#define CARIO_POS_IDIO					"PosIdio"
#define CARIO_POS_RATIO					"PosRatio"

/*For Idio Edit */
#define CARIO_SHOW_CLASSES					"ShowClassesOnKaryo"
#define CARIO_POS_E_IDIO					"PosEditableIdio"
#define CARIO_SHOW_NUMS						"ShowNums"
#define CARIO_SHOW_EDITABLE_NUMS			"ShowEditableNums"
#define CARIO_ERECT_AFTER_DROP				"ErectAfterDrop"
#define CARIO_EDITABLE_IDIO					"ShowEditableIdio"
#define CARIO_ADD_LINES						"ShowAdditionLines"
#define CARIO_ADD_LINES_COLOR				"AdditionLinesColor"

#define KARYO_IDIO_EDIT_HOTZONE "IdioEditHotZone"

#define KARYO_IDIOWIDTH "IdioWidth"
#define KARYO_IDIOHEIGHT "IdioHeight"

/*For Idio Edit */

#define CARIO_CX_RATIO					"RatioCX"
#define CARIO_CX_BRIGHT					"BrightCX"
#define CARIO_CX_AMPL					"AmplCX"

#define PREVIEW_DRAWING_PATH			"\\Drawings\\Preview"
#define ANALIZE_DRAWING_PATH			"\\Drawings\\Analize"
#define IDIODB_DRAWING_PATH				"\\Drawings\\IdioDB"

//event
#define szEventCarioChange	"eventCarioChange"
#define szEventCarioBuild	"eventCarioBuild"

interface ICarioObject : public IUnknown
{	
	com_call InitFromObjectList( IUnknown* punkObjectList ) = 0;		
};

enum CarioViewMode
{
	cvmNotDefine	= 0,//default
	cvmPreview		= 1,
	cvmDesign		= 2,
	cvmCellar		= 3,
	cvmMenu			= 4,
	cvmIdioDB		= 5,
};

#define GCR_ALL			0
#define GCR_CHROMOSOME	1
#define GCR_IDIOGRAM	2
#define GCR_GRAPH		3

interface ICarioView : public IUnknown
{	
	com_call SetMode( CarioViewMode viewMode ) = 0;
	com_call GetMode( CarioViewMode* pViewMode ) = 0;	

	com_call GetLineCount( int* pnLineCount ) = 0;
	com_call GetCellCount( int nLine, int* pnCellCount ) = 0;

	com_call GetActiveLine( int* pnLine ) = 0;
	com_call GetActiveCell( int* pnCell ) = 0;	

	com_call AddLineAtPos( int nLine ) = 0;
	com_call AddCellAtPos( int nLine, int nCell ) = 0;
	
	com_call DeleteLine( int nLine ) = 0;
	com_call DeleteCell( int nLine, int nCell ) = 0;
	com_call DeleteObj( int nLine, int nCell, int nObj ) = 0;

	com_call GetActiveChromo( int* pnLine, int* pnCell, int* pnChromo ) = 0;
	com_call SetActiveChromo( int nLine, int nCell, int nChromo ) = 0;
	//rotating
	com_call SetChromoAngle( int nLine, int nCell, int nChromo, double fAngle ) = 0;
	com_call GetChromoAngle( int nLine, int nCell, int nChromo, double* pfAngle ) = 0;
	//Idiogramm
	com_call GetShowChromoIdiogramm( int nLine, int nCell, int nChromo, BOOL* pbShow ) = 0;
	com_call SetShowChromoIdiogramm( int nLine, int nCell, int nChromo, BOOL bShow ) = 0;
	com_call SetShowChromoAllIdiogramm( BOOL bShow ) = 0;
	//zoom
	com_call GetChromoObject( int nLine, int nCell, int nChromo, IUnknown** ppunk ) = 0;	
	com_call SetChromoZoom( int nLine, int nCell, int nChromo, double fZoom ) = 0;		
	com_call GetChromoZoom( int nLine, int nCell, int nChromo, double *fZoom ) = 0;		
	com_call GetChromoPath( int nLine, int nCell, int nChromo, char **pstr ) = 0;		
	//mirror
	com_call SetChromoMirror( int nLine, int nCell, int nChromo, int bMirror ) = 0;
	com_call GetChromoMirror( int nLine, int nCell, int nChromo, int *bMirror ) = 0;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A.M.
	com_call GetCarioSizes( int* pcx, int* pcy) = 0;
	com_call GetCellByPoint( POINT ptObj, int *pnLine, int *pnCell, int *pnChromoObj, BOOL bTakeLineHeight ) = 0;
	com_call GetCellClass( int nLine, int nCell, int *pnClass ) = 0;
	com_call SetActiveLine( int nLine ) = 0;
	com_call SetActiveCell( int nCell ) = 0;
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	com_call StoreState( IUnknown *punkData ) = 0;
	com_call ArrageObjectsByClasses() = 0;
	com_call MirrorChromo( int nLine, int nCell, int nChromo ) = 0;
	com_call GetChromoRect( long code, int nLine, int nCell, int nChromo, RECT *prect ) = 0; 
	com_call GetShowChromoAllIdiogramm( BOOL *pbShow ) = 0;
	com_call GetShowChromoAddLinesCoord( int nLine, int nCell, int nChromo, int nIndex, double *fPtX, double *fPtY, double *fPtX2, double *fPtY2 ) = 0;
	com_call IsEmpty() = 0;
};

enum IdioType
{
	idtInserted = 1,
	idtEditable = 2,
};

interface ICarioView2 : public ICarioView
{	
	//Brightnes profile
	com_call SetShowChromoAllBrightness( BOOL bShow ) = 0;
	com_call GetShowChromoAllBrightness( BOOL *pbShow ) = 0;

	//FITC/TRITC ratio profile
	com_call SetShowChromoAllRatio( BOOL bShow ) = 0;
	com_call GetShowChromoAllRatio( BOOL *pbShow ) = 0;

	//Amplification and Delition
	com_call SetShowChromoAllAmplification( BOOL bShow ) = 0;
	com_call GetShowChromoAllAmplification( BOOL *pbShow ) = 0;

	com_call SetShowAllChromoObject( BOOL bShow ) = 0;
	com_call GetShowAllChromoObject( BOOL *pbShow ) = 0;

	com_call SetShowAllEditableIdio( BOOL bShow ) = 0;
	com_call GetShowAllEditableIdio( BOOL *pbShow ) = 0;

	com_call GetIdioObject(  int nLine, int nCell, int nChromo, IdioType idt,  IUnknown **punkIdio ) = 0;

	com_call SetErectedState(  int nLine, int nCell, int nChromo, bool bState ) = 0;
	com_call GetErectedState(  int nLine, int nCell, int nChromo, bool *bState ) = 0;
	com_call SetErectedStateAll( bool bState ) = 0;

	com_call SetShowAllChromoAddLines(BOOL bObj ) = 0;
	com_call GetShowAllChromoAddLines(BOOL *bObj ) = 0;

	com_call GetChromoOffsets( int nLine, int nCell, int nChromo, double *pfZoom, long *pnOffset  ) = 0;
};

interface ICarioVDnD : public IUnknown
{	
	com_call GetUndoData( BSTR *pBstr, void *pUndoData, void *pRedoData ) = 0;
};

declare_interface( ICarioObject, "6DE3570C-5EDF-4064-B84D-F9838BB55AA9" );
declare_interface( ICarioView, "F10C3739-592B-4e2a-B876-E69442F239D1" );
declare_interface( IIdioEditView, "659A23D8-491E-403b-B28C-7CB433FDA0A5" );
declare_interface( ICarioView2, "2C86DBF1-DDC8-4371-AEB1-4DAECBB46A17" );
declare_interface( ICarioVDnD, "8811C190-D226-40fb-AC9B-6AD709D289CE" );


#endif//__cario_int_h__
#pragma once

//------------------------------------------------------------------
#include "params.h"

#define STAT_VERSION "StatVersion"
#define STAT_VERSION_NUM 6L

//------------------------------------------------------------------
#define SECT_STATUI_ROOT		"\\StatUI"				// Общая статистическая секция
#define SECT_DEPENDENCE_ROOT	"\\StatDependence"		// Cекция категории зависимости
#define	SECT_STATCMP_ROOT		"\\StatCompare"			// Секция категории сравнения

#define CALC_FOR_GROUPS "CalcForGroups"			// для каких изображений расчитывать
#define CALC_FOR_ALL_CLASSES "CalcForAllClasses"			// для каких классов расчитывать ВСЕГО
#define CALC_FOR_RELATIVE_CLASSES "CalcForRelativeClasses"			// для каких классов расчитывать отнасительные параметры

#define SPLITTER_TYPE	"SplitterType"		// Вид сплиттера
#define PANE0_DATA		"Pane0_Data"		// Данные в пане 0
#define PANE1_DATA		"Pane1_Data"		// Данные в пане 1
#define PANE2_DATA		"Pane2_Data"		// Данные в пане 2
#define CLASS_FILE		"ClassFile"			// Данные в пане 2
#define	STATTABLE_OWN	"OwnStatTable"		// Guid таблицы данных, по которому была создана статистика
//------------------------------------------------------------------
#define FORM_NAME_TABLE_AREA "TableStatArea_PropertyScript" // Название скрипт свойств «таблица статистики»
#define FORM_NAME_LEGEND_AREA "StatParamArea_PropertyScript" // Название скрипт свойств «параметров статистики»
#define FORM_NAME_CHART_AREA "ChartArea_PropertyScript" // Название скрипт свойств «поля графика»
#define FORM_NAME_AXIS_AREA "AxisArea_PropertyScript" // Название скрипт свойств осей
#define FORM_NAME_XPARAM_AREA "X_ParamsArea_PropertyScript" // Название скрипт свойств параметров для оси X
#define FORM_NAME_YPARAM_AREA "Y_ParamsArea_PropertyScript" // Название скрипт свойств параметров для оси Y

#define INDEX_TABLE_AREA "TableStatArea_ImgIndex" // Индекс картинки для кнопки вызова свойств «таблица статистики»
#define INDEX_LEGEND_AREA "StatParamArea_ImgIndex" // Индекс картинки для кнопки вызова свойств «параметров статистики»
#define INDEX_CHART_AREA "ChartArea_ImgIndex" // Индекс картинки для кнопки вызова свойств «поля графика»
#define INDEX_AXIS_AREA "AxisArea_ImgIndex" // Индекс картинки для кнопки вызова свойств осей
#define INDEX_XPARAM_AREA "X_ParamsArea_ImgIndex" // Индекс картинки для кнопки вызова свойств параметров для оси X
#define INDEX_YPARAM_AREA "Y_ParamsArea_ImgIndex" // Индекс картинки для кнопки вызова свойств параметров для оси Y

#define ENABLE_CHART_AREA		"Enable_ChartArea"     // Включить зону свойств «поля графика». Значение: [0;1] Def: 1
#define ENABLE_STAT_PARAM_AREA	"Enable_StatParamArea" // Включить зону свойств «параметров статистики». Значение: [0;1] Default: 1
#define ENABLE_TABLE_STAT_AREA	"Enable_TableStatArea" // Включить зону свойств «таблица статистики» . Значение: [0;1] Default: 1
#define ENABLE_AXIS_AREA		"Enable_AxisArea"	   // Включить зону свойств осей. Значение: [0;1] Default: 1
#define ENABLE_X_PARAMS_AREA	"Enable_X_ParamsArea"  // Включить зону свойств параметров для оси X. Значение: [0;1] Default: 1
#define ENABLE_Y_PARAMS_AREA	"Enable_Y_ParamsArea"  // Включить зону свойств параметров для оси Y. Значение: [0;1] Default: 1

#define ENABLE_Y_PARAMS_AREA	"Enable_Y_ParamsArea"  // Включить зону свойств параметров для оси Y. Значение: [0;1] Default: 1
#define ENABLE_Y_PARAMS_AREA	"Enable_Y_ParamsArea"  // Включить зону свойств параметров для оси Y. Значение: [0;1] Default: 1

#define DEPENDENCE_TYPE "DependenceType" // Вид зависимости. Значение: [0(точки);1(модель); 2(автоподбор)] Default: 0
#define MARKER_TYPE "MarkerType" // Вид маркера. Значение: [0(крестообразные);1(квадратаые); 2(круглые)] Default: 2
#define MARKER_SIZE_2 "MarkerSize"  // Размер маркера. Default: 2
#define MODEL_TYPE "ModelType"  // Зависимости. Def:0 Значения: 
												//	0:   y= a + b * x
												//	1:   y = 1 \ ( a + b * x )
												//	2:   y = a + b \ x
												//	3:   y = x \ ( a + b * x )
												//	4:   y = a * exp( b * x )
												//	5:   y = a * 10 ^ (b * x )
												//	6:   y = 1 \ ( a + b * e ^  (- x )  )
												//	7:   y = a * x  ^  b
												//	8:   y = a  +  b * lg( x )
												//	9:   y = a  +  b * ln( x )
												//	10: y = a * exp( b \ x )
												//	11: y = a + b * x ^ 2
												//	12: y = a + b * x ^ 3

#define MARKER_COLOR "MarkerColor"  // Цвет маркера. Default: 2
#define SHOW_MODEL "ShowModel"  // Показывать ли модель
#define ENABLE_CONNECTING_LINES "EnableConnectingLines"
#define CONN_LINES_CLR "ConnectingLineColor"

//------------------------------------------------------------------
#define SECT_STATUI_STAT_PARAM_ROOT sMainStatParamsSection	// Настройки параметра
#define NORMAL_AREA "NormalArea"
#define MX_PROPABILITY "Probability"


#define PARAM_NAME sUserName // Название
#define FORMAT_STRING sFormat // Определение формата отображения. Пример: %0.3f
#define DESIGNATION sGraphics // Символьное обозначение
#define MODULE_OF_DESIGN "Units" // Единицы измерений
#define PARAM_VISIBILITY "Visible"
//#define SHOW_ON_GRAPH "ShowOnGraph" // Возможность показывать на графике.
#define CALC_FROM "CalcFrom" //Способ расчета. Значение: [0(по объектам);1(по изображениям)].
#define EXPRESSION_STRING sExprString // Строка с расчетной формулой в спец формате
#define CALC_METHOD "CalcMethod" // По какому классу считать. Значение(номер класса): [-1(по каждому); 0..n].
#define PARAM_GREEK_SYMB sGreekSymbol
#define PARAM_CLASSIFIER sClassifier
#define PARAM_TABLE_ORDER "TableOrder"
#define PARAM_LEGEND_ORDER "LegendOrder"

//------------------------------------------------------------------
#define SECT_STATUI_STAT_LEGEND "\\StatUI\\Legend"			// Настройки легенды
#define SECT_STATCMP_STAT_LEGEND "\\StatCompare\\Legend"	// Настройки легенды для сравнения 

#define SECT_STATUI_LEGEND_TITLE_FONT_ROOT "\\StatUI\\Legend\\TitleFont" // Настройки шрифта заголовка

#define TEXT_COLOR "TextColor" // Цвет для отображения значений. Default: ( 0, 0, 0 )
#define SAMPLING_COLOR "SamplingColor" // Цвет выборки при сравнении. Default: ( 0, 0, 0 )
#define SAMPLING_NAME "SamplingName" // Название выборки при сравнении. 

#define FISHER_PROBABILITY "FisherProbability"
#define SECT_STATUI_STAT_LEGEND_USERDATA "\\StatUI\\Legend\\UserValues"	// Технологовские записи
#define ENTRY_NAME "EntryName" // Название записи
#define ENTRY_VALUE "EntryValue" // Значение записи
#define ENTRY_ORDER "EntryOrder" // Значение записи

#define SECT_STATUI_STAT_TABLE_USERDATA "\\StatUI\\StatTable\\UserValues"	// Технологовские записи

//---
#define SECT_STATUI_STAT_LEGEND_FONT "\\StatUI\\Legend\\Font\\"	// Настройки шрифта
//------------------------------------------------------------------
#define SECT_STATUI_STAT_TABLE "\\StatUI\\StatTable\\"	// Настройки таблицы статистики
#define SECT_STATUI_TABLE_TITLE_FONT_ROOT "\\StatUI\\StatTable\\TitleFont" // Настройки шрифта заголовка

#define INCORRECT_COLOR "IncorrectColor" // Цвет для отображения некорректных значений. Default: ( 255, 0, 0 )
#define HIGHLIGHT_COLOR "HighlightColor" // Цвет для подсветки столбика. Default:(127,127,127)
//--
#define SECT_STATUI_STAT_TABLE_COLOR "\\StatUI\\StatTable\\Colors\\"	// Настройки цветов таблицы статистики
//--
#define SECT_STATUI_STAT_TABLE_WIDTH "\\StatUI\\StatTable\\Width\\"	// Настройки ширины столбцов таблицы статистики

#define COLUMN_COLOR_ID "Column" // Цвет колонки номер [X]. Default: ( 0, 0, 0)
//--
#define SECT_STATUI_STAT_TABLE_FONT "\\StatUI\\StatTable\\Font\\"	// Настройки шрифта таблицы
#define SECT_STATUI_STAT_TABLE_HEADER_FONT "\\StatUI\\StatTable\\HeaderFont\\"	// Настройки шрифта таблицы
//--
#define FONT_NAME "Name" // Название шрифта. Default: Arial
#define FONT_SIZE "Size" // Размер в типографских пунктах: Default: 10
#define FONT_BOLD "Bold" // Жирность. Default: 0
#define FONT_ITALIC "Italic" // Наклонный. Default: 0
#define FONT_UNDERLINE "Underline" // Подчеркнутый. Default: 0
#define FONT_STRIKEOUT "StrikeOut" // Перечеркнутый. Default: 0
//------------------------------------------------------------------
#define CALC_FULL_STAT_CLASSES "CalcFullStatClasses" // По каким классам считать общюю статистику. Default: -1 ( по всем )
//------------------------------------------------------------------
#define SECT_STATUI_CHART_ROOT "\\StatUI\\ChartParams" // Настройки графика
#define SECT_DEPENDENCE_CHART_ROOT "\\StatDependence\\ChartParams" // Настройки для зависимости графика
#define SECT_STATCMP_CHART_ROOT "\\StatCompare\\ChartParams" // Настройки для сравнения графика

#define CHART_TYPE "ChartType" // Тип графика. Значение: [0(диаграмма);1(куммулята
#define CHART_VIEW_TYPE "ChartViewType" //Вид графика. Значение: [0(столбовой);1(линейный)] Default: 0
#define CHART_COLOR "ChartColor" // Цвет отображения линейного графика. Default: (255,0,0)
#define SHOW_GRAY_SCALE "ShowGrayScale" // Вид раскраски столбцов. Значение: [0(классы);1(серый)] Default: 1
#define GRAY_SCALE_LEVEL "GrayScaleLevel" // Уровень уровня серого. Значение: [0..255] Default: 128
#define CHART_TITLE "Title" // Заголовок
#define CLASSES_NAME "XParamName" // Название параметра по оси X d режиме от класса
#define SHOW_LEGEND "ShowLegend" // Показывать легенду. Значение: [0(легенда);1(подпись)] Default: 0
#define SHOW_CURVE "ShowCurve" // Отображение кривой модели распределения. Значение: [0;1] Default: 0
#define UNIFORM_DISTRIBUTION "UniformDistribution" // Закон распределения. Значение: [0(нормальный);1(логнормальный)] Default: 0
#define CURVE_COLOR "CurveColor" // Цвет закона распределения. Default: ( 0, 0, 0 )
#define COLOR_HATCH "ColorHatch"

#define SHOW_USERCURVE "ShowUserCurve" // Отображение кривой модели распределения пользователя. Default: 0
#define USERCURVE_COLOR "UserCurveColor" // Цвет закона распределения пользователя. Default: ( 0, 0, 255 )
#define USERCURVE_MX "UserCurveMX" // Мат. ожидание пользовательского закона распределения. Default: 0
#define USERCURVE_DX "UserCurveDX" // Дисперсия пользовательского закона распределения. Default: 1
#define VISIBLE_CLASSES "VisibleClass" // Дисперсия пользовательского закона распределения. Default: 1

#define DISTRIBUTION_SCRIPT "DistributionScript" // Название скрипта вызываемого при двойном клике по столбцу
#define HALF_HOT_ZONE "HalfHotZone" // Половина ширины активной зоны рядом с кривой модели при отбрасывании хинта. Default: 1
#define CHART_PARAM_HEIGHT "ChartParamHeight" // Высота прямоугольника параметров. Default: 10
#define MIN_CLASS_INDEX "MinClassIndex" // Индекс класса с которого начинать. Default: 0
#define CLASS_STEP "ClassStep" // Шаг вывода классов. Default: 1
#define CLASS_COL_TITLE "ClassColTitle" // Заголовок
#define ALL_WORD "AllWordName" // Заголовок
#define SHOW_SINGLE_COLORED_BAR "ShowSingleColoredBar" // Показывть все столбцы одного цвета. Значение: [0,1] Default: 0
#define BAR_SINGLE_COLOR "BarSingleColor" // Уровень уровня серого. Значение: [0..255] Default: 128

#define SHOW_BEGIN_END "ShowBeginEnd" // Качало, конец. Значение: [0;1] Default: 1
#define SHOW_STEP "ShowStep" // Шаг. Значение: [0;1] Default: 1
#define SHOW_CLASS_COUNT "ShowClassCount" // Кол-во классов. Значение: [0;1] Default: 1
#define SHOW_CHECK_MODEL "ShowCheckModel" // Показывать критерий проверки. Значение: [0;1] Default: 1
#define CHECK_MODEL_TYPE "CheckModelType" // Тип критерия проверки. Значение: [0(хи-квадрат);1(Фишер)] Default: 0
#define USER_SIGNATURE "Signature" // Подпись пользователя

#define BAR_WIDTH "BarWidth" // Ширина столбцов гистограммы. 
#define BAR_WIDTH_SPACE "BarWidthSpace" //Ширина между столбцами от ширины столбца

#define SECT_STATUI_CHART_TITLE_FONT_ROOT "\\StatUI\\ChartParams\\TitleFont" // Настройки шрифта заголовка графика
#define SECT_STATUI_CHART_LEGEND_FONT_ROOT "\\StatUI\\ChartParams\\LegendFont"  // Настройки шрифта легенды графика
#define SECT_STATUI_CHART_AXIS_FONT_ROOT "\\StatUI\\ChartParams\\AxisFont"  // Настройки шрифта разметки графика
#define SECT_STATUI_CHART_SIGNATURE_FONT_ROOT "\\StatUI\\ChartParams\\SignatureFont" // Настройки шрифта заголовка графика
#define SECT_STATUI_CHART_HINT_FONT_ROOT "\\StatUI\\ChartParams\\HintFont" // Настройки шрифта подписи (МОДЫ)  на графике
#define SECT_STATUI_CHART_PARAM_FONT_ROOT "\\StatUI\\ChartParams\\ParamNameFont" // Настройки шрифта параметров графика

#define SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT "\\StatDependence\\ChartParams\\TitleFont" // Настройки шрифта заголовка графика  для зависимости
#define SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT "\\StatDependence\\ChartParams\\LegendFont"  // Настройки шрифта легенды графика  для зависимости
#define SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT "\\StatDependence\\ChartParams\\AxisFont"  // Настройки шрифта разметки графика  для зависимости
#define SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT "\\StatDependence\\ChartParams\\ParamNameFont" // Настройки шрифта параметров графика  для зависимости

//--
#define SECT_STATUI_CHART_AXIS_ROOT "\\StatUI\\ChartParams\\Axis" // Настройки осей графика
#define SECT_DEPENDENCE_CHART_AXIS_ROOT "\\StatDependence\\ChartParams\\Axis" // Настройки осей для зависимости графика

#define X_USE_LOGA_SCALE "X_UseLogaScale" // Использовать логарифмическую шкалу для X. Значение: [0;1] Default: 1
#define Y_USE_LOGA_SCALE "Y_UseLogaScale" // Использовать логарифмическую шкалу для y. Значение: [0;1] Default: 1

#define X_AXIS_TYPE "X_AxisType" // Класс или параметр. Значение: [0(класс);1(параметр)]
#define X_PARAM_KEY "X_ParamKey" // Тип параметра.

#define X_USE_CUSTOM_FORMAT "X_UseCustomFormat" 
#define Y_USE_CUSTOM_FORMAT "Y_UseCustomFormat" 

#define X_CUSTOM_FORMAT "X_CustomFormat" 
#define Y_CUSTOM_FORMAT "Y_CustomFormat" 

#define X_BEGIN "X_Begin" // Начало значений по оси
#define X_END "X_End" // Конец значений по оси

#define Y_BEGIN "Y_Begin" // Начало значений по оси
#define Y_END "Y_End" // Конец значений по оси

#define X_USE_VALUE_RANGE "X_UseValueRange" // Использовать интервал значений. Значение: [0;1] Default: 0
#define Y_USE_VALUE_RANGE "Y_UseValueRange" // Использовать интервал значений. Значение: [0;1] Default: 0

#define X_SHOW_AXIS_VALUES "X_ShowAxisValues" // Отображать значения. Значение: [0;1]
#define Y_SHOW_AXIS_VALUES "Y_ShowAxisValues" // Отображать значения. Значение: [0;1]

#define X_SHOW_AXIS_LABELS "X_ShowAxisLabels" // Отображать штрихи. Значение: [0;1]
#define Y_SHOW_AXIS_LABELS "Y_ShowAxisLabels" // Отображать штрихи. Значение: [0;1]

#define X_TEXT_OFFSET "X_TextOffset"
#define Y_TEXT_OFFSET "Y_TextOffset"


#define X_SCALE_FACTOR "X_ScaleFactor" // Шаг штрихов по оси X.
#define X_SCALE_FACTOR_VALUES "X_ScaleFactorValues" // Шаг значений по оси X.
#define X_SCALE_FACTOR_GRID "X_ScaleFactorGrid" // Шаг сетки по оси X.

#define X_LOGARITHMIC_SCALE "X_LogarithmicScale" // Логарифмическая шкала. Значение: [0;1]
#define X_GRID "X_Grid" // Сетка по оси. Значение: [0(по цене деления);1(0,5 цены деления)]
#define Y_PARAM_KEY "Y_ParamKey" // Тип параметра.

#define Y_SCALE_FACTOR "Y_ScaleFactor" // Шаг штрихов по оси Y.
#define Y_SCALE_FACTOR_VALUES "Y_ScaleFactorValues" // Шаг значений по оси Y.
#define Y_SCALE_FACTOR_GRID "Y_ScaleFactorGrid" // Шаг сетки по оси Y.

#define Y_GRID "Y_Grid" // Сетка по оси. 
#define GRID_COLOR "GridColor" // Цвет сетки. Default: 0

#define CONVERT_TO_CALIBR "ConvertToCalibr"

#define DRAW_END_CHART_GRID_LINE_X "DrawEndChartGridLineX"
#define DRAW_END_CHART_GRID_LINE_Y "DrawEndChartGridLineY"
#define TABLE_ALIGN "TableAlign"

#define USE_CUSTOM_INTERVALS "UseCustomIntervals"
#define SECT_CUSTOM_INTERVALS "\\CustomIntervals"
#define ERROR_NUMBER "ErrorNumber"
#define ERROR_STRING "ErrorString"

#define SECT_DEPENDENCE_RESULT_ROOT "\\StatDependence\\Result"
#define RESUL_MODEL			"ModelIndex"
#define RESUL_CORREL		"Correlation"
#define RESUL_COEF_A		"Coef_A"
#define RESUL_COEF_B		"Coef_B"
#define RESULT_ALL_COUNT	"AllCount"		// количество объектов в таблице данных
#define RESULT_CURR_COUNT	"CurrCount"		// реальное количество объектов, отображаемых в зависимости

//------------------------------------------------------------------
#define SECT_STATUI_VIRTUAL_CLASSES_DIVISION_ROOT "\\StatUI\\VirtualClassesDivision" // Параметры разделения на виртуальные классы
#define DIVISION_CLASS_COUNT "ClassCount" //Кол-во классов на которое будет производится деление: Default: 0
#define DIVISION_TYPE "DivisionType:" // Тип деления. Значение: [0(норм.);1(логнорм)] Default: 0
#define DIVISION_VALUE "DivisionValue" // Параметр деления. 

//------------------------------------------------------------------
#define PROPBAG_PANES  "Views"
//------------------------------------------------------------------
#define VIRTUAL_STAT_CLASSIFIER  "VirtualClasses.ini"
#define VIRTUAL_MAIN_CLASS  "VirtualMainClass"
#define VIRTUAL_CLASS_COUNT  "VirtualClassCount"
//------------------------------------------------------------------
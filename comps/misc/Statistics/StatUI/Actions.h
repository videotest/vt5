#pragma once

//#include "action_main.h"
#include "action_misc.h"
#include "statistics.h"
#include "statui_consts.h"

#include "action_undo.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////
//
//	class CShowStatTable
//
//////////////////////////////////////////////////////////////////////
namespace ViewSpace
{
	namespace ViewActions
	{
		class CShowStatObjectView : public CActionShowView,
								public _dyncreate_t<CShowStatObjectView>
		{
		public:
			route_unknown();	
		public:
			virtual _bstr_t GetViewProgID() { return _bstr_t( szStatObjectViewProgID ); }
		protected:
			HRESULT __stdcall DoInvoke();
		};

		class CShowStatObjectViewInfo : public _ainfo_t<IDS_SHOW_STAT_OBJECT_VIEW, _dyncreate_t<CShowStatObjectView>::CreateObject, 0>,
									public _dyncreate_t<CShowStatObjectViewInfo>
		{
			route_unknown();
		public:
			CShowStatObjectViewInfo(){}
			arg	*args()	{return NULL;}
			virtual _bstr_t target()			{return szTargetFrame; }	
		};
		/////////////////////////////////////////////////////////////////////////////
/**********************************************************************************/
		class _SubViewAviableBase
		{
		public:
			virtual bool _is_avaible( IUnknownPtr sptrT );
			virtual bool _is_checked();
		};
/**********************************************************************************/
		class CActionChartViewOnOff : public CAction,
								public _dyncreate_t<CActionChartViewOnOff>, public _SubViewAviableBase
		{
		public:
			route_unknown();	
		public:
			CActionChartViewOnOff();
			virtual ~CActionChartViewOnOff();
			
			//invoke
			com_call	DoInvoke();
			com_call GetActionState( DWORD *pdwState );

		protected:
			bool _is_checked();
		};


		/////////////////////////////////////////////////////////////////////////////
		class CActionChartViewOnOffInfo : public _ainfo_t<IDS_CHART_VIEW_ONOFF, _dyncreate_t<CActionChartViewOnOff>::CreateObject, 0>,
									public _dyncreate_t<CActionChartViewOnOffInfo>, public _SubViewAviableBase
		{
			route_unknown();
		public:
			CActionChartViewOnOffInfo(){}
			arg	*args()	{return s_pargs;}
			virtual _bstr_t target()			{return szTargetViewSite; }	
			static arg s_pargs[];
		};
/**********************************************************************************/
		class CActionLegendViewOnOff : public CAction,
								public _dyncreate_t<CActionLegendViewOnOff>, public _SubViewAviableBase
		{
		public:
			route_unknown();	
		public:
			CActionLegendViewOnOff();
			virtual ~CActionLegendViewOnOff();
			
			//invoke
			com_call	DoInvoke();
			com_call GetActionState( DWORD *pdwState );
		protected:
			bool _is_checked();
		};


		/////////////////////////////////////////////////////////////////////////////
		class CActionLegendViewOnOffInfo : public _ainfo_t<IDS_LEGEND_VIEW_ONOFF, _dyncreate_t<CActionLegendViewOnOff>::CreateObject, 0>,
									public _dyncreate_t<CActionChartViewOnOffInfo>
		{
			route_unknown();
		public:
			CActionLegendViewOnOffInfo(){}
			arg	*args()	{return s_pargs;}
			virtual _bstr_t target()			{return szTargetViewSite; }	
			static arg s_pargs[];
		};
/**********************************************************************************/
		class CActionTableViewOnOff : public CAction,
								public _dyncreate_t<CActionTableViewOnOff>, public _SubViewAviableBase
		{
		public:
			route_unknown();	
		public:
			CActionTableViewOnOff();
			virtual ~CActionTableViewOnOff();
			
			//invoke
			com_call	DoInvoke();
			com_call GetActionState( DWORD *pdwState );
		protected:
			bool _is_checked();
		};


		/////////////////////////////////////////////////////////////////////////////
		class CActionTableViewOnOffInfo : public _ainfo_t<IDS_TABLE_VIEW_ONOFF, _dyncreate_t<CActionTableViewOnOff>::CreateObject, 0>,
									public _dyncreate_t<CActionTableViewOnOffInfo>
		{
			route_unknown();
		public:
			CActionTableViewOnOffInfo(){}
			arg	*args()	{return s_pargs;}
			virtual _bstr_t target()			{return szTargetViewSite; }	
			static arg s_pargs[];
		};
/**********************************************************************************/
	}
}
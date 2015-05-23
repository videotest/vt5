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
		
		class CShowStatCmpView : public CActionShowView,
								public _dyncreate_t<CShowStatCmpView>
		{
		public:
			route_unknown();	
		public:
			virtual _bstr_t GetViewProgID() { return _bstr_t(szCmpStatObjectViewProgID); }
		protected:
			HRESULT __stdcall DoInvoke();
		};

		class CShowStatCmpViewInfo : public _ainfo_t<IDS_SHOW_STAT_CMP_VIEW, _dyncreate_t<CShowStatCmpView>::CreateObject, 0>,
									public _dyncreate_t<CShowStatCmpViewInfo>
		{
			route_unknown();
		public:
			CShowStatCmpViewInfo(){}
			arg	*args()	{return NULL;}
			virtual _bstr_t target()			{return szTargetFrame; }	
		};
		/////////////////////////////////////////////////////////////////////////////
/**********************************************************************************/
		class SubViewAvaibleBase
		{
		public:
			virtual bool _is_avaible( IUnknownPtr sptrT );
			virtual bool _is_checked();
		};
/**********************************************************************************/
		class CActionChartCmpOnOff : public CAction,
								public _dyncreate_t<CActionChartCmpOnOff>, public SubViewAvaibleBase
		{
		public:
			route_unknown();	
		public:
			CActionChartCmpOnOff();
			virtual ~CActionChartCmpOnOff();
			
			//invoke
			com_call	DoInvoke();
			com_call GetActionState( DWORD *pdwState );

		protected:
			bool _is_checked();
		};


		/////////////////////////////////////////////////////////////////////////////
		class CActionChartCmpOnOffInfo : public _ainfo_t<IDS_CHART_CMP_ONOFF, _dyncreate_t<CActionChartCmpOnOff>::CreateObject, 0>,
									public _dyncreate_t<CActionChartCmpOnOffInfo>, public SubViewAvaibleBase
		{
			route_unknown();
		public:
			CActionChartCmpOnOffInfo(){}
			arg	*args()	{return s_pargs;}
			virtual _bstr_t target()			{return szTargetViewSite; }	
			static arg s_pargs[];
		};
/**********************************************************************************/
		class CActionLegendCmpOnOff : public CAction,
								public _dyncreate_t<CActionLegendCmpOnOff>, public SubViewAvaibleBase
		{
		public:
			route_unknown();	
		public:
			CActionLegendCmpOnOff();
			virtual ~CActionLegendCmpOnOff();
			
			//invoke
			com_call	DoInvoke();
			com_call GetActionState( DWORD *pdwState );
		protected:
			bool _is_checked();
		};


		/////////////////////////////////////////////////////////////////////////////
		class CActionLegendCmpOnOffInfo : public _ainfo_t<IDS_LEGEND_CMP_ONOFF, _dyncreate_t<CActionLegendCmpOnOff>::CreateObject, 0>,
									public _dyncreate_t<CActionChartCmpOnOffInfo>
		{
			route_unknown();
		public:
			CActionLegendCmpOnOffInfo(){}
			arg	*args()	{return s_pargs;}
			virtual _bstr_t target()			{return szTargetViewSite; }	
			static arg s_pargs[];
		};
/**********************************************************************************/
		class CActionTableCmpOnOff : public CAction,
								public _dyncreate_t<CActionTableCmpOnOff>, public SubViewAvaibleBase
		{
		public:
			route_unknown();	
		public:
			CActionTableCmpOnOff();
			virtual ~CActionTableCmpOnOff();
			
			//invoke
			com_call	DoInvoke();
			com_call GetActionState( DWORD *pdwState );
		protected:
			bool _is_checked();
		};


		/////////////////////////////////////////////////////////////////////////////
		class CActionTableCmpOnOffInfo : public _ainfo_t<IDS_TABLE_CMP_ONOFF, _dyncreate_t<CActionTableCmpOnOff>::CreateObject, 0>,
									public _dyncreate_t<CActionTableCmpOnOffInfo>
		{
			route_unknown();
		public:
			CActionTableCmpOnOffInfo(){}
			arg	*args()	{return s_pargs;}
			virtual _bstr_t target()			{return szTargetViewSite; }	
			static arg s_pargs[];
		};
	}
}
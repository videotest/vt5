#include "StdAfx.h"
#include "stat_depend_actions.h"
#include "stat_consts.h"

namespace STAT_DEPENDENCE
{
namespace STAT_DEPENDENCE_ACTIONS
{
/*************************************************************************/

//////////////////////////////////////////////////////////////////////
//
//	class show_stat_dependence
//
//////////////////////////////////////////////////////////////////////
_bstr_t show_stat_dependence::GetViewProgID()
{
	return _bstr_t( szStatTableDependenceViewProgID );
}

/*************************************************************************/
};
};
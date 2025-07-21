#include "pch.h"
#include "LuaAPIStage.h"

#include "ScriptDef.h"

namespace LuaAPIStage
{
	void Init(lua_State* L)
	{
		lua_tinker::def(L, "OnEndRound", OnEndRound);
		lua_tinker::def(L, "OnEndStage", OnEndStage);
		lua_tinker::def(L, "OnPreStartRound", OnPreStartRound);
		lua_tinker::def(L, "OnStartRound", OnStartRound);
	}

	// Return : ����
	// To do
	LVOID OnEndRound()
	{
		return LNULL;
	}

	// Return : ����
	// To do
	LVOID OnEndStage()
	{
		return LNULL;
	}

	// Return : ����
	// To do
	LVOID OnPreStartRound()
	{
		return LNULL;
	}

	// Return : ����
	// To do
	LVOID OnStartRound()
	{
		return LNULL;
	}

	// Return : ����
	// To do
	LVOID OnStartStage()
	{
		return LNULL;
	}
}
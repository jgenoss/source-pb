#include "pch.h"
#include "LuaAPIRoom.h"

#include "ScriptDef.h"

namespace LuaAPIRoom
{
	void Init(lua_State* L)
	{
		lua_tinker::def(L, "OnDetachPlayer", OnDetachPlayer);
		lua_tinker::def(L, "OnJoinPlayer", OnJoinPlayer);
		lua_tinker::def(L, "OnStageConfig", OnStageConfig);
	}

	// Return : ����
	// To do
	LVOID OnDetachPlayer( HPLAYER Player )
	{
		return LNULL;
	}

	// Return : ����
	// To do
	LVOID OnJoinPlayer( HPLAYER player )
	{
		return LNULL;
	}

	// Return : ����
	// To do
	LVOID OnStageConfig()
	{
		return LNULL;
	}
}


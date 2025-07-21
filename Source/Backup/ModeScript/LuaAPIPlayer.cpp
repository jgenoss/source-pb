#include "pch.h"
#include "LuaAPIPlayer.h"

#include "ScriptDef.h"

namespace LuaAPIPlayer
{
	void Init(lua_State* L)
	{
		lua_tinker::def(L, "OnDamagePlayer", OnDamagePlayer);
		lua_tinker::def(L, "OnEnterTrigger", OnEnterTrigger);
		lua_tinker::def(L, "OnInstall", OnInstall);
		lua_tinker::def(L, "OnIntrudePlayer", OnIntrudePlayer);
		lua_tinker::def(L, "OnKillPlayer", OnKillPlayer);
		lua_tinker::def(L, "OnLeaveTrigger", OnLeaveTrigger);
		lua_tinker::def(L, "OnObtainWeapon", OnObtainWeapon);
		lua_tinker::def(L, "OnStayTrigger", OnStayTrigger);
		lua_tinker::def(L, "OnThrowAwayWeapon", OnThrowAwayWeapon);
		lua_tinker::def(L, "OnUninstall", OnUninstall);
	}

	//
	// Called : Client, Server
	//

	// Return : ����
	// To do : getAttacker, getAttackerType, addDamage, killPlayer, addCount, addScore, addGauge
	LVOID OnDamagePlayer( HPLAYER player, REAL32 damage )
	{
		return LNULL;
	}

	// Return : ����
	// To do : getObjectName
	LVOID OnEnterTrigger( HPLAYER player, HOBJECT trigger )
	{
		return LNULL;
	}

	// Return : ��ġ�� ����Ϸ��� true, �׷��� ������ false�� ��ȯ�Ѵ�.
	// To do : installObject
	bool OnInstall( HPLAYER player, HWEAPON weapon )
	{
		return true;
	}

	// Return : ����
	// To do : setIntrudeEnable
	LVOID OnIntrudePlayer( HPLAYER player )
	{
		return LNULL;
	}

	// Return : ����
	// To do :
	LVOID OnKillPlayer( HPLAYER player )
	{
		return LNULL;
	}

	// Return : ����
	// To do : destroyObject, damageObject
	LVOID OnLeaveTrigger( HPLAYER player, HOBJECT trigger )
	{
		return LNULL;
	}

	// Return : ���� ������ ����Ϸ��� true, �׷��� ������ false�� ��ȯ�Ѵ�.
	// To do : getWeaponID, getWeaponClass
	bool OnObtainWeapon ( HPLAYER player, HWEAPON weapon )
	{
		return true;
	}

	// Return : ����
	// To do
	LVOID OnStayTrigger( HPLAYER player, HOBJECT trigger)
	{
		return LNULL;
	}

	// Return : ����
	// To do
	LVOID OnThrowAwayWeapon ( HPLAYER player, HWEAPON weapon )
	{
		return LNULL;
	}

	// Return : ��ü�� ����Ϸ��� true, �׷��� ������ false�� ��ȯ�Ѵ�.
	// To do
	bool OnUninstall( HPLAYER player, HWEAPON weapon )
	{
		return true;
	}

	LVOID	addEXP( HPLAYER player, INT32 exp )
	{
		return LNULL;
	}
	LVOID	addPoint( HPLAYER player, INT32 exp )
	{
		return LNULL;
	}
	LVOID	addTempWeapon( HPLAYER player, WEAPON_ID weapon )
	{
		return LNULL;
	}
	LVOID	disableRespawn( HPLAYER player )
	{
		return LNULL;
	}
	LVOID	disableWeapon( HPLAYER player, WEAPON_USAGE_TYPE slot )
	{
		return LNULL;
	}
	LVOID	enableRespawn( HPLAYER player )
	{
		return LNULL;
	}
	LVOID	enableWeapon( HPLAYER player, WEAPON_USAGE_TYPE slot )
	{
		return LNULL;
	}
	INT32	getAlivePlayerCount( TEAM team )
	{
		return 0;
	}
	HPLAYER getKiller( HPLAYER player )
	{
		return NULL;
	}
	CLAN_ID GetPlayerClan( HPLAYER player )
	{
		return 0;
	}
	INT32	getPlayerHP( HPLAYER player )
	{
		return 0;
	}
	const char* GetPlayerName( HPLAYER player )
	{
		return NULL;
	}
	INT32	GetPlayerPoint( HPLAYER player )
	{
		return 0;
	}
	PVEC3D	getPlayerPos( HPLAYER player )
	{
		return NULL;
	}
	RANK	GetPlayerRank( HPLAYER player )
	{
		return -1;
	}
	HWEAPON GetPlayerWeapon( HPLAYER player, WEAPON_USAGE_TYPE usage )
	{
		return NULL;
	}
	TEAM	getTeam( HPLAYER player )
	{
		return BOTH;
	}
	LVOID	setRole( HPLAYER player, ROLE role )
	{
		return LNULL;
	}

}
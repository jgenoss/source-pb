#pragma once

#include "LuaAPIDef.h"

extern CSampleClass g_kSample; 

namespace LuaAPIPlayer
{
	void Init(lua_State* L);

	//
	// Event Handler Functions
	// Called : Client, Server
	//
	LVOID	OnDamagePlayer( HPLAYER player, REAL32 damage );			// Player�� ������ ���� �������� ���� ��� ȣ��ȴ�. ������ �� Player�� ���ؼ��� ȣ����� �ʴ´�.
	LVOID	OnEnterTrigger( HPLAYER player, HOBJECT trigger );		// Player�� Trigger�� �������� ��, �ѹ� ȣ��ȴ�.
	bool	OnInstall( HPLAYER player, HWEAPON weapon );				// Player�� �̼� ������ �� ���� ���� ��ġ���� �� ȣ��ȴ�.
	LVOID	OnIntrudePlayer( HPLAYER player );						// ���ο� Player�� �ش� �濡 �������� �� ȣ��Ǵ� �Լ�. ������ ��쿡�� ȣ��Ǹ�, ó�� ���� �Բ� ������ ����� Player���� ���⼭ ȣ����� �ʴ´�. �� �Լ��� ���޵� Player�� �̹� ������ ���� ����̱� ������ �� �̻� ���� �Ұ� �� ������ �Ұ����ϴ�.
	LVOID	OnKillPlayer( HPLAYER player );							// Player�� ���� ��쿡 ȣ��ȴ�. ���� ��ü�� ������ ���̱� ������ �� �Լ��� ���� ������ �� ����.
	LVOID	OnLeaveTrigger( HPLAYER player, HOBJECT trigger );		// Player�� Trigger�� ��� ��, �ѹ� ȣ��ȴ�.
	bool	OnObtainWeapon ( HPLAYER player, HWEAPON weapon );			// Player�� ������ ���⸦ �������� �� ȣ��ȴ�. ��ġ ���⸦ �������� ������ ȣ��ȴ�.
	LVOID	OnStayTrigger( HPLAYER player, HOBJECT trigger );			// Player�� Trigger�� �ӹ��� �ִ� ���� �� Frame���� ȣ��ȴ�.
	LVOID	OnThrowAwayWeapon ( HPLAYER player, HWEAPON weapon );		// Player�� ���⸦ ������ �� ȣ��ȴ�. ��ġ ���⸦ ������ ��쿡�� ȣ��ȴ�.
	bool	OnUninstall( HPLAYER player, HWEAPON weapon );				// Player�� �̼� ������ �� ���� ���� ��ü���� �� ȣ��ȴ�.


	LVOID	addEXP( HPLAYER player, INT32 exp );				// ���� Player���� ����ġ�� �����ϴ� �Լ�.
	LVOID	addPoint( HPLAYER player, INT32 exp );				// ���� Player���� Point�� �����ϴ� �Լ�.
	LVOID	addTempWeapon( HPLAYER player, WEAPON_ID weapon );	// �ش� Round�� ���� �ӽ÷� Ư�� ���⸦ �������ִ� �Լ�. ���޵� ����� Round�� ����� �� �������.
	LVOID	disableRespawn( HPLAYER player );					// ������ Player�� Respawn�� �Ұ����ϵ��� �����Ѵ�. �Ұ����ϰ� ������ Player�� ���� ���·� ������, �̰��� enableRespawn �Լ��� ���� Ȱ��ȭ�� ������ ���ӵȴ�.
	LVOID	disableWeapon( HPLAYER player, WEAPON_USAGE_TYPE slot );	// �ش� Player�� Ư�� ������� ��� �Ұ����ϵ��� �����Ѵ�.
	LVOID	enableRespawn( HPLAYER player );					// ���� Player�� Respawn�� �����ϵ��� �����Ѵ�. Respawn�� �Ұ����ϵ��� ������ Player�� ������ �����ϸ� Respawn �ܰ�� �Ѿ�� �ʴ´�.
	LVOID	enableWeapon( HPLAYER player, WEAPON_USAGE_TYPE slot );	// �ش� Player�� Ư�� ������� ��� �����ϵ��� �����Ѵ�.
	INT32	getAlivePlayerCount( TEAM team );					// ���� ���� ���� Player ���� ���´�.
	HPLAYER getKiller( HPLAYER player );						// ������ player�� ���� ��� Player�� ��ȯ�Ѵ�.
	CLAN_ID GetPlayerClan( HPLAYER player );					// Player�� ���� Clan�� ID.
	INT32	getPlayerHP( HPLAYER player );						// Ư�� Player�� HP�� ���ؿ´�.
	const char* GetPlayerName( HPLAYER player );				// Ư�� Player�� �̸��� ��ȯ�Ѵ�.
	INT32	GetPlayerPoint( HPLAYER player );					// Player�� Point�� ��ȯ�Ѵ�.
	PVEC3D	getPlayerPos( HPLAYER player );						// ���� Player�� ���� ��ġ�� ��ȯ�մϴ�.
	RANK	GetPlayerRank( HPLAYER player );					// Player�� ���� ���
	HWEAPON GetPlayerWeapon( HPLAYER player, WEAPON_USAGE_TYPE usage );// Player�� ���� �ֹ���, �������� ���� ���� �� �ִ�.
	TEAM	getTeam( HPLAYER player );							// Player�� ��( RED �Ǵ� BLUE)�� ��ȯ�Ѵ�.
	LVOID	setRole( HPLAYER player, ROLE role );				// Ư���� ������ MOD�� ���� ���Ǵ� ������,�� Player���� Ư�� ������ �ο��ϴµ� ���Ǵ� �Լ�.													// �� Timer�� ����Ǿ��� ���, OnTimeOut1, �Ǵ� OnTimeOut2 Event Handler �Լ����� ���� ȣ��ȴ�.
};

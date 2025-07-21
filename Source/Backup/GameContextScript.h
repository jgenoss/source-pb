#if !defined (__GAME_CONTEXT_SCRIPT_H__)
#define __GAME_CONTEXT_SCRIPT_H__

#if defined( USE_LUA_UI)

#include "GameContext.h"

namespace GameContextScript
{
	extern "C"	int		Init(lua_State * L);
				bool	InitGameDefine(lua_State *L);
				void	OnGameEvent(i3UIScene * pUIScene, INT32 nEvent,INT32 arg);

	/////////////////////////////////////////////////////////////////////////////////////////
	//
	//extern "C"	int		OnGameEvent(

	extern "C"	int		setEventOnce(lua_State *L);
	/////////////////////////////////////////////////////////////////////////////////////////
	// Section Lobby Room
	/////////////////////////////////////////////////////////////////////////////////////////
	extern "C"	int		getLobbyRoomCount( lua_State * L);
	extern "C"	int		getLobbyRoomInfo( lua_State * L);

	// Lobby - set selected room
	extern "C"	int		setLobbyRoomSelect(lua_State *L);				// �ش� �ε����� ���� ���� ���·� ����

	// Lobby-common string
	extern "C"	int		getLobbyRoomNumber_s(lua_State *L);				// ���ȣ: todo: ��Ʈ���� ���� �����°� Ȯ���ؾ��ؼ�..�ϴ� �׳� ����
	extern "C"	int		getLobbyRoomMapName_s(lua_State *L);			// ���̸�: ��) "�ƿ�����Ʈ"
	extern "C"	int		getLobbyRoomMode_s( lua_State * L);				// ���Ӹ��: ��) "�ı����"
	extern "C"	int		getLobbyRoomName_s( lua_State * L);				// ������: ��) "�ϰ� ¯�Դϴٿ�"

	extern "C"	int		getLobbyRoomProgressStatus_s(lua_State *L);		// �����Ȳ: ��) "������/�����"
	extern "C"	int		getLobbyRoomPingStatus(lua_State *L);			// ping����: ��) 4
	extern "C"	int		getLobbyRoomPlayerStatus(lua_State *L);			// �ο���: ��) 02/16

	extern "C"	int		getLobbySelectedConfig_s( lua_State * L);		// ���Ӽ���: ��) "100kill/10��"
	extern "C"	int		getLobbySelectedRoomMaster_s(lua_State * L);	// �����̸�: ��) "���÷��̾�"
	extern "C"	int		getLobbySelectedRoomTimeStatus_s(lua_State * L);// �����Ȳ: ��) "2:37"
	
	extern "C"	int		getLobbySelectedRoomLimitInterEnter_s(lua_State * L);	// ��������: ��) "����"
	extern "C"	int		getLobbySelectedRoomLimitWeapon_s(lua_State * L);		// ��������: ��) "����"
	extern "C"	int		getLobbySelectedRoomObserver_s(lua_State * L);			// �������: ��) "�����"
	extern "C"	int		getLobbySelectedRoomTeamBalance_s(lua_State * L);		// ���뷱��: ��) "����"

	/////////////////////////////////////////////////////////////////////////////////////////
	// Section Lobby User
	/////////////////////////////////////////////////////////////////////////////////////////
	extern "C"	int		getLobbyUserCount( lua_State *L);
	extern "C"	int		getLobbyUserInfo( lua_State *L);

	extern "C"	int		getLobbyUserNick( lua_State *L);
	extern "C"	int		getLobbyUserClanMark( lua_State *L);
	extern "C"	int		getLobbyUserRank( lua_State *L);
	extern "C"	int		getLobbyUserEsportsID( lua_State *L);
};
#endif	//#if defined( USE_LUA_UI)
#endif
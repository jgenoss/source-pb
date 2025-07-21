#include "pch.h"
#include "GlobalVariables.h"
#include "GameContextScript.h"
#include "GameContext.h"

#if defined( USE_LUA_UI)

#include "lua/luabind/luabind.hpp"
#include "TextSet.h"

LuaFuncReg pbGameContextGlue[] = 
{
	{"setEvent",							GameContextScript::setEvent										},
	{"getLobbyRoomCount",					GameContextScript::getLobbyRoomCount							},
	{"getLobbyRoomInfo",					GameContextScript::getLobbyRoomInfo								},
	
	{"setLobbyRoomSelect",					GameContextScript::setLobbyRoomSelect							},

	{"getLobbyRoomNumber_s",				GameContextScript::getLobbyRoomNumber_s							},
	{"getLobbyRoomMapName_s",				GameContextScript::getLobbyRoomMapName_s						},
	{"getLobbyRoomMode_s",					GameContextScript::getLobbyRoomMode_s							},
	{"getLobbyRoomName_s",					GameContextScript::getLobbyRoomName_s							},
	{"getLobbyRoomProgressStatus_s",		GameContextScript::getLobbyRoomProgressStatus_s					},
	{"getLobbyRoomPingStatus",				GameContextScript::getLobbyRoomPingStatus						},
	{"getLobbyRoomPlayerStatus",			GameContextScript::getLobbyRoomPlayerStatus						},
	{"getLobbySelectedConfig_s",			GameContextScript::getLobbySelectedConfig_s						},
	{"getLobbySelectedRoomMaster_s",		GameContextScript::getLobbySelectedRoomMaster_s					},
	{"getLobbySelectedRoomTimeStatus_s",	GameContextScript::getLobbySelectedRoomTimeStatus_s				},
	{"getLobbySelectedRoomLimitInterEnter_s", 	GameContextScript::getLobbySelectedRoomLimitInterEnter_s	},
	{"getLobbySelectedRoomLimitWeapon_s",	GameContextScript::getLobbySelectedRoomLimitWeapon_s			},
	{"getLobbySelectedRoomObserver_s",		GameContextScript::getLobbySelectedRoomObserver_s				},
	{"getLobbySelectedRoomTeamBalance_s", 	GameContextScript::getLobbySelectedRoomTeamBalance_s			},

	//
	{"getLobbyUserCount", 		GameContextScript::getLobbyUserCount		},
	{"getLobbyUserInfo", 		GameContextScript::getLobbyUserInfo			},
	{"getLobbyUserNick", 		GameContextScript::getLobbyUserNick			},
	{"getLobbyUserClanMark", 	GameContextScript::getLobbyUserClanMark		},
	{"getLobbyUserRank", 		GameContextScript::getLobbyUserRank			},
	{"getLobbyUserEsportsID", 	GameContextScript::getLobbyUserEsportsID	},

	{NULL, NULL},
};

int GameContextScript::Init(lua_State * L)
{
	InitGameDefine( L);

	for(int i=0; pbGameContextGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( L, pbGameContextGlue[i].name, pbGameContextGlue[i].func);
	}
 
	return 0;
}

int GameContextScript::SetEventOnce(lua_State *L)
{
	if( i3Lua::CheckEnumType(L, "pbGameEvent", -1))
	{
		INT32 nEvent = i3Lua::GetEnumValue(L, -1);

		I3TRACE("setEvent: %d\n", nEvent);
	}

	return 0;
}

void GameContextScript::OnGameEvent(i3UIScene * pUIScene, INT32 nEvent,INT32 arg)
{
	if( pUIScene == NULL)
		return;

	return pUIScene->OnEvent( nEvent, arg);
}



int GameContextScript::getLobbyRoomCount( lua_State * L)
{
	INT32 count = 8;
	if( g_pGameContext != NULL)
	{
		count = g_pGameContext->getLobbyRoomCount();
	}

	i3Lua::PushInteger( L, count);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////
#define LOBBYROOM()\
	INT32 index = i3Lua::GetIntegerArg(L, 1, 0);\
	\
	ROOM_INFO_BASIC Info;\
	Info.Reset();\
	const ROOM_INFO_BASIC * pInfo = NULL;\
	if( g_pGameContext != NULL && (g_pGameContext->getLobbyRoomCount() >= index))\
	{\
		pInfo = g_pGameContext->getRoomInfo(index);\
		if( pInfo == NULL)\
		{\
			pInfo = &Info;\
		}\
	}\
	\
	if( pInfo == NULL) pInfo = &Info;

/////////////////////////////////////////////////
#define LOBBYUSER()\
	INT32 index = i3Lua::GetIntegerArg(L, 1, 0);\
	\
	LOBBY_USER_INFO_LIST Info;\
	if( g_pGameContext != NULL && (g_pGameContext->GetLobbyUserCount() >= index))\
	{\
		Info = *g_pGameContext->getLobbyUserInfo( index);\
	}\
	else\
	{\
		Info.Reset();\
	}\
	\
	LOBBY_USER_INFO_LIST * pInfo = &Info

#define PUSH_ASCIITOUTF8_STRING(L, s)\
	char conv[MAX_PATH];\
	INT32 len = i3::generic_string_size(s);\
	INT32 length = MultiByteToWideChar( CP_UTF8, 0, s, len, NULL, 0);\
	\
	wchar_t * wTemp = (wchar_t*) i3MemAlloc( sizeof( wchar_t) * (length+1));\
	I3ASSERT( wTemp != NULL);\
	wTemp[length] = 0;\
	\
	MultiByteToWideChar( CP_UTF8, 0, s, len, wTemp, length);\
	\
	i3Lua::PushString(L, (const char*)wTemp);\
	\
	I3MEM_SAFE_FREE( wTemp)

////////////////////////////////////////////////////////////////////////////////////////
// Lobby���� ������ ���� ������ ���� �ɴϴ�.( editor������ Ȯ�� �����ϵ��� �ϱ����� 
// gamecontext�� �������� �ʾ������ �⺻ �����͸� �̿��մϴ�.
////////////////////////////////////////////////////////////////////////////////////////
int GameContextScript::getLobbyRoomInfo( lua_State * L)
{
	LOBBYROOM();

	i3Lua::NewTable(L);

	INT32 top = i3Lua::StackGetTop(L);

	i3Lua::PushString(L, "_Idx");
	i3Lua::PushInteger(L, pInfo->_Idx);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_InfoFlag");
	i3Lua::PushInteger(L, (INT32)pInfo->_InfoFlag);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_PersonPlayerMax");
	i3Lua::PushInteger(L, (INT32)pInfo->_PersonPlayerMax);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_PersonNow");
	i3Lua::PushInteger(L, (INT32)pInfo->_PersonNow);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_Ping");
	i3Lua::PushInteger(L, (INT32)pInfo->_Ping);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_StageID");
	i3Lua::PushInteger(L, (INT32)pInfo->_StageID);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_StageMask");
	i3Lua::PushInteger(L, (INT32)pInfo->_StageMask);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_State");
	i3Lua::PushInteger(L, (INT32)pInfo->_State);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_Title");
	i3Lua::PushA2UTF8String(L, pInfo->_Title);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_WeaponFlag");
	i3Lua::PushInteger(L, (INT32)pInfo->_WeaponFlag);
	i3Lua::SetTable(L, top);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////
// Lobby functions
///////////////////////////////////////////////////////////////////////////////////////

// �ش� �ε����� ���� ���õ� ���·� ����� ���ؽ�Ʈ�� �߰� ������ ��û �մϴ�.
int GameContextScript::setLobbyRoomSelect(lua_State *L)
{
	LOBBYROOM();

	g_pGameContext->setSelectedRoomIdx( pInfo->_Idx);

	g_pGameContext->SetEvent( EVENT_GET_ROOM_ADDINFO_N );

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
// Lobby String
// gamecontext�� �������� �ʾ������ �⺻ �����͸� �̿��մϴ�.
////////////////////////////////////////////////////////////////////////////////////////

int GameContextScript::getLobbyRoomNumber_s(lua_State *L)
{
	LOBBYROOM();

	char conv[MAX_PATH];
	i3::snprintf( conv, MAX_PATH -1, "%03d", pInfo->_Idx);

	i3Lua::PushString( L, conv);

	return 1;
}

// Lobby���� ������ ���� �̸��� ���� �ɴϴ�.
int	GameContextScript::getLobbyRoomName_s( lua_State * L)				// ������: ��) "�ϰ� ¯�Դϴٿ�"
{
	LOBBYROOM();

	i3Lua::PushA2UTF8String( L, pInfo->_Title);

	return 1;
}

int	GameContextScript::getLobbyRoomMode_s( lua_State * L)				// ���Ӹ��: ��) "�ı����"
{
	LOBBYROOM();

	if( pInfo->_PersonNow <= 0)
	{
		i3Lua::PushString(L, "NotInUse");
	}
	else
	{
		// ���Ӹ��
		if( GET_STAGEMASK_CLANMATCH( pInfo->_StageMask ) )		i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_CLAN_MATCH_MAKING_TEXT"));	/*Ŭ�� ��ġ����ŷ*/	// Ŭ�� ��ġ ����ŷ���� ��쿡�� �ʿ� ������� ��ġ����ŷ ���ڸ� ����ش�.
		else if (GET_STAGEMASK_TRAINING( pInfo->_StageMask ))	i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_TRANING_TEXT") );			/*�Ʒ���*/
		else if (GET_STAGEMASK_SNIPER( pInfo->_StageMask ))		i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_SNIPERMODE_TITLE") );		/*������*/
		else if (GET_STAGEMASK_SHOTGUN( pInfo->_StageMask ))	i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_SHOTGUNMODE_TITLE") );		/*������*/
		else
		{
			i3Lua::PushA2UTF8String( L, getStageTypeName( (STAGE_ID) pInfo->_StageID).c_str());
		}
	}

	return 1;
}

int	GameContextScript::getLobbyRoomMapName_s(lua_State *L)			// ���̸�: ��) "�ƿ�����Ʈ"
{
	LOBBYROOM();

	if( pInfo->_PersonNow <= 0)
	{
		i3Lua::PushString(L, "NotInUse");
	}
	else
	{
		char szStageName[ MAX_STRING_COUNT];
		getStageDisplayName( szStageName, (STAGE_ID) pInfo->_StageID);
		i3Lua::PushA2UTF8String(L, szStageName);
	}

	return 1;
}


int	GameContextScript::getLobbyRoomProgressStatus_s(lua_State *L)		// �����Ȳ: ��) "������/�����"
{
	LOBBYROOM();

	if (ROOM_STATE_READY == pInfo->_State)
	{
		i3Lua::PushA2UTF8String(L, GAME_STRING( "STBL_IDX_ROOM_STATE_WAIT"));
	}
	else
	{
		i3Lua::PushA2UTF8String(L, GAME_STRING( "STBL_IDX_ROOM_STATE_PLAY"));
	}

	return 1;
}

int	GameContextScript::getLobbyRoomPlayerStatus(lua_State *L)			// �ο���: ��) 02/16
{
	LOBBYROOM();

	char strPerson[MAX_STRING_COUNT];

	i3::snprintf( strPerson, MAX_STRING_COUNT, "%02d/%02d", pInfo->_PersonNow, pInfo->_PersonPlayerMax);

	i3Lua::PushString(L, strPerson);

	return 1;
}

int	GameContextScript::getLobbyRoomPingStatus(lua_State *L)			// ping����: ��) 4
{
	// nLevel = MINMAX(1, nLevel, 5);
	LOBBYROOM();

	i3Lua::PushInteger(L, pInfo->_Ping);	// ping level
	i3Lua::PushInteger(L, GET_ROOM_INFO_REAL_IP(pInfo->_InfoFlag)); // if > 0 then real ip

	return 2;
}

int	GameContextScript::getLobbySelectedConfig_s( lua_State * L)			// ���Ӽ���: ��) "100kill/10��"
{
	LOBBYROOM();

	STAGE_TYPE			stageType		= getStageType( pInfo->_StageID);

	UINT8 GameSetPrim	= g_pGameContext->getSelectedRoomInfoAdd()->_SubType & 0x0F;
	UINT8 GameSetSec	= g_pGameContext->getSelectedRoomInfoAdd()->_SubType & 0xF0;

	char szTemp[MAX_PATH];
	switch( stageType)
	{
	case STAGE_TYPE_ESCAPE			: 
	case STAGE_TYPE_ANNIHILATION	: 
	case STAGE_TYPE_BOMB			: 
	case STAGE_TYPE_DESTROY			: 
	case STAGE_TYPE_DEFENCE			:
	case STAGE_TYPE_SUDDEN_DEATH	: 
	case STAGE_TYPE_HEAD_KILLER	:
		{
			i3::safe_string_copy( szTemp, TextSet::Round(GameSetPrim), MAX_PATH);
		}
		break; 		
	case STAGE_TYPE_DEATHMATCH		: 
	case STAGE_TYPE_HEAD_HUNTER	:
	default							: 
		{
			i3::safe_string_copy(szTemp, TextSet::KillCount(GameSetPrim), MAX_PATH);
		}
		break;
	}
	i3::generic_string_cat( szTemp, " /");
	// Time Count
	i3::generic_string_cat( szTemp, TextSet::GameTime( GameSetSec) );

	i3Lua::PushA2UTF8String(L, szTemp);

	return 1;
}

int	GameContextScript::getLobbySelectedRoomMaster_s(lua_State * L)			// �����̸�: ��) "���÷��̾�"
{
	i3Lua::PushA2UTF8String(L, g_pGameContext->getSelectedRoomInfoAdd()->getNick());

	return 1;
}

int	GameContextScript::getLobbySelectedRoomTimeStatus_s(lua_State * L)		// �����Ȳ: ��) "2:37"
{
	LOBBYROOM();

	STAGE_TYPE			stageType		= getStageType( pInfo->_StageID);

	// ����
	if (ROOM_STATE_READY == pInfo->_State)
	{
		i3Lua::PushA2UTF8String(L, GAME_STRING("STR_TBL_GUI_LOBBY_WAIT_STATE_TEXT"));/*�����*/
	}	
	else
	{
		// ���Ӹ��
		char szTemp[MAX_PATH];
		switch( stageType)
		{
		case STAGE_TYPE_DEATHMATCH:// ��ü��		
		case STAGE_TYPE_HEAD_HUNTER:
			sprintf_s(szTemp, "%02d:%02d", (INT32)(g_pGameContext->getSelectedRoomInfoAdd()->_NowIngTime / 60), (INT32)( g_pGameContext->getSelectedRoomInfoAdd()->_NowIngTime % 60)); 
			break;
		case STAGE_TYPE_HEAD_KILLER		:
		case STAGE_TYPE_SUDDEN_DEATH	:
		case STAGE_TYPE_ESCAPE			:// �����̼���
		case STAGE_TYPE_ANNIHILATION	:// ������
		case STAGE_TYPE_DESTROY	: 
		case STAGE_TYPE_DEFENCE	: 
		case STAGE_TYPE_BOMB	: 
		default:			//�̼� 
			sprintf_s(szTemp,"%d Round", g_pGameContext->getSelectedRoomInfoAdd()->_NowRoundCount);	
			break;
		}

		i3Lua::PushA2UTF8String( L, szTemp);
	}

	return 1;
}

int	GameContextScript::getLobbySelectedRoomLimitInterEnter_s(lua_State * L)	// ��������: ��) "����"
{
	if (0 == g_pGameContext->getSelectedRoomInfoAdd()->_LockInterEnter)
	{
		i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_NONE_TEXT"));/*����*/
	}
	else
	{
		i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_TEXT"));/*����*/
	}

	return 1;
}

int	GameContextScript::getLobbySelectedRoomLimitWeapon_s(lua_State * L)		// ��������: ��) "����"
{
	// ���� ����
	UINT8 WeaponFlag = g_pGameContext->GetMyRoomWeaponFlag();
	if (WeaponFlag == LOCK_BASIC)
	{
		i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_NONE_TEXT"));/*����*/
	}
	else
	{
		i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_TEXT"));/*����*/
	}

	return 1;
}

int	GameContextScript::getLobbySelectedRoomObserver_s(lua_State * L)			// �������: ��) "�����"
{
	if( TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_CHASE) &&
		TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_FREE) &&
		TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_ENEMY) &&
		TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_ENEMY_HP) )
	{
		i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER1"));// ��λ��
	}
	else if(TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_CHASE) ||
			TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_FREE) ||
			TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_ENEMY) ||
			TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_ENEMY_HP) )
	{
		i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER2"));//���
	}
	else
	{
		i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER3"));//������� ����
	}

	return 1;
}

int	GameContextScript::getLobbySelectedRoomTeamBalance_s(lua_State * L)		// ���뷱��: ��) "����"
{
	if (g_pGameContext->getSelectedRoomInfoAdd()->_LockTeamBalance == 0)
	{
		i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_NONE_TEXT"));/*����*/
	}
	else
	{
		i3Lua::PushA2UTF8String( L, GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_TEXT"));/*����*/
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Lobby User Section
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GameContextScript::getLobbyUserCount( lua_State *L)
{
	INT32 cnt = 0;
	if( g_pGameContext != NULL)		
	{
		cnt = g_pGameContext->GetLobbyUserCount();
	}

	i3Lua::PushInteger( L, cnt);

	return 1;
}

int GameContextScript::getLobbyUserInfo( lua_State *L)
{
	LOBBYUSER();

	i3Lua::NewTable(L);

	INT32 top = i3Lua::StackGetTop(L);

	//i3Lua::PushString(L, "_Session");
	//i3Lua::PushInteger(L, pInfo->_Session);
	//i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_ClanMark");
	i3Lua::PushInteger(L, pInfo->_ClanMark);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_Rank");
	i3Lua::PushInteger(L, (INT32)pInfo->_Rank);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_eSportID");
	i3Lua::PushInteger(L, (INT32)pInfo->_eSportID);
	i3Lua::SetTable(L, top);

	i3Lua::PushString(L, "_Nick");
	i3Lua::PushA2UTF8String(L, pInfo->_Nick);
	i3Lua::SetTable(L, top);

	return 1;
}


int GameContextScript::getLobbyUserNick( lua_State *L)
{
	LOBBYUSER();

	//USER_INFO_BASIC * pInfo = UserInfoContext::i()->GetMyUserInfoBasic();

	i3Lua::PushA2UTF8String(L, pInfo->_Nick);

	return 1;
}

int GameContextScript::getLobbyUserClanMark( lua_State *L)
{
	LOBBYUSER();

	i3Lua::PushInteger(L, pInfo->_ClanMark);

	return 1;
}

int GameContextScript::getLobbyUserRank( lua_State *L)
{
	LOBBYUSER();

	i3Lua::PushInteger(L, pInfo->_Rank);

	return 1;
}

int GameContextScript::getLobbyUserEsportsID( lua_State *L)
{
	LOBBYUSER();

	i3Lua::PushInteger(L, pInfo->_eSportID);

	return 1;
}

#endif //#if defined( USE_LUA_UI)
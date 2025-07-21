#include "pch.h"
#include "GameContextScript.h"

#if defined( USE_LUA_UI)

bool	GameContextScript::InitGameDefine(lua_State *L)
{
	bool bReturn = false;
	// Stage Type
	bReturn = i3Lua::AddEnumToLua(L, "STAGE_TYPE",
		"NA", STAGE_TYPE_NA,
		"DEATHMATCH", STAGE_TYPE_DEATHMATCH,
		"BOMB", STAGE_TYPE_BOMB,
		"DESTROY", STAGE_TYPE_DESTROY,
		"ANNIHILATION", STAGE_TYPE_ANNIHILATION,
		"DEFENCE", STAGE_TYPE_DEFENCE,
		"SUDDENDEATH", STAGE_TYPE_SUDDEN_DEATH,
		"BOSS", STAGE_TYPE_ESCAPE,
		"HEADHUNTER", STAGE_TYPE_HEAD_HUNTER,
		"HEADKILLER", STAGE_TYPE_HEAD_KILLER,
		"MAX", STAGE_TYPE_MAX,
		0);

	if( bReturn == false)
	{
		I3FATAL("Add Enum to Lua failed");
		return false;
	}

	// Game Event
	// Global event
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bReturn = i3Lua::AddEnumToLua(L, "pbGameEvent",
	"NOTHING",					EVENT_NOTHING,		// �ƹ��� �̺�Ʈ�� �����ϴ�.
	"ENTER_LOBBY",				EVENT_ENTER_LOBBY,					// �κ� ����
	"LEAVE_LOBBY",				EVENT_LEAVE_LOBBY,					// �κ񿡼� ������(������������)
	"ENTER_INVENTORY",			EVENT_ENTER_INVENTORY,				// ��� ���� ȭ������ ����
	"LEAVE_INVENTORY",			EVENT_LEAVE_INVENTORY,				// ��� ���� ����
	"ENTER_INFO",				EVENT_ENTER_INFO,					// ���������� ����
	"LEAVE_INFO",				EVENT_LEAVE_INFO,					// ���������� ����
	"ENTER_SHOP",				EVENT_ENTER_SHOP,					// Shop���� ����
	"LEAVE_SHOP",				EVENT_LEAVE_SHOP,					// Shop���� ������ 
	"GET_NEW_CHALLENGE_CARD",	EVENT_GET_NEW_CHALLENGE_CARD,		//
	"REFRESH_MEDAL",			EVENT_REFRESH_MEDAL,				// �� �޴����� Ȯ�� - NEW ǥ�� ������
	"CONNECT_SERVER_GATEWAY",	EVENT_CONNECT_SERVER_GATEWAY,		// ����Ʈ���� ������ ����
	"CONNECT_SERVER_GAME",		EVENT_CONNECT_SERVER_GAME,				// ���ؼ� �Ϸ� üũ
	"GET_USERINFO",				EVENT_GET_USERINFO,					// ���� ���� �б� (arg : UserDB index)
	"GET_SERVER_LIST",			EVENT_GET_SERVER_LIST,
	"GET_CHANNEL_LIST",			EVENT_GET_CHANNEL_LIST,				// ä�� ����Ʈ �о����
	"SELECT_CHANNEL",			EVENT_SELECT_CHANNEL,				// ä�� ����
	"LOG_IN",					EVENT_LOG_IN,						// �α���
	"LOG_IN_WEBKEY",			EVENT_LOG_IN_WEBKEY,				// ������ �α���
	"LOG_OUT",					EVENT_LOG_OUT,						// ���ϰ��� ���� ���� ��û
	"USER_LEAVE",				EVENT_USER_LEAVE,					// ���� �̵� ������
	"USER_ENTER",				EVENT_USER_ENTER,					// ���� �̵� ������
	"ENTER_PW",					EVENT_ENTER_PW,						// ���� �̵� �н�����
	"ITEM_AUTH",				EVENT_ITEM_AUTH,					// ������ ������ �����ϱ� - ����ϱ�
	"ITEM_AUTH_DATA",			EVENT_ITEM_AUTH_DATA,				// ������ ������ �����ϱ� ����Ÿ���� - ����ϱ�
	"AUTH_DELETE",				EVENT_AUTH_DELETE,					// �Ⱓ�� ������ �����ϱ� - ����ϱ�
	"USE_ITEM_CHECK_NICK",		EVENT_USE_ITEM_CHECK_NICK,			// �г��� �ߺ��˻�
	"VIEW_OTHER_USER_ITEM_INFO",EVENT_VIEW_OTHER_USER_ITEM_INFO,	// ���� ���� ����(������)

	"REQUEST_GOODS_LIST",		EVENT_REQUEST_GOODS_LIST,			// ��ǰ ����Ʈ ��û
	"BUY_GOODS",				EVENT_BUY_GOODS,					// ��ǰ ����
	"BUY_AUTH",					EVENT_BUY_AUTH,						// ! ������.
	"AUTH_GIFT_GOODS",			EVENT_AUTH_GIFT_GOODS,				// ��ǰ �����ϱ�
	"ITEM_REPAIR",				EVENT_ITEM_REPAIR,					// ������ ����
	"REQ_GIFT_LIST",			EVENT_REQ_GIFT_LIST,				// �������� ����Ʈ�� ������ ��û.
	"GET_GIFT",					EVENT_GET_GIFT,						// ������ �޾Ҵٴ� �̺�Ʈ
	"CHECK_GIFT_GOODS",			EVENT_CHECK_GIFT_GOODS,				// ��ǰ ���� �ޱ� ���� Ȯ��

	"GET_REMAIN_MONEY",			EVENT_GET_REMAIN_MONEY,				// ���� ����Ʈ, �� �ޱ�

//	"RENDEZVOUS_CONNECT",		EVENT_RENDEZVOUS_CONNECT,			// ������ ������ �����Ѵ�.
//	"RENDEZVOUS_NAT_STATE",		EVENT_RENDEZVOUS_NAT_STATE,			// ���� ������ NAT ���¸� �˷��ش�.
//	"RENDEZVOUS_HOLEPUNCH",		EVENT_RENDEZVOUS_HOLEPUNCH,			// ���� ������ Ȧ��Ī �Ϸ� ���¸� �˷��ش�.

	//	Clan
	"CLAN_ENTER",				EVENT_CLAN_ENTER,					// ����
	"CLAN_LEAVE",				EVENT_CLAN_LEAVE,					// ������
	"CLAN_LIST_CONTEXT",		EVENT_CLAN_LIST_CONTEXT,			// Ŭ�� ����� ���� Context ��û
	"CLAN_CREATE",				EVENT_CLAN_CREATE,					// �����ϱ�
	"CLAN_DUPLICATE_NAME",		EVENT_CLAN_DUPLICATE_NAME,			// �̸� �ߺ��˻� 0 - ����ص� ��,  >0 ������ , <0 ����
	"CLAN_DUPLICATE_ADDRESS",	EVENT_CLAN_DUPLICATE_ADDRESS,		// ����Ʈ �ּ� �ߺ��˻� 0 - ����ص� ��,  >0 ������ , <0 ����
	"CLAN_DUPLICATE_MARK",		EVENT_CLAN_DUPLICATE_MARK,			// Ŭ����ũ �ߺ��˻� 0 - ����ص� ��,  >0 ������ , <0 ����
	"CLAN_JOIN_REQUEST",		EVENT_CLAN_JOIN_REQUEST,			// ���� ��û�ϱ�
	"CLAN_JOINCHECK_AUTHORITY", EVENT_CLAN_JOINCHECK_AUTHORITY,		// ���Ա��� üũ
	"CLAN_CANCEL_REQUEST",		EVENT_CLAN_CANCEL_REQUEST,			// ���Խ�û ���.
	"CLAN_MEMBER_CONTEXT",		EVENT_CLAN_MEMBER_CONTEXT,			// ��� ��� ����
	"CLAN_MEMBER_LIST",			EVENT_CLAN_MEMBER_LIST,				// ��� ��� �ޱ�
	"CLAN_REQUEST_CONTEXT",		EVENT_CLAN_REQUEST_CONTEXT,			// ���Խ�û�� �������
	"CLAN_ACCEPT_REQUEST",		EVENT_CLAN_ACCEPT_REQUEST,			// ���Խ�û�� ����
	"CLAN_DENIAL_REQUEST",		EVENT_CLAN_DENIAL_REQUEST,			// ���Խ�û�� ����
	"CLAN_SECESSION",			EVENT_CLAN_SECESSION,				// Ŭ�� Ż��
	"CLAN_COMMISSIONMASTER",	EVENT_CLAN_COMMISSIONMASTER,		// ������ ����
	"CLAN_COMMISSIONSTAFF",		EVENT_CLAN_COMMISSIONSTAFF,			// Ŭ�� ���� �Ӹ�
	"CLAN_COMMISSION_REGULAR",	EVENT_CLAN_COMMISSION_REGULAR,		// Ŭ�� �Ϲ����� ����
	"CLAN_DISMISSALMEMBER",		EVENT_CLAN_DISMISSALMEMBER,			// Ŭ���� ����
	"CLAN_DISSOLUTION",			EVENT_CLAN_DISSOLUTION,				// ���� ��ü
	"CLAN_NOTICE_MODIFY",		EVENT_CLAN_NOTICE_MODIFY,			// �������� ����
	"CLAN_GUIDE_MODIFY",		EVENT_CLAN_GUIDE_MODIFY,			// Ŭ���Ұ� ����
	"CLAN_MENAGEMENT",			EVENT_CLAN_MENAGEMENT,				// Ŭ�� ��Ÿ ���� ����
	"CLAN_CHANGE_MARK",			EVENT_CLAN_CHANGE_MARK,				// Ŭ����ũ ����
	"CLAN_MEMBER_INSERT",		EVENT_CLAN_MEMBER_INSERT,			// Ŭ���ɹ� �߰�
	"CLAN_INVITE",				EVENT_CLAN_INVITE,					// Ŭ�� �ʴ�
	"CLAN_NOTE",				EVENT_CLAN_NOTE,					// Ŭ�� ��ü����
	"MATCH_TEAM_CREATE",		EVENT_MATCH_TEAM_CREATE,			// Ŭ���� �� �����
	"MATCH_TEAM_JOIN",			EVENT_MATCH_TEAM_JOIN,				// Ŭ���� �� ����
	"MATCH_TEAM_LEAVE",			EVENT_MATCH_TEAM_LEAVE,				// Ŭ���� �� ������
	"MATCH_TEAM_CHANGE_PER",	EVENT_MATCH_TEAM_CHANGE_PER,		// Ŭ���� �ο��� ����
	"MATCH_TEAM_CONTEXT",		EVENT_MATCH_TEAM_CONTEXT,			// Ŭ���� ����� Context
	"MATCH_TEAM_ALL_CONTEXT",	EVENT_MATCH_TEAM_ALL_CONTEXT,		// Ŭ���� ����� Context
	"MATCH_TEAM_DETAIL_INFO",	EVENT_MATCH_TEAM_DETAIL_INFO,		// Ŭ�� �� ���� �ޱ�
	"MATCH_FIGHT_QUICK_REQUEST", EVENT_MATCH_FIGHT_QUICK_REQUEST,	// Ŭ���� ���� ��û
	"MATCH_FIGHT_REQUEST",		EVENT_MATCH_FIGHT_REQUEST,			// Ŭ���� ������û
	"MATCH_FIGHT_ACCECT",		EVENT_MATCH_FIGHT_ACCECT,			// Ŭ���� ������û ����

	// Community
	"INSERT_FRIEND",			EVENT_INSERT_FRIEND,				// ģ�� �߰�
	"ACCEPT_FRIEND",			EVENT_ACCEPT_FRIEND,				// ģ�� ����
	"DELETE_FRIEND",			EVENT_DELETE_FRIEND,				// ģ�� ����
	"INVITE_FRIEND",			EVENT_INVITE_FRIEND,				// ģ�� �ʴ��ϱ�
	"INVITED_MESSAGE",			EVENT_INVITED_MESSAGE,				// ģ�� �ʴ�ޱ�
	"SEND_WHISPER",				EVENT_SEND_WHISPER,					// �ӼӸ�
	"SEND_WHISPER_FIND_UID",	EVENT_SEND_WHISPER_FIND_UID,		// Receiver UID�� �ӼӸ�
	"FIND_USER",				EVENT_FIND_USER,					// ģ��ã��
	"SEND_NOTE",				EVENT_SEND_NOTE,					// ���� ������
	"SEND_NOTE_BY_UID",			EVENT_SEND_NOTE_BY_UID,				// ReceiverUID�� ���� ������
	"CHECK_READED_NOTE",		EVENT_CHECK_READED_NOTE,			// ���� ����ǥ��
	"DELETE_NOTE",				EVENT_DELETE_NOTE,					// ���� ���� ��û
	"DELETE_NOTE_ASK",			EVENT_DELETE_NOTE_ASK,				// ���� ���� ����
	"RECEIVE_NOTE",				EVENT_RECEIVE_NOTE,					// ������ �ޱ�

	"INSERT_BLOCK",				EVENT_INSERT_BLOCK,					// ���� �߰�
	"DELETE_BLOCK",				EVENT_DELETE_BLOCK,					// ���� ����

	// Lobby event
	"CREATE_NICK_NAME",			EVENT_CREATE_NICK_NAME,				// ���� �г��� �����	- ������ �ƴϴ�
	"GET_FIRST_SUPPLY",			EVENT_GET_FIRST_SUPPLY,				// �ʵ� ����
	"CREATE_ROOM",				EVENT_CREATE_ROOM,					// �游���							( Lobby )
	"JOIN_ROOM",				EVENT_JOIN_ROOM,					// �濡 ����						( Lobby )
	"JOIN_QUICK",				EVENT_JOIN_QUICK,					// ���� �������� ã�� ����			( Lobby )
	"UPDATE_USER_LIST",			EVENT_UPDATE_USER_LIST,				// �κ��� ���� ����Ʈ�� ���ŉ��.	( Lobby )
	// Readyroom event
	"GET_LOBBY_USER",			EVENT_GET_LOBBY_USER,				// �ʴ��� �ޱ�
	"INVITE_LOBBY_USER",		EVENT_INVITE_LOBBY_USER,			// �ʴ��ϱ�
	"SEARCH_HOST",				EVENT_SEARCH_HOST,					// ������ ���� �ĺ��� ã�´�
	"TRANSFER_HOST",			EVENT_TRANSFER_HOST,				// ���� �ѱ��
	"REQUEST_HOST_CHANGE",		EVENT_REQUEST_HOST_CHANGE,			// ���庯�� ��û

	"ARMEDBOMB_BATTLE",			EVENT_ARMEDBOMB_BATTLE,				// ���� �̼ǿ��� ��ź ��ġ 
	"DISARMBOMB_BATTLE",		EVENT_DISARMBOMB_BATTLE,			// ���� �̼ǿ��� ��ź ��ġ 

	// ��� �̼�(���潺�̼�)
	"DAMAGE_DEFENCE_OBJECT_BATTLE", EVENT_DAMAGE_DEFENCE_OBJECT_BATTLE,	// ��� ������Ʈ ������

	// Quest Event
	"QUEST_CHANGE_ACTIVE_IDX",	EVENT_QUEST_CHANGE_ACTIVE_IDX,		// Active Quest Index�� ���� : stageend���� ����Ʈ �Ϸῡ���� ���� �� ������ ī�峪 ī����� �����Ͽ� active index�� �����Ѱ�� ȣ��	
	"QUEST_FINISH",				EVENT_QUEST_FINISH,					// Quest�� �Ϸ�				 : stage�߿� �Ϸ�Ǵ� ����Ʈ������ ������ �˸�
//	"QUEST_BUY_ONE_CARD",		EVENT_QUEST_BUY_ONE_CARD,			// ī�� ���� ����
	"QUEST_BUY_CARD_SET",		EVENT_QUEST_BUY_CARD_SET,			// ī��� ����
	"QUEST_DELETE_CARD_SET",	EVENT_QUEST_DELETE_CARD_SET,		// ī��� ����

	//// 1.5 Check Designation
	//// ȣĪ
	//"DESIGNATION_GET",			EVENT_DESIGNATION_GET,				// ȣĪ ȹ��
	//"DESIGNATION_CHANGE",		EVENT_DESIGNATION_CHANGE,			// ȣĪ ����
	//"DESIGNATION_RELEASE",		EVENT_DESIGNATION_RELEASE,			// ȣĪ ����

	"GAME_GUARD_AUTH2",			EVENT_GAME_GUARD_AUTH2,				// ���Ӱ��� ����Ű ��û.

	"READY_BATTLE",				EVENT_READY_BATTLE,					// ��Ʋ �غ� �Ϸ�					( ReadyRoom )
	"BATTLE_COUNTDOWN",			EVENT_BATTLE_COUNTDOWN,				// ��Ʋ ���� ī��Ʈ�ٿ�
    
	"DEATH_BATTLE",				EVENT_DEATH_BATTLE,					// �׾����ϴ�.							( Battle )
	"HOLEPUNCH_OK",				EVENT_HOLEPUNCH_OK,					// ������ �ٲ�� CN�� Ȯ�ε��� �˷���..	( Battle )
	"DISCONNECT",				EVENT_DISCONNECT,					// ���� ����
	"TEAM_CHANGE",				EVENT_TEAM_CHANGE,					// ��������
	"RESPAWN_BATTLE",			EVENT_RESPAWN_BATTLE,				// ��Ȱ�߽��ϴ�.						( Battle )
	"RESPAWN_BATTLE_FOR_AI",	EVENT_RESPAWN_BATTLE_FOR_AI,		// �Ʒ��� AI ������
	// Battle event
	"RENDEZVOUS_HOLEPUNCH_BATTLE", EVENT_RENDEZVOUS_HOLEPUNCH_BATTLE,	// ������ ������ Ȧ��Ī�� �մϴ�.
	"PRESTART_BATTLE",			EVENT_PRESTART_BATTLE,				// UDP�� üũ�Ѵ�.						( Battle )	
	"START_BATTLE",				EVENT_START_BATTLE,					// ��Ʋ ����.							( Battle )	
	"GIVEUP_BATTLE",			EVENT_GIVEUP_BATTLE,				// ��Ʋ�� ������.						( Battle )
	"CHANGE_DIFFICULTY_LEVEL",	EVENT_CHANGE_DIFFICULTY_LEVEL,		// �Ʒ��� ���̵� ���� �뺸
	"BATTLE_TOUCHDOWN",			EVENT_BATTLE_TOUCHDOWN,				// �̼� ��ġ�� �����߽��ϴ�.

	"INVITE_CLAN",				EVENT_INVITE_CLAN,					// Ŭ�� �ʴ��ϱ�


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Game -> Context
	"N",						EVENT_N,						// ������ ��޸� �ʿ䰡 ���� �޽���
	"GET_MYOPTION_N",			EVENT_GET_MYOPTION_N,				// �ɼ����� �ޱ�
	"LOBBY_REFRESH_N",			EVENT_LOBBY_REFRESH_N,				// Lobby����
	"LOBBY_PAGE_PREV_N",		EVENT_LOBBY_PAGE_PREV_N,			// Lobby������ ������ ����
	"LOBBY_PAGE_NEXT_N",		EVENT_LOBBY_PAGE_NEXT_N,			// Lobby������ ������ ����
	"CHANGE_ROOM_ALLINFO_N",	EVENT_CHANGE_ROOM_ALLINFO_N,		// ������ ��κ���					( ReadyRoom )
	"CHANGE_ROOMINFO_N",		EVENT_CHANGE_ROOMINFO_N,			// ������ ����						( ReadyRoom )
	"CHANGE_ROOM_PASSWORD_N",	EVENT_CHANGE_ROOM_PASSWORD_N,		// ���н����� ����
	"CHANGE_SLOT_N",			EVENT_CHANGE_SLOT_N,				// ������ �����ٰ� �ݾҴ� �Ѵ�. 
	"CHATTING_N",				EVENT_CHATTING_N,					// ä�� 
	"CHANGE_TEAM_N",			EVENT_CHANGE_TEAM_N,				// ������
	"CHANGE_OBSERVER_SLOT",		EVENT_CHANGE_OBSERVER_SLOT,			// �������������� �̵�
	"CHANGE_HIDDEN_SLOT",		EVENT_CHANGE_HIDDEN_SLOT,			// ���罽������ �̵�(GM�� ����)

	"GET_SLOTINFO_N",			EVENT_GET_SLOTINFO_N,				// ���� ������ ��´�
	"GET_USERINFO_N",			EVENT_GET_USERINFO_N,				// ���� ������ ��´�
	"EQUIPMENT_TEMP_WEAPON_N",	EVENT_EQUIPMENT_TEMP_WEAPON_N,		// �ӽù��� ���� �� Ż��
	"CHANGE_ROOM_OPTION_N",		EVENT_CHANGE_ROOM_OPTION_N,			// �ɼ����� ���� 
	"CHANGE_EQUIPMENT_N",		EVENT_CHANGE_EQUIPMENT_N,			// ������ ��ü						(����)
	"GET_ROOM_ADDINFO_N",		EVENT_GET_ROOM_ADDINFO_N,			//
	"UNREADY_4VS4_N",			EVENT_UNREADY_4VS4_N,				// 4��4 ��� ���� ���� ���� ����	
	"SENDPING_N",				EVENT_SENDPING_N,					// MS�� Ping ������ �����ϴ�.
	"DAMAGEOBJ_BATTLE_N",		EVENT_DAMAGEOBJ_BATTLE_N,			// �ı��̼�
	"OPTION_SAVE_N",			EVENT_OPTION_SAVE_N,				//
	"COMMUNITY_RECORD_DB_N",	EVENT_COMMUNITY_RECORD_DB_N,		// Ŀ�´�Ƽ���� ���� ��û
	"COMMUNITY_RECORD_SESSION_N", EVENT_COMMUNITY_RECORD_SESSION_N, 	// Ŀ�´�Ƽ���� ���� ��û
	"COMMUNITY_RECORD_ALL_N",	EVENT_COMMUNITY_RECORD_ALL_N,		// ���� ����Ʈ������ ���� ��ü ����
	"COMMUNITY_RECORD_ALL_DB_N", EVENT_COMMUNITY_RECORD_ALL_DB_N,	// Ŀ�´�Ƽ â������ ���� ��ü ����
	"COMMUNITY_RECORD_CLAN_N", EVENT_COMMUNITY_RECORD_CLAN_N,		// Ŀ�´�Ƽ���� Ŭ������ ��û
	
	
	// 20080707 ������ �������Դϴ�.

	//Ŭ��
	"CLAN_LIST_N",				EVENT_CLAN_LIST_N,						// ����Ʈ ��û
	"CLAN_REQUEST_LIST_N",		EVENT_CLAN_REQUEST_LIST_N,				// ���Խ�û�� ��Ϻ���
	"CLAN_DETAIL_INFO_N",		EVENT_CLAN_DETAIL_INFO_N,				// �� ���� ��û
	"CLAN_MEMBER_LIST_N",		EVENT_CLAN_MEMBER_LIST_N,				// ��� ��� ���� 
	"CLAN_REQUEST_INFO_N",		EVENT_CLAN_REQUEST_INFO_N,				// ���Խ�û�� ������ ����
	"BATTLE_REMAINTIME_N",		EVENT_BATTLE_REMAINTIME_N,				// ��Ʋ �������� ������ ���� ���� �� ��Ʋ �����ð��� ������ ����
	"CLAN_INVITE_ACCEPT_N",		EVENT_CLAN_INVITE_ACCEPT_N,				// Ŭ�� �ʴ� ����
	

	"MATCH_TEAM_LIST_N",		EVENT_MATCH_TEAM_LIST_N,				// Ŭ���� ����� �ޱ�
	"MATCH_TEAM_ALL_LIST_N",	EVENT_MATCH_TEAM_ALL_LIST_N,			// Ŭ���� ��� �� ��� �ޱ�
	"MATCH_TEAM_CHANGE_PER_RESULT_N", EVENT_MATCH_TEAM_CHANGE_PER_RESULT_N,	// Ŭ���� �ο��� ���� ���
	"MATCH_TEAM_INFO_N",		EVENT_MATCH_TEAM_INFO_N,				// Ŭ���� �� ����	

	"HEART_BIT_N",				EVENT_HEART_BIT_N,
	"N_NORMAL_END",				EVENT_N_NORMAL_END,						//���� ����
	"N_ABNORMAL_END",			EVENT_N_ABNORMAL_END,					//������ ���� 
	"N_QUEST_END_NOTICE",		EVENT_N_QUEST_END_NOTICE,				//�̺�Ʈ ���� ��Ƽ��


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Context -> Game	
	"M",						EVENT_M,							// ���������� ���� �޽��� 
	"M_KICK_USER",				EVENT_M_KICK_USER,					// ���� 
	"M_END_BATTLE",				EVENT_M_END_BATTLE,					// ���� �� 
	"M_DRIVE_AWAY",				EVENT_M_DRIVE_AWAY,					// ���� ���� ������ ��û�� ���� ��������
	"M_ERROR",					EVENT_M_ERROR,						// �������� ������ ��������� ������
	"M_DISCONNECTION",			EVENT_M_DISCONNECTION,				// DISCONNECTIONSUCCESS_ACK�� ���� ó��
	"M_GAMEGUARD_ERROR",		EVENT_M_GAMEGUARD_ERROR,			// ���Ӱ��� ���� 
	"M_TIMEOUTCN",				EVENT_M_TIMEOUTCN,					// TimeOut
	"M_TIMEOUTCS",				EVENT_M_TIMEOUTCS,					// TimeOut ����
	"M_PRE_ROUNDSTART_BATTLE",	EVENT_M_PRE_ROUNDSTART_BATTLE,		// ���� ���� �غ�
	"M_ROUNDSTART_BATTLE",		EVENT_M_ROUNDSTART_BATTLE,			// ���� ����
	"M_ROUNDEND_BATTLE",		EVENT_M_ROUNDEND_BATTLE,			// ���� ��
	"M_HOSTLOST_BATTLE",		EVENT_M_HOSTLOST_BATTLE,			// ���庯��
	"M_INVALID_TIMESYNC",		EVENT_M_INVALID_TIMESYNC,			// �������� ������ ����Ÿ�Ӱ� ����Ÿ���� �����ʴ´ٶ�� �Ǵ�
	"M_DESTROYED_NETWORK",		EVENT_M_DESTROYED_NETWORK,			// ��Ʈ��ũ ������ �����.

	"M_CLAN_MEMBER_INFO",		EVENT_M_CLAN_MEMBER_INFO,			// Ŀ�´�ƼŬ���ɹ� �ޱ�
	"M_MATCH_FIGHT_RESULT",		EVENT_M_MATCH_FIGHT_RESULT,			// Ŭ���� ������û ����
	"M_MATCH_FIGHT_ACCECT_RESULT", EVENT_M_MATCH_FIGHT_ACCECT_RESULT,	// Ŭ���� ������û ����
	"M_MATCH_FIGHT_CHANGE_INFO_RESULT", EVENT_M_MATCH_FIGHT_CHANGE_INFO_RESULT,	// Ŭ���� ������û ����

	"M_MATCH_ROOM_CREATE",		EVENT_MATCH_ROOM_CREATE,			// Ŭ���� ���� �� ����
	"M_MATCH_ROOM_JOIN",		EVENT_MATCH_ROOM_JOIN,			// Ŭ���� ���� �� ����

	"INVITED_CLAN_MESSAGE",		EVENT_INVITED_CLAN_MESSAGE,			// Ŭ�� �ʴ�ޱ�

	"RUN_PARAM_CHECK",			EVENT_RUN_PARAM_CHECK,				// ���� ���� üũ�ϱ�.

	// GM ���
	"KICK_USER",				EVENT_KICK_USER,					// ���� ����
	"EXIT_USER",				EVENT_EXIT_USER,					// �� ���� ��������
	"DESTROY_ROOM",				EVENT_DESTROY_ROOM,					// �� �ı�
	"LOBBY_EXIT_USER",			EVENT_LOBBY_EXIT_USER,				// �κ� ���� ��������
	0);

	if( bReturn == false)
	{
		I3FATAL("Add Enum to Lua failed");
		return false;
	}

	return true;
}
#endif	// #if defined( USE_LUA_UI)

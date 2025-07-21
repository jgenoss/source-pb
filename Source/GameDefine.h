#if !defined ( _GAME_DEFINE_H__ )
#define	_GAME_DEFINE_H__

#include "../../../Common/CommonSource/WeaponDefine.h"

#define GAME_AGE_LIMITED

#define	GAME_DEVICE_KEYBOARD	0
#define	GAME_DEVICE_MOUSE		1
#define	GAME_DEVICE_JOYSTICK	2

#define BATTLE_CHARA_SET	"Gunplay"


// �޴����� �ƴ� ���� ���ӿ��� ���Ǵ� Keyboard Input
#define GAME_KEY_MOVE_LEFT					0x00000001
#define GAME_KEY_MOVE_BACKWARD				0x00000002
#define GAME_KEY_MOVE_RIGHT					0x00000004
#define GAME_KEY_MOVE_FORWARD				0x00000008

#define	GAME_KEY_ACTION_JUMP				0x00000010
#define GAME_KEY_ACTION_RELOAD				0x00000020
#define GAME_KEY_ACTION_CROUCH				0x00000040
#define GAME_KEY_ACTION_FOOTSTEP			0x00000080
#define GAME_KEY_ACTION_ATTACK				0x00000100
#define GAME_KEY_ACTION_SUBFUNC				0x00000200				// ���콺 ������
#define GAME_KEY_ACTION_SPECIAL				0x00000400				// Ư�� ��ư (��ź ��ü��)
#define GAME_KEY_ACTION_EXTENSION_ACTIVE	0x0000008000000000		// ���� Extension Ȱ��ȭ ���
#define GAME_KEY_ACTION_DROP				0x0000010000000000
#define GAME_KEY_ACTION_TURNBACK			0x0000040000000000
#define GAME_KEY_ACTION_WEAPON_EXTENSION	0x0000800000000000		// 'B' ��ư

#define GAME_KEY_CHANGEWEAPON				0x0000020000000000
#define	GAME_KEY_CHANGEWEAPON1				0x00000800
#define GAME_KEY_CHANGEWEAPON2				0x00001000
#define GAME_KEY_CHANGEWEAPON3				0x00002000
#define GAME_KEY_CHANGEWEAPON4				0x00004000
#define	GAME_KEY_CHANGEWEAPON5				0x00008000
#define GAME_KEY_CHANGEWEAPON6				0x00010000
#define GAME_KEY_CHANGEWEAPON_QUICK			0x00020000
#define GAME_KEY_CHANGEWEAPON_PRE			0x00040000
#define GAME_KEY_CHANGEWEAPON_NEXT			0x00080000
#define GAME_KEY_CHANGEWEAPON7				0x0020000000000000
#define GAME_KEY_CHANGEWEAPON8				0x0040000000000000
#define GAME_KEY_CHANGEWEAPON9				0x0080000000000000

#define GAME_KEY_CMD_SHOWSCORE				0x00100000
#define GAME_KEY_CMD_CHAT					0x00200000
#define GAME_KEY_CMD_MOUSESENE_UP			0x00400000
#define GAME_KEY_CMD_MOUSESENE_DOWN			0x00800000
#define GAME_KEY_CMD_RESET					0x01000000
#define GAME_KEY_CMD_VOICECHAT				0x02000000
#define GAME_KEY_CMD_RADIO1					0x04000000
#define GAME_KEY_CMD_RADIO2					0x08000000
#define GAME_KEY_CMD_RADIO3					0x10000000

#define GAME_KEY_FUNC_F2					0x20000000
#define GAME_KEY_FUNC_F3					0x40000000
#define GAME_KEY_FUNC_F4					0x80000000
#define GAME_KEY_FUNC_SCREENSHOT			0x0000000100000000
#define GAME_KEY_FUNC_ESC					0x0000000200000000

#define GAME_KEY_PLUS						0x0000000400000000
#define GAME_KEY_MINUS						0x0000000800000000

#define GAME_KEY_FUNC_CHATSCROLL			0x0000001000000000
#define GAME_KEY_FUNC_TOGGLEMAP				0x0010000000000000

#define GAME_KEY_INGAME_SHOP				0x0001000000000000		// 'Y' ��ư�� ����Ʈ.
#define GAME_KEY_FUNC_VIDEOCAPTURE			0x0002000000000000		// Scroll Lock ��ư�� ����Ʈ
#define	GAME_KEY_SPECIAL_MISSION			0x0004000000000000		// ����'0'�� ����Ʈ


// ������ �ƴ� �޴����� ���Ǵ�  Keyboard Input
#define	GAME_KEY_LEFT						0x00000001
#define GAME_KEY_RIGHT						0x00000002
#define GAME_KEY_UP							0x00000004
#define GAME_KEY_DOWN						0x00000008

#define GAME_KEY_TAB						0x00000010


// ����ϸ� �ȵǴ� Ű�Դϴ�. ���Ŀ� ����
#define GAME_KEY_CMD_ALLDEATH				0x0000002000000000
#define GAME_KEY_CMD_ALLEXPLOSION			0x0000004000000000

//FLYCAMERA���� ����� PIVOTŰ �Դϴ�.	CTRL + (J, L, I, K)
#define GAME_KEY_CMD_CAMPIVOT_LEFT			0x0000080000000000
#define GAME_KEY_CMD_CAMPIVOT_RIGHT			0x0000100000000000
#define GAME_KEY_CMD_CAMPIVOT_UP			0x0000200000000000
#define GAME_KEY_CMD_CAMPIVOT_DOWN			0x0000400000000000

// ���� �޼��� ����Ű �۾��� ���� �߰�. ���� �� ���� Ű �Դϴ�.
#define GAME_KEY_CMD_TEMP_1					0x0000800000000000
#define GAME_KEY_CMD_TEMP_2					0x0001000000000000

enum GAME_LAYER
{
	LAYER_WORLD = 0,
	LAYER_CHARAC,
	LAYER_OBJECT,
	LAYER_EFFECT,
	MAX_LAYER_COUNT,
};

//Collisionn Define
#define		CGRP_MASK_WORLD				0x80000000
#define		CGRP_MASK_WORLD_HERO		0x40000000
#define		CGRP_MASK_CHARA_LINE		0x00000001
#define		CGRP_MASK_CHARA_BOUND		0x00000002
#define		CGRP_MASK_OBJECT			0x00000004
#define		CGRP_MASK_ITEM				0x00000010
#define		CGRP_MASK_CAMERA			0x00000020
#define		CGRP_MASK_TEAM_BLUE_ROUGH	0x00000040
#define		CGRP_MASK_TEAM_BLUE			0x00000080
#define		CGRP_MASK_TEAM_BLUE_ATTACK	0x00000100
#define		CGRP_MASK_TEAM_RED_ROUGH	0x00000200
#define		CGRP_MASK_TEAM_RED			0x00000400
#define		CGRP_MASK_TEAM_RED_ATTACK	0x00000800
#define		CGRP_MASK_WORLD_ATTACK		0x00001000
#define		CGRP_MASK_ATTACK			0x00002000
#define		CGRP_MASK_LIGHTVOLUME		0x00004000
#define		CGRP_MASK_GRENADE			0x00008000
#define		CGRP_MASK_TRIGGER			0x00010000


#define		CGRP_WORLD					0
//#define		CGRP_CHARA_LINE				1		// ��� ����
#define		CGRP_CHARA_BOUND			2		// Network �� ĳ���� ��Ʈ�ѷ� �ø��� �׷�
#define		CGRP_OBJECT_WHOLE_DYNAMIC	3		// hit, ����� ���� ( no character collision)						[Dynamic object]
#define		CGRP_OBJECT					4		// only Character collision(simulate) ( no world collision)			[Static Object, Kinematic Object]
#define		CGRP_ITEM					5		// ������ ������ �ø��� (no character collision)					[����]
//#define		CGRP_CAMERA					6
#define		CGRP_TEAM_BLUE				7		// ��Ʈ�� ĳ���� �ø���
#define		CGRP_TEAM_BLUE_ROUGH		8		// ��Ʈ�� ĳ����
//#define		CGRP_TEAM_BLUE_ATTACK		9
#define		CGRP_TEAM_RED				10
#define		CGRP_TEAM_RED_ROUGH			11
//#define		CGRP_TEAM_RED_ATTACK		12
#define		CGRP_WORLD_ATTACK			13
//#define		CGRP_ATTACK					14
#define		CGRP_LIGHTVOLUME			15
#define		CGRP_CHARA_LIGHTVOL			16
#define		CGRP_ITEM_WHOLE				17		// �ù� & ���̿� ������
#define		CGRP_OBJECT_DYNAMIC			18		// only world collision ( no character collision)					[Dynamic object]
#define		CGRP_OBJECT_HIT				19		// hit ( no world collision)										[Static Object, Kinematic object]
#define		CGRP_OBJECT_WHOLE			20		// only Character collision(Hit, Simulate) ( no world collision )	[Static object, Kinematic object]
#define		CGRP_DUMMY					21		// �ƹ��� �ø����� ���Ѵ�.
#define		CGRP_MYCHARA_BOUND			22		// �ڽ��� ĳ���Ϳ� ��Ʈ�ѷ� �ø��� �׷�
#define		CGRP_DECAL					23		// ��Į�� �׷�
#define		CGRP_OBJECT_LADDER			24
#define		CGRP_TRIGGER				25
#define		CGRP_OBJECT_ALL_NOHIT		26		// ������ ������Ʈ, ��Ʈ�ø����� ����
#define		CGRP_ITEM_HIT				27		// Raycast�� ������													[C5]
#define		CGRP_EFFECT					28		// only world collision, no character collision (����� ��� ����)
#define		CGRP_OBJECT_ALL				29		// ��Ʈ, ĳ����, ����, ������Ʈ�� ����								[Dynamic object]
#define		CGRP_TRIGGER_ALL			30		// Trigger : Character, Object, Weapon ��� Hit�� �� �ִ� Trigger
#define		CGRP_TREX_BOUND				31		//

#define		CGRP_MASKGROUP_ATTACK			0
#define		CGRP_MASKGROUP_CHARA_RED		1
#define		CGRP_MASKGROUP_CHARA_BLUE		2
#define		CGRP_MASKGROUP_CHARA_ROUGH_RED	3
#define		CGRP_MASKGROUP_CHARA_ROUGH_BLUE	4
#define		CGRP_MASKGROUP_OBJECT			5
#define		CGRP_MASKGROUP_GRENADE			6
#define		CGRP_MASKGROUP_ONLYWORLDHIT		7

#define		CGRP_MASKGROUP_ALLATTACK_BLUE	8	// World, object, Blue Chara
#define		CGRP_MASKGROUP_ALLATTACK_RED	9	// World, object, Red chara
#define		CGRP_MASKGROUP_ENVIRONMENT		10

//////////////////////////////////////////////////////////////////////////
//								Network Event							//
// �ۼ��� 2006. 8. 14
// �ۼ��� ������

#define		EVENT_NORESPONSE 0x8000		// ������ ��޸� �ʿ䰡 ���� �޽���

enum GAME_EVENT
{
	EVENT_FIRST_RESPWAN_BATTLE = -1, //Ŭ�� �����..(���� ������ ù��° ������)
	// Global event
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	EVENT_NOTHING			= 0,		// �ƹ��� �̺�Ʈ�� �����ϴ�.

	EVENT_BASE_S			= EVENT_NOTHING,

	EVENT_LOG_IN,						// �α���
	EVENT_LOG_IN_WEBKEY,				// ������ �α���
	EVENT_LOG_OUT,						// ���ϰ��� ���� ���� ��û
	EVENT_LOG_IN_WAIT,					// �α��� ���⿭
	EVENT_DISCONNECT,					// ���� ����
	EVENT_ENTER_LOBBY,					// �κ� ����
	EVENT_LEAVE_LOBBY,					// �κ񿡼� ������(������������)
	EVENT_ENTER_PROFILE,				// 1.1 �����ʷ� �̵�
	EVENT_LEAVE_PROFILE,
	



	EVENT_ENTER_INFO,					// ���������� ����
	EVENT_LEAVE_INFO,					// ���������� ����
	EVENT_ENTER_ROOM,					// �� ����
	EVENT_LEAVE_ROOM,					// �� ����
	EVENT_ENTER_SHOP,					// Shop���� ����
	EVENT_LEAVE_SHOP,					// Shop���� ������ 
	EVENT_GET_NEW_CHALLENGE_CARD,		//
	EVENT_REFRESH_MEDAL,				// �� �޴����� Ȯ�� - NEW ǥ�� ������
	EVENT_CONNECT_SERVER_GATEWAY,		// ����Ʈ���� ������ ����
	EVENT_CONNECT_SERVER_GAME,			// ���ؼ� �Ϸ� üũ
	EVENT_GET_USERINFO,					// ���� ���� �б� (arg : UserDB index)
	EVENT_CREATE_CHARA,					// ĳ���� ���� ���� �б�
	EVENT_SET_CHARA,					// ĳ���� ���� ����
	EVENT_REVERT_CHARA,					// ĳ���� ���� �ǵ�����
	EVENT_REVERT_PARTS,					// ĳ���� ���� �ǵ�����
	EVENT_REPAIR,					// ĳ���� ���� �ǵ�����
	EVENT_DELETE_CHARA,					// ĳ���� ����
	//EVENT_GET_SERVER_LIST,
	EVENT_GET_CHANNEL_LIST,				// ä�� ����Ʈ �о����
	EVENT_SELECT_CHANNEL,				// ä�� ����
	EVENT_USER_LEAVE,					// ���� �̵� ������
	EVENT_USER_ENTER,					// ���� �̵� ������
	EVENT_ENTER_PW,						// ���� �̵� �н�����
	EVENT_M_BLOCK_ACCOUNT,				// ������ �α��� �õ�
	EVENT_SHUTDOWN_TIME_LOGIN,			// �˴ٿ��� �ð� �α��� �õ�
	
	//�ϵ��� �߱� 2011-03-07
	EVENT_RECEIVE_REALNAME,				// �Ǹ����� ��û from Server
	EVENT_RECEIVE_REALNAME_SUCCESS,		// �Ǹ����� ��û ���� from Server
	
	EVENT_BUY_AUTH,						// ! ������.
	EVENT_ITEM_REPAIR,					// ������ ����
	EVENT_REPAIR_EQUIPMENT,				// ������ ��� ����

	EVENT_GET_REMAIN_MONEY,				// ���� ����Ʈ, �� �ޱ�

	EVENT_GAME_GUARD_AUTH2,				// ���Ӱ��� ����Ű ��û.
	EVENT_GAME_GUARD_AUTH3,				// ���Ӱ��� ����Ű ��û.
	EVENT_XTRAP_HACK_DETECTED,			// X-Trap hack detecte.

	// GM ���
	EVENT_KICK_USER,					// ���� ����
	EVENT_EXIT_USER,					// �� ���� ��������
	EVENT_BLOCK_USER,					// ���� ���
	EVENT_DESTROY_ROOM,					// �� �ı�
	EVENT_LOBBY_EXIT_USER,				// �κ� ���� ��������
	EVENT_GM_EXIT_USER,					// ���� ���� ���� �̺�Ʈ

	// �Ƿε� �ý���
	EVENT_FCM_INFO_RETRY,				// �Ƿε� �Ǹ����� Ȯ�� ��û
	// ���¡(Abusing) �ý���
	EVENT_ABUSING_SET,					// ���¡(Abusing) �˾� ��� �Ϸ� ������ ��û
	EVENT_LOADSTAGE,					// �������� �ε� �� Code���� Ȯ���ϱ�

	EVENT_EXIT_DAILYRECORD_SEND,		// ���� ���� ���� ������

	EVENT_READYBOX_MESSAGE,				// ���� ���� �޼���
	EVENT_TUTORIAL_ROUND_END,			// PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END ó��
	EVENT_KICK_CHEAT_USER,		//ġƮ ������ ȥ�� �ֱ� ���� ������ �˷��ش�.
	EVENT_BATTLE_ESCAPEBLOCK,				// �̼� ��ġ�� �����߽��ϴ�.

	EVENT_GAME_END,						// �������� ���� ���� 
	EVENT_COUPON_AWARD,					// ���� ���� �����ޱ�
	EVENT_USER_DETAIL_INFO,				// ���� ������

	EVENT_PLAYTIME_ALARM,				// �÷��� �ð� �˸�
	EVENT_SHUTDOWNTIME_ALARM,			// �˴ٿ� �ð� �˸�
	EVENT_CLOSE_PLAYTIME_ALARM,			// �÷���Ÿ�� �˶� �ݱ�

	EVENT_TIMEOUT_CLIENT,				// TimeOut
	EVENT_TIMEOUT_HOST,					// TimeOut ����

	EVENT_RUN_PARAM_CHECK,				// ���� ���� üũ�ϱ�.

	EVENT_WEBCONTROL_ERROR,				// URL Banner�� Error�� �ʰ� �Ѿ�� Web��ʸ� ���� �̺�Ʈ
	EVENT_REQ_INGAME_URL,				//�ΰ��� URL ��û

	EVENT_HACKTOOL_DETECTED,			// ��ŷ�� Ž��

	//----------------------------------------------------------------------//
	//							PB v 1.5 Event        						//
	//----------------------------------------------------------------------//

	// PB 1.0 = GET_MYINFO - ���߿� �����մϴ�.
	EVENT_GET_USER_INFO,
	//EVENT_GET_CHARA_INFO,
	EVENT_GET_INVEN_INFO,
	EVENT_GET_SYSTEM_INFO,
	
	EVENT_SET_SERVERTAB_SELECT,
	EVENT_SET_CREATECHARA,				//ĳ���� ���� ��Ŷ

	EVENT_CHANGE_CHARA_NICK,

	// ���� ���� ��û 
	// ACK�� 3���� ��� �����ϱ⶧���� PushEventGame�� EVENT_GET_USERINFO_SESSION�� ���
	EVENT_GET_USERINFO_SESSION,
	EVENT_GET_USERINFO_UID,
	EVENT_GET_USERINFO_NICK,	
	EVENT_GET_USERINFO_ROOM,
	EVENT_GET_USERINFO_SUBTASK,

	// ���� �� ����
	EVENT_GET_USER_DETAIL_INFO,

	// �Ϲ����� �ʴ�
	EVENT_INVITE_NORMAL_USER,
	EVENT_INVITED_NORMAL_USER_MESSAGE,

	// ��Ư��
	EVENT_SKILL_CHARA_SET,				// ��Ư�� ĳ���� ����
	EVENT_SKILL_CLASS_SET,				// ��Ư�� Ŭ���� ����
	EVENT_SKILL_SLOT_LEARN,				// ��Ư�� ���� ����
	EVENT_SKILL_SLOT_CANCEL,			// ��Ư�� ���� ���� ���
	EVENT_SKILL_EQUIP_CHANGE,			// ��ų ���� �� ����

	EVENT_USER_GUIDE,					// �������̵� �ý���

	// �ǽ�ī��
	EVENT_PCCAFE_CHANGED_STATUS,		// �ǽ�ī�� ���º�ȭ (�ǽð����� ���ϴ� �ǽ�ī�����ö����� �������� �ƹ����� ����ü�����)

	//------------------------------------------------------------------------------//
	//									GM Option									//
	//------------------------------------------------------------------------------//

	EVENT_GM_AUTHORITY_REQ,					// GM ���� ���� ���� ��û
	EVENT_GM_AUTHORITY_POPUP_OPEN,			// ���� ���� �˾� ����
	EVENT_GM_AUTHORITY_CHAT_SEND,			// GM ä�� �޼��� ������
	EVENT_GM_AUTHORITY_CHAT_RECEIVE,		// GM ä�� �޽��� �ޱ�
	EVENT_GM_AUTHORITY_USER_PENALTY_APPLY,	// �г�Ƽ ���� ��û
	EVENT_GM_AUTHORITY_POPUP_CLOSE,			// ���� ���� �˾� �ݱ�

	EVENT_GM_CHEATKEY_RANDOMBOX,			// Randombox ġƮŰ GM ������ �־�� ��
	//------------------------------------------------------------------------------//
	//									Medal event									//
	//------------------------------------------------------------------------------//

	EVENT_MEDAL_GET_NORMAL_INFO,		// �Ϲ� �޴� ��� ���� ��û
	EVENT_MEDAL_GET_SET_CUR_INFO,		// ��Ʈ �޴�(��������) ��� ���� ��û
	EVENT_MEDAL_GET_SET_COM_INFO,		// ��Ʈ �޴�(�Ϸ��) ��� ���� ��û
	EVENT_MEDAL_GET_REWARD,				// �޴޽ý��� ����ޱ� ��û
	EVENT_MEDAL_GET_ALL_REWARD,			// �޴޽ý��� ��� ����ޱ� ��û
	EVENT_MEDAL_NOTIFY_REGISTER,		// �޴� �˸��� ��� ��û
	EVENT_MEDAL_COMMAND,				// �޴� Ŀ�ǵ�
	EVENT_RANK_UP,

	//------------------------------------------------------------------------------//
	//									QA Command									//
	//------------------------------------------------------------------------------//

	EVENT_QA_COMMAND_INCREASE_KILL_COUNT,	// ųī��Ʈ ����
	EVENT_QA_COMMAND_INCREASE_PLAY_TIME,	// TS �ð� ����
	EVENT_QA_COMMAND_ENABLE_SOLO_PLAY,		// 1�� �÷��� ����ȭ
	EVENT_QA_COMMAND_TELEPORT_SOMEONE,		// �����̵�
	EVENT_QA_COMMAND_DAMAGE_TO_OBJECT,		// ������Ʈ �������ֱ�
	EVENT_QA_COMMAND_DETECT_HACK_OFF,		// �ٰ��� ����
	EVENT_QA_COMMAND_TIMER_GMPAUSE,			// Ÿ�̸ӷ� GM Pause �ϱ�
	EVENT_QA_COMMAND_CONTROL_ROUNDTIME,		// ���� �ð� ����
	EVENT_QA_COMMAND_CLAN_WAR_MATCHING_POINT,		// Ŭ�� ��ġ����ŷ - ��ġ�� ��Ī ����Ʈ ��ȸ
	EVENT_QA_COMMAND_CLAN_WAR_MATCH_RESULT,			// Ŭ�� ��ġ����ŷ - ��ġ ��� ��ȸ

	//------------------------------------------------------------------------------//
	//									Designation ȣĪ							//
	//------------------------------------------------------------------------------//

	EVENT_DESIGNATION_GET,				// ȣĪ ȹ��
	EVENT_DESIGNATION_CHANGE,			// ȣĪ ����
	EVENT_DESIGNATION_RELEASE,			// ȣĪ ����

	EVENT_GET_STAGE,

	EVENT_BASE_E,

	EVENT_BASE_NS = EVENT_NORESPONSE + EVENT_BASE_E,
	EVENT_GET_MYOPTION_N,				// �ɼ����� �ޱ�
	EVENT_CHATTING_N,					// ä�� 
	EVENT_GET_SLOTINFO_N,				// ���� ������ ��´�
	EVENT_GET_USERINFO_N,				// ���� ������ ��´�
	EVENT_COMMUNITY_RECORD_DB_N,		// Ŀ�´�Ƽ���� ���� ��û
	EVENT_COMMUNITY_RECORD_SESSION_N, 	// Ŀ�´�Ƽ���� ���� ��û
	EVENT_COMMUNITY_RECORD_ALL_N,		// ���� ����Ʈ������ ���� ��ü ����
	EVENT_COMMUNITY_RECORD_ALL_DB_N,	// Ŀ�´�Ƽ â������ ���� ��ü ����
	EVENT_COMMUNITY_RECORD_CLAN_N,		// Ŀ�´�Ƽ���� Ŭ������ ��û
	EVENT_OPTION_SAVE_N,				// Option ������ �����Ѵ�.
	//EVENT_OPTION_SAVE_MACRO_N,			// Option Macro ������ �����Ѵ�.

	EVENT_BASE_NE,

	//------------------------------------------------------------------------------//
	//									Shop Event									//
	//------------------------------------------------------------------------------//
	EVENT_SHOP_S = 0x100,

	EVENT_ITEM_AUTH,					// ������ ������ �����ϱ� - ����ϱ�
	EVENT_ITEM_AUTH_DATA,				// ������ ������ �����ϱ� ����Ÿ���� - ����ϱ�
	EVENT_AUTH_DELETE,					// �Ⱓ�� ������ �����ϱ� - ����ϱ�
	EVENT_USE_ITEM_CHECK_NICK,			// �г��� �ߺ��˻�
	EVENT_VIEW_OTHER_USER_ITEM_INFO,	// ���� ���� ����(������)

	EVENT_REQUEST_GOODS_LIST,			// ��ǰ ����Ʈ ��û
	EVENT_REFRESH_CASH,					// ĳ���� ����Ʈ �ֽ�ȭ
	
	EVENT_BUY_GOODS,					// ��ǰ ����
	EVENT_BUY_CHAR_GOODS,				// �ɸ��� ��ǰ ����
	EVENT_BUY_EXTEND_GOODS,				// �Ⱓ ����

	EVENT_ENTER_GACHA,					// Gacha ����
	EVENT_LEAVE_GACHA,					// Gacha ����

	EVENT_ENTER_GACHA_INFO,				// Gacha ���� ��û
	EVENT_END_GACHA_STATE,				// Gacha ���� �ֽ�ȭ
	EVENT_START_GACHA,					// Gacha ����
	EVENT_END_GACHA,					// Gacha ���
	EVENT_GACHA_ANNOUNCE,				// Announce �ֽ�ȭ
	EVENT_GACHA_STATE_REQ,				// Gacha ���� �ֽ�ȭ ��û
	EVENT_GACHA_PURCHASE_COUNT,			// ù ����� Purchase count �ֽ�ȭ ��û
	EVENT_ITEM_AUTH_DATA_CHAGNE,		// ������� ������ ���� ���� ��û

	////////////////////////// Gift Shop /////////////////////////////
	EVENT_ENTER_GIFTSHOP,				// ���� ���� ����
	EVENT_LEAVE_GIFTSHOP,				// ���� ���� ����
	EVENT_FAIL_ENTER_GIFTSHOP,

	EVENT_GET_GIFT_LIST,				// �������� ����Ʈ�� ������ ��û.
	EVENT_SEND_GIFT,					// ��ǰ �����ϱ�
	EVENT_RECEIVE_GIFT,					// ������ �޾Ҵٴ� �̺�Ʈ
	EVENT_AUTH_GIFT_GOODS,				// ��ǰ ���� �ޱ� ���� Ȯ��
	EVENT_CHECK_GIFT_GOODS,				// 
	//////////////////////////////////////////////////////////////////

	EVENT_SHOP_E,

	//------------------------------------------------------------------------------//
	//								Inventory Event									//
	//------------------------------------------------------------------------------//
	EVENT_INVEN_S = 0x180,

	EVENT_ENTER_INVENTORY,				// ��� ���� ȭ������ ����
	EVENT_LEAVE_INVENTORY,				// ��� ���� ����
	EVENT_INVEN_CHANGE_CHARA_EQUIP,			// ĳ���� ����/���� ���� ���� ����
	EVENT_INVEN_E,

	//------------------------------------------------------------------------------//
	//									Clan Event									//
	//------------------------------------------------------------------------------//
	EVENT_CLAN_S = 0x280,

	EVENT_CLAN_ENTER,					// ����
	EVENT_CLAN_LEAVE,					// ������
	EVENT_CLAN_LIST_CONTEXT,			// Ŭ�� ����� ���� Context ��û
	EVENT_CLAN_CREATE,					// �����ϱ�
	EVENT_CLAN_DUPLICATE_NAME,			// �̸� �ߺ��˻� 0 - ����ص� ��,  >0 ������ , <0 ����
	EVENT_CLAN_DUPLICATE_ADDRESS,		// ����Ʈ �ּ� �ߺ��˻� 0 - ����ص� ��,  >0 ������ , <0 ����
	EVENT_CLAN_DUPLICATE_MARK,			// Ŭ����ũ �ߺ��˻� 0 - ����ص� ��,  >0 ������ , <0 ����
	EVENT_CLAN_JOIN_REQUEST,			// ���� ��û�ϱ�
	EVENT_CLAN_JOINCHECK_AUTHORITY,		// ���Ա��� üũ
	EVENT_CLAN_CANCEL_REQUEST,			// ���Խ�û ���.
	EVENT_CLAN_MEMBER_CONTEXT,			// ��� ��� ����
	EVENT_CLAN_MEMBER_LIST,				// ��� ��� �ޱ�
	EVENT_CLAN_REQUEST_CONTEXT,			// ���Խ�û�� �������
	EVENT_CLAN_ACCEPT_REQUEST,			// ���Խ�û�� ����
	EVENT_CLAN_DENIAL_REQUEST,			// ���Խ�û�� ����
	EVENT_CLAN_SECESSION,				// Ŭ�� Ż��
	EVENT_CLAN_COMMISSIONMASTER,		// ������ ����
	EVENT_CLAN_COMMISSIONSTAFF,			// Ŭ�� ���� �Ӹ�
	EVENT_CLAN_COMMISSION_REGULAR,		// Ŭ�� �Ϲ����� ����
	EVENT_CLAN_DISMISSALMEMBER,			// Ŭ���� ����
	EVENT_CLAN_DISSOLUTION,				// ���� ��ü
	EVENT_CLAN_NOTICE_MODIFY,			// �������� ����
	EVENT_CLAN_GUIDE_MODIFY,			// Ŭ���Ұ� ����
	EVENT_CLAN_ACCEPT_OPTION_MODIFY,	// Ŭ�� �ڵ�/���� ���� ����
	EVENT_CLAN_MANAGEMENT,				// Ŭ�� ��Ÿ ���� ����
	EVENT_CLAN_CHANGE_MARK,				// Ŭ����ũ ����
	EVENT_CLAN_MEMBER_INSERT,			// Ŭ���ɹ� �߰�
	EVENT_CLAN_INVITE,					// Ŭ�� �ʴ�
	EVENT_CLAN_NOTE,					// Ŭ�� ��ü����
	EVENT_CLAN_DETAIL_INFO,				// Ŭ�� ��������

	EVENT_CLAN_PERSON_PLUS,				// Ŭ���� �ִ�ġ ����
	EVENT_CLAN_COMMUNITY_LIST,			// Ŭ�� Ŀ�´�Ƽ ���

	
	EVENT_CLAN_E,

	EVENT_CLAN_NS = EVENT_NORESPONSE + EVENT_CLAN_E,

	EVENT_CLAN_LIST_N,						// ����Ʈ ��û
	EVENT_CLAN_LIST_DETAIL_INFO,			// Ŭ�� ����Ʈ�� ������
	EVENT_CLAN_CREATION_INFO,
	EVENT_CLAN_REQUEST_LIST_N,				// ���Խ�û�� ��Ϻ���
	EVENT_CLAN_DETAIL_INFO_N,				// �� ���� ��û
	EVENT_CLAN_MEMBER_LIST_N,				// ��� ��� ���� 
	EVENT_CLAN_REQUEST_INFO_N,				// ���Խ�û�� ������ ����
	EVENT_CLAN_INVITE_ACCEPT_N,				// Ŭ�� �ʴ� ����
	
	EVENT_INVITE_CLAN,					// Ŭ�� �ʴ��ϱ�

	EVENT_CLAN_MATCH_RESULT_CONTEXT,		// Ŭ�� ��ġ ��� ��û
	EVENT_CLAN_MATCH_RESULT_LIST,			// Ŭ�� ��ġ ��� ����Ʈ ��û
	EVENT_CLAN_MATCH_RESULT_EMOTION,		// Ŭ�� ��ġ ��� �̸�� ����

	EVENT_CLAN_WAR_PRESEASON_RANKING,		// Ŭ���� ���� ���� ��ŷ Ȯ��
	EVENT_CLAN_WAR_ALL_MATCHING_INFO,		// ��� ������ Ŭ�� ��Ī ���� ��û (ī��Ʈ)
	EVENT_CLAN_WAR_GET_TEAM_LIST,			// �� Ŭ���� ��ġ �� ��� �ޱ�
	EVENT_CLAN_WAR_CREATE_TEAM,				// Ŭ���� �� ����
	EVENT_CLAN_WAR_JOIN_TEAM,				// Ŭ���� �� ����
	EVENT_CLAN_WAR_LEAVE_TEAM,				// Ŭ���� �� ����
	EVENT_CLAN_WAR_CHANGE_OPERATION,		// Ŭ���� �� ������ ����
	EVENT_CLAN_WAR_CHANGE_MAX_PERSON,		// Ŭ���� �� �ִ� �ο� �� ����
	EVENT_CLAN_WAR_REGISTER_MATCHMAKING,	// Ŭ���� �� ��ġ����ŷ ���
	EVENT_CLAN_WAR_REGISTER_CANCEL_MATCHMAKING,	// Ŭ���� �� ��ġ����ŷ ������

	EVENT_CLAN_WAR_GET_MERCENARY_LIST,		// Ŭ���� �뺴 ����Ʈ ��û
	EVENT_CLAN_WAR_REGISTER_MERCENARY,		// Ŭ���� �뺴 ���.
	EVENT_CLAN_WAR_REMOVE_MERCENARY,		// Ŭ���� �뺴 ��� ���.
	EVENT_CLAN_WAR_SEND_INVITE_MERCENARY,	// Ŭ���� �뺴 �ʴ� ��û(��ġ��)
	EVENT_CLAN_WAR_RECV_INVITE_MERCENARY,	// Ŭ���� �뺴 �ʴ� ����.(�뺴)
	EVENT_CLAN_WAR_ACCEPT_INVITE,			// Ŭ���� �뺴 �ʴ� ����.
	EVENT_CLAN_WAR_DENIAL_INVITE,			// Ŭ���� �뺴 �ʴ� ����.
	EVENT_CLAN_WAR_MERCENARY_DETAIL_INFO,	// Ŭ���� �뺴 �� ����.
	EVENT_CLAN_WAR_RESULT,					// Ŭ�� ��ġ����ŷ ��� ���� ��û
	EVENT_ENTER_TEMP,
	EVENT_LEAVE_TEMP,


	EVENT_CLAN_NE,
	

	//------------------------------------------------------------------------------//
	//									Community									//
	//------------------------------------------------------------------------------//
	EVENT_COMMUNITY_S = 0x300,

	EVENT_INSERT_FRIEND,				// ģ�� �߰�
	EVENT_ACCEPT_FRIEND,				// ģ�� ����
	EVENT_DELETE_FRIEND,				// ģ�� ����
	EVENT_INVITE_FRIEND,				// ģ�� �ʴ��ϱ�
	EVENT_INVITED_MESSAGE,				// ģ�� �ʴ�ޱ�
	EVENT_SEND_WHISPER,					// �ӼӸ�
	EVENT_SEND_MEGAPHONE,				// Ȯ����
	EVENT_SEND_WHISPER_FIND_UID,		// Receiver UID�� �ӼӸ�
	EVENT_FIND_USER,					// ģ��ã��
	EVENT_SEND_NOTE,					// ���� ������
	EVENT_SEND_NOTE_BY_UID,				// ReceiverUID�� ���� ������
	EVENT_CHECK_READED_NOTE,			// ���� ����ǥ��
	EVENT_DELETE_NOTE,					// ���� ���� ��û
	EVENT_DELETE_NOTE_ASK,				// ���� ���� ����
	EVENT_RECEIVE_NOTE,					// ������ �ޱ�
	EVENT_INSERT_BLOCK,					// ���� �߰�
	EVENT_DELETE_BLOCK,					// ���� ����

	EVENT_COMMUNITY_E,

	//------------------------------------------------------------------------------//
	//									Lobby event									//
	//------------------------------------------------------------------------------//
	EVENT_LOBBY_S = 0x400,

	EVENT_CREATE_NICK_NAME,				// ���� �г��� �����	- ������ �ƴϴ�
	EVENT_GET_FIRST_SUPPLY,				// �ʵ� ����
	EVENT_CREATE_ROOM,					// �游���							( Lobby )
	EVENT_JOIN_ROOM,					// �濡 ����						( Lobby )
	EVENT_JOIN_QUICK,					// ���� �������� ã�� ����			( Lobby )
	EVENT_UPDATE_USER_LIST,				// �κ��� ���� ����Ʈ�� ���ŉ��.	( Lobby )
	// Quest Event
	EVENT_QUEST_CHANGE_ACTIVE_IDX,		// Active Quest Index�� ���� : stageend���� ����Ʈ �Ϸῡ���� ���� �� ������ ī�峪 ī����� �����Ͽ� active index�� �����Ѱ�� ȣ��	
	EVENT_QUEST_FINISH,					// Quest�� �Ϸ�				 : stage�߿� �Ϸ�Ǵ� ����Ʈ������ ������ �˸�
	EVENT_QUEST_BUY_CARD_SET,			// ī��� ����
	EVENT_QUEST_DELETE_CARD_SET,		// ī��� ����
	
	// �⼮ üũ �߰�
	//EVENT_ATTENDANCE_INFO,				// �⼮üũ ����
	EVENT_ATTENDANCE_CHECK,				// �⼮üũ
	EVENT_ATTENDANCE_CLEAR,				// �⼮üũ �Ϸ�

	EVENT_LOBBY_E,

	EVENT_LOBBY_NS = EVENT_NORESPONSE + EVENT_LOBBY_E,

	EVENT_LOBBY_REFRESH_N,				// Lobby����
	EVENT_LOBBY_PAGE_PREV_N,			// Lobby������ ������ ����
	EVENT_LOBBY_PAGE_NEXT_N,			// Lobby������ ������ ����
	EVENT_GET_ROOM_ADDINFO_N,			//

	EVENT_LOBBY_NE,
	

	//------------------------------------------------------------------------------//
	//									Readyroom event								//
	//------------------------------------------------------------------------------//
	EVENT_ROOM_S = 0x500,

	EVENT_GET_LOBBY_USER,				// ����뿡�� ���� ���� �ޱ�
	EVENT_GET_INVITE_LOBBY_USER,		// �ʴ��� �ޱ�
	EVENT_INVITE_LOBBY_USER,			// �ʴ��ϱ�
	EVENT_SEARCH_HOST,					// ������ ���� �ĺ��� ã�´�
	EVENT_TRANSFER_HOST,				// ���� �ѱ��
	EVENT_REQUEST_HOST_CHANGE,			// ���庯�� ��û
	EVENT_TEAM_CHANGE,					// ��������
	EVENT_READY_BATTLE,					// ��Ʋ �غ� �Ϸ�
	EVENT_START_GAME,					// ���� ����
	EVENT_BATTLE_COUNTDOWN,				// ��Ʋ ���� ī��Ʈ�ٿ�
	EVENT_CHANGE_READYROOM_INFO,		// �� ���� ���� �̺�Ʈ
	EVENT_TEAM_BALANCE,					// �� ������ �̺�Ʈ
	EVENT_GET_USER_EQUIPMENT_INFO,		// ����� ��� ���� �ޱ�

	EVENT_CHANGE_MAINCHARA_FROM_ROOM,	// �� ĳ���͸� �ٲ۴�.

	EVENT_ROOM_E,

	EVENT_ROOM_NS = EVENT_NORESPONSE + EVENT_ROOM_E,

	EVENT_CHANGE_ROOM_ALLINFO_N,		// ������ ��κ���					( ReadyRoom )
	EVENT_CHANGE_ROOMINFO_N,			// ������ ����						( ReadyRoom )
	EVENT_CHANGE_ROOM_PASSWORD_N,		// ���н����� ����
	EVENT_CHANGE_SLOT_N,				// ������ �����ٰ� �ݾҴ� �Ѵ�. 
	EVENT_CHANGE_TEAM_N,				// ������
	EVENT_CHANGE_OBSERVER_SLOT,			// �������������� �̵�
	EVENT_CHANGE_HIDDEN_SLOT,			// ���罽������ �̵�(GM�� ����)
	EVENT_CHANGE_ROOM_OPTION_N,			// �ɼ����� ���� 
	EVENT_UNREADY_4VS4_N,				// 4��4 ��� ���� ���� ���� ����	
	EVENT_EQUIPMENT_TEMP_WEAPON_N,		// �ӽù��� ���� �� Ż��
	EVENT_GET_ROOM_SCORE, //�� ���ھ� ������ ��û

	EVENT_ROOM_NE,

	//------------------------------------------------------------------------------//
	//									QuickJoin event								//
	//------------------------------------------------------------------------------//
	EVENT_QUICKJOIN_S = 0x600,
	EVENT_QUICKJOIN_START,
	EVENT_QUICKJOIN_SECONDBEST,
	EVENT_QUICKJOIN_E,
	
	//------------------------------------------------------------------------------//
	//									Battle Event								//
	//------------------------------------------------------------------------------//
	EVENT_BATTLE_S = 0x700,

	EVENT_DEATH_BATTLE,					// �׾����ϴ�.							( Battle )
	EVENT_HOLEPUNCH_OK,					// ������ �ٲ�� CN�� Ȯ�ε��� �˷���..	( Battle )
	EVENT_RESPAWN_BATTLE,				// ��Ȱ�߽��ϴ�.						( Battle )
	EVENT_RESPAWN_BATTLE_FOR_AI,		// �Ʒ��� AI ������
	
	// ���и��
	EVENT_RESPAWN_DOMI_DINO,			// ���� �̼� Dino ������
	EVENT_DEATH_DOMI_USER,				// ���� �̼� User Death
	EVENT_WAVE_PRE_START,				// ���� �̼� Wave PreStart
	EVENT_WAVE_END,

	EVENT_RENDEZVOUS_HOLEPUNCH_BATTLE,	// ������ ������ Ȧ��Ī�� �մϴ�.
	EVENT_BATTLE_ENTERANCE_USER,		// �ٸ� ������ ��Ʋ�� �����Ѵ�.
	EVENT_PRESTART_BATTLE,				// UDP�� üũ�Ѵ�.						( Battle )	
	EVENT_START_BATTLE,					// ��Ʋ ����.							( Battle )	
	EVENT_GIVEUP_BATTLE,				// ��Ʋ�� ������.						( Battle )
	EVENT_CHANGE_DIFFICULTY_LEVEL,		// �Ʒ��� ���̵� ���� �뺸
	EVENT_BATTLE_TOUCHDOWN,				// �̼� ��ġ�� �����߽��ϴ�.
	EVENT_BATTLE_DEATHBLOW,				// ������ο�
	EVENT_ARMEDBOMB_BATTLE,				// ���� �̼ǿ��� ��ź ��ġ 
	EVENT_DISARMBOMB_BATTLE,			// ���� �̼ǿ��� ��ź ��ġ
	EVENT_OBJECT_USE_BATTLE,			// �̼� ������Ʈ ���
	EVENT_DAMAGE_DEFENCE_OBJECT_BATTLE,	// ��� ������Ʈ ������

	EVENT_KICK_VOTE_SUGGEST_RESULT,		// ��ǥ ���� ��û�� ���ΰ���� ���� �޽��ϴ�.
	EVENT_KICK_VOTE_START,				// �������� ��ǥ ������ �����մϴ�.
	EVENT_KICK_VOTE,					// ��ǥ�ϱ�	(Game->Context)
	EVENT_KICK_VOTE_COUNT,				// ��ǥ ���� ǥ�� �޽��ϴ�.(Context->Game)
	EVENT_KICK_VOTE_CANCEL,				// �����ĺ��ڰ� ���� ���� �������ϴ�.
	EVENT_KICK_VOTE_RESULT,				// ��ǥ���	(Context->Game)
	EVENT_KICK_VOTE_MY_KICKED,			// ������ ��ǥ ű��������, �����ڰ� �޴¸޽���...

	EVENT_GETINFO_IGSHOP,				// �κ����Խ�,�Ϲݻ���������û�� �Բ� ������, �������� ����������û�̿ܿ�, UI���½ÿ��� ��������ġ�� ���Ե�..
	EVENT_OPEN_IGSHOP,					// ���� ���������� EVENT_ENTER/LEAVE ��� ���½ÿ� ����Ȯ�θ� �մϴ�...

	EVENT_BATTLE_GM_PAUSE,				// GM Pause ��� on��
	EVENT_BATTLE_GM_RESUME,				// GM Pause ��� off��
	EVENT_START_COUNTDOWN,				// ���� ���� ī��Ʈ�ٿ�.
	EVENT_BATTLE_OPTION_SCOPETYPE,		// ������ ����(�������������� �ɼ�)


	EVENT_BATTLE_E,

	EVENT_BATTLE_NS = EVENT_NORESPONSE + EVENT_BATTLE_E,

	EVENT_INGAMESHOP_APPLY_ITEM,		// �ΰ��� ���� ����/���/������ ��� ���� 
	EVENT_SENDPING_N,					// MS�� Ping ������ �����ϴ�.
	EVENT_DAMAGEOBJ_BATTLE_N,			// �ı��̼�
	EVENT_HEART_BIT_N,
	EVENT_N_QUEST_END_NOTICE,				//�̺�Ʈ ���� ��Ƽ��
	EVENT_BATTLE_REMAINTIME_N,				// ��Ʋ �������� ������ ���� ���� �� ��Ʋ �����ð��� ������ ����

	// ���и��
	EVENT_STAGE_JUMP_ROUND_START,			// �������� ���� �� ���� ��ŸƮ �̺�Ʈ
	EVENT_DOMI_SKILL,
	EVENT_DOMI_CHEAT_MESSAGE,
	EVENT_DOMI_INTERMISSION_READY,
	EVENT_DOMI_INTERMISSION_READY_CANCEL,
	EVENT_DOMI_USE_ITEM,

	EVENT_BATTLE_NE,





	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Game -> Context
	EVENT_N = 0x700,
	EVENT_N_NORMAL_END,					//���� ����
	EVENT_N_ABNORMAL_END,				//������ ���� 
	//EVENT_ABNORMAL_LOG, //������ ����α�

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Context -> Game	
	EVENT_M_S = 0x800,					// ���������� ���� �޽��� 

	EVENT_M_KICK_USER,					// ���� 
	EVENT_M_END_BATTLE,					// ���� �� 
	EVENT_M_DRIVE_AWAY,					// ���� ���� ������ ��û�� ���� ��������
	EVENT_M_ERROR,						// �������� ������ ��������� ������
	EVENT_M_DISCONNECTION,				// DISCONNECTIONSUCCESS_ACK�� ���� ó��
	EVENT_M_GAMEGUARD_ERROR,			// ���Ӱ��� ���� 
	EVENT_M_PRE_ROUNDSTART_BATTLE,		// ���� ���� �غ�
	EVENT_M_ROUNDSTART_BATTLE,			// ���� ����
	EVENT_M_ROUNDEND_BATTLE,			// ���� ��
	EVENT_M_HOSTLOST_BATTLE,			// ���庯��
	EVENT_M_INVALID_TIMESYNC,			// �������� ������ ����Ÿ�Ӱ� ����Ÿ���� �����ʴ´ٶ�� �Ǵ�
	EVENT_M_DESTROYED_NETWORK,			// ��Ʈ��ũ ������ �����.
	EVENT_M_SHUTDOWNTIME,				// �˴ٿ����� ���� ���� ����

	EVENT_M_CLAN_MEMBER_INFO,			// Ŀ�´�ƼŬ���ɹ� �ޱ�
	EVENT_M_CLAN_DEPROTATION,			// Ŭ�� ����
	EVENT_M_CLAN_CHANGE_GRADE,			// Ŭ�� ��� ��� ���� ����

	EVENT_M_RECEIVE_USER_SLOT,			// (ReadyRoom) Slot ���� ����

	EVENT_INVITED_CLAN_MESSAGE,			// Ŭ�� �ʴ�ޱ�
	
	EVENT_INVEN_GETINFO,				// ���������� �κ��� �����κ��� �Ϲ������� ���ŵɶ� �� �޼����� ó��...
	EVENT_DOMI_ENTER,					// ���� ���� ó��
	EVENT_FORCE_LEAVE_MATCHROOM,			// Ŭ����ġ - ��ġ�Ǿ� ����� ������϶�, ���� ����� ���� ��ü�� ����

	EVENT_INVEN_REFRESH,				//�κ����� �ֽ�ȭ 
	EVENT_GET_INGAME_URL,				//�ΰ��� URL ���� ���� ��

	EVENT_M_E,

};


//P2P Battle���� ����ϴ� �̺�Ʈ �Դϴ�. ���� 1����Ʈ�� Commond�� ����մϴ�. 
//�߿䵵�� �� �������� �޽������� ����մϴ�.
#define EVENT_BATTLE_MASK			0xFF00000
#define EVENT_BATTLE_KILLCOUNT		0x0200000
#define EVENT_BATTLE_DEATHCOUNT		0x0300000

inline
REAL32	DEVIATION_TIME( REAL32 angle)
{
	return 1.0f; // + log10f( angle);
}

enum CHARA_HIT_PART_SIMPLE
{
	CHPS_UNDKNOWN = -1,
	CHPS_BODY,
	CHPS_ARM,
	CHPS_HAND,
	CHPS_LEG,
	CHPS_FOOT,
	CHPS_COUNT
};

enum ChallengeBulletType
{
	CBT_START = 0,

	CBT_NONE = CBT_START,
	CBT_CHALLENGING,
	CBT_COMPLATE,

	CBT_MAX
};

struct GAMETOCONTEXT_INFO_WEAPON_THROW
{
	T_ItemID			_ItemID = 0;
	WEAPON::ATTACK_TYPE	_AttackType = WEAPON::ATTACK_TYPE_L1;

	VEC3D *				_pPos = nullptr;
	VEC3D *				_pNormal = nullptr;
	INT32				_nBoundType = 0;
	UINT8				_nAddStateFlag = 0;
	UINT8				_ucRemainingTime = 0;
	bool				_bExtension = false;
	VEC3D *				_AttachedObjPos = nullptr;

	GAMETOCONTEXT_INFO_WEAPON_THROW(T_ItemID itemID, VEC3D * pPos, UINT8 State)
		: _ItemID(itemID), _pPos(pPos), _nAddStateFlag(State)
	{
	}
};


#endif

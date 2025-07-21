#if !defined( __UI_DEFINE_H__)
#define __UI_DEFINE_H__

#include "CSI_Stage.h"

//Framework Login Phase �����մϴ�. 
enum	LOGIN_UIPHASE
{
	LOGIN_UIPHASE_START = -1,

	LOGIN_UIPHASE_LOGIN,
	LOGIN_UIPHASE_NATION,

	LOGIN_UIPHASE_MAX
};

// Framework���� �ִ� Event���Դϴ�.
// �� UI���� ó����.
enum UIEVENT
{
	UIEVENT_START = I3_EVT_USER + 100100,

	UIEVENT_CHANGE_AVATAR,
	UIEVENT_CHANGE_WEAPON,
	UIEVENT_CHANGE_PARTS,	// �̸�����
	UIEVENT_EQUIP_PARTS,	// ���
	UIEVENT_UPDATE_CHARA_SLOT,		// ���� ������Ʈ
	UIEVENT_UPDATE_ABILITY,			// Ability update
	UIEVENT_SMENU_CHANGE_ITEM,		// Side Menu Equipment update

	UIEVENT_OPEN_POPUP,		// Popup�� Open
	UIEVENT_CLOSE_POPUP,	// Popup�� Close
	UIEVENT_SET_FOCUS_POPUP,
	UIEVENT_KILL_FOCUS_POPUP,

	UIEVENT_NONEBATTLE_MAX,

	UIEVENT_BATTLE,
	UIEVENT_BATTLE_WEAPONCHANGE,		// ���� ��üâ���� ���� ��ȯ
	UIEVENT_BATTLE_WEAPONCHANGE_CANCEL,
	UIEVENT_BATTLE_MAX
};

// ������ �Ǵ� Phase
// �ݵ�� �� Phase�� �����ϰ� �־�� �Ѵ�.
enum	UIPHASE
{
	UIPHASE_START = -1,
	
	UIPHASE_SERVER_CHANNEL_LIST,
	UIPHASE_CREATE_CHARA,
	UIPHASE_LOBBY,
	UIPHASE_CLANLOBBY,
	UIPHASE_READY_ROOM,

	UIPHASE_MAX
};

// Sub Phase
// �������� ȣ��Ǵ� Phase ����
enum	UISUBPHASE
{
	USP_NONE = -1,
	USP_SHOP,
	USP_INVENTORY,
	USP_GACHA,
	USP_GIFTSHOP,
	USP_PROFILE,
	USP_TEMP,

	USP_MAX
};

// ���� �߿��մϴ�.
// depth�� ���� rendering ������ �����ǹǷ� ���� �������� ������ ��ġ�մϴ�.
enum	UIFLOATWIN
{
	UFW_CHARACTER = 0,		///<< Character View
	UFW_MINIPROFILE,		///<< �����ϴ� ��������â
	UFW_READYROOMINFO,		///<< ReadyRoom ��������â
	UFW_LOBBYMAPINFO,		///<< Room Info â
	UFW_CHAT,				///<< chatting window
	UFW_USERLIST,			///<< UserList
	UFW_PLAYTIMEALARM,		///<< �÷���Ÿ�� �˸�

	UFW_CHANNEL_LIST,		///<< Channel List

	// ����Դϴ�.
	UFW_SCBANNER,			///<< Server & Channel banner
	UFW_LOBBYBANNER,		///<< Lobby Banner
	UFW_NEWANDHOT,			///< New&Hot
	UFW_CLANLOBBY,
	UFW_MERCENARY,			///<< �뺴
	UFW_MISSIONBOARD,		///<< �̼�ī�� �÷ο�
	UFW_READYROOM_USERINFO,	///<< ����� ���� ����(V10)
	UFW_READYROOM_CHAT,		///<< 1.0�� ����� Chat�� ���� ���ϴ�.
	UFW_COMMUNITY,			///Ŀ�´�Ƽ

	UFW_MAX
};

// Popup Window
enum	UIPOPUPWIN
{
	UPW_NULL = -1,
	UPW_START = 0,
	UPW_OPTION = UPW_START,
	
	UPW_BUYITEM,			// BuyItem

	UPW_SHOPEXTENSION,
	UPW_COUPONLIST,			// Coupon List
	UPW_CREATEROOM_EX,		// Create Room Ex

	UPW_MAILBOX,			// MailBox
	UPW_MAILREV,			// Mail ReceiveMail Popup
	UPW_QUICKJOINOPT,		// Quick Join Option
	UPW_USERPROFILE,		// User Profile
	UPW_INVITELOBBYUSER,	// invite lobby user
	UPW_QUERY_PASSWORD,		// Query Pass word
	UPW_REPORT,				// Report �Ű��ϱ�
	UPW_CLAN_CREATE,		// Clan �����ϱ�
	UPW_CLAN_NAME_CHECK,    // Clan Name �ߺ� üũ ȭ�� �߰�  2011-06-29 : ������
	UPW_CLAN_WARNING,		// Clan ������ ���â�� �߰�  2011-06-29 : ������
	UPW_CLAN_JOIN,          // Clan ����â �߱�� �߰�    2011-06-29 : ������
	UPW_LINK_SHOP,
	UPW_CREATENICKNAME,		// Nick ����
	UPW_RANK_UP,			// RankUp �˸�â
	UPW_PCCAFE,				// PC�� �˸�â
	UPW_ADDBLOCK,			// Add Block list
	UPW_PLAYWITH,			// Play With,

	// ������� -->
	UPW_QUERY_ITEM,			// ������ ����� ���� �˾�
	UPW_RESULT_USEITEM,			// ������ ��� ��� �˾�

	UPW_CHANGENICK,				//	�г��� ����
	UPW_CHANGE_COLOR_NICK,		// �÷� �г��� ���� or ClanName
	UPW_CHANGE_CROSSHAIR_COLOR,		//	�÷� ũ�ν����
	UPW_DISGUISE_RANK,			//	���� ���
	UPW_CHANGE_CLAN_MARK,		// Ŭ����ũ ����
	UPW_COLOR_FIRE_MUZZLE_EFFECT,		// �÷� �ѱ� ȭ�� �����
	// --> ������� ���� �ٲٸ� �ȵ˴ϴ�. swoongoo

	UPW_COUPON_INPUT,

	UPW_DISCOUNT_COUPON,	// ���� ����

	UPW_EVENT,				// �̺�Ʈ������

	UPW_EXIT_POPUP,			// (New)���� ����â
	UPW_TS_REWARD,	//TS ����

	UPW_VISIT_EVENT,			// �Ⱓ �� 1ȸ ����		
	UPW_ATTENDANCE_CHECK,		// �⼮ üũ
	UPW_ATTENDANCE_COMPLETE,	// �⼮ üũ �Ϸ�

	UPW_PROFILE,

	UPW_CLANHONOR,	//Ŭ������ ����Ʈ

	UPW_CLAN_NON_MEMBER,
	UPW_CLAN_MEMBER,

	UPW_MEDAL_REWARD,
	UPW_MEDAL_QUICK_REWARD,	// ���� �޴� ����

	UPW_BREAK_INTO_GAME, //���� ���ھ�
	UPW_WEB_BOX,

	UPW_DOMI_SCORE,
	UPW_DOMI_ROUND,
	UPW_DOMI_ENTER,
	UPW_GACHA_WEAPON_DETAIL,
	UPW_GACHA_RESULT,
	UPW_GACHA_HELP_GUIDE,		// �̱����2 Help ���̵�
	UPW_MAILREP,			// Mail SendPopup after Receive
#if legacy_gui_scene
	UPW_ERROR,				// Error
	//UPW_MAILREP,			// Mail SendPopup after Receive
	UPW_ABUSE,				// Abuse �˸�â
	UPW_CLAN_MARKCHANGE_OLD,// Ŭ������ Ŭ����ũ ���� GUI 
	UPW_CHANGE_CLAN_NAME,		// Ŭ������ �̸� ����
#endif

	UPW_QUICK_JOIN,				// �������� ����
	UPW_QUICK_JOIN_SECOND_BEST, // �������� ����å

	UPW_INVEN_DELETE_ITEM_NEW,	// �κ� ������ ���� �ű� UI


	// Key : GM ��ȭ ��� �߰�
	UPW_GM_OPTION,				// GM ��ȭ ��� (GM)
	UPW_GM_CHATBOX,				// GM ä��â (����)

	UPW_MEDAL_ITEM,				// �Ŵ� �������� �߰� 
	
	UPW_GIFT_SEND,				// ���� ������ �˾�

	UPW_SKILLOPEN_ITEM,			// ��ų ���� ������ 
	UPW_SKILLOPEN_RESULT,		// ��ų ���� ������ ���

	UPW_CLAN_SEASON_OVERALL_RANKING,	// Ŭ�� - �������� ��ü ����
	UPW_CLAN_SEASON_INDIVISUAL_RANKING,	// Ŭ�� - �������� ���� ����

	// Key : Ʈ��ġ Ƽ�� ��� �߰�
	UPW_TWITCHLOGIN,			//Ʈ��ġ Ƽ�� �α��� �˾�.
	UPW_TWITCHOPTION,			//Ʈ��ġ Ƽ�� �ɼ� �˾�.
	
	UPW_USERACTION,				// 1.1 ��� �߰� : User Interaction Popup
	UPW_SENDMAIL,				// 1.1 ��� �߰� : Send Mail
	UPW_BASKET,					// 1.1 ��� �߰�
	UPW_QUERY_USEITEM,			// 1.1 ��� �߰� 
	UPW_REPAIR,			// 1.1 ��� �߰� 
	
	UPW_MISSIONINFO,			// �̼�ī�� �˾� (ī������)
	UPW_MISSIONSHOP,			// �̼�ī�� �˾� (ī�屸��)
	UPW_REWARDITEM,				// �Ƹ��� �̼�ī�� �����˾��ϵ�..
	UPW_REWARDITEM_FINALITEM,	// 1.0������ ���յǾ� ���� �ʰ� ������ ����մϴ�. 1.1�� �����ϰ� �� �� �ͽ��ϴ�.
	UPW_REWARDITEM_FINALMASTER,	// ������ �� ���� �˾�

	UPW_SEND_CLANMAIL,			// Ŭ�������� ��ü�޼��� ������
	UPW_COMMUNITY,				// Ŀ�´�Ƽ
	UPW_USERSEARCH,				// 1.0 ���� ã�� �˾�

	UPW_MONTH_ATTENDANCE,		// 31�� �⼮üũ �˾�
	UPW_DESIG_SELECT,			// ȣĪ ���� �˾�
	UPW_DORMANT_EVENT,			// �޸���� �̺�Ʈ �˾�

	UPW_MAX
};

enum UITOOLTIP
{
	UTT_NONE = -1,
	//UTT_ROOMINFO,

	UTT_MAX
};

enum SS_MAINTYPE
{
	SS_MAINTYPE_BEGINNER = 0,		// �ʺ� ����
	SS_MAINTYPE_BEGINNER2,			// �ʺ� ���� 2
	SS_MAINTYPE_NORMAL,				// �Ϲ� ����
	SS_MAINTYPE_EXPERT,				// ��� ����
	SS_MAINTYPE_EXPERT2,			// ��� ���� 2
	SS_MAINTYPE_CLAN,				// Ŭ�� ����
	SS_MAINTYPE_CHAMPIONSHIP,		// ��ȸ ����
	SS_MAINTYPE_NATIONAL,			// �߳��� �����߰�
	SS_MAINTYPE_MIDDLE,				// �߼� ����

	SS_MAINTYPE_EVENT0,				// �̺�Ʈ ����
	SS_MAINTYPE_EVENT1,
	SS_MAINTYPE_EVENT2,
	SS_MAINTYPE_EVENT3,
	SS_MAINTYPE_EVENT4,
	SS_MAINTYPE_EVENT5,
	SS_MAINTYPE_EVENT6,
	SS_MAINTYPE_EVENT7,
	SS_MAINTYPE_EVENT8,
	SS_MAINTYPE_EVENT9,

	SS_MAINTYPE_GWARNET_SILVER,	// �δ� Sliver	// revision 40858
	SS_MAINTYPE_GWARNET_GOLD,	// �δ� ���
	SS_MAINTYPE_GWARNET_CLAN,	// �δ� Ŭ��

	SS_MAINTYPE_MAX,
};

enum SS_SUBTYPE
{
	SS_SUBTYPE_CATEGORY_TEXT = 0,			//ä�� ����
	SS_SUBTYPE_CATEGORY_SLOT_TEXT,			//ä�� �� ����
	SS_SUBTYPE_CATEGORY_SLOT_FULL_TEXT,		//ä�� �� ���� ���� á�� ���
	SS_SUBTYPE_CATEGORY_READY_SLOT_TEXT,	//�κ�濡 ä�� ����

	SS_SUBTYPE_COUNT,
};



enum UI_BGM
{
	UI_BGM_NONE = -1,
	UI_BGM_0,

	UI_BGM_MAX
};

enum UI_MODE_TYPE
{
	UMT_NONE = -1,
	UMT_BOMBMISSION		= 0,	// 0���Ĺ̼�
	UMT_ANNIHILATION	= 1,	// 1 ������
	UMT_DEATHMATCH		= 2,	//2 ��ü��
	UMT_DESTROY			= 3,	//3 �ı��̼�
	UMT_AI				= 4,	//4 �Ʒ���
	UMT_ESCAPE			= 5,	//5 Ż��̼�
	UMT_DEFENCE			= 6,	//6 ���̼�
	UMT_SUDDENDEATH		= 7,	//7 ���絥��
	UMT_HEADHUNTER		= 8,	//8 �������
	UMT_CLANMATCH		= 9,	//9 Ŭ����
	UMT_KNUCKLE,		//10 ��Ŭ��		// UI�� �ִ� ������ ���� ������ ���Ǹ� �ȵ˴ϴ�.
	UMT_SNIPER,			//11 ������
	UMT_SHOTGUN,		//12 ������
	UMT_TUTORIAL		= 13,	//13 Ʃ�丮��
	UMT_DOMINATION		= 14,	//14 ���й̼�
	UMT_CCMODE			= 15,	//15 CC���(����vs�ΰ�)
	UMT_CONVOY			= 16,	//16 ���� Ż�� ��� 
	UMT_RUN_AWAY		= 17,	//17 �ܿ���
	UMT_CHAOS			= 18,	//18 ī�������
	UMT_SPACE			= 19,   //19 �����̽����
	UMT_MADDNES			= 20,   //�޵�Ͻ����
	UMT_HARDCORE_BOMB	= 21,   //�ϵ��ھ� ����
	UMT_HARDCORE_CONVOY = 22,   //�ϵ��ھ� Ż��
};

enum UI_CREATEROOMMODE_TYPE
{
	UMT_CR_NONE = -1,
	UMT_CR_BOMBMISSION		= 0,	// 0���Ĺ̼�
	UMT_CR_ANNIHILATION		= 1,	// 1 ������
	UMT_CR_DEATHMATCH		= 2,	//2 ��ü��
	UMT_CR_DESTROY			= 3,	//3 �ı��̼�
	UMT_CR_AI				= 4,	//4 �Ʒ���
	UMT_CR_ESCAPE			= 5,	//5 Ż��̼�
	UMT_CR_DEFENCE			= 6,	//6 ���̼�
	UMT_CR_SUDDENDEATH		= 7,	//7 ���絥��
	UMT_CR_HEADHUNTER		= 8,	//8 �������
	UMT_CR_CLANMATCH		= 9,	//9 Ŭ����
	UMT_CR_KNUCKLE,					//10 ��Ŭ��		// UI�� �ִ� ������ ���� ������ ���Ǹ� �ȵ˴ϴ�.
	UMT_CR_SNIPER,					//11 ������
	UMT_CR_SHOTGUN,					//12 ������
	UMT_CR_TUTORIAL			= 13,	//13 Ʃ�丮��
	UMT_CR_DOMINATION		= 14,	//14 ���й̼�
	UMT_CR_CCMODE			= 15,	//15 CC���(����vs�ΰ�)
	UMT_CR_SEASON			= 16,	//16 ���� �̺�Ʈ
	UMT_CR_CONVOY			= 17,	//17 ���� Ż�� ��� 
	UMT_CR_RUN_AWAY			= 18,	//18 �ܿ���
	UMT_CR_MADNESS			= 19,
	UMT_CR_CHAOS			= 20,	//ī�������
	UMT_CR_DIEHARD			= 21,   //�����ϵ���
};

enum UI_LIMIT_MODE
{
	ULM_NONE = -1,
	ULM_KNIFE,			// Į��
	ULM_PISTOL,			// ������
	ULM_ROCKET,			// RPG��
	ULM_SNIPER,			// ������
	ULM_SHOTGUN,		// ������
	ULM_KNUCKLE,		// ��Ŭ��
};

enum USERLIST_TAB
{
	USERLIST_TAB_NONE = -1,
	USERLIST_TAB_CURRENT,		// ����� �κ��Դϴ�.
	USERLIST_TAB_FRIEND,		//	ģ�� 
	USERLIST_TAB_CLAN,			//	Ŭ��
	USERLIST_TAB_SEARCH,		//	���� �˻�

	USERLIST_TAB_BLOCK_V11,		// UI 1.1 �� ���� Tab
	
	USERLIST_TAB_MAX
};

enum UI_EVENT_MARK
{
	UIEM_NONE = -1,
	UIEM_PCCAFE,
	UIEM_EVENT,
	UIEM_ITEM,
	UIEM_PCCAFE_PLUS,
	UIEM_PCCAFE_PLUS_SYMBOL,
	UIEM_GARENA_GCA,

	UIEM_MAX
};

struct UIEVENT_OPEN_POPUP_INFO
{
	UIPOPUPWIN		_Type;
	void *			_pParam1;
	void *			_pParam2;
};

enum PhaseState
{
	PS_START = 0,

	PS_IDLE = PS_START,
	PS_ENTRANCE_START,
	PS_ENTRANCING,
	PS_ENTRANCE_END,
	PS_PLAYING,
	PS_EXIT_START,
	PS_EXITING,
	PS_EXIT_END,

	PS_MAX
};

// ������ ����
#define REPAIR_COLOR_BEST	(0xff0ABE5A) // ���� ����
#define REPAIR_COLOR_NORMAL (0xff96DC5A) // ���� ����
#define REPAIR_COLOR_BAD	(0xffF0780F) // ��ȭ ����
#define REPAIR_COLOR_BROKEN	(0xffD20000) // �ҷ� ����







#define UI_LOBBY_ROOMLIST_VIEWCOUNT 10

#define UI_LOBBY_BUTTON_COUNT 5



enum UI_LOBBY_ROOM_STATE
{
	ULRS_NONE = -1,
	ULRS_ADMISSION,		// ���� ����
	ULRS_PLAY,			// ������
	ULRS_NO_ADMISSION,	// ���� �Ұ�
};

struct SORT_ORDER_INFO
{
	INT32	_nType;
	bool	_bAscending;
};

struct LOBBY_ROOM_INFO
{
	bool				_bModify;
	ROOM_INFO_BASIC 	_RoomInfo;
};


#ifndef NC_BUILD
#include "ItemChecker.h"

namespace
{
	void AutoCheckEquipments()
	{
		static bool has_been_visited_lobby = false;
		if(has_been_visited_lobby==false)
		{
			CheckerEquipments()();
			has_been_visited_lobby = true;
		}
	}
}
#endif

namespace SortRoom
{
	enum SORT_LIMIT_TYPE
	{
		SLT_NONE = 0,
		SLT_SHOTGUN,
		SLT_SNIPER,
		SLT_PISTOL,
		SLT_KNUCKLE,
		SLT_KNIFE,
	
		SLT_MAX
	};

	static char priority[129];
	void InitPriority();

	SortRoom::SORT_LIMIT_TYPE GetSortLimitType(UINT8 weaponFlag);

	INT32 value_compare(INT32 lValue, INT32 rValue);

	INT32 priority_compare(const TTCHAR* lhs, const TTCHAR* rhs);

	INT32 SortbyNumber(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);

	INT32 SortbyRoomName(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);
	INT32 SortbyMode(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);
	
	INT32 SortbyUIMode(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);

	INT32 SortbyStageName(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);

	INT32 SortbyRoomState(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);

	INT32 SortbyPerson(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs);
}
#endif

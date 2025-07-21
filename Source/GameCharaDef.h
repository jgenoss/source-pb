#if !defined( _GAME_CHARA_DEF_H__)
#define _GAME_CHARA_DEF_H__

#include "ItemID\\ItemTypeCharaClass.h"
#include "../../../Common/CommonSource\WeaponDefine.h"
#include "../../../Common/CommonSource/NetworkP2PDef.h"
#include "Game.h"

#include "../../LeucoShell/Include/Client/Lib/LeucoShell.h"


#include "MyRoomInfoContext.h"

enum	CHARA_STATE_LIST
{
	// ��ü ����
	CHARA_STATE_CROUCH						= 0 ,
	CHARA_STATE_RUN							,
	CHARA_STATE_WALK						,
	CHARA_STATE_JUMP						,

	//��ü ����
	CHARA_STATE_FIRE						,
	CHARA_STATE_FIREREADY					,
	CHARA_STATE_SECFIRE						,
	CHARA_STATE_RELOAD						,
	CHARA_STATE_SWAPWEAPON					,
	CHARA_STATE_ZOOM						,
	CHARA_STATE_DAMAGE						,

	//��ü ����
	CHARA_STATE_DEATH						,

	//��Ÿ ����
	CHARA_STATE_FIRSTRESPAWN				,
	CHARA_STATE_INVINCIBLE					,
	CHARA_STATE_FLOOR						,
	CHARA_STATE_PHYSIXFREEZE				,
	CHARA_STATE_CLIMB						,
	CHARA_STATE_ONOBJECT					,
	CHARA_STATE_OBSERVER					,
	CHARA_STATE_CAMFREEZE					,
	CHARA_STATE_SLOPESLIDE					,

	CHARA_STATE_USED_COMBINE_ANIM			,
	CHARA_STATE_CHANGEPOSE					,

	CHARA_STATE_ARMINGBOMB					,
	CHARA_STATE_DISARMINGBOMB				,

	CHARA_STATE_DEFAULT						,

	CHARA_STATE_PHYSIXCONTACT				,

	CHARA_STATE_CHANGEDIR					,
	CHARA_STATE_FOOTSTEPRIGHT				,

	CHARA_STATE_JUMPPUSH					,
	CHARA_STATE_DEATHPOSITION				,

	CHARA_STATE_JUMPING_UP					,
	CHARA_STATE_JUMPING_DOWN				,

	CHARA_STATE_FORCEJUMP					,

	CHARA_STATE_MAX
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define	CHARA_STATE_DISABLE_CONTROL			0x00000001			//	ĳ���Ϳ� ���õ� Ű�Է��� ���� �ʴ´�.
#define	CHARA_STATE_DISABLE_SHOT			0x00000002			//	���� �߻簡 �Ұ����ϴ�.
#define	CHARA_STATE_DISABLE_MOVE			0x00000004			//	ĳ���� �̵��� �Ұ����Ѵ�.
#define CHARA_STATE_DISABLE_CROUCH			0x00000008			// �ɱ� �Ұ���
#define CHARA_STATE_DISABLE_ZOOM			0x00000010			// �� ��� �Ұ���
#define	CHARA_STATE_DISABLE_WEAPONCHANGE	0x00000020			//	���� ��ȯ�� �Ұ����ϴ�.
#define CHARA_STATE_DISABLE_RELOAD			0x00000040			// ���� �Ұ���
#define CHARA_STATE_DISABLE_SECFIRE			0x00000080			// ���� �߻� �Ұ���
#define CHARA_STATE_DISABLE_JUMP			0x00000100			// ���� �Ұ���
#define CHARA_STATE_DISABLE_EXTENSION		0x00000200			// Ȯ���� ��� �Ұ���
#define CHARA_STATE_DISABLE_DROPWEAPON		0x00000400			// ���� ������ �Ұ���

#define CHARA_STATE_DISABLE_GRAVITY			0x00000800			// �� �÷��׷� �߷� �����ϵ��� ó��. (���� Ladder Trigger ������ ������ �մϴ�.)

#define			CAPSULE_FULL_HEIGHT			0.65f//0.75f
#define			CAPSULE_HALF_HEIGHT			0.4f
#define			RADIUS						0.39f //0.45f physx 2.8. �� ���� skin width �������� ���� ���� - �ڸ��� 2008. 11. 10.

#define CHARA_LOD_SLOPE		3.5f

enum ACTION_LOAD_MAG_TYPE
{
	ACTION_LOAD_MAG = 0,
	ACTION_LOAD_MAG_QUICK,
	ACTION_LOAD_MAG_B,
	ACTION_LOAD_MAG_BULLET,
	ACTION_LOAD_REMOVE_SHELL,
	ACTION_LOAD_READY_MAG,
	ACTION_LOAD_FIRE_BULLET,		// �߻��� ���� ����
};


namespace CHARA {

	//--------------------------------------
	//	CHARA_TYPE
	//--------------------------------------
	enum TYPE
	{
		TYPE_NA				= -1,
		TYPE_PLAYER,
		TYPE_PLAYER_NET,
		TYPE_NPC,
		TYPE_PROFILE,
		TYPE_AI,					// 8�� ������ ����ϴ� �Ϲ� AI��
		TYPE_DOMI_FULLBODY_AI,		// ���и�� ����
		TYPE_FIRST_VIEW_PLAYER, //1��Ī �÷��̾�, �ѱ� �׼�, ��Ʈ��ũ �̿� �� ��

		TYPE_MAX
	};

	//////////////////////////////////////////////////////////////////////////
	//					 ������ ������ Ÿ�� get �Լ�	                    //
	//////////////////////////////////////////////////////////////////////////
	inline
	CHARACTER_CLASS_TYPE GetClassToBeret(void)
	{
		if( g_pEnvSet->IsV2Version() )
			return CHARACTER_CLASS_HEADGEAR;

		return CHARACTER_CLASS_BERET;
	}

	//////////////////////////////////////////////////////////////////////////

	inline
	ITEM_TYPE		ItemID2Type( T_ItemID itemID)
	{
		return static_cast<ITEM_TYPE>( GET_ITEM_TYPE(itemID) );
	}

	inline
	CHARACTER_TEAM_TYPE ItemID2Team( T_ItemID itemID )
	{
		return static_cast<CHARACTER_TEAM_TYPE>( GET_ITEM_SUBTYPE( itemID) );
	}

	//--------------------------------------
	//	CHARA_RES_ID
	// ItemID �� ResID�Դϴ�.
	//--------------------------------------	
	inline
	CHARA_RES_ID	ItemID2ResID( T_ItemID itemID)
	{
		return (CHARA_RES_ID) GET_ITEM_NUMBER( itemID);
	}

	inline
	CHARACTER_CLASS_TYPE	ItemID2Class( T_ItemID itemID)
	{
		switch( GET_ITEM_TYPE( itemID))
		{
		case ITEM_TYPE_CHARA :		return CHARACTER_CLASS_CHARA_TYPE;
		case ITEM_TYPE_HEADGEAR :	return CHARACTER_CLASS_HEADGEAR;
		case ITEM_TYPE_FACEGEAR :	return CHARACTER_CLASS_FACEGEAR;
		case ITEM_TYPE_UPPER :		return CHARACTER_CLASS_UPPER;
		case ITEM_TYPE_LOWER :		return CHARACTER_CLASS_LOWER;
		case ITEM_TYPE_GLOVE :		return CHARACTER_CLASS_GLOVE;
		case ITEM_TYPE_BELT :		return CHARACTER_CLASS_BELT;
		case ITEM_TYPE_HOLSTER :	return CHARACTER_CLASS_HOLSTER;
		case ITEM_TYPE_SKIN :		return CHARACTER_CLASS_SKIN;
		case ITEM_TYPE_DINO :		return CHARACTER_CLASS_CHARA_DINO;
		case ITEM_TYPE_MISSION :	return CHARACTER_CLASS_RESERVED;
		case ITEM_TYPE_BERET :		return GetClassToBeret();
		default :
			return CHARACTER_CLASS_UNKNOWN;
		}
	}

	inline
	TEAM_TYPE		CharaTeamType2TeamType( CHARACTER_TEAM_TYPE team)
	{
		if( (team & CHARACTER_TEAM_RED) || (team & NPC_TEAM_RED))	return TEAM_RED;
		return TEAM_BLUE;
	}

	// PhysX Shape�� ���� Ÿ��
	enum PHYSX_SHAPE_TYPE {
		PHYSX_SHAPE_MALE = 0,		// ������ ����
		PHYSX_SHAPE_FEMALE,			// ������ ���� (��Ű�� ĳ���͵� ���⿡ �ش�ȴ�)
		PHYSX_SHAPE_ETC,			// ���⿡ �ش�Ǵ� ���� ���°�. 
		PHYSX_SHAPE_TYPE_COUNT
	};

	inline
	PHYSX_SHAPE_TYPE	ResIDToShape( CHARA_RES_ID id)
	{
		switch( id)
		{
			// �ΰ�
		case CHARA_RES_ID_RED_BULLS			:
		case CHARA_RES_ID_ACID_POLE			:
		case CHARA_RES_ID_D_FOX				:
		case CHARA_RES_ID_LEOPARD_MADNESS	:
		case CHARA_RES_ID_LEOPARD_MADNESS_SALES :
		case CHARA_RES_ID_ACID_POLE_MADNESS	:
		case CHARA_RES_ID_ACID_POLE_MADNESS_SALES:
		case CHARA_RES_ID_RF_RED_BULLS :
		case CHARA_RES_ID_RF_ACID_POLE :
		case CHARA_RES_ID_RF_D_FOX :
		case CHARA_RES_ID_RF_LEOPARD :
		case CHARA_RES_ID_RFC_D_FOX :
		case CHARA_RES_ID_RFC_LEOPARD :
		case CHARA_RES_ID_GRS_RED_BULLS :
		case CHARA_RES_ID_GRS_ACID_POLE :
		case CHARA_RES_ID_EXP20_D_FOX :
		case CHARA_RES_ID_EXP20_LEOPARD :
		case CHARA_RES_ID_BOPE_REOPARD :
		case CHARA_RES_ID_LEOPARD		:	return PHYSX_SHAPE_MALE;
		case CHARA_RES_ID_TARANTULA		:
		case CHARA_RES_ID_SWAT_FEMALE	:
		case CHARA_RES_ID_VIPERRED		:
		case CHARA_RES_ID_HIDE			:
		case CHARA_RES_ID_RICA			:
		case CHARA_RES_ID_SWAT_FEMALE_MADNESS :
		case CHARA_RES_ID_SWAT_FEMALE_MADNESS_SALES :
		case CHARA_RES_ID_HIDE_MADNESS	:
		case CHARA_RES_ID_HIDE_MADNESS_SALES :
		case CHARA_RES_ID_POINT30_VIPERRED :
		case CHARA_RES_ID_POINT30_HIDE :
		case CHARA_RES_ID_RF_TARANTULA :
		case CHARA_RES_ID_RF_SWAT_FEMALE :
		case CHARA_RES_ID_RF_VIPERRED :
		case CHARA_RES_ID_RF_HIDE :
		case CHARA_RES_ID_RF_BELLA :
		case CHARA_RES_ID_RF_CHOU:
		case CHARA_RES_ID_RFC_VIPERRED :
		case CHARA_RES_ID_RFC_HIDE :
		case CHARA_RES_ID_GRS_TARANTULA:
		case CHARA_RES_ID_GRS_SWAT_FEMALE :
		case CHARA_RES_ID_KOPASSUS_HIDE :
		case CHARA_RES_ID_HIDE_SOCCER:
		case CHARA_RES_ID_TARANTULA_SOCCER :
		case CHARA_RES_ID_CHOU			:	return PHYSX_SHAPE_FEMALE;
		default :
			// ������ �����ؾ��մϴ�.
			break;
//			return PHYSX_SHAPE_ETC;
		}
//		return PHYSX_SHAPE_MALE;
		return PHYSX_SHAPE_ETC;
	}

	inline
	const char * ItemID2DescIndex( T_ItemID itemID)
	{
		CHARA_RES_ID resID = ItemID2ResID( itemID);

		switch( resID)
		{
		case CHARA_RES_ID_RED_BULLS			:
		case CHARA_RES_ID_RF_RED_BULLS		:	return "STR_CREATECHARA_DESC_REDBULLS";		// Red Bulls
		case CHARA_RES_ID_ACID_POLE			:
		case CHARA_RES_ID_RF_ACID_POLE		:	return "STR_CREATECHARA_DESC_ACIDPOLE";		// acid pole
		case CHARA_RES_ID_TARANTULA			:
		case CHARA_RES_ID_RF_TARANTULA		:	return "STR_CREATECHARA_DESC_TARANTULA";		// tarantula
		case CHARA_RES_ID_SWAT_FEMALE		:
		case CHARA_RES_ID_RF_SWAT_FEMALE	:	return "STR_CREATECHARA_DESC_KEENEYES";		// keen eyes
		case CHARA_RES_ID_VIPERRED			:
		case CHARA_RES_ID_RF_VIPERRED		:
		case CHARA_RES_ID_RFC_VIPERRED		:
		case CHARA_RES_ID_POINT30_VIPERRED	:	return "STR_CREATECHARA_DESC_VIPERRED";		// viperred
		case CHARA_RES_ID_D_FOX				:
		case CHARA_RES_ID_RF_D_FOX			:
		case CHARA_RES_ID_RFC_D_FOX			:
		case CHARA_RES_ID_EXP20_D_FOX		:	return "STR_CREATECHARA_DESC_DFOX";		// D Fox
		case CHARA_RES_ID_LEOPARD			:
		case CHARA_RES_ID_RF_LEOPARD		:
		case CHARA_RES_ID_RFC_LEOPARD		:
		case CHARA_RES_ID_EXP20_LEOPARD		:	return "STR_CREATECHARA_DESC_LEOPARD";		// leopard
		case CHARA_RES_ID_HIDE				:
		case CHARA_RES_ID_RF_HIDE			:
		case CHARA_RES_ID_RFC_HIDE			:
		case CHARA_RES_ID_POINT30_HIDE		:	return "STR_CREATECHARA_DESC_HIDE";		// hide
		case CHARA_RES_ID_RICA				:	return "STR_CREATECHARA_DESC_RICA";		// Rica
		case CHARA_RES_ID_CHOU				:	return "STR_CREATECHARA_DESC_CHOU";		// Chou
		/*RES_ID_CHOU = 33,
		RES_ID_BELLA = 34,
		RES_ID_RF_CHOU = 35,
		RES_ID_RF_BELLA = 36,*/
		}
		return nullptr;	
	}

	//gamecontext �� charamanager�� �ִ� character ���� Ÿ��..
	enum BUFFER_TYPE
	{
		BUFFER_USER = 0,		//�Ϲ� ���� 16��
		BUFFER_NEW_AI,			//���� ��� 32��
		BUFFER_MAX
	};

	inline
	BUFFER_TYPE Type2BufferType( TYPE eType)
	{
		switch( eType)
		{
		case TYPE_NA:
			return BUFFER_USER;

		case TYPE_PLAYER: case TYPE_PLAYER_NET:
		case TYPE_NPC: case TYPE_PROFILE:
		case TYPE_FIRST_VIEW_PLAYER:
		case TYPE_AI:
			return BUFFER_USER;
						
		case TYPE_DOMI_FULLBODY_AI:	
			return BUFFER_NEW_AI;

		default:
			I3ASSERT_0;
			break;
		}

		return BUFFER_USER;
	}

	enum eABILITY
	{
		eABILITY_DAMAGE = 0,
		eABILITY_DAMAGERATE,
		eABILITY_CEBONUS,
		eABILITY_DEFENSE_ARM,
		eABILITY_DEFENSE_BODY,
		eABILITY_DEFENSE_LEG,
		eABILITY_DEFENSE_HAND,
		eABILITY_DEFENSE_FOOT,
		eABILITY_DETECTC5,				// C5 ����(�Ÿ���)
		eABILITY_FALLDAMAGE,
		eABILITY_PROTECTION_RATE,
		eABILITY_INVINCIBLE_TIME,
		eABILITY_MOVESPEED_FRONT,
		eABILITY_MOVESPEED_BACK,
		eABILITY_MOVESPEED_SIDE,
		eABILITY_MOVESPEED_DIAGONAL,
		eABILITY_RESPAWNTIME,
		eABILITY_STBONUS,
		eABILITY_MOVESOUND_VOL,
		eABILITY_ACCURACY,
		eABILITY_MAXBULLET,
		eABILITY_PENETRATE,
		eABILITY_RANGE,
		eABILITY_RELOADTIME,
		eABILITY_SWAPTIME,
		eABILITY_FIREDELAY,
		eABILITY_THROWSPEED,
		eABILITY_THROWCOUNT,
		eABILITY_HPINCREASE,		// ������ + ���
		eABILITY_C4SPEEDKIT,		// ������ + ���
		eABILITY_JUMPHEIGHT,

		eABILITY_COUNT
	};
}


enum GIRAN_NPCRES_TYPE
{
	GIRAN_NPCRES_UNKNOWN = -1,
	GIRAN_NPCRES_TEAM_RED0,			// giran NPC orc
	GIRAN_NPCRES_TEAM_BLUE0,		// giran NPC protector

	GIRAN_NPCRES_TYPE_COUNT
};

#define GIRAN_NPC_INFO_NUM		100

enum SCR_EFFECT_TYPE
{
	SCR_EFFECT_NONE = 0,
	SCR_EFFECT_DAMAGE,
	SCR_EFFECT_FLASHBANG,
	SCR_EFFECT_JUMPER,
	SCR_EFFECT_HOWL,
	SCR_EFFECT_DEATHBLOW,

	SCR_EFFECT_TYPE_COUNT
};

enum CHARA_STATE_FOR_CAM
{
	CSFC_UNKNOWN	= -1,
	CSFC_STAND,
	CSFC_SITMOVE,
	CSFC_SIT,
	CSFC_JUMP,
};

enum GAME_CHARA_NETWORK_MOVE_TYPE
{
	GAME_CHARA_NETWORK_STAND = 0,
	GAME_CHARA_NETWORK_RUN,
	GAME_CHARA_NETWORK_WALK,
};


enum EMoveSpeedType
{
	MST_WALK,
	MST_RUN,
	MST_JUMP_WALK,
	MST_JUMP_RUN,
	MST_COUNT
};

enum EMoveDirectionType
{
	MDT_FRONT,
	MDT_SIDE,
	MDT_DIAGONAL,
	MDT_BACK,
	MDT_COUNT
};

enum EMoveModeType
{
	//��忡 ���� �߰� �̵��� �̳Ѱ�.
	//�߰��� ���� StageTypeToMMT �߰� ���
	//�߰��� CharaInfo, CharaInfo2 �� �� �д� �κ� �߰� ���.

	MMT_NORMAL,
	MMT_SPACE,
	MMT_COUNT

};

namespace CHARAMOVE
{
	inline EMoveModeType StageTypeToMMT(CSI_STAGE stage)
	{
		if (stage.IsSpace())	return MMT_SPACE;
		
		return MMT_NORMAL;
	}

	inline bool	isCurrentStage_Same_MMT(EMoveModeType emovemodetype)
	{
		return ( StageTypeToMMT(*MyRoomInfoContext::i()->getStage()) == emovemodetype );
	}
};

//--------------------------------------
//	AI_CHARA
//--------------------------------------
enum AI_EVASION_TYPE
{
	AIET_SIDECRAWL = 0,
	AIET_MOVE,
	AIET_SIDECRAWLCROUCH,
	AIET_MOVECROUCH,
	AIET_SIDECRAWLFOOTSTEP,
	AIET_MOVEFOOTSTEP,
	AIET_SIDECRAWLCROUCHFOOTSTEP,
	AIET_MOVECROUCHFOOTSTEP,
	AIET_CROUCH,
	AIET_STAND,
	AIET_JUMP,

	AIET_COUNT,
};

enum AI_GRENADE_PATTERN
{
	AI_GRENADE_NO = 0,
	AI_GRENADE_BUSY,			// ������ �� ���� ��ź�� ������.
	AI_GRENADE_LAZY,			// ���� �ϴ� ��, ���� ���� �ʾҴµ� �þ߿��� ����� �ÿ� ��ź�� ������.
	AI_GRENADE_RANDOM			// �����ϰ� ���� 3���� ��, ����
};

struct PB_AI_DATA
{
	REAL32	_PreAttack = 0.0f;					// ai�� ���� �߰��ϰ� �����ϱ����� ���/�غ� �ϴ½ð�	: rate
	REAL32	_Attack = 0.0f;					// ai�� �������� �����ϴ� �ð�							: rate
	REAL32	_AfterAttack = 0.0f;				// ai�� �������� �������� ��ó���ϴ� �ð�				: rate
	REAL32	_TotalTime = 0.0f;					// 1�ֱ⸦ �����ϴµ� �ɸ��� �� �ð�					: length
	REAL32	_Angle = 0.0f;						// ���氢��
	REAL32	_Range = 0.0f;						// �� ���� �̳��� ������� �߰߰���.�� ��ġ������ ���ݰ����� ����.
	REAL32	_Deviation = 0.0f;					// �߻�� �ݵ���
	INT32	_EvasionType = 0;				// ȸ�� Ÿ��
	REAL32	_KeepWatchTime = 0.0f;				// �߿� ����Ʈ ���� �ð�, �ɸ����� ��ü�� Ȱ������ ����Ѵ�.
	INT32	_Priority = 0;					// �߿� ����Ʈ�� �����ϴ� ����Ʈ�� �ּ� Priority�䱸ġ
	REAL32	_SwapWeaponDistance = 0.0f;		// �� �Ÿ����� �հ��� ���� ��������� Į/���ѵ����� �������� �̵�
	INT32	_SwapRatioHandgun = 0;			// �������� ������ �̵�Ȯ��
	INT32	_SwapRatioKnife = 0;			// Į�� �̵�Ȯ��
	REAL32	_TargetHeightUnerNeck = 0.0f;		// ai�� �⺻���� �����ϴ� ����ġ���� �󸶳� �Ʒ��� ��ġ�� ������ ���ΰ��� ����( ��>0 �̸� �� �Ʒ�, ��<0�̸� ����, m����)
};

enum AI_PlAYER_COUNT
{
	AIPLY_1 = 0,
	AIPLY_2,
	AIPLY_3,
	AIPLY_4,

	AIPLY_COUNT,
};

struct AI_FEATURE_DATA
{
	INT32	_Attack_Death_Player = 0;
};

struct AI_BIND_DATA			// <-------- GameCharaManager���� ���Ǵ� ����ü�Դϴ�.
{
	i3::rc_wstring		_wstr_nickname;
	INT32				_rank = 0;
	T_ItemID			_primaryWeaponName = 0;
	T_ItemID			_secondaryWeaponName = 0;
	T_ItemID			_meleeWeaponName = 0;
	T_ItemID			_throwWeaponName = 0;
	T_ItemID			_itemWeaponName = 0;
	INT32				_hp = 0;
	INT32				_respawnTime = 0;
	REAL32				_EvasionRate[AIET_COUNT] = { 0.0f };
	INT32				_SmokeProbability = 0;			// ����ź�� ��ô�ϴ� Ȯ�� (0~100)
	AI_GRENADE_PATTERN	_GrenadePattern = AI_GRENADE_PATTERN::AI_GRENADE_NO;			// ����ź ��ô ���� (�ٷ� ������ : ���� �� ������)	
	PB_AI_DATA			_AIData;
	
	//�ŵ�Ͻ� ���� �߰��Ǵ� �����͵� ( ��ȭ : ReinforceStatus )
	REAL32				_Jump_Height = 0.0f;
	REAL32				_Jump_Time = 0.0f;
	REAL32				_ReduceDamage = 0.0f;
	REAL32				_ReduceDamage_PlayerCount[AIPLY_COUNT] = { 0.0f };
	REAL32				_Speed = 0.0f;
	REAL32				_InvincibleTime = 0.0f;
	AI_FEATURE_DATA		_AIFeature;

	CHARA_PARTS_INFO	_Parts;						// 1.5 Parts
} ;

struct AI_CREATE_DATA
{
	T_ItemID			_chara_item_id = 0;
	INT16				life_time = 0;
	INT8				grade_for_server = 0;

	INT16				_hp = 0;
	VEC3D				_pos;

	AI_CREATE_DATA() {}

	AI_CREATE_DATA( T_ItemID ItemID, INT16 Hp, const VEC3D* Pos, INT16 LifeTime, INT8 Grede) :
	_chara_item_id(ItemID), _hp(Hp), life_time(LifeTime), grade_for_server(Grede)
	{	
		i3Vector::Copy( &_pos, const_cast<VEC3D*>(Pos));
	}
};

struct AI_INTRUSION : AI_CREATE_DATA
{
	INT32				_index = 0;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
//	���� ĳ����
struct GLOBAL_CHARA_INFO
{
private:
	// Status
	VAR_LR(INT32, 76)					_NetIdx = -1;
	VAR_LR(REAL32, 77)					_rHp = -1.0f;					// ĳ���� ���� ü��
	VAR_LR(INT32, 78)					_LoadSlotIdx = -1;
	VAR_LR(CHARACTER_TEAM_TYPE, 79)	_Team = CHARACTER_TEAM_NONE;	// (bitMask) RESPAWN_TEAM_RED, RESPAWN_TEAM_BLUE
	VAR_LR(INT32, 80)					_RespawnCnt = 0;				// ������ Ƚ��
	VAR_LR(CHARA_RES_ID, 81)			_ResType = CHARA_RES_ID_UNKNOWN;
	VAR_LR(CHARA::TYPE, 82)			_Type = CHARA::TYPE_NA;
	VAR_LR(REAL32, 83)					_r32StoreHp = -1.0f;
	VAR_LR(bool, 84)					_bStoredHp = false;

	CHARA_PARTS_INFO	_tParts;


	AI_BIND_DATA *		_pBindData = nullptr;
	bool				_bAIToClient = false;

public:
	GLOBAL_CHARA_INFO() { _tParts.Reset(); }

	bool		GetAIToClient() const;
	void		SetAIToClient( bool bAI)	{ _bAIToClient = bAI; }

	CHARA::TYPE	GetType() const;
	void		SetType( CHARA::TYPE val);

	CHARACTER_TEAM_TYPE	GetTeam( void) const;
	void		SetTeam( CHARACTER_TEAM_TYPE type);

	CHARA_RES_ID	GetResType() const;
	void			SetResType(CHARA_RES_ID val);
	REAL32			GetHP() const;
	void			SetHP(REAL32 val);
	void			DecreaseHP(REAL32 val);
	INT32			GetNetIdx() const;
	void			SetNetIdx(INT32 val);

	INT32			GetRespawnCnt( void) const;
	void			SetRespawnCnt( INT32 cnt);

	void			SaveHP()							{ _r32StoreHp = _rHp; _bStoredHp = true; }
	void			LoadHP()							{ if ( !_bStoredHp ) return; _rHp = _r32StoreHp; _bStoredHp = false; }

	const CHARA_PARTS_INFO* GetParts() const			{ return &_tParts; }
	CHARA_PARTS_INFO *	SetParts()						{ return &_tParts; }
	
	
	AI_BIND_DATA*	GetAIBind( void) const				{ return _pBindData;}
	void			SetAIBind( AI_BIND_DATA * pData)	{ _pBindData = pData;}

	void	Reset()
	{
		_Type		= CHARA::TYPE_NA;
		_LoadSlotIdx = -1;
		//// 1.5 Check CharaTeam
		_Team		= CHARACTER_TEAM_NONE;
		_ResType	= CHARA_RES_ID_UNKNOWN;
		_bAIToClient = false;
		
		SetHP( -1.f);
		SetNetIdx( -1);

		_tParts.Reset();
		_pBindData = nullptr;
		_RespawnCnt = 0;
		_bStoredHp = false;
	}

	void	CopyTo( GLOBAL_CHARA_INFO * pDest )
	{
		pDest->_Type	= _Type;
		pDest->_Team	= _Team;
		pDest->_ResType = _ResType;
		pDest->_rHp		= _rHp;
		pDest->_NetIdx	= _NetIdx;
		pDest->_pBindData = _pBindData;

		_tParts.CopyTo( &pDest->_tParts);

		pDest->_pBindData = _pBindData;
		pDest->_bAIToClient = _bAIToClient;
	}
	bool	IsRaptor() const		{return (GetResType() == CHARA_RES_ID_DINO_RAPTOR);}
	bool	IsRex() const			{return (GetResType() == CHARA_RES_ID_DINO_TREX);}
	bool	IsSting() const			{return (GetResType() == CHARA_RES_ID_DINO_STING);}
	bool	IsTank() const			{return (GetResType() == CHARA_RES_ID_DINO_TANK);}
	bool	IsAcid() const			{return (GetResType() == CHARA_RES_ID_DINO_ACID);}
	bool	IsElite() const			{return (GetResType() == CHARA_RES_ID_DINO_ELITE);}
	
	bool	IsCCRaptor() const		{ return (GetResType() == CHARA_RES_ID_DINO_CC_RAPTOR); }
	bool	IsCCSting() const 		{ return (GetResType() == CHARA_RES_ID_DINO_CC_STING);  }
	bool	IsCCAcid() const		{ return (GetResType() == CHARA_RES_ID_DINO_CC_ACID);   }
	bool	IsRaptorMercury() const	{ return (GetResType() == CHARA_RES_ID_DINO_RAPTOR_MERCURY); }
	bool	IsStingMars() const 	{ return (GetResType() == CHARA_RES_ID_DINO_STING_MARS);  }
	bool	IsAcidVulcan() const	{ return (GetResType() == CHARA_RES_ID_DINO_ACID_VULCAN);   }
	bool	IsDino() const
	{
		CHARA_RES_ID type = GetResType();
		return (type > CHARA_RES_ID_DINO_S) && (type < CHARA_RES_ID_DINO_E);
	}

#ifdef DOMI_DINO
	bool	IsDomiDino() const
	{
		CHARA_RES_ID type = GetResType();
		return ((type == CHARA_RES_ID_DINO_DOMI_RAPTOR) ||
				(type == CHARA_RES_ID_DINO_DOMI_RAPTOR_GREEN) ||
				(type == CHARA_RES_ID_DINO_DOMI_RAPTOR_BLUE) ||
				(type == CHARA_RES_ID_DINO_DOMI_ACID_ITEM) ||
				(type == CHARA_RES_ID_DINO_DOMI_ACID) ||
				(type == CHARA_RES_ID_DINO_DOMI_ACID_NOT_BOMB) ||
				(type == CHARA_RES_ID_DINO_DOMI_STING) ||
				(type == CHARA_RES_ID_DINO_DOMI_MUTANTREX) ||
				(type == CHARA_RES_ID_DINO_DOMI_TANK) ||
				(type == CHARA_RES_ID_DINO_DOMI_RAPTOR_MERCURY) ||
				(type == CHARA_RES_ID_DINO_DOMI_STING_MARS) ||
				(type == CHARA_RES_ID_DINO_DOMI_ACID_VULCAN) );
	}

	bool	IsDomiRaptorBoneBase() const
	{
		CHARA_RES_ID type = GetResType();
		return ((type == CHARA_RES_ID_DINO_DOMI_RAPTOR) || 
				(type == CHARA_RES_ID_DINO_DOMI_RAPTOR_GREEN) ||
				(type == CHARA_RES_ID_DINO_DOMI_RAPTOR_BLUE) ||
				(type == CHARA_RES_ID_DINO_DOMI_ACID_ITEM) ||
				(type == CHARA_RES_ID_DINO_DOMI_ACID) ||
				(type == CHARA_RES_ID_DINO_DOMI_ACID_NOT_BOMB) ||
				(type == CHARA_RES_ID_DINO_DOMI_STING) ||
				(type == CHARA_RES_ID_DINO_DOMI_TANK)||
				(type == CHARA_RES_ID_DINO_DOMI_RAPTOR_MERCURY) ||
				(type == CHARA_RES_ID_DINO_DOMI_STING_MARS) ||
				(type == CHARA_RES_ID_DINO_DOMI_ACID_VULCAN) );
	}

	bool	IsDomiRaptor() const			{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_RAPTOR);			}
	bool	IsDomiRaptorGreen()	const		{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_RAPTOR_GREEN);		}
	bool	IsDomiRaptorBlue() const		{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_RAPTOR_BLUE);		}
	bool	IsDomiAcidItem() const			{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_ACID_ITEM);		}
	bool	IsDomiAcid() const				{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_ACID);				}
	bool	IsDomiAcidNotBomb()	const		{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_ACID_NOT_BOMB);	}
	bool	IsDomiSting() const				{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_STING);			}
	bool	IsDomiMutantRex() const			{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_MUTANTREX);		}
	bool	IsDomiTank() const				{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_TANK);				}
	bool	IsDomiRaptorMercury() const		{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_RAPTOR_MERCURY);	}
	bool	IsDomiStingMars() const			{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_STING_MARS);		}
	bool	IsDomiAcidVulcan() const		{ return (GetResType() == CHARA_RES_ID_DINO_DOMI_ACID_VULCAN);		}
#endif

	bool	IsRaptorBase() const
	{
		CHARA_RES_ID type = GetResType();
		return (type == CHARA_RES_ID_DINO_RAPTOR ||  type == CHARA_RES_ID_DINO_STING ||  type == CHARA_RES_ID_DINO_TANK || type == CHARA_RES_ID_DINO_ACID || type == CHARA_RES_ID_DINO_ELITE
			|| type == CHARA_RES_ID_DINO_CC_RAPTOR || type == CHARA_RES_ID_DINO_CC_STING || type == CHARA_RES_ID_DINO_CC_ACID
			|| type == CHARA_RES_ID_DINO_RAPTOR_MERCURY || type == CHARA_RES_ID_DINO_STING_MARS || type == CHARA_RES_ID_DINO_ACID_VULCAN );
	}
	
	void	setLoadSlotIndex( INT32 idx)			{ _LoadSlotIdx = idx; }
	INT32	getLoadSlotIndex( void) const			{ return _LoadSlotIdx; }

	bool	isValid( void)
	{
		return _tParts.IsValid();
	}
	void TeamChange(void)
	{
		if( GetTeam() == CHARACTER_TEAM_RED)
			SetTeam( CHARACTER_TEAM_BLUE);
		else if(GetTeam() == CHARACTER_TEAM_BLUE)
			SetTeam( CHARACTER_TEAM_RED);

	}

} ;

struct VIEW_CHARA_INFO
{
	UINT8				_ChangeFlag = 0;		///<< ��� ���� ���ο� ���� Flag 1:Chara, 2:Head, 4:Item
	GLOBAL_CHARA_INFO	_charaInfo;

	UINT32 _weapon[WEAPON_SLOT_COUNT] = { 0 };
	INT32 _weaponSlot = -1;

	void CopyTo(VIEW_CHARA_INFO * pInfo)
	{
		pInfo->_ChangeFlag = _ChangeFlag;
		_charaInfo.CopyTo(&pInfo->_charaInfo);

		pInfo->_weaponSlot = _weaponSlot;

		for(INT32 i=0; i<WEAPON_SLOT_COUNT; i++)
			pInfo->_weapon[i] = _weapon[i];
	}

} ;


/////////////////////////////////////////////////////////////////////////////////////////////////
//	������ ����
class WeaponBase;
class CWeaponInfo;

class CHARA_DAMAGEINFO
{
private:
	CHARA_HIT_PART		_HitPart;			// ĳ������ ���� ����

public:

	CHARA_DEATH_TYPE	_DeathType;			//	ĳ���� ���� Ÿ��
	WEAPON::DAMAGE_TYPE	_DamageType;		//	������ Ÿ��
	WEAPON::FIRE_ORDER	_FireOrder;			//	���� �߻� Ÿ��
	bool				_IsOwner;			//	�÷��̾� �ڽ� ����
	INT32				_nKillerIdx;		//	������ Net ID
	CHARA::TYPE			_nKillerType;		//  ������ CHARA::TYPE
	GAMEINFO_HIT_TYPE	_HitType;			//	���� Ÿ��(1:chara, 2:Item, 3:object)
	INT32				_nVictimIdx;		//	������ Net id

	UINT32				_ui32BaseWeaponID;	//	���� ���� ID (UDP Send )
	UINT32				_ui32FireWeaponID;	//	�߻� ���� ID (Client ó����)
	bool				_bExtension;

	REAL32				_rDamage;			//	������ ��

	WeaponBase *		_pWeapon;			// ���� ����
	const CWeaponInfo *	_pWeaponInfo;		// ���� ���� ����

	// xgkick
	REAL32				_rDistance;			// �Ÿ�

	I3_NETWORK_GAMEOBJECT_TYPE	_ObjNetType;	// GHT_OBJECT�� ��� ������Ʈ�� SyncMode

	INT32				_WallShotReduceRate;	// ������ ���� ������ ���ҷ� ( 0 ~ 100) 100�̸� ���� �ȵ�

	REAL32	_rExplosionRange;		//	���� ����
	REAL32	_rExplosionSlope;		//	���� ���� Slope
	REAL32	_rEffectiveTime;		//	ȭ�� ����Ʈ ���� �ð�
	VEC3D *	_pVecPos;				//	���� ������ ��ġ
	VEC3D *	_pVecDir;				//	���� ����

	I3_PHYSIX_HIT_RESULT * _pHitResult;		//	�浹 ���
	I3_PHYSIX_HIT_RESULT * _pWallHitResult;	// wall hit data

	bool				_IsNPC;
	bool				_EnableEffect;		// ����Ʈ�� ��������� ���� ����

	UINT8				_HelmetFlag;		// 1 : Drop, 2 : Protection

	UINT8				_ByObjectState;		// Object�� ���� �ǰݽ� Object State (dedicated)
	UINT16				_ByObjectIndex;		// Object�� ���� �ǰݽ� Object Index (dedicated)

	VEC3D				_vStartPos;			// �ø��� ������
	VEC3D				_vHitPos;			// �ø����� ��ġ

	VEC3D				_vHeadPos;			// ��� ��ġ
	UINT16				_HitByObjExplosion[SLOT_MAX_COUNT];	//������Ʈ ���߿� �¾Ҵ��� üũ

	INT32				_nNetidx;			// ��ô ���� Network Index

	WEAPON::ATTACK_TYPE _AttackType;

	CHARA_DAMAGEINFO::CHARA_DAMAGEINFO() { Reset(); }

	void Reset() {
		_DeathType	= CHARA_DEATH_UN;
		_DamageType = WEAPON::DAMAGE_UNKNOWN;
		_FireOrder	= WEAPON::PRIMARY_FIRE;
		_IsOwner	= false;
		_nKillerIdx = -1;
		_nKillerType= CHARA::TYPE_NA;
		_HitType	=  (GAMEINFO_HIT_TYPE)GHT_AI;
		_nVictimIdx	= -1;

		_ui32BaseWeaponID = 0;
		_ui32FireWeaponID = 0;

		_bExtension = false;

		_rDamage	= 0;
		_pWeapon	= nullptr;
		_pWeaponInfo = nullptr;
		SetHitPart(CHARA_HIT_UNKNOWN);
		
		_rExplosionRange = 0.f;
		_rExplosionSlope = 0.f;
		_rEffectiveTime	= 0.f;
		_pVecPos	= nullptr;
		_pVecDir	= nullptr;
		_rDistance	= 0.0f;

		_pHitResult	= nullptr;
		_pWallHitResult = nullptr;

		_WallShotReduceRate = 0;
		_IsNPC		= false;
		_EnableEffect	= true;

		_HelmetFlag = 0;

		_nNetidx =0;

		_ByObjectState = 0xFF;
		_ByObjectIndex = 0xFFFF;

		i3Vector::Zero( &_vStartPos);
		i3Vector::Zero( &_vHitPos);
		i3Vector::Zero( &_vHeadPos);

		for(int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			_HitByObjExplosion[i] = 0;
		}

		_AttackType = WEAPON::ATTACK_TYPE_L1;
	}

	bool isValid();
	

	bool isHelmProtectDamage()
	{
		if( isFireWeaponType(WEAPON_CLASS_THROWING_GRENADE) ||
			isFireWeaponType((WEAPON_CLASS_TYPE)(WEAPON_CLASS_NPC_ARROW) ) || 
			isFireWeaponType(WEAPON_CLASS_GRENADESHELL) || (_DeathType == CHARA_DEATH_O) )
		{
			return false;
		}
		return true;
	}

	bool isDamageTypeMelee( void)
	{
		if( isFireWeaponType(WEAPON_CLASS_KNIFE) ||	isFireWeaponType(WEAPON_CLASS_KNUCKLE) || 
			isFireWeaponType(WEAPON_CLASS_DUALKNIFE) || 
			(isFireWeaponType(WEAPON_CLASS_CIC) && (_FireOrder == WEAPON::SECONDARY_FIRE)) )
			return true;
		return false;
	}

	bool isDamageTypeRange( void)
	{
		if( isFireWeaponType(WEAPON_CLASS_THROWING_GRENADE) ||
			isFireWeaponType(WEAPON_CLASS_THROWING_CONTAIN) ||
			isFireWeaponType(WEAPON_CLASS_THROWING_ATTACK)	||
			isFireWeaponType(WEAPON_CLASS_THROWING_HEAL)	||
			isFireWeaponType(WEAPON_CLASS_GRENADESHELL) )
			return true;
		return false;
	}

	// ����� ���ְ� Ư������ ���� ���
	// Host�� �ؾ��ϴ� ����̴�. Dedicated������ �ڽ��� �����ϴ� ���
	// Grenade, Throw weapon�� �ش�ȴ�.
	bool	IsCalculateOwned( bool bDedicatedMode) ;
	bool	isFireThrowingKnife( void);

	UINT32	GetRealWeaponNumber();

	void	SetDeathTypeByHitPart( CHARA_HIT_PART hitPart, CHARA_DEATH_TYPE deathType);

	CHARA_HIT_PART	GetHitPart() const
	{
#if defined (USE_ARXAN)
		CHARA_HIT_PART _temp;
		AIP_Getter_CHARA_DAMAGEINFO_HitPart(&_temp,&_HitPart,sizeof(_temp));
		return _temp;
#else
		return _HitPart;
#endif
	}

	void	SetHitPart(CHARA_HIT_PART hitPart)
	{
#if defined (USE_ARXAN)
		AIP_Setter_CHARA_DAMAGEINFO_HitPart(&_HitPart,&hitPart,sizeof(_HitPart));
#else
		_HitPart = hitPart;
#endif
	}

	void	SetWeaponItemID(WeaponBase * pWeapon);
	void	SetWeaponItemID(CWeaponInfo * pCurWeaponInfo, CWeaponInfo * pBaseWeaponInfo );

	WEAPON_CLASS_TYPE	GetFireWeaponClass(void)			{ return (WEAPON_CLASS_TYPE) GET_ITEM_SUBTYPE(_ui32FireWeaponID); }
	WEAPON_SLOT_TYPE	GetFireWeaponSlot(void)				{ return GetWeaponSlot_WeaponItemIDFunc(_ui32FireWeaponID); }
	INT32				GetFireWeaponNumber(void)			{ return (INT32)GET_ITEM_NUMBER(_ui32FireWeaponID); }

	bool	isFireWeaponType(WEAPON_CLASS_TYPE classType)	{ return GetFireWeaponClass() == classType; }
	bool	isFireWeaponSlot(WEAPON_SLOT_TYPE slotType)		{ return GetFireWeaponSlot() == slotType; }
	bool	isFireWeaponNumber( INT32 i32Idx, bool isWit = false);
	bool	isRealWeaponNumber( INT32 i32Idx);

	void	DumpDamageInfo(void)
	{
		I3TRACE("============== Dump DAMAGEINFO ================\n");
		I3TRACE("_DeathType = %d\n",			_DeathType);
		I3TRACE("_DamageType = %d\n",			_DamageType);
		I3TRACE("_ui32BaseWeaponID = %d\n",		_ui32BaseWeaponID);
		I3TRACE("_ui32FireWeaponID = %d\n",		_ui32FireWeaponID);
		I3TRACE("_rExplosionRange = %f\n",		_rExplosionRange);
		I3TRACE("_rExplosionSlope = %f\n",		_rExplosionSlope);
		I3TRACE("_rEffectiveTime = %f\n",		_rEffectiveTime);
		I3TRACE("_IsOwner = %d\n",				_IsOwner);
		I3TRACE("_rDamage = %f\n",				_rDamage);
		I3TRACE("_WallShotReduceRate = %d\n",	_WallShotReduceRate);
		I3TRACE("_HitType = %d\n",				_HitType);
		I3TRACE("_HitPart = %d\n",				GetHitPart());
		I3TRACE("_AttackType = %d\n",			_AttackType);
		I3TRACE("===============================================\n");
	}
};

struct CHARA_MATERIAL
{
	i3MaterialAttr *	m_pMaterial = nullptr;
	COLORREAL	diffuse;
	COLORREAL	specular;
	COLORREAL	emissive;
	REAL32		shininess = 0.0f;
} ;

i3::rc_wstring GetCharaRank(INT32 rank);


#endif





#pragma once

namespace HUD
{
	enum HUD_STYLE 
	{ 
		HS_1_5, HS_1_0, HS_MAX
	};

	enum NoticeMessageType
	{
		NMT_Challenge,
		NMT_Tutorial,
	};

	enum CCVSMessageType
	{
		CCVSMT_PreStartRound,
		CCVSMT_Battle
	};

	enum ChallengeMessage
	{
		CHM_NONE = -1,

		CHM_20_KILL,
		CHM_40_KILL,
		CHM_60_KILL,
		CHM_80_KILL,
		CHM_100_KILL,
		CHM_120_KILL,
		CHM_140_KILL,
		CHM_160_KILL,
		CHM_180_KILL,
		CHM_200_KILL,

		CHM_5_DEATH,
		CHM_20_DEATH,

		CHM_5_CHAIN_KILL,
		CHM_20_CHAIN_KILL,

		CHM_PIERCING_KILL,
		CHM_MASS_KILL,
		CHM_CHAIN_SLUGGER,
		CHM_CHAIN_STOPPER,

		CHM_BATTLE_START,

		MAX_CHM_MESSAGE
	};

	enum UIType
	{
		FIRST_HUD,

		PLAYER_STATE,
		PLAYER_DINO_STATE,
		AMMO_STATE,
		DINO_STATE,
		DASH_BOARD,
		MINIMAP,
		MISSION_BOARD,
		NOTICE,
		CHALLENGE_BOARD,
		CROSS_COUNTER_VS,
		RESPAWN_TIMER,
		TARGET_OBJECT,
		FLOAT_BOARD,
		GAME_RESULT,
		QUEST_EFFECT,
		DEFAUTOCHANGE,
		C4INDICATOR,
		WINTERMODE,
		MADNESSNOTICE,
		BATTLEUSEITEM,
		MULTIWEAPON,

		LAST_HUD,
		UIT_MAX,
	};
	typedef i3::vector<UIType> UITypeList;

	enum CrossCountTeamMark
	{
		RED_HUMAN_OTHERTEAM = 0,
		RED_HUMAN_MYTEAM,

		BLUE_DINO_OTHERTEAM,
		BLUE_DINO_MYTEAM,

		BLUE_HUMAN_OTHERTEAM,
		BLUE_HUMAN_MYTEAM,

		RED_DINO_OTHERTEAM,
		RED_DINO_MYTEAM,
	};

	enum RespawnType
	{
		RT_START,
		RT_SEND_BLOCK,
		RT_SEND_UNBLOCK,

		RT_HOLD,
		RT_RELEASE_HOLD,
		RT_IMMEDIATE,		//immediate
		RT_RESET,
	};

	enum 
	{
		ObserverOffsetX = -100,
	};

	enum MISSION_MODE
	{
		MISSION_NONE = -1,
		MISSION_DEFAULT,
		MISSION_OBSERVER,		
	};

	enum TargetShapeType
	{
		TST_NONE = -1,
		TST_HELICOPTER,
		TST_GIRAN,
		TST_TANK,
		TST_GENERATOR,
	};

	enum KillStarType
	{
		KST_DISABLE = -1,
	};

	enum ItemState //���� �Ǵ� ����
	{
		IS_NONE = -1, //�������� �����ϰ� ���� �ʴ�.
		IS_HAVE, //������..
		IS_NOT_HAVE, //���� �� �ϰ� ����.
		IS_USE, //�����
		IS_NOT_USE, //������
		IS_HAVE_MISSION_OBJ, //������
	};

	enum GameResultState
	{
		GRS_Ok, //ok ��ư
		GRS_ScreenSave,  //ScnShot
		GRS_WinTeamState, //win team
		GRS_UpdateKillCount, //���� ų ������Ʈ(�ش� ���� �������� ����)
		GRS_UserRoomIn, //���� ����
		GRS_UserRoomOut, //���� ����
		GRS_HeadShot, //��弦 ī����
		GRS_UserKilled, //�ǰ�����
		GRS_Medal, //�޴� ȹ��
		GRS_RoundShow, //���� ����
		GRS_EndShow, //���� ����
	};

	enum DashBoardState
	{
		DBS_TargetShow,
		DBS_SetGameTimeZero,
		DBS_SetRunningTime,
		DBS_SetRidingHelicopter,
		DBS_SetUnderAttack,
	};

	enum MadnessNoticeState
	{
		MN_FINISH,		//�ƹ��͵� ���� ���ϴ� ���� ���ο��� ���.
		MN_START,		//���� ����ڴ� �̰͸� ��.
		MN_UPDATING,	//���ο��� ���.
		MN_STOP,		//���� ���ο��� ���.
	};
}

namespace DinoState
{
	enum Attack	// �̹��� �ε����� ��Ī�Ǵ� ������ ����. PBRe_HUD.i3UIe�� Attack1 ��Ʈ�� ���ø� �����ٶ�.
	{
		None = -1,

		HOWL,				//STR_TBL_INGAME_HUD_DINO_ATTACK_HOWL
		SCRATCH,			//STR_TBL_INGAME_HUD_DINO_ATTACK_SCRATCH
		BITE,				//STR_TBL_INGAME_HUD_DINO_ATTACK_BITE
		ELITE_SCRATCH,		//STR_TBL_INGAME_HUD_ELITE_ATTACK_A
		RAPTOR_BUFF,
		POISON_PRICKLE,		//STR_TBL_INGAME_HUD_DINO_ATTACK_POISON_PRICKLE
		STING_BITE,
		BLAST,				//STR_TBL_INGAME_HUD_DINO_ATTACK_BLAST
		BUTT,
		RUSH,
		POISON_GAS,

		MAX_COUNT,
	};

	enum DinoEvent
	{
		De_Show,
		De_AttackIndex1,
		De_AttackIndex2,
		De_SkillGuage,
		De_Name,
	};
}

namespace ResultState
{
	struct UserInform
	{
		i3::rc_wstring wstr_nick; //i3::string ����ϸ� ������ ������

		UINT32 clan_idx, clan_mark; //Ŭ�� idx, Ŭ�� ��ũ
		INT32	idx, score, rank; //user idx, team score, ���

		UINT16 point, exp;								// ����Ʈ, ����ġ
		UINT16 total_bonus_point, total_bonus_exp;		// �� ���ʽ� ����Ʈ, �� ���ʽ� ����ġ
		UINT16 kill_count, death_count, assist_count;	// ų, ����, ��ý�Ʈ ī����
		bool QuestEndFlag;

		UINT8	 mission, result_mark, nick_color, e_sport_mark; //����Ʈ, ���ʽ� ����Ʈ ��ũ, �г��� ����, e sport ��ũ
	
		bool is_challenge; //ç���� ����
		bool is_my_inform; //�� ���� ����
	};
}

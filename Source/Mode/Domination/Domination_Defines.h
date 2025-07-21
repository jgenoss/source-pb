#pragma once

#include "../../../../../Common/CommonSource/CommonDediCli.h"
#include "DomiUserSkillMgr.h"

//=========================================
// ModeScript ������
//=========================================
namespace ModeScript
{
	//======================================================
	enum NEW_FLOW_TYPE
	{
		NF_NONE	= 0,
		NF_INTERMISSION,
		NF_DEFENSE,
		NF_SURVIVAL,
		NF_DESTROY,
		NF_COLLECTION,
		NF_TOWERDEFENSE,
		NF_BOSS,
		NF_MAX
	};	


	// Flow ��ǥ ���� �ȿ��� ���� Mission�� �����Ѵ�.
	enum WAVE_MISSION_TYPE
	{
		WAVE_MISSION_NONE	= 0,
		WAVE_MISSION_TIMELIMIT,
		WAVE_MISSION_ELIMINATE,
		WAVE_MISSION_RECOVERY,
		WAVE_MISSION_RECOVERY2,
		WAVE_MISSION_DEFENCE,
		WAVE_MISSION_DESTROY,
		WAVE_MISSION_BOMB,
		WAVE_MISSION_BLOCK,

		WAVE_MISSION_MAX
	};

	const char*		GetFlowName( INT32 val);
	const char*		GetFlowMessage( INT32 val);
};

struct DOMI_BOSS_HP
{
	REAL32	boss_max_hp;
	CGameCharaBase* boss;

	DOMI_BOSS_HP( REAL32 max, CGameCharaBase* p)
	{ 
		boss_max_hp = max;
		boss = p;
	}
};

struct DOMI_SP
{
	UINT16 sp[4];
};

enum DOMI_REWARD_GRADE_STATE
{
	DOMI_REWARD_GRADE_STATE_NONE,
	DOMI_REWARD_GRADE_STATE_FIRST_WAVE,	//1wave
	DOMI_REWARD_GRADE_STATE_USER_ENGER,	//���� ���� �� ������
	DOMI_REWARD_GRADE_STATE_USER_LEAVE,	//���� ����
};

namespace DomiDefs
{
	struct FLOW
	{
		REAL32	play_time;
		INT16	round;
		INT8	max_wave;		//max ���̺�
		INT8	wave_count;		//current ���̺�

		REAL32	wave_pre_counter;
		REAL32	intermission_time;

		UINT32	total_score[4];
		INT16	max_dino;
		INT16	dino_count;
	};

	struct INTERMISSION
	{
		bool    inIntermission;
		bool	intermission_ready[4];
	};

	struct REVIVAL
	{
		UINT32	single_respawn_count;
		UINT32	everyone_respawn_count;
	};

	struct MUTANTREX_HP
	{
		REAL32	boss_max_hp;
		CGameCharaBase* boss;

		MUTANTREX_HP() {}
		MUTANTREX_HP( REAL32 max, CGameCharaBase* p)
		{ 
			boss_max_hp = max;
			boss = p;
		}
	};

	struct MUTANTREX_HOWL
	{
		INT32  boss_howling_time;

		INT32 boos_dash_target_type;
		INT32  boss_dash_target_idx;

		REAL32 boss_howl_cancel_threshold;
		REAL32 boss_cur_hp_at_howl_start;
	};

	struct AWARD
	{
		UINT32	award_item;			// ������ ������ID
		UINT32	award_item_arg;		// �Ⱓ �Ǵ� ����
		UINT16	award_point;		// point
	};

	struct ACIDITEM_APPEARENCE
	{
		REAL32	etc_time;			// �ֽõ� ������ ���� ���� �ð�.
		INT8	etc_count;			// �ֽõ� ������ ���� ���� ��.
	};

	struct GRADE
	{
		DOMI_REWARD_GRADE_STATE grade_state;
		i3::string grade_user_nick;
	};

	struct RESULT_BASE
	{
		INT16 kill;
		INT32 score;
	};
	struct WAVE_RESULT_INFO
	{
		RESULT_BASE base;
		INT32 slotIdx;
	};
	struct ROUND_RESULT_INFO
	{
		RESULT_BASE base;
		UINT8 icon;
		INT16 exp;
		INT16 point;
		INT32 slotIdx;
	};
}

//�������� ���� �Ŀ��� ���� ���� ����.
//���� �̼��� ������ ��� ����.
template <> struct is_tinst_manual<struct domi_state_info> : std::tr1::true_type {};
struct domi_state_info
{
	domi_state_info() {}
	~domi_state_info() {}

	static domi_state_info* i() { return tinst<domi_state_info>(); }

	static void create()
	{ 		
		if( tinst<domi_state_info>() == nullptr)
		{
			tinst<domi_state_info>() = new domi_state_info();
			MEMDUMP_NEW( tinst<domi_state_info>(), sizeof( domi_state_info));

			tinst<domi_state_info>()->init();

			// ���� ��ų
			tinst<DomiUserSkillMgr>() = new DomiUserSkillMgr();
			MEMDUMP_NEW( tinst<DomiUserSkillMgr>(), sizeof( DomiUserSkillMgr));
		}
	}
	static void destroy()
	{
		if( tinst<domi_state_info>() != nullptr)
		{
			I3_SAFE_DELETE(tinst<DomiUserSkillMgr>());

			delete tinst<domi_state_info>();
			tinst<domi_state_info>() = nullptr;
		}
	}

	void init();
	void init_wave();
	void init_shop_item();

	//buffer ��.
	bool is_wave_end;


	//sp ui
	DOMI_SP sp;



	//flow ui
	REAL32	play_time;
	INT16	round;
	INT8	max_wave;		//max ���̺�
	INT8	wave_count;		//current ���̺�

	REAL32	wave_pre_counter;
	REAL32	intermission_time;

	UINT32	total_score[4];
	INT16	max_dino;
	INT16	dino_count;




	//intermission ui
	bool    inIntermission;
	bool	intermission_ready[4];





	//revival
	UINT32	single_respawn_count;
	UINT32	everyone_respawn_count;




	//boss hp ui
	i3::vector<DOMI_BOSS_HP> boss_list;




	//award ui
	UINT32	award_item;			// ������ ������ID
	UINT32	award_item_arg;		// �Ⱓ �Ǵ� ����
	UINT16	award_point;		// point




	//item acid ui
	REAL32	etc_time;			// �ֽõ� ������ ���� ���� �ð�.
	INT8	etc_count;			// �ֽõ� ������ ���� ���� ��.
	
	// mutanttrex howling time
	INT32  boss_howling_time;
	
	INT32 boos_dash_target_type;
	INT32  boss_dash_target_idx;
	
	REAL32 boss_howl_cancel_threshold;
	REAL32 boss_cur_hp_at_howl_start;

	
	//grade valiable
	DOMI_REWARD_GRADE_STATE grade_state;
	i3::rc_wstring grade_user_nick;
};

//namespace domi_util
//{
//	void buy_domi_item(CASHITEM_GROUP_TYPE group_type); //�������� �����Ѵ�.
//	void buy_domi_cash_item(CASHITEM_GROUP_TYPE group_type); //�������� �����Ѵ�.
//	INT32 get_cash_item_count(CASHITEM_GROUP_TYPE group_type); //���� ��� 0 ����
//
//	bool is_buyable_domi_item(CASHITEM_GROUP_TYPE buy_group_type); //���� ��ǰ ��Ͽ� ��� ���ο� ���� ���� ��������..
//	bool is_buyable_domi_cash_item(CASHITEM_GROUP_TYPE buy_group_type); //���� ��ǰ ��Ͽ� ��� ���ο� ���� ���� ��������..
//}
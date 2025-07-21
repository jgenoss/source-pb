#pragma once

#include "../../../../CommonSource/CommonDediCli.h"

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


//�������� ���� �Ŀ��� ���� ���� ����.
//���� �̼��� ������ ��� ����.
template <> struct is_tinst_manual<class domi_state_info> : true_type {};
class domi_state_info
{
public:
	domi_state_info() {}
	~domi_state_info() {}

	static void create()
	{ 
		if( tinst<domi_state_info>() == NULL) 
			tinst<domi_state_info>() = new domi_state_info();

		if( tinst<domi_state_info>() != NULL) 
			tinst<domi_state_info>()->init();
	}
	static void destroy()
	{
		if( tinst<domi_state_info>() != NULL)
		{
			delete tinst<domi_state_info>();
			tinst<domi_state_info>() = NULL;
		}
	}

	void init();
	void init_wave();

	// ����� ����

	ST_DOMI_PROTOCOL_FIRST_VALUE_ACK first_value;
	ST_DOMI_PROTOCOL_USER_SP_ACK sp;

	INT32	create_count;
	INT32	respawn_count;
	INT32	death_count;
	REAL32	wave_pre_counter;
	REAL32	play_time;
	REAL32	intermission_time;
	INT16	max_dino;
	INT16	dino_count;
	INT16	single_respawn_count;
	INT16	everyone_respawn_count;
	INT16	round;			//����
	UINT32	total_score;
	INT8	max_wave;		//max ���̺�
	INT8	wave_count;		//current ���̺�
	INT8	wave_type;		// ���͹̼�
	INT8	user_skill_lv[8][USER_SKILL_COUNT];
	bool    inIntermission;
	bool	intermission_ready[4];

	REAL32	boss_max_hp;


	UINT32	award_item;			// ������ ������ID
	UINT32	award_item_arg;		// �Ⱓ �Ǵ� ����


	REAL32	etc_time;			// �ֽõ� ������ ���� ���� �ð�.
	INT8	etc_count;			// �ֽõ� ������ ���� ���� ��.



	UINT32	debug_total_score;
	

	UINT16	debug_combo_scroe;
	UINT16	debug_sub_score;
	UINT16	debug_spe_score;

	UINT16  ui_combo_sum;
	UINT16	ui_combo_add;
	UINT16	ui_spe_score;
};

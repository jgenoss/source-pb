#pragma once

enum QUEST_STATE
{
	QST_NONE = -1,
	QST_NOT_STARTED = 0,
	QST_DOING,
	QST_COMPLETED
};

namespace HUD
{
	enum ChallengeMessage;
	enum TargetShapeType;
	enum ItemState;
}
namespace DinoState
{
	enum Attack;
}
enum ResultWinState;

namespace ResultState
{
	struct UserInform;
	typedef i3::vector< UserInform > UserInformList;
}

namespace hu	//hud util
{
	/*	Stage�� ���� WeaponOnObject*�� �ٸ��� ������ �̸� ���Ѵ�.	 **/
	 void init_m179_object();

	/*	WeaponOnObject* ����ϴ� Stage���� �˷��ش�.	 **/
	bool is_m179_mode();

	/*	Ÿ���� �����ϴ� ��� ������ �˷��ش�. (Ÿ�� ���� : ������, �⸰, �︮����, ��ũ..)
	 *		is_default_shape�� true �̸� (--- : --- ���) , false�� (=== ���) �̴�.
	 **/
	bool is_target_mode(HUD::TargetShapeType & type, bool & is_default_shape);
	
	/*	pChara ������ ��� �ִ� ���⿡ ���� ��ź ���� �˷��ش�.	 *	*/
	bool get_bullet_cnt(CGameCharaBase* pChara, INT32 & bullet, INT32 & max_bullet);
	bool get_bullet_cnt(CGameCharaBase* pChara, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet);

	/*	pChara ������ ��� �ִ� ���⿡ ���� ��ź ���� �˷��ش�.	 *	*/
	bool get_bullet_cntF(CGameCharaBase* pChara, REAL32 & bullet, REAL32 & max_bullet);
	bool get_bullet_cntF(CGameCharaBase* pChara, REAL32 & bullet, REAL32 & max_bullet, REAL32 & max_load_bullet);

	/*	pChara ������ ��� �ִ� ���⿡ ���� Slot Type�� �˷��ش�.	 *	*/
	WEAPON_SLOT_TYPE get_weapon_slot_type(CGameCharaBase* pChara);

	/*	pChara ������ ������ Ÿ���� �˷��ش�	 *	*/
	bool get_weapon_bullet_type(CGameCharaBase* pChara);
	
	/*	WeaponM197 ���⿡ ���� ��ź ���� �˷��ش�.(gauge ���̴�)	 *	*/
	void get_m179_gauge(REAL32 & gauge, REAL32 & max_gauge);

	/*	target object�� ���� hp�� �˷��ش�.	**/
	void get_target_object_hp(INT32 & red_hp, INT32 & max_red_hp, INT32 & blue_hp, INT32 & max_blue_hp);

	/*	pChara�� ���� hp�� �˷��ش�.(dino �� ����)	**/
	void get_chara_hp(CGameCharaBase* pChara, INT32 & hp, INT32 & max_hp);
	void get_chara_hp(CGameCharaBase* pChara, REAL32 & per); //100% �� �˷��ش�.
	REAL32 get_chara_hp_ratio(CGameCharaBase* pChara);

	/*	WeaponOnObject* �� �̸��� �˷��ش�. **/
	i3::rc_wstring get_m179_name();
	
	/*	pChara�� ��� �ִ� ���� �̸��� �˷��ش�.(���� ��� "" ��ȯ) **/
	i3::rc_wstring get_weapon_name(CGameCharaBase* pChara);

	/*	���� ç���� ��� ���̵��� �˷��ش�. **/
	i3::rc_wstring get_challenge_difficulty_text(INT32 diff);
	INT32 get_challenge_difficulty_idx(INT32 diff);
	
	/*	���� ç���� ��� ������ �˷��ش�. **/
	INT32 get_challenge_point();

	/*	���� ĳ���Ϳ� ���� ų/����/��ý�Ʈ�� �˷��ش�. **/
	void get_kill_death_assist(INT32 slot_idx, INT32 & kill, INT32 & death, INT32 & assist);
	void get_kill_death_assist(INT32 & kill, INT32 & death, INT32 & assist);
	void get_KDA_to_string(i3::wstring & kill, i3::wstring & assist, i3::wstring & death);

	/*	HUD::ChallengeMessage �� ���� ç���� ��� ���ڸ� �˷��ش�. (���� ��� "" ��ȯ) **/
	i3::rc_wstring get_challenge_msg(HUD::ChallengeMessage idx);

	/*	idx �� ���� Ʃ�丮�� ��� ���ڸ� �˷��ش�. (���� ��� "" ��ȯ) **/
	i3::rc_wstring get_tutorial_msg(INT32 idx);

	/*	�ΰ��� ���� �ð��� �˷��ش�. **/
	void set_running_time(UINT32 time);
	void get_running_time(INT32 & minute, INT32 & second);

	/* GM ������ ���� �뽬������ �ð��� ������Ŵ **/
	void set_time_pause(bool paused);
	
	/*	�ΰ��� �� ������ �˷��ش�. **/
	void get_team_score(INT32 & red, INT32 & blue);

	/* mission Ȱ��ȭ ������ �˷��ش�. **/
	bool is_enable_mission();

	/*	mission_idx�� ���� �̼� ����, ���¸� �˷��ش�.	*/
	QUEST_STATE get_mission_info(INT32 mission_idx, i3::wstring & text);
	
	/*	mission_idx�� ���� Ʃ�丮�� ����, ���¸� �˷��ش�.	*/
	QUEST_STATE get_tutorial_info(INT32 mission_idx, i3::wstring & text);

	/*	���� stage�� ���� ��ǥ ������ �˷��ش�. (is_round�� true �̸� round ����)	*/
	INT32 get_round_goal_count(bool & is_round);
	
	/* �ڱⰡ �������� ���մϴ� */
	CHARACTER_TEAM_TYPE getMyTeam();

	/*	pPlayer�� ���� dino, human ��ũ�� �˷��ش�.	*/
	TEAM_TYPE get_team_mark(CGameCharaBase* pPlayer, INT32 & red, INT32 & blue);

	/*	Ż�� �̼ǿ� block�� �˱� ���� �ð��� �����Ѵ�.	*/
	void update_escape_block_time(REAL32 time);

	/*	Ż�� �̼ǿ� ���� block �ð� ���¸� �˷��ش�.	*/
	INT32 get_escape_block_time_shape_idx();
	
	/*	3D ��ǥ(vIn) -> 2D ��ǥ(fOutX, fOutY)	*/
	bool sceneToScreen(REAL32 * fOutX, REAL32 * fOutY, VEC3D * vIn);

	/*	pChara ��ġ�� ���� target object�� ���� ��ǥ, ũ�⸦ �˷��ش�.	*/
	bool get_target_object_position(CGameCharaBase* pChara, VEC2D & pos, VEC2D & size);

	/*	�ΰ��� ���� ������ ���� ����Ʈ ���
	 *		std::pair<INT32, i3::rc_string> : netIdx, nick name
	 *		add_me : ���� ���� ����
	 *		return : ���� ��(0 �Ǵ� 1)
	 **/
	INT32 get_ingame_user_list(i3::vector< i3::pair<INT32, i3::rc_wstring> > & list, bool add_me); //�ΰ���

	/*	�Ӱ��ӿ� ���� ������ ���� ����Ʈ�� red, blue ������ ��Ȱ �Ѵ�.
	 *		src : get_ingame_user_list �Լ����� ���� user list �Ǵ� ������ �� �� user list
	 *		lhs : red user list
	 *		rhs : blue user list
	**/
	void partition_ingame_user_list(const i3::vector< i3::pair<INT32, i3::rc_wstring> > & src,
		i3::vector< i3::pair<INT32, i3::rc_wstring> > & lhs,
		i3::vector< i3::pair<INT32, i3::rc_wstring> > & rhs);

	/*	�ֹ��� ù ���� ų�� ���Ǵ� helper fuctions..
	 **/
	void enable_check_primary_weapon_first_shot(); //������ �Ŀ� ȣ��ȴ�.
	void diable_check_primary_weapon_first_shot(); //���⸦ ������ ȣ��ȴ�.
	bool is_primary_weapon_first_shot(CGameCharaBase* pChara);

	/*	���� ĳ���Ϳ� �׼� ���¸� ��ȯ�Ѵ�.
	 **/
	CHARA_ACTION_BODYLOWER get_chara_action_state(CGameCharaBase* pChara);

	/*	���� ĳ���Ϳ� ����, C5 ���� ������ ��ȯ�Ѵ�.
	 **/
	HUD::ItemState get_helmet_protect_state(CGameCharaBase* pChara);
	HUD::ItemState get_c5_state(CGameCharaBase* pChara);
	
	/*	������ ���� ������ ��� ���ο� ���� String�� ��ȯ �Ѵ�.
	 *		return : ���� ������ ��뿡 ���� String �Ǵ� ��� �� �� ��� "" ��ȯ
	 **/
	i3::rc_wstring get_text_using_respawn_item();
}

namespace hc //hud control
{
	void btn_set_shape(i3UIButtonImageSet* p, INT32 idx); //-1�� ��Ȱ��ȭ��.
}

namespace hu_dino 
{
	//hud�� ǥ���� ������ ��� �մ�.
	struct attribute
	{
		attribute() : ratio(1.0f) {}

		DinoState::Attack idx1, idx2;
		REAL32 ratio;
		i3::rc_wstring	wstr_name;
	};
	extern attribute att;

	void reset_dino_attribute();
	void set_dino_attribute(const i3::rc_wstring& name, DinoState::Attack idx1, DinoState::Attack idx2) ;
	void set_dino_raio(REAL32 ratio);
	i3::rc_wstring get_dino_attck_string(DinoState::Attack idx);
	INT32 get_dino_shape_index();
}

namespace hu
{
	void sort_ingame_result_inform(i3::vector< ResultState::UserInform > & list); //ų, ����, point, exp, nick name ����

	template <class UnaryPredicate>
	inline void sort_ingame_result_inform(i3::vector< ResultState::UserInform > & list, UnaryPredicate pred, bool use_stable_sort) 
	{
		if(use_stable_sort == true)
			std::stable_sort(list.begin(), list.end(), pred); 
		else
			std::sort(list.begin(), list.end(), pred);
	}

	void get_ingame_result_inform( ResultState::UserInformList & red_list, ResultState::UserInformList & blue_list);
	bool is_my_team_win();

	//������ ������ �˱� ���ؼ��� bitmask���� benefit_mark���� �м��ؾ� �ȴ�.
	INT32 get_benefit_mark(INT32 benefit_mark, INT32 benefit_mask);
}

namespace hu
{
	bool is_zero_durability_weapon( T_ItemID ItemID,T_ItemDBIdx ware_idx); //�������� 0���� �ƴ��� �Ǵ�.
}

//***************************//
// Hud Unit Test
namespace hu_ut
{
#if ENABLE_UNIT_TEST
	extern struct _test_factor
	{
		CWeaponInfo* pInfo;
		STAGE_ID stage_id;
		CHARA_ACTION_BODYLOWER chara_action_body_lower;
		HUD::ItemState headgear_parts_state, c5_state;
		REAL32 escape_block_time;
		INT32 bullet;//, max_bullet, max_load_bullet;
		INT32 hp, kill, death, challenge_total_score;
		INT32 remain_battle_time;
		INT32 red_score, blue_score;
		INT32 red_object_hp, blue_object_hp;
		INT32 round_goal_count;
		UINT8 room_sub_type;
		bool chara_dino_type, changing_hud;
	} test_factor;

	void change_test_factor();
#endif

	//#if defined(NC_BUILD) �� ����Ѵ�.
	STAGE_ID	get_stage_id();
	STAGE_MODE	get_stage_type();
	UINT8 get_my_room_sub_type();
	bool is_test_changing_hud();
	bool is_kill_mode();
	bool is_stage_mask_training();
	bool is_stage_mask_madness();
	bool is_stage_mask_diehard();
	bool is_domination_mode();
	bool is_dino_escape_mode();
	bool is_cross_count_mode();
	bool is_annihilation_mode();
	bool is_tutorial_mode();
	bool is_destroy_mission_mode();
	bool is_defence_mission_mode();
	bool is_bomb_mission_mode();
	bool is_dino(CGameCharaBase* pMyChara);
}
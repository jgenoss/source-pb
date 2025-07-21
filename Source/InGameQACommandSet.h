#ifndef _INGAMEQACOMMANDSET_H__
#define _INGAMEQACOMMANDSET_H__

#undef ENUM_LIST
#define ENUM_LIST(enum_value) \
	enum_value(none) \
	enum_value(mission_win) \
	enum_value(time_limit) \
	enum_value(suicide) \
	enum_value(pos) \
	enum_value(move_pos) \
	enum_value(collision) \
	enum_value(cash) \
	enum_value(notice) \
	enum_value(change_grade) \
	enum_value(set_kill) \
	enum_value(respawn_time) \
	enum_value(item_period) \
	enum_value(mastery_point) \
	enum_value(set_hp) \
	enum_value(bullet_limit) \
	enum_value(grenade_limit) \
	enum_value(respawn) \
	enum_value(object) \
	enum_value(camera) \
	enum_value(fly) \
	enum_value(ui_toggle) \
	enum_value(exp) \
	enum_value(pino) \
	enum_value(challenge) \
	enum_value(clan_rank) \
	enum_value(clan_exp) \
	enum_value(log) \
	enum_value(move_offset) \
	enum_value(acceleration_offset) \
	enum_value(jump_height_offset) \
	enum_value(jump_damage_offset) \
	enum_value(clan_match_point) \
	enum_value(clan_match_win_lose) \
	enum_value(clan_match_room_create) \
	enum_value(medal_count) \
	enum_value(death_all) \
	enum_value(change_character) \
	enum_value(change_designation) \
	enum_value(kill_text) \
	enum_value(view_bone) \
	enum_value(invincible) \
	enum_value(skill) \
	enum_value(move_minimap) \
	enum_value(teamband) \
	enum_value(domination) \
	enum_value(killcount) \
	enum_value(ts) \
	enum_value(solo) \
	enum_value(tp) \
	enum_value(obdmg) \
	enum_value(hackoff) \
	enum_value(timergmpause) \
	enum_value(wc) \
	enum_value(max)


//-----------------------------------------------------------------------------------------------------------------------
//command ����.  'CommandSet::type �˻�'

// CommandSet::mission_win				--> �̼��� ��ī��� ���� Ŀ�ǵ�		
// CommandSet::time_limit				--> ���� ���� �ð� ������ Ŀ�ǵ�	
// CommandSet::suicide					--> ĳ���� �ڻ� Ŀ�ǵ� 
// CommandSet::pos						--> ĳ���� ��ġ ��ǥ Ŀ�ǵ�
// CommandSet::move_pos					--> ĳ���� �ڷ���Ʈ Ŀ�ǵ�
// CommandSet::collision				--> �� Texture, �浹üũ ǥ�� Ŀ�ǵ�
// CommandSet::cash						--> ����ĳ���� ����
// CommandSet::notice					--> �������� ��� Ŀ�ǵ�
// CommandSet::change_grade				--> ������� Ŀ�ǵ�
// CommandSet::set_kill					--> ���� ų �� ���� Ŀ�ǵ�
// CommandSet::respawn_time				--> ������ Ÿ�� ���� Ŀ�ǵ�
// CommandSet::item_period				--> ���� ������ �Ⱓ ���� Ŀ�ǵ�
// CommandSet::mastery_point			--> Mastery Point���� Ŀ�ǵ�
// CommandSet::set_hp					--> HP��ġ ���� Ŀ�ǵ�
// CommandSet::bullet_limit				--> ���� źâ ��� Ŀ�ǵ�
// CommandSet::grenade_limit			--> ��ô���� �� ��Ÿ ���� ��� Ŀ�ǵ�
// CommandSet::respawn					--> ĳ���� ������ Ŀ�ǵ�
// CommandSet::object					--> �� ������Ʈ ������ Ŀ�ǵ�
// CommandSet::camera					--> ���� ī�޶� ��� Ŀ�ǵ�( 1/3��Ī ���θ�..)
// CommandSet::fly						--> fly camera�� ��ȯ
// CommandSet::ui_toggle				--> 2D UI�� Enable/Disable
// CommandSet::exp						--> ����ġ ����
// CommandSet::pino						--> �ǳ� ���� 
// CommandSet::challenge				--> �ӹ� 0 ~ 9 ������ ���ڸ� ������ ����
// CommandSet::clan_rank				--> Ŭ�� ��ũ �ø���
// CommandSet::clan_exp					--> Ŭ�� ����ġ �ø���
// CommandSet::log						--> �α�(PBLog) ���Ϸ� ����
// CommandSet::move_offset				--> �̵��ӵ� �� ȣĪ ���� �Ӽ� ����
// CommandSet::acceleration_offset		--> �̵��ӵ� �� ȣĪ ���� �Ӽ� ����
// CommandSet::jump_height_offset		--> �̵��ӵ� �� ȣĪ ���� �Ӽ� ����
// CommandSet::jump_damage_offset		--> �̵��ӵ� �� ȣĪ ���� �Ӽ� ����

// CommandSet::clan_match_point			--> Ŭ�� ����Ʈ ����
// CommandSet::clan_match_win_lose		--> Ŭ�� ��, �� ����
// CommandSet::clan_match_room_create	--> Ŭ�� ��ġ �� ���� �����
// CommandSet::medal_count				--> ����, ����, ���� ������Ʈ

// CommandSet::death_all				--> ��� ĳ���� ����
// CommandSet::change_character			--> ĳ���� ����
// CommandSet::change_designation		--> ȣĪ ���� (arg1 : ����, arg2 : ���� ȣĪ, arg3 : On/Off)
// CommandSet::kill_text				--> Kill Text�� test�Ѵ�.
// CommandSet::view_bone				--> ĳ���� Bone�� ���ϴ�.
// CommandSet::invincible				--> NOT DAMAGED

// CommandSet::skill					--> ��ų �����ϱ�(arg1 : ��ų��ȣ, arg2 : ��ų��, arg2 : ��ų����)
// CommandSet::move_minimap				--> �̴ϸ� �̵� (for Domination)
// CommandSet::teamband					--> �� ��� �ý���
// CommandSet::domination				--> ���� �̼� Ŀ��� Ÿ��

//-----------------------------------------------------------------------------------------------------------------------



#undef DO_ENUM
#define DO_ENUM(value) value,

#undef DO_DESC
#define DO_DESC(value) L ## #value,

namespace CommandSet
{
	enum TYPE {ENUM_LIST(DO_ENUM)};
	static wchar_t* g_wstr_cmd[] = {ENUM_LIST(DO_DESC)};

	enum { VALUE_COUNT = 4};
	static REAL32 g_value[VALUE_COUNT];

	static CommandSet::TYPE _type;
	CommandSet::TYPE parsing( const i3::wliteral_range& wCmdRng);
};

namespace Domination_Util
{
	CommandSet::TYPE parsing( const i3::wliteral_range& wCmdRng);
	extern i3::wstring cmd_msg;
	extern bool enable_domi_nick;

	extern char g_domi_rank_web[MAX_RANKING_URL_SIZE];

	extern INT32 g_current_score;
	extern INT32 g_best_score;
	extern bool g_renewal_best;
};

class CGameCommandSet :	public i3ElementBase
{
	I3_CLASS_DEFINE( CGameCommandSet, i3ElementBase );

protected:
	char *			m_pszCommand = nullptr;

	REAL32			m_fValue[CommandSet::VALUE_COUNT] = { 0.0f };
	bool			m_bAlwaysShowPos = false;
	bool			m_bSkillView = false;

	i3TextNodeDX2 *		m_pSkillCmdText;		// [initialize at constructor]	

public:
	CGameCommandSet(void);
	~CGameCommandSet(void);

	void			ProcessCommand();
	void			SkillDebugView(void);

	bool			IsShowPos(void)	const				{ return m_bAlwaysShowPos;	}
	bool			ToogleShowPos(void)					{ return m_bAlwaysShowPos = !m_bAlwaysShowPos; }

	bool			isShowSkill(void) const				{ return m_bSkillView;		}
	void			setShowSkill(bool bEnable)			{ m_bSkillView = bEnable;	}
	i3TextNodeDX2*	GetSkillView(void) const			{ return m_pSkillCmdText;	}
};

#endif //_INGAMEQACOMMANDSET_H__
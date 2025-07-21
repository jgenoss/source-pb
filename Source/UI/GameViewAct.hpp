#include "pch.h"

#include "UIHudUtil.h"
#include "BattleHUD_Scope.h"
#include "GameCharaUpdateContext.h"
#include "GameCharaActionContext.h"
#include "Character/GameCharaUpdater.h"

/* ���� �ൿ State Manager */
namespace gwd
{
	//���� �ൿ State
	enum ActionType 
	{
		at_null,

		//sniper - scope�� �ִ� ��
		at_sniper_scope_show,
		at_sniper_scope_shot,
		at_sniper_scope_shot_after,

		//shotgun - ź���� ������ ����
		at_shotgun_shot,

		//�Ϲ����� ��
		at_normal_shot,

		//Į
		at_melee_shot,

		//����
		at_reload, 

		at_max,
	};
	enum WeaponType { wt_sniper_scope, wt_shotgun, wt_normal, wt_melee }; //���� �Ŀ� ���� ActionType �����ϱ� ����..
	enum TargetPointState { tps_sniper, tps_random, };
}

/* ���� �ൿ State Manager */
namespace gwd
{
	class ActStateManager;

	struct ActState
	{
	public:
		virtual void start() = 0;
		virtual void finish() = 0;

	public:
		ActStateManager* parent;
	};

	class ActStateManager
	{
	public:
		ActStateManager();
		~ActStateManager();

	public:
		void start_action(gwd::ActionType type);
		void update(REAL32 rDeltaSeconds);

	public:
		//ĳ���� �ൿ �Ϸ� ���� �Ǵ��ϴ� �Լ���..
		void set_time_update_fn() { pf_update = &ActStateManager::update_time_impl; } //�ð����� �Ǵ�
		void set_chara_state_update_fn() { pf_update = &ActStateManager::update_chara_state_impl; } //ĳ���� �ൿ���� �Ǵ�
		void set_time_chara_state_update_fn() { pf_update = &ActStateManager::update_time_and_chara_state_impl; } //�ð�, �ൿ���� �Ǵ�(�� �� �ϳ� �Ϸ�Ǹ� �Ϸ�)

	private:
		//ĳ���� �ൿ �Ϸ� ���� �Ǵ��ϴ� �Լ���..
		void update_time_impl(REAL32 rDeltaSeconds);
		void update_chara_state_impl(REAL32 rDeltaSeconds);
		void update_time_and_chara_state_impl(REAL32 rDeltaSeconds);
		void (ActStateManager::*pf_update)(REAL32 rDeltaSeconds);

	public:
		WeaponBase* m_pWeaponBase;
		Avatar* m_Avatar;
		//CHUD_Scope* m_pScope;
		i3::vector<gwd::ActState*> m_ActStateList;
		REAL32 m_ActionTime;

		gwd::ActionType m_CurrActionType;
		gwd::WeaponType m_WeaponType;
	};
}

/* ���� �ൿ State Manager */
namespace gwd
{
	//�ڵ尡 ������� �̷��� ǥ����.. �ǹ� ����..
	struct NullActState : ActState { virtual void start() {} virtual void finish() {} };

	struct Sniper_Scope_Show : ActState { virtual void start(); virtual void finish(); };
	struct Sniper_Scope_Shot : ActState { virtual void start(); virtual void finish(); };
	struct Sniper_Scope_Shot_After : ActState { virtual void start(); virtual void finish(); };
	struct ShotGun_Shot : ActState { virtual void start(); virtual void finish(); };
	struct Normal_Shot : ActState { virtual void start(); virtual void finish(); };
	struct Melee_Shot : ActState { virtual void start(); virtual void finish(); };

	struct Reload : ActState { virtual void start(); virtual void finish(); };
}

//---- gwd::ActStateManager -------------//
gwd::ActStateManager::ActStateManager()
{
	m_ActStateList.resize(at_max);

	m_ActStateList[gwd::at_null] = new NullActState;
	MEMDUMP_NEW( m_ActStateList[gwd::at_null], sizeof(NullActState) );

	m_ActStateList[gwd::at_null]->parent = this;

	m_ActStateList[gwd::at_sniper_scope_show] = new Sniper_Scope_Show;
	MEMDUMP_NEW( m_ActStateList[gwd::at_sniper_scope_show], sizeof(Sniper_Scope_Show) );
	m_ActStateList[gwd::at_sniper_scope_show]->parent = this;

	m_ActStateList[gwd::at_sniper_scope_shot] = new Sniper_Scope_Shot;
	MEMDUMP_NEW( m_ActStateList[gwd::at_sniper_scope_shot], sizeof(Sniper_Scope_Shot) );
	m_ActStateList[gwd::at_sniper_scope_shot]->parent = this;

	m_ActStateList[gwd::at_sniper_scope_shot_after] = new Sniper_Scope_Shot_After;
	MEMDUMP_NEW( m_ActStateList[gwd::at_sniper_scope_shot_after], sizeof(Sniper_Scope_Shot_After) );
	m_ActStateList[gwd::at_sniper_scope_shot_after]->parent = this;

	m_ActStateList[gwd::at_shotgun_shot] = new ShotGun_Shot;
	MEMDUMP_NEW( m_ActStateList[gwd::at_shotgun_shot], sizeof(ShotGun_Shot) );
	m_ActStateList[gwd::at_shotgun_shot]->parent = this;

	m_ActStateList[gwd::at_normal_shot] = new Normal_Shot;
	MEMDUMP_NEW( m_ActStateList[gwd::at_normal_shot], sizeof(Normal_Shot) );
	m_ActStateList[gwd::at_normal_shot]->parent = this;

	m_ActStateList[gwd::at_melee_shot] = new Melee_Shot;
	MEMDUMP_NEW( m_ActStateList[gwd::at_melee_shot], sizeof(Melee_Shot) );
	m_ActStateList[gwd::at_melee_shot]->parent = this;

	m_ActStateList[gwd::at_reload] = new Reload;
	MEMDUMP_NEW( m_ActStateList[gwd::at_reload], sizeof(Reload) );
	m_ActStateList[gwd::at_reload]->parent = this;

	m_ActionTime = 0.f;
	m_CurrActionType = gwd::at_null;

	pf_update = &ActStateManager::update_time_impl;
}
gwd::ActStateManager::~ActStateManager()
{
	struct fn { void operator()(gwd::ActState* p) const { I3_SAFE_DELETE( p); } };
	std::for_each(m_ActStateList.begin(), m_ActStateList.end(), fn() );
	m_ActStateList.clear();
}

void gwd::ActStateManager::start_action(gwd::ActionType type)
{
	m_CurrActionType = type;

	if (m_Avatar && m_Avatar->isCharaStateMask(CHARA_STATE_DEATH))
	{
		m_Avatar->removeCharaStateMask(CHARA_STATE_DEATH);
	}

	m_ActStateList[m_CurrActionType]->start();
}

void gwd::ActStateManager::update(REAL32 rDeltaSeconds)
{
	(this->*pf_update)(rDeltaSeconds);
}

//ĳ���� �ൿ �Ϸ� ���� �Ǵ��ϴ� �Լ�
void gwd::ActStateManager::update_time_impl(REAL32 rDeltaSeconds)
{
	m_ActionTime -= rDeltaSeconds;

	if(m_ActionTime < 0.f )
		m_ActStateList[m_CurrActionType]->finish();
}
void gwd::ActStateManager::update_chara_state_impl(REAL32 rDeltaSeconds)
{
	if( m_Avatar->getActionContext()->getUpper() == m_Avatar->getActionContext()->getUpperLeft() )
		m_ActStateList[m_CurrActionType]->finish();
}
void gwd::ActStateManager::update_time_and_chara_state_impl(REAL32 rDeltaSeconds)
{
	update_time_impl(rDeltaSeconds);
	update_chara_state_impl(rDeltaSeconds);
}

//---- gwd::ActState -------------//
namespace inner
{
	//�Ѿ� ���� ������ �˷��ش�.
	bool need_reload(CGameCharaBase* avatar, WeaponBase* weapon_base)
	{
		INT32 bullet, max_bullet;
		hu::get_bullet_cnt(avatar, bullet, max_bullet);

		if( max_bullet == 0 && bullet == 1 )
		{
			weapon_base->setTotalBulletCount( weapon_base->getWeaponInfo()->GetMaxBullet() ); //bulllet�� ���� ä���.
			return false;
		}

		if( bullet == 0 ) return true;

		return false;
	}
}

namespace inner
{
	//ź�� ��ġ�� ���ϴ� �Լ�
	void get_sniper_target_point(VEC3D & pos)
	{
		pos.x = -3.37f; 
		pos.y = 1.61f;
		pos.z = -14.70f;
	}
	void get_random_target_point(VEC3D & pos)
	{
		pos.x = -3.37f;  pos.y = 1.03f; pos.z = -14.70f;
		pos.x += (i3Math::Randf2() / 2.f); pos.y += (i3Math::Randf2() / 2.f);
	}
}

namespace inner
{
	//ź�� ǥ���ϴ� �Լ�
	void add_terrain_effect(WeaponBase* weapon_base, gwd::TargetPointState tps)
	{
		VEC3D pos, normal, dir;

		//�̰� �����ص� �����ϴ�.
		normal.x = 0.f; normal.y = 0.f; normal.z = -0.1f;
		dir.x = 0.f; dir.y = -0.05f; dir.z = 0.99f;

		//ź�� ��ġ ����
		switch( tps )
		{
		case gwd::tps_sniper: get_sniper_target_point(pos); break;
		case gwd::tps_random: get_random_target_point(pos); break;
		}

		I3ASSERT( weapon_base != nullptr);
		WEAPON_CLASS_TYPE wct = weapon_base->getWeaponInfo()->GetTypeClass();
		BULLET_EFFECT_TYPE bullettype = static_cast<BULLET_EFFECT_TYPE>(weapon_base->getWeaponInfo()->GetLoadBulletType());
		g_pEffectManager->AddTerrainEffect(I3_TERRAIN_PAPER, wct, bullettype, &pos, &normal, &dir );
	}

	void shotgun_add_terrain_effect(WeaponBase* weapon_base)
	{
		enum { ShotGun_mAX_Terrain = 8 };

		for(INT32 i =0; i<ShotGun_mAX_Terrain; ++i)
		{
			add_terrain_effect(weapon_base, gwd::tps_random);
		}
	}
}

/*------------------------------------*//*------------------------------------*/
//at_sniper_scope_show
void gwd::Sniper_Scope_Show::start()
{
	// ���� ���⼭ Scope�� Dual Mag�� �������� ���� ������ �ٲٴ� ���� ���� �� ����.
	// Sniper Dual Mag�� ������ ExtensionType�� EXT_DUAL_MAG���� �ٲٴ� �ڵ尡 ��� ����.
	// �̰� �ٲٱ⿡�� ���� �۾��� �ʿ��� �� ���� ���� ���⸸ ����ó�� ��. Jinsik.kim(2015.04.01)
	if(i3::same_of<WeaponSniperDualMagazine*>(parent->m_pWeaponBase) == true)
	{
		if(parent->m_pWeaponBase->GetCurrentExtensionType() == WEAPON::EXT_DUAL_MAG)
			parent->m_pWeaponBase->SwapExtensionType();
	}
	parent->m_Avatar->Cmd_Extension( true);
	//parent->m_pScope->SetSniperScope( true, parent->m_pWeaponBase->getWeaponInfo()); 
	parent->m_ActionTime = 0.4f;
	parent->set_time_update_fn();
}
void gwd::Sniper_Scope_Show::finish()
{
	parent->start_action(gwd::at_sniper_scope_shot);
}
//at_sniper_scope_shot
void gwd::Sniper_Scope_Shot::start()
{
	parent->m_Avatar->Cmd_Attack();
	parent->m_ActionTime = parent->m_pWeaponBase->GetFireDelay() - 0.1f;
	parent->set_time_chara_state_update_fn();
	inner::add_terrain_effect(parent->m_pWeaponBase, gwd::tps_sniper);
}
void gwd::Sniper_Scope_Shot::finish()
{
	parent->m_Avatar->Cmd_Extension( false);
	//parent->m_pScope->SetSniperScope( false, parent->m_pWeaponBase->getWeaponInfo());
	parent->start_action(gwd::at_sniper_scope_shot_after);
}
//at_sniper_scope_shot_after
void gwd::Sniper_Scope_Shot_After::start()
{
	parent->m_ActionTime = 1.3f;
	parent->set_time_update_fn();
}
void gwd::Sniper_Scope_Shot_After::finish()
{
	if( inner::need_reload(parent->m_Avatar, parent->m_pWeaponBase) == true)
	{
		parent->m_WeaponType = gwd::wt_sniper_scope;
		parent->start_action(gwd::at_reload);
	}
	else
	{
		parent->start_action(gwd::at_sniper_scope_show);
	}
}

/*------------------------------------*//*------------------------------------*/
//at_normal_shot
void gwd::Normal_Shot::start()
{
	if( parent->m_pWeaponBase->isFireEnabled() )
	{
		parent->m_Avatar->Cmd_Attack();
		parent->m_ActionTime = parent->m_pWeaponBase->GetFireDelay();
		parent->set_time_update_fn();
		inner::add_terrain_effect(parent->m_pWeaponBase, gwd::tps_random);
	}
}
void gwd::Normal_Shot::finish()
{
	if( inner::need_reload(parent->m_Avatar, parent->m_pWeaponBase) == true)
	{
		parent->m_WeaponType = gwd::wt_normal;
		parent->start_action(gwd::at_reload);
	}
	else
	{
		parent->start_action(gwd::at_normal_shot);
	}
} 

/*------------------------------------*//*------------------------------------*/
//at_melee_shot
void gwd::Melee_Shot::start()
{
	parent->m_Avatar->Cmd_Attack();
	parent->m_ActionTime = parent->m_pWeaponBase->GetFireDelay() + 0.1f;
	parent->set_time_update_fn();
}
void gwd::Melee_Shot::finish()
{
	parent->start_action(gwd::at_melee_shot);
} 


/*------------------------------------*//*------------------------------------*/
//at_shotgun_shot
void gwd::ShotGun_Shot::start()
{
	parent->m_Avatar->Cmd_Attack();
	parent->m_ActionTime = parent->m_pWeaponBase->GetFireDelay() + 0.1f;
	parent->set_time_update_fn();
	inner::shotgun_add_terrain_effect(parent->m_pWeaponBase);
}
void gwd::ShotGun_Shot::finish()
{
	if( inner::need_reload(parent->m_Avatar, parent->m_pWeaponBase) == true)
	{
		parent->m_WeaponType = gwd::wt_shotgun;
		parent->start_action(gwd::at_reload);
	}
	else
	{
		parent->start_action(gwd::at_shotgun_shot);
	}
} 

/*------------------------------------*//*------------------------------------*/
//at_reload
void gwd::Reload::start()
{
	//parent->m_Avatar->Cmd_Reload();
	parent->set_chara_state_update_fn();
}
void gwd::Reload::finish()
{
	switch(parent->m_WeaponType)
	{
	case gwd::wt_sniper_scope: parent->start_action(gwd::at_sniper_scope_show); break;
	case gwd::wt_shotgun: parent->start_action(gwd::at_shotgun_shot); break;
	case gwd::wt_normal: parent->start_action(gwd::at_normal_shot); break;
	}
}
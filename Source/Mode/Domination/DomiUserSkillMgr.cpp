#include "pch.h"
#include "DomiUserSkillMgr.h"
#include "../../../Common/CommonSource/CommonDediCli.h"
#include "../../GameCharaMoveContext.h"
#include "../../GameCharaWeaponContext.h"
#include "../../UI/UIBattlePopupSkillShop.h"
#include "../GameDropItemManager.h"
#include "../../UI/HUD/UIHUD_Domination_Manager.h"
#include "BattleSlotContext.h"
#include "DomiSkillProperties.h"
#include "../../Weapon/WeaponItemID.h"

DomiUserSkillMgr::DomiUserSkillMgr()
{
	// ���� �� �ٷ� �����ϴ� �͵�(���� ���� ��� ��ų �߰��Ǿ�� ��)
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_HP_UP, &DomiUserSkillMgr::HpUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_DAMAGE_UP, &DomiUserSkillMgr::DamageUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_SPEED_UP, &DomiUserSkillMgr::SpeedUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_MAGAZINE_UP, &DomiUserSkillMgr::MagazineUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_WEAPON_EFFECT_RATIO_UP, &DomiUserSkillMgr::WeaponSpecialEffectUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_RELOAD_SPEED_FAST, &DomiUserSkillMgr::MagazineReloadSpeedUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_GRENADE_DAMAGE_UP, &DomiUserSkillMgr::GrenadeDamageUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_AGGRO_DOWN, &DomiUserSkillMgr::AggroDown));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_GET_SP_UP, &DomiUserSkillMgr::SpGainUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_MAX_SP_UP, &DomiUserSkillMgr::SpGaugeUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_SPEND_SP_DOWN, &DomiUserSkillMgr::SpCostDown));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_GET_DAMAGE_DOWN, &DomiUserSkillMgr::HitDamageByDinoDown));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_MEATBOMB, &DomiUserSkillMgr::EquipMeatBomb));

	// ������ �� ������ ����(�����Ͱ� ���µǾ� �ٽ� �����ؾ��� �ʿ䰡 �ִ� �͵�)
	m_mapApplyAtRespawn.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_HP_UP, &DomiUserSkillMgr::HpUp));
	m_mapApplyAtRespawn.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_SPEED_UP, &DomiUserSkillMgr::SpeedUp));
	m_mapApplyAtRespawn.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_MAGAZINE_UP, &DomiUserSkillMgr::MagazineUp));
	m_mapApplyAtRespawn.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_MAX_SP_UP, &DomiUserSkillMgr::SpGaugeUp));
}

DomiUserSkillMgr::~DomiUserSkillMgr() 
{
	m_mapApplyDirect.clear();
	m_mapApplyAtRespawn.clear();
}

void DomiUserSkillMgr::ApplyDirect(INT32 customerIdx, USER_SKILL type, INT32 lv)
{
	i3::unordered_map<INT32, fn>::iterator it = m_mapApplyDirect.find(type);
	if (it != m_mapApplyDirect.end())
	{
		fn& mf = it->second;
		m_lvRecoder.SetLevel(customerIdx, static_cast<USER_SKILL>(type), lv);
		(this->*mf)(g_pCharaManager->getCharaByNetIdx(customerIdx), lv);
	}
}

void DomiUserSkillMgr::ApplyAtRespawn(INT32 customerIdx)
{
	for (INT32 i=0; i<USER_SKILL_COUNT; i++)
	{
		USER_SKILL skill = static_cast<USER_SKILL>(i);

		INT32 currentLevel = m_lvRecoder.GetLevel(customerIdx, skill);
		if (currentLevel <= 0) continue;

		i3::unordered_map<INT32, fn>::iterator it = m_mapApplyAtRespawn.find(skill);
		if (it != m_mapApplyAtRespawn.end())
		{
			fn& mf = it->second;
			(this->*mf)(g_pCharaManager->getCharaByNetIdx(customerIdx), currentLevel);
		}
	}
}

void DomiUserSkillMgr::SetLevel(INT32 customerIdx, USER_SKILL type, INT32 lv)
{
	m_lvRecoder.SetLevel(customerIdx, type, lv);
}

INT32 DomiUserSkillMgr::GetLevel(INT32 customerIdx, USER_SKILL type) const 
{ 
	return m_lvRecoder.GetLevel(customerIdx, type); 
}

// �Ʒ� ���� ��ų�� �ڼ��� ������ �Ʒ� �������� Ȯ��
// http://tiki.zepetto.biz/tiki-download_wiki_attachment.php?attId=3083
// ���� ���� �Լ��� �������� ó��
void DomiUserSkillMgr::HpUp(CGameCharaBase* customer, INT32 lv)
{
	customer->setCharaInfo()->SetHP(static_cast<REAL32>(customer->getFullHP()));
	BattleSlotContext::i()->setGameInfo_Chara( customer->getCharaNetIndex())->_tNetData._tHP.setHP( static_cast<UINT16>(customer->getFullHP()) );
}

void DomiUserSkillMgr::DamageUp(CGameCharaBase* customer, INT32 lv) {}

void DomiUserSkillMgr::SpeedUp(CGameCharaBase* customer, INT32 lv)
{
	REAL32 addupStat = domi::SkillProp::i()->GetUserSkill(USER_SKILL_SPEED_UP).GetIncrRatio(lv);

	customer->getMoveContext()->EnableSpeedReduction(addupStat, -1.0f);
}

void DomiUserSkillMgr::MagazineUp(CGameCharaBase* customer, INT32 lv)
{
	WeaponBase* primary = customer->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY);
	WeaponBase* secondary = customer->getWeaponContext()->getWeapon(WEAPON_SLOT_SECONDARY);
	
	WeaponBase* wp[2] = {primary, secondary};
	REAL32 addupStat = domi::SkillProp::i()->GetUserSkill(USER_SKILL_MAGAZINE_UP).GetIncrRatio(lv);

	for (INT32 i=0; i<2; i++)
	{
		// ��������, ���� ���׷��̵� ������ �ȵ� ���ɼ��� �ֽ��ϴ�.
		// ���� �����ؾ��մϴ� .. 2013.01.19 ���õ
		if(wp[i] == nullptr) continue;

		wp[i]->setLoadedBulletCount(wp[i]->getWeaponInfo()->GetLoadBullet());
		INT32 mazineCapacity = wp[i]->GetLoadBulletWithCashItem(); // 1 �� źâ �ִ� ź��
		INT32 maxBullet = static_cast<INT32>(wp[i]->GetMaxBulletWithCashItem() * addupStat); // ��ų ����� �ִ� ź��
		wp[i]->setTotalBulletCount(maxBullet - mazineCapacity);
	}

	// �ѱ� ��ü ���׼� �� ����Ʈ
	CGameCharaWeaponContext* weaponCtx = customer->getWeaponContext();
	WEAPON_SLOT_TYPE curWeaponSlot = weaponCtx->getCurrentSlotNum();
	if (WEAPON_SLOT_PRIMARY == curWeaponSlot)
	{	
		WEAPON_SLOT_TYPE trickSlot = static_cast<WEAPON_SLOT_TYPE>(curWeaponSlot+1);
		weaponCtx->setCurrentSlotNum( trickSlot );
		weaponCtx->setSwapSlotNum( trickSlot );
	}
	customer->Cmd_ChangeWeapon(WEAPON_SLOT_PRIMARY, false);

	CGameDropItem::effect(customer, DINOKILLITEM_AMMO);
}

void DomiUserSkillMgr::WeaponSpecialEffectUp(CGameCharaBase* customer, INT32 lv) {}
void DomiUserSkillMgr::MagazineReloadSpeedUp(CGameCharaBase* customer, INT32 lv) 
{
	// �Ʒ� �Լ����� �ڵ� ����
	// REAL32 WeaponBase::GetReloadTime(), REAL32 WeaponBase::GetLoadBulletTime()
}
void DomiUserSkillMgr::GrenadeDamageUp(CGameCharaBase* customer, INT32 lv) {}
void DomiUserSkillMgr::AggroDown(CGameCharaBase* customer, INT32 lv) {}
void DomiUserSkillMgr::SpGainUp(CGameCharaBase* customer, INT32 lv) {}

void DomiUserSkillMgr::SpGaugeUp(CGameCharaBase* customer, INT32 lv)
{
	REAL32 ratio = domi::SkillProp::i()->GetUserSkill(USER_SKILL_MAX_SP_UP).GetIncrRatio(lv);
	
	CUIHudDomi_Skill* hud = static_cast<CUIHudDomi_Skill*>(UIHUDDominationMgr::i()->GetHUD(HUD_DOMI_SKILL));
	hud->SetSPMax(static_cast<INT16>(domi::SkillProp::i()->GetSP(domi_state_info::i()->round).max->GetVal() * ratio));
}

void DomiUserSkillMgr::SpCostDown(CGameCharaBase* customer, INT32 lv) 
{
	REAL32 ratio = domi::SkillProp::i()->GetUserSkill(USER_SKILL_SPEND_SP_DOWN).GetIncrRatio(lv);

	// ���ö��� HP / AMMO
	domi::SkillProp::i()->GetSupplycost().hpsupply_sp->SetFactor(ratio);
	domi::SkillProp::i()->GetSupplycost().ammosupply_sp->SetFactor(ratio);

	// ��Ʈ����
	for (INT32 i=1; i<=MAX_LEVEL_SENTRYGUN; i++)
	{
		domi::SkillProp::i()->GetSentrygun(i).install_sp->SetFactor(ratio);
		domi::SkillProp::i()->GetSentrygun(i).repair_sp->SetFactor(ratio);
	}

	// ����
	for (INT32 i=1; i<=MAX_LEVEL_DUMMY; i++)
	{
		domi::SkillProp::i()->GetDummy(i).install_sp->SetFactor(ratio);
		domi::SkillProp::i()->GetDummy(i).repair_sp->SetFactor(ratio);
	}

	// ������ų
	for (INT32 type=0; type<USER_SKILL_COUNT; type++)
	{
		INT8 maxLv = domi::SkillProp::i()->GetUserSkill(type).level_max->GetVal();
		for (INT32 i=1; i<=maxLv; i++)
			domi::SkillProp::i()->GetUserSkill(type).GetCostSp(i).SetFactor(ratio);
	}
}

void DomiUserSkillMgr::HitDamageByDinoDown(CGameCharaBase* customer, INT32 lv) {}

void DomiUserSkillMgr::EquipMeatBomb(CGameCharaBase* customer, INT32 lv) 
{
	// ���� �������� �ٷ� ����/����.
	// ���� �� �̻�� ���¿��� ������ �� SetCharaWeaponSet()���� �ڵ������ϹǷ� 
	// m_mapApplyAtRespawn�� ����ϸ� �ȵ�.
	T_ItemID ItemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, WEAPON_CLASS_THROWING_GRENADE, WEAPON_CLASS_THROWING_GRENADE, WEAPON::getItemIndex(WEAPON::DOMI_MEATBOMB));
	customer->Cmd_CreateWeapon(WEAPON_SLOT_MISSION, ItemID);
	customer->Cmd_ChangeWeapon(WEAPON_SLOT_MISSION, false);
}



UserSkillLvRecoder::UserSkillLvRecoder() {}
UserSkillLvRecoder::~UserSkillLvRecoder() {}

void UserSkillLvRecoder::CurrentLevels::SetValue(USER_SKILL type, INT32 lv)
{
	I3ASSERT(0 <= type && type < USER_SKILL_COUNT);
	I3ASSERT(0 <= lv && lv < MAX_LEVEL_USERSKILL+1);

	levels[type] = lv;
};

INT32 UserSkillLvRecoder::CurrentLevels::GetValue(USER_SKILL type) const
{
	I3ASSERT(0 <= type && type < USER_SKILL_COUNT);

	return levels[type];
};

void  UserSkillLvRecoder::SetLevel(INT32 userIdx, USER_SKILL type, INT32 lv)
{
	INT32 idx = userIdx / 2; // (0, 2, 4, 6) -> (0, 1, 2, 3)
	I3ASSERT(0 <= idx && idx < domiUserMaxCnt);
	
	m_val[idx].SetValue(type, lv);
}

INT32 UserSkillLvRecoder::GetLevel(INT32 userIdx, USER_SKILL type) const
{
	INT32 idx = userIdx / 2; // (0, 2, 4, 6) -> (0, 1, 2, 3)
	I3ASSERT(0 <= idx && idx <= domiUserMaxCnt);

	return m_val[idx].GetValue(type);
}
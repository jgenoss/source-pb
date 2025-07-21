#include "pch.h"
#include "DominationUserSkillApply.h"
#include "../../CommonSource/CommonDediCli.h"
#include "../../GameCharaMoveContext.h"
#include "../../GameCharaWeaponContext.h"
#include "Domination_Defines.h"
#include "../../UI/UIBattlePopupSkillShop.h"
#include "../GameDropItemManager.h"

const REAL32 g_domiSkillLvIncrRatio[USER_SKILL_COUNT][MAX_LEVEL_USERSKILL+1] = 
{
	{1.0f, 1.2f, 1.4f, 1.6f, 1.8f, 2.0f},  // USER_SKILL_HP_UP
	{1.0f, 1.2f, 1.4f, 1.6f, 1.8f, 2.0f},  // USER_SKILL_DAMAGE_UP
	{1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f},  // USER_SKILL_SPEED_UP
	{1.0f, 1.2f, 1.4f, 1.6f, 1.8f, 2.0f}   // USER_SKILL_MAGAZINE_UP
};

UserSkillApply::UserSkillApply()
{
	m_map.insert(std::tr1::unordered_map<INT8, fn>::value_type(USER_SKILL_HP_UP, &UserSkillApply::HpUp));
	m_map.insert(std::tr1::unordered_map<INT8, fn>::value_type(USER_SKILL_DAMAGE_UP, &UserSkillApply::DamageUp));
	m_map.insert(std::tr1::unordered_map<INT8, fn>::value_type(USER_SKILL_SPEED_UP, &UserSkillApply::SpeedUp));
	m_map.insert(std::tr1::unordered_map<INT8, fn>::value_type(USER_SKILL_MAGAZINE_UP, &UserSkillApply::MagazineUp));
}

UserSkillApply::~UserSkillApply() 
{
	m_map.clear();
}

void UserSkillApply::ApplyDirect(INT8 customerIdx, INT8 skillType, INT8 lv)
{
	std::tr1::unordered_map<INT8, fn>::iterator it = m_map.find(skillType);
	if (it != m_map.end())
	{
		fn& mf = it->second;
		(this->*mf)(g_pCharaManager->getCharaByNetIdx(customerIdx), lv);
	}
}

void UserSkillApply::ApplyAtRespawn(INT8 customerIdx)
{
	for (INT32 i=0; i<USER_SKILL_COUNT; i++)
	{
		INT32 lv = tinst<domi_state_info>()->user_skill_lv[customerIdx][i];
		if (lv <= 0) continue;

		std::tr1::unordered_map<INT8, fn>::iterator it = m_map.find(static_cast<INT8>(i));
		if (it != m_map.end())
		{
			fn& mf = it->second;
			(this->*mf)(g_pCharaManager->getCharaByNetIdx(customerIdx), static_cast<INT8>(i));
		}
	}
}

void UserSkillApply::HpUp(CGameCharaBase* customer, INT8 lv)
{
	customer->getCharaInfo()->SetHP(static_cast<REAL32>(customer->getFullHP()));
	g_pGameContext->getGameInfo_Chara(
		customer->getCharaNetIndex())->_tNetData._tDefData.setHP(static_cast<UINT16>(customer->getFullHP()));
}

void UserSkillApply::DamageUp(CGameCharaBase* customer, INT8 lv)
{
	// ������ ����� �������� �ϹǷ� ���� ó���� �ʿ����.
}

void UserSkillApply::SpeedUp(CGameCharaBase* customer, INT8 lv)
{
	customer->getMoveContext()->EnableSpeedReduction(g_domiSkillLvIncrRatio[USER_SKILL_SPEED_UP][lv], -1.0f);
}

void UserSkillApply::MagazineUp(CGameCharaBase* customer, INT8 lv)
{
	WeaponBase* primary = customer->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY);
	WeaponBase* secondary = customer->getWeaponContext()->getWeapon(WEAPON_SLOT_SECONDARY);
	
	WeaponBase* wp[2] = {primary, secondary};

	for (INT32 i=0; i<2; i++)
	{
		// ��������, ���� ���׷��̵� ������ �ȵ� ���ɼ��� �ֽ��ϴ�.
		// ���� �����ؾ��մϴ� .. 2013.01.19 ���õ
		if(wp[i] == NULL) continue;

		wp[i]->setLoadedBulletCount(wp[i]->getWeaponInfo()->GetLoadBullet());

		REAL32 incrRatio = g_domiSkillLvIncrRatio[USER_SKILL_MAGAZINE_UP][lv];
		INT32 mazineCapacity = wp[i]->GetLoadBulletWithCashItem(); // 1 �� źâ �ִ� ź��
		INT32 maxBullet = static_cast<INT32>(wp[i]->GetMaxBulletWithCashItem()*incrRatio); // ��ų ����� �ִ� ź��
		wp[i]->setTotalBulletCount(maxBullet - mazineCapacity);
	}

	// �ѱ� ��ü ���׼� �� ����Ʈ
	CGameCharaWeaponContext* weaponCtx = customer->getWeaponContext();
	INT32 curWeaponSlot = weaponCtx->getCurrentSlotNum();
	if (WEAPON_SLOT_PRIMARY == curWeaponSlot)
	{	
		INT32 trickSlot = curWeaponSlot+1;
		weaponCtx->setCurrentSlotNum(trickSlot);
		weaponCtx->setSwapSlotNum(trickSlot);
	}
	customer->Cmd_ChangeWeapon(WEAPON_SLOT_PRIMARY, false);

	CGameDropItem::effect(customer, DINOKILLITEM_AMMO);
}
#include "pch.h"
#include "DominationSkillNotice.h"
#include "../../CommonSource/CommonDediCli.h"
#include "UI/HUD/UIHUD_Domination_Manager.h"
#include "DominationSkillSystem.h"
#include "../../UI/UIBattleFrame.h"
#include "DominationUserSkillApply.h"

using namespace dss;

SkillNotice::SkillNotice()
{
	m_map.insert(std::tr1::unordered_map<UINT32, fn>::value_type(STATE_INSTALLABLE, &SkillNotice::Setup));
	m_map.insert(std::tr1::unordered_map<UINT32, fn>::value_type(STATE_UPGRADABLE, &SkillNotice::Upgrade));
	m_map.insert(std::tr1::unordered_map<UINT32, fn>::value_type(STATE_REPAIRABLE, &SkillNotice::Fix));
	m_map.insert(std::tr1::unordered_map<UINT32, fn>::value_type(STATE_HP_RECOVER, &SkillNotice::UseSupplyBaseHP));
	m_map.insert(std::tr1::unordered_map<UINT32, fn>::value_type(STATE_AMMO_RECOVER, &SkillNotice::UseSupplyBaseAMMO));
	m_map.insert(std::tr1::unordered_map<UINT32, fn>::value_type(STATE_SHOP_SALABLE, &SkillNotice::OpenSkillShop));

	m_noticeHUD = static_cast<CUIHudDomi_Skill*>(UIHUDDominationMgr::i()->GetHUD(HUD_DOMI_SKILL));
}

SkillNotice::~SkillNotice() 
{ 
	m_map.clear(); 
	m_noticeHUD = 0;
}

void SkillNotice::Show(CGameCharaBase* customer, CServiceable* service) 
{ 
	if (customer && service)
	{
		std::tr1::unordered_map<UINT32, fn>::iterator it = m_map.find(service->GetState());
		if (it != m_map.end())
		{
			fn& mf = it->second;
			(this->*mf)(customer, service);
		}
	}
}

void SkillNotice::Hide() { m_noticeHUD->HideNotice(); }

void SkillNotice::Setup(CGameCharaBase* customer, CServiceable* service) // setup�� ������ ��Ȳ, E Ű�� �䱸 SP�� ���
{
	if (::strcmp(service->GetName(), "RAW_OBJECT") != 0) return;

	INT16 cost = 0;
	const ST_DOMI_PROTOCOL_FIRST_VALUE_ACK* values = &tinst<domi_state_info>()->first_value;
	switch (service->GetType())
	{
	case DOMI_TYPE_DUMMY: cost = values->_i16SpendSP_DummyInstall[0]; break;
	case DOMI_TYPE_SENTRYGUN: cost = values->_i16SpendSP_SentryGunInstall[0]; break;
	default: return;
	}

	INT16 sp = tinst<domi_state_info>()->sp._i16SP[customer->getCharaNetIndex()/2];
	if (sp >= cost)
	{
		m_noticeHUD->AllowTheUse("SETUP", cost); // ���� ��ġ�� ������ ����1��.
		service->EnableCharaIdx(customer->getCharaNetIndex());
	}
	else
	{
		m_noticeHUD->WarningNoUse("SP NOT ENOUGH");
		service->DisableCharaIdx(customer->getCharaNetIndex());
	}
}

void SkillNotice::Upgrade(CGameCharaBase* customer, CServiceable* service)
{
	if (::strcmp(service->GetName(), "RAW_OBJECT") == 0) return;

	INT16 cost = 0;
	const ST_DOMI_PROTOCOL_FIRST_VALUE_ACK* values = &tinst<domi_state_info>()->first_value;
	INT32 nextLV = service->GetWeapon()->getWeaponInfo()->GetLevel()+1;
	switch (service->GetType())
	{
	case DOMI_TYPE_DUMMY: cost = values->_i16SpendSP_DummyInstall[nextLV]; break;
	case DOMI_TYPE_SENTRYGUN: cost = values->_i16SpendSP_SentryGunInstall[nextLV]; break;
	default: return;
	}

	INT16 sp = tinst<domi_state_info>()->sp._i16SP[customer->getCharaNetIndex()/2];
	if (sp >= cost)
	{
		m_noticeHUD->AllowTheUse("UPGRADE", cost);
		service->EnableCharaIdx(customer->getCharaNetIndex());
	}
	else
	{
		m_noticeHUD->WarningNoUse("SP NOT ENOUGH");
		service->DisableCharaIdx(customer->getCharaNetIndex());
	}
}

void SkillNotice::Fix(CGameCharaBase* customer, CServiceable* service) 
{
	if (::strcmp(service->GetName(), "RAW_OBJECT") == 0) return;

	INT16 cost = 0;
	const ST_DOMI_PROTOCOL_FIRST_VALUE_ACK* values = &(tinst<domi_state_info>()->first_value);
	INT32 level = service->GetWeapon()->getWeaponInfo()->GetLevel();
	switch (service->GetType())
	{
	case DOMI_TYPE_DUMMY: cost = values->_i16SpendSP_DummyFix[level]; break;
	case DOMI_TYPE_SENTRYGUN: cost = values->_i16SpendSP_SentryGunFix[level]; break;
	default: return;
	}

	INT16 sp = tinst<domi_state_info>()->sp._i16SP[customer->getCharaNetIndex()/2];
	if (sp >= cost)
	{
		m_noticeHUD->AllowTheUse("FIX", cost);
		service->EnableCharaIdx(customer->getCharaNetIndex());
	}
	else
	{
		m_noticeHUD->WarningNoUse("SP NOT ENOUGH");
		service->DisableCharaIdx(customer->getCharaNetIndex());
	}
}

void SkillNotice::UseSupplyBaseHP(CGameCharaBase* customer, CServiceable* service)
{
	if (::strcmp(service->GetName(), "RAW_OBJECT") == 0) return;

	if (customer->getCurHP() == customer->getFullHP())
	{
		m_noticeHUD->WarningNoUse("HP IS FULL");
		service->DisableCharaIdx(customer->getCharaNetIndex());
		return;
	}

	INT16 cost = tinst<domi_state_info>()->first_value._i16SpendSP_HPSupply;
	INT16 sp = tinst<domi_state_info>()->sp._i16SP[customer->getCharaNetIndex()/2];
	if (sp >= cost)
	{
		m_noticeHUD->AllowTheUse("HP", cost);
		service->EnableCharaIdx(customer->getCharaNetIndex());
	}
	else
	{
		m_noticeHUD->WarningNoUse("SP NOT ENOUGH");
		service->DisableCharaIdx(customer->getCharaNetIndex());
	}
}

void SkillNotice::UseSupplyBaseAMMO(CGameCharaBase* customer, CServiceable* service)
{
	if (::strcmp(service->GetName(), "RAW_OBJECT") == 0) return;

	WeaponBase* primary = customer->GetCharaWeapon(WEAPON_SLOT_PRIMARY);
	WeaponBase* secondary = customer->GetCharaWeapon(WEAPON_SLOT_SECONDARY);

	if (primary == NULL && secondary == NULL)
	{
		m_noticeHUD->WarningNoUse("THERE'S NO WEAPON!");
		service->DisableCharaIdx(customer->getCharaNetIndex());
		return;
	}

	INT32 lv = tinst<domi_state_info>()->user_skill_lv[customer->getCharaNetIndex()][USER_SKILL_MAGAZINE_UP];
	REAL32 incrRatio = g_domiSkillLvIncrRatio[USER_SKILL_MAGAZINE_UP][lv];

	WeaponBase* wp[2] = {primary, secondary};
	for (INT32 i=0; i<2; i++)
	{
		if (wp[i] == NULL) continue;

		INT32 max = static_cast<INT32>(wp[i]->GetMaxBulletWithCashItem()*incrRatio); // �ִ� ���� ���� ź��
		INT32 loaded = wp[i]->GetLoadBulletWithCashItem(); // 1źâ �ִ� �Ѿ� ����
		INT32 reserve = wp[i]->getTotalBulletCount(); // ���� ź��

		if (max > loaded+reserve) break;
			
		m_noticeHUD->WarningNoUse("NO USE");
		service->DisableCharaIdx(customer->getCharaNetIndex());
		return;
	}

	INT16 cost = tinst<domi_state_info>()->first_value._i16SpendSP_AmmoSupply;
	INT16 sp = tinst<domi_state_info>()->sp._i16SP[customer->getCharaNetIndex()/2];
	if (sp >= cost)
	{
		m_noticeHUD->AllowTheUse("AMMO", cost);
		service->EnableCharaIdx(customer->getCharaNetIndex());
	}
	else
	{
		m_noticeHUD->WarningNoUse("SP NOT ENOUGH");
		service->DisableCharaIdx(customer->getCharaNetIndex());
	}
}

void SkillNotice::OpenSkillShop(CGameCharaBase* customer, CServiceable* service)
{
	if (tinst<domi_state_info>()->inIntermission == false) return;
	if (::strcmp(service->GetName(), "RAW_OBJECT") == 0) return; // �̷����� ��������...
	if (UIBattleFrame::i()->IsOpenPopup(UBP_SKILL_SHOP)) 
	{
		Hide(); return;
	}

	m_noticeHUD->AllowTheUse("OPEN");
	service->EnableCharaIdx(customer->getCharaNetIndex());
}
#include "pch.h"
#include "WeaponDualSwordTH.h"

I3_CLASS_INSTANCE(WeaponDualSwordTH);

WeaponDualSwordTH::WeaponDualSwordTH() : m_aiiAttackBRight3PV(0)
{
}

WeaponDualSwordTH::~WeaponDualSwordTH()
{
}

void WeaponDualSwordTH::OnBindResource()
{
	WeaponDualKnife::OnBindResource();

	m_aiiAttackBRight3PV = static_cast<INT8>(m_pAICtx->FindAIStateByName("AttackB_Right_3PV"));
}

void WeaponDualSwordTH::OnAttackBRight(i3AIContext* pCtx, REAL32 tm)
{
	if (m_pOwner->is3PV())
	{
		_PlayWeaponAnimWithApplyTimeScale(m_aiiAttackBRight3PV);	// 3��Ī�� �´� �ִϸ��̼Ǹ� ȣ��.
		return;
	}

	WeaponDualKnife::OnFire(pCtx, tm);
}
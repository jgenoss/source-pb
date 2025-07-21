#include "pch.h"
#include "MainWeapon_TouchBomb.h"
#include "BattleObjContext.h"

MainWeapon_TouchBomb::MainWeapon_TouchBomb(WeaponBase * p) : MainWeapon_Grenade(p), explosion(false)
{

}

void MainWeapon_TouchBomb::OnInitVariable()
{
	MainWeapon_Grenade::OnInitVariable();

	explosion = false;
}

void MainWeapon_TouchBomb::Reset()
{
	MainWeapon_Grenade::Reset();

	explosion = false;
}

void MainWeapon_TouchBomb::OnUpdate_Grenade(REAL32 rDeltaSeconds)
{
	if (isThrowing())
	{	// ���忡 ������ ����ź�Դϴ�.
		// ������ �ִ��� üũ�ؼ� ������ ��ȯ�����ݴϴ�.
		if (_CheckProcessUserLeave() == false)
			return;

		m_nStateFlag = NET_GRENADE_STATE_FLY;

		bool	bHost = m_pWeaponBase->IsDedicatedHost();

		m_rElapsedExplosionTime += rDeltaSeconds;

		REAL32 rExplosionRange = m_pWeaponBase->GetExplosionTime();

		this->UpdateFlying(rDeltaSeconds, rExplosionRange);

		m_pWeaponBase->_TransformWorldSpace(rDeltaSeconds);

		i3Vector::Copy(&m_ThrowData.m_vOldPos, m_pWeaponBase->GetPos());

		if (m_pWeaponBase->getNetworkIdx() != -1)
		{
			if (m_ThrowData.m_rElapsedThrowTime > NET_GRENADE_OFFTIME)
			{// ��Ʈ���� �޴� ��� 15�ʱ��� ��Ŷ�� �� ������ �׳� �����ݴϴ�.
				GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon(m_pWeaponBase->getNetworkIdx());
				pInfo->Reset();

				m_pWeaponBase->_ResetGrenadeInfo();
				g_pWeaponManager->PushThrowWeapon(m_pWeaponBase);
			}

			if (bHost)
			{
				if (explosion)
				{
					m_nStateFlag = NET_GRENADE_STATE_EXPLOSION;
				}

				// ��Ʈ��ũ�� �����ݴϴ�.
				I3ASSERT(m_nStateFlag < 256);
				GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), &m_ThrowData.m_vOldPos, (UINT8)m_nStateFlag);

				if (SuccessiveStateFlagCheck(m_nStateFlag))
				{
					GAMEEVENT::Write_ThrowingWeapon(m_pWeaponBase->getNetworkIdx(), &throwInfo);
				}
			}

			if (explosion)
				this->Explosion();
		}
	}
}

void MainWeapon_TouchBomb::CheckWorldCollision(REAL32 rDeltaSeconds)
{
	VEC3D	vTemp;
	REAL32	rLength;
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult;

	VEC3D	vNew;
	i3Vector::Scale(&vNew, &m_ThrowData.m_vDirection, m_ThrowData.m_rSpeed * m_ThrowData.m_rElapsedThrowTime);
	i3Vector::Add(m_pWeaponBase->GetPos(), &m_ThrowData.m_vStartPos, &vNew);
	addY(m_pWeaponBase->GetPos(), -5.0f * m_ThrowData.m_rElapsedThrowTime  * m_ThrowData.m_rElapsedThrowTime);
	i3Matrix::PreRotateX(m_pWeaponBase->GetMatrix(), -I3_DEG2RAD(rDeltaSeconds * 360.f));

	ColLine.SetStart(&m_ThrowData.m_vOldPos);
	i3Vector::Sub(&vTemp, m_pWeaponBase->GetPos(), &m_ThrowData.m_vOldPos);
	rLength = i3Vector::Normalize(&vTemp, &vTemp);

	if (rLength <= 0.0f)
	{
		_ProcessBoundObjectForce();
		return;
	}

	ColLine.SetEnd(m_pWeaponBase->GetPos());
	ColLine.SetDir(&vTemp);

	// ù��° raycast�� Chara���� Rough Collision�� �˻��մϴ�.
	pResult = g_pFramework->raycastClosest(&ColLine, CGRP_MASKGROUP_GRENADE);
	if (pResult != nullptr)
	{
		i3EventReceiver * pRecv = pResult->m_pDestShapeSet->getEventReceiver();

		g_pFramework->reserveRaycastResult(1);

		if (i3::kind_of<CGameCharaBase*>(pRecv))
		{
			UINT32 nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
			CGameCharaBase * pTarget = (CGameCharaBase *)pRecv;

			if (pTarget == m_pWeaponBase->getOwner()) //m_pOwner )
			{
				pResult = nullptr;
			}
			else
			{
				if (pTarget->getCharaTeam() & CHARACTER_TEAM_RED)
				{
					nColGroup = CGRP_MASKGROUP_CHARA_RED;
				}

				pResult = g_pFramework->raycastClosest(&ColLine, nColGroup);
				if (pResult != nullptr)
				{
					pRecv = pResult->m_pDestShapeSet->getEventReceiver();
					if (i3::kind_of<CGameCharaBase*>(pRecv))
					{
						pTarget = (CGameCharaBase *)pRecv;
						if (pTarget->isCharaStateMask(CHARA_STATE_DEATH))
							pResult = nullptr;
					}
				}
			}
		}
	}

	if (pResult == nullptr)
	{
		// Rough Hit Collision�� Chara Hit����� ������ World�� Object�� ���Ͽ� �׽�Ʈ�� �մϴ�.
		pResult = g_pFramework->raycastClosest(&ColLine, CGRP_MASKGROUP_ATTACK);
		if (pResult == nullptr)
			return;

		g_pFramework->reserveRaycastResult(1);
	}

	{
		VEC3D	vecNew, vecNewPos, vecNormal;

		//�̵� ���� ���� �� �̵��Ÿ�
		i3Vector::Sub(&vecNew, &pResult->m_Intersect, &m_ThrowData.m_vOldPos);
		REAL32 rTestLen = i3Vector::Normalize(&vecNew, &vecNew);

		// �����̴� ������Ʈ��� ���� ����ؼ� �ݻ緮�� ��������մϴ�. by dfly79 080117
		m_rBoundObjectLen = _BoundObject(pResult, ColLine.GetDir());

		if (rLength + 0.05f < rTestLen + m_rBoundObjectLen) //rTestLen * rTestLen)
		{
			//��ȿ�� �Ÿ����� �浹�� ����..
			_ProcessBoundObjectForce();
			return;
		}
		
		VEC3D vecRevert;
		i3Vector::Scale(&vecRevert, &vecNew, -0.1f);
		i3Vector::Add(m_pWeaponBase->GetPos(), &pResult->m_Intersect, &vecRevert);

		// Explosion
		explosion = true;
	}

	g_pFramework->resetRaycastResult();
}
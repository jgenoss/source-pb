#include "pch.h"
#include "GameCharaHUDControl.h"
#include "GlobalVariables.h"
#include "GameCharaHUDContext.h"
#include "Camera/GameCamera_OnObject.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIBattlePopupBase.h"
#include "CrosshairMgr.h"
#include "BattleHud.h"
#include "UserInfoContext.h"
#include "UI/UIUtil.h"
#include "MyRoomInfoContext.h"

I3_CLASS_INSTANCE( CGameCharaHUDControl);

namespace
{
	bool Is1PVObserver(const CGameCharaBase* pOwner)
	{
		if (!pOwner) return false;

		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
			return false;

		return pOwner->isCharaStateMask(CHARA_STATE_OBSERVER) && pOwner->is1PV() ? true : false;
	}
}



/*virtual*/ void CGameCharaHUDControl::OnChangeWeapon( void)
{
}

/*virtual*/ void CGameCharaHUDControl::OnDamage( VEC3D * pDir /*= nullptr*/, REAL32 rHitTheta /*= 0.f*/, bool bDirectionView /*= false*/, INT32 filter_index /*=-1*/ )
{
	if( CHud::i() == nullptr)
		return;

	if( bDirectionView && pDir != nullptr)
	{
		REAL32 Rotation = atan2f( i3Vector::GetX( pDir ), i3Vector::GetZ( pDir ) ); // XZ ���
		REAL32 rTargetAngle = i3Math::NormalizeAngle( Rotation );
		REAL32 fAngleGap = i3Math::NormalizeAngle( rTargetAngle - rHitTheta );
	
		CHud::i()->SetDamageDirection( fAngleGap);
	}

	// ���� ������ ����Ʈ ����� ���� ����
	//if( m_pOwner->IsDino() || m_pOwner->IsHelmProtection() == false)
	//{
		if( UserInfoContext::i()->IsGM_Observer() == false)
		{
			CHud::i()->OnDamageFilter(filter_index);
		}
	//}
}

/*virtual*/ bool CGameCharaHUDControl::OnApplyCrosshair( void)
{
 	if( m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
	{
		if (g_pCamera == NULL)						return false;
		if (g_pCamera->getCurTarget() != m_pOwner)	return false;
		if (!g_pFramework->IsBattleStage())			return false;

		CROSSHAIR_MODE type = CH_MODE_NORMAL;

		CStageBattle * pStage = (CStageBattle*)g_pFramework->GetCurrentStage();

	if( pStage == nullptr)
		return false;

	if (CrosshairMgr::i()->getCrosshair() == nullptr) return false;
	if (CHud::i() == nullptr) return false;

	bool bEnableCross = true;			// CH�� ��Ÿ���� ������ ����
	WeaponBase * pWeapon = nullptr;
	WEAPON_CLASS_TYPE weaponClass = WEAPON_CLASS_UNKNOWN;

	if( m_pOwner->IsDino() == false)
		pWeapon = getUsedWeapon();
	

		CHud::i()->SetEnableGrenadeLauncherCrossHair(false);

		if (pWeapon != NULL)
		{
			CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
			I3ASSERT(pWeaponInfo != NULL);

			weaponClass = pWeaponInfo->GetTypeClass();

			//	Crosshair
			if (!m_pOwner->isCharaStateMask(CHARA_STATE_DEATH))
			{
				if (i3::kind_of<WeaponDotLaser*>(pWeapon))
				{
					BOOL bEnable = TRUE;

					if (!Is1PVObserver(m_pOwner))
						bEnable = ((WeaponDotLaser*)pWeapon)->getEnableDotLaser();

					if (bEnable || pWeapon->isWeaponStateMask(WSM_ZOOM) ||
						pWeapon->isPossibleTurnOffWithDotLaser() == FALSE ||
						(pWeapon->GetCurrentExtensionType() == WEAPON::EXT_ZOOM && m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM)))
					{
						bEnableCross = FALSE;
						CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
					}
					else // ��Ʈ �����͸� ���� ''?
					{
						CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
					}
				}
				//	������ ����Ʈ �޸� ��ź �߻���� ���
				else if (i3::kind_of<WeaponGrenadeLauncherWithLaser*>(pWeapon))
				{
					bEnableCross = (pWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER);

					CHud::i()->SetEnableGrenadeLauncherCrossHair(bEnableCross);

					bEnableCross = FALSE;
				}
				// �ͽ��ټ� ���� ��ź �߻� ������ ���
				else if (i3::kind_of<WeaponHandGrenade*>(pWeapon))
				{
					CHud::i()->SetEnableGrenadeLauncherCrossHair(TRUE);
					bEnableCross = FALSE;

				}
				//	������ ����Ʈ �޸� ��ź �߻���� ���
				else if (i3::kind_of<WeaponShotGunLauncherWithLaser*>(pWeapon))
				{
					CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);

					bEnableCross = FALSE;
				}
				// 2014-06-18 Ż���� ������ ����� ��� // ENG_��뿵
				else if (i3::kind_of<WeaponWatermelon*>(pWeapon))
				{
					bEnableCross = FALSE;
				}
				else
				{
					if ((weaponClass == WEAPON_CLASS_SNIPER) ||
						(weaponClass == WEAPON_SUBCLASS_SNIPER))
					{	// �ͽ��ټǾ��� ������
						bEnableCross = m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM);

						if (bEnableCross)
							type = CH_MODE_SCOPE;						// Scope
					}

					switch (pWeapon->GetCurrentExtensionType())
					{
					case WEAPON::EXT_SCOPE:
					{
						bEnableCross = TRUE;
						if (m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM))
							type = CH_MODE_SCOPE;						// Scope
						CHud::i()->SetEnableGrenadeLauncherCrossHair( false);
					}
					break;

				case WEAPON::EXT_ZOOM :
				case WEAPON::EXT_DOT_SIGHT :
					bEnableCross = ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM);
					CHud::i()->SetEnableGrenadeLauncherCrossHair( false);
					break;

					case WEAPON::EXT_GLAUNCHER:
						bEnableCross = !pWeapon->isExtensionActivate();
						CHud::i()->SetEnableGrenadeLauncherCrossHair(!bEnableCross);
						break;

					default:
						CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
						break;
					}

					if (pWeaponInfo->GetTypeClass() == WEAPON_CLASS_ROCKET_LAUNCHER)
					{	// RPG7
						bEnableCross = FALSE;
						type = CH_MODE_NORMAL;
						CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
					}
				}
			}
			else
			{
				bEnableCross = FALSE;	//	ĳ���Ͱ� ���� ���
				CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
			}

			if (pWeaponInfo->GetCrossHairInvisible() & 0x00000002)
			{
				// �� �Ŀ� ������ Scope CH�� ������ٰ� �ϴ���, ���� ScopeCH�� ON�̶�� ����
				// Scope ����̱� ������ �Ϲ� CH�� ��Ÿ���� �ʾƾ� �Ѵ�.
				if (type == CH_MODE_SCOPE)
					bEnableCross = false;

				type = CH_MODE_NORMAL;
			}

			if (pWeaponInfo->GetCrossHairInvisible() & 0x00000001)
			{
				// Normal Crosshair
				if (type == CH_MODE_NORMAL)
					bEnableCross = FALSE;
			}

			if (((weaponClass == WEAPON_CLASS_SNIPER) ||
				(weaponClass == WEAPON_SUBCLASS_SNIPER)) &&
				(pWeaponInfo->GetCrossHairExpand() == 0.0f && !m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM)))
			{
				bEnableCross = FALSE;
			}
		}

		// ���� �Ǵ� ��⿡ ž�½� Crosshair�� �׻� Disable ��Ų��.
		if (i3::same_of<CGameCamera_Observer*>(g_pCamera)
			|| i3::same_of<CGameCamera_Observer2*>(g_pCamera)
			|| i3::same_of<CGameCamera_FLY*>(g_pCamera)
			|| (i3::kind_of<CGameCamera_OnObject*>(g_pCamera)))
		{
			// ���� �ÿ��� Crosshair�� �׻� Disable ��Ų��.
			bEnableCross = FALSE;
		}

		if (m_pOwner->isCharaStateMask(CHARA_STATE_DEATH))
			bEnableCross = FALSE;

		UIBattlePopupBase * p = GameUI::GetBattlePopup(UBP_ACCUSE_CRIME);
		if (p != NULL && p->isEnable() == TRUE)
			bEnableCross = FALSE;
		// Dragonov�� SSG69�� ���� ������ �Ǵ� ������ ��쿣 Scope_CH�� ��Ÿ���� �ʴ´�.
		if ((type == CH_MODE_SCOPE) && (pWeapon->getScopeCrosshairEnable() == FALSE))
			type = CH_MODE_NORMAL;

		//�������� �ش� ������ ũ�ν��� ����ؾ��Ѵ�!
		if (Is1PVObserver(m_pOwner) && bEnableCross)
		{
			CrosshairMgr::i()->ApplyTargetCrosshair(m_pOwner);
		}
		else
		{
			CrosshairMgr::i()->setCrosshairType(type);
			CrosshairMgr::i()->enableCrossHair(bEnableCross ? true : false);
			CrosshairMgr::i()->setTargetChara(NULL);
		}

		if (MyRoomInfoContext::i()->getStage()->IsHudOff())
		{
			CrosshairMgr::i()->enableCrossHair(false);// != FALSE);
			if (weaponClass == WEAPON_CLASS_HANDGUN) CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
		}

		return true;
	}

	return false;
}


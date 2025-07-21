#include "pch.h"
#include "GameCharaBase.h"
#include "GameCharaContexts.h"
#include "Character/GameDinoContexts.h"
#include "GameEvent.h"
#include "StageBattle.h"
#include "TutorialMode.h"
#include "GameMode.h"
#include "BattleHud.h"
#include "GameStateMgr.h"

#include "UI/UIHudUtil.h"
#include "UI/UIHudManager.h"
#include "Mode/Domination/DominationSkillSystem.h"

#include "./StageBattle/UserContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "GameMissionManager.h"
#include "Weapon/WeaponItemID.h"

#include "MainWeapon_DualCIC.h"

#include "ObserverBattle.h"
#include "i3Scene/i3XrayScene.h"
#include "StageBattle/AIModeContext.h"

bool CGameCharaBase::Cmd_ApplyCrosshair( void)
{
	if (getHUDContext())
		getHUDContext()->_ApplyCrosshair();
	else
		TRACE("CGameCharaBase::Cmd_ApplyCrosshair getHUDContext() = nullptr\n");

	return true;
}

bool CGameCharaBase::Cmd_Attack( WEAPON::FIRE_ORDER fireOrder,
								VEC3D * pPos /*= nullptr*/, VEC3D * pTarget /*= nullptr*/,
								bool bOnlyAction /*= false*/, INT32 iSlot /*= WEAPON_SLOT_PRIMARY*/)
{
	if(MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) && this->isAIToClient() )
		g_pSndMng->StartCharacterActionSnd( GTBCA_MADNESS_ATTACK, (UINT32)this->getDBInfo()->GetCharaSexTypeDISPLAY(), this->GetPos(), this->is1PV());

	// �űԳ����� �ý��� - �������� 0�̸� ���� ����
	//if( isLocal() && UserInfoContext::i()->IsNewRepair())
	if( isLocal())
	{
		WeaponBase*		pWeapon			= GetCurrentCharaWeapon();

		if( pWeapon && pWeapon->IsEnduranceZero() )
		{
			CWeaponInfo*	pCurWeaponInfo	= pWeapon->getWeaponInfo();
			UINT64			keyStroke		= getInputContext()->getKeyStrokeInput();

			if( keyStroke & GAME_KEY_ACTION_ATTACK && pCurWeaponInfo->isGun() )
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand()%2, GetPos(), true);
			}
			return false;
		}
	}


	if( bOnlyAction)
	{
		// KEYWORD : WEAPON_KNIFE	20080909. dfly79
		// �������� ���� ���带 ���� ȣ������� �մϴ�.
		// �Ϲ� �ѱ�� �߻� �ִϸ��̼ǿ��� ȣ���� ������ ���������� �ø����� �ϱ� ������ ���带 ������� �ʽ��ϴ�.
		if( IsDino() == false)
		{
			WeaponBase * pWeapon = GetCurrentCharaWeapon();
			if( pWeapon != nullptr)
			{
				pWeapon = pWeapon->getUsedWeapon();
				// KEYWORD : WEAPON_CIC
				if( i3::same_of<WeaponDualCIC*>(pWeapon))
				{
					MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(pWeapon->GetMainWeapon());
					pWeapon = mainWeapon_DualCIC->getLeftWeapon();
				}
				
				// ���� ó�� ����
				// ���� �ִϸ��̼��� �ִ� ��� ó���մϴ�.
				if( i3::same_of<WeaponShotGun3*, WeaponGunWithKnife*, WeaponBow*, WeaponKnife*, 
						WeaponDualKnife*, WeaponKnuckle*, WeaponShotGunWithButt*, WeaponShotGunMaleFemale* >(pWeapon)
					|| i3::kind_of<WeaponMiniAxe*, WeaponKnuckle*, WeaponKnife*, WeaponDualKnife*>(pWeapon) )
					// 2009.11.10
					// Network M1887 �ٰŸ� ���ݽ� ���� �ִϸ��̼��� �����ؾ� ��.
				{
					VEC3D vTargetPos;
					pWeapon->GetShotTargetPos( &vTargetPos);
					pWeapon->NET_Fire( pPos, &vTargetPos, fireOrder); 
				}

				if (i3::kind_of<WeaponGrenade*>(pWeapon))
				{
					getWeaponContext()->PushThrowWeaponIndex(getWeaponContext()->getCurrentSlotNum());

					if (i3::same_of<WeaponDualGrenade*>(pWeapon))
					{
						getWeaponContext()->PushThrowWeaponIndex(getWeaponContext()->getCurrentSlotNum());
					}
				}
			}
		}
		// ���۸� ���ϴ� ���⸸ �ش�˴ϴ�.
		getActionContext()->_Play_Upper_Fire( iSlot, fireOrder);
	}
	else
	{
		if( IsDino()) {
				// WEAPON_SECONDARY_FIRE : Sting => Poison(Grenade), CCSting => Spit(Melee) �̹Ƿ� ����ó��
				if (!IsCCSting() && !IsStingMars())
					getActionContext()->_Play_Upper_Fire( iSlot, WEAPON::SECONDARY_FIRE);
		}
		else
		{
			WeaponBase* pCurrWeapon = GetCurrentCharaWeapon();
			if (pCurrWeapon)
			{
				if (i3::same_of<WeaponBow*>(pCurrWeapon))
				{
					if (static_cast<WeaponBow*>(pCurrWeapon)->isExtensionActivate())
						fireOrder = WEAPON::SECONDARY_FIRE;
				}
			}

			switch( fireOrder)
			{
			case WEAPON::PRIMARY_FIRE :
				{
					WeaponBase * pCurWeapon  = GetCurrentCharaWeapon();
					if( pCurWeapon == nullptr)	return false;

					CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
					I3ASSERT( pCurWeaponInfo != nullptr);

					if( getWeaponContext()->_Fire( WEAPON::PRIMARY_FIRE, pPos, pTarget) )
					{
						//	��ź ��ġ�� ���
						if( pCurWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION)
						{
							if( isLocal())
								Cmd_Install();
							else
							{	// ��Ʈ��ũ�̸� �׼Ǹ� �����ݴϴ�.
								getActionContext()->_Play_Upper_Fire( iSlot, fireOrder);
							}
						}
						//	�Ϲ� ������ ���
						else
						{
							getActionContext()->_Play_Upper_Fire(iSlot, fireOrder);
						}

						return true;
					}
					else
					{
						if( pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER && pCurWeapon->isExtensionActivate() )
						{
							WeaponGrenadeLauncher * pGLauncher = (WeaponGrenadeLauncher*) pCurWeapon;
							if( pGLauncher->getTotalShellCount() > 0)
								Cmd_Reload();
						}
						else if( pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && pCurWeapon->isExtensionActivate() )
						{
							WeaponShotGunLauncher * pLauncher = (WeaponShotGunLauncher*) pCurWeapon;
							if( pLauncher->getTotalShellCount() > 0)
								Cmd_Reload();
						}
						if( pCurWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION && pCurWeaponInfo->GetTypeUsage() == WEAPON_SLOT_MISSION)
						{
							if( getActionContext()->isUpper( CHARA_UPPER_FIRE) )
								CGameMissionManager::i()->SendMissionInstall( false);
						}
						

						removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
					}
				}
				break;

			case WEAPON::SECONDARY_FIRE :
				if( getWeaponContext()->_Fire( WEAPON::SECONDARY_FIRE, pPos, pTarget))
				{
					getActionContext()->_Play_Upper_Fire( iSlot, WEAPON::SECONDARY_FIRE);
					return true;
				}
				break;

			case WEAPON::READY_FIRE :
				if( getWeaponContext()->_Fire( WEAPON::READY_FIRE) )
					return getActionContext()->_Play_Upper_Fire( iSlot, WEAPON::READY_FIRE);
				break;

			case WEAPON::HITED_FIRE :
				if( is1PV() )
				{
					getActionContext()->setUpper(CHARA_UPPER_FIRE);
					getAnimContext()->_Play_Anim_Upper( ID_UPPER_ATTACK2);
				}
				break;
			
			}
		}
	}

	return false;
}

void CGameCharaBase::Cmd_ChangeView( CHARA_VIEW_TYPE type, bool bPlayAnimSync /*= true*/)
{
	// ������ Ÿ�� ���⿡ �ִ� ���� ��ŵ�ؾ��մϴ�. OutPost�� M197 ���� ó�� 2009.06.09 dfly79
	if( IsOnObject() )	return;

	ChangeControl_View( type);

	if (getAnimContext())
		getAnimContext()->ChangeAnimSet( type, bPlayAnimSync);
	getSoundContext()->SetSoundOwner();

	//AI �ɸ��� �ΰ�� ����,.
	if( getCharaInfo()->GetAIToClient()) return;

	if( type == CHARA_VIEW_3PV )
	{
		// 3��Ī�� ��� ���̵��� ����
		if( UserContext::i()->IsPlayingSlot( getCharaNetIndex()))
		{
			SetEnable( true);

			Cmd_SetPhysics();
		}
	}
}

bool CGameCharaBase::Cmd_ChangeWeapon( WEAPON_SLOT_TYPE slot, bool bDirect /*= false*/)
{
	if( IsDino()) return false;
	
	//	Weapon Change
	if( getWeaponContext()->_ChangeWeapon( slot, bDirect) )
	{	
		if( !bDirect)
		{	// ���� �����ӿ� ����ȴٸ� ���翡 ĳ���� �ִϸ��̼��� �̸� �����մϴ�.
			if( isCharaStateMask( CHARA_STATE_ZOOM))
			{	//	���� ���¿����� �Ϲ� ī�޶�� ���ư��ϴ�
				getCameraContext()->_Idle_Camera();
			}

			// revision 36076 C4 ��ġ�� ���� ���� �� �� �߻� �����ϰ� C4 ��ġ ���尡 ��� play�Ǵ� ���� ����
			// C4 ��ġ ���� ��� ��µǴ� ���尡 ĳ���Ϳ� �پ��ִ�.
			// ���� ���� �ִϸ��̼��� �����ϰ� �� ���
			// AI Index �� �ι� �ٲ�� �Ǿ� ���������� ���ᰡ ���� �ʴ� ������ �߻��ؼ�
			// ������ ���� AI�� ���� �����ش�.
			getAnimContext()->FinishCurrentAIUpper();

			// ��ü �ִϸ��̼����� �����մϴ�.
			if (getActionContext())
				getActionContext()->_Play_Upper_ChangeWeapon();
			return true;
		}
	}

	return false;
}

void CGameCharaBase::Cmd_CreateEquip( CHARA_RES_ID charaResType, T_ItemID itemID, T_ItemID subitemID/* = 0*/)
{
	if( getEquipContext())
	{
		T_ItemID EquipItemID = itemID;
		EQUIP::eBERET res_subtype = EQUIP::eBERET_NONE;

		if( subitemID > 0 )
		{
			EquipItemID = subitemID;
			const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo(itemID);
			res_subtype = static_cast<EQUIP::eBERET>(pEquipInfo->GetCombinationType());
		}

		if( EquipItemID > 0)
		{
			// ������ ������ Resource�� �ε�����
			((Avatar*)this)->SetEquipResource( EquipItemID, res_subtype);

			getEquipContext()->CreateEquip( charaResType, EquipItemID, res_subtype);
		}
	}
}

void CGameCharaBase::Cmd_DeleteEquip( EQUIP::ePART part)
{
	if( getEquipContext() != nullptr)
	{
		getEquipContext()->ReleaseEquip( part);
	}
}

bool CGameCharaBase::Cmd_CreateWeapon( WEAPON_SLOT_TYPE slot, T_ItemID Itemid)
{
	if (!getWeaponContext())
		return false;

	bool rv = getWeaponContext()->_Create_Weapon( slot, Itemid);

	if( slot == 0)
	{
		// KEYWORD : AI_MOVE_TO_WEAPON
		//
		// �׻� 1���� ���⸦ ���� �ʿ䰡 �ֱ� ������....
		Cmd_ChangeWeapon( slot, true);
	}

	return rv;
}

bool CGameCharaBase::Cmd_CreateWeapon_Only( WEAPON_SLOT_TYPE slot, T_ItemID Itemid)
{
	return getWeaponContext()->_Create_Weapon( slot, Itemid);
}

bool CGameCharaBase::Cmd_Crouch( void)
{
	if (m_rCrouchParalysis > 0.f)
		return false;

	if( getActionContext()->isBody( CHARA_BODYLOWER_STAND ) )
	{
		if( !isCharaStateMask( CHARA_STATE_CHANGEPOSE))
		{	// �ڼ� �������� �ƴ� ��쿡�� ó��
			if( isEnableStateMask( CHARA_STATE_DISABLE_CROUCH) == false)
			{
			
				getActionContext()->_Play_Crouch();
				return true;
			}
		}
	}

	return false;
}

void CGameCharaBase::Cmd_Enable_TeamTag( bool bEnable)
{
	getMaterialContext()->SetTeamMarkEnable( bEnable);
}

void CGameCharaBase::Cmd_Enable_Buff( INT32 Attack, INT32 Speed)
{
	getMaterialContext()->SetTeamMarkEnable( true,Attack,Speed);
}


void CGameCharaBase::Cmd_Idle( void)
{
	Cmd_SyncWeapon();

	getActionContext()->_Play_Upper_Idle();
	getActionContext()->_Play_Lower_Idle();

	// ���⸦ �⺻ Idle ���·�
	WeaponBase * pWeapon = GetCurrentCharaWeapon();
	if( pWeapon != nullptr)
		pWeapon->ReturnToReady();
}

void CGameCharaBase::Cmd_Idle_Lower( void)
{
	if (IsDino())
	{
		getActionContext()->_Play_DinoLower_Idle();
	}
	else
	{
		getActionContext()->_Play_Lower_Idle();

	}
}

void CGameCharaBase::Cmd_Idle_Upper( void)
{
	if( IsDino())
	{
		if( getActionContext()->isUpper( CHARA_UPPER_IDLE) ||
			getActionContext()->isUpper( CHARA_UPPER_MOVE))
		{
			getActionContext()->_Play_Upper_Idle();
		}
	}
	else
	{
		// KEYWORD : WEAPON_DUALHANDGUN
		WeaponBase * pWeapon = GetCurrentCharaWeapon();

		if( pWeapon != nullptr)
			pWeapon = pWeapon->getUsedWeapon();

		if( pWeapon != nullptr && pWeapon->isDualGun() )
		{
			if( ( (getActionContext()->isUpper(CHARA_UPPER_MOVE) ) 
				|| (getActionContext()->isUpper(CHARA_UPPER_IDLE) ) )
				&& 
				( (getActionContext()->getUpperLeft() == CHARA_UPPER_MOVE) 
				|| (getActionContext()->getUpperLeft() == CHARA_UPPER_IDLE) )
				)
			{
				getActionContext()->_Play_Dual_Idle_Right();
				getActionContext()->_Play_Dual_Idle_Left();
			}
		}
		else
		{
			if( getActionContext()->isUpper( CHARA_UPPER_IDLE) ||
				getActionContext()->isUpper( CHARA_UPPER_MOVE))
			{
				getActionContext()->_Play_Upper_Idle();
			}
		}
	}
}

void CGameCharaBase::Cmd_Idle_Upper_Move( void)
{
	if( !IsDino())
	{
		// KEYWORD : WEAPON_DUALHANDGUN
		WeaponBase * pWeapon = GetCurrentCharaWeapon();

		if( pWeapon != nullptr )
			pWeapon = pWeapon->getUsedWeapon();

		if( pWeapon != nullptr && pWeapon->isDualGun() )
		{
			CGameCharaActionContext*	pActCtx	= getActionContext();

			if( pActCtx->getUpper() == CHARA_UPPER_MOVE )
			{
				pActCtx->_Play_Dual_Idle_Right();
			}

			if( pActCtx->getUpperLeft() == CHARA_UPPER_MOVE )
			{
				pActCtx->_Play_Dual_Idle_Left();
			}
		}
	}
}

void CGameCharaBase::Cmd_Idle_Camera( void)
{
	getCameraContext()->_Idle_Camera();
}

bool CGameCharaBase::Cmd_Install( bool bEnable /*= true*/)
{
#if defined( DEF_OBSERVER_MODE)
	// Observer�� ó������ �ʴ´�.
	if( BattleSlotContext::i()->isObserverMe())
		return false;
#endif

	if( !isLocal() )
		return false;

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	if( bEnable)
	{
		// Jump �������� Ȯ��
		// Jump���̸� ��ġ �Ұ�
		if( isCharaStateMask( CHARA_STATE_JUMP) || !isCharaStateMask( CHARA_STATE_FLOOR) )
			return false;

		if( pMissionMng->getMissionState() == GAME_MISSION_STATE_INBATTLE)
		{//	�̼ǿ� ��ź�� ��ġ�Ǿ� ���� �ʴٸ�
			//	�̵� �Ұ�. 
			i3Vector::Zero( getMoveContext()->getMoveDir());
			getMoveContext()->setAccel( 0.f, 0.f, 0.f);
			getMoveContext()->setVelocity( 0.f, 0.f, 0.f);

			getActionContext()->_Play_Lower_Idle();

			// ��ġ�� ������ �ɴ´�.
			getActionContext()->_Play_Crouch();

			// ��ź ��ġ!!
			getActionContext()->_Play_Upper_Fire();

			addEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
			addCharaStateMask( CHARA_STATE_ARMINGBOMB);

			pMissionMng->SendMissionInstall( true );
			return true;
		}
		else
		{
			pMissionMng->SendMissionInstall( false );
		}
	}
	else
	{
		INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();

		ISNETWORK
		{
			if(	(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// �������� �ƴѰ��
					return false;
		
			if( pMissionMng->IsInstalling( mySlotIdx ) || pMissionMng->isMyMission() )
			{	//	�ڽ��� ��ź ��ġ ���̸� ���
				pMissionMng->SendMissionInstall( false );	//	��ź ��ġ ���

				if( i3::same_of<WeaponC4*>(GetCurrentCharaWeapon()) )
				{
					if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON ) )
					{
						getActionContext()->_Play_Lower_Idle();
						removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
					}
					else
					{
						Cmd_Idle();
						removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
					}
				}

				return true;
			}
		}
	}

	return false;
}

bool CGameCharaBase::Cmd_RunAwayInstall( bool bEnable /*= true*/)
{
#if defined( DEF_OBSERVER_MODE)
	// Observer�� ó������ �ʴ´�.
	if( BattleSlotContext::i()->isObserverMe())
		return false;
#endif

	if( !isLocal() )
		return false;

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	if( bEnable)
	{
		// Jump �������� Ȯ��
		// Jump���̸� ��ġ �Ұ�
		if( isCharaStateMask( CHARA_STATE_JUMP) || !isCharaStateMask( CHARA_STATE_FLOOR) )
			return false;

		if( pMissionMng->IsEnableMy() == false)
			return false;

		if( pMissionMng->getMissionState() == GAME_MISSION_STATE_INBATTLE
			&& ( pMissionMng->IsEnableCrossHit() ))
		{//	�̼ǿ� ��ź�� ��ġ�Ǿ� ���� �ʴٸ�
			//	�̵� �Ұ�. 
			i3Vector::Zero( getMoveContext()->getMoveDir());

			if(( getActionContext()->isUpper( CHARA_UPPER_IDLE ) == true) 
				|| ( getActionContext()->isUpper( CHARA_UPPER_MOVE ) == true))
			{

				Cmd_Idle();
				// ��ü�� ������ �ɴ´�.
				//getActionContext()->_Play_Crouch();

				addEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH );
				addCharaStateMask( CHARA_STATE_ARMINGBOMB);

				pMissionMng->SendMissionInstall( true );
				return true;
			}

		}
		else
		{
			pMissionMng->SendMissionInstall( false );	//	��ź ��ġ ���
			removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
		}
	}
	else
	{
		INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();

		//ISNETWORK
		{
			if(	(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// �������� �ƴѰ��
				return false;

			if( pMissionMng->IsInstalling( mySlotIdx ) || pMissionMng->isMyMission() )
			{	//	�ڽ��� ��ź ��ġ ���̸� ���
				pMissionMng->SendMissionInstall( false );	//	��ź ��ġ ���
				removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

				return true;
			}
		}
	}

	return false;
}

void CGameCharaBase::Cmd_Invincible( bool bEnable, REAL32 InvincibleElapseTime)
{
	if( g_pFramework->IsBattleStage() == false)
		return;

	if( bEnable )
	{
		if( isAIToClient() && MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
		{
			AI_ENTRY *		pAiEntry = &AIModeContext::i()->AIEntry[getCharaNetIndex()];
			AI_BIND_DATA * pBindData = g_pCharaManager->getAiBindData( pAiEntry->_number );
			REAL32 InvTime = pBindData->_InvincibleTime;
			setInvincibleTime( InvTime );
		}
		else
		{
			// �����ð� ������ ��Ʈ�� �� �� �ִ�.
			if(InvincibleElapseTime <= 0.f)
				setInvincibleTime(getAbility(CHARA::eABILITY_INVINCIBLE_TIME));
			else
				setInvincibleTime(InvincibleElapseTime);
		}

		getMaterialContext()->SetBlendEnable( true);
	}
	else
	{
		setInvincibleTime( 0.f);
		getMaterialContext()->SetBlendEnable( false );
	}
}

void CGameCharaBase::Cmd_JumpStart( void)
{
	if( m_rJumpParalysis > 0)	// revision 53433  HK417 ������� �߰�
		return;

	if( isCharaStateMask( CHARA_STATE_DEATH))
		return;	// ���� ���¸� ��ŵ. �����ϸ� �ȵȴ�.

	if( getActionContext()->isLower( CHARA_LOWER_JUMP ))
		return; // �̹� ���� ���̴�.

	if( isLocal() && (( !isEnableStateMask( CHARA_STATE_DISABLE_MOVE) &&
		 ( isCharaStateMask( CHARA_STATE_FLOOR) ||
		    getMoveContext()->isJumpingOnLadder() ) )   &&
		!isCharaStateMask( CHARA_STATE_SLOPESLIDE)) ||
		!isLocal())
	{	
		if( getActionContext()->_Play_JumpStart())
		{
			getSoundContext()->_PlayJumpStartSound();
		}
	}
}

void CGameCharaBase::Cmd_Only_AttackWeapon( WEAPON::FIRE_ORDER type /*= WEAPON::PRIMARY_FIRE*/,
										   VEC3D * pPos /*= nullptr*/, VEC3D * pTarget /*= nullptr*/)
{	
	if (IsDino()) return;
	getWeaponContext()->_Fire( type, pPos, pTarget);
}

void CGameCharaBase::Cmd_Only_ChangeWeaponAction( void)
{
	getActionContext()->_Play_Upper_ChangeWeapon();
}

void CGameCharaBase::Cmd_Only_DamageAction( CHARA_HIT_PART part, VEC3D * pDir)
{
	getActionContext()->_Play_Damage( part, pDir);
}

void CGameCharaBase::Cmd_Observer( bool bEnable /*= true*/)
{
	if( bEnable)
	{
		if( i3::same_of<CGameCamera_FPS*>(g_pCamera))
		{
			// �ٲ�� ĳ���ʹ� 1��Ī��� ó��
			//ChangeFPSView( true);
			Cmd_ChangeView( CHARA_VIEW_1PV);
		}
		else
		{	// 3��Ī �����
			Cmd_ChangeView( CHARA_VIEW_3PV);
			//ChangeObserverView( true);

			if( i3::same_of<CGameCamera_Kill*>(g_pCamera))
			{
				if( ((CGameCamera_Kill*)g_pCamera)->isFPS() )
					SetVisible( false);
			}
		}

		g_pFramework->SetNightVisionEnable(false);
		addCharaStateMask( CHARA_STATE_OBSERVER);

		getHUDContext()->SetScope( false);
		getHUDContext()->ClearScreenEffect();
		Cmd_Observer_Zoom();
	}
	else
	{
		Cmd_ChangeView( CHARA_VIEW_3PV);
		//ChangeObserverView();
		removeCharaStateMask( CHARA_STATE_OBSERVER);
	}


	if (g_pGameContext)
	{
		if (UserInfoContext::i()->IsGM_Observer())
		{
			CGameFramework * pFramework = (CGameFramework*)i3Framework::getCurrentFramework();

			if (!bEnable)
			{
				if (pFramework->getSgContext()->GetXrayScene()->IsXrayState())
					g_pCharaManager->SetXrayState(this, true);
			}
			else
			{
				g_pCharaManager->SetXrayState(this, false);
			}
		}
	}

}

void CGameCharaBase::Cmd_Observer_Zoom( void)
{
	getCameraContext()->_Observer_Zoom();
}

void CGameCharaBase::Cmd_PickupWeapon( WeaponBase * pWeapon, UINT8 nSyncNumber, bool bCheckDropList, bool bPushWeaponList)
{
	if( isAIToClient() && MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
		return;

	if( pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY )
		hu::diable_check_primary_weapon_first_shot();

	getWeaponContext()->_Pickup( pWeapon, nSyncNumber, bCheckDropList, bPushWeaponList);
}

void CGameCharaBase::Cmd_Profile_Ceremony( void)
{
	WeaponBase * pWeapon = GetCurrentCharaWeapon();

	if( pWeapon != nullptr)	pWeapon->SetEnable( false);

	if( getActionContext()->getCommon() != CHARA_ACTION_COMMON_CEREMONY)
		getActionContext()->_Play_Ceremony();
}

void CGameCharaBase::Cmd_Profile_Idle_A( void)
{
	if (getWeaponContext())
	{
		WeaponBase * pWeapon = GetCurrentCharaWeapon();

		if( pWeapon != nullptr)	
		{
			pWeapon->SetEnable( true);
			pWeapon->PlayAnim_UI_IdleA();
		}
	}
	
	if (getActionContext())
		getActionContext()->_Play_Profile_Idle_A();
}

void CGameCharaBase::Cmd_Profile_Idle_B( void)
{
	WeaponBase * pWeapon = GetCurrentCharaWeapon();
	if( pWeapon != nullptr)
	{
		pWeapon->SetEnable( true);
		pWeapon->PlayAnim_UI_IdleB();
	}

	getActionContext()->_Play_Profile_Idle_B();
}

void CGameCharaBase::Cmd_Profile_Idle_Change( bool bIsSameWeapon)
{
	WeaponBase * pWeapon = GetCurrentCharaWeapon();

	// Kriss�� Dual ���Ⱑ �����ǵ��� ����
	if( i3::kind_of<WeaponKrissSuperV*>(pWeapon))
	{
		((WeaponKrissSuperV*)pWeapon)->SetEnableDualWeapon( true, false);
		getAnimContext()->ChangeAnimationContext();
	}
	else if( i3::kind_of<WeaponShotGunGenderWithDual*>(pWeapon) )
	{
		((WeaponShotGunGenderWithDual*)pWeapon)->SetEnableDualWeapon( true, false);
		getAnimContext()->ChangeAnimationContext();
	}

	// ���� �ִϸ��̼��� Idle Stand�̰�, ������ ���� ������ ���, Idle_Stand ȣ��
	if(getActionContext() && 
		getActionContext()->isUpper(CHARA_UPPER_NONEBATTLE_IDLE_STAND) && bIsSameWeapon)                                  
	{
		Cmd_Profile_Idle_Stand();
		return;
	}
	
	if( pWeapon != nullptr)	
	{
		pWeapon->SetEnable( true);
		pWeapon->PlayAnim_UI_IdleB();
	}

	if (getActionContext())
		getActionContext()->_Play_Profile_Idle_Change(bIsSameWeapon);
}

void CGameCharaBase::Cmd_Profile_Idle_Stand( void)
{
	WeaponBase * pWeapon = GetCurrentCharaWeapon();
	if( pWeapon != nullptr)
	{
		pWeapon->SetEnable( true);
		pWeapon->PlayAnim_UI_IdleA();
	}

	getActionContext()->_Play_Profile_Idle_Stand();
}

bool CGameCharaBase::Cmd_Reload( ACTION_LOAD_MAG_TYPE type /*= ACTION_LOAD_MAG*/)
{
	WeaponBase * pWeapon = GetCurrentCharaWeapon();

	if( pWeapon != nullptr)
	{
		// �������� ������ �غ� ����� �� ���
		if( type == ACTION_LOAD_READY_MAG &&
			pWeapon->getWeaponInfo()->IsLoadMagazineReady() && pWeapon->isReadyToLoadMagazine() == false)
		{
			if( getWeaponContext()->_Load_Magazine_Ready())
				return getActionContext()->_Play_Upper_Reload( ACTION_LOAD_READY_MAG);
			else
				return false;
		}

		// ���� �߻��� �Ǵ� �������� ���� ���
		if( pWeapon->getWeaponState() == WEAPON::STATE_NEEDTO_LOADBULLET ||
			type == ACTION_LOAD_MAG_BULLET || type == ACTION_LOAD_FIRE_BULLET)
		{
			if( getWeaponContext()->_Load_Bullet() )
			{
				if(type == ACTION_LOAD_FIRE_BULLET)
					return getActionContext()->_Play_Upper_Reload( ACTION_LOAD_FIRE_BULLET);
				else
					return getActionContext()->_Play_Upper_Reload( ACTION_LOAD_MAG_BULLET);
			}		
		}
		else if( (pWeapon->getWeaponState() == WEAPON::STATE_NEEDTO_QUICKLOAD) ||
			(type == ACTION_LOAD_MAG_QUICK) )
		{
			//	���� Extension ��� Ȱ��ȭ
			if( getWeaponContext()->_Load_Magazine_Dual() )
				return getActionContext()->_Play_Upper_Reload( ACTION_LOAD_MAG_QUICK);
		}
		else if( (pWeapon->getWeaponState() == WEAPON::STATE_NEEDTO_REMOVESHELL) ||
			(type == ACTION_LOAD_REMOVE_SHELL))
		{
			addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE);
			if( getWeaponContext()->_Remove_Shell())
				return getActionContext()->_Play_Upper_Reload( ACTION_LOAD_REMOVE_SHELL);
		}
		else if( (pWeapon->getWeaponState() == WEAPON::STATE_NEEDTO_LOAD_B_MAG) ||
			(type == ACTION_LOAD_MAG_B) )
		{
			if( getWeaponContext()->_Load_Magazine())
				return getActionContext()->_Play_Upper_Reload( ACTION_LOAD_MAG_B);
		}
		else if( (pWeapon->getWeaponState() == WEAPON::STATE_NEEDTO_LOADMAG) ||
			(type == ACTION_LOAD_MAG))
		{
			if( getWeaponContext()->_Load_Magazine())
				return getActionContext()->_Play_Upper_Reload( ACTION_LOAD_MAG);
		}
	}

	return false;
}

void CGameCharaBase::Cmd_Reset( void)
{
	getCameraContext()->_Idle_Camera();

	if( IsDino() == false)
		getWeaponContext()->_Idle();

	if ( !IsDomiDino())
		getActionContext()->_All_Stop();
}

void CGameCharaBase::Cmd_Respawn( VEC3D * pPos, REAL32 rTheta, bool bInvincible /* = true */)
{
	INT32 i;
	UINT32 hitMask;
	VEC3D vLength;


	//	ĳ���� ���� �ʱ�ȭ
	ResetChara();

	// ������ ī�޶�(Idle ������ FPSī�޶�� ����)
	getCameraContext()->_Respawn_Camera();

	SetEyeEnable( true);

	Cmd_Visible( true);

	// ����Ʈ ������ �� �ش�.
	if( isLocal())
		g_pFramework->SetNightVisionEnable(false);

	// ���� ȣ���ؾ� �մϴ�. (Respawn ���۽� ChangeWeapon�� ��ŵ���� �ʵ���) ���� 2010.11.08
	removeCharaStateMask( CHARA_STATE_DEATH);

	// respawn cnt�� ���� (���� ���ҽ� ������ ���˴ϴ�.) swoongoo
	setCharaInfo()->SetRespawnCnt( getCharaInfo()->GetRespawnCnt() + 1);
	
	if( IsDino() == false)
	{
		//	ó�� �������� 0�� ����� ��ü�ϴ� �����Ѵ�.
		getWeaponContext()->_Respawn();
	}

	// ����Ʈ�� ����Ʈ �ڽ��� Kinematic���� ����
	getCollisionContext()->SetContactShapeSetEnable( false);

	//	������ ��ġ ����
	forceSetPos( pPos);
	// ĳ���� �������� ���尡 ���� ���� ������ ����ó���� �˴ϴ�.
	getMoveContext()->setTakeOffPos( pPos);

#if defined( I3_DEBUG) | defined( BUILD_RELEASE_QA_VERSION)
	if( g_pConfig != nullptr && !g_pConfig->m_bNetworking)
	{
		MATRIX mat;
		VEC3D	vDir;
		
		i3Matrix::SetRotateY( &mat, getBoneContext()->getTheta() );
		i3Vector::Copy( &m_vDebugOriginalPos, pPos );
		i3Vector::TransformCoord( &vDir, getMoveContext()->getMoveDir(), &mat );
		i3Vector::Add( &m_vDebugTargetPos, pPos, &vDir );
		m_rDebugElp = 1.f;
	}
#endif

	// ������ �� ĳ���� üũ�� �Ͽ� �ɸ��� �ʵ��� �մϴ�.
	if( isLocal() )
	{
		INT32 charaCnt = g_pCharaManager->getCharaCount();
		for( i = 0; i < charaCnt; i++ )
		{
			CGameCharaBase * pOtherChara = g_pCharaManager->getChara( i );
			if( pOtherChara == this || pOtherChara == nullptr)
				continue;
			
			i3Vector::Sub( &vLength, pPos, pOtherChara->GetPos() );

			REAL32 rLength = i3Vector::Normalize( &vLength, &vLength );
			if( rLength > 1.f )
				continue;
		
			MATRIX mat;
			i3Vector::Set( &vLength, 0.f, 0.f, -1.f );
			i3Matrix::SetRotateY( &mat, I3_PI * 0.12f * getCharaNetIndex() );
			i3Vector::TransformCoord( &vLength, &vLength, &mat );

			getMoveContext()->getPhysixController()->Move( &vLength, 0.01f, &hitMask);
			getMoveContext()->GetPosition( &vLength);

			forceSetPos( &vLength);
		}
	}
	else
	{
		// ĳ���� ��ũ ����
		getMaterialContext()->SetTeamMarkEnable();
	}

	if( getBoneContext())
		getBoneContext()->setTheta( rTheta );

	Cmd_Invincible( bInvincible);		//	ĳ���� ���� ����
	
	addEnableStateMask( CHARA_STATE_DISABLE_SHOT );

	removeEnableStateMask( CHARA_STATE_DISABLE_SHOT |
		CHARA_STATE_DISABLE_CONTROL | CHARA_STATE_DISABLE_MOVE );

	getHUDContext()->_ApplyCrosshair();

	// ������ NightVision ȿ��
	if( isLocal() && IsDino())
	{
		/*g_pFramework->SetVisionMaskingType( VE_DINO_EYE);
		g_pFramework->SetNightVisionEnable( true);*/
	}

	if( g_pCamera != nullptr )
	{	// ���� ��ġ���� �ѹ� ������ �Ǵ� ���� ���� ���� ������Ʈ�� �ѹ� ������ �ִϸ��̼��� �÷��� ��ŵ�ϴ�.
		// ������ ��ġ�� �ű��
		if (getAnimContext())
			getAnimContext()->OnUpdate( 0.016f);
		
		getUpdateContext()->OnUpdate( 0.016f);
	}

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		CStageBattle * pStage = g_pFramework->GetStageBattle();
		I3ASSERT(pStage);

		if(pStage != nullptr && gmode::i()->GetTutorialMode() != nullptr)
		{
			if( gmode::i()->GetTutorialMode()->GetTutorialState() != TS_END )
			{
				gmode::i()->GetTutorialMode()->FirstTickInTutorialState();
			}
		}
	}

	// revision 52959 �Ͽ︵ ������ ���� �ʱ�ȭ.
	setDamagedByHowl( false);
	setDamagedByHowlTime( 0.0f);

	if( IsDino() )
	{
		// hansoft. 12656 : ������ ���� �� �ִϸ��̼��� ���������� �۵� �� ��.
		Cmd_Idle_Lower();
		Cmd_Idle_Upper();
	}

	g_pCharaManager->SetXrayState(this, true);
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	pFramework->getSgContext()->GetXrayScene()->SetXrayState(true);
}

bool CGameCharaBase::Cmd_Return_Upper( void)
{
	//	��ü�� ���� ���·� ���ư���.
	return getActionContext()->_Play_Upper_Return();
}

void CGameCharaBase::Cmd_Return_Lower( void)
{
	getActionContext()->_Play_Lower_Return();
}

void CGameCharaBase::Cmd_SyncWeapon( void)
{
	getWeaponContext()->_SyncWeapon();
}

void CGameCharaBase::Cmd_ToggleBodyState( void)
{
	getActionContext()->_Toggle_BodyState();
}

bool CGameCharaBase::Cmd_ToggleExtensionWeapon( void)
{
	WeaponBase * pWeapon = GetCurrentCharaWeapon();
	if( pWeapon == nullptr )
		return false;
	
	return Cmd_Extension( !pWeapon->isExtensionActivate());
}

void CGameCharaBase::Cmd_ToggleZoom( void)
{
	WeaponBase * pWeapon = GetCurrentCharaWeapon();
	if( pWeapon == nullptr )		return;

	pWeapon = pWeapon->getUsedWeapon();
	I3ASSERT( pWeapon != nullptr);

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr );

	WEAPON::ZOOM_TYPE type = pWeaponInfo->GetTypeZoom();

	//	���� ����� �ִ� ����
	switch( type)
	{
	case WEAPON::ZOOM_ZOOM :
		Cmd_Extension( !isCharaStateMask( CHARA_STATE_ZOOM));
		break;

	case WEAPON::ZOOM_SCOPE :
		//	������ ����� �ִ� ����
		if( isCharaStateMask( CHARA_STATE_ZOOM))
		{
			if( getBoneContext()->getScopeWaveData()->m_nCurScopeLevel < pWeaponInfo->GetMaxScopeLevel() )
				Cmd_Extension( true);
			else
				Cmd_Extension( false);
		}
		else
			Cmd_Extension( true);
		break;

	case WEAPON::ZOOM_STEADY_AIM :
		Cmd_Extension( ! isCharaStateMask( CHARA_STATE_ZOOM));
		break;
	}
}
bool CGameCharaBase::Cmd_ToggleZoomForDSR( void)
{
	WeaponBase * pWeapon = GetCurrentCharaWeapon();
	if( pWeapon == nullptr )	return false;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	if( pWeaponInfo == nullptr )	return false;

	pWeapon->SetVisible( true);

	if( pWeaponInfo->isCameraZoomType())
		{
			
			if( !isCharaStateMask( CHARA_STATE_ZOOM))
			{
				if( getWeaponContext()->_Zoom() )
					return getActionContext()->_Play_Zoom();
			}
			else
			{
				if( getActionContext()->_Play_ZoomCancel())
				{	//	�Ϲ� ī�޶� ����
					getCameraContext()->_Idle_Camera();
				}
			}

			
		}
	return true;

}

void CGameCharaBase::Cmd_Turn( REAL32 deg /*= 180.f*/)
{
	REAL32 rTheta = getBoneContext()->getTheta();

	rTheta		+= I3_DEG2RAD( deg);
	getBoneContext()->setTheta( i3Math::NormalizeAngle( rTheta));

	MATRIX * pCharaMat = getSceneObject()->GetMatrix();
	i3Matrix::PreRotateAxisDeg( pCharaMat, &I3_YAXIS, 180.0f);

	// Recoil ���� ���� Restore ���¶�� �� ���¸� ������ �־�� �Ѵ�.
	getBoneContext()->cancelRecoilHorz();
}

void CGameCharaBase::Cmd_Visible( bool bVisible)
{
	SetEnable( bVisible);

	if (getMoveContext())
		getMoveContext()->EnableController( bVisible);

	// ����ũ �� ����
	if (getMaterialContext())
		getMaterialContext()->SetTeamMarkEnable();
}

void CGameCharaBase::Cmd_SetCharaNick( bool bFriendly)
{
	CGameCharaHUDContext * pHUD = getHUDContext();
	if( pHUD != nullptr )
	{
		pHUD->ShowCharacterNick( bFriendly);
	}
}

void CGameCharaBase::Cmd_SetColor( COLORREAL * pDiff /*= nullptr*/,
								  COLORREAL * pSpec /*= nullptr*/, REAL32 alpha /*= -1.f*/)
{
	getMaterialContext()->SetMaterialColor( pDiff, pSpec);
	getMaterialContext()->SetBlendAlpha( alpha);

	if( getEquipContext() != nullptr)
	{
		getEquipContext()->SetMaterialColor( EQUIP::ePART_HEADGEAR, pDiff, pSpec, true);
		getEquipContext()->SetMaterialColor( EQUIP::ePART_FACEGEAR, pDiff, pSpec, true);
		getEquipContext()->SetMaterialColor( EQUIP::ePART_HOLSTER, pDiff, pSpec, true);
		getEquipContext()->SetMaterialColor( EQUIP::ePART_BELT, pDiff, pSpec, true);

		getEquipContext()->SetBlendAlpha( EQUIP::ePART_HEADGEAR, alpha, true);
		getEquipContext()->SetBlendAlpha( EQUIP::ePART_FACEGEAR, alpha, true);
		getEquipContext()->SetBlendAlpha( EQUIP::ePART_HOLSTER, alpha, true);
		getEquipContext()->SetBlendAlpha( EQUIP::ePART_BELT, alpha, true);
	}

	if( getWeaponContext() != nullptr)
	{
		getWeaponContext()->_Change_Color( pDiff, pSpec, alpha);
	}
}

void CGameCharaBase::Cmd_SetPhysics( void)
{
	CGameCharaEquip * pEquip;

	if( IsDino())	return;

	// Equip�� ���°� �پ��ִ� ���¶�� Collision�� dynamic�� ���ϴ�.
	if( getEquipContext()->isAttachOnChara( EQUIP::ePART_HEADGEAR ) )
	{	
		pEquip = getEquipContext()->getEquip( EQUIP::ePART_HEADGEAR );
		if( pEquip != nullptr)
		{
			pEquip->setDynamicActiveState( true, false );
			pEquip->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true);
		}
	}

	if( getEquipContext()->isAttachOnChara( EQUIP::ePART_FACEGEAR))
	{
		pEquip = getEquipContext()->getEquip( EQUIP::ePART_FACEGEAR );
		if( pEquip != nullptr)
		{
			pEquip->setDynamicActiveState( true, false );
			pEquip->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true);
		}
	}

	// ��Ʈ
	// �������� Physic�� ���� �ʴ´�.
	{
		pEquip = getEquipContext()->getEquip( EQUIP::ePART_BELT );
		if( pEquip != nullptr)
		{
			pEquip->setDynamicActiveState( true, false );
			pEquip->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true);
		}
	}

	{	
		pEquip = getEquipContext()->getEquip( EQUIP::ePART_HOLSTER );
		if( pEquip != nullptr)
		{
			pEquip->setDynamicActiveState( true, false );
			pEquip->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true);
		}
	}

	// �ֹ��Ⱑ � �پ� ������ Kinematic���� �մϴ�.
	if( getWeaponContext()->getBackpack() != nullptr )
	{
		getWeaponContext()->getBackpack()->setDynamicActiveState( true, false );
		getWeaponContext()->getBackpack()->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true);
		getWeaponContext()->getBackpack()->SetEnable( true);
	}
}

void CGameCharaBase::Cmd_SpeedReduction( CHARA_HIT_PART part, INT32 nCurHP, INT32 nAfterHP)
{
	getMoveContext()->EnableSpeedReduction( (UINT8) part);

	if( nAfterHP > 0 && nAfterHP < 20 && nCurHP >= 20)
	{
		g_pSndMng->StartCharacterActionSnd( GTBCA_ON_HEARTBEAT, 0, nullptr, true);
	}
}

bool CGameCharaBase::Cmd_StandUp( void)
{
	if (m_rCrouchParalysis > 0.f)
		return false;

	if( !getActionContext()->isBody( CHARA_BODYLOWER_STAND ) )
	{
		//if( !isCharaStateMask( CHARA_STATE_CHANGEPOSE))	// revision 48244 �ɱ� ���� �� CTRL Ű�� ���� �ݺ� �ؼ� ���� �� ���, �ɱ� ������ ������ ��
		{	// �ڼ� �������� �ƴ� ��쿡�� ó��
			if( isEnableStateMask( CHARA_STATE_DISABLE_CROUCH) == false)
			{
				getActionContext()->_Play_StandUp();
				return true;
			}
		}
	}

	return false;
}

void CGameCharaBase::Cmd_Run( VEC3D * pDir, bool bUpper /*= true*/, bool bLower /*= true*/)
{
	if( m_rRunParalysis > 0.f)	// revision 53433 HK417 ������� �߰�
		return;

	if( bLower && pDir != nullptr)
	{
		if (IsDino())
		{
			getActionContext()->_Play_Dino_Lower_Run( pDir);
		}
		else
		{
			getActionContext()->_Play_Lower_Run( pDir);
		}
	}

	if( bUpper)
	{
		if( getActionContext()->isUpper( CHARA_UPPER_IDLE) ||
			getActionContext()->isUpper( CHARA_UPPER_JUMP) ||
			getActionContext()->isUpper( CHARA_UPPER_MOVE))
		{
			getActionContext()->_Play_Upper_Run();
		}
	}
}

void CGameCharaBase::Cmd_JumpEnd( void)
{
	getActionContext()->_Play_JumpEnd();

	getAnimContext()->setOldLowerID( ID_LOWER_IDLE);

	if( getActionContext()->isUpper( CHARA_UPPER_JUMP) ||
		getActionContext()->isUpper( CHARA_UPPER_IDLE) ||
		getActionContext()->isUpper( CHARA_UPPER_MOVE) )
	{// ��ü�� �������̰ų� ���̵� �����϶��� ���� ���带 �Ѵ�.
		getActionContext()->_Play_Upper_JumpEnd();
		getAnimContext()->setOldUpperID( ID_UPPER_IDLE);
	}

	// ��� ������ ��� ���ʰ� �������� �ȸ����� ���� ó������� �Ѵ�.
	if( getAnimContext()->getAIUpper_Left() != nullptr )
	{
		CHARA_ACTION_UPPER	eActionUpper	= getActionContext()->getUpperLeft();

		if( eActionUpper == CHARA_UPPER_JUMP 
			|| eActionUpper == CHARA_UPPER_IDLE 
			|| eActionUpper == CHARA_UPPER_MOVE
		  )
		{
			getActionContext()->_Play_Dual_JumpEnd_Left();
		}
	}

	getSoundContext()->_PlayJumpEndSound();
}

void CGameCharaBase::Cmd_Roar( void)
{
	((CGameDinoActionContext*)getActionContext())->_Play_Dino_Roar();
	((CGameDinoAnimContext*)getAnimContext())->setOldDinoLowerID( ID_LOWER_DINO_IDLE);
}


void CGameCharaBase::Cmd_Dino_Idle( void)
{
	((CGameDinoActionContext*)getActionContext())->_Play_Dino_Idle();
	((CGameDinoAnimContext*)getAnimContext())->setOldDinoLowerID( ID_LOWER_DINO_IDLE);
}


void CGameCharaBase::Cmd_Dino_Stun( void)
{
	((CGameDinoActionContext*)getActionContext())->_Play_Dino_Stun();
	((CGameDinoAnimContext*)getAnimContext())->setOldDinoLowerID( ID_LOWER_DINO_IDLE);
}

void CGameCharaBase::Cmd_Damage( CHARA_DAMAGEINFO * pInfo)
{
	I3ASSERT( pInfo != nullptr);

	// revision 46126���� ���Ḧ üũ �ؼ� ���尡 ���� ������ �������� ���Ե��� �Ѵ�.
	if( !g_pFramework->IsBattleStage())				return;
	if( gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE)	return;

	// ��������
	if( isInvincible() )
		return;

	// revision 53433 HK417 ������� �߰�
	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( pInfo->_ui32FireWeaponID );
	if( (pWeaponInfo != nullptr ) && ( pWeaponInfo->IsParalysis()))
	{
		Cmd_Idle_Lower();	// revision 54229
		m_rJumpParalysis = pWeaponInfo->GetJumpParalysis();
		m_rWalkParalysis = pWeaponInfo->GetWalkParalysis();
		m_rRunParalysis = pWeaponInfo->GetRunParalysis();
		m_rCrouchParalysis = pWeaponInfo->GetCrouchParalysis();
	}

	//M14�� ������ �ɰ� ��
	if (i3::same_of<WeaponM14*>(pInfo->_pWeapon) &&	(pInfo->GetFireWeaponClass() == WEAPON_CLASS_THROWING_GRENADE))
	{
		if (getActionContext()->isBody(CHARA_BODYLOWER_CROUCH))
		{
			Cmd_StandUp();
		}
		else if (getActionContext()->isBody(CHARA_BODYLOWER_STAND))
		{
			Cmd_Crouch();
		}
		m_rCrouchParalysis = pWeaponInfo->GetCrouchParalysis();
	}


	// Medical Kit �϶� ����Ʈ, Hit Motion ��� ����
	if(i3::same_of<WeaponMedicalKit*>(pInfo->_pWeapon))
		return;

	if( pInfo->_DamageType == WEAPON::DAMAGE_POISON)
	{
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( pInfo->_nKillerIdx);
		if (pChara == nullptr) return; 

		if(pChara->IsSting())
		{
			CGameCharaDinoSting * pSting = (CGameCharaDinoSting*)pChara;

			// ��ų ���ӽð� / �̵� �ӵ��� 
			REAL32 rSkillDuration = pSting->GetDinoSkillDuration(WEAPON::SECONDARY_FIRE);
			REAL32 rSkillMovementRate = pSting->GetDinoSkillMovementRate(WEAPON::SECONDARY_FIRE);

			//blur
			getHUDContext()->ScreenEffect_Howl( rSkillDuration);
			// �̼� ���� ȿ��
			getMoveContext()->EnableSpeedReduction(rSkillMovementRate, rSkillDuration );
		}
	}
	
	//C4 ��ġ�� ����ϴ� �ڵ� �Դϴ�. ( revision 64090 WPSmoke �������� ��Ұ� �ȵǴ� ���� ���� - 2013. 9. 4 �ѻ���)
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) && !(isCharaStateMask(CHARA_STATE_DEATH))) // (pInfo->_DeathType != CHARA_DEATH_POISON) )
	{
		CGameMissionManager::i()->Install( getCharaNetIndex(), CGameMissionManager::i()->WhoInstallingArea( getCharaNetIndex() ), false );
		CGameMissionManager::i()->Uninstall( getCharaNetIndex(), false );

		if( isLocal())
		{
			//	��ź ��ġ ���̸� ����ϵ���
			if( isCharaStateMask( CHARA_STATE_ARMINGBOMB) )
				Cmd_Install( false);
			//	��ź ��ü ���̸� ����ϵ���
			else if( isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
				Cmd_Uninstall( false);
		}
	}

	//C4 ��ġ�� ����ϴ� �ڵ� �Դϴ�. ( revision 64090 WPSmoke �������� ��Ұ� �ȵǴ� ���� ���� - 2013. 9. 4 �ѻ���)
	if(  MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY) && !(isCharaStateMask(CHARA_STATE_DEATH))) // (pInfo->_DeathType != CHARA_DEATH_POISON) )
	{
		CGameMissionManager::i()->Install( getCharaNetIndex(), CGameMissionManager::i()->WhoInstallingArea( getCharaNetIndex() ), false );
		CGameMissionManager::i()->Uninstall( getCharaNetIndex(), false );

		if( isLocal())
		{
			//	��ź ��ġ ���̸� ����ϵ���
			if( isCharaStateMask( CHARA_STATE_ARMINGBOMB) )
				Cmd_RunAwayInstall( false);
		}
	}
	
	if( IsDino() == false)
	{
		// Hit Effect
		if( ((CHARA_HIT_PART) pInfo->GetHitPart() == CHARA_HIT_HEAD) &&
			isAttachedProtectParts() &&	(pInfo->_pVecDir != nullptr) )
		{
			if( pInfo->_HelmetFlag & 0x01)
			{
				// ����� ����߸���.
				if( ! pInfo->isFireWeaponType(WEAPON_CLASS_THROWING_GRENADE) )
					DropEquip( pInfo->_pVecDir);
			}
		
			//	��信 �¾� �Ҳ��� Ƥ��.
			HelmetEffect( pInfo);
		}
	}
	
	Cmd_HitEffect(pInfo);

	bool bDamageAction = true;
	if( IsRex())
	{
		if( pInfo->GetHitPart() != CHARA_HIT_VITAL_EYE)
		{
			bDamageAction = false;
		}
	}

	// Damage Action
	if( (pInfo->_DeathType == CHARA_DEATH_POISON) && is1PV() )
	{
		if( isLocal() && (pInfo->_DeathType == CHARA_DEATH_POISON) &&
			( isCharaStateMask( CHARA_STATE_ARMINGBOMB ) || isCharaStateMask(CHARA_STATE_DISARMINGBOMB) ) )
		{
			// �鸰ź
			// �����̸鼭 ���Ĺ̼ǿ��� ��ġ�ϰų� �������ΰ��� �׼��� ������ �ʽ��ϴ�.
			bDamageAction = false;
		}
	}

	if( bDamageAction)
	{
		VEC3D dir;

		if( pInfo->_pVecDir != nullptr)	i3Vector::Copy( &dir, pInfo->_pVecDir);
		else							i3Vector::Set( &dir, 0.f, 0.f, 1.f);

		if( getActionContext())
			getActionContext()->_Play_Damage( pInfo->GetHitPart(), &dir);
	}
	

	// Head Watch system
	// �������� �ִ� ĳ���͸� �ֽ��ϵ��� ����
	if( pInfo->_nKillerIdx > -1 && pInfo->_nKillerIdx < SLOT_MAX_COUNT)
		getBoneContext()->setHeadWatchUserIdx( pInfo->_nKillerIdx);

}

void CGameCharaBase::Cmd_DamageSelf( INT32 nDamage, UINT16 iObjectIdx /*= 0xFFFF*/, bool bFalling /*= false*/)
{
	VEC3D vDir;
	i3Vector::Set( &vDir, 0.f, 0.f, 1.f );

	// revision 46126 ���� ���Ḧ üũ �ؼ� ���尡 ���� ������ �������� ���Ե��� �Ѵ�.
	if( !g_pFramework->IsBattleStage())				return;
	if( gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE )return;

	if( nDamage > 0)
	{
		CHARA_DEATH_TYPE	DeathType = CHARA_DEATH_F;

		if( iObjectIdx != 0xFFFF)
		{	
			DeathType		= CHARA_DEATH_FAST_OBJECT;
		}
		if( BattleServerContext::i()->IsP2PHost() )
		{
			// ���常 ó���մϴ�. Client�� ��Ŷ���� ������ ó���մϴ�.

			// ���� ó��
			if( bFalling && getCurHP() > nDamage && g_pCamera != nullptr && is1PV())
			{
				REAL32 TrembAmpRate = MIN( (REAL32)nDamage, getFullHP()) / getFullHP();

				g_pCamera->SetTremble( 2.5f + TrembAmpRate, 1.0f, 0.2f, 0.5f);

				_PlayDownfallSound();

				INT32 HP = getCurHP() - nDamage;
				if(( getCurHP() > 20) && ( HP <= 20) && ( HP > 0))
				{
					g_pSndMng->StartCharacterActionSnd( GTBCA_ON_HEARTBEAT, 0, nullptr, true);
					//I3TRACE("�Ͼ�~ �Ͼ�~~�Ͼ�~~~!~!~!~\n");
				}
			}

			setCharaInfo()->DecreaseHP((REAL32)nDamage);

			REAL32 hp = MAX( getCharaInfo()->GetHP(), 0.f);

			setCharaInfo()->SetHP( hp);

			GAMEINFO_CHARACTER * pCharaInfo		= BattleSlotContext::i()->setGameInfo_Chara( getCharaNetIndex() );
			pCharaInfo->_tNetData._tHP.setHP( (UINT16) hp);

			if( hp <= 0.f)
			{	// ���
				g_pCharaManager->DeathChara( this, getCharaNetIndex(), &vDir, CHARA_DEATH_A );

				GAMETOCONTEXT_INFO_CHARA_DEATH deathInfo;

				deathInfo.Reset();
				deathInfo._iKillidx			= getCharaNetIndex();
				deathInfo._iDeathidx		= getCharaNetIndex();

				WEAPON_CLASS_TYPE classType = WEAPON_CLASS_UNKNOWN;
				INT32 i32Number = 0;
				WEAPON_SLOT_TYPE slotType= WEAPON_SLOT_UNKNOWN;
				
				if(IsAcid())
				{
					classType = WEAPON_CLASS_DINO;
					slotType = WEAPON_SLOT_PRIMARY;
					i32Number = WEAPON::getItemIndex( WEAPON::DINO_ACID_EXPLOSION);
				}
				else if (IsCCAcid())
				{
					classType = WEAPON_CLASS_DINO;
					slotType = WEAPON_SLOT_PRIMARY;
					i32Number = WEAPON::getItemIndex( WEAPON::DINO_CC_ACID_EXPLOSION);
				}
				else if (IsAcidVulcan())
				{
					classType = WEAPON_CLASS_DINO;
					slotType = WEAPON_SLOT_PRIMARY;
					i32Number = WEAPON::getItemIndex( WEAPON::DINO_ACID_VULCAN_EXPLOSION);
				}
				else
				{
					classType = WEAPON_CLASS_KNIFE;
					slotType = WEAPON_SLOT_MELEE;
					i32Number = WEAPON::getItemIndex( WEAPON::KNIFE_M7);
				}

				deathInfo._ui32WeaponID	= Weapon::GetItemID(classType, i32Number);

				if( iObjectIdx != 0xFFFF)
				{
					deathInfo._iByObjectIdx		= iObjectIdx;
				}

				deathInfo._nDeathType		= DeathType;
				deathInfo._nHitPart			= CHARA_HIT_BODY1;
				deathInfo._pHitDir			= &vDir;
				
				GAMEEVENT::Write_Chara_Death( &deathInfo);
			}
			else
			{
				GAMEEVENT::Write_Chara_Suffering( this, CHARA_DEATH_F, getCharaNetIndex(), CHARA_HIT_BODY1, false);
			}
		}

		if( isLocal() )
		{
			MIN( nDamage, 255);
			GAMEEVENT::Write_Chara_SuicideDamage( this, DeathType, CHARA_HIT_BODY1,
				(UINT16) nDamage, &vDir, WEAPON_CLASS_KNIFE, WEAPON::getItemIndex(WEAPON::KNIFE_M7), iObjectIdx, getCharaNetIndex());

#if 0
			if( bFalling)
			{//���Ͻ� hp�� �������� ����ϴ� ���� Ȯ�� ���� �α� �߰� hansoft.4675
				I3PRINTLOG(I3LOG_NOTICE, "##falling suicide damage## slot(%d) nick(%s) full_hp(%d) current_hp(%d) damage(%d)",
					getCharaNetIndex(), BattleSlotContext::i()->getNickForSlot(getCharaNetIndex()), getFullHP(), getCurHP(), nDamage);
			}
#endif
		}
	}

	if( isLocal() )
	{
		getHUDContext()->_Damage( nullptr, false);
	}

	getActionContext()->_Play_Damage( CHARA_HIT_UNKNOWN, nullptr, true);
}


void CGameCharaBase::Cmd_Death( INT32 iShooterIdx, CHARA_DEATH_TYPE type, VEC3D * pHitDir)
{
	CStageBattle * pStage = g_pFramework->GetStageBattle();

	if( pStage == nullptr)
		return;

	// revision 53433 HK417 ���� �߰�
	m_rJumpParalysis = 0.f;
	m_rWalkParalysis = 0.f;
	m_rRunParalysis = 0.f;
	m_rCrouchParalysis = 0.0f;

	//  Character HP�� 0
	setCharaInfo()->SetHP( 0.f);

	//	�Ϲ� ī�޶� ���� (ĳ���� Visible On, ScopeHUD off, Cam FOV default)
	getCameraContext()->_Idle_Camera();

	if(UserInfoContext::i()->IsGM_Observer() == false)
	{
		if( isLocal() )
		{	
			getCameraContext()->_Kill_Camera( iShooterIdx);
		}
	}

	BattleSlotContext::i()->SetKillerIdxMostRecent(iShooterIdx);
	
	// �̵����� 0����
	getMoveContext()->setVelocity( 0.0f, 0.0f, 0.0f);
	getMoveContext()->setAccel( 0.0f, 0.0f, 0.0f);

	if( getHUDContext()->IsEffectAvailable())
		getHUDContext()->ClearScreenEffect();

	if( IsDino() == false)
	{
		//	���⵵ ��� ������ �����ϰ� Ready ���·�
		WeaponBase * pWeapon = GetCurrentCharaWeapon();

		if( pWeapon != nullptr)
		{
			pWeapon->ReturnToReady();
			pWeapon->SetEnable( true);
		}
	}
	
	//if( (UserInfoContext::i()->IsGM_Observer() == true && iShooterIdx == BattleSlotContext::i()->getMySlotIdx()))
	if( BattleSlotContext::i()->IsGMUserForClient( getCharaNetIndex()) && BattleSlotContext::i()->IsGMObserverForClient( getCharaNetIndex()))
	{
		I3TRACE( " [CGameCharaManager::DeathHuman]GM Death animation is not play..\n");
	}
	else
		getActionContext()->_Play_Death( iShooterIdx, type, pHitDir);

	addCharaStateMask( CHARA_STATE_DEATH);
	getUpdateContext()->setToHideTime( 1.f);
	getUpdateContext()->setHideTimer( 1.f);

	getHUDContext()->_ApplyCrosshair();

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Tutorial, 23);
	}

	g_pCharaManager->SetXrayState(this, false);
}

void CGameCharaBase::Cmd_DeleteWeapon( WEAPON_SLOT_TYPE slot)
{
	if (getWeaponContext())
		getWeaponContext()->_Delete_Weapon( slot);
	else
		TRACE("CGameCharaBase::Cmd_DeleteWeapon getWeaponContext = nullptr");
}

void CGameCharaBase::Cmd_DetachWeapon( WEAPON_SLOT_TYPE slot)
{
	if (getWeaponContext())
		getWeaponContext()->_Detach_Weapon( slot);
	else
		TRACE("CGameCharaBase::Cmd_DetachWeapon getWeaponContext = nullptr");
}

void CGameCharaBase::Cmd_DetachWeapon( WeaponBase * pWeapon)
{
	if (getWeaponContext())
		getWeaponContext()->_Detach_Weapon( pWeapon);
	else
		TRACE("CGameCharaBase::Cmd_DetachWeapon getWeaponContext = nullptr");
}

bool CGameCharaBase::Cmd_DropEquip( EQUIP::ePART equipType, VEC3D * pPos)
{
	if (!getEquipContext())
		return false;

	bool bv = getEquipContext()->DropEquip(equipType, getSceneObject()->GetParent()->GetParent(), getParent(), pPos);

	// ����� ���ܠ����� ���� �Ӹ��� ��ü
	((Avatar*)this)->SetCharaHead( false);

	return bv;
}

bool CGameCharaBase::Cmd_DropWeapon( WEAPON_SLOT_TYPE iSlotIdx, VEC3D * pPos, bool bNextWeapon /*= true*/, bool bDirectPickup /*=false*/)
{
	if (getWeaponContext())
	{
		return getWeaponContext()->_Drop( iSlotIdx, pPos, bNextWeapon, bDirectPickup);
	}
	
	TRACE("CGameCharaBase::Cmd_DropWeapon getWeaponContext = nullptr");
	return false;
}

bool CGameCharaBase::Cmd_Extension( bool bEnable)
{
	WeaponBase * pWeapon = GetCurrentCharaWeapon();
	if( pWeapon == nullptr )	return false;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	if( pWeaponInfo == nullptr )	return false;

	CGameCharaActionContext * pActionCtx = getActionContext();
	if( pActionCtx == nullptr )	return false;

	pWeapon->SetVisible( true);

	if (i3::same_of<WeaponSniperDualMagazine*>(pWeapon)
		&& pActionCtx->isBody2( CHARA_BODYUPPER_ZOOM) )
	{
		Cmd_ToggleZoomForDSR();
	}
	else if (i3::same_of<WeaponKrissSuperV*>(pWeapon))	// hansoft. 12594. ���� ���� �߻����� �ӽ� ó��. ���� ���� ��� �ʿ���.
	{
		WeaponKrissSuperV* kriss = static_cast<WeaponKrissSuperV*>(pWeapon);
		if (kriss->m_pDualWeapon)
		{
			if (kriss->m_pDualWeapon->getLoadedBulletCount() <= 0)
			{
				if (kriss->GetCurrentExtensionType() == WEAPON::EXT_DUAL)
					kriss->setExtensionType(WEAPON::EXT_DOT_SIGHT);
			}
		}
	}

	switch( pWeapon->GetCurrentExtensionType())
	{
	case WEAPON::EXT_NONE:
	case WEAPON::EXT_SCOPE :
	case WEAPON::EXT_DOT_SIGHT:
	case WEAPON::EXT_STEADY_AIM:
	case WEAPON::EXT_ZOOM :					// ����
 		if( pWeaponInfo->isCameraZoomType())
		{
			if( bEnable)
			{
				//pWeapon->setExtensionActivate(true);
				// SCOPE�� ���ÿ��� Scope�� �����ٰ� �˷��־�� �մϴ�.
				// Jinsik.kim(2015/04/23)
				pWeapon->addWeaponStateMask(WSM_ACTIVATE_SCOPE);
				if( getWeaponContext()->_Zoom() )
					return getActionContext()->_Play_Zoom();
			}
			else
			{
				//pWeapon->setExtensionActivate(false);
				pWeapon->removeWeaponStateMask(WSM_ACTIVATE_SCOPE);
				if( getActionContext()->_Play_ZoomCancel())
				{	//	�Ϲ� ī�޶� ����
					getCameraContext()->_Idle_Camera();
				}
			}
		}
		break;
	
	default:	//	���� Extension ��� Ȱ��ȭ
		//	���� Extension ����
		if( getWeaponContext()->_Extension( bEnable) )
			return getActionContext()->_Play_Extension( bEnable);
		break;
	}

	return false;
}

bool CGameCharaBase::Cmd_HitEffect( CHARA_DAMAGEINFO * pDamageInfo)
{
	if(UserInfoContext::i()->IsGM_Observer())
	{
		return false;
	}

	// Medical Kit �϶�, Hit ����Ʈ ��� ����
	if(i3::same_of<WeaponMedicalKit*>(pDamageInfo->_pWeapon))
		return false;
	
	// revision 48465 //���� Ÿ�ݽ� Sound �� �� ������ ���� ����
	// revision 49123
	if(IsRex() == false)
	{
		if( IsHelmProtection() && isCharaStateMask( CHARA_STATE_DEATH) == false)
		{
			return false;
		}
	}

	if ( isLocal() && !(pDamageInfo->_DeathType == CHARA_DEATH_BUFF) )
	{
		INT32 temp_index = -1;
		VEC3D vDir;
		if (!(pDamageInfo->_pWeaponInfo == nullptr))
		{
			temp_index = pDamageInfo->_pWeaponInfo->GetWeaponSpecialHitHudIndex();
			i3Vector::Sub(&vDir, pDamageInfo->_pVecPos, GetPos());
			getHUDContext()->_Damage(&vDir, ((pDamageInfo->_DeathType == CHARA_DEATH_A) || (pDamageInfo->_DeathType == CHARA_DEATH_H)), temp_index);
		}
		else
		{
			CWeaponInfo* find_weaponinfo = nullptr;
			find_weaponinfo = g_pWeaponInfoDataBase->getWeaponInfo(pDamageInfo->_ui32FireWeaponID);
			if (find_weaponinfo != nullptr)
			{
				temp_index = find_weaponinfo->GetWeaponSpecialHitHudIndex();
				i3Vector::Sub(&vDir, pDamageInfo->_pVecPos, GetPos());
				getHUDContext()->_Damage(&vDir, ((pDamageInfo->_DeathType == CHARA_DEATH_A) || (pDamageInfo->_DeathType == CHARA_DEATH_H)), temp_index);
			}
		}
	}

	I3_PHYSIX_HIT_RESULT * pResult;
	I3ASSERT( pDamageInfo != nullptr);

	INT32 effectCount = 1;

	if( !pDamageInfo->_EnableEffect ||
		(!isCharaFlag(CHARA_FLAG_ENABLE_EFFECT) && ((pDamageInfo->_DeathType == CHARA_DEATH_POISON) ||
		(pDamageInfo->_DeathType == CHARA_DEATH_TRAMPLED) || (pDamageInfo->_DeathType == CHARA_DEATH_HOWL)) ))
		return false;
	
	switch( pDamageInfo->GetHitPart() )
	{
	case CHARA_HIT_VITAL_EYE:
	case CHARA_HIT_NECK_LOWER:
	case CHARA_HIT_HEAD :	effectCount = 6;	break;
	case CHARA_HIT_NECK :
	case CHARA_HIT_LCLAVICLE :
	case CHARA_HIT_RCLAVICLE :
	case CHARA_HIT_BODY1 :
	case CHARA_HIT_BODY2 :
	case CHARA_HIT_BODY3 :
	case CHARA_HIT_ROOT :
		effectCount = 3;	break;
	}
	
	//	1��Ī �ڽ��� �ǰ� ������ ȭ�� �� ����Ʈ ó���մϴ�.
	if( is1PV()) 
	{
		//	�ǰ� ������ ������ �չ��� �밢�� ���� �����ϰ� ���������� �ڽ��� ������ �Ѵ�.
		VEC3D vPos;
		MATRIX Temp;
		
		i3Matrix::SetRotateAxisDeg( &Temp, i3Matrix::GetRight( g_pCamera->getCamMatrix()), -45.0f); 		
		i3Vector::Scale( &vPos, i3Matrix::GetAt( g_pCamera->getCamMatrix()), -0.5f);
		i3Vector::Add( &vPos, i3Matrix::GetPos( m_pBoneContext->getIKMatrix()), &vPos);

		//	Hit Effect
		bool bEffected = false;

		if ( pDamageInfo->_DeathType == CHARA_DEATH_BUFF)
		{
			removeCharaFlag( CHARA_FLAG_ENABLE_EFFECT);
			bEffected = true;
		}
		else if(pDamageInfo->isFireWeaponType(WEAPON_CLASS_DINO) && 
			(pDamageInfo->_DeathType == CHARA_DEATH_F) )
		{
			if( pDamageInfo->isFireWeaponNumber(WEAPON::DINO_ACID_EXPLOSION, true) || 
				pDamageInfo->isFireWeaponNumber(WEAPON::DINO_CC_ACID_EXPLOSION, true) ||
				pDamageInfo->isFireWeaponNumber(WEAPON::DINO_ACID_VULCAN_EXPLOSION, true) )
			{
				removeCharaFlag( CHARA_FLAG_ENABLE_EFFECT);
				bEffected = true;
			}
		}

		if( bEffected == false)
		{
			if( (pDamageInfo->_DeathType != CHARA_DEATH_POISON) && ( pDamageInfo->_DeathType != CHARA_DEATH_F ) )
			{
				bool  bEnable1PV = false;
				INT32 CharaHitEff = -1;
				if(pDamageInfo->_pWeapon)
				{
					CharaHitEff = pDamageInfo->_pWeapon->getCharaHitEff1PVIdx();
					if( CharaHitEff < 0 )
						CharaHitEff = pDamageInfo->_pWeapon->getCharaHitEffIdx();
					else
						bEnable1PV = true;
				}

				//�̷� ���� �ȵǴ� �ڵ带 ���� �� �ۿ� ���� �� PB�� �Ӵ�.
				//����ź�� ��� _pWeapon�� �ֹ���� �´�. _pWeaponInfo�� ����ź���� �´�. ����Ʈ ó���� _pWeapon���� �ؾ� �ȴ�.
				//�� ���� �ƹ��ų� nullptr �ϼ��� �ִ�. ����Ÿ���� ������ �´�.
				//�� �ڵ�� ���� ����ź���� Ư�� �ǰ� ����Ʈ�� ���� �� ����.
				//�ְ� ������ _pWeaponInfo�� �ǰ� ����Ʈ ������ �Űܾ� �Ǵµ� ������ �� �ٲ��. �𸣰ڴ�....
				if ((pDamageInfo->_DeathType == CHARA_DEATH_B) || (pDamageInfo->_DeathType == CHARA_DEATH_O))
				{
					bEnable1PV = false;
					CharaHitEff = -1;
				}
					
				if(CharaHitEff < 0)
				{
					BULLET_EFFECT_TYPE effectType = BULLET_EFFECT_DEFAULT;

					if( pDamageInfo->_pWeaponInfo != nullptr)
						effectType = static_cast<BULLET_EFFECT_TYPE>(pDamageInfo->_pWeaponInfo->GetLoadBulletType());

					if( IsRex() || IsDomiMutantRex())
					{
						g_pEffectManager->AddDinoHitEffect( WEAPON_CLASS_UNKNOWN, effectType, &vPos, i3Matrix::GetUp( &Temp), pDamageInfo->_rDistance);
					}
					else
					{
						g_pEffectManager->AddCharaHitEffect( WEAPON_CLASS_UNKNOWN, effectType, &vPos, i3Matrix::GetUp( &Temp), pDamageInfo->_rDistance);
					}
				}
				else
				{
					if( bEnable1PV )
						g_pEffectManager->AddEffect( CharaHitEff, &vPos, i3Matrix::GetUp( &Temp));
					else
						g_pEffectManager->AddEffect( CharaHitEff, &vPos, i3Matrix::GetUp( &Temp), pDamageInfo->_rDistance);
					
				}

				getSoundContext()->_PlayCharaHitSound( pDamageInfo, &vPos, true);

				addCharaFlag( CHARA_FLAG_ENABLE_EFFECT);
			}
			else
			{
				_PlayDownfallSound();
				removeCharaFlag( CHARA_FLAG_ENABLE_EFFECT);
			}
		}
	}
	//	3��Ī ���
	else
	{
		MATRIX mat;
		VEC3D vPos, vRand, vDir;
		VEC3D * pPos = nullptr, * pNormal = nullptr;
		REAL32 rRandFactor = 0.05f;

		//�븻�� ���ΰ�� ������ �߻��Ͽ� �ʱ���� - ������
		vDir.x = 0.f;
		vDir.y = 0.f;
		vDir.z = 0.f;
		pNormal = &vDir;

		pPos = pDamageInfo->_pVecPos;

		if( pDamageInfo->_pHitResult != nullptr)
		{
			pPos = &pDamageInfo->_pHitResult->m_Intersect;
			pNormal = &pDamageInfo->_pHitResult->m_Normal;
		}
		else
		{
			if( !pDamageInfo->isFireWeaponType(WEAPON_CLASS_THROWING_GRENADE) &&
				!pDamageInfo->isFireWeaponType((WEAPON_CLASS_TYPE)(WEAPON_CLASS_NPC_ARROW)) &&
				!pDamageInfo->isFireWeaponType(WEAPON_CLASS_GRENADESHELL) )
			{
				if( (pDamageInfo->GetHitPart() != CHARA_HIT_UNKNOWN) &&
					(pDamageInfo->GetHitPart() != CHARA_HIT_DEATH_TRAMPLE) &&
					(pDamageInfo->GetHitPart() != CHARA_HIT_DEATH_HOWL))
				{
					if(m_PhysixShapeSetInfo[ pDamageInfo->GetHitPart()]._pShapeSet == nullptr)
					{
						// 2011.10.12 �̰�찡 live���� �߻��Ͽ� �ϴ� ó������ �ʵ��� �Ѵ�.swoongoo.jung
						return false;
						//I3PRINTLOG(I3LOG_FATAL,  "_pShapeSet is nullptr. _HitPart %d.", pDamageInfo->GetHitPart());
					}
					m_PhysixShapeSetInfo[ pDamageInfo->GetHitPart()]._pShapeSet->getShapeGlobalPose( &mat);
					pPos = i3Matrix::GetPos( &mat);
				}

				pNormal = &vDir;
				i3Vector::Scale( pNormal, pDamageInfo->_pVecDir, -1.f);
			}
		}
		
		if( pDamageInfo->isFireWeaponType(WEAPON_CLASS_THROWING_GRENADE) ||
			pDamageInfo->isFireWeaponType(WEAPON_CLASS_GRENADESHELL))
		{	// ����ź ���� ó��
			pPos = i3Matrix::GetPos( getBoneContext()->getBoneMatrix( BONE_NECK));
			effectCount = 3;
			rRandFactor = 0.5f;
		}


		if ( pDamageInfo->_DeathType == CHARA_DEATH_BUFF)
		{
			removeCharaFlag( CHARA_FLAG_ENABLE_EFFECT);
		}
		else if( pDamageInfo->isFireWeaponType(WEAPON_CLASS_DINO) &&
			pDamageInfo->isFireWeaponNumber(WEAPON::DINO_ACID_EXPLOSION, true) &&
			(pDamageInfo->_DeathType == CHARA_DEATH_F) )
		{
			removeCharaFlag( CHARA_FLAG_ENABLE_EFFECT);
		}
		else if( (pDamageInfo->_DeathType != CHARA_DEATH_POISON) && (pDamageInfo->_DeathType != CHARA_DEATH_F) )
		{
			//
			INT32 CharaHitEff = -1;
			if(pDamageInfo->_pWeapon)
				CharaHitEff = pDamageInfo->_pWeapon->getCharaHitEffIdx();

			//�ְ� ������ _pWeaponInfo�� �ǰ� ����Ʈ ������ �Űܾ� �Ǵµ� ������ �� �ٲ��. �𸣰ڴ�.... ���� �Ȱ��� ������.
			if ((pDamageInfo->_DeathType == CHARA_DEATH_B) || (pDamageInfo->_DeathType == CHARA_DEATH_O))
			{
				CharaHitEff = -1;
			}

			if(CharaHitEff < 0)
			{
				BULLET_EFFECT_TYPE effectType = BULLET_EFFECT_DEFAULT;
				if( pDamageInfo->_pWeaponInfo != nullptr)
					effectType = static_cast<BULLET_EFFECT_TYPE>(pDamageInfo->_pWeaponInfo->GetLoadBulletType());

				for( INT32 i = 0; i < effectCount; i++ )
				{
					i3Vector::Add( &vPos, pPos, &vRand);

					if( IsRex() || IsDomiMutantRex())
						g_pEffectManager->AddDinoHitEffect( pDamageInfo->GetFireWeaponClass(), effectType, &vPos, pNormal, pDamageInfo->_rDistance);
					else
						g_pEffectManager->AddCharaHitEffect( pDamageInfo->GetFireWeaponClass(), effectType, &vPos, pNormal, pDamageInfo->_rDistance);

					i3Vector::Set( &vRand, i3Math::Randf2() * rRandFactor, i3Math::Randf2() * rRandFactor, i3Math::Randf2() * rRandFactor);
				}
			}
			else
			{
				for( INT32 i = 0; i < effectCount; i++ )
				{
					i3Vector::Add( &vPos, pPos, &vRand);

					g_pEffectManager->AddEffect( CharaHitEff, &vPos, pNormal);

					i3Vector::Set( &vRand, i3Math::Randf2() * rRandFactor, i3Math::Randf2() * rRandFactor, i3Math::Randf2() * rRandFactor);
				}
			}

			bool bListener = false;
			
			CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx);
			if( pChara) bListener = pChara->getCharaInfo()->GetType() == CHARA::TYPE_PLAYER;
			
			if( IsDomiDino() && i3::same_of<WeaponSentryGun*>(pDamageInfo->_pWeapon) )
			{	// ��Ʈ���� ������ 3��Ī ����� ����� �Ѵ�.
				bListener = false;
			}

			getSoundContext()->_PlayCharaHitSound( pDamageInfo, pPos, bListener);

			addCharaFlag( CHARA_FLAG_ENABLE_EFFECT);
		}
		else
		{
			_PlayDownfallSound();

			removeCharaFlag( CHARA_FLAG_ENABLE_EFFECT);
		}
	}

	// Wall effect
	if( g_pEnvSet->m_bEnableBlood && (pDamageInfo->_DeathType != CHARA_DEATH_POISON) )
	{
		if( pDamageInfo->_rDamage < I3_EPS )
			pDamageInfo->_rDamage = I3_EPS;

		REAL32	rHeight = 1.f /(REAL32)pDamageInfo->_rDamage;
		BLOOD_EFFECT_TYPE effectType = BLOOD_EFFECT_ASSAULT;

		switch( pDamageInfo->GetFireWeaponClass() )
		{
		case WEAPON_SUBCLASS_ASSAULT		:
		case WEAPON_CLASS_ASSAULT			:	effectType = BLOOD_EFFECT_ASSAULT;	rHeight *= 21.f;	break;
		case WEAPON_SUBCLASS_BOW			:
		case WEAPON_CLASS_BOW				:	effectType = BLOOD_EFFECT_ASSAULT;	rHeight *= 21.f;	break;
		case WEAPON_SUBCLASS_SMG			:
		case WEAPON_CLASS_SMG				:	effectType = BLOOD_EFFECT_SMG;		rHeight *= 25.f;	break;
		case WEAPON_CLASS_HANDGUN			:	effectType = BLOOD_EFFECT_HANDGUN;	rHeight *= 20.f;	break;
		case WEAPON_SUBCLASS_SHOTGUN		:
		case WEAPON_CLASS_SHOTGUN			:	
		case WEAPON_CLASS_DUALSHOTGUN		:	effectType = BLOOD_EFFECT_SHOTGUN;	rHeight *= 15.f;	break;
		case WEAPON_CLASS_KNIFE				:	effectType = BLOOD_EFFECT_KNIFE;	rHeight *= 80.f;	break;
		case WEAPON_CLASS_KNUCKLE			:	effectType = BLOOD_EFFECT_KNIFE;	rHeight *= 80.f;	break;
		case WEAPON_CLASS_GRENADESHELL		:
		case WEAPON_CLASS_THROWING_GRENADE	:	effectType = BLOOD_EFFECT_GRENADE;	rHeight *= 70.f;	break;
		case WEAPON_SUBCLASS_SNIPER			:
		case WEAPON_CLASS_SNIPER			:	effectType = BLOOD_EFFECT_RIFLE;	rHeight *= 50.f;	break;
		case WEAPON_CLASS_MG				:		effectType = BLOOD_EFFECT_ASSAULT;	rHeight *= 20.f;	break;
		case WEAPON_CLASS_CIC				:
			{
				if( pDamageInfo->_FireOrder == WEAPON::PRIMARY_FIRE)
				{
					effectType = BLOOD_EFFECT_HANDGUN;
					rHeight *= 20.f;
				}
				else
				{
					effectType = BLOOD_EFFECT_KNIFE;
					rHeight *= 80.f;
				}
			}
			break;
		case WEAPON_CLASS_DUALHANDGUN		:	effectType = BLOOD_EFFECT_HANDGUN;	rHeight *= 20.f;	break;
		case WEAPON_CLASS_DUALKNIFE			:	effectType = BLOOD_EFFECT_KNIFE;	rHeight *= 80.f;	break;
		case WEAPON_CLASS_OBJECT_GUN		:	effectType = BLOOD_EFFECT_ASSAULT;	rHeight *= 21.f;	break;
		default								:	rHeight *= 50.f;	break;
		}

		if( pDamageInfo->_pWeaponInfo != nullptr
			&& pDamageInfo->_pWeaponInfo->GetLoadBulletType() == 1)
			effectType = BLOOD_EFFECT_WATER;


		if (pDamageInfo->_DeathType == CHARA_DEATH_BUFF)
		{
			i3CollideeLine Line;

			{
				VEC3D	vDir;
			
				if (pDamageInfo->_pVecDir != nullptr)
				{
					i3Vector::Set(&vDir, getX(pDamageInfo->_pVecDir), -getY(pDamageInfo->_pVecDir) - rHeight, getZ(pDamageInfo->_pVecDir));
				}
				else
				{
					i3Vector::Scale(&vDir, i3Matrix::GetAt(GetMatrix()), -1.f);
				}

				Line.SetStart(i3Matrix::GetPos(m_pBoneContext->getHeadMatrix()));
				Line.SetDir(&vDir);
			
				pResult = g_pFramework->raycastClosest(&Line, CGRP_MASKGROUP_ATTACK);
			}

			if( pResult != nullptr )
			{
				VEC3D vDir;

				I3ASSERT( pResult->m_pDestShapeSet != nullptr);
				
				i3Vector::Sub( &vDir, &pResult->m_Intersect, Line.GetStart() );
				i3Vector::Normalize( &vDir, &vDir );

				g_pEffectManager->AddDinoBuffEffect(&pResult->m_Intersect, &pResult->m_Normal  );
			}

		}
		// ���鿡 �Ǹ� ���Դϴ�.
		else if( pDamageInfo->_pWallHitResult != nullptr )
		{
			pResult = pDamageInfo->_pWallHitResult;
			if( pResult->m_pTerrain != nullptr )
			{
				i3Object * pObj = nullptr;
				I3ASSERT( pResult->m_pDestShapeSet != nullptr);
				if( pResult->m_pDestShapeSet->getEventReceiver() && 
					i3::kind_of<i3Object*>(pResult->m_pDestShapeSet->getEventReceiver()))
				{
					pObj = (i3Object*) pResult->m_pDestShapeSet->getEventReceiver();
				}

				if( effectType == BLOOD_EFFECT_KNIFE )
					g_pEffectManager->AddBloodEffect( BLOOD_TERRAIN_FLOOR, effectType, &pResult->m_Intersect, &pResult->m_Normal, pDamageInfo->_pVecDir, 0.f, pObj );
				else
					g_pEffectManager->AddBloodEffect( BLOOD_TERRAIN_WALL, effectType, &pResult->m_Intersect, &pResult->m_Normal, pDamageInfo->_pVecDir, 0.f, pObj );
			}
		}
		else
		{
			// �ٴڿ� �Ǹ� ���Դϴ�.
			VEC3D	vDir, vRand;
			i3CollideeLine Line;

			if( pDamageInfo->_pVecDir != nullptr)
			{
				if( pDamageInfo->isDamageTypeMelee() )
				{	// ���������� �������� �ǰ� Ƣ���� �մϴ�.
					i3Vector::Set( &vDir, -getX( pDamageInfo->_pVecDir ), -getY( pDamageInfo->_pVecDir ) - rHeight, -getZ( pDamageInfo->_pVecDir ) );
				}
				else
				{
					i3Vector::Set( &vDir, getX( pDamageInfo->_pVecDir ), -getY( pDamageInfo->_pVecDir ) - rHeight, getZ( pDamageInfo->_pVecDir ) );
				}
			}
			else
			{
				i3Vector::Scale( &vDir, i3Matrix::GetAt( GetMatrix()), -1.f);
			}

			if( pDamageInfo->_pHitResult != nullptr )	Line.SetStart( &pDamageInfo->_pHitResult->m_Intersect );
			else									Line.SetStart( i3Matrix::GetPos( m_pBoneContext->getHeadMatrix() ) );

			i3Vector::Set( &vRand, (REAL32)((i3Math::Rand() % 100)-50)*0.001f, (REAL32)((i3Math::Rand() % 100)-50)*0.001f, (REAL32)((i3Math::Rand() % 100)-50)*0.001f );
			i3Vector::Add( &vDir, &vDir, &vRand );
			Line.SetDir( &vDir );

			pResult = g_pFramework->raycastClosest( &Line, CGRP_MASKGROUP_ATTACK );
			if( pResult != nullptr )
			{
				i3Object * pObj = nullptr;
				// 5���� �ȿ� �ִ� ��쿡�� ���Դϴ�.
				if( (pResult->m_pTerrain != nullptr) && (pResult->m_T < 5.f) )
				{
					REAL32	rRotate = 0.f;
					CGameCharaBase * pChara = nullptr;

					I3ASSERT( pResult->m_pDestShapeSet != nullptr);

					if( !pDamageInfo->_IsNPC )
					{
						CHARA::BUFFER_TYPE eBufferType = CHARA::Type2BufferType( pDamageInfo->_nKillerType);
						pChara = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx, (eBufferType == CHARA::BUFFER_NEW_AI));
					}
					else
					{
						I3PRINTLOG(I3LOG_FATAL, "pDamageInfo->_IsNPC is true. must be false.");
					}
					
					if( pChara != nullptr )
					{// �ش� ĳ���Ͱ� ���� �������� ������ ������Ʈ�ϵ��� �մϴ�.
						rRotate = -pChara->getBoneContext()->getTheta();
					}

					if( pResult->m_pDestShapeSet->getEventReceiver() &&
						i3::kind_of<i3Object*>(pResult->m_pDestShapeSet->getEventReceiver()))
					{
						pObj = (i3Object*) pResult->m_pDestShapeSet->getEventReceiver();
					}

					VEC3D vDir2;
					i3Vector::Sub( &vDir2, &pResult->m_Intersect, Line.GetStart() );
					i3Vector::Normalize( &vDir2, &vDir2 );
					 
					g_pEffectManager->AddBloodEffect( BLOOD_TERRAIN_FLOOR, effectType, &pResult->m_Intersect, &pResult->m_Normal, &vDir2, rRotate, pObj );
				}
			}
		}
	}

	return false;
}

bool CGameCharaBase::Cmd_SetClanTexture( INT32 idx)
{	
	if( idx < 0 || idx >= g_pCharaManager->GetCharaClanTexCount() )		return false;

	i3Texture * pTex = (i3Texture *) g_pCharaManager->QueryCharaClanTex( idx);	
	I3ASSERT( pTex != nullptr);

	//	3PV �ؽ�ó ����
	getMaterialContext()->SetClanMark( pTex);

	return true;
}

bool CGameCharaBase::Cmd_UseDomiSkill()
{
#if defined( DEF_OBSERVER_MODE)
	// Observer�� ó������ �ʴ´�.
	if( BattleSlotContext::i()->isObserverMe())
		return false;
#endif
	if( !isLocal())
		return false;

	// ���� ���� ������ ����Ѵ�.
	if( isCharaStateMask( CHARA_STATE_JUMP))
	{
		return false;
	}

	CDominationSkillSystem::i()->OnButtonClicked(this);
	return true;
}

bool CGameCharaBase::Cmd_CancelDomiSkill()
{
#if defined( DEF_OBSERVER_MODE)
	// Observer�� ó������ �ʴ´�.
	if( BattleSlotContext::i()->isObserverMe())
		return false;
#endif
	if( !isLocal())
		return false;

	if( isCharaStateMask( CHARA_STATE_DISARMINGBOMB))
	{
		//CDominationSkillSystem::i()->CancelSkill();

		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
		removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);

		return true; 
	}

	removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
	removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);

	return false;
}

bool CGameCharaBase::Cmd_Uninstall( bool bEnable /*= true*/)
{
#if defined( DEF_OBSERVER_MODE)
	// Observer�� ó������ �ʴ´�.
	if( BattleSlotContext::i()->isObserverMe())
		return false;
#endif

	if( !isLocal())
		return false;

	if( bEnable)
	{
		CGameMissionManager * pMissionMng = CGameMissionManager::i(); ;
		I3ASSERT( pMissionMng != nullptr);

		if(pMissionMng->IsPossibleDisarmBomb() == false)
		{
			return false;
		}
		

		// ���� ���� ������ ����Ѵ�.
		if( isCharaStateMask( CHARA_STATE_JUMP))
		{
			CGameMissionManager::i()->SendMissionUninstall( false );
			return false;
		}

		if( pMissionMng->IsEnableMy() == false)
		{
			if(isCharaStateMask(CHARA_STATE_DISARMINGBOMB))
			{
				// revision 55571 C4 ��ü�� ���� �������� ������ ���ÿ� ������ ���, ������ ������ �ʰ� ��� �����Ǵ� ���� ����(hansoft. 4662)
				//2013. 3. 21 �ڱ⼺
				//���� ���� �ٱ����� ������ �� ��ź ���� ���̾��ٸ� ������ش�.
				CGameMissionManager::i()->SendMissionUninstall( false );		//	��ź ��ü ���

				removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
				removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);

				WeaponBase * pTempWeapon = nullptr;
				pTempWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));

				if( pTempWeapon != nullptr)
				{
					I3ASSERT( i3::same_of<WeaponC4*>(pTempWeapon));
					WeaponC4 * pBomb = ( WeaponC4*)pTempWeapon;

					pBomb->ActiveDisarm( false);
				}
				//I3TRACE("-- ��ź ��ü ���!!\n");
				return true;
			}
			else
			{
				return false;		
			}
		}
		
		//	��ź ��ü	
		if( (getActionContext()->isLower( CHARA_LOWER_IDLE) || getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK)) &&
			(getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE)))
		{	
			if( pMissionMng->IsEnableCrossHit() )
			{
				WeaponBase * pWeapon = nullptr;

				if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
				{
				#if defined( USE_EVENT_SHUTTLEX)
					pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SHUTTLEX));
				#else
					pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
				#endif

					if( pWeapon != nullptr)
					{
						I3ASSERT( i3::same_of<WeaponC4*>(pWeapon));
						WeaponC4 * pBomb = (WeaponC4 *) pWeapon;

						addCharaStateMask( CHARA_STATE_DISARMINGBOMB);

						pBomb->ActiveDisarm( true);
					}
					else
					{
						removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);
						removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
					}
				}
			}
			else
			{
				removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);
				removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
			}
		}

		if( isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
		{	// ��ü��
			//	�̵� �Ұ�. 
			i3Vector::Zero( getMoveContext()->getMoveDir());

			getActionContext()->_Play_Lower_Idle();

			// ��ü�� ������ �ɴ´�.
			getActionContext()->_Play_Crouch();

			if( getActionContext()->isUpper( CHARA_UPPER_MOVE))
				getActionContext()->_Play_Upper_Idle();

			addEnableStateMask( CHARA_STATE_DISABLE_SHOT |
				CHARA_STATE_DISABLE_MOVE |
				CHARA_STATE_DISABLE_CROUCH );

			CGameMissionManager::i()->SendMissionUninstall( true );		//	��ź ��ü

			return true;
		}

		CGameMissionManager::i()->SendMissionUninstall( false );
		return false;
	}
	else
	{
		WeaponBase * pWeapon = nullptr;
		INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();

		CGameMissionManager * pMissionMng = CGameMissionManager::i();
		I3ASSERT( pMissionMng != nullptr);
		
		if( pMissionMng->IsPossibleDisarmBomb() == false )
			return false;

		if( isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
		{
			//	�ڽ��� ��ź ���� ���̸� ���
			if( pMissionMng->IsUninstalling( mySlotIdx) )
			{			
				CGameMissionManager::i()->SendMissionUninstall( false );		//	��ź ��ü ���

				removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
				removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);

	#if defined( USE_EVENT_SHUTTLEX)
				pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex( WEAPON::SHUTTLEX));
	#else
				pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
	#endif

				if( pWeapon != nullptr)
				{
					I3ASSERT( i3::same_of<WeaponC4*>(pWeapon));
					WeaponC4 * pBomb = ( WeaponC4*)pWeapon;

					pBomb->ActiveDisarm( false);
				}

				//I3TRACE("-- ��ź ��ü ���!!\n");

				return true;
			}
		}
	}
	return false;
}

void CGameCharaBase::Cmd_Walk( VEC3D * pDir)
{
	if( m_rWalkParalysis > 0)	// revision 53433 HK417 ������� �߰�
		return;

	getActionContext()->_Play_Lower_Walk( pDir);

	if( getActionContext()->isUpper( CHARA_UPPER_IDLE) ||
		getActionContext()->isUpper( CHARA_UPPER_JUMP) ||
		getActionContext()->isUpper( CHARA_UPPER_MOVE) )
	{
		if (IsRaptorBase())
		{
			getActionContext()->_Play_Upper_Walk();
		}
		else
		getActionContext()->_Play_Upper_Run();
	}
}

void CGameCharaBase::Cmd_WalkLower( VEC3D * pDir)
{
	getActionContext()->_Play_Lower_Walk( pDir);
}






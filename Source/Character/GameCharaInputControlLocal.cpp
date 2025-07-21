#include "pch.h"
#include "GameCharaInputControlLocal.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "GameCharaDino.h"
#include "GameEvent.H"
#include "GameStateMgr.h"
#include "GameMode.h"
#include "BattleHud.h"
#include "BattleHUD_CountDown.h"

#include "../Mode/Domination/GameMode_Domination.h"
#include "../StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "HackContext.h"
#include "UserInfoContext.h"
#include "GameMissionManager.h"

#include "UI/UIHudManager.h"
#include "UI/UIHudMultiWeapon.h"

#include "MainWeapon_DualCIC.h"
#include "GameMaster.h"
#include "CQAWeaponCMgr.h"

#include "BattleServerContext.h"
#include "BattleObjContext.h"

#define THROW_DELAY_TIME	4.0f

I3_CLASS_INSTANCE( CGameCharaInputControlLocal);//, CGameCharaInputControl);

CGameCharaInputControlLocal::CGameCharaInputControlLocal() : m_fTimeLine_Throw(THROW_DELAY_TIME)
{
}

void CGameCharaInputControlLocal::OnFirstUpdate( REAL32 rDeltatime)
{
	CGameCharaInputControl::OnFirstUpdate( rDeltatime);

	if( m_pOwner == nullptr)		return;

	ProcessInput( rDeltatime);
}

void CGameCharaInputControlLocal::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaInputControl::OnUpdate( rDeltatime);
	// Add Code here
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))
	{
		m_fTimeLine_Throw = THROW_DELAY_TIME;
		m_bThrowBomb = false;
	}
}

void CGameCharaInputControlLocal::OnLastUpdate( REAL32 rDeltatime)
{
	CGameCharaInputControl::OnLastUpdate( rDeltatime);
}

void CGameCharaInputControlLocal::SetMoveDir( VEC3D * pVecOut)
{
	UINT64 keyPress = getInputContext()->getKeyPressInput();

	if( keyPress & GAME_KEY_MOVE_FORWARD )			setZ( pVecOut, 1.f);
	else if( keyPress & GAME_KEY_MOVE_BACKWARD)		setZ( pVecOut, -1.f);
	else											setZ( pVecOut, 0.f);

	if( keyPress & GAME_KEY_MOVE_LEFT )				setX( pVecOut, 1.f);
	else if( keyPress & GAME_KEY_MOVE_RIGHT )		setX( pVecOut, -1.f);
	else											setX( pVecOut, 0.f);
}

void CGameCharaInputControlLocal::ProcessDropWeapon( REAL32 rDeltatime)
{
	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	if(m_pOwner->IsDino())
		return;

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
		MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) ) 
		return;

	if( keyStroke & GAME_KEY_ACTION_DROP )
	{
		if ( m_pOwner->getMultiWeaponContext()->IsDropProhibit() && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			return;

		if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) && !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_DROPWEAPON))
		{
			WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
			if( pWeapon != nullptr)
			{
				if ( HUD::instance()->IsEnableHud(HUD::MULTIWEAPON) == true && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
				{
					UIHudMultiWeapon * pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);
					pHud->SetEnable( false);
				}

				GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, getWeaponContext()->getCurrentSlotNum(), pWeapon->getLoadedBulletCount(),
					pWeapon->getLoadedBulletDualCount(), pWeapon->getTotalBulletCount());
			}
			
			// ���⸦ ������ 0.5�ʰ� �߻縦 ���ϵ��� �����մϴ�.
			m_pOwner->setDropWeaponTimer( 0.5f);
		}
	}
}

bool CGameCharaInputControlLocal::ProcessCrouch( REAL32 rDeltatime)
{
	bool bChangeBody = false;
	UINT64 keyPress = getInputContext()->getKeyPressInput();

	if( keyPress & GAME_KEY_ACTION_CROUCH  )
	{	
		// �ɱ�
		bChangeBody = m_pOwner->Cmd_Crouch();
	}
	else
	{	// �Ͼ��
		bChangeBody = m_pOwner->Cmd_StandUp();
	}


	return bChangeBody;
}

// -------------------------------------  �̵�  ----------------------------------------------------
void CGameCharaInputControlLocal::ProcessMovement( REAL32 rDeltatime, bool bChangeBody)
{	
	VEC3D vOldDir;

	UINT64 keyPress = getInputContext()->getKeyPressInput();

	CGameCharaActionContext * pActionCtx = getActionContext();
	CGameCharaMoveContext * pMoveCtx = getMoveContext();

	VEC3D * pMoveDir = pMoveCtx->getMoveDir();

	bool bHaveUpperPlay = false;
	CHARA_ACTION_UPPER nActionUpper = pActionCtx->getUpper();
	if( (nActionUpper == CHARA_UPPER_IDLE) || (nActionUpper == CHARA_UPPER_MOVE) )
		bHaveUpperPlay = true;

	if(m_pOwner->IsDino() == false)
	{
		WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
		if( pWeapon != nullptr)
		{
			if( pWeapon->isDualGun() ||
				( i3::same_of<WeaponKrissSuperV*>(pWeapon)
				&& ((WeaponKrissSuperV*)pWeapon)->isUsedDualWeapon()) )
			{
				CHARA_ACTION_UPPER nActionUpper_Left = pActionCtx->getUpperLeft();
				if( (nActionUpper_Left == CHARA_UPPER_IDLE) || (nActionUpper_Left == CHARA_UPPER_MOVE) )
					bHaveUpperPlay = true;
			}
		}
	}

	bool bIsHackUser = HackContext::i()->isHackUserFrezen( BattleSlotContext::i()->getMySlotIdx());

	bool jump_move_enable = false;
	jump_move_enable = MyRoomInfoContext::i()->getStage()->IsSpace();
	
	if( jump_move_enable ||												//  ���� �� �̵��� �����ϸ�?
		(!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_MOVE) &&		//	�̵��� �����ϸ�?
		!pActionCtx->isLower( CHARA_LOWER_JUMP)  &&						//	�������� �ƴϸ�?
		!bIsHackUser) )													//  ġ�� ���� �ƴϸ�
	{
		if( (keyPress & GAME_KEY_MOVE_LEFT) || (keyPress & GAME_KEY_MOVE_RIGHT)
			|| (keyPress & GAME_KEY_MOVE_FORWARD) || (keyPress & GAME_KEY_MOVE_BACKWARD))
		{
			i3Vector::Copy( &vOldDir, pMoveDir);

			SetMoveDir( pMoveDir);	//	�Է�Ű�� ���� �̵� ���� ����

			REAL32 rSpeed = pMoveCtx->getSpeed();
			REAL32 rDist = i3Vector::Length( pMoveCtx->getVelocity());
			REAL32 rRet = rDist - rSpeed;

			if( m_pOwner->is1PV())
			{
				//	���� Ű���� �ٽ� �����ϴ� ���� ���´� (�Է� ������ �ٲ��)
				if( (vOldDir.x != getX( pMoveDir)) || (vOldDir.y != getY( pMoveDir)) ||	(vOldDir.z != getZ( pMoveDir)) ||
					(i3Math::abs( rRet) > 0.05f) )
				{
					if( keyPress & GAME_KEY_ACTION_FOOTSTEP )
					{	if (!m_pOwner->IsTank())
						m_pOwner->Cmd_Walk( pMoveDir);//	�ȱ�
						else
						m_pOwner->Cmd_Run( pMoveDir);	//	�ٱ�

					}
					else
						m_pOwner->Cmd_Run( pMoveDir);	//	�ٱ�

					/*if( bHaveUpperPlay)
						pActionCtx->_Play_Upper_Run();*/
				}
				else
				{	//	�ٴ� �߿� �ȱ⸦ ������
					if( pActionCtx->isLower( CHARA_LOWER_RUN))
					{
						if( keyPress & GAME_KEY_ACTION_FOOTSTEP )
						{	
							
							if (!m_pOwner->IsTank())
							m_pOwner->Cmd_Walk( pMoveDir);		//	�ȱ�� ��ȯ
							else
							m_pOwner->Cmd_Run( pMoveDir);

							/*if( !bChangeBody && bHaveUpperPlay )
								pActionCtx->_Play_Upper_Run();*/
						}
					}
					else if( pActionCtx->isLower( CHARA_LOWER_WALK))
					{
						if( (keyPress & GAME_KEY_ACTION_FOOTSTEP) == 0 )
						{	
							m_pOwner->Cmd_Run( pMoveDir);		//	�ٱ�� ��ȯ

							/*if( !bChangeBody && bHaveUpperPlay )
								pActionCtx->_Play_Upper_Run();*/
						}
					}

					/*if( bChangeBody && bHaveUpperPlay)
						pActionCtx->_Play_Upper_Run();*/
				}
			}
			else
			{
				if( keyPress & GAME_KEY_ACTION_FOOTSTEP )
				{
					if (!m_pOwner->IsTank())
					{
						m_pOwner->Cmd_Walk( pMoveDir);	//	�ȱ�
					}
					else
						m_pOwner->Cmd_Run( pMoveDir);	//	�ٱ�
				}
				else
					m_pOwner->Cmd_Run( pMoveDir);	//	�ٱ�

				/*if( bHaveUpperPlay)
					pActionCtx->_Play_Upper_Run();*/
			}
		}	
	}

	///////////////////////////////////////////////////////////		����
	if( !(keyPress & GAME_KEY_MOVE_LEFT) && 
		!(keyPress & GAME_KEY_MOVE_RIGHT) && 
		!(keyPress & GAME_KEY_MOVE_FORWARD) && 
		!(keyPress & GAME_KEY_MOVE_BACKWARD) || 
		m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_MOVE) )
	{	
		//	�̵� �߿� ����
		if( pActionCtx->isLower( CHARA_LOWER_RUN) || pActionCtx->isLower( CHARA_LOWER_WALK))
		{
			i3Vector::Zero( pMoveDir);

			m_pOwner->Cmd_Idle_Lower();
			m_pOwner->Cmd_Idle_Upper();
		}
		else	// revision 29547 ��� �ڵ�� �̵� ����� ������ �ִϸ��̼� ���� ����
		{
			m_pOwner->Cmd_Idle_Upper_Move();
		}
	}
}

void CGameCharaInputControlLocal::ProcessDinoAttack( REAL32 rDeltatime)
{

	//�ɾ��ִ°�� ���� ó���� ���� �ʽ��ϴ�.
	if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
	{	
		getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
	}


	bool bCancelMission = false;	//	�̼� �ൿ ��� ����
	bool bCancelBooster = false;	//  ���� �Է� ��� ���� // revision 60267 (Hansoft 5252) ���� ���� ���� ��ư Ŭ�� ���¿��� ų�ϸ� ų��ũ�� ���� �̹����� ������ ���� ����

	UINT64 keyPress = getInputContext()->getKeyPressInput();
	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->SetNowAttackFirstFrame(false);

	bool bIsHackUser = HackContext::i()->isHackUserFrezen( BattleSlotContext::i()->getMySlotIdx());

	if( (keyPress & GAME_KEY_ACTION_ATTACK) && !bIsHackUser )
	{
		// revision 60267, 63436 (���þ� Hansoft 330) Ƽ��� �Ͽ︵ ��ư ����ä�� ���� ���� �� �Ͽ︵ ���� �������� ���� ������ ���� ����
		if (m_pOwner->IsCCRaptor() || m_pOwner->IsRaptor() || m_pOwner->IsRex())
		{
			bCancelBooster = true;
		}

		if( (m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) == false) && (m_pOwner->isDeathblowState() == false))
		{
			if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
			{	
				getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
			}

			((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->DinoAttack(WEAPON::PRIMARY_FIRE);
		}
	}
	// revision 60267
	// �߻� Ŭ���� ��ư�� ���� (Key Stroke ����)
	if( (keyPress & GAME_KEY_ACTION_ATTACK) == 0)
	{
		bCancelBooster = false;
	}

	////////////////////////////////////////////////////////////	���� ����	
	if( (keyPress & GAME_KEY_ACTION_SUBFUNC) && !bIsHackUser && !bCancelBooster)	//	<- Pressed ��
	{
		if((m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) == false ) && (m_pOwner->isDeathblowState() == false))
		{
			((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->DinoAttack(WEAPON::SECONDARY_FIRE);
		}
	}

	////////////////////////////////////////////////////////////	���� ��ü	
	CHud * pHud = CHud::i();
	
	if( pHud != nullptr)
	{
		if( !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE) && !pHud->IsRadioChat() )
		{
			if(! g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT ))
			{
				if( keyStroke & GAME_KEY_CHANGEWEAPON1 )
				{
					bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY);
				}
			}
		}
	}
}

#if defined( FIREDELAY_COST) && defined( I3_DEBUG)
static bool s_bFiring = false;
static REAL32 s_rFireTime = 0.f;
#endif

void CGameCharaInputControlLocal::ProcessWeapon( REAL32 rDeltatime)
{	
	if(m_pOwner->IsDino())
	{
		return;
	}

	WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pCurWeapon == nullptr)	return;
	WeaponBase * pFireWeapon = pCurWeapon->getUsedWeapon();
	I3ASSERT( pFireWeapon != nullptr);
	CWeaponInfo * pCurWeaponInfo = pFireWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	WEAPON::eSTATE	weaponState = pFireWeapon->getWeaponState();

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	
	//������ �� ��� bBowFireOrder�� False���ִ� �κ��� ��� ��ź�߻� ���װ� �־� ����ó���մϴ�. 
	WEAPON_CLASS_TYPE weaponClass = pFireWeapon->getWeaponInfo()->GetTypeClass();
	if( weaponClass != WEAPON_CLASS_BOW && weaponClass != WEAPON_SUBCLASS_BOW )
	{
		UserContext::i()->bBowFireOrder = false;
	}

	bool bCancelMission = false;	//	�̼� �ൿ ��� ����
	bool bCancelDisarm = false;		//	��ź ��ü ��� ����

	UINT64 keyPress = getInputContext()->getKeyPressInput();
	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	// ����ź ���� ������ ���ϵ��� ������ ���
	if( m_bThrowBomb ) 
		m_fTimeLine_Throw -= rDeltatime;
	if( m_fTimeLine_Throw < 0.0f) 
	{
		m_fTimeLine_Throw = THROW_DELAY_TIME;
		m_bThrowBomb = false;		
	}

	bool bIsHackUser = HackContext::i()->isHackUserFrezen( BattleSlotContext::i()->getMySlotIdx());

	BOOL bMultiWeapon = TRUE;

	if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		UIHudMultiWeapon * pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);

		I3ASSERT( pHud != nullptr);

		if ( pHud->IsVisible() && !pHud->GetAutoView())
		{
			bMultiWeapon = FALSE;
		}
	}

	if ( ( keyPress & GAME_KEY_ACTION_ATTACK) && !bIsHackUser)
	{
		UIHudMultiWeapon* pHud = nullptr;
		pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && HUD::instance()->IsEnableHud(HUD::MULTIWEAPON))
		{
			// ������ ���߹��⽽���� ���� ������ ���߹����� ���԰� �ٸ� ��� ���� ��ü
			if ( m_pOwner->getMultiWeaponContext()->GetCurrSlotInfo()->nSlotNumber != pHud->GetCurrentSlotIdx())
			{
				// HUD�� ���ø� �� �� ���� ��ü�� ���߿� ������

				// �������ⱳü���� ����ϱ� ���� ��ü �� Index�� ���� - �ٸ������� �ϰ� ó�� �������� Ȯ�� �ʿ�
				m_pOwner->getMultiWeaponContext()->SetQuickChangeIdx();
				m_pOwner->getMultiWeaponContext()->HasHUDSelectedChange();

				m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY);
			}

			pHud->SetEnableFire( FALSE);
			pHud->SetEnable( false);
		}
		else
		{
			PB_VM_S

				// �߻� ����(�ִϸ��̼Ǹ� �Ҽ� �ִ� �����)
				bool bJustFire = false;

			/*if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && pHud->GetEnableFire() == FALSE)
			{
				REAL32 fTimer = m_pOwner->getMultiWeaponTimer() + 0.1f;

				if ( m_pOwner->getMultiWeaponTimer() > 1.0f)
				{
					pHud->SetEnableFire( TRUE);
					fTimer = 0.f;
				}
				m_pOwner->setMultiWeaponTimer(fTimer);
			}*/

			if ( CQAWeaponCMgr::i()->getOnWeaponCollision())
			{
				ProcessWeaponCollisionFire(rDeltatime);
				return;
			}

			if (i3::kind_of<WeaponGrenadeLauncher*>(pFireWeapon) ||
				i3::kind_of<WeaponBow*>(pFireWeapon))
			{
				if (pFireWeapon->isExtensionActivate())
					bJustFire = true;
			}
			else if (i3::same_of<WeaponBombTrigger*>(pFireWeapon))
				bJustFire = true;
			else if (pCurWeaponInfo->GetTypeUsage() == WEAPON_SLOT_MELEE)
				bJustFire = true;

			const bool isDisableShot = m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_SHOT);
			const bool isFireState = pFireWeapon->isWeaponStateMask(WSM_FIRING);

			if ((!isDisableShot && !isFireState) &&
				(pFireWeapon->hasLoadedBullet() || bJustFire) &&
				(m_pOwner->getDropWeaponTimer() < 0.f))// &&	// ��� ���� Ÿ�̸Ӱ� �ִٸ� �߻縦 ���ϵ��� �մϴ�.
				//(m_pOwner->getMultiWeaponTimer() <= 0.f))	// ���� ���� Ÿ�̸Ӱ� �ִٸ� �߻縦 ���ϵ��� �մϴ�.
			{
				if (pFireWeapon->isFireEnabled())
				{
#if defined( FIREDELAY_COST) && defined( I3_DEBUG)
					s_bFiring = true;
#endif
					PB_VM_E

						//	Mission ���� ����ó��
						if (i3::same_of<WeaponC4*>(pFireWeapon))
						{//C4ó���ϴ� �ڵ� �Դϴ�. ������ �̻��մϴ�. 
							//	�ൿ
							if (!m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) && m_pOwner->isCharaStateMask(CHARA_STATE_FLOOR))
							{
								//��ġ�� �ϴ� �����Դϴ�. 
								if (m_pOwner->Cmd_Attack())
									bCancelMission = false;
							}
						}
						else
						{
							//	��ź ��ü ���϶� �߻� �Ұ�
							if (m_pOwner->isCharaStateMask(CHARA_STATE_DISARMINGBOMB) == false)
							{
								//	���� �߻�!!		(��ô ���⸸ �߻��غ�)
								switch (pCurWeaponInfo->GetTypeClass())
								{
								case WEAPON_CLASS_THROWING_GRENADE:
								case WEAPON_CLASS_THROWING_CONTAIN:
								case WEAPON_CLASS_THROWING_ATTACK:
								case WEAPON_CLASS_THROWING_HEAL:
									//case WEAPON_CLASS_BOW:
								{
									if ( !pFireWeapon->isWeaponStateMask( WSM_FIRING))
									{
										if ( pCurWeaponInfo->GetUseThrowDelay())
										{
											if ( m_fTimeLine_Throw >= THROW_DELAY_TIME )
											{
												bCancelDisarm = true;
												m_pOwner->Cmd_Attack( WEAPON::READY_FIRE);
											}
										}
										else
										{
											bCancelDisarm = true;
											m_pOwner->Cmd_Attack( WEAPON::READY_FIRE);
										}
									}
								}
								break;
								default:
								{
									bCancelDisarm = true;
									m_pOwner->Cmd_Attack();
								}
								break;
								}
							}
						}
				}
				// ������ �߻� �����ϸ� ���� ������ �ؾ� �ϴµ� ��¾ ������ �ʹ� �������� ��ŵ�Ǿ�
				// �̹� �������� ��ŵ���� �ν� �ּ�ó�� �մϴ�.
				//else if( m_pOwner->isCharaStateMask(CHARA_STATE_RELOAD)
				//			&& pFireWeapon->getWeaponInfo()->GetTypeTrigger() == WEAPON::TRIGGER_RELOADING_SHOT 
				//			&& pFireWeapon->getWeaponInfo()->GetLoadBulletType() != 1)
				//{	// ������ �߻� �����ϸ� ź�� ���� ������ ���Ѵ�(����)
				//	
				//	bCancelMission = m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);
				//}
			}
			else
			{
				// �ڵ����� ������ �ϴ� ���
				switch ( weaponState)
				{
				case WEAPON::STATE_NEEDTO_LOADMAG:
				{
					ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

					if( pCurWeapon->getWeaponInfo()->IsLoadMagazineReady() && pCurWeapon->isReadyToLoadMagazine() == false &&
						getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
						nLoadType = ACTION_LOAD_READY_MAG;

					// revision 59544  P99 & HAK RMB ������ (�ڵ�)���� �ȵǵ��� ����. hansoft.3655
					bool enable_loadmag = true;
					if (i3::same_of<WeaponDualCIC*>(pCurWeapon))
					{
						MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(pCurWeapon->GetMainWeapon());
						enable_loadmag = mainWeapon_DualCIC->getCurrentUseWeapon() != WEAPON_CIC_CURRENTUSE_LEFT;
					}
					m_pOwner->SetLoadMagChg(false);
					if (enable_loadmag)
						bCancelMission = m_pOwner->Cmd_Reload(nLoadType);

#if defined( FIREDELAY_COST) && defined( I3_DEBUG)
					if (s_bFiring)
					{
						I3TRACE("Fire Time : %.4f\n", s_rFireTime);
						s_rFireTime = 0.f;
						s_bFiring = false;
					}
#endif
				}
				break;
				case WEAPON::STATE_NEEDTO_LOADBULLET:
				{
					if (pCurWeapon->IsFireBulletWeapon())
					{
						bCancelMission = m_pOwner->Cmd_Reload(ACTION_LOAD_FIRE_BULLET);
					}
					else
					{
						bCancelMission = m_pOwner->Cmd_Reload(ACTION_LOAD_MAG_BULLET);
					}
				}
				break;
				case WEAPON::STATE_NEEDTO_REMOVESHELL:
				{
					bCancelMission = m_pOwner->Cmd_Reload();
				}
				break;
				}
			}
		}
	}
	else if( (keyPress & GAME_KEY_ACTION_ATTACK) == 0 )	//	�߻� Ŭ���� ��ư�� ���� (Key Stroke ����)
	{
		if ( !pFireWeapon->isWeaponStateMask(WSM_POINTSHOTING) )
		{
			//WSM_POINTSHOTING ��ƾ ������ ��� �߻簡 �Ϸ�� �� WSM_POINTSHOTING ����ũ�� ����� �ش� �κп��� �ʱ�ȭ �ǵ��� ����. 
			//�ѹ� Ŭ���ϸ� ������ ������ �߻���� �� �����ߵǹǷ�.
			//����, WSM_POINTSHOTING �� ������ ��ƾ�� �ٲ� ��� WSM_POINTSHOTING ����ũ�� ������ �Ѵ�.
			pFireWeapon->FireEnd();		//	�߻� ���� ������ ���⿡ �˷��ش�.
		}

		bCancelMission = true;

		//	��ô ������ ��� ��ư�� ������ ������.
		if (pFireWeapon->isWeaponStateMask(WSM_FIRING))
		{
			switch (pCurWeaponInfo->GetTypeClass())
			{
			case WEAPON_CLASS_THROWING_GRENADE:
			case WEAPON_CLASS_THROWING_CONTAIN:
			case WEAPON_CLASS_THROWING_ATTACK:
			case WEAPON_CLASS_THROWING_HEAL:
				//case WEAPON_CLASS_BOW:
			{
				if (getActionContext()->isUpper(CHARA_UPPER_FIRE_READY) &&
					!m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_SHOT) && pFireWeapon->getLoadedBulletCount() > 0)
				{
					if (pCurWeaponInfo->GetUseThrowDelay())
					{
						m_bThrowBomb = true;
						bCancelDisarm = m_pOwner->Cmd_Attack();
					}
					else
					{
						if (UserContext::i()->bBowFireOrder)
						{
							bCancelDisarm = m_pOwner->Cmd_Attack(WEAPON::SECONDARY_FIRE);
							UserContext::i()->bBowFireOrder = false;
						}
						else
							bCancelDisarm = m_pOwner->Cmd_Attack();
					}
				}
			}
			break;
			}
		}
	}
	
#if defined( FIREDELAY_COST) && defined( I3_DEBUG)
	if( s_bFiring )
	{
		s_rFireTime += rDeltatime;
	}
#endif

	//�Ѿ��� ���ڶ� ������ΰ�� �ݹ߽õ��� �ϰԵǸ� ���� �Ҹ�
	if (keyStroke & GAME_KEY_ACTION_ATTACK && bMultiWeapon)
	{
		if ( ( weaponState == WEAPON::STATE_EMPTY_MAGAZINE))
		{
			if ( pCurWeaponInfo->isGun())
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand() % 2, m_pOwner->GetPos(), true);
			}
		}
		else if ( pFireWeapon->isExtensionActivate() &&
			i3::same_of<WeaponBow*>(pFireWeapon) &&
			(((WeaponBow*)pFireWeapon)->getTotalShellCount() <= 0) &&
			(((WeaponBow*)pFireWeapon)->getLoadedShellCount() <= 0))
		{
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand() % 2, m_pOwner->GetPos(), true);
			m_pOwner->Cmd_Extension( false);
		}
	}

	bool bOnSubAttack = true;
	WEAPON_CLASS_TYPE weapon_class_type = pCurWeaponInfo->GetTypeClass();
	if( bIsHackUser && 
		(weapon_class_type == WEAPON_CLASS_KNIFE || weapon_class_type == WEAPON_CLASS_KNUCKLE ||
		weapon_class_type == WEAPON_CLASS_DUALKNIFE) )
	{
		bOnSubAttack = false;
	}

	////////////////////////////////////////////////////////////	���� ����	
	if( keyPress & GAME_KEY_ACTION_SUBFUNC && bOnSubAttack && bMultiWeapon)	//	<- Pressed ��
	{
		bool bEnableWeapon = false;
		bool bMustFire = false;

		switch( pCurWeaponInfo->GetTypeClass())
		{
		case WEAPON_CLASS_KNIFE:			bEnableWeapon = true;			break;
		case WEAPON_CLASS_KNUCKLE:			bEnableWeapon = true;			break;
		// KEYWORD : WEAPON_C5
		case WEAPON_CLASS_BOMBTRIGGER :
			if( pFireWeapon->getLoadedBulletCount() > 0 )	// źȯ�� �ִ� ��� ����
				bEnableWeapon = true;
			break;
		case WEAPON_CLASS_DUALKNIFE :
		case WEAPON_CLASS_CIC :
			bEnableWeapon = true;
			bMustFire = true;			// revision 32315 CIC źȯ�� 0�̾ ���� ����
			break;
		case WEAPON_CLASS_THROWING_GRENADE:
			if( i3::same_of<WeaponM14*>(pFireWeapon) == true) bEnableWeapon = true;
			break;
		case WEAPON_CLASS_DUALSHOTGUN :		bEnableWeapon = true;			break;
		//case WEAPON_CLASS_BOW :
		//	{
		//		if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) == false)
		//		{
		//			if( !pFireWeapon->isWeaponStateMask( WSM_FIRING))
		//			{
		//				{										
		//					bCancelDisarm = true;
		//					UserContext::i()->bBowFireOrder = true;
		//					
		//					m_pOwner->Cmd_Attack( WEAPON::READY_FIRE);
		//				}
		//			}
		//		}

		//	}
		//	break;

		default :
			if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_BAYONET ||
				pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_DIRECT_MELEE )
			{
				bEnableWeapon = true;
				bMustFire = true;		// ��ü���ʹ� ������ �����ϵ��� �մϴ�. źȯ�� 0�̾..
			}
			break;
		}


		if( bEnableWeapon )
		{
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) &&					//	��ź ��ü ���� �ƴϰ�
				(!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) &&				//	�߻� �Ұ��� ���°� �ƴϰ�
				(pFireWeapon->isFireEnabled() || bMustFire) && !pFireWeapon->isWeaponStateMask( WSM_FIRING)))	// ������ �߻簡 ������ ���
			{	
				if( pCurWeaponInfo->GetTypeClass() == WEAPON_CLASS_BOMBTRIGGER ) 
				{
					WeaponBase* bomb = static_cast<WeaponBombTrigger*>(pCurWeapon)->getBomb();
					if (bomb && i3::same_of<WeaponC5*>(bomb))
					{
						m_bThrowBomb = true;
						if( m_fTimeLine_Throw >= THROW_DELAY_TIME)
						{
							bCancelDisarm = m_pOwner->Cmd_Attack( WEAPON::SECONDARY_FIRE);
						}
					}
				}
				else
				{
					bCancelDisarm = m_pOwner->Cmd_Attack( WEAPON::SECONDARY_FIRE);
				}
			}
		}
		
	}

	//////////////////////////////////////////////////
	//	'B' ��ư���� ���� �ͽ��ټ� Ȱ��ȭ

	// issue : BUG_PORTABLE #5124
	// Ÿ ���⿡�� M4 ����� �����԰� ���ÿ� 'B'��ư�� �������� ���� �������ν� �ӽ÷� ���׸� ��Ĩ�ϴ�. 2011-09-26. ugeun.ji.  
	REAL32 elapsedTimeUpperAnim		= getAnimContext()->getUpperLocalTime();
	bool enableChangeWeapon			= !(m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_WEAPONCHANGE));
	bool enableChangeExtensionMode	= ((elapsedTimeUpperAnim > 0.05f) && enableChangeWeapon);

	// ������ 0�� ���� �ͽ��ټ� ����
	if( UserInfoContext::i()->IsNewRepair() && pCurWeapon->IsEnduranceZero() )
	{
		enableChangeExtensionMode	= false;
	}

	if( (keyStroke & GAME_KEY_ACTION_WEAPON_EXTENSION) && enableChangeExtensionMode && bMultiWeapon)
	{
		// �ϳ��� ���⿡ �ΰ��� Extension�� �ִٸ� ����� �����Ѵ�.
		if( pFireWeapon->isMultipleExtension())
		{
			bool bPossible = true;

			// ��ȹ�� ��û���� Ŀ���� M4�� ��ź/LSS źȯ ��� ������ ��ź/LSS ���� �ͽ��ټ� ��ȯ�� �ȵȴ�.
			if( i3::kind_of<WeaponGrenadeLauncher*>(pFireWeapon))
			{
				WeaponGrenadeLauncher * pWeaponGrenadeLauncher = (WeaponGrenadeLauncher*) pFireWeapon;

				if((pWeaponGrenadeLauncher->getTotalShellCount() <= 0) && (pWeaponGrenadeLauncher->getLoadedShellCount() <= 0))
					bPossible = false;
			}
			else if( i3::kind_of<WeaponShotGunLauncher*>(pFireWeapon))
			{
				WeaponShotGunLauncher * pWeaponShotGunLauncher = (WeaponShotGunLauncher*) pFireWeapon;

				if((pWeaponShotGunLauncher->getTotalShellCount() <= 0) && (pWeaponShotGunLauncher->getLoadedShellCount() <= 0))
					bPossible = false;
			}
			else if ( i3::same_of<WeaponCrossFire*>(pFireWeapon) )
			{
				// HK45 Dual�� ���, 'B' ��ư�� �ƴ� RMB�� Extension�� ��ȯ�ǹǷ�, ó������ �ʴ´�.
				bPossible = false;
			}
			else if ( i3::same_of<WeaponKrissSuperV*>(pFireWeapon))
			{
				WeaponKrissSuperV* kriss = static_cast<WeaponKrissSuperV*>(pFireWeapon);
				if (kriss->m_pDualWeapon)
				{
					if (kriss->m_pDualWeapon->getLoadedBulletCount() <= 0)
						bPossible = false;
				}
			}

			// ���� �����߿� ������� �ʽ��ϴ�.
			if(weaponState == WEAPON::STATE_LOADMAG)
			{
				bPossible = false;
			}

			// ���� ��, �߻���, EXT ��ȯ���϶��� EXT ��ȯ �ȵǵ��� ����. komet
			if( pFireWeapon->isWeaponStateMask( WSM_EXT_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE))
			{
			#if defined( I3_DEBUG)
				if(pFireWeapon->isWeaponStateMask( WSM_EXT_DISABLE))
				{
					I3TRACE("Ext");
				}

				if(pFireWeapon->isWeaponStateMask( WSM_LOADMAG_DISABLE))
				{
					I3TRACE("MAG");
				}

				if(pFireWeapon->isWeaponStateMask( WSM_LOADBULLET_DISABLE))
				{
					I3TRACE("Bullet");
				}
			#endif
				//if((pFireWeapon->isWeaponStateMask(WSM_LOADMAG_DISABLE ) && !(pFireWeapon->isWeaponStateMask(WSM_LOADED_BULLET ))))
					bPossible = false;
			}

			//2015-04-06 BŰ�� �ͽ��ټ��� ������ ������� �ͽ��ټ� ���°� 2�̻��� ���� ( ���������� 2���� �ƽ� )�׷��Ƿ�, ���´�. �ϳ��϶��� ó�����ϵ���...
			// ShotGun Launcher�� �׻� �ͽ��ټ��� ���� �״� �ؾ� �մϴ�. Next�� ���� ���� �������� �ٲ���� ���...
			if( pFireWeapon->GetNextExtensionType() == WEAPON::EXT_NONE && !i3::kind_of<WeaponShotGunLauncher*>(pFireWeapon))
				bPossible = false;

			if( bPossible)
			{
				//	���� Extension�� Ȱ��ȭ�Ǿ� ������ ����.
				if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM) || pFireWeapon->isExtensionActivate())						
					m_pOwner->Cmd_Extension( false);
				
				
				//	������ �ͽ��ټ��� �����Ѵ�. 
				if( i3::same_of<WeaponKrissSuperV*, WeaponSniperDualMagazine* >(pFireWeapon) == false )
				{
					// ���źâ �������� ������ ��� ���� ��Ŭ������ , b�δ� źâ��ü�� ��. ���⼭ Extention Type �ٲ��� �ʰ� ������ŵ�ϴ�.
					pFireWeapon->SwapExtensionType();
				}
							
				if( pFireWeapon->getOwner() != nullptr && pFireWeapon->getOwner()->isLocal())
				{
				//	char szString[MAX_STRING_COUNT] = "";
					i3::stack_wstring wstrString;

					if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER)
					{
						// ��ȹ�� ��û���� ��ź ���� ���� ��ȯ
						m_pOwner->Cmd_Extension( true);

						//	"��ź ������ ����Ǿ����ϴ�."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_GRENADE_LAUNCHER_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}				
					else if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_SCOPE)
					{ 
						//	"��Ʈ����Ʈ ������ ����Ǿ����ϴ�."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_DOT_SIGHT_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}
					else if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER)
					{
						// ��ȹ�� ��û���� LSS ���� ���� ��ȯ
						m_pOwner->Cmd_Extension( true);

						//	"LSS ������ ����Ǿ����ϴ�."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_LSS_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}
					else if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_ZOOM)
					{
						//	"�� ��� ������ ����Ǿ����ϴ�."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_ZOOM_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}
					else if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_BAYONET)
					{	
						m_pOwner->Cmd_Extension( true);
						//	"���� ��� ������ ����Ǿ����ϴ�."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_BAYONET_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}
					else if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_CROSSFIRE)
					{	
						m_pOwner->Cmd_Extension( true);
						//	"ũ�ν����̾� ��� ������ ����Ǿ����ϴ�."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_CROSSFIRE_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}
				}

				//I3TRACE( "EXT %s\n", ((CGunInfo*)pCurWeaponInfo)->GetStringExtensionType());
			}
		}
		
		if( i3::same_of<WeaponKrissSuperV*>(pCurWeapon))
		{
			// CGameCharaWeaponControl::OnExtActivate() ���� �ͽ��ټ� ������ �Ѿ� ������ ��� ������ �������� ������.
			if (pCurWeapon->isExtensionActivate())
			{
				pCurWeapon->ExtensionActivate( !pCurWeapon->isExtensionActivate() );
				pCurWeapon->SwapExtensionType();
			}
			else
			{
				if (pCurWeapon->getLoadedBulletDualCount() > 0)
				{
					pCurWeapon->ExtensionActivate( !pCurWeapon->isExtensionActivate() );
					//���� �ٲ�� ���� Extension(ZOOM) �����ϰ�쵵 �����Ƿ� Extension�� ����.
					m_pOwner->Cmd_Extension(false);
					pCurWeapon->SwapExtensionType();
				}
			}
		}

		if( i3::same_of<WeaponShotGunGenderWithDual*>(pCurWeapon) )
		{
			bool bFlag = true;

			if( pCurWeapon->isExtensionActivate() ||			// ���� Ȱ��ȭ���ְų�..
				pCurWeapon->getLoadedBulletDualCount() <= 0 )	// ź ���� ������ Ext ��� ��Ȱ��
			{
				bFlag = false;
			}

			pCurWeapon->ExtensionActivate( bFlag );
		}

		if( i3::same_of<WeaponSniperDualMagazine*>(pCurWeapon))
		{
			bool bPossible = true;

			if( pFireWeapon->isWeaponStateMask( WSM_EXT_DISABLE  | WSM_LOADBULLET_DISABLE) ||
				getActionContext()->isUpper(CHARA_UPPER_EXTENSION_ACTIVATE) || 
				getActionContext()->isUpper(CHARA_UPPER_EXTENSION_DEACTIVATE) )
			{
				bPossible = false;
			}

			if( bPossible)
			{
				if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM) || pFireWeapon->isExtensionActivate())						
					m_pOwner->Cmd_Extension( false);	
				else
					m_pOwner->Cmd_Extension( true);
			}
		}
	}

	////////////////////////////////////////////////////////////	���ε�
	if( keyStroke & GAME_KEY_ACTION_RELOAD)// && HUD::instance()->GetHud(HUD::MULTIWEAPON)->IsEnable() == false)
	{
		if( ( pCurWeapon->isLoadMagazineEnabled() && !(keyPress & GAME_KEY_ACTION_SPECIAL)) && bMultiWeapon)
		{
			if (i3::same_of<WeaponKrissSuperV*>(pCurWeapon))
			{
				if (!((WeaponKrissSuperV*)pCurWeapon)->isUsedDualWeapon())
					bCancelMission = m_pOwner->Cmd_Reload();
			}
			else
			{
				bool bAvailable = true;
				if (i3::same_of<WeaponDualCIC*>(pCurWeapon))
				{// revision 52253 CIC �����̰�, Į�� �ֵθ��� �ִ� ���̶�� ���ε� �Ұ���.
					MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(pCurWeapon->GetMainWeapon());

					if (mainWeapon_DualCIC->getCurrentUseWeapon() == WEAPON_CIC_CURRENTUSE_LEFT)
						bAvailable = false;
				}

				if (bAvailable)
				{
					ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

					if( pCurWeapon->getWeaponInfo()->IsLoadMagazineReady() && pCurWeapon->isReadyToLoadMagazine() == false &&
						getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
						nLoadType = ACTION_LOAD_READY_MAG;
					m_pOwner->SetLoadMagChg(false);
					bCancelMission = m_pOwner->Cmd_Reload(nLoadType);
				}
			}
		}
	}

	////////////////////////////////////////////////////////////	���� ��ü	
	bool bEnable = true;
	if( CHud::i() == nullptr || CHud::i()->IsRadioChat())
		bEnable = false;
	if( m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE))
		bEnable = false;
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
		MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
		bEnable = false;

	if( bEnable)
	{
		bool bEnableSwapWeapon = true;

		UIHudMultiWeapon* pHud = nullptr;

		if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			if ( m_pOwner->getMultiWeaponContext()->IsHUDSelectedChange() || m_pOwner->getMultiWeaponContext()->IsDropProhibit())
			{
				bEnableSwapWeapon = false;
			}

			pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);

			I3ASSERT( pHud != nullptr);
		}

		if (!g_pFramework->getKeyboard()->GetPressState(I3I_KEY_LALT))
		{
			WEAPON_SLOT_TYPE weapontype = WEAPON_SLOT_UNKNOWN;
			WEAPON_SLOT_TYPE currentweapontype = (WEAPON_SLOT_TYPE)m_pOwner->getWeaponContext()->getCurrentSlotNum();
			WEAPON_SLOT_TYPE swapweapontype = (WEAPON_SLOT_TYPE)m_pOwner->getWeaponContext()->getSwapSlotNum();

			if (keyStroke & GAME_KEY_CHANGEWEAPON1)
				weapontype = WEAPON_SLOT_PRIMARY;
			else if (keyStroke & GAME_KEY_CHANGEWEAPON2)
				weapontype = WEAPON_SLOT_SECONDARY;
			else if (keyStroke & GAME_KEY_CHANGEWEAPON3)
				weapontype = WEAPON_SLOT_MELEE;
			else if (keyStroke & GAME_KEY_CHANGEWEAPON4)
				weapontype = WEAPON_SLOT_THROWING1;
			else if (keyStroke & GAME_KEY_CHANGEWEAPON5)
				weapontype = WEAPON_SLOT_THROWING2;
			else if (keyStroke & GAME_KEY_CHANGEWEAPON6)
				weapontype = WEAPON_SLOT_MISSION;

			if ( weapontype != WEAPON_SLOT_UNKNOWN && bEnableSwapWeapon)
			{
				bool bChange = true;

				if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
				{
					bChange = false;

					// HUD ���� ���� ���
					if ( HUD::instance()->IsEnableHud(HUD::MULTIWEAPON) == TRUE)
					{
						if (weapontype == WEAPON_SLOT_PRIMARY && m_pOwner->GetCurrentCharaWeapon() != nullptr)
						{
							pHud->SetMultiWeaponSlotImage();
						}
						else if( weapontype != WEAPON_SLOT_PRIMARY)
						{
							pHud->ShowMultiWeaponSlot();
							bChange = true;
						}
					}
					else
					{
						if (weapontype == WEAPON_SLOT_PRIMARY && m_pOwner->GetCurrentCharaWeapon() != nullptr)
						{
							if ( m_pOwner->GetCurrentCharaWeapon()->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY
								&& !( m_pOwner->isLocal() && m_pOwner->getMultiWeaponContext()->IsHUDSelectedChange()))
							{
								pHud->UpdateMultiWeaponSlot();
								pHud->ShowMultiWeaponSlot();
								bCancelMission = true;
							}
							else
								bChange = true;
						}
						else if( weapontype != WEAPON_SLOT_PRIMARY)
						{
							bChange = true;
						}
					}
				}

				if ((currentweapontype != weapontype) && (swapweapontype != weapontype) && bChange
					&& !(m_pOwner->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY) == nullptr && weapontype == WEAPON_SLOT_PRIMARY) )
					bCancelMission = m_pOwner->Cmd_ChangeWeapon(weapontype);
			}

		}

		//	���� ����� ��ü
		if ( ( keyStroke & GAME_KEY_CHANGEWEAPON_QUICK) && bEnableSwapWeapon)
		{
			if ( getWeaponContext()->getOldSlotNum() != getWeaponContext()->getCurrentSlotNum() ||
				m_pOwner->getMultiWeaponContext()->GetQuickChangeIdx() != -1 && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			{
				if ( getWeaponContext()->getOldSlotNum() == WEAPON_SLOT_PRIMARY &&
					getWeaponContext()->getCurrentSlotNum() == WEAPON_SLOT_PRIMARY &&
					m_pOwner->getMultiWeaponContext()->GetMultiWeaponCount() > 1 &&
					MyRoomInfoContext::i()->getStage()->IsMultiWeapon() )
				{
					m_pOwner->getMultiWeaponContext()->HasQuickChange();
					getWeaponContext()->setForcedChange( true);
					I3TRACE("setForcedChange true\n");
				}
				else
				{
					I3TRACE("OldSlot %d, CurrentSlot %d\n", getWeaponContext()->getOldSlotNum(), getWeaponContext()->getCurrentSlotNum());
				}

				bCancelMission = m_pOwner->Cmd_ChangeWeapon(getWeaponContext()->getOldSlotNum());
			}
			else
			{
				I3TRACE("QUICK Change False - OldSlot %d, CurrentSlot %d, QuickIndex %d\n",
					getWeaponContext()->getOldSlotNum(), 
					getWeaponContext()->getCurrentSlotNum(),
					m_pOwner->getMultiWeaponContext()->GetQuickChangeIdx());
			}
		}
		//	Mouse Wheel Down (���� ���� ��ü)
		else if( ( keyStroke & GAME_KEY_CHANGEWEAPON_NEXT) && bEnableSwapWeapon)
		{
			if (pHud != nullptr && m_pOwner->GetCurrentCharaWeapon()->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY
				&& HUD::instance()->IsEnableHud(HUD::MULTIWEAPON) == TRUE)
			{
				pHud->SetMultiWeaponSlotImage(FALSE);
			}
			else
			{
				bCancelMission = m_pOwner->Cmd_ChangeWeapon(getWeaponContext()->getNextUsableWeaponSlot());
			}
		}
		//	Mouse Wheel Up (���� ���� ��ü)		
		else if( ( keyStroke & GAME_KEY_CHANGEWEAPON_PRE) && bEnableSwapWeapon)
		{
			if (pHud != nullptr && m_pOwner->GetCurrentCharaWeapon()->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY
				&& HUD::instance()->IsEnableHud(HUD::MULTIWEAPON) == TRUE)
			{
				pHud->SetMultiWeaponSlotImage();
			}
			else
			{
				bCancelMission = m_pOwner->Cmd_ChangeWeapon(getWeaponContext()->getPrevUsableWeaponSlot());
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	�� ���� Ư�� ��ư�̿ܿ��� bCancelMission�� ���õǾ� �ٸ� ���� ��ư�� �����ؼ� �ȵ˴ϴ�.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	if( keyStroke & GAME_KEY_ACTION_SPECIAL )
	{
		I3TRACE("PRESS E KEY \n");
	}
	////////////////////////////////////////////////////////////	Ư�� ��ư (��ź ��ü��)
	if( keyPress & GAME_KEY_ACTION_SPECIAL && HUD::instance()->GetHud(HUD::MULTIWEAPON)->IsVisible() == false)
	{
		if( !bCancelDisarm)
		{
			if( (pMissionMng->getMissionState() == GAME_MISSION_STATE_INSTALLED ))
			{
				m_pOwner->Cmd_Uninstall();
			}//	�̼� ����
			else if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
			{
				m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
				m_pOwner->removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);
			}
		}
	}
	else
	{	
		bCancelDisarm = true;
	} 

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY) )
	{
		if( keyPress & GAME_KEY_ACTION_SPECIAL )
		{
			if( !bCancelDisarm)
			{
				m_pOwner->Cmd_RunAwayInstall();
				ISTEST_S
				{
					BattleObjContext::i()->OnObjectMission(true);
				}
			}
		}
		else
		{	
			bCancelDisarm = true;
			m_pOwner->Cmd_RunAwayInstall(false);
			ISTEST_S
			{
				BattleObjContext::i()->OnObjectMission(false);
			}
		} 
		bCancelMission = false;
	}

	////////////////////////////////////////////////////////////////////////
	//	�̼��� �ൿ ���	(�Է� �ൿ�� ������ �޴´�.)
	if( bCancelMission)
	{
		//	�̼� ���
		m_pOwner->Cmd_Install( false);		//	�ൿ ���
	}
	
	//	��ź ��ü ���	(�Է� �ൿ�� ������ �޴´�.)	
	if( bCancelDisarm)
	{
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY) )
		{
			if( m_pOwner->isCharaStateMask( CHARA_STATE_ARMINGBOMB))		
			{
				m_pOwner->Cmd_RunAwayInstall(false);
			}
		}
		else
		{
			if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB))		
			{
				m_pOwner->Cmd_Uninstall( false);	//	��Ʈ��ũ ��ź ��ü ���
			}
		}
	}
}

void CGameCharaInputControlLocal::ProcessExtension(void)
{
	WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pCurWeapon == nullptr)
		return;

	if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		UIHudMultiWeapon * pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);

		if ( pHud->IsVisible() && !pHud->GetAutoView())
			return;
	}

	// ������ ���� ����� �ͽ��ټ� ����
	if( UserInfoContext::i()->IsNewRepair() && pCurWeapon->IsEnduranceZero() ) 
		return;

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	if( getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) || getActionContext()->isUpper(CHARA_UPPER_LOADBULLET) ||
		getActionContext()->isUpper(CHARA_UPPER_FIREBULLET) || getActionContext()->isUpper(CHARA_UPPER_FIREBULLET) || 
		getActionContext()->isUpper(CHARA_UPPER_SWAPWEAPON) || getActionContext()->isUpper(CHARA_UPPER_EXTENSION_ACTIVATE) || 
		getActionContext()->isUpper(CHARA_UPPER_EXTENSION_DEACTIVATE) )
		return;

	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	switch( pCurWeapon->GetCurrentExtensionType())
	{
	default :
		// Extension�� ���� ���� ���...
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			switch( pCurWeaponInfo->GetTypeZoom())
			{
				case WEAPON::ZOOM_SCOPE :	m_pOwner->Cmd_ToggleZoom();	break;		// ������ ��� ���
			}
		}
		break;

	case WEAPON::EXT_SCOPE :
	case WEAPON::EXT_ZOOM :					// ����
		// K2, K1
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			// ������ ��� ���
			m_pOwner->Cmd_ToggleZoom();
		}
		break;

	case WEAPON::EXT_STEADY_AIM :
		// Spectre
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			// ������ ��� ���
			m_pOwner->Cmd_ToggleZoom();
		}
		break;

		// Activate �ܰ� ���� ����ϴ� Extension �����
	case WEAPON::EXT_DUAL_MAG :		// AK47
	case WEAPON::EXT_SILENCER :		// ���� ���ÿ� Activate
	// MP5K	// MK23
		
		if ( i3::same_of<WeaponSniperDualMagazine*>(pCurWeapon))
		{

			if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
			{
				m_pOwner->Cmd_ToggleZoomForDSR();
			}
			
		}
		else
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;
	case WEAPON::EXT_DOT_SIGHT :
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			// ������ ��� ���
			m_pOwner->Cmd_ToggleZoom();
		}
		break;
	case WEAPON::EXT_GLAUNCHER:
	case WEAPON::EXT_SHOTGUN_LAUNCHER:
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{			
			bool bPossibleSubfunc = true;

			// ��ȹ�� ��û���� Ŀ���� M4�� ��� ��ź/LSS �ͽ��ټ��� 'B' ��ư���θ� �����ϹǷ� ��ź/LSS ����� ��� ���콺 ������ ��ư�� ���´�.
			if( pCurWeapon->isMultipleExtension() && pCurWeapon->isExtensionActivate() &&
				(pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER || pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER))
			{
				bPossibleSubfunc = false;
			}

			// ź���� ���� Extension ��Ȱ��ȭ �ڵ尡 ���� �߰��մϴ�.
			if (i3::kind_of<WeaponGrenadeLauncher*>(pCurWeapon))
			{
				WeaponGrenadeLauncher * pWeaponGrenadeLauncher = (WeaponGrenadeLauncher*)pCurWeapon;

				if ((pWeaponGrenadeLauncher->getTotalShellCount() <= 0) && (pWeaponGrenadeLauncher->getLoadedShellCount() <= 0))
					bPossibleSubfunc = false;
			}
			else if (i3::kind_of<WeaponShotGunLauncher*>(pCurWeapon))
			{
				WeaponShotGunLauncher * pWeaponShotGunLauncher = (WeaponShotGunLauncher*)pCurWeapon;

				if ((pWeaponShotGunLauncher->getTotalShellCount() <= 0) && (pWeaponShotGunLauncher->getLoadedShellCount() <= 0))
					bPossibleSubfunc = false;
			}
			else if (i3::kind_of<WeaponBow*>(pCurWeapon))
			{
				if (static_cast<WeaponBow*>(pCurWeapon)->getLoadedShellCount() <= 0)
					bPossibleSubfunc = false;
			}

			if( bPossibleSubfunc)
			{
				//	���� Extension Ȱ��ȭ ���
				m_pOwner->Cmd_ToggleExtensionWeapon();
			}
		}
		break;

	case  WEAPON::EXT_BAYONET :
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{			
			bool bPossibleSubfunc = true;

			if( pCurWeapon->isMultipleExtension() && pCurWeapon->isExtensionActivate() &&
				pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_BAYONET)
			{
				bPossibleSubfunc = false;
			}

			if( bPossibleSubfunc)
			{
				//	���� Extension Ȱ��ȭ ���
				m_pOwner->Cmd_ToggleExtensionWeapon();
				
			//	m_pOwner->Cmd_Attack();
			}
		}

		break;

	case	WEAPON::EXT_CROSSFIRE :
		
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;	
	}
}

void CGameCharaInputControlLocal::ProcessInput( REAL32 rDeltatime)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))
		return;

	if(CGameMaster::i()->IsPaused())
	{
		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara( i);
			if( pChara->isCharaStateMask( CHARA_STATE_DEATH) == false)
				pChara->Cmd_Idle();
		}
		return;
	}

	UINT64 keyPress;// = getInputContext()->getKeyPressInput();
	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	if( m_pOwner->isCharaStateMask( CHARA_STATE_FIRSTRESPAWN) )
	{
		// ���� ��� ���� üũ
		ProcessDropWeapon( rDeltatime);

		if( keyStroke & GAME_KEY_ACTION_TURNBACK)
		{	// �ڵ��� ���� ���
			m_pOwner->Cmd_Turn();
		}
	}
	//	Ű �Է�( �������߿��� Ű�Է��� ����)
	if(	!i3::kind_of<CGameCamera_FLY*>(g_pCamera) &&
		!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_CONTROL) &&
 		g_pFramework->getGameKeyMode() != GAMEKEY_MODE_MENU )
	{	
		keyPress = g_pFramework->getKeyPress();
		keyStroke = g_pFramework->getKeyStroke();

		REAL32 theta = getBoneContext()->getTheta();
		REAL32 rho = getBoneContext()->getRhoValue();
		
		//REAL32 fl = 0.16666666f * tm;
		//	ĳ���� ȸ��
		if( !g_pEnvSet->m_bEnableMouseInvert)
		{
			getInputContext()->setInputMoveRho( -g_pFramework->getMouseMoveDelta()->y * 0.005f);
		}
		else
		{
			getInputContext()->setInputMoveRho( g_pFramework->getMouseMoveDelta()->y * 0.005f);
		}

		rho		+= getInputContext()->getInputMoveRho();

		getInputContext()->setInputMoveTheta( -g_pFramework->getMouseMoveDelta()->x * 0.005f);
		theta		+= getInputContext()->getInputMoveTheta();

		if( g_pFramework->getMouseMoveDelta()->x != 0.0f)
		{
			// ���� �ݵ��� ȸ�� ��, ���콺�� ���ư��� ȸ�� ��带 Cancel�Ѵ�.
			//if( m_RecoilData.m_HorzState == RECOIL_HORZ_RESTORE)
				getBoneContext()->getRecoilData()->m_HorzState = RECOIL_HORZ_NA;
		}
		
		theta		= i3Math::NormalizeAngle( theta);
		rho			= i3Math::NormalizeAngle( rho);
		//	����

		REAL32 fAimingAngleMax = g_pCharaInfoDataBase->getGlobal()->GetAimingAngleMax();	//	�ִ밢
		REAL32 fAimingAngleMin = g_pCharaInfoDataBase->getGlobal()->GetAimingAngleMin();	//	�ּҰ�

		//	���� ���� (���۽� �ٶ󺸴� �ü��� ������ 0���� ����)
		if( rho > I3_DEG2RAD( fAimingAngleMax))			rho = I3_DEG2RAD(fAimingAngleMax);	
		else if( rho < I3_DEG2RAD( fAimingAngleMin))		rho = I3_DEG2RAD(fAimingAngleMin);

		getBoneContext()->setTheta( theta);
		getBoneContext()->setRho( rho);
	}
	else
	{
		keyPress = keyStroke = 0;

		//if( g_pFramework->getGameKeyMode() != GAMEKEY_MODE_MENU )
		//	m_pOwner->MoveStop();	//	Ű �Է��� ������ �̵� ����

		getInputContext()->setInputMoveRho( 0.f);
		getInputContext()->setInputMoveTheta( 0.f);
	}	

	if( CHUD_CountDown::instance() )
	{
		if( CHUD_CountDown::instance()->is_act_count_down() == true ) return; //ī��Ʈ ���� ��쿣 Ű �ൿ�� �����Ѵ�.
	}

	// �ӽ� �ڵ�
	// Local�� �մ� ���� ó���� ���� �����س����ϴ�. ���� ���� ����
	getInputContext()->setKeyPressInput( keyPress);
	getInputContext()->setKeyStrokeInput( keyStroke);

	//	�̵� ���� ó���մϴ�.
	bool bChangeBody = false;

	///////////////////////////////////////////////////////////		�ɾ�/�Ͼ
	bChangeBody = ProcessCrouch( rDeltatime);

	///////////////////////////////////////////////////////////		����
	if( keyStroke & GAME_KEY_ACTION_JUMP )
	{
		if (m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB) == false)
			m_pOwner->Cmd_JumpStart();
	}
	
	
	ProcessMovement( rDeltatime, bChangeBody);
	

	//	ĳ���� ��ġ �̵�
	getMoveContext()->ProcessAccel();

	if( gstatemgr::i()->getStageState() == GAMESTAGE_STATE_BATTLE && m_pOwner->IsDino() == false)
	{
		ProcessWeapon( rDeltatime);
		ProcessExtension();
	}
	else if(gstatemgr::i()->getStageState() == GAMESTAGE_STATE_BATTLE && m_pOwner->IsDino() == true)
	{
		ProcessDinoAttack( rDeltatime);
	}
}

void CGameCharaInputControlLocal::ProcessWeaponCollisionFire( REAL32 rDeltatime )
{
	//UINT64 keyPress = getInputContext()->getKeyPressInput();
	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	bool bIsHackUser = HackContext::i()->isHackUserFrezen( BattleSlotContext::i()->getMySlotIdx());

	if( (keyStroke & GAME_KEY_ACTION_ATTACK) && !bIsHackUser)
	{	
		CQAWeaponCMgr::i()->ThrowBox(m_pOwner);
	}
}
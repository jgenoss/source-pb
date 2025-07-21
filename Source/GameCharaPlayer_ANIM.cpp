
#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaPlayer.h"
#include "StageBattle.h"

//#define ANIMTIME_MOVE_DURATION		0.666f			//	MOVE �ѱ���
//#define ANIMTIME_MOVE_RIGHT_STEP	0.322f			//	MOVE ������ Ÿ�̹�
//#define ANIMTIME_MOVE_LEFT_STEP		(ANIMTIME_MOVE_RIGHT_STEP * 2.0f - 0.01f)			//	MOVE �޹� Ÿ�̹�
/*
void CGameCharaPlayer::ResetDefaultAnim( BOOL bIs3PV)
{
	//	ĳ���� Anim ���� ����
	m_pActionContext->setBody( CHARA_BODY_STAND);
	m_pActionContext->setBody2( CHARA_BODY_ATTACKIDLE);
		
	m_pActionContext->setLower( CHARA_LOWER_IDLE);
	m_pActionContext->setUpper( CHARA_UPPER_IDLE);

	if( !bIs3PV)
		m_AnimSet = CHARA_ANIMSET_1PV;
	else
		m_AnimSet = CHARA_ANIMSET_3PV;

	ACTION_apply( bIs3PV);
	//
	I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

	_ANIM_stop();				

	ANIM_playLower( CHARA_LOWER_IDLE, ID_LOWER_IDLE);
	ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_IDLE);

	//	������ ����
	m_pAnimContext->getAILower()->SetAIStateImmediately( g_pViewer->getDeltaTime());
	m_pAnimContext->getAIUpper()->SetAIStateImmediately( g_pViewer->getDeltaTime());
}

void CGameCharaPlayer::ResetLobbyAnim( void)
{
	//	ĳ���� Anim ���� ����
	m_pActionContext->setBody( CHARA_BODY_STAND);
	m_pActionContext->setBody2( CHARA_BODY_ATTACKIDLE);
		
	m_pActionContext->setLower( CHARA_LOWER_NONE);
	m_pActionContext->setUpper( CHARA_UPPER_NONE);

	m_AnimSet = CHARA_ANIMSET_3PV;

	//ACTION_apply( TRUE);
	getAnimContext()->getControl()->AnimSetApply();

	//
	I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

	_ANIM_stop();				

	//	�κ� �ִϸ��̼����� �÷���
	ANIM_playLower( CHARA_LOWER_IDLE, ID_LOWER_IDLE);
	ANIM_playUpper( CHARA_UPPER_NONEBATTLEIDLE, ID_UPPER_NONEBATTLE_IDLE, 0.0f);

	//	������ ����
	m_pAnimContext->getAILower()->SetAIStateImmediately( g_pViewer->getDeltaTime());
	m_pAnimContext->getAIUpper()->SetAIStateImmediately( g_pViewer->getDeltaTime());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Anim Callback
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHSND_ACTRATE_FACTER 10	//�߻��󵵰��

#define CHSND_ACTRATE_ACCESSORY 4	//��� �ޱ׶� �Ÿ��� �Ҹ�
#define CHSND_ACTRATE_RUSTLE	8	//���� �ν÷� �Ÿ��� �Ҹ�

void CGameCharaPlayer::OnMoveRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnMoveRun( pCtx, tm );

	m_fRunElapseTime += tm;
	
	//	���� �̵��Ҷ��� ���ڱ� �Ҹ�������...���� �ɾ� �̵��ϸ� ���ڱ� �Ҹ� �ȳ����� �Ѵ�.
	if( getActionContext()->isLower( CHARA_LOWER_MOVE)  || getActionContext()->isLower( CHARA_LOWER_WALK))
	{	
//		if( isNetwork()) I3TRACE("ON MOVE RUN : %f, Elapse : %f\n", tm, m_fRunElapseTime);
		//I3TRACE("ON MOVE RUN : %f, Elapse : %f\n", tm, m_fRunElapseTime);

		//	���ڱ� �Ҹ� �÷���~
//		REAL32 fRunDuration = ANIMTIME_MOVE_DURATION;
		BOOL bTrue1PV = FALSE;
		if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
		{	
			CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
			if( pCharacter == this ) bTrue1PV = TRUE;
		}	

		if( m_fRunElapseTime <= tm)
		{
			if( m_pAnimContext->isAILower())
			{
				i3AIState * pAIState = m_pAnimContext->getAILower()->getCurrentAIState();
				m_fRunDuration = (pAIState != NULL) ? pAIState->getDuration() * (1.0f / m_pMoveContext->getSpeed()) : 0.0f;
				if( m_fRunDuration > ANIMTIME_MOVE_DURATION) m_fRunDuration = ANIMTIME_MOVE_DURATION;
			}

			//if( is1PV())
			if( bTrue1PV || (m_fRunDuration == 0.0f) )	
			{
				if( getActionContext()->isUpper( CHARA_UPPER_MOVE ))
				{
					i3AIState * pAI = m_pAnimContext->getAIUpper()->getCurrentAIState();
					if( pAI != NULL)
					{
						m_fRunDuration = pAI->getActualDuration();//	1��Ī �ִϸ��̼��� ��ü�� �÷������� �ʱ� ������ ���� �ð� ����
					}
				}

				REAL32	rScaleRate = 0.0f;
				if( !getActionContext()->isUpper( CHARA_UPPER_MOVE ))
				{
					WeaponBase * pWeapon = getWeaponContext()->getCurrent();
					if( pWeapon != NULL )
					{	
						CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

						if( getActionContext()->isBody( CHARA_BODY_STAND))
							rScaleRate =  pWeaponInfo->m_rMoveNormalStand * 0.01f;
						else if( getActionContext()->isBody( CHARA_BODY_CROUCH))
							rScaleRate = pWeaponInfo->m_rMoveNormalSit * 0.01f;

						if( isStateMask( CHARA_STATE_ZOOM))
						{
							if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) &&
								!getActionContext()->isUpper( CHARA_UPPER_LOADBULLET))	//	���� �������� �����ÿ��� ����ȵǰ�...
								rScaleRate *= pWeaponInfo->m_rMoveNormalZoom * 0.01f;
						}

						if( getActionContext()->isUpper( CHARA_UPPER_FIRE) || getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE))			//	�߻� �����϶�
						{
							rScaleRate *= pWeaponInfo->m_rMoveNormalFire * 0.01f;
						}

						m_fRunDuration = 0.544f * (1.0f / rScaleRate);
					}
				}

				if( m_fRunDuration < I3_EPS)
				{
					m_fRunDuration = 0.544f * (1.0f / rScaleRate);
				}
			}

			m_pAnimContext->setLeftStepTime( m_fRunDuration * 0.822f);
			m_pAnimContext->setRightStepTime( m_fRunDuration * 0.322f);
		}

		if( m_fRunDuration > 0.0f)
		{
			if( m_fRunElapseTime >= m_fRunDuration)
			{	
				//I3TRACE("STEP ELAPSE RESET: %f : DeltaTime: %f\n", m_fRunElapseTime, tm);
				//	Reset
				m_fRunElapseTime = 0.f;
				m_bRightStepSound = FALSE;

				if( isAlive())
				{
					m_nAccessory = ( m_nAccessory > 0)	? 0 : 1;
					{
						if( !getActionContext()->isBody( CHARA_BODY_CROUCH))
						{
							if( getActionContext()->isLower( CHARA_LOWER_WALK))
							{
								g_pSndMng->StartCharacterActionSnd( GTBCA_ON_WALK_EQUIPMENT, m_nAccessory, GetPos(), is1PV());
							}
							else if( getActionContext()->isLower( CHARA_LOWER_MOVE))
							{
								//if( !isNetwork())
								{
									g_pSndMng->StartCharacterActionSnd( GTBCA_ON_RUN_EQUIPMENT, m_nAccessory, GetPos(), is1PV());
								}
							}
						}
					}
				}
			}
			else if( m_fRunElapseTime >= m_pAnimContext->getLeftStepTime())
			{	//	�޹� �Ҹ� �÷���	
				if( m_bRightStepSound)
				{
					if( !getActionContext()->isBody( CHARA_BODY_CROUCH) && !getActionContext()->isLower( CHARA_LOWER_WALK))
					{	
						m_pSoundContext->PlayFootStep( m_pMoveContext->getFloorTerrainType(), true, is1PV());
					}

					m_bRightStepSound = FALSE;
					//I3TRACE("LEFT STEP: %f, DeltaTime: %f\n", m_fRunElapseTime, tm);
				}				
			}
			else if( m_fRunElapseTime >= m_pAnimContext->getRightStepTime())
			{	//	������ �Ҹ� �÷���
				if( !m_bRightStepSound)
				{
					if( !getActionContext()->isBody( CHARA_BODY_CROUCH) && !getActionContext()->isLower( CHARA_LOWER_WALK))
					{
						m_pSoundContext->PlayFootStep( m_pMoveContext->getFloorTerrainType(), false, is1PV());
					}

					m_bRightStepSound = TRUE;
					//I3TRACE("RIGHT STEP: %f, DeltaTime: %f\n", m_fRunElapseTime, tm);
				}
			}
			else
			{
				//I3TRACE("no available: %f, DeltaTime: %f\n", m_fRunElapseTime, tm);
			}
		}
	}
}

void CGameCharaPlayer::OnAttackRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnAttackRun( pCtx, tm);

	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;

	WeaponStateIdle();
}

void CGameCharaPlayer::OnAttackReadyFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnAttackReadyFinish( pCtx, tm );

	removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);		//	�غ� ������ �ٷ� �߻� ����
}

void CGameCharaPlayer::OnJumpEndFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnJumpEndFinish( pCtx, tm );

	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;

	//	�ٸ� �������� ��ȯ�� �ִϸ��̼��� �ٸ������ ���� �����̸� ���� �������� ���ư��� �ʴ´�.		
	if( _ANIM_getUpperLocalTime() >= _ANIM_getUpperDuration())
	{
		ANIM_playUpperReturn( !is1PV());		//	���� �����̾��� Upper�� Idle �Ǵ� Move�� ���ƿ´�.
	}
}

void CGameCharaPlayer::OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnLoadBulletRun( pCtx, tm );
}

void CGameCharaPlayer::OnDamageRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnDamageRun( pCtx, tm );

	if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE) || getActionContext()->isLower( CHARA_LOWER_DEATH))	return;

	//	�ٸ� �������� ��ȯ�� �ִϸ��̼��� �ٸ������ ���� �����̸� ���� �������� ���ư��� �ʴ´�.
	
	if( _ANIM_getUpperLocalTime() >= _ANIM_getUpperDuration())
	{
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE
			| CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_WEAPONCHANGE);

		//	���� �ൿ���� ���ƿɴϴ�.. (Idle �̳� Move)
		ANIM_playUpperReturn( !is1PV());
	}
}

void CGameCharaPlayer::OnExtensionDeactivateRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnExtensionDeactivateRun( pCtx, tm );
 
	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;

	if( _ANIM_getUpperLocalTime() >= _ANIM_getUpperDuration() - tm)
	{
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);

		ANIM_playUpperReturn( !is1PV());
		
		ApplyCrosshair();
	}
}

void CGameCharaPlayer::OnExtensionActivateRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnExtensionActivateRun( pCtx, tm );

	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;
	
	if( _ANIM_getUpperLocalTime() >= _ANIM_getUpperDuration() - tm)
	{
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);

		ANIM_playUpperReturn( !is1PV());
		
		ApplyCrosshair();
	}
}

void CGameCharaPlayer::OnDeathFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnDeathFinish( pCtx, tm );

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	//	������ Ÿ�̸� ����
	if( !g_pGameContext->IsBombMissionMode() ||
		(pMissionMng->getTotalRespawnCount() > pMissionMng->getRespawnCount( getCharaNetIndex())) )
	{
#if defined( I3_DEBUG )
		if( !g_pConfig->m_bNetworking)
		{
			m_rRespawnElapseTime = g_pCharaInfoDataBase->getGlobal()->m_rRespawnTime * 2.f;
		}
		else
#endif
		{
			m_rRespawnElapseTime = g_pCharaInfoDataBase->getGlobal()->m_rRespawnTime;
		}
	}
}

void CGameCharaPlayer::OnUI_IdleAFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnUI_IdleAFinish( pCtx, tm);

	// �⺻ Idle�� ���ϴ�
	if (FALSE == g_pFramework->IsBattleStage())	
	{
		//if( STATE_getUpper( ) == CHARA_UPPER_NONEBATTLEWEAPONSHOW )
		//	ACTION_NoneBattleIdle();
	}
	else
	{
		if (g_pFramework->GetCurrentStage())
		{
			char szTrace[ 256] = { 0,};
			sprintf( szTrace, "�߸��� �ִϸ޴��̼�. ����:%s(%d), ", g_pGameContext->getNickForSlot( getCharaNetIndex()), getCharaNetIndex() );
			if( getWeaponContext()->getCurrent() != NULL)
			{
				sprintf( szTrace, "����:%s", getWeaponContext()->getCurrent()->getWeaponInfo()->m_szName);
			}

			I3TRACE( szTrace);
		}
		else
		{
			// CurrentStage�� NULL�̴�. �������� ���� �� ȣ��Ǿ���.
			I3TRACE("[WARNING] CGameCharaPlayer::OnUI_IdleAFinish() - currentStage is NULL\n");			
		}
	}
}

void CGameCharaPlayer::OnUI_IdleBRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnUI_IdleBRun( pCtx, tm);

	if( m_pAnimContext->getAIUpper() == NULL )
		return;

	if( getUpdateContext()->getUpperAniTimer() + tm > m_pAnimContext->getAIUpper()->getCurrentAIState()->getDuration())
	{
		ACTION_NoneBattleIdle();
		getUpdateContext()->setUpperAniTimer( 0.f);
	}
}

void CGameCharaPlayer::OnUI_IdleBFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnUI_IdleBFinish( pCtx, tm);

	// �⺻ Idle�� ���ϴ�
	if (FALSE == g_pFramework->IsBattleStage())	
	{
		ACTION_NoneBattleIdle();
		getUpdateContext()->setUpperAniTimer( 0.f);
	}
	else
	{
		if (g_pFramework->GetCurrentStage())
		{
			char szTrace[ 256] = { 0,};
			sprintf( szTrace, "�߸��� �ִϸ޴��̼�. ����:%s(%d), ", g_pGameContext->getNickForSlot( getCharaNetIndex()), getCharaNetIndex() );
			if( getWeaponContext()->getCurrent() != NULL)
			{
				sprintf( szTrace, "����:%s", getWeaponContext()->getCurrent()->getWeaponInfo()->m_szName);
			}

			I3TRACE( szTrace);
		}
		else
		{
			// CurrentStage�� NULL�̴�. �������� ���� �� ȣ��Ǿ���.
			I3TRACE("[WARNING] CGameCharaPlayer::OnUI_IdleAFinish() - currentStage is NULL\n");			
		}
	}
}
*/


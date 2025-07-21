#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaAnimControl.h"
#include "GameCharaBase.h"
#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "Weapon.h"
#include "StageBattle.h"
#include "GameCharaDino.h"
#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "HackContext.h"
#include "GameMissionManager.h"
#include "GameCharaMultiWeaponContext.h"

#include "MainWeapon_DualCIC.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "ClientStageInfoUtil.h"


I3_CLASS_INSTANCE( CGameCharaAnimControl);

void CGameCharaAnimControl::_ProcessJumpEnd()
{
	VEC3D * pVecDir = getMoveContext()->getMoveDir();

	CHARA_ACTION_LOWER lower_state = CHARA_LOWER_RUN;
		
	//	������ �ٽ� �ѹ� ����Ű �Է��� üũ�մϴ�. (�׳� ���� ���� �ٷ� �̵�����)
	UINT64 keyPress = getInputContext()->getKeyPressInput();

	if( keyPress & GAME_KEY_MOVE_FORWARD )				setZ( pVecDir, 1.f);
	else if( keyPress & GAME_KEY_MOVE_BACKWARD )		setZ( pVecDir, -1.f);
	else												setZ( pVecDir, 0.f);

	if( keyPress & GAME_KEY_MOVE_LEFT )					setX( pVecDir, 1.f);
	else if( keyPress & GAME_KEY_MOVE_RIGHT)			setX( pVecDir, -1.f);
	else												setX( pVecDir, 0.f);

	if( (getX( pVecDir) == 0.0f) && (getY( pVecDir) == 0.0f) && (getZ( pVecDir) == 0.0f))
		lower_state = CHARA_LOWER_IDLE;

	if(m_pOwner->IsDino() == false)
	{
		WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
		if( pWeapon != nullptr )
		{
			WEAPON::eSTATE state = pWeapon->getWeaponState();

			if( state == WEAPON::STATE_READY || state == WEAPON::STATE_WAIT )
				pWeapon->PlayAnimation_JumpEnd();
		}
	}
	
	bool bIsHackUser = HackContext::i()->isHackUserFrezen( BattleSlotContext::i()->getMySlotIdx());

	//	���ڸ� ������. -> ���� �� �ִϸ��̼�����
	//���ͽ���
	if( lower_state == CHARA_LOWER_IDLE || m_pOwner->IsRaptorBase() || bIsHackUser)			
	{
		m_pOwner->Cmd_JumpEnd();
	}
	else
	{	//	�̵��� ������. -> �̵� �ִϸ��̼�����
		if( keyPress & GAME_KEY_ACTION_FOOTSTEP)
			m_pOwner->Cmd_Walk( pVecDir);	//	�ȱ�
		else
			m_pOwner->Cmd_Run( pVecDir);	//	�ٱ�
	}

	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH );

	//���� ������ �ɱ� ��ư�� ������ ������ �ɾƹ�����.
	if( keyPress & GAME_KEY_ACTION_CROUCH)
	{
		getAnimContext()->setOldLowerID( ID_LOWER_JUMP_END);
		m_pOwner->Cmd_Crouch();
	}

	//	���� ������ �ð� ����
	//m_fJumpDelayElapsedTime = getDBInfo()->m_rJumpDelay;
	m_pOwner->setJumpDelayElapsedTime( 0.f);

	getAnimContext()->setEnableRunJump( false);
}

REAL32 CGameCharaAnimControl::_PlayFootStepSound( REAL32 rDuration, REAL32 rElapseTime, REAL32 tm)
{
	if( rDuration > 0.0f)
	{
		if( rElapseTime >= rDuration || rElapseTime <= tm)
		{	
			if( rElapseTime >= rDuration)
				rElapseTime = 0.f;

			m_pOwner->removeCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT);
			INT32 nAccessory = ( getAnimContext()->getAccessorySoundIdx() > 0)	? 0 : 1;
			getAnimContext()->setAccessorySoundIdx( nAccessory);

			if( m_pOwner->IsDino() == false)
			{
				if( !getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
				{
					if( getActionContext()->isLower( CHARA_LOWER_WALK))
					{
						g_pSndMng->StartCharacterActionSnd( GTBCA_ON_WALK_EQUIPMENT, nAccessory, m_pOwner->GetPos(), false);
					}
					else if( getActionContext()->isLower( CHARA_LOWER_RUN))
					{
						g_pSndMng->StartCharacterActionSnd( GTBCA_ON_RUN_EQUIPMENT, nAccessory, m_pOwner->GetPos(), false);
					}
				}
			}
		}
		else if( rElapseTime >= getAnimContext()->getLeftStepTime())
		{	//	�޹� �Ҹ� �÷���	
			if( m_pOwner->isCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT))
			{
				if( !getActionContext()->isBody( CHARA_BODYLOWER_CROUCH) &&
					!getActionContext()->isLower( CHARA_LOWER_WALK))
				{
					getSoundContext()->PlayFootStep( getMoveContext()->getFloorTerrainType(), true, false);
				}

				//g_pEffectManager->AddEnvironmentEffect( ENV_EFEECT_TREX_WALK, m_pOwner->GetPos(), &I3_YAXIS);

				m_pOwner->removeCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT);
			}				
		}
		else if( rElapseTime >= getAnimContext()->getRightStepTime())
		{	//	������ �Ҹ� �÷���
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT))
			{
				if( !getActionContext()->isBody( CHARA_BODYLOWER_CROUCH) &&
					!getActionContext()->isLower( CHARA_LOWER_WALK))
				{
					getSoundContext()->PlayFootStep( getMoveContext()->getFloorTerrainType(), false, false);
				}
				m_pOwner->addCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT);

				//g_pEffectManager->AddEnvironmentEffect( ENV_EFEECT_TREX_WALK, m_pOwner->GetPos(), &I3_YAXIS);
			}
		}
	}

	return rElapseTime;
}

bool CGameCharaAnimControl::PlayLower( ID_LOWER_AI id, REAL32 timeStart, REAL32 timeScale)
{
	I3ASSERT( m_pOwner->IsDino() == false);

	bool bRv = false;
	i3AIContext * pAILower = getAnimContext()->getAILower();
	
	// ó������ �ʱ�ȭ ���·� UNKNOWN���°� ���� �� �ֽ��ϴ�.
	// �⺻������ IDLE�� ������ ���ݴϴ�.
	if( (id <= ID_LOWER_UNKNOWN) || (id >= ID_LOWER_AI_MAX) )
		id = ID_LOWER_IDLE;

	//	���� ������ �������� �����Ѵ�.
	getAnimContext()->setOldLowerID( id);
		
	if( (pAILower != nullptr) )
	{
		INT32 nAIIdx = 0;

		if(m_pOwner->IsDino())
			nAIIdx = ((CGameDinoAnimContext*)getAnimContext())->getDinoAILowerID( (ID_LOWER_DINO_AI)id);
		else
			nAIIdx = getAnimContext()->getAILowerID( getActionContext()->getBodyLower(), id);

		I3ASSERT( nAIIdx >= 0);

		//GlobalFunc::PB_TRACE("nAIIdx %d", nAIIdx);
		pAILower->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	��ü AI ���� ����

		bRv = true;

#if defined( ENABLE_CHARA_ANIM_OUTPUT )
		{
			TRACE_FUNC;
			AnimDebug( pAILower, timeStart );
		}
#endif
	}

	return bRv;
}

bool CGameCharaAnimControl::PlayUpper( ID_UPPER_AI id, REAL32 timeStart, REAL32 timeScale)
{
	if( (id <= ID_UPPER_UNKNOWN) || (id >= ID_UPPER_AI_MAX))
	{
		I3PRINTLOG(I3LOG_NOTICE, "�߸��� ID�Դϴ�.");
		return false;			// �ʱ�ȭ �ȵ� �ִϸ��̼��� ������ ��찡 �ֽ��ϴ�. 2008. 8.29
	}

	//GlobalFunc::PB_TRACE("PlayUpper id %d", id);

	if( getActionContext()->isLower( CHARA_LOWER_DEATH))	return false;

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper == nullptr) return false;

	i3AIContext * pAIUpper_Left = getAnimContext()->getAIUpper_Left();

	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();

	//	���� ��ȯ�� ���ܷ� ����� ���� �ٲ��� ���� �����̸� ����� �����̴�.
	//	���� �����ӿ� ���� ��尡 �ٲ�� �Ǿ� �����Ƿ� ĳ���ʹ� ����� ����� �ν��ؾ� �Ѵ�.	komet
	WEAPON_SLOT_TYPE nSwapWeaponSlotNum = getWeaponContext()->getSwapSlotNum();
	WEAPON_SLOT_TYPE nCurWeaponSlotNum = getWeaponContext()->getCurrentSlotNum();
	if( nSwapWeaponSlotNum != nCurWeaponSlotNum )//&& ID_UPPER_SWAP_WEAPON == id)
		pWeapon = getWeaponContext()->getWeapon( nSwapWeaponSlotNum);

	if (getWeaponContext()->getSwapSlotNum() == WEAPON_SLOT_PRIMARY && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		pWeapon = m_pOwner->getMultiWeaponContext()->GetSwapMultiWeapon(false);
	}
	
	if( pWeapon == nullptr )	return false;

	pWeapon = pWeapon->getUsedWeapon();
	I3ASSERT( pWeapon != nullptr);
	//	WeaponInfo�� swap������ ��ȯ�ɼ� �����Ƿ� ���� ������ �ִϸ��̼����� ���
	CWeaponInfo * pWeaponInfo = pWeapon->getAbsoluteWeaponInfo();	

	// KEYWORD : WEAPON_C5
	if( i3::same_of<WeaponBombTrigger*>(pWeapon))
	{
		WeaponBase * pLinkWeapon = ((WeaponBombTrigger*)pWeapon)->getBomb();
		if( pLinkWeapon != nullptr)
			pWeaponInfo = pLinkWeapon->getWeaponInfo();
	}

	I3ASSERT( pWeaponInfo != nullptr );

#if defined( AI_MOVE_TO_WEAPON)
	INT32 nAIIdx = pWeapon->getCharaAIIndex( m_pOwner->getAnimSet(), pActionCtx->getBodyUpper(), id);
#else
	INT32 nAIIdx = getAnimContext()->getAIUpperID( pWeaponInfo->GetTypeClass(), pWeaponInfo->GetNumber(), id);
#endif

	if( nAIIdx < 0 || nAIIdx >= pAIUpper->getAIStateCount() )
	{
//#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
//		//m_pOwner->DumpChara();
//		I3PRINTLOG(I3LOG_WARN,  "Chara Animset no animation. ANIMSET(%d) W_CLASS(%d) W_NUMBER(%d), BODY2(%d), ID(%d)",
//			m_pOwner->getViewType(), pWeaponInfo->GetTypeClass(), pWeaponInfo->GetNumber(), getActionContext()->getBodyUpper(), id);
//#endif
		return false;
	}

	//	(���� ������ �������� �����Ѵ�.)
	getAnimContext()->setOldUpperID( id);
	getAnimContext()->setOldUpperDualLeftID( ID_UPPER_DUAL_UNKNOWN);
	getAnimContext()->setOldUpperDualRightID( ID_UPPER_DUAL_UNKNOWN);

#if defined( AI_MOVE_TO_WEAPON)
	if( id == ID_UPPER_SWAP_WEAPON)
	{
		// ���� ���� ����, 1 Frame ������ ���� ��� ���Ⱑ �߸� ��Ÿ���� ������
		// �����ϱ� ����, Delay�� �߻����� �ʴ� setCurrentAIStateDirect() �Լ���
		// ����.
		pAIUpper->setCurrentAIStateDirect( nAIIdx, timeStart, timeScale);		//	��ü AI ���� ����

		if( (pAIUpper_Left != nullptr) && g_pFramework->IsBattleStage() )	// UI �ִϸ��̼��� ���� �ʵ��� �մϴ�.
		{
			pAIUpper_Left->setCurrentAIStateDirect( nAIIdx, timeStart, timeScale);
		}
	}
	else
#endif
	{
		pAIUpper->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	��ü AI ���� ����

		if( (pAIUpper_Left != nullptr) && g_pFramework->IsBattleStage() )	// UI �ִϸ��̼��� ���� �ʵ��� �մϴ�.
		{
			pAIUpper_Left->setCurrentAIState( nAIIdx, timeStart, timeScale);
		}
	}

#if defined( ENABLE_CHARA_ANIM_OUTPUT )
	TRACE_FUNC;
	AnimDebug( pAIUpper, timeStart);
#endif

	return true;
}

bool CGameCharaAnimControl::PlayDinoUpper( ID_UPPER_DINO_AI id, REAL32 timeStart, REAL32 timeScale)
{
#ifdef ENABLE_CHARA_ANIM_OUTPUT
	GlobalFunc::PB_TRACE("PlayDinoUpper id %d", id);
#endif

	if( (id <= ID_UPPER_DINO_UNKNOWN) &&  (id >= ID_UPPER_DINO_AI_MAX))
	{
		I3PRINTLOG(I3LOG_NOTICE, "�߸��� ID�Դϴ�.");
		return false;			// �ʱ�ȭ �ȵ� �ִϸ��̼��� ������ ��찡 �ֽ��ϴ�. 2008. 8.29
	}

	if( m_pOwner->IsRaptorBase())
	{
		if( id == ID_UPPER_DINO_DAMAGE_MOVE_LEFT)		{	id = ID_UPPER_DINO_DAMAGE_LEFT;}
		else if( id == ID_UPPER_DINO_DAMAGE_MOVE_RIGHT) {	id = ID_UPPER_DINO_DAMAGE_RIGHT;}
	}

	//GlobalFunc::PB_TRACE("ID_UPPER_DINO_AI %d \n", id);

	if( getActionContext()->isLower( CHARA_LOWER_DEATH))	return false;

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper == nullptr) return false;
	i3AIContext * pAIUpper_Left = getAnimContext()->getAIUpper_Left();


	INT32 nAIIdx = ((CGameDinoAnimContext*)getAnimContext())->getDinoAIUpperID( id);
	if( nAIIdx < 0 || nAIIdx >= pAIUpper->getAIStateCount() )
	{
		I3PRINTLOG(I3LOG_WARN,  "Dino Animset no animation. ANIMSET(%d) ID(%d)",	m_pOwner->getViewType(), id);
		return false;
	}

	//	(���� ������ �������� �����Ѵ�.)
	((CGameDinoAnimContext*)getAnimContext())->setOldDinoUpperID( id);
	getAnimContext()->setOldUpperDualLeftID( ID_UPPER_DUAL_UNKNOWN);
	getAnimContext()->setOldUpperDualRightID( ID_UPPER_DUAL_UNKNOWN);
	pAIUpper->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	��ü AI ���� ����

	if( pAIUpper_Left != nullptr)
		pAIUpper_Left->setCurrentAIState( nAIIdx, timeStart, timeScale);

#if defined( ENABLE_CHARA_ANIM_OUTPUT )
	TRACE_FUNC;
	AnimDebug( pAIUpper, timeStart);
#endif

	return true;
}

/*virtual*/bool CGameCharaAnimControl::PlayUpperDual( ID_UPPER_DUAL_AI id, REAL32 timeStart, REAL32 timeScale)
{	
	INT32 nAIIdx;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return false;
	if( id <= ID_UPPER_DUAL_UNKNOWN || id >= ID_UPPER_DUAL_AI_MAX) return false;
	
	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr )	return false;
	
	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr );

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper == nullptr)	return false;
	i3AIContext * pAIUpperLeft = getAnimContext()->getAIUpper_Left();
	if( pAIUpperLeft == nullptr)	return false;
	
#if defined( AI_MOVE_TO_WEAPON)
	// KEYWORD : AI_MOVE_TO_WEAPON
	nAIIdx = pWeapon->getCharaDualAIIndex( m_pOwner->getAnimSet(), id);
#else
	nAIIdx = getAnimContext()->getAIUpperDualID( pWeaponInfo->GetNumber(), id);
#endif

	if( nAIIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN,  "Chara Animset no animation. ANIMSET(%d) W_CLASS(WEAPON_CLASS_DUALHANDGUN) W_NUMBER(%d), ID(%d)",
			m_pOwner->getViewType(),	pWeaponInfo->GetNumber(), id);
		return false;
	}

	if( id < ID_UPPER_DUAL_IDLE_RIGHT)
	{
		//	(���� ������ �������� �����Ѵ�.)
		getAnimContext()->setOldUpperDualLeftID( id);
		pAIUpperLeft->setCurrentAIState( nAIIdx, timeStart, timeScale);
	}
	else
	{
		//	(���� ������ �������� �����Ѵ�.)
		getAnimContext()->setOldUpperDualRightID( id);
		pAIUpper->setCurrentAIState( nAIIdx, timeStart, timeScale);
	}

	getAnimContext()->setOldUpperID( ID_UPPER_UNKNOWN);

	return true;
}

void CGameCharaAnimControl::PlayStop( void)
{
	m_pOwner->getSceneObject()->GetBody()->StopAnim();
}

void CGameCharaAnimControl::AnimDebug( i3AIContext * pCtx, REAL32 timeStart)
{//	i3AIContext�� nullptr�� ��쵵 �ִ�. ��ŵ�ϵ���
	if( pCtx == nullptr)		return;

	i3AIState * pAIState = nullptr;
	char * pszAIName = "(null)";

	//	���� Anim�� �� �����ӿ� ������� �ʾҴ�.
	if( pCtx->getNextAIStateIndex() != -1)		pAIState = pCtx->getNextAIState();
	else										pAIState = pCtx->getCurrentAIState();
	
	if( pAIState != nullptr)
	{
		if( pAIState->getAnim())	
			pszAIName = (char *) pAIState->getAnim()->GetName();
		
		// �̰� ���� ���ð� ENABLE_CHARA_ANIM_OUTPUT �������� ������ ���⿡ �ȵ��ɴϴ�. komet
		I3TRACE("- AI (%s) Anim (%s) (start %.4f, scale %.2f)\n", pAIState->GetName(), pszAIName, timeStart, pAIState->getTimeScale());
	}
	else
	{
		I3TRACE("- AI (null)\n");
	}
}

#if defined( AI_MOVE_TO_WEAPON)
void CGameCharaAnimControl::_CharaChangeAIContext( const char * pszPVState )
{
	char szAIUpperPath[ MAX_PATH] = "CharaAI:";
	char szAIUpperDual[ MAX_PATH] = {0,};

	//char * pszWeaponClass = nullptr;
	
	char * pszIdleState = "AttackIdle";	

	INT32 nSwapWeaponSlotNum = getWeaponContext()->getSwapSlotNum();
	INT32 nCurWeaponSlotNum = getWeaponContext()->getCurrentSlotNum();

	i3AIContext * UpperRoot = nullptr;
	WeaponBase * pWeapon = nullptr;

	INT32 nSwapWeaponSlotNum = pWeaponCtx->getSwapSlotNum();
	INT32 nCurWeaponSlotNum = pWeaponCtx->getCurrentSlotNum();

	if( nCurWeaponSlotNum != nSwapWeaponSlotNum)		//	���� �����ӿ� �ٲ� ������ ���
		pWeapon = getWeaponContext()->getWeapon( nSwapWeaponSlotNum);
	else
		pWeapon = m_pOwner->GetCurrentCharaWeapon();

	if( pWeapon == nullptr)
		return;

	// KEYWORD : DUALSMG
	pWeapon = pWeapon->getUsedWeapon();
	I3ASSERT( pWeapon != nullptr);

	CWeaponInfo * pWeaponInfo = pWeapon->getAbsoluteWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	//	note. ���� CHARA_BODYUPPER_NONE ���´� Upper�� Dodge �Ǵ� Death ���ۿ����� �����Դϴ�
	switch( getActionContext()->getBodyUpper())
	{
	case CHARA_BODYUPPER_NONE:			//	���� �ִϸ��̼��� AttackIdle, Idle ������ �����ϴ�.
	case CHARA_BODYUPPER_ATTACKIDLE:		pszIdleState = "AttackIdle";	break;
	case CHARA_BODYUPPER_ZOOM:
		// ���� ���� ���Ⱑ ������ ��찡 �ֽ��ϴ�. �����δ� ������ �ȵ����� ��ũ ������ ������ ��찡 �߻��մϴ�.
		if( !pWeaponInfo->isAnimZoomType())
			pszIdleState = "AttackIdle";
		else
			pszIdleState = "Zoom";
		break;
	case CHARA_BODYUPPER_GRENADE :
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER)
			pszIdleState = "Grenade";
		else
			pszIdleState = "AttackIdle";
		break;
	case CHARA_BODYUPPER_SHOTGUN :		//	LSS (���� ����)
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER)
			pszIdleState = "ShotGun";
		else
			pszIdleState = "AttackIdle";
		break;
	case CHARA_BODYUPPER_KNIFE :		//	�Ѱ�
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_BAYONET)
			pszIdleState = "Knife";
		else
			pszIdleState = "AttackIdle";
		break;
	case CHARA_BODYUPPER_CROSSFIRE :	//	�������
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_CROSSFIRE)
			pszIdleState = "CrossIdle";
		else
			pszIdleState = "AttackIdle";
		break;
	default:						pszIdleState = "AttackIdle";	break;
	}
	
	//	Upper AI Path
	i3::generic_string_cat( szAIUpperPath, pszPVState);							//	����
	i3::safe_string_copy( szAIUpperDual, szAIUpperPath, MAX_PATH);

	if( getActionContext()->isBody2( CHARA_BODYUPPER_NONE))
	{
		szAIUpperPath[ strlen( szAIUpperPath) - 1] = '\0';					//	No AttackIdle or Idle or Zoom
	}
	else
	{
		i3::generic_string_cat( szAIUpperPath, pszIdleState);						//	AttackIdle or Idle or Zoom
	}


	i3AIContext * pAIUpper;
	//if( pAIUpper != nullptr)	pAIUpper->SetExclusiveEnable( false, false);				// ���� AI_Upper�� Disable ��Ű�� �ڵ�.

	pAIUpper = m_pOwner->FindAI( szAIUpperPath);

	I3ASSERT( pAIUpper != nullptr);
	if( pAIUpper == nullptr)
	{
		char szMsg[MAX_STRING_COUNT] = "";
		pWeaponInfo->GetWeaponName( szMsg);

		I3PRINTLOG(I3LOG_NOTICE, "Cannot find a animation node (%s) with weapon (%s)", szAIUpperPath, szMsg);
		return;
	}

	getAnimContext()->setAIUpper( pAIUpper);

	{
		i3AI * pAI = pAIUpper->getAI();
		i3AIDriver * pDriver = pAI->getAIDriver();

		I3TRACE( "AI Driver : %s\n", pDriver->GetMeta()->GetClassName());
	}

	//	UpperRoot�� �������� m_pAIUpper�� Ȱ���ϰ� ���þ��� AIState�� Disable ��Ų��.
	UpperRoot = m_pOwner->FindAI( "CharaAI");
	I3ASSERT(UpperRoot != nullptr);
	
	UpperRoot->setGroupEnable( pAIUpper);

	if( pWeapon->isDualGun())
	{
		i3::generic_string_cat( szAIUpperDual, "AttackIdle_Left");
		i3AIContext * pAIUpperDual = m_pOwner->FindAI( szAIUpperDual);
		if( pAIUpperDual != nullptr)
		{
			getAnimContext()->setAIUpper_Left( pAIUpperDual);
			pAIUpperDual->SetExclusiveEnable( true, false);
		}
	}
	else
	{
		getAnimContext()->setAIUpper_Left( nullptr);
	}
}

#else

void CGameCharaAnimControl::_CharaChangeAIContext( const char * pszPVState )
{
	char szAIUpperPath[ MAX_PATH] = "Upper:";
	char szAIUpperDual[ MAX_PATH] = {0,};

	char * pszWeaponClass = nullptr;
	
	char * pszIdleState = "AttackIdle";	

	i3AIContext * UpperRoot = nullptr;
	WeaponBase * pWeapon = nullptr;

	if (!getWeaponContext())
	{
		TRACE("CGameCharaAnimControl::_CharaChangeAIContext getWeaponContext() = nullptr\n");
		return;
	}
	WEAPON_SLOT_TYPE nSwapWeaponSlotNum = getWeaponContext()->getSwapSlotNum();
	WEAPON_SLOT_TYPE nCurWeaponSlotNum = getWeaponContext()->getCurrentSlotNum();

	//	���� �����ӿ� �ٲ� ������ ��� - ���� �ʿ�
	if ( !g_pFramework->IsBattleStage())
	{
		if (nCurWeaponSlotNum != nSwapWeaponSlotNum)		//	���� �����ӿ� �ٲ� ������ ���
			pWeapon = getWeaponContext()->getWeapon(nSwapWeaponSlotNum);
		else
			pWeapon = m_pOwner->GetCurrentCharaWeapon();
	}
	else
	{
		if (nCurWeaponSlotNum != nSwapWeaponSlotNum && !(m_pOwner->getMultiWeaponContext()->IsPickupChange() && MyRoomInfoContext::i()->getStage()->IsMultiWeapon()))
		{
			pWeapon = getWeaponContext()->getWeapon(nSwapWeaponSlotNum);
		}
		else if ( (nSwapWeaponSlotNum == WEAPON_SLOT_PRIMARY && nCurWeaponSlotNum == WEAPON_SLOT_PRIMARY &&	MyRoomInfoContext::i()->getStage()->IsMultiWeapon()) ||
			( m_pOwner->getMultiWeaponContext()->IsPickupChange() && nSwapWeaponSlotNum == WEAPON_SLOT_PRIMARY && MyRoomInfoContext::i()->getStage()->IsMultiWeapon()))
		{
			pWeapon = m_pOwner->getMultiWeaponContext()->GetSwapMultiWeapon(false);
		}
		else
			pWeapon = m_pOwner->GetCurrentCharaWeapon();
	}

	if ( pWeapon == nullptr)
	{
		return;
	}
	else
	{
		if ( pWeapon->getWeaponInfo()->GetTypeUsage() != nSwapWeaponSlotNum)
		{
			m_pOwner->getWeaponContext()->getWeapon( nSwapWeaponSlotNum);
		}
	}

	// KEYWORD : DUALSMG
	pWeapon = pWeapon->getUsedWeapon();
	I3ASSERT( pWeapon != nullptr);

	CWeaponInfo * pWeaponInfo = pWeapon->getAbsoluteWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);
	
	switch( pWeaponInfo->GetTypeClass())
	{
	case WEAPON_CLASS_KNIFE:			pszWeaponClass = "Knife:";			break;
	case WEAPON_CLASS_HANDGUN:			pszWeaponClass = "HandGun:";		break;
	case WEAPON_SUBCLASS_ASSAULT :
	case WEAPON_CLASS_ASSAULT:			pszWeaponClass = "AssaultRifle:";	break;
	case WEAPON_SUBCLASS_SMG :
	case WEAPON_CLASS_SMG:				pszWeaponClass = "SMG:";			break;
	case WEAPON_SUBCLASS_SNIPER :
	case WEAPON_CLASS_SNIPER:			pszWeaponClass = "SniperRifle:";	break;
	case WEAPON_SUBCLASS_SHOTGUN :
	case WEAPON_CLASS_SHOTGUN:			pszWeaponClass = "ShotGun:";		break;
	case WEAPON_CLASS_BOMBTRIGGER :
	case WEAPON_CLASS_THROWING_GRENADE:
	case WEAPON_CLASS_THROWING_CONTAIN:
	case WEAPON_CLASS_THROWING_ATTACK:
	case WEAPON_CLASS_THROWING_HEAL:	pszWeaponClass = "Throwing:";		break;
	case WEAPON_CLASS_MISSION:			pszWeaponClass = "Mission:";		break;
	case WEAPON_CLASS_MG :				pszWeaponClass = "MG:";				break;
	case WEAPON_CLASS_CIC :				pszWeaponClass = "CIC:";			break;
	case WEAPON_CLASS_DUALHANDGUN :		pszWeaponClass = "DualHandGun:";	break;
	case WEAPON_CLASS_DUALKNIFE :		pszWeaponClass = "DualKnife:";	break;
	case WEAPON_CLASS_ROCKET_LAUNCHER :	pszWeaponClass = "RocketLauncher:";	break;
	case WEAPON_CLASS_DUALSMG :			pszWeaponClass = "DualSMG:";	break;
	case WEAPON_CLASS_KNUCKLE:			pszWeaponClass = "Knife:";			break;
	case WEAPON_SUBCLASS_BOW :
	case WEAPON_CLASS_BOW:				pszWeaponClass = "Bow:";			break;
	case WEAPON_CLASS_DUALSHOTGUN :		pszWeaponClass = "DualShotGun:";	break;

	default:
		I3PRINTLOG(I3LOG_FATAL,  "�߸��� �����Դϴ�. ���� Ÿ�� %d", pWeaponInfo->GetTypeClass() );
		return;
	}

	//	note. ���� CHARA_BODYUPPER_NONE ���´� Upper�� Dodge �Ǵ� Death ���ۿ����� �����Դϴ�
	switch( getActionContext()->getBodyUpper())
	{
	case CHARA_BODYUPPER_NONE:			//	���� �ִϸ��̼��� AttackIdle, Idle ������ �����ϴ�.
	case CHARA_BODYUPPER_ATTACKIDLE:		pszIdleState = "AttackIdle";	break;
	case CHARA_BODYUPPER_ZOOM:
		// ���� ���� ���Ⱑ ������ ��찡 �ֽ��ϴ�. �����δ� ������ �ȵ����� ��ũ ������ ������ ��찡 �߻��մϴ�.
		if( !pWeaponInfo->isAnimZoomType())
		{
			pszIdleState = "AttackIdle";
		}
		else
		{
			pszIdleState = "Zoom";
		}
		break;
	case CHARA_BODYUPPER_GRENADE :
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER)
			pszIdleState = "Grenade";
		else
			pszIdleState = "AttackIdle";
		break;
	case CHARA_BODYUPPER_SHOTGUN :		//	LSS (���� ����)
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER)
			pszIdleState = "ShotGun";
		else
			pszIdleState = "AttackIdle";
		break;
	case CHARA_BODYUPPER_KNIFE :		//	�Ѱ�
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_BAYONET)
			pszIdleState = "Knife";
		else
			pszIdleState = "AttackIdle";
		break;
	case	CHARA_BODYUPPER_CROSSFIRE :
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_CROSSFIRE )
			pszIdleState = "CrossIdle";
		else
			pszIdleState = "AttackIdle";
		break;
	default:						pszIdleState = "AttackIdle";	break;
	}
	
	//	Upper AI Path
	i3::safe_string_cat( szAIUpperPath, pszPVState, _countof(szAIUpperPath));							//	����
	i3::safe_string_cat( szAIUpperPath, pszWeaponClass, _countof(szAIUpperPath));						//	���� Ŭ����

	i3::string strFindWeapon;

	if( i3::same_of<WeaponBombTrigger*>(pWeapon))
	{
		WeaponBase * pLinkWeapon = ((WeaponBombTrigger*)pWeapon)->getBomb();
		if (pLinkWeapon)
			i3::safe_string_copy( strFindWeapon, "C5", 256 );
	}
	else
	{
		//	GetLinkedToCharaAI �׸��� ���� ������ �߰��Ͽ� FindWeapon�� ����մϴ�. komet
		i3::rc_wstring wstrFindWeapon;
		pWeaponInfo->GetLinkedToCharaAI( wstrFindWeapon);
		i3::utf16_to_mb(wstrFindWeapon, strFindWeapon);
	}

	i3::generic_string_cat( szAIUpperPath, strFindWeapon);					//	���� �̸�
	i3::generic_string_cat( szAIUpperPath,":");

	i3::safe_string_copy( szAIUpperDual, szAIUpperPath, MAX_PATH );

	if( getActionContext()->isBody2( CHARA_BODYUPPER_NONE))
	{
		szAIUpperPath[ strlen( szAIUpperPath) - 1] = '\0';					//	No AttackIdle or Idle or Zoom
	}
	else
	{
		i3::generic_string_cat( szAIUpperPath, pszIdleState);						//	AttackIdle or Idle or Zoom
	}

	//GlobalFunc::PB_TRACE("_CharaChangeAIContext %s", szAIUpperPath);

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper != nullptr)	pAIUpper->SetEnable( false, false);

	pAIUpper = m_pOwner->FindAI( szAIUpperPath);

	if (pAIUpper == nullptr)
	{
		TRACE("CGameCharaAnimControl::_CharaChangeAIContext pAIUpper = %s\n",szAIUpperPath);

		i3::rc_wstring wstrMsg;
		pWeaponInfo->GetWeaponName(wstrMsg);
		I3PRINTLOG(I3LOG_NOTICE, "Cannot find a animation node (%s) with weapon (%s)", szAIUpperPath, wstrMsg);
		I3ASSERT(pAIUpper != nullptr);
		return;
	}

	getAnimContext()->setAIUpper( pAIUpper);

	//	UpperRoot�� �������� m_pAIUpper�� Ȱ���ϰ� ���þ��� AIState�� Disable ��Ų��.
	UpperRoot = m_pOwner->FindAI( "Upper");
	I3ASSERT(UpperRoot != nullptr);
	
	UpperRoot->setGroupEnable( pAIUpper);

	if( pWeapon->isDualGun())
	{
		if( i3::generic_is_iequal( pszIdleState, "CrossIdle" )  )
		{
			i3::generic_string_cat( szAIUpperDual, "CrossIdle_Left");
		}
		else
		{
			i3::generic_string_cat( szAIUpperDual, "AttackIdle_Left");
		}

		i3AIContext * pAIUpperDual = m_pOwner->FindAI( szAIUpperDual);
		if( pAIUpperDual != nullptr)
		{
			// next AI �� ������ ��� �ʱ�ȭ�� �ع����� ���װ� �ִ�.
			INT32 nextai = pAIUpperDual->getNextAIStateIndex();
			// CurAI�� ���� �ؾ� �ϴµ� �߰��� �ʱ�ȭ �ع����� �ȳѾ�� ���� ����.
			INT32 curAI = pAIUpperDual->getCurrnetAIStateIndex();
			getAnimContext()->setAIUpper_Left( pAIUpperDual);

			// �̰� ������
			// ������ Current�� �����ϴ� ������ �־� �ӽ÷� nullptr�� �ʱ�ȭ �մϴ�.
			pAIUpperDual->ResetCurrentAI();
			pAIUpperDual->SetEnable( true, false);

			// ���� AI State�� ������ ��� �ʱ�ȭ�� �ؼ� ������ ���� ��¥ ���� �ؾ� �մϴ�.
			if (nextai != -1)
			{
				REAL32 nextai_Timescale = pAIUpperDual->getAIState(nextai)->getTimeScale();
				pAIUpperDual->setCurrentAIState(nextai, 0.f, nextai_Timescale);
			}
			// �� �� �ϳ��� �����ؾ� �ҵ�.
			else if (curAI != -1)
			{
				REAL32 curAI_Timescale = pAIUpperDual->getAIState(curAI)->getTimeScale();
				REAL32 curAI_LocalTime = pAIUpperDual->getLocalTime();
				pAIUpperDual->setCurrentAIStateDirect(curAI, curAI_LocalTime, curAI_Timescale);
			}
		}
	}
	else
	{
		getAnimContext()->setAIUpper_Left( nullptr);
	}
}
#endif

void CGameCharaAnimControl::_DinoChangeAIContext( const char * pszPVState )
{
	char szAIUpperPath[ MAX_PATH] = "Upper:";

	i3AIContext * UpperRoot = nullptr;

	//	Upper AI Path
	i3::generic_string_cat( szAIUpperPath, pszPVState);							//	����

	i3AIContext * pAIUpper = m_pOwner->FindAI( szAIUpperPath);
	I3ASSERT( pAIUpper != nullptr);
	
	getAnimContext()->setAIUpper( pAIUpper);

	//	UpperRoot�� �������� m_pAIUpper�� Ȱ���ϰ� ���þ��� AIState�� Disable ��Ų��.
	UpperRoot = m_pOwner->FindAI( "Upper");
	I3ASSERT(UpperRoot != nullptr);

	UpperRoot->setGroupEnable( pAIUpper);

	getAnimContext()->setAIUpper_Left( nullptr);
}

void CGameCharaAnimControl::OnChangeAIContext( const char * pszPVState )
{
	if(m_pOwner->IsDino())
	{
		_DinoChangeAIContext(pszPVState);
	}
	else
	{
		_CharaChangeAIContext(pszPVState);
	}
}

bool CGameCharaAnimControl::PlayUpperReturn( void)
{	//	������ �������� Upper �ִϸ��̼����� ���ư���.

	TRACE_FUNC;

	//	��ü �ִϸ��̼ǰ� �´� Ÿ�� ��ũ�ؾ� �մϴ�.
	if (getAnimContext())
	{
		REAL32 fSyncTime = getAnimContext()->GetLowerSyncTime();

		if( getActionContext()->isLower( CHARA_LOWER_RUN) )		PlayUpper( ID_UPPER_MOVE, fSyncTime);		//	���� Move ��������
		else													PlayUpper( ID_UPPER_IDLE, fSyncTime);		//	���� Idle ��������
	}
	else
		TRACE("CGameCharaAnimControl::PlayUpperReturn getActionContext = nullptr\n");
	
	return true;
}

bool CGameCharaAnimControl::PlayBodyLowerSync( void)
{	
	//	��ü�� ��ũ ���� �÷��̸� ����Ѵ�.
	getAnimContext()->ChangeAnimSetLowerSync( m_pOwner->getViewType());

	return false;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Anim Callback
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHSND_ACTRATE_FACTER 10	//�߻��󵵰��

#define CHSND_ACTRATE_ACCESSORY 4	//��� �ޱ׶� �Ÿ��� �Ҹ�
#define CHSND_ACTRATE_RUSTLE	8	//���� �ν÷� �Ÿ��� �Ҹ�

void CGameCharaAnimControl::OnIdleRun( i3AIContext * pCtx, REAL32 tm)
{	
	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);
	PlayAutoReload();
}

void CGameCharaAnimControl::PlayAutoReload()
{

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) 
		|| m_pOwner->IsDino() == true) return;

	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();

	if( m_pOwner->isLocal() 
		&& pWeapon != nullptr
		&& pWeapon->getWeaponInfo()->GetGaugeReload() == 1)
	{
		WEAPON::eSTATE		nState = pWeapon->getWeaponState();
		//	�Ϲ� ����
		switch( nState)
		{
			// �߻� ��, �Ѿ��� ���� źâ�� ���� ������ �ϴ� ����
			//	�Ϲ� ���� Reload ����( LoadMagazine) �ϳ��� �����ϴ�.
		case  WEAPON::STATE_NEEDTO_LOADMAG:		
			{
				ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

				if( pWeapon->getWeaponInfo()->IsLoadMagazineReady() && pWeapon->isReadyToLoadMagazine() == false &&
					getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
					nLoadType = ACTION_LOAD_READY_MAG;

				bool enable_loadmag = true;
				if( i3::same_of<WeaponDualCIC*>(getWeaponContext()->getCurrent()))
					enable_loadmag = pWeapon->getFireOrder() != WEAPON::SECONDARY_FIRE;

				if( enable_loadmag)
					m_pOwner->Cmd_Reload(nLoadType);	

				return;
			}
			break;
			// �߻� ��, ������ ���־�� �ϴ� ����
		case WEAPON::STATE_NEEDTO_LOADBULLET :
			{
				if( pWeapon->IsFireBulletWeapon())
				{
					m_pOwner->Cmd_Reload( ACTION_LOAD_FIRE_BULLET);	
					return;
				}
				else
				{
					m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);	
					return;
				}
			}
			break;
		}
	}

}

void CGameCharaAnimControl::OnMoveRun( i3AIContext * pCtx, REAL32 tm)
{
	if( getAnimContext()->isEnableRunJump() == false)
	{
		REAL32 rRunElapseTime = getAnimContext()->getRunElapseTime();
		rRunElapseTime += tm;
		getAnimContext()->setRunElapseTime( rRunElapseTime);
	}
	else
	{
		
		
			REAL32 rRunJumpScale = getAnimContext()->getRunJumpAnimScale();
			REAL32 rJumpTime = 1.f;
			if( m_pOwner->getDBInfo()->GetJumpTime() > I3_EPS)
				rJumpTime = 1.f / m_pOwner->getDBInfo()->GetJumpTime();

			rRunJumpScale += tm * rJumpTime * 2.f;

			if( rRunJumpScale > 1.f)
				rRunJumpScale = 1.f;

			getAnimContext()->setRunJumpAnimScale( rRunJumpScale);

			REAL32 fScaleTime = 1.f;

			if( (getAnimContext()->getOldUpperID() == ID_UPPER_MOVE) && (rRunJumpScale > -0.65f) )
			{
				getAnimContext()->_Play_Anim_Upper( ID_UPPER_JUMP_START);
			}

			switch( getAnimContext()->getOldLowerID() )
			{
			case ID_LOWER_MOVE_FRONT :
				if( rRunJumpScale < -0.6f)		fScaleTime = 1.1f;
				else if( rRunJumpScale < 0.85f)	fScaleTime = 0.7f;
				else							fScaleTime = 0.7f;
				break;
			case ID_LOWER_MOVE_RIGHT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 1.6f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.6f;
				else								fScaleTime = 1.4f;
				break;
			case ID_LOWER_MOVE_FRONT_LEFT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 1.6f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.6f;
				else								fScaleTime = 1.4f;
				break;
			case ID_LOWER_MOVE_LEFT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 1.8f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.6f;
				else								fScaleTime = 1.3f;
				break;
			case ID_LOWER_MOVE_BACK :
				if( rRunJumpScale < -0.6f)			fScaleTime = 2.;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.6f;
				else								fScaleTime = 1.5f;
				break;
			case ID_LOWER_MOVE_FRONT_RIGHT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 1.7f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.55f;
				else								fScaleTime = 1.5f;
				break;
			case ID_LOWER_MOVE_BACK_LEFT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 2.f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.8f;
				else								fScaleTime = 1.8f;
				break;
			case ID_LOWER_MOVE_BACK_RIGHT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 2.f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.8f;
				else								fScaleTime = 1.8f;
				break;
			}

			if( getAnimContext()->getLowerTimeScale() != fScaleTime )
				getAnimContext()->SetLowerTimeScale( fScaleTime );
		
		
		if( m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) ) //	���� ������
			_ProcessJumpEnd();
			
	}
	PlayAutoReload();	
}

void CGameCharaAnimControl::OnAttackRun( i3AIContext * pCtx, REAL32 tm)
{
	if(m_pOwner->IsDino())
	{
		((CGameCharaDino*)((CGameCharaBase*)(m_pOwner)))->OnDinoAttackRun(pCtx, tm);

	//	if( m_pOwner->IsControlledDino())
		{
			m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);

			/*GlobalFunc::PB_TRACE("getLocalTimeMulSclae() %f tm %f getUpperActualDuration() %f getUpperDuration %f", pCtx->getLocalTimeMulSclae(), tm, getAnimContext()->getUpperActualDuration(), getAnimContext()->getUpperDuration());*/
			//	���� �ൿ���� ���ƿɴϴ�.. (Idle �̳� Move)
			if( (pCtx->getLocalTimeMulSclae() + tm > getAnimContext()->getUpperDuration()) )
			{
				//GlobalFunc::PB_TRACE("Cmd_Return_Upper");
				m_pOwner->Cmd_Return_Upper();
				m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE);
			}
		}
	}
	else
	{
		//if( !m_pOwner->isLocal() && !m_pOwner->isAI())	return;
		if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;
		WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
		if( pCurWeapon == nullptr)		return;
		// KEYWORD : WEAPON_CIC
		if( i3::same_of<WeaponDualCIC*>(pCurWeapon))
		{
			MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(pCurWeapon->GetMainWeapon());

			if( mainWeapon_DualCIC->getCurrentUseWeapon() == WEAPON_CIC_CURRENTUSE_LEFT)
			{	// ���� �޼չ��⿡ ���� �ڵ鸵���̶�� �޼չ��⿡ ���ؼ� ó��
				pCurWeapon = mainWeapon_DualCIC->getLeftWeapon();
				I3ASSERT( pCurWeapon != nullptr);
			}
		}
		WEAPON::eSTATE		nState = pCurWeapon->getWeaponState();

		CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
		I3ASSERT( pCurWeaponInfo != nullptr);

		if( i3::kind_of<WeaponMiniAxe*>(pCurWeapon) && pCurWeapon->getUsedWeapon() )
		{
			nState	= pCurWeapon->getUsedWeapon()->getWeaponState();
		}
		//	�߻��� ������ Idle ���·� ���ƿ´�.
		if( nState == WEAPON::STATE_FIRE )
			return;

		if( i3::kind_of<WeaponMiniAxe*>(pCurWeapon))
		{
			if( i3::kind_of<WeaponMiniAxe*>(pCurWeapon->getUsedWeapon()))
			{
				if( pCurWeapon->getUsedWeapon()->getLoadedBulletCount() <= 0)
				{
					pCurWeapon->ExtensionActivate( false);
					m_pOwner->Cmd_Only_ChangeWeaponAction();
					return;
				}
			}	
		}

		//WEAPON_CLASS_TYPE	nType = pCurWeaponInfo->GetTypeClass();

		// KEYWORD : WEAPON_CIC
		if( i3::same_of<WeaponDualCIC*>( pCurWeapon->getPairWeapon()))
		{
			// revision 59544 P99 & HAK RMB ������ (�ڵ�)���� �ȵǵ��� ����. hansoft.3655
			WeaponDualCIC* pCIC = static_cast<WeaponDualCIC*>(pCurWeapon->getPairWeapon());
			INT32 bullet_count = pCIC->getLoadedBulletCount() + pCIC->getTotalBulletCount();
			if( bullet_count > 0)
			{
				MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(pCIC->GetMainWeapon());
				mainWeapon_DualCIC->setCurrentUseWeapon( WEAPON_CIC_CURRENTUSE_RIGHT);
			}
		}

		if( m_pOwner->isLocal() || m_pOwner->is1PVProfile() )
		{
			// ���⿡ ���� ����� �����ϵ��� �Ѵ�.
			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);

			//	�Ϲ� ����
			switch( nState)
			{
				// �߻� ��, �Ѿ��� ���� źâ�� ���� ������ �ϴ� ����
				//	�Ϲ� ���� Reload ����( LoadMagazine) �ϳ��� �����ϴ�.
				case  WEAPON::STATE_NEEDTO_LOADMAG:	
					{
						ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

						if( pCurWeapon->getWeaponInfo()->IsLoadMagazineReady() && pCurWeapon->isReadyToLoadMagazine() == false &&
							getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
							nLoadType = ACTION_LOAD_READY_MAG;
						
						// revision 59589 P99 & HAK RMB ���� �� �ڵ� ���� ���� �ʵ��� ����.
						bool enable_loadmag = true;
						if( i3::same_of<WeaponDualCIC*>(pCurWeapon))
							enable_loadmag = pCurWeapon->getFireOrder() != WEAPON::SECONDARY_FIRE;
						
						m_pOwner->SetLoadMagChg(false);
						if( enable_loadmag)
							m_pOwner->Cmd_Reload(nLoadType);
						return;
					}
					break;
					// �߻� ��, ������ ���־�� �ϴ� ����
				case WEAPON::STATE_NEEDTO_LOADBULLET :
					{
						if( pCurWeapon->IsFireBulletWeapon())
						{
							m_pOwner->Cmd_Reload( ACTION_LOAD_FIRE_BULLET);	
							return;
						}
						else
						{
							m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);	
							return;
						}
					}
					break;
	
					//	���� �߻��մϴ�.
				case WEAPON::STATE_NEEDTO_FIRE:	
						m_pOwner->Cmd_Attack(); return;
						break;
					// ź���� ���� ���
				case WEAPON::STATE_EMPTY_MAGAZINE:
					{
						// ���߹��⸦ ��� ���̸� ������ ���⸦ ���� �� ��ü�ؾ��ϱ� ������ �̰����� ó���ϸ� �ȵ˴ϴ�. �ֹ��Ⱑ �� �� ������ ���� ����...
						if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && pCurWeaponInfo->GetTypeUsage() == WEAPON_SLOT_PRIMARY )
						{
							if ( m_pOwner->getMultiWeaponContext()->GetMultiWeaponCount() > 1)
							{
								GAMEEVENT::Write_Chara_DropWeapon(m_pOwner, pCurWeaponInfo->GetTypeUsage(), pCurWeapon->getLoadedBulletCount(), pCurWeapon->getLoadedBulletDualCount(), pCurWeapon->getTotalBulletCount());

								return;
							}
						}

						if( i3::same_of<WeaponKrissSuperV*>(pCurWeapon))
						{
							I3ASSERT( ((WeaponKrissSuperV*)pCurWeapon)->getDualWeapon() );
							if( !((WeaponKrissSuperV*)pCurWeapon)->isExtensionActivate() )
							{
								
	#if defined( USE_AUTO_DROPWEAPON)
								GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, pCurWeaponInfo->GetTypeUsage(), pCurWeapon->getLoadedBulletCount(), pCurWeapon->getLoadedBulletDualCount(), pCurWeapon->getTotalBulletCount() );
	#else
								m_pOwner->Cmd_ChangeWeapon( getWeaponContext()->getNextUsableWeaponSlot());
	#endif
							}
						}
						else if( i3::same_of<WeaponShotGunGenderWithDual*>(pCurWeapon) )
						{
							I3ASSERT( ((WeaponShotGunGenderWithDual*)pCurWeapon)->getDualWeapon() );
							if( !((WeaponShotGunGenderWithDual*)pCurWeapon)->isExtensionActivate() )
							{

#if defined( USE_AUTO_DROPWEAPON)
								GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, pCurWeaponInfo->GetTypeUsage(), pCurWeapon->getLoadedBulletCount(), pCurWeapon->getLoadedBulletDualCount(), pCurWeapon->getTotalBulletCount() );
#else
								m_pOwner->Cmd_ChangeWeapon( getWeaponContext()->getNextUsableWeaponSlot());
#endif
							}
						}
						else
						{
							//	����ź�̳� �������� ��� EMPTY�� 0�� ����� ��ü
							if( (pCurWeaponInfo->GetTypeUsage() >= WEAPON_SLOT_MELEE) )
							{
								INT32 nIdx = 0;
								
								// 0�� ���Ⱑ ������ ���� ��ȣ�� ����� ��ü
								while( !getWeaponContext()->checkUsableWeaponSlot( static_cast<WEAPON_SLOT_TYPE>(nIdx), false) )
								{
									if( nIdx < MAX_WEAPON_CHECK_SLOT_COUNT )	nIdx++;
									else										nIdx = 0;

									if( nIdx == m_pOwner->getWeaponContext()->getCurrentSlotNum())	//	�ѹ��� ����( �ٲ� ���Ⱑ ���ٸ�)
									{
										nIdx = -1;
										break;
									}
								}

								m_pOwner->Cmd_ChangeWeapon( static_cast<WEAPON_SLOT_TYPE>(nIdx));
	#if defined( USE_AUTO_DROPWEAPON)
							}
							else
							{
								GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, pCurWeaponInfo->GetTypeUsage(), pCurWeapon->getLoadedBulletCount(), pCurWeapon->getLoadedBulletDualCount(), pCurWeapon->getTotalBulletCount() );
							}
	#else
							}
							else
								m_pOwner->Cmd_ChangeWeapon( getWeaponContext()->getNextUsableWeaponSlot());
	#endif
						}
						return;
					}
					break;

				case WEAPON::STATE_NEEDTO_QUICKLOAD :	m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_QUICK);	return;
				case WEAPON::STATE_NEEDTO_REMOVESHELL :	m_pOwner->Cmd_Reload( ACTION_LOAD_REMOVE_SHELL);return;

				case WEAPON::STATE_NEEDTO_DROPWEAPON:
					{
						GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, pCurWeaponInfo->GetTypeUsage(), pCurWeapon->getLoadedBulletCount(), pCurWeapon->getLoadedBulletDualCount(), pCurWeapon->getTotalBulletCount() );
						
						WeaponRocketLauncher * pLauncher = i3::same_cast<WeaponRocketLauncher*>(pCurWeapon);
						if(pLauncher != nullptr)
							pLauncher->SetDropLauncher(true);
					}
					return;
			}
		}

		//2015_03_18 ENG_��뿵 �����̰� ���콺���� ���� �ȶ´ٸ� ���� �߻� �Ϸ� ó�� �ϸ� �ȵ˴ϴ�. (��, WAIT ���¿����� �Ϸ�ó�� �� �� �ֵ��� �մϴ�. 3���� ���� ���� ������ )
		UINT64 keyPress = getInputContext()->getKeyPressInput();
		if( (nState !=  WEAPON::STATE_WAIT && keyPress & GAME_KEY_ACTION_ATTACK) != 0 && pCurWeapon->getWeaponInfo()->GetAutoFireCount() == 0 )
			return;

		// ���� �߻� �Ϸ�
		//	���� �ൿ���� ���ƿɴϴ�.. (Idle �̳� Move)
		if( ( getAnimContext()->getUpperLocalTime() + tm > getAnimContext()->getUpperActualDuration()) )
		{
			switch(pCurWeaponInfo->GetTypeClass())
			{
			case WEAPON_CLASS_THROWING_GRENADE:
			case WEAPON_CLASS_THROWING_CONTAIN:
			case WEAPON_CLASS_THROWING_ATTACK:
			case WEAPON_CLASS_THROWING_HEAL:
				{
					pCurWeapon->ReturnToReady();
					pCurWeapon->SetEnable( true);
					getActionContext()->setUpper( CHARA_UPPER_SWAPWEAPON);
					getAnimContext()->_Play_Anim_Upper( ID_UPPER_SWAP_WEAPON);
				}
				break;
			default:
				{
					if( m_pOwner->is1PV() && i3::same_of<CGameCamera_FPS*>(g_pCamera))
					{
						((CGameCamera_FPS*)g_pCamera)->SetNeckAnim(false);
					}

					bool returnupper = false;
					if (m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx())
					{
						if (nState != WEAPON::STATE_FIRING) returnupper = true;
					}
					else
					{
						if (nState != WEAPON::STATE_FIRING)
						{
							if (m_pOwner->IsNetworkAttackReturnUpper())
							{
								returnupper = true;
								m_pOwner->ResetNetworkAttackCount();
							}
							else
							{
								m_pOwner->NetworkAttackCountPlus(tm);
							}
						}
					}

					if(returnupper)
						m_pOwner->Cmd_Return_Upper();
				}
				break;
			}
		}
	}
}

void CGameCharaAnimControl::OnAttackFinish( i3AIContext * pCtx, REAL32 tm)
{
}

void CGameCharaAnimControl::OnAttackReadyFinish( i3AIContext * pCtx, REAL32 tm)
{
	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);		//	�غ� ������ �ٷ� �߻� ����
}

void CGameCharaAnimControl::OnJumpStartRun( i3AIContext * pCtx, REAL32 tm)
{
	//if( !getActionContext()->isLower( CHARA_LOWER_JUMP))
	//	return;
	
	if( !m_pOwner->isLocal() && getAnimContext()->isEnableRunJump() )
	{
		ID_LOWER_AI nId = getAnimContext()->getMoveIdByDir( getMoveContext()->getMoveDir());
		REAL32 rSyncTime = 0.f;
		if( getAnimContext()->getRunJumpStepTime() > 0.1f && getAnimContext()->getRunJumpStepTime() < 0.4f)
		{
			rSyncTime = 0.31f;
		}

		getAnimContext()->_Play_Anim_Lower( nId, rSyncTime);

		if( (getAnimContext()->getOldUpperID() == ID_UPPER_JUMP_START) )
		{
			getAnimContext()->_Play_Anim_Upper( ID_UPPER_MOVE);
		}
	}

		if( m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR))	//	���� ������
		_ProcessJumpEnd();
}

void CGameCharaAnimControl::OnJumpEndRun( i3AIContext * pCtx, REAL32 tm)
{
	if( getAnimContext()->getUpperLocalTime() + tm >= getAnimContext()->getUpperDuration())
	{
		// revision 33110
		if(m_pOwner->IsDino())
		{
			m_pOwner->Cmd_Return_Upper();
			m_pOwner->Cmd_Return_Lower();
		}
		else
		{
			//	���� �����̾��� Upper�� Idle �Ǵ� Move�� ���ƿ´�.
			// revision 31983
			WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
			// revision 32656
			//pumpaction weapons�� ������ ����� �ٸ��ѱ���� �޶� �����ִϸ��̼��� ��ü�� �ʱ�ȭ ��ų��� ���� ���������� ���� �ʾ� ����ó���մϴ�. 
			if( pCurWeapon != nullptr && pCurWeapon->getWeaponInfo()->GetPumpAction())
			{
				m_pOwner->Cmd_Return_Lower();
			}
			else
			{
				m_pOwner->Cmd_Return_Upper();
				m_pOwner->Cmd_Return_Lower();
			}
		}
	}
}

void CGameCharaAnimControl::OnJumpEndFinish( i3AIContext * pCtx, REAL32 tm)
{
	//	�ٸ� �������� ��ȯ�� �ִϸ��̼��� �ٸ������ ���� �����̸� ���� �������� ���ư��� �ʴ´�.		
	if( getAnimContext()->getUpperLocalTime() + tm >= getAnimContext()->getUpperDuration())
	{
		// revision 33110
		if(m_pOwner->IsDino())
		{
			m_pOwner->Cmd_Return_Upper();
			m_pOwner->Cmd_Return_Lower();
		}
		else
		{
			//	���� �����̾��� Upper�� Idle �Ǵ� Move�� ���ƿ´�.
			// revision 31983
			//M1887���� �ѱ�� ������ ����� �ٸ��ѱ���� �޶� �����ִϸ��̼��� ��ü�� �ʱ�ȭ ��ų��� ���� ���������� ���� �ʾ� ����ó���մϴ�. 
			WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
			if( pCurWeapon != nullptr
				&& pCurWeapon->getWeaponInfo()->GetPumpAction() )
			{
				m_pOwner->Cmd_Return_Lower();
			}
			else
			{
				m_pOwner->Cmd_Return_Upper();
				m_pOwner->Cmd_Return_Lower();
			}
		}
	}
}

void CGameCharaAnimControl::OnLoadMagazineRun( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pWeapon == nullptr)
		return;

	if( i3::same_of<WeaponKrissSuperV*>(pWeapon))
	{
		if( ((WeaponKrissSuperV*)pWeapon)->isUsedDualWeapon())
		{	
			if( ((WeaponKrissSuperV*)pWeapon)->getDualWeapon()->getWeaponState() == WEAPON::STATE_NEEDTO_LOADBULLET )
			{
				((WeaponKrissSuperV*)pWeapon)->SetEnableDualWeapon( false);
			}
		}
	}

	switch( pWeapon->getWeaponState())
	{
		case  WEAPON::STATE_NEEDTO_LOADMAG:
			{
				ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

				if( pWeapon->getWeaponInfo()->IsLoadMagazineReady() && pWeapon->isReadyToLoadMagazine() == false && 
					getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
					nLoadType = ACTION_LOAD_READY_MAG;

				m_pOwner->Cmd_Reload(nLoadType);
			}
			break;

		case WEAPON::STATE_NEEDTO_LOADBULLET :
			m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);
			break;

		case WEAPON::STATE_NEEDTO_REMOVESHELL :
			break;

		case WEAPON::STATE_READY:
			{	
				//	���� �ܿ��� �Ϲ� ����� Idle ���·� ���ư��� �ϱ� ������
				//	OnLoadBullet�� ȣ������ �ʱ� ������ (AIState�� �����Ƿ�) ȣ���ؾ� �մϴ�.
				OnLoadBulletRun( pCtx, tm);	
			}
			break;
		case WEAPON::STATE_NEEDTO_LOAD_B_MAG:
			//GlobalFunc::PB_TRACE("CGameCharaAnimControl::OnLoadMagazineRun WEAPON::STATE_NEEDTO_LOAD_B_MAG\n");
			m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_B);
			break;
		default :
			break;
	}
}

void CGameCharaAnimControl::OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;

	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pWeapon == nullptr)		return;

	//	������ ������
	if( pWeapon->getWeaponState() == WEAPON::STATE_READY )
	{
		// KEYWORD : WEAPON_GRENADELAUNCHER
		if( i3::kind_of<WeaponGrenadeLauncher*>(pWeapon)
			&& pWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER
			&& pWeapon->isExtensionActivate() )
			getActionContext()->ApplyActionBody2( CHARA_BODYUPPER_GRENADE);	
		// KEYWORD : WEAPON_SHOTGUNLAUNCHER
		else if( i3::kind_of<WeaponShotGunLauncher*>(pWeapon)
			&& pWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER
			&& pWeapon->isExtensionActivate() )
			getActionContext()->ApplyActionBody2( CHARA_BODYUPPER_SHOTGUN);	
		else
			getActionContext()->ApplyActionBody2( CHARA_BODYUPPER_ATTACKIDLE);		//	Idle ���´� AttackIdle ���·� �Ѵ�.

		if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM ) )
			m_pOwner->Cmd_Extension( false);
	
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE 
			| CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_ZOOM | CHARA_STATE_DISABLE_WEAPONCHANGE);

		//	���� �ൿ���� ���ƿɴϴ�.. (Idle �̳� Move)
		m_pOwner->Cmd_Return_Upper();
	}
}

void CGameCharaAnimControl::OnFireBulletRun( i3AIContext * pCtx, REAL32 tm)
{
	OnLoadBulletRun( pCtx, tm);
}

void CGameCharaAnimControl::OnWeaponSwapRun( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) ) return;

	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pWeapon == nullptr)		return;

	//	�ٸ� �������� ��ȯ�� �ִϸ��̼��� �ٸ������ ���� �����̸� ���� �������� ���ư��� �ʴ´�.		
	//	������ ������.
	if( getAnimContext()->getUpperLocalTime() > getAnimContext()->getUpperActualDuration())
	{
		//	���� �����̾��� Upper�� Idle �Ǵ� Move�� ���ƿ´�.
		m_pOwner->Cmd_Return_Upper();
		
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
	}
	else	//	������
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);

	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);
}

void CGameCharaAnimControl::OnDamageRun( i3AIContext * pCtx, REAL32 tm)
{
	if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE) ||	m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;

	//	�ٸ� �������� ��ȯ�� �ִϸ��̼��� �ٸ������ ���� �����̸� ���� �������� ���ư��� �ʴ´�.
	if( getAnimContext()->getUpperLocalTime() >= getAnimContext()->getUpperDuration())
	{
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE
			| CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);

		//	���� �ൿ���� ���ƿɴϴ�.. (Idle �̳� Move)
		m_pOwner->Cmd_Return_Upper();
	}
}

void CGameCharaAnimControl::OnDeathFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	if( g_pFramework->IsBattleStage() == false)
		return;

	//	������ Ÿ�̸� ����
	if( CStageInfoUtil::IsEternerlRespawn(*(MyRoomInfoContext::i()->getStage())) )
	{
		REAL32 rRespawnTime = 0.0f;
			
		if( m_pOwner->isAI())	//	AI�� ��� ���� ������ ���� ���
		{
			rRespawnTime = m_pOwner->getDBInfo()->GetNPCRespawnTime();
		}
		else
		{
			rRespawnTime = MyRoomInfoContext::i()->GetRespawnTime( m_pOwner->getCharaNetIndex());
			
			//	�ڽ� �Ǵ� ������ �����ϴ� ĳ������ ��� �������� StageBattle���� RespawnDelayTime�� �����ϱ� ������
			//if( m_pOwner->isLocal() || m_pOwner->isNetwork())
			{
				const REAL32 fRespawnDelayTime = 2.0f;

				rRespawnTime += fRespawnDelayTime;
			}
		}

		m_pOwner->setRespawnElapseTime( rRespawnTime);
	}
}

void CGameCharaAnimControl::OnExtensionActivateRun( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;
	
	if( getAnimContext()->getUpperLocalTime() + tm > getAnimContext()->getUpperDuration())
	{
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		m_pOwner->Cmd_Return_Upper();
		m_pOwner->Cmd_ApplyCrosshair();
	}
}

void CGameCharaAnimControl::OnExtensionDeactivateRun( i3AIContext * pCtx, REAL32 tm)
{ 
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;

	if( getAnimContext()->getUpperLocalTime() + tm > getAnimContext()->getUpperDuration())
	{
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		m_pOwner->Cmd_Return_Upper();
		m_pOwner->Cmd_ApplyCrosshair();
	}
}

void CGameCharaAnimControl::OnUI_IdleBRun( i3AIContext * pCtx, REAL32 tm)
{
	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper == nullptr)
		return;

	i3AIState * pAIState = pAIUpper->getCurrentAIState();

	if( pAIState == nullptr)
		return;

	if( getAnimContext()->getUpperAniTimer() + tm > pAIState->getDuration())
	{
		m_pOwner->Cmd_Profile_Idle_Stand();
		getAnimContext()->setUpperAniTimer( 0.f);
	}
}

void CGameCharaAnimControl::OnUI_IdleBFinish( i3AIContext * pCtx, REAL32 tm)
{

	// �⺻ Idle�� ���ϴ�
	if (false == g_pFramework->IsBattleStage())	
	{
		m_pOwner->Cmd_Profile_Idle_Stand();
		getAnimContext()->setUpperAniTimer( 0.f);
	}
	else
	{
		if (g_pFramework->GetCurrentStage())
		{
			INT32 idx = m_pOwner->getCharaNetIndex();
			char szTrace[ 256] = { 0,};
			i3::snprintf( szTrace, _countof(szTrace) ,"�߸��� �ִϸ޴��̼�. ����:%s(%d), ", BattleSlotContext::i()->getNickForSlot( idx), idx );
			WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
			if( pWeapon != nullptr)
			{
				i3::rc_wstring wstrWeaponName;
				pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);
				i3::snprintf( szTrace, 256, "����:%s", wstrWeaponName);	// �ڵ�����ȯ�� ����Ѵ�.
			}
			I3TRACE( szTrace);
		}
		else
		{
			// CurrentStage�� nullptr�̴�. �������� ���� �� ȣ��Ǿ���.
			I3TRACE("[WARNING] CGameCharaAnimControl::OnUI_IdleAFinish() - currentStage is nullptr\n");			
		}
	}
}

void CGameCharaAnimControl::OnUI_IdleChangeRun( i3AIContext * pCtx, REAL32 tm)
{
	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper == nullptr)
		return;

	i3AIState * pAIState = pAIUpper->getCurrentAIState();

	if( pAIState == nullptr)
		return;

	if( getAnimContext()->getUpperAniTimer() + tm > pAIState->getDuration())
	{
		m_pOwner->Cmd_Profile_Idle_Stand();
		getAnimContext()->setUpperAniTimer( 0.f);
	}
}

bool CGameCharaAnimControl::PlayDinoLower( ID_LOWER_DINO_AI id, REAL32 timeStart, REAL32 timeScale)
{
	bool bRv = false;
	i3AIContext * pAILower = getAnimContext()->getAILower();

	// ó������ �ʱ�ȭ ���·� UNKNOWN���°� ���� �� �ֽ��ϴ�.
	// �⺻������ IDLE�� ������ ���ݴϴ�.
	if( (id <= ID_LOWER_DINO_UNKNOWN) || (id >= ID_LOWER_DINO_AI_MAX) )
		id = ID_LOWER_DINO_IDLE;

	//	���� ������ �������� �����Ѵ�.
	((CGameDinoAnimContext*)getAnimContext())->setOldDinoLowerID( id);

	if( pAILower != nullptr)
	{
		INT32 nAIIdx = 0;

		nAIIdx = ((CGameDinoAnimContext*)getAnimContext())->getDinoAILowerID( id);

		if (nAIIdx >= 0)
		{
			//GlobalFunc::PB_TRACE("nAIIdx %d", nAIIdx);
			pAILower->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	��ü AI ���� ����

			bRv = true;

#if defined( ENABLE_CHARA_ANIM_OUTPUT )
			{
				TRACE_FUNC;
				AnimDebug( pAILower, timeStart );
			}
#endif
		}
	}

	return bRv;
}
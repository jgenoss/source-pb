#include "pch.h"
#include "GameCharaBase.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "Character/GameDinoAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaMoveContext.h"
#include "GameCharaWeaponContext.h"
#include "WeaponDualHandGun.h"
#include "GlobalVariables.h"

#include "Weapon/WeaponUtil.h"
#include "Weapon/WeaponItemID.h"
#include "./Character/GameCharaAnimControl1PV.h"
#include "./Character/GameCharaAnimControl3PV.h"
#include "./Character/GameDinoAnimControl3PV.h"

#include "MainWeapon_Dual.h"
#include "AIDriver_Weapon.h"
#include "i3Base/string/ext/utf16_to_mb.h"

I3_CLASS_INSTANCE( CGameCharaAnimContext);

CGameCharaAnimContext::CGameCharaAnimContext() : m_fRunDuration(ANIMTIME_MOVE_DURATION)
{
	m_MaxWeaponNumber			= WEAPON::getItemCount( WEAPON_CLASS_ASSAULT);
	m_MaxDualWeaponNumber		= WEAPON::getItemCount( WEAPON_CLASS_DUALSMG);
}

CGameCharaAnimContext::~CGameCharaAnimContext()
{
	OnDestroy();

	I3MEM_SAFE_FREE(m_UpperBoneList);
	I3MEM_SAFE_FREE(m_LowerBoneList);
}

INT32 CGameCharaAnimContext::getAIUpperID( WEAPON_CLASS_TYPE weaponClass, WEAPON::RES_ID nWeaponNum, ID_UPPER_AI ai)
{
	I3ASSERT(!m_pOwner->IsDino());
	return g_pCharaManager->getAIUpperID( m_pOwner->getViewType(), weaponClass, nWeaponNum, getActionContext()->getBodyUpper(), ai);
}

INT32 CGameCharaAnimContext::getAIUpperDualID( WEAPON::RES_ID nWeaponNum, ID_UPPER_DUAL_AI ai)
{
	I3ASSERT(!m_pOwner->IsDino());
	return g_pCharaManager->getAIUpperDualID( m_pOwner->getViewType(), nWeaponNum, ai);
}

ID_LOWER_AI CGameCharaAnimContext::getOldLowerID( void)
{
	I3ASSERT( m_pOwner->IsDino() == false);
	return m_nOldLowerId;
}

ID_UPPER_AI CGameCharaAnimContext::getOldUpperID( void)
{
	I3ASSERT( m_pOwner->IsDino() == false);
	return m_nOldUpperId;
}

INT32 CGameCharaAnimContext::getAILowerID( CHARA_ACTION_BODYLOWER body, ID_LOWER_AI ai)
{
	return g_pCharaManager->getAILowerID( body, ai);
}

void CGameCharaAnimContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	m_pCurrentAnimControl = nullptr;
	m_p1PVAnimControl = CGameCharaAnimControl1PV::new_object();
	m_p1PVAnimControl->BindOwner( pOwner);

	if( pOwner->IsDino())
		m_p3PVAnimControl = CGameDinoAnimControl3PV::new_object();
	else
		m_p3PVAnimControl = CGameCharaAnimControl3PV::new_object();

	m_p3PVAnimControl->BindOwner( pOwner);

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);

	if(m_UpperBoneList != nullptr || m_LowerBoneList != nullptr)
	{
		I3ASSERT_0;
	}

	INT32 nUpperBoneCount = pOwner->_GetUpperBoneCount();
	INT32 nLowerBoneCount = pOwner->_GetLowerBoneCount();

	I3_BOUNDCHK( nUpperBoneCount, 128);
	I3_BOUNDCHK( nLowerBoneCount, 128);

	m_UpperBoneList = (INT8*)i3MemAlloc(sizeof(INT8)*nUpperBoneCount); 
	m_LowerBoneList = (INT8*)i3MemAlloc(sizeof(INT8)*nLowerBoneCount); 
}

void CGameCharaAnimContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();

	m_pCurrentAnimControl = nullptr;
	I3_SAFE_RELEASE( m_p1PVAnimControl );
	I3_SAFE_RELEASE( m_p3PVAnimControl );
}

void CGameCharaAnimContext::OnReset()
{
	setAILower( nullptr);
	setAIUpper( nullptr);
	setOldLowerID( ID_LOWER_UNKNOWN);
	setOldUpperID( ID_UPPER_UNKNOWN);
	setAIUpper_Left( nullptr);
	setOldUpperDualRightID( ID_UPPER_DUAL_UNKNOWN);
	setOldUpperDualLeftID( ID_UPPER_DUAL_UNKNOWN);

	m_fUpperAniTimer			= 0.f;

	m_fLeftStepTime				= 0.0f;
	m_fRightStepTime			= 0.0f;

	m_nAccessory				= 0;
	m_fRunElapseTime			= 0.f;
	m_fRunDuration				= ANIMTIME_MOVE_DURATION;	// 0���� �ʱ�ȭ �ϸ� �ȵ˴ϴ�. by ������ 2009.04.23 3��Ī ���ڱ� �Ҹ�

	
	m_rLowerJumpStepTime		= 0.f;
	m_rJumpUpperPoseChangeTime	= 0.f;
	m_rLowerAnimScale			= 1.f;
	m_rUpperAnimScale			= 1.f;
	setEnableRunJump( false);
	setUpperWalk( false);
}

void CGameCharaAnimContext::OnChange1PV()
{
	if( !m_pOwner->isNPC())
		m_pCurrentAnimControl = m_p1PVAnimControl;
}

void CGameCharaAnimContext::OnChange3PV()
{
	CGameCharaContextBase::OnChange3PV();

	if( !m_pOwner->isNPC())
		m_pCurrentAnimControl = m_p3PVAnimControl;
}

void CGameCharaAnimContext::OnChangeLocal()
{
	CGameCharaContextBase::OnChangeLocal();

	if( m_pOwner->is1PV())
		m_pCurrentAnimControl = m_p1PVAnimControl;
	else
		m_pCurrentAnimControl = m_p3PVAnimControl;
}

void CGameCharaAnimContext::OnChangeNetwork()
{
	CGameCharaContextBase::OnChangeNetwork();

	if( m_pOwner->is1PV())
		m_pCurrentAnimControl = m_p1PVAnimControl;
	else
		m_pCurrentAnimControl = m_p3PVAnimControl;
}

void CGameCharaAnimContext::OnChangeNPC()
{
	CGameCharaContextBase::OnChangeNPC();
}

void CGameCharaAnimContext::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaContextBase::OnUpdate( rDeltatime);
}

void CGameCharaAnimContext::BindBoneList()
{
	i3Body * pBody = m_pOwner->getSceneObject()->GetBody();

	if( pBody == nullptr )
		return;

	if( m_pOwner->getCharaInfo()->GetType() == CHARA::TYPE_NPC)
		return;

#ifdef DOMI_DINO
	if (m_pOwner->getCharaInfo()->GetType() == CHARA::TYPE_DOMI_FULLBODY_AI)
		return;
#endif

	for( INT32 i = 0; i < CHARA_LOWER_BONE_COUNT; i++ )
	{
		m_LowerBoneList[i] = static_cast<INT8>(pBody->FindBoneByName( g_szLowerBoneName[i] ));
		I3ASSERT( m_LowerBoneList[ i ] != -1 );
	}

	for( INT32 i = 0; i < CHARA_UPPER_BONE_COUNT; i++)
	{
		m_UpperBoneList[ i] = static_cast<INT8>(pBody->FindBoneByName( g_szUpperBoneName[ i]));
		I3ASSERT( m_UpperBoneList[ i] != -1);
	}
}


REAL32 CGameCharaAnimContext::GetUpperSyncTime( void)
{
	i3Body * pBody = m_pOwner->getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);

	REAL32 fSyncTime = 0.0f;
	//	���� AI State�� ������. (�ٲ�� AI�� NextAI�� ��ϵǰ� ���� �����ӿ� ����Ǿ� AI�� ����˴ϴ�.)
	if( isAIUpper())
	{
		if( getAIUpper()->getNextAIStateIndex() == -1)
		{
			//	��ü �ִϸ��̼ǰ� �´� Ÿ�� ��ũ�ؾ� �մϴ�.			
			fSyncTime = pBody->getPlayTime( getBoneContext()->getBoneIndex( BONE_IK));
		}
	}

	return fSyncTime;
}

REAL32 CGameCharaAnimContext::GetLowerSyncTime( void)
{
	i3Body * pBody = m_pOwner->getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);

	REAL32 fSyncTime = 0.0f;
	if( isAILower())
	{
		//	���� AI State�� ������. (�ٲ�� AI�� NextAI�� ��ϵǰ� ���� �����ӿ� ����Ǿ� AI�� ����˴ϴ�.)
		if( m_pAILower->getNextAIStateIndex() == -1)
		{
			//	��ü �ִϸ��̼ǰ� �´� Ÿ�� ��ũ�ؾ� �մϴ�.			
			fSyncTime = pBody->getPlayTime( getBoneContext()->getBoneIndex( BONE_PELVIS));
		}
	}	

	return fSyncTime;
}

ID_LOWER_AI CGameCharaAnimContext::getMoveIdByDir( VEC3D * vDir)
{
	//char * pszAIName = "Idle";	
	
	// ��Ʈ��ũ ������ ���� ���� ������ �־����ϴ� by ������

	if( vDir->z > NET_MOVE_DIR_RANGE_VAL)	// 0.f)
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_MOVE_FRONT_LEFT;	//pszAIName = "FrontLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_MOVE_FRONT_RIGHT;	//pszAIName = "FrontRight";
		else											return ID_LOWER_MOVE_FRONT;	//pszAIName = "Front";
	}
	else if( vDir->z < -NET_MOVE_DIR_RANGE_VAL) //0.f) 
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_MOVE_BACK_LEFT;	//pszAIName = "BackLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_MOVE_BACK_RIGHT;	//pszAIName = "BackRight";
		else											return ID_LOWER_MOVE_BACK;	//pszAIName = "Back";
	}
	else if (vDir->y > NET_MOVE_DIR_RANGE_VAL)			// ���⼭ y�� z�� �����ϰ� ó�����ش�..
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_MOVE_LEFT;	//pszAIName = "FrontLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_MOVE_RIGHT;	//pszAIName = "FrontRight";
//		else if (m_pOwner && m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) == false) return ID_LOWER_MOVE_FRONT;
		else return ID_LOWER_MOVE_FRONT;
	}
	else if (vDir->y < -NET_MOVE_DIR_RANGE_VAL)
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_MOVE_LEFT;	//pszAIName = "BackLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_MOVE_RIGHT;	//pszAIName = "BackRight";
//		else if (m_pOwner && m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) == false) return ID_LOWER_MOVE_BACK;	//pszAIName = "Back";
		else return ID_LOWER_MOVE_BACK;
	}
	else
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_MOVE_LEFT;	//pszAIName = "Left";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_MOVE_RIGHT;	//pszAIName = "Right";
	}
	
	
	//return pszAIName;
	return ID_LOWER_IDLE;
}

ID_LOWER_AI CGameCharaAnimContext::getWalkIdByDir( VEC3D * vDir)
{
	I3ASSERT( m_pOwner->IsDino() == false);

	// ��Ʈ��ũ ������ ���� ���� ������ �־����ϴ� by ������
	if( vDir->z > NET_MOVE_DIR_RANGE_VAL)	// 0.f)
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_WALK_FRONT_LEFT;	//pszAIName = "FrontLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_WALK_FRONT_RIGHT;	//pszAIName = "FrontRight";
		else											return ID_LOWER_WALK_FRONT;	//pszAIName = "Front";
	}
	else if( vDir->z < -NET_MOVE_DIR_RANGE_VAL) //0.f) 
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_WALK_BACK_LEFT;	//pszAIName = "BackLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_WALK_BACK_RIGHT;	//pszAIName = "BackRight";
		else											return ID_LOWER_WALK_BACK;	//pszAIName = "Back";
	}
	else if( vDir->y > NET_MOVE_DIR_RANGE_VAL)	// 0.f)	// ���⼭ y�� z�� �����ϰ� ó�����ش�..
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_WALK_LEFT;	//pszAIName = "FrontLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_WALK_RIGHT;	//pszAIName = "FrontRight";
//		else if (m_pOwner && m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) == false) return ID_LOWER_WALK_FRONT;
		else return ID_LOWER_WALK_FRONT;
	}
	else if( vDir->y < -NET_MOVE_DIR_RANGE_VAL) //0.f) 
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_WALK_LEFT;	//pszAIName = "BackLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_WALK_RIGHT;	//pszAIName = "BackRight";
//		else if (m_pOwner && m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) == false) return ID_LOWER_WALK_BACK;
		else return ID_LOWER_WALK_BACK;
	}
	else
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_WALK_LEFT;	//pszAIName = "Left";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_WALK_RIGHT;	//pszAIName = "Right";
	}
	
	return ID_LOWER_IDLE;
}



void CGameCharaAnimContext::SetLowerTimeScale( REAL32 scale)
{	
	i3Body * pBody = m_pOwner->getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);

	INT32 iBoneCount = CHARA_LOWER_BONE_COUNT;
	if( m_pOwner->IsDino())
	{
		iBoneCount = CHARA_LOWER_DINO_BONE_COUNT;
	}

	for( INT32 i = 0; i < iBoneCount; i++ )
	{
		pBody->setBoneTimeScale( getLowerBoneID( i), scale );
	}

	if( isAILower())
	{
		m_pAILower->setCurrentAITimeScale( scale);
	}

	m_rLowerAnimScale = scale;
}

void CGameCharaAnimContext::SetUpperTimeScale( REAL32 scale)
{
	i3Body * pBody = m_pOwner->getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);
	INT32 iBoneCount = 0;
	iBoneCount = m_pOwner->_GetUpperBoneCount();

	for( INT32 i = 0; i < iBoneCount; i++ )
		pBody->setBoneTimeScale( getUpperBoneID( i), scale);

	if( m_pAIUpper != nullptr )
	{
		m_pAIUpper->setCurrentAITimeScale( scale);
		i3AIState * pAI = m_pAIUpper->getCurrentAIState();
		if( pAI != nullptr)	pAI->setTimeScale( scale);
	}

	m_rUpperAnimScale = scale;
}

void CGameCharaAnimContext::SetUpperLeftTimeScale( REAL32 scale)
{
	INT32 i;
	i3Body * pBody = m_pOwner->getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);

	for( i = 0; i < CHARA_UPPER_BONE_BODY_COUNT; i++ )
		pBody->setBoneTimeScale( getUpperBoneID( i), scale);

	for( i = CHARA_UPPER_BONE_RIGHT_COUNT; i < CHARA_UPPER_BONE_COUNT; i++)
		pBody->setBoneTimeScale( getUpperBoneID( i), scale);

	if( m_pAIUpper_Left != nullptr )
	{
		m_pAIUpper_Left->setCurrentAITimeScale( scale);
		i3AIState * pAI = m_pAIUpper_Left->getCurrentAIState();
		if( pAI != nullptr)	pAI->setTimeScale( scale);
	}
}

void CGameCharaAnimContext::SetUpperRightTimeScale( REAL32 scale)
{
	INT32 i;
	i3Body * pBody = m_pOwner->getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);

	for( i = 0; i < CHARA_UPPER_BONE_BODY_COUNT + CHARA_UPPER_BONE_RIGHT_COUNT; i++ )
		pBody->setBoneTimeScale( getUpperBoneID( i), scale);

	if( m_pAIUpper != nullptr )
	{
		m_pAIUpper->setCurrentAITimeScale( scale);
		i3AIState * pAI = m_pAIUpper->getCurrentAIState();
		if( pAI != nullptr)	pAI->setTimeScale( scale);
	}
}

void CGameCharaAnimContext::ChangeAnimSet( CHARA_VIEW_TYPE type, bool bAnimSync)
{
	I3ASSERT( m_pCurrentAnimControl != nullptr);

	bool bNeedStopAnim = (m_pOwner->getOldViewType() != type);
	
	ChangeAnimationContext();
		
	if( bAnimSync)
	{
		//	�ݵ�� ANIM_Stop�ϱ� ���� �÷��� �ð��� �޾� ���ƾ� �Ѵ�. stop�� �ϸ� Local Time�� 0 �� �ǹǷ�...����
		REAL32 fLowerSyncTime = GetLowerSyncTime();
		REAL32 fUpperSyncTime = GetUpperSyncTime();
		
		if( bNeedStopAnim )
			m_pCurrentAnimControl->PlayStop();

		//	������ ����� ��� �ִϸ��̼� ��Ʈ ��ü�� ����Ǳ� ������ ���� ��ũ�� ���� �÷����ؾ� �Ѵ�.
		//	�Ȱ��� �������� �������� 1��Ī�̳� 3��Ī�̳Ŀ� ���� ���� �ٸ� �ִϸ��̼� �����̱� �����̴�.
		if (m_pOwner->IsDino())
		{
			m_pCurrentAnimControl->PlayDinoLower( ((CGameDinoAnimContext*)this)->getOldDinoLowerID(), fLowerSyncTime);
			m_pCurrentAnimControl->PlayDinoUpper( ((CGameDinoAnimContext*)this)->getOldDinoUpperID(), fUpperSyncTime);
		}
		else
		{
			m_pCurrentAnimControl->PlayLower( getOldLowerID(), fLowerSyncTime);

			WeaponBase * pWeapon = getUsedWeapon();

			if( pWeapon == nullptr) return;

			if( pWeapon->isDualGun() )
			{
				// ���ÿ� �����ϴ� ����� ��� ����ó��
				if( (getOldUpperID() == ID_UPPER_LOAD_MAGAZINE) 
					|| (getOldUpperID() == ID_UPPER_LOAD_MAGAZINE2) 
					|| (getOldUpperID() == ID_UPPER_LOAD_BULLET) 
					|| (getOldUpperID() == ID_UPPER_FIRE_BULLET) 
					|| (getOldUpperID() == ID_UPPER_SWAP_WEAPON) 
					|| (getOldUpperID() >= ID_UPPER_DAMAGE_FRONT_LEFT_A) )
				{
					m_pCurrentAnimControl->PlayUpper( getOldUpperID(), fUpperSyncTime);
				}
				else
				{
					ID_UPPER_DUAL_AI rightAI = getOldUpperDualRightID();
					if( rightAI == ID_UPPER_DUAL_UNKNOWN )
						rightAI = ID_UPPER_DUAL_IDLE_RIGHT;
					m_pCurrentAnimControl->PlayUpperDual( rightAI, fUpperSyncTime);

					ID_UPPER_DUAL_AI leftAI = getOldUpperDualLeftID();
					if( leftAI == ID_UPPER_DUAL_UNKNOWN )
						leftAI = ID_UPPER_DUAL_IDLE_LEFT;
					m_pCurrentAnimControl->PlayUpperDual( leftAI, fUpperSyncTime);
				}
			}
			else
			{
				m_pCurrentAnimControl->PlayUpper( getOldUpperID(), fUpperSyncTime);
			}

			// ĳ���͸� 1, 3��Ī���� �ٲٴ� �������� ���� Animation�� �״�� �Ӵϴ�.
			// �׷��� Compound Bow ���� Idle Animation�� Ȯ���ϰ� �ٸ� ������� ���̰� ���� ����
			// Idle �ִϸ��̼��� �ٽ� �����ϰ� �Ǹ� �´� Type���� ��ȯ�˴ϴ�. Jinsik.kim
			pWeapon->_OnPlayWeaponIdleAnim();
		}
	}
}

void CGameCharaAnimContext::ChangeAnimSetLowerSync( CHARA_VIEW_TYPE type)
{
	I3ASSERT( m_pCurrentAnimControl != nullptr);

	bool bNeedStopAnim = (m_pOwner->getViewType() != type);
	
	m_pOwner->Cmd_ChangeView( type, false);
	
	ChangeAnimationContext();
	
	//	�ݵ�� ANIM_Stop�ϱ� ���� �÷��� �ð��� �޾� ���ƾ� �Ѵ�. stop�� �ϸ� Local Time�� 0 �� �ǹǷ�...����
	REAL32 fLowerSyncTime = GetLowerSyncTime();

	if( bNeedStopAnim )
		m_pCurrentAnimControl->PlayStop();

	//	������ ����� ��� �ִϸ��̼� ��Ʈ ��ü�� ����Ǳ� ������ ���� ��ũ�� ���� �÷����ؾ� �Ѵ�.
	//	�Ȱ��� �������� �������� 1��Ī�̳� 3��Ī�̳Ŀ� ���� ���� �ٸ� �ִϸ��̼� �����̱� �����̴�.
	m_pCurrentAnimControl->PlayLower( getOldLowerID(), fLowerSyncTime);
}

void CGameCharaAnimContext::SetAIStateImmediately( REAL32 tm)
{
	if( isAILower())
		m_pAILower->SetAIStateImmediately( tm);

	if( isAIUpper())
		m_pAIUpper->SetAIStateImmediately( tm);
}

void CGameCharaAnimContext::ChangeAnimationContext( void)
{
	I3ASSERT( m_pCurrentAnimControl != nullptr);
	m_pCurrentAnimControl->OnChangeAIContext();
}

// revision 36076 C4 ��ġ�� ���� ���� �� �� �߻� �����ϰ� C4 ��ġ ���尡 ��� play�Ǵ� ���� ����
void CGameCharaAnimContext::FinishCurrentAIUpper( REAL32 tm )
{
	if( m_pAIUpper != nullptr )
	{
		i3AIState*	pAIState	= m_pAIUpper->getCurrentAIState();

		if( pAIState != nullptr )
		{
			// revision 36093
			i3AIDriver*		pAIDriver	= m_pAIUpper->getAI()->getAIDriver();
			if( pAIDriver != nullptr)
			{
				I3_AISTATE_PROC pProc = pAIDriver->getFinishAIProc( m_pAIUpper->getCurrnetAIStateIndex() );
				if( pProc != nullptr)
					pProc( m_pAIUpper, tm );
			}
			pAIState->OnFinishState( m_pAIUpper, tm );

			// TimeSeq node�� �����Ѵ�.
			i3TimeSequence*		pTimeSeq	= pAIState->getTimeSeq();
			if( pTimeSeq != nullptr)
			{
				pTimeSeq->OnChangeTime( 0.0f);
				m_pAIUpper->getOwner()->getAttachNode()->RemoveChild( pTimeSeq );
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameCharaAnimContext::CallbackAnim_IdleRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnIdleRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_AttackRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnAttackRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_AttackFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnAttackFinish( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_AttackReadyFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnAttackReadyFinish( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_MoveRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnMoveRun( pCtx, rDeltatime);
}

namespace
{
	void ProcessKrissButtAnimation(CGameCharaBase* pOwner, WeaponBase* pWeapon, const REAL32 tm)
	{
		if( pWeapon == nullptr)
			return;

		// 2014-09-18 ���� ���� ������ OA���� ó���� �Ǿ��־� �ش� AI�� �����߽��ϴ�. ENG_��뿵
		if( pWeapon->GetAIContext() != nullptr &&
			i3::same_of<AIDriverGun_13PV_FoldedGun_OA*>(pWeapon->GetAIContext()->getAI()->getAIDriver()) )
			return;

		// �ϵ��ڵ�. ���� ���� �� ���Ӹ��� �������� �ִϸ��̼� ȣ�� ����. 
		// pef�� �� ���� ������ �� ������ ũ���� �迭�� �ƴ� �ٸ� �ѱ⿡�� ���Ӹ����� �������� �ϴ�
		// ��Ȳ�� ���� �� ��ġ�� �� ���� �� �ϴ�.
		if (i3::same_of<WeaponKrissSuperV*>(pWeapon) && !pWeapon->isExtensionActivate())
		{
			if (pOwner->getAnimContext()->getUpperLocalTime()+tm < 0.40f)
			{
				static_cast<WeaponKrissSuperV*>(pWeapon)->PlayIdleFoldedAnim();
			}
			else
			{
				static_cast<WeaponKrissSuperV*>(pWeapon)->PlayIdleAnim();
			}
		}
	}
}

void CGameCharaAnimContext::CallbackAnim_WeaponSwapRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnWeaponSwapRun( pCtx, rDeltatime);

	ProcessKrissButtAnimation(m_pOwner, m_pOwner->GetCurrentCharaWeapon(), rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_LoadBulletRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnLoadBulletRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_LoadBulletFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnLoadBulletFinish( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_FireBulletRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnFireBulletRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_FireBulletFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnFireBulletFinish( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_LoadMagazineRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnLoadMagazineRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_JumpStartRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnJumpStartRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_JumpEndRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnJumpEndRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_JumpEndFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnJumpEndFinish( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_DamageRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnDamageRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_DeathFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnDeathFinish( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_ExtensionActivateRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnExtensionActivateRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_ExtensionDeactivateRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnExtensionDeactivateRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_Profile_IdleARun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnUI_IdleARun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_Profile_IdleAFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnUI_IdleAFinish( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_Profile_IdleBRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnUI_IdleBRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_Profile_IdleBFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnUI_IdleBFinish( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_Profile_IdleChangeRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnUI_IdleChangeRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_Profile_IdleChangeFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnUI_IdleChangeFinish( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_Profile_IdleStandRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnUI_IdleStandRun( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_Profile_IdleStandFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pCurrentAnimControl->OnUI_IdleStandFinish( pCtx, rDeltatime);
}

void CGameCharaAnimContext::CallbackAnim_Profile_CeremonyRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	if( m_pCommon == nullptr)
		return;

	i3AIState * pAIState = m_pCommon->getCurrentAIState();

	if( pAIState == nullptr)
		return;

	if( m_pCommon->getLocalTime() + rDeltatime > pAIState->getDuration())
	{
		m_pCommon->StopAI( rDeltatime);
		m_pCommon->SetExclusiveEnable( false, false);
		m_pOwner->Cmd_Profile_Idle_Stand();
		m_fUpperAniTimer = 0.f;
	}
}

void CGameCharaAnimContext::CallbackAnim_Dual_Left_IdleRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE);
}

void CGameCharaAnimContext::CallbackAnim_Dual_Left_AttackRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;
	
	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr)		return;
	
	if( i3::kind_of<WeaponDualHandGun*, WeaponDualSMG*, WeaponDualShotGun*>(pWeapon) )
	{
		MainWeapon_Dual* mainWeapon_Dual = static_cast<MainWeapon_Dual*>(pWeapon->GetMainWeapon());
		pWeapon = mainWeapon_Dual->getLeftWeapon();
	}
	else
	{
		// 2008.12.29 ��Ʈ��ũ�� ���� ��ũ�� ���� �ʴ� ����Դϴ�. ����ó���ϰ� ���Ͻ�ŵ�ϴ�.
		getActionContext()->_Play_Upper_Return();
		return;
	}

	CWeaponInfo * pCurWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	//	�߻��� ������ Idle ���·� ���ƿ´�.
	if( pWeapon->getWeaponState() == WEAPON::STATE_FIRE )				return;

	// ���⿡ ���� ����� �����ϵ��� �Ѵ�.
	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE );


	CGameCharaActionContext * pActionCtx = getActionContext();
	CGameCharaAnimContext * pAnimCtx = getAnimContext();
	
	//	���� �ൿ���� ���ƿɴϴ�.. (Idle �̳� Move)
	i3AIContext * pAIUpper_Left = getAIUpper_Left();
	REAL32	rDuration = 0.f;

	if( pAIUpper_Left != nullptr)
	{
		i3AIState * pAIState = pAIUpper_Left->getCurrentAIState();
		rDuration = pAIState->getDuration();
	}

	if( pAnimCtx->getUpperLocalTime() + rDeltatime > rDuration )
	{
		if( getActionContext()->getUpperLeft() != CHARA_UPPER_JUMP)
		{
			if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
				pActionCtx->_Play_Dual_Run_Left();
			else
				pActionCtx->_Play_Dual_Idle_Left();
		}
	}
}

void CGameCharaAnimContext::CallbackAnim_Dual_Left_JumpStartRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
}

void CGameCharaAnimContext::CallbackAnim_Dual_Left_JumpEndFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr)		return;

	if( !i3::kind_of<WeaponDualHandGun*, WeaponDualSMG*, WeaponDualShotGun*>(pWeapon)
		/*&& !i3::kind_of<WeaponCrossFire*>(pWeapon) )*/
		)
	{
		//I3PRINTLOG(I3LOG_FATAL,  "�� ���� ���� ����.");
		// 2008.12.29 ��Ʈ��ũ�� ���� ��ũ�� ���� �ʴ� ����Դϴ�. ����ó���ϰ� ���Ͻ�ŵ�ϴ�.
		getActionContext()->_Play_Upper_Return();
		return;
	}

	// revision 45934 ��� �ѱ� ���� �� �����ϸ鼭 ������ �ϸ� ĳ���� ������ �ִϸ��̼��� �ȵǴ� ���� ���� - hansoft. 1520
	CHARA_ACTION_UPPER eUpper = getActionContext()->getUpperLeft();
	if( eUpper != CHARA_UPPER_FIRE && eUpper != CHARA_UPPER_LOADMAGAZINE && eUpper != CHARA_UPPER_SWAPWEAPON)
	{
		if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
			getActionContext()->_Play_Dual_Run_Left();
		else
			getActionContext()->_Play_Dual_Idle_Left();

		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE );
	}
}

void CGameCharaAnimContext::CallbackAnim_Dual_Right_IdleRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE);
}

// KEYWORD : WEAPON_DUALHANDGUN
void CGameCharaAnimContext::CallbackAnim_Dual_Right_AttackRun( i3AIContext * pCtx, REAL32 rDeltatime)
{	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;

	WeaponBase * pLeftWeapon = nullptr;
	WeaponBase * pWeapon = getUsedWeapon();

	if( pWeapon == nullptr)		return;

	if( i3::kind_of<WeaponDualHandGun*, WeaponDualSMG*, WeaponDualShotGun*>(pWeapon))
	{
		MainWeapon_Dual* mainWeapon_Dual = static_cast<MainWeapon_Dual*>(pWeapon->GetMainWeapon());
		pLeftWeapon = mainWeapon_Dual->getLeftWeapon();
	}
	else
	{
		//I3PRINTLOG(I3LOG_FATAL,  "�� ���� ���� ����.");
		// 2008.12.29 ��Ʈ��ũ�� ���� ��ũ�� ���� �ʴ� ����Դϴ�. ����ó���ϰ� ���Ͻ�ŵ�ϴ�.
		getActionContext()->_Play_Upper_Return();
		return;
	}

	CWeaponInfo * pCurWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	WEAPON::eSTATE		nState = pWeapon->getWeaponState();
	//	�߻��� ������ Idle ���·� ���ƿ´�.
	if( nState == WEAPON::STATE_FIRE )				return;

	CGameCharaActionContext * pActionCtx = getActionContext();
	CGameCharaAnimContext * pAnimCtx = getAnimContext();
	
	// ���⿡ ���� ����� �����ϵ��� �Ѵ�.
	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE );

	if( !m_pOwner->isNetwork() )
	{
		//	�Ϲ� ����
		switch( nState)
		{
		case  WEAPON::STATE_NEEDTO_LOADMAG:
			// �߻� ��, �Ѿ��� ���� źâ�� ���� ������ �ϴ� ����
			//	�Ϲ� ���� Reload ����( LoadMagazine) �ϳ��� �����ϴ�.
			m_pOwner->Cmd_Reload();
			return;

		case WEAPON::STATE_NEEDTO_LOADBULLET :
			// �߻� ��, ������ ���־�� �ϴ� ����
			m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);
			return;
		
		case WEAPON::STATE_NEEDTO_FIRE:
			//	���� �߻��մϴ�.
			m_pOwner->Cmd_Attack();
			return;

		case WEAPON::STATE_EMPTY_MAGAZINE:
			if( pLeftWeapon->getWeaponState() == WEAPON::STATE_EMPTY_MAGAZINE)
			{
				//��󹫱�� �Ѿ��� ��� �������� �ٸ������ ������ �ȵ����� �ͽ��ټ��� �ƴ� �⺻�� ����� ������ ������ ������ �Ѵ�
				if (i3::same_of<WeaponDualUzi*>(pWeapon) || i3::same_of<WeaponDualUziWithSilencer*>(pWeapon))
				{
					m_pOwner->Cmd_ChangeWeapon(getWeaponContext()->getNextUsableWeaponSlot());
					return;
				}
			}
			break;
		}
	}

	//	���� �ൿ���� ���ƿɴϴ�.. (Idle �̳� Move)
	if( pAnimCtx->getUpperLocalTime() + rDeltatime > pAnimCtx->getUpperDuration() )
	{
		if( getActionContext()->getUpper() != CHARA_UPPER_JUMP)
		{
			if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
				pActionCtx->_Play_Dual_Run_Right();
			else
				pActionCtx->_Play_Dual_Idle_Right();
		}
	}
}


void CGameCharaAnimContext::CallbackAnim_Dual_Right_JumpStartRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
}

void CGameCharaAnimContext::CallbackAnim_Dual_Right_JumpEndFinish( i3AIContext * pCtx, REAL32 rDeltatime)
{
	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr)		return;

	if( !i3::kind_of<WeaponDualHandGun*, WeaponDualSMG*, WeaponDualShotGun*>(pWeapon)
		/*&& !i3::kind_of<WeaponCrossFire*>(pWeapon))*/
		)
	{
		//I3PRINTLOG(I3LOG_FATAL,  "�� ���� ���� ����.");
		// 2008.12.29 ��Ʈ��ũ�� ���� ��ũ�� ���� �ʴ� ����Դϴ�. ����ó���ϰ� ���Ͻ�ŵ�ϴ�.
		getActionContext()->_Play_Upper_Return();
		return;
	}

	// revision 45934
	CHARA_ACTION_UPPER eUpper = getActionContext()->getUpper();

	if(eUpper != CHARA_UPPER_FIRE && eUpper != CHARA_UPPER_LOADMAGAZINE && eUpper != CHARA_UPPER_SWAPWEAPON)
	{
		//	���� �ൿ���� ���ƿɴϴ�.. (Idle �̳� Move)
		if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
		{	
			getActionContext()->_Play_Dual_Run_Right();
		}
		else
			getActionContext()->_Play_Dual_Idle_Right();

		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE );
	}
}

void CGameCharaAnimContext::CallbackAnim_Dual_SwapWeaponRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	if( !getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON) || m_pOwner->isCharaStateMask( CHARA_STATE_DEATH)) return;

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr)		return;

	if( m_pOwner->isNetwork() )
	{
		if( !i3::kind_of<WeaponDualHandGun*, WeaponDualSMG*, WeaponDualShotGun*>(pWeapon)
			/*&& !i3::kind_of<WeaponCrossFire*>(pWeapon))*/
			)
		{
			//I3PRINTLOG(I3LOG_FATAL,  "�� ���� ���� ����.");
			// 2008.12.29 ��Ʈ��ũ�� ���� ��ũ�� ���� �ʴ� ����Դϴ�. ����ó���ϰ� ���Ͻ�ŵ�ϴ�.
			getActionContext()->_Play_Upper_Return();
			return;
		}
	}

	//	�ٸ� �������� ��ȯ�� �ִϸ��̼��� �ٸ������ ���� �����̸� ���� �������� ���ư��� �ʴ´�.		
	//	������ ������.
	if( getAnimContext()->getUpperLocalTime() + rDeltatime > getAnimContext()->getUpperActualDuration())
	{
		//	���� �����̾��� Upper�� Idle �Ǵ� Move�� ���ƿ´�.
		if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
		{
			getActionContext()->_Play_Dual_Run_Left();
			getActionContext()->_Play_Dual_Run_Right();
		}
		else
		{
			getActionContext()->_Play_Dual_Idle_Left();
			getActionContext()->_Play_Dual_Idle_Right();
		}
		
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
	}
	else	//	������
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);

	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE);
}

void CGameCharaAnimContext::CallbackAnim_Dual_LoadMagazineRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr)		return;

	WeaponBase * pLeftWeapon = nullptr;
	if( i3::kind_of<WeaponDualHandGun*, WeaponDualSMG*, WeaponDualShotGun*>(pWeapon))
	{
		MainWeapon_Dual* mainWeapon_Dual = static_cast<MainWeapon_Dual*>(pWeapon->GetMainWeapon());

		pLeftWeapon = mainWeapon_Dual->getLeftWeapon();
	}
	else
	{
		//I3PRINTLOG(I3LOG_FATAL,  "�� ���� ���� ����.");
		// 2008.12.29 ��Ʈ��ũ�� ���� ��ũ�� ���� �ʴ� ����Դϴ�. ����ó���ϰ� ���Ͻ�ŵ�ϴ�.
		getActionContext()->_Play_Upper_Return();
		return;
	}

	if( pWeapon->getWeaponState() == WEAPON::STATE_READY && pLeftWeapon->getWeaponState() == WEAPON::STATE_READY )
	{
		//	���� �ܿ��� �Ϲ� ����� Idle ���·� ���ư��� �ϱ� ������
		//	OnLoadBullet�� ȣ������ �ʱ� ������ (AIState�� �����Ƿ�) ȣ���ؾ� �մϴ�.
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE 
			| CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_ZOOM | CHARA_STATE_DISABLE_WEAPONCHANGE);

		//	���� �ൿ���� ���ƿɴϴ�.. (Idle �̳� Move)
		if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
		{
			getActionContext()->_Play_Dual_Run_Left();
			getActionContext()->_Play_Dual_Run_Right();
		}
		else
		{
			getActionContext()->_Play_Dual_Idle_Left();
			getActionContext()->_Play_Dual_Idle_Right();
		}
	}
	
}


void CGameCharaAnimContext::CallbackAnim_LoadMagazine_ReadyRun( i3AIContext * pCtx, REAL32 rDeltatime)
{
	if( getAnimContext()->getUpperLocalTime() + rDeltatime > getAnimContext()->getUpperDuration() )
	{
		//	Next animation (LoadMag)
		getActionContext()->_Play_Upper_Reload( ACTION_LOAD_MAG);
	}
}

bool CGameCharaAnimContext::_Play_Anim_Upper( ID_UPPER_AI aiID,
											 REAL32 startTime /*= 0.f*/,
											 REAL32 scale /*= 1.f*/)
{
	I3ASSERT( m_pOwner->IsDino() == false);
	return m_pCurrentAnimControl->PlayUpper( aiID, startTime, scale) == true;
}

bool CGameCharaAnimContext::_Play_Anim_Lower( ID_LOWER_AI aiID,
											 REAL32 startTime /*= 0.f*/,
											 REAL32 scale /*= 1.f*/)
{
	I3ASSERT( m_pOwner->IsDino() == false);
	return m_pCurrentAnimControl->PlayLower( aiID, startTime, scale) == true;
}

bool CGameCharaAnimContext::_Play_Anim_DualUpper( ID_UPPER_DUAL_AI aiID,
												 REAL32 startTime /*= 0.f*/,
												 REAL32 scale /*= 1.f*/)
{
	I3ASSERT( m_pOwner->IsDino() == false);
	return m_pCurrentAnimControl->PlayUpperDual( aiID, startTime, scale) == true;
}

void CGameCharaAnimContext::_Stop_Anim( void)
{
	m_pCurrentAnimControl->PlayStop();
}

bool CGameCharaAnimContext::_Play_Body_Sync( void)
{
	return m_pCurrentAnimControl->PlayBodyLowerSync() == true;
}


bool CGameCharaAnimContext::_Play_Anim_Common( ID_COMMON_AI aiID)
{
	I3ASSERT( aiID < ID_COMMON_AI_MAX);

	if( m_pCommon != nullptr)
	{	
		INT32 nAIIdx = m_nAICommonId[ aiID];
		I3ASSERT( nAIIdx != -1);

		m_pCommon->SetExclusiveEnable( true, false);
		m_pCommon->setCurrentAIState( nAIIdx);
		m_fUpperAniTimer = 0.f;

		return true;
	}	

	return false;
}



void CGameCharaAnimContext::_PreloadAnimation( T_ItemID itemID)
{	// 0.6 ~ 18ms
	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( itemID);
	if( pWeaponInfo == nullptr)
		return;

	char * pszAnimSet = nullptr;

	i3AnimationResManager * pManager = static_cast<i3AnimationResManager*>(i3NamedResourceManager::FindByMeta( i3AnimationResManager::static_meta()));

	//	GetLinkedToCha	raAI �׸��� ���� ������ �߰��Ͽ� FindWeapon�� ����մϴ�. komet

	i3::rc_wstring wstrFindWeapon;
	pWeaponInfo->GetLinkedToCharaAI( wstrFindWeapon);
	
	i3::string strFindWeapon;		i3::utf16_to_mb(wstrFindWeapon, strFindWeapon);


	const char * pszWeaponType = WEAPON::GetClassName( WEAPON::ItemID2Class( itemID) );

	if( pszWeaponType == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "���� ������ �����ϴ�. Ȯ��!!!!");
		return;
	}

	i3AIContext * pAIUpper = static_cast<i3AIContext*>(m_pOwner->FindChildByName( "Upper", false));
	I3ASSERT( pAIUpper != nullptr);

	for( INT32 nAnimCnt = 0; nAnimCnt < CHARA_VIEW_MAX; nAnimCnt++)
	{
		if( nAnimCnt == CHARA_VIEW_1PV)			pszAnimSet = "1PV";
		else if( nAnimCnt ==  CHARA_VIEW_3PV)	pszAnimSet = "3PV";
		else
		{
			I3ASSERT_0;
		}

		i3AIContext * pAIAnimSetCtx = static_cast<i3AIContext*>(pAIUpper->FindChildByName( pszAnimSet, false));
		I3ASSERT( pAIAnimSetCtx != nullptr);

		i3AIContext * pAIWeaponTypeCtx = static_cast<i3AIContext*>(pAIAnimSetCtx->FindChildByName( pszWeaponType, false));
		if( pAIWeaponTypeCtx == nullptr)
			continue;

		i3AIContext * pAIWeaponCtx = static_cast<i3AIContext*>(pAIWeaponTypeCtx->FindChildByName( strFindWeapon.c_str() , false));
		if( pAIWeaponCtx == nullptr)
			continue;

		//	Body2
		for( INT32 nBodyState = 0; nBodyState < CHARA_BODYUPPER_MAXCOUNT; nBodyState++)
		{
			const char * pszBody2Name = m_pOwner->_GetWeaponBodyUpperName( (CHARA_ACTION_BODYUPPER) nBodyState, pWeaponInfo);

			if( pszBody2Name == nullptr)
				continue;

			i3AIContext * pAICtx = static_cast<i3AIContext*>(pAIWeaponCtx->FindChildByName( pszBody2Name, false));
			if( pAICtx == nullptr)	continue;
		
			INT32 stateCnt = pAICtx->getAIStateCount();
			for( INT32 i = 0; i < stateCnt; i++)
			{
				i3AIState * pAIState = pAICtx->getAIState( i);
				if( pAIState->getAnim() == nullptr)
				{	// Animation�� ����. Load
					if( pAIState->getAnimName()[0] != 0)
					{
						i3Animation * pAnim = (i3Animation*) pManager->GetResource( nullptr, nullptr, pAIState->getAnimName());
						if( pAnim != nullptr)
						{
							pAIState->SetAnim( pAnim);
						}
						else
						{
							g_pCharaManager->LoadWeaponAnimation( pAIState, pAIState->getAnimName());
						}
					}
				}
			}
		}
	}
}


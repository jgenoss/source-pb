#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaBase.h"
#include "GameCamera.h"
#include "StageBattle.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"


// ��Ʈ��ũ ������ ���� ���� ������ �־����ϴ� by ������
//#define NET_MOVE_DIR_RANGE_VAL			0.3826834323650897717284599840304f
/*
void CGameCharaBase::ACTION_apply( BOOL bFixed3PV)
{	
	CHARA_ANIMSET_TYPE AnimSet = getAnimSet();

	char szAIUpperPath[256] = "Upper:";
	char szAILowPath[256] = "Lower:";

	char * pszPVState = "3PV:";
	char * pszWeaponClass = NULL;
	
	char * pszStandState = "Stand:";
	char * pszIdleState = "AttackIdle";	

	i3AIContext * UpperRoot = NULL;
	WeaponBase * pWeapon = NULL;

	if( getWeaponContext()->getCurrentSlotNum() != getWeaponContext()->getSwapSlotNum())		//	���� �����ӿ� �ٲ� ������ ���
		pWeapon = getWeaponContext()->getWeapon( getWeaponContext()->getSwapSlotNum());
	else
		pWeapon = getWeaponContext()->getCurrent();

	if( pWeapon == NULL )
	{	// ���Ⱑ ������ ��ŵ�մϴ�. ��Ʈ��ũ ������ ���� �� �ֽ��ϴ�. - by dfly
		return;
	}
	
	switch( pWeapon->getWeaponInfo()->m_TypeClass)
	{
	case WEAPON_CLASS_KNIFE:	pszWeaponClass = "Knife:";			break;
	case WEAPON_CLASS_HANDGUN:	pszWeaponClass = "HandGun:";		break;
	case WEAPON_CLASS_ASSAULT:	pszWeaponClass = "AssaultRifle:";	break;
	case WEAPON_CLASS_SMG:		pszWeaponClass = "SMG:";			break;
	case WEAPON_CLASS_SNIPER:	pszWeaponClass = "SniperRifle:";	break;
	case WEAPON_CLASS_SHOTGUN:	pszWeaponClass = "ShotGun:";		break;
	case WEAPON_CLASS_THROWING:	pszWeaponClass = "Throwing:";		break;
	case WEAPON_CLASS_ITEM:		pszWeaponClass = "Item:";			break;
	case WEAPON_CLASS_MISSION:	pszWeaponClass = "Mission:";		break;
	case WEAPON_CLASS_MG :		pszWeaponClass = "MG:";				break;
	default:	I3ASSERT_0;		break;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	//	�ִϸ��̼� ����
	// ! praptor - ���� ī�޶� �������� ����
	
	//if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{
	//	CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
	//	if( pCharacter == this ) bFixed3PV = FALSE;
	}
	

	if( bFixed3PV)
	{
		pszPVState = "3PV:";			//	������ ������� 3��Ī ����	
		AnimSet = CHARA_ANIMSET_3PV;
	}
	else
	{
		if( AnimSet == CHARA_ANIMSET_1PV)			pszPVState = "1PV:";		//	1��Ī ĳ�����̴�.
		else if( AnimSet == CHARA_ANIMSET_3PV)		pszPVState = "3PV:";		//	3��Ī ĳ�����̴�.
		else I3ASSERT_0;	// ! praptor - ���� ī�޶� ���� �߰�
	}
	
	switch( getActionContext()->getBody())
	{
	case CHARA_BODY_STAND:		pszStandState = "Stand:";		break;
	case CHARA_BODY_CROUCH:		pszStandState = "Sit:";			break;	
	default:					pszStandState = "Stand:";		break;
	}

	//	note. ���� CHARA_BODY_NONE ���´� Upper�� Dodge �Ǵ� Death ���ۿ����� �����Դϴ�
	switch( getActionContext()->getBody2())
	{
	case CHARA_BODY_NONE:			pszIdleState = "";				break;		//	���� �ִϸ��̼��� AttackIdle, Idle ������ �����ϴ�.
	//case CHARA_BODY_IDLE:			pszIdleState = "Idle";			break;
	case CHARA_BODY_ATTACKIDLE:		pszIdleState = "AttackIdle";	break;
	case CHARA_BODY_ZOOM:			pszIdleState = "Zoom";			break;
	default:						pszIdleState = "AttackIdle";	break;
	}
		
	//	1��Ī������ ����ü ���� �ִϸ��̼��� �����Ƿ� ������ 3��Ī�� �ִϸ��̼��� �÷����Ѵ�.
	if( getActionContext()->isBody2( CHARA_BODY_NONE ))
		pszPVState = "3PV:";

	//	Upper AI Path
	i3String::Concat( szAIUpperPath, pszPVState);							//	����
	i3String::Concat( szAIUpperPath, pszWeaponClass);						//	���� Ŭ����

	char szFindWeapon[ 256] = {0,};
	for( INT32 i = 0; i < i3String::Length( pWeapon->GetName()); i++)
	{
		if( pWeapon->GetName()[ i] == '_')
			break;
		szFindWeapon[ i] = pWeapon->GetName()[i];
	}

	i3String::Concat( szAIUpperPath, szFindWeapon);					//	���� �̸�
	i3String::Concat( szAIUpperPath,":");
	//i3String::Concat( szAIUpperPath, pszStandState);						//	Stand or Sit

	if( getActionContext()->isBody2( CHARA_BODY_NONE))
	{
		szAIUpperPath[ strlen( szAIUpperPath) - 1] = '\0';					//	No AttackIdle or Idle or Zoom
	}
	else
	{
		i3String::Concat( szAIUpperPath, pszIdleState);						//	AttackIdle or Idle or Zoom
	}

	//	Lower AI Path
	i3String::Concat( szAILowPath, pszStandState);
	szAILowPath[ strlen( szAILowPath) - 1] = '\0';

	switch( AnimSet)
	{
	//	1��Ī �϶�
	case CHARA_ANIMSET_1PV:
		{
			#if 1
				if( m_pAnimContext->isAILower())		
					m_pAnimContext->getAILower()->SetEnable( false, true);

				m_pAnimContext->setAILower( NULL);
			#else
				m_pAnimContext->setAILower( FindAI( szAILowPath));
				I3ASSERT( m_pAnimContext->getAILower() != NULL);
			#endif

			m_pAnimContext->setAIUpper( FindAI( szAIUpperPath));
			I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

			//	UpperRoot�� �������� m_pAnimContext->getAIUpper()�� Ȱ���ϰ� ���þ��� AIState�� Disable ��Ų��.
			UpperRoot = FindAI( "Upper");
			UpperRoot->setGroupEnable( m_pAnimContext->getAIUpper());		
		}
		break;
	//	3��Ī �϶�
	case CHARA_ANIMSET_3PV:
		{
			m_pAnimContext->setAILower( FindAI( szAILowPath));
			I3ASSERT( m_pAnimContext->getAILower() != NULL);

			//	UpperRoot�� �������� m_pAnimContext->getAILower()�� Ȱ���ϰ� ���þ��� AIState�� Disable ��Ų��.
			UpperRoot = FindAI( "Lower");
			UpperRoot->setGroupEnable( m_pAnimContext->getAILower());

			m_pAnimContext->setAIUpper( FindAI( szAIUpperPath));
			I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

			//	UpperRoot�� �������� m_pAnimContext->getAIUpper()�� Ȱ���ϰ� ���þ��� AIState�� Disable ��Ų��.
			UpperRoot = FindAI( "Upper");
			UpperRoot->setGroupEnable( m_pAnimContext->getAIUpper());
		}
		break;
	default: I3ASSERT_0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ANIM
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameCharaBase::ChangeAnimSetLowerSync( CHARA_ANIMSET_TYPE type, BOOL isFixed3PV)
{
	BOOL bNeedStopAnim = (m_AnimSet != type);
	
	m_AnimSet = type;
	
	//ACTION_apply( isFixed3PV);
	getAnimContext()->getControl()->AnimSetApply();
	
	REAL32 fLowerSyncTime = 0.0f;

	//	�ݵ�� ANIM_Stop�ϱ� ���� �÷��� �ð��� �޾� ���ƾ� �Ѵ�. stop�� �ϸ� Local Time�� 0 �� �ǹǷ�...����
	fLowerSyncTime = GetSyncTimeLower();

	//	1��Ī���� �ٲ� ���� �������� 3��Ī ��ü �ִϸ��̼ǿ� ������ ������ �ȵȴ�.
	//BOOL bNetFPSView = FALSE;
	//if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{	
	//	CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
	//	if( pCharacter != g_pPlayer && pCharacter == this ) bNetFPSView = TRUE;
	}
	

	if( bNeedStopAnim )
	{
		_ANIM_stop();
	}

	//	������ ����� ��� �ִϸ��̼� ��Ʈ ��ü�� ����Ǳ� ������ ���� ��ũ�� ���� �÷����ؾ� �Ѵ�.
	//	�Ȱ��� �������� �������� 1��Ī�̳� 3��Ī�̳Ŀ� ���� ���� �ٸ� �ִϸ��̼� �����̱� �����̴�.
	ANIM_playLower( getActionContext()->getLower(), (ID_LOWER_AI)m_pAnimContext->getOldLowerID(), fLowerSyncTime);
}

void CGameCharaBase::ChangeAnimSet( CHARA_ANIMSET_TYPE type, BOOL bAnimSync, BOOL isFixed3PV)
{
	BOOL bNeedStopAnim = (m_AnimSet != type);
	
	setAnimSet( type);
	
	//ACTION_apply( isFixed3PV);
	getAnimContext()->getControl()->AnimSetApply();
		
	if( bAnimSync)
	{
		REAL32 fLowerSyncTime = 0.0f;
		REAL32 fUpperSyncTime = 0.0f;

		//	�ݵ�� ANIM_Stop�ϱ� ���� �÷��� �ð��� �޾� ���ƾ� �Ѵ�. stop�� �ϸ� Local Time�� 0 �� �ǹǷ�...����
		fLowerSyncTime = GetSyncTimeLower();
		fUpperSyncTime = GetSyncTimeUpper();

		//	1��Ī���� �ٲ� ���� �������� 3��Ī ��ü �ִϸ��̼ǿ� ������ ������ �ȵȴ�.
		
		//BOOL bNetFPSView = FALSE;
		//if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
		{	
		//	CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		//	if( pCharacter != g_pPlayer && pCharacter == this ) bNetFPSView = TRUE;
		}
		
		// || bNetFPSView

		if( bNeedStopAnim  )			_ANIM_stop();
		

		//	������ ����� ��� �ִϸ��̼� ��Ʈ ��ü�� ����Ǳ� ������ ���� ��ũ�� ���� �÷����ؾ� �Ѵ�.
		//	�Ȱ��� �������� �������� 1��Ī�̳� 3��Ī�̳Ŀ� ���� ���� �ٸ� �ִϸ��̼� �����̱� �����̴�.
		ANIM_playLower( getActionContext()->getLower(), (ID_LOWER_AI)m_pAnimContext->getOldLowerID(), fLowerSyncTime);
		ANIM_playUpper( getActionContext()->getUpper(), (ID_UPPER_AI)m_pAnimContext->getOldUpperID(), fUpperSyncTime);
		
	}
}

/*

ID_LOWER_AI CGameCharaBase::_ANIM_getMoveId( VEC3D * vDir)
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
	else
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_MOVE_LEFT;	//pszAIName = "Left";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_MOVE_RIGHT;	//pszAIName = "Right";
	}
	
	//return pszAIName;
	return ID_LOWER_IDLE;
}

ID_LOWER_AI CGameCharaBase::_ANIM_getWalkId( VEC3D * vDir)
{
	//char * pszAIName = ID_LOWER_IDLE;	
	
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
	else
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_WALK_LEFT;	//pszAIName = "Left";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_WALK_RIGHT;	//pszAIName = "Right";
	}
	
	//return pszAIName;
	return ID_LOWER_IDLE;
}


BOOL CGameCharaBase::_ANIM_playBodyLowerSync( CHARA_ACTION_BODY state, BOOL bIs3PV)
{
	if( getActionContext()->isBody( state))	return FALSE;

	getActionContext()->setBody( state);

	//	��ü�� ��ũ ���� �÷��̸� ����Ѵ�.
	ChangeAnimSetLowerSync( getAnimSet(), bIs3PV);

	return FALSE;
}

BOOL CGameCharaBase::_ANIM_playBody( CHARA_ACTION_BODY state, BOOL bIs3PV)
{
	if( getActionContext()->isBody( state))	return FALSE;

	getActionContext()->setBody( state);

	ChangeAnimSet( getAnimSet(), TRUE, bIs3PV);

	return FALSE;
}

BOOL CGameCharaBase::ANIM_playBody( CHARA_ACTION_BODY state, BOOL bIs3PV)
{
	return _ANIM_playBody( state, bIs3PV );
}

BOOL CGameCharaBase::_ANIM_playBody2( CHARA_ACTION_BODY2 state, BOOL bIs3PV)
{
	if( getActionContext()->isBody2( state))	return FALSE;

	//	�ϴ� AttackIdle�� ����
	if( state == CHARA_BODY_IDLE)	state = CHARA_BODY_ATTACKIDLE;

	getActionContext()->setBody2( state);

	//ACTION_apply( bIs3PV);
	getAnimContext()->getControl()->AnimSetApply();

	if( getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE) || 
		getActionContext()->isUpper( CHARA_UPPER_JUMP) || getActionContext()->isUpper( CHARA_UPPER_FIRE) ||
		getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) || getActionContext()->isUpper(CHARA_UPPER_DAMAGE) )
	{
		switch( getActionContext()->getBody2())
		{
		case CHARA_BODY_ATTACKIDLE:
			{	//	�ִϸ��̼� Ÿ�� ��ũ�� �ʿ���� ���
				if( !getActionContext()->isLower( CHARA_LOWER_IDLE) && !getActionContext()->isLower( CHARA_LOWER_MOVE) && 
					!getActionContext()->isLower( CHARA_LOWER_WALK) && !getActionContext()->isLower( CHARA_LOWER_JUMP))
					return TRUE;
			}
			//	�Ʒ� ���۵� �մϴ�. No break!!
		case CHARA_BODY_ZOOM:
		case CHARA_BODY_IDLE:
			{
				//	�ִϸ��̼��� ��� �մϴ�.
				{
					REAL32 fSyncTime = 0.0f;

					//	��ü �ִϸ��̼ǰ� �´� Ÿ�� ��ũ�ؾ� �մϴ�.
					if( m_pAnimContext->isAILower())
					{
						fSyncTime = GetSyncTimeLower();
					}
					else
					{	//	��ü�� Ÿ�� ��ũ�ϱ� ����
						fSyncTime = GetSyncTimeUpper();
					}

					//	�ִϸ��̼� Ÿ�� ��ũ�ؾ� �մϴ�.			
					ANIM_playLower( getActionContext()->getLower(), (ID_LOWER_AI)m_pAnimContext->getOldLowerID(), fSyncTime);	
					ANIM_playUpper( getActionContext()->getUpper(), (ID_UPPER_AI)m_pAnimContext->getOldUpperID(), fSyncTime);				
				}
			}
			break;	
		}
	}
	
	return TRUE;
}

BOOL CGameCharaBase::ANIM_playBody2( CHARA_ACTION_BODY2 state, BOOL bIs3PV )
{
	return _ANIM_playBody2( state, bIs3PV );
}

void CGameCharaBase::_ANIM_Debug( i3AIContext * pCtx, REAL32 timeStart)
{	//	i3AIContext�� NULL�� ��쵵 �ִ�. ��ŵ�ϵ���
	if( pCtx == NULL)		return;

	i3AIState * pAIState = NULL;
	char * pszAIName = "(null)";

	//	���� Anim�� �� �����ӿ� ������� �ʾҴ�.
	if( pCtx->getNextAIStateIndex() != -1)
		pAIState = pCtx->getNextAIState();
	else
		pAIState = pCtx->getCurrentAIState();
	
	if( pAIState != NULL)
	{
		if( pAIState->getAnim())	
			pszAIName = (char *) pAIState->getAnim()->GetName();
		
		I3TRACE("- AI (%s) Anim (%s) (start %.4f, scale %.2f)\n", pAIState->GetName(), pszAIName, timeStart, pAIState->getTimeScale());
	}
	else
	{
		I3TRACE("- AI (null)\n");
	}
}

void CGameCharaBase::_ANIM_stop( void)
{
	GetSceneObject()->GetBody()->StopAnim();
}

BOOL CGameCharaBase::_ANIM_playLower( CHARA_ACTION_LOWER state, ID_LOWER_AI id, REAL32 timeStart, REAL32 timeScale)
{
	if( getActionContext()->isLower( CHARA_LOWER_DEATH))	return FALSE;

	//	���°� �㰡�Ǹ� ��ä �ִϸ��̼��� �Ѵ�.
	getActionContext()->setLower( state);

	//	Combined  Anim�� ���� ��ü ������ ���ش�.
	if( state == CHARA_LOWER_DEATH)
		m_pAnimContext->setOldLowerID( ID_LOWER_UNKNOWN);
		
	if( m_pAnimContext->isAILower() && (id > ID_LOWER_UNKNOWN) && (id < ID_LOWER_AI_MAX))
	{
		//	���� ������ �������� �����Ѵ�.
		m_pAnimContext->setOldLowerID( id);

		INT32 nAIIdx = m_pAnimContext->getAILowerID( getActionContext()->getBody(), id);
		I3ASSERT( nAIIdx != -1);

		m_pAnimContext->getAILower()->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	��ü AI ���� ����

		return TRUE;
	}	

	return FALSE;
}

BOOL CGameCharaBase::ANIM_playLower( CHARA_ACTION_LOWER state, ID_LOWER_AI id, REAL32 timeStart, REAL32 timeScale )
{
#if defined( ENABLE_ANIM_OUTPUT )
	BOOL bRv = _ANIM_playLower(state, id, timeStart, timeScale );
	if( bRv )
	{
		if(m_pAnimContext->getAILower() != NULL)
		{
			TRACE_FUNC;
			_ANIM_Debug( m_pAnimContext->getAILower(), timeStart );
		}
		
	}
	return bRv;
#else
	return _ANIM_playLower(state, id, timeStart, timeScale );
#endif
}

BOOL CGameCharaBase::_ANIM_playUpper( CHARA_ACTION_UPPER state, ID_UPPER_AI id, REAL32 timeStart, REAL32 timeScale)
{
	if( getActionContext()->isLower( CHARA_LOWER_DEATH))	return FALSE;
	
	if( m_pAnimContext->isAIUpper() && (id > ID_UPPER_UNKNOWN) && (id < ID_UPPER_AI_MAX))
	{
		CWeaponInfo * pWeaponInfo = NULL;
		WeaponBase * pWeapon = getWeaponContext()->getCurrent();
		if( pWeapon != NULL )
		{
			pWeaponInfo = pWeapon->getWeaponInfo();
		}

		//	���� ��ȯ�� ���ܷ� ����� ���� �ٲ��� ���� �����̸� ����� �����̴�.
		//	���� �����ӿ� ���� ��尡 �ٲ�� �Ǿ� �����Ƿ� ĳ���ʹ� ����� ����� �ν��ؾ� �Ѵ�.	komet
		if( getWeaponContext()->getSwapSlotNum() != getWeaponContext()->getCurrentSlotNum())	
		{
			WeaponBase * pWeapon = getWeaponContext()->getWeapon( getWeaponContext()->getSwapSlotNum());		
			if( pWeapon == NULL )
				return FALSE;

			pWeaponInfo = pWeapon->getWeaponInfo();
		}

		if( pWeaponInfo == NULL )
			return FALSE;

		INT32 nAIIdx = m_pAnimContext->getAIUpperID( getAnimSet(), pWeaponInfo->m_TypeClass, pWeaponInfo->m_nNumber, getActionContext()->getBody2(), id);
		if( nAIIdx == -1)
		{
			I3TRACE( "Chara Animset no animation.\n" );
			return FALSE;
		}

		//	���°� �㰡�Ǹ� ��ü �ִϸ��̼��� �Ѵ�.
		//	����! Combined �ִϸ��̼��� ��ü�� ���Ѵ�.
		getActionContext()->setUpper( state);

		//	(���� ������ �������� �����Ѵ�.)
		m_pAnimContext->setOldUpperID( id);

		m_pAnimContext->getAIUpper()->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	��ü AI ���� ����
		
		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaBase::ANIM_playUpper( CHARA_ACTION_UPPER state, ID_UPPER_AI id, REAL32 timeStart, REAL32 timeScale )
{
#if defined( ENABLE_ANIM_OUTPUT )
	BOOL bRv = _ANIM_playUpper( state, id, timeStart, timeScale );
	if( bRv )
	{
		if(m_pAnimContext->getAIUpper() != NULL)
		{
			TRACE_FUNC;
			_ANIM_Debug( m_pAnimContext->getAIUpper(), timeStart);
		}
	}
	return bRv;
#else
	return _ANIM_playUpper( state, id, timeStart, timeScale );
#endif
}


BOOL CGameCharaBase::_ANIM_playUpperReturn( BOOL bIsNetwork)
{	//	������ �������� Upper �ִϸ��̼����� ���ư���.

	if( getActionContext()->isBody2( CHARA_BODY_IDLE) )
		_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, bIsNetwork);

	//	��ü �ִϸ��̼ǰ� �´� Ÿ�� ��ũ�ؾ� �մϴ�.
	REAL32 fSyncTime = GetSyncTimeLower();
	
	if( getActionContext()->isLower( CHARA_LOWER_MOVE) )//|| getActionContext()->isLower( CHARA_LOWER_WALK) )
	{
		ANIM_playUpper( CHARA_UPPER_MOVE, ID_UPPER_MOVE, fSyncTime);				//	���� Move ��������
	}
	else
	{		
		ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_IDLE, fSyncTime);		//	���� Idle ��������
	}
	
	return TRUE;
}

BOOL CGameCharaBase::ANIM_playUpperReturn( BOOL bIs3PV )
{
	TRACE_FUNC;
	return _ANIM_playUpperReturn( bIs3PV );
}

REAL32 CGameCharaBase::_ANIM_getUpperDuration(void)
{
	if( !m_pAnimContext->isAIUpper())	return 0.0f;
	i3AIState * pAIState = m_pAnimContext->getAIUpper()->getCurrentAIState();
	return (pAIState != NULL ? pAIState->getDuration() : 0.0f);								
}

REAL32 CGameCharaBase::_ANIM_getUpperLocalTime( void)
{
	if( !m_pAnimContext->isAIUpper())	return 0.0f;
	return m_pAnimContext->getAIUpper()->getLocalTime();
}

REAL32 CGameCharaBase::_ANIM_getLowerDuration( void)		
{	
	if( !m_pAnimContext->isAILower())	return 0.0f;

	i3AIState * pAIState = m_pAnimContext->getAILower()->getCurrentAIState();
	return (pAIState != NULL ? pAIState->getDuration() : 0.0f);
}

REAL32 CGameCharaBase::_ANIM_getLowerLocalTime( void)
{
	if( !m_pAnimContext->isAILower())	return 0.0f;
	return m_pAnimContext->getAILower()->getLocalTime();
}*/


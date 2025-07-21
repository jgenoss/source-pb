#include "pch.h"
#include "GameCharaInputControlAI.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "GameStateMgr.h"
#include "i3Base/itl/vector_set.h"
#include "MyRoomInfoContext.h"
#include "BattleObjContext.h"
#include "HackContext.h"
#include "GameMissionManager.h"
#include "GameMaster.h"

#if 0
#define		AIDUMP			I3TRACE
#else
#define		AIDUMP			__noop
#endif

#include "../ui/Batmanager.h"


I3_CLASS_INSTANCE( CGameCharaInputControlAI);


CGameCharaInputControlAI::CGameCharaInputControlAI()
{
	i3mem::FillZero(m_EvasionRate, sizeof(m_EvasionRate));
	i3mem::FillZero(m_WayPoint, sizeof(m_WayPoint));

	OnResetAI();
}

CGameCharaInputControlAI::~CGameCharaInputControlAI()
{
	I3_SAFE_RELEASE( m_pGamePath);

	for( size_t i = 0;i < m_RemoveObjectList.size(); ++i)
	{
		TMPOBJ_INFO * pInfo = m_RemoveObjectList[i];
		
		I3MEM_SAFE_FREE( pInfo);
	}
	m_RemoveObjectList.clear();
}

void CGameCharaInputControlAI::BindAI(AI_BIND_DATA* pData)
{
	// ��� ���� (�г���, ����, ���, ��빫��, ������)
	m_nickname = pData->_wstr_nickname;	
	m_rank = pData->_rank;

	m_primaryWeaponName = pData->_primaryWeaponName;
	m_secondaryWeaponName = pData->_secondaryWeaponName;
	m_meleeWeaponName = pData->_meleeWeaponName;
	m_throwWeaponName = pData->_throwWeaponName;
	m_itemWeaponName= pData->_itemWeaponName;
	m_hp = pData->_hp;
	m_respawnTime = pData->_respawnTime;

	//Attack Time
	m_AIData._PreAttack = pData->_AIData._PreAttack;
	m_AIData._Attack = pData->_AIData._Attack;
	m_AIData._Attack += pData->_AIData._PreAttack;

	m_AIData._AfterAttack = pData->_AIData._AfterAttack;
	m_AIData._AfterAttack += pData->_AIData._Attack;

	m_AIData._TotalTime += pData->_AIData._TotalTime;

	//SeeSight Angle
	m_AIData._Angle = pData->_AIData._Angle;
	m_AIData._Range = pData->_AIData._Range;

	//Targeting Level
	m_AIData._Deviation = pData->_AIData._Deviation;

	m_AIData._KeepWatchTime = pData->_AIData._KeepWatchTime;
	m_AIData._Priority = pData->_AIData._Priority;
	m_AIData._SwapWeaponDistance = pData->_AIData._SwapWeaponDistance;

	m_AIData._SwapRatioHandgun = pData->_AIData._SwapRatioHandgun;
	m_AIData._SwapRatioKnife= pData->_AIData._SwapRatioKnife;

	// ����ź ��ô ����
	m_GrenadePattern = pData->_GrenadePattern;
	m_SmokeProbability = pData->_SmokeProbability;

	// �⺻ ���� ��ġ���� �Ʒ��� �����Ͽ� ����ϴ� ����
	m_AIData._TargetHeightUnerNeck = pData->_AIData._TargetHeightUnerNeck;

	m_EvasionRate[AIET_SIDECRAWL] = pData->_EvasionRate[AIET_SIDECRAWL];
	m_EvasionRate[AIET_MOVE] = pData->_EvasionRate[AIET_MOVE];
	m_EvasionRate[AIET_SIDECRAWLCROUCH] = pData->_EvasionRate[AIET_SIDECRAWLCROUCH];
	m_EvasionRate[AIET_MOVECROUCH]= pData->_EvasionRate[AIET_MOVECROUCH];
	m_EvasionRate[AIET_SIDECRAWLFOOTSTEP] = pData->_EvasionRate[AIET_SIDECRAWLFOOTSTEP];
	m_EvasionRate[AIET_MOVEFOOTSTEP] = pData->_EvasionRate[AIET_MOVEFOOTSTEP];
	m_EvasionRate[AIET_SIDECRAWLCROUCHFOOTSTEP] = pData->_EvasionRate[AIET_SIDECRAWLCROUCHFOOTSTEP];
	m_EvasionRate[AIET_MOVECROUCHFOOTSTEP] = pData->_EvasionRate[AIET_MOVECROUCHFOOTSTEP];
	m_EvasionRate[AIET_CROUCH] = pData->_EvasionRate[AIET_CROUCH];
	m_EvasionRate[AIET_STAND] = pData->_EvasionRate[AIET_STAND];
	
	if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
		m_bAttack_Death_Player = pData->_AIFeature._Attack_Death_Player == 1 ? true:false;

	m_bAttacking_Death_Player = false;

}

void CGameCharaInputControlAI::OnResetAI()
{
	m_nickname.clear();	

	m_rank = 0;
	m_primaryWeaponName = 0;
	m_secondaryWeaponName = 0;
	m_meleeWeaponName = 0;
	m_redCharaName = 0;	
	m_blueCharaName = 0;
	m_itemWeaponName = 0;
	m_headCharaName = 0;
	m_beretCharaName = 0;
	m_hp = 0;
	m_respawnTime = 0;


	m_nActionState	= 0;
	m_nMoveState	= 0;

	m_idxCurTargetPoint	= -1;
	m_idxOldTargetPoint	= -1;

	m_CurrentTargetIdx = -1;

	m_WayPointCount		= 0;
	m_CurWayPointIndex	= 0;
	m_rElapsedKeepWatchTime = 0.0f;
	m_rElapsedTraceTime = 0.0f;
	m_rElapsedJumpTime	= 0.0f;
	m_rAIUpdateElapsedTime = 10.0f;
	m_rElapsedAutoFireTime	= 0.0f;
	m_rAfterThrowTimer		= 0.0f;

	m_rTargetTheta		= 0.0f;
	m_rOldTheta			= 0.0f;
	m_rThetaTimer		= 0.0f;
	m_InterpolateTime	= 0.2f;

	m_rEvasionStateTime	= 0.0;

	m_nAIState	=  m_nOldAIState = AI_STATE_NONE;
	if( m_pGamePath != nullptr)
	{
		m_nAIState	=  m_nOldAIState = AI_STATE_GOPATH;
	}

	m_bLeft				= false;
	m_timeGrenadeThrow	= 0.0f;

	if( m_GrenadePattern == AI_GRENADE_RANDOM)
	{
		m_GrenadePattern = (AI_GRENADE_PATTERN)(i3Math::Rand() % 3);
	}

	{
		// ����ź�� ���� Ȯ���� �������� ���.
		INT32 p = i3Math::Rand() % 100;

		m_bThrowSmoke = ( p <= m_SmokeProbability);
	}

	m_bForceEvadeSelfThrow = false;

	m_bMoveFlag[0] = m_bMoveFlag[1] = false;

	m_bAttacking_Death_Player = false;

}

void CGameCharaInputControlAI::OnFirstUpdate( REAL32 rDeltatime)
{
	if( m_pOwner == nullptr)		return;

	if( m_pGamePath == nullptr)
	{
		if( gstatemgr::i()->getStageState() == GAMESTAGE_STATE_BATTLE)
		{
			CStageBattle * pStage = g_pFramework->GetStageBattle();
			I3ASSERT( pStage != nullptr);

			i3GamePath * pGamePath = (i3GamePath*)i3Framework::FindGameNodeByType( pStage, i3GamePath::static_meta(), true);

			if (pGamePath == nullptr)
			{
				HackContext::i()->HackingDetected( CC_AI_HACK ,C_AI_HACK );
			}
			else
			I3_REF_CHANGE( m_pGamePath, pGamePath);
		}

		m_nAIState			= AI_STATE_GOPATH;
	}
}

AI_STATE CGameCharaInputControlAI::ProcessAI( REAL32 rDeltatime)
{
	if( m_nAIState == AI_STATE_NONE)	return AI_STATE_NONE;

	AI_STATE oldState	= m_nAIState;

	//�ֺ��� ���� �ִ��� �ľ�
	INT32 idx = FindTarget();
	if( idx != -1)
	{
		//�ִٸ� ���� ����
		if( FightWidth( idx) )
		{
			AI_STATE nextState	= AI_STATE_FIGHT;
			
			//���� ���۽� ���������� ���ϱ�����
			if( oldState != AI_STATE_FIGHT)
			{
				m_rElapsedTraceTime = 5.0f;
			}
			return nextState;
		}
	}

	//�ƴϸ� �̵����·� 
	AI_STATE nextState = AI_STATE_MOVE;

	//������ �������̾��ٸ�
	if( oldState == AI_STATE_FIGHT)	
	{
		nextState = AfterFight();
	}
	else
	{
		m_nActionState &= ~AI_ACTION_STOPATTACK;
	}

	return nextState;
}

AI_STATE CGameCharaInputControlAI::AfterFight(void)
{
	bool bFixed = false;
	AI_STATE		nextState = AI_STATE_MOVE;

	//���� ���̾��� �ɸ��Ͱ� ���� ���Ŀ��� ��Ҵٸ�. 
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_CurrentTargetIdx);

	if( pChara != nullptr )
	{
		if( !pChara->isCharaStateMask( CHARA_STATE_DEATH) && m_idxCurTargetPoint >= 0)
		{
			// ����ź�� ���� �� ������ ������.
			if( ProcessGrenadeForLazyPattern( pChara->GetPos()))
			{
				nextState = AI_STATE_THROW;
			}
			else
			{
				INT32 idx = GetClosestPoint( pChara->GetPos());
				if( idx >= 0 && m_idxCurTargetPoint != idx)
				{
					FindTargetPosition( m_idxCurTargetPoint, idx, MAX_PATH_DEPTH);
				}
			}

			bFixed = true;
		}
	}

	if( bFixed == false)
	{
		FindStartPosition();
	}

	StopAttack();

	m_nActionState |= AI_ACTION_STOPATTACK;

	return nextState;
}

void CGameCharaInputControlAI::OnGoPath(REAL32 rDeltaSeconds, bool bFirst)
{
	m_idxCurTargetPoint = FindStartPosition();

	if( m_idxCurTargetPoint != -1)
	{
		m_nAIState = AI_STATE_MOVE;
	}
}

bool CGameCharaInputControlAI::IsThisNegativeByWatchTime()
{
	REAL32	MaxLazy	= 10.0f;
	INT32	nRand = i3Math::Rand()%100;
	REAL32	rRand, Lazy, Rate;

	Lazy		= MIN( m_AIData._KeepWatchTime, MaxLazy);

	Rate = Lazy / MaxLazy;
	Rate *= Rate;
	
    rRand = nRand * 0.01f;
	if( rRand < Rate)
	{
		return true;
	}
	
	return false;
}

//�������� �ʰ� �ֽ��Ѵ�.
void CGameCharaInputControlAI::OnKeepWatch(REAL32 rDeltaSeconds, bool bFirst)
{
	m_rElapsedTraceTime += rDeltaSeconds;
	m_rElapsedKeepWatchTime += rDeltaSeconds;

	if( m_rAIUpdateElapsedTime > 0.5f)
	{
		INT32 idx = FindTarget();
		if( idx != -1)
		{
			//�ִٸ� ���� ����
			if( FightWidth( idx) )
			{
				m_nAIState = AI_STATE_FIGHT;

				m_rElapsedTraceTime = 5.0f;

				return ;
			}
		}

		m_rAIUpdateElapsedTime = 0.0f;
	}

	if( m_rElapsedKeepWatchTime > 1.5f)
	{	
		I3_NEXTPOINTINFO Next[10];
		INT32 cnt = m_pGamePath->GetLinkedPoint( m_idxOldTargetPoint, Next, 10);
		if( cnt > 0)
		{
			INT32 nRand = i3Math::Rand() %cnt;
			if( m_idxOldTargetPoint != Next[nRand].m_idxNext)
			{
				i3::pack::PATH_POINT_INFO * pPointInfo = m_pGamePath->getPoint( Next[nRand].m_idxNext);
				VEC3D target(pPointInfo->m_Pos.x, pPointInfo->m_Pos.y, pPointInfo->m_Pos.z);
				SeeTargetPosition( &target, 0.2f, true);
			}
		}

		m_rElapsedKeepWatchTime = 0.0f;
	}

	m_nActionState |= AI_ACTION_CROUCH;

	if( m_rElapsedTraceTime > m_AIData._KeepWatchTime)
	{
		m_nAIState = AI_STATE_MOVE;

		m_rElapsedTraceTime = 0.0f;

		m_nActionState &= ~AI_ACTION_CROUCH;
	}
}

//�̵��Ѵ�.
void CGameCharaInputControlAI::OnMove(REAL32 rDeltaSeconds, bool bFirst)
{
	if( m_idxCurTargetPoint == -1)
	{
		FindStartPosition();

		return;
	}

	i3::pack::PATH_POINT_INFO * pPointInfo = m_pGamePath->getPoint( m_idxCurTargetPoint);
	//I3TRACE("MOVE TO:%d\n", m_idxCurTargetPoint);

	VEC3D pos(pPointInfo->m_Pos.x, pPointInfo->m_Pos.y, pPointInfo->m_Pos.z);
	AI_MOVE_RETURN_VALUE ret = MoveTo( &pos, rDeltaSeconds);

	if( ret == AI_MOVE_RET_ARRIVED)
	{
		m_rElapsedTraceTime = 0.0f;

		if( m_CurWayPointIndex >= m_WayPointCount)
		{
			FindNextTargetPosition();
		}
		else
		{
			m_idxOldTargetPoint = m_idxCurTargetPoint;

			m_idxCurTargetPoint = m_WayPoint[m_CurWayPointIndex];

			m_CurWayPointIndex++;

			m_rElapsedTraceTime = 0.0f;
		}

		AI_STATE next = OnArrived_FindNext();
		if( next == AI_STATE_MOVE)
		{
			pPointInfo = m_pGamePath->getPoint( m_idxCurTargetPoint);

			SeeTargetPosition( &pPointInfo->m_Pos);

			getBoneContext()->setRho( 0.0f);

			SelectWeapon();
		}
		else if( next == AI_STATE_KEEPWATCH)
		{
			m_rElapsedTraceTime = 0.0f;
			m_rElapsedKeepWatchTime = 0.0f;
			
			m_nAIState = AI_STATE_KEEPWATCH;

			SelectWeapon();
		}
	}
	else if( ret == AI_MOVE_RET_BLOCKED)
	{
		if( DestroyIIObject( rDeltaSeconds) == false)
		{
			FindStartPosition();

			pPointInfo = m_pGamePath->getPoint( m_idxCurTargetPoint);

			SeeTargetPosition( &pPointInfo->m_Pos);

			m_rElapsedTraceTime = 0.0f;
		}
	}
	else
	{
		//�̵����̴� ���ο��� ���� ��� ��쿣 ���� �̵������� ����
		if( m_idxOldTargetPoint != -1 && m_idxCurTargetPoint != -1 &&
			!m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) && m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) && (m_rElapsedTraceTime > 1.0f))
		{
			VEC3D vDir, vDest;

			pPointInfo = m_pGamePath->getPoint( m_idxCurTargetPoint);
			i3::pack::PATH_POINT_INFO * pOldPointInfo = m_pGamePath->getPoint( m_idxOldTargetPoint);

			i3Vector::Sub( &vDir, &pPointInfo->m_Pos, &pOldPointInfo->m_Pos);
			i3Vector::Normalize( &vDir, &vDir);

			i3Vector::Sub( &vDest, m_pOwner->GetPos(), &pOldPointInfo->m_Pos);
			REAL32 t = i3Vector::Normalize( &vDest, &vDest);

			REAL32 cost = 1 - i3Math::abs( i3Vector::Dot( &vDest, &vDir));
			 
			REAL32 dist = t * cost;
			if( dist > 1.0f)
			{
				FindStartPosition();
			}
		}
	}
}

AI_STATE CGameCharaInputControlAI::OnArrived_FindNext()
{
	i3::pack::PATH_POINT_INFO * pPointInfo = m_pGamePath->getPoint( m_idxOldTargetPoint);
	if( pPointInfo->m_Priority >= m_AIData._Priority)
	{
		if( IsThisNegativeByWatchTime())	//�� ��ġ�� �����µ� �������̶��
		{
			for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)	// ����ġ�� �ִ� �ٸ��Ʊ��� �ִ��� Ȯ���غ���
			{
				CGameCharaBase * pChara = g_pCharaManager->getChara( i);
				if( pChara == nullptr )	continue;
				if( m_pOwner == pChara)
					continue;

				if( pChara->isCharaStateMask( CHARA_STATE_DEATH))
					continue;

				// �� üũ
				if( m_pOwner->getCharaTeam() != pChara->getCharaTeam())	
					continue;

				//
				VEC3D vDiff;
				i3Vector::Sub( &vDiff, m_pOwner->GetPos(), pChara->GetPos());
				if( i3Vector::Length( &vDiff) < 1.0f)
				{
					//�� ��ġ�� �ٸ� �Ʊ��� �ִٸ� �׳� �̵� ����
					return AI_STATE_MOVE;
				}
			}

			return AI_STATE_KEEPWATCH;
		}
		else
		{
			return AI_STATE_MOVE;
		}
	}
	
	return AI_STATE_MOVE;
}

void CGameCharaInputControlAI::ActivateExtension( bool bActivate)
{
	WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pCurWeapon == nullptr) return;

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	if( getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) || getActionContext()->isUpper(CHARA_UPPER_LOADBULLET) ||
		getActionContext()->isUpper(CHARA_UPPER_FIREBULLET) || getActionContext()->isUpper(CHARA_UPPER_SWAPWEAPON) || 
		getActionContext()->isUpper(CHARA_UPPER_EXTENSION_ACTIVATE) || getActionContext()->isUpper(CHARA_UPPER_EXTENSION_DEACTIVATE) )
		return;

	WEAPON::EXT_TYPE type = pCurWeapon->GetCurrentExtensionType();
	if( type == WEAPON::EXT_DUAL_MAG || type == WEAPON::EXT_SILENCER)
	{
		return;
	}

	if(type == WEAPON::EXT_DOT_SIGHT || type == WEAPON::EXT_SCOPE)
	{
		if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM) == bActivate)
			return;
	}
	else
	{
		if( pCurWeapon->isExtensionActivate() == bActivate)	
			return;
	}

	switch( pCurWeapon->GetCurrentExtensionType())
	{
	case WEAPON::EXT_SCOPE :
	case WEAPON::EXT_ZOOM :					// ����
	case WEAPON::EXT_STEADY_AIM :

	case WEAPON::EXT_DOT_SIGHT :
		m_nActionState |= AI_ACTION_SUBFUNC;
		break;
	case WEAPON::EXT_GLAUNCHER:
		{
			I3ASSERT( i3::kind_of<WeaponGrenadeLauncher*>(pCurWeapon));

			WeaponGrenadeLauncher * pLauncher = (WeaponGrenadeLauncher*)pCurWeapon;
			if( pLauncher->getLoadedShellCount() > 0)
			{
				m_nActionState |= AI_ACTION_SUBFUNC;
			}
		}
		break;
	case WEAPON::EXT_SHOTGUN_LAUNCHER:
		{
			I3ASSERT( i3::kind_of<WeaponShotGunLauncher*>(pCurWeapon));

			WeaponShotGunLauncher * pLauncher = (WeaponShotGunLauncher*)pCurWeapon;
			if( pLauncher->getLoadedShellCount() > 0)
			{
				m_nActionState |= AI_ACTION_SUBFUNC;
			}
		}
		break;
	case WEAPON::EXT_BAYONET:
		{
			m_nActionState |=AI_ACTION_EXT_ACTIVATE;// AI_ACTION_SUBFUNC;
		}


		break;
	case WEAPON::EXT_DUAL_MAG :		// AK47
	case WEAPON::EXT_SILENCER :		// ���� ���ÿ� Activate
			break;
	default:
		m_nActionState |= AI_ACTION_SUBFUNC;
		break;
	}
}

void CGameCharaInputControlAI::OnHit( INT32 idx)
{
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idx);
	if( pChara != nullptr)
	{
		SeeTargetPosition( pChara->GetPos());
	}
}

//�����Ѵ�.
void CGameCharaInputControlAI::OnFight( REAL32 rDeltaSeconds, bool bFirst)
{
	WeaponBase * pCurWeapon = nullptr;

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_CurrentTargetIdx);
	if( pChara == nullptr)
		return;

	if(m_pOwner->IsDino())
	{
		pCurWeapon = ((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->GetDinoWeapon();
	}
	else
	{
		pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	}
	
	if( pCurWeapon == nullptr) return;

	CWeaponInfo * pWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	REAL32	rDistance;
	VEC3D vStart;

	REAL32 rate = 1.0f;

	if(m_pOwner->IsDino())
	{
		rate = 1.f;
	}
	else
	{
		if( getWeaponContext()->getCurrentSlotNum() == 1)
		{
			rate = 0.3f;
		}
		else if( getWeaponContext()->getCurrentSlotNum() == 2)
		{
			rate = 0.5f;
		}
	}
	

	REAL32	Pre = m_AIData._PreAttack * rate;
	REAL32	For = m_AIData._Attack * rate;
	REAL32	After = m_AIData._AfterAttack * rate;
	REAL32	Total = m_AIData._TotalTime * rate;
	bool	bForce = true;

	if( m_pOwner->IsDino() || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS))
	{
		bForce = false;
	}
	else
	{
		// ���������� ���, �������� �Ʒ��� ������ �ʽ��ϴ�.
		if( !(MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS)) 
			&& getWeaponContext()->getCurrentSlotNum() == 2 )	
			bForce = false;
	}
	

	rDistance = GetAttackDirection( pChara, &vStart, &m_vTargetDir, bForce);
	if( rDistance < 0.0f)
		return;

	if(( m_nActionState & 0x000FF000) == 0)
	{	
		//���� ó�� ����
		/*GlobalFunc::PB_TRACE("Pre %f", Pre);
		GlobalFunc::PB_TRACE("m_rElapsedAutoFireTime %f", m_rElapsedAutoFireTime);
		GlobalFunc::PB_TRACE("For %f", For);
		GlobalFunc::PB_TRACE("After %f", After);
		*/
		if( m_rElapsedAutoFireTime >= 0.0f && m_rElapsedAutoFireTime < Pre)
		{
			ActivateExtension( true);

			SeeTarget( &m_vTargetDir, true);
		}
		else if( (m_rElapsedAutoFireTime >= Pre) && (m_rElapsedAutoFireTime < For))
		{
			//GlobalFunc::PB_TRACE("Attack");
			Attack( &m_vTargetDir, rDeltaSeconds);
		}
		else if( (m_rElapsedAutoFireTime >= For) && (m_rElapsedAutoFireTime < After))
		{
			ActivateExtension( false);

			SeeTarget( &m_vTargetDir, true);
		}
		else
		{
			SeeTarget( &m_vTargetDir, true);
		}

		m_rElapsedAutoFireTime += rDeltaSeconds;

		if( m_rElapsedAutoFireTime > Total)
		{
			m_rElapsedAutoFireTime = 0.0f;
		}
	}

	m_rElapsedTraceTime += rDeltaSeconds;
	if( m_rElapsedTraceTime > 4.0f)
	{
		//ȸ�� ������ �����Ѵ�.
		SelectFightPose();

		m_rElapsedTraceTime = 0.0f;
	}

	m_nMoveState = 0;
	if( pWeaponInfo->GetRange() < rDistance)
	{
		MoveToTarget( pChara, pWeaponInfo->GetRange(), rDeltaSeconds);
	}
	else
	{
		if( m_rAfterThrowTimer > 0.0f)
		{	
			MoveOnFight( rDistance, rDeltaSeconds);
		}
		else
		{
			if( m_nActionState & AI_ACTION_MOVE)
			{
				MoveOnFight( rDistance, rDeltaSeconds);
			}
			else if( m_nActionState & AI_ACTION_EVASION)
			{
				ProcessEvasion( rDeltaSeconds);
			}
		}
	}
}

void CGameCharaInputControlAI::ProcessEvasion( REAL32 rDeltaSeconds)
{
	m_rEvasionStateTime += rDeltaSeconds;

	if( m_rEvasionStateTime > 1.0f)
	{
		m_bLeft = !m_bLeft;

		m_rEvasionStateTime = 0.0f;
	}

    if( m_bLeft)
	{
		m_nMoveState &= ~AI_MOVE_RIGHT;
		m_nMoveState |= AI_MOVE_LEFT;
	}
	else
	{
		m_nMoveState &= ~AI_MOVE_LEFT;
		m_nMoveState |= AI_MOVE_RIGHT;
	}
}

void	CGameCharaInputControlAI::SelectFightPose()
{
	WeaponBase * pWeapon = nullptr;

	if(m_pOwner->IsDino())
	{
		pWeapon = ((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->GetDinoWeapon();
	}
	else
	{
		pWeapon = m_pOwner->GetCurrentCharaWeapon();
	}

	m_nActionState &= ~0x00000000000000FF;

	
	if( pWeapon == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "pWeapon != nullptr");
		return ;
	}

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	if( pWeapon == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "pWeaponInfo != nullptr");
		return ;
	}

	if( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_KNIFE || pWeaponInfo->GetTypeClass() == WEAPON_CLASS_DUALKNIFE)
	{
		if(MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) )
		{
			switch( m_AIData._EvasionType)
			{
			case 5:	m_nActionState	|= AI_ACTION_EVASION;		break;
			case 4:	m_nActionState	|= AI_ACTION_MOVE;			break;
			case 3:	m_nActionState	|= AI_ACTION_MOVE;			break;
			case 2: m_nActionState	|= AI_ACTION_JUMP;			break;
			case 1: m_nActionState	|= AI_ACTION_CROUCH;		break;
			case 0: m_nActionState	|= AI_ACTION_FOOTSTEP;		break;
			}
		}
		else if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD) )
		{
			INT32 nRate = i3Math::Rand()%10 * i3Math::Rand()%10;	

			INT32 move = g_pTempConfig->m_Evade[EVADE_TYPE_MOVE];

			if( move < nRate) m_nActionState |= AI_ACTION_MOVE;
			else m_nActionState |= AI_ACTION_JUMP;
		}
		else if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
		{
			INT32 nRate = i3Math::Rand()%10 * i3Math::Rand()%10;	

			REAL32 move = m_EvasionRate[AIET_MOVE];

			if( m_bAttacking_Death_Player )
			{
				m_nActionState |= AI_ACTION_CROUCH;
			}
			else
			{
				if( move < nRate) m_nActionState |= AI_ACTION_MOVE;
				else m_nActionState |= AI_ACTION_JUMP;
			}
		}
	}
	else
	{
		INT32 idx = 9;

		REAL32 Rand = (REAL32)(i3Math::Rand()%100);
		REAL32 Rate = m_EvasionRate[0];
		for( INT32 i = 0; i < AIET_COUNT - 1; ++i)
		{
			if( Rate >= Rand)
			{
				idx = i;
				break;
			}

			Rate += m_EvasionRate[i+1];
		}

		switch( idx)
		{
			case 0:	m_nActionState	|= AI_ACTION_EVASION;											break;
			case 1: m_nActionState	|= AI_ACTION_MOVE;												break;
			case 2: m_nActionState	|= AI_ACTION_EVASION	| AI_ACTION_CROUCH;						break;						
			case 3: m_nActionState  |= AI_ACTION_MOVE		| AI_ACTION_CROUCH;						break;
			case 4: m_nActionState	|= AI_ACTION_EVASION	| AI_ACTION_FOOTSTEP;					break;
			case 5: m_nActionState	|= AI_ACTION_MOVE		| AI_ACTION_FOOTSTEP;					break;
			case 6: m_nActionState	|= AI_ACTION_EVASION	| AI_ACTION_FOOTSTEP | AI_ACTION_CROUCH;break;
			case 7: m_nActionState	|= AI_ACTION_MOVE		| AI_ACTION_FOOTSTEP | AI_ACTION_CROUCH;break;
			case 8: m_nActionState	|= AI_ACTION_CROUCH;											break;
			case 9: m_nActionState	|= AI_ACTION_JUMP;												break;
				/*
			default:
				break;
				*/
		}
	}
}

void	CGameCharaInputControlAI::MoveOnFight( REAL32 rDistance, REAL32 rDeltaSeconds)
{
	WeaponBase * pWeapon = nullptr;

	if(m_pOwner->IsDino())
	{
		pWeapon = ((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->GetDinoWeapon();
	}
	else
	{
		pWeapon = m_pOwner->GetCurrentCharaWeapon();
	}
	
	if( pWeapon == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "pWeapon != nullptr");
		return ;
	}

	if( m_idxCurTargetPoint != -1)
	{
		i3::pack::PATH_POINT_INFO * pPointInfo = m_pGamePath->getPoint( m_idxCurTargetPoint);

		VEC3D pos(pPointInfo->m_Pos.x, pPointInfo->m_Pos.y, pPointInfo->m_Pos.z);
		if( MoveTo( &pos, rDeltaSeconds) != AI_MOVE_RET_OK)
		{
			FindNextTargetPosition();
		}
	}
}

//ȸ���Ѵ�.
void CGameCharaInputControlAI::OnEscape( REAL32 rDeltaSeconds, bool bFirst)
{

}


void CGameCharaInputControlAI::OnUpdate( REAL32 rDeltatime)
{
	if( BaTMaN::use_batman )
	{
		if( BaTMaN::disable_ai_attack ) return;
	}

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DISABLE_CONTROL) == true)	return;

	bool bFirst = false;

	CGameCharaInputControl::OnUpdate( rDeltatime);

	if( m_pGamePath == nullptr)
		return;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) == true)	return;

	m_rAIUpdateElapsedTime += rDeltatime;

	m_nActionState &= ~AI_ACTION_FIRE;

	EvadeSelfThrow( rDeltatime);

	//����ź ��ô�� �ڽ��� ����ź�� �׾���� ������ �ִ� ��Ȳ�̸� ������ ���ϵ���
	if( m_bForceEvadeSelfThrow == false)
	{
		if( (m_nAIState & AI_STATE_MASK_HOLDPOSITION) == 0)
		{
			if( m_rAIUpdateElapsedTime > 0.5f)
			{
				m_nAIState = ProcessAI( rDeltatime);
				// ������ AIState�� �޶����ٸ�...
				m_rAIUpdateElapsedTime = 0.f;
			}
		}
		
		{
			bool bGrenade = true;

			if( m_bThrowSmoke)
			{
				if( isWeaponExist( WEAPON_SLOT_THROWING2))
				{
					// ����ź�� ������ �����̸鼭 ���� ������ ������ �ʾҴٸ�...
					ProcessGrenadeForBusyPattern( rDeltatime, WEAPON_SLOT_THROWING2);
				}

				bGrenade = false;
			}

			if( bGrenade && (m_GrenadePattern == AI_GRENADE_BUSY))
			{
				// ������ ��, ���� (������ ��) ����ź�� ������ ������ ���...

				if( m_nAIState & AI_STATE_MASK_THROW_CONSIDER)
				{
					// ����ź�� ���� �� �ִ� ��Ȳ.
					ProcessGrenadeForBusyPattern( rDeltatime, WEAPON_SLOT_THROWING1);
				}
			}
		}
	}
	else
	{
		if( (m_nAIState & AI_STATE_MASK_HOLDPOSITION) != 0)
		{
			m_nAIState = AI_STATE_MOVE;
		}
	}

	m_rAfterThrowTimer -= rDeltatime;

	if( m_nAIState != m_nOldAIState)
	{
		bFirst = true;

		m_rAIUpdateElapsedTime = 0.0f;

		m_nOldAIState = m_nAIState;
	}

	switch( m_nAIState)
	{
	case AI_STATE_GOPATH:
		{
			AIDUMP( "[%p] AI : GoPath\n", this);
			OnGoPath( rDeltatime, bFirst);
		}break;
	case AI_STATE_KEEPWATCH:
		{
			AIDUMP( "[%p] AI : KeepWatch\n", this);
			OnKeepWatch( rDeltatime, bFirst);
		}break;
	case AI_STATE_MOVE: 
		{
			AIDUMP( "[%p] AI : Move\n", this);
			OnMove( rDeltatime, bFirst);
		}break;
	case AI_STATE_FIGHT: 
		{
			AIDUMP( "[%p] AI : Fight\n", this);
			OnFight(rDeltatime, bFirst);

		}break;
	case AI_STATE_ESCAPE: 
		{
			AIDUMP( "[%p] AI : Escape\n", this);
			OnEscape(rDeltatime, bFirst);
		}break;

	case AI_STATE_THROW :
		AIDUMP( "[%p] AI : Throw\n", this);
		OnThrow( rDeltatime, bFirst);
		break;
	case AI_STATE_REMOVE_OBJ:
		AIDUMP( "[%p] AI : Remove Obj\n", this);
		OnRemoveObject( rDeltatime);
		break;
	default:
		{
			AIDUMP( "[%p] AI : Unknown\n", this);
		}break;
	}

	// Add Code here
	ProcessInput( rDeltatime);

	OnInterpolateTheta( rDeltatime);
}

void CGameCharaInputControlAI::OnLastUpdate( REAL32 rDeltatime)
{
	CGameCharaInputControl::OnLastUpdate( rDeltatime);
}

void CGameCharaInputControlAI::EvadeSelfThrow( REAL32 rDeltaTime)
{
	//����ź�� ������ �󸶰��� ����ź�� ���� �����̴°Ϳ� �켱������ �ش�.
	if( m_rAfterThrowTimer > 0.0f)
	{
		if( (m_rAfterThrowTimer - rDeltaTime) <= 0.0f)
		{
			INT32 valididx = -1;
			INT32 cnt = IsThrowWasSafeZone(&valididx);
			if( valididx != -1)
			{
				const GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->getGameInfo_ThrowWeapon( m_pOwner->getCharaNetIndex(), valididx);
				I3ASSERT( pInfo != nullptr );

				if( pInfo->getWeaponBase() != nullptr)
				{
					VEC3D vInvDir;
					WeaponBase * pWeapon = const_cast<WeaponBase*>(pInfo->getWeaponBase());
					i3Vector::Sub( &vInvDir, m_pOwner->GetPos(), pWeapon->GetPos());
					i3Vector::Normalize( &vInvDir, &vInvDir);

					INT32 cur = GetClosestDirectionPoint( m_pOwner->GetPos(), &vInvDir);
					if( cur != -1)
					{
						m_idxOldTargetPoint = m_idxCurTargetPoint;
						m_idxCurTargetPoint	= cur;

						m_WayPoint[0] = cur;
						m_WayPointCount = 1;

						m_rElapsedTraceTime = 0.0f;

						INT32 idx = GetClosestDirectionPoint( cur, &vInvDir);
						if( idx != -1)
						{
							m_WayPoint[1] = idx;

							m_CurWayPointIndex = 1;
							m_WayPointCount = 2;
						}

						m_bForceEvadeSelfThrow = true;
					}
				}
			}
			else
			{
				m_bForceEvadeSelfThrow = false;
			}

			if( cnt > 0)
			{
				m_rAfterThrowTimer = 1.0f + rDeltaTime;
			}
			else
			{
				m_rAfterThrowTimer		= -0.1f;
				m_bForceEvadeSelfThrow	= false;
			}
		}
	}
}

void CGameCharaInputControlAI::SetMoveDir( VEC3D * pVecOut)
{
	UINT64	moveState = getMoveState();

	if( moveState & AI_MOVE_FORWARD )			setZ( pVecOut, 1.f);
	else if( moveState & AI_MOVE_BACKWARD)		setZ( pVecOut, -1.f);
	else											setZ( pVecOut, 0.f);

	if( moveState & AI_MOVE_LEFT )				setX( pVecOut, 1.f);
	else if( moveState & AI_MOVE_RIGHT )		setX( pVecOut, -1.f);
	else											setX( pVecOut, 0.f);
}

struct DEFCOMP_DIR
{
	REAL32	rDot = 0.0f;
	REAL32  length = 0.0f;
	INT32	idx = 0;
	VEC3D	vDir;
};

template<> struct i3::less<DEFCOMP_DIR*> 
{
	bool operator()( const DEFCOMP_DIR * p1, const DEFCOMP_DIR *p2 ) const {  return p2->rDot < p1->rDot ; }		// ��������..
};


/*
static INT32 _compFunc( _defCompDir * p1, _defCompDir *p2)
{
	if( p1->rDot < p2->rDot)	return 1;
	else if( p2->rDot < p1->rDot)	return -1;
	else return 0;
}
*/

//�������� ��ġ�� ���� �������� ��ġ�� �̵������� pathpoint�� ��������� ���� ������ ������ ����Ʈ�� ���մϴ�.
INT32 CGameCharaInputControlAI::GetClosestDirectionPoint( VEC3D * pvTargetPos, VEC3D * pvDirection)
{
	if( m_pGamePath == nullptr)
	{
		return -1;
	}

	INT32 idxArray[20];
	VEC3D vDir, vStart;
	i3CollideeLine line;
	i3::pack::PATH_POINT_INFO * pPointInfo;

	i3::vector_multiset<DEFCOMP_DIR*> list;			// ������ϰ�����...�̷��� �׳� ��Ʈ�Լ��� ���°� �ξ� ����..(2012.05.30.����.�߸��� �ڷᱸ�����)
//	list.SetCompareProc( (COMPAREPROC)_compFunc);

	DEFCOMP_DIR dir[20];

	//������ �ִ°͵��� ����� ���� ������ ������ ����
	INT32 idx = m_pGamePath->FindClosestPoints( pvTargetPos, idxArray, 20);
	if( idx > 0)
	{
		INT32 i;
		for( i = 0; i < idx; ++i)
		{
			pPointInfo = m_pGamePath->getPoint( idxArray[i]);

			i3Vector::Sub( &vDir, &pPointInfo->m_Pos, pvTargetPos);

			dir[i].length = i3Vector::Normalize( &dir[i].vDir, &vDir);
			dir[i].idx = idxArray[i];
			dir[i].rDot = i3Vector::Dot( pvDirection, &dir[i].vDir);

			list.insert( &dir[i]);
		}

		i3Vector::Copy( &vStart, pvTargetPos);
		i3Vector::AddY( &vStart, 0.25f);
		for( i = 0; i < idx; ++i)
		{
			DEFCOMP_DIR * pInfo = list.get_vector()[i];
			
			line.SetStart( &vStart);
			line.SetDir( &pInfo->vDir);

			I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
			if( pHitResult != nullptr)
			{
				if( pHitResult->m_T >= pInfo->length)
				{
					return pInfo->idx;
				}
			}
		}
	}

	return -1;
}

//�������� ��ġ�� ���� �������� ��ġ�� �̵������� pathpoint�� ���մϴ�.
INT32 CGameCharaInputControlAI::GetClosestPoint( VEC3D * pvTargetPos)
{
	if( m_pGamePath == nullptr)
	{
		return -1;
	}

	INT32 idxArray[20];
	INT32 idx = m_pGamePath->FindClosestPoints( pvTargetPos, idxArray, 20);
	if( idx > 0)
	{
		for( INT32 i = 0; i < idx; ++i)
		{
			if( idxArray[i] != m_idxCurTargetPoint)
			{
				VEC3D vDir, vStart;
				i3CollideeLine line;
				REAL32 rLength;
				i3::pack::PATH_POINT_INFO * pPointInfo;

				pPointInfo = m_pGamePath->getPoint( idxArray[i]);

				i3Vector::Copy( &vStart, pvTargetPos);
				i3Vector::Sub( &vDir, &pPointInfo->m_Pos, pvTargetPos);
				rLength = i3Vector::Normalize( &vDir, &vDir);

				i3Vector::AddY( &vStart, 0.25f);
				line.SetStart( &vStart);
				line.SetDir( &vDir);

				I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
				if( pHitResult != nullptr)
				{
					if( pHitResult->m_T > rLength)
					{
						return idxArray[i];
					}
				}
			}
		}
	}

	return -1;
}

INT32 CGameCharaInputControlAI::FindStartPosition(void)
{
	if( m_pGamePath == nullptr)
	{
		return -1;
	}

	//I3TRACE("FIND NEW START POSITION\n");

	m_idxOldTargetPoint = m_idxCurTargetPoint;

	INT32 next = GetClosestPoint( m_pOwner->GetPos());
	if( next != -1)
	{
		m_idxCurTargetPoint = next;

		m_rElapsedTraceTime = 0.0f;
		m_CurWayPointIndex	= 0;
		m_WayPointCount		= 1;

		m_WayPoint[0]		= m_idxCurTargetPoint;

		return next;
	}

//	I3PRINTLOG(I3LOG_WARN, "Path�� ���̰� �ʹ� ����� �����մϴ�. �������ּ���: ���� CLOEST���� 20");

	return -1;
}

void CGameCharaInputControlAI::FindTargetPosition(I3_POINTIDX idxStart, I3_POINTIDX idxEnd, INT32 nDepth)
{
	if( m_pGamePath == nullptr)		return;

	//�������̸� ���� �н��� �ǵ��� ���� �ʵ���
	INT32 cnt = m_pGamePath->getPointCount();
	if( cnt > 0)
	{
		m_WayPointCount = m_pGamePath->FindShortestPath( idxStart, idxEnd, m_WayPoint, nDepth);
		if( m_WayPointCount > 1)
		{
			m_idxOldTargetPoint = m_idxCurTargetPoint;

			m_idxCurTargetPoint = m_WayPoint[1];

			m_CurWayPointIndex	= 1;
		}

		return;
	}
}

void CGameCharaInputControlAI::FindNextTargetPosition(void)
{
	if( m_pGamePath == nullptr)		return;
	if( m_idxCurTargetPoint == -1)
	{
		FindStartPosition();

		return;
	}

	m_WayPointCount = SelectNextPath( m_idxOldTargetPoint, m_idxCurTargetPoint);
	if( m_WayPointCount > 1)
	{
		m_idxOldTargetPoint = m_idxCurTargetPoint;

		m_idxCurTargetPoint = m_WayPoint[1];

		m_CurWayPointIndex	= 1;
	}
	else
	{
		m_WayPoint[0] = m_idxOldTargetPoint;

		m_idxOldTargetPoint = m_idxCurTargetPoint;

		m_idxCurTargetPoint = m_WayPoint[0];

		m_CurWayPointIndex = 0;

		m_WayPointCount = 1;
	}
}

INT32 CGameCharaInputControlAI::SelectNextPath( INT32 idxOld, INT32 idxCur)
{
	//�������̸� ���� �н��� �ǵ��� ���� �ʵ���
	INT32 cnt = m_pGamePath->getPointCount();
	if( cnt > 0)
	{
		INT32 next = i3Math::Rand()%cnt;
		while( next == idxCur)
		{
			next = i3Math::Rand()%cnt;
		}

		INT32 newPathCount = m_pGamePath->FindShortestPath( idxCur, next, m_WayPoint, MAX_PATH_DEPTH);
		if( newPathCount > 0)
		{
			//I3TRACE("SelectNextPath:%d \n", newPathCount);
			return newPathCount;
		}
	}

	//I3TRACE("SelectNextPath: FAILED \n");
	return -1;
}

void CGameCharaInputControlAI::SeeTargetPosition( VEC3D * pvPos, REAL32 rTime, bool bRho)
{
	VEC3D vDir;
	i3Vector::Sub( &vDir, pvPos, m_pOwner->GetPos());

	SeeTarget( &vDir, rTime, bRho);
}

void CGameCharaInputControlAI::SeeTargetPosition(i3::pack::VEC3D * pPos, REAL32 rTime, bool bRho)
{
	VEC3D vDir;
	i3Vector::Sub(&vDir, pPos, m_pOwner->GetPos());

	SeeTarget(&vDir, rTime, bRho);
}

void CGameCharaInputControlAI::OnInterpolateTheta( REAL32 rDeltaSeconds)
{
	if( m_rThetaTimer < m_InterpolateTime)
	{
		REAL32 cur		= getBoneContext()->getTheta();
		REAL32 ratio	= MINMAX( 0.0f, m_rThetaTimer/m_InterpolateTime, 1.0f);

		REAL32 old		= ( m_rOldTheta < 0.0f) ? m_rOldTheta + I3_2PI : m_rOldTheta;
		REAL32 target	= ( m_rTargetTheta < 0.0f) ? m_rTargetTheta + I3_2PI : m_rTargetTheta;

		REAL32 diff		= target - old;
		if( i3Math::abs( diff) > I3_PI)
		{
			cur	= old - i3Math::vsin( ratio) * i3Math::NormalizeAngle(I3_2PI - diff);
		}
		else
		{
			cur = old + i3Math::vsin( ratio) * diff;
		}

		cur = i3Math::NormalizeAngle( cur);

		if( m_rThetaTimer + rDeltaSeconds > m_InterpolateTime)
		{
			cur = m_rTargetTheta;
		}

		getBoneContext()->setTheta( cur);
	}

	m_rThetaTimer += rDeltaSeconds;
}

void CGameCharaInputControlAI::SeeTarget( VEC3D *pvDir, REAL32 Time, bool bRho)
{
	VEC3D vTarget;

	REAL32 fLen;
	i3Vector::Normalize( &vTarget, pvDir);

	m_rOldTheta		= i3Math::NormalizeAngle(getBoneContext()->getTheta());
	m_rTargetTheta  = i3Math::NormalizeAngle( i3Math::atan2( getX( &vTarget), getZ( &vTarget)));
	m_rThetaTimer	= 0.0f;
	m_InterpolateTime = Time;

	if(( m_nActionState & AI_ACTION_ATTACK) != 0)
	{
		m_rThetaTimer = m_InterpolateTime + 1.0f;

		getBoneContext()->setTheta( m_rTargetTheta);
	}

	if( bRho)
	{
		fLen			= i3Math::Sqrt( (getX(&vTarget) * getX(&vTarget)) + (getZ(&vTarget) * getZ(&vTarget)) );
		REAL32 rho		= atan2f( getY(&vTarget), fLen);

		getBoneContext()->setRho( rho);
	}
}

bool CGameCharaInputControlAI::MakeKeyInput( VEC3D * pvDir, MATRIX * pOwnerMtx)
{
	VEC3D vDir;
	MATRIX mtx, invMtx;
	i3Matrix::Copy( &mtx, pOwnerMtx);
	i3Matrix::SetPos( &mtx, 0.0f, 0.0f, 0.0f);
	i3Matrix::Inverse( &invMtx, nullptr, &mtx);

	i3Vector::TransformCoord( &vDir, pvDir, &invMtx);

	m_nMoveState &= ~0x000000000000000F;

	setY( &vDir, 0.0f);
	i3Vector::Normalize( &vDir, &vDir);

	REAL32	comp = 0.5f;
	if( m_bMoveFlag[0] == true)
	{
		comp = 0.293f;
	}

	if( i3Math::abs( vDir.z) > comp)
	{
		if( vDir.z > 0.0f)
		{
			m_nMoveState |= AI_MOVE_FORWARD;
		}
		else
		{
			m_nMoveState |= AI_MOVE_BACKWARD;
		}

		m_bMoveFlag[0] = true;
	}
	else
	{
		 m_bMoveFlag[0] = false;
	}

	comp = 0.5f;
	if( m_bMoveFlag[1] == true)
	{
		comp = 0.293f;
	}

	if( i3Math::abs( vDir.x) > comp)
	{
		if( vDir.x > 0.0f)
		{
			m_nMoveState |= AI_MOVE_LEFT;
		}
		else
		{
			m_nMoveState |= AI_MOVE_RIGHT;
		}

		m_bMoveFlag[1] = true;
	}
	else
	{
		m_bMoveFlag[1] = false;
	}

	return true;
}

bool CGameCharaInputControlAI::MoveToTarget( i3GameObj* pTarget, REAL32 minLength, REAL32 rDeltaSeconds)
{
	m_nMoveState = 0;

	VEC3D	vDir;
	i3Vector::Sub( &vDir, pTarget->GetPos(), m_pOwner->GetPos());
	if( i3Vector::Length( &vDir) < minLength)
	{
		return false;
	}

	MakeKeyInput( &vDir, m_pOwner->GetMatrix());

	return true;
}

AI_MOVE_RETURN_VALUE CGameCharaInputControlAI::MoveTo( VEC3D * vPos, REAL32 rDeltaTime)
{
	m_nMoveState = 0;
	m_nActionState &= ~AI_ACTION_JUMP;

	AI_MOVE_RETURN_VALUE ret = AI_MOVE_RET_OK;

	VEC3D	vDir;
	i3Vector::Sub( &vDir, vPos, m_pOwner->GetPos());

	m_rElapsedTraceTime += rDeltaTime;

	REAL32 rHeight		= getY( &vDir);
	REAL32 rDistance	= i3Vector::Normalize( &vDir, &vDir);
	if( rDistance <= 0.5f)
	{
		m_rElapsedJumpTime = 0.0f;
		ret = AI_MOVE_RET_ARRIVED;
	}

	if( m_rElapsedTraceTime > MINMAX( 2.0f, rDistance, 8.0f))
	{
//		I3TRACE("TIME ELAPSED: %f, distance : %f\n", m_rElapsedTraceTime, rDistance);
		m_rElapsedJumpTime = 0.0f;
		ret = AI_MOVE_RET_BLOCKED;
	}

	if( ret == AI_MOVE_RET_OK)
	{
		//�ɸ��� �κ��� ����� ����...;;
		m_rElapsedJumpTime += rDeltaTime;

		if( m_rElapsedJumpTime > 0.5f)
		{
			m_rElapsedJumpTime = 0.0f;

			REAL32 fLen;
			fLen		= i3Math::Sqrt( (getX(&vDir) * getX(&vDir)) + (getZ(&vDir) * getZ(&vDir)) );
			REAL32 rho	= atan2f( getY(&vDir), fLen);

			if( rho > 0.3f || rHeight > 0.3f)
			{
				m_nActionState |= AI_ACTION_JUMP;
			}
		}
	}

	MakeKeyInput( &vDir, m_pOwner->GetMatrix());

	return ret;
}

WEAPON_SLOT_TYPE CGameCharaInputControlAI::_GetAvailableWeapon(void)
{
	WeaponBase * pWeaponPrimary = getWeaponContext()->getWeapon( WEAPON_SLOT_PRIMARY);
	WeaponBase * pWeaponSecond = getWeaponContext()->getWeapon( WEAPON_SLOT_SECONDARY);

	if( pWeaponPrimary == nullptr)
	{
		//I3PRINTLOG(I3LOG_WARN,  "no weapon AI %s", BattleSlotContext::i()->getNickForSlot( m_pOwner->getCharaNetIndex()));

		if( pWeaponSecond == nullptr)
		{
			return WEAPON_SLOT_MELEE;
		}
		else
		{
			if( pWeaponSecond->hasLoadedBullet() == false && pWeaponSecond->getTotalBulletCount() <= 0)
			{
				return WEAPON_SLOT_MELEE;
			}
			else
			{
				return WEAPON_SLOT_SECONDARY;
			}
		}		
	}
	else if(pWeaponSecond == nullptr)
	{
		return WEAPON_SLOT_MELEE;
	}
	else if( pWeaponPrimary->hasLoadedBullet() == false && pWeaponPrimary->getTotalBulletCount() <= 0)//������ �Ѿ˵� ����..��..���� źâ�� ����...
	{
		if( pWeaponSecond->hasLoadedBullet() == false && pWeaponSecond->getTotalBulletCount() <= 0)
		{
			return WEAPON_SLOT_MELEE;
		}
		else
		{
			return WEAPON_SLOT_SECONDARY;
		}
	}
	else
	{
		return WEAPON_SLOT_PRIMARY;
	}
}

bool CGameCharaInputControlAI::_SwapAvailableWeapon()
{
	if( getWeaponContext() == nullptr)	return false;

	INT32 curSlot = getWeaponContext()->getCurrentSlotNum();

	INT32 nSlot = _GetAvailableWeapon();
	if( nSlot != curSlot)
	{
		switch( nSlot)
		{
		case 0 : m_nActionState |= AI_ACTION_SWAPWEAPON0;
			break;
		case 1 : m_nActionState |= AI_ACTION_SWAPWEAPON1;
			break;
		case 2 : m_nActionState |= AI_ACTION_SWAPWEAPON2;
			break;
		}
	}

	return true;
}

bool CGameCharaInputControlAI::SelectWeapon()
{
	if( m_pOwner->IsDino())
		return false;

	I3ASSERT( getWeaponContext() != nullptr);
	
	REAL32 min = 3.0f;

	INT32 curSlot = getWeaponContext()->getCurrentSlotNum();

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_CurrentTargetIdx);

	if(( m_nActionState & AI_ACTION_ATTACK) != 0 && (pChara != nullptr))
	{
		VEC3D vDiff;
		i3Vector::Sub(  &vDiff, pChara->GetPos(), m_pOwner->GetPos());
		REAL32 length = i3Vector::Length( &vDiff);

		if( length > min)
		{
			_SwapAvailableWeapon();	
		}
		else
		{
			INT32 nSlot = _GetAvailableWeapon();
			if( nSlot != curSlot)
			{
				m_nActionState |= AI_ACTION_SWAPWEAPON2;
			}
		}
	}
	else
	{
		if( m_nAIState == AI_STATE_KEEPWATCH || m_nAIState == AI_STATE_REMOVE_OBJ)
		{
			_SwapAvailableWeapon();	
		}
		else
		{
			if( m_idxOldTargetPoint != -1)
			{
				i3::pack::PATH_POINT_INFO * pPoint		= m_pGamePath->getPoint( m_idxCurTargetPoint);
				i3::pack::PATH_POINT_INFO * pOldPoint  = m_pGamePath->getPoint( m_idxOldTargetPoint);

				VEC3D vDiff;
				i3Vector::Sub( &vDiff, &pPoint->m_Pos, &pOldPoint->m_Pos);
				if( i3Vector::Length( &vDiff) > m_AIData._SwapWeaponDistance)
				{
					INT32 nRand = i3Math::Rand()%100;
					if( nRand < m_AIData._SwapRatioHandgun)
					{
						m_nActionState |= AI_ACTION_SWAPWEAPON1;
					}
					else if( nRand >= m_AIData._SwapRatioHandgun && nRand < m_AIData._SwapRatioKnife + m_AIData._SwapRatioHandgun)
					{
						m_nActionState |= AI_ACTION_SWAPWEAPON2;
					}
				}
			}
			else
			{
				_SwapAvailableWeapon();	
			}
		}
	}

	return true;
}

bool CGameCharaInputControlAI::FightWidth( INT32 idx)
{
	CGameCharaBase * pTarget = g_pCharaManager->getCharaByNetIdx( idx);
	if( pTarget == nullptr ) return false;

	if( m_CurrentTargetIdx != idx)
	{
		m_rElapsedAutoFireTime	= 0.0f;
		m_CurrentTargetIdx = idx;
	}

	m_nActionState |= AI_ACTION_ATTACK;

	SelectWeapon();
	
	return true;
}

REAL32 CGameCharaInputControlAI::GetAttackDirection( CGameCharaBase * pTargetChara, VEC3D * pvStart, VEC3D * pvDir, bool bForce)
{
	REAL32 rLen;
	CGameCharaBoneContext * pBoneCtx = pTargetChara->getBoneContext();
	if( pBoneCtx == nullptr)
	{
		//I3TRACE("Target Character's BoneContext is Null\n");
		return -1.0f;
	}

	MATRIX * pMatrixNeck = pBoneCtx->getBoneMatrix( BONE_HEAD);

	m_pOwner->getCameraContext()->getVirtualCamPosition( pvStart);
	VEC3D vEnd;
	i3Vector::Copy( &vEnd, i3Matrix::GetPos( pMatrixNeck));
	i3Vector::AddY( &vEnd, - 0.25f);

	if( bForce)
		i3Vector::AddY( &vEnd, - m_AIData._TargetHeightUnerNeck);
	
	i3Vector::Sub( pvDir, &vEnd, pvStart);

	rLen = i3Vector::Normalize( pvDir, pvDir);

	return rLen;
}

INT32 CGameCharaInputControlAI::FindTarget()
{
	WeaponBase* pWeapon = nullptr;

	if( getWeaponContext() == nullptr)	
	{
		if(m_pOwner->IsDino())
		{
			pWeapon = ((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->GetDinoWeapon();
		}
		else
		{
			return -1;
		}
	}
	else
	{
		pWeapon = m_pOwner->GetCurrentCharaWeapon();
	}
		
	if( pWeapon == nullptr)	return -1;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	INT32	nTargetIndex = -1;
	REAL32	rMinLength = pWeaponInfo->GetRange();
	if( rMinLength < 30.0f)	rMinLength = 30.0f;

	UINT32 nColGroup = CGRP_TEAM_BLUE_ROUGH;
	if( (m_pOwner->getCharaInfo()->GetTeam() & CHARACTER_TEAM_RED))
	{
		nColGroup = CGRP_TEAM_RED_ROUGH;
	}

	for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if( pChara == nullptr )	continue;
		if( m_pOwner == pChara)
			continue;

		if( pChara->isCharaStateMask( CHARA_STATE_DEATH))
		{
			if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) && !m_bAttack_Death_Player)
				continue;
		}

		// �� üũ
		if( m_pOwner->getCharaTeam() == pChara->getCharaTeam())	
			continue;

		REAL32 rLen;
		VEC3D vStart, vDir;
		rLen = GetAttackDirection( pChara, &vStart, &vDir, false);
		if( rLen < 0.0f)
			continue;
		
		REAL32 rDot = i3Vector::Dot( &vDir, i3Matrix::GetAt( m_pOwner->GetMatrix()));

		//�þ߳��� ���� �ְų� ���� �ּҰŸ� ���� ������
		if( rDot > m_AIData._Angle || (rLen < m_AIData._Range)) 
		{
			i3CollideeLine line;
			line.SetStart( &vStart);
			line.SetDir( &vDir);

			I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ONLYWORLDHIT);
			if( pHitResult != nullptr)
			{
				if( pHitResult->m_T > rLen)
				{
					if( rLen < rMinLength)
					{
						rMinLength	= rLen;
						nTargetIndex	= pChara->getCharaNetIndex();
					}
				}
			}
			else
			{
				if( rLen < rMinLength)
				{
					rMinLength	= rLen;
					nTargetIndex	= pChara->getCharaNetIndex();
				}
			}
		}
	}

	return nTargetIndex;
}

void CGameCharaInputControlAI::StopAttack( void)
{
	m_nActionState &= ~0x0000000000000FFF;
	m_nActionState &= ~AI_ACTION_FIRE;

	m_rElapsedAutoFireTime = 0.0f;

	ActivateExtension( false);

	m_bAttacking_Death_Player = false;
}

void CGameCharaInputControlAI::Attack( VEC3D *pvTarget, REAL32 rDeltatime)
{
	SeeTarget( pvTarget, 0.0f, true);

	m_nActionState |= AI_ACTION_FIRE;

	m_bAttacking_Death_Player = false;
	
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_CurrentTargetIdx);
	if( pChara != nullptr )
	{
		if( pChara->isCharaStateMask( CHARA_STATE_DEATH) )
		{
			m_bAttacking_Death_Player = true;
		}
	}
}

void CGameCharaInputControlAI::Attack( INT32 target, REAL32 rDeltatime)
{
	if( target == -1)	return;
	CGameCharaBase * pTarget = g_pCharaManager->getCharaByNetIdx( target);
	if( pTarget == nullptr )return;

	Attack( pTarget, rDeltatime);
}

void CGameCharaInputControlAI::Attack( i3GameObj * pTarget, REAL32 rDeltatime)
{
	if( pTarget == nullptr) return;

	VEC3D vDir;
	i3Vector::Sub( &vDir, pTarget->GetPos(), m_pOwner->GetPos());

	Attack( &vDir, rDeltatime);
}

REAL32 CGameCharaInputControlAI::GetTargetRate(void)
{
	return m_AIData._Deviation;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGameCharaInputControlAI::ProcessDropWeapon( REAL32 rDeltatime)
{
	UINT64	actionState = getActionState();

	if( actionState & AI_ACTION_DROPWEAPON)
	{
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) && !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_DROPWEAPON))
		{
			WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();

			if( pWeapon != nullptr)
			{
				GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, getWeaponContext()->getCurrentSlotNum(), pWeapon->getLoadedBulletCount(),
					pWeapon->getLoadedBulletDualCount(), pWeapon->getTotalBulletCount());
			}

			// ���⸦ ������ 0.5�ʰ� �߻縦 ���ϵ��� �����մϴ�.
			m_pOwner->setDropWeaponTimer( 0.5f);
		}
	}
}

bool CGameCharaInputControlAI::ProcessCrouch( REAL32 rDeltatime)
{
	bool	bChangeBody = false;
	UINT64	actionState = getActionState();

	if(m_pOwner->IsDino()) {
		return false;
	}

	if( actionState & AI_ACTION_CROUCH )
	{	// �ɱ�
		bChangeBody = m_pOwner->Cmd_Crouch();
	}
	else
	{	// �Ͼ��
		bChangeBody = m_pOwner->Cmd_StandUp();
	}

	return bChangeBody;
}


// -------------------------------------  �̵�  ----------------------------------------------------
void CGameCharaInputControlAI::ProcessMovement( REAL32 rDeltatime, bool bChangeBody)
{	
	VEC3D vOldDir;
	WeaponBase * pWeapon = nullptr;

	pWeapon = m_pOwner->GetCurrentCharaWeapon();

	CGameCharaActionContext * pActionCtx = getActionContext();
	CGameCharaMoveContext * pMoveCtx = getMoveContext();

	VEC3D * pMoveDir = pMoveCtx->getMoveDir();

	bool bHaveUpperPlay = false;
	CHARA_ACTION_UPPER nActionUpper = pActionCtx->getUpper();
	if( (nActionUpper == CHARA_UPPER_IDLE) || (nActionUpper == CHARA_UPPER_MOVE) )
		bHaveUpperPlay = true;

	
	if( (pWeapon != nullptr) && pWeapon->isDualGun())
	{
		CHARA_ACTION_UPPER nActionUpper_Left = pActionCtx->getUpperLeft();
		if( (nActionUpper_Left == CHARA_UPPER_IDLE) || (nActionUpper_Left == CHARA_UPPER_MOVE) )
			bHaveUpperPlay = true;
	}

	UINT64	moveState = getMoveState();
	if( !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_MOVE) &&		//	�̵��� �����ϸ�?
		!pActionCtx->isLower( CHARA_LOWER_JUMP))						//	�������� �ƴϸ�?
	{
		if( (moveState & AI_MOVE_LEFT) || (moveState & AI_MOVE_RIGHT)
			|| (moveState & AI_MOVE_FORWARD) || (moveState & AI_MOVE_BACKWARD))
		{
			i3Vector::Copy( &vOldDir, pMoveDir);

			SetMoveDir( pMoveDir);	//	�Է�Ű�� ���� �̵� ���� ����

			if( getActionState() & AI_ACTION_FOOTSTEP )
				m_pOwner->Cmd_Walk( pMoveDir);			//	�ȱ�
			else
				m_pOwner->Cmd_Run( pMoveDir);	//	�ٱ�

			/*if( bHaveUpperPlay)						pActionCtx->_Play_Upper_Run();*/
		}	
	}

	///////////////////////////////////////////////////////////		����
	if( !(moveState & AI_MOVE_LEFT) && !(moveState & AI_MOVE_RIGHT) && !(moveState & AI_MOVE_FORWARD)
		&& !(moveState & AI_MOVE_BACKWARD) || m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_MOVE) )
	{	
		//	�̵� �߿� ����
		if( pActionCtx->isLower( CHARA_LOWER_RUN) || pActionCtx->isLower( CHARA_LOWER_WALK))
		{
			i3Vector::Zero( pMoveDir);

			m_pOwner->Cmd_Idle_Lower();
			m_pOwner->Cmd_Idle_Upper();
		}			
	}

	// revision 53646 HK417 ������� �߰�
	if( (moveState & AI_MOVE_LEFT) || (moveState & AI_MOVE_RIGHT) || (moveState & AI_MOVE_FORWARD) || (moveState & AI_MOVE_BACKWARD))
	{	
		//	�̵� �߿� ����
		if( pActionCtx->isLower( CHARA_LOWER_RUN) && (m_pOwner->getRunParalysis() > 0))
		{
			i3Vector::Zero( pMoveDir);

			m_pOwner->Cmd_Idle_Lower();
			m_pOwner->Cmd_Idle_Upper();
		}			
		else if( pActionCtx->isLower( CHARA_LOWER_WALK)&& (m_pOwner->getWalkParalysis() > 0))
		{
			i3Vector::Zero( pMoveDir);

			m_pOwner->Cmd_Idle_Lower();
			m_pOwner->Cmd_Idle_Upper();
		}
	}
}

void CGameCharaInputControlAI::ProcessWeapon( REAL32 rDeltatime)
{	
	WeaponBase * pCurWeapon = nullptr;

	if(m_pOwner->IsDino())
	{
		pCurWeapon = ((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->GetDinoWeapon();
	}
	else
	{
		pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	}
	
	if( pCurWeapon == nullptr)	return;

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	WEAPON::eSTATE	weaponState = pCurWeapon->getWeaponState();

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);


	bool bCancelMission = false;	//	�̼� �ൿ ��� ����
	bool bCancelDisarm = false;		//	��ź ��ü ��� ����

	UINT64 actionState = getActionState();

	if( actionState & AI_ACTION_FIRE )
	{	
		bool bFire = false;

		if( i3::kind_of<WeaponGrenadeLauncher*>(pCurWeapon))
		{
			if( pCurWeapon->isExtensionActivate())
				bFire = true;
		}
		else if( i3::same_of<WeaponBombTrigger*>(pCurWeapon))
			bFire = true;
		else if( pCurWeaponInfo->GetTypeUsage() == WEAPON_SLOT_MELEE )
			bFire = true;

		if( (!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) && (!pCurWeapon->isWeaponStateMask( WSM_FIRING)))
			&& (pCurWeapon->hasLoadedBullet() || bFire) && (m_pOwner->getDropWeaponTimer() < 0.f) )	// ��� ���� Ÿ�̸Ӱ� �ִٸ� �߻縦 ���ϵ��� �մϴ�.
		{
			if( pCurWeapon->isFireEnabled() )
			{
				//	Mission ���� ����ó��
				if( pCurWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION)
				{//C4ó���ϴ� �ڵ� �Դϴ�. ������ �̻��մϴ�. 
					//	�ൿ
					if( !m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) && m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) )
					{
						//��ġ�� �ϴ� �����Դϴ�. 
						if( m_pOwner->Cmd_Attack())
						{
							bCancelMission = false;
						}
					}
				}
				else
				{
					//	��ź ��ü ���϶� �߻� �Ұ�
					if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) == false)
					{
						//	���� �߻�!!		(��ô ���⸸ �߻��غ�)
						switch(pCurWeaponInfo->GetTypeClass())
						{
						case WEAPON_CLASS_THROWING_GRENADE:
						case WEAPON_CLASS_THROWING_CONTAIN:
						case WEAPON_CLASS_THROWING_ATTACK:
						case WEAPON_CLASS_THROWING_HEAL:
							{
								if( !pCurWeapon->isWeaponStateMask( WSM_FIRING))
								{
									bCancelDisarm = true;
									m_pOwner->Cmd_Attack( WEAPON::READY_FIRE);
								}
							}
							break;
						default:
							{
								SeeTarget( &m_vTargetDir, true);
								bCancelDisarm = true;
								m_pOwner->Cmd_Attack();
							}
							break;
						}
					}
				}
			}
			else if( pCurWeapon->getWeaponInfo()->GetTypeTrigger() == WEAPON::TRIGGER_RELOADING_SHOT )
			{	
				// ������ �߻� �����ϸ� ź�� ���� ������ ���Ѵ�(����)
				bCancelMission = m_pOwner->Cmd_Reload();
			}
		}
		else
		{
			// �ڵ����� ������ �ϴ� ���
			switch( weaponState)
			{
				case WEAPON::STATE_NEEDTO_LOADMAG :
					{
						ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

						if( pCurWeapon->getWeaponInfo()->IsLoadMagazineReady() && pCurWeapon->isReadyToLoadMagazine() == false &&
							getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
							nLoadType = ACTION_LOAD_READY_MAG;

						bCancelMission = m_pOwner->Cmd_Reload(nLoadType);
					}
					break;
				case WEAPON::STATE_NEEDTO_LOADBULLET :
					{
						if( pCurWeapon->IsFireBulletWeapon())
						{
							bCancelMission = m_pOwner->Cmd_Reload( ACTION_LOAD_FIRE_BULLET);	
						}
						else
						{
							bCancelMission = m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);	
						}
					}
					break;
				case WEAPON::STATE_NEEDTO_REMOVESHELL :		
					{
						bCancelMission = m_pOwner->Cmd_Reload();	
					}
					break;
				case WEAPON::STATE_NEEDTO_FIRE:
					{
						SeeTarget( &m_vTargetDir, true);
						m_pOwner->Cmd_Attack();
						bCancelDisarm = true;
					}break;
			}
		}
	}
	else if( pCurWeapon->isWeaponStateMask( WSM_FIRING))//	�߻� Ŭ���� ��ư�� ���� (Key Stroke ����)
	{
		bCancelMission = true;  

		//	��ô ������ ��� ��ư�� ������ ������.
		if( pCurWeapon->isWeaponStateMask( WSM_FIRING) )
		{
			switch(pCurWeaponInfo->GetTypeClass())
			{
			case WEAPON_CLASS_THROWING_GRENADE:
			case WEAPON_CLASS_THROWING_CONTAIN:
			case WEAPON_CLASS_THROWING_ATTACK:
			case WEAPON_CLASS_THROWING_HEAL:
				{
					if( getActionContext()->isUpper( CHARA_UPPER_FIRE_READY) &&
						!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) && pCurWeapon->getLoadedBulletCount() > 0)
					{
						SeeTarget( &m_vTargetDir, true);
						bCancelDisarm = m_pOwner->Cmd_Attack();
					}
				}
				break;
			}
		}

		pCurWeapon->FireEnd();		//	�߻� ���� ������ ���⿡ �˷��ش�.
	}

	//�Ѿ��� ���ڶ� ������ΰ�� �ݹ߽õ��� �ϰԵǸ� ���� �Ҹ�
	if( actionState & AI_ACTION_FIRE )
	{
		if( (weaponState == WEAPON::STATE_EMPTY_MAGAZINE) )
		{
			if( pCurWeaponInfo->isGun() )
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand()%2, m_pOwner->GetPos(), m_pOwner->is1PV());

				SelectWeapon();
			}
		}
		else if( pCurWeapon->isExtensionActivate() &&
			((pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER)) &&			
			(((WeaponGrenadeLauncher*)pCurWeapon)->getTotalShellCount() <= 0) && 
			(((WeaponGrenadeLauncher*)pCurWeapon)->getLoadedShellCount() <= 0) )
		{
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand()%2, m_pOwner->GetPos(), m_pOwner->is1PV());

			m_pOwner->Cmd_Extension( false);

			// ��ȹ�� ��û���� Ŀ���� M4�� ��ź/LSS ���� źȯ ��� ������ �ͽ��ټ��� deavtivate�ǰ� �⺻ ���� ���ư���.
			if( pCurWeapon->isMultipleExtension())
				pCurWeapon->SwapExtensionType();
		}
		else if( pCurWeapon->isExtensionActivate() &&
			((pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER)) &&			
			(((WeaponShotGunLauncher*)pCurWeapon)->getTotalShellCount() <= 0) && 
			(((WeaponShotGunLauncher*)pCurWeapon)->getLoadedShellCount() <= 0) )
		{
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand()%2, m_pOwner->GetPos(), m_pOwner->is1PV());

			m_pOwner->Cmd_Extension( false);

			// ��ȹ�� ��û���� Ŀ���� M4�� ��ź/LSS ���� źȯ ��� ������ �ͽ��ټ��� deavtivate�ǰ� �⺻ ���� ���ư���.
			if( pCurWeapon->isMultipleExtension())
				pCurWeapon->SwapExtensionType();
		}
	}


	////////////////////////////////////////////////////////////	���� ����	
	if( actionState & AI_ACTION_SUBFUNC )	//	<- Pressed ��
	{
		bool bEnableWeapon = false;

		switch( pCurWeaponInfo->GetTypeClass())
		{
		case WEAPON_CLASS_KNIFE:			bEnableWeapon = true;			break;
		case WEAPON_CLASS_KNUCKLE:			bEnableWeapon = true;			break;
		// KEYWORD : WEAPON_C5
		case WEAPON_CLASS_BOMBTRIGGER :
			if( pCurWeapon->getLoadedBulletCount() > 0 )	// źȯ�� �ִ� ��� ����
				bEnableWeapon = true;
			break;
		case WEAPON_CLASS_DUALKNIFE :
		case WEAPON_CLASS_CIC :				bEnableWeapon = true;			break;
		}

		if( bEnableWeapon )
		{
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) &&					//	��ź ��ü ���� �ƴϰ�
				(!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) &&				//	�߻� �Ұ��� ���°� �ƴϰ�
				pCurWeapon->isFireEnabled() && !pCurWeapon->isWeaponStateMask( WSM_FIRING)))	// ������ �߻簡 ������ ���
			{	
				bCancelDisarm = m_pOwner->Cmd_Attack( WEAPON::SECONDARY_FIRE);
			}
		}
		
	}

	////////////////////////////////////////////////////////////	���ε�
	if( actionState & AI_ACTION_RELOAD )
	{
		if( pCurWeapon->isLoadMagazineEnabled() && !(actionState & AI_ACTION_SPECIAL) )
		{
			ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

			if( pCurWeapon->getWeaponInfo()->IsLoadMagazineReady() && pCurWeapon->isReadyToLoadMagazine() == false &&
				getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
				nLoadType = ACTION_LOAD_READY_MAG;

			bCancelMission = m_pOwner->Cmd_Reload(nLoadType);
		}
	}

	////////////////////////////////////////////////////////////	���� ��ü	
	if( !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE))
	{
		if( actionState & AI_ACTION_SWAPWEAPON0 )
		{
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY);
		}
		else if( actionState & AI_ACTION_SWAPWEAPON1 )
		{	
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_SECONDARY);
		}
		else if( actionState & AI_ACTION_SWAPWEAPON2 )
		{
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_MELEE);
		}
		else if( actionState & AI_ACTION_SWAPWEAPON3 )
		{
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_THROWING1);
		}
		else if( actionState & AI_ACTION_SWAPWEAPON4 )
		{
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_THROWING2);
		}
		else if( actionState & AI_ACTION_SWAPWEAPON5 )
		{
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_MISSION);
		}

		m_nActionState &= ~0x00000000000FF000;	//SWAP WEAPON FLAG
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	�� ���� Ư�� ��ư�̿ܿ��� bCancelMission�� ���õǾ� �ٸ� ���� ��ư�� �����ؼ� �ȵ˴ϴ�.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////	Ư�� ��ư (��ź ��ü��)
	if( actionState & AI_ACTION_SPECIAL )
	{	
		if( !bCancelDisarm)
		{
			//	�̼��� �� ���
			if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) ||
				MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
			{			
				//	�̼� ������
				if( (pMissionMng->getMissionState() == GAME_MISSION_STATE_INSTALLED ))
				{
					//	��Ʈ��ũ ��ź ��ü
					m_pOwner->Cmd_Uninstall();
				}//	�̼� ����
				else if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
				{
					m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
					m_pOwner->removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);
				}
			}
		}
	}
	else if( (actionState & GAME_KEY_ACTION_SPECIAL) == 0)
	{	//	�̼��� �� ���
		if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
		{
			bCancelDisarm = true;
		}
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
		if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB))		
		{
			m_pOwner->Cmd_Uninstall( false);	//	��Ʈ��ũ ��ź ��ü ���	
		}
	}
}

void CGameCharaInputControlAI::ProcessExtension(void)
{
	WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pCurWeapon == nullptr)
		return;

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	if( getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) || getActionContext()->isUpper(CHARA_UPPER_LOADBULLET) ||
		getActionContext()->isUpper(CHARA_UPPER_FIREBULLET) || getActionContext()->isUpper(CHARA_UPPER_SWAPWEAPON) || 
		getActionContext()->isUpper(CHARA_UPPER_EXTENSION_ACTIVATE) || getActionContext()->isUpper(CHARA_UPPER_EXTENSION_DEACTIVATE) )
		return;

	UINT64 actionState = getActionState();

	switch( pCurWeapon->GetCurrentExtensionType())
	{
	default :
		// Extension�� ���� ���� ���...
		if( actionState & AI_ACTION_SUBFUNC )
		{
			switch( pCurWeaponInfo->GetTypeZoom())
			{
				case WEAPON::ZOOM_SCOPE :	m_pOwner->Cmd_ToggleZoom();		break;		// ������ ��� ���
			}
		}
		break;
	case WEAPON::EXT_SCOPE :
	case WEAPON::EXT_ZOOM :					// ����
		// K2, K1
		if( actionState & AI_ACTION_SUBFUNC )
		{
			// ������ ��� ���
			m_pOwner->Cmd_ToggleZoom();
		}
		break;

	case WEAPON::EXT_STEADY_AIM :
		// Spectre
		if( actionState & AI_ACTION_SUBFUNC )
		{
			// ������ ��� ���
			m_pOwner->Cmd_ToggleZoom();
		}
		break;

		// Activate �ܰ� ���� ����ϴ� Extension �����
	case WEAPON::EXT_DUAL_MAG :		// AK47
	case WEAPON::EXT_SILENCER :		// ���� ���ÿ� Activate
		// MP5K	// MK23
		if( actionState & AI_ACTION_SUBFUNC )
		{
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;
	case WEAPON::EXT_DOT_SIGHT :
		if( actionState & AI_ACTION_SUBFUNC )
		{
			// ������ ��� ���
			m_pOwner->Cmd_ToggleZoom();
		}
		break;
	case WEAPON::EXT_GLAUNCHER:
	case WEAPON::EXT_SHOTGUN_LAUNCHER:
		if( actionState & AI_ACTION_SUBFUNC )
		{
			//	���� Extension Ȱ��ȭ ���
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;
	case WEAPON::EXT_BAYONET:
		if( actionState & AI_ACTION_SUBFUNC )
		{
			//	���� Extension Ȱ��ȭ ���
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;

	case	WEAPON::EXT_CROSSFIRE :
		if( actionState & AI_ACTION_SUBFUNC )
		{
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;
	}


	m_nActionState &= ~AI_ACTION_EXT_ACTIVATE;
	m_nActionState &= ~AI_ACTION_SUBFUNC;
}

void CGameCharaInputControlAI::ProcessInput( REAL32 rDeltatime)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		return;
	}

	if ( CGameMaster::i()->IsPaused() )
	{
		return;
	}

	if( m_pOwner->isCharaStateMask( CHARA_STATE_FIRSTRESPAWN) )
	{
		// ���� ��� ���� üũ
		ProcessDropWeapon( rDeltatime);
	}

	//	�̵� ���� ó���մϴ�.
	bool bChangeBody = false;

	///////////////////////////////////////////////////////////		�ɾ�/�Ͼ
	bChangeBody = ProcessCrouch( rDeltatime);

	///////////////////////////////////////////////////////////		����
	if( m_nActionState & AI_ACTION_JUMP )
	{
		if( (!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_MOVE) &&
			(m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) || m_pOwner->isCharaStateMask( CHARA_STATE_CLIMB)))
			&& !m_pOwner->isCharaStateMask( CHARA_STATE_SLOPESLIDE) )
		{
			m_pOwner->Cmd_JumpStart();

			m_nActionState &= ~AI_ACTION_JUMP;
		}		
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

void CGameCharaInputControlAI::ProcessDinoAttack( REAL32 rDeltatime)
{
	//�ɾ��ִ°�� ���� ó���� ���� �ʽ��ϴ�.
	if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
	{	
		getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
	}


	//bool bCancelMission = false;	//	�̼� �ൿ ��� ����

	UINT64 actionState = getActionState();

	((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->SetNowAttackFirstFrame(false);

	if( actionState & AI_ACTION_FIRE )
	{	
		if( (m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) == false) && (m_pOwner->isDeathblowState() == false))
		{
			if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
			{	
				getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
			}

			((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->DinoAttack(WEAPON::PRIMARY_FIRE);
		}
	}

	////////////////////////////////////////////////////////////	���� ����	
	if( actionState & AI_ACTION_SUBFUNC )	//	<- Pressed ��
	{
		if((m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) == false ) && (m_pOwner->isDeathblowState() == false))
		{
			((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->DinoAttack(WEAPON::SECONDARY_FIRE);
		}
	}
}



struct TMPOBJ_INFO
{
	i3GameObj * pObj = nullptr;
	VEC3D		vPos;
};

bool CGameCharaInputControlAI::DestroyIIObject( REAL32 rDeltaSeconds)
{
	for( size_t i = 0;i < m_RemoveObjectList.size(); ++i)
	{
		TMPOBJ_INFO * pInfo = m_RemoveObjectList[i];
		
		I3MEM_SAFE_FREE( pInfo);
	}
	m_RemoveObjectList.clear();
	

	if( _CheckObjectCollision( m_RemoveObjectList, 1.0f, rDeltaSeconds) > 0)
	{
		m_nAIState = AI_STATE_REMOVE_OBJ;
		SelectWeapon();

		m_nActionState |= AI_ACTION_ATTACK;
		return true;
	}
	else
		return false;
}

void CGameCharaInputControlAI::OnRemoveObject( REAL32 rDeltaSeconds)
{
	size_t nCnt = m_RemoveObjectList.size();

	if( nCnt > 0)
	{
		TMPOBJ_INFO * pInfo = m_RemoveObjectList[0];

		i3Object * pObj = (i3Object*)pInfo->pObj;
		if( pObj->getCurrentHP() <= 0.0f)
		{
			i3::vu::remove(m_RemoveObjectList, pInfo);

			I3MEM_SAFE_FREE( pInfo);
		}
		else
		{
			VEC3D vStart;
			VEC3D vDir;

			m_pOwner->getCameraContext()->getVirtualCamPosition( &vStart);

			i3Vector::Sub( &vDir, &pInfo->vPos, &vStart);
			i3Vector::Normalize( &m_vTargetDir, &vDir);

 			SeeTarget( &m_vTargetDir, 0.0f, true);

			m_nActionState |= AI_ACTION_FIRE;
		}
	}
	else
	{
		m_nAIState = AI_STATE_GOPATH;
		m_nActionState |= AI_ACTION_STOPATTACK;
		m_nActionState &= ~AI_ACTION_ATTACK;
	}
}

INT32 CGameCharaInputControlAI::_CheckObjectCollision( i3::vector<TMPOBJ_INFO*>& List, REAL32 rRange, REAL32 rDeltaSeconds)
{
	INT32		i;
	i3Object *	pGameObj;
	
	CStageBattle * pStage = g_pFramework->GetStageBattle();
	if( pStage == nullptr )
		return 0;
	
	StageObjectListPtr pObjectList = pStage->GetStageObjectList();

	if( pObjectList == nullptr )
		return 0;

	I3ASSERT( rRange > I3_EPS);

	NxScene * pNxScene = g_pPhysixCtx->getScene();
	NxShape* pShapeList[ 256];
	
	NxSphere nxSphere;
	nxSphere.center.x = getX( m_pOwner->GetPos());
	nxSphere.center.y = getY( m_pOwner->GetPos()) + rRange * 0.5f;
	nxSphere.center.z = getZ( m_pOwner->GetPos());

	nxSphere.radius = rRange;

	INT32 iActiveGrp = (1<<CGRP_OBJECT_HIT) | (1<<CGRP_OBJECT_ALL) | (1<<CGRP_OBJECT_WHOLE) | (1<<CGRP_OBJECT_WHOLE_DYNAMIC);
    
	INT32 nOverlapCount = pNxScene->overlapSphereShapes( nxSphere, NX_DYNAMIC_SHAPES, 256, pShapeList, nullptr, iActiveGrp);

	//I3TRACE("�ɸ��� ������Ʈ %d�� �߰�!! ���μ���\n", nOverlapCount);

	INT32 nCnt = 0;
	for( i = 0; i < nOverlapCount; i++ )
	{
		i3EventReceiver * pReceiver = (i3EventReceiver*) pShapeList[ i]->userData;
		if( pReceiver == nullptr)
			continue;

		if( !i3::same_of<i3PhysixShapeSet*>(pReceiver))
			continue;

		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pReceiver;
		if( !i3::kind_of<i3Object*>(pShapeSet->getEventReceiver()))
			continue;

		pGameObj = (i3Object*)pShapeSet->getEventReceiver();
		I3ASSERT( pGameObj->getNetIndex() > -1);

		// HP�� �ִ� ������Ʈ�� 0�̸� ��ŵ
		if( (pGameObj->getStateCount() > 1) && (pGameObj->getCurrentHP() == 0.f) )
			continue;

		//ã�� ������Ʈ�� ���� �ڽ��� �׳� �н� �̰� ����� �ֵ��� ��� ���ϴ�.
		if( i3::same_of<CGameObjectWeaponBox*>(pGameObj) )
			continue;

		TMPOBJ_INFO * pInfo = (TMPOBJ_INFO*)i3MemAlloc( sizeof( TMPOBJ_INFO));

		pInfo->pObj = pGameObj;
		pShapeSet->getShapeMassCenterPos( &pInfo->vPos);

		List.push_back( pInfo);

		nCnt++;
	}

	return nCnt;
}

#include "pch.h"
#include "GameMissionManager.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameObject.h"
#include "ObjectManager.h"

#include "GameCharaWeaponContext.h"
#include "GameCharaEquipContext.h"
#include "GameStateMgr.h"
#include "BattleHud.h"
#include "UI/HUD/UIHUD_Domination_Manager.h"
#include "GameCharaCameraContext.h"

#include "./StageBattle/UserContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleSyncTimer.h"

#include "BattleObjContext.h"
#include "Weapon/WeaponItemID.h"

#include "ClientStageInfoUtil.h"

#define BOMBINSTALLAREA			2.f			// 
#define BOMBUNINSTALLAREA	 	2.f
#define UDPSENDTIME_MISSION		0.1f		// Mission Packet �ʴ� 10ȸ
#define INTERVAL_DAMAGEOBJECT	0.01f		// 1�ۼ�Ʈ ������ ������ �˷��ݴϴ�.
#define SENDTIME_DESTRUCTIONMODE	10.f	// �ּ� 10�ʿ� �ѹ��� ��ũ�� ����ϴ�.

const REAL32 DEFAULT_RESPAWN_TIME		= 5.f;
const REAL32 DEFAULT_INVINCIBLE_TIME	= 4.f;
const REAL32 CHAOS_INVINCIBLE_TIME	= 6.f;

#if 0
#define CASHITEM_TRACE	I3TRACE
#else
#define CASHITEM_TRACE	__noop
#endif


CGameMissionManager::CGameMissionManager()
{
	m_MissionState = GAME_MISSION_STATE_NONE;
	m_MyMissionObject = MY_MISSOIN_OBJECT_NONE;	
	m_RespawnMode	= GAME_RESPAWN_MODE_NOLIMIT;

	m_pUnInstallSound	= nullptr;

	m_pDestructionData = nullptr;

	m_bObjectSend		= false;

	m_rRespawnTime			= 0.f;
	m_rRespawnTime4vs4		= 0.f;
	m_rInvincibleTime		= 0.f;
	m_rInvincibleTime4vs4	= 0.f;

	m_nCurrentUninstallUserIndex	= -1;

	m_pDefenceData		= nullptr;

	m_bResetObj			= false;
	i3mem::FillZero( &m_rMissionObjPos, sizeof(VEC3D));
	
	Reset();
	m_RunAwayObjPos.clear();
}

CGameMissionManager::~CGameMissionManager()
{
	EndMission();

	I3MEM_SAFE_FREE( m_pDestructionData);
	I3MEM_SAFE_FREE( m_pDefenceData);
}


void CGameMissionManager::InitializeObject( STAGE_MODE type)
{
	switch( type)
	{
	case STAGE_MODE_DESTROY :
		if( m_pDestructionData == nullptr)
			m_pDestructionData = (MISSION_DATA_DESTRUCTION*) i3MemAlloc( sizeof( MISSION_DATA_DESTRUCTION));

		I3MEM_SAFE_FREE( m_pDefenceData);
		break;

	case STAGE_MODE_DEFENCE :
		if( m_pDefenceData == nullptr)
			m_pDefenceData = (MISSION_DATA_DEFENCE*) i3MemAlloc( sizeof( MISSION_DATA_DEFENCE));

		I3MEM_SAFE_FREE( m_pDestructionData);
		break;
	}
}

void CGameMissionManager::StartMission( CSI_STAGE* stage)
{
	switch( stage->GetStageMode() )
	{
	case STAGE_MODE_DESTROY :
		{
			CGameObjectManager::i()->EnableObject();

			I3ASSERT( m_pDestructionData != nullptr);

			m_pDestructionData->m_ObjectRecvDamageInfo.Reset();
			m_pDestructionData->m_ObjectSendDamageInfo.Reset();
		}
		break;
	case STAGE_MODE_DEFENCE :
		{
			CGameObjectManager::i()->EnableObject();

			I3ASSERT( m_pDefenceData != nullptr);

			m_pDefenceData->getRecvDamageInfo()->Reset();
			m_pDefenceData->getSendDamageInfo()->Reset();
		}
		break;
	default :
		break;
	}
}

void CGameMissionManager::EndMission()
{
	Reset();

	m_RunAwayObjPos.clear();
	
	if (CGameObjectManager::i())
		CGameObjectManager::i()->DestroyObject();
	
	if( m_pDestructionData != nullptr)
	{
		m_pDestructionData->m_pObjectBlue = nullptr;
		m_pDestructionData->m_pObjectRed = nullptr;

		for( INT32 i = 0; i < USER_SLOT_COUNT; i++)
		{
			m_pDestructionData->m_ObjectIntervalIdx[ i] = 0;
			m_pDestructionData->m_ObjectSendDamageInfo._nUserDamage[ i] = 0;
		}
	}

	if( m_pDefenceData != nullptr)
	{
		for( INT32 i = 0; i < MAX_OBJECT_COUNT; i++)
		{
			m_pDefenceData->setDefenceObj( i, nullptr);

			for( INT32 j = 0; j < USER_SLOT_COUNT; j++)
			{
				m_pDefenceData->setObjIntervalIndex( i, j, 0);
				m_pDefenceData->getSendDamageInfo()->setUserDamage( i, j, 0);
			}
		}

		for( INT32 i = 0; i < USER_SLOT_COUNT; i++)
		{
			m_pDefenceData->setAttackingObjectUser( i, false);
			m_pDefenceData->setAttackerTime( i, 0.f);
		}
	}
}

void CGameMissionManager::WhoInstallReset()
{
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_bWhoInstall[i]=false;
		m_fBombInstallingTimer[i]=0.f;
	}
}

void CGameMissionManager::WhoUninstallReset()
{
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_bWhoUninstall[i]=false;
		m_fBombUninstallingTimer[i]=0.f;
	}
}

// Start_Battle, Round_Start���� �ݵ�� ȣ��Ǿ�� �մϴ�.
void CGameMissionManager::Reset()
{
	INT32 i, j;

	m_bEnableMyCrossHit = m_bEnableMyArea = false;
	m_bSendMsgBombInstall = m_bSendMsgBombUninstall = false;
	m_fExplosionTimer = m_fBombElapsedKeyTime = 0.f;
	m_nWhoInstalledIdx	= -1;

	for( i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_bWhoInstall[ i ]				= m_bWhoUninstall[ i ] = false;
		m_bIsInstalling[ i ]			= m_bIsUninstalling[ i ] = false;
		m_rWhoInstallCheckTime[ i ]		= 0.f;
		m_rWhoUninstallCheckTime[ i ]	= 0.f;
		m_fBombInstallingTimer[ i ]		= 0.f;
		m_fBombUninstallingTimer[ i ]	= 0.f;
		for( j = 0; j < MAX_BOMB_AREA_COUNT; j++)
		{
			m_bInstallingArea[i][j] = false;
		}

		m_nRespawnCurrentCount[ i]		= 0;
	}
	m_nRespawnTotalCount = 0;

	for( i = 0; i < MAX_BOMB_AREA_COUNT; i++ )
	{	
		m_bInstalledArea[ i ] = false;
	}

	m_rSendTimerForObject = 0.f;
	m_bOpenedLocationOfBomb = false;

	if( m_pDestructionData != nullptr)
	{
		m_pDestructionData->reset();
	}

	if( m_pDefenceData != nullptr)
	{
		m_pDefenceData->reset();
	}

	m_bObjectSend = false;

	m_bMyMissionState = false;

	m_UsurpationData.Reset();

	ClearUninstallingSound();
}

// �������� ����ĳ���õǸ� ���� ���������� 
// ���и�忡���� ����� ������ üũ���ش�.
void CGameMissionManager::setEnableCrossHit( bool bEnable )
{
	m_bEnableMyCrossHit		= bEnable;
}

void CGameMissionManager::setBlueObject( i3Object * pObj)
{
	I3ASSERT( m_pDestructionData != nullptr);
	m_pDestructionData->m_pObjectBlue = pObj;

	if( pObj != nullptr)
	{
		m_pDestructionData->m_intervalRateBlue = (UINT16)(pObj->getMaxHP() * 0.01f);
		I3ASSERT( m_pDestructionData->m_intervalRateBlue > 0);

		CGameObjectManager::i()->SetGameDestructionObject( TEAM_BLUE, pObj);
	}
}

void CGameMissionManager::setRedObject( i3Object * pObj)
{
	I3ASSERT( m_pDestructionData != nullptr);
	m_pDestructionData->m_pObjectRed = pObj;

	if( pObj != nullptr)	
	{
		m_pDestructionData->m_intervalRateRed = (UINT16)(pObj->getMaxHP() * 0.01f);
		I3ASSERT( m_pDestructionData->m_intervalRateRed > 0);

		CGameObjectManager::i()->SetGameDestructionObject( TEAM_RED, pObj);
	}
}

void CGameMissionManager::setRunAwayObject( i3Object * pObj)
{
	if( pObj != nullptr)	
	{
		CGameObjectEvent* pEvent = (CGameObjectEvent*)pObj;
		m_RunAwayObjPos.push_back(*pEvent->GetPos());
	}
}

void CGameMissionManager::Install( INT32 idx, BOMB_AREA area, bool active )
{
	I3_BOUNDCHK( idx, SLOT_MAX_COUNT );
	if( area < MAX_BOMB_AREA_COUNT)
	{
		m_bWhoInstall[ idx ] = active;
		m_rWhoInstallCheckTime[ idx ] = 0.f;

		if( active == true )
		{
			m_bInstallingArea[idx][ area] = active;
		}
		else
		{
			for( INT32 i = 0; i < MAX_BOMB_AREA_COUNT; i++ )
				m_bInstallingArea[idx][i] = false;
		}
	}
}

void CGameMissionManager::Uninstall( INT32 idx, bool active )
{
	I3ASSERT( idx < SLOT_MAX_COUNT );
	m_bWhoUninstall[ idx ] = active;
	m_rWhoUninstallCheckTime[ idx ] = 0.f;
}

void CGameMissionManager::_ProcessInBattleForBlastMode( REAL32 rDeltatime)
{
	INT32 i;
	// �Ÿ� ����
	VEC3D vDir;

#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
		return;
#endif

	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	if( myPlayer == nullptr)	return;

	INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();
	bool bHost = BattleServerContext::i()->IsP2PHost();

	for( i = 0; i < MAX_BOMB_AREA_COUNT; i++)
	{
		const BombInstallAreaProp* pProp = C4Context::i()->FindAreaProp(static_cast<BOMB_AREA>(i));
		if (pProp == nullptr) continue;

		//i3Vector::Sub( &vDir, C4Context::i()->getBombAreaPos( BOMB_AREA(i)), myPlayer->GetPos());
		i3Vector::Sub( &vDir, &pProp->pos, myPlayer->GetPos());
		REAL32 rLenSq = i3Vector::LengthSq( &vDir );

		if( (rLenSq < BOMBINSTALLAREA * BOMBINSTALLAREA) && (i3Math::abs( getY( &vDir)) < 0.05f) )
		{	// ���̰� 5��Ƽ �����̸� ��ġ ����
			m_bEnableMyArea = true;
			m_bInstallingArea[mySlotIdx][i] = true;
			break;
		}

		m_bEnableMyArea = false;
		m_bInstallingArea[mySlotIdx][i] = false;
	}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( !g_pConfig->m_bNetworking )
	{
		m_bEnableMyArea = true;
	}
#endif


	if( !m_bSendMsgBombInstall )
	{	// ��ź�� ��ġ���� �ʾҴٸ�
		for( i = 0; i < SLOT_MAX_COUNT; i++ )
		{	
			bool bCheckCancel = false;

			if( m_bIsInstalling[ i ] )
			{	// ��ź ��ġ���Դϴ�.
				// Network sync
				if( !WhoInstall( i ) )
				{// �ش� ������ ���� �ƴմϴ�.
					bCheckCancel = true;
				}

				if( bCheckCancel )
				{	// ��ġ�� ��ҵǾ����ϴ�.
					if( m_bInstallingArea[mySlotIdx][ BOMB_AREA_A] )		Install( i, BOMB_AREA_A, false );
					else													Install( i, BOMB_AREA_B, false );

					m_bIsInstalling[ i ] = false;
					m_fBombInstallingTimer[ i ] = BattleSlotContext::i()->getMissionBomb_SetTime_UsingSpeedKit( i);
				}
				else
				{	// ��ġ Ÿ�̸Ӹ� ����մϴ�
					m_fBombInstallingTimer[ i ] -= rDeltatime;
					if( m_fBombInstallingTimer[ i ] <= 0.f )
					{	// Armed
						if( bHost && (i == mySlotIdx) )
						{	// ���� �ڽŸ� ó���ϵ��� �սô�
							SendInstallComplete( i );
						}
						m_fBombInstallingTimer[ i ] = 0.f;
					}
				}
			}
			else
			{	// ��ġ������ üũ
				if( WhoInstall( i ) )
				{	// ��ġ�ڵ带 �޾ҽ��ϴ�. ��ġ������ �����ϰ� Ÿ�̸Ӹ� �����մϴ�.
					m_bIsInstalling[ i ] = true;
					m_fBombInstallingTimer[ i ] = BattleSlotContext::i()->getMissionBomb_SetTime_UsingSpeedKit( i);
					m_fBombInstallingTimer[ i ]+= BattleSlotContext::i()->getMissionBomb_SetTime_UsingSkill( i);
				}
			}
		}
	}


	ClearUninstallingSound();
}

void CGameMissionManager::_ProcessInstalledForBlastMode( REAL32 rDeltatime)
{
	INT32 i;

#if defined( DEF_OBSERVER_MODE)
	// Observer�� ó������ �ʴ´�.
	if( BattleSlotContext::i()->isObserverMe())
		return;
#endif

	INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();
	bool bHost = BattleServerContext::i()->IsP2PHost();
	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	if( m_MyMissionObject == MY_MISSION_OBJECT_DEFENSIVE || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		WeaponBase * pWeapon = nullptr;

#if defined( USE_EVENT_SHUTTLEX)
		pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SHUTTLEX));
#else
		pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4) );
#endif	

		if( pWeapon != nullptr )
		{
			I3ASSERT(pWeapon->getSceneObject());
			I3ASSERT(pWeapon->getSceneObject()->GetCacheMatrix());

			VEC3D vDir;

			// �����̴� ��ź�� ���� ��ü ��ġ�� GetCacheMatrix�� ���� �� - �ڸ��� 2008. 7. 25
			// i3Vector::Sub( &vDir, pWeapon->GetPos(), g_pPlayer->GetPos() );

			i3Vector::Sub( &vDir, i3Matrix::GetPos(pWeapon->getSceneObject()->GetCacheMatrix()), myPlayer->GetPos() );

			REAL32 rLenSq = i3Vector::LengthSq( &vDir );


			if( rLenSq < BOMBUNINSTALLAREA * BOMBUNINSTALLAREA)
			{
				m_bEnableMyArea = true;
			}
			else
			{
				m_bEnableMyArea = false;
			}
		}

		//������ �÷��̾��� ��ź�� ��ġ�Ǿ��ִ� �κ��� �߰��Ѱ�� ���������� ��ź�� ��ġ�� �̴ϸʿ� ǥ���ϱ� ���� ó��
		if( !isOpenedLocationOfBomb())
		{
			for( i = 0; i < g_pCharaManager->getCharaCount(); ++i)
			{
				CGameCharaBase * pChara = g_pCharaManager->getChara( i);

				if( pChara == nullptr)
					continue;

				if( pChara->getCharaInfo()->GetTeam() == myPlayer->getCharaInfo()->GetTeam())
				{
					if( pWeapon != nullptr)
					{
						VEC3D vDir;

						//i3Vector::Sub( &vDir, pWeapon->GetPos(), pChara->GetPos());
						I3ASSERT(pWeapon->getSceneObject());
						I3ASSERT(pWeapon->getSceneObject()->GetCacheMatrix());
						i3Vector::Sub( &vDir, i3Matrix::GetPos(pWeapon->getSceneObject()->GetCacheMatrix()), pChara->GetPos() );

						if( i3Vector::LengthSq( &vDir ) < BOMBUNINSTALLAREA * BOMBUNINSTALLAREA)
						{
							setOpenLocationOfBomb();
						}
					}
				}
			}
		}
	}

	// TCP üũ ��ź ���� �޽����� �����̶�� üũ
	if( !m_bSendMsgBombUninstall )
	{
		for( i = 0; i < SLOT_MAX_COUNT; i++ )
		{
			bool bCheck = false;

			if( m_bIsUninstalling[ i ] )
			{	// ��ź ��ü���̶�� �ٽ� üũ(UDP�� ��ü �ڵ尡 ������ ��ü���� ���� Ǯ���ݴϴ�.)
				// Network sync
				if( !WhoUninstall( i ) )
				{// �ش� ������ ��ü���� �ƴմϴ�.
					bCheck = true;
				}
				else
				{
					if( bHost && (i != mySlotIdx) )
					{
						// ���� Ȯ��
						if( rDeltatime < 0.25f)
							m_rWhoUninstallCheckTime[ i ] += rDeltatime;
						else
							m_rWhoUninstallCheckTime[ i ] += 0.25f;

						if( m_rWhoUninstallCheckTime[ i ] > 1.f )
						{	// �ش� �ð����� ��Ŷ(��ü�ڵ�)�� �� �޾Ҵٸ� ��ü�� ����մϴ�.
							bCheck = true;
						}
					}
				}

				if( bCheck )
				{	// ��ü�� ��� �Ǿ����ϴ�.
					Uninstall( i, false );

					m_bIsUninstalling[ i ] = false;
					m_fBombUninstallingTimer[ i ]	=  BattleSlotContext::i()->getMissionBomb_DisarmTime_UsingSpeedKit( i);

					// BUG_PORTABLE #2450 - ����ȣ
					// ��ź ����Ű�� ������ ������ ��Ŷ�����ؾ��Ѵ�. 
					// ���� ������ ������ ����Ű ��Ÿ�� ��Ŷ�� ���۵��� �ʾ� ��ź�������·� ����¡�ȴ�
					if( i == mySlotIdx )
					{
						m_fBombElapsedKeyTime	= UDPSENDTIME_MISSION;
					}
				}
				else
				{
					// ��ź ��ü���Դϴ�.
					m_fBombUninstallingTimer[ i ] -= rDeltatime;
					if( m_fBombUninstallingTimer[ i ] <= 0.f )
					{	// ��ü ����
						if( bHost && (i == mySlotIdx) )
						{
							SendUninstallComplete( i );
						}
						m_fBombUninstallingTimer[ i ] = 0.f;
					}
				}
			}
			else
			{	// ��ü�ڵ带 �޾Ҵ��� üũ
				if( WhoUninstall( i ) )
				{// ��ü�ڵ带 �޾Ҵٸ� ��ü������ ���� Ÿ�̸Ӹ� ����ϵ��� �մϴ�.
					m_bIsUninstalling[ i ]	= true;
					m_fBombUninstallingTimer[ i ]	= BattleSlotContext::i()->getMissionBomb_DisarmTime_UsingSpeedKit( i);
				}
			}
		}
	}

	m_fExplosionTimer -= rDeltatime;
	if( m_fExplosionTimer < 0.f )
	{// Boom
		m_fExplosionTimer = 0.f;
		// TCP�� ó���մϴ�. Ÿ�̸Ӹ� 0�� �մϴ�.
	}

	ProcessUninstallingSound( rDeltatime);
}

void CGameMissionManager::ClearUninstallingSound( void)
{
	if( m_pUnInstallSound != nullptr)
	{
		m_pUnInstallSound->Stop();
		m_pUnInstallSound = nullptr;
	}
}

void CGameMissionManager::ProcessUninstallingSound( REAL32 rDeltaSeconds)
{
	INT32 i;

	if( !m_bSendMsgBombUninstall)
	{
		bool bUninstalling = false;
		INT32	nFirstUninstallUserIdx = -1;
		REAL32	rMaxUninstallTime = 0.0f;
		for( i = 0; i < SLOT_MAX_COUNT; i++ )
		{
			if( m_bIsUninstalling[ i ] )
			{
				bUninstalling = true;

				rMaxUninstallTime = MAX( rMaxUninstallTime, m_fBombUninstallingTimer[i]);
				nFirstUninstallUserIdx = MAX( nFirstUninstallUserIdx, i);
			}
		}

		//
		if( bUninstalling )
		{
			if( nFirstUninstallUserIdx != -1 && rMaxUninstallTime >= 0.0f)
			{
				CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( nFirstUninstallUserIdx );

				if( pChara != nullptr)
				{
					if( m_pUnInstallSound == nullptr )
					{
						m_pUnInstallSound = g_pSndMng->StartWeapon_C4StateSnd( GTFW_C4_DISARM, pChara->GetPos(), (nFirstUninstallUserIdx == BattleSlotContext::i()->getMySlotIdx()), false);
						m_nCurrentUninstallUserIndex = nFirstUninstallUserIdx;
					}
					else
					{
						//���� ��ü�� �õ��� �÷��̾ ��ü�� ����ϰ� �ھ����ü�õ��� ������ �Űܰ� ���
						if( m_nCurrentUninstallUserIndex != nFirstUninstallUserIdx)
						{
							INT32 myindex = BattleSlotContext::i()->getMySlotIdx();

							if( nFirstUninstallUserIdx == myindex || m_nCurrentUninstallUserIndex == myindex)
							{
								m_pUnInstallSound->Stop();
								m_pUnInstallSound = g_pSndMng->StartWeapon_C4StateSnd( GTFW_C4_DISARM, pChara->GetPos(), (nFirstUninstallUserIdx == myindex), false);
							}
							else
							{
								MATRIX mtx;
								i3Matrix::SetTranslate( &mtx, pChara->GetPos());
								m_pUnInstallSound->UpdatePos( &mtx);
							}

							m_nCurrentUninstallUserIndex = nFirstUninstallUserIdx;
						}
					}

					return ;
				}
			}
		}
	}

	ClearUninstallingSound();
}

//��Ŷ �����°��� ����մϴ�.  �̺κе� ���� ��Ʈ��ũ�� �̵��մϴ�. 
void CGameMissionManager::_ProcessInBattleForDestructionMode( REAL32 rDeltatime)
{
	// Host�� ����մϴ�.
	if( !BattleServerContext::i()->IsP2PHost() )return;

	// Battle ���¿����� ó��
	if( !g_pFramework->IsBattleStage() )return;

	//	CStageBattle * pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	if( gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE)	return;

	m_rSendTimerForObject += rDeltatime;
	if( (m_bObjectSend && m_rSendTimerForObject > 1.f) ||
		(m_rSendTimerForObject > SENDTIME_DESTRUCTIONMODE) )
	{
		if( m_pDestructionData != nullptr)
			GameEventSender::i()->SetEventOnce( EVENT_DAMAGEOBJ_BATTLE_N, &m_pDestructionData->m_ObjectSendDamageInfo);
		else if( m_pDefenceData != nullptr)
			GameEventSender::i()->SetEventOnce( EVENT_DAMAGE_DEFENCE_OBJECT_BATTLE, m_pDefenceData->getSendDamageInfo());

		m_rSendTimerForObject = 0.f;
		m_bObjectSend = false;		
	}

	// Object ������ ���� üũ
	if( m_pDefenceData != nullptr)
	{
		INT32 i;
		REAL32 remainTime;

		for( i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if( m_pDefenceData->isAttackingObject( i))
			{
				remainTime = m_pDefenceData->getAttackerTime( i);
				remainTime -= rDeltatime;
				m_pDefenceData->setAttackerTime( i, remainTime);

				if( remainTime < 0.f)
					m_pDefenceData->setAttackingObjectUser( i, false);
			}
		}
	}
}

//��Ŷ �����°��� ����մϴ�.  �̺κе� ���� ��Ʈ��ũ�� �̵��մϴ�. 
void CGameMissionManager::_ProcessInBattleForConvoyEventMode( REAL32 rDeltatime)
{
	// Battle ���¿����� ó��
	if( !g_pFramework->IsBattleStage() )return;

	if(	m_UsurpationData.m_iUsurpationIdx != 0xff)
	{
		CGameCharaBase* Player = g_pCharaManager->getCharaByNetIdx( m_UsurpationData.m_iUsurpationIdx);
		if( Player != nullptr)
			CopyMissionObjPos( Player->GetPos());
	}
}


void CGameMissionManager::_ProcessInBattleForConvoyMode( REAL32 rDeltatime)
{
	// Battle ���¿����� ó��
	if( !g_pFramework->IsBattleStage() )return;

	//	CStageBattle * pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	if( BattleSlotContext::i()->getMySlotIdx() == m_UsurpationData.m_iUsurpationIdx)
		m_UsurpationData.m_bMyUsurpationState = true;
	else
		m_UsurpationData.m_bMyUsurpationState = false;


	if(	m_UsurpationData.m_iUsurpationIdx < SLOT_MAX_COUNT)
	{
		CGameCharaBase* Player = g_pCharaManager->getCharaByNetIdx( m_UsurpationData.m_iUsurpationIdx);
		if(Player != nullptr)
			CopyMissionObjPos( Player->GetPos());
	}
}

void CGameMissionManager::_ProcessInBattleForRunAwayMode( REAL32 rDeltatime)
{
	VEC3D vDir;
	INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();
	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(mySlotIdx);
	bool bHost = BattleServerContext::i()->IsP2PHost();

	if( myPlayer == nullptr)
		return;

	for(size_t i = 0; i < m_RunAwayObjPos.size(); i++)
	{
		i3Vector::Sub( &vDir, &m_RunAwayObjPos.at(i), myPlayer->GetPos());
		REAL32 rLenSq = i3Vector::LengthSq( &vDir );

		if(rLenSq < 15)
		{	// ���̰� 5��Ƽ �����̸� ��ġ ����
			m_bEnableMyArea = true;
			break;
		}
		else
			m_bEnableMyArea = false;
	}

	if( !m_bSendMsgBombInstall )
	{	// ��ź�� ��ġ���� �ʾҴٸ�
		for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
		{	
			bool bCheckCancel = false;

			if( m_bIsInstalling[ i ] )
			{	// ��ź ��ġ���Դϴ�.
				// Network sync
				if( !WhoInstall( i ) )
				{// �ش� ������ ���� �ƴմϴ�.
					bCheckCancel = true;
				}

				if( bCheckCancel )
				{	// ��ġ�� ��ҵǾ����ϴ�.
					if( m_bInstallingArea[mySlotIdx][ BOMB_AREA_A] )		Install( i, BOMB_AREA_A, false );
					else													Install( i, BOMB_AREA_B, false );

					m_bIsInstalling[ i ] = false;
					m_fBombInstallingTimer[ i ] = BattleSlotContext::i()->getMissionBomb_SetTime_UsingSpeedKit( i);
				}
				else
				{	// ��ġ Ÿ�̸Ӹ� ����մϴ�
					m_fBombInstallingTimer[ i ] -= rDeltatime;
					if( m_fBombInstallingTimer[ i ] <= 0.f )
					{	// Armed
						if( bHost && (i == mySlotIdx) )
						{	// ���� �ڽŸ� ó���ϵ��� �սô�
							SendInstallComplete( i );
						}
						m_fBombInstallingTimer[ i ] = 0.f;
					}
				}
			}
			else
			{	// ��ġ������ üũ
				if( WhoInstall( i ) )
				{	// ��ġ�ڵ带 �޾ҽ��ϴ�. ��ġ������ �����ϰ� Ÿ�̸Ӹ� �����մϴ�.
					m_bIsInstalling[ i ] = true;
					m_fBombInstallingTimer[ i ] = BattleSlotContext::i()->getMissionBomb_SetTime_UsingSpeedKit( i);
					m_fBombInstallingTimer[ i ]+= BattleSlotContext::i()->getMissionBomb_SetTime_UsingSkill( i);
				}
			}
		}
	}

}

void CGameMissionManager::OnUpdate( REAL32 rDeltatime )
{
	ISTEST_S
	{
		m_fBombElapsedKeyTime += rDeltatime;

		switch( MyRoomInfoContext::i()->getStage()->GetStageMode() )
		{
		case STAGE_MODE_BOMB :
		case STAGE_MODE_TUTORIAL :
			{
				m_fBombElapsedKeyTime += rDeltatime;

				switch( m_MissionState )
				{
				case GAME_MISSION_STATE_NONE		:	break;	// ���Ʈ ��ŸƮ�� ���� ���� ���� 
				case GAME_MISSION_STATE_INBATTLE	:			// ��Ʋ�� ��ŸƮ �� ���� 
					_ProcessInBattleForBlastMode( rDeltatime);
					break;
				case GAME_MISSION_STATE_INSTALLED :				// ��Ʋ�� ��ź�� ��ġ�Ǿ���.
					_ProcessInstalledForBlastMode( rDeltatime);
					break;
				default :
					break;
				}
			}
			break;
		case STAGE_MODE_DESTROY :
		case STAGE_MODE_DEFENCE :
		case STAGE_MODE_ESCAPE :
			{
				switch( m_MissionState )
				{
				case GAME_MISSION_STATE_NONE		:	break;//���Ʈ ��ŸƮ�� ���� ���� ���� 
				case GAME_MISSION_STATE_INBATTLE	:
					_ProcessInBattleForDestructionMode( rDeltatime);
					break;
				default :
					break;
				}
			}
			break;
		case STAGE_MODE_CONVOY:
			if( MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
				_ProcessInBattleForConvoyEventMode(rDeltatime);
			else
				_ProcessInBattleForConvoyMode(rDeltatime);
			break;
		case STAGE_MODE_RUN_AWAY:
			_ProcessInBattleForRunAwayMode(rDeltatime);
			break;
		}
	}
	else
	ISTEST_E
	{
		switch( MyRoomInfoContext::i()->getStage()->GetStageMode() )
		{
		case STAGE_MODE_BOMB :
		case STAGE_MODE_TUTORIAL :
			{
				m_fBombElapsedKeyTime += rDeltatime;

				switch( m_MissionState )
				{
				case GAME_MISSION_STATE_NONE		:	break;	// ���Ʈ ��ŸƮ�� ���� ���� ���� 
				case GAME_MISSION_STATE_INBATTLE	:			// ��Ʋ�� ��ŸƮ �� ���� 
					_ProcessInBattleForBlastMode( rDeltatime);
					break;
				case GAME_MISSION_STATE_INSTALLED :				// ��Ʋ�� ��ź�� ��ġ�Ǿ���.
					_ProcessInstalledForBlastMode( rDeltatime);
					break;
				default :
					break;
				}
			}
			break;
		case STAGE_MODE_DESTROY :
		case STAGE_MODE_DEFENCE :
		case STAGE_MODE_ESCAPE :
			{
				switch( m_MissionState )
				{
				case GAME_MISSION_STATE_NONE		:	break;//���Ʈ ��ŸƮ�� ���� ���� ���� 
				case GAME_MISSION_STATE_INBATTLE	:
					_ProcessInBattleForDestructionMode( rDeltatime);
					break;
				default :
					break;
				}
			}
			break;
		case STAGE_MODE_CONVOY:
			if( MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
				_ProcessInBattleForConvoyEventMode(rDeltatime);
			else
				_ProcessInBattleForConvoyMode(rDeltatime);
			break;
		case STAGE_MODE_RUN_AWAY:
			_ProcessInBattleForRunAwayMode(rDeltatime);
			break;
		}
	}
}
void CGameMissionManager::SendMissionInstall( bool active )
{
#if defined( DEF_OBSERVER_MODE)
	I3ASSERT( BattleSlotContext::i()->isObserverMe() == false);
#endif

	if( active )
		m_bMyMissionState = true;
	else
		m_bMyMissionState = false;

	INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();
	NET_MISSION_STATE nState = NET_MISSION_NONE;
	BOMB_AREA nArea = BOMB_AREA_A;


	if( m_bIsInstalling[mySlotIdx] && !active )
	{	// ��ġ ��Ҹ� �ϰ� �Ǹ� �ݵ��� ������� �մϴ�. �����ټ� �ֵ��� Ÿ�̸Ӹ� �����մϴ�.
		m_fBombElapsedKeyTime = UDPSENDTIME_MISSION;
	}

	if( m_fBombElapsedKeyTime < UDPSENDTIME_MISSION )
	{// ������ Ÿ���� �� �Ǿ����� ó������ �ʽ��ϴ�.
		return;
	}

	m_fBombElapsedKeyTime		= 0.f;
	m_bWhoInstall[ mySlotIdx ]	= active;

	if( active )
	{
		{
			nState	= NET_MISSION_TRIGGER_ARM_START;
			if (C4Context::i()->getInstallStart())
			{
				C4Context::i()->setInstallStart(false);
				C4Context::i()->setInstallStartTime( BattleSyncTimer::i()->GetTimeElapsed());
			}
		}
	}
	else
	{
		nState	= NET_MISSION_TRIGGER_ARM_CANCEL;
		C4Context::i()->setInstallStart(true);
		C4Context::i()->setInstallStartTime(0.f);

	}

	if( m_bInstallingArea[mySlotIdx][BOMB_AREA_B] )	nArea	= BOMB_AREA_B;

	UINT32 WeaponId =0;

	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	INT32 Weapon  =  myPlayer->getWeaponContext()->getCurrentSlotNum();


	if (Weapon == ITEM_TYPE_MISSION)
	{
		WeaponId =  MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));

	}

	GAMEEVENT::Write_Chara_Mission( myPlayer, nState, nArea,WeaponId, -1, C4Context::i()->getInstallStartTime());
}

void CGameMissionManager::SendMissionUninstall( bool active )
{
	INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();
	NET_MISSION_STATE nState = NET_MISSION_NONE;
	BOMB_AREA nArea = BOMB_AREA_A;
	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	
	if( m_MissionState != GAME_MISSION_STATE_INSTALLED )
	{// ��ź�� ��ġ�Ǿ� ���� �ʴٸ� ó������ �ʽ��ϴ�.
		return;
	}

	if( IsUninstalling( mySlotIdx ) && !active )
	{// ����ϴ� ���� �ݵ��� ������� �մϴ�. sendTime�� ���� �� �ֵ��� ����
		m_fBombElapsedKeyTime = UDPSENDTIME_MISSION;
	}

	if( m_fBombElapsedKeyTime < UDPSENDTIME_MISSION )
	{// ������ Ÿ���� �� �Ǿ����� ó������ �ʽ��ϴ�.
		return;
	}

	m_fBombElapsedKeyTime = 0.f;		// ������ ���̸� Ÿ�̸Ӹ� �ʱ�ȭ
	m_bWhoUninstall[ mySlotIdx ] = active;

	//////
	if( active )
	{	// ����

		nState	= NET_MISSION_TRIGGER_DISARM_START;
	
		if (C4Context::i()->getInstallStart())
		{
			C4Context::i()->setInstallStart(false);
			C4Context::i()->setInstallStartTime( BattleSyncTimer::i()->GetTimeElapsed());
		}
	}
	else
	{
		nState	= NET_MISSION_TRIGGER_DISARM_CANCEL;
		C4Context::i()->setInstallStart(true);
		C4Context::i()->setInstallStartTime(0.f);

	}

	if( m_bInstalledArea[ BOMB_AREA_B] )	nArea = BOMB_AREA_B;

	UINT32	WeaponId = 0;
	INT32 Weapon  =  myPlayer->getWeaponContext()->getCurrentSlotNum();



	if (Weapon == ITEM_TYPE_MISSION)
	{
		WeaponId =  MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
	}

	GAMEEVENT::Write_Chara_Mission( myPlayer, nState, nArea,WeaponId, -1, C4Context::i()->getInstallStartTime());
}
void CGameMissionManager::SendInstallComplete( INT32 idx )
{
	VEC3D vPos;

	if( !m_bSendMsgBombInstall )
	{
		//i3Vector::Copy( &vPos, C4Context::i()->getBombAreaPos(BOMB_AREA_A));
		const BombInstallAreaProp* pProp = C4Context::i()->FindAreaProp(BOMB_AREA_A);
		if (pProp == nullptr) return;

		m_bSendMsgBombInstall = true;
		m_fBombInstallingTimer[ idx ] = 0.f;
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idx);
		if( pChara != nullptr)
		{
			//BOMB_AREA nArea = BOMB_AREA_A;
			//if( m_bInstallingArea[ idx][BOMB_AREA_B] )	nArea = BOMB_AREA_B;

			UINT32 WeaponId;
			WeaponId =  MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));

			i3Vector::Copy( &vPos, pChara->GetPos());
		}

		GameEventSender::i()->SetEventOnce( EVENT_ARMEDBOMB_BATTLE, &idx, &vPos );

		m_nWhoInstalledIdx = idx;

		ISTEST_S
		{
			//	��ź�� ���忡 ������.
			CGameCharaBase * pChara2 = g_pCharaManager->getCharaByNetIdx( idx);
			if (pChara2 == nullptr) return;

			i3Vector::Copy( &vPos, pChara2->GetPos() );
			i3Vector::Add( &vPos, 0.f, 0.1f, 0.f );
			pChara2->Cmd_DropWeapon( WEAPON_SLOT_MISSION, &vPos);
		}
		ISTEST_E
	}
}

void CGameMissionManager::SendUninstallComplete( INT32 idx )
{
	if( !m_bSendMsgBombUninstall )
	{
		m_bSendMsgBombUninstall = true;
		GameEventSender::i()->SetEventOnce( EVENT_DISARMBOMB_BATTLE, &idx );
		m_fBombInstallingTimer[ idx ] = 0.f;
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idx);
		if( pChara != nullptr)
		{
			BOMB_AREA nArea = BOMB_AREA_A;
			if( m_bInstalledArea[ BOMB_AREA_B] )	nArea = BOMB_AREA_B;	
			UINT32 WeaponId; 
			WeaponId =  MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
		}
	}
}

void CGameMissionManager::ProcessMissionPreStart( void)
{
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe() == false)
	{
#endif
		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
		// ��ġ ������ �ƴ����� �ϴ� ���⼭ �������ݴϴ�.. ���Ŀ� �̼Ǹ�� �Ǵ� �ʿ� ���� ���� �ٲ� �� �ֽ��ϴ�. �� ������ �̼Ǹ�忡 ���� ��������� �մϴ�.
		if( (myPlayer != nullptr) && (myPlayer->getCharaTeam() & CHARACTER_TEAM_RED))
			CGameMissionManager::i()->setMyMissionObject( MY_MISSION_OBJECT_OFFENSIVE );
		else
			CGameMissionManager::i()->setMyMissionObject( MY_MISSION_OBJECT_DEFENSIVE );

#if defined( DEF_OBSERVER_MODE)
	}
#endif

	setMissionState( GAME_MISSION_STATE_PREBATTLE );

	WhoInstallReset();
	WhoUninstallReset();
}

void CGameMissionManager::ProcessMissionStart( void )
{
	setMissionState( GAME_MISSION_STATE_INBATTLE );
}


void CGameMissionManager::ProcessInstalled( void )
{
	if( m_bIsInstalling[ BattleSlotContext::i()->getMySlotIdx() ] == true)
	{
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
		pChara->Cmd_Install( false);
	}


	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_bWhoInstall[ i ] = false;
		m_bIsInstalling[ i ] = false;
		m_fBombInstallingTimer[ i ] = 0.f;
		m_rWhoInstallCheckTime[ i ] = 0.f;
	}

	setMissionState( GAME_MISSION_STATE_INSTALLED );
}

void CGameMissionManager::ProcessUninstalled( void )
{
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_bWhoUninstall[ i ] = false;
		m_bIsUninstalling[ i ] = false;
		m_fBombUninstallingTimer[ i ] = 0.f;
		m_rWhoUninstallCheckTime[ i ] = 0.f;
	}

	setMissionState( GAME_MISSION_STATE_UNINSTALLED );
	m_bEnableMyArea = false;
}

// Server���� ������ �����͸� �����մϴ�. (TCP)
void	CGameMissionManager::setDestructionInfo( DESTRUCTION_INFO * pInfo)
{
	if( !g_pFramework->IsBattleStage() )
		return;

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE)) {

		pInfo->CopyTo( &m_pDestructionData->m_ObjectRecvDamageInfo );

		if( !BattleServerContext::i()->IsP2PHost() )
		{
			pInfo->CopyTo( &m_pDestructionData->m_ObjectSendDamageInfo);	// Host Lost�� ���ο� ���嵵 ���� �����͸� ������ ���� �ʿ䰡 �ֽ��ϴ�.
		}
	}
	else {
		//		CStageBattle * pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
		INT32 objIdx = 0;
		UINT16 intervalRate;
		UINT16 interval;

		for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			// 10 �ۼ�Ʈ���� �޽����� ����ݴϴ�.
			if( pInfo->_nUserDamage[ i] > m_pDestructionData->m_ObjectRecvDamageInfo._nUserDamage[ i])
			{
				interval = pInfo->_nUserDamage[ i];

				if( i & 1)
				{
					intervalRate = m_pDestructionData->m_intervalRateRed * 10 ;
					objIdx = 0;
				}
				else
				{
					intervalRate = m_pDestructionData->m_intervalRateBlue * 10 ;
					objIdx = 1;
				}

				if( interval > intervalRate * (UINT16) m_pDestructionData->m_ObjectIntervalIdx[ i])
				{	
					// �ı��̼� ���¡���� ���� �߰� ���� ���� (2011.08.04 ���õ)
					INT32 nAddPoint = 5;

					// �޼��� ���
					if(CHud::i() != nullptr)
					{
						CHud::i()->CreateInGameMessage(i, objIdx, nAddPoint);

						if( i == BattleSlotContext::i()->getMySlotIdx())
							CHud::i()->AddTargetBonus(nAddPoint);
					}

					m_pDestructionData->m_ObjectIntervalIdx[ i]++;
				}
			}
		}

		CSI_STAGE* stage = MyRoomInfoContext::i()->getStage();
		if( (stage->IsModeforModeType(STAGE_MODE_DEATHMATCH) && stage->GetStageUID() == STAGE_UID_GIRAN) ||
			(stage->IsModeforModeType(STAGE_MODE_DEATHMATCH) && stage->GetStageUID() == STAGE_UID_GIRAN2) )
		{
			if( m_pDestructionData->m_ObjectRecvDamageInfo._nRedObjectHP > pInfo->_nRedObjectHP )
			{
				interval = (UINT16) m_pDestructionData->m_pObjectRed->getMaxHP() - pInfo->_nRedObjectHP;
				intervalRate = m_pDestructionData->m_intervalRateRed * 5 * (UINT16) m_pDestructionData->m_RedObjectIntervalIdx;				//���ݵ����Դϴ�. 
				if( interval > intervalRate)
				{
					if(((CGameControledObject *) m_pDestructionData->m_pObjectRed)->GetControledObjectState() == 0)
					{
						((CGameControledObject *) m_pDestructionData->m_pObjectRed)->SetControledObjectState( 1);
					}

					m_pDestructionData->m_RedObjectIntervalIdx++;
				}
			}

			if( m_pDestructionData->m_ObjectRecvDamageInfo._nBlueObjectHP > pInfo->_nBlueObjectHP )
			{
				interval = (UINT16) m_pDestructionData->m_pObjectBlue->getMaxHP() - pInfo->_nBlueObjectHP;
				intervalRate = m_pDestructionData->m_intervalRateBlue * 10 * (UINT16) m_pDestructionData->m_BlueObjectIntervalIdx;			//���ݵ����Դϴ�. 
				if( interval > intervalRate)
				{
					if(((CGameControledObject *) m_pDestructionData->m_pObjectBlue)->GetControledObjectState() == 0)
					{
						UINT32 state = 1;
						if( ( m_pDestructionData->m_BlueObjectIntervalIdx & 1) == 0)
							state = 3;

						((CGameControledObject *) m_pDestructionData->m_pObjectBlue)->SetControledObjectState( state);
					}

					m_pDestructionData->m_BlueObjectIntervalIdx++;
				}
			}
		}

		pInfo->CopyTo( &m_pDestructionData->m_ObjectRecvDamageInfo );

		if( !BattleServerContext::i()->IsP2PHost() )
		{
			pInfo->CopyTo( &m_pDestructionData->m_ObjectSendDamageInfo);	// Host Lost�� ���ο� ���嵵 ���� �����͸� ������ ���� �ʿ䰡 �ֽ��ϴ�.
			REAL32 rDamage = m_pDestructionData->m_pObjectRed->getCurrentHP() - (REAL32) m_pDestructionData->m_ObjectRecvDamageInfo._nRedObjectHP;
			if( rDamage > 0.f)
				m_pDestructionData->m_pObjectRed->Damage( rDamage, nullptr, nullptr);
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
			else if( rDamage < 0.f)
			{
				I3PRINTLOG(I3LOG_WARN,  "invalid Calc HP for client. %.4f %d", m_pDestructionData->m_pObjectRed->getCurrentHP(), m_pDestructionData->m_ObjectRecvDamageInfo._nRedObjectHP);
			}
#endif
			m_pDestructionData->m_pObjectRed->setCurrentHP( (REAL32) m_pDestructionData->m_ObjectRecvDamageInfo._nRedObjectHP);

			rDamage = m_pDestructionData->m_pObjectBlue->getCurrentHP() - (REAL32) m_pDestructionData->m_ObjectRecvDamageInfo._nBlueObjectHP;
			if( rDamage > 0.f)
				m_pDestructionData->m_pObjectBlue->Damage( rDamage, nullptr, nullptr);
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
			else if( rDamage < 0.f)
			{
				I3PRINTLOG(I3LOG_WARN,  "invalid Calc HP for client. %.4f %d", m_pDestructionData->m_pObjectRed->getCurrentHP(), m_pDestructionData->m_ObjectRecvDamageInfo._nRedObjectHP);
			}
#endif
			m_pDestructionData->m_pObjectBlue->setCurrentHP( (REAL32) m_pDestructionData->m_ObjectRecvDamageInfo._nBlueObjectHP);
		}
	}
}

void CGameMissionManager::setDestructionInfoNoEffect( DESTRUCTION_INFO * pInfo)
{
	pInfo->CopyTo( &m_pDestructionData->m_ObjectRecvDamageInfo );
	m_pDestructionData->m_pObjectRed->setCurrentHP( (REAL32) m_pDestructionData->m_ObjectRecvDamageInfo._nRedObjectHP);
	m_pDestructionData->m_pObjectBlue->setCurrentHP( (REAL32) m_pDestructionData->m_ObjectRecvDamageInfo._nBlueObjectHP);

	CSI_STAGE* stage = MyRoomInfoContext::i()->getStage();
	if ((stage->IsModeforModeType(STAGE_MODE_DEATHMATCH) && stage->GetStageUID() == STAGE_UID_GIRAN) ||
		(stage->IsModeforModeType(STAGE_MODE_DEATHMATCH) && stage->GetStageUID() == STAGE_UID_GIRAN2))
	{
		//���� ����Ƚ�� �ε����� ���մϴ�.
		m_pDestructionData->m_RedObjectIntervalIdx = 
			1 + ( (INT32)m_pDestructionData->m_pObjectRed->getMaxHP() - m_pDestructionData->m_ObjectRecvDamageInfo._nRedObjectHP)
			/ ( (INT32)m_pDestructionData->m_pObjectRed->getMaxHP() / 5);

		m_pDestructionData->m_BlueObjectIntervalIdx = 
			1 + ( (INT32)m_pDestructionData->m_pObjectBlue->getMaxHP() - m_pDestructionData->m_ObjectRecvDamageInfo._nBlueObjectHP) 
			/ ( (INT32)m_pDestructionData->m_pObjectBlue->getMaxHP() / 10);

		m_pDestructionData->m_ObjectIntervalIdx[0] = m_pDestructionData->m_BlueObjectIntervalIdx;
		m_pDestructionData->m_ObjectIntervalIdx[1] = m_pDestructionData->m_RedObjectIntervalIdx;
	}


	if( !BattleServerContext::i()->IsP2PHost() )
	{
		pInfo->CopyTo( &m_pDestructionData->m_ObjectSendDamageInfo);	// Host Lost�� ���ο� ���嵵 ���� �����͸� ������ ���� �ʿ䰡 �ֽ��ϴ�.		
	}

	//���� ������ ��� ���� ��ŸƮ�� �ı� ������Ʈ�� hp�� reset ���� �ʱ� ���ؼ�.
	SetResetObjFlag( !IntrudeContext::i()->IsInterEnter() );
}

//���常 ���ɴϴ�. 
void CGameMissionManager::DamageObjectForUser( INT32 idx, REAL32 damage, i3Object * pObj, bool bSend)
{
	I3_BOUNDCHK( idx, SLOT_MAX_COUNT);
	//	CStageBattle * pStage = (CStageBattle*) g_pFramework->GetCurrentStage();	
	if( gstatemgr::i()->getStageState() > GAMESTAGE_STATE_BATTLE ) 			return;//��Ʋ���̳� ���������� ���;� �մϴ�. 		

	UINT32 iDamage = (UINT32)damage;
	INT32 iObjectIdx = -1;

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) == false) {
		if( m_pDestructionData != nullptr)
		{
			if(idx&0x01)
			{
				if( pObj == m_pDestructionData->m_pObjectRed)
				{
					if( m_pDestructionData->m_ObjectSendDamageInfo._nRedObjectHP < iDamage ) iDamage = m_pDestructionData->m_ObjectSendDamageInfo._nRedObjectHP; 
					// Helispot ����(2009.02.17)
					// ���� ��Ŀ��� ���ù������ ����
					m_pDestructionData->m_ObjectSendDamageInfo._nRedObjectHP = (UINT16) pObj->getCurrentHP();
				}
			}
			else
			{
				if( pObj == m_pDestructionData->m_pObjectBlue)
				{
					if( m_pDestructionData->m_ObjectSendDamageInfo._nBlueObjectHP < iDamage ) iDamage = m_pDestructionData->m_ObjectSendDamageInfo._nBlueObjectHP; 

					m_pDestructionData->m_ObjectSendDamageInfo._nBlueObjectHP = (UINT16) pObj->getCurrentHP();
				}
			}
		}
		else if( m_pDefenceData != nullptr)
		{
			if( (idx & 0x01) == 0)
			{	// red team�� ����
				if( pObj == m_pDefenceData->getDefenceObj(0) )
				{
					iObjectIdx = 0;
				}
				else if( pObj == m_pDefenceData->getDefenceObj(1) )
				{
					iObjectIdx = 1;
				}
				else
				{
					I3PRINTLOG(I3LOG_FATAL,  "invalid.. function call..");
				}

				I3_BOUNDCHK_RETURN( iObjectIdx, 2);

				UINT16 objHP = m_pDefenceData->getSendDamageInfo()->getObjectHP( iObjectIdx);
				if( objHP < iDamage )
					iDamage = objHP;

				m_pDefenceData->getSendDamageInfo()->setObjectHP( iObjectIdx, (UINT16) pObj->getCurrentHP());

				// ������ ǥ��
				m_pDefenceData->setAttackingObjectUser( idx, true);
			}
		}

		if( pObj->getCurrentHP() == 0.f)
		{	// HP�� 0�̸� ������ �����ϴ�.
			m_rSendTimerForObject = 1.f;
		}	
	}	
	else {
		CGameCharaDinoTRex * pTRex = g_pCharaManager->GetTRex();
		if(pTRex != nullptr) {
			if(pTRex->getCurHP_Real() == 0.f) {
				m_rSendTimerForObject = 1.f;
			}
		}
	}

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE)) {
		iDamage = (INT32)(((REAL32)iDamage) * BOSS_MIDDLE_RATIO + 0.5f);
	}

	if( m_pDestructionData != nullptr)
		m_pDestructionData->m_ObjectSendDamageInfo.AddUserDamage(idx, iDamage);
	else if( m_pDefenceData != nullptr)
	{
		I3_BOUNDCHK_RETURN( iObjectIdx, 2);
		m_pDefenceData->getSendDamageInfo()->AddUserDamage( iObjectIdx, idx, iDamage );
	}

	//Send Packet Setting  ���� �ٸ������� �̵��մϴ�. 
	if(iDamage == 0) return;

	m_bObjectSend = true;

	return;
}

void CGameMissionManager::GiveupBattle( INT32 idx)
{
	I3_BOUNDCHK( idx, SLOT_MAX_COUNT);

	if( m_pDestructionData != nullptr)
	{
		m_pDestructionData->m_ObjectIntervalIdx[ idx] = 0;
		m_pDestructionData->m_ObjectSendDamageInfo._nUserDamage[ idx] = 0;
	}
	else if( m_pDefenceData != nullptr)
	{
		INT32 i;
		for( i = 0; i < MAX_OBJECT_COUNT; i++)
		{
			m_pDefenceData->setObjIntervalIndex( i, idx, 0);
			m_pDefenceData->getSendDamageInfo()->setUserDamage( i, idx, 0);
		}

		m_pDefenceData->setAttackingObjectUser( idx, false);
		m_pDefenceData->setAttackerTime( idx, 0.f);
	}
}

void CGameMissionManager::AddUserDamage(INT32 iKiller)
{
	I3_BOUNDCHK(iKiller, SLOT_MAX_COUNT);

	CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(iKiller);
	if(pChara == nullptr) {
		return;
	}
	//KEY : EscapeMission �ʿ���� �ּ�ó��
	if(pChara->IsRex()) {
		//	m_pDestructionData->m_ObjectSendDamageInfo.AddUserDamage(iKiller, 150);
	}
	else if(pChara->IsRaptor()) {
		//	m_pDestructionData->m_ObjectSendDamageInfo.AddUserDamage(iKiller, 100);
	}
}


bool CGameMissionManager::ProcessCharaRespawn( INT32 idx)
{
	if( m_RespawnMode < GAME_RESPAWN_MODE_NOLIMIT )
	{
		if( m_nRespawnTotalCount <= m_nRespawnCurrentCount[ idx ] )
			return false;

		m_nRespawnCurrentCount[ idx] += 1;
	}

	return true;
}

REAL32 CGameMissionManager::GetInvincibleTime( CSI_STAGE* stage ) const
{
	if(CStageInfoUtil::GetSlotMode(*stage) == STAGE_SLOT_MODE_4VS4)		return m_rInvincibleTime4vs4;
	else																return m_rInvincibleTime;
}

REAL32 CGameMissionManager::GetRespawnTime( CSI_STAGE* stage, INT32 userIdx) const
{
	REAL32 rRespawnLimit = 0;

	if( CStageInfoUtil::GetSlotMode(*stage) == STAGE_SLOT_MODE_4VS4)		rRespawnLimit = m_rRespawnTime4vs4;
	else																	rRespawnLimit = m_rRespawnTime;

	// 1.0 version
	if( LocaleValue::Enable( "EnableImmediateRespawn"))
	{//��� ���� ��� ������ ����( �̺�Ʈ�� ) //EVENT_IMMEDIATE_RESPAWN//
		rRespawnLimit = 0.0f;
	}
	else
	{
		REAL32 fRatio = BattleSlotContext::i()->GetUseCashItemValue(userIdx, CASHITEMSKILL_RESPAWN);
		rRespawnLimit -= fRatio * rRespawnLimit ;
		
		CASHITEM_TRACE("CASHITEMSKILL_RESPAWN = %f\n", fRatio);
	}

	// 1.5 version
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( userIdx);
	if( pChara != nullptr && pChara->getEquipContext() != nullptr)
		rRespawnLimit += pChara->getEquipContext()->GetAbility( EQUIP::eABILITY_RESPAWNTIME );

	return  rRespawnLimit;
}

void CGameMissionManager::LoadPropertiesByStage()
{
	i3RegKey * pRoot = Config::FindKey("Stage");
	I3ASSERT( pRoot != nullptr);

	i3StageInfo* pInfo = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex());

	i3RegKey * pKey = Config::FindKey( pInfo->GetName(), pRoot);
	I3ASSERT(pKey != nullptr);

	if( pKey != nullptr)
	{
		// ���������� ������ Ÿ��
		REAL32 rTime, rTime4vs4;

		{
			rTime = 0.f;
			rTime4vs4 = 0.f;
			FIND_REG_DATA( pKey, LS_STR("RespawnTime"), &rTime);
			FIND_REG_DATA( pKey, LS_STR("RespawnTime4vs4"), &rTime4vs4);

			if( rTime == 0.f)		rTime = DEFAULT_RESPAWN_TIME;
			if( rTime4vs4 == 0.f)	rTime4vs4 = DEFAULT_RESPAWN_TIME;

			setRespawnTime( rTime);
			setRespawnTime4vs4( rTime4vs4);
		}		

		// ���������� ���� Ÿ��
		rTime = 0.f;
		rTime4vs4 = 0.f;
		FIND_REG_DATA( pKey, LS_STR("InvincibleTime"), &rTime);
		FIND_REG_DATA( pKey, LS_STR("InvincibleTime4vs4"), &rTime4vs4);

		if (MyRoomInfoContext::i()->getStage()->IsOneShotOneKill())
		{
			if (rTime == 0.f)		rTime = CHAOS_INVINCIBLE_TIME;
			if (rTime4vs4 == 0.f)	rTime4vs4 = CHAOS_INVINCIBLE_TIME;
		}
		else
		{
			if (rTime == 0.f)		rTime = DEFAULT_INVINCIBLE_TIME;
			if (rTime4vs4 == 0.f)	rTime4vs4 = DEFAULT_INVINCIBLE_TIME;
		}



		setInvincibleTime( rTime);
		setInvincibleTime4vs4( rTime4vs4);

	}
}

void CGameMissionManager::setDefenceObj( INT32 idx, i3Object * pObj)
{
	I3ASSERT( m_pDefenceData != nullptr);
	I3_BOUNDCHK( idx, MAX_OBJECT_COUNT);

	m_pDefenceData->setDefenceObj( idx, pObj);

	if( pObj != nullptr)
	{
		UINT16 rate = (UINT16) (pObj->getMaxHP() * 0.01f);
		I3ASSERT( rate > 0);

		m_pDefenceData->setIntervalRateObject( idx, rate);
	
		CGameObjectManager::i()->SetGameDefenceObject( idx, pObj);
	}
}

void CGameMissionManager::setDefenceInfo( DEFENCE_INFO * pInfo)
{
	INT32 i, j;

	I3ASSERT( m_pDefenceData != nullptr);

	if( !g_pFramework->IsBattleStage())
		return;

	UINT16 intervalRate;
	UINT16 interval;
	INT32 intervalIndex;
	INT32	addPoint = 10;

	for( i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( g_pCharaManager->getCharaByNetIdx( i) == nullptr)
			continue;

		for( j = 0; j < MAX_OBJECT_COUNT; j++)
		{
			intervalRate = m_pDefenceData->getIntervalRateObject( j) * 10 ;

			// 10 �ۼ�Ʈ���� �޽����� ����ݴϴ�.
			interval = pInfo->getUserDamage( j, i);

			if( interval > m_pDefenceData->getRecvDamageInfo()->getUserDamage( j, i) )
			{
				if( j == 0)
					addPoint = 10;
				else
					addPoint = 15;

				intervalIndex = m_pDefenceData->getObjIntervalIndex( j, i);

				while( (interval > intervalRate * (UINT16) intervalIndex) )
				{	
					// �޼��� ���
					if(CHud::i() != nullptr)
					{
						CHud::i()->CreateInGameMessage(i, j, addPoint);

						if( i == BattleSlotContext::i()->getMySlotIdx())
							CHud::i()->AddTargetBonus( addPoint);
					}

					intervalIndex++;

					m_pDefenceData->setObjIntervalIndex( j, i, intervalIndex);
				}
			}
		}
	}

	pInfo->CopyTo( m_pDefenceData->getRecvDamageInfo() );

	if( !BattleServerContext::i()->IsP2PHost() )
	{
		pInfo->CopyTo( m_pDefenceData->getSendDamageInfo());	// Host Lost�� ���ο� ���嵵 ���� �����͸� ������ ���� �ʿ䰡 �ֽ��ϴ�.

		for( i = 0; i < MAX_OBJECT_COUNT; i++)
		{
			REAL32 rObjectHP = (REAL32) m_pDefenceData->getRecvDamageInfo()->getObjectHP( i);

			REAL32 rDamage = m_pDefenceData->getDefenceObj( i)->getCurrentHP() - rObjectHP;

			if( rDamage > 0.f)
				m_pDefenceData->getDefenceObj( i)->Damage( rDamage, nullptr, nullptr);
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
			else if( rDamage < 0.f)
			{
				I3PRINTLOG(I3LOG_WARN,  "invalid Calc HP for client. %.4f %d", m_pDefenceData->getDefenceObj( i)->getCurrentHP(), rObjectHP);
			}
#endif
			m_pDefenceData->getDefenceObj( i)->setCurrentHP( rObjectHP );
		}
	}
}

void CGameMissionManager::setDefenceInfoNoEffect( DEFENCE_INFO * pInfo)
{
	INT32 i, j;

	for( i = 0; i < MAX_OBJECT_COUNT; i++)
	{
		INT32 intervalRate = m_pDefenceData->getIntervalRateObject( i) * 10 ;

		I3ASSERT( intervalRate > 0);

		for( j = 0; j < SLOT_MAX_COUNT; j++)
		{
			INT32 intervalIndex = (pInfo->getUserDamage( i, j) / intervalRate) + 1;
			m_pDefenceData->setObjIntervalIndex( i, j, intervalIndex);
		}
	}

	pInfo->CopyTo( m_pDefenceData->getRecvDamageInfo() );

	for( i = 0; i < MAX_OBJECT_COUNT; i++)
	{
		REAL32 rObjectHP = (REAL32) m_pDefenceData->getRecvDamageInfo()->getObjectHP( i);
		INT32 iState = m_pDefenceData->getDefenceObj( i)->GetStateByHP(rObjectHP);
		GlobalFunc::ChangeObjState_( m_pDefenceData->getDefenceObj( i), iState, 0.f);
		m_pDefenceData->getDefenceObj( i)->setCurrentHP( rObjectHP );
	}

	if( !BattleServerContext::i()->IsP2PHost() )
	{
		pInfo->CopyTo( m_pDefenceData->getSendDamageInfo());	// Host Lost�� ���ο� ���嵵 ���� �����͸� ������ ���� �ʿ䰡 �ֽ��ϴ�.		
	}

	SetResetObjFlag( !IntrudeContext::i()->IsInterEnter());
}

void CGameMissionManager::setDefenceInfoIntrude( DEFENCE_INFO * pInfo, UINT32 SlotIndex )
{
	for( INT32 i = 0; i < MAX_OBJECT_COUNT; i++)
	{
		INT32	intervalRate	= m_pDefenceData->getIntervalRateObject( i) * 10 ;

		I3ASSERT( intervalRate > 0);

		UINT16	Damage			= pInfo->getUserDamage( i, SlotIndex );
		INT32	intervalIndex	= (Damage / intervalRate) + 1;

		m_pDefenceData->setObjIntervalIndex( i, SlotIndex, intervalIndex);
		m_pDefenceData->getRecvDamageInfo()->setUserDamage( i, SlotIndex, Damage );
		m_pDefenceData->getSendDamageInfo()->setUserDamage( i, SlotIndex, Damage );
	}
}

bool CGameMissionManager::isMissionObject( i3Object * pObj)
{
	if( getBlueObject() == pObj)	return true;
	if( getRedObject() == pObj)		return true;

	INT32 i;

	if( m_pDefenceData != nullptr)
	{
		for( i = 0; i < MAX_OBJECT_COUNT; i++)
		{
			if( m_pDefenceData->getDefenceObj( i) == pObj)
				return true;
		}
	}

	if( i3::same_of<CGameObjectWeaponBox*>(pObj))
	{
		CGameObjectWeaponBox * pWeaponBox = static_cast<CGameObjectWeaponBox*>(pObj);
		if( pWeaponBox->GetWeaponType() == WT_CROWNWATERMELON)
			return true;
	}

	return false;
}

bool CGameMissionManager::IsPossibleDisarmBomb()
{
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		if(getMissionState() == GAME_MISSION_STATE_INSTALLED)
		{
			return true;
		}
	}
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY))
	{
		return true;
	}
	else
	{
		if(getMissionState() == GAME_MISSION_STATE_INSTALLED &&	// ��ġ ������
			getMyMissionObject() == MY_MISSION_OBJECT_DEFENSIVE )	// ���������
		{
			return true;
		}
	}

	return false;
}

bool CGameMissionManager::CheckCrownMinimap(void)
{
	bool bview = true;
	bool bTeamCheck = false;

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)  == true)
	{
		TEAM_TYPE myTeam = CHARA::CharaTeamType2TeamType( BattleSlotContext::i()->getMyTeam());
		if(myTeam == TEAM_BLUE)
			bTeamCheck = true;
	}

	if(bTeamCheck == true)
	{
		if(	m_UsurpationData.m_iUsurpationIdx == 0xff)
			bview = true;
		else
			bview = false;
	}
	return bview;
}

void CGameMissionManager::MsgHud(INT32 idx)
{
	if(CHud::i() != nullptr)
	{
		if(idx== 255)
		{
			CHud::i()->ConvoyMsgDrop();
		}
		else
		{
			CHud::i()->ConvoyMsgGet();
		}
	}

}
#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaLocal.h"
#include "StageBattle.h"
#include "GameCamera_FPS.h"

I3_CLASS_INSTANCE( CGameCharaLocal, CGameCharaPlayer );

CGameCharaLocal::CGameCharaLocal( void)
{
	OnInitVariable();
}

CGameCharaLocal::~CGameCharaLocal( void)
{	
}

void CGameCharaLocal::OnInitVariable( void)
{
	CGameCharaPlayer::OnInitVariable();

	//m_KeyPressInput			= 0;
	//m_KeyStrokeInput		= 0;

//	m_bDisarmingBomb		= FALSE;
//	m_bArmingBomb			= FALSE;

	//m_rWorldCollisionLength = 0.f;

#if defined( I3_DEBUG)
	m_pRenderableWorldCol		= NULL;
	m_pRenderableRoughHitCol	= NULL;
#endif
}

/*
BOOL CGameCharaLocal::_CrossCollisionToWorld( void )
{	
	VEC3D vTargetPos, vAt, vTemp;
	I3_PHYSIX_HIT_RESULT * pResult;

	m_rWorldCollisionLength	= 1000000.f;

	i3Vector::Scale( &vAt, i3Matrix::GetAt( g_pCamera->getCamMatrix()), -1000000.f );
	i3Vector::Add( &vTargetPos, &vAt, i3Matrix::GetPos( g_pCamera->getCamMatrix()));

	m_CameraLine.SetStart( i3Matrix::GetPos( g_pCamera->getCamMatrix()));
	m_CameraLine.SetEnd( &vTargetPos);
	
	pResult = g_pFramework->raycastClosest( &m_CameraLine, CGRP_ATTACK);
	if( pResult != NULL)
	{
		i3Vector::Sub( &vTemp, m_CameraLine.GetStart(), &pResult->m_Intersect );
		m_rWorldCollisionLength = i3Vector::Length( &vTemp);
		return TRUE;
	}

	return FALSE;
}

*/
void CGameCharaLocal::Create( i3GameResChara * pRes, CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo)
{
	CGameCharaPlayer::Create( pRes, pCharaInfo, pDBInfo);
	//	Add after here

	//setAnimSet( CHARA_ANIMSET_1PV);
	Change1PV();
	ChangeLocal();

	//i3BoundBox * pBoundBox = m_pBody->GetBound();
	//if( pBoundBox != NULL )
	//	m_pBody->SetBound( NULL );
}

void CGameCharaLocal::ResetChara( void)
{
	CGameCharaPlayer::ResetChara();

	//	Ű �ʱ�ȭ
//	m_KeyPressInput			= 0;
//	m_KeyStrokeInput		= 0;

//	m_bDisarmingBomb		= FALSE;
//	m_bArmingBomb			= FALSE;
	
	//EFFECT_Clear();					// ��ũ�� ����Ʈ Ŭ����
}

void CGameCharaLocal::OnUpdate( REAL32 tm)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	// ������ ó�� ������ �ſ� �߿��ϰ�, ������ ���� ���� �ʿ��ϴ�.

	//REAL_PROFILE( "CGameCharaLocal");  

	CGameCharaPlayer::OnUpdate( tm);

	

	//	Add code here
	/*
	if( !g_pGameContext->IsP2PActivate() || !g_pGameContext->IsBattle() )	return;

	INT32 nMyIdx = getCharaInfo()->_NetIdx;
	I3ASSERT( nMyIdx != -1);

	if( g_pCharaManager->getCharaTask( nMyIdx) == CHARA_TASK_RESPAWN)	//	���� ������ �����̸�
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		if( ((CStageBattle*)pStage)->getStageState() == GAMESTAGE_STATE_BATTLE ||
			((CStageBattle*)pStage)->getStageState() == GAMESTAGE_STATE_ENDROUND )
		{
			//_ProcessOwner( tm);
			//_ProcessWeapon( tm);
			//_ProcessExtension( tm );
		}
		else
		{
			ACTION_ReturnToReady( TRUE);
		}
	}
	else			//	ĳ���Ͱ� �׾�����
	{	
		//_ProcessRespawn( tm);
	}

	//	������� HUD ȭ�� ����Ʈ 
//	_ProcessScreenEffect( tm);

	// ��Ʈ��ũ ������ ����
	//_SaveNetworkData();

	//if( g_pGameContext->IsP2PHost() )
	//{
	//	if( getCharaNetIndex() != -1 )
	//	{
	//		GAMEINFO_RECV * pGameInfo		= g_pGameContext->getRecvGameInfo( getCharaNetIndex() );
	//		pGameInfo->_tDefData._nHP		= (UINT8) m_pCharaInfo->_Hp;
	//	}
	//}
	*/
}

void CGameCharaLocal::DumpChara( void)
{
	CGameCharaPlayer::DumpChara();
}

/*
void CGameCharaLocal::_SetMoveDir( UINT64 key)
{
	if( key & GAME_KEY_MOVE_FORWARD )			m_MoveDir.z = 1.0f;		
	else if( key & GAME_KEY_MOVE_BACKWARD)		m_MoveDir.z = -1.0f;
	else										m_MoveDir.z = 0.0f;

	if( key & GAME_KEY_MOVE_LEFT )				m_MoveDir.x = 1.0f;		
	else if( key & GAME_KEY_MOVE_RIGHT )		m_MoveDir.x = -1.0f;
	else										m_MoveDir.x = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Process
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CGameCharaLocal::ProcessFirst( REAL32 tm)
{	
	CGameCharaPlayer::ProcessFirst( tm);
	//	Add code here

	
	
	
	if( g_pFramework->getKeyStroke() & GAME_KEY_CMD_ALLDEATH )
	{
		if( g_pGameContext->IsP2PHost() )
		{
			VEC3D vec;
			for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
			{
				CGameCharaPlayer * pChara = g_pCharaManager->getCharaByNetIdx( i );

				if( pChara != NULL && pChara != this )
				{
					if( pChara->getCharaTeam() != getCharaTeam() )
					{
						i3Vector::Sub( &vec, pChara->GetPos(), GetPos() );
						i3Vector::Normalize( &vec, &vec );
						
						g_pCharaManager->DeathChara( i, g_pGameContext->getMySlotIdx(), &vec, CHARA_DEATH_H, CHARA_HIT_HEAD );
						g_pGameContext->WriteCharaDeath( g_pGameContext->getMySlotIdx(), i, WEAPON_USAGE_MELEE, WEAPON_CLASS_KNIFE,
							WEAPON_NUM_KNIFE_M7, CHARA_DEATH_H, CHARA_HIT_HEAD, &vec );
					}
				}
			}
		}
	}
	
	{
		if( isStateMask( CHARA_STATE_FIRSTRESPAWN) )
		{
			// ���� ������� ���Ⱑ ������ ���忡 �ִ� ������� üũ�մϴ�.
			WEAPON_CheckTaking( tm);

			if( getKeyStrokeInput() & GAME_KEY_ACTION_DROP )
			{
				if( isAlive() )
					g_pGameContext->WriteMyDropWeapon( WEAPON_getCurrentSlotNum() );
			}

			if( getKeyStrokeInput() & GAME_KEY_ACTION_TURNBACK)
			{
				ACTION_TurnBack();
			}
		}

		//	Ű �Է�( �������߿��� Ű�Է��� ����)
		if(	!g_pCamera->IsTypeOf( CGameCamera_FLY::GetClassMeta()) && 
			!isEnableStateMask( CHARA_STATE_DISABLE_CONTROL) &&
			g_pFramework->getGameKeyMode() != GAMEKEY_MODE_MENU )
		{
			m_KeyPressInput		= g_pFramework->getKeyPress();
			m_KeyStrokeInput	= g_pFramework->getKeyStroke();

			//REAL32 fl = 0.16666666f * tm;
			//	ĳ���� ȸ��
			if( !g_pEnvSet->m_bEnableMouseInvert)
			{
				m_Rho		-= g_pFramework->getMouseMoveDelta()->y * 0.005f;
			}
			else
			{
				m_Rho		+= g_pFramework->getMouseMoveDelta()->y * 0.005f;
			}

			m_Theta		-= g_pFramework->getMouseMoveDelta()->x * 0.005f; 

			if( g_pFramework->getMouseMoveDelta()->x != 0.0f)
			{
				// ���� �ݵ��� ȸ�� ��, ���콺�� ���ư��� ȸ�� ��带 Cancel�Ѵ�.
				//if( m_RecoilData.m_HorzState == RECOIL_HORZ_RESTORE)
					m_RecoilData.m_HorzState = RECOIL_HORZ_NA;
			}
			
			m_Theta		= i3Math::NormalizeAngle( m_Theta);
			m_Rho		= i3Math::NormalizeAngle( m_Rho);
			//	����

			REAL32 fAimingAngleMax = g_pCharaInfoDataBase->getGlobal()->m_rAimingAngleMax;	//	�ִ밢
			REAL32 fAimingAngleMin = g_pCharaInfoDataBase->getGlobal()->m_rAimingAngleMin;	//	�ּҰ�

			//	���� ���� (���۽� �ٶ󺸴� �ü��� ������ 0���� ����)
			if( m_Rho > I3_DEG2RAD( fAimingAngleMax))			m_Rho = I3_DEG2RAD(fAimingAngleMax);	
			else if( m_Rho < I3_DEG2RAD( fAimingAngleMin))		m_Rho = I3_DEG2RAD(fAimingAngleMin);	
		}
		else
		{
			m_KeyPressInput = m_KeyStrokeInput = 0;

			if( g_pFramework->getGameKeyMode() != GAMEKEY_MODE_MENU )
				MoveStop();	//	Ű �Է��� ������ �̵� ����
		}	

		//	�̵� ���� ó���մϴ�.
		ProcessMoveInput( tm);
	}
}

void CGameCharaLocal::_ProcessOwner( REAL32 tm)
{	//	�ڱ��ڽſ� �ʿ��� ó���� �մϴ�.
	//	��� �ð� üũ
	//ProcessTimer( tm);	
}

void CGameCharaLocal::_ProcessWeapon( REAL32 tm)
{	
	UINT64 InputKeyStroke = getKeyStrokeInput();
	UINT64 InputKeyPress = getKeyPressInput();
	
	WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pCurWeapon != NULL);

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != NULL);

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	BOOL bCancelMission = FALSE;	//	�̼� �ൿ ��� ����
	BOOL bCancelDisarm = FALSE;		//	��ź ��ü ��� ����


	////////////////////////////////////////////////////////////	���� �߻�
	if( InputKeyPress & GAME_KEY_ACTION_ATTACK )
	{	
		if( (!isEnableStateMask( CHARA_STATE_DISABLE_SHOT) && !pCurWeapon->isStateMask( WSM_FIRING))
			&& (( pCurWeapon->getMagazineType() != WEAPON_DUAL_MAGAZINE_RIGHT && pCurWeapon->getLoadedBulletCount() > 0)
			|| (pCurWeapon->getMagazineType() == WEAPON_DUAL_MAGAZINE_RIGHT && pCurWeapon->getLoadedBulletDualCount() > 0 ))  )
		{	
			if( pCurWeapon->isFireEnabled() )
			{
				//	Mission ���� ����ó��
				if( pCurWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION)
				{
					if( g_pGameContext->IsBombMissionMode() || !g_pConfig->m_bNetworking)
					{	//	�ൿ
						if( ACTION_ActiveMission( pCurWeapon))
							bCancelMission = FALSE;
					}
				}
				else
				{	
					BOOL bFireEnable = TRUE;

					if( g_pGameContext->IsBombMissionMode())
					{
						if( isStateMask( CHARA_STATE_DISARMINGBOMB))	//	��ź ��ü ���϶� �߻� �Ұ�
							bFireEnable = FALSE;
					}

					if( bFireEnable)
					{
						//	���� �߻�!!		(��ô ���⸸ �߻��غ�)
						if( pCurWeaponInfo->m_TypeClass == WEAPON_CLASS_THROWING)
							bCancelDisarm = ACTION_FireReady();					
						else
						{
							if (g_pFramework->IsBattleStage())
							{
								bCancelDisarm = ACTION_Fire();
							}
						}
					}
				}
			}
			else if( pCurWeapon->getWeaponInfo()->m_TypeTrigger == WEAPON_TRIGGER_RELOADING_SHOT )
			{	// ������ �߻� �����ϸ� ź�� ���� ������ ���Ѵ�
				bCancelMission = ACTION_LoadBullet();
			}
		}
		else if( pCurWeapon->getState() == WEAPON_STATE_NEEDTO_LOADMAG)
		{
			bCancelMission = ACTION_LoadMagazine();		//	����
		}
		else if( pCurWeapon->getState() == WEAPON_STATE_NEEDTO_LOADBULLET)
		{
			bCancelMission = ACTION_LoadBullet();		//	����
		}
	}
	else if( (InputKeyPress & GAME_KEY_ACTION_ATTACK) == 0 )	//	�߻� Ŭ���� ��ư�� ���� (Key Stroke ����)
	{
		pCurWeapon->FireEnd();		//	�߻� ���� ������ ���⿡ �˷��ش�.	

		bCancelMission = TRUE;  

		//	��ô ������ ��� ��ư�� ������ ������.
		if( pCurWeaponInfo->m_TypeClass == WEAPON_CLASS_THROWING && pCurWeapon->isStateMask( WSM_FIRING) )
		{
			if( getActionContext()->isUpper( CHARA_UPPER_FIRE_READY) && !isEnableStateMask( CHARA_STATE_DISABLE_SHOT) && pCurWeapon->getLoadedBulletCount() > 0)
			{
				bCancelDisarm = ACTION_Fire();
			}
		}
	}

	//�Ѿ��� ���ڶ� ������ΰ�� �ݹ߽õ��� �ϰԵǸ� ���� �Ҹ�
	if( InputKeyStroke & GAME_KEY_ACTION_ATTACK )
	{
		if( pCurWeapon->getState() == WEAPON_STATE_EMPTY_MAGAZINE)
		{
			if( pCurWeaponInfo->m_TypeFire == WEAPON_FIRE_RANGE )
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand()%2, GetPos(), TRUE);
			}
		}
	}

	////////////////////////////////////////////////////////////	���� ����	
	if( InputKeyPress & GAME_KEY_ACTION_SUBFUNC )	//	<- Pressed ��
	{
		switch( pCurWeaponInfo->m_TypeClass)
		{
		//	���� �������� ����
		case WEAPON_CLASS_KNIFE:
			if( (!isEnableStateMask( CHARA_STATE_DISABLE_SHOT) && pCurWeapon->isFireEnabled() && 
				!pCurWeapon->isStateMask( WSM_FIRING) && pCurWeapon->getLoadedBulletCount() > 0))	
			{
				BOOL bFireEnable = TRUE;

				if( g_pGameContext->IsBombMissionMode())
				{
					if( isStateMask( CHARA_STATE_DISARMINGBOMB))	//	��ź ��ü ���϶� �߻� �Ұ�
						bFireEnable = FALSE;
				}

				//	���� �߻�!!	
				if( bFireEnable)
					bCancelDisarm = ACTION_Fire( 0, WEAPON_SECONDARY_FIRE);
			}
			break;
		}
		
	}

	////////////////////////////////////////////////////////////	���ε�
	if( InputKeyStroke & GAME_KEY_ACTION_RELOAD )
	{
		if( pCurWeapon->isLoadMagazineEnabled() && !(InputKeyPress & GAME_KEY_ACTION_SPECIAL) )
		{
			bCancelMission = ACTION_LoadMagazine();
		}
	}

	////////////////////////////////////////////////////////////	���� ��ü	
	CHud * pHud = ((CStageBattle*)g_pFramework->GetCurrentStage())->getHud();

	if( !isEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE) && !pHud->IsRadioChat() )
	{
		if(! g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT ))
		{
			if( InputKeyStroke & GAME_KEY_CHANGEWEAPON1 )
			{
				bCancelMission = ACTION_SwapWeapon( 0);
			}
			else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON2 )
			{
				bCancelMission = ACTION_SwapWeapon( 1);
			}
			else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON3 )
			{
				bCancelMission = ACTION_SwapWeapon( 2);
			}
			else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON4 )
			{
				bCancelMission = ACTION_SwapWeapon( 3);
			}
			else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON5 )
			{
				bCancelMission = ACTION_SwapWeapon( 4);
			}
			else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON6 )
			{
				bCancelMission = ACTION_SwapWeapon( 5);
			}
		}

		//	���� ����� ��ü
		if ( InputKeyStroke & GAME_KEY_CHANGEWEAPON_QUICK )
		{
			bCancelMission = ACTION_SwapOldWeapon();
		}
		//	Mouse Wheel Down (���� ���� ��ü)
		else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON_NEXT)
		{			
			bCancelMission = ACTION_SwapNextWeapon();
		}
		//	Mouse Wheel Up (���� ���� ��ü)		
		else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON_PRE)
		{			
			bCancelMission = ACTION_SwapPrevWeapon();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	�� ���� Ư�� ��ư�̿ܿ��� bCancelMission�� ���õǾ� �ٸ� ���� ��ư�� �����ؼ� �ȵ˴ϴ�.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////	Ư�� ��ư (��ź ��ü��)
	if( InputKeyPress & GAME_KEY_ACTION_SPECIAL )
	{	
		if( !bCancelDisarm)
		{
			//	�̼��� �� ���
			if( g_pGameContext->IsBombMissionMode() )
			{			
				//	�̼� ������
				if( (pMissionMng->getMissionState() == GAME_MISSION_STATE_INSTALLED ))
				{					
					ACTION_ActiveDisarm();		//	��Ʈ��ũ ��ź ��ü
				}
				//	�̼� ����
				else if( isStateMask( CHARA_STATE_DISARMINGBOMB))
				{
					removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
					removeStateMask( CHARA_STATE_DISARMINGBOMB);
				}
			}
		}
	}
	else if( (InputKeyPress & GAME_KEY_ACTION_SPECIAL) == 0)
	{	//	�̼��� �� ���
		if( isStateMask( CHARA_STATE_DISARMINGBOMB))
		{
			if( g_pGameContext->IsBombMissionMode() )
			{
				bCancelDisarm = TRUE;
			}
		}
	} 

	////////////////////////////////////////////////////////////////////////
	//	�̼��� �ൿ ���	(�Է� �ൿ�� ������ �޴´�.)
	if( bCancelMission)
	{
		//	�̼� ���
		ACTION_CancelMission();		//	�ൿ ���
	}
	
	//	��ź ��ü ���	(�Է� �ൿ�� ������ �޴´�.)	
	if( bCancelDisarm)
	{
		if( isStateMask( CHARA_STATE_DISARMINGBOMB))
		{
			ACTION_CancelDisarm();			//	��Ʈ��ũ ��ź ��ü ���
		}
	}
}

*/

/*
void CGameCharaLocal::_ProcessScreenEffect( REAL32 tm)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//	���� ����Ʈ �ð� ��� (����ź���� ���)
	if( m_rFlashElapseTime != 0.f)
	{		
		CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
		I3ASSERT( pStage != NULL);

		CHud * pHud = pStage->getHud();

		if( m_rFlashElapseTime <= 0.f)
		{			
			i3Sprite2D * pSrc = pHud->GetScreenEffect();
			I3ASSERT( pSrc != NULL);

			INT32 FadeAlpha = (INT32) i3Color::GetA( pSrc->GetColor(0));		
			FadeAlpha -= (INT32) (tm * 255.0f);
			if( FadeAlpha < 0)	FadeAlpha = 0;

			//	����Ʈ�� ������ ������ ��������.
			if( FadeAlpha <= 0)
			{
				m_rFlashElapseTime = 0.0f;
				m_rFlashEffectiveTime = 0.0f;

				pHud->SetScreenEffectEnable( FALSE);
				pHud->SetScreenTex2DEnable( FALSE);

				return;	//	������ �մϴ�.
			}
			else
			{
				pHud->SetScreenEffectEnable( TRUE);
				pHud->SetScreenTex2DEnable( 0, TRUE);
				pHud->SetScreenEffectColor( 255, 255, 255, (UINT8) FadeAlpha);			
				pHud->SetScreenTex2DColor( 0, 255, 255, 255, (UINT8) FadeAlpha);
			}
		}
		else	//	����Ʈ ��
		{
			REAL32 fMinReduceAlpha = 150.0f;
			REAL32 fMinAlpha = 255.0f - fMinReduceAlpha;
			REAL32 fGapTime = m_rFlashEffectiveTime - m_rFlashElapseTime;
			
			if( fGapTime <= 0.5f)		//	���ڱ� �Ͼ�ȭ������ ���̵� �ȴ�.
			{
				INT32 a = (INT32) (fGapTime * 2.0f * 255.f);
				if( a > 255) a = 255;

				pHud->SetScreenEffectColor( 255, 255, 255, (UINT8) a);
			}	
			else if( fGapTime <= 1.0f)	//	�Ͼ�ȭ���� ���ӵȴ�.
			{
				pHud->SetScreenEffectColor( 255, 255, 255, 255);
			}					 
			else						//	�Ͼ�ȭ���� �ﷷ(?) �Ÿ���.
			{			
				pHud->SetScreenEffectEnable( TRUE);

				REAL32 fAlpha = i3Math::abs( i3Math::sin( m_rFlashElapseTime)) * fMinReduceAlpha;
				UINT8 alpha = (UINT8) (255.f - fAlpha);
				if( alpha < (UINT8) fMinAlpha)		alpha = (UINT8) fMinAlpha;

				pHud->SetScreenEffectColor( 255, 255, 255, alpha);
			}

			pHud->SetScreenEffectEnable( TRUE);
			pHud->SetScreenTex2DEnable( 0, TRUE);
			pHud->SetScreenTex2DColor( 0, 255, 255, 255, 150);

			for( INT32 i = 1; i < SCR_BACKBUFFER_COUNT; i++)
			{	//	ù��° ��ũ���� ���� �ȳ�����
				pHud->SetScreenTex2DEnable( i, FALSE);
			}
		}

		m_rFlashElapseTime -= tm;			
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//	������ ��ũ�� ����Ʈ (����ź���� ���)
	if( m_rDamageElapseTime != 0.0f)
	{
		i3RasterizeChain_Blur * pRasterizeBlur = g_pFramework->GetRasterizeBlur();
		I3ASSERT( pRasterizeBlur != NULL);

		CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
		I3ASSERT( pStage != NULL);
		CHud * pHud = pStage->getHud();

		m_rDamageElapseTime -= tm;

		if( pRasterizeBlur->getEnable())
		{	//	�� ȿ��(?)�� ������
			if( m_rDamageElapseTime <= 0.0f)
			{	//	�� ȿ��(?)�� ������ �������. (���� Min ���ϰ� �Ǹ� �ڵ������� ���ο��� Disable�ȴ�)
				pRasterizeBlur->AddBlurAlpha( -0.02f);

				//	������ ���İ� �������.
				I3COLOR * pBlurColor = pHud->GetScreenEffectColor();				
				INT32 a = (INT32) i3Color::GetA( pBlurColor);
				a -= 2;
				if( a > 255)	a = 255; 
				else if( a < 0) a = 0;
				i3Color::SetA( pBlurColor, (UINT8) a);

				pHud->SetScreenEffectEnable( TRUE);
				pHud->SetScreenEffectColor( pBlurColor);

				//I3TRACE( "���� �����..(%.4f)\n", pRasterizeBlur->GetBlurAlpha());
			}
			
			{
				//I3TRACE( "���� ������..(%.4f)\n", pRasterizeBlur->GetBlurAlpha());
			}
		}
		else
		{	//	ȿ�� ����
			pHud->SetScreenEffectEnable( FALSE);
			
			m_rDamageElapseTime = 0.0f;
		}

	}
}
*/

/*
BOOL CGameCharaLocal::ACTION_ActiveMission( WeaponBase * pWeapon)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

#if defined( I3_DEBUG)
	if( g_pConfig->m_bNetworking )
	{
#endif
	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// ���ĸ�尡 �ƴѰ��
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) ||			// ��ġ���� �ƴѰ��
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// �������� �ƴѰ��
		return FALSE;
#if defined( I3_DEBUG)
	}
#endif

	I3ASSERT( pWeapon != NULL);
	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);
	
	CGameMissionManager * pMissionManager = g_pGameContext->getMissionManager();
	
	if( (pWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION) && (pWeaponInfo->m_nNumber == WEAPON_NUM_MISSION_C4 ) )
	{		
		if( (getActionContext()->isLower( CHARA_LOWER_IDLE) || getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK)) &&
			(getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE)))
		{	
			switch( pMissionManager->getMissionState() )
			{
			case GAME_MISSION_STATE_INBATTLE :					//	�̼ǿ� ��ź�� ��ġ�Ǿ� ���� �ʴٸ�
				if( pMissionManager->IsEnableMy())
				{//	��ź ��ġ�� �����ϸ�
					//	�̵� �Ұ�. 
					i3Vector::Zero( &m_MoveDir);

					if( !getActionContext()->isLower( CHARA_LOWER_IDLE))
						ACTION_IdleLower();

					ACTION_Crouch();	// ��ġ�� ������ �ɴ´�.
					ACTION_Fire();		// ��ź ��ġ!!

					addEnableStateMask( CHARA_STATE_DISABLE_MOVE);
					addEnableStateMask( CHARA_STATE_DISABLE_CROUCH);

					m_bArmingBomb = TRUE;

					g_pGameContext->WriteMyInstallingMission( TRUE );

					return TRUE;
				}
				else
				{
					CStageBattle * pBattle = (CStageBattle*)g_pFramework->GetCurrentStage();
					I3ASSERT( pBattle != NULL);
					pBattle->getHud()->SetMissionNotice( STBL_IDX_BOMB_MISSION_NONAREA);			// ���������� ��ġ�Ҽ� �ֽ�.
				}
				break;

			case GAME_MISSION_STATE_INSTALLED :					//	�̹� ��ġ �ƴٸ�
				{
					removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
				}
				
			default :
				{
				//	CStageBattle * pBattle = (CStageBattle*)g_pFramework->GetCurrentStage();
				//	I3ASSERT( pBattle != NULL);
				//	pBattle->getHud()->SetMissionNotice( STBL_IDX_BOMB_MISSION_NONAREA);			// ���������� ��ġ�Ҽ� �ֽ�.
				}
				
				break;
			}
		}
	}

	return FALSE;
}

BOOL CGameCharaLocal::ACTION_ActiveDisarm( void)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// ���ĸ�尡 �ƴѰ��
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INSTALLED) ||			// ��ġ �İ� �ƴѰ��
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_DEFENSIVE) )		// ������� �ƴѰ��
	{
		return FALSE;
	}
	
	//	��ź ��ü	
	if( (getActionContext()->isLower( CHARA_LOWER_IDLE) || getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK)) &&
		(getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE)))
	{	
		switch( pMissionMng->getMissionState() )
		{
		case GAME_MISSION_STATE_INSTALLED :					//	�̼ǿ� ��ź�� ��ġ�Ǿ� �ִٸ�
			if( pMissionMng->IsEnableMy() && pMissionMng->IsEnableCrossHit() )
			{
				WeaponBase * pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_C4);

				if( pWeapon != NULL)
				{
					I3ASSERT( pWeapon->IsExactTypeOf( WeaponC4::GetClassMeta()));
					WeaponC4 * pBomb = (WeaponC4 *) pWeapon;
					
					m_bDisarmingBomb = TRUE;

					pBomb->ActiveDisarm( TRUE);
				}
				else
				{
					m_bDisarmingBomb = FALSE;
				}
			}
			else
			{
				m_bDisarmingBomb = FALSE;
			}
			break;
		}
	}

	if( m_bDisarmingBomb)
	{
		//	�̵� �Ұ�. 
		i3Vector::Zero( &m_MoveDir);

		if( !getActionContext()->isLower( CHARA_LOWER_IDLE))
			ACTION_IdleLower();

		if( !getActionContext()->isUpper( CHARA_UPPER_IDLE))
			ACTION_IdleUpper();

		ACTION_Crouch();	// ��ü�� ������ �ɴ´�.

		addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		addEnableStateMask( CHARA_STATE_DISABLE_MOVE);
		addEnableStateMask( CHARA_STATE_DISABLE_CROUCH);

		g_pGameContext->WriteMyUninstallingMission( TRUE );		//	��ź ��ü

		return TRUE;
	}
	else
	{
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		g_pGameContext->WriteMyUninstallingMission( FALSE );

		//I3TRACE( "-- ��ź ��ü���...\n");

		return FALSE;
	}
}

BOOL CGameCharaLocal::ACTION_CancelMission( void)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// ���ĸ�尡 �ƴѰ��
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) ||			// ��ġ ���� �ƴѰ��
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// �������� �ƴѰ��
		return FALSE;
	
	if( pMissionMng->IsInstalling( g_pGameContext->getMySlotIdx() ) )
	{	//	�ڽ��� ��ź ��ġ ���̸� ���
		g_pGameContext->WriteMyInstallingMission( FALSE );	//	��ź ��ġ ���
		
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		//if( getActionContext()->isUpper( CHARA_UPPER_FIRE) || getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE))
			ACTION_ReturnToReady( TRUE);

		//I3TRACE("-- ��ź ��ġ ���!!\n");

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaLocal::ACTION_CancelDisarm( void)
{	//	��ź ��ü ���̸�
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// ���ĸ�尡 �ƴѰ��
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INSTALLED) ||			// ��ġ �İ� �ƴѰ��
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_DEFENSIVE) )		// ������� �ƴѰ��
		return FALSE;

	INT32 mySlotIdx = g_pGameContext->getMySlotIdx();

	if( m_bDisarmingBomb)
	{
		//	�ڽ��� ��ź ���� ���̸� ���
		//I3TRACE("isUnInstalling() : %d, WhoUnInstall() : %d\n", pMissionData->IsUninstalling( g_pGameContext->getMySlotIdx() ), pMissionData->WhoUninstall( nMyNetIdx));
		if( pMissionMng->IsUninstalling( mySlotIdx ))
		{			
			g_pGameContext->WriteMyUninstallingMission( FALSE );		//	��ź ��ü ���

			removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

			m_bDisarmingBomb = FALSE;

			WeaponBase * pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_C4);

			if( pWeapon != NULL)
			{
				I3ASSERT( pWeapon->IsExactTypeOf( WeaponC4::GetClassMeta()));
				WeaponC4 * pBomb = ( WeaponC4*)pWeapon;

				pBomb->ActiveDisarm( FALSE);
			}

			//I3TRACE("-- ��ź ��ü ���!!\n");

			return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HUD
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameCharaLocal::ShowDamageHUD( VEC3D * pShooter, VEC3D * pHitUser, REAL32 fHitTheta )
{
	VEC3D	vDir;

	i3Vector::Sub( &vDir, pShooter, pHitUser );
	i3Vector::Normalize( &vDir, &vDir );
	
	REAL32 Rotation = atan2f( i3Vector::GetX( &vDir ), i3Vector::GetZ( &vDir ) ); // XZ ���
	REAL32 rTargetAngle = i3Math::NormalizeAngle( Rotation );
	REAL32 fAngleGap = i3Math::NormalizeAngle( rTargetAngle - fHitTheta );		
	((CStageBattle*)(g_pFramework->GetCurrentStage()))->getHud()->SetDamageDirection( fAngleGap);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Network
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameCharaLocal::_SaveNetworkData( void)
{
#ifdef I3_DEBUG
	if( g_pConfig->m_bNetworking == FALSE )	return;
#endif

	BOOL	bSaved = FALSE;
	GAMEINFO_SEND * pMyGameInfo = g_pGameContext->getSendGameInfo();

	if( pMyGameInfo->_nDecentSendCount > 0 )
	{
		bSaved = TRUE;
	}

	// �߻�ü�� ���� ó��
	if( pMyGameInfo->_nFireSendCount > 0 )
	{	
		bSaved = TRUE;
	}

	// �׼ǿ� ���� ó��
	if( bSaved == FALSE && pMyGameInfo->_nMissionSendCount <= 0 )
	{	
		if( pMyGameInfo->_tDefData._Body != (UINT32) getActionContext()->getBody() )
			bSaved = TRUE;

		if( pMyGameInfo->_tDefData._Body2 != (UINT32) getActionContext()->getBody2() )
		{
			bSaved = TRUE;
		}

		if( pMyGameInfo->_tDefData._Upper != (UINT32) getActionContext()->getUpper() )
		{
			if( !getActionContext()->isUpper( CHARA_UPPER_FIRE) &&
				!getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) &&
				!getActionContext()->isUpper(CHARA_UPPER_DAMAGE) &&	
				!getActionContext()->isUpper( CHARA_UPPER_MOVE) )
				bSaved = TRUE;
		}

		if( pMyGameInfo->_tDefData._Lower != (UINT32) getActionContext()->getLower() )
		{
			if( !getActionContext()->isLower( CHARA_LOWER_DEATH) &&
				!getActionContext()->isLower( CHARA_LOWER_MOVE) &&
				!getActionContext()->isLower( CHARA_LOWER_WALK) )
				bSaved = TRUE;
		}

		if( pMyGameInfo->_tDefData._nFireAction == 1 )
			bSaved = TRUE;					// �߻� ���۸� �ִ� ���

		if( pMyGameInfo->_tDefData._nRadio > 0 )
			bSaved = TRUE;

		if( pMyGameInfo->_tDefData._nWeaponSlot != getWeaponContext()->getCurrentSlotNum() ) 
			bSaved = TRUE; // must send packet for Weapon change

		if( bSaved )
			pMyGameInfo->_nActionSendCount	= 1;
	}

	pMyGameInfo->_tDefData._nWeaponSlot		= (UINT8) getWeaponContext()->getCurrentSlotNum();

	if( getWeaponContext()->getCurrent() != NULL )
	{
		pMyGameInfo->_tDefData._nWeaponExtension = (UINT8) getWeaponContext()->getCurrent()->isExtensionActivate();
	}
	
	g_pGameContext->WriteMyKey( getKeyPressInput() );

	if( !isStateMask( CHARA_STATE_ONOBJECT) )
	{
		g_pGameContext->WriteMyCharaPos( GetPos(), m_Theta, m_Rho, FALSE, 0 );
	}

	UINT32 nSecFire = 0;
	if( getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE ))
		nSecFire = 1;

	UINT32 nHelmet = HELMET_STATE_NONE;
	if( g_pGameContext->IsP2PHost() )
	{	
		if( EQUIP_IsAttachedOnChara( EQUIP_CLASS_TYPE_HELMET ) )
			nHelmet = HELMET_STATE_EQUIP;
	}

	UINT32	nBody2;
	if( isStateMask( CHARA_STATE_ZOOM ) )
		nBody2 = CHARA_BODY_ZOOM;
	else
		nBody2 = CHARA_BODY_ATTACKIDLE;

	g_pGameContext->WriteMyCharaAction( getActionContext()->getBody(), nBody2, getActionContext()->getUpper(), getActionContext()->getLower(), nSecFire, nHelmet );
}
*/

// �ڽ��� ��Ʈ��ũ ���� ó���Դϴ�.
// Network Client���� Damage �� Death ���� üũ
/*
void CGameCharaLocal::ProcessNetworkMime( void)
{	// UDP �����͸� ó���մϴ�.
	if( g_pGameContext->IsP2PHost() )		return;

	INT32	idx = g_pGameContext->getMySlotIdx();
	GAMEINFO_RECV * pGameInfo	= g_pGameContext->getRecvGameInfo( idx );

	if( !pGameInfo->_bRecvFlag )			return;

	GAMEINFO_HIT *	pHitInfo	= &pGameInfo->_tHitData;
	GAMEINFO_FIRE * pFireInfo	= &pGameInfo->_tFireData;

	switch( pGameInfo->_RecvType )
	{
	case CHARA_PACKETTYPE_DEATH :
		if( (pGameInfo->_tDefData._nHP == 0) &&
			!getActionContext()->isLower( CHARA_LOWER_DEATH) &&
			pHitInfo->_nDeathType != CHARA_DEATH_UN )
		{	// �ڽ��� ���� ���
			if( pHitInfo->_nDeathType == CHARA_DEATH_B || pHitInfo->_nDeathType == CHARA_DEATH_O )
				i3Vector::Scale( &pHitInfo->_vHitDir, &pHitInfo->_vHitDir, -1.f );

			g_pCharaManager->DeathChara( idx, pHitInfo->_nShooter,
				&pHitInfo->_vHitDir, (CHARA_DEATH_TYPE) pHitInfo->_nDeathType, (CHARA_HIT_PART) pHitInfo->_nHitpart );
			
			g_pGameContext->setUserDeath( idx, TRUE );
			pGameInfo->_tHitData.Reset();
		}
		break;

	case CHARA_PACKETTYPE_HIT :
		// ����� ����ź �� ������Ʈ �ı��� ���� �������� ���ɴϴ�.
		if( (pHitInfo->_nHitpart != CHARA_HIT_UNKNOWN) &&
			m_pCharaInfo->_Hp != pGameInfo->_tDefData._nHP )
		{	// �ڽ��� ���� ���
			if( EQUIP_IsAttachedOnChara( EQUIP_CLASS_TYPE_HELMET ) && ((CHARA_HIT_PART) pHitInfo->_nHitpart == CHARA_HIT_HEAD 
				|| (pGameInfo->_tDefData._nHelmet == HELMET_STATE_DROP)))
			{	// �Ӹ��� �¾Ҿ��.. ��� ���
				EQUIP_DropToWorld( EQUIP_CLASS_TYPE_HELMET, &pHitInfo->_vHitDir );
				//	��信 �¾� �Ҳ��� Ƥ��.
				g_pEffectManager->AddEquipHitEffect( EQUIP_CLASS_TYPE_HELMET, WEAPON_CLASS_THROWING,
					i3Matrix::GetPos( getHeadMatrix()), &pHitInfo->_vHitDir, !isNetwork());
			}
			
			ACTION_Damage( (CHARA_HIT_PART) pHitInfo->_nHitpart, &pHitInfo->_vHitDir );
			// ��� �Ҹ�
			PlayCharaDamageSound((CHARA_HIT_PART) pHitInfo->_nHitpart, TRUE);
			
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_THROW, 0, GetPos(), true);

			pGameInfo->_tHitData.Reset();
		}
		break;

	case CHARA_PACKETTYPE_FIRENHIT :
		// �ڽ��� ���� ����Դϴ�.
		VEC3D vDir;
		for( INT32 i = 0; i < pFireInfo->_FireHitCount; i++ )
		{
			CGameCharaPlayer * pHitUser = g_pCharaManager->getCharaByNetIdx( pFireInfo->_FireHitWho[ i ] );

			if( pHitUser != NULL)
			{
				i3Vector::Sub( &pHitInfo->_vHitDir, pHitUser->GetPos(), GetPos() );
				i3Vector::Normalize( &pHitInfo->_vHitDir, &pHitInfo->_vHitDir );

				i3Vector::Scale( &vDir, &pHitInfo->_vHitDir, -1.f );

				// ��� ��� üũ
				if( (CHARA_HIT_PART) pFireInfo->_FireHitpart[ i ] == CHARA_HIT_HEAD &&
					pHitUser->EQUIP_IsAttachedOnChara( EQUIP_CLASS_TYPE_HELMET ) )
				{
					pHitUser->EQUIP_DropToWorld( EQUIP_CLASS_TYPE_HELMET, &vDir );

					//	��信 �¾� �Ҳ��� Ƥ��.
					if( getWeaponContext()->getCurrent() != NULL )
					{
						CWeaponInfo * pWeaponInfo = getWeaponContext()->getCurrent()->getWeaponInfo();
						g_pEffectManager->AddEquipHitEffect( EQUIP_CLASS_TYPE_HELMET, pWeaponInfo->m_TypeClass,
							i3Matrix::GetPos( pHitUser->getHeadMatrix()), &pGameInfo->_tHitData._vHitDir, pHitUser->is1PV());
					}
				}
				
				pHitUser->ACTION_Damage( (CHARA_HIT_PART) pFireInfo->_FireHitpart[ i ], &pHitInfo->_vHitDir );
				//	������ ������ ���Ҷ��� ��� �Ҹ��� ���� �Ѵ�.
				pHitUser->PlayCharaDamageSound( (CHARA_HIT_PART) pFireInfo->_FireHitpart[ i ] , TRUE);
			}
		}

		pFireInfo->_FireHitCount	= 0;
		break;
	}

	// ����ź
	GAMEINFO_GRENADE * pMyGameObj = &g_pGameContext->getSendGameInfo()->_tGrenadeData;
	if( pMyGameObj->_nThrowState == NET_GRENADE_THROW_SEND )
	{	// ���� ��Ŷ�� �� ���� ���� ��� �����ݴϴ�.
		if( getActionContext()->isUpper( CHARA_UPPER_FIRE ) )
		{
			g_pGameContext->getSendGameInfo()->_nFireSendCount = 1;
		}
	}
	else if( pMyGameObj->_nThrowState == NET_GRENADE_THROW_RECV )
	{ // �ڽ��� ���� �����ݴϴ�.
		VEC3D	vPos, vDir, normal;
		GetVirtualCamPosition( &vPos );
		i3Vector::Sub( &vPos, &vPos, GetPos());

		if( pGameInfo->_tDefData._bOnLoadObject )
		{
			// local to world matrix
			if( g_pGameContext->getObjectCountInfo()->m_pAnimObjectList->GetCount() > pGameInfo->_tDefData._nOnLoadObjectIdx )
			{
				i3Object * pObject = (i3Object*) g_pGameContext->getObjectCountInfo()->m_pAnimObjectList->GetItem( pGameInfo->_tDefData._nOnLoadObjectIdx );
				i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( pObject->GetSceneObject(), i3Transform2::GetClassMeta() );

				i3Vector::TransformCoord( &pFireInfo->_vStartPos, &pFireInfo->_vStartPos, pTrans->GetCacheMatrix() );
			}
		}
		
		i3Vector::Add( &vPos, &vPos, &pFireInfo->_vStartPos);
		i3Vector::Sub( &vDir, &pFireInfo->_vTargetPos, &vPos );
		i3Vector::Normalize( &normal, &vDir );
		
		MATRIX mat;
		i3Matrix::Identity( &mat);
		i3Matrix::SetRotateXDeg( &mat, 60.0f);
		i3Vector::TransformCoord( &vDir, &normal, &mat);
		
		i3Vector::Add( &vPos, &vPos, &vDir );
		WeaponBase * pWeapon = WEAPON_get( g_pGameContext->getSendGameInfo()->_tGrenadeData._nSlotIdx);
		I3ASSERT( pWeapon != NULL);
		pWeapon->NET_FirstThrow( &vPos, &vDir );
		g_pGameContext->getSendGameInfo()->_tGrenadeData._nThrowState	= NET_GRENADE_THROW_DONE;
	}

	{//Hit�Ȱ�� �̵��ӵ� �������� ȿ��
		if( m_pCharaInfo->_Hp > pGameInfo->_tDefData._nHP )
		{
			EnableSpeedReduction( (UINT8)m_pCharaInfo->_Hp - pGameInfo->_tDefData._nHP);
		}
	}
	
	m_pCharaInfo->_Hp		= pGameInfo->_tDefData._nHP;
	pGameInfo->_bRecvFlag	= FALSE;
	pGameInfo->_RecvType	= CHARA_PACKETTYPE_NONE;
}

*/

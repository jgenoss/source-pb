#include "pch.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaNetwork.h"

//#define NO_TEST_INSERT0						// �ӽ��ڵ� �ȱ� ���ۿ� ���� ó���� ���� �ʽ��ϴ�.

//#define NET_ASSIST_MINTIME			0.5f			// �ּ� ���� Ÿ�� �� �ð��� �ʰ��ϸ� �������� �ʴ´�.
//#define NET_ASSIST_MAXTIME			0.16f //0.1f//0.14f //0.18f //0.16f //0.14f //			// �� �ð� ������ ��ġ �̵��� �̷������.
//#define NET_ASSIST_MAXTIMEINV		6.25f //10.f //7.1428571428571428571428571428571f //5.555555555555555555555555f //6.25f //7.1428571428571428571428571428571f 

//#define NET_DIRECTION_ASSIST_VALUE	0.3826834323650897717284599840304f		// 0.7142857142857142857142857142857f
//#define NET_ASSIST_MOVE_MINLENGTH	I3_EPS

I3_CLASS_INSTANCE( CGameCharaNetwork, CGameCharaPlayer );

CGameCharaNetwork::CGameCharaNetwork( void)
{
	//m_pTeamTag = NULL;
	OnInitVariable();
}

CGameCharaNetwork::~CGameCharaNetwork( void)
{
	//I3_SAFE_RELEASE( m_pTeamTag);
}

void CGameCharaNetwork::OnInitVariable( void)
{
	CGameCharaPlayer::OnInitVariable();

	//I3_SAFE_RELEASE( m_pTeamTag);
	//m_CheckMoveType = GAME_CHARA_NETWORK_STAND;
	//m_rVelocity		= 0.f;
	//m_rAttackIdleTime	= 0.f;
	//m_bChangeDir	= FALSE;

	#if defined( I3_DEBUG )
		m_rDebugFireTimer = 0.f;
	#endif
}

void CGameCharaNetwork::Create( i3GameResChara * pRes, CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo)
{
	CGameCharaPlayer::Create( pRes, pCharaInfo, pDBInfo);
	//	Add code here
	/*
	if (g_pFramework->IsBattleStage())
	{
		if( g_pPlayer != NULL)
		{

			if( g_pPlayer->getCharaInfo()->_Team == pCharaInfo->_Team && g_pEnvSet->m_bEnableFrindlyMark)
			{				
				CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();

				if( m_pTeamTag)
				{
					m_pTeamTag->RemoveFromParent();
					I3_SAFE_RELEASE( m_pTeamTag);
				}

				m_pTeamTag = i3Quad::NewObject();
				m_pTeamTag->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
				m_pTeamTag->Create( 1, I3SG_QUAD_SPACE_WORLD, TRUE, FALSE, TRUE );
				m_pTeamTag->SetColor( 0, 255, 255, 255, 255 );
				m_pTeamTag->SetSize( 0, 0.8f, 0.8f );
				m_pTeamTag->SetCenter( 0, 0.0f, 2.2f, 0.0f );
				m_pTeamTag->SetEnable( 0, FALSE );
				m_pTeamTag->SetTexture( pStage->getHud()->getWepMarkTexture() );

				{
					i3LightingEnableAttr *pLightEnable = i3LightingEnableAttr::NewObjectRef();
					pLightEnable->Set( FALSE );
					m_pTeamTag->AddAttr( pLightEnable );

//					i3BlendModeAttr * pBlendModeAttr = i3BlendModeAttr::NewObjectRef();
//					pBlendModeAttr->SetSource( I3G_BLEND_SRCALPHA);
//					pBlendModeAttr->SetDestination( I3G_BLEND_ONE);
//					m_pTeamTag->AddAttr( pBlendModeAttr);			

					i3TextureFunctionAttr * pTexFuncAttr = i3TextureFunctionAttr::NewObjectRef();
					pTexFuncAttr->SetFunction( I3G_TEXTURE_FUNC_ADD);
					m_pTeamTag->AddAttr( pTexFuncAttr);

					i3ZWriteEnableAttr * pZWirteAttr = i3ZWriteEnableAttr::NewObjectRef();
					pZWirteAttr->Set( FALSE);
					m_pTeamTag->AddAttr( pZWirteAttr);
				}

				this->GetSceneObject()->AddChild( m_pTeamTag );
			}

		}
	}
	*/

	/// control�� Network������ �����ݴϴ�.	dfly 20080129
	// ���Ŀ� �Űܾ� �մϴ�.
	//ChangeAnimSet( CHARA_ANIMSET_3PV, FALSE, TRUE);
	Change3PV();
	ChangeNetwork();
}

void CGameCharaNetwork::ResetChara( void)
{
	CGameCharaPlayer::ResetChara();
	//	Add code here
}

void CGameCharaNetwork::OnUpdate( REAL32 tm)
{	
	REAL_PROFILE( "CGameCharaNetwork");

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	// ������ ó�� ������ �ſ� �߿��ϰ�, ������ ���� ���� �ʿ��ϴ�.

	if( g_pGameContext->IsP2PActivate() )	
	{
		//_ProcessNetworkInput( tm );

		CGameCharaPlayer::OnUpdate( tm);
	}
	else
	{
		CGameCharaPlayer::OnUpdate( tm);
		return;
	}

	/*

	// ������ Ÿ�ӿ� ���� ���� �ð� ���
	{
		if( m_rInvincibleElapseTime > 0.0f)				
		{	
			m_rInvincibleElapseTime -= tm;
			
			m_pMaterialContext->SetBlendEnable( TRUE);
		}
		else if( m_rInvincibleElapseTime < 0.0f)		
		{
			m_rInvincibleElapseTime = 0.0f;

			m_pMaterialContext->SetBlendEnable( FALSE);
		}
	}

	if( !isAlive())
	{
		_ProcessRespawn( tm);
	}

#if defined( I3_DEBUG)
	if( !g_pConfig->m_bNetworking )
	{	
		m_rDebugMoveTime += (tm * m_rDebugElp);
		if( m_rDebugMoveTime > 3.f || m_rDebugMoveTime < 0.f)
		{
			m_rDebugElp *= -1.f;
			m_rDebugMoveTime += (tm * m_rDebugElp);
			i3Vector::Scale( m_pMoveContext->getMoveDir(), m_pMoveContext->getMoveDir(), -1.f );
		}

		i3Vector::Lerp( &m_vDebugPos, &m_vDebugOriginalPos, &m_vDebugTargetPos, m_rDebugMoveTime );
		i3Vector::Sub( m_pMoveContext->getOldVelocity(), &m_vDebugTargetPos, &m_vDebugOriginalPos);
		//ACTION_MoveLower();

		if( i3Vector::IsSame( getMoveDir(), &I3_ZEROVECTOR) )
		{
			if(!STATE_IS_LOWER( CHARA_LOWER_IDLE))
				CALL_ACTION_COMMAND(ACTION_CMD_STEADYLOWER, NULL);
		}
		else
		{
			ActionParameter * param = GET_PREPARE_PARAM;
			i3Vector::Copy(&param->_dir, getMoveDir());

			CALL_ACTION_COMMAND(ACTION_CMD_RUNLOWER, param);
		}

		forceSetPos( &m_vDebugPos );
	}
#endif

	if( g_pGameContext->IsP2PHost() )
	{
		GAMEINFO_RECV * pGameInfo		= g_pGameContext->getRecvGameInfo( getCharaNetIndex() );
		pGameInfo->_tDefData._nHP		= (UINT8) m_pCharaInfo->_Hp;
	}
	*/
}

void CGameCharaNetwork::DumpChara( void)
{
	CGameCharaPlayer::DumpChara();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Process
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameCharaNetwork::ProcessFirst( REAL32 tm)
{	
	CGameCharaPlayer::ProcessFirst( tm);
	//	Add code here

	/*
	UINT32 keyStroke = g_pFramework->getKeyStroke();
	UINT32 keyPress = g_pFramework->getKeyPress();

	//	ĳ���� ���� ����
	if( keyStroke & GAME_KEY_CHANGEWEAPON6)		
	{			
		if( keyPress & GAME_KEY_ACTION_FOOTSTEP)	//	���� ����Ʈ
		{
			for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++ )
			{	//	��� ĳ�� ����
				CGameCharaPlayer * pChara = g_pCharaManager->getChara( i );
				pChara->DumpChara();	
			}
		}
		else
		{	//	�ڱ� ĳ���� ����
			DumpChara();				
		}
	}*/
}
/*
void CGameCharaNetwork::_ProcessNetworkInput( REAL32 rDeltatime )
{
	// ��Ʈ��ũ �Է�
	VEC3D	vLerp, vPos, vDir;
	WeaponBase *	pWeapon;
	CWeaponInfo *	pWeaponInfo;

	if( !isAlive() )		return;

#if defined( I3_DEBUG )
	if( !g_pConfig->m_bNetworking )
		return;
#endif

	INT32 idx = getCharaNetIndex();

	if( idx == -1 ) return;

	GAMEINFO_RECV *		pGameInfo	= g_pGameContext->getRecvGameInfo( idx );
	GAMEINFO_NORMAL *	pDefInfo	= &pGameInfo->_tDefData;
	GAMEINFO_FIRE *		pFireInfo	= &pGameInfo->_tFireData;
	GAMEINFO_ASSIST *	pAssist		= &pGameInfo->_tAssistData;

	m_bChangeDir = FALSE;

	pWeapon = getWeaponContext()->getCurrent();
	
	if( pGameInfo->_bRecvFlag == TRUE )
	{
		// ���� �ð����� ���� �������� �ٷ� Position�� �����Ѵ�.
		if( pAssist->_fRecvPosTime > NET_ASSIST_MINTIME )
			pAssist->_fRecvPosTime = NET_ASSIST_MAXTIME;
		else
			pAssist->_fRecvPosTime	= 0.f;

		if( pAssist->_fRecvThetaRhoTime	> NET_ASSIST_MINTIME )
			pAssist->_fRecvThetaRhoTime = NET_ASSIST_MAXTIME;
		else
			pAssist->_fRecvThetaRhoTime	= 0.f;

		m_CheckMoveType = _ProcessNetworkMoveDir( pAssist, &pDefInfo->_vPos, pDefInfo->_Lower, rDeltatime );

		if( pGameInfo->_RecvType == CHARA_PACKETTYPE_SYNC || pGameInfo->_RecvType == CHARA_PACKETTYPE_MISSION )
		{
			pAssist->_fPostTheta	= m_pBoneContext->getTheta();
			pAssist->_fRecvTheta	= pGameInfo->_tDefData._fTheta;
			pAssist->_fPostRho		= m_pBoneContext->getRhoValue();
			pAssist->_fRecvRho		= pGameInfo->_tDefData._fRho;

			if( pAssist->_fPostTheta < -I3_PI2 && pAssist->_fRecvTheta > I3_PI2 )
				pAssist->_fPostTheta += I3_2PI;
			else if( pAssist->_fPostTheta > I3_PI2 && pAssist->_fRecvTheta < -I3_PI2 )
				pAssist->_fPostTheta -= I3_2PI;

			if( pAssist->_fPostRho < -I3_PI2 && pAssist->_fRecvRho > I3_PI2 )
				pAssist->_fPostRho += I3_2PI;
			else if( pAssist->_fPostRho > I3_PI2 && pAssist->_fRecvRho < -I3_PI2 )
				pAssist->_fPostRho -= I3_2PI;

			if( pGameInfo->_RecvType == CHARA_PACKETTYPE_MISSION )
			{
				if( pGameInfo->_tMissionData._nState == NET_MISSION_TRIGGER_ARM_START )
				{
					// ���Ⱑ �´��� üũ�غ����մϴ�.
					if( getWeaponContext()->getCurrentSlotNum() != MAX_ITEMSLOT_COUNT - 1 )
						ACTION_SwapWeapon( pGameInfo->_tDefData._nWeaponSlot );

					pWeapon = getWeaponContext()->getCurrent();
					if( pWeapon != NULL )
					{
						pWeaponInfo = pWeapon->getWeaponInfo();

						if( pWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION )
						{// Fire !!
							if( !getActionContext()->isUpper( CHARA_UPPER_FIRE ) )
							{	
								if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE ) )
								{
									ACTION_GhostFire();
								}
							}

							pWeapon->Fire();
						}
					}
				}
			}
		}
		// ���� �߻� �� ��Ʈ ���
		else if( pGameInfo->_RecvType == CHARA_PACKETTYPE_FIRE ||
			pGameInfo->_RecvType == CHARA_PACKETTYPE_FIRENHIT )
		{	
			// VirtualCamera�� Y���� �����ɴϴ�.
			m_pCameraContext->getVirtualCamPosition( &vPos );
			i3Vector::Sub( &vPos, &vPos, GetPos());
			i3Vector::Add( &vPos, &vPos, &pAssist->_vRecvPosition );

			// Theta, Rho�� ���ϱ� ���� Direction ���
			i3Vector::Sub( &vDir, &pFireInfo->_vTargetPos, &vPos );
			i3Vector::Normalize( &vDir, &vDir );

			pAssist->_fPostTheta	= m_pBoneContext->getTheta();
			pAssist->_fPostRho		= m_pBoneContext->getRhoValue();

			REAL32	fX = getX( &vDir );
			REAL32	fY = getY( &vDir );
			REAL32	fZ = getZ( &vDir );

			pAssist->_fRecvTheta	= i3Math::atan2( fX, fZ );
			pAssist->_fRecvRho		= (I3_PI2) - i3Math::acos( fY );

			if( pAssist->_fPostTheta < -I3_PI2 && pAssist->_fRecvTheta > I3_PI2 )
				pAssist->_fPostTheta += I3_2PI;
			else if( pAssist->_fPostTheta > I3_PI2 && pAssist->_fRecvTheta < -I3_PI2 )
				pAssist->_fPostTheta -= I3_2PI;

			if( pAssist->_fPostRho < -I3_PI2 && pAssist->_fRecvRho > I3_PI2 )
				pAssist->_fPostRho += I3_2PI;
			else if( pAssist->_fPostRho > I3_PI2 && pAssist->_fRecvRho < -I3_PI2 )
				pAssist->_fPostRho -= I3_2PI;

			// ���� �߻�
			if( pGameInfo->_nFirePrvID < pFireInfo->_nID )
			{	
				// ���Ⱑ �´��� üũ�غ����մϴ�.
				getWeaponContext()->getControl()->WeaponValidCheck( (WEAPON_CLASS_TYPE) pFireInfo->_nWeaponType, pFireInfo->_nWeaponNum );

				// Random Sync
				g_pRand->SetCode( pFireInfo->_nRand );

				pWeapon = getWeaponContext()->getCurrent();
				if( pWeapon != NULL )
				{
					pWeaponInfo = pWeapon->getWeaponInfo();

					if( pWeaponInfo->m_TypeFire == WEAPON_FIRE_MELEE )				// Į.
					{
						// �ִϸ��̼��� �� �մϴ�. ������ �׼��� ���մϴ�.
						pWeapon->FireCollision( &vPos, &pFireInfo->_vTargetPos );	// �ø����� �����Ѵ�.
					}
					else if( pWeaponInfo->m_TypeFire == WEAPON_FIRE_THROWING )		// ����ź
					{
						// �ִϸ��̼��� �� �մϴ�. ������ �׼��� ���մϴ�.
						pWeapon->NET_Throw( &vPos, &pFireInfo->_vTargetPos );		// ���� ������ �ȴ�..
					}
					else
					{// Fire !!
						if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE ) )
						{
							if( !getActionContext()->isUpper( CHARA_UPPER_FIRE ) )
								ACTION_GhostFire();	// �߻� ����
						}

						pWeapon->NET_Fire( &vPos, &pFireInfo->_vTargetPos );

						if( pWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION )
						{
							if( !getActionContext()->isLower( CHARA_LOWER_IDLE ))
								CALL_ACTION_COMMAND( ACTION_CMD_STEADYLOWER, NULL);
								//ACTION_IdleLower();
						}
					}

					m_rAttackIdleTime = 0.5f;		// 0.5�ʰ� �߻� ������ �����մϴ�.
				}

				pGameInfo->_nFirePrvID = pFireInfo->_nID;
			}
		}

		pGameInfo->_bRecvFlag	= FALSE;
		pGameInfo->_RecvType	= CHARA_PACKETTYPE_NONE;
	}

	m_rAttackIdleTime -= rDeltatime;

	
	if( pDefInfo->_bOnLoadObject )
	{	// �ִϸ��̼��ϴ� ������Ʈ�� �˻��մϴ�.
		CONTEXT_OBJECT * pObjectCountInfo = g_pGameContext->getObjectCountInfo();

		if( pObjectCountInfo->m_pAnimObjectList->GetCount() > pDefInfo->_nOnLoadObjectIdx )
		{
			i3Object * pObj = (i3Object*) pObjectCountInfo->m_pAnimObjectList->GetItem( pDefInfo->_nOnLoadObjectIdx );
			
			i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( pObj->GetSceneObject(), i3Transform2::GetClassMeta() );
			if( pTrans != NULL )
			{
				if( pTrans->IsAnimated() )
				{	// �ִϸ��̼��ϴ� ������Ʈ�� ź ���
					// �ִϸ��̼ǵ� ������Ʈ�� ĳ���Ϳ��� �̵� ���� ����ؼ� ������ ��������� �մϴ�.
					I3MATRIXINFO Env;
					MATRIX	matTop, matObj;

					i3Node * pParent = pTrans->GetParent();
					i3Matrix::Identity( &matTop );
					i3Matrix::Copy( &matObj, pTrans->GetMatrix() );

					// �ش� �ִϸ��̼��� ����մϴ�. ���� ����� Matrix�� �����;� �մϴ�.
					while( pParent != NULL )
					{	
						if( pParent->IsTypeOf( i3Transform::GetClassMeta() ) )
						{
							i3Matrix::Mul( &matTop, &matTop, ((i3Transform*)pParent)->GetMatrix() );
						}

						pParent = pParent->GetParent();
					}

					Env.m_pIn	= &matObj;
					Env.m_pOut	= &matObj;
					Env.m_Time	= g_pViewer->getDeltaTime();
					Env.m_pModelViewMatrix = &matTop;
					pTrans->GetAnimatedMatrix( &Env );

					// �ִϸ��̼��� �ϰ� �ٽ� ���� Ÿ���� ���������ϴ�.
					pTrans->AddPlayTime( -g_pViewer->getDeltaTime() );

					i3Matrix::Mul( &matTop, &matObj, &matTop );

					VEC3D vNewPosDir;
					i3Vector::Sub( &vNewPosDir, i3Matrix::GetPos( &matTop ), i3Matrix::GetPos( pTrans->GetCacheMatrix() ) );

					i3Vector::Add( &pAssist->_vRecvPosition, &pAssist->_vRecvPosition, &vNewPosDir);
					i3Vector::Add( &pAssist->_vPostPosition, GetPos(), &vNewPosDir);
				}
			}
		}
	}
	
	// ���� Ÿ�̸�
	REAL32	flerp;

	pAssist->_fRecvThetaRhoTime += rDeltatime;
	flerp	= pAssist->_fRecvThetaRhoTime;
	if( flerp > NET_ASSIST_MAXTIME )
		flerp = NET_ASSIST_MAXTIME;
	flerp *= NET_ASSIST_MAXTIMEINV;

	//	Theta
	m_pBoneContext->setTheta( pAssist->_fPostTheta + ((pAssist->_fRecvTheta - pAssist->_fPostTheta) * flerp));
	//	Rho
	m_pBoneContext->setRho( pAssist->_fPostRho + ((pAssist->_fRecvRho - pAssist->_fPostRho) * flerp));

	// ĳ���� ��ġ ����(���������̼��մϴ�.)
	pAssist->_fRecvPosTime += rDeltatime;
	flerp = pAssist->_fRecvPosTime;
	if( flerp > NET_ASSIST_MAXTIME )
		flerp = NET_ASSIST_MAXTIME;
	flerp *= NET_ASSIST_MAXTIMEINV;

	// Ű���� �������� ĳ���͸� �ణ �� �������ݴϴ�
	VEC3D vTemp, vMoveDir;
	MATRIX mat;
	i3Matrix::Identity( &mat );
	i3Matrix::SetRotateY( &mat, m_pBoneContext->getTheta() );

	i3Vector::Zero( &vMoveDir );
	BOOL	bCheckKey = FALSE;
	
	i3Vector::Lerp( &vLerp, &pAssist->_vPostPosition, &pAssist->_vRecvPosition, flerp );

	if( getActionContext()->isLower( CHARA_LOWER_MOVE ) || getActionContext()->isLower( CHARA_LOWER_WALK ) )
	{
		if( (pGameInfo->_tDefData._nMoveKey == GAME_KEY_MOVE_LEFT) && (getX( m_pMoveContext->getMoveDir() ) == 1.f) )
		{
			addX( m_pMoveContext->getAccel(), m_pDBInfo->m_rSideMoveSpeed * rDeltatime * 0.2f);
			if( getX( m_pMoveContext->getAccel()) > m_pDBInfo->m_rSideMoveSpeed * rDeltatime * NET_ASSIST_MAXTIME)
			{
				setX( m_pMoveContext->getAccel(), m_pDBInfo->m_rSideMoveSpeed * rDeltatime * NET_ASSIST_MAXTIME);
			}
			bCheckKey = TRUE;
		}
		else if( pGameInfo->_tDefData._nMoveKey == GAME_KEY_MOVE_RIGHT )
		{
			addX( m_pMoveContext->getAccel(), -m_pDBInfo->m_rSideMoveSpeed * rDeltatime * 0.2f);
			if( getX( m_pMoveContext->getAccel()) < -m_pDBInfo->m_rSideMoveSpeed * rDeltatime * NET_ASSIST_MAXTIME)
			{
				setX( m_pMoveContext->getAccel(), -m_pDBInfo->m_rSideMoveSpeed * rDeltatime * NET_ASSIST_MAXTIME);
			}
			bCheckKey = TRUE;
		}
		else
		{
			setX( m_pMoveContext->getAccel(), 0.f);
		}

		if( pGameInfo->_tDefData._nMoveKey == GAME_KEY_MOVE_FORWARD )
		{
			addZ( m_pMoveContext->getAccel(), m_pDBInfo->m_rFrontMoveSpeed * rDeltatime * 0.2f);
			if( getZ( m_pMoveContext->getAccel()) > m_pDBInfo->m_rFrontMoveSpeed * rDeltatime * NET_ASSIST_MAXTIME)
			{
				setZ( m_pMoveContext->getAccel(), m_pDBInfo->m_rFrontMoveSpeed * rDeltatime * NET_ASSIST_MAXTIME);
			}
			bCheckKey = TRUE;
		}
		else if( pGameInfo->_tDefData._nMoveKey == GAME_KEY_MOVE_BACKWARD )
		{
			addZ( m_pMoveContext->getAccel(), -m_pDBInfo->m_rBackMoveSpeed * rDeltatime * 0.2f );
			if( getZ( m_pMoveContext->getAccel()) < -m_pDBInfo->m_rBackMoveSpeed * rDeltatime * NET_ASSIST_MAXTIME)
			{
				setZ( m_pMoveContext->getAccel(), -m_pDBInfo->m_rBackMoveSpeed * rDeltatime * NET_ASSIST_MAXTIME);
			}
			bCheckKey = TRUE;
		}
		else
		{
			setZ( m_pMoveContext->getAccel(), 0.f);
		}

		i3Vector::TransformCoord( &vTemp, m_pMoveContext->getAccel(), &mat );

		i3Vector::Add( &vLerp, &vLerp, &vTemp );
	}
	
	// ĳ���� ���ǵ� ���(��ü �ִϸ��̼ǿ� ���˴ϴ�)
	i3Vector::Sub( m_pMoveContext->getVelocity(), &vLerp, GetPos() );
	i3Vector::Copy( m_pMoveContext->getOldVelocity(), m_pMoveContext->getVelocity() );
	pAssist->_fLength = i3Vector::Length( m_pMoveContext->getVelocity() );
	INT32 ntemp = (INT32)(pAssist->_fLength * 100.f );
	pAssist->_fLength = (REAL32) ntemp * 0.01f;
	
	forceSetPos( &vLerp );
	
	//m_pMoveContext->ProcessFloorCheck( rDeltatime);

	//////////////////////////////////////////////////////////////////////////
	//						ĳ���� �׼� ���� & ���� �߻�					//

	// �ɾ�/�Ͼ
	switch( pDefInfo->_Body )
	{
	case CHARA_BODY_STAND :
		if( !getActionContext()->isBody( CHARA_BODY_STAND ) )
			ACTION_Stand();
		break;
	case CHARA_BODY_CROUCH :
		if( !getActionContext()->isBody( CHARA_BODY_CROUCH ) )
			ACTION_Crouch();
		break;
	}

	// Zoom
	switch( pDefInfo->_Body2 )
	{
	case CHARA_BODY_NONE :
		break;
	case CHARA_BODY_IDLE :
		if( !getActionContext()->isBody2( CHARA_BODY_IDLE ) )
			ACTION_ToggleIdle();
		break;
	case CHARA_BODY_ATTACKIDLE :
		if( !getActionContext()->isBody2( CHARA_BODY_ATTACKIDLE ) )
			ACTION_CancelZoom();
		break;
	case CHARA_BODY_ZOOM :
		if( !getActionContext()->isBody2( CHARA_BODY_ZOOM) )
			ACTION_ActiveZoom();
		break;

	default :
		NETTRACE( "CGameChara::ProcessNetworkInput() - Body2 invalid action. %d\n", pDefInfo->_Body2 );
		break;
	}

	
	switch( pDefInfo->_Upper )
	{
	case CHARA_UPPER_IDLE :
		if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE) )
		{
			if( getActionContext()->isUpper( CHARA_UPPER_JUMP) )
				CALL_ACTION_COMMAND(ACTION_CMD_JUMPUPPER_FINISH, NULL);
				//ACTION_JumpEndUpper();
			else if( !getActionContext()->isUpper( CHARA_UPPER_IDLE) && m_CheckMoveType == GAME_CHARA_NETWORK_STAND )
				CALL_ACTION_COMMAND( ACTION_CMD_STEADYUPPER, NULL);
				//ACTION_IdleUpper();
		}
		break;
	case CHARA_UPPER_LOADMAGAZINE :
		if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) && !getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON) )
			ACTION_LoadMagazine();
		break;
	case CHARA_UPPER_LOADBULLET :
		if( !getActionContext()->isUpper( CHARA_UPPER_LOADBULLET) )
			ACTION_LoadBullet();
		break;

	case CHARA_UPPER_FIRE_READY :
		if( pWeapon != NULL && pWeapon->getWeaponInfo()->m_TypeFire == WEAPON_FIRE_THROWING )
		{
			if( !getActionContext()->isUpper( CHARA_UPPER_FIRE_READY))
				ACTION_FireReady();
		}
		break;

	case CHARA_UPPER_FIRE :
		break;

	case CHARA_UPPER_MOVE :
		if( !getActionContext()->isUpper( CHARA_UPPER_MOVE) )
		{
			if( (m_CheckMoveType == GAME_CHARA_NETWORK_RUN) && getActionContext()->isBody( CHARA_BODY_STAND) && (m_rAttackIdleTime <= 0.f))
			{
				//ACTION_MoveUpper( NULL );
				ActionParameter * param = GET_PREPARE_PARAM;
				i3Vector::Copy(&param->_dir, getMoveDir());

				CALL_ACTION_COMMAND(ACTION_CMD_RUNUPPER, param);
			}
			//else if( !getActionContext()->isUpper( CHARA_UPPER_IDLE))
			//	ACTION_IdleUpper();
		}

		break;

	case CHARA_UPPER_NONE :
	case CHARA_UPPER_SWAPWEAPON :
	case CHARA_UPPER_SECONDARY_FIRE :
	case CHARA_UPPER_JUMP :
	case CHARA_UPPER_DAMAGE :
		break;
	case CHARA_UPPER_EXTENSION_ACTIVATE :
	case CHARA_UPPER_EXTENSION_DEACTIVATE :
		if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE) )
		{
			if( getActionContext()->isUpper( CHARA_UPPER_JUMP) )
				//ACTION_JumpEndUpper();
				CALL_ACTION_COMMAND(ACTION_CMD_JUMPUPPER_FINISH, NULL);
			else if( !getActionContext()->isUpper( CHARA_UPPER_IDLE) && m_CheckMoveType == GAME_CHARA_NETWORK_STAND )
				CALL_ACTION_COMMAND( ACTION_CMD_STEADYUPPER, NULL);
				//ACTION_IdleUpper();
		}
		break;
	default :
		I3TRACE( "CGameChara::processNetworkInput() - Upper invalid action. %d\n", pDefInfo->_Upper );
		break;
	}

	if( pDefInfo->_nWeaponExtension == 1)
	{
		if( pWeapon != NULL && !pWeapon->isExtensionActivate() )
		{
			pWeapon->setExtensionActivate( TRUE );
			pWeapon->NET_ExtensionActivate( TRUE, FALSE);
			pWeapon->SetVisibleExtension( TRUE);
		}
	}
	else if( pDefInfo->_nWeaponExtension == 0)
	{
		if( pWeapon != NULL && pWeapon->isExtensionActivate() )
		{
			pWeapon->setExtensionActivate( FALSE );
			pWeapon->NET_ExtensionActivate( FALSE, FALSE);
			pWeapon->SetVisibleExtension( FALSE);
		}
	}

	// ���� ��ȯ
	if( pDefInfo->_nWeaponSlot != getWeaponContext()->getCurrentSlotNum() )
		ACTION_SwapWeapon( (INT32) pDefInfo->_nWeaponSlot );

	switch( pDefInfo->_Lower )
	{
	case CHARA_LOWER_IDLE :			// ����
		if( getActionContext()->isLower( CHARA_LOWER_JUMP ) )
		{	// ���� ���°� �����̸�
			//ACTION_JumpEndLower();
			CALL_ACTION_COMMAND(ACTION_CMD_JUMPLOWER_FINISH, NULL);
			g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, getMoveContext()->getFloorTerrainType(), GTBT_JUMP_END, GetPos(), is1PV());
		}
		break;
	case CHARA_LOWER_JUMP :			// jump
		if( !getActionContext()->isLower( CHARA_LOWER_JUMP ) )
		{
			ACTION_Jump();
		}
		break;
	case CHARA_LOWER_WALK :
	case CHARA_LOWER_MOVE :
		if( getActionContext()->isLower( CHARA_LOWER_JUMP ) )
		{	// ���� ���°� �����̸�
			//ACTION_JumpEndLower();
			CALL_ACTION_COMMAND(ACTION_CMD_JUMPLOWER_FINISH, NULL);
			g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, getMoveContext()->getFloorTerrainType(), GTBT_JUMP_END, GetPos(), is1PV());
		}
		break;
	case CHARA_LOWER_NONE :
	case CHARA_LOWER_DEATH :
		break;
	default :
		I3TRACE( "CGameChara::processNetworkInput() - Lower invalid action. %d\n", pDefInfo->_Lower );
		break;
	}

	// �̵� ��ü
	if( !getActionContext()->isLower( CHARA_LOWER_JUMP ) )
	{	// ���� ���̸� ��ŵ
		if( m_CheckMoveType == GAME_CHARA_NETWORK_RUN )
		{
			if( m_bChangeDir || ((m_rVelocity != pAssist->_fLength) ) )
			{	// ������ �ٲ�ų� �̵� �ӵ��� �ٸ��� �ִϸ��̼��� �ٽ� �մϴ�.( ���ο��� �����ϸ� ��)
				//ACTION_MoveLower();
				
				if( i3Vector::IsSame( getMoveDir(), &I3_ZEROVECTOR) )
				{
					if(!STATE_IS_LOWER( CHARA_LOWER_IDLE))
						CALL_ACTION_COMMAND(ACTION_CMD_STEADYLOWER, NULL);
				}
				else
				{
					ActionParameter * param = GET_PREPARE_PARAM;
					i3Vector::Copy(&param->_dir, getMoveDir());

					CALL_ACTION_COMMAND(ACTION_CMD_RUNLOWER, param);
				}

				m_rVelocity = pAssist->_fLength;
			}
		}
		else if( m_CheckMoveType == GAME_CHARA_NETWORK_WALK )
		{
			if( m_bChangeDir || ((m_rVelocity != pAssist->_fLength)) )
			{	// ������ �ٲ�ų� �̵� �ӵ��� �ٸ��� �ִϸ��̼��� �ٽ� �մϴ�.( ���ο��� �����ϸ� ��)
				//ACTION_WalkLower();
				ActionParameter * param = GET_PREPARE_PARAM;
				i3Vector::Copy(&param->_dir, getMoveDir());

				CALL_ACTION_COMMAND(ACTION_CMD_WALKLOWER, param);

				m_rVelocity = pAssist->_fLength;
			}
		}
		else if( !getActionContext()->isLower( CHARA_LOWER_IDLE ) )
		{
			//ACTION_IdleLower();
			CALL_ACTION_COMMAND( ACTION_CMD_STEADYLOWER, NULL);
		}
	}
}


void CGameCharaNetwork::SetCharaMark( CWeaponInfo * pWeaponInfo, BOOL bEnable)
{

	if( m_pTeamTag != NULL)
	{
		m_pTeamTag->SetEnable( 0, bEnable);

		if( bEnable)
		{
			// ���⿡ ���� UV����
			switch( pWeaponInfo->m_TypeClass)
			{
			case WEAPON_CLASS_SHOTGUN:
				m_pTeamTag->SetTextureCoord( 0, 0.0f, 0.0f, 0.5f, 0.5f );				
				break;
			case WEAPON_CLASS_SNIPER:
				m_pTeamTag->SetTextureCoord( 0, 0.5f, 0.0f, 1.0f, 0.5f );
				break;
			case WEAPON_CLASS_MG:
			case WEAPON_CLASS_ASSAULT:
				m_pTeamTag->SetTextureCoord( 0, 0.0f, 0.5f, 0.5f, 1.0f );
				break;
			case WEAPON_CLASS_SMG:
				m_pTeamTag->SetTextureCoord( 0, 0.5f, 0.5f, 1.0f, 1.0f );
				break;
			default:
				m_pTeamTag->SetEnable( 0, FALSE);
				break;
			}		
		}	
	}

}

GAME_CHARA_NETWORK_MOVE_TYPE CGameCharaNetwork::_ProcessNetworkMoveDir( GAMEINFO_ASSIST * pAssist, VEC3D * pRecvPos, UINT32 nLower, REAL32 rDeltatime )
{	
	BOOL	bCheckMove = FALSE;
	MATRIX	mat;
	GAME_CHARA_NETWORK_MOVE_TYPE	nCheckMove = GAME_CHARA_NETWORK_STAND;

	if( getCharaNetIndex() == -1 )
		return nCheckMove;

	if( g_pGameContext->getUserDeath( getCharaNetIndex() ) == TRUE )
		return nCheckMove;
	
	m_bChangeDir = FALSE;

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	GAMEINFO_RECV * pRecvGameInfo = g_pGameContext->getRecvGameInfo( getCharaNetIndex() );

	i3Matrix::Identity( &mat);

	if( pRecvGameInfo->_tDefData._bOnLoadObject )
	{
		// local to world matrix
		if( g_pGameContext->getObjectCountInfo()->m_pAnimObjectList->GetCount() > pRecvGameInfo->_tDefData._nOnLoadObjectIdx )
		{
			i3Object * pObject = (i3Object*) g_pGameContext->getObjectCountInfo()->m_pAnimObjectList->GetItem( pRecvGameInfo->_tDefData._nOnLoadObjectIdx );
			i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( pObject->GetSceneObject(), i3Transform2::GetClassMeta() );

			i3Vector::TransformCoord( &pAssist->_vRecvPosition, pRecvPos, pTrans->GetCacheMatrix() );
			i3Vector::Copy( &pAssist->_vPostPosition, GetPos() );
			i3Vector::Sub( &pAssist->_vRecvDir, &pAssist->_vRecvPosition, GetPos() );
		}
		else
		{
			return nCheckMove;
		}
	}
	else
	{
		i3Vector::Sub( &pAssist->_vRecvDir, pRecvPos, &pAssist->_vRecvPosition );
		
		i3Vector::Copy( &pAssist->_vRecvPosition, pRecvPos );
		i3Vector::Copy( &pAssist->_vPostPosition, GetPos() );
	}
	
	i3Vector::Copy( m_pMoveContext->getVelocity(), &pAssist->_vRecvDir );
	pAssist->_fLength = i3Vector::Normalize( &pAssist->_vRecvDir, &pAssist->_vRecvDir );

	pAssist->_fLength *= NET_ASSIST_MAXTIMEINV;

	if( pAssist->_fLength > 60.f )
	{	// ���� �̵� �����ݴϴ�.
		i3Vector::Copy( &pAssist->_vPostPosition, &pAssist->_vRecvPosition );
		pAssist->_fLength = 0.f;
		forceSetPos( &pAssist->_vPostPosition );
		return nCheckMove;
	}
	else if( pAssist->_fLength > NET_ASSIST_MOVE_MINLENGTH )
	{	// ��� �� ������ �����մϴ�.
		REAL32	rDestVel = 0.f;
		i3Matrix::SetRotateY( &mat, -pAssist->_fRecvTheta );
		i3Vector::TransformNormal( &pAssist->_vPostDir, &pAssist->_vRecvDir, &mat );

		if( (getZ( &pAssist->_vPostDir ) > NET_DIRECTION_ASSIST_VALUE))
		{
			if(getZ( m_pMoveContext->getMoveDir() ) != 1.f)
				m_bChangeDir = TRUE;

			i3Vector::SetZ( m_pMoveContext->getMoveDir(), 1.f );
			rDestVel = m_pDBInfo->m_rWalkFrontMove;
			bCheckMove =TRUE;
		}
		else if( (getZ( &pAssist->_vPostDir ) < -NET_DIRECTION_ASSIST_VALUE))
		{
			if(getZ( m_pMoveContext->getMoveDir()) != -1.f)
				m_bChangeDir = TRUE;
			i3Vector::SetZ( m_pMoveContext->getMoveDir(), -1.f );
			rDestVel = m_pDBInfo->m_rWalkBackMove;
			bCheckMove = TRUE;
		}
		else
		{
			if( getZ( m_pMoveContext->getMoveDir()) != 0.f )
				m_bChangeDir = TRUE;
			i3Vector::SetZ( m_pMoveContext->getMoveDir(), 0.f );
		}

		if( (getX( &pAssist->_vPostDir ) > NET_DIRECTION_ASSIST_VALUE))
		{
			if(getX( m_pMoveContext->getMoveDir()) != 1.f)
				m_bChangeDir = TRUE;
			i3Vector::SetX( m_pMoveContext->getMoveDir(), 1.f );
			if( bCheckMove )
				rDestVel = m_pDBInfo->m_rWalkDiagonalMove;
			else
			{
				rDestVel = m_pDBInfo->m_rWalkSideMove;
				bCheckMove = TRUE;
			}
		}
		else if( (getX( &pAssist->_vPostDir ) < -NET_DIRECTION_ASSIST_VALUE))
		{
			if(getX( m_pMoveContext->getMoveDir()) != -1.f)
				m_bChangeDir = TRUE;
			i3Vector::SetX( m_pMoveContext->getMoveDir(), -1.f );
			if( bCheckMove )
				rDestVel = m_pDBInfo->m_rWalkDiagonalMove;
			else
			{
				rDestVel = m_pDBInfo->m_rWalkSideMove;
				bCheckMove = TRUE;
			}
		}
		else
		{
			if( getX( m_pMoveContext->getMoveDir()) != 0.f )
				m_bChangeDir = TRUE;
			i3Vector::SetX( m_pMoveContext->getMoveDir(), 0.f );
		}
		
		if( bCheckMove )
		{
			if( (pWeapon != NULL) && (pWeapon->getWeaponInfo() != NULL))
			{
				CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

				if( getActionContext()->isBody( CHARA_BODY_STAND ))
					rDestVel *= (pWeaponInfo->m_rMoveNormalStand ) * 0.01f;
				else if( getActionContext()->isBody( CHARA_BODY_CROUCH))		//	�⺻ �ɾ��ִ� ���¿���
					rDestVel *= (pWeaponInfo->m_rMoveNormalSit ) * 0.01f;

				if( isStateMask( CHARA_STATE_ZOOM))
				{
					if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) && !getActionContext()->isUpper( CHARA_UPPER_LOADBULLET))	//	���� �������� �����ÿ��� ����ȵǰ�...
						rDestVel *= pWeaponInfo->m_rMoveNormalZoom * 0.01f;
				}
			}
			
			if( getActionContext()->isBody( CHARA_BODY_STAND ))
			{	
				if( (nLower == CHARA_LOWER_WALK) )
					nCheckMove = GAME_CHARA_NETWORK_WALK;
				else if( nLower == CHARA_LOWER_MOVE )
					nCheckMove	= GAME_CHARA_NETWORK_RUN;
				else if( pAssist->_fLength > I3_EPS)
					nCheckMove = GAME_CHARA_NETWORK_WALK;
			}
			else
			{
				if( nLower == CHARA_LOWER_MOVE )
					nCheckMove = GAME_CHARA_NETWORK_RUN;
				else if( nLower == CHARA_LOWER_WALK )
					nCheckMove = GAME_CHARA_NETWORK_WALK;
				else if( pAssist->_fLength > I3_EPS)
					nCheckMove = GAME_CHARA_NETWORK_WALK;
			}
		}
	}
	else
	{
		i3Vector::Zero( m_pMoveContext->getMoveDir() );
		nCheckMove = GAME_CHARA_NETWORK_STAND;
	}

	return nCheckMove;
}
*/


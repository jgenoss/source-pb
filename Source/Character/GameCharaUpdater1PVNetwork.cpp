#include "pch.h"
#include "GameCharaUpdater1PVNetwork.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaActionContext.h"
#include "./Character/GameCharaActionControlDefine.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaCollisionContext.h"
#include "GameCharaMaterialContext.h"

I3_CLASS_INSTANCE( CGameCharaUpdater1PVNetwork, CGameCharaUpdater1PV);

CGameCharaUpdater1PVNetwork::CGameCharaUpdater1PVNetwork()
{
}

CGameCharaUpdater1PVNetwork::~CGameCharaUpdater1PVNetwork()
{
}

void CGameCharaUpdater1PVNetwork::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater1PV::OnUpdate( rDeltatime);

	if( m_pOwner == NULL)						return;
	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !g_pGameContext->IsP2PActivate()  )	return;

	//	1��Ī�� ��쿡 ��ü aistate�ǰ��� ȣ����� �����Ƿ� �ʿ��Ѱ�� ���� ȣ�����ֵ��� �ϴ� �Լ�
	OnProcess1PVOnly( rDeltatime);

	getBoneContext()->UpdateAnim( rDeltatime);					// Animation�� Key ���� ���� (�� Bone�� Cache Matrix�� ����)

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		// �ѱ� �߻翡 ���� Theta ����� ��Ʈ��ũ������ ���� �ʾƾ� �մϴ�.
		getBoneContext()->CalcThetaNetwork( rDeltatime);

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		// ���Ͽ����� getSceneObject()�� Matrix�� ����Ǿ �ȵȴ�.
		// ���� ����ȴٸ�, Bone ��꿡�� ����� ��ŭ�� ������ �� �� �ִ�.
		//

		// SceneObject, AxisRotate, IK, Spine1, Spine2, Spine3 ���� World-Space Matrix�� ���
		// 

		getBoneContext()->_CalcSpineBones();

		//	Rho 
		getBoneContext()->CalcRecoil( rDeltatime);					// ������
		getBoneContext()->CalcRho( rDeltatime);						// IK Bone ( IK Cache Matrix)
	}
	else
		ProcessDeathState( rDeltatime);

	getBoneContext()->TransformWorldSpace( rDeltatime);
	getMaterialContext()->DriveLightVolume( rDeltatime);
	getCameraContext()->OnUpdate( rDeltatime);				// Camera Update

	//	ĳ���� �𵨸� ���� ó��
	getMaterialContext()->ProcessMaterial( rDeltatime);

	//	������� HUD ȭ�� ����Ʈ 
	//m_pOwner->ProcessScreenEffect( rDeltatime);

	if( g_pCharaManager->getCharaTask( m_pOwner->getCharaNetIndex()) == CHARA_TASK_RESPAWN)	//	���� ������ �����̸�
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		if( ((CStageBattle*)pStage)->getStageState() == GAMESTAGE_STATE_BATTLE )
			OnProcessInvincible( rDeltatime);
	}
	else
	{	//	ĳ���Ͱ� �׾�����
		ProcessRespawn( rDeltatime);
	}
}


void CGameCharaUpdater1PVNetwork::OnLastUpdate( REAL32 rDeltatime)
{
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Send Network info
	if( g_pGameContext->IsP2PHost() )
		m_pOwner->WriteData_HP();
}

void CGameCharaUpdater1PVNetwork::OnProcessInvincible( REAL32 rDeltatime)
{
	//	������ ���� �����ð��� ����� �ش�.
	REAL32 rInvincible = m_pOwner->getInvincibleTime();

	if( rInvincible > 0.f )
	{
		rInvincible -= rDeltatime;
		m_pOwner->setInvincibleTime( rInvincible);

		getMaterialContext()->SetBlendEnable( true);
	}		
	else if( rInvincible < 0.f )
	{	
		m_pOwner->setInvincibleTime( 0.f);

		getMaterialContext()->SetBlendEnable( false);
	}
}

void CGameCharaUpdater1PVNetwork::OnProcess1PVOnly( REAL32 rDeltatime)
{
	if( getActionContext()->isLower( CHARA_LOWER_JUMP) )
	{
		getAnimContext()->CallbackAnim_JumpStartRun( NULL, rDeltatime);
	}
	else if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK))
	{
		getAnimContext()->CallbackAnim_MoveRun( NULL, rDeltatime);
	}
}



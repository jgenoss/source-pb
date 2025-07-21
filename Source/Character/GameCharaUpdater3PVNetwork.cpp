#include "pch.h"
#include "GameCharaUpdater3PVNetwork.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater3PVNetwork, CGameCharaUpdater3PV);

CGameCharaUpdater3PVNetwork::CGameCharaUpdater3PVNetwork()
{
}

CGameCharaUpdater3PVNetwork::~CGameCharaUpdater3PVNetwork()
{
}

void CGameCharaUpdater3PVNetwork::OnFirstUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater3PV::OnFirstUpdate( rDeltatime);

	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !g_pGameContext->IsP2PActivate() )		return;

	// Animation�� World-Space Transform ���̿� LOD ����� �޶��� �� ������,
	// �װ��� Visual �� ���� ������ �� �� �ֱ� ������
	// �������� LOD�� ����Ų��.
	//	Zoom�� FOV ���� ���
	REAL32 fRatio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();

	//	�𵨸� LOD ���
	m_pOwner->calcLOD( m_pOwner->GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()), fRatio);

	getBoneContext()->UpdateAnim( rDeltatime);				// Animation�� Key ���� ���� (�� Bone�� Cache Matrix�� ����)

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		// �ѱ� �߻翡 ���� Theta ����� ��Ʈ��ũ������ ���� �ʾƾ� �մϴ�.
		getBoneContext()->CalcThetaNetwork( rDeltatime);
		
		// ��, ��ü �� ������.
		getBoneContext()->CalcAimBone( rDeltatime);

		// �Ӹ��� ���� IK
		
		getBoneContext()->CalcHeadBone( rDeltatime);
		
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		// ���Ͽ����� getSceneObject()�� Matrix�� ����Ǿ �ȵȴ�.
		// ���� ����ȴٸ�, Bone ��꿡�� ����� ��ŭ�� ������ �� �� �ִ�.
		//

		// SceneObject, AxisRotate, IK, Spine1, Spine2, Spine3 ���� World-Space Matrix�� ���
		// 

		getBoneContext()->_CalcSpineBones();

		//	Rho 
		getBoneContext()->CalcRecoil( rDeltatime);				// ������
		// ��ü�� ������.
		getBoneContext()->CalcRho3PV( rDeltatime);
	}

	getBoneContext()->TransformWorldSpace( rDeltatime);
}

void CGameCharaUpdater3PVNetwork::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater3PV::OnUpdate( rDeltatime);
	//	Add code here
	if( m_pOwner == NULL)						return;
	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !g_pGameContext->IsP2PActivate() )		return;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		ProcessDeathState( rDeltatime);

	getMaterialContext()->DriveLightVolume( rDeltatime);
	getCameraContext()->OnUpdate( rDeltatime);				// Camera update

	//	ĳ���� �𵨸� ���� ó��
	getMaterialContext()->ProcessMaterial( rDeltatime);

	//	������� HUD ȭ�� ����Ʈ 
	//m_pOwner->ProcessScreenEffect( rDeltatime);

	if( g_pCharaManager->getCharaTask( m_pOwner->getCharaNetIndex()) == CHARA_TASK_RESPAWN)	//	���� ������ �����̸�
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		if( ((CStageBattle*)pStage)->getStageState() == GAMESTAGE_STATE_BATTLE)
			OnProcessInvincible( rDeltatime);
	}
	else
	{	//	ĳ���Ͱ� �׾�����
		ProcessRespawn( rDeltatime);
	}
}

void CGameCharaUpdater3PVNetwork::OnLastUpdate( REAL32 rDeltatime)
{
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Send Network info
	if( g_pGameContext->IsP2PHost() )
		m_pOwner->WriteData_HP();

#if defined( I3_DEBUG)	//DebugProcess
	 DebugProcess( rDeltatime);
#endif
}

void CGameCharaUpdater3PVNetwork::OnProcessInvincible( REAL32 rDeltatime)
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




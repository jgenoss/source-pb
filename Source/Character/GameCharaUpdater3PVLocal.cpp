#include "pch.h"
#include "GameCharaUpdater3PVLocal.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater3PVLocal, CGameCharaUpdater3PV);

CGameCharaUpdater3PVLocal::CGameCharaUpdater3PVLocal()
{
}

CGameCharaUpdater3PVLocal::~CGameCharaUpdater3PVLocal()
{
}

void CGameCharaUpdater3PVLocal::OnFirstUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater3PV::OnFirstUpdate( rDeltatime);

	if( g_pFramework->IsBattleStage())
	{	
		// Local�� ��Ŷ ó��
		ProcessLocalFromRecvPacket();

		getMoveContext()->Move( rDeltatime);
		getMoveContext()->UpdateSpeedReduction( rDeltatime);
	}

	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !g_pGameContext->IsP2PActivate()  )	return;
	
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
		getBoneContext()->CalcTheta( rDeltatime);				// Position

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
		getBoneContext()->CalcRho3PV( rDeltatime);
	}

	getBoneContext()->TransformWorldSpace( rDeltatime);
}

void CGameCharaUpdater3PVLocal::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater3PV::OnUpdate( rDeltatime);
	//	Add code here
	if( m_pOwner == NULL)						return;
	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !g_pGameContext->IsP2PActivate()  )	return;
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		ProcessDeathState( rDeltatime);
	}

	getMaterialContext()->DriveLightVolume( rDeltatime);
	getCameraContext()->OnUpdate( rDeltatime);

	//	ĳ���� �𵨸� ���� ó��
	getMaterialContext()->ProcessMaterial( rDeltatime);

	//	������� HUD ȭ�� ����Ʈ 
	m_pOwner->ProcessScreenEffect( rDeltatime);

	if( g_pCharaManager->getCharaTask( m_pOwner->getCharaNetIndex()) == CHARA_TASK_RESPAWN)	//	���� ������ �����̸�
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		if( ((CStageBattle*)pStage)->getStageState() == GAMESTAGE_STATE_BATTLE)
			OnProcessInvincible( rDeltatime);
		else
		{
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEFAULT ) )
			{	
				m_pOwner->Cmd_Reset();

				m_pOwner->addCharaStateMask( CHARA_STATE_DEFAULT);
				m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_CONTROL);
			}
		}
	}
	else
	{	//	ĳ���Ͱ� �׾�����
		ProcessRespawn( rDeltatime);
	}

	// RPG7 ���� ���� �÷���
	CheckGrazeSoundPlay();
}

void CGameCharaUpdater3PVLocal::OnLastUpdate( REAL32 rDeltatime)
{
	// Local�� �����͸� ��Ʈ��ũ�� �����ϴ�.
	m_pOwner->WriteData_DefaultInfo();
}

void CGameCharaUpdater3PVLocal::OnProcessInvincible( REAL32 rDeltatime)
{
	REAL32 rInvincible = m_pOwner->getInvincibleTime();

	if( g_pFramework->IsBattleStage() == false)
		return;

	//	������ ���� �����ð��� ����� �ش�.
	if( rInvincible > 0.0f)
	{
		rInvincible -= rDeltatime;
		m_pOwner->setInvincibleTime( rInvincible);
		
		//	�������� ���� �����ð� ������ Ű�Է��� Ǯ���ش�. (���� ��ü GUI���� Ű�Է��� �������Ƿ�)
		REAL32 InvTime = g_pGameContext->getMissionManager()->GetInvincibleTime( g_pGameContext->getMyRoomInfo()->_StageID);

		////-----------------	�����ð� +1sec
		//	����ڰ� ������̸� �����ð� ����. (�ڽ� ���)
		if( m_pOwner && g_pGameContext->isUseCashItemInSlot( m_pOwner->getCharaNetIndex(), SLOT_ITEM_INCREASE_INVINCIBLE_TIME))
		{
			//	�����ð� ����
			InvTime += (REAL32) VALUE_CASH_ITEM_INCREASE_INVINCIBLE_TIME;
		}

		if( rInvincible < InvTime - 0.5f)
			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);

		getMaterialContext()->SetBlendEnable( true);
	}		
	else if( rInvincible < 0.0f)		
	{	
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);

		m_pOwner->setInvincibleTime( 0.f);

		getMaterialContext()->SetBlendEnable( false);
	}
}



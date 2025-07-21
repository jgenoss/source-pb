#include "pch.h"
#include "GameCharaUpdater1PVLocal.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater1PVLocal, CGameCharaUpdater1PV);

CGameCharaUpdater1PVLocal::CGameCharaUpdater1PVLocal()
{
}

CGameCharaUpdater1PVLocal::~CGameCharaUpdater1PVLocal()
{
}

void CGameCharaUpdater1PVLocal::OnFirstUpdate( REAL32 rDeltatime)
{	
	// Local�� ��Ŷ ó��
	ProcessLocalFromRecvPacket();
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		return;

	// �̵� ó��
	getMoveContext()->Move( rDeltatime);
	getMoveContext()->UpdateSpeedReduction( rDeltatime);

	getBoneContext()->UpdateAnim( rDeltatime);		// Animation�� Key ���� ���� (�� Bone�� Cache Matrix�� ����)

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		getBoneContext()->CalcTheta( rDeltatime);
		
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		// ���Ͽ����� getSceneObject()�� Matrix�� ����Ǿ �ȵȴ�.
		// ���� ����ȴٸ�, Bone ��꿡�� ����� ��ŭ�� ������ �� �� �ִ�.
		//
		//i3Matrix::Mul( m_pOwner->GetMatrix(), m_pOwner->getLocalMatrix(), m_pOwner->getIdentityMatrix());

		// SceneObject, AxisRotate, IK, Spine1, Spine2, Spine3 ���� World-Space Matrix�� ���
		// 
		getBoneContext()->_CalcSpineBones();

		//	Rho 
		getBoneContext()->CalcRecoil( rDeltatime);		// ������
		getBoneContext()->CalcRho( rDeltatime);			// IK Bone ( IK Cache Matrix)
	}

	getBoneContext()->TransformWorldSpace( rDeltatime);
}

void CGameCharaUpdater1PVLocal::OnUpdate( REAL32 rDeltatime)
{	
	//	Add code here
	//	1��Ī�� ��쿡 ��ü aistate�ǰ��� ȣ����� �����Ƿ� �ʿ��Ѱ�� ���� ȣ�����ֵ��� �ϴ� �Լ�
	OnProcess1PVOnly( rDeltatime);
	
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

void CGameCharaUpdater1PVLocal::OnLastUpdate( REAL32 rDeltatime)
{	
	// Local�� �����͸� ��Ʈ��ũ�� �����ϴ�.
	m_pOwner->WriteData_DefaultInfo();
}

void CGameCharaUpdater1PVLocal::OnProcessInvincible( REAL32 rDeltatime)
{
	REAL32 rInvincible = m_pOwner->getInvincibleTime();

	if( g_pFramework->IsBattleStage() == false)
		return;
	
#if !defined( DEF_OBSERVER_MODE)
	if(g_pGameContext->IsGM_Observer()) 
			g_pPlayer->Suicide();
#endif			

	//	������ ���� �����ð��� ����� �ش�.
	if( rInvincible > 0.0f)
	{
		rInvincible -= rDeltatime;
		m_pOwner->setInvincibleTime( rInvincible);
		
		REAL32 InvTime = g_pGameContext->getMissionManager()->GetInvincibleTime( g_pGameContext->getMyRoomInfo()->_StageID);

		////-----------------	�����ð� +1sec
		//	����ڰ� ������̸� �����ð� ����. (�ڽ� ���)
		if( m_pOwner && g_pGameContext->isUseCashItemInSlot( m_pOwner->getCharaNetIndex(), SLOT_ITEM_INCREASE_INVINCIBLE_TIME))
		{
			//	�����ð� ����
			InvTime += (REAL32) VALUE_CASH_ITEM_INCREASE_INVINCIBLE_TIME;
		}

		//	�������� ���� �����ð� ������ Ű�Է��� Ǯ���ش�. (���� ��ü GUI���� Ű�Է��� �������Ƿ�)
		if( rInvincible < InvTime - 0.5f)
			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);

		getMaterialContext()->SetBlendEnable( true);
	}		
	else if( rInvincible < 0.0f)		
	{	
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);

		rInvincible -= rDeltatime;
		m_pOwner->setInvincibleTime( rInvincible);

		getMaterialContext()->SetBlendEnable( false);
		
	

		if(rInvincible < -0.5f) {
			
			m_pOwner->setInvincibleTime( -0.5f);
		}
	}
}

void CGameCharaUpdater1PVLocal::OnProcess1PVOnly( REAL32 rDeltatime)
{
	if( getActionContext()->isLower( CHARA_LOWER_JUMP) ||
		!m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) || m_pOwner->isCharaStateMask( CHARA_STATE_JUMP))
	{
		getAnimContext()->CallbackAnim_JumpStartRun( NULL, rDeltatime);
	}
	else if( m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR))
	{
		if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK))
		{
			getAnimContext()->CallbackAnim_MoveRun( NULL, rDeltatime);
		}
	}
}






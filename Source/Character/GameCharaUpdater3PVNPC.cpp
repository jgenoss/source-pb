#include "pch.h"
#include "GameCharaUpdater3PVNPC.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater3PVNPC);//, CGameCharaUpdater3PV );

CGameCharaUpdater3PVNPC::CGameCharaUpdater3PVNPC()
{
}

CGameCharaUpdater3PVNPC::~CGameCharaUpdater3PVNPC()
{
}


void CGameCharaUpdater3PVNPC::ProcessRespawn( REAL32 rDeltatime)
{
	//	 ������ Ÿ�̸� üũ	
	REAL32 rRespawnElapseTime = m_pOwner->getRespawnElapseTime();

	if( rRespawnElapseTime > 0.0f)
	{	
		m_pOwner->getMaterialContext()->SetBlendEnable( true);

		rRespawnElapseTime -= rDeltatime;
		m_pOwner->setRespawnElapseTime( rRespawnElapseTime);

		//	�������� �ȴ�.
		if( rRespawnElapseTime <= 0.0f)
		{
			m_pOwner->getMaterialContext()->SetBlendEnable( false);
			m_pOwner->setRespawnElapseTime( 0.f);
		}
	}
}

void CGameCharaUpdater3PVNPC::OnFirstUpdate( REAL32 rDeltatime)
{
	if( !g_pGameContext->IsP2PActivate() )	return;

	// Animation�� World-Space Transform ���̿� LOD ����� �޶��� �� ������,
	// �װ��� Visual �� ���� ������ �� �� �ֱ� ������
	// �������� LOD�� ����Ų��.
	//	Zoom�� FOV ���� ���
	REAL32 fRatio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();

	//	�𵨸� LOD ���
	m_pOwner->calcLOD( m_pOwner->GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()), fRatio);

	getBoneContext()->UpdateAnim( rDeltatime);				// Animation�� Key ���� ���� (�� Bone�� Cache Matrix�� ����)

	getBoneContext()->TransformWorldSpace( rDeltatime);

}

void CGameCharaUpdater3PVNPC::OnUpdate( REAL32 rDeltatime)
{
	if( m_pOwner == NULL )		return;

	CGameCharaUpdater3PV::OnUpdate( rDeltatime);
	//	Add code here

	if( !g_pGameContext->IsP2PActivate() )	return;

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		m_pOwner->ProcessTimer( rDeltatime);
	else
		ProcessRespawn( rDeltatime);

	getMaterialContext()->DriveLightVolume( rDeltatime);
	//	ĳ���� �𵨸� ���� ó��
	//getMaterialContext()->ProcessMaterial( rDeltatime);

	
	REAL32 fAlpha = 1.0f;

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	{
		//////////////////////////////////////////////////////////////////////
		//	������ Ÿ��
		if( m_pOwner->getRespawnElapseTime() > 0.0f)	
		{
			//	���� ĳ���ʹ� ������ ������� ȿ��
			I3ASSERT( m_pOwner->getDBInfo()->GetNPCRespawnTime() > I3_EPS );

			fAlpha = m_pOwner->getRespawnElapseTime() / m_pOwner->getDBInfo()->GetNPCRespawnTime();

			m_pOwner->Cmd_SetColor( NULL, NULL, fAlpha);
		}
		else
		{
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
			{
				//////////////////////////////////////////////////////////////////////////////
				//	���� Ÿ��
				if( m_pOwner->getInvincibleTime() > 0.0f)
				{
					//	�����ð��ÿ��� �����̴� ȿ��		      
					fAlpha = i3Math::abs( i3Math::sin( m_pOwner->getInvincibleTime() * 6.0f));

					m_pOwner->Cmd_SetColor( NULL, NULL, 1.f - fAlpha);
				}
				else
				{
					//	������ �ƴҶ� ���� ���·� ����
					m_pOwner->Cmd_SetColor( NULL, NULL, 1.f);
				}
			}
		}
	}
}

void CGameCharaUpdater3PVNPC::OnProcessInvincible( REAL32 rDeltatime)
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





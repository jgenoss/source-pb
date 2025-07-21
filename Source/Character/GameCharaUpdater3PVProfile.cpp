#include "pch.h"
#include "GameCharaUpdater3PVProfile.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater3PVProfile);

void CGameCharaUpdater3PVProfile::OnFirstUpdate( REAL32 rDeltatime)
{
	//if( m_pOwner->is3PV())
	//{
	//	// Animation�� World-Space Transform ���̿� LOD ����� �޶��� �� ������,
	//	// �װ��� Visual �� ���� ������ �� �� �ֱ� ������
	//	// �������� LOD�� ����Ų��.
	//	//	Zoom�� FOV ���� ���
	//	REAL32 fRatio = 1.f;
	//	VEC3D CamPos;
	//	
	//	if( g_pCamera != nullptr)
	//	{
	//		fRatio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();
	//		i3Vector::Copy( &CamPos, i3Matrix::GetPos( g_pCamera->getCamMatrix()));
	//	}
	//	else
	//	{
	//		i3Vector::Set( &CamPos, 0.f, 0.5f, 1.f);
	//	}


	//	//	�𵨸� LOD ���
	//	m_pOwner->calcLOD( m_pOwner->GetPos(), &CamPos, fRatio);
	//}

	getBoneContext()->UpdateAnim( rDeltatime);				// Animation�� Key ���� ���� (�� Bone�� Cache Matrix�� ����)
	getBoneContext()->_CalcTheta( rDeltatime);				// Position
	getBoneContext()->_CalcSpineBones();
}

void CGameCharaUpdater3PVProfile::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater3PV::OnUpdate( rDeltatime);
		
	//	ĳ���� �𵨸� ���� ó��
	getMaterialContext()->ProcessMaterial( rDeltatime);
}


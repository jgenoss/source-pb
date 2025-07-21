#include "pch.h"

#ifdef	DOMI_OBJECT

#include "WeaponDominationObject.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaActionContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"
#include "WeaponFunction.h"

I3_CLASS_INSTANCE( WeaponDominationObject)//, WeaponBase);

WeaponDominationObject::WeaponDominationObject(void) : m_rInstallElapsedTime(0.0f)
						, m_rUninstallElpasedTime(0.0f)
						, m_rExplosionElapsedTime(0.0f)
						, m_rTracerUpdateTime(0.0f)
						, m_bDisarm(false)
						, m_bEnableActive(false)
{
	
}

WeaponDominationObject::~WeaponDominationObject(void)
{
	
}

void WeaponDominationObject::_OnInitVairable( void)
{	
	WeaponBase::_OnInitVairable();

	m_rInstallElapsedTime	= 0.f;				// ��ġ Ÿ��
	m_rUninstallElpasedTime	= 0.f;				// ��ü Ÿ��	
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;

	m_bDisarm				= false;
	m_bEnableActive			= false;


	i3Matrix::Identity( &m_mtxOld);

	
}

void WeaponDominationObject::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}

void WeaponDominationObject::Reset()
{
	WeaponBase::Reset();		//	���� �ؾ���.

	SetMatrix( &m_mtxOld);

	m_StateMaskWeapon		= WSM_LOADBULLET_DISABLE | WSM_LOADMAG_DISABLE;

	m_rInstallElapsedTime	= 0.f;	
	m_rUninstallElpasedTime	= 0.f;	
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;

	m_bDisarm				= false;
	m_bEnableActive			= false;


}


void WeaponDominationObject::PostCreate()
{
	i3Matrix::Identity( &m_mtxOld);
}

void WeaponDominationObject::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	m_rInstallElapsedTime = 0.f;
}
void WeaponDominationObject::OnUpdate( REAL32 rDeltaSeconds)
{	
	WeaponBase::OnUpdate( rDeltaSeconds);

	if(this->getWeaponInfo()->GetNumber() == GetItemIndex("MISSION_OBJECT3"))
	{
		this->OnSetBlendAlpha(0.5);
	}
}

//void WeaponDominationObject::Fire(void)
//{
//	if( getLoadedBulletCount() <= 0)
//		return;
//
//	// ������ ���ϰ� �ִ� ��ǥ ������ ���Ѵ�.
//	NET_Fire( nullptr, nullptr );
//}

void WeaponDominationObject::FireEnd(void)
{	
	WeaponBase::FireEnd();

	m_rInstallElapsedTime = 0.f;
}

void WeaponDominationObject::ActiveDisarm( bool bEnable)
{
	if( m_bEnableActive)	//	��ġ�� ���¸� ����
	{
		if( bEnable)
		{
	
			m_bDisarm = true;
		}
		else
		{
			m_bDisarm = false;
			m_rUninstallElpasedTime = 0.f;
		}
	}
}

void WeaponDominationObject::ReturnToReady( void)
{	
	WeaponBase::ReturnToReady();

	m_rInstallElapsedTime	= 0.f;	
	m_rUninstallElpasedTime	= 0.f;	
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;

}


void WeaponDominationObject::Uninstall( void)
{

	m_bEnableActive = false;

	//	�ݵ�� ���� �Ŵ����� ���� �����ؾ� �Ѵ�.
	g_pWeaponManager->PushDropWeaponToChara( this );
	//g_pFramework->InvisableDebugObj(false);


}
void WeaponDominationObject::InstallToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat, REAL32 theta)
{
	VEC3D vWeaponPos = VEC3D( pPos->x, pPos->y, pPos->z);
	
	if( getOwner() == nullptr )
		return;
	


	//���к����̼ǿ� C4�� �ε��� 19��~20�� �Դϴ�.
	if (this->getWeaponInfo()->GetNumber() > 18)
	{
		//	������Ʈ�� ���� ȸ��. �ùٸ� ������ �ٶ� ������. ĳ���Ͱ� ��� �����ô� �ٽ� �����ؾ� �Ѵ�.
		i3Matrix::PostRotateAxisDeg( GetMatrix(), &I3_XAXIS, -90.f);
		i3Matrix::PostRotateAxisDeg( GetMatrix(), &I3_YAXIS, 90.f);

		i3Vector::AddY( &vWeaponPos, 0.65f);	//�ٴڿ��� �������� ��ź ��ġ �ø�...
	}
	else	
	{
		SetMatrix( getOwner()->GetMatrix());

		//	������Ʈ�� ���� ȸ��. �ùٸ� ������ �ٶ� ������. ĳ���Ͱ� ��� �����ô� �ٽ� �����ؾ� �Ѵ�.
		i3Matrix::PreRotateAxisDeg( GetMatrix(), &I3_XAXIS, -90.f);
		i3Matrix::PostRotateAxisDeg( GetMatrix(), &I3_YAXIS, 0);
		
		i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetUp( GetMatrix()), 0.01f);
		i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetRight( GetMatrix()), 0.08f);	// hight 5cm
		i3Vector::ScaleAdd( pPos, i3Matrix::GetAt( GetMatrix()), 1.1f);
	}
		

	
	
	g_pSndMng->StartWeapon_C4StateSnd( GTFW_C4_INSTALLATION, pPos, false, false);

	I3TRACE( "--- ȸ�������� ��ġ �Ϸ� (Pos %.4f %.4f %.4f).\n", pPos->x, pPos->y, pPos->z);
	//	C4�� ����(ParentGameNode)�� C4(GameNode)�� �پ�� �մϴ�. (Owner ĳ���Ͱ� ������� �ص� OnUpdate�� ȣ���ؾ� �ϱ� ������)
	WeaponBase::Transmission( pNewSceneNode, pNewOwner, &vWeaponPos, true);


//	g_pFramework->SetPosDebugObj(0,vWeaponPos);
//	g_pFramework->InvisableDebugObj(true);


	
}

void WeaponDominationObject::ReturnInstance( void)
{	
	m_bEnableActive = false;

	Reset();

	//	��� ���� (�������� ���ӳ�忡�� ���ŵȴ�.)
	WeaponBase::ReturnInstance();

}


void WeaponDominationObject::_OnCollision(CGameCharaBase * pChara, i3CollideeLine * pLine, I3_PHYSIX_HIT_RESULT * pResult)
{	
	_OnHit( pResult, pLine, false);
}

#endif
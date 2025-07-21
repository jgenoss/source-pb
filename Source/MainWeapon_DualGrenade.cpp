#include "pch.h"
#include "MainWeapon_DualGrenade.h"

#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaWeaponContext.h"

//�տ��� �������� �ð� (�ִϸ��̼������� ���޹��� ��ġ�Դϴ�.)
#define GRENADE_DETACHHAND_TIME_1	0.4f		// 12 frame
#define GRENADE_DETACHHAND_TIME_2	1.233f		// 37 frame


MainWeapon_DualGrenade::MainWeapon_DualGrenade(WeaponBase* p) : MainWeapon_Grenade(p)
{
	m_bCheckThrowFirst	= false;

	m_pLeftGrenade = nullptr;

	m_TypeGrenade = GRENADE_RIGHT;		// default ������

	for(INT32 i=0; i< GRENADE_MAX; i++)
	{
		m_aiiGrenadeIdle[i] = -1;
		m_aiiGrenadeFire[i] = -1;
	}
}

void	MainWeapon_DualGrenade::OnCreate( bool bCreateInWeapon)
{
	MainWeapon_Grenade::OnCreate( bCreateInWeapon);
	
	// ������(����) ����ź�� ��� ���̿� ���� ����ź�� �����ؼ� ��� �ִ´�.
	//if( m_TypeGrenade == GRENADE_RIGHT)
	if( bCreateInWeapon == false)
	{
		m_pLeftGrenade = WeaponDualGrenade::new_object();
		I3ASSERT( m_pLeftGrenade != nullptr);

		// ���ȣ�� �Ǹ鼭 ���� �����Ǵ� ���� �������� Create�ϱ� ���� �����Ѵ�.
		
		m_pLeftGrenade->PreCreateMainWeaponBeforeCreateCall();

		MainWeapon_DualGrenade* mainWeapon_left = static_cast<MainWeapon_DualGrenade*>(m_pLeftGrenade->GetMainWeapon());
		mainWeapon_left->setGrenadeType( GRENADE_LEFT);	

		m_pLeftGrenade->Create( m_pWeaponBase->getOwner(), 
			m_pWeaponBase->getWeaponInfo()->GetItemID(), true	);
	}

}

void MainWeapon_DualGrenade::OnBindResource_DualGrenade()
{
	i3AIContext* pAICtx = m_pWeaponBase->GetAIContext();

	m_aiiGrenadeIdle[ MainWeapon_DualGrenade::GRENADE_LEFT] = (INT8) pAICtx->FindAIStateByName( "Idle_Left");
	m_aiiGrenadeIdle[ MainWeapon_DualGrenade::GRENADE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "Idle_Right");

	m_aiiGrenadeFire[ MainWeapon_DualGrenade::GRENADE_LEFT] = (INT8) pAICtx->FindAIStateByName( "Fire_Left");
	m_aiiGrenadeFire[ MainWeapon_DualGrenade::GRENADE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "Fire_Right");

	I3ASSERT( m_aiiGrenadeIdle[ MainWeapon_DualGrenade::GRENADE_LEFT] != -1);
	I3ASSERT( m_aiiGrenadeIdle[ MainWeapon_DualGrenade::GRENADE_RIGHT] != -1);	

	I3ASSERT( m_aiiGrenadeFire[ MainWeapon_DualGrenade::GRENADE_LEFT] != -1);	
	I3ASSERT( m_aiiGrenadeFire[ MainWeapon_DualGrenade::GRENADE_RIGHT] != -1);	
	
}

void MainWeapon_DualGrenade::SetOwner_LeftGrenade(CGameCharaBase* pOwner)
{
	if (m_pLeftGrenade)
	{
		m_pLeftGrenade->SetOwner(pOwner);
	}
}

void MainWeapon_DualGrenade::ReturnInstance_LeftGrenade()
{
	if( m_pLeftGrenade)	
	{
		m_pLeftGrenade->getAttachNode()->RemoveAllChild();

		m_pLeftGrenade->RemoveAllChild();	

		m_pLeftGrenade->DetachNode();		//	GameNode , SceneNode ����

		I3_SAFE_RELEASE( m_pLeftGrenade);
	}

}

void	MainWeapon_DualGrenade::OnUpdate_Grenade( REAL32 rDeltaSeconds)
{
	MainWeapon_Grenade::OnUpdate_Grenade(rDeltaSeconds);
	
	if( m_pWeaponBase->getOwner())
	{
		// ���� ����ź
		if( !m_bCheckThrowFirst && m_TypeGrenade == GRENADE_LEFT && 
			m_pWeaponBase->getOwner()->getActionContext()->isUpper( CHARA_UPPER_FIRE_READY))
		{		
			REAL32 fUpperAnimLocalTime = m_pWeaponBase->getOwner()->getAnimContext()->getAIUpper()->getLocalTime();

			// ĳ������ ������ �غ� ���ۿ� ���� �����ʿ� 2�� ����ź�� LeftGrenade ���� �޼����� �ű��.
			if( fUpperAnimLocalTime > 0.25f)
			{
				this->AttachToLeftHand();
			}
		}
	}
}

void MainWeapon_DualGrenade::AttachToLeftHand(void)
{
	if( m_pWeaponBase->getOwner())	// �޼տ� ���δ�.
	{
		//	WeaponBase �Լ��� ����Ͽ� this�� ����ǰ� �ϸ� WeaponDualGrenade::AttachNode�� ȣ����� �ʵ��� �Ѵ�.
		// AttachNode�� �����Լ��ε�..WeaponBase������ ������ ������Ų�� ���� �����̴�.
		// ������, �̷� ������ ���� �ʴ�.
		m_pWeaponBase->WeaponBase::AttachNode( m_pWeaponBase->getOwner()->getWeaponContext()->getDummyLeftHand(), m_pWeaponBase->getOwner());
	}
}

void	MainWeapon_DualGrenade::Reset()
{
	MainWeapon_Grenade::Reset();
	
	if(m_TypeGrenade == GRENADE_RIGHT)	//	������ �Ǵ� ������ ����ź�� ȣ��� ��� m_pLeftGrenade�� �Բ� ����
	{
		m_pLeftGrenade->SetVisible( true);

		MainWeapon_DualGrenade* mainWeapon_Left = static_cast<MainWeapon_DualGrenade*>(m_pLeftGrenade->GetMainWeapon());
		
		mainWeapon_Left->PlayIdleAnim();
	}
}

void	MainWeapon_DualGrenade::ReturnToReady()
{
	MainWeapon_Grenade::ReturnToReady();

	if(m_TypeGrenade == GRENADE_RIGHT)	//	������ �Ǵ� ������ ����ź�� ȣ��� ��� m_pLeftGrenade�� �Բ� ����
	{
		MainWeapon_DualGrenade* mainWeapon_Left = static_cast<MainWeapon_DualGrenade*>(m_pLeftGrenade->GetMainWeapon());
	
		mainWeapon_Left->PlayIdleAnim();
	}

	this->PlayIdleAnim();	
}

void MainWeapon_DualGrenade::PlayIdleAnim( void)
{
	m_pWeaponBase->setAIIndex( WEAPON::AI_IDLE, m_aiiGrenadeIdle[ m_TypeGrenade]);

	static_cast<WeaponGrenade*>(m_pWeaponBase)->WeaponGrenade::_OnPlayWeaponIdleAnim();
}

void MainWeapon_DualGrenade::PlayFireAnim( void)
{
	m_pWeaponBase->setAIIndex( WEAPON::AI_FIRE, m_aiiGrenadeFire[ m_TypeGrenade]);

	static_cast<WeaponGrenade*>(m_pWeaponBase)->WeaponGrenade::_OnPlayWeaponFireAnim();
}

void	MainWeapon_DualGrenade::FireReady()
{
	if( m_TypeGrenade == GRENADE_RIGHT)	//	������ �Ǵ� ������ ����ź�� ȣ��� ��� m_pLeftGrenade�� �Բ� ����
	{
		MainWeapon_DualGrenade* mainWeapon_Left = static_cast<MainWeapon_DualGrenade*>(m_pLeftGrenade->GetMainWeapon());
		mainWeapon_Left->PlayFireAnim();	// ���� ����ź�� �غ� ���ۿ� �÷���
	}
}

void	MainWeapon_DualGrenade::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	MainWeapon_Grenade::OnEndFire(pCtx, tm);

	m_bCheckThrowFirst = false;

	// ������ ���� ����ź�� �����ִٸ� �𵨸��� ���ڸ��� �̵�
	if( m_pWeaponBase->getLoadedBulletCount() > 0)
	{
		if( m_TypeGrenade == GRENADE_RIGHT)	//	������ �Ǵ� ������ ����ź�� ȣ��� ��� m_pLeftGrenade�� �Բ� ����
		{
			m_pLeftGrenade->SetVisible( true);

			MainWeapon_DualGrenade* mainWeapon_Left = static_cast<MainWeapon_DualGrenade*>(m_pLeftGrenade->GetMainWeapon());
			mainWeapon_Left->PlayIdleAnim();
			mainWeapon_Left->AttachToRightHand();
		}

		m_pWeaponBase->SetVisible( true);
		this->PlayIdleAnim();
		this->AttachToRightHand();
	}

}

void	MainWeapon_DualGrenade::AttachToRightHand(void)
{
	if( m_pWeaponBase->getOwner())	//	�����տ� ���δ�.
	{
		//	WeaponBase �Լ��� ����Ͽ� this�� ����ǰ� �ϸ� WeaponDualGrenade::AttachNode�� ȣ����� �ʵ��� �Ѵ�.
		m_pWeaponBase->WeaponBase::AttachNode( m_pWeaponBase->getOwner()->getWeaponContext()->getDummyRightHand(), m_pWeaponBase->getOwner());	// Ŭ���������� ���� �ȵȴ�.
	}

}

void	MainWeapon_DualGrenade::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner )
{
	// OnAttachWeapon ������ ������ �Ǵ� ������ ����ź�� AttachNode�� ȣ���ϱ� ������ ���� ����ź�� ���� ���� 	
	if( m_TypeGrenade == GRENADE_RIGHT)
	{
		if( m_pLeftGrenade)
			m_pLeftGrenade->AttachNode( pSceneNode, pOwner);
	}
}

void	MainWeapon_DualGrenade::DetachNode()
{
	// OnDetachWeapon ������ ������ �Ǵ� ������ ����ź�� AttachNode�� ȣ���ϱ� ������ ���� ����ź�� ���� ���� 
	if( m_TypeGrenade == GRENADE_RIGHT)
	{
		if( m_pLeftGrenade)
			m_pLeftGrenade->DetachNode();
	}
}

void	MainWeapon_DualGrenade::OnSetBlendAlpha( REAL32 val)
{
	if(m_pLeftGrenade)
		m_pLeftGrenade->OnSetBlendAlpha( val);
}

void	MainWeapon_DualGrenade::OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	if(m_pLeftGrenade)
		m_pLeftGrenade->OnSetMaterialColor( pfDiffuse, pfSpecular);
}

bool MainWeapon_DualGrenade::_CheckThrow(REAL32 LocalTime)
{
	// ���� üũ�� ����(������) ����ź������ �Ѵ�. (������ m_pLeftGrenade �� ������ ��)
	if( m_TypeGrenade == GRENADE_LEFT)
		return false;

	// �����պ��� ������.
	if(!m_bCheckThrowFirst && LocalTime > GRENADE_DETACHHAND_TIME_1)
	{
		if( !m_pWeaponBase->getOwner()->isNetwork() )// ��Ʈ��ũ�� ��Ŷ���� ó���մϴ�.
		{
			//�տ��� �������� �ð�
			// ������ ���ϰ� �ִ� ��ǥ ������ ���Ѵ�.
			VEC3D target;
			m_pWeaponBase->GetShotTargetPos( &target);

			m_pWeaponBase->NET_Throw( nullptr, &target, -1 );

			m_pWeaponBase->SetVisible( false);
		}

		m_bCheckThrowFirst = true;
	}
	//	���� �޼� ������.
	else if (LocalTime > GRENADE_DETACHHAND_TIME_2)
	{
		if( !m_pWeaponBase->getOwner()->isNetwork() )// ��Ʈ��ũ�� ��Ŷ���� ó���մϴ�.
		{
			//�տ��� �������� �ð�
			// ������ ���ϰ� �ִ� ��ǥ ������ ���Ѵ�.
			VEC3D target;
			m_pWeaponBase->GetShotTargetPos( &target);

			m_pWeaponBase->NET_Throw( nullptr, &target, -1 );

			m_pLeftGrenade->SetVisible( false);
		}

		return true;
	}

	return false;
}


#include "pch.h"
#include "GameCharaActionContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaInputContext.h"
#include "GameCharaMoveContext.h"
#include "GameCharaWeaponContext.h"

#include "BattleServerContext.h"

#include "./Character/GameCharaInputControlLocal.h"
#include "./Character/GameCharaInputControlNetwork.h"
#include "./Character/GameCharaInputControl.h"
#include "./Character/GameCharaInputControlAI.h"
#include "./Character/InputCtrlAI_ForHuman.h"
#include "./Character/InputCtrlAI_ForDino.h"
#include "./Character/GameCharaInputControlFullbody.h"

#include "Character/Fullbody/GameCharaDomiSting.h"


#include "MainWeapon_HandGrenade.h"
#include "MainWeapon_DualCIC.h"
#include "MainWeapon_GrenadeShell.h"

#if defined( I3_DEBUG)
//#define _AI_PLAYER
#endif

I3_CLASS_INSTANCE( CGameCharaInputContext);

CGameCharaInputContext::~CGameCharaInputContext()
{
	OnDestroy();
}

void CGameCharaInputContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	m_pLocalInputControl = CGameCharaInputControlLocal::new_object();
	m_pLocalInputControl->BindOwner( pOwner);

	m_pNetworkInputControl = CGameCharaInputControlNetwork::new_object();
	m_pNetworkInputControl->BindOwner( pOwner);

	m_pFullbodyInputControl = CGameCharaInputControlFullbody::new_object();
	m_pFullbodyInputControl->BindOwner(pOwner);
	m_pFullbodyInputControl->SetNetworkInputControl(m_pNetworkInputControl);

	//AI ������ ������ ������ ��� m_pAIInputControl �� �ٽ� �����ϱ� ���ؼ� �ּ� ó�� ����
	//if(pOwner->isAI())
	{
		if(pOwner->IsDino())
		{
			m_pAI_DinoInputControl	= CInputCtrlAI_ForDino::new_object();
			m_pAI_DinoInputControl->BindOwner( pOwner);
		}
		else
		{
			m_pAIInputControl	= CInputCtrlAI_ForHuman::new_object();
			m_pAIInputControl->BindOwner( pOwner);
		}
	}

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);
}

void CGameCharaInputContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();

	m_pCurrentInputControl = nullptr;
	I3_SAFE_RELEASE( m_pLocalInputControl );
	I3_SAFE_RELEASE( m_pFullbodyInputControl);
	I3_SAFE_RELEASE( m_pNetworkInputControl );
	I3_SAFE_RELEASE( m_pAIInputControl);
	I3_SAFE_RELEASE( m_pAI_DinoInputControl);
}

void CGameCharaInputContext::OnReset( void)
{
	CGameCharaContextBase::OnReset();

	m_CheckMoveType = GAME_CHARA_NETWORK_STAND;
	m_KeyPressInput		= 0;
	m_KeyStrokeInput	= 0;

	if( m_pCurrentInputControl != nullptr && m_pCurrentInputControl == m_pAIInputControl)
	{
		m_pAIInputControl->OnResetAI();
	}
}

void CGameCharaInputContext::OnChangeLocal()
{
	#if defined( _AI_PLAYER)
		m_pCurrentInputControl = m_pAIInputControl;
	#else
		m_pCurrentInputControl = m_pLocalInputControl;
	#endif
}

void CGameCharaInputContext::OnChangeNetwork()
{
	m_pCurrentInputControl = m_pNetworkInputControl;
}

void CGameCharaInputContext::OnChangeNPC( void)
{
	
}

void CGameCharaInputContext::OnChangeAI_Dino( void)
{
	m_pCurrentInputControl = m_pAI_DinoInputControl;
}

void CGameCharaInputContext::OnChangeAI( void)
{
	m_pCurrentInputControl = m_pAIInputControl;
}

void CGameCharaInputContext::OnChangeFullbody()
{
	m_pCurrentInputControl = m_pFullbodyInputControl;
}

void CGameCharaInputContext::OnFirstUpdate( REAL32 rDeltatime)
{
	CGameCharaContextBase::OnFirstUpdate( rDeltatime);

	if( m_pCurrentInputControl == nullptr)
		return;

	m_pCurrentInputControl->OnFirstUpdate( rDeltatime);
}

void CGameCharaInputContext::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaContextBase::OnUpdate( rDeltatime);

	if( m_pCurrentInputControl == nullptr)
		return;

	m_pCurrentInputControl->OnUpdate( rDeltatime);
}

CGameCharaInputControl * CGameCharaInputContext::getControl( void)
{
	return m_pCurrentInputControl;
}

void CGameCharaInputContext::ProcessActionUpper( CHARA_ACTION_UPPER upper )
{
	m_pNetworkInputControl->ProcessNetworkActionUpper( upper);
}

void CGameCharaInputContext::ProcessActionBody( CHARA_ACTION_BODYLOWER body)
{
	m_pNetworkInputControl->ProcessNetworkActionBody( body);
}

void CGameCharaInputContext::ProcessActionBody2( CHARA_ACTION_BODYUPPER body2)
{
	m_pNetworkInputControl->ProcessNetworkActionBody2( body2);
}

void CGameCharaInputContext::ProcessPosition( GAMEINFO_CHARA_NETDATA * pRecvInfo)
{
	m_pNetworkInputControl->ProcessNetworkPosition( pRecvInfo);
}

void CGameCharaInputContext::ProcessRotationTheta( GAMEINFO_CHARA_NETDATA * pRecvInfo)
{
	m_pNetworkInputControl->ProcessNetworkRotationTheta( pRecvInfo);
}

void CGameCharaInputContext::ProcessRotationRho( GAMEINFO_CHARA_NETDATA * pRecvInfo)
{
	m_pNetworkInputControl->ProcessNetworkRotationRho( pRecvInfo);
}

void CGameCharaInputContext::ProcessNetFire( GAMEINFO_CHARA_NETDATA * pRecvInfo)
{	
	WeaponBase * pWeapon = nullptr;

	if (m_pOwner->IsDino())
		pWeapon = static_cast<CGameCharaDino*>((CGameCharaBase*)m_pOwner)->GetDinoWeapon();
	else
		pWeapon = m_pOwner->GetCurrentCharaWeapon();
	
	if( pWeapon == nullptr)	return;

	pWeapon = pWeapon->getUsedWeapon();
	I3ASSERT( pWeapon != nullptr);

	// Random Sync
	g_pRand->SetCode( pRecvInfo->_tFireData.getRand() );

	// VirtualCamera�� Y���� �����ɴϴ�.
	VEC3D vPos, vDir;
	getCameraContext()->getVirtualCamPosition( &vPos );

	VEC3D * pTargetPos = const_cast<VEC3D*>(pRecvInfo->_tFireData.getTargetPosition());

	if(  BattleServerContext::i()->IsUseDedicationMode() == true)	// revision 60498  AI��� �������� ���󿡼� HK69�� ���� ������ ������ �ٸ��� ���� ��ź�� ���ư�
	{
		VEC3D vTargetPos;
		pWeapon->GetShotTargetPos( &vTargetPos);
		pRecvInfo->_tFireData.setTargetPosition( &vTargetPos);
	}

	i3Vector::Sub( &vPos, &vPos, m_pOwner->GetPos());
	i3Vector::Add( &vPos, &vPos, getMoveContext()->getRecvPosition() );
	i3Vector::Sub( &vDir, pTargetPos, &vPos );

	WEAPON::FIRE_ORDER order = pRecvInfo->_tWeaponData.getFireOrder();
	
	// KEYWORD : WEAPON_CIC
	if( i3::same_of<WeaponDualCIC*>(pWeapon))
	{
		if( pRecvInfo->_tWeaponData.getFireOrder() > 0)
		{
			MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(pWeapon->GetMainWeapon());
			pWeapon = mainWeapon_DualCIC->getLeftWeapon();
		}
	}

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	if( i3::kind_of<WeaponMiniAxe*>(pWeapon) || i3::same_of<WeaponDualThrowKnife*>(pWeapon))
	{
		switch( order )
		{
		case WEAPON::PRIMARY_FIRE :
			// �ִϸ��̼��� �� �մϴ�. ������ �׼��� ���մϴ�.
			pWeapon->FireCollision( &vPos, pTargetPos );	// �ø����� �����Ѵ�.
			// �׼� ���� üũ �׼��� ���ϰ� ���� ������ ���ݴϴ�.
			if( (!getActionContext()->isUpper( CHARA_UPPER_FIRE ) &&
				pRecvInfo->_tWeaponData.getFireOrder() == WEAPON::PRIMARY_FIRE) )
			{
				m_pOwner->Cmd_Attack( order, nullptr, nullptr, true);
			}
			break;
		case WEAPON::SECONDARY_FIRE :
			// ���� ������.
			//if( pRecvInfo->_tFireData.isValidThrow( m_pOwner->getCharaNetIndex() ) )
			{
				//GlobalFunc::PB_TRACE("NET_Throw");
				pWeapon->NET_Throw( &vPos, pTargetPos, pRecvInfo->_tFireData.getNetSlotIndex() );
				// �׼� ���� üũ �׼��� ���ϰ� ���� ������ ���ݴϴ�.
				if( (!getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) &&
					pRecvInfo->_tWeaponData.getFireOrder() == WEAPON::SECONDARY_FIRE) )
				{
					m_pOwner->Cmd_Attack( order, nullptr, nullptr, true);
				}
			}
			break;
		}
	}
	else if(i3::same_of<WeaponBow*>(pWeapon))
	{
		if( pRecvInfo->_tWeaponData.getWeaponExtension())
		{
			((WeaponBow*)pWeapon)->ThrowArrow( &vPos, pTargetPos,
				m_pOwner->GetThrowSpeed(pWeapon->getWeaponInfo()->GetTypeUsage()), -1, true );
		}
		else
		{
			pWeapon->NET_Throw( &vPos, pTargetPos, pRecvInfo->_tFireData.getNetSlotIndex() );

			// �׼� ���� üũ �׼��� ���ϰ� ���� ������ ���ݴϴ�.
			/*if( !getActionContext()->isUpper( CHARA_UPPER_FIRE) )
				m_pOwner->Cmd_Attack( WEAPON::PRIMARY_FIRE, nullptr, nullptr, true);*/
		}

		if (!getActionContext()->isUpper(CHARA_UPPER_DAMAGE))
			m_pOwner->Cmd_Attack(WEAPON::PRIMARY_FIRE, nullptr, nullptr, true);
	}
	else if( pWeaponInfo->GetTypeFire() == WEAPON::FIRE_MELEE )				// Į.
	{
		// �ִϸ��̼��� �� �մϴ�. ������ �׼��� ���մϴ�.
		if( i3::kind_of<WeaponDino*>(pWeapon) )
			((WeaponDino*)pWeapon)->FireCollisionDino(&vPos, pTargetPos );
		else
			pWeapon->FireCollision( &vPos, pTargetPos );	// �ø����� �����Ѵ�.
		// �׼� ���� üũ �׼��� ���ϰ� ���� ������ ���ݴϴ�.
		
		if( !getActionContext()->isUpper(CHARA_UPPER_SWAPWEAPON) )
		{
			if( (!getActionContext()->isUpper( CHARA_UPPER_FIRE ) &&
				order == WEAPON::PRIMARY_FIRE) ||
				(!getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) &&
				order == WEAPON::SECONDARY_FIRE))
			{
				m_pOwner->Cmd_Attack( order, nullptr, nullptr, true);
			}
		}
	}
	else if( i3::same_of<WeaponBombTrigger*>(pWeapon))
	{	
		switch( order )
		{
		case WEAPON::PRIMARY_FIRE :
			if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE ) )
				m_pOwner->Cmd_Attack( order, &vPos, pTargetPos );
			else
				m_pOwner->Cmd_Only_AttackWeapon();
			break;
		case WEAPON::SECONDARY_FIRE :
			if( pRecvInfo->_tFireData.isValidThrow( m_pOwner->getCharaNetIndex() ) )
			{
				pWeapon->NET_Throw( &vPos, pTargetPos, pRecvInfo->_tFireData.getNetSlotIndex() );
			}
			break;// ���� ������ �ȴ�..
		}
	}
	else if( i3::same_of<WeaponHandGrenade*>(pWeapon))
	{
		if( pRecvInfo->_tFireData.isValidThrow( m_pOwner->getCharaNetIndex() ) )
		{
			if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE ) )
			{	// ��Ʈ��ũ 3��Ī�� ��� ��ź �߻� �ִϸ��̼��� �����ϴ�.
				// �⺻ �߻� �ִϸ��̼��� �ϵ��� �մϴ�.
				m_pOwner->Cmd_Attack( WEAPON::PRIMARY_FIRE, nullptr, nullptr, true);
			}
			
			MainWeapon_HandGrenade* mainWeapon_HG =  static_cast<MainWeapon_HandGrenade*>(pWeapon->GetMainWeapon());

			mainWeapon_HG->ThrowShell( &vPos, pTargetPos,
				m_pOwner->GetThrowSpeed(pWeapon->getWeaponInfo()->GetTypeUsage()), pRecvInfo->_tFireData.getNetSlotIndex());
	
		}

	}
	else if( (i3::kind_of<WeaponGrenadeLauncher*>(pWeapon) && pRecvInfo->_tWeaponData.getWeaponExtension())
		|| i3::same_of<WeaponRocketLauncher*>(pWeapon) ) 
	{
		if( pRecvInfo->_tFireData.isValidThrow( m_pOwner->getCharaNetIndex() ) )
		{
			// KEYWORD :  WEAPON_K201
			if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE ) )
			{	// ��Ʈ��ũ 3��Ī�� ��� ��ź �߻� �ִϸ��̼��� �����ϴ�.
				// �⺻ �߻� �ִϸ��̼��� �ϵ��� �մϴ�.
				m_pOwner->Cmd_Attack( WEAPON::PRIMARY_FIRE, nullptr, nullptr, true);
			}

			// ĳ������ �߻� ���μ����� ���� ó��
			getBoneContext()->_ProcessFire();

			// �ѱ����� �߻��ؾ� �˴ϴ�.
			REAL32 Theta, Rho;
			MATRIX mtx;
			VEC3D vPos2, vDir2, vFirePos;

			// VirtualCamera�� Y���� �����ɴϴ�.
			getCameraContext()->getVirtualCamPosition( &vPos2 );
			i3Vector::Sub( &vPos2, &vPos2, m_pOwner->GetPos());
			i3Vector::Add( &vPos2, &vPos2, getMoveContext()->getRecvPosition() );

			i3Vector::Sub( &vDir2, pTargetPos, &vPos2 );
			i3Vector::Normalize( &vDir2, &vDir2 );

			Theta = i3Math::atan2( getX( &vDir2), getZ( &vDir2));
			Rho = i3Math::acos( getY( &vDir2));
			i3Matrix::SetRotateY( &mtx, Theta );

			VEC3D vRight;
			i3Vector::Copy( &vRight, i3Matrix::GetRight( &mtx));
			//Rotate
			i3Matrix::PostRotateAxis( &mtx, &vRight, Rho);
			i3Vector::TransformCoord( &vFirePos, &vFirePos, &mtx);

			i3Vector::Add( &vFirePos, &vFirePos, &vPos2);

			if( i3::same_of<WeaponRocketLauncher*>(pWeapon))
				((WeaponRocketLauncher*)pWeapon)->ThrowShell( &vFirePos, &vDir2, pWeapon->GetThrowSpeed(), pRecvInfo->_tFireData.getNetSlotIndex());
			else
				((WeaponGrenadeLauncher*)pWeapon)->ThrowShell( &vFirePos, &vDir2, pWeapon->GetThrowSpeed(), pRecvInfo->_tFireData.getNetSlotIndex());
		}
	}
	else if( (pWeaponInfo->GetTypeFire() == WEAPON::FIRE_THROWING) )		// ����ź
	{// �ִϸ��̼��� �� �մϴ�. ������ �׼��� ���մϴ�.
		if( pRecvInfo->_tFireData.isValidThrow( m_pOwner->getCharaNetIndex() ) )
		{
			pWeapon->NET_Throw( &vPos, pTargetPos,
				pRecvInfo->_tFireData.getNetSlotIndex() );		// ���� ������ �ȴ�..
		}

		if( pWeaponInfo->GetTypeDamage() == WEAPON::DAMAGE_POISON )
		{
			if(i3::same_of<CGameCharaDinoSting*>((CGameCharaBase*)m_pOwner))
			{
				((CGameCharaDinoSting*)(CGameCharaBase*)m_pOwner)->ThrowPoisonGrenade();
			}
			else
			{
				I3ASSERT_0;
			}
		}
	}
	else if( i3::same_of<WeaponShotGun3*, WeaponShotGunWithButt*>(pWeapon)
		&& (pRecvInfo->_tWeaponData.getFireOrder() == WEAPON::SECONDARY_FIRE) )
	{
		// �ִϸ��̼��� �� �մϴ�. ������ �׼��� ���մϴ�.
		if (i3::same_of<WeaponShotGun3*>(pWeapon))
			((WeaponShotGun3*)pWeapon)->MeleeCollision( &vPos, pTargetPos );	// �ø����� �����Ѵ�.
		else if (i3::same_of<WeaponShotGunWithButt*>(pWeapon))
			((WeaponShotGunWithButt*)pWeapon)->MeleeCollision( &vPos, pTargetPos );	// �ø����� �����Ѵ�.
		// �׼� ���� üũ �׼��� ���ϰ� ���� ������ ���ݴϴ�.// �׼� ���� üũ �׼��� ���ϰ� ���� ������ ���ݴϴ�.
		if( (!getActionContext()->isUpper( CHARA_UPPER_FIRE ) &&
			order == WEAPON::PRIMARY_FIRE) ||
			(!getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) &&
			order == WEAPON::SECONDARY_FIRE))
		{
			m_pOwner->Cmd_Attack( order, nullptr, nullptr, true);
		}
	}
	else if (i3::same_of<WeaponGunWithKnife*>(pWeapon)
		&& (pRecvInfo->_tWeaponData.getFireOrder() == WEAPON::SECONDARY_FIRE) )
	{
		// �ִϸ��̼��� �� �մϴ�. ������ �׼��� ���մϴ�.
		((WeaponGunWithKnife*)pWeapon)->MeleeCollision( &vPos, pTargetPos );	// �ø����� �����Ѵ�.
		// �׼� ���� üũ �׼��� ���ϰ� ���� ������ ���ݴϴ�.
		if( (!getActionContext()->isUpper( CHARA_UPPER_FIRE ) &&
			pRecvInfo->_tWeaponData.getFireOrder() == WEAPON::PRIMARY_FIRE) ||
			(!getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) &&
			pRecvInfo->_tWeaponData.getFireOrder() == WEAPON::SECONDARY_FIRE))
		{
			m_pOwner->Cmd_Attack( order, nullptr, nullptr, true);
		}
	}
	else
	{// Fire !!
		if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE ) )
		{// ĳ���� �׼��� ���ϴ� ���
			// ���� �߻� �׼��� ���ο��� ����˴ϴ�.
			m_pOwner->Cmd_Attack( order, &vPos, pTargetPos);
		}
		else
		{// ĳ���� �׼� ���� ���� �߻� �׼Ǹ� ���ϴ� ���
			m_pOwner->Cmd_Only_AttackWeapon();
		}
		
		// �ø���
		// ���⼭ ó���� �ؾ� Bullet Trace�� ����
		if (i3::kind_of<WeaponDualSMG*>(pWeapon))
		{
			// �� Dual����ε� Kriss�� ��쿡�� ���� ó���մϴ�.
			WeaponBase * PairWeaepon = pWeapon->getPairWeapon();
			if (PairWeaepon != nullptr && i3::same_of<WeaponKrissSuperV*>(PairWeaepon))
			{
				((WeaponKrissSuperV*)PairWeaepon)->getDualWeapon()->FireCollision(&vPos, pTargetPos);
				// ������ ���
				//getBoneContext()->_ProcessFire();

				WeaponDualSMG* pDualWeapon = static_cast<WeaponDualSMG*>(static_cast<WeaponKrissSuperV*>(PairWeaepon)->getDualWeapon());
				WeaponBase* pLeftWeapon = static_cast<MainWeapon_Dual*>(pDualWeapon->GetMainWeapon())->getLeftWeapon();

				pLeftWeapon->FireCollision(&vPos, pTargetPos);
			}
			else
			{
				((WeaponDualSMG*)pWeapon)->FireCollision(&vPos, pTargetPos);

				getBoneContext()->_ProcessFire();

				WeaponBase* pLeftWeapon = static_cast<MainWeapon_Dual*>(pWeapon->GetMainWeapon())->getLeftWeapon();
				if (pLeftWeapon != nullptr)
					pLeftWeapon->FireCollision(&vPos, pTargetPos);
			}
		}
		else
			pWeapon->FireCollision( &vPos, pTargetPos);

		// KEYWORD : WEAPON_C4
		if( pWeapon->getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_MISSION )
		{
			m_pOwner->Cmd_Idle_Lower();
		}
	}
}

void	CGameCharaInputContext::BindAI(AI_BIND_DATA* pData)
{
	if (i3::same_of<CInputCtrlAI_ForHuman*>(m_pCurrentInputControl))
	{
		CInputCtrlAI_ForHuman* pAi = (CInputCtrlAI_ForHuman*)m_pCurrentInputControl;
		pAi->BindAI(pData);
	}
	else if (i3::same_of<CInputCtrlAI_ForDino*>(m_pCurrentInputControl))
	{
		CInputCtrlAI_ForDino* pAi = (CInputCtrlAI_ForDino*)m_pCurrentInputControl;
		pAi->BindAI(pData);
	}
	else
	{
		///[�ӽ� �ڵ�] AI ĳ���͸� �ϴ� NETWORK ĳ���ͷ� ���� �ϱ� ������,,
		//I3PRINTLOG(I3LOG_FATAL,  "InputControl�� AI���� �ƴѺκп��� BineAI�� ȣ��Ǿ����ϴ�.");
	}
}

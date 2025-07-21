#include "pch.h"
#include "MainWeapon_MiniAxe.h"

#include "BattleServerContext.h"
#include "GameCharaWeaponContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "GameCharaCameraContext.h"
#include "MyRoomInfoContext.h"
#include "Weapon/WeaponItemID.h"
#include "Weapon/WeaponInfo/MeleeWeaponInfo.h"
#include "MainWeapon_ThrowKnife.h"

#define WEAPON_KNIFE_SEC_ATTACK_TIME	0.31f
#define	DETACHHAND_TIME					0.55f

MainWeapon_MiniAxe::MainWeapon_MiniAxe(WeaponBase* p) : MainWeapon_Knife(p)
,  m_pWeaponKnife(nullptr)
, m_bCheckThrow(false), m_iAttackSoundPlayCount(0), m_aiiProfileB_Rebel_Male(-1), m_aiiProfileB_Rebel_Female(-1)
, m_aiiProfileB_Rebel_Male2(-1), m_aiiProfileB_Rebel_Female2(-1)
, m_ThrowItemID(0)
{
		
}

void	MainWeapon_MiniAxe::OnCreate(bool bCreateInWeapon)
{
	MainWeapon_Knife::OnCreate(bCreateInWeapon);

	// ���̷� ���� M7���� ����
	if( bCreateInWeapon == false)
	{
		m_pWeaponKnife	= g_pWeaponManager->QueryWeapon( m_pWeaponBase->getOwner(),
			m_pWeaponBase->getWeaponInfo()->GetSubExtensionItemID(WEAPON::EXT_DUAL), 0, false, true );
		if( i3::same_of<WeaponKnife*>(m_pWeaponKnife) )
		{
			m_pWeaponKnife->setPairWeapon(m_pWeaponBase);
		}
		else
		{
			I3ASSERT( m_pWeaponKnife != nullptr );
			I3ASSERT( i3::same_of<WeaponKnife*>(m_pWeaponKnife) );
		}
	}

	// ������ ���������� ���̸� �����Ͽ� �ν��Ͻ��� �޾ƿ����� �Ѵ�.
	for( UINT32 i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( m_pWeaponBase->getWeaponInfo()->getExtensionType(i) == WEAPON::EXT_THROW)
		{
			m_ThrowItemID = m_pWeaponBase->getWeaponInfo()->GetExtensionItemID(i);
			break;
		}
	}

}

void	MainWeapon_MiniAxe::OnBindResource() 
{
	MainWeapon_Knife::OnBindResource();
		
	i3AIContext* pAICtx = m_pWeaponBase->GetAIContext();

	if( pAICtx != nullptr)
	{
		INT32 id = pAICtx->FindAIStateByName( "UI_RebelMale");
		I3ASSERT( id < 128);
		m_aiiProfileB_Rebel_Male	= (INT8) id;

		id = pAICtx->FindAIStateByName( "UI_RebelFemale");
		I3ASSERT( id < 128);
		m_aiiProfileB_Rebel_Female	= (INT8) id;

		id = pAICtx->FindAIStateByName( "UI_RebelMale2");
		I3ASSERT( id < 128);
		m_aiiProfileB_Rebel_Male2	= (INT8) id;

		id = pAICtx->FindAIStateByName( "UI_RebelFemale2");
		I3ASSERT( id < 128);
		m_aiiProfileB_Rebel_Female2	= (INT8) id;
	}
}

void	MainWeapon_MiniAxe::Reset() 
{
	MainWeapon_Knife::Reset();

	m_bCheckThrow	= false;

	m_pWeaponBase->setLoadedBulletCount( m_pWeaponBase->getWeaponInfo()->GetLoadBullet() );
	m_pWeaponBase->setTotalBulletCount( m_pWeaponBase->getLoadedBulletCount() );
	//m_pWeaponBase->setExtensionActivate(true);

	m_pWeaponKnife->Reset();

	m_pWeaponBase->SetEnable(true);
	m_pWeaponKnife->SetEnable(false);
}

void	MainWeapon_MiniAxe::ReturnInstance() 
{
	MainWeapon_Knife::ReturnInstance();

	I3ASSERT( m_pWeaponKnife != nullptr);
	m_pWeaponKnife->setPairWeapon(nullptr);
	g_pWeaponManager->ReleaseCharaWeapon( m_pWeaponKnife );
	m_pWeaponKnife	= nullptr;
}

void	MainWeapon_MiniAxe::OnSetBlendAlpha( REAL32 val) 
{
	MainWeapon_Knife::OnSetBlendAlpha( val);
	m_pWeaponKnife->OnSetBlendAlpha( val);
}

void	MainWeapon_MiniAxe::OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular) 
{
	MainWeapon_Knife::OnSetMaterialColor( pfDiffuse, pfSpecular);
	m_pWeaponKnife->OnSetMaterialColor( pfDiffuse, pfSpecular);
}

void	MainWeapon_MiniAxe::OnChangeFPSView( void) 
{
	MainWeapon_Knife::OnChangeFPSView();
	m_pWeaponKnife->OnChangeFPSView();
}

void	MainWeapon_MiniAxe::OnChangeObserverView( void) 
{
	MainWeapon_Knife::OnChangeObserverView();
	m_pWeaponKnife->OnChangeObserverView();
}

void	MainWeapon_MiniAxe::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner )
{	
	MainWeapon_Knife::AttachNode(pSceneNode, pOwner);

	if( i3::same_of<CGameCharaBase*>(pOwner))
	{
		m_pWeaponKnife->SetOwner(m_pWeaponBase->getOwner() );
	}
}

void	MainWeapon_MiniAxe::DetachNode()
{
	MainWeapon_Knife::DetachNode();
	if (m_pWeaponKnife)
		m_pWeaponKnife->DetachNode();
}

void	MainWeapon_MiniAxe::OnFire_MiniAxe( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pWeaponBase->isExtensionActivate() || m_pWeaponBase->getOwner() == nullptr )
		return;

	INT32 nCollisionCount = 1;
	REAL32 fAttackTiming = 0.f;
	INT32 autoFireCount = m_pWeaponBase->getAutoFireCount();

	//	���� ��Ŀ� ���� �浹�ð��� ���Ѵ�.	
	switch( m_pWeaponBase->getFireOrder() )
	{
	case WEAPON::PRIMARY_FIRE:
		{	
			nCollisionCount = m_pWeaponBase->getWeaponInfo()->GetAutoFireCount();

			if( autoFireCount == 0)
			{
				fAttackTiming = m_pWeaponBase->getWeaponInfo()->GetAttackTimeA(0);
				if( (m_iAttackSoundPlayCount == 0) && (pCtx->getLocalTime() > (fAttackTiming - 0.1f)) )
				{
					m_pWeaponBase->_OnPlayFireSound();
					m_iAttackSoundPlayCount = 1;
				}
			}
			else
			{
				fAttackTiming = m_pWeaponBase->getWeaponInfo()->GetAttackTimeA(1);

				if( (m_iAttackSoundPlayCount == 1) && (pCtx->getLocalTime() > fAttackTiming - 0.1f) )
				{	// ź���ܰ��� 2��° ���带 ������� �ʵ��� �մϴ�.
					if( !((CMeleeWeaponInfo*)m_pWeaponBase->getWeaponInfo())->NoMoreSecSound())
					{
						m_pWeaponBase->_OnPlayFireSound();
						m_iAttackSoundPlayCount = 2;
					}
				}
			}
		}
		break;
	case WEAPON::SECONDARY_FIRE:
		{
			nCollisionCount = m_pWeaponBase->getWeaponInfo()->GetAutoFireCount2();
			fAttackTiming = m_pWeaponBase->getWeaponInfo()->GetAttackTimeB( autoFireCount);
		}
		break;
	default:	I3PRINTLOG(I3LOG_FATAL,  "AttackType is invalid.");	break;
	}

	/*if( (getOwner()->isLocal()))
	{
		if( (pCtx->getLocalTime() < 0.1f) && (pCtx->getLocalTime() + tm >= 0.1f))
		{
			g_pEdgeTrail->Start( this);
		}
	}*/
		
	//	���� ���ۺ��� Ÿ�� �ð��� �Ǹ� ���������� �ѹ� �ø��� �Ѵ�.
	if( autoFireCount < nCollisionCount && pCtx->getLocalTime() > fAttackTiming)
	{
		if( (m_pWeaponBase->getOwner()->isLocal() || m_pWeaponBase->getOwner()->isAI()))
		{
			m_pWeaponBase->FireCollision( nullptr, nullptr );
			//g_pEdgeTrail->Stop();
		}
		
		autoFireCount++;
	}

	m_pWeaponBase->setAutoFireCount( autoFireCount);

	// ���� �����ð��� ����� �����ٸ�...Ready ���·�
	//if( pCtx->getLocalTime() + tm > (m_pWeaponInfo->GetFireDelay()))
	REAL32 rDelay = pCtx->getLocalTime() + tm;
	if( rDelay > m_pWeaponBase->GetFireDelay())
	{
		m_pWeaponBase->_OnProcessFireEndAnim( rDelay);
	}

}

void	MainWeapon_MiniAxe::OnSecondaryFire_MiniAxe( i3AIContext * pCtx, REAL32 tm)
{

	if( m_pWeaponBase->isExtensionActivate() )
		return;

	VEC3D target;
	REAL32 rDelay = pCtx->getLocalTime() + tm;

	if( m_pWeaponBase->getOwner() == nullptr )
		return;

	if( !m_bCheckThrow )
	{
		//ź���ܰ� �߻� �ӵ� DETACHBALLISTIC_TIME�� �����մϴ�. (pef->ThrowDelayTime)
		REAL32		fTime	= 0.0f;

		CMeleeWeaponInfo * pWeaponInfo = (CMeleeWeaponInfo*)m_pWeaponBase->getWeaponInfo();
		if( pWeaponInfo->GetThrowDelayTime() > 0.f)
		{
			fTime	= pWeaponInfo->GetThrowDelayTime();
		}
		else
		{
			fTime	= DETACHHAND_TIME;
		}

		if( (m_iAttackSoundPlayCount == 0) && (rDelay > fTime - 0.1f))
		{
			//_OnPlayFireSound();
			m_iAttackSoundPlayCount = 1;
		}

		if( (BattleServerContext::i()->IsP2PHost() && rDelay > fTime ) ||
			(!BattleServerContext::i()->IsP2PHost() && rDelay > (fTime - 0.05f) ) )
		{
			if( m_pWeaponBase->getOwner()->isLocal() )// ��Ʈ��ũ�� ��Ŷ���� ó���մϴ�.
			{
				//�տ��� �������� �ð�
				// ������ ���ϰ� �ִ� ��ǥ ������ ���Ѵ�.
				m_pWeaponBase->GetShotTargetPos( &target);
				m_pWeaponBase->NET_Throw( nullptr, &target, -1 );
			}
		}
	}

	if( rDelay > m_pWeaponBase->GetFireDelay() && m_bCheckThrow)
	{
		m_pWeaponBase->_OnProcessFireEndAnim( rDelay);
	}

}

bool	MainWeapon_MiniAxe::Fire_MiniAxe( WEAPON::FIRE_ORDER type )
{
	if(m_pWeaponBase->isExtensionActivate() == false)
	{
		m_pWeaponBase->SetFireOrder(type);

		// 1�� ���� ���¿��� ��ô �Ұ���.
		if( (m_pWeaponBase->getLoadedBulletCount() == 1) && 
			(m_pWeaponBase->getFireOrder() == WEAPON::SECONDARY_FIRE) && 
			((CMeleeWeaponInfo*)m_pWeaponBase->getBaseWeaponInfo())->DoNotThrowLast())
		{
			return false;
		}
		
		
		//	Head�� IK �߰� ������ ����
		m_pWeaponBase->NET_Fire( nullptr, nullptr, m_pWeaponBase->getFireOrder() );
		m_pWeaponBase->addWeaponStateMask( WSM_WAIT_AUTOFIREDELAY | WSM_NEED_SEND_FIREACTION | WSM_FIRE_DISABLE | WSM_FIRING | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		m_bCheckThrow	= false;

		return true;
	}
	else
		return m_pWeaponKnife->Fire(type);

}

void	MainWeapon_MiniAxe::FireEnd_MiniAxe(void)
{
	if(m_pWeaponBase->isExtensionActivate() == false)
		return static_cast<WeaponKnife*>(m_pWeaponBase)->WeaponKnife::FireEnd();			// ������ ���� �ȵȴ�..
	else
		return m_pWeaponKnife->FireEnd();
}

void	MainWeapon_MiniAxe::FireCollision_MiniAxe( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	if(m_pWeaponBase->isExtensionActivate() == false)
		static_cast<WeaponKnife*>(m_pWeaponBase)->WeaponKnife::FireCollision( pStartPos, pTargetPos);		// ������ ���� �ȵȴ�..
	else
		m_pWeaponKnife->FireCollision( pStartPos, pTargetPos);
}


void	MainWeapon_MiniAxe::NET_Fire_MiniAxe( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type  )
{
	if(m_pWeaponBase->isExtensionActivate() == false)
	{
		static_cast<WeaponKnife*>(m_pWeaponBase)->WeaponKnife::NET_Fire( pStartPos, pTargetPos, type);		// ������ ���� �ȵȴ�..
		m_iAttackSoundPlayCount = 0;
	}
	else
		m_pWeaponKnife->NET_Fire( pStartPos, pTargetPos, type);
}

void	MainWeapon_MiniAxe::OnPlayChangeAni( REAL32 rTimeScale)
{
	if( m_pWeaponBase->getLoadedBulletCount() > 0)
	{
		m_pWeaponBase->ExtensionActivate( true);

		if( g_pFramework->IsBattleStage() )
			static_cast<WeaponKnife*>(m_pWeaponBase)->WeaponKnife::OnPlayChangeAniImpl( rTimeScale);			// ������ ���� �ȵȴ�..
	}
	else
	{
		m_pWeaponBase->setExtensionActivate(true);		// ���������� �ͽ��ټ��� �������ֱ� ���ؼ� ���
		m_pWeaponBase->ExtensionActivate( false);

		if(m_pWeaponKnife != nullptr)
			m_pWeaponKnife->OnPlayChangeAni( rTimeScale);
	}

}

void	MainWeapon_MiniAxe::ReturnToReady_MiniAxe( void)
{
	if( m_pWeaponBase->isExtensionActivate() == false)
	{
		static_cast<WeaponKnife*>(m_pWeaponBase)->WeaponKnife::ReturnToReady();						// ������ ���� �ȵȴ�..

		// ReturnToReady���� ���������� Extension�� ���ֱ⶧���� �̰����� �ٽ� �������ش�.
		// MiniAxe�� SetEnableMiniAxe() �Լ������� Extension�� �����Ǿ�� �Ѵ�.
		//m_pWeaponBase->setExtensionActivate(true);
	}
	else
		m_pWeaponKnife->ReturnToReady();
}

void	MainWeapon_MiniAxe::SetOwner( CGameCharaBase * pOwner)
{
	m_pWeaponKnife->SetOwner(pOwner);
}

void	MainWeapon_MiniAxe::SetEnable(bool bTrue /* = true */, REAL32 tm /* = 0.f */)
{
	if( m_pWeaponBase->isExtensionActivate() )
		SetEnableBaseWeapon( false );
}

bool MainWeapon_MiniAxe::SetEnableMiniAxe( bool bUse)
{
	if( m_pWeaponBase->isExtensionActivate() == bUse )
	{
		SetEnableBaseWeapon(bUse);
		return true;
	}

	return false;
}

void	MainWeapon_MiniAxe::SetEnableBaseWeapon(bool bEnable)
{
	WeaponBase*		pEnableWeapon	= nullptr;

	if( bEnable )	pEnableWeapon	= m_pWeaponBase;
	else
	{
		pEnableWeapon	= m_pWeaponKnife;
		m_pWeaponKnife->SetSwapSoundPlayState(I3SND_RT_PLAYSTATE_FREE); // Free ���°� �ƴϸ� ���� ������尡 ��� �ȵ�.
	}

	m_pWeaponBase->setExtensionActivate(bEnable);

	DetachNode();
	pEnableWeapon->AttachNode( m_pWeaponBase->getOwner()->getWeaponContext()->getAttachRoot(), m_pWeaponBase->getOwner());
	pEnableWeapon->ReturnToReady();
	m_pWeaponBase->getOwner()->Cmd_ApplyCrosshair();

	{
		VEC3D vec;
		pEnableWeapon->SetEnable( true);
		pEnableWeapon->SetPos( &vec );		// LocalPose�� �ʱ�ȭ ������մϴ�.
	}

	// �߿�
	// ���Ⱑ �ٲ�� �ش� ���⿡ �´� �ִϸ��̼� ���� �����ؾ� �մϴ�.
	m_pWeaponBase->getOwner()->Cmd_Only_ChangeWeaponAction();

	if( !m_pWeaponBase->getOwner()->isCharaStateMask( CHARA_STATE_DEATH))
		m_pWeaponBase->getOwner()->getWeaponContext()->setStaticCurrentWeapon( pEnableWeapon);

	m_pWeaponKnife->SetVisible(!bEnable);
	m_pWeaponBase->SetVisible(bEnable);
}

void MainWeapon_MiniAxe::_Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx)
{
	WEAPON::RES_ID weaponNum = m_pWeaponBase->getWeaponInfo()->GetRealNumber();

	for( UINT32 i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( m_pWeaponBase->getWeaponInfo()->getExtensionType(i) == WEAPON::EXT_THROW)
		{
			weaponNum = WEAPON::ItemID2ResID( m_pWeaponBase->getWeaponInfo()->GetExtensionItemID(i) );
			break;
		}
	}

	if( m_ThrowItemID > 0)
		weaponNum = WEAPON::ItemID2ResID( m_ThrowItemID);

	WeaponThrowKnife * pNewWeapon = static_cast<WeaponThrowKnife*>(g_pWeaponManager->GetFreeThrowWeapon( WEAPON_CLASS_KNIFE, weaponNum));

	if( pNewWeapon == nullptr )
	{
		// �ν��Ͻ��� �����ϴ�..?
		//I3PRINTLOG(I3LOG_FATAL,  "no instance resource." );
		I3ASSERT( pNewWeapon != nullptr );
		return;
	}

	MainWeapon_ThrowKnife* mainWeapon_TK = static_cast<MainWeapon_ThrowKnife*>(pNewWeapon->GetMainWeapon());

	pNewWeapon->SetOwner( m_pWeaponBase->getOwner());	// ���� �����ؾߵ˴ϴ�.	
	pNewWeapon->Reset();
	pNewWeapon->setPairWeapon(m_pWeaponBase);
	mainWeapon_TK->Throw( vStart, vDir, rSpeed, nNetSlotIdx);
	pNewWeapon->SetFireOrder( WEAPON::SECONDARY_FIRE);

	VEC3D vDirection;
	i3Vector::Set( &vDirection, getX( mainWeapon_TK->getDirection()), 0.f, getZ( mainWeapon_TK->getDirection()));
	i3Vector::Normalize( &vDirection, &vDirection);
	REAL32 rRot = i3Vector::Dot( &vDirection, &I3_ZAXIS);

	i3Matrix::SetRotateX( pNewWeapon->GetMatrix(), I3_PI);

	if( getX( &vDirection) >= 0.f)
		i3Matrix::SetRotateY( pNewWeapon->GetMatrix(), (1.f - rRot) * I3_PI2 + I3_PI);
	else
		i3Matrix::SetRotateY( pNewWeapon->GetMatrix(), -(1.f - rRot) * I3_PI2 + I3_PI);

	i3Vector::Cross( mainWeapon_TK->getRightVec(), &I3_YAXIS, &vDirection);
	i3Vector::Normalize( mainWeapon_TK->getRightVec(), mainWeapon_TK->getRightVec());

	// Network ID
	pNewWeapon->setNetworkIdx( static_cast<INT16>(nNetSlotIdx) );
	if( nNetSlotIdx == -1)
	{
		const GAMEINFO_CHARACTER * pSendBuffer = BattleSlotContext::i()->getGameInfo_Chara( m_pWeaponBase->getOwner()->getCharaNetIndex());
		pNewWeapon->setNetworkIdx( pSendBuffer->_tNetData._tFireData.getNetSlotIndex());
	}
	else if( nNetSlotIdx < MAX_THROWWEAPON_COUNT)
	{
		GAMEINFO_THROW_WEAPON * pGrenadeInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( nNetSlotIdx );
		pGrenadeInfo->_tNetData.SetDir( *(mainWeapon_TK->getDirection()) );
	}

	VEC3D	vTemp;
	m_pWeaponBase->getOwner()->getCameraContext()->getVirtualCamPosition( &vTemp );

	GAMEEVENT::BindNetGrenadeInfo( pNewWeapon->getNetworkIdx(), pNewWeapon, &vTemp );

	if( m_pWeaponBase->IsDedicatedHost() == false )
	{	
		pNewWeapon->SetPos( &vTemp);
	}
}

void	MainWeapon_MiniAxe::NET_FirstThrow_MiniAxe( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx)
{
	bool bFirstThrow = true;

	/*if( m_pWeaponBase->IsDedicatedHost() == false )
	{
		const GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->getGameInfo_ThrowWeapon( nNetSlotIdx );
		if( pInfo->_pWeaponBase != nullptr )
			bFirstThrow = false;
	}
	else*/
	bFirstThrow = !m_bCheckThrow;

	if( m_pWeaponBase->isExtensionActivate() && bFirstThrow == false )
		return;

	I3ASSERT( m_pWeaponBase->getWeaponInfo() != nullptr);

	m_bCheckThrow = true;

	_Throw( pStartPos, pDir, m_pWeaponBase->getWeaponInfo()->GetThrowSpeed(), nNetSlotIdx);

#ifdef ENABLE_TEST_COMMAND
	if( !g_pWeaponManager->COMMAND_IsUnlimitGrenade())		//	Use Cheat
#endif
	{
		INT32 iBullet = m_pWeaponBase->getLoadedBulletCount();

		iBullet--;
		if( iBullet < 0)
			iBullet = 0;

		//GlobalFunc::PB_TRACE("NET_FirstThrow iBullet %d", iBullet);
		m_pWeaponBase->setLoadedBulletCount( iBullet);
	}

	m_pWeaponBase->_PlayFireSound( WEAPON::FIRE_SOUND_NORMAL_2);

	m_pWeaponBase->WeaponBase::NET_FirstThrow( pStartPos, pDir, nNetSlotIdx );			// ������ ���� �ȵȴ�..
}

void	MainWeapon_MiniAxe::NET_Throw_MiniAxe( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx )
{
	m_pWeaponBase->WeaponBase::NET_Throw( pStartPos, pTargetPos, nThrowNetIdx );  // ������ ���� �ȵȴ�..

	VEC3D start, dir;

	if( pStartPos == nullptr )
	{	
		m_pWeaponBase->getOwner()->getCameraContext()->getVirtualCamPosition( &start);					// ����� Camera�� ����
	}
	else
	{
		i3Vector::Copy( &start, pStartPos );
	}


	// �������ϴ�.
	m_pWeaponBase->NET_FireSync( pTargetPos );

	i3Vector::Sub( &dir, pTargetPos, &start);
	i3Vector::Normalize( &dir, &dir);

	m_pWeaponBase->NET_FirstThrow( &start, &dir, nThrowNetIdx );
}

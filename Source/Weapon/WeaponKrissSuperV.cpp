#include "pch.h"
#include "WeaponKrissSuperV.h"
#include "GlobalVariables.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaWeaponContext.h"
#include "WeaponFunction.h"

#include "AIDriver_Weapon.h"

I3_CLASS_INSTANCE( WeaponKrissSuperV);//, WeaponBase);

WeaponKrissSuperV::WeaponKrissSuperV()
{	
	m_bUseDualWeapon = false;
	m_pDualWeapon = nullptr;
}


WeaponKrissSuperV::~WeaponKrissSuperV()
{
}

bool WeaponKrissSuperV::_SwitchingWeapon( WeaponBase * pPrevWeapon, WeaponBase * pNextWeapon, bool bPlaySound /* = false */)
{
	DetachNode();
	pNextWeapon->AttachNode( m_pOwner->getWeaponContext()->getAttachRoot(), m_pOwner);
	pNextWeapon->ReturnToReady();
	pPrevWeapon->ReturnToReady();
	m_pOwner->Cmd_ApplyCrosshair();

	{
		VEC3D vec;
		pNextWeapon->SetEnable( true);
		pNextWeapon->SetPos( &vec );		// LocalPose�� �ʱ�ȭ ������մϴ�.
	}

	// *�߿�* ���Ⱑ �ٲ�� �ش� ���⿡ �´� �ִϸ��̼� ���� �����ؾ� �մϴ�.
	m_pOwner->Cmd_Only_ChangeWeaponAction();

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))
		m_pOwner->getWeaponContext()->setStaticCurrentWeapon( pNextWeapon );

	pPrevWeapon->SetVisible( false);
	pNextWeapon->SetVisible( true);

	// ��ü ���� �÷���
	if( bPlaySound)	pNextWeapon->PlaySwapSound();

	return true;
}

/*virtual*/ void WeaponKrissSuperV::PostCreate( void)
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	T_ItemID dualItemID = m_pWeaponInfo->GetSubExtensionItemID();
	I3ASSERT( dualItemID > 0);

	// ���� ���⸦ ����
	// KrissSuperV�� �˴ϴ�. �ٸ� ���⸦ �߰��Ѵٸ� ���⼭ �ش� ���⿡ �´� ���⸦ ����
	m_pDualWeapon = g_pWeaponManager->QueryWeapon( getOwner(), dualItemID);
	I3ASSERT( m_pDualWeapon != nullptr);

	m_pDualWeapon->Reset();
	m_pDualWeapon->SetOwner( getOwner());
	m_pDualWeapon->setPairWeapon(this);

	// ���� �� ���߿� ����ݴϴ�.
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)( m_pDualWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)( m_pDualWeapon->getIdx() / 10) );
	g_pWeaponManager->MoveShapePosition( m_pDualWeapon, &vec );
}

/*virtual*/ void WeaponKrissSuperV::Reset( void)
{
	WeaponBase::Reset();

	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->Reset();

	SetEnableDualWeapon( false);
}

/*virtual*/ void WeaponKrissSuperV::OnSetBlendAlpha( REAL32 val)
{
	WeaponBase::OnSetBlendAlpha( val);
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->OnSetBlendAlpha( val);
}

/*virtual*/ void WeaponKrissSuperV::OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	WeaponBase ::OnSetMaterialColor( pfDiffuse, pfSpecular);
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->OnSetMaterialColor( pfDiffuse, pfSpecular);
}

/*virtual*/ void WeaponKrissSuperV::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	I3ASSERT( m_pDualWeapon != nullptr);

	if( !m_pDualWeapon->isAttached() )
		WeaponBase::AttachNode( pSceneNode, pOwner);

	if( i3::same_of<CGameCharaBase*>(pOwner))
		m_pDualWeapon->SetOwner( m_pOwner);
}

/*virtual*/ void WeaponKrissSuperV::DetachNode( void )
{
	WeaponBase::DetachNode();
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->DetachNode();
}

/*virtual*/void WeaponKrissSuperV::ReturnInstance()
{
	WeaponBase::ReturnInstance();
	
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->setPairWeapon( nullptr);
	g_pWeaponManager->ReleaseCharaWeapon( m_pDualWeapon);
	m_pDualWeapon = nullptr;
}

/*virtual*/ void WeaponKrissSuperV::OnChangeFPSView( void)
{
	WeaponBase::OnChangeFPSView();
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->OnChangeFPSView();
}

/*virtual*/ void WeaponKrissSuperV::OnChangeObserverView( void)
{
	WeaponBase::OnChangeObserverView();
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->OnChangeObserverView();
}

/*virtual*/ void WeaponKrissSuperV::ReturnToReady( void)
{
	WeaponBase::ReturnToReady();
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->ReturnToReady();
}

bool WeaponKrissSuperV::SetEnableDualWeapon( bool bUse, bool bPlaySound /*= false*/)
{
	I3ASSERT( m_pDualWeapon != nullptr);
	I3ASSERT( m_pOwner != nullptr);

	if( m_bUseDualWeapon && !bUse)
	{	
		// ��󿡼� �̱۷�
		m_bUseDualWeapon = bUse;
		return _SwitchingWeapon(m_pDualWeapon, this, bPlaySound);
	}
	else if( !m_bUseDualWeapon && bUse)
	{	
		// �̱ۿ��� ����
		if( m_pDualWeapon->getLoadedBulletCount() <= 0)
			return false;

		m_bUseDualWeapon = bUse;
		return _SwitchingWeapon( this, m_pDualWeapon, bPlaySound );
	}

	return false;
}

void WeaponKrissSuperV::OnUpdate( REAL32 rDeltaSeconds)
{
	WeaponBase::OnUpdate( rDeltaSeconds);
	I3ASSERT( m_pDualWeapon != nullptr);
	if( m_bUseDualWeapon)
		m_pDualWeapon->OnUpdate( rDeltaSeconds);
}

bool WeaponKrissSuperV::Fire( WEAPON::FIRE_ORDER type)
{
	I3ASSERT( m_pDualWeapon != nullptr);
	if( m_bUseDualWeapon)	return m_pDualWeapon->Fire( type);
	else					return WeaponBase::Fire( type);
}

void WeaponKrissSuperV::FireEnd(void)
{
	I3ASSERT( m_pDualWeapon != nullptr);
	if( m_bUseDualWeapon)	m_pDualWeapon->FireEnd();
	else					WeaponBase::FireEnd();
}

/*virtual*/ void WeaponKrissSuperV::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	I3ASSERT( m_pDualWeapon != nullptr);
	if( m_bUseDualWeapon)	m_pDualWeapon->NET_Fire( pStartPos, pTargetPos, type);
	else					WeaponBase::NET_Fire( pStartPos, pTargetPos, type);
}

/*virtual*/ void WeaponKrissSuperV::OnPlayChangeAniImpl( REAL32 rTimeScale /*= 1.f*/ )
{
	if( m_bUseDualWeapon)
	{
		SetEnableDualWeapon( false);
		SetEnableDualWeapon( true);
		setExtensionActivate( m_bUseDualWeapon);
	}
	else
	{
		SetEnableDualWeapon( true);
		SetEnableDualWeapon( false);	
		setExtensionActivate( false);
	}

	WeaponBase::OnPlayChangeAniImpl( rTimeScale);
}

/*virtual*/ void WeaponKrissSuperV::OnPlayChangeSound( void)
{
	// ���� ��ü ���� 
	// SetEnableDualWeapon �Լ� ���ο��� ReturnToReady�� ȣ���Ͽ� StopSwapSound�� �Ǳ� ������ ���Ŀ� �ؾ� ��. komet
	if( m_pDualWeapon != nullptr && isUsedDualWeapon())
		m_pDualWeapon->PlaySwapSound();
	else
		_PlaySwapSound();
}

/*virtual*/ bool WeaponKrissSuperV::OnCheckExtensionPossible( bool bVal)
{
	//I3ASSERT( m_pDualWeapon != nullptr);
	//if( !m_bUseDualWeapon && m_pDualWeapon->getLoadedBulletCount() <= 0)
	//	return false;

	//return (m_bUseDualWeapon != bVal);
	if( WeaponBase::OnCheckExtensionPossible( bVal))
		return SetEnableDualWeapon( bVal, true);
	return false;
}

/*virtual*/ void WeaponKrissSuperV::OnProcessExtension( bool bVal)
{
	SetEnableDualWeapon( bVal, true);

	if( m_pOwner != nullptr && m_pOwner->isLocal())
	{
		i3::stack_wstring	wstrString;

		if( bVal)
		{
			//	"������ ����Ǿ����ϴ�."
			i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_DUAL_WEAPON"));
			g_pFramework->getChatBox()->PutSystemChatting( wstrString);
		}
		else
		{
			//	"����尡 �����Ǿ����ϴ�."
			i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_DEACTIVATE_DUAL_WEAPON"));
			g_pFramework->getChatBox()->PutSystemChatting( wstrString);
		}
	}
}

/*virtual*/ void WeaponKrissSuperV::OnPlayExtensionAnimation(bool bVal, bool bAnimPlay /* = true */)
{
	if( i3::same_of<AIDriverGun_13PV_FoldedGun_OA*>(this->GetAIContext()->getAI()->getAIDriver()) )
	{
		WeaponBase::OnPlayChangeAniImpl(1.0f);
		return;
	}

	WeaponBase::OnPlayExtensionAnimation(bVal,bAnimPlay);
}

/*virtual*/ void WeaponKrissSuperV::OnDrop( void)
{
	WeaponBase::OnDrop();

	I3ASSERT( m_pDualWeapon != nullptr);

	m_bUseDualWeapon = false;
}

void WeaponKrissSuperV::SetOwner( CGameCharaBase * pOwner)
{
	WeaponBase::SetOwner( pOwner);

	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->SetOwner( pOwner);
}

/*virtual*/ void WeaponKrissSuperV::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	I3ASSERT( m_pDualWeapon != nullptr);

	if( m_bUseDualWeapon)	m_pDualWeapon->FireCollision( pStartPos, pTargetPos);
	else					WeaponBase::FireCollision( pStartPos, pTargetPos);
}

/*virtual*/ void WeaponKrissSuperV::SetBulletCountForNetwork( INT32 iLoadedCount, INT32 iDualLoadedCount, INT32 iMaxCount)
{
	I3ASSERT( m_pWeaponInfo != nullptr);
	if( GetLoadBulletWithCashItem() < iLoadedCount)
		iLoadedCount = GetLoadBulletWithCashItem();
	setLoadedBulletCount( iLoadedCount );

	INT32 iMaxLoadCount =  GetMaxBulletWithCashItem();

	if( iMaxLoadCount < iMaxCount )
		iMaxCount = iMaxLoadCount;
	setTotalBulletCount( iMaxCount);

	// ���⿡ ���� ���� ����
	// revision 36800 KrissSV Drop���� ���� �� ����� źâ �� ���� ���� - hansoft.1497
	if( getDualWeapon() != nullptr )
	{
		INT32	iCashBullet	= getDualWeapon()->GetLoadBulletWithCashItem();	// revision 36866
		if( iCashBullet < iDualLoadedCount)
			iDualLoadedCount	= iCashBullet;
		setLoadedBulletDualCount( iDualLoadedCount);
	}
}

void WeaponKrissSuperV::SetEnable( bool bTrue, REAL32 tm)
{
	WeaponBase::SetEnable( bTrue, tm);

	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->SetEnable( bTrue, tm);
}

void WeaponKrissSuperV::PlayIdleAnim()
{
	WF::PlayWeaponAnim(this, getAIIndex(WEAPON::AI_IDLE));
}

void WeaponKrissSuperV::PlayIdleFoldedAnim()
{
	WF::PlayWeaponAnim(this, getAIIndex(WEAPON::AI_IDLE_FOLDED));
}

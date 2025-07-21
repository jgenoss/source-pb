#include "pch.h"
#include "GlobalVariables.h"
#include "WeaponShotGunLauncher.h"
#include "GameCharaBoneContext.h"
#include "GameCharaMoveContext.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaCameraContext.h"
#include "WeaponFunction.h"
#include "i3Base/string/ext/utf16_to_mb.h"

I3_CLASS_INSTANCE( WeaponShotGunLauncher);//, WeaponBase);

WeaponShotGunLauncher::WeaponShotGunLauncher() : m_aiiFireShell(-1), m_aiiFireShell1PV(-1)
	, m_aiiFireShell3PV(-1), m_aiiReloadShell1PV(-1), m_aiiReloadShell3PV(-1), m_idxShellFireEffect(-1)
	, m_idxLauncherFXBone(-1), m_idxLSSBulletEffect(-1), m_nLoadedShell(0), m_nTotalShell(0)
	, m_bLoadedShell(true), m_idxLauncherFXBullet(-1)
{	
}

WeaponShotGunLauncher::~WeaponShotGunLauncher()
{
}

void WeaponShotGunLauncher::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponBase::Create( pOwner, Itemid, bCreateInWeapon);

#ifdef I3_DEBUG
	WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( Itemid );
	WEAPON::RES_ID number = WEAPON::ItemID2ResID( Itemid );	
#endif

	// �ͽ��ټ� ������ �ݵ��� �ʿ��մϴ�.
	I3ASSERT( m_pExtWeaponInfo != nullptr);

	// M26 LSS �ϰ�� �߻� ����
	//if( classType == WEAPON_CLASS_SMG && number == GetItemIndex( "SMG_M4_CQBR_LV3"))
	{
		m_idxLauncherFXBone = getSceneObject()->GetBody()->FindBoneByName( "FXDummy_XM26LSS");
		m_idxLauncherFXBullet = getSceneObject()->GetBody()->FindBoneByName( "EmptyCartridgeFXDummy_LSS");
	}

#ifdef I3_DEBUG
	if( m_idxLauncherFXBone == -1)
	{
		I3TRACE("[WeaponShotGunLauncher] Cannot find Launcher FXDummy (class:%d num:%d)\n", classType, number);
	}

	if( m_idxLauncherFXBullet == -1)
	{
		I3TRACE("[WeaponShotGunLauncher] Cannot find Launcher EmptyCartrige FXDummy (class:%d num:%d)\n", classType, number);
	}
#endif

	// �߻� ����Ʈ
	if( m_pExtWeaponInfo != nullptr)
	{
		i3::rc_wstring	wstrFireEffect;
		m_pExtWeaponInfo->GetFireEffect(wstrFireEffect);
		i3::string strFireEffect;	i3::utf16_to_mb(wstrFireEffect, strFireEffect);

		m_idxShellFireEffect = g_pEffectManager->FindEffect(strFireEffect.c_str());
	}

	// LSS ź��
	m_idxLSSBulletEffect = g_pEffectManager->FindEffect( "Bul_Sg_870MCS");
	I3ASSERT( m_idxLSSBulletEffect != -1);
}

void WeaponShotGunLauncher::Reset( void)
{
	WeaponBase::Reset();

	setLoadedShellCount( m_pExtWeaponInfo->GetLoadBullet());

	// Total Shell Count = �ִ� ź�� - ������ ź��
	INT32 i32TotalShell = m_pExtWeaponInfo->GetMaxBullet() - m_pExtWeaponInfo->GetLoadBullet();

	if(i32TotalShell < 0)
		i32TotalShell = 0;

	setTotalShellCount( i32TotalShell );

	m_bLoadedShell = true;

	m_pWeaponInfo = m_pAbsoluteWeaponInfo;		//	����� �ֹ��� ������ �ٽ� ��ȯ

	m_CurrentExtensionType = m_pWeaponInfo->getExtensionType(0); //	ù��° �ͽ��ټ����� �ʱ�ȭ ����. �߰��� �ٲ�� �ִ� ��
}

void WeaponShotGunLauncher::SetExtWeaponInfo()
{
	// �ͽ��ټ� ��� ������ �ͽ��ټ� ����
	for( INT32 i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( m_pWeaponInfo->getExtensionType( i) == WEAPON::EXT_SHOTGUN_LAUNCHER)
		{
			m_pExtWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( m_pWeaponInfo->GetExtensionItemID(i));
			break;
		}
	}
	
	I3ASSERT( m_pExtWeaponInfo != nullptr);
}

void WeaponShotGunLauncher::OnBindResource(void)
{
	WeaponBase::OnBindResource();

	if( m_pAICtx != nullptr)
	{
		m_aiiFireShell = (INT8) m_pAICtx->FindAIStateByName( "FireShell");
		m_aiiFireShell1PV = (INT8) m_pAICtx->FindAIStateByName( "FireShell1PV");
		m_aiiFireShell3PV = (INT8) m_pAICtx->FindAIStateByName( "FireShell3PV");
		m_aiiReloadShell1PV = (INT8) m_pAICtx->FindAIStateByName( "LoadShell1PV");
		m_aiiReloadShell3PV = (INT8) m_pAICtx->FindAIStateByName( "LoadShell3PV");
	}
}

bool WeaponShotGunLauncher::OnCheckExtensionPossible( bool bVal)
{
	if(WeaponBase::OnCheckExtensionPossible( bVal))
	{
		if( m_pOwner != nullptr && m_pOwner->isNetwork() )
		{
			if( bVal )
				m_CurrentExtensionType	= m_pAbsoluteWeaponInfo->getExtensionType(1);
			else
				m_CurrentExtensionType	= m_pAbsoluteWeaponInfo->getExtensionType(0);
		}

		return true;
	}

	return false;
}

bool WeaponShotGunLauncher::Fire( WEAPON::FIRE_ORDER type )
{
	if( GetCurrentExtensionType() != WEAPON::EXT_SHOTGUN_LAUNCHER )
	{
		if( getLoadedBulletCount() <= 0)
			return false;
	}
	else
	{
		if( getLoadedShellCount() <= 0)
			return false;
	}

	m_FireOrder = type;

	// ������ ���ϰ� �ִ� ��ǥ ������ ���Ѵ�.
	GetShotTargetPos( m_TargetPos);

	NET_Fire( nullptr, m_TargetPos, m_FireOrder );

	

	return true;
}

void WeaponShotGunLauncher::OnPlayExtensionAnimation( bool bVal, bool bAnimPlay)
{
	if( bAnimPlay )
	{
		if( bVal)
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE));
		else
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE));
	}
	else
		_OnPlayWeaponIdleAnim();
}

/*virtual*/ void WeaponShotGunLauncher::OnProcessExtension( bool bVal)
{
	// LSS�� ��� ������ ���� ������ �����ϹǷ�, LSS�� Extension Ȱ��ȭ�� ������ ���Ǵ� m_pWeaponInfo�� �����ϱ� ���� m_pExtWeaponInfo ������ ��ü�ȴ�.	
	if( m_pExtWeaponInfo != nullptr && GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER)
	{
		if( bVal)
			m_pWeaponInfo = m_pExtWeaponInfo;		//	Extension ���Ⱑ �ֹ��� ������ �ȴ�.
		else
			m_pWeaponInfo = m_pAbsoluteWeaponInfo;	//	���� �ֹ��� ������ �ٽ� ��ȯ
	}
}

void WeaponShotGunLauncher::LoadShell( void)
{
	if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()) &&
		(getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT) )
	{
		addWeaponStateMask( WSM_FIRE_DISABLE | WSM_EXT_DISABLE | WSM_LOADMAG_DISABLE);
	}

	if(  m_pOwner != nullptr && m_pOwner->is1PV())
		_PlayWeaponAnimWithApplyTimeScale( m_aiiReloadShell1PV );
	else if(  m_pOwner != nullptr && m_pOwner->is3PV())
		_PlayWeaponAnimWithApplyTimeScale( m_aiiReloadShell3PV );

	setWeaponState( WEAPON::STATE_LOADMAG);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//		Animation Callback
void WeaponShotGunLauncher::OnIdle( i3AIContext * pCtx, REAL32 tm)
{	
	bool bLoaded = true;

	if( getOwner() == nullptr )
		return;

	// ���� ĳ������ ���Ⱑ �ƴ϶�� ������ �ȵ˴ϴ�.
	if( !getOwner()->isLocal() && !getOwner()->isAI() )
		return;

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	//2015-04-06 �ͽ��ټ� ���� �������� �ش� �������� �ٲߴϴ�. (���� isExtension() ) ENG_��뿵
	if( GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER )
	{
		if( getLoadedShellCount() <= 0)
		{
			if( getTotalShellCount() > 0)
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);		// �ڵ� ������
				removeWeaponStateMask( WSM_LOADMAG_DISABLE);
			}
			else
			{
				{
					setWeaponState( WEAPON::STATE_READY);
					removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_EXT_DISABLE);

					// ĳ������ �ͽ��ټ��� ���� ȣ���ϱ� ������ ĳ������ ���¸� �ʱ�ȭ ���־�� �Ѵ�.
					// ĳ���ʹ� �� ������ ���¿��� ���� ������ ���Ƴ���. ���Ⱑ ���� ���·� �ٲ�� �ʱ�ȭ����� �Ѵ�.
					m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);

					m_pOwner->Cmd_Extension( false);

					//	��ȹ�� ��û���� Ŀ���� M4�ǰ�� LSS źȯ�� ������ �⺻ ������·� �ڵ����� ���ư���.
					if( isMultipleExtension())		SwapExtensionType();
				}
			}

			addWeaponStateMask( WSM_FIRE_DISABLE);
		}
		else
		{
			if( m_bLoadedShell )
			{
				setWeaponState( WEAPON::STATE_READY);
				removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_EXT_DISABLE);
			}
		}
	}
	else
	{
		INT32 iBullet = getLoadedBulletCount();

		if( iBullet <= 0)
		{
			if( getTotalBulletCount() > 0)
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);		// �ڵ� ������
				removeWeaponStateMask( WSM_LOADMAG_DISABLE );
			}
			else
				setWeaponState( WEAPON::STATE_EMPTY_MAGAZINE);

			addWeaponStateMask( WSM_FIRE_DISABLE);
			bLoaded = false;
		}
		else
		{
			removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_EXT_DISABLE);

			if( iBullet < GetLoadBulletWithCashItem() || getLoadedBulletDualCount() < GetLoadBulletWithCashItem() )
				removeWeaponStateMask( WSM_LOADMAG_DISABLE);
			else
				addWeaponStateMask( WSM_LOADMAG_DISABLE);

			if( isWeaponStateMask( WSM_FIRING) == false)
			{	// �߻����� �ƴϸ�
				if(getWeaponInfo()->GetPumpAction() == false)	//	�����׼� ����̸�
				{
					setWeaponState( WEAPON::STATE_READY);
					removeWeaponStateMask( WSM_FIRE_DISABLE);
				}
			}
			else
			{
				INT32 autoCnt = 0;
				INT32 autopointshot = 0;

				if (getOwner()->isCharaStateMask(CHARA_STATE_ZOOM) == false)
					autoCnt = m_pWeaponInfo->GetAutoFireCount();
				else
					autoCnt = m_pWeaponInfo->GetExtAutoFireCount();

				if (getOwner()->isCharaStateMask(CHARA_STATE_ZOOM) == false)
					autopointshot = m_pWeaponInfo->GetAutoPointShotEnable();
				else
					autopointshot = m_pWeaponInfo->GetExtAutoPointShotEnable();

				switch( autoCnt)
				{
					case 0 :		// ����
						setWeaponState( WEAPON::STATE_NEEDTO_FIRE);

						removeWeaponStateMask( WSM_FIRE_DISABLE);

#if defined( FIREDELAY_COST)
						addWeaponStateMask( WSM_MORE_FIRING);

						// Idle ���¿��� �߻� üũ�� Ÿ�̸Ӹ� ����Ѵ�.
						// 60fps�� ������ ������ ����Ͽ� Idle Ÿ���� ���
						m_rFireTime = m_rFireTime - (0.016f - tm);
#endif
						break;

					case 1 :		// �ܹ�
						setWeaponState( WEAPON::STATE_READY);

						addWeaponStateMask( WSM_FIRE_DISABLE);
						break;

					default :		// ����
						if (autopointshot != 0)
						{
							ProcessAutoPoint(tm);
						}
						else
						{
							// ���� ��...
							if (isWeaponStateMask(WSM_WAIT_AUTOFIREDELAY))
							{
								// ���� ������ ������ ��ٸ��� ����.
								m_timeAutoFire += tm;
								addWeaponStateMask(WSM_FIRE_DISABLE);

								if (m_timeAutoFire >= (m_pWeaponInfo->GetAutoFireDelay() - tm))
								{
									m_timeAutoFire = 0.0f;
									setAutoFireCount(0);
									setWeaponState(WEAPON::STATE_NEEDTO_FIRE);
									removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);
								}
							}
							else
							{
								// �߻��Ѵ�.
								if (getAutoFireCount() > autoCnt)
								{
									// �߻� ������ ���� ����ŭ...
									addWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);

									m_timeAutoFire = 0.0f;
									setWeaponState(WEAPON::STATE_WAIT);
								}
								else
								{
									setWeaponState(WEAPON::STATE_NEEDTO_FIRE);
								}
							}
							removeWeaponStateMask(WSM_FIRE_DISABLE);
						}
						break;
				}
			}
		}
	}
}

void WeaponShotGunLauncher::OnShellFire( i3AIContext * pCtx, REAL32 tm)
{
	// extension Info�� �ʿ��մϴ�
	CWeaponInfo * pWeaponInfo = getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	if( m_pOwner == nullptr)	return;

	if( pCtx->getLocalTime() == 0.0f)		
	{
		_OnProcessFireFirstAnim();
	
		SetCurrentAttachHand();
	}
	else
	{
		REAL32 rDelay = pCtx->getLocalTime() + tm;

		_OnProcessFireMiddleAnim( rDelay);

		if( pWeaponInfo->isOverFireDelay(rDelay))
		{// �߻� �����ð��� ����� �����ٸ�...
			_OnProcessFireEndAnim( rDelay);
		}
	}    
}

void WeaponShotGunLauncher::OnEndShellFire( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

	if( getWeaponInfo()->GetPumpAction())	//	�����׼� ����̸�
	{
		if( getLoadedShellCount() > 0)
		{
			setWeaponState( WEAPON::STATE_READY);
			m_bLoadedShell = true;
		}
		else
		{
			m_bLoadedShell = false;
			if( getOwner() != nullptr && getOwner()->isLocal())
				addWeaponStateMask( WSM_FIRE_DISABLE);
		}
	}
}

void WeaponShotGunLauncher::OnShellReload( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnShellReload( pCtx, tm);

	REAL32 localTime = pCtx->getLocalTime();

	if( pCtx->getCurrentAIState() == nullptr)
		return;
	
	if( localTime + tm >= pCtx->getCurrentAIState()->getDuration())
	{
		PB_VM_S

		INT32 loadedShell = getLoadedShellCount();
		INT32 totalShell = getTotalShellCount();

		INT32 nNeedShell = (m_pExtWeaponInfo->GetLoadBullet() - loadedShell);	
		INT32 nLoadingShell = ((totalShell - nNeedShell) > 0 ? nNeedShell : totalShell);

		loadedShell += nLoadingShell;
		totalShell -= nLoadingShell;
		m_bLoadedShell = true;

		setLoadedShellCount( loadedShell);
		setTotalShellCount( totalShell);

		PB_VM_E

		_OnPlayWeaponIdleAnim();
	}
}

void WeaponShotGunLauncher::OnEndShellReload( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

	if(getWeaponInfo()->GetPumpAction())	//	�����׼� ����̸�
	{
		if( m_bLoadedShell == false && (getOwner() != nullptr && getOwner()->isLocal()))
			addWeaponStateMask( WSM_FIRE_DISABLE);
	}
}

void WeaponShotGunLauncher::OnExtensionActivate( i3AIContext * pCtx, REAL32 tm)
{
	REAL32 rElaps = pCtx->getLocalTime() + tm;

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	//	������ ����Ǹ�
	if( rElaps > 0.8f )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		addWeaponStateMask( WSM_ACTIVATE_EXTENSION);
	}
	else
	{
		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
	}
}

void WeaponShotGunLauncher::OnExtensionDeactivate( i3AIContext * pCtx, REAL32 tm)
{
	REAL32 rElaps = pCtx->getLocalTime() + tm;

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	//	������ ����Ǹ�
	if( rElaps > 0.8f )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_ACTIVATE_EXTENSION);

		_OnPlayWeaponIdleAnim();
	}
	else
	{
		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
	}
}

REAL32 WeaponShotGunLauncher::GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai )
{
	REAL32 rTimeScale = 1.f;

	// LSS�� ��� ���� ó��
	if( m_pOwner != nullptr && GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate())
	{
		CGameCharaAnimContext * pAnimCtx = m_pOwner->getAnimContext();

		i3AIContext * pAIUpper = pAnimCtx->getAIUpper();

		#if defined( AI_MOVE_TO_WEAPON)
			INT32 nID = getCharaAIIndex( m_pOwner->getAnimSet(), m_pOwner->getActionContext()->getBodyUpper(), ai);
		#else
			INT32 nID = pAnimCtx->getAIUpperID( getAbsoluteWeaponInfo()->GetTypeClass(), getAbsoluteWeaponInfo()->GetNumber(), ai);
		#endif

		if( pAIUpper != nullptr )
		{
			if( nID == -1)
			{
				I3PRINTLOG(I3LOG_NOTICE, "nID == -1");
				return 1.f;
			}
			I3ASSERT( nID < pAIUpper->getAIStateCount());
			i3AIState * pNewAIState = pAIUpper->getAIState( nID );
			if( pNewAIState != nullptr )
			{
				CGameFramework::LoadAIStateAnimation( pNewAIState);
				rTimeScale = pNewAIState->getAnimDuration() / GetFireDelay();
			}
		}
	}

	return rTimeScale;
}

void WeaponShotGunLauncher::_OnProcessFireFirstAnim()
{
	WeaponBase::_OnProcessFireFirstAnim();
}

void WeaponShotGunLauncher::_OnProcessFireEndAnim( REAL32 rDelay)
{
	// LSS�� ��� ���� ó��
	if( GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate())
	{
		// extension Info�� �ʿ��մϴ�
		CWeaponInfo * pWeaponInfo = getWeaponInfo();

		I3ASSERT( m_pOwner != nullptr);
		I3ASSERT( pWeaponInfo != nullptr);

		if( pWeaponInfo->GetPumpAction())	//	�����׼� ����̸�
		{
			if( getLoadedShellCount() > 0 )
			{
				m_bLoadedShell = true;
				removeWeaponStateMask( WSM_FIRE_DISABLE);
			}
			else
			{
				m_bLoadedShell = false;
				if( (m_pOwner->isLocal() || m_pOwner->isAI()))	addWeaponStateMask( WSM_FIRE_DISABLE);
			}

			_OnPlayWeaponIdleAnim();
		}
		else
		{
			if( pWeaponInfo->GetNeedToLoadBullet() && (getLoadedShellCount() > 0))  
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADBULLET);
				removeWeaponStateMask( WSM_LOADBULLET_DISABLE);
			}
			else
			{	
				if( isWeaponStateMask( WSM_FIRING ) ||
					rDelay >= m_pOwner->getAnimContext()->getUpperDuration() )
				{
					_OnPlayWeaponIdleAnim();

					if( pWeaponInfo->GetAutoFireCount() == 0)
					{	// ����, �Ǵ� ����
						removeWeaponStateMask( WSM_FIRE_DISABLE);
					}
				}
				else
				{
					if( getLoadedShellCount() <= 0)	_OnPlayWeaponIdleAnim();
					else
					{
						removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
						setWeaponState( WEAPON::STATE_FIRING);
					}
				}
			}
		}
	}
	// �Ϲ� �ѱ��� ���
	else
	{
		WeaponBase::_OnProcessFireEndAnim( rDelay);
	}
}


INT32 WeaponShotGunLauncher::_OnProcessFireBulletCount()
{
	PB_VM_S

		// LSS�� ��� ���� ó��
		if( GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate())
		{
			INT32 loadedShell = getLoadedShellCount();

			loadedShell -= m_pWeaponInfo->GetFireDecBulletCount();

			if( loadedShell < 0)
				loadedShell = 0;

			setLoadedShellCount( loadedShell);

			return loadedShell;
		}
		else
		{
			INT32 iBullet = getLoadedBulletCount();

			iBullet -= m_pWeaponInfo->GetFireDecBulletCount();

			if( iBullet < 0)
				iBullet = 0;

			setLoadedBulletCount( iBullet);

			return iBullet;
		}

		PB_VM_E
}

void WeaponShotGunLauncher::_OnProcessFireMiddleAnim( REAL32 rDelay)
{
	// LSS�� ��� ���� ó��
	if( GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate())
	{
		// extension Info�� �ʿ��մϴ�
		CWeaponInfo * pWeaponInfo = getWeaponInfo();

		I3ASSERT( m_pOwner != nullptr);

		// LSS �߻� ź�� ����
		if( !isWeaponStateMask( WSM_EMPTY_CARTRIDGE) && (m_idxLSSBulletEffect != -1) && (m_idxLauncherFXBullet != -1) )
		{
			if( pWeaponInfo->GetPumpAction() && rDelay > 0.7f )		
				addWeaponStateMask( WSM_EMPTY_CARTRIDGE);
			else if ( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SHOTGUN &&
				pWeaponInfo->GetNumber() == GetItemIndex( "SHOTGUN_M26_LSS") && rDelay > 0.7f)
				addWeaponStateMask( WSM_EMPTY_CARTRIDGE);

			if( isWeaponStateMask( WSM_EMPTY_CARTRIDGE) )
			{
				MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( m_idxLauncherFXBullet);
				I3TRACE( "LSS : %f %f %f\n", i3Matrix::GetPos( pMtx)->x, i3Matrix::GetPos( pMtx)->y, i3Matrix::GetPos( pMtx)->z);

				_EmptyCartridgeEffect( m_idxLauncherFXBullet, m_idxLSSBulletEffect);
				I3ASSERT( m_pOwner->getMoveContext() != nullptr);
				_AddEmptyCartridgeSound( m_pOwner->getMoveContext()->getFloorTerrainType(), DELAY_EMPTYBULLETSOUND, i3Matrix::GetPos( pMtx) );
			}
		}
	}
	// �Ϲ� �ѱ��� ���
	else
	{
		WeaponBase::_OnProcessFireMiddleAnim( rDelay);
	}
}

void WeaponShotGunLauncher::_OnPlayWeaponFireAnim(void)
{		
	if( GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate())
	{
		INT8 aiiFire = -1;

		if( (m_aiiFireShell1PV != -1) && (m_aiiFireShell3PV != -1))		// �켱
		{
			if(  m_pOwner != nullptr && m_pOwner->is1PV())			aiiFire = m_aiiFireShell1PV;
			else if(  m_pOwner != nullptr && m_pOwner->is3PV())	aiiFire = m_aiiFireShell3PV;
			else if( getAIIndex( WEAPON::AI_FIRE) != -1)		aiiFire = m_aiiFireShell;
		}
		else
		{
			aiiFire = m_aiiFireShell;
		}
		
		// ������ ��� �߻� �ӵ� ������ ����
		I3ASSERT( aiiFire != -1);
		i3AIState * pAIState = m_pAICtx->getAIState( aiiFire);
		I3ASSERT( pAIState != nullptr);
		REAL32 rDelay = GetFireDelay();
		I3ASSERT( rDelay != 0.f);
		REAL32 rTimeScale = pAIState->getDuration() / rDelay;
		WF::PlayWeaponAnim( this, aiiFire, rTimeScale );
	}
	else
	{
		if( ( getAIIndex( WEAPON::AI_FIRE1PV) != -1) && (getAIIndex( WEAPON::AI_FIRE3PV) != -1))		// �켱
		{
			if(  m_pOwner != nullptr && m_pOwner->is1PV())
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE1PV) );
			else if(  m_pOwner != nullptr && m_pOwner->is3PV())
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE3PV) );
			else if( getAIIndex( WEAPON::AI_FIRE) != -1)
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE) );
		}
		else
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE) );
		}
	}
}

void WeaponShotGunLauncher::_OnEmitFireEffect( void)
{
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )	_FireEffect( getLauncherFXMatrix(), m_idxShellFireEffect);
	else						_FireEffect( getFXMatrix(), getFireEffectIndex());
}

void WeaponShotGunLauncher::_OnPlayFireSound( void)
{
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )	_PlayFireSound( WEAPON::FIRE_SOUND_EXT);			//	LSS ����
	else						_PlayFireSound( WEAPON::FIRE_SOUND_NORMAL);		//	�Ϲ� ����
}

//====================================================================================================//
// ��ȣȭ 2010.08.11 ����

INT32 WeaponShotGunLauncher::getTotalShellCount(void)
{
	return GlobalFunc::DecryptValue( &m_nTotalShell, m_u8EncryptKey);
}

void WeaponShotGunLauncher::setTotalShellCount( INT32 nCount)
{
	GlobalFunc::EncryptValue( &nCount, m_u8EncryptKey);
	m_nTotalShell = nCount;
}

INT32 WeaponShotGunLauncher::getLoadedShellCount(void)
{
	//return GlobalFunc::DecryptValue( &m_nLoadedShell, m_u8EncryptKey);
	return m_nLoadedShell;
}

void WeaponShotGunLauncher::setLoadedShellCount( INT32 nCount)
{
	//GlobalFunc::EncryptValue( &nCount, m_u8EncryptKey);
	m_nLoadedShell = nCount;
}

void WeaponShotGunLauncher::GetNetBullet(GAMEINFO_BULLET * pBullet)
{
	pBullet->Reset();
	pBullet->SetLoadBulletCount(	WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT, (UINT8)getLoadedBulletCount());
	pBullet->SetStandbyBulletCount(	WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT, (UINT16)getTotalBulletCount());
	pBullet->SetLoadBulletCount(	WEAPON::EXTENSION_ON,  WEAPON::HAND_TYPE_LEFT, (UINT8)getLoadedShellCount());
}

void WeaponShotGunLauncher::SetNetBullet(GAMEINFO_BULLET * pBullet)
{
	setLoadedBulletCount( pBullet->GetLoadBulletCount(	  WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT));
	setTotalBulletCount(  pBullet->GetStandbyBulletCount( WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT));
	m_nLoadedShell =	  pBullet->GetLoadBulletCount(	  WEAPON::EXTENSION_ON,  WEAPON::HAND_TYPE_LEFT);
}
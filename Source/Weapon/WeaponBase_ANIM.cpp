#include "pch.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "GameCharaAnimContext.h"
#include "GameCharaMoveContext.h"
#include "BattleSlotContext.h"

//#define FIREDELAY_COST

///////////////////////////////////////////////////////////////////////////////////////////
void WeaponBase::OnIdle( i3AIContext * pCtx, REAL32 tm)
{	
	if( m_pOwner == nullptr)	return;
	
	bool bLoaded = true;

	if( pCtx->getLocalTime() == 0.0f)
	{
		SetCurrentAttachHand();
		
		if (g_pFramework->IsBattleStage() == true && m_pOwner->is1PV())
		{
			if (getDropInfo()->_bDropping == false && getDropInfo()->_bDroped == false)
			{
				// �ΰ��� �� ��쿡�� ���带 ����մϴ�.
				// ������̰ų� ����Ǿ��� ���� �ƴ� ��쿡�� ����
				_PlayIdleSound();
			}
		}
	}

	// �ڵ� ������
	INT32 iBullet = getLoadedBulletCount();

	if( iBullet <= 0)
	{
		if( getTotalBulletCount() > 0)
		{
			setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);		// �ڵ� ������
			removeWeaponStateMask( WSM_LOADMAG_DISABLE);
		}
		else
			setWeaponState( WEAPON::STATE_EMPTY_MAGAZINE);

		if( m_pOwner->isLocal()) addWeaponStateMask( WSM_FIRE_DISABLE);
		bLoaded = false;
	}

	// �ε�Ÿ���� ���� �ϰ�� źâ�� ���ε� 
	if(	m_pOwner != nullptr
		&& getWeaponInfo()->GetGaugeReload() == 1 )
	{
//		INT32 iBullet = getLoadedBulletCount();
		INT32 iTotal = getTotalBulletCount();
		INT32 iMaxBullet = GetLoadBulletWithCashItem();

		if(	(iBullet < iMaxBullet) && (iTotal > 0))
		{
			if( getWeaponInfo()->IsLoadMagazineReady() 
				&& isReadyToLoadMagazine() == false)
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);
				addWeaponStateMask( WSM_READY_TO_LOADMAG);
			}
		}
	}

	if( bLoaded )
	{
		if(getWeaponInfo()->GetPumpAction())	//	�����׼��̸�
		{
			addWeaponStateMask( WSM_LOADED_BULLET);
			removeWeaponStateMask( WSM_FIRE_DISABLE);
		}
		else
		{
			removeWeaponStateMask( WSM_FIRE_DISABLE);
		}

		if( iBullet < GetLoadBulletWithCashItem() )	removeWeaponStateMask( WSM_LOADMAG_DISABLE);
		else if( m_pOwner->isLocal())			addWeaponStateMask( WSM_LOADMAG_DISABLE);

		if( isWeaponStateMask( WSM_FIRING) == false)
		{	// �߻����� �ƴϸ�
			if(getWeaponInfo()->GetPumpAction() == false)
			{
				setWeaponState( WEAPON::STATE_READY);
				removeWeaponStateMask( WSM_FIRE_DISABLE
#if defined( FIREDELAY_COST)
					| WSM_MORE_FIRING
#endif
					);
			}
		}
		else
		{
			INT32 autoCnt;
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
				if( m_pOwner->isLocal()) addWeaponStateMask( WSM_FIRE_DISABLE);
				break;

			default :		// ����
				// ���� ��...
				if (autopointshot != 0)
				{
					ProcessAutoPoint(tm);
				}
				else
				{
					if (isWeaponStateMask(WSM_WAIT_AUTOFIREDELAY))
					{
						// ���� ������ ������ ��ٸ��� ����.
						m_timeAutoFire += tm;
						if (m_pOwner->isLocal()) addWeaponStateMask(WSM_FIRE_DISABLE);

						if (m_timeAutoFire >= (m_pWeaponInfo->GetAutoFireDelay() - tm))
						{
							m_timeAutoFire = 0.0f;
							setAutoFireCount(0);
							setWeaponState(WEAPON::STATE_NEEDTO_FIRE);
							removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE );
						}
					}
					else
					{
						// �߻��Ѵ�.
						if (getAutoFireCount() > autoCnt)
						{
							// �߻� ������ ���� ����ŭ...
							if (m_pOwner->isLocal()) addWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);
							m_timeAutoFire = 0.0f;
							setWeaponState(WEAPON::STATE_WAIT);
						}
						else
							setWeaponState(WEAPON::STATE_NEEDTO_FIRE);
					}
					removeWeaponStateMask(WSM_FIRE_DISABLE);
				}
				break;
			}				
		}
	}
}

void WeaponBase::ProcessAutoPoint(REAL32 tm)
{
	INT32 autoCnt = 0;

	if (getRemainAutoFireCount() != 0)
	{
		autoCnt = getRemainAutoFireCount();
	}
	else
	{
		if (getOwner()->isCharaStateMask(CHARA_STATE_ZOOM) == false)
			autoCnt = m_pWeaponInfo->GetAutoFireCount();
		else
			autoCnt = m_pWeaponInfo->GetExtAutoFireCount();

	}

	if (isWeaponStateMask(WSM_POINTSHOTING) && getAutoFireCount() <= autoCnt)
	{
		if (m_rFireTime >= m_pWeaponInfo->GetFireDelay())
			m_pOwner->Cmd_Attack();

		m_rFireTime = m_rFireTime + tm;

		if (getAutoFireCount() > autoCnt)
		{
			m_timeAutoFire = 0.0f;
			setWeaponState(WEAPON::STATE_WAIT);
		}
	}
	else if (getAutoFireCount() > autoCnt)
	{
		m_timeAutoFire += tm;
		addWeaponStateMask(WSM_FIRE_DISABLE);

		if (m_timeAutoFire >= (m_pWeaponInfo->GetAutoFireDelay() - tm))
		{
			addWeaponStateMask(WSM_FIRE_DISABLE);
			removeWeaponStateMask(WSM_POINTSHOTING);
			m_timeAutoFire = 0.0f;
		}
	}
}

void WeaponBase::OnEndIdle(i3AIContext * pCtx, REAL32 tm)
{
	I3ASSERT(m_pWeaponInfo != nullptr);

	if (m_pOwner == nullptr)	return;

	_StopIdleSound();
}

void WeaponBase::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	if( m_pOwner == nullptr)	return;

	if( pCtx->getLocalTime() == 0.0f)
	{
		SetCurrentAttachHand();

		_OnProcessFireFirstAnim();
#if defined( FIREDELAY_COST)
		m_rFireTime = m_rFireTime + tm;
#endif
	}
	else
	{	
		//REAL32 rDelay = pCtx->getLocalTime() + tm;
#if defined( FIREDELAY_COST)
		m_rFireTime = m_rFireTime + tm;
#else
		m_rFireTime = pCtx->getLocalTime() + tm;
#endif	

		_OnProcessFireMiddleAnim( m_rFireTime);

		REAL32 rDelay = GetFireDelay();
		rDelay = MINMAX(0.06f, rDelay, 3.f);

		if( m_rFireTime > rDelay )
		{// �߻� �����ð��� ����� �����ٸ�...
			_OnProcessFireEndAnim( pCtx->getLocalTime() + tm);

			// ������Ų��.
#if defined( FIREDELAY_COST)
			m_rFireTime = m_rFireTime - rDelay;
#endif
		}
	}
}

void WeaponBase::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE
#if defined( FIREDELAY_COST)
		| WSM_MORE_FIRING
#endif
		);

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	if(getWeaponInfo()->GetPumpAction())	//	�����׼� ����̸�
	{
		if( getLoadedBulletCount() > 0)
		{
			setWeaponState( WEAPON::STATE_READY);
			addWeaponStateMask( WSM_LOADED_BULLET);
		}
		else
		{
			removeWeaponStateMask( WSM_LOADED_BULLET);
			if( getOwner() != nullptr && getOwner()->isLocal())
				addWeaponStateMask( WSM_FIRE_DISABLE);
		}
	}

	m_mainWeapon->OnEndFire(pCtx, tm);
}

void WeaponBase::OnExtensionActivate( i3AIContext * pCtx, REAL32 tm)
{
	if( getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE) == -1)
		return;

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	i3AIState * pAI = pCtx->getAIState( getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE));

	//	������ ����Ǹ�
	if( pCtx->getLocalTime() + tm >= pAI->getDuration() ||
		(m_pWeaponInfo->GetTypeZoom() > WEAPON::ZOOM_NONE && pCtx->getLocalTime() + tm >= 0.5f) )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		addWeaponStateMask( WSM_ACTIVATE_EXTENSION);

		_OnPlayWeaponIdleAnim();
	}
	else
	{
		if( (m_pOwner != nullptr) && m_pOwner->isLocal())
		{
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}

		removeWeaponStateMask( WSM_ACTIVATE_EXTENSION);
	}

	OnSetVisibleExtension( true);
}

void WeaponBase::OnExtensionDeactivate( i3AIContext * pCtx, REAL32 tm)
{
	if( getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE) == -1)
		return;

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	i3AIState * pAI = pCtx->getAIState( getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE));

	REAL32 rElaps = pCtx->getLocalTime() + tm;
	//	������ ����Ǹ�
	if( rElaps >= pAI->getDuration() ||
		(m_pWeaponInfo->GetTypeZoom() > WEAPON::ZOOM_NONE && rElaps + tm >= 0.5f) )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_ACTIVATE_EXTENSION);

		_OnPlayWeaponIdleAnim();

		OnSetVisibleExtension( false);
	}
	else
	{
		if( getOwner() != nullptr && getOwner()->isLocal())
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		addWeaponStateMask( WSM_ACTIVATE_EXTENSION);
	}
}

bool WeaponBase::_OnNormalReload(i3AIContext * pCtx, REAL32 tm)
{
	bool bDoneLoadMag = false;

	REAL32 rReloadTime = 1.f;

	i3AIState * pAIState = pCtx->getCurrentAIState();

	if (isDualGun() && getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL) != -1)
		pAIState = pCtx->getAIState(getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL));

	if (m_pPairWeapon != nullptr &&  getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL) != -1)
		pAIState = pCtx->getAIState(getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL));

	if (pAIState != nullptr)
		rReloadTime = pAIState->getActualDuration();

	//	ź�� Effect. źâ�� �Ϲ� �ѱⰡ �ƴ� ��Ʈ �׼ǽ� �ѱ⸸ (��Ʈ�׼� �ѱ�)
	if ((pCtx->getLocalTime() > 2.f) && !isWeaponStateMask(WSM_EMPTY_CARTRIDGE) &&
		getWeaponInfo()->GetNeedToLoadBullet() && (getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER) &&
		(m_idxBulletEffect != -1) && (m_idxFXBullet != -1))
	{
		_EmptyCartridgeEffect(m_idxFXBullet, m_idxBulletEffect);
		addWeaponStateMask(WSM_EMPTY_CARTRIDGE);

		MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix(m_idxFXBullet);
		_AddEmptyCartridgeSound(m_pOwner->getMoveContext()->getFloorTerrainType(), 0.8f, i3Matrix::GetPos(pMtx));
	}


	if ((getWeaponInfo()->GetGaugeReload() == 0)
		&& (pCtx->getLocalTime() + tm > rReloadTime))
	{
		if (getWeaponInfo()->GetLoadMagToLoadBullet() == false)
			AddNomalBullet();
	}
	else if (getWeaponInfo()->GetGaugeReload() == 1)
	{
		AddPumpBullet(tm);
	}

	if (pCtx->getLocalTime() + tm > rReloadTime)
	{

		bDoneLoadMag = true;

		INT32 iBullet = getLoadedBulletCount();
		INT32 iTotal = getTotalBulletCount();
		INT32 iMaxBullet = GetLoadBulletWithCashItem();

		// Load Magaine ���Ŀ� ���� �Ǿ� �� �ϴ� ���� 
		// Bullet���� ó�� �ϵ��� �����ؾ� �մϴ�.
		if (getWeaponInfo()->GetLoadMagToLoadBullet() == true)
		{
			if (m_pWeaponInfo->GetNeedToLoadBullet())
			{
				setWeaponState(WEAPON::STATE_NEEDTO_LOADBULLET);
				removeWeaponStateMask(WSM_LOADBULLET_DISABLE);

				addWeaponStateMask(WSM_LOADMAG_TO_NEEDBULLET);
			}
		}
		// �������������� ���� ������ źâ�� ä������ �ʾҴٸ�
		// �ٽ� Load Mag�� �䱸�Ѵ�.
		// ������ ����Ǵ� ��ǥ���� ��.
		else if ((iBullet < iMaxBullet) && (iTotal > 0))
		{
			setWeaponState(WEAPON::STATE_NEEDTO_LOADMAG);
		}
		else
		{
			//	�����׼� ��İ� Mag�ϰ� �� �ڿ� LoadBullet �� �ʿ��� ���� �� ��쿡�� ����մϴ�.
			if (getWeaponInfo()->GetPumpAction())
			{
				if (m_pWeaponInfo->GetNeedToLoadBullet())
				{
					setWeaponState(WEAPON::STATE_NEEDTO_LOADBULLET);
					removeWeaponStateMask(WSM_LOADBULLET_DISABLE);
				}
			}
			else
			{
				// �� �̻� ��ź�� �Ѿ��� ����.
				// �׳� Idle ���·� ������.
				setWeaponState(WEAPON::STATE_READY);
				_OnPlayWeaponIdleAnim();
			}

			//�����ϼ��� �־ ���� ó�� �մϴ�. 
			iMaxBullet = MIN(iBullet, iMaxBullet);
			setLoadedBulletCount(iMaxBullet);
		}

		removeWeaponStateMask(WSM_LOADMAG_DISABLE);
		addWeaponStateMask(WSM_WAIT_FOR_LOADMAG);
	}

	return bDoneLoadMag;
}

bool WeaponBase::_OnLoopAnimationReload(i3AIContext * pCtx, REAL32 tm)
{
	bool bDoneLoadMag = false;
	
	// Time�� ���� ����մϴ�.
	m_rReloadTime += tm;

	REAL32 rWeaponReloadTime = getWeaponInfo()->GetReloadTime();

	// ���⿡ ������ �ӵ� ���� ���� ��.
	REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);
	rWeaponReloadTime -= (rWeaponReloadTime * ratio);

	//	ź�� Effect. źâ�� �Ϲ� �ѱⰡ �ƴ� ��Ʈ �׼ǽ� �ѱ⸸ (��Ʈ�׼� �ѱ�)
	if ((pCtx->getLocalTime() > 2.f) && !isWeaponStateMask(WSM_EMPTY_CARTRIDGE) &&
		getWeaponInfo()->GetNeedToLoadBullet() && (getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER) &&
		(m_idxBulletEffect != -1) && (m_idxFXBullet != -1))
	{
		_EmptyCartridgeEffect(m_idxFXBullet, m_idxBulletEffect);
		addWeaponStateMask(WSM_EMPTY_CARTRIDGE);

		MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix(m_idxFXBullet);
		_AddEmptyCartridgeSound(m_pOwner->getMoveContext()->getFloorTerrainType(), 0.8f, i3Matrix::GetPos(pMtx));
	}
	
	if ((getWeaponInfo()->GetGaugeReload() == 0)
		&& (m_rReloadTime > rWeaponReloadTime))
	{
		if (getWeaponInfo()->GetLoadMagToLoadBullet() == false)
			AddNomalBullet();
	}
	else if (getWeaponInfo()->GetGaugeReload() == 1)
	{
		AddPumpBullet(tm);
	}

	if (m_rReloadTime > rWeaponReloadTime)
	{

		bDoneLoadMag = true;

		INT32 iBullet = getLoadedBulletCount();
		INT32 iTotal = getTotalBulletCount();
		INT32 iMaxBullet = GetLoadBulletWithCashItem();

		// Load Magaine ���Ŀ� ���� �Ǿ� �� �ϴ� ���� 
		// Bullet���� ó�� �ϵ��� �����ؾ� �մϴ�.
		if (getWeaponInfo()->GetLoadMagToLoadBullet() == true)
		{
			if (m_pWeaponInfo->GetNeedToLoadBullet())
			{
				setWeaponState(WEAPON::STATE_NEEDTO_LOADBULLET);
				removeWeaponStateMask(WSM_LOADBULLET_DISABLE);

				addWeaponStateMask(WSM_LOADMAG_TO_NEEDBULLET);
			}
		}
		// �������������� ���� ������ źâ�� ä������ �ʾҴٸ�
		// �ٽ� Load Mag�� �䱸�Ѵ�.
		// ������ ����Ǵ� ��ǥ���� ��.
		else if ((iBullet < iMaxBullet) && (iTotal > 0))
		{
			setWeaponState(WEAPON::STATE_NEEDTO_LOADMAG);
		}
		else
		{
			//	�����׼� ��İ� Mag�ϰ� �� �ڿ� LoadBullet �� �ʿ��� ���� �� ��쿡�� ����մϴ�.
			if (getWeaponInfo()->GetPumpAction())
			{
				if (m_pWeaponInfo->GetNeedToLoadBullet())
				{
					setWeaponState(WEAPON::STATE_NEEDTO_LOADBULLET);
					removeWeaponStateMask(WSM_LOADBULLET_DISABLE);
				}
			}
			else
			{
				// �� �̻� ��ź�� �Ѿ��� ����.
				// �׳� Idle ���·� ������.
				setWeaponState(WEAPON::STATE_READY);
				_OnPlayWeaponIdleAnim();
			}

			//�����ϼ��� �־ ���� ó�� �մϴ�. 
			iMaxBullet = MIN(iBullet, iMaxBullet);
			setLoadedBulletCount(iMaxBullet);
		}

		removeWeaponStateMask(WSM_LOADMAG_DISABLE);
		addWeaponStateMask(WSM_WAIT_FOR_LOADMAG);
	}

	return bDoneLoadMag;
}

bool WeaponBase::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bDoneLoadMag = false;

	if( m_pOwner == nullptr) 
		return false;

	if( pCtx->getLocalTime() == 0.0f)
	{
		if( m_pOwner->isLocal())
			addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_FIRE_DISABLE);
		setWeaponState( WEAPON::STATE_LOADMAG);

		m_rReloadTime = 0.f;
		removeWeaponStateMask( WSM_WAIT_FOR_LOADMAG | WSM_EMPTY_CARTRIDGE);

		if(( getWeaponInfo()->GetTypeTrigger() == WEAPON::TRIGGER_RELOADING_SHOT) &&
			isWeaponStateMask( WSM_LOADED_BULLET) )
		{
			removeWeaponStateMask( WSM_FIRE_DISABLE);
		}

		if( GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER &&
			isExtensionActivate() )
		{
			// none
		}
		else
		{
			// KEYWORD : WEAPON_DUMMY
			if( IsLoadMagazineLeftBarrel() )
			{
				ChangeAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND, WEAPON_ATTACH_BARREL);
			}
		}
	}

	if( isWeaponStateMask(WSM_WAIT_FOR_LOADMAG) == false)
	{
		// Loop�� Non Loop�� �����մϴ�.
		if (isReloadLoopAnimation())
		{
			bDoneLoadMag = _OnLoopAnimationReload(pCtx, tm);
		}
		else
		{
			bDoneLoadMag = _OnNormalReload(pCtx, tm);
		}
	}

	return bDoneLoadMag;
}

void WeaponBase::AddNomalBullet()
{
	INT32 addCount;

	INT32 iBullet = getLoadedBulletCount();
	INT32 iTotal = getTotalBulletCount();

	PB_VM_S

		addCount = MIN( m_pWeaponInfo->GetReloadBulletCount(), iTotal);

	if((iBullet + addCount) > GetLoadBulletWithCashItem())
		addCount -= (iBullet + addCount) - GetLoadBulletWithCashItem();

	iBullet += addCount;

	setLoadedBulletCount( iBullet);

#ifdef ENABLE_TEST_COMMAND
	if( !g_pWeaponManager->COMMAND_IsUnlimitGun())		//	Use Cheat
#endif
	{
		iTotal -= addCount;
	}

	setTotalBulletCount( iTotal);

	PB_VM_E
}

void WeaponBase::AddPumpBullet(REAL32 tm)
{
	REAL32 addCount = 0.f;
	REAL32 iBullet = getLoadedBulletCountF();
	REAL32 iTotal = static_cast<REAL32>(getTotalBulletCount());
	INT32 iMaxBullet = GetLoadBulletWithCashItem();
	REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);

	addCount = MIN( m_pWeaponInfo->GetReloadBulletCount(), iTotal);
	addCount = (tm / (addCount));
	addCount += (addCount*ratio);

	iBullet += addCount;


	//iBullet = MIN( iBullet, iTotal);

	if( iMaxBullet <= iBullet)
		SetEndurance(100);
	setLoadedBulletCountF( iBullet);

	//setTotalBulletCount( iTotal);


}
void WeaponBase::OnEndLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

	if(getWeaponInfo()->GetPumpAction())	//	�����׼� ����̸�
	{
		if( (isWeaponStateMask( WSM_LOADED_BULLET) == false) && (getOwner() != nullptr && getOwner()->isLocal()))
			addWeaponStateMask( WSM_FIRE_DISABLE);
	}

	SetCurrentAttachHand();

	m_mainWeapon->OnEndLoadMag(pCtx, tm);
}

void WeaponBase::OnLoadBullet( i3AIContext * pCtx, REAL32 tm)
{	
	i3AIState * pLoadBullet = pCtx->getCurrentAIState();

	I3ASSERT( pLoadBullet != nullptr);


	if( pCtx->getLocalTime() == 0.f )
	{
		if( getOwner() != nullptr && getOwner()->isLocal())
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		// KEYWORD : WEAPON_DUMMY
		if( IsLoadBulletLeftBarrel() )
		{	// MG�� ź�� �����ÿ��� �޼� ����
			ChangeAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND, WEAPON_ATTACH_BARREL);
		}		
	}

	//	ź�� Effect. źâ�� �Ϲ� �ѱⰡ �ƴ� ��Ʈ �׼ǽ� �ѱ⸸ (��Ʈ�׼� �ѱ�)
	if( (getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER) &&
		(pCtx->getLocalTime() > 0.5f) && !isWeaponStateMask( WSM_EMPTY_CARTRIDGE) && getWeaponInfo()->GetNeedToLoadBullet() &&
		(m_idxFXBullet != -1) && (m_idxBulletEffect != -1))			
	{
		_EmptyCartridgeEffect( m_idxFXBullet, m_idxBulletEffect);
		addWeaponStateMask( WSM_EMPTY_CARTRIDGE);

		MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( m_idxFXBullet);
		_AddEmptyCartridgeSound( m_pOwner->getMoveContext()->getFloorTerrainType(), DELAY_EMPTYBULLETSOUND, i3Matrix::GetPos( pMtx) );
	}
	
	if( pCtx->getLocalTime() > pLoadBullet->getActualDuration())
	{
		addWeaponStateMask( WSM_LOADED_BULLET);

		// �� ��ġ���� źâ �߰�!
		if (isWeaponStateMask(WSM_LOADMAG_TO_NEEDBULLET) == true)
		{
			// ź ���� ä��ϴ�.
			AddNomalBullet();

			INT32 iBullet = getLoadedBulletCount();
			INT32 iTotal = getTotalBulletCount();
			INT32 iMaxBullet = GetLoadBulletWithCashItem();

			// �������������� ���� ������ źâ�� ä������ �ʾҴٸ�
			// �ٽ� Load Mag�� �䱸�Ѵ�.
			// ������ ����Ǵ� ��ǥ���� ��.
			if ((iBullet < iMaxBullet) && (iTotal > 0))
			{
				setWeaponState(WEAPON::STATE_NEEDTO_LOADMAG);
			}
			else
			{
				removeWeaponStateMask(WSM_LOADMAG_TO_NEEDBULLET);

				setWeaponState(WEAPON::STATE_READY);

				_OnPlayWeaponIdleAnim();
			}
		}
		else
		{
			setWeaponState(WEAPON::STATE_READY);

			_OnPlayWeaponIdleAnim();
		}
	}
}

void WeaponBase::OnEndLoadBullet( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_FIRE_DISABLE);

	if( pCtx->getLocalTime() == 0.f )
		SetCurrentAttachHand();
}

void WeaponBase::OnEndFireBullet( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_FIRE_DISABLE);

	if( pCtx->getLocalTime() == 0.f )
		SetCurrentAttachHand();
}

void WeaponBase::OnSwaping( i3AIContext * pCtx, REAL32 tm)
{
	I3ASSERT( pCtx != nullptr);

	REAL32 localTime = pCtx->getLocalTime();
	
	i3AIState * pAIState = pCtx->getCurrentAIState();
	I3ASSERT( pAIState != nullptr);

	if( pCtx->getLocalTime() == 0.f )
		SetCurrentAttachHand();

	if( localTime + tm > pAIState->getDuration())
	{
		_OnPlayWeaponIdleAnim();
		setWeaponState( WEAPON::STATE_READY);
		removeWeaponStateMask( WSM_FIRE_DISABLE);
	}
}

void WeaponBase::OnLoadMag_Ready( i3AIContext * pCtx, REAL32 tm)
{
	if( pCtx->getLocalTime() == 0.0f)
	{
		// KEYWORD : WEAPON_DUMMY
		if( IsLoadMagazineLeftBarrel() )
		{
			ChangeAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND, WEAPON_ATTACH_BARREL);
		}		
	}
}

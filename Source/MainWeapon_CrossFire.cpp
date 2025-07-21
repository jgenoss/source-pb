#include "pch.h"

#include "MainWeapon_CrossFire.h"

#include "GameCharaEquipContext.h"
#include "BattleSlotContext.h"
#include "Designation/DesigApplyInfo.h"

MainWeapon_CrossFire::MainWeapon_CrossFire(WeaponBase* p) : MainWeapon_DualHandGun(p)
{
	m_bDualFired	= false;
}

bool	MainWeapon_CrossFire::Fire_CrossFire( WEAPON::FIRE_ORDER type)
{
	if( !m_pWeaponBase->isExtensionActivate() )
	{
		return Fire_DualHandGun( type );
	}
	
	// righthand
	if( m_pWeaponBase->getLoadedBulletCount() <= 0)	return false;

	m_pWeaponBase->SetFireOrder(type);

	m_pWeaponBase->GetShotTargetPos( m_pWeaponBase->GetTargetPos() );	// ������ ���ϰ� �ִ� ��ǥ ������ ���Ѵ�.

	NET_Fire_CrossFire( nullptr, m_pWeaponBase->GetTargetPos(), m_pWeaponBase->getFireOrder() );
	
	// lefthand
	if( m_pWeaponBase->getOwner()->isLocal() )
	{
		m_pWeaponBase->addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
		m_pWeaponBase->removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);
	}

	if( m_pLeftHandWeapon )	m_pLeftHandWeapon->Fire(WEAPON::PRIMARY_FIRE);
	
	return true;

}

void	MainWeapon_CrossFire::NET_Fire_CrossFire( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	if( !m_pWeaponBase->isExtensionActivate() )
	{
		MainWeapon_DualHandGun::NET_Fire_DualHandGun( pStartPos, pTargetPos, type);
		return;
	}

	// revision 29646
	m_bDualFired	= false;

	INT32 iBullet = m_pWeaponBase->getLoadedBulletCount();

	if( iBullet > 1 )		// �Ѿ� �ѹ߳������� ���ʸ� �߻�!!
	{
		m_pWeaponBase->WeaponBase::NET_Fire( pStartPos, pTargetPos, type);		// �����ڸ� ���� �ȵȴ�..(�񰡻����̾����)
		m_bDualFired	= true;
	}
	
	if( m_pLeftHandWeapon)
		m_pLeftHandWeapon->NET_Fire( pStartPos, pTargetPos, type);

	iBullet--;
	m_pWeaponBase->setLoadedBulletCount( iBullet);
	
}


REAL32	MainWeapon_CrossFire::GetRange_CrossFire(void)
{
	if( !m_pWeaponBase->isExtensionActivate() )
		return m_pWeaponBase->WeaponBase::GetRange();		// �����ڸ� ���� �ȵȴ�..(�񰡻����̾����)

	REAL32		rVal = m_pWeaponBase->getWeaponInfo()->GetExtRange();

	// �ƿ� ���Ӽ�/��ų/ȣĪ ������ ���� �����ֱ淡 �����Ѵ�...
	// �Ʒ��� WeaponBase::GetRange���� ��� ���� ���ٵ� �ڵ��̴�...( �ð��� �Ǹ� GetRange()/GetExtRange()���õǾ� �ڵ������� �ʿ���)
	// �̰��� �ǵ�ġ ���� �����ϼ��� �����Ƿ� ������ �ȴٸ� �����ؾ��Ѵ�..�׷���, 12���� ȣĪ������ �ݿ��� ������..
	// 2015.05.22.����...
	if (m_pWeaponBase->getOwner() )
	{
		CGameCharaBase* pOwner = m_pWeaponBase->getOwner();

		if ( pOwner->getEquipContext() )
			rVal += (rVal * pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_RANGE));

		// ��ų �Ӽ�.
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( pOwner->getCharaNetIndex(), pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(m_pWeaponBase->getWeaponInfo()->GetTypeClass(), SKILLLIST_RANGE);
		rVal += ratio;

		// ȣĪ ����
		const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
		if (desig_apply_info)
			rVal += desig_apply_info->desigWeapon.weaponAttackRange;
	}

	return rVal;
}

REAL32	MainWeapon_CrossFire::GetFireDelay_CrossFire(void)
{
	if( !m_pWeaponBase->isExtensionActivate() )
		return m_pWeaponBase->WeaponBase::GetFireDelay();	// �����ڸ� ���� �ȵȴ�..(�񰡻����̾����)

	REAL32 rVal = m_pWeaponBase->getWeaponInfo()->GetExtFireDelay();
	
	if ( m_pWeaponBase->getOwner() )
	{
		m_pWeaponBase->_CalcFireDelayToVersion(rVal);
	}

	return rVal;
}


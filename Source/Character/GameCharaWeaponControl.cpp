#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaWeaponControl.h"
#include "GameCharaMultiWeaponContext.h"

#include "WeaponBase.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "UtilBattle.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "GameMissionManager.h"
#include "GameEvent.H"

#include "MainWeapon_DualCIC.h"
#include "MainWeapon_DualKnife.h"

#include "BattleObjContext.h"
#include "UserInfoContext.h"
#include "BattleSlotContext.h"
#include "WeaponWatermelon.h"
#include "../Weapon/WeaponItemID.h"

#include "../UI/UIHudMultiWeapon.h"
#include "../UI/UIHudManager.h"

I3_CLASS_INSTANCE( CGameCharaWeaponControl);//, CGameCharaControlBase );

bool CGameCharaWeaponControl::_ProcessDropedWeaponSearching( void)
{
	INT32	i;
	VEC3D	vDir;
	REAL32	rLength;
	WeaponBase * pWeapon;

	if( m_pOwner->IsDino()) {
		return false;
	}

	I3ASSERT( g_pWeaponManager != nullptr );
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH)) return false;
	// ������ ���Ⱑ ������ ����
	if( g_pWeaponManager->GetDropListCount() == 0 ) return false;

	if ( m_pOwner->getMultiWeaponContext()->IsPickupProhibit() && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		return false;

	REAL32 rCheckTime = getWeaponContext()->getDropedWeaponCheckTime();

	REAL32 rCheckTimeSync = 1.0f;
	if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		rCheckTimeSync = 2.0f;

	rCheckTime += g_pViewer->getDeltaTime();
	if( rCheckTime > (WEAPON_CHECKTAKING_TIME * rCheckTimeSync))
	{
		getWeaponContext()->setDropedWeaponCheckTime( 0.f);

		// �Ϲ������� �ֹ��� ������ ��� �ִ��� Ȯ��
		// ���߹��� �ý����� ����� ��� ���� ���� �ִ�ġ�� ������ �ȵ�(AI�� ���߹��⸦ ����� �� ����)
		if ( ( getWeapon(WEAPON_SLOT_PRIMARY) == nullptr && !MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			|| ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && m_pOwner->getMultiWeaponContext()->CheckEmptySlot() && !m_pOwner->isAI()))
		{
			for ( i = 0; i < g_pWeaponManager->GetDropListCount(); i++)
			{
				pWeapon = g_pWeaponManager->GetDropWeapon(i);
				if ( !pWeapon->IsDroped())
					continue;

				if ( pWeapon->getWeaponInfo()->GetTypeUsage() != WEAPON_SLOT_PRIMARY)
					continue;

				GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( static_cast<INT32>( pWeapon->getNetworkIdx()));
				if ( pInfo->getState() != NET_WEAPONDROPED_SLEEP)
					continue;

				if ( pWeapon->getTotalBulletCount() > 0 ||
					pWeapon->getLoadedBulletCount() > 0 ||
					pWeapon->getLoadedBulletDualCount() > 0)
				{
					i3Vector::Sub( &vDir, pWeapon->GetPos(), m_pOwner->GetPos());
					rLength = i3Vector::LengthSq( &vDir);
					if ( rLength < 1.f)
					{
						// Pickup weapon
						GAMEEVENT::Write_Chara_PickupWeapon(m_pOwner, pWeapon);
						return true;
					}
				}
			}
		}

		// ���� ���� ������ ����ִ�. üũ
		if( getWeapon( WEAPON_SLOT_SECONDARY) == nullptr )
		{
			for( i = 0; i < g_pWeaponManager->GetDropListCount(); i++ )
			{
				pWeapon = g_pWeaponManager->GetDropWeapon( i );
				if( !pWeapon->IsDroped())
					continue;

				if( pWeapon->getWeaponInfo()->GetTypeUsage() != WEAPON_SLOT_SECONDARY)
					continue;

				GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon(static_cast<INT32>(pWeapon->getNetworkIdx()));
				if (pInfo->getState() != NET_WEAPONDROPED_SLEEP)
					continue;

				if( pWeapon->getTotalBulletCount() > 0 ||
					pWeapon->getLoadedBulletCount() > 0 )
				{
					i3Vector::Sub( &vDir, pWeapon->GetPos(), m_pOwner->GetPos() );
					rLength = i3Vector::LengthSq( &vDir );
					if( rLength < 1.f )
					{	
						// Pickup weapon
						GAMEEVENT::Write_Chara_PickupWeapon( m_pOwner, pWeapon);
						return true;
					}
				}
			}
		}
	}
	else
	{
		getWeaponContext()->setDropedWeaponCheckTime( rCheckTime);
	}

	return false;
}


bool CGameCharaWeaponControl::_ProcessFireForNetwork( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER fireOrder)
{
	if( getCurrentWeapon() == nullptr )		return false;

	CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	CGameMissionManager * pMissionManager = CGameMissionManager::i();

	//	�� ����
	switch( pWeaponInfo->GetTypeClass())
	{			
	case WEAPON_CLASS_KNIFE:
	case WEAPON_CLASS_KNUCKLE:
		{	//	�������� ��� ���� ó��
			if( i3::same_of<WeaponKnife*>(getCurrentWeapon()))
			{
				WeaponKnife * pWeaponKnife = (WeaponKnife *) getCurrentWeapon();
				//pWeaponKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
				pWeaponKnife->GetMainWeapon()->SetKnifeAttackType(WEAPON_KNIFE_ATTACK);

			}
			else if( i3::same_of<WeaponDualKnife*>(getCurrentWeapon()))
			{
			//	WeaponDualKnife * pWeaponKnife = (WeaponDualKnife *) getCurrentWeapon();
				MainWeapon_DualKnife* mainWeapon_DualKnife = static_cast<MainWeapon_DualKnife*>(getCurrentWeapon()->GetMainWeapon());
				mainWeapon_DualKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
			}
		}
		break;
	case WEAPON_CLASS_MISSION :
		{
			ISNETWORK
			{
				if( !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) ||		// ���ĸ�尡 �ƴѰ��
					(pMissionManager->getMissionState() != GAME_MISSION_STATE_INBATTLE) )			// ��ġ���� �ƴѰ��
					return false;
			}
		}
		break;
	}

	getCurrentWeapon()->NET_Fire( pStartPos, pTargetPos, fireOrder);
	m_pOwner->Cmd_ApplyCrosshair();

	// ������ ���
	getBoneContext()->_ProcessFire();

	return true;
}

void CGameCharaWeaponControl::_AfterProcessDropWeapon( WeaponBase * pWeapon, bool bDirectPickup, bool bChangeNextWeapon /*= true*/)
{
	// revision 60658 ��Ʈ��ũ ������ ���� �ڵ� ���� ��ȯ �����ϵ��� ���� ( hansoft. 5600 )
	/*if( m_pOwner->isNetwork() )
		return;*/

	if( !bChangeNextWeapon )
		return;

	if( pWeapon == nullptr)
		return;

	if( pWeapon->getWeaponInfo()->isC4() )
	{
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		//	�ֹ���� �ٷ� ��ü
		if( !m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY))
		{
			if( getWeaponContext()->getCurrentSlotNum() != getWeaponContext()->getOldSlotNum())
			{
				//	�ٷ� ���� ����� ��ü
				m_pOwner->Cmd_ChangeWeapon( getWeaponContext()->getOldSlotNum());
			}
		}
	}
	else
	{
		//	���� ����� �ٷ� ��ü
		INT32 nIdx = 0;

		//	���� ��ȣ�� ����� ��ü
		while( !getWeaponContext()->checkUsableWeaponSlot( static_cast<WEAPON_SLOT_TYPE>(nIdx), true) )
		{
			if( nIdx < MAX_WEAPON_CHECK_SLOT_COUNT )
				nIdx++;
			else
			{
				nIdx = -1;
				break;
			}
		}

		m_pOwner->Cmd_ChangeWeapon( static_cast<WEAPON_SLOT_TYPE>(nIdx));
	}
}


void CGameCharaWeaponControl::_NoUseWeapon( WeaponBase * pWeapon)
{
	VEC3D vec;

	I3ASSERT( pWeapon != nullptr);

	pWeapon->ReturnToReady();									// Ready ���·� ����
	pWeapon->getSceneObject()->GetBody()->StopAnim();			// �ִϸ��̼��� ����
	pWeapon->getSceneObject()->AddFlag( I3_NODEFLAG_DISABLE );	// ������Ʈ �� �ʿ䰡 �����ϴ�.
	// ����� �������� �����ݴϴ�.
	i3Vector::Set( &vec, (REAL32)( pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)( pWeapon->getIdx() / 10) );
	g_pWeaponManager->MoveShapePosition( pWeapon, &vec );
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool CGameCharaWeaponControl::SetBlendAlpha( WEAPON_SLOT_TYPE nSlot, REAL32 fVal)
{
	WeaponBase * pWeapon = getWeapon( nSlot);

	if( pWeapon == nullptr)
		return false;

	pWeapon->OnSetBlendAlpha( fVal);
	return true;
}

void CGameCharaWeaponControl::SetBlendAlphaAll( REAL32 fVal)
{
	for( INT32 i = 0; i < WEAPON_SLOT_COUNT; ++i)
	{
		SetBlendAlpha( static_cast<WEAPON_SLOT_TYPE>(i), fVal);
	}
}

bool CGameCharaWeaponControl::SetMaterialColor( WEAPON_SLOT_TYPE nSlot, COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	WeaponBase * pWeapon = getWeapon( nSlot);
	
	if( pWeapon == nullptr)
		return false;

	pWeapon->OnSetMaterialColor( pfDiffuse, pfSpecular);
	return true;
}

void CGameCharaWeaponControl::SetMaterialColorAll( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	for( INT32 i = 0; i < WEAPON_SLOT_COUNT; ++i)
	{
		SetMaterialColor( static_cast<WEAPON_SLOT_TYPE>(i), pfDiffuse, pfSpecular);
	}
}

void CGameCharaWeaponControl::OnUpdate( REAL32 rDeltatime)
{
	//	���� ���涧 �������� ������ ���� ��������...���� �����ӿ� �ٲ�(�ٲ��� �����) ���⸦ ���� �����Ѵ�.
	if( getSwapSlotNumber() != getCurrentSlotNumber() || getWeaponContext()->getForcedChange() )
	{
		if( getCurrentWeapon() != nullptr )
		{
			// ���� ����� ������� �ʴ� ����
			_NoUseWeapon( getCurrentWeapon());

			// ����Ǵ� ���Ⱑ �ùٸ� �տ� �پ��־�� �Ѵ�.
			getCurrentWeapon()->SetCurrentAttachHand();
		}

		// ���� ��ü
		OnSelectWeapon( getSwapSlotNumber());
	}
}

bool CGameCharaWeaponControl::OnCreateWeapon( WEAPON_SLOT_TYPE nSlot, T_ItemID Itemid, UINT8 nMultiWeaponSync)
{
	bool bMustCreate = false;
	bool bRecreate = false;
	I3ASSERT( nSlot < WEAPON_SLOT_COUNT );

	CWeaponInfo * pNewWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( Itemid);
	if( pNewWeaponInfo == nullptr)	// ���� ������ ����.
		return false;

	WeaponBase * pNewWeapon = getWeapon( nSlot);
	T_ItemID subItem = Itemid;

	if( pNewWeaponInfo->GetUseBombTrigger())
	{
		Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING1, WEAPON_CLASS_BOMBTRIGGER, WEAPON::getItemIndex(WEAPON::BOMBTRIGGER_DEFAULT)); // 412001
	}

	//	���� �Ŵ����� ���� Instance�� �޾ƿ´�.
	//	���߹��� �ý����� ����� ��� �ش� ���⸦ �ٸ� ������ ����ϰ� ���� �� �ֱ� ������ ���� ����
	if( pNewWeapon == nullptr || ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && g_pFramework->IsBattleStage()) )
	{
		bRecreate = true;
	}
	else
	{
		CWeaponInfo * pWeaponInfo = nullptr;

		// ���� �⺻ ����� BombTrigger ���� �����ÿ��� C5�� ItemID�� ������ ���ؾ� �մϴ�.
		// ������ ���� �޴� ItemID �� C5�� �Ǿ� ������ �����ε����� ���� getWeapon �� pNewWeapon�� ���
		// BombTrigger �� ��� �Ǿ� �־� �ش� ��쿡 ���Ͽ� ����� C5�� ������ ������ ���ϵ��� �����մϴ�.

		if ( i3::same_of<WeaponBombTrigger*>(pNewWeapon) )
		{
			bMustCreate = true;
			if ( ((WeaponBombTrigger*)pNewWeapon)->getBomb() != nullptr )
				pWeaponInfo = ((WeaponBombTrigger*)pNewWeapon)->getBomb()->getWeaponInfo();
		}
		else
		{
			pWeaponInfo = pNewWeapon->getWeaponInfo();
		}

		I3ASSERT( pWeaponInfo != nullptr);
			
		//	���� �ٸ� ������ ��� �ٽ� �޾ƿ´�.
		if( pWeaponInfo->GetItemID() != pNewWeaponInfo->GetItemID())
		{
			OnDeleteMultiWeapon( nSlot);
			bRecreate = true;
		}
		else
		{
			// �̹� �ִ� ����� �Ȱ���.
		}
	}

	if( bRecreate)
	{
		pNewWeapon = g_pWeaponManager->QueryWeapon( m_pOwner, Itemid, subItem , bMustCreate );
		if ( pNewWeapon == nullptr)
			return false;

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nSlot == WEAPON_SLOT_PRIMARY && g_pFramework->IsBattleStage())
		{
			// ���� ���⿡���� ���� ����� �ٸ��� ����
			if ( m_pOwner->isLocal())
			{
				m_pOwner->getMultiWeaponContext()->AddMultiWeapon( pNewWeapon, nMultiWeaponSync);
			}
			else
				m_pOwner->getMultiWeaponContext()->ChangeMultiWeapon( pNewWeapon);
		}
		else
		{
			setWeapon( nSlot, pNewWeapon);
		}
	}

	I3ASSERT( pNewWeapon != nullptr);

	// ���� �� ���߿� ����ݴϴ�.
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)( pNewWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)( pNewWeapon->getIdx() / 10) );
	g_pWeaponManager->MoveShapePosition( pNewWeapon, &vec );

	pNewWeapon->SetOwner( m_pOwner);
	pNewWeapon->setCreater( m_pOwner);
	pNewWeapon->setUserRespawnCnt( m_pOwner->getCharaInfo()->GetRespawnCnt());
	pNewWeapon->Reset();

	return bRecreate;
}

void CGameCharaWeaponControl::OnDeleteWeapon( WEAPON_SLOT_TYPE nSlot)
{
	WeaponBase * pWeapon = getWeapon( nSlot);

	if( pWeapon != nullptr )
	{
		//  ��� ���⸦ ������� �մϴ�..
		INT32 i;
		for( i = 0; i < WEAPON_SLOT_COUNT; i++)
		{
			OnDetachWeapon( (WEAPON_SLOT_TYPE) i);
		}
		
		if( pWeapon == getPurchasePrimWeapon() )
		{	// � �Ű� �ִٸ� �����.
			OnDetachPurchasePrimWeapon();
		}
		
		pWeapon->SetOwner( nullptr);
		setWeapon( nSlot, nullptr);
	}
}

void CGameCharaWeaponControl::OnDeleteMultiWeapon( WEAPON_SLOT_TYPE nSlot)
{
	WeaponBase * pWeapon = getWeapon( nSlot);

	if ( pWeapon != nullptr)
	{
		//  ��� ���⸦ ������� �մϴ�..
		INT32 i;
		for ( i = 0; i < WEAPON_SLOT_COUNT; i++)
		{
			OnDetachWeapon( (WEAPON_SLOT_TYPE)i);
		}

		if ( pWeapon == getPurchasePrimWeapon())
		{	// � �Ű� �ִٸ� �����.
			OnDetachPurchasePrimWeapon();
		}

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && m_pOwner->getMultiWeaponContext()->IsUsingMultiWeapon(pWeapon))
		{
			m_pOwner->getMultiWeaponContext()->DeleteMultiWeapon( pWeapon);
		}

		pWeapon->SetOwner(nullptr);
		setWeapon( nSlot, nullptr);
	}
}

/*virtual*/ WeaponBase * CGameCharaWeaponControl::OnAttachWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode)
{
	WeaponBase * pWeapon = getWeapon( nSlot );

	if( pWeapon == nullptr )	return nullptr;

	pWeapon->SetEnable( true);

	pWeapon->ChangeGunDummy( WEAPON_ATTACH_GRIP);	//	�⺻ Root�� ����
	pWeapon->AttachNode( pSceneNode, pGameNode );

	m_pOwner->Cmd_ApplyCrosshair();

	return pWeapon;
}
/*virtual*/ WeaponBase * CGameCharaWeaponControl::OnAttachBackWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode)
{
	WeaponBase * pWeapon = getWeapon( nSlot );

	if( pWeapon == nullptr )	return nullptr;

	pWeapon->SetEnable( true);
		
	pWeapon->ChangeGunDummy( WEAPON_ATTACH_GRIP);	//	�⺻ Root�� ����

	pWeapon->MoveNodeForBow();
	pWeapon->AttachNode( pSceneNode, pGameNode );

	m_pOwner->Cmd_ApplyCrosshair();

	return pWeapon;
}
/*virtual*/ WeaponBase * CGameCharaWeaponControl::OnDetachWeapon( WEAPON_SLOT_TYPE nSlot)
{
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot );

	if( pWeapon != nullptr )
	{
		pWeapon->ReturnToReady();
		pWeapon->DetachNode();
	}

	return pWeapon;
}

void CGameCharaWeaponControl::OnResetWeapon( void)
{
	setCurrentSlotNumber( WEAPON_SLOT_PRIMARY);
	setOldSlotNumber( WEAPON_SLOT_UNKNOWN);
	setSwapSlotNumber( WEAPON_SLOT_PRIMARY);

	if( m_pOwner->getCharaInfo()->GetType() == CHARA::TYPE_NPC)
		return;

	bool bCheck = false;
	INT32 i = 0;

	while( !bCheck )
	{
		if( OnSelectWeapon( static_cast<WEAPON_SLOT_TYPE>(i), false) )
		{
			break;
		}

		i++;

		if( i >= WEAPON_SLOT_COUNT )
		{
			break;
			//I3PRINTLOG(I3LOG_FATAL,  "invalid server information for weapon list." );
		}
	}

	if( getCurrentWeapon() != nullptr )
	{
		getCurrentWeapon()->ReturnToReady();
		getWeaponContext()->setStaticCurrentWeapon( getCurrentWeapon());
	}

	setSwapSlotNumber( getCurrentSlotNumber());
	setOldSlotNumber( getCurrentSlotNumber());

	for( i = 0; i < WEAPON_SLOT_COUNT; ++i)
	{
		WeaponBase * pWeapon = getWeapon( static_cast<WEAPON_SLOT_TYPE>(i));

		if( pWeapon != nullptr)
			pWeapon->Reset();
	}

	//	����� �⺻ �����տ� ����
	if(getCurrentWeapon() != nullptr)
	{
		getCurrentWeapon()->ChangeAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND, WEAPON_ATTACH_GRIP);
	}

	SetBlendAlpha( getCurrentSlotNumber(), 1.0f);
}

void CGameCharaWeaponControl::OnAttackIdleWeapon( void)
{
	WeaponBase*	pWeapon	= getCurrentWeapon();

	if( !i3::same_of<WeaponCrossFire*>(pWeapon) 
		|| !pWeapon->isExtensionActivate()  )
	{
		if (getActionContext())
			getActionContext()->ApplyActionBody2( CHARA_BODYUPPER_ATTACKIDLE);
		else
			TRACE("CGameCharaWeaponControl::OnAttackIdleWeapon getActionContext = nullptr\n");
	}

	if(  i3::same_of<CGameCamera_FPS*>(g_pCamera))
	{
		if(m_pOwner->is1PV())
		{
			( (CGameCamera_FPS*)g_pCamera )->SetNeckAnim(false);
		}
	}
}

bool CGameCharaWeaponControl::OnAttachPurchasePrimWeapon( WEAPON_SLOT_TYPE nSlot)
{
	if( nSlot == WEAPON_SLOT_UNKNOWN)
		return false;

	WeaponBase * pPrvWeapon = getWeapon( nSlot);
	if( pPrvWeapon == nullptr )
		return false;

	WeaponBase * pCurWeapon = getCurrentWeapon();
	if( pCurWeapon == nullptr)
		return false;

	//	���� ���⸦ ��� �����Ƿ� ��� �����.
	if( pCurWeapon == getPurchasePrimWeapon() )
	{
		OnDetachPurchasePrimWeapon();
	}
	
	if( pPrvWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY)
	{	//	���Ⱑ �ֹ���� ��¦�� ���δ� (� �Ǵ�)
		pPrvWeapon->OnSetBlendAlpha( 1.f );
		setPurchasePrimWeapon( pPrvWeapon);
		OnAttachBackWeapon( nSlot, getDummyPrimPurchase(), m_pOwner);
		
		pPrvWeapon->OnSetVisibleExtension( false);

		return true;
	}
	else
	{
		if( getPurchasePrimWeapon() != nullptr)
		{
			if( getPurchasePrimWeapon()->getWeaponInfo() != nullptr )
			{	
				getPurchasePrimWeapon()->MoveNodeForBow();
				//	�Ⱥپ� ������ �ٽ� ���δ�.
				if( !getPurchasePrimWeapon()->IsAttachedScene())
					getPurchasePrimWeapon()->AttachNode( getDummyPrimPurchase(), m_pOwner);
			}
			else
			{// �ش� ����� ���� ����̴�.(���忡 ���� ���)
				setPurchasePrimWeapon( nullptr);
			}
		}
	}

	return false;
}

void CGameCharaWeaponControl::OnDetachPurchasePrimWeapon( void)
{
	if( getPurchasePrimWeapon() == nullptr )
		return;

	if( getPurchasePrimWeapon()->getParentScene() == getDummyPrimPurchase())
		getPurchasePrimWeapon()->DetachNode();

	setPurchasePrimWeapon( nullptr);
}

bool CGameCharaWeaponControl::OnAttachPurchaseSideWeapon( WEAPON_SLOT_TYPE nSlot)
{
	return false;
}

void CGameCharaWeaponControl::OnDetachPurchaseSideWeapon( void)
{
}

bool CGameCharaWeaponControl::OnPreSelectWeapon(WEAPON_SLOT_TYPE nSlot)
{
	if ( nSlot <= WEAPON_SLOT_UNKNOWN || nSlot >= WEAPON_SLOT_COUNT)
		return false;

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	if ( nSlot == WEAPON_SLOT_MISSION)
	{
		if ( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) && (pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE))
			return false;	// ��ź�� ��� ��ġ���� �ƴ϶�� �ٲ� �� �����ϴ�.
	}

	// ��ü�� ����
	WeaponBase * pSwapWeapon;

	if ( !MyRoomInfoContext::i()->getStage()->IsMultiWeapon()
		|| ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nSlot != WEAPON_SLOT_PRIMARY)
		|| !g_pFramework->IsBattleStage() )
	{
		pSwapWeapon = getWeaponContext()->getWeapon( nSlot);
	}
	else
	{
		// ���߹��� ��ü ��
		if ( m_pOwner->getMultiWeaponContext()->GetCurrSlotInfo() != nullptr)
		{
			UINT8 nPrevIndex = m_pOwner->getMultiWeaponContext()->GetCurrSlotInfo()->nSlotNumber;
			m_pOwner->getWeaponContext()->setPrevMultiWeaponIdx( nPrevIndex);
		}

		pSwapWeapon = m_pOwner->getMultiWeaponContext()->GetSwapMultiWeapon( false);

		if ( pSwapWeapon != nullptr && getCurrentSlotNumber() == WEAPON_SLOT_PRIMARY && nSlot == WEAPON_SLOT_PRIMARY)
		{
			getWeaponContext()->setForcedChange(true);
		}
	}

	// ��ü�� ���� ������ ������ Error
	if ( pSwapWeapon == nullptr)
		return false;

	if ( pSwapWeapon->isWorldWeapon() == false)
	{
		if ( MyRoomInfoContext::i()->IsLockedWeaponSlot(nSlot))
		{
			return false;
		}
	}

	CWeaponInfo * pWeaponInfo = pSwapWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	if ( pWeaponInfo->isUnableSwapIfZero())
	{
		//	�Ҹ� ������ Throwing �Ǵ� Item�� ���� ������ 0�̸� ��ü�� �� ����.
		if ( pSwapWeapon->getLoadedBulletCount() <= 0)
			return false;
	}

	// KEYWORD : WEAPON_C5
	// ��ź�� ��ٸ� �ٲ��� �ʵ����մϴ�. ��ȹ ��û 0724 - dfly79
	if ( i3::same_of<WeaponBombTrigger*>(pSwapWeapon))
	{
		if ( (pSwapWeapon->getLoadedBulletCount() == 0) && (((WeaponBombTrigger*)pSwapWeapon)->getDropedBombCount() == 0))
			return false;
	}

	WeaponBase * pCurWeapon = getCurrentWeapon();

	if ( pCurWeapon != nullptr)
	{
		if (i3::same_of<WeaponDualThrowKnife*>(pCurWeapon))
		{
			if (static_cast<WeaponDualThrowKnife*>(pCurWeapon)->getDoThrowKnife())
			{
				return false;
			}
		}
		else if (i3::same_of<WeaponDualGrenade*>(pCurWeapon))
		{
			if (static_cast<WeaponDualGrenade*>(pCurWeapon)->getThrowFirstGrenade())
			{
				return false;
			}
		}
	}

	//	Weapon Change
	if( getCurrentWeapon() != nullptr)
	{	
		//	ĳ���� �ִϸ��̼��� �����ϸ� ���� �����Ӻ��� ����Ǳ� ������ �̹� �����ӿ� ���⸦ �ٲ��� �ʰ� �ٸ� �����ӿ� 
		//	���⸦ �ٲٵ��� �Ѵ�.
		if ( getCurrentWeapon()->isWeaponStateMask(WSM_POINTSHOTING))
		{
			//������ ��쿡�� �ű� ���� ����� �۵� ���̴��� �ߴ� ��Ű�� �ʱ�ȭ�Ѵ�.
			//���� ���� �߻��߿� �ٲ�°� �����̳�, ��ȹ�ǵ��� ������� �ϹǷ� �ش� ������ ����.
			getCurrentWeapon()->removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE | WSM_POINTSHOTING);
			getCurrentWeapon()->setAutoFireCount(0);
		}

		if ( m_pOwner->isNetwork() && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			getCurrentWeapon()->ReturnToReady();
			getCurrentWeapon()->DetachNode();
		}

		setOldSlotNumber( getCurrentSlotNumber());
		setSwapSlotNumber( nSlot);		//	���� �����ӿ� �ٲ� ���� ��ȣ 
	}
	else
	{	//	���� ���Ⱑ nullptr�̸� ���� �ٲ۴�.		
		//	�ִϸ��̼� �ϱ� ���� ���� ���� ��ü(�𵨸�)�� �ؾ� �Ѵ�.
		if( !OnSelectWeapon( nSlot))			return false;

		//	����� �⺻ �����տ� ����
		//revision 55728 2013. 3. 25 �ڱ⼺
		//ç���� ��忡�� �̴Ͼ׽� ���� �� ��� ���⸦ ������ �̴Ͼ׽��� �پ���� ���� ����
		//�̴Ͼ׽� �� �ƴ϶� ź���ܰ˵� �߻�.
		if(!i3::kind_of<WeaponMiniAxe*>(getCurrentWeapon()))
			getCurrentWeapon()->ChangeAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND, WEAPON_ATTACH_GRIP);
	}

	if( g_pEdgeTrail != nullptr)
		g_pEdgeTrail->Reset();

	return true;
}

void CGameCharaWeaponControl::NoUseWeapon(WeaponBase* pWeapon)
{
#if defined(I3_DEBUG)
	i3::rc_wstring wstrWeaponName;
	pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);
	I3TRACE("�������� ���� - %s\n", wstrWeaponName);
#endif

	pWeapon->DetachNode();

	_NoUseWeapon(pWeapon);
}

bool CGameCharaWeaponControl::OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound /*= true*/)
{
	if( nSlot == WEAPON_SLOT_UNKNOWN)
		return false;

	//	��ü�� ����
	WeaponBase * pNextWeapon = nullptr;
	
	if (!MyRoomInfoContext::i()->getStage()->IsMultiWeapon()
		|| ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nSlot != WEAPON_SLOT_PRIMARY)
		|| !g_pFramework->IsBattleStage())
	{
		pNextWeapon = getWeapon(nSlot);

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			getWeaponContext()->setForcedChange(false);
			m_pOwner->getMultiWeaponContext()->ResetChange();
		}
	}
	else
	{
		// ���߹��⿡�� ��ü ���� ����
		pNextWeapon = m_pOwner->getMultiWeaponContext()->GetSwapMultiWeapon( true);

		getWeaponContext()->setForcedChange(false);
	}

	if( pNextWeapon != nullptr )
	{	
		// ��� ���⸦ ������� �մϴ�..
		INT32 i;
		for( i = 0; i < WEAPON_SLOT_COUNT; i++)
		{
			OnDetachWeapon( (WEAPON_SLOT_TYPE)i);
		}

		if ( m_pOwner->getWeaponContext()->getPrevMultiWeaponIdx() != 0 &&
			MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			UINT8 nPrevWeaponIndex = m_pOwner->getWeaponContext()->getPrevMultiWeaponIdx();
			WeaponBase* pPrevWeapon = m_pOwner->getMultiWeaponContext()->GetMultiWeapon( nPrevWeaponIndex);

			if ( pPrevWeapon != nullptr)
			{
				pPrevWeapon->DetachNode();
				_NoUseWeapon(pPrevWeapon);
				m_pOwner->getWeaponContext()->setPrevMultiWeaponIdx(0);
			}
		}

		if( pNextWeapon->getOwner() == nullptr)
			pNextWeapon->SetOwner( m_pOwner);

		//	�ٲ� ����� ���δ�.
		OnAttachWeapon( nSlot, getWeaponAttachRoot(), m_pOwner );

		pNextWeapon->ReturnToReady();

		if(pNextWeapon->IsExtensionWeapon())
		{
			pNextWeapon->OnSetVisibleExtension( pNextWeapon->isExtensionActivate());
		}

		#if defined( AI_MOVE_TO_WEAPON)
			setOldSlotNumber( getCurrentSlotNumber());
		#endif
		
		setCurrentSlotNumber( nSlot );
		setSwapSlotNumber( nSlot);

		// �ٲ� ����
		if( g_pFramework->IsBattleStage())
		{
			pNextWeapon->WeaponChange( bPlaySound);		//	���� ��ü�� ���� ����� Change ���·�
			getHUDContext()->ChangeWeapon();

			m_pOwner->Cmd_ApplyCrosshair();
		}
		else
		{
			pNextWeapon->WeaponChange( false);		//	���� ��ü�� ���� ����� Change ���·�
		}
		
		{
			VEC3D vec;
			pNextWeapon->SetEnable( true);
			pNextWeapon->SetPos( &vec );		// LocalPose�� �ʱ�ȭ ������մϴ�.
		}

		// �߿�
		// ���Ⱑ �ٲ�� �ش� ���⿡ �´� �ִϸ��̼� ���� �����ؾ� �մϴ�.
		getAnimContext()->ChangeAnimationContext();

		if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))
		{
			getWeaponContext()->setStaticCurrentWeapon( pNextWeapon);
		}

		return true;
	}

	return false;
}

bool CGameCharaWeaponControl::OnWeaponDropToWorld( WEAPON_SLOT_TYPE nSlot, VEC3D * pPos, bool bChangeNextWeapon, bool bDirectPickup)
{
	//	SceneNode -> World Scene Node
	//	GameNode -> World Game Node
	WeaponBase*	pWeapon	= nullptr;

	pWeapon = getWeapon( nSlot);

	if( pWeapon == nullptr)
		return false;
	
	//pWeapon->setExtensionActivate( false );				// ������ Extension ���¸� �ʱ�ȭ���ش�.
	pWeapon->ResetSound();					// ���� �ʱ�ȭ
	pWeapon->getSceneObject()->GetBody()->StopAnim();	// �ִϸ��̼� ������Ʈ ����
	pWeapon->ReturnToReady();							// ����
	
	if( i3::same_of<WeaponC4*>(pWeapon))
	{

#if defined( USE_EVENT_SHUTTLEX)
		{
			// �̺�Ʈ�� C4
			// ���� C4�� �����ϰ� �̺�Ʈ�� C4�� ��ü�մϴ�.
			g_pWeaponManager->ReleaseCharaWeapon( pWeapon);
			pWeapon = g_pWeaponManager->QueryWeapon( m_pOwner, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_SHUTTLEX);
			pWeapon->setOwner( m_pOwner);
			pWeapon->Reset();
			pWeapon->SetEnable( true);
		}
#endif

		if(m_pOwner->getMoveContext()->getRideObject() == nullptr)
		{
			//	����(ĳ���� �θ�)�� �ٴ´�.
			((WeaponC4*)pWeapon)->InstallC4ToWorld(m_pOwner->getSceneObject()->GetParent()->GetParent(), m_pOwner->getParent(), pPos, true, nullptr);
		}
		else 
		{
			VEC3D v3Pos;
			MATRIX invMat;

			i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( m_pOwner->getMoveContext()->getRideObject()->getSceneObject());
				
			I3ASSERT(pTrans);

			MATRIX* pMat = pTrans->GetCacheMatrix();
				
			i3Matrix::Inverse( &invMat, nullptr, pMat);

			if( BattleServerContext::i()->IsUseDedicationMode() == false)
				i3Vector::TransformCoord( &v3Pos, pPos, &invMat);
			else
				i3Vector::Copy( &v3Pos, pPos);

			((WeaponC4*)pWeapon)->InstallC4ToMoveObject( pTrans, m_pOwner->getParent(), &v3Pos, pPos, &invMat);
		}
	}
	else
	{
		//Stage root�� ���δ�
		pWeapon->Transmission(m_pOwner->getSceneObject()->GetParent()->GetParent(), m_pOwner->getParent(), pPos, true);
	}

	// ���� ĳ���ͷν� �����ڸ� ������ �� �ִ�.
	pWeapon->SetOwner( m_pOwner);
	
	//	�������� ���� �Ŵ����� �����̸� �����ȴ�. �ش� ĳ���ʹ� �������� ����.
	g_pWeaponManager->MoveWeaponCharaToDrop( pWeapon);	

	pWeapon->getSceneObject()->GetBody()->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
	if( i3::same_of<WeaponC4*>(pWeapon))
	{
		I3ASSERT( ((WeaponC4*)pWeapon)->m_bEnableActive == false);

		((WeaponC4*)pWeapon)->m_bEnableActive = true;
	}
	
	pWeapon->EnableRenderAABB(true);
	
	if ( !( nSlot == WEAPON_SLOT_PRIMARY && MyRoomInfoContext::i()->getStage()->IsMultiWeapon()))
	{
		setWeapon( nSlot, nullptr);
	}
	
	if( bChangeNextWeapon )
		_AfterProcessDropWeapon( pWeapon, bDirectPickup);

	return true;
}

bool CGameCharaWeaponControl::OnWeaponTake( WeaponBase * pWeapon, UINT8 nSyncNumber, bool bCheckDropList, bool bPushWeaponList)
{	
	WEAPON_SLOT_TYPE	nWeaponSlot = WEAPON_SLOT_PRIMARY;
	I3ASSERT( pWeapon != nullptr );

	switch( pWeapon->getWeaponInfo()->GetTypeUsage() )
	{
	case WEAPON_SLOT_UNKNOWN	:	break;
	case WEAPON_SLOT_PRIMARY	:
	case WEAPON_SLOT_SECONDARY	:
	case WEAPON_SLOT_MELEE		:
	case WEAPON_SLOT_MISSION	:	nWeaponSlot = pWeapon->getWeaponInfo()->GetTypeUsage(); break;

	case WEAPON_SLOT_THROWING1	:
	case WEAPON_SLOT_THROWING2	:
	default :
		I3PRINTLOG(I3LOG_FATAL,  "Take weapon.  invalid type." );
		break;
	}

	bool bUsedMultiWeapon = false;

	// �� �κ� ���䰡 �ʿ��մϴ�...
	if ( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
		 MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) )
	{
		if( getWeapon( nWeaponSlot ) != nullptr )
		{
			if( getWeapon(nWeaponSlot)->getWeaponInfo()->GetNumber() == pWeapon->getWeaponInfo()->GetNumber())
				OnDeleteMultiWeapon( nWeaponSlot);
		}

		if( pWeapon->getWeaponInfo()->GetTypeUsage() ==  WEAPON_SLOT_MISSION)
		{
			//�̼� ���� ȹ��
			I3_BOUNDCHK( m_pOwner->getCharaNetIndex(), SLOT_MAX_COUNT);
			UserInfoContext::i()->SetMyUsurpationIdx( static_cast<UINT8>(m_pOwner->getCharaNetIndex()) );
		}
	}
	else
	{
		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nWeaponSlot == WEAPON_SLOT_PRIMARY)
		{
			bUsedMultiWeapon = true;
		}

		//	���ο� ���⸦ �����ϱ� ���ؼ��� �ش� ���� ������ �ݵ�� ��� �־�� �մϴ�. ���⼭ �ɸ��� ������...komet
		if( getWeapon( nWeaponSlot ) != nullptr && !bUsedMultiWeapon)
		{
			if( pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_MELEE)
			{
				if( getWeapon(nWeaponSlot)->getWeaponInfo()->GetNumber() == pWeapon->getWeaponInfo()->GetNumber())
					OnDeleteWeapon( nWeaponSlot);
				else
					return false;

			}
			else
				return false;
		}
	}

	I3TRACE( "[TakeWeapon] chara : %d, id : %d, idx : %d\n", m_pOwner->getCharaNetIndex(), pWeapon->getNetworkIdx(), pWeapon->getNetworkIdx() );
	// Ageia ���⸦ Kinematic���� �����ϱ����� �ø����� �ȵǵ��� ����(������ �־�Դϴ�.)
	VEC3D vec;

	CWeaponInfo *pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);
	i3Vector::Set( &vec, (REAL32)( pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)( pWeapon->getIdx() / 10) );
	g_pWeaponManager->MoveShapePosition( pWeapon, &vec );

	pWeapon->setDynamicActiveState( true, false );
	pWeapon->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true );

	if( pWeapon->getContactHitBox() != nullptr)
	{
		pWeapon->getContactHitBox()->modifyDynamicState( I3_PHYSIX_STATE_NO_COLLISION, 0);
	}

	pWeapon->DetachNode();
	pWeapon->SetOwner( m_pOwner);
	
	// ���� ���⿡���� ������ �ܿ� �ֹ��� �������� ����
	if ( !bUsedMultiWeapon)
	{
		setWeapon(nWeaponSlot, pWeapon);
	}

	// LOD�� �����ϰ� �ٿ�� �ڽ��� �����Ѵ�.
	pWeapon->OnChangeFPSView();

	if( BattleServerContext::i()->IsP2PActivate() )
	{
		MATRIX mat;
		
		pWeapon->getSceneObject()->SetMatrix( &mat );
		if( pWeapon->isExtensionActivate() )
			pWeapon->OnSetVisibleExtension( true );
	}

	//	���忡�� ĳ���ͷ� �ű��.
	pWeapon->Transmission( m_pOwner->getSceneObject(), m_pOwner, nullptr, false);			// SceneGraph ��� ����, ĳ���� ���� ���Դϴ�.
	pWeapon->ReturnToReady();

	if ( m_pOwner->getWeaponContext()->getCurrentSlotNum() == WEAPON_SLOT_PRIMARY &&
		nWeaponSlot == WEAPON_SLOT_PRIMARY && m_pOwner->isLocal() &&
		MyRoomInfoContext::i()->getStage()->IsMultiWeapon() )
	{
		m_pOwner->getMultiWeaponContext()->SetQuickChangeIdx();
	}
	
	// bOnlyList
	// nSyncNumber - ���߹��� �߰��� �� �� �ʿ�
	// bCheckDropList - �������Ʈ���� ã�� �ʴ� ���, ��Ʋ������Ʈ�����ۿ��� ���
	// bPushWeaponList - m_CharaWeaponList ������� �ʿ�
	// m_CharaWeaponList�� ������ CreateAllSpecialDamageFilter ���� ����
	g_pWeaponManager->MoveWeaponDropToChara( pWeapon, false, nSyncNumber, bCheckDropList, bPushWeaponList);	// ���� �������� ĳ���Ϳ� ����Ʈ�� �ű��.
	
	 if( nWeaponSlot == WEAPON_SLOT_SECONDARY && getCurrentSlotNumber() == WEAPON_SLOT_PRIMARY)
	{	// �ֹ��⸦ ��� �������⸸ ������ ���
		// �ʱ�ȭ ���� �߿��մϴ�.
		pWeapon->ReturnToReady();
		OnDetachWeapon( nWeaponSlot);
		
		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			m_pOwner->getMultiWeaponContext()->ResetChange();
	}
	else
	{	// ������ ���⿡ ���ؼ��� �ش� ����� �鵵�� �մϴ�.
		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nWeaponSlot == WEAPON_SLOT_PRIMARY)
		{
			m_pOwner->getMultiWeaponContext()->HasPickupChange();
		}

		if( m_pOwner->Cmd_ChangeWeapon( nWeaponSlot))
		{
			// ���� �����ӿ� ���� ��ü�� �ϱ� ���� �ٲٱ� �� ���⸦ �����ϴµ�
			if ( !( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nWeaponSlot == WEAPON_SLOT_PRIMARY))
			{
				OnAttachWeapon(nWeaponSlot, getWeaponAttachRoot(), m_pOwner);		// ĳ���Ϳ� GameNode�� ���Դϴ�.
				getAnimContext()->ChangeAnimationContext();
			}
		}
		else
		{
			if ( nWeaponSlot == WEAPON_SLOT_PRIMARY)
			{
				pWeapon->ReturnToReady();
				OnAttachPurchasePrimWeapon(nWeaponSlot);
			}
		}
	}
	
	 //2014.07.16 ENG_��뿵 ���� 9893 
	 //Ż�� ��忡�� �ݴ� ������ �߽� Į�� ���� ���带 ����ϵ��� �ڵ� �߰���. 
	
	 if ( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)  // Ż�� ����� ��쿡��.
		 && pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::CHINESECLEAVER_SEIZED) ) 
	 {
		pWeapon->OnPlayChangeSound();
	 }

	 //2014.07.16 
	return true;
}

bool CGameCharaWeaponControl::OnDropedWeaponSearching( REAL32 rDeltatime)
{
	I3ASSERT( !m_pOwner->isNetwork() );

	return _ProcessDropedWeaponSearching();
}

bool CGameCharaWeaponControl::OnFire( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	if( !m_pOwner->isNetwork())
	{
		if( getCurrentWeapon() == nullptr )	return false;

		CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
		I3ASSERT( pWeaponInfo != nullptr);

		CGameMissionManager * pMissionManager = CGameMissionManager::i();

		//	�� ����
		switch( pWeaponInfo->GetTypeClass())
		{			
		case WEAPON_CLASS_KNIFE:
		case WEAPON_CLASS_KNUCKLE:
			{	//	�������� ��� ���� ó��
				if( i3::same_of<WeaponKnife*>(getCurrentWeapon()))
				{
					WeaponKnife * pWeaponKnife = (WeaponKnife *) getCurrentWeapon();
					pWeaponKnife->GetMainWeapon()->SetKnifeAttackType(WEAPON_KNIFE_ATTACK);
				//	pWeaponKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
				}
				else if( i3::same_of<WeaponDualKnife*>(getCurrentWeapon()))
				{
				//	WeaponDualKnife * pWeaponKnife = (WeaponDualKnife *) getCurrentWeapon();
					MainWeapon_DualKnife* mainWeapon_DualKnife = static_cast<MainWeapon_DualKnife*>(getCurrentWeapon()->GetMainWeapon());
					mainWeapon_DualKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
				}

			}
			break;
		case WEAPON_CLASS_MISSION :
			{
				if ( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) )
				{
					return false;
				}
				else 
				{
					ISNETWORK
					{
						if( (pMissionManager->getMissionState() != GAME_MISSION_STATE_INBATTLE) ||			// ��ġ���� �ƴѰ��
							(pMissionManager->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// �������� �ƴѰ��
							return false;
					}
				}

				if( m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) ||
					getActionContext()->isUpper( CHARA_UPPER_JUMP) ||
					!m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR))
					return false;

				if( pMissionManager->IsEnableMy() == false)
				{	// C4 ��ġ �Ұ���
					if( CHud::i())
						CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_BOMB_MISSION_NONAREA"));	// ���������� ��ġ�Ҽ� �ֽ�.
					return false;
				}
			}
			break;
		}

		if( !getCurrentWeapon()->Fire( WEAPON::PRIMARY_FIRE) )
			return false;

		//���� ���� ������ �ű� ���� ����� �۵� ���Ѿ� �Ѵٸ� �߻�� ���ÿ� �ش� ���·� ���⸦ �ٲ���.
		//�����̵� �� �Լ��� ��ӹ��� �κ��� ���� �������� �� ������ ����ȴ�.
		INT32 autopointshot = 0;
		if (m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM) == false)
			autopointshot = pWeaponInfo->GetAutoPointShotEnable();
		else 
			autopointshot = pWeaponInfo->GetExtAutoPointShotEnable();

		if(autopointshot != 0)
			getCurrentWeapon()->addWeaponStateMask(WSM_POINTSHOTING);

		// �̰�	�����̳� ��ȹ���� ��û���� ������� �ʽ��ϴ�. �߻��߿� ���ҾȵǴ� ����Դϴ�.
		//addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
		// ����ź �迭�� ���׷� ������ ���ϰ� ������ �� �ֽ��ϴ�.

		switch(pWeaponInfo->GetTypeClass())
		{
		case WEAPON_CLASS_THROWING_GRENADE:
		case WEAPON_CLASS_THROWING_CONTAIN:
		case WEAPON_CLASS_THROWING_ATTACK:
		case WEAPON_CLASS_THROWING_HEAL:
		case WEAPON_CLASS_ROCKET_LAUNCHER:
			{
				m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
			}
			break;
		}

		// ���� �����ӿ� �߻簡 �ȵǵ���
		if( pWeaponInfo->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT )
			m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);

		m_pOwner->Cmd_ApplyCrosshair();

		// ������ ���
		getBoneContext()->_ProcessFire();

		return true;
		
	}
	
	return _ProcessFireForNetwork( pStartPos, pTargetPos);
//	return false;
}

bool CGameCharaWeaponControl::OnSecFire( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	if( !m_pOwner->isNetwork() )
	{
		if( getCurrentWeapon() == nullptr )		return false;

		CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
		I3ASSERT( pWeaponInfo != nullptr);

		//	���� ����
		switch( pWeaponInfo->GetTypeClass())
		{			
		case WEAPON_CLASS_KNIFE:
		case WEAPON_CLASS_KNUCKLE:
			{
				if( i3::same_of<WeaponKnife*>(getCurrentWeapon()))
				{
					WeaponKnife * pWeaponKnife = (WeaponKnife *) getCurrentWeapon();
					// pWeaponKnife->SetAttackType( WEAPON_KNIFE_SECONDARY_ATTACK );
					pWeaponKnife->GetMainWeapon()->SetKnifeAttackType(WEAPON_KNIFE_SECONDARY_ATTACK);

				}
				else if( i3::same_of<WeaponDualKnife*>(getCurrentWeapon()))
				{
					WeaponDualKnife * pWeaponKnife = (WeaponDualKnife *) getCurrentWeapon();
					
					MainWeapon_DualKnife* mainWeapon_DualKnife = static_cast<MainWeapon_DualKnife*>(pWeaponKnife->GetMainWeapon());
					
					mainWeapon_DualKnife->SetAttackType( WEAPON_KNIFE_SECONDARY_ATTACK );
				}
			}
			break;
		case WEAPON_CLASS_CIC:
			if( m_pOwner->is1PV())
			{
				MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(getCurrentWeapon()->GetMainWeapon());
		
				WeaponKnife * pWeaponKnife = (WeaponKnife*) mainWeapon_DualCIC->getLeftWeapon();
				
				if( i3::same_of<WeaponKnifeGender*>(mainWeapon_DualCIC->getLeftWeapon()) )
					break;

			//	pWeaponKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
				pWeaponKnife->GetMainWeapon()->SetKnifeAttackType(WEAPON_KNIFE_ATTACK);
				if( i3::same_of<CGameCamera_FPS*>(g_pCamera))
				{
					((CGameCamera_FPS*)g_pCamera)->SetNeckAnim(true);
				}
			}
			break;
		}

		if( !getCurrentWeapon()->Fire( WEAPON::SECONDARY_FIRE) )
			return false;

		// �̰�	�����̳� ��ȹ���� ��û���� ������� �ʽ��ϴ�. �߻��߿� ���ҾȵǴ� ����Դϴ�.
		//addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );

		if( i3::same_of<WeaponBombTrigger*>(getCurrentWeapon())
			||	i3::kind_of<WeaponGrenadeLauncher*>(getCurrentWeapon()))
		{
			m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
		}

		m_pOwner->Cmd_ApplyCrosshair();

		// ������ ���
		getBoneContext()->_ProcessFire();

		return true;
	}
	else
		return _ProcessFireForNetwork( pStartPos, pTargetPos, WEAPON::SECONDARY_FIRE);
}

bool CGameCharaWeaponControl::OnFireReady( void)
{
	if( getCurrentWeapon() == nullptr )	return false;

	CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);
	
	// (�ӽ�) �߻� �غ�. ���� ��ô ���⸸ ������ ����....����� �ִϸ��̼� �߰� �ʿ���.
	switch(pWeaponInfo->GetTypeClass())
	{
	case WEAPON_CLASS_THROWING_GRENADE	:
	case WEAPON_CLASS_THROWING_CONTAIN	:
	case WEAPON_CLASS_THROWING_ATTACK	:
	case WEAPON_CLASS_THROWING_HEAL		:
	case WEAPON_SUBCLASS_BOW:
	case WEAPON_CLASS_BOW				:	
		{
			getCurrentWeapon()->FireReady();
			return true;
		}
		break;
	}
	
	return false;

}

bool CGameCharaWeaponControl::OnExtActivate( void)
{
	WeaponBase * pWeapon = getCurrentWeapon();
	if( pWeapon == nullptr )				return false;
	if( !pWeapon->IsExtensionWeapon())	return false;
	if( m_pOwner->isLocal() )
	{
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_DUAL_MAG)
		{// ��� źâ�� ��ü�� �� �ִ°�?
			if( pWeapon->getTotalBulletCount() <= 0 &&
				pWeapon->getLoadedBulletDualCount() <= 0)
				return false;
		}

		if( pWeapon->isWeaponStateMask( WSM_EXT_DISABLE ))
			return false;
	}

	//	���� Extension Ȱ��ȭ / ��Ȱ��ȭ �߿��� ����� �߻� ���Ѵ�.	
	bool bAnimPlay = true;
	// DualMagazine, Silencer�� ĳ���� �ִϸ��̼��� ����.
	if( m_pOwner->is3PV() )
	{
		if( i3::same_of<WeaponDualMagazine*, WeaponSilencer*>(pWeapon))
			bAnimPlay = false;
	}

	if( pWeapon->ExtensionActivate( true, bAnimPlay))
	{
		if( m_pOwner->isLocal())
			pWeapon->addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		return true;
	}

	return false;
}

bool CGameCharaWeaponControl::OnExtDeactivate( void)
{
	WeaponBase * pWeapon = getCurrentWeapon();

	if( pWeapon == nullptr )				return false;
	if( !pWeapon->IsExtensionWeapon())	return false;

	if( m_pOwner->isLocal())
	{
		if( pWeapon->isWeaponStateMask( WSM_EXT_DISABLE) )
			return false;

		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_DUAL_MAG)
		{// ��� źâ�� ��ü�� �� �ִ°�?
			if( pWeapon->getTotalBulletCount() <= 0 &&
				pWeapon->getLoadedBulletDualCount() <= 0)
				return false;
		}
	}

	//	���� Extension Ȱ��ȭ / ��Ȱ��ȭ �߿��� ����� �߻� ���Ѵ�.	
	bool bAnimPlay = true;
	// DualMagazine�� 3��Ī ĳ���� �ִϸ��̼��� ����.
	if( m_pOwner->is3PV() )
	{
		if( i3::same_of<WeaponDualMagazine*, WeaponSilencer*>(pWeapon))
			bAnimPlay = false;
	}

	if( pWeapon->ExtensionActivate( false, bAnimPlay))
	{
		if( m_pOwner->isLocal())
			pWeapon->addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		return true;
	}

	return false;
}

bool CGameCharaWeaponControl::OnLoadMagazine( void)
{
	if( getCurrentWeapon() == nullptr )		return false;

	CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	//	�ѱ� �迭�� ����
	if( pWeaponInfo->isGun() == false)		return false;

	getCurrentWeapon()->SetEnable( true);

	if( !m_pOwner->isNetwork())
	{
		// ���� �����߿��� ���ε� ����
		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON))
			return false;
	}

	if (!m_pOwner->isNetwork() && getCurrentWeapon()->isWeaponStateMask(WSM_POINTSHOTING))
	{
		getCurrentWeapon()->setAutoFireCount(0);
		getCurrentWeapon()->removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE | WSM_POINTSHOTING);
	}
	
	if( getCurrentWeapon()->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER &&
		getCurrentWeapon()->isExtensionActivate() )
	{
		WeaponGrenadeLauncher * pGLauncher = (WeaponGrenadeLauncher*) getCurrentWeapon();

		if( !m_pOwner->isNetwork())
		{
			if( pGLauncher->getTotalShellCount() <= 0)
				return false;
		}
		
		pGLauncher->LoadShell();
	}
	//	KEYWORD : WEAPON_SHOTGUN_LAUNCHER	
	else if( getCurrentWeapon()->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER &&
		getCurrentWeapon()->isExtensionActivate() )
	{
		WeaponShotGunLauncher * pLauncher = (WeaponShotGunLauncher*) getCurrentWeapon();
		
		if( !m_pOwner->isNetwork())
		{
			if( pLauncher->getTotalShellCount() <= 0)
				return false;

			if( (pLauncher->getMagazineType() == WEAPON_DUAL_MAGAZINE_NONE) &&
				pLauncher->getLoadedShellCount() == pLauncher->getExtensionWeaponInfo()->GetLoadBullet() )
				return false;
		}
		
		//	LSS ����
		pLauncher->LoadShell();
	}
	else
	{
		if( !m_pOwner->isNetwork())
		{
			//	���� �Ѿ��� ������ ��� ������ �� �� ����.
			if( getCurrentWeapon()->getTotalBulletCount() <= 0)
				return false;

			if( (getCurrentWeapon()->getMagazineType() == WEAPON_DUAL_MAGAZINE_NONE) &&
				getCurrentWeapon()->getLoadedBulletCount() == getCurrentWeapon()->GetLoadBulletWithCashItem() )
				return false;
		}

		// Zoom�� Ű�� LoadMag�� �ϸ� Zoom�� �������ϴ�.
		// Jinsik.kim(2015/04/10)
		// ��뿵 : ������, �Ѱ� ���� ������ WEAPON_EXT_MASK_DIRECT_USE �̰� ����ϰ� �;�����, ���� �̰� ���� �ȵǼ�...
		// �̷��� ó���մϴ�.
		if( getCurrentWeapon()->GetCurrentExtensionType() != WEAPON::EXT_SILENCER &&
			getCurrentWeapon()->GetCurrentExtensionType() != WEAPON::EXT_BAYONET )
			m_pOwner->Cmd_Extension(false);	//���Ϸ����� �������� �ص� ���� �ؾߵ�.
		getCurrentWeapon()->LoadMagazine();
	}
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
		m_pOwner->Cmd_Idle_Camera();

	return true;
}

bool CGameCharaWeaponControl::OnLoadMagazineDual( void)
{	
	if( getCurrentWeapon() == nullptr )								return false;
	I3ASSERT( getCurrentWeapon()->getWeaponInfo() != nullptr);
	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)		return false;
	if( getCurrentWeapon()->GetCurrentExtensionType() != WEAPON::EXT_DUAL_MAG )
		return false;

	if (!m_pOwner->isNetwork() && getCurrentWeapon()->isWeaponStateMask(WSM_POINTSHOTING))
	{
		getCurrentWeapon()->setAutoFireCount(0);
		getCurrentWeapon()->removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE | WSM_POINTSHOTING);
	}

	if( !m_pOwner->isNetwork())
	{
		//	���� �Ѿ��� ������ ��� ������ �� �� ����.
		if( getCurrentWeapon()->getTotalBulletCount() <= 0)	
			return false;
	}

	getCurrentWeapon()->SetEnable( true);

	//	���� Extension ���� 
	getCurrentWeapon()->ExtensionActivate( true);

	if( !m_pOwner->isNetwork())
	{
		g_pFramework->getChatBox()->PutSystemChatting( L"Extension On");
		//	 �ִϸ��̼��� ������ �߻� ����
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
	}

	return true;
}

bool CGameCharaWeaponControl::OnLoadBullet( void)
{
	if( getCurrentWeapon() == nullptr )							return false;
	//	�ѱ� �迭�� ����
	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)	return false;

	if( !m_pOwner->isNetwork())
	{
		if( getCurrentWeapon()->isLoadBulletEnabled() == false)
			return false;
	}

	getCurrentWeapon()->SetEnable( true);
	getCurrentWeapon()->LoadBullet();
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
		m_pOwner->Cmd_Idle_Camera();

	return true;
}

/*virtual*/ bool CGameCharaWeaponControl::OnProcessRespawn( void)
{
	//	�ִϸ��̼� �ϱ� ���� ���� ���� ��ü(�𵨸�)�� �ؾ� �Ѵ�.
	OnSelectWeapon( WEAPON_SLOT_MELEE, false);	// �������ѿ� ���Ե��� �ʴ� ��������� �ʱ� ����

	m_pOwner->Cmd_SyncWeapon();

	if( getCurrentWeapon() != nullptr )
		getCurrentWeapon()->ReturnToReady();	//	���� ��ü�� ���� ����� Ready ���·� ����

	return true;
}

bool CGameCharaWeaponControl::OnLoadMagazine_Ready( void)
{
	if( getCurrentWeapon() == nullptr )
		return false;

	I3ASSERT( getCurrentWeapon()->getWeaponInfo() != nullptr);

	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)		return false;

	if( !m_pOwner->isNetwork())
	{
		//	���� �Ѿ��� ������ ��� ������ �� �� ����.
		if( getCurrentWeapon()->getTotalBulletCount() <= 0)
			return false;

		if( (getCurrentWeapon()->getMagazineType() == WEAPON_DUAL_MAGAZINE_NONE) &&
			getCurrentWeapon()->getLoadedBulletCount() == getCurrentWeapon()->GetLoadBulletWithCashItem() )
			return false;

		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON ))	// ���� �����߿��� ���ε� ����
			return false;

		if( !getCurrentWeapon()->isLoadMagazineEnabled())
			return false;
	}


	getCurrentWeapon()->SetEnable( true);
	getCurrentWeapon()->LoadMagazine_Ready();
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
		m_pOwner->Cmd_Idle_Camera();

	return true;
}





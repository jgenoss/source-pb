#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaBase.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "./Character/GameCharaWeaponControl1PV.h"
#include "./Character/GameCharaWeaponControl3PV.h"

#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameMissionManager.h"
#include "MainWeapon_Dual.h"
#include "MainWeapon_DualSMG.h"
#include "Weapon/WeaponItemID.h"

//#include "./Character/GameCharaWeaponControl3PVNPC.h"
#include "UtilBattle.h"

I3_CLASS_INSTANCE( CGameCharaWeaponContext);

CGameCharaWeaponContext::CGameCharaWeaponContext()
{
	for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
		m_pWeapon[ i] = nullptr;

	m_bForcedChange				= false;
}

CGameCharaWeaponContext::~CGameCharaWeaponContext()
{
	OnDestroy();
}

void CGameCharaWeaponContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork )
{
	// WeaponControl
	m_pCurrentWeaponControl = nullptr;
	m_p1PVWeaponControl			= CGameCharaWeaponControl1PV::new_object();
	m_p1PVWeaponControl->BindOwner( pOwner);
	m_p3PVWeaponControl			= CGameCharaWeaponControl3PV::new_object();
	m_p3PVWeaponControl->BindOwner( pOwner);

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork );
}

void CGameCharaWeaponContext::OnDestroy()
{
	CGameCharaContextBase::OnDestroy();

	for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
		m_pCurrentWeaponControl->OnDeleteMultiWeapon( static_cast<WEAPON_SLOT_TYPE>(i));
	}

	m_pCurrentWeaponControl = nullptr;
	I3_SAFE_RELEASE( m_p3PVWeaponControl);
	I3_SAFE_RELEASE( m_p1PVWeaponControl);
}

void CGameCharaWeaponContext::OnChange1PV( void)
{
	m_pCurrentWeaponControl = m_p1PVWeaponControl;

	m_pCurrentWeaponControl->OnChangeViewWeapon();
}

void CGameCharaWeaponContext::OnChange3PV( void)
{
	m_pCurrentWeaponControl = m_p3PVWeaponControl;

	m_pCurrentWeaponControl->OnChangeViewWeapon();
}

void CGameCharaWeaponContext::OnChangeLocal( void)
{
	if( m_pOwner->is1PV())
		m_pCurrentWeaponControl = m_p1PVWeaponControl;
	else
		m_pCurrentWeaponControl = m_p3PVWeaponControl;
}

void CGameCharaWeaponContext::OnChangeNetwork( void)
{
	if( m_pOwner->is1PV())
		m_pCurrentWeaponControl = m_p1PVWeaponControl;
	else
		m_pCurrentWeaponControl = m_p3PVWeaponControl;
}

void CGameCharaWeaponContext::OnChangeNPC( void)
{
	
}

void CGameCharaWeaponContext::OnUpdate( REAL32 rDeltatime)
{
	if( m_pCurrentWeaponControl == nullptr)
		return;

	// ���� ������� ���Ⱑ ������ ���忡 �ִ� ������� üũ�մϴ�.
	if( !m_pOwner->isNetwork() && m_pOwner->getDropWeaponTimer() < 0.3f)
	{
		I3ASSERT(m_pOwner->IsDino() == false);

		//AI�� ����� ���⸦ ���� ���ϵ��� ��
		if(m_pOwner->isAI() == false)
			m_pCurrentWeaponControl->OnDropedWeaponSearching( rDeltatime);
	}

	m_pCurrentWeaponControl->OnUpdate( rDeltatime);
}

void CGameCharaWeaponContext::OnChangeBoneHierarchy( i3Body * pBody)
{
	INT32 i;

	if( m_pOwner->getCharaInfo()->GetType() == CHARA::TYPE_NPC)
		return;

	//////////////////////////////	������ ���� 
	m_pDummyRightHand = i3Scene::FindNodeByName( pBody, "WeaponPointDummyRight");

	if (m_pDummyRightHand)
	{
		I3ASSERT( m_pDummyRightHand != nullptr);
		if( i3::same_of<i3BoneRef*>(m_pDummyRightHand))
		{	
			if( m_pDummyRightHand->GetChildCount() > 0 )
			{
				i3Node * pChild = m_pDummyRightHand->GetChild( 0 );
				if( i3::same_of<i3Transform*>(pChild))
					m_pDummyRightHand = pChild;
			}
		}

		//	�⺻ ����������
		m_pWeaponAttachRoot = m_pDummyRightHand;		
	}
	else
		TRACE("CGameCharaWeaponContext::__SetDummyVariable WeaponPointDummyRight = nullptr\n");
	


	//////////////////////////////	�޼� ����
	i3Node * pNode = i3Scene::FindNodeByName( pBody, "WeaponPointDummyLeft");
	if (pNode)
	{
		I3ASSERT( pNode != nullptr);
		if( i3::same_of<i3BoneRef*>(pNode))
		{	
			for( i = 0; i < pNode->GetChildCount(); i++ )
			{
				i3Node * pChild = pNode->GetChild( i );

				if( pChild->hasName() == false)
					continue;

				const char * pszName = pChild->GetName();
				if( pszName[ 0] == 'W' && pszName[ 6] == 'P' && pszName[ 11] == 'D')
				{
					if( pszName[ 20] == 'S')
						m_pDummyLeftSideHand = pChild;		// �ѿ� ������ ��� ���� ����
					else
						m_pDummyLeftHand = pChild;			// �׸��� ����
				}
			}
		}
	}
	else
		TRACE("CGameCharaWeaponContext::__SetDummyVariable WeaponPointDummyLeft = nullptr\n");

	

	//	��¦ ���� (�������� ���� �ֹ��� �ٴ� ��)
	m_pDummyPrimPurchase = i3Scene::FindNodeByName( pBody, "WeaponPointDummyBack");
	if (m_pDummyPrimPurchase)
	{
		I3ASSERT( m_pDummyPrimPurchase != nullptr);
		if( i3::same_of<i3BoneRef*>(m_pDummyPrimPurchase))
		{
			if( m_pDummyPrimPurchase->GetChildCount() > 0 )
			{
				i3Node * pChild = m_pDummyPrimPurchase->GetChild( 0 );
				if( i3::same_of<i3Transform*>(pChild))
					m_pDummyPrimPurchase = pChild;
			}
		}
	}
	else
		TRACE("CGameCharaWeaponContext::__SetDummyVariable WeaponPointDummyBack = nullptr\n");
}

WeaponBase* CGameCharaWeaponContext::getWeapon( WEAPON_SLOT_TYPE nSlotNum)
{
	if (nSlotNum <= WEAPON_SLOT_UNKNOWN || nSlotNum >= WEAPON_SLOT_COUNT)
	{
		I3PRINTLOG(I3LOG_NOTICE, "WeaponSlot invalid %d", nSlotNum);
		return nullptr;
	}
	
	if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon()	&& g_pFramework->IsBattleStage() && m_pOwner->getMultiWeaponContext() != nullptr)
	{
		// ���� ���⿡�� �ֹ��� ������ �����´�
		if ( nSlotNum == WEAPON_SLOT_PRIMARY && m_pOwner->getMultiWeaponContext()->GetCurrSlotInfo() != nullptr)
			return m_pOwner->getMultiWeaponContext()->GetCurrSlotInfo()->pWeapon;
	}

	return m_pWeapon[nSlotNum];
}

WEAPON_SLOT_TYPE CGameCharaWeaponContext::getNextUsableWeaponSlot(void)
{
	INT32 nIdx = getCurrentSlotNum() + 1;

	if( nIdx > MAX_WEAPON_CHECK_SLOT_COUNT )
		nIdx = WEAPON_SLOT_PRIMARY;

	if( nIdx == WEAPON_SLOT_MISSION)//C4
	{
		CGameMissionManager * pMissionMng = CGameMissionManager::i();
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) &&
			(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) )
		{
			nIdx = WEAPON_SLOT_PRIMARY;	// ��ź�� ��� ��ġ���� �ƴ϶�� �ٲ� �� �����ϴ�.
		}
	}

	//	���� ��ȣ�� ����� ��ü
	while( !checkUsableWeaponSlot( static_cast<WEAPON_SLOT_TYPE>(nIdx), false) )
	{
		if( nIdx < MAX_WEAPON_CHECK_SLOT_COUNT )
			nIdx++;
		else	
			nIdx = 0;

		if( nIdx == getCurrentSlotNum())	//	�ѹ��� ����( �ٲ� ���Ⱑ ���ٸ�)
			return WEAPON_SLOT_UNKNOWN;
	}

	return static_cast<WEAPON_SLOT_TYPE>(nIdx);
}

WEAPON_SLOT_TYPE CGameCharaWeaponContext::getPrevUsableWeaponSlot( void)
{
	INT32 nIdx = getCurrentSlotNum() - 1;

	if( nIdx < 0)		nIdx = MAX_WEAPON_CHECK_SLOT_COUNT;

	if( nIdx == WEAPON_SLOT_MISSION)//C4
	{
		CGameMissionManager * pMissionMng = CGameMissionManager::i();
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) &&
			(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) )
			nIdx = WEAPON_SLOT_THROWING2;	// ��ź�� ��� ��ġ���� �ƴ϶�� �ٲ� �� �����ϴ�.
	}

	//	���� ��ȣ�� ����� ��ü
	while( !checkUsableWeaponSlot( static_cast<WEAPON_SLOT_TYPE>(nIdx), false) )
	{
		if( nIdx > 0)
			nIdx--;
		else
			nIdx = MAX_WEAPON_CHECK_SLOT_COUNT ;

		if( nIdx == getCurrentSlotNum())	//	�ѹ��� ����( �ٲ� ���Ⱑ ���ٸ�)
			return WEAPON_SLOT_UNKNOWN;
	}

	return static_cast<WEAPON_SLOT_TYPE>(nIdx);
}

bool CGameCharaWeaponContext::checkUsableWeaponSlot( WEAPON_SLOT_TYPE slot, bool bCheckMultiWeapon)
{
	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	if( slot == WEAPON_SLOT_MISSION)//C4
	{
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) &&
			(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) )
			return false;	// ��ź�� ��� ��ġ���� �ƴ϶�� �ٲ� �� �����ϴ�.
	}

	WeaponBase * pCheckWeapon = nullptr;
	
	bool bIsMultiWeapon = MyRoomInfoContext::i()->getStage()->IsMultiWeapon();

	if ( slot != WEAPON_SLOT_PRIMARY)
	{
		pCheckWeapon = getWeapon(slot);
	}
	else
	{
		if ( !bIsMultiWeapon || ( bIsMultiWeapon && !bCheckMultiWeapon))
		{
			pCheckWeapon = getWeapon(slot);
		}
		else if (m_pOwner->getMultiWeaponContext()->GetMultiWeaponCount() < 1)
		{
			pCheckWeapon = nullptr;
		}
		else
		{
			pCheckWeapon = m_pOwner->getMultiWeaponContext()->GetSwapMultiWeapon(false);
		}
	}

	WEAPON_SLOT_TYPE	nCurSlot = getCurrentSlotNum();

	if( pCheckWeapon != nullptr )
	{
		if( pCheckWeapon->isWorldWeapon() == false)
		{
			// ���� ���� �˻�
			if (true == MyRoomInfoContext::i()->IsLockedWeaponSlot(slot))
			{
				return false;
			}
		}
	
		// ���߹��� �ý����� ����� ��� �ֹ��Ⱑ ���� �ִ��� Ȯ���ؾ���
		if( nCurSlot != slot || bCheckMultiWeapon)
		{
			CWeaponInfo * pWeaponInfo = pCheckWeapon->getWeaponInfo();
			I3ASSERT( pWeaponInfo != nullptr);

			if( pWeaponInfo->isUnableSwapIfZero() )
			{
				//	�Ҹ� ������ Throwing �Ǵ� Item�� ���� ������ 0�̸� ��ü�� �� ����.
				if( pCheckWeapon->getLoadedBulletCount() <= 0)
					return false;
			}

			i3::rc_wstring wstrWeaponName;
			pWeaponInfo->GetWeaponName(wstrWeaponName);
			I3TRACE("���� Ȯ�� : %s\n", wstrWeaponName);

			return true;
		}
	}

	return false;
}

void CGameCharaWeaponContext::NoUseWeapon(WeaponBase* pWeapon)
{
	I3ASSERT(pWeapon != nullptr);

	// �������� ������ �����ڸ� �˰� �־�� ��
	pWeapon->setOwner(m_pOwner);

	m_pCurrentWeaponControl->NoUseWeapon( pWeapon);
}

void CGameCharaWeaponContext::_SyncWeapon( void)
{
	//	���� ��ü��(SwapWeapon)�� ���� �����ӿ� ����Ǳ� ������ ���� �����ӿ��� ��ü ���⸦ ������ �ʿ䰡 ���� ���
	//	ȣ���ؾ� �մϴ�. (��Ʈ��ũ ���� ...)
	//	
	//	��.)
	//	Frame			Command			Current Weapon
	//=====================================================================
	//	(1) :	SwapWeapon (No.1)	CurWeapon(No.0) <= current (before swap)
	//			Fire (No.1)					:
	//	(2) :		:				CurWeapon(No.1)	<= Apply swap 

	if( m_nSwapWeaponSlotNum != m_nCurWeaponSlotNum )
	{
		//	���� ��� ����
		m_pCurrentWeaponControl->OnSelectWeapon( m_nSwapWeaponSlotNum);
		if( m_pOwner->is3PV() )
		{
			m_pCurrentWeaponControl->OnAttachPurchasePrimWeapon( m_nOldWeaponSlotNum);
			m_pCurrentWeaponControl->OnAttachPurchaseSideWeapon( m_nOldWeaponSlotNum);
		}
	}
}


//EXPORT_AVALIABLE_CODE_VALUE
void CGameCharaWeaponContext::WeaponValidCheck( WEAPON_CLASS_TYPE nClassType, WEAPON::RES_ID WeaponNum, WEAPON_SLOT_TYPE slot, UINT8 nMultiWeaponIndex, bool bSyncChange)
{
	bool bCheck = false;

	// ���⺰ Slot ��ȣ �߿���.
	// Ư���� ����ó���Ǿ�� �ϴ� ����� ���⼭ ���� ��ȣ�� �����ؾ� �Ѵ�.
	// �׷��� ������ OnCreateWeapon �ؼ� ũ���� ���� komet
	WEAPON_SLOT_TYPE nSlot = WEAPON_SLOT_PRIMARY;

	switch( nClassType )
	{
	case WEAPON_CLASS_ASSAULT			:
	case WEAPON_CLASS_SMG				:
	case WEAPON_CLASS_SNIPER			:
	case WEAPON_CLASS_SHOTGUN			:
	case WEAPON_CLASS_MG				:	
	case WEAPON_CLASS_BOW				:
	case WEAPON_CLASS_DUALSMG			:
	case WEAPON_CLASS_DUALSHOTGUN		:
	case WEAPON_CLASS_ROCKET_LAUNCHER	:		nSlot = WEAPON_SLOT_PRIMARY;	break;
	case WEAPON_CLASS_HANDGUN			:			
	case WEAPON_CLASS_CIC:
	case WEAPON_SUBCLASS_ASSAULT :
	case WEAPON_SUBCLASS_SHOTGUN :
	case WEAPON_SUBCLASS_SNIPER :
	case WEAPON_SUBCLASS_SMG :
	case WEAPON_SUBCLASS_G_LAUNCHER :
	case WEAPON_SUBCLASS_BOW :
	case WEAPON_CLASS_DUALHANDGUN		:		nSlot = WEAPON_SLOT_SECONDARY;	break;
	case WEAPON_CLASS_KNIFE				:
	case WEAPON_CLASS_KNUCKLE			:
	case WEAPON_CLASS_DUALKNIFE			:		nSlot = WEAPON_SLOT_MELEE;		break;
	case WEAPON_CLASS_BOMBTRIGGER		:		nSlot = WEAPON_SLOT_THROWING1;  break;
	case WEAPON_CLASS_THROWING_GRENADE	:		nSlot = WEAPON_SLOT_THROWING1;	break;
	case WEAPON_CLASS_THROWING_CONTAIN	:		nSlot = WEAPON_SLOT_THROWING2;	break;
	case WEAPON_CLASS_THROWING_ATTACK	:		nSlot = WEAPON_SLOT_THROWING2;	break;
	case WEAPON_CLASS_THROWING_HEAL		:		nSlot = WEAPON_SLOT_THROWING2;	break;
	case WEAPON_CLASS_MISSION			:		nSlot = WEAPON_SLOT_MISSION;	break;

	// nullptr ����� ������ � ������ nullptr �� �ؾ����� �𸣱⿡ ������ ���� ������ nullptr�� ���� - ������
	case WEAPON_CLASS_UNKNOWN :			nSlot = slot;			break;
	default:		// �����ȵ� Ÿ���� ������ ��� �ݵ�� �����ؾ� ��.
		return;
	}

	// �߸� ������ ������ �ٷ� ũ���� �ǵ��� ���⼭ �׳� �������� ������ ���߿� �ٸ����� ũ���� ���� �� �ִ�.
	if( nSlot <= WEAPON_SLOT_UNKNOWN || nSlot >= WEAPON_SLOT_COUNT)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Unknown weapon slot. please check weapon.");
	}

	WEAPON_SLOT_TYPE i32CurSlot = getCurrentSlotNum();

	WeaponBase * pWeapon = getWeapon( nSlot );
	
	if( pWeapon != nullptr )
	{	
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		I3ASSERT( pWeaponInfo != nullptr);

		// KEYWORD : WEAPON_C5
		if( i3::same_of<WeaponBombTrigger*>(pWeapon)
			&& ((WeaponBombTrigger*)pWeapon)->getBomb() != nullptr)
		{
			if( nClassType == WEAPON_CLASS_BOMBTRIGGER)		return;
			pWeaponInfo = ((WeaponBombTrigger*)pWeapon)->getBomb()->getWeaponInfo();
		}

		// KEYWORD : WEAPON_DUALHANDGUN
		if( i3::kind_of<WeaponDualHandGun*>(pWeapon))
		{	
			if( !pWeaponInfo->isSame(nClassType, WeaponNum))
			{
				MainWeapon_Dual* mainWeapon_Dual = static_cast<MainWeapon_Dual*>(pWeapon->GetMainWeapon());

				WeaponBase * pLeftWeapon = mainWeapon_Dual->getLeftWeapon();
				if( !pLeftWeapon->getWeaponInfo()->isSame( nClassType, WeaponNum))
				{
					bCheck = true;
					m_pCurrentWeaponControl->OnDeleteWeapon( nSlot);	// ���Ⱑ �ٸ��� ���� ����� ��ȯ
					I3TRACE( "wrong weapon. recreate.\n" );
				}
			}
		}
		else if( i3::kind_of<WeaponDualSMG*>(pWeapon))
		{
			if( !pWeaponInfo->isSame(nClassType, WeaponNum))
			{
				MainWeapon_DualSMG* mainWeapon_DualSMG = static_cast<MainWeapon_DualSMG*>(pWeapon->GetMainWeapon());

				WeaponBase * pLeftWeapon = mainWeapon_DualSMG->getLeftWeapon();
				if( !pLeftWeapon->getWeaponInfo()->isSame( nClassType, WeaponNum))
				{
					bCheck = true;
					m_pCurrentWeaponControl->OnDeleteWeapon( nSlot);	// ���Ⱑ �ٸ��� ���� ����� ��ȯ
					I3TRACE( "wrong weapon. recreate.\n" );
				}
			}
		}
		else if( i3::kind_of<WeaponCrossFire*>(pWeapon))
		{
			if( !pWeaponInfo->isSame(nClassType, WeaponNum))
			{
				MainWeapon_Dual* mainWeapon_Dual = static_cast<MainWeapon_Dual*>(pWeapon->GetMainWeapon());

				WeaponBase * pLeftWeapon = mainWeapon_Dual->getLeftWeapon();
				if( !pLeftWeapon->getWeaponInfo()->isSame( nClassType, WeaponNum))
				{
					bCheck = true;
					m_pCurrentWeaponControl->OnDeleteWeapon( nSlot);	// ���Ⱑ �ٸ��� ���� ����� ��ȯ
					I3TRACE( "wrong weapon. recreate.\n" );
				}
			}
		}
		// LSS ���� ������ �ϴ� Ư��ó��. komet
		// WeaponShotGunLauncher::ExtensionActivate���� ���� �ͽ��ټ��� Swap�� ���� ��Ŷ ������ ���� 
		// LSS�� ExtensionActivate �Լ������� m_pWeaponInfo�� m_pExtWeaponInfo���� ������ ���������� LSS WeaponInfo�� ������ �� �ִ�.
		// ������ ��Ʈ��ũ ���� ���� �ͽ��ټ� Ÿ���� ���� ��Ŷ���� �����Ƿ� �̿� ���� WEAPON::EXT_TYPE���� �� �� ����.
		// GAMEINFO_CHARA_WEAPON���� _bWeaponExtension�� bool �� (����� On/Off ������ ����) �� �����Ѵ�.
		// ���� UDP���� ����ϴ� ��Ŷ ������ __Network_P2P_PCInfo_Weapon�� iWeapon ������
		// 4bit ((pIn->iWeapon >> 4) & 0x01)�� Extension On/Off������ �Ҵ�Ǿ� �ִ�. 
		// ��Ȯ�� ��Ʈ��ũ ����� ���� ������ WEAPON::EXT_TYPE���� ��Ŷ������ ���ԵǾ�� �ٺ����� ���׸� �ذ��� �� ���� ���̴�.		
		else if( i3::kind_of<WeaponShotGunLauncher*>(pWeapon))
		{	
			CWeaponInfo * pExtWeaponInfo = pWeapon->getExtensionWeaponInfo();
			if( pExtWeaponInfo)
			{
				// �ֹ��⿡ �Ҽӵ� ���������̹Ƿ� ���� ���⸦ OnCreateWeapon ���� �ʵ��� (���������� �𵨸��� �������� ������ �ֹ��� �𵨸��� ���ԵǾ� �ִ�)
				if( pExtWeaponInfo->GetTypeClass() == nClassType && pExtWeaponInfo->GetNumber() == WeaponNum)
					bCheck = false;
			}
		}
		else if(i3::kind_of<WeaponMiniAxe*>(pWeapon))
		{
			if( !pWeaponInfo->isSame(nClassType, WeaponNum))
			{
				T_ItemID ExtItemID = pWeaponInfo->GetSubExtensionItemID(WEAPON::EXT_DUAL);
				WEAPON_CLASS_TYPE ext_ct = WEAPON::ItemID2Class( ExtItemID );
				WEAPON::RES_ID ext_num = WEAPON::ItemID2ResID( ExtItemID );	

				// �ͽ��ټ� �迭�� �ٸ� ���, ���� ���� ����
				if(nClassType != ext_ct || WeaponNum != ext_num )				
				{
					bCheck = true;
					m_pCurrentWeaponControl->OnDeleteWeapon( nSlot);
					I3TRACE( "wrong weapon. recreate.\n" );
				}
			}
		}
		else if ( i3::kind_of<WeaponShotGunGenderWithDual*>(pWeapon) )
		{
			if( !pWeaponInfo->isSame(nClassType, WeaponNum))
			{
				T_ItemID ExtItemID = pWeaponInfo->GetSubExtensionItemID(WEAPON::EXT_DUAL);
				WEAPON_CLASS_TYPE ext_ct = WEAPON::ItemID2Class( ExtItemID );
				WEAPON::RES_ID ext_num = WEAPON::ItemID2ResID( ExtItemID );	

				// �ͽ��ټ� �迭�� �ٸ� ���, ���� ���� ����
				if(nClassType != ext_ct || WeaponNum != ext_num )				
				{
					bCheck = true;
					m_pCurrentWeaponControl->OnDeleteWeapon( nSlot);
					I3TRACE( "wrong weapon. recreate.\n" );
				}
			}
		}
		else
		{
			if( !pWeaponInfo->isSame(nClassType, WeaponNum))
			{
				bCheck = true;
				I3TRACE("WeaponInfo %d Class, %d Number\n", pWeaponInfo->GetTypeClass(), pWeaponInfo->GetNumber());
				I3TRACE("WeaponSync %d Class, %d Number\n", nClassType, WeaponNum);
				m_pCurrentWeaponControl->OnDetachWeapon( nSlot);	// ���Ⱑ �ٸ��� ���� ����� ��ȯ
				I3TRACE( "wrong weapon. recreate.\n" );
			}
			else if (m_pOwner->getMultiWeaponContext()->GetCurrSlotInfo() != nullptr
				&& nSlot == WEAPON_SLOT_PRIMARY && bSyncChange
				&& MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			{
				if (m_pOwner->getMultiWeaponContext()->GetCurrSlotInfo()->nSyncNumber != nMultiWeaponIndex)
				{
					bCheck = TRUE;
					m_pCurrentWeaponControl->OnDetachWeapon(nSlot);
				}
			}
		}
	}
	else
	{
		// ���Ⱑ ������ �޾ƿ´�.
		bCheck = true;
	}
	
	if( bCheck )
	{
		// - ������
		// ������ ������� ����ȭ ������ ��� - �̹� ����� ���� �˻� �������� OnDeleteWeapon ���� ����� ��.
		if( nClassType == WEAPON_CLASS_UNKNOWN )
		{
			// ����� �ǹǷ� ���� ���⸦ �鵵�� �մϴ�.
			if( _ChangeWeapon( WEAPON_SLOT_MELEE ) )
			{
				if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
					m_pOwner->Cmd_Idle_Camera();

				//getAnimContext()->FinishCurrentAIUpper();

				getActionContext()->setUpper( CHARA_UPPER_SWAPWEAPON);
				getAnimContext()->_Play_Anim_Upper( ID_UPPER_SWAP_WEAPON );
			}
			return;
		}

		// ������ ��� ���� ����ִ� ����� ���� ������ ��쿡�� ���� ���⸦ ��ü�Ͽ� �鵵�� �մϴ�.
		// ����ִ� ������ ��ü�ع����� �ȵǱ� ���� �Դϴ�. �ٸ� ������ ���� ������ �ٲ��ݴϴ�.
		
		bool bChangeWeapon;

		if( m_pOwner->isLocal() )
		{
			if( i32CurSlot == nSlot )
				bChangeWeapon = true;
			else
				bChangeWeapon = false;
		}
		else	bChangeWeapon = true;

		if( bChangeWeapon )
		{
			if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
				m_pOwner->Cmd_Idle_Camera();

			if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nSlot == WEAPON_SLOT_PRIMARY 
				&& m_pOwner->getMultiWeaponContext()->GetMultiWeaponCount() > 0)
			{
				UINT8 idx = m_pOwner->getMultiWeaponContext()->GetMultiWeaponIdx( nMultiWeaponIndex);
				m_pOwner->getMultiWeaponContext()->SetMultiWeaponSlot(idx);
			}
			else
			{
				T_ItemID ItemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, GetItemType_WeaponSlotTypeFunc( (WEAPON_SLOT_TYPE)nSlot), nClassType, WeaponNum);
				m_pCurrentWeaponControl->OnCreateWeapon( nSlot, ItemID, nMultiWeaponIndex);
				
				I3TRACE("CreateWeapon : %d, %d\n", nSlot, ItemID);
			}
			m_pCurrentWeaponControl->OnSelectWeapon( nSlot );

			if (m_pOwner->isNetwork())
			{
				m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY);
			}

			// �⺻ �ִϸ��̼��� �ϵ��� �մϴ�.
			pWeapon = getCurrent();
			if( pWeapon != nullptr)
			{
				pWeapon = pWeapon->getUsedWeapon();
				if( pWeapon->isDualGun() )
				{
					getAnimContext()->_Play_Anim_DualUpper( getAnimContext()->getOldUpperDualLeftID(), 0.f);
					getAnimContext()->_Play_Anim_DualUpper( getAnimContext()->getOldUpperDualRightID(), 0.f);
				}
				else
				{
					getAnimContext()->_Play_Anim_Upper( getAnimContext()->getOldUpperID());
				}
			}
		}
		else
		{
			T_ItemID itemID = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, Weapon::GetItemTypeFromClass( nClassType),		nClassType, WeaponNum);
			pWeapon = g_pWeaponManager->QueryWeapon( m_pOwner, itemID);

			if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nSlot == WEAPON_SLOT_PRIMARY)
			{
				if ( m_pOwner->isLocal())
				{
					i3::rc_wstring wstrWeaponName;
					pWeapon->getWeaponInfo()->GetWeaponName(wstrWeaponName);
					I3TRACE("MAKE_ITEM_FLAG - AddMultiWeapon %s(%d)\n", wstrWeaponName, nMultiWeaponIndex);
					m_pOwner->getMultiWeaponContext()->AddMultiWeapon(pWeapon, nMultiWeaponIndex);
				}
				else
				{
					i3::rc_wstring wstrWeaponName;
					pWeapon->getWeaponInfo()->GetWeaponName(wstrWeaponName);
					I3TRACE("MAKE_ITEM_FLAG - ChangeMultiWeapon %s(%d)\n", wstrWeaponName, nMultiWeaponIndex);
					m_pOwner->getMultiWeaponContext()->ChangeMultiWeapon(pWeapon);
				}
				m_pOwner->Cmd_ChangeWeapon(WEAPON_SLOT_PRIMARY);
			}
			else
			{
				setWeapon(nSlot, pWeapon);
			}
		}
	}
}

WeaponBase * CGameCharaWeaponContext::FindWeaponByExtensionItemID( T_ItemID extItemID)
{
	for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
		if( m_pWeapon[ i] == nullptr) continue;
		for( UINT32 j = 0; j < MAX_WEAPON_EXTENSION; j++)
		{
			if( m_pWeapon[ i]->getWeaponInfo()->GetExtensionItemID( j) == extItemID)
				return m_pWeapon[ i];
		}
	}

	return nullptr;
}

WEAPON_SLOT_TYPE CGameCharaWeaponContext::CreateWeaponByGameMode( UINT8 WeaponFlag)
{
	WEAPON_SLOT_TYPE slot = WEAPON_SLOT_UNKNOWN;
	T_ItemID item_id = 0;

	if( TESTBIT( WeaponFlag, LOCK_RPG7) )
	{
		slot = WEAPON_SLOT_PRIMARY;
		item_id = g_pFramework->FindWeaponIDForGameMode(WEAPON_CLASS_ROCKET_LAUNCHER);
	}
	else if( TESTBIT( WeaponFlag, LOCK_SHOTGUN))
	{
		slot = WEAPON_SLOT_PRIMARY;
		item_id = g_pFramework->FindWeaponIDForGameMode(WEAPON_CLASS_SHOTGUN);
	}
	else if( TESTBIT( WeaponFlag, LOCK_SNIPER))
	{
		slot = WEAPON_SLOT_PRIMARY;
		item_id = g_pFramework->FindWeaponIDForGameMode(WEAPON_CLASS_SNIPER);
	}
	else if( TESTBIT( WeaponFlag, LOCK_KNUCKLE))
	{
		slot = WEAPON_SLOT_MELEE;
		item_id = g_pFramework->FindWeaponIDForGameMode(WEAPON_CLASS_KNUCKLE);
	}

	if( slot != WEAPON_SLOT_UNKNOWN )
	{
		WEAPON_CLASS_TYPE type = WEAPON::ItemID2Class(item_id);

		if( m_pWeapon[ slot] == nullptr || m_pWeapon[ slot]->getWeaponInfo()->GetTypeClass() != type)
		{
			_Create_Weapon( slot, item_id);
			_ChangeWeapon( slot, true);
			getAnimContext()->ChangeAnimationContext();

			if( m_pWeapon[ slot] != nullptr)
				m_pWeapon[ slot]->ReturnToReady();

			m_pCurrentWeaponControl->OnAttackIdleWeapon();
		}
	}

	return slot;
}

INT32 CGameCharaWeaponContext::ChangeToEquipedWeapon( INT32 nSlot, UINT8 weapponLimitFlag)
{	
	UINT32 noWeapon = 0;
	INT32 nEquipedWeaponSlot = -1;
	USER_INFO_EQUIPMENT_WEAPON EquipWeapon;

	if( BattleSlotContext::i()->GetMySlotIdxEx() == nSlot)
	{
		UserInfoContext::i()->GetMyUserInfoEquipmentWeapon(EquipWeapon);
	}
	else
	{
		EquipWeapon.Reset();
	}


	//	������� ���Ⱑ �����ϴ��� �˻��� ���
	//	�ֹ���
	if( EquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_PRIMARY ] > 0 && TESTBIT( weapponLimitFlag, LOCK_PRIMARY))
	{
		nEquipedWeaponSlot = 0;
		noWeapon = EquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_PRIMARY ];
	}
	//	��������
	else if( EquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_SECONDARY ] > 0 && TESTBIT( weapponLimitFlag, LOCK_SECONDARY))
	{
		nEquipedWeaponSlot = 1;
		noWeapon = EquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_SECONDARY ];
	}
	//	��������
	else if( EquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_MELEE ] > 0 && TESTBIT( weapponLimitFlag, LOCK_MELEE))
	{
		nEquipedWeaponSlot = 2;
		noWeapon = EquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_MELEE ];
	}
	//	��ô����
	else if( EquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING1 ] > 0 && TESTBIT( weapponLimitFlag, LOCK_THROW))
	{
		nEquipedWeaponSlot = 3;
		noWeapon = EquipWeapon._nomelee;
	}
	//	Ư������
	else if( EquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING2 ] > 0)
	{
		nEquipedWeaponSlot = 4;
		noWeapon = EquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_MELEE ];
	}

	if( nEquipedWeaponSlot >= 0 && nEquipedWeaponSlot < WEAPON_SLOT_COUNT)
	{
		for(INT32 i=0; i< nEquipedWeaponSlot; i++)
		{	// ��� ���� ����
			m_pCurrentWeaponControl->OnDeleteWeapon( static_cast<WEAPON_SLOT_TYPE>(i));
		}

		// �����
		m_pWeapon[ nEquipedWeaponSlot] = g_pWeaponManager->QueryWeapon( m_pOwner, noWeapon);
		
		m_pCurrentWeaponControl->OnAttackIdleWeapon();
	}	

	// ����� ���� ���� ��ȯ
	return nEquipedWeaponSlot;
}

bool CGameCharaWeaponContext::_Change_Color( COLORREAL * pDiff, COLORREAL * pSpec, REAL32 alpha /*= 1.f*/)
{	
	if( pDiff != nullptr && pSpec != nullptr)
		return m_pCurrentWeaponControl->SetMaterialColor( getCurrentSlotNum(), pDiff, pSpec) == true;

	if( alpha >= 0.f)
		m_pCurrentWeaponControl->SetBlendAlphaAll( alpha);

	return true;
}

bool CGameCharaWeaponContext::_ChangeWeapon( WEAPON_SLOT_TYPE slot, bool bDirect /*= false*/)
{
	if( bDirect)
	{	// ���� �����ӿ��� ���⸦ ��ü
		if( m_pCurrentWeaponControl->OnSelectWeapon( slot))
		{
			m_pCurrentWeaponControl->OnAttackIdleWeapon();
			return true;
		}
	}
	else
	{	// ���� �����ӿ� ���⸦ ��ü
		if( m_pCurrentWeaponControl->OnPreSelectWeapon( slot))
		{
			m_pCurrentWeaponControl->OnAttackIdleWeapon();
			return true;
		}
	}

	return false;
}

bool CGameCharaWeaponContext::_Create_Weapon( WEAPON_SLOT_TYPE slot, T_ItemID Itemid)
{
	//// 1.5 Check Debug - C4 �� slot index �� �ʹ� ū���̶�,
	// ���������� ������� �ʴ� ������ �ӽ÷� �߰��մϴ�. ���߿� �����Ҳ�����. EN_���õ
	if(slot == WEAPON_SLOT_MISSION)
	{
		slot = GetWeaponSlot_WeaponItemIDFunc(DEFAULT_WEAPON_MISSON);
	}
	
	bool bRv = m_pCurrentWeaponControl->OnCreateWeapon( slot, Itemid);
	
	m_pCurrentWeaponControl->OnAttackIdleWeapon();

	return bRv;
}

void CGameCharaWeaponContext::_Delete_Weapon( WEAPON_SLOT_TYPE slot)
{
	m_pCurrentWeaponControl->OnDeleteMultiWeapon( slot);
}

void CGameCharaWeaponContext::_Detach_Weapon( WEAPON_SLOT_TYPE slot)
{
	m_pCurrentWeaponControl->OnDetachWeapon( slot);
}

void CGameCharaWeaponContext::_Detach_Weapon( WeaponBase * pWeapon)
{
	if( getBackpack() == pWeapon )
	{	
		// ��� ������ϴ�.
		m_pCurrentWeaponControl->OnDetachPurchasePrimWeapon();
	}
	//else if( getSidepack() == pWeapon)
	//{
	//	// ���⸦ ������ϴ�.
	//	m_pCurrentWeaponControl->OnDetachPurchaseSideWeapon();
	//}
}

bool CGameCharaWeaponContext::_Drop( WEAPON_SLOT_TYPE slot, VEC3D * pPos, bool bNextWeapon, bool bDirectPickup)
{
	if( m_pCurrentWeaponControl->OnWeaponDropToWorld( slot, pPos, bNextWeapon, bDirectPickup))
	{
		m_pCurrentWeaponControl->OnAttackIdleWeapon();
		return true;
	}

	return false;
}

bool CGameCharaWeaponContext::_Extension( bool bEnable)
{
	if( bEnable )
		return m_pCurrentWeaponControl->OnExtActivate() == true;
	else
		return m_pCurrentWeaponControl->OnExtDeactivate() == true;
}

bool CGameCharaWeaponContext::_Fire( WEAPON::FIRE_ORDER order, VEC3D * pPos /*= nullptr*/, VEC3D * pTarget /*= nullptr*/)
{
	switch( order)
	{
	case WEAPON::PRIMARY_FIRE :
		return m_pCurrentWeaponControl->OnFire( pPos, pTarget) == true;
	case WEAPON::SECONDARY_FIRE :
		return m_pCurrentWeaponControl->OnSecFire( pPos, pTarget) == true;
	case WEAPON::READY_FIRE :
		return m_pCurrentWeaponControl->OnFireReady() == true;
	}

	return false;
}

void CGameCharaWeaponContext::_Idle( void)
{
	if( getCurrent() != nullptr)
		getCurrent()->ReturnToReady();
}

bool CGameCharaWeaponContext::_Load_Bullet( void)
{
	if( m_pCurrentWeaponControl->OnLoadBullet() )
	{
		m_pCurrentWeaponControl->OnAttackIdleWeapon();
		return true;
	}

	return false;
}

bool CGameCharaWeaponContext::_Load_Magazine( void)
{
	if( m_pCurrentWeaponControl->OnLoadMagazine())
	{
		 m_pCurrentWeaponControl->OnAttackIdleWeapon();
		 return true;
	}
	return false;
}

bool CGameCharaWeaponContext::_Load_Magazine_Dual( void)
{
	if( m_pCurrentWeaponControl->OnLoadMagazineDual() )
	{
		m_pCurrentWeaponControl->OnAttackIdleWeapon();
		return true;
	}

	return false;
}

bool CGameCharaWeaponContext::_Load_Magazine_Ready( void)
{
	if( m_pCurrentWeaponControl->OnLoadMagazine_Ready())
	{
		m_pCurrentWeaponControl->OnAttackIdleWeapon();
		return true;
	}

	return false;
}

bool CGameCharaWeaponContext::_Remove_Shell( void)
{
	return m_pCurrentWeaponControl->OnRemoveShell() == true;
}

void CGameCharaWeaponContext::_Reset_Weapons( void)
{
	m_pCurrentWeaponControl->OnResetWeapon();
}

bool CGameCharaWeaponContext::_Respawn( void)
{	
	if( m_pCurrentWeaponControl->OnProcessRespawn() )
	{
		m_pCurrentWeaponControl->OnAttackIdleWeapon();
		return true;
	}

	return false;
}

bool CGameCharaWeaponContext::_Pickup( WeaponBase * pWeapon, UINT8 nSyncNumber, bool bCheckDropList, bool bPushWeaponList)
{
	if ( m_pCurrentWeaponControl->OnWeaponTake( pWeapon, nSyncNumber, bCheckDropList, bPushWeaponList))
	{
		m_pCurrentWeaponControl->OnAttackIdleWeapon();
		return true;
	}

	return false;
}

bool CGameCharaWeaponContext::_Zoom( void)
{
	return m_pCurrentWeaponControl->OnZoom() == true;
}


void CGameCharaWeaponContext::PushThrowWeaponIndex(INT32 nIdx)
{
	m_deqThrowList.push_back(nIdx);
}

INT32 CGameCharaWeaponContext::GetThrowWeaponIndex()
{
	if (m_deqThrowList.empty())
		return -1;

	INT32 nIdx = (*m_deqThrowList.begin());
	m_deqThrowList.pop_front();

	return nIdx;
}

void CGameCharaWeaponContext::PopThrowWeaponIndex()
{
	m_deqThrowList.pop_front();
}



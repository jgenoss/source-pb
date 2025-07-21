#include "pch.h"
#include "IGShopItemController.h"
#include "GameCharaWeaponContext.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "UserInfoContext.h"
#include "BattleSlotContext.h"

#define MAX_CASHITEM_COUNT	23

IGShopItemController* IGShopItemController::m_pInstance = 0;

IGShopItemController::IGShopItemController()
{
	memset(&m_Base_ItemInfo_Buf, 0, sizeof(PACKET_PROTOCOL_BASE_EQUIPMENT_ACK));
	memset(&m_Slot_ItemInfo_Buf, 0, sizeof(PACKET_H_PROTOCOL_SLOT_EQUIPMENT_ACK));
}

void IGShopItemController::Apply()
{
	ApplyWeapon();
	ApplyWeaponEndurance();
	ApplyEquipment();
	ApplyCashItem();
	ApplyTitle();
}

void IGShopItemController::ApplyWeapon()
{
	if (_CanApply() == false) return;

	NET_CHARA_INFO* charaNetInfo = _GetCharaNetInfo();

	UINT32 oldWeaponList[WEAPON_SLOT_COUNT] = {0, };
	UINT32 curWeaponList[WEAPON_SLOT_COUNT] = {0, };


	// ��Ʈ��ũ �������� : �ٷ� �����ϱ� ����(�÷��� ���� ��� ĳ���͸� �������)

	for(INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
		oldWeaponList[ i] = charaNetInfo->getWeaponList( (WEAPON_SLOT_TYPE)i)->m_TItemID;

	CHARA_WEAPON_INFO_OTHER* _Info = &m_Slot_ItemInfo_Buf.m_Slot_Equipment.m_PosMultiSlot.m_CharaEquipInfo.m_CharWeaponInfo;
	charaNetInfo->SetWeaponList( _Info);

	if (_IsMyCharacter())
	{
		UI_CHARA_INFO* myInfo = UserInfoContext::i()->SetMyUICharaInfo();
		myInfo->SetWeaponList( charaNetInfo->getWeaponList( WEAPON_SLOT_PRIMARY));
	}

	for(INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
		curWeaponList[i] = charaNetInfo->getWeaponList( (WEAPON_SLOT_TYPE) i)->m_TItemID;

	_ChangeWeapon( _FindSlotWillBeApplied(oldWeaponList, curWeaponList));
}

void IGShopItemController::ApplyWeaponEndurance()
{
	if (_CanApply() == false) return;

	_GetCharaNetInfo()->setEndurance( m_Slot_ItemInfo_Buf.m_Slot_Equipment.m_ui8WeaponEndurance);

	if (_IsMyCharacter())
	{
		//UI_CHARA_INFO* myInfo = UserInfoContext::i()->SetMyUICharaInfo();
		//myInfo->setEndurance( m_Slot_ItemInfo_Buf.m_Slot_Equipment.m_ui8WeaponEndurance);
	}
}

void IGShopItemController::ApplyEquipment()
{
	if (_CanApply() == false) return;

	_SetEquipToNetChara(_GetCharaNetInfo());

	if (_IsMyCharacter())
	{
		//_SetEquipToNetChara(UserInfoContext::i()->SetMyCharaInfo());
	}

	const CHARA_PARTS_INFO* net_chara_parts_info = _GetCharaNetInfo()->getCharaParts();
	net_chara_parts_info->CopyTo( _SetCharaInfo()->SetParts()); //global_chara_parts_info = net_chara_parts_info; �� ���� ����..

	//common parts�� ���ؼ��� ��� �ؾߵ��� �𸣰ڴ�.
	//const COMMON_PARTS_INFO* net_chara_common_parts_info = _GetCharaNetInfo()->getCommonParts();

	g_pCharaManager->SetCharaEquipSet(_GetChara(), _GetCharaInfo());
}

void IGShopItemController::ApplyCashItem()
{
	if (_CanApply() == false) return;

	UINT32 oldItemFlag		= _GetSlotInfo()->_Item;
	UINT32 curItemFlag		= m_Slot_ItemInfo_Buf.m_EquipItem;
	_SetSlotInfo()->_Item	= curItemFlag;

	// �����κ��� ���� ���� �� �ִ� �������� �� 23���Դϴ�(SLOT_ITEM_VALENTINE_CHOCOLATE �˻�)
	// �Ʒ��� HP�� ��ź�� ���� ���� 6�� �������� ĳ���� ���� �� HUD�� ��� �����ؾ� �մϴ�.
	// ������ 17�� �������� ��� �������� �κ��丮 �˻��� ���� �ڵ� ����˴ϴ�.
	// * ��� �������� �ߺ� ������� �ʽ��ϴ�.
	switch (_FindCashItemFlagPurchased(oldItemFlag, curItemFlag))
	{
	case SLOT_ITEM_VALENTINE_CHOCOLATE:
	case SLOT_ITEM_KETUPAT:
	case SLOT_ITEM_MEGA_HP10:
	case SLOT_ITEM_MEGA_HP5:
		{
			INT32 curHP = _GetChara()->getCurHPAppliedCashItem();

   			_SetCharaInfo()->SetHP(static_cast<REAL32>(curHP));
			_SetGameCharaInfo()->_tNetData._tHP.setHP(static_cast<UINT16>(curHP));	
		}
		break;
	case SLOT_ITEM_AMMO_UP:
	case SLOT_ITEM_INCREASE_GRENADE_SLOT:
	case SLOT_ITEM_INCREASE_THR2_SLOT:
		{
			CGameCharaWeaponContext* weaponCtx = _GetChara()->getWeaponContext();
			if (weaponCtx == nullptr) break;

			WeaponBase* weapon = weaponCtx->getWeapon(weaponCtx->getCurrentSlotNum());
			if (weapon == nullptr) break;
				
			weapon->ApplyBulletCountWithCashItem();
		}
		break;
	default: break;
	}

	ApplyCharaHUD();
}

void IGShopItemController::ApplyTitle()
{
	if (_CanApply() == false) return;

	if (_IsMyCharacter())
	{
	}
}

void IGShopItemController::ApplyCharaHUD()
{
}

WEAPON_SLOT_TYPE IGShopItemController::_FindSlotWillBeApplied(UINT32* old, UINT32* cur)
{
	for (INT32 i = 0; i < WEAPON_SLOT_COUNT-1; i++)
	{
		if (old[i] != cur[i]) return static_cast<WEAPON_SLOT_TYPE>(i);
	}

	return WEAPON_SLOT_UNKNOWN;
}

// changeDirect
//  - true(����Ʈ) : ���� ���� �� ĳ���Ϳ� �ٷ� ����.
//  - false        : ĳ���Ͱ� �����ϰ� �ִ� ����� ������ ������ ����.
void IGShopItemController::_ChangeWeapon( WEAPON_SLOT_TYPE slot, bool changeDirect)
{
	if (slot <= WEAPON_SLOT_UNKNOWN || WEAPON_SLOT_COUNT <= slot) return;

	CGameCharaBase* chara = _GetChara();
	if( chara->Cmd_CreateWeapon_Only(slot, _GetCharaNetInfo()->getWeaponItemID( (WEAPON_SLOT_TYPE)slot)) == false) return;

	// trickSlot : �ΰ��� �������� ������ ���⸦ �����Ϸ��� Cmd_ChangeWeapon()�� ȣ��Ǿ��
	//			   �մϴ�. Cmd_ChangeWeapon()�� ��ü�ϰ��� �ϴ� ���⽽�԰� ��� ���� ĳ������ ���⽽����
	//			   �����ϸ� ���⸦ ���� ���̴� ������ �������� �ʽ��ϴ�. ���� ĳ������ ����
	//			   ���� ������ �����ϴ� Ʈ���� ����մϴ�(trickSlot = slot+1)
	CGameCharaWeaponContext* weaponCtx = chara->getWeaponContext();
	WEAPON_SLOT_TYPE curWeaponSlot = weaponCtx->getCurrentSlotNum();
	if (changeDirect)
	{
		if (slot == curWeaponSlot)
		{	
			WEAPON_SLOT_TYPE trickSlot = static_cast<WEAPON_SLOT_TYPE>(curWeaponSlot+1);
			_ChangeWeaponSlot(trickSlot);
		}

		chara->Cmd_ChangeWeapon(slot, false);
	}
	else
	{
		WEAPON_SLOT_TYPE oldWeaponSlot = curWeaponSlot;
		WEAPON_SLOT_TYPE trickSlot = static_cast<WEAPON_SLOT_TYPE>(curWeaponSlot+1);
		_ChangeWeaponSlot(trickSlot);

		chara->Cmd_ChangeWeapon(oldWeaponSlot, false);
	}
}

void IGShopItemController::_ChangeWeaponSlot( WEAPON_SLOT_TYPE slot)
{
	CGameCharaWeaponContext* weaponCtx = _GetChara()->getWeaponContext();

	weaponCtx->setCurrentSlotNum(slot);
	weaponCtx->setSwapSlotNum(slot);
}

UINT32 IGShopItemController::_FindCashItemFlagPurchased(UINT32 oldItemFlag, UINT32 curItemFlag)
{
	if(oldItemFlag == curItemFlag) return 0;

	for (INT32 i = 0; i < MAX_CASHITEM_COUNT; i++)
	{
		if (((oldItemFlag>>i) & 0x0001) != ((curItemFlag>>i) & 0x0001))
			return 0x0001<<i;
	}

	return 0; // flag�� 1���� �����ϱ� ������
}

bool IGShopItemController::_CanApply() const
{
	INT32 charaNetIndex = m_Slot_ItemInfo_Buf.m_Slot_Equipment.m_i32SlotIdx;
	if (charaNetIndex < 0 || SLOT_MAX_COUNT <= charaNetIndex) return false;

	CGameCharaBase* chara = _GetChara();
	if (chara == nullptr)  return false;
	if (chara->isNPC() || chara->IsDino()) return false;

	return true;
}

void IGShopItemController::_SetEquipToNetChara(NET_CHARA_INFO* charaNetInfo) const
{
	if (charaNetInfo == nullptr) return;

	charaNetInfo->setCharaParts( &m_Base_ItemInfo_Buf.m_EquipInfo.m_CharPartsInfo );
}

NET_CHARA_INFO*		IGShopItemController::_GetCharaNetInfo() const 
{ 
	return const_cast<NET_CHARA_INFO*>(BattleSlotContext::i()->getNetCharaInfo( get_chara_net_index() ));
}

const GAMEINFO_CHARACTER*	IGShopItemController::_GetGameCharaInfo() const 
{ 
	return BattleSlotContext::i()->getGameInfo_Chara( get_chara_net_index() );
}

GAMEINFO_CHARACTER* IGShopItemController::_SetGameCharaInfo()
{
	return BattleSlotContext::i()->setGameInfo_Chara( get_chara_net_index() );
}

const SLOT_INFO*	IGShopItemController::_GetSlotInfo()  const
{ 
	return BattleSlotContext::i()->getSlotInfo( get_chara_net_index() ); 
}

SLOT_INFO* IGShopItemController::_SetSlotInfo()
{
	return BattleSlotContext::i()->setSlotInfo( get_chara_net_index() ); 
}

bool		IGShopItemController::_IsMyCharacter() const 
{ 
	return (m_Slot_ItemInfo_Buf.m_Slot_Equipment.m_i32SlotIdx == BattleSlotContext::i()->getMySlotIdx()) ; 
}

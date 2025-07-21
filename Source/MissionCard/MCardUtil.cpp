#include "pch.h"
#include "MCardUtil.h"
#include "MCardMgr.h"

#include "MCardActionInfo.h"

namespace MCardUtil
{
	void Update_Recv_PROTOCOL_BASE_GET_USER_INFO_ACK( const QUESTING_INFO* questingInfo, const UINT8 (&aQuestCtxActive)[ MAX_QUEST_ALL_USE_NEW ])
	{
		MCardMgr::i()->UpdateUserMCardInfoFromServer(questingInfo, aQuestCtxActive);	
	}

	void Update_Recv_PROTOCOL_BASE_QUEST_GET_ACK(INT32 activeSetIdx, INT32 cardIdx, INT32 questIdx, const QUEST_CTX& ctx)
	{
		MCardMgr::i()->ModifyQuestFromServer(activeSetIdx, cardIdx, questIdx, ctx);
	}

	void Update_Recv_PROTOCOL_BASE_QUEST_GET_ITEM_ACK(INT32 activeSetIdx, INT32 cardIdx, const QUEST_ITEM_INFO_NEW& questItemInfo)
	{
		MCardMgr::i()->ModifyCardPrizeFromServer(activeSetIdx, cardIdx, questItemInfo);
	}

	void Update_Recv_PROTOCOL_BASE_QUEST_GET_INFO_ACK(const QUESTING_INFO* questingInfo, INT32 setSlot)
	{
		MCardMgr::i()->ActivateCardSetSlotFromServer(questingInfo, setSlot);
	}

	void Update_Recv_PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK(const QUESTING_INFO* questingInfo, INT32 setSlot)
	{
		MCardMgr::i()->ActivateCardSetSlotFromServer(questingInfo, setSlot);	// �� �Լ��� ��������.
	}

	void Update_Recv_PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK(const QUESTING_INFO* questingInfo)
	{
		MCardMgr::i()->UpdateCardSetSlotsFromServer(questingInfo);
	}

	void Update_Recv_PROTOCOL_BASE_QUEST_CHANGE_ACK(UINT8 ui8FinishFlagAndProcessedQuestIdx, UINT8 ui8ProcessedTaskCount)
	{
		UINT8 u8Complete = ui8FinishFlagAndProcessedQuestIdx & 0xf0;
		UINT8 u8Idx = ui8FinishFlagAndProcessedQuestIdx & 0x0f;
		
		GlobalFunc::PB_TRACE("FinishFlag %x QuestIdx %d TaskCount %d", u8Complete, u8Idx, ui8ProcessedTaskCount);
		
		MCardMgr::i()->UpdateSelectedCardQuestFromServer_OnBattle( u8Idx, ui8ProcessedTaskCount, (u8Complete == 0xf0) );

	}

	void Update_Recv_PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK(bool isFinalCardCompleted)
	{
		MCardMgr::i()->ReceiveSelectAndResultFromServer(isFinalCardCompleted);
	}
	
	void Update_Recv_PROTOCOL_BATTLE_ENDBATTLE_ACK()
	{
		MCardMgr::i()->ReceiveSelectAndResultFromServer(false, true);
	}

	void Update_Recv_PROTOCOL_BATTLE_STARTBATTLE_ACK()
	{
		MCardMgr::i()->UpdateAllHUDCallbackOnBattle();
	}

	void Update_Recv_PROTOCOL_BATTLE_MISSION_ROUND_START_ACK()
	{
		MCardMgr::i()->UpdateAllHUDCallbackOnBattle();
	}
	

	// ���� ������ ���̽��� Ŭ����̵� 3��, �������̵� 1����..
	// 1. EVENT_GIVEUP_BATTLE�� Ŭ�󿡼� SEND�Ҷ� ���ÿ�..
	// 2. ������ǥ ����� �����ڰ� �ڵ����� ���������� �Ǵ� ���..( PROTOCOL_BATTLE_NOTIFY_BE_KICKED_BY_KICKVOTE_ACK )
	// 3. EVENT_M_TIMEOUTCS (��Ʋ�� �������� ��ġ�� ���) .. 
	// 4. EVENT_M_TIMEOUTCN (��Ʋ�� �������� ��ġ�� ���) .. 

	void Update_Process_GiveupBattle()
	{
		MCardMgr::i()->ProcessGiveupBattle();
	}
	
	bool RemoveSelectedCardSetAfterFinalPrize()
	{
		return MCardMgr::i()->RemoveSelectedCardSetAfterFinalPrize();
	}
	
	bool RemoveCardSetAfterFinalPrize(INT32 cardSetSlot)
	{
		return MCardMgr::i()->RemoveCardSetAfterFinalPrize(cardSetSlot);
	}

	i3::rc_wstring GetCurrHUDDesc(INT32 idxQuest)
	{
		return MCardMgr::i()->GetCurrHUDDesc(idxQuest);
	}


	void Update_Recv_PROTOCOL_BASE_GET_SYSTEM_INFO_ACK(UINT8 ui8ActiveQuestCount, UINT32* pui32ActiveQuest, const UINT32 (&ui32QuestCardGoodsID)[QUEST_CARDSET_TYPE_COUNT])
	{
		MCardMgr::i()->UpdateSystemInfoFromServer(ui8ActiveQuestCount, pui32ActiveQuest, ui32QuestCardGoodsID);
	}

	i3::rc_wstring GetPrizeDescription(BADGE_TYPE t)
	{
		return MCardMgr::i()->GetPrizeDescription(t);
	}

	i3::rc_wstring	FindMCardActionName(ACTION_ID actionID)
	{
		INT32 intID = INT32(actionID);
		INT32 num   = MCardMgr::i()->GetMCardActionInfoCountByID();
		
		if (intID < 0 || intID >= num)
			return i3::rc_wstring();
		
		return MCardMgr::i()->GetMCardActionInfoByID(actionID)->m_wstrActionName;
	}

	INT32		GetMCardSetCount_Server() 
	{
		return MCardMgr::i()->GetMCardSetCount_Server();
	}

	INT32		GetMCardSetType_Server(INT32 idx) 		// �μ��� ��Ÿ���� �ƴϰ� ���ϰ��� ��Ÿ����..
	{
		return MCardMgr::i()->GetMCardSetType_Server(idx);
	}

	bool		IsExistMCardSet_Server(INT32 MCardSetType) 
	{
		return MCardMgr::i()->IsExistMCardSet_Server(MCardSetType);
	}

	MCardSet*		GetSelectedMCardSet()
	{
		return MCardMgr::i()->GetSelectedMCardSet();
	}

	INT32		GetSelectedMCardSetIndex()
	{
		return MCardMgr::i()->GetSelectedMCardSetIndex();
	}

	MCard*			GetSelectedMCard()
	{
		return MCardMgr::i()->GetSelectedMCard();
	}

	const MCardSetInfo*	FindMCardSetInfoBySetType( INT32 setType )
	{
		return MCardMgr::i()->FindMCardSetInfoBySetType(setType);
	}

	bool		IsServerListIdxCurrentSelectedSet(INT32 serverIdx)
	{
		INT32 slotIdx =	FindCardSetSlotWithServerListIdx(serverIdx);
		return slotIdx != -1 && slotIdx == MCardMgr::i()->GetSelectedMCardSetIndex();
	}


	MCardSet*		GetMCardSet(INT32 slotIdx)
	{
		return MCardMgr::i()->GetMCardSet(slotIdx);
	}

	bool			HasMCardSet(INT32 cardSetType)
	{
		return MCardMgr::i()->HasMCardSet(cardSetType);
	}

	INT32		GetServerIndexWithMCardSetType(INT32 MCardSetType)
	{
		return MCardMgr::i()->GetServerIndexWithMCardSetType(MCardSetType);
	}

	INT32		FindCardSetSlotWithCardSetType(INT32 cardSetType)
	{
		return MCardMgr::i()->FindCardSetSlotWithCardSetType(cardSetType);
	}

	INT32		FindCardSetSlotWithServerListIdx(INT32 serverIdx)
	{
		INT32 cardSetType = GetMCardSetType_Server(serverIdx);
		if (cardSetType == QUEST_CARDSET_TYPE_NONE)
			return -1;
		return FindCardSetSlotWithCardSetType(cardSetType);
	}
	
	bool		LoadPef(const i3::rc_string& szFileName, const i3::rc_string& szTextFileTitle)
	{
		return MCardMgr::i()->LoadRegistryFile(szFileName, szTextFileTitle);
	}

	INT32		FindNextAvailableCardSetSlotIndex()
	{
		return MCardMgr::i()->FindNextAvailableCardSetSlotIndex();
	}

	MCardSet*	FindNextAvailableCardSet()
	{
		INT32 next_slotIdx = MCardMgr::i()->FindNextAvailableCardSetSlotIndex();
		if (next_slotIdx == -1)
			return nullptr;
		return MCardMgr::i()->GetMCardSet(next_slotIdx);
	}

	bool		SelectActiveCardSetToServerWithSetType( INT32 setType)
	{
		if (setType == QUEST_CARDSET_TYPE_NONE)
			return false;

		INT32 slotIdx = MCardUtil::FindCardSetSlotWithCardSetType(setType);
		if (slotIdx == -1)
			return false;
		
		MCardUtil::SelectActiveCardSetToServer(slotIdx);
		return true;
	}

	void		SelectActiveCardSetToServer(INT32 selSetIdx)
	{
		MCardMgr::i()->SelectActiveCardSetToServer(selSetIdx);
	}

	void		SelectActiveCardSetToServer(INT32 selSetIdx, INT32 selCardIdx)
	{
		MCardMgr::i()->SelectActiveCardSetToServer(selSetIdx, selCardIdx);
	}

	void		SelectActiveCardToServer(INT32 selCardIdx)
	{
		MCardMgr::i()->SelectActiveCardToServer(selCardIdx);
	}


	void		PopRewardMark(const MCardInfo*& outInfo, bool& isAnyQuestCompleted, bool& isCardRewardExist, bool& isFinalCard)
	{
		MCardMgr::i()->PopRewardMark(outInfo, isAnyQuestCompleted, isCardRewardExist, isFinalCard);
	}

	void SetMCardCallback(MCardCallback* callback)
	{
		MCardMgr::i()->SetCallback(callback);
	}


	enum ITEM_CLASS_TYPE_V10
	{
		ITEM_CLASS_UNKNOWN_V10	= 0,		// ����..

		ITEM_CLASS_ASSAULT_V10	= 1,
		ITEM_CLASS_SMG_V10,
		ITEM_CLASS_SNIPER_V10,
		ITEM_CLASS_SHOTGUN_V10,
		ITEM_CLASS_MACHINE_V10,
		ITEM_CLASS_HANDGUN_V10,
		ITEM_CLASS_KNIFE_V10,
		ITEM_CLASS_GRENADE_V10,
		ITEM_CLASS_THROWING_V10,			// �������� Ȯ���� WEAPON_USAGE_TYPE���� ��Ī��..


		ITEM_CLASS_CHARA_V10,			// CHARACTER_CLASS_TYPE ��� UsageType ���..
		ITEM_CLASS_HEAD_V10,			// CHARACTER_CLASS_TYPE ��� UsageType ���..
		ITEM_CLASS_MAINTENANCE_V10,			// CASHITEM_USAGE_TYPE	
		ITEM_CLASS_EXPENDABLE_V10,			// CASHITEM_USAGE_TYPE	
		ITEM_CLASS_COUPON_V10,			// Usage�� �Ⱥ���..
		ITEM_CLASS_POINT_V10,			// ������ ���� ���� ����Ʈ ��ġ�� ��ϵ�.. #define GET_POINTITEM(ItemID)			((GET_ITEM_FLAG_USAGE(ItemID)*1000000)+(GET_ITEM_FLAG_NUMBER(ItemID)*1000)+(GET_ITEM_FLAG_TYPE(ItemID)*100))
		ITEM_CLASS_TEMP_V10, 			// �Һи�...GetItemClass_TempFunc()���� Usage�� WeaponClassType���� �������� ã�µ��ϴ�... 
		ITEM_CLASS_QUESTCARD_V10,	// �ӹ�ī��	// �Ⱦ��� ������....�����ÿ� �ʿ�..
		ITEM_CLASS_SALECOUPON_V10,	// ��������	// Usage�� ����..
		ITEM_CLASS_RSCOUPON_V10,	// �̱��̿��	// Usage�� �����..

		ITEM_CLASS_WEAPON_INGAME_V10,	// ��Ʋ���� ���̴� Ư������� (ex> ���Ϸ�ó, C4 ��)	// ����� �����̹Ƿ�, WEAPON_USAGE_TYPE���δ� ó��..

		ITEM_CLASS_COUNT_V10		// MAX���� ��
	};

	UINT32  ConvertItemID_V10_TO_V20( UINT32 itemID_V10_V20 )
	{
		if (itemID_V10_V20 == 0)		// 0������ ���̵��� �׳� ����..
			return 0;

		// 1. �ϴ��� V1�������� V2�������� ������ �ʿ���..  V2�����̶�� �����Ű�� �ʰ� �״�� �����ϸ� �ɰ� ����..

		UINT32 class_V10 = itemID_V10_V20 / 100000000;

		if (class_V10 == 0)		// V1������ ITEM_CLASS_UNKNOWN�ϼ��� ������, �׷� ���� ���⼭�� ������ ���̸�, V2 itemID�� �����Ѵ�.
			return itemID_V10_V20;

		// 2. �� ���������� V1 itemID�� �����ϰ�, V2 itemID�� �������ϵ��� �Ѵ�..

		UINT32 usage_V10 = (itemID_V10_V20 % 100000000) / 1000000;

		UINT32 typeNumber = itemID_V10_V20 % 1000000;			// classType�� number�� ������ ���� ������ �׳� �ϳ��� ������ �Ѵ�..
		UINT32 typeCommon = (itemID_V10_V20 % 1000000) / 1000;
		//	UINT32 numberCommon = itemID_V10_V20 % 1000;

		UINT32 result_ItemType_V20 = 0;

		switch(class_V10)
		{
		case ITEM_CLASS_ASSAULT_V10:
		case ITEM_CLASS_SMG_V10:
		case ITEM_CLASS_SNIPER_V10:
		case ITEM_CLASS_SHOTGUN_V10:
		case ITEM_CLASS_MACHINE_V10:
		case ITEM_CLASS_HANDGUN_V10:
		case ITEM_CLASS_KNIFE_V10:
		case ITEM_CLASS_GRENADE_V10:
		case ITEM_CLASS_THROWING_V10:			// �������� Ȯ���� WEAPON_USAGE_TYPE���� ��Ī��..	
		case ITEM_CLASS_WEAPON_INGAME_V10:
			{
				// CommonDef.h�� �Լ��� WeaponClass���� ITEM_TYPE�� ������ ���������� ����..
				result_ItemType_V20 = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)typeCommon);
			}
			break;

		case ITEM_CLASS_CHARA_V10:
		case ITEM_CLASS_HEAD_V10:
			{
				I3PRINTLOG(I3LOG_WARN, "Character or Head can't convert V10 itemID Into V20 itemID.... 1.0 itemID = %d", itemID_V10_V20);
				return 0;			// ���� �������� ����...		���� ������ �����ϰ�����...������ �ΰ�,���� ������ ī�忡 ����� ����ũ�� ������....�����..
			}
			break;

		case ITEM_CLASS_MAINTENANCE_V10:
			{
				result_ItemType_V20 = ITEM_TYPE_MAINTENANCE;
			}
			break;

		case ITEM_CLASS_EXPENDABLE_V10:
			{
				switch( usage_V10 )
				{	
				case CASHITEM_USAGE_MAINTENANCE:	result_ItemType_V20 = ITEM_TYPE_WRAP_PERIOD;	break;
				case CASHITEM_USAGE_EXPENDABLES:	result_ItemType_V20 = ITEM_TYPE_WRAP_COUNT;		break;
				default:							result_ItemType_V20 = ITEM_TYPE_UNKNOWN;		break;
				}
			}
			break;

		case ITEM_CLASS_POINT_V10:
			result_ItemType_V20 = ITEM_TYPE_POINT;	break;
			break;

		default:
			I3ASSERT_0;
			return 0;
		}

		return result_ItemType_V20*100000 + typeNumber;
	}

}

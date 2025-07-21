#pragma once

#include "StepProfileDef.h"

struct MCardSetInfo;
struct MCardInfo;
struct MCardActionInfo;

class MCardSet;
class MCard;
class MCardCallback;

class MCardMgr : public i3::shared_ginst<MCardMgr>
{
public:
	
	MCardMgr();
	~MCardMgr();

	void		SetCallback(MCardCallback* callback);

	INT32				GetMCardSetCountBySetType() const;
	const MCardSetInfo*	FindMCardSetInfoBySetType( INT32 setType ) const;


	i3::rc_wstring GetPrizeDescription(BADGE_TYPE t) const; 

	MCardSet*		GetSelectedMCardSet() const;
	MCard*			GetSelectedMCard() const;
	INT32			GetSelectedMCardSetIndex() const;
	MCardSet*		GetMCardSet(INT32 slotIdx) const;
	bool			HasMCardSet(INT32 cardSetType) const;

	i3::rc_wstring GetCurrHUDDesc(INT32 idxQuest) const;
		

	INT32		FindCardSetSlotWithCardSetType(INT32 cardSetType) const;

	void		SelectActiveCardSetToServer(INT32 selSetIdx);	
	void		SelectActiveCardSetToServer(INT32 selSetIdx, INT32 selCardIdx);
	void		SelectActiveCardToServer(INT32 selCardIdx);

	INT32		FindNextAvailableCardSetSlotIndex() const;

	bool		LoadRegistryFile(const i3::rc_string& strFileName, const i3::rc_string& scriptTextFileTitle);	

	void		UpdateSystemInfoFromServer(UINT8 ui8ActiveQuestCount, UINT32* pui32ActiveQuest, const UINT32 (&ui32QuestCardGoodsID)[QUEST_CARDSET_TYPE_COUNT]);

	void		UpdateUserMCardInfoFromServer(const QUESTING_INFO* questingInfo, const UINT8 (&aQuestCtxActive)[ MAX_QUEST_ALL_USE_NEW ]);
	void		ModifyQuestFromServer(INT32 activeSetIndex, INT32 cardIdx, INT32 questIdx, const QUEST_CTX& ctx);
	void		ModifyCardPrizeFromServer(INT32 activeSetIndex, INT32 cardIdx, const QUEST_ITEM_INFO_NEW& questItemInfo);
	void		ActivateCardSetSlotFromServer(const QUESTING_INFO* questingInfo, INT32 setSlot);
	void		UpdateCardSetSlotsFromServer(const QUESTING_INFO* questingInfo);

	void		UpdateSelectedCardQuestFromServer_OnBattle(INT32 questIdx, INT32 completeCountOfQuest, bool isQuestComplete);

	void		ProcessGiveupBattle();
	void		RequestSelectActiveCardSetToServer(UINT32 activeCardsetIdx);		// �����ϰ� �Ƿ��� �����κ��� �ް� ó���ϴ°� ������ ������..
	
	void		ReceiveSelectAndResultFromServer(bool isFinalCardCompleted, bool isEndProtocal = false );		// Ŭ��ۼ����� �������ε� ���̰�, ��Ʋ ���� ���� �Ŀ��� �Ϲ������� �ޱ⵵ �ϴ� �� ����..
	void		SendClientSelectStateToServer();
	void		UpdateAllHUDCallbackOnBattle();
	
	bool		RemoveSelectedCardSetAfterFinalPrize();
	bool		RemoveCardSetAfterFinalPrize(INT32 cardSetSlot);

	const MCardActionInfo*	GetMCardActionInfoByID( ACTION_ID actionID) const;
	size_t					GetMCardActionInfoCountByID() const;

	// �����κ��� ���� ����� _Server�� ǥ��..
	INT32		GetMCardSetCount_Server() const;
	INT32		GetMCardSetType_Server(INT32 idx) const;		// �μ��� ��Ÿ���� �ƴϰ� ���ϰ��� ��Ÿ����..
	bool		IsExistMCardSet_Server(INT32 MCardSetType) const;

	INT32		GetServerIndexWithMCardSetType(INT32 MCardSetType) const;

	void		PopRewardMark(const MCardInfo*& outInfo, bool& isAnyQuestCompleted, bool& isCardRewardExist, bool& isFinalCard);

private:	
	i3::svector<MCardSetInfo*>		m_vecCardSetInfoByID;
	i3::svector<MCardActionInfo>	m_vecActionByID;
	
	i3::svector<INT32>				m_vecCardSetTypeList_Server;
	i3::svector<INT32>				m_vecServerListIdx_BySetType;		// �����κ��� �޴� �� Ȯ�ο�..

	i3::rc_wstring				m_awstrMedalDescription[BADGE_TYPE_COUNT];
	INT32						m_selCardSetIndex;		// -1, 0,1,2,3 --> ���ų�, 4������..
	MCardSet*					m_apMCardSet[ MAX_CARDSET_PER_USER ];		// ������ ���� 4���� �����Ȱ� ����..
	MCardCallback*				m_pCallback;

	const MCardInfo*			m_pCardInfoForReward;			// ����ǥ�ø� ���� ��� ������ ���..
	bool						m_bIsAnyQuestCompleted;		// ī�� �� ����Ʈ ����Ϸᰡ �ϳ��� ������ ���..
	bool						m_bIsCardRewardExist;
	bool						m_bIsCardRewardFinal;		// ������ ī�� �����̸� ���������� �߰� ǥ���ؾߵ�.
};																				


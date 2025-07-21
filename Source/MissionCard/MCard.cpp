#include "pch.h"
#include "MCard.h"

#include "MCardSet.h"
#include "MCardSetInfo.h"
#include "MCardInfo.h"
#include "MCardQuest.h"


MCard::MCard(const MCardSet* pSet, INT32 myIndex) : m_pParent(pSet), m_myIndex(myIndex), m_pCardInfo(nullptr)
{
	*((UINT32*)m_abyQuestComplete_OffBattle) = 0;
	*((UINT32*)m_abyQuestComplete_OnBattle) = 0;

	for (INT32 i = 0 ; i < MAX_QUEST_PER_CARD_NEW ; ++i)
	{
		m_apMCardQuest[i] = new MCardQuest(this, i);
	}
}

MCard::~MCard()
{
	i3::for_each(m_apMCardQuest, m_apMCardQuest+MAX_QUEST_PER_CARD_NEW, i3::fu::delete_fun());	
}


void		MCard::SetQuestCompletion_OffBattle(INT32 questIdx, bool bComplete)
{
	I3ASSERT(questIdx < MAX_QUEST_PER_CARD_NEW);

	if ( m_apMCardQuest[questIdx]->IsActive() )
	{
		UINT8 val = (bComplete) ? 0xFF : 0x00;
		m_abyQuestComplete_OffBattle[questIdx] = val;
		m_abyQuestComplete_OnBattle[questIdx] = val;
	}
	else
	{
		m_abyQuestComplete_OffBattle[questIdx] = 0xFF;		// Active���� ������ �Ϸ��Ѱ����� ����..
		m_abyQuestComplete_OnBattle[questIdx] = 0xFF;
	}
}

void		MCard::SetQuestCompletion_OnBattle(INT32 questIdx, bool bComplete)
{
	I3ASSERT(questIdx < MAX_QUEST_PER_CARD_NEW);

	if ( m_apMCardQuest[questIdx]->IsActive() )
	{
		UINT8 val = (bComplete) ? 0xFF : 0x00;
		m_abyQuestComplete_OnBattle[questIdx] = val;
	}
	else
	{
		m_abyQuestComplete_OnBattle[questIdx] = 0xFF;		// ��Ȱ���� ������ �Ϸ� ����..
	}
}

bool		MCard::IsAllQuestCompleted_OffBattle() const
{
	return *((UINT32*)m_abyQuestComplete_OffBattle) == 0xFFFFFFFF;
}

bool		MCard::IsQuestCompleted_OffBattle(INT32 questIdx) const	// ��Ȱ��ȭ�� ���� �Ϸ�� ����..
{
	return m_abyQuestComplete_OffBattle[questIdx] == 0xFF;
}

bool		MCard::IsAllQuestCompleted_OnBattle() const
{
	return *((UINT32*)m_abyQuestComplete_OnBattle) == 0xFFFFFFFF;
}

bool		MCard::IsDifferentComplete_OnOffBattle() const
{
	return *((UINT32*)m_abyQuestComplete_OnBattle) != *((UINT32*)m_abyQuestComplete_OffBattle);
}

bool		MCard::IsQuestCompleted_OnBattle(INT32 questIdx) const	// ��Ȱ��ȭ�� ���� �Ϸ�� ����..
{
	return m_abyQuestComplete_OnBattle[questIdx] == 0xFF;
}

bool		MCard::IsBattleAvailableCard() const
{
	if (m_pCardInfo == nullptr )
		return false;

	const MCardSetInfo* pSetInfo = m_pParent->GetMCardSetInfo();
	if (pSetInfo == nullptr)
		return false;

	if (pSetInfo->setID != QUEST_CARDSET_TYPE_TUTORIAL)
	{
		if ( IsAllQuestCompleted_OffBattle() )			// �Ϸ�� ī��� ���̻� �ݺ����� ����... 
			return false;
	}
	else
	{
		if (m_myIndex == 0) 
			return true;
		MCard* prevCard =	m_pParent->GetMCard(m_myIndex - 1);
		if (prevCard->IsActive() && prevCard->IsAllQuestCompleted_OffBattle() )		// ������ �ݺ������ѵ�, ��� ����ī�� �Ϸ�� �Ǿ����..
			return true;
		return false;
	}

	return true;
}

bool	MCard::IsSelectEnableCard() const		// UI�󿡼� ���� ���������� ����..(��밡�ɰ� ������ �ٸ�)
{
	if (m_pCardInfo == nullptr)
		return false;

	const MCardSetInfo* pSetInfo = m_pParent->GetMCardSetInfo();
	if (pSetInfo == nullptr)
		return false;

	if (pSetInfo->setID != QUEST_CARDSET_TYPE_TUTORIAL)		// �Ϲ������δ� �׻� ���� ����..
		return true;
	
	// Ʃ�͸����� ���� ī�尡 �Ϸ���� ������ ���õ� �Ұ����� ���̴�..
	if (m_myIndex == 0)
		return true;
	MCard* prevCard =	m_pParent->GetMCard(m_myIndex - 1);
	if (prevCard->IsActive() && prevCard->IsAllQuestCompleted_OffBattle() )	
		return true;
	
	return false;
}




void	MCard::Activate(const MCardInfo* pCardInfo)
{
	m_pCardInfo = pCardInfo;

	if (pCardInfo)
	{
		for (INT32 i = 0; i < MAX_QUEST_PER_CARD_NEW ; ++i)
		{
			MCardQuestInfo* pQuestInfo = pCardInfo->aQuestInfo[i].get();
			m_apMCardQuest[i]->Activate(pQuestInfo);
		}
	}
	else
	{
		for (INT32 i = 0; i < MAX_QUEST_PER_CARD_NEW ; ++i)
		{
			m_apMCardQuest[i]->Deactivate();
		}
	}

	for (INT32 i = 0; i < MAX_QUEST_PER_CARD_NEW; ++i)
	{
		bool bActive = m_apMCardQuest[i]->IsActive();
		MCard::SetQuestCompletion_OffBattle(i, !bActive);		// ��Ȱ��ȭ�� �Ϸ�ó���Ѵ�..
	}
}


void	MCard::Deactivate()
{
	MCard::Activate(nullptr);
}

void	MCard::CancelCompletion()
{
	for (INT i = 0 ; i < MAX_QUEST_PER_CARD_NEW ; ++i)
	{
		m_abyQuestComplete_OnBattle[i] = m_abyQuestComplete_OffBattle[i];		// ��Ʋ ���� ���·� ����..

		MCardQuest* pQuest = m_apMCardQuest[i];
		if ( pQuest->IsActive() )
			pQuest->CancelCompletion();
	}

}

void	MCard::ConfirmCompletion()
{
	for (INT i = 0 ; i < MAX_QUEST_PER_CARD_NEW ; ++i)
	{
		m_abyQuestComplete_OffBattle[i] = m_abyQuestComplete_OnBattle[i];
		MCardQuest* pQuest = m_apMCardQuest[i];
		if ( pQuest->IsActive() )
			pQuest->ConfirmCompletion();		// �̿Ϸ��� �ϴ���, ����Ʈ ����Ƚ���� �����Ҽ��� �ִ�..
	}
}

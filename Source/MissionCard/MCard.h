#pragma once

class MCardSet;
class MCardQuest;
struct MCardInfo;

class MCard
{
public:
	MCard(const MCardSet* pSet, INT32 myIndex);
	~MCard();
	bool			IsActive() const { return m_pCardInfo != nullptr; }
	void			Activate(const MCardInfo* pCardInfo);
	void			Deactivate();

	void			SetQuestCompletion_OffBattle(INT32 questIdx, bool bComplete);
	void			SetQuestCompletion_OnBattle(INT32 questIdx, bool bComplete);

	MCardQuest*		GetMCardQuest(INT32 idx) const { return m_apMCardQuest[idx]; }

	const MCardInfo*	GetMCardInfo() const { return m_pCardInfo; }

	bool			IsAllQuestCompleted_OffBattle() const;
	bool			IsQuestCompleted_OffBattle(INT32 questIdx) const;	// ��Ȱ��ȭ�� ���� �Ϸ�� ����..
	bool			IsAllQuestCompleted_OnBattle() const;
	bool			IsQuestCompleted_OnBattle(INT32 questIdx) const;	// ��Ȱ��ȭ�� ���� �Ϸ�� ����..

	bool			IsDifferentComplete_OnOffBattle() const;

	bool			IsBattleAvailableCard() const;		// ��� ���ɼ� ���θ� ����(��κ��������. Ʃ�͸����� ���밡��)
	bool			IsSelectEnableCard() const;			// UI�󿡼� ���� ���������� ����..(��밡�ɰ� ������ �ٸ�)

	void			CancelCompletion();
	void			ConfirmCompletion();
	INT32			GetMyIndex() const { return m_myIndex; }

private:
	const MCardSet*				m_pParent;
	INT32						m_myIndex;
	UINT8						m_abyQuestComplete_OffBattle[MAX_QUEST_PER_CARD_NEW];
	UINT8						m_abyQuestComplete_OnBattle[MAX_QUEST_PER_CARD_NEW];
	const MCardInfo*			m_pCardInfo;
	MCardQuest*					m_apMCardQuest[MAX_QUEST_PER_CARD_NEW];
};

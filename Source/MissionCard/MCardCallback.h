#pragma once

struct MCardInfo;


class MCardCallback
{
public:
	// �Ʒ��� ��Ʋ�� �̷���� ������ ..�������� ��Ʋ���ᰡ �ƴ϶�� ����� �Ϸ�� ���� �ƴϴ�..
	virtual void OnCurrQuestCompletionCountChanged_OnBattle(INT32 questIdx, bool bCompleteCard, bool bCompleteQuest) = 0;
	virtual void OnCurrQuestCompleted_OnBattle(INT32 questIdx, bool bCompleteCard) = 0;
	virtual void OnCurrMissionCardCompleted_OnBattle(INT32 lastQuestIdx) = 0;

	virtual void OnCurrMissionCardCompleted_AfterBattle(INT32 completeCardIndex, const MCardInfo* card ) = 0;
	virtual void OnNextMissionCardAutoSelecting_AfterBattle(INT32 prevCardIdx, INT32 currCardIdx) = 0;

	virtual void OnUpdateAllHUDOnBattle() = 0;

	virtual ~MCardCallback() {}
};

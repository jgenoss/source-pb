#if !defined( _CQUEST_MANAGER_H__)
#define _CQUEST_MANAGER_H__

#include "../../CommonSource/QuestDef.h"
#include "QuestBase.h"

#include "CardInfo.h"
#include "CardSet.h"

class CCardSet;
class CCardInfo;

typedef struct _tagQuestInfo
{
	UINT8			nidx;		// �� 4���� Row���� ����Ʈ �ε���
	UINT16			ctxidx;		// QuestCtx���� �ε���
	bool			isComplete;	// ���� �Ϸ����� ����
	CQuestBase	*	pQuest;		// ����Ʈ
}QUESTINF;


class CQuestManager : public i3ElementBase
{
	I3_CLASS_DEFINE( CQuestManager, i3ElementBase);

protected:
	i3::vector<CCardSet*>				m_CardSet;	// CCardSet

	//���� �������� ����� ���� ���� �Դϴ�. 
	QUESTINF			m_QuestData[MAX_QUEST_COL_PER_CARD];	//�������� ����Ʈ������ ����	
	CCardSet	*		m_pCurrentCardSet;						//���� �������� ī���

	bool				m_bNeedUpdate;

	//ȭ�鿡 ���� ���õ� �κ��� �����ֱ� ���� ���� 
	BOOL				m_bGainedPrizeByBattle;			// ���� �Ϸ� �� �����˾��� ���� ���̺�
	INT32				m_gainedPrizeByBattle;			// ���� �Ϸ� �� 
	INT32				m_GainedTotalExp;				// ���� �Ϸ� �� ��� ����Ʈ
	INT32				m_GainedTotalPoint;				// ���� �Ϸ� �� ��� ����Ʈ 
	INT32				m_CompletedCardIndex;

	BOOL				m_bCompleteQuestByBattle;		// ���� �Ϸ� �� ����Ʈ �ϷῩ��

private:
	// Active CardSet INDEX, ���� active ������ CARD_SET�� index - ����� Maximum 4��
	UINT8				m_ActiveIndex;

public:
	void					setQuestContext( const QUEST_CTX & ctx, INT32 CardSetIndex, INT32 CardIndex, INT32 QuestIndex );
	void					setQuestComplete( INT32 CardSetIndex, INT32 CardIndex, INT32 QuestIndex, UINT8 Value );

	void					setQuestItemInfo( INT32 CardSetIndex, INT32 CardIndex, const QUEST_ITEM_INFO_NEW & ItemInfo );

public:
	CQuestManager(void);
	virtual ~CQuestManager(void);

	bool				BuildQuestData(QUESTING_INFO * pQuestAllInfo);
	bool				Reset();


protected:
	bool				_SetActiveCard(void);			//ī����� ����Ǹ� ȣ���Ͽ� ī����� �������ִ� ����� �� �Ŵ����� �����Ѵ�.
	bool				_SetActiveQuest(void);			//ī�尡 ����Ǹ� ȣ���Ͽ� ī�尡 ������ �ִ� ����� �� �Ŵ����� �����Ѵ�.
	bool				_SetActiveCardSet(void);		//���� ī����� ������������ ���� �Ŵ����� ����

	bool				_RestoreQuest(void);			//����� ����Ʈ�� ������ �ִ°����� �ǵ���������( �������� ���������� ��� ������� ���� ���·� ��������)
	
	void				_CheckGainedPrizeByBattle(void);		// ������ �����˾��� ���̺� �� ���

	INT32				_GetNextAvailableCardIndex(void);

public:
	INT32				_SetNextAvailableCardSetIndex(void);
	UINT8				getActiveCardSetIndex( void);
	UINT8				getActiveCardIndex( void);

	bool				ChangeActiveCardSet( UINT32 idx);			//active card set�� ����
	bool				ChangeActiveCard( UINT32 idx);				//active card�� ����
	bool				IsAvailableCard( INT32 idx);
	bool				CanSelectCard( INT32 idx);

public:
	bool				HasQuest( UINT32 Index );

	//get active
	bool				IsActivedCardset()				{ return m_pCurrentCardSet != NULL; }
	CCardInfo	*		getActiveCard( void);
	CQuestBase	*		getActiveQuest( UINT32 nRow)	{ I3_BOUNDCHK( nRow, MAX_QUEST_PER_CARD_NEW); return m_QuestData[nRow].pQuest;}

	//find card
	CCardInfo	*		FindCard( UINT32 idxCard, UINT32 idxCardSet);
	CQuestBase	*		FindQuest( UINT32 idxQuest, UINT32 idxCard, UINT32 idxCardSet);

	BOOL				isCompleteQuest(UINT32 iIdx);
	BOOL 				isNowCompleteQuest(UINT32 iIdx);
	bool				isQuestActivated(void);

	void				GetQuestQueryDesc(char* pszBuff,INT32 idxQuest,INT32 maxLen);
	bool				GetQuestQueryHUDDesc(char* pszBuff,INT32 idxQuest,INT32 maxLen);
	bool				IsCompleteActiveCardSet(void);
	bool				IsCompleteActiveCard(void);
	bool				IsCompletedCard(INT32 CardIndex);
	BOOL				BePrize();

	//
	bool				NoticeChangeActiveIndex();	//���� active index���� ���¸� ������ ����
	void				ResetGainedPrizeByBattle(void);			// ������ �����˾��� ���̺� �ʱ�ȭ
	BOOL				BeGainedPrizeByBattle(void);			// ������ �����˾��� ���̺� �������� �ִ��� Ȯ��
	INT32				GetGainedPrizeByBattle(void);
	BOOL				BeCompleteQuestByBattle(void);			// ���� �� �Ϸ�� ����Ʈ�� �����ϴ°�

	INT32				getGainedTotalExp(void)				{	return m_GainedTotalExp;}
	INT32				getGainedTotalPoint(void)			{	return m_GainedTotalPoint;}
	INT32				getCompletedCardIndex(void)			{ return m_CompletedCardIndex;}

public:
	void				OnUpdate( RT_REAL32 rDeltaSeconds);
	bool				OnGiveUpBattle(void);
	bool				OnStageStart();		//(STAGE_INFO * pStage);
	bool				OnRoundStart();     //(STAGE_INFO * pStage);
	bool				OnStageEnd();		// (STAGE_INFO * pStageInfo);
	
public:
	bool				AddQuestFinishToHud();
	bool				UpdateQuestStateBoard();
	BOOL				CheckCompleteCard(void);

	QUESTINF	*		getQuestData(INT32 idx)	{	I3_BOUNDCHK( idx, MAX_QUEST_PER_CARD_NEW); return &m_QuestData[idx];}
	bool				RemoveActiveCardset();

	bool				HasCardSet(CARDSET_ID CardsetID);
	bool				HasCard(INT32 Index);
	bool				HasEventCardset();
	INT32				GetCardsetIndex(CARDSET_ID CardsetID);
	bool				SetFirstCardsetActive();
	CARDSET_ID			GetActiveCardsetID();

	bool				GetCardReward(UINT8 & Prize, UINT8 & Point, UINT8 & Exp);

public:
	//2011�� 3�� 18�� ������
	//�̼� ī�� �̸� ���⿡ �ʿ��� �޼ҵ�
	CCardSet*		_getCardSet( UINT32 idx);	
	CCardSet*		_getActiveCardSet()	{	return m_pCurrentCardSet;	}

private:
	CCardSet *		getCardSet( UINT32 idx);	
	CCardSet *		AcquireCardSet( INT32 CardSetIndex );	// ������ ����� ������ ��ȯ
	CCardInfo *		getCard( UINT32	idx);
	CQuestBase *	getQuest( UINT32 idxQuest);

	UINT32			getActiveQuestIndex( UINT32 nIdx);
	CCardSet *		getActiveCardSet( void)			{ return m_pCurrentCardSet;}

	bool			RemoveQuestCard( UINT32 CardSetIdx );


	void			InitData();

	// �ӹ�ī�� ������ ������ ���� �Լ�
public:
	bool			Process_PROTOCOL_BASE_QUEST_CHANGE_ACK(UINT8 ui8FinishFlagAndProcessedQuestIdx, UINT8 ui8ProcessedTaskCount);
	void			Process_PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK();
	void			SaveQuest();

public:
	const char * GetTemplateName(ENUM_QUEST_CARDSET_TYPE CardType);
	INT32 GetTemplateIndex(ENUM_QUEST_CARDSET_TYPE CardType);
};



INT32 CalculateQuestCardIndexInArray( INT32 CardSetIndex, INT32 CardIndex, INT32 QuestIndex );

#endif

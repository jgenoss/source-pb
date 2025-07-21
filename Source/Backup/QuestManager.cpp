#include "pch.h"
#include "QuestManager.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"

I3_CLASS_INSTANCE( CQuestManager);//, i3ElementBase);

void _set_template_name();

CQuestManager::CQuestManager(void)
{
	m_CardSet.resize(MAX_CARDSET_PER_USER);

	i3mem::FillZero( m_QuestData, sizeof(m_QuestData) );

	InitData();
	
	// InitData()���� �ʱ�ȭ�ϸ� �ȵ�. �������� ���� �޾ƿͼ� �ʱ�ȭ�Ҷ� ���� ����
	m_ActiveIndex = 0;

	_set_template_name();
}

CQuestManager::~CQuestManager(void)
{
	i3::cu::for_each_safe_release(m_CardSet, &CCardSet::Release); 
}

bool CQuestManager::BuildQuestData(QUESTING_INFO * pQuestAllInfo)
{
	//Setting
	m_ActiveIndex	= pQuestAllInfo->m_ActiveIndexUse;	//Set Active

	for( INT32 CardSetIndex = 0; CardSetIndex < MAX_CARDSET_PER_USER; CardSetIndex++)	//4�� ����
	{
		if( pQuestAllInfo->m_pCardSetType[CardSetIndex] == QUEST_CARDSET_TYPE_NONE)
		{
			RemoveQuestCard(CardSetIndex);
			continue;
		}


		CCardSet * pCardSet =  AcquireCardSet(CardSetIndex);

		pCardSet->SetActiveCardIndex(pQuestAllInfo->m_pActiveIndexCardSet[CardSetIndex]);
		pCardSet->SetID(pQuestAllInfo->m_pCardSetType[CardSetIndex]);


		for( INT32 CardIndex = 0; CardIndex < MAX_CARD_PER_CARDSET; CardIndex++)		//ī��� 10��
		{
			//ī��� ����
			CCardInfo * pCard = pCardSet->getCard(CardIndex);
			pCard->SetActiveIndex( pQuestAllInfo->m_pActiveIndexCard[CardSetIndex][CardIndex] );
		}
	}

	return Reset();
}

//---------------------------------------------------------------------------------------
// create quests
//---------------------------------------------------------------------------------------
bool CQuestManager::Reset()
{
	InitData();

	for(INT32 i = 0; i < MAX_CARDSET_PER_USER; ++i)	// 4
	{
		CCardSet * pCardSet = getCardSet(i);
		if( pCardSet && pCardSet->Reset( i ) == false)
		{
			I3FATAL( "Could not Reset CardSet at %d\n", i);
			return false;
		}
	}

	_SetActiveCardSet();

	return true;
}

//---------------------------------------------------------------------------------------
// function
//---------------------------------------------------------------------------------------
// �Ŵ������� ������ active quest���� ����
bool CQuestManager::_SetActiveCardSet(void)
{
	m_pCurrentCardSet = getCardSet(m_ActiveIndex);
	if( m_pCurrentCardSet == NULL)
	{
		for( INT32 i = 0; i < MAX_CARDSET_PER_USER; ++i)
		{
			if( getCardSet(i) != NULL)
			{
				m_pCurrentCardSet			= getCardSet(i);
				m_ActiveIndex	= (UINT8)i;

				i = MAX_CARDSET_PER_USER;
			}
		}
	}

	return _SetActiveCard();
}

bool CQuestManager::_SetActiveCard(void)
{
	if( m_pCurrentCardSet == NULL)
		return false;

	return _SetActiveQuest();
}

//���Լ��� �������� �� �ο쿡 �ִ°��� �ϳ��� ���� ���ִ� �Լ� ���µ� ������
//�ϳ����� �����Ƿ� �Ϸ��ߴ�. �ͷ���ߴٸ� �������ָ� �˴ϴ�.
bool CQuestManager::_SetActiveQuest(void)
{
	CCardInfo * pCard = getActiveCard();

	if(pCard == NULL)
	{
		return false;
	}

	for( INT32 i = 0; i < MAX_QUEST_PER_CARD_NEW; ++i)
	{
		if( pCard->getActiveQuestIndex( i ) == 0xF)
		{
			m_QuestData[i].nidx			= (UINT8)0xF;
			m_QuestData[i].ctxidx		= (UINT16)0xFF;
			m_QuestData[i].isComplete	= false;
			m_QuestData[i].pQuest		= NULL;
		}
		else
		{
			m_QuestData[i].nidx			= (UINT8)pCard->getActiveQuestIndex( i);
			m_QuestData[i].ctxidx		= (UINT16)CalculateQuestCardIndexInArray(m_pCurrentCardSet->GetIndx(), pCard->GetIndx(), i);;
			m_QuestData[i].pQuest		= pCard->getQuest( i);

			if( m_QuestData[i].pQuest != NULL)
			{
				m_QuestData[i].isComplete	= (m_QuestData[i].pQuest->OnQueryState() == QST_COMPLETED)? true : false;
			}
			else
			{
				m_QuestData[i].nidx			= (UINT8)0xF;
				m_QuestData[i].ctxidx		= (UINT16)0xFF;
				m_QuestData[i].isComplete	= false;
				m_QuestData[i].pQuest		= NULL;
			}
		}
	}

	return true;
}

//�������� ī��� ī�� ����Ʈ�� ����
bool CQuestManager::ChangeActiveCardSet( UINT32 idx)
{
	if( getCardSet(idx) == NULL)
		return false;

	if( m_ActiveIndex == idx)
		return true;

	I3ASSERT( idx < MAX_CARDSET_PER_USER);

	m_ActiveIndex = (UINT8)idx;

	m_pCurrentCardSet = getCardSet(idx);

	_SetActiveCard();

	return true;
}

bool CQuestManager::ChangeActiveCard( UINT32 idx)
{
	if( m_pCurrentCardSet == NULL)
		return false;

	m_pCurrentCardSet->ChangeActiveCard( idx);

	_SetActiveCard();

	return true;
}

CCardSet * CQuestManager::_getCardSet( UINT32 idx )
{
	return getCardSet( idx );
}

CCardSet * CQuestManager::getCardSet( UINT32 idx )
{
//	I3_BOUNDCHK( idx, MAX_CARDSET_PER_USER);

	return NULL;
	return m_CardSet[idx];
}

CCardInfo	* CQuestManager::getCard( UINT32 idx )
{
	//I3_BOUNDCHK( idx, MAX_CARD_PER_CARDSET);
	if( idx < MAX_CARD_PER_CARDSET)
		return m_pCurrentCardSet->getCard( idx);
	else
		return NULL;
}

CQuestBase	* CQuestManager::getQuest( UINT32 idxQuest )
{
	I3ASSERT( idxQuest < MAX_QUEST_PER_CARD_NEW);
	CCardInfo * pCard = getActiveCard();
	if ( pCard )
		return pCard->getQuest( idxQuest);
	return NULL;
}


bool CQuestManager::HasQuest( UINT32 Index )
{
	return getQuest(Index) != NULL;
}

CCardInfo	* CQuestManager::getActiveCard( void )
{
	if ( m_pCurrentCardSet)
		return m_pCurrentCardSet->getActiveCard();
	return NULL;
}


bool CQuestManager::isQuestActivated( void )
{
	return getActiveCard() != NULL;
}


//find function
CCardInfo	* CQuestManager::FindCard( UINT32 idxCard, UINT32 idxCardSet)
{
	I3ASSERT( idxCardSet < MAX_CARDSET_PER_USER);
	I3ASSERT( idxCard < MAX_CARD_PER_CARDSET);

	if( getCardSet(idxCardSet) == NULL)
		return NULL;

	return getCardSet(idxCardSet)->getCard( idxCard);
}

CQuestBase	* CQuestManager::FindQuest( UINT32 idxQuest, UINT32 idxCard, UINT32 idxCardSet)
{
	CCardInfo * pCard = FindCard( idxCard, idxCardSet);

	if( pCard == NULL)
		return NULL;

	return pCard->getQuest( idxQuest);
}


void CQuestManager::GetQuestQueryDesc(char* pszBuff,INT32 idxQuest,INT32 maxLen)
{
	CQuestBase* quest = getQuest(idxQuest);
	if( quest == NULL)
	{
		I3WARN("�������� �ʴ� ����Ʈ�� ���õǾ����ϴ�.\n");
		return;
	}

	quest->OnQueryDesc(pszBuff, maxLen);
}


bool CQuestManager::GetQuestQueryHUDDesc( char* pszBuff,INT32 idxQuest,INT32 maxLen )
{
	CQuestBase* quest = getQuest(idxQuest);
	if( quest == NULL)
	{
		I3WARN("�������� �ʴ� ����Ʈ�� ���õǾ����ϴ�.\n");
		return false;
	}

	quest->OnQueryHUDDesc(pszBuff, maxLen);
	return true;
}


bool CQuestManager::IsCompleteActiveCardSet( void )
{
	CCardSet * pCardset = getActiveCardSet();
	if ( ! pCardset )
		return false;

	return pCardset->IsComplate();
}


bool CQuestManager::IsCompleteActiveCard(void)
{
	if(getActiveCard() == NULL)
	{
		return false;
	}
	else
	{
		return getActiveCard()->IsCompletedCard();
	}
}


bool CQuestManager::IsCompletedCard( INT32 CardIndex )
{
	CCardInfo* pInfo = NULL;
	pInfo = getCard(CardIndex);
	if (pInfo == NULL) return false;

	return pInfo->IsCompletedCard();
}


BOOL CQuestManager::BePrize()
{
	// ���� Ȱ��ȭ�� ī�尡 �־���Ѵ�.
	CCardInfo * pCard = getActiveCard();
	if (pCard)
	{
		UINT32 prize = pCard->GetPrize();
		UINT32 point = pCard->GetPoint();
		UINT32 exp	= pCard->GetExp();

		// ǥ��, ���ӸӴ�, ����ġ �� �ϳ��� ������ ������ �����Ѵ�.
		if (prize || point || exp)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//����� ����Ʈ�� ������ �ִ°����� �ǵ���������( �������� ���������� ��� ������� ���� ���·� ��������)
bool CQuestManager::_RestoreQuest(void)
{
	CCardInfo * pCard = getActiveCard();
	if ( pCard == NULL)
		return false;
	pCard->RestoreQuest();
	return true;
}

void CQuestManager::_CheckGainedPrizeByBattle(void)
{
	//�Ϸ� �Ŀ��� ���ð� ��������.
	// ���� �� �Ϸ�� �ӹ��� �ִ��� Ȯ���Ѵ�.
	BOOL AllFinish	 = TRUE;

	//�� �Լ��� ȣ���ϴ� ������ �߿��Ұ� ���Ƽ� �̷��� ó�� �ϵ��� ������
	m_bCompleteQuestByBattle  = FALSE;
	INT32 nCompletedQuestCount = 0;
	for(UINT32 i = 0; i < MAX_QUEST_PER_CARD_NEW; i++)
	{
		if( isNowCompleteQuest(i) )
		{
			m_bCompleteQuestByBattle = TRUE;
		}
		else
		{
			if( getActiveQuestIndex(i) != 0x0F)
			{
				AllFinish = FALSE;
			}
			else
			{
				// �̹� �Ϸ�� ����Ʈ ī��Ʈ
				nCompletedQuestCount++;
			}
		}
	}

	// �̹� �Ϸ�� �ӹ�ī�尡 ������ ��쿡�� �������� �˾��� ����� �ʽ��ϴ�.
	if( nCompletedQuestCount == MAX_QUEST_PER_CARD_NEW) AllFinish = FALSE;

	//ī�带 �ϳ� �̻��� �Ϸ��ϰ�
	if(AllFinish)
	{
		//ī�� ����
		CCardInfo * pCard = getActiveCard();
		m_bGainedPrizeByBattle  = TRUE;
		if(pCard != NULL)
		{
			m_gainedPrizeByBattle	= pCard->GetPrize();
			m_GainedTotalExp		= pCard->GetExp();
			m_GainedTotalPoint		= pCard->GetPoint();
		}
		
		if(m_pCurrentCardSet != NULL)
		{
			m_CompletedCardIndex	= static_cast<INT32>(m_pCurrentCardSet->getActiveCardIndex());
		}
	}

	return;
}



bool CQuestManager::Process_PROTOCOL_BASE_QUEST_CHANGE_ACK(UINT8 ui8FinishFlagAndProcessedQuestIdx, UINT8 ui8ProcessedTaskCount)
{
	UINT8 u8Temp = ui8FinishFlagAndProcessedQuestIdx & 0xf0;
	UINT8 u8Idx = ui8FinishFlagAndProcessedQuestIdx & 0x0f;

	GlobalFunc::PB_TRACE("FinishFlag %x QuestIdx %d TaskCount %d", u8Temp, u8Idx, ui8ProcessedTaskCount);

	if(u8Idx >= MAX_QUEST_COL_PER_CARD)
	{
		I3ASSERT_0;
		return false;
	}
	
	if( m_QuestData[u8Idx].nidx != 0xF && m_QuestData[u8Idx].pQuest != NULL)
	{
		if( m_QuestData[u8Idx].isComplete == false)
		{
			// 5�� ����� ������ �ӹ��� ī��Ʈ�� �����Ѵ�.
			m_QuestData[u8Idx].pQuest->setArg( 5, ui8ProcessedTaskCount);
			if(u8Temp == 0xf0)
			{
				m_QuestData[u8Idx].isComplete = true;
				AddQuestFinishToHud();//���� ���� �Ϸ��� ȭ���߾ӿ� ����ִ� �޽��� ���
			}
		}
		else
		{
			m_QuestData[u8Idx].pQuest->setArg( 5, ui8ProcessedTaskCount);
		}
	}

	m_bNeedUpdate = true;

	return true;
}

bool CQuestManager::OnStageEnd()// STAGE_INFO * pStageInfo)
{

	Process_PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK();

	return true;
}

void CQuestManager::SaveQuest()
{
	for( INT32 i = 0;i  < MAX_QUEST_PER_CARD_NEW; ++i)
	{
		if( m_QuestData[i].nidx != 0xF && m_QuestData[i].pQuest != NULL)
		{
			if(getActiveCard() != NULL)
			{
				getActiveCard()->SaveQuest( i);
			}
		}
	}
}

void CQuestManager::Process_PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK()
{

	INT32 nCompleteCnt = 0;

	for( INT32 i = 0;i  < MAX_QUEST_PER_CARD_NEW; ++i)
	{
		if( m_QuestData[i].nidx != 0xF && m_QuestData[i].pQuest != NULL)
		{
			if( m_QuestData[i].isComplete == true)
			{
				if(getActiveCard() != NULL)
				{
					nCompleteCnt++;
					getActiveCard()->CompleteQuest( i);
				}
			}
		}
	}

	// ������ �����˾��� ���̺� �� ���
	_CheckGainedPrizeByBattle();

	if( nCompleteCnt > 0)//�ϳ��� ī�忡�� �̵��ϴ°��� üũ
	{
		NoticeChangeActiveIndex();
	}

	if( CheckCompleteCard() )
	{
		NoticeChangeActiveIndex();
	}

}

BOOL CQuestManager::CheckCompleteCard(void)
{
	if(getActiveCard() != NULL)
	{
		if( getActiveCard()->IsCompletedCard())
		{
			//Play�Ҽ� �ִ� ī�� �ε����� �޾Ƽ�
			INT32 next = _GetNextAvailableCardIndex();
			if( next != -1)
			{
				ChangeActiveCard( next);
				return TRUE;
			}
		}
	}
	return FALSE;
}

bool CQuestManager::NoticeChangeActiveIndex(void)
{
	// Ȱ��ȭ ī����� ���� ��� �뺸�� �ʿ� ����.
	if (NULL == m_pCurrentCardSet)
	{
		return false;
	}

	//������ ����� active index���� ����
	QUEST_ACTIVE_INDEX aidx;

	aidx._idxUse	= getActiveCardSetIndex();
	aidx._idxCard	= getActiveCardIndex();

	if( aidx._idxCard == 0xFF)
	{
		aidx._idxQuest	= (UINT16)0xFFFF;
	}
	else
	{
		if(getActiveCard() != NULL)
		{
			aidx._idxQuest	= (UINT16)getActiveCard()->getActiveQuestIndex();
		}
		else
		{	
			aidx._idxQuest = (UINT16)0xFFFF;
		}
		
	}

	//����
	if( g_pGameContext->SetEvent( EVENT_QUEST_CHANGE_ACTIVE_IDX, &aidx) == FALSE)
	{
		I3TRACE( "fail NoticeChangeActiveIndex\n");
		return false;
	}

	_SetActiveCardSet();

	return true;
}

bool	CQuestManager::IsAvailableCard( INT32 idx)
{
	CCardSet* cardSet = getActiveCardSet();
	CCardInfo* card = getCard( idx);

	if (!cardSet)
		return false;
	if (QUEST_CARDSET_TYPE_TUTORIAL != cardSet->GetID())// �Ϲ� ī���� ���
	{
		if (!card || card->IsCompletedCard())
			return false;
		return true;
	}
	else												// Ʃ�丮�� ī���� ���
	{
		if (0 == idx) // ù��° ī��� �׻� ���� ������.
			return true;
		card = getCard(idx - 1); // �׿� ī��� �ٷ� ����ī�尡 �Ϸ�Ǿ�� ������ �� �ִ�.
		if (card && card->IsCompletedCard())
			return true;
		return false;
	}


	//// ī�带 ������ �־���Ѵ�.
	//CCardSet* cardSet = getActiveCardSet();
	//if (!cardSet)
	//	return false;

	//CCardInfo* card = getCard( idx);
	//if (!card || card->IsCompletedCard())
	//	return false;

	//// ī����� Ȯ���Ѵ�.
	//if (QUEST_CARDSET_TYPE_TUTORIAL != cardSet->GetID())
	//	return true;

	//// Ʃ�丮�� ī����� ����ó���Ѵ�.
	//// ù��°ī��� �������� ������ �� �ִ�.
	//if (0 == idx)
	//	return true;


	//// �׿� ī��� �ٷ� ����ī�尡 �Ϸ�Ǿ�� ������ �� �ִ�.
	//card = getCard(idx - 1);

	//if (card && card->IsCompletedCard())
	//	return true;

	//return false;
}

bool	CQuestManager::CanSelectCard( INT32 idx)
{
	// ī�带 ������ �־���Ѵ�.
	CCardSet* cardSet = getActiveCardSet();
	if (!cardSet)
		return false;

	CCardInfo* card = getCard( idx);
	if (!card )
		return false;

	// Ʃ�丮�� �ӹ�ī�常 ������� ó���ؾ���.
	if (QUEST_CARDSET_TYPE_TUTORIAL != cardSet->GetID())
		return true;

	if (0 == idx)
		return true;

	// �׿� ī��� �ٷ� ����ī�尡 �Ϸ�Ǿ�� ������ �� �ִ�.
	card = getCard(idx - 1);
	if (card && card->IsCompletedCard())
		return true;

	return false;
}

INT32	CQuestManager::_SetNextAvailableCardSetIndex(void)
{
	for( INT32 i = 0;i < MAX_CARDSET_PER_USER; ++i)
	{
		CCardSet * pCardSet = getCardSet( i);
		if( pCardSet != NULL)
		{
			ChangeActiveCardSet( i);

			CCardInfo * pCardInfo = getActiveCard();
			if( pCardInfo != NULL && !pCardInfo->IsCompletedCard())
			{
				ChangeActiveCard( getActiveCardIndex());
				return i;
			}
			else
			{
				INT32 cardidx = _GetNextAvailableCardIndex();
				if( cardidx != -1)
				{
					ChangeActiveCard( cardidx);
					return i;
				}
			}
		}
	}

	return -1;
}

INT32	CQuestManager::_GetNextAvailableCardIndex()
{
	INT32 nNextCardIdx = getActiveCardIndex() + 1;
	if( nNextCardIdx >= MAX_CARD_PER_CARDSET)
	{
		nNextCardIdx = 0;
	}

	while( nNextCardIdx != getActiveCardIndex())
	{
		if( IsAvailableCard( nNextCardIdx))
		{
			return nNextCardIdx;
		}

		nNextCardIdx++;

		if( nNextCardIdx >= MAX_CARD_PER_CARDSET)
		{
			nNextCardIdx = 0;
		}
	}

	return -1;
}



void CQuestManager::OnUpdate( RT_REAL32 rDeltaSeconds)
{
	if( m_bNeedUpdate && CHud::i() != NULL)
	{
		UpdateQuestStateBoard();
		m_bNeedUpdate = false;
	}
}

bool CQuestManager::OnGiveUpBattle(void)
{
	_RestoreQuest();

	_SetActiveQuest();

	return true;
}

bool CQuestManager::UpdateQuestStateBoard()
{
	if( g_pFramework->IsBattleStage() == false)
		return false;

	CHud * pHUD = CHud::i();
	if( pHUD == NULL)
	{
		I3WARN( "�߸��� ����Ʈ Ŭ����޽��� ȣ��, hud�� null\n");
		return false;
	}

	pHUD->ChangeChallenge();

	return true;
}

bool CQuestManager::OnStageStart()// (STAGE_INFO * pStage)
{
	_SetActiveCardSet();

	m_bNeedUpdate = true;

	ResetGainedPrizeByBattle();

	return true;
}

bool CQuestManager::OnRoundStart() //(STAGE_INFO * pStage)
{
	m_bNeedUpdate = true;

	return true;
}

bool CQuestManager::AddQuestFinishToHud(void)
{
	if( g_pFramework->IsBattleStage() == false)
	{
		I3WARN( "�߸��� ����Ʈ Ŭ����޽��� ȣ��, ���������� ���ų� ��Ʋ���������� �ƴ϶��...\n");
		return false;
	}

	CHud * pHUD = CHud::i();
	if( pHUD == NULL)
	{
		I3WARN( "�߸��� ����Ʈ Ŭ����޽��� ȣ��, hud�� null�̶�׿�...��ó���� ����\n");
		return false;
	}

	//������ �Ϸ�� �̺�Ʈ�� ������.
	g_pGameContext->SetEvent(EVENT_N_QUEST_END_NOTICE);
	pHUD->AddChallengeClear();

	return true;
}

void CQuestManager::ResetGainedPrizeByBattle(void)
{
	// ������ �����˾� Ȯ�ο� ���̺� �ʱ�ȭ
	m_bGainedPrizeByBattle = FALSE;
	m_gainedPrizeByBattle = 0;

	m_GainedTotalExp = 0;
	m_GainedTotalPoint = 0;
	m_CompletedCardIndex = 0;

	m_bCompleteQuestByBattle = FALSE;
}

BOOL CQuestManager::BeGainedPrizeByBattle(void)
{
	// ���� �� �Ϸ�� ����Ʈ�� �ִ°�
	return m_bGainedPrizeByBattle;
}

BOOL CQuestManager::GetGainedPrizeByBattle(void)
{
	return m_gainedPrizeByBattle;
}



BOOL CQuestManager::BeCompleteQuestByBattle(void)
{
	return m_bCompleteQuestByBattle;
}


void CQuestManager::setQuestContext( const QUEST_CTX & ctx, INT32 CardSetIndex, INT32 CardIndex, INT32 QuestIndex )
{
	CCardSet * pCardSet = AcquireCardSet(CardSetIndex);
	if ( pCardSet )
	{
		CCardInfo * pCard = pCardSet->getCard(CardIndex);
		if (pCard)
		{
			pCard->SetQuestData(QuestIndex, ctx);
		}
	}
}


void CQuestManager::setQuestComplete( INT32 CardSetIndex, INT32 CardIndex, INT32 QuestIndex, UINT8 Value )
{
	CCardSet * pCardSet = getCardSet(CardSetIndex);
	if ( pCardSet )
	{
		CCardInfo * pCard = pCardSet->getCard(CardIndex);
		if (pCard)
		{
			pCard->SetQuestCompalte(QuestIndex, Value);
		}
	}
}


void CQuestManager::setQuestItemInfo( INT32 CardSetIndex, INT32 CardIndex, const QUEST_ITEM_INFO_NEW & ItemInfo )
{
	CCardSet * pCardSet = AcquireCardSet(CardSetIndex);
	if ( pCardSet )
	{
		pCardSet->SetCardQuestItemInfo(CardIndex, ItemInfo);
	}
}


void CQuestManager::InitData()
{
	m_pCurrentCardSet	= NULL;


	m_bNeedUpdate = true;

	m_bGainedPrizeByBattle = FALSE;
	m_gainedPrizeByBattle  = 0;

	m_GainedTotalExp = 0;
	m_GainedTotalPoint = 0;
	m_CompletedCardIndex = 0;

	m_bCompleteQuestByBattle = FALSE;
}

bool CQuestManager::RemoveQuestCard( UINT32 CardSetIdx )
{
	CCardSet * pCardSet = getCardSet(CardSetIdx);
	if ( ! pCardSet )
		return false;
	pCardSet->Release();
	m_CardSet[CardSetIdx] = NULL;
	
	if(CardSetIdx == m_ActiveIndex)
	{
		m_pCurrentCardSet = NULL;
	}

	return true;
}


bool CQuestManager::RemoveActiveCardset()
{
	CCardSet * pCardSet = getCardSet( getActiveCardSetIndex() );
	if ( ! pCardSet )
		return false;

	// �Ϸ� �ӹ�ī�� ���� - �̺�Ʈ ī��� �Ⱓ�� ������ �������� �����Ѵ�.
	if (pCardSet->GetID() == QUEST_CARDSET_TYPE_EVENT)
		return true;

	return RemoveQuestCard( getActiveCardSetIndex() );
}


CCardSet * CQuestManager::AcquireCardSet( INT32 CardSetIndex )
{
	CCardSet * pCardSet = getCardSet(CardSetIndex);
	if ( pCardSet )
		return pCardSet;

	pCardSet = CCardSet::new_object();
	m_CardSet[CardSetIndex] = pCardSet;
	return pCardSet;
}

bool CQuestManager::HasCardSet( CARDSET_ID CardsetID )
{
	return GetCardsetIndex(CardsetID) != -1;
}

bool CQuestManager::HasCard( INT32 Index )
{
	return getCard(Index) != NULL;
}


bool CQuestManager::HasEventCardset()
{
	return GetCardsetIndex(QUEST_CARDSET_TYPE_EVENT) != -1;
}

INT32 CQuestManager::GetCardsetIndex( CARDSET_ID CardsetID )
{
	for(INT32 i = 0; i < MAX_CARDSET_PER_USER; i++)
	{
		CCardSet* pCardSet = getCardSet(i);
		if ( pCardSet && pCardSet->GetID() == CardsetID )
			return i;
	}
	return -1;
}

bool CQuestManager::SetFirstCardsetActive()
{
	for( INT32 i = 0; i < MAX_CARDSET_PER_USER; i++)
	{
		CCardSet* pCardSet = getCardSet(i);
		if( pCardSet != NULL)
		{
			ChangeActiveCardSet(i);
			return true;
		}
	}
	return false;
}

CARDSET_ID CQuestManager::GetActiveCardsetID()
{
	CCardSet * pCardset = getActiveCardSet();
	if ( pCardset )
		return pCardset->GetID();
	return QUEST_CARDSET_TYPE_NONE;
}

bool CQuestManager::GetCardReward( UINT8 & Prize, UINT8 & Point, UINT8 & Exp )
{

	return false;
}

UINT8 CQuestManager::getActiveCardSetIndex( void )
{
	return m_ActiveIndex;
}

UINT8 CQuestManager::getActiveCardIndex( void )
{
	if ( m_pCurrentCardSet )
		return m_pCurrentCardSet->getActiveCardIndex();
	return 0;
}

UINT32 CQuestManager::getActiveQuestIndex( UINT32 nIdx )
{
	I3ASSERT( nIdx < MAX_QUEST_PER_CARD_NEW);
	return m_QuestData[nIdx].nidx;
}


BOOL CQuestManager::isCompleteQuest( UINT32 iIdx )
{
	I3ASSERT( iIdx < MAX_QUEST_PER_CARD_NEW);
	return m_QuestData[iIdx].nidx == 0xF;
}

BOOL CQuestManager::isNowCompleteQuest( UINT32 iIdx )
{
	I3ASSERT( iIdx < MAX_QUEST_PER_CARD_NEW);
	return m_QuestData[iIdx].isComplete;
}


INT32 CalculateQuestCardIndexInArray( INT32 CardSetIndex, INT32 CardIndex, INT32 QuestIndex )
{
	INT32 CardIdx = (CardSetIndex * MAX_QUEST_PER_CARDSET_NEW)+(CardIndex * MAX_QUEST_PER_CARD_NEW)+QuestIndex;
	return CardIdx;
}

//�ӹ�ī�忡 ���� �̹��� ���ø� �̸��� �����Ѵ�.
typedef std::pair< std::string, INT32 > CardTypeTemplateAtt;
typedef std::map< ENUM_QUEST_CARDSET_TYPE, CardTypeTemplateAtt > CardTypeTemplateAttList;

static CardTypeTemplateAttList gCardTypeTemplateAttList;


void _add_template_name_List(ENUM_QUEST_CARDSET_TYPE quest_type, const std::string& template_name, INT32 template_index)
{
	CardTypeTemplateAttList::iterator It = 
		gCardTypeTemplateAttList.find( quest_type );

	if( It != gCardTypeTemplateAttList.end() )
	{
		I3FATAL( "[_add_template_name_List] �̹� ��ϵ� CARDSET_TYPE �Դϴ�." );
	}

	CardTypeTemplateAtt att(template_name, template_index);

	gCardTypeTemplateAttList.insert( 
		CardTypeTemplateAttList::value_type( quest_type, att ) );
}

void _set_template_name()
{
	gCardTypeTemplateAttList.clear();

	//�ش� �ӹ�ī�忡 ���� �̹��� ���ø� �̸��� �����ϸ� �ȴ�.
	_add_template_name_List( QUEST_CARDSET_TYPE_NONE, "ButtonImageSet_Item_Card1_hs", 0 );
	_add_template_name_List( QUEST_CARDSET_TYPE_TUTORIAL, "ButtonImageSet_Item_Card1_hs", 0 );

	_add_template_name_List( QUEST_CARDSET_TYPE_GOLD, "ButtonImageSet_Item_Card2_hs", 1 );
	_add_template_name_List( QUEST_CARDSET_TYPE_ASSAULT, "ButtonImageSet_Item_Card3_hs", 2 );
	_add_template_name_List( QUEST_CARDSET_TYPE_BACKUP, "ButtonImageSet_Item_Card4_hs", 3 );
	_add_template_name_List( QUEST_CARDSET_TYPE_INFILTRATION, "ButtonImageSet_Item_Card5_hs", 4 );
	_add_template_name_List( QUEST_CARDSET_TYPE_SPECIAL, "ButtonImageSet_Item_Card6_hs", 5 );
	_add_template_name_List( QUEST_CARDSET_TYPE_DEFCON, "ButtonImageSet_Item_Card7_hs", 6 );
	_add_template_name_List( QUEST_CARDSET_TYPE_COMMISSIONED_O, "ButtonImageSet_Item_Card8_hs", 7 );
	_add_template_name_List( QUEST_CARDSET_TYPE_COMPANY_O, "ButtonImageSet_Item_Card9_hs", 8 );
	_add_template_name_List( QUEST_CARDSET_TYPE_FIELD_O, "ButtonImageSet_Item_Card10_hs", 9 );

	_add_template_name_List( QUEST_CARDSET_TYPE_EVENT, "Event_Mission_Card", 18 );

	_add_template_name_List( QUEST_CARDSET_TYPE_DINO_TUTORIAL, "ButtonImageSet_Item_Card13_hs", 12 );
	_add_template_name_List( QUEST_CARDSET_TYPE_HUMAN_TUTORIAL, "ButtonImageSet_Item_Card14_hs", 13 );
	_add_template_name_List( QUEST_CARDSET_TYPE_DINO_2ND, "ButtonImageSet_Item_Card16_hs", 15 );
	_add_template_name_List( QUEST_CARDSET_TYPE_HUMAN_2ND, "ButtonImageSet_Item_Card15_hs", 14 );
	_add_template_name_List( QUEST_CARDSET_TYPE_DINO_3RD, "ButtonImageSet_Item_Card18_hs", 17 );
	_add_template_name_List( QUEST_CARDSET_TYPE_HUMAN_3RD, "ButtonImageSet_Item_Card17_hs", 16 );
}

const char * CQuestManager::GetTemplateName(ENUM_QUEST_CARDSET_TYPE CardType)
{
	CardTypeTemplateAttList::iterator It = gCardTypeTemplateAttList.find( CardType );

	if( It == gCardTypeTemplateAttList.end() )
	{
		I3FATAL( "[SetTemplateNameByIndex] ��� �� �� CARDSET_TYPE �Դϴ�." );
	}

	return It->second.first.c_str();
}

INT32 CQuestManager::GetTemplateIndex(ENUM_QUEST_CARDSET_TYPE CardType)
{
	CardTypeTemplateAttList::iterator It = gCardTypeTemplateAttList.find( CardType );

	if( It == gCardTypeTemplateAttList.end() )
	{
		I3FATAL( "[SetTemplateNameByIndex] ��� �� �� CARDSET_TYPE �Դϴ�." );
	}

	return It->second.second;
}
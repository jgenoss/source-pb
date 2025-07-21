#include "pch.h"
#include "CardInfo.h"

//////////////////////////////////////////////////////////////////////////
//include for Class Table
//////////////////////////////////////////////////////////////////////////
#include "QuestBase_Map_Count_Accm_Team.h"	//���� class table�� ����Ͽ� createinstance(meta) �������� �ҵ� �մϴ�.
#include "QuestBase_Map_Count_Weapon_Accm.h"	//���� class table�� ����Ͽ� createinstance(meta) �������� �ҵ� �մϴ�.
#include "QuestBase_Map_Count_Weapon_cont.h"	//���� class table�� ����Ͽ� createinstance(meta) �������� �ҵ� �մϴ�.

//////////////////////////////////////////////////////////////////////////
//QUEST_ID �� ��Ī�Ǵ� Ŭ���� ���̺� �Դϴ�.
i3ClassMeta *	GetQuestClassMeta(INT32 QuestID)
{
	static i3ClassMeta * s_pQuestClassTable[USE_QUEST_COUNT] = 
	{
		NULL,
		CQuestBase_Map_Count_Accm_Team::static_meta(),
		CQuestBase_Map_Count_Weapon_Accm::static_meta(),
		CQuestBase_Map_Count_Weapon_Cont::static_meta()
	};

	I3ASSERT(QuestID > 0 && QuestID < USE_QUEST_COUNT);
	if (QuestID > 0 && QuestID < USE_QUEST_COUNT)
	{
		return s_pQuestClassTable[QuestID];
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

enum 
{
	ENC_SEED_QUEST_CTX = 2,
};

I3_CLASS_INSTANCE( CCardInfo);//, i3ElementBase);



CCardInfo::CCardInfo()
{
	m_MyIndex		= -1;
	m_ActiveIndex	= 0;
	i3mem::FillZero( &m_QuestItemInfo, sizeof(m_QuestItemInfo) );
	i3mem::FillZero( m_pQuest, sizeof( CQuestBase *) * MAX_QUEST_PER_CARD_NEW);
	i3mem::FillZero( m_QuestData, sizeof( m_QuestData ) );
}

CCardInfo::~CCardInfo()
{
	for( INT32 i = 0; i < MAX_QUEST_PER_CARD_NEW; ++i)
	{
		I3_SAFE_RELEASE( m_pQuest[i]);
	}

}

bool CCardInfo::Reset( INT32 MyIndex )
{
	m_MyIndex	= MyIndex;


	for( INT32 i = 0; i < MAX_QUEST_PER_CARD_NEW; ++i)
	{
		QUEST_CTX ctx;
		GetQuestData(i, ctx);

		if( ctx.m_ID == 0 )
		{
			I3_SAFE_RELEASE(m_pQuest[i]);
			continue;
		}

		i3ClassMeta * pMeta = GetQuestClassMeta(ctx.m_ID);
		if (pMeta)
		{
			I3_SAFE_RELEASE(m_pQuest[i]);
			m_pQuest[i] = (CQuestBase*)pMeta->create_instance();	// CREATEINSTANCE(pMeta);
			m_pQuest[i]->OnBind( &ctx );
			m_pQuest[i]->AddRef();
		}
		else
		{
			DEV_LOG("Invalid Value [QuestID :%d][%d]\n", ctx.m_ID, i);
		}

	}

	return true;
}


void CCardInfo::RestoreQuest()
{
	QUEST_CTX ctx;
	for( INT32 i = 0; i < MAX_QUEST_PER_CARD_NEW; ++i)
	{
		if (m_pQuest[i] && getActiveQuestIndex(i) != 0xF)
		{
			GetQuestData(i, ctx);
			m_pQuest[i]->OnBind( &ctx );
		}
	}
}

void CCardInfo::ApplyQuest(INT32 idx)
{
	QUEST_CTX ctx;
	GetQuestData(idx, ctx);
	for (int i = 0 ; i < MAX_QUEST_ARG ; ++i)
	{
		ctx.m_Arg[i] = m_pQuest[idx]->getArg(i);
	}
	SetQuestData(idx, ctx);
}


UINT32 CCardInfo::GetPrize(void)
{
	return m_QuestItemInfo.m_ui32Prize; 
}

UINT32 CCardInfo::GetPoint(void)
{
	return m_QuestItemInfo.m_ui32Point; 
}

UINT32 CCardInfo::GetExp(void)
{
	return m_QuestItemInfo.m_ui32Exp; 
}

void CCardInfo::SetActiveQuestIndex( UINT32 nRow, UINT32 idx)
{
	//idx�� card col max index���� ũ�� �� �Ϸ���·� ǥ��
	if( idx >= MAX_QUEST_COL_PER_CARD)
	{
		m_ActiveIndex = (UINT16)(m_ActiveIndex | 0xF << (nRow * 4));	// �ش� ���� active idx �� 0xF�� ����( �Ϸ�)
		return;
	}

	//quest ctx��ü�� ���ٴ°��� ���� ����Ʈ��� �̾߱�
	QUEST_CTX Ctx;
	GetQuestData( nRow, Ctx );


	UINT32 ncol = m_ActiveIndex & ~( 0xF << (nRow * 4));		//���� ������ �ε������� �����
	if( Ctx.m_ID == 0)							//quest�� ���³�
	{
		m_ActiveIndex = (UINT16)(ncol | 0xF << (nRow * 4));	//���� ���� �ε����� ����
	}
	else
	{
		// (����) ���� �� �����ϴ�.
		//if( pACtx->m_ID == 0xFFFF)											//�����γ�
		//{
		//	//make new quest for Randomcard
		//}
		//else
		{
			m_ActiveIndex = (UINT16)(ncol | 0xF << (nRow * 4));//���� ���� �ε����� ���� 
		}
	}
}


//�ش� ���� active quest�� �Ϸ��Ų��.

bool CCardInfo::CompleteQuest( UINT32 nIdx)
{
	UINT32 nQuestRow = 0;

	nQuestRow = getActiveQuestIndex( nIdx );

	if( nQuestRow == 0xF)//�̹� ����ó���� ���Դϴ�.
		return false;
	
	SaveQuest(nIdx);

	//���� ����Ʈ�� ActiveQuest�� ����
	SetActiveQuestIndex( nIdx, nQuestRow+1 );

	return true;
}

void CCardInfo::SaveQuest( UINT32 nIdx)
{
	//active quest�� ������·� �����.
	CQuestBase	* pQuest	= getQuest(nIdx);
	QUEST_CTX	QuestCtx;
	GetQuestData(nIdx, QuestCtx);

	//QuestCtx�� arg���� ����ȭ��Ų��. �̹� �������� ���� ����� ����
	for( INT32 i = 0; i < MAX_QUEST_ARG; ++i)
	{
		QuestCtx.m_Arg[i] = pQuest->getArg( i);
	}
	SetQuestData(nIdx, QuestCtx);
}


bool CCardInfo::IsCompletedCard()
{
	return (getActiveQuestIndex() == 0xFFFF);
}


void CCardInfo::SetQuestItemData( const QUEST_ITEM_INFO_NEW & ItemInfo )
{
	m_QuestItemInfo = ItemInfo;
}


void CCardInfo::GetQuestData( INT32 Index, QUEST_CTX & Ctx )
{
	//TODO: ��ȣȭ
	Ctx = m_QuestData[Index];
	GlobalFunc::DecryptValue(&Ctx, sizeof(Ctx), ENC_SEED_QUEST_CTX);
}

void CCardInfo::SetQuestData( INT32 Index, QUEST_CTX Ctx )
{
	//TODO: ��ȣȭ
	GlobalFunc::EncryptValue(&Ctx, sizeof(Ctx), ENC_SEED_QUEST_CTX);
	m_QuestData[Index] = Ctx;
}

void CCardInfo::SetQuestCompalte( INT32 Index, UINT8 Value )
{
	QUEST_CTX ctx;
	GetQuestData(Index, ctx);
	ctx.m_Arg[MAX_QUEST_ARG_COMPLETE] = Value;
	SetQuestData(Index, ctx);
}


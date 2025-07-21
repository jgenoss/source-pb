#include "pch.h"
#include "QuestBase.h"

I3_CLASS_INSTANCE( CQuestBase);//, i3ElementBase);

CQuestBase::CQuestBase(void)
{
	i3mem::FillZero( m_Arg, sizeof( m_Arg));
	m_ID = 0;
}

CQuestBase::~CQuestBase(void)
{
}

void CQuestBase::getProgressColor( INT32 cur, INT32 maxcount, COLOR * pColor)
{
	static UINT8 s_ColorTable[3][4] =
	{
		{	206, 206, 206, 255 },		// ���۵� ������ ��
		{	252, 255, 100, 255 },		// ���� ���� ��
		{	159, 252, 0,	255 }		// �Ϸ����� ��
	};

	if( cur <= 0.0f)
	{
		pColor->r = s_ColorTable[0][0];
		pColor->g = s_ColorTable[0][1];
		pColor->b = s_ColorTable[0][2];
		pColor->a = s_ColorTable[0][3];
	}
	else if( cur == maxcount)
	{
		pColor->r = s_ColorTable[2][0];
		pColor->g = s_ColorTable[2][1];
		pColor->b = s_ColorTable[2][2];
		pColor->a = s_ColorTable[2][3];
	}
	else
	{
		pColor->r = s_ColorTable[1][0];
		pColor->g = s_ColorTable[1][1];
		pColor->b = s_ColorTable[1][2];
		pColor->a = s_ColorTable[1][3];
	}
}

void CQuestBase::OnBind( const QUEST_CTX * pCtx)
{
	m_ID = pCtx->m_ID;

	for( INT32 i = 0; i < MAX_QUEST_ARG; i++)
	{
		m_Arg[i] = pCtx->m_Arg[i];
	}

//	I3TRACE("Arg 1: %d 2: %d 3: %d 4: %d 5: %d 6: %d\n", m_Arg[0],m_Arg[1],m_Arg[2],m_Arg[3],m_Arg[4],m_Arg[5]);
}

// ���ΰ����� ����, �ٸ� ĳ���Ͱ� �׾��� �� ȣ��ȴ�.
// Kill Chara.�� ���� ��ü���� ������ KILL_INFO Structure�� ����Ǿ� �ִ�.
bool		CQuestBase::OnKill( KILL_INFO * pKillInfo)
{
	return false;
}

// �ٸ� ĳ���Ϳ� ���� ���ΰ� ĳ���Ͱ� ������ ������ ���� ȣ��ȴ�.
// Kill Chara�� ���� ��ü���� ������ KILL_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase::OnDeath( KILL_INFO * pDeathInfo)
{
	return false;
}

// �ɸ��Ͱ� ������ ������ �� ȣ��ȴ�.
// ���� ���뿡 ���� ��ü���� ������ DAMAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase::OnDamage( DAMAGE_INFO * pDamgeInfo)
{
	return false;
}

// ���Ӱ� Stage�� �������� �� ȣ��ȴ�.
// Deathmatch, Mission ���� ������ �����ϰ� �� Stage���� �ѹ��� ȣ��ȴ�.
// ���� �����ϴ� Stage�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase::OnStageStart( STAGE_INFO * pStage)
{
	return false;
}

// Stage�� ������ �� ȣ��ȴ�.
// Deathmatch, Mission ���� ������ �����ϰ� �� Stage���� �ѹ��� ȣ��ȴ�.
// ���� �����ϴ� Stage�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase::OnStageEnd( STAGE_INFO * pStageInfo)
{
	return false;
}

// ���Ӱ� Round�� �������� �� ȣ��ȴ�.
// Deathmatch ��� ���� Round�� Mission�� �ƴѰ�쿡�� ȣ����� ���� �� �ִ�.
// ���� �����ϴ� Round�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase::OnRoundStart( STAGE_INFO * pStageInfo)
{
	return false;
}

// Round�� ������ �� ȣ��ȴ�.
// Deathmatch ��� ���� Round�� Mission�� �ƴѰ�쿡�� ȣ����� ���� �� �ִ�.
// ���� �����ϴ� Round�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase::OnRoundEnd( STAGE_INFO * pStageInfo)
{
	return false;
}

// ���ΰ� ĳ���Ͱ� Ư�� ������ ������ �� ȣ��ȴ�.
// ���ۿ� ���� ��ü���� ������ ACTION_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase::OnAction( ACTION_INFO * pAction)
{
	return false;
}

// ȭ�鿡 ����� Mission�� ���� ������ �䱸�� �� ȣ��ȴ�.
// CQuestBase Class���� �Ļ��� ���� Mission Class����
// �ݵ�� �� �Լ��� Override�Ͽ� ������ ������ ���ڿ��� �����ؾ� �Ѵ�.
void		CQuestBase::OnQueryDesc( char * pszBuff, INT32 maxLen)
{
	I3ASSERT( pszBuff != NULL);

	*pszBuff = 0;
}

void		CQuestBase::OnQueryHUDDesc( char * pszBuff, INT32 maxLen, bool bLineFeed)
{
	*pszBuff = 0;
}

// Quest�� ���� ���¸� ��ȯ�Ѵ�.
QUEST_STATE	CQuestBase::OnQueryState(void)
{
	return QST_NOT_STARTED; 
}

bool	CQuestBase::ForceComplete(void)
{
	return false;
}

#if !defined( __QUEST_BASE_MAP_COUNT_ACCM_TEAM_H)
#define __QUEST_BASE_MAP_COUNT_ACCM_TEAM_H

#include "QuestBase.h"

class CQuestBase_Map_Count_Accm_Team : public CQuestBase
{
	I3_CLASS_DEFINE( CQuestBase_Map_Count_Accm_Team, CQuestBase);
protected:
	UINT8				m_nValidKillCount;							//���� �׼ǿ� ���� ī��Ʈ�� �ʿ��Ѱ��(double/triple kill)�ӹ��� ���������� kill�� ī��Ʈ�Ѵ�.

	ACTION_ID		_getAction(void)						{	return (ACTION_ID) getArg( 0); }
	void			_setAction( ACTION_ID action)			{	setArg( 0, (UINT8)action); }

	STAGE_ID		_getStage(void)							{	return (STAGE_ID) getQuestStageID( getArg( 1)); }
	//void			_setStage( STAGE_ID stage)				{	setArg( 1, (UINT8) stage); }

	UINT8			_getCount(void)							{	return getArg( 2);	}
	void			_setCount( UINT8 cnt)					{	setArg( 2, cnt); }

	bool			_isAccmQuest(void)						{	return getArg( 3) != 0; }
	void			_setAccmQuest( bool bFlag)				{	if( bFlag)	setArg( 3, 1);	else	setArg( 3, 0);	}

	UINT8			_getTeam(void)							{	return getArg( 4); }
	void			_setTeam( UINT8 team)					{	setArg( 4, team); }

	bool			_checkCondition(void);

	void			_setValidKillCount( UINT8 cnt)				{	m_nValidKillCount = cnt;}
	UINT8			_getValidKillCount(void)					{	return m_nValidKillCount;}

public:
	CQuestBase_Map_Count_Accm_Team(void);
	virtual ~CQuestBase_Map_Count_Accm_Team(void);

	// ���ΰ����� ����, �ٸ� ĳ���Ͱ� �׾��� �� ȣ��ȴ�.
	// Kill Chara.�� ���� ��ü���� ������ KILL_INFO Structure�� ����Ǿ� �ִ�.
	virtual bool		OnKill( KILL_INFO * pKillInfo);

	// �ٸ� ĳ���Ϳ� ���� ���ΰ� ĳ���Ͱ� ������ ������ ���� ȣ��ȴ�.
	// Kill Chara�� ���� ��ü���� ������ KILL_INFO Structure�� ���� ���޵ȴ�.
	virtual bool		OnDeath( KILL_INFO * pDeathInfo);

	// �ɸ��Ͱ� ������ ������ �� ȣ��ȴ�.
	// ���� ���뿡 ���� ��ü���� ������ DAMAGE_INFO Structure�� ���� ���޵ȴ�.
	virtual bool		OnDamage( DAMAGE_INFO * pDamgeInfo);

	// ���Ӱ� Stage�� �������� �� ȣ��ȴ�.
	// Deathmatch, Mission ���� ������ �����ϰ� �� Stage���� �ѹ��� ȣ��ȴ�.
	// ���� �����ϴ� Stage�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
	virtual bool		OnStageStart( STAGE_INFO * pStage);

	// Stage�� ������ �� ȣ��ȴ�.
	// Deathmatch, Mission ���� ������ �����ϰ� �� Stage���� �ѹ��� ȣ��ȴ�.
	// ���� �����ϴ� Stage�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
	virtual bool		OnStageEnd( STAGE_INFO * pStageInfo);

	// ���Ӱ� Round�� �������� �� ȣ��ȴ�.
	// Deathmatch ��� ���� Round�� Mission�� �ƴѰ�쿡�� ȣ����� ���� �� �ִ�.
	// ���� �����ϴ� Round�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
	virtual bool		OnRoundStart( STAGE_INFO * pStageInfo);

	// Round�� ������ �� ȣ��ȴ�.
	// Deathmatch ��� ���� Round�� Mission�� �ƴѰ�쿡�� ȣ����� ���� �� �ִ�.
	// ���� �����ϴ� Round�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
	virtual bool		OnRoundEnd( STAGE_INFO * pStageInfo);

	// ���ΰ� ĳ���Ͱ� Ư�� ������ ������ �� ȣ��ȴ�.
	// ���ۿ� ���� ��ü���� ������ ACTION_INFO Structure�� ���� ���޵ȴ�.
	virtual bool		OnAction( ACTION_INFO * pAction);

	// ȭ�鿡 ����� Mission�� ���� ������ �䱸�� �� ȣ��ȴ�.
	// CQuestBase Class���� �Ļ��� ���� Mission Class����
	// �ݵ�� �� �Լ��� Override�Ͽ� ������ ������ ���ڿ��� �����ؾ� �Ѵ�.
	virtual void		OnQueryDesc( char * pszBuff, INT32 maxLen);

	virtual void		OnQueryHUDDesc( char * pszBuff, INT32 maxLen, bool bLineFeed = true);

	// Quest�� ���� ���¸� ��ȯ�Ѵ�.
	virtual QUEST_STATE	OnQueryState(void);

	// Quest�����Ϸ�
	virtual	bool		ForceComplete(void);
};

#endif

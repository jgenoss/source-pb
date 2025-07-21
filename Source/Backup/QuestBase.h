#if !defined( __QUEST_BASE_H)
#define __QUEST_BASE_H

enum QUEST_STATE
{
	QST_NONE = -1,
	QST_NOT_STARTED = 0,
	QST_DOING,
	QST_COMPLETED
};

class CQuestBase : public i3ElementBase
{
	I3_CLASS_DEFINE( CQuestBase, i3ElementBase);
protected:
	QUEST_ID				m_ID;
	UINT8					m_Arg[ MAX_QUEST_ARG];

protected:
	void					getProgressColor( INT32 current, INT32 maxCount, COLOR * pColor);
public:
	CQuestBase(void);
	virtual ~CQuestBase(void);

	QUEST_ID			getID(void)							{ return m_ID; }
	void				setID( QUEST_ID id)				{ m_ID = id; }

	UINT8				getArg( INT32 idx)					{ I3ASSERT( (idx >= 0) && (idx < MAX_QUEST_ARG));		return m_Arg[ idx]; }
	void				setArg( INT32 idx, UINT8 val)		{ I3ASSERT( (idx >= 0) && (idx < MAX_QUEST_ARG));		m_Arg[ idx] = val; }

	virtual void		OnBind( const QUEST_CTX * pCtx);

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

	// HUD �󿡼� ����� Quest�� ������� ���ڿ��� ��ȯ�Ѵ�.
	virtual void		OnQueryHUDDesc( char * pszBuff, INT32 maxLen, bool bLineFeed = true);

	// Quest�� ���� ���¸� ��ȯ�Ѵ�.
	virtual QUEST_STATE	OnQueryState(void);

	// Quest�����Ϸ�
	virtual	bool		ForceComplete(void);
};

#endif

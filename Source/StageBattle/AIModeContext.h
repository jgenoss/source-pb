#if !defined( __AI_MODE_CONTEXT_H)
#define __AI_MODE_CONTEXT_H

struct AIModeContext : public i3::shared_ginst<AIModeContext>
{
	INT32 FriendCount;			// �챺 ��
	
	INT32 EnemyCount;			// ���� ��
	INT32 NewEnemyCount;		// 

	INT32 StartLevel;
	INT32 CurrentLevel;			// AIMode ���̵�

	INT32 CurrentAILevel[SLOT_MAX_COUNT];		// ÿ������ ������ ��� Ai �ɸ��� �� ����
	AI_ENTRY AIEntry[SLOT_MAX_COUNT];			// �Ʒ��� ���̵� ����

	INT32 KillCount;
	UINT16 UserScore[SLOT_MAX_COUNT]; 

	AIModeContext();

	/** \brief ���� ������ ����ϴ� ������� BGLoad�� ��´�. */
	void		PrepareAIWeapon( void);
	
};

#endif
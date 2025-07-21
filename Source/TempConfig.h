#ifndef _TEMP_CONFIG_H__
#define _TEMP_CONFIG_H__

// �� Ŭ������ ���� �ʿ信 ���� �ӽ������� ����ϴ� Ŭ�����Դϴ�.
// �ܺ����Ϸ� ���� ���� �о�ö� ����մϴ�. (ini���� ���)

//AI Mode�� ���(DieHard)
#define MAX_AI_COUNT	10

enum EVADE_TYPE
{
	EVADE_TYPE_MOVE = 0,
	EVADE_TYPE_JUMP,	

	EVADE_TYPE_COUNT,
};

class CTempConfig :	public i3ElementBase
{
	I3_CLASS_DEFINE( CTempConfig, i3ElementBase );

public:
	////////////////////////////////////////////////////////
	REAL32		m_Speed[MAX_AI_COUNT+1];
	REAL32		m_Damage[MAX_AI_COUNT+1];
	REAL32		m_PlayerCountPerDamage[4];
	INT32		m_Weapon[MAX_AI_COUNT+1];
	INT32		m_Evade[EVADE_TYPE_COUNT];
	REAL32		m_RespawnTime;
	REAL32		m_JumpHeight;
	REAL32		m_JumpTime;

public:
	CTempConfig(void);
	~CTempConfig(void);

	void		Load();
};

extern CTempConfig * g_pTempConfig;

#endif //_TEMP_CONFIG_H__
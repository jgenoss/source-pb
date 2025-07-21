#pragma once

#include "ModeScript.h"

//  TODO : GameContext�� ��ų����Ʈ ���õ� ��� ���� �Լ��� �������� �ű⵵�� ����...
//       : �������̸� �����Լ��� find_modedata_index, get_modedata, calc_modedata, set_modedata�� Ȱ���� �ܼ��� �����ϴ°� ���ڴ�.

class DomiScript : public IModeScript
{
private:
	BOOL		_SetSkillSPTable(void);
	INT32		_GetSkillLevelIndex( INT32 nType, INT32 nLv);

	void		_SetSentrygunMaxHP();

public:
	DomiScript(i3Stage* pStage);
	virtual ~DomiScript();

	virtual void OnOpen();				// �ʿ��� ��� �������� ��� ��...�� ���� ���� �ʿ��Ѱ͸�...

	INT32		GetDay(void) const			{ return m_i32RoomModeData[ m_nIdxDay];			}
	INT32		GetWave(void) const			{ return m_i32RoomModeData[ m_nIdxWave];		}
	INT32		GetFlow(void) const			{ return m_i32RoomModeData[ m_nIdxFlowType];	}
	INT32		GetWaveMission(void) const  { return m_i32RoomModeData[ m_nIdxSubFlowType ];}

	INT32		GetScore( INT32 nIndex) const	{ I3_BOUNDCHK( nIndex, SLOT_MAX_COUNT); return m_i32RoomModeData[ m_nIdxScoreStart + nIndex];	}
	REAL32		GetSPCur( INT32 nIndex) const	{ I3_BOUNDCHK( nIndex, SLOT_MAX_COUNT); return (REAL32)m_i32RoomModeData[ m_nIdxSPStart + nIndex];	}
	INT32		GetLife( INT32 nIndex) const	{ I3_BOUNDCHK( nIndex, SLOT_MAX_COUNT); return m_i32RoomModeData[ m_nIdxLifeStart + nIndex];	}
	REAL32		GetStartTime( INT32 nIndex) const	{ I3_BOUNDCHK( nIndex, SLOT_MAX_COUNT); return (REAL32)m_i32RoomModeData[ m_nIdxSTTimeStart + nIndex];	}

	INT32		GetRemainDino(void) const		{ return m_i32RoomModeData[ m_nIdxRemainDino ];		}
	INT32		GetWaveSetTime(void) const		{ return m_i32RoomModeData[ m_nIdxWaveSetTime ];	}

	REAL32		GetSPMax(void) const			{ return m_rSPMax;				}
	REAL32		GetSPChargeSpd(void) const		{ return m_rSPChargeSpd;		}

	INT32		GetSkillCount(void) const		{ return m_nSkillCount;			}
	INT32		GetMaxLevel(void) const			{ return m_nSkillMaxLevel;		}

	INT32		GetSkillCool(INT32 nType, INT32 nLv);	// ���� ��Ÿ��
	REAL32		GetSkillOpen(INT32 nType, INT32 nLv);	// ��ų�� ���������� ���� SP
	REAL32		GetSkillCostSP(INT32 nType, INT32 nLv);	// ��ų�� �Ҹ� SP
	UINT32		GetSkillItem(INT32 nType, INT32 nLv);	// ��ų�� ���Ǵ� ������ ID

	INT32		GetUserGameOverFlag(void) const		{ return m_i32RoomModeData[ m_nIdxUserGameOverFlag ];	}

	INT32		GetSentrygunMaxHP(INT32 level);

private:
	INT32		m_nIdxDay;
	INT32		m_nIdxWave;
	INT32		m_nIdxFlowType;
	INT32		m_nIdxSubFlowType;

	INT32		m_nIdxScoreStart;
	INT32		m_nIdxScoreEnd;
	INT32		m_nIdxSPStart;
	INT32		m_nIdxSPEnd;
	INT32		m_nIdxLifeStart;
	INT32		m_nIdxLifeEnd;
	INT32		m_nIdxSTTimeStart;
	INT32		m_nIdxSTTimeEnd;

	INT32		m_nIdxRemainDino;
	INT32		m_nIdxWaveSetTime;

	REAL32		m_rSPMax;
	REAL32		m_rSPChargeSpd;

	INT32*		m_pnSkillTable;
	INT32		m_nSkillCount;
	INT32		m_nSkillValueSize;		// ��ų �ϳ��� ���̺� ũ��
	INT32		m_nSkillMaxLevel;
	INT32		m_nSkillValueCount;

	INT32		m_nIdxUserGameOverFlag;

	INT32       m_nSentrygunMaxHP[4];
};



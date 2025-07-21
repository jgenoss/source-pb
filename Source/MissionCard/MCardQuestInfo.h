#pragma once


struct MCardQuestInfo
{
	QUEST_ID			m_QuestID = 0;								// ������ ������ �̻��ϰ� ���̰� ����..(��������¿�)
	ACTION_ID			m_ActionID = ACTION_ID_NA;					// ������ enum ������ �������� ������, ü�迡 ���� ����..(��������¿�)

	STAGE_ID			m_StageID = 0;								// ������ ����..
	INT32				m_TargetCompleteCount = 0;	//
	TEAM_TYPE			m_Team = TEAM_NONE;							// Red or Blue or Any
	WEAPON_CLASS_TYPE	m_WeaponClassType = WEAPON_CLASS_UNKNOWN;	// ������(������ ����)
	INT32				m_WeaponNumber = 0;							// ����ѹ�(������ ����)

	bool				m_bAccum = false;							// 0 : ���� 1: ����..	
};

void ResetMCardQuestInfo(MCardQuestInfo* info);


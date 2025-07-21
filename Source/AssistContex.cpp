#include "pch.h"
#include "AssistContext.h"
#include "BattleSlotContext.h"

AssistContext::AssistContext()
{
	i3mem::Fill(m_Assistor, -1, sizeof(m_Assistor));
}

void AssistContext::SetAssistor(INT32 idx, INT32 assistor)
{
	I3ASSERT(idx >= 0 && idx < SLOT_MAX_COUNT)

	// Ÿ����, �ǰ��� �� �� ������ ���� ������ Ȯ��
	if (BattleSlotContext::i()->IsMyTeam(idx))
	{
		// �ǰ��ڰ� �������� ��� �ǰ����� ���¸� Ȯ��
		if (BattleSlotContext::i()->getSlotInfo(idx)->_State == SLOT_STATE_BATTLE && m_Assistor[idx][1] != assistor)
		{
			m_Assistor[idx][0] = m_Assistor[idx][1];
			m_Assistor[idx][1] = assistor;
		}
	}
	else
	{
		// Ÿ���ڰ� �������� ��� Ÿ������ ���¸� Ȯ��
		if (BattleSlotContext::i()->getSlotInfo(assistor)->_State == SLOT_STATE_BATTLE && m_Assistor[idx][1] != assistor)
		{
			m_Assistor[idx][0] = m_Assistor[idx][1];
			m_Assistor[idx][1] = assistor;
		}
	}

}

INT32 AssistContext::GetAssistor(INT32 idx, INT32 killer)
{
	I3ASSERT( idx >= 0 && idx < SLOT_MAX_COUNT)

	INT32 nAssistor = m_Assistor[idx][1];

	// ���� ��ý��Ͱ� Killer�� ���� ��� ���� ��ý��ͷ� �����
	if (nAssistor == killer && m_Assistor[idx][0] != -1)
		nAssistor = m_Assistor[idx][0];

	// Kill�� �߻������Ƿ� �ش� �ε����� ��ý��͸� �����
	m_Assistor[idx][0] = m_Assistor[idx][1] = -1;

	return nAssistor;
}

void AssistContext::AllResetAssistor()
{
	for (INT32 i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		for (INT32 j = 0; j < 2; j++)
		{
			m_Assistor[i][j] = -1;
		}
	}
}

void AssistContext::UpdateAssistorInfo()
{
	for (INT32 i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		if (m_Assistor[i][1] == -1)
			continue;

		// ������ ������ �� ���� ������ ������ ���� ���� ������ �ε����� ������ ���� ������
		if (BattleSlotContext::i()->IsMyTeam(i))
		{
			if (BattleSlotContext::i()->getSlotInfo(i)->_State != SLOT_STATE_BATTLE)
				m_Assistor[i][0] = m_Assistor[i][1] = -1;
		}
		else
		{
			if (BattleSlotContext::i()->getSlotInfo(m_Assistor[i][1])->_State != SLOT_STATE_BATTLE)
			{
				m_Assistor[i][1] = m_Assistor[i][0];
				m_Assistor[i][0] = -1;
			}
		}
	}
}
#pragma once

// AI�������� ����ϴ� ��ý��� ���� ���� Context
// ���� �������� ��ý�Ʈ ������ ���������� ���� AI�� ������ ����� ���� ������ �����Ƿ�
// AI���� ������ �����ϱ� ������ �ӽ÷� ����մϴ�.
// ���� AI���� ������ �����ϸ� �����ؾ���...

class AssistContext : public i3::shared_ginst<AssistContext>
{
public:
	AssistContext();

	// AI������ ������ ��ý�Ʈ �Ǻ��Ͽ� ������
	void				SetAssistor(INT32 idx, INT32 assistor);		// Hit�� �� ������ Ÿ���ڸ� ��ý��ͷ� ���� 
	INT32				GetAssistor(INT32 idx, INT32 killer);		// Kill�� �߻����� ��� ��ý��͸� ����

	void				AllResetAssistor();
	void				UpdateAssistorInfo();

private:
	INT32				m_Assistor[16][2];				// [initialize at constructor]
};

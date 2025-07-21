#pragma once

// �ٱ����� ����...(��������ü�� ���� �ʾ� ���δ�.. )

struct NEW_CREATE_CHARA
{	
	UINT8					m_ui8BuyType = 0;
	T_ItemID				m_TItemID = 0;										// ĳ���� ������ ID (����)
	UINT8					m_ui8Class = 0;
	i3::rc_wstring			m_wstrCharaName;						// NET_CHARA_NICK_NAME_SIZE = 33 (��ũ�ΰ� �ټ� �������̴�)

	bool					_bCheck = false;
};

inline void		Reset( NEW_CREATE_CHARA* inout)
{

	inout->m_TItemID = 0;
	inout->m_ui8Class = CHARA_CLASS_NONE;

	inout->m_wstrCharaName.clear();
	inout->_bCheck = false;
}

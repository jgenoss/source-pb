#if !defined( __MEDALDEF_CLIENT_H__)
#define __MEDALDEF_CLIENT_H__

#include "../../../Common/CommonSource/Medal/MedalDef.h"

#define	MAX_MEDAL_INFO_PROTOCOL	3
														// �켱 �ӽ÷� �̷��� ó��..
struct MEDAL_INFO
{
	bool					m_bActiveMedal = false;		// �������� �ִ� �ε��� �޴޸� Ȱ��ȭ
	UINT16					m_ui16ImageIdx = 0;			// �޴� �̹��� �ε���
	UINT16					m_ui16Idx = 0;				// �޴� ���� �ε���.
	UINT16					m_ui16BattleCount = 0;		// �޴� �ӹ� ���� Ƚ��
	bool					m_bAcq = false;				// �޴� ȹ�� ����
	bool					m_bNew = false;				// ���� ȹ���� �޴� ����(UI��)
	bool					m_bNew2 = false;			// ���� ȹ���� �޴� ����(���â ǥ�ÿ�) - ���â ������ ����
	UINT8					m_ui8CurrentLevel = 0;		// ���� ����

	// ������ �޼�Ƚ��.
	UINT16					m_ui16Count[MEDAL_LEVEL_MAX + 1] = { 0 };
	// ������ ��������
	MQF_REWARD_INFO			m_eRewardInfo[MEDAL_LEVEL_MAX + 1] = { 0 };
	// ������ ����ȹ�濩��
	bool					m_bRewardAcq[MEDAL_LEVEL_MAX] = { false };

	char					m_szNameKey[MAX_STRING_MEDAL_NAME_KEY_COUNT] = { 0 };
	char					m_szDescription[MAX_STRING_MEDAL_DESCRIPTION_KEY_COUNT] = { 0 };



	MEDAL_INFO(MQF_MEDAL_INFO * m)
	{
		CopyFrom(m);
	}
	void	CopyFrom(MQF_MEDAL_INFO * m)
	{
		m_bActiveMedal =		false;
		m_ui16Idx =				m->m_ui16Idx;
		m_ui16ImageIdx =		m->m_ui16ImageIdx;
		m_ui16BattleCount =		0;
		m_ui8CurrentLevel =		0;
		m_bNew2 = false;
		setAcquired(false);
		setNewMark(false);
		
		m_ui16Count[0] = 0;
		for(INT32 i=0; i < MEDAL_LEVEL_MAX; ++i)
		{
			m_bRewardAcq[i]	 =	false;
		}
		for(INT32 i=0; i < MEDAL_LEVEL_MAX; ++i)
		{
			m_ui16Count[i+1] =	m->m_ui16Count[i];
			m_eRewardInfo[i] =	m->m_eRewardInfo[i];
		}

		i3::safe_string_copy(m_szNameKey,		m->m_szNameKey,		MAX_STRING_MEDAL_NAME_KEY_COUNT			);
		i3::safe_string_copy(m_szDescription, m->m_szDescription, MAX_STRING_MEDAL_DESCRIPTION_KEY_COUNT	);
	}

	void	setBattleCount(UINT16 num, bool inBattle);
	void	setTotalMedalReward(UINT8 reward);
	void	setMedalReward(UINT8 lv, bool bAcq);
	void	setNewMark(bool bNew)		{ m_bNew = bNew;	}
	void	setNewMedal(bool bNew)		{ m_bNew2 = bNew;	}
	void	setAcquired(bool bAcq)		{ m_bAcq = bAcq;	}
	void	setActive(bool bActive)		{ m_bActiveMedal = bActive;	}
	bool	isActive()					{ return m_bActiveMedal;	}
};

struct MedalSetInfo
{
	MEDAL_SET_TYPE	setType;
	UINT16			setIndex;
	INT32			medalSetCount;
	bool			bAquired;
	i3::vector< MEDAL_INFO* >		medalSet;
	MedalSetInfo(MEDAL_SET_TYPE _setType, UINT16 _setIndex, INT32 _medalSetCount, bool _bAquired)
		: setType(_setType), setIndex(_setIndex),  medalSetCount(_medalSetCount), bAquired(_bAquired)
	{}
};

struct MedalCompleteInfo
{
	MEDAL_SET_TYPE	_type = MEDAL_SET_TYPE_NORMAL;	// �޴� Ÿ��
	UINT16			_idx = 0;	// �޴� ���� �ε���
	MEDAL_LEVEL		_level = MEDAL_LEVEL_1;	// �޴� ����

	MedalCompleteInfo() {}

	void Reset(void)
	{
		_type = MEDAL_SET_TYPE_NORMAL;
		_idx = 0;
		_level = MEDAL_LEVEL_1;
	}

	void SetMedalCompleteInfo(MEDAL_SET_TYPE type, UINT16 idx, MEDAL_LEVEL level)
	{
		_type = type;
		_idx = idx;
		_level = level;
	}

	friend bool operator == (const MedalCompleteInfo & lhs, const MedalCompleteInfo & rhs)
	{
		return ( (lhs._type == rhs._type) && (lhs._idx == rhs._idx) && (lhs._level == rhs._level) );
	}
};

struct MedalRewardResult
{
	MedalCompleteInfo	_info;
	T_ItemID			_itemid = 0;
	bool				_success = false;

	MedalRewardResult() {}

	MedalRewardResult(const MedalCompleteInfo & info)
	{
		_info = info;
	}

	friend bool operator == (const MedalRewardResult & lhs, const MedalCompleteInfo & rhs)
	{
		return lhs._info == rhs;
	}
};

namespace medal_util
{
	typedef bool (*pf_direcory)(const i3::string& path);
	typedef void (*pf_file)(const i3::string& file_path, const i3::string& file_name);

	void browse_directory(const char* path, pf_direcory pf_d, pf_file pf_f);
}

#endif
#include "pch.h"
#include "PBCmd_Rules.h"
#include "../TBoundChk.h"
#include "../../../Cheatkey_LimitPred.h"
#include "../../../GameContextUtil.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace Cheatkey
{
	bool KillCount::_ParseParams(const i3::vector<i3::string>& params)
	{
		if( params.size() != 1)	return false;

		m_killCount = ::atoi(params[0].c_str());

		if (!TBoundChk<INT32>(m_killCount, 0, std::numeric_limits<INT32>::max())) return false;

		return true;
	}

	bool KillCount::_Execute()
	{
		GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_INCREASE_KILL_COUNT, &m_killCount );
		return true;
	}

	ICommand* KillCount::Creator()
	{
		return new KillCount(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// ���� �ð� ����.
	// ex) "/roundtime
	//--------------------------------------------------
	bool RoundTimeCtrl::_ParseParams(const i3::vector<i3::string>& params)
	{
		return true;
	}

	bool RoundTimeCtrl::_Execute()
	{
		GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_CONTROL_ROUNDTIME);

		return true;
	}

	ICommand* RoundTimeCtrl::Creator()
	{
		return new RoundTimeCtrl(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// TS �÷��� Ÿ�� ����.
	// ex) "/ts
	//--------------------------------------------------
	bool TS_IncrPlayTime::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.empty()) 
			m_minute = 1;
		else
			m_minute = ::atoi(params[0].c_str());

		if (!TBoundChk<INT32>(m_minute, 0, std::numeric_limits<INT32>::max())) return false;

		return true;
	}

	bool TS_IncrPlayTime::_Execute()
	{
		GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_INCREASE_PLAY_TIME, &m_minute );

		return true;
	}

	ICommand* TS_IncrPlayTime::Creator()
	{
		return new TS_IncrPlayTime(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// �ַ� �÷���.
	// ex) "/ts
	//--------------------------------------------------
	bool SoloPlay::_Execute()
	{
		GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_ENABLE_SOLO_PLAY);

		return true;
	}

	ICommand* SoloPlay::Creator()
	{
		return new SoloPlay(new LPred_PermittedAccountOnly(new LPred_OutgameOnly));
	}

	//--------------------------------------------------
	// �� Ž�� OFF
	// ex) "/ts
	//--------------------------------------------------
	bool HackDetectOff::_Execute()
	{
		GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_DETECT_HACK_OFF);

		return true;
	}

	ICommand* HackDetectOff::Creator()
	{
		return new HackDetectOff(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// �̼Ǹ�� ��ī��Ʈ ����.
	// ex) "/mwin ī��Ʈ
	//--------------------------------------------------
	bool ChangeMissionModeWinCount::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() != 2) return false;

		m_count_R = static_cast<UINT8>(::atoi(params[0].c_str()));
		m_count_B = static_cast<UINT8>(::atoi(params[1].c_str()));

		if (!TBoundChk<UINT8>(m_count_R, 0, std::numeric_limits<UINT8>::max())) return false;
		if (!TBoundChk<UINT8>(m_count_B, 0, std::numeric_limits<UINT8>::max())) return false;

		return true;
	}

	bool ChangeMissionModeWinCount::_Execute()
	{
		GameContextUtil::QA_SetRound(m_count_R, m_count_B); 

		return true;
	}

	ICommand* ChangeMissionModeWinCount::Creator()
	{
		return new ChangeMissionModeWinCount(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// ��Ʋ ����ð� ���������� ����.
	// ex) "/ltime
	//--------------------------------------------------
	bool ChangeBattleTimeUnlimit::_Execute()
	{
		GameContextUtil::QA_SetEndlessBattle(); 

		return true;
	}

	ICommand* ChangeBattleTimeUnlimit::Creator()
	{
		return new ChangeBattleTimeUnlimit(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// ��Ʋ ����
	// ex) "/pause ��
	//--------------------------------------------------
	bool PauseBattle::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() != 1) return false;

		m_sec = static_cast<UINT32>(::atoi(params[0].c_str()));

		if (!TBoundChk<UINT32>(m_sec, 0, std::numeric_limits<UINT32>::max())) return false;

		return true;
	}

	bool PauseBattle::_Execute()
	{
		GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_TIMER_GMPAUSE, &m_sec);

		return true;
	}

	ICommand* PauseBattle::Creator()
	{
		return new PauseBattle(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}


	//--------------------------------------------------
	// ųī��Ʈ ����
	// ex) "/setkill ������ī��Ʈ �����ī��Ʈ
	//--------------------------------------------------
	bool SetKillCount::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() == 1) 
			m_redKillCnt = static_cast<UINT16>(::atoi(params[0].c_str()));

		if (params.size() == 2)
		{
			m_redKillCnt = static_cast<UINT16>(::atoi(params[0].c_str()));
			m_blueKillCnt = static_cast<UINT16>(::atoi(params[0].c_str()));
		}

		if (!TBoundChk<UINT16>(m_redKillCnt, 0, std::numeric_limits<UINT16>::max())) return false;
		if (!TBoundChk<UINT16>(m_blueKillCnt, 0, std::numeric_limits<UINT16>::max())) return false;

		return true;
	}

	bool SetKillCount::_Execute()
	{
		GameContextUtil::QA_SetKillCount(m_redKillCnt , m_blueKillCnt);	

		return true;
	}

	ICommand* SetKillCount::Creator()
	{
		return new SetKillCount(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}


	//--------------------------------------------------
	// Ŭ�� ��ġ����ŷ - ��ġ�� ��Ī ����Ʈ ��ȸ
	// ex) "/clanwarm
	//--------------------------------------------------
	bool ClanWarMatchingPoint::_Execute()
	{
		GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_CLAN_WAR_MATCHING_POINT);

		return true;
	}

	ICommand* ClanWarMatchingPoint::Creator()
	{
		return new ClanWarMatchingPoint(new LPred_PermittedAccountOnly(new LPred_Anywhere));
	}


	//--------------------------------------------------
	// Ŭ�� ��ġ����ŷ - ��ġ ��� ��ȸ
	// ex) "/clanwarr �¸���.	
	//--------------------------------------------------
	bool ClanWarMatchResult::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() != 1) return false;

		m_eWinTeam = static_cast<TEAM_TYPE>(::atoi(params[0].c_str()));

		if( m_eWinTeam <= TEAM_NONE ) return false;
		if( m_eWinTeam >= TEAM_COUNT ) return false;

		return true;
	}

	bool ClanWarMatchResult::_Execute()
	{
		GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_CLAN_WAR_MATCH_RESULT, &m_eWinTeam);

		return true;
	}

	ICommand* ClanWarMatchResult::Creator()
	{
		return new ClanWarMatchResult(new LPred_PermittedAccountOnly(new LPred_Anywhere));
	}
}
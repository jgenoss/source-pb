#include "pch.h"
#include "ScoreContext.h"

#include "UI/UIHudUtil.h"
#include "./DinoModeContext.h"
#include "MyRoomInfoContext.h"
#include "UserInfoContext.h"
#include "BattleServerContext.h"
#include "../ui/UIHudManager.h"
#include "ObserverBattle.h"


ScoreContext::ScoreContext()
{
	CurRoundCount = 0;
	CurrentDediRound = 0;

	m_AccTrainingScore = 0;
	m_AddTrainingScore = 0;

	m_redWinRound = 0;
	m_blueWinRound = 0;
	
	m_autoTeamChangeExecuted = false;
	m_executeShowTeamAutoChangeHudEffect = false;
}

UINT8 ScoreContext::getCurrentRound( void)
{
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
	{	
		CurRoundCount = DinoModeContext::i()->CurrentDinoRound;	
	}

	return CurRoundCount;
}

void ScoreContext::setCurrentRound( UINT8 Round)
{
	CurRoundCount = Round;
}

void ScoreContext::ExecuteAutoTeamChange()
{
	if(!MyRoomInfoContext::i()->IsAutoTeamChangeableRoomState())
		return;

	m_autoTeamChangeExecuted = true;

	// �׻� m_autoTeamChangeExecuted = true �ؿ� ��ġ��Ű�ÿ�
	BattleServerContext::i()->ChangeTeam(); // ��ü���� �ȵȽ��Ը� �ٲٴ½õ���	

	// ���ٲ���� �����ǿ� �����Ͽ� ���ٲ���� ��������Ͽ� ��ϵ� �÷����ǰ�� �ٲ������� ��������� �������ʿ��ϴ�.
	// ��)3������ ���Ĺ̼�, 2���� �������� ���Խ���, 3���� �����÷��̾�->�������ε�� , 3���� ���ٲ���Ŷ�ް� ������, �ٲ������°� ��������� �����ʿ�
	//observer_battle::i()->RefreshObserver();
}

void ScoreContext::ShowTeamAutoChangeHudEffect()
{
	if(!MyRoomInfoContext::i()->IsAutoTeamChangeableRoomState())
		return;

	// ���� Ż�� or Cross Counter ��忡���� HUD ��� ����.
	if ( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || 
		MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT) )
		return;

	if(m_executeShowTeamAutoChangeHudEffect)
		return;
	
	m_executeShowTeamAutoChangeHudEffect = true;
	if( HUD::instance() != nullptr )
		HUD::instance()->SetParameter(HUD::DEFAUTOCHANGE, true);
	// UIHudDefAutoChange
}

void ScoreContext::InitTeamKillCount( void)
{
	m_teamScore[TEAM_RED].m_KillCount = 0;
	m_teamScore[TEAM_BLUE].m_KillCount = 0;
}

UINT16 ScoreContext::GetSvrTeamWinRound(TEAM_TYPE team) const
{
	return team == TEAM_RED ? m_redWinRound : m_blueWinRound;
}

void ScoreContext::SetSvrTeamWinRound(UINT16 red, UINT16 blue)
{
	m_redWinRound = red;
	m_blueWinRound = blue;
}

const SCORE_INFO* ScoreContext::GetSvrTeamScore(TEAM_TYPE team) const
{
	return &m_teamScore[team];
}

void ScoreContext::SetSvrTeamScore(const SCORE_INFO& redScore, const SCORE_INFO& blueScore)
{ 
	m_teamScore[TEAM_RED] = redScore;
	m_teamScore[TEAM_BLUE] = blueScore;
}

UINT16 ScoreContext::GetRedTeamWinRound(void) const
{ 
	return m_autoTeamChangeExecuted ? m_blueWinRound : m_redWinRound;
}

UINT16 ScoreContext::GetBlueTeamWinRound(void) const			
{ 
	return m_autoTeamChangeExecuted ? m_redWinRound : m_blueWinRound;
}

UINT16 ScoreContext::GetTeamWinRound(TEAM_TYPE team) const
{ 
	if(m_autoTeamChangeExecuted)
		return (team==TEAM_RED)? m_blueWinRound : m_redWinRound;
	else
		return (team==TEAM_RED)? m_redWinRound : m_blueWinRound;
}

const UINT16 ScoreContext::GetDinoEscapeScore(TEAM_TYPE team) const
{
	if (m_autoTeamChangeExecuted)
		return (team == TEAM_RED) ? DinoModeContext::i()->BlueTouchDownCount : DinoModeContext::i()->RedTouchDownCount;
	
	return (team == TEAM_RED) ? DinoModeContext::i()->RedTouchDownCount : DinoModeContext::i()->BlueTouchDownCount;
}

const SCORE_INFO* ScoreContext::GetTeamScore(TEAM_TYPE team) const
{ 
	I3_BOUNDCHK( team, TEAM_COUNT); 

	if(m_autoTeamChangeExecuted)
		return &m_teamScore[(team == TEAM_RED) ? TEAM_BLUE : TEAM_RED];

	return &m_teamScore[team];
}

void ScoreContext::CreateBattle( void)
{
	for( UINT32 i = 0; i < TEAM_COUNT; i++)
		m_teamScore[i].Reset();

	for( UINT32 i = 0; i < SLOT_MAX_COUNT; i++ )
		IndividualScore[i].Reset();

	ResetTrainingScore();

	m_redWinRound = 0;
	m_blueWinRound = 0;

	m_autoTeamChangeExecuted = false;
	m_executeShowTeamAutoChangeHudEffect = false;
}

void ScoreContext::DestroyBattle(void)
{
	m_autoTeamChangeExecuted = false;
	m_executeShowTeamAutoChangeHudEffect = false;
}

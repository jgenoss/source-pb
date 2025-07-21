#include "pch.h"
#include "GameEventReceiver.h"

GameEventReceiver::GameEventReceiver()
{


}

GameEventReceiver::~GameEventReceiver()
{


}

void GameEventReceiver::PushReceivedGameEvent( GAME_EVENT Event, INT32 Arg /*= EVENT_ERROR_SUCCESS*/)
{	//pArg�� ���߿� �ʿ��ϸ� ó��

	g_pFramework->PrintConsole(CONSOLE_LOG_ETC, "ReceiveGameEvent : %d", Event);

	// ó���� �ȵ� �̺�Ʈ�� ť�� ����.
	// moveó�������� �ϴ� ���� �ְ� ���߿� �����Ѵ�..
	m_qRecvEvent.push( i3::tuple<GAME_EVENT, INT32, i3::user_data>() );
	i3::tuple<GAME_EVENT, INT32, i3::user_data>& data =	m_qRecvEvent.back();

	i3::get<0>(data) = Event;
	i3::get<1>(data) = Arg;

}

void	GameEventReceiver::PushReceivedGameEventUserData( GAME_EVENT Event, i3::user_data& MoveData, INT32 Arg )
{
	m_qRecvEvent.push( i3::tuple<GAME_EVENT, INT32, i3::user_data>() );
	i3::tuple<GAME_EVENT, INT32, i3::user_data>& data =	m_qRecvEvent.back();

	i3::get<0>(data) = Event;
	i3::get<1>(data) = Arg;
	i3::get<2>(data) = i3::move(MoveData);			
}

bool	GameEventReceiver::PopRecvData(GAME_EVENT& evt, INT32& arg, i3::user_data& ud)	// ����° �μ��� copy�� �ƴϰ�, move�Ǿ���Ѵ�..
{
	if (m_qRecvEvent.empty() ) 
		return false;

	i3::tuple<GAME_EVENT, INT32, i3::user_data>& data = m_qRecvEvent.front();
	
	evt = i3::get<0>(data);
	arg = i3::get<1>(data);
	ud  = i3::move(i3::get<2>(data));
	m_qRecvEvent.pop();
	return true;
}


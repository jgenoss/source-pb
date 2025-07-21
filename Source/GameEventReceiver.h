#pragma once

#include "i3Base/itl/user_data.h"
#include "i3Base/itl/queue.h"

struct RecvData_EVENT_LOG_IN_WAIT
{
	INT32	WaitCount = 0;
	UINT32	WaitLoginTime = 0;
};

struct RecvData_EVENT_LOG_IN
{
	i3::rc_wstring	wstrCustomMsg;		//
};

struct RecvData_EVENT_DELETE_CHARA {
	UINT8		_DeleteSlotIdx = 0;
};

struct RecvData_EVENT_DEATH_BATTLE
{
	i3::vector<DEATH_INFO_CHARA>	DeathCharaInfo;		// ������̹Ƿ�, ���������ؼ� ������ �Ѵ�..
	UINT32				_KillWeaponFlag = 0;				//
	INT32				_nByObjectIdx = 0;					//
	UINT32				MyKillMsg = 0;
	INT32				MyKillCount = 0;
	INT32				MyPrevKillCount = 0;
	INT32				KillIdx = 0;
	UINT8				AssistIdx = 0;
};

struct RecvData_EVENT_DEATH_DOMI_USER
{
	i3::vector<DEATH_INFO_CHARA>	DeathCharaInfo;		// ������̹Ƿ�, ���������ؼ� ������ �Ѵ�..
};

struct RecvData_EVENT_BUY_GOODS
{
	i3::vector<INT32>				resultList;			
	i3::vector<T_GoodsID>			GoodsIDList;
};

struct RecvData_EVENT_SEND_GIFT : RecvData_EVENT_BUY_GOODS {};			// �����ϱ� ���ú���Ŷ�� ��������� ���� ���ſ� �����մϴ�.

struct RecvData_EVENT_ITEM_REPAIR : RecvData_EVENT_BUY_GOODS {};		// ������ �����ҵ� �ϳ�...�ϴ� Ÿ���� �����д�..



class GameEventReceiver : public i3::shared_ginst<GameEventReceiver>
{
public:
	GameEventReceiver();
	~GameEventReceiver();
	
	INT32		GetNumRecvEvent() const { return (INT32) m_qRecvEvent.size(); }
	bool		PopRecvData(GAME_EVENT& evt, INT32& arg, i3::user_data& ud);		// ����° �μ��� copy�� �ƴϰ�, move�Ǿ���Ѵ�..
	
	//��Ʈ��ũ �������� �̺�Ʈ ���ۿ� �ֽ��ϴ�. (Network -> GameContext)
	void	PushReceivedGameEvent( GAME_EVENT Event, INT32 Arg = EVENT_ERROR_SUCCESS);
	void	PushReceivedGameEventUserData( GAME_EVENT Event, i3::user_data& MoveData, INT32 Arg = EVENT_ERROR_SUCCESS);

	template<class T> typename i3::disable_if< i3::is_same<T, i3::user_data> >::type
	PushReceivedGameEventUserData( GAME_EVENT Event, const T& Data, INT32 Arg = EVENT_ERROR_SUCCESS)
	{
		i3::user_data udata(Data);
		PushReceivedGameEventUserData(Event, udata, Arg);
	}
	
private:
	
	// �����Ҵ����� �ʰ� �׳� ������ �����Ѵ�.. i3:deque/queue�� �Ҹ�ɶ����� �Ҵ���۸� ������ �ʴ´�..
	i3::queue<i3::tuple< GAME_EVENT, INT32, i3::user_data> >	m_qRecvEvent;
};


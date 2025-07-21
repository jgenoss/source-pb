#pragma once

#include "LuaAPIDef.h"

extern CSampleClass g_kSample; 

namespace LuaAPIEtc
{
	void Init(lua_State* L);

	//
	// Event Handler Functions
	// Called : Client, Server
	//
	LVOID OnTimeOut( TIMER timerIdx );	// setTimer �Լ��� ���� ������ Timer�� Time-Out�Ǿ��� �� �ѹ� ȣ��ȴ�. Time-Out�� �� Timer�� �⺻������ ������ �Ǹ�, �������� �ʴ´�.

	LVOID addKillScore( TEAM team, INT32 score );	// ������ ���� Kill Count�� ������Ų��.
	INT32 getKillScore( TEAM team );				// �ش� ���� Kill Count ������ ��ȯ�Ѵ�.
	LVOID killRoundTimer( bool ShowUI );			// Round Timer�� �����Ѵ�.
	LVOID killTimer( INT32 idTimer, bool showUI );	// PB Engine�� �����ϴ� 2���� User-Timer�� �����Ѵ�.
	LVOID setRespawnDelayTime( INT32 secs );		// Respawn ��� �ð��� �����Ѵ�.
	LVOID setRoundTimer( INT32 secs );				// Round Time�� �����Ѵ�.
	LVOID setTimer( INT32 idTimer, INT32 secs );	// PB Engine�� �����ϴ� 2���� User Timer�� �����Ѵ�. ������ Timer�� �ڵ����� �ð��� �����ϸ�, ���� UI�� ǥ���Ѵ�. �� Timer�� �ڽŸ��� UI�� ������ ������, ǥ�� ������ �����Ǿ� �ִ�. ���� Timer�� ���� �ڼ��� ������ �Ʒ��� Parameter �׸��� �����ϱ� �ٶ���.
													// �� Timer�� ����Ǿ��� ���, OnTimeOut1, �Ǵ� OnTimeOut2 Event Handler �Լ����� ���� ȣ��ȴ�.
};

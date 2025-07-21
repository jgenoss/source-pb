#pragma once

#include "LuaAPIDef.h"

extern CSampleClass g_kSample; 

namespace LuaAPIStage
{
	void Init(lua_State* L);

	//
	// Event Handler Functions
	// Called : Client, Server
	//
	LVOID OnEndRound();		// Round�� ����� �� �ѹ� ȣ��ȴ�. �� �Լ��� Round���� �ƴ�, ��ü���� ��쿡�� �ѹ� ȣ��ȴ�.
	LVOID OnEndStage();		// Stage�� ����� �� �ѹ� ȣ��ȴ�.
	LVOID OnPreStartRound();// Round�� ���۵Ǳ� �� �ѹ� ȣ��ȴ�. �� �Լ��� Round���� �ƴ�, ��ü���� ��쿡�� �ѹ� ȣ��ȴ�.
	LVOID OnStartRound();	// Round�� ������ �� �ѹ� ȣ��ȴ�. �� �Լ��� Round���� �ƴ�, ��ü���� ��쿡�� �ѹ� ȣ��ȴ�.
	LVOID OnStartStage();	// Stage�� ���۵� �� �ѹ� ȣ��ȴ�.

};

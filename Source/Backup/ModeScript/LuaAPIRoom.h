#pragma once

#include "LuaAPIDef.h"

namespace LuaAPIRoom
{
	// ���� ����(Room Master)�� ���� �������, Stage�� �����ϰ�, 
	// �����ϴ� �������� �߻��ϴ� ���� ���� �������� ȣ��Ǵ� Event Handler �Լ����̴�.
	void Init(lua_State* L);

	//
	// Event Handler Functions
	// Called : Client, Server
	//
	LVOID OnDetachPlayer( HPLAYER Player );	// Player�� ��Ʋ���� ���������� �ѹ��� ȣ��Ǹ�, ���������� ���� �÷��̾�� ��Ʋ �Ǵ� ���带 ���� �������� �Ǵ��Ѵ�.
	LVOID OnJoinPlayer( HPLAYER player );	// Player�� ��Ʋ�� ���ö����� �ѹ��� ȣ��Ǹ�, ������ ���ö� ��Ʋ ���� ���������� �Ǵ��Ͽ� ��Ʋ�� �����Ѵ�.
	LVOID OnStageConfig();					// Stage�� ���� ������ �Ѵ�. �̴� �Ϲ������� ���� �����Ǿ� Stage�� ������ �� Stage�� ����� ������ ȣ��ȴ�.
};

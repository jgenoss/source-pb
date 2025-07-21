#pragma once

#include "LuaAPIDef.h"

struct lua_State;

namespace LuaAPIObject
{
	void Init(lua_State* L);

	//
	// Event Handler Functions
	// Called : Client, Server
	//
	bool OnDamageObject( HPLAYER player, HOBJECT obj ); // Player�� �ش� Object�� Damage�� �� ��� ȣ��ȴ�.
	LVOID OnDestroyObject( HOBJECT obj );				// Ư�� Object�� HP�� 0�� �Ǿ� �ı��� �� �ѹ� ȣ��ȴ�. �ı��� �̹� ������ �����̸�, �������� �ʴ´�.

	
	HOBJECT	getObject( const char* name );
	UINT32	getObjectHP( HOBJECT obj );
	const char*	getObjectName( HOBJECT obj );
}
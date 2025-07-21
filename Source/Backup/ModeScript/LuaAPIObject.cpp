#include "pch.h"
#include "LuaAPIObject.h"

#include "ScriptDef.h"

namespace LuaAPIObject
{
	void Init(lua_State* L)
	{
		lua_tinker::def(L, "OnDamageObject", OnDamageObject);
		lua_tinker::def(L, "OnDestroyObject", OnDestroyObject);

		lua_tinker::def(L, "getObject", getObject);
		lua_tinker::def(L, "getObjectHP", getObjectHP);
		lua_tinker::def(L, "getObjectName", getObjectName);

	}

	// Return : Damage�� ����Ϸ��� true, �׷��� ������ false�� ��ȯ�Ѵ�.
	bool OnDamageObject(HPLAYER player, HOBJECT obj)
	{
		return true;
	}

	// Return : ����
	LVOID OnDestroyObject( HOBJECT obj )
	{
		return LNULL;
	}


	HOBJECT getObject( const char* name )
	{
		return &g_kSample;
	}

	UINT32 getObjectHP( HOBJECT obj )
	{
		return obj->GetHP();
	}

	const char* getObjectName( HOBJECT obj )
	{
		return obj->GetName();
	}
}
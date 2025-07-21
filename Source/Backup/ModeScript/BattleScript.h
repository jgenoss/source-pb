#pragma once

#include <Windows.h>
#include <string>

#include "ModeScript.h"

#include "ScriptDef.h"

class ModeScriptMgr;
struct lua_State;

class CBattleScript : public IModeScript
{
public:
	CBattleScript(i3Stage* pStage);
	virtual ~CBattleScript();

public:

	void DestroyModeData();			// �ı��̼�
	void DefenseModeData();			// ���̼�
	void DominationModeData(void);

};



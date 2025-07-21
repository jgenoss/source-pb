#ifndef _LUAMANAGER_H_
#define _LUAMANAGER_H_

class IModeScript;
struct lua_State;
struct mode_script_arg;

#include "ScriptDef.h"
#include "tinst.h"

// Ŭ���̾�Ʈ���� �ϳ��� ��ũ��Ʈ�� ȣ���Ѵٴ� �����Ͽ� ������ ��ҽ��ϴ�.
// Ȥ��, �������� ��彺ũ��Ʈ�� �����ÿ� ���������� �̷������ �մϴ�.  ��ƿ� ���õ� ������ �����ϰڽ��ϴ�. - ������

//
// �������̽������߽��ϴ�. - ����.
//

class ModeScriptMgr
{
public:
	void				Init();
	void				Set( STAGE_TYPE eStage, i3Stage* pStage, bool bDataReset );
	void				Reset();
	IModeScript*		GetScript() { return m_pScript; }
	const IModeScript*	GetScript() const { return m_pScript; }

	void				set_modedata(PROTOCOL p, P_BATTLE_SERVER_SYNC_MODEDATA_ACK* data, INT32 num);
	mode_script_arg&	call(const char* str_fun); // { return m_pScript->call(str_fun); }

private:
	ModeScriptMgr();
	virtual ~ModeScriptMgr();
	friend tinst_creator<ModeScriptMgr>;
	char m_szFile[STAGE_TYPE_MAX][MAX_STRING_COUNT];

	STAGE_TYPE		m_StageType;
	IModeScript*	m_pScript;
};

tinst<ModeScriptMgr>;

inline 
mode_script_arg&	script_call(const char* str_fun)
{
	return tinst<ModeScriptMgr>()->call(str_fun);
}

inline
void				set_modedata(PROTOCOL p, P_BATTLE_SERVER_SYNC_MODEDATA_ACK* data, INT32 num)
{
	return tinst<ModeScriptMgr>()->set_modedata(p, data, num);
}

#endif
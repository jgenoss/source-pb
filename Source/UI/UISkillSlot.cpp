#include "pch.h"
#include "UISkillSlot.h"

#include "UIMainFrame.h"
#include "UITabProfileAbility.h"

namespace
{
	class SkillSlot_Callback_None : public SkillSlot_Callback
	{
	public:
		virtual void	OnLoadSlot( INT32 slot_idx, i3UIScene* slot_scn ) {}				// �ε�� ���, �ʱ�ȭ�� ȣ�� ��ġ..
		virtual void	OnEnableSlot( INT32 slot_idx, i3UIScene* slot_scn) {}			// enableȣ��� ��� ������Ʈ�� ������ ä�� �Լ�..
	} g_callback_none;
}

UISkillSlot::UISkillSlot() : m_Parent(NULL), m_Callback(&g_callback_none), m_Row(0)
{
}

UISkillSlot::~UISkillSlot()
{
}

void UISkillSlot::OnCreate(UIBase* parent, INT32 row)
{
	m_Parent = parent;
	m_Row = row;
}

void UISkillSlot::RegisterSlotScene( const char *szSlotScene, const LuaFuncReg* Lua_Func, bool bLoaded)
{
	for (INT32 i = 0 ; i < m_Row ; ++i )	//���籸���� ���η� �� �Ѹ��� �����̱� ������..
	{
		m_Parent->AddScene(szSlotScene, Lua_Func, bLoaded, !bLoaded);					// ������ �Ѳ����� �ε��Ҷ� ���� �������� �ϴ� �����ҽ����ϰ� ��������� �ߺ��ε��Ǵ°� �ƴѰ� �ϴ� ��..
	}
}
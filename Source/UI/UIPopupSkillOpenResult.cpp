#include "pch.h"

#include "UIPopupSkillOpenResult.h"

#include "UIMainFrame.h"


I3_CLASS_INSTANCE( UIPopupSkillOpenResult);//, UIPopupItemBase);


void UIPopupSkillOpenResult::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate( pParent);
	AddScene("Scene/Popup/PBRe_PopUp_item_Use_Check.i3UIs");
}

bool UIPopupSkillOpenResult::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/ )
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	
	m_item_id = *( static_cast<T_ItemID*>( pArg1 ) );
	chara_idx = *( static_cast<INT8*>( pArg2 ));

	SetInfo();
	
	return true;
}

bool UIPopupSkillOpenResult::OnExitStart( void )
{
	if( UIPopupBase::OnExitStart() == false)
		return false;

	return true;
}

bool UIPopupSkillOpenResult::OnKey_Enter()
{
	_OnClose();

	return true;
}

bool UIPopupSkillOpenResult::OnKey_Escape( void )
{
	_OnClose();

	return true;
}

void UIPopupSkillOpenResult::SetInfo()
{
	LuaState* L = 0;

	
		
	CHARA_INFO* chara = const_cast<CHARA_INFO*>(CCharaInfoContext::i()->GetCharaInfoBySlotIdx(chara_idx));

	i3::wliteral_range chara_nick = chara->m_CharaBasicInfo.m_strCharaName;

	L = _CallLuaFunction( "SetCharaInfo");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, chara_nick);
		_EndLuaFunction(L, 1);
	}

	// SkillRelTime�� 500�ð����� ������Ų��	
	// Skill Master �������� ����ϰ� �Ǹ� ���������� 500�ð����� ������Ű�� ������
	// Ŭ���̾�Ʈ������ 500�ð����� �������ѵ� ����
	chara->m_ui32SkillRelTime = 1800000;
}

UIPopupSkillOpenResult::UIPopupSkillOpenResult()
{

}

UIPopupSkillOpenResult::~UIPopupSkillOpenResult()
{

}
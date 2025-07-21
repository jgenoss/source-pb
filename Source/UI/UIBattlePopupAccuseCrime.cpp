#include "pch.h"
#include "UIBattlePopupAccuseCrime.h"

#include "UIBattleFrame.h"
#include "UIUtil.h"
#include "CrosshairMgr.h"
#include "UIHudUtil.h"
#include "GameMaster.h"
#include "UIHUDForceRemove.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( UIBattlePopupAccuseCrime);

/************************/
extern "C" 
{
	int AccuseCrimeClick( LuaState * L)
	{	
		INT32 value1 = i3Lua::GetIntegerArg( L, 1);
		INT32 value2 = i3Lua::GetIntegerArg( L, 2);

		UIBattlePopupAccuseCrime * pThis = static_cast<UIBattlePopupAccuseCrime*>(GameUI::GetBattlePopup(UBP_ACCUSE_CRIME) );
		if( pThis != nullptr)
			pThis->clicked(value1, value2);

		return 0;
	}
}

LuaFuncReg UIHudAccuseCrimeClick_Glue[] = 
{
	{"Click", AccuseCrimeClick	},
	{nullptr,							nullptr}
};

/************************/
namespace inner
{
	enum	{ REASON, VOTE, USER, OK, CANCEL };

	//�ܼ� �ڵ� ���̱� ���� �뵵
	void to_lua_user_list(i3UIScene* pScene, const char* lua_name, const i3::vector< i3::pair<INT32, i3::rc_wstring> > & list);
}


/************************/
void UIBattlePopupAccuseCrime::clicked(int idx1, int idx2)
{
	switch( idx1 )
	{
	case inner::REASON: m_Reason = (FORCEREMOVE_CAUSE)idx2; check_ok_btn(); break;
	case inner::VOTE: m_Vote_Type = idx2; check_ok_btn(); break;
	case inner::USER: m_Selected_Index = idx2; check_ok_btn(); break;
	case inner::OK: click_ok();break;
	case inner::CANCEL: click_cancel(); break;
	}
}

/************************/
UIBattlePopupAccuseCrime::UIBattlePopupAccuseCrime()
{
	m_Reason = FORCEREMOVE_NONE;
	m_Vote_Type = 0;
}

UIBattlePopupAccuseCrime::~UIBattlePopupAccuseCrime()
{
}

/************************/
/*virtual*/ void UIBattlePopupAccuseCrime::OnCreate( i3GameNode * pParent)
{
	UIBattlePopupBase::OnCreate( pParent);

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == true ) return; // Ʃ�丮�󿡼��� �ε����� �ʵ���

	AddScene("Scene/HUD/PBRe_Report_Player.i3UIs", UIHudAccuseCrimeClick_Glue, true, false);
}

bool UIBattlePopupAccuseCrime::OnEntranceStart( void * pArg1 , void * pArg2 )
{
	if( UIBattlePopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	CrosshairMgr::i()->BackupEnable();
	
	m_UiType = (CRIME_UI_TYPE)(INT32)pArg1;

	return true;
}

void UIBattlePopupAccuseCrime::OnLoadAllScenes()
{
	UIBattlePopupBase::OnLoadAllScenes();

	m_Reason = FORCEREMOVE_CURSE;
	m_Selected_Index = UINT_MAX;

	LuaState * L = 0;
	L = _CallLuaFunction("SetValue");
	i3Lua::PushInteger( L, m_Reason); //reason
	i3Lua::PushInteger( L, m_Vote_Type); //vote
	_EndLuaFunction( L, 2);

	UserList UserList;
	hu::get_ingame_user_list(UserList, false);
	hu::partition_ingame_user_list(UserList, m_LeftUserList, m_RightUserList);

	i3UIScene* pScene = GetScene(0);
	inner::to_lua_user_list(pScene, "Set_Left_Player", m_LeftUserList);
	inner::to_lua_user_list(pScene, "Set_Right_Player", m_RightUserList);
}

void UIBattlePopupAccuseCrime::OnEntranceEnd()
{
	UIBattlePopupBase::OnEntranceEnd();

	LuaState*L = _CallLuaFunction("EntranceEnd");
	UILua::EndLuaFunction(L , 0);
}

void	 UIBattlePopupAccuseCrime::OnExitEnd( void)
{
	CrosshairMgr::i()->RestoreEnable();

	UIBattlePopupBase::OnExitEnd();
}



/************************/
bool UIBattlePopupAccuseCrime::OnKey_Enter( void)
{
	if( m_Selected_Index != UINT_MAX )
		click_ok();
	else
		click_cancel();

	return true;
}

void UIBattlePopupAccuseCrime::check_ok_btn()
{
	//�÷��̾� ������ �� �Ǿ����� ��� �Ű� ��ư�� ���´�.
	bool disable_btn = (m_Selected_Index == UINT_MAX);

	LuaState* L = _CallLuaFunction("InputDisableOkBtn");
	i3Lua::PushBoolean( L, disable_btn);
	_EndLuaFunction( L, 1);
}

bool UIBattlePopupAccuseCrime::click_ok()
{
	if( get_user_list_size() < 2 )
	{
		/*��ȿ� ��ǥ �ο��� 2�� ���϶� ������ �� �����ϴ�.*/
		g_pFramework->PutSystemChatting( GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_NUMPLAYERS") );
		return true;
	}

	//���õ� slot�� ������ ���� ������ ������ ���Ѵ�. (�׵��� ������ ��ų�, �ٸ� ����� ä������ �ִ�.)
	const i3::pair<INT32, i3::rc_wstring>& user_info = get_selected_user_info();

	//INT32 selected_slot = user_info.first;
	//i3::rc_wstring wstrNick_From_Slot = BattleSlotContext::i()->getNickForSlot( selected_slot );

	// 12920 : BattleSlotContext�� SlotIndex�� ��ǥ����Ʈ���� ��Ÿ���� SlotIndex�� ����Ʈ�� ���� ������ �����ϱ� ������ �г����� ���� ������ �޾ƿ�.
	INT32 real_slot_index = BattleSlotContext::i()->GetUserIndex(user_info.second);
	const SLOT_INFO* pSlotInfo = BattleSlotContext::i()->getSlotInfo(real_slot_index);
	
	// ���� ������ �ƿ� ���ų�, ��Ʋ���� �ƴ� ������ ���� ��ǥ ����� �ƴ�.
	if ( pSlotInfo == nullptr || (pSlotInfo != nullptr && pSlotInfo->_State != SLOT_STATE_BATTLE))
	{
		i3::rc_wstring wstrMsg;
		i3::sprintf( wstrMsg, GAME_RCSTRING("STR_MSG_FORCIBLYREMOVE_EARLY_CANCEL_NOUSER"), GetSelected_NickName() );
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMsg, &GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_SUBJECT") );
		return true;		// �׳� �ݾƹ�����... 
	}

	if(CGameMaster::i()->IsPaused())
	{
		return true;
	}

	GameEventSender::i()->SetEventOnce( EVENT_KICK_VOTE_START, &real_slot_index, &m_Reason);
	
	return click_cancel();
}

bool UIBattlePopupAccuseCrime::click_cancel()
{
	UIBattleFrame::i()->ClosePopup( UBP_ACCUSE_CRIME );
	return true;
}

/************************/
size_t UIBattlePopupAccuseCrime::get_user_list_size() const
{
	return m_LeftUserList.size() + m_RightUserList.size();
}

const i3::rc_wstring& UIBattlePopupAccuseCrime::GetSelected_NickName() const
{
	return get_selected_user_info().second;
}

const i3::pair<INT32, i3::rc_wstring>& UIBattlePopupAccuseCrime::get_selected_user_info() const
{
	bool is_right = (m_Selected_Index % 2) ? true : false; //¦������ �Ǵ�.

	if( is_right == true )
	{
		return m_RightUserList[ m_Selected_Index/2 ];
	}

	return m_LeftUserList[ m_Selected_Index/2 ];
}

//- inner -------------------------------------//
//�ܼ� �ڵ� ���̱� ���� �뵵
void inner::to_lua_user_list(i3UIScene* pScene, const char* lua_name, 
							 const i3::vector< i3::pair<INT32, i3::rc_wstring> > & list)
{
	enum{ MAX_SINGLE_SLOT = (SLOT_MAX_COUNT>>1) };
	LuaState* L = UILua::CallLuaFunction(pScene, lua_name);

	size_t size = list.size();
	for(size_t i=0;i<MAX_SINGLE_SLOT;++i)
	{
		if( i < size )
		{
			const i3::rc_wstring & user_name = list[i].second;
			i3Lua::PushStringUTF16To8( L, user_name ); //cause
		}
		else
		{
			i3Lua::PushStringUTF16To8( L, L""); //cause
		}
	}
	UILua::EndLuaFunction(L , MAX_SINGLE_SLOT);
}

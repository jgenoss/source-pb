#include "pch.h"
#include "UIBattlePopupForciblyRemove.h"

#include "UIBattledef.h"

#include "UIBattleFrame.h"
#include "UIHUDForciblyRemove.h"
#include "UIUtil.h"

//#include "StageBattle.h"
#include "CrosshairMgr.h"


static UIBattlePopupForciblyRemove * GetThis( void)
{
#if ���_�������
	UIBattleFrame * pFrame = UIBattleFrame::i();
	if( pFrame != NULL)
		return (UIBattlePopupForciblyRemove*) pFrame->GetPopup( UBP_FORCIBLYREMOVE);
#endif
	return NULL;
}


UIBattlePopupForciblyRemove* UIBattlePopupForciblyRemove::instance()
{
	return GetThis();
}

//--------------------------------------------------------------------------//
//								Lua Glue Function							//
//--------------------------------------------------------------------------//
extern "C" {
	int UIBattlePopupForciblyRemove_SelectUser( LuaState * L)
	{	
		INT32 slot = i3Lua::GetIntegerArg( L, 1);

		UIBattlePopupForciblyRemove * pThis = GetThis();
		if( pThis != NULL)
		{
			i3Lua::PushBoolean(L, pThis->SelectUser( slot));
			return 1;
		}

		return 0;
	}

	int UIBattlePopupForciblyRemove_SelectReason( LuaState * L)
	{
		INT32 reason = i3Lua::GetIntegerArg( L, 1);

		UIBattlePopupForciblyRemove * pThis = GetThis();
		if( pThis != NULL)
			pThis->SelectReason( static_cast<FORCEREMOVE_CAUSE>(reason));
		return 0;
	}
}

LuaFuncReg UIBattlePopupForciblyRemove_Glue[] = {
	{"SelectUser",					UIBattlePopupForciblyRemove_SelectUser	},
	{"SelectReason",				UIBattlePopupForciblyRemove_SelectReason	},
	{NULL,							NULL}
};

//--------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIBattlePopupForciblyRemove);//, UIBattlePopupBase);

UIBattlePopupForciblyRemove::UIBattlePopupForciblyRemove() : m_bPrev_CrossHairState(TRUE)
{
	m_SelectedUserIdx = -1;
	m_SelectedListboxIdx = -1;
	m_Reason = FORCEREMOVE_NONE;
}

UIBattlePopupForciblyRemove::~UIBattlePopupForciblyRemove()
{
	
}

void	UIBattlePopupForciblyRemove::_ClearUserNameList()
{
	m_UserNameList.clear();
}

void UIBattlePopupForciblyRemove::_Init( void)
{
	m_SelectedListboxIdx = -1;
	m_SelectedUserIdx = -1;

	m_Reason = FORCEREMOVE_CURSE;

	m_UserList.clear();
	_ClearUserNameList();

	int odd_even = g_pGameContext->getMainSlotIdx() % 2;
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const char*  szNick = g_pGameContext->getNickForSlot(i);
		if( szNick == 0 ) continue;

		SLOT_INFO* pInfo = g_pGameContext->getSlotInfo(i);
		if( pInfo == 0 ) continue;

		bool add_user = false;
		if( pInfo->_State == SLOT_STATE_BATTLE && pInfo->_Rank != RANK_GAMEMASTER )
		{
			if( g_pGameContext->getMainSlotIdx() != i ) //������ ������ ��� ��� �߰�
			{
				add_user = true;
				if( g_pGameContext->IsStageMaskTraining() && odd_even != i  % 2)	//ç���� ���� ���ΰ� �ٸ� ���� AI �̴�.
					add_user = false;
			}
		}

		if( add_user )
		{
			m_UserList.push_back(i);
			m_UserNameList.push_back(szNick);
		}
	}

	LuaState * L = 0;

	L = _CallLuaFunction( "SetSlotCount");
	i3Lua::PushInteger( L, (INT32)m_UserList.size());
	_EndLuaFunction( L, 1);

	for( size_t i = 0; i < m_UserList.size(); i++)
	{
		L = _CallLuaFunction( "SetSlotInfo");
		i3Lua::PushInteger( L, i);
		i3Lua::PushA2UTF8String( L,  m_UserNameList[i].c_str());
		_EndLuaFunction( L, 2);
	}
	
	char szComboList[ MAX_STRING_COUNT] = "";
	i3String::Format( szComboList, sizeof( szComboList), "%s;%s;%s;%s",
		GAME_STRING("STR_POPUP_FORCIBLYREMOVE_REASON_NOMANNER"),
		GAME_STRING("STR_POPUP_FORCIBLYREMOVE_REASON_ILLEGALPROGRAM"),
		GAME_STRING("STR_POPUP_FORCIBLYREMOVE_REASON_ABUSE"),
		GAME_STRING("STR_POPUP_FORCIBLYREMOVE_REASON_ETC") );

	L = _CallLuaFunction( "SetComboList");
	i3Lua::PushA2UTF8String( L, szComboList);
	_EndLuaFunction( L, 1);
}

/*virtual*/ void UIBattlePopupForciblyRemove::OnCreate( i3GameNode * pParent)
{
	UIBattlePopupBase::OnCreate( pParent);

	AddScene( "UIRe/PBRe_ForceExit.i3UIe", UIBattlePopupForciblyRemove_Glue, true, false);
}

/*virtual*/ bool UIBattlePopupForciblyRemove::OnKey_Escape( void)
{
#if ���_�������
	UIBattleFrame * pFrame = UIBattleFrame::i();
	I3ASSERT( pFrame);

	if( isEnable())
	{
		pFrame->TogglePopup( UBP_FORCIBLYREMOVE);
		return true;
	}
#endif

	return false;
}


bool UIBattlePopupForciblyRemove::OnEntranceStart( void * pArg1 , void * pArg2 )
{
	if( UIBattlePopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	// �̰� ��� ���̽�Ŭ������ UIBattleFrame��ۿ��� ó���� �����ϱ� �ѵ�.11.05.26.����.
	if(g_pFramework == NULL)	return false;

	m_bPrev_CrossHairState = CrosshairMgr::i()->isEnableCrossHair();
	CrosshairMgr::i()->enableCrossHair(FALSE); 

	_Init();

	return true;
}

void	 UIBattlePopupForciblyRemove::OnExitEnd( void)
{
	if(g_pFramework == NULL)
		return;

	CrosshairMgr::i()->enableCrossHair(m_bPrev_CrossHairState != FALSE);
	UIBattlePopupBase::OnExitEnd();
}


bool UIBattlePopupForciblyRemove::SelectUser( INT32 idx)
{
//	I3_BOUNDCHK( idx, m_UserList.GetCount() );
	if (idx >= (INT32)m_UserList.size() )
	{
		m_SelectedUserIdx = -1;
		m_SelectedListboxIdx = -1;
		return false;
	}

	m_SelectedListboxIdx = idx;
	m_SelectedUserIdx = m_UserList[ idx];
	I3_BOUNDCHK( m_SelectedUserIdx, SLOT_MAX_COUNT);
	return true;
	
}

void UIBattlePopupForciblyRemove::SelectReason( FORCEREMOVE_CAUSE cause)
{
	I3_BOUNDCHK( cause, FORCEREMOVE_MAX);

	m_Reason = cause;
}

bool UIBattlePopupForciblyRemove::OnOKButton( void)
{
	
	// ���� ���� ó���� �ƹ��͵� ���������� ������ OK��ư�� disable��Ű��, �������� ������ enable��Ű��,
	// �������� �𼿷�Ʈ�ɶ� �ٽ� OK��ư�� disable��Ű�� ����� ���ƺ��Դϴ�....(11.06.02.����)
	// (�̷� �κ��� ����ϰ� ó���Ϸ���, �ش�UIŬ������ �̺�Ʈ�Լ� 2���� ����� ���� ���ƺ���)

	// �� �ܿ� ���ǻ�������, �� Ŭ���� �����߿� ���������� ������, �ٸ� ������ �� ������ ������ ��� ó���� �ֽ��ϴ�. (���� ��ó��)
	// �� ���, �ǽð����� ����� �����ϴ� �δ��� �������� �ְ�, OKó�� ������ ������ ���� �����
	// �г��� ���ϴ� �δ��� ������ �־� ������Դϴ�...(�������� �� ����..)

// �켱��, ���� ������ ���� �ʴ� �̻�, ���������� �������� ���� ������ �����ϰ�, �ƹ��͵� ���� �ʽ��ϴ�.(11.06.02.����)
	if (m_SelectedUserIdx == -1 || m_SelectedListboxIdx == -1) 
	{
		// 11.06.02.����.todo
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_MSG_FORCIBLYREMOVE_SELECT_NOUSER"), GAME_STRING("STR_HUD_FORCIBLYREMOVE_SUBJECT") );
		return true;		// �׳� �ݾƹ�����... 
	}

	// ������ ���������� ��û�Ѵ�.

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( g_pConfig->m_bNetworking == FALSE)
	{
#if ���_�������
		UIBattleFrame::i()->EnableHUD( UIHUD_FORCIBLYREMOVE, true);
#endif
		return true;
	}
#endif

	// �����ǵ� �ε��� �̸��� ������ �г����� ������ ���ؾ��մϴ�...	(�׵��� ������ ��ų�, �ٸ� ����� ä������ �ִ�.)
	const char* szNick_From_Slot = g_pGameContext->getNickForSlot( m_SelectedUserIdx );
	const i3::rc_string& szNick_From_Listbox = m_UserNameList[ m_SelectedListboxIdx];

	if ( szNick_From_Listbox != szNick_From_Slot )
	{
		char strMsg[512];
		StringCchPrintf( strMsg, 512, 
			GAME_STRING("STR_MSG_FORCIBLYREMOVE_EARLY_CANCEL_NOUSER"), szNick_From_Listbox);
		GameUI::MsgBox( MSG_TYPE_GAME_OK, strMsg, GAME_STRING("STR_HUD_FORCIBLYREMOVE_SUBJECT") );
		return true;		// �׳� �ݾƹ�����... 
	}

	g_pGameContext->SetEventOnce( EVENT_KICK_VOTE_START, &m_SelectedUserIdx, &m_Reason);

	// ���� ���� ���ο� ������� �������� ������ ������ HUD�� �ѱ�ϴ�...
	// �������� ���, ��Ŷ���κ��� �г��Ӱ� ���� ������ ������ϴ�.
	// �� â�� ��Ȱ���� ���̹Ƿ�, ���� ����������� HUD�� ��������� �����ؼ� �ѱ�ϴ�.
//	UIHUDForciblyRemove* pHUD = static_cast<UIHUDForciblyRemove*>(UIBattleFrame::i()->GetHUD(UIHUD_FORCIBLYREMOVE));
//	pHUD->SetInfo_BySuggest( szNick_From_Slot, m_Reason);

	return true;
}

bool	UIBattlePopupForciblyRemove::OnCloseButton()
{
	return UIBattlePopupBase::OnCloseButton();
}



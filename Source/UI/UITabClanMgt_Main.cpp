#include "pch.h"
#include "UITabClanMgt_Main.h"
#include "UIMainFrame.h"
#include "UIPhaseClanMember.h"
#include "ClanDef.h"
#include "../QuestTextDictionary.h"
#include "../CommunityContext.h"
#include "ClanContext.h"

#include "UIUtil.h"

I3_CLASS_INSTANCE( UITabClanMgt_Main, UITabClan_Mgt);

static UITabClanMgt_Main * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	UIPhaseBase* pPhase = pFrame->GetCurrentSubPhase();
	UITabClan* pClan = static_cast<UIPhaseClanMember*>(pPhase)->GetCurrTab();
	return static_cast<UITabClanMgt_Main*>(pClan);

}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UITabClanMgt_Modify_Notice(LuaState * L)
	{
		const char* sznotice = i3Lua::GetStringArg(L, 1, NULL, 0);
		char Notice[NET_CLAN_INTRO_SIZE];
		StringCchCopyA(Notice, NET_CLAN_INTRO_SIZE,sznotice);
		Notice[NET_CLAN_INTRO_SIZE - 1] = 0;

		GetThis()->OnClanManagementBoardNoticeModify(Notice);
		return 0;
	}
	int UITabClanMgt_Modify_Intro(LuaState * L)
	{
		const char* szintro = i3Lua::GetStringArg(L, 1, NULL, 0);
		char Intro[NET_CLAN_INTRO_SIZE];
		StringCchCopyA(Intro, NET_CLAN_INTRO_SIZE,szintro);
		Intro[NET_CLAN_INTRO_SIZE - 1] = 0;

//		i3Lua::GetStringArg(L, 1, Intro, MAX_STRING_COUNT);
		GetThis()->OnClanManagementBoardGuideModify(Intro);
		return 0;
	}
	int UITabClanMgt_Modify_SendMail(LuaState * L)
	{
		GetThis()->OnClanSendMessage();;
		return 0;
	}
	int UITabClanMgt_Modify_NameChange(LuaState * L)
	{
		GetThis()->OnClanNameChange();
		return 0;
	}
	int UITabClanMgt_Modify_MarkChange(LuaState * L)
	{
		GetThis()->OnClanMarkChange();
		return 0;
	}
	int UITabClanMgt_Modify_Disperse(LuaState * L)
	{
		GetThis()->OnClanLeave();
		return 0;
	}
	int UITabClanMgt_Modify_NameItem(LuaState * L)
	{
		
		return 0;
	}
	int UITabClanMgt_Modify_MarkItem(LuaState * L)
	{
		
		return 0;
	}

	
}

LuaFuncReg UITabClanMgt_Main_Glue[] = {
	{"UITabClanMgt_Modify_Notice",			UITabClanMgt_Modify_Notice		},
	{"UITabClanMgt_Modify_Intro",			UITabClanMgt_Modify_Intro		},
	{"UITabClanMgt_Modify_SendMail",		UITabClanMgt_Modify_SendMail	},
	{"UITabClanMgt_Modify_NameChange",		UITabClanMgt_Modify_NameChange	},
	{"UITabClanMgt_Modify_MarkChange",		UITabClanMgt_Modify_MarkChange	},
	{"UITabClanMgt_Modify_Disperse",		UITabClanMgt_Modify_Disperse	},
	{"UITabClanMgt_Modify_NameItem",		UITabClanMgt_Modify_NameItem	},
	{"UITabClanMgt_Modify_MarkItem",		UITabClanMgt_Modify_MarkItem	},
	{NULL,						NULL}
};

//------------------------------------------------------------------------------//


UITabClanMgt_Main::UITabClanMgt_Main()
{
	m_pClanContext = g_pFramework->GetClanContext();
	memset( m_szInputheck, 0, sizeof( m_szInputheck ) );
	m_pCheckEditbox = false;
}

UITabClanMgt_Main::~UITabClanMgt_Main()
{

}

void UITabClanMgt_Main::OnClanManagementBoardNoticeModify(char* Notice)
{
	if( FALSE == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_WRITE ) ) 
	{
		//	"�ش� ��ɿ� ���� ������ ������ �����Ǿ� ���� �ʽ��ϴ�."
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));
		_InitNotice();
		return;
	}
	if(g_pFramework->IsFilter(Notice) == FALSE)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
		return;
	}
	if( strcmp( Notice, m_pClanContext->GetMyClanDetailInfo()->m_announce ) != 0 )
	{
		m_pClanContext->BoardNoticeModify( Notice );
		i3String::Copy( m_pClanContext->GetMyClanDetailInfo()->m_announce, Notice, NET_CLAN_INTRO_SIZE );
		//GameUI::MsgBox( MSG_TYPE_GAME_OK,  "���������� �����Ǿ����ϴ�." );
	}
}

void UITabClanMgt_Main::OnClanManagementBoardGuideModify(char* Intro)
{
	if( FALSE == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_WRITE ) ) 
	{
		//	"�ش� ��ɿ� ���� ������ ������ �����Ǿ� ���� �ʽ��ϴ�."
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));
		_InitIntro();
		return;
	}
	if( i3String::Length(Intro) == 0 )
	{
		//	"Ŭ�� �Ұ��� �ùٸ��� �ʽ��ϴ�.\n�ٽ� Ȯ���� �ֽʽÿ�."
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STBL_IDX_CLAN_WRONG_CLAN_GUIDE") );
		return;
	}
	if(g_pFramework->IsFilter(Intro) == FALSE)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
		return;
	}

	//	Ŭ���Ұ� ���� �Ϸ�
	if( strcmp( Intro, m_pClanContext->GetMyClanDetailInfo()->m_intro ) != 0 )
	{
		m_pClanContext->BoardGuideModify(Intro);
		i3String::Copy( m_pClanContext->GetMyClanDetailInfo()->m_intro, Intro, NET_CLAN_INTRO_SIZE);		
	}
}

void UITabClanMgt_Main::OnClanNameChange()
{
	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"������ ������ �ʿ��մϴ�."
		return;
	}
	//	���� ĳ�� �������� ��� �� ������ �κ��丮���� ó���ϸ� Ŭ���� ���� �����۰� Ŭ�� ��ũ ���� �����۸� Ŭ�� ���� ���������� �Ҽ� �����Ƿ� ����ó���� �ʿ��ϴ�. komet
	INT32 ItemID = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_WRAP_COUNT, CASHITEM_GROUP_CHANGE_CLAN_NAME, CASHITEM_DAY_NO);

	if( g_pFramework->IsUsingCashItem( ItemID))
	{
		m_authItemFlag = ItemID;
		UIMainFrame *pFrame = g_pFramework->GetUIMainframe();
		pFrame->TogglePopup( UPW_CHANGE_CLAN_NAME, &ItemID);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING( "STR_REQUEST_BUY_ITEM"));
	}
}

void UITabClanMgt_Main::OnClanMarkChange()
{
	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"������ ������ �ʿ��մϴ�."
		return;
	}
#if defined(USE_CLAN_MARK)
	//	���� ĳ�� �������� ��� �� ������ �κ��丮���� ó���ϸ� Ŭ���� ���� �����۰� Ŭ�� ��ũ ���� �����۸� Ŭ�� ���� ���������� �Ҽ� �����Ƿ� ����ó���� �ʿ��ϴ�. komet
	INT32 ItemID = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_WRAP_COUNT, CASHITEM_GROUP_CHANGE_CLAN_MARK, CASHITEM_DAY_NO);

	if( g_pFramework->IsUsingCashItem( ItemID))
	{
		m_authItemFlag = ItemID;
		UIMainFrame *pFrame = g_pFramework->GetUIMainframe();
		pFrame->TogglePopup( UPW_QUERY_USEITEM, &ItemID);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING( "STR_REQUEST_BUY_ITEM"));	// "�������� �����ϼžߵ˴ϴ�."
	}
#else
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_PREPARE_PLAN"));	//	"���Ŀ� ������ �����Դϴ�."
	return;
#endif
}

void UITabClanMgt_Main::OnClanSendMessage()
{
	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"������ ������ �ʿ��մϴ�."
		return;
	}
	if ( CanClanMemberSendNoteAll() )
	{
		UIMainFrame *pFrame = g_pFramework->GetUIMainframe();
		pFrame->TogglePopup( UPW_CLAN_SENDMESSAGE);
	}
	else
	{
		//���Ŀ� ������ �����Դϴ�.
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_PREPARE_PLAN"));
	}
}

void UITabClanMgt_Main::OnClanLeave()
{
	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"������ ������ �ʿ��մϴ�."
		return;
	}
	g_pFramework->GetClanContext()->ProcessDissolutionClan();
}

void UITabClanMgt_Main::_InitNotice()
{
	LuaState * L = _CallLuaFunction("SetClanNotice");
	i3Lua::PushA2UTF8String(L, m_pClanContext->GetMyClanDetailInfo()->m_announce);
	_EndLuaFunction(L, 1);
	
	StringCchCopyA(m_szInputheck[0] , MAX_STRING_COUNT, m_pClanContext->GetMyClanDetailInfo()->m_announce);
}

void UITabClanMgt_Main::_InitIntro()
{
	LuaState * L = _CallLuaFunction("SetClanIntro");
	i3Lua::PushA2UTF8String(L, m_pClanContext->GetMyClanDetailInfo()->m_intro);
	_EndLuaFunction(L, 1);
	StringCchCopyA(m_szInputheck[1] , MAX_STRING_COUNT, m_pClanContext->GetMyClanDetailInfo()->m_announce);
}

void UITabClanMgt_Main::_InitUI()
{
	LuaState * L = _CallLuaFunction("InitUI");
	_EndLuaFunction(L, 0);
}

void UITabClanMgt_Main::_InitAuthority()
{
	LuaState * L = _CallLuaFunction("InitAuthority");
	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER )
		i3Lua::PushBoolean(L, true);
	else
		i3Lua::PushBoolean(L, false);
	_EndLuaFunction(L,1);
}

/*virtual*/ void UITabClanMgt_Main::OnCreate(i3GameNode * pParent)
{
	base::OnCreate(pParent);
	//LoadScene
	char * pszPath = "UIRe/PBRe_ClanMgt_Main.i3UIe";
	AddScene( pszPath,UITabClanMgt_Main_Glue);

}

/*virtual*/ void UITabClanMgt_Main::OnLoadAllScenes()
{
	base::OnLoadAllScenes();
	_InitNotice();
	_InitIntro();
	_InitUI();
	_InitAuthority();
}

/*virtual*/ void UITabClanMgt_Main::OnUnloadAllScenes()
{
	base::OnUnloadAllScenes();
}

/*virtual*/ void UITabClanMgt_Main::ProcessGameEvent(INT32 event, INT32 arg)
{
	base::ProcessGameEvent(event, arg);

}

/*virtual*/ void UITabClanMgt_Main::OnExitEnd()
{
	base::OnExitEnd();
}

/*virtual*/ bool UITabClanMgt_Main::OnEntranceStart( void * pArg1, void * pArg2)
{
	if (base::OnEntranceStart(pArg1, pArg2) == false ) return false;
	m_pCheckEditbox = true;

	return true;
}

/*virtual*/ void UITabClanMgt_Main::OnEntranceEnd()
{
	base::OnEntranceEnd();
	m_pCheckEditbox = false;
	
}

void UITabClanMgt_Main::_CheckIMELength( void)
{
	for(INT32 i= 0; i < 2; i++)
	{
		if( m_pEdInputCheck[i] == NULL)
			continue;

		m_pEdInputCheck[i]->OnSetFocus();

		if( m_pEdInputCheck[i]->GetIME() == NULL )
			continue;
			

		const WCHAR16* wcsIMETextBuffer = m_pEdInputCheck[i]->getIMETextBuf();

		int writtenByte = WideCharToMultiByte( GetCodePage(), 0, wcsIMETextBuffer, wcslen(wcsIMETextBuffer), NULL, 0, NULL, NULL );

		if( writtenByte >= NET_CLAN_INTRO_SIZE )
		{
			WCHAR16 wcsbuffer[MAX_STRING_COUNT] = {0,};
			MultiByteToWideChar( GetCodePage (), 0, m_szInputheck[i], strlen(m_szInputheck[i]), wcsbuffer, MAX_STRING_COUNT );	

			m_pEdInputCheck[i]->GetIME()->SetInputFinish();

			m_pEdInputCheck[i]->SetIMEText( wcsbuffer );
		}
		else
		{
			memset( m_szInputheck[i], 0, sizeof(char)*MAX_STRING_COUNT );
			WideCharToMultiByte( GetCodePage(), 0, wcsIMETextBuffer, wcslen(wcsIMETextBuffer), m_szInputheck[i], MAX_STRING_COUNT, NULL, NULL );
		}
	}
}

/*virtual*/ void UITabClanMgt_Main::_InitializeAtLoad( i3UIScene * pScene)
{
	UITabClan_Mgt::_InitializeAtLoad( pScene);

	m_pEdInputCheck[0] = (i3UIEditBox*) pScene->FindChildByName( "NoticeEditBox");
	I3ASSERT( m_pEdInputCheck[0]);

	m_pEdInputCheck[1] = (i3UIEditBox*) pScene->FindChildByName( "IntroEditBox");
	I3ASSERT( m_pEdInputCheck[1]);
}

/*virtual*/ bool	UITabClanMgt_Main::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	UITabClan_Mgt::OnKeyInput( pInputMng);
	
	return true;
}

/*virtual*/ void	UITabClanMgt_Main::OnUpdate( REAL32 rDeltaSeconds)
{
	if(m_pCheckEditbox)
		_CheckIMELength();
	UITabClan_Mgt::OnUpdate( rDeltaSeconds);
}

	
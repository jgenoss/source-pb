#include "pch.h"

#if legacy_gui_function
#include "ContextMenu.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE(CContextMenu);//, CGuiNotifyReceiver);

CContextMenu::CContextMenu() : m_commandCount(0)
{
	m_pContextGui = nullptr;
	m_pList = nullptr;
	m_pObserver = nullptr;
	m_requestId = 0;

	for(INT32 i = 0; i < CMC_MAX; i++)
	{
		m_commandMap[i] = CMC_INIT;
		m_commandText[i][0] = nullptr;
	}
}

CContextMenu::~CContextMenu()
{
	/* Do nothing */
}

void CContextMenu::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_CM_L_LIST:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) _OnCommand(pNotify->m_nPriParam);
		break;
	default:
		/* Do nothing */
		break;
	}
}

bool CContextMenu::OnLinkControl(const i3::vector_set<STRCINFO*>* plist, i3GameNode * pControl)
{
	LinkControlEx(*plist, "ContextMenu", m_pContextGui, -1);
	LinkControlEx(*plist, "ctList", m_pList, GCI_CM_L_LIST);

	return false;
}

void CContextMenu::OnInitControl(void)
{
	CGuiNotifyReceiver::OnInitControl();

	m_pContextGui->SetEnable(false);
	
	m_pList->CreateTextEx(CMC_MAX,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pList->SetSelColor(100, 100, 100, 100);
	m_pList->SetTextColor(GameGUI::GetColor(GCT_FOCUS));
	m_pList->SetExclusiveEnableControl(true);
}

void CContextMenu::OnUpdate(REAL32 rDeltaSeconds)
{
	CGuiNotifyReceiver::OnUpdate(rDeltaSeconds);

	// m_pList�� ��Ŀ���� ������, m_pContextGui�� �Բ� ��������Ѵ�.
	if (false == m_pList->isEnable())
	{
		m_pContextGui->SetEnable(false);
	}
}

void CContextMenu::Reset(void)
{
	m_commandCount = 0;
	m_pObserver = nullptr;
	m_pContextGui->SetEnable(false);
}

void CContextMenu::Open(i3EventReceiver* observer,INT32 requestId)
{
	if (0 == m_commandCount) return;
	if (nullptr == observer) return;

	m_pObserver = observer;
	m_requestId = requestId;


	m_pList->Clear();
	m_pList->SetDisplayCount(m_commandCount);
	
	for(UINT32 i = 0; i < m_commandCount; i++)
	{
 		m_pList->AddElement(m_commandText[i]);
	}

	ExclusionPopup(m_pList); 

	REAL32 fScreenHeightRate = i3GuiRoot::getGuiResolutionHeight();
	REAL32 fScreenWidthRate = i3GuiRoot::getGuiResolutionWidth();

	i3InputMouse* mouse = g_pFramework->getMouse();
	REAL32 x = mouse->GetX() / i3GuiRoot::getGuiResolutionWidth();
	REAL32 y = mouse->GetY() / i3GuiRoot::getGuiResolutionHeight();

	INT32 FrameOffset = (INT32)(14.f * fScreenHeightRate);	
	INT32 w = (INT32)((REAL32)(m_pContextGui->getOriginalWidth()) * fScreenWidthRate); 
	INT32 h = (m_pList->GetTextHeight() * m_commandCount) + FrameOffset;

	m_pContextGui->SetControlSize(w, h);
	m_pContextGui->setPosition(x, y);	
	m_pContextGui->SetEnable(true);

	//	������¡�� ����ǵ��� �ٽ� �ѹ� ������Ʈ �ϵ��� �Ѵ�. �̰� ���� ������ ���������� ������¡�� ����˴ϴ�.
	FLUSH_CONTROL(m_pContextGui);
}

void CContextMenu::AddCommand(ContextMenuCommand command)
{
	I3ASSERT(m_commandCount < CMC_MAX);

    char commandText[MAX_STRING_COUNT] = {0, };

	switch(command)
	{
	case CMC_TRANSFER_HOST:		i3::safe_string_copy(commandText, GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_GIVE_MASTER"), MAX_STRING_COUNT );		break;	// ����ѱ��
	case CMC_KICK:				i3::safe_string_copy(commandText, GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_FORCE_GET_AWAY"), MAX_STRING_COUNT );	break;	// ��������
	case CMC_PROFILE:			i3::safe_string_copy(commandText, GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_PERSONAL_INFO"), MAX_STRING_COUNT );	break;	// ��������
	case CMC_WHISPER:			i3::safe_string_copy(commandText, GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_WHISPER"), MAX_STRING_COUNT );			break;	// �ӼӸ�
	case CMC_INVITE:			i3::safe_string_copy(commandText, GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_INVITE"), MAX_STRING_COUNT );			break;	// �ʴ��ϱ�
	case CMC_VISIT:				i3::safe_string_copy(commandText, GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_FIND_OUT"), MAX_STRING_COUNT );		break;	// ã�ư���
	case CMC_ADD_FRIEND:		i3::safe_string_copy(commandText, GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_ADD_FRIEND"), MAX_STRING_COUNT );		break;	// ģ���߰�
	case CMC_ADD_BLOCK:			i3::safe_string_copy(commandText, GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_CUT_OFF"), MAX_STRING_COUNT );			break;	// �����ϱ�
	case CMC_CLAN_INVITE:		i3::safe_string_copy(commandText, GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_CLAN_INVITE"), MAX_STRING_COUNT );		break;	// Ŭ���ʴ�
	case CMC_VIEW_DETAIL:		i3::safe_string_copy(commandText, GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_DETAIL_INFO"), MAX_STRING_COUNT );		break;	// ������
	case CMC_VIEW_OTHER_USER_ITEM_INFO:	i3::safe_string_copy(commandText, GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_VIEW_ITEM_INFO"), MAX_STRING_COUNT );	break;	// ������
	default:
		I3ASSERT_0;
		break;
	}

	i3::safe_string_copy( m_commandText[m_commandCount], commandText, MAX_STRING_COUNT );
	m_commandMap[m_commandCount] = command;
	m_commandCount++;
}

void CContextMenu::_OnCommand(INT32 commandIdx)
{
	if (commandIdx < (signed)m_commandCount)
	{
		if (m_pObserver)
		{
			m_pObserver->OnEvent(m_commandMap[commandIdx], this, m_requestId, I3_EVT_CODE_ACTIVATE);
		}
	}
}
#endif
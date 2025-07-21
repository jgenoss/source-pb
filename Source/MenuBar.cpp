#include "pch.h"
#include "MenuBar.h"
#include "StageReady.h"
#include "GlobalVariables.h"
#include "MessageBox.h"

I3_CLASS_INSTANCE(CMenuBar, CGuiNotifyReceiver);

char CMenuBar::m_strAnnounceBuffer[MAX_STRING_COUNT];
INT32 CMenuBar::m_idxAnnounceLetter = -1;
char CMenuBar::m_strAnnounceBuffer2[MAX_STRING_COUNT];
BOOL CMenuBar::m_bFCMFlag = FALSE;
BOOL CMenuBar::m_bFlowFlag = FALSE;

CMenuBar::CMenuBar()
{
	m_pMenuBar = NULL;
	m_pInfoBar = NULL;
	m_pClan = NULL;
	m_pProfile = NULL;
	m_pInventory = NULL;
	m_pShop = NULL;
	m_pCommunity = NULL;
	m_pOption = NULL;
	m_pExit = NULL;
	m_pNickname = NULL;
	m_pExp = NULL;
	m_pPinoKey = NULL;
	m_pPinoValue = NULL;
	m_pNcCoinKey = NULL;
	m_pNcCoinValue = NULL;
	m_pRank = NULL;
	m_pExpBar = NULL;
	m_pExpGage = NULL;
	m_pESport = NULL;
	m_pExpText = NULL;
	m_pAnnounce = NULL;
	m_pCalculateText = NULL;

	m_bAlarmCommunity = FALSE;
	m_bAlarmFlag = FALSE;
	m_rAlarmTerm = 0.0f;

	m_pMessageBox = NULL;
	m_bRoomMsterKickAlarmFlag = FALSE;
	m_nCurrentCountDown = 0;
	m_ExitMsgBoxOffset.x = m_ExitMsgBoxOffset.y = 0;
	m_bNickSpace = false;

	m_bFlowTime = 0.0f;

	m_FFlow = FALSE;
	m_flowState = FS_INIT;
	m_nAnnounceWidth = 0;
	m_nCurBlankLength = 0;
	m_nMaxBlankLength = 0;
	m_nMoveOffset = 0;
	m_rCurrentOffset = 0;
}

CMenuBar::~CMenuBar()
{
	if(m_pMessageBox)
	{
		g_pFramework->CloseMessageBox(m_pMessageBox);
		m_pMessageBox = NULL;
	}

	_DestroyFlowAnnounce();
}

void CMenuBar::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch( pNotify->m_nID)
	{
	case GCI_MB_B_CLAN:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_CLAN);
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnClan();
			}
		}
		break;
	case GCI_MB_B_PROFILE:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_PROFILE);	
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnProfile();
			}
		}
		break;
	case GCI_MB_B_INVENTORY:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_INVENTORY);	
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 
		{
			if( !m_bNickSpace )
			{
				OnInventory();
			}
		}
		break;
	case GCI_MB_B_SHOP:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_SHOP);	
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnShop();
			}
		}
		break;
	case GCI_MB_B_COMMUNITY:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_COMMUNITY);	
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 
		{
			if( !m_bNickSpace )
			{
				OnCommunity();
			}
		}
		break;
	case GCI_MB_B_OPTION:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_OPTION);	
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 
		{
			if( !m_bNickSpace )
			{
				OnOption();
			}
		}
		break;
	case GCI_MB_B_EXIT:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_EXIT);
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnExit();
			}
		}
		break;
	}
}

BOOL CMenuBar::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	// �޴���
	LinkControlEx(plist, "mbMenuBarBody", m_pMenuBar, -1);
	LinkControlEx(plist, "mbInfoBar", m_pInfoBar, -1);

	// �޴��� ��ư
	LinkControlEx(plist, "mbClan", m_pClan, GCI_MB_B_CLAN);
	LinkControlEx(plist, "mbProfile", m_pProfile, GCI_MB_B_PROFILE);
	LinkControlEx(plist, "mbInventory", m_pInventory, GCI_MB_B_INVENTORY);
	LinkControlEx(plist, "mbShop", m_pShop, GCI_MB_B_SHOP);
	LinkControlEx(plist, "mbCommunity", m_pCommunity, GCI_MB_B_COMMUNITY);
	LinkControlEx(plist, "mbOption", m_pOption, GCI_MB_B_OPTION);
	LinkControlEx(plist, "mbExit", m_pExit, GCI_MB_B_EXIT);

	// ������ �ؽ�Ʈ ����
    LinkControlEx(plist, "mbNickname", m_pNickname, -1);
	LinkControlEx(plist, "mbExp", m_pExp, -1);
	LinkControlEx(plist, "mbPinoKey", m_pPinoKey, -1);
	LinkControlEx(plist, "mbPinoValue", m_pPinoValue, -1);
	LinkControlEx(plist, "mbNcCoinKey", m_pNcCoinKey, -1);
	LinkControlEx(plist, "mbNcCoinValue", m_pNcCoinValue, -1);

	// ����ƽ ��ũ, ����ġ ������
	LinkControlEx(plist, "mbRankMark", m_pRank, -1);
	LinkControlEx(plist, "mbExpBar", m_pExpBar, -1);
	LinkControlEx(plist, "mbExpGage", m_pExpGage, -1);
	LinkControlEx(plist, "mbESport", m_pESport, -1);	

	LinkControlEx(plist, "mbExpText", m_pExpText, -1);
	
	// ��� ����
	LinkControlEx(plist, "mbAnnounce", m_pAnnounce, -1);	
	
	return FALSE;
}

void CMenuBar::OnInitControl(void)
{
	CGuiNotifyReceiver::OnInitControl();

	// ��ư �ʱ�ȭ
	INIT_BUTTON_EX(m_pClan);
	INIT_BUTTON_EX(m_pProfile);
	INIT_BUTTON_EX(m_pInventory);
	INIT_BUTTON_EX(m_pShop);
	INIT_BUTTON_EX(m_pCommunity);
	INIT_BUTTON_EX(m_pOption);
	INIT_BUTTON_EX(m_pExit);


	// ����Ʈ �ڽ� �ʱ�ȭ
	const UINT32 MaxEdit = 7;
	i3GuiEditBox* table[MaxEdit] = {m_pNickname, m_pExp, m_pPinoKey, 
									m_pPinoValue, m_pNcCoinKey, 
									m_pNcCoinValue, m_pExpText};

	for(UINT32 i = 0; i < MaxEdit; i++)
	{
		table[i]->SetTextColor(GetColor(GCT_INVERSE));
	}

	m_pNickname->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

	m_pExp->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPinoKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPinoValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNcCoinKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNcCoinValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);

	m_pAnnounce->SetTextColor(GetColor(GCT_NOTIFY));
	m_pAnnounce->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);	

	// ���ڿ� ������ ���� '��'�ڿ� ���� ���� 2�� �߰��߽��ϴ�.
	m_pPinoKey->SetText(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_POINT"));/*����Ʈ    :*/


#if defined( LOCALE_KOREA)
	char str[256] = "";
	i3String::Format( str, 256, "%s\n%s", GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_MONEY"), GAME_STRING("STR_TBL_GUI_HAPPY_COIN"));
	m_pNcCoinKey->SetText(str);/*N-Coin  �������Ρ���    :*/
#else
	m_pNcCoinKey->SetText(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_MONEY"));/*�硡��    :*/
#endif

	m_pExpText->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	// ����ƽ �ʱ�ȭ
	GameGUI::SetRankStatic(m_pRank);
	
	m_pExpGage->SetAllShapeEnable(FALSE);

	m_pESport->SetEnable( FALSE);
	m_pESport->SetAllShapeEnable(FALSE);


	m_pClan->SetEnable(FALSE);
	m_pInfoBar->SetEnable(FALSE);
	m_pProfile->SetEnable(FALSE);
	m_pInventory->SetEnable(FALSE);
	m_pShop->SetEnable(FALSE);
	m_pCommunity->SetEnable(FALSE);
	m_pOption->SetEnable(FALSE);


	_InitFlowAnnounce();
}

void CMenuBar::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CGuiNotifyReceiver::OnUpdate(rDeltaSeconds);

	_UpdateUserInfo();

	_UpdateFlowAnnounce(rDeltaSeconds);	

	_UpdateAlarm(rDeltaSeconds);

	_UpdateRoomMasterKickAlarm();


	UpdateButton();
}

void CMenuBar::OnClan(void)
{
	if ( ! g_pConfigEx->GetClan().Enable )
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_UNDER_CONSTRUCTION"));/*���Ŀ� ������ �����Դϴ�.*/
		return;
	}

	StepType stepType = STEP_NULL;
	BOOL bEnter = FALSE;

	if(g_pFramework->GetClanContext()->IsMember())
	{
		stepType = STEP_CLAN_MEMBER;			//	Ŭ�� ������ 

		bEnter = ((CStageReady*)m_pParent)->IsEnterClanMember();
	}
	else
	{
		stepType = STEP_CLAN_NON_MEMBER;		//	Ŭ�� �̰�����

		bEnter = ((CStageReady*)m_pParent)->IsEnterClanNonMember();
	}

	if (bEnter)
	{
		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);

#if defined TEST_CLAN_MEMBER
		((CStageReady*)m_pParent)->ChangeStep(STEP_CLAN_MEMBER);	
#else
		((CStageReady*)m_pParent)->ChangeStep(stepType);	
#endif
	}
	else
	{
		if (FALSE == i3GuiRoot::getGuiDisableInput())
		{
			((CStageReady*)m_pParent)->ExclusedEnter(stepType);
		}
	}

}

void CMenuBar::OnProfile(void)
{
	g_pGameContext->DeActiveMissionButtonEffect();
#if defined (ENABLE_MISSION_CARD)
	if (((CStageReady*)m_pParent)->IsEnterInfo())
	{
		CStageReady * pParent = ((CStageReady*)m_pParent);
		I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));

		if( g_pFramework->GetClanContext()->InBattleTeam())
		{
			if( g_pFramework->GetClanContext()->GetInClanBattleLobby())
				pParent->ChangeStep(STEP_CLAN_BATTLE_LOBBY);		//	Ŭ���� �κ� ���� ó��
			else
				pParent->ChangeStep(STEP_PROFILE);
		}
		else
		{
			pParent->ChangeStep(STEP_PROFILE);
		}

		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
	}
	else
	{
		if (FALSE == i3GuiRoot::getGuiDisableInput())
		{
			((CStageReady*)m_pParent)->ExclusedEnter(STEP_PROFILE);
		}
	}
#else
	STRING_POPUP(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_UNDER_CONSTRUCTION"));/*���Ŀ� ������ �����Դϴ�.*/
#endif
}

void CMenuBar::OnInventory(void)
{
	if (((CStageReady*)m_pParent)->IsEnterInventory())
	{
		CStageReady * pParent = ((CStageReady*)m_pParent);
		I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));

		if( g_pFramework->GetClanContext()->InBattleTeam())
		{
			if( g_pFramework->GetClanContext()->GetInClanBattleLobby())
				pParent->ChangeStep(STEP_CLAN_BATTLE_LOBBY);		//	Ŭ���� �κ� ���� ó��
			else
				pParent->ChangeStep(STEP_INVENTORY);
		}
		else
		{
			pParent->ChangeStep(STEP_INVENTORY);
		}

		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);			
	}
	else
	{
		if (FALSE == i3GuiRoot::getGuiDisableInput())
		{
			((CStageReady*)m_pParent)->ExclusedEnter(STEP_INVENTORY);			
		}
	}
}

void CMenuBar::OnShop(void)
{
	if ( ! g_pConfigEx->GetShop().Enable )
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_UNDER_CONSTRUCTION"));/*���Ŀ� ������ �����Դϴ�.*/
		return;
	}

	if (((CStageReady*)m_pParent)->IsEnterShop())
	{
		CStageReady * pParent = ((CStageReady*)m_pParent);
		I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));

		if( g_pFramework->GetClanContext()->InBattleTeam())
		{
			if( g_pFramework->GetClanContext()->GetInClanBattleLobby())
				pParent->ChangeStep(STEP_CLAN_BATTLE_LOBBY);		//	Ŭ���� �κ� ���� ó��
			else
				pParent->ChangeStep(STEP_SHOP);
		}
		else
		{
			pParent->ChangeStep(STEP_SHOP);
		}

		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);		
	}
	else
	{
		if (FALSE == i3GuiRoot::getGuiDisableInput())
		{
			((CStageReady*)m_pParent)->ExclusedEnter(STEP_SHOP);
			g_pFramework->GetShopContext()->EnterServer();	
		}
	}
}

void CMenuBar::OnCommunity(void)
{
	EndAlarmCommunity();
	((CStageReady*)m_pParent)->TogglePopup(POPUP_COMMUNITY);
}

void CMenuBar::OnOption(void)
{
	((CStageReady*)m_pParent)->TogglePopup(POPUP_OPTION);
	//((CStageReady*)m_pParent)->TogglePopup(POPUP_COLOR_SET);
	
}

void CMenuBar::OnExit(void)
{
	CMessageBox * pMessageBox = ERROR_POPUP_Q(GAME_STRING("STBL_IDX_EP_GAME_EXIT_NORMAL_QE"), EXIT_CALLBACK(CbExitNormal), this);
	POINT2D ptCenter = g_pFramework->GetCenterPosition(pMessageBox->GetWidth(), pMessageBox->GetHeight());
	ptCenter.x += m_ExitMsgBoxOffset.x;
	ptCenter.y += m_ExitMsgBoxOffset.y;
	pMessageBox->SetPosition(ptCenter.x, ptCenter.y);
}

void CMenuBar::SetInputDisable(BOOL bEnable)
{
	m_pMenuBar->setInputDisable(bEnable);
}

void CMenuBar::SetClanInputDisable(BOOL bEnable)
{
	m_pClan->setInputDisable(bEnable);
}

void CMenuBar::SetServerMode(void)
{
	m_pClan->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pProfile->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pInventory->setControlState(I3GUI_CONTROL_STATE_DISABLED);		
	m_pShop->setControlState(I3GUI_CONTROL_STATE_DISABLED);		
	m_pCommunity->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pOption->setControlState(I3GUI_CONTROL_STATE_DISABLED);
}

void CMenuBar::SetReadyMode(void)
{
	m_pInfoBar->SetEnable(TRUE);
	m_pClan->SetEnable(TRUE);
	m_pProfile->SetEnable(TRUE);
	m_pInventory->SetEnable(TRUE);
	m_pShop->SetEnable(TRUE);
	m_pCommunity->SetEnable(TRUE);
	m_pOption->SetEnable(TRUE);

	m_pClan->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pProfile->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pInventory->setControlState(I3GUI_CONTROL_STATE_NORMAL);	
	m_pOption->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_pShop->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	
	m_pCommunity->setControlState(I3GUI_CONTROL_STATE_NORMAL);
}

void CMenuBar::StartAlarmCommunity(void)
{
	m_bAlarmCommunity = TRUE;
}

void CMenuBar::EndAlarmCommunity(void)
{
	m_bAlarmCommunity = FALSE;

	// ���󺹱�
	m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
	m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
}

void CMenuBar::SetPushedStateButton(INT32 menuButtonType,BOOL bPushed)
{
	// ���¸� �ٲ� ��ư�� �޴´�.
	i3GuiButton* button;

	switch(menuButtonType)
	{
	case MBT_CLAN:			button = m_pClan;		break;
	case MBT_PROFILE:		button = m_pProfile;	break;
	case MBT_INVENTORY:		button = m_pInventory;	break;
	case MBT_SHOP:			button = m_pShop;		break;
	case MBT_COMMUNITY:		button = m_pCommunity;	break;
	case MBT_OPTION:		button = m_pOption;		break;
	default:
		return;
	}

	// ��ư�� ���¸� �ٲ۴�.
	if (bPushed)
	{
		button->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_PRESS_BUTTON_CLICKED);		
		button->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_PRESS_BUTTON_CLICKED);
	}
	else
	{
		button->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_PRESS_BUTTON_NORMAL);
		button->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_PRESS_BUTTON_ONMOUSE);
	}
}

void CMenuBar::SetExitMsgBoxOffset(INT32 x, INT32 y)
{
	m_ExitMsgBoxOffset.x	= x;
	m_ExitMsgBoxOffset.y	= y;
}

void CMenuBar::SetFlowAnnounce(const char* strAnnounce)
{
	if (MAX_STRING_COUNT > i3String::Length(strAnnounce))
	{
		// ���� ������ �ٸ� ��츸 �ٲ۴�
		if (0 != i3String::Compare(m_strAnnounceBuffer, strAnnounce))
		{
			i3String::Copy(m_strAnnounceBuffer, strAnnounce, MAX_STRING_COUNT );		
			m_idxAnnounceLetter = -1;			
		}
	}
}

void CMenuBar::SetFlowAnnounce2(const char* strAnnounce)
{
	if (MAX_STRING_COUNT > i3String::Length(strAnnounce))
	{
		// ���� ������ �ٸ� ��츸 �ٲ۴�
		if (0 != i3String::Compare(m_strAnnounceBuffer2, strAnnounce))
		{
			i3String::Copy(m_strAnnounceBuffer2, strAnnounce, MAX_STRING_COUNT );		
			m_idxAnnounceLetter = -1;			
			m_bFCMFlag = TRUE;
			m_bFlowFlag = TRUE;
		}
	}
}
//�ϵ��� : Abusing �Ϸ� �� ���� : �ϵ���
void CMenuBar::_UpdateUserInfo(void)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
	
	// �г���
	if( MyUserInfoBasic._nickcolor == 0)
	{
		m_pNickname->SetTextColor(GetColor(GCT_INVERSE));	//	�÷� �г����� �ƴ� ���
		m_pNickname->SetTextEllipsis(MyUserInfoBasic._nick);
	}
	else
	{	//	�÷� �г����� ����ϰ� ���� ���
		SetNameUsingColorTable(m_pNickname, MyUserInfoBasic._nick, MyUserInfoBasic._nickcolor);
	}



	/*
	switch( MyUserInfoBasic._eSportID)
	{
	case ESPORT_ID_LEVEL_ZERO:	// ������ E-Sport �±�
		{
			m_pESport->SetEnable( TRUE);
			m_pESport->SetShapeEnable(0, TRUE);
		}
		break;
	default:
		{
			m_pESport->SetEnable( FALSE);
		}
		break;
	}
	*/
	if (MyUserInfoBasic._eSportID == ESPORT_ID_LEVEL_ZERO)
	{
		m_pESport->SetEnable( FALSE);
	}
	else
	{
		m_pESport->SetEnable( TRUE);
		m_pESport->SetShapeEnable(0, TRUE);

	}

	{	
		char strBuf[256];
		char strBuf1[256];
		_ShowRankMark( MyUserInfoBasic._rank );

		UINT32 wRank = g_pGameContext->GetRealRank();

		INT32 nCurrentExp = MyUserInfoBasic._exp - _GetExpMax(wRank);
		nCurrentExp = MAX(nCurrentExp, 0);	// �������� 0
		INT32 nExpRate = 100;


		// ����� ����� ��ǥ�� ���(45:��� �� �̻�) ��ǥ ����ġ ����
		if(51 == wRank)
		{
			i3String::Copy( strBuf, GAME_STRING("STR_TBL_GUI_ETC_CUMULATIVE_EXP"), 256 );
			sprintf_s( strBuf1, "%d    ", MyUserInfoBasic._exp);
			i3String::Concat( strBuf,strBuf1 );
		
		}
		else if (45 <= wRank)
		{
			sprintf_s( strBuf, "%d (100%%)   ", MyUserInfoBasic._exp);
		}
		else
		{
			INT32 nMaxExp = _GetExpMax(wRank + 1) - _GetExpMax(wRank);		
			if( nMaxExp != 0) nExpRate = (nCurrentExp * 100) / nMaxExp;

			nExpRate = MINMAX(0, nExpRate, 100);	// 100%�� ���� ��� 100%���� ���� ���
			sprintf_s( strBuf, "%d/%d (%d%%)  ", nCurrentExp, nMaxExp, nExpRate);
		}
		m_pExp->SetText(strBuf);


		char gageText[MAX_STRING_COUNT] = {0, };
		BOOL visibleGage = TRUE;

        // ����� ��� (45:��� �� �̻�)
		
		if(51 == wRank)
		{
			i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_HERO"), MAX_STRING_COUNT );//����

			visibleGage = FALSE;
		}
		else if (50 == wRank)
		{
			i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED5"), MAX_STRING_COUNT );//���� 1% ��ŷ�� ���� ��� ���� ��
			visibleGage = FALSE;
		}
		else if (45 <= wRank)
		{
			if (_GetExpMax(50) <= (signed)MyUserInfoBasic._exp)
			{
				i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED3"), MAX_STRING_COUNT );//���� ������ ���� 1% ��ŷ �ʿ�
				visibleGage = FALSE;
			}
			else if (_GetExpMax(49) <= (signed)MyUserInfoBasic._exp)
			{
				i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED6"), MAX_STRING_COUNT );//���� ������ ���� 2% ��ŷ �ʿ�
				visibleGage = FALSE;
			}
			else if (_GetExpMax(48) <= (signed)MyUserInfoBasic._exp)
			{
				i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED1"), MAX_STRING_COUNT );//���� ������ ���� 3% ��ŷ �ʿ�
				visibleGage = FALSE;
			}
			else if (_GetExpMax(47) <= (signed)MyUserInfoBasic._exp)
			{
				i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED4"), MAX_STRING_COUNT );//���� ������ ���� 4% ��ŷ �ʿ�
				visibleGage = FALSE;
			}
			else if (_GetExpMax(46) <= (signed)MyUserInfoBasic._exp)
			{
				i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED2"), MAX_STRING_COUNT );//���� ������ ���� 5% ��ŷ �ʿ�
				visibleGage = FALSE;
			}
		}

	
		m_pExpText->SetText(gageText);
		m_pExpBar->SetEnable(visibleGage);

		if (visibleGage)
		{
			_ShowDotRate(nExpRate);
		}

		sprintf_s( strBuf, "%d", MyUserInfoBasic._point);
		m_pPinoValue->SetText(strBuf);

#if defined (LOCALE_KOREA)		
		//N-Coin & �������δ� �ѱ� NC�� ���
		sprintf_s( strBuf, "%d\n %d", MyUserInfoBasic._NCCoin, MyUserInfoBasic._HappyCoin);
#else
		sprintf_s( strBuf, "%d", MyUserInfoBasic._cash);

#endif
		m_pNcCoinValue->SetText( strBuf);
	}
}

void CMenuBar::_ShowRankMark(INT32 nRank)
{
	m_pRank->SetShapeUVIndex(nRank);
}

INT32 CMenuBar::_GetExpMax(INT32 nRank)
{
	I3ASSERT(nRank >= 0);
	I3ASSERT((nRank >= 0 && nRank < RANK_NEWBIE/*�ӽ�*/) || (nRank == RANK_GAMEMASTER) || (nRank == RANK_NEWBIE));

	if (nRank >= 0 && nRank < RANK_51+1)
	{
		return g_ExpValue[nRank];
	}
	else
	{
		return 0;
	}
}

void CMenuBar::_ShowDotRate(INT32 nExpRate)
{
	INT32 nDotNumber;

	if( nExpRate >= 100)	nDotNumber = 20;
	else if( nExpRate <= 0)		nDotNumber = 0;
	else nDotNumber = nExpRate / 5;

	m_pExpGage->SetAllShapeEnable(FALSE);
	
	for(INT32 i = 0; i < nDotNumber; i++)
	{
		m_pExpGage->SetShapeEnable(GUI_SHAPE_OFFSET + i, TRUE);
	}
}

void CMenuBar::_InitFlowAnnounce(void)
{
	I3ASSERT(NULL == m_pCalculateText);
	
	I3COLOR color;
	i3Color::Set(&color, (UINT8) 0, 0, 0, 0);

	m_pCalculateText = i3TextNodeDX2::NewObject();
#if USE_VERTEX_BANK
	m_pCalculateText->Create( GetDefaultFontName(), 256, 10, FW_NORMAL, 0, 0, 0, 0, FONT_SHADOW_QUALITY_LOW, 1.0f, TRUE);
#else
	m_pCalculateText->Create( GetDefaultFontName(), 256, 10, FW_NORMAL, 0, 0, 0, 0, FONT_SHADOW_QUALITY_LOW, 1.0f, FALSE);
#endif
	m_pCalculateText->setPos(0, 0);
	m_pCalculateText->setSize(100, 25);
	m_pCalculateText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);	
	m_pCalculateText->SetColor(&color);
	g_pFramework->getMessagBoxLayer()->AddChild(m_pCalculateText);

	REAL32 rateX = i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f;
	m_nAnnounceWidth = (INT32)(m_pAnnounce->getWidth() * rateX);
	m_nAnnounceWidth = MAX(0, m_nAnnounceWidth - 60);

	// dummmy
	//SetFlowAnnounce(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_ADVERTISEMENT"));/*[����] Ÿ������! ����Ʈ ��ũ, ���� 100�� ���� ��� ���� �̺�Ʈ - �Ž�ų�� �����! �ѳ༮, �γ༮ ���ư������� ��ǰ�� �Բ� ������! �̴�����, �Ͽ��� ��Ű��, AK47(���ݺ���), ��ī����2, ������ �ڵ����� �ѿ��� �� �ϱ����� ��ǰ���� �غ�Ǿ��ִ�!*/
	SetFlowAnnounce( g_pGameContext->getNormalNotice() );
	
	m_idxAnnounceLetter = 0;
	m_flowState = FS_INIT;
}

void CMenuBar::_DestroyFlowAnnounce(void)
{
	if (m_pCalculateText)
	{
		g_pFramework->getMessagBoxLayer()->RemoveChild(m_pCalculateText);
		I3_SAFE_RELEASE(m_pCalculateText);
	}
}

void CMenuBar::_UpdateFlowAnnounce(REAL32 rDeltaSeconds)
{	
	// 2���� ������ �������Դϴ�. - praptor
	//  1. i3GuiEditBox��ü�� �̵���Ű�� ���
	//  2. SetTextOffset�� �̿��� �ѱ��ھ� ��/Ǫ���ϸ鼭 ���� ���
	//
	// ���� �������� �ʰų�, �ǹ̰� ���� �ٸ� �����͸���� �ֽ��ϴ�. �������� �����ּ���.
	// ���� ���ʴ� ���� �帧�� '����Ÿ���' ������ �ֽ��ϴ�.
	//
	//  3. 2���� i3GuiEditBox�� �̿��� ��ȣ Filp��Ű�� �̵���Ű�� ����� ������Դϴ�.	

	switch(m_flowState)
	{
	case FS_INIT:
		if(m_bFlowTime > 120.0f)
		{
			m_bFCMFlag = FALSE;
			m_bFlowFlag = FALSE;
			m_bFlowTime = 0.0f;
			i3String::Copy(m_strAnnounceBuffer2, m_strAnnounceBuffer, MAX_STRING_COUNT );
		}
		if(TRUE  == m_bFCMFlag)	
		{
			//m_bFCMFlag = FALSE;
			m_pAnnounce->SetText(m_strAnnounceBuffer2);			
		}
		else
		{
			m_pAnnounce->SetText(m_strAnnounceBuffer);
		}

		m_nMoveOffset = m_pAnnounce->getTextNodeDX()->getTextWidth();
		m_rCurrentOffset = (REAL32)i3GuiRoot::getViewer()->GetViewWidth();
		m_pAnnounce->setPositionX(m_rCurrentOffset);
		m_flowState = FS_FLOW;
		break;
	case FS_FLOW:
		{
			m_rCurrentOffset -= 60.0f * rDeltaSeconds;
			m_pAnnounce->setPositionX(m_rCurrentOffset);
			if(m_bFlowFlag)
			{
				m_bFlowTime += rDeltaSeconds;
			}

			// ���̳ʽ� m_nMoveOffset���� �۾�����
			if (-m_nMoveOffset > m_rCurrentOffset || -1 == m_idxAnnounceLetter)
			{
				m_idxAnnounceLetter = 0;
				m_flowState = FS_INIT;
			}
		}
		break;
	case FS_READ_LETTER:
		// Empty
		break;
	default:
		break;
	}
}

void CMenuBar::_UpdateAlarm(REAL32 rDeltaSeconds)
{
	if (m_bAlarmCommunity)
	{
		m_rAlarmTerm += rDeltaSeconds;

		if (0.25f < m_rAlarmTerm)
		{
			m_rAlarmTerm = 0.0f;
			m_bAlarmFlag = !m_bAlarmFlag;

			if (m_bAlarmFlag)
			{
				m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
				m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
			}
			else
			{
				m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
				m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
			}
		}
	}
}

// ä��â�� ���� ���� �ִ� ��쿡�� ���â�� ǥ��
void CMenuBar::_UpdateRoomMasterKickAlarm(void)
{	
#ifdef USE_ROOM_MASTER_KICK
	// ������ ��쿡�� ���� ��.
	if(TRUE == ((CStageReady*)m_pParent)->Get_bRoomMsterAwayCountDown())
	{		
		// ������ �뿡�� �ٸ� â���� ������ Ȯ���ϱ� ���� ���
		if(STEP_NULL == ((CStageReady*)m_pParent)->GetCurrentStepType())
		{
			if(TRUE == m_bRoomMsterKickAlarmFlag)	m_bRoomMsterKickAlarmFlag = FALSE;
			return;
		}
		// ���� ���� ���â�� Ȯ�� ��ư�� ���� ������� �˻�
		if(FALSE == g_pFramework->IsMessageBoxEnabled() &&	TRUE == m_bRoomMsterKickAlarmFlag)	return;
		// ���� ���� ���â�� ó������ �˾��� ��� �˾� Flag�� Ȱ��ȭ�Ѵ�.
		if(FALSE == m_bRoomMsterKickAlarmFlag)	m_bRoomMsterKickAlarmFlag = TRUE;
		// ���� ���� ���â���� ���� �ð��� �˸�
		if ((INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION - 1.0f) <= ((CStageReady*)m_pParent)->GetRoomMasterAwayCheckerTime())// && (INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_TIME + AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION) > m_RoomMasterAwayCheckerTime)
		{			
			if(m_nCurrentCountDown == ((CStageReady*)m_pParent)->GetRoomMasterAwayCheckerTime())	return;
			m_nCurrentCountDown = ((CStageReady*)m_pParent)->GetRoomMasterAwayCheckerTime();

			char countdownText[MAX_STRING_COUNT];
			if(m_pMessageBox)
			{
				g_pFramework->CloseMessageBox(m_pMessageBox);
				m_pMessageBox = NULL;
			}
			i3String::Format(countdownText, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_READYROOM_NOTICE_PRE_ROOM_MSTER_KICK_ALARM"), ((CStageReady*)m_pParent)->GetRoomMasterAwayCheckerTime() - (INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION - 1.0f));/*{col:255,120,0,255}���� ���� %d�� ��{/col}*/
			m_pMessageBox = STRING_POPUP(countdownText);
		}
	}
	else
	{
		// ���� ���� ��ҵǾ��� ��� ���â �˾� Flag�� �ʱ�ȭ�Ѵ�.
		if(TRUE == m_bRoomMsterKickAlarmFlag)	m_bRoomMsterKickAlarmFlag = FALSE;
		// ���� ���� ī��Ʈ �ð��� �ʱ�ȭ�Ѵ�.
		if(0 != m_nCurrentCountDown)	m_nCurrentCountDown = 0;
		// ���� ���� ���â�� �� �ִ� ��� â�� �ݴ´�.
		if(m_pMessageBox)
		{
			g_pFramework->CloseMessageBox(m_pMessageBox);
			m_pMessageBox = NULL;
		}		
	}
#endif
}

i3GuiButton* CMenuBar::GetEffectButton()
{
	return m_pShop;
}

void CMenuBar::UpdateButton()
{
	CStageReady * pReady = ((CStageReady*)m_pParent);
	bool bMouseOnProfile = false;

	if(m_pShop->getControlState() == I3GUI_CONTROL_STATE_ONMOUSE)
	{
		g_ShopState = FALSE;
		m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET+1);
	}
	else if(m_pProfile->getControlState() == I3GUI_CONTROL_STATE_ONMOUSE)
	{
		bMouseOnProfile = true;
		m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET+1);
	}
	else
	{
		if(g_FirstShop)
			g_ShopState = TRUE;
		else
			g_ShopState = FALSE;
	}
	
	if(pReady->GetButtonEffect())
	{
		if(g_ShopState)
		{
			m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
			m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
		}
		if(g_pGameContext->IsShowMissionButtonEffect() && bMouseOnProfile == false)
		{
			m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
			m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
		}
	}
	else
	{
		if(g_ShopState)
		{
			m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
			m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
		}
		if(g_pGameContext->IsShowMissionButtonEffect() && bMouseOnProfile == false)
		{
			m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
			m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
		}
	}
}

void CMenuBar::EnterNickSpace()	
{
	m_bNickSpace = true;	
}

void CMenuBar::OutroNickSpace()	
{	
	m_bNickSpace = false;	
}
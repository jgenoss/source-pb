#include "pch.h"
#include "ClanDef.h"
#include "GlobalVariables.h"
#include "StepClanMember.h"
#include "StageGui/Popup/PopupUseItem.h"
#include "ContextMenu.h"
#include "CommunityContext.h"
#include "ServerInfo.h"

const INT32 g_nAdmissionLimitRankCount = 5;
const char * g_pszAdmissionLimitRankItem[ g_nAdmissionLimitRankCount] = { NULL, NULL, NULL, NULL, NULL};

const INT32 g_nAdmissionLimitAgeLowCount = 4;
const char * g_pszAdmissionLimitAgeLowItem[ g_nAdmissionLimitAgeLowCount] = { NULL, NULL, NULL, NULL};

const INT32 g_nAdmissionLimitAgeHighCount = 4;
const char * g_pszAdmissionLimitAgeHighItem[ g_nAdmissionLimitAgeHighCount] = { NULL, NULL, NULL, NULL};

CLAN_SORT_TYPE CStepClanMember::m_sInfoClanMemberSortType = CLAN_SORT_NONE;
CLAN_SORT_TYPE CStepClanMember::m_sManageClanMemberSortType = CLAN_SORT_NONE;
CLAN_SORT_TYPE CStepClanMember::m_sManageSubscriberSortType = CLAN_SORT_NONE;

#define LIST_UPDATE_TIME	2.0f

#if defined( TEST_CLAN_BATTLE)
static CLAN_BATTLE_TEAM_LIST * g_pTestTeamInfoList = NULL;
#endif

I3_CLASS_INSTANCE(CStepClanMember, CStepBase);

CStepClanMember::CStepClanMember(void)
{
	m_pClanContext = g_pFramework->GetClanContext();

	m_nChatType = 0;
	m_nOldChatType = 0;
	m_CurrentPage = CLAN_PAGE_UNKNOWN;

	m_pClanMemberRoot = NULL;

	m_pChatBody = CGameChatBody::NewObject();
	
	m_pMyClanWin = NULL;
	m_pMyClanCaption = NULL;

	m_pClanManagementWin = NULL;
	m_pClanManagementCaption = NULL;

	m_pInfoTab = NULL;
	m_pBattleTab = NULL;
	m_pSearchTab = NULL;
	m_pManagementTab = NULL;

	m_pInfoHistoryButton = NULL;
	m_pInfoMemberButton = NULL;
	m_pInfoSecessionButton = NULL;

	m_pInfoPhase = NULL;
	m_pBattlePhase = NULL;
	m_pSearchPhase = NULL;
	m_pManagementPhase = NULL;
	
	m_pInfoHistroyPhase = NULL;
	m_pInfoHistoryNoticeStatic = NULL;
	m_pInfoHistoryNoticeValue = NULL;
	m_pInfoHistoryListHeadSeasonScore = NULL;
	m_pInfoHistoryListHeadTotalScore = NULL;

	m_pInfoMemberPhase = NULL;

	m_pBattleMakeTeamStatic = NULL;
	m_pBattleMakeTeamEntryButton = NULL;
	m_pBattleMakeTeamCombo = NULL;
	m_pBattleMakeTeamComboButton = NULL;
	m_pBattleMakeTeamComboPopup = NULL;
	m_pBattleTeamScrollList = NULL;

	m_idxBattleTeamListStartSlot = 0;
	m_idxBattleTeamMatchType = 0;
	m_nBattleTeamListSelectedIdx = 0;

	for(INT32 i=0; i<CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX; i++)
	{
		m_slotBattleTeamList[i].pSlot = NULL;
		m_slotBattleTeamList[i].pLocale = NULL;
		m_slotBattleTeamList[i].pTeamName = NULL;
		m_slotBattleTeamList[i].pState = NULL;
		m_slotBattleTeamList[i].pLeaderValue = NULL;
		m_slotBattleTeamList[i].pLeaderRank = NULL;
		m_slotBattleTeamList[i].pLeaderNick = NULL;
		m_slotBattleTeamList[i].pMatchType = NULL;
		m_slotBattleTeamList[i].pEntryMember = NULL;
		m_slotBattleTeamList[i].pJoinButton = NULL;
	}

	m_pSearchStatic = NULL;
	m_pSearchInput = NULL;
	m_pSearchButton = NULL;
	m_pSearchViewAll = NULL;
	m_pClanMembeSerchText = NULL;

	m_pClanSerchBar = NULL;
	m_pClanSerchGage = NULL;
	m_pClanSerchText = NULL;

	m_pSearchInfoAddressBullet = NULL;

	m_pSearchInfoClanMarkImage = NULL;
	m_pSearchInfoNameStatic = NULL;
	m_pSearchInfoNameValue = NULL;
	m_pSearchInfoMasterStatic = NULL;
	m_pSearchInfoMasterValue = NULL;
	m_SearchInfoMasterRankImage = NULL;
	m_pSearchInfoClanRankStatic = NULL;
	m_pSearchInfoClanRankValue = NULL;
	m_SearchInfoClanRankImage = NULL;
	m_pSearchInfoStaffStatic = NULL;
	m_pSearchInfoStaffValue = NULL;
	m_pSearchInfoAddressStatic = NULL;
	m_pSearchInfoAddressValue = NULL;
	m_pSearchInfoOrderStatic = NULL;
	m_pSearchInfoOrderValue = NULL;
	m_pSearchInfoExpStatic = NULL;
	m_pSearchInfoExpValue = NULL;
	m_SearchInfoExpBar = NULL;

	m_pSearchClanListHeadRank = NULL;
	m_pSearchClanListHeadName = NULL;
	m_pSearchClanListHeadDate = NULL;
	m_pSearchClanListHeadMember = NULL;

	m_pSearchClanListScroll = NULL;
	m_pSearchClanListFocus = NULL;
	m_pSearchClanListSelect = NULL;

	for(INT32 i=0; i<CLAN_SEARCH_LIST_SLOT_MAX; i++)
	{
		m_ClanListSlot[i]._pSlot = NULL;
		m_ClanListSlot[i]._pRankImage = NULL;
		m_ClanListSlot[i]._pName = NULL;
		m_ClanListSlot[i]._pClanMarkImage = NULL;
		m_ClanListSlot[i]._pDate = NULL;
		m_ClanListSlot[i]._pMember = NULL;
	}

	m_pManagementWritingBoardPhase = NULL;
	m_pManagementMemberPhase = NULL;
	m_pManagementSubscriptionPhase = NULL;
	m_pManagementOthersPhase = NULL;

	m_pManagementWritingBoardButton = NULL;
	m_pManagementMemberButton = NULL;
	m_pManagementSubscriptionButton = NULL;	
	m_pManagementOthersButton = NULL;

	m_pManagementWritingBoardNoticeStatic = NULL;
	m_pManagementWritingBoardNoticeEdit = NULL;
	m_pManagementWritingBoardGuideStatic = NULL;
	m_pManagementWritingBoardGuideEdit = NULL;
	m_pManagementWritingBoardNoticeModify = NULL;	
	m_pManagementWritingBoardGuideModify = NULL;

	m_pManagementMemberStatic = NULL;
	m_pManagementListHeadNick = NULL;
	m_pManagementListHeadClass = NULL;
	m_pManagementListHeadEntryDay = NULL;
	m_pManagementMemberScrollList = NULL;

	m_pManagementSubscriberStatic = NULL;
	m_pSubscriberListHeadNick = NULL;
	m_pSubscriberListHeadEntryDay = NULL;
	m_pManagementSubscriberScrollList = NULL;

	m_pManagementInstructionStatic = NULL;
	m_pManagementInstructionCombo = NULL;
	m_pManagementInstructionButton = NULL;
	m_pManagementInstructionPopup = NULL;

	m_pSubscriptionInstructionStatic = NULL;
	m_pSubscriptionInstructionCombo = NULL;
	m_pSubscriptionInstructionButton = NULL;
	m_pSubscriptionInstructionPopup = NULL;

	m_pManagementOthersAuthorityStatic = NULL;			
	m_pManagementOthersWritingAuthorityStatic = NULL;
	m_pManagementOthersInviteAuthorityStatic = NULL;	
	m_pManagementOthersDimissalAuthorityStatic = NULL;	
	m_pManagementOthersSubscriptionAuthorityStatic = NULL;
	m_pManagementOthersWritingAuthorityCheck = NULL;	
	m_pManagementOthersInviteAuthorityCheck = NULL;	
	m_pManagementOthersDimissalAuthorityCheck = NULL;	
	m_pManagementOthersSubscriptionAuthorityCheck = NULL;
	m_pManagementOthersSendNoteStatic = NULL;			
	m_pManagementOthersSendNoteButton = NULL;			
	m_pManagementOthersAdmissionLimitStatic = NULL;		
	m_pManagementOthersAdmissionLimitRankStatic = NULL;	
	m_pManagementOthersAdmissionLimitRankCombo = NULL;	
	m_pManagementOthersAdmissionLimitRankButton = NULL;
	m_pManagementOthersAdmissionLimitRankPopup = NULL;
	m_pManagementOthersAdmissionLimitAgeStatic = NULL;

	for(INT32 i=0; i<ADMISSION_LIMIT_CONTROL_COUNT; i++)
	{
		m_pManagementOthersAdmissionLimitAgeCombo[i] = NULL;
		m_pManagementOthersAdmissionLimitAgeButton[i] = NULL;
		m_pManagementOthersAdmissionLimitAgePopup[i] = NULL;
	}

	m_pManagementOthersChangeClanNameStatic = NULL;		
	m_pManagementOthersChangeClanNameButton = NULL;		
	m_pManagementOthersChangeClanMarkStatic = NULL;		
	m_pManagementOthersChangeClanMarkButton = NULL;		
	m_pManagementOthersDissolutionStatic = NULL;
	m_pManagementOthersDissolutionButton = NULL;
	m_pManagementOthersModifyButton = NULL;

	m_pInfoBestListStatic = NULL;
	m_pInfoMemberListStatic = NULL;
	m_slotBestMemberListHeadSeason = NULL;
	m_slotBestMemberListHeadTotal = NULL;
	m_slotClanMemberListHeadClass = NULL;
	m_slotClanMemberListHeadNick = NULL;
	m_slotClanMemberListHeadState = NULL;
	m_pClanMemberScrollList = NULL;

	m_pMyClanMarkImage = NULL;
	m_pMyClanMasterRankImage = NULL;
	m_pMyClanRankImage = NULL;
	m_pMyClanExpBar = NULL;
	m_pMyClanAddressBullet = NULL;

	m_pMyClanNameStatic = NULL;
	m_pMyClanNameValue = NULL;
	m_pMyClanStaffStatic = NULL;
	m_pMyClanStaffValue = NULL;
	m_pMyClanMasterStatic = NULL;
	m_pMyClanMasterValue = NULL;
	m_pMyClanRankStatic = NULL;
	m_pMyClanRankValue = NULL;
	m_pMyClanExpStatic = NULL;
	m_pMyClanExpValue = NULL;
	m_pMyClanOrderStatic = NULL;
	m_pMyClanOrderValue = NULL;
	m_pMyClanAddressStatic = NULL;
	m_pMyClanAddressValue = NULL;
	m_pMyClanPointStatic = NULL;
	m_pMyClanPointValue = NULL;

	m_idxClanInfoMemberStartSlot = 0;
	m_idxClanManagementMemberStartSlot = 0;
	m_idxClanManagementSubscriberStartSlot = 0;

	i3mem::FillZero( m_ManageMemberInfo, sizeof(m_ManageMemberInfo));
	i3mem::FillZero( m_ManageSubscriberInfo, sizeof(m_ManageSubscriberInfo));

	m_bUpdateTeamList = FALSE;
	m_fUpdateTeamSection = 0.f;
	m_nTeamSection = 0;
	m_authItemFlag = 0;

	memset( &m_SelectedClanInfo, 0, sizeof( SORT_CLAN_INFO ) );

	m_bNeedUpdateSearchClanList = FALSE;
	m_nLastedSelectSlot = 0;
	m_nSearchClanSelectedSlot = 0;
	m_nSearchClanStartSlot = 0;
}

CStepClanMember::~CStepClanMember(void)
{
	I3_SAFE_RELEASE( m_pChatBody);

#if defined(TEST_CLAN_BATTLE)
	I3MEM_SAFE_FREE(g_pTestTeamInfoList);
#endif	
}

void CStepClanMember::OnPreGuiNotify(void)
{
	m_pManagementSubscriberFocus->SetEnable(FALSE);
	m_pSearchClanListFocus->SetEnable(FALSE);
}

void CStepClanMember::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	//	Ŭ�� ��Ʈ ��� �ʱ�ȭ
	{
		if( m_pInfoTab->isEnable())
		{
			m_slotClanMemberListHeadClass->SetTextColor( GetColor(GCT_DEFAULT));		
			m_slotClanMemberListHeadNick->SetTextColor( GetColor(GCT_DEFAULT));
			m_slotClanMemberListHeadState->SetTextColor( GetColor(GCT_DEFAULT));
		}

		if( m_pSearchTab->isEnable())
		{
			m_pSearchClanListHeadRank->SetTextColor( GetColor(GCT_DEFAULT));
			m_pSearchClanListHeadName->SetTextColor( GetColor(GCT_DEFAULT));
			m_pSearchClanListHeadDate->SetTextColor( GetColor(GCT_DEFAULT));
			m_pSearchClanListHeadMember->SetTextColor( GetColor(GCT_DEFAULT)); 
		}

		if( m_pManagementTab->isEnable())
		{
			m_pManagementListHeadClass->SetTextColor( GetColor(GCT_DEFAULT));
			m_pManagementListHeadNick->SetTextColor( GetColor(GCT_DEFAULT));
			m_pManagementListHeadEntryDay->SetTextColor( GetColor(GCT_DEFAULT));
			
			m_pSubscriberListHeadNick->SetTextColor( GetColor(GCT_DEFAULT));
			m_pSubscriberListHeadEntryDay->SetTextColor( GetColor(GCT_DEFAULT));
		}
	}

	switch(pNotify->m_nID)
	{
	//	Common	
	case GCI_CLAN_B_MEMBER_LEAVE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnLeave();
		break;
	case GCI_CLAN_B_MEMBER_INFO_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanInfoTab();
		break;
	case GCI_CLAN_B_MEMBER_BATTLE_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanBattleTab();
		break;
	case GCI_CLAN_B_MEMBER_SEARCH_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanSearchTab();
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanManagementTab();
		break;
	case GCI_CLAN_L_MEMBER_CHAT_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) m_pChatBody->OnDragChatList();
		break;
	case GCI_CLAN_P_MEMBER_CHAT_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnClanChatPopup(pNotify->m_nPriParam);
		break;
	case GCI_CLAN_B_MEMBER_CHAT_TYPE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnClanChatType();
		break;
	case GCI_CLAN_P_MEMBER_CHAT_CHANNEL1:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	m_pChatBody->OnChatChannel(0);
		break;
	case GCI_CLAN_P_MEMBER_CHAT_CHANNEL2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	m_pChatBody->OnChatChannel(1);
		break;
	case GCI_CLAN_P_MEMBER_CHAT_CHANNEL3:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	m_pChatBody->OnChatChannel(2);
		break;

	//	Ŭ������
	case GCI_CLAN_B_MEMBER_INFO_HISTORY:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanInfoHistoryButton();
		break;
	case GCI_CLAN_B_MEMBER_INFO_MEMBER:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanInfoMemberButton();
		break;
	case GCI_CLAN_B_MEMBER_INFO_SECESSION:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanInfoSecessionButton();
		break;
	case GCI_CLAN_L_MEMBER_INFO_MEMBER_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnClanInfoMemberScroll();
		break;
	case GCI_CLAN_S_MEMBER_INFO_SLOT_1:
	case GCI_CLAN_S_MEMBER_INFO_SLOT_2:
	case GCI_CLAN_S_MEMBER_INFO_SLOT_3:
	case GCI_CLAN_S_MEMBER_INFO_SLOT_4:
	case GCI_CLAN_S_MEMBER_INFO_SLOT_5:
	case GCI_CLAN_S_MEMBER_INFO_SLOT_6:
	case GCI_CLAN_S_MEMBER_INFO_SLOT_7:
    	if (I3GUI_STATIC_NOTIFY_RCLICK == pNotify->m_nEvent) OnClanInfoMemberContext(pNotify->m_nID - GCI_CLAN_S_MEMBER_INFO_SLOT_1);
		break;
	case GCI_CLAN_E_MEMBER_INFO_HEADER_LEVEL:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortClanMemberListByLevel();
		break;
	case GCI_CLAN_E_MEMBER_INFO_HEADER_NAME:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortClanMemberListByName();
		break;
	case GCI_CLAN_E_MEMBER_INFO_HEADER_STATE:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortClanMemberListByState();
		break;

	//	Ŭ���� 
	case GCI_CLAN_B_MEMBER_BATTLE_TEAM_ENTRY:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanBattleTeamEntry();
		break;
	case GCI_CLAN_E_MEMBER_BATTLE_TEAM_TYPE_COMBO:
		if(I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanBattleTeamTypeButton();
		break;
	case GCI_CLAN_B_MEMBER_BATTLE_TEAM_TYPE_BUTTON:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanBattleTeamTypeButton();
		break;
	case GCI_CLAN_P_MEMBER_BATTLE_TEAM_TYPE_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanBattleTeamTypePopup(pNotify->m_nPriParam);
		break;
	case GCI_CLAN_L_MEMBER_BATTLE_TEAM_LIST_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnClanBattleTeamListScroll();		
		break;	
	case GCI_CLAN_L_MEMBER_BATTLE_TEAM_JOIN1:
	case GCI_CLAN_L_MEMBER_BATTLE_TEAM_JOIN2:
	case GCI_CLAN_L_MEMBER_BATTLE_TEAM_JOIN3:
	case GCI_CLAN_L_MEMBER_BATTLE_TEAM_JOIN4:
	case GCI_CLAN_L_MEMBER_BATTLE_TEAM_JOIN5:
	case GCI_CLAN_L_MEMBER_BATTLE_TEAM_JOIN6:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanBattleTeamJoin(pNotify->m_nID - GCI_CLAN_L_MEMBER_BATTLE_TEAM_JOIN1);
		break;

	//	Ŭ��ã��
	case GCI_CLAN_B_MEMBER_SEARCH_SEARCHING:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSearchClanKeyword();
		break;
	case GCI_CLAN_B_MEMBER_SEARCH_VIEW_ALL:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSearchClanViewAll();
		break;
	case GCI_CLAN_E_MEMBER_SEARCH_HEADER_RANK:
		if(I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent) OnSortSearchClanListByRank();
		break;
	case GCI_CLAN_E_MEMBER_SEARCH_HEADER_NAME:
		if(I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent) OnSortSearchClanListByName();
		break;
	case GCI_CLAN_E_MEMBER_SEARCH_HEADER_DATE:
		if(I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent) OnSortSearchClanListByDate();
		break;
	case GCI_CLAN_E_MEMBER_SEARCH_HEADER_MEMBER:
		if(I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent) OnSortSearchClanListByMember();
		break;
	case GCI_CLAN_S_MEMBER_SEARCH_SLOT1:
	case GCI_CLAN_S_MEMBER_SEARCH_SLOT2:
	case GCI_CLAN_S_MEMBER_SEARCH_SLOT3:
	case GCI_CLAN_S_MEMBER_SEARCH_SLOT4:
	case GCI_CLAN_S_MEMBER_SEARCH_SLOT5:
	case GCI_CLAN_S_MEMBER_SEARCH_SLOT6:
	case GCI_CLAN_S_MEMBER_SEARCH_SLOT7:
	case GCI_CLAN_S_MEMBER_SEARCH_SLOT8:
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnSearchClanFocus( pNotify->m_nID - GCI_CLAN_S_MEMBER_SEARCH_SLOT1);
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnSearchClanSelect( pNotify->m_nID - GCI_CLAN_S_MEMBER_SEARCH_SLOT1);
		break;
	case GCI_CLAN_L_MEMBER_SEARCH_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnSearchClanListScroll();		
		break;	
		
	//	Ŭ������
	case GCI_CLAN_B_MEMBER_MANAGEMENT_WRITING_BOARD:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanManagementWritingBoardButton();
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_MEMBER:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanManagementMemberButton();
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_SUBSCRIPTION:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanManagementSubscriptionButton();
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanManagementOthersButton();
		break;
	case GCI_CLAN_E_MEMBER_MANAGEMENT_LIST_COMBO:		
		if(I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent) OnManagementInstructionButton();
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_LIST_BUTTON:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnManagementInstructionButton();
		break;
	case GCI_CLAN_P_MEMBER_MANAGEMENT_LIST_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnManagementInstructionPopup(pNotify->m_nPriParam);
		break;
	case GCI_CLAN_E_MEMNER_MANAGEMENT_LIST_HEADER_LEVEL:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortClanMemberListByLevel();
		break;
	case GCI_CLAN_E_MEMNER_MANAGEMENT_LIST_HEADER_NAME:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortClanMemberListByName();
		break;
	case GCI_CLAN_E_MEMNER_MANAGEMENT_LIST_HEADER_DATE:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortClanMemberListByDate();
		break;
	case GCI_CLAN_E_MEMBER_SUBSCRIPTION_LIST_COMBO:		
		if(I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent) OnSubscriptionInstructionButton();
		break;
	case GCI_CLAN_B_MEMBER_SUBSCRIPTION_LIST_BUTTON:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSubscriptionInstructionButton();
		break;
	case GCI_CLAN_P_MEMBER_SUBSCRIPTION_LIST_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnSubscriptionInstructionPopup(pNotify->m_nPriParam);
		break;
	case GCI_CLAN_E_MEMBER_SUBSCRIPTION_LIST_HEADER_NAME:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortSubscriberListByName();
		break;
	case GCI_CLAN_E_MEMBER_SUBSCRIPTION_LIST_HEADER_DATE:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortSubscriberListByDate();
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_BOARD_NOTICE_MODIFY:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanManagementBoardNoticeModify();
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_BOARD_GUIDE_MODIFY:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanManagementBoardGuideModify();
		break;
	case GCI_CLAN_L_MEMBER_MANAGEMENT_LIST_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnClanManagementMemberScroll();		
		break;		
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_ALL:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanManagementMemberCheckAll();
		break;
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_1:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_2:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_3:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_4:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_5:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_6:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_7:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_8:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_9:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_10:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_11:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_12:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanManagementMemberCheck(pNotify->m_nID - GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_1);
		break;
	case GCI_CLAN_L_MEMBER_SUBSCRIPTION_LIST_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnClanManagementSubscriberScroll();
		break;	
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_ALL:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanManagementSubscriberCheckAll();
		break;
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_1:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_2:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_3:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_4:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_5:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_6:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_7:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_8:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_9:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_10:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_11:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_12:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanManagementSubscriberCheck(pNotify->m_nID - GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_1);
		break;
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_1:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_2:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_3:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_4:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_5:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_6:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_7:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_8:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_9:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_10:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_11:
	case GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_12:
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent)		OnClanManagementSubscriberFocus(pNotify->m_nID - GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)		OnClanManagementSubscriberSelect(pNotify->m_nID - GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)	OnClanManagementSubscriberConfirm(pNotify->m_nID - GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_1);				
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS_SEND_NOTE:
		if( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnManagementOthersSendNote();
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS_CHANGE_CLAN_NAME:
		if( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnManagementOthersChangeClanName();
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS_CHANGE_CLAN_MARK:
		if( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnManagementOthersChangeClanMark();
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS_DISSOLUTION:
		if( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnManagementOthersDissolution();
		break;
	case GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS_MODIFY:
		if( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnManagementOthersModify();
		break;
	case GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_WRITING_AUTHORITY_CHECK:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_INVITE_AUTHORITY_CHECK:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_DISMISSAL_AUTHORITY_CHECK:
	case GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_SUBSCRIPTION_AUTHORITY_CHECK:
		if( I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent)		OnManagementOthersAuthorityCheck(pNotify->m_nID);
		break;
	case GCI_CLAN_E_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_RANK_COMBO:		
		if(I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent)			OnManagementOthersAdmissionLimitOpen( ADMISSION_LIMIT_RANK);
		break;
	case GCI_CLAN_P_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_RANK_POPUP:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnManagementOthersAdmissionLimitOpen( ADMISSION_LIMIT_RANK);
		break;
	case GCI_CLAN_L_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_RANK_SCROLL:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnManagementOthersAdmissionLimitInstruction( ADMISSION_LIMIT_RANK, pNotify->m_nPriParam);
		break;
	case GCI_CLAN_E_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_AGE_COMBO1:		
		if(I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent)			OnManagementOthersAdmissionLimitOpen( ADMISSION_LIMIT_AGE1);
		break;
	case GCI_CLAN_P_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_AGE_POPUP1:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnManagementOthersAdmissionLimitOpen( ADMISSION_LIMIT_AGE1);
		break;
	case GCI_CLAN_L_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_AGE_SCROLL1:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnManagementOthersAdmissionLimitInstruction( ADMISSION_LIMIT_AGE1, pNotify->m_nPriParam);
		break;
	case GCI_CLAN_E_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_AGE_COMBO2:		
		if(I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent)			OnManagementOthersAdmissionLimitOpen( ADMISSION_LIMIT_AGE2);
		break;
	case GCI_CLAN_P_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_AGE_POPUP2:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnManagementOthersAdmissionLimitOpen( ADMISSION_LIMIT_AGE2);
		break;
	case GCI_CLAN_L_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_AGE_SCROLL2:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnManagementOthersAdmissionLimitInstruction( ADMISSION_LIMIT_AGE2, pNotify->m_nPriParam);
		break;

	}
}

BOOL CStepClanMember::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char strName[MAX_STRING_COUNT] = "";

	LinkControlEx(plist, "ClanMemberWin", m_pClanMemberRoot, GCI_UNKNOWN);

	//	��Ŭ�� ������
	{
		LinkControlEx(plist, "clanMyClanWin", m_pMyClanWin, GCI_UNKNOWN);
		LinkControlEx(plist, "clanMyClanCaption", m_pMyClanCaption, GCI_UNKNOWN);
				
		{
			LinkControlEx(plist, "clanMyClanClanMark", m_pMyClanMarkImage, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanMasterRankImage", m_pMyClanMasterRankImage, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanRankImage", m_pMyClanRankImage, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanExpBar", m_pMyClanExpBar, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanAddressBullet", m_pMyClanAddressBullet, GCI_UNKNOWN);

			LinkControlEx(plist, "clanMyClanNameStatic", m_pMyClanNameStatic, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanNameValue", m_pMyClanNameValue, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanStaffStatic", m_pMyClanStaffStatic, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanStaffValue", m_pMyClanStaffValue, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanRankStatic", m_pMyClanRankStatic, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanMasterStatic", m_pMyClanMasterStatic, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanMasterValue", m_pMyClanMasterValue, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanRankValue", m_pMyClanRankValue, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanExpStatic", m_pMyClanExpStatic, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanExpValue", m_pMyClanExpValue, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanOrderStatic", m_pMyClanOrderStatic, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanOrderValue", m_pMyClanOrderValue, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanAddressStatic", m_pMyClanAddressStatic, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanAddressValue", m_pMyClanAddressValue, GCI_UNKNOWN);	
			LinkControlEx(plist, "clanMyClanPointStatic", m_pMyClanPointStatic, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanPointValue", m_pMyClanPointValue, GCI_UNKNOWN);	

			LinkControlEx(plist, "clanMyClanChatTextBox", m_pChatBody->m_pTextBox, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanChatList", m_pChatBody->m_pChatList, GCI_CLAN_L_MEMBER_CHAT_SCROLL);
			LinkControlEx(plist, "clanMyClanChatBox", m_pChatBody->m_pChatBox, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanChatLang", m_pChatBody->m_pChatLang, GCI_UNKNOWN);

			LinkControlEx(plist, "clanMyClanChatCombo", m_pChatBody->m_pChatCombo, GCI_UNKNOWN);
			LinkControlEx(plist, "clanMyClanChatButton", m_pChatBody->m_pChatButton, GCI_CLAN_B_MEMBER_CHAT_TYPE);
			LinkControlEx(plist, "clanMyClanChatPopup", m_pChatBody->m_pChatPopup, GCI_CLAN_P_MEMBER_CHAT_POPUP);

			for(INT32 i=0; i<CHAT_CHANNEL_BUTTON_COUNT; i++)
			{
				sprintf_s(strName, "clanMyClanChatTab%d", i + 1);

				LinkControlEx(plist, strName, m_pChatBody->m_pChatChannelButton[i], GCI_CLAN_P_MEMBER_CHAT_CHANNEL1+i);
			}
		}
	}

	//	Ŭ������ ������
	{
		LinkControlEx(plist, "clanManagementWin", m_pClanManagementWin, GCI_UNKNOWN);
		LinkControlEx(plist, "clanManagementCaption", m_pClanManagementCaption, GCI_UNKNOWN);

		LinkControlEx(plist, "clanInfoTab", m_pInfoTab, GCI_CLAN_B_MEMBER_INFO_TAB);
		LinkControlEx(plist, "clanBattleTab", m_pBattleTab, GCI_CLAN_B_MEMBER_BATTLE_TAB);
		LinkControlEx(plist, "clanSearchTab", m_pSearchTab, GCI_CLAN_B_MEMBER_SEARCH_TAB);
		LinkControlEx(plist, "clanManagementTab", m_pManagementTab, GCI_CLAN_B_MEMBER_MANAGEMENT_TAB);

		LinkControlEx(plist, "clanMemberLeaveButton", m_pLeaveButton, GCI_CLAN_B_MEMBER_LEAVE);

		LinkControlEx(plist, "clanInfoPhase", m_pInfoPhase, GCI_CLAN_S_MEMBER_INFO_PHASE);
		LinkControlEx(plist, "clanBattlePhase", m_pBattlePhase, GCI_CLAN_S_MEMBER_BATTLE_PHASE);
		LinkControlEx(plist, "clanSearchPhase", m_pSearchPhase, GCI_CLAN_S_MEMBER_SEARCH_PHASE);
		LinkControlEx(plist, "clanManagementPhase", m_pManagementPhase, GCI_CLAN_S_MEMBER_MANAGEMENT_PHASE);

		//	Ŭ������ ��
		{			
			LinkControlEx(plist, "clanInfoHistory", m_pInfoHistoryButton, GCI_CLAN_B_MEMBER_INFO_HISTORY);
			LinkControlEx(plist, "clanInfoMember", m_pInfoMemberButton, GCI_CLAN_B_MEMBER_INFO_MEMBER);
			LinkControlEx(plist, "clanInfoSecession", m_pInfoSecessionButton, GCI_CLAN_B_MEMBER_INFO_SECESSION);

			//	Ŭ������ ������		
			{
				LinkControlEx(plist, "clanInfoHistoryPhase", m_pInfoHistroyPhase, GCI_CLAN_S_MEMBER_INFO_HISTORY_PHASE);

				LinkControlEx(plist, "clanInfoHistoryNoticeStatic", m_pInfoHistoryNoticeStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanInfoHistoryNoticeTextBox", m_pInfoHistoryNoticeValue, GCI_UNKNOWN);

				LinkControlEx(plist, "clanInfoHistoryListHeadSeasonScore", m_pInfoHistoryListHeadSeasonScore, GCI_UNKNOWN);
				LinkControlEx(plist, "clanInfoHistoryListHeadTotalScore", m_pInfoHistoryListHeadTotalScore, GCI_UNKNOWN);

				for(UINT32 i = 0; i < CLANINFO_HISTORY_LIST_ROW_MAX; i++)
				{
					sprintf_s(strName, "clanInfoHistoryListSlot%d", i + 1);
					LinkControlEx(plist, strName, m_slotClanHistoryList[i].pSlot, GCI_UNKNOWN);

					sprintf_s(strName, "clanInfoHistoryListTitle%d", i + 1);
					LinkControlEx(plist, strName, m_slotClanHistoryList[i].pTitle, GCI_UNKNOWN);

					sprintf_s(strName, "clanInfoHistoryListSeasonScore%d", i + 1);
					LinkControlEx(plist, strName, m_slotClanHistoryList[i].pSeasonScore, GCI_UNKNOWN);

					sprintf_s(strName, "clanInfoHistoryListTotalScore%d", i + 1);
					LinkControlEx(plist, strName, m_slotClanHistoryList[i].pTotalScore, GCI_UNKNOWN);
				}
			}

			//	Ŭ�� ��� ������		
			{
				LinkControlEx(plist, "clanInfoMemberPhase", m_pInfoMemberPhase, GCI_CLAN_S_MEMBER_INFO_MEMBER_PHASE);

				LinkControlEx(plist, "clanBestListStatic", m_pInfoBestListStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanMemberListStatic", m_pInfoMemberListStatic, GCI_UNKNOWN);

				//	��� ��� ����Ʈ
				LinkControlEx(plist, "clanBestListHeadSeason", m_slotBestMemberListHeadSeason, GCI_UNKNOWN);
				LinkControlEx(plist, "clanBestListHeadTotal", m_slotBestMemberListHeadTotal, GCI_UNKNOWN);

				for(UINT32 i = 0; i < BEST_MEMBER_SLOT_MAX; i++)
				{
					sprintf_s(strName, "clanBestListSlot%d", i + 1);
					LinkControlEx(plist, strName, m_slotBestMemberList[i].pSlot, GCI_UNKNOWN);

					sprintf_s(strName, "clanBestListTitle%d", i + 1);
					LinkControlEx(plist, strName, m_slotBestMemberList[i].pTitle, GCI_UNKNOWN);

					sprintf_s(strName, "clanBestListSeasonMember%d", i + 1);
					LinkControlEx(plist, strName, m_slotBestMemberList[i].pSeasonMember, GCI_UNKNOWN);

					sprintf_s(strName, "clanBestListTopMember%d", i + 1);
					LinkControlEx(plist, strName, m_slotBestMemberList[i].pTopMember, GCI_UNKNOWN);
				}

				//	Ŭ�� ��� ����Ʈ				
				LinkControlEx(plist, "clanMemberListHeadClass", m_slotClanMemberListHeadClass, GCI_CLAN_E_MEMBER_INFO_HEADER_LEVEL);
				LinkControlEx(plist, "clanMemberListHeadNick", m_slotClanMemberListHeadNick, GCI_CLAN_E_MEMBER_INFO_HEADER_NAME);
				LinkControlEx(plist, "clanMemberListHeadState", m_slotClanMemberListHeadState, GCI_CLAN_E_MEMBER_INFO_HEADER_STATE);				
				LinkControlEx(plist, "clanMemberListScrollList", m_pClanMemberScrollList, GCI_CLAN_L_MEMBER_INFO_MEMBER_SCROLL); 

				for(UINT32 i = 0; i < CLANINFO_MEMBER_LIST_ROW_MAX; i++)
				{
					sprintf_s(strName, "clanMemberListSlot%d", i + 1);
					LinkControlEx(plist, strName, m_slotClanMemberList[i].pSlot, GCI_CLAN_S_MEMBER_INFO_SLOT_1 + i);

					sprintf_s(strName, "clanMemberListRank%d", i + 1);
					LinkControlEx(plist, strName, m_slotClanMemberList[i].pRank, GCI_UNKNOWN);

					sprintf_s(strName, "clanMemberListClass%d", i + 1);
					LinkControlEx(plist, strName, m_slotClanMemberList[i].pClass, GCI_UNKNOWN);

					sprintf_s(strName, "clanMemberListNick%d", i + 1);
					LinkControlEx(plist, strName, m_slotClanMemberList[i].pNick, GCI_UNKNOWN);

					sprintf_s(strName, "clanMemberListState%d", i + 1);
					LinkControlEx(plist, strName, m_slotClanMemberList[i].pState, GCI_UNKNOWN);
				}
			}
		}

		//	Ŭ���� ��
		{
			LinkControlEx(plist, "clanBattleMakeTeamStatic", m_pBattleMakeTeamStatic, GCI_UNKNOWN);
			LinkControlEx(plist, "clanBattleMakeTeamEntryButton",m_pBattleMakeTeamEntryButton, GCI_CLAN_B_MEMBER_BATTLE_TEAM_ENTRY);

			LinkControlEx(plist, "clanBattleMakeTeamCombo", m_pBattleMakeTeamCombo, GCI_CLAN_E_MEMBER_BATTLE_TEAM_TYPE_COMBO);
			LinkControlEx(plist, "clanBattleMakeTeamComboButton", m_pBattleMakeTeamComboButton, GCI_CLAN_B_MEMBER_BATTLE_TEAM_TYPE_BUTTON);
			LinkControlEx(plist, "clanBattleMakeTeamComboPopup", m_pBattleMakeTeamComboPopup, GCI_CLAN_P_MEMBER_BATTLE_TEAM_TYPE_POPUP);

			LinkControlEx(plist, "clanBattleMakeTeamScrollList", m_pBattleTeamScrollList, GCI_CLAN_L_MEMBER_BATTLE_TEAM_LIST_SCROLL);

			for(INT32 i=0; i<CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX; i++)
			{
				sprintf_s(strName, "clanBattleMakeTeamSlot%d", i+1);
				LinkControlEx(plist,  strName, m_slotBattleTeamList[i].pSlot, GCI_UNKNOWN);

				sprintf_s(strName, "clanBattleMakeTeamName%d", i+1);
				LinkControlEx(plist,  strName, m_slotBattleTeamList[i].pTeamName, GCI_UNKNOWN);

				sprintf_s(strName, "clanBattleMakeTeamLocale%d", i+1);
				LinkControlEx(plist,  strName, m_slotBattleTeamList[i].pLocale, GCI_UNKNOWN);

				sprintf_s(strName, "clanBattleMakeTeamState%d", i+1);
				LinkControlEx(plist,  strName, m_slotBattleTeamList[i].pState, GCI_UNKNOWN);

				sprintf_s(strName, "clanBattleMakeTeamLeaderValue%d", i+1);
				LinkControlEx(plist,  strName, m_slotBattleTeamList[i].pLeaderValue, GCI_UNKNOWN);

				sprintf_s(strName, "clanBattleMakeTeamLeaderRank%d", i+1);
				LinkControlEx(plist,  strName, m_slotBattleTeamList[i].pLeaderRank, GCI_UNKNOWN);

				sprintf_s(strName, "clanBattleMakeTeamLeaderNick%d", i+1);
				LinkControlEx(plist,  strName, m_slotBattleTeamList[i].pLeaderNick, GCI_UNKNOWN);

				sprintf_s(strName, "clanBattleMakeTeamMatchType%d", i+1);
				LinkControlEx(plist,  strName, m_slotBattleTeamList[i].pMatchType, GCI_UNKNOWN);

				sprintf_s(strName, "clanBattleMakeTeamMatchMember%d", i+1);
				LinkControlEx(plist,  strName, m_slotBattleTeamList[i].pEntryMember, GCI_UNKNOWN);

				sprintf_s(strName, "clanBattleMakeTeamJoin%d", i+1);
				LinkControlEx(plist,  strName, m_slotBattleTeamList[i].pJoinButton, GCI_CLAN_L_MEMBER_BATTLE_TEAM_JOIN1 + i);
			}
		}

		//	Ŭ�� ã��
		{
			LinkControlEx(plist,  "clanSearchClanSearchStatic", m_pSearchStatic, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanSearchInput", m_pSearchInput, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanSearchButton", m_pSearchButton, GCI_CLAN_B_MEMBER_SEARCH_SEARCHING);
			LinkControlEx(plist,  "clanSearchClanListViewAll", m_pSearchViewAll, GCI_CLAN_B_MEMBER_SEARCH_VIEW_ALL);

			LinkControlEx(plist,  "clanMemberSerchText", m_pClanMembeSerchText, GCI_UNKNOWN);

			LinkControlEx(plist,  "clanSerchText", m_pClanSerchText, -1);		
			LinkControlEx(plist,  "clanSerchGage", m_pClanSerchGage, -1);
			LinkControlEx(plist,  "clanSerchBar", m_pClanSerchBar, -1);

			LinkControlEx(plist,  "clanSearchAddressBullet", m_pSearchInfoAddressBullet, GCI_UNKNOWN);

			LinkControlEx(plist,  "clanSearchClanMark", m_pSearchInfoClanMarkImage, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanNameStatic", m_pSearchInfoNameStatic, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanNameValue", m_pSearchInfoNameValue, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanMasterStatic", m_pSearchInfoMasterStatic, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanMasterValue", m_pSearchInfoMasterValue, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanMasterRankImage", m_SearchInfoMasterRankImage, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanRankStatic", m_pSearchInfoClanRankStatic, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanRankValue", m_pSearchInfoClanRankValue, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanRankImage", m_SearchInfoClanRankImage, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanStaffStatic", m_pSearchInfoStaffStatic, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanStaffValue", m_pSearchInfoStaffValue, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchAddressStatic", m_pSearchInfoAddressStatic, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchAddressValue", m_pSearchInfoAddressValue, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanOrderStatic", m_pSearchInfoOrderStatic, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanOrderValue", m_pSearchInfoOrderValue, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanExpStatic", m_pSearchInfoExpStatic, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanExpValue", m_pSearchInfoExpValue, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanExpBar", m_SearchInfoExpBar, GCI_UNKNOWN);

			LinkControlEx(plist,  "clanSearchClanListHeadRank", m_pSearchClanListHeadRank, GCI_CLAN_E_MEMBER_SEARCH_HEADER_RANK);
			LinkControlEx(plist,  "clanSearchClanListHeadName", m_pSearchClanListHeadName, GCI_CLAN_E_MEMBER_SEARCH_HEADER_NAME);
			LinkControlEx(plist,  "clanSearchClanListHeadDate", m_pSearchClanListHeadDate, GCI_CLAN_E_MEMBER_SEARCH_HEADER_DATE);
			LinkControlEx(plist,  "clanSearchClanListHeadMember", m_pSearchClanListHeadMember, GCI_CLAN_E_MEMBER_SEARCH_HEADER_MEMBER);

			LinkControlEx(plist,  "clanSearchClanListScrollList", m_pSearchClanListScroll, GCI_CLAN_L_MEMBER_SEARCH_SCROLL);
			LinkControlEx(plist,  "clanSearchClanListFocus", m_pSearchClanListFocus, GCI_UNKNOWN);
			LinkControlEx(plist,  "clanSearchClanListSelect", m_pSearchClanListSelect, GCI_UNKNOWN);

			for(INT32 i=0; i<CLAN_SEARCH_LIST_SLOT_MAX; i++)
			{
				sprintf_s(strName, "clanSearchClanListSlot%d", i+1);
				LinkControlEx(plist,  strName, m_ClanListSlot[i]._pSlot, GCI_CLAN_S_MEMBER_SEARCH_SLOT1 + i);

				sprintf_s(strName, "clanSearchClanListRank%d", i+1);
				LinkControlEx(plist,  strName, m_ClanListSlot[i]._pRankImage, GCI_UNKNOWN);

				sprintf_s(strName, "clanSearchClanLisClanName%d", i+1);
				LinkControlEx(plist,  strName, m_ClanListSlot[i]._pName, GCI_UNKNOWN);

				sprintf_s(strName, "clanSearchClanListClanMarkImage%d", i+1);
				LinkControlEx(plist,  strName, m_ClanListSlot[i]._pClanMarkImage, GCI_UNKNOWN);

				sprintf_s(strName, "clanSearchClanListEntryDay%d", i+1);
				LinkControlEx(plist,  strName, m_ClanListSlot[i]._pDate, GCI_UNKNOWN);

				sprintf_s(strName, "clanSearchClanListEntryMember%d", i+1);
				LinkControlEx(plist,  strName, m_ClanListSlot[i]._pMember, GCI_UNKNOWN);
			}
		}

		//	Ŭ������ ��
		{
			LinkControlEx(plist, "clanManagementWritingBoard", m_pManagementWritingBoardButton, GCI_CLAN_B_MEMBER_MANAGEMENT_WRITING_BOARD);
			LinkControlEx(plist, "clanManagementMember", m_pManagementMemberButton, GCI_CLAN_B_MEMBER_MANAGEMENT_MEMBER);
			LinkControlEx(plist, "clanManagementSubscription", m_pManagementSubscriptionButton, GCI_CLAN_B_MEMBER_MANAGEMENT_SUBSCRIPTION);
			LinkControlEx(plist, "clanManagementOthers", m_pManagementOthersButton, GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS);

			//	�Խñ� ���� ������		
			{
				LinkControlEx(plist, "clanManagementWritingBoardPhase", m_pManagementWritingBoardPhase, GCI_CLAN_S_MEMBER_MANAGEMENT_WRITING_BOARD_PHASE);

				LinkControlEx(plist, "clanManagementWritingBoardNoticeStatic", m_pManagementWritingBoardNoticeStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementWritingBoardNoticeTextBox", m_pManagementWritingBoardNoticeEdit, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementWritingBoardGuideStatic", m_pManagementWritingBoardGuideStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementWritingBoardGuideTextBox", m_pManagementWritingBoardGuideEdit, GCI_UNKNOWN);

				LinkControlEx(plist, "clanManagementWritingBoardNoticeModify", m_pManagementWritingBoardNoticeModify, GCI_CLAN_B_MEMBER_MANAGEMENT_BOARD_NOTICE_MODIFY);
				LinkControlEx(plist, "clanManagementWritingBoardGuideModify", m_pManagementWritingBoardGuideModify, GCI_CLAN_B_MEMBER_MANAGEMENT_BOARD_GUIDE_MODIFY);
			}

			//	Ŭ���� ���� ������		
			{
				LinkControlEx(plist, "clanManagementMemberPhase", m_pManagementMemberPhase, GCI_CLAN_S_MEMBER_MANAGEMENT_MEMBER_PHASE);

				LinkControlEx(plist, "clanManagementMemberStatic", m_pManagementMemberStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementInstructionStatic", m_pManagementInstructionStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementInstructionCombo", m_pManagementInstructionCombo, GCI_CLAN_E_MEMBER_MANAGEMENT_LIST_COMBO);
				LinkControlEx(plist, "clanManagementInstructionButton", m_pManagementInstructionButton, GCI_CLAN_B_MEMBER_MANAGEMENT_LIST_BUTTON);
				LinkControlEx(plist, "clanManagementInstructionPopup", m_pManagementInstructionPopup, GCI_CLAN_P_MEMBER_MANAGEMENT_LIST_POPUP);

				LinkControlEx(plist, "clanManagementListHeadNick", m_pManagementListHeadNick, GCI_CLAN_E_MEMNER_MANAGEMENT_LIST_HEADER_NAME);
				LinkControlEx(plist, "clanManagementListHeadClass", m_pManagementListHeadClass, GCI_CLAN_E_MEMNER_MANAGEMENT_LIST_HEADER_LEVEL);
				LinkControlEx(plist, "clanManagementListHeadDay", m_pManagementListHeadEntryDay, GCI_CLAN_E_MEMNER_MANAGEMENT_LIST_HEADER_DATE);
				LinkControlEx(plist, "clanManagementListHeadCheckAll", m_pManagementListHeadCheckAll, GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_ALL);
				LinkControlEx(plist, "clanManagementListScrollList", m_pManagementMemberScrollList, GCI_CLAN_L_MEMBER_MANAGEMENT_LIST_SCROLL); 

				for(INT32 i=0; i<MANAGEMENT_LIST_ROW_MAX; i++)
				{
					sprintf_s(strName, "clanManagementListSlot%d", i + 1);
					LinkControlEx(plist, strName, m_slotManagementList[i].pSlot, GCI_UNKNOWN);

					sprintf_s(strName, "clanManagementListRank%d", i + 1);
					LinkControlEx(plist, strName, m_slotManagementList[i].pRank, GCI_UNKNOWN);

					sprintf_s(strName, "clanManagementListTitle%d", i + 1);
					LinkControlEx(plist, strName, m_slotManagementList[i].pTitle, GCI_UNKNOWN);

					sprintf_s(strName, "clanManagementListNick%d", i + 1);
					LinkControlEx(plist, strName, m_slotManagementList[i].pNick, GCI_UNKNOWN);

					sprintf_s(strName, "clanManagementListClass%d", i + 1);
					LinkControlEx(plist, strName, m_slotManagementList[i].pClass, GCI_UNKNOWN);

					sprintf_s(strName, "clanManagementListDay%d", i + 1);
					LinkControlEx(plist, strName, m_slotManagementList[i].pEntryDay, GCI_UNKNOWN);

					sprintf_s(strName, "clanManagementListCheck%d", i + 1);
					LinkControlEx(plist, strName, m_slotManagementList[i].pCheck, GCI_CLAN_C_MEMBER_MANAGEMENT_LIST_CHECK_1 + i);
				}
			}

			//	���� ���� ������		
			{
				LinkControlEx(plist, "clanManagementSubscriptionPhase", m_pManagementSubscriptionPhase, GCI_CLAN_S_MEMBER_MANAGEMENT_SUBSCRIPTION_PHASE);

				LinkControlEx(plist, "clanManagementSubscriptionStatic", m_pManagementSubscriberStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanSubscriptionInstructionStatic", m_pSubscriptionInstructionStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanSubscriptionInstructionCombo", m_pSubscriptionInstructionCombo, GCI_CLAN_E_MEMBER_SUBSCRIPTION_LIST_COMBO);
				LinkControlEx(plist, "clanSubscriptionInstructionButton", m_pSubscriptionInstructionButton, GCI_CLAN_B_MEMBER_SUBSCRIPTION_LIST_BUTTON);
				LinkControlEx(plist, "clanSubscriptionInstructionPopup", m_pSubscriptionInstructionPopup, GCI_CLAN_P_MEMBER_SUBSCRIPTION_LIST_POPUP);

				LinkControlEx(plist, "clanSubscriptionListHeadNick", m_pSubscriberListHeadNick, GCI_CLAN_E_MEMBER_SUBSCRIPTION_LIST_HEADER_NAME);
				LinkControlEx(plist, "clanSubscriptionListHeadDay", m_pSubscriberListHeadEntryDay, GCI_CLAN_E_MEMBER_SUBSCRIPTION_LIST_HEADER_DATE);
				LinkControlEx(plist, "clanSubscriptionListHeadCheckAll", m_pSubscriberListHeadCheckAll, GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_ALL);
				LinkControlEx(plist, "clanSubscriptionListScrollList", m_pManagementSubscriberScrollList, GCI_CLAN_L_MEMBER_SUBSCRIPTION_LIST_SCROLL); 

				LinkControlEx(plist, "clanSubscriptionListFocus", m_pManagementSubscriberFocus, -1);
				LinkControlEx(plist, "clanSubscriptionListSelection", m_pManagementSubscriberSelection, -1);	

				for(INT32 i=0; i<SUBSCRIPTION_LIST_ROW_MAX; i++)
				{
					sprintf_s(strName, "clanSubscriptionListSlot%d", i + 1);
					LinkControlEx(plist, strName, m_slotSubscriptionList[i].pSlot, GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_SLOT_1 + i);

					sprintf_s(strName, "clanSubscriptionListRank%d", i + 1);
					LinkControlEx(plist, strName, m_slotSubscriptionList[i].pRank, GCI_UNKNOWN);

					sprintf_s(strName, "clanSubscriptionListTitle%d", i + 1);
					LinkControlEx(plist, strName, m_slotSubscriptionList[i].pTitle, GCI_UNKNOWN);

					sprintf_s(strName, "clanSubscriptionListNick%d", i + 1);
					LinkControlEx(plist, strName, m_slotSubscriptionList[i].pNick, GCI_UNKNOWN);

					sprintf_s(strName, "clanSubscriptionListDay%d", i + 1);
					LinkControlEx(plist, strName, m_slotSubscriptionList[i].pEntryDay, GCI_UNKNOWN);

					sprintf_s(strName, "clanSubscriptionListCheck%d", i + 1);
					LinkControlEx(plist, strName, m_slotSubscriptionList[i].pCheck, GCI_CLAN_C_MEMBER_SUBSCRIPTION_LIST_CHECK_1 + i);
				}
			}

			//	��Ÿ ���� ������		
			{
				LinkControlEx(plist, "clanManagementOthersPhase", m_pManagementOthersPhase, GCI_CLAN_S_MEMBER_MANAGEMENT_OTHERS_PHASE);

				LinkControlEx(plist, "clanManagementOthersAuthorityStatic", m_pManagementOthersAuthorityStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementOthersWritingAuthorityStatic", m_pManagementOthersWritingAuthorityStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementOthersInviteAuthorityStatic", m_pManagementOthersInviteAuthorityStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementOthersDimissalAuthorityStatic", m_pManagementOthersDimissalAuthorityStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementOthersSubscriptionAuthorityStatic", m_pManagementOthersSubscriptionAuthorityStatic, GCI_UNKNOWN);

				LinkControlEx(plist, "clanManagementOthersWritingAuthorityCheck", m_pManagementOthersWritingAuthorityCheck, GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_WRITING_AUTHORITY_CHECK);
				LinkControlEx(plist, "clanManagementOthersInviteAuthorityCheck", m_pManagementOthersInviteAuthorityCheck, GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_INVITE_AUTHORITY_CHECK);
				LinkControlEx(plist, "clanManagementOthersDimissalAuthorityCheck", m_pManagementOthersDimissalAuthorityCheck, GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_DISMISSAL_AUTHORITY_CHECK);
				LinkControlEx(plist, "clanManagementOthersSubscriptionAuthorityCheck", m_pManagementOthersSubscriptionAuthorityCheck, GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_SUBSCRIPTION_AUTHORITY_CHECK);
                
				LinkControlEx(plist, "clanManagementOthersSendNoteStatic", m_pManagementOthersSendNoteStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementOthersSendNoteButton", m_pManagementOthersSendNoteButton, GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS_SEND_NOTE);

				LinkControlEx(plist, "clanManagementOthersAdmissionLimitStatic", m_pManagementOthersAdmissionLimitStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementOthersAdmissionLimitRankStatic", m_pManagementOthersAdmissionLimitRankStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementOthersAdmissionLimitRankCombo", m_pManagementOthersAdmissionLimitRankCombo, GCI_CLAN_E_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_RANK_COMBO);
				LinkControlEx(plist, "clanManagementOthersAdmissionLimitRankButton", m_pManagementOthersAdmissionLimitRankButton, GCI_CLAN_P_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_RANK_POPUP);
				LinkControlEx(plist, "clanManagementOthersAdmissionLimitRankPopup", m_pManagementOthersAdmissionLimitRankPopup, GCI_CLAN_L_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_RANK_SCROLL);
				LinkControlEx(plist, "clanManagementOthersAdmissionLimitAgeStatic", m_pManagementOthersAdmissionLimitAgeStatic, GCI_UNKNOWN);

				for(INT32 i=0; i<ADMISSION_LIMIT_CONTROL_COUNT; i++)
				{
					char temp[256] = "";

					sprintf_s(temp, "clanManagementOthersAdmissionLimitAgeCombo%d", i+1);

					LinkControlEx(plist, temp, m_pManagementOthersAdmissionLimitAgeCombo[i], 
								GCI_CLAN_E_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_AGE_COMBO1 + (i * 3));

					sprintf_s(temp, "clanManagementOthersAdmissionLimitAgeButton%d", i+1);

					LinkControlEx(plist, temp, m_pManagementOthersAdmissionLimitAgeButton[i], 
								GCI_CLAN_P_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_AGE_POPUP1 + (i * 3));

					sprintf_s(temp, "clanManagementOthersAdmissionLimitAgePopup%d", i+1);

					LinkControlEx(plist, temp, m_pManagementOthersAdmissionLimitAgePopup[i], 
								GCI_CLAN_L_MEMBER_MANAGEMENT_OTHERS_ADMISSION_LIMIT_AGE_SCROLL1 + (i * 3));
				}

				LinkControlEx(plist, "clanManagementOthersChangeClanNameStatic", m_pManagementOthersChangeClanNameStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementOthersChangeClanNameButton", m_pManagementOthersChangeClanNameButton, GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS_CHANGE_CLAN_NAME);

				LinkControlEx(plist, "clanManagementOthersChangeClanMarkStatic", m_pManagementOthersChangeClanMarkStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementOthersChangeClanMarkButton", m_pManagementOthersChangeClanMarkButton, GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS_CHANGE_CLAN_MARK);

				LinkControlEx(plist, "clanManagementOthersDissolutionStatic", m_pManagementOthersDissolutionStatic, GCI_UNKNOWN);
				LinkControlEx(plist, "clanManagementOthersDissolutionButton", m_pManagementOthersDissolutionButton, GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS_DISSOLUTION);

				LinkControlEx(plist, "clanManagementOthersModifyButton", m_pManagementOthersModifyButton, GCI_CLAN_B_MEMBER_MANAGEMENT_OTHERS_MODIFY);
			}
		}
	}

	return FALSE;
}

void CStepClanMember::OnInitControl(void)
{
	m_pClanMemberRoot->SetEnable(FALSE);

	g_pszAdmissionLimitRankItem[0] = GAME_STRING("STBL_IDX_CLAN_RANK_UNLIMIT");	//	"��ü ���",
	g_pszAdmissionLimitRankItem[1] = GAME_STRING("STBL_IDX_CLAN_RANK_LIMIT_1");	//	"���� �̻�",
	g_pszAdmissionLimitRankItem[2] = GAME_STRING("STBL_IDX_CLAN_RANK_LIMIT_2");	//	"���� �̻�",
	g_pszAdmissionLimitRankItem[3] = GAME_STRING("STBL_IDX_CLAN_RANK_LIMIT_3");	//	"�ҷ� �̻�",
	g_pszAdmissionLimitRankItem[4] = GAME_STRING("STBL_IDX_CLAN_RANK_LIMIT_4");	//	"���� �̻�",

	g_pszAdmissionLimitAgeLowItem[0] = GAME_STRING("STBL_IDX_CLAN_AGE_UNLIMIT");	//	"���� ����",
	g_pszAdmissionLimitAgeLowItem[1] = GAME_STRING("STBL_IDX_CLAN_AGE_UPPER_1");	//	"15�� �̻�",
	g_pszAdmissionLimitAgeLowItem[2] = GAME_STRING("STBL_IDX_CLAN_AGE_UPPER_2");	//	"20�� �̻�",
	g_pszAdmissionLimitAgeLowItem[3] = GAME_STRING("STBL_IDX_CLAN_AGE_UPPER_3");	//	"30�� �̻�",

	g_pszAdmissionLimitAgeHighItem[0] = GAME_STRING("STBL_IDX_CLAN_AGE_UNLIMIT");//	"���� ����",
	g_pszAdmissionLimitAgeHighItem[1] = GAME_STRING("STBL_IDX_CLAN_AGE_LOWER_1");//	"15�� ����",
	g_pszAdmissionLimitAgeHighItem[2] = GAME_STRING("STBL_IDX_CLAN_AGE_LOWER_2");//	"20�� ����",
	g_pszAdmissionLimitAgeHighItem[3] = GAME_STRING("STBL_IDX_CLAN_AGE_LOWER_3");//	"30�� ����",

	//	ä��
	m_pChatBody->InitChatBody( CHAT_BODY_STATE_CLAN_MEMBER, MAX_CHAT_LINE_FOR_CLAN_MEMNER);

	if ( ! g_pConfigEx->GetClan().Enable_Chat )
	{
		m_pChatBody->m_pTextBox->setInputDisable(TRUE);
		m_pChatBody->m_pChatList->setInputDisable(TRUE);
		m_pChatBody->m_pChatBox->setInputDisable(TRUE);
		m_pChatBody->m_pChatCombo->setInputDisable(TRUE);
		m_pChatBody->m_pChatPopup->setInputDisable(TRUE);
		m_pChatBody->m_pChatButton->setInputDisable(TRUE);

		for(INT32 i=0; i<CHAT_CHANNEL_BUTTON_COUNT; i++)
		{
			m_pChatBody->m_pChatChannelButton[i]->setInputDisable(TRUE);
		}
	}

	m_pClanSerchGage->SetAllShapeEnable(FALSE);
	m_pClanSerchText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanSerchText->SetText("");	

	_InitMyClanWin();
	_InitClanManagementWin();

	// Ŭ�� ���� ���� ����
	if( g_pConfigEx->GetClan().Enable_AdmissionForAge == FALSE)
	{
		m_pManagementOthersAdmissionLimitAgeStatic->SetEnable(FALSE);

		for(INT32 i=0; i<ADMISSION_LIMIT_CONTROL_COUNT; i++)
		{
			m_pManagementOthersAdmissionLimitAgeCombo[i]->SetEnable( FALSE);
			m_pManagementOthersAdmissionLimitAgeButton[i]->SetEnable( FALSE);
			m_pManagementOthersAdmissionLimitAgePopup[i]->SetEnable( FALSE);
		}
	}

	CStepBase::OnInitControl();
}

void CStepClanMember::EnterServer(void)
{
	if( !m_pClanContext->IsEnter())
		m_pClanContext->EnterServer();
}

void CStepClanMember::LeaveServer(void)
{
	if( m_pClanContext->IsEnter())
	{
		m_pClanContext->LeaveServer();

		m_listManageMember.Clear();
		m_listManageSubscriber.Clear();

		i3mem::FillZero( m_ManageMemberInfo, sizeof(m_ManageMemberInfo));
		i3mem::FillZero( m_ManageSubscriberInfo, sizeof(m_ManageSubscriberInfo));

		m_pClanContext->DeleteBattleTeamList();
		m_pClanContext->DeleteBattleAllTeamList();

#if defined(TEST_CLAN_BATTLE)
		I3MEM_SAFE_FREE(g_pTestTeamInfoList);
#endif
	}
}

void CStepClanMember::OnLeave(void)
{	
	((CStageReady*)m_pParent)->ChangeStep(STEP_NULL);

	//m_CurrentPage = CLAN_PAGE_UNKNOWN;
}

void CStepClanMember::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CStepBase::OnUpdate(rDeltaSeconds);

	//	�� Ŭ��
	_UpdateMyClanPhase(rDeltaSeconds);

	switch( m_CurrentPage)
	{
	case CLAN_PAGE_INFO:		_UpdateClanInfoPhase(rDeltaSeconds);		break;
	case CLAN_PAGE_BATTLE:		_UpdateClanBattlePhase(rDeltaSeconds);		break;
	case CLAN_PAGE_SEARCH:		_UpdateClanSearchPhase(rDeltaSeconds);		break;
	case CLAN_PAGE_MANAGEMENT:	_UpdateClanManagementPhase(rDeltaSeconds);	break;
	}


	//	ä��
	if ( g_pConfigEx->GetClan().Enable_Chat )
	{
		I3ASSERT( m_pChatBody != NULL);
		m_pChatBody->OnUpdate( rDeltaSeconds, this);
	}
}

void CStepClanMember::OnGameEvent( INT32 event, INT32 arg)
{
	//	�̺�Ʈ ����� �޾� ó���Ѵ�.
	switch( event )
	{
	case EVENT_CLAN_ACCEPT_REQUEST:		// ���Խ�û�� ����
	case EVENT_CLAN_DENIAL_REQUEST:		// ���Խ�û�� ����
		{
			m_idxClanManagementSubscriberStartSlot = 0;
			m_pManagementSubscriberScrollList->SetCurValue(0.0f);

			_SetSubscriptionListCheckAll(FALSE);
		}
		break;
	case EVENT_CLAN_COMMISSIONMASTER:	// ������ ����
	case EVENT_CLAN_COMMISSIONSTAFF:	// Ŭ�� ���� �Ӹ�
	case EVENT_CLAN_COMMISSION_REGULAR:	// Ŭ�� �Ϲ����� ����
	case EVENT_CLAN_DISMISSALMEMBER:	// Ŭ���� ����
		{
			m_idxClanManagementMemberStartSlot = 0;
			m_pManagementMemberScrollList->SetCurValue(0.0f);

			_SetManagementListCheckAll(FALSE);			
		}
		break;
	case EVENT_CLAN_MEMBER_LIST:
		{
			m_listManageMember.Clear();			//	���� update�� ����Ʈ�� �籸���ϵ���
		}
		break;
	//////////////////////////////////////////////////////////////////////////////////////////////
	//	Ŭ���� �̺�Ʈ
	//////////////////////////////////////////////////////////////////////////////////////////////
	case EVENT_MATCH_TEAM_LIST_N:
		{
			m_bUpdateTeamList = TRUE;
		}
		break;
	case EVENT_MATCH_TEAM_CREATE:		//	Ŭ�� �� ����
		{
			if( EV_SUCCESSED(arg))
			{
				//	�� ����� �Ǿ����Ƿ� �κ�� �Ѿ��.
				((CStageReady*)m_pParent)->ChangeStep(STEP_CLAN_BATTLE_LOBBY);

				m_pClanContext->DeleteBattleTeamList();
			}
			else
			{
				switch(arg)
				{
				case EVENT_ERROR_CLAN_BATTLE_TEAM_HAVE_FAIL:
					//	"�̹� ���� ���� �ֽ��ϴ�.\n ���� ����Ϸ��� ������ Ż���ؾ� �մϴ�."
					STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_ALREADY_TEAM"));
					break;
				case EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_FULL_FAIL:
					//	"������ ���� ����á���ϴ�.\n ���� ���̻� ����� �� �����ϴ�."
					STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_FULL_TEAM"));
					break;
				case EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_PER_FAIL:
					//	"����Ϸ��� �� �ο��� �߸��Ǿ����ϴ�.\n �ٽ� �õ��� �ּ���."
					STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_INVALID_TEAM"));
					break;
				case EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_NOTEMPTY_FAIL:
				default:
					//	"��Ʈ��ũ ������ ���� ������� ���߽��ϴ�.\n �ٽ� �õ��� �ּ���."
					STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_FAILED_TEAM"));
					break;
				}
			}
		}
		break;
	case EVENT_MATCH_TEAM_JOIN:			//	Ŭ�� �� ����
		{
			if( EV_SUCCESSED(arg))
			{
				//	���� �����Ͽ����Ƿ� �κ�� �Ѿ��.
				((CStageReady*)m_pParent)->ChangeStep(STEP_CLAN_BATTLE_LOBBY);

				m_pClanContext->DeleteBattleTeamList();
			}
			else
			{
				switch( arg)
				{				
				case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_NOSEARCH_FAIL:
				case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_EMPTY_FAIL:
					//	"�� ������ �����߽��ϴ�.\n �߸��� ���Դϴ�."
					STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_FAIL_JOIN_TEAM_BY_INVALID"));
					break;
				case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_PERMAX_FAIL:
					//	"���� ������ ��� á�� ������ �ش� ���� ������ �� �����ϴ�."
					STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_FAIL_JOIN_TEAM_BY_FULL"));
					break;	
				default:
					//	"��Ʈ��ũ ������ ���� �������� ���߽��ϴ�.\n �ٽ� �õ��� �ּ���."
					STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_FAIL_JOIN_TEAM_BY_SERVER"));
					break;
				}
			}
		}
		break;
	}
}

BOOL CStepClanMember::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		I3ASSERT(pObj != NULL);
		i3InputDeviceManager * pMng = (i3InputDeviceManager*) pObj;
		i3InputKeyboard * pKeyboard = pMng->GetKeyboard();

		if( (pObj != NULL) && pObj->IsExactTypeOf(CContextMenu::GetClassMeta()))
		{
			INT32 memberIdx = param2;

			switch(event)
			{
			case CMC_WHISPER:		OnWhisper(memberIdx);	break;
			case CMC_INVITE:		OnInviteRoom(memberIdx);	break;
			case CMC_VISIT:			OnVisit(memberIdx);		break;
			case CMC_ADD_FRIEND:	OnAddFriend(memberIdx);	break;
			default:
				return FALSE;
			}

			return TRUE;
		}

		//	Ŭ�� �˻��϶�
		if( m_pSearchInput->getIsFocus())
		{
			if(pKeyboard->GetStrokeState(I3I_KEY_ENTER))
			{
				OnSearchClanKeyword();
				return TRUE;
			}
		}

		//	ä�� ó��
		if( m_pClanMemberRoot->isEnable() &&
			!((CStageReady*)this->getParent())->IsEnablePopup( POPUP_OPTION) &&
			!((CStageReady*)this->getParent())->IsEnablePopup( POPUP_COMMUNITY) )
		{
			// ä�� Ÿ��: ��ü, ��
			m_pChatBody->UpdateChatType();

			//	��Ƽ������ �ԷµǴ� ����Ʈ�ڽ������� ä�� ��Ŀ�� ����
			if( !m_pManagementWritingBoardNoticeEdit->getIsFocus() &&
				!m_pManagementWritingBoardGuideEdit->getIsFocus())
			{
				// ���ͽ� ��Ŀ�� ó��
				m_pChatBody->UpdateChatFocus();
			}
		}
	}

	I3ASSERT(pObj != NULL);
	return CStepBase::OnEvent(event, pObj, param2, code);
}

void CStepClanMember::OnStart(void)
{
	CStepBase::OnStart();

	//	���� ������
	switch(m_CurrentPage)
	{
	case CLAN_PAGE_INFO:		OnClanInfoTab();		break;
	case CLAN_PAGE_BATTLE:
		{
			//	Ŭ�� �����̸�
			if(g_pGameContext->GetConnectedChannelType() == CHANNEL_TYPE_CLAN)
			{
				OnClanBattleTab();
			}
			else
			{
				OnClanInfoTab();
			}
		}
		break;
	case CLAN_PAGE_SEARCH:		OnClanSearchTab();		break;
	case CLAN_PAGE_MANAGEMENT:
		{
			//	������ �Ǵ� ���ܸ� Ŭ�� ���� ���� ���δ�.
			if( m_pClanContext->GetMyStaffLevel() == CLAN_MEMBER_LEVEL_MASTER ||
				m_pClanContext->GetMyStaffLevel() == CLAN_MEMBER_LEVEL_STAFF )
			{
				OnClanManagementTab();
			}
			else
			{
				OnClanInfoTab();
			}
		}
		break;
	default:	OnClanInfoTab();		break;
	}

	m_pChatBody->SetEntranceStart(this);

	m_pClanContext->SetInClanBattleLobby(FALSE);

	SetPushedStateButton(MBT_CLAN, TRUE);
}

void CStepClanMember::OnEnd(void)
{
	CStepBase::OnEnd();

	m_pChatBody->OnExitEnd();

	SetPushedStateButton(MBT_CLAN, FALSE);
}

void CStepClanMember::OnInitRun(void)
{
	CStepBase::OnInitRun(); 

	MoveCamera(CL_RECORD); 
}

BOOL CStepClanMember::OnSliding(RT_REAL32 rDeltaSeconds)
{
	if (FALSE == CStepBase::OnSliding(rDeltaSeconds))
	{
		return FALSE;
	}

	// �ð��� ���� ������ġ ����
	m_pClanMemberRoot->setPositionY(CStepBase::GetSlideY());

	// ���� ��, ��Ȱ��ȭ. �׿� Ȱ��ȭ
	BOOL enableWindow = TRUE;

	if (SS_OUT == CStepBase::GetSlideState())
	{
		enableWindow = FALSE;
	}

	m_pClanMemberRoot->SetEnable(enableWindow);

	return TRUE;
}

void CStepClanMember::OnExitKey(void)
{
	OnLeave();
}

void CStepClanMember::OnWheelUpdate(INT32 scrollby)
{
	if (0 < scrollby)
	{
		for(INT32 i = 0; i < scrollby; i++)
		{
			if( m_pChatBody->m_pChatList->getIsFocus())
				m_pChatBody->m_pChatList->MovePrev();

			if( m_pInfoMemberPhase->isEnable())
				m_pClanMemberScrollList->MovePrev();

			if( m_pBattlePhase->isEnable())
				m_pBattleTeamScrollList->MovePrev();

			if( m_pSearchPhase->isEnable())
				m_pSearchClanListScroll->MovePrev();

			if( m_pManagementMemberPhase->isEnable())
				m_pManagementMemberScrollList->MovePrev();

			if( m_pManagementSubscriptionPhase->isEnable())
				m_pManagementSubscriberScrollList->MovePrev();
		}
	}
	else
	{
		for(INT32 i = 0; i > scrollby; i--)
		{
			if( m_pChatBody->m_pChatList->getIsFocus())
				m_pChatBody->m_pChatList->MoveNext();

			if( m_pInfoMemberPhase->isEnable())
				m_pClanMemberScrollList->MoveNext();

			if( m_pBattlePhase->isEnable())
				m_pBattleTeamScrollList->MoveNext();

			if( m_pSearchPhase->isEnable())
				m_pSearchClanListScroll->MoveNext();

			if( m_pManagementMemberPhase->isEnable())
				m_pManagementMemberScrollList->MoveNext();

			if( m_pManagementSubscriptionPhase->isEnable())
				m_pManagementSubscriberScrollList->MoveNext();
		}			
	}	

	if( m_pInfoMemberPhase->isEnable())
		m_idxClanInfoMemberStartSlot = m_pClanMemberScrollList->GetCurrentTop();

	if( m_pBattlePhase->isEnable())
	{
		OnClanBattleTeamListScroll();		
	}

	if( m_pSearchPhase->isEnable())
	{
		OnSearchClanListScroll();
	}	

	if( m_pManagementMemberPhase->isEnable())
		m_idxClanManagementMemberStartSlot = m_pManagementMemberScrollList->GetCurrentTop();

	if( m_pManagementSubscriptionPhase->isEnable())
		m_idxClanManagementSubscriberStartSlot = m_pManagementSubscriberScrollList->GetCurrentTop();

	if( m_pChatBody->m_pChatList->getIsFocus())
	{
		g_pFramework->getChatBox()->CalcBackupBufferByRatio(m_pChatBody->m_pChatList->getCurValue()*0.01f, MAX_CHAT_LINE_FOR_CLAN_MEMNER);	
	}
}

void CStepClanMember::SetInputDisable(BOOL bDisable)
{
	m_pClanMemberRoot->setInputDisable(bDisable);
}

BOOL CStepClanMember::GetInputDisable(void)
{
	return m_pClanMemberRoot->getInputDisable();
}

void CStepClanMember::OnClanChatPopup(INT32 idx)
{
	m_pChatBody->OnChatPopup(idx);
}

void CStepClanMember::OnClanChatType(void)
{
	ExclusionPopup(m_pChatBody->m_pChatPopup);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�� Ŭ��

void CStepClanMember::_InitMyClanWin(void)
{
	m_pMyClanWin->SetEnable(TRUE);
	
	INIT_WIN_CAPTION(m_pMyClanCaption, GAME_STRING("STBL_IDX_CLAN_MY_CLAN"));	//	"�� Ŭ��"
	
	_InitMyClanInfo();
}

void CStepClanMember::_InitMyClanInfo(void)
{
	INIT_STATIC_CAPTION(m_pMyClanNameStatic, GAME_STRING("STBL_IDX_CLAN_NAME"));		//	"Ŭ����:"
	INIT_STATIC_CAPTION(m_pMyClanStaffStatic, GAME_STRING("STBL_IDX_CLAN_PERSON"));	//	"Ŭ�� �ο���:"
	INIT_STATIC_CAPTION(m_pMyClanMasterStatic, GAME_STRING("STBL_IDX_CLAN_MASTER"));	//	"Ŭ�� ������:"
	INIT_STATIC_CAPTION(m_pMyClanRankStatic, GAME_STRING("STBL_IDX_CLAN_RANK"));		//	"Ŭ�� ���:"
	INIT_STATIC_CAPTION(m_pMyClanExpStatic, GAME_STRING("STBL_IDX_CLAN_EXP"));		//	"Ŭ�� ����ġ:"
	INIT_STATIC_CAPTION(m_pMyClanOrderStatic, GAME_STRING("STBL_IDX_CLAN_ORDER"));	//	"Ŭ�� ����:"
	INIT_STATIC_CAPTION(m_pMyClanAddressStatic, GAME_STRING("STBL_IDX_CLAN_AZIT"));	//	"Ŭ�� ����Ʈ:"
	INIT_STATIC_CAPTION(m_pMyClanPointStatic, GAME_STRING("STBL_IDX_CLAN_POINT"));	//	"Ŭ�� ����Ʈ:"
		
	INIT_VALUE_CAPTION(m_pMyClanNameValue, "");
	INIT_VALUE_CAPTION(m_pMyClanStaffValue, "");
	INIT_VALUE_CAPTION(m_pMyClanMasterValue, "");
	INIT_VALUE_CAPTION(m_pMyClanRankValue, "");
	INIT_VALUE_CAPTION(m_pMyClanExpValue, "");
	INIT_VALUE_CAPTION(m_pMyClanOrderValue, "");
	INIT_VALUE_CAPTION(m_pMyClanAddressValue, "");
	INIT_VALUE_CAPTION(m_pMyClanPointValue, "");

	m_pMyClanAddressValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);

	I3COLOR color;
	i3Color::Set(&color, (UINT8) 255, 125, 0, 255);
	m_pMyClanAddressValue->SetTextColor(&color);
	m_pMyClanPointValue->SetTextColor(&color);

#ifndef CLAN_ADDRESS_ENABLE
	{
		m_pMyClanAddressBullet->SetEnable( FALSE);
		m_pMyClanAddressStatic->SetEnable( FALSE);
		m_pMyClanAddressValue->SetEnable(FALSE);
	}
#endif
}

void CStepClanMember::SetMyClanName(char * pszName, UINT8 cColor)
{
	SetNameUsingColorTable( m_pMyClanNameValue, pszName, cColor );
}

void CStepClanMember::SetMyClanStaff(INT32 nNum, INT32 nTotal)
{
	SetClanStaff(m_pMyClanStaffValue, nNum, nTotal);
}

void CStepClanMember::SetMyClanMaster(char * pszName, UINT8 nRank)
{
	SetClanMaster(m_pMyClanMasterValue, m_pMyClanMasterRankImage, pszName, nRank);
}

void CStepClanMember::SetMyClanRank(UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank)
{
	SetClanRank(m_pMyClanRankValue, m_pMyClanRankImage, nClanExp, nClanStaffNum, nClanRank);
}

void CStepClanMember::SetMyClanOrder(INT32 nCurOrder, INT32 nPrevOrder)
{
	SetClanOrder(m_pMyClanOrderValue, nCurOrder, nPrevOrder);
}

void CStepClanMember::SetMyClanAddress(char * pszAddress)
{
#ifdef CLAN_ADDRESS_ENABLE
	{
		m_pMyClanAddressStatic->SetEnable( TRUE);
		m_pMyClanAddressValue->SetEnable(TRUE);

		m_pMyClanAddressValue->SetText(pszAddress);		
	}
#else
	{
		m_pMyClanAddressStatic->SetEnable( FALSE);
		m_pMyClanAddressValue->SetEnable(FALSE);
	}
#endif
}

void CStepClanMember::SetMyClanPoint(REAL32 fPoint)
{
	SetClanPoint(m_pMyClanPointValue, fPoint);
}

void CStepClanMember::SetMyClanExp(UINT32 nCurExp, UINT32 nMaxExp)
{
	REAL32 rx = ((CStageReady*)m_pParent)->GetGuiRoot()->getGuiResolutionWidth();

	SetClanExpText(m_pMyClanExpValue, nCurExp, nMaxExp);
	SetClanExpGauge(m_pMyClanExpBar, rx, nCurExp, nMaxExp);
}

void CStepClanMember::SetMyClanMark(UINT32 nFlag)
{
#if defined( USE_CLAN_MARK)
	::SetClanMarkTexEx(m_pMyClanMarkImage, CLAN_MARK_MEDIUM, nFlag);
#endif
}

void CStepClanMember::_SetBestMember(BEST_MEMBER_SLOT slot, const CLAN_MEMBER_BASIC_INFO * pSeasonMember, const CLAN_MEMBER_BASIC_INFO * pTopMember)
{	
	I3_BOUNDCHK( slot, (INT32)BEST_MEMBER_SLOT_MAX);

	if( pSeasonMember != NULL)
	{
		SetNameUsingColorTable( m_slotBestMemberList[slot].pSeasonMember, 
								pSeasonMember->_nick, pSeasonMember->_color);
	}
	else
	{
		m_slotBestMemberList[slot].pSeasonMember->SetText( "-");
		m_slotBestMemberList[slot].pSeasonMember->SetTextColor( GetColor(GCT_DEFAULT));
	}

	if( pTopMember != NULL)
	{
		SetNameUsingColorTable( m_slotBestMemberList[slot].pTopMember, 
								pTopMember->_nick, pTopMember->_color);
	}
	else
	{
		m_slotBestMemberList[slot].pTopMember->SetText( "-");
		m_slotBestMemberList[slot].pTopMember->SetTextColor( GetColor(GCT_DEFAULT));
	}
}

void CStepClanMember::_UpdateMyClanPhase(RT_REAL32 rDeltaSeconds)
{
	char temp[256] = "";
	CLAN_DETAIL_INFO * pClanInfo = m_pClanContext->GetMyClanDetailInfo();
	
	if(pClanInfo)
	{
		SetMyClanName(pClanInfo->_name, pClanInfo->_color );
		SetMyClanStaff(pClanInfo->_currentPerson, pClanInfo->_maxPerson);
		SetMyClanMaster(pClanInfo->_masterNick, pClanInfo->_masterRank);
		SetMyClanRank(pClanInfo->_exp, pClanInfo->_currentPerson, pClanInfo->_expRank); 
 		//SetMyClanOrder(pClanInfo->_clanRank, pClanInfo->_prevRank); 
		SetMyClanOrder(0, 0); 
 
		UINT32 wExp = pClanInfo->_exp;
		if( wExp > DF_CLAN_RANK_L5 ) wExp = DF_CLAN_RANK_L5;
		UINT32 BeforeExp = GetClanBeforeRankExp( wExp );
		SetMyClanExp(wExp - BeforeExp, GetClanNextRankExp(wExp) - BeforeExp);
		SetMyClanMark(pClanInfo->_mark);

		i3String::Copy( temp, g_pConfigEx->GetClan().AzitAddressBase, 256 );
		i3String::Concat(temp, pClanInfo->_azitURL);
		SetMyClanAddress(temp);
		SetMyClanPoint(pClanInfo->_clanpoint);
	}
	else
	{
		SetMyClanName("", 0);
		SetMyClanStaff(0, 0);
		SetMyClanMaster("", 0);
		SetMyClanRank(0, 0, 0);
		SetMyClanOrder(0, 0);
		SetMyClanAddress("");
		SetMyClanExp(0, 0);
		SetMyClanMark(0);
		SetMyClanPoint(0.f);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Ŭ�� ����

void CStepClanMember::OnClanInfoTab(void)
{
	m_pInfoPhase->SetEnable(TRUE);
	m_pBattlePhase->SetEnable(FALSE);
	m_pSearchPhase->SetEnable(FALSE);
	m_pManagementPhase->SetEnable(FALSE);

	m_pInfoTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pBattleTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pSearchTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pManagementTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	OnClanInfoHistoryButton();
	//OnClanInfoMemberButton();

	m_CurrentPage = CLAN_PAGE_INFO;

	//	ä�ÿ� �׻� ��Ŀ��
	//m_pMyClanChatEditBox->SetFocus(TRUE);
}

void CStepClanMember::OnClanInfoHistoryButton(void)
{
	m_pInfoHistroyPhase->SetEnable(TRUE);
	m_pInfoMemberPhase->SetEnable(FALSE);

	m_pInfoHistoryButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pInfoMemberButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
}

void CStepClanMember::OnClanInfoMemberButton(void)
{
	m_pInfoHistroyPhase->SetEnable(FALSE);
	m_pInfoMemberPhase->SetEnable(TRUE);

	m_pInfoHistoryButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pInfoMemberButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_idxClanInfoMemberStartSlot = 0;
	m_pClanMemberScrollList->SetCurValue(0.0f);

	i3mem::FillZero( m_ManageMemberInfo, sizeof(m_ManageMemberInfo));
	m_listManageMember.Clear();
	m_listManageMember.SetCompareProc(cbSortInfoMemberListByState);
	m_sInfoClanMemberSortType = CLAN_SORT_STATE_DESC;

	//	������ Ŭ�� ��� ����Ʈ ��û
	m_pClanContext->EnterClanMember();
}

void CStepClanMember::OnClanInfoSecessionButton(void)
{ 
	// Ŭ�����ϰ�쿡�� Ż�����
	if( m_pClanContext->IsMaster() ) {
		//	"Ŭ�� �����ʹ� Ŭ���� Ż���� �� �����ϴ�.\nŻ���ϱ� ���ؼ� Ŭ�� �����͸� �����ؾ� �մϴ�."
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_SECESSION_MASTER_FAIL"));
	}
	else {
        m_pClanContext->ProcessSecessionFromClan();
	}
}

void CStepClanMember::_InitClanManagementWin(void)
{
	m_pClanManagementWin->SetEnable(TRUE);
	
	INIT_WIN_CAPTION(m_pClanManagementCaption, GAME_STRING("STBL_IDX_CLAN_INFORMATION"));	//	"Ŭ�� ����"

	INIT_TAB_EX(m_pInfoTab);
	INIT_TAB_EX(m_pBattleTab);
	INIT_TAB_EX(m_pSearchTab);
	INIT_TAB_EX(m_pManagementTab);
	m_pInfoTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	INIT_BUTTON_EX(m_pLeaveButton);

	_InitClanInfoPhase();
	_InitClanBattlePhase();
	_InitClanSearchPhase();
	_InitClanManamementPhase();	
}

void CStepClanMember::_InitClanInfoPhase(void)
{
	INIT_CAPTION_BUTTON(m_pInfoHistoryButton, GAME_STRING("STBL_IDX_CLAN_HISTORY"));		//	"Ŭ�� ����"
	INIT_CAPTION_BUTTON(m_pInfoMemberButton, GAME_STRING("STBL_IDX_CLAN_MEMBER"));		//	"Ŭ�� ���"
	INIT_CAPTION_BUTTON(m_pInfoSecessionButton, GAME_STRING("STBL_IDX_CLAN_SECESSION"));	//	"Ŭ�� Ż��"

	_InitClanInfoHistory();
	_InitClanInfoMember();

	//	Ŭ�� Ż�� ��ư�� Ȱ��ȭ
	ACTIVATE_BUTTON(m_pInfoSecessionButton);
}

void CStepClanMember::_InitClanInfoHistory(void)
{
	INIT_STATIC_CAPTION(m_pInfoHistoryNoticeStatic, GAME_STRING("STBL_IDX_CLAN_NOTICE"));	//	��������
	
	//	�������� ����
	m_pInfoHistoryNoticeValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_BIG_SIZE );
	m_pInfoHistoryNoticeValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pInfoHistoryNoticeValue->SetTextColor(GetColor(GCT_VALUE));
	m_pInfoHistoryNoticeValue->SetText("");
	m_pInfoHistoryNoticeValue->SetTextSpace( 0, 4 );

	INIT_NORMAL_CAPTION(m_pInfoHistoryListHeadSeasonScore, GAME_STRING("STBL_IDX_CLAN_SEASON_SCORE"));	//	���� ����
	INIT_NORMAL_CAPTION(m_pInfoHistoryListHeadTotalScore, GAME_STRING("STBL_IDX_CLAN_TOTAL_SCORE"));	//	��� ����

	INIT_NORMAL_CAPTION(m_slotClanHistoryList[CLAN_HISTORY_SLOT_MATCH].pTitle, GAME_STRING("STBL_IDX_CLAN_MATCH_COUNT"));		//	��ġ ��
	INIT_NORMAL_CAPTION(m_slotClanHistoryList[CLAN_HISTORY_SLOT_WIN].pTitle, GAME_STRING("STBL_IDX_CLAN_WIN_COUNT"));			//	�¸� ��
	INIT_NORMAL_CAPTION(m_slotClanHistoryList[CLAN_HISTORY_SLOT_LOSE].pTitle, GAME_STRING("STBL_IDX_CLAN_LOSE_COUNT"));			//	�й� ��
	INIT_NORMAL_CAPTION(m_slotClanHistoryList[CLAN_HISTORY_SLOT_MATCHES].pTitle, GAME_STRING("STBL_IDX_CLAN_VICTORIES"));		//	�·�

	m_slotClanHistoryList[CLAN_HISTORY_SLOT_MATCH].pTitle->SetTextSpace( 0, -4);
	m_slotClanHistoryList[CLAN_HISTORY_SLOT_WIN].pTitle->SetTextSpace( 0, -4);
	m_slotClanHistoryList[CLAN_HISTORY_SLOT_LOSE].pTitle->SetTextSpace( 0, -4);
	m_slotClanHistoryList[CLAN_HISTORY_SLOT_MATCHES].pTitle->SetTextSpace( 0, -4);

	for(INT32 i=0; i<CLANINFO_HISTORY_LIST_ROW_MAX; i++)
	{
		INIT_SLOT(m_slotClanHistoryList[i].pTitle);
		INIT_SLOT(m_slotClanHistoryList[i].pSeasonScore);
		INIT_SLOT(m_slotClanHistoryList[i].pTotalScore);
	}
	
	//	�ʱ�ȭ
	SetClanInfoHistoryMatch( 0, 0 );
	SetClanInfoHistoryWin( 0, 0 );
	SetClanInfoHistoryLose( 0, 0 );
	SetClanInfoHistoryMatches( 0.f, 0.f);	
}

void CStepClanMember::_InitClanInfoMember(void)
{	
	INIT_STATIC_CAPTION(m_pInfoBestListStatic, GAME_STRING("STBL_IDX_CLAN_BEST_MEMBER"));		//	��� Ŭ����
	INIT_STATIC_CAPTION(m_pInfoMemberListStatic, GAME_STRING("STBL_IDX_CLAN_MEMBER_LIST"));		//	Ŭ���� ����Ʈ

	INIT_NORMAL_CAPTION(m_slotBestMemberListHeadSeason, GAME_STRING("STBL_IDX_CLAN_SEASON"));		//	����
	INIT_NORMAL_CAPTION(m_slotBestMemberListHeadTotal, GAME_STRING("STBL_IDX_CLAN_TOTAL"));			//	���

	INIT_NORMAL_CAPTION(m_slotClanMemberListHeadClass, GAME_STRING("STBL_IDX_CLAN_MEMBER_LEVEL"));		//	����
	INIT_NORMAL_CAPTION(m_slotClanMemberListHeadNick, GAME_STRING("STBL_IDX_CLAN_MEMBER_NAME"));		//	�г���
	INIT_NORMAL_CAPTION(m_slotClanMemberListHeadState, GAME_STRING("STBL_IDX_CLAN_MEMBER_STATE"));		//	����

	INIT_NORMAL_CAPTION(m_slotBestMemberList[0].pTitle, GAME_STRING("STBL_IDX_CLAN_GET_EXP"));			//	����ġ
	INIT_NORMAL_CAPTION(m_slotBestMemberList[1].pTitle, GAME_STRING("STBL_IDX_CLAN_PARTICIPATION"));	//	������
	INIT_NORMAL_CAPTION(m_slotBestMemberList[2].pTitle, GAME_STRING("STBL_IDX_CLAN_WIN"));				//	�¼�
	INIT_NORMAL_CAPTION(m_slotBestMemberList[3].pTitle, GAME_STRING("STBL_IDX_CLAN_KILL"));				//	ų
	INIT_NORMAL_CAPTION(m_slotBestMemberList[4].pTitle, GAME_STRING("STBL_IDX_CLAN_HEADSHOT"));			//	��弦

	m_slotBestMemberList[0].pTitle->SetTextSpace(0, -4);
	m_slotBestMemberList[1].pTitle->SetTextSpace(0, -4);
	m_slotBestMemberList[2].pTitle->SetTextSpace(0, -4);
	m_slotBestMemberList[3].pTitle->SetTextSpace(0, -4);
	m_slotBestMemberList[4].pTitle->SetTextSpace(0, -4);
	

	// ��� Ŭ���� �ʱ�ȭ
	for(UINT32 i = 0; i < BEST_MEMBER_SLOT_MAX; i++)
	{
		INIT_SLOT(m_slotBestMemberList[i].pTitle);
		INIT_SLOT(m_slotBestMemberList[i].pSeasonMember);
		INIT_SLOT(m_slotBestMemberList[i].pTopMember);
	}

	// Ŭ���� ����Ʈ �ʱ�ȭ
	for(UINT32 i = 0; i < CLANINFO_MEMBER_LIST_ROW_MAX; i++)
	{
		INIT_SLOT(m_slotClanMemberList[i].pClass);
		INIT_SLOT2(m_slotClanMemberList[i].pNick);
		INIT_SLOT(m_slotClanMemberList[i].pState);

		m_slotClanMemberList[i].pClass->SetTextSpace(0, -4);
	}

	m_pClanMemberScrollList->CreateTextEx(CLANINFO_MEMBER_LIST_ROW_MAX,  GetDefaultFontName());
	m_pClanMemberScrollList->SetSelColor(0, 0, 0, 0);
}

void CStepClanMember::_UpdateClanInfoPhase(RT_REAL32 rDeltaSeconds)
{
	if(m_pInfoHistroyPhase->isEnable())
	{
		_UpdateClanInfoHistoryPhase(rDeltaSeconds);
	}
	else if(m_pInfoMemberPhase->isEnable())
	{
		_UpdateClanInfoMemberPhase(rDeltaSeconds);
	}
}

void CStepClanMember::_UpdateClanInfoHistoryPhase(RT_REAL32 rDeltaSeconds)
{
	m_pInfoHistoryNoticeValue->SetText( m_pClanContext->GetMyClanDetailInfo()->_announce );

	// Ŭ�� ������ ǥ���մϴ�.
	CLAN_DETAIL_INFO* pClanDetailInfo = g_pGameContext->setMyClanDetailInfo();

	SetClanInfoHistoryMatch( pClanDetailInfo->_smatch, pClanDetailInfo->_match );
	SetClanInfoHistoryWin( pClanDetailInfo->_swin, pClanDetailInfo->_win );
 	SetClanInfoHistoryLose( pClanDetailInfo->_slose, pClanDetailInfo->_lose );	
	SetClanInfoHistoryMatches( (pClanDetailInfo->_smatch == 0) ? 0.f : ((REAL32)pClanDetailInfo->_swin / (REAL32)pClanDetailInfo->_smatch) * 100.f,
		(pClanDetailInfo->_match == 0 ) ? 0.f : ((REAL32)pClanDetailInfo->_win / (REAL32)pClanDetailInfo->_match) * 100.f );
}

void CStepClanMember::_UpdateClanTopBast()
{
	// Ŭ�� ������ ǥ��
	const CLAN_MEMBER_BASIC_INFO* pTopSeasonMember[BEST_MEMBER_SLOT_MAX];
	const CLAN_MEMBER_BASIC_INFO* pTopMember[BEST_MEMBER_SLOT_MAX];	

	for(INT32 i=0; i<BEST_MEMBER_SLOT_MAX; i++)
	{
		pTopSeasonMember[i] = NULL;
		pTopMember[i] = NULL;
	}


	if( g_pGameContext->GetMyClanMemberMaxCount() > 0 )
	{
		CLAN_DETAIL_INFO* pClanDetailInfo = g_pGameContext->setMyClanDetailInfo();
		for( UINT8 i = 0 ; i < pClanDetailInfo->_currentPerson ; i++ )
		{
			const CLAN_MEMBER_BASIC_INFO * pMember = g_pGameContext->getMyClanMember(i);
			if ( pMember == NULL )
				continue;

			if( pClanDetailInfo->_stopexp != 0 && pClanDetailInfo->_stopexp			== pMember->_i64UID )	pTopSeasonMember[BEST_MEMBER_SLOT_EXP] = pMember;
			if( pClanDetailInfo->_stopconnect != 0 && pClanDetailInfo->_stopconnect	== pMember->_i64UID )	pTopSeasonMember[BEST_MEMBER_SLOT_CONNECTING] = pMember;
			if( pClanDetailInfo->_stopwin != 0 && pClanDetailInfo->_stopwin			== pMember->_i64UID )	pTopSeasonMember[BEST_MEMBER_SLOT_WIN_COUNT] = pMember;
			if( pClanDetailInfo->_stopkill != 0 && pClanDetailInfo->_stopkill		== pMember->_i64UID )	pTopSeasonMember[BEST_MEMBER_SLOT_KILL_COUNT] = pMember;
			if( pClanDetailInfo->_stophead != 0 && pClanDetailInfo->_stophead		== pMember->_i64UID )	pTopSeasonMember[BEST_MEMBER_SLOT_HEAD_COUNT] = pMember;

			if( pClanDetailInfo->_topexp != 0 && pClanDetailInfo->_topexp			== pMember->_i64UID )	pTopMember[BEST_MEMBER_SLOT_EXP]		= pMember;
			if( pClanDetailInfo->_topconnect != 0 && pClanDetailInfo->_topconnect	== pMember->_i64UID )	pTopMember[BEST_MEMBER_SLOT_CONNECTING] = pMember;
			if( pClanDetailInfo->_topwin != 0 && pClanDetailInfo->_topwin			== pMember->_i64UID )	pTopMember[BEST_MEMBER_SLOT_WIN_COUNT]	= pMember;
			if( pClanDetailInfo->_topkill != 0 && pClanDetailInfo->_topkill			== pMember->_i64UID )	pTopMember[BEST_MEMBER_SLOT_KILL_COUNT] = pMember;
			if( pClanDetailInfo->_tophead != 0 && pClanDetailInfo->_tophead			== pMember->_i64UID )	pTopMember[BEST_MEMBER_SLOT_HEAD_COUNT] = pMember;
		}
	}

	for(INT32 i=0; i<BEST_MEMBER_SLOT_MAX; i++)
	{
		_SetBestMember( (BEST_MEMBER_SLOT) i, pTopSeasonMember[i], pTopMember[i] );
	}
}

void CStepClanMember::_UpdateClanInfoMemberPhase(RT_REAL32 rDeltaSeconds)
{	
	//	Ŭ���� ���� ������Ʈ
	m_pClanContext->UpdateClanMember(rDeltaSeconds);

	_UpdateClanTopBast();

	//	��� ����Ʈ �籸��
	_UpdateSortClanMemberList();

	m_pClanMemberScrollList->Clear();
	for(INT32 i=0; i<m_listManageMember.GetCount(); i++)
		m_pClanMemberScrollList->AddElement("");

	for(INT32 i=0; i<CLANINFO_MEMBER_LIST_ROW_MAX; i++)
	{
		BOOL bValid = FALSE;
		ManagementClanMemberInfo * pMember = NULL;

		INT32 idx = i + m_idxClanInfoMemberStartSlot;

		if( idx < m_listManageMember.GetCount())
		{			
			pMember = (ManagementClanMemberInfo*) m_listManageMember.GetItem(idx);

			if(pMember != NULL)
			{
				if(pMember->_info._i64UID > 0)
					bValid = TRUE;
			}
		}

		if( bValid)
			_SetClanInfoMemberListSlot(i, pMember);
		else
			_ResetClanInfoMemberListSlot(i);
	}
}

void CStepClanMember::OnClanInfoMemberScroll(void)
{
	m_idxClanInfoMemberStartSlot = m_pClanMemberScrollList->GetCurrentTop();
}

void CStepClanMember::OnClanInfoMemberContext(INT32 nItemIdx)
{  
#if 0
	// Ŀ�´�Ƽ�� �ִ� ����̹Ƿ� �켱 �����ϴ�.
	INT32 memberIdx = m_pClanMemberScrollList->GetCurrentTop() + nItemIdx;	
	if (memberIdx >= m_pClanMemberScrollList->GetElementCount())	return;

	ContextMenuReset();
	
#if 0	// �Է� â�� ���� ���´�
	ContextMenuAddCommand(CMC_WHISPER);
#endif

	// �濡 �̸� �ӼӸ�, �ʴ��ϱ�
	if (PHASE_READY_ROOM == ((CStageReady*)m_pParent)->GetCurrentPhaseType())
	{
		ContextMenuAddCommand(CMC_INVITE);
	}

	// ģ���� �濡 ������ ã�ư���
	CLAN_MEMBER_BASIC_INFO * pMember = NULL;
	pMember = m_pClanContext->GetMyClanMember( memberIdx );

	if (FRIEND_STATE_ROOM == GET_FRIEND_STATE(pMember->_state))
	{			
		ContextMenuAddCommand(CMC_VISIT);
	}

	ContextMenuAddCommand(CMC_ADD_FRIEND);
	
	ContextMenuOpen(this, memberIdx);
#endif
}

void CStepClanMember::_ResetClanInfoMemberListSlot(INT32 nIdx)
{
	I3_BOUNDCHK(nIdx, CLANINFO_MEMBER_LIST_ROW_MAX);

	m_slotClanMemberList[nIdx].pSlot->SetEnable(TRUE);
	
	m_slotClanMemberList[nIdx].pRank->SetEnable(FALSE);
	m_slotClanMemberList[nIdx].pNick->SetText("");
	m_slotClanMemberList[nIdx].pClass->SetText("");
	m_slotClanMemberList[nIdx].pState->SetText("");
}


void CStepClanMember::_SetClanInfoMemberListSlot(INT32 nIdx, ManagementClanMemberInfo * pMember)
{
	char temp[MAX_STRING_COUNT] = "";

	I3_BOUNDCHK(nIdx, CLANINFO_MEMBER_LIST_ROW_MAX);

	m_slotClanMemberList[nIdx].pSlot->SetEnable(TRUE);
	
	//	���
	SetRankImage(m_slotClanMemberList[nIdx].pRank, pMember->_info._rank);

	//	�г���
	SetNameUsingColorTable( m_slotClanMemberList[nIdx].pNick, pMember->_info._nick, pMember->_info._color);
	
	//	��� ����
	GetMemberLevelName2(pMember->_info._level, temp, MAX_STRING_COUNT);
	m_slotClanMemberList[nIdx].pClass->SetText(temp);

	UINT32 ui32State = FRIEND_STATE_64_TO_32(pMember->_info._state);

	//	���� ����
	GetMemberConnectStateName( ui32State, temp, MAX_STRING_COUNT );

	m_slotClanMemberList[nIdx].pState->SetText(temp);

	INT32 state = GET_FRIEND_STATE(ui32State);
	if( state == FRIEND_STATE_ONLINE || state == FRIEND_STATE_ROOM)
		m_slotClanMemberList[nIdx].pState->SetTextColor(GetColor(GCT_VALUE));
	else
		m_slotClanMemberList[nIdx].pState->SetTextColor(GetColor(GCT_STATIC));
}

void CStepClanMember::SetClanInfoHistoryMatch(INT32 nSeason, INT32 nTotal)
{
	char temp[MAX_STRING_COUNT] = "";

	sprintf_s(temp, "%d %s", nSeason, GAME_STRING("STBL_IDX_CLAN_MATCH"));	//	��ġ
	m_slotClanHistoryList[CLAN_HISTORY_SLOT_MATCH].pSeasonScore->SetText(temp);

	sprintf_s(temp, "%d %s", nTotal, GAME_STRING("STBL_IDX_CLAN_MATCH"));	//	��ġ
	m_slotClanHistoryList[CLAN_HISTORY_SLOT_MATCH].pTotalScore->SetText(temp);
}

void CStepClanMember::SetClanInfoHistoryWin(INT32 nSeason, INT32 nTotal)
{
	char temp[MAX_STRING_COUNT] = "";

	sprintf_s(temp, "%d %s", nSeason, GAME_STRING("STBL_IDX_CLAN_WIN"));	//	��
	m_slotClanHistoryList[CLAN_HISTORY_SLOT_WIN].pSeasonScore->SetText(temp);

	sprintf_s(temp, "%d %s", nTotal, GAME_STRING("STBL_IDX_CLAN_WIN"));	//	��
	m_slotClanHistoryList[CLAN_HISTORY_SLOT_WIN].pTotalScore->SetText(temp);
}

void CStepClanMember::SetClanInfoHistoryLose(INT32 nSeason, INT32 nTotal)
{
	char temp[MAX_STRING_COUNT] = ""; 

	sprintf_s(temp, "%d %s", nSeason, GAME_STRING("STBL_IDX_CLAN_LOSE"));		//	��
	m_slotClanHistoryList[CLAN_HISTORY_SLOT_LOSE].pSeasonScore->SetText(temp);

	sprintf_s(temp, "%d %s", nTotal, GAME_STRING("STBL_IDX_CLAN_LOSE"));		//	��
	m_slotClanHistoryList[CLAN_HISTORY_SLOT_LOSE].pTotalScore->SetText(temp);
}

void CStepClanMember::SetClanInfoHistoryMatches(REAL32 fSeason, REAL32 fTotal)
{
	char temp[MAX_STRING_COUNT] = "";

	sprintf_s(temp, "%.2f%%", fSeason);
	m_slotClanHistoryList[CLAN_HISTORY_SLOT_MATCHES].pSeasonScore->SetText(temp);

	sprintf_s(temp, "%.2f%%", fTotal);
	m_slotClanHistoryList[CLAN_HISTORY_SLOT_MATCHES].pTotalScore->SetText(temp);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Ŭ����

void CStepClanMember::OnClanBattleTab(void)
{
	if ( ! g_pConfigEx->GetClan().Enable_Battle )
	{
		//���Ŀ� ������ �����Դϴ�.
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_PREPARE_PLAN"));
		return;
	}

	if(g_pGameContext->GetConnectedChannelType() != CHANNEL_TYPE_CLAN)
	{
		if(m_CurrentPage == CLAN_PAGE_BATTLE)
		{
			OnClanInfoTab();	//	����Ʈ�� ����â���� ����Ѵ�.
		}
		else
		{
			//	"Ŭ������ Ŭ�� ���������� �����մϴ�."
			STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_ONLY_IN_CLAN_SERVER") );
		}

		return;
	} 

	if( PHASE_READY_ROOM == ((CStageReady*)m_pParent)->GetCurrentPhaseType() &&
		m_pClanContext->GetVisibleLobbyPhase())
	{
		if(m_CurrentPage == CLAN_PAGE_BATTLE)
		{
			OnClanInfoTab();	//	����Ʈ�� ����â���� ����Ѵ�.
		}
		else
		{
			//	"Ŭ������ �κ񿡼��� �����մϴ�."
			STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_ONLY_IN_LOBBY"));
		}

		return;
	}

	m_pInfoPhase->SetEnable(FALSE);
	m_pBattlePhase->SetEnable(TRUE);
	m_pSearchPhase->SetEnable(FALSE);
	m_pManagementPhase->SetEnable(FALSE);

	m_pInfoTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pBattleTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pSearchTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pManagementTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_idxBattleTeamListStartSlot = 0;
	m_pBattleTeamScrollList->SetCurValue(0.0f);

	m_idxBattleTeamMatchType = 0;
	m_pBattleMakeTeamComboPopup->SetSelectedElement(m_idxBattleTeamMatchType, FALSE);

	const char * pszMatchType = GetClanBattleMatchType( (CLAN_MATCH_TYPE) m_idxBattleTeamMatchType);
	m_pBattleMakeTeamCombo->SetText(pszMatchType);

	//	������ �� Ŭ���� �� ���� ��û
	m_pClanContext->CreateBattleTeamList( MAX_BATTLE_CLAN_ROOM_COUNT );
	m_pClanContext->MatchTeamContext();	

	//	����Ʈ ��û
	m_pClanContext->MatchTeamList(0);

	m_bUpdateTeamList = FALSE;
	m_fUpdateTeamSection = 0.f;
	m_nTeamSection = 0;

	m_CurrentPage = CLAN_PAGE_BATTLE;

}

void CStepClanMember::OnClanBattleTeamEntry(void)
{
	//	�� ���
#if defined( TEST_CLAN_BATTLE)
	((CStageReady*)m_pParent)->ChangeStep(STEP_CLAN_BATTLE_LOBBY);
#else
	I3_BOUNDCHK( m_idxBattleTeamMatchType, CLAN_MATCH_MAX);

	INT32 nMemberNum = GetClanBattleMatchMember((CLAN_MATCH_TYPE)m_idxBattleTeamMatchType);

	m_pClanContext->MatchTeamCreate((UINT8)nMemberNum);
#endif
}

void CStepClanMember::OnClanBattleTeamTypePopup(INT32 idx)
{
	m_idxBattleTeamMatchType = idx; 
	I3_BOUNDCHK( m_idxBattleTeamMatchType, CLAN_MATCH_MAX);

	char * pszMatchType = (char*)GetClanBattleMatchType( (CLAN_MATCH_TYPE) m_idxBattleTeamMatchType);
	m_pBattleMakeTeamCombo->SetText(pszMatchType);
}

void CStepClanMember::OnClanBattleTeamJoin(INT32 slot)
{
#if defined( TEST_CLAN_BATTLE)
	((CStageReady*)m_pParent)->ChangeStep(STEP_CLAN_BATTLE_LOBBY);
#else
	INT32 max = m_pClanContext->GetClanBattleTeamContext()->_totalcount;

	m_nBattleTeamListSelectedIdx = slot;

	INT32 idx = m_idxBattleTeamListStartSlot + m_nBattleTeamListSelectedIdx;
	I3_BOUNDCHK(idx, max); 

	CLAN_BATTLE_TEAM_LIST * pTeamInfo = m_pClanContext->GetClanBattleTeamList() + idx;

	if( pTeamInfo == NULL )
	{
		I3WARN( "ClanBattleTeamList index (%d) is NULL", idx );
		return;
	}

	INT32 i32Channel = pTeamInfo->_Team._Channel;
	if( g_pServerInfo->IsUseServerSelectPage() )
	{
		i32Channel -= GAME_CHANNEL_COUNT;
	}

	if( g_pGameContext->GetServerNumber() != i32Channel+1)
	{
		//	"�ٸ� ä���� ������ ������ �� �����ϴ�."
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_CANNOT_OTHER_CHANNEL"));
	}
	else if( pTeamInfo != NULL || pTeamInfo->_Team._PerMax > 0)
	{
		if( pTeamInfo->_Team._PerMax <= pTeamInfo->_Team._PerCount)
		{
			//	"���� ������ ��� á�� ������ �ش� ���� ������ �� �����ϴ�."
			STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_CANNOT_JOIN_FULL"));
		}
		else
		{
			m_pClanContext->MatchTeamJoin(pTeamInfo->_Idx);
		}
	}
	
#endif
}

void CStepClanMember::OnClanBattleTeamTypeButton(void)
{
	ExclusionPopup(m_pBattleMakeTeamComboPopup);
}

void CStepClanMember::OnClanBattleTeamListScroll(void)
{
	m_idxBattleTeamListStartSlot = m_pBattleTeamScrollList->GetCurrentTop();
	m_bUpdateTeamList = TRUE;
}

void CStepClanMember::_InitClanBattlePhase(void)
{
	INIT_STATIC_CAPTION2(m_pBattleMakeTeamStatic, GAME_STRING("STBL_IDX_CLAN_CAPTION_TEAM"));		//	�� ����
	INIT_CAPTION_BUTTON(m_pBattleMakeTeamEntryButton, GAME_STRING("STBL_IDX_CLAN_CREATE_TEAM"));		//	�� ���
	m_pBattleMakeTeamStatic->SetCaptionTextSpace( 0, -4);
	m_pBattleMakeTeamStatic->SetCaptionTextOffset( 0.0f, -3.0f);
	m_pBattleMakeTeamEntryButton->SetCaptionTextSpace( 0, -4);
	m_pBattleMakeTeamEntryButton->SetCaptionTextOffset( 0.0f, -3.0f);
	
	INIT_COMBO_EX(m_pBattleMakeTeamComboButton);

	I3_BOUNDCHK( m_idxBattleTeamMatchType, CLAN_MATCH_MAX);

	char * pszMatchType = (char*)GetClanBattleMatchType( (CLAN_MATCH_TYPE) m_idxBattleTeamMatchType);
	INIT_NORMAL_CAPTION(m_pBattleMakeTeamCombo, pszMatchType);	//	�� ���� �޺�

	//	�� ���� �˾�
	m_pBattleMakeTeamComboPopup->CreateTextEx(5,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pBattleMakeTeamComboPopup->SetSelColor(100, 100, 100, 100);
	m_pBattleMakeTeamComboPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pBattleMakeTeamComboPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pBattleMakeTeamComboPopup->SetExclusiveEnableControl(TRUE);

	for(INT32 i=0; i<CLAN_MATCH_MAX; i++)
	{
		char * pszMatchType = (char*)GetClanBattleMatchType( (CLAN_MATCH_TYPE) i);
		m_pBattleMakeTeamComboPopup->AddElement( pszMatchType);
	}

	m_pBattleMakeTeamComboPopup->SetSelectedElement(m_idxBattleTeamMatchType, FALSE);
	m_pBattleMakeTeamComboPopup->SetEnable(FALSE);

	m_pBattleTeamScrollList->CreateTextEx(CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX,  GetDefaultFontName());
	m_pBattleTeamScrollList->SetSelColor(0, 0, 0, 0); 

	for(INT32 i=0; i<CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX; i++)
	{
		INIT_STATIC_CAPTION(m_slotBattleTeamList[i].pTeamName, INVALID_TEXT);		//	�� �̸�

		INIT_STATIC_CAPTION2(m_slotBattleTeamList[i].pLocale, INVALID_TEXT);	//	����ġ
		
		//	�� ����
		m_slotBattleTeamList[i].pState->SetEnable(TRUE);		
		m_slotBattleTeamList[i].pState->SetAllShapeEnable(FALSE);
		m_slotBattleTeamList[i].pState->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);	

		//	'����'
		INIT_WIN_CAPTION(m_slotBattleTeamList[i].pLeaderValue, GAME_STRING("STBL_IDX_CLAN_TEAM_LEADER"));	//	"����"

		//	���� ���
		SetRankImage(m_slotBattleTeamList[i].pLeaderRank, 0);

		//	���� �г���	
		INIT_VALUE_CAPTION(m_slotBattleTeamList[i].pLeaderNick, "");
		m_slotBattleTeamList[i].pLeaderNick->SetTextEllipsis(INVALID_TEXT);

		//	��ġ ����
		INIT_WIN_CAPTION(m_slotBattleTeamList[i].pMatchType, "-");

		//	���� �ο���
		INIT_WIN_CAPTION(m_slotBattleTeamList[i].pEntryMember, "-");

		//	'�����ϱ�'
		INIT_CAPTION_BUTTON(m_slotBattleTeamList[i].pJoinButton, GAME_STRING("STBL_IDX_CLAN_JOIN_TEAM"));	//	"�����ϱ�"
		m_slotBattleTeamList[i].pJoinButton->SetEnable(TRUE);

		m_slotBattleTeamList[i].pSlot->SetEnable(FALSE);
	}
}

void CStepClanMember::_UpdateClanBattlePhase(RT_REAL32 rDeltaSeconds)
{
	INT32 count = 0;
	CLAN_BATTLE_TEAM_LIST * pTeamInfoList = NULL;
	CLAN_BATTLE_TEAM_LIST * pTeamInfo = NULL;

#if defined(TEST_CLAN_BATTLE)

	count = 100; 
	pTeamInfoList = g_pTestTeamInfoList;		

	if( !m_bUpdateTeamList && g_pTestTeamInfoList == NULL && m_pClanContext->IsEnter())
	{
		//	�׽�Ʈ������ ���� ����Ʈ�� �����.
		g_pTestTeamInfoList = (CLAN_BATTLE_TEAM_LIST*) i3MemAlloc( sizeof(CLAN_BATTLE_TEAM_LIST) * count);
		i3mem::FillZero(g_pTestTeamInfoList, sizeof(CLAN_BATTLE_TEAM_LIST) * count);

		pTeamInfoList = g_pTestTeamInfoList;	

		for(INT32 i=0; i<count; i++)
		{
			pTeamInfo = g_pTestTeamInfoList + i;

			pTeamInfo->_Idx = (INT32)(i+1);

			pTeamInfo->_Member._i64UID = 100 + i;
			sprintf_s(pTeamInfo->_Member._Name, "MASTER %d", i+1);
			pTeamInfo->_Member._Rank = (UINT8)(rand() % 50);
			pTeamInfo->_Member._State = 0;
			
			pTeamInfo->_Team._MasterIdx = (UINT8) i;
			pTeamInfo->_Team._Naming = (UINT8) i;
			pTeamInfo->_Team._PerMax = (UINT8) (rand() % 5 + 4);
			pTeamInfo->_Team._PerCount = (UINT8) (rand() % pTeamInfo->_Team._PerMax + 1);
			pTeamInfo->_Team._State = (UINT8) (rand() % (INT32) CLAN_ROOM_PLAY + 1);
		}

		m_bUpdateTeamList = TRUE;
	}

#else
	CLAN_BATTLE_TEAM_CONTEXT * pTeamContext = m_pClanContext->GetClanBattleTeamContext();
	
	if( pTeamContext != NULL)
	{
		count = pTeamContext->_totalcount;
		pTeamInfoList = m_pClanContext->GetClanBattleTeamList();
	}
	
	m_fUpdateTeamSection += rDeltaSeconds;

	//	�� ����Ʈ�� �޴´�.
	if( pTeamContext->_sectionSize > 0 )
	{
		if( m_fUpdateTeamSection > LIST_UPDATE_TIME)
		{
			I3ASSERT( pTeamContext != NULL);

			m_fUpdateTeamSection = 0.0f;

			m_nTeamSection++;

			//	�� �ð����� ��� ����Ʈ�� ������ ��û�ϵ��� (����)
			if(m_nTeamSection > pTeamContext->_sectionCount)	
				m_nTeamSection = 0;			

			//	�� ����Ʈ�� ������ ��û�Ѵ�.
			m_pClanContext->MatchTeamList((UINT8) m_nTeamSection);
			
		}
	}
	
#endif

	//	�� ���� ������Ʈ
	if( m_bUpdateTeamList && pTeamInfoList != NULL) 
	{
		INT32 nSlot = 0;
		m_pBattleTeamScrollList->Clear();
		for(INT32 i=0; i<count; i++)
		{
			pTeamInfo = pTeamInfoList + i;

			//	�� ���°� CLAN_ROOM_EMPTY�� �ƴѰ��
			if(pTeamInfo->_Team._State != CLAN_ROOM_EMPTY)
			{
				m_pBattleTeamScrollList->AddElement("");

				//	�������� ���� ����
				if( i >= m_idxBattleTeamListStartSlot && nSlot < CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX)
				{
					_UpdateClanBattleTeamSlot(nSlot, pTeamInfo);
					nSlot++;
				}				
			}
		}

		//	�� ���� ó��
		if( count < CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX) 
		{
			for(INT32 i = nSlot; i<CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX; i++)
				_UpdateClanBattleTeamSlot(i, NULL);
		}

		m_bUpdateTeamList = FALSE;
	}
}

void CStepClanMember::_UpdateClanBattleTeamSlot(INT32 idx, CLAN_BATTLE_TEAM_LIST * pTeamInfo)
{
	BOOL bPlaying = FALSE;
	char szTemp[256] = "";
	I3_BOUNDCHK(idx, CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX);

	ClanEntryTeamSlot * pSlotInfo = &m_slotBattleTeamList[idx];

	if( pTeamInfo == NULL)
	{
		pSlotInfo->pSlot->SetEnable(FALSE);
		return;
	}

	pSlotInfo->pSlot->SetEnable(TRUE);

	CLAN_BATTLE_TEAM * pTeam = &pTeamInfo->_Team;
	CLAN_BATTLE_MEMBER * pMaster = &pTeamInfo->_Member;	

	INT32 i32Channel = pTeamInfo->_Team._Channel;
	if( g_pServerInfo->IsUseServerSelectPage() )
	{
		i32Channel -= GAME_CHANNEL_COUNT;
	}

	//	����
	pSlotInfo->pLeaderValue->SetText(GAME_STRING("STBL_IDX_CLAN_TEAM_LEADER"));	//	"����"

	//	����ġ
	sprintf_s(szTemp, GAME_STRING("STBL_IDX_CLAN_BATTLE_SERVER"), (INT32) (i32Channel+1));	//	"%02d����"
	pSlotInfo->pLocale->SetText(szTemp);	

	//	�� ����
	pSlotInfo->pState->SetEnable(TRUE);		
	pSlotInfo->pState->SetAllShapeEnable(FALSE);

	DEACTIVATE_BUTTON(pSlotInfo->pJoinButton);

	switch(pTeam->_State)
	{
	//	���� �� ��Ī �� ���´� ������ READY �����Դϴ�.
	case CLAN_ROOM_STAY:
		{
			pSlotInfo->pState->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);		
			
			//	Ǯ���� �ƴ� ���Ը�
			if( pTeam->_PerCount < pTeam->_PerMax)
			{
				//	���� ä���� ���� �����ϵ���
				if( g_pGameContext->GetServerNumber() == i32Channel+1)
				{
					ACTIVATE_BUTTON(pSlotInfo->pJoinButton);
				}
			}
		}
		break;
	//	���� �� ��Ī�Ǹ� ������ PLAY �����Դϴ�.
	case CLAN_ROOM_READY:
	case CLAN_ROOM_PLAY:
		{
			pSlotInfo->pState->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
			bPlaying = TRUE;
		}
		break;
	default:
		{
			pSlotInfo->pTeamName->SetText(INVALID_TEXT);
			pSlotInfo->pLeaderNick->SetText("");
			pSlotInfo->pLeaderRank->SetEnable(FALSE);
			pSlotInfo->pMatchType->SetText("");
			pSlotInfo->pEntryMember->SetText("");
			return;
		}
	}

	//	�� �̸�
	GetBattleTeamName( szTemp, 256, (INT32) pTeam->_Naming);
	pSlotInfo->pTeamName->SetText( szTemp);

	//	������ ���
	SetRankImage(pSlotInfo->pLeaderRank, pMaster->_Rank);

	//	���� �г���	
	pSlotInfo->pLeaderNick->SetTextEllipsis(pMaster->_Name);

	//	��ġ ����
	sprintf_s(szTemp, "%d VS %d", (INT32)pTeam->_PerMax, (INT32)pTeam->_PerMax);
	pSlotInfo->pMatchType->SetText( szTemp);

	//	���� �ο���
	sprintf_s( szTemp, "%d / %d", (INT32)pTeam->_PerCount, (INT32)pTeam->_PerMax);
	pSlotInfo->pEntryMember->SetText(szTemp);

	//	�ɹ� �ο� ����
	if(bPlaying) 
	{
		COLOR color;
		i3Color::Set( &color, (UINT8) 110, 110, 110, 255);
		pSlotInfo->pEntryMember->SetTextColor( &color);		//	�÷�����
	}
	else if( pTeam->_PerMax <= pTeam->_PerCount)
		pSlotInfo->pEntryMember->SetTextColor( GetColor( GCT_RED));		//	Ǯ��
	else
		pSlotInfo->pEntryMember->SetTextColor( GetColor( GCT_VALUE));	//	��������
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Ŭ��ã��

void CStepClanMember::OnClanSearchTab(void)
{
	if ( ! g_pConfigEx->GetClan().Enable_Search )
	{
		//���Ŀ� ������ �����Դϴ�.
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_PREPARE_PLAN"));
		return;
	}

	m_pInfoPhase->SetEnable(FALSE);
	m_pBattlePhase->SetEnable(FALSE);
	m_pSearchPhase->SetEnable(TRUE);
	m_pManagementPhase->SetEnable(FALSE);

	m_pInfoTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pBattleTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pSearchTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pManagementTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_CurrentPage = CLAN_PAGE_SEARCH;
	
	m_bNeedUpdateSearchClanList = FALSE;

	m_nSearchClanStartSlot = 0;
	m_nSearchClanSelectedSlot = -1;
	i3mem::FillZero( &m_SelectedClanInfo, sizeof(SORT_CLAN_INFO));
	m_pSearchClanListScroll->SetCurValue(0.0f);

	m_pSearchInput->SetIMEText("");
	m_pSearchInput->SetFocus(TRUE);

	m_pSearchClanListFocus->SetEnable(FALSE);
	m_pSearchClanListSelect->SetEnable(FALSE);

	m_pClanContext->EnterClanList( TRUE);
}

void CStepClanMember::OnSearchClanListScroll(void)
{
	m_nSearchClanStartSlot = m_pSearchClanListScroll->GetCurrentTop();
	m_bNeedUpdateSearchClanList = TRUE;

	INT32 hilightslot = m_nSearchClanSelectedSlot - m_nSearchClanStartSlot;

	if( hilightslot < 0 || hilightslot >= CLAN_SEARCH_LIST_SLOT_MAX)
	{
		m_pSearchClanListSelect->SetEnable( FALSE);
	}
	else
	{
		m_pSearchClanListSelect->setPositionY( hilightslot * 27.0f + 1.0f);
		m_pSearchClanListSelect->SetEnable( TRUE);
	}
}

void CStepClanMember::_SetSearchHilightClanList(INT32 idx)
{
	if( idx < 0)
	{
		i3mem::FillZero( &m_SelectedClanInfo, sizeof(SORT_CLAN_INFO));

		if( idx < -1)	idx = -1;
	}
	else
	{
		SORT_CLAN_INFO * pSelected = m_pClanContext->GetClanList(idx);
		I3ASSERT( pSelected != NULL);

		i3mem::Copy( &m_SelectedClanInfo, pSelected, sizeof(SORT_CLAN_INFO));		
	}
	
	m_nSearchClanSelectedSlot = idx;

	INT32 hilightslot = m_nSearchClanSelectedSlot - m_nSearchClanStartSlot;

	if( hilightslot < 0 || hilightslot >= CLAN_SEARCH_LIST_SLOT_MAX)
	{
		m_pSearchClanListSelect->SetEnable( FALSE);
	}
	else
	{
		m_pSearchClanListSelect->setPositionY( hilightslot * 27.0f + 1.0f);
		m_pSearchClanListSelect->SetEnable( TRUE);
	}
}

BOOL CStepClanMember::_FindSearchHilightClanList(void)
{
	BOOL bFind = FALSE;

	//	���϶���Ʈ Ŭ�� ������ ã�� ��Ī�Ѵ�.
	if( m_SelectedClanInfo._info._idx > 0)
	{
		for(INT32 i=0; i<m_pClanContext->GetClanListCount(); i++)
		{
			SORT_CLAN_INFO * pInfo = (SORT_CLAN_INFO*) m_pClanContext->GetClanList(i);
			
			if( pInfo->_info._idx == m_SelectedClanInfo._info._idx)
			{
				_SetSearchHilightClanList(i);

				bFind = TRUE;
				break;
			}
		}
	}

	if( !bFind)
	{	//	���� ��Ͽ��� ���� ����.
		_SetSearchHilightClanList(-1);
	}

	return bFind;
}

void CStepClanMember::_RebuildSearchClanListScrollBar(void)
{
	INT32 nNewCount = m_pClanContext->GetClanListCount();

	// ���� ������ Ŭ���� ������ �������� ������ �̻��¿��� �߰���
	if( m_pSearchClanListScroll->GetElementCount() < nNewCount)
	{
		INT32 nAddCount = nNewCount - m_pSearchClanListScroll->GetElementCount();

		for( INT32 i=0; i<nAddCount; ++i)
		{
			m_pSearchClanListScroll->AddElement( "");
		}
	}
	// ���� ������ Ŭ���� ������ �������� ������ �׳� ����Ʈ�� �ʱ�ȭ�� �ٽ� �ִ´�.
	else if( m_pSearchClanListScroll->GetElementCount() > nNewCount) 
	{
		m_pSearchClanListScroll->Clear();
		for( INT32 i=0; i<nNewCount; ++i)
		{
			m_pSearchClanListScroll->AddElement( "");
		}
	}
}

void CStepClanMember::OnSortSearchClanListByRank(void)
{
	if( m_pClanContext->GetClanSortType() == CLAN_SORT_RANK_DESC)
	{
		m_pClanContext->SortClanList(CLAN_SORT_RANK_ASC);
	}
	else	//	default
	{
		m_pClanContext->SortClanList(CLAN_SORT_RANK_DESC);
	}

	m_bNeedUpdateSearchClanList = TRUE;

	//	���϶���Ʈ ����
	_FindSearchHilightClanList();

	//	��ũ�ѹ� ũ�� ���� ����
	_RebuildSearchClanListScrollBar();

	m_pSearchClanListHeadRank->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnSortSearchClanListByName(void)
{
	if( m_pClanContext->GetClanSortType() == CLAN_SORT_NAME_ASC)
	{
		m_pClanContext->SortClanList(CLAN_SORT_NAME_DESC);
	}
	else	//	default
	{
		m_pClanContext->SortClanList(CLAN_SORT_NAME_ASC);
	}	

	m_bNeedUpdateSearchClanList = TRUE;

	//	���϶���Ʈ ����
	_FindSearchHilightClanList();

	//	��ũ�ѹ� ũ�� ���� ����
	_RebuildSearchClanListScrollBar();

	m_pSearchClanListHeadName->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnSortSearchClanListByDate(void)
{
	if( m_pClanContext->GetClanSortType() == CLAN_SORT_DATE_DESC)
	{
		m_pClanContext->SortClanList(CLAN_SORT_DATE_ASC);
	}
	else	//	default
	{
		m_pClanContext->SortClanList(CLAN_SORT_DATE_DESC);
	}

	m_bNeedUpdateSearchClanList = TRUE;

	//	���϶���Ʈ ����
	_FindSearchHilightClanList();

	//	��ũ�ѹ� ũ�� ���� ����
	_RebuildSearchClanListScrollBar();

	m_pSearchClanListHeadDate->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnSortSearchClanListByMember(void)
{
	if( m_pClanContext->GetClanSortType() == CLAN_SORT_MEMBER_DESC)
	{
		m_pClanContext->SortClanList(CLAN_SORT_MEMBER_ASC);
	}
	else	//	default
	{
		m_pClanContext->SortClanList(CLAN_SORT_MEMBER_DESC);
	}

	m_bNeedUpdateSearchClanList = TRUE;

	//	���϶���Ʈ ����
	_FindSearchHilightClanList();

	//	��ũ�ѹ� ũ�� ���� ����
	_RebuildSearchClanListScrollBar();

	m_pSearchClanListHeadMember->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnSearchClanKeyword(void)
{
	const char * pszKeyword = m_pSearchInput->getIMETextBuf();

	_SearchClanList(pszKeyword);
}

void CStepClanMember::OnSearchClanViewAll(void)
{
	_SearchClanList("");
}

void CStepClanMember::OnSearchClanFocus(INT32 idx)
{
	I3_BOUNDCHK( idx, CLAN_SEARCH_LIST_SLOT_MAX);

	m_pSearchClanListFocus->setPositionY( idx * 27.0f + 1.0f);
	m_pSearchClanListFocus->SetEnable( TRUE);
}

void CStepClanMember::OnSearchClanSelect(INT32 idx)
{
	I3_BOUNDCHK( idx, CLAN_SEARCH_LIST_SLOT_MAX);

	INT32 slot = idx + m_nSearchClanStartSlot;

	if( slot >= m_pClanContext->GetClanListCount())
		return;

	_SetSearchHilightClanList(m_nSearchClanStartSlot + idx);

	PlaySound(GUI_SND_ITEM_SELECTING);

#if defined TEST_CLAN_LIST
	STRING_POPUP("TEST_CLAN_LIST");
#else
	{
		I3_BOUNDCHK(slot, m_pClanContext->GetClanListCount());
		SORT_CLAN_INFO * pSortClanInfo = m_pClanContext->GetClanList(slot);

		UINT8 cType = 1;
		g_pGameContext->SetEvent( EVENT_CLAN_DETAIL_INFO_N, &pSortClanInfo->_info._idx, &cType);
	}
#endif
}

void CStepClanMember::_UpdateClanSerchRate( REAL32 rDeltaSeconds)
{
	char szRate[MAX_PATH];
	INT32 nDotNumber = 0;
	INT32 nRate = m_pClanContext->GetClanSerchRate();
	INT32 nTotal = g_pGameContext->getClanContext()->_totalCount;
	static REAL32 fDeltaSec = 0;

	if( nRate >= 100)		nDotNumber = 20;
	else if( nRate <= 0)	nDotNumber = 0;
	else nDotNumber = nRate / 5;

	m_pClanSerchGage->SetAllShapeEnable(FALSE);
	
	for(INT32 i = 0; i < nDotNumber; i++)
	{
		m_pClanSerchGage->SetShapeEnable(GUI_SHAPE_OFFSET + i, TRUE);
	}

	if( nRate == 100)
	{
		char szTemp[256] = "";
		// �˻��Ϸ�
		sprintf_s( szTemp, "%s   %d / %d", GAME_STRING("STBL_IDX_CLAN_SERCH_COMPLETE"), m_pClanContext->GetClanListCount(), nTotal); 

		// �Ϸ�.
		m_pClanMembeSerchText->SetEnable(FALSE);
		m_pClanSerchText->SetText( szTemp);	
	}
	else
	{
		fDeltaSec += rDeltaSeconds;

		if( fDeltaSec > 1.0f)
		{
			if(m_pClanMembeSerchText->isEnable()) m_pClanMembeSerchText->SetEnable( FALSE);
			else m_pClanMembeSerchText->SetEnable( TRUE);

			fDeltaSec = 0.0f;
		}

		// ���� ������.
		m_pClanMembeSerchText->SetText(GAME_STRING("STBL_IDX_CLAN_WAIT_SERCH")); // "Ŭ������Ʈ�� �˻����Դϴ�. ��ø� ��ٷ� �ֽʽÿ�."

		sprintf_s( szRate, GAME_STRING("STBL_IDX_CLAN_TRY_SERCH"), nRate); // "Ŭ������Ʈ �˻� ��(%d%%)"

		char szTemp[256] = "";
		sprintf_s( szTemp, "   %d / %d", m_pClanContext->GetClanListCount(), nTotal);
		i3String::Concat( szRate, szTemp);

		m_pClanSerchText->SetText(szRate);	
	}
}

void CStepClanMember::_InitClanSearchPhase(void)
{
	INIT_NORMAL_CAPTION(m_pSearchStatic, GAME_STRING("STBL_IDX_CLAN_BATTLE_SEARCHING"));	//	"Ŭ��ã��"

	//	Ŭ���� �˻� �Է�
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NET_CLAN_NAME_SIZE - 1;
	ime._StyleIME = 0;
	m_pSearchInput->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pSearchInput->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSearchInput->SetTextColor(GetColor(GCT_VALUE));
	m_pSearchInput->SetText("");
	m_pSearchInput->SetIMEMode( IME_MODE_NATIVE);

	INIT_BUTTON_EX(m_pSearchButton);
	INIT_CAPTION_BUTTON(m_pSearchViewAll, GAME_STRING("STBL_IDX_CLAN_VIEW_ALL"));	//  "��ü����"

	INIT_STATIC_CAPTION(m_pSearchInfoNameStatic,  GAME_STRING("STBL_IDX_CLAN_NAME"));		// "Ŭ���� :"
	INIT_VALUE_CAPTION( m_pSearchInfoNameValue, "");

	INIT_STATIC_CAPTION(m_pSearchInfoMasterStatic,  GAME_STRING("STBL_IDX_CLAN_MASTER"));	// "Ŭ�� ������ :"
	INIT_VALUE_CAPTION( m_pSearchInfoMasterValue, "");

	INIT_STATIC_CAPTION(m_pSearchInfoClanRankStatic,  GAME_STRING("STBL_IDX_CLAN_RANK"));	// "Ŭ�� ��� :"
	INIT_VALUE_CAPTION( m_pSearchInfoClanRankValue, "");

	INIT_STATIC_CAPTION(m_pSearchInfoStaffStatic,  GAME_STRING("STBL_IDX_CLAN_PERSON"));		// "Ŭ�� �ο��� :"
	INIT_VALUE_CAPTION( m_pSearchInfoStaffValue, "");

	INIT_STATIC_CAPTION(m_pSearchInfoAddressStatic,  GAME_STRING("STBL_IDX_CLAN_AZIT"));		// "Ŭ�� ����Ʈ :"
	INIT_VALUE_CAPTION( m_pSearchInfoAddressValue, "");
	m_pSearchInfoAddressValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);

#ifndef CLAN_ADDRESS_ENABLE
	{
		m_pSearchInfoAddressBullet->SetEnable( FALSE);
		m_pSearchInfoAddressStatic->SetEnable( FALSE);
		m_pSearchInfoAddressValue->SetEnable( FALSE);
	}
#endif

	I3COLOR color;
	i3Color::Set(&color, (UINT8) 255, 125, 0, 255);
	m_pSearchInfoAddressValue->SetTextColor(&color);

	INIT_STATIC_CAPTION(m_pSearchInfoOrderStatic,  GAME_STRING("STBL_IDX_CLAN_ORDER"));		// "Ŭ�� ���� :"
	INIT_VALUE_CAPTION( m_pSearchInfoOrderValue, "");

	INIT_STATIC_CAPTION(m_pSearchInfoExpStatic,  GAME_STRING("STBL_IDX_CLAN_EXP"));			// "Ŭ�� ����ġ :
	INIT_VALUE_CAPTION( m_pSearchInfoExpValue, "");

	//Ŭ�� ����Ʈ ���
	INIT_NORMAL_CAPTION( m_pSearchClanListHeadRank, GAME_STRING("STBL_IDX_CLAN_HEADER_RANK"));		// Ŭ�� ����Ʈ ��� : "���"
	INIT_NORMAL_CAPTION( m_pSearchClanListHeadName, GAME_STRING("STBL_IDX_CLAN_HEADER_NAME"));		// Ŭ�� ����Ʈ ��� : "Ŭ����"
	INIT_NORMAL_CAPTION( m_pSearchClanListHeadDate, GAME_STRING("STBL_IDX_CLAN_HEADER_ENTRY"));		// Ŭ�� ����Ʈ ��� : "������"
	INIT_NORMAL_CAPTION( m_pSearchClanListHeadMember, GAME_STRING("STBL_IDX_CLAN_HEADER_PERSON"));	// Ŭ�� ����Ʈ ��� : "���� �ο�"
	m_pSearchClanListHeadMember->SetTextSpace( 0, -4);

	m_pSearchClanListFocus->SetEnable(FALSE);
	m_pSearchClanListFocus->setInputDisable( TRUE);
	m_pSearchClanListSelect->SetEnable(FALSE);
	m_pSearchClanListSelect->setInputDisable( TRUE);

	m_pSearchClanListScroll->CreateTextEx(CLAN_SEARCH_LIST_SLOT_MAX,  GetDefaultFontName());
	m_pSearchClanListScroll->SetSelColor(0, 0, 0, 0);

	for(INT32 i=0; i<CLAN_SEARCH_LIST_SLOT_MAX; i++)
	{
		m_ClanListSlot[i]._pSlot->SetEnable(TRUE);

		m_ClanListSlot[i]._pClanMarkImage->SetAllShapeEnable( FALSE);
		m_ClanListSlot[i]._pClanMarkImage->SetEnable( FALSE);
		m_ClanListSlot[i]._pClanMarkImage->setInputDisable( TRUE);

		m_ClanListSlot[i]._pRankImage->SetAllShapeEnable( FALSE);
		m_ClanListSlot[i]._pRankImage->SetEnable( FALSE);
		m_ClanListSlot[i]._pRankImage->setInputDisable( TRUE);

		m_ClanListSlot[i]._pDate->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
		m_ClanListSlot[i]._pDate->SetEnable( FALSE);
		m_ClanListSlot[i]._pDate->setInputDisable( TRUE);
		m_ClanListSlot[i]._pDate->SetTextSpace( 0, -2);

		m_ClanListSlot[i]._pMember->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
		m_ClanListSlot[i]._pMember->SetEnable( FALSE);
		m_ClanListSlot[i]._pMember->setInputDisable( TRUE);

		m_ClanListSlot[i]._pName->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
		m_ClanListSlot[i]._pName->SetEnable( FALSE);
		m_ClanListSlot[i]._pName->setInputDisable( TRUE);
	}

	m_pSearchInfoOrderValue->SetText(GAME_STRING("STBL_IDX_CLAN_UNKNOWN"));	// "�˼�����"

	_SetSearchClanRank( 0, 0, 0);
	_SetSearchClanMasterName( GAME_STRING("STBL_IDX_CLAN_UNKNOWN"), 0);	// "�˼�����"
	_SetSearchClanStaff( 0, 0);
	_SetSearchClanName( GAME_STRING("STBL_IDX_CLAN_UNDECIDED"));	// "���õ��� ����"
	_SetSearchClanExp(0, 0);
	_SetSearchClanAddress( GAME_STRING("STBL_IDX_CLAN_UNKNOWN"));	// "�˼�����"
	_SetSearchClanMark(0);
}

void CStepClanMember::_UpdateClanSearchPhase(RT_REAL32 rDeltaSeconds)
{
	//	Ŭ�� ����Ʈ�� ������Ʈ�Ѵ�.
	BOOL ret = m_pClanContext->UpdateClanList(rDeltaSeconds);
	if( ret)
	{
		// ��ũ�ѹ� �缳��
		_RebuildSearchClanListScrollBar();

		//	Ŭ�� ����Ʈ�� ��������Ƿ� ���õ� ������ �ٽ� ó���Ѵ�.
		_FindSearchHilightClanList();

		m_bNeedUpdateSearchClanList = TRUE;
	}

	//	����Ʈ ������Ʈ
	_UpdateSearchClanList();

	_UpdateSearchClanDetailedInfo();	

	_UpdateClanSerchRate(rDeltaSeconds);
}

void CStepClanMember::_UpdateSearchClanList(void)
{
	if( m_bNeedUpdateSearchClanList == FALSE)
		return;

	INT32 i = 0;
	
//	INT32 nNewCount = m_pClanContext->GetClanListCount();
//	if( m_pSearchClanListScroll->GetElementCount() != nNewCount)
//	{
//		m_pSearchClanListScroll->Clear();
//		for( INT32 i=0; i<nNewCount; ++i)
//		{
//			m_pSearchClanListScroll->AddElement( "");
//		}
//	}

	//	Ŭ�� ����Ʈ ����
	for(i = 0; i < CLAN_SEARCH_LIST_SLOT_MAX; i++)
	{
		INT32 idx = i + m_nSearchClanStartSlot;
		if( idx >= m_pClanContext->GetClanListCount())
		{
			_SetSearchClanListSlot(i, NULL);
		}
		else
		{
			SORT_CLAN_INFO * pSortClanInfo = m_pClanContext->GetClanList(idx);
			CLAN_BASIC_INFO * pInfo = &pSortClanInfo->_info;

			if( pInfo->_idx > 0)
			{
				_SetSearchClanListSlot(i, pInfo);
			}
			else
			{
				_SetSearchClanListSlot(i, NULL);
			}
		}
	}

	m_bNeedUpdateSearchClanList = FALSE;
}

void CStepClanMember::_SetSearchClanListSlot(INT32 idx, CLAN_BASIC_INFO * pInfo)
{
	I3_BOUNDCHK( idx, CLAN_SEARCH_LIST_SLOT_MAX);

	char szString[MAX_STRING_COUNT] = "";

	if( pInfo)
	{
		//Ŭ�� ��ũ �̹���
		{
			m_ClanListSlot[idx]._pClanMarkImage->SetShapeEnable( GUI_SHAPE_OFFSET, TRUE);
			m_ClanListSlot[idx]._pClanMarkImage->SetEnable( TRUE);

#if defined( USE_CLAN_MARK)
			::SetClanMarkTexEx(m_ClanListSlot[idx]._pClanMarkImage, CLAN_MARK_SMALL, pInfo->_mark);
#endif
		}

		//Ŭ�� ���
		{
			SetClanRankImage(m_ClanListSlot[idx]._pRankImage, pInfo->_rank);
		}

		//Ŭ�� ������
		{
			INT32 nYear = (INT32)(pInfo->_date / 10000);
			INT32 nMonth = (INT32)((pInfo->_date - nYear * 10000) / 100);
			INT32 nDay = (INT32)(pInfo->_date - nYear * 10000 - nMonth * 100);

			sprintf_s(szString, GAME_STRING("STBL_IDX_CLAN_DECLARE_DATE"), 
								nYear, nMonth, nDay);		// "%04d�� %02d�� %02d��"

			m_ClanListSlot[idx]._pDate->SetText( szString);
			m_ClanListSlot[idx]._pDate->SetEnable( TRUE);
		}

		//Ŭ�� �����
		{
#if defined( LOCALE_RUSSIA)
			sprintf_s( szString, "%d", pInfo->_person);	// "%d"
#else
			sprintf_s( szString, GAME_STRING("STBL_IDX_CLAN_DECLARE_PERSON"), pInfo->_person);	// "%d ��"
#endif

			m_ClanListSlot[idx]._pMember->SetText( szString);
			m_ClanListSlot[idx]._pMember->SetEnable( TRUE);
		}

		//Ŭ�� �̸�
		{
			sprintf_s( szString, "%s", pInfo->_name);

			SetNameUsingColorTable( m_ClanListSlot[idx]._pName, pInfo->_name, pInfo->_color );
			m_ClanListSlot[idx]._pName->SetEnable( TRUE);
		}
	}
	else
	{
		m_ClanListSlot[idx]._pClanMarkImage->SetAllShapeEnable( FALSE);
		m_ClanListSlot[idx]._pClanMarkImage->SetEnable( FALSE);

		m_ClanListSlot[idx]._pRankImage->SetAllShapeEnable( FALSE);
		m_ClanListSlot[idx]._pRankImage->SetEnable( FALSE);

		m_ClanListSlot[idx]._pDate->SetEnable( FALSE);
		m_ClanListSlot[idx]._pMember->SetEnable( FALSE);
		m_ClanListSlot[idx]._pName->SetEnable( FALSE);
	}
}

void CStepClanMember::_UpdateSearchClanDetailedInfo(void)
{
	char temp[256] = "";

	if( m_nSearchClanSelectedSlot == -1)
	{
		m_pSearchInfoOrderValue->SetText(GAME_STRING("STBL_IDX_CLAN_UNKNOWN"));	// "�˼�����"

		_SetSearchClanRank( 0, 0, 0);
		_SetSearchClanMasterName( GAME_STRING("STBL_IDX_CLAN_UNKNOWN"), 0);	// "�˼�����"
		_SetSearchClanStaff( 0, 0);
		_SetSearchClanName( GAME_STRING("STBL_IDX_CLAN_UNDECIDED"));	// "���õ��� ����"
		_SetSearchClanExp(0, 0);
		_SetSearchClanAddress( GAME_STRING("STBL_IDX_CLAN_UNKNOWN"));	// "�˼�����"
		_SetSearchClanMark(0);
	}
	else
	{
		CLAN_DETAIL_INFO * pInfo = g_pGameContext->getClanDetailInfo();		

		//_SetSearchClanOrder( pInfo->_clanRank, pInfo->_prevRank);
		_SetSearchClanOrder( 0, 0);
		_SetSearchClanRank( pInfo->_exp, pInfo->_currentPerson, pInfo->_expRank);
		_SetSearchClanMasterName( pInfo->_masterNick, pInfo->_masterRank);
		_SetSearchClanStaff( pInfo->_currentPerson, pInfo->_maxPerson);

		SetNameUsingColorTable( m_pSearchInfoNameValue, pInfo->_name, pInfo->_color );

		UINT32 wExp = pInfo->_exp;
		if( wExp > DF_CLAN_RANK_L5 ) wExp = DF_CLAN_RANK_L5;
		UINT32 BeforeExp = GetClanBeforeRankExp( wExp );
		_SetSearchClanExp(wExp - BeforeExp, GetClanNextRankExp(wExp) - BeforeExp);
		_SetSearchClanMark(pInfo->_mark);

		i3String::Copy(temp, g_pConfigEx->GetClan().AzitAddressBase, 256 );
		i3String::Concat(temp, pInfo->_azitURL);
		_SetSearchClanAddress(temp);
	}
}

void CStepClanMember::_SearchClanList(const char * pszKeyword)
{
	if( !m_pClanContext->SearchClanListView( pszKeyword))
	{
		return;
	}

	//	������ ��� �ٽ� ��� Ŭ���� �����ش�.
	if( i3String::Length(pszKeyword) == 0)
	{
		DEACTIVATE_BUTTON(m_pSearchViewAll);
	}
	//	���ϴ� Ű����� Ŭ���� ã�� �����ش�.
	else if( pszKeyword != NULL && i3String::Length(pszKeyword) > 0)
	{
		ACTIVATE_BUTTON(m_pSearchViewAll);
	}

	m_bNeedUpdateSearchClanList = TRUE;

	// ��ũ�ѹ� �缳��
	_RebuildSearchClanListScrollBar();

	m_nSearchClanStartSlot = 0;
	m_pSearchClanListScroll->SetCurValue(0.0f);

	//	Ŭ�� ����Ʈ�� ��������Ƿ� ���õ� ������ �ٽ� ó���Ѵ�. (�� �ؿ��� ó���ؾ� ��)
	_FindSearchHilightClanList();
}

void CStepClanMember::_SetSearchClanName(const char * pszString)
{
	m_pSearchInfoNameValue->SetTextEllipsis( pszString);
}

void CStepClanMember::_SetSearchClanStaff(INT32 nNum, INT32 nTotal)
{
	//	Ŭ�� �ο���
	::SetClanStaff(m_pSearchInfoStaffValue, nNum, nTotal);
}

void CStepClanMember::_SetSearchClanMasterName(const char * pszString, INT32 nRank)
{
	//	Ŭ�� ������
	::SetClanMaster(m_pSearchInfoMasterValue, m_SearchInfoMasterRankImage, pszString, (UINT8)nRank);
}

void CStepClanMember::_SetSearchClanRank(UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank)
{
	//	Ŭ�� ���
	::SetClanRank(m_pSearchInfoClanRankValue, m_SearchInfoClanRankImage, nClanExp, nClanStaffNum, nClanRank);
}

void CStepClanMember::_SetSearchClanOrder(INT32 nCurValue, INT32 nPrevValue)
{
	//	Ŭ�� ����
	::SetClanOrder(m_pSearchInfoOrderValue, nCurValue, nPrevValue);
}

void CStepClanMember::_SetSearchClanExp(UINT32 nCurValue, UINT32 nMaxValue)
{
	REAL32 rx = ((CStageReady*)m_pParent)->GetGuiRoot()->getGuiResolutionWidth();

	//	Ŭ�� ����ġ
	SetClanExpText(m_pSearchInfoExpValue, nCurValue, nMaxValue);
	SetClanExpGauge(m_SearchInfoExpBar, rx, nCurValue, nMaxValue);
}

void CStepClanMember::_SetSearchClanAddress(const char * pszString)
{
#ifdef CLAN_ADDRESS_ENABLE
	{
		m_pSearchInfoAddressStatic->SetEnable( TRUE);
		m_pSearchInfoAddressValue->SetEnable( TRUE);

		//	Ŭ�� �ּ�
		m_pSearchInfoAddressValue->SetText( pszString);
	}
#else
	{
		m_pSearchInfoAddressStatic->SetEnable( FALSE);
		m_pSearchInfoAddressValue->SetEnable( FALSE);
	}
#endif	
}

void CStepClanMember::_SetSearchClanMark(UINT32 nFlag)
{
#if defined( USE_CLAN_MARK)
	::SetClanMarkTexEx(m_pSearchInfoClanMarkImage, CLAN_MARK_MEDIUM, nFlag);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Ŭ������

void CStepClanMember::OnClanManagementTab(void)
{		
	//	������ �Ǵ� ���ܸ� Ŭ�� ���� ���� ���δ�.
	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER &&
		m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_STAFF )
	{
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER_OR_STAFF"));	//	"������ �Ǵ� ������ ������ �ʿ��մϴ�."
		return;
	}

	m_pInfoPhase->SetEnable(FALSE);
	m_pBattlePhase->SetEnable(FALSE);
	m_pSearchPhase->SetEnable(FALSE);
	m_pManagementPhase->SetEnable(TRUE);

	m_pInfoTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pBattleTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pSearchTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pManagementTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	if ( CanClanMemberSendNoteAll() )
	{
		//	���� ������ ����� �ϴ� �����͸� ����
		if(m_pClanContext->GetMyStaffLevel() == CLAN_MEMBER_LEVEL_MASTER)
		{
			ACTIVATE_BUTTON( m_pManagementOthersSendNoteButton);
		}
		else
		{
			DEACTIVATE_BUTTON( m_pManagementOthersSendNoteButton);
		}
	}
	else
	{
		DEACTIVATE_BUTTON( m_pManagementOthersSendNoteButton);
	}


	OnClanManagementSubscriptionButton();
	
	m_CurrentPage = CLAN_PAGE_MANAGEMENT;
}

void CStepClanMember::_InitClanManamementPhase(void)
{
	INIT_CAPTION_BUTTON(m_pManagementWritingBoardButton, GAME_STRING("STBL_IDX_CLAN_MANAGEMENT_WRITING_BOARD"));	//	�Խñ� ����
	INIT_CAPTION_BUTTON(m_pManagementMemberButton, GAME_STRING("STBL_IDX_CLAN_MANAGEMENT_MEMBER"));				//	Ŭ���� ����
	INIT_CAPTION_BUTTON(m_pManagementSubscriptionButton, GAME_STRING("STBL_IDX_CLAN_MANAGEMENT_SUBSCRIPTION"));	//	���� ����
	INIT_CAPTION_BUTTON(m_pManagementOthersButton, GAME_STRING("STBL_IDX_CLAN_MANAGEMENT_OTHERS"));				//	��Ÿ ����

	m_pManagementWritingBoardButton->SetCaptionTextSpace( 0, -5);
	m_pManagementMemberButton->SetCaptionTextSpace( 0, -5);
	m_pManagementSubscriptionButton->SetCaptionTextSpace( 0, -5);
	m_pManagementOthersButton->SetCaptionTextSpace( 0, -5);

	_InitClanManagementWritingBoard();
	_InitClanManagementMember();
	_InitClanManagementSubscriber();
	_InitClanManagementOthers();

	//	�Խñ� ���� ��ư�� Ȱ��ȭ
	ACTIVATE_BUTTON(m_pManagementWritingBoardButton);	

	//	Ŭ���� ���� ��ư�� Ȱ��ȭ
	ACTIVATE_BUTTON(m_pManagementMemberButton);

	//	��Ÿ ���� ��ư�� Ȱ��ȭ
	ACTIVATE_BUTTON(m_pManagementOthersButton);
}

void CStepClanMember::_InitClanManagementWritingBoard(void)
{
	INIT_STATIC_CAPTION(m_pManagementWritingBoardNoticeStatic, GAME_STRING("STBL_IDX_CLAN_MODIFY_NOTICE"));	//	�������� ����
	INIT_STATIC_CAPTION(m_pManagementWritingBoardGuideStatic, GAME_STRING("STBL_IDX_CLAN_MODIFY_GUIDE"));	//	Ŭ���Ұ� ����

	//	��������
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = 240;
	ime._StyleIME = IME_STYLE_MULTILINE;
	m_pManagementWritingBoardNoticeEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_BIG_SIZE, TRUE, &ime);
	m_pManagementWritingBoardNoticeEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pManagementWritingBoardNoticeEdit->SetTextColor(GetColor(GCT_VALUE));
	m_pManagementWritingBoardNoticeEdit->SetText("");
	m_pManagementWritingBoardNoticeEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pManagementWritingBoardNoticeEdit->SetTextSpace( 0, 4 );


	//	Ŭ���Ұ�
	CREATE_IME_ELEMENT ime2;
	ime2._hWnd = g_hWnd;
	ime2._hInst = g_hInstance;
	ime2._nTextLimitCount = 240;
	ime2._StyleIME = IME_STYLE_MULTILINE;
	m_pManagementWritingBoardGuideEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, 11, TRUE, &ime2);
	m_pManagementWritingBoardGuideEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pManagementWritingBoardGuideEdit->SetTextColor(GetColor(GCT_VALUE));
	m_pManagementWritingBoardGuideEdit->SetText("");
	m_pManagementWritingBoardGuideEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pManagementWritingBoardGuideEdit->SetTextSpace( 0, 4 );

	// ���þ� ���忡���� SetOutCharaProcess�� ������ ��� Ű�Է��� �ȵż� ����.
	//m_pManagementWritingBoardNoticeEdit->SetOutCharaProcess( TRUE);
	//m_pManagementWritingBoardGuideEdit->SetOutCharaProcess( TRUE);

	INIT_CAPTION_BUTTON(m_pManagementWritingBoardNoticeModify, GAME_STRING("STBL_IDX_CLAN_MODIFY_OK"));		//	�����Ϸ�
	INIT_CAPTION_BUTTON(m_pManagementWritingBoardGuideModify, GAME_STRING("STBL_IDX_CLAN_MODIFY_OK"));		//	�����Ϸ�
	m_pManagementWritingBoardNoticeModify->SetCaptionTextSpace( 0, -6);	
	m_pManagementWritingBoardNoticeModify->SetCaptionTextOffset(0.0f, -3.0f);
	m_pManagementWritingBoardGuideModify->SetCaptionTextSpace( 0, -6);
	m_pManagementWritingBoardGuideModify->SetCaptionTextOffset(0.0f, -3.0f);

	DEACTIVATE_BUTTON(m_pManagementWritingBoardNoticeModify);
	DEACTIVATE_BUTTON(m_pManagementWritingBoardGuideModify);
}

void CStepClanMember::_InitClanManagementMember(void)
{
	INIT_STATIC_CAPTION(m_pManagementMemberStatic, GAME_STRING("STBL_IDX_CLAN_MEMBER_LIST"));				//	Ŭ���� ����Ʈ
	INIT_STATIC_CAPTION2(m_pManagementInstructionStatic, GAME_STRING("STBL_IDX_CLAN_SELECTED_MEMBER"));		//	���õ� Ŭ����

	INIT_NORMAL_CAPTION(m_pManagementListHeadNick, GAME_STRING("STBL_IDX_CLAN_MEMBER_NAME"));			//	�г���
	INIT_NORMAL_CAPTION(m_pManagementListHeadClass, GAME_STRING("STBL_IDX_CLAN_MEMBER_LEVEL"));			//	����
	INIT_NORMAL_CAPTION(m_pManagementListHeadEntryDay, GAME_STRING("STBL_IDX_CLAN_MEMBER_JOIN_DATE"));	//	������

	INIT_COMBO_EX(m_pManagementInstructionButton);
	INIT_NORMAL_CAPTION(m_pManagementInstructionCombo, GAME_STRING("STBL_IDX_CLAN_CHOOSING"));	//	- ���� - 
	m_pManagementInstructionCombo->SetTextAlign( ALIGN_LEFT, ALIGN_MIDDLE);
	m_pManagementInstructionCombo->SetOffsetPos( 2.0f, 0.0f);

	m_pManagementInstructionPopup->CreateTextEx(4,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pManagementInstructionPopup->SetSelColor(100, 100, 100, 100);
	m_pManagementInstructionPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pManagementInstructionPopup->SetExclusiveEnableControl(TRUE);
	m_pManagementInstructionPopup->AddElement(GAME_STRING("STBL_IDX_CLAN_MANAGEMENT_APPOINT_MEMBER"));		//	������ �Ӹ�
	m_pManagementInstructionPopup->AddElement(GAME_STRING("STBL_IDX_CLAN_MANAGEMENT_COMMISSION_MASTER"));	//	������ ����
	m_pManagementInstructionPopup->AddElement(GAME_STRING("STBL_IDX_CLAN_MANAGEMENT_DISMISSAL_MEMBER"));	//	���� ����
	m_pManagementInstructionPopup->AddElement(GAME_STRING("STBL_IDX_CLAN_MANAGEMENT_DEMOTE_MEMBER"));		//	�Ϲ����� ����
	m_pManagementInstructionPopup->SetEnable(FALSE);

	for(INT32 i=0; i<MANAGEMENT_LIST_ROW_MAX; i++)
	{
		INIT_SLOT(m_slotManagementList[i].pTitle);
		INIT_SLOT2(m_slotManagementList[i].pNick);
		INIT_SLOT(m_slotManagementList[i].pClass);
		INIT_SLOT(m_slotManagementList[i].pEntryDay);
		m_slotManagementList[i].pEntryDay->SetTextSpace( 0, -3);

		m_slotManagementList[i].pSlot->OnUpdate(0.0f);
	}	

	m_pManagementListHeadCheckAll->setCheckBoxState(FALSE);

	m_pManagementMemberScrollList->CreateTextEx(MANAGEMENT_LIST_ROW_MAX,  GetDefaultFontName());
	m_pManagementMemberScrollList->SetSelColor(0, 0, 0, 0);
}	

void CStepClanMember::_InitClanManagementSubscriber(void)
{
	INIT_STATIC_CAPTION(m_pManagementSubscriberStatic, GAME_STRING("STBL_IDX_CLAN_SUBSCRIBER_LIST"));		//	���Դ�� ���� ����Ʈ
	INIT_STATIC_CAPTION2(m_pSubscriptionInstructionStatic, GAME_STRING("STBL_IDX_CLAN_SELECTED_USER"));		//	���õ� ����
	m_pSubscriptionInstructionStatic->SetTextAlign( ALIGN_RIGHT, ALIGN_MIDDLE);

	INIT_NORMAL_CAPTION(m_pSubscriberListHeadNick, GAME_STRING("STBL_IDX_CLAN_MEMBER_NAME"));					//	�г���
	INIT_NORMAL_CAPTION(m_pSubscriberListHeadEntryDay, GAME_STRING("STBL_IDX_CLAN_MEMBER_REQUEST_DATE"));		//	��û��

	INIT_COMBO_EX(m_pSubscriptionInstructionButton);
	INIT_NORMAL_CAPTION(m_pSubscriptionInstructionCombo, GAME_STRING("STBL_IDX_CLAN_CHOOSING"));	//	- ���� -
	m_pSubscriptionInstructionCombo->SetTextAlign( ALIGN_LEFT, ALIGN_MIDDLE);
	m_pSubscriptionInstructionCombo->SetOffsetPos( 2.0f, 0.0f);

	m_pSubscriptionInstructionPopup->CreateTextEx(3,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pSubscriptionInstructionPopup->SetSelColor(100, 100, 100, 100);
	m_pSubscriptionInstructionPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSubscriptionInstructionPopup->SetExclusiveEnableControl(TRUE);
	m_pSubscriptionInstructionPopup->AddElement(GAME_STRING("STBL_IDX_CLAN_DETAIL_VIEW"));				//	�󼼺���
	m_pSubscriptionInstructionPopup->AddElement(GAME_STRING("STBL_IDX_CLAN_SUBSCRIBER_INTROMISSION"));	//	���� ����
	m_pSubscriptionInstructionPopup->AddElement(GAME_STRING("STBL_IDX_CLAN_SUBSCRIBER_REJECTION"));		//	���� �ź�
	m_pSubscriptionInstructionPopup->SetEnable(FALSE);

	// ����Ʈ ����Ʈ, ��Ŀ�� �ʱ�ȭ
	m_pManagementSubscriberFocus->SetEnable(FALSE);
	m_pManagementSubscriberSelection->SetEnable(FALSE);

	for(INT32 i=0; i<MANAGEMENT_LIST_ROW_MAX; i++)
	{
		INIT_SLOT(m_slotSubscriptionList[i].pTitle);
		INIT_SLOT2(m_slotSubscriptionList[i].pNick);
		INIT_SLOT(m_slotSubscriptionList[i].pEntryDay);
		m_slotSubscriptionList[i].pEntryDay->SetTextSpace( 0, -3);
		
		m_slotSubscriptionList[i].pSlot->OnUpdate(0.0f);
	}	

	m_pSubscriberListHeadCheckAll->setCheckBoxState(FALSE);

	m_pManagementSubscriberScrollList->CreateTextEx(SUBSCRIPTION_LIST_ROW_MAX,  GetDefaultFontName());
	m_pManagementSubscriberScrollList->SetSelColor(0, 0, 0, 0);
}

void CStepClanMember::_InitClanManagementOthers(void)
{
	INIT_STATIC_CAPTION( m_pManagementOthersAuthorityStatic, GAME_STRING("STBL_IDX_CLAN_AUTHORITY_STAFF"));			//	������ ���� ����
	INIT_STATIC_CAPTION( m_pManagementOthersWritingAuthorityStatic, GAME_STRING("STBL_IDX_CLAN_AUTHORITY_WRITING"));	//	�Խñ� ���� ����
	INIT_STATIC_CAPTION( m_pManagementOthersInviteAuthorityStatic, GAME_STRING("STBL_IDX_CLAN_AUTHORITY_INVITE"));		//	Ŭ�� �ʴ� ����
	INIT_STATIC_CAPTION( m_pManagementOthersDimissalAuthorityStatic, GAME_STRING("STBL_IDX_CLAN_AUTHORITY_DISMISSAL"));	//	Ŭ���� ���� ����
	INIT_STATIC_CAPTION( m_pManagementOthersSubscriptionAuthorityStatic, GAME_STRING("STBL_IDX_CLAN_AUTHORITY_SUBSCRIPTION"));	//	���� ���� ����

	INIT_CHECK_EX( m_pManagementOthersWritingAuthorityCheck);		//	�Խñ� ���� ���� üũ�ڽ�
	INIT_CHECK_EX( m_pManagementOthersInviteAuthorityCheck);		//	Ŭ�� �ʴ� ���� üũ�ڽ�
	INIT_CHECK_EX( m_pManagementOthersDimissalAuthorityCheck);		//	Ŭ���� ���� ���� üũ�ڽ�
	INIT_CHECK_EX( m_pManagementOthersSubscriptionAuthorityCheck);	//	���� ���� ���� üũ�ڽ�	

	INIT_STATIC_CAPTION( m_pManagementOthersSendNoteStatic, GAME_STRING("STBL_IDX_CLAN_SEND_NOTE_STATIC"));		//	Ŭ�� ��ü ���� ������
	INIT_CAPTION_BUTTON( m_pManagementOthersSendNoteButton, GAME_STRING("STBL_IDX_CLAN_SEND_NOTE"));				//	���� ������

	DEACTIVATE_BUTTON( m_pManagementOthersSendNoteButton);

	INIT_STATIC_CAPTION( m_pManagementOthersAdmissionLimitStatic, GAME_STRING("STBL_IDX_CLAN_ADMISSION_LIMIT_RANK"));	//	Ŭ�� ���� ���� ����
	INIT_STATIC_CAPTION( m_pManagementOthersAdmissionLimitRankStatic, GAME_STRING("STBL_IDX_CLAN_MEMBER_RANK"));		//	���

	INIT_NORMAL_CAPTION(m_pManagementOthersAdmissionLimitRankCombo, GAME_STRING("STBL_IDX_CLAN_CHOOSING"));	//	- ���� - 
	INIT_COMBO_EX(m_pManagementOthersAdmissionLimitRankButton);
		
	m_pManagementOthersAdmissionLimitRankPopup->CreateTextEx(g_nAdmissionLimitRankCount,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pManagementOthersAdmissionLimitRankPopup->SetSelColor(100, 100, 100, 100);
	m_pManagementOthersAdmissionLimitRankPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pManagementOthersAdmissionLimitRankPopup->SetExclusiveEnableControl(TRUE);
	m_pManagementOthersAdmissionLimitRankPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pManagementOthersAdmissionLimitRankPopup->SetEnable(FALSE);	
	

	for(INT32 j=0; j<g_nAdmissionLimitRankCount; j++)
	{
		m_pManagementOthersAdmissionLimitRankPopup->AddElement( g_pszAdmissionLimitRankItem[j]);
	}

	INIT_STATIC_CAPTION( m_pManagementOthersAdmissionLimitAgeStatic, GAME_STRING("STBL_IDX_CLAN_MEMBER_AGE"));		//	����

	
	//	���� ���� ����	
	m_pManagementOthersAdmissionLimitAgePopup[0]->CreateTextEx(g_nAdmissionLimitAgeLowCount,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pManagementOthersAdmissionLimitAgePopup[0]->SetSelColor(100, 100, 100, 100);
	m_pManagementOthersAdmissionLimitAgePopup[0]->SetTextColor(GetColor(GCT_DEFAULT));
	m_pManagementOthersAdmissionLimitAgePopup[0]->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pManagementOthersAdmissionLimitAgePopup[0]->SetExclusiveEnableControl(TRUE);
	m_pManagementOthersAdmissionLimitAgePopup[0]->SetEnable(FALSE);

	for(INT32 j=0; j<g_nAdmissionLimitAgeLowCount; j++)
	{
		m_pManagementOthersAdmissionLimitAgePopup[0]->AddElement( g_pszAdmissionLimitAgeLowItem[j]);
	}

	//	���� ���� �̻�
	m_pManagementOthersAdmissionLimitAgePopup[1]->CreateTextEx(g_nAdmissionLimitAgeHighCount,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pManagementOthersAdmissionLimitAgePopup[1]->SetSelColor(100, 100, 100, 100);
	m_pManagementOthersAdmissionLimitAgePopup[1]->SetTextColor(GetColor(GCT_DEFAULT));
	m_pManagementOthersAdmissionLimitAgePopup[1]->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pManagementOthersAdmissionLimitAgePopup[1]->SetExclusiveEnableControl(TRUE);
	m_pManagementOthersAdmissionLimitAgePopup[1]->SetEnable(FALSE);

	for(INT32 j=0; j<g_nAdmissionLimitAgeHighCount; j++)
	{
		m_pManagementOthersAdmissionLimitAgePopup[1]->AddElement( g_pszAdmissionLimitAgeHighItem[j]);
	}

	for(INT32 i=0; i<ADMISSION_LIMIT_CONTROL_COUNT; i++)
	{
		INIT_NORMAL_CAPTION(m_pManagementOthersAdmissionLimitAgeCombo[i], GAME_STRING("STBL_IDX_CLAN_CHOOSING"));	//	- ���� - 
		INIT_COMBO_EX(m_pManagementOthersAdmissionLimitAgeButton[i]);
	}

	INIT_STATIC_CAPTION( m_pManagementOthersChangeClanNameStatic, GAME_STRING("STBL_IDX_CLAN_CHANGE_CLAN_NAME_STATIC"));		//	Ŭ���� �����ϱ�
	INIT_CAPTION_BUTTON( m_pManagementOthersChangeClanNameButton, GAME_STRING("STBL_IDX_CLAN_CHANGE_CLAN_NAME"));			//	Ŭ���� ����

	INIT_STATIC_CAPTION( m_pManagementOthersChangeClanMarkStatic, GAME_STRING("STBL_IDX_CLAN_CHANGE_CLAN_MARK_STATIC"));	//	Ŭ�� ��ũ �����ϱ�
	INIT_CAPTION_BUTTON( m_pManagementOthersChangeClanMarkButton, GAME_STRING("STBL_IDX_CLAN_CHANGE_CLAN_MARK"));		//	Ŭ�� ��ũ ����

	INIT_STATIC_CAPTION( m_pManagementOthersDissolutionStatic, GAME_STRING("STBL_IDX_CLAN_DISSOLUTION_STATIC"));		//	Ŭ�� ��ü�ϱ�
	INIT_CAPTION_BUTTON( m_pManagementOthersDissolutionButton, GAME_STRING("STBL_IDX_CLAN_DISSOLUTION"));			//	��ü�ϱ�

	INIT_CAPTION_BUTTON( m_pManagementOthersModifyButton, GAME_STRING("STBL_IDX_CLAN_APPLY"));	//	�����ϱ�

	DEACTIVATE_BUTTON( m_pManagementOthersModifyButton );

	DEACTIVATE_BUTTON( m_pManagementOthersChangeClanNameButton);
	DEACTIVATE_BUTTON( m_pManagementOthersChangeClanMarkButton);

	if ( g_pConfigEx->GetClan().Enable_Cancellation )
	{
		ACTIVATE_BUTTON( m_pManagementOthersDissolutionButton);
	}
	else
	{
		DEACTIVATE_BUTTON( m_pManagementOthersDissolutionButton);
	}
}

void CStepClanMember::_UpdateClanManagementPhase(RT_REAL32 rDeltaSeconds)
{
	if( m_pManagementWritingBoardPhase->isEnable())
	{
		_UpdateClanManagementWritingBoardPhase(rDeltaSeconds);
	}
	else if( m_pManagementMemberPhase->isEnable())
	{
		_UpdateClanManagementMemberPhase(rDeltaSeconds);
	}
	else if( m_pManagementSubscriptionPhase->isEnable())
	{
		_UpdateClanManagementSubscriberPhase(rDeltaSeconds);
	}
	else if( m_pManagementOthersPhase->isEnable())
	{
		_UpdateClanManagementOthersPhase(rDeltaSeconds);
	}
}

void CStepClanMember::_UpdateClanManagementWritingBoardPhase(RT_REAL32 rDeltaSeconds)
{
	if( i3String::Compare( m_pClanContext->GetMyClanDetailInfo()->_announce, m_pManagementWritingBoardNoticeEdit->getIMETextBuf() ) != 0 )
	{
		ACTIVATE_BUTTON(m_pManagementWritingBoardNoticeModify);		
	}
	else
	{
		DEACTIVATE_BUTTON( m_pManagementWritingBoardNoticeModify );
	}

 	if( i3String::Compare( m_pClanContext->GetMyClanDetailInfo()->_intro, m_pManagementWritingBoardGuideEdit->getIMETextBuf() ) != 0 )
	{
		ACTIVATE_BUTTON(m_pManagementWritingBoardGuideModify);
	}
	else
	{
		DEACTIVATE_BUTTON( m_pManagementWritingBoardGuideModify );
	}	
}

INT32 CStepClanMember::cbSortManageMemberListByName(void* item1, void* item2)
{
	ManagementClanMemberInfo * pInfo1 = (ManagementClanMemberInfo*) item1;
	ManagementClanMemberInfo * pInfo2 = (ManagementClanMemberInfo*) item2;

	//	�̸���
	if( CStepClanMember::m_sManageClanMemberSortType == CLAN_SORT_NAME_ASC)
	{
		return CompareString(pInfo1->_info._nick, pInfo2->_info._nick, TRUE);
	}
	else if( CStepClanMember::m_sManageClanMemberSortType == CLAN_SORT_NAME_DESC)
	{
		return CompareString(pInfo1->_info._nick, pInfo2->_info._nick, FALSE);
	}

	return 0;
}

INT32 CStepClanMember::cbSortManageMemberListByLevel(void* item1, void* item2)
{
	ManagementClanMemberInfo * pInfo1 = (ManagementClanMemberInfo*) item1;
	ManagementClanMemberInfo * pInfo2 = (ManagementClanMemberInfo*) item2;

	//	���޼�
	if( CStepClanMember::m_sManageClanMemberSortType == CLAN_SORT_LEVEL_ASC)
	{
		return COMPARE_ASC(pInfo1->_info._level, pInfo2->_info._level);
	}
	else if( CStepClanMember::m_sManageClanMemberSortType == CLAN_SORT_LEVEL_DESC)
	{
		return COMPARE_DESC(pInfo1->_info._level, pInfo2->_info._level);
	}

	return 0;
}

INT32 CStepClanMember::cbSortManageMemberListByDate(void* item1, void* item2)
{
	ManagementClanMemberInfo * pInfo1 = (ManagementClanMemberInfo*) item1; 
	ManagementClanMemberInfo * pInfo2 = (ManagementClanMemberInfo*) item2;

	//	�����ϼ�
	if( CStepClanMember::m_sManageClanMemberSortType == CLAN_SORT_DATE_ASC)
	{
		return COMPARE_ASC(pInfo1->_info._date, pInfo2->_info._date);
	}
	else if( CStepClanMember::m_sManageClanMemberSortType == CLAN_SORT_DATE_DESC)
	{
		return COMPARE_DESC(pInfo1->_info._date, pInfo2->_info._date);
	}

	return 0;
}

INT32 CStepClanMember::cbSortInfoMemberListByName(void* item1, void* item2)
{
	ManagementClanMemberInfo * pInfo1 = (ManagementClanMemberInfo*) item1;
	ManagementClanMemberInfo * pInfo2 = (ManagementClanMemberInfo*) item2;

	//	�̸���
	if( CStepClanMember::m_sInfoClanMemberSortType == CLAN_SORT_NAME_ASC)
	{
		return CompareString(pInfo1->_info._nick, pInfo2->_info._nick, TRUE);
	}
	else if( CStepClanMember::m_sInfoClanMemberSortType == CLAN_SORT_NAME_DESC)
	{
		return CompareString(pInfo1->_info._nick, pInfo2->_info._nick, FALSE);
	}

	return 0;
}

INT32 CStepClanMember::cbSortInfoMemberListByLevel(void* item1, void* item2)
{
	ManagementClanMemberInfo * pInfo1 = (ManagementClanMemberInfo*) item1;
	ManagementClanMemberInfo * pInfo2 = (ManagementClanMemberInfo*) item2;

	//	���޼�
	if( CStepClanMember::m_sInfoClanMemberSortType == CLAN_SORT_LEVEL_ASC)
	{
		return COMPARE_ASC(pInfo1->_info._level, pInfo2->_info._level);
	}
	else if( CStepClanMember::m_sInfoClanMemberSortType == CLAN_SORT_LEVEL_DESC)
	{
		return COMPARE_DESC(pInfo1->_info._level, pInfo2->_info._level);
	}

	return 0;
}

INT32 CStepClanMember::cbSortInfoMemberListByState(void* item1, void* item2)
{
	ManagementClanMemberInfo * pInfo1 = (ManagementClanMemberInfo*) item1;
	ManagementClanMemberInfo * pInfo2 = (ManagementClanMemberInfo*) item2;

	UINT32 state1 = static_cast<UINT32>(GET_FRIEND_STATE(pInfo1->_info._state));
	UINT32 state2 = static_cast<UINT32>(GET_FRIEND_STATE(pInfo2->_info._state));

	//	���� ���¼� (FRIEND_STATE�� ������ �ȴ�.)
	if( CStepClanMember::m_sInfoClanMemberSortType == CLAN_SORT_STATE_ASC)
	{
		return COMPARE_ASC(state1, state2);
	}
	else if( CStepClanMember::m_sInfoClanMemberSortType == CLAN_SORT_STATE_DESC)
	{
		return COMPARE_DESC(state1, state2);
	}

	return 0;
}

INT32 CStepClanMember::cbSortSubscriberListByName(void* item1, void* item2)
{
	ManagementSubscriberInfo * pInfo1 = (ManagementSubscriberInfo*) item1;
	ManagementSubscriberInfo * pInfo2 = (ManagementSubscriberInfo*) item2;

	//	�̸���
	if( CStepClanMember::m_sManageSubscriberSortType == CLAN_SORT_NAME_ASC)
	{
		return CompareString(pInfo1->_info._nick, pInfo2->_info._nick, TRUE);
	}
	else if( CStepClanMember::m_sManageSubscriberSortType == CLAN_SORT_NAME_DESC)
	{
		return CompareString(pInfo1->_info._nick, pInfo2->_info._nick, FALSE);
	}

	return 0;
}

INT32 CStepClanMember::cbSortSubscriberListByDate(void* item1, void* item2)
{
	ManagementSubscriberInfo * pInfo1 = (ManagementSubscriberInfo*) item1;
	ManagementSubscriberInfo * pInfo2 = (ManagementSubscriberInfo*) item2;

	//	���� ���¼� (FRIEND_STATE�� ������ �ȴ�.)
	if( CStepClanMember::m_sManageSubscriberSortType == CLAN_SORT_DATE_ASC)
	{
		return COMPARE_ASC(pInfo1->_info._date, pInfo2->_info._date);
	}
	else if( CStepClanMember::m_sManageSubscriberSortType == CLAN_SORT_DATE_DESC)
	{
		return COMPARE_DESC(pInfo1->_info._date, pInfo2->_info._date);
	}

	return 0;
}

void CStepClanMember::_UpdateClanManagementMemberPhase(RT_REAL32 rDeltaSeconds)
{
	CLAN_DETAIL_INFO * pClanDetailInfo = m_pClanContext->GetMyClanDetailInfo();
	I3ASSERT( pClanDetailInfo);

	//	Ŭ���� ���� ������Ʈ
	m_pClanContext->UpdateClanMember(rDeltaSeconds);

	//	�ɹ� ����Ʈ �籸��
	_UpdateSortClanMemberList();

	INT32 count = m_listManageMember.GetCount();

	m_pManagementMemberScrollList->Clear();
	for(INT32 i=0; i<count; i++)
		m_pManagementMemberScrollList->AddElement("");

	for(INT32 i=0; i<MANAGEMENT_LIST_ROW_MAX; i++)
	{
		BOOL bValid = FALSE;
		ManagementClanMemberInfo * pMember = NULL;

		INT32 idx = i + m_idxClanManagementMemberStartSlot;

		if( idx < count)
		{
			pMember = (ManagementClanMemberInfo*) m_listManageMember.GetItem(idx);

			if(pMember != NULL)
			{
				if(pMember->_info._i64UID > 0)
					bValid = TRUE;
			}
		}

		if( bValid)
		{
			//	EntryDate�� ���� GameContext���� ������ ���� ������ �ӽ÷� 0���� �����մϴ�.
			//	���� GameContext�� ������ ���� ���ڷ� �־�� �մϴ�. komet

			_SetManagementListSlot(i, pMember);	//	���� �� ����

			//	������ �ڽ��� ���			
			if( CLAN_MEMBER_LEVEL_MASTER == pMember->_info._level )  
			{
				m_slotManagementList[i].pCheck->SetEnable(FALSE);
			}
		}
		else
		{
			_ResetManagementListSlot(i);	//	�� ����
		}
	}	
}

void CStepClanMember::_UpdateClanManagementSubscriberPhase(RT_REAL32 rDeltaSeconds)
{
	BOOL bValid = FALSE;

	//	���� ����� ���� ������Ʈ
	m_pClanContext->UpdateClanSubscriber(rDeltaSeconds);

	//	�ɹ� ����Ʈ �籸��
	_UpdateSortSubscriberList();

	INT32 count = m_listManageSubscriber.GetCount();

	m_pManagementSubscriberScrollList->Clear();
	for(INT32 i=0; i<count; i++)
		m_pManagementSubscriberScrollList->AddElement("");

	for(INT32 i=0; i<SUBSCRIPTION_LIST_ROW_MAX; i++)
	{
		if( i >= count)
		{
			_ResetSubscriptionListSlot(i);	//	�� ����
		}
		else
		{
			INT32 idx = i + m_idxClanManagementSubscriberStartSlot;

			ManagementSubscriberInfo * pSubscriber = (ManagementSubscriberInfo*) m_listManageSubscriber.GetItem(idx);

			if( pSubscriber != NULL)
			{
				if( pSubscriber->_info._i64UID > 0)
					bValid = TRUE;
			}
			
			if(bValid)
				_SetSubscriptionListSlot(i, pSubscriber);	//	���� �� ����
			else
				_ResetSubscriptionListSlot(i);		//	�� ����
		}
	}
}

void CStepClanMember::_UpdateClanManagementOthersPhase(RT_REAL32 rDeltaSeconds)
{ 
	CLAN_MANAGEMENT modify;

	//	���� ��� ����
	switch(m_pManagementOthersAdmissionLimitRankPopup->getSelectedElement())
	{
	case 0:		modify._limitRank = CLAN_LIMIT_RANK_1;		break;		//	��ü ���
	case 1:		modify._limitRank = CLAN_LIMIT_RANK_2;		break;		//	��� ���
	case 2:		modify._limitRank = CLAN_LIMIT_RANK_3;		break;		//	���� ���
	case 3:		modify._limitRank = CLAN_LIMIT_RANK_4;		break;		//	���� ���
	case 4:		modify._limitRank = CLAN_LIMIT_RANK_5;		break;		//	�强 ���
	default:	modify._limitRank = CLAN_LIMIT_RANK_1;		break;
	}	
	//	���� ���� ���� ����
	switch(m_pManagementOthersAdmissionLimitAgePopup[0]->getSelectedElement())
	{
	case 0:		modify._limitAgeLow = CLAN_LIMIT_AGE_1;		break;		//	��ü ���
	case 1:		modify._limitAgeLow = CLAN_LIMIT_AGE_2;		break;		//	15�� �̻�
	case 2:		modify._limitAgeLow = CLAN_LIMIT_AGE_3;		break;		//	20�� �̻�
	case 3:		modify._limitAgeLow = CLAN_LIMIT_AGE_4;		break;		//	30�� �̻�
	default:	modify._limitAgeLow = CLAN_LIMIT_AGE_1;		break;
	}
	//	���� ���� ���� ����
	switch(m_pManagementOthersAdmissionLimitAgePopup[1]->getSelectedElement())
	{
	case 0:		modify._limitAgeHigh = CLAN_LIMIT_AGE_1;	break;		//	��ü ���
	case 1:		modify._limitAgeHigh = CLAN_LIMIT_AGE_2;	break;		//	15�� ����
	case 2:		modify._limitAgeHigh = CLAN_LIMIT_AGE_3;	break;		//	20�� ����
	case 3:		modify._limitAgeHigh = CLAN_LIMIT_AGE_4;	break;		//	30�� ����
	default:	modify._limitAgeHigh = CLAN_LIMIT_AGE_1;	break;
	}

	//	�Խ��� ���� ����
	UINT8 wr = (UINT8) m_pManagementOthersWritingAuthorityCheck->getCheckBoxState();
	//	Ŭ�� �ʴ� ����
	UINT8 in = (UINT8) m_pManagementOthersInviteAuthorityCheck->getCheckBoxState();
	//	Ŭ�� �ɹ� ���� ����
	UINT8 de = (UINT8) m_pManagementOthersDimissalAuthorityCheck->getCheckBoxState();
	//	���� ���� ����
	UINT8 ac = (UINT8) m_pManagementOthersSubscriptionAuthorityCheck->getCheckBoxState();

	modify._authority = CLAN_MENAGE_AUTHORITY( wr, in, de, ac);

	if( m_pClanContext->GetMyClanDetailInfo()->_authstaff	!= modify._authority	||
		m_pClanContext->GetMyClanDetailInfo()->_ranklock	!= modify._limitRank	||
		m_pClanContext->GetMyClanDetailInfo()->_lagelock	!= modify._limitAgeLow	||
		m_pClanContext->GetMyClanDetailInfo()->_hagelock	!= modify._limitAgeHigh	)
	{		
		//	������ ���� ������ ����
		ACTIVATE_BUTTON( m_pManagementOthersModifyButton );
	}
	else
	{
		DEACTIVATE_BUTTON( m_pManagementOthersModifyButton );
	}
}

void CStepClanMember::_UpdateSortClanMemberList(void)
{		
#if defined(TEST_CLAN_MEMBER_LIST)
	INT32 count = 20;

	//	�׽�Ʈ������ ���� �߰�
	if( m_listManageMember.GetCount() != count) 
	{
		char temp[NET_NICK_NAME_SIZE] = "";

		m_listManageMember.Clear();
		
		for(INT32 i=0; i<count; i++) 
		{
			m_ManageMemberInfo[i]._info._i64UID = i + 1000;
			m_ManageMemberInfo[i]._info._idx = (UINT8) i;
			m_ManageMemberInfo[i]._info._rank = (UINT8) (rand() % 50);
			m_ManageMemberInfo[i]._info._level = (UINT8) (rand() % (CLAN_MEMBER_LEVEL_MAX-1) + 1);
			m_ManageMemberInfo[i]._info._state = MAKE_FRIEND_STATE( rand() % FRIEND_STATE_ROOM+1, rand() % 5, rand() % 10, rand() % 100);
			m_ManageMemberInfo[i]._info._date =	(2008 * 10000) + ((rand() % 12 + 1) * 100) + (rand() % 30 + 1);

			sprintf_s(temp, "TEST NAME (%d)", i+1);
			i3String::Copy(m_ManageMemberInfo[i]._info._nick, temp);

			m_listManageMember.Add((void*) &m_ManageMemberInfo[i]);
		}	
	}
#else
	INT32 nNewCount = m_pClanContext->GetMyClanMemberCount(); 
	INT32 nOldCount = m_listManageMember.GetCount();

	//	Ŭ���� ���� ����Ʈ �籸��
	if( nOldCount != nNewCount)
	{
		m_listManageMember.Clear();
		
		for(INT32 i=0; i<nNewCount; i++)
		{
			const CLAN_MEMBER_BASIC_INFO * pInfo = m_pClanContext->GetMyClanMember(i);

			if( pInfo->_i64UID > 0)
			{
				i3mem::Copy( &m_ManageMemberInfo[i]._info, pInfo, sizeof(CLAN_MEMBER_BASIC_INFO));

				m_listManageMember.Add((void*) &m_ManageMemberInfo[i]);
			}
			else
			{
				i3mem::FillZero( &m_ManageMemberInfo[i], sizeof(m_ManageMemberInfo[i]));
			}
		}	

		if( nOldCount > nNewCount)	//	������ �κ��� �ʱ�ȭ �Ѵ�.
		{
			for(INT32 i=nNewCount; i<nOldCount; i++)
				i3mem::FillZero( &m_ManageMemberInfo[i], sizeof(m_ManageMemberInfo[i]));
		}
	}
	//	����� ������ ����
	else
	{
		for(INT32 i=0; i<nNewCount; i++)
		{
			const CLAN_MEMBER_BASIC_INFO * pInfo = m_pClanContext->GetMyClanMember(i);
			i3mem::Copy( &m_ManageMemberInfo[i]._info, pInfo, sizeof(CLAN_MEMBER_BASIC_INFO));
		}
	}
#endif

}

void CStepClanMember::_UpdateSortSubscriberList(void)
{
#if defined(TEST_CLAN_SUBSCRITION_LIST)
	INT32 count = 20;

	if( m_listManageSubscriber.GetCount() != count) 
	{
		char temp[NET_NICK_NAME_SIZE] = "";

		m_listManageSubscriber.Clear();
		
		for(INT32 i=0; i<count; i++) 
		{
			m_ManageSubscriberInfo[i]._info._i64UID = i + 1000;
			m_ManageSubscriberInfo[i]._info._idx = (UINT8) i;
			m_ManageSubscriberInfo[i]._info._rank = (UINT8) (rand() % 50);
			m_ManageSubscriberInfo[i]._info._date =	(2008 * 10000) + ((rand() % 12 + 1) * 100) + (rand() % 30 + 1);

			sprintf_s(temp, "TEST NAME (%d)", i+1);
			i3String::Copy(m_ManageSubscriberInfo[i]._info._nick, temp);

			m_listManageSubscriber.Add((void*) &m_ManageSubscriberInfo[i]);
		}	
	}
#else
	//	���� ����� ���� ����Ʈ �籸��
	INT32 nNewCount = m_pClanContext->GetMyClanSubscriberCount(); 
	INT32 nOldCount = m_listManageSubscriber.GetCount();

	if( nNewCount > 100 ) nNewCount = 100;
	if( nOldCount > 100 ) nOldCount = 100;

	if( nOldCount != nNewCount)
	{
		m_listManageSubscriber.Clear();
		
		for(INT32 i=0; i<nNewCount; i++)
		{
			if( i >= MAX_REQUEST_COUNT)		
			{
				I3TRACE("[StepClanMember] Clan subscribers are over count 100 [%d]", i); 

				continue;				
			}

			CLAN_REQUEST_BASIC_INFO * pInfo = m_pClanContext->GetMyClanSubscriber(i);

			i3mem::Copy( &m_ManageSubscriberInfo[i]._info, pInfo, sizeof(CLAN_REQUEST_BASIC_INFO));

			if( pInfo->_i64UID > 0)
			{
				m_listManageSubscriber.Add((void*) &m_ManageSubscriberInfo[i]);
			}			
			else
			{
				i3mem::FillZero( &m_ManageSubscriberInfo[i], sizeof(m_ManageSubscriberInfo[i]));
			}
		}	

		if( nOldCount > nNewCount)	//	������ �κ��� �ʱ�ȭ �Ѵ�.
		{
			for(INT32 i=nNewCount; i<nOldCount; i++)
			{
				if( i >= MAX_REQUEST_COUNT)	
					continue;		

				i3mem::FillZero( &m_ManageSubscriberInfo[i], sizeof(m_ManageSubscriberInfo[i]));
			}
		}
	}
	//	����� ������ ����
	else
	{
		for(INT32 i=0; i<nNewCount; i++)
		{
			if( i >= MAX_REQUEST_COUNT)		
				continue;		

			CLAN_REQUEST_BASIC_INFO * pInfo = m_pClanContext->GetMyClanSubscriber(i);

			i3mem::Copy( &m_ManageSubscriberInfo[i]._info, pInfo, sizeof(CLAN_REQUEST_BASIC_INFO));
		}
	}
#endif
}

void CStepClanMember::OnClanManagementWritingBoardButton(void)
{
	if( FALSE == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_WRITE ) )
	{
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));		//	"�ش� ��ɿ� ���� ������ ������ �����Ǿ� ���� �ʽ��ϴ�."
		return;
	}

	m_pManagementWritingBoardPhase->SetEnable(TRUE);
	m_pManagementMemberPhase->SetEnable(FALSE);
	m_pManagementSubscriptionPhase->SetEnable(FALSE);
	m_pManagementOthersPhase->SetEnable(FALSE);

	m_pManagementWritingBoardButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);
  	m_pManagementMemberButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
 	m_pManagementSubscriptionButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
  	m_pManagementOthersButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	
  	m_pManagementWritingBoardNoticeEdit->SetIMEText( m_pClanContext->GetMyClanDetailInfo()->_announce );
	m_pManagementWritingBoardGuideEdit->SetIMEText( m_pClanContext->GetMyClanDetailInfo()->_intro );

	m_pManagementWritingBoardNoticeEdit->SetFocus(TRUE);
}

void CStepClanMember::OnClanManagementMemberButton(void)
{
	m_pManagementWritingBoardPhase->SetEnable(FALSE);
	m_pManagementMemberPhase->SetEnable(TRUE);
	m_pManagementSubscriptionPhase->SetEnable(FALSE);
	m_pManagementOthersPhase->SetEnable(FALSE);

	m_pManagementWritingBoardButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pManagementMemberButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pManagementSubscriptionButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pManagementOthersButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	for(INT32 i=0; i<MANAGEMENT_LIST_ROW_MAX; i++)
		_ResetManagementListSlot(i);

	m_idxClanManagementMemberStartSlot = 0;
	m_pManagementMemberScrollList->SetCurValue(0.0f);

	//	üũ ����
	m_pManagementListHeadCheckAll->setCheckBoxState(FALSE);

	i3mem::FillZero( m_ManageMemberInfo, sizeof(m_ManageMemberInfo));
	m_listManageMember.Clear();
	m_listManageMember.SetCompareProc(cbSortManageMemberListByLevel);
	m_sManageClanMemberSortType = CLAN_SORT_LEVEL_ASC;

	//	������ Ŭ�� ��� ����Ʈ ��û
	m_pClanContext->EnterClanMember();
}

void CStepClanMember::OnClanManagementSubscriptionButton(void)
 {
	// �� ������
	//m_pClanContext->MatchTeamCreate( 7 );
	//m_pClanContext->MatchTeamJoin( 0 );
	//m_pClanContext->MatchTeamLeave( 0 );
	//m_pClanContext->MatchFightRequest( 0, 1 );
	//m_pClanContext->MatchFightAccect( 1, 0, 1 );
	//m_pClanContext->MatchTeamLeave( g_pGameContext->getClanBattleTeamInfo()->_Idx );	

	m_pManagementWritingBoardPhase->SetEnable(FALSE);
	m_pManagementMemberPhase->SetEnable(FALSE);
	m_pManagementSubscriptionPhase->SetEnable(TRUE);
	m_pManagementOthersPhase->SetEnable(FALSE);

	m_pManagementWritingBoardButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pManagementMemberButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pManagementSubscriptionButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pManagementOthersButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	for(INT32 i=0; i<SUBSCRIPTION_LIST_ROW_MAX; i++)
		_ResetSubscriptionListSlot(i);

	m_idxClanManagementSubscriberStartSlot = 0;
	m_pManagementSubscriberScrollList->SetCurValue(0.0f);

	//	üũ ����
	m_pSubscriberListHeadCheckAll->setCheckBoxState(FALSE); 

	i3mem::FillZero( m_ManageSubscriberInfo, sizeof(m_ManageSubscriberInfo));
	m_listManageSubscriber.Clear();
	m_listManageSubscriber.SetCompareProc(cbSortSubscriberListByDate);
	m_sManageSubscriberSortType = CLAN_SORT_DATE_DESC;

	//	������ ���� ����� ����Ʈ ��û
	m_pClanContext->EnterClanSubscriber();

	m_nLastedSelectSlot = -1;
}

void CStepClanMember::OnClanManagementOthersButton(void)
{
	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
	{
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"������ ������ �ʿ��մϴ�."
		return;
	}

	m_pManagementWritingBoardPhase->SetEnable(FALSE);
	m_pManagementMemberPhase->SetEnable(FALSE);
	m_pManagementSubscriptionPhase->SetEnable(FALSE);
	m_pManagementOthersPhase->SetEnable(TRUE);

	m_pManagementWritingBoardButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pManagementMemberButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
 	m_pManagementSubscriptionButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pManagementOthersButton->setControlState(I3GUI_CONTROL_STATE_DISABLED); 

	// üũ�ڽ� �ʱ�ȭ
	if( GET_CLAN_MENAGE_WRITE( m_pClanContext->GetMyClanDetailInfo()->_authstaff ) )
		m_pManagementOthersWritingAuthorityCheck->setCheckBoxState( TRUE );
	else
		m_pManagementOthersWritingAuthorityCheck->setCheckBoxState( FALSE );

	if( GET_CLAN_MENAGE_INVITED( m_pClanContext->GetMyClanDetailInfo()->_authstaff ))
		m_pManagementOthersInviteAuthorityCheck->setCheckBoxState( TRUE );
	else
		m_pManagementOthersInviteAuthorityCheck->setCheckBoxState( FALSE );

	if( GET_CLAN_MENAGE_DEPORTATION( m_pClanContext->GetMyClanDetailInfo()->_authstaff ))
		m_pManagementOthersDimissalAuthorityCheck->setCheckBoxState( TRUE );
	else 
		m_pManagementOthersDimissalAuthorityCheck->setCheckBoxState( FALSE );

	if( GET_CLAN_MENAGE_ACCEPT( m_pClanContext->GetMyClanDetailInfo()->_authstaff ))
		m_pManagementOthersSubscriptionAuthorityCheck->setCheckBoxState( TRUE );
	else
		m_pManagementOthersSubscriptionAuthorityCheck->setCheckBoxState( FALSE );

	// �޺��ڽ� �ʱ�ȭ
	INT32 nComboIdx = 0;
	// ���
	switch( m_pClanContext->GetMyClanDetailInfo()->_ranklock )
	{
		case CLAN_LIMIT_RANK_1:		nComboIdx = 0;			break;
		case CLAN_LIMIT_RANK_2:		nComboIdx = 1;			break;
		case CLAN_LIMIT_RANK_3:		nComboIdx = 2;			break;
		case CLAN_LIMIT_RANK_4:		nComboIdx = 3;			break;
		case CLAN_LIMIT_RANK_5:		nComboIdx = 4;			break;
	}
	m_pManagementOthersAdmissionLimitRankPopup->SetSelectedElement( nComboIdx, FALSE );
	m_pManagementOthersAdmissionLimitRankCombo->SetText( g_pszAdmissionLimitRankItem[ nComboIdx ] );

	// ����
	switch( m_pClanContext->GetMyClanDetailInfo()->_lagelock )
	{
		case CLAN_LIMIT_AGE_1:		nComboIdx = 0;			break;
		case CLAN_LIMIT_AGE_2:		nComboIdx = 1;			break;
		case CLAN_LIMIT_AGE_3:		nComboIdx = 2;			break;
		case CLAN_LIMIT_AGE_4:		nComboIdx = 3;			break;
	}
	m_pManagementOthersAdmissionLimitAgePopup[0]->SetSelectedElement( nComboIdx, FALSE );
	m_pManagementOthersAdmissionLimitAgeCombo[0]->SetText( g_pszAdmissionLimitAgeLowItem[ nComboIdx ] );

	// ����
	switch( m_pClanContext->GetMyClanDetailInfo()->_hagelock )
	{
		case CLAN_LIMIT_AGE_1:		nComboIdx = 0;			break;
		case CLAN_LIMIT_AGE_2:		nComboIdx = 1;			break;
		case CLAN_LIMIT_AGE_3:		nComboIdx = 2;			break;
		case CLAN_LIMIT_AGE_4:		nComboIdx = 3;			break;
	}
	m_pManagementOthersAdmissionLimitAgePopup[1]->SetSelectedElement( nComboIdx, FALSE );
	m_pManagementOthersAdmissionLimitAgeCombo[1]->SetText( g_pszAdmissionLimitAgeHighItem[ nComboIdx ] );

	//	Ŭ���� ���� ��ư (ĳ�� �������� ������ Ȱ��ȭ �Ѵ�.)
	if(m_pClanContext->PossibleChangeClanName())
	{
		ACTIVATE_BUTTON( m_pManagementOthersChangeClanNameButton);
	}
	else
	{
		DEACTIVATE_BUTTON( m_pManagementOthersChangeClanNameButton);
	}
	
	//	Ŭ�� ��ũ ���� ��ư (ĳ�� �������� ������ Ȱ��ȭ �Ѵ�.)
	if(m_pClanContext->PossibleChangeClanMark())
	{
		ACTIVATE_BUTTON( m_pManagementOthersChangeClanMarkButton);
	}
	else
	{
		DEACTIVATE_BUTTON( m_pManagementOthersChangeClanMarkButton);
	}
}

void CStepClanMember::OnClanManagementBoardNoticeModify(void)
{
	//	�������� ���� �Ϸ�
 	if( strcmp( m_pManagementWritingBoardNoticeEdit->GetText(), m_pClanContext->GetMyClanDetailInfo()->_announce ) != 0 )
	{
        m_pClanContext->BoardNoticeModify( m_pManagementWritingBoardNoticeEdit->GetText() );
		i3String::Copy( m_pClanContext->GetMyClanDetailInfo()->_announce, m_pManagementWritingBoardNoticeEdit->GetText(), NET_CLAN_INTRO_SIZE );        
	}
}

void CStepClanMember::OnClanManagementBoardGuideModify(void)
{
	if( i3String::Length( m_pManagementWritingBoardGuideEdit->GetText()) == 0 )
	{
		//	"Ŭ�� �Ұ��� �ùٸ��� �ʽ��ϴ�.\n�ٽ� Ȯ���� �ֽʽÿ�."
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_WRONG_CLAN_GUIDE") );
		return;
	}

	//	Ŭ���Ұ� ���� �Ϸ�
	if( strcmp( m_pManagementWritingBoardGuideEdit->GetText(), m_pClanContext->GetMyClanDetailInfo()->_intro ) != 0 )
	{
		m_pClanContext->BoardGuideModify( m_pManagementWritingBoardGuideEdit->GetText() );
		i3String::Copy( m_pClanContext->GetMyClanDetailInfo()->_intro, m_pManagementWritingBoardGuideEdit->GetText(), NET_CLAN_INTRO_SIZE );		
	}
}

void CStepClanMember::OnManagementInstructionButton(void)
{
	ExclusionPopup(m_pManagementInstructionPopup);
}

void CStepClanMember::OnManagementInstructionPopup(INT32 nItemIdx)
{	
#if defined(TEST_CLAN_MEMBER_LIST)
	STRING_POPUP("TEST_CLAN_MEMBER_LIST");
	return;
#endif

	INT64 i64UID[MAX_MEMBER_COUNT] = {0,};
	INT32 count = 0;
	
	for(INT32 i=0; i<m_listManageMember.GetCount(); i++)
	{
		ManagementClanMemberInfo * pMember = (ManagementClanMemberInfo*) m_listManageMember.GetItem(i);

		if( pMember->_bCheck)
		{
			//	Ŭ�� ������ �Ǵ� �ڱ��ڽ��� �������� ���ϵ���
			if( pMember->_info._i64UID > 0 && 
				pMember->_info._i64UID != m_pClanContext->GetMyDBIdx() &&
				pMember->_info._level != (UINT8) CLAN_MEMBER_LEVEL_MASTER)
			{
				I3_BOUNDCHK( count, MAX_MEMBER_COUNT);

				i64UID[count] = pMember->_info._i64UID;
				count++;
			}
		}
	}

	switch(nItemIdx)
	{
	case 0:		//	���� �Ӹ�
		{
			if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
			{
				STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"������ ������ �ʿ��մϴ�."

				_SetManagementListCheckAll(FALSE);
			}
			else
			{
                m_pClanContext->ProcessAppointMember(i64UID, count);
			}
		}			 
		break;
	case 1:		//	������ ����	
		{
			if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
			{
				STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));		//	"������ ������ �ʿ��մϴ�."

				_SetManagementListCheckAll(FALSE);
			}
			else
			{
                BOOL bRet = m_pClanContext->ProcessCommissionMaster(i64UID, count);

				if( !bRet)	//	1�� �̻��� ������ ��� ��� üũ ����
					_SetManagementListCheckAll(FALSE);
			}
		}
		break;
	case 2:		//	���� ����	
		{
			if( FALSE == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_DEPORTATION ) ) 
			{
				//	"�ش� ��ɿ� ���� ������ ������ �����Ǿ� ���� �ʽ��ϴ�."
				STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));
			}
			else
			{
                m_pClanContext->ProcessDismissalMember(i64UID, count);
			}
		}
		break;
	case 3:		//	�Ϲ����� ����
		{
			if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
			{
				STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"������ ������ �ʿ��մϴ�."

				_SetManagementListCheckAll(FALSE);
			}
			else
			{
				m_pClanContext->ProcessDemoteMember(i64UID, count);
			}
		}
		break;
	}
}

void CStepClanMember::OnClanManagementMemberCheck(INT32 nCheckIdx)
{
	I3_BOUNDCHK( nCheckIdx, MANAGEMENT_LIST_ROW_MAX);

	BOOL bCheck = m_slotManagementList[nCheckIdx].pCheck->getCheckBoxState();

	ManagementClanMemberInfo * pMember = (ManagementClanMemberInfo*) m_listManageMember.GetItem(nCheckIdx + m_idxClanManagementMemberStartSlot);

	pMember->_bCheck = bCheck;

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnClanManagementMemberCheckAll(void)
{
	BOOL bCheck = m_pManagementListHeadCheckAll->getCheckBoxState();

	_SetManagementListCheckAll( bCheck);

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnClanManagementMemberScroll(void)
{
	m_idxClanManagementMemberStartSlot = m_pManagementMemberScrollList->GetCurrentTop();
}

void CStepClanMember::OnSubscriptionInstructionButton(void)
{
	ExclusionPopup(m_pSubscriptionInstructionPopup);
}

void CStepClanMember::OnSubscriptionInstructionPopup(INT32 nItemIdx)
{
#if defined(TEST_CLAN_SUBSCRITION_LIST)
	STRING_POPUP("TEST_CLAN_SUBSCRITION_LIST");
	return;
#endif

	INT64 i64UID[MAX_REQUEST_COUNT] = {0,};
	INT32 count = 0;
	
	for(INT32 i=0; i<m_listManageSubscriber.GetCount(); i++)
	{
		if( i >= MAX_REQUEST_COUNT)
			continue;

		ManagementSubscriberInfo * pSubscriber = (ManagementSubscriberInfo*) m_listManageSubscriber.GetItem(i);

		if( pSubscriber->_bCheck && pSubscriber->_info._i64UID > 0)
		{
			I3_BOUNDCHK( count, MAX_REQUEST_COUNT);

			i64UID[count] = pSubscriber->_info._i64UID;
			count++;
		}
	}

	switch(nItemIdx)
	{
	case 0:		//	�󼼺���
		{
			if( m_pClanContext->ProcessViewMember(i64UID, count))
			{
				((CStageReady*)m_pParent)->TogglePopup( POPUP_CLAN_SUBSCRIBER_INFO, 0);
			}
		}
		break;
	case 1:		//	���� ����
		{
			if( FALSE == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_ACCEPT ) ) 
			{
				//	"�ش� ��ɿ� ���� ������ ������ �����Ǿ� ���� �ʽ��ϴ�."
				STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));

				_SetSubscriptionListCheckAll(FALSE);
			}
			else
			{
                m_pClanContext->ProcessIntromissionMember(i64UID, count);
			}
		}
		break;
	case 2:		//	���� �ź�
		{
			if( FALSE == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_ACCEPT ) )
			{
				//	"�ش� ��ɿ� ���� ������ ������ �����Ǿ� ���� �ʽ��ϴ�."
				STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));

				_SetSubscriptionListCheckAll(FALSE);
			}
			else
			{
                m_pClanContext->ProcessRejectionMember(i64UID, count);
			}
		}
		break;
	}
}

void CStepClanMember::OnClanManagementSubscriberCheck(INT32 nCheckIdx)
{
	I3_BOUNDCHK( nCheckIdx, SUBSCRIPTION_LIST_ROW_MAX);

	BOOL bCheck = m_slotSubscriptionList[nCheckIdx].pCheck->getCheckBoxState();

	ManagementSubscriberInfo * pSubscriber = (ManagementSubscriberInfo*) m_listManageSubscriber.GetItem( nCheckIdx + m_idxClanManagementSubscriberStartSlot);

	pSubscriber->_bCheck = bCheck;

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnClanManagementSubscriberCheckAll(void)
{
	BOOL bCheck = m_pSubscriberListHeadCheckAll->getCheckBoxState();

	_SetSubscriptionListCheckAll(bCheck);

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnClanManagementSubscriberScroll(void)
{
	m_idxClanManagementSubscriberStartSlot = m_pManagementSubscriberScrollList->GetCurrentTop();
}

void CStepClanMember::OnClanManagementSubscriberFocus(INT32 nIdx)
{
	I3ASSERT(nIdx >= 0);
    I3ASSERT(nIdx < SUBSCRIPTION_LIST_ROW_MAX);

	m_pManagementSubscriberFocus->setPositionY(nIdx * 27.0f + 1.0f);
	m_pManagementSubscriberFocus->SetEnable(TRUE);
}

void CStepClanMember::OnClanManagementSubscriberSelect(INT32 nIdx)
{
	/*m_nLastedSelectSlot = nIdx + m_idxClanManagementSubscriberStartSlot;
	m_pManagementSubscriberSelection->setPositionY(nIdx * 27.0f + 1.0f);
	m_pManagementSubscriberSelection->SetEnable(TRUE);*/
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnClanManagementSubscriberConfirm(INT32 nIdx)
{
#if defined(TEST_CLAN_SUBSCRITION_LIST)
	STRING_POPUP("TEST_CLAN_SUBSCRITION_LIST");
	return;
#endif

	m_nLastedSelectSlot = nIdx + m_idxClanManagementSubscriberStartSlot;
	I3_BOUNDCHK( m_nLastedSelectSlot, m_listManageSubscriber.GetCount());

	ManagementSubscriberInfo * pSubscriber = (ManagementSubscriberInfo*) m_listManageSubscriber.GetItem(m_nLastedSelectSlot);
	
	//	���� â�� ���� ���� ������ �׸��� �ε����� context�� �ִ´�.
	m_pClanContext->CopyIndexBuffer( &pSubscriber->_info._i64UID, 1);

	((CStageReady*)m_pParent)->TogglePopup( POPUP_CLAN_SUBSCRIBER_INFO, 0);
}

void CStepClanMember::OnSortClanMemberListByLevel(void)
{
	if( m_pInfoPhase->isEnable()) 
	{
		if( m_sInfoClanMemberSortType == CLAN_SORT_LEVEL_ASC)
		{
			m_sInfoClanMemberSortType = CLAN_SORT_LEVEL_DESC;
		}
		else	//	default
		{
			m_sInfoClanMemberSortType = CLAN_SORT_LEVEL_ASC;
		}

		m_listManageMember.SetCompareProc(cbSortInfoMemberListByLevel);
		m_slotClanMemberListHeadClass->SetTextColor( GetColor(GCT_INVERSE));
	}
	else if( m_pManagementPhase->isEnable())
	{
		if( m_sManageClanMemberSortType == CLAN_SORT_LEVEL_ASC)
		{
			m_sManageClanMemberSortType = CLAN_SORT_LEVEL_DESC;
		}
		else	//	default
		{
			m_sManageClanMemberSortType = CLAN_SORT_LEVEL_ASC;
		}

		m_listManageMember.SetCompareProc(cbSortManageMemberListByLevel);
		m_pManagementListHeadClass->SetTextColor( GetColor(GCT_INVERSE));
	}

	//	�����Ͽ� ����Ʈ�� �籸���Ѵ�.
	INT32 count = m_listManageMember.GetCount();

	m_listManageMember.Clear();	
	for(INT32 i=0; i<count; i++)
	{
		if(m_ManageMemberInfo[i]._info._i64UID > 0)
			m_listManageMember.Add((void*) &m_ManageMemberInfo[i]);
	}	

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnSortClanMemberListByName(void)
{
	if( m_pInfoPhase->isEnable()) 
	{
		if( m_sInfoClanMemberSortType == CLAN_SORT_NAME_ASC)
		{
			m_sInfoClanMemberSortType = CLAN_SORT_NAME_DESC;
		}
		else	//	default
		{
			m_sInfoClanMemberSortType = CLAN_SORT_NAME_ASC;
		}

		m_listManageMember.SetCompareProc(cbSortInfoMemberListByName);
		m_slotClanMemberListHeadNick->SetTextColor( GetColor(GCT_INVERSE));
	}
	else if( m_pManagementPhase->isEnable())
	{
		if( m_sManageClanMemberSortType == CLAN_SORT_NAME_ASC)
		{
			m_sManageClanMemberSortType = CLAN_SORT_NAME_DESC;
		}
		else	//	default
		{
			m_sManageClanMemberSortType = CLAN_SORT_NAME_ASC;
		}

		m_listManageMember.SetCompareProc(cbSortManageMemberListByName);
		m_pManagementListHeadNick->SetTextColor( GetColor(GCT_INVERSE));
	}

	//	�����Ͽ� ����Ʈ�� �籸���Ѵ�.
	INT32 count = m_listManageMember.GetCount();

	m_listManageMember.Clear();	
	for(INT32 i=0; i<count; i++)
	{
		if(m_ManageMemberInfo[i]._info._i64UID > 0)
			m_listManageMember.Add((void*) &m_ManageMemberInfo[i]);
	}	

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnSortClanMemberListByDate(void)
{
	if( m_sManageClanMemberSortType == CLAN_SORT_DATE_DESC)
	{
		m_sManageClanMemberSortType = CLAN_SORT_DATE_ASC;
	}
	else	//	default
	{
		m_sManageClanMemberSortType = CLAN_SORT_DATE_DESC;
	}

	m_listManageMember.SetCompareProc(cbSortManageMemberListByDate);
	m_pManagementListHeadEntryDay->SetTextColor( GetColor(GCT_INVERSE));

	//	�����Ͽ� ����Ʈ�� �籸���Ѵ�.
	INT32 count = m_listManageMember.GetCount();

	m_listManageMember.Clear();	
	for(INT32 i=0; i<count; i++)
	{
		if(m_ManageMemberInfo[i]._info._i64UID > 0)
			m_listManageMember.Add((void*) &m_ManageMemberInfo[i]);
	}	

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnSortClanMemberListByState(void)
{
	if( m_sInfoClanMemberSortType == CLAN_SORT_STATE_DESC)
	{
		m_sInfoClanMemberSortType = CLAN_SORT_STATE_ASC;
	}
	else	//	default
	{
		m_sInfoClanMemberSortType = CLAN_SORT_STATE_DESC;
	}

	m_listManageMember.SetCompareProc(cbSortInfoMemberListByState);
	m_slotClanMemberListHeadState->SetTextColor( GetColor(GCT_INVERSE));

	//	�����Ͽ� ����Ʈ�� �籸���Ѵ�.
	INT32 count = m_listManageMember.GetCount();

	m_listManageMember.Clear();	
	for(INT32 i=0; i<count; i++)
	{
		if(m_ManageMemberInfo[i]._info._i64UID > 0)
			m_listManageMember.Add((void*) &m_ManageMemberInfo[i]);
	}	

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnSortSubscriberListByName(void)
{
	if( m_sManageSubscriberSortType == CLAN_SORT_NAME_ASC)
	{
		m_sManageSubscriberSortType = CLAN_SORT_NAME_DESC;
	}
	else	//	default
	{
		m_sManageSubscriberSortType = CLAN_SORT_NAME_ASC;
	}

	m_listManageSubscriber.SetCompareProc(cbSortSubscriberListByName);

	//	�����Ͽ� ����Ʈ�� �籸���Ѵ�.
	INT32 count = m_listManageSubscriber.GetCount();

	m_listManageSubscriber.Clear();	
	for(INT32 i=0; i<count; i++)
	{
		if( i >= MAX_REQUEST_COUNT)		
		{
			I3TRACE("[StepClanMember] Clan subscribers are over count 100 [%d]", i); 

			continue;				
		}	

		if(m_ManageSubscriberInfo[i]._info._i64UID > 0)
			m_listManageSubscriber.Add((void*) &m_ManageSubscriberInfo[i]);
	}	
	
	m_pSubscriberListHeadNick->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::OnSortSubscriberListByDate(void)
{
	if( m_sManageSubscriberSortType == CLAN_SORT_DATE_DESC)
	{
		m_sManageSubscriberSortType = CLAN_SORT_DATE_ASC;
	}
	else	//	default
	{
		m_sManageSubscriberSortType = CLAN_SORT_DATE_DESC;
	}

	m_listManageSubscriber.SetCompareProc(cbSortSubscriberListByDate);

	//	�����Ͽ� ����Ʈ�� �籸���Ѵ�.
	INT32 count = m_listManageSubscriber.GetCount();

	if( count > 100 ) count = 100;

	m_listManageSubscriber.Clear();	
	for(INT32 i=0; i<count; i++)
	{
		if( i >= MAX_REQUEST_COUNT)		
		{
			I3TRACE("[StepClanMember] Clan subscribers are over count 100 [%d]", i); 

			continue;				
		}	

		if(m_ManageSubscriberInfo[i]._info._i64UID > 0)
			m_listManageSubscriber.Add((void*) &m_ManageSubscriberInfo[i]);
	}	

	m_pSubscriberListHeadEntryDay->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanMember::_ResetManagementListSlot(INT32 nIdx)
{
	I3_BOUNDCHK(nIdx, MANAGEMENT_LIST_ROW_MAX);

	m_slotManagementList[nIdx].pSlot->SetEnable(FALSE);

	m_slotManagementList[nIdx].pNick->SetText("");
	m_slotManagementList[nIdx].pClass->SetText("");
	m_slotManagementList[nIdx].pEntryDay->SetText("");
	m_slotManagementList[nIdx].pRank->SetEnable(FALSE);

	SetCheckBox(m_slotManagementList[nIdx].pCheck, FALSE);	
}

void CStepClanMember::_SetManagementListCheckAll(BOOL bCheck)
{
	//	GUI ���Կ� üũ
	for(INT32 i=0; i<MANAGEMENT_LIST_ROW_MAX; i++)
	{
		SetCheckBox(m_slotManagementList[i].pCheck, bCheck);
	}

	SetCheckBox( m_pManagementListHeadCheckAll, bCheck);

	for(INT32 i=0; i<m_listManageMember.GetCount(); i++)
	{
		ManagementClanMemberInfo * pMember = (ManagementClanMemberInfo*) m_listManageMember.GetItem(i);

		pMember->_bCheck = bCheck;		
	}
}

void CStepClanMember::_SetManagementListSlot(INT32 nIdx, ManagementClanMemberInfo * pMember)
{
	I3_BOUNDCHK(nIdx, MANAGEMENT_LIST_ROW_MAX);
	I3ASSERT( pMember != NULL);

	char szTemp[MAX_STRING_COUNT] = "";

	m_slotManagementList[nIdx].pSlot->SetEnable(TRUE);

	SetCheckBox(m_slotManagementList[nIdx].pCheck, pMember->_bCheck);	
	
	SetNameUsingColorTable(m_slotManagementList[nIdx].pNick, pMember->_info._nick, pMember->_info._color);

	GetMemberLevelName2(pMember->_info._level, szTemp, MAX_STRING_COUNT);
	m_slotManagementList[nIdx].pClass->SetText(szTemp);

	if(pMember->_info._date > 0)
	{		
		//	"%04u�� %02u�� %02u��"
		sprintf_s(szTemp, GAME_STRING("STBL_IDX_CLAN_DECLARE_DATE"), 
						(INT32)GetEntryYear(pMember->_info._date), 
						(INT32)GetEntryMonth(pMember->_info._date), 
						(INT32)GetEntryDay(pMember->_info._date));

		m_slotManagementList[nIdx].pEntryDay->SetText(szTemp);
	}
	else
	{
		m_slotManagementList[nIdx].pEntryDay->SetText("");
	}

	SetRankImage(m_slotManagementList[nIdx].pRank, pMember->_info._rank);
}

void CStepClanMember::_ResetSubscriptionListSlot(INT32 nIdx)
{
	I3_BOUNDCHK(nIdx, SUBSCRIPTION_LIST_ROW_MAX);

	m_slotSubscriptionList[nIdx].pSlot->SetEnable(FALSE);

	m_slotSubscriptionList[nIdx].pNick->SetText("");
	m_slotSubscriptionList[nIdx].pEntryDay->SetText("");
	m_slotSubscriptionList[nIdx].pRank->SetEnable(FALSE);

	SetCheckBox(m_slotSubscriptionList[nIdx].pCheck, TRUE);	
}

void CStepClanMember::_SetSubscriptionListCheckAll(BOOL bCheck)
{
	//	GUI ���Կ� üũ
	for(INT32 i=0; i<SUBSCRIPTION_LIST_ROW_MAX; i++)
	{
		SetCheckBox(m_slotSubscriptionList[i].pCheck, bCheck);
	}

	SetCheckBox( m_pSubscriberListHeadCheckAll, bCheck);

	for(INT32 i=0; i<m_listManageSubscriber.GetCount(); i++)
	{
		ManagementSubscriberInfo * pMember = (ManagementSubscriberInfo*) m_listManageSubscriber.GetItem(i);

		pMember->_bCheck = bCheck;
	}
}

void CStepClanMember::_SetSubscriptionListSlot(INT32 nIdx, ManagementSubscriberInfo * pSubscriber)
{
	I3_BOUNDCHK(nIdx, SUBSCRIPTION_LIST_ROW_MAX);
	I3ASSERT( pSubscriber != NULL);

	char szEntry[MAX_STRING_COUNT] = "";

	m_slotSubscriptionList[nIdx].pSlot->SetEnable(TRUE);

	SetCheckBox(m_slotSubscriptionList[nIdx].pCheck, pSubscriber->_bCheck);
	
	m_slotSubscriptionList[nIdx].pNick->SetTextEllipsis(pSubscriber->_info._nick);

	//	"%04u�� %02u�� %02u��"
	sprintf_s(szEntry, GAME_STRING("STBL_IDX_CLAN_DECLARE_DATE"), 
				(INT32)GetEntryYear(pSubscriber->_info._date), 
				(INT32)GetEntryMonth(pSubscriber->_info._date), 
				(INT32)GetEntryDay(pSubscriber->_info._date));

	m_slotSubscriptionList[nIdx].pEntryDay->SetText(szEntry);

	SetRankImage(m_slotSubscriptionList[nIdx].pRank, pSubscriber->_info._rank);
}

void CStepClanMember::OnManagementOthersSendNote(void)
{
	if ( CanClanMemberSendNoteAll() )
	{
		((CStageReady*)m_pParent)->TogglePopup( POPUP_CLAN_SEND_NOTE);
	}
	else
	{
		//���Ŀ� ������ �����Դϴ�.
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_PREPARE_PLAN"));
	}
}

void CStepClanMember::OnManagementOthersChangeClanName(void)
{
	//	���� ĳ�� �������� ��� �� ������ �κ��丮���� ó���ϸ� Ŭ���� ���� �����۰� Ŭ�� ��ũ ���� �����۸� Ŭ�� ���� ���������� �Ҽ� �����Ƿ� ����ó���� �ʿ��ϴ�. komet
	INT32 ItemID = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_EXPENDABLE, CASHITEM_GROUP_CHANGE_CLAN_NAME, CASHITEM_DAY_NO);

	if( g_pFramework->IsUsingCashItem( ItemID))
	{
		m_authItemFlag = ItemID;

		//	�������� ������� ����
		((CStageReady*)m_pParent)->TogglePopup(POPUP_USE_ITEM, &ItemID);
	}
}

void CStepClanMember::OnManagementOthersChangeClanMark(void)
{
#if defined(USE_CLAN_MARK)
	//	���� ĳ�� �������� ��� �� ������ �κ��丮���� ó���ϸ� Ŭ���� ���� �����۰� Ŭ�� ��ũ ���� �����۸� Ŭ�� ���� ���������� �Ҽ� �����Ƿ� ����ó���� �ʿ��ϴ�. komet
	INT32 ItemID = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_EXPENDABLE, CASHITEM_GROUP_CHANGE_CLAN_MARK, CASHITEM_DAY_NO);

	if( g_pFramework->IsUsingCashItem( ItemID))
	{
		m_authItemFlag = ItemID;

		((CStageReady*)m_pParent)->TogglePopup( POPUP_USE_ITEM, &ItemID);
	}
#else
	STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_PREPARE_PLAN"));		//	"���Ŀ� ������ �����Դϴ�."
	return;
#endif
}

void CStepClanMember::AuthStartSuccess(INT32 arg)
{	
	if (EV_SUCCESSED(arg))
	{	//	Ŭ�� ��ũ ���� UI�� ���� POPUP_USE_ITEM�� ������� �����Ƿ� ����ó���մϴ�.
		INT32 changeClanMarkItemFlag = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_EXPENDABLE, CASHITEM_GROUP_CHANGE_CLAN_MARK, CASHITEM_DAY_NO);

		if( changeClanMarkItemFlag == m_authItemFlag)
		{
			((CStageReady*) m_pParent)->TogglePopup(POPUP_NULL);	//	Ŭ�� ��ũ ���� â�� ����.
			
			CAPTION_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_COMPLETE_CHANGE"),	//	"Ŭ�� ��ũ ������ �Ϸ��Ͽ����ϴ�."
						GAME_STRING("STBL_IDX_CLAN_CHANGE_CLAN_MARK"));			//	"Ŭ�� ��ũ ����"

			//	��� ���� Ŭ�� ��ũ�� ����
			((CStageReady*)m_pParent)->GetReadyBg()->CharaChangeClanMarkInReadyBg();
		}

		m_authItemFlag = 0;

		//	Ŭ���� ���� ��ư (ĳ�� �������� ������ Ȱ��ȭ �Ѵ�.)
		if(m_pClanContext->PossibleChangeClanName())
		{
			ACTIVATE_BUTTON( m_pManagementOthersChangeClanNameButton);
		}
		else
		{
			DEACTIVATE_BUTTON( m_pManagementOthersChangeClanNameButton);
		}
		
		//	Ŭ�� ��ũ ���� ��ư (ĳ�� �������� ������ Ȱ��ȭ �Ѵ�.)
		if(m_pClanContext->PossibleChangeClanMark())
		{
			ACTIVATE_BUTTON( m_pManagementOthersChangeClanMarkButton);
		}
		else
		{
			DEACTIVATE_BUTTON( m_pManagementOthersChangeClanMarkButton);
		}
	}
	else
	{
		//	"������ �����Ͽ����ϴ�. ��� �� �ٽ� �õ����ּ���."
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_FAIL_ITEM_AUTH"));
	}
}

void CStepClanMember::OnManagementOthersDissolution(void)
{
	m_pClanContext->ProcessDissolutionClan();
}

void CStepClanMember::OnManagementOthersModify(void)
{
	CLAN_MANAGEMENT modify;

	//	���� ��� ����
	switch(m_pManagementOthersAdmissionLimitRankPopup->getSelectedElement())
	{
	case 0:		modify._limitRank = CLAN_LIMIT_RANK_1;		break;		//	��ü ���
	case 1:		modify._limitRank = CLAN_LIMIT_RANK_2;		break;		//	��� ���
	case 2:		modify._limitRank = CLAN_LIMIT_RANK_3;		break;		//	���� ���
	case 3:		modify._limitRank = CLAN_LIMIT_RANK_4;		break;		//	���� ���
	case 4:		modify._limitRank = CLAN_LIMIT_RANK_5;		break;		//	�强 ���
	default:	modify._limitRank = CLAN_LIMIT_RANK_1;		break;
	}	
	//	���� ���� ���� ����
	switch(m_pManagementOthersAdmissionLimitAgePopup[0]->getSelectedElement())
	{
	case 0:		modify._limitAgeLow = CLAN_LIMIT_AGE_1;		break;		//	��ü ���
	case 1:		modify._limitAgeLow = CLAN_LIMIT_AGE_2;		break;		//	15�� �̻�
	case 2:		modify._limitAgeLow = CLAN_LIMIT_AGE_3;		break;		//	20�� �̻�
	case 3:		modify._limitAgeLow = CLAN_LIMIT_AGE_4;		break;		//	30�� �̻�
	default:	modify._limitAgeLow = CLAN_LIMIT_AGE_1;		break;
	}
	//	���� ���� ���� ����
	switch(m_pManagementOthersAdmissionLimitAgePopup[1]->getSelectedElement())
	{
	case 0:		modify._limitAgeHigh = CLAN_LIMIT_AGE_1;	break;		//	��ü ���
	case 1:		modify._limitAgeHigh = CLAN_LIMIT_AGE_2;	break;		//	15�� ����
	case 2:		modify._limitAgeHigh = CLAN_LIMIT_AGE_3;	break;		//	20�� ����
	case 3:		modify._limitAgeHigh = CLAN_LIMIT_AGE_4;	break;		//	30�� ����
	default:	modify._limitAgeHigh = CLAN_LIMIT_AGE_1;	break;
	}

	//	�Խ��� ���� ����
	UINT8 wr = (UINT8) m_pManagementOthersWritingAuthorityCheck->getCheckBoxState();

	//	Ŭ�� �ʴ� ����
	UINT8 in = (UINT8) m_pManagementOthersInviteAuthorityCheck->getCheckBoxState();

	//	Ŭ�� �ɹ� ���� ����
	UINT8 de = (UINT8) m_pManagementOthersDimissalAuthorityCheck->getCheckBoxState();

	//	���� ���� ����
	UINT8 ac = (UINT8) m_pManagementOthersSubscriptionAuthorityCheck->getCheckBoxState();

	modify._authority = CLAN_MENAGE_AUTHORITY( wr, in, de, ac);

	if( m_pClanContext->GetMyClanDetailInfo()->_authstaff	!= modify._authority	||
		m_pClanContext->GetMyClanDetailInfo()->_ranklock	!= modify._limitRank	||
		m_pClanContext->GetMyClanDetailInfo()->_lagelock	!= modify._limitAgeLow	||
		m_pClanContext->GetMyClanDetailInfo()->_hagelock	!= modify._limitAgeHigh	)
	{
		m_pClanContext->GetMyClanDetailInfo()->_authstaff	= modify._authority;
		m_pClanContext->GetMyClanDetailInfo()->_ranklock	= modify._limitRank;
		m_pClanContext->GetMyClanDetailInfo()->_lagelock	= modify._limitAgeLow;
		m_pClanContext->GetMyClanDetailInfo()->_hagelock	= modify._limitAgeHigh;	
		//	������ ���� ������ ����
		m_pClanContext->ProcessManagementModify( &modify);
	}
}

void CStepClanMember::OnManagementOthersAuthorityCheck(INT32 nIdx)
{
	switch( nIdx)
	{
	//	�Խ��� ���� ����
	case GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_WRITING_AUTHORITY_CHECK:
		{
		}
		break;
	//	Ŭ�� �ʴ� ����
	case GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_INVITE_AUTHORITY_CHECK:
		{		
		}
		break;
	//	Ŭ�� �ɹ� ���� ����
	case GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_DISMISSAL_AUTHORITY_CHECK:
		{
		}
		break;
	//	���� ���� ����
	case GCI_CLAN_C_MEMBER_MANAGEMENT_OTHERS_SUBSCRIPTION_AUTHORITY_CHECK:
		{
		}
		break;
	}
}

void CStepClanMember::OnManagementOthersAdmissionLimitOpen( ADMISSION_LIMIT_TYPE type)
{
	switch( type)
	{
	case ADMISSION_LIMIT_RANK:	//	��� ����
		{
			ExclusionPopup(m_pManagementOthersAdmissionLimitRankPopup);
		}
		break;
	case ADMISSION_LIMIT_AGE1:	//	���� ���� ����
		{
			ExclusionPopup(m_pManagementOthersAdmissionLimitAgePopup[0]);
		}
		break;
	case ADMISSION_LIMIT_AGE2:	//	���� ���� ����
		{
			ExclusionPopup(m_pManagementOthersAdmissionLimitAgePopup[1]);
		}
		break;
	}
}

void CStepClanMember::OnManagementOthersAdmissionLimitInstruction( ADMISSION_LIMIT_TYPE type, INT32 nItemIdx)
{
	switch( type)
	{
	case ADMISSION_LIMIT_RANK:	//	��� ����
		{
			I3_BOUNDCHK( nItemIdx, g_nAdmissionLimitRankCount);
			m_pManagementOthersAdmissionLimitRankCombo->SetText( g_pszAdmissionLimitRankItem[nItemIdx]);
		}
		break;
	case ADMISSION_LIMIT_AGE1:	//	���� ���� ����
		{
			const char * pszItem = g_pszAdmissionLimitAgeLowItem[nItemIdx];
			char temp[256] = "";

			//	���� ��
			INT32 idxHigh = m_pManagementOthersAdmissionLimitAgePopup[1]->getSelectedElement();
			const char * pszHigh = m_pManagementOthersAdmissionLimitAgePopup[1]->GetText( idxHigh);

			//	������ ���ѵǾ� ���� ���
			switch(nItemIdx)
			{
			case 0:		//	��ü ���
				break;
			case 1:		//	15�� �̻�
				break;
			case 2:		//	20�� �̻�
				{
					if( idxHigh == 1)
					{
						//	"�Ұ����� �����Դϴ�. \n ���� ���� ������ %s�� �Ǿ��ֽ��ϴ�."
						sprintf_s(temp, GAME_STRING("STBL_IDX_CLAN_MESSAGE_SUBSCRIPTION_AGE_UPPER"), pszHigh);
						CAPTION_POPUP(temp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_AGE"));	//	"���� ���� ����" 

						//	�߸��� �������� �缳���Ѵ�.
						INT32 idxRevision = idxHigh;
						pszItem = m_pManagementOthersAdmissionLimitAgePopup[0]->GetText(idxRevision);
						m_pManagementOthersAdmissionLimitAgePopup[0]->SetSelectedElement(idxRevision, FALSE);
					}
				}
				break;
			case 3:		//	30�� �̻�
				{
					if( idxHigh == 1 || idxHigh == 2)
					{
						//	"�Ұ����� �����Դϴ�. \n ���� ���� ������ %s�� �Ǿ��ֽ��ϴ�."
						sprintf_s(temp, GAME_STRING("STBL_IDX_CLAN_MESSAGE_SUBSCRIPTION_AGE_UPPER"), pszHigh);
						CAPTION_POPUP(temp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_AGE"));	//	"���� ���� ����"

						//	�߸��� �������� �缳���Ѵ�.
						INT32 idxRevision = idxHigh;
						pszItem = m_pManagementOthersAdmissionLimitAgePopup[0]->GetText(idxRevision);
						m_pManagementOthersAdmissionLimitAgePopup[0]->SetSelectedElement(idxRevision, FALSE);
					}
				}
				break;
			}

			I3_BOUNDCHK( nItemIdx, g_nAdmissionLimitAgeLowCount);
			m_pManagementOthersAdmissionLimitAgeCombo[0]->SetText( (char*) pszItem);

			/**/
		}
		break;
	case ADMISSION_LIMIT_AGE2:	//	���� ���� ����
		{
			const char * pszItem = g_pszAdmissionLimitAgeHighItem[nItemIdx];
			char temp[256] = "";

			//	���� ��
			INT32 idxLow = m_pManagementOthersAdmissionLimitAgePopup[0]->getSelectedElement();
			const char * pszLow = m_pManagementOthersAdmissionLimitAgePopup[0]->GetText( idxLow);

			//	������ ���ѵǾ� ���� ���
			switch(nItemIdx)
			{
			case 0:		//	��ü ���
				break;
			case 1:		//	15�� ����
				{
					if( idxLow == 2 || idxLow == 3)
					{
						//	"�Ұ����� �����Դϴ�. \n ���� ���� ������ %s�� �Ǿ��ֽ��ϴ�."
						sprintf_s(temp, GAME_STRING("STBL_IDX_CLAN_MESSAGE_SUBSCRIPTION_AGE_LOWER"), pszLow);
						CAPTION_POPUP(temp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_AGE"));	//	"���� ���� ����"

						//	�߸��� �������� �缳���Ѵ�.
						INT32 idxRevision = idxLow;
						pszItem = m_pManagementOthersAdmissionLimitAgePopup[1]->GetText(idxRevision);
						m_pManagementOthersAdmissionLimitAgePopup[1]->SetSelectedElement(idxRevision, FALSE);
					}
				}
				break;
			case 2:		//	20�� ����
				{
					if( idxLow == 3)
					{
						//	"�Ұ����� �����Դϴ�. \n ���� ���� ������ %s�� �Ǿ��ֽ��ϴ�."
						sprintf_s(temp, GAME_STRING("STBL_IDX_CLAN_MESSAGE_SUBSCRIPTION_AGE_LOWER"), pszLow);
						CAPTION_POPUP(temp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_AGE"));	//	"���� ���� ����" 

						//	�߸��� �������� �缳���Ѵ�.
						INT32 idxRevision = idxLow;
						pszItem = m_pManagementOthersAdmissionLimitAgePopup[1]->GetText(idxRevision);
						m_pManagementOthersAdmissionLimitAgePopup[1]->SetSelectedElement(idxRevision, FALSE);
					}
				}
				break;
			case 3:		//	30�� ����
				{
				}
				break;
			}

			I3_BOUNDCHK( nItemIdx, g_nAdmissionLimitAgeHighCount);
			m_pManagementOthersAdmissionLimitAgeCombo[1]->SetText( (char*) pszItem);
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CStepClanMember::OnWhisper(INT32 memberIdx)
{

}

void CStepClanMember::OnInviteRoom(INT32 memberIdx)
{
}

void CStepClanMember::OnVisit(INT32 memberIdx)
{
 	const CLAN_MEMBER_BASIC_INFO* member = m_pClanContext->GetMyClanMember(memberIdx);
	if (NULL == member)	return;

	if (FRIEND_STATE_ROOM == GET_FRIEND_STATE(member->_state))
	{			
		g_pCommunity->SetInviteViewClanMember( memberIdx, static_cast<UINT32>(member->_state) );
		((CStageReady*)m_pParent)->OnInviteMoveClan();
	}
	else
	{
		//	"ã�ư��÷��� ģ���� �濡 �־���մϴ�."
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_NO_FRIEND_IN_ROOM"));
	}
}

void CStepClanMember::OnAddFriend(INT32 memberIdx)
{
	const CLAN_MEMBER_BASIC_INFO* member = m_pClanContext->GetMyClanMember(memberIdx);
	if (NULL == member)	return;

	char nick[MAX_STRING_COUNT];
	
	i3String::Copy( nick, member->_nick, MAX_STRING_COUNT );
	
	if (g_pCommunity->GetFriendCount() >= FRIEND_MAX_COUNT)
	{
		//	"���̻� ģ���� �߰��� �� �����ϴ�."
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_OVERFLOW_FRIEND"));
		return;
	}

	for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
		//if(NULL == node) continue;

		if (0 == i3String::Compare(nick, node->_nick))
		{
			char message[MAX_STRING_COUNT];

			//	"���� �̹� ģ����Ͽ� ��ϵǾ� �ֽ��ϴ�."
			sprintf_s(message, "[%s] %s", nick, GAME_STRING("STBL_IDX_CLAN_MESSAGE_ALREADY_FRIEND"));
			STRING_POPUP(message);
			return;
		}
	}

	if (g_pGameContext->IsMyNickName(nick))
	{
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_MYSELF_FRIEND"));		//	"�ڽ��� �߰��� �� �����ϴ�."
		return;
	}

	g_pCommunity->InsertFriend(nick);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void  CStepClanMember::_ProcessChat( REAL32 rDeltaSeconds)
{	
	
}

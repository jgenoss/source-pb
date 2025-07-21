#include "pch.h"
#include "PopupCommunity.h"
#include "GlobalVariables.h"
#include "StageReady.h"
#include "ContextMenu.h"
#include "PhaseBase.h"
#include "StepBase.h"
#include "Shop.h"
#include "CommunityContext.h"
#include "UserLocationInfo.h"
#include "ServerInfo.h"
#include "Record.h"

#include <vector>
#include <algorithm>

#define USE_FRIEND_REFUSE

void PrintFriendState( UINT32 State, char * OutStr, UINT32 OutStrSize )
{
	switch(GET_FRIEND_STATE(State))
	{
	case FRIEND_STATE_REQUEST:	i3String::Copy(OutStr, GAME_STRING("STR_POPUP_SECRET"), OutStrSize );		/*�����*/	break;
	case FRIEND_STATE_HOLD:		i3String::Copy(OutStr, GAME_STRING("STR_POPUP_SECRET"), OutStrSize );		/*�����*/	break;
	case FRIEND_STATE_OFFLINE:	i3String::Copy(OutStr, GAME_STRING("STR_POPUP_OFFLINE_STATE"), OutStrSize );/*��������*/break;
	case FRIEND_STATE_ONLINE:	i3String::Copy(OutStr, GAME_STRING("STR_POPUP_READY_STATE"), OutStrSize );	/*�����*/	break;
	case FRIEND_STATE_ROOM:
		{
			CUserLocationInfo	UserLocInfo(State);

			i3String::Format(OutStr, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_SERVER"),
				UserLocInfo.GetDisplayServerChannelNumber());/*%03d����*/

			if (FRIEND_STATE_ROOM <= GET_FRIEND_STATE(State))
			{
				char room[MAX_STRING_COUNT];
				i3String::Format(room, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_ROOM"),
					UserLocInfo.GetDisplayRoomNumber());/* %d�� ��*/
				i3String::Concat(OutStr, room);
			}
		}
		break;
	}
}

I3_CLASS_INSTANCE(CPopupCommunity, CPopupBase);

BOOL CPopupCommunity::m_bWarningNoteCount = FALSE;

CPopupCommunity::CPopupCommunity()
{
	m_pCommunityGui = NULL;
	m_pCommunityWin = NULL;
	m_pCommunityCaption = NULL;
	m_pContactChild = NULL;
	m_pNoteChild = NULL;
	m_pContactSect = NULL;
	m_pNoteSect = NULL;
	m_pContactTab = NULL;
	m_pNoteTab = NULL;
	m_pNoteKey = NULL;
	m_pNoteGageBg = NULL;
	m_pNoteGage = NULL;
	m_pLeave = NULL;

    m_pContactKey = NULL;
	m_pProfile = NULL;
	m_pFriend = NULL;
	m_pInvite = NULL;
	m_pContactClose = NULL;
	m_pFriendRefuse = NULL;
	m_pNicknameKey = NULL;
	m_pNicknameValue = NULL;
	m_pSeasonRecordKey = NULL;
	m_pSeasonRecord = NULL;
	m_pSeasonKillDeath = NULL;
	m_pSeasonHeadshot = NULL;
	m_pTotalRecordKey = NULL;
	m_pTotalRecord = NULL;
	m_pTotalKillDeath = NULL;
	m_pTotalHeadshot = NULL;
	m_pPositionKey = NULL;
	m_pPositionValue = NULL;

	i3mem::FillZero(m_pRecordTab, sizeof(m_pRecordTab));

	m_pUserDetailInfoPopup = NULL;
#ifdef USE_MODE_RECORD
	m_pCaption = NULL;

	// UserDetailInfoPopupFrame
	// User Info
	m_pUserDetailInfoRankMark = NULL;
	m_pUserDetailInfoNicknameValue = NULL;
	m_pUserDetailInfoRank = NULL;
	m_pUserDetailInfoClanName = NULL;	
	m_pUserDetailInfoTitle = NULL;	
	m_pUserDetailInfoPoint = NULL;
	m_pUserDetailInfoToll = NULL;

	m_pUserDetailInfoPositionKey = NULL;
	m_pUserDetailInfoPositionValue = NULL;

	// UserDetailInfoPopupFrame2
	// Total Record
	m_pUserDetailInfoTotalRecordKey = NULL;
	m_pUserDetailInfoTotalRecord = NULL;
	m_pUserDetailInfoTotalKillDeath = NULL;
	m_pUserDetailInfoTotalHeadshot = NULL;
	m_pUserDetailInfoTotalDis = NULL;
	m_pUserDetailInfoTotalUserRanking = NULL;

	// Season Record
	m_pUserDetailInfoSeasonRecordKey = NULL;
	m_pUserDetailInfoSeasonRecord = NULL;
	m_pUserDetailInfoSeasonKillDeath = NULL;
	m_pUserDetailInfoSeasonHeadshot = NULL;
	m_pUserDetailInfoSeasonDis = NULL;
	m_pUserDetailInfoSeasonUserRanking = NULL;

	// UserDetailInfoPopupFrame3		
	m_pModeRecordTab[0] = NULL;	// ���, ���� tab
	m_pModeRecordTab[1] = NULL;	// ���, ���� tab
	m_bRecordTabFlag = FALSE;

	m_pModeTypeStatic = NULL;
	m_pModeTypeCombo = NULL;
	m_pModeTypeButton = NULL;
	m_pModeTypePopup = NULL;

	m_pModeRecordValue = NULL;
	m_pModeKillDeath = NULL;
	m_pModeHeadshot = NULL;
	m_pModeHitPercent = NULL;

	m_nModeTypeComboIdx = 0;

	// [Button]
	// Button
	m_pUserDetailInfoProfile = NULL;
	m_pUserDetailInfoFriend = NULL;
	m_pUserDetailInfoInvite = NULL;
	m_pUserDetailInfoDetail = NULL;
	m_pUserDetailInfoContactClose = NULL;
	m_pUserDetailInfoFriendRefuse = NULL;

	m_bUserDetailInfoFlag = FALSE;	
	m_SelectUid = 0;

	//UserDetailInfoPopupMessageBox
	m_pUserDetailInfoMsg = NULL;
	m_pUserDetailInfoMsgCaption = NULL;
	m_pUserDetailInfoMsgMessage = NULL;	
	m_pUserDetailInfoMsgConfirm = NULL;
	m_pUserDetailInfoMsgCancel = NULL;

#endif
	//////////////////////////////////////////////////////////////////////////
	m_pFriendTab = NULL;
	m_pBlockTab = NULL;
	m_pClanTab = NULL;
	m_pFind = NULL;
	m_pAdd = NULL;
//	m_pDelete = NULL;
	m_pContactListName = NULL;
	m_pContactListState = NULL;
	m_pContactSelect = NULL;
	m_pContactFocus = NULL;
	m_pContactList = NULL;

	i3mem::FillZero(m_contactSlot, sizeof(m_contactSlot));

	m_pNoteListTab = NULL;
	m_pNoteDelete = NULL;
	m_pNoteWrite = NULL;
	m_pNoteSelect = NULL;
	m_pNoteFocus = NULL;
	m_pNoteListContent = NULL;
	m_pNoteListWho = NULL;
	m_pNoteListDate = NULL;
	
	i3mem::FillZero(m_noteSlot, sizeof(m_noteSlot));

	m_pNoteList = NULL;
	m_pNoteWhoKey = NULL;
	m_pNoteWhoValue = NULL;
	m_pNoteValue = NULL;
	m_pNoteSend = NULL;
	m_pNoteClose = NULL;
	m_pNoteAccept = NULL;
	m_pNoteReject = NULL;

	m_bReceiverUIDFlag = FALSE;
	m_iListIdx = 0;

	//	Gift note
	
	//
	m_pNicknameWin = NULL;
	m_pNicknameCaption = NULL;
	m_pNicknameMessage = NULL;
	m_pNicknameInput = NULL;
	m_pNicknameConfirm = NULL;
	m_pNicknameCancel = NULL;


	m_SelectedContact = -1;
	m_SelectedNote = -1;
	m_SelectedNoteIndex = -1;
	m_lastDeleteFriend = -1;

	m_nickPopupType = NPT_ADD_FRIEND;

	m_fNoteEmptyRate = 0.0f;
	for(UINT32 i = 0; i < MAX_NOTE_COUNT; i++)
		m_bNoteCheck[i] = FALSE;
	m_bAlarmCommunity = FALSE;
	m_bAlarmFlag = FALSE;
	m_rAlarmTerm = 0.0f;

	m_SelectedFriendNode.Reset();
	m_SelectedClanMemberNode.Reset();

	m_PackageGoodsID		= 0;
	m_CurrentUserInfoUID	= 0;
	m_CurrentUserInfoType	= COMMUNITY_TAB_INVALID;
	
	m_SelectUid = 0;
	m_acceptFriendIdx = 0;

	m_bRecordTabFlag = FALSE;
	m_bUserDetailInfoFlag = FALSE;
	m_currentTab = COMMUNITY_TAB_INVALID;

	m_nModeTypeComboIdx = 0;
	m_nNoteType = NOTE_TYPE_SEND;

	memset( m_nTypeMessage, 0, sizeof(m_nTypeMessage ) );
	memset( m_pModeRecordTab, 0, sizeof(m_pModeRecordTab ) );
	memset( m_pNoteGiftValue, 0, sizeof(m_pNoteGiftValue ) );
}

CPopupCommunity::~CPopupCommunity()
{
	/* Do nothing */
}

void CPopupCommunity::OnPreGuiNotify(void)
{
	CPopupBase::OnPreGuiNotify();

	// Ŀ�´�Ƽ
	{
		m_pContactFocus->SetEnable(FALSE);

		INT32 idxSlot = m_SelectedContact - m_pContactList->GetCurrentTop();

		if (0 <= idxSlot && idxSlot < MAX_SLOT_FOR_CONTACT_LIST)
		{
			m_pContactSelect->setPositionY(30.f + (idxSlot * 30.f));
			m_pContactSelect->SetEnable(TRUE);
		}
		else
		{
			m_pContactSelect->SetEnable(FALSE);
		}
	}

	// ����
	{
		m_pNoteFocus->SetEnable(FALSE);
	
		INT32 idxSlot = m_SelectedNote - m_pNoteList->GetCurrentTop();

		if (0 <= idxSlot && idxSlot < MAX_NOTE_SLOT)
		{
			m_pNoteSelect->setPositionY(29.0f + (idxSlot * 24.0f));
			m_pNoteSelect->SetEnable(TRUE);
		}
		else
		{
			m_pNoteSelect->SetEnable(FALSE);
		}
	}
}

void CPopupCommunity::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_CMP_B_CONTACT_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnContactTab();
		break;
	case GCI_CMP_B_NOTE_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNoteTab();
		break;
	case GCI_CMP_B_LEAVE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnLeave();
		break;
	case GCI_CMP_B_PROFILE:
	case GCI_CMP_B_DETAIL_PROFILE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);
			if(NULL == node) break;

			if(node)
			{
				switch(GET_FRIEND_STATE(node->_state))
				{
				//	��� ��� ģ���� �����Ѵ�.
				case FRIEND_STATE_HOLD:			OnProfile();		break;
				//	�̹� ��ϵ� ģ����� �����Ѵ�.
				case FRIEND_STATE_REQUEST:
				case FRIEND_STATE_OFFLINE:
				case FRIEND_STATE_ONLINE:
				case FRIEND_STATE_ROOM:			OnDelete();			break;
				default:	break;
				}
			}
		}
		break;
	case GCI_CMP_B_FRIEND:
	case GCI_CMP_B_DETAIL_FRIEND:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnVisit();
		break;
	case GCI_CMP_B_INVITE:
	case GCI_CMP_B_DETAIL_INVITE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnInviteRoom();
		break;
	#ifdef USE_MODE_RECORD	
	case GCI_CMP_B_DETAIL:		
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	
		{
			if(500 == SUBTRACTE_POINT)	OnUserDetailInfoPupupMsgConfirm();
			else						OnUserDetailInfo();
			
		}
		break;
	case GCI_CMP_B_DETAIL_CONFIRM:		
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnUserDetailInfoPupupMsgConfirm();
		break;
	case GCI_CMP_B_DETAIL_CANCEL:		
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnUserDetailInfoPupupMsgCancel();
		break;
	case GCI_CMP_B_MODE_TOTAL_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnModeTab(FALSE);
		break;
	case GCI_CMP_B_MODE_SEASON_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnModeTab(TRUE);
		break;
	case GCI_CMP_B_MODE_TYPE:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnTypeComboButton();
		break;		
	case GCI_CMP_P_MODE_TYPE_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnTypeComboPopup(pNotify->m_nPriParam);
		break;
	#endif
	case GCI_CMP_B_CONTACT_CLOSE:
	case GCI_CMP_B_DETAIL_CONTACT_CLOSE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnContactClose();
		break;
	case GCI_CMP_B_FRIEND_REFUSE:
	case GCI_CMP_B_DETAIL_FRIEND_REFUSE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnRefuseFriend();
		break;
    case GCI_CMP_B_FRIEND_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnFriendTab();
		break;
	case GCI_CMP_B_BLOCK_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnBlockTab();
		break;
	case GCI_CMP_B_CLAN_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanTab();
		break;
	case GCI_CMP_B_RECORD_TAB_1:
	case GCI_CMP_B_RECORD_TAB_2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnRecordTab(pNotify->m_nID - GCI_CMP_B_RECORD_TAB_1);
		break;
	case GCI_CMP_B_FIND:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnFind();
		break;
	case GCI_CMP_B_ADD:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnAdd();
		break;
	case GCI_CMP_B_DELETE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnDelete();
		break;
	case GCI_CMP_L_CONTACT_LIST:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnContactDrag();
		if (I3GUI_LIST_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnContactFocus(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnContactSelect(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_RCLICK == pNotify->m_nEvent) OnContactContext(pNotify->m_nPriParam);
		break;
	case GCI_CMP_B_NOTE_DELETE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) STRING_POPUP_Q( GAME_STRING("STR_POPUP_Q_MESSAGE_DELETE_NOTE"), MAKE_CALLBACK(CbDeleteNote), this);/*������ ������ ���� �Ͻðڽ��ϱ�?*/
		break;
	case GCI_CMP_B_NOTE_WRITE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnWriteNote();
		break;
	case GCI_CMP_L_NOTE:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) g_pGameContext->AddGameFlag( GAMEFLAG_NOTEREFRESH);
		if (I3GUI_LIST_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnNoteFocus(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_CLICKED == pNotify->m_nEvent) g_pGameContext->AddGameFlag( GAMEFLAG_NOTEREFRESH);
		break;
	case GCI_CMP_B_SEND:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSendNote();
		break;
	case GCI_CMP_B_ACCEPT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNoteAccept(TRUE);
		break;
	case GCI_CMP_B_REJECT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNoteAccept(FALSE);
		break;
	case GCI_CMP_B_CLOSE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNoteClose();
		break;
	case GCI_CMP_B_NICK_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNickConfirm();
		break;
	case GCI_CMP_B_NICK_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNickCancel();
		break;
	case GCI_CMP_C_NOTE_CHECK_1:	case GCI_CMP_C_NOTE_CHECK_2:	case GCI_CMP_C_NOTE_CHECK_3:
	case GCI_CMP_C_NOTE_CHECK_4:	case GCI_CMP_C_NOTE_CHECK_5:	case GCI_CMP_C_NOTE_CHECK_6:
	case GCI_CMP_C_NOTE_CHECK_7:	case GCI_CMP_C_NOTE_CHECK_8:	case GCI_CMP_C_NOTE_CHECK_9:
	case GCI_CMP_C_NOTE_CHECK_10:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) SetNoteCheck(pNotify->m_nID - GCI_CMP_C_NOTE_CHECK_1);
		if (I3GUI_CHECK_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnNoteFocus(pNotify->m_nID - GCI_CMP_C_NOTE_CHECK_1);
		break;

	case GCI_CMP_E_NOTE_CONTENT_1:	case GCI_CMP_E_NOTE_CONTENT_2:	case GCI_CMP_E_NOTE_CONTENT_3:
	case GCI_CMP_E_NOTE_CONTENT_4:	case GCI_CMP_E_NOTE_CONTENT_5:	case GCI_CMP_E_NOTE_CONTENT_6:
	case GCI_CMP_E_NOTE_CONTENT_7:	case GCI_CMP_E_NOTE_CONTENT_8:	case GCI_CMP_E_NOTE_CONTENT_9:
	case GCI_CMP_E_NOTE_CONTENT_10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) SetSelectedNote(pNotify->m_nID - GCI_CMP_E_NOTE_CONTENT_1);
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnNoteFocus(pNotify->m_nID - GCI_CMP_E_NOTE_CONTENT_1);
		break;
	}
}

BOOL CPopupCommunity::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char strName[MAX_STRING_COUNT];

	LinkControlEx(plist, "Community", m_pCommunityGui, -1);
	LinkControlEx(plist, "cmCommunityWin", m_pCommunityWin, -1);
	LinkControlEx(plist, "cmCommunityCaption", m_pCommunityCaption, -1);
	LinkControlEx(plist, "cmContactChild", m_pContactChild, -1);
	LinkControlEx(plist, "cmNoteChild", m_pNoteChild, -1);
	LinkControlEx(plist, "cmContactSect", m_pContactSect, -1);
	LinkControlEx(plist, "cmNoteSect", m_pNoteSect, -1);
	LinkControlEx(plist, "cmContactTab", m_pContactTab, GCI_CMP_B_CONTACT_TAB);
	LinkControlEx(plist, "cmNoteTab", m_pNoteTab, GCI_CMP_B_NOTE_TAB);
	LinkControlEx(plist, "cmNoteKey", m_pNoteKey, -1);
	LinkControlEx(plist, "cmNoteGageBg", m_pNoteGageBg, -1);
	LinkControlEx(plist, "cmNoteGage", m_pNoteGage, -1);
	LinkControlEx(plist, "cmLeave", m_pLeave, GCI_CMP_B_LEAVE);

    LinkControlEx(plist, "cmContactKey", m_pContactKey, -1);
	LinkControlEx(plist, "cmProfile", m_pProfile, GCI_CMP_B_PROFILE);
	LinkControlEx(plist, "cmFriend", m_pFriend, GCI_CMP_B_FRIEND);
	LinkControlEx(plist, "cmInvite", m_pInvite, GCI_CMP_B_INVITE);
	LinkControlEx(plist, "cmContactClose", m_pContactClose, GCI_CMP_B_CONTACT_CLOSE);
	LinkControlEx(plist, "cmFriendRefuse", m_pFriendRefuse, GCI_CMP_B_FRIEND_REFUSE);
	LinkControlEx(plist, "cmNicknameKey", m_pNicknameKey, -1);
	LinkControlEx(plist, "cmNicknameValue", m_pNicknameValue, -1);
	LinkControlEx(plist, "cmSeasonRecordKey", m_pSeasonRecordKey, -1);
	LinkControlEx(plist, "cmSeasonRecord", m_pSeasonRecord, -1);
	LinkControlEx(plist, "cmSeasonKillDeath", m_pSeasonKillDeath, -1);
	LinkControlEx(plist, "cmSeasonHeadshot", m_pSeasonHeadshot, -1);
	LinkControlEx(plist, "cmTotalRecordKey", m_pTotalRecordKey, -1);
	LinkControlEx(plist, "cmTotalRecord", m_pTotalRecord, -1);
	LinkControlEx(plist, "cmTotalKillDeath", m_pTotalKillDeath, -1);
	LinkControlEx(plist, "cmTotalHeadshot", m_pTotalHeadshot, -1);
	LinkControlEx(plist, "cmPositionKey", m_pPositionKey, -1);
	LinkControlEx(plist, "cmPositionValue", m_pPositionValue, -1);

	for(INT32 i=0; i<2; i++)
	{
		sprintf_s(strName, "cmRecordTab%d", i+1);
		LinkControlEx(plist, strName, m_pRecordTab[i], GCI_CMP_B_RECORD_TAB_1+i);
	}

	LinkControlEx(plist, "cmFriendTab", m_pFriendTab, GCI_CMP_B_FRIEND_TAB);
	LinkControlEx(plist, "cmBlockTab", m_pBlockTab, GCI_CMP_B_BLOCK_TAB);
	LinkControlEx(plist, "cmClanTab", m_pClanTab, GCI_CMP_B_CLAN_TAB);
	LinkControlEx(plist, "cmFind", m_pFind, GCI_CMP_B_FIND);
	LinkControlEx(plist, "cmAdd", m_pAdd, GCI_CMP_B_ADD);
//	LinkControlEx(plist, "cmDelete", m_pDelete, GCI_CMP_B_DELETE);
	LinkControlEx(plist, "cmContactListName", m_pContactListName, -1);
	LinkControlEx(plist, "cmContactListState", m_pContactListState, -1);
	
	for(UINT32 i = 0; i < MAX_SLOT_FOR_CONTACT_LIST; i++)
	{
		sprintf_s(strName, "cmContactSlotMark%d", i + 1);
		LinkControlEx(plist, strName, m_contactSlot[i]._pMark, -1);

		sprintf_s(strName, "cmContactSlotNick%d", i + 1);
		LinkControlEx(plist, strName, m_contactSlot[i]._pNick, -1);

		sprintf_s(strName, "cmContactSlotState%d", i + 1);
		LinkControlEx(plist, strName, m_contactSlot[i]._pState, -1);
	}

    LinkControlEx(plist, "cmContactList", m_pContactList, GCI_CMP_L_CONTACT_LIST);
	LinkControlEx(plist, "cmContactSelect", m_pContactSelect, -1);
	LinkControlEx(plist, "cmContactFocus", m_pContactFocus, -1);

	LinkControlEx(plist, "cmNoteListTab", m_pNoteListTab, -1);
	LinkControlEx(plist, "cmNoteDelete", m_pNoteDelete, GCI_CMP_B_NOTE_DELETE);
	LinkControlEx(plist, "cmNoteWrite", m_pNoteWrite, GCI_CMP_B_NOTE_WRITE);
	LinkControlEx(plist, "cmNoteSelect", m_pNoteSelect, -1);
	LinkControlEx(plist, "cmNoteFocus", m_pNoteFocus, -1);
	LinkControlEx(plist, "cmNoteListContent", m_pNoteListContent, -1);
	LinkControlEx(plist, "cmNoteListWho", m_pNoteListWho, -1);
	LinkControlEx(plist, "cmNoteListDate", m_pNoteListDate, -1);
	
	for(UINT32 i = 0; i < MAX_NOTE_SLOT; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "cmNoteCheck%d", i + 1);
		LinkControlEx(plist, strName, m_noteSlot[i]._pCheck, GCI_CMP_C_NOTE_CHECK_1+i);

		sprintf_s(strName, "cmNoteContent%d", i + 1);
		LinkControlEx(plist, strName, m_noteSlot[i]._pContent, -1);

		sprintf_s(strName, "cmNoteWho%d", i + 1);
		LinkControlEx(plist, strName, m_noteSlot[i]._pWho, -1);

		sprintf_s(strName, "cmNoteDate%d", i + 1);
		LinkControlEx(plist, strName, m_noteSlot[i]._pDate, -1);

		sprintf_s(strName, "cmNoteSel%d", i + 1);
		LinkControlEx(plist, strName, m_noteSlot[i]._pSelect, GCI_CMP_E_NOTE_CONTENT_1+i);
	}
	
	LinkControlEx(plist,  "cmNoteGiftItemFrame", m_pNoteGiftItemFrame, -1);
	LinkControlEx(plist,  "cmNoteGiftWeaponImage", m_pNoteGiftWeaponImage, -1);
	LinkControlEx(plist,  "cmNoteGiftCharacterImage", m_pNoteGiftCharacterImage, -1);
	LinkControlEx(plist,  "cmNoteGiftCashItemImage", m_pNoteGiftCashItemImage, -1);
	LinkControlEx(plist,  "cmNoteGiftSeparate", m_pNoteGiftSeparate, -1);

	for(UINT32 i = 0; i < 2; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "cmNoteGiftValue%d", i + 1);
		LinkControlEx(plist, strName, m_pNoteGiftValue[i], -1);
	}

	LinkControlEx(plist, "cmNoteList", m_pNoteList, GCI_CMP_L_NOTE);
	LinkControlEx(plist, "cmNoteWhoKey", m_pNoteWhoKey, -1);
	LinkControlEx(plist, "cmNoteWhoValue", m_pNoteWhoValue, -1);
	LinkControlEx(plist, "cmNoteValue", m_pNoteValue, -1);
	LinkControlEx(plist, "cmNoteSend", m_pNoteSend, GCI_CMP_B_SEND);
	LinkControlEx(plist, "cmNoteClose", m_pNoteClose, GCI_CMP_B_CLOSE);
	LinkControlEx(plist, "cmNoteAccept", m_pNoteAccept, GCI_CMP_B_ACCEPT);
	LinkControlEx(plist, "cmNoteReject", m_pNoteReject, GCI_CMP_B_REJECT);

	LinkControlEx(plist, "cmNicknameWin", m_pNicknameWin, -1);
	LinkControlEx(plist, "cmNicknameCaption", m_pNicknameCaption, -1);
	LinkControlEx(plist, "cmNicknameMessage", m_pNicknameMessage, -1);
	LinkControlEx(plist, "cmNicknameInput", m_pNicknameInput, -1);
	LinkControlEx(plist, "cmNicknameConfirm", m_pNicknameConfirm, GCI_CMP_B_NICK_CONFIRM);
	LinkControlEx(plist, "cmNicknameCancel", m_pNicknameCancel, GCI_CMP_B_NICK_CANCEL);

	// UserDetailInfo
	LinkControlEx(plist,  "cmUserDetailInfoPopup", m_pUserDetailInfoPopup, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupCaption", m_pCaption, -1);

	// UserDetailInfoPopupFrame
	// User Info	
	LinkControlEx(plist,  "cmUserDetailInfoPopupNicknameValue", m_pUserDetailInfoNicknameValue, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupClanName", m_pUserDetailInfoClanName, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupTitle", m_pUserDetailInfoTitle, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupRankValue", m_pUserDetailInfoRank, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupRank", m_pUserDetailInfoRankMark, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupPoint", m_pUserDetailInfoPoint, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupToll", m_pUserDetailInfoToll, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupPositionKey", m_pUserDetailInfoPositionKey, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupPositionValue", m_pUserDetailInfoPositionValue, -1);

	// UserDetailInfoPopupFrame2
	// Season Record
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonRecordKey", m_pUserDetailInfoSeasonRecordKey, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonRecord", m_pUserDetailInfoSeasonRecord, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonKillDeath", m_pUserDetailInfoSeasonKillDeath, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonHeadshot", m_pUserDetailInfoSeasonHeadshot, -1);	
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonDis", m_pUserDetailInfoSeasonDis, -1);	
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonUserRanking", m_pUserDetailInfoSeasonUserRanking, -1);	

	// Total Record
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalRecordKey", m_pUserDetailInfoTotalRecordKey, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalRecord", m_pUserDetailInfoTotalRecord, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalKillDeath", m_pUserDetailInfoTotalKillDeath, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalHeadshot", m_pUserDetailInfoTotalHeadshot, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalDis", m_pUserDetailInfoTotalDis, -1);	
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalUserRanking", m_pUserDetailInfoTotalUserRanking, -1);	

	// UserDetailInfoPopupFrame3
	// Mode Record
	LinkControlEx(plist,  "cmUserDetailInfoPopupModeRecord", m_pModeRecordValue, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupModeKillDeath", m_pModeKillDeath, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupModeHeadshot", m_pModeHeadshot, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupModeAccuracyRate", m_pModeHitPercent, -1);

	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalTab", m_pModeRecordTab[0], GCI_CMP_B_MODE_TOTAL_TAB);
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonTab", m_pModeRecordTab[1], GCI_CMP_B_MODE_SEASON_TAB);

	LinkControlEx(plist, "cmUserDetailInfoPopupModeTypeComboCaption", m_pModeTypeStatic, -1);
	LinkControlEx(plist, "cmUserDetailInfoPopupModeTypeCombo", m_pModeTypeCombo, -1);
	LinkControlEx(plist, "cmUserDetailInfoPopupModeTypeButton", m_pModeTypeButton, GCI_CMP_B_MODE_TYPE);
	LinkControlEx(plist, "cmUserDetailInfoPopupModeTypePopup", m_pModeTypePopup, GCI_CMP_P_MODE_TYPE_POPUP);

	// Button
	LinkControlEx(plist,  "cmUserDetailInfoPopupProfile", m_pUserDetailInfoProfile, GCI_CMP_B_DETAIL_PROFILE);
	LinkControlEx(plist,  "cmUserDetailInfoPopupFriend", m_pUserDetailInfoFriend, GCI_CMP_B_DETAIL_FRIEND);
	LinkControlEx(plist,  "cmUserDetailInfoPopupInvite", m_pUserDetailInfoInvite, GCI_CMP_B_DETAIL_INVITE);
	LinkControlEx(plist,  "cmUserDetailInfoPopupDetail", m_pUserDetailInfoDetail, GCI_CMP_B_DETAIL);
	LinkControlEx(plist,  "cmUserDetailInfoPopupClose", m_pUserDetailInfoContactClose, GCI_CMP_B_DETAIL_CONTACT_CLOSE);
	LinkControlEx(plist, "cmUserDetailInfoPopupFriendRefuse", m_pUserDetailInfoFriendRefuse, GCI_CMP_B_DETAIL_FRIEND_REFUSE);

	// UserDetailInfoPopupMessageBox
	LinkControlEx(plist, "cmUserDetailInfoPopupMsg", m_pUserDetailInfoMsg, -1);
	LinkControlEx(plist, "cmUserDetailInfoPopupMsgCaption", m_pUserDetailInfoMsgCaption, -1);
	LinkControlEx(plist, "cmUserDetailInfoPopupMsgMessage", m_pUserDetailInfoMsgMessage, -1);	
	LinkControlEx(plist, "cmUserDetailInfoPopupMsgConfirm", m_pUserDetailInfoMsgConfirm, GCI_CMP_B_DETAIL_CONFIRM);
	LinkControlEx(plist, "cmUserDetailInfoPopupMsgCancel", m_pUserDetailInfoMsgCancel, GCI_CMP_B_DETAIL_CANCEL);
	

	// ������ ����Ʈ
	/*m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE ]			= STR_TBL_GUI_MESSAGER_CLAN_INVITE;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_ACCEPT ]	= STR_TBL_GUI_MESSAGER_CLAN_INVITE_ACCEPT;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_DENIAL ]	= STR_TBL_GUI_MESSAGER_CLAN_INVITE_DENIAL;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_ACCEPT ]		= STR_TBL_GUI_MESSAGER_CLAN_JOIN_ACCEPT;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_DENIAL ]		= STR_TBL_GUI_MESSAGER_CLAN_JOIN_DENIAL;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_SECESSION ]		= STR_TBL_GUI_MESSAGER_CLAN_SECESSION;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_DEPORTATION ]		= STR_TBL_GUI_MESSAGER_CLAN_DEPORTATION;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_MASTER ]			= STR_TBL_GUI_MESSAGER_CLAN_MASTER;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_STAFF ]			= STR_TBL_GUI_MESSAGER_CLAN_STAFF;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_REGULAR ]			= STR_TBL_GUI_MESSAGER_CLAN_REGULAR;*/
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_ACCEPT ]	= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE_ACCEPT");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_DENIAL ]	= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE_DENIAL");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_ACCEPT ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_JOIN_ACCEPT");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_DENIAL ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_JOIN_DENIAL");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_SECESSION ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_SECESSION");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_DEPORTATION ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_DEPORTATION");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_MASTER ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_MASTER");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_STAFF ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_STAFF");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_REGULAR ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_REGULAR");
	return FALSE;
}

void CPopupCommunity::OnInitControl(void)
{
	_InitCommon();
	_InitContactSect();
	_InitContactChild();
	_InitUserDetailInfo();
	_InitNoteSect();
	_InitNoteChild();

	CPopupBase::OnInitControl();
}

void CPopupCommunity::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate(rDeltaSeconds);

	_UpdateContactList();
	_UpdateContactChild();
	_UpdateNote();
	

	// ��Ʈ�˶�~!
	_UpdateAlarmNote(rDeltaSeconds);
	
	if( GetInputDisable() == FALSE)
	{
		if (GUI_IME_CONFIRM(m_pNicknameInput))
		{
			if (m_pNicknameInput->getIsFocus())
			{
				OnNickConfirm();

				m_pNicknameInput->InputDone();
				m_pNicknameInput->SetFocus(FALSE);
				i3GuiRoot::setFocusControl(NULL);
			}
			else
			{
				m_pNicknameInput->InputDone();
			}
		}
	}
}

void CPopupCommunity::OnExitKey(void)
{
	OnClosePopup();
}

#ifdef USE_MODE_RECORD
void CPopupCommunity::OnModeTab(BOOL bModeFlag)
{
	if(m_bRecordTabFlag == bModeFlag) return;

	m_bRecordTabFlag = bModeFlag;	

	if(FALSE == m_bRecordTabFlag)
	{
		m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pModeRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	}
	else
	{		
		m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pModeRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	}	
}

void CPopupCommunity::OnTypeComboButton(void)
{
	ExclusionPopup(m_pModeTypePopup);
}

void CPopupCommunity::OnTypeComboPopup(INT32 idx)
{
	m_nModeTypeComboIdx = idx; 
	char * pszSelect = (char*) m_pModeTypePopup->GetText(idx);
	m_pModeTypeCombo->SetText( pszSelect);
}
#endif

void CPopupCommunity::SetInputDisable(BOOL bDisable)
{
	m_pCommunityGui->setInputDisable(bDisable);
}

BOOL CPopupCommunity::GetInputDisable(void)
{
	return m_pCommunityGui->getInputDisable();
}

void CPopupCommunity::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);

	#ifdef USE_MODE_RECORD
	m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_DISABLED);	
	m_bRecordTabFlag = FALSE;	
	m_bUserDetailInfoFlag = FALSE;
	g_pCommunity->SetUserDetailInfoFlag(FALSE);
	#endif
	
	m_pFriendTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pBlockTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pClanTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_currentTab = COMMUNITY_FRIEND_TAB;

	SetCurrentUserInfoType(COMMUNITY_FRIEND_TAB);

	_UpdateContactChild();
	_UpdateContactList();

	m_SelectedContact = -1;
	m_SelectedNote = -1;
	m_pContactList->SetCurValue(0.0f);
	m_pNoteList->SetCurValue(0.0f);

//	m_pDelete->setControlDisable(TRUE);
	g_pCommunity->SetCommunityUserInfo(FALSE);

	m_pCommunityGui->SetEnable(TRUE);

	m_SelectedFriendNode.Reset();
	m_SelectedClanMemberNode.Reset();

	//	���� Ŀ�´�Ƽ�� �������� ���� ������ �ֳ� üũ�ϵ��� ��û�մϴ�.
	//g_pFramework->GetShopContext()->RequestGiftList();

	SetPushedStateButton(MBT_COMMUNITY, TRUE);
}

void CPopupCommunity::OnClosePopup(void * pArg1, void * pArg2)
{
	#ifdef USE_MODE_RECORD
	m_bRecordTabFlag = FALSE;
	m_bUserDetailInfoFlag = FALSE;	
	m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pModeRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	// �� ���� ���� Ȯ��â�� �ݴ´�.		
	m_pUserDetailInfoMsg->SetEnable(FALSE);
	ACTIVATE_BUTTON(m_pUserDetailInfoDetail);	
	g_pCommunity->SetUserDetailInfoFlag(FALSE);
	#endif

	CPopupBase::OnClosePopup();

	m_pCommunityGui->SetEnable(FALSE);

	m_pCommunityWin->setInputDisable(FALSE);
	m_pNicknameWin->SetEnable(FALSE);

	m_SelectedFriendNode.Reset();
	m_SelectedClanMemberNode.Reset();

	SetPushedStateButton(MBT_COMMUNITY, FALSE);

	// Note����
	ResetNote();
}

BOOL CPopupCommunity::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (pObj && pObj->IsExactTypeOf(CContextMenu::GetClassMeta()))
		{
			// ���� ���õǾ��ִ� ģ��
			switch(event)
			{
			case CMC_WHISPER		:		OnWhisper();	break;
			case CMC_INVITE			:		OnInviteRoom();	break;
			case CMC_VISIT			:		OnVisit();		break;
			case CMC_ADD_FRIEND		:		OnAddFriend(); break;

			case CMC_CLAN_INVITE	:
				if ( ! CanClanInvite() )
					return FALSE;
				OnClanInvite();
				break;

			default:
				return FALSE;
			}

			return TRUE;
		}

		if (I3_EVT_INPUT == event)
		{
			I3ASSERT(pObj != NULL);
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
			{
				if (m_pNicknameWin->isEnable())
				{
					OnNickCancel();
					return TRUE;
				}

				#ifdef USE_MODE_RECORD
				if (m_pUserDetailInfoMsg->isEnable())
				{
					OnUserDetailInfoPupupMsgCancel();
					return TRUE;
				}

				if (m_pUserDetailInfoPopup->isEnable())
				{
					OnContactClose();
					return TRUE;
				}
				#else
				if (m_pContactChild->isEnable())
				{
					OnContactClose();
					return TRUE;
				}
				#endif

				if (m_pCommunityWin->isEnable())
				{
					OnLeave();
					return TRUE;
				}
			}

			if( InputKeyboardEnter())
			{
				if (m_pNicknameWin->isEnable())
				{
					BOOL nickFocus = m_pNicknameInput->getIsFocus();

					if (FALSE == nickFocus)
					{
						// ENTERŰ�� focus������ IME control���� ó���ϱ⶧����,
						// focus������ IME control�� ���� ��쿡�� ó���Ѵ�.

						OnNickConfirm();
						return TRUE;
					}
				}
			}
		}
	}

	I3ASSERT(pObj != NULL);
	return CPopupBase::OnEvent(event, pObj, param2, code);
}

void CPopupCommunity::OnGameEvent( INT32 event, INT32 arg)
{
}

void CPopupCommunity::OnWheelUpdate(INT32 scrollby)
{
	if (m_pContactSect->isEnable())
	{
		if (0 < scrollby)
		{
			for(INT32 i = 0; i < scrollby; i++)
			{
				m_pContactList->MovePrev();
			}
		}
		else
		{
			for(INT32 i = 0; i > scrollby; i--)
			{
				m_pContactList->MoveNext();
			}
		}

		_UpdateContactList();
	}
	else if (m_pNoteSect->isEnable())
	{
		if (0 < scrollby)
		{
			for(INT32 i = 0; i < scrollby; i++)
			{
				m_pNoteList->MovePrev();
			}
		}
		else
		{
			for(INT32 i = 0; i > scrollby; i--)
			{
				m_pNoteList->MoveNext();
			}
		}

		g_pGameContext->AddGameFlag( GAMEFLAG_NOTEREFRESH);
		_UpdateNote();
	}
}

void CPopupCommunity::OnContactTab(void)
{
	m_pContactTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pNoteTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_pNoteSect->SetEnable(FALSE);
	m_pNoteGageBg->SetEnable( FALSE);
	m_pContactSect->SetEnable(TRUE);
	m_pNoteChild->SetEnable(FALSE);

	// �������� ���Ǵ� �������� �ʱ�ȭ�մϴ�.
	{
		m_SelectedNote = -1;
		m_SelectedNoteIndex = -1;
	}
}

void CPopupCommunity::OnNoteTab(void)
{
	if ( ! g_pConfigEx->GetCommunity().Enable_Note )
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_PREPARE_PLAN"));/*���� ���� �����Դϴ�.*/
		return;
	}

	m_pContactTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pNoteTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_pNoteSect->SetEnable(TRUE);
	m_pNoteGageBg->SetEnable( TRUE);
	m_pContactSect->SetEnable(FALSE);

	g_pCommunity->SetCommunityUserInfo(FALSE);
	
	ReceiveAllNote();
	//
	EndAlarmNote();
}

void CPopupCommunity::OnLeave(void)
{
	OnClosePopup();
}

void CPopupCommunity::OnProfile(void)
{
	// dummy�� ģ���������� �ϴ� ��ü

	if (-1 != m_SelectedContact)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);

		if(node && FRIEND_STATE_HOLD == GET_FRIEND_STATE(node->_state))
		{
			m_acceptFriendIdx = m_SelectedContact;

			char message[MAX_STRING_COUNT];

			sprintf_s(message, GAME_STRING("STR_POPUP_Q_MESSAGE_ACCEPT_TO_FRINED"), node->_nick);/*[%s]���� ģ����û�� �����Ͻðڽ��ϱ�?*/
			STRING_POPUP_Q(message, MAKE_CALLBACK(CbAcceptFriend), this);
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_READY_FRIEND"));/*��ϴ�� ���� ģ���� �����ϼž��մϴ�.*/
		}
	}
	else
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_ACCEPT"));/*�����Ͻ� ģ���� �����ϼž��մϴ�.*/
	}
}

//////////////////////////////////////////////////////////////////////////
#ifdef USE_MODE_RECORD
void CPopupCommunity::OnUserDetailInfo(void)
{
	m_pUserDetailInfoMsg->SetEnable(TRUE);
}

void CPopupCommunity::OnUserDetailInfoPupupMsgCancel(void)
{
	// �� ���� ���� Ȯ��â�� �ݴ´�.	
	m_pUserDetailInfoMsg->SetEnable(FALSE);
}

void CPopupCommunity::OnUserDetailInfoPupupMsgConfirm(void)
{
	// �� ���� ���� Ȯ��â�� �ݴ´�.	
	m_pUserDetailInfoMsg->SetEnable(FALSE);

	//
	m_bUserDetailInfoFlag = FALSE;
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	if (SUBTRACTE_POINT > MyUserInfoBasic._point)
	{
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_NO_POINT"));		//	"����Ʈ�� �����մϴ�."
		return;
	}

	switch(m_currentTab)
	{
	case COMMUNITY_FRIEND_TAB:	// ģ��
		{
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);
			if(NULL == node) break;

			// �ڱ� �ڽ��̸� �ڽ��� ���� ǥ��, �ƴϸ� ���Ӽ����� ���� ��û
			if(i3String::Compare( MyUserInfoBasic._nick, node->_nick) == 0)
			{
				m_bUserDetailInfoFlag = TRUE;		
			}
			else
			{
				m_SelectUid = node->_uid;
				g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_ALL_DB_N, &m_SelectUid);
			}				
		}
		break;
	case COMMUNITY_CLAN_TAB:	// Ŭ��
		{
			const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);
			I3ASSERT(node != NULL);
			if (!node)	return;	

			// �ڱ� �ڽ��̸� �ڽ��� ���� ǥ��, �ƴϸ� ���Ӽ����� ���� ��û
			if(i3String::Compare( MyUserInfoBasic._nick, node->_nick) == 0)
			{
				m_bUserDetailInfoFlag = TRUE;		
			}
			else
			{
				m_SelectUid = node->_uid;
				g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_ALL_DB_N, &m_SelectUid);
			}	
		}		
		break;
	default:
		{
			// Error
			return;
		}		
		break;
	}	

	DEACTIVATE_BUTTON( m_pUserDetailInfoDetail);
}
#endif

void CPopupCommunity::OnAddFriend(void)
{
	char nick[MAX_STRING_COUNT];
	// Ŭ������ ģ���� �߰�
	const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);

	memcpy(nick, node->_nick, sizeof(char) * NET_NICK_NAME_SIZE);
	_AddFriend(nick);
}

void CPopupCommunity::OnVisit(void)
{
	//TODO: ã�ư���
	// Ŭ�� ��ġ ���̸� ã�ư��� ����
	if( g_pFramework->GetClanContext()->InBattleTeam() )
		return;

	const CServerInfo * pMyServerInfo = g_pServerInfo->GetCurGameServer();
	CHECK_ASSERT(pMyServerInfo != NULL);


	CUserLocationInfo	UserLocInfo;
	if (!GetCurSelUserLoccationInfo(UserLocInfo))
		return;

	const CServerInfo * pOtherServerInfo = g_pServerInfo->GetServerInfo( UserLocInfo.GetServerNumber() );
	CHECK_ASSERT(pOtherServerInfo != NULL);

	// ���� ��� ��ȸ�����̸� �̵��� �����ϵ��� �Ѵ�.
	if ( pMyServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP ||
		pOtherServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP )
	{
		if ( pMyServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			SYSTEM_POPUP2(GAME_STRING("STR_TBL_GUI_CAN_NOT_VISIT_FROM_IN_CHAMPIONSHIP_USER"));
			return;
		}
		if ( pOtherServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			SYSTEM_POPUP2(GAME_STRING("STR_TBL_GUI_CAN_NOT_VISIT_TO_IN_CHAMPIONSHIP_USER"));
			return;
		}
	}


	switch( m_currentTab )
	{
	case COMMUNITY_FRIEND_TAB:
		OnVisitFriend();
		break;
	case COMMUNITY_CLAN_TAB:
		OnVisitClan();
		break;
	}
}

void CPopupCommunity::OnWhisper(void)
{
	char * pszNick = NULL;

	if( m_SelectedFriendNode._nick[0] != 0)
		pszNick = &m_SelectedFriendNode._nick[0];

	if( m_SelectedClanMemberNode._nick[0] != 0)
		pszNick = &m_SelectedClanMemberNode._nick[0];

	CStepBase * pStep =  ((CStageReady*) m_pParent)->GetCurrentStep();
	CPhaseBase * pPhase = ((CStageReady*) m_pParent)->GetCurrentPhase();
	CGameChatBody * pChatBody = NULL;

	//	GUI Phase ������ GUI Step�� �� ������ GUI Step�� ä�� �����⸦ �켱���� �Ѵ�.
	if( ((CStageReady*) m_pParent)->GetCurrentStepType() != STEP_NULL)
	{
		pChatBody = pStep->GetChatBody();

		if( pChatBody != NULL)
			pChatBody->OnWhisper(pszNick);
	}

	//	GUI Phase ������ �� �ִ� GUI Step�� ������ GUI Phase�� ä������ ������.
	if ( pPhase != NULL)
	{
		pChatBody = pPhase->GetChatBody();

		if( pChatBody != NULL)
			pChatBody->OnWhisper(pszNick);
	}
}

void CPopupCommunity::OnClanInvite(void)
{
	// ģ���� Ŭ�������� �ʴ�
	const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);
	if(NULL == node) return;

	g_pFramework->GetClanContext()->SendClanInviteUID(node->_uid, 0);

	// ���� �ڵ�
	//const CLAN_MEMBER_NODE* node = g_pCommunity->GetClanMember(m_SelectedContact);
	//g_pFramework->GetClanContext()->SendClanInviteUID( node->_uid, 0 );
}

void CPopupCommunity::OnInviteRoom(void)
{
	const CServerInfo * pMyServerInfo = g_pServerInfo->GetCurGameServer();
	CHECK_ASSERT(pMyServerInfo != NULL);


	CUserLocationInfo	UserLocInfo;
	if (!GetCurSelUserLoccationInfo(UserLocInfo))
		return;

	const CServerInfo * pOtherServerInfo = g_pServerInfo->GetServerInfo( UserLocInfo.GetServerNumber() );
	CHECK_ASSERT(pOtherServerInfo != NULL);

	if ( pMyServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP ||
		pOtherServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP )
	{
		if ( pMyServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			SYSTEM_POPUP2(GAME_STRING("STR_TBL_GUI_CAN_NOT_INVITE_FROM_IN_CHAMPIONSHIP_USER"));
			return;
		}
		if ( pOtherServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			SYSTEM_POPUP2(GAME_STRING("STR_TBL_GUI_CAN_NOT_INVITE_TO_IN_CHAMPIONSHIP_USER"));
			return;
		}
	}


	// ���� ��ġ ������ �ƴ����� ���� üũ
	if( g_pFramework->GetClanContext()->InBattleTeam() )
	{
		OnClanRoomInvite();
	}
	else
	{
		OnFriendInvite( m_currentTab );
	}
}


void CPopupCommunity::OnVisitFriend(void)
{
	//TODO: ã�ư���
	if (-1 != m_SelectedContact)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);
		
		if (node && FRIEND_STATE_ROOM == GET_FRIEND_STATE(node->_state))
		{
			// ���� ���ڷ뿡 �ְ�, ����, ü�� ���� ������ �̹� �濡 �������Դϴ�.
			BOOL	SameRoom = g_pGameContext->IsSameRoom(node->_state);
			if( PHASE_READY_ROOM == ((CStageReady*)m_pParent)->GetCurrentPhaseType() && SameRoom)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*�̹� �濡 �������Դϴ�.*/
			}
			else
			{
				g_pCommunity->SetVisitViewFriend(m_SelectedContact);
				((CStageReady*)m_pParent)->OnInviteMove();
			}
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_FRIEND_IN_THE_ROOM"));/*ã�ư��÷��� ģ���� �濡 �־���մϴ�.*/
		}
	}
	else
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_PLEASE_MESSAGE_FRIEND_TO_VISIT"));/*ã�ư� ģ���� �����ϼž��մϴ�.*/
	}
}

void CPopupCommunity::OnVisitClan(void)
{
	if (-1 == m_SelectedContact)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_PLEASE_MESSAGE_FRIEND_TO_VISIT"));/*ã�ư� ģ���� �����ϼž��մϴ�.*/
		return;
	}

	const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);
	if( node == NULL )
		return;

	if( GET_FRIEND_MATCH( node->_state ) > 0 )
	{
		if ( FRIEND_STATE_ROOM != GET_FRIEND_STATE(node->_state))
		{
			g_pCommunity->SetVisitViewClan(m_SelectedContact);
			((CStageReady*)m_pParent)->OnInviteMoveClan();
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_VISIT_MATCHING"));/*��ġ �߿��� ã�ư��⸦ �Ͻ� �� �����ϴ�.*/
		}
	}
	else
	{
		if (node && FRIEND_STATE_ROOM == GET_FRIEND_STATE(node->_state))
		{
			// ���� ���ڷ뿡 �ְ�, ����, ü�� ���� ������ �̹� �濡 �������Դϴ�.
			BOOL	SameRoom = g_pGameContext->IsSameRoom(node->_state);
			if( PHASE_READY_ROOM == ((CStageReady*)m_pParent)->GetCurrentPhaseType() && SameRoom)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*�̹� �濡 �������Դϴ�.*/
			}
			else
			{
				g_pCommunity->SetVisitViewClan(m_SelectedContact);
				((CStageReady*)m_pParent)->OnInviteMoveClan();
			}
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_FRIEND_IN_THE_ROOM"));/*ã�ư��÷��� ģ���� �濡 �־���մϴ�.*/
		}
	}


}

void CPopupCommunity::OnFriendInvite( COMMUNITY_TAB currentTab )
{
	if ( PHASE_READY_ROOM != ((CStageReady*)m_pParent)->GetCurrentPhaseType() )
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_IN_ROOM"));/*�濡���� �ʴ��� �� �ֽ��ϴ�.*/
		return;
	}

	if (0 > m_SelectedContact)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_INVITE"));/*�ʴ��� ģ���� �����ϼž� �մϴ�.*/
		return;
	}
	
	switch( currentTab )
	{
	case COMMUNITY_FRIEND_TAB:
		{
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);

			if (NULL == node)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_INVITE"));/*�ʴ��� ģ���� �����ϼž� �մϴ�.*/
				return;
			}

			if (FRIEND_STATE_ONLINE > GET_FRIEND_STATE(node->_state))
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_ONLINE_FRIEND"));/*���� ���� ģ���� �ʴ��� �� �ֽ��ϴ�.*/
				return;
			}

			BOOL	SameRoom = g_pGameContext->IsSameRoom(node->_state);
			if (FRIEND_STATE_ROOM != GET_FRIEND_STATE(node->_state) || SameRoom == FALSE)
			{
				g_pCommunity->InviteViewFriend(m_SelectedContact);
			}
			else
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*�̹� �濡 �������Դϴ�.*/
			}
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);

			if (NULL == node)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_INVITE"));/*�ʴ��� ģ���� �����ϼž� �մϴ�.*/
				return;
			}

			if (FRIEND_STATE_ONLINE > GET_FRIEND_STATE(node->_state))
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_ONLINE_FRIEND"));/*���� ���� ģ���� �ʴ��� �� �ֽ��ϴ�.*/
				return;
			}

			BOOL	SameRoom = g_pGameContext->IsSameRoom(node->_state);
			if (FRIEND_STATE_ROOM != GET_FRIEND_STATE(node->_state) || SameRoom == FALSE)
			{
				g_pCommunity->InviteViewClan(m_SelectedContact);
			}
			else
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*�̹� �濡 �������Դϴ�.*/
			}
		}
		break;
	}

}

void CPopupCommunity::OnClanRoomInvite(void)
{
  	if ( ( PHASE_LOBBY != ((CStageReady*)m_pParent)->GetCurrentPhaseType() ))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_CLAN_IN_LOBBY"));/*Ŭ�� �� �κ񿡼��� �ʴ��� �� �ֽ��ϴ�.*/
		return;
	}

	if (0 > m_SelectedContact)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_CLAN_MEMBER"));/*�ʴ��� Ŭ������ �����ϼž� �մϴ�.*/
		return;
	}

	const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);

	if (NULL == node)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_CLAN_MEMBER"));/*�ʴ��� Ŭ������ �����ϼž� �մϴ�.*/
		return;
	}

	if (FRIEND_STATE_ONLINE > GET_FRIEND_STATE(node->_state))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_ONLINE_FRIEND"));/*���� ���� ģ���� �ʴ��� �� �ֽ��ϴ�.*/
		return;
	}

	if (FRIEND_STATE_ROOM != GET_FRIEND_STATE(node->_state) ||
		g_pServerInfo->GetCurGameServerIndex() != GET_FRIEND_SERVER(node->_state) ||
		g_pServerInfo->GetCurChannelNumber() != GET_FRIEND_CHANNEL(node->_state) ||
		g_pGameContext->getClanBattleMyTeamInfo()->_Team._Naming != GET_FRIEND_MATCH( node->_state )-1)
	{
		if (g_pCommunity->InviteViewClan(m_SelectedContact))
		{
			char message[MAX_STRING_COUNT];

			sprintf_s(message, GAME_STRING("STR_POPUP_MESSAGE_SEND_INVITE_MESSAGE"), node->_nick);/*[%s]�Կ��� �ʴ� �޽����� �����Ͽ����ϴ�.*/
			STRING_POPUP(message);
		}
	}
	else
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_ALREADY_JOIN_TEAM"));/*�̹� ���� �������Դϴ�.*/
	}
}

void CPopupCommunity::OnContactClose(void)
{
	#ifdef USE_MODE_RECORD
	m_bRecordTabFlag = FALSE;
	m_bUserDetailInfoFlag = FALSE;	
	m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pModeRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	// �� ���� ���� Ȯ��â�� �ݴ´�.		
	m_pUserDetailInfoMsg->SetEnable(FALSE);	
	g_pCommunity->SetUserDetailInfoFlag(FALSE);
	ACTIVATE_BUTTON(m_pUserDetailInfoDetail);
	#endif

	g_pCommunity->SetCommunityUserInfo(FALSE);
}

void CPopupCommunity::OnFriendTab(void)
{
	OnContactClose();

  	m_pSeasonRecordKey->SetText(GAME_STRING("STR_POPUP_SEASON_HISTORY"));/*��������*/
	m_pTotalRecordKey->SetText(GAME_STRING("STR_POPUP_TOTAL_HISTORY"));/*�������*/
	m_pAdd->SetCaption( GAME_STRING("STR_POPUP_ADD_FRIEND"));/*ģ���߰�*/

	m_pAdd->SetEnable( TRUE);
	m_pFind->SetEnable( TRUE);

	m_pFriendTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pBlockTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
  	m_pClanTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_SelectedContact = -1;
	
	//	default
	#ifdef USE_MODE_RECORD
	m_pUserDetailInfoFriend->SetEnable(TRUE);
	m_pUserDetailInfoInvite->SetEnable(TRUE);
	m_pUserDetailInfoProfile->SetEnable(TRUE);
	m_pUserDetailInfoFriendRefuse->SetEnable(TRUE);
	#else
	m_pFriend->SetEnable(TRUE);
	m_pInvite->SetEnable(TRUE);
	m_pProfile->SetEnable(TRUE);
	#ifdef USE_FRIEND_REFUSE
	m_pFriendRefuse->SetEnable(TRUE);	
	#endif
	#endif

	m_currentTab = COMMUNITY_FRIEND_TAB;

	ACTIVATE_BUTTON( m_pFind );
	ACTIVATE_BUTTON( m_pAdd );
//	ACTIVATE_BUTTON( m_pDelete );

	for(INT32 i=0; i<2; i++)
		m_pRecordTab[i]->SetEnable(FALSE);

	g_pCommunity->SetCommunityUserInfo(FALSE);
}

void CPopupCommunity::OnBlockTab(void)
{
	OnContactClose();

#if defined( USE_COMMUNITY_BLOCK)
	m_pAdd->SetCaption( GAME_STRING("STR_POPUP_ADD_BLOCK"));/*�����߰�*/

	m_pAdd->SetEnable( TRUE);
	m_pFind->SetEnable( TRUE);

	m_pFriendTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pBlockTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pClanTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_SelectedContact = -1;

	m_currentTab = COMMUNITY_BLOCK_TAB;

	ACTIVATE_BUTTON( m_pFind );
	ACTIVATE_BUTTON( m_pAdd );
//	ACTIVATE_BUTTON( m_pDelete );

	g_pCommunity->SetCommunityUserInfo(FALSE);
#else
	STRING_POPUP(GAME_STRING("STR_POPUP_PREPARE_PLAN"));/*���� ���� �����Դϴ�.*/
#endif
}

void CPopupCommunity::OnClanTab(void)
{
	if ( ! g_pConfigEx->GetClan().Enable )
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_PREPARE_PLAN"));		//���� ���� �����Դϴ�.
		return;
	}

	OnContactClose();

	m_pSeasonRecordKey->SetText(GAME_STRING("STR_POPUP_CLAN_SEASON_HISTORY"));	//Ŭ�� ��������
	m_pTotalRecordKey->SetText(GAME_STRING("STR_POPUP_CLAN_TOTAL_HISTORY"));	//Ŭ�� �������

	m_pAdd->SetEnable( FALSE);
	m_pFind->SetEnable( FALSE);

    m_pFriendTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pBlockTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pClanTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	
	m_pProfile->SetEnable(FALSE);
	m_pFriendRefuse->SetEnable(FALSE);

	#ifdef USE_MODE_RECORD
	m_pUserDetailInfoProfile->SetEnable( FALSE );
	m_pUserDetailInfoFriendRefuse->SetEnable( FALSE );
	#endif

	m_SelectedContact = -1;

	m_currentTab = COMMUNITY_CLAN_TAB;

	DEACTIVATE_BUTTON( m_pFind );
	DEACTIVATE_BUTTON( m_pAdd );
//	DEACTIVATE_BUTTON( m_pDelete );

	ACTIVATE_BUTTON( m_pRecordTab[0]);
	ACTIVATE_BUTTON( m_pRecordTab[1]);
	
	g_pCommunity->SetCommunityUserInfo(FALSE);
}

void CPopupCommunity::OnRecordTab(INT32 idx)
{
	const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);
	I3ASSERT( node != NULL);

	switch(idx)
	{
	case 0:		//	Ŭ������
		{
			// ���� ���� �ִ� �������� ��û�� ��ŵ.
			if ( g_pCommunity->BeCommunityUserInfo() && GetCurrentUserInfoUID() == node->_uid && GetCurrentUserInfoType() == COMMUNITY_CLAN_TAB)
				break;

			m_pRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);

			m_pSeasonRecordKey->SetText(GAME_STRING("STR_POPUP_CLAN_SEASON_HISTORY"));/*Ŭ�� ��������*/
			m_pTotalRecordKey->SetText(GAME_STRING("STR_POPUP_CLAN_TOTAL_HISTORY"));/*Ŭ�� �������*/

			g_pCommunity->SetCommnunityInfoNick(node->_nick);
			//g_pCommunity->SetCommnunityConnectState(((UINT32)node->_state));

			SetCurrentUserInfoUID(node->_uid);
			SetCurrentUserInfoType(COMMUNITY_CLAN_TAB);

			g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_DB_N, &node->_uid);
			// TODO : Ŭ������ ��ȹ �Ϸ� �� �۾� ����.
			//g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_CLAN_N, &node->_uid);
		}
		break;
	case 1:		//	��������
		{
			if ( g_pCommunity->BeCommunityUserInfo() && GetCurrentUserInfoUID() == node->_uid && GetCurrentUserInfoType() == COMMUNITY_FRIEND_TAB)
				break;

			m_pRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_DISABLED);

			m_pSeasonRecordKey->SetText(GAME_STRING("STR_POPUP_PRIVATE_SEASON_HISTORY"));/*���� ��������*/
			m_pTotalRecordKey->SetText(GAME_STRING("STR_POPUP_PRIVATE_TOTAL_HISTORY"));/*���� �������*/

			g_pCommunity->SetCommnunityInfoNick(node->_nick);
			//g_pCommunity->SetCommnunityConnectState(node->_state);

			SetCurrentUserInfoUID(node->_uid);
			SetCurrentUserInfoType(COMMUNITY_FRIEND_TAB);
			g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_DB_N, &node->_uid);
		}
		break;
	}
}

void CPopupCommunity::OnFind(void)
{
#if 0
	STRING_POPUP(GAME_STRING("STR_POPUP_PREPARE_PLAN"));/*���� ���� �����Դϴ�.*/
#else
	m_pCommunityWin->setInputDisable(TRUE);

	m_pNicknameCaption->SetText(GAME_STRING("STR_POPUP_FIND_USER"));/*����ã��*/
	m_pNicknameMessage->SetText(GAME_STRING("STR_POPUP_MESSAGE_FINDING_NAME"));/*ã�� ������� �г�����\n�Է��Ͻʽÿ�.*/

	m_pNicknameWin->SetEnable(TRUE);
	m_pNicknameInput->SetText("");
	m_pNicknameInput->SetIMEText("");
	m_pNicknameInput->SetFocus(TRUE);

	m_nickPopupType = NPT_FIND_USER;
#endif
}

void CPopupCommunity::OnAdd(void)
{
	m_pCommunityWin->setInputDisable(TRUE);

	switch(m_currentTab)
	{
	case COMMUNITY_FRIEND_TAB:
		{
			m_pNicknameCaption->SetText(GAME_STRING("STR_POPUP_ADD_FRIEND"));/*ģ���߰�*/
			m_pNicknameMessage->SetText(GAME_STRING("STR_POPUP_MESSAGE_INPUT_FRIEND_NAME"));/*ģ����Ͽ� �߰��� ������� �г�����\n�Է��Ͻʽÿ�.*/

			m_pNicknameWin->SetEnable(TRUE);
			m_pNicknameInput->SetText("");
			m_pNicknameInput->SetIMEText("");
			m_pNicknameInput->SetFocus(TRUE);

			m_nickPopupType = NPT_ADD_FRIEND;
		}
		break;
	case COMMUNITY_BLOCK_TAB:
		{
			m_pNicknameCaption->SetText(GAME_STRING("STR_POPUP_ADD_BLOCK"));/*����߰�*/
			m_pNicknameMessage->SetText(GAME_STRING("STR_POPUP_MESSAGE_INPUT_BLOCK_NAME"));/*���� ��Ͽ� �߰��� ������� �г����� �Է� �Ͻʽÿ�.*/

			m_pNicknameWin->SetEnable(TRUE);
			m_pNicknameInput->SetText("");
			m_pNicknameInput->SetIMEText("");
			m_pNicknameInput->SetFocus(TRUE);

			m_nickPopupType = NPT_ADD_BLOCK;
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			//
		}
		break;
	}

}

void CPopupCommunity::OnDelete(void)
{
	m_lastDeleteFriend = m_SelectedContact;

	const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);

	if (NULL == node)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_DELETE"));/*������ ģ���� �����ϼž��մϴ�.*/
	}
	else
	{
		char message[MAX_STRING_COUNT];

		sprintf_s(message, GAME_STRING("STR_POPUP_MESSAGE_DELETE_FRIEND"), node->_nick);/*[%s]���� ģ����Ͽ��� �����մϴ�.*/
		STRING_POPUP_Q(message, MAKE_CALLBACK(CbDeleteFriend), this);
	}
}

void CPopupCommunity::OnRefuseFriend(void)
{
	m_lastDeleteFriend = m_SelectedContact;

	const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);

	if (NULL == node)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_REFUSE"));/*ģ����û�� ������ ģ���� �����ϼž��մϴ�.*/
	}
	else
	{
		char message[MAX_STRING_COUNT];

		sprintf_s(message, GAME_STRING("STR_POPUP_MESSAGE_REFUSE_FRIEND"), node->_nick);/*[%s]���� ģ����û�� �����Ͻðڽ��ϱ�?*/
		STRING_POPUP_Q(message, MAKE_CALLBACK(CbDeleteFriend), this);
	}
}

void CPopupCommunity::OnContactDrag(void)
{
	_UpdateContactList();
}

void CPopupCommunity::OnContactFocus(UINT32 idxItem)
{
	I3_BOUNDCHK( idxItem, MAX_SLOT_FOR_CONTACT_LIST);
	m_pContactFocus->setPositionY(30.f + (idxItem * 30.f));
	m_pContactFocus->SetEnable(TRUE);
}

void CPopupCommunity::OnContactSelect(UINT32 idxItem)
{
	m_SelectedContact = idxItem;

	m_SelectedFriendNode.Reset();
	m_SelectedClanMemberNode.Reset();

//	m_pDelete->setControlDisable(FALSE);
	#ifdef USE_MODE_RECORD
	if(TRUE == m_bUserDetailInfoFlag)
	{
		DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
	}
	else
	{
		ACTIVATE_BUTTON(m_pUserDetailInfoDetail);
	}
	#endif
	
	switch( m_currentTab)
	{
	case COMMUNITY_FRIEND_TAB:
		{
			FRIEND_NODE* node = (FRIEND_NODE*) g_pCommunity->GetViewFriend(idxItem);			

			if (node)
			{
				if ( g_pCommunity->BeCommunityUserInfo() && GetCurrentUserInfoUID() == node->_uid && GetCurrentUserInfoType() == COMMUNITY_FRIEND_TAB)
					break;

				#ifdef USE_MODE_RECORD
				m_bRecordTabFlag = FALSE;
				m_bUserDetailInfoFlag = FALSE;	
				m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
				m_pModeRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
				// �� ���� ���� Ȯ��â�� �ݴ´�.		
				m_pUserDetailInfoMsg->SetEnable(FALSE);	
				g_pCommunity->SetUserDetailInfoFlag(FALSE);
				ACTIVATE_BUTTON(m_pUserDetailInfoDetail);
				#endif

				node->CopyTo( &m_SelectedFriendNode);

				g_pCommunity->SetCommnunityInfoNick(node->_nick);
				g_pCommunity->SetCommnunityConnectState(node->_state);

				SetCurrentUserInfoUID(node->_uid);
				SetCurrentUserInfoType(COMMUNITY_FRIEND_TAB);
				g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_DB_N, &node->_uid);
			}
			else
			{
				OnContactClose();				
			}
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			CLAN_MEMBER_NODE* node = (CLAN_MEMBER_NODE*) g_pCommunity->GetViewClanMember(idxItem);
	
			if (node)
			{
				node->CopyTo( &m_SelectedClanMemberNode);

				OnRecordTab(0);			//	default to clan score
			}
		}
		break;
	}

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CPopupCommunity::OnContactAccept(UINT32 idxItem)
{
#if 0
	if (idxItem >= g_pCommunity->GetFriendCount())	return;

	m_SelectedContact = idxItem;

	const FRIEND_NODE* node = g_pCommunity->GetViewFriend(idxItem);
	if(NULL == node) return;

	if (FRIEND_STATE_HOLD == GET_FRIEND_STATE(node->_state))
	{
		m_acceptFriendIdx = idxItem;

		char message[MAX_STRING_COUNT];

		sprintf_s(message, GAME_STRING("STR_POPUP_Q_MESSAGE_ACCEPT_TO_FRINED"), node->_nick);/*[%s]���� ģ����û�� �����Ͻðڽ��ϱ�?*/
		STRING_POPUP_Q(message, CbAcceptFriend, this);
	}
	else
	{
		OnContactSelect(idxItem);
	}
#endif
}

BOOL CPopupCommunity::_IsMyFriend( INT64 uid)
{
	for(UINT32 i=0; i<g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* pNode = g_pCommunity->GetViewFriend(i);
		if(NULL == pNode) continue;

		if( pNode->_uid == uid)
			return TRUE;
	}

	return FALSE;
}

BOOL CPopupCommunity::_IsMyClanMember( INT64 uid)
{
	for(UINT32 i=0; i<g_pCommunity->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* pNode = g_pCommunity->GetViewClanMember(i);

		if( pNode->_uid == uid)		return TRUE;
	}

	return FALSE;
}

BOOL CPopupCommunity::_CheckCanWhisper(FRIEND_STATE State)
{
	//	���ӵǾ� ������
	if( State != FRIEND_STATE_OFFLINE)
	{
		CStepBase * pStep =  ((CStageReady*) m_pParent)->GetCurrentStep();
		CPhaseBase * pPhase = ((CStageReady*) m_pParent)->GetCurrentPhase();
		CGameChatBody * pChatBody = NULL;

		//	GUI Phase ������ GUI Step�� �� ������ GUI Step�� ä�� �����⸦ �켱���� �Ѵ�.
		if( ((CStageReady*) m_pParent)->GetCurrentStepType() != STEP_NULL)
		{
			pChatBody = pStep->GetChatBody();

			if( pChatBody != NULL)		return TRUE;
		}

		//	GUI Phase ������ �� �ִ� GUI Step�� ������ GUI Phase�� ä������ ������.
		if ( pPhase != NULL)
		{
			pChatBody = pPhase->GetChatBody();

			if( pChatBody != NULL)		return TRUE;
		}
	}

	return FALSE;
}

void CPopupCommunity::OnContactContext(UINT32 idxItem)
{
	m_SelectedFriendNode.Reset();
	m_SelectedClanMemberNode.Reset();

	m_SelectedContact = idxItem;

	ContextMenuReset();

	switch( m_currentTab )
	{
	case COMMUNITY_FRIEND_TAB:
		{
			if( g_pCommunity->GetFriendCount() <= idxItem)
			{
				m_SelectedContact = -1;
				return;
			}

			FRIEND_NODE* node = (FRIEND_NODE*) g_pCommunity->GetViewFriend(idxItem);
			if(NULL == node) break;

			node->CopyTo( &m_SelectedFriendNode);

			if( _CheckCanWhisper((FRIEND_STATE)GET_FRIEND_STATE(node->_state)))
				ContextMenuAddCommand(CMC_WHISPER);

			switch(GET_FRIEND_STATE(node->_state))
			{
			case FRIEND_STATE_ONLINE:

				if( TRUE == g_pFramework->GetClanContext()->InBattleTeam() )
					break;

				if (PHASE_READY_ROOM != ((CStageReady*)m_pParent)->GetCurrentPhaseType())
					break;

				ContextMenuAddCommand(CMC_INVITE);
				break;

			case FRIEND_STATE_ROOM:
				if( TRUE == g_pFramework->GetClanContext()->InBattleTeam() )
					break;

				if (PHASE_READY_ROOM != ((CStageReady*)m_pParent)->GetCurrentPhaseType())
				{
					ContextMenuAddCommand(CMC_VISIT);
					break;
				}

				BOOL	SameRoom = g_pGameContext->IsSameRoom(node->_state);
				// ���� ���� �ƴ϶�� �ʴ�� ã�ư���
				if (SameRoom == FALSE)
				{
					ContextMenuAddCommand(CMC_INVITE);
					ContextMenuAddCommand(CMC_VISIT);
				}
				break;
			}

			// Ŭ�� �ʴ���� ������ Ŭ���ʴ�
			if ( CanClanInvite() )
			{
				if ((g_pFramework->GetClanContext()->IsPossibleClanInvite()) &&
					(!_IsMyClanMember( node->_uid)))
				{
					ContextMenuAddCommand(CMC_CLAN_INVITE);
				}
			}
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			if( g_pCommunity->GetClanMemberCount() <= idxItem)
			{
				m_SelectedContact = -1;
				return;
			}

			CLAN_MEMBER_NODE* node = (CLAN_MEMBER_NODE*) g_pCommunity->GetViewClanMember(idxItem);

			node->CopyTo( &m_SelectedClanMemberNode);

			// Ŭ�� ���� 64 �� ģ���� ���� 32�� �� ���� �մϴ�.
			UINT32 ui32State = FRIEND_STATE_64_TO_32(node->_state);
			if( _CheckCanWhisper( (FRIEND_STATE)GET_FRIEND_STATE(ui32State) ))
				ContextMenuAddCommand(CMC_WHISPER);

			// Ŭ������ ģ���� �ƴ� ���
			// ģ�� �߰� ����
			if ( !_IsMyFriend( node->_uid))
			{
				ContextMenuAddCommand(CMC_ADD_FRIEND);
			}

			switch(GET_FRIEND_STATE(node->_state))
			{
				//ģ���� ���翡 ������
			case FRIEND_STATE_ONLINE:
				{
					if( GET_FRIEND_MATCH( node->_state ) > 0 )			// ����� ��Ʋ �� �����϶�
					{
						if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() ) // ���� ���� ������
						{	// ã�ư��� ��
							ContextMenuAddCommand(CMC_VISIT);
						}
					}
					else												// ����� ��Ʋ �� ���� �ƴҶ�
					{
						// �ʴ��ϱ� ��
						if( PHASE_READY_ROOM == ((CStageReady*)m_pParent)->GetCurrentPhaseType() )
						{	// ã�ư��� ��
							ContextMenuAddCommand(CMC_INVITE);
						}
						else
						{
							if( g_pFramework->GetClanContext()->InBattleTeam() )
							{
								ContextMenuAddCommand(CMC_INVITE);
							}
						}
					}
				}
				break;
				//	ģ���� �濡 ������
			case FRIEND_STATE_ROOM:
				{
					if( GET_FRIEND_MATCH( node->_state ) == 0 )			// ����� ��Ʋ �� ���°� �ƴҶ�
					{
						if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
						{
							//	�ڽ��� �濡 �� ������ ģ���ʴ� Ȱ��
							if(PHASE_READY_ROOM == ((CStageReady*)m_pParent)->GetCurrentPhaseType())
								ContextMenuAddCommand(CMC_INVITE);

							ContextMenuAddCommand(CMC_VISIT);		//	ģ�� ã�ư��� Ȱ��
						}
						else
						{
							ContextMenuAddCommand(CMC_INVITE);
						}
					}
				}
				break;
			}
		}
		break;
	}

#if defined(CHANGE_ADDCOMMAND_METHOD)
	INT32 OldCmdMap[64] = {};
	INT32 NewCmdMap[64] = {};
	for ( INT32 i = 0 ; i < 64 ; ++ i )
	{
		OldCmdMap[i] = NewCmdMap[i] = 0xff;
	}

	((CStageReady*)m_pParent)->CopyContextMenuList(OldCmdMap);

	ContextMenuReset();

	{
		UINT64	State = 0;
		INT64	UID = 0;
		switch(m_currentTab)
		{
		case COMMUNITY_FRIEND_TAB:
			{
				FRIEND_NODE* node = (FRIEND_NODE*) g_pCommunity->GetViewFriend(idxItem);
				if(NULL == node) break;

				node->CopyTo( &m_SelectedFriendNode);
				State	= node->_state;
				UID		= node->_uid;
			}
			break;
		case COMMUNITY_CLAN_TAB:
			{
				CLAN_MEMBER_NODE* node = (CLAN_MEMBER_NODE*) g_pCommunity->GetViewClanMember(idxItem);

				node->CopyTo( &m_SelectedClanMemberNode);
				State	= node->_state;
				UID		= node->_uid;
			}
			break;
		}

		CUserLocationInfo	LocInfo(State);

		AddPopupCommand(CMC_ADD_FRIEND, LocInfo, UID);
		AddPopupCommand(CMC_CLAN_INVITE, LocInfo, UID);
		AddPopupCommand(CMC_INVITE, LocInfo, UID);
		AddPopupCommand(CMC_VISIT, LocInfo, UID);
		AddPopupCommand(CMC_WHISPER, LocInfo, UID);
	}


	((CStageReady*)m_pParent)->CopyContextMenuList(NewCmdMap);

	std::vector<INT32>	vNewCmpMap(&NewCmdMap[0], &NewCmdMap[64]);
	std::vector<INT32>	vOldCmpMap(&OldCmdMap[0], &OldCmdMap[64]);

	std::sort(vNewCmpMap.begin(), vNewCmpMap.end());
	std::sort(vOldCmpMap.begin(), vOldCmpMap.end());

	if ( vNewCmpMap != vOldCmpMap )
	{
		string	strNewCmd, strOldCmd;
		for ( size_t i = 0 ; i < vNewCmpMap.size() ; ++i )
		{
			if (vNewCmpMap[i] == 0xff )
				continue;
			char str[64] = {};
			i3String::Format(str, 64, "%d, ", vNewCmpMap[i]);
			strNewCmd += str;
		}

		strNewCmd += "\n\n";

		for ( size_t i = 0 ; i < vOldCmpMap.size() ; ++i )
		{
			if (vOldCmpMap[i] == 0xff )
				continue;
			char str[64] = {};
			i3String::Format(str, 64, "%d, ", vOldCmpMap[i]);
			strOldCmd += str;
		}

		I3FATAL("[Dev Msg]������ �˾� �޴� �߸��Ǿ����ϴ�. �����ϰ� �����ص� �˴ϴ�.\n%s\n%s", strNewCmd.c_str(), strOldCmd.c_str());
	}

#endif

	ContextMenuOpen(this, idxItem);
}

void CPopupCommunity::OnDeleteNote(void)
{
	INT32 i = 0;
	UINT8 DeleteCount = 0;
	INT32 DeleteArray[MAX_NOTE_COUNT];
	
	NOTE_RECEIVE* pNote = NULL;

	for( i=g_pGameContext->GetNoteReceiveCount() - 1; i>=0; i--)
	{
		if( m_bNoteCheck[i])
		{
			// if it Checked...
			pNote = g_pGameContext->GetReceiveNote( i);

			//	���������� ���� �����ǹǷ� ���⼭ �����Ǹ� �ȵ˴ϴ�. komet
			if( pNote->header._type != NT_GIFT)
			{
				DeleteArray[DeleteCount] = pNote->header._id;
				DeleteCount++;
			}
		}
	}

	// üũ�ڽ��� ���õǾ����͵�Ӹ� �ƴ϶�, ���� �������� �����ִ� ������ ���ؼ��� ����ó���� �����մϴ�.
	if( m_SelectedNoteIndex != -1)
	{
		UINT32 TopIdx = m_pNoteList->GetCurrentTop();
		INT32 nListIdx = g_pGameContext->GetNoteReceiveCount() - (m_SelectedNoteIndex+1)-TopIdx;
		if( m_bNoteCheck[nListIdx] == FALSE)
		{
			// if it Checked...
			pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( nListIdx);

			//	���������� ���� �����ǹǷ� ���⼭ �����Ǹ� �ȵ˴ϴ�. komet
			if( pNote->header._type != NT_GIFT)
			{
				DeleteArray[DeleteCount] = pNote->header._id;
				DeleteCount++;
			}
		}
	}

	for(UINT32 i = 0; i < MAX_NOTE_SLOT; i++)
	{
		m_noteSlot[i]._pCheck->setCheckBoxState( FALSE);
	}

	for(UINT32 i = 0; i < MAX_NOTE_COUNT; i++) m_bNoteCheck[i] = FALSE;

	if( DeleteCount>0)
	{
		g_pGameContext->SetEvent( EVENT_DELETE_NOTE, &DeleteCount, DeleteArray);

		OnNoteClose();
	}
}

void CPopupCommunity::OnWriteNote(void)
{
	SetNoteType( NOTE_TYPE_SEND);
	m_pNoteWhoValue->SetIMEText( "");
	m_pNoteValue->SetIMEText( "");

	m_pNoteAccept->SetEnable(FALSE);
	m_pNoteReject->SetEnable(FALSE);
	m_pNoteSend->SetEnable(TRUE);
	m_pNoteValue->SetEnable(TRUE);
}

void CPopupCommunity::OnNoteFocus(UINT32 idxItem)
{
	if( g_pGameContext->GetNoteReceiveCount() >= (INT32)(idxItem+1))
	{
		m_pNoteFocus->setPositionY(29.0f + (idxItem * 24.0f));
		m_pNoteFocus->SetEnable(TRUE);
	}
}

void CPopupCommunity::OnNoteSelect(UINT32 idxItem)
{
	// ���ڵ�� ������ �ʽ��ϴ�.
	{
		UINT8 ReadCount = 0;
		UINT32 TopIdx = m_pNoteList->GetCurrentTop();

		//
		PlaySound( GUI_SND_ITEM_SELECTING);

		ReceiveAllNote();

		INT32 nListIdx = g_pGameContext->GetNoteReceiveCount() - (idxItem+1) - TopIdx;

		SetNoteType( NOTE_TYPE_REPLY);

		NOTE_RECEIVE* pNote = NULL;
		pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( nListIdx);

		// �������·� �����մϴ�.
		pNote->header._state = NS_READED;

		//
		m_pNoteWhoValue->SetIMEText(pNote->body._sender);
	
		//
		m_pNoteValue->SetIMEText(pNote->body._message);

		m_noteSlot[idxItem]._pContent->SetTextColor(GetColor(GCT_NOTIFY));
		m_noteSlot[idxItem]._pWho->SetTextColor(GetColor(GCT_NOTIFY));
		m_noteSlot[idxItem]._pDate->SetTextColor(GetColor(GCT_NOTIFY));

		ReadCount++;
		g_pGameContext->SetEvent( EVENT_CHECK_READED_NOTE, &ReadCount, &pNote->header._id);
	}

}

void CPopupCommunity::OnSendNote(void)
{
	switch( m_nNoteType)
	{
	case NOTE_TYPE_SEND:
		{
			if(i3String::Length( m_pNoteWhoValue->GetText()) <= 0)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INPUT_RECEIVER"));/*�޴� ����� �Է��� �ּ���.*/
			}
			else if(i3String::Length( m_pNoteValue->GetText()) <= 0)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_INPUT_NOTE"));/*���� ������ �Է��� �ּ���.*/
			}
			else
			{
				if ( i3String::CompareCase( g_pGameContext->GetMyNickName(), m_pNoteWhoValue->GetText()) == 0)
				{
					STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_SEND_SELF"));/*�ڽſ��� ������ ���� �� �����ϴ�.*/
					m_pNoteWhoValue->SetIMEText( "");
					m_pNoteWhoValue->SetFocus( TRUE);
				}
				else
				{
					if( g_pFramework->IsFilter(m_pNoteValue->GetText()) == FALSE && m_nNoteType == NOTE_TYPE_SEND)
					{
						STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_INVALID_WORD"));/*�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�.*/
						m_pNoteValue->SetFocus( TRUE);
					}
					else
					{
						m_pNoteChild->SetEnable(FALSE);

						// ���� ������
				#ifdef USE_MESS_TEST
						if (FALSE == m_bReceiverUIDFlag)
						{
							NOTE_BODY note;
							i3String::Copy( note._sender, m_pNoteWhoValue->GetText(), NET_NICK_NAME_SIZE );
							i3String::Copy( note._message, m_pNoteValue->GetText(), NET_NOTE_SIZE );
							note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
							note._messageSize = (UINT8)(i3String::Length( note._message) + 1);
	
							g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
						}
						else
						{
							NOTE_BODY_BY_UID note;
							char Sender[NET_NICK_NAME_SIZE];

							m_bReceiverUIDFlag = FALSE;

							i3String::Copy( Sender, m_pNoteWhoValue->GetText(), NET_NICK_NAME_SIZE );

							// ���� ������� Ȯ��
							note._receiverUid = _FindNoteByUid(Sender);
							if (0 >= note._receiverUid)
							{
								break;
							}
							i3String::Copy( note._message, m_pNoteValue->GetText(), NET_NOTE_SIZE );
	
							note._messageSize = (UINT8)(i3String::Length( note._message) + 1);

							g_pGameContext->SetEvent( EVENT_SEND_NOTE_BY_UID, &note);
						}
				#else
						NOTE_BODY note;
						i3String::Copy( note._sender, m_pNoteWhoValue->GetText());
						i3String::Copy( note._message, m_pNoteValue->GetText());
						note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
						note._messageSize = (UINT8)(i3String::Length( note._message) + 1);

						g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
				#endif
					}
				}
			}
		}
		break;
	case NOTE_TYPE_REPLY:
	case NOTE_TYPE_GIFT:
		{
			m_bReceiverUIDFlag = TRUE;
			// �����ϱ�
			SetNoteType( NOTE_TYPE_SEND);

			m_pNoteValue->SetEnable(TRUE);
			m_pNoteValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
			m_pNoteValue->SetIMEText( "");
			m_pNoteValue->SetFocus( TRUE);
		}
		break;
	}
}

void CPopupCommunity::OnNoteClose(void)
{
	m_SelectedNote = -1;
	m_SelectedNoteIndex = -1;
	m_pContactList->SetCurValue(0.0f);
	m_pNoteList->SetCurValue(0.0f);
	ReceiveAllNote();

	m_pNoteChild->SetEnable(FALSE);
}

void CPopupCommunity::OnNoteAccept(BOOL bVal)
{
	UINT32 TopIdx = m_pNoteList->GetCurrentTop();
	INT32 nListIdx = g_pGameContext->GetNoteReceiveCount() - (m_SelectedNoteIndex+1)-TopIdx;

	if( 0 > nListIdx || nListIdx > g_pGameContext->GetNoteReceiveCount() - 1)
	{
		I3ASSERT_0;
		return;
	}
	
	// if it Checked...
	NOTE_RECEIVE * pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( nListIdx);

	//	Ŭ�� ���� ��û�� �ܿ�
	if(pNote->header._type == NT_CLAN_ASK)
	{
		UINT32 wClanDBIdx = (INT32) pNote->header._appendingData;

		g_pFramework->GetClanContext()->SendClanInviteAccept(wClanDBIdx, (UINT8) bVal);

		// �������·� �����մϴ�.
		pNote->header._state = NS_READED;

		// ���� ������ ������ �����Ⱓ�� 7�Ϸ� �����մϴ�.
		pNote->header._remainDay = 7;

		INT32 ReadCount = 1;
		g_pGameContext->SetEvent( EVENT_CHECK_READED_NOTE, &ReadCount, &pNote->header._id);
	}

	OnNoteClose();
}

void CPopupCommunity::OnNickConfirm(void)
{
	// �г��� �Է�â�� �����Ѵ�.
	m_pCommunityWin->setInputDisable(FALSE);
	m_pNicknameWin->SetEnable(FALSE);

	
	// �Էµ� �г����� �о�´�.
	char nick[MAX_STRING_COUNT];

	i3String::Copy( nick, m_pNicknameInput->getIMETextBuf(), MAX_STRING_COUNT );

	if (0 == i3String::Length(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INPUT_NAME"));/*�г����� �Է��ϼž� �մϴ�.*/
		return;
	}

	if (FALSE == CStringFilter::IsValidNickname(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_LIMITED_NAME_LENGTH"));/*�г����� 2�� �̻� 16�� ���ϸ� �����մϴ�.*/
		return;
	}


	// �ش� ����� ó���Ѵ�
	switch(m_nickPopupType)
	{
	case NPT_ADD_FRIEND:
		_AddFriend(nick);
		break;
	case NPT_FIND_USER:
		_FindUser(nick);
		break;
	case NPT_ADD_BLOCK:
		_AddBlock(nick);
		break;
	default:
		I3ASSERT_0;
		break;
	}
}

void CPopupCommunity::OnNickCancel(void)
{
	m_pCommunityWin->setInputDisable(FALSE);
	m_pNicknameWin->SetEnable(FALSE);
}

void CPopupCommunity::CbAcceptFriend(void* pThis,INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		CPopupCommunity* community = (CPopupCommunity*)pThis;
		g_pCommunity->AcceptViewFriend(community->m_acceptFriendIdx);
	}
}

void CPopupCommunity::CbDeleteFriend(void* pThis,INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		CPopupCommunity* community = (CPopupCommunity*)pThis;

		if (0 > community->m_lastDeleteFriend || FALSE == g_pCommunity->DeleteViewFriend(community->m_lastDeleteFriend))
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_DELETE"));/*������ ģ���� �����ϼž��մϴ�.*/
		}
		else
		{
			community->m_SelectedContact = -1;
			g_pCommunity->SetCommunityUserInfo(FALSE);
		}
	}
}

void CPopupCommunity::CbDeleteNote( void * pThis, INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		CPopupCommunity* community = (CPopupCommunity*)pThis;
		community->OnDeleteNote();
	}
}
void CPopupCommunity::ReceiveAllNote(INT32 offset)
{
	INT32 curValue = 0;
	INT32 i = 0;
	NOTE_RECEIVE* pNote = NULL;
	char szTemp[256];

	for( i=g_pGameContext->GetNoteReceiveCount() - 1 - offset; curValue<MAX_NOTE_SLOT; i--, curValue++)
	{
		if( i >= 0)
		{
			pNote = g_pGameContext->GetReceiveNote( i);

			m_noteSlot[curValue]._pCheck->SetEnable(TRUE);

			if( 0 < pNote->body._messageSize )
			{
				char * pDumpMessage = pNote->body._message;
				char Subject[NET_NOTE_SIZE] = {0,};
				INT32 SubjectIdx = 0;
				while(*pDumpMessage)
				{
					// 10�� NewLine���ڷν�, ȭ�鿡 �����ִ� �޼������� �ڸ��ϴ�.
					if( *pDumpMessage == 10) break;

					pDumpMessage++;
					SubjectIdx++;
				}

				i3mem::Copy( Subject, pNote->body._message, sizeof(char)*SubjectIdx);
	
                m_noteSlot[curValue]._pContent->SetTextEllipsis(Subject);
			}
			else
			{
				UINT8* pcSize		= (UINT8*)pNote->body._message;
				UINT8* pcMessazeType = (UINT8*)&pNote->body._message[1];

				char szMessage[ MAX_PATH ];
				szMessage[ 0 ] = '\0';

				switch( *pcMessazeType )
				{
					case NOTE_MESSAGE_CLAN_INVITE_ACCEPT:	// "�����մϴ�!\nŬ�� ������ ���εǾ����ϴ�.\n��ſ� Ŭ�� Ȱ���� ��⼼��."
					case NOTE_MESSAGE_CLAN_MASTER:			// "Ŭ�� ������ ������ ���ӵǾ����ϴ�.\nŬ�� �����Ͱ� �ǽ� ���� ���� �մϴ�."
					case NOTE_MESSAGE_CLAN_STAFF:			// "�����մϴ�.\nŬ�� �����Ͱ� �������� �Ӹ��Ͽ����ϴ�."
					case NOTE_MESSAGE_CLAN_REGULAR:			// "Ŭ�� �����Ͱ� �Ϲ�ȸ������ �Ӹ��Ͽ����ϴ�."
						i3String::Copy( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), MAX_PATH );
						break;
					case NOTE_MESSAGE_CLAN_INVITE:			// "[%s] Ŭ������ ����\nŬ�����κ��� ������ �ʴ�޾ҽ��ϴ�.\n���Կ� �����Ͻðڽ��ϱ�?"
					case NOTE_MESSAGE_CLAN_INVITE_DENIAL:	// "[%s]\nŬ�����κ��� ���� ��û��\n�ź� �Ǿ����ϴ�."
					case NOTE_MESSAGE_CLAN_DEPORTATION:		// "[%s]\nŬ������ ����Ǿ����ϴ�."
						sprintf_s( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), pNote->body._sender );
						break;
					case NOTE_MESSAGE_CLAN_JOIN_ACCEPT:		// "[%s] �Բ���\nŬ�� ���� �ʴ븦 �����Ͽ����ϴ�."
					case NOTE_MESSAGE_CLAN_JOIN_DENIAL:		// "[%s] �Բ���\nŬ�� ���� ��û�� �ź��Ͽ����ϴ�."
					case NOTE_MESSAGE_CLAN_SECESSION:		// "[%s] �Բ���\nŬ������ Ż���Ͽ����ϴ�."
						{
							char szName[ NET_NICK_NAME_SIZE ];
							i3String::NCopy( szName, &pNote->body._message[2], *pcSize - 1 );
                            sprintf_s( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), szName );
						}
						break;
				}

				m_noteSlot[curValue]._pContent->SetTextEllipsis( szMessage );
			}
			m_noteSlot[curValue]._pWho->SetTextEllipsis(pNote->body._sender);

			if( (pNote->header._id & NT_GIFT_FLAG) &&
				(pNote->header._remainDay == 0))	//	������ �������� ���ϴ� ���� (�ڵ����� ���߿� ������)
			{
				i3String::Copy( szTemp, "--", 256 );
			}
			else
			{
				sprintf_s( szTemp, GAME_STRING("STR_POPUP_WHAT_DAY"), pNote->header._remainDay);/*%d��*/
			}

			m_noteSlot[curValue]._pDate->SetText(szTemp);

			switch(pNote->header._state)
			{
			case NS_READED:
				m_noteSlot[curValue]._pContent->SetTextColor(GetColor(GCT_DEFAULT));
				m_noteSlot[curValue]._pWho->SetTextColor(GetColor(GCT_DEFAULT));
				m_noteSlot[curValue]._pDate->SetTextColor(GetColor(GCT_DEFAULT));
				break;
			case NS_UNREAD:
				m_noteSlot[curValue]._pContent->SetTextColor(GetColor(GCT_VALUE));
				m_noteSlot[curValue]._pWho->SetTextColor(GetColor(GCT_VALUE));
				m_noteSlot[curValue]._pDate->SetTextColor(GetColor(GCT_VALUE));
				break;
			case NS_NEW:
				m_noteSlot[curValue]._pContent->SetTextColor(GetColor(GCT_VALUE));
				m_noteSlot[curValue]._pWho->SetTextColor(GetColor(GCT_VALUE));
				m_noteSlot[curValue]._pDate->SetTextColor(GetColor(GCT_VALUE));
				break;
			}

			//	���� ������ ���� �����ǹǷ� ������ ���� ������ �� ����.
			if( pNote->header._type == NT_GIFT)
			{
				m_noteSlot[curValue]._pCheck->setCheckBoxState(FALSE);
				m_noteSlot[curValue]._pCheck->SetEnable(FALSE);
			}
			else
			{
				m_noteSlot[curValue]._pCheck->setCheckBoxState(m_bNoteCheck[i]);
				m_noteSlot[curValue]._pCheck->SetEnable(TRUE);
			}
		}
		else
		{
			m_noteSlot[curValue]._pCheck->SetEnable(FALSE);
			m_noteSlot[curValue]._pContent->SetTextEllipsis("");
			m_noteSlot[curValue]._pWho->SetTextEllipsis("");
			m_noteSlot[curValue]._pDate->SetText("");
		}
	}

	INT32 idxSlot = m_SelectedNote - m_pNoteList->GetCurrentTop();
	if( idxSlot >= 0 && idxSlot < MAX_NOTE_SLOT)
	{
		m_noteSlot[idxSlot]._pContent->SetTextColor(GetColor(GCT_NOTIFY));
		m_noteSlot[idxSlot]._pWho->SetTextColor(GetColor(GCT_NOTIFY));
		m_noteSlot[idxSlot]._pDate->SetTextColor(GetColor(GCT_NOTIFY));
	}
}

void CPopupCommunity::SetNoteCheck( INT32 idx)
{
	UINT32 TopIdx = m_pNoteList->GetCurrentTop();
	INT32 nListIdx = (g_pGameContext->GetNoteReceiveCount()-1) - idx - TopIdx;

	if( m_bNoteCheck[nListIdx])
	{
		m_bNoteCheck[nListIdx] = FALSE;
	}
	else
	{
		m_bNoteCheck[nListIdx] = TRUE;
	}
}

void CPopupCommunity::SetNoteType( NOTE_TYPE type)
{
	m_nNoteType = type;
	m_pNoteChild->SetEnable(TRUE);

	switch( m_nNoteType)
	{
	case NOTE_TYPE_SEND:
		m_pNoteWhoKey->SetText(GAME_STRING("STR_POPUP_RECEIVER"));/*�޴»��*/
		m_pNoteSend->SetCaption( GAME_STRING("STR_POPUP_SEND"));/*������*/
		m_pNoteWhoValue->setInputDisable( FALSE);
		m_pNoteValue->setInputDisable( FALSE);
		m_pNoteWhoValue->SetFocus( TRUE);
		m_pNoteValue->SetEnable( TRUE);

		m_pNoteGiftItemFrame->SetEnable(FALSE);
		m_pNoteGiftWeaponImage->SetEnable(FALSE);
		m_pNoteGiftCharacterImage->SetEnable(FALSE);
		m_pNoteGiftCashItemImage->SetEnable(FALSE);
		m_pNoteGiftValue[0]->SetEnable(FALSE);
		m_pNoteGiftValue[1]->SetEnable(FALSE);
		m_pNoteGiftSeparate->SetEnable(FALSE);
		break;
	case NOTE_TYPE_REPLY:
		m_pNoteWhoKey->SetText(GAME_STRING("STR_POPUP_SENDER_2"));/*�������*/
		m_pNoteSend->SetCaption( GAME_STRING("STR_POPUP_REPLY"));/*�����ϱ�*/
		m_pNoteWhoValue->setInputDisable( TRUE);
		m_pNoteValue->setInputDisable( TRUE);
		m_pNoteValue->SetEnable( TRUE);

		m_pNoteGiftItemFrame->SetEnable(FALSE);
		m_pNoteGiftWeaponImage->SetEnable(FALSE);
		m_pNoteGiftCharacterImage->SetEnable(FALSE);
		m_pNoteGiftCashItemImage->SetEnable(FALSE);
		m_pNoteGiftValue[0]->SetEnable(FALSE);
		m_pNoteGiftValue[1]->SetEnable(FALSE);
		m_pNoteGiftSeparate->SetEnable(FALSE);
		break;
	case NOTE_TYPE_GIFT:
		m_pNoteWhoKey->SetText(GAME_STRING("STR_POPUP_SENDER_2"));/*�������*/
		m_pNoteSend->SetCaption( GAME_STRING("STR_POPUP_REPLY"));/*�����ϱ�*/
		m_pNoteWhoValue->setInputDisable( TRUE);
		m_pNoteValue->setInputDisable( TRUE);
		m_pNoteValue->SetEnable( FALSE);

		m_pNoteGiftItemFrame->SetEnable(TRUE);
		m_pNoteGiftWeaponImage->SetEnable(FALSE);
		m_pNoteGiftCharacterImage->SetEnable(FALSE);
		m_pNoteGiftCashItemImage->SetEnable(FALSE);

		FLUSH_CONTROL(m_pNoteGiftItemFrame);
		FLUSH_CONTROL(m_pNoteGiftWeaponImage);
		FLUSH_CONTROL(m_pNoteGiftCharacterImage);
		FLUSH_CONTROL(m_pNoteGiftCashItemImage);

		m_pNoteGiftValue[0]->SetEnable(TRUE);
		m_pNoteGiftValue[1]->SetEnable(TRUE);
		m_pNoteGiftSeparate->SetEnable(TRUE);
		break;
	}
}

BOOL CPopupCommunity::_SetNormalNote( NOTE_RECEIVE* pNote)
{
	SetNoteType( NOTE_TYPE_REPLY);

	m_pNoteValue->SetEnable(TRUE);
	m_pNoteValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);

	if( 0 < pNote->body._messageSize )
		m_pNoteValue->SetIMEText(pNote->body._message);
	else
	{
		UINT8* pcSize		= (UINT8*)pNote->body._message;
		UINT8* pcMessazeType = (UINT8*)&pNote->body._message[1];

		char szMessage[ MAX_PATH ];
		szMessage[ 0 ] = '\0';

		switch( *pcMessazeType )
		{
			case NOTE_MESSAGE_CLAN_INVITE_ACCEPT:	// "�����մϴ�!\nŬ�� ������ ���εǾ����ϴ�.\n��ſ� Ŭ�� Ȱ���� ��⼼��."
			case NOTE_MESSAGE_CLAN_MASTER:			// "Ŭ�� ������ ������ ���ӵǾ����ϴ�.\nŬ�� �����Ͱ� �ǽ� ���� ���� �մϴ�."
			case NOTE_MESSAGE_CLAN_STAFF:			// "�����մϴ�.\nŬ�� �����Ͱ� �������� �Ӹ��Ͽ����ϴ�."
			case NOTE_MESSAGE_CLAN_REGULAR:			// "Ŭ�� �����Ͱ� �Ϲ�ȸ������ �Ӹ��Ͽ����ϴ�."
				i3String::Copy( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), MAX_PATH );
				break;
			case NOTE_MESSAGE_CLAN_INVITE:			// "[%s] Ŭ������ ����\nŬ�����κ��� ������ �ʴ�޾ҽ��ϴ�.\n���Կ� �����Ͻðڽ��ϱ�?"
			case NOTE_MESSAGE_CLAN_INVITE_DENIAL:	// "[%s]\nŬ�����κ��� ���� ��û��\n�ź� �Ǿ����ϴ�."
			case NOTE_MESSAGE_CLAN_DEPORTATION:		// "[%s]\nŬ������ ����Ǿ����ϴ�."
				sprintf_s( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), pNote->body._sender );
				break;
			case NOTE_MESSAGE_CLAN_JOIN_ACCEPT:		// "[%s] �Բ���\nŬ�� ���� �ʴ븦 �����Ͽ����ϴ�."
			case NOTE_MESSAGE_CLAN_JOIN_DENIAL:		// "[%s] �Բ���\nŬ�� ���� ��û�� �ź��Ͽ����ϴ�."
			case NOTE_MESSAGE_CLAN_SECESSION:		// "[%s] �Բ���\nŬ������ Ż���Ͽ����ϴ�."
				{
					char szName[ NET_NICK_NAME_SIZE ];
					i3String::NCopy( szName, &pNote->body._message[2], *pcSize - 1 );
                    sprintf_s( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), szName );
				}
				break;
		}

		m_pNoteValue->SetIMEText( szMessage );
	}

	return TRUE;
}

BOOL CPopupCommunity::_SetGiftNote( NOTE_RECEIVE* pNote)
{
	char szWeaponName[ MAX_STRING_COUNT];

	//	���� ������ ǥ��
	if(pNote->header._type != NT_GIFT)
		return FALSE;

	INT32 iType = AUTH_UNKNWON;
	char message[1024] = "";
	i3String::Copy( message, GAME_STRING("STR_POPUP_UNKNOWN_GIFT"), 1024 );	/*�� �� ���� ��ǰ�Դϴ�.*/

	//	�����ϱ� Note���� pNote->body._user �����ʹ� GIFT_BUFFER ����ü ��������.
	GIFT_BUFFER * pInfo = (GIFT_BUFFER*) pNote->body._user;

	INT32 authTerm = 0, authCount = 1;
	BOOL IsPackageGoods = FALSE;


	// GIFT_BUFFER�� _GoodsID�� 32��Ʈ�� �ٲ������ Ȯ���Ͽ��� �մϴ�.
	//I3ASSERT_0;

	UINT32 GoodsID = pInfo->_ui32GoodsID;
	CShop* shop = g_pGameContext->GetShop();
	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(pInfo->_ui32GoodsID);
	if ( pGoodsList )
	{
		if (1 < pGoodsList->_ui8ItemCount)
		{
			IsPackageGoods = TRUE;
		}
		else
		{//authArg
			iType = pGoodsList->_ppItemData[0]->_ui8AuthType;

			if ( AUTH_COUNT == iType )
			{
				authCount = pGoodsList->_pui32ItemArg[0];
			}
			else if ( AUTH_SEC == iType )
			{
				authTerm = pGoodsList->_pui32ItemArg[0];
			}
		}
	}


	SetNoteType( NOTE_TYPE_GIFT);

	if (IsPackageGoods)
	{

		BOOL isPeriod = FALSE;
		INT32 periodItemIdx = 0;
		INT32 notPointItemIdx = 0;

		INT32 ItemCount = pGoodsList->_ui8ItemCount;
		for(INT32 j = 0; j < ItemCount; j++)
		{
			if (AUTH_SEC == pGoodsList->_ppItemData[j]->_ui8AuthType)
			{
				isPeriod = TRUE;
				periodItemIdx = j;
				break;
			}
			else if (ITEM_TYPE_POINT != GET_ITEM_FLAG_TYPE(pGoodsList->_ppItemData[j]->_ui32ItemID))
			{
				notPointItemIdx = j;
			}
		}


		if (isPeriod)
		{
	
			UINT32 seconds = pGoodsList->_pui32ItemArg[periodItemIdx];
			UINT32 day = ConvertSecToUseDay(seconds);

			ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
			I3ASSERT( pInfo != NULL);

			sprintf_s( message, GAME_STRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), pInfo->GetScriptName(), day);/*�̸�: %s\n���Ⱓ: %d��*/
		}
		else
		{
			INT32 ItemID = pGoodsList->_ppItemData[periodItemIdx]->_ui32ItemID;
			INT32 itemUsage = (ITEM_TYPE) GET_ITEM_FLAG_TYPE(ItemID);

			//	���� ������ Goods���� pGoods->_AuthArg�� ��� ������ �ֱ� ������ ���� ó���� �ʿ� komet
			switch( (ITEM_TYPE) itemUsage)
			{
				//	���Ӽ�
			case ITEM_TYPE_MAINTENANCE:
				{
					//	getInfo�� keyItem���� �����;� ��Ȯ�� info�� ���� �� �ִ�. (�׷��������� NULL�� �� �ִ�.)
					ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
					I3ASSERT( pInfo != NULL);

					UINT32 Day = pGoodsList->_pui32ItemArg[periodItemIdx];
					sprintf_s( message, GAME_STRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), pInfo->GetScriptName(), Day);/*�̸�: %s\n���Ⱓ: %d��*/
				}
				break;
				//	�Ҹ�
			case ITEM_TYPE_EXPENDABLE:
				{
					UINT32 count = pGoodsList->_pui32ItemArg[periodItemIdx];

					//	getInfo�� keyItem���� �����;� ��Ȯ�� info�� ���� �� �ִ�. (�׷��������� NULL�� �� �ִ�.)
					ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
					I3ASSERT( pInfo != NULL);

					sprintf_s( message, GAME_STRING("STR_POPUP_EXPENDABLES_GIFT_NAME"), pInfo->GetScriptName(), count);/*�̸�: %s\n��밳��: %d��*/;
				}
				break;
			default:
				sprintf_s( message, UNKNOWN_TEXT);
				break;
			}
		}


		g_pFramework->SetPackageGoodsShape(m_pNoteGiftCashItemImage, GoodsID);

		REAL32 posX = m_pNoteGiftCashItemImage->getPositionX();
		REAL32 posY = m_pNoteGiftCashItemImage->getPositionY();

		posX += 24.0f;
		posY -= 14.0f;

		m_pNoteGiftCashItemImage->setPosition(posX, posY);

		m_PackageGoodsID = GoodsID;

	}
	else
	{
		I3ASSERT(pGoodsList != NULL);
		UINT32 ItemID = pGoodsList->_ppItemData[0]->_ui32ItemID;
		INT32 authDay = (INT32) ConvertSecToUseDay((UINT32)authTerm);	//	�����Ⱓ (��)�� ��ȯ

		//	��ǰ ������ ���� �з�
		ITEM_TYPE itemClass = (ITEM_TYPE)GET_ITEM_FLAG_TYPE(ItemID);
		switch(itemClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				m_pNoteGiftWeaponImage->SetEnable(TRUE);
				FLUSH_CONTROL(m_pNoteGiftWeaponImage);

				WEAPON_CLASS_TYPE type = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(ItemID);
				INT32 number = GET_ITEM_FLAG_NUMBER(ItemID);

				GUI_WEAPON_VALID( type, number);

				CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( type, number);

				I3ASSERT( iType != AUTH_UNKNWON );
				switch( iType )
				{
				case AUTH_COUNT:
					{
						pInfo->GetWeaponInfoName( szWeaponName);
						sprintf_s( message, GAME_STRING("STR_POPUP_EXPENDABLES_GIFT_NAME"), szWeaponName, authCount);/*�̸�: %s\n��밳��: %d��*/
					}
					break;
				case AUTH_SEC:
					{
						pInfo->GetWeaponInfoName( szWeaponName);
						sprintf_s( message, GAME_STRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), szWeaponName, authDay);/*�̸�: %s\n���Ⱓ: %d��*/
					}
					break;
				}

				g_pFramework->SetWeaponSelectMark(m_pNoteGiftWeaponImage, (UINT8)type , static_cast<UINT8>(number));
			}
			break;
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
			{
				m_pNoteGiftCharacterImage->SetEnable(TRUE);
				FLUSH_CONTROL(m_pNoteGiftCharacterImage);

				g_pFramework->SetCharacterShape(m_pNoteGiftCharacterImage, ItemID);

				char name[MAX_STRING_COUNT] = "";
				g_pCharaInfoDataBase->GetCharacterName(ItemID, name, sizeof(name));

				I3ASSERT( iType != AUTH_UNKNWON );
				switch( iType )
				{
				case AUTH_COUNT:
					{
						sprintf_s( message, GAME_STRING("STR_POPUP_EXPENDABLES_GIFT_NAME"), name, authCount);/*�̸�: %s\n��밳��: %d��*/
					}
					break;
				case AUTH_SEC:
					{
						sprintf_s( message, GAME_STRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), name, authDay);/*�̸�: %s\n���Ⱓ: %d��*/
					}
					break;
				}
			}
			break;
			//	�������� ���� ��ǰ�� ��� 1��, 7��, 30�� ¥�� �Ⱓ���� �������� ��޵Ǳ� ������ ��Ȯ�� �Ⱓ��, ��������
			//	�Ǵ��Ϸ��� ITEM_TYPE�� ������� ���� CASHIITEM_USAGE�� ����ؾ� �Ѵ�.
			//	���������� ���� �������� ��� �Ⱓ���� �������� �ǸŵǱ� ����. komet
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
			{
				INT32 itemUsage = GET_ITEM_FLAG_TYPE(ItemID);

				m_pNoteGiftCashItemImage->setPosition(59.0f, 11.0f);
				REAL32 widthRate = i3GuiRoot::getGuiResolutionWidth();
				REAL32 heightRate = i3GuiRoot::getGuiResolutionHeight();
				m_pNoteGiftCashItemImage->SetControlSize((INT32)(90 * widthRate), (INT32)(65 * heightRate));

				m_PackageGoodsID = 0;
				m_pNoteGiftCashItemImage->SetEnable(TRUE);
				FLUSH_CONTROL(m_pNoteGiftCashItemImage);

				g_pFramework->SetCashItemShape(m_pNoteGiftCashItemImage, ItemID);

				//	getInfo�� keyItem���� �����;� ��Ȯ�� info�� ���� �� �ִ�. (�׷��������� NULL�� �� �ִ�.)
				char Name[MAX_STRING_COUNT] = {};
				g_pShopItemInfoDataBase->GetItemName(ItemID, Name, MAX_STRING_COUNT);

				I3ASSERT( iType != AUTH_UNKNWON );
				switch( iType )
				{
				case AUTH_SEC:
					{
						sprintf_s( message, GAME_STRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), Name, authDay);/*�̸�: %s\n���Ⱓ: %d��*/
					}
					break;
				case AUTH_COUNT:
					{
						char szName[512] = "";

						//	���������� �������� �Ѿ������ ������ �Ⱓ�� �������� ��� ������ �̸� �ڿ� �Ⱓ�� ���δ�. komet
						//	(��: ����ġ 130% 7�ϱ� -> ����: 1��)
						//	ĳ�� ������ ������ �з��� ITEM_TYPE�� ������� ���� CASHIITEM_USAGE�� ����ؾ� �Ѵ�. komet
						if( itemUsage == ITEM_TYPE_MAINTENANCE)
						{
							INT32 nTerm = pGoodsList->_pui32ItemArg[0];
							if( nTerm > 0)
							{
								UINT32 Day = ConvertSecToUseDay(nTerm);
								sprintf_s(szName, GAME_STRING("STR_INVEN_CASH_ITEM_TERM"), Name, Day);/*%s %d�ϱ�*/
							}
							else
							{
								sprintf_s(szName, "%s", Name);
							}
						}
						else
						{
							sprintf_s(szName, "%s", Name);
						}

						sprintf_s( message, GAME_STRING("STR_POPUP_EXPENDABLES_GIFT_NAME"), szName, authCount);/*�̸�: %s\n��밳��: %d��*/
					}
					break;
				}

			}
		case ITEM_TYPE_TEMP:
			{
			}
			break;
		}
	}

	//	�޽���
	m_pNoteGiftValue[0]->SetText( message);
	m_pNoteGiftValue[1]->SetText( pNote->body._message);

	return TRUE;

}

void CPopupCommunity::SetSelectedNote( INT32 idx)
{
	if( g_pGameContext->GetNoteReceiveCount() <= idx) return;

	m_SelectedNoteIndex = idx;

	UINT8 ReadCount = 0;
	UINT32 TopIdx = m_pNoteList->GetCurrentTop();

	UINT32 idxSlot = idx - m_pNoteList->GetCurrentTop();
	m_pNoteSelect->setPositionY(29.0f + (idxSlot * 24.0f));
	m_pNoteSelect->SetEnable(TRUE);

	//
	PlaySound( GUI_SND_ITEM_SELECTING);

	ReceiveAllNote(TopIdx);

	INT32 nListIdx = g_pGameContext->GetNoteReceiveCount() - (idx+1)-TopIdx;

	m_SelectedNote = g_pGameContext->GetNoteReceiveCount() - (nListIdx+1);

	NOTE_RECEIVE* pNote = NULL;
	pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( nListIdx);

	m_pNoteWhoValue->SetIMEText(pNote->body._sender);

	// ���� �޽���
	m_iListIdx = nListIdx;
	m_pNoteAccept->SetEnable(FALSE);
	m_pNoteReject->SetEnable(FALSE);
	m_pNoteSend->SetEnable(FALSE);

	m_pNoteGiftItemFrame->SetEnable(FALSE);
	m_pNoteGiftWeaponImage->SetEnable(FALSE);
	m_pNoteGiftCharacterImage->SetEnable(FALSE);
	m_pNoteGiftCashItemImage->SetEnable(FALSE);
	m_pNoteGiftSeparate->SetEnable(FALSE);

	m_pNoteGiftValue[0]->SetEnable(FALSE);
	m_pNoteGiftValue[1]->SetEnable(FALSE);
	m_pNoteValue->SetEnable(FALSE);

	//	GUI ó��
	{
		switch( pNote->header._type)
		{
		case NT_NORMAL:
			{
				m_pNoteSend->SetEnable(TRUE);
				_SetNormalNote(pNote);
			}
			break;
		case NT_GIFT:
			{
				m_pNoteSend->SetEnable(TRUE);
				_SetGiftNote(pNote);
			}
			break;
		case NT_WEB:
		case NT_NOTICE:
			{
				_SetNormalNote(pNote);
			}
			break;
		case NT_CLAN:
			{
				_SetNormalNote(pNote);

				m_pNoteWhoKey->SetText(GAME_STRING("STR_POPUP_CLAN_SENDER"));/*����Ŭ��*/
			}
			break;
		case NT_CLAN_ASK:
			{
				m_pNoteAccept->SetEnable(TRUE);
				m_pNoteReject->SetEnable(TRUE);

				if( pNote->header._state != NS_READED )
				{
					ACTIVATE_BUTTON( m_pNoteAccept );
					ACTIVATE_BUTTON( m_pNoteReject );
				}
				else
				{
					DEACTIVATE_BUTTON( m_pNoteAccept );
					DEACTIVATE_BUTTON( m_pNoteReject );
				}

				_SetNormalNote(pNote);
			}
			break;
		default:
			{
				m_pNoteValue->SetEnable(TRUE);
				m_pNoteValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
				m_pNoteValue->SetIMEText(GAME_STRING("STR_POPUP_UNKNOWN_NOTE"));/*�� �� ���� �����Դϴ�.*/

				SetNoteType( NOTE_TYPE_REPLY);
			}
			break;
		}
	}

	//	������ �̺�Ʈ ó��
	{
		switch( pNote->header._type)
		{
		case NT_GIFT:
			{
				if( pNote->header._state != NS_READED)
				{
					//	���������� ���� ������ �ִ�.
					GIFT_BUFFER * pInfo = (GIFT_BUFFER*) pNote->body._user;
					I3ASSERT( pInfo->_ui32GoodsID != 0);
					//I3ASSERT( pInfo->_giftId != 0); // ������� ����.

					// �������·� �����մϴ�.
					pNote->header._state = NS_READED;
	
					//	������ ��ǰ�� ������ ������ ���ÿ� �κ��� ����ǵ��� ��.
					//g_pGameContext->SetEvent( EVENT_CHECK_GIFT_GOODS, &pInfo->_i64GiftWareDBIdx, &pInfo->_ui32GoodsID );
					g_pGameContext->SetEvent( EVENT_CHECK_GIFT_GOODS, &pInfo->_i32GiftWareDBIdx );	// GoodsID�� ������ �ʽ��ϴ�.
				}
			}
			break;
		case NT_CLAN_ASK:
			//	���� ��û ���� �޽����� ������ �������� �ʴ´�. ���߿� �亯�Ҷ� ����.
			break;
		case NT_NORMAL:
		case NT_NOTICE:
		case NT_WEB:
		case NT_CLAN:
			{
				if(pNote->header._state != NS_READED)
				{
				// �������·� �����մϴ�.
					pNote->header._state = NS_READED;

					// ���� ������ ������ �����Ⱓ�� 7�Ϸ� �����մϴ�.
					pNote->header._remainDay = 7;

					ReadCount++;
					g_pGameContext->SetEvent( EVENT_CHECK_READED_NOTE, &ReadCount, &pNote->header._id);
				}
			}
			break;
		}
	}

	//
	g_pGameContext->AddGameFlag( GAMEFLAG_NOTEREFRESH);
	_UpdateNote();
}

void CPopupCommunity::ResetNote(void)
{
	m_pNoteChild->SetEnable(FALSE);

	for(UINT32 i = 0; i < MAX_NOTE_SLOT; i++)
	{
		m_noteSlot[i]._pCheck->setCheckBoxState( FALSE);
	}

	for(UINT32 i = 0; i < MAX_NOTE_COUNT; i++) m_bNoteCheck[i] = FALSE;

	OnContactTab();
}

void CPopupCommunity::_InitCommon(void)
{
	m_pCommunityGui->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pCommunityCaption, GAME_STRING("STR_POPUP_COMMUNITY"));/*Ŀ�´�Ƽ*/

	INIT_TAB_EX(m_pContactTab);
	INIT_TAB_EX(m_pNoteTab);
	INIT_CAPTION_BUTTON(m_pLeave, GAME_STRING("STR_POPUP_LEAVE"));/*������*/

	m_pContactTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);


	m_pNicknameWin->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pNicknameCaption, GAME_STRING("STR_POPUP_ADD_FRIEND"));/*ģ���߰�*/
	INIT_CAPTION_BUTTON(m_pNicknameConfirm, GAME_STRING("STR_POPUP_OK"));/*Ȯ��*/
	INIT_CAPTION_BUTTON(m_pNicknameCancel, GAME_STRING("STR_POPUP_CANCEL"));/*���*/

	m_pNicknameMessage->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNicknameMessage->SetTextColor(GetColor(GCT_DEFAULT));

	CREATE_IME_ELEMENT ime;

	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;

	m_pNicknameInput->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pNicknameInput->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNicknameInput->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNicknameInput->SetText("");


	// dummy
	m_pNoteGageBg->SetEnable(FALSE);
}

void CPopupCommunity::_InitContactSect(void)
{
	// ���� ����� ��ϼ��� �������� ��Ȱ��ȭ ����
	// ���ڸ��� Ŭ������ �̵��մϴ�.
	{
		m_pBlockTab->SetEnable( FALSE);
		m_pClanTab->setPosition( m_pBlockTab->getPositionX(), m_pBlockTab->getPositionY());
	}

	INIT_CAPTION_TAB(m_pFriendTab, GAME_STRING("STR_POPUP_FRIEND"));/*ģ��*/
	INIT_CAPTION_TAB(m_pBlockTab, GAME_STRING("STR_POPUP_BLOCK"));/*����*/
    INIT_CAPTION_TAB(m_pClanTab, GAME_STRING("STR_POPUP_CLAN"));/*Ŭ��*/
	m_pFriendTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_currentTab = COMMUNITY_FRIEND_TAB;

	INIT_CAPTION_BUTTON(m_pFind, GAME_STRING("STR_POPUP_FIND_USER"));
	INIT_CAPTION_BUTTON(m_pAdd, GAME_STRING("STR_POPUP_ADD_FRIEND"));
//	INIT_BUTTON_EX(m_pDelete);

	m_pContactListName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pContactListName->SetTextColor(GetColor(GCT_DEFAULT));
	m_pContactListName->SetText(GAME_STRING("STR_POPUP_NAME"));/*�̸�*/

	m_pContactListState->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pContactListState->SetTextColor(GetColor(GCT_DEFAULT));
	m_pContactListState->SetText(GAME_STRING("STR_POPUP_STATE"));/*����*/

	for(UINT32 i = 0; i < MAX_SLOT_FOR_CONTACT_LIST; i++)
	{
		GameGUI::SetRankStatic(m_contactSlot[i]._pMark);
		m_contactSlot[i]._pMark->SetEnable(FALSE);
		m_contactSlot[i]._pNick->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_contactSlot[i]._pNick->SetOffsetPos(3.0f, 0.0f);
		m_contactSlot[i]._pNick->SetTextColor(GetColor(GCT_DEFAULT));
		m_contactSlot[i]._pState->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_contactSlot[i]._pState->SetTextColor(GetColor(GCT_DEFAULT));
	}

	m_pContactList->CreateTextEx(MAX_SLOT_FOR_CONTACT_LIST,  GetDefaultFontName());
	m_pContactList->SetTextColor(GetColor(GCT_DEFAULT));
	m_pContactList->SetSelColor(0, 0, 0, 0);

	m_pContactSelect->SetEnable(FALSE);
	m_pContactFocus->SetEnable(FALSE);

//	m_pDelete->setControlDisable(TRUE);
	g_pCommunity->SetCommunityUserInfo(FALSE);
}

void CPopupCommunity::_InitContactChild(void)
{
	m_pContactChild->SetEnable(FALSE);

	g_pCommunity->SetCommunityUserInfo(FALSE);

	INIT_CAPTION(m_pContactKey, GAME_STRING("STR_POPUP_DETAIL_INFO"));/*������*/

#if 0
	INIT_CAPTION_BUTTON(m_pProfile, GAME_STRING("STR_POPUP_LOOK_PRIVATE_ROOM"));/*������ ����*/
	INIT_CAPTION_BUTTON(m_pFriend, GAME_STRING("STR_POPUP_ADD_FRIEND"));/*ģ���߰�*/
#else
	// dummy
	INIT_CAPTION_BUTTON(m_pProfile, GAME_STRING("STR_POPUP_ACCEPT_FRIEND"));/*ģ������*/
	INIT_CAPTION_BUTTON(m_pFriend, GAME_STRING("STR_POPUP_VISIT"));/*ã�ư���*/
#endif
	INIT_CAPTION_BUTTON(m_pInvite, GAME_STRING("STR_POPUP_INVITE"));/*�ʴ��ϱ�*/
	INIT_CAPTION_BUTTON(m_pContactClose, GAME_STRING("STR_POPUP_CLOSE"));/*�ݱ�*/
	INIT_CAPTION_BUTTON(m_pFriendRefuse, GAME_STRING("STR_POPUP_FRIEND_REFUSE"));/*ģ������*/

	DEACTIVATE_BUTTON(m_pProfile);
	DEACTIVATE_BUTTON(m_pFriend);
	DEACTIVATE_BUTTON(m_pInvite);
	DEACTIVATE_BUTTON(m_pContactClose);
	DEACTIVATE_BUTTON(m_pFriendRefuse);

	#ifndef USE_MODE_RECORD
	ACTIVATE_BUTTON(m_pProfile);
	ACTIVATE_BUTTON(m_pFriend);
	ACTIVATE_BUTTON(m_pInvite);
	ACTIVATE_BUTTON(m_pContactClose);
	ACTIVATE_BUTTON(m_pFriendRefuse);
	#endif

	m_pFriendRefuse->SetEnable(FALSE);

	//	���� ��
	INIT_CAPTION_TAB(m_pRecordTab[0], GAME_STRING("STR_POPUP_CLAN"));/*Ŭ��*/
	INIT_CAPTION_TAB(m_pRecordTab[1], GAME_STRING("STR_POPUP_PRIVATE"));/*����*/

	for(INT32 i=0; i<2; i++)
		m_pRecordTab[i]->SetEnable(FALSE);

	m_pNicknameKey->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_BOLD);
	m_pSeasonRecordKey->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_BOLD);
	m_pTotalRecordKey->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_BOLD);
	m_pPositionKey->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_BOLD);
	m_pNicknameKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonRecordKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalRecordKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPositionKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNicknameKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSeasonRecordKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTotalRecordKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pPositionKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNicknameKey->SetText(GAME_STRING("STR_POPUP_NICKNAME"));/*�г���*/
	m_pSeasonRecordKey->SetText(GAME_STRING("STR_POPUP_SEASON_HISTORY"));/*��������*/
	m_pTotalRecordKey->SetText(GAME_STRING("STR_POPUP_TOTAL_HISTORY"));/*�������*/
	m_pPositionKey->SetText(GAME_STRING("STR_POPUP_CUR_POSITION"));/*������ġ*/

	m_pNicknameValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonRecord->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonKillDeath->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonHeadshot->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalRecord->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalKillDeath->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalHeadshot->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPositionValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
}

void CPopupCommunity::_InitUserDetailInfo(void)
{
	m_pUserDetailInfoPopup->SetEnable(FALSE);
	g_pCommunity->SetCommunityUserInfo(FALSE);

	INIT_WIN_CAPTION( m_pCaption, GAME_STRING("STR_POPUP_DETAIL_INFO"));	//"������"

	// UserDetailInfoPopupFrame
	// User Info
	GameGUI::SetRankStatic(m_pUserDetailInfoRankMark);
	m_pUserDetailInfoRankMark->SetEnable(FALSE);	
	INIT_VALUE_CAPTION( m_pUserDetailInfoNicknameValue, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoRank, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoClanName, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoTitle, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoPoint, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoToll, "");

	INIT_STATIC_CAPTION( m_pUserDetailInfoPositionKey, GAME_STRING("STR_POPUP_CUR_POSITION"));	//	"������ġ"
	INIT_VALUE_CAPTION( m_pUserDetailInfoPositionValue, "");

	// UserDetailInfoPopupFrame2
	// Total Record	
	INIT_STATIC_CAPTION( m_pUserDetailInfoTotalRecordKey, GAME_STRING("STR_POPUP_TOTAL_HISTORY"));	//	"�������"
	INIT_VALUE_CAPTION( m_pUserDetailInfoTotalRecord, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoTotalKillDeath, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoTotalHeadshot, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoTotalDis, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoTotalUserRanking, "");

	// Season Record
	INIT_STATIC_CAPTION( m_pUserDetailInfoSeasonRecordKey, GAME_STRING("STR_POPUP_SEASON_HISTORY"));	//	"��������"
	INIT_VALUE_CAPTION( m_pUserDetailInfoSeasonRecord, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoSeasonKillDeath, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoSeasonHeadshot, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoSeasonDis, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoSeasonUserRanking, "");

	// UserDetailInfoPopupFrame3	
	INIT_CAPTION_TAB(m_pModeRecordTab[0], GAME_STRING("STR_POPUP_TOTAL_HISTORY"));
	INIT_CAPTION_TAB(m_pModeRecordTab[1], GAME_STRING("STR_POPUP_SEASON_HISTORY"));
	m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_bRecordTabFlag = FALSE;

	INIT_STATIC_CAPTION(m_pModeTypeStatic, "");
	INIT_WIN_CAPTION(m_pModeTypeCombo, GAME_STRING("STBL_IDX_CLAN_ALL_MEMBER"));/*��ü*/
	INIT_COMBO_EX(m_pModeTypeButton);

	//	�� ���� �˾�
	m_pModeTypePopup->CreateTextEx(STAGE_TYPE_ANNIHILATION,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pModeTypePopup->SetSelColor(100, 100, 100, 100);
	m_pModeTypePopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pModeTypePopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pModeTypePopup->SetExclusiveEnableControl(TRUE);

	for(int i = 0; i < MODE_RECORD_MAX; i++)
	{
		switch(i)
		{
		case MODE_DEATHMATCH:
			{
				m_pModeTypePopup->AddElement(GAME_STRING("STR_HEADER_MODE_TEAM_DEATH_MATCH"));/*��ü��*/	//	break;
			}
			break;
		case MODE_ANNIHILATION:
			{
				m_pModeTypePopup->AddElement(GAME_STRING("STR_HEADER_MODE_ANNIHILATION"));/*������*/	//	break;
			}
			break;
		case MODE_BOMB:
			{
				m_pModeTypePopup->AddElement(GAME_STRING("STR_HEADER_MODE_EXPLOSION"));/*���Ĺ̼�*/	//break;
			}
			break;
		case MODE_ETC:
			{
				//m_pModeTypePopup->AddElement(GAME_STRING("STR_HEADER_MODE_DESTROY"));/*�ı��̼�*/	break;
				//m_pModeTypePopup->AddElement(GAME_STRING("STR_HEADER_MODE_BOSS"));/*�����̼�*/	break;
				m_pModeTypePopup->AddElement("etc");/*��Ÿ*///	break;
			}
			break;
		}
	}

	m_nModeTypeComboIdx = 0;
	m_pModeTypePopup->SetSelectedElement(m_nModeTypeComboIdx, FALSE);
	m_pModeTypePopup->SetEnable(FALSE);

	INIT_VALUE_CAPTION( m_pModeRecordValue,"");
	INIT_VALUE_CAPTION( m_pModeKillDeath,  "");
	INIT_VALUE_CAPTION( m_pModeHeadshot,   "");
	INIT_VALUE_CAPTION( m_pModeHitPercent, "");

	// [Button]
	// Button
#if 0
	INIT_CAPTION_BUTTON(m_pUserDetailInfoProfile,		GAME_STRING("STR_POPUP_LOOK_PRIVATE_ROOM"));/*������ ����*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoFriend,		GAME_STRING("STR_POPUP_ADD_FRIEND"));		/*ģ���߰�*/
#else
	// dummy
	INIT_CAPTION_BUTTON(m_pUserDetailInfoProfile,		GAME_STRING("STR_POPUP_ACCEPT_FRIEND"));							/*ģ������*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoFriend,		GAME_STRING("STR_POPUP_VISIT"));									/*ã�ư���*/
#endif
	INIT_CAPTION_BUTTON(m_pUserDetailInfoInvite,		GAME_STRING("STR_POPUP_INVITE"));									/*�ʴ��ϱ�*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoDetail,		GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN_VIEW"));		/*��Ȯ�� ��������*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoContactClose,	GAME_STRING("STR_POPUP_CLOSE"));									/*�ݱ�*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoFriendRefuse,	GAME_STRING("STR_POPUP_FRIEND_REFUSE"));							/*ģ������*/

	DEACTIVATE_BUTTON(m_pUserDetailInfoProfile);
	DEACTIVATE_BUTTON(m_pUserDetailInfoFriend);
	DEACTIVATE_BUTTON(m_pUserDetailInfoInvite);
	DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
	DEACTIVATE_BUTTON(m_pUserDetailInfoContactClose);
	DEACTIVATE_BUTTON(m_pUserDetailInfoFriendRefuse);

	#ifdef USE_MODE_RECORD
	ACTIVATE_BUTTON(m_pUserDetailInfoProfile);
	ACTIVATE_BUTTON(m_pUserDetailInfoFriend);
	ACTIVATE_BUTTON(m_pUserDetailInfoInvite);
	ACTIVATE_BUTTON(m_pUserDetailInfoDetail);
	ACTIVATE_BUTTON(m_pUserDetailInfoContactClose);
	ACTIVATE_BUTTON(m_pUserDetailInfoFriendRefuse);
	#endif

	m_pUserDetailInfoPositionKey->SetEnable( FALSE);
	m_pUserDetailInfoPositionValue->SetEnable( FALSE);

	m_bUserDetailInfoFlag = FALSE;	

	// UserDetailInfoPopupMessageBox
	INIT_WIN_CAPTION(m_pUserDetailInfoMsgCaption, GAME_STRING("STR_TBL_PROFILE_NOTICE_TEXT"));			//�˸� /*���������� �����̿�� ����â*/	
	char Msg[MAX_STRING_COUNT];
	Msg[0] = NULL;
	#ifdef USE_MODE_RECORD
	sprintf_s(Msg, GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_BILLING"), SUBTRACTE_POINT);	
	#endif
	INIT_VALUE_CAPTION( m_pUserDetailInfoMsgMessage,Msg);		/*������ �������� �������� SUBTRACTE_POINT ����Ʈ�� �����˴ϴ�. Ȯ���Ͻðڽ��ϱ�?*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoMsgConfirm, GAME_STRING("STR_POPUP_OK"));/*Ȯ��*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoMsgCancel, GAME_STRING("STR_POPUP_CANCEL"));/*���*/
	m_pUserDetailInfoMsgMessage->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pUserDetailInfoMsgMessage->SetTextColor(GetColor(GCT_DEFAULT));

	m_pUserDetailInfoMsg->SetEnable(FALSE);
}

void CPopupCommunity::_InitNoteSect(void)
{
	m_pNoteSect->SetEnable(FALSE);

	INIT_CAPTION_TAB(m_pNoteListTab, GAME_STRING("STR_POPUP_NOTE"));/*����*/
	INIT_BUTTON_EX(m_pNoteDelete);
	INIT_CAPTION_BUTTON(m_pNoteWrite, GAME_STRING("STR_POPUP_WRITE_NOTE"));/*��������*/
	m_pNoteListTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_pNoteListContent->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteListWho->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteListDate->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteListContent->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNoteListWho->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNoteListDate->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNoteListContent->SetText(GAME_STRING("STR_POPUP_CONTENT"));/*����*/
	m_pNoteListWho->SetText(GAME_STRING("STR_POPUP_SENDER_1"));/*������*/
	m_pNoteListDate->SetText(GAME_STRING("STR_POPUP_REMAIN_DAY"));/*������¥*/

	for(UINT32 i = 0; i < MAX_NOTE_SLOT; i++)
	{
		m_noteSlot[i]._pCheck->setCheckBoxState(FALSE);
		m_noteSlot[i]._pCheck->setInputDisable(FALSE);
		m_noteSlot[i]._pCheck->SetEnable( FALSE);
		m_noteSlot[i]._pContent->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_noteSlot[i]._pWho->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_noteSlot[i]._pDate->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_noteSlot[i]._pContent->SetTextColor(GetColor(GCT_VALUE));
		m_noteSlot[i]._pWho->SetTextColor(GetColor(GCT_VALUE));
		m_noteSlot[i]._pDate->SetTextColor(GetColor(GCT_VALUE));
	}

	m_pNoteList->CreateTextEx(MAX_NOTE_SLOT,  GetDefaultFontName());
	m_pNoteList->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNoteList->SetSelColor(0, 0, 0, 0);

	m_pNoteSelect->SetEnable(FALSE);
	m_pNoteFocus->SetEnable(FALSE);
}

void CPopupCommunity::_InitNoteChild(void)
{
	m_pNoteChild->SetEnable(FALSE);

	m_pNoteWhoKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteWhoValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pNoteWhoKey->SetTextColor(GetColor(GCT_STATIC));
	m_pNoteWhoValue->SetTextColor(GetColor(GCT_VALUE));
	m_pNoteValue->SetTextColor(GetColor(GCT_VALUE));
	m_pNoteWhoKey->SetText(GAME_STRING("STR_POPUP_RECEIVER"));/*�޴»��*/

	INIT_CAPTION_BUTTON(m_pNoteSend, GAME_STRING("STR_POPUP_REPLY"));/*�����ϱ�*/
	INIT_CAPTION_BUTTON(m_pNoteAccept, GAME_STRING("STR_POPUP_ACCEPT"));/*����*/
	INIT_CAPTION_BUTTON(m_pNoteReject, GAME_STRING("STR_POPUP_REJECT"));/*����*/
	INIT_CAPTION_BUTTON(m_pNoteClose, GAME_STRING("STR_POPUP_CLOSE"));/*�ݱ�*/

	//	����, ĳ�� ������ �������� ũ�Ⱑ  �ѹ� �÷��� �ǵ���
	m_pNoteGiftItemFrame->SetEnable(TRUE);
	m_pNoteGiftWeaponImage->SetEnable(TRUE);
	m_pNoteGiftCharacterImage->SetEnable(TRUE);
	m_pNoteGiftCashItemImage->SetEnable(TRUE);
	FLUSH_CONTROL(m_pNoteGiftItemFrame);
	FLUSH_CONTROL(m_pNoteGiftWeaponImage);
	FLUSH_CONTROL(m_pNoteGiftCharacterImage);
	FLUSH_CONTROL(m_pNoteGiftCashItemImage);
	m_pNoteGiftItemFrame->SetEnable(TRUE);
	m_pNoteGiftWeaponImage->SetEnable(TRUE);
	m_pNoteGiftCharacterImage->SetEnable(TRUE);
	m_pNoteGiftCashItemImage->SetEnable(TRUE);

	//
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;

	// �޴� ���
	ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
	m_pNoteWhoValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pNoteWhoValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteWhoValue->SetTextColor(GetColor(GCT_VALUE));
	m_pNoteWhoValue->SetTextAutoWrap( FALSE);
	m_pNoteWhoValue->SetIMEMode( IME_MODE_NATIVE);
	
	// ������ ����
	ime._nTextLimitCount = 120;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
	ime._StyleIME |= IME_STYLE_MULTILINE;
	m_pNoteValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pNoteValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pNoteValue->SetTextColor(GetColor(GCT_VALUE));
	m_pNoteValue->SetIMEMode( IME_MODE_NATIVE);

	m_pNoteGiftItemFrame->SetEnable(FALSE);
	m_pNoteGiftWeaponImage->SetEnable(FALSE);
	m_pNoteGiftCharacterImage->SetEnable(FALSE);
	m_pNoteGiftCashItemImage->SetEnable(FALSE);
	m_pNoteGiftSeparate->SetEnable(FALSE);

	for(INT32 i=0; i<2; i++)
	{
		INIT_STATIC_CAPTION(m_pNoteGiftValue[i], "");

		m_pNoteGiftValue[i]->SetEnable(FALSE);
	}

	// Note ������
	INIT_CAPTION(m_pNoteGageBg, "");
}

void CPopupCommunity::_UpdateContactChild(void)
{
	if (0 < m_PackageGoodsID && m_pNoteGiftCashItemImage->isEnable())
	{
		g_pFramework->SetPackageGoodsShape(m_pNoteGiftCashItemImage, m_PackageGoodsID);

		REAL32 posX = m_pNoteGiftCashItemImage->getPositionX();
		REAL32 posY = m_pNoteGiftCashItemImage->getPositionY();

		posX += 24.0f;
		posY -= 14.0f;

		m_pNoteGiftCashItemImage->setPosition(posX, posY);
		m_pNoteGiftCashItemImage->UpdateRenderObjects();
	}

	switch( m_currentTab)
	{
	case COMMUNITY_FRIEND_TAB:
		{
			// ������Ʈ�� ������ �ִ��� Ȯ���Ѵ�.
			if (g_pCommunity->BeCommunityUserInfo())
			{
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoPopup->SetEnable(TRUE);
				#else
				m_pContactChild->SetEnable(TRUE);
				#endif
			}
			else
			{
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoPopup->SetEnable(FALSE);
				#else
				m_pContactChild->SetEnable(FALSE);
				#endif
				return;
			}

			#ifdef USE_MODE_RECORD			
			// ���� ��ġ/����ǥ��
			char position[MAX_STRING_COUNT] = {0, };
			UINT32 connectState = g_pCommunity->GetCommnunityConnectState();
			PrintFriendState(connectState, position, MAX_STRING_COUNT);
			m_pUserDetailInfoPositionValue->SetText(position);

			if(TRUE == g_pCommunity->GetUserDetailInfoFlag())
			{
				m_bUserDetailInfoFlag = TRUE;
				g_pCommunity->SetUserDetailInfoFlag(FALSE);
				DEACTIVATE_BUTTON( m_pUserDetailInfoDetail);
			}
			const USER_INFO_ALL_RECORD* info = g_pCommunity->GetCommunityAllRecord();
			_SetRecord( info);
			#else
			// ���� ��ġ/����ǥ��
			char position[MAX_STRING_COUNT] = {0, };
			UINT32 connectState = g_pCommunity->GetCommnunityConnectState();
			PrintFriendState(connectState, position, MAX_STRING_COUNT);
			m_pPositionValue->SetText(position);

			// �г���
			m_pNicknameValue->SetTextEllipsis(g_pCommunity->GetCommnunityInfoNick());

			// ������
			char record[MAX_STRING_COUNT];
			char killDeath[MAX_STRING_COUNT];
			char headshot[MAX_STRING_COUNT];

			const USER_INFO_RECORD* info = g_pCommunity->GetCommunityRecord();

			sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), info->_match, info->_win, info->_lose);/*%d�� %d�� %d��*/
			sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), info->_killcount, info->_death, g_pGameContext->getKillRate(info->_killcount, info->_death));/*KILL/DEATH: %d/%d*/

			INT32 headshotPercent = TotalHeadShotPercent(info);

			sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);/*HEADSHOT: %d%%*/

			m_pTotalRecord->SetText(record);
			m_pTotalKillDeath->SetText(killDeath);
			m_pTotalHeadshot->SetText(headshot);


			sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), info->_smatch, info->_swin, info->_slose);/*%d�� %d�� %d��*/
			sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), info->_skillcount, info->_sdeath, g_pGameContext->getKillRate(info->_skillcount, info->_sdeath));/*KILL/DEATH: %d/%d*/

			INT32 sheadshotPercent = SeasonHeadShotPercent(info);
			
			sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), sheadshotPercent);/*HEADSHOT: %d%%*/

			m_pSeasonRecord->SetText(record);
			m_pSeasonKillDeath->SetText(killDeath);
			m_pSeasonHeadshot->SetText(headshot);
			#endif

			// ��ư ����
			m_pProfile->SetEnable(TRUE);

			//	default
			DEACTIVATE_BUTTON(m_pFriend);
			DEACTIVATE_BUTTON(m_pInvite);


			// ģ�� ��Ͽ� �ִ��� Ȯ���Ѵ�.
			BOOL beFriendList = FALSE;
			for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
			{
				const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
				if(NULL == node) continue;

				if (0 == i3String::Compare(g_pCommunity->GetCommnunityInfoNick(), node->_nick))
				{
					beFriendList = TRUE;
					break;
				}
			}

			// ģ�� ��Ͽ� ���ٸ�, ��ưó�� ���� �ʴ´�.
			if (!beFriendList)
			{
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoProfile->SetEnable(TRUE);
				m_pUserDetailInfoFriendRefuse->SetEnable(TRUE);
				DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
				#else
				m_pProfile->SetEnable(FALSE);
				m_pFriendRefuse->SetEnable(FALSE);
				#endif
				return;
			}


			//	ģ�� ��û�����̸� 'ģ������' ��ư����, ���� ģ���� 'ģ������' ��ư����,
			switch(GET_FRIEND_STATE(connectState))
			{
			//	ģ���� ��� ���
			case FRIEND_STATE_HOLD:
				{
					#ifdef USE_MODE_RECORD
					m_pUserDetailInfoProfile->SetCaption(GAME_STRING("STR_POPUP_ACCEPT_FRIEND"));/*ģ������*/					
					m_pUserDetailInfoFriendRefuse->SetEnable(TRUE);
					DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
					#else
					m_pProfile->SetCaption(GAME_STRING("STR_POPUP_ACCEPT_FRIEND"));/*ģ������*/
					#ifdef USE_FRIEND_REFUSE
					m_pFriendRefuse->SetEnable(TRUE);
					#endif
					#endif
				}
				break;
			//	ģ���� ���� ��û �Ǵ� ��������
			case FRIEND_STATE_REQUEST:
			case FRIEND_STATE_OFFLINE:
				{
					#ifdef USE_MODE_RECORD					
					m_pUserDetailInfoProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*ģ������*/										
					m_pUserDetailInfoFriendRefuse->SetEnable(FALSE);
					DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
					#else
					m_pProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*ģ������*/
					m_pFriendRefuse->SetEnable(FALSE);
					#endif
				}
				break;
			//	ģ���� �����
			case FRIEND_STATE_ONLINE:
				{
					#ifdef USE_MODE_RECORD
					m_pUserDetailInfoProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*ģ������*/
					m_pUserDetailInfoFriendRefuse->SetEnable(FALSE);
					#else
					m_pProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*ģ������*/
					m_pFriendRefuse->SetEnable(FALSE);
					#endif

					CStageReady * pStageReady = ((CStageReady*)m_pParent);
					I3ASSERT( pStageReady != NULL);
	
					if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
					{	// ���� ��Ʋ ���� �ƴ϶��
						if(PHASE_READY_ROOM == pStageReady->GetCurrentPhaseType())
						{	//	�ڽ��� �濡 �� ������ ģ���ʴ� Ȱ��
							#ifdef USE_MODE_RECORD
							ACTIVATE_BUTTON(m_pUserDetailInfoInvite);
							#else
							ACTIVATE_BUTTON(m_pInvite);
							#endif
						}
					}
				}
				break;
			//	ģ���� �濡 ������
			case FRIEND_STATE_ROOM:
				{
					#ifdef USE_MODE_RECORD
					m_pUserDetailInfoProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*ģ������*/
					m_pUserDetailInfoFriendRefuse->SetEnable(FALSE);
					#else
					m_pProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*ģ������*/
					m_pFriendRefuse->SetEnable(FALSE);
					#endif

					CStageReady * pStageReady = ((CStageReady*)m_pParent);
					I3ASSERT( pStageReady != NULL);

					if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
					{	// ���� ��Ʋ ���� �ƴ϶��
						//	�ڽ��� �濡 �� ������ ģ���ʴ� Ȱ��
						if(PHASE_READY_ROOM == pStageReady->GetCurrentPhaseType())
						{
							BOOL	SameRoom = g_pGameContext->IsSameRoom(connectState);
							if (SameRoom == FALSE)
							{
								#ifdef USE_MODE_RECORD
								ACTIVATE_BUTTON(m_pUserDetailInfoInvite);
								ACTIVATE_BUTTON(m_pUserDetailInfoFriend);		//	ģ�� ã�ư��� Ȱ��
								#else
								ACTIVATE_BUTTON(m_pInvite);
								ACTIVATE_BUTTON(m_pFriend);		//	ģ�� ã�ư��� Ȱ��
								#endif
							}
						}
						else
						{
							#ifdef USE_MODE_RECORD
							ACTIVATE_BUTTON(m_pUserDetailInfoFriend);		//	ģ�� ã�ư��� Ȱ��
							#else
							ACTIVATE_BUTTON(m_pFriend);		//	ģ�� ã�ư��� Ȱ��
							#endif
						}
					}
				}
				break;
			default:	
				{
					#ifdef USE_MODE_RECORD
					m_pUserDetailInfoProfile->SetEnable(FALSE);
					m_pUserDetailInfoFriendRefuse->SetEnable(FALSE);
					DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
					#else
					m_pProfile->SetEnable(FALSE);
					m_pFriendRefuse->SetEnable(FALSE);
					#endif
				}
				break;				
			}
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			if (g_pCommunity->BeCommunityUserInfo())
			{
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoPopup->SetEnable(TRUE);
				#else
				m_pContactChild->SetEnable(TRUE);
				#endif
			}
			else
			{
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoPopup->SetEnable(FALSE);
				#else
				m_pContactChild->SetEnable(FALSE);
				#endif
				return;
			}


			const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);

 			if (node)
			{
				char position[MAX_STRING_COUNT] = {0, };

				CStageReady * pStageReady = ((CStageReady*)m_pParent);
				I3ASSERT( pStageReady != NULL);

				CUserLocationInfo	UserLocInfo(node->_state);
				INT32 nMatch = UserLocInfo.GetMatch();

				if( nMatch > 0 )
				{
					char state2[MAX_STRING_COUNT] = {0, };

					switch(GET_FRIEND_STATE(node->_state))
					{
					case FRIEND_STATE_OFFLINE:
						{
							i3String::Copy(position, GAME_STRING("STR_POPUP_OFFLINE_STATE"), MAX_STRING_COUNT );/*��������*/
							#ifdef USE_MODE_RECORD
							DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
							#endif
						}
						break;
					case FRIEND_STATE_ONLINE:
						{
							GetBattleTeamName( state2, MAX_STRING_COUNT, nMatch-1, TRUE );

							sprintf_s(position, GAME_STRING("STR_POPUP_WHAT_READY"),
									UserLocInfo.GetDisplayServerChannelNumber(), state2);/*%03d-%s �����*/
						}
						break;
					case FRIEND_STATE_ROOM:
						{
							GetBattleTeamName( state2, MAX_STRING_COUNT, nMatch-1, TRUE );

							sprintf_s(position, GAME_STRING("STR_POPUP_WHAT_PLAY"),
									UserLocInfo.GetDisplayServerChannelNumber(), state2);/*%03d-%s ������*/
							break;
						}
					}
				}
				else
				{
					UINT32	State = FRIEND_STATE_64_TO_32(node->_state);
					PrintFriendState(State, position, MAX_STRING_COUNT);

					#ifdef USE_MODE_RECORD
					if(FRIEND_STATE_OFFLINE == GET_FRIEND_STATE(State))
					{
						DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
					}
					#endif
				}

				#ifdef USE_MODE_RECORD
				if(TRUE == g_pCommunity->GetUserDetailInfoFlag())
				{
					m_bUserDetailInfoFlag = TRUE;
					g_pCommunity->SetUserDetailInfoFlag(FALSE);
				}

				m_pUserDetailInfoNicknameValue->SetTextEllipsis((char*)node->_nick);
				m_pUserDetailInfoPositionValue->SetText(position);

				const USER_INFO_ALL_RECORD* info = g_pCommunity->GetCommunityAllRecord();
				_SetRecord(info);
				#else
				char record[MAX_STRING_COUNT];
				char killDeath[MAX_STRING_COUNT];
				char headshot[MAX_STRING_COUNT];

				m_pNicknameValue->SetTextEllipsis(node->_nick);
				m_pPositionValue->SetText(position);
	
				const USER_INFO_RECORD* info = g_pCommunity->GetCommunityRecord();

				sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), info->_match, info->_win, info->_lose);/*%d�� %d�� %d��*/
				sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), info->_killcount, info->_death, g_pGameContext->getKillRate(info->_killcount, info->_death));/*KILL/DEATH: %d/%d*/

				INT32 headshotPercent = TotalHeadShotPercent(info);

				sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);/*HEADSHOT: %d%%*/

				m_pTotalRecord->SetText(record);
				m_pTotalKillDeath->SetText(killDeath);
				m_pTotalHeadshot->SetText(headshot);

				sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), info->_smatch, info->_swin, info->_slose);/*%d�� %d�� %d��*/
				sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), info->_skillcount, info->_sdeath, g_pGameContext->getKillRate(info->_skillcount, info->_sdeath));/*KILL/DEATH: %d/%d*/
	
				INT32 sheadshotPercent = SeasonHeadShotPercent(info);
	
				sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), sheadshotPercent);/*HEADSHOT: %d%%*/

				m_pSeasonRecord->SetText(record);
				m_pSeasonKillDeath->SetText(killDeath);
				m_pSeasonHeadshot->SetText(headshot);
				#endif

				#ifdef USE_MODE_RECORD
				DEACTIVATE_BUTTON(m_pUserDetailInfoFriend);
				DEACTIVATE_BUTTON(m_pUserDetailInfoInvite);
				#else
				DEACTIVATE_BUTTON(m_pFriend);
				DEACTIVATE_BUTTON(m_pInvite);
				#endif

				//	ģ�� ��û�����̸� 'ģ������' ��ư����, ���� ģ���� 'ģ������' ��ư����,
				switch(GET_FRIEND_STATE(node->_state))
				{
					//ģ���� ���翡 ������
				case FRIEND_STATE_ONLINE:
					{
						if( GET_FRIEND_MATCH( node->_state ) > 0 )			// ����� ��Ʋ �� �����϶�
						{
							if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() ) // ���� ���� ������
							{	// ã�ư��� ��
								#ifdef USE_MODE_RECORD
								ACTIVATE_BUTTON(m_pUserDetailInfoFriend);								
								#else
								ACTIVATE_BUTTON(m_pFriend);
								#endif
							}
						}
						else												// ����� ��Ʋ �� ���� �ƴҶ�
						{
							// �ʴ��ϱ� ��
							if( PHASE_READY_ROOM == pStageReady->GetCurrentPhaseType() )
							{	// ã�ư��� ��
								#ifdef USE_MODE_RECORD
								ACTIVATE_BUTTON(m_pUserDetailInfoInvite);								
								#else
								ACTIVATE_BUTTON(m_pInvite);
								#endif
							}
							else
							{
								if( g_pFramework->GetClanContext()->InBattleTeam() )
								{
									#ifdef USE_MODE_RECORD
									ACTIVATE_BUTTON(m_pUserDetailInfoInvite);								
									#else
									ACTIVATE_BUTTON(m_pInvite);
									#endif
								}
							}
						}
					}
					break;
					//	ģ���� �濡 ������
				case FRIEND_STATE_ROOM:
					{
						if( GET_FRIEND_MATCH( node->_state ) == 0 )			// ����� ��Ʋ �� ���°� �ƴҶ�
						{
							if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
							{
								#ifdef USE_MODE_RECORD
								ACTIVATE_BUTTON(m_pUserDetailInfoFriend);								
								#else
								ACTIVATE_BUTTON(m_pFriend);			//	ģ�� ã�ư��� Ȱ��
								#endif

								CStageReady * pStageReady = ((CStageReady*)m_pParent);
								I3ASSERT( pStageReady != NULL);

								//	�ڽ��� �濡 �� ������ ģ���ʴ� Ȱ��
								if(PHASE_READY_ROOM == pStageReady->GetCurrentPhaseType())
								{
									#ifdef USE_MODE_RECORD
									ACTIVATE_BUTTON(m_pUserDetailInfoInvite);								
									#else
									ACTIVATE_BUTTON(m_pInvite);			//	ģ�� ã�ư��� Ȱ��
									#endif
								}
							}
							else
							{
								#ifdef USE_MODE_RECORD
								ACTIVATE_BUTTON(m_pUserDetailInfoFriend);								
								#else
								ACTIVATE_BUTTON(m_pInvite);			//	ģ�� ã�ư��� Ȱ��
								#endif
							}
						}
					}
					break;
				}
			}
		}
		break;
	}
}

void CPopupCommunity::_UpdateContactList(void)
{
	UINT32 topIdx = m_pContactList->GetCurrentTop();

	for(UINT32 i = 0; i < MAX_SLOT_FOR_CONTACT_LIST; i++)
	{
		m_contactSlot[i]._pNick->SetText("");
		m_contactSlot[i]._pState->SetText("");
		m_contactSlot[i]._pMark->SetEnable(FALSE);
	}

	m_pContactList->Clear();

	//////////////////////////////////////////////////////////////
	//	ģ�� ���
	switch( m_currentTab)
	{
	case COMMUNITY_FRIEND_TAB:
		{
			UINT32 count = g_pCommunity->GetFriendCount();

			for(UINT32 i = 0; i < count; i++)
			{
				m_pContactList->AddElement("");
			}

			for(UINT32 i = 0; i + topIdx < count && i < MAX_SLOT_FOR_CONTACT_LIST; i++)
			{
				const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i + topIdx);
				if(NULL == node)	continue;

				m_contactSlot[i]._pMark->SetEnable(TRUE);
				m_contactSlot[i]._pMark->SetShapeUVIndex(node->_rank);
				m_contactSlot[i]._pNick->SetTextEllipsis((char*)node->_nick);

				char state[MAX_STRING_COUNT] = {0, };

				switch(GET_FRIEND_STATE(node->_state))
				{
				case 1:		i3String::Copy( state, GAME_STRING("STR_POPUP_NEED_TO_ACCEPT"), MAX_STRING_COUNT );/*������û*/		break;
				case 2:		i3String::Copy( state, GAME_STRING("STR_POPUP_READY_TO_ACCEPT"), MAX_STRING_COUNT );/*��ϴ��*/	break;
				case 3:		i3String::Copy( state, GAME_STRING("STR_POPUP_OFFLINE_STATE"), MAX_STRING_COUNT );/*��������*/		break;
				case 4:		i3String::Copy( state, GAME_STRING("STR_POPUP_READY_STATE"), MAX_STRING_COUNT );/*�����*/			break;
				case 5:
					{
						CUserLocationInfo	UserLocInfo(node->_state);

						i3String::Format(state, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_POSITION"),
							UserLocInfo.GetDisplayServerChannelNumber(), UserLocInfo.GetDisplayRoomNumber());/*%d-%d����*/
						break;
					}
				default:	i3String::Copy( state, "", MAX_STRING_COUNT );	break;
				}

				m_contactSlot[i]._pState->SetText(state);


				// ��
				I3COLOR* color;

				if (3 < GET_FRIEND_STATE(node->_state))
				{
					color = GetColor(GCT_FOCUS);
				}
				else
				{
					color = GetColor(GCT_DEFAULT);
				}

				m_contactSlot[i]._pNick->SetTextColor(color);
				m_contactSlot[i]._pState->SetTextColor(color);

				#ifdef USE_MODE_RECORD
				// �����ִ� ���� ���� �󼼺��� â�� ������ ����
				if(m_CurrentUserInfoUID == node->_uid)
				{
					g_pCommunity->SetCommnunityConnectState(node->_state);
					g_pCommunity->SetCommnuitUserInfoBasic3()->_ui32Rank = node->_rank;
				}				
				#endif				
			}

			if (m_pContactList->GetElementCount() <= m_SelectedContact)
			{
				m_SelectedContact = -1;
//				m_pDelete->setControlDisable(TRUE);
				g_pCommunity->SetCommunityUserInfo(FALSE);
			}
		}
		break;
	//////////////////////////////////////////////////////////////
	//	���� ���
	case COMMUNITY_BLOCK_TAB:
		{
		}
		break;
	//////////////////////////////////////////////////////////////
	//	Ŭ���� ���
	case COMMUNITY_CLAN_TAB:
		{
			UINT32 count = g_pCommunity->GetClanMemberCount();

			for(UINT32 i = 0; i < count; i++)
			{
				m_pContactList->AddElement("");
			}

 			for(UINT32 i = 0; i + topIdx < count && i < MAX_SLOT_FOR_CONTACT_LIST; i++)
			{
				const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(i + topIdx);

				m_contactSlot[i]._pMark->SetEnable(TRUE);
				m_contactSlot[i]._pMark->SetShapeUVIndex(node->_rank);
				m_contactSlot[i]._pNick->SetTextEllipsis(node->_nick);

				char strState[MAX_STRING_COUNT] = {0, };
	
				CUserLocationInfo	UserLocInfo(node->_state);

				INT32 nMatch = UserLocInfo.GetMatch();

				if( nMatch > 0 )
				{
					char strState2[MAX_STRING_COUNT] = {0, };

					switch(UserLocInfo.GetState())
					{
					case FRIEND_STATE_OFFLINE:	i3String::Copy(strState, GAME_STRING("STR_POPUP_OFFLINE_STATE"), MAX_STRING_COUNT );/*��������*/	break;
					case FRIEND_STATE_ONLINE:
						{
							GetBattleTeamName( strState2, MAX_STRING_COUNT, nMatch-1, FALSE );

							i3String::Format(strState, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_READY_MATCHING"),
								UserLocInfo.GetDisplayServerChannelNumber(), strState2);/*%03d-%s\nŬ����ġ �����*/
						}
						break;
					case FRIEND_STATE_ROOM:
						{
							GetBattleTeamName( strState2, MAX_STRING_COUNT, nMatch-1, FALSE );

							i3String::Format(strState, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_PLAY_MATCHING"),
								UserLocInfo.GetDisplayServerChannelNumber(), strState2);/*%03d-%s\nŬ����ġ ������*/
							break;
						}
					default:	i3String::Copy( strState, "", MAX_STRING_COUNT );			break;
					}
				}
				else
				{
					switch(UserLocInfo.GetState())
					{
					case FRIEND_STATE_OFFLINE:	i3String::Copy( strState, GAME_STRING("STR_POPUP_OFFLINE_STATE"), MAX_STRING_COUNT );/*��������*/	break;
					case FRIEND_STATE_ONLINE:	i3String::Copy( strState, GAME_STRING("STR_POPUP_ONLINE_STATE"), MAX_STRING_COUNT );/*�¶���*/		break;
					case FRIEND_STATE_ROOM:
						{
							i3String::Format(strState, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_POSITION"),
								UserLocInfo.GetDisplayServerChannelNumber(), UserLocInfo.GetDisplayRoomNumber());/*%d-%d����*/
							break;
						}
					default:	i3String::Copy( strState, "", MAX_STRING_COUNT );			break;
					}
				}

				m_contactSlot[i]._pState->SetText(strState);

				// ��
				I3COLOR* color;

				if (FRIEND_STATE_OFFLINE < GET_FRIEND_STATE(node->_state))
				{
					color = GetColor(GCT_FOCUS);
				}
				else
				{
					color = GetColor(GCT_DEFAULT);
				}

				m_contactSlot[i]._pNick->SetTextColor(color);
				m_contactSlot[i]._pState->SetTextColor(color);

				#ifdef USE_MODE_RECORD
				// �����ִ� ���� ���� �󼼺��� â�� ������ ����
				if(m_CurrentUserInfoUID == node->_uid)
				{
					g_pCommunity->SetCommnunityConnectState((UINT32)node->_state);
					g_pCommunity->SetCommnuitUserInfoBasic3()->_ui32Rank = node->_rank;
				}				
				#endif
			}

			if (m_pContactList->GetElementCount() <= m_SelectedContact)
			{
				m_SelectedContact = -1;
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoPopup->SetEnable(FALSE);
				#else
				m_pContactChild->SetEnable(FALSE);
				#endif
				g_pCommunity->SetCommunityUserInfo(FALSE);
			}
		}
		break;
	}
}

void CPopupCommunity::_AddFriend(char* nick)
{
	if (g_pCommunity->GetFriendCount() >= FRIEND_MAX_COUNT)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_NO_MORE_FRIEND"));/*�� �̻� ģ���� �߰��� �� �����ϴ�.*/
		return;
	}

	for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
		if(NULL == node) continue;

		if (0 == i3String::Compare(nick, node->_nick))
		{
			char message[MAX_STRING_COUNT];

			sprintf_s(message, GAME_STRING("STR_POPUP_MESSAGE_ALREADY_FRIEND"), nick);/*[%s]���� �̹� ģ����Ͽ� ��ϵǾ� �ֽ��ϴ�.*/
			STRING_POPUP(message);
			return;
		}
	}

	if (g_pGameContext->IsMyNickName(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_SELF"));/*�ڽ��� �ʴ��� �� �����ϴ�.*/
		return;
	}

	g_pCommunity->InsertFriend(nick);
}

void CPopupCommunity::_AddBlock(char* nick)
{
}

void CPopupCommunity::_FindUser(char* nick)
{
	// �ڽ��� ���´�.
	if (g_pGameContext->IsMyNickName(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INPUT_SELF"));/*�ڽ��� �г����� �Է��ϼ̽��ϴ�.*/
		return;
	}

	// ģ�� ��Ͽ� �ִ��� Ȯ���Ѵ�.
	for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
		if(NULL == node) continue;

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// ��ϼ���
			OnContactSelect(i);
			return;
		}
	}

	// Ŭ�� ��Ͽ� �ִ��� Ȯ���Ѵ�.
	for(UINT32 i = 0; i < g_pCommunity->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(i);

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// UID ����
			OnContactSelect(i);
			return;
		}
	}

	// ���� ������ ���� ��Ͽ� �ִ��� Ȯ���Ѵ�.
	// TODO :

	// ��Ͽ� ���ٸ� ������ ��û
	g_pCommunity->SetCommunityUserInfo(FALSE);
	g_pCommunity->SetCommnunityInfoNick(nick);
	g_pCommunity->FindUser(nick);
}

#ifdef USE_MODE_RECORD
void CPopupCommunity::_SetRecord( const USER_INFO_ALL_RECORD * pRecord)
{
	if( pRecord != NULL)
	{
		INT32 headshotPercent = 0;
		INT32 hitPercent = 0;

		// User Info
		char rank[MAX_STRING_COUNT];
		char clan_name[MAX_STRING_COUNT];
		char title[MAX_STRING_COUNT];
		char point[MAX_STRING_COUNT];
		char cash[MAX_STRING_COUNT];

		// Record
		char str_record[MAX_STRING_COUNT];
		char record[MAX_STRING_COUNT] = "";
		char all_record[MAX_STRING_COUNT];
		char killDeath[MAX_STRING_COUNT];
		char headshot[MAX_STRING_COUNT];
		char dis_count[MAX_STRING_COUNT];
		char str_user_ranking[MAX_STRING_COUNT];
		char user_ranking[MAX_STRING_COUNT];

		// Mode Record
		char mode_match[MAX_STRING_COUNT] = "";
		char mode_killDeath[MAX_STRING_COUNT];
		char mode_headshot[MAX_STRING_COUNT];
		char mode_hit_count[MAX_STRING_COUNT];

		sprintf_s(clan_name, "%s %s", GAME_STRING("STBL_IDX_CLAN_NAME"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
		sprintf_s(title, "%s :  -  ,  -  ,  -  ", GAME_STRING("STR_TBL_DESIGNATION_TEXT"));
		sprintf_s(point, "%s : %s", GAME_STRING("STR_TBL_PROFILE_CHALLENGE_PRIZE_POINT_TEXT"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
		sprintf_s(cash, "%s : %s", GAME_STRING("STR_SHOP_CASH"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));

		sprintf_s(str_record, "%s%s : ", GAME_STRING("STBL_IDX_BATTLE_WIN_COUNT"),GAME_STRING("STBL_IDX_BATTLE_LOSE_COUNT"));
		//sprintf_s(killDeath, "%s : ", GAME_STRING("STR_TBL_PROFILE_KILL_DEATH_RATE2"));
		//sprintf_s(headshot, "%s : ", GAME_STRING("STR_TBL_PROFILE_HEADSHOT_TEXT"));
		//sprintf_s(dis_count, "%s", GAME_STRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"));
		sprintf_s(str_user_ranking, "UserRanking : ");

		sprintf_s(mode_match, "%s", GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
		sprintf_s(mode_killDeath, "%s : %s", GAME_STRING("STR_TBL_PROFILE_KILL_DEATH_RATE2"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
		sprintf_s(mode_headshot, "%s : %s", GAME_STRING("STR_TBL_PROFILE_HEADSHOT_TEXT"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
		sprintf_s(mode_hit_count, "%s : %s", GAME_STRING("STR_POPUP_HIT_RATE"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));

		m_pUserDetailInfoRankMark->SetEnable(TRUE);
		m_pUserDetailInfoNicknameValue->SetEnable(TRUE);
		m_pUserDetailInfoRank->SetEnable(TRUE);
		m_pUserDetailInfoClanName->SetEnable(TRUE);	
		m_pUserDetailInfoTitle->SetEnable(TRUE);	
		m_pUserDetailInfoPoint->SetEnable(TRUE);
		m_pUserDetailInfoToll->SetEnable(TRUE);
		//m_pPositionKey->SetEnable(TRUE);
		//m_pPositionValue->SetEnable(TRUE);
		// Total Record
		m_pUserDetailInfoTotalRecord->SetEnable(TRUE);
		m_pUserDetailInfoTotalKillDeath->SetEnable(TRUE);
		m_pUserDetailInfoTotalHeadshot->SetEnable(TRUE);
		m_pUserDetailInfoTotalDis->SetEnable(TRUE);
		m_pUserDetailInfoTotalUserRanking->SetEnable(TRUE);
		// Season Record
		m_pUserDetailInfoSeasonRecord->SetEnable(TRUE);
		m_pUserDetailInfoSeasonKillDeath->SetEnable(TRUE);
		m_pUserDetailInfoSeasonHeadshot->SetEnable(TRUE);
		m_pUserDetailInfoSeasonDis->SetEnable(TRUE);
		m_pUserDetailInfoSeasonUserRanking->SetEnable(TRUE);	
		// Mode Record
		m_pModeRecordValue->SetEnable(TRUE);
		m_pModeKillDeath->SetEnable(TRUE);
		m_pModeHeadshot->SetEnable(TRUE);
		//m_pModeHitPercent->SetEnable(TRUE);

		// User Info		
		UINT32 connectState = GET_FRIEND_STATE(g_pCommunity->GetCommnunityConnectState());
		UINT32 ui32Rank = g_pCommunity->GetCommunityUserInfoBasic3()->_ui32Rank;
		m_pUserDetailInfoNicknameValue->SetTextEllipsis((char*)g_pCommunity->GetCommnunityInfoNick());		

		sprintf_s(rank, "%s : %s", GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_RANK"), GetCharaRank(ui32Rank));
		m_pUserDetailInfoRank->SetText(rank);
		m_pUserDetailInfoRankMark->SetShapeUVIndex(ui32Rank);

		if (TRUE == m_bUserDetailInfoFlag)
		{
			// User Info
			//	Ŭ����
			sprintf_s(clan_name, "%s%s", GAME_STRING("STBL_IDX_CLAN_NAME"), g_pCommunity->GetCommunityUserInfoBasic3()->_clanName);
			// ȣĪ
			INT32	Loop = 0;
			char	EquipUserTitle[MAX_EQUIP_USER_TITLE][256];
			for(Loop = 0; Loop < MAX_EQUIP_USER_TITLE; Loop++)
			{				
				EquipUserTitle[Loop][0] = ' ';
				EquipUserTitle[Loop][1] = ' ';
				EquipUserTitle[Loop][2] = '-';
				EquipUserTitle[Loop][3] = ' ';
				EquipUserTitle[Loop][4] = ' ';
				EquipUserTitle[Loop][5] = '\0';				
			}

			for(Loop = 0; Loop < MAX_EQUIP_USER_TITLE; Loop++)
			{
				if(0 < g_pCommunity->GetCommunityUserInfoBasic3()->_EquipUserTitle[Loop])
				{
					GetUserTitleName(g_pCommunity->GetCommunityUserInfoBasic3()->_EquipUserTitle[Loop], EquipUserTitle[Loop]);
				}
			}

			sprintf_s(title, "%s : %s, %s, %s", GAME_STRING("STR_TBL_DESIGNATION_TEXT"), EquipUserTitle[0], EquipUserTitle[1], EquipUserTitle[2]);
			// ����Ʈ
			sprintf_s(point, "%s : %d", GAME_STRING("STR_TBL_PROFILE_CHALLENGE_PRIZE_POINT_TEXT"), g_pCommunity->GetCommunityUserInfoBasic3()->_ui32Point);
			// ��
			sprintf_s(cash, "%s : %d", GAME_STRING("STR_SHOP_CASH"), g_pCommunity->GetCommunityUserInfoBasic3()->_ui32cash);

			m_pUserDetailInfoClanName->SetText(clan_name);
			m_pUserDetailInfoTitle->SetText(title);
			m_pUserDetailInfoPoint->SetText(point);
			m_pUserDetailInfoToll->SetText(cash);
		}
		else
		{
			// User Info
			m_pUserDetailInfoClanName->SetText(clan_name);
			m_pUserDetailInfoTitle->SetText(title);
			m_pUserDetailInfoPoint->SetText(point);
			m_pUserDetailInfoToll->SetText(cash);
		}

		// ���� ���
		{
			headshotPercent = 0;

			//	"���� : %d�� %d�� %d��"
			sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_basic_record._season_record._match, pRecord->_basic_record._season_record._win, pRecord->_basic_record._season_record._lose);
			sprintf_s(all_record, "%s%s", str_record, record);			
			//	"KILL/DEATH: %d/%d"
			sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), pRecord->_basic_record._season_record._killcount, pRecord->_basic_record._season_record._death, g_pGameContext->getKillRate(pRecord->_basic_record._season_record._killcount, pRecord->_basic_record._season_record._death));

			if (0 == pRecord->_basic_record._season_record._dkillcount)
			{
				headshotPercent = 0;
			}
			else
			{
				headshotPercent = (INT32)(pRecord->_basic_record._season_record._headshot * 100.0f / pRecord->_basic_record._season_record._dkillcount);
			}

			//	"HEADSHOT: %d%%"
			sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);
			sprintf_s(dis_count, GAME_STRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"), pRecord->_basic_record._season_record._dis);
			if (TRUE == m_bUserDetailInfoFlag)
			{
				if(0 >= pRecord->_basic_record._season_record._user_ranking)
				{
					sprintf_s(user_ranking, "%s  -  ", str_user_ranking);
				}
				else
				{
					sprintf_s(user_ranking, "%s%d", str_user_ranking, pRecord->_basic_record._season_record._user_ranking);
				}
			}
			else
			{
				sprintf_s(user_ranking, "%s%s", str_user_ranking, GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
			}

			m_pUserDetailInfoSeasonRecord->SetText(all_record);
			m_pUserDetailInfoSeasonKillDeath->SetText(killDeath);
			m_pUserDetailInfoSeasonHeadshot->SetText(headshot);
			m_pUserDetailInfoSeasonDis->SetText(dis_count);
			m_pUserDetailInfoSeasonUserRanking->SetText(user_ranking);			
		}

		// ��Ż ���
		{
			headshotPercent = 0;			

			//	"���� : %d�� %d�� %d��"
			sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_basic_record._total_record._match, pRecord->_basic_record._total_record._win, pRecord->_basic_record._total_record._lose);
			sprintf_s(all_record, "%s%s", str_record, record);
			//	"KILL/DEATH: %d/%d"
			sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), pRecord->_basic_record._total_record._killcount, pRecord->_basic_record._total_record._death, g_pGameContext->getKillRate(pRecord->_basic_record._total_record._killcount, pRecord->_basic_record._total_record._death));

			if (0 == pRecord->_basic_record._total_record._dkillcount)
			{
				headshotPercent = 0;
			}
			else
			{
				headshotPercent = (INT32)(pRecord->_basic_record._total_record._headshot * 100.0f / pRecord->_basic_record._total_record._dkillcount);
			}

			//	"HEADSHOT: %d%%"
			sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);
			sprintf_s(dis_count, GAME_STRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"), pRecord->_basic_record._total_record._dis);			
			if (TRUE == m_bUserDetailInfoFlag)
			{
				if(0 >= pRecord->_basic_record._season_record._user_ranking)
				{
					sprintf_s(user_ranking, "%s  -  ", str_user_ranking);
				}
				else
				{
					sprintf_s(user_ranking, "%s%d", str_user_ranking, pRecord->_basic_record._total_record._user_ranking);
				}
			}
			else
			{
				sprintf_s(user_ranking, "%s%s", str_user_ranking, GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
			}

			m_pUserDetailInfoTotalRecord->SetText(all_record);
			m_pUserDetailInfoTotalKillDeath->SetText(killDeath);
			m_pUserDetailInfoTotalHeadshot->SetText(headshot);
			m_pUserDetailInfoTotalDis->SetText(dis_count);
			m_pUserDetailInfoTotalUserRanking->SetText(user_ranking);
		}    

		// Mode Record
		if (TRUE == m_bUserDetailInfoFlag)
		{
			headshotPercent = 0;
			hitPercent = 0;

			if (FALSE == m_bRecordTabFlag)
			{	
				//	"���� : %d�� %d�� %d��"
				sprintf_s(mode_match, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._dmatch, pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._win, pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._lose);
				sprintf_s(all_record, "%s%s", str_record, mode_match);
				//	"KILL/DEATH: %d/%d"
				sprintf_s(mode_killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._dkillcount, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._death, g_pGameContext->getKillRate(pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._killcount, pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._death));

				if (0 == pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._dkillcount)
				{
					headshotPercent = 0;
				}
				else
				{
					headshotPercent = (INT32)(pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._headshot * 100.0f / pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._dkillcount);
				}
				sprintf_s(mode_headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);

				if (0 == pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._shoot_count)
				{
					hitPercent = 0;
				}
				else
				{
					hitPercent = (INT32)(pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._hit_count * 100.0f / pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._shoot_count);
				}				
				sprintf_s(mode_hit_count, "%s : %d", GAME_STRING("STR_POPUP_HIT_RATE"), hitPercent);
			}
			else
			{// ���� ����
				//	"���� : %d�� %d�� %d��"
				sprintf_s(mode_match, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._dmatch, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._win, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._lose);
				sprintf_s(all_record, "%s%s", str_record, mode_match);
				//	"KILL/DEATH: %d/%d"
				sprintf_s(mode_killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._dkillcount, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._death, g_pGameContext->getKillRate(pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._killcount, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._death));

				if (0 == pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._dkillcount)
				{
					headshotPercent = 0;
				}
				else
				{
					headshotPercent = (INT32)(pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._headshot * 100.0f / pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._dkillcount);
				}
				sprintf_s(mode_headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);

				if (0 == pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._shoot_count)
				{
					hitPercent = 0;
				}
				else
				{
					hitPercent = (INT32)(pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._hit_count * 100.0f / pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._shoot_count);
				}
				sprintf_s(mode_hit_count, "%s : %d", GAME_STRING("STR_POPUP_HIT_RATE"), hitPercent);
			}

			m_pModeRecordValue->SetText(all_record);
			m_pModeKillDeath->SetText(mode_killDeath);
			m_pModeHeadshot->SetText(mode_headshot);
			//m_pModeHitPercent->SetText(mode_hit_count);
		}		
		else
		{
			sprintf_s(all_record, "%s%s", str_record, mode_match);
			m_pModeRecordValue->SetText(all_record);
			m_pModeKillDeath->SetText(mode_killDeath);
			m_pModeHeadshot->SetText(mode_headshot);
			//m_pModeHitPercent->SetText(mode_hit_count);
		}
	}
	else
	{
		m_pUserDetailInfoRankMark->SetEnable(FALSE);
		m_pUserDetailInfoNicknameValue->SetEnable(FALSE);
		m_pUserDetailInfoRank->SetEnable(FALSE);
		m_pUserDetailInfoClanName->SetEnable(FALSE);	
		m_pUserDetailInfoTitle->SetEnable(FALSE);	
		m_pUserDetailInfoPoint->SetEnable(FALSE);
		m_pUserDetailInfoToll->SetEnable(FALSE);
		//m_pPositionKey->SetEnable(FALSE);
		//m_pPositionValue->SetEnable(FALSE);
		// Total Record
		m_pUserDetailInfoTotalRecord->SetEnable(FALSE);
		m_pUserDetailInfoTotalKillDeath->SetEnable(FALSE);
		m_pUserDetailInfoTotalHeadshot->SetEnable(FALSE);
		m_pUserDetailInfoTotalDis->SetEnable(FALSE);
		m_pUserDetailInfoTotalUserRanking->SetEnable(FALSE);
		// Season Record
		m_pUserDetailInfoSeasonRecord->SetEnable(FALSE);
		m_pUserDetailInfoSeasonKillDeath->SetEnable(FALSE);
		m_pUserDetailInfoSeasonHeadshot->SetEnable(FALSE);
		m_pUserDetailInfoSeasonDis->SetEnable(FALSE);
		m_pUserDetailInfoSeasonUserRanking->SetEnable(FALSE);
		// Mode Record
		m_pModeRecordValue->SetEnable(FALSE);
		m_pModeKillDeath->SetEnable(FALSE);
		m_pModeHeadshot->SetEnable(FALSE);
		//m_pModeHitPercent->SetEnable(FALSE);
	}	
}
#endif

#ifdef USE_MESS_TEST
INT64 CPopupCommunity::_FindUserByUid(char* nick)
{
	// �ڽ��� ���´�.
	if (g_pGameContext->IsMyNickName(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INPUT_SELF"));/*�ڽ��� �г����� �Է��ϼ̽��ϴ�.*/
		return 0;
	}

	// ģ�� ��Ͽ� �ִ��� Ȯ���Ѵ�.
	for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
		if(NULL == node) continue;

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// UID ����
			return node->_uid;
		}
	}

	// Ŭ�� ��Ͽ� �ִ��� Ȯ���Ѵ�.
	for(UINT32 i = 0; i < g_pCommunity->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(i);

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// UID ����
			return node->_uid;
		}
	}

	// ���� ������ ���� ��Ͽ� �ִ��� Ȯ���Ѵ�.
	// TODO :
	// ��Ͽ� ���ٸ� ������ ��û
	//g_pCommunity->SetCommunityUserInfo(FALSE);
	//g_pCommunity->SetCommnunityInfoNick(nick);
	//g_pCommunity->FindUser(nick);

	return 0;
}
#endif

void CPopupCommunity::_UpdateNote(void)
{
	char szTemp[256];
	BOOL m_bIsSelectNote = FALSE;

	UINT32 count = g_pGameContext->GetNoteReceiveCount();
	m_pNoteList->Clear();
	for(UINT32 i = 0; i < count; i++)	m_pNoteList->AddElement("");
	UINT32 topIdx = m_pNoteList->GetCurrentTop();

	if( g_pGameContext->IsNoteNew())
	{
		//m_SelectedNote = -1;
		//m_SelectedNoteIndex = -1;
		// ���� ����Ʈ �ε��� �ʱ�ȭ ����
		ReceiveAllNote(topIdx);
		g_pGameContext->RemoveGameFlag( GAMEFLAG_NOTENEW);
	}

	if( g_pGameContext->IsNoteRefresh() )
	{
		ReceiveAllNote(topIdx);
		g_pGameContext->RemoveGameFlag( GAMEFLAG_NOTEREFRESH);
	}

	// ���� ����� Ȱ��ȭ/��Ȱ��ȭ
	{
		for( INT32 i=0 ; i<g_pGameContext->GetNoteReceiveCount(); i++)
		{
			if( m_bNoteCheck[i])
			{
				m_bIsSelectNote = TRUE;
				break;
			}
		}

		// üũ�ڽ��� ���õǾ����͵�Ӹ� �ƴ϶�, ���� �������� �����ִ� ������ ���ؼ��� Ȱ��ȭ/��Ȱ��ȭ�մϴ�.
		if( m_SelectedNoteIndex != -1) m_bIsSelectNote = TRUE;

		if( !m_bIsSelectNote)
		{
			m_pNoteDelete->setControlState( I3GUI_CONTROL_STATE_DISABLED);
		}
		else
		{
			if( m_SelectedNoteIndex != -1)
			{
				UINT32 TopIdx = m_pNoteList->GetCurrentTop();
				INT32 nListIdx = g_pGameContext->GetNoteReceiveCount() - (m_SelectedNoteIndex+1)-TopIdx;
				NOTE_RECEIVE * pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( nListIdx);

				if( pNote->header._type == NT_GIFT)
				{	//	���� ������ ���� �����ǹǷ� ������ ���� ������ �� ����.
					m_pNoteDelete->setControlState( I3GUI_CONTROL_STATE_DISABLED);
				}
				else
				{
					if( m_pNoteDelete->getControlState() == I3GUI_CONTROL_STATE_DISABLED)
					{
						m_pNoteDelete->setControlState( I3GUI_CONTROL_STATE_NORMAL);
					}
				}
			}
			else
			{
				if( m_pNoteDelete->getControlState() == I3GUI_CONTROL_STATE_DISABLED)
				{
					m_pNoteDelete->setControlState( I3GUI_CONTROL_STATE_NORMAL);
				}
			}
		}
	}
	
	// ���� note ��뷮
	m_fNoteEmptyRate = (REAL32)g_pGameContext->GetNoteReceiveCount();
	if( m_fNoteEmptyRate > 100.0f) m_fNoteEmptyRate = 100.0f;
	
	if( !m_bWarningNoteCount && m_pNoteSect->isEnable())
	{
		m_bWarningNoteCount = TRUE;

		if( m_fNoteEmptyRate >= 100.0f)
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_DELETE_FULL_NOTE"));/*�������� �뷮�� �����Ͽ� �� �̻� ������ ������ �� �����ϴ�.\n���ʿ��� ������ ������ �ֽʽÿ�.*/
		}
		else if( m_fNoteEmptyRate >= 80.0f)
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_DELETE_NEEDLESS_NOTE"));/*�������� �뷮�� �� ���� �ʾҽ��ϴ�.\n�������� �� ���� �Ǹ� ������ ������ �� ������\n���ʿ��� ������ ������ �ֽʽÿ�.*/
		}
	}

	// �ػ󵵿� ���� Offset��.
	REAL32 rateX = i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f;

	i3Sprite2D* pSprite = m_pNoteGage->getSprite();
	m_pNoteGage->setSize((INT32)((m_fNoteEmptyRate / 100.0f) * (147.0f*rateX)), (INT32)(m_pNoteGage->getHeight()));
	pSprite->SetRect(GUI_SHAPE_OFFSET, m_pNoteGage->getTotalPosX(), m_pNoteGage->getTotalPosY(), (m_fNoteEmptyRate / 100.0f) * (147.0f*rateX), (REAL32)m_pNoteGage->getHeight());

	sprintf_s( szTemp, "%d%%", (INT32)m_fNoteEmptyRate);
	m_pNoteGageBg->SetCaption( szTemp);
	
	if( m_nNoteType == NOTE_TYPE_SEND)
	{
		if(i3GuiRoot::getFocusControl() == m_pNoteWhoValue)
		{
			if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_TAB ) ||
				g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ) ||
				g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_NUMPADENTER ) )
			{
				m_pNoteValue->SetEnable( TRUE);
				m_pNoteValue->SetFocus( TRUE);
			}
		}
	}
}

void CPopupCommunity::StartAlarmNote(void)
{
	m_bAlarmCommunity = TRUE;

	// ���� ���â �÷��� Ȱ��ȭ
	m_bWarningNoteCount = FALSE;
}

void CPopupCommunity::EndAlarmNote(void)
{
	m_bAlarmCommunity = FALSE;

	// ���󺹱�
	m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
	m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
}

void CPopupCommunity::_UpdateAlarmNote(REAL32 rDeltaSeconds)
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
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
			}
			else
			{
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
			}
		}
	}
}

#ifdef USE_MESS_TEST
INT64 CPopupCommunity::_FindNoteByUid(char* nick)
{
	// �ڽ��� ���´�.
	if (g_pGameContext->IsMyNickName(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INPUT_SELF"));/*�ڽ��� �г����� �Է��ϼ̽��ϴ�.*/
		return 0;
	}

	NOTE_RECEIVE* pNote = NULL;
	pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( m_iListIdx);

	if (pNote && 0 != i3String::Compare(pNote->body._sender, nick))
	{
		NOTE_BODY note;
		i3String::Copy( note._sender, m_pNoteWhoValue->GetText(), NET_NICK_NAME_SIZE );
		i3String::Copy( note._message, m_pNoteValue->GetText(), NET_NOTE_SIZE );
		note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
		note._messageSize = (UINT8)(i3String::Length( note._message) + 1);
		g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
		return 0;
	}

	I3ASSERT(pNote != NULL);
	return pNote->header._suid;
}

#if defined(CHANGE_ADDCOMMAND_METHOD)
void CPopupCommunity::AddPopupCommand( ContextMenuCommand Cmd, const CUserLocationInfo & UserLocInfo, INT64 UID )
{
	//
	// �Լ� �� �ؿ��� �޴��� �߰��˴ϴ�.
	//

	switch(Cmd)
	{
	case CMC_TRANSFER_HOST:
		break;
	case CMC_KICK:
		break;
	case CMC_PROFILE:
		break;
	case CMC_WHISPER:
		{
			if( ! _CheckCanWhisper(UserLocInfo.GetState()))
				return;
		}
		break;
	case CMC_INVITE:
		{
			if ( UserLocInfo.IsMatch() )
				return;

			switch(m_currentTab)
			{
			case COMMUNITY_FRIEND_TAB:
				{
					if( TRUE == g_pFramework->GetClanContext()->InBattleTeam() )
						return;

					if (PHASE_READY_ROOM != ((CStageReady*)m_pParent)->GetCurrentPhaseType())
						return;

					if (g_pGameContext->IsSameRoom(UserLocInfo))
					{
						return;
					}
				}
				break;
			case COMMUNITY_CLAN_TAB:
				{
					if (PHASE_READY_ROOM != ((CStageReady*)m_pParent)->GetCurrentPhaseType() &&
						FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
					{
						return;
					}
				}
				break;
			}

		}
		break;
	case CMC_VISIT:
		{
			if (UserLocInfo.GetState() != FRIEND_STATE_ROOM)
			{
				if ( UserLocInfo.IsMatch() == FALSE )
					return;
				if ( TRUE ==  g_pFramework->GetClanContext()->InBattleTeam())
					return;
			}

			if (g_pGameContext->IsSameRoom(UserLocInfo))
			{
				return;
			}
		}
		break;
	case CMC_ADD_FRIEND:
		{
			if ( _IsMyFriend(UID) )
				return;
		}
		break;
	case CMC_ADD_BLOCK:
		break;
	case CMC_CLAN_INVITE:
		{
			if ( m_currentTab == COMMUNITY_CLAN_TAB )
				return;

			if ( ! CanClanInvite() )
				return;

			if ( !g_pFramework->GetClanContext()->IsPossibleClanInvite() )
				return;

			if (_IsMyClanMember( UID))
				return;
		}
		break;
	case CMC_VIEW_DETAIL:
		break;
	case CMC_VIEW_OTHER_USER_ITEM_INFO:
		break;
	default:
		break;
	}

	ContextMenuAddCommand(Cmd);
}


#endif //CHANGE_ADDCOMMAND_METHOD


BOOL	CPopupCommunity::GetCurSelUserLoccationInfo(CUserLocationInfo & UserLocInfo)
{
	if( g_pFramework->GetClanContext()->InBattleTeam() )
	{
		const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);
		if (!node)
			return FALSE;
		
		UserLocInfo = CUserLocationInfo(node->_state);
		return TRUE;
	}


	switch( m_currentTab )
	{
	case COMMUNITY_FRIEND_TAB:
		{
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);
			if (!node)
				return FALSE;

			UserLocInfo = CUserLocationInfo(node->_state);
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);
			if (!node)
				return FALSE;

			UserLocInfo = CUserLocationInfo(node->_state);
		}
		break;
	}


	return TRUE;
}

#endif




#include "pch.h"
#include "ReadyPhaseBase.h"
#include "GlobalVariables.h"
#include "StageGui/Popup/PopupUseItem.h"
#include "ShopDef.h"
#include "CommunityContext.h"

I3_ABSTRACT_CLASS_INSTANCE(CReadyPhaseBase, CPhaseBase);

CReadyPhaseBase::CReadyPhaseBase()
{
	InitStageLimitTimeList();
}

CReadyPhaseBase::~CReadyPhaseBase()
{
}

void CReadyPhaseBase::OnGameEvent(INT32 event,INT32 arg)
{
	CPhaseBase::OnGameEvent(event, arg);

	switch(event) 
	{
	case EVENT_ENTER_INVENTORY:
		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
		((CStageReady*)m_pParent)->ChangeStep(STEP_INVENTORY);
		i3GuiRoot::setGuiDisableInput(FALSE);

		g_pGameContext->BackupEquipment();
		break;
	case EVENT_LEAVE_INVENTORY:
		if (EV_SUCCESSED(arg))
		{
			// Do nothing
		}
		else
		{
			if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_EQUIP_USER_TITLE_NOT_ALLOWED))
			{
				STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_NO_EQUIP_PRE_DESIGNATION"));/*ȣĪ�� ȹ���ϱ����� ����� �� �����ϴ�.*/
			}
			else// if (EV_IS_ERROR(arg, EVENT_ERROR_SUCCESS))
			{
				STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_EQUIP_FAIL_BY_NETWORK_PROBLEM"));/*��Ʈ��ũ ������ ����� �� �����ϴ�.*/
			}
			
			UpdateCharacter(VT_DEFAULT);
		}

		((CStageReady*)m_pParent)->EnterDelayedEventStep();
		break;
	case EVENT_ITEM_AUTH:	// ������ ������ ����
	case EVENT_USE_ITEM_CHECK_NICK:
		{
			i3GuiRoot::setGuiDisableInput(FALSE);

			//	������ ���� (����� ���)
			((CStageReady*)m_pParent)->AuthStartSuccess(arg);

			//	ĳ�� ������ ��� â�� �̺�Ʈ�� ó��
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);
		}
		break;
	case EVENT_AUTH_DELETE:
		{
			((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);
		}
		break;

#if defined(ENABLE_MISSION_CARD)
	case EVENT_ENTER_INFO:
		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
		((CStageReady*)m_pParent)->ChangeStep(STEP_PROFILE);
		i3GuiRoot::setGuiDisableInput(FALSE);
		break;

	case EVENT_LEAVE_INFO:
		((CStageReady*)m_pParent)->EnterDelayedEventStep();
		break;
#endif

	case EVENT_ENTER_SHOP:	
		{
			g_pFramework->GetShopContext()->OnGameEvent(event, arg);
				
			((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
			((CStageReady*)m_pParent)->ChangeStep(STEP_SHOP);

			i3GuiRoot::setGuiDisableInput(FALSE);
		}
		break;
	case EVENT_LEAVE_SHOP:
		((CStageReady*)m_pParent)->EnterDelayedEventStep();
		break;
	case EVENT_REQUEST_GOODS_LIST:
	case EVENT_BUY_GOODS:
	case EVENT_AUTH_GIFT_GOODS:
	case EVENT_CHECK_GIFT_GOODS:
	case EVENT_ITEM_REPAIR:
		{
			g_pFramework->GetShopContext()->OnGameEvent(event, arg);
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);
			((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);
		}
		break;
	case EVENT_GET_GIFT:
		STRING_POPUP(GAME_STRING("STR_SHOP_INFORM_RECEIVE_GIFT"));	//������ �޾ҽ��ϴ�.\n ������ Ȯ�����ּ���.
		break;


	case EVENT_CLAN_ENTER:
		{
			StepType stepType = STEP_NULL;

			if(arg == 1)
			{
				stepType = STEP_CLAN_MEMBER;			//	Ŭ�� ������ 
			}
			else if(arg == 0)
			{
				stepType = STEP_CLAN_NON_MEMBER;		//	Ŭ�� �̰�����
			}

#if defined TEST_CLAN_MEMBER
			stepType = STEP_CLAN_MEMBER;			//	Ŭ�� ������ 
			arg = 1;
#endif

			g_pFramework->GetClanContext()->OnGameEvent(event, arg);

			((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
			
			if (((CStageReady*)m_pParent)->GetCurrentStepType() != stepType)
			{
				((CStageReady*)m_pParent)->ChangeStep(stepType);
			}
			
			i3GuiRoot::setGuiDisableInput(FALSE);
		}
		break;
	case EVENT_CLAN_LEAVE:
		// ��� Ŭ�� �޽����� ClanContext���� ó���ؾ��ϳ�,
		// �� ��� CStageReady�� ������ �� ����, ClanContext ���� ���� ó���Ѵ�.
		((CStageReady*)m_pParent)->EnterDelayedEventStep();		
		// Fall through
	case EVENT_CLAN_CREATE:
	case EVENT_CLAN_DUPLICATE_NAME:
	case EVENT_CLAN_DUPLICATE_ADDRESS:
	case EVENT_CLAN_DUPLICATE_MARK:
	case EVENT_CLAN_MENAGEMENT:
	case EVENT_CLAN_CHANGE_MARK:
	case EVENT_CLAN_JOIN_REQUEST:	
	case EVENT_CLAN_MEMBER_CONTEXT:
	case EVENT_CLAN_MEMBER_LIST:
	case EVENT_CLAN_REQUEST_CONTEXT:
	case EVENT_CLAN_ACCEPT_REQUEST:
	case EVENT_CLAN_DENIAL_REQUEST:	
	case EVENT_CLAN_DISMISSALMEMBER:
	case EVENT_CLAN_COMMISSIONSTAFF:
	case EVENT_CLAN_COMMISSION_REGULAR:
	case EVENT_CLAN_NOTICE_MODIFY:
	case EVENT_CLAN_GUIDE_MODIFY:
	case EVENT_MATCH_TEAM_CREATE:			// Ŭ���� �� �����
	case EVENT_MATCH_TEAM_JOIN:				// Ŭ���� �� ����
	case EVENT_MATCH_TEAM_LEAVE:			// Ŭ���� �� ������
	case EVENT_MATCH_TEAM_INFO_N:			// Ŭ���� �� ����
	case EVENT_MATCH_TEAM_CONTEXT:			// Ŭ���� ����� Context
	case EVENT_MATCH_TEAM_ALL_CONTEXT:		// Ŭ���� ����� Context
	case EVENT_MATCH_TEAM_LIST_N:			// Ŭ���� ����� �ޱ�
	case EVENT_MATCH_TEAM_ALL_LIST_N:		// Ŭ���� ��� �� ��� �ޱ�
	case EVENT_MATCH_TEAM_DETAIL_INFO:
	case EVENT_MATCH_FIGHT_QUICK_REQUEST:	// Ŭ���� ���� ��û
	case EVENT_MATCH_FIGHT_REQUEST:			// Ŭ���� ������û
	case EVENT_M_MATCH_FIGHT_RESULT:		// Ŭ���� ������û ����
	case EVENT_MATCH_FIGHT_ACCECT:			// Ŭ���� ������û ����
	case EVENT_M_MATCH_FIGHT_ACCECT_RESULT:
	case EVENT_MATCH_TEAM_CHANGE_PER:
	case EVENT_MATCH_TEAM_CHANGE_PER_RESULT_N:
	case EVENT_M_MATCH_FIGHT_CHANGE_INFO_RESULT:
	case EVENT_CLAN_LIST_CONTEXT:
	case EVENT_CLAN_CREATION_INFO:
	case EVENT_CLAN_PERSON_PLUS:
		{
			g_pFramework->GetClanContext()->OnGameEvent(event, arg);
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);
			((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);
		}
		break;		
	case EVENT_CLAN_JOINCHECK_AUTHORITY:
		{
			if( EV_SUCCESSED( arg ) )
			{
				INT32 nClanIdx = g_pFramework->GetClanContext()->GetJoinClanIdx();

				//	�����κ��� Ŭ�� ���� ������ �����Ǿ Ŭ�� ���� ��û���� ���ϴ�.
				((CStageReady*)m_pParent)->TogglePopup( POPUP_CLAN_CREATE, &nClanIdx);
			}
			else
			{
				switch( arg )
				{
				case EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL:
					CAPTION_POPUP( GAME_STRING("STR_TBL_GUI_BASE_CLAN_JOIN_ABORTED_BY_AGE"),
						GAME_STRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST"));/*���� ��û ����*//*�ش� Ŭ���� ������ �� �ִ� ���ɰ� ���� �ʾ� Ŭ�� ���� ��û�� ��� �Ǿ����ϴ�.*/
					break;
				case EVENT_ERROR_CLAN_JOIN_AUTH_RANK_FAIL:
					CAPTION_POPUP( GAME_STRING("STR_TBL_GUI_BASE_CLAN_JOIN_ABORTED_BY_GRADE"),
						GAME_STRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST"));/*���� ��û ����*//*�ش� Ŭ���� ������ �� �ִ� ��ް� ���� �ʾ� Ŭ�� ���� ��û�� ��� �Ǿ����ϴ�.*/
					break;
				default:
					{
						char szTemp[MAX_PATH];
						sprintf_s( szTemp, GAME_STRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST_BY_SYSTEM_PROBLEM"), arg );/*\n���� ��û�� �����Ͽ����ϴ�.\n �˼��մϴ�. �����ڿ��� �����Ͽ� �ֽʽÿ�.\n [Errcode : 0x%08x]*/

						CAPTION_POPUP( szTemp, GAME_STRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST"));/*���� ��û ����*/
					}
					break;
				}
			}
		}
		break;
	case EVENT_CLAN_DISSOLUTION:
	case EVENT_CLAN_SECESSION:
		{			
			g_pFramework->GetClanContext()->OnGameEvent(event, arg);
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);

			//	Ŭ�� ȸ�� Ż�� �����ϸ� �ٷ� Ŭ�� �̰����� �������� �Ѿ�ϴ�.
			if( EV_SUCCESSED(arg))
			{
				USER_INFO_BASIC		MyUserInfoBasic;
				g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
				MyUserInfoBasic._clanidx		= 0;
				MyUserInfoBasic._clanstate	= CLAN_MEMBER_LEVEL_UNKNOWN;
				g_pGameContext->SetMyUserInfoBasic(MyUserInfoBasic);

				g_pCommunity->ClearClanMember();
				((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
				((CStageReady*)m_pParent)->ChangeStep(STEP_NULL);
			}
		}
		break;
	case EVENT_CLAN_COMMISSIONMASTER:
		{
			g_pFramework->GetClanContext()->OnGameEvent(event, arg);
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);

			//	������������ �����ϸ� �ٷ� Ŭ�� �̰����� �������� �Ѿ�ϴ�.
			if( EV_SUCCESSED(arg))
			{
				((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
				((CStageReady*)m_pParent)->ChangeStep(STEP_CLAN_MEMBER);
			}
		}
		break;
	case EVENT_CLAN_CANCEL_REQUEST:
		{
			g_pFramework->GetClanContext()->OnGameEvent(event, arg);
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);

			if( EV_SUCCESSED(arg))
			{
				USER_INFO_BASIC		MyUserInfoBasic;
				g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
				MyUserInfoBasic._clanidx = 0;
				g_pGameContext->SetMyUserInfoBasic(MyUserInfoBasic);
			}
		}
		break;
	case EVENT_M_CLAN_MEMBER_INFO:
		{
			// �˶�
			((CStageReady*)m_pParent)->StartAlarmCommunity();
		}
		break;
	case EVENT_CLAN_MEMBER_INSERT:
		{
			// �˶�
			((CStageReady*)m_pParent)->StartAlarmCommunity();
		}
		break;
	case EVENT_CLAN_INVITE_ACCEPT_N:	//	Ŀ�´�Ƽ ������ Ŭ�� ������ �� ��� �̹� ���ԵǾ� �ִ� ���
		{	
			//STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_ALREADY_JOINED_STATE"));/*�̹� Ŭ���� ���ԵǾ� �ִ� �����Դϴ�.*/

			switch( arg ) 
			{
			case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
				{
					//	"\nŬ�� ���� ó���� �����߽��ϴ�.\nŬ���� ã���� �����ϴ�."
					//	Ŭ�� ���� ���� ����
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_NO_FINDING"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));	
				}
				break;
			case EVENT_ERROR_CLAN_NOMEMBER:
				{
					//	"\nŬ�� ���� ó���� �����߽��ϴ�.\nŬ������ �ƴմϴ�."
					//	Ŭ�� ���� ���� ����
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_NO_MEMBER"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_LEVEL_NOMASTER:
				{
					//	"\nŬ�� ���� ó���� �����߽��ϴ�.\nŬ�� �����Ͱ� �ƴմϴ�."
					//	Ŭ�� ���� ���� ����
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_NO_MASTER"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_MEMBER:
				{
					//	"\nŬ�� ���� ó���� �����߽��ϴ�.\n�̹� Ŭ���� ���ԵǾ��ֽ��ϴ�."
					//	Ŭ�� ���� ���� ����
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ALREADY_MEMBER"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_MAXMEMBER:
				{
					//	"\nŬ�� ���� ó���� �����߽��ϴ�.\n���̻� ���� �� �� �����ϴ�."
					//	Ŭ�� ���� ���� ����
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_OVER_MEMBER"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_SEARCH_REQUESTIDX:
				{
					//	"\n���� ��û �����Ͽ����ϴ�.\n�̹� �ٸ� Ŭ���� ���ԵǾ� �ְų� ���� ��û�� �����Դϴ�."
					//	Ŭ�� ���� ���� ����
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ALREADY_SUBSCRIPTION"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			default:
				{
					char szTemp[MAX_PATH];
					//	""\nŬ�� ���� ó���� �����߽��ϴ�.\n �˼��մϴ�. �����ڿ��� �����Ͽ� �ֽʽÿ�."
					sprintf_s( szTemp, "%s\n [Errcode : 0x%08x]", GAME_STRING("STBL_IDX_CLAN_MESSAGE_ASK_FOR_INFO"), arg );

					STRING_POPUP( szTemp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));	//	Ŭ�� ���� ���� ����
				}
				break;
			}

		}
		break;

	case EVENT_DELETE_NOTE_ASK:
		{
			PhaseType ePhaseType = ((CStageReady*)m_pParent)->GetCurrentPhaseType();
			if (PHASE_LOBBY == ePhaseType)
			{
				STRING_POPUP(GAME_STRING("STR_TBL_NETWORK_RECEIVE_CLAN_NOTE"));/*Ŭ�� ��ü ������ �����߽��ϴ�. �������� ������ ������ ������ ������ �����մϴ�.*/ 				
			}
		}
		break;
	case EVENT_RECEIVE_NOTE:
		{
			// ������ �˶�
			((CStageReady*)m_pParent)->StartAlarmNote();	
		}
		break;
	case EVENT_CLAN_NOTE:
		{
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);
		}
		break;
	case EVENT_INSERT_FRIEND:
		if (EV_SUCCESSED(arg))
		{
			INT32 realIdx = g_pCommunity->GetViewIndexOfRealFriend(arg);
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(realIdx);			

			if (node)
			{
				if (FRIEND_STATE_HOLD == GET_FRIEND_STATE(node->_state))
				{
					// ��밡 ��û�� ���
					((CStageReady*)m_pParent)->StartAlarmCommunity();
				}
				else
				{
					// �ڽ��� ��û�� ���
					char message[MAX_STRING_COUNT];
					sprintf_s(message, GAME_STRING("STR_TBL_GUI_BASE_NOTICE_ADD_FRIEND_COMPLETE"), node->_nick);/*[%s]���� ģ����Ͽ� �߰��Ǿ����ϴ�.*/
					STRING_POPUP(message);
				}
			}
			else
			{
				I3TRACE("[ERROR] EVENT_INSERT_FRIEND - Invalid friend idx(%d)\n", g_pCommunity->GetRealIndexOfViewFriend(arg));
			}
		}
		else
		{
			char message[MAX_STRING_COUNT] = {0, };

			switch(TESTBIT(arg, EVENT_ERROR_MASK_FAIL))
			{
			case EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER:
				i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST"), MAX_STRING_COUNT );//������ ã�� �� �����ϴ�.
				break;
			case EVENT_ERROR_EVENT_FRIEND_INSERT_FULL:
				if (FRIEND_MAX_COUNT <= g_pCommunity->GetFriendCount())
				{
					// �ڽ��� ģ���� �߰��� �� ����.
					i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_NO_MORE_GET_FRIEND_STATE"), MAX_STRING_COUNT);//���̻� ģ���� �߰��� �� �����ϴ�.
				}
				else
				{
					// ��밡 ģ���� �߰��� �� ����.
					i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_FAIL_REQUEST_FRIEND_BY_LIMIT"), MAX_STRING_COUNT);//������ �� �̻� ģ���� �߰��� �� ���� �����Դϴ�.
				}
				break;
			case EVENT_ERROR_EVENT_FRIEND_INSERT_QUERY:
				i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST"), MAX_STRING_COUNT);//������ ã�� �� �����ϴ�.
				break;
			case EVENT_ERROR_EVENT_FRIEND_INSERT_ALREADY_FRIEND:
				i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_ALREADY_REGIST_FRIEND_LIST"), MAX_STRING_COUNT);//�̹� ģ����Ͽ� ��ϵǾ� �ֽ��ϴ�.
				break;				
			case EVENT_ERROR_EVENT_FRIEND_INSERT_NOT_FIND_NICK:
				i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST"), MAX_STRING_COUNT);//������ ã�� �� �����ϴ�.
				break;
			default:
				i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST"), MAX_STRING_COUNT);//������ ã�� �� �����ϴ�.
				break;
			}

			STRING_POPUP(message);
		}
		break;
	case EVENT_ACCEPT_FRIEND:
		if (EV_SUCCESSED(arg))
		{
			// ģ�� ������ �����ϸ� ģ�� ����â�� �ݾ� ģ���� ���¸� ���� �� �� �ֵ��� �Ѵ�.
			g_pCommunity->SetCommunityUserInfo(FALSE);
		}
		else
		{
			// ��Ȳ�� ���� �˸��� �޽��� �ʿ���. ���� �޽����� �ʹ� ��������.
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_FAIL_REQUEST_FRIEND"));/*ģ����û������ �����Ͽ����ϴ�.*/
		}
		break;
	case EVENT_DELETE_FRIEND:
		if (EV_SUCCESSED(arg))
		{
			const FRIEND_NODE* node = g_pCommunity->GetLastDeleteFriend();

			char message[MAX_STRING_COUNT];
			sprintf_s(message, GAME_STRING("STR_TBL_GUI_BASE_NOTICE_DELETE_FRIEND_COMPLETE"), node->_nick);/*[%s]���� ģ�� ��Ͽ��� �����Ǿ����ϴ�.*/
			STRING_POPUP(message);
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_FAIL_DELETE_FRIEND"));/*ģ�������� �����Ͽ����ϴ�.*/
		}
		break;
	case EVENT_CLAN_INVITE:
		if (EV_SUCCESSED(arg))
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_COMPLETE_NORMAL"));/*���������� ó���Ǿ����ϴ�.*/
		}
		else if( EV_IS_ERROR( arg, EVENT_ERROR_CLAN_MEMBER ) ) 
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_ALREADY_JOINED"));/*�̹� Ŭ���� ������ �����Դϴ�.*/
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_FAIL_SEND_NOTE"));/*���� �����⿡ �����Ͽ����ϴ�.*/
		}
		break;
	case EVENT_QUEST_BUY_CARD_SET:	((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);	break;
	case EVENT_QUEST_DELETE_CARD_SET:	((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);	break;
	case EVENT_QUEST_CHANGE_ACTIVE_IDX:		break;

	case EVENT_GET_USERINFO :
	//// 1.5 Check Designation
	//case EVENT_DESIGNATION_GET :
	//case EVENT_DESIGNATION_CHANGE:
	//case EVENT_DESIGNATION_RELEASE:
		{	//	ȣĪ ����
			((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);
		}
		break;

	default:
		/* Do nothing */
		break;
	}
}

void CReadyPhaseBase::ApplyMsgReadyBox()
{
	if( g_pGameContext->EmptyReadyBoxMsg() )
		return;

	size_t size = g_pGameContext->GetReadyBoxMsg_Size();

	for(size_t i = 0; i<size; ++i)
	{
		char NameBuffer[MAX_STRING_COUNT] = {0};

		UINT32 ItemID = g_pGameContext->GetReadyBoxMsg_ItemID( i );
		g_pShopItemInfoDataBase->GetItemName(ItemID, NameBuffer, MAX_STRING_COUNT);

		//ä��â�� �ڶ��Ѵ�.
		//�˸� : {col:240, 30, 40, 255}%d{/col} ���� {col:240, 30, 40, 255}%s{/col} ���� {col:240, 30, 40, 255}%s{/col}�� ȹ���ϼ̽��ϴ�.
		char strBuf[MAX_STRING_COUNT];

		memset(strBuf, 0, sizeof(strBuf));
		_snprintf(strBuf, MAX_STRING_COUNT-1, 
			GAME_STRING("STR_READYBOX_COMMENT"), 
			g_pGameContext->GetReadyBoxMsg_RoomIdx( i ) + 1,
			g_pGameContext->GetReadyBoxMsg_Nick( i ).c_str(),
			NameBuffer	);

		//ä��â�� ���� ��� �ڶ��Ѵ�.
		g_pFramework->getChatBox()->PutSystemChatting( strBuf );
	}

	g_pGameContext->ClearReadyBoxMsg();
}

void CReadyPhaseBase::GetStageMapName(BOOL useRandomMap, STAGE_ID stageID,
									  std::string & strMapName)
{
#if defined (USE_RANDOMMAP_MODE)				
	if(useRandomMap)
	{
		strMapName = GAME_STRING("STR_POPUP_RANDOMMAP");
	}
	else
	{
		char MapName[MAX_STRING_COUNT] = "";
		getStageDisplayName(MapName, stageID);
		strMapName = MapName;
	}
#else
	char MapName[MAX_STRING_COUNT] = "";
	getStageDisplayName(MapName, stageID);
	strMapName = MapName;
#endif
}

void CReadyPhaseBase::GetStageModeName(STAGE_TYPE stageType,
								   std::string & strModeName, BOOL & bAllowChangeMode)
{
	strModeName = getStageTypeName(stageType);

	switch( stageType)
	{
	case STAGE_TYPE_DEATHMATCH:		//��ü��
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_HEAD_HUNTER:	//�������
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_HEAD_KILLER:	//���ų��
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_SUDDEN_DEATH:	//���絥��
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_BOMB:	//���Ĺ̼�
		bAllowChangeMode = FALSE;
		break;
	case STAGE_TYPE_DESTROY:	//�ı��̼�
		bAllowChangeMode = FALSE;
		break;
	case STAGE_TYPE_DEFENCE :	//���̼�
		bAllowChangeMode = FALSE;
		break;
	case STAGE_TYPE_ANNIHILATION:		//������
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_ESCAPE:	//Ż��̼�
		bAllowChangeMode = FALSE;
		break;
	case STAGE_TYPE_TUTORIAL:	//Ʃ�丮��
		bAllowChangeMode = FALSE;
		break;
	}
}

void CReadyPhaseBase::GetEnableMap(STAGEID_INFO_LIST::EnableStageList & EnableMap,
								   STAGE_TYPE StageType, 
								   V_STAGEOPTION StageOptionType)
{
	EnableMap.clear();

	if( StageOptionType == STAGE_OPTION_AI )
	{
		if( g_pGameContext->getAiMode() == AI_MODE_STATE_DIEHARD )
		{
			getChallengeDieHardStage( EnableMap );
		}
	}
	else
	{
		s_StageInfoTableList.GetEnableActiveStage(EnableMap, StageType, STAGE_OPTION_AI );
	}
}

void CReadyPhaseBase::InitEnableMapList(STAGE_TYPE type, i3GuiPopupList* m_pMapList)
{
	COLOR color;
	i3Color::Set(&color, (I3COLOR*)GameGUI::GameModeColor(type));

	V_STAGEOPTION option = g_pGameContext->GetStageOption();
	//������ �����Ѵ�.
	switch( option )
	{
	case STAGE_OPTION_AI:	
		i3Color::Set(&color, GetColor(GCT_TRAINING_MODE));		break;
	case STAGE_OPTION_KNUCKLE:	
		i3Color::Set(&color, GetColor(GCT_KNUCKLE_MODE));		break;
	case STAGE_TYPE_ESCAPE:	
		i3Color::Set(&color, GetColor(GCT_BOSS_MODE));	break;
	case STAGE_OPTION_SNIPER:	
		i3Color::Set(&color, GetColor(GCT_SNIPER_MODE));	break;
	case STAGE_OPTION_SHOTGUN:	
		break;
	case STAGE_OPTION_NONE:
		break;
	}
	s_StageInfoTableList.GetEnableActiveStage(m_EnableStageList, type, option );

	size_t size = m_EnableStageList.size();

	m_pMapList->Clear();
	m_pMapList->SetDisplayCount( size );
	m_pMapList->SetFixedElementCount(FALSE);

	for( size_t i = 0; i < size; ++i )
	{
		m_pMapList->AddElement( m_EnableStageList[i].first.c_str() );
		m_pMapList->SetTextColor(i, &color);
	}
}


void CReadyPhaseBase::InitStageLimitTimeList()
{
	std::vector< UINT > type1, type2, type3, type4, type5;

	type1.push_back( HINIBBLE(BATTLE_TIME_MISSION_TYPE_3) );
	type1.push_back( HINIBBLE(BATTLE_TIME_MISSION_TYPE_5) );
	type1.push_back( HINIBBLE(BATTLE_TIME_MISSION_TYPE_7) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_BOMB, type1) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_DESTROY, type1) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_ANNIHILATION, type1) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_SUDDEN_DEATH, type1) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_HEAD_KILLER, type1) );

	type2.push_back( HINIBBLE(BATTLE_TIME_MISSION_TYPE_3) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_DEFENCE, type2) );

	type3.push_back( HINIBBLE(BATTLE_TIME_TYPE_5) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_ESCAPE, type3) );

	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_3) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_5) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_10) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_15) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_20) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_25) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_30) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_DEATHMATCH, type4) );

	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_5) );
	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_10) );
	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_15) );
	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_20) );
	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_25) );
	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_30) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_HEAD_HUNTER, type5) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_MAX, type5) );	//default�� ��ü�ȴ�.
}

UINT8 CReadyPhaseBase::_RotateTimeIndex(STAGE_TYPE stageType, UINT8 subType, BOOL bNext)
{
	LimitTimeList::iterator It = m_LimitTimeList.find( stageType );

	if( It == m_LimitTimeList.end() )
	{
		It = m_LimitTimeList.find( STAGE_TYPE_MAX );
	}

	UINT8 gameTime = HINIBBLE(subType);
	UINT8 gameType = LONIBBLE(subType);

	size_t size= It->second.size();

	if( size > 1 )
	{
		for(size_t i = 0; i < size; ++i)
		{
			if( It->second[i] == gameTime )
			{
				if( i == 0 )
				{
					if (bNext)
						gameTime = (UINT8)It->second[ i + 1 ] ;
					else
						gameTime = (UINT8)It->second[ size - 1 ] ;
				}
				else if( i == size - 1 )
				{
					if (bNext)
						gameTime = (UINT8)It->second[ 0 ] ;
					else
						gameTime = (UINT8)It->second[ i - 1 ] ;
				}
				else
				{
					if (bNext)
						gameTime = (UINT8)It->second[ i + 1 ] ;
					else
						gameTime = (UINT8)It->second[ i - 1 ] ;
				}
				break;
			}
		}
	}

	return MAKEBYTE(gameTime, gameType);
}

void CReadyPhaseBase::createRandomMap(STAGE_TYPE type)
{
		g_RandomMap.CreateRandomMapList( type, g_pGameContext->GetStageOption() );
}
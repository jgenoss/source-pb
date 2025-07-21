#include "pch.h"
#include "ClanContext.h"

#include "GlobalVariables.h"
#include "ClanDef.h"
#include "ClanMark.h"
#include "ServerInfo.h"
#include "UI/UIMainFrame.h"
#include "UI/UIPhaseNonClanMember.h"
#include "UI/UIUtil.h"
#include "i3Base/itl/algorithm/sort.h"
#include "i3Base/string/ext/snprintf.h"
#include "ChannelContext.h"

#include "ClanGameContext.h"
#include "ClanSeasonContext.h"
#include "./StageBattle/NetworkContext.h"
#include "UserInfoContext.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"


#if legacy_gui_function
	#include "GuiNotifyReceiver.h"
#endif


namespace 
{
	enum CLAN_SORT_TYPE
	{
		CLAN_SORT_NONE = 0,	
		CLAN_SORT_CUR_SEASON,	// Ŭ�� ����Ʈ (���� ����)
		CLAN_SORT_PREV_SEASON,	// Ŭ�� ����Ʈ (���� ����)
		CLAN_SORT_TOTAL_RECORD,	// �� ����
		CLAN_SORT_MY_CLAN		// �� Ŭ�� ���� (�� ���� ���� ������ Ŭ�� 1����)
	};

	CLAN_SORT_TYPE g_sort_type = CLAN_SORT_NONE;

	bool sort_by_cur_season(const SORT_CLAN_INFO* pInfo1, const SORT_CLAN_INFO* pInfo2)
	{
		return pInfo1->_SortInfo.m_clanpoint > pInfo2->_SortInfo.m_clanpoint ;
	}

	bool sort_by_prev_season(const SORT_CLAN_INFO* pInfo1, const SORT_CLAN_INFO* pInfo2)
	{
		return pInfo1->_SortInfo.m_Preclanpoint > pInfo2->_SortInfo.m_Preclanpoint ;
	}

	bool sort_by_total_record(const SORT_CLAN_INFO* pInfo1, const SORT_CLAN_INFO* pInfo2)
	{
		return pInfo1->_RecordInfo.m_totalWin > pInfo2->_RecordInfo.m_totalWin;
	}

	bool sort_by_my_clan(const SORT_CLAN_INFO* pInfo1, const SORT_CLAN_INFO* pInfo2)
	{
		return pInfo1->_RecordInfo.m_totalWin > pInfo2->_RecordInfo.m_totalWin;
	}

	bool sort_by_clan_idx(const SORT_CLAN_INFO* pInfo1, const SORT_CLAN_INFO* pInfo2)
	{
		return pInfo1->_info._idx < pInfo2->_info._idx ;
	}

}

I3_CLASS_INSTANCE( CClanContext);//, i3GameNode);


CClanContext::CClanContext(void)
{
	m_bEnter = false;
	m_bMember = false;		//	Ŭ�����̸� true

	m_rUpdateClanMemberTime			= 0.0f; 
	m_nUpdateClanMemberSection		= 0; 
//	m_bListUpdateClanMemberFinish	= true; 

	m_rUpdateClanRequestTime		= 0.0f; 
	m_nUpdateClanRequestSection		= 0;
	m_bListUpdateClanRequestFinish	= true; 
	m_bInBattleTeam = false;
	m_bInClanBattleLobby = false;
	m_bVisibleLobbyPhase = true;

	m_nJoinClanIdx = 0;

	m_IndexBufferCount = 0;
	i3mem::FillZero(m_IndexBuffer, sizeof( m_IndexBuffer));

	m_nSortClanInfoCount		= 0;

	m_rUpdateClanListTime		= 0.f; 
	m_nUpdateClanListReq_BeginClanIdx	= 0;
	m_nUpdateClanListAck_BeginClanIdx = -1;
	m_nUpdateClanListAck_CurrRecvClanCount = 0;

	m_nUpdateClanListCount		= 0;
	m_nAppendClanListCount		= 0;
	m_bUpdateClanListFinish		= false;
	m_bNeedUpdateAppendClanList = true;

	m_HasClanCreationInfo	= false;
	m_bClanListUpdated			= false;

	m_ClanCreationRequirementPoint = 0;
	m_ClanCreationRequirementRank = 0;

	m_bEnableReservePhase = false;

	i3mem::FillZero(&m_ClanResultContext, sizeof(CLAN_MATCH_RESULT_CONTEXT));
	i3mem::FillZero(&m_ClanResultList, sizeof(CLAN_MATCH_RESULT) * MAX_MATCH_RESULT_COUNT);
	m_i32EmotionReqIndex = 0;
	m_rMatchListReqTime			= 0.0f; 
	m_ui8MatchListReqSection	= 0; 
	m_bMatchListReqFinish		= false;

	m_bIsClanVisit = false;
	m_bWaitMemberInfoReq = false;


	InitGameEventFunc();
}

CClanContext::~CClanContext(void)
{
	_ReleaseSortClanList();
}

void CClanContext::Create(void)
{

}
void CClanContext::SetEnter(bool bEnter)			
{
	m_bEnter = bEnter;		
}

void CClanContext::EnterServer(void * pArg)
{	

#if defined TEST_CLAN_MEMBER
	UINT16 year = 2007;
	UINT8 month = 10;
	UINT8 day = 1;	
	UINT32 date = ((year << 16) | (month << 8) | day);

	if( GetClanMemberCount() == 0)
	{
		for(INT32 i=0; i<16; i++)
		{
			char temp[256] = "";
			sprintf_s(temp, "TEST_%d", i);

			AddClanMember(temp, (UINT8)i, "TEST", date);

			AddClanSubscriber(temp, (UINT8)i, date);
		}
	}
#endif
	GameEventSender::i()->SetEvent(EVENT_CLAN_ENTER);

	m_nJoinClanIdx = 0;

	//m_bEnter = true;
}

void CClanContext::LeaveServer(void * pArg)
{
	GameEventSender::i()->SetEvent(EVENT_CLAN_LEAVE, pArg);

	ClearIndexBuffer();

	_ReleaseSortClanList();

	//m_bEnter = false;
}

void CClanContext::OnGameEvent(GAME_EVENT event,INT32 arg, const i3::user_data & userData)
{
	i3::unordered_map< GAME_EVENT, call_func >::iterator it;

	it = m_EventFuncMap.find( event);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, userData);
	}
}

void	CClanContext::SetMemberInfoChange( INT64 i64UID, UINT64 ui64State )
{
	if( ClanGameContext::i()->GetMyClanMemberMaxCount() <= 0 )
		return;

	for( INT32 i = 0 ; i < GetMyClanMemberCount() ; i++ )
	{
		const CLAN_MEMBER_BASIC_INFO * pInfo = ClanGameContext::i()->getMyClanMember(i);
		if ( pInfo == nullptr )
			continue;

		if( pInfo->_i64UID == i64UID )
		{
			CLAN_MEMBER_BASIC_INFO NewMemberInfo = *pInfo;
			NewMemberInfo._state = ui64State;

			ClanGameContext::i()->setMyClanMember(i, NewMemberInfo);
			return;
		}
	}
}

void	CClanContext::SetMemberBasicInfo( CLAN_MEMBER_BASIC_INFO* pMemberBasicInfo )
{
	I3ASSERT(pMemberBasicInfo != nullptr);
	if ( pMemberBasicInfo == nullptr )	return;
	if( ClanGameContext::i()->GetMyClanMemberMaxCount() <= 0 )
		return;

	for( INT32 i = 0 ; i < GetMyClanMemberCount() ; i++ )
	{
		const CLAN_MEMBER_BASIC_INFO * pInfo = ClanGameContext::i()->getMyClanMember(i);
		if ( pInfo == nullptr )
			continue;

		if( pInfo->_i64UID == pMemberBasicInfo->_i64UID )
		{
			ClanGameContext::i()->setMyClanMember(i, *pMemberBasicInfo);
			return;
		}
	}
}

bool CClanContext::IsMaster(void)
{
	if( UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_MASTER)
		return true;

	return false;
}

CLAN_MEMBER_LEVEL_TYPE CClanContext::GetMyStaffLevel(void)
{
	return (CLAN_MEMBER_LEVEL_TYPE) UserInfoContext::i()->GetMyClanState();
}

INT64 CClanContext::GetMyDBIdx(void)			
{ 
	return NetworkContext::i()->UID;		
}

#define LIST_UPDATE_TIME	3.0f 
// ��� ���� ó�� �ʿ��� 20080103 ���� 

//	Ŭ���� 
void CClanContext::EnterClanMember(void)
{
	GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_CONTEXT);

	//	üũ ����
	ClearIndexBuffer();
}

void CClanContext::RequestClanMember(void)
{
	m_rUpdateClanMemberTime			= LIST_UPDATE_TIME + FLT_MIN; // 1000.0f;  
	m_nUpdateClanMemberSection		= 0;  
//	m_bListUpdateClanMemberFinish	= false;  	

	//	üŷ ����Ʈ �ٽ� ����
	ClearIndexBuffer();
}

void CClanContext::UpdateClanMember(REAL32 rDeltaSeconds)
{
	CLAN_MEMBER_CONTEXT * pMemberContext = ClanGameContext::i()->getMyClanMemberContext(); 

	if ( m_nUpdateClanMemberSection < pMemberContext->_sectionCount )
	{
		m_rUpdateClanMemberTime += rDeltaSeconds;

		if( m_rUpdateClanMemberTime > LIST_UPDATE_TIME )
		{
			m_rUpdateClanMemberTime = 0.0f;
			GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_LIST_N, &m_nUpdateClanMemberSection);
			++m_nUpdateClanMemberSection;

			if (m_nUpdateClanMemberSection >= pMemberContext->_sectionCount)
				m_nUpdateClanMemberSection = UCHAR_MAX;
		}
	}
}

//	���� �����
void CClanContext::EnterClanSubscriber(void)
{
	GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_CONTEXT);

	ClearIndexBuffer();
}

void CClanContext::RequestClanSubscriber(void)
{
	m_rUpdateClanRequestTime		= 1000.0f; 
	m_nUpdateClanRequestSection		= 0; 
	m_bListUpdateClanRequestFinish	= false; 

	//	üŷ ����Ʈ �ٽ� ����
	ClearIndexBuffer();
}

void CClanContext::UpdateClanSubscriber(REAL32 rDeltaSeconds)
{
	if( !m_bListUpdateClanRequestFinish )
		m_rUpdateClanRequestTime += rDeltaSeconds;

	if( m_rUpdateClanRequestTime > LIST_UPDATE_TIME )
	{
		m_rUpdateClanRequestTime = 0.0f;

		PACKET_CS_NEW_REQUEST_LIST_REQ ReqPacket;

		ReqPacket.m_ui8Section = m_nUpdateClanRequestSection;

		GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_LIST_N, &ReqPacket);			

		CLAN_REQUEST_CONTEXT * pClanRequestContext = ClanGameContext::i()->getMyClanRequestContext();
		m_nUpdateClanRequestSection++;
		if(m_nUpdateClanRequestSection >= pClanRequestContext->_sectionCount)
			m_bListUpdateClanRequestFinish = true;
	}
}

void CClanContext::RequsetSubscriberInfo(T_UID dbIdx)
{
	//	������ ���� �䱸
	GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_INFO_N, &dbIdx);	
}

bool CClanContext::ProcessSecessionFromClan(void)
{
	//	"Ŭ���� ���� Ż�� �Ͻðڽ��ϱ�?"
	GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_SECESSION_Q"), nullptr , MAKE_CALLBACK(CbSecessionFromClan), this);

	return true;
}

bool CClanContext::ProcessAppointMember(INT64 * pdbIdx, INT32 count)
{ 
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		//
		//	"Ŭ�� ��� ������ �Ӹ�"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_APPOINT_MEMBER"));

		return false;
	}
	else
	{
		//
		//	"�ش� ����� �������� �Ӹ��ϰڽ��ϱ�?"
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_APPOINT_MEMBER_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_APPOINT_MEMBER"), MAKE_CALLBACK(CbAppointMember), this);

		return true;
	}
}

bool CClanContext::ProcessDemoteMember(INT64 * pdbIdx, INT32 count)
{	// Ŭ�� ���ǿ��� �Ϲ����� ����
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		// "�ּ��� �Ѹ� �̻��� ������ �����ؾ� �մϴ�."
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DEMOTE_MEMBER"));

		return false;
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DEMOTE_MEMBER_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DEMOTE_MEMBER"), MAKE_CALLBACK(CbDemoteMember), this);

		return true;
	}
}

bool CClanContext::ProcessCommissionMaster(INT64 * pdbIdx, INT32 count)
{
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	bool is_use_clan_xp_plus_150 = CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_CLAN_XP_PLUS_150);
	bool is_use_color_clan = CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_COLOR_CLAN);
	if( is_use_clan_xp_plus_150 || is_use_color_clan )
	{
		// ������� ������
	}

	if( count == 0)
	{
		//	"�ּ��� �Ѹ��̻��� ����� �����ؾ� �մϴ�."
		//	"Ŭ�� ������ ����"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISSION_MASTER"));
	}
	else if( count != 1)
	{
		//	"���� �Ѹ��� ����� ������ �� �ֽ��ϴ�.
		//	"Ŭ�� ������ ����"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISSION_MASTER_ONLY"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISSION_MASTER"));
	}
	else
	{
		i3::rc_wstring wstrTemp;

		const CLAN_MEMBER_BASIC_INFO * pMember = FindMyClanMember(m_IndexBuffer[0]);
		I3ASSERT( pMember != nullptr);

		if (pMember != nullptr)
		{
			//	"�ش� ������� �����͸� �����ϰڽ��ϱ�?" (����ȯ ������.) 2014.07.10.����.
			i3::sprintf(wstrTemp, L"[%s] \n \n %s", pMember->_nick, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISSION_MASTER_Q"));
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISSION_MASTER"), MAKE_CALLBACK(CbCommissionMaster), this);	//	"Ŭ�� ������ ����"
		}
		else
		{
			//	"Ŭ�� ������ ������ �����߽��ϴ�."
			//	"Ŭ�� ������ ���� ����"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_MASTER_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_MASTER_FAIL"));
			return false;
		}		

		return true;
	}

	return false;
}

bool CClanContext::ProcessDismissalMember(INT64 * pdbIdx, INT32 count)
{
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		//	"�ּ��� �Ѹ��̻��� ����� �����ؾ� �մϴ�."
		//	"Ŭ�� ��� ����"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_MEMBER"));
	}
	else if( count > MAX_MEMBER_COUNT)
	{
		//	"������ �� �ִ� �ִ� ��� �׸��� �ʰ��߽��ϴ�."
		//	"Ŭ�� ��� ����"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISMISSAL_MEMBER_OVERFLOW"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_MEMBER"));
	}
	else
	{
		//	"���õ� ����� ���� Ŭ������ ���� ��ŵ�ϱ�?"
		//	"Ŭ�� ��� ����"	
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISMISSAL_MEMBER_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_MEMBER"), MAKE_CALLBACK(CbDismissalMember), this);
		return true;
	}

	return false;
}

bool CClanContext::ProcessViewMember(INT64 * pdbIdx, INT32 count)
{
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		//	"�ּ��� �Ѹ� �̻��� ������ �����ؾ� �մϴ�."
		//	"�� ����"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_VIEW_MEMBER"));

		return false;
	}
	else
	{
		return true;
	}
}

bool CClanContext::ProcessIntromissionMember(INT64 * pdbIdx, INT32 count)
{	
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		//	"�ּ��� �Ѹ� �̻��� ������ �����ؾ� �մϴ�."
		//	"Ŭ�� ���� ����"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"),  &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_INTROMISSION_MEMBER"));

		return false;
	}
	else
	{
		CLAN_DETAIL_INFO * pClanInfo = GetMyClanDetailInfo();
		I3ASSERT( pClanInfo != nullptr);

		//	���� �ο� �ʰ�
		if(pClanInfo->m_maxPerson < pClanInfo->m_currentPerson + m_IndexBufferCount)
		{
			//	"������ ������ �� �����ϴ�.\nŬ�� ���� ��� �ο��� �ʰ��Ͽ����ϴ�."
			//	"Ŭ�� ���� ����"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_INTROMISSION_MEMBER_OVERFLOW"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_INTROMISSION_MEMBER"));
		}
		else
		{
			/*char temp[256] = "";
			char message[1024] = "";

			UINT8 * pIdx = GetSlotCheckedIndexArray();

			//	���õ� ��� �ɹ� �г����� ǥ���Ѵ�. ������ �� �ִ� �ɹ����� ������ ���ٸ� ���� ���ۿ� ������ ���� �� �ֽ��ϴ�. komet
			for(INT32 i=0; i<GetSlotCheckedIndexCount(); i++)
			{
			CLAN_REQUEST_BASIC_INFO * pSubscriber = GetMyClanSubscriber(*pIdx + i);
			I3ASSERT( pSubscriber != nullptr);

			sprintf_s(temp, "[%s] \n ",  pSubscriber->_nick);
			i3::generic_string_cat(message, temp);			
			}

			i3::generic_string_cat(message, GAME_RCSTRING("STBL_IDX_CLAN_Q_MEMBER_ADMISSION"));//\n �ش� ������ Ŭ�� ������ �����Ͻðڽ��ϱ�?*/

			//	"�ش� ������ Ŭ�� ������ �����Ͻðڽ��ϱ�?"
			//	"Ŭ�� ���� ����"	
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_INTROMISSION_MEMBER_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_INTROMISSION_MEMBER"), MAKE_CALLBACK(CbIntromissionMember), this);
		}

		return true;
	}
}

bool CClanContext::ProcessRejectionMember(INT64 * pdbIdx, INT32 count)
{
	I3ASSERT( pdbIdx != nullptr);

	CopyIndexBuffer(pdbIdx, count);

	if( count == 0)
	{
		//	"�ּ��� �Ѹ��̻��� ������ �����ؾ� �մϴ�."
		//	"Ŭ�� ���� �ź�"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_ONE"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_REJECTION_MEMBER"));

		return false;
	}
	else
	{
		/*char temp[256] = "";
		char message[1024] = "";

		UINT8 * pIdx = GetSlotCheckedIndexArray();

		//	���õ� ��� �ɹ� �г����� ǥ���Ѵ�. ������ �� �ִ� �ɹ����� ������ ���ٸ� ���� ���ۿ� ������ ���� �� �ֽ��ϴ�. komet
		for(INT32 i=0; i<GetSlotCheckedIndexCount(); i++)
		{
		CLAN_REQUEST_BASIC_INFO * pSubscriber = GetMyClanSubscriber(*pIdx + i);
		I3ASSERT( pSubscriber != nullptr);

		sprintf_s(temp, "[%s] \n ",  pSubscriber->_nick);
		i3::generic_string_cat(message, temp);			
		}

		i3::generic_string_cat(message, GAME_RCSTRING("STBL_IDX_CLAN_Q_MEMBER_REJECTION"));//\n �ش� ������ Ŭ�� ������ �ź��Ͻðڽ��ϱ�?*/

		//	"�ش� ������ Ŭ�� ������ �ź��Ͻðڽ��ϱ�?"
		//	"Ŭ�� ���� �ź�"
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_REJECTION_MEMBER_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_REJECTION_MEMBER"), MAKE_CALLBACK(CbRejectionMember), this);

		return true;
	}
}

bool CClanContext::ProcessChangeClanName(INT64 authFlag, const i3::rc_wstring & clan_name)
{
	m_wstrClanName = clan_name;

	const i3::rc_wstring& result_str = clan_name;

	UINT8 ui8Size = (UINT8)(result_str.size()+1)*sizeof(TTCHAR);
	
	GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH_DATA, &authFlag, &ui8Size,  result_str.c_str());
	
	return true;	
}

bool CClanContext::ProcessChangeClanMark(INT64 authFlag, UINT32 nClanMarkFlag)
{
	UINT8 ui8Size = sizeof(UINT32);
	GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH_DATA, &authFlag, &ui8Size, &nClanMarkFlag);

	return true;	
}

bool CClanContext::ProcessChangeClanMark(UINT32 nClanMarkFlag)
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_CHANGE_MARK, &nClanMarkFlag );

	return true;
}

bool CClanContext::ProcessDissolutionClan(void)
{
	CLAN_DETAIL_INFO * pClanInfo = GetMyClanDetailInfo();
	I3ASSERT( pClanInfo != nullptr);

	//	�ڱ�ܿ� �ٸ� Ŭ�� �ɹ��� ���� �ִٸ� Ŭ���� ��ü�� �� ����.
	if( pClanInfo->m_currentPerson > 1)	
	{
		//	"Ŭ�� ����� ���� ���·δ� Ŭ�� ��ü�� �Ұ��� �մϴ�."                                                                                                               
		//	"Ŭ�� ��ü"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_REMAIN_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION"));
	}
	else
	{
		//	"���� Ŭ���� ��ü�Ͻðڽ��ϱ�?"
		//	"Ŭ�� ��ü"
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_Q"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION"), MAKE_CALLBACK(CbDissolutionClan), this);

		return true;
	}

	return false;
}

bool CClanContext::ProcessManagementModify(CLAN_MANAGEMENT * pModify)
{
	if( pModify == nullptr) return false;

	GameEventSender::i()->SetEvent(EVENT_CLAN_MANAGEMENT, pModify );

	return true;
}

bool CClanContext::SendCreateClan(const i3::wliteral_range& wClanNameRng, const i3::wliteral_range& wIntroRng, const i3::wliteral_range& wURLRng, UINT32 nClanMarkFlag)
{
	// xgkick_thai
	// Ŭ�� ������ �ùٸ��� ���� �ʴ� �κ� ����. (����Ʈ URL ����)
#if !defined (LOCALE_KOREA)
	if( i3::generic_string_size(wClanNameRng) == 0 )  
	{
		return false;
	}
#else
/*  Ŭ�� URL ���� �����Ƿ� ���� : 
	if( pszClanName == nullptr || i3::generic_string_size(pszClanName) == 0 ||
		pszURL == nullptr || i3::generic_string_size(pszURL) == 0 || pszIntro == nullptr)  
*/
	if( i3::generic_string_size(wClanNameRng) == 0 )  
	{
		return false;
	}
#endif

#ifdef CLAN_ADDRESS_ENABLE
	if( i3::generic_string_size(wURLRng) == 0)		return false;
#endif

	PACKET_CS_NEW_CREATE_CLAN_REQ info;
	i3mem::FillZero( &info, sizeof( PACKET_CS_NEW_CREATE_CLAN_REQ));

	info.m_ui32ClanMark = nClanMarkFlag;

	i3::safe_string_copy(info.m_strClanName, wClanNameRng, NET_CLAN_NAME_SIZE);
	i3::safe_string_copy(info.m_strClanIntro, wIntroRng, NET_CLAN_INTRO_SIZE);
	i3::safe_string_copy(info.m_strClanAzit, wURLRng, NET_CLAN_AZIT_URL_SIZE);

	GameEventSender::i()->SetEvent( EVENT_CLAN_CREATE, &info );

	return true;
}

bool CClanContext::SendOverlappingClanMark(UINT32 nClanMarkFlag)
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_DUPLICATE_MARK, &nClanMarkFlag );

	return true;
}

bool CClanContext::SendOverlappingClanName(const i3::wliteral_range& wTextRng)
{
	if( i3::generic_string_size(wTextRng) == 0) return false;

	//i3::safe_string_copy(m_szName,sizeof(m_szName),pszText);
	size_t size = wTextRng.size();

	GameEventSender::i()->SetEvent( EVENT_CLAN_DUPLICATE_NAME, wTextRng.c_str(), &size );		// TODO : ����ȯ �䱸�� �����ؾ��Ѵ�... 

	return true;
}

bool CClanContext::SendOverlappingClanAddress(const i3::wliteral_range& wTextRng)
{
	if( i3::generic_string_size(wTextRng) == 0) return false;

	GameEventSender::i()->SetEvent( EVENT_CLAN_DUPLICATE_ADDRESS, wTextRng.c_str());	// TODO : ����ȯ �䱸�� �����ؾ��Ѵ�... 	

	return true;
}

bool CClanContext::SendRequestJoining(UINT32 nClanIdx)
{
	m_nJoinClanIdx = nClanIdx;

	GameEventSender::i()->SetEvent(EVENT_CLAN_JOINCHECK_AUTHORITY, &nClanIdx );

	return true;
}

bool CClanContext::SendJoinToClanMember(UINT32 nClanIdx, const i3::rc_wstring& wstrMessage )
{

	GameEventSender::i()->SetEvent( EVENT_CLAN_JOIN_REQUEST, &nClanIdx, wstrMessage.c_str() );		

	return true;
}

bool CClanContext::SendCancelJoinFromClanMember()
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_CANCEL_REQUEST );

	return true;
}


void CClanContext::SendClanInvite()
{
	// revision 54949 ����ã��� Ŭ���ʴ� ���� �ʴ� ���� ����(hansoft. 2929, 4388)

	GameEventSender::i()->SetEvent( EVENT_CLAN_INVITE);	
}

void CClanContext::SendClanInviteAccept( UINT32 wClanDBIdx, UINT8 cAccept )
{
	INT32 nServer	= g_pServerInfo->GetCurGameServerIndex();

	GameEventSender::i()->SetEvent( EVENT_CLAN_INVITE_ACCEPT_N, &wClanDBIdx, &nServer, &cAccept );	
}

void CClanContext::SendClanNote( UINT8 cType, const i3::wliteral_range& wNoteRng )
{
	UINT8 cSize = (UINT8)i3::generic_string_size( wNoteRng );
	GameEventSender::i()->SetEvent( EVENT_CLAN_NOTE, &cType, &cSize, wNoteRng.c_str() );
}

void CClanContext::SendClanDetailInfoNEvent(UINT32 ui32ClanUID, UINT8 uiClanType)
{
	PACKET_CS_NEW_DETAIL_INFO_REQ ReqPacket;

	ReqPacket.m_ui32CUID = ui32ClanUID;
	ReqPacket.m_ui8ClanType = uiClanType; // 0 == MyDetail , 1 == Detail

	GameEventSender::i()->SetEvent( EVENT_CLAN_DETAIL_INFO_N, &ReqPacket);
}

void CClanContext::CbSecessionFromClan(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		GameEventSender::i()->SetEventOnce(EVENT_CLAN_SECESSION);
	}
}

void CClanContext::CbAppointMember(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		UINT8 userCount = 0;

		CClanContext * pContext = (CClanContext*) pThis;

		for( UINT8 i = 0 ; i < pContext->GetIndexBufferCount(); i++ ) 
		{
			INT64 dbIdx = pContext->GetIndexBuffer(i);

			const CLAN_MEMBER_BASIC_INFO * pMember = pContext->FindMyClanMember( dbIdx );
			I3ASSERT(pMember != nullptr);

			if( pMember == nullptr || pMember->_level != CLAN_MEMBER_LEVEL_REGULAR )
				continue;

			userCount++;
		}

		if( userCount == 0 )
		{
			//	"�̹� �������� �Ӹ�� ����Դϴ�."
			//	"������ �Ӹ� ����"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_STAFF_ALREADY"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_STAFF_FAIL"));
			return;
		}

		INT32 count = pContext->GetIndexBufferCount();

		GameEventSender::i()->SetEvent(EVENT_CLAN_COMMISSIONSTAFF, &count, pContext->GetIndexBuffer() );	
		pContext->SetWaitMemberinfoReq(true);
	}
}

void CClanContext::CbDemoteMember(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		UINT8 userCount = 0;

		CClanContext * pContext = (CClanContext*) pThis;

		for( UINT8 i = 0 ; i < pContext->GetIndexBufferCount(); i++ ) 
		{
			INT64 dbIdx = pContext->GetIndexBuffer(i);

			const CLAN_MEMBER_BASIC_INFO * pMember = pContext->FindMyClanMember( dbIdx );
			I3ASSERT(pMember != nullptr);

			if( pMember == nullptr || pMember->_level != CLAN_MEMBER_LEVEL_STAFF )
				continue;

			userCount++;
		}

		if( userCount == 0 )
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_REGULAR_ALREADY"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_REGULAR_FAIL"));
			return;
		}

		INT32 count = pContext->GetIndexBufferCount();

		GameEventSender::i()->SetEvent(EVENT_CLAN_COMMISSION_REGULAR, &count, pContext->GetIndexBuffer() );	
		pContext->SetWaitMemberinfoReq(true);
	}
}

void CClanContext::CbCommissionMaster(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		CClanContext * pContext = (CClanContext*) pThis;

		//	������ ������ ���� �Ѹ��Դϴ�.
		if( pContext->GetIndexBufferCount() > 1 || pContext->GetIndexBufferCount() == 0)
			return;

		INT64 dbIdx = pContext->GetIndexBuffer(0);

		const CLAN_MEMBER_BASIC_INFO * pMember = pContext->FindMyClanMember(dbIdx);
		I3ASSERT( pMember != nullptr);

		GameEventSender::i()->SetEvent( EVENT_CLAN_COMMISSIONMASTER, &pMember->_i64UID );
		pContext->SetWaitMemberinfoReq(true);
	}
}

void CClanContext::CbDismissalMember(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		CClanContext * pContext = (CClanContext*) pThis;
		I3ASSERT( i3::kind_of<CClanContext*>(pContext));

		//	�ɹ��� �������� �� �ִ�.
		for( UINT8 i = 0 ; i < pContext->GetIndexBufferCount() ; i++ ) 
		{
			INT64 dbIdx = pContext->GetIndexBuffer(i);

			if( dbIdx == NetworkContext::i()->UID) 
			{
				//	"�ڱ� �ڽ��� ���� ��ų �� �����ϴ�."
				//	"Ŭ�� ��� ���� ����"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISMISSAL_MYSELF"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_FAIL"));
				return;
			}
		}

		INT32 count = pContext->GetIndexBufferCount();
		GameEventSender::i()->SetEvent(EVENT_CLAN_DISMISSALMEMBER, &count, pContext->GetIndexBuffer() );	
		pContext->SetWaitMemberinfoReq(true);
	}
}	

void CClanContext::CbIntromissionMember(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		CClanContext * pContext = (CClanContext*) pThis; 
		I3ASSERT( i3::kind_of<CClanContext*>(pContext));

		INT32 count = pContext->GetIndexBufferCount();

		//	���� ����
		GameEventSender::i()->SetEvent(EVENT_CLAN_ACCEPT_REQUEST, &count, pContext->GetIndexBuffer() );	
		pContext->SetWaitMemberinfoReq(true);
	}
}	

void CClanContext::CbRejectionMember(void* pThis,INT32 nParam)
{
	I3ASSERT( pThis != nullptr);

	if( nParam == MBR_OK)
	{
		CClanContext * pContext = (CClanContext*) pThis;
		I3ASSERT( i3::kind_of<CClanContext*>(pContext));

		INT32 count = pContext->GetIndexBufferCount();

		//	���� ����
		GameEventSender::i()->SetEvent(EVENT_CLAN_DENIAL_REQUEST, &count, pContext->GetIndexBuffer() );
		pContext->SetWaitMemberinfoReq(true);
	}
}

void CClanContext::CbDissolutionClan(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		//I3ASSERT( pThis != nullptr);

		//CClanContext * pContext = (CClanContext*) pThis;
		GameEventSender::i()->SetEvent(EVENT_CLAN_DISSOLUTION);
	}
}

// �������� ����
void CClanContext::BoardNoticeModify( const i3::wliteral_range& wNoticeRng )
{
	UINT8 wLenght = (UINT8)i3::generic_string_size( wNoticeRng );
	GameEventSender::i()->SetEvent(EVENT_CLAN_NOTICE_MODIFY, &wLenght, wNoticeRng.c_str() );
	SetWaitMemberinfoReq(true);
}

// Ŭ���Ұ� ����
void CClanContext::BoardGuideModify( const i3::wliteral_range& wGuideRng )
{
	UINT8 wLenght = (UINT8)i3::generic_string_size( wGuideRng );

	GameEventSender::i()->SetEvent(EVENT_CLAN_GUIDE_MODIFY, &wLenght, wGuideRng.c_str() );
	SetWaitMemberinfoReq(true);
}

// Ŭ�� ���� ���� ���� ����
void CClanContext::JoinOptionModify( UINT32 ui32State )
{
	PACKET_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ ReqPacket;

	ReqPacket.m_ui32AutoAcceptOption = ui32State;

	GameEventSender::i()->SetEvent(EVENT_CLAN_ACCEPT_OPTION_MODIFY, &ReqPacket);
	SetWaitMemberinfoReq(true);
}

CLAN_DETAIL_INFO * CClanContext::GetMyClanDetailInfo(void)
{ 
	return ClanGameContext::i()->getMyClanDetailInfo();			
}

CLAN_BASIC_INFO * CClanContext::GetMyClanBasicInfo(void)
{ 
	return ClanGameContext::i()->getMyClanInfo();				
}

CLAN_MEMBER_CONTEXT * CClanContext::GetMyClanMemberContext(void)
{
	return ClanGameContext::i()->getMyClanMemberContext();
}


const CLAN_MEMBER_BASIC_INFO * CClanContext::GetMyClanMember(INT32 idx)
{
	INT32 count = GetMyClanMemberCount();

	if( count == 0) return nullptr;

	I3_BOUNDCHK(idx, count);

	return ClanGameContext::i()->getMyClanMember(idx);
}

INT32 CClanContext::GetMyClanMemberCount(void)
{
	return (INT32)GetMyClanMemberContext()->_totalCount;
}

const CLAN_MEMBER_BASIC_INFO * CClanContext::FindMyClanMember(INT64 dbIdx)
{
	for(INT32 i=0; i<GetMyClanMemberCount(); i++)
	{
		const CLAN_MEMBER_BASIC_INFO * pMember = GetMyClanMember(i);

		if( pMember->_i64UID == dbIdx)
			return pMember;
	}

	return nullptr;
}

CLAN_REQUEST_CONTEXT* CClanContext::GetMyClanSubscriberContext(void)
{
	return ClanGameContext::i()->getMyClanRequestContext();
}

CLAN_REQUEST_BASIC_INFO* CClanContext::GetMyClanSubscriberList(void)
{
	return ClanGameContext::i()->getMyClanRequest();
}

CLAN_REQUEST_BASIC_INFO * CClanContext::GetMyClanSubscriber(INT32 idx)
{
	INT32 count = GetMyClanSubscriberCount();

	if( count == 0) return nullptr;

	I3_BOUNDCHK(idx, count);

	return GetMyClanSubscriberList() + idx;
}

CLAN_REQUEST_DETAIL_INFO* CClanContext::GetMyClanSubscriberDetail(void)
{
	return ClanGameContext::i()->getClanRequestDetailInfo();
}

INT32 CClanContext::GetMyClanSubscriberCount(void)
{
	//return (INT32) GetMyClanSubscriberContext()->_sectionCount * GetMyClanSubscriberContext()->_sectionSize;
	return (INT32)GetMyClanSubscriberContext()->_totalCount;
}

void CClanContext::ClearIndexBuffer(void)
{
	m_IndexBufferCount = 0;

	i3mem::FillZero(m_IndexBuffer, sizeof( m_IndexBuffer));
}

void CClanContext::CopyIndexBuffer(INT64 * pIdxArray, INT32 count)
{
	I3ASSERT( pIdxArray != nullptr);
	I3_BOUNDCHK( count, MAX_ARRAY_COUNT);

	i3mem::FillZero(m_IndexBuffer, sizeof( m_IndexBuffer));
	m_IndexBufferCount = count;

	if( count > 0)
	{	
		i3mem::Copy(&m_IndexBuffer, pIdxArray, sizeof(INT64) * count);	
	}
}

bool	CClanContext::CheckMenagementAuth( CLAN_MEMBER_LEVEL_TYPE eEnumType, CLAN_MENAGE_AUTHORITY_TYPE eMenageType )
{
	if( CLAN_MEMBER_LEVEL_MASTER == eEnumType )		return true;
	if( CLAN_MEMBER_LEVEL_STAFF != eEnumType )		return false;

	switch( eMenageType )
	{
	case CLAN_MENAGE_TYPE_WRITE:
		if(GetMyClanDetailInfo()->m_authstaff & CLAN_STAFF_AUTH_BOARD)		return true;
		break;
	case CLAN_MENAGE_TYPE_INVITED:
		if(GetMyClanDetailInfo()->m_authstaff & CLAN_STAFF_AUTH_INVITE)		return true;
		break;
	case CLAN_MENAGE_TYPE_DEPORTATION:
		if(GetMyClanDetailInfo()->m_authstaff & CLAN_STAFF_AUTH_EJECT)	return true;
		break;
	case CLAN_MENAGE_TYPE_ACCEPT:
		if(GetMyClanDetailInfo()->m_authstaff & CLAN_STAFF_AUTH_JOIN)		return true;
		break;
	}
	return false;
}

// Ŭ�� ��Ʋ �� ����Ʈ �ޱ�
void	CClanContext::MatchTeamList( void )
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_GET_TEAM_LIST );
}

// ��Ʋ �� ����
void	CClanContext::MatchTeamCreate( UINT8 cPerCount )
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_CREATE_TEAM, &cPerCount );
}

// ��Ʋ �� ����
void	CClanContext::MatchTeamJoin( UINT32 wTeamIdx )
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_JOIN_TEAM, &wTeamIdx );
}

// ��Ʋ �� ������
void	CClanContext::MatchTeamLeave( void )
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_LEAVE_TEAM );
}

// ������ ����
void	CClanContext::MatchTeamChangeOperation( const i3::rc_wstring & strOperation)
{
	GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_CHANGE_OPERATION, &strOperation);
}

// ��Ʋ �� �ο��� ����
void	CClanContext::MatchTeamChangeMaxPerson( UINT8 ui8MaxPerson )
{
	GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_CHANGE_MAX_PERSON, &ui8MaxPerson );
}

//	��ġ����ŷ �� ���
void	CClanContext::MatchTeamRegisterMatchMaking( void)
{
	if ( ClanGameContext::i()->isMatchTeamLeader() )
	{
		GameEventSender::i()->SetEvent( EVENT_CLAN_WAR_REGISTER_MATCHMAKING );
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_PROPOSE_ONLY_LEADER"));/*������ Ŭ���� ��û�� �����մϴ�.*/
	}
}

//   ��ġ����ŷ �� ������
void	CClanContext::MatchTeamCancelRegisterMatchMaking(void)
{
	if (ClanGameContext::i()->isMatchTeamLeader())
	{
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REGISTER_CANCEL_MATCHMAKING);
	}
}

void CClanContext::cbClanCreate(void* pThis, INT32 nParam)
{
	if( nParam == MB_OK)
		GameEventSender::i()->SetEvent(EVENT_CLAN_CREATION_INFO);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Ŭ�� ����Ʈ
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClanContext::EnterClanList(bool bReqToServerForList)
{	
	_ReleaseSortClanList();

	//	default sorting type
	m_wstrCurSearchClan.clear(); 

	m_rUpdateClanListTime		= 0.f; 
	m_nUpdateClanListReq_BeginClanIdx	= 0;
	m_nUpdateClanListAck_BeginClanIdx	= -1;
	m_nUpdateClanListAck_CurrRecvClanCount = 0;

	m_nUpdateClanListCount		= 0;
	m_nAppendClanListCount		= 0;
	m_bUpdateClanListFinish		= false;
	m_bNeedUpdateAppendClanList	= true;

	if( bReqToServerForList)
		GameEventSender::i()->SetEvent(EVENT_CLAN_LIST_CONTEXT);
}

#define CLAN_LIST_UPDATE_TIME	3.0f
bool CClanContext::UpdateClanList(REAL32 rDeltaSeconds)
{
	bool bRet = false;
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();

	//��� ���� ó�� �ʿ��� 20080102 ���� 
#if defined TEST_CLAN_LIST
	if( m_sortClanList.GetCount() == 0)
	{
		_BuildSortClanListDummy();

		return true;
	}
#else
	
	// 14376 : Ŭ��ä�ο��� ������ Ŭ����ü�� �õ��ϸ� Lobby Leave �� ���¿���
	// EVENT_CLAN_LIST_N �� ������ ���� �߻�. UI �̵��߿��� ��û���� �ʵ��� ����.
	if (!g_pFramework->GetUIMainframe()->GetCurrentPhaseType())
		m_bUpdateClanListFinish = false;

	if( m_bUpdateClanListFinish == false) 
	{
		if( m_bNeedUpdateAppendClanList )
		{
			m_rUpdateClanListTime += rDeltaSeconds;

			// �����ð��Ǹ� ������ Ŭ�� ����Ʈ�� ��û�Ѵ�.
			if( m_rUpdateClanListTime > CLAN_LIST_UPDATE_TIME)
			{
				m_rUpdateClanListTime	= 0.0f;

				// Ŭ��ä�ο��� (Ż��/����/��ü) ���� ������ ������ �̵����̸� ó������ �ʽ��ϴ�
				if(!ClanGameContext::i()->IsForceLeaveChannel())
					GameEventSender::i()->SetEvent(EVENT_CLAN_LIST_N, &m_nUpdateClanListReq_BeginClanIdx);

				m_bNeedUpdateAppendClanList = false;
				
			}
		}
	}

	// �����κ��� ���� ���ο� ������ Ŭ�� ����Ʈ�� �޾Ƽ� ���� ����Ʈ �߰�.
	{
		if( m_nUpdateClanListCount >= (INT32) pClanContext->_totalCount)
			m_bUpdateClanListFinish = true;
		else
			m_bUpdateClanListFinish = false;

		// ������Ʈ �������� ���ŵǸ� ����Ʈ�� �߰�.
		if( !m_bUpdateClanListFinish)
		{
			if( m_nUpdateClanListReq_BeginClanIdx == m_nUpdateClanListAck_BeginClanIdx)
			{
				//	Ŭ�� ������ ���ٸ� ������ ��� ��û�ϵ���. Ŭ�� ������ �ִٸ� ����Ʈ�� �����Ѵ�
				if( pClanContext->_sectionCount > 0)
				{
					m_nUpdateClanListCount += m_nUpdateClanListAck_CurrRecvClanCount;

					// Ŭ�� ����Ʈ �߰�
					_BuildSortClanAppendList( m_wstrCurSearchClan);
					SortClanList(g_sort_type);

					bRet = true;	//	����Ʈ�� ���ŵƽ��ϴ�.
				}
			}

			if( m_nUpdateClanListAck_BeginClanIdx + m_nUpdateClanListAck_CurrRecvClanCount < (INT32)pClanContext->_totalCount &&
				m_nUpdateClanListReq_BeginClanIdx == m_nUpdateClanListAck_BeginClanIdx)
			{
				m_nUpdateClanListReq_BeginClanIdx += m_nUpdateClanListAck_CurrRecvClanCount; // ���� ��û ����

			//	m_nUpdateClanListReqSection++;	// ���� ��û ����

				m_bNeedUpdateAppendClanList = true;
			}
		}
	}
#endif

	return bRet;
}

bool CClanContext::SearchClanListView(const i3::wliteral_range& wKeywordRng)
{
	i3::stack_wstring wstrKeyword;

	UINT32 size = i3::generic_string_size(wKeywordRng);

	for( UINT32 i = 0; i < size; i++)
	{
		if( wKeywordRng[i] != L' ')
			wstrKeyword.push_back(wKeywordRng[i]);
	}

	if( i3::generic_is_iequal( m_wstrCurSearchClan, wstrKeyword  ) )
	{
		return false;
	}

	m_wstrCurSearchClan = wstrKeyword;
	SortClanList(g_sort_type);

	return true;
}

void CClanContext::InitSortClanList(INT32 type)
{
	SortClanList(type);
}

//-----------------------------------------------------------------------------------
//	List���� 20000���� Ŭ���� �������� sort�� �ð��� �����߽��ϴ�. 
//	(���� ����� timeGetTime �Լ��� �̿��� start�� end �ð��� ���Ѱ���.)
//
// [linear sort in List]
//	- name (string)				: about 22 s
//	- entry date (UINT32)		: about 9.3 s
//	- rank or member (UINT8)	: about 8.9 s
//
// [quick sort in List]
//	- name (string)				: about 16-17 s
//	- entry date (UINT32)		: about 9 s
//	- rank or member (UINT8)	: about 8.6 s
//-----------------------------------------------------------------------------------
void CClanContext::SortClanList(INT32 type)
{
	g_sort_type = static_cast<CLAN_SORT_TYPE>(type);

	_DynamicBuildSortClanList();

	switch( g_sort_type)
	{
	case CLAN_SORT_CUR_SEASON:
		{
			if( ClanSeasonContext::i()->GetCurrentSeason()->IsValidSeason() )
				std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_cur_season);
			else
				std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_total_record);
		}
		break;
	case CLAN_SORT_PREV_SEASON:
		{
			if( ClanSeasonContext::i()->GetPreSeasonInfo()->IsValidSeason() )
				std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_prev_season);
			else
				std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_total_record);
		}
		break;
	case CLAN_SORT_TOTAL_RECORD:
		{
			std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_total_record);
		}
		break;
	case CLAN_SORT_MY_CLAN:
		{
			std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_my_clan);

			// ���� ��, �� Ŭ�� �������� ���Ʒ� �Ѱ� Ŭ������ ǥ�õǵ��� �����մϴ�.
			{
				size_t count = m_sortClanList.size(); 

				for(size_t idx = 0 ; idx < count ; ++idx)
				{
					SORT_CLAN_INFO * pInfo = m_sortClanList[idx];
					if( pInfo->_info._idx == GetMyClanDetailInfo()->m_ui32CID )
					{
						i3::vector<SORT_CLAN_INFO*>	_list;

						if( idx > 0 )
							_list.push_back( m_sortClanList[idx - 1] );

						_list.push_back( m_sortClanList[idx] );

						if( idx < (count - 1))
							_list.push_back( m_sortClanList[idx + 1] );
						
						m_sortClanList = _list;
						break;
					}
				}
			}

		}
		break;
	default:		
		{
			std::sort(m_sortClanList.begin(), m_sortClanList.end(), sort_by_clan_idx);
		}
		break;
	}
}

INT32 CClanContext::GetClanSerchRate(void)
{
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();

//	if( pClanContext->_sectionCount > 0 && m_nUpdateClanListAckSection >= 0)
	if ( pClanContext->_totalCount > 0 )
	{

		REAL32 rate = (REAL32)(m_nUpdateClanListCount) / (REAL32)(pClanContext->_totalCount);
					//(REAL32) (m_nUpdateClanListAckSection +1) / (REAL32)(pClanContext->_sectionCount);

		if( rate > 1.0f) rate = 1.0f;

		return (INT32)(rate * 100.0f);
	}

	return 0;
}

void CClanContext::_DynamicBuildSortClanList(void)
{
	bool build_all_clan = false;

	if( g_sort_type == CLAN_SORT_MY_CLAN )
		build_all_clan = true;

	if( m_wstrCurSearchClan.length() == 0)
		build_all_clan = true;

	// ��ü Ŭ�� ����Ʈ ����.
	if( build_all_clan == true )
	{
#if defined TEST_CLAN_LIST
		_BuildSortClanListDummy();
#else
		_BuildSortClanList();
#endif
	}
	else
	{
		//	������ Ű����� Ŭ�� ����Ʈ ����.
#if defined TEST_CLAN_LIST
		_BuildSearchClanListDummy( m_wstrCurSearchClan);
#else
		_BuildSearchClanList( m_wstrCurSearchClan);
#endif 
	}
}

void CClanContext::_BuildSortClanList(void)
{
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();
	CLAN_LIST_BASIC_INFO  * pClanBaseInfo = ClanGameContext::i()->getClanList();
	INT32 count = pClanContext->_totalCount;

	if( pClanBaseInfo != nullptr)
		_BuildSortClanList(pClanBaseInfo, count);
}

void CClanContext::_BuildSearchClanList(const i3::wliteral_range& wKeywordRng)
{
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();
	CLAN_LIST_BASIC_INFO * pClanBaseInfo = ClanGameContext::i()->getClanList();	
	INT32 count = pClanContext->_totalCount;

	_BuildSearchClanList(wKeywordRng, pClanBaseInfo, count);
}

void CClanContext::_BuildSortClanListDummy(void)
{
	//	�׽�Ʈ Ŭ�� �ӽ� ����
	UINT32 cnt = 3000;
	CLAN_LIST_BASIC_INFO * pDummy = (CLAN_LIST_BASIC_INFO*) i3MemAlloc(sizeof( CLAN_LIST_BASIC_INFO) * cnt);
	I3ASSERT( pDummy != nullptr);
	i3mem::FillZero(pDummy, sizeof( CLAN_LIST_BASIC_INFO) * cnt);

	for(UINT32 i=0; i<cnt; i++)
	{
		CLAN_LIST_BASIC_INFO * pInfo = pDummy+i;

		TTCHAR temp[256] = _TT("");
		i3::snprintf(temp, _countof(temp), _TT("DUMMY %d"), i+1);
		pInfo->m_ClanBasicInfo._idx = i+1;
		i3::safe_string_copy( pInfo->m_ClanBasicInfo._name, temp, NET_CLAN_NAME_SIZE);
		pInfo->m_ClanBasicInfo._rank = (UINT8)(i3Math::Rand() % 50 + 1);
		pInfo->m_ClanBasicInfo._person = (UINT8)(i3Math::Rand() % 200 + 1);    
		pInfo->m_ClanBasicInfo._date = 0;

		INT32 a1 = rand() % CClanMarkManager::GetMarkBackImageCount() + 1;
		INT32 b1 = rand() % CClanMarkManager::GetMarkShapeImageCount() + 1;
		INT32 a2 = rand() % CClanMarkManager::GetMarkColorCount() + 1;
		INT32 b2 = rand() % CClanMarkManager::GetMarkColorCount() + 1;

		pInfo->m_ClanBasicInfo._mark = MAKE_CLANMARK_FLAG(a1, b1, a2, b2);
	}

	_BuildSortClanList(pDummy, cnt);

	I3MEM_SAFE_FREE( pDummy);
}

void CClanContext::_BuildSearchClanListDummy(const i3::wliteral_range& wKeywordRng)
{
	//	�׽�Ʈ Ŭ�� �ӽ� ����
	UINT32 cnt = 3000;
	CLAN_LIST_BASIC_INFO * pDummy = (CLAN_LIST_BASIC_INFO*) i3MemAlloc(sizeof( CLAN_LIST_BASIC_INFO) * cnt);
	I3ASSERT( pDummy != nullptr);
	i3mem::FillZero(pDummy, sizeof( CLAN_LIST_BASIC_INFO) * cnt);

	for(UINT32 i=0; i<cnt; i++)
	{
		CLAN_LIST_BASIC_INFO * pInfo = pDummy+i;

		TTCHAR temp[256] = _TT("");
		i3::snprintf(temp, _countof(temp), _TT("DUMMY %d"), i+1);
		pInfo->m_ClanBasicInfo._idx = i+1;
		i3::safe_string_copy( pInfo->m_ClanBasicInfo._name, temp, NET_CLAN_NAME_SIZE);
		pInfo->m_ClanBasicInfo._rank = (UINT8)(i3Math::Rand() % 50 + 1);
		pInfo->m_ClanBasicInfo._person = (UINT8)(i3Math::Rand() % 200 + 1);    
		pInfo->m_ClanBasicInfo._date = 0;

		INT32 a1 = rand() % CClanMarkManager::GetMarkBackImageCount() + 1;
		INT32 b1 = rand() % CClanMarkManager::GetMarkShapeImageCount() + 1;
		INT32 a2 = rand() % CClanMarkManager::GetMarkColorCount() + 1;
		INT32 b2 = rand() % CClanMarkManager::GetMarkColorCount() + 1;

		pInfo->m_ClanBasicInfo._mark = MAKE_CLANMARK_FLAG(a1, b1, a2, b2);
	}

	_BuildSearchClanList(wKeywordRng, pDummy, cnt);

	I3MEM_SAFE_FREE( pDummy);
}

bool CClanContext::_CreateSortClanListStorage( INT32 nCreateCount)
{
	if( (INT32) m_nSortClanInfoCount != nCreateCount)
	{//���� ī��Ʈ�� �ٽù��� ī��Ʈ�� �ٸ��ٸ� �޸� ���Ҵ�
		i3::cu::for_each_delete_clear(m_pSortClanInfo);

		if( nCreateCount > 0)
		{
			m_pSortClanInfo.resize(nCreateCount);
			for (int i = 0; i < nCreateCount; i++)
				m_pSortClanInfo[i] = (new SORT_CLAN_INFO());
		}

		m_nSortClanInfoCount = nCreateCount;

		return true;
	}

	return false;
}

void CClanContext::_BuildSortClanAppendList( const i3::wliteral_range& wKeywordRng )
{
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();
	CLAN_LIST_BASIC_INFO * pClanBaseInfo = ClanGameContext::i()->getClanList();	
	INT32 nTotalCnt = pClanContext->_totalCount;
	INT32 nNeedUpdateCnt = (m_nUpdateClanListCount - m_nAppendClanListCount);
	INT32 nNeedUpdateStart = m_nAppendClanListCount;

	I3ASSERT( pClanBaseInfo != nullptr);

	// ����Ʈ �޸� �Ҵ�
	if( _CreateSortClanListStorage( nTotalCnt))
	{
		//�ʱ�ȭ 
		m_sortClanList.clear();

		//I3TRACE( "SortClanList clear.\n");
	}

	if( nTotalCnt <= 0 || nNeedUpdateCnt <= 0 )
		return;

	m_bClanListUpdated = true;

	//	����Ʈ �ڷ� ���ο� Ŭ���� �߰�
	for(INT32 i=nNeedUpdateStart; i< m_nUpdateClanListCount; i++)
	{
		if( i >= (INT32) m_nSortClanInfoCount) break;

		m_nAppendClanListCount++;

		CLAN_LIST_BASIC_INFO * pData = pClanBaseInfo + i;

		if( pData->m_ClanBasicInfo._idx > 0)
		{
			SORT_CLAN_INFO * pNewInfo = m_pSortClanInfo[i];

			//	Ŭ�� ���� ���纻�� �����.
			pNewInfo->CopyFromClanListInfo(pData);

			if( i3::generic_string_size( wKeywordRng) > 0)
			{
				//	������Ʈ�Ǵ� Ŭ�� ����Ʈ�� �޾ƿ��� �߿� �˻����� �ִٸ� ���� ���������� �ڵ����� �˻��ؼ� Ŭ�� ����Ʈ�� �߰��Ѵ�. komet
				i3::wliteral_range clan_name = pNewInfo->_info._name;
				if( i3::icontain_string( clan_name, wKeywordRng) != -1)
				{
					m_sortClanList.push_back(pNewInfo);

					//I3TRACE( "SortClanList search add %s (%d)\n", pNewInfo->_info._name, i);
				}
			}
			else
			{
				m_sortClanList.push_back(pNewInfo);

				//I3TRACE( "SortClanList add %s (%d)\n", pNewInfo->_info._name, i);
			}
		}

		//I3TRACE( "add (%d) - %s\n", i, pData->_name);
	}
}

void CClanContext::_BuildSortClanList(const i3::vector<CLAN_LIST_BASIC_INFO*>& InfoList)
{
	
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();

	const size_t count = InfoList.size();

	// ����Ʈ �޸� �Ҵ�
	_CreateSortClanListStorage(pClanContext->_totalCount);

	//�ʱ�ȭ 
	m_sortClanList.clear();

	if( count <= 0)
		return;

	//	Ŭ�� ����Ʈ ����
	for( size_t i=0; i<count; i++)
	{
		if( i >= (INT32) m_nSortClanInfoCount) break;

		CLAN_LIST_BASIC_INFO * pData = InfoList[i];

		if( pData->m_ClanBasicInfo._idx > 0)
		{
			SORT_CLAN_INFO * pNewInfo = m_pSortClanInfo[i];

			//	Ŭ�� ���� ���� ������ �д�.
			pNewInfo->CopyFromClanListInfo(pData);

			m_sortClanList.push_back(pNewInfo);
		}
	}

}

void CClanContext::_BuildSortClanList(CLAN_LIST_BASIC_INFO * pInfo, INT32 count)
{
	I3ASSERT( pInfo != nullptr);

	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext();

	// ����Ʈ �޸� �Ҵ�
	_CreateSortClanListStorage(pClanContext->_totalCount);
	
	//�ʱ�ȭ 
	m_sortClanList.clear();

	if( count <= 0)
		return;

	//	Ŭ�� ����Ʈ ����
	for(INT32 i=0; i<count; i++)
	{
		if( i >= (INT32) m_nSortClanInfoCount) break;

		CLAN_LIST_BASIC_INFO * pData = pInfo + i;

		if( pData->m_ClanBasicInfo._idx > 0)
		{
			SORT_CLAN_INFO * pNewInfo = m_pSortClanInfo[i];

			//	Ŭ�� ���� ���纻�� �����.
			pNewInfo->CopyFromClanListInfo(pData);

			m_sortClanList.push_back( pNewInfo);
		}
	}

}

void CClanContext::_BuildSearchClanList(const i3::wliteral_range& wKeywordRng, CLAN_LIST_BASIC_INFO * pInfo, INT32 count)
{	
	i3::vector<CLAN_LIST_BASIC_INFO*> searchList;
	
	for(INT32 i=0; i<count; i++) 
	{
		CLAN_LIST_BASIC_INFO * pClanInfo = pInfo + i;

		//	keyword�� �����ϴ� Ŭ�� �̸��� �ִٸ�
		i3::wliteral_range clan_name = pClanInfo->m_ClanBasicInfo._name;
		if( i3::icontain_string(clan_name, wKeywordRng) != -1)
		{
			searchList.push_back(pClanInfo);
		}
	} 

	_BuildSortClanList(searchList);
}

void CClanContext::_ReleaseSortClanList(void)
{
	i3::cu::for_each_delete_clear(m_pSortClanInfo);

	m_sortClanList.clear();

	m_nSortClanInfoCount	= 0;	
	m_nAppendClanListCount  = 0;
}

bool CClanContext::PossibleChangeClanName(void)
{	//	Ŭ���� ���� ������ ���� ����
	return CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_CHANGE_CLAN_NAME);
}

bool CClanContext::PossibleChangeClanMark(void)
{	//	Ŭ�� ��ũ ������ ���� ���� ����
	return CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_CHANGE_CLAN_MARK);
}

// �ڽ��� Ŭ�� �ʴ���� �������� Ȯ��
bool CClanContext::IsPossibleClanInvite(void)
{
	return CheckMenagementAuth( GetMyStaffLevel(), CLAN_MENAGE_TYPE_INVITED );
}

void CClanContext::SetClanCreationRequirementInfo( UINT8 Rank, UINT32 Point )
{
	m_ClanCreationRequirementRank	= Rank;
	m_ClanCreationRequirementPoint	= Point;
	m_HasClanCreationInfo	= true;
}

bool CClanContext::GetClanCreationRequirementInfo( UINT8 * Rank, UINT32 * Point )
{
	if ( ! m_HasClanCreationInfo )
		return false;

	if ( Rank )
		*Rank	= m_ClanCreationRequirementRank;

	if ( Point )
		*Point	= m_ClanCreationRequirementPoint;

	return true;
}


//////////////////	Ŭ�� ��ġ ��� // ///////////////

void CClanContext::_ClearMatchResultList(void)
{
	i3mem::FillZero(m_ClanResultList, sizeof(m_ClanResultList));
}

//void CClanContext::RequestClanMatchResultList(void)
//{
//	m_rMatchListReqTime			= 1000.0f; 
//	m_ui8MatchListReqSection	= 0; 
//	m_bMatchListReqFinish		= false;
//
//	//	��ġ ��� ����Ʈ �ٽ� ����
//	_ClearMatchResultList();
//}
//
//void CClanContext::UpdateClanMatchRsultList(REAL32 rDeltaSeconds)
//{
//	if( m_bMatchListReqFinish == true )
//		return;
//
//	m_rMatchListReqTime += rDeltaSeconds;
//
//	if( m_rMatchListReqTime > LIST_UPDATE_TIME )
//	{
//		m_rMatchListReqTime = 0.0f;
//
//		PACKET_CS_CLAN_MATCH_RESULT_LIST_REQ ReqPacket;
//
//		// �ڱ� Ŭ�� or Ÿ Ŭ��(�湮��)�� ���� Ŭ�� UID �� �����մϴ�.
//		if(IsClanVisit() == true)
//			ReqPacket.m_ui32CUID = ClanGameContext::i()->getClanDetailInfo()->m_ui32CID;
//		else
//			ReqPacket.m_ui32CUID = ClanGameContext::i()->getMyClanDetailInfo()->m_ui32CID;
//
//		ReqPacket.m_ui8Section = m_ui8MatchListReqSection;
//
//		GameEventSender::i()->SetEvent(EVENT_CLAN_MATCH_RESULT_LIST, &ReqPacket);
//
//		m_ui8MatchListReqSection++;
//
//		if(m_ui8MatchListReqSection >= m_ClanResultContext.m_ui8SectionCount )
//			m_bMatchListReqFinish = true;
//	}
//}

void CClanContext::ResetMatchResultContext( void)
{
	i3mem::FillZero(&m_ClanResultContext, sizeof(m_ClanResultContext));
	_ClearMatchResultList();
}

/************************************************************************/
/*								Game Event Func                         */
/************************************************************************/
void CClanContext::InitGameEventFunc(void)
{
	RegisterGameEventFunc(EVENT_CLAN_ENTER,						&CClanContext::ReceiveGameEvent_Clan_Enter					);		// Ŭ�� ������ ����
	RegisterGameEventFunc(EVENT_CLAN_LEAVE,						&CClanContext::ReceiveGameEvent_Clan_Leave					);		// Ŭ�� ������ ����
	RegisterGameEventFunc(EVENT_CLAN_MEMBER_CONTEXT,			&CClanContext::ReceiveGameEvent_Clan_MemberContext			);		// Ŭ���� ���ؽ�Ʈ ��û
	RegisterGameEventFunc(EVENT_CLAN_MEMBER_LIST,				&CClanContext::ReceiveGameEvent_Clan_MemberList				);		// Ŭ���� ����Ʈ ��û
	RegisterGameEventFunc(EVENT_CLAN_REQUEST_CONTEXT,			&CClanContext::ReceiveGameEvent_Clan_RequestContext			);		// ���� ����� ����Ʈ ��û
	RegisterGameEventFunc(EVENT_CLAN_ACCEPT_REQUEST,			&CClanContext::ReceiveGameEvent_Clan_AcceptRequest			);		// Ŭ�� ���� ����
	RegisterGameEventFunc(EVENT_CLAN_DENIAL_REQUEST,			&CClanContext::ReceiveGameEvent_Clan_DenialRequest			);		// Ŭ�� ���� �ź�
	RegisterGameEventFunc(EVENT_CLAN_SECESSION,					&CClanContext::ReceiveGameEvent_Clan_Secession				);		// Ŭ�� Ż��
	RegisterGameEventFunc(EVENT_CLAN_COMMISSIONMASTER,			&CClanContext::ReceiveGameEvent_Clan_CommissionMaster		);		// Ŭ�� ������ ����
	RegisterGameEventFunc(EVENT_CLAN_COMMISSIONSTAFF,			&CClanContext::ReceiveGameEvent_Clan_CommissionStaff		);		// Ŭ�� ������ �Ӹ�
	RegisterGameEventFunc(EVENT_CLAN_COMMISSION_REGULAR,		&CClanContext::ReceiveGameEvent_Clan_CommissionRegular		);		// �Ϲ� Ŭ�������� ����
	RegisterGameEventFunc(EVENT_CLAN_DISMISSALMEMBER,			&CClanContext::ReceiveGameEvent_Clan_DismissalMember		);		// Ŭ���� ����
	RegisterGameEventFunc(EVENT_CLAN_NOTICE_MODIFY,				&CClanContext::ReceiveGameEvent_Clan_ModifyOption			);		// �������� ����
	RegisterGameEventFunc(EVENT_CLAN_GUIDE_MODIFY,				&CClanContext::ReceiveGameEvent_Clan_ModifyOption			);		// Ŭ�� �Ұ��� ����
	RegisterGameEventFunc(EVENT_CLAN_ACCEPT_OPTION_MODIFY,		&CClanContext::ReceiveGameEvent_Clan_ModifyOption			);		// Ŭ�� ���� �ڵ�/���� ���� ����
	RegisterGameEventFunc(EVENT_CLAN_CANCEL_REQUEST,			&CClanContext::ReceiveGameEvent_Clan_CancelRequest			);		// ���� ��û�� ���
	RegisterGameEventFunc(EVENT_CLAN_DISSOLUTION,				&CClanContext::ReceiveGameEvent_Clan_Dissolution			);		// Ŭ�� ��ü
	RegisterGameEventFunc(EVENT_CLAN_MANAGEMENT,				&CClanContext::ReceiveGameEvent_Clan_Management				);		// Ŭ�� ���� ������ ����

	RegisterGameEventFunc(EVENT_CLAN_WAR_CREATE_TEAM,			&CClanContext::ReceiveGameEvent_ClanWar_CreateTeam			);		// Ŭ���� �� ����
	RegisterGameEventFunc(EVENT_CLAN_WAR_JOIN_TEAM,				&CClanContext::ReceiveGameEvent_ClanWar_JoinTeam			);		// Ŭ���� �� ����
	RegisterGameEventFunc(EVENT_CLAN_WAR_LEAVE_TEAM,			&CClanContext::ReceiveGameEvent_ClanWar_LeaveTeam			);		// Ŭ���� �� ����
	RegisterGameEventFunc(EVENT_CLAN_WAR_CHANGE_MAX_PERSON,		&CClanContext::ReceiveGameEvent_ClanWar_ChangeMaxPerson		);		// Ŭ���� �� ���� ����
}


void CClanContext::RegisterGameEventFunc(GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert( i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f) );
}

void CClanContext::ReceiveGameEvent_Clan_Enter(INT32 arg, const i3::user_data & )
{
	if( EV_FAILED(arg))
	{
		GameUI::ExitGamePopup( GAME_RCSTRING("STBL_IDX_EP_CLAN_ENTER_FAIL_E") );
		return;
	}

	m_bEnter = true;
	
	if( arg == 1 )		m_bMember = true;		//	Ŭ���� ���ԵǾ� �ֽ��ϴ�.
	else if (arg == 0)	m_bMember = false;		//	Ŭ�� �̰��� �����Դϴ�.
	else				I3ASSERT_0;
}

void CClanContext::ReceiveGameEvent_Clan_Leave(INT32 arg, const i3::user_data & )
{
	if (EV_FAILED(arg))
	{
		GameUI::ExitGamePopup( GAME_RCSTRING("STBL_IDX_EP_CLAN_LEAVE_FAIL_E") );
		return;
	}

	m_bEnter = false;
}

void CClanContext::ReceiveGameEvent_Clan_MemberContext(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED(arg) )
		RequestClanMember();
}

void CClanContext::ReceiveGameEvent_Clan_MemberList(INT32 arg, const i3::user_data & )
{

}

void CClanContext::ReceiveGameEvent_Clan_RequestContext(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED(arg) )
		RequestClanSubscriber();
}

void CClanContext::ReceiveGameEvent_Clan_AcceptRequest(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		// "%d���� Ŭ�� ���� ó���� ���εǾ����ϴ�."
		i3::rc_wstring wstrTemp;
		i3::sprintf( wstrTemp, GAME_RCSTRING("STBL_IDX_CLAN_PROCESS_SUBSCRIPTION"), arg);
		// Ŭ�� ���� ����
		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp , &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_SUCCESS"));
		GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_CONTEXT);

		SendClanDetailInfoNEvent(GetMyClanDetailInfo()->m_ui32CID, 0);
	}
	else
	{
		switch( arg ) 
		{
		case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
			{
				//	"\nŬ�� ���� ó���� �����߽��ϴ�.\nŬ���� ã���� �����ϴ�."
				//	Ŭ�� ���� ���� ����
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_FINDING"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_NOMEMBER:
			{
				//	"\nŬ�� ���� ó���� �����߽��ϴ�.\nŬ������ �ƴմϴ�."
				//	Ŭ�� ���� ���� ����
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_LEVEL_NOMASTER:
			{
				//	"\nŬ�� ���� ó���� �����߽��ϴ�.\nŬ�� �����Ͱ� �ƴմϴ�."
				//	Ŭ�� ���� ���� ����
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_MASTER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_MEMBER:
			{
				//	"\nŬ�� ���� ó���� �����߽��ϴ�.\n�̹� Ŭ���� ���ԵǾ��ֽ��ϴ�."
				//	Ŭ�� ���� ���� ����
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_ALREADY_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_MAXMEMBER:
			{
				//	"\nŬ�� ���� ó���� �����߽��ϴ�.\n���̻� ���� �� �� �����ϴ�."
				//	Ŭ�� ���� ���� ����
				GameUI::MsgBox(MSG_TYPE_GAME_OK,GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_OVER_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
			}
			break;
		default:
			{
				i3::rc_wstring wstrTemp;
				//	""\nŬ�� ���� ó���� �����߽��ϴ�.\n �˼��մϴ�. �����ڿ��� �����Ͽ� �ֽʽÿ�."
				i3::sprintf( wstrTemp, L"%s\n [Errcode : 0x%08x]", GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_ASK_FOR_INFO"), arg );
				GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));	//	Ŭ�� ���� ���� ����
			}
			break;
		}
		GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_CONTEXT); 
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_DenialRequest(INT32 arg, const i3::user_data & )
{
	if( EV_FAILED(arg))			
	{
		//	"Ŭ�� �ź� ó���� �����߽��ϴ�.
		//	"Ŭ�� ���� �ź�"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_REJECTION_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_REJECTION_FAIL"));
		GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_CONTEXT); 
	}
	else
	{	
		//	"%d���� Ŭ�� ������ �ź��Ͽ����ϴ�."
		i3::rc_wstring wstrTemp;
		i3::sprintf( wstrTemp, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_REJECTION_SUCCESS"), arg);
		//	"Ŭ�� ���� �ź�"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_REJECTION_FAIL"));
		GameEventSender::i()->SetEvent(EVENT_CLAN_REQUEST_CONTEXT);

		SendClanDetailInfoNEvent(GetMyClanDetailInfo()->m_ui32CID, 0);
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_Secession(INT32 arg, const i3::user_data & )
{
	if( EV_FAILED( arg ) )
	{
		switch( arg )
		{
		case EVENT_ERROR_CLAN_LEVEL_MASTER:
			//	"Ŭ�� �����ʹ� Ŭ���� Ż���� �� �����ϴ�.\nŻ���ϱ� ���ؼ� Ŭ�� �����͸� �����ؾ� �մϴ�."
			//	"Ŭ�� Ż�� ����"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_SECESSION_MASTER_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SECESSION_FAIL"));
			break;
		case EVENT_ERROR_CLAN_BATTLE_TEAM_HAVE_FAIL:
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_SECESSION_MATCH_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SECESSION_FAIL"));
			break;
		case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
			if(g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_CLAN_MEMBER) == true)
			{
				g_pFramework->GetUIMainframe()->ClosePopup(UPW_CLAN_MEMBER);
			}
			break;
		default:
			//	"Ŭ�� Ż�� �����߽��ϴ�."
			//	"Ŭ�� Ż�� ����"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_SECESSION_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SECESSION_FAIL"));
			break;
		}
	}
}

void CClanContext::ReceiveGameEvent_Clan_CommissionMaster(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		//	"Ŭ�� ������ ������ ���������� ó���Ǿ����ϴ�."
		//	"Ŭ�� ������ ���� �Ϸ�"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_MASTER_SUCCESS"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_MASTER_SUCCESS"));

		GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_CONTEXT);

		USER_INFO_BASIC		MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
		MyUserInfoBasic.m_ui32ClanState = CLAN_MEMBER_LEVEL_STAFF;
		UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

		SendClanDetailInfoNEvent(GetMyClanDetailInfo()->m_ui32CID, 0);
	}
	else 
	{
		switch( arg )
		{
		case EVENT_ERROR_CLAN_USE_ITEM_FAIL:
			//	"������� �������� �־�\nŬ�� ������ ���ӿ� �����߽��ϴ�."
			//	"Ŭ�� ������ ���� ����"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_MASTER_ITEM_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_MASTER_FAIL"));
			break;
		default:
			//	"Ŭ�� ������ ������ �����߽��ϴ�."
			//	"Ŭ�� ������ ���� ����"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_MASTER_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_MASTER_FAIL"));
			break;
		}
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_CommissionStaff(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		//	"1���� �������� �Ӹ� �Ͽ����ϴ�."
		i3::rc_wstring wstrTemp;
		i3::sprintf( wstrTemp, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_STAFF_SUCESS"), arg);
		//	"������ �Ӹ� �Ϸ�"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_STAFF_SUCESS"));

		GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_CONTEXT);
	}
	else {
		//	"������ �Ӹ� �����߽��ϴ�."
		//	"������ �Ӹ� ����"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_STAFF_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_STAFF_FAIL"));
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_CommissionRegular(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		i3::rc_wstring wstrTemp;

		i3::sprintf( wstrTemp, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_REGULAR_SUCCESS"), arg);
		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_REGULAR_SUCCESS"));

		GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_CONTEXT);
	}
	else {
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMMISION_REGULAR_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMMISION_REGULAR_FAIL"));
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_DismissalMember(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		//	"%d���� ���� �Ͽ����ϴ�."
		i3::rc_wstring wstrTemp;
		i3::sprintf( wstrTemp, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISMISSAL_SUCCESS"), arg);
		//	"Ŭ�� ��� ���� �Ϸ�"
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_SUCCESS"));

		GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_CONTEXT);

		SendClanDetailInfoNEvent(GetMyClanDetailInfo()->m_ui32CID, 0);
	}
	else 
	{
		//	"Ŭ�� ��� ���� �����߽��ϴ�."
		//	"Ŭ�� ��� ���� ����"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISMISSAL_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISMISSAL_FAIL"));
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_ModifyOption(INT32 arg, const i3::user_data & )
{	
	if( EV_SUCCESSED( arg ) )
	{
		//	"������ ������ ������ �Ϸ�Ǿ����ϴ�."
		//	"���� �Ϸ�"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_SUCCESS"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_MODIFY_SUCCESS") );
	}
	else 
	{
		if( EV_IS_ERROR( arg, EVENT_ERROR_CLAN_LEVEL_NOSTAFF))
		{
			//	"�ش� ��ɿ� ���� ������ ������ �����Ǿ� ���� �ʽ��ϴ�."
			//	"���� ����"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_MODIFY_FAIL"));
		}
		else
		{
			//	"������ ������ ���忡 �����߽��ϴ�."
			//	"���� ����"
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_MODIFY_FAIL") );
		}
	}

	SetWaitMemberinfoReq(false);
}

void CClanContext::ReceiveGameEvent_Clan_CancelRequest(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{
		ResetClanCreationInfo();
		//	"���Խ�û�� ��ҵǾ����ϴ�."
		//	"���Խ�û ��� �Ϸ�"
		// MessageBox ���� �� Ŭ�� ���� ��� �˾��� ��Ÿ���� �ǹǷ� 
		// ����ɶ����� ����Ѵ�.
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_CANCEL_REQUEST_SUCCESS"), 
			&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_CANCEL_REQUEST_SUCCESS"));//, /*MAKE_CALLBACK(cbClanCreate)*/nullptr , this);
	}
	else 
	{
		//	"���Խ�û�� ��Ұ� �����߽��ϴ�."
		//	"���Խ�û ��� ����"
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_CANCEL_REQUEST_FAIL"), 
			&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_CANCEL_REQUEST_FAIL"));
	}
}

void CClanContext::ReceiveGameEvent_Clan_Dissolution(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED( arg ) )
	{

	}
	else
	{
		switch( arg )
		{
		case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
			{
				//	"Ŭ���� ã���� �����ϴ�."
				//	"Ŭ�� ��ü ����"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_NO_FINDING"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;

		case EVENT_ERROR_CLAN_NOMEMBER:
			{
				//	"Ŭ���� ����� �ƴմϴ�."
				//	"Ŭ�� ��ü ����"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_NO_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_LEVEL_NOMASTER:
			{
				//	"Ŭ�� ��ü�� �����͸� �����մϴ�."
				//	"Ŭ�� ��ü ����"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_ONLY_MASTER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_CLOSECLAN_MEMBER_FAIL:
			{
				//	 "Ŭ�� ����� ���� ���·δ� Ŭ�� ��ü�� �Ұ��� �մϴ�."
				//	"Ŭ�� ��ü ����"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_REMAIN_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;
		case EVENT_ERROR_CLAN_CLOSECLAN_FAIL:
			{
				//	"Ŭ�� ��ü�� �����߽��ϴ�."
				//	"Ŭ�� ��ü ����"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;
		default:
			{
				i3::rc_wstring wstrTemp;
				//	"\nŬ�� ��ü�� �����Ͽ����ϴ�.\n �˼��մϴ�. �����ڿ��� �����Ͽ� �ֽʽÿ�."
				i3::sprintf( wstrTemp, L"%s\n [Errcode : 0x%08x]", 
					GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_ASK_FOR_INFO"), arg );

				//	"Ŭ�� ��ü ����"
				GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_FAIL"));
			}
			break;
		}
	}
}

void CClanContext::ReceiveGameEvent_Clan_Management(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED(arg))
	{
		SendClanDetailInfoNEvent(GetMyClanDetailInfo()->m_ui32CID, 0);
		//	"������ ������ ������ �Ϸ�Ǿ����ϴ�."
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_COMPLETE"), &GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY")); 
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_FAIL"), &GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY"));
	}
}

void CClanContext::ReceiveGameEvent_ClanWar_CreateTeam(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED(arg))
		m_bInBattleTeam = true;
	else
	{
		m_bInBattleTeam = false;
	}
}

void CClanContext::ReceiveGameEvent_ClanWar_JoinTeam(INT32 arg, const i3::user_data & )
{
	if( EV_SUCCESSED(arg))
		m_bInBattleTeam = true;
	else
	{
		m_bInBattleTeam = false;

		switch( arg)
		{				
		case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_NOSEARCH_FAIL:
		case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_EMPTY_FAIL:
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_CANNOT_JOIN"));/*�� ������ �����߽��ϴ�.\n �߸��� ���Դϴ�.*/
			}
			break;
		case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_PERMAX_FAIL:
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_FULL_MEMBER"));/*���� ������ ��� á�� ������ �ش� ���� ������ �� �����ϴ�.*/
			}
			break;	
		default:
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_FAILED_JOIN"));/*��Ʈ��ũ ������ ���� �������� ���߽��ϴ�.\n �ٽ� �õ��� �ּ���.*/
			}
			break;
		}
	}
}

void CClanContext::ReceiveGameEvent_ClanWar_LeaveTeam(INT32 arg, const i3::user_data & )
{
	const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();

	if( pMySquadInfo->isJoinTeam() == false )
		m_bInBattleTeam = false;
}

void CClanContext::ReceiveGameEvent_ClanWar_ChangeMaxPerson(INT32 arg, const i3::user_data & )
{
	//	�� ���� ������ �õ�
	if( EV_SUCCESSED(arg))
	{
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_CHANGE_ORGANIZATION"));/*�� ������ ����Ǿ����ϴ�.*/
	}
	else
	{
		if( ClanGameContext::i()->isMatchTeamLeader() )	//	�������Ը� �����ش�.
		{
			switch(arg)
			{
			case EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL:
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_CANNOT_FIND"));/*���� ã�� �� �����ϴ�.\n �ٽ� �õ��� �ּ���.*/
				break;
			case EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_FAIL:
			case EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_PERMAX_FAIL:
			case EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_NOMASTER_FAIL:
			default:
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_CANNOT_CHANGE"));/*�� ������ ������ �� �����ϴ�.*/
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////
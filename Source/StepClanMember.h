#if !defined( _STEP_CLAN_MEMBER_H__)
#define _STEP_CLAN_MEMBER_H__

#include "StepBase.h"
#include "StageDef.h"
#include "ClanContext.h"
#include "GameChat_Body.h"

//#define ENABLE_CLAN_CHAT		//	Ŭ�� ä�� ����


enum CLAN_PAGE_TYPE {
	CLAN_PAGE_UNKNOWN,
	CLAN_PAGE_INFO,
	CLAN_PAGE_BATTLE,
	CLAN_PAGE_SEARCH,
	CLAN_PAGE_MANAGEMENT,
};


#define MAX_CHAT_LINE_FOR_CLAN		15

enum BEST_MEMBER_SLOT
{
	BEST_MEMBER_SLOT_EXP		= 0,	
	BEST_MEMBER_SLOT_CONNECTING,
	BEST_MEMBER_SLOT_WIN_COUNT,	
	BEST_MEMBER_SLOT_KILL_COUNT,
	BEST_MEMBER_SLOT_HEAD_COUNT,	

	BEST_MEMBER_SLOT_MAX
};

struct BestMemberSlot
{
	i3GuiStatic * pSlot;
	i3GuiEditBox * pTitle;
	i3GuiEditBox * pSeasonMember;
	i3GuiEditBox * pTopMember;

	BestMemberSlot() : pSlot(NULL), pTitle(NULL), pSeasonMember(NULL), pTopMember(NULL)
	{
	}
};

#define CLANINFO_MEMBER_LIST_ROW_MAX 7

struct ClanMemberSlot
{
	i3GuiStatic * pSlot;
	i3GuiStatic * pRank;
	i3GuiEditBox * pClass;
	i3GuiEditBox * pNick;
	i3GuiEditBox * pState;	

	ClanMemberSlot() : pSlot(NULL), pRank(NULL), pClass(NULL), pNick(NULL), pState(NULL)
	{
	}
};

#define CLANINFO_HISTORY_LIST_ROW_MAX 4

enum CLAN_HISTORY_SLOT
{
	CLAN_HISTORY_SLOT_MATCH = 0,
	CLAN_HISTORY_SLOT_WIN,
	CLAN_HISTORY_SLOT_LOSE,
	CLAN_HISTORY_SLOT_MATCHES,
};

struct ClanHistorySlot
{
	i3GuiStatic * pSlot;
	i3GuiEditBox * pTitle;
	i3GuiEditBox * pSeasonScore;
	i3GuiEditBox * pTotalScore;

	ClanHistorySlot() : pSlot(NULL), pSeasonScore(NULL), pTotalScore(NULL)
	{
		pTitle = NULL;
	}
};

#define MANAGEMENT_LIST_ROW_MAX 12

struct ClanListSlot
{
	i3GuiStatic	*	_pSlot;
	i3GuiStatic	*	_pRankImage;
	i3GuiStatic	*	_pClanMarkImage;
	i3GuiEditBox*	_pName;
	i3GuiEditBox*	_pDate;
	i3GuiEditBox*	_pMember;
};

#define CLAN_SEARCH_LIST_SLOT_MAX 8

struct ClanEntryTeamSlot
{	
	i3GuiStatic * pSlot;
	i3GuiEditBox * pTeamName;		//	�� �̸�
	i3GuiEditBox * pLocale;			//	����ġ
	i3GuiStatic * pState;			//	����
	i3GuiEditBox * pLeaderValue;	//	'����'
	i3GuiStatic * pLeaderRank;		//	���� ���
	i3GuiEditBox * pLeaderNick;		//	���� �г���
	i3GuiEditBox * pMatchType;		//	�ο� Ÿ��
	i3GuiEditBox * pEntryMember;	//	���� �ο���
	i3GuiButton * pJoinButton;		//	'�����ϱ�'
};

#define CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX	6

struct ClanManagementSlot
{
	i3GuiStatic * pSlot;
	i3GuiStatic * pRank;
	i3GuiCheckBox * pCheck;
	i3GuiEditBox * pTitle;
	i3GuiEditBox * pNick;
	i3GuiEditBox * pClass;
	i3GuiEditBox * pEntryDay;

	ClanManagementSlot() : pSlot(NULL), pCheck(NULL), pTitle(NULL), pNick(NULL), pClass(NULL), pEntryDay(NULL)
	{
		pRank = NULL;
	}
};

#define SUBSCRIPTION_LIST_ROW_MAX 12

struct ClanSubscriptionSlot
{
	i3GuiStatic * pSlot;
	i3GuiStatic * pRank;
	i3GuiCheckBox * pCheck;
	i3GuiEditBox * pTitle;
	i3GuiEditBox * pNick;
	i3GuiEditBox * pEntryDay;

	ClanSubscriptionSlot() : pSlot(NULL), pCheck(NULL), pTitle(NULL), pNick(NULL), pEntryDay(NULL)
	{
		pRank = NULL;
	}
};

struct ManagementClanMemberInfo
{
	BOOL _bCheck;
	CLAN_MEMBER_BASIC_INFO _info;

	ManagementClanMemberInfo() : _bCheck(FALSE)
	{		
		i3mem::FillZero(&_info, sizeof(CLAN_MEMBER_BASIC_INFO));
	}
};

struct ManagementSubscriberInfo
{
	BOOL _bCheck;
	CLAN_REQUEST_BASIC_INFO _info;

	ManagementSubscriberInfo() : _bCheck(FALSE)
	{
		i3mem::FillZero(&_info, sizeof(CLAN_REQUEST_BASIC_INFO));
	}
};

enum ADMISSION_LIMIT_TYPE
{
	ADMISSION_LIMIT_RANK,
	ADMISSION_LIMIT_AGE1,
	ADMISSION_LIMIT_AGE2,
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
class CStepClanMember : public CStepBase
{
	I3_CLASS_DEFINE(CStepClanMember);

protected:
	CClanContext * m_pClanContext;

	INT32 m_nChatType;
	INT32 m_nOldChatType;
	CLAN_PAGE_TYPE m_CurrentPage;

	i3GuiStatic * m_pClanMemberRoot;		//	Gui Root

	i3GuiStatic * m_pMyClanWin;				//	�� Ŭ�� â
	i3GuiEditBox * m_pMyClanCaption;		//	'�� Ŭ��'

	i3GuiStatic * m_pClanManagementWin;		//	Ŭ������ â
	i3GuiEditBox * m_pClanManagementCaption;	//	'Ŭ������'

	i3GuiButton * m_pInfoTab;			//	Ŭ������ ��
	i3GuiButton * m_pBattleTab;			//	Ŭ���� ��
	i3GuiButton * m_pSearchTab;			//	Ŭ��ã�� ��
	i3GuiButton * m_pManagementTab;		//	Ŭ������ ��
	i3GuiButton * m_pLeaveButton;		//	������ ��ư

	i3GuiStatic * m_pInfoPhase;				//	Ŭ������ �� ������
	i3GuiStatic * m_pBattlePhase;			//	Ŭ���� �� ������
	i3GuiStatic * m_pSearchPhase;			//	Ŭ��ã�� �� ������
	i3GuiStatic * m_pManagementPhase;		//	Ŭ������ �� ������

	//==================================================================================================
	//	�� Ŭ�� ��Ʈ��
	//==================================================================================================
	i3GuiStatic * m_pMyClanMarkImage;			//	��Ŭ�� ��ũ �̹���
	i3GuiStatic * m_pMyClanMasterRankImage;		//	��Ŭ�� ������ ��� �̹���
	i3GuiStatic * m_pMyClanRankImage;			//	��Ŭ�� ��� �̹���
	i3GuiStatic * m_pMyClanExpBar;				//	��Ŭ�� ����ġ ������

	i3GuiStatic * m_pMyClanAddressBullet;

	i3GuiEditBox * m_pMyClanNameStatic;			//	Ŭ����
	i3GuiEditBox * m_pMyClanNameValue;
	i3GuiEditBox * m_pMyClanStaffStatic;		//	Ŭ�� �ο���
	i3GuiEditBox * m_pMyClanStaffValue;
	i3GuiEditBox * m_pMyClanMasterStatic;		//	Ŭ�� ������
	i3GuiEditBox * m_pMyClanMasterValue;
	i3GuiEditBox * m_pMyClanRankStatic;			//	Ŭ�� ���
	i3GuiEditBox * m_pMyClanRankValue;
	i3GuiEditBox * m_pMyClanExpStatic;			//	Ŭ�� ����ġ
	i3GuiEditBox * m_pMyClanExpValue;
	i3GuiEditBox * m_pMyClanOrderStatic;		//	Ŭ�� ����
	i3GuiEditBox * m_pMyClanOrderValue;
	i3GuiEditBox * m_pMyClanAddressStatic;		//	Ŭ�� ����Ʈ
	i3GuiEditBox * m_pMyClanAddressValue;		
	i3GuiEditBox * m_pMyClanPointStatic;		//	Ŭ�� ����Ʈ
	i3GuiEditBox * m_pMyClanPointValue;		

	//==================================================================================================
	//	Ŭ������ ��Ʈ��
	//==================================================================================================
	i3GuiButton * m_pInfoHistoryButton;		//	Ŭ������ ��ư
	i3GuiButton * m_pInfoMemberButton;		//	Ŭ����� ��ư
	i3GuiButton * m_pInfoSecessionButton;	//	Ŭ��Ż�� ��ư
	
	i3GuiStatic * m_pInfoHistroyPhase;		//	Ŭ������ ��ư ������
	i3GuiStatic * m_pInfoMemberPhase;		//	Ŭ����� ��ư ������

	i3GuiEditBox * m_pInfoBestListStatic;				//	'��� Ŭ�� ����Ʈ'
	i3GuiEditBox * m_pInfoMemberListStatic;				//	'Ŭ���� ����Ʈ'
	i3GuiEditBox * m_slotBestMemberListHeadSeason;		//	'����' �÷�
	i3GuiEditBox * m_slotBestMemberListHeadTotal;		//	'���' �÷�
	i3GuiEditBox * m_slotClanMemberListHeadClass;		//	'����' �÷�
	i3GuiEditBox * m_slotClanMemberListHeadNick;		//	'�г���' �÷�
	i3GuiEditBox * m_slotClanMemberListHeadState;		//	'����' �÷�
	i3GuiList *	m_pClanMemberScrollList;				//	Ŭ���� ��ũ�� ����Ʈ
	
	i3GuiEditBox * m_pInfoHistoryNoticeStatic;		//	'��������'
	i3GuiEditBox * m_pInfoHistoryNoticeValue;		//	�������� ����
	i3GuiEditBox * m_pInfoHistoryListHeadSeasonScore;	//	'���� ����' �÷�
	i3GuiEditBox * m_pInfoHistoryListHeadTotalScore;	//	'��� ����' �÷�

	INT32 m_idxClanInfoMemberStartSlot;

	BestMemberSlot m_slotBestMemberList[BEST_MEMBER_SLOT_MAX];				//	Ŭ������ ��� Ŭ���� ����Ʈ
	ClanMemberSlot m_slotClanMemberList[CLANINFO_MEMBER_LIST_ROW_MAX];		//	Ŭ������ Ŭ���� ����Ʈ
	ClanHistorySlot m_slotClanHistoryList[CLANINFO_HISTORY_LIST_ROW_MAX];	//	Ŭ������ ���� ����Ʈ

	//==================================================================================================
	//	Ŭ���� ��Ʈ��
	//==================================================================================================
	i3GuiEditBox * m_pBattleMakeTeamStatic;			//	'�� ����'
	i3GuiButton * m_pBattleMakeTeamEntryButton;		//	'�� ���'

	i3GuiEditBox * m_pBattleMakeTeamCombo;			//	�� ��� �ĺ�
	i3GuiButton * m_pBattleMakeTeamComboButton;		//	�� ��� �ĺ� ��ư
	i3GuiPopupList * m_pBattleMakeTeamComboPopup;	//	'5 vs 5'...

	i3GuiList * m_pBattleTeamScrollList;		//	����� ��ũ�� ����Ʈ
	INT32 m_idxBattleTeamListStartSlot;	
	INT32 m_idxBattleTeamMatchType;
	INT32 m_nBattleTeamListSelectedIdx;

	ClanEntryTeamSlot m_slotBattleTeamList[CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX];	//	��ϵ� �� ����

	BOOL m_bUpdateTeamList;
	REAL32 m_fUpdateTeamSection;
	INT32 m_nTeamSection;

	//==================================================================================================
	//	Ŭ��ã�� ��Ʈ��
	//==================================================================================================
	i3GuiEditBox * m_pSearchStatic;
	i3GuiEditBox * m_pSearchInput;
	i3GuiButton * m_pSearchButton;
	i3GuiButton * m_pSearchViewAll;
	i3GuiEditBox	*	m_pClanMembeSerchText;		//	Ŭ��ã�� ���� ���� �ؽ�Ʈ

	i3GuiStatic		*	m_pClanSerchBar;				//	Ŭ��ã�� ���α׷����� 
	i3GuiStatic		*	m_pClanSerchGage;				//	Ŭ��ã�� ���α׷����� ������
	i3GuiEditBox	*	m_pClanSerchText;				//	Ŭ��ã�� ���α׷����� �ؽ�Ʈ

	i3GuiStatic * m_pSearchInfoAddressBullet;

	i3GuiStatic * m_pSearchInfoClanMarkImage;
	i3GuiEditBox * m_pSearchInfoNameStatic;
	i3GuiEditBox * m_pSearchInfoNameValue;
	i3GuiEditBox * m_pSearchInfoMasterStatic;
	i3GuiEditBox * m_pSearchInfoMasterValue;
	i3GuiStatic * m_SearchInfoMasterRankImage;
	i3GuiEditBox * m_pSearchInfoClanRankStatic;
	i3GuiEditBox * m_pSearchInfoClanRankValue;
	i3GuiStatic * m_SearchInfoClanRankImage;
	i3GuiEditBox * m_pSearchInfoStaffStatic;
	i3GuiEditBox * m_pSearchInfoStaffValue;
	i3GuiEditBox * m_pSearchInfoAddressStatic;
	i3GuiEditBox * m_pSearchInfoAddressValue;
	i3GuiEditBox * m_pSearchInfoOrderStatic;
	i3GuiEditBox * m_pSearchInfoOrderValue;
	i3GuiEditBox * m_pSearchInfoExpStatic;
	i3GuiEditBox * m_pSearchInfoExpValue;
	i3GuiStatic * m_SearchInfoExpBar;

	i3GuiEditBox * m_pSearchClanListHeadRank;
	i3GuiEditBox * m_pSearchClanListHeadName;
	i3GuiEditBox * m_pSearchClanListHeadDate;
	i3GuiEditBox * m_pSearchClanListHeadMember;

	i3GuiList * m_pSearchClanListScroll;
	i3GuiStatic * m_pSearchClanListFocus;
	i3GuiStatic * m_pSearchClanListSelect;

	ClanListSlot m_ClanListSlot[CLAN_SEARCH_LIST_SLOT_MAX];

	BOOL m_bNeedUpdateSearchClanList;
	INT32 m_nSearchClanStartSlot;
	INT32 m_nSearchClanSelectedSlot;
	SORT_CLAN_INFO m_SelectedClanInfo;

	//==================================================================================================
	//	Ŭ������ ��Ʈ��
	//==================================================================================================
	i3GuiStatic * m_pManagementWritingBoardPhase;		//	�Խ��� ���� ��ư ������
	i3GuiStatic * m_pManagementMemberPhase;				//	��� ���� ��ư ������
	i3GuiStatic * m_pManagementSubscriptionPhase;		//	���� ���� ��ư ������
	i3GuiStatic * m_pManagementOthersPhase;				//	��Ÿ ���� ��ư ������

	i3GuiButton * m_pManagementWritingBoardButton;		//	�Խ��� ���� ��ư
	i3GuiButton * m_pManagementMemberButton;			//	��� ���� ��ư
	i3GuiButton * m_pManagementSubscriptionButton;		//	���� ���� ��ư
	i3GuiButton * m_pManagementOthersButton;			//	��Ÿ ���� ��ư

	i3GuiEditBox * m_pManagementWritingBoardNoticeStatic;	//	�������� - 0
	i3GuiEditBox * m_pManagementWritingBoardNoticeEdit;		//	�������� �Է¶� - 0
	i3GuiEditBox * m_pManagementWritingBoardGuideStatic;	//	Ŭ���Ұ� - 0
	i3GuiEditBox * m_pManagementWritingBoardGuideEdit;		//	Ŭ���Ұ� �Է¶� - 0
	i3GuiButton * m_pManagementWritingBoardNoticeModify;	//	�������� �����Ϸ� ��ư - 0
	i3GuiButton * m_pManagementWritingBoardGuideModify;		//	Ŭ���Ұ� �����Ϸ� ��ư - 0

	i3GuiEditBox * m_pManagementMemberStatic;			//	'Ŭ���� ����Ʈ'
	i3GuiEditBox * m_pManagementListHeadNick;			//	'�г���' �÷�
	i3GuiEditBox * m_pManagementListHeadClass;			//	'����' �÷�
	i3GuiEditBox * m_pManagementListHeadEntryDay;		//	'������' �÷�
	i3GuiCheckBox * m_pManagementListHeadCheckAll;		//	����Ʈ ��� �׸� üũ�ڽ�
	i3GuiList *	m_pManagementMemberScrollList;			//	Ŭ���� ���� ��ũ�� ����Ʈ

	i3GuiEditBox * m_pManagementSubscriberStatic;		//	'���Դ�� ���� ����Ʈ'
	i3GuiEditBox * m_pSubscriberListHeadNick;			//	'�г���' �÷�
	i3GuiEditBox * m_pSubscriberListHeadEntryDay;		//	'��û��' �÷�
	i3GuiCheckBox * m_pSubscriberListHeadCheckAll;		//	����Ʈ ��� �׸� üũ�ڽ�
	i3GuiList *	m_pManagementSubscriberScrollList;		//	���� ����� ���� ��ũ�� ����Ʈ
	i3GuiStatic * m_pManagementSubscriberFocus;
	i3GuiStatic * m_pManagementSubscriberSelection;

	i3GuiEditBox * m_pManagementInstructionStatic;		//	'���õ� Ŭ����'
	i3GuiEditBox * m_pManagementInstructionCombo;
	i3GuiButton * m_pManagementInstructionButton;
	i3GuiPopupList * m_pManagementInstructionPopup;

	i3GuiEditBox * m_pSubscriptionInstructionStatic;	//	'���õ� ����'
	i3GuiEditBox * m_pSubscriptionInstructionCombo;
	i3GuiButton * m_pSubscriptionInstructionButton;
	i3GuiPopupList * m_pSubscriptionInstructionPopup;

	i3GuiEditBox * m_pManagementOthersAuthorityStatic;				//	'���� ���� ����'
	i3GuiEditBox * m_pManagementOthersWritingAuthorityStatic;		//	'�Խñ� ���� ����'
	i3GuiEditBox * m_pManagementOthersInviteAuthorityStatic;		//	'Ŭ�� �ʴ� ����'
	i3GuiEditBox * m_pManagementOthersDimissalAuthorityStatic;		//	'Ŭ���� ���� ����'
	i3GuiEditBox * m_pManagementOthersSubscriptionAuthorityStatic;	//	'���� ���� ����'
	i3GuiCheckBox * m_pManagementOthersWritingAuthorityCheck;		//	�Խñ� ���� ���� üũ�ڽ�
	i3GuiCheckBox * m_pManagementOthersInviteAuthorityCheck;		//	Ŭ�� �ʴ� ���� üũ�ڽ�
	i3GuiCheckBox * m_pManagementOthersDimissalAuthorityCheck;		//	Ŭ���� ���� ���� üũ�ڽ�
	i3GuiCheckBox * m_pManagementOthersSubscriptionAuthorityCheck;	//	���� ���� ���� üũ�ڽ�	
	i3GuiEditBox * m_pManagementOthersSendNoteStatic;				//	'Ŭ�� ��ü ���� ������'
	i3GuiButton * m_pManagementOthersSendNoteButton;				//	'���� ������'
	i3GuiEditBox * m_pManagementOthersAdmissionLimitStatic;			//	'Ŭ�� ���� ���� ����
	i3GuiEditBox * m_pManagementOthersAdmissionLimitRankStatic;		//	Ŭ�� ���� ���� ���� : '���'
	i3GuiEditBox * m_pManagementOthersAdmissionLimitRankCombo;		//	Ŭ�� ���� ���� ���� ��� �ĺ�
	i3GuiButton * m_pManagementOthersAdmissionLimitRankButton;
	i3GuiPopupList * m_pManagementOthersAdmissionLimitRankPopup;	
	i3GuiEditBox * m_pManagementOthersAdmissionLimitAgeStatic;	//	Ŭ�� ���� ���� ���� : '����'
	i3GuiEditBox * m_pManagementOthersAdmissionLimitAgeCombo[ADMISSION_LIMIT_CONTROL_COUNT];	//	Ŭ�� ���� ���� ���� ���� �ĺ�
	i3GuiButton * m_pManagementOthersAdmissionLimitAgeButton[ADMISSION_LIMIT_CONTROL_COUNT];
	i3GuiPopupList * m_pManagementOthersAdmissionLimitAgePopup[ADMISSION_LIMIT_CONTROL_COUNT];	
	i3GuiEditBox * m_pManagementOthersChangeClanNameStatic;			//	'Ŭ���� �����ϱ�'
	i3GuiButton * m_pManagementOthersChangeClanNameButton;			//	'Ŭ���� ����'
	i3GuiEditBox * m_pManagementOthersChangeClanMarkStatic;			//	'Ŭ�� ��ũ �����ϱ�'
	i3GuiButton * m_pManagementOthersChangeClanMarkButton;			//	'Ŭ�� ��ũ ����'
	i3GuiEditBox * m_pManagementOthersDissolutionStatic;	//	'Ŭ�� ��ü�ϱ�'
	i3GuiButton * m_pManagementOthersDissolutionButton;		//	'��ü�ϱ�'
	i3GuiButton * m_pManagementOthersModifyButton;		//	 �����ϱ�
		
	INT32 m_idxClanManagementMemberStartSlot;
	INT32 m_idxClanManagementSubscriberStartSlot;
	INT32 m_nLastedSelectSlot;
	INT32 m_authItemFlag;
	
	ManagementClanMemberInfo m_ManageMemberInfo[MAX_MEMBER_COUNT];
	ManagementSubscriberInfo m_ManageSubscriberInfo[MAX_REQUEST_COUNT];

	i3BinList m_listManageMember;
	i3BinList m_listManageSubscriber;

	ClanManagementSlot m_slotManagementList[MANAGEMENT_LIST_ROW_MAX];		//	Ŭ������ Ŭ���� ����Ʈ
	ClanSubscriptionSlot m_slotSubscriptionList[SUBSCRIPTION_LIST_ROW_MAX];	//	Ŭ������ ���Դ�� ���� ����Ʈ
	
	void _InitMyClanWin(void);
	void _InitClanManagementWin(void);

	void _InitClanInfoPhase(void);
	void _InitClanBattlePhase(void);
	void _InitClanSearchPhase(void);
	void _InitClanManamementPhase(void);

	void _InitMyClanInfo(void);
	void _InitClanInfoHistory(void);
	void _InitClanInfoMember(void);

	void _InitClanManagementWritingBoard(void);
	void _InitClanManagementMember(void);
	void _InitClanManagementSubscriber(void);
	void _InitClanManagementOthers(void);

	void OnClanInfoTab(void);
	void OnClanBattleTab(void);
	void OnClanSearchTab(void);
	void OnClanManagementTab(void);

	void OnClanChatPopup(INT32 idx);
	void OnClanChatType(void);

	void OnClanInfoHistoryButton(void);					//	Ŭ�� ���� ��ư
	void OnClanInfoMemberButton(void);					//	Ŭ�� ��� ��ư
	void OnClanInfoSecessionButton(void);				//	Ż�� ��ư

	void OnClanBattleTeamEntry(void);					//	�� ���
	void OnClanBattleTeamTypePopup(INT32 idx);			//	�� ���� �˾�
	void OnClanBattleTeamJoin(INT32 slot);				//	�����ϱ� ��ư
	void OnClanBattleTeamTypeButton(void);
	void OnClanBattleTeamListScroll(void);

	void OnSortSearchClanListByRank(void);
	void OnSortSearchClanListByName(void);
	void OnSortSearchClanListByDate(void);
	void OnSortSearchClanListByMember(void);
	void OnSearchClanListScroll(void);
	void OnSearchClanKeyword(void);
	void OnSearchClanViewAll(void);
	void OnSearchClanFocus(INT32 idx);
	void OnSearchClanSelect(INT32 idx);

	void OnClanManagementWritingBoardButton(void);		//	Ŭ�� ���� �Խ��� ��ư
	void OnClanManagementMemberButton(void);			//	Ŭ���� ���� ��ư
	void OnClanManagementSubscriptionButton(void);		//	Ŭ�� ���� ���� ��ư
	void OnClanManagementOthersButton(void);			//	Ŭ�� ��Ÿ ���� ��ư

	void OnManagementInstructionButton(void);			//	Ŭ���� ���� ��� ��ư
	void OnSubscriptionInstructionButton(void);			//	Ŭ�� ������ ���� ��� ��ư
	void OnManagementInstructionPopup(INT32 nItemIdx);		
	void OnSubscriptionInstructionPopup(INT32 nItemIdx);

	void OnClanManagementBoardNoticeModify(void);		//	�������� �����Ϸ�
	void OnClanManagementBoardGuideModify(void);		//	Ŭ���Ұ� �����Ϸ�

	void OnManagementOthersSendNote(void);				//	Ŭ�� ��ü �ʱ� ������
	void OnManagementOthersChangeClanName(void);		//	Ŭ���� ����
	void OnManagementOthersChangeClanMark(void);		//	Ŭ�� ��ũ ����
	void OnManagementOthersDissolution(void);			//	Ŭ�� ��ü�ϱ�
	void OnManagementOthersModify(void);				//	Ŭ�� ��Ÿ ���� �����ϱ�
	void OnManagementOthersAuthorityCheck(INT32 nIdx);	//	Ŭ�� ���� ���� üũ
	void OnManagementOthersAdmissionLimitOpen( ADMISSION_LIMIT_TYPE type);							//	Ŭ�� ���� ����
	void OnManagementOthersAdmissionLimitInstruction( ADMISSION_LIMIT_TYPE type, INT32 nItemIdx);	//	Ŭ�� ���� ����
	

	void OnClanInfoMemberScroll(void);
	void OnClanInfoMemberContext(INT32 nItemIdx);
	void OnClanManagementMemberScroll(void);	
	void OnClanManagementSubscriberScroll(void);

	void OnClanManagementMemberCheck(INT32 nCheckIdx);
	void OnClanManagementMemberCheckAll(void);
	void OnClanManagementSubscriberCheck(INT32 nCheckIdx);
	void OnClanManagementSubscriberCheckAll(void);

	void OnClanManagementSubscriberFocus(INT32 nIdx);
	void OnClanManagementSubscriberSelect(INT32 nIdx);
	void OnClanManagementSubscriberConfirm(INT32 nIdx);	

	void OnSortClanMemberListByLevel(void);
	void OnSortClanMemberListByName(void);
	void OnSortClanMemberListByState(void);
	void OnSortClanMemberListByDate(void);

	void OnSortSubscriberListByName(void);
	void OnSortSubscriberListByDate(void);

	void OnLeave(void);

	void _UpdateMyClanPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanInfoPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanBattlePhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanSearchPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanManagementPhase(RT_REAL32 rDeltaSeconds);

	void _UpdateClanSerchRate( REAL32 rDeltaSeconds);

	void _UpdateClanInfoHistoryPhase(RT_REAL32 rDeltaSeconds);	
	void _UpdateClanTopBast();					// ���� ������ ���Դϴٸ�, ClanServer ���� �۾������� ������� ���Դϴ�. // 20080423 ������
	void _UpdateClanInfoMemberPhase(RT_REAL32 rDeltaSeconds);

	void _UpdateClanManagementWritingBoardPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanManagementMemberPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanManagementSubscriberPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanManagementOthersPhase(RT_REAL32 rDeltaSeconds);
	
	void _UpdateSortClanMemberList(void);
	void _UpdateSortSubscriberList(void);

	void _SetBestMember(BEST_MEMBER_SLOT slot, const CLAN_MEMBER_BASIC_INFO * pSeasonMember, const CLAN_MEMBER_BASIC_INFO * pTopMember);
	
	void _ResetClanInfoMemberListSlot(INT32 nIdx);	
	void _SetClanInfoMemberListSlot(INT32 nIdx, ManagementClanMemberInfo * pMember);

	void _UpdateClanBattleTeamSlot(INT32 idx, CLAN_BATTLE_TEAM_LIST * pTeamInfo);

	void _ResetManagementListSlot(INT32 nIdx);
	void _SetManagementListCheckAll(BOOL bCheck);
	void _SetManagementListSlot(INT32 nIdx, ManagementClanMemberInfo * pMember);

	void _ResetSubscriptionListSlot(INT32 nIdx);
	void _SetSubscriptionListCheckAll(BOOL bCheck);
	void _SetSubscriptionListSlot(INT32 nIdx, ManagementSubscriberInfo * pSubscriber);

	void _SetSearchClanName(const char * pszString);
	void _SetSearchClanStaff(INT32 nNum, INT32 nTotal);
	void _SetSearchClanMasterName(const char * pszString, INT32 nRank);
	void _SetSearchClanRank(UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank);
	void _SetSearchClanOrder(INT32 nCurValue, INT32 nPrevValue);
	void _SetSearchClanExp(UINT32 nCurValue, UINT32 nMaxValue);
	void _SetSearchClanAddress(const char * pszString);
	void _SetSearchClanMark(UINT32 nFlag);
	void _SetSearchClanListSlot(INT32 idx, CLAN_BASIC_INFO * pInfo);
	void _UpdateSearchClanList(void);	
	void _UpdateSearchClanDetailedInfo(void);
	void _SearchClanList(const char * pszKeyword);
	void _SetSearchHilightClanList(INT32 idx);
	void _RebuildSearchClanListScrollBar(void);
	BOOL _FindSearchHilightClanList(void);
		 
	//	ä��
	void _ProcessChat( REAL32 rDeltaSeconds);
	void _UpdateMacroChat(void);
	void _UpdateParsingType(void);
	void _UpdateLang(void);
	void _UpdateChatBuffer(void);

public:
	CStepClanMember();
	virtual ~CStepClanMember();

	virtual void OnPreGuiNotify(void);	
	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);	
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

	virtual void OnStart(void);
	virtual void OnEnd(void);
	virtual void OnInitRun(void);
	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);
	virtual void AuthStartSuccess(INT32 arg);

	static CLAN_SORT_TYPE m_sInfoClanMemberSortType;
	static CLAN_SORT_TYPE m_sManageClanMemberSortType;
	static CLAN_SORT_TYPE m_sManageSubscriberSortType;
	
	static INT32 cbSortManageMemberListByName(void* item1, void* item2);
	static INT32 cbSortManageMemberListByLevel(void* item1, void* item2);
	static INT32 cbSortManageMemberListByDate(void* item1, void* item2);

	static INT32 cbSortInfoMemberListByName(void* item1, void* item2);
	static INT32 cbSortInfoMemberListByLevel(void* item1, void* item2);
	static INT32 cbSortInfoMemberListByState(void* item1, void* item2);

	static INT32 cbSortSubscriberListByName(void* item1, void* item2);
	static INT32 cbSortSubscriberListByDate(void* item1, void* item2);

	void EnterServer(void);
	void LeaveServer(void);

	//	�� Ŭ�� ������
	void SetMyClanName(char * pszString, UINT8 cColor );					//	�� Ŭ�� �̸� ����
	void SetMyClanRank(UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank);	//	Ŭ�� ��� ȣĪ
	void SetMyClanOrder(INT32 nCurOrder, INT32 nPrevOrder);	//	�� Ŭ�� ���� ����
	void SetMyClanStaff(INT32 nNum, INT32 nTotal);			//	�� Ŭ���� �� ����
	void SetMyClanMaster(char * pszName, UINT8 nRank);		//	�� Ŭ�� ������ ����
	void SetMyClanExp(UINT32 nCurExp, UINT32 nMaxExp);		//	�� Ŭ�� ����ġ ����
	void SetMyClanAddress(char * pszAddress);				//	�� Ŭ�� ����Ʈ URL
	void SetMyClanMark(UINT32 nFlag);						//	�� Ŭ�� ��ũ
	void SetMyClanPoint(REAL32 fPoint);						//	�� Ŭ�� ����Ʈ

	//	���� ������
	void SetClanInfoHistoryMatch( INT32 nSeason, INT32 nTotal);		//	��ġ ��
	void SetClanInfoHistoryWin(INT32 nSeason, INT32 nTotal);		//	�¸� ��
	void SetClanInfoHistoryLose(INT32 nSeason, INT32 nTotal);		//	�й� ��
	void SetClanInfoHistoryMatches(REAL32 fSeason, REAL32 fTotal);	//	�·�	

	// ������ư ���
	void OnWhisper(INT32 memberIdx);
	void OnInviteRoom(INT32 memberIdx);
	void OnVisit(INT32 memberIdx);
	void OnAddFriend(INT32 memberIdx);	
};

#endif // _STEP_CLAN_MEMBER__H
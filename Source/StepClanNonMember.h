#if !defined( _STEP_CLAN_NON_MEMBER_H__)
#define _STEP_CLAN_NON_MEMBER_H__

#include "StepBase.h"
#include "StageDef.h"
#include "ClanContext.h"

typedef struct _tagClanInfoSlot
{
	i3GuiStatic	*	_slot;
	i3GuiStatic	*	_pRank;
	i3GuiStatic	*	_ClanMarkImage;
	i3GuiEditBox*	_ClanName;
	i3GuiEditBox*	_ClanEntryDay;
	i3GuiEditBox*	_ClanMember;
}CLAN_NMB_INFO;


#define	CLANFIND_LIST_ROWMAX 16

class CStepClanNonMember : public CStepBase
{
	I3_CLASS_DEFINE(CStepClanNonMember);

protected:
	CClanContext	*	m_pClanContext;

	i3GuiStatic		*	m_pClanNonMemberRoot;

	// Ŭ�� ����
	i3GuiStatic		*	m_pClanSubcribeWin;				//	�̰�����/Ŭ������ â
	i3GuiEditBox	*	m_pClanNonMemberCaption;		//	'Ŭ�� ã��'
	i3GuiEditBox	*	m_pClanNonMembeSerchText;		//	Ŭ��ã�� ���� ���� �ؽ�Ʈ

	i3GuiStatic		*	m_pClanSerchBar;				//	Ŭ��ã�� ���α׷����� 
	i3GuiStatic		*	m_pClanSerchGage;				//	Ŭ��ã�� ���α׷����� ������
	i3GuiEditBox	*	m_pClanSerchText;				//	Ŭ��ã�� ���α׷����� �ؽ�Ʈ

	i3GuiButton		*	m_pClanSubscription;			// Ŭ�� ����
	i3GuiButton		*	m_pClanCreate;					// Ŭ�� ����

	//
	i3GuiStatic		*	m_pClanMark;					// Ŭ����ũ
	i3GuiStatic		*	m_pClanMarkFrame;				// Ŭ����ũ ������
	
	i3GuiEditBox	*	m_pClanNameStatic;				// Ŭ���� :
	i3GuiEditBox	*	m_pClanNameValue;				// Ŭ����

	i3GuiEditBox	*	m_pClanStaffStatic;					// �ο���
	i3GuiEditBox	*	m_pClanStaffValue;

	i3GuiEditBox	*	m_pClanMasterStatic;				// Ŭ�� ������ :
	i3GuiEditBox	*	m_pClanMasterValue;					// Ŭ����
	i3GuiStatic		*	m_pClanMasterRankImage;				// Ŭ�� �������� ��� �̹��� ǥ��

	i3GuiEditBox	*	m_pClanRankStatic;					// Ŭ�� ���� :
	i3GuiEditBox	*	m_pClanRankValue;					// 100��
	i3GuiStatic		*	m_pClanRankImage;					// Ŭ���� ��� �̹��� ǥ��

	i3GuiEditBox	*	m_pClanExpStatic;					// Ŭ�� ����ġ
	i3GuiEditBox	*	m_pClanExpValue;					//

	i3GuiEditBox	*	m_pClanOrderStatic;					// Ŭ�� ����
	i3GuiEditBox	*	m_pClanOrderValue;			

	i3GuiEditBox	*	m_pClanExpBarFrame;					// Ŭ�� ����ġ �� ������
	i3GuiStatic		*	m_pClanExpBar;

	i3GuiEditBox	*	m_pClanAddressStatic;				// Ŭ�� ����Ʈ �ּ� URL
	i3GuiEditBox	*	m_pClanAddressValue;				// Ŭ�� ����Ʈ �ּ� URL
	
	i3GuiStatic		*	m_pClanIntroduceFrame;				// Ŭ�� �Ұ� ����
	i3GuiEditBox	*	m_pClanIntroduceValue;				// Ŭ�� �Ұ� ��

	i3GuiStatic		*	m_pClanSearchBullet;		
	i3GuiEditBox	*	m_pClanSearchStatic;				//	Ŭ���� �˻�
	i3GuiEditBox	*	m_pClanSearchEdit;
	i3GuiButton		*	m_pClanSearchButton;

	// Ŭ��
	i3GuiStatic		*	m_pClanListWin;						//	Ŭ������ â
	i3GuiEditBox	*	m_pClanListCaption;					//	'Ŭ��'
	
	i3GuiButton		*	m_pClanListTab;						// Ŭ�� ����Ʈ ��ư
	i3GuiButton		*	m_pUserListTab;						// ���� ����Ʈ ��ư
	i3GuiButton		*	m_pViewAllButton;					// ��ü����

	i3GuiStatic		*	m_pClanListPhase;					// Ŭ������Ʈ ������
	i3GuiStatic		*	m_pUserListPhase;					// ���� ����Ʈ ������

	i3GuiList		*	m_pClanListScrollList;				// Ŭ�� ����Ʈ ��ũ�� 
	i3GuiList		*	m_pUserListScrollList;				// ���� ����Ʈ ��ũ��
	CLAN_NMB_INFO		m_ClanInfo[CLANFIND_LIST_ROWMAX];	//Ŭ�� ����Ʈ�� �� ������ ����

	// Ŭ�� ����Ʈ ���
	i3GuiEditBox	*	m_pClanListHeadRank;				// Ŭ�� ����Ʈ ��� : Ŭ�����
	i3GuiEditBox	*	m_pClanListHeadClanName;			// Ŭ�� ����Ʈ ��� : Ŭ�� �̸�
	i3GuiEditBox	*	m_pClanListHeadEntryDay;			// Ŭ�� ����Ʈ ��� : Ŭ�� ������
	i3GuiEditBox	*	m_pClanListHeadMember;				// Ŭ�� ����Ʈ ��� : Ŭ�� �����

	// ���� ����Ʈ ���
	i3GuiEditBox	*	m_pUserListHeadTitle;				// ���� ����Ʈ ��� : Ÿ��Ʋ

	// ������
	i3GuiButton		*	m_pLeaveButton;

	i3GuiStatic*		m_pFocusedSlot;
	i3GuiStatic*		m_pSelect;

	//	�̰����� ä��
	i3GuiPopupList *	m_pChatComboPopup;
	
protected:
	INT32				m_nClanListStartIndex;
	INT32				m_nClanListPageIndex;

	INT32				m_nUserListStartIndex;
	INT32				m_nUserListPageIndex;

	INT32				m_nCurFocusSlotIdx;
	INT32				m_nCurSelectedSlotIdx;

	BOOL				m_bChangedClanSlot;

	INT32				m_nCurDisplayingClanDetailInfo;		//���� ���������ִ� Detailed Info�� dbindex
	BOOL				m_bNeedUpdateClanList;

	SORT_CLAN_INFO 		m_SelectedClanInfo;
	
public:
	CStepClanNonMember();
	virtual ~CStepClanNonMember();

	virtual void OnPreGuiNotify(void);	
	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);	
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

	virtual void OnStart(void);
	virtual void OnEnd(void);
	virtual void OnInitRun(void);
	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);

	void EnterServer(void);
	void LeaveServer(void);

	void SetClanName(const char * pszString, UINT8 cColor );
	void SetClanRank(UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank);
	void SetClanOrder(INT32 nCurValue, INT32 nPrevValue);
	void SetClanStaff(INT32 nNum, INT32 nTotal);
	void SetClanMasterName(const char * pszString, INT32 nRank);
	void SetClanExp(UINT32 nCurValue, UINT32 nMaxValue); 
	void SetClanAddress(const char * pszString);
	void SetClanMark(UINT32 nFlag);
	
	void OnSortClanListByRank(void);
	void OnSortClanListByName(void);
	void OnSortClanListByDay(void);
	void OnSortClanListByMember(void);
	void OnSearchClanList(void);
	void OnViewAll(void);

protected:
	void	_UpdateClanList( REAL32 rDeltaSeconds);
	void	_UpdateClanSerchRate( REAL32 rDeltaSeconds);

	void	_InitClanListTab( void);
	void	_InitUserListTab( void);

	void	_InitFindClanPage( void);
	void	_InitClanPage( void);

	void	_OnClanCreate();
	void	_OnClanSubscription();
	void	_OnLeave();

	void	_OnClanListOnMouse( INT32 idx);
	void	_OnClanListOnClicked( INT32 idx);
	void	_OnClanListDoubleClick( INT32 idx);
	void	_UpdateClanDetailedInfo(void);
	void	_OnClanListScroll(void);
	void	_OnUserListScroll(void);

	void	_SetClanListSelectedHilight( INT32 idx);
	BOOL	_SearchHilightClanList(void);

	void	_OnUserListTab(void);

	void	_SearchClanListView(const char * pszKeyword);
	void	_RebuildClanListScrollBar(void);
};

#endif // _STEP_CLAN_NON_MEMBER__H
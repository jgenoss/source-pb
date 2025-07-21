#if !defined( __UI_TAB_PROFILE_MEDAL_H__)
#define __UI_TAB_PROFILE_MEDAL_H__

#include	"UITabProfileBase.h"
#include	"UILBSlot.h"
#include	"MedalManager.h"

#define MEDAL_DETAIL_INFO_MAX_SLOT 4

class UIBlinkLVButtons;
class UILoadingIcon;

class UITabProfileMedal : public UITabProfileBase
{
	I3_CLASS_DEFINE( UITabProfileMedal, UITabProfileBase );

public:
	UITabProfileMedal();
	~UITabProfileMedal();

	virtual void	OnCreate( i3GameNode * pParent ) override;
	virtual bool	OnEntranceStart( void * pArg1, void * pArg2 ) override;
	virtual void	OnEntranceEnd() override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual bool	OnExitStart() override;
	virtual void	OnExitEnd() override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg , const i3::user_data& UserData) override;

public:		// �ʱ�ȭ �Լ�
	void	InitScroll();
	void	InitViewList()		{ if( m_pviewList != nullptr) m_pviewList->clear();		}
	void	InitRewardList()	{ if( m_prewardList != nullptr) m_prewardList->clear();	}
	void	InitSortType();
	//void	InitLVButtonState();
	void	InitTab();
	void	InitNotify();
	void	MakeViewList();
	void	SetMedalScrollFocus();

private:
	void	_SetDetailInfoSlot();
	void	SetRewardMedalPage();
	void	SetRewardMedalSlot(INT32 idx, INT32 rewardmedal);
	void	SetRewardPageControl();
	void	SetAchievementRate();									// �޴� ����Ʈ �޼��� ����
	void	UpdateToolTip(INT32 idx);

public:
	void	_PushNotifyMedal(INT32 idx);
	void	SelectedMedalSlot(INT32 idx);
	void	UpdateRewardMedalList();
	void	MoveRewardPage(INT32 idx);
	void	ChangeRewardPage(INT32 idx);
	void	SetRewardToolTip(INT32 idx);
	void	CloseRewardToolTip();

public:		// ���� �ݹ� ��� �Լ�
	void	OnLoadSlot(INT32, i3UIScene*);
	void	OnUnloadSlot(INT32, i3UIScene*);
	void	OnEnabledSlot(INT32, i3UIScene*);
	void	OnDisabledSlot(INT32, i3UIScene*);
	void	OnEnterSlot(INT32, i3UIScene*);
	void	OnLeaveSlot(INT32, i3UIScene*);
	bool	EnableMouseOver() { return true; }

public:		// �޴� ������ �Լ�
	void	UpdateMedalInfo(UINT8 lv = 0xff, bool inout = true);
	void	UpdateMedalTab();
	void	ChangeTab( MEDALTAB eTab );		// �޴� �� ����
	void	ChangeSortType(INT32);			// �޴� ���� ����
	void	ChangeLevel(INT32, bool bEvent = true);				// �޴� ���� Ŭ��
	//bool	SetSelIdx(INT32 idx);				// �޴� ���� ����
	void	GetReward(INT32);				// ���� ȹ�� ��ư Ŭ��
	void	AllGetReward();
	void	NotifySelected(INT32);			// �˸��� ����
	void	NotifySelectedClear(INT32);			// �˸��� ���� ����
	void	NotifyAllClear();				// �˸��� ��ü ����
	bool	isChecked(INT32, UINT16);
	void	CheckSlotEnable();				// ���� üũ�ڽ� Ȱ��/��Ȱ�� üũ

	void	SetComboEditBoxText(INT32 sortType);	// �ܺο��� �޺��ڽ� EditBox �̸��� ������ ����
	void	setMedalTab(INT32 idx);

public:		// �Է� �Լ�(��� InputEvent)
	void	Click_MedalSlot(INT32);
	void	DoubleClick_MedalSlot(INT32);
	void	Check_MedalSlot(INT32, bool);
	void	UpdateWheel(INT32);				// ���콺 ��
	void	UpdateScroll(INT32 idx);					// ��ũ�� �̺�Ʈ

private:
	INT32	getSlotRow();
	void	SetMedalPushed(UINT8);			// ������ �ϴ� ������ ��ư ���� ����
	void	UpdateBlinkingLVButtons();

private:
	typedef TUILBSlot_Callback<UITabProfileMedal, &UITabProfileMedal::OnLoadSlot, &UITabProfileMedal::OnUnloadSlot, &UITabProfileMedal::OnEnabledSlot, &UITabProfileMedal::OnDisabledSlot,
		&UITabProfileMedal::OnEnterSlot, &UITabProfileMedal::OnLeaveSlot, &UITabProfileMedal::EnableMouseOver> SlotCallback;
	friend class SlotCallback;
	SlotCallback	m_slotCallback;

	MEDALTAB			m_eTab;								// ���� ���õ� ��
	//UILBSlotSet			m_SlotSet;							// ���� ��Ʈ
	class UIImgSetCtrl*	m_apImgSetCtrl[Medal_SLOT_TOTAL];	// ���� �̹���
	i3UIScrollBar*		m_pMedalScroll;						// �޴������� ��ũ��
	i3UIComboBox*		m_pComboBox;						// �޴� ���� �޺� �ڽ�
	i3UIToolTip*		m_pRewardToolTip;					// ���� ��� ����
	i3UIButtonComposed3*	m_pMedalDetailButton[4];		// 
	INT32				m_selectedIdx;						// �������� �ε���
	INT32				m_viewIdx;							// ȭ����� �ε���
	UINT8				m_gameEventLevel;					// ��Ŷ ���� �� ����� ����

	INT32				m_CurPos;
	INT32				m_OldPos;
	bool				m_bScrollFirst;
	INT32				m_SelectedSlotIdx;

	INT32				m_RewardMedalPage;
	INT32				m_RewardMedalCount;
	INT32				m_RewardMedalCurPage;

	REAL32				m_RewardToolTipHeight;

	i3::vector<MedalViewInfo *>*	m_pviewList;
	i3::vector<MedalViewInfo *>*	m_prewardList;
	NOTIFY_MEDAL		m_NotifyInfo[MAX_NOTIFY_MADAL_COUNT];

	//UIBlinkLVButtons*	m_pBlinkLVButtons;
};

//class UIBlink;

// ����Ʈ â �װ��� LV��ư �����Ÿ� ȿ�� ����.
/*class UIBlinkLVButtons
{
public:
	explicit UIBlinkLVButtons(i3UIScene* pScene);
	~UIBlinkLVButtons();

	void OnUpdate(REAL32 tm);

	void Enable(const MEDAL_LEVEL lv);
	void Disable(const MEDAL_LEVEL lv);

private:
	UIBlink* m_pUIBlink[MEDAL_LEVEL_MAX];
};*/

#endif
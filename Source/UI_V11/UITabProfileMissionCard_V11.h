#pragma once


// ������ �� 1.0/1.1 �̼�ī�� UI ��� �����ⰰ�� �ʴ��ѵ�, v3�� �ű�� �м��ؼ� �ټ� �ٿ��� ��������..������ ���ϰ� �ʹ� ���� ���´�.....
// ���� �м��� �ʿ���..

// �����ڵ尡 �Լ��� �������� �ߺ�ȣ���� �ؽ��ؼ� ���� �� �ϴ� ��...
// �̼�ī��� ����UI�ܿ� �����˾�, �����˾�, HUD ó���� �� �ֱ� ������ ���� ����, ���� ó������ ����..

#include "UITabProfileBase_V11.h"

class UITabProfileMissionCard_V11 : public UITabProfileBase_V11
{
	I3_CLASS_DEFINE( UITabProfileMissionCard_V11, UITabProfileBase_V11);
	typedef UITabProfileBase_V11		base;
private:

	INT32			m_selectedCardSetIndexFromList;		// �̹� ���ŵ� ��Ʈ ����(4)�� ī��� ����Ʈ(10���̻�)���� ������ �ʿ���..
	INT32			m_nextSetTypeForDelete;

	i3UIButtonImageSet*	m_CardSlot[MAX_CARD_PER_CARDSET];
	i3UIButton*		m_CardBG[MAX_CARD_PER_CARDSET];
	i3UIListView_Box*   m_pCardSetListView;
	
	void			UpdateCardSlotsAndInfoUI();
	void			UpdateCardSetListSelection();

public:
	UITabProfileMissionCard_V11();
	virtual ~UITabProfileMissionCard_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE ) override;
	virtual bool	OnClick( i3UIControl * pControl) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;

	/** \brief �ӹ� ���� Update */
	void			UpdatePrize_Lua();
	void			SelectCardSetType_Server( INT32 serverIdx);
	void			SelectCard( INT32 cardIdx);
	void			InitCardList_Lua(void);

	//Luaglue function Link
	void			CardInfo();
	void			CardBuy();
	void			CardDelete();
protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
protected:
	void			_ChangeActiveCardSet(INT32 cardSetType);
	void			_UpdateChallengeSlotState();
	void			_UpdateChallengeCardInfo();
	void			_UpdateCardSlotImage();
private:
	void			SetButtonState(i3UIButton *, I3UI_BUTTON_SHAPE);

	//ī�� �������� ����
	void			SetCCardNotice(INT32, const i3::rc_wstring& wstrText);
	void			SetCCardNoticeColor(INT32, INT32);
	void			SetCCardNoticeEnable(INT32, bool);
	void			SetCCardNoticeCheck(INT32,INT32);
	//�̺�Ʈ ����
	void			SetMissionPossibleCount_Lua();
};

/**
 * \file	UI\UIPopupUserProfile_V11.h
 *
 * Declares the user interface popup user profile class.
 */


#if !defined( __UI_POPUP_USERPROFILE_H__)
#define __UI_POPUP_USERPROFILE_H__

#include "../UI/UIPopupBase.h"
#include "../UI/UIUtil.h"

enum UIP_BTN_STATE
{
	UIP_BTN_STATE_NONE = -1,
	UIP_BTN_STATE_TRACKING,		// ã�ư���
	UIP_BTN_STATE_INVITE,		// �ʴ��ϱ�
	UIP_BTN_STATE_ADDFRIEND,	// ģ���߰�
	UIP_BTN_STATE_ACCEPTFRIEND,	// ģ������
	UIP_BTN_STATE_REFUSEFRIEND,	// ģ������
	UIP_BTN_STATE_REMOVEFRIEND,	// ģ������
	UIP_BTN_STATE_INVITECLAN,	// Ŭ���ʴ�
	UIP_BTN_STATE_INVITEMERC,	// �뺴 �ʴ�

	UIP_BTN_STATE_MAX
};

enum UIP_TAB
{
	UIP_TAB_RECORD = 0,	// ����
	UIP_TAB_EQUIPMENT,	// ���
};

class UIPopupUserProfile_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupUserProfile_V11, UIPopupBase);

private:
	bool			m_bUserDetailInfoFlag;
	INT32			m_SelectedIndex;		// ģ��,Ŭ���� ���õ� ���� �ε���
	INT32			m_ReadyRoomSlotIndex;	// ���濡���� Slot Index

	UIP_BTN_STATE	m_BtnState1;
	UIP_BTN_STATE	m_BtnState2;
	UIP_BTN_STATE	m_BtnState3;
	UIP_BTN_STATE	m_BtnState4;

	USERLIST_TAB	m_UserTab;

	LOBBY_USER_INFO_LIST 	m_CurrentUser;	// �κ��� �Ϲ� ���� ����

	bool			m_bReceivedUserInfo;
	bool			m_bWaitReceive;
	bool			m_bCreateChara;
	bool			m_bClanMercenary; 

	UIP_TAB			m_CurrentTab;

	i3UIFrameWnd *	m_pCharaViewFrame;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	/** \brief ����� ���¿� ���� ��ư Ȱ��ȭ ó�� */
	void			_SetButtonState( bool bClan = false);

	/** \brief UI ��ư �ʱ�ȭ */
	void			_InitializeUI( void);

	// ���� ���� �ʱ�ȭ
	bool			_InitInfoForUserList(void * pArg);
	bool			_InitinfoForSearchUser(void);

	/** \brief �⺻ ���� ���� */
	void			_SetDefault( void);

	/** \brief ���� ���� */
	void				_SetRecord( void);
	USER_INFO_RECORD *	_GetUserRecordInfo(void);

	/** \brief Item ������ ���� */
	void			_SetEquipment( void);
	void 			_GetUserEquipWeapon(USER_INFO_EQUIPMENT_WEAPON & outInfo);
	
	void			_SetCharacter();

	/** \brief ��ư �Է¿� ���� �ൿ�� ó���մϴ�. */
	void			_ProcessClickButton( UIP_BTN_STATE state);


public:
	UIPopupUserProfile_V11();
	virtual ~UIPopupUserProfile_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;
	virtual bool	OnExitStart( void) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;

	virtual void	OnDragStart( i3UIControl * pControl, INT32 x, INT32 y) override;
	virtual void	OnDragging( i3UIControl * pControl, INT32 x, INT32 y) override;

	virtual bool	OnCloseButton(void) override;

	void			UpdateUserInfo( void);

	void			SetUserInfo( void);
	void			SetFriendInfo( void);
	void			SetClanMemberInfo( void);
	void			SetBlockUserInfo( void);

	bool			GetCurrentSelectedUserLocation( CUserLocationInfo & UserLocInfo);

	void			ChangeTab( UIP_TAB tab);
	void			ClickButton( INT32 idx);
	void			AddFriend( void);
	void			BuyViewItem( void);
	void			CharacterChange( void);

	void			DoInviteClan( void);
	void			SetReadyroomIdx(INT32 RoomIdx);

	LOBBY_USER_INFO_LIST *	m_Info;
	LOBBY_USER_INFO_LIST * GetUserInfo(){ return m_Info; };

	//------------------------------------------------------------------------------//
	//								Callback procedure								//
	static void		CbInviteClan(void* pThis, INT32 nParam);
};

#endif

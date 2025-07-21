/**
 * \file	UI\UIPopupUserProfile.h
 *
 * Declares the user interface popup user profile class.
 */


#if !defined( __UI_POPUP_USERPROFILE_H__)
#define __UI_POPUP_USERPROFILE_H__

#include "UIPopupBase.h"
#include "UIUtil.h"
#include "CommunityContext.h"

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
	UIP_BTN_STATE_BLOCK,		// ����
	UIP_BTN_STATE_INVITEMERC,	// �뺴 �ʴ�

	UIP_BTN_STATE_MAX
};

// ����� ����
enum TARGET_STATE
{
	// ����Ÿ�� TSU(TARGET_STATE_USER)
	TSU_TYPE_MYSELF	= 1<<1,		//���ڽ�
	TSU_TYPE_FRIEND	= 1<<2,		//ģ��
	TSU_TYPE_HOLD	= 1<<3,		//ģ�����
	TSU_TYPE_NORMAL	= 1<<4,		//�Ϲ�

	// Ŭ��Ÿ�� TSC(TARGET_STATE_CLAN)
	TSC_TYPE_NONE		= 1<<5, //Ŭ���� ����.
	TSC_TYPE_SAMECLAN	= 1<<6,	//���� Ŭ��
	TSC_TYPE_OTHERCLAN	= 1<<7, //�ٸ� Ŭ��
	TSC_ENABLE_INVITE   = 1<<8, //�ʴ����

	// ������ TSB(TARGET_STATE_BLOCK)
	TSB_STATE_ENABLE	= 1<<9, //�� ��ư ���
	TSB_STATE_BLOCK		= 1<<10, //�� �����

	// ��ġ TSL(TARGET_STATE_LOCATION)
	TSL_USER_ONLINE		= 1 << 11,	//ä��
	TSL_USER_LOBBY		= 1<<12,	//�κ�
	TSL_USER_READY_ROOM	= 1<<13,	//�����
	TSL_TARGET_ONLINE	= 1 << 14,	//��� �κ�
	TSL_TARGET_LOBBY	= 1<<15,	//��� �κ�
	TSL_TARGET_READY_ROOM = 1<<16,	//��� �����
	TSL_TARGET_OFFLINE	= 1<<17,	//����� ��������
	TSL_TARGET_CLAN_CHENNEL = 1<<18,	// ����� Ŭ��ä�ο� ����
	TSL_TARGET_CLAN_MATCH = 1<<19,	// ����� Ŭ�� ��ġ ���濡 ����.
	TSL_SAME_LOCATION	= 1<<20,	//���� ������ġ
};

namespace GameUnitTest{ namespace Popup{ class UserProFile;}};
class UIPopupUserProfile : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupUserProfile, UIPopupBase);

	friend class GameUnitTest::Popup::UserProFile;

private:
	UIP_BTN_STATE	m_BtnState[3];
	USERLIST_TAB	m_UserTab;
	INT32			m_i32SelIdx, m_i32SelSlotIdx;

	i3UIImageBoxEx * m_pClanMark;

	i3::rc_wstring	m_wstrBlockReason;
	i3::rc_wstring	m_wstrUserNickName;		/// [NET_NICK_NAME_SIZE];
	bool	m_bClanMercenary;

protected:

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

	/** \brief ����� ���¿� ���� ��ư Ȱ��ȭ ó�� */
	void			_SetButtonState(void);
	// ��ư�� ��ɺ��� �����ϴ�.
	void			_SetUserInfoButton(void);	// ���� ���� ��ư(���� ����, �ӼӸ� ..)
	void			_SetCommunityButton(void);	// Ŀ�´�Ƽ ��ư (ģ�� �߰�, ����, ���󰡱� ��..)
	void			_SetDefaultButton(void);	// �⺻ ��ư (���� �ѱ��, �ݱ�, GM �ɼ� ��..)

	/** \brief ��ư �Է¿� ���� �ൿ�� ó���մϴ�. */
	void			_ProcessClickButton( UIP_BTN_STATE state);

	void			_SetUserInfo(void);
	void			_SetMerceInfo(void);

	void			_SetNickAndKillDeathRate(i3::stack_wstring& out, INT32 Kill, INT32 Death);
	void			_SetUserLocation(i3::stack_wstring& out, const USER_SIMPLE_INFO * pInfo);
	void			_SetClanMark(UINT32 ui32ClanMark);

	void			_SetMercenaryRecord();

	USERLIST_TAB	_FindRelationTab(void);

private:
	void ClickUserInfo_in_none() {}
	void ClickUserInfo_in_room();
	void ClickUserInfo_in_lobby();
	void (UIPopupUserProfile::*fn_click_user_info)();

public:
	UIPopupUserProfile();
	virtual ~UIPopupUserProfile();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 i3::rc_wstring*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd(void) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;
	virtual bool	OnCloseButton( void) override;

	void			UpdateUserInfo( void);

	void			ClickButton( INT32 idx);
	void			ClickWhisper(void);
	void			ClickDetailInfo(void);
	void			ClickUserInfo(void);
	void			ClickGMButton(void);

	void			AddFriend( void);

	void			DoInviteClan( void);
	void			ChangeHost( void);

	//------------------------------------------------------------------------------//
	//								Callback procedure								//
	static void		CbInviteClan(void* pThis, INT32 nParam);
	static void		CbDeleteFriend(void* pThis,INT32 nParam);

protected:
	bool			_IsUserState( UINT32 _type );		// ����� ����Ȯ��
	void			_AddUserState( UINT32 _type );		// ����� �����߰�
	void			_UpdateUserState();					// ����� ���� ������Ʈ
	void			_UpdateMerceState();
	void			_ResetUserState();					// ����� ���� �ʱ�ȭ

	UINT32			 m_nUTD;
};

#endif

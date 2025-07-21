#if !defined( __UIPOPUP_USEITEM_CHANGECLANMARK_H__)
#define __UIPOPUP_USEITEM_CHANGECLANMARK_H__

#include "UI/UIPopupUseItemBase.h"

#define UI_CHANGE_CLANMARK_SHAPE_SLOT_MAX		5
#define UI_CHANGE_CLANMARK_COLOR_SLOT_MAX		10

class UIPopupUseItemChangeClanMark_V11 : public UIPopupUseItemBase
{
	I3_CLASS_DEFINE( UIPopupUseItemChangeClanMark_V11, UIPopupUseItemBase);

private:
	INT32 m_nBackImageIdx;		//	1 base (0 unknown)
	INT32 m_nShapeImageIdx;		//	1 base (0 unknown)
	INT32 m_nBackColorIdx;		//	1 base (0 unknown)
	INT32 m_nShapeColorIdx;		//	1 base (0 unknown)

	i3UIImageBoxEx *	m_pClanMarkIcon[ MAX_CLAN_MARK_LAYER][ UI_CHANGE_CLANMARK_SHAPE_SLOT_MAX];
	i3UIImageBoxEx *	m_pFinalMark;

	i3Texture *		m_pTexClanMarkFinal;
	i3Texture *		m_pTexClanMarkSelect[ MAX_CLAN_MARK_LAYER][ UI_CHANGE_CLANMARK_SHAPE_SLOT_MAX];

	bool			m_bWaitingForCheck;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);
	virtual LuaState * _SetItemInfo( void);


	void			_CleanupTexture( void);

	void			_ClanMarkUserSelectImageUpdate( CLAN_MARK_LAYER_TYPE layerType = CLAN_MARK_LAYER_FRAME);

	/** \brief	�ϳ��� ������ ��ũ ���� �̹����� �����մϴ�. */
	void			_ChangeSelectedImage( CLAN_MARK_LAYER_TYPE LayerType, INT32 nSlotIdx, INT32 nImageIdx, INT32 nColorIdx);

	void			_ClanMarkUserFinalImage( void);

	/** \brief Ȯ�� ��ư�� ��Ȱ�� ���·� �����մϴ�. */
	void			_SetOKButtonActivateState( bool bActivate);

public:
	UIPopupUseItemChangeClanMark_V11();
	virtual ~UIPopupUseItemChangeClanMark_V11();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);

	virtual void	OnEntranceEnd( void);

	virtual	bool	OnExitStart( void);

	virtual bool	OnOKButton( void);
	/** \brief Change Background color */
	void			ChangeBackImageColor( INT32 colorIdx);

	/** \brief Change Shape Color */
	void			ChangeShapeColor( INT32 colorIdx);

	/** \brief ���� BG�� ���� */
	void			ChangePrevBGImage( void);

	/** \brief ���� BG�� ���� */
	void			ChangeNextBGImage( void);

	/** \brief ���� Shape���� ���� */
	void			ChangePrevShapeImage( void);

	/** \brief ���� Shape����  ���� */
	void			ChangeNextShapeImage( void);

	/** \brief �ߺ� �˻縦 �ǽ��Ѵ�. */
	void			CheckDuplicateMark( void);

private:
	typedef void	(UIPopupUseItemChangeClanMark_V11::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Clan_Duplicate_Mark( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Clan_Change_Mark( INT32 arg, const i3::user_data&);
};

#endif
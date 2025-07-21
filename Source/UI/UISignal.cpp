#include "pch.h"
#include "UISignal.h"


// �ñ׳��� std::function�� �����Ͽ�, ���� ����ü�� �Լ���� ���� �����Ƿ�, �̸��� �Լ��̸����� ���絵 �����մϴ�...



namespace UISignal
{
	
	i3::unique_signal<void()>	UpdateCartBoard;    // "update_cart_board"
	i3::unique_signal<void()>	UpdatePreviewSlot;		// "update_preview_slot"
	i3::unique_signal<void(bool, bool)>	UpdateSlot9;	// "update_slot9"
	

	i3::unique_signal<void(bool)>	FinishRepairItem;	// "finish_repair_item"
	i3::unique_signal<void(bool)> FinishRepairAllItem; // "finish_repair_all_item"

	i3::unique_signal_reference<void(bool)>	CurrentFinishRepairItem = FinishRepairItem;
	
//	namespace detail
//	{
//		i3::unique_signal<void(bool)>* g_pFinishRepairItemDispatch = &FinishRepairItem;		// �ΰ��� ���� �ʵ��� �� �����Ұ�..
//	}
	
	i3::unique_signal<void(bool, const i3::vector< buy::DisplayedGoodsPtr >&)>  FinishBuyItem;			//
	i3::unique_signal<void(bool)> FinishBuyGift;											// ������ �и��մϴ�. (�μ��� �ٸ��ϴ�..)
	i3::unique_signal<void(bool)>	FinishBuyExtendItem;								 // "finish_buy_extend_item"

	i3::unique_signal<void(bool, const SHOP_GOODS*)> FinishAuthItem;					//	"finish_buy_auth_item", "finish_auth_item"
	i3::unique_signal<void(bool, const SHOP_GOODS*)> FinishPreviewAuthItem;			//  "finish_preview_auth_item"
	i3::unique_signal<void(bool, const SHOP_GOODS*)> FinishExtensionBuyAuthItem;		//  "finish_extension_buy_auth_item"
	
	i3::unique_signal_reference<void(bool, const SHOP_GOODS*)> CurrentFinishAuthItem = FinishAuthItem;

//	namespace detail
//	{
//		i3::unique_signal<void(bool, const SHOP_GOODS*)>* g_pFinishAuthItem = &FinishAuthItem;
//	}
	
	i3::unique_signal<void(bool, const ITEM_INFO*)>	FinishRemoveItem;					// "finish_remove_item"

	
	i3::unique_signal<void(bool, bool)> FinishBuyPopup;	// "finish_buy_popup" 
														// UBP_BUYITEM, UPW_BUYITEM ( UIPopupBuyItem, UIBattlePopupBuyItem)�� �μ��� �ѱ⵵�� �Ѵ�.
	i3::unique_signal<void(bool, bool)> FinishExtensionBuyItem;	// "finish_extension_buy_item"
																	// UBP_BUYITEM, UPW_BUYITEM ( UIPopupBuyItem, UIBattlePopupBuyItem)�� �μ��� �ѱ⵵�� �Ѵ�.
	i3::unique_signal<void()> SMenuEquipSlotClear;		//"smenu_equip_slot_clear"

	i3::unique_signal<void(bool)> SMenuEquipClickClose; 	// "smenu_equip_click_close"

	i3::unique_signal<void()> ClanLobbyCreateTeam;		//"clan_lobby_create_team"
	i3::unique_signal<void(INT32)> ClanLobbyJoinTeam;	// "clan_lobby_join_team"
	
	i3::unique_signal<void()> ClanLobbyLeaveTeam;		// "clan_lobby_leave_team"
	i3::unique_signal<void()> ClanLobbyQuickMatch;		// "clan_lobby_quick_match"
	i3::unique_signal<void()> ClanLobbyQuickMatchCancel;		// "clan_lobby_quick_match_cancel"
	i3::unique_signal<void()> ClanLobbyEnterRoomList;	// "clan_lobby_enter_room_list"

	i3::unique_signal<void(INT32)> ClanLobbyScrollTeamList;  // "clan_lobby_scroll_team_list"
	i3::unique_signal<void(INT32)> ClanLobbyChangePerson;    // "clan_lobby_change_person"
	i3::unique_signal<void(const i3::literal_range&)> ClanLobbyChangeOperation;	// "clan_lobby_change_operation"

	i3::unique_signal<void(bool)> EnableCompareDisplayAttr;  // "enable_compare_display_attr"
	i3::unique_signal<void(bool)> SetCompareSkipButton;		// "set_compare_skip_button"
	i3::unique_signal<void()> SMenuAbilityClose;			// "smenu_ability_close"

	i3::unique_signal<void(bool&)>	IsIgnoreShared;			// "is_ingore_shared"   ���� �����糪...ö�� ����.. ���� ���̹��� ���� ���� ������..
															

/*
	void	FinishRepairItemBySelection(bool bArg)
	{
		(*detail::g_pFinishRepairItemDispatch)(bArg);			// �ΰ� �˻����� �ʵ��� �Ű澵 ��..
	}

	void	SelectFinishRepairItemFunction_Single()
	{
		detail::g_pFinishRepairItemDispatch = &FinishRepairItem;
	}

	void	SelectFinishRepairItemFunction_All()
	{
		detail::g_pFinishRepairItemDispatch = &FinishRepairAllItem;
	}

	void	FinishAuthItemBySelection(bool bArg, const SHOP_GOODS* pGood)
	{
		(*detail::g_pFinishAuthItem)(bArg, pGood);
	}

	void	SelectFinishAuthItem_Normal()
	{
		detail::g_pFinishAuthItem = &FinishAuthItem;
	}

	void	SelectFinishAuthItem_Preview()
	{
		detail::g_pFinishAuthItem = &FinishPreviewAuthItem;
	}

	void	SelectFinishAuthItem_Extension()
	{
		detail::g_pFinishAuthItem = &FinishExtensionBuyAuthItem;
	}
	*/

}




#pragma once

// ���������� CBFuction (call_cb / insert_cb) ���� �ϱ� ���� ������ ���� ó���մϴ�...
// �Է¹��ڿ��� �ϵ��ڵ��ӿ��� �ұ��ϰ�, ������ �˻��ϸ鼭 �Լ� ȣ���� �ʿ���ٰ� �ǴܵǸ�, �������� ���ڿ� Ű�� �ʿ��ϴٸ�,
// �׶� �ܺο��� �˻��ڷᱸ���� ������ �����Ͽ� ����ó���Ѵ�... ������ ����...

#include "i3Base/itl/signal/unique_signal.h"
#include "i3Base/itl/signal/unique_signal_reference.h"

namespace buy {typedef i3::shared_ptr <class DisplayedGoods> DisplayedGoodsPtr; }

struct ITEM_INFO;

namespace UISignal
{
	extern i3::unique_signal<void()>	UpdateCartBoard;    // "update_cart_board"
	extern i3::unique_signal<void()>	UpdatePreviewSlot;		// "update_preview_slot"
	extern i3::unique_signal<void(bool, bool)>	UpdateSlot9;	// "update_slot9"

	extern i3::unique_signal<void(bool)>	FinishRepairItem;	// "finish_repair_item"
	extern i3::unique_signal<void(bool)>	FinishRepairAllItem; // "finish_repair_all_item"

	extern i3::unique_signal_reference<void(bool)>		CurrentFinishRepairItem;
		
	extern i3::unique_signal<void(bool, const i3::vector< buy::DisplayedGoodsPtr >&)>  FinishBuyItem;		// "finish_buy_item"  ��������(ĳ���� ���Ե˴ϴ�..)
	extern i3::unique_signal<void(bool)>	FinishBuyExtendItem;								 // "finish_buy_extend_item"

	extern i3::unique_signal<void(bool)> FinishBuyGift;											// ������ �и��մϴ�. (�μ��� �ٸ��ϴ�..)
	extern i3::unique_signal<void(bool, const SHOP_GOODS*)> FinishAuthItem;					//	"finish_buy_auth_item", "finish_auth_item"
	extern i3::unique_signal<void(bool, const SHOP_GOODS*)> FinishPreviewAuthItem;			//  "finish_preview_auth_item"
	extern i3::unique_signal<void(bool, const SHOP_GOODS*)> FinishExtensionBuyAuthItem;		//  "finish_extension_buy_auth_item"

	extern i3::unique_signal_reference<void(bool, const SHOP_GOODS*)> CurrentFinishAuthItem;


	extern i3::unique_signal<void(bool, const ITEM_INFO*)>	FinishRemoveItem;					// "finish_remove_item"


	extern i3::unique_signal<void(bool, bool)> FinishBuyPopup;	// "finish_buy_popup" 
																// UBP_BUYITEM, UPW_BUYITEM ( UIPopupBuyItem, UIBattlePopupBuyItem)�� �μ��� �ѱ⵵�� �Ѵ�.
	extern i3::unique_signal<void(bool, bool)> 	FinishExtensionBuyItem;	// "finish_extension_buy_item"
																// UBP_BUYITEM, UPW_BUYITEM ( UIPopupBuyItem, UIBattlePopupBuyItem)�� �μ��� �ѱ⵵�� �Ѵ�.

	extern i3::unique_signal<void()> SMenuEquipSlotClear;		//"smenu_equip_slot_clear"
	extern i3::unique_signal<void(bool)> SMenuEquipClickClose; 	// "smenu_equip_click_close"

	extern i3::unique_signal<void()> ClanLobbyCreateTeam;		//"clan_lobby_create_team"
	extern i3::unique_signal<void(INT32)> ClanLobbyJoinTeam;	// "clan_lobby_join_team"
	extern i3::unique_signal<void()> ClanLobbyLeaveTeam;		//  "clan_lobby_leave_team"
	extern i3::unique_signal<void()> ClanLobbyQuickMatch;		// "clan_lobby_quick_match"
	extern i3::unique_signal<void()> ClanLobbyQuickMatchCancel;		// "clan_lobby_quick_match_cancel"
	extern i3::unique_signal<void()> ClanLobbyEnterRoomList;	// "clan_lobby_enter_room_list"
	extern i3::unique_signal<void(INT32)> ClanLobbyScrollTeamList;  // "clan_lobby_scroll_team_list"
	extern i3::unique_signal<void(INT32)> ClanLobbyChangePerson;    // "clan_lobby_change_person"
	extern i3::unique_signal<void(const i3::literal_range&)> ClanLobbyChangeOperation;	// "clan_lobby_change_operation"

	extern i3::unique_signal<void(bool)> EnableCompareDisplayAttr;  // "enable_compare_display_attr"
	extern i3::unique_signal<void(bool)> SetCompareSkipButton;		// "set_compare_skip_button"
	extern i3::unique_signal<void()>	 SMenuAbilityClose;			// "smenu_ability_close"

	extern i3::unique_signal<void(bool&)>	IsIgnoreShared;			// "is_ingore_shared"   ���� �����糪...ö�� ����.. ���� ���̹��� ���� ���� ������..
																	// �ϴ� ��ġ�ϰڽ��ϴ�...


}





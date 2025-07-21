#if !defined( __UI_POPUP_SHOPEXTENSION_H__)
#define __UI_POPUP_SHOPEXTENSION_H__

#include "UIPopupBase.h"
#include "UILBScrollSlot.h"
#include "BuyUtil.h"
#include "GameCharaBase.h"
#include "WeaponDetail3DView.h"
#include "i3Base/itl/signal/signal_common.h"

//PBCLIENT_KDY
/************************
2015-12-04 : 
- �ű� UI�� ������ ����.
- 3D VIEW ���� ���� �� ����Ÿ ���� Ŭ������ �и�.
- EFFECT ����.
2015-12-07 :
- �ڵ� ����.
- ���� ������ �ý��� ����.
- ������ ����̿��Ͽ� ������ �ִ��� ����.
2015-12-08
- ��û���� �׽�Ʈ �ý��� ��ȭ�մϴ�.
*************************/

/*								UIPopupShopExtenstion								*/

class UIPopupShopExtension : public UIPopupBase
{

	I3_CLASS_DEFINE( UIPopupShopExtension, UIPopupBase);

public:
	UIPopupShopExtension();
	virtual ~UIPopupShopExtension();

	virtual void OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 const SHOP_GOODS_PACK* or 
	param[in] pArg2 bool*
	*/
	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void OnLoadAllScenes() override;
	virtual bool OnExitStart( void) override;
	virtual void OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool OnKey_Escape(void) override;
	virtual bool OnLostDevice( bool bLostDevice) override;
	virtual bool OnRevive( i3RenderContext * pCtx) override;
	virtual void OnUnloadAllScenes(void) override;

	void clicked(INT32 idx);
	void clicked_combo(INT32 tab);

protected:

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

private:

	void cb_finish_buy_item(bool, bool);
	void cb_finish_buy_auth_item(bool arg1, const SHOP_GOODS* arg2);

	bool	enable_buy();
	void	update_payment();
	void	update_weapon_attribute();
	void	update_weapon_attribute_detail();
	void	update_weapon_class_used(UINT32 itemid);

	void	set_detail_data(int progress_number, INT32 data, float animation_Speed);

	// ���콺�� 3D VIEW ������ üũ �ϴ� �ݹ�.
	void hit_test_enter(UINT32 inx, i3UIControl* pWnd) { is_mouse_pt_in_frame_wnd = true; }
	void hit_test_leave(UINT32 inx, i3UIControl* pWnd) { is_mouse_pt_in_frame_wnd = false; }
	UI::tmiHover_Callback< UIPopupShopExtension, &UIPopupShopExtension::hit_test_enter, &UIPopupShopExtension::hit_test_leave	> m_HitTestCallback;
	UI::tmHover m_tmHitTest;

private:
	WeaponDetail3DView * m_pWeapon3DView;

	i3UIFrameWnd*       view_frame_wnd;

	i3UIStaticText*		text_rate[8];
	i3UIProgressBar*	progress_rate[8];

	CGameCharaBase* m_pChara;

	T_ItemID m_nItemID;
	buy::DisplayedGoodsPtr m_buyable_weapon_item;

	bool is_mouse_pt_in_frame_wnd, is_mouse_leftbtn_down;
	bool isOpenInventory;

	i3::scoped_connection_set		m_SigConnection;

/* ������ �۾��� ���Ͽ� ���� �׽�Ʈ�� UI ( RELEASE && DEBUG �� ) */
#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)

private:
	i3UIStaticText*		output_ui;
	
	void	update_output();
	void	switch_visible();

	virtual bool OnKeyInput(i3InputDeviceManager * pInputMng);

#endif

};

#endif

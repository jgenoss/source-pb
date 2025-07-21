#include "pch.h"
#include "UIHudFirst.h"

#include "UIHudUtil.h"
#include "UIBattlePopupSelectWeapon.h"
#include "UIHudManager.h"
#include "UIHudUtil.h"
#include "UIBattleFrame.h"
#include "UIUtil.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#include "../StageBattle/UserContext.h"
#include "BattleSlotContext.h"

UIHudFirst::UIHudFirst(UIHudManager* p) : iHudBase(p)
{
}

UIHudFirst::~UIHudFirst()
{
}

/*********************************/
void UIHudFirst::InitializeAtLoad( i3UIScene * pScene)
{
	hu_dino::reset_dino_attribute();
}

/*********************************/
void UIHudFirst::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	SetMyChara(g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx()));

	hu::init_m179_object();

	UIBattlePopupSelectWeapon * pPopup = static_cast<UIBattlePopupSelectWeapon*>(GameUI::GetBattlePopup( UBP_SELECT_WEAPON));
	if( pPopup != nullptr)
		pPopup->SetInitList(); //������ �Ǿ��⿡ ���ⱳüâ db�� ���ϵ��� �Ѵ�.

	m_parent->is_complete_respawn = ( UserContext::i()->IsPlayingSlot( s_pMyChara->getCharaNetIndex() ) == false); //���� ������ ��쿣 �ٸ� ��Ŷ�� �� ��� ���� ������ true�� �����Ѵ�.
}

/*virtual*/ void UIHudFirst::event_m_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();
	//���� �� ĳ���͸� ���� -> ���� �ϱ� ������ �ٽ� ���� ��.
	SetMyChara(g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx()));
}

/*virtual*/ void UIHudFirst::event_respawn_battle(INT32 arg, const i3::user_data& UserData)
{
	//���� �� ĳ���͸� ���� -> ���� �ϱ� ������ �ٽ� ���� ��.
	i3_prof_func();

	SetMyChara(g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx()));

	hu::enable_check_primary_weapon_first_shot(); //���� ��ų �˻� ����..

	UIBattleFrame::i()->ClosePopup( UBP_SELECT_WEAPON );
	UIBattleFrame::i()->ClosePopup( UBP_SELECT_CHARA );

	UIBattlePopupSelectWeapon * pPopup = static_cast<UIBattlePopupSelectWeapon*>(GameUI::GetBattlePopup( UBP_SELECT_WEAPON));
	if( pPopup != nullptr)
		pPopup->SetInitList(); //������ �Ǿ��⿡ ���ⱳüâ db�� ���ϵ��� �Ѵ�.
}


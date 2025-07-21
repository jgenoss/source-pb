#include "pch.h"
#include "UIFloatCharacterView_V10.h"

#include "UI/UIMainFrame.h"
#include "UI/UICharaView.h"
#include "UI/UISideMenu.h"
#include "UI/UICharaInfoMgr.h"
#include "UI/UIUtil.h"

#include "UITopMenu_V10.h"
//#include "UIPhaseProfile.h"

#include "../CharaInfoContext.h"
#include "../GameCharaActionContext.h"
#include "../GameCharaAnimContext.h"
#include "../GameCharaWeaponContext.h"

#include "UI/UIShopFunction.h"
#include "UI/PurchaseMgr.h"

#include "UserInfoContext.h"
#include "BattleSlotContext.h"
#include "UI/UISMenu_Ability.h"
#include "UI/BuyUtil.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "UI/UISignal.h"

I3_CLASS_INSTANCE( UIFloatCharacterView_V10);//, UIFloatingBase);

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" { 
}

LuaFuncReg FloatCharacterView_V10_Glue[] =
{
	{ nullptr,				nullptr}
};

//------------------------------------------------------------------------------//

UIFloatCharacterView_V10::UIFloatCharacterView_V10()
{
	m_rVersionPosX = 0.0;

	if (g_pEnvSet->IsBaseOnUIVersion_1_0())
		m_rVersionPosX = 0.5;

	RegisterGameEventFunc(EVENT_CREATE_NICK_NAME,		&UIFloatCharacterView::ReceiveGameEvent_DoUpdateCharacter);
	RegisterGameEventFunc(EVENT_TEAM_CHANGE,			&UIFloatCharacterView::ReceiveGameEvent_Team_Change);
	RegisterGameEventFunc(EVENT_TEAM_BALANCE,			&UIFloatCharacterView::ReceiveGameEvent_Team_Balance);
	RegisterGameEventFunc(EVENT_CHANGE_ROOM_OPTION_N,	&UIFloatCharacterView::ReceiveGameEvent_DoUpdateCharacter);
	RegisterGameEventFunc(EVENT_DELETE_CHARA,			&UIFloatCharacterView::ReceiveGameEvent_DeleteCharacter);
	RegisterGameEventFunc(EVENT_SET_CHARA,				&UIFloatCharacterView::ReceiveGameEvent_SetCharacter);
	RegisterGameEventFunc(EVENT_REVERT_CHARA,			&UIFloatCharacterView::ReceiveGameEvent_RevertCharacter);
	RegisterGameEventFunc(EVENT_REVERT_PARTS,			&UIFloatCharacterView::ReceiveGameEvent_RevertParts);
	RegisterGameEventFunc(EVENT_REPAIR,					&UIFloatCharacterView::ReceiveGameEvent_Repair);
}

UIFloatCharacterView_V10::~UIFloatCharacterView_V10()
{
	I3_SAFE_RELEASE( m_pPlaneShape);
}

void UIFloatCharacterView_V10::UpdateCharaPosition(void)
{
	VEC3D vPos;
	i3Camera * pCam = g_pFramework->getSgContext()->getCurrentCamera();
	if (pCam != nullptr && m_bCenterFrame)
	{
		// ī�޶��� ������ǥ
		VEC2D vPoint;

		m_pPlaneShape->modifyDynamicState(I3_PHYSIX_STATE_KINEMATIC, I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RESPONSE);

		MATRIX invMx;
		REAL32 dx, dy;
		VEC3D pos1, pos2;
		INT32 w, h;

		w = g_pFramework->GetViewer()->GetViewWidth();
		h = g_pFramework->GetViewer()->GetViewHeight();

		REAL32 scale = h / 768.f;

		i3Vector::Set(&vPoint, getX(m_pFrameWnd->getAccumulatedPos()), getY(m_pFrameWnd->getAccumulatedPos()));

		i3Vector::AddX(&vPoint, (i3UI::getManager()->getDisplayOffsetX() + (m_pFrameWnd->getWidth() * 0.5f)));
		i3Vector::AddY(&vPoint, (i3UI::getManager()->getDisplayOffsetY() + m_pFrameWnd->getHeight() - 40.f));

		vPoint.y *= scale;

		{
			MATRIX tmx;
			i3Matrix::Mul(&tmx, pCam->GetViewMatrixAttr()->GetMatrix(), pCam->GetProjectionMatrix());
			i3Matrix::Inverse(&invMx, nullptr, &tmx);
		}

		dx = (vPoint.x / w * 2.0f) - 1.0f;
		dy = (vPoint.y / h * 2.0f) - 1.0f;

		i3Vector::Set(&pos1, dx, -dy, 0.0f);
		i3Vector::Set(&pos2, dx, -dy, 1.0f);

		i3Vector::TransformCoord(&pos1, &pos1, &invMx);
		i3Vector::TransformCoord(&pos2, &pos2, &invMx);

		// �켱 Raycasting�� �õ��Ѵ�.
		{
			I3_PHYSIX_HIT_RESULT * pResult;
			i3CollideeLine line;

			line.SetStart(&pos1);
			line.SetEnd(&pos2);
			pResult = g_pFramework->raycastClosest(&line, CGRP_MASKGROUP_ATTACK);

			if (pResult != nullptr)
			{
				i3Vector::Copy(&vPos, &pResult->m_Intersect);
			}
			else
			{
				i3Vector::Set(&vPos, m_rVersionPosX, 0.f, 0.0f);
			}
		}
	}
	else
	{
		i3Vector::Set(&vPos, m_rVersionPosX, 0.f, 0.0f);
	}

	CharaViewMgr::getView()->SetCharaPosition(&vPos);

	m_bUpdatedCharaPos = false;
}

void UIFloatCharacterView_V10::RevertChara(REVERT_TYPE type)
{
	RevertCharaData(type);
	DoUpdateCharacter(false, WEAPON_SLOT_PRIMARY);
}

void UIFloatCharacterView_V10::RevertCharaData(REVERT_TYPE type)
{
	if (m_iCurrentCharaIdx != -1)
	{
		//CCharaInfoContext::i()->SetTeamSlotBySlotIdx(m_iCurrentCharaIdx);
		// ���� ������ �ƹ�Ÿ�� ����� �������ش�.
		// ���� ������ ������ ����
		UISideMenu * pSide = UISideMenu::instance();
		if (pSide != nullptr)
		{
			SMenuAbility* ability = static_cast<SMenuAbility*>(pSide->getSubSidemenu(SIDEMENU_BTN_ABILITY));
			ability->SetMainInfoUpdateType(AMUT_REVERT);

			if (type & REVERT_TYPE_WEAPON)
			{
				const CHARA_WEAPON_INFO * pWeaponInfo = CCharaInfoContext::i()->GetCharaWeaponInfo(m_iCurrentCharaIdx);
				if (pWeaponInfo != nullptr)
				{
					// vv3 - check
					for (INT32 i = 0; i < CHAR_EQUIPMENT_WEAPON_COUNT; ++i)
					{
						UserInfoContext::i()->SetMyUICharaInfo()->setWeaponItemID((WEAPON_SLOT_TYPE)i, pWeaponInfo->m_Info[i].m_TItemID);
					}
					ability->applyWeapon(g_pWeaponInfoDataBase->getWeaponInfo(pWeaponInfo->m_Info[CHAR_EQUIPMENT_WEAPON_PRIMARY].m_TItemID, true));
				}
			}

			if (type & REVERT_TYPE_EQUIP)
			{
				const CHARA_PARTS_INFO * pPartsInfo = CCharaInfoContext::i()->GetCharaPartsInfo(m_iCurrentCharaIdx);
				if (pPartsInfo != nullptr)
				{
					T_ItemID chara_id = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
					CCharaInfoContext::i()->SetTeamSlotByCharaID(chara_id);
					UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(pPartsInfo);
					UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(pPartsInfo, CCharaInfoContext::i()->GetTeamSlot());
					ability->applyParts(const_cast<CHARA_PARTS_INFO*>(pPartsInfo));
				}

				// �ٸ� ������ �̵��� Preview ������ ��� ���� �ʱ�ȭ �Ѵ�.
				// ������ Part ���� �� �̵��ÿ��� ���⸸ �����ϹǷ� Equip�� �ǵ����� preview�� ������.
				buy::ClearPreview();
			}
		}
	}
}

void UIFloatCharacterView_V10::SelectChara( T_ItemDBIdx chara_db_idx)
{
	UINT8 slot_idx = CCharaInfoContext::i()->find_slot_idx_by_DBIdx( chara_db_idx );

	if (slot_idx != 0xFF)
	{
		INT32 list_idx = CCharaInfoContext::i()->find_slot_idx(slot_idx);
		UpdateCharaSlot(static_cast<UINT8>(list_idx));
	}

	I3ASSERT(slot_idx != 0xFF);
}

void UIFloatCharacterView_V10::UpdateCharaSlot( INT32 charaSlot )
{
	// ������� ��� �� ������ ����
	if (GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM && GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE )
		CCharaInfoContext::i()->SetTeamSlotBySlotIdx( BattleSlotContext::i()->GetMySlotIdxEx() );

	if( charaSlot == -1 )
		charaSlot = CCharaInfoContext::i()->GetMainCharaIdx();

	if( m_iCurrentCharaIdx == charaSlot)
		return;

	m_iCurrentCharaIdx = charaSlot;
	CCharaInfoContext::i()->SetTeamSlotBySlotIdx(m_iCurrentCharaIdx);

	CCharaInfoContext::i()->SetMainCharaIdx( m_iCurrentCharaIdx);
	INT32 chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

	Avatar * pavtar = g_pCharaManager->GetLobbyAvatar();

	if( pavtar != nullptr )
	{
		if( pavtar->getCharaInfo()->getLoadSlotIndex() == -1)
		{
			{	// �κ��丮�� �ƹ�Ÿ ��ü�� ����
				CharaInfoMgr * mCharaMgr = CharaInfoMgr::instance();
				for(INT32 i = WEAPON_SLOT_PRIMARY; i <  WEAPON_CHARA_SLOT_MAX; i++)
				{
					ITEM_INFO item = mCharaMgr->find( chara_idx, item_def::SCTG_WEAPON, i);
					
					UserInfoContext::i()->SetMyUICharaInfo()->setWeaponItemID( (WEAPON_SLOT_TYPE)i, item.m_TItemID);
				}
			}
	
			{
				const ItemInfoList & list = CharaInfoMgr::instance()->get_item_info_list( chara_idx, item_def::SCTG_PARTS);
				size_t size = list.size();

				// vv3 - check �̰� ���߿� CCharInfoContext �����Ǹ� �ٽ� �۾� �ؾߵ�.

				CHARA_PARTS_INFO * pPartsInfo = const_cast<CHARA_PARTS_INFO*>(UserInfoContext::i()->SetMyUICharaInfo()->getCharaParts(CCharaInfoContext::i()->GetTeamSlot()));
				for(size_t i=0;i<size;++i)
				{
					pPartsInfo->SetItemInfo( (CHAR_EQUIPMENT_PARTS)(i+1), list[i]);
				}

				pPartsInfo->m_Info[CHAR_EQUIPMENT_PARTS_CHARA] =  CCharaInfoContext::i()->GetMainCharaPartsInfo()->m_Info[CHAR_EQUIPMENT_PARTS_CHARA] ;
			}

			DoUpdateCharacter();
		}
	}

	// CharaInfoMgr�� ���� ĳ���� ������ ����
	CharaInfoMgr::instance()->m_current_state.slot_idx = CCharaInfoContext::i()->GetMainCharaIdx();
	CharaInfoMgr::instance()->m_current_state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();
	CharaInfoMgr::instance()->build_current_equip_info();

//	UI::call_cb("update_cart_board");	//īƮ ���� ����
	UISignal::UpdateCartBoard();		//īƮ ���� ����
	UISideMenu::instance()->smenu_click_equip( false, 2 );	//���� �޴� ��� ���� ����

//	UI::call_cb( "update_slot9", (void*) true, (void*) true);	//���� ĳ���� �°� ���� ���� ���� ���� ����
	UISignal::UpdateSlot9(true, true);							//���� ĳ���� �°� ���� ���� ���� ���� ����

	// ������ preview ������ ������ �ʱ�ȭ
	buy::ClearPreview();
	UISignal::UpdatePreviewSlot();
//	UI::call_cb( "update_preview_slot");
}

void UIFloatCharacterView_V10::DoUpdateCharacter( bool bDummyChara, WEAPON_SLOT_TYPE mustUpdateWeapon)
{
	bool bBtnChangeChara = false;
	bool bBtnChangeTeam = false;
	bool bUICenter = false;			// Frame Pos (true = Center , false = Left)
	bool bNickVisible = true;

	UIPHASE ePhase = GetMainFrame()->GetCurrentPhaseType();
	UISUBPHASE eSubPhase = GetMainFrame()->GetCurrentSubPhaseType();

	switch (ePhase)
	{
	case UIPHASE_CREATE_CHARA:
		bBtnChangeChara = false;
		bBtnChangeTeam = true;
		bUICenter = true;
		bNickVisible = false;
		m_bSkillSlot = false;
		break;
	case UIPHASE_CLANLOBBY:
	case UIPHASE_LOBBY:
		bBtnChangeChara = true;
		bBtnChangeTeam = true;
		bUICenter = false;
		m_bSkillSlot = true;

		switch (eSubPhase)
		{
		case USP_INVENTORY:
			bBtnChangeChara = true;
			bBtnChangeTeam = true;
			bUICenter = false;
			break;
		case USP_SHOP:
			bBtnChangeChara = true;
			bBtnChangeTeam = true;
			bUICenter = false;
			break;
		}
		break;
	case UIPHASE_READY_ROOM:
		bBtnChangeChara = true;
		bBtnChangeTeam = false;
		bUICenter = false;
		m_bSkillSlot = true;

		switch (eSubPhase)
		{
		case USP_INVENTORY:
			bBtnChangeChara = true;
			bBtnChangeTeam = true;
			bUICenter = false;
			break;
		case USP_SHOP:
			bBtnChangeChara = true;
			bBtnChangeTeam = true;
			bUICenter = false;
			break;
		}
		break;

	default:
		break;
	}

	// Character Create���� NetCharaInfo�� �����Ѵ�.
	// ĳ���Ͱ� �Ѹ����� ä������ư off
	if (CCharaInfoContext::i()->GetHasCharacterCount() == 1)
		bBtnChangeChara = false;

	const CHARA_INFO * info = CCharaInfoContext::i()->GetCharaInfo(m_iCurrentCharaIdx);

	// �̰��� nullptr�̶�� �⺻ĳ���Ͱ� ����� �ȵǾ� �ִ�.
	if (info == nullptr)
	{
		m_bChangeChara = false;
		m_bChangeTeam = false;
		m_bCenterFrame = bUICenter;
		_SetUIState(false, false, m_bSkillSlot);
		_SetCharaClass();
		_UpdateDurability();
		return;
	}

	// �����̴�. ���� ��������!!
	TEAM_TYPE		teamType;
	if( BattleSlotContext::i()->GetMySlotIdxEx() > -1 &&
		(ePhase == UIPHASE_READY_ROOM && eSubPhase == USP_NONE))
	{
		teamType = (BattleSlotContext::i()->GetMySlotIdxEx()%2==0) ? TEAM_RED:TEAM_BLUE;
	}
	else
	{
		teamType = CCharaInfoContext::i()->GetTeamSlot();
	}

	{
		if (m_TeamType != teamType)
		{
			m_TeamType = teamType;

			m_bChangingTeam = true;
			m_ChangingTeamTimer = 0.f;
		}
	}

	{
		if (UserInfoContext::i()->SetMyUICharaInfo()->getCharaParts(m_TeamType)->IsValid() == false)
			UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(&info->m_CharaEquipInfo.m_CharPartsInfo, m_TeamType);

		_SetCharaClass();
	}

	m_bDummyChara = bDummyChara;

	//SetClassstate
	m_bChangeChara = bBtnChangeChara;
	m_bChangeTeam = bBtnChangeTeam;
	m_bCenterFrame = bUICenter;
	_SetUIState(bBtnChangeChara, bBtnChangeTeam, m_bSkillSlot);

	CharaViewMgr::i()->UpdateCharacter(BattleSlotContext::i()->GetMySlotIdxEx(), true, mustUpdateWeapon, m_TeamType, m_pFrameWnd);

	_UpdateDurability();

	m_bUpdatedCharaPos = true;

	g_pFramework->OnEvent( UIEVENT_UPDATE_ABILITY, this, 0, I3_EVT_CODE_ACTIVATE);
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVEN_REFRESH);
}

void UIFloatCharacterView_V10::ReceiveGameEvent_RevertParts(INT32 arg, const i3::user_data&)
{
	RevertChara(arg);
	Update_CharaView_Parts_Preview();
}


bool UIFloatCharacterView_V10::UIEvent_Change_Parts( i3ElementBase * pObj, UINT32 param, I3_EVT_CODE)
{
	RevertCharaData();
	bool bDummyChara = false;

	if( pObj != nullptr && pObj == g_pFramework )
	{
		// ���� ����	
		// vv3 - check ������ ������ �ϴµ� 
		for( int team = 0 ; team < TEAM_COUNT ; team++ )
		{
			CHARA_PARTS_INFO parts;
			const CHARA_PARTS_INFO * pMyParts = UserInfoContext::i()->GetMyUICharaInfo()->getCharaParts(team);
			pMyParts->CopyTo( &parts);

			UINT32 ui32ItemType = GET_ITEM_TYPE( param);			
			T_ItemID ItemID = param;

			if( param < DEFAULT_PARTS_ITEM_ID_MIN && GET_ITEM_NUMBER( param) == 0  )
				ItemID = 0;	// ������ �ѹ��� 0�� ���, ���� �����Դϴ�.

			switch( ui32ItemType )
			{
				// vv3 - check
			case ITEM_TYPE_DINO :
				{
					COMMON_PARTS_INFO commonParts;
					commonParts.SetPartsItemID( ItemID );
					UserInfoContext::i()->SetMyUICharaInfo()->setCommonParts( &commonParts);
				}
				break;
			case ITEM_TYPE_CHARA :		
				{
					CCharaInfoContext::i()->SetTeamSlotByCharaID(ItemID);
					parts.SetPartsItemID( CHAR_EQUIPMENT_PARTS_CHARA, ItemID);	
					bDummyChara = true;	
				}
				break;
			case ITEM_TYPE_BERET :		
				{
					parts.SetPartsItemID( EQUIP::GetCommonPartToBeret(), ItemID);	
					bDummyChara = true;	
				}
				break;

			default:
				if( ui32ItemType < ITEM_TYPE_HEADGEAR || ui32ItemType > ITEM_TYPE_SKIN ) 
				{
					I3ASSERT_0;
					return false;
				}
				UINT32 ui32Parts = ui32ItemType - ITEM_TYPE_CHARA;
				parts.SetPartsItemID( (CHAR_EQUIPMENT_PARTS)ui32Parts, ItemID);
				break;
			}

			//2014-06-17 ���� 9506 ENG_��뿵
			//ĳ���� �信�� ����, �Ϲݽ�, �۷��긦 ��ȯ�Ͽ� �����ϸ� ĳ���� ��ü�� ȸ������ �ʱ�ȭ �˴ϴ�.
			//�׿� ���缭 UI���� ȸ������ �ʱ�ȭ �Ͽ����ϴ�.
			if(	ui32ItemType == ITEM_TYPE_UPPER ||
				ui32ItemType == ITEM_TYPE_LOWER ||
				ui32ItemType == ITEM_TYPE_GLOVE  )
			{
				CharaViewMgr::i()->SetCharaRotateTheta(0.0f);
			}

			SMenuAbility* ability = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
			ability->applyParts(&parts);
			ability->SetMainInfoUpdateType(AMUT_PREVIEW_PARTS);

			UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts( &parts, team);
		}

		m_UpdatableItemID = param;
	}

	CharaInfoMgr * mCharaMgr = CharaInfoMgr::instance();
	mCharaMgr->adjust_equip_with_version_UIChara(m_UpdatableItemID);

	DoUpdateCharacter( true);
	return true;
}

void	UIFloatCharacterView_V10::SetChangeTeam(TEAM_TYPE teamType)
{
	if (m_TeamType != teamType)
	{
		m_bChangingTeam = true;
		m_ChangingTeamTimer = 0.f;

		INT32 charaSlot = CCharaInfoContext::i()->GetMainCharaIdx(teamType);
		UpdateCharaSlot(static_cast<UINT8>(charaSlot));
	}
}
#include "pch.h"
#include "UIPhaseProfile_V10.h"
#include "UITabProfileDesignation_V10.h"
#include "UI/UIMainFrame_V10.h"
#include "UIPhaseProfile_V10.h"
#include "UI/UIUtil.h"

#include "Designation/DesigUtil.h"
#include "Designation/UserDesigInfo.h"
#include "Designation/DesigAttr.h"
#include "Designation/DesigSet.h"

#include "UserInfoContext.h"

#include "UI/UIShopFunction.h"
#include "../StepProfileDef.h"
#include "MedalContext.h"
#include "UI/UICharaInfoMgr.h"


static UITabProfileDesignation_V10 * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	UIPhaseSubBase* pSubPhase = pFrame->GetCurrentSubPhase();
	UITabProfileDesignation_V10* pTab = static_cast<UIPhaseProfile_V10*>(pSubPhase)->GetDesignation();
	I3ASSERT( i3::same_of<UITabProfileDesignation_V10*>(pTab)); 

	return static_cast<UITabProfileDesignation_V10*>(pTab);
}

namespace
{
	INT32 GetEmptySlot()
	{
		const CUserDesigInfo*	myDesigInfo = DesigUtil::GetMyUserDesigInfo();

		UINT8 desigSetTypes[MAX_EQUIP_USER_TITLE];
		myDesigInfo->GetEquipDesig(desigSetTypes);

		for (INT32 i = 0 ; i < MAX_EQUIP_USER_TITLE ; ++i)
		{
			const DesigSet* pDesigSet = DesigUtil::FindDesigSetByType(desigSetTypes[i]);
			if (pDesigSet == nullptr)
				return i;
		}

		return -1;
	}

	// �� �κ��� V2 �������� ���� ������ �����ϰ� �Ϸᰡ �Ǿ� ���� �ʱ� ������ �ټ� �Ҿ����ϴ�..(2015.05.29.����)
	// �ش� �Լ��� ȣĪ������ ����սô�. (2015.07.29 ���õ)
	void CheckUserTitlesBeret()
	{
		//�����ϴ� ȣĪ�� ���� �����ϰ� �ִ� ������ �����ϱ� ����
		//�ʿ� ȣĪ�̶�� ���� �����ϰ� �ִ� ������ �����. (�������� �˾Ƽ� ����ٰ� �����ϰ� ���⼭��
		//��Ƽ���׸� �Ű� ������ �Ѵ�...)

		CharaInfoMgr::instance()->list_up();

		// ���� �������� ������ �����ɴϴ� (ĳ���ͺ� ����)
		const ITEM_INFO & beret_info = CharaInfoMgr::instance()->get_current_equip_item(UserEquipInfo::SUE_BERET);
		if(false == DesigUtil::CheckWearable_MyItemByDesig(beret_info.m_TItemID) )
		{
			ItemInfoConvert item_convert(beret_info);
		
			for (INT32 team = 0; team < TEAM_COUNT; ++team)
			{
				UINT8 chara_slot_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx(static_cast<TEAM_TYPE>(team));
				const CHARA_PARTS_INFO * parts_info = CCharaInfoContext::i()->GetCharaPartsInfoBySlotIdx(chara_slot_idx);
				T_ItemID chara_item_id = parts_info->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

				CHARA_PARTS_INFO parts;
				const CHARA_PARTS_INFO * pMyParts = UserInfoContext::i()->GetMyUICharaInfo()->getCharaParts(team);
				pMyParts->CopyTo(&parts);
				
				ITEM_INFO default_item_info = item_def::get_default_item(chara_item_id, EQUIP::ePART_BERET);
				parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_BERET, default_item_info);
				UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(&parts, team);
			}

			CharaInfoMgr::instance()->change_equip_item(item_convert, beret_info);
		}

		// ����� ������ ���� ������ ������, ������ ������ ���⵵ ����Ͽ�, �ϴ� ������ �صӴϴ�.
		// �ʿ� �� �ּ� ���� �� ����Ͻø� �˴ϴ�. 2015.07.29 ���õ
		//const ItemInfoList & weapon_list = CharaInfoMgr::instance()->get_current_equip_weapon();
		//for( INT32 idx = 0 ; idx < item_def::WT_MAX ; ++idx)
		//{
		//	const ITEM_INFO & weapon_info = weapon_list[item_def::WT_PRIMARY];

		//	if( DesigUtil::CheckWearable_MyItemByDesig(weapon_info.m_TItemID) )
		//	{
		//		ItemInfoConvert item_convert(weapon_info);
		//		CharaInfoMgr::instance()->change_equip_item(item_convert, weapon_info);
		//	}
		//}

		// ���� ������ ����� ��쿡��, ��� ���� ��Ŷ�� �����սô�.
		if( CharaInfoMgr::instance()->send_event_change_equip() == true )
		{
			// ����Ͻ� ������ ���� ��� �ڵ����� �����˴ϴ�.
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_TBL_PROFILE_VELEMO_RELEASE"));		
			// ���ӽ�Ʈ���� �߰��Ǿ�� �� �� ����...�ϴ� ��ġ..
			//	if( g_pFramework->EquipCharacterToMyInven(0, CHARACTER_CLASS_ITEM, 0, true))
			//			PlaySound( UISND_ITEM_EQUIP);
		}
	}
}

namespace
{
	static void CbDesignationGet(void* pThis, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			INT32 ShowingDesignation = GetThis()->GetCurShowingDesigType();
			GameEventSender::i()->SetEvent(EVENT_DESIGNATION_GET, &ShowingDesignation );
		}
	}

	static void CbDesignationRelese(void* pThis, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			UINT8 RemoveTitleSlotIndex = (UINT8)GetThis()->GetSelectedDesignationReleaseSlotIdx();

			// �����ϱ� ������ DesignationID ��0���� �־��ش�.
			//CheckUserTitlesBeret(RemoveTitleSlotIndex, 0);

			GameEventSender::i()->SetEvent( EVENT_DESIGNATION_RELEASE, &RemoveTitleSlotIndex);
		}
	}

	static void CbDesignationChange(void* pThis, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			// GetThis()->__OnDesignationChange();
			UIMainFrame* pFrame = g_pFramework->GetUIMainframe();
			pFrame->OpenPopup(UPW_DESIG_SELECT);
		}
	}


}

struct SetMyInfoDesignation_Image_Enable_V10
{
	void operator()(i3UIScene* pScene, UINT32 idx, bool Sig) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Enable");
		i3Lua::PushInteger(L, idx);
		i3Lua::PushBoolean(L, Sig);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Enable_V10;

struct SetMyInfoDesignation_Image_Lock_V10
{
	void operator()(i3UIScene* pScene, UINT32 idx, bool Sig) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Lock");
		i3Lua::PushInteger(L, idx);
		i3Lua::PushBoolean(L, Sig);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Lock_V10;

struct SetMyInfoDesignation_Image_Index_V10
{
	void operator()(i3UIScene* pScene, UINT32 slot_idx, UINT32 desigSetIndex) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Index");
		i3Lua::PushInteger(L, slot_idx);
		i3Lua::PushInteger(L, desigSetIndex);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Index_V10;

struct SetMyInfoDesignation_Text_V10
{
	void operator()(i3UIScene* pScene, UINT32 slot_idx, i3::rc_wstring wstrTitle) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Text");
		i3Lua::PushInteger(L, slot_idx);
		i3Lua::PushStringUTF16To8(L, wstrTitle.c_str());
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Text_V10;


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UITabProfileDesignation_V10_Click(LuaState * L)
	{
		GetThis()->__UpdateInfoFromLuaGlue();
		return 0;
	}
	//int UITabProfileDesignation_V10_MyDesigClick(LuaState * L)
	//{
	//	INT32 idx = i3Lua::GetIntegerArg(L, 1);
	//	GetThis()->__MyDesigClickFromLuaGlue(idx);
	//	return 0;
	//}
	//int UITabProfileDesignation_V11_MyDesigDoubleClick(LuaState * L)
	//{
	//	INT32 idx = i3Lua::GetIntegerArg(L, 1);
	//	GetThis()->__MyDesigDoubleClickFromLuaGlue(idx);
	//	return 0;
	//}
	//int UITabProfileDesignation_V11_Release(LuaState * L)
	//{
	//	INT32 idx = i3Lua::GetIntegerArg(L, 1);
	//	GetThis()->__MyEquipReleaseFromLuaGlue(idx);
	//	return 0;
	//}
}

LuaFuncReg UITabProfileDesignation_V10_Glue[] =
{
	//{ "ClickMyDesigSlot",	UITabProfileDesignation_V11_MyDesigClick		},
	//{ "D_ClickMyDesigSlot",	UITabProfileDesignation_V11_MyDesigDoubleClick	},
	{ "UseDesignation",		UITabProfileDesignation_V10_Click				},
	//{ "ClickXButton",		UITabProfileDesignation_V11_Release				},
	{ nullptr,		nullptr}
};


I3_CLASS_INSTANCE( UITabProfileDesignation_V10);

UITabProfileDesignation_V10::UITabProfileDesignation_V10() : m_vecGetDesignation(MAX_DEFAULT_USER_TITLE_COUNT)
{
	::memset(m_pButtonImage, 0, sizeof(m_pButtonImage));
	::memset(m_pShapeImage, 0, sizeof(m_pShapeImage));
	m_nCurShowingDesignationType = 0;
	m_nCurDesignationSlotCount = 1;
}

UITabProfileDesignation_V10::~UITabProfileDesignation_V10()
{
}

void UITabProfileDesignation_V10::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	switch( event)
	{
	case EVENT_DESIGNATION_GET :
		{
			// ȹ�� ȣĪ
			INT32 result = arg;
			switch( result )
			{
			case EVENT_ERROR_SUCCESS:
				{
					// ȣĪ ȹ��� ���� �� �˸�â
					i3::rc_wstring wstrMessage;
					DesigUtil::Update_Recv_EVENT_DESIGNATION_GET( wstrMessage );
					GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMessage);
				}
				break;
			case EVENT_ERROR_USER_TITLE_REQUIREMENT_FAIL:
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_TBL_NETWORK_NOT_MATCH_BUY_DESIGNATION"));/*ȣĪ ��ȯ ������ �������� �ʽ��ϴ�.*/
				break;
			case EVENT_ERROR_USER_TITLE_ALREADY_GAIN_FAIL :
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_TBL_NETWORK_ALREADY_GET_DESIGNATION"));/*�̹� ȹ���� ȣĪ�Դϴ�.*/
				break;
			}
			_Update_All();
		}
		break;
	case EVENT_DESIGNATION_CHANGE:
	case EVENT_DESIGNATION_RELEASE :
		{
			CheckUserTitlesBeret();
			_Update_All();
		}
		break;
	case EVENT_GET_USERINFO :
		_Update_All();
		break;
	}
}



/*virtual*/ bool UITabProfileDesignation_V10::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/ )
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pParam = (I3UI_CONTROL_NOTIFY*) param2;
		//	/*
		//	----------------------------------------�ʵ�---------------------------------------------
		//	�̺�Ʈ�� ȣĪ ��ư�� �޽��ϴ�. ������ UI �̺�Ʈ�� ��Ʒ�...
		//	�ӽ÷� ������Ʈ�� ȣĪâ�� �°� ����(���� üũ�ڽ��� State�� �����Ͻø� �ȵǿ�)
		//	�������� UIControl�� ������. ȣĪ��ư�� Press����(���� �����ϰ� �ִ� ȣĪ) �����Դϴ�.
		//	I3UI_CHECKBUTTON_SHAPE_DISABLED_CHECKED			-> Press ���� + OnMouse(�����Ķ�)
		//	I3UI_CHECKBUTTON_SHAPE_DISABLED_UNCHECKED;		-> Press ����(���Ķ�) or �׵θ�
		//	I3UI_CHECKBUTTON_SHAPE_ENABLED_CHECKED_ONMOUSE	-> ����Ʈ ����(�Ķ�) + OnMouse(�׵θ�)
		//	I3UI_CHECKBUTTON_SHAPE_ENABLED_CHECKED			-> ����Ʈ ����(�Ķ�)
		//	I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED_ONMOUSE	-> ��� ���� + OnMouse -> Ƣ����� ȿ��
		//	I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED			-> ��� ���� -> ȿ�� ��
		//	-----------------------------------------------------------------------------------------
		//	*/

		// *************** Shape Idx Info
		// 0 : ȹ������ ���� ȣĪ Normal 
		// 1 : ȹ������ ���� ȣĪ MouseOver
		// 2 : ȹ������ ���� ȣĪ Selected
		// 3 : ȹ���� ȣĪ Normal
		// 4 : ȹ���� ȣĪ MouseOver
		// 5 : ȹ���� ȣĪ Selected
		// *************** Shape Idx Info

		bool bCheckBox = false;


		if(_GetDesigIdx(pParam->m_pCtrl) >= 0)
		{
			bCheckBox = true;
		}

		if(	bCheckBox == false )
		{
			if(pParam->m_nEvent == I3UI_EVT_CLICKED)
				I3TRACE(pParam->m_pCtrl->GetName());
			return true;
		}

		// MouseOver 
		if( pParam->m_nEvent == I3UI_EVT_ENTERMOUSE)
		{
			INT32 idx = _GetDesigIdx( pParam->m_pCtrl);
			// �ε��� ���� üũ�� �Ʒ� �Լ� ���ο��� ����
			_ChangeButtonImgState(I3UI_EVT_ENTERMOUSE, idx);
		}

		if( pParam->m_nEvent == I3UI_EVT_LEAVEMOUSE)
		{
			INT32 idx = _GetDesigIdx( pParam->m_pCtrl);
			_ChangeButtonImgState(I3UI_EVT_LEAVEMOUSE, idx);
		}

		if( pParam->m_nEvent == I3UI_EVT_DBL_CLICKED)
		{
			I3ASSERT( pParam->m_pCtrl != nullptr);
			__SetCurrentButton( _GetDesigIdx( pParam->m_pCtrl) );

			if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
			{
				_ChangeButtonImgState(I3UI_EVT_DBL_CLICKED, m_SelectedDesignationIdx);
				m_pButtonImage[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
				m_pButtonImage[m_OldSelectedDesignationIdx]->setModifyState(true);
			}

			_Update_DetailInfoBox();
			_Update_GetInfoBox();
			__OnDoubleClickedDesignationButton();
			_Update_DesignationTree();	
		}
		else if( pParam->m_nEvent == I3UI_EVT_CLICKED)
		{
			I3ASSERT( pParam->m_pCtrl != nullptr);
			__SetCurrentButton( _GetDesigIdx( pParam->m_pCtrl) );

			if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
			{
				_ChangeButtonImgState(I3UI_EVT_CLICKED, m_SelectedDesignationIdx);
				m_pButtonImage[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
				m_pButtonImage[m_OldSelectedDesignationIdx]->setModifyState(true);
			}
			_Update_DetailInfoBox();
			_Update_GetInfoBox();
		}

	}


	return UITabProfileBase_V10::OnEvent( event, pObj, param2, code);
}

/*virtual*/ void UITabProfileDesignation_V10::OnCreate( i3GameNode * pParent)
{
	UITabProfileBase_V10::OnCreate( pParent);
	
	AddScene( "Scene/Main/PointBlankRe_MyInfomation_Designation.i3UIs", UITabProfileDesignation_V10_Glue);
}



bool UITabProfileDesignation_V10::OnEntranceStart( void * pArg1, void * pArg2 )
{
	// ���⼭ �̸� �޾Ƶξ��, ȣĪ ������ ���� ȹ���ϰ� �Ǿ��� ������ ���� �˾��� ����� �� �ֽ��ϴ�
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();
	INT32 limitTitleCount = myDesigInfo->GetLimitCount();

	m_nCurDesignationSlotCount = limitTitleCount;

	if( UITabProfileBase_V10::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	return true;
}


bool UITabProfileDesignation_V10::OnClick( i3UIControl * pControl)
{
	if( UITabProfileBase_V10::OnClick( pControl) == false)
		return false;

	//if( i3::same_of<i3UICheckBox*>(pControl))
	//	PlaySound( UISND_CLICK);

	return true;
}

bool UITabProfileDesignation_V10::OnEnterMouse( i3UIControl * pControl)
{
	//if( UITabProfileBase_V10::OnEnterMouse( pControl) == false)
	//	return false;

	return true;
}

/*virtual*/void UITabProfileDesignation_V10::_InitializeAtLoad( i3UIScene * pScene)
{
	I3ASSERT( pScene != nullptr);

	m_pScene = pScene;

	m_pDesigFmw = (i3UIFrameWnd *)pScene->FindChildByName("Fmw_Designation");
	I3ASSERT( m_pDesigFmw !=  nullptr );

	// ���� ���� ���� UIControl ���
	m_pHavedMedal[0] = (i3UIStaticText*)pScene->FindChildByName("TextMiniHave");	// ����
	m_pHavedMedal[1] = (i3UIStaticText*)pScene->FindChildByName("TextInsHave");		// ����
	m_pHavedMedal[2] = (i3UIStaticText*)pScene->FindChildByName("TextMedalCurrent");	// ����
	m_pHavedMedal[3] = (i3UIStaticText*)pScene->FindChildByName("TextMasterHave");	// ����������

	m_pNeedMedal[0] = (i3UIStaticText*)pScene->FindChildByName("TextMiniNeed");	// ����
	m_pNeedMedal[1] = (i3UIStaticText*)pScene->FindChildByName("TextInsNeed");	// ����
	m_pNeedMedal[2] = (i3UIStaticText*)pScene->FindChildByName("TextMedalNeeds");	// ���� 
	m_pNeedMedal[3] = (i3UIStaticText*)pScene->FindChildByName("TextMasterNeed");	// ���������� 

	m_pNeedTitle[0] = (i3UIStaticText*)pScene->FindChildByName("TextNeedDes_context00");
	m_pNeedTitle[1] = (i3UIStaticText*)pScene->FindChildByName("TextNeedDes_context01");

	m_pNeedRank = (i3UIStaticText*)pScene->FindChildByName("TextNeedDes_context02");

	// ȣĪ Ʈ�� UIControl ���
	i3UIFrameWnd* pFmwIcon =	(i3UIFrameWnd *)pScene->FindChildByName("FmwDesView");
	i3::rc_wstring wstrCtrlName;
	if(pFmwIcon != nullptr)
	{
		INT32 fmwViewCnt =  pFmwIcon->GetChildCount();
		i3UIControl * pChild = (i3UIControl *)pFmwIcon->getFirstChild();

		for(INT32 i=0; i<fmwViewCnt ; ++i)
		{
			INT32 fmwIconCnt =  pChild->GetChildCount();
			i3UIControl * pChild2 = (i3UIControl *)pChild->getFirstChild();

			for(INT32 j=0; j<fmwIconCnt ; ++j)
			{

				if(i3::kind_of<i3UIButtonImageSet*>(pChild2))
				{
					i3::sprintf(wstrCtrlName, L"Imgbtn%02d", i);
					if(i3::generic_is_iequal( pChild2->GetNameString(),wstrCtrlName ))
					{
						m_pButtonImage[i] = (i3UIButtonImageSet*)pChild2;
					}

					i3::sprintf(wstrCtrlName, L"ImgShape%02d", i);
					if(i3::generic_is_iequal( pChild2->GetNameString(),wstrCtrlName ))
					{
						m_pShapeImage[i] = (i3UIButtonImageSet*)pChild2;
					}
				}
				else if (i3::kind_of<i3UIImageBox*>(pChild2))
				{
					i3::sprintf(wstrCtrlName, L"ImgEquipFlag%02d", i);
					if(i3::generic_is_iequal( pChild2->GetNameString(), wstrCtrlName))
						m_pGainDesigFlag[i] = (i3UIImageBox*)pChild2;

				}

				pChild2 = (i3UIControl *)pChild2->getNext();
			}

			pChild = (i3UIControl *)pChild->getNext();
		}

	}

	// �� ������ �ʱ�ȭ
	_InitTabInfo();
	// ��ü ���� ����
	_Update_All();

	GetSlide()->AddFrameWnds(GetScene(0));

	const CUserDesigInfo* userDesigInfo = DesigUtil::GetMyUserDesigInfo();
	m_nCurDesignationSlotCount = userDesigInfo->GetLimitCount();
}

void	UITabProfileDesignation_V10::_InitTabInfo()
{
	// ȣĪ Ʈ�� Shape ����
	for(INT32 i = 0 ; i<MAX_DEFAULT_USER_TITLE_COUNT; ++i)
	{
		if(m_pShapeImage[i] != nullptr)
		{
			m_pShapeImage[i]->SetShapeIdx(i);
			m_pShapeImage[i]->SetViewOnly(true);
			m_pGainDesigFlag[i]->EnableCtrl(true);
		}
	}

	// ���� �˾� ���� ����
	LuaState* L = _CallLuaFunction("setMyInfo");

	if (L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, UserInfoContext::i()->GetMyNickName().c_str());
		i3Lua::PushInteger(L, UserInfoContext::i()->GetMyRank());
		_EndLuaFunction(L, 2);
	}

	// ���� ���� �ʱ�ȭ
	// ���� ����  /  �ʿ� ���� ����
	i3::stack_wstring wstrCount;

	// ����
	UINT32 ribbonCount = MedalContext::i()->getMiniature();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), ribbonCount);
	m_pHavedMedal[0]->SetText(wstrCount.c_str());
	// ����
	UINT32 InsigniaCount = MedalContext::i()->getInsignia();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), InsigniaCount);
	m_pHavedMedal[1]->SetText(wstrCount.c_str());
	// ����
	UINT32 orderCount = MedalContext::i()->getOrder();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), orderCount);
	m_pHavedMedal[2]->SetText(wstrCount.c_str());
	// ������ ����
	UINT32 masterCount = MedalContext::i()->getMaster();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), masterCount);
	m_pHavedMedal[3]->SetText(wstrCount.c_str());

	for(INT32 i = 0; i < BADGE_TYPE_COUNT; i++)
	{
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_NEEDED_BADGECOUNT_TEXT"), 0);
		m_pNeedMedal[i]->SetText(wstrCount.c_str());
	}
}

void UITabProfileDesignation_V10::_ChangeButtonImgState(I3UI_EVT _evt, INT32 idx)
{
	if( idx < 0 || idx >= MAX_DEFAULT_USER_TITLE_COUNT) 
	{
		return;
	}
	
	// ȣĪ�� ȹ������ ���� ����
	if(m_pButtonImage[idx]->GetShapeIdx() < 3)
	{
		switch(_evt)
		{
		case I3UI_EVT_ENTERMOUSE:
			if(m_pButtonImage[idx]->GetShapeIdx() != 2)
				m_pButtonImage[idx]->SetShapeIdx(1);
			break;
		case I3UI_EVT_LEAVEMOUSE:
			if(m_pButtonImage[idx]->GetShapeIdx() != 2)
				m_pButtonImage[idx]->SetShapeIdx(0);
			break;
		case I3UI_EVT_CLICKED:
			m_pButtonImage[idx]->SetShapeIdx(2);			
			break;
		}
	}
	else
	{
		switch(_evt)
		{
		case I3UI_EVT_ENTERMOUSE:
			if(m_pButtonImage[idx]->GetShapeIdx() != 5)
				m_pButtonImage[idx]->SetShapeIdx(4);
			break;
		case I3UI_EVT_LEAVEMOUSE:
			if(m_pButtonImage[idx]->GetShapeIdx() != 5)
				m_pButtonImage[idx]->SetShapeIdx(3);
			break;
		case I3UI_EVT_CLICKED:
			m_pButtonImage[idx]->SetShapeIdx(5);
			break;
		}
	}

	// �ε��� m_OldSelectedDesignationIdx�� �ٱ����� üũ
	if(_evt == I3UI_EVT_CLICKED)
	{
		if(m_pButtonImage[m_OldSelectedDesignationIdx]->GetShapeIdx() < 3 )
			m_pButtonImage[m_OldSelectedDesignationIdx]->SetShapeIdx(0); 
		else
			m_pButtonImage[m_OldSelectedDesignationIdx]->SetShapeIdx(3); 
	}

}

void UITabProfileDesignation_V10::OnLoadAllScenes()
{
	//GetSlide()->AddFrameWnds(GetScene(0));
}

void UITabProfileDesignation_V10::OnUnloadAllScenes()
{
}

void UITabProfileDesignation_V10::_ClearAtUnload( i3UIScene * pScene)
{
	//UITabProfileBase_V10::_ClearAtUnload( pScene);
}

void UITabProfileDesignation_V10::__OnDoubleClickedDesignationButton()
{
	if( m_vecGetDesignation[m_SelectedDesignationIdx])
	{
		if( _CheckDesignationEquip()) // �ش� ȣĪ�� ���� ���� Ȯ��
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_DESIGNATION_RELEASE_ANSWER_MESSAGE"), nullptr, MAKE_CALLBACK(CbDesignationRelese), this);	//������ ȣĪ�� �����Ͻðڽ��ϱ�?
		else
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_DESIGNATION_EQUIP_ANSWER_MESSAGE"), nullptr, MAKE_CALLBACK(CbDesignationChange), this);	//������ ȣĪ�� �����Ͻðڽ��ϱ�?
	}
	else
	{	
		switch( m_DesignationMode)
		{
		case DESIGNATION_MODE_GET:
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_DESIGNATION_GET_ANSWER_MESSAGE"), nullptr, MAKE_CALLBACK(CbDesignationGet), this);/*������ ȣĪ�� ȹ���Ͻðڽ��ϱ�?*/
			break;
		case DESIGNATION_MODE_CHANGE:
			//	m_pDesignationChangeBG->SetEnable( true);
			break;
		case DESIGNATION_MODE_RELEASE:
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_STRING("STR_TBL_DESIGNATION_RELEASE_ANSWER_MESSAGE"), nullptr, MAKE_CALLBACK(CbDesignationRelese), this);/*������ ȣĪ�� �����Ͻðڽ��ϱ�?*/
			break;
		default:
			break;
		}
	}
}

void UITabProfileDesignation_V10::__OnDesignationChange(INT32 targetSlotIdx)
{


	m_SelectedDesignationChangeSlotIdx = targetSlotIdx;
	CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

	if( m_SelectedDesignationChangeSlotIdx >= 0 && m_SelectedDesignationChangeSlotIdx <myDesigInfo->GetLimitCount() )
	{

		UINT8 myEquipDesigTypes[MAX_EQUIP_USER_TITLE];
		myDesigInfo->GetEquipDesig(myEquipDesigTypes);

		bool DesigSetAlreadyUsed = false;

		const DesigSet* pDesigSetSelected = DesigUtil::FindDesigSetByUI(m_SelectedDesignationIdx);

		if ( pDesigSetSelected != nullptr )
		{

			for (INT32 i = 0 ; i < MAX_EQUIP_USER_TITLE ; ++i)
			{
				if ( i == m_SelectedDesignationChangeSlotIdx)
					continue;

				if (myEquipDesigTypes[i] == pDesigSetSelected->desigSetType)
				{
					DesigSetAlreadyUsed = true;
					break;
				}
			}
		}

		if( DesigSetAlreadyUsed )
		{
			//�ٸ� ���Կ� ������ �迭�� ȣĪ�� ����ϰ� �־� ��뿡 �����Ͽ����ϴ�.
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_TBL_DESIGNATION_FAIL_ANOTHER_USED"));
		}
		else
		{
			if ( pDesigSetSelected != nullptr )
			{
				UINT8 ui8DesigSlot = (UINT8)m_SelectedDesignationChangeSlotIdx;
				UINT8 DesignationType = (UINT8)pDesigSetSelected->desigSetType;
			//	CheckUserTitlesBeret(m_SelectedDesignationChangeSlotIdx, DesignationType);

				GameEventSender::i()->SetEvent( EVENT_DESIGNATION_CHANGE, &ui8DesigSlot, &DesignationType);
			}
		}
	}
	else
	{
		//����ϽǼ� �ִ� ������ �����ϴ�.\nȣĪ ������ �ٽ� ����� �ֽʽÿ�.
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_DESIGNATION_FAIL_ANOTHER_USED"));	
	}

	m_DesignationMode = DESIGNATION_MODE_GET;
	m_SelectedDesignationChangeSlotIdx = -1;
}

bool UITabProfileDesignation_V10::_CheckDesignationEquip(void)
{
	const DesigSet* desigSet = DesigUtil::FindDesigSetByUI(m_SelectedDesignationIdx);

	if ( desigSet == nullptr )
		return false;

	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

	UINT8 desigSetType = (UINT8)desigSet->desigSetType;
	INT32 slotIndex = myDesigInfo->FindDesigEquipSlot(desigSetType);

	if( slotIndex != -1)
	{
		m_SelectedDesignationReleaseSlotIdx = slotIndex;
		return true;
	}

	return false;
}



INT32	UITabProfileDesignation_V10::_GetDesigIdx(i3UIControl* pCtrl)
{
	if(	i3::same_of<i3UIButton*>(pCtrl))
	{
		i3::rc_string wstrName = pCtrl->GetNameString();
		wstrName.erase( 0, 10 ); 

		return ::atoi(wstrName.c_str());
	}
	return -1;
}



void UITabProfileDesignation_V10::__SetCurrentButton(INT32 idx)
{
	if(idx != m_SelectedDesignationIdx)
	{
		m_OldSelectedDesignationIdx = m_SelectedDesignationIdx;
		m_SelectedDesignationIdx = idx;
	}
}

//////////////////////////////////////////////////////////////////////////
//						Update Infomation								//
//////////////////////////////////////////////////////////////////////////

void	UITabProfileDesignation_V10::_Update_All()
{
	// ȣĪ ���� Ȯ��
	{
		const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();
		INT32 limitTitleCount = myDesigInfo->GetLimitCount();

		// �߰������� ȹ���Ͽ��ٸ�~!!  --> ???
		if ( limitTitleCount > m_nCurDesignationSlotCount )
		{
			m_nCurDesignationSlotCount = limitTitleCount;

			if(limitTitleCount != 0)
			{
				i3::stack_wstring wstrMsg;
				/*���ϵ帳�ϴ�!!\n \nȣĪ ������ ȹ���Ͽ� ȣĪ�� ���ÿ�\n����� �� �ִ� ������ �����Ͽ����ϴ�.\n(�ִ� %d���� ȣĪ ��� ����)*/
				i3::sprintf(wstrMsg, GAME_RCSTRING("STR_TBL_DESIGNATION_GET_SLOT_MESSAGE"), m_nCurDesignationSlotCount);
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrMsg);
			}
		}
	}


	DesigUtil::AcquireGainedDesigSetsForUI( m_vecGetDesignation, MAX_DEFAULT_USER_TITLE_COUNT);

	// �� ���� ȣĪ ����
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

	INT32 iLimitTitleCount = myDesigInfo->GetLimitCount();

	I3ASSERT( iLimitTitleCount <= MAX_EQUIP_USER_TITLE);

	for( INT32 i = 0; i < iLimitTitleCount; ++i)
	{
		fnSetMyInfoDesignation_Image_Enable_V10(m_pScene, i,false);
		fnSetMyInfoDesignation_Image_Lock_V10(m_pScene, i,false);//Enable true, false
	}

	for( INT32 i = iLimitTitleCount; i < MAX_EQUIP_USER_TITLE; ++i)
	{
		fnSetMyInfoDesignation_Image_Enable_V10(m_pScene, i,false);
		fnSetMyInfoDesignation_Image_Lock_V10(m_pScene, i,true);
	}

	// ȣĪ�̹��� �� ȣĪ�̸�.
	for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; ++i)
	{
		// ���� �ʱ�ȭ
		fnSetMyInfoDesignation_Text_V10(m_pScene, i, L"");

		UINT8 myEquipDesigSet = myDesigInfo->GetEquipDesig(i);

		if( myEquipDesigSet > MAX_DEFAULT_USER_TITLE_COUNT)
			continue;

		const DesigSet* desigSet = DesigUtil::FindDesigSetByType(myEquipDesigSet);

		if (desigSet != nullptr)
		{
			fnSetMyInfoDesignation_Image_Enable_V10(m_pScene, i, true);
			fnSetMyInfoDesignation_Image_Index_V10(m_pScene, i, desigSet->desigSetUIIndex );
			fnSetMyInfoDesignation_Text_V10(m_pScene, i, desigSet->wstrName);
		}
	}

	// �� ���� ����
	_Update_GetInfoBox();

	// ȣĪ Ʈ�� ������Ʈ
	_Update_DesignationTree();
}

void	UITabProfileDesignation_V10::_Update_DetailInfoBox()
{
	if(m_SelectedDesignationIdx < 0)
	{
		m_nCurShowingDesignationType = 0;
		return;
	}
	// Detail Info
	const DesigSet* desigSet = DesigUtil::FindDesigSetByUI(m_SelectedDesignationIdx);
	if (desigSet == nullptr) return;

	m_nCurShowingDesignationType = desigSet->desigSetType;

	i3::stack_wstring wstrTemp1;
	i3::stack_wstring wstrTemp2;

	wstrTemp1.clear();		wstrTemp2.clear();

	// Plus Effect
	for(INT32 i = 0; i < 2; ++i)
	{
		INT32 desigIndex = desigSet->plusEffect[i];

		const DesigAttr* attr = DesigUtil::FindDesigAttr(desigIndex);

		if( attr ) 
		{
			if (i == 0)
				i3::sprintf(wstrTemp1, L"{col:80,150,215,255}%s{/col}", attr->GetText() );
			else
				i3::sprintf(wstrTemp2, L"%s\n{col:80,150,215,255}%s{/col}", wstrTemp1, attr->GetText() );
		}
		else
		{
			if( i == 0)	
				i3::sprintf( wstrTemp1, GAME_RCSTRING("STR_TBL_DESIGNATION_NONE_WITH_COLOR1"));/*{col:80,150,215,255}����{/col}*/
		}
	}

	if ( !wstrTemp2.empty() )
		wstrTemp1 = wstrTemp2;

	{
		LuaState * L = UILua::CallLuaFunction(m_pScene, "SetPlusEffectString");
		i3Lua::PushStringUTF16To8(L, wstrTemp1);
		_EndLuaFunction(L, 1);
	}

	// Minus Effect
	wstrTemp1.clear();		wstrTemp2.clear();

	for(INT32 i=0 ; i < 2 ; ++i)
	{
		INT32 desigIndex = desigSet->minusEffect[i];

		const DesigAttr* attr = DesigUtil::FindDesigAttr(desigIndex);

		if( attr ) 
		{
			if ( i == 0)
				wstrTemp1 = attr->GetText(); 
			else
				i3::sprintf(wstrTemp2, L"%s\n%s", wstrTemp1, attr->GetText() );
		}
		else
		{
			if( i == 0)	
				i3::sprintf(wstrTemp1, GAME_RCSTRING("STR_TBL_DESIGNATION_NONE_WITH_COLOR2"));/*{col:205,100,50,255}����{/col}*/
		}
	}

	if ( !wstrTemp2.empty() )
		wstrTemp1 = wstrTemp2;

	{
		LuaState * L = UILua::CallLuaFunction(m_pScene, "SetMinusEffectString");
		i3Lua::PushStringUTF16To8(L, wstrTemp1);
		_EndLuaFunction(L, 1);
	}

	// Add Effect

	const i3::vector<UINT32>* pvecItemID = DesigUtil::FindItemIDsFromDesigType( (UINT8)desigSet->desigSetType );

	const INT32 numItemID = (pvecItemID) ? (INT32)pvecItemID->size() : 0;

	bool	UseSlotEffect[2];
	wstrTemp1.clear();		wstrTemp2.clear();

	i3::stack_wstring wstrAddEffect(L"INVALID");
	i3::rc_wstring wstrItemName(L"INVALID");

	for(INT32 i = 0; i< 2; ++i )
	{	
		UseSlotEffect[i] = false;

		INT32 desigIndex = desigSet->addEffect[i];

		const DesigAttr* attr = DesigUtil::FindDesigAttr(desigIndex);

		if( attr )		// (plus, minus�� �޸� 0�� ��� ���)..
		{

			if (desigIndex == 0 || attr->IsPurchasable() )
			{
				if ( i < numItemID )
				{
					usf::GetShopItemName(wstrItemName, (*pvecItemID)[i]);
					if ( !wstrItemName.empty() )
					{
						UseSlotEffect[i] = true;
						i3::sprintf(wstrAddEffect, GAME_RCSTRING("STR_HEADER_AUTHORITY_WEAPON"), wstrItemName);
					}
				}
			}
			else		// ȣĪ ����� ����...
			{
				wstrAddEffect =	attr->GetText(); 
				UseSlotEffect[i] = true;
			}
		}

		if(i==0 && UseSlotEffect[i] == true)
		{	//	ù��° ȣĪ ȿ��
			i3::sprintf( wstrTemp1, L"{col:204,204,204,255}%s{/col}", wstrAddEffect);
		}
		else if(i==1 && UseSlotEffect[i] == true)
		{	//	�ι�° ȣĪ ȿ��
			i3::sprintf( wstrTemp2, L"%s\n{col:204,204,204,255}%s{/col}", wstrTemp1, wstrAddEffect);
		}	
	}

	if (!wstrTemp2.empty())
		wstrTemp1 = wstrTemp2;

	// ȣĪ ȿ�� ����
	if(!UseSlotEffect[0] && !UseSlotEffect[1])
	{
		i3::sprintf( wstrTemp1, GAME_RCSTRING("STR_TBL_DESIGNATION_NONE_WITH_COLOR3"));/*{col:204,204,204,255}����{/col}*/	
	}	

	{
		LuaState * L = UILua::CallLuaFunction(m_pScene, "SetAddEffectString");
		i3Lua::PushStringUTF16To8(L, wstrTemp1);
		_EndLuaFunction(L, 1);
	}

	// Title Text/UI Shape Setting
	{
		LuaState* L = UILua::CallLuaFunction(m_pScene, "SetDesigDetailInfo");

		if(L != nullptr)
		{
			i3Lua::PushStringUTF16To8(L, desigSet->wstrName);
			i3Lua::PushInteger(L, m_SelectedDesignationIdx);
			_EndLuaFunction(L, 2);

		}
	}
}

void	UITabProfileDesignation_V10::_Update_GetInfoBox()
{
	i3::stack_wstring wstrCount;
	const DesigSet* pDesigSet = DesigUtil::FindDesigSetByUI( m_SelectedDesignationIdx );
	if(pDesigSet == nullptr) return;

	INT32 desigSetType = pDesigSet->desigSetType;

	// ������ ǥ�� ����

	// ����
	UINT32 ribbonCount = MedalContext::i()->getMiniature();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), ribbonCount);
	m_pHavedMedal[0]->SetText(wstrCount.c_str());

	// ����
	UINT32 InsigniaCount = MedalContext::i()->getInsignia();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), InsigniaCount);
	m_pHavedMedal[1]->SetText(wstrCount.c_str());

	// ����
	UINT32 orderCount = MedalContext::i()->getOrder();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), orderCount);
	m_pHavedMedal[2]->SetText(wstrCount.c_str());
	//
	// ������ ����
	UINT32 masterCount = MedalContext::i()->getMaster();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), masterCount);
	m_pHavedMedal[3]->SetText(wstrCount.c_str());

	// �ʿ��� ǥ�� ����
	INT32 iCount[ 4] = {0,};
	for(INT32 i = 0; i < USER_TITLE_REQUIREMENT_COUNT; ++i)
	{
		switch( g_UserTitleRequirement[desigSetType]._ribbon[i]._type)
		{
		case UTR_RIBBON:	iCount[ 0] += g_UserTitleRequirement[desigSetType]._ribbon[i]._count;	break;
		case UTR_ENSIGN:	iCount[ 1] += g_UserTitleRequirement[desigSetType]._ribbon[i]._count;	break;
		case UTR_MEDAL:		iCount[ 2] += g_UserTitleRequirement[desigSetType]._ribbon[i]._count;	break;
		case UTR_MASTER:	iCount[ 3] += g_UserTitleRequirement[desigSetType]._ribbon[i]._count;	break;
		case UTR_NOTHING:		break;
		}
	}

	for(INT32 i = 0; i < BADGE_TYPE_COUNT; i++)
	{
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_NEEDED_BADGECOUNT_TEXT"), iCount[ i]);
		m_pNeedMedal[i]->SetText(wstrCount.c_str());
	}

	// �ʿ� ȣĪ ����
	for( INT32 i = 0; i < 2; i++)
	{
		UINT8 reqDesigSetType = g_UserTitleRequirement[desigSetType]._userTitleId[i];

		const DesigSet* reqSet = DesigUtil::FindDesigSetByType( reqDesigSetType );

		if( reqSet )
			m_pNeedTitle[i]->SetText(reqSet->wstrName.c_str());
		else
			m_pNeedTitle[i]->SetText("----");
	}

	// �ʿ� ��� ����
	i3::stack_wstring wstrRankText = GetCharaRank(g_UserTitleRequirement[desigSetType]._rank);
	m_pNeedRank->SetText(wstrRankText.c_str());


	/////////////////////////////////////////////////////////////////////////
	// // ȣĪ ����ư Enable On/Off
	struct SetDesignationSetButton
	{
		void operator()( i3UIScene* pScene, bool bEnable) const
		{
			LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignSetEnable");
			i3Lua::PushBoolean(L, bEnable);
			UILua::EndLuaFunction(L, 1);
		}
	} fnSetDesignationSetButton;

	struct SetDesignationSetCaption
	{
		void operator()( i3UIScene* pScene, UINT32 CState) const
		{
			LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignationSetCaption");
			i3Lua::PushInteger(L, CState);
			UILua::EndLuaFunction(L, 1);
		}
	} fnSetDesignationSetCaption;

	// ȣĪ ȹ��/���/����
	{
		if(m_SelectedDesignationIdx == -1)
		{
			fnSetDesignationSetButton( GetScene(0), false);
			return;
		}
		fnSetDesignationSetButton(GetScene(0), true);

		if( !m_vecGetDesignation[m_SelectedDesignationIdx])
		{
			m_DesignationMode = DESIGNATION_MODE_GET;
			fnSetDesignationSetCaption(GetScene(0), 1);						/*ȣĪ ȹ��*/
		}
		else if( m_vecGetDesignation[m_SelectedDesignationIdx])
		{
			if( _CheckDesignationEquip())
			{
				m_DesignationMode = DESIGNATION_MODE_RELEASE;
				fnSetDesignationSetCaption(GetScene(0), 2);					/*ȣĪ ����*/
			}
			else
			{
				m_DesignationMode = DESIGNATION_MODE_CHANGE;
				fnSetDesignationSetCaption(GetScene(0), 3);					/*ȣĪ ���*/
			}
		}
	}
}



void	UITabProfileDesignation_V10::_Update_DesignationTree()
{
//	if( IsReadyToUpdate() == false) return;

	// ȹ���� ȣĪ�� ���Ͽ��� �׻� Ư���� ǥ�ø� �մϴ�.
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

	UINT8 TitleBuffer[MAX_EQUIP_USER_TITLE];
	myDesigInfo->GetEquipDesig(TitleBuffer);

	//������ ȣĪ �Ķ� �׵θ� ȿ�� 3������ �����˴ϴ�.
	// OnActivate -> CheckBox �̺�Ʈ �ڵ鸵�� ���Ƴ������μ� ������ ������Ʈ���� ������ŵ�ϴ�. ���Ŀ� �ٸ���Ʈ�ѷ� �����ؾ���(�Ķ� �׵θ� ���� ȿ��)
	for( INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		m_pButtonImage[i]->SetShapeIdx(0);
		if(m_vecGetDesignation[i])
		{
			m_pGainDesigFlag[i]->EnableCtrl(false);
		}
		
	}

	for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		const DesigSet* desigSet = DesigUtil::FindDesigSetByType(TitleBuffer[i]);
		if (desigSet)
		{
			INT32 idxUI = desigSet->desigSetUIIndex;
			if (idxUI < MAX_DEFAULT_USER_TITLE_COUNT)
				m_pButtonImage[idxUI]->SetShapeIdx(4);
		}
	}

}

// ���Ⱑ Ŭ���ϴ� �κ�. V1 �����ؼ� ����. 
void	UITabProfileDesignation_V10::__UpdateInfoFromLuaGlue()
{
	if( m_SelectedDesignationIdx >=0 && m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
	{

		__OnDoubleClickedDesignationButton();

		//__OnDoubleClickedDesignationButton();
		//_Update_DetailInfoBox();
		//_Update_GetInfoBox();		
		//_Update_DesignationTree();
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_DESIGNATION_GET_SELECT"));
	}
}

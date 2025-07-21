#include "pch.h"
#include "UITabProfileDesignation_V11.h"
#include "UI/UIMainFrame_V11.h"
#include "UIPhaseProfile_V11.h"
#include "UI/UIUtil.h"

#include "Designation/DesigUtil.h"
#include "Designation/UserDesigInfo.h"
#include "Designation/DesigAttr.h"
#include "Designation/DesigSet.h"

#include "UI/UIShopFunction.h"
#include "../StepProfileDef.h"
#include "MedalContext.h"
#include "UI/UICharaInfoMgr.h"

#include "UserInfoContext.h"

//
// �κ��� get_default_item�� ���⿡ ������� �ߺ��ۼ��ϰ� ���� �صд�.. ( 2015.05.29.����)
// �κ� ���ӽ����̽��� �ִ� ���� ���� ����ϱⰡ �������� ������...
//

namespace
{
	// �� �κ��� V2 �������� ���� ������ �����ϰ� �Ϸᰡ �Ǿ� ���� �ʱ� ������ �ټ� �Ҿ����ϴ�..(2015.05.29.����)
	// �ش� �Լ��� ȣĪ������ ����սô�. (2015.07.29 ���õ)
	void CheckUserTitlesBeret()
	{
		 //�����ϴ� ȣĪ�� ���� �����ϰ� �ִ� ������ �����ϱ� ����
		 //�ʿ� ȣĪ�̶�� ���� �����ϰ� �ִ� ������ �����. (�������� �˾Ƽ� ����ٰ� �����ϰ� ���⼭��
		 //��Ƽ���׸� �Ű� ������ �Ѵ�...)

		// �ش� �Լ��� ȣĪ������ ����սô�.
		CharaInfoMgr::instance()->list_up();

		// ���� �������� ������ �����ɴϴ� (ĳ���ͺ� ����)
		const ITEM_INFO & beret_info = CharaInfoMgr::instance()->get_current_equip_item(UserEquipInfo::SUE_BERET);
		if (false == DesigUtil::CheckWearable_MyItemByDesig(beret_info.m_TItemID))
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
}

static UITabProfileDesignation_V11 * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	UIPhaseSubBase* pSubPhase = pFrame->GetCurrentSubPhase();
	UITabProfileDesignation_V11* pTab = static_cast<UIPhaseProfile_V11*>(pSubPhase)->GetDesignation();
	I3ASSERT( i3::same_of<UITabProfileDesignation_V11*>(pTab)); 

	return static_cast<UITabProfileDesignation_V11*>(pTab);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UITabProfileDesignation_V11_Click(LuaState * L)
	{
		GetThis()->__UpdateInfoFromLuaGlue();
		return 0;
	}
	int UITabProfileDesignation_V11_MyDesigClick(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		GetThis()->__MyDesigClickFromLuaGlue(idx);
		return 0;
	}
	int UITabProfileDesignation_V11_MyDesigDoubleClick(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		GetThis()->__MyDesigDoubleClickFromLuaGlue(idx);
		return 0;
	}
	int UITabProfileDesignation_V11_Release(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		GetThis()->__MyEquipReleaseFromLuaGlue(idx);
		return 0;
	}
}

LuaFuncReg UITabProfileDesignation_V11_Glue[] =
{
	{ "ClickMyDesigSlot",	UITabProfileDesignation_V11_MyDesigClick		},
	{ "D_ClickMyDesigSlot",	UITabProfileDesignation_V11_MyDesigDoubleClick	},
	{ "UseDesignation",		UITabProfileDesignation_V11_Click				},
	{ "ClickXButton",		UITabProfileDesignation_V11_Release				},
	{ nullptr,		nullptr}
};

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
			GetThis()->__OnDesignationChange();
		}
	}
	
	
}


I3_CLASS_INSTANCE( UITabProfileDesignation_V11);

UITabProfileDesignation_V11::UITabProfileDesignation_V11() : m_vecGetDesignation(MAX_DEFAULT_USER_TITLE_COUNT)
{
	m_SelectedDesignationIdx = -1;
	m_OldSelectedDesignationIdx = -1;

	m_SelectedDesignationChangeSlotIdx = -1;
	m_SelectedDesignationReleaseSlotIdx = -1;
	m_MySlotIdx = -1;
	m_DesignationMode = DESIGNATION_MODE_GET;

	m_nCurShowingDesignationType = 0;

	// ���� 4���� ��� ������ �迭�� �׳� memset���� ����..
	::memset(m_pCheckBox, 0, sizeof(m_pCheckBox));				// ���۷��� ������ �̰͸�..
	::memset(m_pImageBox, 0, sizeof(m_pImageBox));
	::memset(m_pButtonImage, 0, sizeof(m_pButtonImage));
	::memset(m_dummyButton, 0, sizeof(m_dummyButton));

	m_nCurDesignationSlotCount = 0;
}

UITabProfileDesignation_V11::~UITabProfileDesignation_V11()
{
}


struct SetMyInfoDesignation_Image_Enable
{
	void operator()(i3UIScene* pScene, UINT32 idx, bool Sig) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Enable");
		i3Lua::PushInteger(L, idx);
		i3Lua::PushBoolean(L, Sig);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Enable;
	
struct SetMyInfoDesignation_Image_Lock
{
	void operator()(i3UIScene* pScene, UINT32 idx, bool Sig) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Lock");
		i3Lua::PushInteger(L, idx);
		i3Lua::PushBoolean(L, Sig);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Lock;


struct SetMyInfoDesignation_Image_Index
{
	void operator()(i3UIScene* pScene, UINT32 slot_idx, UINT32 desigSetIndex) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Index");
		i3Lua::PushInteger(L, slot_idx);
		i3Lua::PushInteger(L, desigSetIndex);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Index;


void UITabProfileDesignation_V11::_Update_All( void)
{
	// 1. ���� ȹ�� üũ..
	{
		const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();
		INT32 limitTitleCount = myDesigInfo->GetLimitCount();
	
		// �߰������� ȹ���Ͽ��ٸ�~!!  --> ???
		if ( limitTitleCount > m_nCurDesignationSlotCount )
		{
			m_nCurDesignationSlotCount = limitTitleCount;
			
			i3::stack_wstring wstrMsg;
			/*���ϵ帳�ϴ�!!\n \nȣĪ ������ ȹ���Ͽ� ȣĪ�� ���ÿ�\n����� �� �ִ� ������ �����Ͽ����ϴ�.\n(�ִ� %d���� ȣĪ ��� ����)*/
			i3::sprintf(wstrMsg, GAME_RCSTRING("STR_TBL_DESIGNATION_GET_SLOT_MESSAGE"), m_nCurDesignationSlotCount);
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrMsg);
		}
	}

	// 2. ȹ�� ȣĪ UI ǥ�� ����..
	{
		DesigUtil::AcquireGainedDesigSetsForUI( m_vecGetDesignation, MAX_DEFAULT_USER_TITLE_COUNT);

		COLOR tColor;	tColor.a = 255;
		const size_t num_GetDesig = m_vecGetDesignation.size();
		for (size_t i = 0 ; i < num_GetDesig ; ++i)
		{
			tColor.r = tColor.g = tColor.b = ( m_vecGetDesignation[i] ) ? 255 : 33;
			m_pImageBox[i]->setColor(&tColor);
			m_pButtonImage[i]->setColor(&tColor);
		}
	}

	// 3. �� ���� �ڽ� �����..
	_Update_DetailInfoBox();
	_Update_GetInfoBox();
	
	// 4. �� ���� ȣĪ ����..
	if( IsReadyToUpdate() )
	{
		const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

		INT32 iLimitTitleCount = myDesigInfo->GetLimitCount();
		
		I3ASSERT( iLimitTitleCount <= MAX_EQUIP_USER_TITLE);
		
		for( INT32 i = 0; i < iLimitTitleCount; ++i)
		{
			fnSetMyInfoDesignation_Image_Enable(GetScene(0), i,false);
			fnSetMyInfoDesignation_Image_Lock(GetScene(0), i,false);//Enable true, false
		}
		
		for( INT32 i = iLimitTitleCount; i < MAX_EQUIP_USER_TITLE; ++i)
		{
			fnSetMyInfoDesignation_Image_Enable(GetScene(0), i,false);
			fnSetMyInfoDesignation_Image_Lock(GetScene(0), i,true);
		}

		// ȣĪ�̹��� �� ȣĪ�̸�.
		for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; ++i)
		{
			UINT8 myEquipDesigSet = myDesigInfo->GetEquipDesig(i);

			if( myEquipDesigSet > MAX_DEFAULT_USER_TITLE_COUNT)
				continue;
		
			const DesigSet* desigSet = DesigUtil::FindDesigSetByType(myEquipDesigSet);

			if (desigSet)
			{
				fnSetMyInfoDesignation_Image_Enable(GetScene(0), i, true);
				fnSetMyInfoDesignation_Image_Index(GetScene(0), i, desigSet->desigSetUIIndex );
			}
		}		
		
	}

	// 5. ������ ȣĪ ����.
	_Update_DesignationTree();					
}

void UITabProfileDesignation_V11::_Update_DetailInfoBox()
{
	if( IsReadyToUpdate() == false) return;

	i3::stack_wstring wstrTemp1;
	i3::stack_wstring wstrTemp2;
//	char szName_Explain[ MAX_STRING_COUNT];			// �̻��ϰ� 1.1������ Explain�� ��������..

	if( (m_SelectedDesignationIdx != -1) && (m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT))
	{
		const DesigSet* desigSet = DesigUtil::FindDesigSetByUI(m_SelectedDesignationIdx);

		m_nCurShowingDesignationType = desigSet->desigSetType;
	
		// �̹��� �ε��� ��Ʒ� ����..
		{
			LuaState * L = _CallLuaFunction("SetDesignImage");
			i3Lua::PushInteger(L, m_SelectedDesignationIdx);
			_EndLuaFunction(L, 1);
		}
	
		// ȣĪ �̸� ǥ��..
		{
			i3::sprintf( wstrTemp1, L"{col:80,150,215,255}%s{/col}\n{col:204,204,204,255}%s{/col}", desigSet->wstrName, desigSet->wstrNameE );
			LuaState * L = _CallLuaFunction("SetDesignTitle");
			i3Lua::PushStringUTF16To8(L, wstrTemp1);
			_EndLuaFunction(L, 1);
		}
		
			// Plus Effect
		wstrTemp1.clear();		wstrTemp2.clear();

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
			LuaState * L = _CallLuaFunction("SetPlusEffectString");
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
			LuaState * L = _CallLuaFunction("SetMinusEffectString");
			i3Lua::PushStringUTF16To8(L, wstrTemp1);
			_EndLuaFunction(L, 1);
		}
		
		// Add Effect  (ȣĪ ����� ��쵵 �ֱ� ������ �������� ���� ����..)
		// �ϴ� ���� ������ �����Ͽ� ȣĪ ���� ���� ������ �������� �Լ��Ѵ�..
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
			LuaState * L = _CallLuaFunction("SetAddEffectString");
			i3Lua::PushStringUTF16To8(L, wstrTemp1);
			_EndLuaFunction(L, 1);
		}

	}
	else
	{
		m_nCurShowingDesignationType = 0;

		// �̹��� �ε��� ��Ʒ� ����..
		{
			LuaState * L = _CallLuaFunction("SetDesignImage");
			i3Lua::PushInteger(L, -1);		// -1����.. (�ƹ��͵� ���� �ȵǴ� ��?)
			_EndLuaFunction(L, 1);
		}

		// ȣĪ �̸� ǥ�� ( ����ش�..)
		{
			LuaState * L = _CallLuaFunction("SetDesignTitle");
			i3Lua::PushStringUTF16To8(L, L"");
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetPlusEffectString");
			i3Lua::PushStringUTF16To8(L, L"");
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetMinusEffectString");
			i3Lua::PushStringUTF16To8(L, L"");
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetAddEffectString");
			i3Lua::PushStringUTF16To8(L, L"");
			_EndLuaFunction(L, 1);
		}
	}
}



void UITabProfileDesignation_V11::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
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


//
void UITabProfileDesignation_V11::__UpdateInfoFromLuaGlue()
{
	if( IsReadyToUpdate() == false) return;

	if( m_SelectedDesignationIdx >=0 && m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
	{
		_Update_DetailInfoBox();
		_Update_GetInfoBox();
		__OnDoubleClickedDesignationButton();
		_Update_DesignationTree();	
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_DESIGNATION_GET_SELECT"));
	}
}


/*virtual*/ bool UITabProfileDesignation_V11::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/ )
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
		bool bCheckBox = false;

		for(INT32 i=0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
		{
			if(m_pCheckBox[i] == pParam->m_pCtrl)
			{
				bCheckBox = true;
				break;
			}
		}
		
		if(	bCheckBox == false ) 	
		{
			if(pParam->m_nEvent == I3UI_EVT_CLICKED)
				I3TRACE(pParam->m_pCtrl->GetName());
			return UITabProfileBase_V11::OnEvent( event, pObj, param2, code);
		}

		if (pParam->m_nEvent == I3UI_EVT_BTN_PUSHED)
		{
			I3ASSERT(pParam->m_pCtrl != nullptr);
			INT32 curSel = i3::atoi(pParam->m_pCtrl->GetName());

			if (m_SelectedDesignationIdx == curSel)		// ������ �������� ������ �����ؾߵ�..
			{
				m_pCheckBox[m_SelectedDesignationIdx]->addState(I3UI_STATE_SELECTED);
				m_pCheckBox[m_SelectedDesignationIdx]->setModifyState(true);
			}
		}
		else
		if( pParam->m_nEvent == I3UI_EVT_DBL_CLICKED)
		{
			I3ASSERT( pParam->m_pCtrl != nullptr);
			INT32 curSel = i3::atoi(pParam->m_pCtrl->GetName());

			__SetCurrentButton(curSel);

			if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
			{
				m_pCheckBox[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
				m_pCheckBox[m_OldSelectedDesignationIdx]->setModifyState(true);
			}
		
			_Update_DetailInfoBox();
			_Update_GetInfoBox();
			__OnDoubleClickedDesignationButton();
			_Update_DesignationTree();
		}
		else if( pParam->m_nEvent == I3UI_EVT_CLICKED)
		{
			I3ASSERT( pParam->m_pCtrl != nullptr);
			INT32 curSel = i3::atoi(pParam->m_pCtrl->GetName());

			__SetCurrentButton(curSel);

			I3TRACE("Old_Select_Idx : %d\n", m_OldSelectedDesignationIdx);
			I3TRACE("Current_Select_Idx : %d\n", m_SelectedDesignationIdx);
			if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
			{
				m_pCheckBox[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
				m_pCheckBox[m_OldSelectedDesignationIdx]->setModifyState(true);
			}
			_Update_DetailInfoBox();
			_Update_GetInfoBox();
		}

	}

	return UITabProfileBase_V11::OnEvent( event, pObj, param2, code);
}

/*virtual*/ void UITabProfileDesignation_V11::OnCreate( i3GameNode * pParent)
{
	UITabProfileBase_V11::OnCreate( pParent);

	char * pszPath = "Scene/Main/PointBlankRe_MyInfomation_Designation.i3UIs";
	AddScene( pszPath,UITabProfileDesignation_V11_Glue);
}

void UITabProfileDesignation_V11::_Update_GetInfoBox( void)
{	
	if( IsReadyToUpdate() == false) return;

	// ������ �ӹ�ī��� ������ �ֱ� ������, ������ �������� �ִٸ� �ӹ�ī�� ó���Ҷ� ���� �� ��...(2015.05.29.����)
	// ���� ǥ��..

	i3::stack_wstring wstrCount;

	{
		struct SetDesigAppendixString
		{
			void operator()( i3UIScene* pScene, BADGE_TYPE badgeType, const i3::stack_wstring& wstr) const
			{
				LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignAppendixString");
				i3Lua::PushInteger(L, badgeType);
				i3Lua::PushStringUTF16To8(L, wstr);;
				UILua::EndLuaFunction(L, 2);
			}
		} fnSetDesigAppendixString;

	
		// ����
		UINT32 ribbonCount = MedalContext::i()->getMiniature();
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), ribbonCount);
		fnSetDesigAppendixString(GetScene(0), BADGE_TYPE_RIBBON, wstrCount);
		
		// ����
		UINT32 InsigniaCount = MedalContext::i()->getInsignia();
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), InsigniaCount);
		fnSetDesigAppendixString(GetScene(0), BADGE_TYPE_ENSIGN, wstrCount);
		
		// ����
		UINT32 orderCount = MedalContext::i()->getOrder();
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), orderCount);
		fnSetDesigAppendixString(GetScene(0), BADGE_TYPE_MEDAL, wstrCount);
		//
		// ������ ����
		UINT32 masterCount = MedalContext::i()->getMaster();
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), masterCount);
		fnSetDesigAppendixString(GetScene(0), BADGE_TYPE_MASTER, wstrCount);
	}

	struct SetDesignMedalCount
	{
		void operator()( i3UIScene* pScene, INT32 idx, const i3::stack_wstring& wstrCount) const
		{
			LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignMedalCount");
			i3Lua::PushInteger(L, idx);
			i3Lua::PushStringUTF16To8(L, wstrCount);
			UILua::EndLuaFunction(L, 2);
		}
	} fnSetDesignMedalCount;

	struct SetNeedText
	{
		void operator()( i3UIScene* pScene, UINT32 idx, const i3::stack_wstring& wstrDesigName) const
		{
			LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignNeedText");
			i3Lua::PushInteger(L, idx);
			i3Lua::PushStringUTF16To8(L, wstrDesigName);
			UILua::EndLuaFunction(L, 2);
		}
	} fnSetNeedText;

	struct SetDesignNeedRankText
	{
		void operator()( i3UIScene* pScene, const i3::stack_wstring& wstrNeedRank) const
		{
			LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignNeedRankText");
			i3Lua::PushStringUTF16To8(L, wstrNeedRank);
			UILua::EndLuaFunction(L, 1);
		}
	} fnSetDesignNeedRankText;

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

	if( m_SelectedDesignationIdx != -1)
	{
		struct SetDesignationSetCaption
		{
			void operator()( i3UIScene* pScene, UINT32 CState) const
			{
				LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignationSetCaption");
				i3Lua::PushInteger(L, CState);
				UILua::EndLuaFunction(L, 1);
			}
		} fnSetDesignationSetCaption;

		{
			// ȣĪ ȹ��/���/����
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

		if( m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
		{
			const DesigSet* pDesigSet = DesigUtil::FindDesigSetByUI( m_SelectedDesignationIdx );
			INT32 desigSetType = pDesigSet->desigSetType;

			// 1.�ʿ� ǥ�� ����
		
			INT32 iCount[ BADGE_TYPE_COUNT] = {0,};
		
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
				fnSetDesignMedalCount(GetScene(0), i, wstrCount);
			}
				
			// 2. �ʿ� ȣĪ ����

			for( INT32 i = 0; i < 2; i++)
			{
				UINT8 reqDesigSetType = g_UserTitleRequirement[desigSetType]._userTitleId[i];

				const DesigSet* reqSet = DesigUtil::FindDesigSetByType( reqDesigSetType );
				
				if( reqSet )
					fnSetNeedText(GetScene(0), i+1, reqSet->wstrName);
				else
					fnSetNeedText(GetScene(0), i+1, L"----");

			}
	
			// 3. �ʿ� ��� ����
			i3::stack_wstring wstrRankText = GetCharaRank(g_UserTitleRequirement[desigSetType]._rank);
			fnSetDesignNeedRankText(GetScene(0), wstrRankText);

		}
	}
	else
	{
		for(INT32 i = 0; i < BADGE_TYPE_COUNT; ++i)
		{
			i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_NEEDED_BADGECOUNT_TEXT"), 0);
			fnSetDesignMedalCount(GetScene(0), i, wstrCount);
		}

		fnSetNeedText(GetScene(0), 1, L"{col:204, 204, 204, 255}----{/col}");
		fnSetNeedText(GetScene(0), 2, L"{col:204, 204, 204, 255}----{/col}");
		fnSetDesignNeedRankText(GetScene(0), L"{col:204, 204, 204, 255}----{/col}");
		fnSetDesignationSetButton(GetScene(0), false);
	}

}

void UITabProfileDesignation_V11::__OnDoubleClickedDesignationButton()
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
		// ȣĪ ȹ��  OnDesignationGet();
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

bool UITabProfileDesignation_V11::_CheckDesignationEquip(void)
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

void UITabProfileDesignation_V11::__OnDesignationChange()
{
	//�󽽷��� ã�ƾ� �Ѵ�.
	//�󽽷��� �ε����� �޾ƿ´�.
	//3ĭ�� ����ִٸ� ���� ��������.
	//

	m_SelectedDesignationChangeSlotIdx = GetEmptySlot();
	
	if( m_SelectedDesignationChangeSlotIdx >= 0 && m_SelectedDesignationChangeSlotIdx < 3)
	{
		CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();
//		const INT32 numDesigSetType = DesigUtil::GetNumDesigSetByType();

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

void UITabProfileDesignation_V11::_Update_DesignationTree()
{
	if( IsReadyToUpdate() == false)return;

	// ȹ���� ȣĪ�� ���Ͽ��� �׻� Ư���� ǥ�ø� �մϴ�.
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

	UINT8 TitleBuffer[MAX_EQUIP_USER_TITLE];
	myDesigInfo->GetEquipDesig(TitleBuffer);

	//������ ȣĪ �Ķ� �׵θ� ȿ�� 3������ �����˴ϴ�.
	// OnActivate -> CheckBox �̺�Ʈ �ڵ鸵�� ���Ƴ������μ� ������ ������Ʈ���� ������ŵ�ϴ�. ���Ŀ� �ٸ���Ʈ�ѷ� �����ؾ���(�Ķ� �׵θ� ���� ȿ��)
	
	

	bool deactviated_idx[MAX_DEFAULT_USER_TITLE_COUNT];
	memset(&deactviated_idx, 0, sizeof(deactviated_idx));
	


	for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		const DesigSet* desigSet = DesigUtil::FindDesigSetByType(TitleBuffer[i]);
		if (desigSet)
		{
			INT32 idxUI = desigSet->desigSetUIIndex;
			if (idxUI < MAX_DEFAULT_USER_TITLE_COUNT)
			{
				deactviated_idx[idxUI] = true;
			//	m_pCheckBox[idxUI]->OnActivate(false);
			}
		}
	}
	
	for (INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		if (deactviated_idx[i] == true)
		{
			m_pCheckBox[i]->OnActivate(false);
		}
		else
		{
			m_pCheckBox[i]->OnActivate(true);
		}
	}


}


bool UITabProfileDesignation_V11::OnEntranceStart( void * pArg1, void * pArg2 )
{
	if( UITabProfileBase_V11::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	LuaState * L = _CallLuaFunction("InitUI");
	_EndLuaFunction(L,0);

	m_SelectedDesignationIdx = -1;
	// ȣĪ �̹����� ��ġ�� �°� ȣ���մϴ�.
	for(INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		m_pButtonImage[i]->SetShapeIdx(i);
	}

	for(INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++) //���� �ϴ� ����ĭ�� �̹����� �ʱ�ȭ �մϴ�.
	{
		fnSetMyInfoDesignation_Image_Enable(GetScene(0), i,false);
		fnSetMyInfoDesignation_Image_Lock(GetScene(0), i,true);
	}

	_Update_All();

	return true;
}


bool UITabProfileDesignation_V11::OnClick( i3UIControl * pControl)
{
	if( UITabProfileBase_V11::OnClick( pControl) == false)
		return false;

	if( i3::same_of<i3UICheckBox*>(pControl)) 
		PlaySound( UISND_CLICK);

	return true;
}

bool UITabProfileDesignation_V11::OnEnterMouse( i3UIControl * pControl)
{
	if( UITabProfileBase_V11::OnEnterMouse( pControl) == false)
		return false;

	if( i3::generic_is_iequal( pControl->GetNameString(), "i3UIButtonConfirm") )
		PlaySound( UISND_ENTER_MOUSE1);

	return true;
}

void UITabProfileDesignation_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UITabProfileBase_V11::_InitializeAtLoad( pScene);

	I3PRINTLOG(I3LOG_NOTICE, "ProfileDesignation Loading....\n");

	i3UIFrameWnd * pFrameWndIcon = (i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd3_MID");

	if( pFrameWndIcon != nullptr)
	{
		m_pCheckBoxTemplate = i3UI::getUILibrary()->FindByName("CheckBox_Designation");
		VEC3D* pos;
		char szCountName[ MAX_STRING_COUNT];
		INT32 Count1 = 0, Count2 = 0;
		INT32 ChildCount = pFrameWndIcon->GetChildCount();
		i3UIControl * pChild = (i3UIControl *)pFrameWndIcon->getFirstChild();
		
		for(INT32 i = 0; i < ChildCount ; i++)
		{
			if(i3::kind_of<i3UIImageBox*>(pChild)) 
			{
				m_pImageBox[Count1] = (i3UIImageBox *)pChild;
				++Count1;
			}
			else if(i3::kind_of<i3UIButtonImageSet*>(pChild)) 
			{
				m_pButtonImage[Count2] = (i3UIButtonImageSet *)pChild;
				m_pButtonImage[Count2]->SetViewOnly(true);
				++Count2;
			}
			pChild = (i3UIControl *)pChild->getNext();
		}

		pChild = (i3UIControl *)pFrameWndIcon->getFirstChild();
		Count1 = Count2 = 0;

		VEC3D tPos;

		for(INT32 i = 0; i < ChildCount ; i++)
		{
			if(i3::kind_of<i3UIButtonImageSet*>(pChild)) 
			{
				//üũ�ڽ� ��Ʈ�� ����
				i3UICheckBox * pChkBox = (i3UICheckBox*) i3UI::CreateControl( m_pCheckBoxTemplate, &tPos, pChild->getSize(), pChild);

				i3::itoa(Count2, szCountName, _countof(szCountName));
				++Count2;
				pChkBox->SetName( szCountName);
				pChkBox->SetForceEnableInput(true);

				m_pCheckBox[Count1] = pChkBox;

				pos = pChild->getPos();
				m_pImageBox[Count1]->setPos(pos);
				m_pImageBox[Count1]->setSize(pChild->getSize());
				++Count1;

			}
			pChild = (i3UIControl *)pChild->getNext();
		}

	}

	//Lock(�ڹ���)�̹����� ȣĪ ����/��ü �̺�Ʈ�� ���� ���� �̺�Ʈ ��Ʈ�� ����, �����ϰ� ó���Ѵ�.
	COLOR tColor;
	tColor.a = 0;
	tColor.r = tColor.g = tColor.b = 0;
	m_dummyButton[0] = (i3UIImageBox *)pScene->FindChildByName("i3UIDummyButton1");
	m_dummyButton[0]->setColor(&tColor);
	m_dummyButton[1] = (i3UIImageBox *)pScene->FindChildByName("i3UIDummyButton2");
	m_dummyButton[1]->setColor(&tColor);
	m_dummyButton[2] = (i3UIImageBox *)pScene->FindChildByName("i3UIDummyButton3");
	m_dummyButton[2]->setColor(&tColor);
	
	const CUserDesigInfo* userDesigInfo = DesigUtil::GetMyUserDesigInfo();
	m_nCurDesignationSlotCount = userDesigInfo->GetLimitCount();

}

//
void UITabProfileDesignation_V11::OnLoadAllScenes()
{
	GetSlide()->AddFrameWnds(GetScene(0));
}

void UITabProfileDesignation_V11::OnUnloadAllScenes()
{
}

void UITabProfileDesignation_V11::_ClearAtUnload( i3UIScene * pScene)
{
	for(INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		if( m_pCheckBox[ i] != nullptr)
		{
			i3UI::DeleteControl( m_pCheckBox[ i]);
			m_pCheckBox[ i] = nullptr;
		}
	}

	UITabProfileBase_V11::_ClearAtUnload( pScene);
}

void UITabProfileDesignation_V11::__SetCurrentButton(INT32 idx)
{
	if(idx != m_SelectedDesignationIdx)
	{
		m_OldSelectedDesignationIdx = m_SelectedDesignationIdx;
		m_SelectedDesignationIdx = idx;
	}
}

void UITabProfileDesignation_V11::__MyDesigClickFromLuaGlue(INT32 idx)
{
	m_MySlotIdx = idx;

	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo(); 
	UINT8 myDesigEquipTypes[MAX_EQUIP_USER_TITLE];
	myDesigInfo->GetEquipDesig(myDesigEquipTypes);
	
	const DesigSet* desigSet = DesigUtil::FindDesigSetByType( myDesigEquipTypes[idx] );

	if (desigSet)
	{
		INT32 idxUI = desigSet->desigSetUIIndex;
		__SetCurrentButton( idxUI );
		if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
		{
			m_pCheckBox[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
			m_pCheckBox[m_OldSelectedDesignationIdx]->setModifyState(true);
		}
		_Update_DetailInfoBox();
		_Update_GetInfoBox();
	}
}

void UITabProfileDesignation_V11::__MyDesigDoubleClickFromLuaGlue(INT32 idx)
{
	m_MySlotIdx = idx;

	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo(); 
	UINT8 myDesigEquipTypes[MAX_EQUIP_USER_TITLE];
	myDesigInfo->GetEquipDesig(myDesigEquipTypes);

	const DesigSet* desigSet = DesigUtil::FindDesigSetByType( myDesigEquipTypes[idx] );

	if (desigSet)
	{
		INT32 idxUI = desigSet->desigSetUIIndex;
		__SetCurrentButton( idxUI );

		if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
		{
			m_pCheckBox[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
			m_pCheckBox[m_OldSelectedDesignationIdx]->setModifyState(true);
		}
		_Update_DetailInfoBox();
		_Update_GetInfoBox();
		__OnDoubleClickedDesignationButton();
		_Update_DesignationTree();	
	}
	else
	{
		__OnDoubleClickedDesignationButton();
	}
}


void UITabProfileDesignation_V11::__MyEquipReleaseFromLuaGlue(INT32 slotindex)
{

	if( m_SelectedDesignationIdx >= 0 && m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
	{
		m_pCheckBox[m_SelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
		m_pCheckBox[m_SelectedDesignationIdx]->setModifyState(true);
	}
	
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo(); 
	UINT8 myDesigEquipTypes[MAX_EQUIP_USER_TITLE];
	myDesigInfo->GetEquipDesig(myDesigEquipTypes);

	const DesigSet* desigSet = DesigUtil::FindDesigSetByType( myDesigEquipTypes[slotindex] );

	if (desigSet == nullptr)
	{
		m_SelectedDesignationIdx = -1;
		__UpdateInfoFromLuaGlue();
		return;
	}
	
	m_SelectedDesignationIdx = desigSet->desigSetUIIndex;
	__UpdateInfoFromLuaGlue();

}

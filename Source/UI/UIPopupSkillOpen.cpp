#include "pch.h"
#include "UIPopupSkillOpen.h"

#include "ItemImageMgr.h"
#include "UIMainFrame.h"
#include "Useable_Item_Def.h"
#include "ItemConsumer.h"
#include "UIShopFunction.h"
#include "UIUtil.h"

#include "i3Base/string/ext/utf8_to_utf16.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"


I3_CLASS_INSTANCE( UIPopupSkillOpen);

static UIPopupSkillOpen* GetThis()
{
	UIPopupSkillOpen* pSkillopen = static_cast<UIPopupSkillOpen*>(GameUI::GetPopup(UPW_SKILLOPEN_ITEM));
	if(pSkillopen != nullptr)
	{
		return pSkillopen;
	}
	return nullptr;
}

namespace inner
{
	void Reuse(void * pThis, INT32 nParam);
}

void inner::Reuse(void * pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		GetThis()->use_item();
	}
}


/***********************//***********************/
extern "C" 
{
	int skill_item_click( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);

		char chara_name[MAX_STRING_COUNT] = "";
		i3Lua::GetStringArg( L, 2, chara_name, MAX_STRING_COUNT);

		// UTF-8�� �����Ѵ�.
		i3::stack_wstring wstr_chara_name;
		i3::utf8_to_utf16( chara_name, wstr_chara_name);
		
		UIPopupSkillOpen* pPopup = GetThis();
		if(pPopup != nullptr)
		{
			pPopup->SetCharaIdx(wstr_chara_name);
			pPopup->clicked(idx);
		}
		return idx;
	}
}

LuaFuncReg skill_item_glue[] =
{
	{ "Click", skill_item_click	},
	{ nullptr, nullptr}
};

/***********************//***********************/

UIPopupSkillOpen::UIPopupSkillOpen() : chara_idx(-1)
{

}

UIPopupSkillOpen::~UIPopupSkillOpen()
{

}

void UIPopupSkillOpen::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate(pParent);

	AddScene("Scene/Popup/PBRe_PopUp_item_Use.i3UIs", skill_item_glue);
}

bool UIPopupSkillOpen::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/ )
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
	{
		return false;
	}

	item_query_style = *( static_cast<ITEM_QUERY_STYLE*>( pArg2 ) );
	if(item_query_style == IQS_REMOVE)
	{
		const ITEM_INFO* info = static_cast<ITEM_INFO*>( pArg1 );
		m_item_id = info->m_TItemID;
		m_item_db_idx = info->m_TItemDBIdx;
	}
	else
	{
		m_item_id = *( static_cast<T_ItemID*>( pArg1 ) );
		m_item_db_idx = CInvenList::i()->FindWareDBIdx_ByItemID(m_item_id);
	}

	SetInfo();
	
	GetSlide()->ForceEnd();
	
	return true;
}

void UIPopupSkillOpen::SetInfo()
{
	LuaState * L = 0;
	
	i3::rc_wstring wstrItemName;
	usf::GetShopItemName(wstrItemName, m_item_id);

	i3::rc_wstring wstrItemDescription;
	usf::GetShopItemDescription(wstrItemDescription, m_item_id);

	L = _CallLuaFunction( "SetItemInfo");
	if ( L != nullptr )
	{
		i3Lua::PushStringUTF16To8(L, wstrItemName.c_str(), wstrItemName.size());
		i3Lua::PushStringUTF16To8(L, wstrItemDescription.c_str(), wstrItemDescription.size());
		_EndLuaFunction(L, 2);
	}

	i3::stack_wstring character_list;
	getCharaString(character_list);		

	L = _CallLuaFunction( "SetComboBox");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, character_list);		
		i3Lua::PushInteger(L, CCharaInfoContext::i()->GetMainCharaIdx());
		_EndLuaFunction(L, 2);
	}

	GetItemImageMgr()->SelectImage(m_pImageCtrl, m_item_id);	
}


void UIPopupSkillOpen::getCharaString(i3::stack_wstring& out)
{
	out.clear();

	INT32 iskillsize[3];
	INT32 _level = 0;
	NCHARA_SKILL * pSkillSet = nullptr;
	REAL32 rPlayTime = 0.f;
	
	iskillsize[0] = MAX_COMMON_SKILL;
	iskillsize[1] = MAX_MAIN_SKILL;
	iskillsize[2] = MAX_ASSIST_SKILL;

	INT32 character_count = CCharaInfoContext::i()->GetHasCharacterCount();

	for(INT32 i = 0; i < character_count; i++)
	{
		const CHARA_INFO* chara = CCharaInfoContext::i()->GetCharaInfoByList(i);
		
		if( chara)
		{

			rPlayTime = chara->m_ui32SkillRelTime / (REAL32)3600;			//�÷��̽ð�
		}
		
		INT32 j = 0;
		for(j = 0; j < iskillsize[CATEGORYLIST_MAIN]; j++)
		{
			pSkillSet = g_pSkillSystemValueSet->getSkillsystemSet( CATEGORYLIST_MAIN, j);

			if(pSkillSet == nullptr)
				continue;

			//�������
			if( pSkillSet->getApplyCategory() == CATEGORYLIST_UNKNOWN)
				continue;

			if(i == CATEGORYLIST_MAIN)
			{
				if(chara != nullptr && pSkillSet->isWeaponType( chara->m_CharaSkillInfo.m_PrimarySkill) == false)
					continue;
			}
			
			_level =pSkillSet->getApplyRequisiteLevel(static_cast<INT32>(rPlayTime));
			if(_level < 10)
			{
				break;
			}
		}
		
		if(j != iskillsize[CATEGORYLIST_MAIN])
		{
			const i3::wliteral_range tmp_str = (chara) ? chara->m_CharaBasicInfo.m_strCharaName : i3::wliteral_range();

			out.append(tmp_str);	out += L';'; 
		}
	}
	
	if(out.size() <= 0)
	{
		out = L"No Select Character;";
	}

	out.erase( out.size() -1);
}

void UIPopupSkillOpen::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();

	m_pImageCtrl = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet");
}

void UIPopupSkillOpen::OnUnloadAllScenes()
{
	UIPopupBase::OnUnloadAllScenes();

	m_pImageCtrl->RestoreSizePos();
	I3_SAFE_RELEASE(m_pImageCtrl);
}

bool UIPopupSkillOpen::OnKey_Enter()
{
	clicked( OK );

	return UIPopupBase::OnKey_Enter();
}

bool UIPopupSkillOpen::OnKey_Escape( void )
{
	clicked( CANCEL );

	return UIPopupBase::OnKey_Escape();
}

void UIPopupSkillOpen::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData )
{
	//	�̺�Ʈ ����� �޾� ó���Ѵ�.
	switch( event )
	{
	case EVENT_ITEM_AUTH:
	case EVENT_ITEM_AUTH_DATA:
		{
			GetMainFrame()->ClosePopup(UPW_RESULT_USEITEM);	
			GetMainFrame()->ClosePopup(UPW_SKILLOPEN_ITEM);
			
			if (EV_SUCCESSED(arg) )
			{
				void* arg1 = &m_item_id;
				void* arg2 = &chara_idx;
				
				GetMainFrame()->OpenPopup(UPW_SKILLOPEN_RESULT, arg1, arg2);
			}
			else
			{
				// ������ �����Ͽ����ϴ�. ��� �� �ٽ� �õ����ּ���.
				/*������ ��� ����*/
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_MESSAGE_FAILED_AUTH"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
			}
		}
		break;
	}
}

void UIPopupSkillOpen::clicked( int idx )
{
	switch(idx)
	{
	case OK:
		{
			if(chara_idx < 0)
			{
				/*����� �� ���� �������Դϴ�.*/ /*������ ��� ����*/
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
				GetMainFrame()->ClosePopup(UPW_SKILLOPEN_ITEM);
				return;
			}

			// ������ �������� Master�̱� ������ ������ ������
			if( ITEM_TYPE_WRAP_COUNT == GET_ITEM_TYPE(m_item_id) )
			{
				use_masterticket();
			}
			else
			{
				use_checkitem();
			}
		}
		break;
	case CANCEL:
		{
			switch( item_query_style )
			{
			case IQS_USE: 
				if( GetMainFrame()->GetCurrentSubPhaseType() == USP_SHOP )
				{
					//�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
						&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
				}
				break;
			case IQS_REMOVE: break;
			}
			
			GetMainFrame()->ClosePopup(UPW_SKILLOPEN_ITEM);
		}
		break;

	default:
		{
			
		}
		break;
	}
}

void UIPopupSkillOpen::use_checkitem()
{
	// - ����
	// �Ⱓ�� �������� ������ ������ ������ �Ⱓ�� �������� �������� �Ⱓ���� �������� �ٲ�� �κ��丮�� ����ȴ�.
	// �Ⱓ�� �������� �ȳ� �޽����� ������ش�.
	T_ItemID ItemID = GetCashItemByGroup( m_item_id);

	UINT8 Type = 0; UINT32 Arg = 0;
	CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM, ItemID, &Type, &Arg);
	if( Arg <= 0)
	{
		// �Ⱓ�� �������ε� �����Ⱓ�� 0�̸� ������ΰ� ���°��̴�.
		// �ٷ� ����ϵ��� �Ѵ�.
		use_item();
		return;
	}

	//	"������� �������Դϴ�.\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
	// ������� �������̸� �ٽ� �ѹ� ���´�.
	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_POPUP_MESSAGE_CAN_NORMAL_ITEM"), &GAME_RCSTRING("STR_POPUP_CAPTION_USE_ITEM"), MAKE_CALLBACK(inner::Reuse), this);
}

void UIPopupSkillOpen::use_item()
{
	ItemConsumer::instance()->use(GetMainFrame(), m_item_id);
}

void UIPopupSkillOpen::use_masterticket()
{
	if( m_item_db_idx == INVALID_WAREDB_INDEX)
	{
		/*����� �� ���� �������Դϴ�.*/ /*������ ��� ����*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
		return;
	}

	//�ߺ� ��� ������ ���������� ���� �˻�.
	usf::ValidUseType result = usf::IsUseableItem(m_item_id);
	if( result != usf::vut_Ok )
	{
		usf::Message_UseResert(result);
		return;
	}

	INT8 chara = static_cast<INT8>(chara_idx);
	INT8 ui8Size = sizeof(INT8);

	GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH_DATA, &m_item_db_idx, &ui8Size, &chara);
}

void UIPopupSkillOpen::SetCharaIdx( const i3::wliteral_range& nickname)
{
	if(nickname.empty())
		return;
	
	// Chara Index �ʱ�ȭ
	chara_idx = -1;
	
	const i3::wliteral_range& str_nick = nickname;

	// �г������� �˻��Ͽ� ���� �г����� �ε����� �����´�.
	for(INT32 i = 0; i < CCharaInfoContext::i()->GetHasCharacterCount(); i++)
	{
		const TTCHAR* chara_nick = CCharaInfoContext::i()->GetNickName(i);
		if( i3::generic_is_equal(str_nick, chara_nick) )
		{
			const CHARA_INFO* chara_info = CCharaInfoContext::i()->GetCharaInfoByList(i);
			chara_idx = chara_info->m_CharaBasicInfo.m_ui8CharaSlotIdx;
			return;
		}
	}
}

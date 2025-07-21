#include "pch.h"
#include "UIPopupMonthAttendance.h"

#include "ItemImageMgr.h"
#include "UIUtil.h"
#include "AttendanceContext.h"
#include "ShopContext.h"
#include "UIShopFunction.h"
#include "UIMainFrame.h"

I3_CLASS_INSTANCE(UIPopupMonthAttendance);

extern "C" {
	int UIPopupMonthAttendance_ClickButton( LuaState * L)
	{
		UIPopupMonthAttendance* p = static_cast<UIPopupMonthAttendance*>(GameUI::GetPopup(UPW_MONTH_ATTENDANCE));
		if( p != nullptr)
			p->ClickButton();
		return 0;
	}

	int UIPopupMonthAttendance_ItemClick( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		UIPopupMonthAttendance* p = static_cast<UIPopupMonthAttendance*>(GameUI::GetPopup(UPW_MONTH_ATTENDANCE));
		if( p != nullptr)
			p->SelectReward(idx);
		return 0;
	}
}
LuaFuncReg UIPopupMonthAttendance_Glue[] = {
	{"Click",		UIPopupMonthAttendance_ClickButton},
	{"ItemClick",	UIPopupMonthAttendance_ItemClick},
	{ nullptr,			nullptr}	
};

UIPopupMonthAttendance::UIPopupMonthAttendance() : m_RewardItemIdx(-1), m_TodayAttendance(-1), m_EventState(0)
, m_ViewItemIdx(-1)
{
	for(INT32 i = 0; i < 2; i++)
	{
		m_pImgCtrl[i] = nullptr;
		m_pRewardText[i] = nullptr;
		m_pRewardIcon[i] = nullptr;
	}

	m_vMonthItemBG.reserve(MONTH_MAX_COUNT);
}

UIPopupMonthAttendance::~UIPopupMonthAttendance()
{

}

void UIPopupMonthAttendance::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate(pParent);

	AddScene("Scene/Popup/PBRe_PopUp_Attandence.i3UIs", UIPopupMonthAttendance_Glue);
}

bool UIPopupMonthAttendance::OnEntranceStart( void * pArg1 /*= nullptr */, void * pArg2 /*= nullptr */ )
{
	if(UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	// �����͸� �����ϱ� ���� �̸� �����մϴ�.
	// -1�� �ϴ� ������ �⼮ üũ�� �ϰ� �Ǹ� AttendanceDay�� �ö󰡼� Complate�Ǹ� 1�� ����
	// ���� ��¥�� ���ɴϴ�.
	m_TodayAttendance = AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceDay;
	if(m_EventState == ATTENDANCE_STATE_COMPLETE)
		m_TodayAttendance--;

	// �̺�Ʈ ������ ����
	_SetEventData();

	return true;
}

void UIPopupMonthAttendance::_InitializeAtLoad( i3UIScene * pScene )
{
	UIPopupBase::_InitializeAtLoad(pScene);

	m_pImgCtrl[0] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "GiveItem1");
	m_pImgCtrl[1] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "GiveItem2");

	m_pRewardText[0] = (i3UIStaticText *) pScene->FindChildByName("GiveItemText1");
	m_pRewardText[1] = (i3UIStaticText *) pScene->FindChildByName("GiveItemText2");

	m_pRewardIcon[0] = (i3UIButtonImageSet *) pScene->FindChildByName("BtnIS_ItemBg_01");
	m_pRewardIcon[1] = (i3UIButtonImageSet *) pScene->FindChildByName("BtnIS_ItemBg_02");

	// ���� ����ϹǷ� �����صӴϴ�.
	m_EventState = AttendanceContext::i()->GetAttendanceUser()->m_ui8EventState;

	_SetStaticText(pScene);
}

void UIPopupMonthAttendance::_ClearAtUnload( i3UIScene * pScene )
{
	for(INT32 i = 0 ; i < 2 ; i++)
	{
		I3_SAFE_RELEASE(m_pImgCtrl[i]);
		m_pRewardText[i] = nullptr;
	}

	INT32 count = m_vMonthItemBG.size();
	for(INT32 i = 0; i < count; i++)
	{
		i3GameNode * parent = m_vMonthItemBG[i]->getParent();
		parent->RemoveChild(m_vMonthItemBG[i]);
		//I3_SAFE_RELEASE(m_vMonthItemBG[i]);
	}
	m_vMonthItemBG.clear();

	m_RewardItemIdx = -1;
	m_ViewItemIdx = -1;

	// �켱 BG���� ���� �մϴ�.
	UIPopupBase::_ClearAtUnload(pScene);
}

void UIPopupMonthAttendance::_SetEventData()
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
	if(pInfo == nullptr) return;

	i3::rc_wstring wstrEventName;
	i3::sprintf(wstrEventName, L"%s", pInfo->m_strEventName);
	SetStaticText("EventName", wstrEventName);

	// �⼮ �̺�Ʈ �Ⱓ
	i3::rc_wstring wstrPeriod;
	SetPeriod(wstrPeriod, pInfo->m_ui32StartDate, pInfo->m_ui32EndDate);
	SetStaticText("EventPeriod", wstrPeriod);

	i3::rc_wstring wstrNextPeriod;
	SetPeriod(wstrNextPeriod, pInfo->m_ui32NextStartDate, pInfo->m_ui32NextEndDate, true);
	SetStaticText("Next_Check", wstrNextPeriod);

	// �޷� ǥ��
	_SetViewMonthItem();

	// ���� ������ ǥ��
	_SetViewTodayItem();

	// ���� ������ ǥ���մϴ�.
	SelectReward(0);
}	

void UIPopupMonthAttendance::_SetStaticText( i3UIScene * pScene )
{
	i3UIStaticText * UIText = nullptr;
	i3UIButton * UIButton = nullptr;

	UIText = (i3UIStaticText *) pScene->FindChildByName("Static_Title");
	if(UIText != nullptr)
	{
		UIText->SetText(GAME_STRING("STR_TBL_UI_ATTENDANCE_CHECK_EVENT"));
	}

	UIText = (i3UIStaticText *) pScene->FindChildByName("Static_CheckItem");
	if(UIText != nullptr)
	{
		UIText->SetText(GAME_STRING("STR_TBL_UI_ATTENDANCE_SELECT_ITEM"));
	}

	UIButton = (i3UIButton *) pScene->FindChildByName("Btn_OK");
	if(UIButton != nullptr)
	{
		if(m_EventState == ATTENDANCE_STATE_COMPLETE)
			UIButton->SetText(GAME_STRING("STBL_IDX_BUTTON_CLOSE"));
		else
			UIButton->SetText(GAME_STRING("STR_TBL_UI_ATTENDANCE_CHECK"));
	}
}

void UIPopupMonthAttendance::_SetViewMonthItem()
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
	if(pInfo == nullptr) return;
	
	// �� ���� goods item�� ���� �մϴ�.
	INT32 count = pInfo->m_ui8AttendanceDay;
	/*while(pInfo->m_ui32GiveGoodsID[count].m_ui8GoodsCount > 0)
	{
		count++;
	}*/

	// ����� ���� �����͸� ��� �մϴ�.
	for(INT32 i = 0; i < count; i++)
	{
		i3::rc_wstring wstrString;
		i3::string controlname;

		// �� �� ��
		i3::sprintf(wstrString, GAME_STRING("STR_TBL_UI_ATTENDANCE_DAY"), i+1);
		i3::sprintf(controlname, "Day%02d", i+1);
		SetStaticText(controlname, wstrString);

		// item ����
		ATTENDANCE_CHOICE_ITEM item_ = pInfo->m_ui32GiveGoodsID[i];
		for(INT32 itemcount = 0; itemcount < (INT32)item_.m_ui8GoodsCount; itemcount++)
		{
			i3::sprintf(controlname, "DayItem%02d_%d", i+1, itemcount+1);
			
			INT32 itemID = CShopContext::i()->ConvertGoodsID(item_.m_ui32GoodsID[itemcount]);
			usf::GetShopItemName(wstrString, itemID);

			SetStaticText(controlname, wstrString);
		}

		// �� ������ �ٿ��� �մϴ�.
		//UINT8 attendanceday = AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceDay;
		INT32 shape_type = -1;

		if( m_TodayAttendance > i) shape_type = CLEAR_ATTENDANCE;
		else if(m_TodayAttendance == i) shape_type = TODAY_ATTENDANCE;
		else shape_type = NEXT_ATTENDANCE;

		_SetViewItemBG(i, shape_type);
	}
}

void UIPopupMonthAttendance::_SetViewTodayItem()
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
	if(pInfo == nullptr) return;

	// ���⼭ �����͸� �����ɴϴ�.
	//UINT8 attendanceday = AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceDay;
	ATTENDANCE_CHOICE_ITEM item_ = pInfo->m_ui32GiveGoodsID[m_TodayAttendance];
	
	UINT8 goodscount = item_.m_ui8GoodsCount;

	// Goods�� 1���� ���� ���
	if(goodscount != 0)
	{
		m_ViewItemIdx = m_TodayAttendance;

		// ���� ������ ����
		__SetTodayItemIcon(item_, m_EventState == ATTENDANCE_STATE_COMPLETE);
	}
	// Goods�� ���� ��� �ڿ� ������ üũ�մϴ�.
	else
	{
		// ���� ����� ������ ���� �� �� Input�� �����ϴ�.
		INT32 i = 0;
		for(i = m_TodayAttendance; i < (INT32)pInfo->m_ui8AttendanceDay; i++)
		{
			ATTENDANCE_CHOICE_ITEM temp = pInfo->m_ui32GiveGoodsID[i];
			
			if(temp.m_ui8GoodsCount != 0)
			{
				item_ = temp;
				break;
			}
		}

		m_ViewItemIdx = i;
		
		// ���� ������ ����
		__SetTodayItemIcon(item_, true);

		// UI Text�� �ٲߴϴ�.
		i3UIStaticText * UIText = (i3UIStaticText *) GetScene(0)->FindChildByName("Static_CheckItem");
		if(UIText != nullptr)
		{
			UIText->SetText(GAME_STRING("STR_TBL_UI_ATTENDANCE_NON_ITEM"));
		}
	}
}

void UIPopupMonthAttendance::__SetTodayItemIcon(ATTENDANCE_CHOICE_ITEM& item_, bool IsviewOnly)
{
	INT32 goodscount = item_.m_ui8GoodsCount;

	// �������� �����մϴ�.
	for(INT32 i = 0; i < goodscount; i++)
	{
		INT32 itemID = CShopContext::i()->ConvertGoodsID(item_.m_ui32GoodsID[i]);

		i3::rc_wstring itemstring;
		usf::GetShopItemName(itemstring, itemID);
		m_pRewardText[i]->SetText(itemstring);

		GetItemImageMgr()->SelectImage(m_pImgCtrl[i], itemID);

		if(IsviewOnly)
		{
			m_pRewardIcon[i]->setInputDisable(true);
			m_pRewardIcon[i]->addStyle(I3UI_STYLE_IGNOREINPUT);
			m_pRewardIcon[i]->SetViewOnly(true);
		}
	}

	// 1�� �� ���
	if( goodscount < 2)
	{
		// �����׸� ����
		m_pRewardIcon[1]->EnableCtrl(false);

		VEC3D * pos_ = m_pRewardIcon[0]->getPos();
		VEC3D * pos2_ = m_pRewardIcon[1]->getPos();
		REAL32 xpos = (pos2_->x - pos_->x) / 2.f;
		pos_->x += xpos;
		m_pRewardIcon[0]->setPos(pos_);
	}
}

void UIPopupMonthAttendance::_SetViewItemBG( INT32 idx, INT32 shape_type )
{
	if( shape_type == -1) return;

	i3::string controlname;
	i3::string bgControl;
	i3::sprintf(controlname, "DayFrame%02d", idx+1);

	i3UIFrameWnd * pWnd = (i3UIFrameWnd * )GetScene(0)->FindChildByName(controlname.c_str());
	if(pWnd == nullptr) return;

	// Shape Template ã��
	switch(shape_type)
	{
	case CLEAR_ATTENDANCE:
		i3::sprintf(bgControl, "V3_BtnIS_DayCheck_02");
		i3::sprintf(controlname, "V3_BtnIS_DayCheck_03");
		break;
	case TODAY_ATTENDANCE:
		if(m_EventState == ATTENDANCE_STATE_COMPLETE)
		{
			i3::sprintf(bgControl, "V3_BtnIS_DayCheck_02");
			i3::sprintf(controlname, "V3_BtnIS_DayCheck_03");
		}
		else
		{
			i3::sprintf(controlname, "V3_BtnIS_DayCheck_02");
		}
		break;
	case NEXT_ATTENDANCE:
		i3::sprintf(controlname, "V3_BtnIS_DayCheck_02");
		break;
	}

	// ��ġ �̵� / ũ�� ����
	VEC3D wndPos;
	wndPos.Set(0.0f, 0.0f, 0.0f);
	VEC2D * wndSize = pWnd->getSize();

	if(bgControl.empty() == false)
	{
		i3UITemplate* BGTemplate = i3UI::getUILibrary()->FindByName(bgControl.c_str());
		i3UIButtonImageSet * UIButton = (i3UIButtonImageSet*)i3UI::CreateControl(BGTemplate, &wndPos, wndSize, pWnd);

		m_vMonthItemBG.push_back(UIButton);
		UIButton->SetShapeIdx(1);
		UIButton->SetViewOnly(true);
	}

	i3UITemplate* BGTemplate = i3UI::getUILibrary()->FindByName(controlname.c_str());
	i3UIButtonImageSet * UIButton = (i3UIButtonImageSet*)i3UI::CreateControl(BGTemplate, &wndPos, wndSize, pWnd);

	m_vMonthItemBG.push_back(UIButton);
	
	// Shape ����
	if(shape_type != TODAY_ATTENDANCE || m_EventState == ATTENDANCE_STATE_COMPLETE)
	{
		UIButton->SetShapeIdx(1);
		UIButton->SetViewOnly(true);
	}
	else
	{
		// Title�� �� ���� �ø��ϴ�.
		// ������ �ٲ� �ٽ� �����մϴ�.
		i3::sprintf(controlname, "Day%02d", idx + 1);
		i3GameNode * title_text = pWnd->FindChildByName(controlname.c_str());
		i3GameNode * uiParent = title_text->getParent();
		if(uiParent != nullptr)
			title_text->ChangeParent(uiParent);
	}
}

void UIPopupMonthAttendance::ClickButton()
{
	// Click �̺�Ʈ
	if(m_EventState == ATTENDANCE_STATE_COMPLETE)
	{	
		// ���� �˴ϴ�.
		OnKey_Escape();
	}
	else
	{
		const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
		GameEventSender::i()->SetEvent(EVENT_ATTENDANCE_CHECK, &pInfo->m_ui32Version, &pInfo->m_ui8AttendanceType);
	}
}

void UIPopupMonthAttendance::SelectReward( INT32 idx )
{
	if(idx != m_RewardItemIdx && m_TodayAttendance == m_ViewItemIdx)
	{
		if(-1 < idx && idx < 2)
		{
			if( m_RewardItemIdx == -1 ) m_RewardItemIdx = 0;

			m_pRewardIcon[m_RewardItemIdx]->SetShapeIdx(0);
			m_pRewardIcon[idx]->SetShapeIdx(1);

			m_RewardItemIdx = idx;
		}
	}
	else if(m_RewardItemIdx == -1)
	{
		// �� ���� ���� �� �� ��, Complate�� ����̴�.
		// ���⼭ ���� ������ �ε����� ������ �˴ϴ�.
		if( m_TodayAttendance == m_ViewItemIdx)
		{
			m_RewardItemIdx = AttendanceContext::i()->GetAttendanceUser()->m_ui8ItemNum;

			if(m_RewardItemIdx != -1)
			{
				m_pRewardIcon[m_RewardItemIdx]->SetShapeIdx(1);
			}
		}
	}
}

void UIPopupMonthAttendance::ReceiveGameEvent_Attendance_Check( INT32 arg, const i3::user_data &UserData )
{
	switch( arg)
	{
		// ����
	case EVENT_ERROR_ATTENDANCE_SUCCESS:
		_SetSuccessUI();
		_SetSuccessEvent();
		break;
	default:
		SetErrorMsg(arg);
		break;
	}
}

void UIPopupMonthAttendance::_SetSuccessUI()
{
	// Check ����
	// �������� ���� �ȵǰ� ���޵Ǿ ���� ����
	AttendanceContext::i()->SetAttendanceState(ATTENDANCE_STATE_COMPLETE);
	m_EventState = ATTENDANCE_STATE_COMPLETE;

	// �����غ��� ������ �� �ϰ� ������?
	//// �޷��� �����ϱ� �� BG�� ����ϴ�.
	//INT32 count = m_vMonthItemBG.size();
	//for(INT32 i = 0; i < count; i++)
	//{
	//	i3GameNode * parent = m_vMonthItemBG[i]->getParent();
	//	parent->RemoveChild(m_vMonthItemBG[i]);
	//	I3_SAFE_RELEASE(m_vMonthItemBG[i]);
	//}
	//m_vMonthItemBG.clear();
	//
	//// �޷� ����
	//_SetViewMonthItem();

	//// ��ư �ؽ�Ʈ ����
	//i3UIButton * ok_btn = (i3UIButton*)GetScene(0)->FindChildByName("Btn_OK");
	//if(ok_btn != nullptr)
	//{
	//	ok_btn->SetText(GAME_STRING("STBL_IDX_BUTTON_CLOSE"));
	//}
}



void UIPopupMonthAttendance::_SetSuccessEvent()
{	
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
	if( pInfo == nullptr) return;


	INT32 nCurSel = 0;

	ATTENDANCE_CHOICE_ITEM item_ = pInfo->m_ui32GiveGoodsID[m_TodayAttendance];

	if( item_.m_ui8GoodsCount > 1)
	{
		// ���õ� �� �����մϴ�.
		nCurSel = m_RewardItemIdx;
	}

	// Item Count�� 0�� �ƴҰ�� �� ���� ���� �� �ִ� �������� ���� ��쿡�� �����ϴ�.
	if( item_.m_ui8GoodsCount != 0)
	{
		GameEventSender::i()->SetEvent(EVENT_ATTENDANCE_CLEAR, pInfo, &nCurSel);
		AttendanceContext::i()->SetAttendanceSelectItem((UINT8)nCurSel);
	}
	else
	{
		// ���� ���� �� �ִ� �������� ���� ��� �޽��� �ڽ��� ���� �����ϴ�.
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_UI_ATTENDANCE_CHECK_SUCCESS"));
	}

	// �˾� ����
	GetMainFrame()->ClosePopup(UPW_MONTH_ATTENDANCE);
}

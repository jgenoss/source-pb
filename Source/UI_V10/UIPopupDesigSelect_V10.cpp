#include "pch.h"
#include "UIPopupDesigSelect_V10.h"

#include "../UI/UIMainFrame_V10.h"

#include "Designation/DesigUtil.h"
#include "Designation/UserDesigInfo.h"
#include "Designation/DesigAttr.h"
#include "Designation/DesigSet.h"

#include "UIPhaseProfile_V10.h"
#include "UITabProfileDesignation_V10.h"

static UIPopupDesigSelect_V10 * GetThis(void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	return (UIPopupDesigSelect_V10*)pFrame->GetPopup(UPW_DESIG_SELECT);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupDesigSelect_V10_OK(LuaState * L)
	{
		GetThis()->ClickOK();
		return 0;
	}

	int UIPopupDesigSelect_V10_Select(LuaState *L)
	{
		INT32 Slot = i3Lua::GetIntegerArg(L, 1); 
		GetThis()->SelectIcon(Slot);
		return 0;
	}
}

LuaFuncReg UIPopupDesigSelect_V10_Glue[] =
{
	{ "ClickOK",		UIPopupDesigSelect_V10_OK },
	{ "SelectIcon",		UIPopupDesigSelect_V10_Select },
	{ nullptr,				nullptr }
};

I3_CLASS_INSTANCE(UIPopupDesigSelect_V10);

UIPopupDesigSelect_V10::UIPopupDesigSelect_V10()
{
	m_SelectedSlot = -1;
}

UIPopupDesigSelect_V10::~UIPopupDesigSelect_V10()
{

}

/*virtual*/ void UIPopupDesigSelect_V10::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	char strName[MAX_STRING_COUNT] = { 0, };
	for (int i = 0; i < MAX_EQUIP_USER_TITLE; ++i)
	{
		i3::snprintf(strName, MAX_STRING_COUNT, "btnIcon0%d", i);
		m_pBtnIcon[i] = static_cast<i3UIButtonImageSet*>(pScene->FindChildByName(strName));
		i3::snprintf(strName, MAX_STRING_COUNT, "TextTitle0%d", i);
		m_pTitle[i] = static_cast<i3UIStaticText*>(pScene->FindChildByName(strName));
		i3::snprintf(strName, MAX_STRING_COUNT, "ImgLock0%d", i);
		m_pImgLock[i] = (static_cast<i3UIButtonImageSet*>(pScene->FindChildByName(strName)));
		i3::snprintf(strName, MAX_STRING_COUNT, "FmwSelected0%d", i);
		m_pFmwSelected[i] = static_cast<i3UIFrameWnd*>(pScene->FindChildByName(strName));
	}
}

/*virtual*/ void UIPopupDesigSelect_V10::_ClearAtUnload(i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload(pScene);
}

/*virtual*/ void UIPopupDesigSelect_V10::OnCreate(i3GameNode * pParent)
{
	UIPopupBase::OnCreate(pParent);

	// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_MyInfomation_Designation.i3UIs", UIPopupDesigSelect_V10_Glue);
}

/*virtual*/ bool UIPopupDesigSelect_V10::OnEntranceStart(void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if (UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	// �ʱ�ȭ
	InitUI();

	m_SelectedSlot = -1;

	return true;
}

void UIPopupDesigSelect_V10::InitUI()
{
	for (int i = 0; i < MAX_EQUIP_USER_TITLE; ++i)
	{
		// �ʱ�ȭ
		m_pImgLock[i]->EnableCtrl(true);
		m_pBtnIcon[i]->EnableCtrl(false);
		m_pTitle[i]->EnableCtrl(false);
		m_pFmwSelected[i]->EnableCtrl(false);

		// �� ����
		SetSlotInfo(i);
	}
}

void UIPopupDesigSelect_V10::SetSlotInfo(INT32 SlotIdx)
{
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

	// �ڽ��� ���� �� �ִ� �ִ� ���Ժ��� ũ�� �������� ����
	INT32 LimitCount =  myDesigInfo->GetLimitCount(); 
	if (SlotIdx >= LimitCount)
		return;

	// ����� �� �ִ� �����̹Ƿ� Lock Icon�� ����
	m_pImgLock[SlotIdx]->EnableCtrl(false);

	UINT8 myEquipDesigSet = myDesigInfo->GetEquipDesig(SlotIdx);
	const DesigSet* pDesigSet = DesigUtil::FindDesigSetByType(myEquipDesigSet);

	// ������ ȣĪ�� ������ �������� ����
	if (pDesigSet == nullptr)
		return;

	m_pBtnIcon[SlotIdx]->EnableCtrl(true);
	m_pTitle[SlotIdx]->EnableCtrl(true);

	LuaState* L = _CallLuaFunction("SetSlot");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, SlotIdx);								// ���� �ε���
		i3Lua::PushInteger(L, pDesigSet->desigSetUIIndex);			// ������ UI Shape Index
		i3Lua::PushStringUTF16To8(L, pDesigSet->wstrName.c_str());	// �̸�
		_EndLuaFunction(L, 3);
	}
}

void UIPopupDesigSelect_V10::ProcessGameEvent(GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
}

/*virtual*/ bool UIPopupDesigSelect_V10::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	return UIPopupBase::OnEvent(event, pObj, param2, code);
}

void UIPopupDesigSelect_V10::ClickOK()
{
	if ( m_SelectedSlot != -1 )
	{
		UIPhaseProfile_V10* pSubPhase = ((UIPhaseProfile_V10*)(GetMainFrame()->GetSubPhase(USP_PROFILE)));
		UITabProfileDesignation_V10* pTab = ((UIPhaseProfile_V10*)pSubPhase)->GetDesignation();
		I3ASSERT(i3::same_of<UITabProfileDesignation_V10*>(pTab));

		pTab->__OnDesignationChange(m_SelectedSlot);
		GetMainFrame()->ClosePopup(UPW_DESIG_SELECT);
	}
	else
	{
		/* ������ �������� �ʾҽ��ϴ�. ������ ������ �� �ٽ� Ȯ�� ��ư�� �����ֽñ� �ٶ��ϴ� */
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_DESIGNATION_NO_SELECTED_SLOT_MESSAGE"));
	}
}

void UIPopupDesigSelect_V10::SelectIcon(INT32 Slot)
{
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();
	INT32 LimitCount = myDesigInfo->GetLimitCount();

	if (Slot < LimitCount)
	{
		m_SelectedSlot = Slot;

		// ���õ� ������ Selected ���·� �ٲߴϴ�
		for (int i = 0; i < MAX_EQUIP_USER_TITLE; ++i)
		{
			bool visible = false;
			if (m_SelectedSlot == i)
				visible = true;

			m_pFmwSelected[i]->EnableCtrl(visible);
		}
	}
	else  // �������� ���� ������ �����ϸ� Select ó���� �ʱ�ȭ�մϴ�
	{	
		m_SelectedSlot = -1;
		for (int i = 0; i < MAX_EQUIP_USER_TITLE; ++i)
		{
			m_pFmwSelected[i]->EnableCtrl(false);
		}
	}
}
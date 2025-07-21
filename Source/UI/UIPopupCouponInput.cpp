#include "pch.h"
#include "UIPopupCouponInput.h"
#include "../CommunityContext.h"
#include "UIMainFrame.h"

#include "UIUtil.h"

#include "i3Base/string/compare/generic_is_iequal.h"


I3_CLASS_INSTANCE( UIPopupCouponInput);//, UIPopupBase);

bool CheckLength(const char *t1)
{
	if(i3::generic_string_size(t1) > GIFTCOUPON_LENGTH)
		return false;

	return true;
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIPopupCouponInput_Confirm(LuaState * L)
	{
		UIPopupCouponInput * pThis = static_cast<UIPopupCouponInput*>(GameUI::GetPopup(UPW_COUPON_INPUT));
		if( pThis != nullptr)
			pThis->Confirm();
		return 0;
	}

	int UIPopupCouponInput_Cancel(LuaState * L)
	{
		UIPopupCouponInput * pThis = static_cast<UIPopupCouponInput*>(GameUI::GetPopup(UPW_COUPON_INPUT));
		if( pThis != nullptr)
			pThis->Cancel();
		return 0;
	}
}

LuaFuncReg UIPopupCouponInput_Glue[] = {
	{"Confirm",					UIPopupCouponInput_Confirm	},
	{"Cancel",					UIPopupCouponInput_Cancel	},
	{nullptr,						nullptr						}
};

//------------------------------------------------------------------------------//


UIPopupCouponInput::UIPopupCouponInput()
{
	m_bWaitingConfirmSubscripting = false;
	m_nOldLength = 0;
	m_pCouponBox = nullptr;
	i3mem::FillZero(m_szCurEditText, COUPON_MAX_LETTER_COUNT);
}

UIPopupCouponInput::~UIPopupCouponInput()
{
}

/*virtual*/ void UIPopupCouponInput::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene("Scene/Popup/PBRe_PopUp_InputCoupon.i3UIs",UIPopupCouponInput_Glue);
}

/*virtual*/ bool UIPopupCouponInput::OnEntranceStart(void * pArg1 , void * pArg2 )
{
	if( UIPopupBase::OnEntranceStart(pArg1,pArg2) == false)
		return false;

	m_bWaitingConfirmSubscripting = false;
	m_nOldLength = 0;

	return true;
}

/*virtual*/ void UIPopupCouponInput::OnEntranceEnd( void)
{
	UIPopupBase::OnEntranceEnd();

	if(m_pCouponBox)
	{
		m_pCouponBox->SetInputCharOnlyAlphaNumeric();  // ���������Է¸��
		m_pCouponBox->SetInputAlphabetAutoCase(1); // �빮���Է¸��
	}

	LuaState * L = _CallLuaFunction("GetFirstEditBox");

	if(L != nullptr)
	{
		i3UIEditBox * pFirstEdit = reinterpret_cast<i3UIEditBox*>(UILua::ReturnLuaPointer(L , 0));
		g_pFramework->SetFocus(pFirstEdit);
	}
}

/*virtual*/ void UIPopupCouponInput::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	m_pCouponBox = (i3UIEditBox *) pScene->FindChildByName("i3UIEditInput1");
	m_pCouponBox->setAlphaNumericOnly(true);
}

/*virtual*/ bool UIPopupCouponInput::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	UIPopupBase::OnEvent( event, pObj, param2, code);

	return false;
}

void UIPopupCouponInput::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	if( evt == EVENT_COUPON_AWARD)
	{
		ReceiveGameEvent_Coupon_Award( arg, UserData);
	}
}

void UIPopupCouponInput::Confirm()
{
	LuaState* L;

	//���ĺ��� ������ ���ڰ� ���ۿ� ������ ������ ���� �� �ִ�.
	L = UILua::CallLuaFunction(GetScene(0), "GetInputText1");
	const char* _text1 = UILua::ReturnLuaString(L, 0);

	if( CheckLength(_text1) == false )
	{
		UIPopupCouponInput * pThis = static_cast<UIPopupCouponInput*>(GameUI::GetPopup(UPW_COUPON_INPUT));
		if( pThis != nullptr)
			pThis->NotifyFail(EVENT_ERROR_FAIL);
		return;
	}

	char Coupon_Num[GIFTCOUPON_LENGTH + 1] = {0};
	i3::generic_string_copy( Coupon_Num, _text1 );

	if( m_bWaitingConfirmSubscripting == true)
		return;

	GameEventSender::i()->SetEvent(EVENT_COUPON_AWARD, (void *)Coupon_Num);
	m_bWaitingConfirmSubscripting = true;
}

void UIPopupCouponInput::NotifyFail(INT32 Arg)
{
	switch( Arg ) {
		case EVENT_FAIL_COUPON_AWARD_ITEM_OF_BILLING_PROBLEM :
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_COUPON_INPUT_FAIL")); break;		// ���� ������ ������ �ֽ��ϴ�.
		case EVENT_ERROR_NETWORK :			
		case EVENT_ERROR_COUPON_WRONG_NUMBER :
		case EVENT_ERROR_COUPON_ALREADY_USED :
		case EVENT_ERROR_COUPON_EXPIRED :
		default:
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_COUPON_INPUT_FAIL")); break;
	}
	GetMainFrame()->TogglePopup(UPW_COUPON_INPUT);
}

void UIPopupCouponInput::NotifySuccess()
{
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_COUPON_INPUT_SUCCESS"));
	GetMainFrame()->TogglePopup(UPW_COUPON_INPUT);
}

void UIPopupCouponInput::Cancel()
{
	GetMainFrame()->TogglePopup(UPW_COUPON_INPUT);
}

void UIPopupCouponInput::AutoFocusing(UINT32 nEditState)
{
	i3UIEditBox * pCurEdit = reinterpret_cast<i3UIEditBox*>(i3UI::getGlobalIme()->getOwner());

	if(pCurEdit == nullptr)
		return;

	if( nEditState == EDIT_STATE_CHANGE_INPUT_CHAR)
	{	
		WCHAR szText[MAX_STRING_COUNT] = {0};
		i3::string_ncopy_nullpad(szText, pCurEdit->getUIText()->getText(), MAX_STRING_COUNT);
		INT32 nLength = i3::generic_string_size(szText);
		
		i3UIEditBox * pNextEdit = nullptr;
		bool bNextFocus = true;

		// ���� ���ڸ� �Ѱ� �Է����� ��� ó���Ѵ�.
		// IME�� �ӽ÷� ������ �ɾ��ְ� �Էµ� ���� ���۸� Ŭ���� �� �缳��
		if(nLength > COUPON_MAX_LETTER_COUNT)
		{	
			nLength = COUPON_MAX_LETTER_COUNT;

			i3UI::getGlobalIme()->setLimit(nLength);
			i3UI::getGlobalIme()->SetInputText(m_szCurEditText, nLength);

			// KillFocus�� ȣ��Ǿ��, EditBox�� text�� IME�� �Էµ� ���ڿ���
			// ����Ǳ� ������ KillFocus�� �ٽ� SetFocus�� ���ش�.
			pCurEdit->OnKillFocus(pCurEdit);
			pCurEdit->OnSetFocus();
		}

		bool bMoveToPrevEdit = i3::generic_is_iequal(m_szCurEditText, szText);
		//	i3String::NCompareW(m_szCurEditText, szText, MAX_STRING_COUNT) == 0;

		if(!bMoveToPrevEdit)
		{
			// �׳� �ǳ� ��� ���̽� 
			// 1. [���� �Է�] ���� ���̰� MAX_LETTER_COUNT ���� 0 �̻�,
			//		- 5���� ������ ��Ŀ�� �̵� ���� �ԷµǾ���Ѵ�.
			// 2. [�齺���̽� �Է�] ���ڱ��� = 0, ���� ���ڱ��� 1�� �̻� 
			//		- ���� ���ڵ� 0�� ��쿡�� �� EditBox�� �̵��ȴ�.
			if((nLength < COUPON_MAX_LETTER_COUNT && nLength > 0) || (nLength == 0 && m_nOldLength > 0))
			{
				i3::string_ncopy_nullpad(m_szCurEditText, szText, COUPON_MAX_LETTER_COUNT);
				m_nOldLength = nLength;					
				return;
			}
		}

		// ���� ����Ʈ�ڽ��� �̵� (�齺���̽� �Է�)
		// 1. ���� ����Ʈ�ڽ��� ���ڰ� �ϳ��� ���� ����.
		// 2. ĳ���� ����Ʈ�ڽ� �� �տ� �ִ� ���.
		if((nLength == 0 && m_nOldLength == 0) || bMoveToPrevEdit)
		{
			bNextFocus = false;
		}

		// Ű �Է½� Limit�� �ʰ��ϸ� ���� ��Ŀ���� �̵��Ѵ�.
		LuaState * L = _CallLuaFunction("ChangeFocusingToAuto");
		if( L != nullptr)
		{
			i3Lua::PushUserData(L , pCurEdit);
			i3Lua::PushBoolean(L , bNextFocus);
			pNextEdit = reinterpret_cast<i3UIEditBox*>(_ReturnLuaPointer(L , 2));
		}	

		i3::string_ncopy_nullpad(m_szCurEditText, szText, COUPON_MAX_LETTER_COUNT);
		m_nOldLength = nLength;		

		if(pNextEdit != nullptr && pNextEdit != pCurEdit)
		{
			g_pFramework->SetFocus(pNextEdit);
			m_nOldLength = i3::generic_string_size(pNextEdit->getUIText()->getText());
		}
	}
}

void UIPopupCouponInput::ReceiveGameEvent_Coupon_Award( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED( arg ) )
	{
		NotifySuccess();
	}
	else
	{
		NotifyFail( arg);				
	}

	m_bWaitingConfirmSubscripting = false;
}
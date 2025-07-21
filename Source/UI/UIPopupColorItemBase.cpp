#include "pch.h"
#include "UIPopupColorItemBase.h"

#include "UIUtil.h"
#include "ItemConsumer.h"

#define SELECT_CROSSHAIR_COLOR_BOX_COUNT		10	//	���� ���� ���� ����

//I3_ABSTRACT_CLASS_INSTANCE( UIPopupColorItemBase, UIPopupUseItemBase);
I3_CLASS_INSTANCE( UIPopupColorItemBase);

UIPopupColorItemBase::UIPopupColorItemBase()
{
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
}

UIPopupColorItemBase::~UIPopupColorItemBase()
{
}

/*virtual*/ void UIPopupColorItemBase::OnEntranceEnd( void)
{
	UIPopupUseItemBase::OnEntranceEnd();
	ItemConsumer::instance()->m_nSelected = -1;
}


/*virtual*/ bool UIPopupColorItemBase::OnOKButton( void)
{
	if( ItemConsumer::instance()->m_nSelected == -1)
	{
		/*������ ������ ������ �ֽʽÿ�.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));
	}
	else
	{
		SetInputDisable( true);

		UINT8 ui8Size = sizeof(INT32);
		INT32 idx = ItemConsumer::instance()->m_nSelected + 1;	//	GUI�� -1 ���̽��̸�, ������ 0 ���̽��̴�. �����Ұ�!
		GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &idx);
	}

	// ���� event�� ������ �ݴ´�.
	return false;
}


I3COLOR * UIPopupColorItemBase::GetColor( INT32 idx)
{
	I3ASSERT( idx >= -1 && idx < SELECT_CROSSHAIR_COLOR_BOX_COUNT);	//	-1���� ���, 0~9������ ����

	I3COLOR * pSelectColor = nullptr;

	ItemConsumer::instance()->m_nSelected = idx;

	if( idx == -1)
	{
		//	���õ��� ���� ���� �Ǵ� �⺻ ����	�⺻ �÷�
		pSelectColor = (I3COLOR*)GetColorCashItem(0);
	}
	else
	{
		//	���õ� �÷� �ڽ��� ��ġ�� ���� Ŀ���� �����.
		INT32 idxColor = idx + 1;	//	idxColor�� 1 base�̴�.

		//	���� �÷�
		pSelectColor = (I3COLOR*)GetColorCashItem((UINT8)idxColor);
	}

	return pSelectColor;
}

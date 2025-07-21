#include "pch.h"
#include "GlobalVariables.h"
#include "PopupDeleteItem.h"

#if !defined( ENABLE_UI2)



struct DeleteCashItemMessage
{
	char * _pszResultStatic;	//	DELETE_ITEM_COMPLETE '��� ������'
	char * _pszResultValue;		//	DELETE_ITEM_COMPLETE '��� ���'
	char * _pszResultMessage;	//	DELETE_ITEM_COMPLETE '�ȳ� �޽���'
};

I3_CLASS_INSTANCE(CPopupDeleteItem, CPopupBase);

CPopupDeleteItem::CPopupDeleteItem(void)
{
	m_ItemID = 0;
	m_AuthWareDBIndex = 0;
	m_Type = DELETE_ITEM_UNKNOWN;

	m_Style = DELETE_ITEM_STYLE_NORMAL;
	m_pStringFilter = NULL;
	m_pStringFilter2 = NULL;


	m_pRoot = NULL;

	m_pQueryRoot = NULL;
	m_pQueryCaption = NULL;
	m_pQueryButtonOk = NULL;
	m_pQueryButtonCancel = NULL;
	m_pQueryItemImage = NULL;
	m_pQueryItemStatic = NULL;
	m_pQueryItemValue = NULL;
	m_pQueryAttrStatic = NULL;
	m_pQueryAttrValue = NULL;
	m_pQueryNotiveValue = NULL;
	m_pQueryStatic = NULL;
	m_pQueryStatic2 = NULL;

	m_nSelected = 0;
	memset( m_szChangeName, 0, sizeof( m_szChangeName ) );
}

CPopupDeleteItem::~CPopupDeleteItem(void)
{
	I3_SAFE_RELEASE(m_pStringFilter);
	I3_SAFE_RELEASE(m_pStringFilter2);
}

BOOL CPopupDeleteItem::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, "DeleteItem", m_pRoot, -1);

	//	�����ۻ�� ���� �˾�
	{
		LinkControlEx(plist, "deleteItemQueryPopup", m_pQueryRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "deleteItemQueryPopupCaption", m_pQueryCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "deleteItemQueryPopupOk", m_pQueryButtonOk, GCI_DELETE_ITEM_B_QUERY_OK);
		LinkControlEx(plist, "deleteItemQueryPopupCancel", m_pQueryButtonCancel, GCI_DELETE_ITEM_B_QUERY_CANCEL);
		LinkControlEx(plist, "deleteItemQueryPopupItemImage", m_pQueryItemImage, GCI_UNKNOWN);
		LinkControlEx(plist, "deleteItemQueryPopupItemStatic", m_pQueryItemStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "deleteItemQueryPopupItemValue", m_pQueryItemValue, GCI_UNKNOWN);
		LinkControlEx(plist, "deleteItemQueryPopupAttrStatic", m_pQueryAttrStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "deleteItemQueryPopupAttrValue", m_pQueryAttrValue, GCI_UNKNOWN);
		LinkControlEx(plist, "deleteItemQueryPopupNoticeValue", m_pQueryNotiveValue, GCI_UNKNOWN);
		LinkControlEx(plist, "deleteItemQueryPopupStatic", m_pQueryStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "deleteItemQueryPopupStatic2", m_pQueryStatic2, GCI_UNKNOWN);
	}

	return FALSE;
}

void CPopupDeleteItem::OnInitControl(void)
{
	//	�����ۻ�� �˾�
	{
		INIT_WIN_CAPTION( m_pQueryCaption, GAME_STRING("STR_POPUP_CAPTION_DELETE_ITEM"));/*������ ����*/

		INIT_CAPTION_BUTTON(m_pQueryButtonOk, GAME_STRING("STR_POPUP_OK"));			//	"Ȯ��"
		INIT_CAPTION_BUTTON(m_pQueryButtonCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"���"

		INIT_STATIC_CAPTION(m_pQueryItemStatic, GAME_STRING("STR_POPUP_DELETE_ITEM"));/*���� ������:*/
		INIT_VALUE_CAPTION(m_pQueryItemValue, "");

		INIT_STATIC_CAPTION(m_pQueryAttrStatic, GAME_STRING("STR_POPUP_TERM_DELETE_ITEM"));/*�����Ⱓ:*/
		INIT_VALUE_CAPTION(m_pQueryAttrValue, "");

		INIT_VALUE_CAPTION(m_pQueryNotiveValue, "");
		m_pQueryNotiveValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pQueryNotiveValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_pQueryNotiveValue->SetTextSpace(0, 2);
		m_pQueryNotiveValue->SetTextAutoWrap(TRUE);

		INIT_WIN_CAPTION(m_pQueryStatic, GAME_STRING("STR_POPUP_Q_MESSAGE_WANT_DELETE_ITEM"));/*�������� ��� �Ͻðڽ��ϱ�?*/
		INIT_WIN_CAPTION(m_pQueryStatic2, GAME_STRING("STR_POPUP_Q_MESSAGE_WARNING_DELETE_ITEM"));/*�������� ��� �Ͻðڽ��ϱ�?*/

		m_pQueryRoot->SetEnable(FALSE);
	}

}

void CPopupDeleteItem::OnGameEvent( INT32 event, INT32 arg)
{

}

void CPopupDeleteItem::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
		//	������ ���
	case GCI_DELETE_ITEM_B_QUERY_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	_QueryOK();
		break;
	case GCI_DELETE_ITEM_B_QUERY_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnCancel();
		break;
	case GCI_DELETE_ITEM_B_DELETE_ITEM_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnCancel();
		break;
	case GCI_DELETE_ITEM_B_CONFIRM_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnClosePopup();
		break;
	}
}

void CPopupDeleteItem::_QueryOK(void)
{
	//	"�������� �����ϸ� ���� ���� ȿ���� �Բ� ������ϴ�.\n�������� �����Ͻðڽ��ϱ�?"
	STRING_POPUP_Q(GAME_STRING("STR_INVEN_MESSAGE_DELETE_ITEM"), MAKE_CALLBACK(CbQueryOk), this);
}

void CPopupDeleteItem::OnOpenPopup(void * pArg1, void * pArg2)
{
	m_ItemID = 0;
	m_AuthWareDBIndex = 0;

	m_pRoot->setInputDisable( FALSE);

	if(pArg1 == NULL)
		return;


	m_ItemID = *(INT32*)pArg1;
	if( m_ItemID <= 0)
	{
		STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_CANNOT_DELETE_ITEM_1"));/*�������� ����� �� �����ϴ�.\n�߸��� �������Դϴ�.*/
		return;
	}


	if(pArg2 != NULL)
	{
		m_Style = *(DELETE_ITEM_STYLE*)pArg2;
	}
	else
	{
		m_Style = DELETE_ITEM_STYLE_NORMAL;
	}

	if(m_Style == DELETE_ITEM_STYLE_RESULT)
	{
		CPopupBase::OnOpenPopup( pArg1, pArg2);		//	enable

		//	������ ��� ��� â
		_Open(DELETE_ITEM_COMPLETE);
	}
	else
	{
		CPopupBase::OnOpenPopup( pArg1, pArg2);		//	enable

		//	������ ó���� �������� ������ ����� ���� �˾��� ����.
		if( !_Open(DELETE_ITEM_QUERY))
			OnExitKey();
	}
}

void CPopupDeleteItem::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();

	m_pRoot->SetEnable(FALSE);

	I3_SAFE_RELEASE(m_pStringFilter);
	I3_SAFE_RELEASE(m_pStringFilter2);

	m_ItemID = 0;
}

void CPopupDeleteItem::OnCancel(void)
{
	OnClosePopup();
}

void CPopupDeleteItem::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);
}

void CPopupDeleteItem::OnPreGuiNotify(void)
{

}

void CPopupDeleteItem::OnExitKey(void)
{

	OnClosePopup();
	m_pQueryRoot->SetEnable(FALSE);

}


BOOL CPopupDeleteItem::_Open( DELETE_ITEM_POPUP_TYPE type)
{
	// ��뿩��
	I3ASSERT( g_pFramework != NULL);
	I3ASSERT( g_pFramework->GetInvenList() != NULL);

	m_Type = type;
	m_pRoot->SetEnable(TRUE);
	_SetEnablePopup(m_Type);
	PlaySound(GUI_SND_POPUP_OPEN);

	switch( type)
	{
	case DELETE_ITEM_QUERY:
		{
			_InitQueryPopup();

			m_nSelected = -1;
		}
		break;
	default:
		{
			return FALSE;
		}
		break;
	}

	return TRUE;
}

void CPopupDeleteItem::_SetInputLock(BOOL bValue)
{
	m_pRoot->setInputDisable( bValue);
	i3GuiRoot::setGuiDisableInput( bValue);

	((CStageReady*)m_pParent)->SetMenuBarInputDisable( bValue);
}

void CPopupDeleteItem::_InitQueryPopup(void)
{
	m_pQueryRoot->SetEnable(TRUE);

	//	������ �̹���
	g_pFramework->SetCashItemShape(m_pQueryItemImage, m_ItemID);

	// �̸�,�Ӽ�,���� ���
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(m_ItemID);
	if( pInfo == NULL)
	{
		m_pQueryNotiveValue->SetText(INVALID_TEXT);
		m_pQueryAttrValue->SetText(INVALID_TEXT);
		m_pQueryItemValue->SetText(INVALID_TEXT);
	}

	char Name[MAX_STRING_COUNT] = {};
	g_pShopItemInfoDataBase->GetItemName(m_ItemID, Name, MAX_STRING_COUNT);

	//	�Ӽ� :
	if ( GET_ITEM_FLAG_TYPE(m_ItemID) != ITEM_TYPE_POINT )
	{
		char szRemainTeam[MAX_STRING_COUNT] = INVALID_TEXT;
		_GetInvenUseDay(m_ItemID, szRemainTeam);
		m_pQueryAttrValue->SetText(szRemainTeam);
		m_pQueryAttrValue->SetEnable(TRUE);
		m_pQueryAttrStatic->SetEnable(TRUE);
	}
	else
	{
		m_pQueryAttrStatic->SetEnable(FALSE);
		m_pQueryAttrValue->SetEnable(FALSE);
	}

	m_pQueryItemValue->SetText(Name);

	if( pInfo != NULL)
		m_pQueryNotiveValue->SetText(pInfo->GetDescription());	// ����
}

void CPopupDeleteItem::_InitCompletePopup(void)
{

}




void CPopupDeleteItem::OnDeleteItem(void)
{
	_DeleteItem( m_ItemID);

}

BOOL CPopupDeleteItem::_CheckItem(INT32 nItemFlag)
{
	return TRUE;
}

void CPopupDeleteItem::_DeleteItem(INT32 ItemID)
{
	INT64 authID = g_pFramework->GetInvenList()->GetCashItemWareDBIdx( ItemID);
	if (INVALID_WAREDB_INDEX == authID) return;

	g_pGameContext->SetEvent(EVENT_AUTH_DELETE, &authID);


	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(ItemID);
	I3ASSERT( pInfo != NULL);
}

void CPopupDeleteItem::_DeleteItemOk(void)
{

}

BOOL CPopupDeleteItem::_SetCurrentDeleteItemName(i3GuiEditBox * pControl)
{
	I3ASSERT( pControl != NULL);
	char Name[MAX_STRING_COUNT] = {};
	g_pShopItemInfoDataBase->GetItemName(m_ItemID, Name, MAX_STRING_COUNT);
	pControl->SetText( Name);
	return TRUE;
}

void CPopupDeleteItem::_SetEnablePopup(DELETE_ITEM_POPUP_TYPE type)
{
	switch( type) 
	{
	case DELETE_ITEM_QUERY:
		{
			m_pQueryRoot->SetEnable(TRUE);
		}
		break;
	}
}

void CPopupDeleteItem::CbQueryOk(void * pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		((CPopupDeleteItem*) pThis)->OnDeleteItem();
		((CPopupDeleteItem*) pThis)->OnClosePopup();
	}
	else
	{
		((CPopupDeleteItem*) pThis)->OnCancel();
	}

}

void CPopupDeleteItem::_GetInvenUseDay(UINT32 ItemID, char * pOut)
{
		UINT32 itemTerm = 0;
		UINT8 itemAttr = 0;
		UINT32 itemArg = 0;

		g_pFramework->GetCashItemTypeByID( ItemID, &itemAttr, &itemArg);
		
		//	���� ������� ���� �Ⱓ�� �˾ƿ´�.
		if( itemArg > 0)
		{
			itemTerm = g_pGameContext->GetInventorySecTime(itemArg);
			if( itemTerm > 0)
			{
				//	�Ⱓ ���� - komet
				if( itemTerm >= 120)
					itemTerm -= 120;

				itemTerm = ConvertSecToUseDay(itemTerm);
			}
		}
		else
		{
			itemTerm = 0;
		}

		if (1 < itemTerm)
		{
			//	���� �Ⱓ : %d��
			i3String::Format(pOut, MAX_STRING_COUNT, "{col:255,255,255,255}%d%s{/col}",
				itemTerm, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));/*���� �Ⱓ : {col:255,255,255,255}%d%s{/col}*/
		}
		else if( itemTerm == 0)
		{
			//	���� �Ⱓ : %d��
			i3String::Format(pOut, MAX_STRING_COUNT, "{col:255,255,255,255}%d%s{/col}",
				itemTerm, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));/*���� �Ⱓ : {col:255,255,255,255}%d%s{/col}*/
		}
		else
		{
			//	���� �Ⱓ : 1�� �̸�
			i3String::Format(pOut, MAX_STRING_COUNT,  "{col:255,255,255,255}1%s %s{/col}",
				GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"), GAME_STRING("STBL_IDX_ITEM_LESS_THEN"));/*���� �Ⱓ : {col:210,0,0,255}1%s %s{/col}*/
		}
		//test
}

#endif	// ENABLE_UI2

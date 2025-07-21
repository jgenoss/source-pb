#include "pch.h"
#include "GlobalVariables.h"
#include "PopupUseItem.h"

#if !defined( ENABLE_UI2)

enum ITEM_USE_METHOD
{
	USE_METHOD_UNKNOWN,

	USE_METHOD_DEFAULT_AUTH,		//DefaultAuth,
	USE_METHOD_DISGUISE_NICK,		//DisguiseNick,
	USE_METHOD_DISGUISE_RANK,		//DisguiseRank,
	USE_METHOD_COLOR_NICK,			//ColorNick,
	USE_METHOD_COLOR_CLAN,			//ColorClan,
	USE_METHOD_COLOR_CROSS_HAIR,	//ColorCrossHair,
	USE_METHOD_CHANGE_NICK,			//ChangeNick,
	USE_METHOD_CHANGE_CLAN_NAME,	//ChangeClanName,
	USE_METHOD_CHANGE_CLAN_MARK,	//ChangeClanMark,
	USE_METHOD_BATTLE_FIELD_MOVE,	//BattleFieldMove,

	MAX_USE_METHOD
};

enum ITEM_RESULT_METHOD
{
	RESULT_METHOD_UNKNOWN,

	RESULT_METHOD_COLOR_NICK,		//ColorNick,
	RESULT_METHOD_COLOR_CLAN,		//ColorClan,
	RESULT_METHOD_NICK,				//Nick,
	RESULT_METHOD_RANK,				//Rank,
	RESULT_METHOD_COLOR_CROSS_HAIR,	//ColorCrossHair
	RESULT_METHOD_CLAN_NAME,		//ClanName,
	RESULT_METHOD_CLAN_PERSON_PLUS,	//ClanPersonPlus
	RESULT_METHOD_GRAB_BAG,			//GrabBag,
	RESULT_METHOD_POINT,			//Point,

	MAX_RESULT_METHOD
};

ITEM_USE_METHOD		UseMethodStringToEnum(const char * pString)
{
	const char * Keywords[MAX_USE_METHOD] =
	{
		_T(""),
		_T("DefaultAuth"),
		_T("DisguiseNick"),
		_T("DisguiseRank"),
		_T("ColorNick"),
		_T("ColorClan"),
		_T("ColorCrossHair"),
		_T("ChangeNick"),
		_T("ChangeClanName"),
		_T("ChangeClanMark"),
		_T("BattleFieldMove"),
	};

	for (INT32 i = 1 ; i < MAX_USE_METHOD ; ++i )
	{
		if ( 0 == i3String::Compare(Keywords[i], pString) )
			return static_cast<ITEM_USE_METHOD>(i);
	}

	return USE_METHOD_UNKNOWN;
}

ITEM_RESULT_METHOD	ResultMethodStringToEnum(const char * pString)
{
	const char * Keywords[MAX_RESULT_METHOD] =
	{
		_T(""),
		_T("ColorNick"),
		_T("ColorClan"),
		_T("Nick"),
		_T("Rank"),
		_T("ColorCrossHair"),
		_T("ClanName"),
		_T("ClanPersonPlus"),
		_T("GrabBag"),
		_T("Point"),
	};

	for (INT32 i = 1 ; i < MAX_RESULT_METHOD ; ++i )
	{
		if ( 0 == i3String::Compare(Keywords[i], pString) )
			return static_cast<ITEM_RESULT_METHOD>(i);
	}

	return RESULT_METHOD_UNKNOWN;
}



struct UseCashItemMessage
{
	char * _pszResultStatic;	//	USE_ITEM_COMPLETE '��� ������'
	char * _pszResultValue;		//	USE_ITEM_COMPLETE '��� ���'
	char * _pszResultMessage;	//	USE_ITEM_COMPLETE '�ȳ� �޽���'
};

I3_CLASS_INSTANCE(CPopupUseItem, CPopupBase);

CPopupUseItem::CPopupUseItem(void)
{
	m_ItemID = 0;
	m_AuthWareDBIndex = 0;
	m_Type = USE_ITEM_UNKNOWN;

	m_Style = USE_ITEM_STYLE_NORMAL;
	m_pStringFilter = NULL;
	m_pStringFilter2 = NULL;

	m_bChangeNameChecked = FALSE;

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

	m_pChangeNameRoot = NULL;
	m_pChangeNameCaption = NULL;
	m_pChangeNameButtonOk = NULL;
	m_pChangeNameButtonCancel = NULL;
	m_pChangeNameItemStatic = NULL;
	m_pChangeNameItemValue = NULL;
	m_pChangeNameBeforeStatic = NULL;
	m_pChangeNameBeforeValue = NULL;
	m_pChangeNameUserStatic = NULL;
	m_pChangeNameUserEdit = NULL;
	m_pChangeNameCheckStatic = NULL;
	m_pChangeNameCheck = NULL;
	m_pChangeNameNoticeValue = NULL;

	m_pChangeColorRoot = NULL;
	m_pChangeColorCaption = NULL;
	m_pChangeColorButtonOk = NULL;
	m_pChangeColorButtonCancel = NULL;
	m_pChangeColorItemStatic = NULL;
	m_pChangeColorItemValue = NULL;
	m_pChangeColorCurrentStatic = NULL;
	m_pChangeColorCurrentColorBox = NULL;
	m_pChangeColorSelectStatic = NULL;
	m_pChangeColorApplyStatic = NULL;
	m_pChangeColorApplyValue = NULL;
	m_pChangeColorNoticeValue = NULL;
	m_pChangeColorSelectColorBoxFrame = NULL;

	i3mem::FillZero(m_pChangeColorSelectColorBox, sizeof(m_pChangeColorSelectColorBox));


	m_pChangeCrosshairColorRoot = NULL;
	m_pChangeCrosshairColorCaption = NULL;
	m_pChangeCrosshairColorButtonOk = NULL;
	m_pChangeCrosshairColorButtonCancel = NULL;
	m_pChangeCrosshairColorItemStatic = NULL;
	m_pChangeCrosshairColorItemValue = NULL;
	m_pChangeCrosshairColorCurrentStatic = NULL;
	m_pChangeCrosshairColorCurrentColorBox = NULL;
	m_pChangeCrosshairColorSelectStatic = NULL;
	m_pChangeCrosshairColorNoticeValue = NULL;
	m_pChangeCrosshairColorSelectColorBoxFrame = NULL;
	m_pChangeCrosshairColorCrossImage = NULL;
	m_pChangeCrosshairColorCrossDotImage = NULL;
	m_pChangeCrosshairColorDotImage = NULL;
	m_pChangeCrosshairColorCircleImage = NULL;

	i3mem::FillZero(m_pChangeCrosshairColorSelectColorBox, sizeof(m_pChangeCrosshairColorSelectColorBox));

	m_pChangeRankRoot = NULL;
	m_pChangeRankCaption = NULL;
	m_pChangeRankButtonOk = NULL;
	m_pChangeRankButtonCancel = NULL;
	m_pChangeRankItemStatic = NULL;
	m_pChangeRankItemValue = NULL;
	m_pChangeRankCurrentStatic = NULL;
	m_pChangeRankCurrentValue = NULL;
	m_pChangeRankSelectStatic = NULL;
	m_pChangeRankSelectCombo = NULL;
	m_pChangeRankSelectButton = NULL;
	m_pChangeRankSelectList = NULL;
	m_pChangeRankNoticeValue = NULL;

	m_pCompleteRoot = NULL;
	m_pCompleteCaption = NULL;
	m_pCompleteButtonOk = NULL;
	m_pCompleteResultStatic = NULL;
	m_pCompleteResultValue = NULL;
	m_pCompleteNoticeValue = NULL;

	m_szChangeName[0] = 0;
	m_nSelected = 0;
}

CPopupUseItem::~CPopupUseItem(void)
{
	I3_SAFE_RELEASE(m_pStringFilter);
	I3_SAFE_RELEASE(m_pStringFilter2);
}

BOOL CPopupUseItem::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char strName[MAX_STRING_COUNT] = "";

	LinkControlEx(plist, "UseItem", m_pRoot, -1);

	//	�����ۻ�� ���� �˾�
	{
		LinkControlEx(plist, "useItemQueryPopup", m_pQueryRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupCaption", m_pQueryCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupOk", m_pQueryButtonOk, GCI_USE_ITEM_B_QUERY_OK);
		LinkControlEx(plist, "useItemQueryPopupCancel", m_pQueryButtonCancel, GCI_USE_ITEM_B_QUERY_CANCEL);
		LinkControlEx(plist, "useItemQueryPopupItemImage", m_pQueryItemImage, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupItemStatic", m_pQueryItemStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupItemValue", m_pQueryItemValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupAttrStatic", m_pQueryAttrStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupAttrValue", m_pQueryAttrValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupNoticeValue", m_pQueryNotiveValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupStatic", m_pQueryStatic, GCI_UNKNOWN);
	}

	//	�г��� ����
	{
		LinkControlEx(plist, "useItemChangeNamePopup", m_pChangeNameRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupCaption", m_pChangeNameCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupOk", m_pChangeNameButtonOk, GCI_USE_ITEM_B_USE_ITEM_OK);
		LinkControlEx(plist, "useItemChangeNamePopupCancel", m_pChangeNameButtonCancel, GCI_USE_ITEM_B_USE_ITEM_CANCEL);
		LinkControlEx(plist, "useItemChangeNamePopupItemStatic", m_pChangeNameItemStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupItemValue", m_pChangeNameItemValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupBeforeStatic", m_pChangeNameBeforeStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupBeforeValue", m_pChangeNameBeforeValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupUserStatic", m_pChangeNameUserStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupUserEdit", m_pChangeNameUserEdit, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupCheckStatic", m_pChangeNameCheckStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupCheck", m_pChangeNameCheck, GCI_USE_ITEM_B_CHANGE_NAME_CHECK);
		LinkControlEx(plist, "useItemChangeNamePopupStatic", m_pChangeNameNoticeValue, GCI_UNKNOWN);
	}

	//	�÷� ����
	{
		LinkControlEx(plist, "useItemChangeColorPopup", m_pChangeColorRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupCaption", m_pChangeColorCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupOk", m_pChangeColorButtonOk, GCI_USE_ITEM_B_USE_ITEM_OK);
		LinkControlEx(plist, "useItemChangeColorPopupCancel", m_pChangeColorButtonCancel, GCI_USE_ITEM_B_USE_ITEM_CANCEL);
		LinkControlEx(plist, "useItemChangeColorPopupTitleStatic", m_pChangeColorItemStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupTitleValue", m_pChangeColorItemValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupCurrentStatic", m_pChangeColorCurrentStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCurrentColorBox", m_pChangeColorCurrentColorBox, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupSelectStatic", m_pChangeColorSelectStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupApplyStatic", m_pChangeColorApplyStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupApplyValue", m_pChangeColorApplyValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupNoticeValue", m_pChangeColorNoticeValue, GCI_UNKNOWN);

		for(INT32 i=0; i<SELECT_COLOR_BOX_COUNT; i++)
		{
			sprintf_s(strName, "useItemChangeColorSelectColorBox%d", i + 1);
			LinkControlEx(plist, strName, m_pChangeColorSelectColorBox[i], GCI_USE_ITEM_S_CHANGE_COLOR_BOX1 + i);
		}

		LinkControlEx(plist, "useItemChangeColorSelectColorSelected", m_pChangeColorSelectColorBoxFrame, GCI_UNKNOWN);

	}

	//	�÷� ũ�ν����
	{
		LinkControlEx(plist, "useItemChangeColorCrosshairPopup", m_pChangeCrosshairColorRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupCaption", m_pChangeCrosshairColorCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupOk", m_pChangeCrosshairColorButtonOk, GCI_USE_ITEM_B_USE_ITEM_OK);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupCancel", m_pChangeCrosshairColorButtonCancel, GCI_USE_ITEM_B_USE_ITEM_CANCEL);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupTitleStatic", m_pChangeCrosshairColorItemStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupTitleValue", m_pChangeCrosshairColorItemValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupCurrentStatic", m_pChangeCrosshairColorCurrentStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairCurrentColorBox", m_pChangeCrosshairColorCurrentColorBox, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupSelectStatic", m_pChangeCrosshairColorSelectStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupNoticeValue", m_pChangeCrosshairColorNoticeValue, GCI_UNKNOWN);

		LinkControlEx(plist, "useItemChangeColorCrosshairCross", m_pChangeCrosshairColorCrossImage, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairCrossDot", m_pChangeCrosshairColorCrossDotImage, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairDot", m_pChangeCrosshairColorDotImage, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairCircle", m_pChangeCrosshairColorCircleImage, GCI_UNKNOWN);

		for(INT32 i=0; i<SELECT_CROSSHAIR_COLOR_BOX_COUNT; i++)
		{
			sprintf_s(strName, "useItemChangeColorCrosshairSelectColorBox%d", i + 1);
			LinkControlEx(plist, strName, m_pChangeCrosshairColorSelectColorBox[i], GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX1 + i);
		}

		LinkControlEx(plist, "useItemChangeColorCrosshairSelectColorSelected", m_pChangeCrosshairColorSelectColorBoxFrame, GCI_UNKNOWN);

	}

	//	��� ����
	{
		LinkControlEx(plist, "useItemChangeRankPopup", m_pChangeRankRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupCaption", m_pChangeRankCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupOk", m_pChangeRankButtonOk, GCI_USE_ITEM_B_USE_ITEM_OK);
		LinkControlEx(plist, "useItemChangeRankPopupCancel", m_pChangeRankButtonCancel, GCI_USE_ITEM_B_USE_ITEM_CANCEL);
		LinkControlEx(plist, "useItemChangeRankPopupTitleStatic", m_pChangeRankItemStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupTitleValue", m_pChangeRankItemValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupCurrentStatic", m_pChangeRankCurrentStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupCurrentValue", m_pChangeRankCurrentValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupSelectStatic", m_pChangeRankSelectStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupSelectCombo", m_pChangeRankSelectCombo, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupSelectButton", m_pChangeRankSelectButton, GCI_USE_ITEM_B_CHANGE_RANK_SELECT_BUTTON);
		LinkControlEx(plist, "useItemChangeRankPopupSelectList", m_pChangeRankSelectList, GCI_USE_ITEM_L_CHANGE_RANK_SELECT_LIST);
		LinkControlEx(plist, "useItemChangeRankPopupNoticeValue", m_pChangeRankNoticeValue, GCI_UNKNOWN);
	}

	//	��� Ȯ�� �˾�
	{
		LinkControlEx(plist, "useItemCompletePopup", m_pCompleteRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupCaption", m_pCompleteCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupOk", m_pCompleteButtonOk, GCI_USE_ITEM_B_CONFIRM_OK);
		LinkControlEx(plist, "useItemCompletePopupResultStatic", m_pCompleteResultStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupResultValue", m_pCompleteResultValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupNoticeValue", m_pCompleteNoticeValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupCrosshairCross", m_pCompleteCrosshair_Cross, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupCrosshairCrossDot", m_pCompleteCrosshair_CrossDot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupCrosshairDot", m_pCompleteCrosshair_Dot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupCrosshairCircle", m_pCompleteCrosshair_Circle, GCI_UNKNOWN);

	}

	return FALSE;
}

void CPopupUseItem::OnInitControl(void)
{
	//	�����ۻ�� �˾�
	{
		INIT_WIN_CAPTION( m_pQueryCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*������ ���*/

		INIT_CAPTION_BUTTON(m_pQueryButtonOk, GAME_STRING("STR_POPUP_OK"));			//	"Ȯ��"
		INIT_CAPTION_BUTTON(m_pQueryButtonCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"���"

		INIT_STATIC_CAPTION(m_pQueryItemStatic, GAME_STRING("STR_POPUP_USE_ITEM"));/*��� ������:*/
		INIT_VALUE_CAPTION(m_pQueryItemValue, "");

		INIT_STATIC_CAPTION(m_pQueryAttrStatic, GAME_STRING("STR_POPUP_ITEM_ATTRIBUTE"));/*�Ӽ�:*/
		INIT_VALUE_CAPTION(m_pQueryAttrValue, "");

		INIT_VALUE_CAPTION(m_pQueryNotiveValue, "");
		m_pQueryNotiveValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pQueryNotiveValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_pQueryNotiveValue->SetTextSpace(0, 2);
		m_pQueryNotiveValue->SetTextAutoWrap(TRUE);

		INIT_WIN_CAPTION(m_pQueryStatic, GAME_STRING("STR_POPUP_Q_MESSAGE_WANT_USE_ITEM"));/*�������� ��� �Ͻðڽ��ϱ�?*/

		m_pQueryRoot->SetEnable(FALSE);
	}

	//	�г��� ���� �˾�
	{
		INIT_WIN_CAPTION( m_pChangeNameCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*������ ���*/

		INIT_CAPTION_BUTTON(m_pChangeNameButtonOk, GAME_STRING("STR_POPUP_OK"));	//	"Ȯ��"
		INIT_CAPTION_BUTTON(m_pChangeNameButtonCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"���"

		INIT_STATIC_CAPTION(m_pChangeNameItemStatic, GAME_STRING("STR_POPUP_USE_ITEM"));/*��� ������:*/
		INIT_VALUE_CAPTION(m_pChangeNameItemValue, "");

		INIT_STATIC_CAPTION(m_pChangeNameBeforeStatic, GAME_STRING("STR_POPUP_CURRENT_NICK_NAME"));/*���� �г���:*/
		INIT_VALUE_CAPTION(m_pChangeNameBeforeValue, "");

		INIT_STATIC_CAPTION(m_pChangeNameUserStatic, GAME_STRING("STR_POPUP_MODIFIED_NICK"));/*���� �г���:*/

		CREATE_IME_ELEMENT ime;
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
		ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
		m_pChangeNameUserEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pChangeNameUserEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pChangeNameUserEdit->SetTextColor(GetColor(GCT_VALUE));
		m_pChangeNameUserEdit->SetText("");

		INIT_SLOT(m_pChangeNameNoticeValue);
		INIT_STATIC_CAPTION2(m_pChangeNameCheckStatic, GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*�ߺ��˻縦 ���ֽʽÿ�.*/
		INIT_CAPTION_BUTTON(m_pChangeNameCheck, GAME_STRING("STR_POPUP_CONFIRM_OVERLAPPING"));/*�ߺ� Ȯ��*/

		m_pChangeNameRoot->SetEnable(FALSE);
	}

	//	�÷� ����
	{
		INIT_WIN_CAPTION( m_pChangeColorCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*������ ���*/

		INIT_STATIC_CAPTION(m_pChangeColorItemStatic, GAME_STRING("STR_POPUP_USE_ITEM"));/*��� ������:*/
		INIT_VALUE_CAPTION(m_pChangeColorItemValue, "");

		INIT_STATIC_CAPTION(m_pChangeColorCurrentStatic, GAME_STRING("STR_POPUP_CURRENT_COLOR"));/*���� ����:*/
		INIT_STATIC_CAPTION(m_pChangeColorSelectStatic, GAME_STRING("STR_POPUP_MODIFIED_COLOR"));/*���� ����:*/

		INIT_STATIC_CAPTION(m_pChangeColorApplyStatic, "");
		INIT_VALUE_CAPTION(m_pChangeColorApplyValue, "");

		INIT_WIN_CAPTION( m_pChangeColorNoticeValue, GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));/*������ ������ ������ �ֽʽÿ�.*/

		INIT_CAPTION_BUTTON(m_pChangeColorButtonOk, GAME_STRING("STR_POPUP_OK"));	//	"Ȯ��"
		INIT_CAPTION_BUTTON(m_pChangeColorButtonCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"���"

		for(INT32 i=0; i<SELECT_COLOR_BOX_COUNT; i++)
		{
			m_pChangeColorSelectColorBox[i]->setInputDisable(FALSE);
			m_pChangeColorSelectColorBox[i]->SetAllShapeEnable(FALSE);
			m_pChangeColorSelectColorBox[i]->SetShapeEnable(i, TRUE);
		}

		m_pChangeColorRoot->SetEnable(FALSE);
	}

	//	��� ����
	{
		INIT_WIN_CAPTION( m_pChangeRankCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*������ ���*/

		INIT_STATIC_CAPTION(m_pChangeRankItemStatic, GAME_STRING("STR_POPUP_USE_ITEM"));/*��� ������:*/
		INIT_VALUE_CAPTION(m_pChangeRankItemValue, "");

		INIT_STATIC_CAPTION(m_pChangeRankCurrentStatic, GAME_STRING("STR_POPUP_CURRENT_RANK"));/*���� ���:*/
		INIT_VALUE_CAPTION(m_pChangeRankCurrentValue, "");

		INIT_STATIC_CAPTION(m_pChangeRankSelectStatic, GAME_STRING("STR_POPUP_MODIFIED_RANK"));/*���� ���:*/
		INIT_STATIC_CAPTION(m_pChangeRankSelectCombo, "");

		INIT_WIN_CAPTION( m_pChangeRankNoticeValue, GAME_STRING("STR_POPUP_MESSAGE_MODIFY_RANK"));/*������ ����� ������ �ֽʽÿ�.*/

		m_pChangeRankSelectList->CreateTextEx(5,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pChangeRankSelectList->SetSelColor(100, 100, 100, 100);
		m_pChangeRankSelectList->SetTextColor(GetColor(GCT_DEFAULT));
		m_pChangeRankSelectList->SetExclusiveEnableControl(TRUE);
		m_pChangeRankSelectList->AddElement(GAME_STRING("STBL_IDX_RANK_0"));		//	"�Ʒú�"
		m_pChangeRankSelectList->AddElement(GAME_STRING("STBL_IDX_RANK_1"));		//	"�̺�"
		m_pChangeRankSelectList->AddElement(GAME_STRING("STBL_IDX_RANK_2"));		//	"�Ϻ�"
		m_pChangeRankSelectList->AddElement(GAME_STRING("STBL_IDX_RANK_3"));		//	"��"
		m_pChangeRankSelectList->AddElement(GAME_STRING("STBL_IDX_RANK_4"));		//	"����"
		m_pChangeRankSelectList->SetEnable(FALSE);

		INIT_CAPTION_BUTTON(m_pChangeRankButtonOk, GAME_STRING("STR_POPUP_OK"));	//	"Ȯ��"
		INIT_CAPTION_BUTTON(m_pChangeRankButtonCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"���"

		m_pChangeRankRoot->SetEnable(FALSE);
	}

	//	�÷� ũ�ν����
	{
		INIT_WIN_CAPTION( m_pChangeCrosshairColorCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*������ ���*/

		INIT_STATIC_CAPTION(m_pChangeCrosshairColorItemStatic, GAME_STRING("STR_POPUP_USE_ITEM"));/*��� ������:*/
		INIT_VALUE_CAPTION(m_pChangeCrosshairColorItemValue, "");

		INIT_STATIC_CAPTION(m_pChangeCrosshairColorCurrentStatic, GAME_STRING("STR_POPUP_CURRENT_COLOR"));/*���� ����:*/
		INIT_STATIC_CAPTION(m_pChangeCrosshairColorSelectStatic, GAME_STRING("STR_POPUP_MODIFIED_COLOR"));/*���� ����:*/

		INIT_WIN_CAPTION( m_pChangeCrosshairColorNoticeValue, GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));/*������ ������ ������ �ֽʽÿ�.*/

		INIT_CAPTION_BUTTON(m_pChangeCrosshairColorButtonOk, GAME_STRING("STR_POPUP_OK"));	//	"Ȯ��"
		INIT_CAPTION_BUTTON(m_pChangeCrosshairColorButtonCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"���"

		for(INT32 i=0; i<SELECT_CROSSHAIR_COLOR_BOX_COUNT; i++)
		{
			m_pChangeCrosshairColorSelectColorBox[i]->setInputDisable(FALSE);
			m_pChangeCrosshairColorSelectColorBox[i]->SetAllShapeEnable(FALSE);
			m_pChangeCrosshairColorSelectColorBox[i]->SetShapeEnable(i, TRUE);
		}

		m_pChangeCrosshairColorRoot->SetEnable(FALSE);
	}

	//	��� Ȯ�� �˾�
	{
		INIT_WIN_CAPTION( m_pCompleteCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*������ ���*/
		INIT_CAPTION_BUTTON(m_pCompleteButtonOk, GAME_STRING("STR_POPUP_OK"));	//	"Ȯ��"

		INIT_STATIC_CAPTION(m_pCompleteResultStatic, "");
		INIT_VALUE_CAPTION(m_pCompleteResultValue, "");
		m_pCompleteResultValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);

		INIT_WIN_CAPTION(m_pCompleteNoticeValue, "");

		m_pCompleteRoot->SetEnable(FALSE);
	}
}

void CPopupUseItem::OnGameEvent( INT32 event, INT32 arg)
{
	if( m_ItemID <= 0)		//	������ ������� �ƴϹǷ� �����Ѵ�.
		return;

	//	�̺�Ʈ ����� �޾� ó���Ѵ�.
	switch( event )
	{	
	case EVENT_ITEM_AUTH:	// ������ ������ ����
	case EVENT_CLAN_PERSON_PLUS:	// Ŭ���� ���� ĳ�� ������ (CASHITEM_GROUP_CLAN_PERSON_PLUS_50) ��� ���
		{
			_SetInputLock(FALSE);	//	��� ȭ���̹Ƿ� �Է� ���� Ǭ��.

			if (EV_SUCCESSED(arg))
			{				
				// Ŭ���� ���� ĳ�� �������� EVENT_ITEM_AUTH �̺�Ʈ�϶��� Ŭ������ ���� ������ ��ġ�� ������ ��ũ���� ������ �����Ƿ�
				// ��Ȯ�ϰ� �������� ������Ʈ�Ǵ� EVENT_CLAN_PERSON_PLUS �̺�Ʈ�� �������� ��� ��� â�� ����Ѵ�.
				if( event == EVENT_ITEM_AUTH && 
					m_ItemID == MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_EXPENDABLE, CASHITEM_GROUP_CLAN_PERSON_PLUS_50, CASHITEM_DAY_NO))
					break;

				_Open(USE_ITEM_COMPLETE);
			}
			else
			{
				CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_FAILED_AUTH"),
					GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*������ ��� ����*//*������ �����Ͽ����ϴ�. ��� �� �ٽ� �õ����ּ���.*/

				OnExitKey();
			}
		}
		break;
	case EVENT_USE_ITEM_CHECK_NICK:
	case EVENT_CLAN_DUPLICATE_NAME:
		{
			if (arg == 0)
			{
				m_bChangeNameChecked = TRUE;

				I3COLOR color;
				i3Color::Set(&color, (UINT8) 139, 217, 37, 255);
				m_pChangeNameCheckStatic->SetTextColor(&color);

				if( m_Type == USE_ITEM_CHANGE_CLAN_NAME)
					m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_CAN_USE_CLAN_NAME"));/*��� ������ Ŭ�����Դϴ�.*/
				else
					m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_CAN_USE_NICK_NAME"));/*��� ������ �г����Դϴ�.*/

				ACTIVATE_BUTTON(m_pChangeNameButtonOk);
			}
			else
			{
				if( m_Type == USE_ITEM_CHANGE_CLAN_NAME)
					m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_CLAN_NAME"));/*���� ������̰ų� ����� �� ���� Ŭ�����Դϴ�*/
				else
					m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_NICKNAME"));/*���� ������̰ų� ����� �� ���� �г����Դϴ�*/

				m_pChangeNameCheckStatic->SetTextColor(GetColor(GCT_RED));

				m_pChangeNameUserEdit->SetFocus(TRUE);

				DEACTIVATE_BUTTON(m_pChangeNameButtonOk);
			}
		}
		break;
	}
}

void CPopupUseItem::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
		//	������ ���
	case GCI_USE_ITEM_B_QUERY_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	_QueryOK();
		break;
	case GCI_USE_ITEM_B_QUERY_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnCancel();
		break;
	case GCI_USE_ITEM_B_USE_ITEM_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	_UseItemOk();
		break;
	case GCI_USE_ITEM_B_USE_ITEM_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnCancel();
		break;
	case GCI_USE_ITEM_B_CHANGE_NAME_CHECK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChangeNameDuplicate();
		break;
	case GCI_USE_ITEM_B_CONFIRM_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnClosePopup();
		break;
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX1:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX2:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX3:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX4:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX5:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX6:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX7:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX8:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX9:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChangeColorSelect(pNotify->m_nID - GCI_USE_ITEM_S_CHANGE_COLOR_BOX1);
		break;
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX1:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX2:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX3:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX4:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX5:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX6:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX7:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX8:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX9:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChangeCrosshairColorSelect(pNotify->m_nID - GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX1);
		break;
	case GCI_USE_ITEM_B_CHANGE_RANK_SELECT_BUTTON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChangeRankPopupButton();
		break;
	case GCI_USE_ITEM_L_CHANGE_RANK_SELECT_LIST:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnChangeRankPopupList(pNotify->m_nPriParam);
		break;
	}
}

void CPopupUseItem::OnOpenPopup(void * pArg1, void * pArg2)
{
	m_ItemID = 0;
	m_AuthWareDBIndex = 0;

	m_pRoot->setInputDisable( FALSE);

	if(pArg1 == NULL)
		return;


	m_ItemID = *(INT32*)pArg1;
	if( m_ItemID <= 0)
	{
		STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_1"));/*�������� ����� �� �����ϴ�.\n�߸��� �������Դϴ�.*/
		return;
	}


	if(pArg2 != NULL)
	{
		m_Style = *(USE_ITEM_STYLE*)pArg2;
	}
	else
	{
		m_Style = USE_ITEM_STYLE_NORMAL;
	}

	if(m_Style == USE_ITEM_STYLE_RESULT)
	{
		CPopupBase::OnOpenPopup( pArg1, pArg2);		//	enable

		//	������ ��� ��� â
		_Open(USE_ITEM_COMPLETE);
	}
	else
	{
		CPopupBase::OnOpenPopup( pArg1, pArg2);		//	enable

		//	������ ó���� �������� ������ ����� ���� �˾��� ����.
		if( !_Open(USE_ITEM_QUERY))
			OnExitKey();
	}
}

void CPopupUseItem::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();

	m_pRoot->SetEnable(FALSE);

	I3_SAFE_RELEASE(m_pStringFilter);
	I3_SAFE_RELEASE(m_pStringFilter2);

	m_ItemID = 0;
}

void CPopupUseItem::OnCancel(void)
{
	//	�������� ������ ����ҷ��� ������ ���� ����� ���
	if( m_Style == USE_ITEM_STYLE_BUYING)
	{
		//	"�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�."
		CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"),
			GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
	}

	OnClosePopup();
}

void CPopupUseItem::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);
}

void CPopupUseItem::OnPreGuiNotify(void)
{

}

void CPopupUseItem::OnExitKey(void)
{
	OnClosePopup();

	m_pQueryRoot->SetEnable(FALSE);
	m_pCompleteRoot->SetEnable(FALSE);
	m_pChangeNameRoot->SetEnable(FALSE);
	m_pChangeColorRoot->SetEnable(FALSE);
	m_pChangeRankRoot->SetEnable(FALSE);
	m_pChangeCrosshairColorRoot->SetEnable(FALSE);
}


BOOL CPopupUseItem::_Open( USE_ITEM_POPUP_TYPE type)
{
	// ��뿩��
	UINT8 Type = 0;
	UINT32 Arg = 0;

	I3ASSERT( g_pFramework != NULL);
	I3ASSERT( g_pFramework->GetInvenList() != NULL);

	if (g_pFramework->GetCashItemTypeByID(m_ItemID, &Type, &Arg))
	{
		//	���� ������� ������
		if (ITEM_ATTR_TYPE_AUTH == Type)
		{
			CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CURRENT_USE_ITEM"),
				GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*������ ���*//*���� ������� �������Դϴ�.*/

			return FALSE;
		}
	}

	m_Type = type;

	m_pRoot->SetEnable(TRUE);

	_SetEnablePopup(m_Type);

	PlaySound(GUI_SND_POPUP_OPEN);

	switch( type)
	{
	case USE_ITEM_QUERY:
		{
			_InitQueryPopup();

			m_nSelected = -1;
		}
		break;
	case USE_ITEM_CHANGE_NICK:
		{
			_InitChangeNickPopup();
		}
		break;
	case USE_ITEM_CHANGE_CLAN_NAME:
		{
			_InitChangeClanNamePopup();
		}
		break;
	case USE_ITEM_COLOR_NICK:
		{
			_InitColorNickPopup();
		}
		break;
	case USE_ITEM_COLOR_CLAN_NAME:
		{
			_InitColorClanNamePopup();
		}
		break;
	case USE_ITEM_COLOR_CROSSHAIR:
		{
			_InitColorCrosshairPopup();
		}
		break;
	case USE_ITEM_DISGUISE_NICK:
		{
			_InitDisguiseNickPopup();
		}
		break;
	case USE_ITEM_DISGUISE_RANK:
		{
			_InitDisguiseRankPopup();
		}
		break;
	case USE_ITEM_COMPLETE:
		{
			_InitCompletePopup();
		}
		break;
	default:
		{
			CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"),
				GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*����� �� ���� �������Դϴ�.*//*������ ��� ����*/

			return FALSE;
		}
		break;
	}

	return TRUE;
}

void CPopupUseItem::_SetInputLock(BOOL bValue)
{
	m_pRoot->setInputDisable( bValue);
	i3GuiRoot::setGuiDisableInput( bValue);

	((CStageReady*)m_pParent)->SetMenuBarInputDisable( bValue);
}

void CPopupUseItem::_InitQueryPopup(void)
{
	m_pQueryRoot->SetEnable(TRUE);
	m_pCompleteRoot->SetEnable(FALSE);
	m_pChangeNameRoot->SetEnable(FALSE);
	m_pChangeColorRoot->SetEnable(FALSE);
	m_pChangeRankRoot->SetEnable(FALSE);
	m_pChangeCrosshairColorRoot->SetEnable(FALSE);

	INT32 itemUsage = GET_ITEM_FLAG_TYPE(m_ItemID);
	INT32 itemNum = GET_ITEM_FLAG_NUMBER(m_ItemID);

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
		char Attr[MAX_STRING_COUNT] = INVALID_TEXT;
		switch( itemUsage)
		{
		case ITEM_TYPE_MAINTENANCE:	i3String::Format(Attr, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_USABLE_TERM"), itemNum);/*%d�ϱ�*/	break;
		case ITEM_TYPE_EXPENDABLE:	i3String::Copy( Attr, GAME_STRING("STR_POPUP_DISPOSABLE"), MAX_STRING_COUNT );/*1ȸ��*/	break;
		}
		m_pQueryAttrValue->SetText(Attr);
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

void CPopupUseItem::_InitCompletePopup(void)
{
	BOOL	bUseColor = FALSE;
	char	temp[MAX_STRING_COUNT * 2] = "";
	INT32	idxColor = m_nSelected+1;

	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(m_ItemID);
	I3ASSERT( pInfo != NULL);

	//	BC�� ShopItem �׸� �� �����۸��� ���ǵǾ� �ִ� �޽����� ����Ѵ�.
	const char * pszResultStatic	= pInfo->GetResultStatic();
	const char * pszResultValue		= pInfo->GetResultValue();
	const char * pszResultNotice	= pInfo->GetResultNotice();

	m_pCompleteCrosshair_Cross->SetEnable(FALSE);
	m_pCompleteCrosshair_CrossDot->SetEnable(FALSE);
	m_pCompleteCrosshair_Dot->SetEnable(FALSE);
	m_pCompleteCrosshair_Circle->SetEnable(FALSE);


	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	ITEM_RESULT_METHOD	Method = ResultMethodStringToEnum(pInfo->GetResultMethod());
	switch (Method)
	{
	case RESULT_METHOD_COLOR_NICK		:
		{
			pszResultValue = MyUserInfoBasic._nick;
			bUseColor = TRUE;
		}
		break;
	case RESULT_METHOD_COLOR_CLAN		:
		{
			pszResultValue = MyUserInfoBasic._clanName;
			bUseColor = TRUE;
		}
		break;
	case RESULT_METHOD_NICK				:
		{
			pszResultValue = MyUserInfoBasic._nick;
		}
		break;
	case RESULT_METHOD_RANK				:
		{
			UINT32 rank = MyUserInfoBasic._rank;

			pszResultValue = GetCharaRank(rank);
		}
		break;
	case RESULT_METHOD_COLOR_CROSS_HAIR	:
		{
			I3COLOR * pSelectColor = (I3COLOR*)GetColorCashItem(idxColor);

			pszResultValue = "";

			m_pCompleteCrosshair_Cross->SetEnable(TRUE);
			m_pCompleteCrosshair_CrossDot->SetEnable(TRUE);
			m_pCompleteCrosshair_Dot->SetEnable(TRUE);
			m_pCompleteCrosshair_Circle->SetEnable(TRUE);

			//	ũ�ν���� ��� �̹��� ���� ����
			m_pCompleteCrosshair_Cross->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
			m_pCompleteCrosshair_CrossDot->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
			m_pCompleteCrosshair_Dot->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
			m_pCompleteCrosshair_Circle->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
		}
		break;
	case RESULT_METHOD_CLAN_NAME		:
		{
			//	Ŭ������ ������ �������� SUCCESS�� ����������� ������ �ٷ� ���ŵ��� �ʴ´�.
			//	���� �ð��� ������ �� �ִ� ���ŵǹǷ� ����� �����Ѱ�ó�� ������ �Ѵ�.
			pszResultValue = &m_szChangeName[0];
		}
		break;
	case RESULT_METHOD_CLAN_PERSON_PLUS	:
		{
			//	Ŭ������ ������ �������� SUCCESS�� ����������� ������ �ٷ� ���ŵ��� �ʴ´�.
			//	���� �ð��� ������ �� �ִ� ���ŵǹǷ� ����� �����Ѱ�ó�� ������ �Ѵ�.
			CLAN_DETAIL_INFO * pClanInfo = g_pGameContext->getMyClanDetailInfo();

			sprintf_s( temp, "%d", pClanInfo->_maxPerson);

			pszResultValue = &temp[0];
		}
		break;
	case RESULT_METHOD_GRAB_BAG			:
		{
			I3ASSERT(g_pGameContext->getCapsuleItemID() != m_ItemID);
			ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(m_ItemID);
			pszResultValue = pInfo->GetCapsuleItemResultName(g_pGameContext->getCapsuleItemIdx());
		}
		break;
	case RESULT_METHOD_POINT:
		{
			OnExitKey();
			return;
		}
		break;
	}


	m_pCompleteCaption->SetText(GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*������ ���*/
	m_pCompleteResultStatic->SetText(pszResultStatic);

	if( bUseColor)	//	�÷�
	{
		SetNameUsingColorTable(m_pCompleteResultValue, pszResultValue, (UINT8)idxColor);
	}
	else
	{
		m_pCompleteResultValue->SetText(pszResultValue);
		m_pCompleteResultValue->SetTextColor( GetColor( GCT_VALUE));
	}

	m_pCompleteNoticeValue->SetText(pszResultNotice);
}

void CPopupUseItem::_QueryOK(void)
{
	// ������ �������� ��쿣 �ٷ� ����Ѵ�.
	if ( ITEM_TYPE_EXPENDABLE == GET_ITEM_FLAG_TYPE(m_ItemID) )
	{
		_UseItem( m_ItemID);
		return;
	}

	// - ����
	// �Ⱓ�� �������� ������ ������ ������ �Ⱓ�� �������� �������� �Ⱓ���� �������� �ٲ�� �κ��丮�� ����ȴ�.
	//

	//
	// �Ⱓ�� �������� �ȳ� �޽����� ������ش�.
	//

	INT32 ItemID = GetCashItemByGroup( m_ItemID);

	UINT8 Type = 0;
	UINT32 Arg = 0;
	g_pFramework->GetCashItemTypeByID(ItemID, &Type, &Arg);
	if( Arg <= 0)
	{
		// �Ⱓ�� �������ε� �����Ⱓ�� 0�̸� ������ΰ� ���°��̴�.
		// �ٷ� ����ϵ��� �Ѵ�.
		_UseItem( m_ItemID);
		return;
	}



	//	"������� �������Դϴ�.\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
	const char * pszMessage = GAME_STRING("STR_POPUP_MESSAGE_CAN_NORMAL_ITEM");

	switch( GET_ITEM_FLAG_CLASS( m_ItemID))
	{
	case CASHITEM_GROUP_COLOR_CLAN:
	case CASHITEM_GROUP_COLOR_NICK:
	case CASHITEM_GROUP_COLOR_CROSSHAIR:
	case CASHITEM_GROUP_COLOR_CHATTING:
		{
			//	"������� �������Դϴ�.\n���� ������ ������ �� ������\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
			pszMessage = GAME_STRING("STR_POPUP_MESSAGE_CAN_CHANGE_COLOR");
		}
		break;
	case CASHITEM_GROUP_DISGUISE_RANK:
		{
			//	"������� �������Դϴ�.\n���� ���� ������� ������ �� ������\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
			pszMessage = GAME_STRING("STR_POPUP_MESSAGE_CAN_DISGUISE_RANK");
		}
		break;
	case CASHITEM_GROUP_DISGUISE_NICK:
		{
			//	"������� �������Դϴ�.\n���� �ӽ� �г������� ������ �� ������\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
			pszMessage = GAME_STRING("STR_POPUP_MESSAGE_CAN_DISGUISE_NICK");
		}
		break;
	}

	// ������� �������̸� �ٽ� �ѹ� ���´�.
	CAPTION_POPUP_Q(pszMessage, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"), MAKE_CALLBACK(CbQueryOk), this);

}

void CPopupUseItem::CbQueryOk(void * pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		((CPopupUseItem*) pThis)->OnUseItem();
	}
	else
	{
		((CPopupUseItem*) pThis)->OnCancel();
	}
}

void CPopupUseItem::OnUseItem(void)
{
	_UseItem( m_ItemID);
}

BOOL CPopupUseItem::_CheckItem(INT32 nItemFlag)
{
	switch( GET_ITEM_FLAG_TYPE(nItemFlag))
	{
		//	���Ӽ�
	case ITEM_TYPE_MAINTENANCE:
		{
			switch( GET_ITEM_FLAG_CLASS( nItemFlag))
			{
				//	HP 5%�� �̹� ������̸� HP 10%�� ����� �� ����.
			case CASHITEM_GROUP_MEGA_HP10:
				{
					INT32 useItem = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_MEGA_HP5, CASHITEM_DAY_NO);

					if( g_pFramework->IsUsingCashItem( useItem))
					{
						CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_BY_DUPLICATE"),
							GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*"�ߺ����� ����� �� ���� �������Դϴ�.".*//*������ ��� ����*/

						return FALSE;
					}
				}
				break;
				//	HP 10%�� �̹� ������̸� HP 5%�� ����� �� ����.
			case CASHITEM_GROUP_MEGA_HP5:
				{
					INT32 useItem = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_MEGA_HP10, CASHITEM_DAY_NO);

					if( g_pFramework->IsUsingCashItem( useItem))
					{
						CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_BY_DUPLICATE"),
							GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*"�ߺ����� ����� �� ���� �������Դϴ�.".*//*������ ��� ����*/

						return FALSE;
					}
				}
				break;
				//	Full Metal Jacket Ammo ������̸� Hollow Point Ammo �Ǵ� Jacketed Hollow Point �����۰� �ߺ� ��� �Ұ�.
			case CASHITEM_GROUP_FULLMETALJACKETAMMO:
			case CASHITEM_GROUP_HOLLOW_POINT_AMMO:
			case CASHITEM_GROUP_JACKETED_HELLOW_POINT_AMMO:
			case CASHITEM_GROUP_HOLLOW_POINT_AMMO_PLUS:
				{
					INT32 checkItemID[] =
					{
						MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_FULLMETALJACKETAMMO, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_HOLLOW_POINT_AMMO, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_JACKETED_HELLOW_POINT_AMMO, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_HOLLOW_POINT_AMMO_PLUS, CASHITEM_DAY_NO),
					};
					const INT32 checkItemCount = GET_ARRAY_COUNT(checkItemID);

					for ( INT32 i = 0 ; i < checkItemCount ; ++i )
					{
						// üũ���� �ʽ��ϴ�.
						if ( GET_ITEM_FLAG_CLASS(checkItemID[i]) == GET_ITEM_FLAG_CLASS( nItemFlag) )
							continue;

						if ( g_pFramework->IsUsingCashItem( checkItemID[i]) )
						{
							CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_BY_DUPLICATE"),
								GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*"�ߺ����� ����� �� ���� �������Դϴ�.".*//*������ ��� ����*/

							return FALSE;
						}
					}
				}
				break;

			case CASHITEM_GROUP_BULLET_PROOF_VEST:
			case CASHITEM_GROUP_BULLET_PROOF_VEST_PLUS:
			case CASHITEM_GROUP_BULLET_PROOF_VEST_GM:
			case CASHITEM_GROUP_BULLET_PROOF_VEST_METAL:
				{
					INT32 checkItemID[] =
					{
						MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_BULLET_PROOF_VEST, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_BULLET_PROOF_VEST_PLUS, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_BULLET_PROOF_VEST_GM, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_BULLET_PROOF_VEST_METAL, CASHITEM_DAY_NO),
					};
					const INT32 checkItemCount = GET_ARRAY_COUNT(checkItemID);

					for ( INT32 i = 0 ; i < checkItemCount ; ++i )
					{
						// üũ���� �ʽ��ϴ�.
						if ( GET_ITEM_FLAG_CLASS(checkItemID[i]) == GET_ITEM_FLAG_CLASS( nItemFlag) )
							continue;

						if ( g_pFramework->IsUsingCashItem( checkItemID[i]) )
						{
							CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_BY_DUPLICATE"),
								GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*"�ߺ����� ����� �� ���� �������Դϴ�.".*//*������ ��� ����*/

							return FALSE;
						}
					}
				}
				break;
			}
		}
		break;
		//	�Ҹ�
	case ITEM_TYPE_EXPENDABLE:
		{
		}
		break;
	}

	return TRUE;
}

void CPopupUseItem::_UseItem(INT32 ItemID)
{
	m_AuthWareDBIndex = g_pFramework->GetInvenList()->GetCashItemWareDBIdx( ItemID);

	if( m_AuthWareDBIndex == INVALID_WAREDB_INDEX)
	{
		CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"),
			GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*����� �� ���� �������Դϴ�.*//*������ ��� ����*/

		return;
	}

	//	��� ������ ���������� ���� �˻�.
	if( !_CheckItem( ItemID))
	{
		return;
	}

	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(ItemID);
	I3ASSERT( pInfo != NULL);

	ITEM_USE_METHOD	Method = UseMethodStringToEnum(pInfo->GetUseMethod());
	switch (Method)
	{
	case USE_METHOD_DEFAULT_AUTH		:
		{
			_SetInputLock(TRUE);

			//	������ �������� ������ �䱸
			g_pGameContext->SetEvent(EVENT_ITEM_AUTH, &m_AuthWareDBIndex);
		}
		break;
	case USE_METHOD_DISGUISE_NICK		:
		{
			//	�г��� ���� â ���
			if( !_Open(USE_ITEM_DISGUISE_NICK))
				OnExitKey();
		}
		break;
	case USE_METHOD_DISGUISE_RANK		:
		{
			//	��� ���� â ���
			if( !_Open(USE_ITEM_DISGUISE_RANK))
				OnExitKey();
		}
		break;
	case USE_METHOD_COLOR_NICK			:
		{
			if( !_Open(USE_ITEM_COLOR_NICK))
				OnExitKey();
		}
		break;
	case USE_METHOD_COLOR_CLAN			:
		{
			if( !_Open(USE_ITEM_COLOR_CLAN_NAME))
				OnExitKey();
		}
		break;
	case USE_METHOD_COLOR_CROSS_HAIR	:
		{
			if( !_Open(USE_ITEM_COLOR_CROSSHAIR))
				OnExitKey();
		}
		break;
	case USE_METHOD_CHANGE_NICK			:
		{
			//	'�ӽ� �г���' �������� �̹� ������̶�� ������ ��� �Ұ�
			INT32 itemDisguiseNick = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_NICK, CASHITEM_DAY_NO);

			if( g_pFramework->IsUsingCashItem( itemDisguiseNick))
			{
				OnClosePopup();

				//	�ӽ� �г��� ���� �߿��� �г��� ������� ����� �� �����ϴ�.\n�ӽ� �г��� ������ ���� �� �г����� �����Ͻñ� �ٶ��ϴ�.
				CAPTION_POPUP(GAME_STRING("STR_POPUP_Q_MESSAGE_MODIFY_NICK_WITHOUT_DISUISE_NICK"),
					GAME_STRING("STR_INVEN_DO_NOT_USE"));
			}
			else	//	������� �ƴҰ�� �ٷ� �г��� ���� â����
			{
				OnOpenChangeNick();
			}
		}
		break;
	case USE_METHOD_CHANGE_CLAN_NAME	:
		{
			if( !_Open(USE_ITEM_CHANGE_CLAN_NAME))
				OnExitKey();
		}
		break;
	case USE_METHOD_CHANGE_CLAN_MARK	:
		{
			CLAN_DETAIL_INFO * pClanInfo = g_pFramework->GetClanContext()->GetMyClanDetailInfo();
			UINT32 nOldMark = pClanInfo->_mark;

			OnClosePopup();

			//	Ŭ�� ��ũ ������ ��� ���� �˾��� �ƴ� �ٸ� �˾�(CPopupChangeClanMark)�� ����Ѵ�.
			((CStageReady*)m_pParent)->TogglePopup( POPUP_CHANGE_CLANMARK, &m_AuthWareDBIndex, &nOldMark);
		}
		break;
	case USE_METHOD_BATTLE_FIELD_MOVE	:
		{
			CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_USE_IN_GAME"),
				GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*�����߿��� ����� �� �ֽ��ϴ�.*//*������ ��� ����*/

			OnExitKey();
		}
		break;
	default:
		{
			CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"),
				GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*����� �� ���� �������Դϴ�.*//*������ ��� ����*/

			OnExitKey();
		}
		break;
	}

}

void CPopupUseItem::_UseItemOk(void)
{
	switch( m_Type)
	{
	case USE_ITEM_DISGUISE_NICK:
	case USE_ITEM_CHANGE_NICK:
	case USE_ITEM_CHANGE_CLAN_NAME:		OnChangeNameOk();			break;
	case USE_ITEM_COLOR_NICK:			_ChangeColorNickOk();		break;
	case USE_ITEM_COLOR_CLAN_NAME:		_ChangeColorClanNameOk();	break;
	case USE_ITEM_COLOR_CROSSHAIR:		_ChangeColorCrosshairOk();	break;
	case USE_ITEM_DISGUISE_RANK:		_ChangeDisguiseRankOk();	break;
	}
}

BOOL CPopupUseItem::_SetCurrentUseItemName(i3GuiEditBox * pControl)
{
	I3ASSERT( pControl != NULL);
	char Name[MAX_STRING_COUNT] = {};
	g_pShopItemInfoDataBase->GetItemName(m_ItemID, Name, MAX_STRING_COUNT);
	pControl->SetText( Name);
	return TRUE;
}

void CPopupUseItem::_SetEnablePopup(USE_ITEM_POPUP_TYPE type)
{
	switch( type)
	{
	case USE_ITEM_QUERY:
		{
			m_pQueryRoot->SetEnable(TRUE);
			m_pCompleteRoot->SetEnable(FALSE);
			m_pChangeNameRoot->SetEnable(FALSE);
			m_pChangeColorRoot->SetEnable(FALSE);
			m_pChangeRankRoot->SetEnable(FALSE);
			m_pChangeCrosshairColorRoot->SetEnable(FALSE);
		}
		break;
	case USE_ITEM_CHANGE_NICK:
	case USE_ITEM_CHANGE_CLAN_NAME:
	case USE_ITEM_DISGUISE_NICK:
		{
			m_pQueryRoot->SetEnable(FALSE);
			m_pCompleteRoot->SetEnable(FALSE);
			m_pChangeNameRoot->SetEnable(TRUE);
			m_pChangeColorRoot->SetEnable(FALSE);
			m_pChangeRankRoot->SetEnable(FALSE);
			m_pChangeCrosshairColorRoot->SetEnable(FALSE);
		}
		break;
	case USE_ITEM_COLOR_NICK:
	case USE_ITEM_COLOR_CLAN_NAME:
		{
			m_pQueryRoot->SetEnable(FALSE);
			m_pCompleteRoot->SetEnable(FALSE);
			m_pChangeNameRoot->SetEnable(FALSE);
			m_pChangeColorRoot->SetEnable(TRUE);
			m_pChangeRankRoot->SetEnable(FALSE);
			m_pChangeCrosshairColorRoot->SetEnable(FALSE);
		}
		break;
	case USE_ITEM_COLOR_CROSSHAIR:
		{
			m_pQueryRoot->SetEnable(FALSE);
			m_pCompleteRoot->SetEnable(FALSE);
			m_pChangeNameRoot->SetEnable(FALSE);
			m_pChangeColorRoot->SetEnable(FALSE);
			m_pChangeRankRoot->SetEnable(FALSE);
			m_pChangeCrosshairColorRoot->SetEnable(TRUE);
		}
		break;
	case USE_ITEM_DISGUISE_RANK:
		{
			m_pQueryRoot->SetEnable(FALSE);
			m_pCompleteRoot->SetEnable(FALSE);
			m_pChangeNameRoot->SetEnable(FALSE);
			m_pChangeColorRoot->SetEnable(FALSE);
			m_pChangeRankRoot->SetEnable(TRUE);
			m_pChangeCrosshairColorRoot->SetEnable(FALSE);
		}
		break;
	case USE_ITEM_COMPLETE:
		{
			m_pQueryRoot->SetEnable(FALSE);
			m_pCompleteRoot->SetEnable(TRUE);
			m_pChangeNameRoot->SetEnable(FALSE);
			m_pChangeColorRoot->SetEnable(FALSE);
			m_pChangeRankRoot->SetEnable(FALSE);
			m_pChangeCrosshairColorRoot->SetEnable(FALSE);
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_CHANGE_NICK

void CPopupUseItem::_InitChangeNickPopup(void)
{
	// ������
	_SetCurrentUseItemName( m_pChangeNameItemValue);

	m_pChangeNameBeforeStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_NICK_NAME"));/*���� �г���:*/

	// �г���
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	SetNameUsingColorTable(m_pChangeNameBeforeValue, MyUserInfoBasic._nick, MyUserInfoBasic._nickcolor);

	m_pChangeNameUserStatic->SetText(GAME_STRING("STR_POPUP_MODIFIED_NICK"));/*���� �г���:*/

	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
	m_pChangeNameUserEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pChangeNameUserEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pChangeNameUserEdit->SetIMEText( "");
	m_pChangeNameUserEdit->SetFocus(TRUE);

	m_pChangeNameCheckStatic->SetTextColor( GetColor(GCT_DEFAULT));
	m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*�ߺ��˻縦 ���ֽʽÿ�.*/

	m_pChangeNameNoticeValue->SetText(GAME_STRING("STR_POPUP_MESSAGE_MODIFY_NICK"));/*������ �г����� �Է����ֽʽÿ�.*/

	DEACTIVATE_BUTTON(m_pChangeNameButtonOk);

	m_bChangeNameChecked = FALSE;

	m_szChangeName[0] = 0;

	if(m_pStringFilter == NULL)
	{
		m_pStringFilter = CStringFilter::NewObject();
		m_pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	}

	if(m_pStringFilter2 == NULL)
	{
		m_pStringFilter2 = CStringFilter::NewObject();
		m_pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);
	}
}

void CPopupUseItem::_ChangeNickOk(void)
{
	_SetInputLock(TRUE);

	//	������ ���� ������ �г����� ������.
	UINT8 ui8Size = static_cast<UINT8>(i3String::Length( m_szChangeName )+1);
	g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, m_szChangeName);
}

BOOL CPopupUseItem::OnChangeNameOk( void)
{
	//	���� �г��Ӱ� Ŭ���� ����
	if( m_Type == USE_ITEM_CHANGE_NICK || m_Type == USE_ITEM_CHANGE_CLAN_NAME || m_Type == USE_ITEM_DISGUISE_NICK)
	{
		if(m_bChangeNameChecked == FALSE)
		{
			STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*�ߺ��˻縦 ���ֽʽÿ�.*/

			return FALSE;
		}
		else
		{
			char szInputName[MAX_STRING_COUNT] = "";
			i3String::Copy( szInputName, m_pChangeNameUserEdit->getIMETextBuf(), MAX_STRING_COUNT );

			//	�ߺ��˻�� �г��Ӱ� �Է�â�� �г����� �ٸ��� �ٽ� �ߺ��˻縦 �ϵ��� �Ѵ�.
			if( i3String::Compare(m_szChangeName, szInputName) != 0)
			{
				m_bChangeNameChecked = FALSE;

				m_pChangeNameCheckStatic->SetTextColor( GetColor(GCT_DEFAULT));
				m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*�ߺ��˻縦 ���ֽʽÿ�.*/

				m_pChangeNameUserEdit->SetFocus(TRUE);

				return FALSE;
			}

			switch( m_Type)
			{
			case USE_ITEM_CHANGE_NICK:			_ChangeNickOk();			break;
			case USE_ITEM_CHANGE_CLAN_NAME:		_ChangeClanNameOk();		break;
			case USE_ITEM_DISGUISE_NICK:		_ChangeDisguiseNickOk();	break;
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CPopupUseItem::OnChangeNameDuplicate(void)
{
	BOOL ret = TRUE;
	char szError[MAX_STRING_COUNT] = "";

	//	�̹� üũ�� �������̴�.
	if( m_szChangeName[0] != 0 && i3String::Compare( m_szChangeName, m_pChangeNameUserEdit->getIMETextBuf()) == 0)
	{
		return TRUE;
	}

	m_bChangeNameChecked = FALSE;

	i3String::Copy( m_szChangeName, m_pChangeNameUserEdit->getIMETextBuf(), MAX_STRING_COUNT );

	if( i3String::Length(m_szChangeName) <= 0)
	{
		switch( m_Type)
		{
		case USE_ITEM_CHANGE_NICK:		i3String::Copy(szError, GAME_STRING("STR_POPUP_MESSAGE_PLEASE_NAME"), MAX_STRING_COUNT);/*"�г����� �����ּ���."*/	break;
		case USE_ITEM_CHANGE_CLAN_NAME:	i3String::Copy(szError, GAME_STRING("STR_POPUP_MESSAGE_INPUT_CLAN_NAME"), MAX_STRING_COUNT);/*Ŭ������ �����ּ���.*/	break;
		case USE_ITEM_DISGUISE_NICK:	i3String::Copy(szError, GAME_STRING("STR_POPUP_MESSAGE_INPUT_DISGUISED_NICK_1"), MAX_STRING_COUNT);/*�ӽ� �г����� �����ּ���.*/break;
		}

		ret = FALSE;

		goto LABEL_CHECK_NAME_EXIT;
	}

	// �г��Ӹ� �ּұ��ڼ� ����
	if( m_Type == USE_ITEM_CHANGE_NICK || m_Type == USE_ITEM_DISGUISE_NICK)
	{
		if (FALSE == CStringFilter::IsValidNickname(m_szChangeName))
		{
			//  �ּ� ���ڼ� ���Եǵ��� ���� �����ؾ��մϴ�.
			i3String::Copy( szError, GAME_STRING("STBL_IDX_EP_LOBBY_NICK_LENGTH"), MAX_STRING_COUNT );/*2�ڿ��� 16�� ���̷� �����Ͽ� �ֽñ� �ٶ��ϴ�.*/

			ret = FALSE;

			goto LABEL_CHECK_NAME_EXIT;
		}
	}

	// ��ȿ�� �˻�
	{
		if ( ! g_pFramework->CheckValidNickname(m_szChangeName) )
		{
			//	�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�"
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
			ret = FALSE;
			goto LABEL_CHECK_NAME_EXIT;
		}
	}

LABEL_CHECK_NAME_EXIT:
	//	�⺻ �˻�
	if( ret)
	{
		switch( m_Type)
		{
		case USE_ITEM_DISGUISE_NICK:
		case USE_ITEM_CHANGE_NICK:			g_pGameContext->SetEvent(EVENT_USE_ITEM_CHECK_NICK, m_szChangeName);		break;
		case USE_ITEM_CHANGE_CLAN_NAME:		g_pFramework->GetClanContext()->SendOverlappingClanName(m_szChangeName);	break;
		}

		return TRUE;
	}
	else
	{
		//m_pChangeNameUserEdit->SetIMEText( "");
		m_pChangeNameUserEdit->SetFocus(TRUE);

		m_pChangeNameCheckStatic->SetText(szError);
		m_pChangeNameCheckStatic->SetTextColor(GetColor(GCT_DEFAULT));

		return FALSE;
	}
}

void CPopupUseItem::OnOpenChangeNick(void)
{
	if( !_Open(USE_ITEM_CHANGE_NICK))
		OnExitKey();
}

void CPopupUseItem::CbChangeNickWarnning(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		((CPopupUseItem*)pThis)->OnOpenChangeNick();
	}
	else
	{
		((CPopupUseItem*)pThis)->OnCancel();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_CHANGE_CLAN_NAME

void CPopupUseItem::_InitChangeClanNamePopup(void)
{
	// ������
	_SetCurrentUseItemName( m_pChangeNameItemValue);

	m_pChangeNameBeforeStatic->SetText(GAME_STRING("STR_POPUP_CURRNET_CLAN_NAME"));/*���� Ŭ����:*/

	// Ŭ����
	char * pszName = g_pGameContext->getMyClanDetailInfo()->_name;
	m_pChangeNameBeforeValue->SetText(pszName);

	m_pChangeNameUserStatic->SetText(GAME_STRING("STR_POPUP_MODIFIED_CLAN_NAME"));/*���� Ŭ����:*/

	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NET_CLAN_NAME_SIZE - 1;
	ime._StyleIME = 0;
	m_pChangeNameUserEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pChangeNameUserEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pChangeNameUserEdit->SetIMEText( "");
	m_pChangeNameUserEdit->SetFocus(TRUE);

	m_pChangeNameCheckStatic->SetTextColor( GetColor(GCT_DEFAULT));
	m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*�ߺ��˻縦 ���ֽʽÿ�.*/

	m_pChangeNameNoticeValue->SetText(GAME_STRING("STR_POPUP_MESSAGE_INPUT_MODIFY_CLAN_NAME"));/*������ Ŭ������ �Է����ֽʽÿ�.*/

	DEACTIVATE_BUTTON(m_pChangeNameButtonOk);

	m_bChangeNameChecked = FALSE;

	m_szChangeName[0] = 0;

	if(m_pStringFilter == NULL)
	{
		m_pStringFilter = CStringFilter::NewObject();
		m_pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	}

	if(m_pStringFilter2 == NULL)
	{
		m_pStringFilter2 = CStringFilter::NewObject();
		m_pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);
	}
}

void CPopupUseItem::_ChangeClanNameOk(void)
{
	_SetInputLock(TRUE);

	//	������ ���� ������ �г����� ������.
	g_pFramework->GetClanContext()->ProcessChangeClanName(m_AuthWareDBIndex, m_szChangeName);
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_DISGUISE_NICK

void CPopupUseItem::_InitDisguiseNickPopup(void)
{
	// ������
	_SetCurrentUseItemName( m_pChangeNameItemValue);

	m_pChangeNameBeforeStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_NICK_NAME"));/*���� �г���:*/

	// �г���
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	SetNameUsingColorTable(m_pChangeNameBeforeValue, MyUserInfoBasic._nick, MyUserInfoBasic._nickcolor);

	m_pChangeNameUserStatic->SetText(GAME_STRING("STR_POPUP_DISGUISED_NICK"));/*�ӽ� �г���:*/

	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
	m_pChangeNameUserEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pChangeNameUserEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pChangeNameUserEdit->SetIMEText( "");
	m_pChangeNameUserEdit->SetFocus(TRUE);

	m_pChangeNameCheckStatic->SetTextColor( GetColor(GCT_DEFAULT));
	m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*�ߺ��˻縦 ���ֽʽÿ�.*/

	m_pChangeNameNoticeValue->SetText(GAME_STRING("STR_POPUP_MESSAGE_INPUT_DISGUISED_NICK_2"));/*�ӽ� �г����� �Է����ֽʽÿ�.*/

	DEACTIVATE_BUTTON(m_pChangeNameButtonOk);

	m_bChangeNameChecked = FALSE;

	m_szChangeName[0] = 0;

	if(m_pStringFilter == NULL)
	{
		m_pStringFilter = CStringFilter::NewObject();
		m_pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	}

	if(m_pStringFilter2 == NULL)
	{
		m_pStringFilter2 = CStringFilter::NewObject();
		m_pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);
	}
}

void CPopupUseItem::_ChangeDisguiseNickOk(void)
{
	_SetInputLock(TRUE);

	//	������ �ۼ��� �ӽ� �г����� ������.
	UINT8 ui8Size = static_cast<UINT8>(i3String::Length( m_szChangeName )+1);
	g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, m_szChangeName);
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_DISGUISE_RANK

void CPopupUseItem::_InitDisguiseRankPopup(void)
{
	// ������
	_SetCurrentUseItemName( m_pChangeRankItemValue);

	m_pChangeRankCurrentStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_RANK"));/*���� ���:*/

	// ���� ���
	const char * pszCurRank = GetCharaRank(g_pGameContext->GetMyRank());
	m_pChangeRankCurrentValue->SetText(pszCurRank);

	m_pChangeRankSelectStatic->SetText(GAME_STRING("STR_POPUP_DISGUISE_RANK"));/*���� ���:*/
	const char * pszSelected = m_pChangeRankSelectList->GetText( m_pChangeRankSelectList->getSelectedElement());
	m_pChangeRankSelectCombo->SetText( pszSelected);
}

void CPopupUseItem::_ChangeDisguiseRankOk(void)
{
	switch( m_pChangeRankSelectList->getSelectedElement())
	{
	case 0:		m_nSelected = 0;		break;	//	�Ʒú�
	case 1:		m_nSelected = 1;		break;	//	�̺�
	case 2:		m_nSelected = 2;		break;	//	�Ϻ�
	case 3:		m_nSelected = 3;		break;	//	��
	case 4:		m_nSelected = 4;		break;	//	����
	default:	m_nSelected = -1;		break;
	}

	_SetInputLock(TRUE);

	//	���� ����� ������.
	UINT8 ui8Size = sizeof(INT32);
	g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &m_nSelected);
}

void CPopupUseItem::OnChangeRankPopupButton(void)
{
	ExclusionPopup(m_pChangeRankSelectList);
}

void CPopupUseItem::OnChangeRankPopupList(UINT32 idxItem)
{
	char * pszSelected = (char*) m_pChangeRankSelectList->GetText( m_pChangeRankSelectList->getSelectedElement());
	m_pChangeRankSelectCombo->SetText( pszSelected);
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_COLOR_NICK

void CPopupUseItem::_InitColorNickPopup(void)
{
	m_pChangeColorCurrentStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_COLOR"));/*���� ����:*/
	m_pChangeColorSelectStatic->SetText(GAME_STRING("STR_POPUP_MODIFIED_COLOR"));/*���� ����:*/
	m_pChangeColorApplyStatic->SetText(GAME_STRING("STR_POPUP_APPLIED_NICK_NAME"));/*���� �г���:*/

	_SetCurrentUseItemName( m_pChangeColorItemValue);

	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	SetNameUsingColorTable(m_pChangeColorApplyValue, MyUserInfoBasic._nick, MyUserInfoBasic._nickcolor);

	m_pChangeColorCurrentColorBox->SetEnable( FALSE);
	m_pChangeColorSelectColorBoxFrame->SetEnable( FALSE);

	for(INT32 i=0; i<SELECT_COLOR_BOX_COUNT; i++)
	{
		m_pChangeColorSelectColorBox[ i]->SetEnable( TRUE);
		m_pChangeColorSelectColorBox[ i]->SetAllShapeEnable(FALSE);
		m_pChangeColorSelectColorBox[ i]->SetShapeEnable( i, TRUE);
	}

	INT8 idxColor = MyUserInfoBasic._nickcolor;
	if( idxColor == 0)
		OnChangeColorSelect( -1);	//	�⺻ ���
	else
		OnChangeColorSelect( idxColor-1);	//	������ 1���� ����, GUI�� 0���� ����
}

void CPopupUseItem::_ChangeColorNickOk(void)
{
	if( m_nSelected == -1)
	{
		STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));/*������ ������ ������ �ֽʽÿ�.*/
	}
	else
	{
		INT32 idx = m_nSelected + 1;	//	GUI�� -1 ���̽��̸�, ������ 0 ���̽��̴�. �����Ұ�!

		_SetInputLock(TRUE);

		UINT8 ui8Size = sizeof(INT32);
		g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &idx);
	}
}

void CPopupUseItem::OnChangeColorSelect(INT32 idx)
{
	I3ASSERT( idx >= -1 && idx < SELECT_COLOR_BOX_COUNT);	//	-1���� ���, 0~9������ ����

	I3COLOR * pSelectColor = NULL;

	m_nSelected = idx;

	if(m_nSelected == -1)
	{
		//	���õ��� ���� ���� �Ǵ� �⺻ ����
		m_pChangeColorSelectColorBoxFrame->SetEnable( FALSE);
		m_pChangeColorCurrentColorBox->SetEnable( FALSE);

		//	�⺻ �÷�
		pSelectColor = (I3COLOR*)GetColorCashItem(0);
	}
	else
	{
		//	���õ� �÷� �ڽ��� ��ġ�� ���� Ŀ���� �����.
		INT32 idxColor = m_nSelected+1;	//	idxColor�� 1 base�̴�.
		REAL32 slotPos = m_pChangeColorSelectColorBox[ m_nSelected]->getPositionX();

		REAL32 offsetWidth = (REAL32) (m_pChangeColorSelectColorBoxFrame->getOriginalWidth() -
			m_pChangeColorSelectColorBox[ m_nSelected]->getOriginalWidth()) * 0.5f;

		m_pChangeColorSelectColorBoxFrame->SetEnable( TRUE);
		m_pChangeColorSelectColorBoxFrame->setPositionX( slotPos - offsetWidth);

		m_pChangeColorCurrentColorBox->SetEnable( TRUE);
		m_pChangeColorCurrentColorBox->SetAllShapeEnable(FALSE);
		m_pChangeColorCurrentColorBox->SetShapeEnable( m_nSelected, TRUE);

		//	���� �÷�
		pSelectColor = (I3COLOR*)GetColorCashItem(idxColor);
	}

	m_pChangeColorApplyValue->SetTextColor( pSelectColor);
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_COLOR_CLAN_NAME

void CPopupUseItem::_InitColorClanNamePopup(void)
{
	m_pChangeColorCurrentStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_COLOR"));/*���� ����:*/
	m_pChangeColorSelectStatic->SetText(GAME_STRING("STR_POPUP_MODIFIED_COLOR"));/*���� ����:*/
	m_pChangeColorApplyStatic->SetText(GAME_STRING("STR_POPUP_APPLIED_CLAN_NAME"));/*���� Ŭ����:*/

	_SetCurrentUseItemName( m_pChangeColorItemValue);

	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	SetNameUsingColorTable( m_pChangeColorApplyValue, MyUserInfoBasic._clanName, MyUserInfoBasic._clanNamecolor );

	INT8 idxColor = MyUserInfoBasic._clanNamecolor;
	if( idxColor == 0)
		OnChangeColorSelect( -1);	//	�⺻ ���
	else
		OnChangeColorSelect( idxColor-1);	//	������ 1���� ����, GUI�� 0���� ����
}

void CPopupUseItem::_ChangeColorClanNameOk(void)
{
	if( m_nSelected == -1)
	{
		STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));/*������ ������ ������ �ֽʽÿ�.*/
	}
	else
	{
		INT32 idx = m_nSelected + 1;	//	GUI�� -1 ���̽��̸�, ������ 0 ���̽��̴�. �����Ұ�!

		_SetInputLock(TRUE);

		UINT8 ui8Size = sizeof(INT32);
		g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &idx);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_COLOR_CROSSHAIR

void CPopupUseItem::_InitColorCrosshairPopup(void)
{
	m_pChangeCrosshairColorCurrentStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_COLOR"));/*���� ����:*/
	m_pChangeCrosshairColorSelectStatic->SetText(GAME_STRING("STR_POPUP_MODIFIED_COLOR"));/*���� ����:*/

	_SetCurrentUseItemName( m_pChangeCrosshairColorItemValue);

	USER_INFO_INVITEM_DATA Data;
	g_pGameContext->GetMyUserInfoInvItemData(&Data);

	INT8 idxColor = Data._cColorCrossHair;
	if( idxColor == 0)
		OnChangeCrosshairColorSelect( -1);	//	�⺻ ���
	else
		OnChangeCrosshairColorSelect( idxColor-1);	//	������ 1���� ����, GUI�� 0���� ����
}

void CPopupUseItem::_ChangeColorCrosshairOk(void)
{
	if( m_nSelected == -1)
	{
		STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));/*������ ������ ������ �ֽʽÿ�.*/
	}
	else
	{
		INT32 idx = m_nSelected + 1;	//	GUI�� -1 ���̽��̸�, ������ 0 ���̽��̴�. �����Ұ�!

		_SetInputLock(TRUE);

		UINT8 ui8Size = sizeof(INT32);
		g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &idx);
	}
}

void CPopupUseItem::OnChangeCrosshairColorSelect(INT32 idx)
{
	I3ASSERT( idx >= -1 && idx < SELECT_CROSSHAIR_COLOR_BOX_COUNT);	//	-1���� ���, 0~9������ ����

	I3COLOR * pSelectColor = NULL;

	m_nSelected = idx;

	if(m_nSelected == -1)
	{
		//	���õ��� ���� ���� �Ǵ� �⺻ ����
		m_pChangeCrosshairColorSelectColorBoxFrame->SetEnable( FALSE);
		m_pChangeCrosshairColorCurrentColorBox->SetEnable( FALSE);

		//	�⺻ �÷�
		pSelectColor = (I3COLOR*)GetColorCashItem(0);
	}
	else
	{
		//	���õ� �÷� �ڽ��� ��ġ�� ���� Ŀ���� �����.
		INT32 idxColor = m_nSelected+1;	//	idxColor�� 1 base�̴�.

		REAL32 slotPos = m_pChangeCrosshairColorSelectColorBox[ m_nSelected]->getPositionX();

		REAL32 offsetWidth = (REAL32) (m_pChangeCrosshairColorSelectColorBoxFrame->getOriginalWidth() -
			m_pChangeCrosshairColorSelectColorBox[ m_nSelected]->getOriginalWidth()) * 0.5f;

		m_pChangeCrosshairColorSelectColorBoxFrame->SetEnable( TRUE);
		m_pChangeCrosshairColorSelectColorBoxFrame->setPositionX( slotPos - offsetWidth);

		m_pChangeCrosshairColorCurrentColorBox->SetEnable( TRUE);
		m_pChangeCrosshairColorCurrentColorBox->SetAllShapeEnable(FALSE);
		m_pChangeCrosshairColorCurrentColorBox->SetShapeEnable( m_nSelected, TRUE);

		//	���� �÷�
		pSelectColor = (I3COLOR*)GetColorCashItem(idxColor);
	}

	//	ũ�ν���� �̹��� ���� ����
	m_pChangeCrosshairColorCrossImage->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
	m_pChangeCrosshairColorCrossDotImage->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
	m_pChangeCrosshairColorDotImage->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
	m_pChangeCrosshairColorCircleImage->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
}


#endif	// ENABLE_UI2

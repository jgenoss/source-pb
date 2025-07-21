#include "pch.h"
#include "StepProfile_CardShopWin.h"
#include "GuiNotifyReceiver.h"
#include "QuestTextDictionary.h"

I3_CLASS_INSTANCE( CStepProfile_CardShopWin, i3ElementBase);

CStepProfile_CardShopWin::CStepProfile_CardShopWin()
{
	m_pNotifyBase = NULL;

	m_pCardShopWin = NULL;
	m_pCardShopCaption = NULL;
	m_pCardShopConfirm = NULL;
	m_pCardShopCancel = NULL;
	m_pCardShopName = NULL;
	m_pCardShopPrice = NULL;
	m_pCardShopCurrentKey = NULL;
	m_pCardShopCurrentValue = NULL;
	m_pCardShopRemainderKey = NULL;
	m_pCardShopRemainderValue = NULL;
	m_pCardShopOneDummy = NULL;
	m_pCardShopSetDummy = NULL;
	m_pCardShopNotice = NULL;
}

CStepProfile_CardShopWin::~CStepProfile_CardShopWin()
{
}

BOOL CStepProfile_CardShopWin::LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl)
{
	m_pNotifyBase = pBase;

	LinkControlEx(plist, 		"prCardShopWin",			m_pCardShopWin, -1);
	LinkControlEx(plist, 		"prCardShopCaption",		m_pCardShopCaption, -1);
	LinkControlEx(plist, 		"prCardShopConfirm",		m_pCardShopConfirm, GCI_PRS_B_SHOP_CONFIRM);
	LinkControlEx(plist, 		"prCardShopCancel",			m_pCardShopCancel, GCI_PRS_B_SHOP_CANCEL);
	LinkControlEx(plist, 		"prCardShopName",			m_pCardShopName, -1);
	LinkControlEx(plist, 		"prCardShopName",			m_pCardShopName, -1);
	LinkControlEx(plist, 		"prCardShopPrice",			m_pCardShopPrice, -1);
	LinkControlEx(plist, 		"prCardShopCurrentKey",		m_pCardShopCurrentKey, -1);
	LinkControlEx(plist, 		"prCardShopCurrentValue",	m_pCardShopCurrentValue, -1);
	LinkControlEx(plist, 		"prCardShopRemainderKey",	m_pCardShopRemainderKey, -1);
	LinkControlEx(plist, 		"prCardShopRemainderValue", m_pCardShopRemainderValue, -1);
	LinkControlEx(plist, 		"prCardShopOneDummy",		m_pCardShopOneDummy, -1);
	LinkControlEx(plist, 		"prCardShopSetDummy",		m_pCardShopSetDummy, -1);
	LinkControlEx(plist, 		"prCardShopNotice",			m_pCardShopNotice, -1);

	return TRUE;
}

void CStepProfile_CardShopWin::Init( void)
{
	// ������ �ʱ�ȭ
	m_pCardShopWin->SetEnable(FALSE);

	{
		INIT_WIN_CAPTION2(m_pCardShopCaption, GAME_STRING("STR_TBL_PROFILE_NOTICE_TEXT"), GetColor(GCT_DEFAULT));/*�˸�*/
	}

	// ��ư �ʱ�ȭ
	INIT_BUTTON_EX(m_pCardShopConfirm);
	m_pCardShopConfirm->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
	m_pCardShopConfirm->SetCaption(	GAME_STRING("STR_TBL_PROFILE_CARD_SHOP_CONFIRM"));	/*����Ȯ��*/
	m_pCardShopConfirm->SetCaptionColor( GetColor(GCT_VALUE));

	INIT_BUTTON_EX(m_pCardShopCancel);
	m_pCardShopCancel->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
	m_pCardShopCancel->SetCaption( GAME_STRING("STR_TBL_PROFILE_CARD_SHOP_CANCEL"));/*�������*/
	m_pCardShopCancel->SetCaptionColor( GetColor(GCT_VALUE));
	

	// ����Ʈ�ڽ� �ʱ�ȭ	
	m_pCardShopName->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardShopPrice->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardShopCurrentKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardShopCurrentValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardShopRemainderKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardShopRemainderValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardShopNotice->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	m_pCardShopName->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardShopPrice->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardShopCurrentKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardShopCurrentValue->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardShopRemainderKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardShopRemainderValue->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardShopNotice->SetTextColor( GetColor(GCT_DEFAULT));

	m_pCardShopCurrentKey->SetText( GAME_STRING("STR_TBL_PROFILE_HOLD_POINT"));/*���� Point :*/
	m_pCardShopRemainderKey->SetText( GAME_STRING("STR_TBL_PROFILE_REMAIN_MONEY"));/*�ܾ� :*/
    

	// ī��(��ǰ)
	m_pCardShopOneDummy->SetEnable(FALSE);
	m_pCardShopSetDummy->SetEnable(FALSE);
}

void CStepProfile_CardShopWin::Update( REAL32 tm)
{
}

void CStepProfile_CardShopWin::GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify)
{
}

//void CStepProfile_CardShopWin::OnCardBuyOne( UINT8 cardSetId, UINT8 cardIndex)
//{
//	char szText[ MAX_STRING_COUNT];
//
//	// ����ī�� ���
//	g_pFramework->SetCardShape(m_pCardShopOneDummy, cardSetId, cardIndex);
//	m_pCardShopOneDummy->SetEnable(TRUE);
//	m_pCardShopSetDummy->SetEnable(FALSE);
//
//	// ī�� �̸�
//	g_pQuestTextDictionary->GetCardNameString( szText, cardSetId, cardIndex);
//	m_pCardShopName->SetText( szText);	
//
//	// ���� ���
//	UINT32 cardPricePoint = g_QuestCardBuyPoint[cardSetId][cardIndex];
//	UINT32 myCurrentPoint = g_pGameContext->GetMyPoint();
//
//	// ī�尡��
//	sprintf_s( szText, "%u %s", cardPricePoint, GAME_STRING("STBL_IDX_SHOP_MONEY"));
//	m_pCardShopPrice->SetText( szText);
//
//	// �����ݾ�
//	sprintf_s( szText, "%u %s", myCurrentPoint, GAME_STRING("STBL_IDX_SHOP_MONEY"));
//	m_pCardShopCurrentValue->SetText( szText);
//
//    // ������ ���� ����
//	INT32 remainPoint = (signed)myCurrentPoint - (signed)cardPricePoint;
//
//	if (0 <= remainPoint)
//	{
//		sprintf_s( szText, "{col:151, 211, 50, 255}%d{/col} %s", remainPoint, GAME_STRING("STBL_IDX_SHOP_MONEY"));		
//
//		// activate button
//		{
//			m_pCardShopConfirm->SetEnable(TRUE);
//			m_pCardShopConfirm->OnUpdate(__RT_ZERO);
//			m_pCardShopConfirm->setInputDisable(FALSE);
//			m_pCardShopConfirm->SetCaptionColor( GetColor(GCT_VALUE));
//		}
//	}
//	else
//	{
//		sprintf_s( szText, "{col:255, 48, 0, 255}%d{/col} %s", remainPoint, GAME_STRING("STBL_IDX_SHOP_MONEY"));
//
//		// deactivate button
//		{
//			m_pCardShopConfirm->SetEnable(TRUE);
//			m_pCardShopConfirm->OnUpdate(__RT_ZERO);
//			m_pCardShopConfirm->setInputDisable(TRUE);
//			m_pCardShopConfirm->SetCaptionColor( GetColor(GCT_STATIC));
//		}
//	}
//
//	m_pCardShopRemainderValue->SetText( szText);
//
//	// ���� ��� ���
//	m_pCardShopNotice->SetText( GAME_STRING("STR_TBL_PROFILE_CARD_BUY_ONE_MESSAGE"));/*���ñ��� ���� �� �����Ͻ� �ش� �ӹ�ī�带\n��� �����ϰԵǸ�, ������ �����Ͻ�\n������ ������ ī��� ������� �˴ϴ�.*/
//}

void CStepProfile_CardShopWin::OnCardBuyAll( UINT8 cardSetId)
{
	char szText[ MAX_STRING_COUNT];

	// ����ī�� ���
	g_pFramework->SetCardSetShape(m_pCardShopSetDummy, cardSetId);
	m_pCardShopOneDummy->SetEnable(FALSE);
	m_pCardShopSetDummy->SetEnable(TRUE);

	// ī�� �̸�
	g_pQuestTextDictionary->GetCardSetNameString( szText, MAX_STRING_COUNT, cardSetId);
	m_pCardShopName->SetText( szText);


	// ���� ���
	UINT32 cardPricePoint = g_QuestCardSetBuyPoint[cardSetId];
	UINT32 myCurrentPoint = g_pGameContext->GetMyPoint();

	// ī�尡��
	sprintf_s( szText, "%u %s", cardPricePoint, GAME_STRING("STBL_IDX_SHOP_MONEY"));
	m_pCardShopPrice->SetText( szText);

	// �����ݾ�
	sprintf_s( szText, "%u %s", myCurrentPoint, GAME_STRING("STBL_IDX_SHOP_MONEY"));
	m_pCardShopCurrentValue->SetText( szText);

    // ������ ���� ����
	INT32 remainPoint = (signed)myCurrentPoint - (signed)cardPricePoint;

	if (0 <= remainPoint)
	{
		sprintf_s( szText, "{col:151, 211, 50, 255}%d{/col} %s", remainPoint, GAME_STRING("STBL_IDX_SHOP_MONEY"));

		// activate button
		{
			m_pCardShopConfirm->SetEnable(TRUE);
			m_pCardShopConfirm->OnUpdate(__RT_ZERO);
			m_pCardShopConfirm->setInputDisable(FALSE);
			m_pCardShopConfirm->SetCaptionColor( GetColor(GCT_VALUE));
		}
	}
	else
	{
		sprintf_s( szText, "{col:255, 48, 0, 255}%d{/col} %s", remainPoint, GAME_STRING("STBL_IDX_SHOP_MONEY"));
		
		// deactivate button
		{
			m_pCardShopConfirm->SetEnable(TRUE);
			m_pCardShopConfirm->OnUpdate(__RT_ZERO);
			m_pCardShopConfirm->setInputDisable(TRUE);
			m_pCardShopConfirm->SetCaptionColor( GetColor(GCT_STATIC));
		}
	}

	m_pCardShopRemainderValue->SetText( szText);

	// ���� ��� ���
	m_pCardShopNotice->SetText( GAME_STRING("STR_TBL_PROFILE_CARD_BUY_EXPLAIN"));/*��ü���� ���� �� �ش� �ӹ�ī�带 ���\n�����ϰ� �Ǹ�, ������ �����Ͻ� ������\n������ ī��� ��� ������� �˴ϴ�.*/
}
	

#include "pch.h"
#include "StepShop_TabWeapon.h"
#include "StepShop.h"
#include "GuiNotifyReceiver.h"
#include "StageGui/Popup/PopupExtension.h"


CRepairFrameWeapon::CRepairFrameWeapon(CTabBase * pOwner)
: CRepairFrame(pOwner)
{
}

void	CRepairFrameWeapon::OnLinkControl(i3BinList * plist)
{
	LinkControlEx(plist, "shopWeaponRepairFrame", m_pFrame, -1);
	LinkControlEx(plist, "shopWeaponDoRepair", m_pDoRepair, GCI_SHOP_B_REPAIR_WEAPON);


	LinkControlEx(plist, "shopWeaponRepair", m_pArrow, -1);
	LinkControlEx(plist, "shopWeaponRepairText", m_pRepairText, -1);
	LinkControlEx(plist, "shopWeaponRepairPrice", m_pRepairPrice, -1);
	LinkControlEx(plist, "shopWeaponPriceType", m_pRepairPriceType, -1);

	LinkControlEx(plist, "shopWeaponRepairHaveMoneyStatic", m_pHaveMoneyText, -1);
	LinkControlEx(plist, "shopWeaponRepairHaveMoneyValue", m_pHaveMoneyValue, -1);
	LinkControlEx(plist, "shopWeaponRepairHaveMoneyType", m_pHaveMoneyType, -1);

	LinkControlEx(plist, "shopWeaponRepairRemainMoneyStatic", m_pRemainMoneyText, -1);
	LinkControlEx(plist, "shopWeaponRepairRemainMoneyValue", m_pRemainMoneyValue, -1);
	LinkControlEx(plist, "shopWeaponRepairRemainMoneyType", m_pRemainMoneyType, -1);
}


void	CRepairFrameWeapon::Init()
{
	char	temp[256];
	m_pFrame->SetEnable(FALSE);

	INIT_CAPTION_BUTTON(m_pDoRepair, GAME_STRING("STR_SHOP_DO_REPAIR"));
	ACTIVATE_BUTTON(m_pDoRepair);

	m_pRepairText->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRepairText->SetTextColor(GetColor(GCT_DEFAULT));
	m_pRepairText->SetText(GAME_STRING("STR_POPUP_REPAIR_PRICE"));

	m_pRepairPrice->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRepairPrice->SetTextColor(GetColor(GCT_DEFAULT));

	m_pRepairPriceType->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRepairPriceType->SetTextColor(GetColor(GCT_DEFAULT));


	m_pHaveMoneyText->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pHaveMoneyText->SetTextColor(GetColor(GCT_DEFAULT));
	sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_SHOP_HAVE_MONEY"));
	m_pHaveMoneyText->SetText(temp);

	m_pHaveMoneyValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pHaveMoneyValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pHaveMoneyValue->SetText("0");

	m_pHaveMoneyType->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pHaveMoneyType->SetTextColor(GetColor(GCT_DEFAULT));
	m_pHaveMoneyType->SetText(INVALID_TEXT);


	m_pRemainMoneyText->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRemainMoneyText->SetTextColor(GetColor(GCT_DEFAULT));
	sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_SHOP_REMAIN_MONEY"));
	m_pRemainMoneyText->SetText(temp);


	m_pRemainMoneyValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRemainMoneyValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pRemainMoneyValue->SetText("0");

	m_pRemainMoneyType->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRemainMoneyType->SetTextColor(GetColor(GCT_DEFAULT));
	m_pRemainMoneyType->SetText(INVALID_TEXT);
}

void	CRepairFrameWeapon::Update(RT_REAL32 rDeltaSec)
{
	m_pRepairText->SetTextColor(GetColor(GCT_DEFAULT));
	m_pRepairPrice->SetTextColor(GetColor(GCT_DEFAULT));
	m_pRepairPriceType->SetTextColor(GetColor(GCT_DEFAULT));

	DEACTIVATE_BUTTON(m_pDoRepair);
	m_pRepairPrice->SetText("0");


	SHOP_GOODS_PACK * pGoodsPack = m_pOwner->GetFocusGoodsPack();
	if ( !pGoodsPack )
		return;

	SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
	if( pGoods == NULL)
		return;

	CShopContext * pShopContext = g_pFramework->GetShopContext();

	UINT32 Current = 0, Max = 0;
	if ( ! pShopContext->GetDurability(pGoods->_ItemID, Current, Max) )
	{
		char	Msg[512] = {};
		i3String::Format(Msg, 512, "ID : %d, Current : %d, Max : %d\n", pGoods->_ItemID, Current, Max);
		I3FATAL_ONCE(Msg);
		return;
	}


	UINT32	Price = 0;
	GOODS_PRICE_TYPE PriceType;
	if ( ! pShopContext->GetRepairPrice(pGoods->_ItemID, Price, PriceType ) )
	{
		char	Msg[512] = {};
		i3String::Format(Msg, 512, "ID : %d, Price : %d, PriceType : %d\n", pGoods->_ItemID, Price, PriceType);
		I3FATAL_ONCE(Msg);
		return;
	}

	// ���� ����
	char	Str[256] ={};
	StringCchPrintf(Str, 256, "%d", Price);
	m_pRepairPrice->SetText(Str);

	// �� ����
	CopyPriceTypeText( PriceType, Str, 256 );
	m_pRepairPriceType->SetText(Str);
	m_pHaveMoneyType->SetText(Str);
	m_pRemainMoneyType->SetText(Str);

	REPAIR_TYPE	Rv = pShopContext->CheckCanRepair(pGoods->_ItemID);

	if ( Rv == REPAIR_TYPE_CAN_SOME )
	{
		m_pRepairText->SetTextColor(GetColor(GCT_RED));
		m_pRepairPrice->SetTextColor(GetColor(GCT_RED));
		m_pRepairPriceType->SetTextColor(GetColor(GCT_RED));
	}


	// �ܾ� ǥ��
	INT32 nHavePoint = 0, nRemainPoint = 0;
	if( PriceType != GOODS_PRICE_UNKNOWN)
	{
		switch(PriceType)
		{
		case GOODS_PRICE_POINT:
			{
				nHavePoint = g_pGameContext->GetMyPoint();
				nRemainPoint = nHavePoint - Price;
			}
			break;
		case GOODS_PRICE_CASH:
			{
				nHavePoint = g_pGameContext->GetMyCash();
				nRemainPoint = nHavePoint - Price;
			}
			break;
		default:
			{
				nHavePoint = g_pGameContext->GetMyPoint();
				nRemainPoint = nHavePoint - Price;
			}
			break;
		}
	}

		//	���� ����Ʈ
	sprintf_s(Str, "%d", nHavePoint);
	m_pHaveMoneyValue->SetText(Str);

	//	�ܾ�
	sprintf_s(Str, "%d", nRemainPoint);
	m_pRemainMoneyValue->SetText(Str);


	// �����ϱ� ��ư Ȱ��ȭ
	// ���� �������� �ִ�ġ�� ����.
	if ( Current < Max )
	{
		ACTIVATE_BUTTON(m_pDoRepair);
	}

}



CTabWeapon::CTabWeapon(CStepShop * pOwner)
: CTabBase(MAX_SLOT_FOR_SHOP_WEAPON, SLOT_HORI_COUNT_WEAPON, pOwner)
, m_RepairFrame(this)
{
	// weapon initialize
	i3mem::FillZero(m_Slot, sizeof(m_Slot));
	i3mem::FillZero(m_weaponInfo, sizeof(m_weaponInfo));

	m_pAllGroup			= NULL;
	m_pPrimaryGroup		= NULL;
	m_pSecondaryGroup	= NULL;
	m_pMeleeGroup		= NULL;
	m_pThrowingGroup	= NULL;
	m_pItemGroup		= NULL;

	m_pDetailWeapon		= NULL;
	m_pPreviewWeapon	= NULL;

	m_pBulletKey		= NULL;
	m_pBulletValue		= NULL;

	m_pRangeKey			= NULL;
	m_pRangeValue		= NULL;

	m_pInfo				= NULL;
	m_pImage			= NULL;
	m_pInfoKey			= NULL;
	m_pInfoValue		= NULL;
	m_pInfoTerm			= NULL;
	m_pInfoPurchase		= NULL;
	m_pInfoExtension	= NULL;
	m_pGoodsFrame		= NULL;


	m_pBuyWeaponCombo1	= NULL;
	m_pBuyWeaponCombo2	= NULL;

	// ������ -2��� ������ ���� ä��ϴ�.
	// ������ OnChangeGroup ���� ���ٸ� �������ϰ� �ǰ�
	// �ʱ� ���õǴ� �׷�â�� ALL�Դϴ�.
	// �׷��� WEAPON_SLOT_UNKNOWN�� WEAPON_SLOT_UNKNOWN�� ���� ���� ������ �߻��ϱ� ������
	// -2�� ������ �����մϴ�.
	m_nWeaponUsageType		= (WEAPON_SLOT_TYPE)-2;
	m_nCurViewWeaponID	= 0;
}


CTabWeapon::~CTabWeapon()
{
}

void	CTabWeapon::Init()
{
	CTabBase::Init();

	m_RepairFrame.Init();

	char temp[MAX_STRING_COUNT] = "";

	INIT_CAPTION_BUTTON(m_pAllGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_ALL"));
	INIT_CAPTION_BUTTON(m_pPrimaryGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_PRIMARY"));
	INIT_CAPTION_BUTTON(m_pSecondaryGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_SECONDARY"));
	INIT_CAPTION_BUTTON(m_pMeleeGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_MELEE"));
	INIT_CAPTION_BUTTON(m_pThrowingGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_THROWING"));
	INIT_CAPTION_BUTTON(m_pItemGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_SPECIAL"));
	
	m_pAllGroup->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_pScrollList->CreateTextEx(SLOT_VERT_COUNT_WEAPON,  GetDefaultFontName());
	m_pScrollList->SetSelColor(0, 0, 0, 0);

	for(UINT32 i = 0; i < MAX_SLOT_FOR_SHOP_WEAPON ; i++)
	{
		INIT_BUTTON_EX(m_Slot[i]._pBg);

		m_Slot[i]._pKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pValue->SetTextSpace(0, -3);
		m_Slot[i]._pTerm->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pTerm->SetTextColor(GetColor(GCT_DEFAULT));
		m_Slot[i]._pPurchase->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pPurchase->SetTextColor(GetColor(GCT_RED));
		m_Slot[i]._pKey->SetTextColor(GetColor(GCT_DEFAULT));
		m_Slot[i]._pValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_Slot[i]._pImage->SetEnable(FALSE);
		m_Slot[i]._pExtension->SetEnable(FALSE);
		m_Slot[i]._pUsed->SetEnable(FALSE);
		m_Slot[i]._pSaleMark->SetEnable(FALSE);
		m_Slot[i]._pSaleMark->SetAllShapeEnable(FALSE);

		m_Slot[i]._pSaleText[SALE_TEXT_FIRST]->setInputDisable(TRUE);
		m_Slot[i]._pSaleText[SALE_TEXT_SECOND]->setInputDisable(TRUE);
		m_Slot[i]._pSaleText[SALE_TEXT_FIRST]->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pSaleText[SALE_TEXT_SECOND]->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pSaleText[SALE_TEXT_FIRST]->SetEnable(FALSE);
		m_Slot[i]._pSaleText[SALE_TEXT_SECOND]->SetEnable(FALSE);

		I3COLOR * pColor = (I3COLOR*)&g_pConfigEx->GetGUI().DurabilityBarColorInSlot;
		m_Slot[i]._pDurabilityGauge->SetColor(I3GUI_CONTROL_STATE_NORMAL, pColor);
	}

	m_pInfo->SetAllShapeEnable(FALSE);
	m_pInfo->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pImage->SetEnable(FALSE);
	m_pInfoKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInfoKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pInfoValue->SetTextColor(GetColor(GCT_DEFAULT));

	INIT_CAPTION_BUTTON(m_pDetailWeapon, GAME_STRING("STBL_IDX_SHOP_DETAIL"));
	INIT_CAPTION_BUTTON(m_pPreviewWeapon, GAME_STRING("STBL_IDX_SHOP_PREVIEW"))
	INIT_CAPTION_BUTTON(m_PriceFrame.pBuy, GAME_STRING("STBL_IDX_SHOP_BUY"));
	INIT_CAPTION_BUTTON(m_PriceFrame.pGift, GAME_STRING("STBL_IDX_SHOP_GIFT"));

	ACTIVATE_BUTTON(m_PriceFrame.pBuy);

#if defined USE_SHOP_GIFT
	ACTIVATE_BUTTON(m_PriceFrame.pBuy);
#else
	//DEACTIVATE_BUTTON(m_PriceFrame.pGift);	// ��ư ��Ȱ��ȭ
	m_PriceFrame.pGift->SetEnable(FALSE);
#endif

	for(UINT32 i = 0; i < MAX_SHOP_WEAPON_INFO; i++)
	{
#if defined (LOCALE_THAILAND) || defined(GUI_ADJUST_RUSSIA)
		m_weaponInfo[i]._pKey->ReCreateTextEx( GetDefaultFontName() , 32, GAME_FONT_SIZE_ANY_TEXT, FALSE, 0, 0, TRUE, FONT_SHADOW_QUALITY_LOW);
#endif

		m_weaponInfo[i]._pKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponInfo[i]._pValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponInfo[i]._pKey->SetTextColor(GetColor(GCT_DEFAULT));
		m_weaponInfo[i]._pValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_weaponInfo[i]._pGage->SetEnable(FALSE);
	}
	
#if defined (LOCALE_THAILAND) || defined(GUI_ADJUST_RUSSIA)
	m_pBulletKey->ReCreateTextEx( GetDefaultFontName() , 32, GAME_FONT_SIZE_ANY_TEXT, FALSE, 0, 0, TRUE, FONT_SHADOW_QUALITY_LOW);
	m_pRangeKey->ReCreateTextEx( GetDefaultFontName() , 32, GAME_FONT_SIZE_ANY_TEXT, FALSE, 0, 0, TRUE, FONT_SHADOW_QUALITY_LOW);
#endif

	m_pBulletKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pBulletValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pBulletKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pBulletValue->SetTextColor(GetColor(GCT_DEFAULT));

	m_pRangeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRangeValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRangeKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pRangeValue->SetTextColor(GetColor(GCT_DEFAULT));

	m_weaponInfo[WEAPON_INFO_DAMAGE]._pKey->SetText(GAME_STRING("STBL_IDX_SHOP_WEAPON_DAMAGE"));/*������*/
	m_weaponInfo[WEAPON_INFO_RAPID]._pKey->SetText(GAME_STRING("STBL_IDX_SHOP_WEAPON_RAPID"));/*�����*/
	m_weaponInfo[WEAPON_INFO_HIT_RATE]._pKey->SetText(GAME_STRING("STBL_IDX_SHOP_WEAPON_HIT_RATE"));/*���߷�*/
	m_weaponInfo[WEAPON_INFO_VERT_RECOIL]._pKey->SetText(GAME_STRING("STBL_IDX_SHOP_WEAPON_VERT_RECOIL"));/*�����ݵ�*/
	m_weaponInfo[WEAPON_INFO_HORZ_RECOIL]._pKey->SetText(GAME_STRING("STBL_IDX_SHOP_WEAPON_HORZ_RECOIL"));/*����ݵ�*/
	m_weaponInfo[WEAPON_INFO_SPEED]._pKey->SetText(GAME_STRING("STBL_IDX_SHOP_WEAPON_SPEED"));/*�̵��ӵ�*/

	m_pBulletKey->SetText(GAME_STRING("STBL_IDX_SHOP_WEAPON_BULLET"));/*��ź��*/
	m_pRangeKey->SetText(GAME_STRING("STBL_IDX_SHOP_WEAPON_RANGE"));/*�����Ÿ�*/

	INIT_COMBO_EX(m_pBuyWeaponCombo1);
	INIT_COMBO_EX(m_pBuyWeaponCombo2);
	m_pBuyWeaponCombo1->SetEnable( TRUE);
#if defined( BUY_WEAPON_BY_CASH)
	m_pBuyWeaponCombo2->SetEnable( TRUE);
#else
	m_pBuyWeaponCombo2->SetEnable( FALSE);
#endif	
	m_pBuyWeaponCombo1->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
	m_pBuyWeaponCombo2->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
	m_pBuyWeaponCombo1->SetCaption(GAME_STRING("STBL_IDX_SHOP_INFO_BUYTYPE_POINT"));
	m_pBuyWeaponCombo2->SetCaption(GAME_STRING("STBL_IDX_SHOP_INFO_BUYTYPE_CASH"));

	// Weapon Goods frame
#if 0	// ����� �� ��Ʈ���� ������� �ʽ��ϴ�.
	m_PriceFrame.pPointInfoStatic->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_PriceFrame.pPointInfoStatic->SetTextColor(GetColor(GCT_DEFAULT));
	m_PriceFrame.pPointInfoStatic->SetText(GAME_STRING("STBL_IDX_SHOP_INFO_POINT"));
#endif

	m_PriceFrame.pHavePointStatic->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_PriceFrame.pHavePointStatic->SetTextColor(GetColor(GCT_DEFAULT));
	sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_SHOP_HAVE_MONEY"));
	m_PriceFrame.pHavePointStatic->SetText(temp);
	
	m_PriceFrame.pHavePointValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_PriceFrame.pHavePointValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_PriceFrame.pHavePointValue->SetText("0");

	m_PriceFrame.pRemainPointStatic->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_PriceFrame.pRemainPointStatic->SetTextColor(GetColor(GCT_DEFAULT));
	sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_SHOP_REMAIN_MONEY"));
	m_PriceFrame.pRemainPointStatic->SetText(temp);

	m_PriceFrame.pRemainPointValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_PriceFrame.pRemainPointValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_PriceFrame.pRemainPointValue->SetText("0");

	for( INT32 i=0; i<GOODS_MAX_IN_PACK; i++)
	{
		m_PriceFrame.pUseDayStatic[i]->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pUseDayStatic[i]->SetTextColor(GetColor(GCT_DEFAULT));
		sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_SHOP_USE_DAY"));
		m_PriceFrame.pUseDayStatic[i]->SetText(temp);

		m_PriceFrame.pUseDayValue[i]->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pUseDayValue[i]->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pUseDayValue[i]->SetText("0");

		m_PriceFrame.pSaleText[i]->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pSaleText[i]->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pSaleText[i]->SetEnable(FALSE);

		m_PriceFrame.pBuyPointValue[i]->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pBuyPointValue[i]->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pBuyPointValue[i]->SetText("0");

		m_PriceFrame.pBuyPointStatic[i]->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pBuyPointStatic[i]->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pBuyPointStatic[i]->SetText(GAME_STRING("STBL_IDX_SHOP_MONEY"));
	}

	m_PriceFrame.pHavePointUnitStatic->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_PriceFrame.pHavePointUnitStatic->SetTextColor(GetColor(GCT_DEFAULT));
	m_PriceFrame.pHavePointUnitStatic->SetText(GAME_STRING("STBL_IDX_SHOP_MONEY"));

	m_PriceFrame.pRemainPointUnitStatic->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_PriceFrame.pRemainPointUnitStatic->SetTextColor(GetColor(GCT_DEFAULT));
	m_PriceFrame.pRemainPointUnitStatic->SetText(GAME_STRING("STBL_IDX_SHOP_MONEY"));


	CTabBase::OnClose();
}


BOOL	CTabWeapon::OnOpen()
{
	CTabBase::OnOpen();

	m_nWeaponUsageType		= (WEAPON_SLOT_TYPE)-2;
	m_nCurViewWeaponID	= 0;

	WEAPON_SLOT_TYPE FirstGroup = _GetFirstGroup();
	OnChangeGroup( FirstGroup );

	return TRUE;
}


void	CTabWeapon::OnClose()
{
	CTabBase::OnClose();

	i3GuiButton* pButton = GetGroupButton(m_nWeaponUsageType);
	if ( pButton )
		pButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
}


void	CTabWeapon::OnUpdate(RT_REAL32 rDeltaSec)
{
	_UpdateSlotList();
	_UpdateDesc();
	_UpdateBuyType();
	_CheckUpdateGoodsFrame(rDeltaSec);
}


void	CTabWeapon::OnFocus(UINT32 idxItem)
{
	if(MAX_SLOT_FOR_SHOP_WEAPON <= idxItem)
		return;

	if (FALSE == m_Slot[idxItem]._pBg->GetShapeEnable(SLOT_SHAPE_INDEX_PUSHED))
	{
		m_Slot[idxItem]._pBg->SetShapeEnable(SLOT_SHAPE_INDEX_NORMAL, FALSE);
		m_Slot[idxItem]._pBg->SetShapeEnable(SLOT_SHAPE_INDEX_HOVER, TRUE);
	}
}


BOOL	CTabWeapon::OnSelectSlot(UINT32 idxSlot, BOOL ForceSelect)
{
	if( TRUE != CTabBase::OnSelectSlot(idxSlot, ForceSelect) )
		return FALSE;


	BOOL bEnableBuyCash = FALSE;
	SHOP_GOODS * pGoods = m_pFocusedGoods->GetGoods(0);

	// ȣĪ ���������� �˻��Ѵ�.
	if( CheckItemByDesignation( pGoods->_ItemID, g_pGameContext->GetTitleInfo()))
	{
		// ȣĪ�����ε� ������ �� �ִٸ� ����Ʈ �������� �����ش�.
		_SetWeaponBuyType(SHOP_BUY_TYPE_POINT);

 		SHOP_GOODS * pSelectedGoods = m_pFocusedGoods->GetGoods( m_SelectGoodsIndexInPack );
		if ( pSelectedGoods->_PriceCash > 0 )
			bEnableBuyCash = TRUE;
	}
	else
	{
		// ����Ʈ�δ� ��� �ִ� ����� ���� ������
		// ĳ���� ������ �� �ִٸ� ĳ�� �������� �����ش�.
		if ( m_pFocusedGoods->GetCashGoodsCount() > 0 )
			bEnableBuyCash = TRUE;

		if( bEnableBuyCash )	_SetWeaponBuyType(SHOP_BUY_TYPE_CASH);
		else					_SetWeaponBuyType(SHOP_BUY_TYPE_POINT);
	}


#if defined (USE_SHOP_GIFT)
	
#if defined (LOCALE_THAILAND)
	//	ĳ���۸� ���� �ϱ� ����	
	if(bEnableBuyCash)
	{
		ACTIVATE_BUTTON( m_PriceFrame.pGift);
	}
	else
	{
		DEACTIVATE_BUTTON( m_PriceFrame.pGift);
	}
#else
	ACTIVATE_BUTTON( m_PriceFrame.pGift);
#endif

#else
	//DEACTIVATE_BUTTON(m_PriceFrame.pGift);	// ��ư ��Ȱ��ȭ
	m_PriceFrame.pGift->SetEnable(FALSE);
#endif

	return TRUE;
}


void	CTabWeapon::OnBuyGoodsSuccess()
{
	I3ASSERT(m_pSect->isEnable() == TRUE);

	///////////////////////////////////		�±��� ����ó��
#if defined (LOCALE_THAILAND)

	//	�κ��� ���Ⱑ �̹� �ִ� ���� 1,3,7�ϸ�(���� 3��) ��ǰ���� ���´�.
	//	�κ��� ���� ����� ���� ��� �ϴ� �����̹Ƿ� 30�ϸ� ���;� �Ѵ�.
	//	���� ���Կ� �����ϸ� ������ ������ 30�� ��ǰ�� ������ ���ƾ� �Ѵ�. �׷��Ƿ� �ٽ� ��ǰ ����Ʈ�� ������.


	// ���� ���õǾ� �ִ� ��ǰ�� �ε����� �����Ͽ�
	// ��ǰ ����� ���ŵǸ� �ٽ� �� ��ǰ�� �����ϵ��� �մϴ�.
	INT32 CurrentGoodsIndex = m_RefGoodsList.Find(m_pFocusedGoods);

	g_pFramework->GetShopContext()->BuildGoodsList();

	// ��ǰ ����� �ٽ� �����.
	MakeGroupList();

	// �ٽ� �⺻���� �����ϵ��� �մϴ�.	
	OnSelectSlot(CurrentGoodsIndex - m_idxStartSlot, TRUE);


#endif
}


void	CTabWeapon::OnPreGuiNotify()
{
}


void	CTabWeapon::OnLinkControl(i3BinList * plist)
{
	LinkControlEx(plist, "shopWeaponTab", m_pTab, GCI_SHOP_B_WEAPON_TAB);
	LinkControlEx(plist, "shopWeapon", m_pSect, -1);
	LinkControlEx(plist, "shopWeaponScrollList", m_pScrollList, GCI_SHOP_L_WEAPON_SCROLL);

	LinkControlEx(plist, "shopAllWeapon", m_pAllGroup, GCI_SHOP_B_ALL_WEAPON);
	LinkControlEx(plist, "shopPrimaryWeapon", m_pPrimaryGroup, GCI_SHOP_B_PRIMARY);
	LinkControlEx(plist, "shopSecondaryWeapon", m_pSecondaryGroup, GCI_SHOP_B_SECONDARY);
	LinkControlEx(plist, "shopMeleeWeapon", m_pMeleeGroup, GCI_SHOP_B_MELEE);
	LinkControlEx(plist, "shopThrowingWeapon", m_pThrowingGroup, GCI_SHOP_B_THROWING);
	LinkControlEx(plist, "shopItemWeapon", m_pItemGroup, GCI_SHOP_B_ITEM);

	
	for(UINT32 i = 0; i < MAX_SLOT_FOR_SHOP_WEAPON ; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "shopWeaponSlot%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pBg, -1);

		sprintf_s(strName, "shopWeaponSlotValue%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pValue, -1);

		sprintf_s(strName, "shopWeaponSlotKey%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pKey, -1);

		sprintf_s(strName, "shopWeaponSlotDummy%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pImage, i);

		sprintf_s(strName, "shopWeaponSlotExtension%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pExtension, -1);

		sprintf_s(strName, "shopWeaponSlotUsed%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pUsed, -1);

		sprintf_s(strName, "shopWeaponSlotTerm%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pTerm, -1);

		sprintf_s(strName, "shopWeaponSlotPurchase%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pPurchase, -1);

		sprintf_s(strName, "shopWeaponSlotFilter%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pFilter, GCI_SHOP_S_WEAPON_SLOT_1 + i);

		sprintf_s(strName, "shopWeaponSlotSaleMark%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleMark, -1);

		sprintf_s(strName, "shopWeaponFirstSaleText%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleText[SALE_TEXT_FIRST], -1);

		sprintf_s(strName, "shopWeaponSecondSaleText%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleText[SALE_TEXT_SECOND], -1);

		// ������ ��������.
		sprintf_s(strName, "shopWeaponSlotDurabilityBar%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pDurabilityBar, -1);

		sprintf_s(strName, "shopWeaponSlotDurabilityGauge%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pDurabilityGauge, -1);
	}

	LinkControlEx(plist, "shopWeaponInfoImage", m_pInfo, -1);
	LinkControlEx(plist, "shopWeaponDummy", m_pImage, -1);
	LinkControlEx(plist, "shopWeaponInfoExtension", m_pInfoExtension, -1);
	LinkControlEx(plist, "shopWeaponGoodsFrame", m_pGoodsFrame, -1);
	LinkControlEx(plist, "shopWeaponKey", m_pInfoKey, -1);
	LinkControlEx(plist, "shopWeaponValue", m_pInfoValue, -1);
	LinkControlEx(plist, "shopWeaponInfoTerm", m_pInfoTerm, -1);
	LinkControlEx(plist, "shopWeaponInfoPurchase", m_pInfoPurchase, -1);

	LinkControlEx(plist, "shopWeaponInfoDetail", m_pDetailWeapon, GCI_SHOP_B_DETAIL_WEAPON);
	LinkControlEx(plist, "shopWeaponInfoPreview", m_pPreviewWeapon, GCI_SHOP_B_PREVIEW_WEAPON);

	LinkControlEx(plist, "shopWeaponGoodsBuy", m_PriceFrame.pBuy, GCI_SHOP_B_BUY_WEAPON);
	LinkControlEx(plist, "shopWeaponGoodsGift", m_PriceFrame.pGift, GCI_SHOP_B_GIFT_WEAPON);

	WeaponInfo* pWeaponInfo;

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_DAMAGE;
	LinkControlEx(plist, "shopDamageKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "shopDamageValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "shopDamageGage", pWeaponInfo->_pGage, -1);

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_RAPID;
	LinkControlEx(plist, "shopRapidKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "shopRapidValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "shopRapidGage", pWeaponInfo->_pGage, -1);

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_HIT_RATE;
	LinkControlEx(plist, "shopHitRateKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "shopHitRateValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "shopHitRateGage", pWeaponInfo->_pGage, -1);

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_VERT_RECOIL;
	LinkControlEx(plist, "shopVertRecoilKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "shopVertRecoilValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "shopVertRecoilGage", pWeaponInfo->_pGage, -1);

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_HORZ_RECOIL;
	LinkControlEx(plist, "shopHorzRecoilKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "shopHorzRecoilValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "shopHorzRecoilGage", pWeaponInfo->_pGage, -1);

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_SPEED;
	LinkControlEx(plist, "shopSpeedKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "shopSpeedValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "shopSpeedGage", pWeaponInfo->_pGage, -1);

	LinkControlEx(plist, "shopBulletKey", m_pBulletKey, -1);
	LinkControlEx(plist, "shopBulletValue", m_pBulletValue, -1);

	LinkControlEx(plist, "shopRangeKey", m_pRangeKey, -1);
	LinkControlEx(plist, "shopRangeValue", m_pRangeValue, -1);

	LinkControlEx(plist, "shopWeaponGoodsCombo1", m_pBuyWeaponCombo1, GCI_SHOP_B_WEAPON_BUY_TYPE_COMBO1);
	LinkControlEx(plist, "shopWeaponGoodsCombo2", m_pBuyWeaponCombo2, GCI_SHOP_B_WEAPON_BUY_TYPE_COMBO2);


	{
		char szTemp[256] = "";
//		LinkControlEx(plist, "shopWeaponPointInfoStatic", m_PriceFrame.pPointInfoStatic, -1);


		for( INT32 i=0; i<GOODS_MAX_IN_PACK; i++)
		{
			sprintf_s(szTemp, "shopWeaponGoodsRadio0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pGoodsRadio[i], GCI_SHOP_R_WEAPON_GOODS_RADIO_1 + i);

			//	��� �Ⱓ
			sprintf_s(szTemp, "shopWeaponGoodsDayStatic0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pUseDayStatic[i], -1);

			sprintf_s(szTemp, "shopWeaponGoodsDayValue0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pUseDayValue[i], GCI_SHOP_S_WEAPON_USE_DAY_1 + i);


			sprintf_s(szTemp, "shopWeaponGoodsSaleText0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pSaleText[i], -1);

			//	���� ����Ʈ
			sprintf_s(szTemp, "shopWeaponGoodsPointValue0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pBuyPointValue[i], GCI_SHOP_S_WEAPON_BUY_POINT_1 + i);

			// ȭ�����
			sprintf_s(szTemp, "shopWeaponGoodsPointStatic0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pBuyPointStatic[i], -1);

			// �ѱ� N���� �ɺ� �̹���
			sprintf_s(szTemp, "shopWeaponGoodsCashSymbol0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pCashSymbol[i], -1);
		}

		LinkControlEx(plist, "shopWeaponGoodsHavePointStatic", m_PriceFrame.pHavePointStatic, -1);
		LinkControlEx(plist, "shopWeaponGoodsHavePointValue", m_PriceFrame.pHavePointValue, GCI_SHOP_S_WEAPON_HAVE_POINT);
		LinkControlEx(plist, "shopWeaponGoodsHaveCashSymbol", m_PriceFrame.pHaveCashSymbol, -1);
		LinkControlEx(plist, "shopWeaponGoodsPointStatic05", m_PriceFrame.pHavePointUnitStatic, -1);
		LinkControlEx(plist, "shopWeaponGoodsRemainPointStatic", m_PriceFrame.pRemainPointStatic, -1);
		LinkControlEx(plist, "shopWeaponGoodsRemainPointValue", m_PriceFrame.pRemainPointValue, GCI_SHOP_S_WEAPON_REMAIN_POINT);
		LinkControlEx(plist, "shopWeaponGoodsRemainCashSymbol", m_PriceFrame.pRemainCashSymbol, -1);
		LinkControlEx(plist, "shopWeaponGoodsPointStatic06", m_PriceFrame.pRemainPointUnitStatic, -1);
	}

	m_RepairFrame.OnLinkControl(plist);

	CTabBase::OnClose();
}


void	CTabWeapon::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	//	Weapon
	case GCI_SHOP_B_ALL_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) 
			OnChangeGroup(WEAPON_SLOT_UNKNOWN, true);
		break;

	case GCI_SHOP_B_PRIMARY:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) 
			OnChangeGroup(WEAPON_SLOT_PRIMARY, true);
		break;

	case GCI_SHOP_B_SECONDARY:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) 
		{
			_ResetVestige();
			OnChangeGroup(WEAPON_SLOT_SECONDARY, true);
		}
		break;

	case GCI_SHOP_B_MELEE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) 
			OnChangeGroup(WEAPON_SLOT_MELEE, true);
		break;

	case GCI_SHOP_B_THROWING:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) 
			OnChangeGroup(WEAPON_SLOT_THROWING1, true);
		break;

	case GCI_SHOP_B_ITEM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) 
			OnChangeGroup(WEAPON_SLOT_THROWING2, true);
		break;

	case GCI_SHOP_L_WEAPON_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnChangeScroll();
		break;

	case GCI_SHOP_S_WEAPON_SLOT_1:	// fall through
	case GCI_SHOP_S_WEAPON_SLOT_2:	// fall through
	case GCI_SHOP_S_WEAPON_SLOT_3:	// fall through
	case GCI_SHOP_S_WEAPON_SLOT_4:	// fall through
	case GCI_SHOP_S_WEAPON_SLOT_5:	// fall through
	case GCI_SHOP_S_WEAPON_SLOT_6:	// fall through
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnFocus(pNotify->m_nID - GCI_SHOP_S_WEAPON_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnSelectSlot(pNotify->m_nID - GCI_SHOP_S_WEAPON_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)
		{
#if defined(USE_REPAIR_WEAPON)
			if( m_pFocusedGoods == NULL)
				return;

			SHOP_GOODS * pGoods = m_pFocusedGoods->GetGoods(0);
			if( pGoods == NULL)
				return;

			if ( g_pFramework->HaveItem(pGoods->_ItemID) && g_pFramework->GetShopContext()->IsDurabilityItem(pGoods->_ItemID) )
			{
				OnRepair();
			}
			else
			{
				m_pOwner->OnGoodsBuy(pNotify->m_nID - GCI_SHOP_S_WEAPON_SLOT_1, TRUE);
			}
#else
			m_pOwner->OnGoodsBuy(pNotify->m_nID - GCI_SHOP_S_WEAPON_SLOT_1, TRUE);
#endif
		}
		break;

	case GCI_SHOP_R_WEAPON_GOODS_RADIO_1:
	case GCI_SHOP_R_WEAPON_GOODS_RADIO_2:
	case GCI_SHOP_R_WEAPON_GOODS_RADIO_3:
	case GCI_SHOP_R_WEAPON_GOODS_RADIO_4:
		if( I3GUI_RADIO_NOTIFY_CLICKED == pNotify->m_nEvent) 
			OnSelectGoodsRadio(pNotify->m_nID - GCI_SHOP_R_WEAPON_GOODS_RADIO_1);
		break;

	case GCI_SHOP_B_DETAIL_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnWeaponDetail();
		break;

	case GCI_SHOP_B_PREVIEW_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnWeaponPreview();
		break;

	case GCI_SHOP_B_BUY_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) m_pOwner->OnGoodsBuy(pNotify->m_nID - GCI_SHOP_S_WEAPON_SLOT_1);
		break;

	case GCI_SHOP_B_GIFT_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) m_pOwner->OnGoodsGift(pNotify->m_nID - GCI_SHOP_S_WEAPON_SLOT_1);
		break;

	case GCI_SHOP_B_REPAIR_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnRepair();
		break;

	case GCI_SHOP_B_WEAPON_BUY_TYPE_COMBO1:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _SetWeaponBuyType(SHOP_BUY_TYPE_POINT);
		break;

	case GCI_SHOP_B_WEAPON_BUY_TYPE_COMBO2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _SetWeaponBuyType(SHOP_BUY_TYPE_CASH);
		break;
	}
}


void	CTabWeapon::_ShowWeaponGage(UINT32 nInfoType, UINT32 nRate)
{
	_ShowGaugeBarStatic(m_weaponInfo[nInfoType]._pGage, nRate);
}


void	CTabWeapon::_SetWeaponBuyType(INT32 nType)
{
	if(GOODS_PRICE_ALL == m_SelectItemPriceType)
	{
		switch( nType)
		{
		case SHOP_BUY_TYPE_POINT: m_SelectItemCurrentPriceType = GOODS_PRICE_POINT;			break;
		case SHOP_BUY_TYPE_CASH: m_SelectItemCurrentPriceType = GOODS_PRICE_CASH;			break;
		}
	}
	else if(GOODS_PRICE_POINT == m_SelectItemPriceType)
	{
		switch( nType)
		{
		case SHOP_BUY_TYPE_POINT: m_SelectItemCurrentPriceType = GOODS_PRICE_POINT;			break;
		}
	}
	else if(GOODS_PRICE_CASH == m_SelectItemPriceType)
	{
		switch( nType)
		{
		case SHOP_BUY_TYPE_CASH: m_SelectItemCurrentPriceType = GOODS_PRICE_CASH;			break;
		}
	}

	if ( m_pFocusedGoods )
	{
		INT32 RadioIndex = 0;
		#if defined( LOCALE_PHILIPPINES )
			RadioIndex = 0;
		#else
			m_pFocusedGoods->GetLastIndex(m_SelectItemCurrentPriceType, &RadioIndex);
		#endif
		OnSelectGoodsRadio(RadioIndex);
	}
}


void	CTabWeapon::OnSelectGoodsRadio(UINT32 RadioIndex)
{
	CTabBase::OnSelectGoodsRadio(RadioIndex);

	if( ! m_pFocusedGoods )
	{
		m_SelectGoodsIndexInPack = RadioIndex;
		return;
	}


	// ĳ���� ����Ʈ �Ѵ� �Ǹ��� ��� ���� ���õǾ��� ���Ź���� Goods�� ���õǰ� �մϴ�.
	INT32 goodsCount = m_pFocusedGoods->GetGoodsCount();
	for(INT32 i = 0 ; i < goodsCount ; i++)
	{
		SHOP_GOODS * pGoods = m_pFocusedGoods->GetGoods(i);

		// ���� ���õ� ���Ź���� PriceType�� �ٸ��� 
		if( GOODS_PRICE_POINT == m_SelectItemCurrentPriceType )
		{
			if( pGoods->_PriceType == GOODS_PRICE_CASH) 
				continue;
		}
		else if(GOODS_PRICE_CASH == m_SelectItemCurrentPriceType )
		{
			if( pGoods->_PriceType == GOODS_PRICE_POINT)
				continue;
		}

		m_SelectGoodsIndexInPack = i;

		if( RadioIndex == 0) return;
		else RadioIndex--;
	}

}


void	CTabWeapon::OnWeaponDetail(void)
{	
	if (NULL == m_pFocusedGoods) return;	// ���� ���� ������ ����

	SHOP_GOODS * pGoods = m_pFocusedGoods->GetGoods(0);
	if (NULL == pGoods) return;				// ���� ������ ����

	_SetupVestige();

	CPopupExtension::SetWeaponFlag(pGoods->_ItemID);
	m_pOwner->TogglePopupExtension();
}


void	CTabWeapon::OnWeaponPreview(void)
{
	if (NULL == m_pFocusedGoods) 
		return;
 
	SHOP_GOODS * pGoods = m_pFocusedGoods->GetGoods(0);
	if (NULL == pGoods) return;				// ���� ������ ����

	INT32 WeaponID = pGoods->_ItemID;

	m_nCurViewWeaponID = WeaponID;

	//	���� �̸����� ���⸦ �ӽ÷� ����
	VIEW_CHARA_INFO changeCharaInfo;
	switch( GetWeaponSlot_WeaponItemIDFunc( WeaponID))
	{
		case WEAPON_SLOT_PRIMARY:	changeCharaInfo._weapon[0] = WeaponID;	changeCharaInfo._weaponSlot = 0;	break;
		case WEAPON_SLOT_SECONDARY:	changeCharaInfo._weapon[1] = WeaponID;	changeCharaInfo._weaponSlot = 1;	break;
		case WEAPON_SLOT_MELEE:		changeCharaInfo._weapon[2] = WeaponID;	changeCharaInfo._weaponSlot = 2;	break;
		case WEAPON_SLOT_THROWING1:	changeCharaInfo._weapon[3] = WeaponID;	changeCharaInfo._weaponSlot = 3;	break;
		case WEAPON_SLOT_THROWING2:	changeCharaInfo._weapon[4] = WeaponID;	changeCharaInfo._weaponSlot = 4;	break;
	}

	//	����
	if( m_pOwner->UpdateCharacter(VT_OTHER, FALSE, &changeCharaInfo))
		m_pOwner->PlaySound(GUI_SND_ITEM_EQUIP);	//	����
}


void	CTabWeapon::OnRepair(void)
{
	if ( ! m_pFocusedGoods )
		return;

	SHOP_GOODS * pGoods = m_pFocusedGoods->GetGoods(0);
	if ( ! pGoods )
		return;

#if !defined( ENABLE_BUY_USER_TITLE_WEAPON)
	char szNeedUserTitle[MAX_STRING_COUNT] = {};
	if( ! CheckItemByDesignation( pGoods->_ItemID, g_pGameContext->GetTitleInfo(), szNeedUserTitle))
	{
		char szMessage[1024] = "";
		/*�ش� �������� %s ȣĪ��\nȹ���Ͽ��� ������ �� �ֽ��ϴ�.\n(ȣĪ ȹ�� �� ���� ����)*/
		sprintf_s( szMessage, GAME_STRING("STR_SHOP_REQUIRE_DESIGNATION_TO_BUY"), szNeedUserTitle);

		STRING_POPUP( szMessage);
		return;
	}
#endif
	
	((CStageReady*)m_pOwner->getParent())->TogglePopup(POPUP_REPAIR, (void*)pGoods->_ItemID);
}


void	CTabWeapon::_UpdateSlotList(void)
{
	char szWeaponName[ 256];

	for( INT32 i = 0; i < MAX_SLOT_FOR_SHOP_WEAPON ; i++)
	{
		m_Slot[i]._pBg->SetShapeEnable(SLOT_SHAPE_INDEX_SELECTED, FALSE);
		m_Slot[i]._pBg->setInputDisable(TRUE);
		m_Slot[i]._pBg->SetEnable(FALSE);

		m_Slot[i]._pKey->SetEnable(FALSE);
		m_Slot[i]._pImage->SetEnable(FALSE);
		m_Slot[i]._pExtension->SetEnable(FALSE);
		m_Slot[i]._pUsed->SetEnable(FALSE);
		m_Slot[i]._pUsed->SetAllShapeEnable(FALSE);
		m_Slot[i]._pTerm->SetEnable(FALSE);

		m_Slot[i]._pValue->SetEnable(TRUE);
		m_Slot[i]._pValue->SetText( INVALID_TEXT);
		m_Slot[i]._pValue->SetTextColor( GetColor(GCT_DEFAULT));
		m_Slot[i]._pSaleMark->SetEnable(FALSE);
		m_Slot[i]._pSaleMark->SetAllShapeEnable(FALSE);

		m_Slot[i]._pDurabilityBar->SetEnable(FALSE);
	}


	GameGUI::Reset(m_pScrollList, GameGUI::CalcuScrollCount(m_RefGoodsList.GetCount(), SLOT_HORI_COUNT_WEAPON));


	CShop* shop = g_pGameContext->GetShop();
	char szString[MAX_STRING_COUNT] = "";


	//	��ǰ�� ȭ�鿡 ����Ѵ�.
	for( INT32 i = 0 ; i < MAX_SLOT_FOR_SHOP_WEAPON ; i++)
	{
		if( i + m_idxStartSlot >= m_RefGoodsList.GetCount() || (i + m_idxStartSlot) < 0)
		{
			m_Slot[i]._pBg->SetEnable(FALSE);
			continue;
		}

		SHOP_GOODS_PACK * pGoodsPack = (SHOP_GOODS_PACK*) m_RefGoodsList.GetItem(i + m_idxStartSlot);
		if( pGoodsPack == NULL)
			continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;

		UINT32 weaponBuyType = (UINT32)shop->GetGoodsPrice(pGoods->_GoodsID, NULL, NULL);

		INT32 WeaponID = pGoods->_ItemID;

		// ���� ������ üũ
		SetItemSaleMark( m_Slot[i]._pSaleMark, pGoods->_SaleMark);				

		if( WeaponID <= -1)
		{
			continue;	//	���������� �׸����� �Ѿ������ Ŭ���̾�Ʈ�� ��ǰ define �κп��� ��ϵ� ��ǰ�� �ƴϹǷ� �ǳʶڴ�.
		}

		WEAPON_CLASS_TYPE ClassType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(WeaponID);
		INT32 WeaponNum = GET_ITEM_FLAG_NUMBER(WeaponID);
		UINT8 Type = 0;
		UINT32 Arg = 0;

		// KEYWORD : WEAPON_MINIAXE	�Ϲ� �������� �ٸ� ���⸦ ��� �ִ� ����Դϴ�. �ش� ���Ⱑ ǥ�õǵ��� ��������� �մϴ�.
		GUI_WEAPON_VALID( ClassType, WeaponNum);

		if (m_pFocusedGoods == pGoodsPack)
		{
			m_Slot[i]._pBg->SetShapeEnable(SLOT_SHAPE_INDEX_SELECTED, TRUE);
		}

		// ���� Ȱ��ȭ
		m_Slot[i]._pBg->SetShapeEnable(SLOT_SHAPE_INDEX_PUSHED, FALSE);
		m_Slot[i]._pBg->setInputDisable(FALSE);
		m_Slot[i]._pBg->SetEnable(TRUE);


		if (g_pFramework->SetWeaponSelectMark(m_Slot[i]._pImage, (UINT8)ClassType, (UINT8)WeaponNum) )
		{
			m_Slot[i]._pImage->SetEnable(TRUE);
		}

		// ��� ����
		if ( g_pFramework->GetWeaponTypeByID(WeaponID, &Type, &Arg))
		{
#if defined(USE_REPAIR_WEAPON)
			if ( g_pFramework->GetShopContext()->IsDurabilityItem(WeaponID) )
			{
				m_Slot[i]._pDurabilityBar->SetEnable(TRUE);

				UINT32 Current = 0, Max = 0;
				if ( g_pFramework->GetShopContext()->GetDurability(WeaponID, Current, Max) )
				{
					REAL32 fRate = static_cast<REAL32>(Current) / static_cast<REAL32>(Max) * 100.0f;
					UINT32 nRate = static_cast<UINT32>(MINMAX(0, static_cast<INT32>(fRate), 100));
					_ShowGaugeBarStatic(m_Slot[i]._pDurabilityGauge, nRate);
				}
			}
			else
#endif
			{
				BOOL Expendable = (pGoods->_AuthType == AUTH_COUNT) ? TRUE : FALSE;
				m_pOwner->_GetGoodsUseDay(Expendable, Type, Arg, szString, MAX_STRING_COUNT);
				m_Slot[i]._pTerm->SetText(szString);
				m_Slot[i]._pTerm->SetEnable(TRUE);
			}

			m_Slot[i]._pTerm->SetTextColor(GetColor(GCT_DEFAULT));
		}
		else
		{
			//	�̺���
			i3String::Copy( szString, GAME_STRING("STBL_IDX_ITEM_NO_HAVE"), MAX_STRING_COUNT );

			m_Slot[i]._pTerm->SetTextColor( GetColor(GCT_LIGHT_GREEN));
			m_Slot[i]._pTerm->SetText(szString);
			m_Slot[i]._pTerm->SetEnable(TRUE);
		}


#if !defined( ENABLE_BUY_USER_TITLE_WEAPON)
		// ȣĪ ���� �Ǵ�
		if( IsItemByDesignation( pGoods->_ItemID))
		{
			if( CheckItemByDesignation( pGoods->_ItemID, g_pGameContext->GetTitleInfo()))
			{
				//	��ȹ ��û���� ���԰����� ǥ�� ���ϵ���
				m_Slot[i]._pPurchase->SetEnable(FALSE);
				//m_Slot[i]._pPurchase->SetTextColor( GetColor(GCT_LIGHT_GREEN));
				//m_Slot[i]._pPurchase->SetText( GAME_STRING("STR_SHOP_CAN_BUYING"));/*���԰���*/
			}
			else
			{
#if defined(BUY_WEAPON_BY_CASH)	

				//	ȣĪ ����� ĳ�ø� ������ �� �ִ�.
				#if defined( ENABLE_BUY_CASH_WEAPON_IF_NEED_USER_TITLE) 
				if( weaponBuyType & GOODS_PRICE_CASH )
				{
					m_Slot[i]._pPurchase->SetEnable(FALSE);
				}
				else
				#endif
				{
					m_Slot[i]._pPurchase->SetEnable(TRUE);
					m_Slot[i]._pPurchase->SetTextColor( GetColor(GCT_RED));
					m_Slot[i]._pPurchase->SetText( GAME_STRING("STR_SHOP_CANNOT_BUYING"));/*ȣĪ�ʿ�*/
				}
#else
				m_Slot[i]._pPurchase->SetEnable(TRUE);
				m_Slot[i]._pPurchase->SetTextColor( GetColor(GCT_RED));
				m_Slot[i]._pPurchase->SetText( GAME_STRING("STR_SHOP_CANNOT_BUYING"));/*ȣĪ�ʿ�*/
#endif
			}
		}
		else
#endif
		{
			m_Slot[i]._pPurchase->SetEnable(FALSE);
		}


		// ��ǰ�� �Ǹ� ������ �����ش�. �Ʒ����� ���� � ��ǰ������ �����ش�.
		m_pOwner->_PrintSaleText(m_Slot[i]._pSaleText, weaponBuyType);


		// �ͽ��ټ�
		CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( ClassType, WeaponNum);
		if ( pInfo == NULL )
		{
			char Name[MAX_STRING_COUNT] = "";

			i3String::Format(Name, MAX_STRING_COUNT, 
				"INVALID \nGoods:%d \nItem:%d \nType:%d \nNum:%d", 
				pGoods->_GoodsID, pGoods->_ItemID, ClassType, WeaponNum);

			m_Slot[i]._pValue->SetText(Name);			
			m_Slot[i]._pValue->SetEnable(TRUE);
			continue;
		}

#ifndef NC_BUILD
			bool noName = false;
#endif

		// ���� �̸�
		pInfo->GetWeaponInfoName( szWeaponName);

		if( i3String::Length(szWeaponName) == 0)
		{
			i3String::Format(szWeaponName, sizeof(szWeaponName), 
				"NO NAME \nGoods:%d \nItem:%d \nType:%d \nNum:%d", 
				pGoods->_GoodsID, pGoods->_ItemID, ClassType, WeaponNum);

#ifndef NC_BUILD
			noName = true;
#endif

		}

#ifndef NC_BUILD
		m_Slot[i]._pValue->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE );
		m_Slot[i]._pSaleMark->SetEnable( TRUE );
		m_Slot[i]._pImage->SetEnable( TRUE );
		if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ) )
		{
			if( !noName )
			{
				i3String::Format(szWeaponName, sizeof(szWeaponName), 
					"%s\nGoods:%d\nItem:%d\nType:%d\nNum:%d", szWeaponName,
					pGoods->_GoodsID, pGoods->_ItemID, ClassType, WeaponNum);
			}

			m_Slot[i]._pValue->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP );
			m_Slot[i]._pSaleMark->SetEnable( FALSE);
			m_Slot[i]._pImage->SetEnable( FALSE );
		}
#endif

		m_Slot[i]._pValue->SetText( szWeaponName);
		m_Slot[i]._pValue->SetEnable(TRUE);

		//	Endurance zero weapon name is red color
		if( GetEnduranceZero(Type, Arg) )
		{
			m_Slot[i]._pValue->SetTextColor( GetColor(GCT_RED));
		}

		// ī�װ�
		g_pWeaponInfoDataBase->GetWeaponCategoryEx(pInfo, szString, MAX_STRING_COUNT);

		m_Slot[i]._pKey->SetText(szString);
		m_Slot[i]._pKey->SetEnable(TRUE);

	}
}


void	CTabWeapon::_UpdateDesc(void)
{
	char szWeaponName[ 256];

	// ���� �ʱ�ȭ
	m_pInfo->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
	m_pImage->SetEnable(FALSE);
	m_pInfoKey->SetEnable(FALSE);
	m_pInfoValue->SetEnable(FALSE);

	for (INT32 i = 0 ; i < MAX_SHOP_WEAPON_INFO ; ++i)
	{
		m_weaponInfo[i]._pGage->SetEnable(FALSE);
		m_weaponInfo[i]._pValue->SetText("-");
	}
	
	m_pInfoExtension->SetEnable(FALSE);
	m_pBulletValue->SetEnable(FALSE);
	m_pRangeValue->SetEnable(FALSE);

	m_pPreviewWeapon->SetEnable(FALSE);
	m_pDetailWeapon->SetEnable(FALSE);


	if( m_pFocusedGoods == NULL)
		return;

	SHOP_GOODS * pGoods = m_pFocusedGoods->GetGoods(0);
	if( pGoods == NULL)
		return;

	INT32 WeaponID = pGoods->_ItemID;

	WEAPON_CLASS_TYPE ClassType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(WeaponID);
	INT32 WeaponNum = GET_ITEM_FLAG_NUMBER(WeaponID);

	// KEYWORD : WEAPON_MINIAXE	�Ϲ� �������� �ٸ� ���⸦ ��� �ִ� ����Դϴ�. �ش� ���Ⱑ ǥ�õǵ��� ��������� �մϴ�.
	GUI_WEAPON_VALID( ClassType, WeaponNum);

	UINT8	Type = 0;
	UINT32	Arg = 0;
	g_pFramework->GetWeaponTypeByID( WeaponID, &Type, &Arg);
	
	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( ClassType, WeaponNum);
	if( pInfo == NULL)
		return;

	if(g_pFramework->SetWeaponSelectMark(m_pImage, (UINT8)ClassType, (UINT8)WeaponNum))
	{
		m_pInfo->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
		m_pImage->SetEnable(TRUE);
	}

	// ���� �̸�
	pInfo->GetWeaponInfoName( szWeaponName);

	m_pInfoValue->SetText( szWeaponName);
	m_pInfoValue->SetEnable(TRUE);

	//	Endurance zero weapon name is red color
	if( GetEnduranceZero(Type, Arg) )
	{
		m_pInfoValue->SetTextColor( GetColor(GCT_RED));
	}
	else
	{
		m_pInfoValue->SetTextColor( GetColor(GCT_DEFAULT));
	}

	// ī�װ�
	char szString[MAX_STRING_COUNT] = "";
	g_pWeaponInfoDataBase->GetWeaponCategoryEx(pInfo, szString, MAX_STRING_COUNT);

	m_pInfoKey->SetText(szString);
	m_pInfoKey->SetEnable(TRUE);

	// �ͽ��ټ�
//	if (g_pWeaponInfoDataBase->IsExtensionWeapon(pInfo))
//	{
//		m_pInfoExtension->SetEnable(TRUE);
//	}

	// ������
	if (g_pWeaponInfoDataBase->BeDescriptionWeapon(pInfo))
	{		
		m_pDetailWeapon->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pDetailWeapon->SetEnable(TRUE);
	}
	else
	{
		m_pDetailWeapon->SetEnable(FALSE);
	}
	
	//	�̸����� ��ư Ȱ��ȭ/��Ȱ��ȭ
	if( m_nCurViewWeaponID == (UINT32) WeaponID)
	{
		DEACTIVATE_BUTTON(m_pPreviewWeapon);
	}
	else
	{
		ACTIVATE_BUTTON(m_pPreviewWeapon);
	}

	// ������
	char strBuf[MAX_STRING_COUNT];
	INT32 nRate;

	nRate = pInfo->GetPercentage(WEAPON_INFO_DAMAGE);
	if( 0 <= nRate )
	{
		sprintf_s(strBuf, "%d", nRate);
		_ShowWeaponGage(WEAPON_INFO_DAMAGE, nRate);
		m_weaponInfo[WEAPON_INFO_DAMAGE]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_DAMAGE]._pValue->SetText("-");
	}

	// �����
	nRate = pInfo->GetPercentage(WEAPON_INFO_RAPID);
	if( 0 <= nRate)
	{
		sprintf_s(strBuf, "%d", nRate);
		_ShowWeaponGage(WEAPON_INFO_RAPID, nRate);
		m_weaponInfo[WEAPON_INFO_RAPID]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_RAPID]._pValue->SetText("-");
	}

	// ���߷�
	nRate = pInfo->GetPercentage(WEAPON_INFO_HIT_RATE);
	
	if( 0 <= nRate)
	{
		sprintf_s(strBuf, "%d", nRate);
		_ShowWeaponGage(WEAPON_INFO_HIT_RATE, nRate);
		m_weaponInfo[WEAPON_INFO_HIT_RATE]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_HIT_RATE]._pValue->SetText("-");
	}

	// �����ݵ�
	nRate = pInfo->GetPercentage(WEAPON_INFO_VERT_RECOIL);
	if( 0 <= nRate )
	{
		sprintf_s(strBuf, "%d", nRate);
		_ShowWeaponGage(WEAPON_INFO_VERT_RECOIL, nRate);
		m_weaponInfo[WEAPON_INFO_VERT_RECOIL]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_VERT_RECOIL]._pValue->SetText("-");
	}

	// ����ݵ�
	nRate = pInfo->GetPercentage(WEAPON_INFO_HORZ_RECOIL);
	if( 0 <= nRate )
	{
		sprintf_s(strBuf, "%d", nRate);
		_ShowWeaponGage(WEAPON_INFO_HORZ_RECOIL, nRate);
		m_weaponInfo[WEAPON_INFO_HORZ_RECOIL]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_HORZ_RECOIL]._pValue->SetText("-");
	}

	// �̵��ӵ�
	nRate = pInfo->GetPercentage(WEAPON_INFO_SPEED);
	if( 0 <= nRate )
	{
		sprintf_s(strBuf, "%d", nRate);
		_ShowWeaponGage(WEAPON_INFO_SPEED, nRate);
		m_weaponInfo[WEAPON_INFO_SPEED]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_SPEED]._pValue->SetText("-");
	}

	// ��ź��
	{
		pInfo->MakeBulletCountString(szString, MAX_STRING_COUNT);
		m_pBulletValue->SetText(szString);
		m_pBulletValue->SetEnable(TRUE);
	}

	// �����Ÿ�
	{
		pInfo->MakeWeaponRangeString(szString, MAX_STRING_COUNT);
		m_pRangeValue->SetText(szString);
		m_pRangeValue->SetEnable(TRUE);
	}
}

void	CTabWeapon::_UpdateBuyType(void)
{
	if( GOODS_PRICE_ALL == m_SelectItemPriceType )
	{
		m_pBuyWeaponCombo1->SetEnable( TRUE);
		m_pBuyWeaponCombo1->SetCaption(GAME_STRING("STBL_IDX_SHOP_INFO_BUYTYPE_POINT"));

#if defined( BUY_WEAPON_BY_CASH)
		COLOR col;
		i3Color::Set( &col, (UINT8) 244,136,0,255);
		m_pBuyWeaponCombo2->SetCaptionColor(  &col);
		m_pBuyWeaponCombo2->SetEnable( TRUE);
		m_pBuyWeaponCombo2->SetCaption(GAME_STRING("STBL_IDX_SHOP_INFO_BUYTYPE_CASH"));
#endif
	}
	else if( GOODS_PRICE_POINT == m_SelectItemPriceType )
	{
		m_pBuyWeaponCombo1->SetEnable( TRUE);
		m_pBuyWeaponCombo2->SetEnable( FALSE);
		m_pBuyWeaponCombo1->SetCaption(GAME_STRING("STBL_IDX_SHOP_INFO_BUYTYPE_POINT"));
	}
	else if( GOODS_PRICE_CASH == m_SelectItemPriceType )
	{
		m_pBuyWeaponCombo1->SetEnable( TRUE);
		m_pBuyWeaponCombo2->SetEnable( FALSE);
		m_pBuyWeaponCombo1->SetCaption(GAME_STRING("STBL_IDX_SHOP_INFO_BUYTYPE_CASH"));
	}
	else
	{
		m_pBuyWeaponCombo1->SetEnable( FALSE);
		m_pBuyWeaponCombo2->SetEnable( FALSE);
	}

	if( GOODS_PRICE_POINT & m_SelectItemCurrentPriceType )
	{
		m_pBuyWeaponCombo1->SetShapeEnable( 1, TRUE);
	}
#if defined( BUY_WEAPON_BY_CASH)
	else if( GOODS_PRICE_CASH & m_SelectItemCurrentPriceType )
	{
		m_pBuyWeaponCombo2->SetShapeEnable( 1, TRUE);
	}
#endif
}


void	CTabWeapon::_CheckUpdateGoodsFrame(RT_REAL32 rDeltaSec)
{
#if defined(USE_REPAIR_WEAPON)
	if( m_pFocusedGoods == NULL)
		return;

	SHOP_GOODS * pGoods = m_pFocusedGoods->GetGoods(0);
	if( pGoods == NULL)
		return;

	if ( g_pFramework->HaveItem(pGoods->_ItemID) && g_pFramework->GetShopContext()->IsDurabilityItem(pGoods->_ItemID) )
	{
		m_RepairFrame.SetEnable(TRUE);
		m_pGoodsFrame->SetEnable(FALSE);
		// �������� �����ۿ� ��� ���� ������ ǥ���Ѵ�.
		m_RepairFrame.Update(rDeltaSec);
	}
	else
	{
		m_RepairFrame.SetEnable(FALSE);
		m_pGoodsFrame->SetEnable(TRUE);
		// ��ǰ ���� ����� ǥ���Ѵ�.
		_UpdateGoodsFrame(rDeltaSec);
	}
#else
	_UpdateGoodsFrame(rDeltaSec);
#endif
}


void	CTabWeapon::OnChangeGroup(WEAPON_SLOT_TYPE nWeaponUsage, bool ResetVestige/* = false*/)
{
	if (m_nWeaponUsageType == nWeaponUsage)
		return;

	if( ResetVestige )
	{
		_ResetVestige();
	}

	i3GuiButton* pButton = NULL;

	// ���� ��ư�� Ȱ��ȭ��Ų��.
	// �� ��ư�� ó�� ���۽ÿ� NULL�� ��ȯ�� ���Դϴ�.
	pButton = GetGroupButton(m_nWeaponUsageType);
	if ( pButton )
		pButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	// ���� ����Ǵ� �׷��� ��ư�� ��Ȱ��ȭ �Ѵ�.
	pButton = GetGroupButton(nWeaponUsage);
	I3ASSERT( pButton != NULL );
	if ( ! pButton )
		return;
	pButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	// ���ο� �׷� usage�� �����Ѵ�.
	m_nWeaponUsageType = nWeaponUsage;
	m_OldWeaponUsageType = m_nWeaponUsageType;

	// ��ǰ ����� �ٽ� �����.
	MakeGroupList();


	m_pFocusedGoods		= NULL;
	m_idxStartSlot		= 0;

	m_pScrollList->SetCurValue( 0.0f );
	_ApplyScroll();

	m_SelectItemPriceType			= 0;
	m_SelectItemCurrentPriceType	= 0;
	if( m_pGoodsFrame)
		m_pGoodsFrame->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 255);

	UINT32 FirstSelectSlot = _GetFirstSelectItem();

	OnSelectSlot(FirstSelectSlot, TRUE);
}


void	CTabWeapon::MakeGroupList()
{
	//	���͸� ��ģ ��ǰ�� ����Ʈ�� �ִ´�.
	m_RefGoodsList.Clear();

	CShopContext * pShopContext = g_pFramework->GetShopContext();
	INT32 goodsPackCount = pShopContext->GetGoodsPackCount(SHOP_WEAPON_GOODS);

	for(INT32 i = 0 ; i < goodsPackCount ; i++)
	{
		SHOP_GOODS_PACK * pGoodsPack = pShopContext->GetGoods(SHOP_WEAPON_GOODS, i);
		if( pGoodsPack == NULL)
			continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;

		INT32 goodsClass = GET_ITEM_FLAG_CLASS( pGoods->_ItemID);

		switch(m_nWeaponUsageType)
		{
		case WEAPON_SLOT_PRIMARY:
			{
				if( goodsClass != WEAPON_CLASS_ASSAULT &&
					goodsClass != WEAPON_CLASS_SMG &&
					goodsClass != WEAPON_CLASS_SNIPER &&
					goodsClass != WEAPON_CLASS_SHOTGUN &&
					goodsClass != WEAPON_CLASS_MG)
				{
					continue;
				}
			}
			break;
		case WEAPON_SLOT_SECONDARY:
			{
				 if(goodsClass != WEAPON_CLASS_HANDGUN)
					 continue;
			}
			break;
		case WEAPON_SLOT_MELEE:
			{
				if(goodsClass != WEAPON_CLASS_KNIFE)
					 continue;
			}
			break;
		case WEAPON_SLOT_THROWING1:
			{
				if(goodsClass != WEAPON_CLASS_THROWING1)
					 continue;
			}
			break;
		case WEAPON_SLOT_THROWING2:
			{
				if(goodsClass != WEAPON_CLASS_THROWING2)
					 continue;
			}
			break;
		case WEAPON_SLOT_ALL:
		default:
			break;
		}

		m_RefGoodsList.Add((void*) pGoodsPack);
	}
}


i3GuiButton *	CTabWeapon::GetGroupButton(WEAPON_SLOT_TYPE GroupUsage)
{
	i3GuiButton * pButton = NULL;
	switch(GroupUsage)
	{
	case WEAPON_SLOT_UNKNOWN:			pButton = m_pAllGroup;			break;
	case WEAPON_SLOT_PRIMARY:		pButton = m_pPrimaryGroup;		break;
	case WEAPON_SLOT_SECONDARY:	pButton = m_pSecondaryGroup;	break;
	case WEAPON_SLOT_MELEE:		pButton = m_pMeleeGroup;		break;
	case WEAPON_SLOT_THROWING1:		pButton = m_pThrowingGroup;		break;
	case WEAPON_SLOT_THROWING2:			pButton = m_pItemGroup;			break;
	}

	return pButton;
}

WEAPON_SLOT_TYPE	CTabWeapon::_GetFirstGroup()
{
	if( m_bVestigeDuty )
	{
		return m_OldWeaponUsageType;
	}

	return WEAPON_SLOT_ALL;
}

UINT32 CTabWeapon::_GetFirstSelectItem()
{
	if( m_bVestigeDuty )
	{
		m_idxStartSlot = m_OldidxStartSlot;
		return m_OldidxTotalSlot - m_idxStartSlot;
	}

	m_idxStartSlot = 0;
	return 0;
}

void CTabWeapon::_ApplyScroll()
{
	if( m_bVestigeDuty )
	{
		INT32 Index = 0;
		while( Index < m_OldidxStartSlot )
		{
			m_pScrollList->MoveNext();
			Index = m_pScrollList->GetCurrentTop() * m_SlotHoriCount;
		}
	}
}
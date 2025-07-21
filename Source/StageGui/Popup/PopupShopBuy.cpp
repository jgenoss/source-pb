#include "pch.h"
#include "PopupShopBuy.h"
#include "GlobalVariables.h"
#include "PopupUseItem.h"
#include "Shop.h"

#if !defined( ENABLE_UI2)

#if defined (LOCALE_CHINA)
#include "../SDOALink/SDOALink.h"
//#include "CHttpFCMInfo.h"
#endif
//	������ �ð����� ���� ������ ��ٸ��ϴ�.
#define BUY_TIMEOUT			20

I3_CLASS_INSTANCE(CPopupShopBuy, CPopupBase);

CPopupShopBuy::CPopupShopBuy()
{
	i3mem::FillZero( &m_BuyGoods, sizeof( SHOP_GOODS));

	m_pGui = NULL;
	m_pMainForm = NULL;
	m_pMainCaption = NULL;
	m_pButtonOk = NULL;
	m_pButtonCancel = NULL;

	m_pButtonCashOk = NULL;
	m_pButtonCashCancel = NULL;
	m_pButtonCashCharge = NULL;
	m_pButtonCashChargeMark = NULL;
	m_pButtonCashChargeStatic = NULL;

	m_pSaleText = NULL;

	i3mem::FillZero(m_pStaticPoint, sizeof(m_pStaticPoint));

	m_pStaticHavePoint = NULL;
	m_pStaticRemainPoint = NULL;
	m_pValueBuyPoint = NULL;
	m_pValueHavePoint = NULL;
	m_pValueRemainPoint = NULL;

	m_pWeaponInfoFrame = NULL;
	m_pWeaponInfoValue = NULL;
	m_pWeaponInfoKey = NULL;
	m_pWeaponInfoImage = NULL;
	m_pWeaponInfoExtension = NULL;
	m_pWeaponInfoUsed = NULL;

	m_pCharacterInfoFrame = NULL;
	m_pCharacterInfoValue = NULL;
	m_pCharacterInfoImage = NULL;
	m_pCharacterInfoUsed = NULL;

	m_pItemInfoFrame = NULL;
	m_pItemInfoValue = NULL;
	m_pItemInfoImage = NULL;
	m_pItemInfoUsed = NULL;

	m_bConfirm = FALSE;
	m_fTimeout = 0.0f;

	m_pCashSymbol = NULL;
	m_pHaveCashSymbol = NULL;
	m_pRemainCashSymbol = NULL;
}

CPopupShopBuy::~CPopupShopBuy()
{
}

void CPopupShopBuy::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_SHOP_B_POPUP_BUY_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputConfirm();
		break;
	case GCI_SHOP_B_POPUP_BUY_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCancel();
		break;
	case GCI_SHOP_B_POPUP_BUY_CASH_CHARGE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCashCharge();
		break;
	}
}

BOOL CPopupShopBuy::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char temp[256] = "";

	LinkControlEx(plist, "ShopBuy", m_pGui, -1);
	LinkControlEx(plist, "shopBuyPopup", m_pMainForm, -1);
	LinkControlEx(plist, "shopBuyCaption", m_pMainCaption, -1);
	LinkControlEx(plist, "shopBuyOk", m_pButtonOk, GCI_SHOP_B_POPUP_BUY_OK);
	LinkControlEx(plist, "shopBuyCancel", m_pButtonCancel, GCI_SHOP_B_POPUP_BUY_CANCEL);

	LinkControlEx(plist, "shopBuyCashOk", m_pButtonCashOk, GCI_SHOP_B_POPUP_BUY_OK);
	LinkControlEx(plist, "shopBuyCashCancel", m_pButtonCashCancel, GCI_SHOP_B_POPUP_BUY_CANCEL);
	LinkControlEx(plist, "shopBuyCashCharge", m_pButtonCashCharge, GCI_SHOP_B_POPUP_BUY_CASH_CHARGE);	
	LinkControlEx(plist, "shopBuyCashChargeMark", m_pButtonCashChargeMark, -1);
	LinkControlEx(plist, "shopBuyCashChargeStatic", m_pButtonCashChargeStatic, -1);
	
	LinkControlEx(plist, "shopBuyUseSaleText", m_pSaleText, -1);

	for(INT32 i=0; i<STATIC_POINT_COUNT; i++)
	{
		sprintf_s(temp, "shopBuyPointStatic%d", i+1);
		LinkControlEx(plist, temp, m_pStaticPoint[i], -1);
	}

	LinkControlEx(plist, "shopBuyHavePointStatic", m_pStaticHavePoint, -1);
	LinkControlEx(plist, "shopBuyRemainPointStatic", m_pStaticRemainPoint, -1);

	LinkControlEx(plist, "shopBuyPointValue", m_pValueBuyPoint, -1);
	LinkControlEx(plist, "shopBuyHavePointValue", m_pValueHavePoint, -1);
	LinkControlEx(plist, "shopBuyRemainPointValue", m_pValueRemainPoint, -1);

	// �ѱ� N���� �ɺ� �̹���
	LinkControlEx(plist, "shopBuyCashSymbol", m_pCashSymbol, -1);
	LinkControlEx(plist, "shopBuyHaveCashSymbol", m_pHaveCashSymbol, -1);
	LinkControlEx(plist, "shopBuyRemainCashSymbol", m_pRemainCashSymbol, -1);
	
	//	Buy weapon info
	{
		LinkControlEx(plist, "shopBuyWeaponInfo", m_pWeaponInfoFrame, -1);

		LinkControlEx(plist, "shopBuyWeaponValue", m_pWeaponInfoValue, -1);
		LinkControlEx(plist, "shopBuyWeaponKey", m_pWeaponInfoKey, -1);
		LinkControlEx(plist, "shopBuyWeaponDummy", m_pWeaponInfoImage, -1);
		LinkControlEx(plist, "shopBuyWeaponExtension", m_pWeaponInfoExtension, -1);
		LinkControlEx(plist, "shopBuyWeaponUsed", m_pWeaponInfoUsed, -1);
	}

	//	Buy Character info
	{
		LinkControlEx(plist, "shopBuyCharacterInfo", m_pCharacterInfoFrame, -1);

		LinkControlEx(plist, "shopBuyCharacterValue", m_pCharacterInfoValue, -1);
		LinkControlEx(plist, "shopBuyCharacterDummy", m_pCharacterInfoImage, -1);
		LinkControlEx(plist, "shopBuyCharacterUsed", m_pCharacterInfoUsed, -1);
	}

	//	Buy item info
	{
		LinkControlEx(plist, "shopBuyItemInfo", m_pItemInfoFrame, -1);

		LinkControlEx(plist, "shopBuyItemValue", m_pItemInfoValue, -1);
		LinkControlEx(plist, "shopBuyItemDummy", m_pItemInfoImage, -1);
		LinkControlEx(plist, "shopBuyItemUsed", m_pItemInfoUsed, -1);
	}

	return FALSE;
}

void CPopupShopBuy::OnInitControl(void)
{
	char szTemp[256] = "";

	m_pGui->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pMainCaption, GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));

	INIT_CAPTION_BUTTON(m_pButtonOk, GAME_STRING("STBL_IDX_BUTTON_OK"));
	INIT_CAPTION_BUTTON(m_pButtonCancel, GAME_STRING("STBL_IDX_BUTTON_CANCEL"));

	INIT_CAPTION_BUTTON(m_pButtonCashOk, GAME_STRING("STBL_IDX_BUTTON_OK"));
	INIT_CAPTION_BUTTON(m_pButtonCashCancel, GAME_STRING("STBL_IDX_BUTTON_CANCEL"));
	INIT_BUTTON_EX(m_pButtonCashCharge);
	INIT_VALUE_CAPTION2(m_pButtonCashChargeStatic, GAME_STRING("STR_POPUP_CHARGE_CASH"));/*N���� ����*/

	m_pButtonCashOk->SetEnable(FALSE);
	m_pButtonCashCancel->SetEnable(FALSE);
	m_pButtonCashCharge->SetEnable(FALSE);

	m_pButtonCashChargeMark->setInputDisable(TRUE);
	m_pButtonCashChargeStatic->setInputDisable(TRUE);

	m_pSaleText->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);

	sprintf_s(szTemp, "%s :", GAME_STRING("STBL_IDX_SHOP_USE_DAY"));/*��*/
	m_pSaleText->SetTextColor(GetColor(GCT_DEFAULT));

	sprintf_s(szTemp, "%s :", GAME_STRING("STBL_IDX_SHOP_HAVE_MONEY"));/*���� Point*/
	m_pStaticHavePoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pStaticHavePoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pStaticHavePoint->SetText(szTemp);

	sprintf_s(szTemp, "%s :", GAME_STRING("STBL_IDX_SHOP_REMAIN_MONEY"));/*�ܾ�*/
	m_pStaticRemainPoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pStaticRemainPoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pStaticRemainPoint->SetText(szTemp);

	for(INT32 i=0; i<STATIC_POINT_COUNT; i++)
	{
		m_pStaticPoint[i]->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pStaticPoint[i]->SetText(GAME_STRING("STBL_IDX_SHOP_MONEY"));
		m_pStaticPoint[i]->SetTextColor(GetColor(GCT_DEFAULT));
	}


	m_pValueBuyPoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pValueBuyPoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pValueBuyPoint->SetText("0");

	m_pValueHavePoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pValueHavePoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pValueHavePoint->SetText("0");

	m_pValueRemainPoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pValueRemainPoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pValueRemainPoint->SetText("0");

	m_pWeaponInfoFrame->SetEnable(FALSE);
	m_pWeaponInfoFrame->SetAllShapeEnable(FALSE);
	m_pWeaponInfoFrame->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pWeaponInfoKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pWeaponInfoKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pWeaponInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);	
	m_pWeaponInfoValue->SetTextColor(GetColor(GCT_DEFAULT));	
	m_pWeaponInfoExtension->SetEnable(FALSE);
	m_pWeaponInfoUsed->SetEnable(FALSE);	

	m_pCharacterInfoFrame->SetEnable(FALSE);
	m_pCharacterInfoFrame->SetAllShapeEnable(FALSE);
	m_pCharacterInfoFrame->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pCharacterInfoImage->SetEnable(FALSE);
	m_pCharacterInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCharacterInfoValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCharacterInfoUsed->SetEnable(FALSE);		

	m_pItemInfoFrame->SetEnable(FALSE);
	m_pItemInfoFrame->SetAllShapeEnable(FALSE);
	m_pItemInfoFrame->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pItemInfoImage->SetEnable(FALSE);
	m_pItemInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pItemInfoValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pItemInfoUsed->SetEnable(FALSE);		


	for ( INT32 i = 0 ; i < MAX_EFFECT_BTN ; ++i )
		m_BtnEffector[i].SetEffectTime(0.25f);

	m_BtnEffector[EFFECT_BTN_OK].Set(m_pButtonOk);
	m_BtnEffector[EFFECT_BTN_CASH].Set(m_pButtonCashOk);

	m_pCashSymbol->SetEnable(FALSE);
	m_pHaveCashSymbol->SetEnable(FALSE);
	m_pRemainCashSymbol->SetEnable(FALSE);

	CPopupBase::OnInitControl();
}

void CPopupShopBuy::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);

	I3ASSERT(pArg1 != NULL);
	I3ASSERT(pArg2 != NULL);

	SHOP_GOODS * pGoods = (SHOP_GOODS*) pArg1;
	I3ASSERT( pGoods != NULL);


	//
	// SHOP_GOODS�� ���� ������ �����ϴ� �������� �����ϴ�.
	//
	i3mem::Copy( &m_BuyGoods, pGoods, sizeof( SHOP_GOODS));	
	m_BuyGoods._PriceType	= *static_cast<UINT32*>(pArg2);

    m_pGui->SetEnable(TRUE);

	m_bConfirm = FALSE;
	m_fTimeout = 0.0f;

	if ( g_pConfigEx->GetShop().Enable_ChargeCash )
	{
		GOODS_PRICE_TYPE PriceType = m_BuyGoods._PriceType;

		//	ĳ�� ��ǰ�� ��� ĳ�� ���� Ȱ��ȭ
		if( PriceType == GOODS_PRICE_CASH )
		{
			m_pButtonOk->SetEnable(FALSE);
			m_pButtonCancel->SetEnable(FALSE);

			m_pButtonCashOk->SetEnable(TRUE);
			m_pButtonCashCancel->SetEnable(TRUE);
			m_pButtonCashCharge->SetEnable(TRUE);

#if defined( LOCALE_KOREA)
			m_pCashSymbol->SetEnable( TRUE);
			m_pHaveCashSymbol->SetEnable( TRUE);
			m_pRemainCashSymbol->SetEnable( TRUE);
#endif

		}
		else	//	�Ϲ� ����Ʈ ��ǰ
		{
			m_pButtonOk->SetEnable(TRUE);
			m_pButtonCancel->SetEnable(TRUE);

			m_pButtonCashOk->SetEnable(FALSE);
			m_pButtonCashCancel->SetEnable(FALSE);
			m_pButtonCashCharge->SetEnable(FALSE);

#if defined( LOCALE_KOREA)
			m_pCashSymbol->SetEnable( FALSE);
			m_pHaveCashSymbol->SetEnable( FALSE);
			m_pRemainCashSymbol->SetEnable( FALSE);
#endif
		}
	}

	_SetButtonState(I3GUI_CONTROL_STATE_NORMAL);
}

void CPopupShopBuy::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();

	m_pGui->SetEnable(FALSE);

	m_bConfirm = FALSE;
	m_fTimeout = 0.0f;
}

void CPopupShopBuy::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);

	BOOL IsPackageGoods = FALSE;
	CShop* shop = g_pGameContext->GetShop();
	if (shop->IsPackageGoods(m_BuyGoods._GoodsID))
	{
		IsPackageGoods = TRUE;
		_SetPackageGoodsInfo( &m_BuyGoods);
	}



	if (!IsPackageGoods)
	{
		//	��ǰ ������ ���� �з��Ͽ� ��ǰ ��Ͽ� �߰��մϴ�.
		ITEM_TYPE itemClass = (ITEM_TYPE)GET_ITEM_FLAG_TYPE(m_BuyGoods._ItemID);
		switch(itemClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				_SetWeaponInfo( &m_BuyGoods);
			}
			break;
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
			{
				_SetCharacterInfo( &m_BuyGoods);
			}
			break;
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
		case ITEM_TYPE_TEMP:
			{
				_SetItemInfo( &m_BuyGoods);
			}
			break;
		}	
	}

	//	��� ǥ��
	_SetPointInfo( &m_BuyGoods);

	//	������ ���� Ȯ���� ������ �亯�� ���� ������ ��ٸ���. 
	//	���� ��� �ð��� �ʰ��� ��� ������ ���� �亯�� ���޾Ҵٰ� �ǴܵǹǷ� ������ ���� �����մϴ�.
	if(m_bConfirm) 
	{
		//	������ �ð����� ���� ������ ��ٸ���.
		if( m_fTimeout < BUY_TIMEOUT)
		{
			m_fTimeout += rDeltaSeconds;

			GAME_EVENT event = EVENT_NOTHING;
			INT32 arg = 0;

			g_pFramework->GetShopContext()->PopGameEvent( &event, &arg);

			//	�����κ��� ������ �޾����Ƿ� ���� �۵��ϰ� �˾� �����մϴ�.
			if( event == EVENT_BUY_GOODS)
			{
				i3GuiRoot::setGuiDisableInput(FALSE);

				m_bConfirm = FALSE;
				m_fTimeout = 0.0f;

				if( EV_SUCCESSED( arg ))
				{
				//	���� ����
#ifdef AUTH_FROM_BUY_GOODS
					//	���� �ɸ��� ������ ����� ���������� �κ����� ���� ����Դϴ�.
					if( g_pFramework->HaveItem(m_BuyGoods._ItemID) )
					{
						CShop* shop = g_pGameContext->GetShop();

						if (shop->IsPackageGoods(m_BuyGoods._GoodsID))
						{
#if !defined(TEMP_ERASE_CODE)					// Ŀ����Ƽ�� ����...������� �ʴ� �ڵ忡 ���� �� ���� ��ũ��ó��.(11.11.07.����)

							UINT32 point = 0;
							//	��Ʈ��ǰ�� ��������
							SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(m_BuyGoods._GoodsID);
							INT32 ItemCount = pGoodsList->_ui8ItemCount;
							for ( INT32 i = 0 ; i < ItemCount ; ++i )
							{
								if ( ITEM_TYPE_POINT == GET_ITEM_FLAG_TYPE(pGoodsList->_ppItemData[i]->_ui32ItemID) )
								{

									switch(GET_ITEM_FLAG_NUMBER(itemId))
									{
									case 1:	point += 1000;		break;
									case 2: point += 5000;		break;
									case 3: point += 10000;		break;
									case 4: point += 25000;		break;
									case 5: point += 50000;		break;
									case 6: point += 100000;	break;
									case 7: point += 20000;		break;
									}

								}
							}
#endif

#if !defined(TEMP_ERASE_CODE)
							if (0 < point)
							{
								char buyMessage[MAX_STRING_COUNT];

								//	"%d ����Ʈ�� ���޵Ǿ�����\n \n�����Ͻ� �������� �κ��丮����\nȮ���Ͻ� �� �ֽ��ϴ�."
								sprintf_s(buyMessage, GAME_STRING("STBL_IDX_EP_SHOP_BUY_PRICE_SUCCESS"), point);
								CAPTION_POPUP(buyMessage, GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
							}
							else
#endif
							{

								CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
							}

							OnClosePopup();
							return;
						}

//						break;
					}


					//	�ٷ� �������� �����Ѵ�. (������ ������ �ȵ� ��� �����.)
					_AuthGoods( &m_BuyGoods); 
#else
					//	������ �ʿ���� �������� �ٷ� ���� ����
					CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
									GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
#endif
				}
				else
				{
					switch( arg )
					{
					case 0X83000045:
						{
							// �������� �ٲ�鼭 �̹� ������ �ִ� �������̱� ������ ���� ���� -- �����۾� �ʿ� --
							CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_E"), GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));
						}
						break;
					default:
						{
							CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_E"), GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));
						}
						break;
					}
				}

				//	�˾� ����
				OnClosePopup();
			}
		}
		else
		{
			//	��Ʈ��ũ ������ ���Կ� ���������Ƿ� ���� �޽����� ������ ������ ���� �����մϴ�.
			ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_E"), EXIT_CALLBACK(CbExitNormal), this);

			m_bConfirm = FALSE;
			m_fTimeout = 0.0f;

			//	�˾� ����
			OnClosePopup();
		}
	}

#if !defined (LOCALE_KOREA)
	if( m_pButtonOk->isEnable())
		m_BtnEffector[EFFECT_BTN_OK].Update(TRUE, rDeltaSeconds);

	if( m_pButtonCashOk->isEnable())
		m_BtnEffector[EFFECT_BTN_CASH].Update(TRUE, rDeltaSeconds);
#endif
}

void CPopupShopBuy::OnPreGuiNotify(void)
{
}

void CPopupShopBuy::OnExitKey(void)
{
	OnClosePopup();
}

void CPopupShopBuy::SendBuyEvent(SHOP_GOODS * pBuyGoods)
{
	//	�������� ��ǰ ���� �̺�Ʈ�� �����ϴ�.
	g_pFramework->GetShopContext()->BuyGoods(pBuyGoods);

	//	������ ���� ���Կ� ���� �亯�� ���� ������ ������ ������ �Է��� �����ϴ�. �ߺ� ���� ���׸� ���� ����. komet
	i3GuiRoot::setGuiDisableInput(TRUE);

	m_bConfirm = TRUE;
	m_fTimeout = 0.0f;

	// �����Ҵ���� ��ư�� ��Ȱ��ȭ�Ѵ�.
	_SetButtonState(I3GUI_CONTROL_STATE_DISABLED);
}

void CPopupShopBuy::_InputConfirm(void)
{
	//	���� ������� �˻�
	if( ! g_pFramework->GetShopContext()->CheckBuy(m_BuyGoods._GoodsID, m_BuyGoods._PriceType) )
	{
		//	�ܾ��� �����Ͽ� ��ǰ�� ������ �� �����ϴ�.
		CAPTION_POPUP( GAME_STRING("STBL_IDX_EP_SHOP_DEFICIT_MONEY"), 
						GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));

		OnClosePopup();
		return;
	}	


	CShop* shop = g_pGameContext->GetShop();
	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(m_BuyGoods._GoodsID);

	if (pGoodsList && pGoodsList->_ui8ItemCount > 1 )
	{
		INT32 ItemCount = pGoodsList->_ui8ItemCount;
		for(INT32 j = 0; j < ItemCount ; j++)
		{
			BOOL	IsHave	= g_pFramework->GetInvenList()->HaveItem(pGoodsList->_ppItemData[j]->_ui32ItemID);
			if (IsHave)
			{
				CAPTION_POPUP_Q(GAME_STRING("STR_POPUP_Q_MESSAGE_BUY_SET_GOODS"), GAME_STRING("STR_POPUP_CAPTION_CONFIRM_BUY"), MAKE_CALLBACK(CbBuyForHavingGoods), this);
				return;
			}
		}

		SendBuyEvent( &m_BuyGoods);	
		return;
	}



	// �������� �������� �ƴ϶�� �ٷ� ����.
	if ( ! g_pFramework->GetInvenList()->HaveItem(m_BuyGoods._ItemID) )
	{
		SendBuyEvent( &m_BuyGoods);	
		return;
	}


	// ������ �̹� ���� �� ������� �������̸� �ٽ��ѹ� ���´�.
	AUTH_TYPE 	type = shop->GetAuthType( m_BuyGoods._ItemID );

	I3ASSERT(type == AUTH_SEC || type == AUTH_COUNT);
	if( type == AUTH_SEC)  
	{
		/*�������� �������Դϴ�.\n���Խ� �����Ͻ� �Ⱓ��ŭ ��������\n�������� ���� �Ⱓ�� �����մϴ�.\n \n�����Ͻðڽ��ϱ�?*//*����Ȯ��*/
		CAPTION_POPUP_Q(GAME_STRING("STR_POPUP_Q_MESSAGE_BUY_MAINTENANCE_ITEM"), 
			GAME_STRING("STR_POPUP_CAPTION_CONFIRM_BUY"), MAKE_CALLBACK(CbBuyForHavingGoods), this);
	}
	else if ( type == AUTH_COUNT)
	{
		/*�������� �������Դϴ�.\n���Խ� �����Ͻ� ������ŭ ��������\n�������� ������ �����մϴ�.\n \n�����Ͻðڽ��ϱ�?*//*����Ȯ��*/
		CAPTION_POPUP_Q(GAME_STRING("STR_POPUP_Q_MESSAGE_BUY_EXPENDABLE_ITEM"), 
			GAME_STRING("STR_POPUP_CAPTION_CONFIRM_BUY"), MAKE_CALLBACK(CbBuyForHavingGoods), this);
	}


	return;
}

void CPopupShopBuy::_InputCancel(void)
{
	//	����
	OnClosePopup();
}

void CPopupShopBuy::_InputCashCharge(void)
{
	//	ĳ�� ���� �������� ȣ��
#if !defined(LOCALE_CHINA)
	::ExcuteCashWeb();
#else
	g_pSDOA_Link->OpenWidget();
#endif
}

void CPopupShopBuy::_SetWeaponInfo(SHOP_GOODS * pGoods)
{		
	m_pCharacterInfoFrame->SetEnable(FALSE);
	m_pItemInfoFrame->SetEnable(FALSE);

	SetShopWeapon( pGoods->_ItemID, m_pWeaponInfoFrame, m_pWeaponInfoImage, m_pWeaponInfoKey, m_pWeaponInfoValue, m_pWeaponInfoExtension);
}

void CPopupShopBuy::_SetCharacterInfo(SHOP_GOODS * pGoods)
{	
	m_pWeaponInfoFrame->SetEnable(FALSE);
	m_pItemInfoFrame->SetEnable(FALSE);

	SetShopCharacter( pGoods->_ItemID, m_pCharacterInfoFrame, m_pCharacterInfoImage, m_pCharacterInfoValue);
}

void CPopupShopBuy::_SetItemInfo(SHOP_GOODS * pGoods)
{		
	m_pCharacterInfoFrame->SetEnable(FALSE);
	m_pWeaponInfoFrame->SetEnable(FALSE);

	SetShopCashItem( pGoods->_ItemID, m_pItemInfoFrame, m_pItemInfoImage, m_pItemInfoValue);
}

void CPopupShopBuy::_SetPackageGoodsInfo(SHOP_GOODS* pGoods)
{
	m_pCharacterInfoFrame->SetEnable(FALSE);
	m_pWeaponInfoFrame->SetEnable(FALSE);

	SetPackageGoods(pGoods->_GoodsID, m_pItemInfoFrame, m_pItemInfoImage, m_pItemInfoValue);
}

void CPopupShopBuy::_SetPointInfo(SHOP_GOODS * pGoods)
{
	SetShopPointInfo( pGoods, m_pSaleText,
					m_pValueBuyPoint, m_pValueHavePoint, m_pValueRemainPoint,
					m_pStaticPoint, STATIC_POINT_COUNT);
}

BOOL CPopupShopBuy::_AuthGoods(SHOP_GOODS * pGoods)
{	
	I3ASSERT( pGoods!= NULL);

	// ��뿩��
	UINT8 itemAttr = 0;
	UINT32 itemArg = 0;

	INT64	i64UID		= ItemIDToWareDBIndex(pGoods->_ItemID);
	INT32	gameItemId	= pGoods->_ItemID;
	ITEM_TYPE goodsClass = (ITEM_TYPE)GET_ITEM_FLAG_TYPE(gameItemId);

	//	�ٷ� �������� �����Ѵ�. (������ ������ �ȵ� ��� �����.)
	switch( goodsClass)						
	{
		//	���Ӽ�
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				//	������� �ƴϸ�
				if( !g_pFramework->IsUsedWeapon(gameItemId))
				{
					const char * pMessage = GAME_STRING("STR_POPUP_Q_MESSAGE_EQUIP");	/*�ٷ� ��� �Ͻðڽ��ϱ�?*/

					SHOP_ITEM_DATA* pItem = g_pGameContext->GetShop()->GetItemData( gameItemId );
					switch( pItem->_ui8AuthType )
					{
					case AUTH_COUNT:	// ������
						{
							STRING_POPUP_Q( pMessage, MAKE_CALLBACK(CbEquipGoods), this);
							return TRUE;
						}
						break;
					case AUTH_SEC:	// �Ⱓ��
						{
							g_pFramework->GetWeaponTypeByID(gameItemId, &itemAttr, &itemArg);
							if(ITEM_ATTR_TYPE_BUY == itemAttr)
							{
								if( i64UID != INVALID_WAREDB_INDEX )
								{
									STRING_POPUP_Q( pMessage, MAKE_CALLBACK(CbAuthGoods), this);
									return TRUE;
								}
							}
							else
							{
								STRING_POPUP_Q( pMessage, MAKE_CALLBACK(CbEquipGoods), this);
								return TRUE;
							}
						}
						break;
					}
				}
			}
			break;
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:	
			{
				const char * pMessage = GAME_STRING("STR_POPUP_Q_MESSAGE_CHANGE");	/*�ٷ� ���� �Ͻðڽ��ϱ�?*/

				//	������� �ƴϸ�
				BOOL bNeedEquip = !g_pFramework->IsUsedCharacter(gameItemId);

				BOOL bHasInven = g_pFramework->GetCharacterType(gameItemId, &itemAttr, &itemArg);
				
				if (bHasInven)
				{
					//	�̹� ����ϰ� �־ �� ĳ���� ���� ��ǰ�� ���� �ٸ��� ����ؾ� �Ѵ�.
					if( g_pFramework->GetCurrentStage()->IsExactTypeOf( CStageReady::GetClassMeta()))
					{
						CStageReady * pStage = (CStageReady *) g_pFramework->GetCurrentStage();
						
						INT32 nTeam = GET_ITEM_FLAG_CLASS(gameItemId);
						if(CHARACTER_TEAM_RB != nTeam &&
							pStage->GetViewCharaInfo()->_charaInfo.GetTeam() != nTeam)
							bNeedEquip = TRUE;
					}
				}

				if (bNeedEquip)
				{
					SHOP_ITEM_DATA* pItem = g_pGameContext->GetShop()->GetItemData( gameItemId );
					switch( pItem->_ui8AuthType )
					{
					case AUTH_COUNT:	// ������
						STRING_POPUP_Q(pMessage, MAKE_CALLBACK(CbEquipGoods), this);
						return TRUE;
						break;
					case AUTH_SEC:
						if(ITEM_ATTR_TYPE_BUY == itemAttr)
						{
							if( i64UID != INVALID_WAREDB_INDEX )
							{
								STRING_POPUP_Q( pMessage, MAKE_CALLBACK(CbAuthGoods), this);
								return TRUE;
							}
						}
						else
						{
							STRING_POPUP_Q( pMessage, MAKE_CALLBACK(CbEquipGoods), this);
							return TRUE;
						}
						break;
					}
				}
			}
			break;
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
			{
				INVEN_ITEM_TYPE InvenItemType = g_pFramework->GetInvenList()->GetInvenItemType(i64UID);
				if( InvenItemType == INVEN_ITEM_CASHITEM)
				{
					CbAuthGoods( this, MBR_OK);	//	�ٷ� ��뿩�θ� �����.

					return TRUE;
				}
			}
			break;
	}

	//	��� �ʿ����� ���� ���
	CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
					GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));

	return FALSE;
}

void	CPopupShopBuy::_SetButtonState(GUI_CONTROL_STATE State)
{
	// ��ư�� ���¸� �����Ѵ�.
	m_pButtonOk->setControlState(State);
	m_pButtonCashOk->setControlState(State);
	m_pButtonCancel->setControlState(State);
	m_pButtonCashCancel->setControlState(State);
}

void CPopupShopBuy::CbBuyForHavingGoods(void * pThis, INT32 nParam)
{	
	CPopupShopBuy * pPopup = (CPopupShopBuy *) pThis;

	if( nParam == MBR_OK)
	{
		//	���� �̺�Ʈ�� ������ ������.
		pPopup->SendBuyEvent( pPopup->GetBuyGoods());
	}
	else
	{
		pPopup->OnClosePopup();
	}
}

void CPopupShopBuy::CbAuthGoods(void* pThis, INT32 nParam)
{
	CPopupShopBuy * pPopup = (CPopupShopBuy *) pThis;

	SHOP_GOODS * pGoods = pPopup ->GetBuyGoods();
	I3ASSERT( pGoods != NULL);

	INT32 goodsClass = GET_ITEM_FLAG_TYPE(pGoods->_ItemID);

	if( nParam == MBR_OK)
	{	
		INT64 i64UID = ItemIDToWareDBIndex(pGoods->_ItemID);

		switch((ITEM_TYPE) goodsClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
			{
				//	������ �������� ������ �䱸
				g_pGameContext->SetEvent(EVENT_ITEM_AUTH, &i64UID);
			}
			break;
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
			{
#if defined(CHANGE_ITEM_USE_CHECK)
				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(pGoods->_ItemID);
				if ( pInfo != NULL && pInfo->GetUsePosition() == USE_POS_ANYWHERE )
				{
					USE_ITEM_STYLE style = USE_ITEM_STYLE_BUYING;

					//	�������� ������� ����
					((CStageReady*)pPopup->getParent())->TogglePopup(POPUP_USE_ITEM, &pGoods->_ItemID, &style);
				}
				else
				{
					CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
						GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.*/
				}
#else
				//	Ŭ�� ���� �������� �������̸� �������� ������� ���ϵ��� 
				if( CheckCashItemOnlyClanManagementStep( pGoods->_ItemID))
				{
					CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
									GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.*/
				}
				//	�ΰ��ӿ��� ����ϴ� ĳ�� ���������� ���� �Ǵ�.
				else if( CheckCashItemOnlyInGame( pGoods->_ItemID)) 
				{
					CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
									GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.*/
				}
				else
				{
					USE_ITEM_STYLE style = USE_ITEM_STYLE_BUYING;

					//	�������� ������� ����
					((CStageReady*)pPopup->getParent())->TogglePopup(POPUP_USE_ITEM, &pGoods->_ItemID, &style);
				}
#endif
			}
			break;
		}
	}
	else
	{
		switch((ITEM_TYPE) goodsClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				//	������ ������ ���� ���
				CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_ITEM_BUY_COMPLETE_1"), 
							GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*�����Ͻ� �������� �κ��丮����\nȮ���Ͻ� �� ������ ������ �ѹ���\n������� ���� �̻�� �����\n���� ���߿� ��ü�� �� �����ϴ�.*/
			}
			break;
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
		case ITEM_TYPE_TEMP:
		default:
			{
				//	������ ������ ���� ���
				CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
							GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
			}
			break;
		}
	}
}

void CPopupShopBuy::CbEquipGoods(void* pThis, INT32 nParam)
{
	CPopupShopBuy * pPopup  = (CPopupShopBuy *) pThis;

	SHOP_GOODS * pGoods = pPopup->GetBuyGoods();
	I3ASSERT( pGoods != NULL);

	//	�������� �κ��� ���
	if( nParam == MBR_OK)
	{
		if( g_pFramework->GetShopContext()->EquipGoods( pGoods))
			pPopup->PlaySound(GUI_SND_ITEM_EQUIP);	//	�����ϸ� ����
	}
	else
	{
		//	��� ������ ���� ���
		CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
						GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
	}
}

#endif // ENABLE_UI2


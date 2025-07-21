#include "pch.h"
#include "UITabProfileMissionCard_V10.h"
#include "UIFloatMissionBoard_V10.h"
#include "UI/UIMainFrame_V10.h"
#include "UIPhaseProfile_V10.h"
#include "UI/UIUtil.h"
#include "../StepProfileDef.h"
#include "../CommunityContext.h"
#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCardSet.h"
#include "MissionCard/MCardSetInfo.h"
#include "MissionCard/MCard.h"
#include "MissionCard/MCardInfo.h"
#include "MissionCard/MCardQuest.h"

#include "UserInfoContext.h"

static UITabProfileMissionCard_V10 * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	I3ASSERT( pFrame);

	UIPhaseSubBase* pSubPhase = pFrame->GetCurrentSubPhase();

	UITabProfileBase_V10* pTab = static_cast<UIPhaseProfile_V10*>(pSubPhase)->GetMissionCard();

	I3ASSERT( i3::same_of<UITabProfileMissionCard_V10*>(pTab)); 

	return static_cast<UITabProfileMissionCard_V10*>(pTab);
}


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UITabProfileMissionCard_V10_SelectCard( LuaState * L)
	{
		INT32 Idx = i3Lua::GetIntegerArg(L, 1);
		GetThis()->SelectCardSetType_Server(Idx);
		return 0;
	}

	int UITabProfileMissionCard_V10_CardInfo_Confirm( LuaState * L)
	{
		GetThis()->CardInfo();
		return 0;
	}

	int UITabProfileMissionCard_V10_CardBuy_Confirm( LuaState * L)
	{
		GetThis()->CardBuy();
		return 0;
	}

	int UITabProfileMissionCard_V10_CardDelete_Confirm( LuaState * L)
	{
		GetThis()->CardDelete();
		return 0;
	}
}

LuaFuncReg UITabProfileMissionCard_V10_Glue[] =
{
	{"ClickInfoCard",				UITabProfileMissionCard_V10_CardInfo_Confirm		},
	{"ClickBuyCard",				UITabProfileMissionCard_V10_CardBuy_Confirm			},
	{"ClickDeleteCard",				UITabProfileMissionCard_V10_CardDelete_Confirm		},
	{"SelectCard",					UITabProfileMissionCard_V10_SelectCard				},
	{nullptr,					nullptr}
};

I3_CLASS_INSTANCE( UITabProfileMissionCard_V10);

UITabProfileMissionCard_V10::UITabProfileMissionCard_V10() : m_selectedCardSetIndexFromList(-1), m_nextSetTypeForDelete(QUEST_CARDSET_TYPE_NONE)
{
	::memset(m_CardSlot, 0, sizeof(m_CardSlot));
	::memset(m_CardBG, 0, sizeof(m_CardBG));

	m_pCardInfo = nullptr;
}

UITabProfileMissionCard_V10::~UITabProfileMissionCard_V10()
{

}

/*virtual*/ void UITabProfileMissionCard_V10::_InitializeAtLoad( i3UIScene * pScene)
{
	UITabProfileBase_V10::_InitializeAtLoad(pScene);

	char szTemp[MAX_STRING_COUNT];
	
	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		sprintf_s(szTemp, "%d", i);
		m_CardSlot[i] = (i3UIButtonImageSet *)pScene->FindChildByName(szTemp);
		//m_CardSlot[i]->SetViewOnly(true);
	}
	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		sprintf_s(szTemp, "BG%d", i);
		m_CardBG[i] = (i3UIButtonImageSet *)pScene->FindChildByName(szTemp);
		m_CardBG[i]->SetViewOnly(true);
	}

	m_pCardInfo = (i3UIButtonImageSet*) pScene->FindChildByName("InfoCard");
	if(m_pCardInfo != nullptr)
		m_pCardInfo->SetViewOnly(true);

	GetSlide()->AddFrameWnds(GetScene(0));
}

void UITabProfileMissionCard_V10::OnLoadAllScenes()
{
	InitCardList_Lua();			// ������ ī���� ����Ʈ..

	UpdateCardSetListSelection();

	UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI();

	////ī�� ī��Ʈ�� ���
	//SetMissionPossibleCount_Lua();
}



void	UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI()
{

	// InitButtonState
	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
		m_CardBG[i]->SetShapeIdx(I3UI_BUTTON_SHAPE_NORMAL);

	//�����ϰ� �ִ� ī�带 �������ִ� CardSet�̶�� ī�� ����Ʈ���� ī�� Ÿ��(����)�� ������ �� �ش� ����ī���� ��Ŀ�� �� ������ ����
	INT32 cardIdx = -1;
	bool infocard = false;
	
	if ( MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList) ) // �Ŵ����� �����ǰ� UI�������� �ٸ��� ����..
	{
		const MCard* pCard = MCardUtil::GetSelectedMCard();

		if (pCard)
		{
			cardIdx = pCard->GetMyIndex();
			m_CardBG[cardIdx]->SetShapeIdx(I3UI_BUTTON_SHAPE_PUSHED);
			
			infocard = true;
			m_pCardInfo->SetShapeIdx(cardIdx);
		}
	}
	else
		infocard = false;

	LuaState * L = _CallLuaFunction("SetInfoCardEnable");
	i3Lua::PushBoolean(L, infocard);
	_EndLuaFunction(L,1);

	// Pushed�� �ٲ� �� MissionBoard�� ���� �ٲߴϴ�.
	_SetMissionBoard();
	UpdatePrize_Lua();				// ���� �ϴܿ� ������ ǥ���Ѵ�.
	_UpdateChallengeCardInfo();	// ����,������, ���� ��ư ǥ�ÿ���
	_UpdateChallengeSlotState();	// ��� �ϴ� �ӹ�ī�� ���� ����
	_UpdateCardSlotImage();		//
}

/*virtual*/ void UITabProfileMissionCard_V10::OnUnloadAllScenes()
{

}

namespace
{


	bool CheckEventCardSlot(i3UIControl* m_pCtrl, i3UIButtonImageSet* (&aCardSlot)[ MAX_CARD_PER_CARDSET ])
	{
		for(INT32 i=0; i < MAX_CARD_PER_CARDSET; i++)
		{
			if(m_pCtrl == aCardSlot[i])
				return true;
		}
		return false;
	}

}


/*virtual*/ bool UITabProfileMissionCard_V10::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/ )
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pParam = (I3UI_CONTROL_NOTIFY*) param2;
		
		if( pParam->m_nEvent == I3UI_EVT_CLICKED && CheckEventCardSlot(pParam->m_pCtrl, m_CardSlot))
		{
			// ���� ī����� �ƴ� ����� 10�� ī�弱���̶�� ó�� ����..
			if (MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList) )
			{
				I3ASSERT( pParam->m_pCtrl != nullptr);

				MCard* pCard = MCardUtil::GetSelectedMCard();

				INT32 prevSelectedCardSlot = (pCard) ? pCard->GetMyIndex() : -1;
				INT32 newSelectedCardSlot = i3::atoi( pParam->m_pCtrl->GetName());

				m_CardBG[newSelectedCardSlot]->SetShapeIdx(I3UI_BUTTON_SHAPE_PUSHED);

				if( newSelectedCardSlot != prevSelectedCardSlot)
				{
					if (prevSelectedCardSlot >= 0 && prevSelectedCardSlot < MAX_CARD_PER_CARDSET)
						m_CardBG[prevSelectedCardSlot]->SetShapeIdx(I3UI_BUTTON_SHAPE_NORMAL);
				}

				SelectCard(newSelectedCardSlot);
			}
		}
		//else if( pParam->m_nEvent == I3UI_EVT_ENTERMOUSE && CheckEventCardSlot(pParam->m_pCtrl, m_CardSlot) )
		//{
		//	INT32 idxCard = i3::atoi(pParam->m_pCtrl->GetName());

		//	m_CardBG[idxCard]->SetShapeIdx(I3UI_BUTTON_SHAPE_PUSHED);
		//}
		//else if( pParam->m_nEvent == I3UI_EVT_LEAVEMOUSE && CheckEventCardSlot(pParam->m_pCtrl, m_CardSlot) )
		//{
		//	INT32 idxCard = i3::atoi(pParam->m_pCtrl->GetName());

		//	MCard* pCard = MCardUtil::GetSelectedMCard();
		//	INT32 selCardIdx =  (pCard) ? pCard->GetMyIndex() : -1;

		//	if( selCardIdx != idxCard )
		//		m_CardBG[idxCard]->SetShapeIdx(I3UI_BUTTON_SHAPE_NORMAL);
		//}
	}

	return UITabProfileBase_V10::OnEvent( event, pObj, param2, code);
}

/*virtual*/ bool UITabProfileMissionCard_V10::OnClick( i3UIControl * pControl)
{
	if( UITabProfileBase_V10::OnClick( pControl) == false)
		return false;

	if( i3::same_of<i3UIListView_Item*>(pControl)) 
	{
		PlaySound( UISND_CLICK);
	}

	return true;
}

//
void UITabProfileMissionCard_V10::UpdatePrize_Lua()
{
	if( IsReadyToUpdate() == false) return;

	// ������ ǥ���Ѵ�.
	INT32 prize = 0;
	INT32 point = 0;
	INT32 exp = 0;
//
		
//	MCardSet* pCardSet = MCardUtil::GetSelectedMCardSet();
//	MCard* pCard = MCardUtil::GetSelectedMCard();
		
	// ���õ� ���� ������ ������ ���� ���õ� ��Ʈ�� ��쿡�� ǥ��????
	if ( MCardUtil::IsServerListIdxCurrentSelectedSet( m_selectedCardSetIndexFromList ) )
	{	
		MCard* pCard = MCardUtil::GetSelectedMCard();

		if (pCard)
		{
			const MCardInfo* pCardInfo = pCard->GetMCardInfo();
			prize = pCardInfo->mCardPrize.prize;
			point  = pCardInfo->mCardPrize.point;
			exp    = pCardInfo->mCardPrize.exp;
		}
	}

	BADGE_TYPE BadgeType = PrizeToBadgeType((UINT8)prize);

	//Badge Type Lua�̹��� ���
	LuaState * L = _CallLuaFunction("SetBadgeType");
	i3Lua::PushInteger(L, (INT32)BadgeType);
	_EndLuaFunction(L, 1);

	const i3::rc_wstring descriptionText = MCardUtil::GetPrizeDescription(BadgeType);
	i3::rc_wstring wstrNameText;

	switch (BadgeType)
	{
	case BADGE_TYPE_RIBBON:
		wstrNameText = GAME_RCSTRING("STR_TBL_PROFILE_RIBBON_TEXT2");	/*����*/
	break;

	case BADGE_TYPE_ENSIGN:
		wstrNameText = GAME_RCSTRING("STR_TBL_PROFILE_ENSIGN_TEXT2");	/*����*/
	break;

	case BADGE_TYPE_MEDAL:
		wstrNameText = GAME_RCSTRING("STR_TBL_PROFILE_MEDALS_TEXT2");	/*����*/
	break;

	case BADGE_TYPE_MASTER:
		wstrNameText = GAME_RCSTRING("STR_TBL_PROFILE_MEDALS_TEXT2");	/*����*/
	break;
	}

	i3::stack_wstring pointText;
	i3::stack_wstring expText;

	i3::itoa(point, pointText);
	i3::itoa(exp, expText);

	L = _CallLuaFunction( "SetPrizeName");
	i3Lua::PushStringUTF16To8( L, wstrNameText);
	_EndLuaFunction( L, 1);

	L = _CallLuaFunction( "SetPrizeDescription");
	i3Lua::PushStringUTF16To8( L, descriptionText);
	_EndLuaFunction( L, 1);

	L = _CallLuaFunction( "SetPrizePoint");
	i3Lua::PushStringUTF16To8( L, pointText);
	_EndLuaFunction( L, 1);

	L = _CallLuaFunction( "SetPrizeExp");
	i3Lua::PushStringUTF16To8( L, expText);
	_EndLuaFunction( L, 1);

}

//

void UITabProfileMissionCard_V10::SelectCard( INT32 cardIdx) // ī���ǿ��� ī�带 ����������.
{
	// �ش� ī����� ���� �������ΰ� �ƴ� ���̽����..
	if ( MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList) == false )
	{
		return;
	}

	if( cardIdx < 0 || cardIdx >= MAX_CARD_PER_CARDSET )
		return;

	bool bSelectEnable = false;

	MCardSet* pSet = MCardUtil::GetSelectedMCardSet();

	if (pSet != nullptr)
	{
		MCard* pCard = pSet->GetMCard(cardIdx);
		bSelectEnable = pCard->IsSelectEnableCard();
	}

	// ���� ī�� ���� �� Ȱ��ȭ
	if( bSelectEnable )
	{
		MCardUtil::SelectActiveCardToServer( cardIdx );
	}
	else
	{
		INT32 cardSetType = (pSet) ? pSet->GetMCardSetInfo()->setID : QUEST_CARDSET_TYPE_NONE;
		// Ʃ�丮�� �ӹ�ī��� ���� ��� ���þȵǰ��Ѵ�.
		if ( QUEST_CARDSET_TYPE_NONE == cardSetType
			|| QUEST_CARDSET_TYPE_TUTORIAL == cardSetType)
		{
			LuaState * L = _CallLuaFunction("SetAllDefault");
			_EndLuaFunction(L,0);
		}
	}
	
	UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI();

}

//
void UITabProfileMissionCard_V10::SelectCardSetType_Server(INT32 serverIdx) // ����Ʈ�ڽ����� ī�带 ���������� ȣ��
{
	m_selectedCardSetIndexFromList = serverIdx;

	INT32 cardSetType = MCardUtil::GetMCardSetType_Server(serverIdx);
	INT32 slotIdx = MCardUtil::FindCardSetSlotWithCardSetType(cardSetType);

	if (slotIdx != -1)		// -1�� ���� �����ϰ� ���� ����... �����Ѱ� �ƴϸ� ������ �������� ���� �ʿ����..
	{
		MCardUtil::SelectActiveCardSetToServer(slotIdx);
	}
	
	UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI();
}


namespace
{
	enum CARD_SET_TEXT_COLOR
	{
		CARD_SET_USER_HAVE = 0,
		CARD_SET_TUTORIAL,
		CARD_SET_DEFAULT,
		CARD_SET_TEXT_COLOR_MAX,
	};
}

void	UITabProfileMissionCard_V10::InitCardList_Lua()
{
	// ����� ���� �׳� ����� ����..
	LuaState * L = _CallLuaFunction("ClearMissionList");
	_EndLuaFunction(L,0);

	//	�� ���󺰷� �̺�Ʈ �ӹ�ī�带 ������� �ʴ´ٸ� Config/ConfigEx.PEF ���Ͽ��� 
	//	QuestCard �׸��� 13_EVENT ���� ���� 0���� �ϰ� ������ �����ϸ� �ش� Ŭ���̾�Ʈ������ �̺�Ʈ �ӹ�ī�尡 �Ⱥ��Դϴ�.

	const INT32 currentCardSetCount_Server = MCardUtil::GetMCardSetCount_Server();
	i3::rc_wstring CardSetNameColor;

	for( INT32 i = 0; i < currentCardSetCount_Server ; ++i)
	{
		INT32 cardSetType = MCardUtil::GetMCardSetType_Server(i);
		bool HasCard = MCardUtil::HasMCardSet(cardSetType);
		
		INT32 CardColorType = -1;
		if (HasCard)
		{
			CardColorType = CARD_SET_USER_HAVE;
		}
		else
		{
			if (QUEST_CARDSET_TYPE_TUTORIAL == cardSetType || QUEST_CARDSET_TYPE_DINO_TUTORIAL == cardSetType ||
				QUEST_CARDSET_TYPE_HUMAN_TUTORIAL == cardSetType)
				CardColorType = CARD_SET_TUTORIAL;
			else
				CardColorType = CARD_SET_DEFAULT;
		}

		switch(CardColorType)
		{
		case CARD_SET_USER_HAVE:
			CardSetNameColor += L"{col:255,211,3}";
			break;
		case CARD_SET_TUTORIAL:
			CardSetNameColor += L"{col:155,155,155}";
			break;
		case CARD_SET_DEFAULT:
			CardSetNameColor += L"{col:255,255,255}";
			break;
		}
		
		const MCardSetInfo* pInfo =	MCardUtil::FindMCardSetInfoBySetType(cardSetType);

		if (pInfo == nullptr)
			pInfo = MCardUtil::FindMCardSetInfoBySetType(QUEST_CARDSET_TYPE_TUTORIAL);
		
		I3ASSERT(pInfo != nullptr);
		
		//const i3::rc_wstring& wstrCardSetName = pInfo->wstrName;
		CardSetNameColor += pInfo->wstrName;
		CardSetNameColor += L"{/col};";
	}

	// Text Setting
	L = _CallLuaFunction("SetCardList");
	i3Lua::PushStringUTF16To8(L, CardSetNameColor);
	i3Lua::PushInteger(L, 0);
	_EndLuaFunction(L, 2);
	
	if(currentCardSetCount_Server == 0)
	{
		L = _CallLuaFunction("SetCardList");
		i3Lua::PushStringUTF16To8(L, L"Mission card's not detected.");
		i3Lua::PushInteger(L, 0);
		_EndLuaFunction(L, 2);

		L = _CallLuaFunction("SetAllDefault");
		_EndLuaFunction(L,0);
	}
}



void UITabProfileMissionCard_V10::_ChangeActiveCardSet(INT32 cardSetType)
{
	INT32 slotIdx = MCardUtil::FindCardSetSlotWithCardSetType(cardSetType);

	if (slotIdx != -1)
	{
		MCardUtil::SelectActiveCardSetToServer(slotIdx);
	}
}
//

void UITabProfileMissionCard_V10::_UpdateChallengeCardInfo()
{
	if( IsReadyToUpdate() == false) return;

	INT32 cardSetType = MCardUtil::GetMCardSetType_Server(m_selectedCardSetIndexFromList);
	
	if (cardSetType == QUEST_CARDSET_TYPE_NONE )	// ����Ʈ�������� �½��԰� �ٸ�..
	{
		LuaState * L;

		L = _CallLuaFunction("SetBuyButtonEnable");
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L, 1);
	
		L = _CallLuaFunction("SetInfoButtonEnable");
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L, 1);
	
		L = _CallLuaFunction("SetDeleteButtonEnable");
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L, 1);
		return;
	}
	
	// ���� �������� ī��� ���� ǥ��  ( ������ �ʾƼ� ������ ����...2015.06.19.����)
	
	// Ʃ�丮��, �̺�Ʈ ī���� ��� ������ �� ����.

		
	if( QUEST_CARDSET_TYPE_TUTORIAL == cardSetType || QUEST_CARDSET_TYPE_EVENT == cardSetType)
	{
		{
			LuaState * L = _CallLuaFunction("SetBuyButtonEnable");
			i3Lua::PushBoolean(L, false);
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetInfoButtonEnable");
			i3Lua::PushBoolean(L, true);
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetDeleteButtonEnable");
			i3Lua::PushBoolean(L, false);
			_EndLuaFunction(L, 1);
		}
	}
	else
	{
		{
			LuaState * L = _CallLuaFunction("SetBuyButtonEnable");
			i3Lua::PushBoolean(L, true);
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetInfoButtonEnable");
			i3Lua::PushBoolean(L, true);
			_EndLuaFunction(L, 1);
		}

		// ī����� ������ �������� ������ �����ϴ�.
		{
			bool bDeleteEnable = MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList);
			LuaState * L = _CallLuaFunction("SetDeleteButtonEnable");
			i3Lua::PushBoolean(L, bDeleteEnable);
			_EndLuaFunction(L, 1);
		
		}
	}

}

void UITabProfileMissionCard_V10::_UpdateChallengeSlotState()
{
	if( IsReadyToUpdate() == false) return;


	// Ȱ��ȭ ī�尡 ���� ��� ��� �ӹ� ��Ȱ��ȭ
	MCardSet* pSelSet = MCardUtil::GetSelectedMCardSet();
	MCard* pSelCard = MCardUtil::GetSelectedMCard();
	INT32  cardSetType = (pSelSet) ? pSelSet->GetMCardSetInfo()->setID : QUEST_CARDSET_TYPE_NONE;

	
	if (MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList) == false || 
		 pSelCard == nullptr || 
		( pSelCard->IsBattleAvailableCard() == false && cardSetType == QUEST_CARDSET_TYPE_TUTORIAL) )
	{
		for( INT32 i = 0; i < MAX_CARDSET_PER_USER; i++)
		{
			SetCCardNotice(i, L"");
			SetCCardComplete(i, L"");
			SetCCardTItle(L"");
		}
		return;
	}
	
	if (pSelSet == nullptr)
	{
		return;
	}

	// Card Title�� �����մϴ�.
	SetCCardTItle(pSelSet->GetMCardSetInfo()->wstrTitle);

	for( INT32 i = 0; i < MAX_QUEST_PER_CARD_NEW; i++)
	{
		MCardQuest* pQuest = pSelCard->GetMCardQuest(i);
		
		if (pQuest->IsActive() == false)
			continue;

		i3::rc_wstring stateText;

		if ( pSelCard->IsQuestCompleted_OffBattle(i) == false )
		{
			stateText = GAME_RCSTRING("STR_TBL_PROFILE_CHALLENGE_TEXT");/*�ӹ�*/ 
		}
		else
		{
			stateText = GAME_RCSTRING("STR_TBL_PROFILE_COMPLETE_TEXT");/*�Ϸ�*/ 	
		}

		i3::rc_wstring wstrStateText = MCardUtil::GetCurrHUDDesc(i);

		SetCCardComplete(i, stateText);
		SetCCardNotice(i, wstrStateText);
	}
}

//
void UITabProfileMissionCard_V10::_UpdateCardSlotImage()
{
	if( IsReadyToUpdate() == false) return;

	INT32 cardSetTypeOfList = QUEST_CARDSET_TYPE_NONE;

	if (m_selectedCardSetIndexFromList != -1)
		cardSetTypeOfList = MCardUtil::GetMCardSetType_Server(m_selectedCardSetIndexFromList);
	
	i3UIControl *cardTemp = nullptr;
	COLOR cardColor;

	//���ø��� �ش� Select�� ī��� ��ü�� �ش�.

	//�ӹ�ī�� �̹��� �ҷ�����//////////////////////////////////////////////////////////////////////
	INT32 cardSetTypeForUI = cardSetTypeOfList;

	if (cardSetTypeForUI == QUEST_CARDSET_TYPE_NONE)
		cardSetTypeForUI = QUEST_CARDSET_TYPE_TUTORIAL;	
	
	const MCardSetInfo* pCardSetInfo = MCardUtil::FindMCardSetInfoBySetType(cardSetTypeForUI);
	I3ASSERT(pCardSetInfo);
	
	i3::stack_string strTemplate;
	i3::utf16_to_mb(pCardSetInfo->wstrUITemplateName, strTemplate);

	for(INT32 i = 0 ; i < MAX_CARD_PER_CARDSET; i++)
	{
		((i3UIControl *)m_CardSlot[i])->SetTemplateByName(strTemplate.c_str());
		m_CardSlot[i]->SetShapeIdx(i);
	}

	if(m_pCardInfo != nullptr)
	{
		((i3UIControl*) m_pCardInfo)->SetTemplateByName(strTemplate.c_str());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////

	INT32 setSlot = MCardUtil::FindCardSetSlotWithCardSetType( cardSetTypeOfList);		// ������ ���������� ����..
	MCardSet* pCurrSet = MCardUtil::GetSelectedMCardSet();
	bool isSelectedSet = (pCurrSet && pCurrSet->GetMyIndex() == setSlot );
	
	for( INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		//g_pFramework->SetCardShape( m_cardSlot[i]._pCard, GetSelectedCardsetID(), i);
		//SetCardImageShape(i, reinterpret_cast<INT32>GetSelectedCardsetID());
		cardTemp = (i3UIControl *)m_CardSlot[i];
		if(  isSelectedSet  )
		{
			MCard* pCard = pCurrSet->GetMCard(i);
			if( pCard->IsActive() )
			{
				if( QUEST_CARDSET_TYPE_TUTORIAL == cardSetTypeOfList )
				{
					// Ʃ�丮�� �ӹ�ī���� ��� ���������� ����/��µȴ�.
					// ù ī��� ������ Ȱ��ȭ�ȴ�.
					// ��ī�尡 �Ϸ�Ǿ�� Ȱ��ȭ �� �� �ִ�.
					if( pCard->IsAllQuestCompleted_OffBattle() )
					{
						cardColor.r = 255;	cardColor.g = 255;	cardColor.b = 255;	cardColor.a = 255;
						cardTemp->setColor(&cardColor);
						//m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 255);
					}
					else if (0 == i ||	pCurrSet->GetMCard(i-1)->IsAllQuestCompleted_OffBattle() ) 
					{
						cardColor.r = 255;
						cardColor.g = 255;
						cardColor.b = 255;
						cardColor.a = 165;
						cardTemp->setColor(&cardColor);
						//m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 165);
					}
					else
					{
						cardColor.r = 255;
						cardColor.g = 255;
						cardColor.b = 255;
						cardColor.a = 51;
						cardTemp->setColor(&cardColor);
						//m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 51);
					}
				}
				else
				{
					// �� �̿� �ӹ�ī�� ��� ���ϴ� �͸��� ����/����� �� �ִ�.
					if(	pCard->IsAllQuestCompleted_OffBattle() ) 
					{
						cardColor.r = 255;	cardColor.g = 255;	cardColor.b = 255;	cardColor.a = 255;
						cardTemp->setColor(&cardColor);
					}
					else
					{
						cardColor.r = 255;	cardColor.g = 255;	cardColor.b = 255;	cardColor.a = 165;
						cardTemp->setColor(&cardColor);
					}
				}
			}
			else
			{
				cardColor.r = 255;	cardColor.g = 255;	cardColor.b = 255;	cardColor.a = 51;
				cardTemp->setColor(&cardColor);
			}
		}
		else
		{
			cardColor.r = 255;	cardColor.g = 255;	cardColor.b = 255;	cardColor.a = 51;
			cardTemp->setColor(&cardColor);
		}

	}
}

//

void UITabProfileMissionCard_V10::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	
	switch( event)
	{
	case EVENT_QUEST_BUY_CARD_SET :
		if (EV_SUCCESSED(arg))
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_BUY_CARD"));/*ī���ѹ� ���Կ� �����Ͽ����ϴ�.*/ 
	
			InitCardList_Lua();
			//SetMissionPossibleCount_Lua();
			
			UITabProfileMissionCard_V10::UpdateCardSetListSelection();
			UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI();
		}
		else
		{
			if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL))	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_BASE_FAIL_BUY_CARD_BY_NO_CARD_INFO"));/*������ ī�� ������ ã������ ���ſ� �����Ͽ����ϴ�.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_PRICE_FAIL))	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_BASE_NO_POINT_TO_GET_ITEM"));/*�ܾ��� �����Ͽ� �������� ������ �� �����ϴ�.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL))	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_BASE_LIMIT_CARD_COUNT"));/*������ �� �ִ� �ӹ�ī���� ������\n�� 3������ ���ѵǾ� �ֽ��ϴ�.\n \n���� �����Ͻ� �ӹ��� ��� �ϼ��ϰų� ���� �Ͻ� �� �ٽ� �������ֽñ� �ٶ��ϴ�.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_DISQUALIFIED_FAIL))	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_BASE_DID_NOT_TUTORIAL_MISSION_CARD"));

			else I3ASSERT_0;
		}
		break;
	case EVENT_QUEST_DELETE_CARD_SET :
		if (EV_SUCCESSED(arg))
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_DELETE_CARD"));/*ī���ѹ��� �����Ͽ����ϴ�.*/

			InitCardList_Lua();
			////SetMissionPossibleCount_Lua();

			//if ( m_nextSetTypeForDelete != QUEST_CARDSET_TYPE_NONE)
			//{
			//	MCardUtil::SelectActiveCardSetToServerWithSetType(m_nextSetTypeForDelete);
			//}
			//UITabProfileMissionCard_V10::UpdateCardSetListSelection();		// ���� ī��� ��� ������ ����..
			UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI();		// ������ ����ī�� ���� ���� ����..
			
		}
		else
		{
			if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_DELETE_CARD_PARAMETER_FAIL))		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_DELETE_CARD_BY_NOT_INFO"));/*������ ī�� ������ ã������ ������ �����Ͽ����ϴ�.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_DELETE_CARD_UNKOWN_FAIL))	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_DELETE_ITEM_BY_NETWORK_PROBLEM"));/*��Ʈ��ũ ������ �������� ������ �����Ͽ����ϴ�.*/
			else I3ASSERT_0;
		}
		break;
	}
}




void UITabProfileMissionCard_V10::CardInfo()
{
	UIMainFrame *pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT(pFrame);
	// ���õ� ī���� �ε���

	INT32 cardSetType = MCardUtil::GetMCardSetType_Server( m_selectedCardSetIndexFromList );

	pFrame->TogglePopup(UPW_MISSIONINFO, ((void*)&cardSetType));		// �˾� ������ ���߿�(2015.06.22.����)

}

namespace
{

	bool CheckBuyQuestCard(INT32 cardSetType)
	{
		bool Rv = true; 
	//
		INT32 MyRank = UserInfoContext::i()->GetMyRank();
		
		switch(cardSetType)
		{
		case QUEST_CARDSET_TYPE_COMMISSIONED_O	: 
			if( MyRank < 5)//�ϻ� �̻�
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_QEUST_ERROR_COMMISSIONED"));
				Rv = false; 
			}
			break; 
		case QUEST_CARDSET_TYPE_COMPANY_O		: 
			if( MyRank < 17)//���� �̻�
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_QEUST_ERROR_COMPANY"));
				Rv = false; 
			}
			break; 
		case QUEST_CARDSET_TYPE_FIELD_O			: 
			if( MyRank < 31)//�ҷ� �̻�
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_QEUST_ERROR_FIELD"));
				Rv = false; 
			}
			break; 		
		}

		return Rv; 
	}

}

void UITabProfileMissionCard_V10::CardBuy()
{
	UIMainFrame *pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT(pFrame);
	// ���õ� ī���� �ε���
	INT32 cardSetType = MCardUtil::GetMCardSetType_Server(m_selectedCardSetIndexFromList);

	// GetSelectedCardSetType();

	if(!CheckBuyQuestCard(cardSetType))
		return; 
	
	if( MCardUtil::HasMCardSet(cardSetType) )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_PROFILE_CARDSET_DUPLICATE"));
		return;
	}

	pFrame->TogglePopup(UPW_MISSIONSHOP, ((void*)&cardSetType));	// �˾� ������ ���߿�(2015.06.22.����)
}

namespace
{
	void CbDeleteCardSet(void* pThis,INT32 nParam)
	{
		if (MBR_OK == nParam)
		{
			// ������ �ִ� ī��¸� ������ �� �ִ�.
			// ���� Ȱ��ȭ �Ǿ��ִٸ�, ������ �ִ� ī����̴�.
			MCardSet* pCardSet = MCardUtil::GetSelectedMCardSet();

			if (pCardSet != nullptr)		
			{
				UINT8 cardSetSlot = (UINT8)pCardSet->GetMyIndex();
				GameEventSender::i()->SetEvent(EVENT_QUEST_DELETE_CARD_SET, &cardSetSlot);
			}
		}
	}
	
}


void UITabProfileMissionCard_V10::CardDelete()
{
	// ������ �ִ� ī��¸� ������ �����ϴ�.
	// Ȱ��ȭ�Ǿ��ִٸ�, ������ �ִ� ī����̴�.
	if (MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList))
	{
		MCardSet* pNextSet = MCardUtil::FindNextAvailableCardSet();

		m_nextSetTypeForDelete = (pNextSet) ? pNextSet->GetMCardSetInfo()->setID : QUEST_CARDSET_TYPE_NONE;

		GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_PROFILE_DELETE_CARD_MESSAGE"), nullptr, MAKE_CALLBACK(CbDeleteCardSet), this);
		/*�����Ͻ� ������ ��� �ӹ�ī���\n�����Ǹ�, ������ �� �����ϴ�.\n \n�׷��� �����Ͻðڽ��ϱ�?*/
	}
}

//
void UITabProfileMissionCard_V10::SetCCardNotice(INT32 idx, const i3::rc_wstring& wstrText)
{
	LuaState * L = _CallLuaFunction("SetCCardNotice");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushStringUTF16To8(L, wstrText);
	_EndLuaFunction(L, 2);
}

void UITabProfileMissionCard_V10::SetCCardComplete( INT32 idx, const i3::rc_wstring& wstrText )
{
	LuaState * L = _CallLuaFunction("SetCCardComplete");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushStringUTF16To8(L, wstrText);
	_EndLuaFunction(L, 2);
}

void UITabProfileMissionCard_V10::OnCreate( i3GameNode * pParent )
{
	UITabProfileBase_V10::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_MyInfomation_MissionCard.i3UIs", UITabProfileMissionCard_V10_Glue);
}

void UITabProfileMissionCard_V10::UpdateCardSetListSelection()
{
	const MCardSet* pCardSet = MCardUtil::GetSelectedMCardSet();

	m_selectedCardSetIndexFromList = -1;				// ī���Ÿ���� �ƴϰ�, �����κ��� ���� ��� �ε����̴�..
	INT32 cardSetType = QUEST_CARDSET_TYPE_NONE;

	if (pCardSet != nullptr)
	{
		cardSetType = pCardSet->GetMCardSetInfo()->setID;
		m_selectedCardSetIndexFromList =	MCardUtil::GetServerIndexWithMCardSetType(cardSetType);
	}
	else	// �������� ������ �׳� ��Ͽ��� ���� ù��° ���� ���� �����Ѵ�..
	{
		MCardSet* pNextSet = MCardUtil::FindNextAvailableCardSet();

		if (pNextSet)
		{
			MCardUtil::SelectActiveCardSetToServer(pNextSet->GetMyIndex());
			cardSetType = pNextSet->GetMCardSetInfo()->setID;
			m_selectedCardSetIndexFromList =	MCardUtil::GetServerIndexWithMCardSetType(cardSetType);
		}
	}

	if(cardSetType != QUEST_CARDSET_TYPE_NONE)				//�ʱ� ���Խ� ������ Ȱ��ȭ�� CardSet�� CardIndex�� ������
	{
		LuaState * L = _CallLuaFunction("SetCardListSelection");
		i3Lua::PushInteger(L, m_selectedCardSetIndexFromList);
		_EndLuaFunction(L,1);
	}
}

void UITabProfileMissionCard_V10::SetCCardTItle( const i3::rc_wstring& wstrText )
{
	// CardSetTitle �����մϴ�
	LuaState * L = _CallLuaFunction("SetMissionCardTitle");
	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrText);
		_EndLuaFunction(L, 1);
	}
}

void UITabProfileMissionCard_V10::_SetMissionBoard()
{
	UIFloatMissionBoard_V10 * missionboard = (UIFloatMissionBoard_V10*)GetMainFrame()->GetFloating(UFW_MISSIONBOARD);
	if( missionboard != nullptr)
	{
		missionboard->SetMissionCardSlot();
	}
}

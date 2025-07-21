#include "pch.h"
#include "BattleGui.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "AwayChecker.h"
//#include "StageGui/Popup/PopupOption.h"
#include "GuiNotifyReceiver.h"
#include "BattleGUI_BOSS_Result.h"
#include "Shop.h"

#include "UI/UIUtil.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIBattlePopupBase.h"
#include "UI/UIHUDIngameChat.h"
#include "ui/UIItemInfoUtil.h"

#include "LogRespawn.h"
#include "GameExitPopup.h"
#include "GameStateMgr.h"
#include "BattleHud.h"

I3_CLASS_INSTANCE(CBattleGui); //, i3GameObjBase);

//#define		NO_BOSS_RESULT_GUI
//#define		NO_BOSS_SCORE_GUI

CBattleGui::CBattleGui(void)
{	
	INT32 i = 0;

	m_bEnableSelectWep = FALSE;
	m_bEnableRespawn = FALSE;
	m_bEnableResult = FALSE;
	m_bEnableTrainingResult = FALSE;
	m_bAutoRespawn = FALSE;

	m_pBattleGUIRoot = NULL;

	m_fRespawnTime = 0.0f;
	m_fTimeBarScale = 0.0f;

	m_pStaticRespawnBG = NULL;
	//m_pStaticRespawnWepSelectBG = NULL;
	m_pStaticSeconds = NULL;
	m_pStaticText = NULL;
	m_pEditNotice = NULL;
	m_pStaticTimeBar = NULL;
	m_pStaticTimeBarBG = NULL;
	m_pStaticTimeItem = NULL;
	m_pEditTimeItemText = NULL;

	m_pConfirm = NULL;
	m_pCancel = NULL;
	m_pLimitKey = NULL;
	
	for(UINT32 j = 0; j < MAX_CATEGORY_WEAPON; j++)
	{
		m_weaponSlot[j].Reset();
	}

	// Training
	m_pStaticTrainingBG = NULL;
	m_pButtonTrainingConfirm = NULL;
	m_pButtonTrainingUpdateScore = NULL;
	for( i=0; i<5; i++) m_pStaticTrainingNumber[i] = NULL;
	for( i=0; i<GUI_TRAINING_TEXT_COUNT; i++) m_pEditBoxTraining[i] = NULL;

	// Exit Popup
	m_pStaticExitPopupBG = NULL;
	m_pEditBoxExitPopupText = NULL;
	for( i=0; i<GUI_ID_EXITPOPUP_BUTTON_COUNT; i++) m_pButtonExitPopup[i] = NULL;

	m_idxPrimaryWeapon = -1;
	m_idxSecondaryWeapon = -1;
	m_idxMeleeWeapon = -1;
	m_idxThrowWeapon = -1;
	m_idxWeaponItem = -1;

#if defined	USE_AWAY_INPUT
	m_pAwayChecker = NULL;
	m_pAwayMessageBox = NULL;
#endif

//	m_pInGamePopup = NULL;

	m_rWheelDelta = 0.0f;

	m_pPanel_BOSS_Score = NULL;
	m_pPanel_BOSS_PreStart = NULL;
	m_pPanel_Stage_PreStart = NULL;
	m_pPanelResult = NULL;

	memset( &m_weaponSlot, 0, sizeof( WeaponSelectSlot ) * MAX_CATEGORY_WEAPON );
}

CBattleGui::~CBattleGui(void)
{
#if defined	USE_AWAY_INPUT
	if (m_pAwayMessageBox)
	{
		GameUI::CloseMsgBox( m_pAwayMessageBox);
		m_pAwayMessageBox = NULL;
	}

	if (m_pAwayChecker)
	{
		RemoveChild(m_pAwayChecker);
		I3_SAFE_RELEASE(m_pAwayChecker);
	}
#endif

	if( m_pBattleGUIRoot != NULL)
	{
		m_pBattleGUIRoot->getSceneGraphNode()->RemoveFromParent();

		I3_SAFE_RELEASE( m_pBattleGUIRoot);
	}

	m_pBattleGUIRoot = NULL;
	g_pGUIRoot = NULL;

	//I3_SAFE_RELEASE( m_pInGamePopup);

	I3_SAFE_RELEASE( m_pPanel_BOSS_Score);
	I3_SAFE_RELEASE( m_pPanel_BOSS_PreStart);
	I3_SAFE_RELEASE( m_pPanelResult);
}

BOOL CBattleGui::Create()
{
	LoadGUIFile( "Gui/Project/Respawn_Result.gui");	

	//m_pBattleGUIRoot->ADDControlByGUI("Gui/Project/Popup_Option.gui");
	
	((i3GuiWindow*)(m_pBattleGUIRoot->getFirstChild()))->setState(I3GUI_WINDOW_STATE_ALLWAYS_FOCUS);

	/*m_pInGamePopup = CPopupOption::new_object();
	m_pInGamePopup->SetEnable( FALSE);
	m_pInGamePopup->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pInGamePopup);*/

//	if( g_pGameContext->IsBossMissionMode() == FALSE)
//	{
		m_pPanelResult = CBattleGUI_Result::new_object();
//	}
//	else
//	{
///		#if !defined( NO_BOSS_RESULT_GUI)
//			m_pPanelResult = CBattleGUI_BOSS_Result::new_object();
//		#else
//			m_pPanelResult = CBattleGUI_Result::new_object();
//		#endif
//	}

	_LinkControl();	

	InitGui();
	
	SetEnableGui( FALSE);

	// ���� �⺻ ���� ���� - �κ��丮���� ����� ���⸦ �������� ��´�
	NET_CHARA_INFO * pCharaInfo = (NET_CHARA_INFO*)g_pGameContext->GetMyCharaInfo();
	m_idxPrimaryWeapon = CInvenList::instance()->GetWeaponIndex( pCharaInfo->getWeaponItemID( WEAPON_SLOT_PRIMARY));
	m_idxSecondaryWeapon = CInvenList::instance()->GetWeaponIndex( pCharaInfo->getWeaponItemID( WEAPON_SLOT_SECONDARY));
	m_idxMeleeWeapon = CInvenList::instance()->GetWeaponIndex( pCharaInfo->getWeaponItemID( WEAPON_SLOT_MELEE));
	m_idxThrowWeapon = CInvenList::instance()->GetWeaponIndex( pCharaInfo->getWeaponItemID( WEAPON_SLOT_THROWING1));
	m_idxWeaponItem = CInvenList::instance()->GetWeaponIndex( pCharaInfo->getWeaponItemID( WEAPON_SLOT_THROWING2));

#if defined	USE_AWAY_INPUT
	// AwayChecker ����
	m_pAwayChecker = CAwayChecker::new_object();
	m_pAwayChecker->SetReceiver(this);
	m_pAwayChecker->Reset();
	I3_GAMENODE_ADDCHILD(this, m_pAwayChecker);
#endif

	if( g_pGameContext->IsBossMissionMode())
	{
		#if !defined( NO_BOSS_SCORE_GUI)
			m_pPanel_BOSS_Score = CBattleGUI_BOSS_Score::new_object();
			m_pPanel_BOSS_Score->Create( this);
		#endif

		m_pPanel_BOSS_PreStart = CBattleGUI_BOSS_PreStart::new_object();
		m_pPanel_BOSS_PreStart->Create( this);
		m_pPanel_BOSS_PreStart->SetEnable( FALSE);

		I3_GAMENODE_ADDCHILD( this, m_pPanel_BOSS_PreStart);
	}

	if( g_pGameContext->IsHeadHunterRoom() )
	{
		m_pPanel_Stage_PreStart = CBattleGUI_Stage_PreStart::new_object();
		m_pPanel_Stage_PreStart->Create( this);
		m_pPanel_Stage_PreStart->SetEnable( FALSE);

		I3_GAMENODE_ADDCHILD( this, m_pPanel_Stage_PreStart);
	}

	return TRUE;
}

static INT32 nfdafd = 0;

BOOL CBattleGui::LoadGUIFile(const char * szGUIFileName)
{
	i3ResourceFile File;

	if( File.Load( szGUIFileName) == STREAM_ERR)
	{
		I3FATAL( "CBattleGui::LoadGUIFile()");
		return FALSE;
	}

	// GuiRoot�� ã�� �ʱ�ȭ�Ѵ�
	m_pBattleGUIRoot = (i3GuiRoot*)File.FindObjectByMeta(i3GuiRoot::static_meta());
	if( m_pBattleGUIRoot == NULL) return FALSE;

	I3ASSERT(m_pBattleGUIRoot != NULL);

	const i3TextFontSetInfo * pFontTextInfo = GetDefaultFontName();
	m_pBattleGUIRoot->setTextNodeUsage(I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX);	
	m_pBattleGUIRoot->Create( g_pFramework->getGuiLayer(), pFontTextInfo->GetFontName(), pFontTextInfo->GetCharset());
	m_pBattleGUIRoot->SetViewer(g_pViewer, FALSE);
	m_pBattleGUIRoot->setWidth( g_pViewer->GetViewWidth());
	m_pBattleGUIRoot->setHeight( g_pViewer->GetViewHeight());
	m_pBattleGUIRoot->setManualNotifyDispatch( true);

	_Rec_BuildScene((i3GuiObjBase*)(m_pBattleGUIRoot->getFirstChild()));

	m_pBattleGUIRoot->SetEnable(FALSE);		// FadeIn ���������� ��Ȱ��ȭ

	m_pBattleGUIRoot->OnUpdate( __RT_0);

	m_pBattleGUIRoot->AddRef();

	g_pGUIRoot = m_pBattleGUIRoot;

	return TRUE;
}

void CBattleGui::_Rec_BuildScene( i3GuiObjBase * pObj)
{
	if( pObj == NULL)	return;

	i3GameNode	* pParent		= pObj->getParent();
	i3Node		* pParentNode	= NULL;

	pParentNode = ((i3GuiObjBase *)pParent)->GetNode();

	// GUI�� EditBox Control�� EditBox�� ����� �޴� ��� Text�� �����ؾ� �մϴ�.
	if( i3::same_of<i3GuiEditBox*>(pObj) //->IsExactTypeOf( i3GuiEditBox::static_meta()) 
		|| i3::same_of<i3GuiTextBox*>(pObj)) //->IsExactTypeOf( i3GuiTextBox::static_meta()) )
	{
		if (0 == i3String::Compare(((i3NamedElement*)pObj)->GetName(), "Respawn_Notice"))
		{
			// ������ GUI�� �ػ󵵿� ���� Ȯ����� �ʴ´�.
			// �ʱ�ȭ �� ReCreateTextEx()�� ����ϸ� Ȯ�������, �̰����� �̸� �����Ѵ�.
			((i3GuiEditBox*)pObj)->CreateTextEx( GetDefaultFontName(), FONT_COUNT_DEFAULT, GAME_FONT_SIZE_HUD_RESPAWN_TEXT, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_HIGH);

			((i3GuiEditBox*)pObj)->SetTextSpace(g_pConfigEx->GetGUI().DefaultTextSpaceX, g_pConfigEx->GetGUI().DefaultTextSpaceY);
			((i3GuiEditBox*)pObj)->SetOffsetPos(g_pConfigEx->GetGUI().DefaultTextOffsetX, g_pConfigEx->GetGUI().DefaultTextOffsetY);
		}
		else
		{
			((i3GuiEditBox*)pObj)->CreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_DEFAULT_SIZE);

			((i3GuiEditBox*)pObj)->SetTextSpace(g_pConfigEx->GetGUI().DefaultTextSpaceX, g_pConfigEx->GetGUI().DefaultTextSpaceY);
			((i3GuiEditBox*)pObj)->SetOffsetPos(g_pConfigEx->GetGUI().DefaultTextOffsetX, g_pConfigEx->GetGUI().DefaultTextOffsetY);
		}			
	}

	//Add Child
	if( pObj->GetNode() != NULL)
	{
		pParentNode->AddChild( pObj->GetNode());
	}

	//Child
	i3GuiObjBase * pChild = ( i3GuiObjBase *)pObj->getFirstChild();
	if( pChild != NULL)
	{
		_Rec_BuildScene( pChild);
	}

	//Next
	_Rec_BuildScene( (i3GuiObjBase *)pObj->getNext());
}

void CBattleGui::InitGui()
{
 	INT32 i = 0;	

	m_bEnableSelectWep = FALSE;
	m_bEnableRespawn = FALSE;
	m_bEnableResult = TRUE;
	m_bEnableTrainingResult = FALSE;
	m_bAutoRespawn = FALSE;

	// Respawn
	m_pStaticSeconds->SetAllShapeEnable( FALSE);
	m_pStaticText->SetAllShapeEnable( FALSE);

	m_pStaticRespawnBG->SetEnable( FALSE);
	//m_pStaticRespawnBG->SetPosToCenter();
	m_pEditNotice->SetEnable( FALSE);
	m_pStaticTimeBar->SetEnable( FALSE);
	m_pStaticTimeBarBG->SetEnable( FALSE);
	m_pStaticTimeItem->SetEnable( FALSE);

	// ������ ���� ��
	m_pEditTimeItemText->ReCreateTextEx(GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_BIG_SIZE, FALSE, NULL, FW_NORMAL, TRUE);
	m_pEditTimeItemText->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pEditTimeItemText->SetTextColor( GameGUI::GetColor(GCT_YELLOW));
	m_pEditTimeItemText->SetEnable(FALSE);


	REAL32 rateX = 1.0f;
	if (i3GuiRoot::isChangeResolution())
	{
		rateX = i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f;
	}
	
	REAL32 rRespawnLimit = g_pGameContext->GetRespawnTime( g_pGameContext->getMySlotIdx());

	m_fTimeBarScale = (m_pStaticTimeBar->getWidth() * rateX) / rRespawnLimit;
	m_pStaticTimeBar->GetControlData()->m_pShape->getImage(0)->m_nWidth = 0;

	// �ػ� ������ ���ٸ�, Viewer�� �߽ɿ� �°� GUI�� �̵��մϴ�.
	{
		i3GuiRoot * pGUIRoot = getGUIRoot();

		if( !pGUIRoot->isChangeResolution())
		{
			// �����츦 �߾����� �̵�
			i3GuiWindow* pBgWin= (i3GuiWindow*)pGUIRoot->getFirstChild();
			POINT2D ptBgWin= g_pFramework->GetCenterPosition((INT32)(pBgWin->getWidth() / (g_pViewer->GetViewWidth() / I3GUI_RESOLUTION_WIDTH)) , (INT32)(pBgWin->getHeight() / (g_pViewer->GetViewHeight() / I3GUI_RESOLUTION_HEIGHT)));
			
			pBgWin->setPositionX((REAL32)(ptBgWin.x));
			pBgWin->setPositionY((REAL32)(ptBgWin.y));
		}
	}
		
	// ���ⱳüâ �˸�
	{
		char notice[MAX_STRING_COUNT];

		GAME_KEY_STRUCT* pKeyMap = g_pEnvSet->getGameKeyMap();		

		CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( g_pGameContext->getMySlotIdx());

		REAL32 InvTime = g_pGameContext->GetInvincibleTime();

		if( pPlayer != NULL )
			InvTime = pPlayer->GetInvincibleLimitTime();

		sprintf_s(notice, GAME_STRING("STR_TBL_BATTLEGUI_NOTICE_RESPAWN"), //
						(INT32)InvTime, GameUI::GetKeyboardString( &pKeyMap[GAME_KEY_MAP_CHANGEWEAPON]) );

#if defined(GUI_ADJUST_RUSSIA)
		m_pEditNotice->ReCreateTextEx(GetDefaultFontName(), FONT_COUNT_512, 13);
#endif
		m_pEditNotice->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pEditNotice->SetTextSpace(0, 3);
		
		I3COLOR color;
		i3Color::Set(&color, (UINT8) 204, 204, 204, 255);
		m_pEditNotice->SetTextColor(&color);
		m_pEditNotice->SetText(notice);

		//m_pStaticRespawnWepSelectBG->SetPosToCenter();
	}

	// Result
	if( m_pPanelResult != NULL)
		m_pPanelResult->Init();

	// ���� �̼ǿ� GUI
	if( m_pPanel_BOSS_PreStart != NULL)
		m_pPanel_BOSS_PreStart->Init();

	// Stage GUI
	if( m_pPanel_Stage_PreStart != NULL)
		m_pPanel_Stage_PreStart->Init();

	// Training 
	{
		m_pButtonTrainingConfirm->setClickAction( TRUE);
		m_pButtonTrainingConfirm->setInputMode(I3GUI_INPUT_MODE_STROKE);
		m_pButtonTrainingConfirm->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		m_pButtonTrainingConfirm->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
		m_pButtonTrainingConfirm->SetUserDefine(I3GUI_CONTROL_STATE_CLICKED, GUI_SHAPE_OFFSET + 2);

		m_pButtonTrainingUpdateScore->setClickAction( TRUE);
		m_pButtonTrainingUpdateScore->setInputMode(I3GUI_INPUT_MODE_STROKE);
		m_pButtonTrainingUpdateScore->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		m_pButtonTrainingUpdateScore->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
		m_pButtonTrainingUpdateScore->SetUserDefine(I3GUI_CONTROL_STATE_CLICKED, GUI_SHAPE_OFFSET + 2);

		m_pStaticTrainingNumber[0]->SetAllShapeEnable( FALSE);
		m_pStaticTrainingNumber[1]->SetAllShapeEnable( FALSE);
		m_pStaticTrainingNumber[2]->SetAllShapeEnable( FALSE);
		m_pStaticTrainingNumber[3]->SetAllShapeEnable( FALSE);
		m_pStaticTrainingNumber[4]->SetAllShapeEnable( FALSE);

		for( i=0; i<GUI_TRAINING_TEXT_COUNT; i++) m_pEditBoxTraining[i]->SetTextAlign( 1, 1);
	}

	// Exit PopUp
	{
		m_pEditBoxExitPopupText->SetTextAlign( 1, 1);
		m_pEditBoxExitPopupText->SetText(GAME_STRING("STR_TBL_BATTLEGUI_SET_TEXT"));

		INIT_BUTTON_EX( m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_OPTION]);
		INIT_BUTTON_EX( m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_REPORT]);
		INIT_BUTTON_EX( m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_FORCEREMOVE]);
		INIT_BUTTON_EX( m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_EXIT]);
		INIT_BUTTON_EX( m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_CANCEL]);

		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_OPTION]->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_OPTION]->SetCaption(GAME_STRING("STR_TBL_BATTLEGUI_OPTION_TEXT"));
		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_REPORT]->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_REPORT]->SetCaption(GAME_STRING("STR_TBL_BATTLEGUI_REPORT"));
		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_FORCEREMOVE]->CreateCaption( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE);
		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_FORCEREMOVE]->SetCaption(GAME_STRING("STR_EXIT_POPUP_FORCIBLYREMOVE"));
		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_EXIT]->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_EXIT]->SetCaption(GAME_STRING("STR_TBL_BATTLEGUI_EXIT_TEXT"));
		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_CANCEL]->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_CANCEL]->SetCaption(GAME_STRING("STR_TBL_BATTLEGUI_CANCEL_TEXT"));

		// �ʱ⿡�� ��Ȱ��ȭ �����Դϴ�.
		if (m_pStaticExitPopupBG)
			m_pStaticExitPopupBG->SetEnable( FALSE);
		//m_pStaticExitPopupBG->SetPosToCenter();

		// issue : BUG_PORTABLE #8627
		// Ʃ�丮�� ��忡�� �������� ��ư ����. 2011-09-21. ugeun.ji
		{
			BOOL forciblyRemoveMode = LocaleValue::Enable("ForciblyRemove");
			BOOL tutorialMode		= static_cast<BOOL>(g_pGameContext->IsTutorialMode());

			if ( forciblyRemoveMode && !tutorialMode )
			{
				m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_OPTION]->setPosition( 65.0f, 66.0f);
				m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_FORCEREMOVE]->setPosition( 65.0f, 98.0f);
				m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_EXIT]->setPosition( 65.0f, 130.0f);
			}
			else
			{
				m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_OPTION]->setPosition( 65.0f, 74.0f);
				m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_EXIT]->setPosition( 65.0f, 122.0f);
				m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_FORCEREMOVE]->SetEnable( FALSE);
			}

			m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_REPORT]->SetEnable( FALSE);
		}
	}

	// Option�� �ʱ�ȭ �մϴ�.
	//m_pInGamePopup->OnInGame();
	//m_pInGamePopup->OnInitControl();

	// ���� ����
	_InitWeaponSelect();
}

void CBattleGui::SetEnablePopup(void)
{
	////////////////////////////////////////////////////////////////////////
	// ������ ���� Popup�� Ȱ��ȭ/��Ȱ��ȭ �մϴ�.
	////////////////////////////////////////////////////////////////////////
	
	if( m_pBattleGUIRoot == NULL)		// ������ ũ���ð� �� ���ɼ��� �ִٸ� �̸� üũ�� ���� �ٷ� ����..(11.11.07.����)
		return;

	BOOL bEnable = !GetEnablePopup();		//  �� �Լ��� ��ü�Ѵ�...(11.11.22.����) //(m_pStaticExitPopupBG->isEnable());

	if( bEnable)
	{
		g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);

		UIBattleFrame::i()->OpenPopup( UBP_EXIT);
	}
	else
	{
		if(false == ((UIHUDIngameChat *)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING))->GetEnableChat())
		{	
			g_pFramework->SetGameKeyMode( GAMEKEY_MODE_GAME);
		}

		UIBattleFrame::i()->ClosePopup( UBP_EXIT);
	}

	_UpdateRootEnableState();
}

void CBattleGui::_UpdateRootEnableState(void)
{
	BOOL bEnable;
	
	bEnable = m_pStaticRespawnBG->isEnable();// || m_pStaticExitPopupBG->isEnable();
	bEnable |= UIBattleFrame::i()->IsOpenPopup( UBP_EXIT)?TRUE:FALSE;

	if( m_pPanelResult != NULL)
		bEnable = bEnable || m_pPanelResult->isEnable();

	if( m_pPanel_BOSS_Score != NULL)
		bEnable = bEnable || (m_pPanel_BOSS_Score->isVisible() == true);

	if( m_pPanel_BOSS_PreStart != NULL)
	{
		bEnable = bEnable || (m_pPanel_BOSS_PreStart->isVisible() == true);
	}

	if( m_pPanel_Stage_PreStart != NULL)
	{
		bEnable = bEnable || (m_pPanel_Stage_PreStart->isVisible() == true);
	}

	m_pBattleGUIRoot->SetEnable( bEnable);
}

BOOL CBattleGui::GetEnablePopup(void)
{
	UIBattleFrame* frame = UIBattleFrame::i();		// UI�˾��� �����ϴ� ��� ��츦 ó���ؾ���.(11.11.22.����)
	if (frame != NULL)
	{
		if( frame->IsPopupFocused() ) return TRUE;
		return frame->IsOpenPopup( UBP_EXIT);
	}

	return FALSE;
	//return (m_pStaticExitPopupBG->isEnable());
}
 
void CBattleGui::SetEnableGui( BOOL bFlag, GAME_GUI_STATUS nState, BOOL bDeath)
{
	if( m_pBattleGUIRoot == NULL)		// ������ ũ���ð� �� ���ɼ��� �ִٸ� �̸� üũ�� ���� �ٷ� ����..(11.11.07.����)
		return;

	//�ε��� �ȵǾ� �������� ����� �ִ�..

	if( m_pPanelResult != NULL)
	{
		#if defined( NO_BOSS_RESULT_GUI)
			if( g_pGameContext->IsBossMissionMode())
			{
				m_pPanelResult->SetEnableGUI( FALSE, nState, bDeath);
			}
			else
			{
				m_pPanelResult->SetEnableGUI( (bFlag == TRUE), nState, bDeath);
			}
		#else
			m_pPanelResult->SetEnableGUI( (bFlag == TRUE), nState, bDeath);
		#endif
	}

	if( m_pPanel_BOSS_Score != NULL)
		m_pPanel_BOSS_Score->SetEnableGUI( bFlag == TRUE, nState, bDeath);

	if( m_pPanel_BOSS_PreStart != NULL)
		m_pPanel_BOSS_PreStart->SetEnableGUI( bFlag == TRUE, nState, bDeath);

	if( m_pPanel_Stage_PreStart != NULL)
		m_pPanel_Stage_PreStart->SetEnableGUI( bFlag == TRUE, nState, bDeath);

	if( bFlag)
	{					
		m_pBattleGUIRoot->ClearMouseState();

		// Respawn
		switch( nState)
		{
			case GAME_GUI_RESPAWN :
				{
#if defined( DEF_OBSERVER_MODE)
					if( g_pGameContext->isObserverMe())
#else
					if(g_pGameContext->IsGM_Observer())
#endif
						break;

					m_fRespawnTime = 0.0f;

					m_bEnableRespawn = TRUE;
					m_pStaticRespawnBG->SetEnable( TRUE);
					m_pBattleGUIRoot->setCapturedMouseControl(NULL);

					m_bAutoRespawn = FALSE;
					m_bEnableResult = FALSE;
					m_bEnableTrainingResult = FALSE;
					m_pStaticTrainingBG->SetEnable( FALSE);

					m_pBattleGUIRoot->setFocusControl(m_pStaticRespawnBG);

#if defined	USE_AWAY_INPUT
					Check_AwayCheck();
#endif
				}
				break;

			// Result
			case GAME_GUI_RESULT :
				//if(m_pStaticExitPopupBG->isEnable()) 
				if( UIBattleFrame::i()->IsOpenPopup( UBP_EXIT) )
				{
					SetEnablePopup();
					//if( m_pInGamePopup->isEnable()) m_pInGamePopup->OnClosePopup();
				}

				g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);
				m_bEnableResult = TRUE;

				m_bEnableSelectWep = FALSE;
				m_bEnableRespawn = FALSE;
				m_bEnableTrainingResult = FALSE;
				m_pStaticRespawnBG->SetEnable( FALSE);
				m_pStaticTrainingBG->SetEnable( FALSE);
				m_pEditNotice->SetEnable( FALSE);
				m_pStaticTimeBar->SetEnable( FALSE);
				m_pStaticTimeBarBG->SetEnable( FALSE);
				m_pStaticTimeItem->SetEnable( FALSE);
				m_pEditTimeItemText->SetEnable(FALSE);
				break;

			case GAME_GUI_PRESTART_ROUND :
				m_bEnableResult = FALSE;
				m_bEnableSelectWep = FALSE;
				m_bEnableRespawn = FALSE;
				m_bEnableTrainingResult = FALSE;
				m_pStaticRespawnBG->SetEnable( FALSE);
				m_pStaticTrainingBG->SetEnable( FALSE);
				m_pEditNotice->SetEnable( FALSE);
				m_pStaticTimeBar->SetEnable( FALSE);
				m_pStaticTimeBarBG->SetEnable( FALSE);
				m_pStaticTimeItem->SetEnable( FALSE);
				m_pEditTimeItemText->SetEnable(FALSE);
				break;
		}

		CHud::i()->SetRevenge( FALSE);
	}
	else
	{
		UIBattleFrame * pBattleFrame	= UIBattleFrame::i();

		if( pBattleFrame )
		{
			bool bForcebly_Remove_Popup = false;

			UIBattlePopupBase* pPopup_ForciblyRemove =	pBattleFrame->GetPopup(UBP_ACCUSE_CRIME);

			if (pPopup_ForciblyRemove->isEnable() == TRUE)
				bForcebly_Remove_Popup = true;

			if (false == gexit_popup::i()->is_opened() && this->GetEnablePopup() == FALSE && bForcebly_Remove_Popup == false)
			{	
				UIHUDIngameChat * pUIIngameChat = ((UIHUDIngameChat *)pBattleFrame->GetHUD(UIHUD_INGAMECHATING));
				if(false == pUIIngameChat->GetEnableChat())
				{
					if (NULL == m_pParent || FALSE == pUIIngameChat->GetEnableChatScroll())
					{
						g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);
					}
				}
			}
		}
		
		m_bAutoRespawn = FALSE;
		m_bEnableSelectWep = FALSE;
		m_bEnableRespawn = FALSE;

		m_pStaticRespawnBG->SetEnable( FALSE);
		m_pEditNotice->SetEnable( FALSE);
		m_pStaticTimeBar->SetEnable( FALSE);
		m_pStaticTimeBarBG->SetEnable( FALSE);
		m_pStaticTimeItem->SetEnable( FALSE);
		m_pEditTimeItemText->SetEnable(FALSE);

#if defined	USE_AWAY_INPUT
		// ������ ���� �˻� ��
		if (m_pAwayChecker) m_pAwayChecker->Reset();
#endif

		m_bEnableResult = FALSE;
		m_bEnableTrainingResult = FALSE;
		m_pStaticTrainingBG->SetEnable( FALSE);
	}

	_UpdateRootEnableState();
	
	// ���� ��� �� ���� ���� ����
	if( g_pCamera != NULL 
		&& !i3::same_of<CGameCamera_Observer*>(g_pCamera) //->IsExactTypeOf(CGameCamera_Observer::static_meta()) == FALSE
		&& !i3::same_of<CGameCamera_Observer2*>(g_pCamera) //->IsExactTypeOf( CGameCamera_Observer2::static_meta()) == FALSE 
		&& !i3::same_of<CGameCamera_FLY*>(g_pCamera)) //->IsExactTypeOf( CGameCamera_FLY::static_meta()) == FALSE )
	{
		gstatemgr::i()->EnableHUD(!bFlag, (GAME_GUI_STATUS) nState, bDeath);
	}
}

BOOL CBattleGui::GetEnableGui( INT32 nState)
{
	BOOL bRet = FALSE;

	if( nState == GAME_GUI_RESPAWN)
	{
		if( m_bEnableRespawn) bRet = TRUE;
	}
	else if( nState == GAME_GUI_RESULT)
	{
		if( m_bEnableResult) bRet = TRUE;
	}

	return bRet;
}

void CBattleGui::OnUpdate( REAL32 rDeltaSeconds )
{
	// GUI�� ������Ʈ �մϴ�.
	if( m_pBattleGUIRoot != NULL && m_pBattleGUIRoot->isEnable()) 
	{
		m_pBattleGUIRoot->OnUpdate( rDeltaSeconds);

		_ControlNotifyPump();	// OnControlNotify()�� ȣ���Ѵ�
	}

	ProcessSelectHud(rDeltaSeconds);

	// ������ ������ ������Ʈ �մϴ�.
	if( m_bEnableRespawn)
	{
		ProcessRespawn( rDeltaSeconds);
	}

	// ���� ��� ������ ������Ʈ �մϴ�.
	if((m_bEnableResult) && (m_pPanelResult != NULL))
	{
		m_pPanelResult->ProcessResult( rDeltaSeconds );
	}

	// ����� ���â�� ������Ʈ �մϴ�.
	if( m_bEnableTrainingResult)
	{
		ProcessTrainingResult( rDeltaSeconds);
	}

	_ProcessWheel();

	i3GameObjBase::OnUpdate( rDeltaSeconds );
}

void CBattleGui::OnControlNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	CStageBattle * pStage = g_pFramework->GetStageBattle();

	if( pStage == NULL)
		return;

	switch(pNotify->m_nID)
	{
	case GUI_ID_BATTLE_BUTTON_GAMESTART:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnGameStart();
		break;
	/*case GUI_ID_BATTLE_BUTTON_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSelectWeaponCancel();
		break;*/
	case GUI_ID_BATTLE_BUTTON_PREV_1:	// fall through
	case GUI_ID_BATTLE_BUTTON_PREV_2:	// fall through
	case GUI_ID_BATTLE_BUTTON_PREV_3:	// fall through
	case GUI_ID_BATTLE_BUTTON_PREV_4:	// fall through
	case GUI_ID_BATTLE_BUTTON_PREV_5:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnWeaponSelect(pNotify->m_nID - GUI_ID_BATTLE_BUTTON_PREV_1, FALSE);
		break;
	case GUI_ID_BATTLE_BUTTON_NEXT_1:	// fall through
	case GUI_ID_BATTLE_BUTTON_NEXT_2:	// fall through
	case GUI_ID_BATTLE_BUTTON_NEXT_3:	// fall through
	case GUI_ID_BATTLE_BUTTON_NEXT_4:	// fall through
	case GUI_ID_BATTLE_BUTTON_NEXT_5:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnWeaponSelect(pNotify->m_nID - GUI_ID_BATTLE_BUTTON_NEXT_1, TRUE);
		break;
	case GUI_ID_BATTLE_STATIC_PREV_1:	// fall through
	case GUI_ID_BATTLE_STATIC_PREV_2:	// fall through
	case GUI_ID_BATTLE_STATIC_PREV_3:	// fall through
	case GUI_ID_BATTLE_STATIC_PREV_4:	// fall through
	case GUI_ID_BATTLE_STATIC_PREV_5:
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnWeaponFocus(pNotify->m_nID - GUI_ID_BATTLE_STATIC_PREV_1, FALSE);		
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnWeaponSelect(pNotify->m_nID - GUI_ID_BATTLE_STATIC_PREV_1, FALSE);
		break;
	case GUI_ID_BATTLE_STATIC_NEXT_1:	// fall through
	case GUI_ID_BATTLE_STATIC_NEXT_2:	// fall through
	case GUI_ID_BATTLE_STATIC_NEXT_3:	// fall through
	case GUI_ID_BATTLE_STATIC_NEXT_4:	// fall through
	case GUI_ID_BATTLE_STATIC_NEXT_5:
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnWeaponFocus(pNotify->m_nID - GUI_ID_BATTLE_STATIC_NEXT_1, TRUE);		
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnWeaponSelect(pNotify->m_nID - GUI_ID_BATTLE_STATIC_NEXT_1, TRUE);
		break;
	case GUI_ID_BATTLE_BUTTON_RESULT_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnResultConfrim();
		break;

	case GUI_ID_BATTLE_BUTTON_TRAINING_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnTrainingConfrim();
		break;
	case GUI_ID_BATTLE_BUTTON_TRAINING_UPDATE_SCORE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnTrainingUpdateScore();
		break;

	// Exit Popup
	/*case GUI_ID_EXITPOPUP_BUTTON_OPTION:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			SetEnablePopup();
			UIBattleFrame::i()->OpenPopup( UBP_OPTION);
		}
		break;
	case GUI_ID_EXITPOPUP_BUTTON_REPORT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			SetEnablePopup();
		}
		break;
	case GUI_ID_EXITPOPUP_BUTTON_FORCEREMOVE :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			SetEnablePopup();
			UIBattleFrame::i()->TogglePopup( UBP_FORCIBLYREMOVE);
		}
		break;
	case GUI_ID_EXITPOPUP_BUTTON_EXIT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			gexit_popup::i()->open();
			SetEnablePopup();
		}
		break;
	case GUI_ID_EXITPOPUP_BUTTON_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) SetEnablePopup();
		break;*/
	}

	// In Game Option
	//m_pInGamePopup->OnPreGuiNotify();
	//m_pInGamePopup->OnGuiNotify( pNotify);
}

void CBattleGui::OnPreControlNotify(void)
{
	//null
}

void CBattleGui::OnPostControlNotify(void)
{
	//null
}

BOOL CBattleGui::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
	#if defined	USE_AWAY_INPUT
		switch(event)
		{
		case ACE_AWAY:
			{
				SetEnableGui( FALSE);
				i3GuiRoot::setGuiDisableInput(TRUE);
				INT32 nArg = 0;
				g_pGameContext->SetEvent(EVENT_GIVEUP_BATTLE, &nArg);

				// Ȧ��Ī �����Ͽ� ��Ŷ�� �� �� �� �ֱ� ������ �ڽ��� ��������� ��Ŷ�� �ȹ޾Ƶ� ������ ���ư���.
				CGameFramework::JumpToReady();
			}
			return TRUE;
		case ACE_CAUTION:
			m_pAwayMessageBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_BATTLEGUI_NOTICE_LEAVEROOM"));
			return TRUE;
		}
	#endif

		// �Է� �̺�Ʈ ó��
		if (I3_EVT_INPUT == event)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			// ���⼱��â
			/*if (m_bEnableSelectWep)
			{
				if (OnInputForWeaponSelect(pKeyboard))
				{
					return TRUE;
				}
			}*/

			// ���â
			if( (m_bEnableResult) && (m_pPanelResult != NULL))
			{
				if ( m_pPanelResult->OnInputForResult(pKeyboard))
				{
					return TRUE;
				}
			}

			// ����� ���â
			if (m_bEnableTrainingResult)
			{
				if (OnInputForTraining(pKeyboard))
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CBattleGui::OnInputForWeaponSelect(i3InputKeyboard* pKeyboard)
{
	_UpdateSelectWeapon(WEAPON_SLOT_PRIMARY);
	_UpdateSelectWeapon(WEAPON_SLOT_SECONDARY);
	_UpdateSelectWeapon(WEAPON_SLOT_MELEE);
	_UpdateSelectWeapon(WEAPON_SLOT_THROWING1);
	_UpdateSelectWeapon(WEAPON_SLOT_THROWING2);

	
	// Ű���� �Է� ó��
	INT32 idxCurCategory = WEAPON_SLOT_MELEE;

	for(UINT32 i = 0; i < MAX_CATEGORY_WEAPON; i++)
	{
		if (m_weaponSlot[i].m_pSlotEnable->isEnable())
		{
			idxCurCategory = i;
			break;
		}
	}

	// UP/DOWNŰ ó�� - ��񽽷� ī�װ� �̵�
	if (pKeyboard->GetStrokeState(I3I_KEY_UP) || pKeyboard->GetStrokeState(I3I_KEY_DOWN))
	{
		m_weaponSlot[idxCurCategory].m_pSlotEnable->SetEnable(FALSE);
		m_weaponSlot[idxCurCategory].m_pSlotDisable->SetEnable(TRUE);

		if (pKeyboard->GetStrokeState(I3I_KEY_UP))
		{
			idxCurCategory = LOOP_DECREASE(idxCurCategory, WEAPON_SLOT_PRIMARY, WEAPON_SLOT_THROWING2);
		}
		else
		{
			idxCurCategory = LOOP_INCREASE(idxCurCategory, WEAPON_SLOT_PRIMARY, WEAPON_SLOT_THROWING2);
		}
		
		UINT8 WeaponFlag = g_pGameContext->GetMyRoomWeaponFlag();

		while(idxCurCategory < 4 && GlobalFunc::CheckAvailableWeaponUsage(WeaponFlag, (WEAPON_SLOT_TYPE)idxCurCategory) == false)
		{
			if (pKeyboard->GetStrokeState(I3I_KEY_UP))
			{
				idxCurCategory = LOOP_DECREASE(idxCurCategory, WEAPON_SLOT_PRIMARY, WEAPON_SLOT_THROWING2);
			}
			else
			{
				idxCurCategory = LOOP_INCREASE(idxCurCategory, WEAPON_SLOT_PRIMARY, WEAPON_SLOT_THROWING2);
			}				
		}
		
		m_weaponSlot[idxCurCategory].m_pSlotEnable->SetEnable(TRUE);	
		m_weaponSlot[idxCurCategory].m_pSlotDisable->SetEnable(FALSE);
	}

	// LEFTŰ ó�� - ���� ������
	if (pKeyboard->GetStrokeState(I3I_KEY_LEFT))
	{
		OnWeaponSelect(idxCurCategory, FALSE);
	}

	// RIGHTŰ ó�� - ���� ������
	if (pKeyboard->GetStrokeState(I3I_KEY_RIGHT))
	{
		OnWeaponSelect(idxCurCategory, TRUE);
	}
	
	// ENTERŰ ó�� - ����/���ӽ���
	if (m_pConfirm->isEnable() && I3GUI_CONTROL_STATE_DISABLED != m_pConfirm->getControlState())
	{
		if (pKeyboard->GetStrokeState(I3I_KEY_ENTER))
		{
			OnGameStart();
			return TRUE;
		}
	}

	// ESCŰ ó�� - ���
	if (m_pCancel->isEnable() && I3GUI_CONTROL_STATE_DISABLED != m_pCancel->getControlState())
	{
		if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
		{
			OnSelectWeaponCancel();
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CBattleGui::OnInputForTraining(i3InputKeyboard*pKeyboard)
{
	if (m_pButtonTrainingConfirm->isEnable() && I3GUI_CONTROL_STATE_DISABLED != m_pCancel->getControlState())
	{
		if (pKeyboard->GetStrokeState(I3I_KEY_ENTER))
		{
			OnTrainingConfrim();

			return TRUE;
		}
	}

	return FALSE;
}

void CBattleGui::OnResultConfrim(void)
{
	gstatemgr::i()->setEndTime(100.f);
}

void CBattleGui::OnTrainingConfrim(void)
{
	gstatemgr::i()->setEndTime(100.f);
}

void CBattleGui::OnTrainingUpdateScore(void)
{
	gstatemgr::i()->setEndTime(100.f);
}

void CBattleGui::OnGameStart(void)
{
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "inObserverMe : call OnGameStart.....\n");
		return;
	}
#endif

	// Local Mode
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( !g_pConfig->m_bNetworking )
	{
		g_pCharaManager->RespawnChara( g_pGameContext->getMySlotIdx(), true, true );

		SetEnableGui(FALSE, GAME_GUI_RESPAWN);
		return;
	}
#endif

	if (g_pGameContext->IsBombMissionMode() || g_pGameContext->IsAnnihilationMode())
	{
#if defined	USE_AWAY_INPUT
		// ������ ���� �˻� ��
		m_pAwayChecker->Reset();
#endif		
	}
	else if( g_pGameContext->getUserDeath( g_pGameContext->getMySlotIdx()))
	{	// ĳ���Ͱ� ���� ���
		m_bEnableSelectWep = FALSE;

		g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);

		return;
	}	

	SetEnableGui(FALSE, GAME_GUI_RESPAWN);
}

void CBattleGui::OnWeaponSelect(UINT32 nWeaponUsage,BOOL bNext)
{
	UINT8 MyRoomWeaponFlag = g_pGameContext->GetMyRoomWeaponFlag();
	if (bNext)
	{
		if (m_weaponSlot[nWeaponUsage].m_pSlotNext->getControlDisable()) return;

		switch(nWeaponUsage)
		{
		case WEAPON_SLOT_PRIMARY:
			m_idxPrimaryWeapon = _GetNextUsingWeapon(m_idxPrimaryWeapon, WEAPON_SLOT_PRIMARY, MyRoomWeaponFlag);			
			break;
		case WEAPON_SLOT_SECONDARY:
			m_idxSecondaryWeapon = _GetNextUsingWeapon(m_idxSecondaryWeapon, WEAPON_SLOT_SECONDARY, MyRoomWeaponFlag);
			break;
		case WEAPON_SLOT_MELEE:
			m_idxMeleeWeapon = _GetNextUsingWeapon(m_idxMeleeWeapon, WEAPON_SLOT_MELEE, MyRoomWeaponFlag);
			break;
		case WEAPON_SLOT_THROWING1:
			m_idxThrowWeapon = _GetNextUsingWeapon(m_idxThrowWeapon, WEAPON_SLOT_THROWING1, MyRoomWeaponFlag);
			break;
		case WEAPON_SLOT_THROWING2:
			m_idxWeaponItem = _GetNextUsingWeapon(m_idxWeaponItem, WEAPON_SLOT_THROWING2, MyRoomWeaponFlag);
			break;
		}
	}
	else
	{
		if (m_weaponSlot[nWeaponUsage].m_pSlotPrev->getControlDisable()) return;

		switch(nWeaponUsage)
		{
		case WEAPON_SLOT_PRIMARY:
			m_idxPrimaryWeapon = _GetPrevUsingWeapon(m_idxPrimaryWeapon, WEAPON_SLOT_PRIMARY, MyRoomWeaponFlag);			
			break;
		case WEAPON_SLOT_SECONDARY:
			m_idxSecondaryWeapon = _GetPrevUsingWeapon(m_idxSecondaryWeapon, WEAPON_SLOT_SECONDARY, MyRoomWeaponFlag);
			break;
		case WEAPON_SLOT_MELEE:
			m_idxMeleeWeapon = _GetPrevUsingWeapon(m_idxMeleeWeapon, WEAPON_SLOT_MELEE, MyRoomWeaponFlag);
			break;
		case WEAPON_SLOT_THROWING1:
			m_idxThrowWeapon = _GetPrevUsingWeapon(m_idxThrowWeapon, WEAPON_SLOT_THROWING1, MyRoomWeaponFlag);
			break;
		case WEAPON_SLOT_THROWING2:
			m_idxWeaponItem = _GetPrevUsingWeapon(m_idxWeaponItem, WEAPON_SLOT_THROWING2, MyRoomWeaponFlag);
			break;
		}
	}

	for(UINT32 i = 0; i < MAX_CATEGORY_WEAPON; i++)
	{
		m_weaponSlot[i].m_pSlotEnable->SetEnable(FALSE);
		m_weaponSlot[i].m_pSlotDisable->SetEnable(TRUE);
	}

	m_weaponSlot[nWeaponUsage].m_pSlotEnable->SetEnable(TRUE);	
	m_weaponSlot[nWeaponUsage].m_pSlotDisable->SetEnable(FALSE);
}

void CBattleGui::OnWeaponFocus(UINT32 idxSlot,BOOL bNext)
{
	if (bNext)
	{
		if (FALSE == m_weaponSlot[idxSlot].m_pSlotNext->getControlDisable())
		{
			m_weaponSlot[idxSlot].m_pSlotNext->SetShapeEnable(GUI_SHAPE_OFFSET + 0, FALSE);
			m_weaponSlot[idxSlot].m_pSlotNext->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
		}
	}
	else
	{
		if (FALSE == m_weaponSlot[idxSlot].m_pSlotPrev->getControlDisable())
		{
			m_weaponSlot[idxSlot].m_pSlotPrev->SetShapeEnable(GUI_SHAPE_OFFSET + 0, FALSE);
			m_weaponSlot[idxSlot].m_pSlotPrev->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
		}
	}
}

void CBattleGui::OnSelectWeaponCancel(void)
{
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe : call OnSelectWeaponCancel.....\n");
		return;
	}
#endif

	m_bEnableSelectWep = FALSE;

	UIBattleFrame* pFrame = UIBattleFrame::i();
	if( pFrame )
		pFrame->ClosePopup( cc::GetStyle());

	g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);
}


void CBattleGui::Check_AwayCheck()
{
	//����â Ȱ��ȭ�� ���¿��� ������ ���۽�
	UIBattleFrame* pFrame =	UIBattleFrame::i();
	if( pFrame->IsOpenPopup( cc::GetStyle()) )
		m_pAwayChecker->SetTime(AWAY_INPUT_RESPAWN_TIME, AWAY_INPUT_RESPAWN_CAUTION);
}

void	 CBattleGui::ProcessSelectHud(REAL32 rDeltaSeconds)
{
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe : call ProcessRespawn.....\n");
		return;
	}
#endif

	if( ((CStageBattle*)m_pParent)->GetChatEnable() == TRUE ) return;	//ä��â �Է� �޽�����.

	UIBattleFrame* pFrame =	UIBattleFrame::i();

	if (pFrame->IsOpenPopup( UBP_INGAME_SHOP )) return;	//���� ���� Ȱ��ȭ ���¸� ������.

	if (g_pFramework->getKeyStroke() & GAME_KEY_CHANGEWEAPON || 
		g_pFramework->getSysKeyStroke() & GAME_KEY_CHANGEWEAPON )
	{
		//����â�� �ݴ´�.
		if( pFrame->IsOpenPopup( UBP_EXIT) ) pFrame->ClosePopup(UBP_EXIT);

		UIBATTLE_POPUP popup = cc::GetStyle();
		if( popup != UBP_NONE )
		{
			if(pFrame->IsOpenPopup(popup) == false )
			{
				g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);
				m_bEnableSelectWep = TRUE;

				pFrame->OpenPopup( popup );

#if defined	USE_AWAY_INPUT
				if( m_bEnableRespawn)		//������ �߿� ����â Ȱ��ȭ
				{
					m_pAwayChecker->SetTime(AWAY_INPUT_RESPAWN_TIME, AWAY_INPUT_RESPAWN_CAUTION);
				}
#endif
			}
			else
			{
				OnSelectWeaponCancel();

#if defined	USE_AWAY_INPUT
				// ������ ���� �˻� ��
				m_pAwayChecker->Reset();
#endif
			}
		}
	}
}
void CBattleGui::ProcessRespawn( REAL32 rDeltaSeconds )
{
#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe : call ProcessRespawn.....\n");
		return;
	}
#endif

	INT32 mySlotIdx = g_pGameContext->getMySlotIdx();

	REAL32 rRespawnLimit = g_pGameContext->GetRespawnTime( mySlotIdx );
	m_fRespawnTime += rDeltaSeconds;	

	log_respawn::i()->AddRespawnTime(rDeltaSeconds);

	if( m_fRespawnTime > rRespawnLimit) m_fRespawnTime = rRespawnLimit;

	// ������ ���⸦ ���� �ʰ� �ð��� ����ߴٸ� �ٷ� ������ �����մϴ�.
	if( m_fRespawnTime >= rRespawnLimit && m_bEnableSelectWep == FALSE && m_bAutoRespawn == FALSE )
	{
		m_bEnableRespawn = FALSE;
		m_bAutoRespawn = TRUE;
		m_pStaticTimeBar->GetControlData()->m_pShape->getImage(0)->m_nWidth = 0;
		m_pStaticRespawnBG->SetEnable( FALSE);
	}
}

void CBattleGui::ProcessTrainingResult( REAL32 rDeltaSeconds )
{
	char szTemp[256];
	szTemp[0] = 0;

	INT32 nEnemyHitCount = g_pGameContext->getTrainingScore()->_EnemyHitCount;
	INT32 nAlliesHitCount = g_pGameContext->getTrainingScore()->_AlliesHitCount;
	INT32 nTotalHitCount = nEnemyHitCount + nAlliesHitCount;
	INT32 nEnemyPoint = g_pGameContext->getTrainingScore()->_EnemyPoint;
	INT32 nAlliesPoint = g_pGameContext->getTrainingScore()->_AlliesPoint;
	INT32 nTotalPoint = nEnemyPoint - nAlliesPoint;
	
	if( nTotalPoint < 0 ) nTotalPoint = 0;
	else if( nTotalPoint > 99999) nTotalPoint = 0;

	// Nick
	m_pEditBoxTraining[GUI_TRAINING_TEXT_NICK]->SetTextEllipsis(g_pGameContext->getNickForSlot(g_pGameContext->getMySlotIdx()));

	// Enemy Target Name
	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET1]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_ENEMIES_TEXT"));

	// Allies Target Name
	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET2]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_ALLIES_TEXT"));

	// Hero Total Hit Count
	sprintf_s( szTemp, "%d", nTotalHitCount);
	m_pEditBoxTraining[GUI_TRAINING_TEXT_TOTAL_HIT_COUNT]->SetText(szTemp);

	// Enemy Hit Count
	sprintf_s( szTemp, "%d", nEnemyHitCount);
	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET1_HIT]->SetText(szTemp);

	// Enemy Point
	sprintf_s( szTemp, "%d", nEnemyPoint);
	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET1_POINT]->SetText(szTemp);

	// Allies Hit Count
	sprintf_s( szTemp, "%d", nAlliesHitCount);
	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET2_HIT]->SetText(szTemp);

	// Allies Point
	if( nAlliesPoint == 0)	sprintf_s( szTemp, "%d", nAlliesPoint);
	else					sprintf_s( szTemp, "-%d", nAlliesPoint);
	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET2_POINT]->SetText(szTemp);

	// Total Point ǥ��
	{
		i3String::ValueToStr(nTotalPoint, szTemp, sizeof(szTemp));
		INT32 len = i3String::Length((const char*)szTemp);

		m_pStaticTrainingNumber[0]->SetEnable( FALSE);
		m_pStaticTrainingNumber[1]->SetEnable( FALSE);
		m_pStaticTrainingNumber[2]->SetEnable( FALSE);
		m_pStaticTrainingNumber[3]->SetEnable( FALSE);
		m_pStaticTrainingNumber[4]->SetEnable( FALSE);
		m_pStaticTrainingNumber[0]->SetAllShapeEnable( FALSE);
		m_pStaticTrainingNumber[1]->SetAllShapeEnable( FALSE);
		m_pStaticTrainingNumber[2]->SetAllShapeEnable( FALSE);
		m_pStaticTrainingNumber[3]->SetAllShapeEnable( FALSE);
		m_pStaticTrainingNumber[4]->SetAllShapeEnable( FALSE);
		
		switch(len)
		{
		case 1:			
			m_pStaticTrainingNumber[2]->SetEnable( TRUE);
			m_pStaticTrainingNumber[2]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
			break;
		case 2:
			m_pStaticTrainingNumber[2]->SetEnable( TRUE);
			m_pStaticTrainingNumber[1]->SetEnable( TRUE);
			m_pStaticTrainingNumber[2]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
			m_pStaticTrainingNumber[1]->SetShapeEnable( (INT32)szTemp[1]-48, TRUE);
			break;
		case 3:
			m_pStaticTrainingNumber[3]->SetEnable( TRUE);
			m_pStaticTrainingNumber[2]->SetEnable( TRUE);
			m_pStaticTrainingNumber[1]->SetEnable( TRUE);
			m_pStaticTrainingNumber[3]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
			m_pStaticTrainingNumber[2]->SetShapeEnable( (INT32)szTemp[1]-48, TRUE);
			m_pStaticTrainingNumber[1]->SetShapeEnable( (INT32)szTemp[2]-48, TRUE);
			break;
		case 5:
			m_pStaticTrainingNumber[3]->SetEnable( TRUE);
			m_pStaticTrainingNumber[2]->SetEnable( TRUE);
			m_pStaticTrainingNumber[1]->SetEnable( TRUE);
			m_pStaticTrainingNumber[0]->SetEnable( TRUE);
			m_pStaticTrainingNumber[3]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
			m_pStaticTrainingNumber[2]->SetShapeEnable( (INT32)szTemp[2]-48, TRUE);
			m_pStaticTrainingNumber[1]->SetShapeEnable( (INT32)szTemp[3]-48, TRUE);
			m_pStaticTrainingNumber[0]->SetShapeEnable( (INT32)szTemp[4]-48, TRUE);
			break;
		case 6:
			m_pStaticTrainingNumber[4]->SetEnable( TRUE);
			m_pStaticTrainingNumber[3]->SetEnable( TRUE);
			m_pStaticTrainingNumber[2]->SetEnable( TRUE);
			m_pStaticTrainingNumber[1]->SetEnable( TRUE);
			m_pStaticTrainingNumber[0]->SetEnable( TRUE);
			m_pStaticTrainingNumber[4]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
			m_pStaticTrainingNumber[3]->SetShapeEnable( (INT32)szTemp[1]-48, TRUE);
			m_pStaticTrainingNumber[2]->SetShapeEnable( (INT32)szTemp[3]-48, TRUE);
			m_pStaticTrainingNumber[1]->SetShapeEnable( (INT32)szTemp[4]-48, TRUE);
			m_pStaticTrainingNumber[0]->SetShapeEnable( (INT32)szTemp[5]-48, TRUE);
			break;
		default:
			break;
		}
	}
}

INT32 CBattleGui::GetWinnerTeam( void)
{
	if( m_pPanelResult == NULL)
		return -1;

	return m_pPanelResult->getWinnerTeam();
}

void CBattleGui::SetResult(ResultState state)
{
	if(g_pGameContext->IsTutorialMode())
	{
		return;
	}

	if( m_pPanelResult != NULL)
		m_pPanelResult->setResult( state);
}

INT32 CBattleGui::_ControlNotifyPump(void)
{
	if( m_pBattleGUIRoot == NULL) return 0;

	INT32 cPumpedNotify = 0;

	OnPreControlNotify();
	
	I3GUI_CONTROL_NOTIFY notify;
	while( m_pBattleGUIRoot->PopControlNotify(&notify) )
	{
		cPumpedNotify++;
		OnControlNotify(&notify);		
	}

	OnPostControlNotify();

	return cPumpedNotify;
}

void CBattleGui::_InitWeaponSelect(void)
{
	INIT_BUTTON_EX(m_pConfirm);
	INIT_BUTTON_EX(m_pCancel);

	for(UINT32 i = 0; i < MAX_CATEGORY_WEAPON; i++)
	{
		m_weaponSlot[i].m_pSlotEnable->SetEnable(FALSE);
		m_weaponSlot[i].m_pSlotDisable->SetEnable(TRUE);
		
		m_weaponSlot[i].m_pSlotKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponSlot[i].m_pSlotKey->SetTextSpace( 0, -4);
		
		INIT_BUTTON_EX(m_weaponSlot[i].m_pSlotPrev);
		INIT_BUTTON_EX(m_weaponSlot[i].m_pSlotNext);

		m_weaponSlot[i].m_pPrevWeapon->SetAllShapeEnable(FALSE);
		m_weaponSlot[i].m_pSelectWeapon->SetAllShapeEnable(FALSE);
		m_weaponSlot[i].m_pNextWeapon->SetAllShapeEnable(FALSE);		
		m_weaponSlot[i].m_pPrevWeapon->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
		m_weaponSlot[i].m_pSelectWeapon->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
		m_weaponSlot[i].m_pNextWeapon->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);

		m_weaponSlot[i].m_pPrevWeaponValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponSlot[i].m_pSelectWeaponValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponSlot[i].m_pNextWeaponValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponSlot[i].m_pPrevWeaponValue->setInputDisable(TRUE);
		m_weaponSlot[i].m_pSelectWeaponValue->setInputDisable(TRUE);
		m_weaponSlot[i].m_pNextWeaponValue->setInputDisable(TRUE);

		m_weaponSlot[i].m_pPrevWeaponCategory->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponSlot[i].m_pSelectWeaponCategory->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponSlot[i].m_pNextWeaponCategory->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponSlot[i].m_pPrevWeaponCategory->setInputDisable(TRUE);
		m_weaponSlot[i].m_pSelectWeaponCategory->setInputDisable(TRUE);
		m_weaponSlot[i].m_pNextWeaponCategory->setInputDisable(TRUE);

		m_weaponSlot[i].m_pNextWeaponDummy->setInputDisable(TRUE);
		m_weaponSlot[i].m_pSelectWeaponDummy->setInputDisable(TRUE);
		m_weaponSlot[i].m_pPrevWeaponDummy->setInputDisable(TRUE);

		m_weaponSlot[i].m_pNextWeaponExtension->setInputDisable(TRUE);
		m_weaponSlot[i].m_pSelectWeaponExtension->setInputDisable(TRUE);
		m_weaponSlot[i].m_pPrevWeaponExtension->setInputDisable(TRUE);

		m_weaponSlot[i].m_pNextWeaponExtension->SetEnable(FALSE);
		m_weaponSlot[i].m_pSelectWeaponExtension->SetEnable(FALSE);
		m_weaponSlot[i].m_pPrevWeaponExtension->SetEnable(FALSE);
	}

	UINT8 WeaponFlag = g_pGameContext->GetMyRoomWeaponFlag();

	for(UINT32 i = 0; i < 4; i++)
	{
		if (GlobalFunc::CheckAvailableWeaponUsage(WeaponFlag, (WEAPON_SLOT_TYPE)i))
		{
			m_weaponSlot[i].m_pSlotEnable->SetEnable(TRUE);
			m_weaponSlot[i].m_pSlotDisable->SetEnable(FALSE);

			break;
		}
	}

	m_weaponSlot[WEAPON_SLOT_PRIMARY].m_pSlotKey->SetText(GAME_STRING("STR_TBL_BATTLEGUI_PRIMARY_WEAPON"));
	m_weaponSlot[WEAPON_SLOT_SECONDARY].m_pSlotKey->SetText(GAME_STRING("STR_TBL_BATTLEGUI_SECONDRY_WEAPON"));
	m_weaponSlot[WEAPON_SLOT_MELEE].m_pSlotKey->SetText(GAME_STRING("STR_TBL_BATTLEGUI_MELEE_WEAPON"));
	m_weaponSlot[WEAPON_SLOT_THROWING1].m_pSlotKey->SetText(GAME_STRING("STR_TBL_BATTLEGUI_THROW_WEAPON"));
	m_weaponSlot[WEAPON_SLOT_THROWING2].m_pSlotKey->SetText(GAME_STRING("STR_TBL_BATTLEGUI_SPECIAL_WEAPON"));

	if (LOCK_BASIC != WeaponFlag)
	{
		m_pLimitKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pLimitKey->SetText(GAME_STRING("STR_TBL_BATTLEGUI_NOTICE_WEAPON_LIMIT"));
	}
	else
	{
		m_pLimitKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pLimitKey->SetText(GAME_STRING("STR_TBL_BATTLEGUI_NOTICE_SELECETWEAPON"));
	}

	for(UINT32 i = 0; i < 4; i++)
	{
		if (GlobalFunc::CheckAvailableWeaponUsage(WeaponFlag, (WEAPON_SLOT_TYPE)i) == false)
		{
			m_weaponSlot[i].m_pSlotBg->setInputDisable(TRUE);
		}
	}
}

void CBattleGui::RPG7_SelectWeapon()
{
	CWeaponInfo* pInfo = NULL;
	i3::string strWeaponName;
	pInfo = g_pWeaponInfoDataBase->getWeaponInfo(WEAPON_CLASS_ROCKET_LAUNCHER, GetItemIndex("ROCKET_LAUNCHER_RPG7"));
	I3ASSERT(pInfo);
	
	g_pFramework->SetWeaponSelectMark(m_weaponSlot[0].m_pSelectWeaponDummy, WEAPON_CLASS_ROCKET_LAUNCHER, (UINT8) GetItemIndex("ROCKET_LAUNCHER_RPG7")); 
	pInfo->GetWeaponName(strWeaponName);
	m_weaponSlot[0].m_pSelectWeaponValue->SetText(strWeaponName.c_str() );
	m_weaponSlot[0].m_pSelectWeaponValue->SetEnable(TRUE);

	//	���� ī�װ�
	_SetWeaponCategory( m_weaponSlot[0].m_pSelectWeaponCategory, pInfo);

	m_weaponSlot[0].m_pSlotPrev->setControlDisable(TRUE);
	m_weaponSlot[0].m_pSlotNext->setControlDisable(TRUE);
	
	m_weaponSlot[0].m_pSlotEnable->SetEnable(FALSE);

	return;
}

void CBattleGui::_SetSelectWeaponName(INT32 idxWeapon, i3GuiEditBox * pControl, const i3::string& weaponName, INT32 flag)
{
	I3COLOR nameColor;

	pControl->SetText( weaponName.c_str() );
	pControl->SetEnable(TRUE);

	{
#if defined( USE_REPAIR_WEAPON )
		//	check endurance zero weapon
		BOOL	bBroken	= FALSE;

		if( g_pGameContext->IsNewRepair() )
		{
			CShopContext * pShopContext = g_pFramework->GetShopContext();

			if( pShopContext->IsDurabilityItem(flag) )
			{
				REAL32		fRate	= 0.0f;

				pShopContext->GetDurability(flag, fRate);

				if( fRate <= 0.0f )
					bBroken = TRUE;
			}
		}
		else
		{
			UINT8				itemAttr	= 0;
			UINT32				itemArg		= 0;
			WEAPON_SLOT_TYPE	usage		= item_def::get_weapon_slot_type(flag);

			if( CInvenList::instance()->GetWeaponType( idxWeapon, usage, &itemAttr, &itemArg))
			{
				if( GetEnduranceZero(itemAttr, itemArg) )
					bBroken = TRUE;
			}
		}

		if( bBroken)
		{
			i3Color::Set(&nameColor, (UINT8) 210, 0, 0, 255);
		}
		else
#endif
		{
			i3Color::Set(&nameColor, (UINT8) 215, 211, 210, 255);
		}

		pControl->SetTextColor( &nameColor);				
	}
}

BOOL	CBattleGui::_IsUseableWeapon(INT32 IdxWeapon, INT32 WeaponUsage, UINT8 ItemAttr)
{
	// �������� ����� ��� �Ⱓ�������� �����ȵ� ����� ������ ������ �� �������ϴ�.
	// �׷��� 09-06-20�� ���ķδ� ������ ���Ⱑ �߰��Ǿ����ϴ�.
	// ������ ����� ������ �ȵȻ��·� �κ��� �ְ� �Ǵµ� ������ ���⵵ ������
	// ������ �� �ֵ��� ����ó���� ���ݴϴ�. (���� 09-06-22)

	AUTH_TYPE AuthType = AUTH_UNKNWON;
	
	CShop * pShop = g_pGameContext->GetShop();
	I3ASSERT( pShop != NULL);

	// ���� ��Ʈ��ũ ������϶��� ����. �����ϱ�� �ַ��÷��� ��� �������� �ʴ´�.
	ISNETWORK
	{
		UINT32 ItemID = CInvenList::instance()->GetWeaponID(IdxWeapon, (WEAPON_SLOT_TYPE) WeaponUsage);
		AuthType = pShop->GetAuthType(ItemID);
	}

	if ( ITEM_ATTR_TYPE_BUY != ItemAttr || (ITEM_ATTR_TYPE_BUY == ItemAttr && AUTH_COUNT == AuthType) )
		return TRUE;

	return FALSE;
}

void CBattleGui::_UpdateSelectWeapon(INT32 idxSlot)
{	
	INT32 idxWeapon;
	i3::string strWeaponName;
	switch(idxSlot)
	{
	case WEAPON_SLOT_PRIMARY:		idxWeapon = m_idxPrimaryWeapon;		break;
	case WEAPON_SLOT_SECONDARY:	idxWeapon = m_idxSecondaryWeapon;	break;
	case WEAPON_SLOT_MELEE:		idxWeapon = m_idxMeleeWeapon;		break;
	case WEAPON_SLOT_THROWING1:		idxWeapon = m_idxThrowWeapon;		break;
	case WEAPON_SLOT_THROWING2:			idxWeapon = m_idxWeaponItem;		break;
	default:
		return;
	}

	CWeaponInfo* pInfo = NULL;
	
	INT32 weaponCount = CInvenList::instance()->GetWeaponCount( (WEAPON_SLOT_TYPE) idxSlot);

	m_weaponSlot[idxSlot].m_pPrevWeapon->SetAllShapeEnable(FALSE);
	m_weaponSlot[idxSlot].m_pPrevWeapon->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
	m_weaponSlot[idxSlot].m_pPrevWeaponDummy->SetAllShapeEnable(FALSE);
	m_weaponSlot[idxSlot].m_pPrevWeaponDummy->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 125);
	m_weaponSlot[idxSlot].m_pPrevWeaponValue->SetEnable(FALSE);
	m_weaponSlot[idxSlot].m_pPrevWeaponCategory->SetEnable(FALSE);
	m_weaponSlot[idxSlot].m_pPrevWeaponExtension->SetEnable(FALSE);
	m_weaponSlot[idxSlot].m_pSelectWeapon->SetAllShapeEnable(FALSE);
	m_weaponSlot[idxSlot].m_pSelectWeapon->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
	m_weaponSlot[idxSlot].m_pSelectWeaponDummy->SetAllShapeEnable(FALSE);
	m_weaponSlot[idxSlot].m_pSelectWeaponDummy->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 125);
	m_weaponSlot[idxSlot].m_pSelectWeaponValue->SetEnable(FALSE);
	m_weaponSlot[idxSlot].m_pSelectWeaponCategory->SetEnable(FALSE);
	m_weaponSlot[idxSlot].m_pSelectWeaponExtension->SetEnable(FALSE);
	m_weaponSlot[idxSlot].m_pNextWeapon->SetAllShapeEnable(FALSE);
	m_weaponSlot[idxSlot].m_pNextWeapon->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
	m_weaponSlot[idxSlot].m_pNextWeaponDummy->SetAllShapeEnable(FALSE);
	m_weaponSlot[idxSlot].m_pNextWeaponDummy->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 125);
	m_weaponSlot[idxSlot].m_pNextWeaponValue->SetEnable(FALSE);
	m_weaponSlot[idxSlot].m_pNextWeaponCategory->SetEnable(FALSE);
	m_weaponSlot[idxSlot].m_pNextWeaponExtension->SetEnable(FALSE);	

	BOOL bPrevDisable = TRUE;
	BOOL bNextDisable = TRUE;
	
	//REFACTOR_TARGET_SHOTGUN
	// ���� ��� �ִ� ���Ⱑ �κ��� ���� ������ ���
	// ���⼱��â���� ������ �߻��մϴ�.
	// �� ó���� ���ؼ� �Ʒ� ���ǹ���
	// idxWeapon == -1�� �־����ϴ�.
	// �κ��� ���� ���⸦ ��� ���� ���� ���������� 
	// ������ ���� �� �ӽ� ����� ������ ������ ������ �� �Դϴ�.
	// ���� ����Ǿ�� �մϴ�.
	//
	// �Ʒ� ������ ���Ⱑ �������̴�.
	if (0 == weaponCount || idxWeapon == -1 || idxWeapon == weaponCount)
	{
		m_weaponSlot[idxSlot].m_pSlotPrev->setControlDisable(bPrevDisable);
		m_weaponSlot[idxSlot].m_pSlotNext->setControlDisable(bNextDisable);
		m_weaponSlot[idxSlot].m_pSelectWeapon->SetShapeEnable(0, FALSE);
		m_weaponSlot[idxSlot].m_pSelectWeapon->SetShapeEnable(1, FALSE);
		m_weaponSlot[idxSlot].m_pSelectWeapon->SetShapeEnable(2, TRUE);

		return;
	}

	
	// ���� ���� Ȯ��
	UINT8 MyRoomWeaponFlag = g_pGameContext->GetMyRoomWeaponFlag();

	BOOL bLimit = FALSE;

	if (idxSlot < 4 && GlobalFunc::CheckAvailableWeaponUsage(MyRoomWeaponFlag, (WEAPON_SLOT_TYPE)idxSlot) == false)
	{
		bLimit = TRUE;
	}

	INT32 WeaponItemID = CInvenList::instance()->GetWeaponID(idxWeapon, (WEAPON_SLOT_TYPE) idxSlot);

	if( idxSlot == 0 && TESTBIT(MyRoomWeaponFlag, LOCK_RPG7))
	{
		RPG7_SelectWeapon();
		return;
	}

	UINT8 attr;
	UINT32 arg;
	if (CInvenList::instance()->GetWeaponType( idxWeapon, (WEAPON_SLOT_TYPE) idxSlot, &attr, &arg))
	{
		// ����Ҽ� ���� �������̸� �ε����� ������ ������Ų��.
		if( ! _IsUseableWeapon(idxWeapon, idxSlot, attr) )
		{
			switch(idxSlot)
			{
			case WEAPON_SLOT_PRIMARY:
				m_idxPrimaryWeapon = _GetNextUsingWeapon(m_idxPrimaryWeapon, WEAPON_SLOT_PRIMARY, MyRoomWeaponFlag);			
				break;
			case WEAPON_SLOT_SECONDARY:
				m_idxSecondaryWeapon = _GetNextUsingWeapon(m_idxSecondaryWeapon, WEAPON_SLOT_SECONDARY, MyRoomWeaponFlag);
				break;
			case WEAPON_SLOT_MELEE:
				m_idxMeleeWeapon = _GetNextUsingWeapon(m_idxMeleeWeapon, WEAPON_SLOT_MELEE, MyRoomWeaponFlag);
				break;
			case WEAPON_SLOT_THROWING1:
				m_idxThrowWeapon = _GetNextUsingWeapon(m_idxThrowWeapon, WEAPON_SLOT_THROWING1, MyRoomWeaponFlag);
				break;
			case WEAPON_SLOT_THROWING2:
				m_idxWeaponItem = _GetNextUsingWeapon(m_idxWeaponItem, WEAPON_SLOT_THROWING2, MyRoomWeaponFlag);
				break;
			}

			return;
		}
	}
	
	//	���� ������ ���� ��� ���Ⱑ �κ��� ���� �����Դϴ�. Ȯ�����ּ���.
	if( WeaponItemID == 0)
	{
		char temp[MAX_STRING_COUNT] = "";
		i3String::Format( temp, sizeof(temp), GAME_STRING("STR_TBL_BATTLEGUI_WEAPON_NO_INVEN"), idxSlot+1);

		GameUI::MsgBox( MSG_TYPE_GAME_OK, temp);
	
		m_bEnableRespawn = FALSE;
		m_bEnableSelectWep = FALSE;	

		UIBattleFrame::i()->ClosePopup( UBP_SELECT_WEAPON );

		//m_pStaticRespawnWepSelectBG->SetEnable(FALSE);
		m_pStaticRespawnBG->SetEnable(FALSE);

		g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);

		return;
	}
	else  //	if( WeaponItemID != 0)			WeaponItemID == 0 �� ��� �ٷ� ���ϵǹǷ�, �� ���ǹ��� �ǹ̾��� ( 11.11.07.����)
	{
		WEAPON_CLASS_TYPE classType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(WeaponItemID);
		INT32 number = GET_ITEM_FLAG_NUMBER(WeaponItemID);

		if( idxSlot == 0 && TESTBIT(MyRoomWeaponFlag, LOCK_SNIPER) )
		{
			if( classType != WEAPON_CLASS_SNIPER)
			{
				m_idxPrimaryWeapon++;
				return;
			}
		}
		else if( idxSlot == 0 && TESTBIT(MyRoomWeaponFlag, LOCK_SHOTGUN) )
		{
			if( classType != WEAPON_CLASS_SHOTGUN)
			{
				m_idxPrimaryWeapon++;
				return;
			}
		}
		else if( idxSlot == 2 && TESTBIT(MyRoomWeaponFlag, LOCK_KNUCKLE) )
		{
			if( classType != WEAPON_CLASS_KNUCKLE)
			{
				m_idxMeleeWeapon++;
				return;
			}
		}

		GUI_WEAPON_VALID( classType, number);
		
		pInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number);
		I3ASSERT(pInfo);

		g_pFramework->SetWeaponSelectMark(m_weaponSlot[idxSlot].m_pSelectWeaponDummy, (UINT8) classType, (UINT8) number); 
		
		pInfo->GetWeaponName(strWeaponName);
		//	Set Next weapon name
		_SetSelectWeaponName( idxWeapon, 
								m_weaponSlot[idxSlot].m_pSelectWeaponValue, 
								strWeaponName,
                                WeaponItemID);

		//	���� ī�װ�
		_SetWeaponCategory( m_weaponSlot[idxSlot].m_pSelectWeaponCategory, pInfo);

		//	���� ���� �ͽ��ټ� ǥ��
		if(pInfo->isExtensible())
			m_weaponSlot[idxSlot].m_pSelectWeaponExtension->SetEnable(TRUE);		

		if (FALSE == bLimit)
		{
			m_weaponSlot[idxSlot].m_pSelectWeaponDummy->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 255);
			m_weaponSlot[idxSlot].m_pSelectWeapon->SetAllShapeEnable(FALSE);
			m_weaponSlot[idxSlot].m_pSelectWeapon->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
		}
	}
//	else
//	{
//		m_weaponSlot[idxSlot].m_pSelectWeaponDummy->SetEnable(FALSE);
//	}

	if (1 == weaponCount)
	{
		m_weaponSlot[idxSlot].m_pSlotPrev->setControlDisable(bPrevDisable);
		m_weaponSlot[idxSlot].m_pSlotNext->setControlDisable(bNextDisable);
		return;
	}

	// ����
	INT32 idxPrevWeapon = _GetPrevUsingWeapon(idxWeapon, idxSlot, MyRoomWeaponFlag);
	
	if( (2 < weaponCount || idxPrevWeapon < idxWeapon) && (idxPrevWeapon != idxWeapon) )
	{
		WeaponItemID = CInvenList::instance()->GetWeaponID(idxPrevWeapon, (WEAPON_SLOT_TYPE) idxSlot);
		if( WeaponItemID != 0)
		{
			WEAPON_CLASS_TYPE classType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(WeaponItemID);
			INT32 number = GET_ITEM_FLAG_NUMBER(WeaponItemID);

			GUI_WEAPON_VALID( classType, number);

			pInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number);
			pInfo->GetWeaponName(strWeaponName);
			I3ASSERT(pInfo);
			g_pFramework->SetWeaponSelectMark(m_weaponSlot[idxSlot].m_pPrevWeaponDummy, (UINT8) classType, (UINT8) number); 
			
			//	Set Prev weapon name
			_SetSelectWeaponName( idxPrevWeapon, 
								  m_weaponSlot[idxSlot].m_pPrevWeaponValue, 
								  strWeaponName,
                                  WeaponItemID);

			//	���� ī�װ�
			_SetWeaponCategory( m_weaponSlot[idxSlot].m_pPrevWeaponCategory, pInfo);

			if (FALSE == bLimit)
			{
				m_weaponSlot[idxSlot].m_pPrevWeaponDummy->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 255);
				m_weaponSlot[idxSlot].m_pSlotPrev->setControlDisable(FALSE);
			}
			bPrevDisable = FALSE;

			m_weaponSlot[idxSlot].m_pPrevWeapon->SetAllShapeEnable(FALSE);
			m_weaponSlot[idxSlot].m_pPrevWeapon->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);

			//	���� ���� �ͽ��ټ� ǥ��
			if(pInfo->isExtensible())
				m_weaponSlot[idxSlot].m_pPrevWeaponExtension->SetEnable(TRUE);		
		}
		else
		{
			m_weaponSlot[idxSlot].m_pPrevWeaponDummy->SetEnable(FALSE);
		}
	}

	// ����
	INT32 idxNextWeapon = _GetNextUsingWeapon(idxWeapon, idxSlot, MyRoomWeaponFlag);

	if( (2 < weaponCount || idxNextWeapon > idxWeapon) && (idxNextWeapon != idxWeapon) )
	{
		WeaponItemID = CInvenList::instance()->GetWeaponID(idxNextWeapon, (WEAPON_SLOT_TYPE) idxSlot);
		if( WeaponItemID != 0)
		{
			WEAPON_CLASS_TYPE classType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(WeaponItemID);
			INT32 number = GET_ITEM_FLAG_NUMBER(WeaponItemID);

			GUI_WEAPON_VALID( classType, number);

			pInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number);
			pInfo->GetWeaponName(strWeaponName);
			I3ASSERT(pInfo);
			
			g_pFramework->SetWeaponSelectMark(m_weaponSlot[idxSlot].m_pNextWeaponDummy, (UINT8) classType, (UINT8) number); 
			
			//	Set Next weapon name
			_SetSelectWeaponName( idxNextWeapon, 
								  m_weaponSlot[idxSlot].m_pNextWeaponValue, 
								 strWeaponName,
                                  WeaponItemID);

			//	���� ī�װ�
			_SetWeaponCategory( m_weaponSlot[idxSlot].m_pNextWeaponCategory, pInfo);

			if (FALSE == bLimit)
			{
				m_weaponSlot[idxSlot].m_pNextWeaponDummy->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 255);
				m_weaponSlot[idxSlot].m_pSlotNext->setControlDisable(FALSE);
			}
			bNextDisable = FALSE;

			m_weaponSlot[idxSlot].m_pNextWeapon->SetAllShapeEnable(FALSE);
			m_weaponSlot[idxSlot].m_pNextWeapon->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);

			//	���� ���� �ͽ��ټ� ǥ��
			if(pInfo->isExtensible())
				m_weaponSlot[idxSlot].m_pNextWeaponExtension->SetEnable(TRUE);
		}
		else
		{
			m_weaponSlot[idxSlot].m_pNextWeaponDummy->SetEnable(FALSE);
		}
	}


	// ��ư ���� ó��
	m_weaponSlot[idxSlot].m_pSlotPrev->setControlDisable(bPrevDisable);
	m_weaponSlot[idxSlot].m_pSlotNext->setControlDisable(bNextDisable);
}

void CBattleGui::_SetWeaponCategory( i3GuiEditBox * pControl, CWeaponInfo * pInfo)
{
	switch(pInfo->GetTypeClass())
	{
	case WEAPON_CLASS_ASSAULT:		//	���ݼ���
	case WEAPON_CLASS_SMG:			//	�������
	case WEAPON_CLASS_SNIPER:		//	���ݼ���	
	case WEAPON_CLASS_SHOTGUN:		//	��ź��
	case WEAPON_CLASS_MG:			//	�����
		{
			pControl->SetText( g_pWeaponInfoDataBase->GetWeaponClass( pInfo));
			pControl->SetEnable(TRUE);
		}
		break;
	default:
		{
			pControl->SetText("");
			pControl->SetEnable(FALSE);
		}
		break;
	}
}

INT32 CBattleGui::_GetNextUsingWeapon(INT32 idx,INT32 usage, UINT8 MyRoomWeaponFlag)
{
	INT32 count = CInvenList::instance()->GetWeaponCount( (WEAPON_SLOT_TYPE) usage);

	INT32 idxNext = idx;

	for(INT32 i = 0; i < count; i++)
	{
		idxNext = LOOP_INCREASE(idxNext, 0, count - 1);

		UINT8 attr;
		UINT32 arg;

		if (CInvenList::instance()->GetWeaponType(idxNext, (WEAPON_SLOT_TYPE) usage, &attr, &arg))
		{
			if (_IsUseableWeapon(idxNext, usage, attr))
			{
				INT32 iFlag = CInvenList::instance()->GetWeaponID( idxNext, (WEAPON_SLOT_TYPE) usage);
				WEAPON_CLASS_TYPE classType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(iFlag);

				if( usage == WEAPON_SLOT_PRIMARY)
				{
					if( idxNext == idx)
						return idx;

					if( TESTBIT( MyRoomWeaponFlag, LOCK_SNIPER) )
					{
						if( classType != WEAPON_CLASS_SNIPER)
						{
							continue;
						}
					}
					else if( TESTBIT( MyRoomWeaponFlag, LOCK_SHOTGUN) )
					{
						if( classType != WEAPON_CLASS_SHOTGUN)
						{
							continue;
						}
					}
					else if( TESTBIT( MyRoomWeaponFlag, LOCK_KNUCKLE) )
					{
						if( classType != WEAPON_CLASS_KNUCKLE)
						{
							continue;
						}
					}
				}
				else if( usage == WEAPON_SLOT_MELEE)
				{
					if( idxNext == idx)
						return idx;

					if( TESTBIT( MyRoomWeaponFlag, LOCK_KNUCKLE) )
					{
						if( classType != WEAPON_CLASS_KNUCKLE)
						{
							continue;
						}
					}
				}

				return idxNext;
			}
		}
	}

	return idx;
}

INT32 CBattleGui::_GetPrevUsingWeapon( INT32 idx, INT32 usage, UINT8 MyRoomWeaponFlag)
{
	INT32 count = CInvenList::instance()->GetWeaponCount( (WEAPON_SLOT_TYPE) usage);

	INT32 idxPrev = idx;

	for(INT32 i = 0; i < count; i++)
	{
		idxPrev = LOOP_DECREASE(idxPrev, 0, count - 1);

		UINT8 attr;
		UINT32 arg;

		if (CInvenList::instance()->GetWeaponType(idxPrev, (WEAPON_SLOT_TYPE) usage, &attr, &arg))
		{
			if (_IsUseableWeapon(idxPrev, usage, attr) )
			{
				INT32 iFlag = CInvenList::instance()->GetWeaponID( idxPrev, (WEAPON_SLOT_TYPE) usage);
				WEAPON_CLASS_TYPE classType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(iFlag);

				if( usage == WEAPON_SLOT_PRIMARY)
				{
					if( idxPrev == idx)
						return idx;

					if( TESTBIT( MyRoomWeaponFlag, LOCK_SNIPER) )
					{
						if( classType != WEAPON_CLASS_SNIPER)
						{
							continue;
						}
					}
					else if( TESTBIT( MyRoomWeaponFlag, LOCK_SHOTGUN) )
					{
						if( classType != WEAPON_CLASS_SHOTGUN)
						{
							continue;
						}
					}
					else if( TESTBIT( MyRoomWeaponFlag, LOCK_KNUCKLE) )
					{
						if( classType != WEAPON_CLASS_KNIFE)
						{
							continue;
						}
					}
				}
				else if( usage == WEAPON_SLOT_MELEE)
				{
					if( idxPrev == idx)
						return idx;

					if( TESTBIT( MyRoomWeaponFlag, LOCK_KNUCKLE) )
					{
						if( classType != WEAPON_CLASS_KNUCKLE)
						{
							continue;
						}
					}
				}

				return idxPrev;
			}
		}
	}

	return idx;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//#if defined( FINDCTRL_CRC)
//��Ʈ�ѵ��� �̸����� ���Ͽ� ������ ��ũ�ϴºκ��� ���ϸ� �ְ��־.
//��Ʈ���� ���� crc�˻����� �ϱ����� binlist�� �־� �����صӴϴ�.
typedef struct findctrlcrcinfo
{
	i3::vector_set<STRCINFO*>* plist;
	STRCINFO	* pFirst;
	INT32		nCurCnt;
}FCRCIF;

static bool _SetSoundInfoProc( i3GameNode * pNode)
{
	if( !i3::kind_of<i3GuiObjBase*>(pNode)) //->IsTypeOf( i3GuiObjBase::static_meta()))
		return false;

	i3GuiObjBase * pObj = (i3GuiObjBase *)pNode;
	//i3ResourceManager * pResMng = g_pFramework->GetResourceManager();
	//I3ASSERT( pResMng != NULL);

	for( INT32 i = 0; i < I3GUI_CONTROL_STATE_RESERVED3; ++i)
	{
		I3GUI_EVENT_DATA * pEvent = pObj->getEventData( (GUI_CONTROL_STATE)i);
		if( pEvent != NULL && (pEvent->m_nEventStyle & I3GUI_EVENT_SOUND))
		{
			i3SoundNode * pNode = pEvent->m_pSoundNode;

			i3GameResSound * pRes = ( i3GameResSound*)g_pFramework->QuaryResource( pNode->GetName());
			if( pRes == NULL)
				continue;

			i3SoundPlayInfo * pInfo = pNode->getSoundPlayInfo();
			i3SoundPlayInfo * pResInfo = pRes->getSound();

			pInfo->SetDefaultVolume( pResInfo->GetDefaultVolume());
			pInfo->setListenerVolume( pResInfo->getListenerVolume());
		}
	}

	return true;
}

static bool _SetRCCountProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( pNode == NULL)
		return false;

	if( pNode->hasName())
	{
		INT32 * pCnt = (INT32*)pUserData;

		*pCnt = *pCnt+1;
	}

	//�߰���..���� �����ϴ� �κ� �����־����ϴ�.���� ���ν������Ӱ��� ������� �������ּ��� -raja-
	_SetSoundInfoProc( pNode);

	return true;
}


static bool _SetRCInfoAddProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( pNode == NULL)
		return false;

	if( pNode->hasName())
	{
		FCRCIF * pInfo = (FCRCIF*)pUserData;

		STRCINFO * pNewInfo = pInfo->pFirst + pInfo->nCurCnt;

		pNewInfo->crc = CRC32( 0xFFFFFFFF, (UINT8 *) pNode->GetName(), i3String::Length( pNode->GetName()));
		pNewInfo->pCtrl = pNode;

		pInfo->plist->insert( pNewInfo);

		pInfo->nCurCnt++;
	}

	return true;
}

/*
static INT32 _ManagerCompProc( STRCINFO * p1, STRCINFO * p2)
{
	if( p1->crc > p2->crc)
		return 1;
	else if( p2->crc > p1->crc)
		return -1;
	else
		return 0;
}

static INT32 _ManagerFindProc( STRCINFO * p1, UINT32 crc)
{
	if( p1->crc > crc)
		return 1;
	else if( crc > p1->crc)
		return -1;
	else
		return 0;
}
*/





void CBattleGui::_LinkControl(void)
{
	// ù��° ��Ʈ���� ���´�
	i3GuiWindow* pWindow = (i3GuiWindow*)m_pBattleGUIRoot->getFirstChild();
	
	// �ڽ� ��Ʈ���� ���ٸ� �������� �ʴ´�
	if (NULL == pWindow)
	{
		return;
	}

	i3::vector_set<STRCINFO*> CtrlList;
	FCRCIF inf;

	//CtrlList.SetCompareProc((COMPAREPROC)_ManagerCompProc);

	{
		i3GameNodeTraversal * pTraverse = i3GameNodeTraversal::new_object();

		INT32 nCnt = 0;
		
		pTraverse->setUserProc( _SetRCCountProc);
		pTraverse->setUserData((void*)&nCnt);

		pTraverse->Traverse( pWindow);

		I3ASSERT( nCnt > 0);
		
		inf.nCurCnt = 0;
		inf.pFirst	= (STRCINFO*)i3MemAlloc( sizeof( STRCINFO) * nCnt);
		inf.plist	= &CtrlList;

		pTraverse->setUserProc( _SetRCInfoAddProc);
		pTraverse->setUserData((void*)&inf);

		pTraverse->Traverse( pWindow);

		I3_SAFE_RELEASE( pTraverse);
	}

//	CtrlList.SetCompareProc((COMPAREPROC)_ManagerFindProc);

	_LinkControlByHash( &CtrlList, (i3GuiControl*)pWindow);

	I3MEM_SAFE_FREE_POINTER( inf.pFirst);

	CtrlList.clear();
}

void CBattleGui::_LinkControlByHash(const i3::vector_set<STRCINFO*>* plist, i3GuiControl * pControl)
{
	{
        // Respawn
		LinkControlEx(*plist,  "Respawn_BG", m_pStaticRespawnBG, -1);
		LinkControlEx(*plist,  "Respawn_Wep_SelectBG", m_pStaticRespawnWepSelectBG, -1);
		LinkControlEx(*plist,  "Respawn_Notice", m_pEditNotice, -1);
		LinkControlEx(*plist,  "Respawn_Time_Bar", m_pStaticTimeBar, -1);
		LinkControlEx(*plist,  "Respawn_Timer_Bar_BG", m_pStaticTimeBarBG, -1);
		LinkControlEx(*plist,  "Respawn_Time_Item", m_pStaticTimeItem, -1);
		LinkControlEx(*plist,  "Respawn_TimeText", m_pEditTimeItemText, -1);
		LinkControlEx(*plist,  "Respawn_Seconds", m_pStaticSeconds, -1);
		LinkControlEx(*plist,  "Respawn_Text", m_pStaticText, -1);

		// Training Result
		LinkControlEx(*plist, "Result_Training_BG",	m_pStaticTrainingBG,									-1);
		LinkControlEx(*plist, "Training_Confirm",		m_pButtonTrainingConfirm,								GUI_ID_BATTLE_BUTTON_TRAINING_CONFIRM );
		LinkControlEx(*plist, "Training_UpdateScore",	m_pButtonTrainingUpdateScore,							GUI_ID_BATTLE_BUTTON_TRAINING_UPDATE_SCORE );
		LinkControlEx(*plist, "Training_1",			m_pStaticTrainingNumber[0],								-1);
		LinkControlEx(*plist, "Training_10",			m_pStaticTrainingNumber[1],								-1);
		LinkControlEx(*plist, "Training_100",			m_pStaticTrainingNumber[2],								-1);
		LinkControlEx(*plist, "Training_1000",			m_pStaticTrainingNumber[3],								-1);
		LinkControlEx(*plist, "Training_10000",		m_pStaticTrainingNumber[4],								-1);
		LinkControlEx(*plist,  "Training_NickName",		m_pEditBoxTraining[GUI_TRAINING_TEXT_NICK],				-1);
		LinkControlEx(*plist,  "Training_TotalHitCount",	m_pEditBoxTraining[GUI_TRAINING_TEXT_TOTAL_HIT_COUNT],	-1);
		LinkControlEx(*plist,  "Training_TargetName1",	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET1],			-1);
		LinkControlEx(*plist,  "Training_Target1_Hit",	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET1_HIT],		-1);
		LinkControlEx(*plist,  "Training_Target1_Point",	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET1_POINT],	-1);
		LinkControlEx(*plist,  "Training_TargetName2",	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET2],			-1);
		LinkControlEx(*plist,  "Training_Target2_Hit",	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET2_HIT],		-1);
		LinkControlEx(*plist,  "Training_Target2_Point",	m_pEditBoxTraining[GUI_TRAINING_TEXT_TARGET2_POINT],	-1);

		// Exit Popup
		LinkControlEx(*plist, "Esc_PopupBG",			m_pStaticExitPopupBG,									-1);		
		LinkControlEx(*plist,  "Esc_PopupText",			m_pEditBoxExitPopupText,								-1);
		LinkControlEx(*plist, "Esc_Popup_Option",		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_OPTION],		GUI_ID_EXITPOPUP_BUTTON_OPTION );
		LinkControlEx(*plist, "Esc_Popup_Report",		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_REPORT],		GUI_ID_EXITPOPUP_BUTTON_REPORT );
		LinkControlEx(*plist, "Esc_Popup_ForciblyRemove",m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_FORCEREMOVE],	GUI_ID_EXITPOPUP_BUTTON_FORCEREMOVE);
		LinkControlEx(*plist, "Esc_Popup_Exit",		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_EXIT],		GUI_ID_EXITPOPUP_BUTTON_EXIT );
		LinkControlEx(*plist, "Esc_Popup_Cancel",		m_pButtonExitPopup[GUI_ID_EXITPOPUP_BUTTON_CANCEL],		GUI_ID_EXITPOPUP_BUTTON_CANCEL );

		// ���⼱�� ������
		LinkControlEx(*plist, "rsConfirm", m_pConfirm, GUI_ID_BATTLE_BUTTON_GAMESTART);
		LinkControlEx(*plist, "rsCancel", m_pCancel, GUI_ID_BATTLE_BUTTON_CANCEL);
		LinkControlEx(*plist, "rsLimitKey", m_pLimitKey, -1);

		for(UINT32 j = 0; j < MAX_CATEGORY_WEAPON; j++)
		{			
            char strName[MAX_STRING_COUNT];

			sprintf_s(strName, "rsSlotBg%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pSlotBg, -1);

			sprintf_s(strName, "rsSlotEnable%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pSlotEnable, -1);

			sprintf_s(strName, "rsSlotDisable%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pSlotDisable, -1);

			sprintf_s(strName, "rsSlotKey%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pSlotKey, -1);

			sprintf_s(strName, "rsSlotPrev%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pSlotPrev, GUI_ID_BATTLE_BUTTON_PREV_1 + j);

			sprintf_s(strName, "rsSlotNext%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pSlotNext, GUI_ID_BATTLE_BUTTON_NEXT_1 + j);

			sprintf_s(strName, "rsPrevWeapon%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pPrevWeapon, GUI_ID_BATTLE_STATIC_PREV_1 + j);

			sprintf_s(strName, "rsPrevWeaponDummy%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pPrevWeaponDummy, -1);			
			
			sprintf_s(strName, "rsPrevWeaponValue%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pPrevWeaponValue, -1);

			sprintf_s(strName, "rsPrevWeaponCategory%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pPrevWeaponCategory, -1);

			sprintf_s(strName, "rsPrevWeaponExtension%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pPrevWeaponExtension, -1);

			sprintf_s(strName, "rsSelectWeapon%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pSelectWeapon, -1);

			sprintf_s(strName, "rsSelectWeaponDummy%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pSelectWeaponDummy, -1);

			sprintf_s(strName, "rsSelectWeaponValue%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pSelectWeaponValue, -1);

			sprintf_s(strName, "rsSelectWeaponCategory%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pSelectWeaponCategory, -1);

			sprintf_s(strName, "rsSelectWeaponExtension%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pSelectWeaponExtension, -1);

			sprintf_s(strName, "rsNextWeapon%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pNextWeapon, GUI_ID_BATTLE_STATIC_NEXT_1 + j);

			sprintf_s(strName, "rsNextWeaponDummy%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pNextWeaponDummy, -1);

			sprintf_s(strName, "rsNextWeaponValue%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pNextWeaponValue, -1);

			sprintf_s(strName, "rsNextWeaponCategory%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pNextWeaponCategory, -1);

			sprintf_s(strName, "rsNextWeaponExtension%d", j + 1);
			LinkControlEx(*plist, strName, m_weaponSlot[j].m_pNextWeaponExtension, -1);
		}

		m_pStaticRespawnWepSelectBG->SetEnable( FALSE );
	}

	/*m_pInGamePopup->OnLinkControl( list, pControl);*/

	if( m_pPanelResult != NULL)
		m_pPanelResult->Create( this, plist, pControl);
}

void CBattleGui::_ProcessWheel(void)
{
	// ���콺 �ٹ�ư �˻�
	VEC3D mouseDelta;
	i3Vector::Copy(&mouseDelta, g_pFramework->getMouseMoveDelta());

	m_rWheelDelta += mouseDelta.z;

	INT32 scrollby = (INT32)(m_rWheelDelta / WHEEL_DELTA);
	
	// ������Ʈ ���� �ʾ����� ó������ �ʴ´�
	if (0 == scrollby)
	{
		return;
	}

	/*m_pInGamePopup->OnWheelUpdate(scrollby);*/

	// �� ��Ÿ �ʱ�ȭ - WHEEL_DELTA���� �۰� �������ٸ�, Ŀ�������� �����ؾ��Ѵ�
	if (WHEEL_DELTA <= m_rWheelDelta || -WHEEL_DELTA >= m_rWheelDelta)
	{
		m_rWheelDelta = 0.0f;
	}
}

void CBattleGui::UpdateScore( bool bVisible, REAL32 tm)
{
	// �Ϲ����� Battle������ BattleGUI�� Scoreboard�� ǥ������ �ʱ⶧���� �� ���� ����.
	// �׷��� BOSS Mission�� ��쿡�� GUI ���·� Scoreboard�� ����Ѵ�.
	if( m_pPanel_BOSS_Score != NULL)
	{
		// �ݺ����� ȣ���� ��������!
		if(m_pPanel_BOSS_Score->isVisible() == bVisible) return;

		// �������� ���� ��� ȣ��ȴ�.
		m_pPanel_BOSS_Score->ShowScore( bVisible, tm);

		_UpdateRootEnableState();
	}
}

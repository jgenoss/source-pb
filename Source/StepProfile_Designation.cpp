#include "pch.h"
#include "StepProfile_Designation.h"
#include "GuiNotifyReceiver.h"
#include "QuestTextDictionary.h"
#include "StepProfileDef.h"
#include "TitleInfo.h"

I3_CLASS_INSTANCE( CStepProfile_Designation, i3ElementBase);

CStepProfile_Designation::CStepProfile_Designation()
{
	INT32 i;

	m_pNotifyBase	= NULL;

	m_pDesignationChangeText = NULL;
	
	m_pDesignationGetButton = NULL;
	m_pDesignationChangeButton = NULL;
	m_pDesignationNameText = NULL;
	m_pDesignationInfoText = NULL;
	m_pDesignationPlusEffectText = NULL;
	m_pDesignationMinusEffectText = NULL;
	m_pDesignationAddEffectText = NULL;

	for( i = 0; i < BADGE_TYPE_COUNT; i++)
	{
		m_tBadgeNeed[ i]._pBadgeNeedImage		= NULL;
		m_tBadgeNeed[ i]._pBadgeNeedText		= NULL;
		m_tBadgeNeed[ i]._pBadgeAppendixCount	= NULL;
		m_tBadgeNeed[ i]._pBadgeNeedCount		= NULL;
	}


	m_pDesignationNeedText = NULL;
	m_pDesignationNeedText2 = NULL;
	m_pDesignationNeedRankText = NULL;
	m_pDesignationNeedChangeExplain = NULL;

	m_pDesignationSect = NULL;
	m_pDesignationTreeCategory = NULL;
	m_pDesignationTreeEventCategory_BG = NULL;
	m_pDesignationTreeEventCategory = NULL;
	m_pDesignationDeatailCategory = NULL;
	m_pDesignationGetInfoCategory = NULL;
	m_pDesignationPlusEffectKey = NULL;
	m_pDesignationMinusEffectKey = NULL;
	m_pDesignationAddEffectKey = NULL;
	m_pDesignationNeedDesignationKey = NULL;
	m_pDesignationNeedRankKey = NULL;

	for( i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		m_pDesignationButton[i] = NULL;
		m_pDesignationImage[i] = NULL;
	}
	m_pDesignationSelectedImage = NULL;
	for( i = 0; i < MAX_EVENT_DESIGNATION; i++)
	{
		m_pDesignationEventButton[i] = NULL;
		m_pDesignationEventImage[i] = NULL;
	}

	m_pDesignationChangeBG = NULL;

	m_pDesignationChange_Slot1 = NULL;
	m_pDesignationChange_Slot2 = NULL;
	m_pDesignationChange_Slot3 = NULL;

	for( i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{	
		m_pDesignationChange_Slot_BG[i] = NULL;
		m_pDesignationChange_Slot_Image[i] = NULL;
		m_pDesignationChange_Slot_Image_Lock[i] = NULL;
		m_pDesignationChange_Slot_Text[i] = NULL;
	}

	m_SelectedDesignationIdx = -1;
	m_SelectedDesignationChangeSlotIdx = -1;
	m_SelectedDesignationReleaseSlotIdx = -1;
	m_DesignationMode = DESIGNATION_MODE_GET;	
	m_nCurShowingDesignation = DESIGNATION_TYPE_NONE;
	m_nCurDesignationSlotCount = 0;

	m_bIsMyInfoView = false;

	memset( m_bGetDesignation, 0, sizeof( m_bGetDesignation ) );
}

CStepProfile_Designation::~CStepProfile_Designation()
{
}

BOOL CStepProfile_Designation::LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl)
{
	INT32 i;
	char strName[ MAX_STRING_COUNT];

	m_pNotifyBase = pBase;

	LinkControlEx(plist, 		"prDesigzationSect",					m_pDesignationSect,					-1);

	LinkControlEx(plist, "prDesigzationChange_Text", m_pDesignationChangeText, -1);
	
	LinkControlEx(plist, "prDesigzationGetButton", m_pDesignationGetButton, GCI_PRS_B_GET_DESIGNATION);
	LinkControlEx(plist, "prDesigzationChangeButton", m_pDesignationChangeButton, GCI_PRS_B_GET_DESIGNATION_CHANGE);
	LinkControlEx(plist, "prDesigzationName", m_pDesignationNameText, -1);
	LinkControlEx(plist, "prDesigzationName_Info", m_pDesignationInfoText, -1);
	LinkControlEx(plist, "prDesigzationPlusEffect_Text", m_pDesignationPlusEffectText, -1);
	LinkControlEx(plist, "prDesigzationMinusEffect_Text", m_pDesignationMinusEffectText, -1);
	LinkControlEx(plist, "prDesigzationAddEffect_Text", m_pDesignationAddEffectText, -1);
	
	for( i = 0; i < BADGE_TYPE_COUNT; i++)
	{
		sprintf_s( strName, "prDesignation_NeedBadgeImage%d", i);
		LinkControlEx(plist,  strName,	m_tBadgeNeed[ i]._pBadgeNeedImage, -1);

		sprintf_s( strName, "prDesigzationNeedKey%d", i);
		LinkControlEx(plist,  strName,		m_tBadgeNeed[ i]._pBadgeNeedText, -1);

		sprintf_s( strName, "prDesignation_NeedAppendix%d", i);
		LinkControlEx(plist,  strName,		m_tBadgeNeed[ i]._pBadgeAppendixCount, -1);

		sprintf_s( strName, "prDesigzation_NeedCount%d", i);
		LinkControlEx(plist,  strName,		m_tBadgeNeed[ i]._pBadgeNeedCount, -1);
	}

	LinkControlEx(plist, "prNeedDesigzation_Text", m_pDesignationNeedText, -1);
	LinkControlEx(plist, "prNeedDesigzation_Text2", m_pDesignationNeedText2, -1);
	LinkControlEx(plist, "prNeedRank_Text", m_pDesignationNeedRankText, -1);
	LinkControlEx(plist, "prNeedChangeExplain", m_pDesignationNeedChangeExplain, -1);	

	LinkControlEx(plist, 		"Designation_Tree_Category_Key",		m_pDesignationTreeCategory,			-1);
	LinkControlEx(plist, 		"Designation_Tree_Event_Category",		m_pDesignationTreeEventCategory_BG, -1);
	LinkControlEx(plist, 		"Designation_Tree_Event_Category_Key",	m_pDesignationTreeEventCategory,	-1);
	LinkControlEx(plist, 		"prDesigzationDeatailCategoryKey",		m_pDesignationDeatailCategory,		-1);
	LinkControlEx(plist, 		"prDesigzationGetInfoNameKey",			m_pDesignationGetInfoCategory,		-1);
	LinkControlEx(plist, 		"prDesigzationPlusEffectKey",			m_pDesignationPlusEffectKey,		-1);
	LinkControlEx(plist, 		"prDesigzationMinusEffectKey",			m_pDesignationMinusEffectKey,		-1);
	LinkControlEx(plist, 		"prDesigzationAddEffectKey",			m_pDesignationAddEffectKey,			-1);
	LinkControlEx(plist, 		"prNeedDesigzationKey",					m_pDesignationNeedDesignationKey,	-1);
	LinkControlEx(plist, 		"prNeedRankKey",						m_pDesignationNeedRankKey,			-1);

	for( i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		sprintf_s(strName, "Designation_Tree_Slot%d", i + 1);
		LinkControlEx(plist, strName, m_pDesignationButton[i], GCI_PRS_B_DESIGNATION_SLOT_1 + i);

		sprintf_s(strName, "Designation_Tree_Slot_Image%d", i + 1);
		LinkControlEx(plist, strName, m_pDesignationImage[i], GCI_PRS_S_DESIGNATION_IMAGE_1 + i);
	}
	LinkControlEx(plist, "prDesigzationSlotImg", m_pDesignationSelectedImage, -1);
	for( i = 0; i < MAX_EVENT_DESIGNATION; i++)
	{
		sprintf_s(strName, "Designation_Tree_Option_Slot%d", i + 1);
		LinkControlEx(plist, strName, m_pDesignationEventButton[i], GCI_PRS_B_DESIGNATION_EVENT_SLOT_1 + i);

		sprintf_s(strName, "Designation_Tree_Option_Slot_Image%d", i + 1);
		LinkControlEx(plist, strName, m_pDesignationEventImage[i], GCI_PRS_S_DESIGNATION_EVENT_IMAGE_1 + i);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Change popup
	LinkControlEx(plist, 		"prDesigzationChangeBG",		m_pDesignationChangeBG,				-1);

	LinkControlEx(plist, 		"prDesigzationChange_Slot1",	m_pDesignationChange_Slot1,			GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT1);
	LinkControlEx(plist, 		"prDesigzationChange_Slot2",	m_pDesignationChange_Slot2,			GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT2);
	LinkControlEx(plist, 		"prDesigzationChange_Slot3",	m_pDesignationChange_Slot3,			GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT3);

	for( i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		sprintf_s(strName, "prDesigzationChange_Slot%d_BG", i + 1);
		LinkControlEx(plist, 	strName,	m_pDesignationChange_Slot_BG[i],			-1);

		sprintf_s(strName, "prDesigzationChange_Slot%d_Image", i + 1);
		LinkControlEx(plist, 	strName,	m_pDesignationChange_Slot_Image[i],			-1);

		sprintf_s(strName, "prDesigzationChange_Slot%d_Lock", i + 1);
		LinkControlEx(plist, 	strName,	m_pDesignationChange_Slot_Image_Lock[i],	-1);

		sprintf_s(strName, "prDesigzationChange_Slot%d_Text", i + 1);
		LinkControlEx(plist, 	strName,	m_pDesignationChange_Slot_Text[i],			-1);
	}

	return TRUE;
}

void CStepProfile_Designation::Init( void)
{
	INT32 i;

	m_bIsMyInfoView = true;
	
	// �ʱ⿡ ȣĪ �ý����� ��Ȱ��ȭ �Ǿ��ֽ��ϴ�.
	m_pDesignationSect->SetEnable( FALSE);

	m_pDesignationNameText->SetTextColor( GetColor(GCT_STATIC));
	m_pDesignationNameText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationInfoText->SetTextColor( GetColor(GCT_STATIC));
	m_pDesignationInfoText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

	m_pDesignationPlusEffectText->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_SIZE_PROFILE_TEXT);
	m_pDesignationPlusEffectText->SetTextColor( GetColor(GCT_STATIC));
	m_pDesignationPlusEffectText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pDesignationPlusEffectText->SetTextSpace(0, -2);

	m_pDesignationMinusEffectText->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_SIZE_PROFILE_TEXT);
	m_pDesignationMinusEffectText->SetTextColor( GetColor(GCT_STATIC));
	m_pDesignationMinusEffectText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pDesignationMinusEffectText->SetTextSpace(0, -2);

	m_pDesignationAddEffectText->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_SIZE_PROFILE_TEXT);
	m_pDesignationAddEffectText->SetTextColor( GetColor(GCT_STATIC));
	m_pDesignationAddEffectText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pDesignationAddEffectText->SetTextSpace(0, -2);

	// DesignationGetInfo
	g_pFramework->SetRibbonShape( m_tBadgeNeed[ BADGE_TYPE_RIBBON]._pBadgeNeedImage, FIXED_BADGE_NUM_RIBBON);
	g_pFramework->SetEnsignShape( m_tBadgeNeed[ BADGE_TYPE_ENSIGN]._pBadgeNeedImage, FIXED_BADGE_NUM_ENSIGN);
	g_pFramework->SetMedalShape( m_tBadgeNeed[ BADGE_TYPE_MEDAL]._pBadgeNeedImage, FIXED_BADGE_NUM_MEDAL);
	g_pFramework->SetMedalShape( m_tBadgeNeed[ BADGE_TYPE_MASTER]._pBadgeNeedImage, FIXED_BADGE_NUM_MASTER);

	for( i = 0; i < BADGE_TYPE_COUNT; i++)
	{
		m_tBadgeNeed[ i]._pBadgeNeedText->SetTextColor( GetColor(GCT_STATIC));
		m_tBadgeNeed[ i]._pBadgeNeedText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

		m_tBadgeNeed[ i]._pBadgeAppendixCount->SetTextColor( GetColor(GCT_STATIC));
		m_tBadgeNeed[ i]._pBadgeAppendixCount->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

		m_tBadgeNeed[ i]._pBadgeNeedCount->SetTextColor( GetColor(GCT_STATIC));
		m_tBadgeNeed[ i]._pBadgeNeedCount->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}

	m_tBadgeNeed[ BADGE_TYPE_RIBBON]._pBadgeNeedText->SetText( GAME_STRING("STR_TBL_PROFILE_RIBBON_TEXT2")); /*����*/
	m_tBadgeNeed[ BADGE_TYPE_ENSIGN]._pBadgeNeedText->SetText( GAME_STRING("STR_TBL_PROFILE_ENSIGN_TEXT2")); /*����*/
	m_tBadgeNeed[ BADGE_TYPE_MEDAL]._pBadgeNeedText->SetText( GAME_STRING("STR_TBL_PROFILE_MEDALS_TEXT2")); /*����*/
	m_tBadgeNeed[ BADGE_TYPE_MASTER]._pBadgeNeedText->SetText( GAME_STRING("STR_TBL_PROFILE_MASTER_MEDALS_TEXT")); /*������ ����*/


	m_pDesignationNeedText->SetTextColor( GetColor(GCT_STATIC));
	m_pDesignationNeedText->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedText2->SetTextColor( GetColor(GCT_STATIC));
	m_pDesignationNeedText2->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedRankText->SetTextColor( GetColor(GCT_STATIC));
	m_pDesignationNeedRankText->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	
	m_pDesignationTreeCategory->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDesignationTreeCategory->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationTreeCategory->SetText( GAME_STRING("STR_TBL_DESIGNATION_TREE"));/*{col:153,153,153,255}ȣĪ ���� Ʈ��{/col}*/

	m_pDesignationTreeEventCategory->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDesignationTreeEventCategory->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationTreeEventCategory->SetText( GAME_STRING("STR_TBL_DESIGNATION_EVENT_TEXT"));/*{col:153,153,153,255}�̺�Ʈ ȣĪ{/col}*/

	m_pDesignationDeatailCategory->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDesignationDeatailCategory->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationDeatailCategory->SetText( GAME_STRING("STR_TBL_DESIGNATION_GET_DETAIL_INFO"));/*{col:153,153,153,255}ȣĪ �� ����{/col}*/

	m_pDesignationGetInfoCategory->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDesignationGetInfoCategory->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationGetInfoCategory->SetText( GAME_STRING("STR_TBL_DESIGNATION_GET_INFO"));/*{col:153,153,153,255}ȣĪ ȹ�� ����{/col}*/

	m_pDesignationPlusEffectKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDesignationPlusEffectKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationPlusEffectKey->SetText( GAME_STRING("STR_TBL_DESIGNATION_PLUS_EFFECT"));/*{col:153,153,153,255}+ ȿ��   :{/col}*/

	m_pDesignationMinusEffectKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDesignationMinusEffectKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationMinusEffectKey->SetText( GAME_STRING("STR_TBL_DESIGNATION_MINUS_EFFECT"));/*{col:153,153,153,255}- ȿ��   :{/col}*/

	m_pDesignationAddEffectKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDesignationAddEffectKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationAddEffectKey->SetText( GAME_STRING("STR_TBL_DESIGNATION_ADD_EFFECT"));/*{col:153,153,153,255}�ΰ� ȿ��:{/col}*/

	m_pDesignationNeedDesignationKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDesignationNeedDesignationKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedDesignationKey->SetText( GAME_STRING("STR_TBL_DESIGNATION_NEEDED_TEXT"));/*{col:153,153,153,255}�ʿ� ȣĪ{/col}*/

	m_pDesignationNeedRankKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDesignationNeedRankKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedRankKey->SetText( GAME_STRING("STR_TBL_DESIGNATION_NEEDED_GRADE_TEXT"));/*{col:153,153,153,255}�ʿ� ���{/col}*/

	m_pDesignationChangeText->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDesignationChangeText->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationChangeText->SetText(GAME_STRING("STR_TBL_DESIGNATION_USE_TEXT"));/*ȣĪ ���*/
	
	{
		INIT_BUTTON_EX( m_pDesignationGetButton);
		{
			m_pDesignationGetButton->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
			m_pDesignationGetButton->SetCaptionColor( GetColor(GCT_VALUE));
			m_pDesignationGetButton->SetCaption( GAME_STRING("STR_TBL_DESIGNATION_GET_TEXT"));/*ȣĪ ȹ��*/
		}
	}
	{
		INIT_BUTTON_EX( m_pDesignationChangeButton);
		{
			m_pDesignationChangeButton->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
			m_pDesignationChangeButton->SetCaptionColor( GetColor(GCT_VALUE));
			m_pDesignationChangeButton->SetCaption( GAME_STRING("STR_TBL_DESIGNATION_CONFIRM"));/*Ȯ��*/
		}
	}
	
	m_pDesignationNeedChangeExplain->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDesignationNeedChangeExplain->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedChangeExplain->SetTextSpace( 0, 2);
	m_pDesignationNeedChangeExplain->SetText( GAME_STRING("STR_TBL_DESIGNATION_GET_EXPLAIN_MESSAGE"));/*������ ȣĪ�� ����� ������ �����ϰ� Ȯ�� ��ư�� ������ ȣĪ�� ��ϵ˴ϴ�.\n(��,{col:253,167,0,255}���ϰ迭{/col}�� ȣĪ�� ���ÿ� ����Ͻ� �� ������ ��ü�� �����մϴ�.)*/

	for( i = 0; i < MAX_EVENT_DESIGNATION; i++)
	{
		INIT_BUTTON_EX(m_pDesignationEventButton[i]);	
		m_pDesignationEventImage[i]->SetAllShapeEnable( FALSE);
	}

	for( i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		//INIT_BUTTON_EX(m_pDesignationButton[i]);	
		m_pDesignationButton[i]->setInputMode(I3GUI_INPUT_MODE_PRESS);

		m_pDesignationImage[i]->SetAllShapeEnable( FALSE);
		m_pDesignationImage[i]->SetShapeEnable( i, TRUE);
		m_pDesignationImage[i]->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 77);
	}

	for( i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		m_pDesignationChange_Slot_Image[i]->SetAllShapeEnable( FALSE);
		m_pDesignationChange_Slot_Image_Lock[i]->SetEnable(TRUE);

		m_pDesignationChange_Slot_Text[i]->ReCreateTextEx(  GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_SIZE_PROFILE_TEXT);
		m_pDesignationChange_Slot_Text[i]->SetTextColor( GetColor(GCT_DEFAULT));
		m_pDesignationChange_Slot_Text[i]->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

		m_pDesignationChange_Slot_BG[i]->setInputDisable(TRUE);
		m_pDesignationChange_Slot_Image_Lock[i]->setInputDisable(TRUE);
		m_pDesignationChange_Slot_Image[i]->setInputDisable(TRUE);
		m_pDesignationChange_Slot_Text[i]->setInputDisable(TRUE);
	}

	// �ʱ⿡ ȣģ���� ���̾�α״� ��Ȱ��ȭ �մϴ�.
	m_pDesignationChangeBG->SetEnable( FALSE);

	// ���� ������� �����Ƿ� ��Ȱ��ȭ�մϴ�.
	{
		// ȣĪ �̺�Ʈ
		{
			m_pDesignationTreeEventCategory_BG->SetEnable( FALSE);
			for( i = 0; i < MAX_EVENT_DESIGNATION; i++)
				m_pDesignationEventButton[i]->SetEnable( FALSE);
		}
	}


}

void CStepProfile_Designation::SetEnable( BOOL bEnable)
{
	m_pDesignationSect->SetEnable( bEnable);
	__UpdateGetInfoBox();
}

void CStepProfile_Designation::Update( REAL32 tm)
{
	INT32 i;

	for( i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		m_pDesignationButton[ i]->SetShapeEnable( 1, FALSE);
	}
	
	if( m_pDesignationChangeBG->isEnable() )
		__UpdateChangePopup( tm);
}

void CStepProfile_Designation::__OnMouseDesignationButton( INT32 iIdx)
{
	if( iIdx >= 0 && iIdx < MAX_DEFAULT_USER_TITLE_COUNT)
	{
		if( m_pDesignationButton[ iIdx]->GetShapeEnable(3))
		{
			m_pDesignationButton[ iIdx]->SetAllShapeEnable( FALSE);
			m_pDesignationButton[ iIdx]->SetShapeEnable( 1, TRUE);
			m_pDesignationButton[ iIdx]->SetShapeEnable( 3, TRUE);
		}
		else
		{
			m_pDesignationButton[ iIdx]->SetAllShapeEnable( FALSE);
			m_pDesignationButton[ iIdx]->SetShapeEnable( 1, TRUE);
		}
	}
}

void CStepProfile_Designation::__OnClickedDesignationButton( INT32 iIdx)
{
	if( iIdx >= 0 && iIdx < MAX_DEFAULT_USER_TITLE_COUNT)
	{
		if( (m_SelectedDesignationIdx != -1) && (m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT) )
			m_pDesignationButton[ m_SelectedDesignationIdx]->SetShapeEnable( 2, FALSE);

		m_SelectedDesignationIdx = iIdx;

		if( m_pDesignationButton[ iIdx]->GetShapeEnable(3))
		{
			m_pDesignationButton[ iIdx]->SetAllShapeEnable( FALSE);
			m_pDesignationButton[ iIdx]->SetShapeEnable( 2, TRUE);
			m_pDesignationButton[ iIdx]->SetShapeEnable( 3, TRUE);
		}
		else
		{
			m_pDesignationButton[ iIdx]->SetAllShapeEnable( FALSE);
			m_pDesignationButton[ iIdx]->SetShapeEnable( 2, TRUE);
		}
		m_pNotifyBase->PlaySound( GUI_SND_ITEM_SELECTING);

		////////////////////////////////////////////////////////////////////
		// �׽�Ʈ Ȯ�� �ڵ� 
		// ȣĪ ȹ��� ȣĪ ���� ���� ���� Ȯ��â
		// ȣĪ ȹ��� ���� �����츦 ������ ���� ȣĪ�� ȹ���ؾ� �ϴ� ������������ ������� ���� ���� ȣĪ ȹ��â�� ��� ������Ѵ�.
		////////////////////////////////////////////////////////////////////
#if 0
		DESIGNATION_SET * pSelectedInfo = g_pDesignationValueSet->getDesignationSet( m_SelectedDesignationIdx);

		m_pDesignationSelectedImage->SetShapeEnable( m_SelectedDesignationIdx, TRUE);			
		INT32 CurDesignationType = pSelectedInfo->getType();

		__OpenGainWindowForDesignationWhenGetUserTitle( CurDesignationType);
#endif
	}
}

void CStepProfile_Designation::__OnDoubleClickedDesignationButton( INT32 iIdx)
{
	if( iIdx >= 0 && iIdx < MAX_DEFAULT_USER_TITLE_COUNT)
	{
		if( (m_SelectedDesignationIdx != -1) && (m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT) )
			m_pDesignationButton[ m_SelectedDesignationIdx]->SetShapeEnable( 2, FALSE);

		m_SelectedDesignationIdx = iIdx;
				
		if( m_pDesignationButton[ iIdx]->GetShapeEnable(3))
		{
			m_pDesignationButton[ iIdx]->SetAllShapeEnable( FALSE);
			m_pDesignationButton[ iIdx]->SetShapeEnable( 2, TRUE);
			m_pDesignationButton[ iIdx]->SetShapeEnable( 3, TRUE);
		}
		else
		{
			m_pDesignationButton[ iIdx]->SetAllShapeEnable( FALSE);
			m_pDesignationButton[ iIdx]->SetShapeEnable( 2, TRUE);
		}

		m_pNotifyBase->PlaySound( GUI_SND_ITEM_SELECTING);

		if( m_bGetDesignation[m_SelectedDesignationIdx])
		{
			if( __CheckDesignationEquip())
			{
				// ȣĪ ���� �˾�
				STRING_POPUP_Q( GAME_STRING("STR_TBL_DESIGNATION_RELEASE_ANSWER_MESSAGE"), MAKE_CALLBACK(CbDesignationRelese), this);/*������ ȣĪ�� �����Ͻðڽ��ϱ�?*/
			}
			else
			{
				// ȣĪ ��� �˾�
				m_pDesignationChangeBG->SetEnable( TRUE);
			}
		}
		else
		{
			// ȣĪ ȹ��
			__OnDesignationGet();
		}
	}
}

void CStepProfile_Designation::__OnSelectDesignationChangeSlot( INT32 iSlot)
{
	if( m_pDesignationChange_Slot_Image_Lock[ iSlot]->isEnable())
		m_SelectedDesignationChangeSlotIdx = -1;
	else
	{
		m_SelectedDesignationChangeSlotIdx = iSlot;
		m_pNotifyBase->PlaySound( GUI_SND_ITEM_SELECTING);
	}	
}

void CStepProfile_Designation::GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify)
{
	if( pNotify->m_nID >= GCI_PRS_S_DESIGNATION_IMAGE_1 && pNotify->m_nID <= GCI_PRS_S_DESIGNATION_IMAGE_44)
	{
		switch( pNotify->m_nEvent)
		{
		case I3GUI_STATIC_NOTIFY_ONMOUSE :
			__OnMouseDesignationButton(	pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1);
			break;
		case I3GUI_STATIC_NOTIFY_CLICKED :
			__OnClickedDesignationButton( pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1);
			break;
		case I3GUI_STATIC_NOTIFY_DOUBLECLICK :
			if( m_bIsMyInfoView)
				__OnDoubleClickedDesignationButton( pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1);
			break;
		default :			break;
		}

		__UpdateDetailInfoBox();
		__UpdateGetInfoBox();
		__UpdateDesignationTree();
	}
	
	switch( pNotify->m_nID)
	{
	case GCI_PRS_B_GET_DESIGNATION:			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) __OnDesignationGet();		break;
	case GCI_PRS_B_GET_DESIGNATION_CHANGE:	if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) __OnDesignationChange();	break;
	case GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT1:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)
			__OnSelectDesignationChangeSlot( pNotify->m_nID - GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT1);
		else if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)
			__OnDesignationChange();
		break;
	case GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT2:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)
			__OnSelectDesignationChangeSlot( pNotify->m_nID - GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT1);
		else if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)
			__OnDesignationChange();
		break;
	case GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT3:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)
			__OnSelectDesignationChangeSlot( pNotify->m_nID - GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT1);
		else if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)
			__OnDesignationChange();
		break;

	case GCI_PRS_L_PROFILE_TARGET :
	case GCI_PRS_L_PROFILE_USERLIST :
		if( pNotify->m_nEvent == I3GUI_POPUPLIST_NOTIFY_CLICKED)	Event_ChangeUserInfo();
		break;

	case GCI_PRS_B_PROFILE_TARGET :
	case GCI_PRS_B_PROFILE_USERLIST :
	case GCI_PRS_B_PROFILE_USER_SLOT_1 :
	case GCI_PRS_B_PROFILE_USER_SLOT_2 :
	case GCI_PRS_B_PROFILE_USER_SLOT_3 :
	case GCI_PRS_B_PROFILE_USER_SLOT_4 :
	case GCI_PRS_B_PROFILE_USER_SLOT_5 :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		Event_ChangeUserInfo();
		break;
	}
}

void CStepProfile_Designation::Reset( void)
{
	m_DesignationMode = DESIGNATION_MODE_GET;
	m_SelectedDesignationIdx = -1;
	m_SelectedDesignationChangeSlotIdx = -1;
	m_pDesignationChangeBG->SetEnable( FALSE);

	__UpdateDetailInfoBox();
	__UpdateGetInfoBox();

	for( INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		m_pDesignationButton[ i]->SetAllShapeEnable( FALSE);
	}

	__UpdateDesignationTree();

	const CTitleInfo & titleInfo = g_pGameContext->GetTitleInfo();
	m_nCurDesignationSlotCount = titleInfo.GetLimitCount();
	RefreshGainedTitle(titleInfo);
}

void CStepProfile_Designation::__OnDesignationGet( void)
{
	switch( m_DesignationMode)
	{
	case DESIGNATION_MODE_GET:
		STRING_POPUP_Q(GAME_STRING("STR_TBL_DESIGNATION_GET_ANSWER_MESSAGE"), MAKE_CALLBACK(CbDesignationGet), this);/*������ ȣĪ�� ȹ���Ͻðڽ��ϱ�?*/
		break;
	case DESIGNATION_MODE_CHANGE:
		m_pDesignationChangeBG->SetEnable( TRUE);
		break;
	case DESIGNATION_MODE_RELEASE:
		STRING_POPUP_Q(GAME_STRING("STR_TBL_DESIGNATION_RELEASE_ANSWER_MESSAGE"), MAKE_CALLBACK(CbDesignationRelese), this);/*������ ȣĪ�� �����Ͻðڽ��ϱ�?*/
		break;
	default:
		break;
	}
}

void CStepProfile_Designation::__OnDesignationChange( void)
{
	if( m_SelectedDesignationChangeSlotIdx >= 0 && m_SelectedDesignationChangeSlotIdx < 3)
	{
		if( __CheckDesignationSameType())
		{
			STRING_POPUP( GAME_STRING("STR_TBL_DESIGNATION_FAIL_ANOTHER_USED"));/*�ٸ� ���Կ� ������ �迭�� ȣĪ�� ����ϰ� �־� ��뿡 �����Ͽ����ϴ�.*/
		}
		else
		{
			UINT8 DesignationType = (UINT8)g_pDesignationValueSet->getDesignationSet( m_SelectedDesignationIdx)->getType();

			__CheckUserTitlesBeret(m_SelectedDesignationChangeSlotIdx, DesignationType);

			//// 1.5 Check Designation
			//g_pGameContext->SetEvent( EVENT_DESIGNATION_CHANGE, &m_SelectedDesignationChangeSlotIdx, &DesignationType);
		}
	}
	else
	{
		STRING_POPUP(GAME_STRING("STR_TBL_DESIGNATION_NO_SELECTED_SLOT_MESSAGE"));/*������ �������� �ʾҽ��ϴ�. ������ ������ �� �ٽ� Ȯ�� ��ư�� �����ֽñ� �ٶ��ϴ�.*/
	}

	m_DesignationMode = DESIGNATION_MODE_GET;
	m_SelectedDesignationChangeSlotIdx = -1;

	m_pDesignationChangeBG->SetEnable( FALSE);
}

void CStepProfile_Designation::__OnDesignationRelease( void)
{
	UINT8 RemoveTitleSlotIndex = (UINT8)m_SelectedDesignationReleaseSlotIdx;

	// �����ϱ� ������ DesignationID ��0���� �־��ش�.
	__CheckUserTitlesBeret(RemoveTitleSlotIndex, 0);

	//// 1.5 Check Designation
	//g_pGameContext->SetEvent( EVENT_DESIGNATION_RELEASE, &RemoveTitleSlotIndex );
}

void	CStepProfile_Designation::__CheckUserTitlesBeret(INT32 SlotIndex, UINT8 DesignationID)
{
	// �����ϴ� ȣĪ�� ���� �����ϰ� �ִ� ������ �����ϱ� ����
	// �ʿ� ȣĪ�̶�� ���� �����ϰ� �ִ� ������ �����.
	USER_INFO_EQUIPMENT_CHARA	InfoEquipChara;
	g_pGameContext->GetMyUserInfoEquipmentChara(InfoEquipChara);
	UINT32 CurrentEquipBeretID = InfoEquipChara.GetItemNum();

	CTitleInfo titleInfoCopy = g_pGameContext->GetTitleInfo();
	titleInfoCopy.SetEquipTitle(SlotIndex, DesignationID);

	if ( !IsPossibleEquipUserTitleCharacter(CurrentEquipBeretID, titleInfoCopy) )
	{
		// �����ϰ� �ִ� ������ ���� ������ ȣĪ���δ� ���� ���� ������� ������
		// ������ �����.
		STRING_POPUP( GAME_STRING("STR_TBL_PROFILE_VELEMO_RELEASE"));/*����Ͻ� ������ ���� ��� �ڵ����� �����˴ϴ�.*/

		if( g_pFramework->EquipCharacterToMyInven(0, CHARACTER_CLASS_ITEM))
			m_pNotifyBase->PlaySound(GUI_SND_ITEM_EQUIP); 
	}
}

//EXPORT_AVALIABLE_CODE_VALUE
BOOL CStepProfile_Designation::__FindWeaponByDesignationEffect( DESIGNATION_SET * pSet, INT32 idx, WEAPON_CLASS_TYPE &classType, INT32 &weaponNum)
{
	BOOL	Rv = TRUE;
	switch( pSet->getAddEffect( idx))
	{
	case DESIGNATION_ID_GET_DUAL_KNIFE :		classType = WEAPON_CLASS_DUALKNIFE;			weaponNum = GetItemIndex("DUALKNIFE_M9");				break;
	case DESIGNATION_ID_GET_DUAL_HANDGUN :		classType = WEAPON_CLASS_DUALHANDGUN;		weaponNum = GetItemIndex("DUALHANDGUN_MK23_K5");			break;
	case DESIGNATION_ID_GET_870MCS_W :			classType = WEAPON_CLASS_SHOTGUN;			weaponNum = GetItemIndex("SHOTGUN_870MCS_W");			break;
	case DESIGNATION_ID_GET_SG550_S :			classType = WEAPON_CLASS_ASSAULT;			weaponNum = GetItemIndex("ASSAULT_SG550_SE");			break;
	case DESIGNATION_ID_GET_DRAGUNOV_G :		classType = WEAPON_CLASS_SNIPER;			weaponNum = GetItemIndex("SNIPER_DRAGUNOV_GOLD");		break;
	case DESIGNATION_ID_GET_SPECTRE_W :			classType = WEAPON_CLASS_SMG;				weaponNum = GetItemIndex("SMG_SPECTRE_SE");				break;
	case DESIGNATION_ID_GET_C5 :				classType = WEAPON_CLASS_THROWING1;			weaponNum = GetItemIndex("THROWING_C5");					break;
	case DESIGNATION_ID_GET_G36C_EXT :			classType = WEAPON_CLASS_ASSAULT;			weaponNum = GetItemIndex("ASSAULT_G36C_SCOPE2");			break;
	case DESIGNATION_ID_GET_L115A1:				classType = WEAPON_CLASS_SNIPER;			weaponNum = GetItemIndex("SNIPER_L115A1");				break;
	case DESIGNATION_ID_GET_CIC :				classType = WEAPON_CLASS_CIC;				weaponNum = GetItemIndex("CIC_P99");						break;
	case DESIGNATION_ID_GET_P90_EXT :			classType = WEAPON_CLASS_SMG;				weaponNum = GetItemIndex("SMG_P90_DOTSIGHT");			break;
	case DESIGNATION_ID_GET_SPAS15 :			classType = WEAPON_CLASS_SHOTGUN;			weaponNum = GetItemIndex("SHOTGUN_SPAS15");				break;
	case DESIGNATION_ID_GET_PSG1_S :			classType = WEAPON_CLASS_SNIPER;			weaponNum = GetItemIndex("SNIPER_PSG1_S");				break;
	case DESIGNATION_ID_GET_MP5K_G :			classType = WEAPON_CLASS_SMG;				weaponNum = GetItemIndex("SMG_MP5K_SERIF");				break;
	case DESIGNATION_ID_GET_AMOK_KUKRI :		classType = WEAPON_CLASS_KNIFE;				weaponNum = GetItemIndex("KNIFE_KUKRII");				break;
	case DESIGNATION_ID_GET_WP_SMOKE :			classType = WEAPON_CLASS_THROWING1;			weaponNum = GetItemIndex("THROWING_WP_SMOKE");			break;
	case DESIGNATION_ID_GET_KRISS_SUPER_V :		classType = WEAPON_CLASS_SMG;				weaponNum = GetItemIndex("SMG_KRISSSUPERV");				break;
	case DESIGNATION_ID_GET_DUAL_DESERTEAGLE :	classType = WEAPON_CLASS_DUALHANDGUN;		weaponNum = GetItemIndex("DUALHANDGUN_DESERTEAGLE");		break;
	case DESIGNATION_ID_GET_AK_SOPMOD :			classType = WEAPON_CLASS_ASSAULT;			weaponNum = GetItemIndex("ASSAULT_AK_SOPMOD");			break;
	case DESIGNATION_ID_GET_AUG_A3 :			classType = WEAPON_CLASS_ASSAULT;			weaponNum = GetItemIndex("ASSAULT_AUG_A3");				break;
	case DESIGNATION_ID_GET_M1887W :			classType = WEAPON_CLASS_SHOTGUN;			weaponNum = GetItemIndex("SHOTGUN_M1887_S");				break;
	case DESIGNATION_ID_GET_MINI_AXE :			classType = WEAPON_CLASS_KNIFE;				weaponNum = GetItemIndex("KNIFE_MINIAXE_DUMMY");				break;

	case DESIGNATION_ID_GET_WEAPON:		// 		���� ���� �ϵ��ڵ��� �ʿ���� ���� ȣĪ ���� ���ο� ��� komet
		{
			classType = (WEAPON_CLASS_TYPE) pSet->getAuthWeaponClass( idx);
			weaponNum = (INT32) pSet->getAuthWeaponNumber( idx);

			// KEYWORD : WEAPON_MINIAXE	�Ϲ� �������� �ٸ� ���⸦ ��� �ִ� ����Դϴ�. �ش� ���Ⱑ ǥ�õǵ��� ��������� �մϴ�.
			GUI_WEAPON_VALID( classType, weaponNum);
		}
		break;

	default :
		classType = WEAPON_CLASS_UNKNOWN;
		weaponNum = 0;
		Rv = FALSE;
		break;
	}

	return Rv;
}

//EXPORT_AVALIABLE_CODE_VALUE
BOOL	CStepProfile_Designation::__GetPrizeWeaponInfoText(char * pText, const size_t sizeTextMax, WEAPON_CLASS_TYPE classType, INT32 weaponNum)
{
#if defined (LOCALE_THAILAND) || defined (LOCALE_RUSSIA)
	// ���� ����.
	return FALSE;
#else

	char szWeaponName[128];

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, weaponNum);
	if( pWeaponInfo != NULL)
	{
		pWeaponInfo->GetWeaponInfoName( szWeaponName);
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// �ֽ� �ѱ� ���忡�� �ؿܿ� ���������� ȣĪ ���� 10���� �����ϱ�� ����
//#if defined (LOCALE_KOREA)
//		sprintf_s( pText, sizeTextMax, "- [%s] %d%s\n", szWeaponName, 3, GAME_STRING("STBL_IDX_SHOP_USE_DAY")); /* - [�����] 3��*/
//#else
		sprintf_s( pText, sizeTextMax, "- [%s] %d%s\n", szWeaponName, 10, GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT")); /* - [�����] 10Unit*/
//#endif
		return TRUE;
	}
	return FALSE;

#endif
}

void CStepProfile_Designation::__OpenGainWindowForDesignationWhenGetUserTitle( INT32 nGetGainUserTitleIdx)
{	// ȣĪ ȹ��� ���� �� �˸�â	
	const INT32 nGainItemCount = 8;
	UINT32 nGainItemID[nGainItemCount];
	i3mem::FillZero(nGainItemID, sizeof(nGainItemID));

	// �ش� ȣĪ�� ȣĪ ���⸦ �����´�.
	INT32 cnt = GetItemIDByDesignation( (UINT8) nGetGainUserTitleIdx, nGainItemID, nGainItemCount);

	if( cnt > 0)
	{
		// ü�蹫�⸦ �����ϴ� ���
		char szText[ 1024];
		char szGainItemName[ 1024];
		i3String::Format( szText, 1024, "%s\n", GAME_STRING("STR_TBL_NETWORK_SUCESS_GET_DESIGNATION_WITH_PRIZE") );/*ȣĪ ȹ�濡 �����Ͽ� ������ ���޵Ǿ����ϴ�.\n*/

		for( INT32 i=0; i<cnt; i++)
		{
			switch( GET_ITEM_FLAG_TYPE( nGainItemID[i]))
			{
			case ITEM_TYPE_PRIMARY:
			case ITEM_TYPE_SECONDARY:
			case ITEM_TYPE_MELEE:
			case ITEM_TYPE_THROWING1:
			case ITEM_TYPE_THROWING2:
				{
					WEAPON_CLASS_TYPE weaponType = (WEAPON_CLASS_TYPE) GET_ITEM_FLAG_CLASS( nGainItemID[i]);
					INT32 weaponNum = GET_ITEM_FLAG_NUMBER( nGainItemID[i]);

					// KEYWORD : WEAPON_MINIAXE	�Ϲ� �������� �ٸ� ���⸦ ��� �ִ� ����Դϴ�. �ش� ���Ⱑ ǥ�õǵ��� ��������� �մϴ�.
					GUI_WEAPON_VALID( weaponType, weaponNum);

					if( __GetPrizeWeaponInfoText(szGainItemName, 1024, weaponType, weaponNum))
					{
						StringCchCat(szText, 1024, szGainItemName);
					}
				}
				break;
			case ITEM_TYPE_CHARA:
			case ITEM_TYPE_HEADGEAR:
				{
					g_pCharaInfoDataBase->GetCharacterName(nGainItemID[i], szGainItemName, 1024);

					StringCchCat(szText, 1024, szGainItemName);
				}
				break;
			case ITEM_TYPE_MAINTENANCE:
			case ITEM_TYPE_EXPENDABLE:
				{
					g_pShopItemInfoDataBase->GetItemName(nGainItemID[i], szGainItemName, 1024);

					StringCchCat(szText, 1024, szGainItemName);
				}
			}					
		}				

		StringCchCat( szText, 1024, GAME_STRING("STR_POPUP_NOTICE_DESIGNATION_GET_WITH_ITEM"));	/*(�κ��丮���� Ȯ���Ͻ� �� �ֽ��ϴ�.)*/
		STRING_POPUP( szText);
	}
	else
	{
		STRING_POPUP( GAME_STRING("STR_TBL_NETWORK_SUCESS_GET_DESIGNATION"));/*ȣĪ ȹ�濡 �����Ͽ����ϴ�.*/
	}
}

void CStepProfile_Designation::__OnGameEvent_DesingationGet( INT32 result)
{
	switch( result )
	{
	case EVENT_ERROR_SUCCESS:
		{
			CTitleInfo & titleInfo = g_pGameContext->SetTitleInfo();
			INT32 GainUserTitleIdx = g_pGameContext->getGainUserTitleIdx();

			titleInfo.SetGainedTitle(GainUserTitleIdx, 1);

			// ȣĪ ȹ��� ���� �� �˸�â
			__OpenGainWindowForDesignationWhenGetUserTitle( GainUserTitleIdx);
		}
		break;
	case EVENT_ERROR_USER_TITLE_REQUIREMENT_FAIL:
		STRING_POPUP( GAME_STRING("STR_TBL_NETWORK_NOT_MATCH_BUY_DESIGNATION"));/*ȣĪ ��ȯ ������ �������� �ʽ��ϴ�.*/
		break;
	case EVENT_ERROR_USER_TITLE_ALREADY_GAIN_FAIL :
		STRING_POPUP( GAME_STRING("STR_TBL_NETWORK_ALREADY_GET_DESIGNATION"));/*�̹� ȹ���� ȣĪ�Դϴ�.*/
		break;
	}
}


void CStepProfile_Designation::__CheckDesignationSlotGet(void)
{
	if( ! m_bIsMyInfoView)
		return;


	const CTitleInfo & titleInfo = g_pGameContext->GetTitleInfo(m_bIsMyInfoView);
	INT32 iLimitTitleCount = titleInfo.GetLimitCount();
	// �߰������� ȹ���Ͽ��ٸ�~!!
	if( m_nCurDesignationSlotCount == iLimitTitleCount)
		return;

	m_nCurDesignationSlotCount = iLimitTitleCount;

	char szTemp[256];
	sprintf_s( szTemp, GAME_STRING("STR_TBL_DESIGNATION_GET_SLOT_MESSAGE"), m_nCurDesignationSlotCount);/*���ϵ帳�ϴ�!!\n \nȣĪ ������ ȹ���Ͽ� ȣĪ�� ���ÿ�\n����� �� �ִ� ������ �����Ͽ����ϴ�.\n(�ִ� %d���� ȣĪ ��� ����)*/
	STRING_POPUP( szTemp);
}

BOOL CStepProfile_Designation::__CheckDesignationEquip(void)
{
	const CTitleInfo & MyTitleInfo = g_pGameContext->GetTitleInfo();
	DESIGNATION_TYPE Title = g_pDesignationValueSet->getDesignationSet( m_SelectedDesignationIdx)->getType();

	INT32 TitleIndex = MyTitleInfo.HaveTitle((UINT8)Title);
	if( -1 != TitleIndex)
	{
		m_SelectedDesignationReleaseSlotIdx = TitleIndex;
		return TRUE;
	}

	return FALSE;
}

BOOL CStepProfile_Designation::__CheckDesignationSameType(void)
{
	const CTitleInfo & MyTitleInfo = g_pGameContext->GetTitleInfo();

	for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		if( m_SelectedDesignationChangeSlotIdx == i)
			continue;

		INT32 x = __FindSameDesignationIndexByType( (DESIGNATION_TYPE) MyTitleInfo.GetEquipTitle(i));
		if( x == -1)
			continue;

#if 0
		// ȣĪ ���Կ� ���� �迭�� �����Ϸ��� �ּ��� Ǯ�� �˴ϴ�.
		if( g_pDesignationValueSet->getDesignationSet( x)->_ApplyType == DESIGNATION_APPLY_TYPE_ALL)
			continue;

#endif
		if( g_pDesignationValueSet->getDesignationSet( x)->getApplyType() == g_pDesignationValueSet->getDesignationSet( m_SelectedDesignationIdx)->getApplyType())
		{
			// ���ϰ迭 �����
			return TRUE;
		}

	}

	return FALSE;
}

void CStepProfile_Designation::CbDesignationGet( void* pThis, INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		//// 1.5 Check Designation
		//g_pGameContext->SetEvent( EVENT_DESIGNATION_GET, &( ((CStepProfile_Designation*)pThis)->m_nCurShowingDesignation) );
	}
}

void CStepProfile_Designation::CbDesignationRelese( void* pThis, INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		((CStepProfile_Designation*)pThis)->__OnDesignationRelease();
	}
}

void CStepProfile_Designation::__UpdateDetailInfoBox( void)
{
	INT32 i;

	DESIGNATION_ID DesignationEffectidx = DESIGNATION_ID_NONE;
	char szTemp[MAX_STRING_COUNT] = {0,};
	char szName[ MAX_STRING_COUNT];
	char szName_E[ MAX_STRING_COUNT];
	
	if( (m_SelectedDesignationIdx != -1) && (m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT))
	{
		DESIGNATION_SET * pSelectedInfo = g_pDesignationValueSet->getDesignationSet( m_SelectedDesignationIdx);

		m_pDesignationSelectedImage->SetShapeEnable( m_SelectedDesignationIdx, TRUE);			
		pSelectedInfo->getExplain( szTemp);
		m_pDesignationInfoText->SetText( szTemp);


		m_nCurShowingDesignation = pSelectedInfo->getType();
		pSelectedInfo->getName( szName);
		pSelectedInfo->getName_E( szName_E);

		sprintf_s( szTemp, "{col:80,150,215,255}%s{/col}\n{col:204,204,204,255}%s{/col}", szName, szName_E );
		m_pDesignationNameText->SetText(szTemp);

		// Plus Effect
		for( i=0; i<2; i++)
		{
			DesignationEffectidx = (DESIGNATION_ID) pSelectedInfo->getPlusEffect( i);

			if( DesignationEffectidx != DESIGNATION_ID_NONE)
			{
				char text[256] = {};
				g_pDesignationValueSet->getUserTitleEffectText( DesignationEffectidx, text);

				if(i==0)	sprintf_s( szTemp, "{col:80,150,215,255}%s{/col}", text);
				else		sprintf_s( szTemp, "%s\n{col:80,150,215,255}%s{/col}", szTemp, text);
			}
			else
			{
				if(i==0)	sprintf_s( szTemp, GAME_STRING("STR_TBL_DESIGNATION_NONE_WITH_COLOR1"));/*{col:80,150,215,255}����{/col}*/
			}
		}
		m_pDesignationPlusEffectText->SetText( szTemp);

		// Minus Effect
		szTemp[ 0] = 0;
		for( i=0; i<2; i++)
		{
			DesignationEffectidx = (DESIGNATION_ID) pSelectedInfo->getMinusEffect( i);

			if( DesignationEffectidx != DESIGNATION_ID_NONE)
			{
				char text[256] = {};
				g_pDesignationValueSet->getUserTitleEffectText( DesignationEffectidx, text);

				if(i==0)	sprintf_s( szTemp, "{col:205,100,50,255}%s{/col}", text);
				else		sprintf_s( szTemp, "%s\n{col:205,100,50,255}%s{/col}", szTemp, text);
			}
			else
			{
				if(i==0)	sprintf_s( szTemp, GAME_STRING("STR_TBL_DESIGNATION_NONE_WITH_COLOR2"));/*{col:205,100,50,255}����{/col}*/
			}
		}
		m_pDesignationMinusEffectText->SetText( szTemp);

		const INT32 slotMax = 2;

		// Add Effect
		BOOL	UseSlotEffect[ slotMax];
		szTemp[ 0] = 0;
		for( i=0; i<slotMax; i++)
		{	
			char szAddEffect[256] = "INVALID";
			char szWeaponName[256] = "INVALID";

			UseSlotEffect[i] = FALSE;

			DesignationEffectidx = (DESIGNATION_ID) pSelectedInfo->getAddEffect( i);

			if( DesignationEffectidx == DESIGNATION_ID_NONE || IsWeaponByDesignationEffect( DesignationEffectidx)) 
			{
				UINT32 nItemID[slotMax];

				i3mem::FillZero( nItemID, sizeof( nItemID));

				// ȣĪ ������ ���	(�ش� ȣĪ�� �������� ������ ȣĪ ���Ⱑ �ִ� üũ)
				INT32 cnt = GetItemIDByDesignation( (UINT8) m_nCurShowingDesignation, nItemID, slotMax);
			
				if( cnt > 0 && nItemID[i] > 0)		// ȣĪ�� ���Ⱑ �ִ�.
				{
					UINT32 id = nItemID[i];

					UseSlotEffect[i] = TRUE;

					WEAPON_CLASS_TYPE type = (WEAPON_CLASS_TYPE) GET_ITEM_FLAG_CLASS( id);
					INT32 number = GET_ITEM_FLAG_NUMBER( id);

					// KEYWORD : WEAPON_MINIAXE	�Ϲ� �������� �ٸ� ���⸦ ��� �ִ� ����Դϴ�. �ش� ���Ⱑ ǥ�õǵ��� ��������� �մϴ�.
					GUI_WEAPON_VALID( type, number);

					CWeaponInfo * pWeaponInfo =  g_pWeaponInfoDataBase->getWeaponInfo(type, number);

					if( pWeaponInfo)
					{	
						pWeaponInfo->GetWeaponInfoName( szWeaponName);
						i3String::Format( szAddEffect, 256, GAME_STRING("STR_HEADER_AUTHORITY_WEAPON"),szWeaponName );
					}
					else
					{
						char msg[256] = "";
						sprintf_s( msg, "[__UpdateDetailInfoBox] (Designation: %d) Could not find weapon (class %d, num %d) in Weapon.pef\n", m_SelectedDesignationIdx, type, number); 
						I3WARN( msg);
					}
				}
			}
			else
			{
				// ȣĪ ����� ���
				char text[256] = {};
				g_pDesignationValueSet->getUserTitleEffectText( DesignationEffectidx, text);
				i3String::Copy( szAddEffect, text, 256 );

				UseSlotEffect[i] = TRUE;
			}

			if(i==0 && UseSlotEffect[i] == TRUE)	
			{	//	ù��° ȣĪ ȿ��
				sprintf_s( szTemp, "{col:204,204,204,255}%s{/col}", szAddEffect);
			}
			else if(i==1 && UseSlotEffect[i] == TRUE)			
			{	//	�ι�° ȣĪ ȿ��
				sprintf_s( szTemp, "%s\n{col:204,204,204,255}%s{/col}", szTemp, szAddEffect);
			}	
		}	

		// ȣĪ ȿ�� ����
		if(UseSlotEffect[0] == FALSE && UseSlotEffect[1] == FALSE)
		{
			sprintf_s( szTemp, GAME_STRING("STR_TBL_DESIGNATION_NONE_WITH_COLOR3"));/*{col:204,204,204,255}����{/col}*/	
		}	

		m_pDesignationAddEffectText->SetText( szTemp);
	}
	else
	{
		m_pDesignationNameText->SetText("");
		m_pDesignationInfoText->SetText("");
		m_pDesignationPlusEffectText->SetText("");
		m_pDesignationMinusEffectText->SetText("");
		m_pDesignationAddEffectText->SetText("");
		m_nCurShowingDesignation = DESIGNATION_TYPE_NONE;
	}
}

void CStepProfile_Designation::__UpdateGetInfoBox_GainedBadge( void)
{
	char szCount[ MAX_STRING_COUNT];

	// ����
	UINT32 ribbonCount = *g_pGameContext->getMiniature();
	sprintf_s( szCount, GAME_STRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), ribbonCount);
	m_tBadgeNeed[ BADGE_TYPE_RIBBON]._pBadgeAppendixCount->SetText( szCount);

	// ����
	UINT32 InsigniaCount = *g_pGameContext->getInsignia();
	sprintf_s( szCount, GAME_STRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), InsigniaCount);
	m_tBadgeNeed[ BADGE_TYPE_ENSIGN]._pBadgeAppendixCount->SetText( szCount);

	// ����
	UINT32 orderCount = *g_pGameContext->getOrder();
	sprintf_s( szCount, GAME_STRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), orderCount);
	m_tBadgeNeed[ BADGE_TYPE_MEDAL]._pBadgeAppendixCount->SetText( szCount);

	// ������ ����
	UINT32 masterCount = *g_pGameContext->getMaster();
	sprintf_s( szCount, GAME_STRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), masterCount);
	m_tBadgeNeed[ BADGE_TYPE_MASTER]._pBadgeAppendixCount->SetText( szCount);
}

void CStepProfile_Designation::__UpdateGetInfoBox( void)
{	
	INT32 nType = 0;

	char szCount[MAX_STRING_COUNT] = {0,};
	__UpdateGetInfoBox_GainedBadge();

	if( m_SelectedDesignationIdx != -1)
	{
		if( m_bIsMyInfoView)
		{
			// ȣĪ ȹ��/���/����
			m_pDesignationGetButton->SetEnable( TRUE);
			if( !m_bGetDesignation[m_SelectedDesignationIdx])
			{
				m_DesignationMode = DESIGNATION_MODE_GET;
				m_pDesignationGetButton->SetCaption(GAME_STRING("STR_TBL_DESIGNATION_GET_TEXT"));/*ȣĪ ȹ��*/
			}
			else if( m_bGetDesignation[m_SelectedDesignationIdx])
			{
				if( __CheckDesignationEquip())
				{
					m_DesignationMode = DESIGNATION_MODE_RELEASE;
					m_pDesignationGetButton->SetCaption(GAME_STRING("STR_TBL_DESIGNATION_RELEASE_TEXT"));/*ȣĪ ����*/
				}
				else
				{
					m_DesignationMode = DESIGNATION_MODE_CHANGE;
					m_pDesignationGetButton->SetCaption(GAME_STRING("STR_TBL_DESIGNATION_USE_TEXT"));/*ȣĪ ���*/
				}
			}
		}
		else
		{
			m_pDesignationGetButton->SetEnable( FALSE);
		}

		if( m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
		{
			nType = g_pDesignationValueSet->getDesignationSet( m_SelectedDesignationIdx)->getType();

			// 1.�ʿ� ǥ�� ����
			__UpdateRequirementBadge( nType);

			// 2. �ʿ� ȣĪ ����
			__UpdateRequirementDesignation( nType);

			const char * RankText = GetCharaRank(g_UserTitleRequirement[nType]._rank);
			// 3. �ʿ� ��� ����
			m_pDesignationNeedRankText->SetTextEllipsis(RankText);
		}
	}
	else
	{
		INT32 i;
		for( i = 0; i < BADGE_TYPE_COUNT; i++)
		{
			sprintf_s( szCount, GAME_STRING("STR_TBL_DESIGNATION_NEEDED_BADGECOUNT_TEXT"), 0);
			m_tBadgeNeed[ i]._pBadgeNeedCount->SetText( szCount);
		}

		m_pDesignationNeedText->SetText("{col:204, 204, 204, 255}----{/col}");
		m_pDesignationNeedText2->SetText("{col:204, 204, 204, 255}----{/col}");
		m_pDesignationNeedRankText->SetText("{col:204, 204, 204, 255}----{/col}");

		m_pDesignationGetButton->SetEnable( FALSE);
	}
}

void CStepProfile_Designation::__UpdateChangePopup( REAL32 rDeltaSeconds)
{
	char szTemp[ MAX_STRING_COUNT];
	const CTitleInfo & titleInfo = g_pGameContext->GetTitleInfo();

	I3ASSERT( titleInfo.GetLimitCount() <= MAX_EQUIP_USER_TITLE);

	for( INT32 i = 0 ; i < titleInfo.GetLimitCount() ; i++)
	{
		m_pDesignationChange_Slot_Image_Lock[i]->SetEnable(FALSE);
	}

	const CTitleInfo & MyTitleInfo = g_pGameContext->GetTitleInfo();
	
	// ȣĪ�̹��� �� ȣĪ�̸�.
	for( INT32 i = 0 ; i < MAX_EQUIP_USER_TITLE ; i++)
	{	
		if( !m_pDesignationChange_Slot_Image_Lock[ i]->isEnable())
		{
			UINT8 EeuipTitle = MyTitleInfo.GetEquipTitle(i);
			if( EeuipTitle <= MAX_DEFAULT_USER_TITLE_COUNT)
			{
				if( EeuipTitle != 0)
				{
					INT32 x = __FindSameDesignationIndexByType( (DESIGNATION_TYPE) EeuipTitle);
					if( x != -1)
					{
						m_pDesignationChange_Slot_Image[i]->SetAllShapeEnable( FALSE);
						m_pDesignationChange_Slot_Image[i]->SetShapeEnable( x, TRUE);
						g_pDesignationValueSet->getDesignationSet( x)->getName( szTemp);
						m_pDesignationChange_Slot_Text[i]->SetTextEllipsis( szTemp);
					}
					else
					{	
						m_pDesignationChange_Slot_Image[i]->SetAllShapeEnable( FALSE);
						m_pDesignationChange_Slot_Text[i]->SetText("");
					}		
				}
				else
				{
					m_pDesignationChange_Slot_Image[i]->SetAllShapeEnable( FALSE);
					m_pDesignationChange_Slot_Text[i]->SetText("");
				}
			}
			else
			{
				I3WARN( "invalid user title. %d", EeuipTitle);
			}
		}
	}

	// Slot ���� Ȱ��ȭ
	switch( m_SelectedDesignationChangeSlotIdx)
	{
	case 0:		
		m_pDesignationChange_Slot1->SetColor( I3GUI_CONTROL_STATE_NORMAL, 253,167,0,255);
		m_pDesignationChange_Slot2->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255,255,255,255);
		m_pDesignationChange_Slot3->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255,255,255,255);
		break;
	case 1:
		m_pDesignationChange_Slot1->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255,255,255,255);
		m_pDesignationChange_Slot2->SetColor( I3GUI_CONTROL_STATE_NORMAL, 253,167,0,255);
		m_pDesignationChange_Slot3->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255,255,255,255);
		break;
	case 2:
		m_pDesignationChange_Slot1->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255,255,255,255);
		m_pDesignationChange_Slot2->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255,255,255,255);
		m_pDesignationChange_Slot3->SetColor( I3GUI_CONTROL_STATE_NORMAL, 253,167,0,255);
		break;
	default:
		m_pDesignationChange_Slot1->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255,255,255,255);
		m_pDesignationChange_Slot2->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255,255,255,255);
		m_pDesignationChange_Slot3->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255,255,255,255);
		break;
	}
}

void CStepProfile_Designation::__UpdateDesignationTree( void)
{

	m_pDesignationSelectedImage->SetAllShapeEnable( FALSE);
	
	if( (m_SelectedDesignationIdx != -1) && (m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT) )
	{
		m_pDesignationButton[m_SelectedDesignationIdx]->SetShapeEnable( 2, TRUE);				
		m_pDesignationSelectedImage->SetShapeEnable( m_SelectedDesignationIdx, TRUE);
	}

	for( INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		m_pDesignationButton[ i]->SetShapeEnable( 3, FALSE);
	}

	// ȹ���� ȣĪ�� ���Ͽ��� �׻� Ư���� ǥ�ø� �մϴ�.
	const CTitleInfo & titleInfo = g_pGameContext->GetTitleInfo(m_bIsMyInfoView);

	for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		UINT8 Title = titleInfo.GetEquipTitle(i);
		I3ASSERT( Title <= MAX_DEFAULT_USER_TITLE_COUNT);

		if( Title != 0)
		{
			INT32 j = __FindSameDesignationIndexByType( (DESIGNATION_TYPE) Title);
			if( j != -1)
				m_pDesignationButton[ j]->SetShapeEnable( 3, TRUE);
		}
	}
}

void CStepProfile_Designation::OnGameEvent( INT32 event, INT32 arg)
{
	switch( event)
	{
	//// 1.5 Check Designation
	//case EVENT_DESIGNATION_GET :
	//	// ȹ�� ȣĪ
	//	__OnGameEvent_DesingationGet( arg);
	//	Event_ChangeUserInfo();
	//	break;
	//case EVENT_DESIGNATION_CHANGE:
	//case EVENT_DESIGNATION_RELEASE :
	case EVENT_GET_USERINFO :
		Event_ChangeUserInfo();
		break;
	}
}

void CStepProfile_Designation::Event_ChangeUserInfo( void)
{
#if defined( DESIGZATION_ENABLE)
	__CheckDesignationSlotGet();
#endif
	const CTitleInfo & titleInfo = g_pGameContext->GetTitleInfo();
	RefreshGainedTitle(titleInfo);
	__UpdateDetailInfoBox();
	__UpdateGetInfoBox();
	__UpdateDesignationTree();
}

INT32 CStepProfile_Designation::__FindSameDesignationIndexByType( DESIGNATION_TYPE iTitleType)
{
	INT32 i;

	for( i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		if( g_pDesignationValueSet->getDesignationSet( i)->getType() == iTitleType)
			return i;
	}

	return -1;
}

void CStepProfile_Designation::__UpdateRequirementBadge( INT32 nType)
{
	INT32 i;
	char szCount[ MAX_STRING_COUNT];

	INT32 iCount[ BADGE_TYPE_COUNT] = {0,};

	for( i = 0; i < USER_TITLE_REQUIREMENT_COUNT; i++)
	{
		switch( g_UserTitleRequirement[nType]._ribbon[i]._type)
		{
		case UTR_RIBBON:	iCount[ 0] += g_UserTitleRequirement[nType]._ribbon[i]._count;	break;
		case UTR_ENSIGN:	iCount[ 1] += g_UserTitleRequirement[nType]._ribbon[i]._count;	break;
		case UTR_MEDAL:		iCount[ 2] += g_UserTitleRequirement[nType]._ribbon[i]._count;	break;
		case UTR_MASTER:	iCount[ 3] += g_UserTitleRequirement[nType]._ribbon[i]._count;	break;
		case UTR_NOTHING:
			break;
		}

	}

	for( i = 0; i < BADGE_TYPE_COUNT; i++)
	{
		sprintf_s( szCount, GAME_STRING("STR_TBL_DESIGNATION_NEEDED_BADGECOUNT_TEXT"), iCount[ i]);
		m_tBadgeNeed[ i]._pBadgeNeedCount->SetText( szCount);
	}
}

void CStepProfile_Designation::__UpdateRequirementDesignation( INT32 nType)
{
	INT32 i;
	char szTemp[ MAX_STRING_COUNT];

	m_pDesignationNeedText->SetTextEllipsis("----");
	m_pDesignationNeedText2->SetTextEllipsis("----");

	for( i = 0; i < 2; i++)
	{
		if( g_UserTitleRequirement[nType]._userTitleId[i] != 0)
		{
			INT32 x = __FindSameDesignationIndexByType( (DESIGNATION_TYPE) g_UserTitleRequirement[nType]._userTitleId[i]);
			if( x != -1)
			{
				g_pDesignationValueSet->getDesignationSet( x)->getName( szTemp);

				switch( i)
				{
				case 0:	m_pDesignationNeedText->SetTextEllipsis( szTemp);	break;
				case 1:	m_pDesignationNeedText2->SetTextEllipsis( szTemp);	break;
				}
			}
		}
	}
}

void CStepProfile_Designation::RefreshGainedTitle( const CTitleInfo &titleInfo )
{
	for( INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		DESIGNATION_TYPE Type = g_pDesignationValueSet->getDesignationSet(i)->getType();
		if( titleInfo.GetGainedTitle(Type) == 1)
		{
			// ȹ���� ȣĪ.
			m_bGetDesignation[i] = TRUE;
			m_pDesignationImage[i]->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 255);
		}
		else
		{
			// ��ȹ��.
			m_bGetDesignation[i] = FALSE;
			m_pDesignationImage[i]->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 77);
		}
	}
}




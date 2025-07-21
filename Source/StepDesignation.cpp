#include "pch.h"
#include "GlobalVariables.h"
#include "QuestTextDictionary.h"
#include "StepProfile.h"
#include "StageReady.h"

void CStepProfile::_InitDesignation(void)
{
	// ������ ȣĪ������ ���Ͽ� �ʱ�ȭ�մϴ�.
	_InitDesignationInfo();

#ifndef DESIGZATION_ENABLE
	m_pDesignationTab->SetEnable( FALSE);
#endif

	// �ʱ⿡ ȣĪ �ý����� ��Ȱ��ȭ �Ǿ��ֽ��ϴ�.
	m_pDesignationSect->SetEnable( FALSE);

	// DesignationDeatail
	m_pDesignationNameText->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationNameText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationInfoText->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationInfoText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationApplyTypeText->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationApplyTypeText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationPlusEffectText->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, 8);
	m_pDesignationPlusEffectText->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationPlusEffectText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pDesignationMinusEffectText->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, 8);
	m_pDesignationMinusEffectText->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationMinusEffectText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pDesignationAddEffectText->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, 8);
	m_pDesignationAddEffectText->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationAddEffectText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);

	// DesignationGetInfo
	m_pDesignationRibonNeedText1->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationRibonNeedText1->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationRibonNeedText2->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationRibonNeedText2->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationRibonNeedText3->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationRibonNeedText3->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationRibonNeedText4->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationRibonNeedText4->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationRibonNeedCount1->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationRibonNeedCount1->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationRibonNeedCount2->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationRibonNeedCount2->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationRibonNeedCount3->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationRibonNeedCount3->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationRibonNeedCount4->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationRibonNeedCount4->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedText->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationNeedText->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedText2->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationNeedText2->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedRankText->SetTextColor(GetColor(GCT_STATIC));
	m_pDesignationNeedRankText->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	// My info
	m_pDesignationMyInfoNick->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationMyInfoNick->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationMyInfo_Slot_Text[0]->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationMyInfo_Slot_Text[0]->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationMyInfo_Slot_Text[1]->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationMyInfo_Slot_Text[1]->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationMyInfo_Slot_Text[2]->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationMyInfo_Slot_Text[2]->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	// Change Popup
	m_pDesignationChange_Slot_Text[0]->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationChange_Slot_Text[0]->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationChange_Slot_Text[1]->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationChange_Slot_Text[1]->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationChange_Slot_Text[2]->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationChange_Slot_Text[2]->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	// Common
	m_pDesignationTreeCategory->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationTreeCategory->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationTreeCategory->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_TREE));/*{col:153,153,153,255}ȣĪ ���� Ʈ��{/col}*/
	m_pDesignationTreeEventCategory->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationTreeEventCategory->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationTreeEventCategory->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EVENT_TEXT));/*{col:153,153,153,255}�̺�Ʈ ȣĪ{/col}*/
	m_pDesignationDeatailCategory->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationDeatailCategory->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationDeatailCategory->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_GET_DETAIL_INFO));/*{col:153,153,153,255}ȣĪ �� ����{/col}*/
	m_pDesignationGetInfoCategory->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationGetInfoCategory->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationGetInfoCategory->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_GET_INFO));/*{col:153,153,153,255}ȣĪ ȹ�� ����{/col}*/
	m_pDesignationApplyTypeKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationApplyTypeKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationApplyTypeKey->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_APPLY_TYPE));/*{col:153,153,153,255}���� �迭:{/col}*/
	m_pDesignationPlusEffectKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationPlusEffectKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationPlusEffectKey->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_PLUS_EFFECT));/*{col:153,153,153,255}+ ȿ��   :{/col}*/
	m_pDesignationMinusEffectKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationMinusEffectKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationMinusEffectKey->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_MINUS_EFFECT));/*{col:153,153,153,255}- ȿ��   :{/col}*/
	m_pDesignationAddEffectKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationAddEffectKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationAddEffectKey->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_ADD_EFFECT));/*{col:153,153,153,255}�ΰ� ȿ��:{/col}*/
	m_pDesignationNeedRibonKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationNeedRibonKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedRibonKey->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NEEDED_MEDAL_TEXT));/*{col:153,153,153,255}�ʿ� ǥ��{/col}*/
	m_pDesignationNeedDesignationKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationNeedDesignationKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedDesignationKey->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NEEDED_TEXT));/*{col:153,153,153,255}�ʿ� ȣĪ{/col}*/
	m_pDesignationNeedRankKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationNeedRankKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedRankKey->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NEEDED_GRADE_TEXT));/*{col:153,153,153,255}�ʿ� ���{/col}*/
	m_pDesignationMyInfoBG_Text->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationMyInfoBG_Text->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationMyInfoBG_Text->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_MYINFO_TEXT));/*������*/
	m_pDesignationChangeText->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationChangeText->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationChangeText->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_USE_TEXT));/*ȣĪ ���*/

	m_pDesignationMyInof_TargerPopup->CreateTextEx(2,  GetDefaultFontName());
	m_pDesignationMyInof_TargerPopup->SetSelColor(100, 100, 100, 100);
	m_pDesignationMyInof_TargerPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationMyInof_TargerPopup->SetExclusiveEnableControl(TRUE);
	m_pDesignationMyInof_TargerPopup->AddElement(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_CLAN_TEXT));/*Ŭ��*/
	m_pDesignationMyInof_TargerPopup->AddElement(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_FRIEND_TEXT));/*ģ��*/
	m_pDesignationMyInof_TargerPopup->SetEnable(FALSE);	

	m_pDesignationMyInof_WhoPopup->CreateTextEx(5,  GetDefaultFontName());
	m_pDesignationMyInof_WhoPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationMyInof_WhoPopup->SetSelColor(0, 0, 0, 0);
	m_pDesignationMyInof_WhoBG->SetEnable( FALSE);

	INIT_CAPTION_BUTTON(m_pDesignationGetButton, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_GET_TEXT));/*ȣĪ ȹ��*/
	INIT_CAPTION_BUTTON(m_pDesignationChangeButton, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_CONFIRM));/*Ȯ��*/
	
	m_pDesignationNeedChangeExplain->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationNeedChangeExplain->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationNeedChangeExplain->SetTextSpace( 0, 2);
	m_pDesignationNeedChangeExplain->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_GET_EXPLAIN_MESSAGE));/*������ ȣĪ�� ����� ������ �����ϰ� Ȯ�� ��ư�� ������ ȣĪ�� ��ϵ˴ϴ�.\n(��,{col:253,167,0,255}���ϰ迭{/col}�� ȣĪ�� ���ÿ� ����Ͻ� �� ������ ��ü�� �����մϴ�.)*/

	m_pDesignationMyInfo_Key->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDesignationMyInfo_Key->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDesignationMyInfo_Key->SetText(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_TEXT));/*ȣĪ*/

	for(UINT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		INIT_BUTTON_EX(m_pDesignationButton[i]);	

		m_pDesignationImage[i]->SetAllShapeEnable( FALSE);
		m_pDesignationImage[i]->SetShapeEnable( i, TRUE);
		m_pDesignationImage[i]->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 77);
	}
	
#if defined( DESIGZATION_ENABLE)
	CheckDesignationGet();
#endif

	for(UINT32 i = 0; i < MAX_EVENT_DESIGNATION; i++)
	{
		INIT_BUTTON_EX(m_pDesignationEventButton[i]);	
		m_pDesignationEventImage[i]->SetAllShapeEnable( FALSE);
	}

	for(UINT32 i = 0; i < MAX_DESIGNATION_SLOT; i++)
	{		
		m_pDesignationMyInfo_Slot_Image[i]->SetAllShapeEnable( FALSE);
		m_pDesignationMyInfo_Slot_Image_Lock[i]->SetEnable(TRUE);

		m_pDesignationChange_Slot_Image[i]->SetAllShapeEnable( FALSE);
		m_pDesignationChange_Slot_Image_Lock[i]->SetEnable(TRUE);
	}

	m_pDesignationMyInfo_Slot_Text[0]->ReCreateTextEx(  GetDefaultFontName(), FONT_COUNT_32, 8);
	m_pDesignationMyInfo_Slot_Text[1]->ReCreateTextEx(  GetDefaultFontName(), FONT_COUNT_32, 8);
	m_pDesignationMyInfo_Slot_Text[2]->ReCreateTextEx(  GetDefaultFontName(), FONT_COUNT_32, 8);

	m_pDesignationChange_Slot_Text[0]->ReCreateTextEx(  GetDefaultFontName(), FONT_COUNT_32, 8);
	m_pDesignationChange_Slot_Text[1]->ReCreateTextEx(  GetDefaultFontName(), FONT_COUNT_32, 8);
	m_pDesignationChange_Slot_Text[2]->ReCreateTextEx(  GetDefaultFontName(), FONT_COUNT_32, 8);
	
	// �ʱ⿡ ȣģ���� ���̾�α״� ��Ȱ��ȭ �մϴ�.
	m_pDesignationChangeBG->SetEnable( FALSE);
	
	// ���� ������� �����Ƿ� ��Ȱ��ȭ�մϴ�.
	{
		// ȣĪ �̺�Ʈ
		{
			m_pDesignationTreeEventCategory_BG->SetEnable( FALSE);
			for(UINT32 i = 0; i < MAX_EVENT_DESIGNATION; i++)	m_pDesignationEventButton[i]->SetEnable( FALSE);
		}
	}	
}

void CStepProfile::_UpdateDesignationDeatail( REAL32 rDeltaSeconds)
{
	INT32 i = 0;
	DESIGNATION_ID idx = DESIGNATION_ID_NONE;
	char szTemp[MAX_STRING_COUNT];
	char szApplyType[MAX_STRING_COUNT];
	char szPlusEffect[MAX_STRING_COUNT];
	char szMinusEffect[MAX_STRING_COUNT];
	char szAddEffect[MAX_STRING_COUNT];
	szTemp[0] = 0;
	szApplyType[0] = 0;
	szPlusEffect[0] = 0;
	szMinusEffect[0] = 0;
	szAddEffect[0] = 0;

	if( m_SelectedDesignationIdx != -1)
	{
		if( m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
		{
			m_pDesignationSelectedImage->SetShapeEnable( m_SelectedDesignationIdx, TRUE);			
			m_pDesignationInfoText->SetText(g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].szExplain);

			switch( g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].m_AppyType)
			{
			case DESIGNATION_APPLY_TYPE_ALL:
				sprintf( szApplyType, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_TYPE_ALL));/*��ü �迭*/
				break;
			case DESIGNATION_APPLY_TYPE_ASSULT:
				sprintf( szApplyType, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_TYPE_ASSULT));/*���ݼ��� �迭*/
				break;
			case DESIGNATION_APPLY_TYPE_RIFLE:
				sprintf( szApplyType, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_TYPE_SNIPER));/*���ݼ��� �迭*/
				break;
			case DESIGNATION_APPLY_TYPE_SMG:
				sprintf( szApplyType, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_TYPE_SMG));/*������� �迭*/
				break;
			case DESIGNATION_APPLY_TYPE_MELLE:
				sprintf( szApplyType, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_TYPE_MELEE));/*�������� �迭*/
				break;
			case DESIGNATION_APPLY_TYPE_HANDGUN:
				sprintf( szApplyType, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_TYPE_SECONDRY));/*�������� �迭*/
				break;
			case DESIGNATION_APPLY_TYPE_SHOTGUN:
				sprintf( szApplyType, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_TYPE_SHOTGUN));/*��ź�� �迭*/
				break;
			case DESIGNATION_APPLY_TYPE_BOMB:
				sprintf( szApplyType, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_TYPE_EXPLOSION));/*���Ĺ��� �迭*/
				break;				
			}
			sprintf( szTemp, "{col:204,204,204,255}%s{/col}",szApplyType);
			m_pDesignationApplyTypeText->SetText(szTemp);
			m_nCurShowingDesignation = g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].m_Type;

			sprintf( szTemp, "{col:80,150,215,255}%s{/col}\n{col:204,204,204,255}%s{/col}", g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].szName,
																				  		    g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].szName_E );
			m_pDesignationNameText->SetText(szTemp);

			for( i=0; i<2; i++)
			{
				idx = (DESIGNATION_ID)g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].m_PlusEffect[i];
				if( idx != DESIGNATION_ID_NONE)
				{
					if(i==0)
					{
						sprintf( szPlusEffect, "{col:80,150,215,255}%s{/col}", g_UserTitleEffect[idx]._Text);				
					}
					else
					{
						sprintf( szPlusEffect, "%s\n{col:80,150,215,255}%s{/col}", szPlusEffect, g_UserTitleEffect[idx]._Text);
					}
				}
				else
				{
					if(i==0)
					{
						sprintf( szPlusEffect, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NONE_WITH_COLOR1));/*{col:80,150,215,255}����{/col}*/	
					}
				}
			}

			for( i=0; i<2; i++)
			{
				idx = (DESIGNATION_ID)g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].m_MinusEffect[i];
				if( idx != DESIGNATION_ID_NONE)
				{
					if(i==0)
					{
						sprintf( szMinusEffect, "{col:205,100,50,255}%s{/col}", g_UserTitleEffect[idx]._Text);				
					}
					else
					{
						sprintf( szMinusEffect, "%s\n{col:205,100,50,255}%s{/col}", szMinusEffect, g_UserTitleEffect[idx]._Text);
					}
				}
				else
				{
					if(i==0)
					{
						sprintf( szMinusEffect, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NONE_WITH_COLOR2));/*{col:205,100,50,255}����{/col}*/	
					}
				}
			}

			for( i=0; i<2; i++)
			{
				idx = (DESIGNATION_ID)g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].m_AddEffect[i];
				if( idx != DESIGNATION_ID_NONE)
				{
					if(i==0)
					{
						sprintf( szAddEffect, "{col:204,204,204,255}%s{/col}",g_UserTitleEffect[idx]._Text);				
					}
					else
					{
						sprintf( szAddEffect, "%s\n{col:204,204,204,255}%s{/col}", szAddEffect, g_UserTitleEffect[idx]._Text);
					}
				}
				else
				{
					if(i==0)
					{
						sprintf( szAddEffect, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NONE_WITH_COLOR3));/*{col:204,204,204,255}����{/col}*/	
					}
				}
			}

			m_pDesignationPlusEffectText->SetText(szPlusEffect);
			m_pDesignationMinusEffectText->SetText(szMinusEffect);
			m_pDesignationAddEffectText->SetText(szAddEffect);
		}
	}
	else
	{
		m_pDesignationNameText->SetText("");
		m_pDesignationInfoText->SetText("");
		m_pDesignationApplyTypeText->SetText("");
		m_pDesignationPlusEffectText->SetText("");
		m_pDesignationMinusEffectText->SetText("");
		m_pDesignationAddEffectText->SetText("");
		m_nCurShowingDesignation = DESIGNATION_TYPE_NONE;
	}	
}

void CStepProfile::_UpdateDesignationGetInfo( REAL32 rDeltaSeconds)
{
	INT32 i = 0;
	INT32 nType = 0, nID = 0, nCount = 0;
	UINT8 * pID = NULL;
	char szName[MAX_STRING_COUNT];
	char szCount[MAX_STRING_COUNT];
	szName[0] = 0;
	szCount[0] = 0;

	if( m_SelectedDesignationIdx != -1)
	{
		// ȣĪ ȹ��/���/����
		m_pDesignationGetButton->SetEnable( TRUE);
		if( !m_bGetDesignation[m_SelectedDesignationIdx])
		{
			m_DesignationMode = DESIGNATION_MODE_GET;
			m_pDesignationGetButton->SetCaption(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_GET_TEXT));/*ȣĪ ȹ��*/
		}
		else if( m_bGetDesignation[m_SelectedDesignationIdx])
		{
			if( CheckDesignationEquip())
			{
				m_DesignationMode = DESIGNATION_MODE_RELEASE;
				m_pDesignationGetButton->SetCaption(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_RELEASE_TEXT));/*ȣĪ ����*/
			}
			else
			{
				m_DesignationMode = DESIGNATION_MODE_CHANGE;
				m_pDesignationGetButton->SetCaption(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_USE_TEXT));/*ȣĪ ���*/
			}
		}

		if( m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
		{
			nType = g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].m_Type;			

			// 1.�ʿ� ǥ�� ����
			for( i=0; i<USER_TITLE_REQUIREMENT_COUNT; i++)
			{
				nID = g_UserTitleRequirement[nType]._ribbon[i]._id;
				nCount = g_UserTitleRequirement[nType]._ribbon[i]._count;
				
				switch( g_UserTitleRequirement[nType]._ribbon[i]._type)
				{
				case UTR_RIBBON:
					g_pQuestTextDictionary->GetRibbonNameString( szName, (UINT8)nID);
					pID = g_pGameContext->getMiniature();
					break;
				case UTR_ENSIGN:
					g_pQuestTextDictionary->GetEnsignNameString( szName, (UINT8)nID);
					pID = g_pGameContext->getInsignia();
					break;
				case UTR_MEDAL:
					g_pQuestTextDictionary->GetMedalNameString( szName, (UINT8)nID);
					pID = g_pGameContext->getOrder();
					break;
				case UTR_NOTHING:
					sprintf( szName, "----");
					pID = NULL;
					nCount = 0;
					break;
				}						

				// �ʿ� ǥ�� ȹ��翡 ���� Color�� ǥ���մϴ�.
				if( pID != NULL)	
				{
					if( pID[nID] == 0)
					{
						// ���� ȹ������ ������.
						sprintf( szCount, "{col: 153,153,153,255}%d/%d{/col}", pID[nID], nCount);						
					}
					else if( pID[nID] < nCount)
					{
						// ȹ���� �Ͽ����� ���� ������ ����.
						sprintf( szCount, "{col: 204,204,204,255}%d/%d{/col}", pID[nID], nCount);
					}
					else if( pID[nID] >= nCount)
					{
						// ���� ȹ���Ͽ��ų�, ���̻� ȹ���Ͽ���.
						sprintf( szCount, "{col: 155,205,50,255}%d/%d{/col}", pID[nID], nCount);
					}
				}
				else
				{
					sprintf( szCount, "{col: 153,153,153,255}%d/%d{/col}", 0, nCount);
				}

				switch( i)
				{
				case 0:
					m_pDesignationRibonNeedText1->SetTextEllipsis(szName);
					m_pDesignationRibonNeedCount1->SetText(szCount);
					break;
				case 1:
					m_pDesignationRibonNeedText2->SetTextEllipsis(szName);
					m_pDesignationRibonNeedCount2->SetText(szCount);
					break;
				case 2:
					m_pDesignationRibonNeedText3->SetTextEllipsis(szName);
					m_pDesignationRibonNeedCount3->SetText(szCount);
					break;
				case 3:
					m_pDesignationRibonNeedText4->SetTextEllipsis(szName);
					m_pDesignationRibonNeedCount4->SetText(szCount);
					break;
				}
			}

			// 2. �ʿ� ȣĪ ����
			for( i=0; i<2; i++)
			{
				char * pszTemp = NULL;

				if( g_UserTitleRequirement[nType]._userTitleId[i] != 0)
				{
					for(INT32 x=0; x<MAX_DEFAULT_USER_TITLE_COUNT; x++)
					{
						if( g_UserTitleRequirement[nType]._userTitleId[i] == g_pGameContext->m_DesignationInfo[x].m_Type)
						{
							pszTemp = g_pGameContext->m_DesignationInfo[x].szName;
							break;
						}
					}
				}

				if( pszTemp != NULL)
				{
					switch( i)
					{
					case 0:
						m_pDesignationNeedText->SetTextEllipsis(pszTemp);
						break;
					case 1:
						m_pDesignationNeedText2->SetTextEllipsis(pszTemp);
						break;
					}
				}
				else
				{
					switch( i)
					{
					case 0:
						m_pDesignationNeedText->SetTextEllipsis("----");
						break;
					case 1:
						m_pDesignationNeedText2->SetTextEllipsis("----");
						break;
					}
				}
			}

			// 3. �ʿ� ��� ����
			m_pDesignationNeedRankText->SetTextEllipsis( STR_TBL( (STRING_TABLE_IDX)(STBL_IDX_RANK_00 + g_UserTitleRequirement[nType]._rank)));
		}
	}
	else
	{
		m_pDesignationRibonNeedText1->SetText("{col:204, 204, 204, 255}----{/col}");
		m_pDesignationRibonNeedCount1->SetText("{col:204, 204, 204, 255}0/0{/col}");

		m_pDesignationRibonNeedText2->SetText("{col:204, 204, 204, 255}----{/col}");
		m_pDesignationRibonNeedCount2->SetText("{col:204, 204, 204, 255}0/0{/col}");

		m_pDesignationRibonNeedText3->SetText("{col:204, 204, 204, 255}----{/col}");
		m_pDesignationRibonNeedCount3->SetText("{col:204, 204, 204, 255}0/0{/col}");

		m_pDesignationRibonNeedText4->SetText("{col:204, 204, 204, 255}----{/col}");
		m_pDesignationRibonNeedCount4->SetText("{col:204, 204, 204, 255}0/0{/col}");

		m_pDesignationNeedText->SetText("{col:204, 204, 204, 255}----{/col}");
		m_pDesignationNeedText2->SetText("{col:204, 204, 204, 255}----{/col}");
		m_pDesignationNeedRankText->SetText("{col:204, 204, 204, 255}----{/col}");

		m_pDesignationGetButton->SetEnable( FALSE);
	}	
}

void CStepProfile::_UpdateDesignationMyInfo( REAL32 rDeltaSeconds)
{
	INT32 i = 0;
	INT32 x = 0;

	const USER_INFO_BASIC * pInfoBasic = g_pGameContext->GetMyUserInfoBasic();

	// �г���	
	SetNameUsingColorTable(m_pDesignationMyInfoNick, (char*)pInfoBasic->_nick, pInfoBasic->_nickcolor);

	// ���
	INT32 nRank = MINMAX(0, pInfoBasic->_rank, RANK_COUNT - 1);
	m_pDesignationMyInfo_Rank->SetAllShapeEnable(FALSE);
	m_pDesignationMyInfo_Rank->SetShapeEnable(GUI_SHAPE_OFFSET + nRank, TRUE);
	
	I3ASSERT( g_pGameContext->getLimitUserTitleCount() <= MAX_DESIGNATION_SLOT);

	for( i=0; i<g_pGameContext->getLimitUserTitleCount(); i++)
	{
		m_pDesignationMyInfo_Slot_Image_Lock[i]->SetEnable(FALSE);
		m_pDesignationMyInfo_Slot_Image[i]->SetAllShapeEnable( FALSE);
		m_pDesignationMyInfo_Slot_Text[i]->SetText("");
	}

	// ȣĪ�̹��� �� ȣĪ�̸�.
	for( i=0; i<MAX_EQUIP_USER_TITLE; i++)
	{
		if( !m_pDesignationMyInfo_Slot_Image_Lock[i]->isEnable())
		{
			const UINT8* pIdx = g_pGameContext->getEquipUserTitle();

			I3ASSERT( (pIdx[i] >= 0) && (pIdx[i] <= MAX_DEFAULT_USER_TITLE_COUNT));

			if( pIdx[i] != 0)
			{
				for( x=0; x<MAX_DEFAULT_USER_TITLE_COUNT; x++)
				{
					if( g_pGameContext->m_DesignationInfo[x].m_Type == pIdx[i])
					{
						m_pDesignationMyInfo_Slot_Image[i]->SetAllShapeEnable( FALSE);
						m_pDesignationMyInfo_Slot_Image[i]->SetShapeEnable( x, TRUE);
						m_pDesignationMyInfo_Slot_Text[i]->SetTextEllipsis( g_pGameContext->m_DesignationInfo[x].szName);
						break;
					}
					else
					{	
						m_pDesignationMyInfo_Slot_Image[i]->SetAllShapeEnable( FALSE);
						m_pDesignationMyInfo_Slot_Text[i]->SetText("");
					}
				}		
			}
			else
			{
				m_pDesignationMyInfo_Slot_Image[i]->SetAllShapeEnable( FALSE);
				m_pDesignationMyInfo_Slot_Text[i]->SetText("");
			}
		}
	}	

	//
	CheckDesignationGet();
}

void CStepProfile::_UpdateDesignationChangePopup( REAL32 rDeltaSeconds)
{
	INT32 i = 0;
	INT32 x = 0;

	I3ASSERT( g_pGameContext->getLimitUserTitleCount() <= MAX_DESIGNATION_SLOT);

	for( i=0; i<g_pGameContext->getLimitUserTitleCount(); i++)
	{
		m_pDesignationChange_Slot_Image_Lock[i]->SetEnable(FALSE);
	}
	
	// ȣĪ�̹��� �� ȣĪ�̸�.
	for( i=0; i<MAX_EQUIP_USER_TITLE; i++)
	{
		if( !m_pDesignationMyInfo_Slot_Image_Lock[i]->isEnable())
		{
			const UINT8* pIdx = g_pGameContext->getEquipUserTitle();

			I3ASSERT( (pIdx[i] >= 0) && (pIdx[i] <= MAX_DEFAULT_USER_TITLE_COUNT));

			if( pIdx[i] != 0)
			{
				for( x=0; x<MAX_DEFAULT_USER_TITLE_COUNT; x++)
				{
					if( g_pGameContext->m_DesignationInfo[x].m_Type == pIdx[i])
					{
						m_pDesignationChange_Slot_Image[i]->SetAllShapeEnable( FALSE);
						m_pDesignationChange_Slot_Image[i]->SetShapeEnable( x, TRUE);
						m_pDesignationChange_Slot_Text[i]->SetTextEllipsis( g_pGameContext->m_DesignationInfo[x].szName);
						break;
					}
					else
					{	
						m_pDesignationChange_Slot_Image[i]->SetAllShapeEnable( FALSE);
						m_pDesignationChange_Slot_Text[i]->SetText("");
					}
				}		
			}
			else
			{
				m_pDesignationChange_Slot_Image[i]->SetAllShapeEnable( FALSE);
				m_pDesignationChange_Slot_Text[i]->SetText("");
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

void CStepProfile::_UpdateDesignationTree( REAL32 rDeltaSeconds)
{
	m_pDesignationSelectedImage->SetAllShapeEnable( FALSE);
	
	if( m_SelectedDesignationIdx != -1)
	{
		if( m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
		{
			m_pDesignationButton[m_SelectedDesignationIdx]->SetShapeEnable( 2, TRUE);				
			m_pDesignationSelectedImage->SetShapeEnable( m_SelectedDesignationIdx, TRUE);
		}
	}

	// ȹ���� ȣĪ�� ���Ͽ��� �׻� Ư���� ǥ�ø� �մϴ�.
	const UINT8* pIdx = g_pGameContext->getEquipUserTitle();	

	for( UINT32 i=0; i<MAX_EQUIP_USER_TITLE; i++)
	{
		I3ASSERT( (pIdx[i] >= 0) && (pIdx[i] <= MAX_DEFAULT_USER_TITLE_COUNT));

		if( pIdx[i] != 0)
		{
			for(UINT32 x = 0; x < MAX_DEFAULT_USER_TITLE_COUNT; x++)
			{
				if( g_pGameContext->m_DesignationInfo[x].m_Type == pIdx[i])
				{
					m_pDesignationButton[x]->SetShapeEnable( 3, TRUE);
					break;
				}
			}
		}
	}
}

void CStepProfile::OnGuiNotify_Designation(I3GUI_CONTROL_NOTIFY* pNotify)
{
	if( pNotify->m_nID >= GCI_PRS_S_DESIGNATION_IMAGE_1 && pNotify->m_nID <= GCI_PRS_S_DESIGNATION_IMAGE_44)
	{		
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent) 
		{
			if( m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->GetShapeEnable(3))
			{
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetAllShapeEnable( FALSE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 1, TRUE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 3, TRUE);
			}
			else
			{
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetAllShapeEnable( FALSE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 1, TRUE);
			}
		}
		else if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) 
		{
			m_SelectedDesignationIdx = pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1;
		
			if( m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->GetShapeEnable(3))
			{
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetAllShapeEnable( FALSE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 2, TRUE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 3, TRUE);
			}
			else
			{
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetAllShapeEnable( FALSE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 2, TRUE);
			}
			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent) 
		{
			m_SelectedDesignationIdx = pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1;
		
			if( m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->GetShapeEnable(3))
			{
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetAllShapeEnable( FALSE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 2, TRUE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 3, TRUE);
			}
			else
			{
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetAllShapeEnable( FALSE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 2, TRUE);
			}
			PlaySound( GUI_SND_ITEM_SELECTING);

			if( m_bGetDesignation[m_SelectedDesignationIdx])
			{
				if( CheckDesignationEquip())
				{
					// ȣĪ ���� �˾�
					STRING_POPUP_Q(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_RELEASE_ANSWER_MESSAGE), CbDesignationRelese);/*������ ȣĪ�� �����Ͻðڽ��ϱ�?*/
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
				OnDesignationGet();
			}
		}
		else
		{
			if( m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->GetShapeEnable(3))
			{
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetAllShapeEnable( FALSE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 0, TRUE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 3, TRUE);
			}
			else
			{
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetAllShapeEnable( FALSE);
				m_pDesignationButton[pNotify->m_nID - GCI_PRS_S_DESIGNATION_IMAGE_1]->SetShapeEnable( 0, TRUE);
			}
		}		

		if( m_SelectedDesignationIdx != -1)	
		{
			m_pDesignationButton[m_SelectedDesignationIdx]->SetShapeEnable( 2, TRUE);
		}		
	}

	switch(pNotify->m_nID)
	{
	case GCI_PRS_B_GET_DESIGNATION:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnDesignationGet();
		break;
	case GCI_PRS_B_GET_DESIGNATION_CHANGE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnDesignationChange();
		break;
	case GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT1:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) 
		{
			if( m_pDesignationChange_Slot_Image_Lock[0]->isEnable())
			{
				m_SelectedDesignationChangeSlotIdx = -1;
			}
			else
			{
				m_SelectedDesignationChangeSlotIdx = 0;
				PlaySound( GUI_SND_ITEM_SELECTING);
			}			
		}
		else if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent) 
		{	
			OnDesignationChange();
		}
		break;
	case GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT2:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) 
		{
			if( m_pDesignationChange_Slot_Image_Lock[1]->isEnable())
			{
				m_SelectedDesignationChangeSlotIdx = -1;
			}
			else
			{
				m_SelectedDesignationChangeSlotIdx = 1;
				PlaySound( GUI_SND_ITEM_SELECTING);
			}			
		}
		else if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent) 
		{
			OnDesignationChange();
		}
		break;
	case GCI_PRS_S_GET_DESIGNATION_CHANGE_SLOT3:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) 
		{
			if( m_pDesignationChange_Slot_Image_Lock[2]->isEnable())
			{
				m_SelectedDesignationChangeSlotIdx = -1;
			}
			else
			{
				m_SelectedDesignationChangeSlotIdx = 2;
				PlaySound( GUI_SND_ITEM_SELECTING);
			}			
		}
		else if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent) 
		{	
			OnDesignationChange();
		}
		break;
	}

	// ȹ���� ȣĪ�� ���Ͽ��� �׻� Ư���� ǥ�ø� �մϴ�.
	const UINT8* pIdx = g_pGameContext->getEquipUserTitle();	
	for( UINT32 i=0; i<MAX_EQUIP_USER_TITLE; i++)
	{
		I3ASSERT( (pIdx[i] >= 0) && (pIdx[i] <= MAX_DEFAULT_USER_TITLE_COUNT));

		if( pIdx[i] != 0)
		{
			for(UINT32 x = 0; x < MAX_DEFAULT_USER_TITLE_COUNT; x++)
			{
				if( g_pGameContext->m_DesignationInfo[x].m_Type == pIdx[i])
				{
					m_pDesignationButton[x]->GetShapeEnable(3);
					break;
				}
			}
		}
	}

	for(UINT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		if( m_bGetDesignation[i] == TRUE)
		{
			m_pDesignationImage[i]->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 255);
		}
	}
}

void CStepProfile::OnDesignationGet(void)
{
	switch( m_DesignationMode)
	{
	case DESIGNATION_MODE_GET:
		STRING_POPUP_Q(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_GET_ANSWER_MESSAGE), CbDesignationGet);/*������ ȣĪ�� ȹ���Ͻðڽ��ϱ�?*/
		break;
	case DESIGNATION_MODE_CHANGE:
		m_pDesignationChangeBG->SetEnable( TRUE);
		break;
	case DESIGNATION_MODE_RELEASE:
		STRING_POPUP_Q(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_RELEASE_ANSWER_MESSAGE), CbDesignationRelese);/*������ ȣĪ�� �����Ͻðڽ��ϱ�?*/
		break;
	default:
		break;
	}	
}

void CStepProfile::OnDesignationChange(void)
{
	//
	if( m_SelectedDesignationChangeSlotIdx >= 0 && m_SelectedDesignationChangeSlotIdx < 3)
	{
		if( CheckDesignationSameType())
		{
			STRING_POPUP(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_FAIL_ANOTHER_USED));/*�ٸ� ���Կ� ������ �迭�� ȣĪ�� ����ϰ� �־� ��뿡 �����Ͽ����ϴ�.*/
		}
		else
		{
			g_pGameContext->SetEvent( EVENT_DESIGNATION_CHANGE, &m_SelectedDesignationChangeSlotIdx, &g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].m_Type);
		}
	}
	else
	{
		STRING_POPUP(STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NO_SELECTED_SLOT_MESSAGE));/*������ �������� �ʾҽ��ϴ�. ������ ������ �� �ٽ� Ȯ�� ��ư�� �����ֽñ� �ٶ��ϴ�.*/
	}

	m_DesignationMode = DESIGNATION_MODE_GET;
	m_SelectedDesignationChangeSlotIdx = -1;
	m_pDesignationChangeBG->SetEnable( FALSE);	
}

void CStepProfile::OnDesignationRelease(void)
{
	const UINT8* pEquipIdx = g_pGameContext->getEquipUserTitle();

	UINT8 nSlotIdx = (UINT8)m_SelectedDesignationReleaseSlotIdx;
	UINT8 nId = pEquipIdx[nSlotIdx];

	g_pGameContext->SetEvent( EVENT_DESIGNATION_RELEASE, &nSlotIdx, &nId );
}

void CStepProfile::CheckDesignationGet(void)
{
	char szTemp[256];

	for(UINT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		const INT8* pInfo = g_pGameContext->getGainUserTitle();
		if( pInfo[g_pGameContext->m_DesignationInfo[i].m_Type] == 1)
		{
			// ȹ���� ȣĪ.
			m_bGetDesignation[i] = TRUE;
		}
		else
		{
			// ��ȹ��.
			m_bGetDesignation[i] = FALSE;
		}
	}

	// �߰������� ȹ���Ͽ��ٸ�~!!
	if( m_nCurDesignationSlotCount != g_pGameContext->getLimitUserTitleCount())
	{
		if( !g_pFramework->IsMessageBoxEnabled())
		{
			m_nCurDesignationSlotCount = g_pGameContext->getLimitUserTitleCount();

			sprintf( szTemp, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_GET_SLOT_MESSAGE), m_nCurDesignationSlotCount);/*���ϵ帳�ϴ�!!\n \nȣĪ ������ ȹ���Ͽ� ȣĪ�� ���ÿ�\n����� �� �ִ� ������ �����Ͽ����ϴ�.\n(�ִ� %d���� ȣĪ ��� ����)*/
			STRING_POPUP( szTemp);
		}
	}
}

BOOL CStepProfile::CheckDesignationEquip(void)
{
	INT32 i = 0;

	for( i = 0; i < MAX_DESIGNATION_SLOT; i++)
	{
		const UINT8* pEquipIdx = g_pGameContext->getEquipUserTitle();
		if( pEquipIdx[i] == g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].m_Type)
		{
			m_SelectedDesignationReleaseSlotIdx = i;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CStepProfile::CheckDesignationSameType(void)
{
	INT32 i = 0, x = 0;

	for( i = 0; i < MAX_DESIGNATION_SLOT; i++)
	{
		if( m_SelectedDesignationChangeSlotIdx == i) continue;

		for( x = 0; x < MAX_DEFAULT_USER_TITLE_COUNT; x++)
		{
			const UINT8* pEquipIdx = g_pGameContext->getEquipUserTitle();
			if( pEquipIdx[i] == g_pGameContext->m_DesignationInfo[x].m_Type)
			{
				if( g_pGameContext->m_DesignationInfo[x].m_AppyType == g_pGameContext->m_DesignationInfo[m_SelectedDesignationIdx].m_AppyType)
				{
					// ���ϰ迭 �����
					return TRUE;
				}	
			}
		}
	}

	return FALSE;
}

void CStepProfile::LeaveDesignation(void)
{
	m_DesignationMode = DESIGNATION_MODE_GET;
	m_SelectedDesignationIdx = -1;
	m_SelectedDesignationChangeSlotIdx = -1;
	m_pDesignationChangeBG->SetEnable( FALSE);
}

void CStepProfile::CbDesignationGet(void* pThis, INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		g_pGameContext->SetEvent( EVENT_DESIGNATION_GET, &( ((CStepProfile*)pThis)->m_nCurShowingDesignation) );
	}
}

void CStepProfile::CbDesignationRelese(void* pThis, INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		((CStepProfile*)pThis)->OnDesignationRelease();
	}
}

void CStepProfile::_InitDesignationInfo(void)
{
	// A Group
	sprintf( g_pGameContext->m_DesignationInfo[0].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO0));/*�Ϲ� ���*/
	sprintf( g_pGameContext->m_DesignationInfo[0].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO0));/*Member*/
	sprintf( g_pGameContext->m_DesignationInfo[0].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO0));/*���ʷ� �ο� �޴� �⺻���� ȣĪ���� ���� ���� ����� ȣĪ*/
	g_pGameContext->m_DesignationInfo[0].m_AppyType = DESIGNATION_APPLY_TYPE_ALL;
	g_pGameContext->m_DesignationInfo[0].m_Type = DESIGNATION_TYPE_1;
	g_pGameContext->m_DesignationInfo[0].m_PlusEffect[0] = DESIGNATION_ID_JUMP_ABILITY_PLUS_1;
	g_pGameContext->m_DesignationInfo[0].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[0].m_MinusEffect[0] = DESIGNATION_ID_FALLDOWN_DAMAGE_INCREASE_1;
	g_pGameContext->m_DesignationInfo[0].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[0].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[0].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[1].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO1));/*���� ���*/
	sprintf( g_pGameContext->m_DesignationInfo[1].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO1));/*Junior*/
	sprintf( g_pGameContext->m_DesignationInfo[1].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO1));/*��� ���� ���������� ���� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[1].m_AppyType = DESIGNATION_APPLY_TYPE_ALL;
	g_pGameContext->m_DesignationInfo[1].m_Type = DESIGNATION_TYPE_2;
	g_pGameContext->m_DesignationInfo[1].m_PlusEffect[0] = DESIGNATION_ID_MOVE_ABILITY_PLUS_1;
	g_pGameContext->m_DesignationInfo[1].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[1].m_MinusEffect[0] = DESIGNATION_ID_JUMP_ABILITY_MINUS_1;
	g_pGameContext->m_DesignationInfo[1].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[1].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[1].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[2].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO2));/*Ư�� ���*/
	sprintf( g_pGameContext->m_DesignationInfo[2].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO2));/*Veteran*/
	sprintf( g_pGameContext->m_DesignationInfo[2].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO2));/*����� ���� �ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[2].m_AppyType = DESIGNATION_APPLY_TYPE_ALL;
	g_pGameContext->m_DesignationInfo[2].m_Type = DESIGNATION_TYPE_3;
	g_pGameContext->m_DesignationInfo[2].m_PlusEffect[0] = DESIGNATION_ID_MOVE_ABILITY_PLUS_2;
	g_pGameContext->m_DesignationInfo[2].m_PlusEffect[1] = DESIGNATION_ID_FALLDOWN_DAMAGE_REDUCE_1;
	g_pGameContext->m_DesignationInfo[2].m_MinusEffect[0] = DESIGNATION_ID_ADDSPEED_ABILITY_MINUS_1;
	g_pGameContext->m_DesignationInfo[2].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[2].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[2].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[3].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO3));/*Ư�� ���*/
	sprintf( g_pGameContext->m_DesignationInfo[3].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO3));/*Specialist*/
	sprintf( g_pGameContext->m_DesignationInfo[3].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO3));/*Ź���� ���� �ɷ��� ���� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[3].m_AppyType = DESIGNATION_APPLY_TYPE_ALL;
	g_pGameContext->m_DesignationInfo[3].m_Type = DESIGNATION_TYPE_4;
	g_pGameContext->m_DesignationInfo[3].m_PlusEffect[0] = DESIGNATION_ID_MOVE_ABILITY_PLUS_3;
	g_pGameContext->m_DesignationInfo[3].m_PlusEffect[1] = DESIGNATION_ID_ADDSPEED_ABILITY_PLUS_1;
	g_pGameContext->m_DesignationInfo[3].m_MinusEffect[0] = DESIGNATION_ID_FALLDOWN_DAMAGE_INCREASE_1;
	g_pGameContext->m_DesignationInfo[3].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[3].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[3].m_AddEffect[1] = DESIGNATION_ID_NONE;

	// B Group
	sprintf( g_pGameContext->m_DesignationInfo[4].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO4));/*���� �δ��*/
	sprintf( g_pGameContext->m_DesignationInfo[4].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO4));/*Assult Soldier*/
	sprintf( g_pGameContext->m_DesignationInfo[4].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO4));/*���� �ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[4].m_AppyType = DESIGNATION_APPLY_TYPE_ASSULT;
	g_pGameContext->m_DesignationInfo[4].m_Type = DESIGNATION_TYPE_5;
	g_pGameContext->m_DesignationInfo[4].m_PlusEffect[0] = DESIGNATION_ID_HORIZONTAL_REACTION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[4].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[4].m_MinusEffect[0] = DESIGNATION_ID_TARGET_DECLINATION_INCREASE_1;
	g_pGameContext->m_DesignationInfo[4].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[4].m_AddEffect[0] = DESIGNATION_ID_GET_SLOT2_A;
	g_pGameContext->m_DesignationInfo[4].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[5].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO5));/*���� �δ��*/
	sprintf( g_pGameContext->m_DesignationInfo[5].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO5));/*Backup Soldier*/
	sprintf( g_pGameContext->m_DesignationInfo[5].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO5));/*���� �迭 �ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[5].m_AppyType = DESIGNATION_APPLY_TYPE_RIFLE;
	g_pGameContext->m_DesignationInfo[5].m_Type = DESIGNATION_TYPE_6;
	g_pGameContext->m_DesignationInfo[5].m_PlusEffect[0] = DESIGNATION_ID_MOVE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[5].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[5].m_MinusEffect[0] = DESIGNATION_ID_RELOAD_TIME_INCREASE_1;
	g_pGameContext->m_DesignationInfo[5].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[5].m_AddEffect[0] = DESIGNATION_ID_GET_SLOT2_B;
	g_pGameContext->m_DesignationInfo[5].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[6].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO6));/*���� �δ��*/
	sprintf( g_pGameContext->m_DesignationInfo[6].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO6));/*Assail Soldier*/
	sprintf( g_pGameContext->m_DesignationInfo[6].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO6));/*���� �迭 �ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[6].m_AppyType = DESIGNATION_APPLY_TYPE_SMG;
	g_pGameContext->m_DesignationInfo[6].m_Type = DESIGNATION_TYPE_7;
	g_pGameContext->m_DesignationInfo[6].m_PlusEffect[0] = DESIGNATION_ID_FIRE_RANGE_PLUS_1;
	g_pGameContext->m_DesignationInfo[6].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[6].m_MinusEffect[0] = DESIGNATION_ID_FIRE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[6].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[6].m_AddEffect[0] = DESIGNATION_ID_GET_SLOT2_C;
	g_pGameContext->m_DesignationInfo[6].m_AddEffect[1] = DESIGNATION_ID_NONE;

	// C Group
	sprintf( g_pGameContext->m_DesignationInfo[7].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO7));/*�Ϲ� ���ݴ��*/
	sprintf( g_pGameContext->m_DesignationInfo[7].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO7));/*Assaulter*/
	sprintf( g_pGameContext->m_DesignationInfo[7].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO7));/*���ο��� ������ �ӹ���  �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[7].m_AppyType = DESIGNATION_APPLY_TYPE_ASSULT;
	g_pGameContext->m_DesignationInfo[7].m_Type = DESIGNATION_TYPE_8;
	g_pGameContext->m_DesignationInfo[7].m_PlusEffect[0] = DESIGNATION_ID_VERTICAL_REACTION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[7].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[7].m_MinusEffect[0] = DESIGNATION_ID_MOVE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[7].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[7].m_AddEffect[0] = DESIGNATION_ID_GET_SG550_S;
	g_pGameContext->m_DesignationInfo[7].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[14].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO14));/*���� ���ݴ��*/
	sprintf( g_pGameContext->m_DesignationInfo[14].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO14));/*Junior Assaulter*/
	sprintf( g_pGameContext->m_DesignationInfo[14].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO14));/*�η����� �𸣴� ����� ���ݴ������ �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[14].m_AppyType = DESIGNATION_APPLY_TYPE_ASSULT;
	g_pGameContext->m_DesignationInfo[14].m_Type = DESIGNATION_TYPE_9;
	g_pGameContext->m_DesignationInfo[14].m_PlusEffect[0] = DESIGNATION_ID_MOVE_ABILITY_PLUS_1;
	g_pGameContext->m_DesignationInfo[14].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[14].m_MinusEffect[0] = DESIGNATION_ID_VERTICAL_REACTION_INCREASE_1;
	g_pGameContext->m_DesignationInfo[14].m_MinusEffect[1] = DESIGNATION_ID_HORIZONTAL_REACTION_INCREASE_1;
	g_pGameContext->m_DesignationInfo[14].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[14].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[21].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO21));/*���� ���ݴ��*/
	sprintf( g_pGameContext->m_DesignationInfo[21].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO21));/*Senior Assaulter*/
	sprintf( g_pGameContext->m_DesignationInfo[21].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO21));/*������ �����ӹ��� �ϼ��� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[21].m_AppyType = DESIGNATION_APPLY_TYPE_ASSULT;
	g_pGameContext->m_DesignationInfo[21].m_Type = DESIGNATION_TYPE_10;
	g_pGameContext->m_DesignationInfo[21].m_PlusEffect[0] = DESIGNATION_ID_TARGET_DECLINATION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[21].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[21].m_MinusEffect[0] = DESIGNATION_ID_FIRE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[21].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[21].m_AddEffect[0] = DESIGNATION_ID_GET_G36C_EXT;
	g_pGameContext->m_DesignationInfo[21].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[28].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO28));/*Ư�� �����屳*/
	sprintf( g_pGameContext->m_DesignationInfo[28].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO28));/*Veteran Assult Officer*/
	sprintf( g_pGameContext->m_DesignationInfo[28].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO28));/*���� ������ ���ư� �� �ƴ� ����� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[28].m_AppyType = DESIGNATION_APPLY_TYPE_ASSULT;
	g_pGameContext->m_DesignationInfo[28].m_Type = DESIGNATION_TYPE_11;
	g_pGameContext->m_DesignationInfo[28].m_PlusEffect[0] = DESIGNATION_ID_MOVE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[28].m_PlusEffect[1] = DESIGNATION_ID_FIRE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[28].m_MinusEffect[0] = DESIGNATION_ID_FIRE_RANGE_MINUS_1;
	g_pGameContext->m_DesignationInfo[28].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[28].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[28].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[32].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO32));/*���� ���ְ�*/
	sprintf( g_pGameContext->m_DesignationInfo[32].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO32));/*Assault Commander*/
	sprintf( g_pGameContext->m_DesignationInfo[32].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO32));/*���� �ӹ��� ������ �¸��� �Ȱ� �ִ� �� ���� ����*/
	g_pGameContext->m_DesignationInfo[32].m_AppyType = DESIGNATION_APPLY_TYPE_ASSULT;
	g_pGameContext->m_DesignationInfo[32].m_Type = DESIGNATION_TYPE_12;
	g_pGameContext->m_DesignationInfo[32].m_PlusEffect[0] = DESIGNATION_ID_FIRE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[32].m_PlusEffect[1] = DESIGNATION_ID_TARGET_DECLINATION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[32].m_MinusEffect[0] = DESIGNATION_ID_VERTICAL_REACTION_INCREASE_1;
	g_pGameContext->m_DesignationInfo[32].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[32].m_AddEffect[0] = DESIGNATION_ID_GET_AK_SOPMOD;
	g_pGameContext->m_DesignationInfo[32].m_AddEffect[1] = DESIGNATION_ID_GET_SLOT3_A;

	sprintf( g_pGameContext->m_DesignationInfo[38].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO38));/*������ �ͼ�*/
	sprintf( g_pGameContext->m_DesignationInfo[38].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO38));/*Rowdy Beast*/
	sprintf( g_pGameContext->m_DesignationInfo[38].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO38));/*�� ���� ������ ��ǳ���� ���� ��*/
	g_pGameContext->m_DesignationInfo[38].m_AppyType = DESIGNATION_APPLY_TYPE_ASSULT;
	g_pGameContext->m_DesignationInfo[38].m_Type = DESIGNATION_TYPE_13;
	g_pGameContext->m_DesignationInfo[38].m_PlusEffect[0] = DESIGNATION_ID_RELOAD_TIME_REDUCE_1;
	g_pGameContext->m_DesignationInfo[38].m_PlusEffect[1] = DESIGNATION_ID_FIRE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[38].m_MinusEffect[0] = DESIGNATION_ID_HORIZONTAL_REACTION_INCREASE_1;
	g_pGameContext->m_DesignationInfo[38].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[38].m_AddEffect[0] = DESIGNATION_ID_GET_BERET;
	g_pGameContext->m_DesignationInfo[38].m_AddEffect[1] = DESIGNATION_ID_NONE;

    // D Group
	sprintf( g_pGameContext->m_DesignationInfo[8].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO8));/*�Ϲ� ħ����*/
	sprintf( g_pGameContext->m_DesignationInfo[8].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO8));/*Infiltrator*/
	sprintf( g_pGameContext->m_DesignationInfo[8].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO8));/*ħ�� �ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[8].m_AppyType = DESIGNATION_APPLY_TYPE_MELLE;
	g_pGameContext->m_DesignationInfo[8].m_Type = DESIGNATION_TYPE_26;
	g_pGameContext->m_DesignationInfo[8].m_PlusEffect[0] = DESIGNATION_ID_DAMAGE_PLUS_1;
	g_pGameContext->m_DesignationInfo[8].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[8].m_MinusEffect[0] = DESIGNATION_ID_MELEE_RANGE_REDUCE_1;
	g_pGameContext->m_DesignationInfo[8].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[8].m_AddEffect[0] = DESIGNATION_ID_GET_DUAL_KNIFE;
	g_pGameContext->m_DesignationInfo[8].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[15].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO15));/*���� ħ����*/
	sprintf( g_pGameContext->m_DesignationInfo[15].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO15));/*Senior Infiltrator*/
	sprintf( g_pGameContext->m_DesignationInfo[15].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO15));/*������ ħ�� ������ ���� ����� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[15].m_AppyType = DESIGNATION_APPLY_TYPE_MELLE;
	g_pGameContext->m_DesignationInfo[15].m_Type = DESIGNATION_TYPE_27;
	g_pGameContext->m_DesignationInfo[15].m_PlusEffect[0] = DESIGNATION_ID_MELEE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[15].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[15].m_MinusEffect[0] = DESIGNATION_ID_MOVE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[15].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[15].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[15].m_AddEffect[1] = DESIGNATION_ID_NONE;
	
	sprintf( g_pGameContext->m_DesignationInfo[22].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO22));/*Ư�� ħ���屳*/
	sprintf( g_pGameContext->m_DesignationInfo[22].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO22));/*Veteran Infiltrator Officer*/
	sprintf( g_pGameContext->m_DesignationInfo[22].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO22));/*�Ҹ� ���� ������ ���ϰ� ħ���ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[22].m_AppyType = DESIGNATION_APPLY_TYPE_MELLE;
	g_pGameContext->m_DesignationInfo[22].m_Type = DESIGNATION_TYPE_28;
	g_pGameContext->m_DesignationInfo[22].m_PlusEffect[0] = DESIGNATION_ID_MELEE_RANGE_INCREASE_1;
	g_pGameContext->m_DesignationInfo[22].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[22].m_MinusEffect[0] = DESIGNATION_ID_DAMAGE_MINUS_1;
	g_pGameContext->m_DesignationInfo[22].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[22].m_AddEffect[0] = DESIGNATION_ID_GET_AMOK_KUKRI;
	g_pGameContext->m_DesignationInfo[22].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[33].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO33));/*ħ�� ���ְ�*/
	sprintf( g_pGameContext->m_DesignationInfo[33].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO33));/*Infiltrator Commander*/
	sprintf( g_pGameContext->m_DesignationInfo[33].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO33));/*�ͽŰ��� ������ ��� ���� �Ѿư��� ����*/
	g_pGameContext->m_DesignationInfo[33].m_AppyType = DESIGNATION_APPLY_TYPE_MELLE;
	g_pGameContext->m_DesignationInfo[33].m_Type = DESIGNATION_TYPE_29;
	g_pGameContext->m_DesignationInfo[33].m_PlusEffect[0] = DESIGNATION_ID_MELEE_RANGE_INCREASE_1;
	g_pGameContext->m_DesignationInfo[33].m_PlusEffect[1] = DESIGNATION_ID_DAMAGE_PLUS_2;
	g_pGameContext->m_DesignationInfo[33].m_MinusEffect[0] = DESIGNATION_ID_MOVE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[33].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[33].m_AddEffect[0] = DESIGNATION_ID_GET_MINI_AXE;
	g_pGameContext->m_DesignationInfo[33].m_AddEffect[1] = DESIGNATION_ID_GET_SLOT3_B;

	// E Group
    sprintf( g_pGameContext->m_DesignationInfo[9].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO9));/*�Ϲ� ���ݺ�*/
	sprintf( g_pGameContext->m_DesignationInfo[9].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO9));/*Sniper*/
	sprintf( g_pGameContext->m_DesignationInfo[9].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO9));/*�����ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[9].m_AppyType = DESIGNATION_APPLY_TYPE_RIFLE;
	g_pGameContext->m_DesignationInfo[9].m_Type = DESIGNATION_TYPE_14;
	g_pGameContext->m_DesignationInfo[9].m_PlusEffect[0] = DESIGNATION_ID_SCOOPE_RANGE_PLUS_1;
	g_pGameContext->m_DesignationInfo[9].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[9].m_MinusEffect[0] = DESIGNATION_ID_FIRE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[9].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[9].m_AddEffect[0] = DESIGNATION_ID_GET_DRAGUNOV_G;
	g_pGameContext->m_DesignationInfo[9].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[16].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO16));/*���� ���ݺ�*/
	sprintf( g_pGameContext->m_DesignationInfo[16].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO16));/*Junior Sniper*/
	sprintf( g_pGameContext->m_DesignationInfo[16].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO16));/*���� ���߷��� ���̴� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[16].m_AppyType = DESIGNATION_APPLY_TYPE_RIFLE;
	g_pGameContext->m_DesignationInfo[16].m_Type = DESIGNATION_TYPE_15;
	g_pGameContext->m_DesignationInfo[16].m_PlusEffect[0] = DESIGNATION_ID_FIRE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[16].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[16].m_MinusEffect[0] = DESIGNATION_ID_CROSS_RESTORE_TIME_MINUS_1;
	g_pGameContext->m_DesignationInfo[16].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[16].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[16].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[23].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO23));/*���� ���ݺ�*/
	sprintf( g_pGameContext->m_DesignationInfo[23].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO23));/*Senior Sniper*/
	sprintf( g_pGameContext->m_DesignationInfo[23].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO23));/*���� ��𼭳� ��ǥ�� ����Ʈ���� �ڿ��� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[23].m_AppyType = DESIGNATION_APPLY_TYPE_RIFLE;
	g_pGameContext->m_DesignationInfo[23].m_Type = DESIGNATION_TYPE_16;
	g_pGameContext->m_DesignationInfo[23].m_PlusEffect[0] = DESIGNATION_ID_CROSS_RESTORE_TIME_PLUS_1;
	g_pGameContext->m_DesignationInfo[23].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[23].m_MinusEffect[0] = DESIGNATION_ID_FIRE_RANGE_MINUS_1;
	g_pGameContext->m_DesignationInfo[23].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[23].m_AddEffect[0] = DESIGNATION_ID_GET_PSG1_S;
	g_pGameContext->m_DesignationInfo[23].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[29].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO29));/*Ư�� �����屳*/
	sprintf( g_pGameContext->m_DesignationInfo[29].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO29));/*Veteran Sniper*/
	sprintf( g_pGameContext->m_DesignationInfo[29].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO29));/*��ġ�� ������ ���� ��ݼ��� ���� �ڿ��� �ο��ϴ� ȣĪ */
	g_pGameContext->m_DesignationInfo[29].m_AppyType = DESIGNATION_APPLY_TYPE_RIFLE;
	g_pGameContext->m_DesignationInfo[29].m_Type = DESIGNATION_TYPE_17;
	g_pGameContext->m_DesignationInfo[29].m_PlusEffect[0] = DESIGNATION_ID_MOVE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[29].m_PlusEffect[1] = DESIGNATION_ID_SCOOPE_RANGE_PLUS_1;
	g_pGameContext->m_DesignationInfo[29].m_MinusEffect[0] = DESIGNATION_ID_FIRE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[29].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[29].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[29].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[34].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO34));/*���� ���ְ�*/
	sprintf( g_pGameContext->m_DesignationInfo[34].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO34));/*Snipe Commander*/
	sprintf( g_pGameContext->m_DesignationInfo[34].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO34));/*������ ������ �����ϴ� ������ �ʴ� ����*/
	g_pGameContext->m_DesignationInfo[34].m_AppyType = DESIGNATION_APPLY_TYPE_RIFLE;
	g_pGameContext->m_DesignationInfo[34].m_Type = DESIGNATION_TYPE_18;
	g_pGameContext->m_DesignationInfo[34].m_PlusEffect[0] = DESIGNATION_ID_CROSS_RESTORE_TIME_PLUS_1;
	g_pGameContext->m_DesignationInfo[34].m_PlusEffect[1] = DESIGNATION_ID_FIRE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[34].m_MinusEffect[0] = DESIGNATION_ID_SCOOPE_RANGE_MINUS_1;
	g_pGameContext->m_DesignationInfo[34].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[34].m_AddEffect[0] = DESIGNATION_ID_GET_L115A1;
	g_pGameContext->m_DesignationInfo[34].m_AddEffect[1] = DESIGNATION_ID_GET_SLOT3_C;

	sprintf( g_pGameContext->m_DesignationInfo[39].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO39));/*������ �ϻ���*/
	sprintf( g_pGameContext->m_DesignationInfo[39].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO39));/*Still Assassin*/
	sprintf( g_pGameContext->m_DesignationInfo[39].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO39));/*Į������ ������ ���� ��Ȯ���� ���� ��*/
	g_pGameContext->m_DesignationInfo[39].m_AppyType = DESIGNATION_APPLY_TYPE_RIFLE;
	g_pGameContext->m_DesignationInfo[39].m_Type = DESIGNATION_TYPE_19;
	g_pGameContext->m_DesignationInfo[39].m_PlusEffect[0] = DESIGNATION_ID_RELOAD_TIME_REDUCE_1;
	g_pGameContext->m_DesignationInfo[39].m_PlusEffect[1] = DESIGNATION_ID_FIRE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[39].m_MinusEffect[0] = DESIGNATION_ID_MOVE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[39].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[39].m_AddEffect[0] = DESIGNATION_ID_GET_BERET;
	g_pGameContext->m_DesignationInfo[39].m_AddEffect[1] = DESIGNATION_ID_NONE;

	// F Group
	sprintf( g_pGameContext->m_DesignationInfo[10].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO10));/*�Ϲݻ��*/
	sprintf( g_pGameContext->m_DesignationInfo[10].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO10));/*Gunner*/
	sprintf( g_pGameContext->m_DesignationInfo[10].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO10));/*��� �ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[10].m_AppyType = DESIGNATION_APPLY_TYPE_HANDGUN;
	g_pGameContext->m_DesignationInfo[10].m_Type = DESIGNATION_TYPE_30;
	g_pGameContext->m_DesignationInfo[10].m_PlusEffect[0] = DESIGNATION_ID_FIRE_RANGE_PLUS_1;
	g_pGameContext->m_DesignationInfo[10].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[10].m_MinusEffect[0] = DESIGNATION_ID_MOVE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[10].m_MinusEffect[1] = DESIGNATION_ID_FIRE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[10].m_AddEffect[0] = DESIGNATION_ID_GET_CIC;
	g_pGameContext->m_DesignationInfo[10].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[17].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO17));/*�������*/
	sprintf( g_pGameContext->m_DesignationInfo[17].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO17));/*Senior Gunner*/
	sprintf( g_pGameContext->m_DesignationInfo[17].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO17));/*����� ��ݼ��� ���� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[17].m_AppyType = DESIGNATION_APPLY_TYPE_HANDGUN;
	g_pGameContext->m_DesignationInfo[17].m_Type = DESIGNATION_TYPE_31;
	g_pGameContext->m_DesignationInfo[17].m_PlusEffect[0] = DESIGNATION_ID_MOVE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[17].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[17].m_MinusEffect[0] = DESIGNATION_ID_FIRE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[17].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[17].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[17].m_AddEffect[1] = DESIGNATION_ID_NONE;
	
	sprintf( g_pGameContext->m_DesignationInfo[24].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO24));/*Ư�� ����屳*/
	sprintf( g_pGameContext->m_DesignationInfo[24].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO24));/*Veteran Gun Officer*/
	sprintf( g_pGameContext->m_DesignationInfo[24].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO24));/*������ ��տ����� �ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[24].m_AppyType = DESIGNATION_APPLY_TYPE_HANDGUN;
	g_pGameContext->m_DesignationInfo[24].m_Type = DESIGNATION_TYPE_32;
	g_pGameContext->m_DesignationInfo[24].m_PlusEffect[0] = DESIGNATION_ID_TARGET_DECLINATION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[24].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[24].m_MinusEffect[0] = DESIGNATION_ID_MOVE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[24].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[24].m_AddEffect[0] = DESIGNATION_ID_GET_DUAL_HANDGUN;
	g_pGameContext->m_DesignationInfo[24].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[35].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO35));/*��� ���ְ�*/
	sprintf( g_pGameContext->m_DesignationInfo[35].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO35));/*Shot Commander*/
	sprintf( g_pGameContext->m_DesignationInfo[35].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO35));/*������ �Ҳ��� ������  ����*/
	g_pGameContext->m_DesignationInfo[35].m_AppyType = DESIGNATION_APPLY_TYPE_HANDGUN;
	g_pGameContext->m_DesignationInfo[35].m_Type = DESIGNATION_TYPE_33;
	g_pGameContext->m_DesignationInfo[35].m_PlusEffect[0] = DESIGNATION_ID_MOVE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[35].m_PlusEffect[1] = DESIGNATION_ID_FIRE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[35].m_MinusEffect[0] = DESIGNATION_ID_FIRE_RANGE_MINUS_1;
	g_pGameContext->m_DesignationInfo[35].m_MinusEffect[1] = DESIGNATION_ID_TARGET_DECLINATION_INCREASE_1;
	g_pGameContext->m_DesignationInfo[35].m_AddEffect[0] = DESIGNATION_ID_GET_DUAL_DESERTEAGLE;
	g_pGameContext->m_DesignationInfo[35].m_AddEffect[1] = DESIGNATION_ID_GET_SLOT3_D;

	sprintf( g_pGameContext->m_DesignationInfo[40].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO40));/*������ ����*/
	sprintf( g_pGameContext->m_DesignationInfo[40].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO40));/*Shooting Star*/
	sprintf( g_pGameContext->m_DesignationInfo[40].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO40));/*�ѱ��� �Ҳ����� ������ �Ǻ񸰳��� ǳ��� ��*/
	g_pGameContext->m_DesignationInfo[40].m_AppyType = DESIGNATION_APPLY_TYPE_HANDGUN;
	g_pGameContext->m_DesignationInfo[40].m_Type = DESIGNATION_TYPE_34;
	g_pGameContext->m_DesignationInfo[40].m_PlusEffect[0] = DESIGNATION_ID_RELOAD_TIME_REDUCE_1;
	g_pGameContext->m_DesignationInfo[40].m_PlusEffect[1] = DESIGNATION_ID_FIRE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[40].m_MinusEffect[0] = DESIGNATION_ID_MOVE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[40].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[40].m_AddEffect[0] = DESIGNATION_ID_GET_BERET;
	g_pGameContext->m_DesignationInfo[40].m_AddEffect[1] = DESIGNATION_ID_NONE;

	// G Group
	sprintf( g_pGameContext->m_DesignationInfo[11].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO11));/*�Ϲ� ���ݴ��*/
	sprintf( g_pGameContext->m_DesignationInfo[11].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO11));/*Assailant*/
	sprintf( g_pGameContext->m_DesignationInfo[11].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO11));/*���� �ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[11].m_AppyType = DESIGNATION_APPLY_TYPE_SMG;
	g_pGameContext->m_DesignationInfo[11].m_Type = DESIGNATION_TYPE_20;
	g_pGameContext->m_DesignationInfo[11].m_PlusEffect[0] = DESIGNATION_ID_HORIZONTAL_REACTION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[11].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[11].m_MinusEffect[0] = DESIGNATION_ID_TARGET_DECLINATION_INCREASE_1;
	g_pGameContext->m_DesignationInfo[11].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[11].m_AddEffect[0] = DESIGNATION_ID_GET_MP5K_G;
	g_pGameContext->m_DesignationInfo[11].m_AddEffect[1] = DESIGNATION_ID_GET_SPECTRE_W;

	sprintf( g_pGameContext->m_DesignationInfo[18].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO18));/*���� ���ݴ��*/
	sprintf( g_pGameContext->m_DesignationInfo[18].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO18));/*Junior Assailant*/
	sprintf( g_pGameContext->m_DesignationInfo[18].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO18));/*����� ���������� ���� �ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[18].m_AppyType = DESIGNATION_APPLY_TYPE_SMG;
	g_pGameContext->m_DesignationInfo[18].m_Type = DESIGNATION_TYPE_21;
	g_pGameContext->m_DesignationInfo[18].m_PlusEffect[0] = DESIGNATION_ID_VERTICAL_REACTION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[18].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[18].m_MinusEffect[0] = DESIGNATION_ID_MOVE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[18].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[18].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[18].m_AddEffect[1] = DESIGNATION_ID_NONE;
	
	sprintf( g_pGameContext->m_DesignationInfo[25].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO25));/*���� ���ݴ��*/
	sprintf( g_pGameContext->m_DesignationInfo[25].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO25));/*Senior Assailant*/
	sprintf( g_pGameContext->m_DesignationInfo[25].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO25));/*������ �����ӹ��� �ϼ��� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[25].m_AppyType = DESIGNATION_APPLY_TYPE_SMG;
	g_pGameContext->m_DesignationInfo[25].m_Type = DESIGNATION_TYPE_22;
	g_pGameContext->m_DesignationInfo[25].m_PlusEffect[0] = DESIGNATION_ID_TARGET_DECLINATION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[25].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[25].m_MinusEffect[0] = DESIGNATION_ID_VERTICAL_REACTION_INCREASE_1;
	g_pGameContext->m_DesignationInfo[25].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[25].m_AddEffect[0] = DESIGNATION_ID_GET_P90_EXT;
	g_pGameContext->m_DesignationInfo[25].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[30].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO30));/*Ư�� �����屳*/
	sprintf( g_pGameContext->m_DesignationInfo[30].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO30));/*Veteran Assail Officer*/
	sprintf( g_pGameContext->m_DesignationInfo[30].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO30));/*���� �ӹ��� ������ ���� ���� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[30].m_AppyType = DESIGNATION_APPLY_TYPE_SMG;
	g_pGameContext->m_DesignationInfo[30].m_Type = DESIGNATION_TYPE_23;
	g_pGameContext->m_DesignationInfo[30].m_PlusEffect[0] = DESIGNATION_ID_HORIZONTAL_REACTION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[30].m_PlusEffect[1] = DESIGNATION_ID_VERTICAL_REACTION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[30].m_MinusEffect[0] = DESIGNATION_ID_FIRE_RANGE_MINUS_1;
	g_pGameContext->m_DesignationInfo[30].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[30].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[30].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[36].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO36));/*���� ���ְ�*/
	sprintf( g_pGameContext->m_DesignationInfo[36].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO36));/*Assail Commander*/
	sprintf( g_pGameContext->m_DesignationInfo[36].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO36));/*����� ������ �������� ������ �����ϴ� ����*/
	g_pGameContext->m_DesignationInfo[36].m_AppyType = DESIGNATION_APPLY_TYPE_SMG;
	g_pGameContext->m_DesignationInfo[36].m_Type = DESIGNATION_TYPE_24;
	g_pGameContext->m_DesignationInfo[36].m_PlusEffect[0] = DESIGNATION_ID_FIRE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[36].m_PlusEffect[1] = DESIGNATION_ID_MOVE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[36].m_MinusEffect[0] = DESIGNATION_ID_VERTICAL_REACTION_INCREASE_1;
	g_pGameContext->m_DesignationInfo[36].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[36].m_AddEffect[0] = DESIGNATION_ID_GET_KRISS_SUPER_V;
	g_pGameContext->m_DesignationInfo[36].m_AddEffect[1] = DESIGNATION_ID_GET_SLOT3_E;

	sprintf( g_pGameContext->m_DesignationInfo[41].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO41));/*��Ȱ�� ħ����*/
	sprintf( g_pGameContext->m_DesignationInfo[41].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO41));/*Nimble Sneaker*/
	sprintf( g_pGameContext->m_DesignationInfo[41].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO41));/*õ��ó�� ������ �޷���� ��� ���� �������� �ٲ� ���� ��*/
	g_pGameContext->m_DesignationInfo[41].m_AppyType = DESIGNATION_APPLY_TYPE_SMG;
	g_pGameContext->m_DesignationInfo[41].m_Type = DESIGNATION_TYPE_25;
	g_pGameContext->m_DesignationInfo[41].m_PlusEffect[0] = DESIGNATION_ID_RELOAD_TIME_REDUCE_1;
	g_pGameContext->m_DesignationInfo[41].m_PlusEffect[1] = DESIGNATION_ID_TARGET_DECLINATION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[41].m_MinusEffect[0] = DESIGNATION_ID_MOVE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[41].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[41].m_AddEffect[0] = DESIGNATION_ID_GET_BERET;
	g_pGameContext->m_DesignationInfo[41].m_AddEffect[1] = DESIGNATION_ID_NONE;

	// H Group
	sprintf( g_pGameContext->m_DesignationInfo[12].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO12));/*�Ϲ� Ÿ�ݴ��*/
	sprintf( g_pGameContext->m_DesignationInfo[12].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO12));/*Blower*/
	sprintf( g_pGameContext->m_DesignationInfo[12].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO12));/*Ÿ�� �ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[12].m_AppyType = DESIGNATION_APPLY_TYPE_SHOTGUN;
	g_pGameContext->m_DesignationInfo[12].m_Type = DESIGNATION_TYPE_35;
	g_pGameContext->m_DesignationInfo[12].m_PlusEffect[0] = DESIGNATION_ID_FIRE_RANGE_PLUS_1;
	g_pGameContext->m_DesignationInfo[12].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[12].m_MinusEffect[0] = DESIGNATION_ID_RELOAD_TIME_INCREASE_1;
	g_pGameContext->m_DesignationInfo[12].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[12].m_AddEffect[0] = DESIGNATION_ID_GET_870MCS_W;
	g_pGameContext->m_DesignationInfo[12].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[19].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO19));/*���� Ÿ�ݴ��*/
	sprintf( g_pGameContext->m_DesignationInfo[19].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO19));/*Senior Blower*/
	sprintf( g_pGameContext->m_DesignationInfo[19].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO19));/*Ÿ�� �ӹ��� ������ �ɷ��� ���� ������� �ο��ϴ� ȣĪ */
	g_pGameContext->m_DesignationInfo[19].m_AppyType = DESIGNATION_APPLY_TYPE_SHOTGUN;
	g_pGameContext->m_DesignationInfo[19].m_Type = DESIGNATION_TYPE_36;
	g_pGameContext->m_DesignationInfo[19].m_PlusEffect[0] = DESIGNATION_ID_MOVE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[19].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[19].m_MinusEffect[0] = DESIGNATION_ID_TARGET_DECLINATION_INCREASE_1;
	g_pGameContext->m_DesignationInfo[19].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[19].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[19].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[26].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO26));/*Ư�� Ÿ���屳*/
	sprintf( g_pGameContext->m_DesignationInfo[26].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO26));/*Veteran Blower*/
	sprintf( g_pGameContext->m_DesignationInfo[26].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO26));/*�η��� ���� �ҵ��� ���� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[26].m_AppyType = DESIGNATION_APPLY_TYPE_SHOTGUN;
	g_pGameContext->m_DesignationInfo[26].m_Type = DESIGNATION_TYPE_37;
	g_pGameContext->m_DesignationInfo[26].m_PlusEffect[0] = DESIGNATION_ID_TARGET_DECLINATION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[26].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[26].m_MinusEffect[0] = DESIGNATION_ID_MOVE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[26].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[26].m_AddEffect[0] = DESIGNATION_ID_GET_SPAS15;
	g_pGameContext->m_DesignationInfo[26].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[37].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO37));/*Ÿ�� ���ְ�*/
	sprintf( g_pGameContext->m_DesignationInfo[37].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO37));/*Blow Commander*/
	sprintf( g_pGameContext->m_DesignationInfo[37].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO37));/*���� Ÿ���� ������ �����ϴ� ������ ����*/
	g_pGameContext->m_DesignationInfo[37].m_AppyType = DESIGNATION_APPLY_TYPE_SHOTGUN;
	g_pGameContext->m_DesignationInfo[37].m_Type = DESIGNATION_TYPE_38;
	g_pGameContext->m_DesignationInfo[37].m_PlusEffect[0] = DESIGNATION_ID_TARGET_DECLINATION_REDUCE_1;
	g_pGameContext->m_DesignationInfo[37].m_PlusEffect[1] = DESIGNATION_ID_MOVE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[37].m_MinusEffect[0] = DESIGNATION_ID_FIRE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[37].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[37].m_AddEffect[0] = DESIGNATION_ID_GET_SOMETHING;
	g_pGameContext->m_DesignationInfo[37].m_AddEffect[1] = DESIGNATION_ID_GET_SLOT3_F;

	sprintf( g_pGameContext->m_DesignationInfo[42].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO42));/*������ �ı���*/
	sprintf( g_pGameContext->m_DesignationInfo[42].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO42));/*Supreme Buster*/
	sprintf( g_pGameContext->m_DesignationInfo[42].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO42));/*��ǳ���� ������ Ÿ������ ��ȥ���� �Ұ�  ����� ��*/
	g_pGameContext->m_DesignationInfo[42].m_AppyType = DESIGNATION_APPLY_TYPE_SHOTGUN;
	g_pGameContext->m_DesignationInfo[42].m_Type = DESIGNATION_TYPE_39;
	g_pGameContext->m_DesignationInfo[42].m_PlusEffect[0] = DESIGNATION_ID_RELOAD_TIME_REDUCE_1;
	g_pGameContext->m_DesignationInfo[42].m_PlusEffect[1] = DESIGNATION_ID_FIRE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[42].m_MinusEffect[0] = DESIGNATION_ID_FIRE_RANGE_MINUS_1;
	g_pGameContext->m_DesignationInfo[42].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[42].m_AddEffect[0] = DESIGNATION_ID_GET_BERET;
	g_pGameContext->m_DesignationInfo[42].m_AddEffect[1] = DESIGNATION_ID_NONE;

	// I Group
	sprintf( g_pGameContext->m_DesignationInfo[13].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO13));/*�Ϲ� ���Ĵ��*/
	sprintf( g_pGameContext->m_DesignationInfo[13].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO13));/*Blaster*/
	sprintf( g_pGameContext->m_DesignationInfo[13].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO13));/*���� �ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[13].m_AppyType = DESIGNATION_APPLY_TYPE_BOMB;
	g_pGameContext->m_DesignationInfo[13].m_Type = DESIGNATION_TYPE_40;
	g_pGameContext->m_DesignationInfo[13].m_PlusEffect[0] = DESIGNATION_ID_MOVE_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[13].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[13].m_MinusEffect[0] = DESIGNATION_ID_BOMB_RANGE_MINUS_1;
	g_pGameContext->m_DesignationInfo[13].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[13].m_AddEffect[0] = DESIGNATION_ID_GET_C5;
	g_pGameContext->m_DesignationInfo[13].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[20].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO20));/*���� ���Ĵ��*/
	sprintf( g_pGameContext->m_DesignationInfo[20].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO20));/*Junior Blaster*/
	sprintf( g_pGameContext->m_DesignationInfo[20].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO20));/*������ ���������� ���� �ӹ��� �����ϴ� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[20].m_AppyType = DESIGNATION_APPLY_TYPE_BOMB;
	g_pGameContext->m_DesignationInfo[20].m_Type = DESIGNATION_TYPE_41;
	g_pGameContext->m_DesignationInfo[20].m_PlusEffect[0] = DESIGNATION_ID_THROW_SPEED_PLUS_1;
	g_pGameContext->m_DesignationInfo[20].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[20].m_MinusEffect[0] = DESIGNATION_ID_BOMB_TIME_INCREASE_1;
	g_pGameContext->m_DesignationInfo[20].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[20].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[20].m_AddEffect[1] = DESIGNATION_ID_NONE;
	
	sprintf( g_pGameContext->m_DesignationInfo[27].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO27));/*���� ���Ĵ��*/
	sprintf( g_pGameContext->m_DesignationInfo[27].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO27));/*Senior Blaster*/
	sprintf( g_pGameContext->m_DesignationInfo[27].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO27));/*������ ���� ������ ���� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[27].m_AppyType = DESIGNATION_APPLY_TYPE_BOMB;
	g_pGameContext->m_DesignationInfo[27].m_Type = DESIGNATION_TYPE_42;
	g_pGameContext->m_DesignationInfo[27].m_PlusEffect[0] = DESIGNATION_ID_BOMB_TIME_REDUCE_1;
	g_pGameContext->m_DesignationInfo[27].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[27].m_MinusEffect[0] = DESIGNATION_ID_MOVE_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[27].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[27].m_AddEffect[0] = DESIGNATION_ID_GET_WP_SMOKE;
	g_pGameContext->m_DesignationInfo[27].m_AddEffect[1] = DESIGNATION_ID_NONE;

	sprintf( g_pGameContext->m_DesignationInfo[31].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO31));/*Ư�� �����屳*/
	sprintf( g_pGameContext->m_DesignationInfo[31].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO31));/*Veteran Blaster*/
	sprintf( g_pGameContext->m_DesignationInfo[31].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO31));/*���ı���� õ������ ����� ���� ������� �ο��ϴ� ȣĪ*/
	g_pGameContext->m_DesignationInfo[31].m_AppyType = DESIGNATION_APPLY_TYPE_BOMB;
	g_pGameContext->m_DesignationInfo[31].m_Type = DESIGNATION_TYPE_43;	
	g_pGameContext->m_DesignationInfo[31].m_PlusEffect[0] = DESIGNATION_ID_BOMB_RANGE_PLUS_1;
	g_pGameContext->m_DesignationInfo[31].m_PlusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[31].m_MinusEffect[0] = DESIGNATION_ID_BOMB_TIME_INCREASE_1;
	g_pGameContext->m_DesignationInfo[31].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[31].m_AddEffect[0] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[31].m_AddEffect[1] = DESIGNATION_ID_NONE;
	
	sprintf( g_pGameContext->m_DesignationInfo[43].szName, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME_NO43));/*���� ���ְ�*/
	sprintf( g_pGameContext->m_DesignationInfo[43].szName_E, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_NAME2_NO43));/*Blast Commander*/
	sprintf( g_pGameContext->m_DesignationInfo[43].szExplain, STR_TBL_DESIGNATION(STR_TBL_DESIGNATION_EXPLAIN_NO43));/* �Ǹ��� ���� ������ ���� �ٴϴ� ������ â����*/
	g_pGameContext->m_DesignationInfo[43].m_AppyType = DESIGNATION_APPLY_TYPE_BOMB;
	g_pGameContext->m_DesignationInfo[43].m_Type = DESIGNATION_TYPE_44;
	g_pGameContext->m_DesignationInfo[43].m_PlusEffect[0] = DESIGNATION_ID_BOMB_RANGE_PLUS_1;
	g_pGameContext->m_DesignationInfo[43].m_PlusEffect[1] = DESIGNATION_ID_BOMB_TIME_REDUCE_1;
	g_pGameContext->m_DesignationInfo[43].m_MinusEffect[0] = DESIGNATION_ID_THROW_SPEED_MINUS_1;
	g_pGameContext->m_DesignationInfo[43].m_MinusEffect[1] = DESIGNATION_ID_NONE;
	g_pGameContext->m_DesignationInfo[43].m_AddEffect[0] = DESIGNATION_ID_GET_SOMETHING;
	g_pGameContext->m_DesignationInfo[43].m_AddEffect[1] = DESIGNATION_ID_GET_SLOT3_G;
}

#include "pch.h"
#include "MCardQuest.h"

#include "MCardQuestInfo.h"

#include "MCardUtil.h"

#include "ClientStageInfoUtil.h"

MCardQuest::MCardQuest(const MCard* pCard, INT32 myIndex) : 
m_pParent(pCard), m_myIndex(myIndex), m_currCompleteCount_OffBattle(0), m_currCompleteCount_OnBattle(0), m_pCardQuestInfo(nullptr)
{

}

MCardQuest::~MCardQuest()
{
	//
}

void		MCardQuest::Activate(const MCardQuestInfo* pCardQuestInfo)
{
	m_pCardQuestInfo = pCardQuestInfo;
	m_currCompleteCount_OffBattle = 0;	
	m_currCompleteCount_OnBattle = 0;
}

void		MCardQuest::Deactivate()
{
	MCardQuest::Activate(nullptr);
}

void		MCardQuest::SetCompleteCount_OffBattle(INT32 completeCount)
{
	m_currCompleteCount_OffBattle = completeCount;
	m_currCompleteCount_OnBattle = completeCount;
}

void		MCardQuest::SetCompleteCount_OnBattle(INT32 completeCount) 
{ 
	m_currCompleteCount_OnBattle = completeCount;  
}

void	MCardQuest::CancelCompletion()
{
	m_currCompleteCount_OnBattle = m_currCompleteCount_OffBattle;
}

void	MCardQuest::ConfirmCompletion()
{
	m_currCompleteCount_OffBattle = m_currCompleteCount_OnBattle;
}

namespace
{
	enum QuestDispatchType
	{
		QuestDispatchType_Team,
		QuestDispatchType_Weapon_Accum,
		QuestDispatchType_Weapon_Cont,
	};

}

// ���� v1.0 �ڵ�� ����Ʈ �Ļ�Ŭ���� 3���� �бⰡ ���ִµ�...�бⰡ �ξ� �� �������ϰ� �ݺ��� ���ؼ�..�׳� �ϳ��� ��ģ��.

i3::rc_wstring	MCardQuest::GetCurrHUDDesc(bool bLineFeed) const
{
	if (m_pCardQuestInfo == nullptr)
		return i3::rc_wstring();

	bool bTeamCond = ( m_pCardQuestInfo->m_Team == TEAM_BLUE || m_pCardQuestInfo->m_Team == TEAM_RED );
	bool bAccumCond = m_pCardQuestInfo->m_bAccum;
	bool bEnterIntercept = ( m_pCardQuestInfo->m_ActionID == ACTION_ID_STAGE_ENTER || m_pCardQuestInfo->m_ActionID == ACTION_ID_STAGE_INTERCEPT);
	bool bKillCond = ( m_pCardQuestInfo->m_ActionID == ACTION_ID_KILL_WEAPONCLASS ||
		m_pCardQuestInfo->m_ActionID == 	ACTION_ID_DOUBLE_KILL_WEAPONCLASS ||
		m_pCardQuestInfo->m_ActionID == 	ACTION_ID_TRIPLE_KILL_WEAPONCLASS );

	i3::stack_wstring wstrDesc;
	i3::stack_wstring wstrTemp;

	// ������ ����ǥ�ÿ��� ��������, Accum����, ų �������� �� �߱����� ����ó���̴�..
#if defined(LOCALE_CHINA)
	if (bTeamCond == false && bAccumCond == true && bKillCond == true)
	{
		wstrDesc = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC13");/*(����) */
		wstrDesc += GAME_RCSTRING("STR_HEADER_DO_TRIPLE_KILL_ENEMY_CN"); 

		i3::sprintf( wstrTemp, L"(%s)", g_pWeaponInfoDataBase->GetWeaponClass( m_pCardQuestInfo->m_WeaponClassType ) );
		wstrDesc += wstrTemp;
		wstrDesc += GAME_RCSTRING("STR_HEADER_DO_ENEMY_CN");

		i3::rc_wstring wstrActionName =	MCardUtil::FindMCardActionName(m_pCardQuestInfo->m_ActionID);

		wstrDesc += wstrActionName;
	}		
#else	// �� �߱� ������..
	if (g_pEnvSet->IsBaseOnUIVersion_1_0())		// 2�̸� 1.0 / 1�̸� 1.1 / 0�� 1.5
	{
		// Ư�� ����
		if ( bTeamCond )
		{
			if (bAccumCond)
				wstrDesc = GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC11");/*(����) */
			else
				wstrDesc = GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC10");/*(����) */
		}
		else	// �� �ƴϸ� ������..
		{
			if (bAccumCond)
				wstrDesc = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC13");	/*(����) */
			else
				wstrDesc = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC13");	/*(����) */
		}
	}

	// ��ġ	
	if ( m_pCardQuestInfo->m_StageID != STAGE_ID_NA)		// Ư�� ��..
	{
		CSI_MAP* map = CStageInfoUtil::GetMapData(m_pCardQuestInfo->m_StageID);
		if (map == nullptr)
		{
			I3PRINTLOG(I3LOG_WARN, "CSI_MAP nullptr StageID =  %d", m_pCardQuestInfo->m_StageID);
		}
		else
		{
			i3::rc_wstring szDisplayMapName = map->getDisplayName();

			if (bTeamCond)
			{
				if (GetCodePage() == I3_LANG_CODE_KOREAN)		// �� �� �κ��� ������ �����ε�..���� ���� ������ ����.
				{
					if (bEnterIntercept)
						i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC13"), szDisplayMapName); /*%s�� */
					else
						i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC12"), szDisplayMapName); /*%s���� */
				}
				else
				{
					wstrTemp = szDisplayMapName;
				}
			}
			else
				if (bAccumCond)		// ���� ����..
				{
					if (GetCodePage() == I3_LANG_CODE_KOREAN)		// �� �� �κ��� ������ �����ε�..���� ���� ������ ����.
					{
						if (bEnterIntercept)
							i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC15"), szDisplayMapName); /*%s�� */
						else
							i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC14"), szDisplayMapName); /*%s���� */
					}
					else
					{
						wstrTemp = szDisplayMapName;
					}
				}
				else	// ���� ����..
				{
					if (bEnterIntercept)
						i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC15"), szDisplayMapName);/*%s�� */
					else
						i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC14"), szDisplayMapName);/*%s���� */
				}

			wstrDesc += wstrTemp;
		}
	}

	// ������ ��� ���� ���� ǥ��
	if ( bTeamCond == false && m_pCardQuestInfo->m_WeaponClassType != WEAPON_CLASS_UNKNOWN)
	{
		if ( m_pCardQuestInfo->m_WeaponNumber == 0 )		// ���� �ѹ� ǥ�� ���ϸ� ������ ������� ��ü 
		{
			if( GetCodePage() == I3_LANG_CODE_KOREAN)
			{
				if (bKillCond)
					i3::sprintf(wstrTemp, L"%s%s ", g_pWeaponInfoDataBase->GetWeaponClass( m_pCardQuestInfo->m_WeaponClassType), getWeaponClassPostfix_Do( m_pCardQuestInfo->m_WeaponClassType));
				else
					i3::sprintf(wstrTemp, L"%s%s ", g_pWeaponInfoDataBase->GetWeaponClass( m_pCardQuestInfo->m_WeaponClassType), getWeaponClassPostfix_Use( m_pCardQuestInfo->m_WeaponClassType));
			}
			else
			{

#if defined(LOCALE_CHINA)
				i3::sprintf( wstrTemp, L"%s (%s) ", GAME_RCSTRING("STR_HEADER_DO_USE"), g_pWeaponInfoDataBase->GetWeaponClass( m_pCardQuestInfo->m_WeaponClassType ));
#else
				i3::sprintf( wstrTemp, L"(%s) ", g_pWeaponInfoDataBase->GetWeaponClass( m_pCardQuestInfo->m_WeaponClassType));
#endif
			}
		}
		else	//  m_pCardQuest->m_WeaponNumber != 0
		{
			CWeaponInfo* pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( m_pCardQuestInfo->m_WeaponClassType, m_pCardQuestInfo->m_WeaponNumber);
			if (pWeaponInfo != nullptr)
			{
				i3::rc_wstring wstrTemp1;
				pWeaponInfo->GetWeaponName( wstrTemp1);

				//	�� ���� ����� �˸°� ����.
				if( GetCodePage() == I3_LANG_CODE_KOREAN)
				{
					i3::rc_wstring wstrTemp2;
					if (m_pCardQuestInfo->m_ActionID == ACTION_ID_USE_WEAPON)
						pWeaponInfo->GetQuestPostfix_Do( wstrTemp2 );
					else
						pWeaponInfo->GetQuestPostfix_Use( wstrTemp2);

					i3::sprintf( wstrTemp, L"%s%s ", wstrTemp1, wstrTemp2);
				}
				else
				{
					i3::sprintf( wstrTemp, L"(%s) ", wstrTemp1);
				}
			}
			else	// pWeaponInfo == nullptr
			{
				wstrTemp = L"(Unknown) ";
			}

		}

		wstrDesc += wstrTemp;
	}

	// ���� ǥ��..
	if (bTeamCond)	// �� ������ ��� ������..
	{
		i3::rc_wstring wstrActionName = MCardUtil::FindMCardActionName( m_pCardQuestInfo->m_ActionID );
		wstrDesc += wstrActionName;
	}
	else // !bTeamCond	// ���� ������ ��� �ټ� �ڵ尡 �������� ����..
	{
#if defined ( LOCALE_THAILAND)		//�±�
		i3::rc_wstring wstrActionName = MCardUtil::FindMCardActionName( m_pCardQuestInfo->m_ActionID );
		wstrDesc += wstrActionName;

		if ( bKillCond )
		{
			wstrDesc += L' ';
			if (bAccumCond)
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC6");		/*������� */
			else
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC6");		/*������� */
		}	

#else	// �Ϲ� ( �׳� �±��� �ݴ�� ��� �� ����.)
		if (bKillCond)
		{
			if (bAccumCond)
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC6");	/*������� */
			else
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC6");	/*������� */
		}
#if !defined (LOCALE_RUSSIA)
		wstrDesc += L' '; // ��ĭ���
#endif
		i3::rc_wstring wstrActionName = MCardUtil::FindMCardActionName( m_pCardQuestInfo->m_ActionID );
		wstrDesc += wstrActionName;
#endif
	}

#endif	// �� �߱� ������..��..

	if( bLineFeed)
		wstrDesc += L'\n'; // ���ٶ��

	// 1.1 UI Ư�� ����.. (1.0���� ���� ��ܿ� ǥ���Ѵ�..)
	if (g_pEnvSet->m_nUIVersion == UFV_1_1)		// 2�̸� 1.0 / 1�̸� 1.1 / 0�� 1.5
	{
		if (bTeamCond)
		{
			if (bAccumCond)
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC11");	/*(����) */
			else
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC10");	/*(����) */
		}
		else	// ���� ����..
		{
			if (bAccumCond)
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC13"); /*(����) */
			else
				wstrDesc += GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC13"); /*(����) */
		}
	}
	//
	// Ƚ�� (������ ����)
	struct getProgressColor
	{
		void operator()( INT32 cur, INT32 maxCount, COLOR* out )
		{
			static UINT8 s_ColorTable[3][4] =
			{
				{	206, 206, 206, 255 },		// ���۵� ������ ��
				{	252, 255, 100, 255 },		// ���� ���� ��
				{	159, 252, 0, 255 }			// �Ϸ����� ��
			};

			if( cur <= 0.0f)
			{
				out->r = s_ColorTable[0][0];
				out->g = s_ColorTable[0][1];
				out->b = s_ColorTable[0][2];
				out->a = s_ColorTable[0][3];
			}
			else if( cur == maxCount )
			{
				out->r = s_ColorTable[2][0];
				out->g = s_ColorTable[2][1];
				out->b = s_ColorTable[2][2];
				out->a = s_ColorTable[2][3];
			}
			else
			{
				out->r = s_ColorTable[1][0];
				out->g = s_ColorTable[1][1];
				out->b = s_ColorTable[1][2];
				out->a = s_ColorTable[1][3];
			}
		}
	};

	// ���� �Ϸ� ī��Ʈ ���..
	{
		COLOR col;
		getProgressColor()( m_currCompleteCount_OnBattle, m_pCardQuestInfo->m_TargetCompleteCount, &col);

		i3::rc_wstring wstrFmtRate;
		if (bTeamCond)
			wstrFmtRate = GAME_RCSTRING("STR_TBL_QUEST_ACCM_TEAM_ETC9");
		else
			if (bAccumCond)
				wstrFmtRate = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC12");
			else
				wstrFmtRate = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_COUNT_ETC12");

		i3::sprintf(wstrTemp, wstrFmtRate, col.r, col.g, col.b, col.a, m_currCompleteCount_OnBattle, m_pCardQuestInfo->m_TargetCompleteCount);

		wstrDesc += wstrTemp;
	}

	return i3::rc_wstring(wstrDesc);
}

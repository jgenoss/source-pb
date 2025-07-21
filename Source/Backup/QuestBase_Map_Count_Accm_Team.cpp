#include "pch.h"
#include "QuestBase_Map_Count_Accm_Team.h"
#include "QPI.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( CQuestBase_Map_Count_Accm_Team);//, CQuestBase);

CQuestBase_Map_Count_Accm_Team::CQuestBase_Map_Count_Accm_Team(void)
{
	m_nValidKillCount = 0;
}

CQuestBase_Map_Count_Accm_Team::~CQuestBase_Map_Count_Accm_Team(void)
{
}

bool		CQuestBase_Map_Count_Accm_Team::_checkCondition(void)
{
	if( _getStage() != STAGE_ID_NA)
	{
		STAGE_ID id1, id2;

		id1 = _getStage();
		id2 = QPI_GetStage();

		if (getStageOrd(id1) != getStageOrd(id2))
			return false;
	}

	if( _getTeam() != 0)
	{
		if( g_pGameContext->getMyTeam() != _getTeam())
			return false;
	}

	return true;
}

// ���ΰ����� ����, �ٸ� ĳ���Ͱ� �׾��� �� ȣ��ȴ�.
// Kill Chara.�� ���� ��ü���� ������ KILL_INFO Structure�� ����Ǿ� �ִ�.
bool		CQuestBase_Map_Count_Accm_Team::OnKill( KILL_INFO * pKillInfo)
{
	switch( _getAction())
	{
		case ACTION_ID_KILL :
			{
				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;
			}break;
		case ACTION_ID_SUICIDE :
			{
				if( pKillInfo->m_CharaShotIdx != g_pGameContext->getMySlotIdx())
					return false;
			}
			break;
		case ACTION_ID_DOUBLE_KILL:
			{
				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;
			}
			break;
		case ACTION_ID_TRIPLE_KILL:
			{
				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;
			}
			break;

		default :
			return false;
	}

	if( _checkCondition() == false)
		return false;

	//action�� ���� ī��Ʈ�� ���� �ʿ��� ���
	UINT8 validKillCnt = _getValidKillCount();

	_setValidKillCount( validKillCnt + 1);

	bool valid = false;
	switch( _getAction())
	{
		case ACTION_ID_DOUBLE_KILL:
			{
				if(( validKillCnt < 2) && (_getValidKillCount() >= 2))
				{
					valid = true;
				}
			}
			break;
		case ACTION_ID_TRIPLE_KILL:
			{
				if(( validKillCnt < 3) && (_getValidKillCount() >= 3))
				{
					valid = true;
				}
			}
			break;
		default:
			{
			valid = true;
			break;
			}
	}

	if( !valid )	return false;

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	I3TRACE( "OnKill : %p : %d\n", this, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// �ٸ� ĳ���Ϳ� ���� ���ΰ� ĳ���Ͱ� ������ ������ ���� ȣ��ȴ�.
// Kill Chara�� ���� ��ü���� ������ KILL_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase_Map_Count_Accm_Team::OnDeath( KILL_INFO * pDeathInfo)
{
	if( _getAction() != ACTION_ID_DEATH)
		return false;

	if( _checkCondition() == false)
		return false;

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// �ɸ��Ͱ� ������ ������ �� ȣ��ȴ�.
// ���� ���뿡 ���� ��ü���� ������ DAMAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase_Map_Count_Accm_Team::OnDamage( DAMAGE_INFO * pDamageInfo)
{
	switch( _getAction())
	{
		case ACTION_ID_HELMET_PROTECTION :
			if( pDamageInfo->m_HitPart != HIT_CHARA_PART_HELMET)
				return false;

			break;

		default :
			return false;
	}

	if( _checkCondition() == false)
		return false;

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// ���Ӱ� Stage�� �������� �� ȣ��ȴ�.
// Deathmatch, Mission ���� ������ �����ϰ� �� Stage���� �ѹ��� ȣ��ȴ�.
// ���� �����ϴ� Stage�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase_Map_Count_Accm_Team::OnStageStart( STAGE_INFO * pStage)
{
	if( _checkCondition() == false)
		return false;

	switch( _getAction())
	{
		case ACTION_ID_STAGE_ENTER :
			break;

		case ACTION_ID_STAGE_INTERCEPT :
			if( QPI_IsIntercept() == FALSE)
				return false;

			break;

		default :
			return false;
	}

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// Stage�� ������ �� ȣ��ȴ�.
// Deathmatch, Mission ���� ������ �����ϰ� �� Stage���� �ѹ��� ȣ��ȴ�.
// ���� �����ϴ� Stage�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase_Map_Count_Accm_Team::OnStageEnd( STAGE_INFO * pStageInfo)
{
	bool bRet = true;	
	//����ī���� ����Ʈ�� ���� stageend���� ����Ʈ�̿Ϸ��Ǵܽ� ����ī��Ʈ�� 0���� ������־���Ѵ�. 
	//�װ��� ���� �Ǵܿ� -raja

	if( _checkCondition() == false)
		bRet = false;

	switch( _getAction())
	{
		//�� ��忡 ���� �¸��Ǻ����� Ʋ�������, ���ӽý��ۿ��� �Ѱ��ִ� �¸������� �״�� ��� - raja
		case ACTION_ID_WIN :
			if( g_pGameContext->getMyTeam() != pStageInfo->m_WinnerTeam)
			{
				bRet = false;
			}
			break;

		case ACTION_ID_DEFEAT :
			if( g_pGameContext->getMyTeam() == pStageInfo->m_WinnerTeam)
			{
				bRet = false;
			}
			break;

		case ACTION_ID_HIT_HEAD_20 :
		case ACTION_ID_HIT_HEAD_30 :
		case ACTION_ID_HIT_CHEST_20 :
		case ACTION_ID_HIT_CHEST_30 :
		case ACTION_ID_HIT_ARM_20 :
		case ACTION_ID_HIT_ARM_30 :
		case ACTION_ID_HIT_LEG_20 :
		case ACTION_ID_HIT_LEG_30 :
			break;

		case ACTION_ID_ENFORCE_EXIT :
			break;
			
		case ACTION_ID_RANK_1:							// ���� ��� ���� 1
		case ACTION_ID_RANK_2:							// ���� ��� ���� 2
		case ACTION_ID_RANK_3:							// ���� ��� ���� 3
		case ACTION_ID_RANK_4:							// ���� ��� ���� 4
		case ACTION_ID_RANK_5:							// ���� ��� ���� 5
		case ACTION_ID_RANK_6:							// ���� ��� ���� 6
		case ACTION_ID_RANK_7:							// ���� ��� ���� 7
		case ACTION_ID_RANK_8:							// ���� ��� ���� 8
			if( (_getAction() - ACTION_ID_RANK_1) != QPI_GetPlayerStageRank())
			{ 
				bRet = false;
			}
			break;
		default :
			bRet = false;
			break;
	}

	if( bRet)
	{
		UINT8 cnt = getArg( 5);

		cnt++;
		setArg( 5, cnt);

		if( cnt >= _getCount())
			return true;
	}

	if( !_isAccmQuest())
		setArg( 5, 0);

	return false;
}

// ���Ӱ� Round�� �������� �� ȣ��ȴ�.
// Deathmatch ��� ���� Round�� Mission�� �ƴѰ�쿡�� ȣ����� ���� �� �ִ�.
// ���� �����ϴ� Round�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase_Map_Count_Accm_Team::OnRoundStart( STAGE_INFO * pStageInfo)
{
	return false;
}

// Round�� ������ �� ȣ��ȴ�.
// Deathmatch ��� ���� Round�� Mission�� �ƴѰ�쿡�� ȣ����� ���� �� �ִ�.
// ���� �����ϴ� Round�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase_Map_Count_Accm_Team::OnRoundEnd( STAGE_INFO * pStageInfo)
{
	return false;
}

// ���ΰ� ĳ���Ͱ� Ư�� ������ ������ �� ȣ��ȴ�.
// ���ۿ� ���� ��ü���� ������ ACTION_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase_Map_Count_Accm_Team::OnAction( ACTION_INFO * pAction)
{
	if( _getAction() != pAction->m_Action)
		return false;

	if( pAction->m_CharaSlotIdx != g_pGameContext->getMySlotIdx())
		return false;

	if( _checkCondition() == false)
		return false;

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	I3TRACE( "OnAction : %p : %d\n", this, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// ȭ�鿡 ����� Mission�� ���� ������ �䱸�� �� ȣ��ȴ�.
// CQuestBase Class���� �Ļ��� ���� Mission Class����
// �ݵ�� �� �Լ��� Override�Ͽ� ������ ������ ���ڿ��� �����ؾ� �Ѵ�.
void		CQuestBase_Map_Count_Accm_Team::OnQueryDesc( char * pszBuff, INT32 maxLen)
{
	char conv[256], temp[256];

	*pszBuff = 0;

	// ��ġ
	switch( _getStage())
	{
		case STAGE_ID_NA :				i3::safe_string_copy( temp, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC4"), 256 );/*���� ����*/	break;
		default :
		{
			char szTemp2[256];
			getStageDisplayName(szTemp2, _getStage());
			i3::safe_string_copy(temp, szTemp2, 256 );	
		}
		break;
	}
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC5"), temp);/*��ġ : %s\n \n*/
	strcat( pszBuff, conv);

	/*
	// ���
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC8"));//���      : ���� ����\n \n
	strcat( pszBuff, conv);

	// ����
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC3"));//����      : ���� ����\n \n
	strcat( pszBuff, conv);

	// ��� ����
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC6"));//��� ���� : ���� ����\n \n
	strcat( pszBuff, conv);
	*/

	// Ư�� ����
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC2"));/*Ư�� ����\n*/

	if( _isAccmQuest())
		i3::safe_string_copy( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC15"), 256 );/*   - Ƚ�� ���� ����\n \n*/
	else
		i3::safe_string_copy( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC14"), 256 );/*   - Ƚ�� ���� ����\n \n*/
	strcat( pszBuff, conv);

	// ����
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC7"));/*����\n*/
	if( _getAction() != ACTION_ID_NA)
		sprintf_s( conv, "   - %s\n \n", getActionName( _getAction()));
	else
		i3::safe_string_copy( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC16"), 256 );/*   - ����\n \n*/
	strcat( pszBuff, conv);

	// Ƚ��
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC1"));/*Ƚ��\n*/

	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC17"), getArg(5), _getCount());/*   - %d/%dȸ*/
	strcat( pszBuff, conv);
}

void		CQuestBase_Map_Count_Accm_Team::OnQueryHUDDesc( char * pszBuff, INT32 maxLen, bool bLineFeed)
{
	char conv[256];

	*pszBuff = 0;

	// ��ġ
	if( _getStage() != STAGE_ID_NA)
	{
		char szTemp[256];
		getStageDisplayName(szTemp, _getStage());

		//	�� ���� ����� �˸°� ����.
		if( GetCodePage() == I3_LANG_CODE_KOREAN)
		{
			

			if( _getAction() == ACTION_ID_STAGE_ENTER || _getAction() == ACTION_ID_STAGE_INTERCEPT)
			{
				

				sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC13"), szTemp);/*%s�� */
			}
			else
			{
				sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC12"), szTemp);/*%s���� */
			}
		}
		else
		{
			i3::safe_string_copy( conv, szTemp, 256 );
		}

		strcat( pszBuff, conv);
	}

	// ����
	sprintf_s( conv, "%s", getActionName( _getAction()));
	strcat( pszBuff, conv);

	if( bLineFeed)
	{
		strcat( pszBuff, "\n");
	}

	// Ư�� ����
	if( _isAccmQuest())
		strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC11"));/*(����) */
	else
		strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC10"));/*(����) */

	// Ƚ��
	REAL32 rate;
	rate = (REAL32) getArg(5) / _getCount();

	COLOR col;

	getProgressColor( getArg(5), _getCount(), &col);
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_ACCM_TEAM_ETC9"), col.r, col.g, col.b, col.a, getArg(5), _getCount());/*{col:%d,%d,%d,%d} %d/%dȸ{/col}*/
	strcat( pszBuff, conv);
}

// Quest�� ���� ���¸� ��ȯ�Ѵ�.
QUEST_STATE CQuestBase_Map_Count_Accm_Team::OnQueryState(void)
{
	if( getArg( 5) == 0)
	{
		return QST_NOT_STARTED;
	}
	else if( getArg( 5) == _getCount())
	{
		return QST_COMPLETED;
	}

	return QST_DOING;
}

bool	CQuestBase_Map_Count_Accm_Team::ForceComplete(void)
{
	setArg( 5, _getCount());

	return true;
}



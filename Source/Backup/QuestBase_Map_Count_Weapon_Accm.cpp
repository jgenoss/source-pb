#include "pch.h"
#include "QuestBase_Map_Count_Weapon_Accm.h"
#include "QPI.h"
#include "GlobalVariables.h"

#include "i3Base/string/ext/snprintf.h"


I3_CLASS_INSTANCE( CQuestBase_Map_Count_Weapon_Accm);//, CQuestBase);

CQuestBase_Map_Count_Weapon_Accm::CQuestBase_Map_Count_Weapon_Accm(void)
{
	_setValidKillCount( 0);
}

CQuestBase_Map_Count_Weapon_Accm::~CQuestBase_Map_Count_Weapon_Accm(void)
{
}

//
bool		CQuestBase_Map_Count_Weapon_Accm::_checkCondition(void)
{
	if( _getStage() != STAGE_ID_NA)
	{
		STAGE_ID id1, id2;

		id1 = _getStage();
		id2 = QPI_GetStage();

		if (getStageOrd(id1) != getStageOrd(id2))
			return false;
	}

	if(g_pGameContext->IsBossMissionMode() == false)
	{
		if( _getWeaponClass() != WEAPON_CLASS_UNKNOWN)
		{
			if( QPI_GetWeaponClassType() != _getWeaponClass())
				return false;
		}

		if( _getWeaponNumber() != 0)
		{
			if( QPI_GetWeaponType() != _getWeaponNumber())
				return false;
		}
	}

	return true;
}

//OnKill�� OnDeath, OnDamage���� �ٸ������� ó���� ���� �ʿ�
bool CQuestBase_Map_Count_Weapon_Accm::_checkCondition( WEAPON_CLASS_TYPE type, INT32 num)
{
	if( _getStage() != STAGE_ID_NA)
	{
		STAGE_ID id1, id2;

		id1 = _getStage();
		id2 = QPI_GetStage();

		if (getStageOrd(id1) != getStageOrd(id2))
			return false;
	}

	if( _getWeaponClass() != WEAPON_CLASS_UNKNOWN)
	{
		if( WEAPON_CLASS_CIC == type || WEAPON_CLASS_DUALHANDGUN == type)
		{
			if( _getWeaponNumber() == 0)
			{
				type = WEAPON_CLASS_HANDGUN;
			}
		}
		else if( WEAPON_CLASS_DUALKNIFE == type)
		{
			if( _getWeaponNumber() == 0)
			{
				type = WEAPON_CLASS_KNIFE;
			}
		}
		else if( WEAPON_CLASS_DUALSMG == type)
		{
			// Dual SMG �� ��� ������������ �Ϲ� SMG�� �ν� �ϵ���
			type = WEAPON_CLASS_SMG;
		}
		else
		{
			// ��ź���� �Ѿ���� Ŭ������ �߻��� �θ� ���� ������ ��ȯ 
			// ��ź���� ����ص� ���� �߻� ����� ����Ѱ� ó�� �������� �Ϸ��ؾ� �մϴ�. 
			// �ѿ��� ���ڴ� �߻� �θ� ���Ⱑ �ƴ� ��ź �ε����� �Ѿ���� ������ �����Ҷ��� ���� �߻� ���� �ε������� �������� üũ�� �� �� �ִ�.

			INT32 cnt = g_pWeaponInfoDataBase->GetExtWeaponInfoCount();

			// �ش� �ͽ��ټ� ���� ������ ���̺��� �˻�
			for (INT32 i=0; i<cnt; i++)
			{
				ExtWeaponInfoTable * pTable = g_pWeaponInfoDataBase->GetExtWeaponInfoTable(i);

				if( type == pTable->ExtClass && num == pTable->ExtNumber)
				{
					type = pTable->OwnerClass;	// ���� �߻� �θ� ���� �ε����� ��ȯ
					num = pTable->OwnerNumber;	// ���� �߻� �θ� ���� �ε����� ��ȯ

					break;
				}
			}
		}

		if( type != _getWeaponClass())
			return false;
	}

	if( _getWeaponNumber() != 0)
	{
		// if( QPI_GetWeapon_DerivedFrom( type, num) != QPI_GetWeapon_DerivedFrom( type, (INT32)_getWeaponNumber()))
		if(num != (INT32)_getWeaponNumber())
			return false;
	}

	return true;
}

// ���ΰ����� ����, �ٸ� ĳ���Ͱ� �׾��� �� ȣ��ȴ�.
// Kill Chara.�� ���� ��ü���� ������ KILL_INFO Structure�� ����Ǿ� �ִ�.
bool		CQuestBase_Map_Count_Weapon_Accm::OnKill( KILL_INFO * pKillInfo)
{
	switch( _getAction())
	{
		case ACTION_ID_HEADSHOT:
			{
				if( _checkCondition( pKillInfo->m_WeaponType, pKillInfo->m_Weapon) == false)
					return false;

				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;

				if( pKillInfo->m_HitPart != HIT_CHARA_PART_HEAD)
					return false;
			}break;
		case ACTION_ID_PIERCING:
			{
				if( _checkCondition( pKillInfo->m_WeaponType, pKillInfo->m_Weapon) == false)
					return false;

				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;

				if( pKillInfo->m_bPiercingShot == FALSE)
					return false;
			}break;
		case ACTION_ID_KILL :
		case ACTION_ID_DOUBLE_KILL:
		case ACTION_ID_TRIPLE_KILL:
			{
				if( _checkCondition( pKillInfo->m_WeaponType, pKillInfo->m_Weapon) == false)
					return false;

				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
					return false;
			}
			break;
		case ACTION_ID_SUICIDE :
			{
				if( _checkCondition( pKillInfo->m_WeaponType, pKillInfo->m_Weapon) == false)
					return false;

				if( pKillInfo->m_CharaShotIdx != g_pGameContext->getMySlotIdx())
					return false;
			}
			break;
		case ACTION_ID_KILL_WEAPONCLASS:
		case ACTION_ID_DOUBLE_KILL_WEAPONCLASS:
		case ACTION_ID_TRIPLE_KILL_WEAPONCLASS:
			{
				if( _checkCondition( pKillInfo->m_TargetWeaponType, pKillInfo->m_TargetWeapon) == false)
					return false;

				if( pKillInfo->m_CharaShotIdx == g_pGameContext->getMySlotIdx())
						return false;
			}
			break;
		default :
			return false;
	}

	UINT8 validKillCnt = _getValidKillCount();

	_setValidKillCount( validKillCnt + 1);

	bool valid = false;
	switch( _getAction())
	{
		case ACTION_ID_DOUBLE_KILL:
		case ACTION_ID_DOUBLE_KILL_WEAPONCLASS:
			{
				if(( validKillCnt < 2) && (_getValidKillCount() >= 2))
				{
					valid = true;
				}
			}
			break;
		case ACTION_ID_TRIPLE_KILL:
		case ACTION_ID_TRIPLE_KILL_WEAPONCLASS:
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
			}
			break;
	}

	if( !valid )	return false;

	UINT8 cnt = getArg( 5);

	cnt++;
	setArg( 5, cnt);

	if( cnt >= _getCount())
		return true;

	return false;
}

// �ٸ� ĳ���Ϳ� ���� ���ΰ� ĳ���Ͱ� ������ ������ ���� ȣ��ȴ�.
// Kill Chara�� ���� ��ü���� ������ KILL_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase_Map_Count_Weapon_Accm::OnDeath( KILL_INFO * pDeathInfo)
{
	_setValidKillCount(0);

	if( _getAction() != ACTION_ID_DEATH)
		return false;

	if( _checkCondition( pDeathInfo->m_WeaponType, pDeathInfo->m_Weapon) == false)
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
bool		CQuestBase_Map_Count_Weapon_Accm::OnDamage( DAMAGE_INFO * pDamageInfo)
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

	if( _checkCondition(pDamageInfo->m_WeaponType, pDamageInfo->m_Weapon) == false)
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
bool		CQuestBase_Map_Count_Weapon_Accm::OnStageStart( STAGE_INFO * pStage)
{
	_setValidKillCount(0);
	
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
bool		CQuestBase_Map_Count_Weapon_Accm::OnStageEnd( STAGE_INFO * pStageInfo)
{
	_setValidKillCount(0);

	if( _checkCondition() == false)
		return false;
	
	switch( _getAction())
	{
		case ACTION_ID_WIN :
			//�� ��忡 ���� �¸��Ǻ����� Ʋ�������, ���ӽý��ۿ��� �Ѱ��ִ� �¸������� �״�� ���- raja
			if( g_pGameContext->getMyTeam() != pStageInfo->m_WinnerTeam)//
			{
				return false;
			}
			break;

		case ACTION_ID_DEFEAT :
			if( g_pGameContext->getMyTeam() == pStageInfo->m_WinnerTeam)
			{
				return false;
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
				return false;
			}
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

// ���Ӱ� Round�� �������� �� ȣ��ȴ�.
// Deathmatch ��� ���� Round�� Mission�� �ƴѰ�쿡�� ȣ����� ���� �� �ִ�.
// ���� �����ϴ� Round�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase_Map_Count_Weapon_Accm::OnRoundStart( STAGE_INFO * pStageInfo)
{
	_setValidKillCount(0);

	return false;
}

// Round�� ������ �� ȣ��ȴ�.
// Deathmatch ��� ���� Round�� Mission�� �ƴѰ�쿡�� ȣ����� ���� �� �ִ�.
// ���� �����ϴ� Round�� ���� ��ü���� ������ STAGE_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase_Map_Count_Weapon_Accm::OnRoundEnd( STAGE_INFO * pStageInfo)
{
	_setValidKillCount(0);

	return false;
}

// ���ΰ� ĳ���Ͱ� Ư�� ������ ������ �� ȣ��ȴ�.
// ���ۿ� ���� ��ü���� ������ ACTION_INFO Structure�� ���� ���޵ȴ�.
bool		CQuestBase_Map_Count_Weapon_Accm::OnAction( ACTION_INFO * pAction)
{
	if( _getAction() != pAction->m_Action)
		return false;

	if( _getAction() == ACTION_ID_HEADSHOT || _getAction() == ACTION_ID_PIERCING)
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
void		CQuestBase_Map_Count_Weapon_Accm::OnQueryDesc( char * pszBuff, INT32 maxLen)
{
	char conv[256];
	i3::string strTemp;

	*pszBuff = 0;

	// ��ġ
	switch( _getStage())
	{
		case STAGE_ID_NA :				strTemp = GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC4");/*���� ����*/	break;
		default :						
		{
			char szTemp2[256];
			getStageDisplayName(szTemp2, _getStage());
			strTemp = szTemp2;	
		}
		break;
	}
	i3::snprintf( conv, 256, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC5"), strTemp);/*��ġ : %s\n \n*/
	strcat( pszBuff, conv);
/*
	// ���
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC11"));//���      : ���� ����\n \n
	strcat( pszBuff, conv);

	// ����
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC3"));//����      : ���� ����\n \n
	strcat( pszBuff, conv);
*/
	// ��� ����
	{
		i3::snprintf( conv, 256, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC7"));/*��� ���� : ���� ����\n \n*/

		if(_getWeaponClass() != WEAPON_CLASS_UNKNOWN)
		{
			sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC9"), g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()));/*��� ���� : %s �迭 */

			if(_getWeaponNumber() != 0)
			{
				CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo( _getWeaponClass(), _getWeaponNumber());

				if( pInfo != NULL)
				{
					pInfo->GetWeaponName( strTemp);
					i3::snprintf( conv, 256, GAME_RCSTRING("STR_TBL_QUEST_WEAPON_ACCM_ETC8"), strTemp);/*��� ���� : %s*/
				}
			}

			strcat( conv, "\n \n");
		}

		strcat( pszBuff, conv);
	}

	// Ư�� ����
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC2"));/*Ư�� ����\n*/

	i3::safe_string_copy( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC16"), 256 );/*   - Ƚ�� ���� ����\n \n*/
	strcat( pszBuff, conv);

	// ����
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC10"));/*����\n*/
	if( _getAction() != ACTION_ID_NA)
		sprintf_s( conv, "   - %s\n \n", getActionName( _getAction()));
	else
		i3::safe_string_copy( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC17"), 256 );/*   - ����\n \n*/
	strcat( pszBuff, conv);

	// Ƚ��
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC1"));/*Ƚ��\n*/

	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC18"), getArg(5), _getCount());/*   - %d/%dȸ*/
	strcat( pszBuff, conv);
}

void		CQuestBase_Map_Count_Weapon_Accm::OnQueryHUDDesc( char * pszBuff, INT32 maxLen, bool bLineFeed)
{
	char conv[256], temp2[MAX_STRING_COUNT];
	i3::string strTemp;

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
				sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC15"), szTemp);/*%s�� */
			}
			else
			{
				sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC14"), szTemp);/*%s���� */
			}
		}
		else
		{
			i3::safe_string_copy( conv, szTemp, 256 );
		}

		strcat( pszBuff, conv);
	}

	// ����
	if(_getWeaponClass() != WEAPON_CLASS_UNKNOWN)
	{
		//	�� ���� ����� �˸°� ����.
		if( GetCodePage() == I3_LANG_CODE_KOREAN)
		{
			if( _getAction() == ACTION_ID_KILL_WEAPONCLASS ||
				_getAction() == ACTION_ID_DOUBLE_KILL_WEAPONCLASS ||
				_getAction() == ACTION_ID_TRIPLE_KILL_WEAPONCLASS)
			{
				sprintf_s( conv, "%s%s ", g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()), getWeaponClassPostfix_Do( _getWeaponClass()));
			}
			else
			{
				sprintf_s( conv, "%s%s ", g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()), getWeaponClassPostfix_Use( _getWeaponClass()));
			}
		}
		else
		{
#if defined(LOCALE_CHINA)
			sprintf_s( conv, "%s (%s) ", GAME_STRING("STR_HEADER_DO_USE"),g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()));
#else
			sprintf_s( conv, "(%s) ", g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()));
#endif
		}

		if(_getWeaponNumber() != 0)
		{
			CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo( _getWeaponClass(), _getWeaponNumber());

			if( pInfo != NULL)
			{
				//	�� ���� ����� �˸°� ����.
				if( GetCodePage() == I3_LANG_CODE_KOREAN)
				{
					if( _getAction() == ACTION_ID_USE_WEAPON)
					{
						pInfo->GetWeaponName( strTemp);
						pInfo->GetQuestPostfix_Do( temp2);
						i3::snprintf( conv, 256, "%s%s ", strTemp, temp2);
					}
					else
					{
						pInfo->GetWeaponName( strTemp);
						pInfo->GetQuestPostfix_Use( temp2);
						i3::snprintf( conv, 256, "%s%s ", strTemp, temp2);
					}
				}
				else
				{
					pInfo->GetWeaponName( strTemp);
					i3::snprintf( conv, 256, "(%s) ", strTemp);
				}
			}
			else
			{
				sprintf_s( conv, "(%s) ", "Unknown");
			}
		}

		strcat( pszBuff, conv);

		if( _getAction() == ACTION_ID_KILL_WEAPONCLASS ||
			_getAction() == ACTION_ID_DOUBLE_KILL_WEAPONCLASS ||
			_getAction() == ACTION_ID_TRIPLE_KILL_WEAPONCLASS)
		{
			strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC6"));/*������� */
		}
	}

	// ����
	sprintf_s( conv, "%s", getActionName( _getAction()));
	strcat( pszBuff, conv);

#if defined(LOCALE_CHINA)
	if( _getAction() == ACTION_ID_KILL_WEAPONCLASS || _getAction() == ACTION_ID_DOUBLE_KILL_WEAPONCLASS || _getAction() == ACTION_ID_TRIPLE_KILL_WEAPONCLASS )
	{
		sprintf_s( pszBuff, maxLen, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC13"));/*(����) */

		sprintf_s( conv, GAME_STRING("STR_HEADER_DO_TRIPLE_KILL_ENEMY_CN"));
		strcat(pszBuff, conv);

		sprintf_s( conv, "(%s)", g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()));
		strcat(pszBuff, conv);

		sprintf_s( conv, GAME_STRING("STR_HEADER_DO_ENEMY_CN"));
		strcat(pszBuff, conv);

		sprintf_s( conv, "%s", getActionName( _getAction()));
		strcat(pszBuff, conv);

		//sprintf_s( pszBuff, "%s", getActionName( _getAction()));
		//sprintf_s( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC13"));/*(����) */
		//sprintf_s( conv, GAME_STRING(""))
		//sprintf_s( conv, GAME_STRING("STR_HEADER_DO_KILL_ENEMY_CN"),  g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()));
		//strcat(pszBuff, conv);

	}
	//else if(_getAction() == ACTION_ID_DOUBLE_KILL_WEAPONCLASS)
	//{
	//	sprintf_s( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC13"));/*(����) */
	//	sprintf_s(pszBuff, GAME_STRING("STR_HEADER__DO_DOUBLE_KILL_CN"),  g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()));
	//	strcat(pszBuff, conv);
	//}
	//else if(_getAction() == ACTION_ID_TRIPLE_KILL_WEAPONCLASS)	
	//{
	//	sprintf_s( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC13"));/*(����) */
	//	sprintf_s(pszBuff, GAME_STRING("STR_HEADER_DO_TRIPLE_KILL_ENEMY_CN"), g_pWeaponInfoDataBase->GetWeaponClass( _getWeaponClass()));
	//	strcat(pszBuff, conv);
	//}
#endif

	if( bLineFeed)
	{
		strcat( pszBuff, "\n");
	}

	// Ư�� ����
	strcat( pszBuff, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC13"));/*(����) */

	// Ƚ��
	REAL32 rate;
	rate = (REAL32) getArg(5) / _getCount();

	COLOR col;

	getProgressColor( getArg(5), _getCount(), &col);
	sprintf_s( conv, GAME_STRING("STR_TBL_QUEST_WEAPON_ACCM_ETC12"), col.r, col.g, col.b, col.a, getArg(5), _getCount());/*{col:%d,%d,%d,%d} %d/%dȸ{/col}*/
	strcat( pszBuff, conv);
}

// Quest�� ���� ���¸� ��ȯ�Ѵ�.
QUEST_STATE CQuestBase_Map_Count_Weapon_Accm::OnQueryState(void)
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

bool	CQuestBase_Map_Count_Weapon_Accm::ForceComplete(void)
{
	setArg( 5, _getCount());

	return true;
}

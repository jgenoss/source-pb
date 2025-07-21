#include "pch.h"
#include "TextSet.h"

//#define EMPTY_TEXTW L""
#include "ClientStageInfoUtil.h"

UINT32 TextSet::GetTime_Second(UINT32 GameTime)
{
	switch(GameTime)
	{
//	case BATTLE_TIME_TYPE_3:	//3��
	case BATTLE_TIME_MISSION_TYPE_3:	return 3*60;	// 3��
	case BATTLE_TIME_MISSION_TYPE_5:	return 5*60;	// 5��
	case BATTLE_TIME_MISSION_TYPE_7:	return 7*60;	// 7��
	case BATTLE_TIME_TYPE_5:			return 5*60;	// 5��
	case BATTLE_TIME_TYPE_10:			return 10*60;	// 10��
	case BATTLE_TIME_TYPE_15:			return 15*60;	// 15��
	case BATTLE_TIME_TYPE_20:			return 20*60;	// 20��
	case BATTLE_TIME_TYPE_25:			return 25*60;	// 25��
	case BATTLE_TIME_TYPE_30:			return 30*60;	// 30��
	}

	return 0;
}

i3::rc_wstring TextSet::GameTime(UINT32 GameTime)
{
	switch(GameTime)
	{
//	case BATTLE_TIME_TYPE_3:	//3��
	case BATTLE_TIME_MISSION_TYPE_3:	return GAME_RCSTRING("STBL_IDX_ROOM_TIME1");	// 3��
	case BATTLE_TIME_MISSION_TYPE_5:	return GAME_RCSTRING("STBL_IDX_ROOM_TIME2");	// 5��
	case BATTLE_TIME_MISSION_TYPE_7:	return GAME_RCSTRING("STBL_IDX_ROOM_TIME3");	// 7��
	case BATTLE_TIME_TYPE_5:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME2");	// 5��
	case BATTLE_TIME_TYPE_10:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME4");	// 10��
	case BATTLE_TIME_TYPE_15:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME5");	// 15��
	case BATTLE_TIME_TYPE_20:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME6");	// 20��
	case BATTLE_TIME_TYPE_25:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME7");	// 25��
	case BATTLE_TIME_TYPE_30:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME8");	// 30��
	}

	return i3::rc_wstring();
}

i3::rc_wstring TextSet::KillCount( UINT32 KillCount )
{
	// Kill ����
	switch( KillCount)
	{		
	case BATTLE_KILL_TYPE_60:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT1");	// 60Kill
	case BATTLE_KILL_TYPE_80:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT2");	// 80Kill
	case BATTLE_KILL_TYPE_100:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT3");	// 100Kill
	case BATTLE_KILL_TYPE_120:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT4");	// 120Kill
	case BATTLE_KILL_TYPE_140:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT5");	// 140Kill
	case BATTLE_KILL_TYPE_160:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT6");	// 160Kill
	}

	return i3::rc_wstring();
}


i3::rc_wstring TextSet::Round( UINT32 Round )
{
	switch(Round)
	{
	case BATTLE_ROUND_TYPE_1:	return GAME_RCSTRING("STBL_IDX_ROUND_COUNT_TYPE1");	// 1 Round
	case BATTLE_ROUND_TYPE_3:	return GAME_RCSTRING("STBL_IDX_ROUND_COUNT_TYPE2");	// 3 Rounds
	case BATTLE_ROUND_TYPE_5:	return GAME_RCSTRING("STBL_IDX_ROUND_COUNT_TYPE3");	// 5 Rounds
	case BATTLE_ROUND_TYPE_7:	return GAME_RCSTRING("STBL_IDX_ROUND_COUNT_TYPE4");	// 7 Rounds
	case BATTLE_ROUND_TYPE_9:	return GAME_RCSTRING("STBL_IDX_ROUND_COUNT_TYPE5");	// 9 Rounds
	}

	return i3::rc_wstring();
}

i3::rc_wstring TextSet::AIModeLevel(UINT32 level)
{
	switch(level)
	{
	case 1:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_J");	// 1���(����)
	case 2:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_I");	// 2���
	case 3:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_H");	// 3��� 
	case 4:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_G");	// 4���
	case 5:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_F");	// 5���
	case 6:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_E");	// 6���
	case 7:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_D");	// 7���
	case 8:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_C");	// 8���
	case 9:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_B");	// 9���
	case 10:	return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_A");	// 10���
	}

	return i3::rc_wstring();
}

//���� �̼� ���̵� ��Ʈ��( ���� String.txt �� ������ �մϴ�. )
const i3::string TextSet::domi_script( UINT32 level)
{
	//switch(level)
	//{
	//case 1:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_EASY"); /*"Easy"*/
	//case 2:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_NORMAL");/*"Normal"*/
	//case 3:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_HARD");/*"Hard"*/
	//case 4:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_VERYHARD");/*"very Hard"*/
	//default:	return "not found";
	//}
	i3::string str = "script_";

	char strNum[4] = {0,};
	itoa( level, strNum, 10);

	str += strNum;

	if( level == 6) str = "script_random";
	return str;
}

i3::rc_wstring TextSet::Mode( UINT32 Mode )
{
	i3::rc_wstring ret;
	switch(Mode)
	{
		//STBL_IDX_MODE_TYPE1 : ��������
	case STAGE_MODE_CROSSCOUNT:
	case STAGE_MODE_ESCAPE:	

		ret = GAME_RCSTRING("STBL_IDX_MODE_TYPE1");// ��������

		if( i3::generic_is_equal( ret, L"STBL_IDX_MODE_TYPE1" ) )
			return i3::rc_wstring();

		return ret;
	}

	return i3::rc_wstring();
}

i3::rc_wstring TextSet::VictoryCondition( STAGE_MODE StageType, UINT32 GameType )
{
	switch( StageType )
	{
	case STAGE_MODE_DESTROY			:
	case STAGE_MODE_DEFENCE			:
	case STAGE_MODE_ANNIHILATION	:
	case STAGE_MODE_BOMB			:
	case STAGE_MODE_CONVOY			:
		{
			return TextSet::Round(GameType);
		}
		break;
	case STAGE_MODE_CROSSCOUNT:
	case STAGE_MODE_ESCAPE			:
		{
			return TextSet::Mode(StageType);
		}
		break;
	case STAGE_MODE_DEATHMATCH		:
	default							:
		{
			return TextSet::KillCount(GameType);
		}
		break;
	}

	//return EMPTY_TEXT;
}


i3::rc_wstring TextSet::PCCafeMessage( UINT32 PCCafe )
{
	switch(PCCafe)
	{
	case PC_NORMAL_CAFE			:	return GAME_RCSTRING("STR_TBL_GUI_LOBBY_CONNECT_PCROOM");			/*PC�濡�� �����ϼ̽��ϴ�.*/
	case PC_PREMIUM_CAFE		:	return GAME_RCSTRING("STR_TBL_GUI_LOBBY_CONNECT_PRIMIUM_PCROOM");	/*�����̾� PC�濡�� �����ϼ̽��ϴ�.*/
	case PC_VACATION_EVENT_ITEM	:	return L"ID Bonus Package";
	case PC_GARENA_GCA			:	return GAME_RCSTRING("STR_TBL_GUI_LOBBY_CONNECT_PCROOM");			/*PC�濡�� �����ϼ̽��ϴ�.*/
	case PC_GARENA_GCA_PLUS		:	return GAME_RCSTRING("STR_TBL_GUI_LOBBY_CONNECT_PCROOM");			/*PC�濡�� �����ϼ̽��ϴ�.*/
	}

	return i3::rc_wstring();
}

i3::rc_wstring TextSet::ItemType(UINT8 ItemType)
{
	switch(ItemType)
	{
	case ITEM_TYPE_UNKNOWN:			return i3::rc_wstring();										
	case ITEM_TYPE_PRIMARY:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ASSAULT");	// ���ݼ��� ����
	//// 1.5 Check ClassType
	//case ITEM_TYPE_PRIMARY:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SMG");		// ������� ����
	//case ITEM_TYPE_PRIMARY:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SNIPER");		// ������ ����
	//case ITEM_TYPE_PRIMARY:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SHOTGUN");	// ��ź�� ����
	//case ITEM_TYPE_PRIMARY:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_MG");			// ����� ����
	case ITEM_TYPE_SECONDARY:		return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_HANDGUN");	// ���� ���� ����
	case ITEM_TYPE_MELEE:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_KNIFE");		// ���� ���� ����
	case ITEM_TYPE_THROWING1:		return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_THROWING");	// ��ô ���� ����
	case ITEM_TYPE_THROWING2:		return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ITEM");		// Ư�� ���� ����
	case ITEM_TYPE_CHARA:			return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_CHARA");		// ĳ���� ����
	case ITEM_TYPE_HEADGEAR:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_HEAD");		// �Ӹ� ����
	case ITEM_TYPE_MAINTENANCE:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_TERM");		// ���Ӽ� ����
	case ITEM_TYPE_WRAP_PERIOD:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_COUNT");		// �Ҹ� ����
	case ITEM_TYPE_WRAP_COUNT:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_COUNT");		// �Ҹ� ����
	case ITEM_TYPE_INGAMEITEM:																// �������� ������(����)
	default:						return L"NONE_TYPE";
	}
}

i3::rc_wstring TextSet::ClanJoinLimitToRank(UINT8 Rank)
{
	if(Rank >= RANK_46)
		return GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_4");
	else if (Rank >= RANK_31)
		return GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_3");
	else if( Rank >= RANK_17)
		return GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_2");
	else if ( Rank >= RANK_04)
		return GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_1");
	else
		return GAME_RCSTRING("STBL_IDX_CLAN_RANK_UNLIMIT");
}

i3::rc_wstring TextSet::ClanJoinLimitToHighAge(UINT8 Age, bool bUpper)
{
	if(Age <= 0)
		return GAME_RCSTRING("STBL_IDX_CLAN_AGE_UNLIMIT");

	UINT32 nConvertAgeToIdx = 0;

	if(Age > 20)
		nConvertAgeToIdx = 3;	// 30��
	else if (Age > 15)
		nConvertAgeToIdx = 2;	// 20��
	else 
		nConvertAgeToIdx = 1;	// 15��

	i3::string strTemp;

	if(bUpper)
		i3::sprintf(strTemp, "STBL_IDX_CLAN_AGE_UPPER_%d", nConvertAgeToIdx);
	else
		i3::sprintf(strTemp, "STBL_IDX_CLAN_AGE_LOWER_%d", nConvertAgeToIdx);

	return GAME_RCSTRING(strTemp);
}

void TextSet::ClanJoinLimitText(i3::wstring& strOut, CLAN_DETAIL_INFO * pInfo)
{
	// �ʱ�ȭ �κ��� ����ó���� ���Ͽ� �߰�
	if(pInfo == nullptr)
	{
		i3::sprintf(strOut, L"%s %s\n%s %s", 
			GAME_RCSTRING("STR_IDX_CLAN_JOIN_LIMIT_RANK"), TextSet::ClanJoinLimitToRank(0), 
			GAME_RCSTRING("STR_IDX_CLAN_JOIN_LIMIT_AGE"), TextSet::ClanJoinLimitToHighAge(0));
		return;
	}

//	const INT32 TEMP_STRING_SIZE = 64;

	i3::rc_wstring wstrRankLock = TextSet::ClanJoinLimitToRank(pInfo->m_ranklock);		// ��� ���� ����

	// ���� ���� ����
	//char szAgeLock[64] = {0};
	i3::rc_wstring	wstrAgeLock;
	{
		// �ּ� ���� ���� ����
		i3::rc_wstring wstrLAgeLock = TextSet::ClanJoinLimitToHighAge(pInfo->m_lagelock, true);
		// �ִ� ���� ���� ����
		i3::rc_wstring wstrHAgeLock = TextSet::ClanJoinLimitToHighAge(pInfo->m_hagelock);

		if(pInfo->m_lagelock > 0 && pInfo->m_hagelock > 0)
		{
			// X�� �̻� ~ X�� ����
			i3::sprintf(wstrAgeLock, L"%s ~ %s", wstrLAgeLock, wstrHAgeLock);
		}
		else if (pInfo->m_lagelock > 0)
		{
			// X�� �̻�
			wstrAgeLock = wstrLAgeLock;
		}
		else if (pInfo->m_hagelock > 0)
		{
			// X�� ����
			wstrAgeLock = wstrHAgeLock;
		}
		else
		{
			// ���� ����
			wstrAgeLock = GAME_RCSTRING("STBL_IDX_CLAN_AGE_UNLIMIT");
		}
	}

	i3::sprintf(strOut, L"%s %s\n%s %s", 
		GAME_RCSTRING("STR_IDX_CLAN_JOIN_LIMIT_RANK"), wstrRankLock, GAME_RCSTRING("STR_IDX_CLAN_JOIN_LIMIT_AGE"), wstrAgeLock);

}

void	 TextSet::GetWinObjectText(i3::wstring& strOut, CSI_STAGE* stage, UINT8 ui8GameSetPrim, UINT8 ui8GameSetSec)
{
	switch(	stage->GetStageMode())
	{
	case STAGE_MODE_CROSSCOUNT		:
		{
			strOut = TextSet::Mode(STAGE_MODE_CROSSCOUNT);	// CrossCount
		}
		break;

	case STAGE_MODE_ESCAPE			:
		{
			strOut = TextSet::Mode(STAGE_MODE_ESCAPE);	// Escape
		}
		break;
	default							:					
		{
			if( CStageInfoUtil::IsModeRoundType(*stage))
				strOut = TextSet::Round(ui8GameSetPrim);		// Round Type
			else
				strOut = TextSet::KillCount(ui8GameSetPrim); // No Round Type	
		}
		break;		
	}

	strOut += L" /";
	// Time Count
	i3::rc_wstring	wstrTime = TextSet::GameTime( ui8GameSetSec);
	strOut.append(wstrTime.begin(), wstrTime.end());

}

void TextSet::StarPlayerEntranceMessage(i3::rc_wstring & wstr_out, const UINT8 rank, const i3::rc_wstring wstr_nick)
{
	switch (rank)
	{
	case RANK_56: i3::sprintf(wstr_out, GAME_RCSTRING("STBL_IDX_STARPLAYER_ASSAULT"),	wstr_nick);	break;
	case RANK_57: i3::sprintf(wstr_out, GAME_RCSTRING("STBL_IDX_STARPLAYER_SMG"),		wstr_nick);	break;
	case RANK_58: i3::sprintf(wstr_out, GAME_RCSTRING("STBL_IDX_STARPLAYER_MG"),		wstr_nick);	break;
	case RANK_59: i3::sprintf(wstr_out, GAME_RCSTRING("STBL_IDX_STARPLAYER_SHOTGUN"),	wstr_nick);	break;
	case RANK_60: i3::sprintf(wstr_out, GAME_RCSTRING("STBL_IDX_STARPLAYER_SNIPER"),	wstr_nick);	break;
	}
}

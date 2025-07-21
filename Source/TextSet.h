#ifndef __TEXTSET_H__
#define __TEXTSET_H__

#include "CSI_Stage.h"

namespace TextSet
{
	UINT32 GetTime_Second(UINT32 GameTime);
	i3::rc_wstring GameTime(UINT32 GameTime);
	i3::rc_wstring KillCount(UINT32 KillCount);
	i3::rc_wstring Round(UINT32 Round);
	i3::rc_wstring Mode(UINT32 Mode);
	i3::rc_wstring VictoryCondition(STAGE_MODE StageType, UINT32 GameType);
	i3::rc_wstring PCCafeMessage(UINT32 PCCafe);
	i3::rc_wstring AIModeLevel(UINT32 level);

	const i3::string domi_script( UINT32 level);

	i3::rc_wstring ItemType(UINT8 ItemType);

    //Ŭ�� �������� ǥ�õǴ� ���� ���� �����Դϴ�.
	//�ش� �ؽ�Ʈ�� UIPhaseNonClanMember, UITabClanRanking Ŭ�������� �������� ����ϱ� ������
	//TextSet���� �Űܳ����ϴ�.
	void ClanJoinLimitText(i3::wstring& strOut, CLAN_DETAIL_INFO * pInfo = nullptr);
	i3::rc_wstring ClanJoinLimitToRank(UINT8 Rank);
	i3::rc_wstring ClanJoinLimitToHighAge(UINT8 Age, bool bUpper = false);

	void	GetWinObjectText(i3::wstring& strOut, CSI_STAGE* stage, UINT8 ui8GameSetPrim, UINT8 ui8GameSetSec);

	// ��Ÿ �÷��̾� ���� �޼��� (keyword : Star Player)
	void StarPlayerEntranceMessage(i3::rc_wstring & wstr_out, const UINT8 rank, const i3::rc_wstring wstr_nick );
}



#endif	// __TEXTSET_H__
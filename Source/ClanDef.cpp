#include "pch.h"
#include "ClanDef.h"
#include "GlobalVariables.h"
#include "GameGUI.h"
#include "ServerInfo.h"
#include "i3Base/string/ext/generic_string_cat.h"

// GUI Shpae offst
#define GUI_SHAPE_OFFSET			0

//	Ŭ�� ��� ����ġ
UINT32 g_ClanExp[CLAN_RANK_MAX] = 
{
	DF_CLAN_RANK_NEW,	//	�Ż�
	DF_CLAN_RANK_A1,	//	�Ʒ� 1�ܰ�
	DF_CLAN_RANK_A2,	//	�Ʒ� 2�ܰ�
	DF_CLAN_RANK_B1,	//	�߽� 1�ܰ�
	DF_CLAN_RANK_B2,	//	�߽� 2�ܰ�
	DF_CLAN_RANK_C1,	//	���� 1�ܰ�
	DF_CLAN_RANK_C2,	//	���� 2�ܰ�
	DF_CLAN_RANK_C3,	//	���� 3�ܰ�
	DF_CLAN_RANK_D1,	//	��� 1�ܰ�
	DF_CLAN_RANK_D2,	//	��� 2�ܰ�
	DF_CLAN_RANK_D3,	//	��� 3�ܰ�
	DF_CLAN_RANK_D4,	//	��� 4�ܰ�
	DF_CLAN_RANK_E1,	//	���� 1�ܰ�
	DF_CLAN_RANK_E2,	//	���� 2�ܰ�
	DF_CLAN_RANK_E3,	//	���� 3�ܰ�
	DF_CLAN_RANK_E4,	//	���� 4�ܰ�
	DF_CLAN_RANK_F1,	//	���� 1�ܰ�
	DF_CLAN_RANK_F2,	//	���� 2�ܰ�
	DF_CLAN_RANK_F3,	//	���� 3�ܰ�
	DF_CLAN_RANK_F4,	//	���� 4�ܰ�
	DF_CLAN_RANK_F5,	//	���� 5�ܰ�
	DF_CLAN_RANK_G1,	//	�⵿ 1�ܰ�
	DF_CLAN_RANK_G2,	//	�⵿ 2�ܰ�
	DF_CLAN_RANK_G3,	//	�⵿ 3�ܰ�
	DF_CLAN_RANK_G4,	//	�⵿ 4�ܰ�
	DF_CLAN_RANK_G5,	//	�⵿ 5�ܰ�
	DF_CLAN_RANK_H1,	//	���� 1�ܰ�
	DF_CLAN_RANK_H2,	//	���� 2�ܰ�
	DF_CLAN_RANK_H3,	//	���� 3�ܰ�
	DF_CLAN_RANK_H4,	//	���� 4�ܰ�
	DF_CLAN_RANK_H5,	//	���� 5�ܰ�
	DF_CLAN_RANK_I1,	//	Ư�� 1�ܰ�
	DF_CLAN_RANK_I2,	//	Ư�� 2�ܰ�
	DF_CLAN_RANK_I3,	//	Ư�� 3�ܰ�
	DF_CLAN_RANK_I4,	//	Ư�� 4�ܰ�
	DF_CLAN_RANK_I5,	//	Ư�� 5�ܰ�
	DF_CLAN_RANK_J1,	//	Ư�� 1�ܰ�
	DF_CLAN_RANK_J2,	//	Ư�� 2�ܰ�
	DF_CLAN_RANK_J3,	//	Ư�� 3�ܰ�
	DF_CLAN_RANK_J4,	//	Ư�� 4�ܰ�
	DF_CLAN_RANK_J5,	//	Ư�� 5�ܰ�
	DF_CLAN_RANK_K1,	//	Ư�� 1�ܰ�
	DF_CLAN_RANK_K2,	//	Ư�� 2�ܰ�
	DF_CLAN_RANK_K3,	//	Ư�� 3�ܰ�
	DF_CLAN_RANK_K4,	//	Ư�� 4�ܰ�
	DF_CLAN_RANK_K5,	//	Ư�� 5�ܰ�
	DF_CLAN_RANK_L1,	//	Ư�� 1�ܰ�
	DF_CLAN_RANK_L2,	//	Ư�� 2�ܰ�
	DF_CLAN_RANK_L3,	//	Ư�� 3�ܰ�
	DF_CLAN_RANK_L4,	//	Ư�� 4�ܰ�
	DF_CLAN_RANK_L5,	//	Ư�� 5�ܰ�
};

//	Ŭ�� �δ� ����
INT32 g_ClanUnitCondition[CLAN_UNIT_MAX] = 
{
	DF_CLAN_UNIT_SQUARD,			//	�д�
	DF_CLAN_UNIT_PLATOON,			//	�Ҵ�
	DF_CLAN_UNIT_COMPANY,			//	�ߴ�
	DF_CLAN_UNIT_BATTALION,			//	���
	DF_CLAN_UNIT_REGIMENT,			//	����
	DF_CLAN_UNIT_BRIGADE,			//	����
	DF_CLAN_UNIT_DIVISION,			//	���
	DF_CLAN_UNIT_CORPS,				//	����
};

i3::rc_wstring	GetClanMemberColor(INT32 Level)
{
	//	Ŭ���� ���޿� �ش��ϴ� ����
	static const i3::rc_wstring s_pMemberColor[CLAN_MEMBER_LEVEL_MAX] = 
	{
		L"{col:255,255,255,255}%s{/col}",	//	CLAN_MEMBER_LEVEL_UNKNOWN
		L"{col:255,211,3,255}%s{/col}",		//	CLAN_MEMBER_LEVEL_MASTER
		L"{col:0,153,0,255}%s{/col}",		//	CLAN_MEMBER_LEVEL_STAFF
		L"{col:173,173,173,255}%s{/col}",	//	CLAN_MEMBER_LEVEL_REGULAR
	};

	if ( Level >= 0 && Level < CLAN_MEMBER_LEVEL_MAX )
	{
		return s_pMemberColor[Level];
	}

	return s_pMemberColor[CLAN_MEMBER_LEVEL_UNKNOWN];
}



//	Ŭ�� ���
CLAN_RANK GetClanRank(UINT32 nClanExp)
{
	for(INT32 i=CLAN_RANK_TOP; i>=0; i--)
	{
		if(nClanExp >= g_ClanExp[i])
			return (CLAN_RANK) i;
	}

	return CLAN_RANK_UNKNOWN;
}

//	Ŭ�� �ִ� �ο���
INT32 GetClanStaffMax(CLAN_UNIT nUnit)
{
	if( (INT32)nUnit < 0 || (INT32)nUnit >= (INT32)CLAN_UNIT_MAX)
		return -1;

	return g_ClanUnitCondition[ nUnit];
}

//	Ŭ�� ���̻�
CLAN_UNIT GetClanUnit(INT32 nClanStaffNum)
{
	for(INT32 i=CLAN_UNIT_TOP; i>=0; i--)
	{
		if(nClanStaffNum >= g_ClanUnitCondition[i])
			return (CLAN_UNIT) i;
	}

	return CLAN_UNIT_UNKNOWN;
}

//	Ŭ�� ������ �ʿ� ����ġ
UINT32 GetClanNextRankExp(UINT32 nCurExp)
{
	//	�ְ� ������ �Ѿ��.
	if( nCurExp >= g_ClanExp[CLAN_RANK_TOP])
	{
		return g_ClanExp[CLAN_RANK_TOP];
	}

	for(INT32 i=CLAN_RANK_TOP-1; i>=0; i--)
	{
		if(nCurExp >= g_ClanExp[i])
		{
			return g_ClanExp[i + 1];	//	���� �������� �Ѵܰ� �� ���� ����ġ�� �ʿ��ϴ�.
		}
	}

	return 0;
}

//	Ŭ�� ������ �ʿ� ����ġ
UINT32 GetClanBeforeRankExp(UINT32 nCurExp)
{
	//	�ְ� ������ �Ѿ��.
	if( nCurExp >= g_ClanExp[CLAN_RANK_TOP])
	{
		return g_ClanExp[CLAN_RANK_TOP-1];
	}

	for( INT32 i = 1; i <= CLAN_RANK_TOP ; i++ )
	{
		if(nCurExp < g_ClanExp[i])
		{
			return g_ClanExp[i - 1];	//	���� �������� �Ѵܰ� �Ʒ� ���� ����ġ�� �ʿ��ϴ�.
		}
	}

	return 0;
}
//	Ŭ�� ���λ� ��������
void GetClanRankPreName(i3::rc_wstring& out, UINT32 nClanExp)
{
	
	CLAN_RANK nRank = GetClanRank(nClanExp);

	GetClanRankPreName2(out, (UINT8) nRank);
}

void GetClanRankPreName2(i3::rc_wstring& out, UINT8 nClanExpRank)
{
	CLAN_RANK nRank = (CLAN_RANK) nClanExpRank;

	switch(nRank)
	{
	case CLAN_RANK_NEW:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_NEW");	break;	// "�Ż�"
	case CLAN_RANK_A1:
	case CLAN_RANK_A2:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_A");		break;	// "�Ʒ�"
	case CLAN_RANK_B1:	
	case CLAN_RANK_B2:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_B");		break;	// "�߽�"
	case CLAN_RANK_C1:
	case CLAN_RANK_C2:
	case CLAN_RANK_C3:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_C");		break;	// "����"
	case CLAN_RANK_D1:
	case CLAN_RANK_D2:
	case CLAN_RANK_D3:
	case CLAN_RANK_D4:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_D");		break;	// "���"
	case CLAN_RANK_E1:
	case CLAN_RANK_E2:
	case CLAN_RANK_E3:
	case CLAN_RANK_E4:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_E");		break;	// "����"
	case CLAN_RANK_F1:
	case CLAN_RANK_F2:
	case CLAN_RANK_F3:
	case CLAN_RANK_F4:
	case CLAN_RANK_F5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_F");		break;	// "����"
	case CLAN_RANK_G1:
	case CLAN_RANK_G2:
	case CLAN_RANK_G3:
	case CLAN_RANK_G4:
	case CLAN_RANK_G5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_G");		break;	// "�⵿"
	case CLAN_RANK_H1:
	case CLAN_RANK_H2:
	case CLAN_RANK_H3:
	case CLAN_RANK_H4:
	case CLAN_RANK_H5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_H");		break;	// "����"
	case CLAN_RANK_I1:
	case CLAN_RANK_I2:
	case CLAN_RANK_I3:
	case CLAN_RANK_I4:
	case CLAN_RANK_I5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_I");		break;	// "Ư��"
	case CLAN_RANK_J1:
	case CLAN_RANK_J2:
	case CLAN_RANK_J3:
	case CLAN_RANK_J4:
	case CLAN_RANK_J5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_J");		break;	// "Ư��"
	case CLAN_RANK_K1:
	case CLAN_RANK_K2:
	case CLAN_RANK_K3:
	case CLAN_RANK_K4:
	case CLAN_RANK_K5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_K");		break;	// "Ư��"
	case CLAN_RANK_L1:
	case CLAN_RANK_L2:
	case CLAN_RANK_L3:
	case CLAN_RANK_L4:
	case CLAN_RANK_L5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_L");		break;	// "Ư��
	default:
		I3ASSERT_0;
		break;
	}
}

//	Ŭ���̸� ���̻� ��������
void GetClanRankPostName(i3::rc_wstring& out, INT32 nClanStaffNum)
{
	CLAN_UNIT nUnit = GetClanUnit(nClanStaffNum);

	GetClanRankPostName2(out, (UINT8) nUnit);
}

void GetClanRankPostName2(i3::rc_wstring& out, UINT8 nClanPerRank)
{
	CLAN_UNIT nUnit = (CLAN_UNIT) nClanPerRank;

	switch(nUnit)
	{
	case CLAN_UNIT_SQUARD:		out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_SQUARD");	break;	// "�д�"
	case CLAN_UNIT_PLATOON:		out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_PLATOON");	break;	// "�Ҵ�"
	case CLAN_UNIT_COMPANY:		out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_COMPANY");	break;	// "�ߴ�"
	case CLAN_UNIT_BATTALION:	out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_BATTALION");	break;	// "���"
	case CLAN_UNIT_REGIMENT:	out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_REGIMENT");	break;	// "����"
	case CLAN_UNIT_BRIGADE:		out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_BRIGADE");	break;	// "����"
	case CLAN_UNIT_DIVISION:	out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_DIVISION");	break;	// "���"	
	case CLAN_UNIT_CORPS:		out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_CORPS");		break;	// "����"
	default:
		I3ASSERT_0;
		break;
	}
}

//	Ŭ�� ��޺� Ǯ���� ��������
void GetClanFullName(i3::wstring& out, UINT32 nClanExp, INT32 nClanStaffNum)
{

	i3::rc_wstring wstrTemp;
	//	Ŭ�� ���λ�
	GetClanRankPreName(wstrTemp, nClanExp);
	out = wstrTemp;

	//	Ŭ�� ���̻�
	GetClanRankPostName(wstrTemp, nClanStaffNum);
	out.append(wstrTemp.begin(), wstrTemp.end());

}

void GetClanFullName2(i3::wstring& out, UINT8 nClanExpRank, UINT8 nClanPerRank)
{

	i3::rc_wstring wstrTemp;

	//	Ŭ�� ���λ�
	GetClanRankPreName2(wstrTemp, nClanExpRank);
	out = wstrTemp;

	//	Ŭ�� ���̻�
	GetClanRankPostName2(wstrTemp, nClanPerRank);
	out.append(wstrTemp.begin(), wstrTemp.end());
}

//	��� ����
void SetRankImage(i3GuiStatic * pRankStatic, UINT8 nRank)
{
	I3ASSERT( pRankStatic != nullptr);

	pRankStatic->SetEnable(true);

	GameGUI::SetRankStatic(pRankStatic);
	pRankStatic->SetShapeUVIndex(nRank);		

	pRankStatic->OnUpdate(0.0f);
}

void SetClanRankImage(i3GuiStatic * pRankStatic, UINT8 nRank)
{
	I3ASSERT( pRankStatic != nullptr);

	pRankStatic->SetEnable(true);
	pRankStatic->SetAllShapeEnable(false);
	pRankStatic->SetShapeEnable(GUI_SHAPE_OFFSET + nRank, true);
	pRankStatic->OnUpdate(0.0f);
}

CLAN_MEMBER_LEVEL_TYPE GetMemberLevel(UINT8	nLevel)
{
	I3_BOUNDCHK((INT32) nLevel, (INT32) CLAN_MEMBER_LEVEL_MAX);

	return (CLAN_MEMBER_LEVEL_TYPE) nLevel;
}

void GetMemberLevelName(CLAN_MEMBER_LEVEL_TYPE level, i3::wstring& out)
{	
//	I3ASSERT(pBuf != nullptr);

//	i3::rc_wstring temp[MAX_STRING_COUNT] = "";
	i3::rc_wstring wstrColorFormat = GetClanMemberColor(level);

	switch( level)
	{
	case CLAN_MEMBER_LEVEL_MASTER:	i3::sprintf(out, wstrColorFormat, GAME_RCSTRING("STBL_IDX_CLAN_MASTER_STATIC"));		break;	// "������"
	case CLAN_MEMBER_LEVEL_STAFF:	i3::sprintf(out, wstrColorFormat, GAME_RCSTRING("STBL_IDX_CLAN_STAFF_STATIC"));		break;	// "������"
	case CLAN_MEMBER_LEVEL_REGULAR:	i3::sprintf(out, wstrColorFormat, GAME_RCSTRING("STBL_IDX_CLAN_MEMBERSHIP_STATIC"));	break;	// "�Ϲ�ȸ��"
	default:
		{
			// "�˼�����"
			i3::generic_string_cat(wstrColorFormat, L"%d");
			i3::sprintf(out, wstrColorFormat, GAME_RCSTRING("STBL_IDX_CLAN_UNKNOWN"), level);
		}
		break;
	}
}

void GetMemberLevelName2(UINT8 nLevel, i3::wstring& out)
{
	I3_BOUNDCHK((INT32) nLevel, (INT32) CLAN_MEMBER_LEVEL_MAX);

	return GetMemberLevelName( GetMemberLevel(nLevel), out);
}

FRIEND_STATE GetMemberConnectState(UINT32 nState)
{
	return (FRIEND_STATE) GET_FRIEND_STATE( nState);
}

INT32 GetMemberConnectChannel(UINT32 nState)
{
	return GET_FRIEND_CHANNEL( nState);
}

INT32 GetMemberConnectRoomNum(UINT32 nState)
{
	return GET_FRIEND_ROOMIDX( nState);
}

void GetMemberConnectStateName(UINT32 nState, i3::rc_wstring& out )
{
	switch(GetMemberConnectState(nState))
	{
	case FRIEND_STATE_REQUEST:		out = GAME_RCSTRING("STBL_IDX_CLAN_SECRET");	break;	// �����
	case FRIEND_STATE_HOLD:			out = GAME_RCSTRING("STBL_IDX_CLAN_SECRET");	break;	// �����
	case FRIEND_STATE_OFFLINE:		out = GAME_RCSTRING("STBL_IDX_CLAN_OFFLINE");	break;	// ��������
	case FRIEND_STATE_ONLINE:		out = GAME_RCSTRING("STR_POPUP_READY_STATE");	break;	// �¶���
	case FRIEND_STATE_LOBBY:		out = GAME_RCSTRING("STBL_IDX_CLAN_ONLINE");	break;	// �¶���
	case FRIEND_STATE_ROOM:			FormatLocation(nState, out);					break;
	default:						out = GAME_RCSTRING("STBL_IDX_CLAN_UNKNOWN");	break;	// �˼�����
	}
}

bool IsOnlineUser(UINT32 nState)
{
	if(GetMemberConnectState(nState) == FRIEND_STATE_OFFLINE)
		return false;

	return true;
}

UINT32 GetEntryYear(UINT32 nDate)
{
	return (nDate / 10000);
}

UINT32 GetEntryMonth(UINT32 nDate)
{
	return ((nDate % 10000) / 100);
}

UINT32 GetEntryDay(UINT32 nDate)
{
	return (nDate % 100);
}

void SetCheckBox(i3GuiCheckBox * pControl, bool bChecked)
{
	pControl->SetEnable(true);
	pControl->setCheckBoxState(bChecked);

	pControl->OnUpdate(0.0f);
}

void SetClanExpText(i3GuiEditBox * pControl, UINT32 nCurExp, UINT32 nMaxExp)
{
	UINT32 nRate = 0;

	if( nMaxExp <= 0)  
	{
		pControl->SetText(L"");
		return;
	}
	else
	{
		nRate = (nCurExp * 100 / nMaxExp);
	}

	//	����
	i3::stack_wstring wstrTemp;
	i3::sprintf(wstrTemp, L"%u / %u (%u%%)", nCurExp, nMaxExp, nRate );
	pControl->SetText(wstrTemp);
}

void SetClanExpGauge(i3GuiStatic * pExpBar, REAL32 fResolutionWidth, UINT32 nCurExp, UINT32 nMaxExp)
{
	UINT32 nRate = 0;
	REAL32 fOriginWidth = 0.0f;
	i3GuiImage * pExpImage = nullptr;

	if( nMaxExp <= 0) 
	{
		pExpBar->SetEnable(false);
		pExpBar->OnUpdate(0.0f);
		return;		
	}
	else
	{
		pExpBar->SetEnable(true);
		nRate = (nCurExp * 100 / nMaxExp);
	}

	//	������	
	pExpImage = pExpBar->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);		
	fOriginWidth = pExpImage->getOriginalWidth() * fResolutionWidth;

	pExpImage->setWidth( ((REAL32)nRate / 100.0f) * fOriginWidth);

	pExpBar->OnUpdate(0.0f);
}

void SetClanStaff(i3GuiEditBox * pControl, INT32 nCurrentNum, INT32 nTotalNum)
{
	i3::stack_wstring wstrTemp;
	i3::sprintf(wstrTemp, L"%d / %d", nCurrentNum, nTotalNum);

	pControl->SetText(wstrTemp);
}

/*
void SetClanMaster(i3GuiEditBox * pName, i3GuiStatic * pRank, const char * pszName, UINT8 nRank)
{
	pName->SetText(pszName);

	//	�����
	if(pszName == nullptr || strlen(pszName) == 0)
	{
		pRank->SetEnable(false);
	}
	else
	{
		SetRankImage(pRank, nRank);
	}
}
*/
/*
void SetClanRank(i3GuiEditBox * pRankName, i3GuiStatic * pRankImage, UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank)
{
	char szClanName[MAX_STRING_COUNT] = "";

	if( nClanStaffNum > 0)
	{
		//	Ŭ�� ��� ȣĪ
		GetClanFullName(szClanName, MAX_STRING_COUNT, nClanExp, nClanStaffNum);

		//	Ŭ�� ��� �̹���
		pRankImage->SetEnable(true);
		SetClanRankImage(pRankImage, nClanRank);
	}
	else
	{
		pRankImage->SetEnable(false);
	}

	pRankName->SetText(szClanName);
}
*/
/*
void SetClanRank2(i3GuiEditBox * pRankName, i3GuiStatic * pRankImage, UINT8 nClanExpRank, UINT8 nClanPerRank, UINT8 nClanRank)
{
	char szClanName[MAX_STRING_COUNT] = "";

	//	Ŭ�� ��� ȣĪ
	GetClanFullName2(szClanName, MAX_STRING_COUNT, nClanExpRank, nClanPerRank);

	//	Ŭ�� ��� �̹���
	pRankImage->SetEnable(true);
	SetClanRankImage(pRankImage, nClanRank);

	pRankName->SetText(szClanName);
}
*/
void SetClanOrder(i3GuiEditBox * pControl, INT32 nCurOrder, INT32 nPrevOrder)
{
	/*char szTemp[MAX_STRING_COUNT] = "";

	INT32 nGapOrder = nCurOrder - nPrevOrder;

	if(nGapOrder > 0)
		sprintf_s(szTemp, "%d%s (��%d)", nCurOrder, GAME_RCSTRING("STBL_IDX_CLAN_LADDER"), i3Math::abs(nGapOrder));
	else if(nGapOrder < 0)
		sprintf_s(szTemp, "%d%s (��%d)", nCurOrder, GAME_RCSTRING("STBL_IDX_CLAN_LADDER"), i3Math::abs(nGapOrder));
	else
		sprintf_s(szTemp, "%d%s (- %d)", nCurOrder, GAME_RCSTRING("STBL_IDX_CLAN_LADDER"), 0);

	pControl->SetText(szTemp);*/

	pControl->SetText(GAME_RCSTRING("STBL_IDX_CLAN_PLEASE_CHECK_WEB"));/*PB Ȩ���������� Ȯ���� �ֽʽÿ�*/
}

void SetClanPoint(i3GuiEditBox * pControl, REAL32 fPoint)
{
	I3ASSERT( pControl != nullptr);

	i3::stack_wstring wstr_temp;
	i3::sprintf(wstr_temp, L"%d", (INT32) (fPoint + 0.5f));	//	Ŭ�� ����Ʈ�� �Ҽ��� �ݿø��ؼ� ������ ��ȯ
	pControl->SetText(wstr_temp);
}

bool SetClanMarkTex(i3GuiStatic * pControl, CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetClanMarkTex( pControl, type, nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);
}

bool SetClanMarkTex(i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetClanMarkTex( pSprite, nSpriteIdx, type, nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);
}

bool SetClanMarkTexEx(i3GuiStatic * pControl, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetClanMarkTex( pControl, type, 
									CLANMARK_FRAME_IMAGE_IDX(nClanMarkFlag),
									CLANMARK_CONTENT_IMAGE_IDX(nClanMarkFlag),
									CLANMARK_FRAME_COLOR_IDX(nClanMarkFlag),
									CLANMARK_CONTENT_COLOR_IDX(nClanMarkFlag));
}

bool SetClanMarkTexEx(i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetClanMarkTex( pSprite, nSpriteIdx, type, 
									CLANMARK_FRAME_IMAGE_IDX(nClanMarkFlag),
									CLANMARK_CONTENT_IMAGE_IDX(nClanMarkFlag),
									CLANMARK_FRAME_COLOR_IDX(nClanMarkFlag),
									CLANMARK_CONTENT_COLOR_IDX(nClanMarkFlag));
}

bool SetDefaultClanMarkTex( i3GuiStatic * pControl, CLAN_MARK_SIZE type)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetDefaultClanMarkTex( pControl, type);
}

bool SetDefaultClanMarkTex( i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetDefaultClanMarkTex( pSprite, nSpriteIdx, type);
}

bool MakeClanMarkTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, 
					 INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx, INT32 u, INT32 v)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->MakeClanMarkTex( pOutTex, type, nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx, u, v);
}

bool MakeClanMarkTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, UINT32 markFlag, INT32 u, INT32 v)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->MakeClanMarkTex( pOutTex, type, markFlag, u, v);
}

bool MakeClanMarkShapeTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, CLAN_MARK_LAYER_TYPE LayerType, 
							INT32 nImageIdx, INT32 nColorIdx, INT32 u, INT32 v)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->MakeClanMarkShapeTex( pOutTex, type, LayerType, nImageIdx, nColorIdx, u, v);
}

bool SetClanMarkTex(i3GuiStatic * pControl, SLOT_INFO * pInfo)
{
	if( pInfo && pControl)
	{
		if( pInfo->_clanidx > 0 && pInfo->_clanstate != CLAN_MEMBER_LEVEL_UNKNOWN)
		{	
			UINT32 flag = pInfo->_clanMark;

			pControl->SetEnable(true);

			SetClanMarkTexEx( pControl, CLAN_MARK_SMALL, flag);

			return true;
		}
		else
		{
			pControl->SetEnable(false);

			return false;
		}
	}

	return false;
}

INT32 CompareString( const char * str1, const char * str2, bool bAsc)
{	
	I3ASSERT( str1 != nullptr && str2 != nullptr);

	INT32 comp = i3::generic_icompare( str1, str2);

	if(bAsc)
		return (comp == 0 ? 0 : (comp > 0 ? 1 : -1));	//	��������
	else
		return (comp == 0 ? 0 : (comp > 0 ? -1 : 1));	//	��������
}

void GetBattleTeamName(i3::wstring& out, INT32 idx, bool bIncludeTeamString)
{
	i3::sprintf(out, GAME_STRING("STBL_IDX_CLAN_BATTLE_SQUAD "), idx+1);/*%02d �д�*/
}

	//INT32 cnt = idx / maxNamingCount;
	//INT32 mod = idx % maxNamingCount;

	//switch( mod)
	//{
	//case 0:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_0");/*����*/		break;
	//case 1:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_1");/*���*/	break;
	//case 2:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_2");/*����*/		break;
	//case 3:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_3");/*��Ÿ*/		break;
	//case 4:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_4");/*����*/		break;
	//case 5:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_5");/*����*/		break;
	//case 6:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_6");/*����*/		break;
	//case 7:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_7");/*ȣ��*/		break;
	//case 8:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_8");/*�ε��*/	break;
	//case 9:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_9");/*�ٸ���*/	break;
	//case 10:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_10");/*ų��*/		break;
	//case 11:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_11");/*����*/		break;
	//case 12:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_12");/*����ũ*/	break;
	//case 13:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_13");/*����*/	break;
	//case 14:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_14");/*����ī*/	break;
	//case 15:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_15");/*����*/		break;
	//case 16:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_16");/*��*/		break;
	//case 17:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_17");/*�ι̿�*/	break;
	//case 18:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_18");/*����*/		break;
	//case 19:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_19");/*�ʰ�*/		break;
	//case 20:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_20");/*��Ŭ*/		break;
	//case 21:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_21");/*����*/		break;
	//case 22:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_22");/*����Ű*/	break;
	//case 23:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_23");/*��������*/ break;
	//case 24:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_24");/*��Ű*/		break;
	//case 25:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_25");/*�ٷ�*/		break;
	//}

	//if(bIncludeTeamString)
	//{
	//	if( cnt == 0)
	//		sprintf_s(pBuf, sizeBufMax, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_WITHOUT_NUM"), pszNaming, cnt);/*%s ��*/
	//	else if( cnt < 10)
	//		sprintf_s(pBuf, sizeBufMax, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_WITH_NUM2"), pszNaming, cnt);/*%s%02d ��*/
	//	else
	//		sprintf_s(pBuf, sizeBufMax, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_WITH_NUM1"), pszNaming, cnt);/*%s%d ��*/
	//}
	//else
	//{
	//	if( cnt == 0)
	//		sprintf_s(pBuf, sizeBufMax, "%s", pszNaming, cnt);
	//	else if( cnt < 10)
	//		sprintf_s(pBuf, sizeBufMax, "%s%02d", pszNaming, cnt);
	//	else
	//		sprintf_s(pBuf, sizeBufMax, "%s%d", pszNaming, cnt);
	//}

//}
/*
INT32 ContainString( const char * pStr1, const char * pStr2)
{    
	INT32 Len;
	INT32 Idx = 0, StartIdx = 0;

	Len = i3::generic_string_size( pStr2);

	while( *pStr1)
	{
		char c1 = ((*pStr1 >= 'a' && *pStr1 <= 'z') ? (*pStr1 - ('a' - 'A')) : *pStr1);
		char c2 = ((pStr2[Idx] >= 'a' && pStr2[Idx] <= 'z') ? (pStr2[Idx] - ('a' - 'A')) : pStr2[Idx]);

		if( c1 == c2)
		{
			Idx++;

			if( Idx >= Len)
			{
				return StartIdx + 1 - Len;
			}
		}
		else
		{
			if( Idx != 0 )
			{
				pStr1--;
			}
			Idx = 0;
		}

		pStr1++;
		StartIdx++;
	}

	return -1;
}
*/

const wchar_t* GetClanBattleMatchType(CLAN_MATCH_TYPE type)
{
	switch( type)
	{
		//case CLAN_MATCH_4:			return "4 VS 4";	// revision 37397
		case CLAN_MATCH_5:			return L"5 VS 5";
		case CLAN_MATCH_6:			return L"6 VS 6";
		case CLAN_MATCH_7:			return L"7 VS 7";
		case CLAN_MATCH_8:			return L"8 VS 8";
		case CLAN_MATCH_UNKNOWN:	
		default:
			return L"0 VS 0";
	}
}

UINT8 GetClanBattleMatchMember(CLAN_MATCH_TYPE type)
{
	switch( type)
	{
		case CLAN_MATCH_4:			return 4;
		case CLAN_MATCH_5:			return 5;
		case CLAN_MATCH_6:			return 6;
		case CLAN_MATCH_7:			return 7;
		case CLAN_MATCH_8:			return 8;
		case CLAN_MATCH_UNKNOWN:	
		default:
			return 0;
	}
}

CLAN_MATCH_TYPE	ConvertClanBattleMatchType( INT32 nMax)
{
	switch( nMax)
	{
	//case 4:		return CLAN_MATCH_4;
	case 5:		return CLAN_MATCH_5;
	case 6:		return CLAN_MATCH_6;
	case 7:		return CLAN_MATCH_7;
	case 8:		return CLAN_MATCH_8;
	}

	return CLAN_MATCH_UNKNOWN;
}


bool	CanClanInvite()
{
	if ( ! g_pConfigEx->GetCommunity().Enable_Note )
		return false;

	//	Ŭ���� ���� ����� ����Ǿ�� �մϴ�.
	if ( ! g_pConfigEx->GetClan().Enable )
		return false;

	return true;
}

bool	CanClanMemberSendNoteAll()
{
	//
	// ���� define : ENABLE_CLAN_MEMBER_SEND_NOTE
	// �� define�� �����ͷ� ������ �� �Դϴ�.
	if ( ! g_pConfigEx->GetCommunity().Enable_Note )
		return false;

	//	Ŭ���� ���� ����� ����Ǿ�� �մϴ�.
	if ( ! g_pConfigEx->GetClan().Enable )
		return false;

	return true;
}


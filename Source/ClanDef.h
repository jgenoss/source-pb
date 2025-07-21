#ifndef _CLAN_DEF_H
#define _CLAN_DEF_H

#include "ClanMark.h"

//	Ŭ�� ���
extern CLAN_RANK		GetClanRank(UINT32 nClanExp);

//	Ŭ�� �ִ� �ο���
extern INT32			GetClanStaffMax(CLAN_UNIT nUnit);

//	Ŭ�� ���̻�
extern CLAN_UNIT		GetClanUnit(INT32 nClanStaffNum);

//	Ŭ�� ������ �ʿ� ����ġ
extern UINT32			GetClanNextRankExp(UINT32 nCurExp);

//	Ŭ�� ������ �� ����ġ
extern UINT32			GetClanBeforeRankExp(UINT32 nCurExp);

//	Ŭ�� ���λ� ��������
extern void				GetClanRankPreName(i3::rc_wstring& out, UINT32 nClanExp);
extern void				GetClanRankPreName2(i3::rc_wstring& out, UINT8 nClanExpRank);

//	Ŭ���̸� ���̻� ��������
extern void				GetClanRankPostName(i3::rc_wstring& out, INT32 nClanStaffNum);
extern void				GetClanRankPostName2(i3::rc_wstring& out, UINT8 nClanPerRank);

//	Ŭ�� ��޺� Ǯ���� ��������
extern void				GetClanFullName(i3::wstring& out, UINT32 nClanExp, INT32 nClanStaffNum);
extern void				GetClanFullName2(i3::wstring& out, UINT8 nClanExpRank, UINT8 nClanPerRank);

//	��� ����
extern void				SetRankImage(i3GuiStatic * pRankStatic, UINT8 nRank);

extern void				SetClanRankImage(i3GuiStatic * pRankStatic, UINT8 nRank);

// Ŭ���� ���
extern CLAN_MEMBER_LEVEL_TYPE	GetMemberLevel(UINT8 nLevel); 

// Ŭ���� ��޳��� ��ȯ
extern void				GetMemberLevelName(CLAN_MEMBER_LEVEL_TYPE level, i3::wstring& out);

// Ŭ���� ��޳��� ��ȯ
extern void				GetMemberLevelName2(UINT8 nLevel, i3::wstring& out);

//	Ŭ���� ���� ����
extern FRIEND_STATE		GetMemberConnectState(UINT32 nState); 

//	Ŭ���� ���� ä��
extern INT32			GetMemberConnectChannel(UINT32 nState);

//	Ŭ���� ���� ��
extern INT32			GetMemberConnectRoomNum(UINT32 nState);

//	Ŭ���� ���� ���� ���ڿ� ��ȯ
extern void				GetMemberConnectStateName(UINT32 nState, i3::rc_wstring& out );

//	���� ���·� �¶��� ����
extern bool				IsOnlineUser(UINT32 nState);

//	��û�� ��¥(��)
extern UINT32			GetEntryYear(UINT32 nDate);

//	��û�� ��¥(��)
extern UINT32			GetEntryMonth(UINT32 nDate);

//	��û�� ��¥(��)
extern UINT32			GetEntryDay(UINT32 nDate);

//	üũ�ڽ� 
extern void				SetCheckBox(i3GuiCheckBox * pControl, bool bChecked);

//	Ŭ�� ����ġ �ؽ�Ʈ
extern void				SetClanExpText(i3GuiEditBox * pControl, UINT32 nCurExp, UINT32 nMaxExp);

//	Ŭ�� ����ġ ������
extern void				SetClanExpGauge(i3GuiStatic * pExpBar, REAL32 fResolutionWidth, UINT32 nCurExp, UINT32 nMaxExp);

//	Ŭ�� �ο���
extern void				SetClanStaff(i3GuiEditBox * pControl, INT32 nCurrentNum, INT32 nTotalNum);

//	Ŭ�� ������
// extern void				SetClanMaster(i3GuiEditBox * pName, i3GuiStatic * pRank, const char * pszName, UINT8 nRank);		//������ �ʴµ� �� �ϴ� �ּ�..(2014.07.10.����)

//	Ŭ�� ���
// extern void				SetClanRank(i3GuiEditBox * pRankName, i3GuiStatic * pRankImage, UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank);	//������ �ʴµ� �� �ϴ� �ּ�..(2014.07.10.����)
//extern void				SetClanRank2(i3GuiEditBox * pRankName, i3GuiStatic * pRankImage, UINT8 nClanExpRank, UINT8 nClanPerRank, UINT8 nClanRank);

//	Ŭ�� ����
extern void				SetClanOrder(i3GuiEditBox * pControl, INT32 nCurOrder, INT32 nPrevOrder);

//	Ŭ�� ����Ʈ
extern void				SetClanPoint(i3GuiEditBox * pControl, REAL32 fPoint);

//	Ŭ�� ��ũ
extern bool				SetClanMarkTex(i3GuiStatic * pControl, CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);

extern bool				SetClanMarkTex(i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);

extern bool				SetClanMarkTexEx(i3GuiStatic * pControl, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag);

extern bool				SetClanMarkTexEx(i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag);

extern bool				SetClanMarkTex(i3GuiStatic * pControl, SLOT_INFO * pInfo);

extern bool				SetDefaultClanMarkTex( i3GuiStatic * pControl, CLAN_MARK_SIZE type);

extern bool				SetDefaultClanMarkTex( i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type);

extern bool				MakeClanMarkTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, 
										INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx, 
										INT32 u = 0, INT32 v = 0);

extern bool				MakeClanMarkTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, UINT32 markFlag, INT32 u = 0, INT32 v = 0);

extern bool				MakeClanMarkShapeTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, CLAN_MARK_LAYER_TYPE LayerType, 
											  INT32 nImageIdx, INT32 nColorIdx, INT32 u = 0, INT32 v = 0);

//	BinList���� Compare�� ����
extern INT32			CompareString( const char * str1, const char * str2, bool bAsc);

//	Ŭ�� �� ���̹� ��ȯ
extern void				GetBattleTeamName(i3::wstring& out, INT32 idx, bool bIncludeTeamString = true);

//	���ڿ��� ���ԵǾ� �ִ��� ���� (��ҹ��� �Ȱ���)
//extern INT32			ContainString( const char * pStr1, const char * pStr2);

enum CLAN_MATCH_TYPE
{
	CLAN_MATCH_UNKNOWN = -1,
	CLAN_MATCH_4 = 0,			//	0������ ������ �����ؾ� ��. // revision 37397 Ŭ����ġ 4vs4 ����
	CLAN_MATCH_5,
	CLAN_MATCH_6,
	CLAN_MATCH_7,	
	CLAN_MATCH_8,

	CLAN_MATCH_MAX
};

extern const wchar_t*	GetClanBattleMatchType(CLAN_MATCH_TYPE type);		//	Ŭ���� �� Ÿ�� ���ڿ�

extern UINT8			GetClanBattleMatchMember(CLAN_MATCH_TYPE type);		//	Ŭ���� �� Ÿ�� �ɹ���

extern CLAN_MATCH_TYPE	ConvertClanBattleMatchType(INT32 nMax);		//	�� �ִ���� �� ���� Ÿ�� ��ȯ

bool	CanClanInvite();
bool	CanClanMemberSendNoteAll();


//	Ŭ�� ���� ��� ����
enum CLAN_ADMISSION_LIMIT_RANK
{
	CLAN_LIMIT_RANK_1 = 0,		//	��ü ���
	CLAN_LIMIT_RANK_2 = 4,		//	���� �̻� ���
	CLAN_LIMIT_RANK_3 = 17,		//	���� �̻� ���
	CLAN_LIMIT_RANK_4 = 31,		//	�ҷ� �̻� ���
	CLAN_LIMIT_RANK_5 = 46,		//	���� �̻� ���
};

//	Ŭ�� ���� ���� ����
enum CLAN_ADMISSION_LIMIT_AGE
{
	CLAN_LIMIT_AGE_1 = 0,		//	��ü ���
	CLAN_LIMIT_AGE_2 = 15,		//	15�� �̻�
	CLAN_LIMIT_AGE_3 = 20,		//	20�� �̻�
	CLAN_LIMIT_AGE_4 = 30,		//	30�� �̻�
};

//	Ŭ�� ���� ����
enum CLAN_STEP_AUTHORITY 
{
	CLAN_STEP_AUTHORITY_FALSE = 0,		//	���� ����
	CLAN_STEP_AUTHORITY_TRUE = 1,		//	���� �ֽ�
};

#define ADMISSION_LIMIT_CONTROL_COUNT	2	//	Ŭ�� ���� ���� ��Ʈ�� ���� (���� 2��)

#define COMPARE_ASC(a, b)			(a == b ? 0 : (a > b ? 1 : -1))
#define COMPARE_DESC(a, b)			(a == b ? 0 : (a > b ? -1 : 1))

#endif // _CLAN_DEF_H
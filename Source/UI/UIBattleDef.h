#pragma once

enum UIBATTLE_POPUP
{
	UBP_NONE = -1,

	UBP_ACCUSE_CRIME = 0,
#ifdef USE_REPORT
	UBP_USER_REPORT,
#endif
	UBP_INGAME_SHOP,

	UBP_BUYITEM,
	UBP_DELETEITEM,

	UBP_SELECT_WEAPON,
	UBP_SELECT_CHARA,
	UBP_OPTION,
	UBP_EXIT,
	UBP_SKILL_SHOP,
	UBP_HUD_USURPATION,
	UBP_HUD_ESCAPE,

	UBP_TWITCHOPTION,

	UBP_HUD_SPACE,

	UBP_MAX
};

enum UIHUD
{
	UIHUD_NONE = -1,

	UIHUD_FORCE_REMOVE = 0,
	UIHUD_INGAMECHATING,

	UIHUD_TABMINIMAP,

	UIHUD_NOTICE_SHOP,
	UIHUD_RECORD_BOARD,
	UIHUD_MANAGER,
	UIHUD_PLAYTIME_ALARM,
	UIHUD_OBSERVER,

	UIHUD_MAX
};

////////// �켱 �ӽ÷� ���⿡ ���� �ʿ��� ����ü�� �����մϴ�. 11.06.01.����
////////// 4���� �� ��� �ѹ��� ���õǱ� ������, Ŭ������ ������ �ʽ��ϴ�...

enum BATTLE_FORCIBLYREMOVE_RESULT
{
	BATTLE_FORCIBLYREMOVE_SUCCESS,
	BATTLE_FORCIBLYREMOVE_SHORTAGES,
	BATTLE_FORCIBLYREMOVE_NOALLY,
	BATTLE_FORCIBLYREMOVE_NOENEMY,
	BATTLE_FORCIBLYREMOVE_UNKNOWN_ERROR,
};

enum 
{
	INGAME_SHOP_VALID_TIME = 20
};

enum CRIME_UI_TYPE //���� �Ű� Ÿ��
{
	CRIME_UI_ForceRemove, //������¡
	CRIME_UI_UserReport, //�����Ű�
};

enum FORCE_REMOVE_UI_TYPE //���� ���� �˾� Ÿ��
{
	FORCE_REMOVE_UI_START,	//��ǥ ���� �˸� 
	FORCE_REMOVE_UI_GUIDE,	//��ǥ �ȳ�
	FORCE_REMOVE_UI_VOTE,	//��ǥ ȭ��
	FORCE_REMOVE_UI_RESULT, //��ǥ ���
};


const REAL32 INGAME_SHOP_DEATHDELAY_ROUND  = 1.f;
const REAL32 INGAME_SHOP_VALID_DISTANCE_XZ = 5.f;
const REAL32 INGAME_SHOP_VALID_DISTANCE_Y = 2.f;

/// ���� 8����Ʈ�� ����..
inline
INT32  Assemble_ForciblyRemoveResult(UINT8 slot, UINT8 agree, UINT8 disagree, BATTLE_FORCIBLYREMOVE_RESULT result_code)
{
	return  (INT32(slot) << 24) | (INT32(agree) << 16) | (INT32(disagree) << 8) | (INT32(result_code) );
}

inline 
void   Disassemble_ForciblyRemoveResult(INT32& slot, INT32& agree, INT32& disagree, BATTLE_FORCIBLYREMOVE_RESULT& result, INT32 src)
{
	slot = (src & 0xFF000000) >> 24;
	agree = (src & 0x00FF0000) >> 16;
	disagree = (src & 0x0000FF00) >> 8;
	result   = BATTLE_FORCIBLYREMOVE_RESULT(src & 0x000000FF);
}

inline INT32 AssembleValue(UINT8 bit24, UINT8 bit16=0, UINT8 bit8=0, UINT8 bit0=0)
{
	return  (INT32(bit24) << 24) | (INT32(bit16) << 16) | (INT32(bit8) << 8) | (INT32(bit0) );
}
inline void DisassembleValue(INT32 src, INT32& bit24)
{
	bit24 = (src & 0xFF000000) >> 24;
}
inline void DisassembleValue(INT32 src, INT32& bit24, INT32& bit16)
{
	bit24 = (src & 0xFF000000) >> 24;
	bit16 = (src & 0x00FF0000) >> 16;
}
inline void DisassembleValue(INT32 src, INT32& bit24, INT32& bit16, INT32& bit8)
{
	bit24 = (src & 0xFF000000) >> 24;
	bit16 = (src & 0x00FF0000) >> 16;
	bit8 = (src & 0x0000FF00) >> 8;
}
inline void DisassembleValue(INT32 src, INT32& bit24, INT32& bit16, INT32& bit8, INT32& bit0)
{
	bit24 = (src & 0xFF000000) >> 24;
	bit16 = (src & 0x00FF0000) >> 16;
	bit8 = (src & 0x0000FF00) >> 8;
	bit0   = (src & 0x000000FF);
}
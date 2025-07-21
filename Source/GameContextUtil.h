#pragma once
//�� ������ GameContext Ŭ�������� �и� ������ �Լ����� ���ؼ� ����.

namespace GameContextUtil {} 

namespace gcu = GameContextUtil;

class CUserLocationInfo;

namespace GameContextUtil
{
	bool	IsSameRoom(const CUserLocationInfo & UserLocInfo);

	i3::rc_wstring	GetRadioMessage( RADIOCHAT_TYPE nType, INT32 nMsgNum );
	i3SoundPlayState * PlayRadioMessageSound( RADIOCHAT_TYPE nType, INT32 nMsgNum, CGameCharaBase * pPlayer);

	//QA�� ��û���� ���� �׽�Ʈ�� ��ɹ�.. ���� ���񽺽ÿ��� �����ϸ� �ʵ�
	//1. ��� ���� Ŀ�ǵ� 
	//2. �ǳ� ���� 
	//3. ����ġ ���� 
	//10. ���� ���� Ŀ�ǵ� 
	//11. ų�� ���� Ŀ�ǵ� 
	//12. ������ ��Ʋ 
	void	QA_SetRank(UINT32 rank); 
	void	QA_SetPino(UINT32 Pino); 
	void	QA_SetChallenge(void); 
	void	QA_SetExp(UINT32 Exp); 
	void	QA_SetCash(UINT32 Cash); 
	void	QA_SetRound(UINT8 Red, UINT8 Blue); 
	void	QA_SetKillCount(UINT16 Red, UINT16 Blue); 
	void	QA_SetEndlessBattle(void); 
	void	QA_SetClanRank(UINT32 Pino);	//Clan RankUp
	void	QA_SetClanExp(UINT32 Pino);	//Clan Exp
	void	QA_SetClanMatchPoint( UINT32 Pino);
	void	QA_SetClanMatchWinLose( UINT32 wMatch, UINT32 wWin, UINT32 wLose );
	void	QA_SetClanMatchRoomInsert( UINT32 wCount );
	void	QA_SetMedalCount(UINT8 Type, UINT8 Number, UINT8 Count); 

	// UI�� ��û���� �̴ϸ� ��ġ ���� Ŀ�ǵ�
#ifdef UITEST_MINIMAP_MOVE
	void	UITest_SetMinimapMove(bool enable);
	bool	UITest_IsMinimapMoveEnable();
#endif

	// debug
	void	CheckWeaponList( const ITEM_INFO * pList );

	INT32	getKillRate(UINT32 iKill, UINT32 iDeath);
	
}

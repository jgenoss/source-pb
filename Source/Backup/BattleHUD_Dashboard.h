#if !defined( __BATTLE_HUD_DASHBOARD_H)
#define __BATTLE_HUD_DASHBOARD_H

#if ������_�͵�

#include "BattleHUD_Define.h"
#include "BattleGUI_Define.h"
#include "MovingText.h"

enum HUD_TEXT
{
	HUD_TEXT_TIME = 0,
	HUD_TEXT_COLON,
	HUD_TEXT_INFO_RED,
	HUD_TEXT_INFO_BLUE,	
	HUD_TEXT_INFO_GOAL,

	HUD_TEXT_COUNT,
};

enum HUD_BATTLE_INFO
{
	HUD_BATTLE_INFO_SCORE = 0,

	HUD_BATTLE_INFO_TEAM_RED,					// �ڽ��� �������� ǥ�� : RED
	HUD_BATTLE_INFO_TEAM_BLUE,				// �ڽ��� �������� ǥ��	: BLUE
	HUD_BATTLE_INFO_CLAN,
	HUD_BATTLE_INFO_TIME,
	HUD_BATTLE_INFO_LIMIT,
	HUD_BATTLE_INFO_ESCAPEARROW,				//Ż��̼� ������ ���� ǥ��
	
	HUD_BATTLE_INFO_COUNT,
};

enum HUD_BATTLE_INFO_DINO
{
	HUD_BATTLE_INFO_ESCAPESHILD_0,				//Ż��̼� ���潺 �����
	HUD_BATTLE_INFO_ESCAPESHILD_1,				//Ż��̼� ���潺 �ݹ���
	HUD_BATTLE_INFO_ESCAPESHILD_2,				//Ż��̼� ���潺 Ǯ����
	HUD_BATTLE_INFO_ESCAPESHILD_3,				//Ż��̼� ���潺 Ǯ����
	HUD_BATTLE_INFO_ESCAPESHILD_4,				//Ż��̼� ���潺 Ǯ����

	HUD_BATTLE_INFO_DINO_COUNT,


};


class CHud;

class CHUD_Dashboard : public i3GameObjBase
{
	I3_CLASS_DEFINE( CHUD_Dashboard, i3GameObjBase);
protected:
	CHud *			m_pHUD;
	

	CMovingText*	m_pMovingText;
	// TopScore ����
	INT32			m_RedWinScore;				// RED Team�� �¸� ����. MOD�� ���� KillCount�� ����, Round �� �ϼ��� �ִ�.
	INT32			m_BlueWinScore;				// BLUE Team�� �¸� ����. MOD�� ���� KillCount�� ����, Round �� �ϼ��� �ִ�.

	i3Texture *		m_pTex;
	i3Texture *		m_pTexDino;
	i3Sprite2D *	m_pSpriteBattleInfo;
	i3Sprite2D *	m_pSpriteBattleInfoDino;
	i3TextNodeDX2 *	m_pText[HUD_TEXT_COUNT];

	


protected:
	void			UpdateTime();
	void			UpdateRound();

	REAL32			m_updateTimer;
	INT32			m_iRedScore;
	INT32			m_iBlueScore;

	INT32			m_iTempRedScore;
	INT32			m_iTempBlueScore;

public:
	CHUD_Dashboard(void);
	virtual ~CHUD_Dashboard(void);

	virtual bool	Create( CHud * pHUD);
	virtual void	Init(void);

	virtual void	Update( REAL32 tm);

	void			SetScore( INT32 red, INT32 blue);
	void			SetEscapeScore(INT32 RedScore, INT32 BlueScore);
	virtual void	SetEnableWithoutNotice( BOOL bFlag, GAME_GUI_STATUS nState, BOOL bDeath);
};

#endif

#endif

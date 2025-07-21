#pragma once

#include "iHudBase.h"
#include "ui/NumberCount.h"
#include "UIHudDefine.h"

class UIHudManager;

namespace dboard
{
	struct target;
}

class UIHudDashBoard : public iHudBase
{
public:
	UIHudDashBoard(UIHudManager* p);
	virtual ~UIHudDashBoard();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void ClearAtUnload( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

	const VEC3D* GetTimeCtrlPos() const;
	REAL32 GetTimeCtrlHeight() const;

	void SetRidingHelicopter(bool bRiding);

private:
	void change_target_shape();

	void InitializeAtLoad_Common(i3UIScene* pScene);
	void InitializeAtLoad_1_5(i3UIScene* pScene);
	void InitializeAtLoad_1_0(i3UIScene* pScene);
	void (UIHudDashBoard::*pf_InitializeAtLoad[HUD::HS_MAX])(i3UIScene* pScene);

	void init_team_mark();

private:
	virtual void SetParameter(INT32 arg1 =0, INT32 arg2 =0) override;

private:
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_respawn_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;

private:
	i3UIImageBox* m_pTutorial;
	i3UIFrameWnd* m_pScore;

	enum	{ RED_TEAM = 0, TANK_MAIN = 0, BLUE_TEAM = 1, TANK_SUB = 1, MAX_TEAM, };
	NumberCount m_Score[MAX_TEAM];
	i3UIButtonImageSet* m_TeamMark[MAX_TEAM];
	i3UIButtonImageSet* m_TeamBg; //��(����,���) ǥ��

	enum	{	MINUTE, SECOND, MAX2,	};
	NumberCount m_Time[MAX2];
	i3UIStaticText* m_TimeSeparate; //':'

	NumberCount m_Round;	//���� ���, ų ��� ���
	i3UIImageBox* m_RoundInfinite; //ä���� ��� ���(���� ����)
	i3UIButtonImageSet* m_ShieldGauge;		//Ż�� �̼Ǹ� ���

#if ENABLE_UNIT_TEST
	i3UIButtonImageSet* m_RoundCtrl; //Hud Unit Test ������ ����..
	VEC3D m_RoundCtrl_Origin_Pos;
#endif

	//DEFAULT_SHAPE : ---- : ----   //PARALLEL_SHAPE : ====
	enum	{	DEFAULT_SHAPE = 0, PARALLEL_SHAPE, MAX_SHAPE,	};
	dboard::target* m_target[MAX_SHAPE][MAX_TEAM];	// Ÿ�� ���(����,�ı�,���) ���
	dboard::target* m_neutrality_target;			// ���� �̼� �ƿ�����Ʈ ��� ���

	bool m_enable_update_time;
};
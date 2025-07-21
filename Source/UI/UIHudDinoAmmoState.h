#pragma once

#include "UIHudState.h"
#include "HudState_1_0.h"

class UIHudManager;

class UIHudDinoAmmoState : public UIHudState
{
public:
	UIHudDinoAmmoState(UIHudManager* p);
	virtual ~UIHudDinoAmmoState();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

private:
	//Player&AmmoState �� ���� ����ϱ⿡ Player&AmmoState�� Ưȭ ��Ʈ�ѵ��� �����Ѵ�.
	void initialize_player_ammo_control(i3UIScene * pScene);
	
	void enable_dino_ammo_control();
	void disable_player_ammo_control(); 

private:
	virtual void event_respawn_battle_impl(INT32 arg) override;

private:
	hs_1_0::control dino_control;
};
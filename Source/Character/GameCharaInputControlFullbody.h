#ifndef _GAME_CHARA_INPUT_CONTROL_FULLBODY_H_
#define _GAME_CHARA_INPUT_CONTROL_FULLBODY_H_

#include "GameCharaInputControl.h"

// ��Ӻ��ٴ� ��������. ��Ʈ��ũ ��ǲ���� ��밡���� ��ɸ� �������̽��� ����.
// Update() �κп����� ��Ʈ��ũ ��ǲ�� ���ʿ��� ���� ����.
class CGameCharaInputControlNetwork;
class CGameCharaInputControlFullbody : public CGameCharaInputControl
{
	I3_CLASS_DEFINE(CGameCharaInputControlFullbody, CGameCharaInputControl);

public:
	virtual void	OnFirstUpdate(REAL32 tm) override;
	virtual void	OnUpdate(REAL32 tm) override;

	void			ProcessNetworkPosition(GAMEINFO_CHARA_NETDATA* info);
	void			ProcessNetworkRotationTheta(GAMEINFO_CHARA_NETDATA* info);
	void			ProcessNetworkRotationRho(GAMEINFO_CHARA_NETDATA* info);

public:
	void			SetNetworkInputControl(CGameCharaInputControlNetwork* adaptee);

private:
	CGameCharaInputControlNetwork* m_controlNetwork = nullptr;
};

#endif
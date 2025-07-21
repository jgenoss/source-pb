#ifndef _UIHUD_TABMINIMAP_DOMINATION_H_
#define _UIHUD_TABMINIMAP_DOMINATION_H_


#include "UIHUDBase.h"

namespace minimap_new
{
	class Unit;
}

struct UserInfoSlot
{
	i3UIFrameWnd*       slot;						// ���� ������

	i3UIImageBox*       backImage;					// �⺻ ����̹���
	i3UIImageBox*		deadImage;					// ���� ǥ�� ����̹���

	i3UIButtonImageSet* rank;						// ��ũ

	i3UIStaticText*		name;						// �г���

	i3UIStaticText*		hpTitle;					// "HP"
	i3UIProgressBar*	hpBar;						// HP ���α׷�����
	i3UIImageBox*		hpBarBackImage;				// HP ���α׷����� ����̹���

	i3UIStaticText*		spTitle;					// "SP"				
	i3UIProgressBar*	spBar;						// SP ���α׷�����
	i3UIImageBox*		spBarBackImage;				// SP ���α׷����� ����̹���

	i3UIStaticText*		scoreTitle;					// "Score"
	i3UIStaticText*		score;						// ȹ�� ����

	i3UIStaticText*		weaponTitle;				// "Primary Weapon"
	i3UIStaticText*		weapon;						// �ֹ��� ��
};

static const INT32 maxSlotNum = 4;

struct StageInfo
{
	i3UIStaticText* serverChannelRoom;				// [1 Server 3 Channel 31 Room]
	i3UIStaticText* dayWaveDesc;					// Day 1 - Wave 3. Survival
	i3UIStaticText* mapName;						// Black Panther
	i3UIStaticText* wave;							// WAVE
	i3UIStaticText* curWave;						// 00
	i3UIStaticText* maxWave;						// / 00
	i3UIStaticText* missionDesc;					// 3�� ���� �����϶�!
};

class UIHUDTabMinimapDomination : public UIHUDBase
{
	I3_CLASS_DEFINE(UIHUDTabMinimapDomination, UIHUDBase);

public:
	UIHUDTabMinimapDomination();
	virtual ~UIHUDTabMinimapDomination();

	virtual void	_InitializeAtLoad(i3UIScene* scene) override;

	virtual void	OnCreate(i3GameNode* nodeParent) override;
	virtual void	OnUpdate(REAL32 tm) override;
	virtual bool	OnEntranceStart(void* arg1 = nullptr, void* arg2 = nullptr) override;
	void			Show(bool activate);
	void			UpdateInfo();

	const i3::vector<minimap_new::Unit*>&	GetUnitList() const	{ return m_units; }
	i3::vector<minimap_new::Unit*>&		GetUnitList()		{ return m_units; }

private:
	void			_CreateMinimap(i3UIScene* scene);
	void			_CreateUnits();

	void			_SetStageInfo();
	void			_SetUserInfo();

	void			_InitSlotControls(i3UIScene* scene);
	void			_ShowSlot(INT32 num, INT32 slotIdx);
	void			_HideSlot(INT32 num);
	void			_HideAllSlot();

	bool			_IsMinimapUpdatable() const  { return m_bMinimapUpdatable;  }
	bool			_IsUserInfoUpdatable() const { return m_bUserInfoUpdatable; }
	bool			_IsActivated() const		 { return m_bActivate;			}

private:
	bool				m_bActivate;
	bool				m_bMinimapUpdatable;
	bool				m_bUserInfoUpdatable;

	UserInfoSlot		m_slot[maxSlotNum];
	StageInfo			m_stageInfo;

	i3::vector<minimap_new::Unit*> m_units;
	i3::vector<INT32>  m_ranking;
};


#endif
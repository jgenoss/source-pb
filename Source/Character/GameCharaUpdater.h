/**
 * \file	Character\GameCharaUpdater.h
 *
 * Declares the game chara updater class.
 */


#if !defined( __GAME_CHARA_UPDATER_H__)
#define __GAME_CHARA_UPDATER_H__

#include "GameCharaControlBase.h"
#include "../Weapon/WeaponBase.h"

class CGameCharaUpdater : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaUpdater, CGameCharaControlBase );
protected:
	bool			m_bObserverStart = false;		// ������ GM�������� ��� �������� ��ũ�� ���� �ʾ� �������� ������� �Ѵ�. �׶� �ʿ��� ����

public:	
	void			DoFirstUpdate( REAL32 rDeltatime);
	virtual void	OnFirstUpdate( REAL32 rDeltatime);
	virtual void	OnFirstUpdateFullbody(REAL32 tm);
	void			DoUpdate( REAL32 rDeltatime);
	virtual void	OnUpdate( REAL32 rDeltatime);
	void			DoLastUpdate( REAL32 rDeltatime);
	virtual void	OnLastUpdate( REAL32 rDeltatime);

	virtual void	OnProcessInvincible( REAL32 rDeltatime);
	virtual void	OnProcessJump( REAL32 rDeltatime);

	/** \brief Packet�� ������ ó���ϴ� �Լ�
		\note for Only Local */
	void	ProcessLocalFromRecvPacket( void);

	/** \brief Respawn�� ����մϴ�. */
	void	ProcessRespawn( REAL32 rDeltatime);

	/** \brief ���� ȿ�� ���⿡ ���� ����� �մϴ�. */
	void	ProcessPhysixSleep( REAL32 rDeltatime);

	/** \brief ���� ȿ�� ���⿡ ���� ����� �մϴ�.
		\note Object������ ���� ��� ó�� */
	void	ProcessPhysixSleepForOnObject( REAL32 rDeltatime);

	/** \brief ���� ȿ���� ���� ó���� �մϴ�. */
	void	ProcessRecalcPhysixPos( REAL32 rDeltatime);

	/** \brief ���� ��� ó���մϴ�. */
	void	ProcessDeathState( REAL32 rDeltatime);

	/** \brief ĳ������ �̵� ������ �ϴ� Debug code */
	void	DebugProcess( REAL32 rDeltatime);

	/** \brief Graze sound�� �˻��մϴ�. */
	void	CheckGrazeSoundPlay(void);

	/** \brief Graze sound�� ó���մϴ�. */
	void	GrazeSoundPlay(const i3::vector<WeaponRocketLauncherShell*>& List);

	virtual void	Reset(void)		{ m_bObserverStart	= false; }
};

#endif

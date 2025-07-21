/**
 * \file	GameCharaUpdateContext.h
 *
 * Declares the game chara update context class.
 */


#if !defined( __GAME_CHARA_UPDATE_CONTEXT_H__)
#define __GAME_CHARA_UPDATE_CONTEXT_H__

#include "GameCharaContextBase.h"

class CGameCharaUpdater;
class CGameCharaUpdater1PV;
class CGameCharaUpdater3PV;
class CGameCharaUpdater3PVProfile;

class CGameCharaUpdaterFullbody;

class CGameCharaUpdateContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaUpdateContext, CGameCharaContextBase);
protected:
	// Updater
	CGameCharaUpdater *				m_pCurrentUpdater = nullptr;
	CGameCharaUpdater1PV *			m_p1PVUpdater = nullptr;
	CGameCharaUpdater3PV *			m_p3PVUpdater = nullptr;
	CGameCharaUpdater3PVProfile *	m_p3PVProfileUpdater = nullptr;
	CGameCharaUpdaterFullbody *		m_pFullbodyUpdater = nullptr;
	REAL32			m_rHideTimer = 0.0f;
	REAL32			m_rToHideTime = 0.0f;

	// ĳ���� ����� ���� ���� Ÿ�� (PhysX off������ ����)
	REAL32			m_rDeathBlendTime = 0.0f;
	// ĳ���� ����� �̵��� ���̰� (PhysX off������ ����)
	REAL32			m_rDeathBlendHeight = 0.0f;
	// ĳ���� ������� ���� (PhysX off������ ����)
	REAL32			m_rDeathOriginalHeight = 0.0f;

	REAL32			m_rDeathTime = 0.0f;		// ���п��� AIĳ���� ���� �� �����ð��� ������ ĳ���� �����Ҷ� ����

public:
	/** \brief Hide time�� ��ȯ�մϴ�.
		\note hide time : ĳ���� ��� �� ������� �ð� */
	REAL32			getHideTimer( void)							{ return m_rHideTimer; }

	/** \brief Hide time�� �����մϴ�. */
	void			setHideTimer( REAL32 tm)					{ m_rHideTimer = tm; }
	
	/** \brief Hide ��ǥ �ð��� ��ȯ�մϴ�. */
	REAL32			getToHideTime( void)						{ return m_rToHideTime; }

	/** \brief Hide ��ǥ �ð��� �����մϴ�. */
	void			setToHideTime( REAL32 tm)					{ m_rToHideTime = tm; }

	/** \brief Death Blend time�� ��ȯ�մϴ�.
		\note ���� �ִϸ��̼��� ����ϴ� ��� ���� ������ ����� ����մϴ�.
			������� Blend�� ����Ҷ� ����մϴ�. */
	REAL32			getDeathBlendTime( void)					{ return m_rDeathBlendTime; }

	/** \brief Death Blend time�� �����մϴ�. */
	void			setDeathBlendTime( REAL32 tm)				{ m_rDeathBlendTime = tm; }

	/** \brief Death Blend ���̸� ��ȯ�մϴ�. */
	REAL32			getDeathBlendHeight( void)					{ return m_rDeathBlendHeight; }

	/** \brief Death Blend ���̸� �����մϴ�. */
	void			setDeathBlendHeight( REAL32 height)			{ m_rDeathBlendTime = 0.f; m_rDeathBlendHeight = height; }

	/** \brief ���� �� �������κ����� ���̸� ��ȯ�մϴ�. */
	REAL32			getDeathOriginalHeight( void)				{ return m_rDeathOriginalHeight; }

	/** \brief ���� �� �������κ����� ���̸� �����մϴ�. */
	void			setDeathOriginalHeight( REAL32 height)		{ m_rDeathOriginalHeight = height; }

	void			UpdateDeathTime(REAL32 time)				{ m_rDeathTime	+= time;		}
	REAL32			getDeathTime(void) const					{ return m_rDeathTime;			}

	CGameCharaUpdater * getUpdater( void)			{ return m_pCurrentUpdater; }

public:
	virtual ~CGameCharaUpdateContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	// change view
	virtual void	OnChange1PV() override;
	virtual void	OnChange3PV() override;

	virtual void	OnChangeLocal() override;
	virtual void	OnChangeNetwork() override;
	virtual void	OnChangeNPC() override;
	virtual void	OnChangeProfile() override;
	virtual void	OnChangeFullbody() override;
	
	virtual void	OnFirstUpdate( REAL32 rDeltatime) override;
	virtual void	OnUpdate( REAL32 rDeltatime) override;
	virtual void	OnLastUpdate( REAL32 rDeltatime) override;
};

#endif

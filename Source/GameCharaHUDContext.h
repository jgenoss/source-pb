/**
 * \file	GameCharaHUDContext.h
 *
 * Declares the game chara hud context class.
 */


#if !defined( __GAME_CHARA_HUD_CONTEXT_H__)
#define __GAME_CHARA_HUD_CONTEXT_H__

#include "GameCharaContextBase.h"

class CGameCharaHUDControl;
class CGameCharaHUDControlDino;
class CGameCharaHUDControlRaptor;
class CGameCharaHUDControlTRex;
class CGameCharaHUDControlSting;
class CGameCharaHUDControlElite;
class CHud;


class CGameCharaHUDContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaHUDContext, CGameCharaContextBase);

	friend class Avatar;
	friend class CGameCharaBase;
	friend class CGameCharaDino;
	friend class CGameCharaDinoRaptor;
	friend class CGameCharaDinoTRex;
	friend class CGameCharaDinoSting;
	friend class CGameCharaDinoAcid;
	friend class CGameCharaDinoTank;
	friend class CGameCharaDinoElite;

	friend class CGameCharaDinoCCRaptor;
	friend class CGameCharaDinoCCSting;
	friend class CGameCharaDinoCCAcid;
	friend class CGameCharaDinoRaptorMercury;
	friend class CGameCharaDinoStingMars;
	friend class CGameCharaDinoAcidVulcan;

private:
	CGameCharaHUDControl *		m_pCurrentControl = nullptr;
	CGameCharaHUDControl *		m_pHUDControl = nullptr;
	CGameCharaHUDControlRaptor *m_pRaptorHUDControl = nullptr;
	CGameCharaHUDControlTRex *	m_pTRexHUDControl = nullptr;
	CGameCharaHUDControlSting *	m_pStingHUDControl = nullptr;
	CGameCharaHUDControlElite *	m_pEliteHUDControl = nullptr;

	COLOR				m_EFFECT_ScreenColor = { 0, 0, 0, 255 };
	REAL32				m_fBlurStart = 0.0f;
	REAL32				m_fBlurEnd = 0.0f;
	SCR_EFFECT_TYPE		m_nScrEffectType = SCR_EFFECT_NONE;

	REAL32				m_rFlashElapseTime = 0.0f;				// ���� ȿ�� ���� Ÿ��
	REAL32				m_rFlashEffectiveTime = 0.0f;			// ���� ���� �ѽð�(�ѽð��� ���Ǿ ����ȴ�)
	REAL32				m_rDamageElapseTime = 0.0f;				// ������ ���� ȿ��
	REAL32				m_rBlowElapseTime = 0.0f;				// ���� ȿ�� ���� Ÿ��
	REAL32				m_rBlowEffectiveTime = 0.0f;			// ���� ���� �ѽð�(�ѽð��� ���Ǿ ����ȴ�)
	
private:
	/** \brief World��ǥ�� ���� ĳ���͸� �������� 2D ��ǥ�� ����մϴ�. */
	bool			__CalcProjectionPos( REAL32 * pOutX, REAL32 * pOutY, VEC3D * pIn );


public:
	REAL32			getDamageElapseTime( void)		{ return m_rDamageElapseTime; }

public:
	CGameCharaHUDContext() {}
	virtual ~CGameCharaHUDContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnChange1PV( void) override;
	virtual void	OnChangeTrex() override;
	virtual void	OnChangeSting() override;
	virtual void	OnChangeRaptor() override;
	virtual void	OnChangeElite();

	virtual void	OnChangeCCRaptor();
	virtual void	OnChangeCCSting();
	virtual void	OnChangeCCAcid();
	virtual void	OnChangeRaptorMercury();
	virtual void	OnChangeStingMars();
	virtual void	OnChangeAcidVulcan();


	virtual void	OnReset( void) override;

	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief ScreenEffect ��� ���� ���� */
	bool			IsEffectAvailable( void);

	/** \brief ScreenEffect�� �ʱ�ȭ�Ѵ�. */
	void			ClearScreenEffect( void);

	/** \brief ScreenEffect Flashbang�� �����Ѵ�.
		\param[in] pVecWeaponPos flashbang ��ġ
		\param[in] rEffectiveTime ���� �ð�
		\param[in] rExplosionRange ���� �Ÿ�
		\param[in] rExplosionSlope ����
		\param[in] rDamagedLength Ÿ�� �Ÿ� */
	bool			ScreenEffect_Flashbang( VEC3D * pVecWeaponPos, REAL32 rEffectiveTime, REAL32 rExplosionRange, REAL32 rExplosionSlope, REAL32 rDamagedLength);
	
	/** \brief Jumper ȿ���� �����Ѵ�.
		\param[in] rProcessTime ���� �ð� */
	void			ScreenEffect_Jumper( REAL32 rProcessTime);

	void			ScreenEffect_Blow( REAL32 rProcessTime);

	/** \brief Howling ȿ���� �����Ѵ�.
		\param[in] rEffectTime ���� �ð� */
	bool			ScreenEffect_Howl( REAL32 rEffectTime);

	/** \brief NickName ǥ�ø� �����Ѵ�.
		\param[in] bFriendly �Ʊ� ����		*/
	void			ShowCharacterNick( bool bFriendly);

	/** \brief Scope�� �����Ѵ�. 
		\param[in] bEnable ��� ����
		\param[in] pWeapon ���� pointer		*/
	void			SetScope( bool bEnable, WeaponBase * pWeapon = nullptr);

	/** \brief �̼ǿ� message�� �����Ѵ�.
		\param[in] pszMessage
		\param[in] lifeTime ������ Ÿ��
		\param[in] offsetX X ��ǥ
		\param[in] offsetY Y ��ǥ */
	void			ShowMissionNotice( const i3::rc_wstring& wstrMessage, REAL32 lifeTime = 3.f, INT32 offsetX = 0, INT32 offsetY = 0);

	/** \brief ���� ����� ȣ�� */
	void			ChangeWeapon( void);

	SCR_EFFECT_TYPE GetCurScreenEffect() const { return m_nScrEffectType; }

protected:
	/** \brief Crosshair�� �����Ѵ�. */
	void			_ApplyCrosshair( void);

	/** \brief Damage ȿ���� ǥ���Ѵ�.
		\param[in] pDir ����
		\param[in] bDirectionView Damage ���� ǥ�� ����		*/
	void			_Damage( VEC3D * pDir = nullptr, bool bDirectionView = false, INT32 filter_index = -1 );
};

#endif

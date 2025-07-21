/**
 * \file	GameCharaSoundContext.h
 *
 * Declares the game chara sound context class.
 */


#if !defined( __GAME_CHARA_SOUND_CONTEXT_H__)
#define __GAME_CHARA_SOUND_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaDef.h"

enum RUNSOUND_MATERIAL
{
	RUNSOUND_THICKWOOD,
	RUNSOUND_CONCRETE,
	RUNSOUND_METAL,
	RUNSOUND_WOOD,
	RUNSOUND_TYPE_COUNT,
};

enum CHARA_SOUND
{
	CHARA_SOUND_DAMAGE_DEFAULT = 0,
	CHARA_SOUND_DAMAGE_LEFTARM,
	CHARA_SOUND_DAMAGE_RIGHTARM,
	CHARA_SOUND_DAMAGE_CHEST,

	CHARA_SOUND_RUN_COMMON_RIGHT,
	CHARA_SOUND_RUN_COMMON_LEFT,
	CHARA_SOUND_RUN_CONCRETE_RIGHT,
	CHARA_SOUND_RUN_CONCRETE_LEFT,
	CHARA_SOUND_RUN_METAL_RIGHT,
	CHARA_SOUND_RUN_METAL_LEFT,
	CHARA_SOUND_RUN_THICKWOOD_RIGHT,
	CHARA_SOUND_RUN_THICKWOOD_LEFT,
	CHARA_SOUND_RUN_WOOD_RIGHT,
	CHARA_SOUND_RUN_WOOD_LEFT,

	CHARA_SOUND_COUNT
};

enum CHARA_SOUND_TYPE
{
	CHARA_SOUND_TYPE_MALE = 0,
	CHARA_SOUND_TYPE_FEMALE,

	CHARA_SOUND_TYPE_COUNT
};

class CGameCharaBase;
class CGameCharaSoundControl;
class CGameCharaSoundControlDino;


class CGameCharaSoundContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaSoundContext, CGameCharaContextBase);

	friend class CGameCharaBase;
	friend class CGameCharaMoveContext;

protected:
	CGameCharaSoundControl *		m_pCurrentSoundControl = nullptr;
	CGameCharaSoundControl *		m_pSoundControl = nullptr;
	CGameCharaSoundControlDino *	m_pDinoSoundControl = nullptr;

	i3GameResSound *				m_pCharaSoundRes[ CHARA_SOUND_COUNT];
	i3SoundPlayState *				m_pCharaSoundState[ CHARA_SOUND_COUNT];

public:
	CGameCharaSoundContext();
	virtual ~CGameCharaSoundContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	// change view
	virtual void	OnChange1PV( void) override;
	virtual void	OnChange3PV( void) override;
	
	/** \brief ���ڱ� �Ҹ��� play�Ѵ�.
		\param[in] nType ���� type
		\param[in] bLeftfoot �޹� ����
		\param[in] bLisstener 1��Ī ���� */
	bool			PlayFootStep( I3_TERRAIN_TYPE nType, bool bLeftfoot, bool bListener);

	
	/** \brief 1��Ī ���ο� ���� ���� ���� */
	void			SetSoundOwner( void);

	/** \brief Scope Sound�� Play�Ѵ�.
		\param[in] pWeapon ���� Pointer
		\param[in] bZoomIn ZoomIn ���� */
	void			PlayScopeSound( WeaponBase * pWeapon, bool bZoomIn = true);

protected:
	/** \brief ĳ���� ��Ʈ ���带 Play�Ѵ�. */
	void			_PlayCharaHitSound( CHARA_DAMAGEINFO * pDamageInfo, VEC3D * pPos, bool bListener);

	/** \brief ĳ���� ���� ���带 Play�Ѵ�. */
	void			_PlayJumpStartSound( void);

	/** \brief ĳ���� ���� ���带 Play�Ѵ�. */
	void			_PlayJumpEndSound( void);
};

#endif

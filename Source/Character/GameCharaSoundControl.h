/**
 * \file	Character\GameCharaSoundControl.h
 *
 * Declares the game chara sound control class.
 */


#if !defined( __GAME_CHARA_SOUND_CONTROL_H__ )
#define __GAME_CHARA_SOUND_CONTROL_H__

#include "GameCharaControlBase.h"

class CGameCharaSoundControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaSoundControl, CGameCharaControlBase);

public:

	/** \brief ĳ���� Ÿ�� ���带 Update�մϴ�. */
	virtual void	OnHit( CHARA_DAMAGEINFO * pDamageInfo, VEC3D * pPos, bool bListener);

	/** \brief ���� ���带 Update�մϴ�. */
	virtual void	OnJumpStart( void);

	/** \brief ���� ���带 Update�մϴ�. */
	virtual void	OnJumpEnd( void);
};

#endif

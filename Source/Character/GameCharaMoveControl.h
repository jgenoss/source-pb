/**
 * \file	Character\GameCharaMoveControl.h
 *
 * Declares the game chara move control class.
 */


#if !defined( __GAME_CHARA_MOVE_CONTROL_H__ )
#define __GAME_CHARA_MOVE_CONTROL_H__

#include "GameCharaControlBase.h"

class CGameCharaMoveControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaMoveControl, CGameCharaControlBase);

public:
	virtual void	OnUpdate( REAL32 rDeltatime);

	/** \brief �ɱ� ���⿡ ���� ó���� �մϴ�. */
	virtual void	OnChangeHeight( REAL32 rHeight);

	/** \brief �ٴ� ������ �˻��մϴ�. */
	virtual void	OnProcessFloorCheck( REAL32 rDeltatime);

	/** \brief �ٴ��� ������ �˻��մϴ�. */
	void			CheckFloorTerrain( void);
};

#endif

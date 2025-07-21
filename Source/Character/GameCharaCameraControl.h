/**
 * \file	Character\GameCharaCameraControl.h
 *
 * Declares the game chara camera control class.
 */


#if !defined( __GAME_CHARA_CAMERA_CONTROL_H__ )
#define __GAME_CHARA_CAMERA_CONTROL_H__

#include "GameCharaControlBase.h"
#include "GameCharaCameraControlDef.h"

class CGameCharaCameraControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaCameraControl, CGameCharaControlBase);

public:
	/** \brief FPS Camera�� �����մϴ�. */
	virtual bool	OnChangeFPSCamera( void);

	/** \brief Kill Camera�� �����մϴ�. */
	virtual bool	OnChangeKillCamera( INT32 idx)	{ return false; }

	/** \brief Observer Camera�� �����մϴ�. */
	virtual bool	OnChangeObserverCamera( void);

	/** \brief Observer Camera���� Zoom�� �����մϴ�. */
	virtual bool	OnSetObserveZoom(void);

	/** \brief Zoom���� �����մϴ�. */
	virtual bool	OnSetZoom( void);

	/** \brief Respawn�� ó���ϴ� �Լ��Դϴ�. */
	virtual bool	OnProcessRespawn( void);
};

#endif

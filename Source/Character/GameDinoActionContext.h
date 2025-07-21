#if !defined( __GAME_DINO_ACTION_CONTEXT_H__)
#define __GAME_DINO_ACTION_CONTEXT_H__

#include "GameCharaActionContext.h"

class CGameDinoActionContext : public CGameCharaActionContext
{
	I3_CLASS_DEFINE( CGameDinoActionContext, CGameCharaActionContext);

	friend class CGameCharaBase;

protected:
	/** \brief ����������. */
	void	_Play_Dino_Roar( void);
	/** \brief ��⵿��. */
	void	_Play_Dino_Idle( void);
	/** \brief ���ϻ���. */
	void	_Play_Dino_Stun( void);
};

#endif

#if !defined( __UI_FLOAT_MINIMISSONINFO_H__)
#define __UI_FLOAT_MINIMISSIONINFO_H__

#include "UIFloatingBase.h"

class UIFloatMiniMissionInfo : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatMiniMissionInfo);

public:
	UIFloatMiniMissionInfo();
	virtual ~UIFloatMiniMissionInfo();

	virtual void	OnCreate( i3GameNode * pParent);

	/** \brief �ּ� ũ�� */
	virtual void	OnMinimize( void);

	/** \brief �ִ� ũ�� */
	virtual void	OnMaximize( void);

	/** \brief ���� ũ�� */
	virtual void	OnPreSize( void);

	virtual bool	OnSliding( REAL32 rDeltaSeconds);
};

#endif

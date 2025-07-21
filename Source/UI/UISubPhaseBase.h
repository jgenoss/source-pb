#if !defined( __UI_SUBPHASE_BASE_H__)
#define __UI_SUBPHASE_BASE_H__

#include "UIPhaseBase.h"

class UISubPhaseBase : public UIPhaseBase
{
	I3_ABSTRACT_CLASS_DEFINE( UISubPhaseBase);

public:
	UISubPhaseBase();
	virtual ~UISubPhaseBase();
	virtual void	ProcessGameEvent( INT32 event, INT32 arg);
	
	/** \brief SubPhase�� �ڽ��� Sliding�� ������Ʈ�մϴ�. */
	virtual void	OnUpdate( REAL32 rDeltaSeconds);

	
};

#endif

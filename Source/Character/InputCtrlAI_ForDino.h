#if !defined( __INPUT_CTRL_AI_FOR_DINO_H__)
#define __INPUT_CTRL_AI_FOR_DINO_H__

#include "GameCharaInputControlAI.h"

//������->���� �н��� �̵� ->�̵��� ���� ������ ���� -> ���� �������� �ٽ� �̵�

class CInputCtrlAI_ForDino : public CGameCharaInputControlAI
{
	I3_CLASS_DEFINE( CInputCtrlAI_ForDino, CGameCharaInputControlAI);

public:
	virtual bool			MoveToTarget( i3GameObj* pTarget, REAL32 rMinLength, REAL32 rDeltaSeconds) override;
};

#endif // __INPUT_CTRL_AI_FOR_DINO_H__

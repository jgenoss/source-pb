#pragma once

#include "iHudBase.h"
#include "UISlide.h"

class UIHudManager;

#define MAINTAIN_HUD_SEC 1.3
#define MAX_BG_SHAPE_STATE 11
#define UI_STATE_CHANGE_DIFF 5
#define BG_STATE_CHANGE_TERM 0.05f

namespace QE
{
	enum EFFECT_STATE
	{
		NONE,					// �ִϸ��̼� �ǰ����� ���� ����. ���� �ִϸ��̼� ������ ��ٸ�.
		START,					// �ִϸ��̼��� ������. ��ũ�� �ʱ� ��ġ ���� �� Visible ó��.
		MOVE_MARK,				// ��ũ�� ��ġ�� ������ ��ġ�� �̵��ϴ� ����. UI_STATE_CHANGE_DIFF ��ŭ Update���� x�� �̵���.
		MOVE_MARK_END,			// ��ũ�� �̵��� �������� ������ �������� �ִϸ��̼��� ����. �ʱ� ũ�� ���� �� Visible ó��.
		RESIZE_FRAME,			// ������ �����츦 ������¡ �ϴ� ����. ���������� UI_STATE_CHANGE_DIFF ��ŭ Update���� Ŀ��.
		RESIZE_FRAME_END,		// ������ �������� ������¡�� �������� ���� �� �� �ؽ�Ʈ���� Visible ó����. ����Ʈ Ŭ���� ���ο� ���� ���� ��Ʈ�ѵ� �ʱ� ������.
		CHANGE_BGSTATE,			// �Ϲ����� �޴� Ŭ������ ��� DeltaTime �� �����Ͽ� QE::END�� �Ѿ. �׷��� ���� ��� ��¦�� ����Ʈ�� ��ư������ �ٲ�.
		END						// �޴� ����Ʈ�� ��� �������� InitializeMedalInfo �Լ��� ���� ���� ��Ʈ�ѵ��� ��� �ʱ�ȭ �� Visible Off ó��. QE::NONE ���� ���ư�.
	};
}

class UIHudQuestEffect : public iHudBase
{
public:
	UIHudQuestEffect(UIHudManager* p);
	virtual ~UIHudQuestEffect();

	virtual void			InitializeAtLoad( i3UIScene * pScene) override;
	virtual void			SetParameter(INT32 arg1, INT32 arg2 = -1) override;
	virtual void			event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void			Update( REAL32 rDeltaSeconds ) override;

	QE::EFFECT_STATE		GetEffectState(void) const { return m_eEffectState; }


protected:
	void					InitializeMedalInfo( void );

private:
	QE::EFFECT_STATE	m_eEffectState;
	UINT8				m_iBGShapeIdx;
	bool				m_bIsMaxLevel;
	VEC3D				m_vDefaultMarkPos;
	VEC2D				m_vDefaultFrameSize;
	REAL32				m_rElapsedTime;
	REAL32				m_rBGElapsedTime;

private:
	i3UIFrameWnd*		m_pMedalFrame;
	i3UIButtonImageSet* m_pMedalMark;
	i3UIButtonImageSet* m_pMedalMarkBG;
	i3UIStaticText*		m_pMedalName;
	i3UIStaticText*		m_pMedalLevel;
};
#include "pch.h"
#include "UIPhaseSubBase.h"
#include "UIMainFrame.h"
#include "UITopMenu.h"

#include "UIUtil.h"

#include "../CommunityContext.h"

//I3_ABSTRACT_CLASS_INSTANCE( UIPhaseSubBase, UIPhaseBase);
I3_CLASS_INSTANCE( UIPhaseSubBase);

UIPhaseSubBase::UIPhaseSubBase()
{
}

UIPhaseSubBase::~UIPhaseSubBase()
{
}

void UIPhaseSubBase::OnEscapeKey(void)
{
	GetMainFrame()->GetTopMenu()->ClickBackButton();

	//�������� Esc Ű �Է��� �����ϰ��� �ִ´�.
	i3InputKeyboard * pKeyboard = g_pFramework->getKeyboard();
	if( pKeyboard->GetStrokeState(I3I_KEY_ESC) == true) 
		pKeyboard->SetStrokeState(I3I_KEY_ESC, false);
}

// ���� �������� �Ϻ� �Լ��� ������Ʈ�ϵ��� �߰� �ǽð� ������Ʈ �Լ��� ���� �����Ѵ�..
void	UIPhaseSubBase::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate(rDeltaSeconds);
	
	UIPhaseBase* main_phase = GetMainFrame()->GetCurrentPhase();	
	
	if (main_phase != nullptr)
	{
		main_phase->OnUpdate_NoneRsc(rDeltaSeconds);
	}
}


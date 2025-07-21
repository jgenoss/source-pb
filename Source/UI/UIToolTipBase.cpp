#include "pch.h"
#include "UIToolTipBase.h"

//I3_ABSTRACT_CLASS_INSTANCE( UIToolTipBase, UIBase);
I3_CLASS_INSTANCE( UIToolTipBase);

UIToolTipBase::UIToolTipBase()
{
	m_slide.SetTargetTime(false, 0.f);
	m_slide.SetTargetTime(true, 0.f);
}

UIToolTipBase::~UIToolTipBase()
{
}

/*virtual*/ void UIToolTipBase::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBase::_InitializeAtLoad( pScene);
}

/*virtual*/ bool UIToolTipBase::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	INT32 ItemID = *(INT32*) pArg1;

	if( ItemID < 0 )    // <= 0)   ������ 0���� ù��° ���� �ε����� ����Ű�� �ϱ� ������...��ȣ�� ����..(11.09.09.����)
		return false;

	if( UIBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	SetInputDisable( true);

	// ��ġ�� ���콺 ��ġ��..
/*
	if( m_pFrameWnd != nullptr)
	{
		REAL32 fScreenWidthRate = i3UI::getUIResolutionWidth();
		REAL32 fScreenHeightRate = i3UI::getUIResolutionHeight();

		i3InputMouse* mouse = g_pFramework->getMouse();
		REAL32 x = mouse->GetX() / fScreenWidthRate;
		REAL32 y = mouse->GetY() / fScreenHeightRate;

		if( x + m_pFrameWnd->getWidth() > g_pViewer->GetViewWidth())
		{	// ȭ���� ����ϴ�.
			// ������ �������~
			x = g_pViewer->GetViewWidth() - m_pFrameWnd->getWidth() -1;
		}

		if( y + m_pFrameWnd->getHeight() > g_pViewer->GetViewHeight())
		{
			y = g_pViewer->GetViewHeight() - m_pFrameWnd->getHeight() -1;
		}

		m_pFrameWnd->setPos( x, y);
		m_slide.AddFrameWnd(m_pFrameWnd);
	}
*/

	return true;
}



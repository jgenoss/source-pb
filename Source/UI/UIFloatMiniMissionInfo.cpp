#include "pch.h"
#include "UIFloatMiniMissionInfo.h"

I3_CLASS_INSTANCE( UIFloatMiniMissionInfo, UIFloatingBase);

UIFloatMiniMissionInfo::UIFloatMiniMissionInfo()
{
}

UIFloatMiniMissionInfo::~UIFloatMiniMissionInfo()
{
}

/*virtual*/ void UIFloatMiniMissionInfo::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
}

/** \brief �ּ� ũ�� */
/*virtual*/ void UIFloatMiniMissionInfo::OnMinimize( void)
{
}

/** \brief �ִ� ũ�� */
/*virtual*/ void UIFloatMiniMissionInfo::OnMaximize( void)
{
}

/** \brief ���� ũ�� */
/*virtual*/ void UIFloatMiniMissionInfo::OnPreSize( void)
{
}

/*virtual*/ bool UIFloatMiniMissionInfo::OnSliding( REAL32 rDeltaSeconds)
{
	bool bRv = UIFloatingBase::OnSliding( rDeltaSeconds);

	if( m_pFrameWnd)
		m_pFrameWnd->setPos( m_originalX-m_SlideX, m_originalY);

	return bRv;
}

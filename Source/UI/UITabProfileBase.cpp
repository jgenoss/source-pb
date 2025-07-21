#include "pch.h"
#include "UITabProfileBase.h"
#include "UIMainFrame.h"

//I3_ABSTRACT_CLASS_INSTANCE( UITabProfileBase, UITabBase);
I3_CLASS_INSTANCE( UITabProfileBase);

UITabProfileBase::UITabProfileBase()
{
}

UITabProfileBase::~UITabProfileBase()
{
}

void	UITabProfileBase::SetNextProcess( TABPROFILE_NEXT_PROCESS pFn, void* ptr )
{
	m_NextParam.fn		= pFn;
	m_NextParam.ptr		= ptr;
}


void	UITabProfileBase::OnCreate( i3GameNode * pParent)
{
	UITabBase::OnCreate( pParent );
	((UIBase*)pParent)->GetSlide()->AddSyncSlide( GetSlide() );
}

void	UITabProfileBase::OnExitEnd()
{
	UITabBase::OnExitEnd();

	if( nullptr != m_NextParam.fn )
	{
		m_NextParam.fn( m_NextParam.ptr );
	}
}

void	UITabProfileBase::OnLoadAllScenes()
{
	UITabBase::OnLoadAllScenes();
	//GetSlide()->AddFrameWnds( GetScene(0), UISlide::R_TO_L );
}



/*
void	UITabProfileBase::OnOpen()
{
	// ���ڵ忡���� ��ũ�Ѹ���Ʈ�� 0������ ������...���⼭�� �켱 ����..
	SetEnable(true);
	//i3UI::getManager()->ShowSceneTop(GetScene(0));

}
*/
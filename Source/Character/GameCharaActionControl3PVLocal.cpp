#include "pch.h"
#include "GameCharaActionControl3PVLocal.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaActionControl3PVLocal, CGameCharaActionControl3PV);

CGameCharaActionControl3PVLocal::CGameCharaActionControl3PVLocal()
{
}

CGameCharaActionControl3PVLocal::~CGameCharaActionControl3PVLocal()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	Action

//bool CGameCharaActionControl3PVLocal::OnActionStandup( void)
//{
//	return _OnCmdActionStandUpForLocal();
//}
//
//bool CGameCharaActionControl3PVLocal::OnActionJumpStart( void)
//{
//	return OnActionJumpStartLocal();
//}


//bool CGameCharaActionControl3PVLocal::OnActionExtensionWeapon( bool bActivate)
//{	//	ActionParameter�� �ʿ��� �Լ��Դϴ�.
//	if( !CGameCharaActionControl3PV::OnActionExtensionWeapon( bActivate) )
//		return false;
//
//	m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);	//	�߻� �غ��߿� �߻����� ���ϵ���. �ִϸ��̼��� ������ �߻� ����
//
//#if defined( I3_DEBUG )//����� �뵵�� ��������� ����
//	if( m_pOwner->is1PV())
//	{
//		if( bActivate)
//			g_pFramework->getChatBox()->PutSystemChatting( "Extension On");
//		else
//			g_pFramework->getChatBox()->PutSystemChatting( "Extension Off");
//	}
//#endif
//
//	return true;
//}



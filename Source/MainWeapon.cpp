#include "pch.h"
#include "MainWeapon.h"




void	MainWeapon::OnPlayChangeAni( REAL32 rTimeScale)
{
	m_pWeaponBase->OnPlayChangeAniImpl(rTimeScale);					// ������ �����ϱ� ������ �켱 �̷��� ó���ϰ� ���߿� ������Ѵ�..
}



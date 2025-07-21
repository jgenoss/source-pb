#include "pch.h"
#include "MainWeapon_Flash.h"
#include "BattleSlotContext.h"
#include "GameCharaBoneContext.h"


MainWeapon_Flash::MainWeapon_Flash(WeaponBase* p) : MainWeapon_Grenade(p) {}
MainWeapon_Flash::~MainWeapon_Flash() {}

void MainWeapon_Flash::_OnCheckCollisionByExplosion() 
{
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
	{
		I3TRACE( "isObserverMe : call _OnCheckCollisionByExplosion....\n");
		return;
	}
#endif

	I3ASSERT( m_pWeaponBase->getOwner() != nullptr);

	VEC3D vLength;
	REAL32 fLength;
	UINT32 nSelfColGroup = CGRP_MASKGROUP_ATTACK;

	if( BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_RED)	nSelfColGroup	= CGRP_MASKGROUP_CHARA_RED;
	else													nSelfColGroup	= CGRP_MASKGROUP_CHARA_BLUE;

	REAL32 rExplosionRange = m_pWeaponBase->GetExplosionRange();

	//	����� �Ǵ� ĳ���Ϳ� �浹 �˻縦 �մϴ�.
	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	if( pPlayer != nullptr)
	{
		i3Vector::Sub( &vLength, m_pWeaponBase->GetPos(), i3Matrix::GetPos( pPlayer->getBoneContext()->getHeadMatrix() ) );
		fLength = i3Vector::Length( &vLength );
		if( fLength < rExplosionRange )
			_OnCheckCharaCollisionByExplosion( pPlayer, nSelfColGroup, rExplosionRange );
	}
}

bool MainWeapon_Flash::_OnCheckCharaCollisionByExplosion( CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rExplosionRange)
{
	REAL32			rWorldColLength = rExplosionRange;
	i3CollideeLine	ShotLine;
	I3_PHYSIX_HIT_RESULT * pResult;
	bool			bCollide = false;

	ShotLine.SetStart( m_pWeaponBase->GetPos());

	MATRIX * pMtx	= pChara->getBoneContext()->getHeadMatrix();		//	�Ӹ��� �˻�
	ShotLine.SetEnd( i3Matrix::GetPos( pMtx));

	//World�� ���ؼ� �ѹ� �׽�Ʈ
	pResult = g_pFramework->raycastClosest( &ShotLine, CGRP_MASKGROUP_ATTACK);
	if( pResult != nullptr)
	{
		if( pResult->m_T < rWorldColLength )
			rWorldColLength = pResult->m_T;
	}

	if( rWorldColLength > i3Vector::Length( ShotLine.GetDir()) )
	{
		m_pWeaponBase->_OnHitCharaByExplosion( pChara, CHARA_HIT_HEAD, ShotLine.GetDir() ,ShotLine.GetStart());
		bCollide	= true;
	}

	if( bCollide && pChara->is1PV())
		g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_THROW, 1, m_pWeaponBase->GetPos(), true);

	return bCollide;
}
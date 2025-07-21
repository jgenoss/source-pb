#include "pch.h"
#include "WeaponDinoCrash.h"
#include "GameCharaDino.h"
#include "GameCharaDinoTank.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaBoneContext.h"

I3_CLASS_INSTANCE( WeaponDinoCrash);

WeaponDinoCrash::WeaponDinoCrash()
{
}

WeaponDinoCrash::~WeaponDinoCrash()
{
}

bool WeaponDinoCrash::_IsDashCrash(void)
{
	// �ͽ��ټ��� ���� ��� false
	if(m_pExtWeapon == nullptr)
		return false;

	// �ͽ��ټ� �����, Crash�� �ƴѰ�� false
	if( i3::same_of<WeaponDinoCrash*>(m_pExtWeapon) == false )
		return false;

	// Dash Crash �ߵ� ���°� �ƴ� ��� false
	CGameCharaDinoTank * pDino = i3::kind_cast<CGameCharaDinoTank*>(m_pOwner);
	if(pDino == nullptr || pDino->isDashCrash() == false )
		return false;

	return true;
}

bool WeaponDinoCrash::_CheckRange(VEC3D & outDir, REAL32 & outLength, VEC3D * pCharaPos, REAL32 rRange)
{
	VEC3D vLength;
	VEC3D * pOwnerPos = nullptr;
	MATRIX * pMtx = nullptr;
	REAL32	rOffset = 0.0f;

	if( m_pOwner->IsTank() )
	{
		pMtx = m_pOwner->getBoneContext()->getBoneMatrix( RAPTOR_BONE_HEAD);
		pOwnerPos = i3Matrix::GetPos( pMtx );
		rOffset = 2.0f;
	}
	else
	{
		pOwnerPos = m_pOwner->GetPos();
		rOffset = 0.5f;
	}

	i3Vector::Sub( &vLength, pCharaPos, pOwnerPos );

	i3Vector::Normalize( &outDir, &vLength);
	REAL32 y = getY( &vLength);

	setY( &vLength, 0.0f);
	outLength = i3Vector::Length( &vLength);

	if( outLength > rRange)
		return false;

	if( i3Math::abs( y) > rRange * rOffset)
		return false;

	if( m_pOwner->IsTank() )
	{
		_FireEffect( pMtx, g_pEffectManager->FindEffect("Hit_Tank_Dash"), true);
		g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_KNUCKLE_HUMAN_A, 2, m_pOwner->GetPos(), m_pOwner->is1PV());	
	}

	return true;
}

void WeaponDinoCrash::ProcessDinoHit(CHARA_DAMAGEINFO * pDamageInfo)
{
	// Dash Crash�� ��쿡�� extension�� DamageInfo�� �缳���Ͽ� ó��
	if( _IsDashCrash() == true)
	{
		((WeaponDinoCrash*)m_pExtWeapon)->ProcessAreaHitDino();
		return;
	}

	// Crash Hit ���� ����
	{
		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara( i);
			if( pChara == nullptr )	continue;

			INT32 idx = pChara->getCharaNetIndex();
			if( idx == -1)	continue;

			if( pChara->IsDino())
				continue;

			if( pChara->isCharaStateMask( CHARA_STATE_DEATH) == true)
				continue;

			VEC3D vDir;
			REAL32 rDistance = 0.0f;

			if( _CheckRange(vDir, rDistance, pChara->GetPos(), pDamageInfo->_rExplosionRange) )
			{
				pDamageInfo->_nKillerIdx		= m_pOwner->getCharaNetIndex();
				pDamageInfo->_pVecDir			= &vDir;
				pDamageInfo->_pVecPos			= m_pOwner->GetPos();
				pDamageInfo->_rDistance			= rDistance;
				pDamageInfo->_nVictimIdx		= pChara->getCharaNetIndex();

				i3Vector::Copy( &pDamageInfo->_vStartPos, m_pOwner->GetPos() );
				i3Vector::Copy( &pDamageInfo->_vHitPos, pChara->GetPos());

				pChara->OnHit( pDamageInfo );
			}
		}
	}
}

/*virtual*/ bool WeaponDinoCrash::DinoAttackProcess(REAL32 rAnimTime, REAL32 rTimeScale)
{
	// rAnimTime == DeltaSeconds�� ���
	m_rAttakActiveTime += rAnimTime; 

	if(m_rAttakActiveTime < GetDinoAttackTime(0) )
		return false;

	if(	m_pOwner->isCharaStateMask( CHARA_STATE_DEATH ) )
		return false;

	m_rAttakActiveTime = 0.0f;

	// Dedication Mode������ �ڽ��� ����մϴ�.
	if( BattleServerContext::i()->IsUseDedicationMode())
	{
		if( m_pOwner->isLocal() == false)
			return false;
	}
	else if( BattleServerContext::i()->IsP2PHost() == false)
	{
		return false;
	}

	ProcessAreaHitDino();
	return true;
}
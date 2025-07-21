#include "pch.h"
#include "GameCharaDinoAcid.h"

#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "StageBattle.h"
#include "Crosshair_Dino_Raptor.h"
#include "WeaponDino.h"
#include "CrosshairMgr.h"
#include "GameStateMgr.h"

#include "ui/UIHudDefine.h"
#include "ui/UIHudUtil.h"

#include "./StageBattle/UserContext.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( CGameCharaDinoAcid);//, CGameCharaDino);

CGameCharaDinoAcid::CGameCharaDinoAcid()
{
	m_eNowDinoAttackType = DAT_ACID_BITE;
}

void CGameCharaDinoAcid::_CreateDino( void)
{
	CGameCharaDino::_CreateDino();

//	m_arDinoWeapon[WEAPON::SECONDARY_FIRE]->getWeaponInfo()->GetFireEffect(szFireEffect);		// TODO : ???? ������ �ʴ� ������ ���δ�..�ϴ� �ּ������� ����..(2014.07.04.����)

	m_idxExplosionIdleEffect1pv = g_pEffectManager->FindEffect( "Dino_Acid_Fire_B1");
	m_idxExplosionIdleEffect3pv = g_pEffectManager->FindEffect( "Dino_Acid_Fire_B2");

	m_idxExplosionFireEffect = g_pEffectManager->FindEffect( "HMK_Dino_Fire_Attack_B");
}

void CGameCharaDinoAcid::_PlayDownfallSound( void)
{
	g_pSndMng->StartDinoActionSound( 4, GTBDA_ON_DAMAGE_DOWNFALL, GetPos(), is1PV());
}

/*virtual*/ void CGameCharaDinoAcid::_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder)
{
	INT32 nRand = i3Math::Rand() % 2;

	switch(fireOrder)
	{
	case WEAPON::PRIMARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_ACID, GTBDA_ON_ATTACK_1_A + nRand, GetPos(), is1PV());
		}
		break;
	case WEAPON::SECONDARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_ACID, GTBDA_ON_ATTACK_2 + nRand, GetPos(), is1PV());
			g_pSndMng->StartDinoActionSound( GTBD_ON_ACID, GTBDA_ON_EFFECT, GetPos(), is1PV());
		}
		break;
	}
}

void CGameCharaDinoAcid::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCharaDino::OnUpdate( rDeltaSeconds);

	// �ֽõ� ���� ǥ��(���� ����)
	OnDinoExplosionSign();
	
	if( isCharaStateMask( CHARA_STATE_DEATH) == true)
	{
		// �������� ���� ��쿡�� ĳ���Ͱ� ȭ�鿡�� �Ⱥ��̰� �Ѵ�
		if(m_bExplosionOK && IsEnableScene())
			DisableScene();
		return;
	}

	if( isLocal() == false)
		return;

	//�������϶��� �̺�Ʈ ó����
	if( UserContext::i()->fRemainingBattleTime < 0)
		return;

	if( ! (gstatemgr::i()->isPlayingBattle()) )
		return;

	if(isInvincible())
		return;

	//���ݻ��°� �ƴϰ� �������� 2.0���� Ŭ��� ���� ó��
	UINT64 keyPress	= g_pFramework->getKeyPress();	
	UINT64 keyStroke = g_pFramework->getKeyStroke();	
	if((( keyPress & GAME_KEY_ACTION_SUBFUNC || keyStroke & GAME_KEY_ACTION_SUBFUNC) ) )
	{	
		m_rNoPressAccuTime = 0;
		m_rPressAccuTime += g_pViewer->getDeltaTime();
		m_rExplosionSpeedGauge += g_pViewer->getDeltaTime();
		m_bExplosionPenalty = true;

		if(m_rExplosionSpeedGauge > GetDinoSkillCoolTime(WEAPON::SECONDARY_FIRE) )
			m_rExplosionSpeedGauge = GetDinoSkillCoolTime(WEAPON::SECONDARY_FIRE) + 0.1f;
	}
	else 
	{
		if( m_rExplosionSpeedGauge > GetDinoSkillCoolTime(WEAPON::SECONDARY_FIRE) )
		{
			m_arDinoWeapon[WEAPON::SECONDARY_FIRE]->ProcessAreaHitDino();
			m_isAvailDash = true;
			m_bExplosionPenalty = false;
			m_rExplosionSpeedGauge = 0.0f;
			m_rAccuTimeForPenalty = GetDinoSkillCoolTime(WEAPON::SECONDARY_FIRE) + 0.1f;
		}
		else
		{
			m_bExplosionPenalty = false;
			m_isAvailCrash =false;
			m_isAvailDash = false;
			m_rPressAccuTime = 0;
		}
		
		if(m_rExplosionSpeedGauge < 0)
		{
			m_isAvailDash = false;
			m_bExplosionPenalty = false;
			m_rExplosionSpeedGauge = -0.000000001f;
			m_rAccuTimeForPenalty = 0;
		}

		m_rExplosionSpeedGauge -= g_pViewer->getDeltaTime() * 2.5f;
		m_rNoPressAccuTime -= g_pViewer->getDeltaTime();
	}

	m_rAccuTimeForPenalty -= g_pViewer->getDeltaTime();
	if(m_rAccuTimeForPenalty < 0)
		m_rAccuTimeForPenalty = 0.f;
	if(m_rExplosionSpeedGauge < 0)
		m_rExplosionSpeedGauge = 0;
	if(m_rNoPressAccuTime < 0)
		m_rNoPressAccuTime = 0;

	m_rOldExplosionSpeedGauge = m_rExplosionSpeedGauge;


	UpdateFlag();

	if (isLocal())
		UpdateExplosionGuage();
}

ID_UPPER_DINO_AI CGameCharaDinoAcid::DinoAttack(WEAPON::FIRE_ORDER fireOrder)
{
	ID_UPPER_DINO_AI dinoAI = ID_UPPER_DINO_UNKNOWN;
	((WeaponDino*)GetDinoWeapon())->SetFireOrder(fireOrder);
	ChangeDinoWeapon(fireOrder);

	if( isSkipAttack(fireOrder) )
		return dinoAI;

	dinoAI = _FindDinoUpperAttackAI(fireOrder);

	getHUDContext()->_ApplyCrosshair();
	addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );

	if(dinoAI != ID_UPPER_DINO_UNKNOWN)
	{
		_PlayDinoAnimUpper_Fire(fireOrder, dinoAI);
	}

	return dinoAI;
}

void CGameCharaDinoAcid::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm)
{
	if(m_eNowDinoAttackType == DAT_ACID_BITE)
	{
		m_arDinoWeapon[WEAPON::PRIMARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime(), getAnimContext()->getUpperTimeScale() );
		if( isLocal())
			m_arDinoWeapon[WEAPON::PRIMARY_FIRE]->_FireEffect( getFXMatrix(), m_idxButtFireEffect, true);
	}
	else if((m_eNowDinoAttackType == DAT_ACID_EXPLOSION) )
	{
		if(getCharaInfo()->GetHP() > 0 )
			m_rExplosionHP = getCharaInfo()->GetHP();

		//�������� ä��� ������� �̼Ӱ���ȿ�� ����
		if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK) || getActionContext()->isLower( CHARA_LOWER_JUMP))
		{
			getMoveContext()->EnableSpeedReduction( GetDinoSkillMovementRate(WEAPON::SECONDARY_FIRE), GetDinoSkillDuration(WEAPON::SECONDARY_FIRE) );
		}

		if(getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) )
		{
			CGameCharaBoneContext * pBoneCtx = getBoneContext();
			MATRIX * pMtx = pBoneCtx->getBoneMatrix( RAPTOR_BONE_ROOT);
			if(is1PV())
				((WeaponDino*)GetDinoWeapon())->_FireEffect( pMtx, m_idxExplosionIdleEffect1pv, true);
			else
				((WeaponDino*)GetDinoWeapon())->_FireEffect( pMtx, m_idxExplosionIdleEffect3pv, true);

			m_bEnableSign = true;
		}
	}
}

WEAPON::FIRE_ORDER CGameCharaDinoAcid::GetFireOrder()
{
	if(m_eNowDinoAttackType == DAT_ACID_EXPLOSION)
	{
		return WEAPON::SECONDARY_FIRE;
	}
	else if(m_eNowDinoAttackType == DAT_ACID_BITE)
	{
		return WEAPON::PRIMARY_FIRE;
	}

	I3ASSERT_0;
	return WEAPON::PRIMARY_FIRE;
}

void CGameCharaDinoAcid::_PlayBreathSnd(void)
{
	INT32 nRand = i3Math::Rand()%3;

	g_pSndMng->StartDinoActionSound( 4, GTBDA_ON_BREATH_1 + nRand, GetPos(), is1PV());
}

void CGameCharaDinoAcid::ResetChara()
{
	CGameCharaDino::ResetChara();

	Crosshair * pCurCH = CrosshairMgr::i()->getCrosshair();
	if(pCurCH != nullptr) {
		pCurCH->ResetCrosshair();
	}

	hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_ACID"),
		DinoState::BITE, DinoState::BLAST );

	m_rExplosionHP = 0;
	m_bExplosionPenalty = false;
	m_rExplosionSpeedGauge = 0.0f;
	m_rAccuTimeForPenalty = 2.1f;
	m_bExplosionOK = false;

	m_rExplosionSign = 0.0f;
	m_bIncreaseSign = false;
	m_bEnableSign = false;
	
	if(!IsEnableScene())
		EnableScene();
}

void CGameCharaDinoAcid::UpdateFlag()
{

	if(m_rAccuTimeForPenalty > 2.0f)
	{
		m_bExplosionPenalty = false;
	}
}

void CGameCharaDinoAcid::UpdateExplosionGuage()
{
	const REAL32 max = GetDinoSkillCoolTime(WEAPON::SECONDARY_FIRE);
	hu_dino::set_dino_raio( m_rExplosionSpeedGauge / max );
}

void CGameCharaDinoAcid::Effect()
{
	//UI����� ����Ʈ ȣ��
	MATRIX pMtx;
	
	i3Matrix::SetPos(&pMtx, GetPos());	

	((WeaponDino*)GetDinoWeapon())->_FireEffect( &pMtx, m_idxExplosionFireEffect, true);
	m_bExplosionOK = true;	
}

void CGameCharaDinoAcid::OnDinoExplosionSign()
{
	COLORREAL mtl;
	mtl.r = 0.0f;
	mtl.g = 0.0f;
	mtl.b = 0.0f;
	mtl.a = 0.0f;

	if(m_bEnableSign)
	{
		if(m_bIncreaseSign)
		{
			m_rExplosionSign += g_pViewer->getDeltaTime() * 2.0f;
			if(m_rExplosionSign > 1.0f)
			{
				m_rExplosionSign = 1.0f;
				m_bIncreaseSign = false;
			}
		}
		else
		{
			m_rExplosionSign -= g_pViewer->getDeltaTime() * 2.0f;
			if(m_rExplosionSign < 0.0f)
			{
				m_rExplosionSign = 0.0f;
				m_bIncreaseSign = true;
			}
		}	
	}
	else
	{
		// �ұ�Ģ�� ������ ������ ����, ���� ���� �ٿ��־� �ڿ������� �������.
		m_rExplosionSign -= g_pViewer->getDeltaTime() * 2.0f;
		if(m_rExplosionSign < 0.0f)
			m_rExplosionSign = 0.0f;

		m_bIncreaseSign = false;
	}

	mtl.r = m_rExplosionSign;
	getMaterialContext()->SetMaterialColor(&mtl, &mtl);


	// ���� ��ҽÿ��� true�� Ȱ��ȭ�Ǿ��־�, Material ��ü �� EnableSign�� �� ������ �ʱ�ȭ�Ѵ�.
	m_bEnableSign = false;
}

/*virtual*/ EDinoAttackType CGameCharaDinoAcid::GetDinoAttackType(INT32 idx)
{
	if( idx == WEAPON::PRIMARY_FIRE )
	{
		I3TRACE("ACID : BITE \n");
		return DAT_ACID_BITE;
	}
	else if ( idx == WEAPON::SECONDARY_FIRE )
	{
		I3TRACE("ACID : EXPLOSION \n");
		return DAT_ACID_EXPLOSION;
	}

	return m_eNowDinoAttackType;
}

/*virtual*/ bool CGameCharaDinoAcid::isSkipAttack(INT32 idx)
{
	if( idx == WEAPON::SECONDARY_FIRE )
		return isInvincible();

	return false;
}
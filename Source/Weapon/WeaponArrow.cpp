#include "pch.h"
#include "WeaponArrow.h"
#include "GlobalVariables.h"
//#include "GameCharaArtificialIntelligenceContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaCollisionContext.h"
#include "BattleServerContext.h"


I3_CLASS_INSTANCE( WeaponArrow);//, WeaponBase);

WeaponArrow::WeaponArrow() : m_bCheckThrow(FALSE)
							, m_rElapsedThrowTime(0.0f)
							, m_rTracerUpdateTime(0.0f)
							, m_rSpeed(0.0f)
{
	m_pInfo = NULL;
}

WeaponArrow::~WeaponArrow()
{
}

void WeaponArrow::_OnInitVairable( void)
{
	WeaponBase::_OnInitVairable();

	m_bCheckThrow = FALSE;
	i3Vector::Zero( &m_vTracerOldPos);

	i3Vector::Zero( &m_vOldPos);		//���� ��ġ
	i3Vector::Zero( &m_vStartPos);		//���� ��ġ
	i3Vector::Zero( &m_vDirection);		//����
	m_rSpeed = 0.f;						//�ӵ�
	m_pInfo = NULL;
}

void WeaponArrow::Create( CGameCharaBase * pOwner, INT32 Itemid)
{
	//	���� ����
	_OnInitVairable();

	char szResPath[MAX_PATH];

	i3::safe_string_copy( szResPath, "Weapon/wooden_arrow.I3CHR", MAX_PATH );

	if( g_pWeaponInfoDataBase != NULL)
	{
		m_pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( Itemid);

		m_pAbsoluteWeaponInfo = m_pWeaponInfo;		//	ó�� �ʱ�ȭ���� ���õǴ� ���� �����̴�.

		m_CurrentExtensionType = m_pWeaponInfo->getExtensionType(0); //	ù��° �ͽ��ټ����� �ʱ�ȭ ����. �߰��� �ٲ�� �ִ� ��
	}

	i3GameResChara * pRes = (i3GameResChara *) g_pFramework->FindResourceByPath( szResPath);
	I3ASSERT( pRes != NULL);
	
	i3Chara::Create( g_pFramework, pRes);

	Instantiate();

	//	��� ������ ����ó���� ���� AttrSet�� ���δ�.
	{
		m_pAttrSet = i3AttrSet::new_object_ref();
		m_pAttrSet->SetPriority( 0);	

		while( getAttachNode()->GetChildCount() > 0)
		{
			i3Node * pChild = getAttachNode()->GetChild( 0);
			pChild->AddRef();

			pChild->RemoveFromParent();
			m_pAttrSet->AddChild( pChild);

			pChild->Release();
		}	

		getAttachNode()->AddChild( m_pAttrSet);		

		m_pBlendEnable = i3BlendEnableAttr::new_object_ref();
		m_pBlendEnable->Set( FALSE);
		m_pAttrSet->AddAttr( m_pBlendEnable);

		i3BlendModeAttr * pBlendMode = i3BlendModeAttr::new_object_ref();
		pBlendMode->SetSource( I3G_BLEND_SRCALPHA);
		pBlendMode->SetDestination( I3G_BLEND_INVSRCALPHA);
		m_pAttrSet->AddAttr( pBlendMode);

		m_pMaterialAttr = i3MaterialAttr::new_object_ref();
		m_pAttrSet->AddAttr( m_pMaterialAttr);

		COLORREAL col;

		i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
		m_pMaterialAttr->SetDiffuse( &col);
		m_pMaterialAttr->SetSpecular( &col);
		m_pMaterialAttr->SetShininess( 128.0f);
		
	}
    
	//	Set Update Mode
	m_pBody = getSceneObject()->GetBody();
	I3ASSERT( m_pBody != NULL);

	//	���Ƿ� �����ϱ� ���� �ϱ� ���� ������ LOD Slope���� �����մϴ�. (������ CharaTool���� ������ Slope���� ������� �ʽ��ϴ�. komet)
	m_pBody->setSlope( WEAPON_LOD_SLOPE);
	
	if( BattleServerContext::i()->IsP2PActivate() )
	{
		m_pBody->removeStyle( I3BS_LOD_AUTO);
		m_pBody->AddFlag( I3_NODEFLAG_MUSTBEPROCESS , true);
	}
}

void WeaponArrow::OnBindResource( void)
{
	i3Chara::OnBindResource();
}

void WeaponArrow::Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, BOOL bOrigin)
{

	m_rElapsedThrowTime		= 0.f;

	#if !defined( NO_GRENADE_TRACE_EFFECT)
	m_rTracerUpdateTime		= 0.f;
	#endif

	m_rSpeed				= rSpeed;

	i3Vector::Copy( &m_vStartPos, vStart);
	i3Vector::Copy( &m_vDirection, vDir);

	i3Vector::Copy( &m_vOldPos, vStart);
	i3Vector::Copy( &m_vTracerOldPos, vStart);
}

void WeaponArrow::OnUpdate( REAL32 rDeltaSeconds)
{
	if( isWeaponStateMask( WEAPON_ARROW_STATE_FLY))
	{
		UpdateFlying( rDeltaSeconds );

		i3Vector::Copy( &m_vOldPos, GetPos());
	}

	_TransformWorldSpace( rDeltaSeconds);

	WeaponBase::OnUpdate( rDeltaSeconds);
}


void WeaponArrow::UpdateFlying( REAL32 rDeltaSeconds)
{
	if( !isWeaponStateMask( WEAPON_ARROW_STATE_FLY))
	{	
		return;
	}

	//
	if( rDeltaSeconds > 0.5f)rDeltaSeconds = 0.016f;

	m_rElapsedThrowTime += rDeltaSeconds;

	CheckWorldCollision( rDeltaSeconds);

#if !defined( NO_GRENADE_TRACE_EFFECT)
	//	����ź ����(����)�� �����.
	if( (m_rElapsedThrowTime > 0.1f && m_rTracerUpdateTime <= 0.f) &&
		!i3Vector::IsSame( GetPos(), &m_vTracerOldPos) &&
		isWeaponStateMask( WEAPON_ARROW_STATE_FLY))
	{	
		REAL32 len;
		VEC3D vDir;

		CWeaponGlobal * pGlobalInfo = g_pWeaponInfoDataBase->getGlobal();

		i3Vector::Sub( &vDir, GetPos(), &m_vTracerOldPos); 
		len = i3Vector::Length( &vDir); 
		i3Vector::Normalize( &vDir, &vDir);

		//	���� ���� �ð� (�������� tracer�� ���� �����.)
		m_rTracerUpdateTime = pGlobalInfo->m_rTracerArrowPieceCreateTime; 

		//	���� �߻�
		//if( m_rElapsedExplosionTime < m_pWeaponInfo->m_rExplosion_Time - 0.1f)	//	�����ϱ� ������ ������ ���ʿ��ϹǷ� ���´�.
		if( isWeaponStateMask( WEAPON_ARROW_STATE_FLY))
		{
			TracerBulletInfo tracerInfo;
			tracerInfo._Type = TRACER_TYPE_ARROW;
			tracerInfo._LifeTime = pGlobalInfo->m_rTracerArrowPieceLifeTime;
			i3Vector::Copy( &tracerInfo._vBeginPos, &m_vTracerOldPos);
			i3Vector::Copy( &tracerInfo._vDestPos, GetPos());
			tracerInfo._Length = len;
			tracerInfo._Thickness = pGlobalInfo->m_rTracerArrowThickness;
			tracerInfo._EndSpeed = 0;
			i3Color::Copy( &tracerInfo._color, &pGlobalInfo->m_TracerArrowColor);

			g_pTracerBulletManager->Fire( &tracerInfo);
		}

		i3Vector::Copy( &m_vTracerOldPos, GetPos());

		//I3TRACE( "Grenade Tracer : Update Time %.4f, Length : %.4f, Speed : %.4f\n", m_rTracerUpdateTime, len, speed);
	}
	
	m_rTracerUpdateTime -= rDeltaSeconds;
#endif	
}

void WeaponArrow::CheckWorldCollision( REAL32 rDeltaSeconds)
{
	VEC3D	vTemp;
	REAL32	rLength = 100000.f;
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult;

	VEC3D	vNew;
	i3Vector::Scale( &vNew, &m_vDirection, m_rSpeed * m_rElapsedThrowTime);
	i3Vector::Add( GetPos(), &m_vStartPos, &vNew);
	addY( GetPos(), -9.81f * m_pWeaponInfo->GetWeight() * m_rElapsedThrowTime * m_rElapsedThrowTime);

	ColLine.SetStart( &m_vOldPos);
	i3Vector::Sub( &vTemp,  GetPos(), &m_vOldPos);
	rLength = i3Vector::Normalize( &vTemp, &vTemp);

	VEC3D vPos;
	i3Vector::Copy( &vPos, GetPos());
	REAL32 rRotY = i3Math::atan2( getX( &vTemp), getZ( &vTemp));
	REAL32 rRotX = I3_PI2 + i3Math::acos( getY( &vTemp) );
	
	i3Math::NormalizeAngle( rRotY);
	i3Math::NormalizeAngle( rRotX);

	i3Matrix::Identity( GetMatrix());
	i3Matrix::PostRotateY( GetMatrix(), rRotY);
	i3Matrix::PreRotateX( GetMatrix(), rRotX);
	SetPos( &vPos);

	ColLine.SetDir( &vTemp);

	// Rough Hit Collision�� Chara Hit����� ������ World�� Object�� ���Ͽ� �׽�Ʈ�� �մϴ�.
	pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ATTACK );
	
	if( pResult != NULL)
	{
		g_pFramework->reserveRaycastResult( 1);

		if( rLength < pResult->m_T )
			pResult = NULL;
		else
			rLength = pResult->m_T;
	}
	
	{
		UINT32 nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
		if( m_pOwner->getCharaTeam() & NPC_TEAM_BLUE)
		{
			nColGroup = CGRP_MASKGROUP_CHARA_RED;
		}

		I3_PHYSIX_HIT_RESULT * pCharaResult = g_pFramework->raycastClosest( &ColLine, nColGroup );
		if( pCharaResult != NULL)
		{
			g_pFramework->reserveRaycastResult( 2);

			i3EventReceiver * pRecv = pCharaResult->m_pDestShapeSet->getEventReceiver();
			if( i3::kind_of<CGameCharaBase*>(pRecv)) //->IsTypeOf( CGameCharaBase::static_meta()))
			{
				CGameCharaBase * pTarget = (CGameCharaBase *) pRecv;
				if( !pTarget->isNPC() && !pTarget->isCharaStateMask( CHARA_STATE_DEATH) &&
					(rLength > pCharaResult->m_T) )
				{
					pResult = pCharaResult;
					rLength = pCharaResult->m_T;
				}
			}
		}
	}
	

	if( pResult != NULL)
	{
		SetPos( &pResult->m_Intersect);
		
		if( BattleServerContext::i()->IsSubP2PHost() )
			OnHit( pResult, &ColLine);
		else
		{
			if( !i3::same_of<CGameCharaBase*>(pResult->m_pDestShapeSet->getEventReceiver())) //->IsExactTypeOf( CGameCharaBase::static_meta() ))
				OnHit( pResult, &ColLine);
			else
				g_pWeaponManager->PushArrow( (GIRAN_ARROW*) m_pInfo);
		}
		
		removeWeaponStateMask( WEAPON_ARROW_STATE_FLY);
		addWeaponStateMask( WEAPON_ARROW_STATE_HIT);

		//I3TRACE( "Hit Arrow.\n");
	}

	g_pFramework->resetRaycastResult();
}

void WeaponArrow::OnHit( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine)
{
	I3ASSERT( pResult != NULL );
	BOOL bRet = TRUE;

	i3PhysixShapeSet *	pShapeSet = pResult->m_pDestShapeSet;
	
	if( pShapeSet == NULL )
	{
		I3TRACE( "WeaponBase:_OnHit() pDestShapeSet NULL.\n" );
		return;
	}

	I3ASSERT( i3::same_of<i3PhysixShapeSet*>(pShapeSet)); //->IsExactTypeOf( i3PhysixShapeSet::static_meta()));

	switch( pShapeSet->getShapeGroup() )
	{
	case CGRP_WORLD_ATTACK :
		{
			// World�� �¾Ҵ�.
//			I3_PHYSIX_TERRAIN * pTerrain = pResult->m_pTerrain;
		}
		break;
	case CGRP_OBJECT_ALL :
	case CGRP_OBJECT_HIT :
	case CGRP_OBJECT_WHOLE :
	case CGRP_OBJECT_WHOLE_DYNAMIC :
		{	
//			I3_PHYSIX_TERRAIN * pTerrain = pResult->m_pTerrain;
		}
		break;
	case CGRP_WORLD :
	case CGRP_TEAM_BLUE_ROUGH :
	case CGRP_TEAM_RED_ROUGH :
		break;

	default :
		{
			VEC3D vPos;
			
			CGameCharaBase * pHitChara = NULL;
			CGameCharaEquip * pEquip = NULL;

			I3ASSERT( pShapeSet->getEventReceiver() != NULL);

			if( i3::kind_of<CGameCharaEquip*>(pShapeSet->getEventReceiver())) //->IsTypeOf( CGameCharaEquip::static_meta() ))
			{
				pEquip = (CGameCharaEquip*)pShapeSet->getEventReceiver();
				I3ASSERT( pEquip->GetOwner() != NULL);

				if( i3::kind_of<CGameCharaBase*>(pEquip->GetOwner())) //->IsTypeOf( CGameCharaBase::static_meta() ) )
					pHitChara = (CGameCharaBase*) pEquip->GetOwner();
				else
				{
					// ��� ��ü�Դϴ�. 
				}
				bRet = FALSE;
			}
			else if( i3::kind_of<CGameCharaBase*>(pShapeSet->getEventReceiver())) //->IsTypeOf( CGameCharaBase::static_meta() ) )
				pHitChara = (CGameCharaBase*) pShapeSet->getEventReceiver();
			else
			{
				I3FATAL( "�� �� ���� Hit." );
			}
			
			if( pHitChara == NULL)
				break;

			if( pHitChara->isNPC())
			{
				I3FATAL( "�̰��� �̻��� ���Դϴ�.");
				break;
			}

			VEC3D vecDir = { 0.f, 0.f, 0.f};
			CHARA_DAMAGEINFO DamageInfo;
			DamageInfo.Reset();

			CGameCharaBase * pKiller = getOwner();
			BOOL bOwner = FALSE;
			INT32 killIdx = 0;
			if( pKiller != NULL)
			{
				bOwner = pKiller->isNPC();
				killIdx = pKiller->getCharaNetIndex();
				DamageInfo._IsNPC = TRUE;
			}
			DamageInfo._FireOrder		= getFireOrder();
			DamageInfo._rDamage			= getAP();
			DamageInfo._IsOwner			= bOwner;
			DamageInfo._nKillerIdx		= killIdx;
			DamageInfo._pHitResult		= pResult;
			m_pWeaponInfo->SetDamageInfo(&DamageInfo);

			if( getOwner() != NULL)
			{
				VEC3D diff;

				i3Vector::Sub( &diff, pHitChara->GetPos(), getOwner()->GetPos());

				DamageInfo._rDistance = i3Vector::Length( &diff);
			}

			CHARA_HIT_PART part = pHitChara->getCollisionContext()->GetHitPart( pResult->m_pDestShapeSet);

			DamageInfo._DeathType = CHARA_DEATH_A;	//	CHARA_DEATH_A �� �ӽ��̹Ƿ� �ٸ� Death ������ �ʿ��ϸ� ������ �ʿ��մϴ�.			
				
			if( pKiller != NULL)
			{
				i3Vector::Set( &vPos, getX( pKiller->GetPos() ), getY( pKiller->GetPos() ) + pKiller->getCameraContext()->getIKHeightForCam(), getZ( pKiller->GetPos() ) );
			}
			DamageInfo._pVecPos		=  &vPos;

			CalcHitDir( pResult, DamageInfo._pVecPos, &vecDir);
			DamageInfo._pVecDir		= &vecDir;
			DamageInfo._HitType		= GHT_CHARA;

			if( part == CHARA_HIT_HEAD)
				part = CHARA_HIT_BODY1;
			DamageInfo._HitPart = part;

			//I3ASSERT( DamageInfo.isValid() );

			// ĳ������ HP �� ����Ʈ ó��
			pHitChara->OnHit( &DamageInfo);

			g_pWeaponManager->PushArrow( (GIRAN_ARROW*)m_pInfo);

			//
			if( BattleServerContext::i()->IsP2PHost())
			{
				// broadcast pack
				I3ASSERT( m_pOwner->getCurHP() < 255);
				GAMEEVENT::Write_NPC( m_pOwner->getCharaNetIndex(),
					(UINT8) ( NPC_AI_STATE_HITARROW | (pHitChara->getCharaNetIndex() << 4)),
					(UINT8) m_pOwner->getCurHP(), NULL, (INT16) part);
				
				if( pHitChara->is3PV() )
					g_pWeaponManager->AttachArrowToChara( pHitChara, part, &vecDir);
			}
		}
		break;
	}
}





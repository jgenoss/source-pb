#include "pch.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "TracerBulletManager.h"
#include "Crosshair.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "WeaponGlobal.h"
#include "GameCharaCameraContext.h"
#include "GameCharaCollisionContext.h"
#include "GameCharaMoveContext.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaInputContext.h"
#include "GameCharaEquipContext.h"
#include "GameObject.h"
#include "WeaponM197.h"
#include "WeaponDino.h"
#include "GameCharaActionContext.h"
#include "../Character/InputCtrlAI_ForHuman.h"
#include "TutorialMode.h"
#include "WeaponItemID.h"
#include "CrosshairMgr.h"
#include "GameMode.h"
#include "ObjExplosion.h"
#include "Weaponinfo/MeleeWeaponInfo.h"

#ifdef DOMI_DINO
#include "../Character/Fullbody/GameCharaDomiSting.h"
#include "../Mode/Domination/Domination_Defines.h"
#include "../Mode/Domination/DomiUserSkillMgr.h"
#include "../Mode/Domination/DomiSkillProperties.h"
#endif

#include "i3Base/string/ext/snprintf.h"

#include "../StageBattle/UserContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "UserInfoContext.h"
#include "GameMissionManager.h"
#include "WeaponFunction.h"
#include "MainWeapon_C5.h"
#include "MainWeapon_Claymore.h"
#include "MainWeapon_M14.h"
#include "Designation/DesigApplyInfo.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#if 0
#define CASHITEM_TRACE	I3TRACE
#else
#define CASHITEM_TRACE	__noop
#endif

namespace
{
	enum PENALTY_TYPE	{ PENALTY_HORZRECOIL = 0, PENALTY_VERTRECOIL, PENALTY_DAMAGE, PENALTY_DEVIATION };

	// Calculate Endurance Penalty
	void CalEndurancePenalty( PENALTY_TYPE Type, UINT8 ui8Endurance, REAL32& rVal )
	{
		EndurancePenalty*	pPenalty	= g_pConfigEx->GetEndurancePenalty( ui8Endurance );

		if( pPenalty == nullptr ) return;

		switch( Type )
		{
		case	PENALTY_HORZRECOIL :	rVal	*= pPenalty->HorzRecoil;	break;
		case	PENALTY_VERTRECOIL :	rVal	*= pPenalty->VertRecoil;	break;
		case	PENALTY_DAMAGE :		rVal	*= pPenalty->Damage;		break;
		case	PENALTY_DEVIATION :		rVal	*= pPenalty->Deviation;		break;
		}

		return;
	}
}

I3_CLASS_INSTANCE( WeaponBase); //, i3Chara);


WeaponBase::WeaponBase( void)
{
	_OnInitVairable();		
}	

WeaponBase::~WeaponBase( void)
{
	_StopFireSound();
	_StopSwapSound();
	_StopIdleSound();

	for( INT32 i = 0; i < WEAPON::FIRE_SOUND_COUNT; i++)
	{
//		I3_SAFE_RELEASE( m_pFireSound[ i]);
		I3_SAFE_RELEASE( m_pFireSoundPlayInfo[ i]);
	}

	I3_SAFE_RELEASE( m_pSwapSoundPlayInfo);
	I3_SAFE_RELEASE( m_pIdleSoundPlayInfo);

	if( m_pLight != nullptr)
	{
		g_pFramework->removeLight( m_pLight);

		I3ASSERT( m_pLight->GetRefCount() == 1);
		I3_SAFE_RELEASE( m_pLight);
	}

	I3_SAFE_RELEASE( m_pContactHitBox);
	I3_SAFE_DELETE(m_pDamage);

	#if defined( AI_MOVE_TO_WEAPON)
	I3_SAFE_RELEASE( m_pCharaAINode);
	#endif

	i3::cu::for_each_delete(m_listEmptyBulletSound);
	i3::cu::for_each_delete(m_listTerrainSound);

	I3_SAFE_RELEASE( m_pFXLastTrans );

	I3_SAFE_DELETE(m_StartPos);
	I3_SAFE_DELETE(m_TargetPos);

}

void WeaponBase::_OnInitVairable( void)
{

	m_pWeaponInfo		= nullptr;
	m_pAbsoluteWeaponInfo = nullptr;
	m_pBody				= nullptr;

	#if defined( AI_MOVE_TO_WEAPON)
	m_pCharaAINode		= nullptr;
	#endif

	m_pExtWeaponInfo	= nullptr;

	m_pAICtx			= nullptr;

	i3mem::Fill( m_AIIndex, -1, sizeof( m_AIIndex));
	
	m_idxFXBone					= -1;
	m_idxFXIdleBone				= -1;

	i3mem::Fill(m_idxFireEffect, -1, sizeof(m_idxFireEffect));

	m_idxFXBullet				= -1;
	m_idxBulletEffect			= -1;
	m_idxFireEffect_1PV_Zoom	= -1;
	m_idxFXLaserSight			= -1;

	m_rLastIdleEffectTime = 0.0f;
	
	m_pAttrSet			= nullptr;
	m_pMaterialAttr		= nullptr;
	m_pBlendEnable		= nullptr;	
	m_fAlphaVal			= 0.0f;

	m_timeLight			= 0.0f;

#if defined( TEST_COLLISION_LINE )
	m_pVA			= nullptr;
#endif

	for(INT32 i = 0; i < WEAPON::FIRE_SOUND_COUNT; i++)
	{
		m_pFireSoundPlayInfo[ i]	= nullptr;		
	}
	m_pFireSoundPlayState	= nullptr;
	m_pSwapSoundPlayInfo	= nullptr;
	m_pSwapSoundPlayState	= nullptr;
	m_pIdleSoundPlayInfo	= nullptr;
	m_pIdleSoundPlayState	= nullptr;
	
	m_Idx				= -1;						// ���µǸ� �ȵ˴ϴ�.
	m_NetIdx			= -1;						// ���µǸ� �ȵ˴ϴ�.
	setWeaponState( WEAPON::STATE_READY);
	m_StateMaskWeapon	= 0;
	m_FireOrder			= WEAPON::PRIMARY_FIRE;
	m_AttachGrip		= WEAPON_ATTACH_NONE;

	m_WeaponFlag		= 0;
	
	m_DualMagazineMode			= WEAPON_DUAL_MAGAZINE_NONE;
	setLoadedBulletDualCount(0);
	
	setLoadedBulletCount(0);
	m_nMaxBullet		= 0;
    setAutoFireCount(0);
	setAutoPointShotCount(0);
	setRemainAutoFireCount(0);

	m_timeAutoFire		= 0.0f;

	m_bSetStart			= false;
	i3Vector::Zero( m_StartPos);
	i3Vector::Zero( m_TargetPos);

	addWeaponStateMask( WSM_EMPTY_CARTRIDGE | WSM_LOADED_BULLET);

	ResetSound();
	
	SetHitGroup( false);
	m_rPenetrateLength = 0.0f;
	
	m_pOwner		= nullptr;
	m_pParentGame	= nullptr;

	m_pGunDummy			= nullptr;
	m_pGunDummyLeftHand = nullptr;
	m_pGunDummyBack = nullptr;
	i3Matrix::Identity( &m_mtxGunDummy);
	i3Matrix::Identity( &m_mtxGunDummyLeftHand);
	
//	m_nContactReportCount = 0;
//	m_nContactReportReadPos = 0;
	m_vecContactReport.clear();

	setDamageRate( 1.f);

	I3_SAFE_RELEASE( m_pContactHitBox);
	
	if( m_pLight != nullptr)
	{
		g_pFramework->removeLight( m_pLight);

		I3ASSERT( m_pLight->GetRefCount() == 1);
		I3_SAFE_RELEASE( m_pLight);
	}
	
	m_timeAfterZoom = 10.0f;

	i3mem::Fill(m_nCommandBuf, -1, sizeof(m_nCommandBuf));

	m_nPushCommandCount = 0;
	m_nPopCommandCount = 0;

	m_pPairWeapon	= nullptr;						// ���µǸ� �ȵ˴ϴ�.

	m_bTestRoughCollision	= false;
	m_ui8Endurance			= 100;

	addWeaponFlag( WEAPON_FLAG_ENABLE_FIRE_SOUND);		//	default
	m_bOnceJump = false;
	m_bOnceSit = false;

	m_rFireTime = 0.f;
	m_rReloadTime = 0.f;

	m_tDropInfo.Reset();

//
	if (m_mainWeapon)
		m_mainWeapon->OnInitVariable();
//
}

void WeaponBase::_ResetGrenadeInfo(void)
{
	if(m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx())
	{	
		INT32 GrenadeIdx =  getNetworkIdx() % MAX_THROWWEAPON_SLOP;
		if (GrenadeIdx > -1 && GrenadeIdx <= MAX_THROWWEAPON_SLOP)
		{
			GrenadeContext::i()->m_GrenadeCheck[GrenadeIdx].Reset();
		}
	}
}

void WeaponBase::Reset( void)
{

	m_StateMaskWeapon	= 0;
	m_FireOrder		= WEAPON::PRIMARY_FIRE;

	addWeaponStateMask( WSM_EMPTY_CARTRIDGE | WSM_LOADED_BULLET);

	m_CurrentExtensionType = m_pWeaponInfo->getExtensionType(0); //	ù��° �ͽ��ټ����� �ʱ�ȭ ����. �߰��� �ٲ�� �ִ� ��
	setLoadedBulletCount( GetLoadBulletWithCashItem());
	setLoadedBulletDualCount(0);
	
	INT32 nMaxBullet = GetMaxBulletWithCashItem();

	setWeaponState( WEAPON::STATE_READY);
	SetEnable( true);
	SetMatrix( &I3_IDENTITYMATRIX );			//	����ġ��
		
	ResetSound();

	_OnPlayWeaponIdleAnim();

	ISNETWORK
	{
		if( m_pOwner )
		{
#if defined( USE_WEAPON_ENDURANCE)
			//	AI ĳ���ʹ� ����Ǹ� �ȵ�. komet
			if( m_pOwner->getCharaInfo() != nullptr && (m_pOwner->getCharaNetIndex() != -1) && (m_pOwner->isAI() == false) && 				
				m_pOwner->isAIToClient() == false &&
				(m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_ROCKET_LAUNCHER) &&
				(m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_OBJECT_GUN) &&
				(m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_MISSION) &&
				(m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_DINO) &&
				(m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_DINO_LEFT_SCRATCH))
			{
				const NET_CHARA_INFO * pCharaInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());

				// �⺻ ���޹��� ��� ������ ó���� ���� �ʵ��� �մϴ�.
				if( m_pWeaponInfo->IsExceptionEndurance() )
					SetEndurance( 100 );		// ������ ���� ����� �ִ밪 100
				else								
				{
					if( m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_UNKNOWN )
					{
						I3TRACE("Weapon Slot Type Unknown : %d\n", m_pWeaponInfo->GetTypeUsage() );
					}
					else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
					{
						SetEndurance( 100 );
					}
					else
					{
						WEAPON_SLOT_TYPE slotType =	m_pWeaponInfo->GetTypeUsage();

						if (slotType > WEAPON_SLOT_UNKNOWN &&  slotType < WEAPON_SLOT_COUNT)
						{
							WEAPON_SLOT_TYPE weaponSlotType = m_pWeaponInfo->GetTypeUsage();
							I3ASSERT_RETURN(weaponSlotType >= 0);
							SetEndurance(pCharaInfo->getEndurance(weaponSlotType));
						}
					}
				}
			}
#endif
		}
	}

	{
		setTotalBulletCount( nMaxBullet - getLoadedBulletCount());
	}

	//	Material
	if( m_pMaterialAttr != nullptr )
	{
		COLORREAL col;

		i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
		m_pMaterialAttr->SetDiffuse( &col);
		m_pMaterialAttr->SetSpecular( &col);
		m_pMaterialAttr->SetShininess( 64.8f);		

		OnSetBlendAlpha( 1.0f);
	}
	
	ResetCommand();

	// Hit Group
	SetHitGroup( false);

	//	�⺻���� �����տ� �⵵��
	ChangeGunDummy( WEAPON_ATTACH_GRIP);

//	m_nContactReportCount = 0;
//	m_nContactReportReadPos = 0;
	m_vecContactReport.clear();

	m_bTestRoughCollision	= false;

	m_tDropInfo.Reset();

	//	xgkick
	setDynamicActiveState( true, false);
	SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true );

	m_mainWeapon->Reset();
}

void WeaponBase::SetExtWeaponInfo()
{
	// �ͽ��ټ� ��� ������ �ͽ��ټ� ����
	if( m_pWeaponInfo->isExtensible() && m_pWeaponInfo->hasExtensionInfo() )
	{
		m_pExtWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( m_pWeaponInfo->GetSubExtensionItemID() );
	}
}

void WeaponBase::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	
	// �̸� �����صΰ�, ���������� �Ҽ� �ֵ��� �ϱ� ����..
	// �����ǰ˻縦 �߰���.
	if (!m_mainWeapon)
		m_mainWeapon.reset( this->CreateMainWeapon() );
	
	
	WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( Itemid );
	WEAPON::RES_ID number = WEAPON::ItemID2ResID( Itemid );

	m_u8EncryptKey = (UINT8) ((i3Math::Rand() % 7) + 1);

	_OnInitVairable();

	m_pOwner = pOwner;

//	char szFireEffect[100];

	if( g_pWeaponInfoDataBase != nullptr)
	{
		//	���� �����ͺ��̽��� �ִ� ���� ������ �����´�.
		m_pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( Itemid);
		if(m_pWeaponInfo == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL, "m_pWeaponInfo �� nullptr�� Itemid %d \n  \
				PEF ���Ͽ� ���� ������ ��ϵǾ� �ִ���, �Ǵ�\n��ϵ� class ��ȣ, number�� ����� ��ġ�ϴ��� Ȯ���ϼ���.", Itemid);
			return;
		}

		m_pAbsoluteWeaponInfo = m_pWeaponInfo;		//	ó�� �ʱ�ȭ���� ���õǴ� ���� �����̴�.

		m_CurrentExtensionType = m_pWeaponInfo->getExtensionType(0); //	ù��° �ͽ��ټ����� �ʱ�ȭ ����. �߰��� �ٲ�� �ִ� ��

		UINT32 flag = WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_DIRECT_USE;
		if( (m_pWeaponInfo->getExtensionType(0) & flag) && 
			(m_pWeaponInfo->getExtensionType(1) & flag) )
		{
			addWeaponFlag( WEAPON_FLAG_MULTIPLE_EXTENSION);
		}
		else
		{
			removeWeaponFlag( WEAPON_FLAG_MULTIPLE_EXTENSION);
		}

		SetExtWeaponInfo();

		//	���� �Ŵ������� ���ҽ��� �޾ƿ´�.
		// 0.5ms
		i3GameResChara * pRes = g_pWeaponManager->QueryResource( classType, number);
		if( pRes == nullptr)
		{	
			i3::rc_wstring wstrName;
			//���Ⱑ ����� �ȵ� ��� ���� �̸� ǥ�÷� ����
			m_pWeaponInfo->GetWeaponName(wstrName);

			i3::string strName;	i3::utf16_to_mb(wstrName, strName);

			I3PRINTLOG(I3LOG_FATAL, "i3GameResChara �� nullptr�� (Itemid %d ) - %s\n  \
						RSC ���Ͽ� ���� I3CHR ������ ����ߴ��� Ȯ���ϼ���.", Itemid, strName.c_str() );
			return;
		}

		if( pRes->getSceneGraphInfo() == nullptr)
		{
			i3::rc_wstring wstrName;
			//���Ⱑ ����� �ȵ� ��� ���� �̸� ǥ�÷� ����
			m_pWeaponInfo->GetWeaponName(wstrName);

			i3::string strName;		i3::utf16_to_mb(wstrName, strName);

			I3PRINTLOG(I3LOG_FATAL, "i3SceneGraphInfo �� nullptr�� (Itemid %d ) - %s\n  \
						 ������ i3S Ȯ���ϼ���.", Itemid, strName.c_str() );
			return;
		}

		// 2ms
		i3Chara::Create( g_pFramework, pRes);

		// 3ms
		Instantiate();

		//	��� ������ ����ó���� ���� AttrSet�� ���δ�.
		{
			m_pAttrSet = i3AttrSet::new_object_ref();
			m_pAttrSet->SetPriority( 0);	

			while( getAttachNode()->GetChildCount() > 0)
			{
				i3Node * pChild = getAttachNode()->GetChild( 0);
				pChild->ChangeParent( m_pAttrSet);
			}	

			getAttachNode()->AddChild( m_pAttrSet);		

			m_pBlendEnable = i3BlendEnableAttr::new_object_ref();
			m_pBlendEnable->Set( false);
			m_pAttrSet->AddAttr( m_pBlendEnable);

			i3BlendModeAttr * pBlendMode = i3BlendModeAttr::new_object_ref();
			pBlendMode->SetSource( I3G_BLEND_SRCALPHA);
			pBlendMode->SetDestination( I3G_BLEND_INVSRCALPHA);
			m_pAttrSet->AddAttr( pBlendMode);

			i3FaceCullModeAttr * pFaceCullModeAttr =i3FaceCullModeAttr::new_object_ref();

			pFaceCullModeAttr->Set( I3G_FACE_CULL_NONE);

			m_pAttrSet->AddAttr( pFaceCullModeAttr);

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
		I3ASSERT( m_pBody != nullptr);

		//	���Ƿ� �����ϱ� ���� �ϱ� ���� ������ LOD Slope���� �����մϴ�. (������ CharaTool���� ������ Slope���� ������� �ʽ��ϴ�. komet)
		m_pBody->setSlope( WEAPON_LOD_SLOPE);
		m_pBody->setConst( 0.0f);
		m_pBody->removeStyle( I3BS_LOD_AUTO);
//		m_pBody->AddFlag( I3_NODEFLAG_MUSTBEPROCESS , true);

		{
			// 5.5ms
			_LoadAllSound();

			// FX�� ���� ������ Index
			m_idxFXBone = getSceneObject()->GetBody()->FindBoneByName( "FXDummy");
			if( m_idxFXBone == -1)
			{
				m_idxFXBone = getSceneObject()->GetBody()->FindBoneByName( "FxDummy");
			}

			if(LocaleValue::GetInt32("WeaponFxDummy") == 1)
			{
				// Body���� FXDummy�� BoneRef�� ã�� ��ǥ Node�� ��ȯ
				i3Node * pTempFX = nullptr;

				for( UINT32 i = 0; i < getSceneObject()->GetBody()->getLODCount(); i++)
				{
					i3Node * pTempNode = i3Scene::FindNodeByName( getSceneObject()->GetBody()->getLOD( i)->getShapeNode(), "FXDummy");
					if( pTempNode != nullptr)
					{
						I3_REF_CHANGE( pTempFX, pTempNode);
						pTempNode->RemoveFromParent();
					}
				}

				if( pTempFX != nullptr)
				{
					getSceneObject()->GetBody()->AddChild( pTempFX);
					pTempFX->Release();
				}

				// FXDummy Boneref�� �ִ��� �˻�
				i3Node * pTempBone = i3Scene::FindNodeByName( getSceneObject(), "FXDummy");
				if( i3::same_of<i3BoneRef*>(pTempBone) && (pTempBone->GetChildCount() > 0) )
				{
					i3Node * pChild = pTempBone->GetChild(0);
					if( i3::kind_of<i3Transform*>( pChild) )
						m_pFXTrans = static_cast<i3Transform*>(pChild);
				}
			}

			// �߻� ����� �ݵ�� FXDummy�� �־�� �Ѵ�.
#if defined( I3_DEBUG)
			if( getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_RANGE
				&& classType != WEAPON_CLASS_DINO && classType != WEAPON_CLASS_BOW && classType != WEAPON_SUBCLASS_BOW)
			{
				for(UINT32 i=0; i<getSceneObject()->GetBody()->getLODCount(); i++)	
				{
					if( getSceneObject()->GetBody()->getLOD( i)->getSkeleton()->FindBoneByName( "FXDummy") == -1)
					{
						if( getSceneObject()->GetBody()->getLOD( i)->getSkeleton()->FindBoneByName( "FxDummy") == -1)
						{
							I3PRINTLOG(I3LOG_FATAL, "Must have FXDummy. please check LOD\r\nLOD %d classType %d number %d", i, classType, number);
						}
					}
				}
			}
#endif

			m_idxFXBullet = getSceneObject()->GetBody()->FindBoneByName( "EmptyCartridgeFXDummy");
				
			//IdleEffect���� ����� Bone�� �ε��� ��ȣ�� ���Ѵ�
			m_pGunDummy = i3Scene::FindNodeByName(getSceneObject()->GetBody()->getFrameLOD()->getShapeNode(), "GunDummy");
			if( m_pGunDummy != nullptr)
			{
				if (i3::same_of<i3BoneRef*>(m_pGunDummy))
				{
					m_idxFXIdleBone = ((i3BoneRef*)m_pGunDummy)->getBoneIndex();
				}
			}
			

			//	Left-Hand Gun Dummy
			m_pGunDummyLeftHand = i3Scene::FindNodeByName( getSceneObject()->GetBody()->getFrameLOD()->getShapeNode(), "LHandGunDummy");

			if( m_pGunDummyLeftHand != nullptr)
			{
				if( i3::same_of<i3BoneRef*>(m_pGunDummyLeftHand))
				{
					INT32 nBoneIdx = ((i3BoneRef*)m_pGunDummyLeftHand)->getBoneIndex();
					i3Bone * pBone = getSceneObject()->GetBody()->getFrameLOD()->getSkeleton()->getBone( nBoneIdx);

					i3Matrix::Copy( &m_mtxGunDummyLeftHand, pBone->getMatrix());

					i3Matrix::PostRotateXDeg( &m_mtxGunDummyLeftHand, 90.f );
					i3Matrix::PostRotateZDeg( &m_mtxGunDummyLeftHand, 90.f );	

					i3Matrix::Inverse( &m_mtxGunDummyLeftHand, 0, &m_mtxGunDummyLeftHand );

					m_idxFXIdleBone = nBoneIdx;
				}
			}
			else
			{
				//LHandGunDummy��� �̸���� WholeAniDummy��� �̸����� �� ���⵵ ����
				m_pGunDummyLeftHand = i3Scene::FindNodeByName(getSceneObject()->GetBody()->getFrameLOD()->getShapeNode(), "WholeAniDummy");
				if (m_pGunDummyLeftHand != nullptr)
				{
					if (i3::same_of<i3BoneRef*>(m_pGunDummyLeftHand))
					{
						m_idxFXIdleBone = ((i3BoneRef*)m_pGunDummyLeftHand)->getBoneIndex();
					}
				}
			}

			// �߻� ����Ʈ
			if( m_pWeaponInfo->hasFireEffect())
			{
				i3::rc_wstring wstrFireEffect;
				i3::string strFireEffect;

				for(INT32 i=0; i<MAX_FIRE_EFFECT; i++)
				{
					m_pWeaponInfo->GetFireEffect( wstrFireEffect, i);
					if( i3::generic_string_size( wstrFireEffect) > 0)
					{
						i3::utf16_to_mb(wstrFireEffect, strFireEffect);
						m_idxFireEffect[i] = g_pEffectManager->FindEffect(strFireEffect.c_str());
					}
				}

				//	Steady Aim�� ��� '_1st' ����Ʈ�� ��� �պκп� �۰� ���̱� ������ ����ó��. komet
				if( GetCurrentExtensionType() != WEAPON::EXT_STEADY_AIM)
				{
					char conv[MAX_PATH];
					m_pWeaponInfo->GetFireEffect(wstrFireEffect);
					i3::utf16_to_mb(wstrFireEffect, strFireEffect);

					i3::snprintf( conv, MAX_PATH, "%s%s", strFireEffect, "_1st");

					m_idxFireEffect_1PV_Zoom = g_pEffectManager->FindEffect( conv);
				}

				m_idxFireEffect_1PV_Zoom = ( m_idxFireEffect_1PV_Zoom == -1) ? m_idxFireEffect[0] : m_idxFireEffect_1PV_Zoom;
			}
			else
			{
				m_idxFireEffect[0] = g_pEffectManager->FindEffect( "Fir_Ar_M4A1");
				m_idxFireEffect_1PV_Zoom = m_idxFireEffect[0];
			}

			// ź�� ����Ʈ
			{
				char * pszEffName = nullptr;

				switch( m_pWeaponInfo->GetTypeClass())
				{
					case WEAPON_CLASS_CIC			:
					case WEAPON_CLASS_DUALHANDGUN	:
					case WEAPON_CLASS_HANDGUN		:	pszEffName = "Bul_Hg_MK23";		break;
					case WEAPON_SUBCLASS_ASSAULT	:
					case WEAPON_CLASS_ASSAULT		:	pszEffName = "Bul_Ar_M4A1";		break;	
					case WEAPON_SUBCLASS_SMG		:
					case WEAPON_CLASS_SMG			:			
					case WEAPON_CLASS_DUALSMG		:	pszEffName = "Bul_Smg_MP5K";	break;
					case WEAPON_SUBCLASS_SNIPER		:
					case WEAPON_CLASS_SNIPER		:	pszEffName = "Bul_Sr_HKPSG1";	break;
					case WEAPON_SUBCLASS_SHOTGUN	:
					case WEAPON_CLASS_SHOTGUN		:		
					case WEAPON_CLASS_DUALSHOTGUN	:	pszEffName = "Bul_Sg_870MCS";	break;
					case WEAPON_CLASS_MG			:	pszEffName = "Bul_Ar_M4A1";		break;
				}

				if( pszEffName != nullptr)
				{
					m_idxBulletEffect = g_pEffectManager->FindEffect( pszEffName);

					if( m_idxBulletEffect == -1)
					{
						#if defined( I3_DEBUG)//����� ����
							g_pEffectManager->Dump();
						#endif

						m_idxBulletEffect = g_pEffectManager->FindEffect( pszEffName);
					}

					I3ASSERT( m_idxBulletEffect != -1);
				}
				else
				{
					m_idxBulletEffect = -1;
				}
			}


			if( m_pWeaponInfo->GetEnableExtFireEffectOffSet() )
			{
				i3BoneRef * pFXBoneRef = i3BoneRef::new_object_ref();
				pFXBoneRef->setBoneIndex( m_idxFXBone );	
				m_pFXLastTrans = i3Transform::new_object();
				pFXBoneRef->AddChild( m_pFXLastTrans);
				
				MATRIX mtx;		
				
				m_pWeaponInfo->GetPosExtFireEffectOffSet( i3Matrix::GetPos(&mtx) );
				m_pFXLastTrans->SetMatrix(&mtx);
				
				getSceneObject()->GetBody()->AddChild( pFXBoneRef );
			}

			//��Ʈ ����Ʈ
			i3::rc_wstring	wsObjHitEffect;
			m_pWeaponInfo->GetObjHitEffect(wsObjHitEffect);
			i3::string strEffect;	
			i3::utf16_to_mb(wsObjHitEffect, strEffect);
			ObjHitEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
			if(ObjHitEffIdx == -1)
				ObjHitEffIdx = -2;


			i3::rc_wstring	wsCharaHitEffect;
			m_pWeaponInfo->GetCharaHitEffect(wsCharaHitEffect);
			i3::utf16_to_mb(wsCharaHitEffect, strEffect);
			CharaHitEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
			if(CharaHitEffIdx == -1)
				CharaHitEffIdx = -2;

			i3::rc_wstring	wsCharaHitEffect1PV;
			m_pWeaponInfo->GetCharaHitEffect1PV(wsCharaHitEffect1PV);
			i3::utf16_to_mb(wsCharaHitEffect1PV, strEffect);
			CharaHitEff1PVIdx = g_pEffectManager->FindEffect(strEffect.c_str());
			if(CharaHitEff1PVIdx == -1)
				CharaHitEff1PVIdx = -2;

			i3::rc_wstring	wsDecalEffectExt;
			m_pWeaponInfo->GetDecalEffectExt(wsDecalEffectExt);
			i3::utf16_to_mb(wsDecalEffectExt, strEffect);
			DecalEffExtIdx = g_pEffectManager->FindEffect(strEffect.c_str());
			if(DecalEffExtIdx == -1)
				DecalEffExtIdx = -2;

			i3::rc_wstring	wsIdleEffect;
			m_pWeaponInfo->GetIdleEffect(wsIdleEffect);
			i3::utf16_to_mb(wsIdleEffect, strEffect);
			IdleEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
			if (IdleEffIdx == -1)
				IdleEffIdx = -2;


			if(m_pWeaponInfo->GetUnBlowEffFlag() != 0)
				UnBlowHitEffFlag = true;

			if(m_pWeaponInfo->GetUnDecalEffFlag() != 0)
				UnDecalHitEffFlag = true;



			// LaserSight
			{
				m_idxFXLaserSight = getSceneObject()->GetBody()->FindBoneByName( "FXLaserSightDummy");
			}

			// Damage
			{
				if(m_pDamage == nullptr)
					m_pDamage = new weapon::Damage( this);
			}
		}

	}
	
	if ( m_pOwner == nullptr)
	{
		m_idxColorForFireEffect = 0;
	}
	else
	{
		INT32 SlotIdx = pOwner->getCharaInfo()->GetNetIdx();

		if (m_pWeaponInfo && m_pWeaponInfo->GetUnColorEffectFlag())
		{
			m_idxColorForFireEffect = 0;
		}
		else if(SlotIdx == BattleSlotContext::i()->getMySlotIdx())
		{
			USER_INFO_INVITEM_DATA Data;
			Data.Reset();
			UserInfoContext::i()->GetMyUserInfoInvItemData(&Data);
			m_idxColorForFireEffect = Data._ui16MuzzleFlashColor;
		}
		else
		{
			m_idxColorForFireEffect = BattleSlotContext::i()->getFireMuzzleColorForSlot(SlotIdx);
		}
	}

	{
		I3ASSERT( m_pLight == nullptr);
		m_pLight = i3Light::new_object();

		m_pLight->SetLightType( I3G_LIGHT_POINT);

		m_pLight->addState( I3G_LIGHT_STATE_DISABLE);
	}

#if defined( TEST_COLLISION_LINE )
	COLORREAL Col;
	i3Color::Set( &Col, 0.8f, 0.8f, 0.0f, 0.8f);
	i3Node * pNode = (i3Node*) i3SceneUtil::BuildLine( m_pVA, 1, &Col);
	g_pFramework->Get3DRoot()->AddChild( pNode);
#endif

	// 0.1ms
	{
		// Init
		_InitParameters();

		_InitCollideeGroup();
		
		// �⺻������ ������Ʈ�� ��򰡿� �ٿ��ݴϴ�.
		setDynamicActiveState( true, false);
		SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true);
	}
	
	if( CHud::i() != nullptr )
	{
		INT32 temp_index = m_pWeaponInfo->GetWeaponSpecialHitHudIndex();
		if( temp_index > -1 )
		{
			CHud::i()->InsertSpecialDamageFilter( temp_index );
		}

		temp_index = m_pWeaponInfo->GetWeaponSpecialDieHudIndex();
		if (temp_index > -1)
		{
			CHud::i()->InsertSpecialDieFilter(temp_index);
		}

		temp_index = m_pWeaponInfo->GetScopeType();
		if (temp_index > -1)
		{
			CHud::i()->InsertScopeHudTexture(temp_index);
		}
	}

	//
	m_mainWeapon->OnCreate(bCreateInWeapon);
	PostCreate();

	this->EnableUpdateAABB(true);

	this->EnableRenderAABB(true);

}

void WeaponBase::PostCreate( void)
{
}

MainWeapon*	WeaponBase::CreateMainWeapon() 
{
	MainWeapon * p = new MainWeapon(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon));
	return p;
}

void WeaponBase::PreCreateMainWeaponBeforeCreateCall()
{
	if (m_mainWeapon) return;	// ������� ��� �����ϸ� ��..
	m_mainWeapon.reset( this->CreateMainWeapon() );
}

void WeaponBase::_InitCollideeGroup( void)
{
//	InitColliderSet( CGRP_ITEM, CGRP_MASK_ITEM );
	WF::SetCollideeGroup_NormalWeapon(this);
}


void WeaponBase::OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	if( m_pMaterialAttr != nullptr )
	{
		if( pfDiffuse != nullptr)	
		{
			REAL32 fDiffuseAlpha = (REAL32) i3Color::GetA( m_pMaterialAttr->GetDiffuse());
			m_pMaterialAttr->SetDiffuse( pfDiffuse->r, pfDiffuse->g, pfDiffuse->b, fDiffuseAlpha);
		}

		if( pfSpecular != nullptr)	
		{
			REAL32 fSepcularAlpha = (REAL32) i3Color::GetA( m_pMaterialAttr->GetSpecular());
			m_pMaterialAttr->SetSpecular( pfSpecular->r, pfSpecular->g, pfSpecular->b, fSepcularAlpha);
		}
	}

	m_mainWeapon->OnSetMaterialColor(pfDiffuse, pfSpecular);
}

void WeaponBase::CheckAlphaUsed( void)
{
	I3ASSERT( m_pBlendEnable != nullptr );

	bool bCheck = false;
	bool bOldBlend = m_pBlendEnable->Get();

	bCheck = getWeaponInfo()->IsAlphablendUsed();

	if( bOldBlend )
	{
		if( m_tDropInfo._bDropping )
		{
			m_pBlendEnable->Set( false);
		}
		else if( !bCheck )
		{
			m_pBlendEnable->Set( false);
		}
	}
	else
	{
		if( m_tDropInfo._bDropping )
		{
			// ��Ʈ��ũ �Ǵ� ���忡 �ִ� ����
		}
		else if( bCheck )
		{	// �ڽ��� ��� �ִ� �����߿��� ���带 ���� �ȵǴ� �����
			m_pBlendEnable->Set( true);
		}
	}
}

void WeaponBase::OnSetBlendAlpha( REAL32 Val)
{
	REAL32 val = i3Math::Clamp( Val, 0.0f, 1.0f);

	if( (m_fAlphaVal != val) && (m_pMaterialAttr != nullptr) )
	{
		i3Color::SetA( m_pMaterialAttr->GetDiffuse(), val);
		i3Color::SetA( m_pMaterialAttr->GetSpecular(), val);
		
		if( val == 1.0f)	CheckAlphaUsed();
		else
		{
			if( m_pBlendEnable->Get() == false)
				m_pBlendEnable->Set( true);
		}

		m_fAlphaVal = val;
	}

	m_mainWeapon->OnSetBlendAlpha(Val);
}

void WeaponBase::_LoadAllSound(void)
{
	//��Ʋ�� �ƴ� ��쿡 ȿ������ ��Ȱ��ȭ�� �ε����ص� �� ��...
	//��Ʋ�� ��쿣 ���۽� �ε��ϱ� ������ ȿ���� ���� ������ ������� �ε�..
	if( g_pFramework->IsBattleStage() == false && g_pEnvSet->m_bEnableEffect == false ) return;

	_LoadFireSound();
	_LoadSwapSound();
	_LoadIdleSound();
}

bool WeaponBase::GenerateWeaponSndName(char* buf, INT32 bufSiz, CWeaponInfo* winfo, 
									   WEAPON::FIRE_SOUND_TYPE type)
{	
//	char szGetFireSound[256];
//	char szGetFireSoundExt[256];

	i3::rc_wstring wstrGetFireSound;
	i3::rc_wstring wstrGetFireSoundExt;

	winfo->GetFireSound(wstrGetFireSound);
	winfo->GetFireSoundExt(wstrGetFireSoundExt);

	INT32 length = i3::generic_string_size(wstrGetFireSound);
	if( length <= 0)
		return false;

	i3::string strGetFireSound;			i3::utf16_to_mb(wstrGetFireSound, strGetFireSound);
	i3::string strGetFireSoundExt;		i3::utf16_to_mb(wstrGetFireSoundExt, strGetFireSoundExt);

	switch((WEAPON::FIRE_SOUND_TYPE)type)
	{
	case  WEAPON::FIRE_SOUND_NORMAL:		//	�Ϲ� ��� ����
		{				
			i3::snprintf(buf, bufSiz, "%s.wav", strGetFireSound);
		}
		break;

	case WEAPON::FIRE_SOUND_NORMAL_2:
		{
			i3::snprintf(buf, bufSiz, "%s_Secondary.wav", strGetFireSound);
		}
		break;

	case WEAPON::FIRE_SOUND_MUFFLER:
		{
			i3::snprintf(buf, bufSiz, "%s_Muffler.wav", strGetFireSound);
		}
		break;

	case WEAPON::FIRE_SOUND_SCOPE:		//	���� ����
		{
			//if( getWeaponInfo()->m_TypeClass == WEAPON_CLASS_SNIPER)
			{
				i3::safe_string_copy( buf, strGetFireSound, bufSiz );

				INT32	nLength = i3::generic_string_size( buf);
				INT32	nPos	= nLength - 4;
				char * file = &buf[nPos];
				i3::snprintf( file, bufSiz - (nPos + 1), "%s", "Zoom_Fire.wav");
			}
		}
		break;

	case WEAPON::FIRE_SOUND_EXT :
		{
			// ���� FireSoundExt �� �����Ǿ� �ִٸ� �켱
			if( i3::generic_string_size( strGetFireSoundExt) > 0)
				i3::snprintf(buf, bufSiz, "%s.wav", strGetFireSoundExt);
			// m_pExtWeaponInfo�� ������ FireSound�� ����
			else
			{

				// �ͽ��ټ� ��� ������ �ͽ��ټ� ����
				winfo->GetFireSound( wstrGetFireSoundExt);
				i3::utf16_to_mb(wstrGetFireSoundExt, strGetFireSoundExt);

				i3::snprintf(buf, bufSiz, "%s.wav", strGetFireSoundExt);

			}
		}
		break;

	case WEAPON::FIRE_SOUND_NORMAL_3PV:
		{
			i3::snprintf(buf, bufSiz, "%s_3pv.wav", strGetFireSound);
		}
		break;

	default:
		return false;
	}

	return true;
}

void WeaponBase::_LoadFireSound()
{
	I3ASSERT_RETURN( m_pWeaponInfo != nullptr);
	
//	char szGetFireSound[256];
//	char szGetFireSoundExt[256];
	i3::rc_wstring wstrGetFireSound, wstrGetFireSoundExt;

	m_pWeaponInfo->GetFireSound(wstrGetFireSound);
	m_pWeaponInfo->GetFireSoundExt(wstrGetFireSoundExt);

	INT32 length = i3::generic_string_size( wstrGetFireSound);
	if( length <= 0)	return;

	i3::string strGetFireSound, strGetFireSoundExt;
	i3::utf16_to_mb(wstrGetFireSound, strGetFireSound);
	i3::utf16_to_mb(wstrGetFireSoundExt, strGetFireSoundExt);

	for( INT32 i = 0; i < WEAPON::FIRE_SOUND_COUNT; i++)
	{
		char conv[MAX_PATH];
	
		switch( (WEAPON::FIRE_SOUND_TYPE) i)
		{
		case  WEAPON::FIRE_SOUND_NORMAL:		//	�Ϲ� ��� ����
			{				
				i3::snprintf( conv, MAX_PATH, "%s.wav", strGetFireSound);
			}
			break;
		case WEAPON::FIRE_SOUND_NORMAL_2:
			{
				i3::snprintf( conv, MAX_PATH, "%s_Secondary.wav", strGetFireSound);
			}
			break;
		case WEAPON::FIRE_SOUND_MUFFLER:
			{
				i3::snprintf( conv, MAX_PATH, "%s_Muffler.wav", strGetFireSound);
			}
			break;
		case WEAPON::FIRE_SOUND_SCOPE:		//	���� ����
			{
				//if( getWeaponInfo()->m_TypeClass == WEAPON_CLASS_SNIPER)
				{
					i3::safe_string_copy( conv, strGetFireSound, MAX_PATH );

					INT32	nLength = i3::generic_string_size( conv);
					INT32	nPos	= nLength - 4;
					char * file = &conv[nPos];
					i3::snprintf( file, MAX_PATH - (nPos + 1), "%s", "Zoom_Fire.wav");
				}
			}
			break;
		case WEAPON::FIRE_SOUND_EXT :
			{
				// ���� FireSoundExt �� �����Ǿ� �ִٸ� �켱
				if( i3::generic_string_size( strGetFireSoundExt) > 0)
					i3::snprintf( conv, MAX_PATH, "%s.wav",strGetFireSoundExt);
				// m_pExtWeaponInfo�� ������ FireSound�� ����
				else
				{
					if( m_pExtWeaponInfo)
					{
						// �ͽ��ټ� ��� ������ �ͽ��ټ� ����
						m_pExtWeaponInfo->GetFireSound( wstrGetFireSoundExt);
						i3::utf16_to_mb(wstrGetFireSoundExt, strGetFireSoundExt);

						i3::snprintf( conv, MAX_PATH, "%s.wav", strGetFireSoundExt);
					}
				}
			}
			break;

		case WEAPON::FIRE_SOUND_NORMAL_3PV:
			{
				i3::snprintf( conv, MAX_PATH, "%s_3pv.wav", strGetFireSound);
			}
			break;
		}		

		i3GameResSound * pSoundRes = (i3GameResSound *) g_pFramework->QuaryResource( conv);
		
		if( pSoundRes != nullptr)
		{
			I3_SAFE_RELEASE( m_pFireSoundPlayInfo[ i]);

			i3SoundPlayInfo * pSrcInfo = pSoundRes->getSound();
			
			m_pFireSoundPlayInfo[ i] = i3SoundPlayInfo::new_object();

			pSrcInfo->CopyTo( m_pFireSoundPlayInfo[ i], I3_COPY_INSTANCE);
			m_pFireSoundPlayInfo[ i]->setMuteAtMaxDistance( true);

			if((m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_OBJECT_GUN) && (WEAPON::FIRE_SOUND_NORMAL == (WEAPON::FIRE_SOUND_TYPE)i))
			{
				m_pFireSoundPlayInfo[ i]->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);
			}
		}
	}
}

void WeaponBase::_LoadSwapSound( void)
{
	
	I3ASSERT( m_pWeaponInfo != nullptr);
	i3::rc_wstring wstrSwapSound;
	m_pWeaponInfo->GetSwapSound(wstrSwapSound);
	INT32 length = i3::generic_string_size(wstrSwapSound);
	char conv[MAX_PATH] = "";
	if( length > 0)
	{
		i3::string strSwapSound;
		i3::utf16_to_mb(wstrSwapSound, strSwapSound);
		i3::snprintf( conv, MAX_PATH, "%s.wav", strSwapSound);

		i3GameResSound * pSoundRes = (i3GameResSound *) g_pFramework->QuaryResource( conv);

		if( pSoundRes != nullptr)
		{
			I3_SAFE_RELEASE( m_pSwapSoundPlayInfo);
			i3SoundPlayInfo * pSrcInfo = pSoundRes->getSound();
			m_pSwapSoundPlayInfo = i3SoundPlayInfo::new_object();
			pSrcInfo->CopyTo( m_pSwapSoundPlayInfo, I3_COPY_INSTANCE);
			m_pSwapSoundPlayInfo->setMuteAtMaxDistance( true);
		}
#if defined( I3_DEBUG)
		else
		{
			char szWeaponName[MAX_STRING_COUNT];
			char tmp[256] = "";
			
			INT32 len = i3::generic_string_size( m_pWeaponInfo->GetName());

			if( len > 0 )
			{
				I3ASSERT( len < MAX_STRING_COUNT);
				GlobalFunc::DecryptValue( szWeaponName, m_pWeaponInfo->GetName(), len, ENC_SHIFT_WEAPONINFO_RESNAME);
				sprintf_s( tmp, "Could not load weapon (%s) swap sound file (%s)", szWeaponName, conv);
				I3PRINTLOG(I3LOG_WARN,  tmp);
			}
		}
#endif
	}
}

void WeaponBase::_LoadIdleSound()
{
	I3ASSERT(m_pWeaponInfo != nullptr);
	i3::rc_wstring wstrIdleSound;
	m_pWeaponInfo->GetIdleSound(wstrIdleSound);
	INT32 length = i3::generic_string_size(wstrIdleSound);
	char conv[MAX_PATH] = "";
	if (length > 0)
	{
		i3::string strIdleSound;
		i3::utf16_to_mb(wstrIdleSound, strIdleSound);
		i3::snprintf(conv, MAX_PATH, "%s.wav", strIdleSound);

		i3GameResSound * pSoundRes = (i3GameResSound *)g_pFramework->QuaryResource(conv);

		if (pSoundRes != nullptr)
		{
			I3_SAFE_RELEASE(m_pIdleSoundPlayInfo);
			i3SoundPlayInfo * pSrcInfo = pSoundRes->getSound();
			m_pIdleSoundPlayInfo = i3SoundPlayInfo::new_object();
			pSrcInfo->CopyTo(m_pIdleSoundPlayInfo, I3_COPY_INSTANCE);
			m_pIdleSoundPlayInfo->setMuteAtMaxDistance(true);
			m_pIdleSoundPlayInfo->setLoopMode(I3SND_RT_LOOPMODE_LOOPING);
		}
#if defined( I3_DEBUG)
		else
		{
			char szWeaponName[MAX_STRING_COUNT];
			char tmp[256] = "";

			INT32 len = i3::generic_string_size(m_pWeaponInfo->GetName());

			if (len > 0)
			{
				I3ASSERT(len < MAX_STRING_COUNT);
				GlobalFunc::DecryptValue(szWeaponName, m_pWeaponInfo->GetName(), len, ENC_SHIFT_WEAPONINFO_RESNAME);
				sprintf_s(tmp, "Could not load weapon (%s) Idle sound file (%s)", szWeaponName, conv);
				I3PRINTLOG(I3LOG_WARN, tmp);
			}
		}
#endif
	}
}

void WeaponBase::_PlayFireSound( WEAPON::FIRE_SOUND_TYPE type)
{
	I3ASSERT( g_pViewer != nullptr);

	// ���� �÷����� �ʿ� ����.
	if( isWeaponFlag( WEAPON_FLAG_ENABLE_FIRE_SOUND) == false) return;

	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	if( pSndCtx == nullptr)	return;

	bool bSameInfo = true;


	if( m_pOwner->is3PV() )
	{
		if( m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SNIPER &&
			m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex( WEAPON::SNIPER_SVU)  )
			type = WEAPON::FIRE_SOUND_NORMAL_3PV;
	}

	i3SoundPlayInfo * pPlayInfo = m_pFireSoundPlayInfo[ type];
	if( pPlayInfo == nullptr)
	{
		I3TRACE( "[WeaponBase::_PlayFireSound] WEAPON::FIRE_SOUND_TYPE (%d) sound not have. now play default fire sound.\n", type);

		pPlayInfo = m_pFireSoundPlayInfo[ WEAPON::FIRE_SOUND_NORMAL];
	}

	if( pPlayInfo == nullptr)			
	{
		char szWeaponName[ MAX_STRING_COUNT];
		char temp[1024] = "";
		const char * pName = m_pWeaponInfo->GetName();

		INT32 len = i3::generic_string_size( pName);

		if( len > 0)
		{
			I3ASSERT( len < MAX_STRING_COUNT);
			GlobalFunc::DecryptValue( szWeaponName, pName, len, ENC_SHIFT_WEAPONINFO_RESNAME);

			sprintf_s(temp, "[_PlayFireSound] %s (TYPE %d) ���带 �÷����� �� �����ϴ�. ������ ���� ������ ���ų� ���� ���߽��ϴ�. Ȯ���� �ּ���.\n", szWeaponName, type); 

			I3TRACE( temp);
		}

		return;
	}

	if( m_pFireSoundPlayState != nullptr)
	{
		if( m_pFireSoundPlayState->getPlayInfo() != pPlayInfo)
			bSameInfo = false;
	}

	CGameCharaBase * pOwner = m_pOwner;
	
	VEC3D * pPos = nullptr;
	if( getFXMatrix() != nullptr)
	{
		pPos = i3Matrix::GetPos( getFXMatrix());
	}
	else if( pOwner != nullptr )
	{
		i3Node * pNode = pOwner->getWeaponContext()->getAttachRoot();
		if( i3::same_of<i3Transform*>(pNode))
		{
			i3Transform * pTrans = ( i3Transform *) pNode;

			I3ASSERT( i3::same_of<i3Transform*>(pTrans));

			pPos = i3Matrix::GetPos( pTrans->GetCacheMatrix());
		}
		else if( i3::same_of<i3BoneRef*>(pNode))
		{
			pPos = i3Matrix::GetPos( pOwner->getBoneContext()->getBoneMatrix( ((i3BoneRef*)pNode)->getBoneIndex() ) );
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  "Weapon attach node is invalid.");
			return;
		}
	}

	if( bSameInfo && m_pFireSoundPlayState != nullptr)
	{
		if( m_pWeaponInfo->GetTypeClass()  != WEAPON_CLASS_OBJECT_GUN)
		{
			m_pFireSoundPlayState = pSndCtx->RePlay( m_pFireSoundPlayState, pPlayInfo, pPos);
		}
		else
		{
			MATRIX mtx;
			i3Matrix::SetTranslate( &mtx, pPos);

			m_pFireSoundPlayState->UpdatePos( &mtx);
		}
	}
	else
	{
		if( pOwner != nullptr && pOwner->is1PV())
		{
			pPlayInfo->set3DMode( I3SND_RT_3DMODE_DISABLE);
		}
		else
		{
			pPlayInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);
		}

		m_pFireSoundPlayState = pSndCtx->Play( pPlayInfo, pPos);		
	}
}

void WeaponBase::_PlaySwapSound( void)
{
	I3ASSERT( g_pViewer != nullptr);

	i3SoundPlayInfo * pPlayInfo = m_pSwapSoundPlayInfo;
	if( pPlayInfo == nullptr)			
	{
		/*char temp[1024] = "";
		i3::rc_wstring wstrWeaponName;
		i3::rc_wstring wstrSwapSound;

		getWeaponInfo()->GetSwapSound(wstrSwapSound);
		getWeaponInfo()->GetWeaponName(wstrWeaponName);

		i3::snprintf(temp, 256, "[_PlaySwapSound] (%s) %s ���带 �÷����� �� �����ϴ�. ������ ���� ������ ���ų� ���� ���߽��ϴ�. Ȯ���� �ּ���.\n", 
						wstrWeaponName, wstrSwapSound); 

		I3PRINTLOG(I3LOG_WARN,  temp);*/	// ���� ������ ���� ���Ⱑ �ִ�.
		return;
	}

	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	if( pSndCtx == nullptr)	return;

	bool bSameInfo = true;

	if( m_pSwapSoundPlayState != nullptr)
	{
		if( m_pSwapSoundPlayState->getPlayInfo() != pPlayInfo)
			bSameInfo = false;
	}

	VEC3D * pPos = nullptr;
	if( getFXMatrix() != nullptr)
	{
		pPos = i3Matrix::GetPos( getFXMatrix());
	}
	else if( m_pOwner != nullptr )
	{
		i3Node * pNode = m_pOwner->getWeaponContext()->getAttachRoot();
		if( i3::same_of<i3Transform*>(pNode))
		{
			i3Transform * pTrans = ( i3Transform *) pNode;

			I3ASSERT( i3::same_of<i3Transform*>(pTrans));

			pPos = i3Matrix::GetPos( pTrans->GetCacheMatrix());
		}
		else if( i3::same_of<i3BoneRef*>(pNode))
		{
			pPos = i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( ((i3BoneRef*)pNode)->getBoneIndex() ) );
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  "Weapon attach node is invalid.");
			return;
		}
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL, "m_pOwner == nullptr && Weapon attach node is invalid.");
		return;
	}

	if( bSameInfo && m_pSwapSoundPlayState != nullptr)
	{
		if( m_pWeaponInfo->GetTypeClass()  != WEAPON_CLASS_OBJECT_GUN)
		{
			m_pSwapSoundPlayState = pSndCtx->RePlay( m_pSwapSoundPlayState, pPlayInfo, pPos);
		}
		else
		{
			MATRIX mtx;
			i3Matrix::SetTranslate( &mtx, pPos);

			m_pSwapSoundPlayState->UpdatePos( &mtx);
		}
	}
	else
	{
		if( m_pOwner != nullptr && m_pOwner->is1PV())
		{
			pPlayInfo->set3DMode( I3SND_RT_3DMODE_DISABLE);
		}
		else
		{
			pPlayInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);
		}

		m_pSwapSoundPlayState = pSndCtx->Play( pPlayInfo, pPos);
	}
}

void WeaponBase::_PlayIdleSound(void)
{
	I3ASSERT(g_pViewer != nullptr);

	i3SoundPlayInfo * pPlayInfo = m_pIdleSoundPlayInfo;
	if (pPlayInfo == nullptr)
	{
		return;
	}

	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	if (pSndCtx == nullptr)	return;

	bool bSameInfo = true;

	if (m_pIdleSoundPlayState != nullptr)
	{
		if (m_pIdleSoundPlayState->getPlayInfo() != pPlayInfo)
			bSameInfo = false;
	}

	VEC3D * pPos = nullptr;
	if (getFXMatrix() != nullptr)
	{
		pPos = i3Matrix::GetPos(getFXMatrix());
	}
	else if (m_pOwner != nullptr)
	{
		i3Node * pNode = m_pOwner->getWeaponContext()->getAttachRoot();
		if (i3::same_of<i3Transform*>(pNode))
		{
			i3Transform * pTrans = (i3Transform *)pNode;

			I3ASSERT(i3::same_of<i3Transform*>(pTrans));

			pPos = i3Matrix::GetPos(pTrans->GetCacheMatrix());
		}
		else if (i3::same_of<i3BoneRef*>(pNode))
		{
			pPos = i3Matrix::GetPos(m_pOwner->getBoneContext()->getBoneMatrix(((i3BoneRef*)pNode)->getBoneIndex()));
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL, "Weapon attach node is invalid.");
			return;
		}
	}

	if (bSameInfo && m_pIdleSoundPlayState != nullptr)
	{
		if (m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_OBJECT_GUN)
		{
			m_pIdleSoundPlayState = pSndCtx->RePlay(m_pIdleSoundPlayState, pPlayInfo, pPos);
		}
		else
		{
			MATRIX mtx;
			i3Matrix::SetTranslate(&mtx, pPos);

			m_pIdleSoundPlayState->UpdatePos(&mtx);
		}
	}
	else
	{
		if (m_pOwner != nullptr && m_pOwner->is1PV())
		{
			pPlayInfo->set3DMode(I3SND_RT_3DMODE_DISABLE);
		}
		else
		{
			pPlayInfo->set3DMode(I3SND_RT_3DMODE_WORLD_SPACE);
		}

		m_pIdleSoundPlayState = pSndCtx->Play(pPlayInfo, pPos);
	}
}

void WeaponBase::_UpdateSound(REAL32 rDeltaSeconds)
{
	if( m_pFireSoundPlayState != nullptr)
	{
		//if( m_pFireSoundPlayState->isFree() || (m_pFireSoundPlayState->getRequest() & I3SND_RT_REQ_STOP))
		if( m_pFireSoundPlayState->isFree())
		{
			m_pFireSoundPlayState = nullptr;
		}
		else
		{
			/*
			if( !m_pOwner->is1PV())
			{
				if( getFXMatrix() != nullptr)
					m_pFireSoundPlayState->Req_UpdatePos( i3Matrix::GetPos( getFXMatrix()), rDeltaSeconds);
				else
					m_pFireSoundPlayState->Req_UpdatePos( i3Matrix::GetPos( GetMatrix()), rDeltaSeconds);
			}
			*/
		}
	}

	if( m_pSwapSoundPlayState != nullptr)
	{
		if( m_pSwapSoundPlayState->isFree())
		{
			m_pSwapSoundPlayState = nullptr;
		}
	}

	if( m_pIdleSoundPlayState != nullptr)
	{
		if(m_pIdleSoundPlayState->isFree())
		{
			m_pIdleSoundPlayState = nullptr;
		}
	}
}

void WeaponBase::SetSwapSoundPlayState(const UINT32 nState)
{
	if (m_pSwapSoundPlayState)
		m_pSwapSoundPlayState->SetPlayState(nState);
}

void WeaponBase::_StopFireSound()
{
	if( m_pFireSoundPlayState != nullptr)
	{
		m_pFireSoundPlayState->Stop();
		m_pFireSoundPlayState = nullptr;
	}
}

void WeaponBase::_StopSwapSound()
{
	if( m_pSwapSoundPlayState != nullptr)
	{
		m_pSwapSoundPlayState->Stop();
		m_pSwapSoundPlayState = nullptr;
	}
}

void WeaponBase::_StopIdleSound(void)
{
	if (m_pIdleSoundPlayState != nullptr)
	{
		m_pIdleSoundPlayState->Stop();
		m_pIdleSoundPlayState = nullptr;
	}
}

void WeaponBase::_InitParameters(void)
{
	CWeaponInfo * pInfo = getWeaponInfo();
	I3ASSERT( pInfo != nullptr);

	setLoadedBulletDualCount( i3Math::softAdd( GetLoadBulletWithCashItem(), 0));
	setLoadedBulletCount( i3Math::softAdd( GetLoadBulletWithCashItem(), 0));
	setTotalBulletCount( i3Math::softAdd( GetMaxBulletWithCashItem(), 0));
}

void WeaponBase::_TransformWorldSpace( REAL32 tm)
{
	i3SceneTracer * pTracer = g_pFramework->GetViewer()->GetSceneTracer();
	I3ASSERT( pTracer != nullptr);

	pTracer->GetModelViewMatrixStack()->Reset();
	pTracer->SetTime( tm);
	pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

	getSceneObject()->OnUpdate( pTracer);
}


void WeaponBase::FireReady( void)
{	//	�߻� �غ� ����
	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()) )
		addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);

	m_mainWeapon->FireReady();
}

bool WeaponBase::Fire( WEAPON::FIRE_ORDER type)
{
	if( getLoadedBulletCount() <= 0)
		return false;

	m_FireOrder = type;

	// ������ ���ϰ� �ִ� ��ǥ ������ ���Ѵ�.
	GetShotTargetPos( m_TargetPos);

	NET_Fire( nullptr, m_TargetPos, m_FireOrder );

	return true;
}

void WeaponBase::FireEnd(void)
{
	removeWeaponStateMask( WSM_FIRING | WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);
	
	if(getWeaponInfo()->GetPumpAction() && !isWeaponStateMask( WSM_LOADED_BULLET))
	{
		addWeaponStateMask( WSM_FIRE_DISABLE);
	}

	setAutoFireCount(0);
	setRemainAutoFireCount(0);
}

void WeaponBase::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{	
	VEC3D target, start, dir, vXAxis, vYAxis;
	REAL32 length;
	INT32 i, collGroup;

	if( m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_THROWING1 )
		return;

	if( m_pOwner == nullptr)
		return;

	i3Vector::Copy( &target, pTargetPos);

	if( pStartPos == nullptr )
	{
		// �Ѿ��� �߻� �������� (1��Ī�� ��쿡��) Camera�� Position����,
		// (3��Ī�� ��쿡��) �ѱ� �������� �Ѵ�.
		if( m_pOwner->is1PV() || m_pOwner->isAI())
		{
			// ����� Camera�� ����
			m_pOwner->getCameraContext()->getVirtualCamPosition( &start );
		}
		else
		{
			i3Vector::Copy( &start, i3Matrix::GetPos( getFXMatrix()));
		}
	}
	else
	{
		i3Vector::Copy( &start, pStartPos );
	}

	collGroup = CGRP_MASKGROUP_ATTACK;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED)
		collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	else
		collGroup = CGRP_MASKGROUP_CHARA_RED;

	// ��Ʈ��ũ�� �����ݴϴ�.
	NET_FireSync( &target );

	i3Vector::Sub( &dir, &target, &start);
	length = i3Vector::Normalize( &dir, &dir);

	i3Vector::Cross( &vXAxis, &dir, &I3_YAXIS);
	i3Vector::Normalize( &vXAxis, &vXAxis);

	i3Vector::Cross( &vYAxis, &dir, &vXAxis);
	i3Vector::Normalize( &vYAxis, &vYAxis);

	REAL32 deviation;

	// �������� ������ ���̸�, Zoom�� �� ���¿��� ������ 0�̶�� ������ �������� ����.
	// PSG1�� ���� ���簡 ������ �������� �ѿ� ���� ���Ƽ�� �ο��ϱ� ����.
	/*
	if( ( m_pWeaponInfo->m_TypeClass == WEAPON_CLASS_SNIPER) &&
		( m_pOwner->isWeaponStateMask( CHARA_STATE_ZOOM)) &&
		( m_pOwner->getFireDeviation() == m_pOwner->getLastDeviation()))
	{
		deviation = 0.0f;
	}
	else
	{
		// �Ϲ����� ����ġ(Deviation) ����.
		deviation = m_pOwner->getDeviationAngle();
	}
	*/

	if((m_pWeaponInfo->isEnableFastZoom()) && (isWeaponStateMask( WSM_FIRST_ZOOM)))
		deviation = 0.0f;
	else if (m_pOwner->IsSting())
	{
		deviation = m_pWeaponInfo->GetDeviation();
		//deviation = 15.0f;
	}
	else if (m_pOwner->IsCCSting() || m_pOwner->IsStingMars())
	{
		deviation = m_pWeaponInfo->GetDeviation();
	}
	else
		deviation = m_pOwner->getBoneContext()->getFireDeviation();

	if( m_pOwner->isAI() && m_pOwner->IsDino() == false)
	{
		REAL32 deviationRate = ((CInputCtrlAI_ForHuman*)m_pOwner->getInputContext()->getControl())->GetTargetRate();

		deviation = (1.0f + deviation) * deviationRate * 0.5f;

		deviation = MIN( deviation, 45.0f);
	}

	//I3TRACE("%f deviation %f in FireCollision \n", g_pFramework->GetAccTime(), deviation);

	m_bTestRoughCollision = false;
	for( i = 0; i < m_pWeaponInfo->GetFireBulletCount(); i++)
	{
		REAL32 r, t;
		VEC3D tTarget, tdir;
		
		REAL32 orgDeviation = deviation;

		r = i3Math::Randf2();

		if (!m_pOwner->IsDomiDino())
		{
			
			//	UseITEM ��Ȯ�� ���� (deviation ����)
			REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_ACCURACY);
			CASHITEM_TRACE("CASHITEMSKILL_ACCURACY = %f\n", BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_ACCURACY));
			deviation += (orgDeviation * ratio);

			////------------------	SKill ���߷� Apply
			const NET_CHARA_INFO * pCharaInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());

			if( pCharaInfo != nullptr)
			{
				REAL32 ratio_inSkill = pCharaInfo->getWeaponSkillValue( getWeaponInfo()->GetTypeClass(), SKILLLIST_ACCURACY);
				ratio_inSkill += (ratio_inSkill > 0 ? I3_EPS : -I3_EPS);
				deviation += ratio_inSkill;
			}
		}
		
		r *= deviation * 0.5f * 0.5f;
		t = i3Math::Randf2() * deviation * 0.5f * 0.5f;

		MATRIX tmp;
		i3Matrix::SetRotateAxisDeg( &tmp, &vXAxis, r);
		i3Matrix::PostRotateAxisDeg( &tmp, &vYAxis, t);

		i3Vector::TransformNormal( &tdir, &dir, &tmp);

		i3Vector::Scale( &tdir, &tdir, length);
		i3Vector::Add( &tTarget, &start, &tdir);

		DoLineCollision( &start, &tTarget, collGroup);
#if defined( TEST_COLLISION_LINE )
		{
			m_pVA->Lock();
			m_pVA->SetPosition(0, &start);
			m_pVA->SetPosition(1, &tTarget);
			m_pVA->Unlock();
		}
#endif
	}

	INT32 autoFireCount = getAutoFireCount();

	autoFireCount++;

	setAutoFireCount( autoFireCount);
}

I3_PHYSIX_HIT_RESULT * WeaponBase::_CollisionTestByHitBox( i3CollideeLine * pline, REAL32 rMinLength, VEC3D* pVec3)
{
	INT32 i;
	MATRIX mat;
	VEC3D * pShapeCenterPos;
	VEC3D vCenter;
	VEC3D vDir, vCamPos, vShapeDir;
	VEC3D vMovements;
	MATRIX camMtx;
	VEC4D vecRow1, vecRow2, vecRow3;

	I3_PHYSIX_HIT_RESULT * pResult = nullptr;
	if( m_pOwner == nullptr)
		return nullptr;
	
	m_pOwner->getCameraContext()->getVirtualCamPosition( &vCamPos, &camMtx );

	

	i3Vector::Normalize( &vDir, pline->GetDir());
	if(pVec3 == nullptr)
	{
		REAL32 rRange = GetRange();
		i3Vector::Scale( &vMovements, &vDir, rRange * 0.5f);
		m_HitBox.extents.x = rRange * 0.5f;
		m_HitBox.extents.y = 0.01f;
		m_HitBox.extents.z = rRange * 0.5f;
	}
	else 
	{
		i3Vector::Scale( &vMovements, &vDir, pVec3->z);
		m_HitBox.extents.x = pVec3->x;
		m_HitBox.extents.y = pVec3->y;
		m_HitBox.extents.z = pVec3->z;
	}
	
	i3Vector::Add( &vCenter, pline->GetStart(), &vMovements);
	
	i3Matrix::GetRow( &camMtx, 0, &vecRow1);
	i3Matrix::GetRow( &camMtx, 1, &vecRow2);
	i3Matrix::GetRow( &camMtx, 2, &vecRow3);

	NxVec3 row1( i3Vector::GetX( &vecRow1), i3Vector::GetY( &vecRow1), i3Vector::GetZ( &vecRow1));
	NxVec3 row2( i3Vector::GetX( &vecRow2), i3Vector::GetY( &vecRow2), i3Vector::GetZ( &vecRow2));
	NxVec3 row3( i3Vector::GetX( &vecRow3), i3Vector::GetY( &vecRow3), i3Vector::GetZ( &vecRow3));
	
	m_HitBox.center.x = getX( &vCenter);
	m_HitBox.center.y = getY( &vCenter);
	m_HitBox.center.z = getZ( &vCenter);

	m_HitBox.rot.setRow( 0, row1);
	m_HitBox.rot.setRow( 1, row2);
	m_HitBox.rot.setRow( 2, row3);

	INT32 collisionGroup = CGRP_TEAM_RED;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED )
		collisionGroup = CGRP_TEAM_BLUE;
	
	NxScene * pNxScene = g_pPhysixCtx->getScene();

	REAL32 rMaxDot = 0.9f;

	NxShape * pShapeList[ 32];

	INT32 nOverlapCount = pNxScene->overlapOBBShapes( m_HitBox, NX_DYNAMIC_SHAPES, 32, pShapeList, nullptr, (1<<collisionGroup) );

	for( i = 0; i < nOverlapCount; ++i)
	{
		if( pShapeList[ i]->userData == nullptr)	continue;

		if( !i3::same_of<i3PhysixShapeSet*>( (i3EventReceiver*)pShapeList[ i]->userData) )
			continue;

		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*)pShapeList[ i]->userData;
		if(!i3::kind_of<CGameCharaBase*>(pShapeSet->getEventReceiver()))
		{
			continue;	
		}

		if( pShapeSet->getShapeGroup() != collisionGroup)
			continue;

		pShapeSet->getShapeGlobalPose( &mat);

		pShapeCenterPos = i3Matrix::GetPos( &mat);
		i3Vector::Sub( &vShapeDir, pShapeCenterPos, &vCamPos);
		REAL32 rLen = i3Vector::Normalize( &vShapeDir, &vShapeDir);

		if( rLen < rMinLength )
		{
			REAL32 rDot = i3Vector::Dot( &vDir, &vShapeDir);
			
			if( rMaxDot < rDot)
			{
				if(IsThereWallBetween(vCamPos, *pShapeCenterPos, rMinLength) == false)
				{
					pResult = &m_LastHitResult;
					i3Vector::Copy( &pResult->m_Intersect, pShapeCenterPos);
					i3Vector::Scale( &pResult->m_Normal, i3Matrix::GetAt( m_pOwner->getBoneContext()->getHeadMatrix()), -1.f );
					pResult->m_pDestShapeSet = pShapeSet;
					pResult->m_pHitShape = pShapeSet->getNxShape( 0);
					pResult->m_T = rLen;

					rMaxDot = rDot;
				}
			}
		}
	}

	if( pResult != nullptr)
		_OnHit( pResult, pline, false);
	

	return pResult;
}

bool WeaponBase::IsThereWallBetween(VEC3D& v3StartPos, VEC3D& v3EndPos, REAL32 rLength)
{
	i3CollideeLine ColLine;

	ColLine.SetStart(&v3StartPos);
	ColLine.SetEnd(&v3EndPos);

	//World�� ���ؼ� �ѹ� �׽�Ʈ
	// ���� �ø��� �Ÿ����� ������.
	return _CheckHitWorld( ColLine, rLength );
}

I3_PHYSIX_HIT_RESULT * WeaponBase::_DoFireCollisionKnife( i3CollideeLine * pline, INT32 idxGroup, bool *bOwnerWarn)
{
	I3_PHYSIX_HIT_RESULT *	pResult, * pLastResult = nullptr;
	UINT32			nResultCount, nLastResultCount, nRoughResultCount;
	REAL32			rMinLength = GetRange();

	if( m_pOwner == nullptr )
		return nullptr;

	m_rPenetrateLength = rMinLength;

	nResultCount = nLastResultCount = nRoughResultCount = 0;

	m_pHitWorld = m_pHitObject = nullptr;
	// ���� ���� ���� ī��Ʈ�� �ʱ�ȭ �ؾ��մϴ�.
	g_pFramework->reserveRaycastResult( 0);

	//World & object
	pResult = g_pFramework->raycastClosest( pline, CGRP_MASKGROUP_ATTACK);
	if( pResult != nullptr )
	{
		nResultCount = 1;
		g_pFramework->reserveRaycastResult( nResultCount );

		if( pResult->m_T < rMinLength )
		{
			switch( pResult->m_pDestShapeSet->getShapeGroup() )
			{
			case CGRP_WORLD_ATTACK :
				{
					m_pHitWorld = pResult;
					rMinLength = pResult->m_T;
				}
				break;
			case CGRP_OBJECT_ALL :
			case CGRP_OBJECT_HIT :
			case CGRP_OBJECT_WHOLE :
			case CGRP_OBJECT_WHOLE_DYNAMIC :
				{
					m_pHitObject = pResult;
					rMinLength = pResult->m_T;
				}
				break;
			case CGRP_ITEM_HIT :
				{
					if( _OnHit( pResult, pline, false ) )
					{
						pLastResult = pResult;
						return pLastResult;
					}
				}
				break;
			}
		}
	}

	INT32 RoughCollGroup = CGRP_MASKGROUP_CHARA_ROUGH_BLUE;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_BLUE)
	{
		RoughCollGroup = CGRP_MASKGROUP_CHARA_ROUGH_RED;
	}

	// ĳ���� �ø���
	SetHitGroup( false );
	pResult = g_pFramework->raycastClosest( pline, idxGroup );

	i3EventReceiver * pLastEvent = nullptr;

	if( pResult )
	{
		nLastResultCount = 1;
		g_pFramework->reserveRaycastResult( nResultCount + nLastResultCount + nRoughResultCount);
		
		if( pResult->m_T < rMinLength )
		{	
			pLastEvent = pResult->m_pDestShapeSet->getEventReceiver();

			const char * pszName = pResult->m_pDestShapeSet->GetName();

			if( CheckHitPart( pszName ) )
			{
				rMinLength = pResult->m_T;
				if( _OnHit( pResult, pline, false ) )
				{
					pLastResult = pResult;
				}
			}
		}
	}
	// ������ �浹 ������ ���� �ʾ����� �ڽ��� ó�� �Ѵ�.
	if(pLastEvent == nullptr)
	{
		VEC3D v3Temp;
		REAL32 rTemp = 0.f;

		i3Vector::Set(&v3Temp, 0.f, 0.f, 0.f);
		if(i3::same_of<CMeleeWeaponInfo*>(m_pWeaponInfo))
		{
			((CMeleeWeaponInfo*)m_pWeaponInfo)->GetCollisionBoxExtent(&v3Temp);
		}
		rTemp = v3Temp.x + v3Temp.y + v3Temp.z;

		// Extent�� �ִ� �� ���� ó���� �Ѵ�.
		//WEAPON::RES_ID num = m_pWeaponInfo->GetRealNumber();
		if(rTemp > 0)
		{
			rMinLength = v3Temp.z * 2; // v3Temp.z �� Extent�̰� �ڽ��� �Ѻ��� ���̴� Extent * 2 �̴�.
			pLastResult = _CollisionTestByHitBox( pline, rMinLength, &v3Temp);
		}
		else
		{
			bool bProcess = false;
			if( (m_FireOrder == WEAPON::PRIMARY_FIRE) ||
				(i3::same_of<WeaponDualKnife*>(this) && getAutoFireCount() < 2) )
				bProcess = true;

			if( m_FireOrder == WEAPON::SECONDARY_FIRE)
			{
				if( i3::same_of<WeaponKnife*>(this) &&
					((CMeleeWeaponInfo*)m_pWeaponInfo)->IsSpraySecAttack())
					bProcess = true;

				if( i3::same_of<WeaponGunWithKnife*>(this))
					bProcess = true;
			}

			if( bProcess)
				pLastResult = _CollisionTestByHitBox( pline, rMinLength);
		}
		
	}
	
	if( pLastResult == nullptr )
	{
		// world hit effect
		if( m_pHitWorld != nullptr )
		{
			if( _OnHit( m_pHitWorld, pline, false ) )
			{
				pLastResult = m_pHitWorld;
			}
		}

		if( m_pHitObject != nullptr )
		{
			_OnHit( m_pHitObject, pline, false );
			if( m_pHitWorld == nullptr )
				pLastResult = m_pHitObject;
		}

		if( m_pHitItem != nullptr )
		{
			_OnHit( m_pHitItem, pline, false );
			if( m_pHitItem == nullptr )
				pLastResult = m_pHitItem;
		}

	}

	g_pFramework->resetRaycastResult();

	return pLastResult;
}

void WeaponBase::_CalcPhysixForce( i3PhysixShapeSet * pShapeSet, VEC3D * pHitDir, VEC3D * pHitPos, REAL32 rForce, REAL32 rLength)
{
	I3ASSERT( pShapeSet != nullptr);
	I3ASSERT( pHitDir != nullptr);
	I3ASSERT( g_pPhysixInfoDataBase != nullptr);

	MATRIX mat;
	VEC3D vForce, vTorque, vR;

	if( pShapeSet->getShapeGroup() == CGRP_OBJECT_HIT)
	{
		// ��Ʈ�ø����� ���ʹ� �ùķ��̼��� ���� �ʽ��ϴ�.
		// ���� ��忡�� �ùķ��̼��� �ϴ� i3Transform2�� ã�Ƽ� ��������� �մϴ�.

		I3ASSERT( i3::kind_of<i3Object*>(pShapeSet->getEventReceiver()));
			
		i3Object * pObj = (i3Object*) pShapeSet->getEventReceiver();
		i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( pObj->getSceneObject(), i3Transform2::static_meta() );
		if( pTrans != nullptr)
			pShapeSet = pTrans->getShapeSet();
	}

	// �������� ����մϴ�.
	PHYSIXINFODATA_WEAPON * pPhysixData = &g_pPhysixInfoDataBase->getInfo()->m_WeaponObject;

	switch( m_pWeaponInfo->GetTypeClass() )
	{
	case WEAPON_CLASS_KNIFE				:		
	case WEAPON_CLASS_KNUCKLE			:	rForce *= pPhysixData->m_rWeapon_Knife; 	break;
	case WEAPON_CLASS_HANDGUN			:	rForce *= pPhysixData->m_rWeapon_Pistol;	break;
	case WEAPON_SUBCLASS_ASSAULT		:
	case WEAPON_CLASS_ASSAULT			:	rForce *= pPhysixData->m_rWeapon_Assult;	break;
	case WEAPON_SUBCLASS_BOW			:	
	case WEAPON_CLASS_BOW				:	rForce *= pPhysixData->m_rWeapon_Assult;	break;
	case WEAPON_SUBCLASS_SMG			:
	case WEAPON_CLASS_SMG				:	rForce *= pPhysixData->m_rWeapon_SMG;		break;
	case WEAPON_SUBCLASS_SNIPER			:
	case WEAPON_CLASS_SNIPER			:	rForce *= pPhysixData->m_rWeapon_Rifle;		break;
	case WEAPON_SUBCLASS_SHOTGUN		:
	case WEAPON_CLASS_SHOTGUN			:		
	case WEAPON_CLASS_DUALSHOTGUN		:	rForce *= pPhysixData->m_rWeapon_Shotgun;	break;
	case WEAPON_CLASS_THROWING_GRENADE	:
	case WEAPON_CLASS_MISSION			:	rForce *= pPhysixData->m_rWeapon_Grenade;	break;
	case WEAPON_CLASS_MG				:	rForce *= pPhysixData->m_rWeapon_Assult;	break;
	case WEAPON_CLASS_DUALKNIFE			:	rForce *= pPhysixData->m_rWeapon_Knife;		break;
	case WEAPON_CLASS_DUALHANDGUN		:
	case WEAPON_CLASS_CIC				:	rForce *= pPhysixData->m_rWeapon_Pistol;	break;
	case WEAPON_CLASS_GRENADESHELL		:	rForce *= pPhysixData->m_rWeapon_Grenade;	break;
	case WEAPON_SUBCLASS_G_LAUNCHER		:
	case WEAPON_CLASS_ROCKET_LAUNCHER	:	rForce *= pPhysixData->m_rWeapon_Grenade;	break;
	case WEAPON_CLASS_OBJECT_GUN		:	rForce *= pPhysixData->m_rWeapon_Assult;	break;
	default :		
		// ������~!!!!!!!!!!!!
		I3TRACE( "Invalid Weapon class type: %d", m_pWeaponInfo->GetTypeClass());
		rForce *= pPhysixData->m_rWeapon_Grenade;
		break;
	}

	I3ASSERT( m_pWeaponInfo != nullptr);

	if( m_pWeaponInfo->isThrowingClass() )
	{
		REAL32 rExplosionRange = getWeaponInfo()->GetExplosion_Range();
		I3ASSERT( rExplosionRange > I3_EPS);
		rLength = (rExplosionRange - rLength + 0.5f) / rExplosionRange;

		rForce *= MINMAX( 0.f, rLength, 1.f);
	}
	
	// �̵����� ���մϴ�.
	i3Vector::Scale( &vForce, pHitDir, rForce * g_pPhysixInfoDataBase->getInfo()->m_rObjectLinSlop );
	pShapeSet->addForce( &vForce);

	// ȸ������ ���մϴ�.
	pShapeSet->getShapeGlobalPose( &mat );
	i3Vector::Sub( &vR, pHitPos, i3Matrix::GetPos( &mat ) );
	i3Vector::Cross( &vTorque, &vR, &vForce );
	i3Vector::Scale( &vTorque, &vTorque, g_pPhysixInfoDataBase->getInfo()->m_rObjectAngSlop );
	pShapeSet->addTorque( &vTorque);
}

void WeaponBase::_CalcPhysixForce( I3_PHYSIX_HIT_RESULT * pResult, VEC3D * pHitDir, REAL32 rDamage)
{
	_CalcPhysixForce( pResult->m_pDestShapeSet, pHitDir, &pResult->m_Intersect, rDamage, 0.f);
}
void WeaponBase::PlayAnimation_AttackReady(void)
{
	if (m_pOwner->is1PV())
	{
		if( getAIIndex( WEAPON::AI_ATTACKREADY_1PV) != -1 )
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_ATTACKREADY_1PV) );
	}
	else
	{
		if( getAIIndex( WEAPON::AI_ATTACKREADY_3PV) != -1 )
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_ATTACKREADY_3PV) );
	}
}

void WeaponBase::PlayAnimation_JumpEnd( void)
{
	if( getAIIndex( WEAPON::AI_JUMPEND) != -1 )
		WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_JUMPEND) );
}

void WeaponBase::PlayAnimation_Damage( void)
{
	if( getAIIndex( WEAPON::AI_DAMAGE) != -1)
		WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_DAMAGE));
}

void WeaponBase::LoadMagazine( void)
{
	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()))
	{
		if( getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT)
			addWeaponStateMask( WSM_FIRE_DISABLE);
	}

	OnPlayLoadMagAnimation();

	m_mainWeapon->LoadMagazine();
}

void WeaponBase::LoadBullet(void)
{
	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()))
	{
		if( getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT)
			addWeaponStateMask( WSM_FIRE_DISABLE);
	}

	OnPlayLoadBulletAnimation();

	setWeaponState( WEAPON::STATE_LOADBULLET);
}

void WeaponBase::WeaponChange( bool bPlaySound /*= true*/)
{
	I3ASSERT_RETURN(m_pOwner != nullptr);

	CHARA_VIEW_TYPE type = CHARA_VIEW_1PV;


	if( m_pOwner->isLocal() || m_pOwner->isAI())
		addWeaponStateMask( WSM_FIRE_DISABLE);

	type = m_pOwner->getViewType();
	
	getSceneObject()->GetBody()->StopAnim();

	//�̰� �߰� ���ϸ� ���� �ӵ� ���� ���� ���� �� ����� ü���� �ִϸ��̼� ��ü �ִϸ��̼� ���� �������ϴ�.
	REAL32 rTimeScale = _CalcSwapTimeScaleToVersion();

	OnPlayChangeAni( rTimeScale);

	if( bPlaySound )
		OnPlayChangeSound();
}

void		WeaponBase::OnPlayChangeAni( REAL32 rTimeScale )	// �񰡻��Լ��� ����� �����Լ��� �������� ó��..
{
	m_mainWeapon->OnPlayChangeAni(rTimeScale);
}

void		WeaponBase::OnPlayChangeAniImpl_Default( REAL32 rTimeScale )	// �̰͵� �ϴ� �񰡻����� ���� �д�..
{
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
	{
		if( getAIIndex( WEAPON::AI_IDLE_EXTENSION) != -1 )
		{
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_IDLE_EXTENSION), rTimeScale );
			return;
		}
	}

	if( m_pOwner != nullptr && g_pFramework->IsBattleStage())
	{
		if( m_pOwner->is1PV() && (getAIIndex( WEAPON::AI_CHANGE1PV) != -1) )		
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_CHANGE1PV), rTimeScale );
			m_pOwner->Cmd_Only_ChangeWeaponAction();	// revision 35847  �ΰ��ӿ� ó�� ������ �� �ѱ⸦ ������ �ִϸ��̼��� ����� ���׼���
			return;
		}
		else if( m_pOwner->is3PV() && (getAIIndex( WEAPON::AI_CHANGE3PV) != -1) )	
		{	
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_CHANGE3PV), rTimeScale );
			return;
		}
		else if( getAIIndex( WEAPON::AI_CHANGE) != -1)
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_CHANGE), rTimeScale);
			return;
		}else if( getAIIndex(WEAPON::AI_IDLE_FOLDED_CHANGE) != -1)
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_IDLE_FOLDED_CHANGE), rTimeScale);
			return;
		}
	}	

	_OnPlayWeaponIdleAnim();
}

void		WeaponBase::OnPlayChangeAniImpl( REAL32 rTimeScale )
{
	WeaponBase::OnPlayChangeAniImpl_Default(rTimeScale);
}

/* virtual  void WeaponBase::OnPlayChangeAnimation( REAL32 rTimeScale  = 1.f )
{
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
	{
		if( getAIIndex( WEAPON::AI_IDLE_EXTENSION) != -1 )
		{
			_PlayWeaponAnim( getAIIndex( WEAPON::AI_IDLE_EXTENSION), rTimeScale );
			return;
		}
	}

	if( m_pOwner != nullptr && g_pFramework->IsBattleStage())
	{
		if( m_pOwner->is1PV() && (getAIIndex( WEAPON::AI_CHANGE1PV) != -1) )		
		{
			_PlayWeaponAnim( getAIIndex( WEAPON::AI_CHANGE1PV), rTimeScale );
			return;
		}
		else if( m_pOwner->is3PV() && (getAIIndex( WEAPON::AI_CHANGE3PV) != -1) )	
		{	
			_PlayWeaponAnim( getAIIndex( WEAPON::AI_CHANGE3PV), rTimeScale );
			return;
		}
		else if( getAIIndex( WEAPON::AI_CHANGE) != -1)
		{
			_PlayWeaponAnim( getAIIndex( WEAPON::AI_CHANGE), rTimeScale);
			return;
		}
	}	

	_OnPlayWeaponIdleAnim();
}
*/

/*virtual*/ void WeaponBase::OnPlayChangeSound()
{
	// ���� ��ü ����
	_PlaySwapSound();
}

void WeaponBase::SetGachaBrightness(void)
{
	if( m_pMaterialAttr != nullptr )
	{
		COLORREAL col;

		REAL32 rBrightness = m_pWeaponInfo->GetWeaponBrightness();
		i3Color::Set( &col, rBrightness, rBrightness, rBrightness, 1.0f);
		m_pMaterialAttr->SetDiffuse( &col);
		m_pMaterialAttr->SetSpecular( &col);
		m_pMaterialAttr->SetShininess( 64.8f);		

		OnSetBlendAlpha( 1.0f);
	}
}

bool WeaponBase::ExtensionActivate( bool bVal, bool bAnimPlay)
{
	if( OnCheckExtensionPossible( bVal) )
	{
		setExtensionActivate( bVal);	// revision 42335 ��ġ ���� SS1-R5 �ͽ��ټ� ��ȯ�� �ѱ� �ִϸ��̼� �ȹٲ�� ���� ���� (redmine.5406)
		OnPlayExtensionAnimation( bVal, bAnimPlay);
		OnProcessExtension( bVal);

		//	���� Extension Ȱ��ȭ / ��Ȱ��ȭ �߿��� ����� �߻� ���Ѵ�.	
		if( (m_pOwner != nullptr) && (m_pOwner->isLocal() || m_pOwner->isAI()))
		{
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}
		return true;
	}

	return false;
}

static INT32 s_WeaponCount = 0;

static void s_Weapon_SetShapePosition( WeaponBase * pWeapon )
{
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)s_WeaponCount - 1000.f , 12000.f, (REAL32)s_WeaponCount - 1000.f );
	pWeapon->SetPos( &vec );
	s_WeaponCount++;
}

void WeaponBase::ReturnInstance()
{

	m_mainWeapon->ReturnInstance();

	s_Weapon_SetShapePosition( this );

	_StopFireSound();
	_StopSwapSound();
	_StopIdleSound();

	for( INT32 i = 0; i < WEAPON::FIRE_SOUND_COUNT; i++)
	{
		//g_pSndMng->ReleaseSoundFromPlayInfo( m_pFireSoundPlayInfo[ i]);
		I3_SAFE_RELEASE( m_pFireSoundPlayInfo[ i]);
	}

	//g_pSndMng->ReleaseSoundFromPlayInfo( m_pSwapSoundPlayInfo);
	I3_SAFE_RELEASE( m_pSwapSoundPlayInfo);
	I3_SAFE_RELEASE( m_pIdleSoundPlayInfo);

	getAttachNode()->RemoveAllChild();

	RemoveAllChild();	

	DetachNode();		//	GameNode , SceneNode ����

	_OnInitVairable();

	I3_SAFE_RELEASE( m_pFXLastTrans );

	I3_SAFE_RELEASE( m_pSgInfo);
	m_mainWeapon.reset();
	
	//2014-07-24 �̸� ���Ĺ̼��� ����¡ ���� �ذ�. v2���� ����
	//ENG_��뿵
//	if( GetRefCount() > 1)
//		i3Chara::ReturnInstance();

	i3Chara::ReturnInstance();
}

void WeaponBase::ReturnToReady( void)
{
	if( m_pWeaponInfo == nullptr )
		return;

	//	 ������ Ready ���·� ���ư��� ���� �ʿ��մϴ�. (��: ���� �߿� ���⸦ ��ü�ϸ�...)
	getSceneObject()->RemoveFlag( I3_NODEFLAG_DISABLE );

	OnSetVisibleExtension( true );

	_OnPlayWeaponIdleAnim();

	// ���� ó���Դϴ�..
	// ������, ���źâ�� ���� ���ҿ��� �ͽ��ټ� ���¸� �״�� �����մϴ�.
	bool bNoExtensionOff = false;

	switch( m_pWeaponInfo->GetTypeClass() )
	{
	case WEAPON_CLASS_SMG:
		{
			// KrissSuperV ��ŵ
			if( i3::same_of<WeaponKrissSuperV*>(this))
				bNoExtensionOff = true;
		}		
		break;
	case WEAPON_CLASS_SHOTGUN :
		{
			if( i3::same_of<WeaponShotGunGenderWithDual*>(this) )
				bNoExtensionOff = true;
		}
		break;
	case WEAPON_CLASS_DUALSMG :
	case WEAPON_CLASS_DUALSHOTGUN : 
		{
			bNoExtensionOff = true;			// DualSMG, DualShotGun�� ��ŵ
		}
		break;
	case WEAPON_CLASS_KNIFE :
		{
			bNoExtensionOff = m_pWeaponInfo->isNoOffExtAtChange();
		}
		break;
	case WEAPON_CLASS_KNUCKLE :
		break;
	}
	

	if( (GetCurrentExtensionType() != WEAPON::EXT_SILENCER) &&
		(GetCurrentExtensionType() != WEAPON::EXT_DUAL_MAG) &&
		!i3::same_of<WeaponGunWithKnife*>(this) &&
		!bNoExtensionOff )
	{
		ExtensionActivate( false, false);
		
		// Ŀ���� M4 ��� �ͽ��ټ� ��� Ȱ��ȭ�� ���� ����� �ͽ��ټ��� ����.
		if( isMultipleExtension())
		{				
			m_pWeaponInfo = m_pAbsoluteWeaponInfo;		//	����� �ֹ��� ������ �ٽ� ��ȯ

			m_CurrentExtensionType = m_pWeaponInfo->getExtensionType(0); //	ù��° �ͽ��ټ����� �ʱ�ȭ ����. �߰��� �ٲ�� �ִ� ��
		}	
	}

	// state clear	
	setWeaponState( WEAPON::STATE_READY);
	removeWeaponStateMask( WSM_PLAYED_FIRE_SOUND | 0xFF);
	
	//���� ��� ���� �ʱ�ȭ.
	m_timeAutoFire = 0.0f;
	setAutoFireCount(0);
	setRemainAutoFireCount(0);
	removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE | WSM_POINTSHOTING);

	//	�⺻���� �����տ� �⵵��
	if( getOwner() != nullptr)
		getOwner()->getWeaponContext()->setAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND);	// revision 45937 - ���� ��ü �� ���� �����տ� �⵵�� ����
	ChangeGunDummy( WEAPON_ATTACH_GRIP);

	// xgkick_thai
	m_tDropInfo.ReturnToReady();

	if( i3::same_of<WeaponDualKnife*>(this)
		&& m_pFireSoundPlayState != nullptr)
	{
		m_pFireSoundPlayState->Stop();
		m_pFireSoundPlayState = nullptr;
	}

	_StopSwapSound();
	_StopIdleSound();
	_RemoveIdleEffect();
///
	m_mainWeapon->ReturnToReady();
///

}

////////////////////////////////////////////////////////////////////////////////////////
//

// ���� �Ѿ˿� ���� Collision Test�� �ƴ϶�, ���� �� ��ǥ ������ ���ϱ� ����
// Collision Test�� �Ѵ�.
// �̶�, Character�� �浹 �׽�Ʈ ��� ���Ե��� ������, World���� �������
// �ؾ��Ѵ�.
void WeaponBase::GetShotTargetPos( VEC3D * pDest, VEC3D * pStart)
{
	MATRIX mtx;
	VEC3D pos;
	VEC3D at, *pFXPos;
	REAL32	fRange = 400.f;

	if( m_pOwner == nullptr )
		return;
	
	if( m_idxFXBone == -1)	//	FX�� ���� ������ ���
		pFXPos = pStart;
	else
		pFXPos = i3Matrix::GetPos( getFXMatrix());

	if( m_pWeaponInfo->GetTypeFire() == WEAPON::FIRE_MELEE )
		fRange = 400.f;
	else
		fRange = GetRange();

	m_pOwner->getCameraContext()->getVirtualCamPosition( &pos, &mtx);
	i3Vector::Scale( &at, i3Matrix::GetAt( &mtx), - fRange);

	i3Vector::Add( pDest, &at, &pos);


	if( m_pOwner->is1PV() == false)
	{
		i3CollideeLine line;

		line.SetStart( i3Matrix::GetPos( g_pCamera->getCamMatrix()));
		line.SetEnd( pDest);

		I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_WORLD_ATTACK );
		if( pResult != nullptr )
		{
			i3Vector::Sub( &at, &pResult->m_Intersect, pFXPos);
			i3Vector::Normalize( &at, &at);
			i3Vector::Scale( pDest, &at, GetRange());
			i3Vector::Add( pDest, pDest, pFXPos);
		}
	}
}

WEAPON_BULLET_HIT_STATE WeaponBase::_Raycast( i3CollideeLine * pLine, REAL32 & rMinLength, UINT32 & nWorldHitCount, UINT32 & nPenetrateCount)
{
	i3CollideeLine Line;

	//World & object
	//bool bPenetrate = false;
	WEAPON_BULLET_HIT_STATE iHitState = WEAPON_BULLET_HIT_STATE_HIT;

	I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( pLine, CGRP_MASKGROUP_ATTACK);

	if( pResult == nullptr)			return WEAPON_BULLET_HIT_STATE_NO_HIT;
	if( rMinLength < pResult->m_T)	return WEAPON_BULLET_HIT_STATE_LESSRANGE;

	m_bHitWorld = false;

	nWorldHitCount = 1;
	g_pFramework->reserveRaycastResult( nWorldHitCount);

	// ���� �׽�Ʈ
	REAL32 rLen = _CheckWorldHitLength( pResult, rMinLength );

	if( rLen == rMinLength )
	{	// ���� �Ǿ����ϴ�.
		//bPenetrate = true;

		VEC3D vAdd;
		i3Vector::Normalize( &vAdd, pLine->GetDir() );
		i3Vector::Scale( &vAdd, &vAdd, 0.01f );
		i3Vector::Add( &vAdd, &pResult->m_Intersect, &vAdd );
		Line.SetStart( &vAdd );
		Line.SetEnd( pLine->GetEnd() );

		iHitState = WEAPON_BULLET_HIT_STATE_PENETRATE;

		// ����� �Ÿ�
		m_rPenetrateLength = pResult->m_T;
		switch( pResult->m_pDestShapeSet->getShapeGroup() )
		{
		case CGRP_WORLD_ATTACK :			m_pHitWorldPenetrate = pResult;		break;
		case CGRP_OBJECT_ALL :
		case CGRP_OBJECT_HIT :
		case CGRP_OBJECT_WHOLE :
		case CGRP_OBJECT_WHOLE_DYNAMIC :	m_pHitObjectPenetrate = pResult;	break;
		}

		// ����� ������ �ٽ� üũ�մϴ�.
		pResult = g_pFramework->raycastClosest( &Line, CGRP_MASKGROUP_ATTACK);
		if( pResult )
		{
			nPenetrateCount = 1;
			g_pFramework->reserveRaycastResult( nWorldHitCount + nPenetrateCount );

			switch( pResult->m_pDestShapeSet->getShapeGroup() )
			{
			case CGRP_WORLD_ATTACK :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_bHitWorld = true;
				}
				m_pHitWorld = pResult;
				break;
			case CGRP_OBJECT_ALL :
			case CGRP_OBJECT_HIT :
			case CGRP_OBJECT_WHOLE :
			case CGRP_OBJECT_WHOLE_DYNAMIC :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_bHitWorld = true;
				}
				m_pHitObject = pResult;
				break;
			case CGRP_ITEM_HIT :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_pHitItem = pResult;
				}
				break;
			}
		}
	}
	else
	{
		m_bHitWorld = true;
		rMinLength = rLen;
		switch( pResult->m_pDestShapeSet->getShapeGroup() )
		{
		case CGRP_WORLD_ATTACK :			m_pHitWorld = pResult;	break;
		case CGRP_OBJECT_ALL :
		case CGRP_OBJECT_HIT :
		case CGRP_OBJECT_WHOLE :
		case CGRP_OBJECT_WHOLE_DYNAMIC :	m_pHitObject = pResult;	break;
		case CGRP_ITEM_HIT :				m_pHitItem = pResult;	break;
		}
	}

	//return bPenetrate;
	return iHitState;
}

I3_PHYSIX_HIT_RESULT * WeaponBase::DoLineCollision2( i3CollideeLine * pline, INT32 idxGroup, bool *bOwnerWarn)
{
	VEC3D	WarningIntersectPos;
	I3_PHYSIX_HIT_RESULT *	pResult, * pLastResult = nullptr;
	UINT32			nResultCount, nPenetResultCount, nLastResultCount, nRoughResultCount, i;
	REAL32			rTestRange = GetRange();
	REAL32			rMinLength = rTestRange;
	WEAPON_BULLET_HIT_STATE	iHitState = WEAPON_BULLET_HIT_STATE_NONE;

	i3CollideeLine	Line;

	if( m_pOwner == nullptr )
		return nullptr;

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	m_rPenetrateLength = rMinLength;

	nResultCount = nPenetResultCount = nLastResultCount = nRoughResultCount = 0;

	m_pHitWorld = m_pHitWorldPenetrate = m_pHitObject = m_pHitObjectPenetrate = m_pHitItem = nullptr;

	iHitState = _Raycast( pline, rMinLength, nResultCount, nPenetResultCount);

	// Error ����
	if( iHitState == WEAPON_BULLET_HIT_STATE_NONE)
		return nullptr;

	if( rMinLength == rTestRange)
	{	// Hit�Ÿ��� ��Ÿ���� ���� ���� �����Դϴ�.
		if( (iHitState != WEAPON_BULLET_HIT_STATE_NO_HIT) && (iHitState != WEAPON_BULLET_HIT_STATE_LESSRANGE) &&
			(iHitState != WEAPON_BULLET_HIT_STATE_PENETRATE))
		{
			// ���� ���� ���, ���ڶ�� ���, ������ �Ǿ����� ���� ���� �Ÿ��� �ƴ� ��� �ܿ��� �߸��� ����
			return nullptr;
		}
	}

	// Warning Sound�� ���Ǿ� üũ
	if( m_bTestRoughCollision == false)
	{
		INT32 RoughCollGroup = CGRP_MASKGROUP_CHARA_ROUGH_BLUE;
		if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_BLUE)
			RoughCollGroup = CGRP_MASKGROUP_CHARA_ROUGH_RED;

		nRoughResultCount = g_pFramework->raycast( pline, RoughCollGroup );
		if( nRoughResultCount > 0)
		{
			//�ø��� �׽�Ʈ�� ���� �����ϰԵǸ� Result���� ������Ƿ�, �׽�Ʈ�� ������ receiver�� �̸� ������ �д�.
			I3_PHYSIX_HIT_RESULT * pRoughResult = g_pFramework->getRaycastResult();

			g_pFramework->reserveRaycastResult( nResultCount + nPenetResultCount + nRoughResultCount );
			
			for( i = 0; i < nRoughResultCount; ++i)
			{
				i3EventReceiver * pRecv = (i3EventReceiver*)pRoughResult[i].m_pDestShapeSet->getEventReceiver();

				if( (pRecv == pPlayer) && (rMinLength > pRoughResult[ i].m_T))
				{
					*bOwnerWarn = true;
					i3Vector::Copy( &WarningIntersectPos, &pRoughResult[i].m_Intersect);
					break;
				}
			}
		}
		m_bTestRoughCollision = true;
	}

	// ĳ���� �ø���
	i3EventReceiver * pLastEvent = nullptr;
	i3EventReceiver * pTempEvent[128] = {nullptr, };

	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER )
	{
		// ���� ���� 
		nLastResultCount = g_pFramework->raycast( pline, idxGroup );
		if( nLastResultCount > 0 )
		{
			pResult = g_pFramework->getRaycastResult();
			g_pFramework->reserveRaycastResult( nResultCount + nPenetResultCount + nLastResultCount + nRoughResultCount);

			// ���������� ���� ���� - 2013.02.22 ���õ
			struct cmpByDamageRate
			{
				bool operator()( const I3_PHYSIX_HIT_RESULT& p1, const I3_PHYSIX_HIT_RESULT& p2)
				{
					CGameCharaBase * chara1 = i3::kind_cast<CGameCharaBase*>(p1.m_pDestShapeSet->getEventReceiver());
					CGameCharaBase * chara2 = i3::kind_cast<CGameCharaBase*>(p2.m_pDestShapeSet->getEventReceiver());

					if(chara1 != nullptr && chara2 != nullptr)
					{
						CHARA_HIT_PART ePart1 = chara1->getCollisionContext()->GetHitPart( p1.m_pDestShapeSet);
						CHARA_HIT_PART ePart2 = chara2->getCollisionContext()->GetHitPart( p2.m_pDestShapeSet);

						// TRex�̰� ���� ���� ���� return true�� ����� ��. RU.638
						bool is_hit_trex_eye1 = check_trex_hit_eye(chara1, ePart1);
						bool is_hit_trex_eye2 = check_trex_hit_eye(chara2, ePart2);

						if( is_hit_trex_eye1 || is_hit_trex_eye2 )
							return ( is_hit_trex_eye1 && !is_hit_trex_eye2 ) ? true : false;	// return (true && false) ? true : false;

						CHARA::eDAMAGEPART damagePart1 = CHARA::GetDamagePartByHit( ePart1);
						CHARA::eDAMAGEPART damagePart2 = CHARA::GetDamagePartByHit( ePart2);

						CCharaGlobal * pGlobalInfo = g_pCharaInfoDataBase->getGlobal();
						REAL32 rPart1 = chara1->getDBInfo()->GetPartDamageRate( damagePart1) * pGlobalInfo->GetDamageRateByMode( damagePart1);
						REAL32 rPart2 = chara2->getDBInfo()->GetPartDamageRate( damagePart2) * pGlobalInfo->GetDamageRateByMode( damagePart2);

						return  rPart1 > rPart2;
					}
					return ( chara1 == nullptr && chara2 != nullptr ) ? true : false; // return (nullptr && !nullptr) ? true : false;
				}

				bool check_trex_hit_eye( CGameCharaBase * pChara , CHARA_HIT_PART hit_part )
				{
					if( i3::same_of<CGameCharaDinoTRex*>(pChara) )
					{
						if( hit_part == CHARA_HIT_VITAL_EYE )
							return true;
					}

					return false;
				}
			};
			i3::sort( pResult, pResult + nLastResultCount, cmpByDamageRate());


			for( i = 0; i < nLastResultCount; i++ )
			{
				bool isEvent = false;

				if( rMinLength < pResult[ i ].m_T )
					continue;

				for(UINT32 j = 0; j < i; j++)
				{
					if( pTempEvent[j] == pResult[ i ].m_pDestShapeSet->getEventReceiver())
					{
						//���ݼ������� TRex ��� ��, ���� ��ο찡 �Ͼ�� �ʴ� ����.
						//TRex�� ������ ����Ǿ ���� �����Ƿ�, ���� ����Ǿ������� Ȯ���ϸ� �ȴ�.
						CGameCharaBase * tempChara = i3::kind_cast<CGameCharaBase*>(pResult[ i ].m_pDestShapeSet->getEventReceiver());
						if(!tempChara->IsRex())
						{
							isEvent = true;
						}
					}
				}

				if(isEvent == true)
					continue;

				// �´� ������ �ٸ��ٸ� �����Ǻ��� �������ݴϴ�.
				if( pLastEvent != pResult[ i ].m_pDestShapeSet->getEventReceiver() )
				{
					SetHitGroup( false );
					pLastEvent = pResult[ i ].m_pDestShapeSet->getEventReceiver();
				}

				//GlobalFunc::PB_TRACE(" i %d GetName %s", i, pResult[ i ].m_pDestShapeSet->GetName());
				// ���� �������� üũ�մϴ�.
				// ���� ������� ��ŵ
				if( CheckHitPart( pResult[ i ].m_pDestShapeSet->GetName() ) )
				{
					if( _OnHit( &pResult[ i ], pline, false ) )
					{
						pLastResult = &pResult[ i ];
						pTempEvent[i] = pLastEvent;
					}
				}
			}
		}
	}
	else
	{	// �Ϲ� ����
		pResult = g_pFramework->raycastClosest( pline, idxGroup );
		if( pResult != nullptr )
		{
			nLastResultCount = 1;
			g_pFramework->reserveRaycastResult( nResultCount + nPenetResultCount + nLastResultCount + nRoughResultCount);

			if( rMinLength > pResult->m_T)
			{
				_OnHit( pResult, pline, false );
				pLastResult = pResult;
				rMinLength = pResult->m_T;
			}
		}
	}
	
	if( pLastResult == nullptr )
	{
		// world hit effect
		if( m_pHitWorld != nullptr && m_bHitWorld)
		{
			if( _OnHit( m_pHitWorld, pline, false ) )
				pLastResult = m_pHitWorld;
		}

		if( m_pHitObject != nullptr && m_bHitWorld)
		{
			if( _OnHit( m_pHitObject, pline, false ))
				pLastResult = m_pHitObject;
		}

		if( m_pHitItem != nullptr)
		{
			if( _OnHit( m_pHitItem, pline, false))
				pLastResult = m_pHitItem;
		}
	}

	// ������ �Ǿ��ٸ� �޸鵵 üũ
	if( iHitState == WEAPON_BULLET_HIT_STATE_PENETRATE )
	{
		if( m_pHitWorldPenetrate != nullptr )
		{
			_OnHit( m_pHitWorldPenetrate, pline, false );
			if( pLastResult == nullptr)
				pLastResult = m_pHitWorldPenetrate;
		}
		if( m_pHitObjectPenetrate != nullptr )
		{
			_OnHit( m_pHitObjectPenetrate, pline, false );
			if( pLastResult == nullptr)
				pLastResult = m_pHitObjectPenetrate;
		}

		i3CollideeLine BackLine;
		VEC3D	vAdd, vDir;
		i3Vector::Normalize( &vDir, pline->GetDir() );
		i3Vector::Scale( &vAdd, &vDir, -0.01f );

		if( m_pHitWorld != nullptr)		i3Vector::Add( &vAdd, &m_pHitWorld->m_Intersect, &vAdd );
		else if( m_pHitObject != nullptr)	i3Vector::Add( &vAdd, &m_pHitObject->m_Intersect, &vAdd);
		else							i3Vector::Add( &vAdd, pline->GetEnd(), &vAdd);

		BackLine.SetStart( &vAdd );
		BackLine.SetEnd( pline->GetStart() );
		

		// revision 44306
		// 2012.09.07 �ڱ⼺
		// ��ƮŸ�� �ø��� ���� �ڵ� �ּ�ó��
		/*UINT32 nBackCount = 0;
		pResult = g_pFramework->raycastClosest( &BackLine, CGRP_MASKGROUP_ATTACK);
		if( pResult != nullptr )
		{
			nBackCount = 1;
			g_pFramework->reserveRaycastResult( nResultCount + nPenetResultCount + nLastResultCount + nRoughResultCount + nBackCount);
			_OnHit( pResult, &BackLine, false, false );
		}*/
	}

	if(*bOwnerWarn)
	{
		if (m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_DINO)
		{
			if( m_pWeaponInfo->GetTypeFire() == WEAPON::FIRE_RANGE)
			{
				g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_HIT_DINO_STING, i3Math::Rand()%3, &WarningIntersectPos, m_pOwner->is1PV());
			}
		}
		else if(m_pWeaponInfo->isGun())
			g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_GRAZE_BULLET, i3Math::Rand()%3, &WarningIntersectPos, m_pOwner->is1PV());
	}

	g_pFramework->resetRaycastResult();

	return pLastResult;
}

I3_PHYSIX_HIT_RESULT * WeaponBase::DoLineCollision3( i3CollideeLine * pline, INT32 idxGroup, bool *bOwnerWarn)
{	
	UINT32 i;

	VEC3D	WarningIntersectPos;
	I3_PHYSIX_HIT_RESULT *	pResult, * pLastResult = nullptr;
	UINT32			nResultCount, nPenetResultCount, nLastResultCount, nRoughResultCount;
	REAL32			rMinLength = GetRange();
	bool	bPenetrate = false;

	i3CollideeLine	Line;

	if( m_pOwner == nullptr )
		return nullptr;

	m_rPenetrateLength = rMinLength;

	nResultCount = nPenetResultCount = nLastResultCount = nRoughResultCount = 0;

	m_pHitWorld = m_pHitWorldPenetrate = m_pHitObject = m_pHitObjectPenetrate = m_pHitItem = nullptr;

	//bPenetrate = _Raycast( pline, rMinLength, nResultCount, nPenetResultCount);
	UINT32	nCollisionGroup = CGRP_MASKGROUP_ALLATTACK_BLUE;

	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_BLUE)
		nCollisionGroup = CGRP_MASKGROUP_ALLATTACK_RED;

	m_bHitWorld = false;

	//World & object
	UINT32 iHitCount = g_pFramework->raycast( pline, nCollisionGroup);

	if( iHitCount == 0)
		return nullptr;

	I3_PHYSIX_HIT_RESULT * HitChara[ 128];
	i3mem::FillZero( HitChara, sizeof( HitChara));

	UINT32 iHitCharaCount = 0;

	pResult = g_pFramework->getRaycastResult();
	g_pFramework->reserveRaycastResult( iHitCount);

	REAL32 rMyRoughDist = rMinLength;
	REAL32 rMinCharaDist = rMinLength;
	UINT32 iMinCharaIdx = 0;

	for( i = 0; i < iHitCount; i++)
	{
		// ���� �׽�Ʈ
		REAL32 rLen = rMinLength;
		switch( pResult[ i].m_pDestShapeSet->getShapeGroup() )
		{
		case CGRP_WORLD_ATTACK :
			if( rMinLength > pResult[ i].m_T)
			{
				m_bHitWorld	= true;
				if( m_pHitWorld == nullptr)						m_pHitWorld = &pResult[ i];
				else if( m_pHitWorld->m_T > pResult[ i].m_T)	m_pHitWorld = &pResult[ i];
				
				rLen = _CheckWorldHitLength( &pResult[ i], rMinLength );
				if( rLen == rMinLength )
				{	// ���� �Ǿ����ϴ�.
					if( m_rPenetrateLength > pResult[ i].m_T )
					{
						bPenetrate = true;
						VEC3D	vAdd;
						i3Vector::Normalize( &vAdd, pline->GetDir() );
						i3Vector::Scale( &vAdd, &vAdd, 0.01f );
						i3Vector::Add( &vAdd, &pResult[ i].m_Intersect, &vAdd );
						Line.SetStart( &vAdd );
						Line.SetEnd( pline->GetEnd() );

						// ����� �Ÿ�
						m_rPenetrateLength = pResult[ i].m_T;
						m_pHitWorldPenetrate = &pResult[ i];
						m_pHitObjectPenetrate = nullptr;
					}
				}
				else
				{
					m_pHitObject = nullptr;
					m_pHitItem = nullptr;
				}
				rMinLength = rLen;
			}
			break;

		case CGRP_OBJECT_ALL :
		case CGRP_OBJECT_HIT :
		case CGRP_OBJECT_WHOLE :
		case CGRP_OBJECT_WHOLE_DYNAMIC :
			if( rMinLength > pResult[ i].m_T)
			{
				if( m_pHitObject == nullptr)						m_pHitObject = &pResult[ i];
				else if( m_pHitObject->m_T > pResult[ i].m_T)	m_pHitObject = &pResult[ i];
				
				rLen = _CheckWorldHitLength( &pResult[ i], rMinLength );
				if( rLen == rMinLength )
				{	// ���� �Ǿ����ϴ�.
					if( m_rPenetrateLength > pResult[ i].m_T )
					{
						bPenetrate = true;
						VEC3D	vAdd;
						i3Vector::Normalize( &vAdd, pline->GetDir() );
						i3Vector::Scale( &vAdd, &vAdd, 0.01f );
						i3Vector::Add( &vAdd, &pResult[ i].m_Intersect, &vAdd );
						Line.SetStart( &vAdd );
						Line.SetEnd( pline->GetEnd() );

						m_rPenetrateLength = pResult[ i].m_T;
						m_pHitObjectPenetrate = &pResult[ i];
						m_pHitWorldPenetrate = nullptr;
					}
				}
				else
				{
					m_pHitWorld = nullptr;
					m_pHitItem = nullptr;
				}
				rMinLength = rLen;
			}
			break;

		case CGRP_ITEM_HIT :
			if( rMinLength > pResult[ i].m_T)
			{
				if( m_pHitItem == nullptr)						m_pHitItem = &pResult[ i];
				else if( m_pHitItem->m_T > pResult[ i].m_T)	m_pHitItem = &pResult[ i];

				rMinLength = pResult[ i].m_T;

				m_pHitWorld = nullptr;
				m_pHitObject = nullptr;
			}
			break;

		case CGRP_TEAM_BLUE_ROUGH :
		case CGRP_TEAM_RED_ROUGH :
			// Warning Sound�� ���Ǿ� üũ
			{
				i3EventReceiver * pRecv = (i3EventReceiver*)pResult[i].m_pDestShapeSet->getEventReceiver();

				CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

				if( (pRecv == pPlayer))
				{
					*bOwnerWarn = true;
					rMyRoughDist = pResult[ i].m_T;
					i3Vector::Copy( &WarningIntersectPos, &pResult[i].m_Intersect);
				}
			}
			break;
		case CGRP_TEAM_BLUE :
		case CGRP_TEAM_RED :

			if (iHitCharaCount < 128)
			{
				HitChara[iHitCharaCount] = &pResult[i];

				if (rMinCharaDist > pResult[i].m_T)
				{
					rMinCharaDist = rMinLength;
					iMinCharaIdx = iHitCharaCount;
				}

				iHitCharaCount++;

				if (iHitCharaCount >= 128)
				{
					I3PRINTLOG(I3LOG_FATAL, "chara hit buffer overflow.");
					// ������ ������, �̹� ǥ�ð� �Ǹ�, ��� �����°� �����̶��....�� ������ �������� iHitCharaCount�� �����ؾ��Ѵ�!
				}
			}
			break;
		}
	}


	if( bPenetrate )
	{	// ���� �Ǿ����ϴ�.
		// ����� ������ �ٽ� üũ�մϴ�.
		pResult = g_pFramework->raycastClosest( &Line, CGRP_MASKGROUP_ATTACK);
		if( pResult )
		{
			g_pFramework->reserveRaycastResult( iHitCount + 1 );

			switch( pResult->m_pDestShapeSet->getShapeGroup() )
			{
			case CGRP_WORLD_ATTACK :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_bHitWorld = true;
				}
				m_pHitWorld = pResult;
				break;
			case CGRP_OBJECT_ALL :
			case CGRP_OBJECT_HIT :
			case CGRP_OBJECT_WHOLE :
			case CGRP_OBJECT_WHOLE_DYNAMIC :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_bHitWorld = true;
				}
				m_pHitObject = pResult;
				break;
			case CGRP_ITEM_HIT :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_pHitItem = pResult;
				}
				break;
			}
		}
	}

	// ĳ���� �ø���
	i3EventReceiver * pLastEvent = nullptr;

	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER )
	{
		// ���� ���� 
		for( i = 0; i < iHitCharaCount; i++)
		{
			if( rMinLength < HitChara[ i ]->m_T )
				continue;

			// �´� ������ �ٸ��ٸ� �����Ǻ��� �������ݴϴ�.
			if( pLastEvent != HitChara[ i ]->m_pDestShapeSet->getEventReceiver() )
			{
				SetHitGroup( false );
				pLastEvent = HitChara[ i ]->m_pDestShapeSet->getEventReceiver();
			}

			// ���� �������� üũ�մϴ�.
			// ���� ������� ��ŵ
			if( CheckHitPart( HitChara[ i ]->m_pDestShapeSet->GetName() ) )
			{
				if( _OnHit( HitChara[ i ], pline, false ) )
					pLastResult = HitChara[ i ];
			}
		}
	}
	else
	{	// �Ϲ� ����
		if( iHitCharaCount > 0 )
		{
			if( rMinLength > HitChara[ iMinCharaIdx]->m_T)
			{
				_OnHit( HitChara[ iMinCharaIdx], pline, false );
				pLastResult = HitChara[ iMinCharaIdx];
			}
		}
	}
	
	
	if( pLastResult == nullptr )
	{
		// world hit effect
		if( m_pHitWorld != nullptr)
		{
			if( _OnHit( m_pHitWorld, pline, false ) )
				pLastResult = m_pHitWorld;
		}

		if( m_pHitObject != nullptr)
		{
			if( _OnHit( m_pHitObject, pline, false ))
				pLastResult = m_pHitObject;
		}

		if( m_pHitItem != nullptr)
		{
			if( _OnHit( m_pHitItem, pline, false))
				pLastResult = m_pHitItem;
		}
	}

	// ������ �Ǿ��ٸ� �޸鵵 üũ
	if( bPenetrate )
	{
		if( m_pHitWorldPenetrate != nullptr )		_OnHit( m_pHitWorldPenetrate, pline, false );
		if( m_pHitObjectPenetrate != nullptr )		_OnHit( m_pHitObjectPenetrate, pline, false );

		i3CollideeLine BackLine;
		VEC3D	vAdd, vDir;
		i3Vector::Normalize( &vDir, pline->GetDir() );
		i3Vector::Scale( &vAdd, &vDir, -0.01f );

		if( m_pHitWorld != nullptr)		i3Vector::Add( &vAdd, &m_pHitWorld->m_Intersect, &vAdd );
		else if( m_pHitObject != nullptr)	i3Vector::Add( &vAdd, &m_pHitObject->m_Intersect, &vAdd);

		BackLine.SetStart( &vAdd );
		BackLine.SetEnd( pline->GetStart() );
		
		UINT32 nBackCount = 0;
		pResult = g_pFramework->raycastClosest( &BackLine, CGRP_MASKGROUP_ATTACK);
		if( pResult != nullptr )
		{
			nBackCount = 1;
			g_pFramework->reserveRaycastResult( nResultCount + nPenetResultCount + nLastResultCount + nRoughResultCount + nBackCount);
			_OnHit( pResult, &BackLine, false, false );
		}
	}

	if( rMyRoughDist > rMinLength )
		*bOwnerWarn = false;

	if( *bOwnerWarn)
	{
		if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_DINO )
		{
			if( getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_RANGE)
				g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_HIT_DINO_STING, i3Math::Rand()%3, &WarningIntersectPos, m_pOwner->is1PV());
		}
		else if(m_pWeaponInfo->isGun())
			g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_GRAZE_BULLET, i3Math::Rand()%3, &WarningIntersectPos, m_pOwner->is1PV());
	}

	g_pFramework->resetRaycastResult();

	return pLastResult;
}

bool WeaponBase::CheckHitPart( const char * pszPart)
{
	bool bRet = false;

	switch( pszPart[0])
	{
		case 'H':
			if( pszPart[1] == 'E')
			{
				if( !m_bHitGroup[HIT_CHARA_PART_VITAL])
				{
					m_bHitGroup[HIT_CHARA_PART_VITAL] = true;
					bRet = true;
				}
			}
			else if( pszPart[1] == '_')			// Head
			{
				if( pszPart[ 5 ] != 'H' )
				{
					// Hit!
					if( !m_bHitGroup[HIT_CHARA_PART_HEAD])
					{
						m_bHitGroup[HIT_CHARA_PART_HEAD] = true;
						bRet = true;
					}
				}
			}	
			else if( pszPart[1] == 'J')		// Arm
			{
				// Hit!
				if( !m_bHitGroup[HIT_CHARA_PART_HEAD])
				{
					m_bHitGroup[HIT_CHARA_PART_HEAD] = true;
					bRet = true;
				}
			}
			else if( pszPart[1] == 'N')		// Arm
			{
				// Hit!
				if( !m_bHitGroup[HIT_CHARA_PART_ARM])
				{
					m_bHitGroup[HIT_CHARA_PART_ARM] = true;
					bRet = true;
				}
			}
			else if( pszPart[1] == 'u')
			{
				// Hull_Helmet
				if( pszPart[5] == 'H' )
				{
					if( !m_bHitGroup[HIT_CHARA_PART_HELMET])
					{
						m_bHitGroup[HIT_CHARA_PART_HELMET] = true;
						//bRet = true;
						bRet = false;	// ����� ��Ʈ ����� ���� �ʽ��ϴ�. �Ӹ��� ���� �¾ƾ� �մϴ�.
					}
				}
				else if( !m_bHitGroup[HIT_CHARA_PART_WEAPON])	// Weapon
				{					
					m_bHitGroup[HIT_CHARA_PART_WEAPON] = true;
					bRet = false;
				}
			}
			else if( pszPart[1] == 'e')		// Helmet
			{
				if( !m_bHitGroup[HIT_CHARA_PART_HELMET])
				{
					m_bHitGroup[HIT_CHARA_PART_HELMET] = true;
					bRet = false;	// ����� ��Ʈ ����� ���� �ʽ��ϴ�. �Ӹ��� ���� �¾ƾ� �մϴ�.
				}
			}
			break;

		// Body
		case 'T':		// ���� ������ �ٵ�� ó��
		case 'B':
			if( !m_bHitGroup[HIT_CHARA_PART_VITAL])
			{
				if( pszPart[2] == 'N')
				{
					if( pszPart[8] == 'L')
					{
						m_bHitGroup[ HIT_CHARA_PART_VITAL] = true;
						bRet = true;
					}
				}
			}

			// GIRAN NPC
			if( !m_bHitGroup[ HIT_CHARA_PART_HEAD])
			{
				if( pszPart[ 1] == 'i' && pszPart[ 6] == 'H')
				{
					m_bHitGroup[ HIT_CHARA_PART_HEAD] = true;
					bRet = true;
				}
			}
			// ����
			if( pszPart[8] == 'L') {	// B_Neck0_Lower
				if( !m_bHitGroup[HIT_CHARA_PART_NECK]) {
					m_bHitGroup[HIT_CHARA_PART_NECK] = true;
					bRet = true;
				}
			}

            if( !bRet && !m_bHitGroup[HIT_CHARA_PART_BODY])
			{	// body
				m_bHitGroup[HIT_CHARA_PART_BODY] = true;
				bRet = true;
			}
			
			break;

		// Arm
		case 'A':
			// Hit!
			if( !m_bHitGroup[HIT_CHARA_PART_ARM])
			{
				m_bHitGroup[HIT_CHARA_PART_ARM] = true;
				bRet = true;
			}
			break;

		// Leg
		case 'L':
		case 'F':
			// Hit!
			if( !m_bHitGroup[HIT_CHARA_PART_LEG])
			{
				m_bHitGroup[HIT_CHARA_PART_LEG] = true;
				bRet = true;
			}
			break;
		default:
			bRet = false;
			break;
	}

	return bRet;
}

void WeaponBase::SetHitGroup( bool bFlag)
{
	INT32 i = 0;

	for( i=0; i<HIT_CHARA_PART_COUNT; i++)
	{
		m_bHitGroup[i] = bFlag;
	}
}

/*
I3_TERRAIN_NA,							// Not-Assigned
I3_TERRAIN_CONCRETE = 1,				// ��ũ��Ʈ
I3_TERRAIN_STEEL,						// ö��
I3_TERRAIN_GROUND,						// ��
I3_TERRAIN_WOOD,						// ����
I3_TERRAIN_SNOW,						// ��
I3_TERRAIN_WATER_DEEP,					// �� (����)
I3_TERRAIN_WATER_SHALLOW,				// �� (����)
I3_TERRAIN_WET_CLAY,					// ����
I3_TERRAIN_GRASS,						// �ܵ�
I3_TERRAIN_MARBLE,						// �븮��
I3_TERRAIN_FALLLEAF,
I3_TERRAIN_CONCRETE_THIN,
I3_TERRAIN_STEEL_THIN,
I3_TERRAIN_WOOD_THIN,
I3_TERRAIN_MARBLE_THIN,
*/
bool WeaponBase::_CheckPenetration( I3_PHYSIX_HIT_RESULT * pResult, REAL32 rFireRange, REAL32 rWeaponRange )
{
	if( pResult->m_pTerrain == nullptr )
	{
		return false;
	}

	UINT16 nTerrainType = pResult->m_pTerrain->m_Terrain;

	if( nTerrainType < I3_TERRAIN_CONCRETE_THIN )
	{
		return false;
	}

	if( ((nTerrainType > I3_TERRAIN_MARBLE_THIN) &&	(nTerrainType < I3_TERRAIN_GROUND_THIN)) ||
		((nTerrainType > I3_TERRAIN_BARBEDWIRE) && (nTerrainType < I3_TERRAIN_CLOTH_NO_DECAL)) )
	{
		return false;
	}

	// �β��� �Ʒ����� ������ ������ ����
	if(pResult->m_pTerrain->m_Thickness <= 10)
	{
		return true;
	}

	INT32 iPenentratePercent = 0;
	CWeaponInfo * pWeaponInfo = getWeaponInfo();

#ifdef BUILD_RELEASE_QA_VERSION
	I3ASSERT( pWeaponInfo != nullptr);
#else
	if(pWeaponInfo == nullptr) return false;
#endif
	REAL32 Penetrate = GetWeaponPenetrate(false);
	REAL32 rTemp = Penetrate * pWeaponInfo->GetBulletWeight() * rWeaponRange;

	if(rTemp > I3_EPS)
	{
		iPenentratePercent = (INT32)(100 - (( pResult->m_pTerrain->m_Thickness * rFireRange) / rTemp));
		if(i3Math::Rand() % 100 <= iPenentratePercent)
		{
			return true;
		}
	}

	return false;



	// ����Ǿ����ϴ�
	// return true;
}


bool WeaponBase::_CheckHit( i3CollideeLine& ColLine, REAL32 rLength, INT32 HitType )
{
	UINT32	nCount	= g_pFramework->raycast( &ColLine, HitType );
	if( nCount > 0 )
	{
		I3_PHYSIX_HIT_RESULT*	pResult	= g_pFramework->getRaycastResult();
		for( UINT32 i = 0; i < nCount; i++ )
		{
			if( rLength > pResult[i].m_T )
				rLength = _CheckWorldHitLength( &pResult[i], rLength );
		}

		if( rLength <= i3Vector::Length( ColLine.GetDir()) )
			return true;
	}
	return false;
}

bool WeaponBase::_CheckHitWorld( i3CollideeLine& ColLine, REAL32 rLength )
{	// revision 45500
	UINT32	nCount	= g_pFramework->raycast( &ColLine, CGRP_MASKGROUP_ENVIRONMENT );
	if( nCount > 0 )
	{
		I3_PHYSIX_HIT_RESULT*	pResult	= g_pFramework->getRaycastResult();
		for( UINT32 i = 0; i < nCount; i++ )
		{
			if( rLength > pResult[i].m_T )
			{
				i3EventReceiver * pReceiver = pResult->m_pDestShapeSet->getEventReceiver();
				//��� ������Ʈ�� ���Ĺ��� ����.
				if(i3::same_of<CGameObjectRespawnHelicopter*>(pReceiver) ||
				   i3::same_of<CGameObjectRidingHelicopter*>(pReceiver))
				{
					continue;
				}

				rLength = _CheckWorldHitLength( &pResult[i], rLength );
			}
		}

		if( rLength < i3Vector::Length( ColLine.GetDir()) )
			return false;
	}

	return true;
}

REAL32 WeaponBase::_CheckWorldHitLength( I3_PHYSIX_HIT_RESULT * pResult, REAL32	rLength )
{
	if( getWeaponInfo()->isNotPenetrate())
	{// ������ �ȵ˴ϴ�.
	}
	else if( _CheckPenetration( pResult, pResult->m_T,  rLength) )
	{
		return rLength;
	}

	// ������ �ȵǸ� ����� ��Ʈ ���������� �Ÿ��� ���Ͻ����ݴϴ�
	if( pResult->m_T < rLength )
		rLength = pResult->m_T;

	return rLength;
}

struct FORCE_INFO
{
	i3Object *	_pObj;
	VEC3D *		_pImpulse;
	VEC3D *		_pIntersect;
	REAL32		_rTorque;
	i3PhysixShapeSet * _pHitShape;
} ;

static I3SG_TRAVERSAL_RESULT _SetLinearForceProc( i3Node * pNode, void * pData, i3MatrixStack * pStack)
{
	i3PhysixShapeSet * pShape;
	MATRIX	mat;
	VEC3D	vR, vTorque;
	FORCE_INFO * pInfo = (FORCE_INFO *) pData;

	if( !i3::same_of<i3Transform2*>(pNode))
		return I3SG_TRAVERSAL_CONTINUE;

	i3Transform2 * pTrans2 = (i3Transform2 *) pNode;

	pShape = pTrans2->getShapeSet();
	I3ASSERT( pShape != nullptr );

	if( pInfo->_pHitShape != nullptr && pShape != pInfo->_pHitShape )
		return I3SG_TRAVERSAL_CONTINUE;

	pShape->getShapeGlobalPose( &mat );
	i3Vector::Sub( &vR, pInfo->_pIntersect, i3Matrix::GetPos( &mat ) );
	i3Vector::Cross( &vTorque, &vR, pInfo->_pImpulse );
	pShape->addForce( pInfo->_pImpulse);

	i3Vector::Scale( &vTorque, &vTorque, pInfo->_rTorque );
	pShape->addTorque( &vTorque);

	return I3SG_TRAVERSAL_CONTINUE;
}

WEAPON_CLASS_TYPE WeaponBase::_GetDinoTerrainHitWeaponClass( bool & bAddSound)
{
	WEAPON_CLASS_TYPE weaponClass = WEAPON_CLASS_UNKNOWN;
	EDinoAttackType attackType = ((CGameCharaDino*)m_pOwner)->GetNowDinoAttackType();

	if(m_pOwner->IsRaptor() || m_pOwner->IsElite() ||
		m_pOwner->IsCCRaptor() || m_pOwner->IsRaptorMercury()) 
	{
		if( attackType == DAT_RAPTOR_ATTACK_A)
		{
			if( getAutoFireCount() >= m_pWeaponInfo->GetAutoFireCount() - 1 )
			{
				weaponClass = WEAPON_CLASS_DINO_LEFT_SCRATCH;
			}
			else 
			{
				weaponClass = m_pWeaponInfo->GetTypeClass();
			}
		}
		else if( attackType == DAT_ELITE_SCRATCH) 
		{	
			if( getAutoFireCount() >= m_pWeaponInfo->GetAutoFireCount() - 1 )
			{
				weaponClass = WEAPON_CLASS_ELITE_LEFT_SCRATCH;
			}
			else 
			{
				weaponClass = WEAPON_CLASS_ELITE_RIGHT_SCRATCH;
			}
		}
		else if( attackType == DAT_RAPTOR_ATTACK_B) 
		{
			weaponClass = m_pWeaponInfo->GetTypeClass();
		}
		else if( attackType == DAT_ELITE_HOWL)
		{
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL, "invalid Dino attackType : %d", attackType);
		}
	}
	else if (m_pOwner->IsRex())
	{
		if( attackType == DAT_REX_ATTACK1) 
		{
			weaponClass = WEAPON_CLASS_TREX;
			bAddSound = false;
		}
	}
	else if (m_pOwner->IsSting())
	{
		if( attackType == DAT_STING_ATTACK1 || attackType == DAT_STING_ATTACK2) 
		{
			weaponClass = m_pWeaponInfo->GetTypeClass();
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL, "invalid Dino attackType : %d", attackType);
		}
	}
	else if (m_pOwner->IsTank())
	{
	}
	else if (m_pOwner->IsCCSting() || m_pOwner->IsStingMars())
	{
		if( attackType == DAT_STING_ATTACK1)
		{
			weaponClass = m_pWeaponInfo->GetTypeClass();
		}
		else if( attackType == DAT_STING_ATTACK2) 
		{
			weaponClass = WEAPON_CLASS_STING;
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL, "invalid Dino attackType : %d", attackType);
		}
	}
	else if (m_pOwner->IsDomiSting() || m_pOwner->IsDomiStingMars())
	{
		weaponClass = WEAPON_CLASS_STING;
	}

	return weaponClass;
}

void WeaponBase::_CalcHitWorld( I3_PHYSIX_HIT_RESULT * pResult, VEC3D * pDir)
{	
	if (m_pOwner == nullptr) return;
	if (m_pOwner->getBoneContext() == nullptr) return;

	I3_PHYSIX_TERRAIN * pTerrain = pResult->m_pTerrain;

	//I3TRACE( "HIT_WORLD +++++++++++++++++ TERRAIN_TYPE : %02d\n" , (pResult->m_pTerrain->m_Terrain) );

	I3ASSERT( pTerrain != nullptr );

	//	ĳ���� �þ߸� �߽����� �ϴ� ź�� ����Ʈ�� ȸ���ؾ� �ϴ� ���� ���Ѵ�. komet
	REAL32 fRotateRadVal = 0.f;
	REAL32	fFactor = i3Vector::Dot( &pResult->m_Normal, i3Matrix::GetUp( m_pOwner->GetMatrix() ) ); //&I3_YAXIS );
	
	if( fFactor > 0.001f )
	{
		fRotateRadVal = m_pOwner->getBoneContext()->getTheta() * fFactor;
	}
	else if( fFactor < -0.001f)
	{
		if( fFactor > -1.f )
		{
			fRotateRadVal = m_pOwner->getBoneContext()->getTheta() * (fFactor) + I3_PI;
		}
		else
		{
			fRotateRadVal = m_pOwner->getBoneContext()->getTheta() * fFactor - I3_PI2;
		}
	}

	bool bAddSound = true;
	WEAPON_CLASS_TYPE weaponClass = WEAPON_CLASS_UNKNOWN;

	if( m_pOwner->IsDino())
	{
		weaponClass = _GetDinoTerrainHitWeaponClass( bAddSound);
	}

	if( weaponClass != WEAPON_CLASS_UNKNOWN)
	{
		g_pEffectManager->AddTerrainEffect( (I3_TERRAIN_TYPE) pTerrain->m_Terrain, weaponClass, 
									static_cast<BULLET_EFFECT_TYPE>(m_pWeaponInfo->GetLoadBulletType()),
									&pResult->m_Intersect, &pResult->m_Normal, pDir, fRotateRadVal );
	}
	else
	{
		WEAPON_TERRAIN_EFFECT_INFO tEffectInfo;
		tEffectInfo._terrainType	= (I3_TERRAIN_TYPE) pTerrain->m_Terrain;
		tEffectInfo._pPos			= &pResult->m_Intersect;
		tEffectInfo._pNormal		= &pResult->m_Normal;
		tEffectInfo._pDir			= pDir;
		tEffectInfo._rRotate		= fRotateRadVal;
		tEffectInfo._pObj			= nullptr;

		_OnAddTerrainEffect( &tEffectInfo);
	}
	

	REAL32 rDelay = 0.f;

	if( m_pOwner->is1PV())
	{
		VEC3D vDir, vPos;
		m_pOwner->getCameraContext()->getVirtualCamPosition( &vPos);
		i3Vector::Sub( &vDir, &pResult->m_Intersect, &vPos );
		rDelay = i3Vector::Length( &vDir) * 0.02f;
		if( rDelay > 0.1f)
			rDelay = 0.1f;
	}

	if( bAddSound == true)
	{
		_AddTerrainEffectSound( (I3_TERRAIN_TYPE) pTerrain->m_Terrain, rDelay, &pResult->m_Intersect);
	}
}

void WeaponBase::_CalcHitObject( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine, VEC3D * pDir, REAL32 rLength, bool bDamage )
{
	I3ASSERT( pResult != nullptr);

	if( !g_pFramework->IsBattleStage() )
		return;

	if( m_pOwner == nullptr)
		return;

	I3ASSERT( pResult->m_pDestShapeSet != nullptr);

	VEC3D vHitPos;
	I3_PHYSIX_HIT_RESULT * pLastResult = (I3_PHYSIX_HIT_RESULT *)i3MemAlloc(sizeof(I3_PHYSIX_HIT_RESULT));

	i3Object * pObject = (i3Object*) pResult->m_pDestShapeSet->getEventReceiver();
	// ������Ʈ�� RandomDamage ������
	REAL32	rDamage = GetDamage( false);
	I3_PHYSIX_HIT_RESULT * pPenetrate = nullptr;

	if( pObject == nullptr )
	{
		I3PRINTLOG(I3LOG_WARN,  "������Ʈ�� �����~" );
		return;
	}

	if( i3::same_of<CGameObjectHatchling*>(pObject))
	{	// ����� ������ Damage ����
		((CGameObjectHatchling*)pObject)->SetControledObjectState( 1);
	}
	else if( i3::same_of<CGameObjectRidingHelicopter*>(pObject))
	{	// OutPost ���� ���߿� �� ��쿡�� �������� �޽��ϴ�.
		bDamage = (((CGameObjectRidingHelicopter*)pObject)->isEnableDamage() == true);
		if( i3::same_of<WeaponM197*>(this))
		{
			bDamage = false;
		}
	}

	if( bDamage)
	{
		// Wallshot
		if( m_rPenetrateLength < pResult->m_T )
		{ // ����� �� �ڿ� �ִ� ���
			if( m_pHitWorldPenetrate != nullptr && m_pHitWorldPenetrate->m_pTerrain != nullptr )
				pPenetrate = m_pHitWorldPenetrate;
			else if( m_pHitObjectPenetrate != nullptr && m_pHitObjectPenetrate->m_pTerrain != nullptr )
				pPenetrate = m_pHitObjectPenetrate;

			if( pPenetrate != nullptr )
			{
		#if 0
				rDamage	*= (1.f - (REAL32)pPenetrate->m_pTerrain->m_Thickness * 0.01f);
		#else
				rDamage *= 0.5f;
		#endif
			}
		}

		// �������� ������Ʈ�� ���� 1/2 �������� �ش�.
		if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_KNIFE || getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_KNUCKLE)
			rDamage *= 0.5f;

		// revision 62269 ������Ʈ ���� �ø��� üũ ���� �� �ı��̼� ��� ���� ����
		//�ø��� üũ �� ���� ����� ������� �� ������, memcpy�� ������ ������ �����Ѵ�.
		//�ø��� üũ�� ���� �� �ٽ� ����� ��������.
		memcpy(pLastResult, pResult, sizeof(I3_PHYSIX_HIT_RESULT));
		_CalcHitObjectDamage( pResult, pLine, rDamage);
		memcpy(pResult, pLastResult, sizeof(I3_PHYSIX_HIT_RESULT));
		I3MEM_SAFE_FREE(pLastResult);

		if( pObject->getDynamicState() == I3GAME_DYNAMIC_STATE_DYNAMIC )
		{	// Dynamic Object�� ���� �� ���
			if( BattleServerContext::i()->IsP2PHost() ||
				(pObject->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_NONE) ||		// ��Ʈ��ũ ��尡 ���� ����̰ų�
				(pObject->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_STATIC) )		// ����ƽ���� ���õ� ������Ʈ�� ��� ó��
				_CalcPhysixForce( pResult, pDir, rDamage);
		}
	}

	if( !getWeaponInfo()->isThrowingClass() )
	{
		I3_PHYSIX_TERRAIN * pTerrain = pResult->m_pTerrain;
		I3_TERRAIN_TYPE terrain = I3_TERRAIN_CONCRETE;

		if( pTerrain != nullptr)
		{
			terrain = (I3_TERRAIN_TYPE) pTerrain->m_Terrain;
		}

		REAL32 fRotateRadVal = 0.f;
		REAL32	fFactor = i3Vector::Dot( &pResult->m_Normal, &I3_YAXIS );
		REAL32 fTheta = m_pOwner->getBoneContext()->getTheta();
		
		i3Vector::Copy( &vHitPos, &pResult->m_Intersect);
		
		if( fFactor > 0.001f )
		{
			fRotateRadVal = fTheta * fFactor;
		}
		else if( fFactor < -0.001f)
		{
			if( fFactor > -1.f )
			{
				fRotateRadVal = fTheta - I3_PI2;
			}
			else
			{
				fRotateRadVal = fTheta * fFactor - I3_PI2;
			}
		}

		// revision 48169 ������Ʈ ����Ʈ�� ���� ����ó��
		bool bAddSound = true;
		WEAPON_CLASS_TYPE weaponClass = WEAPON_CLASS_UNKNOWN;

		if( m_pOwner->IsDino())
		{
			weaponClass = _GetDinoTerrainHitWeaponClass( bAddSound);
		}

		if( weaponClass != WEAPON_CLASS_UNKNOWN)
		{
			g_pEffectManager->AddTerrainEffect( terrain, weaponClass, 
				static_cast<BULLET_EFFECT_TYPE>(m_pWeaponInfo->GetLoadBulletType()),
				&vHitPos, &pResult->m_Normal, pDir, fRotateRadVal, pObject);
		}
		else
		{
			WEAPON_TERRAIN_EFFECT_INFO tEffectInfo;
			tEffectInfo._terrainType	= terrain;
			tEffectInfo._pPos			= &vHitPos;
			tEffectInfo._pNormal		= &pResult->m_Normal;
			tEffectInfo._pDir			= pDir;
			tEffectInfo._rRotate		= fRotateRadVal;
			tEffectInfo._pObj			= pObject;

			_OnAddTerrainEffect( &tEffectInfo);
		}

		REAL32 rDelay = 0.f;

		if( m_pOwner->is1PV() )
		{
			VEC3D vDir, vPos;
			m_pOwner->getCameraContext()->getVirtualCamPosition( &vPos);
			i3Vector::Sub( &vDir, &pResult->m_Intersect, &vPos );
			rDelay = i3Vector::Length( &vDir) * 0.02f;
			if( rDelay > 0.1f)
				rDelay = 0.1f;
		}

		if( bAddSound)
			_AddTerrainEffectSound( (I3_TERRAIN_TYPE) pTerrain->m_Terrain, rDelay, &pResult->m_Intersect);
	}
}

void WeaponBase::_CalcHitObjectDamage(  I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine,i3Object * pObject, i3PhysixShapeSet * pShapeSet, REAL32 rDamage)
{
	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	
	CHud * pHud = CHud::i();
	if( CHud::i() == nullptr)
		return;
	
	I3ASSERT( m_pWeaponInfo != nullptr);

	if( m_pOwner == nullptr)
		return;

	bool	bMissionObj = false;
	bool	bDamageTo = false;

	if( rDamage < 1.f)
		rDamage = 1.f;

	STAGE_MODE stageType = MyRoomInfoContext::i()->getStage()->GetStageMode();
	switch( stageType)
	{
	case STAGE_MODE_ESCAPE:
	case STAGE_MODE_DESTROY:
		{
			if( pObject == pMissionManager->getRedObject() )
			{
				bMissionObj = true;

				if( isDamageToMissionObject() &&
					(m_pOwner->getCharaTeam() & CHARACTER_TEAM_BLUE) )
					bDamageTo = true;
			}
			else if( pObject == pMissionManager->getBlueObject() )
			{
				bMissionObj = true;

				if( isDamageToMissionObject() &&
					(m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED) )
					bDamageTo = true;
			}
			else
				bDamageTo = true;
		}
		break;

	case STAGE_MODE_DEFENCE:
		{	// ����� Red team�� �����ϴ� �����Դϴ�.
			if( (pObject == pMissionManager->getDefenceObj( 0)) ||
				(pObject == pMissionManager->getDefenceObj( 1)) )
			{
				bMissionObj = true;

				if( isDamageToMissionObject() &&
					(m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED) )
					bDamageTo = true;
			}
			else
				bDamageTo = true;
		}
		break;
	default:
		bDamageTo = true;
		break;
	}

	if( BattleServerContext::i()->IsP2PHost() )
	{
		INT32 nTempState = pObject->getCurState();

		if( bMissionObj)
		{
			if( bDamageTo )
			{
				REAL32 rPreHP = pObject->getCurrentHP();
				pObject->Damage( rDamage, this, pShapeSet);
				//I3TRACE( "Obj damage by %s\n", this->getWeaponInfo()->GetName());

				if( pObject->getCurrentHP() != rPreHP)
				{
					pMissionManager->DamageObjectForUser( m_pOwner->getCharaNetIndex(), rDamage, pObject, (bool)(nTempState != pObject->getCurState()));

					//�������� ĳ���͸� ǥ��
					pHud->SetDetectedEnemyInfo( m_pOwner->getCharaNetIndex());
				}
			}
		}
		else
		{
			pObject->Damage( rDamage, this, pShapeSet);
			//I3TRACE( "Obj damage by %s\n", this->getWeaponInfo()->GetName());
		}
	}
	else
	{
		if( bMissionObj )
		{	// �ı� ������Ʈ�� �����ϴ� �ɸ��Ͱ� �����ΰ�� ǥ��
			//�������� �ɸ��͸� ǥ��
			if( bDamageTo && pObject->getCurrentHP() != 0.f)
			{
				pHud->SetDetectedEnemyInfo( m_pOwner->getCharaNetIndex());
			}
		}
		else if( (pObject->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_NONE) )
		{	// Client�� ��ũ��尡 ���� ������Ʈ�� ������ ���
			pObject->Damage( rDamage, this, pShapeSet);
			//I3TRACE( "Obj damage by %s\n", this->getWeaponInfo()->GetName());
		}
		
		if( BattleServerContext::i()->IsUseDedicationMode())
		{
			// ������Ʈ Ȯ��
			if( pObject->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_ANIM)
			{	// �ִϸ��̼� ������Ʈ
				if( !i3::kind_of<CGameControledObject*>(pObject)
					|| (((CGameControledObject*)pObject)->isManualUpdate() == false) )
				{	// ��Ʈ�� ���� �ʴ� ���� �������� ������ �ʽ��ϴ�.
					bDamageTo = false;
				}
			}

			// revision  47188 
			//��Ʈ ī���Ͱ� 10�� �̻� �Ǳ� ������ //Ʈ�� ������Ʈ ���� ��ŵ�ϴ�.
			INT32 isTreeObject  = -1;
			isTreeObject = i3::contain_string( pObject->getSgRes()->getSceneGraphInfo()->GetName(), "Christmas");

			if( bDamageTo && isTreeObject < 0 )
			{// ���� ��������
				if( getOwner()->isLocal() || getOwner()->isAI() )
				{	// �ڽ��� �������Դϴ�.
					// Network�� �������� ������.
					CHARA_DAMAGEINFO damageInfo;

					GetDamageInfo( &damageInfo);
					
					if( IsGrenade() )	// revision 39201
						damageInfo._DeathType = CHARA_DEATH_B;						

					damageInfo._HitType = GHT_OBJECT;
					damageInfo._nVictimIdx = pObject->getNetIndex();
					damageInfo._ObjNetType = pObject->getNetworkSyncMode();
					damageInfo._rDamage = rDamage;

					if (pLine != nullptr && pResult != nullptr)
					{
						i3Vector::Copy( &damageInfo._vStartPos, pLine->GetStart());
						i3Vector::Copy( &damageInfo._vHitPos, &pResult->m_Intersect);
#if (defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)) && 0
						I3TRACE("������Ʈ �ѱ����� x : %f  y : %f  z : %f  \n",damageInfo._vStartPos.x,damageInfo._vStartPos.y,damageInfo._vStartPos.z);
						I3TRACE("������Ʈ �ǰݹ��� x : %f  y : %f  z : %f  \n",damageInfo._vHitPos.x,damageInfo._vHitPos.y,damageInfo._vHitPos.z);
#endif
					}

					// revision 62135
					obj_explosion::i()->call_for_dedi(getOwner(), damageInfo, pObject);

					GAMEEVENT::Write_Chara_Hit( getOwner(), &damageInfo);
				}
			}
		}
	}
}

void WeaponBase::_CalcHitObjectDamage( I3_PHYSIX_HIT_RESULT * pResult,i3CollideeLine * pLine,REAL32 rDamage)
{
	if( g_pFramework->IsBattleStage() == false)
		return;

	i3Object * pObject = (i3Object*) pResult->m_pDestShapeSet->getEventReceiver();
	I3ASSERT( i3::kind_of<i3Object*>(pObject));

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		CStageBattle* pStage = g_pFramework->GetStageBattle();
		I3ASSERT(pStage);

		if(pStage != nullptr && gmode::i()->GetTutorialMode() != nullptr)
		{
			if( gmode::i()->GetTutorialMode()->IsAvailableTargetOnTutorialStateOnWeapon(pObject->getNetIndex(), this) == false)
			{
				return;
			}
		}
		else
		{
			I3ASSERT_0;
		}
	}

	_CalcHitObjectDamage( pResult, pLine, pObject,pResult->m_pDestShapeSet, rDamage);
}

void WeaponBase::_CalcHitChara( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine, VEC3D * pDir, REAL32 rLength)
{
	VEC3D vPos;
	i3PhysixShapeSet *	pShapeSet = pResult->m_pDestShapeSet;
	I3ASSERT( pShapeSet != nullptr);
	
	CGameCharaBase * pHitChara = nullptr;
	CGameCharaEquip * pEquip = nullptr;

	CHARA_DAMAGEINFO DamageInfo;

	i3EventReceiver * pReceiver = pShapeSet->getEventReceiver();

	if(pReceiver == nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE,"ShapeSet Receiver == nullptr");
		return;
	}

	if( i3::same_of<CGameCharaEquip*>(pReceiver))
	{
		pEquip = (CGameCharaEquip*) pReceiver;
		if( i3::same_of<CGameCharaBase*>(pEquip->GetOwner()))
			pHitChara = (CGameCharaBase*) pEquip->GetOwner();
	}
	else if( i3::kind_of<CGameCharaBase*>(pReceiver))
		pHitChara = (CGameCharaBase*) pReceiver;
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "�� �� ���� Hit." );
	}
	
	
	if( pHitChara != nullptr)
	{
		if( pHitChara->getCharaNetIndex() == -1)
			return;

		if( pHitChara->getCollisionContext() == nullptr)	// �� ������ ��� ���ü� �ִ�.
			return;

		if( !pHitChara->isAIToClient() && pHitChara->getCharaInfo()->GetType() != CHARA::TYPE_DOMI_FULLBODY_AI)
		{
			if( BattleSlotContext::i()->getSlotInfo( pHitChara->getCharaNetIndex())->_State != SLOT_STATE_BATTLE)
				return;
		}

		VEC3D vecDir = VEC3D( 0.f, 0.f, 0.f);
		
		GetDamageInfo( &DamageInfo);

		// GetDamageInfo �ȿ��� getAP()�� ó���ǰ� �ִ�.
		// ������ �ڵ�� �Ʒ� �ڵ��̱⿡ �ּ����� ���ܵ�.
		//DamageInfo._rDamage			= GetDamage();
		DamageInfo._nKillerType		= m_pOwner->getCharaInfo()->GetType();
		DamageInfo._pHitResult		= pResult;
		
		DamageInfo._HitType	= GHT_CHARA;

		// ���뷮
		if( m_rPenetrateLength < pResult->m_T )
		{ // ����� �� �ڿ� �ִ� ���
			if( (m_pHitWorldPenetrate != nullptr) && (m_pHitWorldPenetrate->m_pTerrain != nullptr) )
				DamageInfo._WallShotReduceRate = m_pHitWorldPenetrate->m_pTerrain->m_Thickness;
			else if( (m_pHitObjectPenetrate != nullptr) && (m_pHitObjectPenetrate->m_pTerrain != nullptr) )
				DamageInfo._WallShotReduceRate = m_pHitObjectPenetrate->m_pTerrain->m_Thickness;

			if( (DamageInfo._WallShotReduceRate < 0) || (DamageInfo._WallShotReduceRate > 100) )
			{
				I3TRACE( "Wall thickness is invalid..\n" );
				DamageInfo._WallShotReduceRate = 50;
			}
		}


		VEC3D diff;
		i3Vector::Sub( &diff, pHitChara->GetPos(), m_pOwner->GetPos());
		DamageInfo._rDistance = i3Vector::Length( &diff);


		if( g_pEnvSet->m_bEnableBlood )
		{
			VEC3D	vRand, vAdd;
			i3CollideeLine BloodLine;
			I3_PHYSIX_HIT_RESULT * pWallHitResultTemp = nullptr;
			REAL32 rBloodLength;

			if( m_pHitWorld != nullptr )
			{
				rBloodLength = m_pHitWorld->m_T - pResult->m_T;
				if( rBloodLength > 0.f && rBloodLength < BLOODEFFECT_HIT_LENGTH )
				{
					pWallHitResultTemp = m_pHitWorld;
					BloodLine.SetStart( pLine->GetStart() );
				}
			}
			if( m_pHitWorldPenetrate != nullptr )
			{	
				if( m_pHitWorldPenetrate->m_T < pResult->m_T )
				{	// ����� �� �ڿ� ������ ���������� �ٽ� �˻�
					i3Vector::Scale( &vAdd, &m_pHitWorldPenetrate->m_Normal, -0.01f );
					i3Vector::Add( &vAdd, &vAdd, &m_pHitWorldPenetrate->m_Intersect );
					BloodLine.SetStart( &vAdd );
				}
				else
				{	// ���� ���̶�� ����� ���� ��Ʈ �ø������� �Ѵ�.
					pWallHitResultTemp = m_pHitWorldPenetrate;
					BloodLine.SetStart( pLine->GetStart() );
				}
			}

			if( pWallHitResultTemp != nullptr )
			{
				// calc Random position
				i3Vector::Set( &vAdd, 1.f, 1.f, 1.f );
				i3Vector::Sub( &vAdd, &vAdd, &pWallHitResultTemp->m_Normal );
				i3Vector::Set( &vRand, (REAL32)((i3Math::Rand() % 100) - 50) * 0.001f, (REAL32)((i3Math::Rand() % 100) - 50) * 0.001f, (REAL32)((i3Math::Rand() % 100) - 50) * 0.001f );
				i3Vector::Mul( &vAdd, &vAdd, &vRand );
				i3Vector::Add( &vAdd, &vAdd, &pWallHitResultTemp->m_Intersect );
				BloodLine.SetEnd( &vAdd );

				UINT32 nResultCount = g_pFramework->raycast( &BloodLine, CGRP_MASKGROUP_ATTACK );
				if( nResultCount > 0 )
				{
					pWallHitResultTemp = g_pFramework->getRaycastResult();

					for( UINT32 i = 0; i < nResultCount; i++ )
					{
						if( pWallHitResultTemp[ i ].m_T - pResult->m_T < BLOODEFFECT_HIT_LENGTH )
							DamageInfo._pWallHitResult = &pWallHitResultTemp[ i ];
					}
				}
			}
		}

		CHARA_HIT_PART part = pHitChara->getCollisionContext()->GetHitPart( pResult->m_pDestShapeSet);
		if( part == CHARA_HIT_UNKNOWN)
			return;	// ��� ���� ���� ����

		if( m_pWeaponInfo->isExplosionPhysics())
		{	//	���߹��� ����
			DamageInfo._DeathType	= CHARA_DEATH_B;
		}
		else
		{
			//	��弦 ����				
			if( part == CHARA_HIT_HEAD)
				DamageInfo._DeathType = CHARA_DEATH_H;
			//	���� ����
			else
				DamageInfo._DeathType = CHARA_DEATH_A;	//	CHARA_DEATH_A �� �ӽ��̹Ƿ� �ٸ� Death ������ �ʿ��ϸ� ������ �ʿ��մϴ�.			
			
			if( m_pOwner != nullptr)
			{
				i3Vector::Set( &vPos, getX( m_pOwner->GetPos() ), getY( m_pOwner->GetPos() ) + m_pOwner->getCameraContext()->getIKHeightForCam(), getZ( m_pOwner->GetPos() ) );
			}

			DamageInfo._pVecPos		=  &vPos;
		}

		CalcHitDir( pResult, DamageInfo._pVecPos, &vecDir);
		DamageInfo._pVecDir		= &vecDir;
		DamageInfo.SetHitPart( part );
		DamageInfo._IsNPC = false;
		DamageInfo._nVictimIdx		= pHitChara->getCharaNetIndex();

		if(i3::same_of<WeaponThrowKnife*>(DamageInfo._pWeapon))
		{	
			//Throwing Knife �迭�� ������, �ǰ��� ��ġ�� ������.
			//���� Throwing Knife �迭 ���� ������ ������ ���, ���� ó���ϴ� ���� ������.
			i3Vector::Copy( &DamageInfo._vStartPos, DamageInfo._pVecPos);
			// ������ ��ü�� ���� ��� �ִ� ���⿡ ��� ���� extension�� ���� ������ �ν�
			DamageInfo._bExtension = false;
		}
		else
		{
			i3Vector::Copy( &DamageInfo._vStartPos, pLine->GetStart());
		}
		
		i3Vector::Copy( &DamageInfo._vHitPos, &pResult->m_Intersect);

		I3ASSERT( DamageInfo.isValid() );

		// ĳ������ HP �� ����Ʈ ó��
		pHitChara->OnHit( &DamageInfo, m_pOwner);
	}
	else if( pEquip != nullptr )
	{	// ��� ��ü�� ���� ���(���� ���)
		pShapeSet->setSleeping( false );

		VEC3D vLin, vTorque, vRoll;
		MATRIX mat;
		i3Vector::Normalize( &vLin, pLine->GetDir() );
		i3Vector::Scale( &vLin, &vLin, getAP() * 0.5f );
		
		pShapeSet->getShapeGlobalPose( &mat );
		i3Vector::Sub( &vRoll, &pResult->m_Intersect, i3Matrix::GetPos( &mat ) );
		i3Vector::Cross( &vTorque, &vRoll, &vLin );
		pShapeSet->setLinearVelocity( &vLin );

		pShapeSet->setAngularVelocity( &vTorque );

		g_pEffectManager->AddEquipHitEffect( m_pWeaponInfo->GetTypeClass(),	
			static_cast<BULLET_EFFECT_TYPE>(m_pWeaponInfo->GetLoadBulletType()),
			&pResult->m_Intersect, &pResult->m_Normal, m_pOwner->is1PV() );
	}

	// ũ�ν���� ����Ʈ
	if( g_pEnvSet->m_bCrossHairEffect && (m_pOwner != nullptr) &&
		m_pOwner->isLocal() )
	{
		if( m_pWeaponInfo->isEnableCrossHairEffect() )
		{
			if (i3::kind_of<WeaponSentryGun*>(this) == false)
			{
				if ( CrosshairMgr::i()->getCrosshair()->isEnable())
				{
					CrosshairMgr::i()->getCrosshair()->SetEnableHitEffect(true);
				}
			}

		}
	}

	// ����� ��찡 �ִ°� ���Ƽ� pHitChara �ΰ˻縸 �߰��ϴ� �����θ� �ϴ� ����..
 	if( pHitChara && pHitChara->IsRex() && m_pOwner != nullptr && m_pOwner->isLocal() && DamageInfo.GetHitPart() == CHARA_HIT_VITAL_EYE)
	{
		if( pHitChara->isDeathblowState() == false)
		{
			CHud		*	pHud = CHud::i();

			if( pHud != nullptr)
			{
				pHud->AddDeathBlow();
				pHitChara->SetDeathblowState( true);

				DEATHBLOW_INFO deathBlowInfo;
				deathBlowInfo._nTrexIdx = (UINT8) pHitChara->getCharaNetIndex();
				deathBlowInfo._WeaponInfo = DamageInfo._ui32BaseWeaponID;

				GameEventSender::i()->SetEvent(EVENT_BATTLE_DEATHBLOW ,&deathBlowInfo);
			}
		}
	}
}

void WeaponBase::_CalcHitItem( I3_PHYSIX_HIT_RESULT * pResult, VEC3D * pDir)
{
	I3ASSERT( pResult != nullptr);

	i3PhysixShapeSet * pShapeSet = pResult->m_pDestShapeSet;
	i3EventReceiver * pRecv = pShapeSet->getEventReceiver();
	I3ASSERT( pRecv != nullptr);


	if( i3::same_of<WeaponC5*>(pRecv) || i3::same_of<WeaponClaymore*>(pRecv) || i3::same_of<WeaponM14*>(pRecv))
	{
		//���� �������� �¾����� C5�� ó������ �ʵ��� �մϴ�. ���⿡�� ���� �翬�� �־��...
		if(  getUsedWeapon()->getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_MELEE )
			return;

		WeaponBase * pThrow = (WeaponBase*) pRecv;

		if( getOwner()->isLocal() || getOwner()->isAI())
		{	// �ڽ��� ���� ����Դϴ�.
			// Network�� �������� ������.
			CHARA_DAMAGEINFO damageInfo;

			pThrow->GetDamageInfo( &damageInfo);

			if( IsGrenade() )
				damageInfo._DeathType	= CHARA_DEATH_B;

			damageInfo._HitType = GHT_ITEM_C5;
			damageInfo._nVictimIdx = pThrow->getNetworkIdx();
			damageInfo._rDamage = pThrow->GetDamage( false);

			GAMEEVENT::Write_Chara_Hit( getOwner(), &damageInfo);

			if( BattleServerContext::i()->IsP2PHost() )
			{
				MainWeapon_Throw * pMainWeapon = static_cast<MainWeapon_Throw*>(pThrow->GetMainWeapon());

				if( i3::same_of<WeaponC5*>(pThrow) )
				{
					static_cast<MainWeapon_C5*>(pMainWeapon)->Explosion_C5();
				}
				else if( i3::same_of<WeaponClaymore*>(pThrow) )
				{
					static_cast<MainWeapon_Claymore*>(pMainWeapon)->Explosion();
				}
				else if (i3::same_of<WeaponM14*>(pThrow))
				{
					static_cast<MainWeapon_M14*>(pMainWeapon)->Explosion();
				}
			}
		}
	}

	if( i3::same_of<WeaponDinoDummy*>(pRecv))
	{
		WeaponDinoDummy * pDummy = (WeaponDinoDummy*) pRecv;

		if( getOwner()->isLocal() || getOwner()->isAI())
		{	// �ڽ��� ���� ����Դϴ�.
			// Network�� �������� ������.

			//�������� ó���ϱ�� �߽��ϴ�.


			CHARA_DAMAGEINFO damageInfo;

			GetDamageInfo( &damageInfo);

			damageInfo._HitType = GHT_OBJECT; // ������Ʈ
			damageInfo._nVictimIdx = pDummy->getNetworkIdx();
			damageInfo._rDamage = /*pDummy->*/GetDamage( false);
			damageInfo._ObjNetType = I3_NETWORK_GAMEOBJECT_TYPE_STATIC;

			GAMEEVENT::Write_Chara_Hit( getOwner(), &damageInfo);

		}

		if( BattleServerContext::i()->IsP2PHost() )
		{
			//	pDummy->Uninstall( pDummy->getNetworkIdx());
		}
	}

}


namespace 
{
	template<class T>
	i3PhysixShapeSet* FindValidDomiDinoShapeSet(const T idx, const T max, i3Skeleton* skel, CGameCharaBase* chara)
	{
		i3PhysixShapeSet* shapeSet = skel->getShapeSet(chara->getBoneContext()->getBoneIndex(idx));
		if (shapeSet) return shapeSet;

		for (INT32 i=0; i<static_cast<INT32>(max); i++)
		{
			shapeSet = skel->getShapeSet(chara->getBoneContext()->getBoneIndex(i));
			if (shapeSet) break;
		}

		return shapeSet;
	}
}
void WeaponBase::_OnHitExplosionChara( CGameCharaBase * pChara, CHARA_HIT_PART part, VEC3D * pDir)
{
	VEC3D	vDir;
	I3_PHYSIX_HIT_RESULT	tempHitResult;
	I3_PHYSIX_HIT_RESULT *	pWallHitResultTemp;

	I3ASSERT( pChara != nullptr );
	I3ASSERT( pChara->getSceneObject()->GetBody() != nullptr);
	I3ASSERT( pChara->getSceneObject()->GetBody()->getFrameLOD() != nullptr);

	i3Skeleton * pSkel = pChara->getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();

	I3ASSERT( pSkel != nullptr);
	
	CHARA_DAMAGEINFO DamageInfo;

	tempHitResult.m_T			= i3Vector::Normalize( &tempHitResult.m_Normal, pDir);
	tempHitResult.m_pDestShapeSet = nullptr;

	if( part == CHARA_HIT_HEAD )
	{
		i3Vector::Copy( &tempHitResult.m_Intersect, i3Matrix::GetPos( pChara->getBoneContext()->getHeadMatrix()) );
		if( pChara->IsRex())
		{	
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( TREX_BONE_HEAD) );
		}
		else if( pChara->IsRaptorBase())
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( RAPTOR_BONE_HEAD) );
		}
#ifdef DOMI_DINO
		else if (pChara->IsDomiRaptorBoneBase())
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( DOMI_RAPTOR_BONE_HEAD) );
		}
		else if ( pChara->IsDomiMutantRex())
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( DOMI_MUTANTREX_BONE_HEAD) );
		}
#endif
		else
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( BONE_HEAD) );
		}
	}
	else if( part == CHARA_HIT_ROOT)
	{
		i3Vector::Copy( &tempHitResult.m_Intersect, i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()) );

		if( pChara->IsRex())
		{	
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( TREX_BONE_PELVIS) );
		}
		else if( pChara->IsRaptorBase())
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( RAPTOR_BONE_ROOT) );
		}
#ifdef DOMI_DINO
		else if( pChara->IsDomiRaptorBoneBase())
		{
			tempHitResult.m_pDestShapeSet 
				= FindValidDomiDinoShapeSet<DOMI_RAPTOR_BONES>(DOMI_RAPTOR_BONE_ROOT, DOMI_RAPTOR_BONE_MAX, pSkel, pChara);
		}
		else if (pChara->IsDomiMutantRex())
		{
			tempHitResult.m_pDestShapeSet 
				= FindValidDomiDinoShapeSet<MUTANT_TREX_BONES>(DOMI_MUTANTREX_BONE_PELVIS, DOMI_MUTANTREX_BONE_MAX, pSkel, pChara);
		}
#endif
		else
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( BONE_ROOT) );
		}
	}

	I3ASSERT( tempHitResult.m_pDestShapeSet != nullptr);
	
	i3Vector::Scale( &tempHitResult.m_Normal, &tempHitResult.m_Normal, -1.f);

	GetDamageInfo( &DamageInfo);

	DamageInfo._pHitResult		= &tempHitResult;
	DamageInfo._HitType	= GHT_CHARA;
	DamageInfo.SetHitPart( part );
	DamageInfo._rDistance		= tempHitResult.m_T;

	if( g_pEnvSet->m_bEnableBlood )
	{
		VEC3D	vAdd;
		i3CollideeLine BloodLine;

		if( part == CHARA_HIT_HEAD )
		{
			BloodLine.SetStart( i3Matrix::GetPos( pChara->getBoneContext()->getHeadMatrix()) );
		}
		else if( part == CHARA_HIT_ROOT)
		{
			BloodLine.SetStart( i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()) );
		}
		
		i3Vector::Add( &vAdd, BloodLine.GetStart(), pDir);
		BloodLine.SetEnd( &vAdd );

		pWallHitResultTemp = g_pFramework->raycastClosest( &BloodLine, CGRP_MASKGROUP_ATTACK );
		if( pWallHitResultTemp != nullptr )
		{
			if( pWallHitResultTemp->m_T < BLOODEFFECT_HIT_LENGTH )
				DamageInfo._pWallHitResult = pWallHitResultTemp;
		}
	}

	//	���߹��� ����
	if((m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_GRENADESHELL) &&
		((m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::GSHELL_POISON))))
	{
	
			DamageInfo._DeathType	= CHARA_DEATH_POISON;

	}
	else
	DamageInfo._DeathType	= CHARA_DEATH_B;

	i3Vector::Scale( &vDir, &tempHitResult.m_Normal, -1.f);
	DamageInfo._pVecDir		= &vDir;
	DamageInfo._nVictimIdx	= pChara->getCharaNetIndex();

	i3Vector::Copy( &DamageInfo._vStartPos, GetPos() );
	i3Vector::Copy( &DamageInfo._vHitPos, &tempHitResult.m_Intersect);

	I3ASSERT( DamageInfo.isValid() );

	// ĳ������ HP �� ����Ʈ ó��
	pChara->OnHit( &DamageInfo);
}

REAL32 WeaponBase::_CheckExplosionObjectCollision( REAL32 rRange, REAL32 rRadian)
{
	INT32		i, j;
	VEC3D		vDir, vPos, vTorqPos;
	i3Object *	pGameObj;
	i3CollideeLine	ColLine;

	I3ASSERT( rRange > I3_EPS);
	
	CStageBattle * pStage = g_pFramework->GetStageBattle();
	I3ASSERT_RETURN( pStage != nullptr, 0.f);
	
	StageObjectListPtr pObjectList = pStage->GetStageObjectList();

	if( pObjectList == nullptr )
		return 0.f;

	UINT8	nHitMap[ OBJ_TOTAL_COUNT ] = {0,};
	REAL32 rLength = 0.f;
	REAL32	rDamage = getAP();

	ColLine.SetStart( GetPos() );

	NxScene * pNxScene = g_pPhysixCtx->getScene();
	NxShape* pShapeList[ 256];
	
	NxSphere nxSphere;
	nxSphere.center.x = getX( GetPos());
	nxSphere.center.y = getY( GetPos());
	nxSphere.center.z = getZ( GetPos());

	//if( i3::same_of<WeaponRocketLauncherShell*>(this))
		//rRange += 4.f;

	nxSphere.radius = rRange;

	INT32 iActiveGrp = (1<<CGRP_OBJECT_HIT) | (1<<CGRP_OBJECT_ALL) | (1<<CGRP_OBJECT_WHOLE) | (1<<CGRP_OBJECT_WHOLE_DYNAMIC);

	INT32 nOverlapCount = pNxScene->overlapSphereShapes( nxSphere, NX_DYNAMIC_SHAPES, 256, pShapeList, nullptr, iActiveGrp);
	//I3TRACE( "overlapcount : %d\n", nOverlapCount);
	
	for( i = 0; i < nOverlapCount; i++ )
	{
		i3EventReceiver * pReceiver = (i3EventReceiver*) pShapeList[ i]->userData;
		if( pReceiver == nullptr)
			continue;
		if( !i3::same_of<i3PhysixShapeSet*>(pReceiver))
			continue;

		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pReceiver;
		if( !i3::kind_of<i3Object*>(pShapeSet->getEventReceiver()))
			continue;

		pGameObj = (i3Object*)pShapeSet->getEventReceiver();
		I3ASSERT( pGameObj->getNetIndex() > -1);

		// �̹� ��ϵǾ� �ִ��� Ȯ���մϴ�.
		if( nHitMap[ pGameObj->getNetIndex() ] != 0 )
			continue;

		// �߽����� �������� ����մϴ�.
		MATRIX mat;
		if( pGameObj->getSceneObject()->getCurrentAnim() != nullptr)
		{
			pShapeSet->getShapeGlobalPose( &mat);
			i3Vector::Copy( &vPos, i3Matrix::GetPos( &mat));
		}
		else
		{	
			pShapeSet->getShapeMassCenterPos( &vPos);
		}

		if( IsValidExplosionArea(&vPos, rRange, rRadian, false) == false )
			continue;

		if( i3::same_of<CGameObjectHatchling*>(pGameObj))
		{	// ����� ������ Damage ����
			((CGameObjectHatchling*)pGameObj)->SetControledObjectState( 1);
		}
		else if( i3::same_of<CGameObjectRidingHelicopter*>(pGameObj))
		{	// OutPost ���� ���߿� �� ��쿡�� �������� �޽��ϴ�.
			if( ((CGameObjectRidingHelicopter*)pGameObj)->isEnableDamage() == false)
				continue;
			else
			{
				for( j = 0; j < g_pCharaManager->getCharaCount(); j++)
				{
					CGameCharaBase * pChara = g_pCharaManager->getChara( j);

					if( pChara == nullptr)
						continue;
					if( (pChara->getAttachedObject() == pGameObj) && pChara->is1PV() && (g_pCamera != nullptr) )
					{
						g_pCamera->SetTremble( 5.0f, 15.0f, 0.2f, 2.0f);
					}
				}
			}
		}
		else if( i3::same_of<CGameObjectRespawnHelicopter*>(pGameObj))
		{
			for( j = 0; j < g_pCharaManager->getCharaCount(); j++)
			{
				CGameCharaBase * pChara = g_pCharaManager->getChara( j);		

				if( pChara == nullptr)
					continue;

				if( (pChara->getAttachedObject() == pGameObj) && pChara->is1PV() && (g_pCamera != nullptr) )
				{
					g_pCamera->SetTremble( 5.0f, 15.0f, 0.2f, 2.0f);
				}
			}
		}
		
		// HP�� �ִ� ������Ʈ�� 0�̸� ��ŵ
		if( (pGameObj->getStateCount() > 1) && (pGameObj->getCurrentHP() == 0.f) )
			continue;

		//World�� ���ؼ� �ѹ� �׽�Ʈ
		ColLine.SetEnd( &vPos );

		// revision 63631	������ġ�� ������Ʈ ���̿� ��ֹ��� ������ ������Ʈ�� �������� ���� ���� - 2013. 8. 27 ����
		// revision 64633
		/*if( pResult != nullptr && pResult->m_pDestShapeSet != pShapeSet)
			return 0;*/

		// revision 52397 ���� ������Ʈ���� ���� �ø��� üũ �ڵ� ����
		/*if( _CheckHitWorld( ColLine, m_pWeaponInfo->GetExplosion_Range() ) == false )
			continue;*/

		i3Vector::Sub( &vDir, &vPos, GetPos() );
		rLength = i3Vector::Normalize( &vDir, &vDir );

		// ��ũ�� �ֱ����� ��ġ�� ����մϴ�.
		i3Vector::Scale( &vTorqPos, &vDir, MINMAX( 0.f, 1.f - (rLength / rRange), 1.f) );
		
		if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
		{
			if( gmode::i()->GetTutorialMode() != nullptr)
			{
				if(gmode::i()->GetTutorialMode()->IsAvailableTargetOnTutorialStateOnWeapon(pGameObj->getNetIndex(), this) == false)
				{
					continue;
				}
			}
		}

		//2014.3.27 �ڱ⼺
		//GrenadeShell ������Ʈ �������� ���� �ø��� ���.
		I3_PHYSIX_HIT_RESULT* pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ATTACK);
		_CalcHitObjectDamage( pResult, &ColLine, pGameObj, pShapeSet, rDamage);
		nHitMap[ pGameObj->getNetIndex() ] = 1;

		if( pGameObj->getDynamicState() == I3GAME_DYNAMIC_STATE_DYNAMIC )
		{	// Dynamic Object�� ���� �� ���
			if( BattleServerContext::i()->IsP2PHost() ||
				((pGameObj->getNetworkSyncMode() != I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC) &&
				(pGameObj->getNetworkSyncMode() != I3_NETWORK_GAMEOBJECT_TYPE_MOVE)) )
			{
				_CalcPhysixForce( pShapeSet, &vDir, &vTorqPos, rDamage, rLength);
			}
		}

		//I3TRACE( "HitObject : idx %d (colGroup %d) len : %.4f damage : %.4f\n", pGameObj->getNetIndex(), pDestShapeSet->getShapeGroup(), pNextResult->m_T, rDamage);
	}

	return rLength;
}

void WeaponBase::_CheckExplosionWeapon( REAL32 rRange, REAL32 rRadian )
{
	VEC3D vDir;
	i3CollideeLine line;

	line.SetStart( GetPos());

	// C5�� �˻�
	UINT32 ItemCount = WEAPON::getSettableItemCount();
	for ( UINT32 Index = 0 ; Index < ItemCount; ++Index)
	{
		WEAPON::RES_ID resID = WEAPON::getSettableItemIndex(Index);
		if( resID >= WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE))
			continue;	// ��� �ȵ� ����

		INT32 nCount = g_pWeaponManager->getThrowWeaponCount( resID );

		for( INT32 i = 0; i < nCount; i++)
		{
			WeaponBase * pThrow = static_cast<WeaponBase*>( g_pWeaponManager->getThrowWeapon( resID, i));

			if( pThrow == this)			continue;
			if( !pThrow->isEnable() )	continue;

			MainWeapon_Throw * pMainweapon = static_cast<MainWeapon_Throw*>( pThrow->GetMainWeapon());

			if( !pMainweapon->isBombState( WEAPON_BOMBSTATE_THROWING) )				continue;
			if( pMainweapon->isBombState(WEAPON_BOMBSTATE_EXPLOSION) )				continue;

			if( !IsValidExplosionArea(pThrow->GetPos(), rRange, rRadian, false) )	continue;

			i3Vector::Sub( &vDir, pThrow->GetPos(), GetPos());
			REAL32 rLen = i3Vector::Length( &vDir);

			if( rLen < rRange)
			{
				line.SetDir( &vDir);

				I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
				if( pResult != nullptr)
				{
					if( pResult->m_pDestShapeSet->getShapeGroup() != CGRP_ITEM_HIT)
					{
						// �ش� ������Ʈ���� ����� �Ÿ��� �����ΰ� ������ ��ŵ
						if( pResult->m_T < rLen)
							continue;
					}
				}

				// �ڵ� �����ϵ��� Ŀ�ǵ带 �ݴϴ�.
				// �Ÿ��� ���� ������ Ÿ���� �ݴϴ�.
				if( IsDedicatedHost() )
				{
					//���� Host���� ó���ϴ� �κ��� �ƴ϶�..���𿡼� ó���ϹǷ�..
					//�̰����� �ٷ� ��Ʈ ��Ŷ�� ������.
					CHARA_DAMAGEINFO damageInfo;

					pThrow->GetDamageInfo( &damageInfo);

					/*if( IsGrenade())
						damageInfo._DeathType	= CHARA_DEATH_B;*/

					damageInfo._HitType = GHT_ITEM_C5;
					damageInfo._nVictimIdx = pThrow->getNetworkIdx();
					damageInfo._rDamage = pThrow->GetDamage( false);

					GAMEEVENT::Write_Chara_Hit( getOwner(), &damageInfo);
				}
			}
		}
	}
}

UINT32 WeaponBase::_OnHitBloodMark( I3_PHYSIX_HIT_RESULT ** pResultOut, VEC3D * pStartPos, VEC3D * pDir)
{
	UINT32	i;

	i3CollideeLine BloodLine;
	I3_PHYSIX_HIT_RESULT * pWallHitResultTemp = nullptr;
	REAL32	rMinLen = BLOODEFFECT_HIT_LENGTH;

	BloodLine.SetStart( pStartPos);

	{
		VEC3D	vAdd;
		i3Vector::Add(&vAdd, BloodLine.GetStart(), pDir);
		BloodLine.SetEnd(&vAdd);
	}

	UINT32 nResultCount = g_pFramework->raycast( &BloodLine, CGRP_MASKGROUP_ATTACK );
	if( nResultCount > 0 )
	{
		pWallHitResultTemp = g_pFramework->getRaycastResult();
		g_pFramework->reserveRaycastResult( nResultCount);

		for( i = 0; i < nResultCount; i++ )
		{
			if( pWallHitResultTemp[ i ].m_T < rMinLen )
			{
				*pResultOut = &pWallHitResultTemp[ i ];
				rMinLen = pWallHitResultTemp[ i].m_T;
			}
		}
	}

	return nResultCount;
}

void WeaponBase::_OnHitCharaByExplosion( CGameCharaBase * pHitUser, CHARA_HIT_PART part, VEC3D * pDir,VEC3D * pStart)
{	
	MATRIX mat;
	I3_PHYSIX_HIT_RESULT	tempHitResult;


	I3ASSERT( pHitUser != nullptr );
	I3ASSERT( m_pOwner != nullptr);
	I3_BOUNDCHK( part, CHARA_HIT_MAXCOUNT);

	if(pHitUser->isEnable() == false)
	{
		return;
	}

	const PHYSIX_SHAPESET_INFO * pShapeSetInfo = pHitUser->getPhysixShapeSetInfo( part);

	CHARA_DAMAGEINFO DamageInfo;

	GetDamageInfo( &DamageInfo);
	
	tempHitResult.m_T			= i3Vector::Normalize( &tempHitResult.m_Normal, pDir);
	pShapeSetInfo->_pShapeSet->getShapeGlobalPose( &mat);
	i3Vector::Copy( &tempHitResult.m_Intersect, i3Matrix::GetPos( &mat));
	tempHitResult.m_pDestShapeSet = pShapeSetInfo->_pShapeSet;
	i3Vector::Scale( &tempHitResult.m_Normal, &tempHitResult.m_Normal, -1.f);

	DamageInfo._rExplosionRange = GetExplosionRange();
	DamageInfo._pHitResult		= &tempHitResult;
	DamageInfo._HitType	= GHT_CHARA;
	DamageInfo.SetHitPart( part );
	i3Vector::Copy( &DamageInfo._vStartPos,pStart);
	i3Vector::Copy( &DamageInfo._vHitPos, &tempHitResult.m_Intersect);

	if (DamageInfo.isFireWeaponType(WEAPON_CLASS_GRENADESHELL) &&
		DamageInfo.isFireWeaponNumber( WEAPON::GSHELL_POISON , true) )
	{
		DamageInfo._DeathType		= CHARA_DEATH_POISON;
	}
	else
		DamageInfo._DeathType		= CHARA_DEATH_B;

	DamageInfo._pVecDir			= pDir;
	DamageInfo._rDistance		= tempHitResult.m_T;
	DamageInfo._nVictimIdx		= pHitUser->getCharaNetIndex();
	I3ASSERT( DamageInfo.isValid() );
	// ĳ������ HP �� ����Ʈ ó��
	pHitUser->OnHit( &DamageInfo, m_pOwner);

	if( g_pEnvSet->m_bEnableBlood )
	{
		_OnHitBloodMark( &DamageInfo._pHitResult, i3Matrix::GetPos( &mat), pDir);
		g_pFramework->resetRaycastResult();
	}
}

bool WeaponBase::_OnHit( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine, bool bWarnOwner, bool bDamage )
{	
	I3ASSERT( pResult != nullptr );
	I3ASSERT( m_pOwner != nullptr);

	bool bRet = true;

	i3PhysixShapeSet *	pShapeSet = pResult->m_pDestShapeSet;

	if( pShapeSet == nullptr )
	{
		I3PRINTLOG(I3LOG_WARN,  "WeaponBase:_OnHit() pDestShapeSet nullptr." );
		return false;
	}

	VEC3D	vDir;
	i3Vector::Sub( &vDir, &pResult->m_Intersect, pLine->GetStart() );
	REAL32	rLength = i3Vector::Normalize( &vDir, &vDir );

	switch( pShapeSet->getShapeGroup() )
	{
	case CGRP_WORLD_ATTACK :
		{	// World�� �¾Ҵ�.
			_CalcHitWorld( pResult, &vDir);
		}
		break;
	case CGRP_OBJECT_ALL :
	case CGRP_OBJECT_HIT :
	case CGRP_OBJECT_WHOLE :
	case CGRP_OBJECT_WHOLE_DYNAMIC :
		{	// Object�� �¾Ҵ�.
			_CalcHitObject( pResult,pLine, &vDir, rLength, bDamage);
		}
		break;
	case CGRP_WORLD :
	case CGRP_TEAM_BLUE_ROUGH :
	case CGRP_TEAM_RED_ROUGH :
		break;

	case CGRP_ITEM_HIT:
		_CalcHitItem( pResult, &vDir);
		break;

	case CGRP_TEAM_BLUE :
	case CGRP_TEAM_RED :
		{
			_CalcHitChara( pResult, pLine, &vDir, rLength);
		}
		break;

#ifdef DOMINATION
	case CGRP_ITEM_WHOLE:
		{

		}
		break;
#endif
	}

	return bRet;
}

void WeaponBase::_OnCheckCollisionByExplosion( void)
{
	if( m_pOwner == nullptr)
		return;

	//------------------------------------------------------------------------------------------//
	//										Character Collision	
	UINT32 nColGroup = CGRP_MASKGROUP_ATTACK;
	UINT32 nSelfColGroup = nColGroup;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED)
	{
		nColGroup		= CGRP_MASKGROUP_CHARA_BLUE;
		nSelfColGroup	= CGRP_MASKGROUP_CHARA_RED;
	}
	else
	{
		nColGroup		= CGRP_MASKGROUP_CHARA_RED;
		nSelfColGroup	= CGRP_MASKGROUP_CHARA_BLUE;
	}

	INT32 i;
	VEC3D vLength;

	bool bHost = BattleServerContext::i()->IsP2PHost();

	if( BattleServerContext::i()->IsUseDedicationMode())
	{
		// Dedicated�� �ڽŸ� �ϸ� �ȴ�. ( revision : 14380 ���� )
		if( m_pOwner->isLocal())
			bHost = true;
		else
			bHost = false;
	}

	REAL32 rExplosionRange = GetExplosionRange();

	for( i = 0; i < g_pCharaManager->getCharaCount(); ++i)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if( pChara == nullptr)
			continue;

		i3Vector::Sub( &vLength, GetPos(), i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()));
		REAL32 rLength = i3Vector::Length( &vLength );
		
		if( pChara->IsRex()) rExplosionRange *= 2.0f;

		// ���常 ĳ���� �ø����� �ϵ��� �մϴ�.
		if( (rLength < rExplosionRange))
		{
			if( bHost)
			{
				// Ư�� ����
				if((m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_GRENADESHELL) &&
					((m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::GSHELL_POISON))))
				{
					if( (m_pOwner->getCharaTeam() != pChara->getCharaTeam()))
						_OnCheckCharaCollisionByExplosion( pChara, nColGroup, rExplosionRange);
				}
				else if ((m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION) &&
					(((m_pWeaponInfo->GetNumber() > 6) && (m_pWeaponInfo->GetNumber() < 11)))) //���̴� 7���ε������� ���۵˴ϴ�.
				{
					//���̴� �������� �������� �ʵ��� �մϴ�.
					if( (m_pOwner->getCharaTeam() != pChara->getCharaTeam()))
						_OnCheckCharaCollisionByExplosion( pChara, nColGroup, rExplosionRange);
				}
				else if( (m_pOwner->getCharaTeam() != pChara->getCharaTeam()))	_OnCheckCharaCollisionByExplosion( pChara, nColGroup, rExplosionRange);
				else if( m_pOwner == pChara)								_OnCheckCharaCollisionByExplosion( pChara, nSelfColGroup, rExplosionRange);
			}
		}

		//����ź�� ������ ��ó�� Owner�� ��ġ���ִ°�� ī�޶� ��鸲 ȿ��
		if( (g_pCamera != nullptr) && pChara->is1PV() && !pChara->isCharaStateMask( CHARA_STATE_DEATH) )
		{	
			REAL32 MaxLength	= rExplosionRange * 2.0f;
			if( rLength < MaxLength)
			{
				REAL32 TrembAmpRate = 1.0f - rLength/MaxLength;
				g_pCamera->SetTremble( 15.0f * TrembAmpRate * TrembAmpRate, 10.0f * TrembAmpRate, 0.2f, 1.0f *	TrembAmpRate);
			}
		}
	}

	if( BattleServerContext::i()->IsP2PHost() )
	{
		// ����ź���� ���� ĳ���͸� �켱������ �����ݴϴ�.
		UserContext::i()->SendDeathChara();
	}

	//	Object Collision
	_CheckExplosionObjectCollision( GetExplosionRange());
}

bool WeaponBase::_OnCheckCharaCollisionByExplosion( CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rRange)
{
	bool					bValidCol = false;
	i3CollideeLine			ColLine;
	MATRIX *				pMtx;
	bool					bCharaAlive = !pChara->isCharaStateMask( CHARA_STATE_DEATH) ;
	INT32					nCurHP	= pChara->getCurHP();

	ColLine.SetStart( GetPos());

	addY( ColLine.GetStart(), 0.1f);


	if( bCharaAlive )
	{
		if(pChara->IsDino()) {
			pMtx	= pChara->getBoneContext()->getIKMatrix();
		}
		else {
			pMtx	= pChara->getBoneContext()->getHeadMatrix();
		}
		
		ColLine.SetEnd( i3Matrix::GetPos( pMtx));

		//World�� ���ؼ� �ѹ� �׽�Ʈ
		// ���� �ø��� �Ÿ����� ������.
		if(  _CheckHitWorld( ColLine, rRange ) )
		{
			_OnHitCharaByExplosion( pChara, CHARA_HIT_HEAD, ColLine.GetDir(),ColLine.GetStart() );
			bValidCol = true;
		}
	}

	// ��� ��� �ִ��� �ƴϸ� �׾� �ִ� ��� ó��
	if( bCharaAlive == !pChara->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		pMtx = pChara->getBoneContext()->getIKMatrix();
		ColLine.SetEnd( i3Matrix::GetPos( pMtx));
		
		//World�� ���ؼ� �ѹ� �׽�Ʈ
		if( _CheckHitWorld( ColLine, rRange ) )
		{
			_OnHitCharaByExplosion( pChara, CHARA_HIT_ROOT, ColLine.GetDir(),ColLine.GetStart());
			bValidCol = true;
		}
	}

	if( bValidCol && pChara->isLocal() && (nCurHP > pChara->getCurHP()))
	{	// Hit only Host
		g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_THROW, 0, GetPos(), true);
	}

	return bValidCol;
}

void WeaponBase::CalcHitDir( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pPos, VEC3D * pOutDir)
{
	I3ASSERT( pHitResult != nullptr && pPos != nullptr && pOutDir != nullptr);

	VEC3D shapePos;
	i3PhysixShapeSet*		pDestShapeSet = pHitResult->m_pDestShapeSet;
	I3ASSERT( pDestShapeSet != nullptr);
	
	*((NxVec3 *) &shapePos) = pDestShapeSet->getNxActor()->getGlobalPosition();

	i3Vector::Sub( pOutDir, &shapePos, pPos );
	i3Vector::Normalize( pOutDir, pOutDir);
}

VEC3D* WeaponBase::GetBulletTraceStartPos()
{
	if(getFXMatrix() != nullptr)
		return i3Matrix::GetPos( getFXMatrix());
	return nullptr;
}
VEC3D* WeaponBase::GetSinoBulletTraceStartPos()
{
	return  i3Matrix::GetPos(((CGameCharaDino*)m_pOwner)->getFXMatrix());
}


void WeaponBase::AddBulletTrace(I3_PHYSIX_HIT_RESULT* pBulletHit)
{
	if( g_pTracerBulletManager == nullptr)
		return;

	TracerBulletInfo bulletInfo;
	CWeaponGlobal * pGlobalInfo = g_pWeaponInfoDataBase->getGlobal();
	I3ASSERT( pGlobalInfo != nullptr);

	CWeaponInfo * pWeaponInfo = getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	{	// ����ź ����
		bulletInfo._Type = TRACER_TYPE_BULLET;
		//����:����
		if( (m_pOwner != nullptr) && (m_pOwner->IsSting() || m_pOwner->IsCCSting() || m_pOwner->IsStingMars()))
			i3Vector::Copy( &bulletInfo._vBeginPos, GetSinoBulletTraceStartPos());
	#ifdef DOMI_DINO
		else if ( (m_pOwner != nullptr) && ( m_pOwner->IsDomiSting() || m_pOwner->IsDomiStingMars() ) )
		{
			VEC3D* rp = i3Matrix::GetPos(static_cast<CGameCharaDomiSting*>(m_pOwner)->GetMatWeaponReleasePoint());
			i3Vector::Copy(&bulletInfo._vBeginPos, rp);
		}
	#endif
		else
		{
			VEC3D* pPos = GetBulletTraceStartPos();

			if(pPos != nullptr)
				i3Vector::Copy(&bulletInfo._vBeginPos, GetBulletTraceStartPos());
			else
			{
				i3::rc_wstring wstrWeaponName;
				pWeaponInfo->GetWeaponName(wstrWeaponName);
				I3PRINTLOG(I3LOG_NOTICE, "GetBulletTraceStartPos == nullptr %s", wstrWeaponName);
			}
		}

		i3Vector::Copy( &bulletInfo._vDestPos, m_TargetPos);
		bulletInfo._Length = pGlobalInfo->m_rTracerBulletLength;
		bulletInfo._Thickness = pGlobalInfo->m_rTracerBulletThickness;
		bulletInfo._EndSpeed = pGlobalInfo->m_rTracerBulletSpeed;
		i3Color::Set( &bulletInfo._color, (UINT8) 255, 255, 255, 255);
	}

	TracerBulletInfo bulletSmokeInfo;

	{// ź���� ����
		bulletSmokeInfo._Type = TRACER_TYPE_BULLET_SMOKE;

		i3Vector::Copy( &bulletSmokeInfo._vBeginPos, &bulletInfo._vBeginPos);
		i3Vector::Copy( &bulletSmokeInfo._vDestPos, m_TargetPos);

		// ź������ �Ӽ��� ���⿡�� ���´�.
		pWeaponInfo->SetBulletSmokeInfo(&bulletSmokeInfo);
	}

	bool bEnable = false;
	if( (i3Math::Rand() % 100) < pGlobalInfo->m_iTracerBulletRate)
		bEnable = true;

	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER || getWeaponInfo()->GetTypeClass() == WEAPON_SUBCLASS_BOW)
		bEnable = true;	// �������� ���� 100% ������

	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_DINO)
	{
		if( getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_RANGE)
		{
			bEnable = true;
		}
	}

	if( bEnable)
	{
		if( pBulletHit != nullptr && pBulletHit->m_pDestShapeSet != nullptr )
		{
			VEC3D * pIntersect = &pBulletHit->m_Intersect;

			//	����ź
			i3Vector::Copy( &bulletInfo._vDestPos, pIntersect);
			i3Vector::Copy( &bulletSmokeInfo._vDestPos, pIntersect);
		}

		//	����ź
		g_pTracerBulletManager->Fire(&bulletInfo);

		if(pWeaponInfo->GetBulletSmokeLifeTime() > 0.0f)
		{	//	ź ����
			g_pTracerBulletManager->Fire(&bulletSmokeInfo);
		}
	}
}

void WeaponBase::DoLineCollision( VEC3D * pStart, VEC3D * pEnd, INT32 idxGroup)
{
	bool						bWarningOwner = false;
	I3_PHYSIX_HIT_RESULT *		pBulletHit;
	
	I3ASSERT( pStart != nullptr);
	I3ASSERT( pEnd != nullptr);

	m_CollideeLine.SetStart( pStart);
	m_CollideeLine.SetEnd( pEnd);
	//Sting�� TargetPos�� �޾ƿ°ɷ� �����մϴ�.
	if(m_pOwner->IsSting() || m_pOwner->IsCCSting() || m_pOwner->IsStingMars() ||
		m_pOwner->IsDomiSting() || m_pOwner->IsDomiStingMars() )
	{
		i3Vector::Copy( m_TargetPos, pEnd);
	}

	pBulletHit = DoLineCollision2( &m_CollideeLine, idxGroup, &bWarningOwner);

	AddBulletTrace(pBulletHit);
}

void WeaponBase::_FireEffect( MATRIX * pStart, INT32 idxEffect, bool bAttachWorld)
{
	MATRIX * pMtx = pStart;
	
	if( pMtx == nullptr )
		return;
	
	
	// Light
	if( g_pEnvSet->m_bDynamicLight)
	{
		COLORREAL col;

		i3Color::Set( &col, 1.4f, 1.3f, 1.2f, 1.0f);
		m_pLight->SetDiffuse( &col);

		m_pLight->SetSpecular( &col);
		m_pLight->SetLightType( I3G_LIGHT_POINT);
		m_pLight->setRange( 3.0f);

		if( m_pLight->isState( I3G_LIGHT_STATE_DISABLE))
		{
			g_pFramework->addLight( m_pLight);
		}

		m_pLight->removeState( I3G_LIGHT_STATE_DISABLE);
		m_pLight->SetWorldSpaceTransform( pMtx);
		m_timeLight = 0.0f;
	}

	VEC3D Pos;
	VEC3D norm;

	i3Vector::Copy( &norm, i3Matrix::GetUp(pMtx));
	i3Vector::Copy( &Pos, i3Matrix::GetPos(pMtx));

	if( bAttachWorld)
	{
		pMtx = nullptr;
	}

	i3TimeStamp * pEffect = g_pEffectManager->AddEffect( idxEffect, &Pos, &norm, pMtx);
	

	// Effect�� ����ִ� ����, �ش� ���⸦ �޸� �������� �ʵ��� �ϱ� ���� ��ġ
	if( pEffect != nullptr)
	{
		pEffect->setRefObject( getSceneObject()->GetBody());
	
		SetColorFireEffect(pEffect); // ColorFireEffect (ColorFireMuzzleFlash)
	}
}

void WeaponBase::_IdleEffect(REAL32 rDeltaSeconds)
{
	if ( m_pOwner == nullptr)
		return;

	if (!m_pWeaponInfo->hasIdleEffect())
		return;

	if (!g_pFramework->IsBattleStage())
		return;
	
	if (!isAttached() || IsDroped())
		return;

	if ( m_pOwner->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY) != nullptr
		&& MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		if ( !m_pOwner->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY)->getWeaponInfo()->hasIdleEffect())
			return;
	}
	
	//������Ʈ���� Owner, sceneObject, WeaponInfo�� ������� ��찡 ����
	I3ASSERT(m_pBody != nullptr);
	I3ASSERT(m_pWeaponInfo != nullptr);
	
	//�������� ����Ʈ ���� �ߴ�
	if (isWeaponStateMask(WSM_ZOOM))
		return;

	//����Ʈ ���� Ÿ�� ���
	const REAL32 rLoopTime = m_pWeaponInfo->GetIdleEffectLoopTime();
	m_rLastIdleEffectTime += rDeltaSeconds;

	if (rLoopTime < m_rLastIdleEffectTime)
	{
		//Gundummy�� ����� ����
		MATRIX* pMtx = nullptr;
		if (m_idxFXIdleBone != -1)
			pMtx = m_pBody->getCacheMatrix(m_idxFXIdleBone);
		if (pMtx == nullptr)
			return;

		VEC3D Pos;
		VEC3D norm;

		i3Vector::Copy(&norm, i3Matrix::GetUp(pMtx));
		i3Vector::Copy(&Pos, i3Matrix::GetPos(pMtx));

		i3TimeStamp* pEffect = g_pEffectManager->AddEffect(IdleEffIdx, &Pos, &norm, pMtx);

		// Effect�� ����ִ� ����, �ش� ���⸦ �޸� �������� �ʵ��� �ϱ� ���� ��ġ
		if (pEffect != nullptr)
		{
			pEffect->setRefObject(m_pBody);
		}
		m_rLastIdleEffectTime = 0.0f;
	}

}

void WeaponBase::_RemoveIdleEffect()
{
	if (m_pWeaponInfo->hasIdleEffect())
	{
		if (m_pBody == nullptr)
			return;

		i3EffectTypeInfo* effectType = g_pEffectManager->getEffectType(IdleEffIdx);
		i3TimeStamp* pChild = nullptr;

		for (int i = 0; i < effectType->getRoot()->GetChildCount(); i++)
		{
			pChild = (i3TimeStamp*)effectType->getRoot()->GetChild(i);

			//�� ���⿡�� ���� ����Ʈ�̸� ����
			if (pChild->getRefObject() == m_pBody)
			{
				WEAPON_EFFECT_FORCE_DELETE(pChild);
				//pChild->RemoveFromParent();
			}
		}
	}
}

void WeaponBase::SetColorFireEffect(i3TimeStamp * pEffect)
{

	for (int i = 0; i < pEffect->GetChildCount(); ++i)
	{
		i3Node *pNode = pEffect->GetChild(i);
		for (int j = 0; j < pNode->GetChildCount(); ++j)
		{
			if (i3::same_of<i3Particle*>(pNode->GetChild(j)))
			{
				i3Particle* pParticle = static_cast<i3Particle*>(pNode->GetChild(j));
				
				i3ParticleInfo* pCustomInfo =	pParticle->GetCustomParticleInfo();

				if ( m_idxColorForFireEffect < 1 )
				{
					if (pCustomInfo != nullptr)
					{
						pParticle->EnableCustomParticleInfo(false);
					}
				}
				else
				{
					if (pCustomInfo == nullptr)
					{
						pCustomInfo = i3ParticleInfo::new_object_ref();
						pParticle->SetCustomParticleInfo(pCustomInfo);
					
						i3ParticleInfo* origInfo = pParticle->GetOrigParticleInfo();

						origInfo->CopyTo(pCustomInfo, I3_COPY_INSTANCE);
					}

					pParticle->EnableCustomParticleInfo(true);

					I3COLOR* pColor = const_cast<I3COLOR*>(GetColorbyIdx(0, m_idxColorForFireEffect));

					pCustomInfo->SetColorStyle(0);
					// ������ ����Ʈ�� FUNC_CONSTANT���� ������ ���� ������ �ȵ�

					if (pCustomInfo->GetRGBFunc() != i3ParticleInfo::FUNC_TABLE)
					{
						pCustomInfo->SetRGBFunc(i3ParticleInfo::FUNC_TABLE);
					}

					if (pCustomInfo->GetAlphaTable() != nullptr &&
						pCustomInfo->GetRTable() != nullptr  && 
						pCustomInfo->GetGTable() != nullptr &&
						pCustomInfo->GetBTable() != nullptr)
					{
						*pCustomInfo->GetRTable()->GetTable() = pColor->r;
						*pCustomInfo->GetGTable()->GetTable() = pColor->g;
						*pCustomInfo->GetBTable()->GetTable() = pColor->b;
						*pCustomInfo->GetAlphaTable()->GetTable() = pColor->a;
					}

				}
			}
		}
	} // Color Setting END

	

}

void WeaponBase::_RemoveLight(void)
{
	if( m_pLight != nullptr && (m_pLight->isState( I3G_LIGHT_STATE_DISABLE) == false) )
	{
		m_pLight->addState( I3G_LIGHT_STATE_DISABLE);
		g_pFramework->removeLight( m_pLight);
	}
}

void WeaponBase::_EmptyCartridgeEffect( INT32 mtxIdx, INT32 effectIdx)
{
	// ź�� Effect
	if( (mtxIdx != -1) && ( effectIdx != -1))
	{
		MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( mtxIdx);
		VEC3D norm = VEC3D( 1.0f, 0.0f, 0.0f );

		if( m_pOwner->isLeftHand())
			i3Vector::Scale( &norm, i3Matrix::GetUp(pMtx), -1.0f);
		else
			i3Vector::Copy( &norm, i3Matrix::GetUp(pMtx));

		g_pEffectManager->AddEffect( effectIdx, i3Matrix::GetPos(pMtx), &norm, (MATRIX *) nullptr);
	}
}

void WeaponBase::_PlayEmptyCartridgeSound( I3_TERRAIN_TYPE nTerrainType, VEC3D * pPos )
{
	if( (m_pOwner != nullptr) && getWeaponInfo()->isGun())
	{
		switch( getWeaponInfo()->GetTypeClass() )
		{
		case WEAPON_CLASS_SHOTGUN		:
		case WEAPON_SUBCLASS_SHOTGUN	:
		case WEAPON_CLASS_DUALSHOTGUN	:
			{
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, nTerrainType, GTBT_WPN_SHOTGUN_EMPTYCATRIDGE, pPos, m_pOwner->is1PV());
			}
			break;
		default	:
			{
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, nTerrainType, GTBT_WPN_EMPTYCATRIDGE_1, pPos, m_pOwner->is1PV());
			}
			break;
		}
	}
}

void WeaponBase::_OnPlayTerrainHitSound( WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D * pPos)
{
	//Hit Sound
	if(m_pWeaponInfo->GetLoadBulletType() == 1)
	{
		g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WATERGUN_HIT, pPos, false);
		return;
	}
	//Hit Sound
	switch( m_pWeaponInfo->GetTypeClass() )
	{
	case WEAPON_SUBCLASS_BOW:
	case WEAPON_CLASS_KNIFE :
		{
			WEAPON::RES_ID weaponNum = m_pWeaponInfo->GetRealNumber();
			if( weaponNum == WEAPON::getItemIndex( WEAPON::CANDYCANE))
			{
				//ĵ�������� ����Ʈ ���忡 �����մϴ�.
			}
			else if( weaponNum == WEAPON::getItemIndex(WEAPON::GH5007))
			{
				//	GH5007�� �ι� �ø����Ǿ� �ι� �÷��̵Ǵ� ���� ���ϱ����� ù��° �ø��� �ɶ��� ���������Ѵ�.
				if( fireOrder == WEAPON::SECONDARY_FIRE)
				{
					if( getAutoFireCount() == 1)
						g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_GH5007_SEC1_HIT, pPos, false);
					else
						g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_GH5007_SEC2_HIT, pPos, false);
				}
				else 
					g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_MELEE_HIT, pPos, false);
			}
			else if( weaponNum == WEAPON::getItemIndex(WEAPON::FRYINGPAN)
				|| weaponNum == WEAPON::getItemIndex(WEAPON::FRYINGPAN_DUMMY)
				|| weaponNum == WEAPON::getItemIndex(WEAPON::HAIRDRYER)
				|| weaponNum == WEAPON::getItemIndex(WEAPON::HAIRDRYER_DUMMY) )
			{
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_SUMMEREVENT_WEAPON_HIT, pPos, false);
			}
			else
			{
				INT32 idx = m_pWeaponInfo->GetTerrainHitSoundIndex();
				if( idx > -1)
					g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, idx, pPos, false);
				else
					g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_MELEE_HIT, pPos, false);
			}
		}
		break;
	case WEAPON_CLASS_KNUCKLE :
		{
			INT32 idx = m_pWeaponInfo->GetTerrainHitSoundIndex();
			if( idx > -1)
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, idx, pPos, false);
			else
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_BONEKNIFE_HIT, pPos, false);
		}
		break;
	case WEAPON_CLASS_DUALKNIFE :
		{
			INT32 idx = m_pWeaponInfo->GetTerrainHitSoundIndex();
			if( idx > -1)
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, idx, pPos, false);
			else
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_MELEE_HIT, pPos, false);
		}
		break;

	case WEAPON_SUBCLASS_SHOTGUN	:
	case WEAPON_CLASS_SHOTGUN		:
	case WEAPON_CLASS_DUALSHOTGUN	:
		g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_SHOTGUN_HIT_1, pPos, false);
		break;
	case WEAPON_CLASS_DINO:
	case WEAPON_CLASS_DINO_LEFT_SCRATCH:
	case WEAPON_CLASS_ELITE_LEFT_SCRATCH:
	case WEAPON_CLASS_ELITE_RIGHT_SCRATCH:
		g_pSndMng->ReserveSoundPlay( IT_DINO_TERRAIN_TYPE, type, GTBT_WPN_DINO_HIT, pPos, false);
		break;
	default :
		g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_HIT_1 + i3Math::Rand() % 2, pPos, false);
		break;
	}
}

void WeaponBase::_AddEmptyCartridgeSound( I3_TERRAIN_TYPE nTerrainType, REAL32 delay, VEC3D * pPos )
{
	while ( m_listEmptyBulletSound.size() >= MAX_EMPTYBULLETSOUND_COUNT)		// ����� ��ġ�� �տ��� ������ ����.
	{
		delete m_listEmptyBulletSound.front();
		m_listEmptyBulletSound.pop_front();
	}

	if( delay < I3_EPS)	delay = I3_EPS;

	WEAPON_EMPTYBULLET_SOUND* new_obj = new WEAPON_EMPTYBULLET_SOUND(WEAPON::PRIMARY_FIRE, nTerrainType, delay, *pPos);
	m_listEmptyBulletSound.push_back(new_obj);
}

void WeaponBase::_AddTerrainEffectSound( I3_TERRAIN_TYPE nTerrainType, REAL32 delay, VEC3D * pPos)
{
	while ( m_listTerrainSound.size() >= MAX_TERRAINSOUND_COUNT)		// ����� ��ġ�� �տ��� ������ ����.
	{
		delete m_listTerrainSound.front();
		m_listTerrainSound.pop_front();
	}


	switch( m_pWeaponInfo->GetTypeClass() )
	{
	case WEAPON_CLASS_SHOTGUN		:
	case WEAPON_SUBCLASS_SHOTGUN	:
	case WEAPON_CLASS_DUALSHOTGUN	:	delay = 0.f;	break;
	default							:	break;
	}

	if( delay < I3_EPS)	delay = I3_EPS;

	WEAPON_TERRAIN_SOUND* new_obj = new WEAPON_TERRAIN_SOUND( getFireOrder(), nTerrainType, delay, *pPos);
	m_listTerrainSound.push_back(new_obj);

}

void WeaponBase::ProcessSound( REAL32 tm )
{
	typedef i3::list<WEAPON_EMPTYBULLET_SOUND*>::iterator   iterator1;
	typedef i3::list<WEAPON_TERRAIN_SOUND*>::iterator		iterator2;

	for ( iterator1 it = m_listEmptyBulletSound.begin(), it_end = m_listEmptyBulletSound.end() ; it != it_end ; )
	{
		WEAPON_EMPTYBULLET_SOUND* obj = *it;

		if ( obj == nullptr)
		{
			++it;
			continue;
		}

		obj->_rDelay -= tm;
		if( obj->_rDelay <= 0.f )
		{
			_PlayEmptyCartridgeSound( obj->_nTerrainType, &obj->_vPos);
			delete obj;
			it = m_listEmptyBulletSound.erase(it);
		}
		else 
		{
			++it;
		}
	}

	for ( iterator2 it = m_listTerrainSound.begin(), it_end = m_listTerrainSound.end() ; it != it_end ; )
	{
		WEAPON_TERRAIN_SOUND* obj = *it;

		if (obj == nullptr)
		{
			++it;
			continue;
		}

		obj->_rDelay -= tm;
		if( obj->_rDelay <= 0.f )
		{
			_OnPlayTerrainHitSound( obj->_nFireOrder, obj->_nTerrainType, &obj->_vPos );
			delete obj;
			it = m_listTerrainSound.erase(it);
		}
		else 
		{
			++it;
		}
	}

}

void WeaponBase::ResetSound()
{
	i3::cu::for_each_delete_clear(m_listEmptyBulletSound);
	i3::cu::for_each_delete_clear(m_listTerrainSound);
}

void WeaponBase::OnChangeFPSView(void)
{
	m_pBody->removeStyle(I3BS_LOD_AUTO);
	m_pBody->setCurrentLevel(0);

	// �׸��ڸ� ���ϴ�.
	m_pBody->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER );

	// ���İ� �ִ� �ѱ��� ��� ���ĸ� ���ݴϴ�.
	OnSetBlendAlpha( 1.f);

	SetOwner( m_pOwner);

	m_mainWeapon->OnChangeFPSView();
}

void WeaponBase::OnChangeObserverView(void)
{
	if( (m_pOwner != nullptr) && !m_pOwner->isProfile() )
		m_pBody->addStyle(I3BS_LOD_AUTO);

	// �׸��ڰ� �������� �մϴ�.
	m_pBody->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER );

	// ���İ� �ִ� �ѱ��� ��� ���ĸ� ���ݴϴ�.
	OnSetBlendAlpha( 1.f);

	SetOwner( m_pOwner);

	m_mainWeapon->OnChangeObserverView();
}

void WeaponBase::OnBindResource(void)
{
	i3Chara::OnBindResource();

	m_pAICtx = this->FindAI( "Default");

	if( m_pAICtx == nullptr)
		return;

	i3AnimationResManager * pManager = static_cast<i3AnimationResManager*>(i3NamedResourceManager::FindByMeta( i3AnimationResManager::static_meta()));

	for( INT32 i = 0; i < m_pAICtx->getAIStateCount(); i++)
	{
		i3AIState * pAIState = m_pAICtx->getAIState( i);
		if( pAIState->getAnim() == nullptr )
		{
			if( pAIState->getAnimName()[0] != 0)
			{
				i3Animation * pAnim = (i3Animation*) pManager->GetResource( nullptr, nullptr, pAIState->getAnimName());
				if( pAnim != nullptr)
				{
					pAIState->SetAnim( pAnim);
				}
				else
				{
					g_pCharaManager->LoadWeaponAnimation( pAIState, pAIState->getAnimName());
				}
			}
		}
	}

	setAIIndex( WEAPON::AI_FIRE1PV, (INT8) m_pAICtx->FindAIStateByName( "Fire1PV"));
	setAIIndex( WEAPON::AI_FIRE3PV, (INT8) m_pAICtx->FindAIStateByName( "Fire3PV"));

	setAIIndex( WEAPON::AI_FIRE, (INT8) m_pAICtx->FindAIStateByName( "Fire"));

	//if( getAIIndex( WEAPON::AI_FIRE1PV) == -1 && getAIIndex( WEAPON::AI_FIRE3PV) == -1)
	//	I3ASSERT( getAIIndex( WEAPON::AI_FIRE) != -1);	

	setAIIndex( WEAPON::AI_SECFIRE, (INT8) m_pAICtx->FindAIStateByName( "SecondaryFire"));

	setAIIndex( WEAPON::AI_SECFIRE3PV, (INT8) m_pAICtx->FindAIStateByName( "SecondaryFire_3PV"));
	
	if( getAIIndex( WEAPON::AI_SECFIRE) == -1)
		setAIIndex( WEAPON::AI_SECFIRE, getAIIndex( WEAPON::AI_FIRE));
	
	setAIIndex( WEAPON::AI_LOADMAGAZINE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag"));

	setAIIndex( WEAPON::AI_LOADBULLET, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet"));
	setAIIndex( WEAPON::AI_LOADBULLET1PV, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet1PV"));
	setAIIndex( WEAPON::AI_LOADBULLET3PV, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet3PV"));

	setAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMagMaster1PV"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMagMaster3PV"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER1PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAllMaster1PV"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER3PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAllMaster3PV"));

	setAIIndex( WEAPON::AI_LOADMAGAZINE_1PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_3PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV"));

	setAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAll"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAll_1PV_MALE"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAll_1PV_FEMALE"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAll_3PV_MALE"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAll_3PV_FEMALE"));

	setAIIndex( WEAPON::AI_IDLE1PV, (INT8) m_pAICtx->FindAIStateByName( "Idle1PV"));
	setAIIndex( WEAPON::AI_IDLE3PV, (INT8) m_pAICtx->FindAIStateByName( "Idle3PV"));

	setAIIndex( WEAPON::AI_IDLE, (INT8) m_pAICtx->FindAIStateByName( "Idle"));

	// IDLE �̸����� ���� ���⵵ ���� (Brass Knuckle)
	//if( getAIIndex( WEAPON::AI_IDLE1PV) == -1 || getAIIndex( WEAPON::AI_IDLE3PV) == -1)
	//	I3ASSERT( getAIIndex( WEAPON::AI_IDLE) != -1);

	// Dual ����
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL, (INT8) m_pAICtx->FindAIStateByName( "LoadMagDualGun"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_LEFT, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV_DualGun_Left"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RIGHT, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV_DualGun_Right"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_LEFT, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV_DualGun_Left"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RIGHT, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV_DualGun_Right"));

	setAIIndex( WEAPON::AI_CHANGEDUAL, (INT8) m_pAICtx->FindAIStateByName( "ChangeDualGun"));
	setAIIndex( WEAPON::AI_CHANGEDUAL1PV_LEFT, (INT8) m_pAICtx->FindAIStateByName( "Change1PV_DualGun_Left"));
	setAIIndex( WEAPON::AI_CHANGEDUAL1PV_RIGHT, (INT8) m_pAICtx->FindAIStateByName( "Change1PV_DualGun_Right"));
	setAIIndex( WEAPON::AI_CHANGEDUAL3PV_LEFT, (INT8) m_pAICtx->FindAIStateByName( "Change3PV_DualGun_Left"));
	setAIIndex( WEAPON::AI_CHANGEDUAL3PV_RIGHT, (INT8) m_pAICtx->FindAIStateByName( "Change3PV_DualGun_Right"));



	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_SWAP,   (INT8) m_pAICtx->FindAIStateByName("LoadMag1PVSwapShell"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_SWAP,   (INT8) m_pAICtx->FindAIStateByName("LoadMag3PVSwapShell"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RELOAD, (INT8) m_pAICtx->FindAIStateByName("LoadMag1PVReloadShell"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RELOAD, (INT8) m_pAICtx->FindAIStateByName("LoadMag3PVReloadShell"));

	setAIIndex( WEAPON::AI_EXTENSION_ACTIVATE, (INT8) m_pAICtx->FindAIStateByName( "ExtensionActivate"));
	setAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE, (INT8) m_pAICtx->FindAIStateByName( "ExtensionDeactivate"));

	
	{
		// Extension Activate weapons
		setAIIndex( WEAPON::AI_IDLE_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "Idle_Ext"));
		setAIIndex( WEAPON::AI_FIRE_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "Fire_Ext"));
		setAIIndex( WEAPON::AI_LOADBULLET_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet_Ext"));
		setAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_Ext"));
		
		setAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV_Ext"));
		setAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV_Ext"));

		setAIIndex( WEAPON::AI_EXTENSION_ACTIVATE3PV, (INT8) m_pAICtx->FindAIStateByName( "ExtensionActivate3PV"));
		setAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE3PV, (INT8) m_pAICtx->FindAIStateByName( "ExtensionDeactivate3PV"));
		

	}

	// MachineGun
	{
		setAIIndex( WEAPON::AI_MOVE, (INT8) m_pAICtx->FindAIStateByName( "Move"));
		setAIIndex( WEAPON::AI_CHANGE, (INT8) m_pAICtx->FindAIStateByName( "Change"));
		setAIIndex( WEAPON::AI_CHANGE1PV, (INT8) m_pAICtx->FindAIStateByName( "Change1PV"));
		setAIIndex( WEAPON::AI_CHANGE3PV, (INT8) m_pAICtx->FindAIStateByName( "Change3PV"));
		setAIIndex( WEAPON::AI_DAMAGE, (INT8) m_pAICtx->FindAIStateByName( "Damage"));
		setAIIndex( WEAPON::AI_JUMPEND, (INT8) m_pAICtx->FindAIStateByName( "JumpEnd"));
	}

	// Folded weapon
	{
		setAIIndex( WEAPON::AI_IDLE_FOLDED, (INT8) m_pAICtx->FindAIStateByName( "Idle_Folded"));
		setAIIndex( WEAPON::AI_FIRE_FOLDED, (INT8) m_pAICtx->FindAIStateByName( "Fire_Folded"));
		setAIIndex( WEAPON::AI_IDLE_FOLDED_CHANGE, (INT8) m_pAICtx->FindAIStateByName( "Idle_Folded_Change"));
	}



	// ����ĳ������
	{
		//�Ϲݷε�
		{
			//��
			setAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PVMale"));
			setAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PVMale"));
			//��
			setAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PVFemale"));
			setAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PVFemale"));
		}
		//�ͽ��ټ� ���� �ε�
		{
			//��
			setAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV_Ext_Male"));
			setAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV_Ext_Male"));
			//��
			setAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV_Ext_Female"));
			setAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV_Ext_Female"));

		}
		
		//�ͽ��ټ� ���ε� ������
		{
			//��
			setAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV_EXT, (INT8) m_pAICtx->FindAIStateByName( "LoadMagMaster1PV_Ext"));
			setAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV_EXT, (INT8) m_pAICtx->FindAIStateByName( "LoadMagMaster3PV_Ext"));
		
		}
		//�ε�淿
		{
			//��
			setAIIndex( WEAPON::AI_LOADBULLET1PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet1PVMale"));
			setAIIndex( WEAPON::AI_LOADBULLET3PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet3PVMale"));
			//��
			setAIIndex( WEAPON::AI_LOADBULLET1PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet1PVFemale"));
			setAIIndex( WEAPON::AI_LOADBULLET3PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet3PVFemale"));
		}
		// �߻��� ��Ʈ�׼�
		{
			//��
			setAIIndex( WEAPON::AI_FIREBULLET1PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "FireBullet1PVMale"));
			setAIIndex( WEAPON::AI_FIREBULLET3PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "FireBullet3PVMale"));
			//��
			setAIIndex( WEAPON::AI_FIREBULLET1PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "FireBullet1PVFemale"));
			setAIIndex( WEAPON::AI_FIREBULLET3PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "FireBullet3PVFemale"));

		}
		//�����غ� ����
		{
			setAIIndex( WEAPON::AI_ATTACKREADY_1PV, (INT8) m_pAICtx->FindAIStateByName( "AttackReady1PV"));
			setAIIndex( WEAPON::AI_ATTACKREADY_3PV, (INT8) m_pAICtx->FindAIStateByName( "AttackReady3PV"));

			setAIIndex( WEAPON::AI_GRENADEATTACKREADY_1PV, (INT8) m_pAICtx->FindAIStateByName( "GrenadeAttackReady1PV"));
			setAIIndex( WEAPON::AI_GRENADEATTACKREADY_3PV, (INT8) m_pAICtx->FindAIStateByName( "GrenadeAttackReady3PV"));
			
		}

	}

	#if defined( AI_MOVE_TO_WEAPON)
	// Chara-AI-State�� �˻�
	{
		m_pCharaAINode = FindAI( "CharaAI");

		if( m_pCharaAINode != nullptr)
			m_pCharaAINode->AddRef();

		m_AIIndexTable.Init( this);
	}
	#endif

	m_mainWeapon->OnBindResource();

}

void WeaponBase::_ProcessContactHitBox( void)
{
	INT32 i;
	MATRIX mat;

	if( m_pContactHitBox != nullptr )
	{	
		i3Skeleton * pSkel = getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
		for( i = 0; i < pSkel->getBoneCount(); i++)
		{
			if( pSkel->isShapeSet( i) == false) continue;
			if( pSkel->getShapeSet( i) != nullptr )
			{
				pSkel->getShapeSet( i)->getShapeGlobalPose( &mat);
				m_pContactHitBox->setShapeGlobalPose( &mat);
				break;
			}
		}
	}
}

void WeaponBase::_ProcessDrop( void)
{	
	if( BattleServerContext::i()->IsP2PActivate() == false)
		return;

	if( getNetworkIdx() == -1)
		return;

	if( IsDedicatedHost() )
	{
		if( m_tDropInfo._bDropping && !m_tDropInfo._bDroped && !m_tDropInfo._bContact)
		{
			VEC3D vec;
			i3Skeleton * pSkel = getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
			for( INT32 i = 0; i < pSkel->getBoneCount(); i++)
			{
				if( pSkel->isShapeSet( i) == false) continue;
				if( pSkel->getShapeSet( i) == nullptr)
					continue;

				pSkel->getShapeSet( i)->getLinearVelocity( &vec);
				if( i3Vector::LengthSq( &vec) < 0.01f)
				{
					m_tDropInfo._bDroped = true;
					m_tDropInfo._bContact = true;

					/*
					// Emit dust effect
					if( g_pEnvSet->m_nEffectLevel != 0)
					{
						i3Vector::Set( &vec, i3Math::Randf2(), i3Math::Randf() + 0.1f, i3Math::Randf2() );
						i3Vector::Normalize( &vec, &vec);
						pSkel->getShapeSet( i)->getShapeGlobalPose( &mat);
						g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_WEAPON_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vec );
					}
					*/

					// Network information
					{
						GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( getNetworkIdx() );
						pInfo->setState( NET_WEAPONDROPED_BOUND);
						pInfo->setDroped( true);
					}

					// Fire Effect Color Index Initialize
					{
						m_idxColorForFireEffect = 0;
					}
				}
			}
		}
	}
}

void WeaponBase::OnUpdate( REAL32 rDeltaSeconds)
{
	i3Chara::OnUpdate( rDeltaSeconds);

	if( g_pEnvSet->m_nEffectLevel <= 1)
	{
		_ProcessDrop();
	}
	else
	{
		_ProcessContactHitBox();
		_ParseContactReport();
	}
	#ifdef ENABLE_TEST_COMMAND
		if(g_pWeaponManager && g_pWeaponManager->COMMAND_GetGunBullet() >= 0)
		{
			setTotalBulletCount(g_pWeaponManager->COMMAND_GetGunBullet());
			g_pWeaponManager->COMMAND_SetGunBullet(-1);
		}
	#endif
	if( m_tDropInfo._bContact )
	{	
		if( IsDedicatedHost() && getNetworkIdx() != -1 )
		{
			REAL32 sleepTime = 1.f;

			if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
				sleepTime = 3.f;

			m_tDropInfo._rSleepTimer += rDeltaSeconds;
			if( m_tDropInfo._rSleepTimer > sleepTime)
			{
				PHYSIX_SetSleeping( true );
				m_tDropInfo._rSleepTimer = 0.f;
				m_tDropInfo._bContact = false;

				// Network Information
				GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( getNetworkIdx() );
				if( pInfo->_pWeaponRef != nullptr && pInfo->getState() != NET_WEAPONDROPED_SLEEP)
				{
					pInfo->setState( NET_WEAPONDROPED_SLEEP);
					// ������Ű�� ���� 1ȸ�� �ٷ� ������� �մϴ�.
					pInfo->_fSendTime = 1.0f;
				}
			}
		}
	}

	//	���忡 ������ ����� �ڽ��� ���� LOD ����� �մϴ�.
	if( m_tDropInfo._bDroped)
	{
		//	Zoom�� FOV ���� ���
		REAL32 fRatio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();

		calcLOD( GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()), fRatio);
	}

	// Light
	if( m_pLight != nullptr && (m_pLight->isState( I3G_LIGHT_STATE_DISABLE) == false) )
	{
		m_timeLight += rDeltaSeconds;
		if( m_timeLight > 0.05f)
		{
			m_pLight->addState( I3G_LIGHT_STATE_DISABLE);
			g_pFramework->removeLight( m_pLight);
		}
	}
	
	_IdleEffect(rDeltaSeconds);
	_UpdateSound(rDeltaSeconds);

	// Dragonov�� ���� �ð� ó��
	//if( isStateMask( WSM_ZOOM))
	{
		m_timeAfterZoom += rDeltaSeconds;
	}

	UpdateAfterAnim( rDeltaSeconds);
}

void WeaponBase::PlayLoadMagazine( CHARA::eSEX_TYPE nArg, bool b1PV)
{	
	switch(nArg)
	{
	case CHARA::eSEX_TYPE_MALE :
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_MALE));
		break;
	case CHARA::eSEX_TYPE_FEMALE:
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_FEMALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_FEMALE));
		break;
	case CHARA::eSEX_TYPE_DINO:
		break;
	default:
		break;

	}

}

void WeaponBase::PlayLoadMagazineExt( CHARA::eSEX_TYPE nArg, bool b1PV)
{
	switch(nArg)
	{
	case CHARA::eSEX_TYPE_MALE :
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_MALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_MALE));
		break;
	case CHARA::eSEX_TYPE_FEMALE:
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_FEMALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_FEMALE));
		break;
	case CHARA::eSEX_TYPE_DINO:
		break;
	default:
		break;

	}
}

void WeaponBase::PlayLoadMagazineDualMaz( CHARA::eSEX_TYPE nArg, bool b1PV)
{	
	switch(nArg)
	{
	case CHARA::eSEX_TYPE_MALE :
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_MALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_MALE));
		break;
	case CHARA::eSEX_TYPE_FEMALE:
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_FEMALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_FEMALE));
		break;
	case CHARA::eSEX_TYPE_DINO:
		break;
	default:
		break;
	}
}

bool WeaponBase::NotMasterLoadMagAnim(INT32 idx)
{

	if( (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE)) || 
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION))  ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_MALE)) ||
		 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_FEMALE)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_MALE)) ||
		  (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_FEMALE)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_LEFT)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RIGHT)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_LEFT)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RIGHT)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_FEMALE)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_MALE)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_FEMALE)))

	{
		return true;
	}
	else
	{
		return false;
	}
}

bool WeaponBase::isReloadLoopAnimation()
{
	return getWeaponInfo()->IsReloadLoopAnimation();
}

bool WeaponBase::_PlayWeaponAnimWithApplyTimeScale( INT8 idx)
{	
	if( m_pAICtx == nullptr)	return false;
	if( idx == -1)			return false;
	if( idx >= m_pAICtx->getAIStateCount()) return false;

	REAL32 rTimeScale = 1.f;
	i3AIState * pAIState = m_pAICtx->getAIState( idx);
	CWeaponInfo * pInfo = getWeaponInfo();

	REAL32 rReloadTime = GetReloadTime();
	
	CGameFramework::LoadAIStateAnimation(pAIState);

	// �Ϲ� ���ε�
	if( NotMasterLoadMagAnim(idx) )
	{
		rTimeScale = _CalcReloadTimeScaleToVersion( pAIState, rReloadTime);
	}
	// ������ Īȣ ���� ���ε�
	else if( (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV)) ||
			 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV)) ||
			 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER1PV)) ||
			 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER3PV)) ||
			 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV_EXT)) ||
			 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV_EXT))
			 )
	
	{
		// ���� ������ ��� ���ϴ� ����� TimeScale�� ����.
		if ( !IsFastReloadAnimWeapon() )
		{
			rTimeScale = _CalcReloadTimeScaleToVersion( pAIState, rReloadTime);
		}
	}		
	// LoadBullet
	else if( (idx == getAIIndex( WEAPON::AI_LOADBULLET)) ||
			(idx == getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION))||
			(idx == getAIIndex( WEAPON::AI_LOADBULLET1PV)) ||
			(idx == getAIIndex( WEAPON::AI_LOADBULLET3PV)) ||
			(idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_MALE)) ||
			(idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_FEMALE)) ||
			(idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_MALE)) ||
			(idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_FEMALE)) ||
			(idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_MALE)) ||
			(idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_FEMALE)) ||
			(idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_MALE)) ||
			(idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_FEMALE)) || 
			(idx == getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL1PV_SWAP)) || 
			(idx == getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL3PV_SWAP)))
	{
		REAL32 rLoadBulletTime = pInfo->GetLoadBulletTime();

		if( pInfo->GetTypeClass() != WEAPON_CLASS_SNIPER && rLoadBulletTime > 0.0f)
			rTimeScale = _CalcReloadTimeScaleToVersion(pAIState, rLoadBulletTime, 1.0f);
 	}	
	// ������ Īȣ ���� ���ε�
	else if( idx == getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION) || 
			 idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION))
	{
		if( EnablePlayMasterAnim() )
			rTimeScale = _CalcReloadTimeScaleToVersion( nullptr, 1.0f, MASTER_RELOAD_SPEED_RATIO);
	}
	else if ( EnablePlayMasterAnim() )
	{
		// ���� ������ ��� ���ϴ� ����� TimeScale�� ����.
		if ( !IsFastReloadAnimWeapon() )
			rTimeScale = _CalcReloadTimeScaleToVersion(pAIState, rReloadTime);
	}


	if( idx == getAIIndex( WEAPON::AI_LOADMAGAZINE) || 
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_MALE) || 
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_FEMALE) || 
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_MALE) || 
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_FEMALE) || 
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV) ||
		idx == getAIIndex( WEAPON::AI_LOADBULLET) ||
		idx == getAIIndex( WEAPON::AI_LOADBULLET1PV) || idx == getAIIndex( WEAPON::AI_LOADBULLET3PV) ||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV) ||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER1PV) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER3PV) ||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV_EXT) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV_EXT) ||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL) ||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_LEFT) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RIGHT) ||		
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_LEFT) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RIGHT) ||		
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL) ||
		idx == getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION)	||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION) ||
		idx == getAIIndex(WEAPON::AI_LOADMAGAZINE_1PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADMAGAZINE_3PV_MALE)||
		idx == getAIIndex(WEAPON::AI_LOADMAGAZINE_1PV_FEMALE) || idx == getAIIndex(WEAPON::AI_LOADMAGAZINE_3PV_FEMALE)||
		idx == getAIIndex(WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_MALE) || idx == getAIIndex(WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_MALE)||
		idx == getAIIndex(WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_FEMALE) || idx == getAIIndex(WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_FEMALE)||
		idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_MALE)||
		idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_FEMALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_FEMALE)||
		idx == getAIIndex(WEAPON::AI_LOADBULLET_LEFT_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET_RIGHT_MALE)||
		idx == getAIIndex(WEAPON::AI_LOADBULLET_LEFT_FEMALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET_RIGHT_FEMALE)
	)
	{
		// �ʹݿ� �׳� Time Scale�� 1�� ���� ��ȯ.
		if (isReloadLoopAnimation())
		{
			rTimeScale = 1.f;

			// �̻��ϰ� �̰������� ����Ǿ� ������... ���⵵ ��¿�� ����...
			if (g_pEnvSet->IsV2Version() )
			{
				REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);
				CASHITEM_TRACE("CASHITEMSKILL_REBULLET_RATE = %f\n", ratio);
				rTimeScale += (rTimeScale * ratio);
			}
		}

		// �������� �� �߻��Ҷ� LoadBullet ������ ������ ���� �������.
		if( pInfo->GetTypeClass() == WEAPON_CLASS_SNIPER &&
			(idx == getAIIndex( WEAPON::AI_LOADBULLET) || idx == getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION) ||
		     idx == getAIIndex( WEAPON::AI_LOADBULLET1PV) || idx == getAIIndex( WEAPON::AI_LOADBULLET3PV) ||
			 idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_FEMALE) ||
			 idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_FEMALE) ||
			 idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_MALE) || idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_FEMALE) ||
			 idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_MALE) || idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_FEMALE)))
		{
			// no use
		}
		// MG �����Ҷ� LoadBullet ������ ������ ���� �������.
		else if( pInfo->GetTypeClass() == WEAPON_CLASS_MG &&
			(idx == getAIIndex( WEAPON::AI_LOADBULLET) || idx == getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION) ||
			idx == getAIIndex( WEAPON::AI_LOADBULLET1PV) || idx == getAIIndex( WEAPON::AI_LOADBULLET3PV) ||
			idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_FEMALE) ||
			idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_FEMALE) ||
			idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_MALE) || idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_FEMALE) ||
			idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_MALE) || idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_FEMALE)))
		{
			// no use
		}
		//	���� ���� ������ ���� �ð� ����. 
		else
		{
			if (g_pEnvSet->IsV2Version() == false)
			{
				REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);
				CASHITEM_TRACE("CASHITEMSKILL_REBULLET_RATE = %f\n", ratio);
				rTimeScale += (rTimeScale * ratio);
			}
		}
	}

	if( rTimeScale < I3_EPS)
	{
		rTimeScale = I3_EPS + 0.00001f;
	}

#if defined( USE_RELOAD_NEWANIMATION)
	WF::PlayWeaponAnimDirect( this, (INT32) idx, rTimeScale);
#else
	WF::PlayWeaponAnim( this, (INT32) idx, rTimeScale);
#endif
	setAIIndex( WEAPON::AI_CURRENT, idx);

	//I3PRINTLOG(I3LOG_NOTICE,"Weapon rTimeScale: %f \n", rTimeScale);
	return true;
}

void WeaponBase::_OnPlayWeaponIdleAnim(void)
{	
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
	{
		if( getAIIndex( WEAPON::AI_IDLE_EXTENSION) != -1 )
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_IDLE_EXTENSION) );
			return;
		}
	}

	if( getAIIndex( WEAPON::AI_IDLE_FOLDED) != -1)
	{
		if( i3::kind_of<WeaponDualSMG*>(this) )
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_IDLE_FOLDED));
		else if( i3::kind_of<WeaponDualSMG*>(getPairWeapon()))
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_IDLE_FOLDED));
		else
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_IDLE) );
	}
	else
	{
		if( (getAIIndex( WEAPON::AI_IDLE1PV) != -1) && (getAIIndex( WEAPON::AI_IDLE3PV) != -1))		// �켱
		{
			if(  m_pOwner != nullptr && m_pOwner->is1PV())
				WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_IDLE1PV) );
			else if(  m_pOwner != nullptr && m_pOwner->is3PV())
				WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_IDLE3PV) );
			else if( getAIIndex( WEAPON::AI_IDLE) != -1)
				WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_IDLE) );
		}
		else if( getAIIndex( WEAPON::AI_IDLE) != -1)
		{
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_IDLE) );
		}
	}

	//setWeaponState( WEAPON::STATE_READY);
}

void WeaponBase::_OnPlayWeaponFireAnim(void)
{	
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
	{
		if( getAIIndex( WEAPON::AI_FIRE_EXTENSION) != -1 )
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE_EXTENSION) );
			return;
		}
	}
	
	if( getAIIndex( WEAPON::AI_IDLE_FOLDED) != -1)
	{
		if( i3::kind_of<WeaponDualSMG*>(this))
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE_FOLDED));
		else if( i3::kind_of<WeaponDualSMG*>(getPairWeapon()))
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE_FOLDED));
		else
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE) );
	}
	else
	{
		if( (getAIIndex( WEAPON::AI_FIRE1PV) != -1) && (getAIIndex( WEAPON::AI_FIRE3PV) != -1))		// �켱
		{
			if(  m_pOwner != nullptr && m_pOwner->is1PV())
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE1PV) );
			else if(  m_pOwner != nullptr && m_pOwner->is3PV())
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE3PV) );
			else if( getAIIndex( WEAPON::AI_FIRE) != -1)
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE) );
		}
		else
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE) );
		}
	}
}

void WeaponBase::_OnPlayWeaponSecondFireAnim( void)
{
	if ( getAIIndex( WEAPON::AI_SECFIRE3PV) != -1)
	{
		if ( m_pOwner != nullptr && m_pOwner->is1PV())
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_SECFIRE));
		else if ( m_pOwner != nullptr && m_pOwner->is3PV())
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_SECFIRE3PV));
		else
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_SECFIRE));
	}
	else
	{
		WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_SECFIRE));
	}
}


static bool _SetSoundNodeProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( !i3::same_of<i3AIContext*>(pNode))
		return true;

	bool * isOwner = (bool*)pUserData;

	i3AIContext * pCtx = (i3AIContext *) pNode;
	i3AI * pAI = pCtx->getAI();
	INT32 i, j;

	for( i = 0; i < pAI->getAIStateCount(); i++)
	{
		i3AIState * pState = pAI->getAIState( i);
		i3TimeSequence * pTimeSeq = pState->getTimeSeq();

		if( pTimeSeq == nullptr)
			continue;

		for( j = 0; j < pTimeSeq->GetSequenceCount(); j++)
		{
			i3TimeSequenceInfo * pInfo = pTimeSeq->GetSequence( j);

			if( i3::kind_of<i3SoundNode*>(pInfo->GetNode()))
			{
				i3SoundNode * pSnd = (i3SoundNode *) pInfo->GetNode();
				if( pSnd == nullptr) 
					continue;

				i3SoundPlayInfo * pPlayInfo = pSnd->getSoundPlayInfo();
				if( pPlayInfo == nullptr)
					continue;

				if( pPlayInfo->GetSoundType() == I3SND_PROP_TYPE_VOICE)
				{
					pPlayInfo->RemoveStyle( I3SND_PROP_TYPE_VOICE);
					pPlayInfo->AddStyle( I3SND_PROP_TYPE_EFFECT);
				}

				UINT32 n3DMode = I3SND_RT_3DMODE_WORLD_SPACE;
				//Listener Sound�� �Ӽ��� ������ �����鼭 Owner�Ӽ��ΰ�츸 positional sound�� disable��Ų��.
				if( *isOwner == true)
				{
					n3DMode = I3SND_RT_3DMODE_DISABLE;
				}

				pPlayInfo->set3DMode( n3DMode);
			}
		}
	}

	return true;
}

void WeaponBase::SetOwner( CGameCharaBase * pOwner)
{


	if (pOwner == nullptr)
	{
		m_idxColorForFireEffect = 0;
	}
	else
	{
		INT32 SlotIdx = pOwner->getCharaInfo()->GetNetIdx();
		// ���� �����ڰ� �ڱ� �ڽ��� ���� �ڽ��� InvItemData�� �޾ƿ��� �˴ϴ�
		if ( m_pWeaponInfo != nullptr && m_pWeaponInfo->GetUnColorEffectFlag())
		{
			m_idxColorForFireEffect = 0;
		}
		else
			if(SlotIdx == BattleSlotContext::i()->getMySlotIdx())
			{
				USER_INFO_INVITEM_DATA Data;
				Data.Reset();
				UserInfoContext::i()->GetMyUserInfoInvItemData(&Data);
				m_idxColorForFireEffect = Data._ui16MuzzleFlashColor;
			}
			else
			{
				m_idxColorForFireEffect = BattleSlotContext::i()->getFireMuzzleColorForSlot(SlotIdx);
			}
	}


	m_pOwner = pOwner;

	if( m_pOwner != nullptr )
	{
		bool bOwner = m_pOwner->is1PV();
		//Sound�� Owner������ 3dmode�� ���� �ҷ��� ������ �����Ѵ�.
		{
			//I3TRACE(">>>WEAPON_MANAGER : START<<<%s>>>>>>>>>>\n", pOwner->GetName());

			i3GameNodeTraversal Traverse;

			Traverse.setUserData( &bOwner);
			Traverse.setUserProc( _SetSoundNodeProc);

			Traverse.Traverse( this);

			//I3_SAFE_RELEASE( pTraverse);

			//I3TRACE(">>>WEAPON_MANAGER : END<<<%s>>>>>>>>>>\n", pOwner->GetName());
		}

		for( INT32 i = 0; i < WEAPON::FIRE_SOUND_COUNT; i++)
		{
			if( m_pFireSoundPlayInfo[ i] != nullptr)
			{
				if( bOwner)
		 			m_pFireSoundPlayInfo[ i]->set3DMode( I3SND_RT_3DMODE_DISABLE);
				else
					m_pFireSoundPlayInfo[ i]->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);
			}
		}

		addWeaponFlag( WEAPON_FLAG_TAKE_CHARA);
	}
	else
	{
		m_ui8Endurance		= 100;
		removeWeaponFlag( WEAPON_FLAG_TAKE_CHARA);
	}
	
	if( m_mainWeapon )
		m_mainWeapon->SetOwner(pOwner);
}

void WeaponBase::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	if( pStartPos != nullptr )
	{
		i3Vector::Copy( m_StartPos, pStartPos ); 
		m_bSetStart	= true;
	}
	else
	{
		m_bSetStart	= false;
	}
	
	if( pTargetPos != nullptr )
		i3Vector::Copy( m_TargetPos, pTargetPos);

	setWeaponState( WEAPON::STATE_FIRE);

	//	�߻� Ÿ��
	if( type == WEAPON::PRIMARY_FIRE)			_OnPlayWeaponFireAnim();
	else if( type == WEAPON::SECONDARY_FIRE)		_OnPlayWeaponSecondFireAnim();
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "weapon fire type invalid.");
	}

	m_FireOrder = type;

	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()) )
	{
		addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
	}
	removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);

	//////////////////////////////
	if( isWeaponStateMask( WSM_ZOOM))
		m_timeAfterZoom = 0.0f;

	removeWeaponStateMask( WSM_FIRST_ZOOM);
}

void WeaponBase::ReloadAnimation(CHARA_VIEW_TYPE eCAT)
{
	if( (m_pPairWeapon != nullptr) && m_pPairWeapon->isDualGun())
	{
		if( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL) != -1)
		{
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL));
		}
		else
		{
			if( m_pOwner->is1PV() && ( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_LEFT) != -1) )
			{
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_LEFT));
			}
			else if( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_LEFT) != -1)
			{	
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_LEFT));
			}
			else
			{
				// �ε� �ִϸ��̼��� 1, 3��Ī �������� �Ǿ� �ִ� ���.
				if( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV) == -1)
				{
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE));
				}
				// ������ �Ǿ� �ִ� ��� 2008. 10. 28. ���ķ� ��κ��� ����� 1, 3��Ī ������ �ȴ�.
				else
				{
					if(CHARA_VIEW_1PV == eCAT)
					{
						_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV));
					}
					else
					{
						_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV));
					}
				}
			}
		}		
	}
	else if(EnablePlayMasterAnim())
	{
		if(IsFastReloadAnimWeapon())
		{
			if(CHARA_VIEW_1PV == eCAT)
			{

				if( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV_EXT) != -1  && isWeaponStateMask( WSM_ACTIVATE_EXTENSION))
				{
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV_EXT));
				}
				else
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV));
			
			}
			else
			{
				if( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV_EXT) != -1  && isWeaponStateMask( WSM_ACTIVATE_EXTENSION))
				{
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV_EXT));
				}
				else
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV));
			}
		}
		// �׳� �ִϸ��̼Ǹ� ������ �� �ش�.
		else
		{
			// �ε� �ִϸ��̼��� 1, 3��Ī �������� �Ǿ� �ִ� ���.
			if( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV) == -1&&
				getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE) == -1)
			{
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE));
			}
			// ������ �Ǿ� �ִ� ��� 2008. 10. 28. ���ķ� ��κ��� ����� 1, 3��Ī ������ �ȴ�.
			else
			{
				bool b1PV = (bool)(CHARA_VIEW_1PV == eCAT);

				if( b1PV)
				{
					if( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE) != -1)
					{
						PlayLoadMagazine( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV );
					}
					else
					{
						_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV));
					}
					
				}
				else
				{	
					if( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_MALE) != -1)
					{
						PlayLoadMagazine( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV );
					}
					else
					{
						_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV));
					}
					
				}
			}
		}
	}
	else
	{
		// �ε� �ִϸ��̼��� 1, 3��Ī �������� �Ǿ� �ִ� ���.
		if( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV) == -1 &&				
			getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE) == -1)
		{
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE) );
		}
		// ������ �Ǿ� �ִ� ��� 2008. 10. 28. ���ķ� ��κ��� ����� 1, 3��Ī ������ �ȴ�.
		else
		{
			bool b1PV = (bool) (CHARA_VIEW_1PV == eCAT);
			if( b1PV)
			{
				if( getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_MALE) != -1 && isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
				{
					PlayLoadMagazineExt( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV);
				}
				else if(getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE) != -1)
				{
					PlayLoadMagazine( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV);
				}
				else
				{
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV));
				}
			}
			else
			{
				if(getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_MALE) != -1 && isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
				{
					PlayLoadMagazineExt( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV);
				}
				else if(getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_MALE) != -1)
				{
					PlayLoadMagazine( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV);
				}
				else
				{
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV));
				}
			
			}
		}
	}
}



void  WeaponBase::ChangeDualAnimation( CHARA_VIEW_TYPE eCAT, REAL32 rTimeScale)
{
	if( (m_pPairWeapon != nullptr) && m_pPairWeapon->isDualGun())
	{
		if( getAIIndex( WEAPON::AI_CHANGEDUAL) != -1)
		{
			// revision 28684
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_CHANGEDUAL), rTimeScale );
		}
		else
		{
			if( eCAT == CHARA_VIEW_1PV )
			{
				if( getAIIndex( WEAPON::AI_CHANGEDUAL1PV_LEFT) != -1 )
				{
					WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_CHANGEDUAL1PV_LEFT), rTimeScale );
				}
			}
			else if( getAIIndex( WEAPON::AI_CHANGEDUAL3PV_LEFT) != -1)
			{
				WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_CHANGEDUAL3PV_LEFT), rTimeScale );
			}
		}		
	}
}


void WeaponBase::OnPlayLoadMagAnimation( void)
{	
#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
	I3ASSERT(m_pOwner);
#else
	if(m_pOwner == nullptr) return;
#endif

	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
	{
		if( getAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION) != -1 )
		{
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION));
		}
		else if ( getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION) != -1)
		{
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION));
		}
		else if ( getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION) != -1)
		{
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION));
		}
	    else
		{
			ReloadAnimation( m_pOwner->getViewType());
		}
	}
	else
	{
		ReloadAnimation( m_pOwner->getViewType());
	}

	setWeaponState( WEAPON::STATE_LOADMAG);
}

bool WeaponBase::EnablePlayMasterAnim()
{
	return _IsEnableMasterAnimToVersion();
}

bool WeaponBase::IsFastReloadAnimWeapon()
{
	I3ASSERT(getWeaponInfo());

	return getWeaponInfo()->IsFastReloadAnimation();
}

void WeaponBase::OnPlayLoadBulletAnimation( void)
{
	bool bPlay = false;

	// extension animation�� ���� �÷����غ���.
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION))
	{
		bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION) );
	}
	
	if( bPlay == false)
	{	// extension anim�� ����.
		if( m_pOwner->is1PV())
		{
			// 1��Ī�� anim �÷���
			bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET1PV) );
			
			if( bPlay == false)
			{	// 1��Ī anim�� ����.
				// �� ���� anim�� �÷����غ���.
				switch( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX())
				{
				case CHARA::eSEX_TYPE_MALE :	
					bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET1PV_MALE));		
					break;

				case CHARA::eSEX_TYPE_FEMALE:	
					bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET1PV_FEMALE));	
					break;
				}
			}
		}
		else if( m_pOwner->is3PV())
		{
			bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET3PV) );
			
			if( bPlay == false)
			{
				switch( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX())
				{
				case CHARA::eSEX_TYPE_MALE :	
					bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET3PV_MALE));		
					break;

				case CHARA::eSEX_TYPE_FEMALE:	
					bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET3PV_FEMALE));	
					break;
				}
			}
		}
	}

	if( bPlay == false)
	{// �⺻ anim�� �÷�������
		_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET));
	}
}

void WeaponBase::OnPlayExtensionAnimation( bool bVal, bool bAnimPlay)
{	
	if( bAnimPlay )
	{
		if( bVal)
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE));
		else
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE));
	}
	else
		_OnPlayWeaponIdleAnim();
}

void WeaponBase::NET_FireSync( VEC3D * pTargetPos, WEAPON::FIRE_ORDER fireOrder )
{
	INT32	 nR;

	if( g_pRand->CheckCreation() )
	{	
		if( m_pOwner != nullptr && !m_pOwner->isNetwork())
		{	// �ڽ��� ĳ�����Դϴ�.
			g_pRand->GetPreCode();
			nR = g_pRand->GetRand();
			srand( nR );

			GAMEEVENT::Write_Chara_Fire( m_pOwner, pTargetPos, this);
		}
		else
		{
			// ��Ʈ��ũ ĳ���� �Դϴ�.
			nR = g_pRand->GetRand( g_pRand->GetCode() );
			srand( nR );
		}
	}
}

#ifdef DOMI_STING_POISON_GRENADE
void WeaponBase::NET_FireSync_DomiDinoGrenade(VEC3D* pTargetPos, WEAPON::FIRE_ORDER fireOrder)
{
	GAMEEVENT::Write_Chara_Fire_DomiDinoGrenade( m_pOwner, pTargetPos, this);
}
#endif

void WeaponBase::NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )
{
}

void WeaponBase::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner )
{
	i3Node * pAttachNode = getSceneObject();
	
	//	�������� ���� ���δ�.	
	if( pAttachNode->GetParent() != pSceneNode && pSceneNode != nullptr)
	{
		I3_MUST_ADDREF( pAttachNode);
		if( pAttachNode->GetParent() != nullptr)
			pAttachNode->RemoveFromParent();
				
		pSceneNode->AddChild( pAttachNode);
		I3_MUST_RELEASE( pAttachNode);
	}

	//	������Ʈ�� ���� ���δ�.
	if( m_pParentGame != pOwner && pOwner != nullptr)
	{
		if( m_pParentGame != nullptr)
			m_pParentGame->RemoveChild( this);

		m_pParentGame = pOwner;
		I3_GAMENODE_ADDCHILDTOHEAD( m_pParentGame, this );
	}

	if( pSceneNode != nullptr && m_pParentGame != nullptr)
		addWeaponFlag( WEAPON_FLAG_ATTACHED);

	#if defined( AI_MOVE_TO_WEAPON)
	if( i3::kind_of<i3Chara*>(pOwner))
	{
		if( (m_AttachGrip == WEAPON_ATTACH_GRIP) && (m_pCharaAINode != nullptr))
		{
			// KEYWORD : AI_MOVE_TO_WEAPON
			// ���⸦ �տ� ��� ���...
			AttachCharaAIToChara( (i3Chara *) pOwner);

			_Rec_BindWeaponAnim( m_pCharaAINode);
		}
	}
	#endif

	m_mainWeapon->AttachNode(pSceneNode, pOwner);

}

void WeaponBase::DetachNode( void )
{
	i3Node * pAttachNode = getSceneObject();
	if( ( pAttachNode != nullptr) && (pAttachNode->GetParent() != nullptr) )
	{
		pAttachNode->RemoveFromParent();
	}

	if( m_pParentGame != nullptr)
	{
		if( m_pParentGame->getFirstChild() != nullptr)
			m_pParentGame->RemoveChild( this);
		m_pParentGame = nullptr;
	}

	// ���� ����� Light�� ���� ��� ���ش�
	if( m_pLight != nullptr && (m_pLight->isState( I3G_LIGHT_STATE_DISABLE) == false) )
	{
		m_pLight->addState( I3G_LIGHT_STATE_DISABLE);
		g_pFramework->removeLight( m_pLight);
	}

	removeWeaponFlag( WEAPON_FLAG_ATTACHED);
	ChangeGunDummy( WEAPON_ATTACH_NONE);

	#if defined( AI_MOVE_TO_WEAPON)
	if( m_pCharaAINode != nullptr)
	{
		// KEYWORD : AI_MOVE_TO_WEAPON
		DetachCharaAIFromChara();
	}
	#endif

	if (m_mainWeapon)
		m_mainWeapon->DetachNode();

}

void WeaponBase::OnDrop(void)
{	// ���⸦ ����߸��� �ͽ��ټ��� ����.
	setExtensionActivate(false);
	m_pWeaponInfo = m_pAbsoluteWeaponInfo;		//	����� �ֹ��� ������ �ٽ� ��ȯ

	m_CurrentExtensionType = m_pWeaponInfo->getExtensionType(0); //	ù��° �ͽ��ټ����� �ʱ�ȭ ����. �߰��� �ٲ�� �ִ� ��

}

void WeaponBase::GetDamageInfo( CHARA_DAMAGEINFO * pOut)
{
	WeaponBase * pThis = getUsedWeapon();

	if( getPairWeapon() != nullptr )
	{
		// ũ���� �迭�� ����� Pair�� ���� �����̴�.
		WeaponBase * pPairWeapon = getPairWeapon();
		if( pPairWeapon->getPairWeapon() != nullptr)
			pPairWeapon = pPairWeapon->getPairWeapon();

		pPairWeapon->getWeaponInfo()->GetDamageInfo( pOut);

		if( i3::same_of<WeaponGrenadeShell*>(this))
			pOut->_bExtension = true;
		else
			pOut->_bExtension = pPairWeapon->isExtensionActivate();

		if( pOut->_bExtension)
		{
			// �ͽ��ټ� ���¶�� ���� ����(this�� �ͽ��ټ� �����)�� ����
			pThis = this;
		}
		else
			pThis = pPairWeapon->getUsedWeapon();
	}
	else
	{
		m_pWeaponInfo->GetDamageInfo( pOut);
		pOut->_bExtension = isExtensionActivate();
	}

	I3ASSERT( pThis != nullptr);

	pOut->SetWeaponItemID(this);

	if( m_pOwner != nullptr)
	{
		pOut->_IsOwner			= m_pOwner->isLocal();
		pOut->_nKillerIdx		= m_pOwner->getCharaNetIndex();
	}

	{
		pOut->_FireOrder		= pThis->getFireOrder();
		pOut->_AttackType		= pThis->getWeaponInfo()->GetAttackType( pThis->getFireOrder(), pThis->getAutoFireCount() );
	}

	pOut->_rDamage			= getAP();
	pOut->_nNetidx			= getNetworkIdx();
	pOut->_pWeapon			= this;
	pOut->_pVecPos			= GetPos();

	if( m_pWeaponInfo != nullptr)
	{
		pOut->_pWeaponInfo		= m_pWeaponInfo;
		pOut->_DamageType		= m_pWeaponInfo->GetTypeDamage();
		pOut->_rExplosionRange	= m_pWeaponInfo->GetExplosion_Range();
		pOut->_rExplosionSlope	= m_pWeaponInfo->GetExplosion_EffectiveSlope();
		pOut->_rEffectiveTime	= m_pWeaponInfo->GetEffectiveTime();
	}
}

WEAPON::ATTACK_TYPE	WeaponBase::GetAttackType( void)
{
	return m_pWeaponInfo->GetAttackType( getFireOrder(), getAutoFireCount());
}

void WeaponBase::ResetColideeGroup(void)
{
	_InitCollideeGroup();
}

// xgkick_thai
void WeaponBase::setTeamTag( INT32 team)
{
	m_tDropInfo._nUserTeam = team;
}

void WeaponBase::Transmission( i3Node * pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld)
{
	m_tDropInfo._bDropping = bToWorld;

	DetachNode();
	AttachNode( pNewSceneNode, pNewOwner);

	if( pPos != nullptr)
	{
		SetPos( pPos);
	}

	// Alpha check ���İ� �ʿ��� ��� ���ݴϴ�.
	CheckAlphaUsed();
}

void WeaponBase::ChangeAttachHand( CHARA_WEAPON_ATTACH_HAND nAttachHand, WEAPON_ATTACH_TYPE nWeaponAttach)
{
	if( getOwner() == nullptr) 
		return;

	getOwner()->getWeaponContext()->setAttachHand( nAttachHand);

	//	���� ��� �ִ� ���Ⱑ �ִٸ� ���Ⱑ �ٴ� ���� �ٲ۴�. (������ �Ǵ� ���� �ٲ۴�)
	if (getOwner()->getWeaponContext()->getAttachRoot())
	{
		I3ASSERT( getOwner()->getWeaponContext()->getAttachRoot() != nullptr );
		AttachNode( getOwner()->getWeaponContext()->getAttachRoot(), getOwner());
	}
	else
		TRACE("WeaponBase::ChangeAttachHand getOwner()->getWeaponContext()->getAttachRoot()=nullptr\n");
	

	ChangeGunDummy( nWeaponAttach);
}

// KEYWORD : WEAPON_DUMMY
void WeaponBase::ChangeGunDummy( WEAPON_ATTACH_TYPE nAttachType)
{	
	//	�� ������ 
	switch( nAttachType)
	{
	case WEAPON_ATTACH_GRIP:	getSceneObject()->SetMatrix( &m_mtxGunDummy);		break;
	case WEAPON_ATTACH_BARREL:
#if defined( USE_RELOAD_NEWANIMATION)
		getSceneObject()->SetMatrix( &m_mtxGunDummy);		break;
#else
		if( m_pGunDummyLeftHand != nullptr)
		{
			getSceneObject()->SetMatrix( &m_mtxGunDummyLeftHand );
		}
#endif
		break;
	}

	m_AttachGrip = nAttachType;
}


/**********************************************************************************************//**
 * @fn	void WeaponBase::MoveNodeForBow()
 *
 * @brief	Move node for bow.
 * @desc	Ȱó�� ����̿� �ٴ� ����� �ٸ� ���⸦ ���� ���� �Լ�
 * @author	Ran
 * @date	2011-09-22
 **************************************************************************************************/

void WeaponBase::MoveNodeForBow()
{

	i3Transform *				m_pTransformDummy;

	m_pGunDummyBack = i3Scene::FindNodeByName( getSceneObject(), "Back");
	if( m_pGunDummyBack != nullptr)
	{				
		i3Scene::MoveChildren(m_pGunDummyBack, getSceneObject()->GetBody() );
		m_pTransformDummy =(i3Transform*) m_pGunDummyBack;
		getSceneObject()->SetMatrix( m_pTransformDummy->GetMatrix());	
		
	}

}
void WeaponBase::SetCurrentAttachHand(void)
{
	/*if( getOwner()->getWeaponContext()->getBackpack() != this && 
		getOwner()->getWeaponContext()->getSidepack() != this &&
		getOwner()->GetCurrentCharaWeapon() == this)*/

	/*I3ASSERT( getOwner()->getWeaponContext()->getBackpack() != this && 
		getOwner()->getWeaponContext()->getSidepack() != this );*/

	if( getOwner() == nullptr)
		return;

	WeaponBase * pWeapon = getOwner()->GetCurrentCharaWeapon();
	if( pWeapon == this)
	{
		ChangeAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND, WEAPON_ATTACH_GRIP);
	}
}

void WeaponBase::SetCurrentAttachLeftHand(void)
{
	if( getOwner() == nullptr)
	{
		//I3PRINTLOG(I3LOG_NOTICE,"/*- Exception process -*/");
		return;
	}

	WeaponBase * pWeapon = getOwner()->GetCurrentCharaWeapon();
	if( pWeapon == this)
	{
		ChangeAttachHand( CHARA_WEAPON_GRIP_LEFT_HAND, WEAPON_ATTACH_GRIP);
	}
}

// KEYWORD : WEAPON_DUMMY
bool WeaponBase::IsLoadMagazineLeftBarrel( void)
{	

	if( EnablePlayMasterAnim() && IsFastReloadAnimWeapon() )
	{
		return false;
	}

	bool bRv = (m_pWeaponInfo->IsLoadMagazineLeftBarrel()==true);

	// ��ź �߻�� ������
	if( m_pWeaponInfo->availableExtType( WEAPON::EXT_GLAUNCHER))
	{
		if( !isExtensionActivate() && bRv )	bRv = true;
		else								bRv = false;
	}

	return bRv;
}

// KEYWORD : WEAPON_DUMMY
bool WeaponBase::IsLoadBulletLeftBarrel( void)
{	// true : �븮�� �������� �޼� ���̿� �ٴ� ����
	return getWeaponInfo()->IsLoadBulletLeftBarrel();
}

bool WeaponBase::IsExtensionWeapon( void)
{ 
	CWeaponInfo * pWeaponInfo = getAbsoluteWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	if( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_KNIFE)
	{
		if( pWeaponInfo->IsThrowWeapon())
			return true;
		if( pWeaponInfo->availableExtType( WEAPON::EXT_THROW))
			return true;
	}

	return pWeaponInfo->isExtensible();
}

bool WeaponBase::IsFireBulletWeapon( void)
{	// �߻��� ��Ʈ�׼� ������ ���� ���� ��� 
	// WEAPON::STATE_NEEDTO_LOADBULLET���� LoadBullet ����� ����ؾ� �� ��� ���⼭ false �̿��� LoadBullet ����� ����Ѵ�.
	if( getAIIndex( WEAPON::AI_FIREBULLET1PV_MALE) != -1 || getAIIndex( WEAPON::AI_FIREBULLET3PV_MALE) != -1 ||
		getAIIndex( WEAPON::AI_FIREBULLET1PV_FEMALE) != -1 || getAIIndex( WEAPON::AI_FIREBULLET3PV_FEMALE) != -1)
	{
		return true;
	}

	return false;
}

void WeaponBase::_QueContactReport( const I3_PHYSIX_CONTACT_REPORT_PTR& spReport )
{
//	UINT32 nCount = m_nContactReportCount - m_nContactReportReadPos;
	if( m_vecContactReport.size() >= PHYSIX_CONTACT_REPORT_COUNT )
	{
		//I3TRACE( "WeaponBase::_QueContactReport() - Contact report buffer over flow.\n" );
		return;
	}
	
	m_vecContactReport.push_back(spReport);

//	UINT32 i = m_nContactReportCount % PHYSIX_CONTACT_REPORT_COUNT;
//	i3Physix::ContactReportCopyTo( &m_ContactReportBuf[ i ], pReport );
//	m_nContactReportCount++;
}

void WeaponBase::_ParseContactReport( void )
{
	UINT32 i, j;
	const UINT32 nCount = (UINT32)m_vecContactReport.size();	// m_nContactReportCount - m_nContactReportReadPos;

	if( nCount == 0 )	return;

	if( getNetworkIdx() == -1 )
		return;

	for( i = 0; i < nCount; i++ )
	{
//		UINT32 nPos = (m_nContactReportReadPos + i) % PHYSIX_CONTACT_REPORT_COUNT;
//		I3_PHYSIX_CONTACT_REPORT * pReport = &m_ContactReportBuf[ nPos ];
		I3_PHYSIX_CONTACT_REPORT* pReport = m_vecContactReport[i].get();

		switch( pReport->m_Events)
		{
		case NX_NOTIFY_ON_END_TOUCH :
			{
				m_tDropInfo._bContact = false;
			
				GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( getNetworkIdx() );
				if( pInfo->_pWeaponRef == nullptr )	continue;
				if( pInfo->getState() == NET_WEAPONDROPED_NONE )		continue;

				pInfo->setState( NET_WEAPONDROPED_FLY);
			}
			break;
		
		case NX_NOTIFY_ON_START_TOUCH :
			if( pReport->m_ContactList.size() > 0)
			{
				GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( getNetworkIdx() );
				if( pInfo->_pWeaponRef == nullptr )
					continue;
				if( pInfo->getState() == NET_WEAPONDROPED_NONE )
					continue;

				pInfo->setState( NET_WEAPONDROPED_BOUND);
				pInfo->setDroped( true);

				for( j = 0; j < pReport->m_ContactList.size(); j++ )
				{
					I3_PHYSIX_CONTACT_INFO * pPointInfo = pReport->m_ContactList.at(j);

					if( !m_tDropInfo._bContact && ( i3Vector::Dot( &pPointInfo->m_Normal, &I3_YAXIS ) > I3_EPS + 0.3f ))
					{
						m_tDropInfo._bContact = true;
						m_tDropInfo._bDroped = true;

						i3EventReceiver * pReceiver = pReport->m_pDestShapeSet->getEventReceiver();
						if( i3::kind_of<i3Object*>(pReceiver))
						{
							i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( ((i3Object*)pReceiver)->getSceneObject(), i3Transform2::static_meta() );
							if( (pTrans != nullptr) && pTrans->IsAnimated())
							{
								m_tDropInfo._bContact = false;
							}
						}
					}
				}
			}
			break;
		}
	}

	m_vecContactReport.clear();

//	m_nContactReportReadPos += nCount;
}

bool WeaponBase::OnEvent( UINT32 event, i3ElementBase *pObj, UINT32 param, I3_EVT_CODE code)
{	
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		switch( event )
		{
		case I3_EVT_CONTACT :
			if( i3::same_of<i3PhysixShapeSet*>(pObj))
			{
				i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pObj;
				
				for( INT32 i = 0; i < pShapeSet->getContactCount(); i++ )
				{
					const I3_PHYSIX_CONTACT_REPORT_PTR& spReport = pShapeSet->getContactReport( i );
					
					_QueContactReport( spReport );
				}

				return true;
			}
			break;
		}
	}

	return i3GameObj::OnEvent( event, pObj, param, code);
}

// Contact report
void WeaponBase::CreateContactHitBox( INT32 nUserIndex)
{	
	MATRIX mat;
	VEC3D vDef, vSize;
	INT32 i;

	if( !BattleServerContext::i()->IsP2PActivate() )
		return;

	if( getWeaponInfo() != nullptr && getWeaponInfo()->isNotNeedContactBox())
		return;

	i3Stage * pStage = g_pFramework->GetCurrentStage();
	i3World * pWorld = pStage->getWorld();
	I3ASSERT( pWorld != nullptr );

	i3Skeleton * pSkel = m_pBody->getFrameLOD()->getSkeleton();
	I3ASSERT( pSkel != nullptr );

	i3Matrix::SetPos( &mat, (REAL32)nUserIndex, 10100.f, (REAL32) getWeaponInfo()->GetTypeClass(), 1.f);

	m_pContactHitBox = i3PhysixShapeSet::new_object();

	for( i = 0; i < pSkel->getBoneCount(); i++)
	{
		if( pSkel->isShapeSet( i) == false) continue;
		i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
		if( pShapeSet == nullptr)	continue;

		NxShape * pShape = pShapeSet->getNxShape(0);
		const NxConvexShape * pConvexShape = pShape->isConvexMesh();
		if( pConvexShape != nullptr)
		{
			UINT32 nCount = pConvexShape->getConvexMesh().getCount( 0, NX_ARRAY_VERTICES);
			VEC3D * pVerts = (VEC3D*) pConvexShape->getConvexMesh().getBase( 0, NX_ARRAY_VERTICES );

			for( UINT32 j = 0; j < nCount; j++)
			{
				if( getX( &pVerts[j]) > getX( &vSize))
				{
					setX( &vSize, getX( &pVerts[j]));
				}

				if( getY( &pVerts[j]) > getY( &vSize))
				{
					setY( &vSize, getY( &pVerts[j]));
				}
				
				if( getZ( &pVerts[j]) > getZ( &vSize))
				{
					setZ( &vSize, getZ( &pVerts[j]));
				}
			}
		}
	}

	i3Vector::Add( &vSize, 0.01f, 0.01f, 0.01f );
	
	m_pContactHitBox->CreateBoxShape( &vDef, &vSize, CGRP_ITEM, true, &mat);
	m_pContactHitBox->modifyDynamicState( I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_NO_RAYCAST | I3_PHYSIX_STATE_NO_COLLISION, 0);

#if defined( I3_DEBUG)
	m_pContactHitBox->SetName( "WeaponContactShapeSet" );
	m_pContactHitBox->getNxShape(0)->setName( "WeaponContactShape");
#endif
	m_pContactHitBox->setEventReceiver( this);

	i3Scene::SetActorPairFlag( pWorld->getWorldCollision()->getInstanceSg(), m_pContactHitBox->getNxActor(), NX_NOTIFY_ON_START_TOUCH );

	if( pStage->GetStageObjectList() != nullptr )
	{
		for( i = 0; i < (INT32)pStage->GetStageObjectList()->size(); i++ )
		{
			i3Object * pObj = (*pStage->GetStageObjectList())[i];
			i3Scene::SetActorPairFlag( pObj->getSceneObject(), m_pContactHitBox->getNxActor(), NX_NOTIFY_ON_START_TOUCH );
		}
	}
}

REAL32 WeaponBase::getCrosshairSpeedFactor(void)
{
	CWeaponInfo * pInfo = GetCurrentWeaponInfo();
	if( pInfo->isEnableFastZoom())
	{
		// Normal ���¿��� Zoom ���·� �ٲ� ����, �ð��� ����� �����ٸ�
		// SpeedFactor�� ������ ������ �������� �÷��ش�.

		if( isWeaponStateMask( WSM_FIRST_ZOOM))
			return 0.0f;
	}
	return pInfo->GetCrossHairSpeedFactor();
}

REAL32	WeaponBase::getCrossHairDeviationTargetTime(CWeaponInfo * pInfo)
{
	// m_pWeaponInfo�� ������� �ʰ� WeaponInfo�� ���� �޴� ������
	// Extension���� �����ε� �մϴ�.
	I3ASSERT(pInfo != nullptr);

	// ���߹��� ��ü �� m_pOwner�� NULL�� ��� �߻�
	//I3ASSERT(m_pOwner != nullptr);

	REAL32 SpeedFactorEx = 0.f;
	
	if ( m_pOwner != nullptr)
		SpeedFactorEx = m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_CROSSHAIR_SPEEDFACTOR);

	if ( SpeedFactorEx > 0.f)
		return getCrosshairSpeedFactor() * pInfo->GetCrossHairContractTime() * SpeedFactorEx;
	else
		return getCrosshairSpeedFactor() * pInfo->GetCrossHairContractTime();
}

bool WeaponBase::getScopeCrosshairEnable(void)
{
	if( m_pWeaponInfo->isEnableFastZoom())
	{
		// Zoom ���� ���� ��Ȳ���� ó�� Zoom�� �� ���� Crosshair�� ǥ������ �ʴ´�.
		if( isWeaponStateMask( WSM_FIRST_ZOOM))
			return false;
	}

	return true;
}

INT32 WeaponBase::getAutoPointShotCount(void)
{
#if defined( MEM_ENCRYPTION_LV1)
	return DECRYPT_INTVALUE(m_AutoPointShotCount, m_u8EncryptKey);
#else
	return m_AutoPointShotCount;
#endif
}

void WeaponBase::setAutoPointShotCount(INT32 count)
{
#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::EncryptValue(&count, m_u8EncryptKey);
#endif

	m_AutoPointShotCount = count;
}

//======================================================================================================//
// ��ȣȭ 2010.08.11 ����
INT32 WeaponBase::getAutoFireCount( void)
{
	return m_AutoFireCount;
}

void WeaponBase::setAutoFireCount( INT32 count)
{
	m_AutoFireCount = count;
}

INT32 WeaponBase::getRemainAutoFireCount(void)
{
#if defined( MEM_ENCRYPTION_LV1)
	return DECRYPT_INTVALUE(m_RemainAutoFireCount, m_u8EncryptKey);
#else
	return m_RemainAutoFireCount;
#endif
}

void WeaponBase::setRemainAutoFireCount(INT32 count)
{
#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::EncryptValue(&count, m_u8EncryptKey);
#endif

	m_RemainAutoFireCount = count;
}

/*virtual*/ INT32 WeaponBase::getLoadedBulletDualCount( void)
{
	return static_cast<INT32>(m_nLoadedBulletDual);
}

/*virtual*/ void WeaponBase::setLoadedBulletDualCount( INT32 nCount )
{
	REAL32 temp = static_cast<REAL32>(nCount);

	m_nLoadedBulletDual = temp;
}

INT32 WeaponBase::getLoadedBulletCount(void)
{
	return static_cast<INT32>(m_nLoadedBullet);
}

void WeaponBase::setLoadedBulletCount( INT32 nCount )
{
	REAL32 temp = static_cast<REAL32>(nCount);

	m_nLoadedBullet = temp;
}

INT32 WeaponBase::getTotalBulletCount(void)
{
	return m_nMaxBullet;
}

void WeaponBase::setTotalBulletCount( INT32 nCount )
{
	m_nMaxBullet = nCount;
}

///////////////////////REAL32������ ����ϴ�. 
/*virtual*/ REAL32 WeaponBase::getLoadedBulletDualCountF( void)
{
	return m_nLoadedBulletDual;
}

/*virtual*/ void WeaponBase::setLoadedBulletDualCountF( REAL32 nCount )
{
	m_nLoadedBulletDual = nCount;
}

REAL32 WeaponBase::getLoadedBulletCountF(void)
{
	return m_nLoadedBullet;
}

void WeaponBase::setLoadedBulletCountF( REAL32 nCount )
{
	m_nLoadedBullet = nCount;
}


REAL32 WeaponBase::getDamageRate( void)
{
	return m_DamageRate;
}

void WeaponBase::setDamageRate( REAL32 rDamage)
{	
	m_DamageRate = rDamage;
}

void WeaponBase::setZoom( bool bFlag)
{
	if( bFlag)
	{
		removeWeaponStateMask( WSM_FIRST_ZOOM);

		if( isWeaponStateMask( WSM_ZOOM) == false)
		{
			if( m_timeAfterZoom > 1.0f)
			{
				addWeaponStateMask( WSM_FIRST_ZOOM);
			}
		}

		addWeaponStateMask( WSM_ZOOM);
		_RemoveIdleEffect();
	}
	else
	{
		removeWeaponStateMask( WSM_ZOOM);
	}	
}

void WeaponBase::_OnEmitFireEffect( void)
{
	//	�߻� Effect
	if( m_pOwner->is1PV() && m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
	{	// 1��Ī ���� ����Ʈ�� ���
		_FireEffect( getFXMatrix(), getFireEffectIndex1PVZoom());
	}
	else
	{
		bool bAttachWorld = false;

		_FireEffect( getFXMatrix(), getFireEffectIndex(), bAttachWorld);
	}
}

void WeaponBase::_OnPlayFireSound()
{
	if( m_pOwner->is1PV() && m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM) && getWeaponInfo()->GetTypeZoom() == WEAPON::ZOOM_SCOPE)
		PlayFireSoundByScope();
	else 
		PlayFireSoundByOrder( getFireOrder());
}

INT32 WeaponBase::_OnProcessFireBulletCount( void)
{
	PB_VM_S

	INT32 bullet = getLoadedBulletCount();

	bullet -= m_pWeaponInfo->GetFireDecBulletCount();

	if( bullet < 0)
		bullet = 0;

	if( getWeaponInfo()->GetGaugeReload() == 1)
	{
		if(bullet == 0)
			SetEndurance(0);
	}

	setLoadedBulletCount( bullet);

	return bullet;

	PB_VM_E
}

void WeaponBase::PlayFireSoundByOrder( WEAPON::FIRE_ORDER type /*= WEAPON::PRIMARY_FIRE*/)
{
	// xgkick
	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_MISSION )
		return;

	if( type == WEAPON::SECONDARY_FIRE )
	{
		_PlayFireSound( WEAPON::FIRE_SOUND_NORMAL_2);	//	SECONDARY ����
	}
	else
	{
		_PlayFireSound( WEAPON::FIRE_SOUND_NORMAL);		//	�Ϲ� ����
	}
}

void WeaponBase::PlayFireSoundByScope(void)
{
	// xgkick
	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_MISSION )
		return;

	_PlayFireSound( WEAPON::FIRE_SOUND_SCOPE);			//	���� ����
}


void WeaponBase::_OnAddTerrainEffect( WEAPON_TERRAIN_EFFECT_INFO * pInfo)
{
	I3ASSERT( pInfo->_pPos != nullptr);
	I3ASSERT( pInfo->_pNormal != nullptr);
	I3ASSERT( pInfo->_pDir != nullptr);

	I3ASSERT_RETURN(m_pWeaponInfo);
	
	if(ObjHitEffIdx == -1) // ���� �Լ������� ũ����Ʈ �Լ� �̻��ѵ��� ���̸� HIT EFF IDX �ε� �ȵɼ����ִ°�� ����ó��
	{
		//��Ʈ ����Ʈ
		i3::rc_wstring	wsObjHitEffect;
		m_pWeaponInfo->GetObjHitEffect(wsObjHitEffect);
		i3::string strEffect;	
		i3::utf16_to_mb(wsObjHitEffect, strEffect);
		ObjHitEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
		if(ObjHitEffIdx == -1)
			ObjHitEffIdx = -2;


		i3::rc_wstring	wsCharaHitEffect;
		m_pWeaponInfo->GetCharaHitEffect(wsCharaHitEffect);
		i3::utf16_to_mb(wsCharaHitEffect, strEffect);
		CharaHitEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
		if(CharaHitEffIdx == -1)
			CharaHitEffIdx = -2;

		i3::rc_wstring	wsCharaHitEffect1PV;
		m_pWeaponInfo->GetCharaHitEffect1PV(wsCharaHitEffect1PV);
		i3::utf16_to_mb(wsCharaHitEffect1PV, strEffect);
		CharaHitEff1PVIdx = g_pEffectManager->FindEffect(strEffect.c_str());
		if(CharaHitEff1PVIdx == -1)
			CharaHitEff1PVIdx = -2;

		i3::rc_wstring	wsDecalEffectExt;
		m_pWeaponInfo->GetDecalEffectExt(wsDecalEffectExt);
		i3::utf16_to_mb(wsDecalEffectExt, strEffect);
		DecalEffExtIdx = g_pEffectManager->FindEffect(strEffect.c_str());
		if(DecalEffExtIdx == -1)
			DecalEffExtIdx = -2;

		i3::rc_wstring	wsIdleEffect;
		m_pWeaponInfo->GetIdleEffect(wsIdleEffect);
		i3::utf16_to_mb(wsIdleEffect, strEffect);
		IdleEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
		if (IdleEffIdx == -1)
			IdleEffIdx = -2;

		if(m_pWeaponInfo->GetUnBlowEffFlag() != 0)
			UnBlowHitEffFlag = true;

		if(m_pWeaponInfo->GetUnDecalEffFlag() != 0)
			UnDecalHitEffFlag = true;

	}

	T_ItemID itemID = m_pWeaponInfo->GetItemID();

	BULLET_EFFECT_TYPE effectType = static_cast<BULLET_EFFECT_TYPE>(m_pWeaponInfo->GetLoadBulletType());

	if (8209011 <= itemID && itemID <= 8209014)
	{
		g_pEffectManager->AddTerrainEffect( pInfo->_terrainType, WEAPON_CLASS_ASSAULT,
			effectType, pInfo->_pPos, pInfo->_pNormal, pInfo->_pDir, pInfo->_rRotate, pInfo->_pObj, UnBlowHitEffFlag, UnDecalHitEffFlag );
	}
	else
	{
		if ( getFireOrder() == WEAPON::PRIMARY_FIRE )
		{
			g_pEffectManager->AddTerrainEffect( pInfo->_terrainType, m_pWeaponInfo->GetTypeClass(),
				effectType, pInfo->_pPos, pInfo->_pNormal, pInfo->_pDir, pInfo->_rRotate, pInfo->_pObj, UnBlowHitEffFlag, UnDecalHitEffFlag );
		}
		else
		{	// revision 60217 Į ���� ���� �ѱ� ( Weapon With Knife ) ���� ���� �ø��� ����Ʈ ���⵵�� ( Į ����Ʈ )
			g_pEffectManager->AddTerrainEffect( pInfo->_terrainType, WEAPON_CLASS_KNIFE,
				effectType, pInfo->_pPos, pInfo->_pNormal, pInfo->_pDir, pInfo->_rRotate, pInfo->_pObj, UnBlowHitEffFlag, UnDecalHitEffFlag );
		}

		if(DecalEffExtIdx > -1)
		{
			g_pEffectManager->AddDecal( DecalEffExtIdx, pInfo->_pPos, pInfo->_pNormal, pInfo->_pDir, pInfo->_rRotate, pInfo->_pObj);
		}
		if(ObjHitEffIdx > -1)
		{
			g_pEffectManager->AddEffect( ObjHitEffIdx, pInfo->_pPos, pInfo->_pNormal);
		}
		
	}
}

REAL32 WeaponBase::GetPhysixForce( WEAPON_CLASS_TYPE classType)
{
	PHYSIXINFODATA_WEAPON * pPhysixData = &g_pPhysixInfoDataBase->getInfo()->m_WeaponDeath;
	REAL32 rForce = 1.f;

	switch( classType )
	{
	case WEAPON_CLASS_KNIFE			:	rForce *= pPhysixData->m_rWeapon_Knife * 2.5f;	break;
	case WEAPON_CLASS_KNUCKLE		:	rForce *= pPhysixData->m_rWeapon_Knife * 2.5f;	break;
	case WEAPON_CLASS_HANDGUN		:	rForce *= pPhysixData->m_rWeapon_Pistol;		break;
	case WEAPON_SUBCLASS_ASSAULT	:
	case WEAPON_CLASS_ASSAULT		:	rForce *= pPhysixData->m_rWeapon_Assult;		break;
	case WEAPON_SUBCLASS_SMG		:
	case WEAPON_CLASS_SMG			:	rForce *= pPhysixData->m_rWeapon_SMG;			break;
	case WEAPON_SUBCLASS_SNIPER		:
	case WEAPON_CLASS_SNIPER		:	rForce *= pPhysixData->m_rWeapon_Rifle;			break;
	case WEAPON_SUBCLASS_SHOTGUN	:
	case WEAPON_CLASS_SHOTGUN		:	
	case WEAPON_CLASS_DUALSHOTGUN	:	rForce *= pPhysixData->m_rWeapon_Shotgun;		break;
	case WEAPON_CLASS_MG			:	rForce *= pPhysixData->m_rWeapon_Assult;		break;
	case WEAPON_SUBCLASS_BOW		:
	case WEAPON_CLASS_BOW			:	rForce *= pPhysixData->m_rWeapon_Assult;		break;		
	case WEAPON_CLASS_DUALKNIFE		:	rForce *= pPhysixData->m_rWeapon_Knife * 2.5f;	break;
	case WEAPON_CLASS_DUALHANDGUN	:	rForce *= pPhysixData->m_rWeapon_Pistol;		break;
	default :	break;
	}

	return rForce;
}

void WeaponBase::ResetCommand( void)
{
	m_nPushCommandCount = m_nPopCommandCount = 0;
}

INT32 WeaponBase::CommandCount( void)
{
	INT32 nCount = m_nPushCommandCount - m_nPopCommandCount;
	
	return nCount;
}

void WeaponBase::PushCommnad( INT32 nCmd)
{
	if( m_nPushCommandCount - m_nPopCommandCount > MAX_WEAPON_COMMAND_COUNT)
		return;

	INT32 nCur = m_nPushCommandCount % MAX_WEAPON_COMMAND_COUNT;
	m_nCommandBuf[ nCur] = nCmd;
	m_nPushCommandCount++;
}

INT32 WeaponBase::PopCommand( void)
{
	INT32 nCur = m_nPopCommandCount % MAX_WEAPON_COMMAND_COUNT;
	m_nPopCommandCount++;
	return m_nCommandBuf[ nCur];
}

void WeaponBase::GetNetBullet( GAMEINFO_BULLET* pBullet )
{
	pBullet->Reset();
	pBullet->SetLoadBulletCount(	WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT,	(UINT8)getLoadedBulletCount() );
	pBullet->SetStandbyBulletCount( WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT,	(UINT16)getTotalBulletCount( ) );
	pBullet->SetLoadBulletCount(	WEAPON::EXTENSION_ON,	WEAPON::HAND_TYPE_LEFT,	(UINT8)getLoadedBulletDualCount( ) );
}

void WeaponBase::SetNetBullet( GAMEINFO_BULLET* pBullet )
{
	setLoadedBulletCount( pBullet->GetLoadBulletCount(		WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT ) );
	setTotalBulletCount( pBullet->GetStandbyBulletCount(	WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT ) );
	setLoadedBulletDualCount( pBullet->GetLoadBulletCount(	WEAPON::EXTENSION_ON,	WEAPON::HAND_TYPE_LEFT ) );
}

REAL32 WeaponBase::GetMoveSpeed( WEAPON_MOVE_SPEED_TYPE type)
{
	REAL32 rVal = 0.f;
	CWeaponInfo * pWeaponInfo = m_pWeaponInfo;

	if( isExtensionActivate())
	{
		I3ASSERT( this->getUsedWeapon() != nullptr);
		pWeaponInfo = this->getUsedWeapon()->getWeaponInfo();
	}

	if( i3::kind_of<WeaponGrenadeLauncher*>(this) )
	{
		if( GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER && isExtensionActivate() )
			pWeaponInfo = getExtensionWeaponInfo();
	}

	switch( type)
	{
	case WEAPON_MOVE_SPEED_NORMAL :			rVal = pWeaponInfo->GetMoveNormalStand();		break;
	case WEAPON_MOVE_SPEED_FIRE :			rVal = pWeaponInfo->GetMoveNormalFire();		break;
	case WEAPON_MOVE_SPEED_SIT :			rVal = pWeaponInfo->GetMoveNormalSit();			break;
	case WEAPON_MOVE_SPEED_ZOOM:			rVal = pWeaponInfo->GetMoveNormalZoom();		break;
	}

	if( rVal < 1.f)
		rVal = 1.f;
	
	// ȣĪ weaponMoveSpeed ����
	if (m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
		if (desig_apply_info )
			rVal += desig_apply_info->desigWeapon.weaponMoveSpeed;
	}
	
	return rVal;
}

REAL32 WeaponBase::GetFireDelay()
{
	// FireDelay�� ���� ��ġ�� ����ǵ� ������ �ȵ˴ϴ�.
	// ȭ�� �ֻ����� ��Ʈ��ũ�� ������ �ֱ� �����Դϴ�.
	// ���� ��� ��Ŷ�� �������Ӵ� ������ �ǰ�
	// ȭ�� �ֻ����� �ʴ� 60�������� �������� �Ѵٸ� 0.015�ʴ� �ѹ���
	// FireDelay�� ���ؼ� �˻縦 �ϱ⶧���� 0.001�� ���� ���� ��ġ�� �ݿ��� �ȵ˴ϴ�.
	if( getPairWeapon() != nullptr
		&& getPairWeapon()->getWeaponInfo()->UsePairData() == true )
	{
		return getPairWeapon()->GetFireDelay();
	}

	REAL32 rVal = 0.f;
	
	// revision 35995  ��ī ���������� ��� �ܻ��¿� �Ϲݻ����� �������� �ٸ��� ������
	if (m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SNIPER &&
		m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::SNIPER_SCAR_LB))
	{
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			rVal = m_pWeaponInfo->GetFireDelay();
		else
			rVal = m_pWeaponInfo->GetExtFireDelay();

		//
		// rVal = _CalcFireDelayForDesignation(rVal);	// ȣĪ�� ���� �߻� �����ð�. ( �߰��� �ڵ�)
		// 12�� ������ SCAR �������ۿ��� �ܻ��°� �ƴϸ� Ư��ġ���� ���� WeaponInfo->GetFireDelay()�� �ٷ� ���ϵǰ�..
		// �� ������ ��쿡�� ȣĪ Ư��ġ ���븸 2��� �����....   ( ������� �������� �˼��� ��� �̰� �̴�� ��ġ�մϴ�.)
		// (2015.05.22.����)
		//
	}
	else
	{
		rVal = m_pWeaponInfo->GetFireDelay();
	}
	
	return _CalcFireDelayToVersion(rVal);
}

REAL32 WeaponBase::GetFireDelay2()
{
	// FireDelay�� ���� ��ġ�� ����ǵ� ������ �ȵ˴ϴ�.
	// ȭ�� �ֻ����� ��Ʈ��ũ�� ������ �ֱ� �����Դϴ�.
	// ���� ��� ��Ŷ�� �������Ӵ� ������ �ǰ�
	// ȭ�� �ֻ����� �ʴ� 60�������� �������� �Ѵٸ� 0.015�ʴ� �ѹ���
	// FireDelay�� ���ؼ� �˻縦 �ϱ⶧���� 0.001�� ���� ���� ��ġ�� �ݿ��� �ȵ˴ϴ�.
	REAL32 rVal = m_pWeaponInfo->GetFireDelay2();

	return _CalcFireDelayToVersion(rVal);
}


REAL32 WeaponBase::GetReloadTime()
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	REAL32 rVal = m_pWeaponInfo->GetReloadTime();

	// Dual ������ ��� ������ ������ ���� (PairWeapon)�� �Ѵ�.
	if( getPairWeapon() != nullptr)
		rVal = getPairWeapon()->getWeaponInfo()->GetReloadTime();

	return _CalcReloadTimeToVersion(rVal);
}

REAL32 WeaponBase::GetRecoilHorzAngle()
{	
	REAL32 fRate = 0.0f;
	REAL32 rVal = m_pWeaponInfo->GetRecoilHorzAngle();
	if (m_pOwner == nullptr)
	{
		return rVal;
	}
	
	// ȣĪ ���� ���θ� �� �𸣰ڴ�....���� ȣĪ���� ���� �ѵ�...�ϴ� �����س��´�...( 0.01�� ���ϸ� �ʹ� �۰�, �׷��� ���� ��� �ټ� ū���̶� �����̱� �ϴ�..)
	if (m_pOwner)
	{
		const NET_CHARA_INFO* netCharaInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType() );
		if (netCharaInfo->GetDesigApplyInfo())
		{
			fRate = netCharaInfo->GetDesigApplyInfo()->desigWeapon.weaponHorzReaction;
			fRate += netCharaInfo->GetDesigApplyInfo()->desigWeapon.weaponReaction * 0.01f;
		}
	}

	rVal += (rVal * fRate); //*0.01f);

#if defined( USE_WEAPON_ENDURANCE)
	if( UserInfoContext::i()->IsNewRepair() )
	{
		if( !getOwner()->isAI())
			CalEndurancePenalty( PENALTY_HORZRECOIL, m_ui8Endurance, rVal );
	}
	else
	{
		if( IsEnduranceZero() && !getOwner()->isAI() )
			rVal = CGameWeaponManager::WeaponEnduranceZeroHorzRecoil( rVal);
	}
#endif

	return rVal;
}


REAL32 WeaponBase::GetRecoilVertAngle()
{
	REAL32 fRate = 0.0f;
	REAL32 rVal = m_pWeaponInfo->GetRecoilVertAngle();
	if( m_pOwner == nullptr)
	{
		return rVal;
	}

	// ȣĪ ���� ���θ� �� �𸣰ڴ�....�ϴ� �����س��´�...���� ȣĪ���� ���� �ѵ�...�ϴ� �����س��´�...( 0.01�� ���ϸ� �ʹ� �۰�, �׷��� ���� ��� �ټ� ū���̶� �����̱� �ϴ�..)
	if (m_pOwner)
	{
		const NET_CHARA_INFO* netCharaInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType() );
		if (netCharaInfo->GetDesigApplyInfo())
		{
			fRate = netCharaInfo->GetDesigApplyInfo()->desigWeapon.weaponVertReaction;
			fRate += netCharaInfo->GetDesigApplyInfo()->desigWeapon.weaponReaction * 0.01f;
		}
	}

	rVal = rVal + (rVal * fRate);

#if defined( USE_WEAPON_ENDURANCE)
	if( UserInfoContext::i()->IsNewRepair() )
	{
		if( !getOwner()->isAI())
			CalEndurancePenalty( PENALTY_VERTRECOIL, m_ui8Endurance, rVal );
	}
	else
	{
		if( IsEnduranceZero() && !getOwner()->isAI() )
			rVal = CGameWeaponManager::WeaponEnduranceZeroVertRecoil( rVal);
	}
#endif

	return rVal;
}

REAL32 WeaponBase::GetDeviation()
{
	REAL32 rVal = m_pWeaponInfo->GetDeviation();

	if( m_pOwner != nullptr)
	{
		// ��ȹ ������, pef �ʱⰪ �������� ��ٸ� ���¿��� ����̼� ���� ���Ƽ ������ ���Ѵ�...
		
		if (m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB) )
		{
			rVal *= g_pConfigEx->GetLadderPenaltyRate_Deviation();			
		}

		// SMG���� ������ �Ÿ� ���Ͽ��� �ٸ� ĳ���͸� ������ ���¶��
		// ������ ���� ó���ؾ� �Ѵ�.
		if( m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SMG)
		{
			REAL32 factor = m_pWeaponInfo->GetDeviation_Decrement();

			if( factor > 0.0f)
			{
				if( (m_pOwner->getCollisionContext() != nullptr) && (m_pOwner->getCollisionContext()->getCrossHitDistance() > 0.0f))
				{
					REAL32 dist = m_pOwner->getCollisionContext()->getCrossHitDistance();

					if( dist <= m_pWeaponInfo->GetDeviation_DiminutionDistance())
					{
						rVal *= factor;
					}
				}
			}
		}
		
		rVal = _CalcDeviationToVersion(rVal);
		//I3TRACE( "Deviation %.4f + %.4f\n", m_pWeaponInfo->GetDeviation(), rVal - m_pWeaponInfo->GetDeviation());

		
#if defined( USE_WEAPON_ENDURANCE)
		if (UserInfoContext::i()->IsNewRepair())
		{
			if (!getOwner()->isAI())
				CalEndurancePenalty(PENALTY_DEVIATION, m_ui8Endurance, rVal);
		}
#endif

	}
	
	return rVal;
}

REAL32 WeaponBase::GetRange()
{
	if( getPairWeapon() != nullptr
		&& getPairWeapon()->getWeaponInfo()->UsePairData() == true )
	{
		return getPairWeapon()->GetRange();
	}

	REAL32 rVal = m_pWeaponInfo->GetRange();

	if( m_pOwner != nullptr )
	{
		// ��� �Ӽ�
		if ( m_pOwner->getEquipContext() )
			rVal += (rVal * m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_RANGE));

		// ��ų �Ӽ�.
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_RANGE);
		rVal += ratio;
		
		// ȣĪ ����
		const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
		if (desig_apply_info)
			rVal += desig_apply_info->desigWeapon.weaponAttackRange;
	}

	return rVal;
}

REAL32 WeaponBase::GetDPD_Rate()
{
	REAL32 rVal = m_pWeaponInfo->GetDPD_Rate();

	return rVal;
}

REAL32 WeaponBase::GetDamage( bool bRandomDamage)
{
	I3ASSERT_RETURN( m_pOwner != nullptr, 0.f);
	
	REAL32 AddDamage = 0.f;

	if(m_pOwner->getEquipContext() != nullptr)
	{
		if( !i3::same_of<WeaponWPSmoke*>(this) )	// revision 36503 // WP Smoke �迭�� ����ũ �������� �����Ű�� �ʽ��ϴ�.
			AddDamage = m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_DAMAGE);
	}

	REAL32 rVal =0.f;
	// 36005 ��ī �ͽ��ټ� ���½� ������ ���� �����ϵ��� ��
	if (m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SNIPER &&
		m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::SNIPER_SCAR_LB))
	{
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			rVal =	(REAL32) m_pWeaponInfo->GetDamage() + AddDamage;
		else
			rVal =	(REAL32) m_pWeaponInfo->GetExtDamage() + AddDamage;
	}
	else
	{
		rVal =	(REAL32) m_pWeaponInfo->GetDamage() + AddDamage;
	}

	REAL32 rBasisDamage = rVal;

	WEAPON_CLASS_TYPE nWeaponClass = m_pWeaponInfo->GetTypeClass();

	switch( nWeaponClass)
	{	
	// Į�� ���������� ������� �ʽ��ϴ�.
	case WEAPON_CLASS_KNIFE:
	case WEAPON_CLASS_DUALKNIFE:
	case WEAPON_CLASS_KNUCKLE:
		{						
			switch( m_FireOrder)
			{
			case WEAPON::PRIMARY_FIRE:
				{
					rVal = m_pWeaponInfo->GetDamageA( getAutoFireCount());		// ���� ���� ������
				}
				break;
			case WEAPON::SECONDARY_FIRE :
				{
					rVal = m_pWeaponInfo->GetDamageB( getAutoFireCount());		//	������ ���� ������
				}
				break;
			}
		} break;
	//	���߷� �Ǵ� ��ô���� ���� ���������� ����.
	case WEAPON_CLASS_THROWING_GRENADE:
	case WEAPON_CLASS_THROWING_CONTAIN:
	case WEAPON_CLASS_MISSION:
	case WEAPON_CLASS_GRENADESHELL:
	case WEAPON_CLASS_ROCKET_LAUNCHER:
	case WEAPON_CLASS_THROWING_ATTACK:
	case WEAPON_CLASS_THROWING_HEAL:
		{
			//	���� ��������...
		} break;
	////////////////////////////////////////////	�׿� �ѱ���� ��� RandomDamage ����.
	default:
		{
			if( m_pDamage)
				rVal = m_pDamage->Get( rVal, AddDamage, bRandomDamage);
		} break;
	}

	//	�Ѿ� �߻� �迭 ���⸸ Ammo ĳ�� �������� ����ǵ���
	if( (nWeaponClass == WEAPON_CLASS_CIC && m_FireOrder == WEAPON::PRIMARY_FIRE)	//	CIC �ϰ�� SECONDARY�� Knife �����̱� ������
		|| m_pWeaponInfo->isGun())
	{
		//������ ������ ����
		REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_DAMAGE);
		CASHITEM_TRACE("CASHITEMSKILL_DAMAGE = %f\n", ratio);
		rVal += (rBasisDamage * ratio);

		// PBTN Mask ����ó��. (V1���� �ߺ� ������̿����ϴ�.)
		{
			const CEquipInfo * pFaceInfo = m_pOwner->getEquipContext()->getEquipInfo(EQUIP::ePART_FACEGEAR);
			if (pFaceInfo != nullptr && pFaceInfo->GetITEMID() == 800169)
			{
				ratio = pFaceInfo->GetAbilityValue(EQUIP::eABILITY_DAMAGERATE);
				ratio += (ratio > 0 ? I3_EPS : -I3_EPS);
				rVal += (rBasisDamage * ratio);
			}
		}
	}

	if( m_pOwner->getCharaNetIndex() != -1)
	{	
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());

		//// designation ( V1�� �����ڵ�� ����. �ٸ�, ����ϴ� ȣĪ�� ���� �ּ� ó���մϴ�.)
		//{
		//	if (pNetInfo->GetDesigApplyInfo())
		//		rVal += pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponDamage;
		//}

		// skill
		{
			REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_DAMAGE);
			rVal += rVal * ratio;
		}

		if( m_pOwner->getEquipContext() != nullptr)
		{
			rVal += (rVal * m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_DAMAGERATE));
		}
	}

	//I3TRACE( "Damage %.4f + %.4f\n", m_pWeaponInfo->GetDamage(), rVal - m_pWeaponInfo->GetDamage());



#if defined( USE_WEAPON_ENDURANCE)
	if( UserInfoContext::i()->IsNewRepair() )
	{
		if( !getOwner()->isAI())
			CalEndurancePenalty( PENALTY_DAMAGE, m_ui8Endurance, rVal );
	}
	else
	{
		if( IsEnduranceZero() && !getOwner()->isAI())
			rVal = CGameWeaponManager::WeaponEnduranceZeroDamage( rVal);
	}
#endif

	return rVal;
}

REAL32 WeaponBase::GetScopeFOV()
{
	return m_pWeaponInfo->GetScopeFOV();
}

REAL32 WeaponBase::GetScopeFOV2()
{
	return m_pWeaponInfo->GetScopeFOV2();
}

REAL32 WeaponBase::GetThrowSpeed()
{
	I3ASSERT( m_pWeaponInfo != nullptr);
	REAL32 rVal = 0.f;
	rVal = m_pWeaponInfo->GetThrowSpeed();

	const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());

	REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_BOOMLENGTH);
	rVal += ratio; // rVal * ratio;			// ������ �ʰ� �״�� ���ϴ� ������ ����..(��ȹ ��û)..2015.05.22.����...

	if (pNetInfo->GetDesigApplyInfo() )
	{
		const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
		rVal += desig_apply_info->desigWeapon.weaponThrowSpeed;				// 12������������ �׳� ���ϰ� ���ִµ�..�� �ڵ� �״�� �ݿ���..
	}

	return rVal;
}

REAL32 WeaponBase::GetExThrowSpeed()
{
	CWeaponInfo * pWeaponInfo = m_pWeaponInfo;
	if( i3::kind_of<WeaponGrenadeLauncher*>(this) )
		pWeaponInfo = getExtensionWeaponInfo();

	REAL32 rVal = pWeaponInfo->GetExThrowSpeed();
	if( m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_BOOMLENGTH);
		rVal += ratio; // rVal * ratio;			// ������ �ʰ� �״�� ���ϴ� ������ ����..(��ȹ ��û)..2015.05.22.����...

//		if (pNetInfo->getSkillsystem(0) != nullptr)
//		{
//			rVal += pNetInfo->getSkillsystem(0)->getWeaponThrowSpeed();
//			rVal += pNetInfo->getSkillsystem(1)->getWeaponThrowSpeed();
//			rVal += pNetInfo->getSkillsystem(2)->getWeaponThrowSpeed();				
//		}
		
		if ( pNetInfo->GetDesigApplyInfo() )
		{
			const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
			rVal += desig_apply_info->desigWeapon.weaponThrowSpeed;				// 12������������ �׳� ���ϰ� ���ִµ�..�� �ڵ� �״�� �ݿ���..
		}

	}
	return rVal;
}

REAL32 WeaponBase::GetExplosionTime()
{
	REAL32 rVal = m_pWeaponInfo->GetExplosion_Time();

	if( m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_BOOMTIME);
		rVal -= rVal * ratio;
		
		// ȣĪ ����...
		if (pNetInfo->GetDesigApplyInfo())
			rVal += pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponBombTime;
	}

	return MINMAX(0.f, rVal, 100.f);
}

REAL32 WeaponBase::GetExplosionRange()
{
	REAL32 rVal = m_pWeaponInfo->GetExplosion_Range();

	if( m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_BOOMRANGE);
		rVal += ratio;

		//ȣĪ ����..
		if (pNetInfo->GetDesigApplyInfo())
			rVal += pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponBombRange;
	}

	I3ASSERT( rVal > 0.f);
	return rVal;
}

REAL32 WeaponBase::GetExplosionNearRange()
{
	return m_pWeaponInfo->GetExplosion_NearRange();
}

REAL32 WeaponBase::GetDetectedRange()
{
	return m_pWeaponInfo->GetDetected_Range();
}

INT32 WeaponBase::GetMaxBulletWithCashItem(void)
{
	CWeaponInfo * pInfo = getWeaponInfo();
	I3ASSERT( pInfo != nullptr);

	INT32 count = pInfo->GetMaxBullet();
	REAL32 ability = 1.0f;

	// ź�� ���� ���� �����ۺ��� ��� �� ���� �ɷ�ġ ���
	if( m_pOwner != nullptr)
	{
		// ����ź�� �������� �ִ����� Ȯ���Ͽ� ����ź�� 40% ���� (�ڽ� ���)
		if(	BattleSlotContext::i()->isUseCashItemInSlot( m_pOwner->getCharaNetIndex(), SLOT_ITEM_AMMO_UP) == true )
		{
			// �ֹ����϶��� ����
			if( m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_PRIMARY )
			{
				// �Ϲ� KrissS.V.���� ���� ��ȯ�Ǵ� Dual KrissS.V.�� ��ȹ�� ���뿡�� ����.
				if( !(i3::same_of<WeaponShotGunGenderWithDual*>(getPairWeapon()) == true || i3::same_of<WeaponKrissSuperV*>(getPairWeapon()) == true) )
				{				
					ability += WEAPON_BULLET_INCREASING_COUNT_40;
				}
			}
		}

		// Grenade ź�� ���� ������ ���
		if (pInfo->GetTypeUsage() == WEAPON_SLOT_THROWING1)
		{
			if ( m_pOwner && BattleSlotContext::i()->isUseCashItemInSlot( m_pOwner->getCharaNetIndex(), SLOT_ITEM_INCREASE_GRENADE_SLOT))
			{
				count += pInfo->GetIncreaseCount();
			}
		}
		else if (pInfo->GetTypeUsage() == WEAPON_SLOT_THROWING2)
		{
			if (m_pOwner && BattleSlotContext::i()->isUseCashItemInSlot(m_pOwner->getCharaNetIndex(), SLOT_ITEM_INCREASE_THR2_SLOT))
			{
				count += pInfo->GetIncreaseCount();
			}
		}

		// ���� �ɷ�ġ �߰�
		switch( m_pWeaponInfo->GetTypeClass() )
		{
			//case WEAPON_CLASS_HANDGUN :
		case WEAPON_SUBCLASS_ASSAULT	:
		case WEAPON_CLASS_ASSAULT		:
		case WEAPON_SUBCLASS_SMG		:
		case WEAPON_CLASS_SMG			:
		case WEAPON_SUBCLASS_SNIPER		:
		case WEAPON_CLASS_SNIPER		:
		case WEAPON_SUBCLASS_SHOTGUN	:
		case WEAPON_CLASS_SHOTGUN		:
		case WEAPON_CLASS_MG			:
			//case WEAPON_CLASS_CIC :
			{
				//count = count + (INT32) (m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_MAXBULLET) * count);
				ability += m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_MAXBULLET);
			}
			break;

			//case WEAPON_CLASS_DUALHANDGUN :
		case WEAPON_CLASS_DUALSMG :
			{
				// �Ϲ� KrissS.V.���� ���� ��ȯ�Ǵ� Dual KrissS.V.�� ��ȹ�� ���뿡�� ����.
				if( i3::same_of<WeaponKrissSuperV*>(getPairWeapon()))
					break;

				// ��󹫱�� �� ���⺰�� �������� ���. (��ü �߼� * 0.5 ����)
				INT32 nHarfCount = static_cast<INT32>(count * 0.5f);

				// ������
				ability += m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_MAXBULLET);

				// ������ ���⺰ ź����, ����� ���� (���⺰ ���� ź�� * 2 ����)
				count = static_cast<INT32>( nHarfCount * ability ) * 2;

				// Fix�� Bullet Count
				ability = 1.0f;
			}
			break;

		case WEAPON_CLASS_THROWING_GRENADE :
			{
				if( m_pWeaponInfo->GetDual())
					count = count + (INT32) (m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_THROWCOUNT) * 2);
				else
					count = count + (INT32) m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_THROWCOUNT);
			}
			break;

		default :	// �ٸ� ����� �������� ����.
			break;
		}
	}

	return static_cast<INT32>(count * ability);
}

INT32 WeaponBase::GetMaxBulletWithSkill(void)
{
	CWeaponInfo * pInfo = getWeaponInfo();
	I3ASSERT( pInfo != nullptr);
	INT32 count = pInfo->GetMaxBullet();

	if( m_pOwner && m_pOwner->isLocal())
	{
		// �ֹ����϶��� ����
		if( m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_PRIMARY )
		{
			if( i3::same_of<WeaponKrissSuperV*>(getPairWeapon()) )
			{
				// �Ϲ� KrissS.V.���� ���� ��ȯ�Ǵ� Dual KrissS.V.�� ��ȹ�� ���뿡�� ����.
			}
		}
	}


	return count;
}

INT32 WeaponBase::GetLoadBulletWithCashItem(void)
{
	CWeaponInfo * pInfo = getWeaponInfo();
	I3ASSERT( pInfo != nullptr);

	INT32 count = pInfo->GetLoadBullet();

	// ���忡 �ٴ� ����ź�� ��� �׳� ���� ������ ����
	if( m_pOwner != nullptr 
#ifdef DOMI_STING_POISON_GRENADE
		&& !(i3::kind_of<CGameCharaFullbody*>(m_pOwner))
#endif
		)
	{
		if( m_pOwner->getCharaNetIndex() == 0xffffffff)
			return count;

		//11766 ������, ����ź�� Max Count ���� +1 ���� ó���մϴ� 
		if( IsWeaponByIncreaseGrenadeSlot( pInfo->GetTypeClass(), pInfo->GetRealNumber()))
		{
			count += static_cast<INT32>(BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));
			CASHITEM_TRACE("CASHITEMSKILL_GRENADE_ADD = %f\n", BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));
		}
		else if( IsWeaponByIncreaseDualGrenadeSlot( pInfo->GetTypeClass(), pInfo->GetRealNumber()))
		{
			count += static_cast<INT32>(BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD))*2;
			CASHITEM_TRACE("CASHITEMSKILL_GRENADE_ADD = %f\n", BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));
		}
	}

	return count;
}

void WeaponBase::_OnProcessFireFirstAnim()
{
	// ���̾� �߻����� ���ϵ���...
	if( m_pOwner->isLocal() || m_pOwner->isAI())
	{
		// ������ �� �ٷ� Fire�� �ϰ� �Ǹ� ���⼭ �ʱ�ȭ�� �̷�� ���� ��.
		m_pOwner->ResetNetworkAttackCount();
		addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
	
		if( m_bSetStart )	FireCollision( m_StartPos, m_TargetPos);
		else				FireCollision( nullptr, m_TargetPos);

#if defined( FIREDELAY_COST)
		if( !isWeaponStateMask( WSM_MORE_FIRING))
		{
			m_rFireTime = 0.f;
		}

		removeWeaponStateMask( WSM_MORE_FIRING);
#endif
	}
	// �߻� ����Ʈ
	_OnEmitFireEffect();
	
	// �߻� ����
	_OnPlayFireSound();
	// źȯ�� ����մϴ�.
	// źȯ üũ �ڵ� �߰� 2010.09.07 ����
	INT32 bullet = getLoadedBulletCount();

	if( GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate())
	{
		bullet = ((WeaponShotGunLauncher*)this)->getLoadedShellCount();
	}
	else if( getMagazineType() == WEAPON_DUAL_MAGAZINE_RIGHT)
	{
		bullet = getLoadedBulletDualCount();
	}

	if( bullet <= _OnProcessFireBulletCount())
	{
		if( (bullet > 0) && (m_pOwner != nullptr) && (m_pOwner->isLocal()))
		{
			REAL32 rError = 51.f;
			UINT32 nRemainBattleTime = (UINT32)UserContext::i()->fRemainingBattleTime;

			GameEventSender::i()->SetEvent( EVENT_BATTLE_REMAINTIME_N, &nRemainBattleTime, &rError);
		}
	}

	// �߻� �� ��쿡 Load Mag�� �ߴٴ� flag�� ���ϴ�. �׷��� źâ�� �����ϴ�.
	removeWeaponStateMask(WSM_EMPTY_CARTRIDGE | WSM_LOADMAG_TO_NEEDBULLET);
}

void WeaponBase::_OnProcessFireEndAnim( REAL32 rDelay)
{
	I3ASSERT( m_pOwner != nullptr);
	I3ASSERT( m_pWeaponInfo != nullptr);

	if( m_pWeaponInfo->GetPumpAction())	//	�����׼� ����̸�
	{
		if( getLoadedBulletCount() > 0 )
		{
			addWeaponStateMask( WSM_LOADED_BULLET);
			removeWeaponStateMask( WSM_FIRE_DISABLE);

			// �߻��� ��Ʈ�׼� ������ ������
			if( IsFireBulletWeapon())  
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADBULLET);
				removeWeaponStateMask( WSM_LOADBULLET_DISABLE);			
			}
		}
		else
		{
			removeWeaponStateMask( WSM_LOADED_BULLET);
			if( (m_pOwner->isLocal() || m_pOwner->isAI()))	addWeaponStateMask( WSM_FIRE_DISABLE);
		}

		_OnPlayWeaponIdleAnim();
	}
	else
	{
		if( m_pWeaponInfo->GetNeedToLoadBullet() && (getLoadedBulletCount() > 0))  
		{
			setWeaponState( WEAPON::STATE_NEEDTO_LOADBULLET);
			removeWeaponStateMask( WSM_LOADBULLET_DISABLE);			
		}
		else
		{	
			if( m_pWeaponInfo->GetNeedToLoadBullet() && (getLoadedBulletCount() > 0))  
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADBULLET);
				removeWeaponStateMask( WSM_LOADBULLET_DISABLE);			
			}
			else
			{	
				if( isWeaponStateMask( WSM_FIRING ) ||
					rDelay >= m_pOwner->getAnimContext()->getUpperDuration() )
				{
					_OnPlayWeaponIdleAnim();

					if (getLoadedBulletCount() <= 0)
					{
						if (getTotalBulletCount() > 0)
						{
							setWeaponState(WEAPON::STATE_NEEDTO_LOADMAG);		// �ڵ� ������
							removeWeaponStateMask(WSM_LOADMAG_DISABLE);
						}
						else
							setWeaponState(WEAPON::STATE_EMPTY_MAGAZINE);

						if (m_pOwner->isLocal()) addWeaponStateMask(WSM_FIRE_DISABLE);
					}
					else
					{
						setWeaponState(WEAPON::STATE_READY);
						//2015-03-18 ENG_��뿵 
						//���� �ڵ忡�� �̺κп� ���� ��� ���� �ѱ�� ���������� ������ FIRE ���·� �ٲٴ� �ڵ尡 ������ �־���.
						//�����Ǵ� ������ GameCharaAnimControl���� OnAttackRun�Լ����� �ִϸ��̼� �ð��� ������ �ٷ� Cmd_Return_Upper �Լ� ȣ�� �ع���.
						//�׷��� �� ��� ���߻���� �Ұ�������.( OnAttackRun���� �߻� �Լ� ȣ���ϱ��� �ش� ���°� �ٲ� OnAttackRun �Լ��� ȣ����� ����.) 
						//���� ������ ������ ���¸� �ٲپ� ������ �����ϵ��� �߾��µ�, �ش� ����� ���Ⱑ ����ؼ� ���̾� ���·� ���� �Ǿ���� �ٸ� ��ƾ���� ���������� �۵� �� �Ҽ� �ִ� ������ �־� �ٲߴϴ�. 
						//(OnAttackRun�� ���� ���⸸ ����ó���ϵ��� ����)
					}

				}
				else
				{
					if( getLoadedBulletCount() <= 0)
					{
						setWeaponState( WEAPON::STATE_READY );
						_OnPlayWeaponIdleAnim();
					}
					else
					{
						removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
						setWeaponState( WEAPON::STATE_FIRING );
					}
				}
			}
		}
	}
}

REAL32	WeaponBase::_CalcFireDelayForDesignation(REAL32 rDelay)
{
	if( m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());	
		
		if ( pNetInfo->GetDesigApplyInfo() )
			rDelay += pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponFireSpeed;
	}

	return rDelay;
}

REAL32 WeaponBase::_CalcFireDelayForSkill(REAL32 rDelay)
{
	if( m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_FIRESPEED);
		rDelay -= rDelay * ratio;
	}

	return rDelay;
}

REAL32 WeaponBase::_CalcFireDelayForEquip(REAL32 rDelay)
{
	if(m_pOwner != nullptr && m_pOwner->getEquipContext() != nullptr)
	{
		REAL32 ratio = m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_FIREDELAY);
		rDelay -= rDelay * ratio;
	}

	return rDelay;
}

void WeaponBase::_OnProcessFireMiddleAnim( REAL32 rDelay)
{
	I3ASSERT( m_pOwner != nullptr);

	//	ź�� Effect. ��Ʈ �׼ǽ��� �ƴ� �Ϲ� źâ�� �ѱ��� ���
	//	���ǰ� ��Ʈ �׼ǽ� �ѱ⸦ ������ �Ϲ� �ѱ�� �߻����ڸ��� �ٷ� ź�� �߻�
	if( !isWeaponStateMask( WSM_EMPTY_CARTRIDGE) && (m_idxBulletEffect != -1) && (m_idxFXBullet != -1) )
	{
		if( m_pWeaponInfo->GetNeedToLoadBullet() == false )				addWeaponStateMask( WSM_EMPTY_CARTRIDGE);
		else if( m_pWeaponInfo->GetPumpAction() && rDelay > 0.7f )		addWeaponStateMask( WSM_EMPTY_CARTRIDGE);

		//��ȹ�� ��û���� ���ظӴ� ź������Ʈ�� ������� �ʽ��ϴ�.
		if( isWeaponStateMask( WSM_EMPTY_CARTRIDGE) &&
			!m_pWeaponInfo->isNoExjectShell())
		{
			MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( m_idxFXBullet);

			_EmptyCartridgeEffect( m_idxFXBullet, m_idxBulletEffect);
			I3ASSERT( m_pOwner->getMoveContext() != nullptr);
			_AddEmptyCartridgeSound( m_pOwner->getMoveContext()->getFloorTerrainType(), DELAY_EMPTYBULLETSOUND, i3Matrix::GetPos( pMtx) );
		}
	}
}

/*virtual*/ void WeaponBase::SetBulletCountForNetwork( INT32 iLoadedCount, INT32 iDualLoadedCount, INT32 iMaxCount)
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	INT32 iMaxLoadCount =  GetMaxBulletWithCashItem();
	
	if( GetLoadBulletWithCashItem() < iLoadedCount)
		iLoadedCount = GetLoadBulletWithCashItem();
	setLoadedBulletCount( iLoadedCount );

	if( GetLoadBulletWithCashItem() < iDualLoadedCount)
		iDualLoadedCount = GetLoadBulletWithCashItem();
	setLoadedBulletDualCount( iDualLoadedCount);

	if( iMaxLoadCount < iMaxCount )
		iMaxCount = iMaxLoadCount;
	setTotalBulletCount( iMaxCount);
}

/*virtual*/ void WeaponBase::ApplyBulletCountWithCashItem()
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	setLoadedBulletCount(getLoadedBulletCount());
	setLoadedBulletDualCount(getLoadedBulletDualCount());

	INT32 iMaxLoadCountDefault	= m_pWeaponInfo->GetMaxBullet();
	INT32 iMaxLoadCountWithCash = GetMaxBulletWithCashItem();
	INT32 iIncreaseCount		= iMaxLoadCountWithCash - iMaxLoadCountDefault;

	setTotalBulletCount(getTotalBulletCount() + iIncreaseCount);
}

/*virtual*/ void WeaponBase::ApplyBulletCountWithSkill()
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	setLoadedBulletCount(getLoadedBulletCount());
	setLoadedBulletDualCount(getLoadedBulletDualCount());

//	INT32 iMaxLoadCountDefault	= m_pWeaponInfo->GetMaxBullet();
//	INT32 iMaxLoadCountWithSkill = GetMaxBulletWithSkill();
//	INT32 iIncreaseCount		= iMaxLoadCountWithSkill - iMaxLoadCountDefault;
	
	setTotalBulletCount(getTotalBulletCount() + 300);
}

/*virtual*/ void WeaponBase::ApplyBulletWithDropItem(REAL32 rRate)
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	CGameCharaBase* chara = getOwner();
	I3ASSERT( chara != nullptr);

	INT32 lv = DomiUserSkillMgr::i()->GetLevel(chara->getCharaNetIndex(), USER_SKILL_MAGAZINE_UP);
	REAL32 addupStat = domi::SkillProp::i()->GetUserSkill(USER_SKILL_MAGAZINE_UP).GetIncrRatio(lv);


	//��ź �迭�� 25% ź���� ä�� ��� �������� 0.xx �̱� ������ �ø� ��. hansoft.4640
	struct round_up
	{	
		REAL32 operator()(REAL32 f)
		{
			return ((INT32)((f + 0.9f)/1.f))*1.f;
		}
	}rounder;


	INT32 max = m_pWeaponInfo->GetMaxBullet();			//�ִ� ��.
	INT32 increase = (INT32)rounder((max*rRate));		//���� ��.
	INT32 current = getTotalBulletCount();				//���� ��.

	increase += current;
	if( increase > static_cast<INT32>(GetMaxBulletWithCashItem() * addupStat))
		increase = static_cast<INT32>(GetMaxBulletWithCashItem() * addupStat);

	setTotalBulletCount( increase);
}

void WeaponBase::SetEnable( bool bTrue, REAL32 tm)
{
	i3GameNode::SetEnable( bTrue, tm);

	m_mainWeapon->SetEnable(bTrue, tm);
}

void WeaponBase::Instantiate( void)
{
	MATRIX mat;
	I3_ONINSTANCING_INFO info;
	info._pBase = this;
	info._pMatrixStack = g_pFramework->GetViewer()->GetSceneTracer()->GetModelViewMatrixStack();
	
	i3Matrix::SetPos( &mat, 0.f, g_pConfigEx->GetPhysix().HeightWeapon, 0.f);
	info._pMatrixStack->Push( &mat, nullptr);
	OnInstancing( &info);
	info._pMatrixStack->Pop();
}

void WeaponBase::DumpState( void)
{
	i3::rc_wstring wstrWeaponName;
	getWeaponInfo()->GetWeaponName(wstrWeaponName);
	
	I3PRINTLOG(I3LOG_NOTICE, "<< Dump Weapon >>");
	I3PRINTLOG(I3LOG_NOTICE, "- Name : %s (Class %d, No. %d)", wstrWeaponName, (INT32) getWeaponInfo()->GetTypeClass(), getWeaponInfo()->GetNumber());
	I3PRINTLOG(I3LOG_NOTICE, "- Visible : %d", isVisible());
	I3PRINTLOG(I3LOG_NOTICE, "- Enable : %d", isEnable());
	I3PRINTLOG(I3LOG_NOTICE, "- Owner Addr : 0x%p", m_pOwner);
	I3PRINTLOG(I3LOG_NOTICE, "- Current Pos : %.4f, %.4f, %.4f", GetPos()->x, GetPos()->y, GetPos()->z);

	char * pszState = "(none)";
	switch( getWeaponState())
	{
	case WEAPON::STATE_READY:				pszState = "WEAPON::STATE_READY";			break;
	case WEAPON::STATE_WAIT:					pszState = "WEAPON::STATE_WAIT";				break;
	case WEAPON::STATE_FIRE:					pszState = "WEAPON::STATE_FIRE";				break;
	case WEAPON::STATE_LOADBULLET:			pszState = "WEAPON::STATE_LOADBULLET";		break;
	case WEAPON::STATE_LOADMAG:				pszState = "WEAPON::STATE_LOADMAG";			break;
	case WEAPON::STATE_NEEDTO_FIRE:			pszState = "WEAPON::STATE_NEEDTO_FIRE";		break;
	case WEAPON::STATE_NEEDTO_LOADBULLET:	pszState = "WEAPON::STATE_NEEDTO_LOADBULLET";break;
	case WEAPON::STATE_NEEDTO_LOADMAG:		pszState = "WEAPON::STATE_NEEDTO_LOADMAG";	break;
	case WEAPON::STATE_EMPTY_MAGAZINE:		pszState = "WEAPON::STATE_EMPTY_MAGAZINE";	break;
	}

	switch( m_AttachGrip)
	{
	case WEAPON_ATTACH_NONE :		I3PRINTLOG(I3LOG_NOTICE, "- Weapon no attach");					break;
	case WEAPON_ATTACH_GRIP:		I3PRINTLOG(I3LOG_NOTICE, "- Weapon Attach Grip");					break;
	case WEAPON_ATTACH_BARREL:		I3PRINTLOG(I3LOG_NOTICE, "- Weapon Attach Barrel");				break;
	}

	I3PRINTLOG(I3LOG_NOTICE, "- Current State : %s", pszState);
	I3PRINTLOG(I3LOG_NOTICE, "- State Mask : 0x%x", m_StateMaskWeapon);
	if( isWeaponStateMask( WSM_FIRE_DISABLE))
	{
		I3PRINTLOG(I3LOG_NOTICE, "- On : WSM_FIRE_DISABLE");	
	}
	if( isWeaponStateMask( WSM_LOADMAG_DISABLE))
	{
		I3PRINTLOG(I3LOG_NOTICE, "- On : WSM_LOADMAG_DISABLE");	
	}
	if( isWeaponStateMask( WSM_LOADBULLET_DISABLE))
	{
		I3PRINTLOG(I3LOG_NOTICE, "- On : WSM_LOADBULLET_DISABLE");	
	}
	if( isWeaponStateMask( WSM_FIRING))
	{
		I3PRINTLOG(I3LOG_NOTICE, "- On : WSM_FIRING");	
	}	
	if( isWeaponStateMask( WSM_WAIT_AUTOFIREDELAY))
	{
		I3PRINTLOG(I3LOG_NOTICE, "- On : WSM_WAIT_AUTOFIREDELAY");	
	}
}


REAL32	WeaponBase::GetWeaponPenetrate(bool IsStrengthen)
{
	I3ASSERT(m_pWeaponInfo != nullptr);
	I3ASSERT(m_pOwner != nullptr);

	REAL32 Value = 0.0f;


	if ( IsStrengthen )
		Value = m_pWeaponInfo->GetHelmetPenetrate_Str();
	else
		Value = m_pWeaponInfo->GetHelmetPenetrate();

	if( m_pOwner->getEquipContext()) 
	{
		REAL32 penetrate = m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_PENETRATE);
		//I3TRACE2("penetrate - 1:%f, 2:%f\n\n", Value, penetrate);
		Value += penetrate;
	}

	const NET_CHARA_INFO * pNetChara;
	
	pNetChara = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex() );		// ������..
	
	REAL32 ratio = pNetChara->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_PENETRATE);
	Value += ratio;

	return Value;
}

REAL32	WeaponBase::GetCrossHairMin()
{
	I3ASSERT(m_pOwner != nullptr);

	CWeaponInfo *	pInfo = GetCurrentWeaponInfo();

	REAL32 Value = pInfo->GetCrossHairMin();

	return _CalcCrossHairMinToVersion(Value);
}


INT32	WeaponBase::GetRecoilHorzCount(CWeaponInfo * pInfo, bool bApplyExt)
{
	// ���߹��� ��ü �� m_pOwner�� NULL�� ��� �߻�
	//I3ASSERT(m_pOwner != nullptr);

	INT32	Count = 0;

	if( bApplyExt)
		Count = pInfo->GetExtRecoilHorzCount();
	else
		Count = pInfo->GetRecoilHorzCount();

	if ( m_pOwner != nullptr)
	{
		Count += static_cast<INT32>(m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_RECOILHORZCOUNT));
	}

	return Count;
}

INT32 WeaponBase::GetRecoilVertCount(CWeaponInfo * pInfo, bool bApplyExt)
{
	I3ASSERT(m_pOwner != nullptr);

	INT32	Count = 0;

	if (bApplyExt)
		Count = pInfo->GetExtRecoilVertCount();
	else
		Count = pInfo->GetRecoilVertCount();

	return Count;
}

bool WeaponBase::GetOneVertEnable(CWeaponInfo * pInfo, bool bApplyExt)
{
	I3ASSERT(m_pOwner != nullptr);
	
	if (bApplyExt)
		return pInfo->GetExtOneVertEnable() ? true : false;
	else
		return pInfo->GetOneVertEnable() ? true : false;
}

bool	WeaponBase::GetOneHorzEnable(CWeaponInfo * pInfo, bool bApplyExt)
{
	// ���߹��� ��ü �� m_pOwner�� NULL�� ��� �߻�
	// I3ASSERT(m_pOwner != nullptr);

	if( bApplyExt)
		return pInfo->GetExtOneHorzEnable() ? true : false;
	else
		return pInfo->GetOneHorzEnable() ? true : false;
}

CWeaponInfo *	WeaponBase::GetCurrentWeaponInfo(void)
{
	CWeaponInfo * pInfo = getWeaponInfo();

	if( i3::kind_of<WeaponGrenadeLauncher*>(this)  ||
		i3::kind_of<WeaponShotGunLauncher*>(this) )
	{
		if( isExtensionActivate())
			pInfo = getExtensionWeaponInfo();
	}
	else if( i3::same_of<WeaponKrissSuperV*>(this))
	{
		if( isExtensionActivate() && ((WeaponKrissSuperV*)this)->getDualWeapon())
		{
			pInfo = ((WeaponKrissSuperV*)this)->getDualWeapon()->getWeaponInfo();
		}
	}
	else if( i3::same_of<WeaponShotGunGenderWithDual*>(this) )
	{
		if( isExtensionActivate() )
		{
			pInfo = ((WeaponShotGunGenderWithDual*)this)->getDualWeapon()->getWeaponInfo();
		}
	}

	return pInfo;
}

void WeaponBase::SwapExtensionType(void)
{
	WEAPON::EXT_TYPE type0 = m_pWeaponInfo->getExtensionType(0);
	WEAPON::EXT_TYPE type1 = m_pWeaponInfo->getExtensionType(1);

	if( m_CurrentExtensionType == type0)
		m_CurrentExtensionType = type1;
	else if( m_CurrentExtensionType == type1)
		m_CurrentExtensionType = type0;
}
WEAPON::EXT_TYPE WeaponBase::setExtensionType(WEAPON::EXT_TYPE type)
{
	m_CurrentExtensionType = type;

	return m_CurrentExtensionType;
}
WEAPON::EXT_TYPE	WeaponBase::GetNextExtensionType(void)
{
	WEAPON::EXT_TYPE type0 = getWeaponInfo()->getExtensionType(0);
	WEAPON::EXT_TYPE type1 = getWeaponInfo()->getExtensionType(1);
	WEAPON::EXT_TYPE nextType = WEAPON::EXT_NONE;

	if( GetCurrentExtensionType() == type0)
		nextType = type1;
	else 
		nextType = type0;

	return nextType;
}

bool WeaponBase::isPossibleTurnOffWithDotLaser(void)
{	// ��ȹ�� ��û���� ������ ����Ʈ�� off �Ұ����� ���Ⱑ �ִ�. (��. Ŀ���� M4) komet
	CWeaponInfo * pWeaponInfo = GetCurrentWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);
	return !pWeaponInfo->isLockedDotLaser();
}

#if defined( AI_MOVE_TO_WEAPON)
void WeaponBase::AttachCharaAIToChara( i3Chara * pOwner)
{
	if((m_pCharaAINode != nullptr) && (m_pCharaAINode->getParent() == this))
	{
		m_pCharaAINode->AddRef();

		this->RemoveChild( m_pCharaAINode);

		// Character�� Child�� �߰�.
		I3_GAMENODE_ADDCHILD( pOwner, m_pCharaAINode);
		m_pCharaAINode->SetOwnerToAllChildren( pOwner);

		I3TRACE( "ATTACH - AI:%p - Owner:%p\n", m_pCharaAINode, pOwner);

		m_pCharaAINode->Release();
		m_pCharaAINode->SetExclusiveEnable( true, false);

		m_pAICtx->SetEnable( true);
	}
}

void WeaponBase::DetachCharaAIFromChara(void)
{
	if((m_pCharaAINode != nullptr) && (m_pCharaAINode->getParent() != this))
	{
		m_pCharaAINode->AddRef();

		// ���� Parent(Character)�� ���� ����.
		i3GameNode * pParent = m_pCharaAINode->getParent();
		I3ASSERT( pParent != nullptr);
		pParent->RemoveChild( m_pCharaAINode);

		// ����� �Ű� ���δ�.
		I3_GAMENODE_ADDCHILD( this, m_pCharaAINode);
		m_pCharaAINode->SetOwnerToAllChildren( (i3Chara *) this);

		I3TRACE( "DETACH - AI:%p - Owner:%p\n", m_pCharaAINode, this);

		m_pCharaAINode->Release();
		m_pCharaAINode->SetExclusiveEnable( false, false);

		m_pAICtx->SetEnable( false);
	}
}

void WeaponBase::_MakeAnimPath( char * pszDest, INT32 len, const char * pszSrc, bool bCommon)
{
	CCharaInfo * pInfo = (CCharaInfo *) m_pOwner->getDBInfo();
	const char * pszCharaName = nullptr;

	if( bCommon)
	{
		if( m_pOwner->isFemale())
			pszCharaName = "Base_FM_KnEys";
		else
			pszCharaName = "Base_M_AcdPl";
	}
	else
	{
		pszCharaName = pInfo->getAnimationName();
	}

	i3::safe_string_copy( pszDest, pszSrc);

//	i3String::ReplaceNoCase( pszDest, len, "%c", pszCharaName);
	i3::generic_string_ireplace_all(pszDest, "%c", pszCharaName);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	����� ���⿡ ���߾� �ش� ĳ������ Animation�� �˻��� �����ϴ� �Լ�. (Recursive fasion) </summary>
///
/// <remarks>	������, 2010-12-02. </remarks>
///
/// <param name="pAICtx">	[in,out] If non-null, context for a i. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void WeaponBase::_Rec_BindWeaponAnim( i3AIContext * pAICtx)
{
	INT32 i;
	char szName[MAX_PATH];

	for( i = 0; i < pAICtx->getAIStateCount(); i++)
	{
		i3AIState * pState = pAICtx->getAIState( i);

		if( (pState->getStyle() & I3_AISTATE_STYLE_ANIM_TEMPLATE) == 0)
			continue;

		_MakeAnimPath( szName, sizeof(szName), pState->getAnimName(), false);

		i3Animation * pAnim = (i3Animation *) g_pFramework->FindResourceFromManager( szName, i3Animation::static_meta());

		if( pAnim == nullptr)
		{
			_MakeAnimPath( szName, sizeof(szName), pState->getAnimName(), true);

			pAnim = (i3Animation *) g_pFramework->FindResourceFromManager( szName, i3Animation::static_meta());

			if( pAnim == nullptr)
			{
				I3PRINTLOG(I3LOG_FATAL,  "%s Animation�� ã�� �� ����!", szName);
			}
		}

		pState->SetAnim( pAnim);
	}

	{
		// Child�� ��� ó��...
		i3GameNode * pChild = pAICtx->getFirstChild();

		while( pChild != nullptr)
		{
			if( i3::same_of<i3AIContext*>(pChild))
			{
				_Rec_BindWeaponAnim( (i3AIContext *) pChild);
			}

			pChild = pChild->getNext();
		}
	}
}
#endif

bool WeaponBase::isWorldWeapon( void)
{
	if( m_pWeaponInfo == nullptr)
		return false;

	return m_pWeaponInfo->IsWorldWeapon();
}


bool WeaponBase::isDamageToMissionObject( void)
{
	if( m_pWeaponInfo == nullptr)
		return false;

	if( m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_MELEE)
		return false;

	// �߻� �������� ���� ������ �ִ� ����
	if( m_CurrentExtensionType == WEAPON::EXT_DIRECT_MELEE)
	{
		// �и� ����
		if( getFireOrder() == WEAPON::SECONDARY_FIRE)
			return false;
	}

	return true;
}

bool WeaponBase::IsDedicatedHost( void)
{
	bool bHost = BattleServerContext::i()->IsP2PHost();

	ISTEST_RETURN( bHost);

	if( BattleServerContext::i()->IsUseDedicationMode())
	{
		if( (getOwner() != nullptr) && (getOwner()->isLocal() || getOwner()->isAI()) )
		{	// Dedicated Server�� ������ ���� �ڽ��� ������ �ڽ��� ó���ϵ��� �Ѵ�.
			bHost = true;
		}
		else
		{
			bHost = false;
		}
	}

	return bHost;
}

void WeaponBase::setWeaponState( WEAPON::eSTATE state)			
{ 
	// GlobalFunc::PB_TRACE("WEAPON_STATE %d", state);
	m_StateWeapon = state; 
}

bool WeaponBase::IsLocalOrAI()
{
	if( (getOwner() != nullptr) && (getOwner()->isLocal() || getOwner()->isAI()) )
	{
		return true;
	}

	return false;
}

REAL32 WeaponBase::_CalcReloadTimeToVersion(REAL32 rReloadTime)
{
	// V1 ������ ������ ���ó�� ����.
	if (g_pEnvSet->IsV2Version() == false)
		return rReloadTime;

	// �����ڰ� ������ ���� ��� ó�� ����.
	if (m_pOwner == nullptr)
		return rReloadTime;

	// ��� ability
	if (m_pOwner->getEquipContext() != nullptr && m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_PRIMARY)
	{
		REAL32 ratio = m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_RELOADTIME);
		CGameCharaEquip * Headgear = m_pOwner->getEquipContext()->getEquip(EQUIP::ePART_HEADGEAR);
		
		if(  Headgear != nullptr)
		{
			// �̰� ���� ������ ����ó�� �մϴ�.
			if (Headgear->GetID() == 9)
			{
				ratio += 0.25f;
			}
		}

		rReloadTime -= rReloadTime * ratio;
	}

	//	���� ���� ������ ���� �ð� ����.
	REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);
	CASHITEM_TRACE("CASHITEMSKILL_REBULLET_RATE = %f\n", ratio);
	rReloadTime -= (rReloadTime * ratio);

	//  ��Ư�� ���� ���� ��ų ����
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio_weapon_skill = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_RELOAD);
		rReloadTime -= ratio_weapon_skill;

		// ȣĪ ����.. (12������ �ƿ� �� ����� ��Ȱ��ȭ�� �̻� ���¶�...Ȯ���� �ʿ������� �𸥴�.. 15.05.22.����)
		const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
		if (desig_apply_info)
		{
			//	rVal += desig_apply_info->desigWeapon.weaponReloadTime;				
			REAL32 fRate = m_pWeaponInfo->GetReloadTime() * 0.01f;		// ����?? ������?? �ۼ�Ƽ�� ǥ���Ϸ���...
			rReloadTime += desig_apply_info->desigWeapon.weaponReloadTime * fRate;		// �ϴ� �ݿ����� �ٸ� �������� WeaponInfo������ ������ ���..
		}
	}

	return MINMAX(0.1f, rReloadTime, 100.f); ;
}

REAL32 WeaponBase::_CalcReloadTimeScaleToVersion(i3AIState * pAIState/* = nullptr*/, REAL32 rReloadTime/* = 1.0f*/, REAL32 rSpeedRatio/* = 0.0f*/)
{
	// ������ �������� ����Ʈ ���ڸ�, ����Ʈ ���� �缳�����ݴϴ�.
	if (rSpeedRatio == 0.0f)
	{
		rSpeedRatio = 1.0f;

		if( (g_pEnvSet->IsV2Version() == false) && EnablePlayMasterAnim() )
			rSpeedRatio = MASTER_RELOAD_SPEED_RATIO;
	}

	REAL32 rTimeScale = 1.0f;

	if (pAIState != nullptr)
	{
		rTimeScale = (pAIState->getAnimDuration() / rReloadTime);
	}

	return rTimeScale * rSpeedRatio;
}

REAL32 WeaponBase::_CalcSwapTimeScaleToVersion(void)
{
	if (m_pOwner == nullptr)
		return 1.0f;

	REAL32 rTimeScale = 1.0f;

	if (g_pEnvSet->IsV2Version())
	{
		rTimeScale = m_pOwner->GetWeaponSwapTimeScale();
	}
	else
	{
		// PBTN Beret ����ó��
		{
			const CEquipInfo * pBeretInfo = m_pOwner->getEquipContext()->getEquipInfo(EQUIP::GetePartToBeret());

			if (pBeretInfo != nullptr && pBeretInfo->GetITEMID() == 2700013)
			{
				REAL32 ratio = 0.5f + I3_EPS;
				rTimeScale += (rTimeScale * ratio);
			}
		}
	}

	//	���� ���� ���� ������ ���� �ð� ����.
	REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_SWAPWEAPON_RATE);
	CASHITEM_TRACE("CASHITEMSKILL_SWAPWEAPON_RATE = %f\n", ratio);
	rTimeScale += (rTimeScale * ratio);

	return rTimeScale;
}


REAL32 WeaponBase::_CalcFireDelayToVersion(REAL32 rFireDelay)
{
	if (g_pEnvSet->IsV2Version())
	{
		rFireDelay = _CalcFireDelayForDesignation(rFireDelay);	// ȣĪ�� ���� �߻� �����ð�.
		rFireDelay = _CalcFireDelayForSkill(rFireDelay);		// ��ų�� ���� �߻� �����ð� ���
		rFireDelay = _CalcFireDelayForEquip(rFireDelay);		// ��� ���� �߻� �����ð� ���

		rFireDelay = MINMAX(I3_EPS, rFireDelay, 100.f);
	}
	else
	{
		rFireDelay = _CalcFireDelayForDesignation(rFireDelay);	// ȣĪ�� ���� FireDelay ���

		// ��� ���� FireDelay ��� ( V1�� ���� ���Ұ� �ƴ� ���밪 ��ġ ����.)
		rFireDelay += m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_FIREDELAY);
	}

	return rFireDelay;
}

REAL32 WeaponBase::_CalcCrossHairMinToVersion(REAL32 rCrossHairMin)
{
	if (g_pEnvSet->IsV2Version())
	{
		// �̰��� ���� ���� �������ϴ�.
		rCrossHairMin -= (rCrossHairMin * m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_ACCURACY));

		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_ACCURACY);
		rCrossHairMin -= rCrossHairMin * ratio;
		rCrossHairMin = MINMAX(0.f, rCrossHairMin, 100.f);
	}
	else
	{
		// ��� ���� FireDelay ��� ( V1�� ���� ���Ұ� �ƴ� ���밪 ��ġ ����.)
		rCrossHairMin += m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_ACCURACY);

		if (rCrossHairMin <= 0.0f)
			rCrossHairMin = 0.01f;
	}

	return rCrossHairMin;
}

REAL32 WeaponBase::_CalcDeviationToVersion(REAL32 rDeviation)
{
	REAL32 rRate = 1.0f;

	if( g_pEnvSet->IsV2Version() )
	{
		// Equipment Ability
		// �������� ���� ���� ��������.
		rRate -= (rRate * m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_ACCURACY));

		// Skill ability
		{
			const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
			REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_ACCURACY);
			rRate -= ratio;

			// ȣĪ �Ӽ� �ݿ� ...
			if (pNetInfo->GetDesigApplyInfo())
			{
				const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
				rRate += desig_apply_info->desigWeapon.weaponDeclination;
			}
			// 
		}

		rDeviation *= rRate;
		rDeviation = MINMAX(0.f, rDeviation, 100.f);

	}
	else
	{
		rRate = 0.0f;

		// ȣĪ �Ӽ� �ݿ� ...
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		if (pNetInfo->GetDesigApplyInfo())
		{
			const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
			rRate += desig_apply_info->desigWeapon.weaponDeclination;
		}

		rDeviation += (rDeviation * rRate);
		rDeviation = MINMAX(0.f, rDeviation, 100.f);
	}

	return rDeviation;

}

bool WeaponBase::_IsEnableMasterAnimToVersion(void)
{
	I3ASSERT(m_pOwner);

	if (g_pEnvSet->IsV2Version())
	{
		REAL32 reloadTime = 0.0f;

		const CEquipInfo * pHeadInfo = m_pOwner->getEquipContext()->getEquipInfo(EQUIP::ePART_HEADGEAR);
		reloadTime += pHeadInfo->GetAbilityValue(EQUIP::eABILITY_RELOADTIME);

		//if (m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_RELOADTIME) > 0.0f)

		if (reloadTime != 0.f && reloadTime > 0.f)
			return true;

		return false;
	}
	else
	{
		const CEquipInfo * pBeretInfo = m_pOwner->getEquipContext()->getEquipInfo(EQUIP::GetePartToBeret());

		if (getWeaponInfo() == nullptr ||
			getWeaponInfo()->GetGaugeReload() == 1 ||  // �ε�Ÿ���� �����ϰ�� �����մϴ�. 
			pBeretInfo == nullptr)
		{
			return false;
		}

		T_ItemID beret_id = pBeretInfo->GetITEMID();
		WEAPON_CLASS_TYPE eWeaponClass = getWeaponInfo()->GetTypeClass();

		//// Shotgun �߿��� KelTec�� ���� ������ ����
		//if (eWeaponClass == WEAPON_CLASS_SHOTGUN &&
		//	(getWeaponInfo()->IsFastReloadAnimation() == false))
		//{
		//	return false;
		//}

		switch (beret_id)
		{
		case 2700006:	// Kopassus
		case 2700005:	// CrossSword
		case 2700012:	// PBNC5
		{
			if (eWeaponClass == WEAPON_CLASS_ASSAULT ||
				eWeaponClass == WEAPON_CLASS_SMG ||
				eWeaponClass == WEAPON_CLASS_DUALSMG)
			{
				return true;
			}
		}
		break;
		case 2700014:	// Title_Assault
		case 2700007:	// Star-Red
		{
			if (eWeaponClass == WEAPON_CLASS_ASSAULT)
			{
				return true;
			}
		}
		break;
		case 2700017:	// Title_Submachine
		{
			if (eWeaponClass == WEAPON_CLASS_SMG ||
				eWeaponClass == WEAPON_CLASS_DUALSMG)
			{
				return true;
			}
		}
		break;
		case 2700016:	// Title_Sniper
		case 2700011:	// Star-Yellow
		{
			if (eWeaponClass == WEAPON_CLASS_SNIPER)
			{
				return true;
			}
		}
		break;
		case 2700018:	// Title_ShotGun
		{
			if (eWeaponClass == WEAPON_CLASS_SHOTGUN ||
				eWeaponClass == WEAPON_CLASS_DUALSHOTGUN)
			{
				return true;
			}
		}
		break;
		case 2700015:	// Title_HandGun
		{
			if (eWeaponClass == WEAPON_CLASS_HANDGUN ||
				eWeaponClass == WEAPON_CLASS_DUALHANDGUN ||
				eWeaponClass == WEAPON_CLASS_CIC)
			{
				return true;
			}
		}
		break;
		case 2700001:	// Black
		case 2700003:	// BlackPB
		case 2700013:	// PBTN
		case 2700004:	// Bope
		case 2700008:	// S-General
		case 2700002:	// Brazil
		case 2700019:	// VeraCruz2016
		{
			return true;
		}
		break;
		}
	}

	return false;
}


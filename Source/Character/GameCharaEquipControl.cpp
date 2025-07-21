#include "pch.h"
#include "GameCharaEquipControl.h"
#include "GameCharaEquipContext.h"
#include "GlobalVariables.h"
#include "GameCharaEquipDef.h"

I3_CLASS_INSTANCE( CGameCharaEquipControl);

bool CGameCharaEquipControl::OnCreateEquip( CHARA_RES_ID CharaType, T_ItemID ItemID, TEAM_TYPE team, INT32 res_subtype)
{
	bool ret = false;

	//	Resource
	i3GameResSceneGraph * pRes = (i3GameResSceneGraph *) g_pCharaManager->QueryEquipResource( ItemID, team, res_subtype);

	if(pRes == nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE, "QueryEquipResource Fail CharaType %d ItemID %d", CharaType, ItemID);
		return false;
	}

	I3ASSERT( pRes != nullptr);
	I3ASSERT( pRes->getSceneGraphInfo() != nullptr);

	//	Create
	i3InstanceManager * pManager = g_pFramework->GetCurrentStage()->GetInstanceManager( CGameCharaEquip::static_meta() );
	I3ASSERT( pManager != nullptr);

	CGameCharaEquip * pEquip =  reinterpret_cast<CGameCharaEquip*>(pManager->GetFreeInstance());
	I3ASSERT( pEquip != nullptr);

	pEquip->Create( pRes, ItemID);

	EQUIP::ePART part = EQUIP::ItemID2PartsType(ItemID);

	MATRIX mat;
	i3Body * pBody = pEquip->getSceneObject()->GetBody();
	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	
	i3Matrix::SetPos( &mat, (REAL32)( m_pOwner->getCharaNetIndex()) * 10.f, g_pConfigEx->GetPhysix().HeightEquip, (REAL32)(part) * 10.f, 1.f );

	for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
	{
		if( pSkel->isShapeSet( i))
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
			I3ASSERT( pShapeSet != nullptr );
			pShapeSet->setShapeGlobalPose( &mat );

			if( pEquip->IsPhysixPart() )
				pShapeSet->setPhysxStyle(I3PHYSIX_USAGE_TYPE_KM_FORCE_DISABLE);
		}
	}

	pEquip->Reset();

	ret = OnSet( pEquip);
	if( !ret)
	{
		I3TRACE( "[EQUIP_Add] Warning! Failed to add equipment (ItemID: %d)\n", ItemID);

		//	�޸� ����
		//I3_SAFE_RELEASE( pEquip);
		pEquip->ReturnInstance();
	}

	return ret;
}

bool CGameCharaEquipControl::OnSet( CGameCharaEquip * pEquip)
{
	I3ASSERT( pEquip != nullptr);

	bool ret = false;
	EQUIP::ePART type = pEquip->GetEquipPart();

	if( type < EQUIP::ePART_HEADGEAR)	// ��ü�� ��� �ƴ�.
		return false;

	OnDeleteEquip( type);	//	���� ��� ����

	ret = OnAttach( pEquip);	//	���� ��� ����
	if( !ret)
	{
		I3TRACE( "[EQUIP_Set] Warning! Failed to set equipment (Part: %d, num: %d)\n", type, pEquip->GetID());
	}

	//	1��Ī �ϰ�� ���� ���� �ʿ䰡 ����.
	if( m_pOwner->is1PV())
	{
		pEquip->SetVisible( false);
	}
	else
	{
		pEquip->SetVisible( true);	
	}

	return ret;
}

bool CGameCharaEquipControl::OnDeleteEquip( EQUIP::ePART type)
{
	m_pOwner->getEquipContext()->ReleaseEquip( type);
	return true;
}


bool CGameCharaEquipControl::OnAttach( EQUIP::ePART type, bool bReset)
{
	bool bRv = false;
	CGameCharaEquip * pEquip = getEquip( type);

	if( pEquip != nullptr)
	{
		if( bReset)
			pEquip->Reset();

		if( pEquip->GetOwner() != m_pOwner)
		{
			OnDetach( type);
			bRv = OnAttach( pEquip);
		}
	}

	return bRv;
}

bool CGameCharaEquipControl::OnAttach( CGameCharaEquip * pEquip)
{
	I3ASSERT( pEquip != nullptr);

	//	ĳ���Ϳ� Attach�� �Ѵ�.
	EQUIP::ePART nEquipPart = pEquip->GetEquipPart();

	if( getEquip( nEquipPart ) != nullptr)
	{
		if( getEquip( nEquipPart)->IsAttached())
		{
			I3PRINTLOG(I3LOG_FATAL,  "Failed! Please detach or delete previous equipmenet. (Part: %d)", nEquipPart);
			return false;
		}
	}

	// MustBe Fix
	// ��� �ش��ϴ� ������ �ٿ��ش�.
	i3Node * pAttachBone = m_pOwner->getEquipContext()->getDummy( nEquipPart);

	if (pAttachBone == nullptr)
	{
		TRACE("CGameCharaEquipControl::OnAttach pAttachBone = nullptr\n");
		return false;
	}

	//	ĳ���Ϳ� ���δ�.
	pEquip->Attach( pAttachBone, m_pOwner, true);
	setEquip( nEquipPart, pEquip);

	// ��ǥ������� üũ
	if( nEquipPart == EQUIP::ePART_HEADGEAR)
	{
		const CEquipInfo * pEquipInfo = getEquipContext()->getEquipInfo( EQUIP::ePART_HEADGEAR);
		if( pEquipInfo != nullptr)
			getEquipContext()->setTargetChaseC5( pEquipInfo->GetAbilityValue( EQUIP::eABILITY_DETECTC5));
		else
			getEquipContext()->setTargetChaseC5( 0.f);
	}

	return true;
}

CGameCharaEquip * CGameCharaEquipControl::OnDetach( EQUIP::ePART type)
{
	//	ĳ���Ϳ��� Detach�� �Ѵ�.
	CGameCharaEquip * pEquip = m_pOwner->getEquipContext()->getEquip( type);
	
	if( pEquip != nullptr && pEquip->IsAttached() )
		pEquip->Detach();	//	�θ𿡼� �и�

	// ��ǥ������� üũ
	if( type == EQUIP::ePART_HEADGEAR)
	{
		getEquipContext()->setTargetChaseC5( 0.f);
	}

	return pEquip;
}

CGameCharaEquip * CGameCharaEquipControl::OnDropEquip( EQUIP::ePART type, i3Node * pSceneNode, i3GameNode * pGameNode, VEC3D * pVecPos)
{
	I3ASSERT( pSceneNode != nullptr);
	I3ASSERT( pGameNode != nullptr);

	//	ĳ���� ��� �����
	CGameCharaEquip * pCharaEquip = OnDetach( type);

    //	������ ��� �ִٸ�
	if( pCharaEquip == nullptr)	return nullptr;

	pCharaEquip->SetVisible( true);

	// Equip Matrix �缳��.
	if (pCharaEquip->SetEquipMatrix(0, pVecPos) == false)
		return nullptr;

	//	���ο� �θ� �ٴ´�.
	pCharaEquip->Attach( pSceneNode, pGameNode, false);

	return pCharaEquip;
}



#include "pch.h"
#include "Avatar.h"
#include "../../Avatar/AvatarGen.h"
#include "GameCharaManager.h"
#include "AvatarManager.h"
#include "GameCharaAnimContext.h"
#include "GameCharaMaterialContext.h"
#include "GameCharaEquipContext.h"
#include "GameCharaCollisionContext.h"
#include "GameCharaContexts.h"
#include "StageBattle.h"

#include "StageObject/ObjectWithWeapon.h"
#include "StageObject/ObjectRidingHelicopter.h"
#include "StageObject/ObjectRespawnHelicopter.h"

#include "UI/UICharaView.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "i3Math/octree/occ_culler.h"
#include "i3Math/octree/octree.h"
#include "i3Scene/i3XrayScene.h"

//---------------------------------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( Avatar);//, CGameCharaBase);

struct ISOLATE_INFO
{
	i3Node *		m_pNode = nullptr;
	bool			m_bStatus = false;
} ;

static I3SG_TRAVERSAL_RESULT _AvatarIsolateNodeProc( i3Node * pNode, void * pData, i3MatrixStack * pStack);


Avatar::Avatar()
{
	m_pGen = AvatarGen::new_object();
}

Avatar::~Avatar(void)
{
	_Destroy();
	I3_SAFE_RELEASE(m_pGen);
}

void Avatar::_Destroy( void)
{
	if (m_pTrans)
	{
		m_pTrans->RemoveFromParent();
		m_pTrans->RemoveAllChild();
		I3_SAFE_RELEASE(m_pTrans);
	}

	if (m_pBody)
	{
		m_pBody->RemoveFromParent();
		m_pBody->RemoveAllChild();
		I3_SAFE_RELEASE(m_pBody);
	}

	if( m_pTempBody != nullptr)
	{
		m_pTempBody->RemoveAllChild();
		I3_SAFE_RELEASE( m_pTempBody);
	}
}


INT32 Avatar::_FindFrameLOD( i3Body * pBody)
{
	INT32 base = -1;
	UINT32 maxCount = 0;
	UINT32 lodCnt = m_pBody->getLODCount();

	for( UINT32 i = 0; i < lodCnt; i++)
	{
		i3LOD * pLOD = m_pBody->getLOD( i);
		UINT32 boneCount = pLOD->getBoneCount();

		if( maxCount < boneCount)
		{
			base = (INT32) i;
			maxCount = boneCount;
		}
		else if( maxCount == boneCount)
		{
			INT32 shapeSetCnt = pLOD->hasShapeSet();			

			if( (shapeSetCnt > 0) || ( i == 1))	// LOD1�ΰ��
			{
				base = (INT32) i;
			}
		}
	}

	return base;
}

void Avatar::ResetChara( void)
{
	// 0.0003ms
	ApplyLoadedRes();

	//// �Ӹ� ���̱� (0.09ms)
	SetCharaHead( true);

	// 0.9ms
	CGameCharaBase::ResetChara();

	// ���� �����Ϳ� ������ ����� �Ѵ�.
	if( m_pTempCharaInfo != nullptr)
	{
		m_pTempCharaInfo->SetHP((REAL32) getFullHP());
	}

#if defined (LOCALE_RUSSIA)		
	DisableTeamBand();	// PBRU-1064 ���þ� ��û���� ����� ����
#else
	// 0.5ms ~ 1ms
	if (g_pFramework->IsBattleStage() &&
		(g_pEnvSet->m_bTeamBand == true) && (BattleSlotContext::i()->getMyTeam() != getCharaTeam()))
	{
		SetTeamBand(true);
	}
	else
		DisableTeamBand();
#endif

	//��Ų������ ������ ���ؼ��� ���� ������ �ʰ��Ѵ�
	const CHARA_PARTS_INFO * pPartsInfo = m_pCharaInfo->GetParts();
	const CSkinInfo * pSkinInfo = nullptr;
	T_ItemID skinItemID = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_SKIN);
	if (skinItemID != 0)
	{
		pSkinInfo = static_cast<const CSkinInfo*>(g_pCharaInfoDataBase->GetInfo(skinItemID));
	}

	if (pSkinInfo != nullptr)
	{	
		if (pSkinInfo->GetHeadgearItemID() <= 0)
		{
			CGameCharaEquip * pEquip = getEquipContext()->getEquip(EQUIP::ePART_HEADGEAR);
			if (pEquip != nullptr)
			{
				pEquip->SetVisible(false);
			}

			// FACEGEAR�� ���ش� (PBBUG-16270)
			pEquip = getEquipContext()->getEquip(EQUIP::ePART_FACEGEAR);
			if (pEquip != nullptr)
			{
				pEquip->SetVisible(false);
			}
		}
	}

}

void Avatar::AttachScene( void)
{
	if( IsAttachedScene())
		return;

	if (getGNodeStyle() & I3_GAMENODE_STYLE_XRAY)
	{
		g_pFramework->getSgContext()->GetXrayScene()->AddXrayNode(GetNode(), g_pFramework->getLayer(0)->getLayerRoot(), g_pFramework->GetLightAttrSet());
	}
	else
	{
		g_pFramework->getLayer(0)->getLayerRoot()->AddChild(GetNode());
	}

	addGNodeState( I3_GAMENODE_STATE_ATTACHED);
	
	i3::occ_culler* pCuller = g_pFramework->getSgContext()->GetOccCuller();
	bool			isEnabledOccludee = m_pSceneObj->IsEnabledOccludee();

	if (pCuller && isEnabledOccludee)
	{
		i3::octree_item* pItem = this->getSceneObject()->GetOctreeItem();
		pCuller->add_occludee_item(pItem);
	}

}

void Avatar::DetachScene( void)
{
	if( !IsAttachedScene())
		return;

	if (getGNodeStyle() & I3_GAMENODE_STYLE_XRAY)
	{
		removeGNodeStyle(I3_GAMENODE_STYLE_XRAY);

		g_pFramework->getSgContext()->GetXrayScene()->RemoveXrayNode(GetNode());
	}
	else
	{
		g_pFramework->getLayer(0)->getLayerRoot()->RemoveChild(GetNode());
	}

	removeGNodeState( I3_GAMENODE_STATE_ATTACHED);

	i3::occ_culler* pCuller = g_pFramework->getSgContext()->GetOccCuller();
	bool			isEnabledOccludee = m_pSceneObj->IsEnabledOccludee();

	if (pCuller && isEnabledOccludee)
	{
		i3::octree_item* pItem = this->getSceneObject()->GetOctreeItem();
		pCuller->remove_occludee_item(pItem);
	}
	
}

static void	sRec_ResetBoneRef( i3Node * pNode, i3Skeleton * pOrigin, i3Skeleton * pSrcSkel)
{
	if( i3::same_of<i3BoneRef*>(pNode))
	{
		i3BoneRef * pBoneRef = (i3BoneRef*) pNode;

		i3Bone * pBone = pOrigin->getBone( pBoneRef->getBoneIndex());
		INT32 idx = pSrcSkel->FindBoneByName( pBone->getName());
		I3ASSERT( idx > -1 && idx < pSrcSkel->getBoneCount() );	// ���� �ٸ� �����̴�?
		pBoneRef->setBoneIndex( idx);
	}

	INT32 cnt = pNode->GetChildCount();
	for( INT32 i = 0; i < cnt; i++)
	{
		sRec_ResetBoneRef( pNode->GetChild( i), pOrigin, pSrcSkel);
	}
}

static void	sRecCopyNode( i3Node * pSrc, i3Node * pDest)
{
	I3ASSERT( pSrc->static_meta() == pDest->static_meta());
	I3ASSERT( pSrc->GetChildCount() == pDest->GetChildCount());

	pSrc->CopyTo( pDest, I3_COPY_INSTANCE);

	for( INT32 i = 0; i < pSrc->GetChildCount(); i++)
	{
		sRecCopyNode( pSrc->GetChild(i), pDest->GetChild(i));
	}
}

void Avatar::ApplyLoadedRes( void)
{
	// 1 ~ 2ms
	if( m_pTempBody != nullptr)
	{
		i3Skeleton * pFrameSkel = m_pBody->getFrameLOD()->getSkeleton();
		i3Skeleton * pSrcSkel = m_pTempBody->getFrameLOD()->getSkeleton();

		if( m_bInBattle)
		{// ���ο� �������� ���� Shape�� �ٽ� ��ġ
			INT32 boneCnt = pFrameSkel->getBoneCount();
			for( INT32 i = 0; i < boneCnt; i++)
			{
				if( pFrameSkel->isShapeSet( i))
				{
					i3Bone * pBone = pFrameSkel->getBone( i);
					INT32 idx = pSrcSkel->FindBoneByName( pBone->getName());
					if (idx == -1)
					{
						I3ASSERT(idx != -1);
						continue;
					}
					pSrcSkel->setShapeSet( idx, pFrameSkel->getShapeSet( i));
				}
			}
		}

		// ���⸦ ��� ����.
		// �Ʒ� BoneRef �������� ������� �˻��Ǹ� �ȵ�.
		for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
		{
			getWeaponContext()->_Detach_Weapon( static_cast<WEAPON_SLOT_TYPE>(i));
		}

		// BoneRef�� �ٽ� ����
		sRec_ResetBoneRef( m_pBody, pFrameSkel, pSrcSkel);

		// BoneRef�� ���� transform ���� ����
		i3::vector<i3Node*> list;
		i3Scene::FindNodeByExactType( m_pTempBody, i3BoneRef::static_meta(), list);

		for( UINT32 i = 0; i < list.size(); i++)
		{
			i3Node * pSrc = list.at(i);
			i3Node * pChangableNode = i3Scene::FindNodeByName( m_pBody, pSrc->GetName());
			if( pChangableNode != nullptr)
			{
				sRecCopyNode( pSrc, pChangableNode);
			}
			else
				m_pBody->AddChild( pSrc);
		}

		getWeaponContext()->OnChangeBoneHierarchy( m_pBody);
		getBoneContext()->setEyeDummyLeft( m_pGen->getEyeDummyLeft());
		getBoneContext()->setEyeDummyRight( m_pGen->getEyeDummyRight());

		// �ٽ� ���̱�
		WeaponBase * pWeapon = GetCurrentCharaWeapon();
		if( pWeapon != nullptr)
			pWeapon->AttachNode( getWeaponContext()->getAttachRoot(), this );

		// LOD�� ���
		m_pBody->RemoveAllLOD();

		UINT32 lodCnt = m_pTempBody->getLODCount();
		for( UINT32 i = 0; i < lodCnt; i++)
		{
			i3LOD * pSrcLOD = m_pTempBody->getLOD( i);
			//I3ASSERT( pSrcLOD->GetRefCount() == 1);

			m_pBody->AddLOD( pSrcLOD);
		}

		//// LOD ������ �پ��� �ִ�. �ٽ� ����
		m_pBody->setCurrentLevel( m_pBody->getCurrentLevel());
		m_pBody->setFrameLOD( m_pTempBody->getFrameLOD());

		// Animation Context�� �ٽ� ����. Skeleton ������ ����ȴ�.
		m_pBody->Prepare( false);

		// ���ӿ��� �����ϴ� bone index�� �ٽ� ����
		getBoneContext()->InitBone();
		// anim scale�� ����ϴ� index�� ����
		getAnimContext()->BindBoneList();

		__SearchTeamBandSg();
		__SearchHeadSg();

#if defined (LOCALE_RUSSIA)		
		DisableTeamBand();	// PBRU-1064 ���þ� ��û���� ����� ����
#else
		// 0.5ms ~ 1ms
		if (g_pFramework->IsBattleStage() &&
			(g_pEnvSet->m_bTeamBand == true) && (BattleSlotContext::i()->getMyTeam() != getCharaTeam()))
		{
			SetTeamBand(true);
		}
		else
			DisableTeamBand();
#endif

		if( m_pBody->GetCurrentAnim() != nullptr)
			m_pBody->PlayAnim( m_pBody->GetCurrentAnim());

		I3_SAFE_RELEASE( m_pTempBody);
		m_pMaterialContext->CreateMaterialTable();

		const CHARA_PARTS_INFO * pPartsInfo = m_pCharaInfo->GetParts();

		CHARA_RES_ID charaID = CHARA::ItemID2ResID( pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));

		// vv3 - check
		const CSkinInfo * pSkinInfo = nullptr;
		T_ItemID skinItemID = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_SKIN);
		if( skinItemID != 0)
		{
			pSkinInfo = static_cast<const CSkinInfo*>(g_pCharaInfoDataBase->GetInfo( skinItemID));
		}

		if( pSkinInfo != nullptr)
		{	// skin�� ���
			// vv3 - check
			if( pSkinInfo->GetHeadgearItemID() > 0)
				Cmd_CreateEquip( charaID, pSkinInfo->GetHeadgearItemID());
			else
			{
				// ������ �����ǰ� ������ �ʵ��� �Ѵ�.
				T_ItemID item_id = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_HEAD);
				T_ItemID sub_item_id = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_BERET);

				Cmd_CreateEquip( charaID, item_id, sub_item_id);
				CGameCharaEquip * pEquip = getEquipContext()->getEquip( EQUIP::ePART_HEADGEAR);
				if( pEquip != nullptr)
				{
					pEquip->SetVisible( false);
				}
			}
			if( pSkinInfo->GetFacegearItemID() > 0)
				Cmd_CreateEquip( charaID, pSkinInfo->GetFacegearItemID());
			else
			{
				// 1.5�� Skin�� ��� ������ ����.
				// 1.0/1.1 ���� ��쿡�� Facegear�� �ʿ��մϴ�.
				if( g_pEnvSet->IsV2Version() == true)
					Cmd_DeleteEquip(EQUIP::ePART_FACEGEAR);
				else
				{
					T_ItemID faceID = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_FACE);
					if (faceID > 0)
					{
						Cmd_CreateEquip(charaID, faceID);
						CGameCharaEquip * pEquip = getEquipContext()->getEquip(EQUIP::ePART_FACEGEAR);
						if (pEquip != nullptr)
						{
							pEquip->SetVisible(false);
						}
					}
					else
						Cmd_DeleteEquip(EQUIP::ePART_FACEGEAR);
				}
			}

			if( pSkinInfo->GetHolsterItemID() > 0)
				Cmd_CreateEquip( charaID, pSkinInfo->GetHolsterItemID());
			else
				Cmd_DeleteEquip( EQUIP::ePART_HOLSTER);

			if( pSkinInfo->GetBeltItemID() > 0)
				Cmd_CreateEquip( charaID, pSkinInfo->GetBeltItemID());
			else
				Cmd_DeleteEquip( EQUIP::ePART_BELT);
		}
		else
		{
			if( pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_HEAD) > 0)
				Cmd_CreateEquip( charaID, pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_HEAD), pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_BERET));
			else
				Cmd_DeleteEquip( EQUIP::ePART_HEADGEAR);

			Cmd_CreateEquip( charaID, pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_FACE));
			Cmd_CreateEquip( charaID, pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_HOLSTER));
			Cmd_CreateEquip( charaID, pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_BELT));
		}

		//// Resource�� Node�� �ٽ� ã�´�.
		InitClanMark();

		if( getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx())
		{
			USER_INFO_BASIC		MyUserInfoBasic;
			UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
			CharaChangeClanMark( MyUserInfoBasic.m_ui32ClanMark);
		}
		else
			CharaChangeClanMark( BattleSlotContext::i()->getSlotInfo( getCharaNetIndex())->_clanMark);

		I3TRACE( "���� ĳ���͸� �ƹ�Ÿ�� ��ü %d %d-------------------------------------------\n", getCharaNetIndex(), this->GetRefCount());

		m_bUseDummyResource = false;
	}
}

void Avatar::Cmd_PrepareEquip( EQUIP::ePART part)
{
	if( getEquipContext()->getEquip( part) != nullptr &&
		!getEquipContext()->getEquip( part)->IsModified() )
	{
		getEquipContext()->getEquip( part)->SetModified( true);

		i3SceneObject * pObj = getEquipContext()->getEquip( part)->getSceneObject();
		I3ASSERT( pObj != nullptr);

		// texture�� macrotex�� �����Ѵ�.
		{
			i3::vector<i3Node*> list;
			i3Scene::FindNodeByExactType( m_pBody, i3AttrSet::static_meta(), list);

			i3Texture * pDiff = nullptr;
			i3Texture * pNormal = nullptr;
			i3Texture * pSpecular = nullptr;
			i3Texture * pReflectMask = nullptr;

			UINT32 cnt = list.size();
			for( UINT32 i = 0; i < cnt; i++)
			{
				i3AttrSet * pAttrSet = (i3AttrSet*) list.at(i);

				if( AvatarGen::_CanMergeGeometry( pAttrSet) )
				{
					i3TextureBindAttr * pAttr = (i3TextureBindAttr *) i3Scene::FindAttrByExactType( pAttrSet, i3TextureBindAttr::static_meta());

					if (pAttr != nullptr)
						pDiff = pAttr->GetTexture();

					pAttr = (i3TextureBindAttr *) i3Scene::FindAttrByExactType( pAttrSet, i3NormalMapBindAttr::static_meta());
					if( pAttr != nullptr)
						pNormal = pAttr->GetTexture();

					pAttr = (i3TextureBindAttr *) i3Scene::FindAttrByExactType( pAttrSet, i3SpecularMapBindAttr::static_meta());
					if( pAttr != nullptr)
						pSpecular = pAttr->GetTexture();

					pAttr = (i3TextureBindAttr *) i3Scene::FindAttrByExactType( pAttrSet, i3ReflectMaskMapBindAttr::static_meta());
					if( pAttr != nullptr)
						pReflectMask = pAttr->GetTexture();

					break;	// ��ǥ�� �ϳ��� �ؽ��ĸ� ã���� ��. ��� ���� �ؽ�����
				}
			}

			{
				I3ASSERT( pDiff != nullptr);
				i3::vector<i3RenderAttr*> destList;
				i3Scene::FindAttrByExactType( pObj, i3TextureBindAttr::static_meta(), destList);

				const UINT32 cnt_destList = destList.size();
				for( UINT32 i = 0; i < cnt_destList; i++)
				{
					i3TextureBindAttr * pAttr = (i3TextureBindAttr *) destList.at(i);
					pAttr->SetModified(true);
					pAttr->SetTexture( pDiff);
				}
			}

			if( pNormal != nullptr)
			{
				i3::vector<i3RenderAttr*> destList;

				i3Scene::FindAttrByExactType( pObj, i3NormalMapBindAttr::static_meta(), destList);

				const UINT32 cnt_destList = destList.size();
				for( UINT32 i = 0; i < cnt_destList; i++)
				{
					i3TextureBindAttr * pAttr = (i3TextureBindAttr *) destList.at(i);
					pAttr->SetModified(true);
					pAttr->SetTexture( pNormal);
				}
			}

			if( pSpecular != nullptr)
			{
				i3::vector<i3RenderAttr*> destList;
				i3Scene::FindAttrByExactType( pObj, i3SpecularMapBindAttr::static_meta(), destList);

				const UINT32 cnt_destList = destList.size();
				for( UINT32 i = 0; i < cnt_destList; i++)
				{
					i3TextureBindAttr * pAttr = (i3TextureBindAttr *) destList.at(i);
					pAttr->SetModified(true);
					pAttr->SetTexture( pSpecular);
				}
			}

			if( pReflectMask != nullptr)
			{
				i3::vector<i3RenderAttr*> destList;
				i3Scene::FindAttrByExactType( pObj, i3ReflectMaskMapBindAttr::static_meta(), destList);

				const UINT32 cnt_destList = destList.size();
				for( UINT32 i = 0; i < cnt_destList; i++)
				{
					i3TextureBindAttr * pAttr = (i3TextureBindAttr *) destList.at(i);
					pAttr->SetModified(true);
					pAttr->SetTexture( pReflectMask);
				}
			}
		}

		// UV Coord�� �ٲ��ش�.
		{
			VEC3D vec3;
			VEC2D vec2;
			VEC2D offset, scale;
			const AVATAR::TEXINFO * pTexInfo = AVATAR::getPartTexInfo( AVATAR::VIEW_3P, part);

			offset.x = (REAL32) pTexInfo->m_X / AVT_TEX_WIDTH;
			offset.y = (REAL32) pTexInfo->m_Y / AVT_TEX_HEIGHT;

			scale.x = (REAL32) pTexInfo->m_W / AVT_TEX_WIDTH;
			scale.y = (REAL32) pTexInfo->m_H / AVT_TEX_HEIGHT;

			// Texture Coords.
			i3::vector<i3Node*> list;
			i3Scene::FindNodeByType( pObj, i3Geometry::static_meta(), list);

			UINT32 cnt = list.size();
			for( UINT32 i = 0; i < cnt; i++)
			{
				i3Geometry * pGeo = static_cast<i3Geometry*>(list.at(i));
				INT32 attrCnt = pGeo->GetGeometryAttrCount();

				for( INT32 j = 0; j < attrCnt; j++)
				{
					i3GeometryAttr * pGeoAttr = static_cast<i3GeometryAttr*>(pGeo->GetGeometryAttr( j));
					i3VertexArray * pVA = pGeoAttr->GetVertexArray();
					i3IndexArray * pIA = pGeoAttr->GetIndexArray();

					pVA->setThreadMode( false);
					pIA->setThreadMode( false);

					i3VertexArray * pNewVA = nullptr;//g_pCharaManager->GetAvatarManager()->FindFreeVA( pVA->GetCount(), pVA->GetFormat()->GetHasNormal(),
						//pVA->HasTangent(), 2, pVA->GetFormat()->GetBlendIndexCount());

					if( pNewVA == nullptr)
					{
						pNewVA = i3VertexArray::new_object();
					#if defined( I3_DEBUG)
						pNewVA->SetName( "EquipVA");
					#endif
						pNewVA->Create( pVA->GetFormat(), pVA->GetCount(), pVA->GetUsageFlag() );
						pNewVA->setRestoreType( I3G_RESTORE_MEM);

					}

					I3ASSERT( pVA->Lock() );
					I3ASSERT( pNewVA->Lock() );

					UINT32 vaCnt = pVA->GetCount();
					for( UINT32 k = 0; k < vaCnt; k++)
					{
						pVA->GetPosition( k, &vec3);
						pNewVA->SetPosition( k, &vec3);

						pVA->GetNormal( k, &vec3);
						pNewVA->SetNormal( k, &vec3);

						{
							// Blend Index
							INT8 l;
							INT8 blendCnt = pVA->GetFormat()->GetBlendIndexCount();
							for( l = 0; l < blendCnt; l++)
							{
								pNewVA->SetBlendIndex( l, k, pVA->GetBlendIndex( l, k));
							}

							blendCnt = pVA->GetFormat()->GetBlendWeightCount();
							for( l = 0; l < blendCnt; l++)
							{
								pNewVA->SetBlendWeight( l, k, pVA->GetBlendWeight( l, k));
							}
						}

						// TexCoord
						pVA->GetTextureCoord( 0, k, &vec2);

						// Normalize
						if( vec2.x < 0.0f)
							vec2.x = 1.0f + vec2.x;

						if( vec2.y < 0.0f)
							vec2.y = 1.0f + vec2.y;

						i3Vector::Mul( &vec2, &vec2, &scale);
						i3Vector::Add( &vec2, &vec2, &offset);
						pNewVA->SetTextureCoord( 0, k, &vec2);

						if( pVA->HasTangent() )
						{
							pVA->GetTangent( k, &vec3);
							pNewVA->SetTangent( k, &vec3);
						}

						if( pVA->HasBinormal() )
						{
							pVA->GetBinormal( k, &vec3);
							pNewVA->SetBinormal( k, &vec3);
						}
					}

					pVA->Unlock();
					pNewVA->Unlock();

					i3GeometryAttr * pNewGeoAttr = i3GeometryAttr::new_object_ref();
					pNewGeoAttr->SetIndexArray( pIA);
					pNewGeoAttr->SetVertexArray( pNewVA, pGeoAttr->GetPrimitiveType(), pGeoAttr->GetPrimitiveCount(), pGeoAttr->GetStartIndex(), false);

					pGeo->RemoveGeometryAttr( pGeoAttr);
					pGeo->AppendGeometryAttr( pNewGeoAttr);

					m_EquipmentVAList.push_back( pNewVA);
				}
			}
		}
	}
}

void Avatar::Create(GLOBAL_CHARA_INFO* pInfo)
{
	addCharaStyle( I3_CHARA_STYLE_HUMAN);

	m_pTempCharaInfo = pInfo;

	SetCharaInfoForLoad( pInfo);
	
	m_pCharaInfo = &m_CharaInfoForLoad;

	TEAM_TYPE team = CHARA::CharaTeamType2TeamType( pInfo->GetTeam());
	T_ItemID charaItemID = pInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
	const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( charaItemID);
	I3ASSERT( pCharaDBInfo != nullptr);
	
	//���� ������ �ִٸ� �������ش�..
	if( pInfo->GetParts() != nullptr)
	{
		m_pGen->Destroy();

		m_pGen->setThreadMode( m_bThreadMode);
		m_pGen->SetTeam( team);
		m_pGen->setFemale( (pCharaDBInfo->GetCharaSexTypePHYSIX() == CHARA::eSEX_TYPE_FEMALE) );
	
		if( m_pCharaInfo->GetType() == CHARA::TYPE_FIRST_VIEW_PLAYER)
			m_pGen->setLoadStyle( AVATAR_LOAD_STYLE_1PV);
		else if( m_pCharaInfo->GetType() != CHARA::TYPE_PROFILE)
			m_pGen->setLoadStyle( AVATAR_LOAD_STYLE_MERGE);
		else
			m_pGen->setLoadStyle( 0);
		
		if( g_pFramework->GetEnableNormalMap() == false )	// Off���·� Ŭ���̾�Ʈ�� Ų���.
			m_pGen->setEnableTexture( AVATAR::TEX_NORM, false);
		else
			m_pGen->setEnableTexture( AVATAR::TEX_NORM, g_pEnvSet->m_bEnableNormalMap?true:false);

		if( g_pFramework->GetEnableSpecularMap() == false)	// Off ����
			m_pGen->setEnableTexture( AVATAR::TEX_SPEC, false);
		else
			m_pGen->setEnableTexture( AVATAR::TEX_NORM, (g_pEnvSet->m_nSpecularQualityType>0)?true:false);

		SetParts( pInfo->GetParts(), (m_pCharaInfo->GetType() == CHARA::TYPE_PROFILE || m_pCharaInfo->GetType() == CHARA::TYPE_FIRST_VIEW_PLAYER));
	}

	{
		// 0.6ms
		{
			// �ӽ� ĳ���� �ٵ��� ���ҽ��� ����
			// vv3 - check
			T_ItemID charaID = pInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
			// ���� ĳ���Ͱ� �ƴϰų� ���� ĳ���� �����̶��
			// pef�� ��ϵ� ���� ĳ���� ������ ����
			if( (g_pCharaManager->IsDummyCharaID( charaID) == false) ||
				(g_pEnvSet->isFixedViewCharacter() == true ))
			{
				if( team == TEAM_RED)
					charaID = pCharaDBInfo->GetTeamRed_ItemID();
				else
					charaID = pCharaDBInfo->GetTeamBlue_ItemID();
			}
			I3ASSERT( charaID > 0);
			i3Body * pSrcBody = g_pCharaManager->GetDummyCharacterBody( charaID, team);
			I3ASSERT( pSrcBody != nullptr);

			I3_SAFE_RELEASE( m_pBody);
			m_pBody = (i3Body*) i3Scene::CreateClone( pSrcBody, I3_COPY_INSTANCE);
			I3_MUST_ADDREF(m_pBody);
			m_bUseDummyResource = true;

			__SearchTeamBandSg();
			__SearchHeadSg();

			// 0.5ms ~ 1ms
			// Dummy Body�� TeamBand�� ����
			/*if( g_pFramework->IsBattleStage() &&
				(g_pEnvSet->m_bTeamBand == true) &&	(BattleSlotContext::i()->getMyTeam() != getCharaTeam()) )
			{
				SetTeamBand();
			}
			else
				DisableTeamBand();*/
		}

		// FrameLOD�� �ٽ� ����
		INT32 base = _FindFrameLOD( m_pBody);
		
		if( base != -1)
		{	// 0.09ms
			m_pBody->setFrameLOD( m_pBody->getLOD( base));
			m_pBody->Prepare(false);
		}

		//���ӳ� ȸ������ ����..
		{
			if( m_pTrans != nullptr)
			{
				m_pTrans->RemoveFromParent();
				I3_MUST_RELEASE(m_pTrans);
			}
			m_pTrans = i3Transform::new_object();

			m_pTrans->AddChild(m_pBody);

			MATRIX matRes;
			m_pTrans->SetMatrix(&matRes);
			MATRIX mat;
			i3Matrix::SetRotateX(&mat,I3_DEG2RAD(-90.0f));
			i3Matrix::Mul(&matRes,&matRes,&mat);

			i3Matrix::SetRotateY(&mat,I3_DEG2RAD(-180.0f));
			i3Matrix::Mul(&matRes,&matRes,&mat);
			m_pTrans->SetMatrix(&matRes);

			I3_MUST_ADDREF(m_pTrans);
			i3Chara::Create(m_pTrans);
			I3_MUST_RELEASE(m_pTrans);
		}

		i3Node * pRoot = getSceneObject();
		I3ASSERT( pRoot != nullptr);

		ISOLATE_INFO info;
		info.m_pNode = nullptr;
		info.m_bStatus = false;

		// 0.01ms
		i3Scene::Traverse( pRoot, _AvatarIsolateNodeProc, &info);
				
		//�ӽ�ó��(���� �ƹ�Ÿ�� ���ҽ� Ÿ���̶��� ����)
		CHARA_RES_ID resID = CHARA::ItemID2ResID( pCharaDBInfo->GetRepresentID());
		m_pCharaInfo->SetResType( resID);

		if( GetCallBack() == UICharaView::sAfterLoadChara)
		{
			CGameCharaBase::CreateInLobby( pCharaDBInfo);
			m_bInBattle = false;
		}
		else
		{
			CGameCharaBase::CreateFirst( pCharaDBInfo);
			m_bInBattle = true;
		}
	}
}


void Avatar::SetPartsEquipInfo( const CHARA_PARTS_INFO * pPartsInfo)
{
	// Avatar �� Parts�� ���� ���� ����
	if( getEquipContext() != nullptr)
	{	// 0.01ms
		// vv3 - check
		for( INT32 idx = 0 ; idx < CHAR_EQUIPMENT_PARTS_COUNT ; ++idx)
		{
			T_ItemID item_id =  pPartsInfo->GetPartsItemID( static_cast<CHAR_EQUIPMENT_PARTS>(idx) );

			if( item_id > 0 || pPartsInfo->IsSkipValidCheck(idx) == false )
			{
				I3ASSERT( item_id > 0);
				const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo( item_id );

				EQUIP::ePART eParts = EQUIP::CommonParts2eParts( static_cast<CHAR_EQUIPMENT_PARTS>(idx) );
				getEquipContext()->setEquipInfo( eParts, const_cast<CEquipInfo*>(pEquipInfo) );
			}
		}
	}
}


i3::rc_string Avatar::GetPartName( EQUIP::ePART part) const
{
	return	m_pGen->getPartName( part);
}

void Avatar::SetEquipResource( T_ItemID itemID, INT32 res_subtype)
{
	char szName[ MAX_PATH];
	char szPath[ MAX_PATH];

	EQUIP::ePART part = EQUIP::ItemID2PartsType( itemID);

	const i3GameResSceneGraph * pRes = nullptr;

	i3NamedResourceManager * pManager = i3ResourceFile::GetResourceManager( i3SceneGraphInfo::static_meta());
	I3ASSERT( pManager != nullptr);

	i3BoneRef * pBoneRef = getEquipContext()->getDummy( part);

	if( pBoneRef != nullptr)
	{
		i3SceneGraphInfo * pSgInfo = m_pGen->getSgInfo( part, 1);
		if( pSgInfo != nullptr)
		{
			// ���ҽ� Ȯ��
			// MainThread���� �ε��� �̷���� ��쳪 �̹� �ε��� �Ǿ� �ִ�.
			pRes = g_pCharaManager->GetEquipResource( part, itemID, getTeam(), res_subtype);
			if( pRes == nullptr)
			{	// �ε����� �ʾҴٸ�
				// Thread���� �ε��� Resource�� �������.
				m_pGen->GetResPath( part, 0, szPath, sizeof( szPath));
				pManager->AddResource( pSgInfo, szPath);

			//	i3String::SplitFileName( szPath, szName, true);
				i3::extract_filename(szPath, szName);

				// GameRes�� ���
				pRes = (i3GameResSceneGraph*) g_pFramework->QuaryResource( szName);
				I3ASSERT( pRes != nullptr);
				g_pCharaManager->SetEquipResource( itemID, getTeam(), res_subtype, pRes);
			}
		}
		else
		{	// ��� ����.
			// �ε�
			pRes = g_pCharaManager->QueryEquipResource( itemID, getTeam(), res_subtype);
		}
	}
}

//void Avatar::SetDummyBoneRef(const char* szBoneName, i3BoneRef* destBoneRef)
//{
//	i3LOD* pLOD = getSceneObject()->GetBody()->getFrameLOD();
//
//	INT32 idx = pLOD->getSkeleton()->FindBoneByName(szBoneName);
//
//	if (destBoneRef)
//	{
//		destBoneRef->setBoneIndex(idx);
//		destBoneRef->SetName(szBoneName);
//
//		m_pBody->AddChild(destBoneRef);
//	}
//}

bool Avatar::OnUpdateAvatarGen_InThread( void)
{
	if( m_bLoaded)
		return true;

	if( m_pGen != nullptr)
	{
		// Avatar Gen�� Update ȣ��.
		// ���⿡���� �׻� ȣ���ϰ� ������, ��� Loading�� �Ͼ�� ���� �ʴٸ� ȣ��� �ʿ����.
		m_pGen->UpdateInThread();

		// ��� Loading�� ������, Geometry �� Texture�� ���� �籸���� ��� �Ϸ�Ǹ�
		// isReadyScene() �Լ��� true�� ��ȯ�Ѵ�.
		if( m_pGen->isReadyScene())		
		{
			// ĳ���Ͱ� �������Ǿ� ������ ���� �������� �ٿ��ش�.
			// ���������� Dummy�� �ٿ��� �������Ѵ�.

			m_pTempBody = m_pGen->GetGeneratedBody( true);
			if( m_pTempBody != nullptr)	// �ε��� �� �Ǿ����� LostDevice�� �Ǹ� �ٽ� ������ �Ѵ�...
			{
				if( m_bReleaseQue)
				{	// �����Ǿ� �Ѵ�.
					I3_SAFE_RELEASE( m_pGen);
					return true;
				}
				else
				{
					I3_MUST_ADDREF(m_pTempBody);
					m_bLoaded = true;
					return true;
				}
			}
		}
	}

	return false;
}

void Avatar::SetParts( T_ItemID ItemID, bool bInLobby)
{ 
	EQUIP::ePART part = EQUIP::ItemID2PartsType( ItemID);

	//���������� ���ų� Ÿ���� Ʋ���ϴ�..
	if (part == EQUIP::ePART_UNKNOWN)
		return;

	m_pGen->setTextureQuality( g_pFramework->GetTextureQuality() );
	m_pGen->SetPart( ItemID, bInLobby);

	if( part == EQUIP::ePART_CHARA)
	{	// �⺻ ���ҽ� Ÿ��(�Ӹ�) �� �ٲ�� ��ü������ �ٲ��ش�.
		const CCharaInfo * pCharaInfo = g_pCharaInfoDataBase->GetCharaDBInfo( ItemID);
		I3ASSERT( pCharaInfo != nullptr);

		for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
		{
			T_ItemID itemid = pCharaInfo->GetPartsItemID( (EQUIP::ePART) i);

			m_pGen->SetPart( itemid, bInLobby);
		}
	}
	
	m_bLoaded = false;
}


void Avatar::SetParts( const CHARA_PARTS_INFO * pInfo, bool bInlobby, bool bForceRebuild)
{
	m_bLoaded = false;
	m_pGen->setTextureQuality( g_pFramework->GetTextureQuality() );
	// vv3 - check
	T_ItemID itemID = pInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
	if( EQUIP::Valid( itemID))
		m_pGen->SetPart( itemID, bInlobby, bForceRebuild);

	itemID = pInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_SKIN);
	if( EQUIP::ItemID2PartsType(itemID) == EQUIP::ePART_SKIN)
	{
		m_pGen->SetPart( itemID, bInlobby, bForceRebuild);
		return;
	}

	for( INT32 i = CHAR_EQUIPMENT_PARTS_HEAD ; i < CHAR_EQUIPMENT_PARTS_COUNT ; ++ i )
	{
		if( i == CHAR_EQUIPMENT_PARTS_SKIN ) continue;

		itemID = pInfo->GetPartsItemID( (CHAR_EQUIPMENT_PARTS)i );
		if( EQUIP::Valid(itemID))
			m_pGen->SetPart( itemID, bInlobby, bForceRebuild);
	}	
}

void Avatar::SetWeapon( WEAPON_SLOT_TYPE slot, T_ItemID Itemid, bool bIsSameWeapon)
{
	if( Cmd_CreateWeapon( slot, Itemid ) )
	{	// ���⸦ ���� ����������..
		WeaponBase * pWeapon = GetCurrentCharaWeapon();
		if( pWeapon != nullptr)
		{
			// LOD 0������ ����
			i3Body * pBody = pWeapon->getSceneObject()->GetBody();
			pBody->removeStyle( I3BS_LOD_AUTO);
			pBody->setConst( 0.f);
		}
	}

	if( Cmd_ChangeWeapon( slot, true))
	{	// ���� ��ü�� �̷������ AnimContext�� ����.
		Cmd_SyncWeapon();
		getAnimContext()->ChangeAnimationContext();
	}

	Cmd_Profile_Idle_Change(bIsSameWeapon);
}

void Avatar::LostDevice( void)
{
	m_pGen->LostDevice();
}

void Avatar::Revive( i3RenderContext * pCtx)
{
	SetParts( m_pCharaInfo->GetParts(), (m_pCharaInfo->GetType() == CHARA::TYPE_PROFILE), true);
}

void Avatar::__SearchTeamBandSg( void)
{
	i3::vector<i3Node*> nodeList;
	nodeList.reserve( 32);

	UINT32 lodCnt = m_pBody->getLODCount();
	for( UINT32 i = 0; i < lodCnt; i++)
	{
		i3LOD * pLOD = m_pBody->getLOD( i);
		if( pLOD != nullptr)
		{
			i3Scene::FindNodeByExactType( pLOD->getShapeNode(), i3AttrSet::static_meta(), nodeList);
		}
	}

	char szTemp[32];

	UINT32 nodeCnt = nodeList.size();
	m_TeamBandSgList.clear();
	for( UINT32 i = 0; i < nodeCnt; i++)
	{
		i3Node * pNode = nodeList.at( i);
		i3::safe_string_copy( szTemp, pNode->GetName(), sizeof( szTemp));
		i3::to_upper( szTemp);

		if( i3::contain_string( szTemp, "TEAMBAND") >= 0)
		{
			tagTeamBandInfo info;
			info._pNode = pNode;
			info._pAttr = (i3TextureBindAttr*) i3Scene::FindAttrByExactType( pNode, i3TextureBindAttr::static_meta());
			I3ASSERT( info._pAttr != nullptr);

			m_TeamBandSgList.push_back( info);
		}
	}
}
void Avatar::SetCharaInfoForLoad( GLOBAL_CHARA_INFO * pInfo)
{
	I3ASSERT( pInfo != nullptr); 
	pInfo->CopyTo( &m_CharaInfoForLoad); 
	I3ASSERT( m_CharaInfoForLoad.isValid() );
}

void Avatar::SetTeamBand( bool TeamColor)
{
	if( BattleSlotContext::i()->getMySlotIdx() == -1)
	{	// �κ񿡼��� �Ⱥ��̰�
		DisableTeamBand();
		return;
	}

	// �ڽ��� �ϴ� ��ŵ
	if( BattleSlotContext::i()->getMySlotIdx() == getCharaNetIndex() )
		return;

	i3Texture * pChangedTex = nullptr;

	if( TeamColor && getCharaTeam() == CHARACTER_TEAM_BLUE)
	{
		pChangedTex = g_pCharaManager->GetAvatarManager()->GetTeamBandTexture( TEAM_BLUE);
	}
	else
	{
		pChangedTex = g_pCharaManager->GetAvatarManager()->GetTeamBandTexture( TEAM_RED);
	}

	if( pChangedTex != nullptr)
	{
		UINT32 cnt = m_TeamBandSgList.size();
		for( UINT32 i = 0; i < cnt; i++)
		{
			tagTeamBandInfo & info = m_TeamBandSgList.at(i);
			info._pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
			info._pAttr->SetTexture( pChangedTex);
		}
	}
}

void Avatar::DisableTeamBand( void)
{
	UINT32 cnt = m_TeamBandSgList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		tagTeamBandInfo & info = m_TeamBandSgList.at(i);
		info._pNode->AddFlag( I3_NODEFLAG_DISABLE);
	}
}

void Avatar::ProcessCallback(void)
{
	// �ε� �Ϸ�
	if( m_pfGameNodeEvent != nullptr)
	{
		m_pfGameNodeEvent( this, getCharaNetIndex(),  m_pCharaInfo->GetAIBind() );
	}
}

void Avatar::LoadFinish( void)
{
	// CharaManager�� Info�� ��ü
	m_pCharaInfo = m_pTempCharaInfo;

	setCharaInfo()->setLoadSlotIndex( -1);

	if( m_pLoadFinishCallBack != nullptr)
	{
		m_pLoadFinishCallBack( this, getCharaNetIndex(), m_pCharaInfo->GetAIBind());
	}
}

void Avatar::ReturnInstance( void)
{
	// SceneGraph�� �����ϹǷ� Instance ��ȯ ��(Body release��)�� ȣ���ؾ� �Ѵ�.
	if(IsOnObject())
	{
		GetOnObject()->LeaveObject();
	}

	_Destroy();

	size_t num_EquipmentVA = m_EquipmentVAList.size();

	for (size_t  i = 0 ; i < num_EquipmentVA ; ++i)
	{
		if (m_EquipmentVAList[i])
		{
			m_EquipmentVAList[i]->Release();
		}
	}

	m_EquipmentVAList.clear();

	CGameCharaBase::ReturnInstance();
}


static I3SG_TRAVERSAL_RESULT _AvatarIsolateNodeProc( i3Node * pNode, void * pData, i3MatrixStack * pStack)
{
	ISOLATE_INFO * pInfo = (ISOLATE_INFO *) pData;

	bool bVisible = ! pInfo->m_bStatus;

	// �θ� ��忡�� ��� Node�� �ִ��� Ȯ��.
	{
		i3Node * pParent = pNode;

		while( pParent != nullptr)
		{
			if( pParent == pInfo->m_pNode)
			{
				bVisible = true;
				break;
			}

			pParent = pParent->GetParent();
		}
	}

	if( bVisible)
		pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
	else
		pNode->AddFlag( I3_NODEFLAG_DISABLE);

	return I3SG_TRAVERSAL_CONTINUE;
}

void Avatar::KeepResource( void)
{
	i3SceneGraphInfo * pInfo;

	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		for( INT32 j = 0; j < AVT_LOD_COUNT; j++)
		{
			pInfo = m_pGen->getSgInfo( (EQUIP::ePART) i, j);
			pInfo->setSubCommonRes( true);
		}
	}
}

void Avatar::__SearchHeadSg( void)
{
	char szName0[ 32];
	char szName1[ 32];
	char szName2[ 32];

	i3::safe_string_copy( szName0, "Head_0", sizeof( szName0));
	i3::safe_string_copy( szName1, "Head_1", sizeof( szName1));
	i3::safe_string_copy( szName2, "Head_2", sizeof( szName2));

	m_HeadSgList.clear();
	m_HeadSgList.resize( 3);
	m_HeadSgList[0].clear();
	m_HeadSgList[1].clear();
	m_HeadSgList[2].clear();

	UINT32 lodCnt = m_pBody->getLODCount();
	for( UINT32 i = 0; i < lodCnt; i++)
	{
		i3LOD * pLOD = m_pBody->getLOD( i);

		INT32 childCnt = pLOD->getShapeNode()->GetChildCount();
		for( INT32 j = 0; j < childCnt; j++)
		{
			i3Node * pChild = pLOD->getShapeNode()->GetChild( j);

			if( i3::generic_is_iequal( pChild->GetName(), szName0) )
			{
				tagHeadResInfo & info = m_HeadSgList[0];
				info.push_back( pChild);
			}
			else if( i3::generic_is_iequal( pChild->GetName(), szName1) )
			{
				tagHeadResInfo & info = m_HeadSgList[1];
				info.push_back( pChild);
			}
			else if( i3::generic_is_iequal( pChild->GetName(), szName2) )
			{
				tagHeadResInfo & info = m_HeadSgList[2];
				info.push_back( pChild);
			}
		}
	}
}

void Avatar::SetCharaHead( bool bAttachedHeadgear)
{
	AVATAR::HEAD_TYPE headType = AVATAR::HEAD_TYPE_NORMAL;

	// ���Ǿ��ִ� ���� ��������..
	CGameCharaEquip * pEquip = getEquipContext()->getEquip( EQUIP::ePART_HEADGEAR);
	CGameCharaEquip * pFace = getEquipContext()->getEquip( EQUIP::ePART_FACEGEAR);

	if( bAttachedHeadgear )
	{
		if( pEquip != nullptr && pEquip->IsAttached() )
		{
			const CHeadInfo * pHeadInfo = static_cast<const CHeadInfo*>(pEquip->GetInfo());
			if( pHeadInfo != nullptr)
			{
				AVATAR::HEAD_TYPE changeType = pHeadInfo->getAvatarHeadType( getTeam());
				headType = (changeType!=AVATAR::HEAD_TYPE_NONE) ? changeType : headType;
			}
			
			if( pFace != nullptr)
			{
				if(pHeadInfo && pHeadInfo->getFacegearShow( getTeam()) == false)
				{	// Facegear�� Visible�� ����.
					pFace->SetVisibleOnlyScene( false);
				}
			}
		}
	}
	else
	{
		if( pFace != nullptr)
		{  
			// ����� ���� ��� Facegear�� ������ ���̵���
			// ��Ų�� ���ٸ� ����.
			const CSkinInfo * pSkinInfo = static_cast<const CSkinInfo*>(getEquipContext()->getEquipInfo(EQUIP::ePART_SKIN));
			if (pSkinInfo == NULL)
				pFace->SetVisibleOnlyScene( TRUE);

			const CFacegearInfo * pFacegearInfo = static_cast<const CFacegearInfo*>(getEquipContext()->getEquipInfo( EQUIP::ePART_FACEGEAR));
			if( pFacegearInfo != nullptr )
			{	// facegear���� �Ӹ� ����� �����ϴ� ���
				AVATAR::HEAD_TYPE changeType = pFacegearInfo->getAvatarHeadType( getTeam());
				headType = (changeType!=AVATAR::HEAD_TYPE_NONE) ? changeType : headType;
			}
		}
	}

	if( IsDummyResource() == false)	// ���̸� ����ϴ� ���� �Ʒ� �ڵ尡 �۵��ϸ� �ȵȴ�.
	{
		const CSkinInfo * pSkinInfo = static_cast<const CSkinInfo*>(getEquipContext()->getEquipInfo( EQUIP::ePART_SKIN));
		if( pSkinInfo != nullptr)
		{
			//  vv3 - check
			if( (pEquip == nullptr) || ( pSkinInfo->GetHeadgearItemID() == 0) )
			{	// ��� ���� ��� ��Ų�� �ֻ��� ��� Ÿ���� ���´�.
				AVATAR::HEAD_TYPE changeType = pSkinInfo->GetHeadType();
				headType = (changeType!=AVATAR::HEAD_TYPE_NONE) ? changeType : headType;
			}
		}
		else
		{
			const CJacketInfo * pInfo = static_cast<const CJacketInfo*>(getEquipContext()->getEquipInfo( EQUIP::ePART_VEST));
			if( pInfo != nullptr && pInfo->getHeadgearShow( getTeam() ) == false)
			{	// Headgear�� Visible�� �����Ѵ�.
				if( pEquip != nullptr)
					pEquip->SetVisibleOnlyScene( false);

				if( pFace != nullptr)
				{	// ����� �Ⱥ��̹Ƿ� Facegear�� ������ ���̵���
					pFace->SetVisibleOnlyScene( true);
				}

				// �̰���� �켱������ �Ӹ� ��Ÿ���� ���Ѵ�.
				AVATAR::HEAD_TYPE changeType = pInfo->getAvatarHeadType( getTeam());
				headType = (changeType!=AVATAR::HEAD_TYPE_NONE) ? changeType : headType;
			}
		}
	}

	UINT32 cnt = m_HeadSgList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		tagHeadResInfo & info = m_HeadSgList[ i];

		if( i == (UINT32) headType)
		{
			UINT32 size = info.size();
			for( UINT32 j = 0; j < size; j++)
				info.at( j)->RemoveFlag( I3_NODEFLAG_DISABLE);
		}
		else
		{
			UINT32 size = info.size();
			for( UINT32 j = 0; j < size; j++)
				info.at( j)->AddFlag( I3_NODEFLAG_DISABLE);
		}
	}
}

bool Avatar::IsSkinResource( void)
{	// vv3 - check
	return ( m_pCharaInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_SKIN) > 0);
}

void Avatar::SetEyeEnable( bool enable )
{
	// ���� Dummy�� �ִ��� �˻�.
	i3BoneRef* left = getBoneContext()->getEyeDummyLeft();
	if(left == nullptr) return;

	i3BoneRef* right = getBoneContext()->getEyeDummyRight();
	if(right == nullptr) return;

	// Eye Dummy�� �ִ� ���� Effect�� �ִ� ����̴�.
	if(enable)
	{
		i3Node* left_effect = left->GetChild(0)->GetChild(0)->GetChild(0);
		if(i3::same_of<i3TimeSequence*>(left_effect))
			static_cast<i3TimeSequence*>(left_effect)->SetLocalTime(0.0);

		i3Node* right_effect = right->GetChild(0)->GetChild(0)->GetChild(0);
		if(i3::same_of<i3TimeSequence*>(right_effect))
			static_cast<i3TimeSequence*>(right_effect)->SetLocalTime(0.0);
	}
	else
	{
		i3Node* left_effect = left->GetChild(0)->GetChild(0)->GetChild(0);
		if(i3::same_of<i3TimeSequence*>(left_effect))
			static_cast<i3TimeSequence*>(left_effect)->SetLocalTime(999.0f);

		i3Node* right_effect = right->GetChild(0)->GetChild(0)->GetChild(0);
		if(i3::same_of<i3TimeSequence*>(right_effect))
			static_cast<i3TimeSequence*>(right_effect)->SetLocalTime(999.0f);
	}
}
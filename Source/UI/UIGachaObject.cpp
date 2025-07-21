#include "pch.h"
#include "UIGachaObject.h" 
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "UIMath.h"
#include "GameCharaEquipDef.h"
#include "UIPhaseGachaShop.h"
#include "UIFloatCharacterView.h"

#include "MainWeapon_Dual.h"

#include "UIUtil.h"

#include "i3Framework/i3UI/i3UIFilePathMgr.h"

I3_CLASS_INSTANCE( GachaObject);


const char * s_AnimName_Gacha[] =
{

	"Object/Capsule-Machine/Resource/Idle.i3a",
	"Object/Capsule-Machine/Resource/IdleS.i3a",
	"Object/Capsule-Machine/Resource/Idle_Start.i3a",
	"Object/Capsule-Machine/Resource/Idle_End.i3a",
	"Object/Capsule-Machine/Resource/Select_A.i3a",
	"Object/Capsule-Machine/Resource/Select_B.i3a",
	"Object/Capsule-Machine/Resource/Select_C.i3a",

};

//SLOT_COMMON_ANI * aniManager::getType(GACHA_WEAPON_ANI_TYPE t)
//{
//	if(t == GACHA_ANI_MOVE)
//	{
//
//	}
//	return nullptr;
//}
// 
//aniManager::aniManager()
//{
//	for(size_t i=0; i<GACHA_POS_MAX; i++)
//		mSlotAniList[i] = nullptr;
//	m_accumulate_time	= 0.0f;
//	m_play_time			= 0.0f;
//}
//
//aniManager::~aniManager()
//{
//}
//
//void aniManager::OnStart(REAL32 play_time)
//{
//	
//	for(size_t i=0; i<GACHA_POS_MAX; i++)
//	{
//		if(mSlotAniList[i] != nullptr && mSlotAniList[i]->_weaponBase != nullptr)
//		{
//			mSlotAniList[i]->accumulateTime = 0.0f;
//			mSlotAniList[i]->play_time		= 0.8f;
//			mSlotAniList[i]->_weaponBase->SetEnable(true);
//		}
//	}
//}
//
//void aniManager::Clear()
//{
//
//}
//
//void aniManager::OnUpdate(REAL32 rDeltaTime)
//{
//	for(size_t i=0; i< GACHA_POS_MAX; i++)
//	{
//		AttrAni * pAni = mSlotAniList[i];
//		if(pAni == nullptr)
//			continue;
//		if(pAni->play_time != 0.f)
//		{
//			pAni->accumulateTime += rDeltaTime;
//			if(pAni->_type == GACHA_ANI_MOVE)
//			{
//				REAL32 ratio = uiMath::inverse_interpolate(pAni->accumulateTime, 0.f, pAni->play_time);
//				ratio = uiMath::limited( ratio, 0.0f, 1.0f );
//				(m_ani)(pAni, ratio);
//
//				if( ratio >= 1.0f )
//				{
//					pAni->accumulateTime = 0.f;
//					pAni->play_time = 0.f;
//				}
//			}
//			else if(pAni->_type == GACHA_ANI_SHAKE)
//			{
//				REAL32 ratio = uiMath::inverse_interpolate(pAni->accumulateTime, 0.f, pAni->play_time);
//				ratio = uiMath::limited( ratio, 0.0f, 1.0f );
//				if( ratio >= 1.0f )
//				{
//					pAni->accumulateTime = 0.f;
//				}
//			}
//			
//		}
//	}
//}
//
//void aniManager::setInvert(bool bInvert)
//{
//	for(size_t i=0; i<GACHA_POS_MAX; i++)
//	{
//		if(mSlotAniList[i] != nullptr)
//			mSlotAniList[i]->_invert = !(mSlotAniList[i]->_invert);
//	}
//
//}
//
//void aniManager::moveTarget(size_t pos)
//{
//	AttrAni * pAni = mSlotAniList[pos];
//	pAni->_weaponBase->SetPos(pAni->target_pos_x, pAni->target_pos_y, pAni->target_pos_z);
//}
//
//void aniManager::moveOrigin(size_t pos)
//{
//	AttrAni * pAni = mSlotAniList[pos];
//	pAni->_weaponBase->SetPos(pAni->origin_pos_x, pAni->origin_pos_y, pAni->origin_pos_z);
//}
//
//void SLOT_WEAPON_ANI::operator()(AttrAni* pAttr, REAL32 ratio)
//{
//	REAL32 factor = (pAttr->_invert) ? ratio : (1.f - ratio);
//	REAL32 xPos = uiMath::interpolate(factor, (REAL32)pAttr->origin_pos_x, (REAL32)pAttr->target_pos_x);
//	REAL32 yPos = uiMath::interpolate(factor, (REAL32)pAttr->origin_pos_y, (REAL32)pAttr->target_pos_y);
//	REAL32 zPos = uiMath::interpolate(factor, (REAL32)pAttr->origin_pos_z, (REAL32)pAttr->target_pos_z);
//	pAttr->_weaponBase->SetPos(xPos,yPos,zPos);
//}

GachaObject::GachaObject()
{
	for(INT32 i=0; i<GACHA_CASE_MAX; i++)
	{
		m_pAnim[i] = nullptr;
	}
	for(INT32 i=0; i<GACHA_POS_MAX; i++)
	{
		//m_pWeapon[i]	= nullptr;
		m_pWeaponPos[i] = nullptr;
		m_pDualWeaponPos[i] = nullptr;

		m_TexInfo[i] = nullptr;
		//m_ItemImg[i] = i3Quad::new_object();
		//m_ItemImg[i]->Create(1, I3SG_QUAD_SPACE_WORLD, true, false, true );

		m_ItemImg[i] = i3Sprite::new_object();
		m_ItemImg[i]->Create(1, true, false, true );

		// ������ �̹����� ����Ʈ ������ ������ ��ο����� ������(����Ʈ�� ��ο���) ����Ʈ �Ӽ� ��.
		i3LightingEnableAttr *pLightEnable		= i3LightingEnableAttr::new_object_ref();
		pLightEnable->Set( true );
		m_ItemImg[i]->AddAttr( pLightEnable );	
		m_ItemID[i] = 0;
	}
	m_pGachaState		= GACHA_STATE_IDLE;
	m_eGachaAnimState	= GACHA_PLAY_IDLE;
	m_rStateTime		= 0.0f;
	m_ImgBox			= nullptr;
	
	m_pTexture			= nullptr;
	m_pEquipNode		= nullptr;

	//m_pWeaponDummyPos	= nullptr;
}

/*virtual*/ GachaObject::~GachaObject()
{
	for(INT32 i=0; i<GACHA_CASE_MAX; i++)
	{
		I3_SAFE_RELEASE( m_pAnim[ i]);
	}
	for(INT32 i=0; i<GACHA_POS_MAX; i++)
	{
		I3_SAFE_RELEASE(m_ItemImg[i]);
	}
	I3_SAFE_RELEASE(m_pTexture);
}

bool GachaObject::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;


	for(INT32 i=0; i<GACHA_POS_MAX; i++)
	{
		m_ItemImg[i]->SetTexture((i3Texture*)nullptr);
	}
	
	return true;
}

bool GachaObject::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	i3::pack::RECT	shaperect;

	for(INT32 i = 0; i < GACHA_POS_MAX; i++)
	{
		if( m_ItemID[i] == 0 ) continue;

		ITEM_TYPE type = (ITEM_TYPE) GET_ITEM_TYPE(m_ItemID[i]);
		if(ITEM_TYPE_CHARA <= type && type <= ITEM_TYPE_SKIN)
		{
			SetAvatarTexture((GACHA_WEAPON_POS)i, m_ItemID[i], &shaperect);
		}
		else if(ITEM_TYPE_PRIMARY <= type && type <= ITEM_TYPE_THROWING2)
		{
			SetWeaponTexture((GACHA_WEAPON_POS)i, m_ItemID[i], &shaperect);
		}
		else
		{
			SetItemTexture((GACHA_WEAPON_POS)i, m_ItemID[i], &shaperect);
		}


		m_ItemImg[i]->SetEnable(0, true);

		const VEC2D& scale = GachaItem::i()->GetItemSprScale(i, m_ItemID[i]);
		m_ItemImg[i]->SetSize(0, scale.x, scale.y);

		const VEC3D& rotation = GachaItem::i()->GetItemSprRotation(i, m_ItemID[i]);
		m_ItemImg[i]->SetRotationX(0, I3_DEG2RAD(rotation.x));
		m_ItemImg[i]->SetRotationY(0, I3_DEG2RAD(rotation.y));
		m_ItemImg[i]->SetRotation(0, I3_DEG2RAD(rotation.z));

		m_ItemImg[i]->SetColor(0, 255, 255, 255, 255);
	}
	

	return true;
}

void GachaObject::Create( i3Framework * pFramework, i3GameResObject * pRes )
{
	pRes = (i3GameResObject *) g_pFramework->QuaryResource("Gacha.i3Obj");

	
	if( pRes)
	{
		i3Object* pObj = pRes->getGameObject();

		const INT32 numState = pObj->getStateCount();

		for (INT32 i = 0 ; i < numState ; ++i)
		{
			I3_OBJECT_STATE* pObjState = pObj->getState(i);

			i3Node* pNode =	pObjState->m_pSg->getInstanceSg();

			pNode->RemoveFlag(I3_NODEFLAG_ALPHASORT);			// ������Ʈ�� ���ļ�Ʈ �÷��װ� �̱�������� ���� ���װ� ���Ƿ�, �ϴ� �÷��׸� ���Ӵϴ�.
		}														// �ٺ� �ذ��� �ƴϱ� ������, ���������� ���� �̱���������� ���ļ�Ʈ ���׸� ��� ���캸�ڽ��ϴ�.
																// 2015.08.03 ����.
		if(m_pAnim[0] == nullptr)
		{
			InitAnimation();
		}
		i3Object::Create(pFramework, pRes);
	}

	InitObject((i3GameResObject*)pRes);
}

void GachaObject::InitObject(i3GameResObject* pRes)
{
	i3Node * pNode = this->getSceneObject();
	i3Node * pRootNode = i3Scene::FindNodeByName(pNode, "ObjectDummy");
	char dummyName[256] = {0,};
	for(INT32 i=0; i<GACHA_POS_MAX; i++)
	{
		if( i == GACHA_POS_4)
			sprintf_s(dummyName, "PointDummy_M");
		else if( i < GACHA_POS_4)
			sprintf_s(dummyName, "PointDummy_L%d", i+1);
		else
			sprintf_s(dummyName, "PointDummy_R%d", i-GACHA_POS_4);
		m_pWeaponPos[i] = i3Scene::FindNodeByName(pRootNode, dummyName);
	}

	for(INT32 i=0; i<GACHA_POS_MAX; i++)
	{
		if( i == GACHA_POS_4)
			sprintf_s(dummyName, "PointDummy_M_Dual");
		else if( i < GACHA_POS_4)
			sprintf_s(dummyName, "PointDummy_L%d_Dual", i+1);
		else
			sprintf_s(dummyName, "PointDummy_R%d_Dual", i-GACHA_POS_4);
		m_pDualWeaponPos[i] = i3Scene::FindNodeByName(pRootNode, dummyName);
	}	
}

void GachaObject::ClearData(int pos)
{
	//m_AniAttr[pos].ClearData();
	//if(m_pWeapon[pos] != nullptr)
	//	m_pWeapon[pos]->DetachNode();
	if(m_pEquipNode != nullptr)
	{
		m_pWeaponPos[pos]->RemoveChild(m_pEquipNode);
	}

	if(m_ItemImg[pos]->GetEnable(0) == true)
	{	
		m_pWeaponPos[pos]->RemoveChild(m_ItemImg[pos]);
		m_ItemImg[pos]->SetEnable(0, false);

		// Texture ���۷��� ī���ö����� �����Ҷ� breakâ�� ��Ÿ���� ���� ���� ����.
		m_ItemImg[pos]->SetTexture(m_pTexture);
	}
}
 
//void GachaObject::AddWeaponToPos(GACHA_WEAPON_POS pos, T_ItemID id)
//{
//	ClearData( pos);
//
//	if(id == 0)
//		return;
//
//	CGameCharaBase * pChara = nullptr;
//
//	UIFloatCharacterView * pCharaViewWin = static_cast<UIFloatCharacterView*>(GameUI::GetFloating(UFW_CHARACTER));
//	if( pCharaViewWin != nullptr)
//		pChara = pCharaViewWin->GetCharaView()->GetCharacter();
//
//	m_pWeapon[pos] = g_pWeaponManager->QueryWeapon( pChara, id);
//	if( m_pWeapon[pos] != nullptr)
//	{
//		// ����ҽ� �ֺ� �𵨸� ��� ����.. weapon.pef ���� ����
//		m_pWeapon[pos]->SetGachaBrightness();
//
//		m_pWeapon[pos]->SetOwner( pChara);
//		m_pWeapon[pos]->AttachNode( m_pWeaponPos[pos], this);
//
//		WEAPON_CLASS_TYPE _Class = m_pWeapon[pos]->getWeaponInfo()->GetTypeClass();
//		if(_Class == WEAPON_CLASS_DUALHANDGUN ||
//			_Class == WEAPON_CLASS_DUALSMG ||
//			_Class == WEAPON_CLASS_DUALKNIFE )
//		{
//			m_pDualWeaponPos[pos]->RemoveAllChild();
//			AttachToDualNode(m_pWeapon[pos], m_pDualWeaponPos[pos]);	 
//		}
//		setWeaponPosition(m_pWeapon[pos]);
//
//		m_pWeapon[pos]->SetEnable(true);
//	}
//	else
//	{
//		i3GameResSceneGraph * pRes = nullptr;
//		char szFullPath[ MAX_PATH] = {0,};
//
//		g_pCharaManager->GetEquipPath( szFullPath, EQUIP::ItemID2PartsType(id), EQUIP::ItemID2ResID(id), TEAM_RED);
//		if( szFullPath[0] != 0)
//		{
//			pRes = (i3GameResSceneGraph*) g_pFramework->LoadResource( szFullPath, 0);
//			I3ASSERT(pRes != nullptr);
//			if(pRes != nullptr)
//			{
//				m_pEquipNode = pRes->getScene();
//				I3ASSERT(m_pEquipNode != nullptr);
//				m_pWeaponPos[pos]->AddChild(m_pEquipNode);
//			}
//		}
//	}
//
//	i3SceneTracer * pTracer;
//	pTracer = g_pFramework->GetViewer()->GetSceneTracer();
//	pTracer->GetModelViewMatrixStack()->Reset();
//	pTracer->SetTime( g_pFramework->GetViewer()->getDeltaTime() );
//	getSceneObject()->OnUpdate( pTracer);
//}
//
//void GachaObject::AttachToDualNode(WeaponBase * pBase, i3Node * pSceneNode)
//{
//	WeaponBase * pLeft;
//	
//	MainWeapon* mainWeaponBase = pBase->GetMainWeapon();
//	
//	if ( mainWeaponBase->IsWeaponDualDerivedClass() )				// WeaponDual���� ���ܵǾ��� ������, �ϴ� �Ұ����ϰ� �߰�..(2013.11.21.����)
//	{
//		pLeft = static_cast<MainWeapon_Dual*>(pBase->GetMainWeapon())->getLeftWeapon();		
//	}
//	else
//	{
//		WeaponDual * pDual = (WeaponDual*)pBase;
//		pLeft = pDual->getLeftWeapon();
//	}
//
//	if(pLeft != nullptr)
//	{
//		pLeft->AttachNode( pSceneNode, this);
//		pLeft->ChangeGunDummy( WEAPON_ATTACH_GRIP);
//		pLeft->SetEnable();
//	}
//}

void GachaObject::InitAnimation()
{
	for( INT32 i = 0; i < GACHA_CASE_MAX; i++)
	{
		if( s_AnimName_Gacha[i] != nullptr)
		{
			i3Animation * pAnimation = i3Animation::LoadFromFile( s_AnimName_Gacha[i]);
			if( pAnimation == nullptr)
			{
				I3PRINTLOG(I3LOG_WARN, "Couldn't load Animation : %s", s_AnimName_Gacha[i]);
			}

			I3_REF_CHANGE( m_pAnim[i], pAnimation);
			I3_SAFE_RELEASE(pAnimation);  // LoadFromFile()�� ���۷���ī������ ���� ����.(2012.10.18.����)
		}
	}
}

void GachaObject::OnUpdate( REAL32 tm)
{
	i3Object::OnUpdate(tm);
	//m_AniManager.OnUpdate(tm);

	// Texture �ε尡 �ȵǾ����� üũ�ϴ� ��
	i3VirtualTexture* m_pVTex = i3UI::getVirtualTexture();
		
	if (m_pVTex != nullptr)
	{

		for(INT32 i = 0; i < GACHA_POS_MAX; i++)
		{
			if(m_TexInfo[i] != nullptr)
			{
				if(m_TexInfo[i]->m_bLoaded)
				{
					if (m_TexInfo[i]->m_pWeakTex)
					{
						m_ItemImg[i]->SetTexture((i3Texture*) m_TexInfo[i]->m_pWeakTex->m_pRealTex);
					}
					m_TexInfo[i] = nullptr;
				}
				else
				{	// �ε����� �ʾ����� Validate_Legacyȣ��....( DeviceLost ���Ŀ� ���� UIControl���� ��� ������ ���� �����̰�,
					// m_ImgBoxList ��� �־����...Parent�� ��� Renderer�� Validate_Legacy�� ȣ����� ����)
					m_pVTex->Validate_Legacy(m_TexInfo[i], nullptr);		// �ι�°�μ��� �������� �� �Լ��� ���� �ʴ´�.
					// ���� �������̸� m_ImgBoxList�� �ƿ� ���ִ°� ���ڴµ�, �̰� ������ ���� �� �׽�Ʈ�� ���ľ��ϹǷ� �ϴ� ��ġ..
					// 2015.01.09.����.
				}
			}
		}
	}
}

void GachaObject::SetGachaAnimState(GACHA_ANIM_STATE eState,REAL32 rStartTime, I3_ANIM_PLAY_MODE t1, I3_ANIM_PLAY_MODE t2)
{
	m_rStateTime		= 0.f;
	m_eGachaAnimState	= eState;

	//for(INT32 i=0; i<GACHA_POS_MAX; i++)
	//{
	//	if(m_pWeapon[i] != nullptr)
	//		m_pWeapon[i]->SetEnable(true);
	//}
	m_pSceneObj->SetBody(nullptr);
	m_pSceneObj->OnChangeTime(0.0f);
	if( m_pAnim[eState] != nullptr)
		m_pSceneObj->PlayAnim( m_pAnim[eState],I3_ANIM_BLEND_MODE_BLEND,rStartTime, 0, 1.f, 1.f, t1, t2);
}

void GachaObject::DeleteWeaponData(void)
{
	for(INT32 i=0; i<GACHA_CASE_MAX; i++)
	{
		if(m_pAnim[i] != nullptr)
		{
			I3_SAFE_RELEASE( m_pAnim[ i]);
		}
		
	}
	for(INT32 i=0; i<GACHA_POS_MAX; i++)
	{
		ClearData( i);
		if(m_pWeaponPos[i] != nullptr)
		{
			m_pWeaponPos[i]->RemoveAllChild();
		}
	}

 	INT32 size = m_ImgBoxList.size();
 	for(INT32 i=0; i<size; i++)
 	{
 		i3UI::DeleteControl(m_ImgBoxList.back());
 		I3_SAFE_RELEASE(m_ImgBoxList.back());
 		m_ImgBoxList.pop_back();
 	}
 	m_ImgBoxList.clear();
}


void GachaObject::AddItemToPos( GACHA_WEAPON_POS pos, T_ItemID id, i3UI3DViewBox* viewBox)
{
	
	ClearData( pos);

	if(id == 0)
		return;

	m_ItemID[pos] = id;
	
	i3Texture* texture;
	i3::pack::RECT	shaperect;
	
	ITEM_TYPE type = (ITEM_TYPE) GET_ITEM_TYPE(id);
	if(ITEM_TYPE_CHARA <= type && type <= ITEM_TYPE_SKIN)
	{
		texture = _getAvatarTexture(pos, id, &shaperect, viewBox);
	}
	else if(ITEM_TYPE_PRIMARY <= type && type <= ITEM_TYPE_THROWING2)
	{
		texture = _getWeaponTexture(pos, id, &shaperect, viewBox);
	}
	else
	{
		texture = _getItemTexture(pos, id, &shaperect, viewBox);
	}

	// �̹� ������ texture�� Quad�� ���̱�
	if(texture != nullptr)
	{
		m_ItemImg[pos]->SetTexture(texture);
	}

	m_ItemImg[pos]->SetEnable(0, true);
	
	const VEC2D& scale = GachaItem::i()->GetItemSprScale(pos, id);
	m_ItemImg[pos]->SetSize(0, scale.x, scale.y);

	const VEC3D& rotation = GachaItem::i()->GetItemSprRotation(pos, id);
	m_ItemImg[pos]->SetRotationX(0, I3_DEG2RAD(rotation.x));
	m_ItemImg[pos]->SetRotationY(0, I3_DEG2RAD(rotation.y));
	m_ItemImg[pos]->SetRotation(0, I3_DEG2RAD(rotation.z));

	m_ItemImg[pos]->SetColor(0, 255, 255, 255, 255);
	m_ItemImg[pos]->SetTextureCoord(0, shaperect.left, shaperect.top, shaperect.right, shaperect.bottom);

	m_pWeaponPos[pos]->AddChild(m_ItemImg[pos]);
}

i3Texture* GachaObject::_getItemTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect, i3UI3DViewBox* viewBox)
{
	// �������� 512 * 512 ũ�� �̹Ƿ� uv ��ǥ�� ���� ����
	const REAL32 uvRatio = 0.001953125f;

	ShopItemInfo* info = g_pShopItemInfoDataBase->getInfo(id);
	if(info == nullptr)
		return nullptr;

	i3VirtualTexture* m_pVTex = i3UI::getVirtualTexture();
	if(m_pVTex == nullptr)
		return nullptr;

	i3::stack_string filename, templatename;

	i3::rc_string strCommonUIRootFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

	ITEM_TYPE type = info->GetType();
	switch(type)
	{
	case ITEM_TYPE_WRAP_COUNT:
		i3::sprintf(filename, "%s/Image/Item/item_cash_A%d.tga", strCommonUIRootFolder, info->GetTexIndex());
		i3::sprintf(templatename, "BtImgSet_ItemCash_A%d", info->GetTexIndex());
		break;
	case ITEM_TYPE_WRAP_PERIOD:
		i3::sprintf(filename, "%s/Image/Item/item_cash_B%d.tga", strCommonUIRootFolder, info->GetTexIndex());
		i3::sprintf(templatename, "BtImgSet_ItemCash_B%d", info->GetTexIndex());
		break;
	}
	
	I3VT_TEXINFO* tex = m_pVTex->FindRefTexture(filename.c_str());

	if(tex == nullptr)
		return nullptr;

	tex->m_bBlockAutoTrashTex = true;
	i3UITemplate* image_tmp = i3UI::getUILibrary()->FindByName( templatename.c_str());
	i3UIShape* shape = image_tmp->getShape(info->GetShapeIndex());
	i3::pack::RECT* shape_rect = shape->getTextureCoord();
	
	// Rect ����
	rect->left = shape_rect->left * uvRatio; 
	rect->top = shape_rect->top * uvRatio;
	rect->right = (shape_rect->left + shape_rect->right) * uvRatio; 
	rect->bottom = (shape_rect->top + shape_rect->bottom) * uvRatio;

	if(tex->m_bLoaded != 0)
	{
		return (i3Texture*)	 tex->m_pWeakTex->m_pRealTex;
	}

	// ���⿡ ������ Texture�� �ε��Ǿ� ���� ���� ��
	VEC2D vSize;
	VEC3D vPos;
	i3Vector::Set(&vSize, 1.0, 1.0);
	i3Vector::Set(&vPos, 5000.0, 0.0, 0.0);

	m_ImgBox = (i3UIButtonImageSet*) i3UI::CreateControl(image_tmp, &vPos, &vSize, viewBox->getParent());
	m_ImgBox->SetShapeIdx(info->GetShapeIndex());
	I3_MUST_ADDREF(m_ImgBox);
	m_ImgBoxList.push_back(m_ImgBox);

	m_TexInfo[pos] = tex;

	return nullptr;
}

void GachaObject::SetAvatarTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect)
{
	const REAL32 uvRatio = 0.0009765625f;
	ITEM_TYPE type; 

	i3VirtualTexture* m_pVTex = i3UI::getVirtualTexture();
	if(m_pVTex == nullptr)
		return;

	const CEquipInfo* item_info = g_pCharaInfoDataBase->GetInfo(id);
	if(item_info == nullptr)
		return;

	INT32 shape_index = item_info->GetUIShapeIndex();
	i3UITemplate* image_tmp = nullptr;
	type = (ITEM_TYPE) GET_ITEM_TYPE(id);

	i3::stack_string fullPath;

	i3::rc_string strCommonUIRootFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

	I3VT_TEXINFO* pTexInfo = nullptr;

	switch(type)
	{
	case ITEM_TYPE_CHARA:				// ĳ���� ��ü
		i3::sprintf(fullPath, "%s/Image/Avatar/Mask_1.tga", strCommonUIRootFolder);
		pTexInfo = m_pVTex->FindRefTexture(fullPath.c_str());
		image_tmp = i3UI::getUILibrary()->FindByName( "Mask_1");
		break;
	case ITEM_TYPE_HEADGEAR:			// HeadGear
		i3::sprintf(fullPath, "%s/Image/Avatar/HeadGear_1.tga", strCommonUIRootFolder);
		pTexInfo = m_pVTex->FindRefTexture(fullPath.c_str());
		image_tmp = i3UI::getUILibrary()->FindByName( "HeadGear_1");
		break;
	case ITEM_TYPE_FACEGEAR:			// FaceGear
		if(shape_index < 50)
		{
			i3::sprintf(fullPath, "%s/Image/Avatar/Facegear_1.tga", strCommonUIRootFolder);
			pTexInfo = m_pVTex->FindRefTexture(fullPath.c_str());
			image_tmp = i3UI::getUILibrary()->FindByName( "Facegear_1");
		}
		else
		{
			i3::sprintf(fullPath, "%s/Image/Avatar/FaceGear_2.tga", strCommonUIRootFolder);
			pTexInfo = m_pVTex->FindRefTexture(fullPath.c_str());
			image_tmp = i3UI::getUILibrary()->FindByName( "FaceGear_2");
		}
		break;
	case ITEM_TYPE_UPPER:				// ����
		i3::sprintf(fullPath, "%s/Image/Avatar/Upper_1.tga", strCommonUIRootFolder);
		pTexInfo = m_pVTex->FindRefTexture(fullPath.c_str());
		image_tmp = i3UI::getUILibrary()->FindByName( "Upper_1");
		break;
	case ITEM_TYPE_LOWER:				// ���� 10
		i3::sprintf(fullPath, "%s/Image/Avatar/Lower_1.tga", strCommonUIRootFolder);
		pTexInfo = m_pVTex->FindRefTexture(fullPath.c_str());
		image_tmp = i3UI::getUILibrary()->FindByName( "Lower_1");
		break;
	case ITEM_TYPE_GLOVE:				// �尩
		i3::sprintf(fullPath, "%s/Image/Avatar/Upper_1.tga", strCommonUIRootFolder);
		pTexInfo = m_pVTex->FindRefTexture(fullPath.c_str());
		image_tmp = i3UI::getUILibrary()->FindByName( "Glove_1");
		break;
	case ITEM_TYPE_BELT:				// ��Ʈ
		i3::sprintf(fullPath, "%s/Image/Avatar/Belt_1.tga", strCommonUIRootFolder);
		pTexInfo = m_pVTex->FindRefTexture(fullPath.c_str());
		image_tmp = i3UI::getUILibrary()->FindByName( "Belt_1");
		break;
	case ITEM_TYPE_HOLSTER:				// ������ 
		i3::sprintf(fullPath, "%s/Image/Avatar/Holster_1.tga", strCommonUIRootFolder);
		pTexInfo = m_pVTex->FindRefTexture(fullPath.c_str());
		image_tmp = i3UI::getUILibrary()->FindByName( "Holster_1");
		break;
	case ITEM_TYPE_BERET:				// ������ 
		i3::sprintf(fullPath, "%s/Image/Avatar/HeadGear_1.tga", strCommonUIRootFolder);
		pTexInfo = m_pVTex->FindRefTexture(fullPath.c_str());
		image_tmp = i3UI::getUILibrary()->FindByName( "HeadGear_1");
		break;
	}

	if(pTexInfo == nullptr)
		return ;

	pTexInfo->m_bBlockAutoTrashTex = true;
	i3UIShape* shape = image_tmp->getShape(shape_index);
	i3::pack::RECT* shape_rect = shape->getTextureCoord();

	// Rect ����
	rect->left = shape_rect->left * uvRatio; 
	rect->top = shape_rect->top * uvRatio;
	rect->right = (shape_rect->left + shape_rect->right) * uvRatio; 
	rect->bottom = (shape_rect->top + shape_rect->bottom) * uvRatio;

	m_TexInfo[pos] = pTexInfo;
}

void GachaObject::SetItemTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect)
{
	const REAL32 uvRatio = 0.001953125f;

	ShopItemInfo* info = g_pShopItemInfoDataBase->getInfo(id);
	if(info == nullptr)
		return;

	i3VirtualTexture* m_pVTex = i3UI::getVirtualTexture();
	if(m_pVTex == nullptr)
		return;

	
	i3::stack_string filename, templatename;

	i3::rc_string strCommonUIRootFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

	ITEM_TYPE type = info->GetType();
	switch(type)
	{
	case ITEM_TYPE_WRAP_COUNT:
		i3::sprintf(filename, "%s/Image/Item/item_cash_A%d.tga", strCommonUIRootFolder, info->GetTexIndex());
		i3::sprintf(templatename, "BtImgSet_ItemCash_A%d", info->GetTexIndex());
		break;
	case ITEM_TYPE_WRAP_PERIOD:
		i3::sprintf(filename, "%s/Image/Item/item_cash_B%d.tga", strCommonUIRootFolder, info->GetTexIndex());
		i3::sprintf(templatename, "BtImgSet_ItemCash_B%d", info->GetTexIndex());
		break;
	}

	I3VT_TEXINFO* tex = m_pVTex->FindRefTexture(filename.c_str());

	if(tex == nullptr)
		return;

	tex->m_bBlockAutoTrashTex = true;
	i3UITemplate* image_tmp = i3UI::getUILibrary()->FindByName( templatename.c_str());
	i3UIShape* shape = image_tmp->getShape(info->GetShapeIndex());
	i3::pack::RECT* shape_rect = shape->getTextureCoord();

	// Rect ����
	rect->left = shape_rect->left * uvRatio; 
	rect->top = shape_rect->top * uvRatio;
	rect->right = (shape_rect->left + shape_rect->right) * uvRatio; 
	rect->bottom = (shape_rect->top + shape_rect->bottom) * uvRatio;


	m_TexInfo[pos] = tex;
}
void GachaObject::SetWeaponTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect)
{
	const REAL32 uvRatio = 0.0009765625f;
	

	i3VirtualTexture* m_pVTex = i3UI::getVirtualTexture();
	if(m_pVTex == nullptr)
		return;

	CWeaponInfo* weapon_info = g_pWeaponInfoDataBase->getWeaponInfo(id);
	if( weapon_info == nullptr)
		return;

	INT32 shape_index = weapon_info->GetUIShapeIndex();
	int image_number = shape_index / 88;														//���Ŀ� �̹����� ���� ������ ����Ǹ� �̰� �����ؾߵ�.... ���� pef�� ����...
	int image_for_shape_index = shape_index % 88;			
	int image_sub_number = image_for_shape_index/60;											//���Ŀ� ���� �̹��� �ε��� a���� �� ���� ������ ����Ǹ� �����ؾߵ�... ���� pef�� ����...								

	i3::rc_string strCommonUIRootFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

	i3::stack_string filename, templatename;
	i3::sprintf(filename, "%s/Image/WeaponShape/weapon_select%d_%c.tga", strCommonUIRootFolder, image_number, image_sub_number?"b":"a" );
	i3::sprintf(templatename, "ButtonImageSet_WeaponShape%d_hs", image_number+1 );

	I3VT_TEXINFO* tex = m_pVTex->FindRefTexture(filename.c_str());

	if(tex == nullptr)
		return;

	i3UITemplate* image_tmp = i3UI::getUILibrary()->FindByName( templatename.c_str() );
	if(image_tmp == nullptr)
		return;

	tex->m_bBlockAutoTrashTex = true;
	i3UIShape* shape = image_tmp->getShape(image_for_shape_index);
	i3::pack::RECT* shape_rect = shape->getTextureCoord();

	// Rect ����
	rect->left = shape_rect->left * uvRatio; 
	rect->top = shape_rect->top * uvRatio;
	rect->right = (shape_rect->left + shape_rect->right) * uvRatio; 
	rect->bottom = (shape_rect->top + shape_rect->bottom) * uvRatio;

	m_TexInfo[pos] = tex;
}

i3Texture* GachaObject::_getAvatarTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect, i3UI3DViewBox* viewBox)
{
	// Avatar�� 1024 * 1024 �̹Ƿ� uv�� ���� ����
	const REAL32 uvRatio = 0.0009765625f;
	ITEM_TYPE type; 

	i3VirtualTexture* m_pVTex = i3UI::getVirtualTexture();
	if(m_pVTex == nullptr)
		return nullptr;

	const CEquipInfo* item_info = g_pCharaInfoDataBase->GetInfo(id);
	if(item_info == nullptr)
		return nullptr;

	INT32 shape_index = item_info->GetUIShapeIndex();
	i3UITemplate* image_tmp = nullptr;

	I3VT_TEXINFO* pTexInfo = nullptr;

	type = (ITEM_TYPE) GET_ITEM_TYPE(id);

	switch(type)
	{
	case ITEM_TYPE_CHARA:				// ĳ���� ��ü
		pTexInfo = m_pVTex->FindRefTexture("Image/Avatar/Mask_1.tga");
		image_tmp = i3UI::getUILibrary()->FindByName( "Mask_1");
		break;
	case ITEM_TYPE_HEADGEAR:			// HeadGear
		pTexInfo = m_pVTex->FindRefTexture("Image/Avatar/HeadGear_1.tga");
		image_tmp = i3UI::getUILibrary()->FindByName( "HeadGear_1");
		break;
	case ITEM_TYPE_FACEGEAR:			// FaceGear
		if(shape_index < 50)
		{
			pTexInfo = m_pVTex->FindRefTexture("Image/Avatar/Facegear_1.tga");
			image_tmp = i3UI::getUILibrary()->FindByName( "Facegear_1");
		}
		else
		{
			pTexInfo = m_pVTex->FindRefTexture("Image/Avatar/FaceGear_2.tga");
			image_tmp = i3UI::getUILibrary()->FindByName( "FaceGear_2");
		}
		break;
	case ITEM_TYPE_UPPER:				// ����
		pTexInfo = m_pVTex->FindRefTexture("Image/Avatar/Upper_1.tga");
		image_tmp = i3UI::getUILibrary()->FindByName( "Upper_1");
		break;
	case ITEM_TYPE_LOWER:				// ���� 10
		pTexInfo = m_pVTex->FindRefTexture("Image/Avatar/Lower_1.tga");
		image_tmp = i3UI::getUILibrary()->FindByName( "Lower_1");
		break;
	case ITEM_TYPE_GLOVE:				// �尩
		pTexInfo = m_pVTex->FindRefTexture("Image/Avatar/Glove_1.tga");
		image_tmp = i3UI::getUILibrary()->FindByName( "Glove_1");
		break;
	case ITEM_TYPE_BELT:				// ��Ʈ
		pTexInfo = m_pVTex->FindRefTexture("Image/Avatar/Belt_1.tga");
		image_tmp = i3UI::getUILibrary()->FindByName( "Belt_1");
		break;
	case ITEM_TYPE_HOLSTER:				// ������ 
		pTexInfo = m_pVTex->FindRefTexture("Image/Avatar/Holster_1.tga");
		image_tmp = i3UI::getUILibrary()->FindByName( "Holster_1");
		break;
	case ITEM_TYPE_BERET:				// ������ 
		pTexInfo = m_pVTex->FindRefTexture("Image/Avatar/HeadGear_1.tga");
		image_tmp = i3UI::getUILibrary()->FindByName( "HeadGear_1");
	}
	
	if(pTexInfo == nullptr)
		return nullptr;

	pTexInfo->m_bBlockAutoTrashTex = true;
	i3UIShape* shape = image_tmp->getShape(shape_index);
	i3::pack::RECT* shape_rect = shape->getTextureCoord();

	// Rect ����
	rect->left = shape_rect->left * uvRatio; 
	rect->top = shape_rect->top * uvRatio;
	rect->right = (shape_rect->left + shape_rect->right) * uvRatio; 
	rect->bottom = (shape_rect->top + shape_rect->bottom) * uvRatio;
	
	if(pTexInfo->m_bLoaded != 0)
		return (i3Texture*)pTexInfo->m_pWeakTex->m_pRealTex;
	
	// ���⿡ ������ Texture�� �ε��Ǿ� ���� ���� ��
	VEC2D vSize;
	VEC3D vPos;
	i3Vector::Set(&vSize, 1.0, 1.0);
	i3Vector::Set(&vPos, 5000.0, 0.0, 0.0);

	m_ImgBox = (i3UIButtonImageSet*) i3UI::CreateControl(image_tmp, &vPos, &vSize, viewBox->getParent());
	m_ImgBox->SetShapeIdx(shape_index);
	I3_MUST_ADDREF(m_ImgBox);
	m_ImgBoxList.push_back(m_ImgBox);

	m_TexInfo[pos] = pTexInfo;

	return nullptr;
}

i3Texture* GachaObject::_getWeaponTexture(GACHA_WEAPON_POS pos, T_ItemID id, i3::pack::RECT* rect, i3UI3DViewBox* viewBox)
{
	// Weapon�� 1024 * 1024 �̹Ƿ� uv�� ���� ����
	const REAL32 uvRatio = 0.0009765625f;
	

	i3VirtualTexture* m_pVTex = i3UI::getVirtualTexture();
	if(m_pVTex == nullptr)
		return nullptr;
	
	CWeaponInfo* weapon_info = g_pWeaponInfoDataBase->getWeaponInfo(id);
	if( weapon_info == nullptr)
		return nullptr;

	INT32 shape_index = weapon_info->GetUIShapeIndex();
	int image_number = shape_index / 88;														//���Ŀ� �̹����� ���� ������ ����Ǹ� �̰� �����ؾߵ�.... ���� pef�� ����...
	int image_for_shape_index = shape_index % 88;			
	int image_sub_number = image_for_shape_index/60;											//���Ŀ� ���� �̹��� �ε��� a���� �� ���� ������ ����Ǹ� �����ؾߵ�... ���� pef�� ����...								
	
	i3::stack_string filename, templatename;
	i3::rc_string strCommonUIRootFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

	i3::sprintf(filename, "%s/Image/WeaponShape/weapon_select%d_%c.tga", strCommonUIRootFolder, image_number, image_sub_number?"b":"a" );
	i3::sprintf(templatename, "ButtonImageSet_WeaponShape%d_hs", image_number+1 );
	
	I3VT_TEXINFO* tex = m_pVTex->FindRefTexture( filename.c_str() );

	if(tex == nullptr)
		return nullptr;
	
	i3UITemplate* image_tmp = i3UI::getUILibrary()->FindByName( templatename.c_str() );
	if(image_tmp == nullptr)
		return nullptr;

	tex->m_bBlockAutoTrashTex = true;
	i3UIShape* shape = image_tmp->getShape(image_for_shape_index);
	i3::pack::RECT* shape_rect = shape->getTextureCoord();

	// Rect ����
	rect->left = shape_rect->left * uvRatio; 
	rect->top = shape_rect->top * uvRatio;
	rect->right = (shape_rect->left + shape_rect->right) * uvRatio; 
	rect->bottom = (shape_rect->top + shape_rect->bottom) * uvRatio;

	if(tex->m_bLoaded != 0)
		return (i3Texture*)	 tex->m_pWeakTex->m_pRealTex;

	// ���⿡ ������ Texture�� �ε��Ǿ� ���� ���� ��
	VEC2D vSize;
	VEC3D vPos;
	i3Vector::Set(&vSize, 1.0, 1.0);
	i3Vector::Set(&vPos, 5000.0, 0.0, 0.0);

	m_ImgBox = (i3UIButtonImageSet*) i3UI::CreateControl(image_tmp, &vPos, &vSize, viewBox->getParent());
	m_ImgBox->SetShapeIdx(image_for_shape_index);
	I3_MUST_ADDREF(m_ImgBox);
	m_ImgBoxList.push_back(m_ImgBox);

	m_TexInfo[pos] = tex;

	return nullptr;
}

#include "pch.h"
#include "GameCharaTexture.h"

i3Node * CharaTextureUtil::FindNodeByContainName( i3Node * pNode, const char * pszName)
{
	if( pNode->GetName() )
	{
		i3::string strUpper = pNode->GetName();
		i3::to_upper( strUpper);

		if( i3::contain_string( strUpper, pszName) >= 0)
			return pNode;
	}

	for( INT32 i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node * pFindNode = FindNodeByContainName( pNode->GetChild(i), pszName);
		if( pFindNode != nullptr)
			return pFindNode;
	}

	return nullptr;
}

i3RenderAttr * CharaTextureUtil::FindTextureBindInAttrSet( i3LOD * pLOD, const char * pszAttrName, i3ClassMeta * pMeta, bool bSevereCheck)
{	
	i3Node * pFindNode = i3Scene::FindNodeByName( pLOD->getShapeNode(), pszAttrName);
	if( pFindNode == nullptr)
	{
		pFindNode = FindNodeByContainName( pLOD->getShapeNode(), pszAttrName);
	}

	i3AttrSet * pAttrSet = nullptr;

	//	AttrSet �˻�
	if( pFindNode != nullptr)
	{
		if( i3::same_of<i3AttrSet*>(pFindNode))
		{				
			pAttrSet = (i3AttrSet *) pFindNode;
		}
		else if( bSevereCheck)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Not i3AttrSet Class (name: %s)", pszAttrName);		
			return nullptr;
		}
	}
	else if( bSevereCheck)
	{	// bSevereCheck ���� true�̸� ������ AttrSet�̸��� ã�� ���ϸ� �ٿ�ȴ�.
		I3PRINTLOG(I3LOG_FATAL,  "Not find Node (name: %s)", pszAttrName);
		return nullptr;
	}	
	else
	{
		I3TRACE( "[%s] warning! Cannot find %s (%s)\n", __FUNCTION__, pMeta->class_name(), pszAttrName);
		return nullptr;
	}

	i3RenderAttr * pRenderAttr = (i3RenderAttr *) i3Scene::FindAttrByExactType( pAttrSet, pMeta);

	//	Attribute�� GatherUp �Ǿ� ���� ��츦 ���
	if( pRenderAttr == nullptr)
	{
		i3Node * pParent = pAttrSet->GetParent();
		while( pParent != nullptr)
		{
			//	�� �θ� AttrSet�̸� nullptr�� �ɶ����� �ش� Attribute�� ã�´�.
			if( i3::kind_of<i3AttrSet*>(pParent))
			{
				pRenderAttr = (i3RenderAttr *) i3Scene::FindAttrByExactType( (i3AttrSet *) pParent, pMeta);

				//	ã�Ҵ�.
				if( pRenderAttr != nullptr)		break;
			}

			pParent = pParent->GetParent();
		}
	}
	//I3ASSERT( pRenderAttr != nullptr);

	return pRenderAttr;
};

void CharaTextureUtil::FindTextureBindSet( i3LOD * pLOD, const char * pszAttrName, CharaTexBindSet * pOut)
{
	I3ASSERT( pOut != nullptr);

	//	AttrSet�� �̸����� �˻��Ͽ� TextureBind ã�� �����Ѵ�.
	pOut->pDiffBind = (i3TextureBindAttr *)		CharaTextureUtil::FindTextureBindInAttrSet( pLOD, pszAttrName, i3TextureBindAttr::static_meta());
	pOut->pNormBind = (i3NormalMapBindAttr *)	CharaTextureUtil::FindTextureBindInAttrSet( pLOD, pszAttrName, i3NormalMapBindAttr::static_meta(), false);
	pOut->pSpecBind = (i3SpecularMapBindAttr *)	CharaTextureUtil::FindTextureBindInAttrSet( pLOD, pszAttrName, i3SpecularMapBindAttr::static_meta(), false);
};
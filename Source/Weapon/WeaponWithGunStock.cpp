#include "pch.h"
#include "WeaponWithGunStock.h"
#include "GlobalVariables.h"
#include "GameCharaWeaponContext.h"
#include "WeaponFunction.h"


I3_CLASS_INSTANCE( WeaponWithGunStock);//, WeaponBase);

WeaponWithGunStock::WeaponWithGunStock()
{
	for( INT32 a=0; a< MAX_WEAPON_LOD; a++) 
	{
		for( INT32 b=0; b< MAX_GUN_STOCK_MODEL; b++) 
		{
			m_pGunStockModel[a][b] = nullptr;
		}
	}
	
	for( INT32 i=0; i< IDLE_MAX; i++) 
	{
		m_aiiUI_Idle_SWAT_Male[i] = -1;
		m_aiiUI_Idle_SWAT_Female[i] = -1;
		m_aiiUI_Idle_REBEL_Male[i] = -1;
		m_aiiUI_Idle_REBEL_Female[i] = -1;
	}
}

WeaponWithGunStock::~WeaponWithGunStock()
{
}

/*virtual*/ void WeaponWithGunStock::PostCreate( void)
{
	CWeaponInfo * pWeaponInfo = getWeaponInfo();

	I3ASSERT( m_pWeaponInfo != nullptr);

	switch( pWeaponInfo->GetTypeClass())
	{
	case WEAPON_CLASS_HANDGUN:	
		{
			// Glock 18 �� ��� ��쿡 ���� �Ⱥ������ϴ� ���Ӹ��� ��			
			i3Body * pBody = getSceneObject()->GetBody();
			I3ASSERT( pBody != nullptr);

			for( UINT32 i = 0; i < pBody->getLODCount(); i++ )
			{
				i3LOD * pLOD = pBody->getLOD( i );		

				m_pGunStockModel[i][0] = (i3Geometry*) i3Scene::FindNodeByName( pLOD->getShapeNode(), "Model_Stock_Front");
				m_pGunStockModel[i][1] = (i3Geometry*) i3Scene::FindNodeByName( pLOD->getShapeNode(), "Model_Stock_Back");
			}

			m_aiiUI_Idle_SWAT_Male[ IDLE_A]		= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleA_SWAT_Male");
			m_aiiUI_Idle_SWAT_Female[ IDLE_A]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleA_SWAT_Female");
			m_aiiUI_Idle_REBEL_Male[ IDLE_A]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleA_REBEL_Male");
			m_aiiUI_Idle_REBEL_Female[ IDLE_A]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleA_REBEL_Female");

			m_aiiUI_Idle_SWAT_Male[ IDLE_B]		= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleB_SWAT_Male");
			m_aiiUI_Idle_SWAT_Female[ IDLE_B]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleB_SWAT_Female");
			m_aiiUI_Idle_REBEL_Male[ IDLE_B]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleB_REBEL_Male");
			m_aiiUI_Idle_REBEL_Female[ IDLE_B]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleB_REBEL_Female");
		}
		break;
	}
}


/*virtual*/ void WeaponWithGunStock::OnUpdate( REAL32 rDeltaSeconds)
{	
	WeaponBase::OnUpdate( rDeltaSeconds);	

	if(g_pFramework->IsBattleStage())	
	{
		// ���� ��� �ְų� ���忡 ����߸����� ���Ӹ����� ���̵���
		if( m_pOwner && m_pOwner->GetCurrentCharaWeapon() == this || 
			getDropInfo()->_bDropping || getDropInfo()->_bDroped)
			SetVisibleGunStock( true);
		else
			SetVisibleGunStock( false);	// Glock 18 �� ��� �������� ������ �ʹ� Ŀ�� ���Ӹ����� �Ⱥ��̰� �Ѵ�.
	}
	else
	{
		SetVisibleGunStock( true);
	}
}

/*virtual*/ void WeaponWithGunStock::OnSetVisibleExtension( bool bVal )
{
	//SetVisibleGunStock( bVal);
}

/*virtual*/ void WeaponWithGunStock::OnDrop( void)
{
	WeaponBase::OnDrop();

	// �Ⱥ��̴� ���Ӹ����� ���忡 �������� ���̰� �Ѵ�.
	SetVisibleGunStock( true);
}

void WeaponWithGunStock::SetVisibleGunStock( bool bVal)
{
	for( UINT32 a=0; a< MAX_WEAPON_LOD; a++ )
	{
		for( INT32 b=0; b< MAX_GUN_STOCK_MODEL; b++) 
		{	// �� �Ϻ� ���� ������ �Ⱥ����� ����Ī
			if( m_pGunStockModel[a][b])
			{
				if( bVal)
					m_pGunStockModel[a][b]->RemoveFlag( I3_NODEFLAG_INVISIBLE);
				else
					m_pGunStockModel[a][b]->AddFlag( I3_NODEFLAG_INVISIBLE);
			}
		}
	}
}

void WeaponWithGunStock::PlayAnim_UI_IdleA( void)
{
	CWeaponInfo * pWeaponInfo = getWeaponInfo();

	I3ASSERT( m_pWeaponInfo != nullptr);

	switch( pWeaponInfo->GetTypeClass())
	{
	case WEAPON_CLASS_HANDGUN:	
		{
			// Glock 18 �� ��� �κ񿡼� ���Ӹ����� ���� ���·� 
			if( m_aiiUI_Idle_SWAT_Male[ IDLE_A] > -1)
			{
				WF::PlayWeaponAnim(this, m_aiiUI_Idle_SWAT_Male[ IDLE_A]);
			}
		}
		break;
	}
}

void WeaponWithGunStock::PlayAnim_UI_IdleB( void)
{
	CWeaponInfo * pWeaponInfo = getWeaponInfo();

	I3ASSERT( m_pWeaponInfo != nullptr);

	switch( pWeaponInfo->GetTypeClass())
	{
	case WEAPON_CLASS_HANDGUN:	
		{
			//if( m_pOwner != nullptr)
			//{
			//	// Glock 18 �� ��� ��쿡 ĳ���Ϳ� ���� ���Ӹ����� ������ ����ϴ� ���
			//	switch( m_pOwner->getCharaInfo()->GetResType())
			//	{
			//	case CHARA_RES_ID_ACID_POLE:		WF::PlayWeaponAnim(this, m_aiiUI_Idle_SWAT_Male[ IDLE_B]);		break;
			//	case CHARA_RES_ID_SWAT_FEMALE:	WF::PlayWeaponAnim(this, m_aiiUI_Idle_SWAT_Female[ IDLE_B]);	break;
			//	}
			//}

			if( m_aiiUI_Idle_SWAT_Male[ IDLE_A] > -1)
			{
				WF::PlayWeaponAnim(this, m_aiiUI_Idle_SWAT_Male[ IDLE_A]);
			}
		}
		break;
	}
}


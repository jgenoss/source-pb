#include "pch.h"
#include "MainWeapon_DualUzi.h"
#include "i3Base/string/ext/utf16_to_mb.h"

MainWeapon_DualUzi::MainWeapon_DualUzi(WeaponBase* p) : MainWeapon_DualSMG(p)
{
		
	
}

// MainWeapon_Dual::OnCreate()�� ����ȣ������ �ʴ´�... (������ �׷��� �����Ǿ��� �����̸� ���߿� ��Ե� ��ĥ��..)
void	MainWeapon_DualUzi::OnCreate( bool bCreateInWeapon)
{
	MainWeapon_Dual::OnCreate( bCreateInWeapon);

	// �����Ⱑ ���� ����� �����⸦ �� ���̵��� �Ѵ�.
	{
		const INT32 cnt = 2;
		char * pszSilencerName[cnt] = {
			"Silencer_Material",
			"Model_Silencer_Material",
		};

		i3Body * pBody = m_pWeaponBase->getSceneObject()->GetBody();

		m_SilencerMaterialList.clear();

		for(INT32 repeat=0; repeat < cnt; repeat++)
		{
			for( UINT32 i = 0; i < pBody->getLODCount(); i++ )
			{
				i3LOD * pLOD = pBody->getLOD( i );
				i3Scene::FindNodeByName( pLOD->getShapeNode(), pszSilencerName[ repeat] , m_SilencerMaterialList );

				for( size_t j = 0; j < m_SilencerMaterialList.size(); j++)
				{
					i3Node * pNode = m_SilencerMaterialList[j];
					pNode->AddFlag( I3_NODEFLAG_DISABLE);		//	�⺻�� �Ⱥ��̰�
				}
			}

			if( m_SilencerMaterialList.size() > 0)
				break;
		}
	}

}

void	MainWeapon_DualUzi::ReturnInstance() // MainWeapon_Dual::ReturnInstance()�� ����ȣ������ �ʴ´�... (������ �׷��� �����Ǿ��� �����̸� ���߿� ��Ե� ��ĥ��..)
{
	if( m_pLeftHandWeapon)	
	{
		m_pLeftHandWeapon->setPairWeapon( nullptr);
		g_pWeaponManager->ReleaseCharaWeapon( m_pLeftHandWeapon);
		m_pLeftHandWeapon = nullptr;
	}
}

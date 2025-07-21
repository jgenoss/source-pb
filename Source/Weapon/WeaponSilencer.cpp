#include "pch.h"
#include "WeaponSilencer.h"
#include "GlobalVariables.h"
#include "WeaponFunction.h"
#include "i3Base/string/ext/utf16_to_mb.h"

I3_CLASS_INSTANCE( WeaponSilencer);//, WeaponBase);

WeaponSilencer::WeaponSilencer()
{
	m_idxFireEffect_Silencer	= -1;


}

WeaponSilencer::~WeaponSilencer()
{
	
}

void WeaponSilencer::_OnInitVairable( void)
{
	WeaponBase::_OnInitVairable();

	m_SilencerMaterialList.clear();
}

void WeaponSilencer::PostCreate( void)
{
	m_SilencerMaterialList.clear();

	// �� LOD���� �ش� Material�� ã���ϴ�. �����Ǵ� Extension�� ���� Visible�� �Ǻ��ϱ� ���ؼ��Դϴ�.
	i3Body * pBody = getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);

	for( UINT32 i = 0; i < pBody->getLODCount(); i++ )
	{
		i3LOD * pLOD = pBody->getLOD( i );
		
		i3Scene::FindNodeByName( pLOD->getShapeNode(), "Silencer_Material",			m_SilencerMaterialList );
		i3Scene::FindNodeByName( pLOD->getShapeNode(), "Silencer_Reload_Material",	m_SilencerMaterialList);
		i3Scene::FindNodeByName( pLOD->getShapeNode(), "Model_Silencer_Material",	m_SilencerMaterialList);
	}

	if( m_SilencerMaterialList.size() > 0 )
	{
		OnSetVisibleExtension( false);
	}


	// �߻� ����Ʈ�� ���̸� ã���ϴ�.
	char conv[MAX_PATH];
	i3::rc_wstring	wstrFireEffect;
	m_pWeaponInfo->GetFireEffect(wstrFireEffect);
	i3::snprintf( conv, MAX_PATH, "%s%s", wstrFireEffect , "_Silence");		// �ڵ� ����ȯ ���..

	m_idxFireEffect_Silencer = g_pEffectManager->FindEffect( conv);
	m_idxFireEffect_Silencer = ( m_idxFireEffect_Silencer == -1) ? getFireEffectIndex() : m_idxFireEffect_Silencer;
}

void WeaponSilencer::OnSetVisibleExtension( bool bVal)
{
	WeaponBase::OnSetVisibleExtension( bVal);

	// Extension ���
	if( m_SilencerMaterialList.empty() == true )
		return;

	if( bVal)
	{
		for( size_t i = 0; i < m_SilencerMaterialList.size(); i++ )
		{
			i3AttrSet * pAttrSet = (i3AttrSet*) m_SilencerMaterialList[i];
			pAttrSet->RemoveFlag( I3_NODEFLAG_DISABLE );
		}
	}
	else
	{
		for( size_t i = 0; i < m_SilencerMaterialList.size(); i++ )
		{
			i3AttrSet * pAttrSet = (i3AttrSet*) m_SilencerMaterialList[i];
			pAttrSet->AddFlag( I3_NODEFLAG_DISABLE );
		}
	}
}

void WeaponSilencer::_OnEmitFireEffect( void)
{
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )	_FireEffect( getFXMatrix(), m_idxFireEffect_Silencer);
	else						_FireEffect( getFXMatrix(), getFireEffectIndex());
}

void WeaponSilencer::_OnPlayFireSound( void)
{
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )	_PlayFireSound( WEAPON::FIRE_SOUND_MUFFLER);		//	silencer ����
	else						_PlayFireSound( WEAPON::FIRE_SOUND_NORMAL);		//	�Ϲ� ����
}

/*virtual*/void WeaponSilencer::OnPlayExtensionAnimation( bool bVal, bool bAnimPlay)
{	
	if( bAnimPlay )
	{
		if( bVal)
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE));
		else
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE));
	}
	else
	{
		_OnPlayWeaponIdleAnim();
		OnSetVisibleExtension( bVal);
	}
}


/*virtual*/ void WeaponSilencer::OnProcessExtension( bool bVal)
{
	if( bVal)
		setDamageRate( getWeaponInfo()->GetExtSilencerDmgRate());
	else
		setDamageRate( 1.0f);
}

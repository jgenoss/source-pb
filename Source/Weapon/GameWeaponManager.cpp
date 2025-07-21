#include "pch.h"
#include "GlobalVariables.h"
#include "GameWeaponManager.h"
#include "StageBattle.h"
#include "WeaponGlobal.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaMultiWeaponContext.h"
#include "WeaponM197.h"
#include "WeaponShotGun3.h"
#include "WeaponShotGunWithButt.h"
#include "WeaponShotGunMaleFemale.h"
#include "WeaponShotGunGenderWithDual.h"
#include "WeaponLRU_Manager.h"

#include "BGLoadThread.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleServerContext.h"
#include "BattleObjContext.h"
#include "MainWeapon_Grenade.h"
#include "MainWeapon_C5.h"
#include "MainWeapon_GrenadeShell.h"
#include "MainWeapon_RLShell.h"
#include "GameMissionManager.h"
#include "Weapon/WeaponItemID.h"
#include "Weapon/WeaponInfo/MeleeWeaponInfo.h"

#include "UI/UIHudManager.h"
#include "UI/UIHudMultiWeapon.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "BattleHud.h"
#include "BattleHUD_Scope.h"

#include "GrenadeGunInfo.h"

#include "ClientStageInfoUtil.h"


#define DROPEDWEAPON_SENDTIME		0.064f				// �� �ð� ��ŭ ����� ���� ������ �����ϴ�.	// �ʴ� 10ȸ
#define DROPEDWEAPON_MINSENDTIME	1.f					// �ʴ� 1ȸ ���� �ּ� ��ũ�� ����ϴ�.
#define DROPEDWEAPON_ASSIST_TIME	0.14f
#define DROPEDWEAPON_ASSIST_INVTIME 7.1428571428571428571428571428571f

#define DROPWEAPON_LIFETIME			11.f				// ����� ������ ������ Ÿ��
#define DROPWEAPON_ALPHATIME		10.f				// ����� ���Ⱑ ������� �����ϴ� �ð�
#define DROPWEAPON_ALPHASLOPE		1.f 
#define DROPWEAPON_ALPHASLOPEINV	1.f

#define MAKE_THROW_COUNT			32					// ����ź �迭�� ����
#define MAKE_GRENADESHELL_COUNT		16					// ��ź ����
#define MAKE_ROCKETLAUNCHERSHELL_COUNT	16				// ���Ϸ���(RPG-7) ���� (4vs4)
#define MAKE_ROCKETLAUNCHERSHELL_FOR_ALL_COUNT	48		// ���Ϸ���(RPG-7) ���� (ForAll)
#define MAKE_C5_COUNT				16					// C5 ����
#define MAKE_THROWING_KNIFE_COUNT	16					// ������ ���� ���� ����
#define MAKE_ARROW_COUNT			16					// Giran arrow count


REAL32	CGameWeaponManager::s_BrokenWeaponDecreaseDamage	= 5;		//	���� ������
REAL32	CGameWeaponManager::s_BrokenWeaponVertRecoil		= 5;			//	���� �ݵ�
REAL32	CGameWeaponManager::s_BrokenWeaponHorzRecoil		= 5;			//	���� �ݵ�



I3_CLASS_INSTANCE( CGameWeaponManager);//, i3ElementBase);

static bool _SetSoundInitProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( !i3::same_of<i3AIContext*>(pNode))
		return true;

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

			if( i3::same_of<i3SoundNode*>(pInfo->GetNode()))
			{
				i3SoundNode * pSnd = (i3SoundNode *) pInfo->GetNode();
				if( pSnd == nullptr) 
				{
					continue;
				}

				i3SoundPlayInfo * pPlayInfo = pSnd->getSoundPlayInfo();
				if( pPlayInfo == nullptr)
				{
					continue;
				}

				char szTemp[MAX_PATH];

				i3Sound * pSound = pPlayInfo->getSound();
				if( pSound != nullptr && (pSound->hasName()))
				{
				//	i3String::SplitFileName( pSound->GetName(), szTemp, true);
					i3::extract_filename( pSound->GetName(), szTemp);

					i3GameResSound * pRes = (i3GameResSound *) g_pFramework->QuaryResource( szTemp);
					if( pRes)
					{
						i3SoundPlayInfo * pSndResInfo = pRes->getSound();
						if( pSndResInfo != nullptr)
						{
							pPlayInfo->RemoveStyle( I3SND_PROP_TYPE_BGM | I3SND_PROP_TYPE_VOICE);
							pPlayInfo->AddStyle( I3SND_PROP_TYPE_EFFECT);

							pPlayInfo->SetDistanceInfo( pSndResInfo->getMaxDistance(),
														pSndResInfo->getMinDistance(), 
														pSndResInfo->getVolumeType());

							pPlayInfo->SetDefaultVolume( pSndResInfo->GetDefaultVolume());
							pPlayInfo->setListenerVolume( pSndResInfo->getListenerVolume());
						}
					}
//					else
//					{
//						I3PRINTLOG(I3LOG_DEFALUT,  "Weapon Sound ���� [ %s ] �� ã�� �� ����.", szTemp );
//					}
				}
			}
		}
	}

	return true;
}

WeaponDecoySndInterval::WeaponDecoySndInterval()
{
	m_interval = nullptr;
	m_num = 0;
	m_loop = true;
}

WeaponDecoySndInterval::~WeaponDecoySndInterval()
{
	if(m_interval)
		delete [] m_interval;

}



CGameWeaponManager::CGameWeaponManager( void) : m_rTrashTimer(0.f)
{
	m_pStage = nullptr;
	m_pThrowWeaponList = nullptr; 
	m_pThrowItemList = nullptr;
	m_GrenadeShellWeaponList = nullptr;

	m_nWeaponIDCount = 0;

	{
		m_decoySndInterval = new WeaponDecoySndInterval;

		m_decoySndInterval->m_num = 10;		
		m_decoySndInterval->m_interval = new REAL32[10];

		// �ѽ�°��� (1.0=1��)
		const static REAL32 tmpData[10] = { 0.1f, 0.3f, 0.1f, 0.7f, 0.3f, 0.8f, 0.2f, 0.5f, 0.1f, 0.8f };
		memcpy(m_decoySndInterval->m_interval, tmpData, sizeof(REAL32)*10);
	}

	{
		m_EventDecoySndInterval = new WeaponDecoySndInterval;
		m_EventDecoySndInterval->m_num = 1;

		m_EventDecoySndInterval->m_interval = new REAL32;
		m_EventDecoySndInterval->m_interval[0] = 0.0f;

		m_EventDecoySndInterval->m_loop = false;
	}
	
#ifdef ENABLE_TEST_COMMAND
	m_bUnlimitGun = false;
	m_bUnlimitGrenade = false;
	m_CmdGunBullet = -1;
#endif

	m_pLoadInfoPool = new i3FixedMemoryPool;
	MEMDUMP_NEW( m_pLoadInfoPool, sizeof(i3FixedMemoryPool));

	m_pLoadInfoPool->Create( "WeaponLoadInfoPool", sizeof( sWEAPONLOADINFO), 128);
	m_pLoadInfoPool->SetAutoCompactEnable( false);
}

CGameWeaponManager::~CGameWeaponManager( void)
{
	Destroy();

	if(m_decoySndInterval)
	{
		delete m_decoySndInterval;
		m_decoySndInterval = nullptr;
	}

	if(m_EventDecoySndInterval)
	{
		delete m_EventDecoySndInterval;
		m_EventDecoySndInterval = nullptr;
	}

	I3_SAFE_DELETE( m_pLoadInfoPool);

	i3::cu::for_each_SafeRelease(m_CharaWeaponList);
	i3::cu::for_each_SafeRelease(m_DropWeaponList);
}

// static
void	CGameWeaponManager::InitValue()
{
	s_BrokenWeaponDecreaseDamage	= g_pConfigEx->GetGame().BrokenWeaponDecreaseDamage;
	s_BrokenWeaponVertRecoil		= g_pConfigEx->GetGame().BrokenWeaponVertRecoil;
	s_BrokenWeaponHorzRecoil		= g_pConfigEx->GetGame().BrokenWeaponHorzRecoil;
}

void CGameWeaponManager::Create( i3Stage * pStage)
{
	m_pStage = pStage;
	
	// Common Res�� �ε�
	for( INT32 classType = 0; classType < WEAPON_CLASS_COUNT; classType++)
	{
		UINT32 cnt = WEAPON::getItemCount( (WEAPON_CLASS_TYPE) classType);
		m_WeaponResList[ classType].resize( cnt);

		for( UINT32 num = 1; num < cnt; num++)
		{	
			_LoadRes( (WEAPON_CLASS_TYPE) classType, num);
		}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		m_WeaponResCheckList[ classType].resize( cnt);
#endif
	}

	WeaponLRU_Manager::i()->PreLoad();
}

void CGameWeaponManager::_CreateThrow( i3Stage * pStage, WEAPON_CLASS_TYPE type, UINT32 maxCount)
{
	VEC3D vPos;
	WeaponBase * pWeapon;
	i3ClassMeta * pMeta = nullptr;
	i3InstanceManager * pManager = pStage->GetInstanceManager( WeaponBase::static_meta() );	
	i3ClassMeta * pOldMeta = pManager->getInstanceMeta();

	switch( type)
	{
	case WEAPON_CLASS_THROWING_GRENADE :
		if( m_pThrowWeaponList == nullptr)
		{
			m_pThrowWeaponList = new i3::vector<WeaponGrenade*>[ maxCount];
			MEMDUMP_NEW( m_pThrowWeaponList, sizeof(i3::vector<WeaponGrenade*>) * maxCount);
		}
		break;
	case WEAPON_CLASS_THROWING_CONTAIN :
	case WEAPON_CLASS_THROWING_ATTACK:
	case WEAPON_CLASS_THROWING_HEAL:
		{
			// ������ , ������, ȸ���� �������� ���� �� ����Ʈ�� ����ϴ�.
			if(m_pThrowItemList == nullptr)
			{	
				UINT32 i32ListCount = GetItemCountThrowItem();
				m_pThrowItemList = new i3::vector<WeaponGrenade*>[i32ListCount];	
				MEMDUMP_NEW( m_pThrowItemList, sizeof(i3::vector<WeaponGrenade*>) * i32ListCount);
			}	
		}
		break;
	case WEAPON_CLASS_GRENADESHELL :
		if( m_GrenadeShellWeaponList == nullptr)
		{
			m_GrenadeShellWeaponList = new i3::vector<WeaponGrenadeShell*>[ maxCount];
			MEMDUMP_NEW( m_GrenadeShellWeaponList, sizeof(i3::vector<WeaponGrenadeShell*>) * maxCount);
		}
		break;
	}

	i3::rc_wstring	wstrClassMeta;
	i3::string		strClassMeta;

	for( UINT32 weaponNumber = 1; weaponNumber  < maxCount ; weaponNumber++)
	{
		pMeta = nullptr;
		CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo( type, weaponNumber);
		
		if ( pInfo == nullptr)
			continue;
		
		if( type == WEAPON_CLASS_THROWING_GRENADE && weaponNumber == WEAPON::getItemIndex( WEAPON::DOMI_MEATBOMB))
			continue;	// ��Ʈ���� Domination������ ����Ѵ�.

		// ��ϵ��� ���� ������ ���, �������� ����.
		if( IsRegisteredWeapon(type, weaponNumber) == false )
			continue;

		// ���� ���ҽ�(I3CHR)�� ���� ����� �������� �ʴ´�.
		PreloadWeaponResource( type, weaponNumber);

		wstrClassMeta.clear();
		pInfo->GetClassMetaName(wstrClassMeta);

		if(i3::generic_string_size(wstrClassMeta) > 0)
		{
			i3::utf16_to_mb(wstrClassMeta, strClassMeta);
			pMeta =	i3ClassMetaSystem::i()->find_class_meta(strClassMeta);			//	i3ClassMeta::FindClassMetaByName(szClassMeta);
		}
		
		if(!pMeta && type == WEAPON_CLASS_GRENADESHELL)
		{	// GrenadeShell�迭�� ClassMeta�� ������ WeaponGrenadeShell�� ����Ѵ�.
			pMeta = WeaponGrenadeShell::static_meta();
		}

		if(!pMeta)
		{	
			i3::utf16_to_mb(wstrClassMeta, strClassMeta);
			I3PRINTLOG(I3LOG_FATAL,  "Invalid Throw weapon type. %d / %s", weaponNumber, strClassMeta );
			continue;
		}

		pManager->setInstanceMeta( pMeta);

		for(INT32 j = 0; j < MAKE_THROW_COUNT; j++)
		{
			pWeapon = (WeaponBase *) pManager->GetFreeInstance();
			I3ASSERT( pWeapon->GetRefCount() >= 1);

			ITEM_TYPE itemType = ITEM_TYPE_THROWING1;

			switch( type)
			{
			case WEAPON_CLASS_THROWING_GRENADE	:	itemType = ITEM_TYPE_THROWING1;		break;
			case WEAPON_CLASS_THROWING_CONTAIN	:	itemType = ITEM_TYPE_THROWING2;		break;
			case WEAPON_CLASS_THROWING_ATTACK	:	itemType = ITEM_TYPE_THROWING2;		break;
			case WEAPON_CLASS_THROWING_HEAL		:	itemType = ITEM_TYPE_THROWING2;		break;
			case WEAPON_CLASS_GRENADESHELL		:	itemType = ITEM_TYPE_TEMP;	break;
			}

			T_ItemID Itemid = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, itemType,	type, weaponNumber);

			pWeapon->Create( nullptr, Itemid, false);
			pWeapon->Reset();
			pWeapon->setIdx( m_nWeaponIDCount);
			m_nWeaponIDCount++;
			pWeapon->SetEnable(false);

			switch( type)
			{
			case WEAPON_CLASS_THROWING_GRENADE :
				{
					I3ASSERT_RETURN( weaponNumber < WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE));
					m_pThrowWeaponList[weaponNumber].push_back( static_cast<WeaponGrenade*>(pWeapon) );
				}
				break;
			case WEAPON_CLASS_THROWING_CONTAIN :	
				{
					m_pThrowItemList[weaponNumber].push_back( static_cast<WeaponGrenade*>(pWeapon) );
				}
				break;
			case WEAPON_CLASS_THROWING_ATTACK :	
				{
					INT32 idx = WEAPON::getItemCount(WEAPON_CLASS_THROWING_CONTAIN) + weaponNumber - 1;
					if( idx > -1)
						m_pThrowItemList[idx].push_back( static_cast<WeaponGrenade*>(pWeapon) );
				}
				break;
			case WEAPON_CLASS_THROWING_HEAL :	
				{
					INT32 idx = WEAPON::getItemCount(WEAPON_CLASS_THROWING_CONTAIN)
						+ WEAPON::getItemCount(WEAPON_CLASS_THROWING_ATTACK) + weaponNumber - 2;
					if( idx > -1)
						m_pThrowItemList[idx].push_back( static_cast<WeaponGrenade*>(pWeapon) );
				}
				break;
			case WEAPON_CLASS_GRENADESHELL : 
				{
					m_GrenadeShellWeaponList[weaponNumber].push_back( static_cast<WeaponGrenadeShell*>(pWeapon));	
				}
				break;
			}
			

			// ���� �� ���߿� ����ݴϴ�.
			i3Vector::Set( &vPos, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getIdx() / 10) );
			MoveShapePosition( pWeapon, &vPos);
		}
	}

	pManager->setInstanceMeta( pOldMeta);
}

void CGameWeaponManager::CreateThrowWeapon(i3Stage * pStage)
{
	_CreateThrow( pStage, WEAPON_CLASS_THROWING_GRENADE,	WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE));
	_CreateThrow( pStage, WEAPON_CLASS_THROWING_CONTAIN,	WEAPON::getItemCount(WEAPON_CLASS_THROWING_CONTAIN));
	_CreateThrow( pStage, WEAPON_CLASS_THROWING_ATTACK,		WEAPON::getItemCount(WEAPON_CLASS_THROWING_ATTACK));
	_CreateThrow( pStage, WEAPON_CLASS_THROWING_HEAL,		WEAPON::getItemCount(WEAPON_CLASS_THROWING_HEAL));
}

void CGameWeaponManager::DestroyThrowWeapon(i3Stage * pStage)
{
	if (m_pThrowWeaponList)
	{
		for( UINT32 i = 1; i < WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE); i++)
		{
			for( UINT32 j = 0; j < m_pThrowWeaponList[i].size(); j++ )
			{
				WeaponBase * pWeapon = m_pThrowWeaponList[i][j];				
				pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
				pWeapon->ReturnInstance();
			}	
		}
		I3_SAFE_DELETE_ARRAY( m_pThrowWeaponList);
	}

	if( m_pThrowItemList != nullptr)
	{
		UINT32 count = GetItemCountThrowItem();
		for( UINT32 i = 1; i < count; i++)
		{
			for( UINT32 j = 0; j < m_pThrowItemList[ i].size(); j++)
			{
				WeaponBase * pWeapon = m_pThrowItemList[i][j];
				pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
				pWeapon->ReturnInstance();
			}
		}
		I3_SAFE_DELETE_ARRAY( m_pThrowItemList);
	}
}

namespace {
	bool	s_EnableThrowWeapon( WeaponBase * pWeapon, i3Stage * pStage ) {
		if( pWeapon != nullptr && pWeapon->isEnable() == false)
		{
			pWeapon->SetEnable(true);
			if( pStage->getWorld() != nullptr )
				pStage->getWorld()->getWorldRoot()->AddChild( pWeapon->getSceneObject());
			return true;
		}
		return false;
	}
};

WeaponBase * CGameWeaponManager::GetFreeThrowWeapon( WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number)
{
	i3::vector<WeaponGrenade*>* pList = nullptr;

	switch(classType)
	{
	case WEAPON_CLASS_THROWING_GRENADE:
		{
			pList = m_pThrowWeaponList;
			if( number == 0 || number > WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE) )
			{
				I3TRACE( "invalid ThrowWeapon number.\n" );
				return nullptr;
			}
		}
		break;
	case WEAPON_CLASS_THROWING_CONTAIN:
	case WEAPON_CLASS_THROWING_ATTACK:
	case WEAPON_CLASS_THROWING_HEAL:
		{
			pList = m_pThrowItemList;

			if(number > 0)
			{
				if(classType == WEAPON_CLASS_THROWING_ATTACK)		
					number = WEAPON::getItemCount(WEAPON_CLASS_THROWING_CONTAIN) + number - 1;
				else if(classType == WEAPON_CLASS_THROWING_HEAL)
				{
					number = WEAPON::getItemCount(WEAPON_CLASS_THROWING_CONTAIN)
						+ WEAPON::getItemCount(WEAPON_CLASS_THROWING_ATTACK) + number - 2;
				}
			}

			if( number == 0 || number > GetItemCountThrowItem() )
			{
				I3TRACE( "invalid ThrowItem number.\n" );
				return nullptr;
			}
		}
		break;

	case WEAPON_CLASS_KNIFE:
		{
			i3::vector<WeaponThrowKnife*> * pKnifeList = _GetThrowKnifeList( number);

			for( UINT32 i = 0; i < pKnifeList->size(); i++)
			{
				WeaponThrowKnife * pWeapon = pKnifeList->at(i);
				if( s_EnableThrowWeapon( pWeapon, m_pStage))
					return pWeapon;
			}

			return nullptr;
		}

	case WEAPON_CLASS_GRENADESHELL :
		{
			if( m_GrenadeShellWeaponList == nullptr)
				return nullptr;

			if( number <= 0 || number >= WEAPON::getItemCount( WEAPON_CLASS_GRENADESHELL) )
			{
				I3TRACE( "invalid GrenadeShell number.\n" );
				return nullptr;
			}

			UINT32 cnt = m_GrenadeShellWeaponList[number].size();
			for( UINT32 i = 0; i < cnt; i++)
			{
				WeaponGrenadeShell * pWeapon = m_GrenadeShellWeaponList[number][i];
				if( s_EnableThrowWeapon( pWeapon, m_pStage))
					return pWeapon;
			}

			return nullptr; 
		}

	case WEAPON_CLASS_ROCKET_LAUNCHER :
		{
			WeaponRocketLauncherShell * pWeapon = nullptr;	

			UINT32 cnt = m_RocketLauncherShellWeaponList.size();
			for( size_t i = 0; i < cnt; i++)
			{
				pWeapon = m_RocketLauncherShellWeaponList[i];
				if( s_EnableThrowWeapon( pWeapon, m_pStage) )
					return pWeapon;
			}

			return nullptr;
		}
	}

	if( pList == nullptr)
		return nullptr;

	WeaponGrenade * pWeapon = nullptr;
	UINT32 cnt = pList[number].size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		pWeapon = pList[number][i];
		if( s_EnableThrowWeapon( pWeapon, m_pStage))
			return pWeapon;
	}

	return nullptr; 
}

void CGameWeaponManager::PushThrowWeapon(WeaponBase	* pWeapon)
{
	
	// ���߿� ����ݴϴ�.
	VEC3D vPos;
	i3Vector::Set( &vPos, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getIdx() / 10));
	MoveShapePosition( pWeapon, &vPos);

	if( i3::kind_of<WeaponGrenade*>(pWeapon) || 
		i3::same_of<WeaponC5*, WeaponThrowKnife*, WeaponBow*, WeaponGrenadeShell*, WeaponRocketLauncherShell*, WeaponHandGrenade*>(pWeapon) ) 
	{
		MainWeapon_Throw* mainWeapon = static_cast<MainWeapon_Throw*>(pWeapon->GetMainWeapon());
		mainWeapon->removeBombState( 0xFFFFFFFF);
	}
		
	// Network buffer reset
	_ResetNetworkInfo( pWeapon);

	pWeapon->SetEnable(false);
	pWeapon->SetOwner( nullptr);
	pWeapon->ResetColideeGroup();
	
	if( m_pStage->getWorld() != nullptr )
		m_pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());

	return;
}

WeaponGrenade * CGameWeaponManager::FindThrowWeaponByNetIndex( WEAPON::RES_ID type, INT32 netIdx)
{
	INT32 i;
	INT32 count = getThrowWeaponCount( type);

	for( i = 0; i < count; i++)
	{
		WeaponGrenade * pWeapon = getThrowWeapon( type, i);
		if( pWeapon != nullptr)
		{
			if( pWeapon->getNetworkIdx() == netIdx)
				return pWeapon;
		}
	}

	return nullptr;
}

void CGameWeaponManager::_LoadRes( WEAPON_CLASS_TYPE classType, INT32 number)
{
	char szFileName[ MAX_PATH];

	if( number == 0 || classType == WEAPON_CLASS_UNKNOWN)	return;	//	��ȿ�� ���Ⱑ �ƴϴ�.
	
	i3GameResChara * pRes = nullptr;
	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number);

	if( pWeaponInfo != nullptr)
	{
		if( __GetWeaponFileName( szFileName, pWeaponInfo) == true)
		{
			pRes = (i3GameResChara *) g_pFramework->FindResourceByOnlyName( szFileName);
			if( pRes != nullptr)
				__RegisterWeaponResource( classType, number, pRes);
			else
			{	// Common Res�� �ƴϸ� nullptr�� ����
				__RegisterWeaponResource( classType, number, nullptr);
			}

			return;
		}
	}
	
	//I3TRACE( "���⸦ ã�� �� �����ϴ�. %d, %d \n", classType, number);
	__RegisterWeaponResource( classType, number, nullptr);

	return;
}

void CGameWeaponManager::CreateGrenadeShellWeapon(i3Stage * pStage)
{
	_CreateThrow( pStage, WEAPON_CLASS_GRENADESHELL, WEAPON::getItemCount( WEAPON_CLASS_GRENADESHELL));
}



void CGameWeaponManager::DestroyGrenadeShellWeapon(i3Stage * pStage)
{
	if(m_GrenadeShellWeaponList != nullptr)
	{
		UINT32 count = WEAPON::getItemCount( WEAPON_CLASS_GRENADESHELL);
		for( UINT32 i = 1; i <count ; i++)
		{
			for( UINT32 j = 0; j < m_GrenadeShellWeaponList[i].size(); j++ )
			{
				WeaponBase * pWeapon = m_GrenadeShellWeaponList[i][j];				
				pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
				pWeapon->ReturnInstance();
			}
		}
		I3_SAFE_DELETE_ARRAY( m_GrenadeShellWeaponList);
	}
	return; 
}


void CGameWeaponManager::PushGrenadeShellWeapon( WeaponGrenadeShell * pWeapon)
{
	// ���߿� ����ݴϴ�.
#if !defined( NO_PHYSIX_HEIGHT )
	MATRIX mat;
	
	i3Matrix::SetPos( &mat, (REAL32)(pWeapon->getWeaponInfo()->GetNumber()) * 10.f, g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getNetworkIdx()) * 10.f, 1.f );
	i3Body * pBody = pWeapon->getSceneObject()->GetBody();
	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	for( INT32 k = 0; k < pSkel->getBoneCount(); k++ )
	{
		if( pSkel->isShapeSet( k ))
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( k );
			if( pShapeSet != nullptr )
			{
				pShapeSet->setShapeGlobalPose( &mat );
			}
		}
	}
#endif

	MainWeapon_GrenadeShell* mainWeapon_GS = static_cast<MainWeapon_GrenadeShell*>(pWeapon->GetMainWeapon());
	mainWeapon_GS->removeBombState( 0xFFFFFFFF);

	// Network buffer reset
	_ResetNetworkInfo( pWeapon);

	pWeapon->SetOwner( nullptr);		// �����ϱ� ���� ������ ���� ĳ���͸� �ʱ�ȭ����� �Ѵ�.
	pWeapon->Reset();
	pWeapon->SetEnable(false);
	
	if( m_pStage->getWorld() != nullptr )
		m_pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
}

// Rocket Launcher Shell
void CGameWeaponManager::CreateRocketLauncherShellWeapon( i3Stage * pStage)
{
	WeaponRocketLauncherShell	* pWeapon	= nullptr;
	i3ClassMeta * pMeta		= nullptr;
	i3InstanceManager * pManager = pStage->GetInstanceManager( WeaponBase::static_meta() );	

	VEC3D vPos;

	i3ClassMeta * pOldMeta = pManager->getInstanceMeta();
	{	
		pMeta = WeaponRocketLauncherShell::static_meta();
		pManager->setInstanceMeta( pMeta);

		INT32 maxCnt = MAKE_ROCKETLAUNCHERSHELL_COUNT;		//	default count

		// Enable_RPG7ForAll�� ��� ��� �ʿ��� 8vs8 �������� �ѻ���� 3�߾� �����Ƿ� ������ �ִ��� �ø���.
		if( g_pConfigEx->GetGame().UnlimitedForRPG7)
			maxCnt = MAKE_ROCKETLAUNCHERSHELL_FOR_ALL_COUNT;	//	Entended count

		T_ItemID Itemid;
		if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
		{// ������ ��� RPG7Shell (���� ����)
			Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING1,	WEAPON_CLASS_GRENADESHELL, GetItemIndex("GRENADESHELL_RPG7_FOR_DINO"));
		}
		else
		{
			UINT8 WeaponFlag = MyRoomInfoContext::i()->GetMyRoomWeaponFlag();
			if (TESTBIT( WeaponFlag, LOCK_RPG7))
			{// revision 53430 RPG�� ����
				Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING1,	WEAPON_CLASS_GRENADESHELL, GetItemIndex("GRENADESHELL_RPG7_FOR_MODE") );
			}
			else
			{	// ��ġ�� RPG7 ��� RPG7Shell
				Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING1,	WEAPON_CLASS_GRENADESHELL, GetItemIndex("GRENADESHELL_RPG7"));
			}
		}

		for(INT32 i = 0; i < maxCnt; i++)
		{
			pWeapon = (WeaponRocketLauncherShell *) pManager->GetFreeInstance();
			I3ASSERT( pWeapon->GetRefCount() >= 1);

			pWeapon->Create( nullptr, Itemid, false);
			pWeapon->Reset();
			pWeapon->setIdx( m_nWeaponIDCount );
			m_nWeaponIDCount++;
			pWeapon->SetEnable(false);
			m_RocketLauncherShellWeaponList.push_back( pWeapon);

			// ���� �� ���߿� ����ݴϴ�.
			i3Vector::Set( &vPos, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getIdx() / 10) );
			MoveShapePosition( pWeapon, &vPos);
		}
	}
	pManager->setInstanceMeta( pOldMeta);
}

void CGameWeaponManager::DestroyRocketLauncherWeapon( i3Stage * pStage)
{
	for( size_t i = 0; i < m_RocketLauncherShellWeaponList.size(); i++)
	{
		WeaponRocketLauncherShell * pWeapon = m_RocketLauncherShellWeaponList[i];
		pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
		pWeapon->ReturnInstance();
	}

	m_RocketLauncherShellWeaponList.clear();
}

void CGameWeaponManager::PushRocketLauncherWeapon( WeaponRocketLauncherShell * pWeapon)
{
	// ���߿� ����ݴϴ�.
#if !defined( NO_PHYSIX_HEIGHT )
	MATRIX mat;
	
	i3Matrix::SetPos( &mat, (REAL32)(pWeapon->getWeaponInfo()->GetNumber()) * 10.f, g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getNetworkIdx()) * 10.f, 1.f );
	i3Body * pBody = pWeapon->getSceneObject()->GetBody();
	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	for( INT32 k = 0; k < pSkel->getBoneCount(); k++ )
	{
		if( pSkel->isShapeSet( k))
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( k );
			if( pShapeSet != nullptr )
			{
				pShapeSet->setShapeGlobalPose( &mat );
			}
		}
	}
#endif

	MainWeapon_RLShell* mainWeapon = static_cast<MainWeapon_RLShell*>(pWeapon->GetMainWeapon() );

	static_cast<MainWeapon_RLShell*>(mainWeapon)->removeBombState( 0xFFFFFFFF);

	// Network buffer reset
	_ResetNetworkInfo( pWeapon);

	pWeapon->SetOwner( nullptr);
	pWeapon->Reset();
	pWeapon->SetEnable(false);
	
	if( m_pStage->getWorld() != nullptr )
		m_pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
}

void CGameWeaponManager::Destroy( void )
{
}

void CGameWeaponManager::PreStartRound( void)
{
		
}

void CGameWeaponManager::EnterStage( i3Stage * pStage )		
{
	m_pStage = pStage;
}

void CGameWeaponManager::LeaveStage( bool bDestroy )
{
	// ���� �߿��մϴ�.
	if( i3::same_of<CStageBattle*>( m_pStage))
	{
		DestroyThrowWeapon( m_pStage);
		DestroyGrenadeShellWeapon( m_pStage);
		DestroyRocketLauncherWeapon( m_pStage);
		DestroyThrowKnife( m_pStage);

		RemoveAllDropWeapon();

		// WeaponLRU�� ����Ͽ� ���� ����� common res�� ��ȯ
		WeaponLRU_Manager::i()->BackingDataFromAddList();
		WeaponLRU_Manager::i()->SortWeaponByPriority();
		WeaponLRU_Manager::i()->AfterLoad();	// 30�� �̻�
	}

	_RemoveAllCharaWeapon();

	// CommonRes�� �ƴ� ����� ResourceManager���� �޸𸮸� ������Ų��.
	// List�� ����������
	RemoveWeaponRes( false);

	m_pStage		= nullptr;
	m_nWeaponIDCount = 0;	// ������������ ���ö� �ʱ�ȭ
}

void CGameWeaponManager::RemoveWeaponRes( bool bRemoveCommonRes)
{	
	__RemoveWeaponBGLoadInfo();

	i3ResourceManager * pMng = g_pFramework->GetResourceManager();

	i3::vector<size_t> avecClassID[WEAPON_CLASS_COUNT];
	CGameWeaponManager::_CollectBackgroundClassIDList(avecClassID);

	for(INT32 i = 1; i < WEAPON_CLASS_COUNT; i++)
	{
		const i3::vector<size_t>& vecClassID = avecClassID[i];

		for(size_t j = 0; j < m_WeaponResList[ i].size(); j++)
		{
			i3GameResChara * pRes = m_WeaponResList[ i][j];
			if( pRes != nullptr )
			{
				// CommonRes�� ����� ���
				if( (bRemoveCommonRes == false) && (pRes->isCommonRes() == true) )
				{
					continue;
				}

				if( pRes->GetRefCount() <= 2)
				{
					i3::vector<size_t>::const_iterator it = i3::find(vecClassID.begin(), vecClassID.end(), j);

					if (it == vecClassID.end() )
					{
						pMng->RemoveResource( pRes);
					}
					else
					{
						I3TRACE("can't delete resource because of background loading : %s !!!! \n", pRes->GetName());
					}
				}

				m_WeaponResList[i][j] = nullptr;
			}
		}
	}
}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
bool CGameWeaponManager::IsExistResource( WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number)
{
	if( m_WeaponResCheckList[ classType][ number] == false)
	{
		CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number);

		if( pWeaponInfo == nullptr)
			return false;

		char szName[ MAX_PATH];
		if( __GetWeaponFileName( szName, pWeaponInfo) == true)
		{
			const char * pszFullPath = __GetWeaponResourcePath( pWeaponInfo);
			if( pszFullPath == nullptr)
				return nullptr;
			m_WeaponResCheckList[ classType][ number] = i3FileStream::isFile( pszFullPath);
		}
	}

	return m_WeaponResCheckList[ classType][ number];
}
#endif

i3GameResChara * CGameWeaponManager::QueryResource( WEAPON_CLASS_TYPE classType, INT32 number, bool bDirectLoad)
{
	char szName[ MAX_PATH];

	if (classType >= WEAPON_CLASS_COUNT)
	{
		I3PRINTLOG(I3LOG_NOTICE, "[CGameWeaponManager::QueryResource] Not Find Weapon - classType : %d, number : %d\n", classType, number);
		return NULL;
	}

	if( classType == WEAPON_CLASS_UNKNOWN || (INT32)m_WeaponResList[ classType].size() < number)	return NULL;

	i3GameResChara * pRes = (i3GameResChara *) m_WeaponResList[ classType][ number];

	if( (pRes == nullptr || pRes->getSceneGraphInfo() == nullptr) && bDirectLoad)
	{
		// �޸𸮿� ����?
		// �ٷ� �ε��� �ؾ� �Ѵ�.
		CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number);

		if( pWeaponInfo == nullptr)
			return nullptr;

		if( __GetWeaponFileName( szName, pWeaponInfo) == true)
		{
			const char * pszFullPath = __GetWeaponResourcePath( pWeaponInfo);
			if( pszFullPath == nullptr)
				return nullptr;
			if( i3FileStream::isFile( pszFullPath))
			{
				pRes = (i3GameResChara *) g_pFramework->QuaryResource( szName);
				I3ASSERT( pRes != nullptr);

				if( pRes->getSceneGraphInfo() == nullptr)
				{
					i3::rc_wstring wstrName;
					//���Ⱑ ����� �ȵ� ��� ���� �̸� ǥ�÷� ����
					pWeaponInfo->GetWeaponName(wstrName);

					i3::string strName;
					i3::utf16_to_mb(wstrName, strName);
					I3PRINTLOG(I3LOG_FATAL, "i3GameResChara �� nullptr�� - %s\n  \
								RSC ���Ͽ� ���� I3CHR ������ ����ߴ��� Ȯ���ϼ���.", strName.c_str() );
				}

				__RegisterWeaponResource( classType, number, pRes);
			}
		}
	}

	return pRes;
}

i3GameResChara * CGameWeaponManager::QueryResource( T_ItemID ItemID, bool bDirectLoad)
{
	WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( ItemID );
	WEAPON::RES_ID number = WEAPON::ItemID2ResID( ItemID );

	return QueryResource( classType, number, bDirectLoad);
}

WeaponBase * CGameWeaponManager::QueryWeapon( CGameCharaBase * pOwner, T_ItemID Itemid, T_ItemID subItem, bool MustCreate, bool bCreateInWeapon)
{
	// ����� �� �ִ� ���� ���Ⱑ �ִ��� Ȯ��
	if ( MustCreate == false )
	{
		WeaponBase * pWeapon = _FindFreeWeapon(Itemid);
		if ( pWeapon )
		{
			if( pWeapon->getOwner() == nullptr)
				pWeapon->setOwner(pOwner);
			return pWeapon;
		}
	}

	// ���� ���Ⱑ ������ ������.
	return _CreateNewWeapon(Itemid, subItem, pOwner, bCreateInWeapon);
}

INT32 CGameWeaponManager::_SearchCharaWeapon( WeaponBase * pWeapon)
{	//	���н� -1 ��ȯ. ������ idx ��ȯ
	//return m_CharaWeaponList.IndexOf( (void *) pWeapon);
	return i3::vu::int_index_of(m_CharaWeaponList, pWeapon);
}

INT32 CGameWeaponManager::_SearchDropWeapon( WeaponBase * pWeapon)
{	//	���н� -1 ��ȯ. ������ idx ��ȯ
	//return m_DropWeaponList.IndexOf( (void *) pWeapon);
	return i3::vu::int_index_of(m_DropWeaponList, pWeapon);
}

void CGameWeaponManager::_RemoveAllCharaWeapon( void)
{
	for( INT32 i = 0; i < (INT32)m_CharaWeaponList.size(); i++)
	{
		WeaponBase * pWeapon = m_CharaWeaponList[i];
		I3ASSERT( pWeapon != nullptr);

		if( ReleaseCharaWeapon( pWeapon) == true)
			i = -1;	//��� ���Ⱑ ���ÿ� ���� �� �� �����Ƿ� ó������ �ٽ� ó���ϵ��� �մϴ�.
	}

	I3ASSERT( m_CharaWeaponList.size() == 0);
}

void CGameWeaponManager::RemoveAllDropWeapon( void)
{
	for( size_t i = 0; i < m_DropWeaponList.size(); i++)
	{
		WeaponBase * pWeapon = m_DropWeaponList[i];
		I3ASSERT( pWeapon != nullptr);

		if( _ReleaseDropWeapon( pWeapon) == true)
			i--;
	}

	I3ASSERT( m_DropWeaponList.size() == 0);
}

bool CGameWeaponManager::ReleaseCharaWeapon( WeaponBase * pWeapon)
{
	if( pWeapon != nullptr)
	{
		I3ASSERT( m_pStage != nullptr);
		
		if( (pWeapon->getPairWeapon() == nullptr) )
		{
#if defined( I3_DEBUG)
			i3::rc_wstring wstrWeaponName;
			pWeapon->getWeaponInfo()->GetWeaponName(wstrWeaponName);
			I3TRACE( "[ReleaseCharaWeapon] %s (cnt:%d)\n", wstrWeaponName, (INT32)m_CharaWeaponList.size());
#endif

			i3::vu::remove(m_CharaWeaponList, pWeapon);
			pWeapon->ReturnInstance();		//	�ν��Ͻ� ��ȯ

			BattleObjContext::i()->ResetEventHandler( pWeapon);

			return true;
		}
	}

	return false;
}

bool CGameWeaponManager::_ReleaseDropWeapon( WeaponBase * pWeapon)
{
	if( pWeapon != nullptr)
	{
		I3ASSERT( pWeapon->getPairWeapon() == nullptr);

#if defined( I3_DEBUG)
		i3::rc_wstring wstrWeaponName;
		pWeapon->getWeaponInfo()->GetWeaponName(wstrWeaponName);
		I3TRACE( "[ReleaseDropWeapon] %s %d (cnt:%d)\n", wstrWeaponName, pWeapon->getNetworkIdx(), (INT32)m_DropWeaponList.size());
#endif

		i3::vu::remove(m_DropWeaponList, pWeapon);
		pWeapon->ReturnInstance();		//	�ν��Ͻ� ��ȯ

		BattleObjContext::i()->ResetEventHandler( pWeapon);

		return true;
	}

	return false;
}

bool CGameWeaponManager::_AddCharaWeapon( WeaponBase * pWeapon)
{
	if( _SearchCharaWeapon( pWeapon) == -1)
	{
		i3::rc_wstring wstrName;
		pWeapon->getWeaponInfo()->GetWeaponName(wstrName);
		I3TRACE("_AddCharaWeapon - %s\n", wstrName);

		m_CharaWeaponList.push_back( pWeapon);
		I3_SAFE_ADDREF(pWeapon);
		return true;
	}

	I3ASSERT_0;		//������� ���� �ʵ� 20080701 

	return false;
}

void CGameWeaponManager::_ResetNetworkInfo( WeaponBase * pWeapon)
{	
	// �������̼� ���(�Ϲ���) �� ��, ���� �����ְ� �ڱ� �ڽ� or AI ĳ������ ��쿡�� ó�� ����.
	if ( BattleServerContext::i()->IsUseDedicationMode() )
	{
		CGameCharaBase * pOwner = pWeapon->getOwner();
		if ( pOwner != nullptr && ( pOwner->isLocal() || pOwner->isAI() ) )
		{
			return;
		}
	}
	else if(BattleServerContext::i()->IsP2PHost())
	{ //P2P Host�� ��� ��Ŷ ��ü�� ����(��ǻ� ���� �����ְ� ����)�ϱ� ������ ó�� ����.
		return;
	}

	// ���� �ε����� ���� ��, ��Ʈ��ũ ���� ����
	if ((pWeapon->getNetworkIdx() > -1) &&	(pWeapon->getNetworkIdx() < MAX_THROWWEAPON_COUNT))
	{
		GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon(pWeapon->getNetworkIdx());
		pInfo->Reset();
		pWeapon->setNetworkIdx(-1);
	}
}

bool CGameWeaponManager::MoveWeaponCharaToDrop( WeaponBase * pWeapon)
{	
	//	CharaList���� DropList�� ���� �ν��Ͻ� �̵�
	//	CharaList�� ��ϵǾ� �ִ��� �ϴ� �˻�
	if( _SearchCharaWeapon( pWeapon) != -1)
	{
		m_DropWeaponList.push_back( pWeapon);			//	DropList�� �̵�

		// ���߹��� �ý��� ��� �� �ֹ��� �迭�� ������ GameCharaMultiWeaponContext���� ����
		// ���� ������ ���� ���� �����ӿ� ����
		if ( pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY
			&& MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			// ���⼭ ���߹��� ������ ����
			CGameCharaBase* pChara = pWeapon->getOwner();

			if ( pChara != nullptr)
			{
				I3TRACE("���⸦ ����� ĳ���� Index : %d\n", pChara->getCharaNetIndex());
				pChara->getMultiWeaponContext()->DropedMultiWeapon();
			}
		}

		i3::vu::remove(m_CharaWeaponList, pWeapon);		//	CharaList���� ����

		I3TRACE( "Move Weapon Chara -> Drop %d\n", pWeapon->getNetworkIdx());

		return true;
	}

	return false;
}

bool CGameWeaponManager::MoveWeaponDropToChara( WeaponBase * pWeapon, bool bOnlyList, UINT8 nSyncNumber, bool bCheckDropList, bool bPushWeaponList)
{	
	//	DropList���� CharaList�� ���� �ν��Ͻ� �̵�	
	//	DropList�� ��ϵǾ� �ִ��� �ϴ� �˻�
	if( _SearchDropWeapon( pWeapon) != -1 || bCheckDropList == FALSE)
	{
#if defined(I3_DEBUG)
		i3::rc_wstring wstrName;
		pWeapon->getWeaponInfo()->GetWeaponName(wstrName);
		I3TRACE("MoveWeaponDropToChara - %s\n", wstrName);
#endif

		if( bPushWeaponList)
			m_CharaWeaponList.push_back( pWeapon);		//	CharaList�� �̵� - �� �̷��� �ߴ���...?

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon()
			&& pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY
			&& bOnlyList == FALSE ) // bOnlyList�� ����?
		{
			CGameCharaBase * pChara = pWeapon->getOwner();

			I3ASSERT( pChara != nullptr);

			// ���⸦ ���߹��� �ý��ۿ� �ִ´� - ���⼭ ��ü���� �ϸ� �ȵ�!!! Only �߰���!!
			pChara->getMultiWeaponContext()->AddMultiWeapon( pWeapon, nSyncNumber);
		}

		i3::vu::remove(m_DropWeaponList, pWeapon);		//	DropList���� ����

		I3TRACE( "Move Weapon Drop -> Chara %d\n", pWeapon->getNetworkIdx());
		
		return true;
	}

	return false;
}

WeaponBase * CGameWeaponManager::FindDropWeapon( CGameCharaBase * pOwner, WEAPON_CLASS_TYPE type, INT32 nNumber)
{
	for( size_t i = 0; i < m_DropWeaponList.size(); i++)
	{
		WeaponBase * pCurWeapon = m_DropWeaponList[i];

		I3ASSERT( i3::kind_of<WeaponBase*>(pCurWeapon));

		CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
		I3ASSERT( pCurWeaponInfo != nullptr);
		
		if( pOwner != nullptr)
		{
			if( pCurWeapon->getOwner() != pOwner)		continue;
		}

		if( pCurWeaponInfo->isSame(type, nNumber))
			return pCurWeapon;
	}

	return nullptr;
}

WeaponBase * CGameWeaponManager::FindDropWeaponByIdx( INT32 nIdx )
{
	for( size_t i = 0; i < m_DropWeaponList.size(); i++ )
	{
		WeaponBase * pWeapon = m_DropWeaponList[i];
		if( pWeapon->getNetworkIdx() == nIdx )
			return pWeapon;
	}
	return nullptr;
}

WeaponBase * CGameWeaponManager::SetDropWeapon( INT32 idx, WEAPON_SLOT_TYPE nWeaponSlot, INT16 nDropIdx, bool bNextWeapon, bool bDirectPickup )
{
	VEC3D vLin, vDropPos;
	
	WeaponBase * pWeapon;
	WeaponBase * pWorldWeapon;

	I3ASSERT( idx < SLOT_MAX_COUNT );
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idx );
	if( pChara == nullptr )
	{
		// ���� �� �����غ��ô�.
		I3TRACE( "CGameContext::SetDropWeapon() - ĳ���Ͱ� �����ϴ�. idx:%d ���� �����縶\n", idx );
		return nullptr;
	}

	CGameCharaWeaponContext * pWeaponCtx = pChara->getWeaponContext();

	if( pWeaponCtx == nullptr )		return nullptr;

	pWeapon = pWeaponCtx->getWeapon( nWeaponSlot );
	if( pWeapon == nullptr )		return nullptr;

	if( pWeapon->getWeaponInfo()->isEnableDrop() == false)
	{
		// No drop weapon
		I3PRINTLOG(I3LOG_WARN,  "������ ���Ⱑ �ƴմϴ�. class : %d, num : %d", pWeapon->getWeaponInfo()->GetTypeClass(), pWeapon->getWeaponInfo()->GetNumber() );
		return nullptr;
	}

	bool bHost = pWeapon->IsDedicatedHost();

	// �̹� ������� �ε����� �ѱ� ��� ���¶�� �� ���� �����ݴϴ�. ���� ���� �����.
	pWorldWeapon = FindDropWeaponByIdx( nDropIdx );
	if( pWorldWeapon != nullptr )
	{
		PushDropWeaponToChara( pWorldWeapon );
	}

	// ���⸦ ĳ���Ϳ��� ����.
	pChara->Cmd_DetachWeapon( pWeapon);

	if( !bNextWeapon )
	{	// ���� ���⸦ ������� �ʴ´ٸ� �ڵ����� ������
		pChara->Cmd_DropWeapon( nWeaponSlot, i3Matrix::GetPos( pWeapon->getSceneObject()->GetCacheMatrix()), false);

		if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			pChara->getMultiWeaponContext()->ClearWeaponSlotList();
	}

	// �ø��� �׷��� Simulate������ ��ȯ
	if( bHost)
	{
		if( pWeapon->getContactHitBox() != nullptr )
			pWeapon->getContactHitBox()->modifyDynamicState( 0, I3_PHYSIX_STATE_NO_COLLISION);
	}

	pWeapon->setNetworkIdx( nDropIdx );
	pWeapon->OnSetBlendAlpha( 1.f );

	pWeapon->SetVisible(true);
	
	pChara->getCameraContext()->getVirtualCamPosition( &vDropPos);
	addY( &vDropPos, -0.2f);

	MATRIX mtx;

	i3Matrix::Copy( &mtx, pChara->GetMatrix());
	if(bNextWeapon)
		i3Matrix::SetPos( &mtx, &vDropPos);

	pWeapon->SetMatrix( &mtx );

	if( bHost || (MyRoomInfoContext::i()->getStage()->IsAiMode()) )
	{	// ������ �ش� ���⸦ Physix�� �մϴ�.
		MATRIX mat;

		VEC3D * pAt = i3Matrix::GetAt( pChara->GetMatrix() );

		i3Matrix::SetRotateAxis( &mat, i3Matrix::GetRight( pChara->GetMatrix() ), -pChara->getBoneContext()->getRhoValue() );
		i3Vector::TransformCoord( &vLin, pAt, &mat );
	
		VEC3D vAddPos;
		i3Vector::Scale( &vAddPos, &vLin, 0.05f);
		i3Vector::Add( &vDropPos, &vDropPos, &vAddPos);
		
		if( bNextWeapon )
		{	// ���⸦ ���� ������ ��쿡�� ������ �������� �մϴ�. (ĳ���Ͱ� ������쿡�� ���� ���� ����)
			i3Vector::Scale( &vLin, &vLin, 6.8f );
		}

		i3Skeleton * pSkel = pWeapon->getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
		for( INT32 i = 0; i < pSkel->getBoneCount(); i++)
		{
			if( pSkel->isShapeSet( i))
			{
				i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i);
				if(bNextWeapon)
					pShapeSet->setFBLinearVelocity( &vLin);
			}
		}

		pWeapon->setDynamicActiveState( true, false );
		pWeapon->SetDynamicState( I3GAME_DYNAMIC_STATE_DYNAMIC, true);
	}
	
	
	pWeapon->SetPos( &vDropPos);
		
	// ���� ���⸦ ��� �˴ϴ�.
	if( bNextWeapon )
	{
		pChara->Cmd_DropWeapon( nWeaponSlot, &vDropPos, true, bDirectPickup);
	}
	else
	{// changing Knife & visiable off
		pChara->Cmd_ChangeWeapon( WEAPON_SLOT_MELEE, true);

		if( pWeaponCtx->getCurrent() != nullptr )
		{
			pWeaponCtx->getCurrent()->SetEnable( false);
			pWeaponCtx->getCurrent()->DetachNode();
		}
	}

	pWeapon->OnSetVisibleExtension( false );
	pWeapon->OnDrop();

	// xgkick_thai
	// ������ �ƴ� ���, �ڽ��� ���Ⱑ �ֿ����� �ʴ� ������ �ذ��ϱ� ���� ����.
	pWeapon->setTeamTag( pChara->getCharaTeam());

	// revision 29616
	//GM ������ ��� �� ��� ���� �� GM�� ���Ⱑ �ٸ� �������� �Ⱥ��̵��� ��.( GM �ڽ��� ���Դϴ�.. )
	if( BattleSlotContext::i()->IsGMObserverForClient( pChara->getCharaNetIndex())
		&& pChara != g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx()))
	{
		PushDropWeaponToChara( pWeapon);
	} 

	I3TRACE( "[DropWeapon] chara : %d, id : %d, idx : %d\n", idx, pWeapon->getNetworkIdx(), pWeapon->getNetworkIdx() );

	return pWeapon;
}

void CGameWeaponManager::PushDropWeaponToChara( WeaponBase * pWeapon )
{
	I3TRACE( "Remove DroppedWeapon %d %d\n", pWeapon->getNetworkIdx(), pWeapon->getNetworkIdx());

	MoveWeaponDropToChara( pWeapon, TRUE, 0, TRUE, TRUE);

	pWeapon->setNetworkIdx( -1);
	pWeapon->SetOwner( nullptr );			// �������� ����� �� �ֵ��� �մϴ�
	pWeapon->SetEnable( false);			// ��忡�� ���ϴ�

	// �������� �����ϴ�.
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)(pWeapon->getIdx() / 10) );
	MoveShapePosition( pWeapon, &vec );
}

void CGameWeaponManager::MoveShapePosition( WeaponBase * pWeapon, VEC3D * pPos )
{
	if( pWeapon == nullptr )
		return;

	if( pWeapon->GetRefCount() < 2)
		return;

	i3Body * pBody = pWeapon->getSceneObject()->GetBody();

	if( pBody == nullptr)
		return;

	if( pBody->getFrameLOD() == nullptr)
		return;

	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	if( pSkel == nullptr)
		return;

	pWeapon->SetPos( pPos );

	MATRIX mat;
	i3Matrix::SetPos( &mat, pPos );
	for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
	{
		if( pSkel->isShapeSet( i) )
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
			if( pShapeSet != nullptr )
			{	
				pShapeSet->setShapeGlobalPose( &mat );
			}
		}
	}
}


void CGameWeaponManager::OnUpdate( REAL32 rDeltaTime )
{
	
	// EmptyCartridgeSoundPlay
	UINT32 cnt = m_CharaWeaponList.size();
	for( UINT32 i = 0; i < cnt; i++ )
	{
		WeaponBase* pWeapon = m_CharaWeaponList[i];
		pWeapon->ProcessSound( rDeltaTime );
	}	

	//throw Weapon 
	if( m_pThrowWeaponList != nullptr)
	{
		WeaponBase * pGrenade = nullptr;
		for( UINT32 i = 1; i  < WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE) ; i++)
		{
			cnt = m_pThrowWeaponList[i].size();
			for( UINT32 j = 0; j < cnt; j++)
			{
				pGrenade = m_pThrowWeaponList[i][j];
				if( pGrenade->isEnable() )
				{	
					pGrenade->OnUpdate(rDeltaTime); 
				}
			}
		}		
	}

	if( m_pThrowItemList != nullptr)
	{
		WeaponBase * pGrenade = nullptr;
		UINT32 count = GetItemCountThrowItem();

		for( UINT32 i = 1; i  < count ; i++)
		{
			cnt = m_pThrowItemList[i].size();
			for( UINT32 j = 0; j < cnt; j++)
			{
				pGrenade = m_pThrowItemList[i][j];
				if( pGrenade->isEnable() )
					pGrenade->OnUpdate(rDeltaTime); 
			}
		}		
	}

	if( m_GrenadeShellWeaponList != nullptr)
	{
		WeaponBase * pGrenade = nullptr;
		UINT32 count = WEAPON::getItemCount( WEAPON_CLASS_GRENADESHELL);
		for( UINT32 i = 1; i < count ; i++)
		{
			cnt = m_GrenadeShellWeaponList[i].size();
			for( UINT32 j = 0; j < cnt; j++)
			{
				pGrenade = m_GrenadeShellWeaponList[i][j];
				if( pGrenade->isEnable() )
					pGrenade->OnUpdate(rDeltaTime); 
			}
		}		
	}
	
	// Rocket Launcher shell (RPG-7)
	cnt = m_RocketLauncherShellWeaponList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		WeaponRocketLauncherShell * pWeapon = m_RocketLauncherShellWeaponList[i];
		if( pWeapon->isEnable() )
			pWeapon->OnUpdate( rDeltaTime);
	}

	// Throw knife (miniaxe)
	tThrowKnifeCon::iterator it = m_ThrowKnifeList.begin();
	while( it != m_ThrowKnifeList.end())
	{
		i3::vector<WeaponThrowKnife*> & list = it->second;
		cnt = list.size();
		for( UINT32 i = 0; i < cnt; i++)
		{
			WeaponThrowKnife * pWeapon = list[i];
			if( pWeapon->isEnable() )
				pWeapon->OnUpdate( rDeltaTime);
		}
		it++;
	}

	//DropWeapon
	_ProcessDropedWeapon( rDeltaTime);

	// Background load
	__BackgroundLoading();

#if defined( I3_DEBUG)
	// ����� AI������ ���⸦ �������� �����ϱ⿡ �޸� ������ ���� �� ����.
	// ������ ���ҽ��� �����ϵ��� �Ѵ�.
	TrashWeaponRes( rDeltaTime);
#endif
}


void CGameWeaponManager::_ProcessDropedWeapon( REAL32 rDeltaTime)
{
	bool	bSend;
	bool	bHost;

	for( size_t i = 0; i < m_DropWeaponList.size(); i++ )
	{
		WeaponBase * pWeapon = m_DropWeaponList[i];
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		if( (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)  == false)
			&& ( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION ))
			continue;

		if( pWeapon->getNetworkIdx() == -1 )
		{
			// ��Ʈ��ũ ��ũ�� �̰��� ��� �ü� �ֽ��ϴ�.
			PushDropWeaponToChara( pWeapon );
			i--;	// ����Ʈ���� �����⿡ ī��Ʈ�� �������� �ٽ� üũ�մϴ�.
			continue;
		}

		GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( pWeapon->getNetworkIdx() );
		if( pInfo->_pWeaponRef == nullptr )	//
		{	// �˼� ���� ����
			I3TRACE( "invalid droped weapon _pWeaponRef == nullptr. %d - %d\n", pWeapon->getNetworkIdx(), pWeapon->getNetworkIdx() );
			I3TRACE( "[RemoveDropWeapon] idx : %d\n", pWeapon->getNetworkIdx() );
			// Ageia. ���⸦ �������� ���� �ٽ� ����ϱ� ���� CharaList�� �ű�ϴ�.
			PushDropWeaponToChara( pWeapon );
			pInfo->Reset();
			i--;	// ����Ʈ���� �����⿡ ī��Ʈ�� �������� �ٽ� üũ�մϴ�.
			continue;
		}

		REAL32 fLifeTime = pInfo->_fDropedWeaponLifeTime += rDeltaTime;
		
		bHost = pWeapon->IsDedicatedHost();

		if( bHost )
		{
			//////////////////////////////////////////////////////////////////////////
			//								( P2PServer )							//
			if( CStageInfoUtil::IsDeletableDroppedWeapon(*MyRoomInfoContext::i()->getStage()) )
			{// ���� �ð��� ������ ���⸦ ���忡�� �����մϴ�
				if( fLifeTime > DROPWEAPON_ALPHATIME )
				{
					pWeapon->setDroped( false );
					pWeapon->OnSetBlendAlpha( ((DROPWEAPON_ALPHASLOPE) - (fLifeTime - DROPWEAPON_ALPHATIME)) * DROPWEAPON_ALPHASLOPEINV );
				}

				if( fLifeTime > DROPWEAPON_LIFETIME )
				{	
					pInfo->setState( NET_WEAPONDROPED_REMOVE);
					pInfo->_fDropedWeaponLifeTime = 0.f;
				}
			}

			// ��ź���� Ȯ���Ͽ� 0�̸� �����մϴ�.
			if( (pWeapon->getTotalBulletCount() == 0 && pWeapon->getLoadedBulletCount() == 0 && pWeapon->getLoadedBulletDualCount() == 0)
				&& pWeapon->IsDroped() )
			{	
				if( fLifeTime > 1.f )
					pWeapon->OnSetBlendAlpha( (1.f - (fLifeTime - 1.f)));
				if( fLifeTime > 2.f )
				{
					pInfo->setState( NET_WEAPONDROPED_REMOVE);
					pInfo->_fDropedWeaponLifeTime = 0.f;
				}
			}


			bSend = false;

			switch( pInfo->getState() )
			{
			case NET_WEAPONDROPED_SLEEP :
				// 1�ʿ� 2ȸ���� ��ũ�� �����ݴϴ�.(�����ϰų� ��Ŷ�� �� �޾��� ��츦 ������� �մϴ�)
				pInfo->_fSendTime += rDeltaTime;
				if( pInfo->_fSendTime >= DROPEDWEAPON_MINSENDTIME )
				{
					bSend = true;
				}
				break;

			case NET_WEAPONDROPED_START :	// ���� 1ȸ�� ���� ���� �����ݴϴ�.
				bSend = true;
				pInfo->setState( NET_WEAPONDROPED_FLY);
				break;

			case NET_WEAPONDROPED_FLY :
				pInfo->_fSendTime += rDeltaTime;
				if( pInfo->_fSendTime >= DROPEDWEAPON_SENDTIME )
				{
					bSend = true;
				}
				break;

			case NET_WEAPONDROPED_NONE :
				break;

			case NET_WEAPONDROPED_GET :
				pInfo->setState( NET_WEAPONDROPED_REMOVE ); 
				bSend = true;
				break;

			default ://NET_WEAPONDROPED_BOUND//NET_WEAPONDROPED_REMOVE
				bSend = true;
				break;
			}

			if( bSend )
			{
				pInfo->_fSendTime = 0.f;

				// ȸ������ ���մϴ�.
				MATRIX mat;
				
				i3Matrix::SetRight( &mat, 0.f, 0.f, 1.f, 0.f );
				i3Matrix::SetUp( &mat, 1.f, 0.f, 0.f , 0.f );
				i3Matrix::SetAt( &mat, 0.f, 1.f, 0.f, 0.f );
				i3Matrix::Mul( &mat, &mat, pWeapon->GetMatrix() );

				// broadcast packet
				GAMEEVENT::Write_DroppedWeapon( pWeapon->getNetworkIdx(), pWeapon->GetPos(), &mat);

				// process after send
				if( pInfo->getState() == NET_WEAPONDROPED_REMOVE)
				{
					I3TRACE( "[RemoveDropWeapon] idx : %d\n", pWeapon->getNetworkIdx() );
						
					// Ageia. ���⸦ �������� ���� �ٽ� ����ϱ� ���� CharaList�� �ű�ϴ�.
					PushDropWeaponToChara( pWeapon );
				}
			}
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			//									( Client )							//
			if( CStageInfoUtil::IsDeletableDroppedWeapon(*MyRoomInfoContext::i()->getStage()) )
			{	
				if( fLifeTime > DROPWEAPON_ALPHATIME )
				{
					pWeapon->setDroped( false );
					pWeapon->OnSetBlendAlpha( (DROPWEAPON_ALPHASLOPE - (fLifeTime - DROPWEAPON_ALPHATIME)) * DROPWEAPON_ALPHASLOPEINV );
				}

				if( fLifeTime > DROPWEAPON_LIFETIME + 1.f )
				{
					pInfo->setState( NET_WEAPONDROPED_REMOVE);
					pInfo->_fDropedWeaponLifeTime = 0.f;
					I3TRACE( "Remove Dropped weapon by lifetime.\n " );
				}
			}

			// ��ź���� Ȯ���Ͽ� 0�̸� �����մϴ�.
			if( (pWeapon->getTotalBulletCount() == 0 && pWeapon->getLoadedBulletCount() == 0 && pWeapon->getLoadedBulletDualCount() == 0 ) 
				&& pWeapon->IsDroped() )
			{	
				if( fLifeTime > 1.5f )
				{
					pWeapon->OnSetBlendAlpha( (1.f - (fLifeTime - 1.5f)));
				}
				if( fLifeTime > 3.f )
				{
					pInfo->setState( NET_WEAPONDROPED_REMOVE);
					pInfo->_fDropedWeaponLifeTime = 0.f;
				}
			}

			if( pInfo->isDroped() || fLifeTime > 3.f )
			{	// ���� ������� üũ(Ŭ���̾�Ʈ�� ��Ŷ�� �� ���� ��찡 ���� �� �����Ƿ� 3�� �Ŀ��� ����� ���·� üũ�Ѵ�)
				pWeapon->setDroped( true);
			}

			switch( pInfo->getState() )
			{
			case NET_WEAPONDROPED_FLY:
			case NET_WEAPONDROPED_START:
				pInfo->_tAssistData._fAssistTime += rDeltaTime;
				if( pInfo->_tAssistData._fAssistTime > DROPEDWEAPON_ASSIST_TIME )
					pInfo->_tAssistData._fAssistTime = DROPEDWEAPON_ASSIST_TIME;
				
				break;
			case NET_WEAPONDROPED_SLEEP:
				pInfo->_tAssistData._fAssistTime = DROPEDWEAPON_ASSIST_TIME;
				i3Vector::Copy( &pInfo->_tAssistData._vPostPos, const_cast<VEC3D*>(pInfo->_tNetData.getPosition()) );
				pWeapon->setDroped( true );
				break;
			case NET_WEAPONDROPED_REMOVE :
				{
					PushDropWeaponToChara( pWeapon );

					// ���� �ʱ�ȭ
					pInfo->Reset();
					i--;	// �������Ʈ���� �����߱⿡ �������� �ٽ� �˻�
					continue;
				}
				break;

			case NET_WEAPONDROPED_BOUND :
				{
					//VEC3D vUp;
					//i3Vector::Set( &vUp, 0.f, 1.f, 0.f );
					//g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_WEAPON_DOWN_COMMON, &pInfo->_vRecvPos, &vUp );
					pWeapon->setDroped( true );

					pInfo->_tAssistData._fAssistTime += rDeltaTime;
					if( pInfo->_tAssistData._fAssistTime > DROPEDWEAPON_ASSIST_TIME )
						pInfo->_tAssistData._fAssistTime = DROPEDWEAPON_ASSIST_TIME;
				}
				break;
			}

			VEC3D	vPos;
			REAL32 fSlerp = pInfo->_tAssistData._fAssistTime * DROPEDWEAPON_ASSIST_INVTIME;
			i3Vector::Lerp( &vPos, &pInfo->_tAssistData._vPostPos, pInfo->_tNetData.getPosition(), fSlerp );
			
			i3Matrix::SetRotateQuat( pWeapon->GetMatrix(), pInfo->_tNetData.getRotation() );	// �����̼��� ����������Ʈ�� ���� �ʽ��ϴ�.
			i3Matrix::PostTranslate( pWeapon->GetMatrix(), &vPos );

			pInfo->_bRecv = false;
		}

	}
}

i3::vector<WeaponThrowKnife*> * CGameWeaponManager::_GetThrowKnifeList( WEAPON::RES_ID num)
{
	tThrowKnifeCon::iterator it = m_ThrowKnifeList.find( num);
	if( it != m_ThrowKnifeList.end())
		return &it->second;

	I3PRINTLOG(I3LOG_FATAL,  "insert code here for throw knife!");
	return nullptr;
}

void CGameWeaponManager::_CreateThrowKnifeInstance( i3Stage * pStage, WEAPON::RES_ID weaponNumber)
{	
	VEC3D vPos;
	I3ASSERT( pStage != nullptr);

	tThrowKnifeCon::iterator it = m_ThrowKnifeList.find( weaponNumber);
	if( it != m_ThrowKnifeList.end())
	{	// �̹� �ִ�.
		return;
	}

	CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(WEAPON_CLASS_KNIFE, weaponNumber);
	if( pInfo == nullptr)	return;

	// ��ϵ��� ���� ������ ���, �������� ����.
	if( IsRegisteredWeapon(WEAPON_CLASS_KNIFE, weaponNumber) == false )
		return;

	i3::vector<WeaponThrowKnife*> List;

	i3InstanceManager * pManager = pStage->GetInstanceManager( WeaponBase::static_meta() );

	i3ClassMeta * pOldMeta = pManager->getInstanceMeta();
	pManager->setInstanceMeta( WeaponThrowKnife::static_meta());

	PreloadWeaponResource( WEAPON_CLASS_KNIFE, weaponNumber);

	I3ASSERT( i3::same_of<CMeleeWeaponInfo*>(pInfo));
	INT32 cnt = ((CMeleeWeaponInfo*)pInfo)->GetInstanceCount();

	List.reserve( cnt);

	for(INT32 i = 0; i < cnt; i++)
	{
		WeaponThrowKnife * pWeapon = (WeaponThrowKnife *) pManager->GetFreeInstance();
		I3ASSERT( pWeapon->GetRefCount() >= 1);
		
		pWeapon->Create( nullptr, pInfo->GetItemID(), true );
		pWeapon->Reset();
		pWeapon->setIdx( m_nWeaponIDCount);
		m_nWeaponIDCount++;
		pWeapon->SetEnable(false);

		// ���� �� ���߿� ����ݴϴ�.
		i3Vector::Set( &vPos, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getIdx() / 10) );
		MoveShapePosition( pWeapon, &vPos);

		List.push_back( pWeapon);
	}

	pManager->setInstanceMeta( pOldMeta);

	m_ThrowKnifeList.insert(tThrowKnifeCon::value_type(weaponNumber, List));
}

void CGameWeaponManager::_CreateThrowKnife( i3Stage * pStage, CWeaponInfo * pWeaponInfo, bool bMode)
{
	I3ASSERT( pWeaponInfo != nullptr);

	for( UINT32 j = 0; j < MAX_WEAPON_EXTENSION; j++)
	{
		if( pWeaponInfo->getExtensionType( j) == WEAPON::EXT_THROW)
		{
			T_ItemID extensionID = pWeaponInfo->GetExtensionItemID( j);
			if( extensionID > 0)
			{
				CWeaponInfo * pExtensionInfo = g_pWeaponInfoDataBase->getWeaponInfo( extensionID, true);
				
				if( pExtensionInfo->IsThrowWeapon() &&
					(pWeaponInfo->isModeWeapon() == bMode))
				{
					_CreateThrowKnifeInstance( pStage, WEAPON::ItemID2ResID( extensionID) );
					break;
				}
			}
		}
	}
}

void CGameWeaponManager::_CreateThrowKnife( i3Stage * pStage, WEAPON_CLASS_TYPE mainClassType)
{
	UINT32 cnt = WEAPON::getItemCount( mainClassType);
	for( UINT32 i = 1; i < cnt; i++)
	{
		CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( mainClassType, i);
		if( pWeaponInfo == nullptr)
			continue;

		_CreateThrowKnife( pStage, pWeaponInfo);
	}
}

void CGameWeaponManager::CreateAllThrowKnife( i3Stage * pStage)
{
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)  &&
		MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
	{
		CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_KNIFE, WEAPON::getItemIndex(WEAPON::FRYINGPAN), true);
		_CreateThrowKnife( pStage, pWeaponInfo, true);

		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_KNIFE, WEAPON::getItemIndex(WEAPON::HAIRDRYER), true);
		_CreateThrowKnife( pStage, pWeaponInfo, true);

		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_KNIFE, WEAPON::getItemIndex(WEAPON::CHINESECLEAVER_SEIZED), true);
		_CreateThrowKnife( pStage, pWeaponInfo, true);
	}
	else
	{
		// ������ ���������� ȭ�쵵 ���Ե˴ϴ�.
		_CreateThrowKnife( pStage, WEAPON_CLASS_KNIFE);
		_CreateThrowKnife( pStage, WEAPON_CLASS_DUALKNIFE);
		_CreateThrowKnife( pStage, WEAPON_SUBCLASS_BOW);
	}
}


void CGameWeaponManager::DestroyThrowKnife( i3Stage * pStage)
{
	I3ASSERT( pStage != nullptr);

	tThrowKnifeCon::iterator it = m_ThrowKnifeList.begin();
	while( it != m_ThrowKnifeList.end())
	{
		i3::vector<WeaponThrowKnife*> & list = it->second;
		UINT32 cnt = list.size();
		for( UINT32 i = 0; i < cnt; i++ )
		{
			WeaponBase * pWeapon = list[i];
			pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());
			pWeapon->ReturnInstance();
		}
		list.clear();
		it++;
	}

	m_ThrowKnifeList.clear();
}


void CGameWeaponManager::PushThrowKnife( WeaponBase * pWeapon)
{	
	// ���߿� ����ݴϴ�.
	VEC3D vPos;
	I3ASSERT( pWeapon != nullptr);

	i3Vector::Set( &vPos, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightThrow, (REAL32)(pWeapon->getIdx() / 10));
	MoveShapePosition( pWeapon, &vPos);

	// Network buffer reset
	_ResetNetworkInfo( pWeapon);

	pWeapon->SetOwner( nullptr);
	pWeapon->SetEnable(false);
	
	if( m_pStage->getWorld() != nullptr )
		m_pStage->getWorld()->getWorldRoot()->RemoveChild( pWeapon->getSceneObject());

	return;
}



void CGameWeaponManager::StageRoundEnd( void)
{


	// ���尡 ����Ǹ� 1.5���Ŀ� ��������� �մϴ�.
	for( UINT32 i = 0; i < m_DropWeaponList.size(); i++ )
	{
		WeaponBase * pWeapon = m_DropWeaponList[i];
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		if( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION )
			continue;

		if( pWeapon->getNetworkIdx() == -1 )	continue;

		GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( pWeapon->getNetworkIdx() );
		if( pInfo->_pWeaponRef == nullptr )	continue;

		pInfo->_fDropedWeaponLifeTime = DROPWEAPON_ALPHATIME - 0.5f;
	}

	// All grenade weapon is disable
	if(m_pThrowWeaponList != nullptr)
	{
		for( UINT32 i = 1;i < WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE); i++)
		{
			for( UINT32 j = 0; j < m_pThrowWeaponList[i].size(); j++ )
			{
				WeaponBase * pWeapon = m_pThrowWeaponList[i][j];
				PushThrowWeapon( pWeapon );
			}
		}
	}

	if( m_pThrowItemList != nullptr)
	{
		UINT32 count = GetItemCountThrowItem();
		for( UINT32 i = 1; i < count; i++)
		{
			for( UINT32 j = 0; j < m_pThrowItemList[ i].size(); j++)
			{
				WeaponBase * pWeapon = m_pThrowItemList[i][j];
				PushThrowWeapon( pWeapon);
			}
		}
	}

	//RemoveAllDropWeapon();	//	�������� �ʱ�ȭ�̹Ƿ� ���忡 ������ ���⸸ �����Ѵ�.

	// All grenade weapon is disable
	if(m_GrenadeShellWeaponList != nullptr)
	{
		UINT32 count = WEAPON::getItemCount( WEAPON_CLASS_GRENADESHELL);
		for(UINT32 i = 1; i < count; i++)
		{
			for( UINT32 j = 0; j < m_GrenadeShellWeaponList[i].size(); j++ )
			{
				WeaponBase * pWeapon = m_GrenadeShellWeaponList[i][j];
				PushThrowWeapon( pWeapon );
			}
		}
	}

	for(size_t i = 0; i < m_RocketLauncherShellWeaponList.size(); i++)
	{
		WeaponRocketLauncherShell * pWeapon = m_RocketLauncherShellWeaponList[i];
		PushRocketLauncherWeapon( pWeapon);
	}

}

void CGameWeaponManager::PreloadWeaponResource( WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number)
{
	char pszRes[256] = "";

	CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(classType, number);

	if ( pInfo == nullptr)
		return;


	INT32 len = i3::generic_string_size( pInfo->GetName());
	if( len > 0)
	{
		I3ASSERT( len < 256);

		GlobalFunc::DecryptValue( pszRes, pInfo->GetName(), len, ENC_SHIFT_WEAPONINFO_RESNAME);

		i3::generic_string_cat( pszRes, ".I3CHR");

		// ���� I3CHR ���ҽ��� ������ �ε��Ǿ����� Ȯ��
		g_pFramework->QuaryResource( pszRes);
	}
}

bool CGameWeaponManager::IsRegisteredWeapon(WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number)
{
	// ��ϵ� �������� Ȯ��.	
	CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(classType, number);
	return (__GetWeaponResourcePath( pInfo) != nullptr);
}

WeaponBase * CGameWeaponManager::_CreateNewWeapon( T_ItemID Itemid, T_ItemID subItem, CGameCharaBase * pOwner, bool bCreateInWeapon )
{
//	char szClassMeta[256];

	// Instance�� ���� ����
	if( m_pStage == nullptr)
		return nullptr;

	i3InstanceManager * pManager	= m_pStage->GetInstanceManager( WeaponBase::static_meta());
	i3ClassMeta * pMeta				= WeaponBase::static_meta();

	if( subItem == 0)
		subItem = Itemid;

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( Itemid, true);
	if( pWeaponInfo == nullptr)
	{
		return nullptr;
	}

	
	//	Weapon Class Meta - komet
	//	���ο� ���⸦ ���� �߰��ϱ� ���� �ҽ����� �ڵ带 �߰��ߴ� ���� ����� PEF ���Ͽ��� ���ڿ��� �����Ҽ� �ֵ��� ����.
	//	BC���Ͽ� ������ Ŭ���� ��Ÿ �̸��� �޾ƿ´�.
	//	������ �ȵ������� �⺻������ WeaponBase�� �����Ѵ�.
	
	i3::rc_wstring	wstrClassMeta;

	pWeaponInfo->GetClassMetaName(wstrClassMeta);

//	char * pszCreateClassMeta = szClassMeta;
	if( i3::generic_string_size( wstrClassMeta) > 0)
	{
		i3::string		strClassMeta;
		i3::utf16_to_mb(wstrClassMeta, strClassMeta);

		pMeta = i3ClassMetaSystem::i()->find_class_meta(strClassMeta);
			// i3ClassMeta::FindClassMetaByName( pszCreateClassMeta);
		I3ASSERT( pMeta != nullptr);

		if( pMeta == nullptr)
		{
			char message[MAX_STRING_COUNT] = "";
			i3::snprintf( message, MAX_STRING_COUNT, 
				"[%s] �ش� ������ class meta�� ã���� ���ų� ��ϵ��� �ʾҽ��ϴ�. �ٽ� Ȯ���� �ֽʽÿ�.", strClassMeta);
			I3PRINTLOG(I3LOG_NOTICE,message); 

			return nullptr;
		}
	}

	i3ClassMeta * pOldMeta		= pManager->getInstanceMeta();
	pManager->setInstanceMeta( pMeta);

	WeaponBase * pQuaryWeapon = (WeaponBase *) pManager->GetFreeInstance();
	I3ASSERT( pQuaryWeapon->GetRefCount() == 2);

	pManager->setInstanceMeta( pOldMeta);

	//	�ݵ�� Instance�� ����ϱ� ���ؼ��� AttachStage()�Ǿ� �־�� �Ѵ�. ����!
	I3ASSERT( pQuaryWeapon != nullptr);

	// ĳ���� �ִϸ��̼ǵ� �ε�����
	if( pOwner != nullptr && pOwner->getCharaInfo()->GetType() != CHARA::TYPE_PROFILE)
	{
		//0.6ms
		pOwner->PreloadAnimationByWeapon( subItem);
	}

	// 8ms
	pQuaryWeapon->Create( pOwner, subItem, bCreateInWeapon);

	// ����Ʈ ��Ʈ �ڽ�
	if( pOwner != nullptr && g_pEnvSet->m_nEffectLevel > 1)
		pQuaryWeapon->CreateContactHitBox( pOwner->getCharaNetIndex());

	//	ĳ���� ���� �ν��Ͻ��� ����Ʈ�� �߰�
	_AddCharaWeapon( pQuaryWeapon );		// �� �ʿ��� ADDREF�Ǿ����Ƿ�..
	pQuaryWeapon->Release();				// ���⼭ Relaseó��..(2015.04.27.����)


	// ���⿡ ���� �ε����� �ο��մϴ�.
	pQuaryWeapon->setIdx( m_nWeaponIDCount );
	m_nWeaponIDCount++;

	return pQuaryWeapon;
}

WeaponBase * CGameWeaponManager::_CreateDinoWeapon( T_ItemID Itemid, T_ItemID subItem, CGameCharaBase * pOwner )
{
	// Instance�� ���� ����
	if( m_pStage == nullptr)
		return nullptr;

	i3InstanceManager * pManager	= m_pStage->GetInstanceManager( WeaponBase::static_meta());
	i3ClassMeta * pMeta				= WeaponBase::static_meta();

	if( subItem == 0)
		subItem = Itemid;

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( Itemid, true);
	if (pWeaponInfo == nullptr)
	{
		return nullptr;
	}
	
	i3::rc_wstring	wstrClassMeta;
	pWeaponInfo->GetClassMetaName(wstrClassMeta);

	if( i3::generic_string_size( wstrClassMeta) > 0)
	{
		i3::string strClassMeta;
		i3::utf16_to_mb(wstrClassMeta, strClassMeta);

		pMeta = i3ClassMetaSystem::i()->find_class_meta(strClassMeta);
		I3ASSERT( pMeta != nullptr);

		if( pMeta == nullptr)
		{
			char message[MAX_STRING_COUNT] = "";
			i3::snprintf( message, MAX_STRING_COUNT, 
				"[%s] �ش� ������ class meta�� ã���� ���ų� ��ϵ��� �ʾҽ��ϴ�. �ٽ� Ȯ���� �ֽʽÿ�.", 
				strClassMeta);
			I3PRINTLOG(I3LOG_NOTICE,message); 

			return nullptr;
		}
	}

	i3ClassMeta * pOldMeta		= pManager->getInstanceMeta();
	pManager->setInstanceMeta( pMeta);

	WeaponBase * pQuaryWeapon = (WeaponBase *) pManager->GetFreeInstance();
	I3ASSERT( pQuaryWeapon->GetRefCount() == 2);

	pManager->setInstanceMeta( pOldMeta);

	//	�ݵ�� Instance�� ����ϱ� ���ؼ��� AttachStage()�Ǿ� �־�� �Ѵ�. ����!
	I3ASSERT( pQuaryWeapon != nullptr);

	// 8ms
	pQuaryWeapon->Create( pOwner, subItem, false);

	return pQuaryWeapon;
}

WeaponBase * CGameWeaponManager::_FindFreeWeapon( T_ItemID Itemid )
{
	UINT32 cnt = m_CharaWeaponList.size();
	for( size_t i = 0; i < cnt; i++)
	{
		WeaponBase * pFindWeapon = m_CharaWeaponList[i];
		I3ASSERT( pFindWeapon != nullptr);

		if( pFindWeapon->getPairWeapon() != nullptr)
			continue;

		CWeaponInfo * pWeaponInfo = pFindWeapon->getWeaponInfo();

		if ( ! (pWeaponInfo->GetItemID() ==(T_ItemID)Itemid) )
			continue;

		if ( !pFindWeapon->isTakeChara() )
		{
		#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
			if( pFindWeapon->GetRefCount() > 3)
			{
				I3PRINTLOG(I3LOG_FATAL,  "QueryWeapon RefCount invalid. itemid %d",  pWeaponInfo->GetItemID());
			}
		#endif
			return pFindWeapon;
		}

	}

	return nullptr;
}

void CGameWeaponManager::_ClearGarbageCharaWeaponList( void)
{	
	// ������ ����� ����
	for( size_t i = 0; i < m_CharaWeaponList.size(); i++)
	{
		WeaponBase * pFindWeapon = m_CharaWeaponList[i];
		I3ASSERT( pFindWeapon != nullptr);

		if( pFindWeapon->getPairWeapon() != nullptr)
			continue;

		bool bDeletable = false;

		if( pFindWeapon->isTakeChara() == false)
		{
			if( pFindWeapon->getCreater() != nullptr)
			{
				INT32 userRespawnCnt = pFindWeapon->getCreater()->getCharaInfo()->GetRespawnCnt();
				if( userRespawnCnt - pFindWeapon->getUserRespawnCnt() > 2)
				{	// 3ȸ ������ ���� ������ ���� ����
					bDeletable = true;
				}

				if ( pFindWeapon->getOwner() != nullptr && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
				{
					// �� ĳ���Ͱ� ���� ����� ��� ���� ����� �߰��� ����� �ȵ�
					if ( pFindWeapon->getOwner()->getMultiWeaponContext()->IsUsingMultiWeapon(pFindWeapon))
						continue;
				}
			}
			else if( pFindWeapon->getOwner() == nullptr)
			{	// ������ ���� ���
				bDeletable = true;
			}	
		}

		if( bDeletable)
		{
			if( ReleaseCharaWeapon( pFindWeapon))
				i--;
		}
	}
}

void CGameWeaponManager::__RegisterWeaponResource( WEAPON_CLASS_TYPE classType, INT32 num, i3GameResChara * pRes)
{
	if( pRes != nullptr)
	{
		I3TRACE( "load res %d %d - %s\n", classType, num, pRes->GetName());
	}

	i3::vu::set_value_force(m_WeaponResList[ classType], num, pRes);	//	���� ��ȣ��� ����
}


void CGameWeaponManager::__RemoveWeaponBGLoadInfo(void)
{
	REAL32 tm = 0.0f;

	// �ε����� ���ҽ��� �ִ� ���, 5�ʰ� �ε� �ϷḦ ���.
	if( isBGLoading() )
	{
		I3PRINTLOG(I3LOG_NOTICE, "Queue is not empty\n");

		INT32 queue_size = m_BgLoadQueue.size() + m_BgPriorityLoadQueue.size();
		INT32 delete_count = 0;

		for( INT32 idx = 0 ; idx < (INT32)m_BgLoadQueue.size() ; ++idx)
		{
			sWEAPONLOADINFO * pInfo = m_BgLoadQueue[idx];

			if( pInfo->_state != eWEAPONLOAD_STATE_LOADING )
			{
				m_BgLoadQueue.erase(m_BgLoadQueue.begin() + idx);
				--idx;
				delete_count++;
			}
		}		

		for( INT32 idx = 0 ; idx < (INT32)m_BgPriorityLoadQueue.size() ; ++idx)
		{
			sWEAPONLOADINFO * pInfo = m_BgPriorityLoadQueue[idx];

			if( pInfo->_state != eWEAPONLOAD_STATE_LOADING )
			{
				m_BgPriorityLoadQueue.erase(m_BgPriorityLoadQueue.begin() + idx);
				--idx;
				delete_count++;
			}
		}	

		I3PRINTLOG(I3LOG_NOTICE, "Queue Total Count = %d, Delete Count = %d", queue_size, delete_count);

		i3Timer timer;
		while( isBGLoading() && tm < 3.0f )
		{
			tm += timer.GetDeltaSec();
			__BackgroundLoading();
		}

		I3PRINTLOG(I3LOG_NOTICE, "Finish wait. time = %f\n",tm );
	}
}

bool CGameWeaponManager::__GetWeaponFileName( char * pszOut, CWeaponInfo * pWeaponInfo)
{
	I3ASSERT( pWeaponInfo != nullptr);

	INT32 len = i3::generic_string_size( pWeaponInfo->GetName());

	if( len > 0)
	{
		I3ASSERT( len < MAX_PATH);
		GlobalFunc::DecryptValue( pszOut, pWeaponInfo->GetName(), len, ENC_SHIFT_WEAPONINFO_RESNAME);

		i3::generic_string_cat( pszOut, ".I3CHR");
		return true;
	}

	return false;
}

const char * CGameWeaponManager::__GetWeaponResourcePath( CWeaponInfo * pWeaponInfo)
{
	char szResPath[ MAX_PATH];

	if( pWeaponInfo == nullptr)
		return nullptr;

	if( __GetWeaponFileName( szResPath, pWeaponInfo) == true)
	{
		return g_pFramework->FindResourceFullPathByOnlyName( szResPath);
	}

	return nullptr;
}

const char * CGameWeaponManager::__GetWeaponResourcePath( WEAPON_CLASS_TYPE classType, INT32 num)
{
	return __GetWeaponResourcePath( g_pWeaponInfoDataBase->getWeaponInfo( classType, num));
}

const char * CGameWeaponManager::__GetWeaponResourcePath( T_ItemID ItemID)
{
	return __GetWeaponResourcePath( g_pWeaponInfoDataBase->getWeaponInfo( ItemID));
}

bool CGameWeaponManager::__LoadWeapon( sWEAPONLOADINFO * pInfo)
{
	I3ASSERT( pInfo != nullptr);

	switch( pInfo->_state)
	{
	case eWEAPONLOAD_STATE_PUSH :
		{
			BGLoadThread * pThread = g_pFramework->GetBGLoadThread();
			if( pThread != nullptr )
			{
				const char * pszPath = __GetWeaponResourcePath( pInfo->_ItemID);

				if( pszPath != nullptr)
				{
					pInfo->_pThread = pThread;
					pInfo->_state = eWEAPONLOAD_STATE_LOADING;
					pThread->SetLoadCmd( pszPath);
				}
				else
				{
					I3PRINTLOG(I3LOG_NOTICE, "Weapon Path is NULL, Weapon ItemID %d", pInfo->_ItemID );
					return true;
				}
			}
		}
		break;

	case eWEAPONLOAD_STATE_LOADING :
		{
			// �ε� �ϷḦ Ȯ��
			i3GameResChara * pRes = static_cast<i3GameResChara*>(pInfo->_pThread->GetRes());

			if( pRes != nullptr)
			{
				WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( pInfo->_ItemID );
				WEAPON::RES_ID number = WEAPON::ItemID2ResID( pInfo->_ItemID );

				if( m_WeaponResList[ classType][ number] == nullptr)
				{	// ���ҽ� �޴����� ���������

					i3ResourceManager* pResMgr = g_pFramework->GetResourceManager();

					const char * pszPath = __GetWeaponResourcePath( pInfo->_ItemID);
					
					i3GameResChara* pResForCheck = static_cast<i3GameResChara*>(pResMgr->FindResourceByPath(pszPath));		// ���ξ����� �ε����� ������ �ε��� ���..
					
					if (pResForCheck == nullptr)
						g_pFramework->GetResourceManager()->AddResource( pRes);

					__RegisterWeaponResource( classType, number, pRes);
					I3TRACE( "PreLoad Weapon Complate %d %s\n", pInfo->_ItemID, pRes->GetName());
				}

				I3_MUST_RELEASE(pRes);
				m_pLoadInfoPool->Free( pInfo);
				return true;
			}
		}
		break;
	}

	return false;
}

void CGameWeaponManager::__BackgroundLoading( void)
{
	if( !m_BgPriorityLoadQueue.empty())
	{
		sWEAPONLOADINFO * pInfo = m_BgPriorityLoadQueue.front();
		if( __LoadWeapon( pInfo))
			m_BgPriorityLoadQueue.pop_front();
	}
	else
	{
		if( !m_BgLoadQueue.empty())
		{
			sWEAPONLOADINFO * pInfo = m_BgLoadQueue.front();
			if( __LoadWeapon( pInfo))
				m_BgLoadQueue.pop_front();
		}
	}
}

bool CGameWeaponManager::_IsLoadPushed( T_ItemID itemID, bool bPriority)
{
	i3::deque<sWEAPONLOADINFO*>* pque = &m_BgLoadQueue;

	if( bPriority)
	{
		pque = &m_BgPriorityLoadQueue;
	}

	i3::deque<sWEAPONLOADINFO*>& que = *pque;

	UINT32 cnt = que.size();

	for( UINT32 i = 0; i < cnt; i++)
	{
		sWEAPONLOADINFO * pInfo = que.front();

		que.pop_front();
		que.push_back( pInfo);

		if( pInfo->_ItemID == itemID)
			return true;
	}
	
	return false;
}

void CGameWeaponManager::PushLoadWeapon( T_ItemID itemID, bool bPriority)
{
	if( itemID > 0)
	{
		i3GameResChara * pRes = QueryResource( itemID, false);

		if( pRes == nullptr)
		{
			if( _IsLoadPushed( itemID, bPriority) == false)
			{
				I3ASSERT( m_pLoadInfoPool != nullptr);

				sWEAPONLOADINFO * pNewLoadCmd = reinterpret_cast<sWEAPONLOADINFO*>(m_pLoadInfoPool->Alloc( sizeof( sWEAPONLOADINFO), __FILE__, __LINE__));

				pNewLoadCmd->_state = eWEAPONLOAD_STATE_PUSH;
				pNewLoadCmd->_ItemID = itemID;

				if( bPriority)
					m_BgPriorityLoadQueue.push_back( pNewLoadCmd);
				else
					m_BgLoadQueue.push_back( pNewLoadCmd);

				I3TRACE( "PreLoad Weapon %d\n", itemID);
			}
		}
	}
}


void CGameWeaponManager::PreloadMyWeapons( void)
{
	// ���⸸ ��� �ε�
	CInvenList * pInven = CInvenList::i();

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	if( pChara != nullptr)
	{
		UINT32 size = pInven->GetItemCount( INVEN::DB_WEAPON);
		for(UINT32 i = 0; i < size; i++)
		{
			T_ItemID itemID = pInven->GetItemID( INVEN::DB_WEAPON, i);

			if( WeaponLRU_Manager::i()->IsPriorityWeapon( itemID))
			{
				PushLoadWeapon( itemID);
				pChara->PreloadAnimationByWeapon( itemID);
			}
		}
	}
}


void CGameWeaponManager::_CollectBackgroundClassIDList(i3::vector<size_t>(& inout)[WEAPON_CLASS_COUNT])
{
	const i3::deque<sWEAPONLOADINFO*>& vecLoadQueue1 = m_BgPriorityLoadQueue;
	const size_t num_Que1 = vecLoadQueue1.size();

	for (size_t i = 0 ; i < num_Que1 ; ++i)
	{
		sWEAPONLOADINFO* info = vecLoadQueue1[i];
		WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( info->_ItemID );
		WEAPON::RES_ID number = WEAPON::ItemID2ResID( info->_ItemID );		
		inout[classType].push_back(number);
	}
	
	const i3::deque<sWEAPONLOADINFO*>& vecLoadQueue2 = m_BgLoadQueue;
	const size_t num_Que2 = vecLoadQueue2.size();

	for (size_t i = 0 ; i < num_Que2 ; ++i)
	{
		sWEAPONLOADINFO* info = vecLoadQueue2[i];
		WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( info->_ItemID );
		WEAPON::RES_ID number = WEAPON::ItemID2ResID( info->_ItemID );		
		inout[classType].push_back(number);
	}
}

void CGameWeaponManager::TrashWeaponRes( REAL32 tm)
{
	if( isBGLoading() )
		return;
	
	m_rTrashTimer += tm;
	if( m_rTrashTimer > 30.f)
	{
		_ClearGarbageCharaWeaponList();	// ���� ���� ���� Ȯ��

		i3::vector<size_t> avecClassID[WEAPON_CLASS_COUNT];
		CGameWeaponManager::_CollectBackgroundClassIDList(avecClassID);

		i3ResourceManager * pMng = g_pFramework->GetResourceManager();
		for( UINT32 i = 0; i < WEAPON_CLASS_COUNT; i++)
		{
			const i3::vector<size_t>& vecClassID = avecClassID[i];

			for( UINT32 j = 0; j < m_WeaponResList[i].size(); j++)
			{
				i3GameResChara * pRes = m_WeaponResList[i].at(j);
				if( pRes != nullptr && pRes->isCommonRes() == false)
				{
					if( pRes->GetRefCount() == 2)
					{
						i3::vector<size_t>::const_iterator it = i3::find(vecClassID.begin(), vecClassID.end(), j);

						if (it == vecClassID.end() )
						{
							pMng->RemoveResource( pRes);
							m_WeaponResList[i][j] = nullptr;
						}
						else
						{
							I3TRACE("can't delete resource because of background loading : %s !!!! \n", pRes->GetName());
						}
					}
				}
			}
		}

		m_rTrashTimer = 0.f;
	}
}

const WeaponDecoySndInterval* CGameWeaponManager::GetDecoySndInterval(WEAPON_CLASS_TYPE weaponCT)
{
	return m_decoySndInterval;
}

const WeaponDecoySndInterval* CGameWeaponManager::GetEventDecoySndInverval(void)
{
	return m_EventDecoySndInterval;
}


void CGameWeaponManager::CreateAllSpecialDamageFilter()
{
	if( g_pFramework->IsBattleStage() )
	{
		WeaponBase * pWeapon;
		INT32 temp_index = -1;
		for( UINT32 i = 0; i < m_CharaWeaponList.size(); i++ )
		{
			pWeapon = m_CharaWeaponList[i];
			temp_index = pWeapon->getWeaponInfo()->GetWeaponSpecialHitHudIndex();
			if(temp_index > -1)
				CHud::i()->InsertSpecialDamageFilter(temp_index);
		}	
	}
}

void CGameWeaponManager::CreateAllSpecialDieFilter()
{
	if (g_pFramework->IsBattleStage())
	{
		WeaponBase * pWeapon;
		INT32 temp_index = -1;
		for (UINT32 i = 0; i < m_CharaWeaponList.size(); i++)
		{
			pWeapon = m_CharaWeaponList[i];
			temp_index = pWeapon->getWeaponInfo()->GetWeaponSpecialDieHudIndex();
			if (temp_index > -1)
			{
				CHud::i()->InsertSpecialDieFilter(temp_index);
			}
		}
	}
}

void CGameWeaponManager::CreateAllScopeHudTexture()
{
	if (g_pFramework->IsBattleStage())
	{
		WeaponBase * pWeapon;
		INT32 temp_index = -1;
		for (UINT32 i = 0; i < m_CharaWeaponList.size(); i++)
		{
			pWeapon = m_CharaWeaponList[i];
			temp_index = pWeapon->getWeaponInfo()->GetScopeType();
			if (temp_index > -1)
			{
				CHud::i()->InsertScopeHudTexture(temp_index);
			}
		}
	}
}


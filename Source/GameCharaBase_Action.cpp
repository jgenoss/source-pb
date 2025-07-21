#include "pch.h"
#include "GameCharaBase.h"
#include "GameCharaActionContext.h"

void CGameCharaBase::ResolveAnimName( char * pszDest, const char * pszSrc, INT32 tryCount)
{
	char conv[256];

	while( *pszSrc)
	{
		if( *pszSrc == '%')
		{
			pszSrc++;

			switch( *pszSrc)
			{
				case 'C' :
				case 'c' :		// Character Name
					{
						m_pDBInfo->getResPath( conv);

						i3String::Copy( pszDest, conv);

						pszDest += i3String::Length( conv);
					}
					break;

				case 'V' :
				case 'v' :		// Viewing Mode (1PV, 3PV)
					if( (m_AnimSet == CHARA_ANIMSET_1PV) && (tryCount == 0))
					{
						i3String::Copy( pszDest, "1PV");
					}
					else
					{
						i3String::Copy( pszDest, "3PV");
					}

					pszDest += 3;
					break;

				case 'L' :
				case 'l' :		// Lower Mode (Stand, Sit)
					switch( getActionContext()->getBodyLower())
					{
						case CHARA_BODYLOWER_STAND :	i3String::Copy( pszDest, "STAND");	pszDest += 5;	break;
						default :						i3String::Copy( pszDest, "SIT");	pszDest += 3;	break;
					}
					break;

				case 'W' :
				case 'w' :		// Weapon Name
					{
						WeaponBase * pWeapon = GetCurrentCharaWeapon();

						if( pWeapon != NULL)
						{
							CWeaponInfo * pInfo = pWeapon->getWeaponInfo();
							I3ASSERT( pInfo != NULL);

							i3String::Copy( pszDest, pInfo->GetName());

							pszDest += i3String::Length( pInfo->GetName());
						}
					}
					break;
			}

		}
		else
		{
			*pszDest = (char) toupper( *pszSrc);
			pszDest ++;
		}

		pszSrc++;
	}

	*pszDest = 0;
}

void CGameCharaBase::_Rec_BindAnim( i3GameNode * pNode)
{
	if( pNode->IsExactTypeOf( i3AIContext::GetClassMeta()))
	{
		i3AIContext * pCtx = (i3AIContext *) pNode;
		INT32 i, j;
		char szName[ MAX_PATH];
		i3Animation * pAnim;

		for( i = 0; i < pCtx->getAIStateCount(); i++)
		{
			i3AIState * pState = pCtx->getAIState( i);

			if( (pState->getStyle() & I3_AISTATE_STYLE_ANIM_TEMPLATE) == 0)
			{
				// Animation Name Template �Ӽ��� ������ ���� �ʴٸ�, Animation�� ������ �ʿ䰡 ������ ���Ѵ�.
				continue;
			}

			
			// Animation Name Template�� ������ �ִٸ� �ش� ���ڿ��� �����ϰ� ġȯ�� �־�� �Ѵ�.
			//
			// %C : ĳ���� �̸�
			// %V : 1PV or 3PV
			// %L : Stand or Sit
			// %W : Weapon �̸�
			for( j = 0, pAnim = NULL; (j < 2) && (pAnim == NULL); j++)
			{
				// �� ĳ���ͷ� Animation�� �˻����� ���� ���, ���� Animation�� �˻��ϵ���...

				ResolveAnimName( szName, (const char *) pState->getAnimName(), j);

				pAnim = (i3Animation *) g_pFramework->FindResourceFromManager( szName, i3Animation::GetClassMeta());
			}

			if( pAnim != NULL)
			{
				pState->SetAnim( pAnim);
			}
			else
			{
				I3FATAL( "Could not bind a proper animation. : %s\n", szName);
			}
		}
	}

	// Child Node�鿡 ���� Recursive call
	{
		i3GameNode * pChild = pNode->getFirstChild();

		while( pChild != NULL)
		{
			_Rec_BindAnim( pChild);

			pChild = pChild->getNext();
		}
	}

}

void CGameCharaBase::BindLowerAnim(void)
{
	// Lower AI Context�� ã�� �˻�
	i3GameNode * pNode = this->FindChildByName( "Lower", FALSE);
	I3ASSERT( pNode != NULL);

	_Rec_BindAnim( pNode);
}


void CGameCharaBase::BindUpperAnim(void)
{
	char * pszWeaponClass = NULL;
	WeaponBase * pWeapon = GetCurrentCharaWeapon();

	if( pWeapon == NULL)
		return;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

	// �ش� ������ Class�� ���� Rebind
	switch( pWeaponInfo->GetTypeClass())
	{
		case WEAPON_CLASS_KNIFE :			pszWeaponClass = "Knife";	break;
		case WEAPON_CLASS_HANDGUN :			pszWeaponClass = "HandGun";	break;
		case WEAPON_CLASS_ASSAULT :			pszWeaponClass = "AssaultRifle";	break;
		case WEAPON_CLASS_SMG :				pszWeaponClass = "SMG";	break;
		case WEAPON_CLASS_SNIPER :			pszWeaponClass = "SniperRifle";	break;
		case WEAPON_CLASS_SHOTGUN :			pszWeaponClass = "ShotGun";	break;
		case WEAPON_CLASS_THROWING :		pszWeaponClass = "Throwing";	break;
		case WEAPON_CLASS_ITEM :			pszWeaponClass = "Throwing";	break;
		case WEAPON_CLASS_MISSION :			pszWeaponClass = "Mission";	break;
		case WEAPON_CLASS_MG :				pszWeaponClass = "MG";	break;
		//case WEAPON_CLASS_GRENADESHELL :	pszWeaponClass = "Throwing";	break;
		case WEAPON_CLASS_BOMBTRIGGER :		pszWeaponClass = "Bomb";	break;
		case WEAPON_CLASS_CIC :				pszWeaponClass = "CIC";	break;
		case WEAPON_CLASS_DUALHANDGUN :		pszWeaponClass = "DualHandGun";	break;
		case WEAPON_CLASS_DUALKNIFE :		pszWeaponClass = "DualKnife";	break;
		case WEAPON_CLASS_ROCKET_LAUNCHER :	pszWeaponClass = "RocketLauncher";	break;
		//case WEAPON_CLASS_OBJECT_GUN :		pszWeaponClass = "Throwing";	break;
		case WEAPON_CLASS_DUALSMG :			pszWeaponClass = "SMG";	break;

		//case WEAPON_CLASS_DINO :
		//case WEAPON_CLASS_DINO_LEFT_SCRATCH :

		default :
			I3ASSERT_0;
	}

	i3GameNode * pNode = this->FindChildByName( pszWeaponClass, TRUE);
	I3ASSERT( pNode != NULL);

	_Rec_BindAnim( pNode);
}

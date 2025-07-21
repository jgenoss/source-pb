/**
 * \file	GameEvent.h
 *
 * Declares the game event class.
 */


#if !defined( __GAME_EVENT_H__)
#define __GAME_EVENT_H__

//-------------------------------------------------------------------------------------//
//			Network�� Sync�� ���ߴ� �����鿡 ���� Function�Դϴ�.
//			Character
//			Throwing Weapon
//			Dropped Weapon
//			Object

namespace GAMEEVENT
{
	/** \brief ���� ���� event�� �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] pTargetPos ������
		\param[in] pWeapon ���� */
	void	Write_Chara_Fire( CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon);

#ifdef DOMI_STING_POISON_GRENADE
	void	Write_Chara_Fire_DomiDinoGrenade( CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon);
#endif

	/** \brief Ÿ�� event�� �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nHitWho ���� ĳ���� index
		\param[in] nHitPart ���� ����
		\param[in] rDamage ���ݷ� */
	void	Write_Chara_Hit( CGameCharaBase * pChara, const CHARA_DAMAGEINFO * pDamageInfo, bool bByObj = false);

	/** \brief ���� ������ event�� �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] iWeaponSlot ���� ���� ��ȣ
		\param[in] iLoadedBullet ���� źȯ ��
		\param[in] iDulaLoadedBullet Dual źâ�� ���� źȯ ��
		\param[in] iTotalBullet �� źȯ ��*/
	void	Write_Chara_DropWeapon( CGameCharaBase * pChara, WEAPON_SLOT_TYPE iWeaponSlot, INT32 iLoadedBullet, INT32 iDualLoadedBullet, INT32 iTotalBullet);

	/** \brief ���� �ݱ� event�� �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] pWeapon ���� */
	void	Write_Chara_PickupWeapon( CGameCharaBase * pChara, WeaponBase * pWeapon);

	/** \brief ���� �ݰ� �ٷ� ������ event�� �����մϴ�.
	\param[in] pChara ĳ���� ����
	\param[in] pDropWeapon ������ ���� 
	\param[in] GetItemID ���� ������ ����*/
	void	Write_Chara_DirectPickupWeapon(CGameCharaBase * pChara, T_ItemID GetItemID);

	/** \brief �̼� ������ �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nState �̼� ����
		\param[in] nArea �̼� ���� */
	void	Write_Chara_Mission( CGameCharaBase * pChara, NET_MISSION_STATE nState, BOMB_AREA nArea, T_ItemID ItemId 
#ifdef ALTER_DOMI_INSTALL_OBJ
		, DOMI_TYPE ObjType, INT16 ObjIdx 
#else
		, INT32 ObjIdx,REAL32 fTime
#endif
		);

	/** \brief �ڻ� Damge�� �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nDamage ���ݷ�
		\param[in] iByObjectIdx Object�� ���� ����� Object Index
		\note object index�� Damage�� �ִ� ��� Object Index�� �����˴ϴ�.
			default�� 0xFF��*/
	void	Write_Chara_SuicideDamage( CGameCharaBase * pChara, CHARA_DEATH_TYPE type,
		CHARA_HIT_PART part, UINT16 nDamage = 100, VEC3D * pDir = nullptr,
		WEAPON_CLASS_TYPE WeaponClass = WEAPON_CLASS_UNKNOWN, INT32 WeaponNum = -1,
		UINT16 iByObjectIdx = 0xFFFF, INT32 KillerIdx = -1,
		WEAPON_SLOT_TYPE WeaponUsage = WEAPON_SLOT_PRIMARY);

	/** \brief RadioChat ������ �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nRadio radio chat ������ */
	void	Write_Chara_RadioChat( CGameCharaBase * pChara, UINT8 nRadio);

	/** \brief Object ��뿡 ���� ������ �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nObjIdx Object Index
		\param[in] nActionKey ��� ���� */
	void	Write_Chara_UseObject( CGameCharaBase * pChara, i3Object * pObj, UINT8 nActionKey);

	/** \brief Object Sync�� ������ �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nObjIdx Object Index
		\param[in] nSyncActionKey Sync ���� key
		\note for Only Host */
	void	Write_Chara_SyncUseObject( CGameCharaBase * pChara, i3Object * pObj, UINT8 nSyncActionKey);
	
	/** \brief ĳ���� ������ ���� ������ �����մϴ�.
		\note for Only Host
		\param[in] pDeathInfo ������ ���� ���� structure */
	void	Write_Chara_Death( GAMETOCONTEXT_INFO_CHARA_DEATH * pDeathInfo);

	/** \brief ĳ���� �ǰݿ� ���� ������ �����մϴ�.
		\note for Only Host
		\param[in] pChara ĳ���� ����
		\param[in] type �״� Ÿ��
		\param[in] nShooter ���� ĳ���� index
		\param[in] nHitPart ���� ����		*/
	void	Write_Chara_Suffering( CGameCharaBase * pChara, CHARA_DEATH_TYPE type, INT32 nShooter, CHARA_HIT_PART nHitPart, bool bFlag);

	/** \brief ���忡 ������ ����ź ������ �����մϴ�.
		\note BindNetGrenadeInfo �� ȣ��Ǳ����� ���� ȣ���ϸ� �ȵ˴ϴ�.
		\param[in] idx Network index
		\param[in] pThrowWeaponInfo ���� structrue		*/
	void	Write_ThrowingWeapon( INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo);

#ifdef DOMI_STING_POISON_GRENADE
	void	Write_DomiDinoThrowWeapon( INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo);
#endif

//#ifdef DOMI_UDP_OBJECT
	void	Write_DomiSkillObjectInstall(INT32 customerIdx, INT8 objectIdx);
	void	Write_DomiSkillObjectRepair(INT32 customerIdx, INT8 objectIdx);
	void	Write_DomiSkillObjectUpgrade(INT32 customerIdx, INT8 objectIdx, INT8 lv);
	void	Write_DomiSkillObjectSupplyUse(INT32 customerIdx, INT8 objectIdx);
	void	Write_DomiDropItemCatched(INT32 userIdx, INT8 objectIdx);
//#endif

	/** \brief ������Ʈ ������ �����մϴ�.
		\note for Only Host
		\param[in] idx network index
		\param[in] nHP HP
		\param[in] pPos ��ġ
		\param[in] pRot ȸ��		*/
	void	Write_Object( INT32 idx, INT32 EventSender, UINT16 nHP, VEC3D * pPos = nullptr, QUATERNION * pRot = nullptr );

	/** \brief ���忡 ������ ���� ����
		\param[in] idx network index
		\param[in] pPos ��ġ
		\param[in] pMtxRot ȸ��*/
	void	Write_DroppedWeapon( INT32 idx, VEC3D * pPos, MATRIX * pMtxRot);

	/** \brief animation object ������ �����մϴ�.
		\note �ִϸ��̼� ������Ʈ�� Ű�����Ӹ� �����մϴ�.
			rBroadcastTime ���� ��ũ�� ���ߵ��� �մϴ�.
		\param[in] idx network index
		\param[in] nHP HP
		\param[in] rKeyFrame key��			*/
	void	Write_Object_Animate( INT32 idx, INT32 EventSender, UINT16 nHP, REAL32 rKeyFrame = 0.0f);

	/** \brief Outpost ���� object ������ �����մϴ�.
		\note �ִϸ��̼� ������Ʈ�� Ű�����Ӹ� �����մϴ�.
		\param[in] idx network index
		\param[in] nPathKey Path key��
		\param[in] nHP HP
		\param[in] rKeyFrame key��
		\param[in] bSend ���� Send ����
				bSend�� false�̸� sync time���� ������ ������ �˴ϴ�.*/
	void	Write_Object_AnimateForOutpost( INT32 idx, INT32 EventSender, UINT8 nPathKey, UINT16 nHP, REAL32 rKeyFrame, bool bSend);
	
	// ������ ����ź�� ��Ʈ��ũ ���ۿ� ����մϴ�.
	void	BindNetGrenadeInfo( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos);

	// ������ C5�� ��Ʈ��ũ ���ۿ� ����մϴ�.
	void	BindNetC5Info( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos);

	/** \��ġ�ٿ��� ����� �˸��ϴ�.
		\��ġ�ٿ� ������Ʈ�� �浹���� ��� �����ϴ�.
		\param[in] pChara			ĳ���� ����
		\param[in] ui16ObjectIdx	�浹�� ��ġ�ٿ� ������Ʈ �ε��� */
	void	Write_Chara_TouchDown( CGameCharaBase * pChara, UINT16 ui16ObjectIdx );

	// Server - Client ���� ����� Ű���� ��� �ɴϴ�.
	INT64	GetSecretKey( UINT64 ui64MacAddress );
};

#endif
